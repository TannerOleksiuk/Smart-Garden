#include <stdio.h>
#include "esp_log.h"
#include "camera.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"
#include "wifi.h"
#include "http.h"

static const char* TAG = "MAIN";

void app_main(void)
{
    // Initialize the Camera
    camera_init();

    // WiFi
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();

    // Server
    httpd_handle_t http_handle;
    http_handle = start_webserver();
}
