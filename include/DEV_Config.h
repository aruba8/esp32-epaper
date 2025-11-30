/*****************************************************************************
 * | File        :   DEV_Config.h
 * | Author      :
 * | Function    :   Hardware underlying interface
 * | Info        :
 *                Used to shield the underlying layers of each master
 *                and enhance portability
 *----------------
 * |	This version:   V1.0
 * | Date        :
 * | Info        :
 *
 ******************************************************************************/
#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_

#include <stdint.h>

/**
 * data types
 **/
#define UBYTE uint8_t
#define UWORD uint16_t
#define UDOUBLE uint32_t

/**
 * Pin configuration structure
 **/
typedef struct {
    int rst_pin;
    int dc_pin;
    int cs_pin;
    int busy_pin;
    int clk_pin;
    int mosi_pin;
} epd_pin_config_t;

/**
 * GPIO config
 **/
extern int EPD_RST_PIN;
extern int EPD_DC_PIN;
extern int EPD_CS_PIN;
extern int EPD_BUSY_PIN;
extern int EPD_CLK_PIN;
extern int EPD_MOSI_PIN;

/*------------------------------------------------------------------------------------------------------*/
void DEV_Digital_Write(UWORD Pin, UBYTE Value);
UBYTE DEV_Digital_Read(UWORD Pin);

void DEV_SPI_WriteByte(UBYTE Value);
void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len);
void DEV_Delay_ms(UDOUBLE xms);

UBYTE DEV_Module_Init(const epd_pin_config_t *pin_config);
void DEV_Module_Exit(void);

#endif
