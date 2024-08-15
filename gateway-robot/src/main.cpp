#include "Arduino.h"

#include <espnow.h>
#include <ESP8266WiFi.h>
#include <TimeOut.h>

#include "mqttserial.h"
#include "mqtthandler.h"
#include "messagereceiver.h"
#include "leds.h"
#include "robotdisplay.h"
#include "thermometer.h"

void setup() 
{
    Serial.begin(115200);
    // wait before sending the first bytes to let the robot-gateway skip the garbage bytes after reset
    delay(1000); 

    initLeds();
    setLeds(STATUS_LEDS_ON);

    initDisplay();
    initThermometer();
    initEspNowReceiver();
    initMqttSerial(subscriptions, SUBSCRIPTIONS_LEN);

    drawEyes();
    drawThermometer();
    setBacklight(255);
    publishInitialized();
    publishDisplayState();
    publishLedState();
}

void loop() 
{
    TimeOut::handler();
    Interval::handler();
    mqttSerialTick();
    espNowProcessReceivedMessages();
    delay(10);
}