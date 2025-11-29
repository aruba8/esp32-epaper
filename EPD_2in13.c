/*****************************************************************************
 * | File      	:   EPD_2in13.c
 * | Author      :
 * | Function    :   2.13inch e-paper
 * | Info        :
 *----------------
 * |	This version:   V1.0
 * | Date        :
 * | Info        :
 * -----------------------------------------------------------------------------
 ******************************************************************************/
#include "EPD_2in13.h"
#include "Debug.h"

static const char *TAG = "EPD";

/******************************************************************************
function :	Software reset
parameter:
******************************************************************************/
static void EPD_2IN13_Reset(void)
{
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(20);
    DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(2);
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(20);
}

/******************************************************************************
function :	send command
parameter:
     Reg : Command register
******************************************************************************/
static void EPD_2IN13_SendCommand(UBYTE Reg)
{
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(Reg);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
static void EPD_2IN13_SendData(UBYTE Data)
{
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(Data);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	Wait until the busy_pin goes LOW
parameter:
******************************************************************************/
static void EPD_2IN13_ReadBusy(void)
{
    Debug("e-Paper busy\r\n");
    while (1)
    { //=1 BUSY
        if (DEV_Digital_Read(EPD_BUSY_PIN) == 0)
            break;
        DEV_Delay_ms(10);
    }
    DEV_Delay_ms(10);
    Debug("e-Paper busy release\r\n");
}

/******************************************************************************
function :	Setting the display window
parameter:
    Xstart : X-axis starting position
    Ystart : Y-axis starting position
    Xend : End position of X-axis
    Yend : End position of Y-axis
******************************************************************************/
static void EPD_2IN13_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{
    EPD_2IN13_SendCommand(0x44); // SET_RAM_X_ADDRESS_START_END_POSITION
    EPD_2IN13_SendData((Xstart >> 3) & 0xFF);
    EPD_2IN13_SendData((Xend >> 3) & 0xFF);

    EPD_2IN13_SendCommand(0x45); // SET_RAM_Y_ADDRESS_START_END_POSITION
    EPD_2IN13_SendData(Ystart & 0xFF);
    EPD_2IN13_SendData((Ystart >> 8) & 0xFF);
    EPD_2IN13_SendData(Yend & 0xFF);
    EPD_2IN13_SendData((Yend >> 8) & 0xFF);
}

/******************************************************************************
function :	Set Cursor
parameter:
    Xstart : X-axis starting position
    Ystart : Y-axis starting position
******************************************************************************/
static void EPD_2IN13_SetCursor(UWORD Xstart, UWORD Ystart)
{
    EPD_2IN13_SendCommand(0x4E); // SET_RAM_X_ADDRESS_COUNTER
    EPD_2IN13_SendData(Xstart & 0xFF);

    EPD_2IN13_SendCommand(0x4F); // SET_RAM_Y_ADDRESS_COUNTER
    EPD_2IN13_SendData(Ystart & 0xFF);
    EPD_2IN13_SendData((Ystart >> 8) & 0xFF);
}

/******************************************************************************
function :	Turn On Display
parameter:
******************************************************************************/
static void EPD_2IN13_TurnOnDisplay(void)
{
    EPD_2IN13_SendCommand(0x22); // Display Update Control
    EPD_2IN13_SendData(0xf7);
    EPD_2IN13_SendCommand(0x20); // Activate Display Update Sequence
    EPD_2IN13_ReadBusy();
}

static void EPD_2IN13_TurnOnDisplay_Fast(void)
{
    EPD_2IN13_SendCommand(0x22); // Display Update Control
    EPD_2IN13_SendData(0xc7);    // fast:0x0c, quality:0x0f, 0xcf
    EPD_2IN13_SendCommand(0x20); // Activate Display Update Sequence
    EPD_2IN13_ReadBusy();
}

static void EPD_2IN13_TurnOnDisplay_Partial(void)
{
    EPD_2IN13_SendCommand(0x22); // Display Update Control
    EPD_2IN13_SendData(0xff);    // fast:0x0c, quality:0x0f, 0xcf
    EPD_2IN13_SendCommand(0x20); // Activate Display Update Sequence
    EPD_2IN13_ReadBusy();
}

/******************************************************************************
function :	Initialize the e-Paper register
parameter:
******************************************************************************/
void EPD_2IN13_Init(void)
{
    ESP_LOGI(TAG, "Initializing e-Paper display...");
    EPD_2IN13_Reset();

    EPD_2IN13_ReadBusy();
    EPD_2IN13_SendCommand(0x12); // SWRESET
    EPD_2IN13_ReadBusy();

    EPD_2IN13_SendCommand(0x01); // Driver output control
    EPD_2IN13_SendData(0xF9);
    EPD_2IN13_SendData(0x00);
    EPD_2IN13_SendData(0x00);

    EPD_2IN13_SendCommand(0x11); // data entry mode
    EPD_2IN13_SendData(0x03);

    EPD_2IN13_SetWindows(0, 0, EPD_2IN13_WIDTH - 1, EPD_2IN13_HEIGHT - 1);
    EPD_2IN13_SetCursor(0, 0);

    EPD_2IN13_SendCommand(0x3C); // BorderWavefrom
    EPD_2IN13_SendData(0x05);

    EPD_2IN13_SendCommand(0x21); //  Display update control
    EPD_2IN13_SendData(0x00);
    EPD_2IN13_SendData(0x80);

    EPD_2IN13_SendCommand(0x18); // Read built-in temperature sensor
    EPD_2IN13_SendData(0x80);
    EPD_2IN13_ReadBusy();
    ESP_LOGI(TAG, "e-Paper display initialized");
}

void EPD_2IN13_Init_Fast(void)
{
    ESP_LOGI(TAG, "Initializing e-Paper display (fast mode)...");
    EPD_2IN13_Reset();

    EPD_2IN13_SendCommand(0x12); // SWRESET
    EPD_2IN13_ReadBusy();

    EPD_2IN13_SendCommand(0x18); // Read built-in temperature sensor
    EPD_2IN13_SendData(0x80);

    EPD_2IN13_SendCommand(0x11); // data entry mode
    EPD_2IN13_SendData(0x03);

    EPD_2IN13_SetWindows(0, 0, EPD_2IN13_WIDTH - 1, EPD_2IN13_HEIGHT - 1);
    EPD_2IN13_SetCursor(0, 0);

    EPD_2IN13_SendCommand(0x22); // Load temperature value
    EPD_2IN13_SendData(0xB1);
    EPD_2IN13_SendCommand(0x20);
    EPD_2IN13_ReadBusy();

    EPD_2IN13_SendCommand(0x1A); // Write to temperature register
    EPD_2IN13_SendData(0x64);
    EPD_2IN13_SendData(0x00);

    EPD_2IN13_SendCommand(0x22); // Load temperature value
    EPD_2IN13_SendData(0x91);
    EPD_2IN13_SendCommand(0x20);
    EPD_2IN13_ReadBusy();
    ESP_LOGI(TAG, "e-Paper display initialized (fast mode)");
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void EPD_2IN13_Clear(void)
{
    ESP_LOGI(TAG, "Clearing display to white...");
    UWORD Width, Height;
    Width = (EPD_2IN13_WIDTH % 8 == 0) ? (EPD_2IN13_WIDTH / 8) : (EPD_2IN13_WIDTH / 8 + 1);
    Height = EPD_2IN13_HEIGHT;

    EPD_2IN13_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            EPD_2IN13_SendData(0XFF);
        }
    }

    EPD_2IN13_TurnOnDisplay();
}

void EPD_2IN13_Clear_Black(void)
{
    UWORD Width, Height;
    Width = (EPD_2IN13_WIDTH % 8 == 0) ? (EPD_2IN13_WIDTH / 8) : (EPD_2IN13_WIDTH / 8 + 1);
    Height = EPD_2IN13_HEIGHT;

    EPD_2IN13_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            EPD_2IN13_SendData(0X00);
        }
    }

    EPD_2IN13_TurnOnDisplay();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
    Image : Image data
