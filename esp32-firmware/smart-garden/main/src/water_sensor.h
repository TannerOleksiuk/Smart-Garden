#ifndef WATER_SENS_H
#define WATER_SENS_H
#include <stdint.h>
#include "driver/adc.h"

#define SOIL_SENSOR ADC_CHANNEL_5
#define WATER_TANK_SENSOR ADC_CHANNEL_4

void init_water_sensors();
uint16_t get_sample(adc1_channel_t channel, uint16_t avg_num_samples);
#endif