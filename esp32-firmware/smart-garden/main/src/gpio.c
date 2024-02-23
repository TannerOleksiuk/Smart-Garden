#include "driver/gpio.h"
#include "esp_log.h"

#define PUMP_RELAY  13
#define LIGHTS_RELAY 0
#define OUTPUT_IO_PIN_SEL ((1ULL<<PUMP_RELAY) | (1ULL<<LIGHTS_RELAY))

#define RELAY_ON    0
#define RELAY_OFF   1

static const char *TAG = "GPIO";

void gpio_init()
{
    gpio_config_t io_cfg = {};
    io_cfg.intr_type = GPIO_INTR_DISABLE;
    io_cfg.mode = GPIO_MODE_INPUT_OUTPUT;
    io_cfg.pin_bit_mask = OUTPUT_IO_PIN_SEL;
    io_cfg.pull_down_en = 0;
    io_cfg.pull_up_en = 1;
    gpio_config(&io_cfg);
}

void actuate_pump()
{
    if(gpio_set_level(PUMP_RELAY, 0) == ESP_OK)
    {
        ESP_LOGI(TAG, "PUMP ACTUATED");
    }
}

esp_err_t toggle_pump()
{
    uint8_t relay_state = gpio_get_level(PUMP_RELAY);
    ESP_LOGI(TAG, "RELAY STATE: %u", relay_state);
    if(relay_state == RELAY_OFF)
    {
        if(gpio_set_level(PUMP_RELAY, 0) == ESP_OK)
        {
            ESP_LOGI(TAG, "PUMP ACTUATED");
        }
    }
    else
    {
        if(gpio_set_level(PUMP_RELAY, 1) == ESP_OK)
        {
            ESP_LOGI(TAG, "PUMP RELEASED");
        }
    }
    return ESP_OK;
}

void actuate_lights()
{
    if(gpio_set_level(LIGHTS_RELAY, 0) == ESP_OK)
    {
        ESP_LOGI(TAG, "LIGHTS ACTUATED");
    }
}

esp_err_t toggle_lights()
{
    uint8_t relay_state = gpio_get_level(LIGHTS_RELAY);
    ESP_LOGI(TAG, "RELAY STATE: %u", relay_state);
    if(relay_state == RELAY_OFF)
    {
        if(gpio_set_level(LIGHTS_RELAY, 0) == ESP_OK)
        {
            ESP_LOGI(TAG, "LIGHTS ACTUATED");
        }
    }
    else
    {
        if(gpio_set_level(LIGHTS_RELAY, 1) == ESP_OK)
        {
            ESP_LOGI(TAG, "LIGHTS RELEASED");
        }
    }
    return ESP_OK;
}

void release_pump()
{
    if(gpio_set_level(PUMP_RELAY, 1) == ESP_OK)
    {
        ESP_LOGI(TAG, "PUMP RELEASED");
    }
}

void release_lights()
{
    if(gpio_set_level(LIGHTS_RELAY, 1) == ESP_OK)
    {
        ESP_LOGI(TAG, "LIGHTS RELEASED");
    }
}