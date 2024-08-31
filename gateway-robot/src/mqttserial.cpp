#include "mqttserial.h"

#include <SerialTransfer.h>
#include <pb_encode.h>
#include <pb_decode.h>

#include "leds.h"
#include "mqtthandler.h"
#include "robot_to_gateway_message.pb.h"
#include "gateway_to_robot_message.pb.h"

enum RobotToGatewayMessageType
{
    INIT,
    MQTT_PUBLISH
};

enum GatewayToRobotMessageType
{
    ACK,
    RESET,
    MESSAGE,
};

SerialTransfer transfer;

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

void mqttPublish(byte *payload, size_t length)
{
    uint16_t sendSize = transfer.txObj((uint16_t) length);
    sendSize = transfer.txObj(*payload, sendSize, length);
    transfer.sendData(sendSize, MQTT_PUBLISH);
    blinkLed(MQTT_STATUS_LED, 1);
}

void mqttPublish(robobuf_RobotToGatewayMessage& message) 
{
    byte payload[200];
    pb_ostream_t stream = pb_ostream_from_buffer(payload, sizeof(payload));
    bool status = pb_encode(&stream, robobuf_RobotToGatewayMessage_fields, &message);
    size_t payloadLength = stream.bytes_written;
    if (status) 
    {
        mqttPublish(payload, payloadLength);
    }
    else 
    {
        blinkLed(MQTT_STATUS_LED, 2);
    }
}

void sendCommand(RobotToGatewayMessageType messageType)
{
    transfer.txObj('0'); // dummy data because SerialTransfer packets must contain at least 1 byte
    transfer.sendData(1, messageType);
    blinkLed(MQTT_STATUS_LED, 1);
}

void sendInitialized()
{
    sendCommand(INIT);
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
    byte payload[512];
    uint16_t payloadLength;
    uint16_t rxSize = transfer.rxObj(payloadLength, 0, sizeof(uint16_t));
    rxSize = transfer.rxObj(*payload, rxSize, payloadLength);
    pb_istream_t istream = pb_istream_from_buffer(payload, payloadLength);
    robobuf_GatewayToRobotMessage message = robobuf_GatewayToRobotMessage_init_zero;
    pb_decode(&istream, &robobuf_GatewayToRobotMessage_msg, &message);

    bool messageHandled = handleMessage(message);
    blinkLed(MQTT_STATUS_LED, messageHandled ? 1 : 2);
}

const functionPtr callbacks[] = {handleAck, handleReset, handleMessage};
const uint8_t callbacksLen = 3;

void initMqttSerial()
{
    configST config;
    config.callbacks = callbacks;
    config.callbacksLen = callbacksLen;
    transfer.begin(Serial, config);
}
