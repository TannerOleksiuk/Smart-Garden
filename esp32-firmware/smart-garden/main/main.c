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
#include "water_sensor.h"
#include "temperature.h"

static const char* TAG = "MAIN";
static void monitor_sensors();

static spi_device_handle_t spi_device;

void app_main(void)
{
  // Initialize the Camera
  camera_init();
  // Init GPIO
  gpio_init();
  init_water_sensors();
  spi_device = init_temp_sensor();
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
  xTaskCreate(monitor_sensors, "monitor_sensors", 2048, NULL, 4, NULL);
  //Server
  httpd_handle_t http_handle;
  http_handle = start_webserver();
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &http_handle));
  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &http_handle));
}

static void monitor_sensors()
{
  while(true)
  {
    uint16_t tank = 0;
    uint16_t soil = 0;
    float temp_c = 0;
    tank = get_sample(WATER_TANK_SENSOR, 64);
    soil = get_sample(SOIL_SENSOR, 64);
    temp_c = read_temperature(spi_device);
    ESP_LOGI(TAG, "TANK LEVEL SENSOR: %u", tank);
    ESP_LOGI(TAG, "SOIL LEVEL SENSOR: %u", soil);
    ESP_LOGI(TAG, "TEMPERATURE: %f C", temp_c);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}
