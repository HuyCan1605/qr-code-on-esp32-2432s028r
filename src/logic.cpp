#include "logic.h"
#include "config.h"
#include "state.h"
#include "ui.h"

#include <lvgl.h>
#include <string.h>
#include <stdint.h>
#include <Arduino.h>

void logicLoop(){
    uint32_t now = millis();

    if (S.isShowingQR) {
    bool timeOut = (now > S.qrExpireAt);

    int32_t remain_ms = (int32_t)(S.qrExpireAt - now);
    if (remain_ms < 0) remain_ms = 0;
    uint32_t remain_s = (uint32_t)remain_ms / 1000;

    static uint32_t last_second = UINT32_MAX;
    if (last_second != remain_s) {
      last_second = remain_s;
      char buf[8];
      snprintf(buf, sizeof(buf), "%lus", (unsigned long)remain_s);
      lv_label_set_text(uiTimeLabel(), buf);
    }

    bool successOk = (S.newStatus == 3 && strcmp(S.qr_billNum, S.sms_billNum) == 0);
    if (successOk) {
      uiShowSuccess();
      S.isShowingQR = false;
      S.isWaiting = true;
      S.waitingStart = now;
      return;
    }

    if (timeOut) {
      uiShowFailure();
      S.isShowingQR = false;
      S.isWaiting = true;
      S.waitingStart = now;
      return;
    }
  }

  if (S.isWaiting && (now - S.waitingStart > cfg::WAIT_AFTER_RESULT_MS)) {
    uiShowHome();
    S.isWaiting = false;
  }
}