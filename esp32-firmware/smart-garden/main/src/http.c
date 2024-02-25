
#include "http.h"
#include <stdbool.h>
#include <esp_event.h>
#include <esp_log.h>
#include "camera.h"
#include "sensor_data.h"
#include "gpio.h"
/* Our URI handler function to be called during GET /uri request */
/*esp_err_t get_handler(httpd_req_t *req)
{
    // Send a simple response 
    const char resp[] = "URI GET Response";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}*/

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

static const char *TAG = "WEBSERVER";

static SemaphoreHandle_t handle;

/*
 * Structure holding server handle
 * and internal socket fd in order
 * to use out of request send
 */
struct async_resp_arg {
    httpd_handle_t hd;
    int fd;
};

static esp_err_t toggle_pump_handler(httpd_req_t *req)
{
    toggle_pump();
    return httpd_resp_send(req, "ACK", 4);
}

static esp_err_t toggle_lights_handler(httpd_req_t *req)
{
    toggle_lights();
    return httpd_resp_send(req, "ACK", 4);
}

static esp_err_t tank_handler(httpd_req_t *req)
{
    uint16_t tank = get_tank_level(handle);
    char resp[6];
    memset(resp, 0, sizeof(resp));
    sprintf(resp, "%u", tank);
    return httpd_resp_send(req, resp, sizeof(resp));
}

static esp_err_t soil_handler(httpd_req_t *req)
{
    uint16_t soil = get_soil_level(handle);
    char resp[6];
    memset(resp, 0, sizeof(resp));
    sprintf(resp, "%u", soil);
    return httpd_resp_send(req, resp, sizeof(resp));
}

static esp_err_t temperature_handler(httpd_req_t *req)
{
    float temp = get_temperature(handle);
    char resp[8];
    memset(resp, 0, sizeof(resp));
    if(temp < 1025)
    {
        sprintf(resp, "%7.2f", temp);
    }
    return httpd_resp_send(req, resp, sizeof(resp));
}

/*
 * async send function, which we put into the httpd work queue
 */
static void ws_async_send(void *arg)
{
    static const char * data = "Async data";
    struct async_resp_arg *resp_arg = arg;
    httpd_handle_t hd = resp_arg->hd;
    int fd = resp_arg->fd;
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = (uint8_t*)data;
    ws_pkt.len = strlen(data);
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;

    httpd_ws_send_frame_async(hd, fd, &ws_pkt);
    free(resp_arg);
}

static esp_err_t trigger_async_send(httpd_handle_t handle, httpd_req_t *req)
{
    struct async_resp_arg *resp_arg = malloc(sizeof(struct async_resp_arg));
    resp_arg->hd = req->handle;
    resp_arg->fd = httpd_req_to_sockfd(req);
    return httpd_queue_work(handle, ws_async_send, resp_arg);
}


/*
 * This handler echos back the received ws data
 * and triggers an async send if certain message received
 */
static esp_err_t ws_handler(httpd_req_t *req)
{
    if (req->method == HTTP_GET) {
        ESP_LOGI(TAG, "Handshake done, the new connection was opened");
        return ESP_OK;
    }
    httpd_ws_frame_t ws_pkt;
    uint8_t *buf = NULL;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
    /* Set max_len = 0 to get the frame len */
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
        return ret;
    }
    ESP_LOGI(TAG, "frame len is %d", ws_pkt.len);
    if (ws_pkt.len) {
        /* ws_pkt.len + 1 is for NULL termination as we are expecting a string */
        buf = calloc(1, ws_pkt.len + 1);
        if (buf == NULL) {
            ESP_LOGE(TAG, "Failed to calloc memory for buf");
            return ESP_ERR_NO_MEM;
        }
        ws_pkt.payload = buf;
        /* Set max_len = ws_pkt.len to get the frame payload */
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
            free(buf);
            return ret;
        }
        ESP_LOGI(TAG, "Got packet with message: %s", ws_pkt.payload);
    }

    ESP_LOGI(TAG, "Packet type: %d", ws_pkt.type);
    if (ws_pkt.type == HTTPD_WS_TYPE_TEXT &&
        strcmp((char*)ws_pkt.payload,"Trigger async") == 0) {
        free(buf);
        return trigger_async_send(req->handle, req);
    }

    if(strcmp((const char*)ws_pkt.payload, "capture") == 0)
    {
        ESP_LOGI(TAG, "RECEIVED CAPTURE REQUEST");
        jpg_ws_handler(req);
    }
    else if(strcmp((const char*)ws_pkt.payload, "stream") == 0)
    {
        ESP_LOGI(TAG, "RECEIVED CAPTURE REQUEST");
        jpg_ws_stream_handler(req);
    }
    else
    {
        ret = httpd_ws_send_frame(req, &ws_pkt);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", ret);
        }
    }
    free(buf);
    return ret;
}

static const httpd_uri_t ws = {
        .uri        = "/ws",
        .method     = HTTP_GET,
        .handler    = ws_handler,
        .user_ctx   = NULL,
        .is_websocket = true
};

/* URI handler structure for GET /uri */
httpd_uri_t uri_get = {
    .uri      = "/image",
    .method   = HTTP_GET,
    .handler  = jpg_httpd_handler,
    .user_ctx = NULL
};

/* URI handler structure for GET /uri */
httpd_uri_t uri_stream = {
    .uri      = "/stream",
    .method   = HTTP_GET,
    .handler  = jpg_stream_httpd_handler,
    .user_ctx = NULL
};

/* URI handler structure for relay toggle /uri */
httpd_uri_t pump_toggle = {
    .uri      = "/pump",
    .method   = HTTP_GET,
    .handler  = toggle_pump_handler,
    .user_ctx = NULL
};

/* URI handler structure for relay toggle /uri */
httpd_uri_t lights_toggle = {
    .uri      = "/lights",
    .method   = HTTP_GET,
    .handler  = toggle_lights_handler,
    .user_ctx = NULL
};

/* URI handler structure for relay toggle /uri */
httpd_uri_t get_tank = {
    .uri      = "/tank",
    .method   = HTTP_GET,
    .handler  = tank_handler,
    .user_ctx = NULL
};

/* URI handler structure for relay toggle /uri */
httpd_uri_t get_soil = {
    .uri      = "/soil",
    .method   = HTTP_GET,
    .handler  = soil_handler,
    .user_ctx = NULL
};

/* URI handler structure for relay toggle /uri */
httpd_uri_t get_temp = {
    .uri      = "/temperature",
    .method   = HTTP_GET,
    .handler  = temperature_handler,
    .user_ctx = NULL
};


/* Function for starting the webserver */
httpd_handle_t start_webserver(void)
{
    /* Generate default configuration */
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    handle = create_data_mutex();
    /* Empty handle to esp_http_server */
    httpd_handle_t server = NULL;

    /* Start the httpd server */
    if (httpd_start(&server, &config) == ESP_OK) {
        /* Register URI handlers */
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_stream);
        httpd_register_uri_handler(server, &pump_toggle);
        httpd_register_uri_handler(server, &lights_toggle);
        httpd_register_uri_handler(server, &get_temp);
        httpd_register_uri_handler(server, &get_soil);
        httpd_register_uri_handler(server, &get_tank);
        // WebSocket Server
        httpd_register_uri_handler(server, &ws);
    }
    /* If server failed to start, handle will be NULL */
    return server;
}

/* Function for stopping the webserver */
void stop_webserver(httpd_handle_t server)
{
    if (server) {
        /* Stop the httpd server */
        httpd_stop(server);
    }
}

void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
}

void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}