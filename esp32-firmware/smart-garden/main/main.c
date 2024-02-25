#include <stdio.h>
#include "esp_err.h"
#include "esp_log.h"
#include "camera.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"
#include "wifi.h"
#include "http.h"
#include "gpio.h"

static const char* TAG = "MAIN";
static void task_main();

void app_main(void)
{
  // Initialize the Camera
  camera_init();
  // Init GPIO
  gpio_init();
  release_pump();
  release_lights();
  // WiFi
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);
  ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
  wifi_init_sta();
  // Main Task
  //xTaskCreate(task_main, "main", 2048, NULL, 4, NULL);
  //Server
  httpd_handle_t http_handle;
  http_handle = start_webserver();
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &http_handle));
  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &http_handle));
}

static void task_main()
{
  // //Actuate Relays
  // actuate_pump();
  // actuate_lights();
  // vTaskDelay(pdMS_TO_TICKS(5000));
  // //Release
  // release_lights();
  // release_pump();
  // vTaskDelete(NULL);
}
