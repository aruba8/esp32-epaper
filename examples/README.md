# E-Paper Library Examples

This directory contains example applications demonstrating the ESP32 E-Paper Library features.

## Examples

### basic/
Basic example showing:
- Display initialization
- Text rendering with multiple fonts
- Drawing shapes (rectangles, circles, lines)
- Number display
- Display sleep mode

## Running Examples

To use an example in your project:

1. Copy the example code to your `main/main.c`
2. Ensure your `main/CMakeLists.txt` includes:
   ```cmake
   idf_component_register(
       SRCS "main.c"
       REQUIRES epaper
   )
   ```
3. Configure GPIO pins in `components/epaper/DEV_Config.c` if needed
4. Build and flash:
   ```bash
   idf.py build flash monitor
   ```

## Hardware Setup

Connect your e-paper display to the ESP32:

| E-Paper Pin | ESP32-C6 Default | Description |
|-------------|------------------|-------------|
| VCC         | 3.3V            | Power       |
| GND         | GND             | Ground      |
| DIN         | GPIO 7          | SPI MOSI    |
| CLK         | GPIO 6          | SPI Clock   |
| CS          | GPIO 10         | Chip Select |
| DC          | GPIO 9          | Data/Command|
| RST         | GPIO 4          | Reset       |
| BUSY        | GPIO 18         | Busy Signal |

> **Important:** The BUSY pin must be connected for the display to work properly.

## Customizing GPIO Pins

Edit `components/epaper/DEV_Config.c` to change pin assignments:

```c
static void DEV_GPIO_Init(void)
{
    EPD_RST_PIN  = GPIO_NUM_4;   // Your RST pin
    EPD_DC_PIN   = GPIO_NUM_9;   // Your DC pin
    EPD_BUSY_PIN = GPIO_NUM_18;  // Your BUSY pin
    EPD_CS_PIN   = GPIO_NUM_10;  // Your CS pin
    EPD_CLK_PIN  = GPIO_NUM_6;   // Your CLK pin
    EPD_MOSI_PIN = GPIO_NUM_7;   // Your MOSI pin
    // ...
}
```

## Tips

1. **Buffer Size**: Always calculate correctly:
   ```c
   #define WIDTHBYTE(w) (((w) % 8 == 0) ? ((w) / 8) : ((w) / 8 + 1))
   #define IMAGE_SIZE (WIDTHBYTE(EPD_2IN13_WIDTH) * EPD_2IN13_HEIGHT)
   ```

2. **Landscape Mode**: Use `ROTATE_90` for 250x122 resolution
3. **Partial Updates**: Use `EPD_2IN13_Display_Partial()` for faster updates
4. **Coordinate Safety**: Start shapes from (1,1) instead of (0,0) to avoid underflow

## Troubleshooting

- **Display not updating**: Check BUSY pin connection
- **Garbage on screen**: Verify SPI connections and buffer size
- **Crash on exit**: Ensure IMAGE_SIZE is calculated correctly
