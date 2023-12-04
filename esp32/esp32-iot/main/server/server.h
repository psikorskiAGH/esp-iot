// api/path.h
#ifndef SERVER_H // include guard
#define SERVER_H

#include "../global/includes.h"
#include "../api/path.h"
#include "http_data_stream.h"

namespace server
{
    class RootPath : public api::Path
    {
    public:
        inline RootPath() : Path(""){};
        api::Response *GET();
    };
    extern RootPath *root_path;

    /// @brief Finds path element for given URI.
    /// @param uri URI (path) to search for
    /// @return pointer to found Path, nullptr if not found (no ownership transfered)
    extern api::Path *find_path_element(const char *uri);

    extern esp_err_t get_handler(httpd_req_t *req);
    extern esp_err_t post_handler(httpd_req_t *req);

    /* Empty handle to esp_http_server */
    extern httpd_handle_t server_handle;
    /* Generate default configuration */
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_uri_t uri_get{
        .uri = "*",
        .method = HTTP_GET,
        .handler = server::get_handler,
        .user_ctx = NULL};
    httpd_uri_t uri_post{
        .uri = "*",
        .method = HTTP_POST,
        .handler = server::post_handler,
        .user_ctx = NULL};

    /* Function for starting the webserver */
    extern bool start();
}

#endif