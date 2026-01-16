#pragma once
#include <stdint.h>

namespace cfg{
    constexpr uint16_t SCREEN_WIDTH = 240;
    constexpr uint16_t SCREEN_HEIGHT = 320;

    //WIFI
    constexpr const char* WIFI_SSID ="SEN TAI THU";
    constexpr const char* WIFI_PASS ="88888888";

    //MQTT
    constexpr const char* MQTT_CLIENT_ID = "qr_sen_long_bien";
    constexpr const char* MQTT_HOST = "mqtt.trananh.org";
    constexpr uint16_t     MQTT_PORT = 1883;
    constexpr const char* MQTT_USER = "huynq";
    constexpr const char* MQTT_PASS = "Trananh@2024";
    constexpr const char* TOPIC_QR  = "huynq1605/payment/qr";
    constexpr const char* TOPIC_SMS = "huynq1605/payment/qr";

    constexpr const char* MERCHANT_ID = "";

    //Timing
    constexpr uint32_t WIFI_RETRY_MS = 5000;
    constexpr uint32_t MQTT_RETRY_MS_MIN = 1500;
    constexpr uint32_t MQTT_RETRY_MS_MAX = 15000;
    constexpr uint32_t QR_SHOW_MS = 60000;
    constexpr uint32_t WAIT_AFTER_RESULT_MS = 10000;
    
    //LVGL scheduling
    constexpr uint32_t LV_TIMER_PERIOD_MS = 5;
}