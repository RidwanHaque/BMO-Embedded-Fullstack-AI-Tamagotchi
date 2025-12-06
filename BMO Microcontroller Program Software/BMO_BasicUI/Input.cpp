#include "Input.h"
#include "Config.h"
#include <Arduino.h>

// --- Latched inputs ---
// If user taps once, stays true for 150ms
unsigned long lastUp = 0;
unsigned long lastDown = 0;
unsigned long lastLeft = 0;
unsigned long lastRight = 0;
unsigned long lastCenter = 0;

const int LATCH_TIME = 150; // ms

void readNavLatched(bool &up, bool &down, bool &left, bool &right, bool &center) {

    if (!digitalRead(NAV_UP))     lastUp = millis();
    if (!digitalRead(NAV_DOWN))   lastDown = millis();
    if (!digitalRead(NAV_LEFT))   lastLeft = millis();
    if (!digitalRead(NAV_RIGHT))  lastRight = millis();
    if (!digitalRead(NAV_CENTER)) lastCenter = millis();

    unsigned long now = millis();

    up     = (now - lastUp    < LATCH_TIME);
    down   = (now - lastDown  < LATCH_TIME);
    left   = (now - lastLeft  < LATCH_TIME);
    right  = (now - lastRight < LATCH_TIME);
    center = (now - lastCenter< LATCH_TIME);
}
