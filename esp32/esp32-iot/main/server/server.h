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
    extern api::Response *get_response(const char *uri);
    extern api::Response *post_response(const char *uri, rapidjson::Value const &post_data);

    extern esp_err_t http_get_handler(httpd_req_t *req);
    extern esp_err_t http_post_handler(httpd_req_t *req);
    extern esp_err_t wss_handler(httpd_req_t *req);

    /* Empty handle to esp_http_server */
    extern httpd_handle_t server_handle;
    /* Generate default configuration */
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    static const httpd_uri_t uri_get{
        .uri = "*",
        .method = HTTP_GET,
        .handler = server::http_get_handler,
        .user_ctx = NULL,
        .is_websocket = false,
        .handle_ws_control_frames = true,
        .supported_subprotocol = NULL};
    static const httpd_uri_t uri_post{
        .uri = "*",
        .method = HTTP_POST,
        .handler = server::http_post_handler,
        .user_ctx = NULL,
        .is_websocket = false,
        .handle_ws_control_frames = true,
        .supported_subprotocol = NULL};
    static const httpd_uri_t ws = {
        .uri = "/websocket",
        .method = HTTP_GET,
        .handler = server::wss_handler,
        .user_ctx = NULL,
        .is_websocket = true,
        .handle_ws_control_frames = true,
        .supported_subprotocol = NULL};

    /* Function for starting the webserver */
    extern bool start();
}

#endif