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

        if(EvtMsk & WM_EVT_HandcarInTransit) {
            Uart.Printf("EVT_HandcarInTransit\r");
            switch(State) {
                case asExpectation:
                    Woodman.BacklightON();
                    Woodman.HeartBlinkON();
                    State = asWoodmanActive;
                break;
                default: break;
            }
        }
        if(EvtMsk & WM_EVT_HandcarParked) {
            if (State == asDoorOpened) {
                Woodman.DefaultState();
                State = asExpectation;
            }
        }

        if(EvtMsk & WM_EVT_HeartReturn) {
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

        if(EvtMsk & WM_EVT_HeartBlinkTimeOut) {
            static bool HeartLit = false;
            for (uint8_t i=HeartBeginIndex; i<HeartEndIndex; i++)
                if (!HeartLit) LedWs.ICurrentClr[i] = Brightness(HeartColor, HeartBrightness);
                else LedWs.ICurrentClr[i] = sclBlack;
            LedWs.ISetCurrentColors();
            HeartLit = !HeartLit;
        }
        #define SmileCenterIndex    SmileBeginIndex + SmileWidth_LEDs
        if(EvtMsk & WM_EVT_GestureProcessing) {
            static uint8_t poss = 0;
            static systime_t PastTime = 0;
            for (uint8_t i = SmileCenterIndex; i < SmileWidth_LEDs; i++) {
                if (i <= SmileCenterIndex+GestureMonologue[poss].Level) {
                    LedWs.ICurrentClr[SmileBrightness+i] = Brightness(SmileColor, SmileBrightness);
                    LedWs.ICurrentClr[SmileBrightness-i-1] = Brightness(SmileColor, SmileBrightness);
                } else {
                    LedWs.ICurrentClr[SmileBrightness+i] = sclBlack;
                    LedWs.ICurrentClr[SmileBrightness-i-1] = sclBlack;
                };
            }
            GestureProcess_Tmr.StartOrRestart(MS2ST(GestureMonologue[poss].Position_MS - PastTime));
            LedWs.ISetCurrentColors();
            if (poss < GestureLimit) {
                poss ++;
                PastTime += GestureMonologue[poss].Position_MS;
            } else {
                poss = 0;
                PastTime = 0;
            }
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
    TunnelLighting.Init();
    TunnelLighting.SetBrightness(0);
//    TunnelLighting.SetPwmFrequencyHz(1000);
    TunnelLighting.StartOrContinue(lsqFadeIn);

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
    GestureProcess_Tmr.Init(PThread);
}
