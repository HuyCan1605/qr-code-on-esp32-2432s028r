#include "net.h"
#include "config.h"
#include "state.h"

#include <WiFi.h>
#include <ArduinoJson.h>

// "static" make internal access within the net.cpp file
WiFiClient espClient;
static PubSubClient mqttClient(espClient);

struct NetState {
    uint32_t next_wifi_try = 0;
    uint32_t next_mqtt_try = 0;
    uint32_t mqtt_backoff = cfg::MQTT_RETRY_MS_MIN;
    bool mqtt_subscribed = false;
} N;

static inline uint32_t clamp_u32(uint32_t v, u_int32_t lo, uint32_t hi){
    if(v < lo) return lo;
    if(v > hi) return hi;
    return v;
}

static void formatAmount(uint32_t value, char* out, size_t outSize){
    char buf[20];
    snprintf(buf, sizeof(buf), "%lu", (unsigned long)value);

    int len = (int)strlen(buf);
    int dotCount = (len > 0) ? ((len - 1) / 3) : 0;
    int newLen = len + dotCount;

    if((size_t)newLen + 1 > outSize){
        snprintf(out, outSize, "%lu", (unsigned long)value);
        return;
    }

    out[newLen] = '\0';
    int i = len -1, j = newLen = 1, c = 0;
    while(i >= 0){
        if(c == 3) {
            out[j--] = '.';
            c = 0;
        }
        out[j--] = buf[i--];
        c++;
    }
    
}



static void mqttCallback(char* topic, byte* payload, unsigned int length){
    if(length == 0) return;

    static char msg[768]; //???????
    if(length >= sizeof(msg)) return;
    memcpy(msg, payload, length); //?????
    msg[length] = '\0';

    if(strcmp(topic, cfg:: TOPIC_SMS) == 0){
        StaticJsonDocument<256> doc;
        if(deserializeJson(doc, msg)) return;

        const char* merchantSMS = doc["MerchantId"] | "";
        if(strcasecmp(cfg::MERCHANT_ID, merchantSMS) != 0) return;

        S.newStatus = doc["Payload"]["NewStatus"] | 0;
        const char* bill = doc["Payload"]["BillNumber"] | "";
        strlcpy(S.sms_billNum, bill, sizeof(S.sms_billNum));
        return; //?????
    }

    if(strcmp(topic, cfg::TOPIC_QR) == 0){
        StaticJsonDocument<512> doc;
        if(deserializeJson(doc, msg)) return;

        const char* qrCode = doc["qr_code"] | "";
        const char* merchant = doc["merchant_id"] | "";
        uint32_t amount = doc["amount"] | 0;

        if(strcasecmp(merchant, cfg::MERCHANT_ID) != 0) return;
        if(!qrCode ||  !*qrCode) return; //??????

        const char* bill = doc["bill_number"] | "";
        strlcpy(S.qr_billNum, bill, sizeof(S.qr_billNum));

        formatAmount(amount, S.pendingAmount, sizeof(S.pendingAmount));
        strlcat(S.pendingAmount, "VND", sizeof(S.pendingAmount));

        strlcpy(S.pendingQr, qrCode, sizeof(S.pendingQr));
        S.needUpdateQR = true;
    }
}

void netInit(){
    WiFi.mode(WIFI_STA);
    mqttClient.setServer(cfg::MQTT_HOST, cfg::MQTT_PORT);
    mqttClient.setCallback(mqttCallback);
    N.next_mqtt_try = 0;
    N.next_wifi_try = 0;
}

void wifiLoop(){
    if(WiFi.isConnected()) return;
    uint32_t now = millis();
    if(now < N.next_wifi_try) return;

    WiFi.begin(cfg::WIFI_SSID, cfg::WIFI_PASS);
    N.next_wifi_try = now + cfg::WIFI_RETRY_MS;
}

static void mqttEnsureSubscribed(){
    if(N.mqtt_subscribed) return;
    mqttClient.subscribe(cfg::TOPIC_QR);
    mqttClient.subscribe(cfg::TOPIC_SMS);
    N.mqtt_subscribed = true;
}

void mqttLoop(){
    uint32_t now = millis();

    if(!WiFi.isConnected()){
        if (mqttClient.connected()) mqttClient.disconnect();
        N.mqtt_subscribed = false;
        return;
    }

    if(mqttClient.connected()){
        mqttEnsureSubscribed();

        mqttClient.loop();
        N.mqtt_backoff = cfg::MQTT_RETRY_MS_MIN;
        return;
    }

    if(now < N.next_mqtt_try) return;

    bool ok = mqttClient.connect(cfg::MQTT_CLIENT_ID, cfg::MQTT_USER, cfg::MQTT_PASS);
    if(ok){
        N.mqtt_subscribed = false;
        mqttEnsureSubscribed();
        N.mqtt_backoff = cfg::MQTT_RETRY_MS_MIN;
        N.next_mqtt_try = now + cfg::MQTT_RETRY_MS_MIN;
    } else{
        N.mqtt_backoff = clamp_u32(N.mqtt_backoff * 2, cfg::MQTT_RETRY_MS_MIN, cfg::MQTT_RETRY_MS_MAX);
        N.next_mqtt_try = now + N.mqtt_backoff;
    }
}

PubSubClient& mqtt() {return mqttClient;}