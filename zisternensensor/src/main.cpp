#include "Arduino.h"

#include "iopins.h"

#include "messages/zisternensensor.h"
#include "log.h"
#include "persistentstatus.h"
#include "messagesender.h"
#include "display.h"
#include "serialstatus.h"
#include "sensors.h"

#include <Wire.h>

// pause between measurements
#define MICROS 1E6
#define SLEEP_BETWEEN_MEASUREMENTS_MICROS 5 * 60 * MICROS

ZisternensensorMessage zsMessage;

void setup()
{
  // init
  Serial.begin(115200);
  Serial.println("Zisternensensor starts");

  initEspNowSender();
  initPersistentDeviceStatus(zsMessage.status);
  printStatus(zsMessage.status);

  Wire.pins(I2C_SDA, I2C_SCL);
  initSensors();
  initDisplay();

  measureData(zsMessage.data);

  sensorsOff();
  displayData(zsMessage.data);
  printData(zsMessage.data);

  bool messageSent = sendDataEspNow((uint8_t *)&zsMessage, sizeof(zsMessage)) == ESPNOW_OK;

  // end iteration and sleep to next measurement
  updateAndSaveStatus(zsMessage.status, messageSent, SLEEP_BETWEEN_MEASUREMENTS_MICROS);

  log("Zisternensensor complete");
  ESP.deepSleep(SLEEP_BETWEEN_MEASUREMENTS_MICROS);
}

void loop()
{
  // we should never get here because deepSleep is ended by a reset
  Serial.println("looping");
}