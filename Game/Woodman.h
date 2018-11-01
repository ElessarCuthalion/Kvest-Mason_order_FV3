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
#include "Sequences.h"
#include "led.h"

// Sound
#define WoodmanMonologue_file   "WoodmanMonologue.mp3"
#define WoodmanMonologue_VolLevel   240
#define WoodmanMonologue_Channel    SndCh1
// Heart LEDs
#define HeartBlinkPeriod_MS     1000
#define HeartBeginIndex         0
#define HeartEndIndex           15
#define HeartColor              sclRed
#define HeartBrightness         70
#define SmileColor              sclBlue
#define SmileBrightness         60
#define SmileWidth_LEDs         7
#define SmileBeginIndex         16
// Times
#define TunnelLight_TimeOut_MS  10000
#define HeadUp_TimeOut_MS       2000

// Event mask
#define WM_EVT_WaitTimeOut              EVENT_MASK(1)
#define WM_EVT_HeartBlinkTimeOut        EVENT_MASK(2)
#define WM_EVT_GestureProcessing        EVENT_MASK(3)
#define WM_EVT_HandcarInTransit         EVENT_MASK(4)
#define WM_EVT_HeartReturn              EVENT_MASK(5)
#define WM_EVT_WoodmanTimeOut           EVENT_MASK(6)
#define WM_EVT_HandcarParked            EVENT_MASK(7)


//enum TmrWoodmanState {wsPause, wsHandcarSignal};

typedef enum {
    asExpectation, asWoodmanActive, asHeartReturned, asDoorOpened,
} WoodmanState_t;
typedef struct {
    uint8_t Level;
    systime_t Position_MS;
}  Gesture_t;
const Gesture_t GestureMonologue[] = {
        {5, 1500},
        {3, 5500},
        {7, 2000},
};
#define GestureLimit    countof(GestureMonologue)

static ColorWS_t Brightness(ColorWS_t AColor, uint8_t APercent) {
    ColorWS_t Result = {0};
    Result.R = (uint16_t)(AColor.R*APercent)/100;
    Result.G = (uint16_t)(AColor.G*APercent)/100;
    Result.B = (uint16_t)(AColor.B*APercent)/100;
    return Result;
}

class Woodman_t {
private:
    IntelLeds_t LedWs;
    LedSmooth_t TunnelLighting{LED_PWM4};
    PinOutput_t Backlight {PwPort3_out};
    PinOutput_t Head {PwPort1_out};
    PinOutput_t DoorK2K3 {PwPort2_out};
    PinOutput_t HandcarSignal {PwPort4_out};
    PinInput_t Heart_pin {Port2_in};
    TmrKL_t TmrWait { WM_EVT_WaitTimeOut, tktOneShot };
    TmrKL_t HeartBlinkTmr {MS2ST(HeartBlinkPeriod_MS), WM_EVT_HeartBlinkTimeOut, tktPeriodic };
    TmrKL_t GestureProcess_Tmr { WM_EVT_GestureProcessing, tktOneShot };
    thread_t *IPAppThd;
    bool BacklightOn = false;

    WoodmanState_t State = asExpectation;

public:
    void SignalEvt(uint32_t EvtMsk) {
        chSysLock();
        chEvtSignalI(PThread, EvtMsk);
        chSysUnlock();
    }

    void BacklightON() { Backlight.SetHi(); BacklightOn = true; }
    void BacklightOFF() { Backlight.SetLo(); BacklightOn = false; }
    bool BacklightIsOn() { return BacklightOn; }
    void HeadUp() { Head.SetLo(); }
    void HeartBlinkON() { HeartBlinkTmr.StartOrRestart(); }
    void HeartBlinkOFF() {
        HeartBlinkTmr.Stop();
        for (uint8_t i=HeartBeginIndex; i<HeartEndIndex; i++)
            LedWs.ICurrentClr[i] = HeartColor;
        LedWs.SetCurrentColors();
    }
    void OpenDoor() { DoorK2K3.SetLo(); State = asDoorOpened; }
    void StartGesture() { chEvtSignal(PThread, WM_EVT_GestureProcessing); }
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
        State = asExpectation;
        BacklightOFF();  // отключить подсветку дровосека
        Head.SetHi();       // замагнитить голову
        DoorK2K3.SetHi();   // замагнитить дверь на выход
        HandcarSignal.SetLo();  // погасить сигнал на дрезину (про запас)
        TunnelLighting.StartOrContinue(lsqFadeOut);
        for (uint8_t i=0; i < LED_CNT; i++)     // погасить подсветку туннеля
            LedWs.ICurrentClr[i] = sclBlack;    // погосить подсветку сердца, рта и глаз
        LedWs.SetCurrentColors();
    }

    // Inner use
    thread_t *PThread;
    void Init();
    void ITask();
};

extern Woodman_t Woodman;

