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

        if(EvtMsk & WM_EVT_HandcarInTransit) {
            Uart.Printf("HandcarInTransit\r");
            switch(State) {
                case wsExpectation:
                    BacklightON();
                    HeartBlinkON();
                    TmrWait.StartOrRestart(MS2ST(TunnelLight_TimeOut_MS));   // �� ��������� ����� ������ ��������� � ������
                    State = wsWoodmanActive;
                break;
                default: break;
            }
        }
        if(EvtMsk & WM_EVT_HandcarParked) {
            Uart.Printf("HandcarParked\r");
//            if (State == wsDoorOpened) {
                DefaultState();
//            }
        }

        if(EvtMsk & WM_EVT_HeartReturn) {
            Uart.Printf("HeartReturn\r");
            State = wsHeartReturned;
            EyeON();
            HeartBlinkOFF();
            HeadUp();
            TmrWait.StartOrRestart(MS2ST(HeadUp_TimeOut_MS));   // (��������� ���� �������� �������� ������)
        }
        if(EvtMsk & WM_EVT_WaitTimeOut) {
            Uart.Printf("WoodmanTimeOut\r");
            switch(State) {
                case wsWoodmanActive:
                    TunnelLightingON();
                break;
                case wsHeartReturned:
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
            LedWs.SetCurrentColors();
            HeartLit = !HeartLit;
        }
        if(EvtMsk & WM_EVT_GestureProcessing) {
            #define SmileCenterIndex    SmileBeginIndex + SmileWidth_LEDs
            #define LasrTime            ST2MS(chVTGetSystemTime() - StartTime)
            static uint8_t poss = 0;
            static systime_t StartTime;
            chSysLock();
            if (poss == 0) StartTime = chVTGetSystemTimeX();
            for (uint8_t i = 0; i <= SmileWidth_LEDs; i++) {
                if (i+1 <= GestureMonologue[poss].Level) {
                    LedWs.ICurrentClr[SmileCenterIndex+i] = Brightness(SmileColor, SmileBrightness);
                    LedWs.ICurrentClr[SmileCenterIndex-i-1] = Brightness(SmileColor, SmileBrightness);
                } else {
                    LedWs.ICurrentClr[SmileCenterIndex+i] = sclBlack;
                    LedWs.ICurrentClr[SmileCenterIndex-i-1] = sclBlack;
                };
            }
            LedWs.ISetCurrentColors();
            chSysUnlock();
            if (poss < GestureLimit-1) {
                poss ++;
                if (GestureMonologue[poss].Position_MS > GestureMonologue[poss-1].Position_MS)
                    GestureProcess_Tmr.StartOrRestart( MS2ST(GestureMonologue[poss].Position_MS - LasrTime) );
                else Uart.Printf("Gesture ERROR Position: poss[%u]=%u; poss[%u]=%u\r", poss, GestureMonologue[poss].Position_MS, poss-1, GestureMonologue[poss-1].Position_MS);
            } else poss = 0;
//            Uart.Printf("LasrTime: %u, Position_MS: %u\r", LasrTime, GestureMonologue[poss-1].Position_MS );
//            Uart.Printf("Pause_MS: %u\r\r", GestureMonologue[poss].Position_MS-GestureMonologue[poss-1].Position_MS);
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
