#include "temperature.h"
#include "string.h"
#include "esp_log.h"
#define PIN_NUM_MISO 12
#define CS_PIN 15
#define PIN_NUM_SCLK 14

#define SPI_TEMP_FREQ 4000000

static const char *TAG = "SPI";

spi_device_handle_t init_temp_sensor()
{
    spi_device_handle_t spi_device;
    spi_bus_config_t spi_config = 
    {
        .mosi_io_num = -1, // Not Used
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    spi_device_interface_config_t dev_cfg = 
    {
        .clock_speed_hz = SPI_TEMP_FREQ,
        .mode = 0,
        .spics_io_num = CS_PIN,
        .queue_size = 3,
    };
    esp_err_t ret;
    ret = spi_bus_initialize(SPI2_HOST, &spi_config, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);
    ret = spi_bus_add_device(SPI2_HOST, &dev_cfg, &spi_device);
    ESP_ERROR_CHECK(ret);
    return spi_device;
}

float read_temperature(spi_device_handle_t spi_device)
{
    float temp_c = 0;
    uint16_t temp = 0;
    uint8_t rx_buffer[2];
    memset(rx_buffer,0,sizeof(rx_buffer));
    spi_transaction_t trans = 
    {
        .rx_buffer = &rx_buffer,
        .length = 16,
        .rxlength = 16,
        .tx_buffer = NULL
    };
    spi_device_acquire_bus(spi_device, portMAX_DELAY);
    ESP_ERROR_CHECK(spi_device_transmit(spi_device, &trans));
    spi_device_release_bus(spi_device);
    temp = (rx_buffer[0] << 8) | rx_buffer[1];
    temp = (temp & 0x7FFF) >> 3;
    temp_c = ((float)temp)*0.25f;
    return temp_c;
}