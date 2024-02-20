#ifndef HTTP_H
#define HTTP_H
#include "esp_err.h"
#include "esp_http_server.h"
esp_err_t post_handler(httpd_req_t *req);
esp_err_t get_handler(httpd_req_t *req);
esp_err_t post_handler(httpd_req_t *req);
httpd_handle_t start_webserver(void);
void stop_webserver(httpd_handle_t server);

#endif