/*
 * Piano.h
 *
 *  Created on: 22 ÓÍÚ. 2018 „.
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
#define Piano_VolLevel  240
#define Piano_Key1file  "Key1.mp3"
#define Piano_Key2file  "Key2.mp3"
#define Piano_Key3file  "Key3.mp3"
#define Piano_Key4file  "Key4.mp3"
#define Piano_Key5file  "Key5.mp3"
#define Piano_Key6file  "Key6.mp3"
#define Piano_Key7file  "Key7.mp3"
#define Cupboard_Channel    SndCh2
#define Cupboard_VolLevel   240
#define OpenedCupboard_file "OpenedCupboard.mp3"

// Times
#define ResetCode_TimeOut_MS            10000
#define SignslEvtOfCodeOk_TimeOut_MS    3000

// Event mask
#define PI_EVT_ResetCode        EVENT_MASK(1)
#define PI_EVT_SignslOfCodeOk   EVENT_MASK(2)

#define PianoCode       0x1232167
#define CodeMask        0b1111111


//enum TmrWoodmanState {wsPause, wsHandcarSignal};

typedef enum {
    psExpectation, psDoorOpened,
} PianoState_t;

class Piano_t {
private:
    TmrKL_t TmrResetCode { MS2ST(ResetCode_TimeOut_MS), PI_EVT_ResetCode, tktOneShot };
    TmrKL_t TmrSignslEvtOfCodeOk { MS2ST(SignslEvtOfCodeOk_TimeOut_MS), PI_EVT_SignslOfCodeOk, tktOneShot };
    uint32_t —urrent—omb = 0;

    thread_t *IPAppThd;
    PianoState_t State = psExpectation;

public:
    void CodeProcessing(uint8_t KeyId);

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

    }

    // Inner use
    thread_t *PThread;
    void Init();
    void ITask();
};

extern Piano_t Piano;

