#include "app_http_server.h"
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <sys/param.h>
#include "esp_netif.h"
#include <esp_http_server.h>
#include <string.h>

/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 */
extern const uint8_t index_html_start[] asm("_binary_webserver_html_start");
extern const uint8_t index_html_end[] asm("_binary_webserver_html_end");

static const char *TAGG = "HTTPPP";
static httpd_handle_t server = NULL;

static http_post_handle_t http_post_cb = NULL;

/* An HTTP GET handler */
static esp_err_t http_get_handler(httpd_req_t *req)
{
    // char*  buf;
    // // size_t buf_len;

    /* Send response with custom headers and body set as the
     * string passed in user context*/
    httpd_resp_set_type(req, "text/html");
    const char* resp_str = (const char*) index_html_start;
    httpd_resp_send(req, resp_str, index_html_end - index_html_start);
    return ESP_OK;
}

static const httpd_uri_t http_get = {
    .uri       = "/get",
    .method    = HTTP_GET,
    .handler   = http_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = "Hello World!"
};

/* An HTTP POST handler */
static esp_err_t http_post_handler(httpd_req_t *req)
{
    char buf[100];
    int  data_len = req->content_len;

        /* Read the data for the request */
        httpd_req_recv(req, buf,data_len);
        /* Log data received */
        ESP_LOGI(TAGG, "Data recv: %.*s", data_len, buf);
        http_post_cb(buf, data_len);
    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t http_post = {
    .uri       = "/post",
    .method    = HTTP_POST,
    .handler   = http_post_handler,
    .user_ctx  = NULL
};

/* This handler allows the custom error handling functionality to be
 * tested from client side. For that, when a PUT request 0 is sent to
 * URI /ctrl, the /hello and /echo URIs are unregistered and following
 * custom error handler http_404_error_handler() is registered.
 * Afterwards, when /hello or /echo is requested, this custom error
 * handler is invoked which, after sending an error message to client,
 * either closes the underlying socket (when requested URI is /echo)
 * or keeps it open (when requested URI is /hello). This allows the
 * client to infer if the custom error handler is functioning as expected
 * by observing the socket state.
 */
// esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
// {
//     if (strcmp("/hello", req->uri) == 0) {
//         httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/hello URI is not available");
//         /* Return ESP_OK to keep underlying socket open */
//         return ESP_OK;
//     } else if (strcmp("/echo", req->uri) == 0) {
//         httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/echo URI is not available");
//         /* Return ESP_FAIL to close underlying socket */
//         return ESP_FAIL;
//     }
//     /* For any other URI send 404 and close socket */
//     httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
//     return ESP_FAIL;
// }

void start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAGG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAGG, "Registering URI handlers");
        httpd_register_uri_handler(server, &http_get);
        httpd_register_uri_handler(server, &http_post);
    }
}

void stop_webserver(void)
{
    httpd_stop(server);
}

void http_post_set_callback(void *cb)
{
    if(cb){
        http_post_cb = cb;
    }
}