/*
 * Soundlist.h
 *
 *  Created on: 18 ���. 2015 �.
 *      Author: Kreyl
 */

#pragma once

#include "kl_lib.h"
#include "kl_sd.h"
#include "uart.h"

#define DIR_CNT_MAX     6   // Remember file number in every dir. Set to 1 if not required

struct DirList_t {
    uint32_t FilesCnt = 0;
    int32_t LastN = -1;
    char DirName[MAX_NAME_LEN] = "\0";
    bool IsEqual() { return false; }
    void Print() { Uart.Printf("Dir: %S; Cnt: %u; LastN: %d\r", DirName, FilesCnt, LastN); }
};

class SndList_t {
private:
    DirList_t DirList[DIR_CNT_MAX];
    int DirIndxInList(const char* DirName);
    int AddDirToList(const char* DirName);
    char Filename[MAX_NAME_LEN];    // to store name with path
    DIR Dir;
    FILINFO FileInfo;
    FRESULT CountFilesInDir(const char* DirName, uint32_t *PCnt);
public:
    void PlayRandomFileFromDir(const char* DirName);
    void DelayAndPlayRandomFileFromDir(systime_t Delay_MS, const char* DirName);
    FRESULT UpdateDir(const char* DirName) {
        int indx = DirIndxInList(DirName);
        if(indx == -1)
            indx = AddDirToList(DirName);
        else
            f_readdir(&Dir, &FileInfo);     // ��������� ���������� ������� ���������� ��� ������ ������� �� ���������
        DirList[indx].LastN = -1;
        return CountFilesInDir(DirName, &DirList[indx].FilesCnt);
    }
    int32_t GetTrackNumber(const char* DirName) {
        int indx = DirIndxInList(DirName);
        return DirList[indx].LastN;
    }
    void SetPreviousTrack(const char* DirName, int32_t N) {
        int indx = DirIndxInList(DirName);
        if(indx == -1) {
            indx = AddDirToList(DirName);
            FRESULT Rslt = CountFilesInDir(DirName, &DirList[indx].FilesCnt);
            if(Rslt != FR_OK) return;
        }
        DirList[indx].LastN = N;
    }
};
