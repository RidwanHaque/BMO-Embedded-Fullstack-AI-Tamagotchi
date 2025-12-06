#include "Goldelox_Serial_4DLib.h"
#include "Goldelox_Const4D.h"

#define DisplaySerial Serial1

// Display pins
#define LCD_TX    17
#define LCD_RX    16
#define LCD_RESET 4

Goldelox_Serial_4DLib Display(&DisplaySerial);

// Nav switch pins
#define NAV_UP     11
#define NAV_DOWN   10
#define NAV_LEFT   1
#define NAV_RIGHT  2
#define NAV_CENTER 5

void setup() {
  // --- Reset LCD ---
  pinMode(LCD_RESET, OUTPUT);
  digitalWrite(LCD_RESET, LOW);
  delay(100);
  digitalWrite(LCD_RESET, HIGH);
  delay(3000);

  // --- Start LCD UART at stable 9600 ---
  DisplaySerial.begin(9600, SERIAL_8N1, LCD_RX, LCD_TX);
  Display.TimeLimit4D = 5000;

  // --- Setup nav switch pins ---
  pinMode(NAV_UP, INPUT_PULLUP);
  pinMode(NAV_DOWN, INPUT_PULLUP);
  pinMode(NAV_LEFT, INPUT_PULLUP);
  pinMode(NAV_RIGHT, INPUT_PULLUP);
  pinMode(NAV_CENTER, INPUT_PULLUP);

  // Initial screen
  Display.gfx_Cls();
  Display.txt_FGcolour(WHITE);
  Display.txt_MoveCursor(0, 0);
  Display.print("Nav Switch Test");
}

void loop() {
  Display.txt_FGcolour(GREEN);
  
  if (!digitalRead(NAV_UP)) {
    Display.gfx_Cls();
    Display.txt_MoveCursor(1, 0);
    Display.print("UP");
    delay(150);
  }
  else if (!digitalRead(NAV_DOWN)) {
    Display.gfx_Cls();
    Display.txt_MoveCursor(1, 0);
    Display.print("DOWN");
    delay(150);
  }
  else if (!digitalRead(NAV_LEFT)) {
    Display.gfx_Cls();
    Display.txt_MoveCursor(1, 0);
    Display.print("LEFT");
    delay(150);
  }
  else if (!digitalRead(NAV_RIGHT)) {
    Display.gfx_Cls();
    Display.txt_MoveCursor(1, 0);
    Display.print("RIGHT");
    delay(150);
  }
  else if (!digitalRead(NAV_CENTER)) {
    Display.gfx_Cls();
    Display.txt_MoveCursor(1, 0);
    Display.print("CENTER");
    delay(150);
  }
}
