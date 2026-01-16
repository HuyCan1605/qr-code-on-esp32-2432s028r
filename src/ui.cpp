#include "ui.h"
#include "config.h"
#include "state.h"

#include <TFT_eSPI.h>
#include "my_image.h"

static TFT_eSPI tft;

// LVGL buffers
static lv_color_t buf1[cfg::SCREEN_WIDTH * 40];
static lv_color_t buf2[cfg::SCREEN_WIDTH * 40];

static lv_obj_t *qr = nullptr;
static lv_obj_t *img = nullptr;
static lv_obj_t *success = nullptr;
static lv_obj_t *failure = nullptr;
static lv_obj_t *price_label = nullptr;
static lv_obj_t *time_label = nullptr;

static inline void ui_show(lv_obj_t *o)
{
    lv_obj_clear_flag(o, LV_OBJ_FLAG_HIDDEN);
}
static inline void ui_hide(lv_obj_t *o)
{
    lv_obj_add_flag(o, LV_OBJ_FLAG_HIDDEN);
}

static void flush_cb(lv_disp_t *disp, const lv_area_t *area, uint8_t *color_p)
{
    uint32_t w = (uint32_t)(area->x2 - area->x1 + 1);
    uint32_t h = (uint32_t)(area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)color_p, w * h, true);
    tft.endWrite();

    lv_display_flush_ready(disp);
}

static void ui_build()
{
    lv_color_t BG = lv_color_white();
    lv_color_t FG = lv_color_black();

    qr = lv_qrcode_create(lv_screen_active());
    lv_qrcode_set_size(qr, 180);
    lv_qrcode_set_dark_color(qr, FG);
    lv_qrcode_set_light_color(qr, BG);
    lv_obj_set_style_bg_color(qr, BG, 0);
    lv_obj_set_style_border_width(qr, 5, 0);
    lv_obj_center(qr);

    price_label = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(price_label, BG, 0);
    lv_obj_set_style_text_font(price_label, &lv_font_montserrat_20, 0);
    lv_obj_align(price_label, LV_ALIGN_TOP_MID, 0, 20);

    time_label = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(time_label, lv_color_hex(0x159E7A), 0);
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_18, 0);
    lv_obj_align(time_label, LV_ALIGN_BOTTOM_MID, 0, -20);

    LV_IMAGE_DECLARE(my_image);
    img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, &my_image);
    lv_obj_center(img);

    success = lv_label_create(lv_screen_active());
    lv_label_set_text(success, "Successful transaction");
    lv_obj_set_style_text_color(success, lv_color_hex(0xC94A87), 0);
    lv_obj_set_style_text_font(success, &lv_font_montserrat_20, 0);
    lv_obj_center(success);

    failure = lv_label_create(lv_screen_active());
    lv_label_set_text(failure, "Pending processing.....");
    lv_obj_set_style_text_color(failure, lv_color_hex(0x159E7A), 0);
    lv_obj_set_style_text_font(failure, &lv_font_montserrat_18, 0);
    lv_obj_center(failure);

    uiShowHome();
}

void uiInit(){
    lv_init();
    tft.begin();
    tft.setRotation(0);

    lv_display_t *disp = lv_display_create(cfg::SCREEN_WIDTH, cfg::SCREEN_HEIGHT);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_buffers(disp, buf1, buf2, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_0);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(lv_screen_active(), LV_OPA_COVER, LV_PART_MAIN);

    ui_build();

    S.last_tick_ms = millis();
    S.next_lv_run = S.last_tick_ms + cfg::LV_TIMER_PERIOD_MS;
}

void uiLoop(){
    uint32_t now = millis();

    uint32_t delta = now - S.last_tick_ms;
    S.last_tick_ms = now;
    lv_tick_inc(delta);

    if((int32_t)(now - S.next_lv_run) >= 0){
        lv_timer_handler();
        S.next_lv_run = now + cfg::LV_TIMER_PERIOD_MS;
    }
}

void uiShowHome(){
    ui_show(img);
    ui_hide(qr);
    ui_hide(success);
    ui_hide(failure);
    ui_hide(price_label);
    ui_hide(time_label);
}

void uiShowQr() {
  ui_show(price_label);
  ui_show(qr);
  ui_show(time_label);
  ui_hide(img);
  ui_hide(success);
  ui_hide(failure);
}
void uiShowSuccess() {
  ui_show(success);
  ui_hide(img);
  ui_hide(qr);
  ui_hide(failure);
  ui_hide(price_label);
  ui_hide(time_label);
}
void uiShowFailure() {
  ui_show(failure);
  ui_hide(img);
  ui_hide(qr);
  ui_hide(success);
  ui_hide(price_label);
  ui_hide(time_label);
}

lv_obj_t* uiQrObj() {return qr;}
lv_obj_t* uiPriceLabel() {return price_label;}
lv_obj_t* uiTimeLabel() {return time_label;}