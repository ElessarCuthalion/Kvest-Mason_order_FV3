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

#define WoodmanMonologue_file   "WoodmanMonologue.mp3"

enum TmrWoodmanState {wsPause, wsHandcarSignal};

// Event mask
#define WM_EVT_PauseTimeOut     EVENT_MASK(1)


class Woodman_t {
private:
    PinOutput_t Backlight {PwPort3_out};
    PinOutput_t Head {PwPort1_out};
    PinOutput_t DoorK2K3 {PwPort2_out};
    PinOutput_t HandcarSignal {PwPort4_out};
    PinInput_t Heart_pin {Port2_in};
    TmrKL_t WM_TmrWait { WM_EVT_PauseTimeOut, tktOneShot };
    thread_t *IPAppThd;

public:
    void BacklightON() { Backlight.SetHi(); }
    void HeadUp() { Head.SetLo(); }
//    void HeartBlinkON() {};
//    void HeartBlinkOFF() {};
    void OpenDoor() { DoorK2K3.SetLo(); };
    void SignalToHandcar() {
        HandcarSignal.SetHi();
        chThdSleepMilliseconds(5000);
        HandcarSignal.SetLo();
    }

    void Pause_MS(systime_t ATime_MS) {
        WM_TmrWait.StartOrRestart(MS2ST(ATime_MS));
    }

    bool IsNoHeart() { return !Heart_pin.IsHi(); }

//    void RegisterAppThd(thread_t *PThd) { IPAppThd = PThd; }

    void DefaultState() {
        Backlight.SetLo();  // отключить подсветку
        Head.SetHi();       // замагнитить голову
        DoorK2K3.SetHi();   // замагнитить дверь на выход
//        HeartBlinkOFF();    // погосить подсветку сердца, рта и глаз
        HandcarSignal.SetLo();  // погасить сигнал на дрезину (про запас)
    }

    // Inner use
    thread_t *PThread;
    void Init();
    void ITask();
};

extern Woodman_t Woodman;

