#pragma once

typedef struct __attribute__((__packed__)) {
  unsigned long iteration;
  unsigned long sleepTimeS;
  unsigned long activeTimeS;
  unsigned int activeTimeMicros;
  unsigned int sendSuccessCount;
  unsigned int sendFailedCount;
} DeviceStatus;
