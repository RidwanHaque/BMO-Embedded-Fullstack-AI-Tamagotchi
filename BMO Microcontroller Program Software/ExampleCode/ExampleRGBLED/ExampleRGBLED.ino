
#include <Arduino.h>

// ==== CONFIGURATION ====
// Set to true if your RGB LED is COMMON ANODE
#define COMMON_ANODE true

// Pin definitions
#define BUTTON_FWD 18
#define BUTTON_BWD 19
#define LED_R 21
#define LED_G 22
#define LED_B 23

// ==== State Variables ====
volatile int colorState = 0;      // 0=R, 1=G, 2=B, 3=Y
volatile bool forwardPressed = false;
volatile bool backwardPressed = false;

// For software debounce
volatile unsigned long lastFwdPress = 0;
volatile unsigned long lastBwdPress = 0;
const unsigned long debounceDelay = 200;  // ms

// ==== Interrupt Service Routines ====
void IRAM_ATTR isrForward() {
  unsigned long now = millis();
  if (now - lastFwdPress > debounceDelay) {
    forwardPressed = true;
    lastFwdPress = now;
  }
}

void IRAM_ATTR isrBackward() {
  unsigned long now = millis();
  if (now - lastBwdPress > debounceDelay) {
    backwardPressed = true;
    lastBwdPress = now;
  }
}

// ==== Helper Function ====
void showColor(int state) {
  bool R, G, B;
  switch (state) {
    case 0: R = 1; G = 0; B = 0; break;  // Red
    case 1: R = 0; G = 1; B = 0; break;  // Green
    case 2: R = 0; G = 0; B = 1; break;  // Blue
    case 3: R = 1; G = 1; B = 0; break;  // Yellow
  }

  if (COMMON_ANODE) {
    // invert signals for common anode
    digitalWrite(LED_R, !R);
    digitalWrite(LED_G, !G);
    digitalWrite(LED_B, !B);
  } else {
    digitalWrite(LED_R, R);
    digitalWrite(LED_G, G);
    digitalWrite(LED_B, B);
  }
}

// ==== Setup ====
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("RGB LED Interrupt Program Started!");

  pinMode(BUTTON_FWD, INPUT_PULLUP);
  pinMode(BUTTON_BWD, INPUT_PULLUP);
  attachInterrupt(BUTTON_FWD, isrForward, FALLING);
  attachInterrupt(BUTTON_BWD, isrBackward, FALLING);

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  showColor(colorState);
}

// ==== Main Loop ====
void loop() {
  if (forwardPressed) {
    forwardPressed = false;
    colorState = (colorState + 1) % 4;
    showColor(colorState);
    Serial.printf("Forward pressed -> State: %d\n", colorState);
  }

  if (backwardPressed) {
    backwardPressed = false;
    colorState = (colorState - 1 + 4) % 4;
    showColor(colorState);
    Serial.printf("Backward pressed -> State: %d\n", colorState);
  }
}



