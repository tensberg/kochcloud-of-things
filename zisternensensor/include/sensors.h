#pragma once

#include "messages/zisternensensor.h"

void initSensors();
void measureData(ZisternensensorData &zsData);
void sensorsOff();