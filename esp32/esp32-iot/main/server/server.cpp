#include "server.h"

namespace server
{
    httpd_handle_t server_handle = NULL;

    /* === RootPath === */

    api::Response *RootPath::GET()
    {
        api::ResponseBody *root = API_MACRO_CREATE_BODY;
        auto alloc = root->GetAllocator();
        root->AddMember(std::move("greetings"), std::move("Hello! You have successfully conected to the API."), alloc);
        return new api::ResponseOK(root);
    }

    RootPath *root_path = new RootPath();

    api::Path *find_path_element(const char *uri)
    {
        api::Path *curr_path = root_path;
        size_t uri_i = 1; // At index 0 there is always '/', skip

        char part[256] = {};
        size_t part_i = 0;
        while (true)
        {
            char c = uri[uri_i];
            if (c == '/' || c == '\0' || c == '?')
            {
                // Found uri path end
                if (part_i > 0)
                {
                    // Part is valid
                    part[part_i] = '\0';
                    curr_path = curr_path->get_child_path(part);
                    if (curr_path == nullptr)
                    {
                        // Path not found
                        return nullptr;
                    }
                    part_i = 0;
                    uri_i++;
                    continue;
                }
                else if (c == '/')
                {
                    // Two slashes in a row
                    return nullptr;
                }
                else
                { // c == '\0' || c == '?'
                    // URI finished after slash, that's OK
                    break;
                }
            }
            else
            {
                // Other char, just append
                part[part_i++] = c;
                uri_i++;
            }
        }
        return curr_path;
    }

    static void send_response(httpd_req_t *req, api::Response *response)
    {
        // Takes ownership of response

        rapidjson::StringBuffer resp_buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(resp_buffer);
        response->get_body()->Accept(writer);
        httpd_resp_set_type(req, "application/json");
        httpd_resp_set_status(req, response->status.c_str());
        httpd_resp_send(req, resp_buffer.GetString(), resp_buffer.GetSize());

        delete response; // Finally free up response
    }

    esp_err_t get_handler(httpd_req_t *req)
    {
        const char *uri = req->uri;
#ifdef DEBUG
        printf("Handling GET request to '%s'\r\n", uri);
#endif
        api::Path *path = find_path_element(uri);
        api::Response *response;

        if (path == nullptr)
        {
#ifdef DEBUG
            printf("Path missing\r\n");
#endif
            response = new api::NotFoundError("Resource '" + std::string(uri) + "' does not exist.");
        }
        else
        {
#ifdef DEBUG
            printf("Path found\r\n");
#endif
            response = path->GET();
        }
        send_response(req, response);
        return ESP_OK;
    };

    esp_err_t post_handler(httpd_req_t *req)
    {
        const char *uri = req->uri;
#ifdef DEBUG
        printf("Handling POST request to '%s'\r\n", uri);
#endif
        api::Path *path = find_path_element(uri);
        api::Response *response;

        if (path == nullptr)
        {
#ifdef DEBUG
            printf("Path missing\r\n");
#endif
            response = new api::NotFoundError("Resource '" + std::string(uri) + "' does not exist.");
        }
        else
        {
#ifdef DEBUG
            printf("Path found\r\n");
#endif
            HttpDataStream stream(req);
            // rapidjson::Document doc;
            // doc.ParseStream(stream);
            // response = path->POST(doc);
            
            rapidjson::Document *doc = new rapidjson::Document();
            doc->ParseStream(stream);
            response = new api::ResponseOK(doc);
        }
        send_response(req, response);
        return ESP_OK;
    };

    bool start()
    {
        printf("Starting HTTP server...\n");

        // Allow wildcard matchings
        config.uri_match_fn = httpd_uri_match_wildcard;

        /* Start the httpd server */
        if (httpd_start(&server_handle, &config) == ESP_OK)
        {
            /* Register URI handlers */
            httpd_register_uri_handler(server_handle, &uri_get);
            httpd_register_uri_handler(server_handle, &uri_post);
        }
        /* If server failed to start, handle will be NULL */
        return server_handle == nullptr;
    }

} // namespace server
