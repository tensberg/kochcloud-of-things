#include "Arduino.h"

#include "sensors.h"

#include "iopins.h"
#include "log.h"

// Environment Sensor
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Distance Sensor
#include <NewPing.h>

// environment sensor
#define BME_I2C_ADDRESS 0x76
Adafruit_BME280 bme;

// distance sensor
// Define maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500 cm:
#define MAX_DISTANCE 400
NewPing sonar(DISTANCE_TRIG_PIN, DISTANCE_ECHO_PIN, MAX_DISTANCE);

void initBme()
{
    bool communication = bme.begin(BME_I2C_ADDRESS);
    if (!communication)
    {
        log("Could not find a valid BME280 sensor");
    }
    else
    {
        log("BME Environment Sensor Communication established!");
    }
}

void switchSensorsVcc(int value)
{
    digitalWrite(VCC_3V_PIN, value);
    digitalWrite(VCC_5V_PIN, LOW);
}

void sensorsOn()
{
    switchSensorsVcc(HIGH);
}

void sensorsOff()
{
    switchSensorsVcc(LOW);
}

void measureData(ZisternensensorData &zsData)
{
    zsData.temperature = bme.readTemperature();
    zsData.pressure = bme.readPressure() / 100.0F;
    zsData.humidity = bme.readHumidity();
    zsData.distance = 0; // sonar.convert_cm(sonar.ping_median());
}

void initSensors()
{
    pinMode(VCC_3V_PIN, OUTPUT);
    pinMode(VCC_5V_PIN, OUTPUT);
    sensorsOn();
    initBme();
}