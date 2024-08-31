#pragma once

#include "mqttserial.h"
#include "gateway_to_robot_message.pb.h"

bool handleMessage(robobuf_GatewayToRobotMessage& message);
void publishDisplayState();
void publishLedState();