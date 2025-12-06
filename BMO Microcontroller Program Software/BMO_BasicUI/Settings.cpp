#include "Settings.h"
#include "Config.h"
#include "DisplayUI.h"
#include "Stats.h"
#include "LED.h"
#include "RemoteHappiness.h"
#include <Arduino.h>
#include "esp_sleep.h"

// Wake on NAV_CENTER pin (GPIO 5)
#define WAKE_PIN GPIO_NUM_5

extern AppState appState;
extern int energy;
extern int happiness;

// Input latches from Input.cpp
extern unsigned long lastUp;
extern unsigned long lastDown;
extern unsigned long lastLeft;
extern unsigned long lastRight;
extern unsigned long lastCenter;

// initDisplay() lives in the main .ino
extern void initDisplay();

void enterLowPowerMode() {
    // Notify user
    Display.gfx_Cls();
    Display.txt_FGcolour(CYAN);
    Display.txt_MoveCursor(5, 0);
    Display.print("Entering Low Power...");
    delay(800);

    // Turn off display (put into reset)
    pinMode(LCD_RESET, OUTPUT);
    digitalWrite(LCD_RESET, LOW);

    // Configure wakeup source (EXT1: pin goes LOW)
    esp_sleep_enable_ext1_wakeup(1ULL << WAKE_PIN, ESP_EXT1_WAKEUP_ALL_LOW);

    // Enter light sleep
    esp_light_sleep_start();

    // ---------- WOKE UP HERE ----------

    // Reinit display
    initDisplay();

    // Reconfigure nav pins after wake
    pinMode(NAV_UP,     INPUT_PULLUP);
    pinMode(NAV_DOWN,   INPUT_PULLUP);
    pinMode(NAV_LEFT,   INPUT_PULLUP);
    pinMode(NAV_RIGHT,  INPUT_PULLUP);
    pinMode(NAV_CENTER, INPUT_PULLUP);

    // Re-init ESP-NOW receiver so shakes work again
    initRemoteHappiness();

    // Return to settings menu
    appState = STATE_SETTINGS_MENU;
    menuIndex = 0;

    // Energy refills after low power
    energy = 100;
    ledSetMood(happiness);

    // Clear any latched button presses after wake
    lastUp = lastDown = lastLeft = lastRight = lastCenter = 0;

    drawScreen();
}

void handleSettingsAction(int index) {
    if (index == 0) {
        // Enter real low power (hardware sleep)
        enterLowPowerMode();
    }
    else if (index == 1) {
        // Back to main menu
        appState = STATE_MAIN_MENU;
        menuIndex = 0;
        drawScreen();
    }
}
