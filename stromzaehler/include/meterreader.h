#pragma once

#include <SoftwareSerial.h>

typedef struct
{
    double powerSumWh;
    double feedInSumWh;
    double powerCurrentW;
    uint64_t measurementTimeMs;
} MeterReading;

class MeterReader
{
public:
    MeterReader(int rxPin, int vccPin);

    void setup(int ledPin);
    boolean read(unsigned long timeoutMs, MeterReading &reading);

private:
    int vccPin;
    SoftwareSerial input;

    boolean processByte(unsigned char b, MeterReading &reading);
    void processListend(MeterReading &reading);
};