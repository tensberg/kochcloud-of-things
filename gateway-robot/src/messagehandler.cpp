#include "messagehandler.h"

#include <ArduinoJson.h>

#include "log.h"
#include "mqttserial.h"

#define STROMZAEHLER_TOPIC "stromzaehler"
#define ZISTERNENSENSOR_TOPIC "zisternensensor"

Device::Device(const uint8_t* macAddress, const char* topic) {
    this->macAddress = macAddress;
    this->topic = topic;
}

template<class T>
void MessageDevice<T>::receiveMessage(uint8_t *incomingData, uint8_t len) {
    if (len != sizeof(T))
    {
        log("unexpected message length");
        return;
    }
    memcpy(&this->message, incomingData, len);
    this->messageReceived = true;
}

template<class T>
void MessageDevice<T>::handleReceivedMessage() {
    if (!this->messageReceived) {
        return;
    }

    StaticJsonDocument<200> json;
    this->createJson(json);
    mqttPublish(this->topic, json);
    this->messageReceived = false;
}

void appendStatus(JsonDocument& json, DeviceStatus status) {
    JsonObject jsonStatus = json.createNestedObject("status");
    jsonStatus["iteration"] = status.iteration;
    jsonStatus["activeTime"] = status.activeTimeS;
    jsonStatus["sleepTime"] = status.sleepTimeS;
    jsonStatus["sendFailed"] = status.sendFailedCount;
}

void appendIfSet(JsonObject& json, const char* key, double value) {
    if (value != NAN) {
        json[key] = value;
    }
}

void appendIfSet(JsonObject& json, const char* key, uint8_t value) {
    if (value > 0) {
        json[key] = value;
    }
}

void appendStromzaehlerData(JsonObject& json, const char* key, StromzaehlerData& data) {
    JsonObject jsonData = json.createNestedObject(key);
    if (data.readSuccess) {
        JsonObject power = json.createNestedObject("power");
        power["sum"] = data.powerSumWh;
        power["cur"] = data.powerCurrentW;
    } else {
        jsonData["readFailed"] = true;
    }
}

void StromzaehlerDevice::createJson(JsonDocument& json) {
    JsonObject data = json.createNestedObject("data");
    data["continuous"] = message.continuousMode;
    appendStromzaehlerData(data, "haushalt", message.dataHaushalt);
    appendStromzaehlerData(data, "waerme", message.dataWaerme);
    appendStatus(json, message.status);
}

void ZisternensensorDevice::createJson(JsonDocument& json) {
    JsonObject data = json.createNestedObject("data");
    appendIfSet(data, "temperature", message.data.temperature);
    appendIfSet(data, "humidity", message.data.humidity);
    appendIfSet(data, "pressure", message.data.pressure);
    appendIfSet(data, "distance", message.data.distance);
    appendStatus(json, message.status);
    mqttPublish("zisternensensor", json);
}