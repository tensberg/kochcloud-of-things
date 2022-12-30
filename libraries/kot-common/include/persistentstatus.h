#pragma once

#include <RTCMemory.h>

#include "devicestatus.h"

void initPersistentDeviceStatus(DeviceStatus &deviceStatus);
void updateAndSaveStatus(DeviceStatus &deviceStatus, boolean lastSendSuccess, unsigned long lastSleepTimeMicros);