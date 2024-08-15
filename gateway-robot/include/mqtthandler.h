#pragma once

#include "mqttserial.h"

#define SUBSCRIPTIONS_LEN 4

extern MqttSubscription subscriptions[];

void publishDisplayState();
void publishLedState();