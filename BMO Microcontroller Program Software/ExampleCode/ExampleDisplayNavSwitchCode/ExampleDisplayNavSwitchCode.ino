// Uses library found at: https://github.com/4dsystems/Goldelox-Serial-Arduino-Library/tree/master
// This library can be downloaded directly into the Arduino IDE
// This demo is modified from the GoldeloxBigDemo.ino found in the repo examples

#define DisplaySerial Serial1

#include "Goldelox_Serial_4DLib.h"
#include "Goldelox_Const4D.h"

// Use this if using HardwareSerial or SoftwareSerial
Goldelox_Serial_4DLib Display(&DisplaySerial);

#define RESETLINE 4

// Assign pin numbers for navigation switch (updated pin assignments)
#define NAV_UP     11
#define NAV_DOWN   12
#define NAV_LEFT   13
#define NAV_RIGHT  20
#define NAV_CENTER 19

// Ball properties - using signed integers to prevent wraparound
int CircleX = 64;  // Start at center (changed from word to int)
int CircleY = 64;  // Start at center (changed from word to int)
const int BALL_RADIUS = 5;
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 128;
const int MOVE_SPEED = 7;  // Pixels per button press

// Variables for button state management
bool lastNavState[5] = {HIGH, HIGH, HIGH, HIGH, HIGH}; // UP, DOWN, LEFT, RIGHT, CENTER
unsigned long lastMoveTime = 0;
const unsigned long MOVE_DELAY = 150; // Milliseconds between moves

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting uLCD Navigation Ball Control...");

    // Configure navigation pins as inputs with pull-ups
    pinMode(NAV_UP, INPUT_PULLUP);
    pinMode(NAV_DOWN, INPUT_PULLUP);
    pinMode(NAV_LEFT, INPUT_PULLUP);
    pinMode(NAV_RIGHT, INPUT_PULLUP);
    pinMode(NAV_CENTER, INPUT_PULLUP);

    // Initialize display
    pinMode(RESETLINE, OUTPUT);
    digitalWrite(RESETLINE, 0);
    delay(100);
    digitalWrite(RESETLINE, 1);
    delay(5000);

    DisplaySerial.begin(9600, SERIAL_8N1, 21, 22);
    Display.TimeLimit4D = 5000;
    Display.gfx_Cls();
    
    // Draw initial ball
    Display.gfx_CircleFilled(CircleX, CircleY, BALL_RADIUS, BLUE);
    
    Serial.println("Navigation ball ready! Use navigation switch to move.");
}

void loop()
{
    // Check if enough time has passed since last move
    if (millis() - lastMoveTime < MOVE_DELAY) {
        return;
    }

    // Read current navigation switch states
    bool navUp = (digitalRead(NAV_UP) == LOW);
    bool navDown = (digitalRead(NAV_DOWN) == LOW);
    bool navLeft = (digitalRead(NAV_LEFT) == LOW);
    bool navRight = (digitalRead(NAV_RIGHT) == LOW);
    bool navCenter = (digitalRead(NAV_CENTER) == LOW);

    // Check if any direction is pressed
    bool anyPressed = navUp || navDown || navLeft || navRight || navCenter;
    
    if (anyPressed) {
        // Calculate new position using signed integers
        int newX = CircleX;
        int newY = CircleY;
        
        // Handle diagonal and single direction movements
        if (navUp) {
            newY -= MOVE_SPEED;
        }
        if (navDown) {
            newY += MOVE_SPEED;
        }
        if (navLeft) {
            newX -= MOVE_SPEED;
        }
        if (navRight) {
            newX += MOVE_SPEED;
        }
        
        // Boundary checking - keep ball within screen bounds
        if (newX < BALL_RADIUS) {
            newX = BALL_RADIUS;
        }
        if (newX > SCREEN_WIDTH - BALL_RADIUS) {
            newX = SCREEN_WIDTH - BALL_RADIUS;
        }
        if (newY < BALL_RADIUS) {
            newY = BALL_RADIUS;
        }
        if (newY > SCREEN_HEIGHT - BALL_RADIUS) {
            newY = SCREEN_HEIGHT - BALL_RADIUS;
        }

        // Only update display if position actually changed
        if (newX != CircleX || newY != CircleY) {
            // Erase old ball
            Display.gfx_CircleFilled(CircleX, CircleY, BALL_RADIUS, BLACK);
            
            // Update position
            CircleX = newX;
            CircleY = newY;
            
            // Draw new ball
            Display.gfx_CircleFilled(CircleX, CircleY, BALL_RADIUS, BLUE);
            
            // Print movement for debugging
            String direction = "";
            if (navUp && navRight) direction = "top-right";
            else if (navUp && navLeft) direction = "top-left";
            else if (navDown && navRight) direction = "bottom-right";
            else if (navDown && navLeft) direction = "bottom-left";
            else if (navUp) direction = "up";
            else if (navDown) direction = "down";
            else if (navLeft) direction = "left";
            else if (navRight) direction = "right";
            else if (navCenter) direction = "center";
            
            Serial.print("Moved ");
            Serial.print(direction);
            Serial.print(" to position (");
            Serial.print(CircleX);
            Serial.print(", ");
            Serial.print(CircleY);
            Serial.println(")");
        }
        
        // Update last move time
        lastMoveTime = millis();
    }
}
