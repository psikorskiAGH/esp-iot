#include "server.h"

#define SERVER_TAG "server"

namespace server
{
    httpd_handle_t server_handle = NULL;

    bool start()
    {
        ESP_LOGI(SERVER_TAG, "Starting HTTP server");

        // Allow wildcard matchings
        config.uri_match_fn = httpd_uri_match_wildcard;

        /* Start the httpd server */
        if (httpd_start(&server_handle, &config) == ESP_OK)
        {
            /* Register URI handlers */
            httpd_register_uri_handler(server_handle, &ws);
            httpd_register_uri_handler(server_handle, &uri_get);
            httpd_register_uri_handler(server_handle, &uri_post);
        }
        /* If server failed to start, handle will be NULL */
        return server_handle == nullptr;
    }

    /* === RootPath === */

    api::Response *RootPath::GET()
    {
        api::ResponseBody *root = API_MACRO_CREATE_BODY;
        auto alloc = root->GetAllocator();
        root->AddMember(std::move("greetings"), std::move("Hello! You have successfully conected to the API."), alloc);
        return new api::ResponseOK(root);
    }

    RootPath *root_path = new RootPath();

    /* === Custom resource handling === */

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

    api::Response *get_response(const char *uri)
    {
        api::Path *path = find_path_element(uri);
        api::Response *response;
#ifdef DEBUG_L2
        if (path == nullptr)
            ESP_LOGI(SERVER_TAG, "Path found");
        else
            ESP_LOGI(SERVER_TAG, "Path missing");
#endif
        if (path == nullptr)
        {
            response = new api::NotFoundError("Resource '" + std::string(uri) + "' does not exist.");
        }
        else
        {
            response = path->GET();
        }
        return response;
    }
    api::Response *post_response(const char *uri, rapidjson::Value const &post_data)
    {
        api::Path *path = find_path_element(uri);
        api::Response *response;
#ifdef DEBUG_L2
        if (path == nullptr)
            ESP_LOGI(SERVER_TAG, "Path found");
        else
            ESP_LOGI(SERVER_TAG, "Path missing");
#endif
        if (path == nullptr)
        {
            response = new api::NotFoundError("Resource '" + std::string(uri) + "' does not exist.");
        }
        else
        {
            response = path->POST(post_data);
        }
        return response;
    }

    /* === HTTP handling === */

    /// @brief
    /// @param req
    /// @param response (takes ownership)
    static void http_send_response(httpd_req_t *req, api::Response *response)
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

    esp_err_t http_get_handler(httpd_req_t *req)
    {
#ifdef DEBUG_L1
        ESP_LOGI(SERVER_TAG, "Handling HTTP GET request to '%s'", req->uri);
#endif
        http_send_response(req, get_response(req->uri));
        return ESP_OK;
    };

    esp_err_t http_post_handler(httpd_req_t *req)
    {
#ifdef DEBUG_L1
        ESP_LOGI(SERVER_TAG, "Handling HTTP POST request to '%s'", req->uri);
#endif
        HttpDataStream stream(req);
        rapidjson::Document doc;
        doc.ParseStream(stream);
        http_send_response(req, post_response(req->uri, doc));
        return ESP_OK;
    };

    /* === WSS handling === */

