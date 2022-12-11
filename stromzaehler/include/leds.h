#pragma once

void initLeds();

void blinkLedHaushalt(unsigned int delayMs);

void blinkLedWaerme(unsigned int delayMs);

void ledSignal(bool ledHaushaltHigh, bool ledWaermeHigh, unsigned int loops, unsigned int delayOnMs, unsigned int delayOffMs);