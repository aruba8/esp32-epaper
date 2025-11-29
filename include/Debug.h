/*****************************************************************************
 * | File      	:   Debug.h
 * | Author      :
 * | Function    :   Debug output definition
 * | Info        :   Uses ESP-IDF logging system
 *----------------
 * |	This version:   V2.0
 * | Date        :
 * | Info        :   Replaced printf with ESP logging
 *
 ******************************************************************************/
#ifndef __DEBUG_H
#define __DEBUG_H

#include "esp_log.h"

// Enable debug logging by default
// To disable debug logs, set log level in your app:
// esp_log_level_set("EPD", ESP_LOG_INFO);
// esp_log_level_set("GUI_PAINT", ESP_LOG_INFO);

#define USE_DEBUG 1

#if USE_DEBUG
#define Debug(__info, ...) ESP_LOGD(TAG, __info, ##__VA_ARGS__)
#else
#define Debug(__info, ...)
#endif

#endif
