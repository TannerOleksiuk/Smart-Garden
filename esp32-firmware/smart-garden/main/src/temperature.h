#ifndef TEMP_H
#define TEMP_H
#include "driver/spi_master.h"
#include "driver/spi_common.h"
spi_device_handle_t init_temp_sensor();
float read_temperature(spi_device_handle_t spi_device);
#endif