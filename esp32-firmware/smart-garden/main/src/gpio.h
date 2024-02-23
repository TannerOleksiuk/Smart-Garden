#ifndef RELAY_GPIO_H
#define RELAY_GPIO_H
void gpio_init();
void actuate_pump();
esp_err_t toggle_pump();
esp_err_t toggle_lights();
void actuate_lights();
void release_pump();
void release_lights();
#endif