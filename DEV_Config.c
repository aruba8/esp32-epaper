/*****************************************************************************
 * | File        :   DEV_Config.c
 * | Author      :
 * | Function    :   Hardware underlying interface
 * | Info        :
 *----------------
 * |	This version:   V1.0
 * | Date        :
 * | Info        :   ESP32-C6 implementation
 *
 ******************************************************************************/
#include "DEV_Config.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_rom_sys.h"

static const char *TAG = "DEV";

static spi_device_handle_t spi_handle = NULL;

/**
 * GPIO
 **/
int EPD_RST_PIN;
int EPD_DC_PIN;
int EPD_CS_PIN;
int EPD_BUSY_PIN;
int EPD_CLK_PIN;
int EPD_MOSI_PIN;

/**
 * GPIO read and write
 **/
void DEV_Digital_Write(UWORD Pin, UBYTE Value)
{
    gpio_set_level((gpio_num_t)Pin, Value);
}

UBYTE DEV_Digital_Read(UWORD Pin)
{
    return gpio_get_level((gpio_num_t)Pin);
}

/**
 * SPI
 **/
void DEV_SPI_WriteByte(uint8_t Value)
{
    spi_transaction_t trans = {
        .length = 8,
        .tx_buffer = &Value,
    };
    spi_device_transmit(spi_handle, &trans);
}

void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len)
{
    if (Len == 0)
    {
        return;
    }

    spi_transaction_t trans = {
        .length = Len * 8,
        .tx_buffer = pData,
    };
    spi_device_transmit(spi_handle, &trans);
}

/**
 * delay x ms
 **/
void DEV_Delay_ms(UDOUBLE xms)
{
    vTaskDelay(pdMS_TO_TICKS(xms));
}

/**
 * GPIO Mode
 **/
static void DEV_GPIO_Mode(UWORD Pin, UWORD Mode)
{
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = (1ULL << Pin);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;

    if (Mode == 0)
    { // Input
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    }
    else
    { // Output
        io_conf.mode = GPIO_MODE_OUTPUT;
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    }

    gpio_config(&io_conf);
}

static void DEV_GPIO_Init(void)
{
    EPD_RST_PIN = GPIO_NUM_4;
    EPD_DC_PIN = GPIO_NUM_9;
    EPD_BUSY_PIN = GPIO_NUM_18;
    EPD_CS_PIN = GPIO_NUM_10;
    EPD_CLK_PIN = GPIO_NUM_6;
    EPD_MOSI_PIN = GPIO_NUM_7;

    DEV_GPIO_Mode(EPD_RST_PIN, 1);
    DEV_GPIO_Mode(EPD_DC_PIN, 1);
    DEV_GPIO_Mode(EPD_CS_PIN, 1);
    DEV_GPIO_Mode(EPD_BUSY_PIN, 0);

    DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function:	Module Initialize, the library and initialize the pins, SPI protocol
parameter:
Info:
******************************************************************************/
UBYTE DEV_Module_Init(void)
{
    ESP_LOGI(TAG, "Initializing device module...");

    // GPIO Config
    DEV_GPIO_Init();

    // SPI Config
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = EPD_MOSI_PIN,
        .miso_io_num = -1,
        .sclk_io_num = EPD_CLK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };

    esp_err_t ret = spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "SPI bus init failed");
        return 1;
    }

    spi_device_interface_config_t dev_cfg = {
        .clock_speed_hz = 4 * 1000 * 1000, // 4 MHz
        .mode = 0,
        .spics_io_num = -1, // CS handled manually
        .queue_size = 1,
        .flags = SPI_DEVICE_HALFDUPLEX,
    };

    ret = spi_bus_add_device(SPI2_HOST, &dev_cfg, &spi_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "SPI device add failed");
        spi_bus_free(SPI2_HOST);
        return 1;
    }

    ESP_LOGI(TAG, "Device module initialized");
    return 0;
}

/******************************************************************************
function:	Module exits, closes SPI and BCM2835 library
parameter:
Info:
******************************************************************************/
void DEV_Module_Exit(void)
{
    if (spi_handle)
    {
        spi_bus_remove_device(spi_handle);
        spi_handle = NULL;
    }
    spi_bus_free(SPI2_HOST);
}
