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

extern void ProcessKey1(PinSnsState_t *PState, uint32_t Len);
extern void ProcessKey2(PinSnsState_t *PState, uint32_t Len);
extern void ProcessKey3(PinSnsState_t *PState, uint32_t Len);
extern void ProcessKey4(PinSnsState_t *PState, uint32_t Len);
extern void ProcessKey5(PinSnsState_t *PState, uint32_t Len);
extern void ProcessKey6(PinSnsState_t *PState, uint32_t Len);
extern void ProcessKey7(PinSnsState_t *PState, uint32_t Len);

#define PianoKeys_CNT   7
extern void ProcessKeySens(PinSnsState_t *PState, uint32_t Len);

const PinSns_t PinSns[] = {
        // 5V sns
        {ExternalPWR_Pin, Process5VSns},
#if QUEST_ROOM == WoodmanRoom
        // Button
        {LED_CH2_in, ProcessButtons},
        // Other sns
        {Port1_in, ProcessHandcarCenterSns},
        {Port2_in, ProcessHeartSns},
        {Port3_in, ProcessDoorK1K2Sns},
        {Port4_in, ProcessHandcarStartSns},
        {Port5_in, ProcessHandcarStopSns},
#elif QUEST_ROOM == PianoRoom
        // Button
        {PwPort5_in, ProcessButtons},
        // Other sns
        {Port1_in, ProcessKeySens},
        {Port2_in, ProcessKeySens},
        {Port3_in, ProcessKeySens},
        {Port4_in, ProcessKeySens},
        {Port5_in, ProcessKeySens},
        {USER_RX_in, ProcessKeySens},
        {USER_TX_in, ProcessKeySens},
//        {Port1_in, ProcessKey1},
//        {Port2_in, ProcessKey2},
//        {Port3_in, ProcessKey3},
//        {Port4_in, ProcessKey4},
//        {Port5_in, ProcessKey5},
//        {USER_RX_in, ProcessKey6},
//        {USER_TX_in, ProcessKey7},
#endif
};
#define PIN_SNS_CNT     countof(PinSns)

#endif  // if enabled
