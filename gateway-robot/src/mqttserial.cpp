#include "mqttserial.h"

#include <SerialTransfer.h>

#include "leds.h"

enum RobotToGatewayMessageType
{
    INIT,
    PUBLISH,
    SUBSCRIBE,
};

enum GatewayToRobotMessageType
{
    ACK,
    RESET,
    MESSAGE,
};

SerialTransfer transfer;

const MqttSubscription *mqttSerialSubscriptions;
int mqttSerialSubscriptionsLength;

bool ackReceived = false;

void sendMqttRequest(RobotToGatewayMessageType type, uint16_t sendSize)
{
    ackReceived = false;
    transfer.sendData(sendSize, type);
}

void mqttSerialTick()
{
    transfer.tick();
}

void mqttPublish(const char *topic, byte *payload, uint8_t length)
{
    uint8_t topicLength = strlen(topic);
    uint16_t sendSize = transfer.txObj(topicLength);
    sendSize = transfer.txObj(*topic, sendSize, topicLength);
    sendSize = transfer.txObj(length, sendSize);
    sendSize = transfer.txObj(*payload, sendSize, length);
    transfer.sendData(sendSize, PUBLISH);
    blinkLed(MQTT_STATUS_LED, 1);
}

void mqttPublish(const char *topic, JsonDocument& json) {
    byte payload[200];
    uint8_t payloadLength = serializeMsgPack(json, payload, sizeof(payload));
    mqttPublish(topic, payload, payloadLength);
}

void mqttSendCommand(RobotToGatewayMessageType messageType)
{
    transfer.txObj('0'); // dummy data because SerialTransfer packets must contain at least 1 byte
    transfer.sendData(1, messageType);
    blinkLed(MQTT_STATUS_LED, 1);
}

void mqttInit()
{
    mqttSendCommand(INIT);
}

void mqttSubscribe(const char *topic)
{
    uint8_t topicLength = strlen(topic);
    uint16_t sendSize = transfer.txObj(topicLength);
    sendSize = transfer.txObj(*topic, sendSize, topicLength);
    transfer.sendData(sendSize, SUBSCRIBE);
}

void handleAck()
{
    ackReceived = true;
}

void waitForAck()
{
    mqttSerialTick();
    while (!ackReceived)
    {
        delay(10);
        mqttSerialTick();
    }
}

void handleReset()
{
    ESP.reset();
}

void handleMessage()
{
    char topic[255];
    uint8_t topicLength;
    byte payload[255];
    uint8_t payloadLength;
    uint16_t rxSize = transfer.rxObj(topicLength);
    rxSize = transfer.rxObj(*topic, rxSize, topicLength);
    rxSize = transfer.rxObj(payloadLength, rxSize);
    rxSize = transfer.rxObj(*payload, rxSize, payloadLength);

    for (int i = 0; i < mqttSerialSubscriptionsLength; i++)
    {
        if (strncmp(topic, mqttSerialSubscriptions[i].topic, topicLength) == 0)
        {
            blinkLed(MQTT_STATUS_LED, 1);
            mqttSerialSubscriptions[i].handler(topic, payload, payloadLength);
            break;
        }

        blinkLed(MQTT_STATUS_LED, 2); // notify topic not found
    }
}

const functionPtr callbacks[] = {handleAck, handleReset, handleMessage};
const uint8_t callbacksLen = 3;

void initMqttSerial(const MqttSubscription _subscriptions[], uint8_t _subscriptionsLength)
{
    configST config;
    config.callbacks = callbacks;
    config.callbacksLen = callbacksLen;
    transfer.begin(Serial, config);
    mqttSerialSubscriptions = _subscriptions;
    mqttSerialSubscriptionsLength = _subscriptionsLength;
    mqttInit();
    for (int i = 0; i < mqttSerialSubscriptionsLength; i++)
    {
        mqttSubscribe(mqttSerialSubscriptions[i].topic);
    }
}
