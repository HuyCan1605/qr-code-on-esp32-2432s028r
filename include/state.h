#pragma once

#include <stdint.h>

struct AppState{
    char qr_billNum[11] = ""; // don't use string in order not to make fragment memory
    char sms_billNum[11] = "";

    char pendingQr[512] = {0};
    char pendingAmount[24] = {0};

    volatile bool needUpdateQR = false;
    bool isShowingQR = false;
    bool isWaiting = false;

    int newStatus = 0;

    //timing variables
    uint32_t qrExpireAt = 0;
    uint32_t waitingStart = 0;

    uint32_t last_tick_ms = 0;
    uint32_t next_lv_run = 0;
};

extern AppState S;