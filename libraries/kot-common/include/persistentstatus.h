#pragma once

#include "devicestatus.h"

void initPersistentDeviceStatus(DeviceStatus &deviceStatus);
void updateAndSaveStatus(DeviceStatus &deviceStatus, boolean lastSendSuccess, unsigned long lastSleepTimeMicros);