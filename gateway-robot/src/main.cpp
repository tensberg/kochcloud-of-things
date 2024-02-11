#include "Arduino.h"

#include <espnow.h>
#include <ESP8266WiFi.h>
#include <Adafruit_ST7735.h>

#include "mqttserial.h"
#include "messagereceiver.h"
#include "leds.h"
#include "robotdisplay.h"

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

    initLeds();
    initDisplay();
    initEspNowReceiver();
    initMqttSerial(subscriptions, SUBSCRIPTIONS_LEN);

    for (int i = 1; i <= NUM_LEDS; i++)
    {
        setLed(i, true);
        delay(100);
    }

    drawImage();
    drawText("Hello, world!", ST77XX_BLACK);

    setBacklight(255);
}

void loop() 
{
    mqttSerialTick();
    espNowProcessReceivedMessages();
    delay(10);
}