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
#define WoodmanMonologue_file       "WoodmanMonologue.mp3"
#define WoodmanMonologue_VolLevel   240
#define WoodmanMonologue_Channel    SndCh1
// Heart LEDs
#define HeartBlinkPeriod_MS     1000
#define HeartBeginIndex         0
#define HeartEndIndex           13  // 14LEDs
#define HeartColor              sclRed
#define HeartBrightness         70
// Smile LEDs
#define SmileColor              sclBlue
#define SmileBrightness         20
#define SmileWidth_LEDs         7
#define SmileBeginIndex         14
// Eye LEDs
#define EyeColor                sclGreen
#define EyeBrightness           20
#define EyeLeftIndex            28
#define EyeRightIndex           29
// Times
#define TunnelLight_TimeOut_MS  10000
#define HeadUp_TimeOut_MS       2000
#define Wink_TimeOut_MS         700

// Event mask
#define WM_EVT_DoorK1K2Opened           EVENT_MASK(1)
#define WM_EVT_HandcarParked            EVENT_MASK(2)
#define WM_EVT_HandcarInTransit         EVENT_MASK(3)
#define WM_EVT_HandcarStoped            EVENT_MASK(4)
#define WM_EVT_WaitTimeOut              EVENT_MASK(5)
#define WM_EVT_HeartReturn              EVENT_MASK(6)
#define WM_EVT_HeartBlinkTimeOut        EVENT_MASK(7)
#define WM_EVT_GestureProcessing        EVENT_MASK(8)
#define WM_EVT_ToWink                   EVENT_MASK(9)
#define WM_EVT_SignalToHandcar          EVENT_MASK(10)



//enum TmrWoodmanState {wsPause, wsHandcarSignal};

typedef enum {
    wsExpectation, wsWoodmanActive, wsHeartReturned, wsMonologueCompleted, wsDoorOpened,
} WoodmanState_t;
typedef struct {
    uint8_t Level;  // количество светодиодов
    systime_t Position_MS;  // время в которое они зажглись (позиция в ролике)
}  Gesture_t;
const Gesture_t WoodmanMonologue[] = {
        {0, 0},     // начальный момент времени
        {4, 120}, {2, 360}, {6, 440}, {4, 520},  {7, 760},
        {2, 1000}, {6, 1120}, {0, 1280}, {2, 1560}, {1, 1600}, {3, 1640}, {4, 1760}, {6, 1960},
        {2, 2160}, {3, 2240}, {7, 2360}, {0, 2520}, {2, 2680}, {5, 2760},
        {2, 3080}, {7, 3160}, {1, 3240}, {4, 3320}, {3, 3360}, {4, 3520}, {7, 3600}, {4, 3680}, {6, 3760},
        {2, 4120}, {3, 4200}, {7, 4280}, {2, 4480}, {3, 4560}, {1, 4640}, {3, 4720}, {1, 4760}, {4, 4800}, {7, 4960},
        {4, 5120}, {1, 5240}, {3, 5320}, {2, 5600}, {7, 5680}, {4, 5760}, {6, 5840}, {7, 5960},
        {4, 6200}, {1, 6300}, {0, 6480}, {2, 6880}, {3, 6960},
        {1, 7200}, {4, 7360}, {7, 7480}, {1, 7720}, {3, 7880}, {7, 7960},
        {0, 8160}, {1, 8840}, {3, 8880}, {4, 8960},
        {2, 9200}, {3, 9280}, {4, 9400}, {0, 9440}, {6, 9520}, {0, 9720}, {1, 9960},
        {7, 10160}, {1, 10320}, {3, 10360}, {4, 10440}, {1, 10600}, {0, 10640}, {2, 10680}, {4, 10800}, {6, 10920},
        {2, 11000}, {6, 11080}, {4, 11120}, {7, 11160}, {4, 11360}, {6, 11400}, {3, 11480}, {7, 11520}, {4, 11600}, {6, 11640}, {2, 11760}, {1, 11880}, {3, 11920},
        {4, 12080}, {7, 12160}, {4, 12320}, {0, 12360}, {7, 12640}, {3, 12720}, {4, 12840},
        {1, 13000}, {3, 13200}, {7, 13360}, {4, 13480}, {6, 13560}, {3, 13680}, {2, 13840}, {6, 13920},
        {4, 14280}, {0, 14400},
        {4, 15080}, {3, 15160}, {1, 15360}, {7, 15440}, {2, 15600}, {7, 15680}, {1, 15760}, {0, 15840},
        {1, 16200}, {3, 16280}, {2, 16350}, {4, 16440}, {7, 16520}, {2, 16640}, {7, 16680}, {1, 16760}, {3, 16840},
        {4, 17080}, {2, 17360}, {3, 17400}, {2, 17520}, {4, 17640}, {7, 17760},
        {1, 18000}, {3, 18120}, {7, 18200}, {0, 18400}, {7, 18720}, {0, 18920}, {6, 18960},
        {2, 19160}, {5, 19240}, {2, 19480}, {7, 19520}, {1, 19600}, {3, 19640}, {4, 19800}, {0, 19920},
        {6, 20360}, {2, 20520}, {6, 20640}, {2, 20920},
        {0, 21120}, {4, 21320}, {6, 21440}, {2, 21560}, {6, 21640}, {2, 21800}, {3, 21840}, {4, 21960},
        {2, 22080}, {7, 22120}, {4, 22240}, {7, 22280}, {1, 22520}, {0, 22640}, {7, 22880},
        {2, 23080}, {3, 23160}, {2, 23280}, {3, 23400}, {4, 23520}, {0, 23640}, {2, 23680}, {3, 23840}, {7, 23960},
        {2, 24120}, {1, 24160}, {3, 24200}, {4, 24360}, {0, 24400}, {6, 24440}, {2, 24640}, {0, 24720},
};
#define WoodmanMonologueLength  countof(WoodmanMonologue)
const Gesture_t WoodmanSmile[] = {
        {0, 0},     // начальный момент времени
        {1, 1100}, {2, 1200}, {3, 1350}, {4, 1550}, {5, 1800}, {6, 2100}, {7, 2450},
};
#define WoodmanSmileLength  countof(WoodmanSmile)

