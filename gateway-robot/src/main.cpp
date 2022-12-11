#include "Arduino.h"

#include <espnow.h>
#include <ESP8266WiFi.h>

#include "mqttserial.h"
#include "messagereceiver.h"

#define GATEWAY_ROBOT_TOPIC "gateway-robot"


void subscriptionHandler(const char* topic, const byte* payload, uint16_t length) {

}

#define SUBSCRIPTIONS_LEN 1

MqttSubscription subscriptions[] = { { GATEWAY_ROBOT_TOPIC, subscriptionHandler } };

void setup() 
{
    Serial.begin(115200);
    // wait before sending the first bytes to let the robot-gateway skip the garbage bytes after reset
    delay(1000); 

    initEspNowReceiver();
    initMqttSerial(subscriptions, SUBSCRIPTIONS_LEN);
}

void loop() 
{
    mqttSerialTick();
    espNowProcessReceivedMessages();
    delay(10);
}