#pragma once

#include "mqttserial.h"

#define SUBSCRIPTIONS_LEN 2

extern MqttSubscription subscriptions[];

void publishDisplayState();
