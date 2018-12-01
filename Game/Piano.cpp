/*
 * Woodman.cpp
 *
 *  Created on: 22 ÓÍÚ. 2018 „.
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
            —urrent—omb = 0;
        }
        if(EvtMsk & PI_EVT_SignslOfCodeOk) {
            —urrent—omb = 0;
            TmrResetCode.Stop();
            chEvtSignal(IPAppThd, EVT_PianoCodeOk);
        }
    } // while true
}

void Piano_t::CodeProcessing(uint8_t KeyId) {
    TmrResetCode.StartOrRestart();
    TmrSignslEvtOfCodeOk.Stop();
    —urrent—omb <<= 4;
    —urrent—omb += KeyId;
    —urrent—omb &= CodeMask;
    if (—urrent—omb == PianoCode)
        TmrSignslEvtOfCodeOk.StartOrRestart();
    Uart.Printf("—urrent—omb: %X\r", —urrent—omb);
}


// ================================ Inner use ==================================

void Piano_t::Init() {
    // ==== GPIO init ====


    // LEDs

    // Sounds
    Sound.ONChannel(Piano_Channel);
    Sound.SetVolume(Piano_VolLevel);
    // ==== DREQ IRQ ====
//    IDreq.Init(ttRising);
    // ==== Thread ====
    PThread = chThdCreateStatic(waPianoThread, sizeof(waPianoThread), NORMALPRIO, (tfunc_t)PianoThread, NULL);
}
