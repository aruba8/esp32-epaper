/**
 * @file basic_example.c
 * @brief Basic example for ESP32 E-Paper Display Library
 *
 * This example demonstrates:
 * - Display initialization
 * - Drawing text with different fonts
 * - Drawing basic shapes (rectangles, circles, lines)
 * - Proper cleanup and sleep mode
 */

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "DEV_Config.h"
#include "EPD_2in13.h"
#include "GUI_Paint.h"
#include "fonts.h"

static const char *TAG = "EPD_EXAMPLE";

// Buffer size calculation must match Paint library's WidthByte calculation
#define WIDTHBYTE(width) (((width) % 8 == 0) ? ((width) / 8) : ((width) / 8 + 1))
#define IMAGE_SIZE (WIDTHBYTE(EPD_2IN13_WIDTH) * EPD_2IN13_HEIGHT)

void app_main(void)
{
    ESP_LOGI(TAG, "=== E-Paper Basic Example ===");

    // Configure pins at application level
    epd_pin_config_t pin_config = {
        .rst_pin = GPIO_NUM_4,
        .dc_pin = GPIO_NUM_9,
        .cs_pin = GPIO_NUM_10,
        .busy_pin = GPIO_NUM_18,
        .clk_pin = GPIO_NUM_6,
        .mosi_pin = GPIO_NUM_7
    };

    // Initialize hardware module with pin configuration
    if (DEV_Module_Init(&pin_config) != 0) {
        ESP_LOGE(TAG, "Failed to initialize device module");
        return;
    }

    // Allocate image buffer
    UBYTE *BlackImage = (UBYTE *)malloc(IMAGE_SIZE);
    if (BlackImage == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for image buffer");
        DEV_Module_Exit();
        return;
    }

    ESP_LOGI(TAG, "Image buffer allocated: %d bytes", IMAGE_SIZE);

    // Initialize Paint library in landscape mode (90 degree rotation)
    Paint_NewImage(BlackImage, EPD_2IN13_WIDTH, EPD_2IN13_HEIGHT, ROTATE_90, WHITE);
    Paint_Clear(WHITE);

    // Initialize display
    EPD_2IN13_Init();

    // Clear display
    ESP_LOGI(TAG, "Clearing display...");
    EPD_2IN13_Clear();
    DEV_Delay_ms(2000);

    // Example 1: Display text with different fonts
    ESP_LOGI(TAG, "Drawing text...");
    Paint_Clear(WHITE);
    Paint_DrawString_EN(10, 10, "ESP32 E-Paper Library", &Font16, WHITE, BLACK);
    Paint_DrawString_EN(10, 35, "Font12 - Hello World!", &Font12, WHITE, BLACK);
    Paint_DrawString_EN(10, 55, "Font20 Example", &Font20, WHITE, BLACK);
    EPD_2IN13_Display(BlackImage);
    DEV_Delay_ms(3000);

    // Example 2: Draw shapes
    ESP_LOGI(TAG, "Drawing shapes...");
    Paint_Clear(WHITE);

    // Draw rectangles
    Paint_DrawRectangle(10, 10, 70, 50, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(90, 10, 140, 50, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    // Draw circles
    Paint_DrawCircle(40, 75, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawCircle(110, 75, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    // Draw lines
    Paint_DrawLine(160, 15, 230, 100, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

    EPD_2IN13_Display(BlackImage);
    DEV_Delay_ms(3000);

    // Example 3: Numbers
    ESP_LOGI(TAG, "Drawing numbers...");
    Paint_Clear(WHITE);
    Paint_DrawString_EN(10, 10, "Number Display:", &Font12, WHITE, BLACK);
    Paint_DrawNum(10, 30, 12345, &Font16, WHITE, BLACK);
    Paint_DrawNum(10, 55, -789, &Font16, WHITE, BLACK);
    Paint_DrawNum(10, 80, 2024, &Font16, WHITE, BLACK);
    EPD_2IN13_Display(BlackImage);
    DEV_Delay_ms(3000);

    // Completion message
    ESP_LOGI(TAG, "Displaying completion message...");
    Paint_Clear(WHITE);
    Paint_DrawRectangle(2, 2, 247, 119, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawString_EN(70, 45, "Example Complete!", &Font16, WHITE, BLACK);
    Paint_DrawString_EN(80, 70, "E-Paper Ready!", &Font12, WHITE, BLACK);
    EPD_2IN13_Display(BlackImage);

    ESP_LOGI(TAG, "Demo complete. Entering sleep mode...");
    DEV_Delay_ms(5000);

    // Put display to sleep
    EPD_2IN13_Sleep();

    // Cleanup
    free(BlackImage);
    DEV_Module_Exit();

    ESP_LOGI(TAG, "All done!");
}
