#include <Arduino.h>
#include <sml.h>

#include "log.h"
#include "meterreader.h"

#define OBIS_POWER_SUM_WH 0
#define OBIS_POWER_CURRENT_W 1

#define SETUP_BYTES 150

typedef struct
{
  const unsigned char message[6];
} Obis;

const Obis OBIS[] = {
    {0x01, 0x00, 0x01, 0x08, 0x00, 0xff}, // powerSumWh
    {0x01, 0x00, 0x10, 0x07, 0x00, 0xff}  // powerCurrentW
};

MeterReader::MeterReader(int rxPin, int vccPin)
{
  pinMode(rxPin, INPUT);
  pinMode(vccPin, OUTPUT);
  digitalWrite(vccPin, LOW);
  this->vccPin = vccPin;
  this->input.begin(9600, SWSERIAL_8N1, rxPin, -1, false, 100, 0);
  this->input.enableRx(true);
  this->input.enableTx(false);
}

void MeterReader::setup(int ledPin)
{
  log("starting meter setup");
  digitalWrite(ledPin, HIGH);
  digitalWrite(vccPin, HIGH);

  this->input.listen();
  this->input.flush();

  int bytesRead = 0;

  while (bytesRead < SETUP_BYTES)
  {
    while (!this->input.available())
    {
      delay(10);
    }
    digitalWrite(ledPin, LOW);
    int b;
    while (bytesRead < SETUP_BYTES && (b = this->input.read()) != -1)
    {
      bytesRead++;
      logHex(b, bytesRead % 10 == 0);
    }
    digitalWrite(ledPin, HIGH);
  }

  digitalWrite(vccPin, LOW);
  digitalWrite(ledPin, LOW);
  log("meter setup finished");
}

boolean MeterReader::read(int timeoutMs, MeterReading &reading)
{
  bool messageRead = false;

  reading.powerCurrentW = 0;
  reading.powerSumWh = 0;

  digitalWrite(vccPin, HIGH);
  this->input.listen();
  this->input.flush();
  unsigned long timeoutEnd = millis() + timeoutMs;

  while (millis() < timeoutEnd)
  {
    int b = this->input.read();
    if (b == -1)
    {
      delay(10);
    }
    else
    {
      boolean messageComplete = processByte(b, reading);
      if (messageComplete)
      {
        messageRead = true;
        break;
      }
      yield();
    }
  }

  digitalWrite(vccPin, LOW);
  return messageRead;
}

boolean MeterReader::processByte(unsigned char b, MeterReading &reading)
{
  sml_states_t currentState = smlState(b);
  switch (currentState)
  {
  case SML_START:
    log(">>> Found start of message!");
    break;

  case SML_LISTEND:
    log(">>> Successfully received listend");
      this->processListend(reading);
    break;

  case SML_FINAL:
    log(">>> Successfully received a complete message!");
    return true;
  }

  return false;
}

void MeterReader::processListend(MeterReading &reading)
{
  if (smlOBISCheck(OBIS[OBIS_POWER_SUM_WH].message))
  {
    smlOBISWh(reading.powerSumWh);
  }
  else if (smlOBISCheck(OBIS[OBIS_POWER_CURRENT_W].message))
  {
    smlOBISW(reading.powerCurrentW);
  }
}