******************************************************************************/
void EPD_2IN13_Display(UBYTE *Image)
{
    UWORD Width, Height;
    Width = (EPD_2IN13_WIDTH % 8 == 0) ? (EPD_2IN13_WIDTH / 8) : (EPD_2IN13_WIDTH / 8 + 1);
    Height = EPD_2IN13_HEIGHT;

    EPD_2IN13_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            EPD_2IN13_SendData(Image[i + j * Width]);
        }
    }

    EPD_2IN13_TurnOnDisplay();
}

void EPD_2IN13_Display_Fast(UBYTE *Image)
{
    UWORD Width, Height;
    Width = (EPD_2IN13_WIDTH % 8 == 0) ? (EPD_2IN13_WIDTH / 8) : (EPD_2IN13_WIDTH / 8 + 1);
    Height = EPD_2IN13_HEIGHT;

    EPD_2IN13_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            EPD_2IN13_SendData(Image[i + j * Width]);
        }
    }

    EPD_2IN13_TurnOnDisplay_Fast();
}

/******************************************************************************
function :	Refresh a base image
parameter:
    Image : Image data
******************************************************************************/
void EPD_2IN13_Display_Base(UBYTE *Image)
{
    UWORD Width, Height;
    Width = (EPD_2IN13_WIDTH % 8 == 0) ? (EPD_2IN13_WIDTH / 8) : (EPD_2IN13_WIDTH / 8 + 1);
    Height = EPD_2IN13_HEIGHT;

    EPD_2IN13_SendCommand(0x24); // Write Black and White image to RAM
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            EPD_2IN13_SendData(Image[i + j * Width]);
        }
    }
    EPD_2IN13_SendCommand(0x26); // Write Black and White image to RAM
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            EPD_2IN13_SendData(Image[i + j * Width]);
        }
    }
    EPD_2IN13_TurnOnDisplay();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and partial refresh
