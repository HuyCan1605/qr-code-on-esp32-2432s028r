#pragma once
#include <lvgl.h>

void uiInit();
void uiLoop();

void uiShowHome();
void uiShowQr();
void uiShowSuccess();
void uiShowFailure();

lv_obj_t *uiQrObj();
lv_obj_t *uiPriceLabel();
lv_obj_t *uiTimeLabel();