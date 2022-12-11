#pragma once

#include "Arduino.h"

#include <ArduinoJson.h>

typedef void mqtt_subscribe_callback(const char* topic, const byte* payload, uint16_t length);

typedef struct {
    const char* topic;
    mqtt_subscribe_callback* handler;
} MqttSubscription;

void initMqttSerial(const MqttSubscription subscriptions[], uint8_t subscriptionsLength);

void mqttSerialTick();

void mqttPublish(const char* topic, byte* payload, uint16_t length);

void mqttPublish(const char* topic, JsonDocument& payload);



