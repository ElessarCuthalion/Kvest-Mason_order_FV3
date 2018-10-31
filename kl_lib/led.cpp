/*
 * led.cpp
 *
 *  Created on: 9 ���. 2016 �.
 *      Author: Kreyl
 */

#include "led.h"

void LedSmoothTmrCallback(void *p) {
    chSysLockFromISR();
    ((LedSmooth_t*)p)->IProcessSequenceI();
    chSysUnlockFromISR();
}

#ifdef RGBTmrCallback
void LedRGBTmrCallback(void *p) {
    chSysLockFromISR();
    ((LedRGB_t*)p)->IProcessSequenceI();
    chSysUnlockFromISR();
}
#endif
