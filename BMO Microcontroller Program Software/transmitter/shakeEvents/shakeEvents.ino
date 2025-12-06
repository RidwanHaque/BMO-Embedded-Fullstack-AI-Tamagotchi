#include "ICM_20948.h"
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <esp_now.h>
#include <WiFi.h>

// ---------------------
// Pins
// ---------------------
#define SDA_PIN      6     // GPIO6 -> ICM-20948 SDA
#define SCL_PIN      7     // GPIO7 -> ICM-20948 SCL
#define RGB_LED_PIN  8     // Onboard NeoPixel

// ---------------------
// IMU / LED objects
// ---------------------
ICM_20948_I2C myICM;

Adafruit_NeoPixel pixels(1, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);

// Colors
#define COLOR_WHITE   pixels.Color(255, 255, 255)
#define COLOR_RED     pixels.Color(255, 0, 0)
#define COLOR_GREEN   pixels.Color(0, 255, 0)
#define COLOR_BLUE    pixels.Color(0, 0, 255)
#define COLOR_YELLOW  pixels.Color(255, 255, 0)
#define COLOR_PURPLE  pixels.Color(128, 0, 128)
#define COLOR_CYAN    pixels.Color(0, 255, 255)

// Thresholds
#define TILT_THRESHOLD     20.0    // deg
#define MOTION_THRESHOLD   50.0    // small motion
#define SHAKE_THRESHOLD    150.0   // violent shake (tune this)

// Smoothing / magnitude tracking
float smoothAccelX = 0, smoothAccelY = 0, smoothAccelZ = 0;
float previousMagnitude = 0;
float baselineMagnitude = 0;
bool calibrated = false;

// ---------------------
// ESP-NOW message
// ---------------------
typedef struct struct_message {
  uint8_t eventType;   // 1 = violent shake
} struct_message;

struct_message msg;

// BMO ESP32 MAC address (STA MAC) - REPLACE with your board's MAC
uint8_t receiverMAC[] = { 0xA0, 0x85, 0xE3, 0xDA, 0xC1, 0x0C };
// A0:85:E3:DA:C1:0C

void setup() {
  Serial.begin(115200);
  delay(1000);

  // LED init
  pixels.begin();
  pixels.setBrightness(50);
  pixels.setPixelColor(0, COLOR_WHITE);
  pixels.show();

  // I2C init
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);

  // IMU init
  bool initialized = false;
  while (!initialized) {
    myICM.begin(Wire, 1);
    Serial.print(F("ICM init: "));
    Serial.println(myICM.statusString());

    if (myICM.status != ICM_20948_Stat_Ok) {
      Serial.println("ICM-20948 not ready, retrying...");
      delay(500);
    } else {
      initialized = true;
      Serial.println("ICM-20948 ready!");
    }
  }

  // Baseline calibration
  Serial.println("\n=== CALIBRATING BASELINE ===");
  Serial.println("Keep board STILL for ~3 seconds...");

  float magSum = 0;
  int samples = 50;

  for (int i = 0; i < samples; i++) {
    if (myICM.dataReady()) {
      myICM.getAGMT();
      float ax = myICM.accX();
      float ay = myICM.accY();
      float az = myICM.accZ();
      float mag = sqrt(ax * ax + ay * ay + az * az);
      magSum += mag;
    }
    delay(50);
  }

  baselineMagnitude = magSum / samples;
  previousMagnitude = baselineMagnitude;
  calibrated = true;

  Serial.print("Baseline magnitude: ");
  Serial.print(baselineMagnitude, 2);
  Serial.println(" G");

  Serial.println("\n=== TILT + SHAKE DETECTION ===");
  Serial.println("Violent shake -> send ESP-NOW event");

  // -----------------
  // ESP-NOW init
  // -----------------
  WiFi.mode(WIFI_STA);  // station mode required
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    while (1) delay(10);
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;   // same channel
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add ESP-NOW peer");
    while (1) delay(10);
  }
}

void loop() {
  if (myICM.dataReady() && calibrated) {
    myICM.getAGMT();

    // Raw accelerometer
    float accelX = myICM.accX();
    float accelY = myICM.accY();
    float accelZ = myICM.accZ();

    // Simple smoothing
    smoothAccelX = (smoothAccelX * 0.8f) + (accelX * 0.2f);
    smoothAccelY = (smoothAccelY * 0.8f) + (accelY * 0.2f);
    smoothAccelZ = (smoothAccelZ * 0.8f) + (accelZ * 0.2f);

    // Magnitude and change from baseline
    float magnitude = sqrt(
      smoothAccelX * smoothAccelX +
      smoothAccelY * smoothAccelY +
      smoothAccelZ * smoothAccelZ
    );
    float magnitudeChange = fabs(magnitude - baselineMagnitude);

    // Detect violent shake -> change LED and send ESP-NOW event
    if (magnitudeChange > SHAKE_THRESHOLD) {
      pixels.setPixelColor(0, COLOR_GREEN); // or any color you want for shake
      pixels.show();

      msg.eventType = 1;  // "shake" event
      esp_now_send(receiverMAC, (uint8_t *)&msg, sizeof(msg));
      Serial.println("Violent shake detected -> sending event");
      delay(200);  // simple cooldown

    } else {
      // Optionally: set "normal" LED color when not shaken
      pixels.setPixelColor(0, COLOR_WHITE);
      pixels.show();
    }

    // Debug print occasionally
    static int printCount = 0;
    if (++printCount >= 10) {
      printCount = 0;
      Serial.print("Mag: ");
      Serial.print(magnitude, 1);
      Serial.print("G | dMag: ");
      Serial.print(magnitudeChange, 1);
      Serial.print("G | Baseline: ");
      Serial.print(baselineMagnitude, 1);
      Serial.println("G");
    }
  }

  delay(50);
}

