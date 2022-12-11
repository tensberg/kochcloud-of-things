#pragma once

#include <stdint.h>

#include "devicestatus.h"

typedef struct __attribute__((__packed__)) {
  double temperature;
  double pressure;
  double humidity;
  uint8_t distance;
} ZisternensensorData;

typedef struct __attribute__((__packed__)) {
  ZisternensensorData data;
  DeviceStatus status;
} ZisternensensorMessage;
