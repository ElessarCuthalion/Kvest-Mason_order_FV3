/*
 * File:   main.cpp
 * Author: Elessar
 * Project: MasonOrder
 *
 * Created on May 27, 2016, 6:37 PM
 */

#include "main.h"
#include "SimpleSensors.h"
#include "buttons.h"
#include "kl_adc.h"
//#include "led.h"
//#include "Sequences.h"
#include "sound.h"
#include "Soundlist.h"
#include "Woodman.h"


#if 1 // ======================== Variables and defines ========================
App_t App;
SndList_t SndList;
PinInput_t ExternalPWR{ExternalPWR_Pin};


enum AppState_t {
    asStandby, asGame,
};
AppState_t State = asStandby;

//void BtnHandler(BtnEvt_t BtnEvt, uint8_t BtnID);
void BtnHandler(BtnEvt_t BtnEvt);
//void LoadSettings(const char* FileName);

#endif

// =============================== Main ========================================

int main() {
#if 1  // ==== Init ====
    // ==== Setup clock ====
    Clk.UpdateFreqValues();
    uint8_t ClkResult = retvFail;
    Clk.SetupFlashLatency(24);  // Setup Flash Latency for clock in MHz
    // 12 MHz/6 = 2; 2*192 = 384; 384/8 = 48 (preAHB divider); 384/8 = 48 (USB clock)
    Clk.SetupPLLDividers(6, 192, pllSysDiv8, 8);
    // 48/4 = 12 MHz core clock. APB1 & APB2 clock derive on AHB clock
    Clk.SetupBusDividers(ahbDiv4, apbDiv1, apbDiv1);    // for 24MHz work: ahbDiv2, apbDiv1, apbDiv1
    if((ClkResult = Clk.SwitchToPLL()) == 0) Clk.HSIDisable();
    Clk.UpdateFreqValues();

    // ==== Init OS ====
    halInit();
    chSysInit();
    App.InitThread();

    // ==== Init Hard & Soft ====
    Uart.Init(115200, UART_GPIO, UART_TX_PIN, UART_GPIO, UART_RX_PIN);
    Uart.Printf("\r%S %S\r", APP_NAME, BUILD_TIME);
    Clk.PrintFreqs();
    // Report problem with clock if any
    if(ClkResult) Uart.Printf("Clock failure\r");

    // Setup inputs
    SimpleSensors::Init();
    // Setup outputs

    chThdSleepMilliseconds(100);    // Let power to stabilize

    // Random
    Random::TrueInit();

    // SD
    SD.Init();      // No power delay

    // Sound
    Sound.Init();
    Sound.SetupSeqEndEvt(EVT_PLAY_ENDS);
//    Sound.Ch1_ON();
//    Sound.SetVolume(WoodmanMonologue_VolLevel);

//    LoadSettings("Settings.ini");
    if (ExternalPWR.IsHi()) App.SignalEvt(EVT_USB_CONNECTED);

    // USB related
    MassStorage.Init();

    // LEDs
//    LedWs.Init();
//    LedWs.ISetCurrentColors();

    // Timers

    // Game
    State = asStandby;
    Woodman.Init();
    Woodman.DefaultState();

#endif
    // ==== Main cycle ====
    App.ITask();
}

#if 0
void LoadSettings(const char* SettingsFileName) {
    // Load Sound Settings
    uint8_t VolLevel = 0;
    if (iniRead(SettingsFileName, "Game", "SndVolume", &VolLevel) == retvOk)
        Sound.SetVolume(VolLevel);
    else {
        Uart.Printf("  Sound <- Def VolLevel\r");
        Sound.SetVolume(DEF_VolLevel);
    }

    if (iniRead(SettingsFileName, "Game", "LEDbright", &LEDs_Bright) != retvOk) {
        Uart.Printf("  LEDs <- Def LEDbright\r");
        LEDs_Bright = DEF_LEDs_Bright;
    }

    uint32_t LockDelay;
    if (iniRead(SettingsFileName, "Game", "LockDelay", &LockDelay) == retvOk)
        TmrLockBTN.SetNewPeriod_ms(LockDelay);
    else {
        Uart.Printf("  Time <- Def LockDelay\r");
        TmrLockBTN.SetNewPeriod_ms(DEF_Lock_delay_MS);
    }

    if (iniReadString(SettingsFileName, "Game", "CallTrack", &CallFileName) != retvOk) {
        Uart.Printf("  CallTrack <- Def FileName\r");
        strcpy(CallFileName, DEF_CallTrack);
    }
}
#endif

