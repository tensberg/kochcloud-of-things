#pragma once

#include <stdint.h>
#include <ArduinoJson.h>

#include "shared/messages/stromzaehler.h"
#include "shared/messages/zisternensensor.h"

class Device {
    public:
        Device(const uint8_t* macAddress, const char* topic);
        virtual void receiveMessage(uint8_t *incomingData, uint8_t len) = 0;
        virtual void handleReceivedMessage() = 0;
        const char* topic;
        const uint8_t* macAddress;
};

template <class T>
class MessageDevice: public Device {
    public:
        MessageDevice(const uint8_t* macAddress, const char* topic) : Device(macAddress, topic) {};
        void receiveMessage(uint8_t *incomingData, uint8_t len);
        void handleReceivedMessage();
    
    protected:
        virtual void createJson(JsonDocument& json) = 0;
        T message;

    private:
        boolean messageReceived;
};

class StromzaehlerDevice : public MessageDevice<StromzaehlerMessage> {
    public:
        StromzaehlerDevice(const uint8_t* macAddress, const char* topic) : MessageDevice(macAddress, topic) {};

    protected:
        void createJson(JsonDocument& json);
};

class ZisternensensorDevice: public MessageDevice<ZisternensensorMessage> {
    public:
        ZisternensensorDevice(const uint8_t* macAddress, const char* topic) : MessageDevice(macAddress, topic) {};

    protected:
        void createJson(JsonDocument& json);
};