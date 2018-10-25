/*
 * Woodman.cpp
 *
 *  Created on: 22 окт. 2018 г.
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

        if(EvtMsk & EVT_HandcarInTransit) {
            Uart.Printf("EVT_HandcarInTransit\r");
            switch(State) {
                case asExpectation:
                    Woodman.BacklightON();
            //        Woodman.HeartBlinkON();
                    State = asWoodmanActive;
                break;
                case asDoorOpened:
                    chThdSleepMilliseconds(10000);
                    Woodman.DefaultState();
                    State = asExpectation;
                break;
                default: break;
            }
        }

        if(EvtMsk & EVT_HeartReturn) {
            Uart.Printf("EVT_HeartReturn\r");
            State = asHeartReturned;
            Woodman.HeartBlinkOFF();
            Woodman.HeadUp();
            TmrWait.StartOrRestart(MS2ST(HeadUp_TimeOut_MS));
        }
        if(EvtMsk & WM_EVT_PauseTimeOut) {
            Uart.Printf("EVT_WoodmanTimeOut\r");
            switch(State) {
                case asHeartReturned:
                    chEvtSignal(IPAppThd, EVT_WoodmanCameToLife);
                break;
                default: break;
            }
        }


        switch(EvtMsk) {

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