__attribute__ ((__noreturn__))
void App_t::ITask() {
while(true) {
    eventmask_t EvtMsk = chEvtWaitAny(ALL_EVENTS);

    if(EvtMsk & EVT_WoodmanCameToLife) {
        Sound.ONChannel(WoodmanMonologue_Channel);
        Sound.SetVolume(WoodmanMonologue_VolLevel);
        Sound.Play(WoodmanMonologue_file);
        Woodman.StartGesture();
    }

    if(EvtMsk & EVT_PLAY_ENDS) {
//        if (!ExternalPWR.IsHi()) {
        switch(Woodman.GetState()) {
            case asHeartReturned:
                Sound.OFFChannel(WoodmanMonologue_Channel);
                Woodman.OpenDoor();
                Woodman.SignalToHandcar();
            break;
            default: break;
        }
//        }
    }

    if(EvtMsk & EVT_BUTTONS) {
        BtnEvtInfo_t EInfo;
        while(BtnGetEvt(&EInfo) == retvOk) BtnHandler(EInfo.Type);
//        while(BtnGetEvt(&EInfo) == retvOk) BtnHandler(EInfo.Type, EInfo.BtnID);
    }

 // ==== USB connected/disconnected ====
    if(EvtMsk & EVT_USB_CONNECTED) {
        Sound.Stop();
        chSysLock();
        Clk.SetFreq48Mhz();
        chSysUnlock();
        Usb.Init();
        chThdSleepMilliseconds(540);
        Usb.Connect();
        Uart.Printf("Usb On\r");
        Clk.PrintFreqs();
    }
    if(EvtMsk & EVT_USB_DISCONNECTED) {
        Usb.Shutdown();
        MassStorage.Reset();
        chSysLock();
        Clk.SetFreq12Mhz();
        chSysUnlock();
        Uart.Printf("Usb Off\r");
        Clk.PrintFreqs();
    }

    if(EvtMsk & EVT_UART_NEW_CMD) {
        OnCmd(&Uart);
        Uart.SignalCmdProcessed();
    }

    } // while true
} // App_t::ITask()

//void BtnHandler(BtnEvt_t BtnEvt, uint8_t BtnID) {
void BtnHandler(BtnEvt_t BtnEvt) {
//    if(BtnEvt == beShortPress) Uart.Printf("Btn %u Short\r", BtnID);
//    if(BtnEvt == beLongPress)  Uart.Printf("Btn %u Long\r", BtnID);
//    if(BtnEvt == beRelease)    Uart.Printf("Btn %u Release\r", BtnID);
//    if(BtnEvt == beRepeat)    Uart.Printf("Btn %u Repeat\r", BtnID);
//    if(BtnEvt == beClick)      Uart.Printf("Btn %u Click\r", BtnID);
//    if(BtnEvt == beDoubleClick)Uart.Printf("Btn %u DoubleClick\r", BtnID);

    if (BtnEvt == beShortPress) {
        if (!Woodman.BacklightIsOn() )
            Woodman.BacklightON();
        else {
            Woodman.DefaultState();
        }
//        switch(Woodman.GetState()) {
//            case beShortPress:
//                break;
//            default: break;
//        }
    }
}


// Snsors
void Process5VSns(PinSnsState_t *PState, uint32_t Len) {
//    Uart.Printf("  %S\r", __FUNCTION__);
    if(PState[0] == pssRising) App.SignalEvt(EVT_USB_CONNECTED);
    else if(PState[0] == pssFalling) App.SignalEvt(EVT_USB_DISCONNECTED);
}
// for Woodman
void ProcessHandcarStartSns(PinSnsState_t *PState, uint32_t Len) {
    if(PState[0] == pssFalling) Woodman.SignalEvt(WM_EVT_HandcarParked);
}
void ProcessHandcarCenterSns(PinSnsState_t *PState, uint32_t Len) {
    if(PState[0] == pssFalling) Woodman.SignalEvt(WM_EVT_HandcarInTransit);
}
void ProcessHeartSns(PinSnsState_t *PState, uint32_t Len) {
    if(PState[0] == pssFalling) Woodman.SignalEvt(WM_EVT_HeartReturn);
}

#if UART_RX_ENABLED // ================= Command processing ====================
void App_t::OnCmd(Shell_t *PShell) {
    Cmd_t *PCmd = &PShell->Cmd;
    __attribute__((unused)) int32_t Data = 0;  // May be unused in some configurations
//    Uart.Printf("\r New Cmd: %S\r", PCmd->Name);
    // Handle command
    if(PCmd->NameIs("Ping")) {
        PShell->Ack(retvOk);
    }
    else if(PCmd->NameIs("Play")) {
        SndList.PlayRandomFileFromDir(PlayDir);
        PShell->Ack(retvOk);
    }

    else PShell->Ack(retvCmdUnknown);
}
#endif