static ColorWS_t Brightness(ColorWS_t AColor, uint8_t APercent) {
    ColorWS_t Result;
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
    const Gesture_t *PGesture;
    uint8_t GestureLength;
    bool BacklightOn = false;

    WoodmanState_t State = wsExpectation;
    thread_t *IPAppThd;

public:
    void BacklightON() { Backlight.SetHi(); BacklightOn = true; }
    void BacklightOFF() { Backlight.SetLo(); BacklightOn = false; }
    bool BacklightIsOn() { return BacklightOn; }
    void TunnelLightingON() { TunnelLighting.StartOrContinue(lsqFadeIn); }
    void TunnelLightingOFF() { TunnelLighting.StartOrContinue(lsqFadeOut); }
    void HeadUp() { Head.SetLo(); }
    void HeartBlinkON() { HeartBlinkTmr.StartOrRestart(); }
    void EyeON_and_HeartBlinkOFF() {
        HeartBlinkTmr.Stop();
        for (uint8_t i=HeartBeginIndex; i<=HeartEndIndex; i++)
            LedWs.ICurrentClr[i] = HeartColor;
        LedWs.ICurrentClr[EyeLeftIndex] = Brightness(EyeColor, EyeBrightness);
        LedWs.ICurrentClr[EyeRightIndex] = Brightness(EyeColor, EyeBrightness);
        LedWs.SetCurrentColors();
    }
    void ToWink() { chEvtSignal(PThread, WM_EVT_ToWink); }
    void OpenDoor() { DoorK2K3.SetLo(); State = wsDoorOpened; }
    void StartGesture(const Gesture_t *APGesture, const uint8_t AGestureLength) {
        chSysLock();
        PGesture = APGesture;
//        PGesture = &WoodmanMonologue[0];
        GestureLength = AGestureLength;
        chEvtSignalI(PThread, WM_EVT_GestureProcessing);
        chSysUnlock();
    }
    void SignalToHandcar() { chEvtSignal(PThread, WM_EVT_SignalToHandcar); }

    void Pause_MS(systime_t ATime_MS) {
        TmrWait.StartOrRestart(MS2ST(ATime_MS));
    }

    void SignalEvt(uint32_t EvtMsk) {
        chSysLock();
        chEvtSignalI(PThread, EvtMsk);
        chSysUnlock();
    }
//    void RegisterAppThd(thread_t *PThd) { IPAppThd = PThd; }

    WoodmanState_t GetState() { return State; }
    void SetState(WoodmanState_t AState) {State = AState;}

    void DefaultState() {
        State = wsExpectation;
        BacklightOFF();     // отключить подсветку дровосека
        TunnelLightingOFF();// погасить подсветку туннеля
        Head.SetHi();       // замагнитить голову
        DoorK2K3.SetHi();   // замагнитить дверь на выход
        HandcarSignal.SetLo();  // погасить сигнал на дрезину (про запас)
        TmrWait.Stop();
        HeartBlinkTmr.Stop();
        GestureProcess_Tmr.Stop();
        for (uint8_t i=0; i < LED_CNT; i++)
            LedWs.ICurrentClr[i] = sclBlack;    // погосить подсветку сердца, рта и глаз
        LedWs.SetCurrentColors();
    }

    // Inner use
    thread_t *PThread;
    void Init();
    void ITask();
};

extern Woodman_t Woodman;

