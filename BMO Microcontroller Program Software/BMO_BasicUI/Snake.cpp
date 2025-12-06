#include "Snake.h"
#include "Config.h"
#include "DisplayUI.h"
#include "Input.h"
#include "LED.h"
#include "Sound.h"
#include <Arduino.h>

// ------------------------
// Grid & drawing settings
// ------------------------
// 15 columns * 7 px = 105 px, fits within 0..127
static const int GRID_W   = 18;
static const int GRID_H   = 15;
static const int CELL     = 7;
static const int OFFSET_Y = 26;

// Snake body (grid coordinates)
static int snakeX[128];
static int snakeY[128];
static int snakeLength;

// Direction
static int dirX = 1;
static int dirY = 0;

// Food
static int foodX, foodY;

// Timing
static unsigned long lastMove;
static const int SPEED = 180; // ms

// Game state
static bool gameOver = false;
static int  score = 0;
static int  highScore = 0;

// Incremental draw helpers
static int  prevTailX = -1;
static int  prevTailY = -1;
static bool firstStep = true;

extern AppState appState;

// Latched input timestamps from Input.cpp
extern unsigned long lastUp;
extern unsigned long lastDown;
extern unsigned long lastLeft;
extern unsigned long lastRight;
extern unsigned long lastCenter;

// From other modules
extern int happiness;
extern int menuIndex;

// drawBar is defined in DisplayUI.cpp
extern void drawBar(int x, int y, int w, int val, word color);

// -----------------------------------
// HUD (Score + High Score + Happiness)
// -----------------------------------
static void drawSnakeHUD() {
    // Clear the entire HUD/top area first
    Display.gfx_RectangleFilled(0, 0, 127, OFFSET_Y - 1, BLACK);

    Display.txt_FGcolour(WHITE);
    Display.txt_MoveCursor(0, 0);
    Display.print("SC:");
    Display.print(score);
    Display.print("  HI:");
    Display.print(highScore);

    // Happiness bar on the second text row
    Display.txt_MoveCursor(1, 0);
    Display.print("Happy:");
    drawBar(58, 10, 65, happiness, YELLOW);

    // Separator line just above playfield
    Display.gfx_Line(0, OFFSET_Y - 5, 127, OFFSET_Y - 5, WHITE);
}

// Update SC + HI and happiness bar (keep layout)
static void updateSnakeHUDValues() {
    Display.txt_FGcolour(WHITE);

    // Redraw the whole first HUD line so "SC:..  HI:.." stays intact
    Display.txt_MoveCursor(0, 0);
    Display.print("SC:");
    Display.print(score);
    Display.print("  HI:");
    Display.print(highScore);

    // Happiness bar only
    drawBar(58, 10, 65, happiness, YELLOW);
}


// -----------------------------------
// Full draw: Snake + Food (used once)
// -----------------------------------
static void drawSnakeGridFull() {
    // Clear only the gameplay area under HUD
    Display.gfx_RectangleFilled(0, OFFSET_Y, 127, 127, BLACK);

    // Snake
    for (int i = 0; i < snakeLength; i++) {
        int px = snakeX[i] * CELL;
        int py = snakeY[i] * CELL + OFFSET_Y;
        Display.gfx_RectangleFilled(px, py, px + CELL - 1, py + CELL - 1, GREEN);
    }

    // Food
    int fx = foodX * CELL;
    int fy = foodY * CELL + OFFSET_Y;
    Display.gfx_RectangleFilled(fx, fy, fx + CELL - 1, fy + CELL - 1, RED);
}

// -----------------------------------
// Incremental draw: only previous tail + head
// -----------------------------------
static void drawSnakeGrid() {
    // Erase previous tail cell if valid
    if (prevTailX >= 0 && prevTailY >= 0) {
        int tailPx = prevTailX * CELL;
        int tailPy = prevTailY * CELL + OFFSET_Y;
        Display.gfx_RectangleFilled(tailPx, tailPy,
                                    tailPx + CELL - 1, tailPy + CELL - 1, BLACK);
    }

    // Draw head cell
    int headPx = snakeX[0] * CELL;
    int headPy = snakeY[0] * CELL + OFFSET_Y;
    Display.gfx_RectangleFilled(headPx, headPy,
                                headPx + CELL - 1, headPy + CELL - 1, GREEN);
}

