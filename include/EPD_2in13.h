/*****************************************************************************
 * | File      	:   EPD_2in13.h
 * | Author      :
 * | Function    :   2.13inch e-paper
 * | Info        :
 *----------------
 * |	This version:   V1.0
 * | Date        :
 * | Info        :
 * -----------------------------------------------------------------------------
 ******************************************************************************/
#ifndef __EPD_2IN13_H_
#define __EPD_2IN13_H_

#include "DEV_Config.h"

// Display resolution
#define EPD_2IN13_WIDTH 122
#define EPD_2IN13_HEIGHT 250

void EPD_2IN13_Init(void);
void EPD_2IN13_Init_Fast(void);
void EPD_2IN13_Clear(void);
void EPD_2IN13_Clear_Black(void);
void EPD_2IN13_Display(UBYTE *Image);
void EPD_2IN13_Display_Fast(UBYTE *Image);
void EPD_2IN13_Display_Base(UBYTE *Image);
void EPD_2IN13_Display_Partial(UBYTE *Image);
void EPD_2IN13_Display_PartialRegion(UBYTE *Image, UWORD X, UWORD Y, UWORD Width, UWORD Height);
void EPD_2IN13_Sleep(void);

#endif
