#ifndef WEBSOCKET_CLIENT_H
#define WEBSOCKET_CLIENT_H
#include "esp_websocket_client.h"
#include "freertos/event_groups.h"
void shutdown_signaler(TimerHandle_t xTimer);
void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
void websocket_app_start(void);
#endif