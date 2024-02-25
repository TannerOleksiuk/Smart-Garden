#ifndef SENS_DATA_H
#define SENS_DATA_H
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

SemaphoreHandle_t create_data_mutex();
uint16_t get_tank_level(SemaphoreHandle_t handle);
uint16_t get_soil_level(SemaphoreHandle_t handle);
float get_temperature(SemaphoreHandle_t handle);
void set_tank_level(uint16_t value,SemaphoreHandle_t handle);
void set_soil_level(uint16_t value,SemaphoreHandle_t handle);
void set_temperature(float value,SemaphoreHandle_t handle);
#endif