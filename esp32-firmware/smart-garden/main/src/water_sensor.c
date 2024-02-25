#include "esp_adc_cal.h"
#include "driver/adc.h"
#include "water_sensor.h"
#include "esp_log.h"

static const char *TAG = "ADC";

void init_water_sensors()
{
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(SOIL_SENSOR, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(WATER_TANK_SENSOR, ADC_ATTEN_DB_11);
}

// Multi sample and average to return a sample
uint16_t get_sample(adc1_channel_t channel, uint16_t avg_num_samples)
{
    uint32_t adc_reading = 0;
    for(int i = 0; i < avg_num_samples; i++)
    {
        adc_reading += adc1_get_raw(channel);
    }
    adc_reading /= avg_num_samples;
    return adc_reading;
}