#pragma once

#include "Arduino.h"

#include "robot_to_gateway_message.pb.h"

void initMqttSerial();

void sendInitialized();

void mqttSerialTick();

void mqttPublish(robobuf_RobotToGatewayMessage& message);
