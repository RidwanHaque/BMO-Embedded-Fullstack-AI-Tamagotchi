#include "LED.h"
#include "Config.h"

// Common-anode LED => LOW = ON, HIGH = OFF
static const bool COMMON_ANODE = true;

static void writeChannel(int pin, bool on) {
    if (COMMON_ANODE) {
        digitalWrite(pin, on ? LOW : HIGH);
    } else {
        digitalWrite(pin, on ? HIGH : LOW);
    }
}

void initLED() {
    pinMode(LED_R_PIN, OUTPUT);
    pinMode(LED_G_PIN, OUTPUT);
    pinMode(LED_B_PIN, OUTPUT);

    // Start with LED off
    ledSetRGB(false, false, false);

    // Initial mood based on current happiness
    ledSetMood(happiness);
}

void ledSetRGB(bool r, bool g, bool b) {
    writeChannel(LED_R_PIN, r);
    writeChannel(LED_G_PIN, g);
    writeChannel(LED_B_PIN, b);
}

void ledSetMood(int happiness) {
    // Very happy: teal / greenish
    if (happiness >= 80) {
        ledSetRGB(false, true, true);   // cyan-ish
    }
    // Happy / OK: soft blue
    else if (happiness >= 50) {
        ledSetRGB(false, false, true);  // blue
    }
    // Neutral / low: yellow
    else if (happiness >= 20) {
        ledSetRGB(true, true, false);   // yellow
    }
    // Very low: red
    else {
        ledSetRGB(true, false, false);  // red
    }
}

void ledFlashGood() {
    // Brief bright green flash
    ledSetRGB(false, true, false);
    delay(80);
    ledSetMood(happiness);
}

void ledFlashBad() {
    // Brief bright red flash
    ledSetRGB(true, false, false);
    delay(120);
    ledSetMood(happiness);
}
