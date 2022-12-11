#pragma once

#include <SoftwareSerial.h>

typedef struct {
    double powerSumWh;
    double powerCurrentW;
} MeterReading;

class MeterReader {
    public:
        MeterReader(int rxPin);

        boolean read(int timeoutMs, MeterReading& reading);

    private:
        SoftwareSerial input;

        boolean processByte(unsigned char b, MeterReading& reading);
        void processListend(MeterReading& reading);
};