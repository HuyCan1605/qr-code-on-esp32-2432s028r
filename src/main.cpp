#include <Arduino.h>
#include "state.h"
#include "net.h"
#include "ui.h"
#include "logic.h"

AppState S;

void setup()
{
    Serial.begin(115200);
    netInit();
    uiInit();
}

void loop()
{
    wifiLoop();
    mqttLoop();
    uiLoop();
    logicLoop();
}