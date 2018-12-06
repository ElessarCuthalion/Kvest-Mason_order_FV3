/*
 * Piano.h
 *
 *  Created on: 22 окт. 2018 г.
 *      Author: Elessar
 */

#pragma once

#include "uart.h"
#include "kl_lib.h"
#include "board.h"
#include "evt_mask.h"
#include "Sequences.h"

// Sounds
#define Piano_Channel   SndCh1
#define Piano_VolLevel  250
const char PianoKeysFileNames[][9] = {
        {"Key1.mp3"},
        {"Key2.mp3"},
        {"Key3.mp3"},
        {"Key4.mp3"},
        {"Key5.mp3"},
        {"Key6.mp3"},
        {"Key7.mp3"},
};
#define Cupboard_Channel    SndCh2
#define Cupboard_VolLevel   210
#define OpenedCupboard_file "DoDo_song.mp3"

// Times
#define ResetCode_TimeOut_MS            10000
#define SignslEvtOfCodeOk_TimeOut_MS    3000

// Event mask
#define PI_EVT_ResetCode        EVENT_MASK(1)
#define PI_EVT_SignslOfCodeOk   EVENT_MASK(2)

#define PianoCode       0x1232167
#define CodeMask        0xFFFFFFF


typedef enum {
    psExpectation, psMelodyPlaying, psCupboardOpened,
} PianoState_t;

class Piano_t {
private:
    TmrKL_t TmrResetCode { MS2ST(ResetCode_TimeOut_MS), PI_EVT_ResetCode, tktOneShot };
    TmrKL_t TmrSignslEvtOfCodeOk { MS2ST(SignslEvtOfCodeOk_TimeOut_MS), PI_EVT_SignslOfCodeOk, tktOneShot };
    PinOutput_t CupboardLock {PwPort1_out};
    PinOutput_t CupboardBacklight {PwPort2_out};
    PinOutput_t CommonLight {PwPort3_out};
    uint32_t CurrentComb = 0;
    bool CommonLightON = false;

    thread_t *IPAppThd;
    PianoState_t State = psExpectation;

public:
    void CodeProcessing(uint8_t KeyId);
    void CommonLightingON() { CommonLight.SetHi(); CommonLightON = true; };
    void CommonLightingOFF() { CommonLight.SetLo(); CommonLightON = false; };
    bool CommonLightingIsOn() { return CommonLightON; }
    void OpenCupboard() { CupboardLock.SetLo(); State = psCupboardOpened; };
    void CloseCupboard() { CupboardLock.SetHi(); };
    void CupboardBacklightON() { CupboardBacklight.SetHi(); };
    void CupboardBacklightOFF() { CupboardBacklight.SetLo(); };

    void SignalEvt(uint32_t EvtMsk) {
        chSysLock();
        chEvtSignalI(PThread, EvtMsk);
        chSysUnlock();
    }
//    void RegisterAppThd(thread_t *PThd) { IPAppThd = PThd; }

    PianoState_t GetState() { return State; }
    void SetState(PianoState_t AState) {State = AState;}

    void DefaultState() {
        State = psExpectation;
        CommonLightingOFF();
        CloseCupboard();
        CupboardBacklightOFF();
    }

    // Inner use
    thread_t *PThread;
    void Init();
    void ITask();
};

extern Piano_t Piano;

