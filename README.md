# ESP32 E-Paper Display Library

A complete e-paper display driver library for ESP32/ESP32-C6 microcontrollers, supporting 2.13" e-paper displays with SSD1680 controller.

## Features

- 📟 Full driver support for 2.13" e-paper display (250x122 pixels)
- 🎨 Graphics library with drawing primitives (lines, rectangles, circles)
- 🔤 Multiple font sizes (Font8, Font12, Font16, Font20, Font24)
- 🔄 Display rotation support (0°, 90°, 180°, 270°)
- ⚡ Partial and full display update modes
- 🎯 Hardware abstraction layer for easy porting
- 📊 Integrated ESP-IDF logging for debugging
- ✅ Compatible with ESP-IDF framework

## Supported Displays

- 2.13" e-Paper displays with SSD1680 controller
- WeActStudio 2.13" e-Paper Module
- Any SSD1680-based 122x250 e-paper display

## Hardware Connections

Default GPIO configuration for ESP32-C6:

| E-Paper Pin | ESP32-C6 GPIO | Description |
|-------------|---------------|-------------|
| RST         | GPIO 4        | Reset       |
| DC          | GPIO 9        | Data/Command|
| BUSY        | GPIO 18       | Busy signal |
| CS          | GPIO 10       | SPI Chip Select |
| CLK         | GPIO 6        | SPI Clock   |
| DIN (MOSI)  | GPIO 7        | SPI MOSI    |

> **Note:** These pins can be customized in `DEV_Config.c`

## Installation

### Method 1: As an ESP-IDF Component (Recommended)

1. Navigate to your ESP-IDF project:
```bash
cd your-esp-idf-project
```

2. Add as a git submodule:
```bash
git submodule add https://github.com/YOUR_USERNAME/esp32-epaper-lib.git components/epaper
```

3. Update your `main/CMakeLists.txt`:
```cmake
idf_component_register(
    SRCS "main.c"
    REQUIRES epaper
)
```

### Method 2: Manual Copy

1. Copy the `epaper` directory to your project's `components/` folder
2. Add `REQUIRES epaper` to your main component's `CMakeLists.txt`

## Quick Start

```c
#include <stdlib.h>
#include "esp_log.h"
#include "DEV_Config.h"
#include "EPD_2in13.h"
#include "GUI_Paint.h"
#include "fonts.h"

#define WIDTHBYTE(width) (((width) % 8 == 0) ? ((width) / 8) : ((width) / 8 + 1))
#define IMAGE_SIZE (WIDTHBYTE(EPD_2IN13_WIDTH) * EPD_2IN13_HEIGHT)

void app_main(void)
{
    // Initialize hardware
    DEV_Module_Init();

    // Allocate image buffer
    UBYTE *image = (UBYTE *)malloc(IMAGE_SIZE);

    // Initialize Paint library (landscape mode)
    Paint_NewImage(image, EPD_2IN13_WIDTH, EPD_2IN13_HEIGHT, ROTATE_90, WHITE);
    Paint_Clear(WHITE);

    // Initialize display
    EPD_2IN13_Init();
    EPD_2IN13_Clear();

    // Draw text
    Paint_DrawString_EN(10, 10, "Hello E-Paper!", &Font16, WHITE, BLACK);

    // Draw shapes
    Paint_DrawRectangle(10, 40, 100, 80, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawCircle(150, 60, 30, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    // Display the image
    EPD_2IN13_Display(image);

    // Sleep mode
    EPD_2IN13_Sleep();

    // Cleanup
    free(image);
    DEV_Module_Exit();
}
```

## API Reference

### Display Control

```c
void EPD_2IN13_Init(void);              // Initialize display
void EPD_2IN13_Clear(void);             // Clear display to white
void EPD_2IN13_Display(UBYTE *Image);   // Full display update
void EPD_2IN13_Display_Partial(UBYTE *Image);  // Partial update (faster)
void EPD_2IN13_Sleep(void);             // Enter sleep mode
```

### Graphics Functions

```c
// Drawing primitives
void Paint_DrawPoint(UWORD X, UWORD Y, UWORD Color, DOT_PIXEL Size, DOT_STYLE Style);
void Paint_DrawLine(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend,
                    UWORD Color, DOT_PIXEL Width, LINE_STYLE Style);
void Paint_DrawRectangle(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend,
                         UWORD Color, DOT_PIXEL Width, DRAW_FILL Fill);
void Paint_DrawCircle(UWORD X, UWORD Y, UWORD Radius,
                      UWORD Color, DOT_PIXEL Width, DRAW_FILL Fill);

// Text rendering
void Paint_DrawString_EN(UWORD X, UWORD Y, const char *String,
                         sFONT* Font, UWORD FgColor, UWORD BgColor);
void Paint_DrawNum(UWORD X, UWORD Y, int32_t Number,
                   sFONT* Font, UWORD FgColor, UWORD BgColor);
```

