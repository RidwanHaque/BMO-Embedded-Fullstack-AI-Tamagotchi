#ifndef LED_H
#define LED_H

#include <Arduino.h>

void initLED();

// Set raw RGB (0 or 1) for each channel
void ledSetRGB(bool r, bool g, bool b);

// Map BMO happiness (0–100) to LED color
void ledSetMood(int happiness);

// Short green flash (good event)
void ledFlashGood();

// Short red flash (bad event / error / game over)
void ledFlashBad();

#endif