    /// @brief
    /// @param req
    /// @param buff_pointer May be nullptr if received data was empty. Remember to call free() on that buffer.
    /// @return
    static esp_err_t wss_read_frame(httpd_req_t *req, char **buff_pointer)
    {
        httpd_ws_frame_t ws_pkt = {
            .final = false,
            .fragmented = false,
            .type = HTTPD_WS_TYPE_TEXT,
            .payload = NULL,
            .len = 0,
        };
        char *buf = NULL;
        // ws_pkt.type = HTTPD_WS_TYPE_TEXT;
        esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
        if (ret != ESP_OK)
        {
            ESP_LOGE(SERVER_TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
            return ret;
        }
#ifdef DEBUG_L2
        ESP_LOGI(SERVER_TAG, "WSS frame len: %d", ws_pkt.len);
#endif
        if (ws_pkt.len)
        {
            /* ws_pkt.len + 1 is for NULL termination as we are expecting a string */
            buf = (char *)calloc(1, ws_pkt.len + 1);
            if (buf == NULL)
            {
                ESP_LOGE(SERVER_TAG, "Failed to calloc memory for buf");
                return ESP_ERR_NO_MEM;
            }
            ws_pkt.payload = (uint8_t *)buf;
            /* Set max_len = ws_pkt.len to get the frame payload */
            ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
            if (ret != ESP_OK)
            {
                ESP_LOGE(SERVER_TAG, "httpd_ws_recv_frame failed with %d", ret);
                free(buf);
                return ret;
            }
            ESP_LOGI(SERVER_TAG, "Got packet with message: %s", ws_pkt.payload);
            (*buff_pointer) = buf;
        }
        else
        {
            (*buff_pointer) = nullptr;
        }
        return ESP_OK;
    }

    /// @brief
    /// @param req
    /// @param response (takes ownership)
    static esp_err_t wss_send_response(httpd_req_t *req, api::Response *response)
    {
        // Takes ownership of response

        rapidjson::Document data(rapidjson::kObjectType);
        RAPIDJSON_DEFAULT_ALLOCATOR alloc = data.GetAllocator();

        data.AddMember(std::move("status"), response->get_status_number(), alloc);
        data.AddMember(std::move("body"), *response->get_body(), alloc);

        rapidjson::StringBuffer resp_buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(resp_buffer);
        data.Accept(writer);

        httpd_ws_frame_t ws_pkt = {
            .final = false,
            .fragmented = false,
            .type = HTTPD_WS_TYPE_TEXT,
            .payload = (uint8_t *)resp_buffer.GetString(),
            .len = resp_buffer.GetSize(),
        };

        esp_err_t ret = httpd_ws_send_frame(req, &ws_pkt);
        if (ret != ESP_OK)
        {
            ESP_LOGE(SERVER_TAG, "httpd_ws_send_frame failed with %d", ret);
            return ret;
        }

        delete response; // Finally free up response
        return ESP_OK;
    }

    enum HttpMethod
    {
        GET = 0,
        POST = 1
    };

    esp_err_t wss_handler(httpd_req_t *req)
    {
        // Based on https://github.com/espressif/esp-idf/blob/master/examples/protocols/http_server/ws_echo_server/main/ws_echo_server.c
        if (req->method == HTTP_GET)
        {
#ifdef DEBUG_L1
            ESP_LOGI(SERVER_TAG, "Opening WebSocket connection");
#endif
            return ESP_OK;
        }
        esp_err_t err;
        char *buf = nullptr;
        err = wss_read_frame(req, &buf);
        if (err != ESP_OK)
        {
            return err;
        }

        if (buf == nullptr)
        {
            return wss_send_response(req, new api::BadRequestError("Empty payload."));
        }

        // Parse JSON
        rapidjson::Document data;
        RAPIDJSON_DEFAULT_ALLOCATOR alloc = data.GetAllocator();
        rapidjson::StringStream stream(buf);
        data.ParseStream(stream);
        free(buf);

        api::Response *response;
        if (!data.IsObject())
        {
            return wss_send_response(req, new api::BadRequestError("Payload is not JSON Object."));
        }
        auto end = data.MemberEnd();

        // URI
        auto it_uri = data.FindMember("method");
        if (it_uri == end)
        {
            return wss_send_response(req, new api::BadRequestError("Missing required field 'uri' in request."));
        }
        if (!it_uri->value.IsString())
        {
            return wss_send_response(req, new api::BadRequestError("Field 'uri' has incorrect type."));
        }
        const char * uri = it_uri->value.GetString();
        if (uri[0] != '/')
        {
            return wss_send_response(req, new api::BadRequestError("URI must start with slash ('/')."));
        }
        api::Path *path = find_path_element(uri);
        if (path == nullptr) {
            return wss_send_response(req, new api::NotFoundError(string_format("Resource '%s', does not exist.", uri)));
        }

        // Method
        auto it_method = data.FindMember("method");
        if (it_method == end)
        {
            return wss_send_response(req, new api::BadRequestError("Missing required field 'method' in request."));
        }
        if (!it_method->value.IsString())
        {
            return wss_send_response(req, new api::BadRequestError("Field 'method' has incorrect type."));
        }
        const char *method = it_method->value.GetString();
        if (strcmp(method, "GET"))
        {
            response = path->GET();
        }
        else if (strcmp(method, "POST"))
        {
            // POST data
            auto it_body = data.FindMember("body");
            if (it_body == end)
            {
                return wss_send_response(req, new api::BadRequestError("Missing required field 'body' in POST request."));
            }
            // Don't care about type
            response = path->POST(it_body->value);
        }
        else
        {
            return wss_send_response(req, new api::BadRequestError(string_format("Incorrect method '%s', must be one of 'GET', 'POST' (case sensitive).", method)));
        }

        return wss_send_response(req, response);
    }

} // namespace server
