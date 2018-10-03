/*
 * SnsPins.h
 *
 *  Created on: 17 џэт. 2015 у.
 *      Author: Kreyl
 */

/* ================ Documentation =================
 * There are several (may be 1) groups of sensors (say, buttons and USB connection).
 *
 */

#pragma once

#include "SimpleSensors.h"

#ifndef SIMPLESENSORS_ENABLED
#define SIMPLESENSORS_ENABLED   FALSE
#endif

#if SIMPLESENSORS_ENABLED
#define SNS_POLL_PERIOD_MS      50

// Button handler
extern void ProcessButtons(PinSnsState_t *PState, uint32_t Len);
extern void Process5VSns(PinSnsState_t *PState, uint32_t Len);

const PinSns_t PinSns[] = {
        // Button
        {Port1_in, ProcessButtons},
        {Port2_in, ProcessButtons},
        {Port3_in, ProcessButtons},
        {Port4_in, ProcessButtons},
        {Port5_in, ProcessButtons},
        {PwPort5_in, ProcessButtons},
        // 5V sns
        {ExternalPWR_Pin, Process5VSns},
};
#define PIN_SNS_CNT     countof(PinSns)

#endif  // if enabled
