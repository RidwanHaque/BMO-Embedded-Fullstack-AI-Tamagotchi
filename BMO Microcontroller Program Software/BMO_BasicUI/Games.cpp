#include "Games.h"
#include "Config.h"
#include "Snake.h"
#include "DisplayUI.h"
#include "Sound.h"
#include "LED.h"
#include <Arduino.h>

extern AppState appState;

void handleGamesMenuAction(int index) {

    if (index == 0) {
        // Start Snake
        appState = STATE_GAME_SNAKE;
        ledSetRGB(false, false, true);     // blue while in game
        soundMenuSelect();
        startSnakeGame();
    }

    else if (index == 1) {
        // Back to main
        appState = STATE_MAIN_MENU;
        menuIndex = 0;
        ledSetMood(happiness);             // restore mood color
        drawScreen();
    }
}
