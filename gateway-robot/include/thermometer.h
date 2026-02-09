#pragma once

#include <Arduino.h>

#include "robotdisplay.h"

#define TEMPERATURE_UNDEFINED -1000

void initThermometer();
void drawThermometer();
void setTemperature(Location location, float temperature, float minTemperature = TEMPERATURE_UNDEFINED, float maxTemperature = TEMPERATURE_UNDEFINED, Trend trend = TREND_STABLE);
void drawTemperature(Location location);
