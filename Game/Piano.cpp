/*
 * Woodman.cpp
 *
 *  Created on: 22 îêò. 2018 ã.
 *      Author: Elessar
 */

#include "Piano.h"
#include "kl_lib.h"
#include "sound.h"

Piano_t Piano;


// =========================== Implementation ==================================
static THD_WORKING_AREA(waPianoThread, 256);
__attribute__((noreturn))
static void PianoThread(void *arg) {
    chRegSetThreadName("Piano");
    Piano.ITask();
}

__attribute__((noreturn))
void Piano_t::ITask() {
    while(true) {
        eventmask_t EvtMsk = chEvtWaitAny(ALL_EVENTS);

        if(EvtMsk & PI_EVT_ResetCode) {
            CurrentComb = 0;
        }
        if(EvtMsk & PI_EVT_SignslOfCodeOk) {
            CurrentComb = 0;
            TmrResetCode.Stop();
            chEvtSignal(IPAppThd, EVT_PianoCodeOk);
        }
    } // while true
}

void Piano_t::CodeProcessing(uint8_t KeyId) {
    TmrResetCode.StartOrRestart();
    TmrSignslEvtOfCodeOk.Stop();
    CurrentComb <<= 4;
    CurrentComb += KeyId;
    CurrentComb &= CodeMask;
    if (CurrentComb == PianoCode)
        TmrSignslEvtOfCodeOk.StartOrRestart();
    Uart.Printf("ÑurrentÑomb: %X\r", CurrentComb);
}


// ================================ Inner use ==================================

void Piano_t::Init() {
    // ==== GPIO init ====
    Cupboard.Init();
    Lighting.Init();
    // LEDs

    IPAppThd = chThdGetSelfX();
    // ==== DREQ IRQ ====
//    IDreq.Init(ttRising);
    // ==== Thread ====
    PThread = chThdCreateStatic(waPianoThread, sizeof(waPianoThread), NORMALPRIO, (tfunc_t)PianoThread, NULL);
    TmrResetCode.Init(PThread);
    TmrSignslEvtOfCodeOk.Init(PThread);
}
