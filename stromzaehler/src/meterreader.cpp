#include <Arduino.h>
#include <sml.h>

#include "log.h"
#include "meterreader.h"

#define OBIS_POWER_SUM_WH 0
#define OBIS_POWER_CURRENT_W 1

typedef struct
{
  const unsigned char message[6];
} Obis;

const Obis OBIS[] = {
    {0x01, 0x00, 0x01, 0x08, 0x00, 0xff}, // powerSumWh
    {0x01, 0x00, 0x01, 0x08, 0x00, 0xff}  // powerCurrentW
};

MeterReader::MeterReader(int rxPin, int vccPin)
{
  pinMode(rxPin, INPUT);
  pinMode(vccPin, OUTPUT);
  digitalWrite(vccPin, LOW);
  this->vccPin = vccPin;
  this->input.begin(9600, SWSERIAL_8N1, rxPin, -1, false, 0, 95);
  this->input.enableRx(true);
  this->input.enableTx(false);
}

boolean MeterReader::read(int timeoutMs, MeterReading &reading)
{
  bool messageRead = false;

  digitalWrite(vccPin, HIGH);
  unsigned long timeoutEnd = millis() + timeoutMs;
  this->input.listen();

  while (millis() < timeoutEnd)
  {
    unsigned char b = this->input.read();
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