parameter:
    Image : Image data
******************************************************************************/
void EPD_2IN13_Display_Partial(UBYTE *Image)
{
    UWORD Width, Height;
    Width = (EPD_2IN13_WIDTH % 8 == 0) ? (EPD_2IN13_WIDTH / 8) : (EPD_2IN13_WIDTH / 8 + 1);
    Height = EPD_2IN13_HEIGHT;

    // Reset
    DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(2);
    DEV_Digital_Write(EPD_RST_PIN, 1);

    EPD_2IN13_SendCommand(0x3C); // BorderWavefrom
    EPD_2IN13_SendData(0x80);

    EPD_2IN13_SendCommand(0x01); // Driver output control
    EPD_2IN13_SendData(0xF9);
    EPD_2IN13_SendData(0x00);
    EPD_2IN13_SendData(0x00);

    EPD_2IN13_SendCommand(0x11); // data entry mode
    EPD_2IN13_SendData(0x03);

    EPD_2IN13_SetWindows(0, 0, EPD_2IN13_WIDTH - 1, EPD_2IN13_HEIGHT - 1);
    EPD_2IN13_SetCursor(0, 0);

    EPD_2IN13_SendCommand(0x24); // Write Black and White image to RAM
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            EPD_2IN13_SendData(Image[i + j * Width]);
        }
    }
    EPD_2IN13_TurnOnDisplay_Partial();
}

void EPD_2IN13_Display_PartialRegion(UBYTE *Image, UWORD X, UWORD Y, UWORD Width, UWORD Height)
{
    UWORD RowBytes = (EPD_2IN13_WIDTH % 8 == 0) ? (EPD_2IN13_WIDTH / 8) : (EPD_2IN13_WIDTH / 8 + 1);
    UWORD x_start = (X / 8) * 8;
    UWORD x_end = ((X + Width + 7) / 8) * 8 - 1;
    if (x_end >= EPD_2IN13_WIDTH)
    {
        x_end = EPD_2IN13_WIDTH - 1;
    }
    UWORD x_aligned_start = x_start / 8;
    UWORD x_aligned_end = x_end / 8;
    UWORD window_bytes = x_aligned_end - x_aligned_start + 1;
    UWORD y_start = Y;
    UWORD y_end = Y + Height - 1;
    if (y_end >= EPD_2IN13_HEIGHT)
    {
        y_end = EPD_2IN13_HEIGHT - 1;
    }

    DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(2);
    DEV_Digital_Write(EPD_RST_PIN, 1);

    EPD_2IN13_SendCommand(0x3C); // Border waveform
    EPD_2IN13_SendData(0x80);

    EPD_2IN13_SendCommand(0x01); // driver output control
    EPD_2IN13_SendData(0xF9);
    EPD_2IN13_SendData(0x00);
    EPD_2IN13_SendData(0x00);

    EPD_2IN13_SendCommand(0x11); // data entry mode
    EPD_2IN13_SendData(0x03);

    EPD_2IN13_SetWindows(x_start, y_start, x_end, y_end);
    EPD_2IN13_SetCursor(x_start, y_start);

    EPD_2IN13_SendCommand(0x24);
    for (UWORD row = y_start; row <= y_end; row++)
    {
        UWORD row_offset = row * RowBytes + x_aligned_start;
        for (UWORD col = 0; col < window_bytes; col++)
        {
            EPD_2IN13_SendData(Image[row_offset + col]);
        }
    }
    EPD_2IN13_TurnOnDisplay_Partial();
}

/******************************************************************************
function :	Enter sleep mode
parameter:
******************************************************************************/
void EPD_2IN13_Sleep(void)
{
    ESP_LOGI(TAG, "Entering deep sleep mode...");
    EPD_2IN13_SendCommand(0x10); // enter deep sleep
    EPD_2IN13_SendData(0x01);
    DEV_Delay_ms(100);
    ESP_LOGI(TAG, "Display in deep sleep");
}
