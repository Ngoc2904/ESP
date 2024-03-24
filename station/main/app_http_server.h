#ifndef __APP_HTTP_SERVER_H
#define __APP_HTTP_SERVER_H
#include "stdio.h"
typedef void (*http_get_callback) (void);
typedef void (*http_post_callback)(char *data, uint16_t len);

void start_webserver(void);
void stop_webserver(void);
void sw_callback(void *cb);
void dht11_callback(void *cb);
void dht11_respone(char *data,int len);
void slider_callback(void *cb);
#endif