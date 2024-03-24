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
static httpd_req_t *REG;
static const char *TAG = "HTTP_SERVER";
static httpd_handle_t server = NULL;
static http_post_callback http_post_sw_callback=NULL;
static http_get_callback http_get_dht11_callback=NULL;
static http_post_callback http_post_silder_callback=NULL;
/* An HTTP GET handler */
static esp_err_t http_get_handler(httpd_req_t *req)
{
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

/* An HTTP GET handler */
static esp_err_t dht11_handler(httpd_req_t *req)
{
    REG=req;
    http_get_dht11_callback();
    return ESP_OK;
}
void dht11_respone(char *data,int len){
    httpd_resp_send(REG,data,len);
}
static const httpd_uri_t http_dht11 = {
    .uri       = "/dht11",
    .method    = HTTP_GET,
    .handler   = dht11_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = "Hello World!"
};
/* An HTTP POST handler */
static esp_err_t http_post_handler(httpd_req_t *req)
{
    char buf[100];
        /* Read the data for the request */
        httpd_req_recv(req, buf,req->content_len);
        /* Log data received */
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
/* An HTTP POST handler */
static esp_err_t sw1_handler(httpd_req_t *req)
{
    char buf[100];
        /* Read the data for the request */
        httpd_req_recv(req, buf,req->content_len);
        /* Log data received */
        http_post_sw_callback(buf,req->content_len);
        // End response
         httpd_resp_send_chunk(req, NULL, 0);
        return ESP_OK;
}

static const httpd_uri_t http_sw1 = {
    .uri       = "/sw1",
    .method    = HTTP_POST,
    .handler   = sw1_handler,
    .user_ctx  = NULL
};
/* An HTTP POST handler */
static esp_err_t slider_handler(httpd_req_t *req)
{
    char buf[100];
        /* Read the data for the request */
        httpd_req_recv(req, buf,req->content_len);
        /* Log data received */
        http_post_silder_callback(buf,req->content_len);
        // End response
         httpd_resp_send_chunk(req, NULL, 0);
        return ESP_OK;
}

static const httpd_uri_t http_slider = {
    .uri       = "/slider",
    .method    = HTTP_POST,
    .handler   = slider_handler,
    .user_ctx  = NULL
};

void start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &http_get);
        httpd_register_uri_handler(server, &http_post);
        httpd_register_uri_handler(server, &http_dht11);
        httpd_register_uri_handler(server, &http_sw1);
        httpd_register_uri_handler(server, &http_slider);
    }
}

void stop_webserver(void)
{
    httpd_stop(server);
}
void sw_callback(void *cb){
    http_post_sw_callback=cb;
}
void dht11_callback(void *cb){
    http_get_dht11_callback=cb;
}
void slider_callback(void *cb){
    http_post_silder_callback=cb;
}
