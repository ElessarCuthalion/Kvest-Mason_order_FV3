/*
 * ws2812b.h
 *
 *  Created on: 05 ���. 2014 �.
 *      Author: Kreyl
 */

#pragma once

/*
 * ========== WS2812 control module ==========
 * Only basic command "SetCurrentColors" is implemented, all other is up to
 * higher level software.
 * SPI input frequency should be 8 MHz (which results in 4MHz bitrate)
 */


#include "ch.h"
#include "hal.h"
#include "kl_lib.h"
#include "uart.h"

#ifndef LED_CNT
#define LED_CNT             24   // Number of WS2812 LEDs
#endif
// Do not touch
#define SEQ_LEN             4
#define RST_W_CNT           4 // zero words before and after data to produce reset

// SPI16 Buffer (no tuning required)
#define DATA_BIT_CNT        (LED_CNT * 3 * 8 * SEQ_LEN)   // Each led has 3 channels 8 bit each
#define DATA_W_CNT          ((DATA_BIT_CNT + 15) / 16)
#define TOTAL_W_CNT         (DATA_W_CNT + RST_W_CNT)


// ============================= Simple Colors =================================
#define sclBlack     ((ColorWS_t){0,   0,   0})
#define sclRed       ((ColorWS_t){255, 0,   0})
#define sclGreen     ((ColorWS_t){0,   255, 0})
#define sclBlue      ((ColorWS_t){0,   0,   255})
#define sclYellow    ((ColorWS_t){255, 255, 0})
#define sclMagenta   ((ColorWS_t){255, 0, 255})
#define sclCyan      ((ColorWS_t){0, 255, 255})
#define sclWhite     ((ColorWS_t){255, 255, 255})

#define sclLightGrey ((ColorWS_t){180, 180, 180})
#define sclGrey      ((ColorWS_t){126, 126, 126})
#define sclDarkGrey  ((ColorWS_t){54, 54, 54})


typedef struct {
    uint8_t R, G, B;
} ColorWS_t;

class IntelLeds_t {
private:
    Spi_t ISpi {LEDWS_SPI};
    uint16_t IBuf[TOTAL_W_CNT];
    uint16_t *PBuf;
    void AppendBitsMadeOfByte(uint8_t Byte);
public:
    void Init();
    bool AreOff() {
        for(uint8_t i=0; i<LED_CNT; i++) {
            if(ICurrentClr[i].R != 0) return false;
            if(ICurrentClr[i].G != 0) return false;
            if(ICurrentClr[i].B != 0) return false;
        }
        return true;
    }
    void SetCurrentColors() {
        chSysLock();
        ISetCurrentColors();
        chSysUnlock();
    }
    // Inner use
    ColorWS_t ICurrentClr[LED_CNT];
    void ISetCurrentColors();
};
