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

extern void ProcessDoorK1K2Sns(PinSnsState_t *PState, uint32_t Len);
extern void ProcessHandcarStartSns(PinSnsState_t *PState, uint32_t Len);
extern void ProcessHandcarCenterSns(PinSnsState_t *PState, uint32_t Len);
extern void ProcessHandcarStopSns(PinSnsState_t *PState, uint32_t Len);
extern void ProcessHeartSns(PinSnsState_t *PState, uint32_t Len);

const PinSns_t PinSns[] = {
        // Button
        {LED_CH2_in, ProcessButtons},
        // 5V sns
        {ExternalPWR_Pin, Process5VSns},
        // Other sns
        {Port1_in, ProcessHandcarCenterSns},
        {Port2_in, ProcessHeartSns},
        {Port3_in, ProcessDoorK1K2Sns},
        {Port4_in, ProcessHandcarStartSns},
        {Port5_in, ProcessHandcarStopSns},
};
#define PIN_SNS_CNT     countof(PinSns)

#endif  // if enabled
