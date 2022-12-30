#include "Arduino.h"

#include "messages/stromzaehler.h"
#include "meterreader.h"
#include "iopins.h"
#include "leds.h"
#include "log.h"
#include "persistentstatus.h"
#include <messagesender.h>

#define METER_READ_TIMEOUT_MILLIS 5E3
#define SLEEP_BETWEEN_MEASUREMENTS_MICROS 60E6

// reader configuration
MeterReader readerHaushalt(RX_READER_HAUSHALT, VCC_READER_HAUSHALT);
MeterReader readerWaerme(RX_READER_WAERME, VCC_READER_WAERME);
StromzaehlerMessage message;

boolean continuousMode;

boolean readButton()
{
  pinMode(BUTTON_READ, INPUT);
  pinMode(BUTTON_VCC, OUTPUT);
  digitalWrite(BUTTON_VCC, HIGH);
  delay(50);
  boolean buttonPressed = digitalRead(BUTTON_READ) == HIGH;
  digitalWrite(BUTTON_VCC, LOW);
  return buttonPressed;
}

void signalContinuousMode()
{
  ledSignal(true, true, 3, 500, 500);
}

void updateReadingResult(StromzaehlerData &data, const MeterReading &reading, boolean readSuccess)
{
  data.readSuccess = readSuccess;
  if (readSuccess)
  {
    data.powerSumWh = reading.powerSumWh;
    data.powerCurrentW = reading.powerCurrentW;
  }
  else
  {
    data.powerSumWh = 0;
    data.powerCurrentW = 0;
  }
}

void handleInitEspNowSenderResult(int result) {
  switch (result) {
    case ESPNOW_OK:
      break;
    
    case ESPNOW_ERR_INIT_FAILED:
      ledSignal(true, false, 3, 250, 250);
      break;

    case ESPNOW_ERR_PAIRING_FAILED:
      ledSignal(false, true, 2, 500, 500);
      break;

    case ESPNOW_ERR_REGISTER_CB_FAILED:
      ledSignal(false, true, 3, 500, 500);
      break;

    default:
      ledSignal(false, true, 4, 500, 500);
  }

  if (result != ESPNOW_OK) {
      ESP.deepSleep(5000);
  }
}

void setup()
{
  Serial.begin(115200);
  log("Starting Stromzaehler");

  initPersistentDeviceStatus(message.status);
  int result = initEspNowSender();
  handleInitEspNowSenderResult(result);
  continuousMode = readButton();
  message.continuousMode = continuousMode;
  initLeds();
  if (continuousMode)
  {
    signalContinuousMode();
    readerHaushalt.setup(LED_STATUS_HAUSHALT);
    readerWaerme.setup(LED_STATUS_WAERME);
  }
}

void signalSuccess(int led, boolean success)
{
  if (!success)
  {
    digitalWrite(led, HIGH);
    delay(500);
    digitalWrite(led, LOW);
  }
}

void loop()
{
  boolean success;
  MeterReading reading;

  log("reading haushalt");
  success = readerHaushalt.read(METER_READ_TIMEOUT_MILLIS, reading);
  updateReadingResult(message.dataHaushalt, reading, success);
  signalSuccess(LED_STATUS_HAUSHALT, success);
  log("reading waerme");
  success = readerWaerme.read(METER_READ_TIMEOUT_MILLIS, reading);
  updateReadingResult(message.dataWaerme, reading, success);
  signalSuccess(LED_STATUS_WAERME, success);

  bool messageSent = sendDataEspNow((uint8_t *)&message, sizeof(message));

  updateAndSaveStatus(message.status, messageSent, continuousMode ? 0 : SLEEP_BETWEEN_MEASUREMENTS_MICROS);

  if (!continuousMode)
  {
    log("Sleep until next measurement");
    ESP.deepSleep(SLEEP_BETWEEN_MEASUREMENTS_MICROS);
  }
}
