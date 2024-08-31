#include "messagehandler.h"

#include "log.h"
#include "mqttserial.h"

#include "robot_to_gateway_message.pb.h"

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

    robobuf_RobotToGatewayMessage message = robobuf_RobotToGatewayMessage_init_default;
    this->createMqttMessage(message);
    mqttPublish(message);
    this->messageReceived = false;
}

void setStatus(robobuf_DeviceStatus& mqttStatus, bool& hasStatus, DeviceStatus status) {
    hasStatus = true;
    mqttStatus.iteration = status.iteration;
    mqttStatus.activeTimeS = status.activeTimeS;
    mqttStatus.sleepTimeS = status.sleepTimeS;
    mqttStatus.sendFailedCount = status.sendFailedCount;
}

void setIfAvailable(double& data, bool& hasData, double value) {
    hasData = value != NAN;
    if (hasData) 
    {
        data = value;
    }
}

void setIfAvailable(uint32_t& data, bool& hasData, uint8_t value) {
    hasData = value > 0;
    if (hasData) 
    {
        data = value;
    }
}

void setStromzaehlerData(robobuf_StromzaehlerData& mqttData, bool& hasData, StromzaehlerData& data) {
    hasData = data.readSuccess;

    if (hasData) 
    {
        mqttData.has_power = true;
        robobuf_PowerData& power = mqttData.power;
        power.sumWh = data.powerSumWh;
        power.currentW = data.powerCurrentW;
    }
}

void StromzaehlerDevice::createMqttMessage(robobuf_RobotToGatewayMessage& mqttMessage) {
    mqttMessage.which_message = robobuf_RobotToGatewayMessage_stromzaehler_state_tag;
    robobuf_StromzaehlerState& mqttState = mqttMessage.message.stromzaehler_state;
    setStatus(mqttState.status, mqttState.has_status, message.status);
    mqttState.has_config = true;
    mqttState.config.continuousMode = message.continuousMode;
    setStromzaehlerData(mqttState.haushalt, mqttState.has_haushalt, message.dataHaushalt);
    setStromzaehlerData(mqttState.waerme, mqttState.has_waerme, message.dataWaerme);
}

void ZisternensensorDevice::createMqttMessage(robobuf_RobotToGatewayMessage& mqttMessage) {
    mqttMessage.which_message = robobuf_RobotToGatewayMessage_zisternensensor_state_tag;
    robobuf_ZisternensensorState& mqttState = mqttMessage.message.zisternensensor_state;
    setStatus(mqttState.status, mqttState.has_status, message.status);
    mqttState.has_data = true;
    robobuf_ZisternensensorData& mqttData = mqttState.data;
    setIfAvailable(mqttData.temperature, mqttData.has_temperature, message.data.temperature);
    setIfAvailable(mqttData.humidity, mqttData.has_humidity, message.data.humidity);
    setIfAvailable(mqttData.pressure, mqttData.has_pressure, message.data.pressure);
    setIfAvailable(mqttData.distance, mqttData.has_distance, message.data.distance);
}