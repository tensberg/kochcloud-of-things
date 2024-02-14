#pragma once

#include "mqttserial.h"

#define SUBSCRIPTIONS_LEN 3

extern MqttSubscription subscriptions[];

void publishDisplayState();
void publishLedState();