#pragma once
#include <PubSubClient.h>

void netInit();
void wifiLoop();
void mqttLoop();
PubSubClient& mqtt();