// -----------------------------------
// Game Over Screen
// -----------------------------------
static void drawSnakeGameOver() {
    Display.gfx_Cls();

    Display.txt_FGcolour(RED);
    Display.txt_MoveCursor(3, 0);
    Display.print("GAME OVER");

    Display.txt_MoveCursor(5, 0);
    Display.txt_FGcolour(WHITE);
    Display.print("Score: ");
    Display.print(score);

    Display.txt_MoveCursor(6, 0);
    Display.print("High : ");
    Display.print(highScore);

    Display.txt_MoveCursor(8, 0);
    Display.print("Center = Back");
}

// -----------------------------------
// Food placement (spawn + draw once)
// -----------------------------------
static void spawnFood() {
    foodX = random(0, GRID_W);
    foodY = random(0, GRID_H);

    int fx = foodX * CELL;
    int fy = foodY * CELL + OFFSET_Y;
    Display.gfx_RectangleFilled(fx, fy, fx + CELL - 1, fy + CELL - 1, RED);
}

// -----------------------------------
// PUBLIC API — start game
// -----------------------------------
void startSnakeGame() {
    // Reset inputs
    lastUp = lastDown = lastLeft = lastRight = lastCenter = 0;

    gameOver = false;
    score = 0;

    snakeLength = 3;
    snakeX[0] = 5; snakeY[0] = 5;
    snakeX[1] = 4; snakeY[1] = 5;
    snakeX[2] = 3; snakeY[2] = 5;

    dirX = 1;
    dirY = 0;

    // LED: game mode color (blue)
    ledSetRGB(false, false, true);

    // Clear screen and draw HUD once (includes "HI")
    Display.gfx_Cls();
    drawSnakeHUD();

    // Spawn food and draw full grid
    spawnFood();
    drawSnakeGridFull();

    firstStep = true;
    prevTailX = -1;
    prevTailY = -1;
    lastMove = millis();
}

// -----------------------------------
// PUBLIC API — main game loop
// -----------------------------------
void snakeLoop() {
    bool up, down, left, right, center;
    readNavLatched(up, down, left, right, center);

    // --- If game over: wait for center ---
if (gameOver) {
    if (center) {
        lastCenter = 0;
        ledSetMood(happiness);   // restore mood color
        appState = STATE_GAMES_MENU;
        menuIndex = 0;

        // Fully clear screen before returning to menus
        Display.gfx_Cls();
        drawScreen();
    }
    return;
}


if (center) {
    lastCenter = 0;
    ledSetMood(happiness);       // restore mood color
    appState = STATE_GAMES_MENU;
    menuIndex = 0;

    // Fully clear screen before returning to menus
    Display.gfx_Cls();
    drawScreen();
    return;
}


    // Prevent 180° reverse
    if (up   && dirY !=  1) { dirX = 0;  dirY = -1; }
    else if (down && dirY != -1) { dirX = 0;  dirY =  1; }
    else if (left && dirX !=  1) { dirX = -1; dirY =  0; }
    else if (right&& dirX != -1) { dirX =  1; dirY =  0; }

    // Movement timing
    if (millis() - lastMove < SPEED)
        return;
    lastMove = millis();

    // Remember current tail position for erase on next draw
    prevTailX = snakeX[snakeLength - 1];
    prevTailY = snakeY[snakeLength - 1];

    // Move body (from tail to head)
    for (int i = snakeLength - 1; i > 0; i--) {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }

    // Move head
    snakeX[0] += dirX;
    snakeY[0] += dirY;

    // Wrap edges
    if (snakeX[0] < 0)         snakeX[0] = GRID_W - 1;
    if (snakeX[0] >= GRID_W)   snakeX[0] = 0;
    if (snakeY[0] < 0)         snakeY[0] = GRID_H - 1;
    if (snakeY[0] >= GRID_H)   snakeY[0] = 0;

    // Self collision
    for (int i = 1; i < snakeLength; i++) {
        if (snakeX[i] == snakeX[0] && snakeY[i] == snakeY[0]) {
            // Save highscore (RAM-only; resets on reboot)
            if (score > highScore) {
                highScore = score;
            }

            gameOver = true;
            ledFlashBad();
            soundGameOver();
            drawSnakeGameOver();
            return;
        }
    }

    // Food collision
    if (snakeX[0] == foodX && snakeY[0] == foodY) {
        snakeLength++;
        score++;

        // Increase happiness on each food
        happiness = clamp(happiness + 6);   // choose step you like
        ledSetMood(happiness);              // update LED mood

        // Score and happiness changed → update HUD values only
        updateSnakeHUDValues();

        // Feedback
        ledFlashGood();
        soundFood();

        // New food position and draw it
        spawnFood();
    }

    // First frame after start: full redraw, then incremental
    if (firstStep) {
        drawSnakeGridFull();
        firstStep = false;
    } else {
        drawSnakeGrid();
    }
}
