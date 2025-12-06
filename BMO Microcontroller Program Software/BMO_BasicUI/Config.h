#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "Goldelox_Serial_4DLib.h"
#include "Goldelox_Const4D.h"

// BMO colors
#define BMO_SCREEN_COLOR   0xAFE5  // mint green
#define BMO_EYE_COLOR      BLACK
#define BMO_MOUTH_COLOR    BLACK
#define TURQUOISE 0x07FF

// UART pins (uLCD)
#define LCD_TX    20
#define LCD_RX    21
#define LCD_RESET 4

// Nav switch pins
#define NAV_UP     11
#define NAV_DOWN   10
#define NAV_LEFT   1
#define NAV_RIGHT  2
#define NAV_CENTER 5

// RGB LED pins (common-anode RGB)
#define LED_R_PIN   23
#define LED_G_PIN   3
#define LED_B_PIN   8

// Speaker pin (PWM-capable)
#define SPEAKER_PIN 19

// Application states
enum AppState {
    STATE_MAIN_MENU = 0,
    STATE_SETTINGS_MENU,
    STATE_GAMES_MENU,
    STATE_GAME_SNAKE
};

// Global stats
extern int energy;
extern int happiness;

// Main Menu
extern int menuIndex;
extern const int MENU_ITEMS;
extern const char* menuOptions[];

// Settings Menu
extern const int SETTINGS_ITEMS;
extern const char* settingsOptions[];

// Games Menu
extern const int GAMES_ITEMS;
extern const char* gamesOptions[];

// App state variable
extern AppState appState;

// Display serial
extern HardwareSerial DisplaySerial;
extern Goldelox_Serial_4DLib Display;

// Clamp helper
int clamp(int v);

#endif
