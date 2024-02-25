#include "sensor_data.h"

/**
 * Thread safe getter/setters for accessing data between server thread and monitor thread.
*/

typedef struct
{
    uint16_t tank_level;
    uint16_t soil_level;
    float    temperature;
}sensor_data;

static sensor_data data = {
    .soil_level = 0,
    .tank_level = 0,
    .temperature = 0
};

SemaphoreHandle_t create_data_mutex()
{
    return xSemaphoreCreateRecursiveMutex();
}

uint16_t get_tank_level(SemaphoreHandle_t handle)
{
    uint16_t val = 0;
    xSemaphoreTakeRecursive(handle, portMAX_DELAY);
    val = data.tank_level;
    xSemaphoreGiveRecursive(handle);
    return val;
}

uint16_t get_soil_level(SemaphoreHandle_t handle)
{
    uint16_t val = 0;
    xSemaphoreTakeRecursive(handle, portMAX_DELAY);
    val = data.soil_level;
    xSemaphoreGiveRecursive(handle);
    return val;
}

float get_temperature(SemaphoreHandle_t handle)
{
    float val = 0;
    xSemaphoreTakeRecursive(handle, portMAX_DELAY);
    val = data.temperature;
    xSemaphoreGiveRecursive(handle);
    return val;
}

void set_tank_level(uint16_t value, SemaphoreHandle_t handle)
{
    xSemaphoreTakeRecursive(handle, portMAX_DELAY);
    data.tank_level = value;
    xSemaphoreGiveRecursive(handle);
}

void set_soil_level(uint16_t value, SemaphoreHandle_t handle)
{
    xSemaphoreTakeRecursive(handle, portMAX_DELAY);
    data.soil_level = value;
    xSemaphoreGiveRecursive(handle);
}

void set_temperature(float value, SemaphoreHandle_t handle)
{
    xSemaphoreTakeRecursive(handle, portMAX_DELAY);
    data.temperature = value;
    xSemaphoreGiveRecursive(handle);
}