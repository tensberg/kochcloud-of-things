#include "Arduino.h"

#include "messages/stromzaehler.h"
#include "meterreader.h"
#include "iopins.h"
#include "leds.h"
#include "meterdisplay.h"
#include "log.h"
#include "persistentstatus.h"
#include "messagesender.h"

#define SLEEP_BETWEEN_MEASUREMENTS_MICROS 10E6

// reader configuration
MeterReader readerHaushalt(RX_READER_HAUSHALT);
MeterReader readerWaerme(RX_READER_WAERME);
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

void setup()
{
  Serial.begin(115200);
  log("Starting Stromzaehler");

  initPersistentDeviceStatus(message.status);
  initDisplay();
  initEspNowSender();
  continuousMode = readButton();
  message.continuousMode = continuousMode;
  initLeds();
  if (continuousMode)
  {
    signalContinuousMode();
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

  message.status.iteration++;
  log("reading haushalt");
  success = readerHaushalt.read(1000, reading);
  updateReadingResult(message.dataHaushalt, reading, success);
  signalSuccess(LED_STATUS_HAUSHALT, success);
  log("reading waerme");
  success = readerWaerme.read(1000, reading);
  updateReadingResult(message.dataWaerme, reading, success);
  signalSuccess(LED_STATUS_WAERME, success);

  updateDisplay(message);
  bool messageSent = sendDataEspNow((uint8_t *)&message, sizeof(message));

  updateAndSaveStatus(message.status, messageSent, continuousMode ? 0 : SLEEP_BETWEEN_MEASUREMENTS_MICROS);

  if (!continuousMode)
  {
    log("Sleep until next measurement");
    ESP.deepSleep(SLEEP_BETWEEN_MEASUREMENTS_MICROS);
  }
}
