#pragma once

#include "devicestatus.h"

typedef struct __attribute__((__packed__)) {
    bool readSuccess;
    double powerSumWh;
    double powerCurrentW;
} StromzaehlerData;

typedef struct __attribute__((__packed__)) {
  StromzaehlerData dataHaushalt;
  StromzaehlerData dataWaerme;
  DeviceStatus status;
  bool continuousMode;
} StromzaehlerMessage;
