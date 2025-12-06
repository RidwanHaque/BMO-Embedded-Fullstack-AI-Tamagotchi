#include <esp_now.h>
#include <WiFi.h>
#include "Config.h"
#include "LED.h"
#include "DisplayUI.h"

// Share the same struct as the IMU sender
typedef struct struct_message {
  uint8_t eventType;   // 1 = shake
} struct_message;

struct_message incoming;

// Extern globals from your main code
extern int happiness;
extern AppState appState;

// Optional: clamp helper and mood
extern int clamp(int v);
extern void ledSetMood(int happiness);

// ESP-NOW receive callback
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  if (len != sizeof(struct_message)) return;

  memcpy(&incoming, data, sizeof(incoming));

  Serial.print("ESP-NOW recv: len=");
  Serial.print(len);
  Serial.print(" eventType=");
  Serial.println(incoming.eventType);

  if (incoming.eventType == 1) {
    // Violent shake detected on remote IMU -> boost happiness
    happiness = clamp(happiness + 5);   // tweak amount
    ledSetMood(happiness);

    // Update only happiness bar + face
    updateBarFill(58, 10, 65, happiness, YELLOW);
    drawFace();

    Serial.print("Happiness updated to ");
    Serial.println(happiness);
  }
}

// Call this from setup() AND after wake
void initRemoteHappiness() {
  if (!Serial) {
    Serial.begin(115200);   // ensure Serial is started
    delay(50);
  }

  // Ensure STA mode
  WiFi.mode(WIFI_STA);

  // Reset ESP-NOW state to be safe across sleeps
  esp_now_deinit();
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("ESP-NOW receiver ready");
}