### Available Fonts

- `Font8` - 5x8 pixels
- `Font12` - 7x12 pixels
- `Font16` - 11x16 pixels
- `Font20` - 14x20 pixels
- `Font24` - 17x24 pixels

### Display Rotation

```c
Paint_NewImage(image, EPD_2IN13_WIDTH, EPD_2IN13_HEIGHT, ROTATE_90, WHITE);
```

Rotation options: `ROTATE_0`, `ROTATE_90`, `ROTATE_180`, `ROTATE_270`

## Hardware Abstraction Layer

To port to different ESP32 variants, modify `DEV_Config.c`:

```c
static void DEV_GPIO_Init(void)
{
    EPD_RST_PIN  = GPIO_NUM_4;   // Your RST pin
    EPD_DC_PIN   = GPIO_NUM_9;   // Your DC pin
    EPD_BUSY_PIN = GPIO_NUM_18;  // Your BUSY pin
    EPD_CS_PIN   = GPIO_NUM_10;  // Your CS pin
    EPD_CLK_PIN  = GPIO_NUM_6;   // Your CLK pin
    EPD_MOSI_PIN = GPIO_NUM_7;   // Your MOSI pin

    // ... configure GPIO
}
```

## Logging and Debugging

The library uses ESP-IDF's logging system with three log tags:

- **DEV**: Hardware initialization and SPI operations
- **EPD**: E-paper display operations (init, clear, sleep, busy states)
- **GUI_PAINT**: Graphics operations and boundary checking

### Controlling Log Levels

```c
// Set log level in your app_main() to control verbosity
esp_log_level_set("EPD", ESP_LOG_INFO);       // Show only info and above
esp_log_level_set("GUI_PAINT", ESP_LOG_WARN); // Show only warnings and errors
esp_log_level_set("DEV", ESP_LOG_DEBUG);      // Show all debug messages
```

Log levels (in order of verbosity):
- `ESP_LOG_NONE` - No logging
- `ESP_LOG_ERROR` - Critical errors only
- `ESP_LOG_WARN` - Warnings (e.g., drawing outside boundaries)
- `ESP_LOG_INFO` - Informational messages (default for important operations)
- `ESP_LOG_DEBUG` - Detailed debug info (e.g., busy state transitions)
- `ESP_LOG_VERBOSE` - Everything

## Performance Tips

1. **Use Partial Updates** for frequently changing content (e.g., counters, time)
2. **Landscape Mode** (ROTATE_90) provides 250x122 resolution vs 122x250 portrait
3. **Buffer Size**: Always use the correct calculation:
   ```c
   #define WIDTHBYTE(w) (((w) % 8 == 0) ? ((w) / 8) : ((w) / 8 + 1))
   #define IMAGE_SIZE (WIDTHBYTE(EPD_2IN13_WIDTH) * EPD_2IN13_HEIGHT)
   ```
4. **Avoid (0,0) coordinates** in filled shapes to prevent underflow bugs
5. **Reduce logging in production**: Set log level to `ESP_LOG_WARN` or higher to improve performance

## Examples

See the `examples/` directory for complete demo applications:
- Basic text and graphics
- Partial update counter
- Pattern generation
- Multi-font display

## Troubleshooting

### Display shows nothing
- Check SPI connections
- Verify BUSY pin is connected (display won't work without it)
- Ensure proper power supply (3.3V)
- Enable debug logging: `esp_log_level_set("EPD", ESP_LOG_DEBUG);` to see initialization steps

### "Exceeding display boundaries" warnings
- These are logged at `ESP_LOG_WARN` level when trying to draw outside the display area
- Avoid drawing at x=0 or y=0 with DOT_PIXEL_1X1
- Check coordinate calculations when using rotation
- Enable logging: `esp_log_level_set("GUI_PAINT", ESP_LOG_WARN);` to see exact coordinates

### Guru Meditation Error on free()
- Ensure IMAGE_SIZE uses the WIDTHBYTE macro
- Don't manually calculate buffer size

### Display stuck in busy state
- Check BUSY pin connection
- Enable debug logging: `esp_log_level_set("EPD", ESP_LOG_DEBUG);` to see "e-Paper busy" messages
- Power cycle the display if it remains stuck

## Credits

Based on e-Paper reference implementations, adapted for ESP-IDF:
- Display documentation: [WeActStudio](https://github.com/WeActStudio/WeActStudio.EpaperModule)

## License

MIT License - See LICENSE file for details

## Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Submit a pull request

## Compatibility

- ✅ ESP32
- ✅ ESP32-S2
- ✅ ESP32-S3
- ✅ ESP32-C3
- ✅ ESP32-C6
- ✅ ESP32-H2

Tested on ESP-IDF v5.0+

## Support

For issues and questions:
- GitHub Issues: https://github.com/YOUR_USERNAME/esp32-epaper-lib/issues
- Documentation: See `docs/` directory
