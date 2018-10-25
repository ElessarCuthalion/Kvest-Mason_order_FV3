/*
 * Woodman.h
 *
 *  Created on: 22 окт. 2018 г.
 *      Author: Elessar
 */

#pragma once

#include "uart.h"
#include "kl_lib.h"
#include "board.h"
#include "evt_mask.h"
#include "ws2812b_simple.h"

// Sound
#define WoodmanMonologue_file   "WoodmanMonologue.mp3"
#define WoodmanMonologue_VolLevel      240
// Heart LEDs
#define HeartBlinkPeriod_MS     1000
#define HeartBeginIndex         0
#define HeartEndIndex           15
#define HeartColor              sclRed
// Times
#define HeadUp_TimeOut_MS       2000

// Event mask
#define WM_EVT_PauseTimeOut         EVENT_MASK(1)
#define WM_EVT_HeartBlinkTimeOut    EVENT_MASK(2)
#define WM_EVT_WS_processTimeOut    EVENT_MASK(3)
#define EVT_HandcarInTransit        EVENT_MASK(4)
#define EVT_HeartReturn             EVENT_MASK(5)
#define EVT_WoodmanTimeOut          EVENT_MASK(6)


enum TmrWoodmanState {wsPause, wsHandcarSignal};

typedef enum {
    asExpectation, asWoodmanActive, asHeartReturned, asDoorOpened,
} WoodmanState_t;

class Woodman_t {
private:
    IntelLeds_t LedWs;
    PinOutput_t Backlight {PwPort3_out};
    PinOutput_t Head {PwPort1_out};
    PinOutput_t DoorK2K3 {PwPort2_out};
    PinOutput_t HandcarSignal {PwPort4_out};
    PinInput_t Heart_pin {Port2_in};
    TmrKL_t TmrWait { WM_EVT_PauseTimeOut, tktOneShot };
    TmrKL_t HeartBlinkTmr {MS2ST(HeartBlinkPeriod_MS), WM_EVT_HeartBlinkTimeOut, tktPeriodic };
    TmrKL_t WS_process_Tmr { WM_EVT_WS_processTimeOut, tktOneShot };
    thread_t *IPAppThd;

    WoodmanState_t State = asExpectation;

public:
    void SignalEvt(uint32_t EvtMsk) {
        chSysLock();
        chEvtSignalI(PThread, EvtMsk);
        chSysUnlock();
    }

    void BacklightON() { Backlight.SetHi(); }
    void HeadUp() { Head.SetLo(); }
    void HeartBlinkON() { HeartBlinkTmr.StartOrRestart(); }
    void HeartBlinkOFF() { HeartBlinkTmr.Stop(); }
    void OpenDoor() { DoorK2K3.SetLo(); State = asDoorOpened; }
    void StartGesture() {}
    void SignalToHandcar() {
        HandcarSignal.SetHi();
        chThdSleepMilliseconds(5000);
        HandcarSignal.SetLo();
    }

    void Pause_MS(systime_t ATime_MS) {
        TmrWait.StartOrRestart(MS2ST(ATime_MS));
    }
    WoodmanState_t GetState() { return State; }

//    void RegisterAppThd(thread_t *PThd) { IPAppThd = PThd; }

    void DefaultState() {
        Backlight.SetLo();  // отключить подсветку
        Head.SetHi();       // замагнитить голову
        DoorK2K3.SetHi();   // замагнитить дверь на выход
        HeartBlinkOFF();    // погосить подсветку сердца, рта и глаз
        HandcarSignal.SetLo();  // погасить сигнал на дрезину (про запас)
    }

    // Inner use
    thread_t *PThread;
    void Init();
    void ITask();
};

extern Woodman_t Woodman;

