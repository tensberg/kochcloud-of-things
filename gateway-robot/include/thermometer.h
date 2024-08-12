#pragma once

#include <Arduino.h>

#include "robotdisplay.h"

void drawThermometerBackground();
void drawTemperature(int16_t temperature, Location location);
