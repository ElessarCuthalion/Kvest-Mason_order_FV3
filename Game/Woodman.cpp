/*
 * Woodman.cpp
 *
 *  Created on: 22 ���. 2018 �.
 *      Author: Elessar
 */

#include "Woodman.h"
#include "kl_lib.h"

Woodman_t Woodman;


// =========================== Implementation ==================================
static THD_WORKING_AREA(waWoodmanThread, 256);
__attribute__((noreturn))
static void WoodmanThread(void *arg) {
    chRegSetThreadName("Woodman");
    Woodman.ITask();
}

__attribute__((noreturn))
void Woodman_t::ITask() {
    while(true) {
        eventmask_t EvtMsk = chEvtWaitAny(ALL_EVENTS);

        switch(EvtMsk) {
            case WM_EVT_PauseTimeOut:
                chEvtSignal(IPAppThd, EVT_WoodmanTimeOut);
                break;

            case WM_EVT_HeartBlinkTimeOut:
                static bool HeartGlows = false;
                for (uint8_t i=HeartBeginIndex; i<HeartEndIndex; i++)
                    if (!HeartGlows) LedWs.ICurrentClr[i] = HeartColor;
                    else LedWs.ICurrentClr[i] = sclBlack;
                LedWs.ISetCurrentColors();
                HeartGlows = ~HeartGlows;
                break;

            case WM_EVT_WS_processTimeOut:

                break;

            default: break;
        }
    } // while true
}



// ================================ Inner use ==================================

void Woodman_t::Init() {
    // ==== GPIO init ====
    Backlight.Init();
    Backlight.SetHi();
    Head.Init();
    DoorK2K3.Init();
    HandcarSignal.Init();
//    PinSetupOut(VS_GPIO, VS_RST, omPushPull);
//    PinSetupAlterFunc(VS_GPIO, VS_SI,   omPushPull, pudNone, VS_AF);

    // LEDs
    LedWs.Init();
    LedWs.ISetCurrentColors();

    IPAppThd = chThdGetSelfX();
    // ==== DREQ IRQ ====
//    IDreq.Init(ttRising);
    // ==== Thread ====
    PThread = chThdCreateStatic(waWoodmanThread, sizeof(waWoodmanThread), NORMALPRIO, (tfunc_t)WoodmanThread, NULL);
    TmrWait.Init(PThread);
    HeartBlinkTmr.Init(PThread);
    WS_process_Tmr.Init(PThread);
}
