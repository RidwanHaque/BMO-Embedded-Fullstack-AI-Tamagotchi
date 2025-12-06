#include "MicVoice.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Config.h"
#include "Sound.h"
#include "DisplayUI.h"

// --------- CONFIG ---------

#define MIC_PIN        6           // SPW2430 DC -> ADC1 pin
#define MIC_BTN_PIN    7           // Pushbutton to GND, INPUT_PULLUP

#define SAMPLE_RATE    16000       // Hz
#define MAX_RECORD_SEC 5           // max recording length in buffer
#define MAX_SAMPLES    (SAMPLE_RATE * MAX_RECORD_SEC)

// Max duration actually sent to backend (seconds)
#define MAX_SEND_SEC      3
#define MAX_SEND_SAMPLES (SAMPLE_RATE * MAX_SEND_SEC)

// Tune this after watching quiet raw readings
static const int MIC_DC_OFFSET = 790;   // measured quiet value

const char* VOICE_ENDPOINT = "http://172.20.10.3:3001/api/voice/input";
const char* micWifiSsid     = "Ridwan";
const char* micWifiPassword = "Derp2414";

// --------- STATE ---------

static int16_t audioBuf[MAX_SAMPLES];
static size_t  sampleCount = 0;
static bool    recording   = false;

static unsigned long lastSampleMicros = 0;
static unsigned long pressStart = 0;

// from main sketch to control chat overlay
extern bool inChatOverlay;

// interrupt edge flags (set in ISR, consumed in loop)
volatile bool micBtnFell      = false;  // HIGH -> LOW (pressed)
volatile bool micBtnRose      = false;  // LOW  -> HIGH (released)
volatile unsigned long micBtnLastChange = 0;
const unsigned long MIC_BTN_DEBOUNCE_MS = 50;

// --------- INTERNAL HELPERS ---------

static void connectWifiIfNeeded() {
  if (WiFi.status() == WL_CONNECTED) return;

  WiFi.mode(WIFI_STA);
  WiFi.begin(micWifiSsid, micWifiPassword);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 8000) {
    delay(250);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("MicVoice: WiFi connected, IP=");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("MicVoice: WiFi connect failed");
  }
}

static void startRecording() {
  sampleCount = 0;
  recording = true;
  lastSampleMicros = micros();
  Serial.println("MicVoice: start recording");
}

static void stopRecordingAndSend() {
  recording = false;
  Serial.println("MicVoice: stop recording");

  if (sampleCount == 0) {
    Serial.println("MicVoice: no samples captured");
    return;
  }

  // Require at least 1 second of raw audio
  const size_t MIN_SAMPLES = SAMPLE_RATE * 1; // 1.0 s
  if (sampleCount < MIN_SAMPLES) {
    Serial.print("MicVoice: too short (");
    Serial.print(sampleCount);
    Serial.println(" samples), not sending");
    sampleCount = 0;
    return;
  }

  // ---- Simple silence trimming (energy-based) ----
  int startIdx = 0;
  int endIdx   = (int)sampleCount - 1;
  const int SILENCE_THRESH = 20;   // tweak if needed

  // Trim leading silence
  while (startIdx < (int)sampleCount &&
         abs(audioBuf[startIdx]) < SILENCE_THRESH) {
    startIdx++;
  }

  // Trim trailing silence
  while (endIdx > startIdx &&
         abs(audioBuf[endIdx]) < SILENCE_THRESH) {
    endIdx--;
  }

  int trimmedSamples = endIdx - startIdx + 1;
  if (trimmedSamples <= (int)(SAMPLE_RATE / 2)) {  // less than 0.5 s voiced
    Serial.print("MicVoice: active audio too short after trim (");
    Serial.print(trimmedSamples);
    Serial.println(" samples)");
    sampleCount = 0;
    return;
  }

  // Cap what we actually send so backend never sees > MAX_SEND_SEC
  if (trimmedSamples > MAX_SEND_SAMPLES) {
    Serial.print("MicVoice: trimming to max send length, from ");
    Serial.print(trimmedSamples);
    Serial.print(" to ");
    Serial.println(MAX_SEND_SAMPLES);
    trimmedSamples = MAX_SEND_SAMPLES;
  }

  connectWifiIfNeeded();
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("MicVoice: WiFi not connected, cannot send audio");
    return;
  }

  HTTPClient http;
  http.begin(VOICE_ENDPOINT);
  http.addHeader("Content-Type", "application/octet-stream");

  size_t bytesLen = trimmedSamples * sizeof(int16_t);
  Serial.print("MicVoice: sending trimmed ");
  Serial.print(trimmedSamples);
  Serial.println(" samples");

  int code = http.POST((uint8_t*)&audioBuf[startIdx], bytesLen);
  Serial.print("MicVoice: HTTP POST result = ");
  Serial.println(code);

  if (code == 200) {
    String response = http.getString();
    Serial.println("MicVoice: raw server response:");
    Serial.println(response);

    StaticJsonDocument<1536> doc;
    DeserializationError err = deserializeJson(doc, response);
    if (err) {
      Serial.print("MicVoice: JSON parse error: ");
      Serial.println(err.f_str());
    } else {
      const char* bmoText = doc["bmoResponse"]["content"];
      if (bmoText) {
        Serial.print("MicVoice: BMO says: ");
        Serial.println(bmoText);

        // enter chat overlay: show text, disable center until user presses UP
        inChatOverlay = true;
        showBMOReplyOnScreen(bmoText);
        soundVoiceReply();
      } else {
        Serial.println("MicVoice: bmoResponse.content missing");
      }
    }
  }

  http.end();
  sampleCount = 0;
}

// --------- ISR ---------

void IRAM_ATTR micButtonISR() {
  unsigned long now = millis();
  if (now - micBtnLastChange < MIC_BTN_DEBOUNCE_MS) return;  // debounce

  micBtnLastChange = now;
  bool pressed = (digitalRead(MIC_BTN_PIN) == LOW);  // pressed = LOW

  if (pressed) {
    micBtnFell = true;   // pressed edge
  } else {
    micBtnRose = true;   // released edge
  }
}

// --------- PUBLIC API ---------

void initMicVoice() {
  analogReadResolution(12);
  analogSetPinAttenuation(MIC_PIN, ADC_0db);
  pinMode(MIC_PIN, INPUT);

  pinMode(MIC_BTN_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(MIC_BTN_PIN), micButtonISR, CHANGE);

  if (!Serial) {
    Serial.begin(115200);
    delay(50);
  }

  Serial.println("MicVoice: initialized");
}

void micVoiceLoop() {
  unsigned long now = millis();

  // Handle button edges latched by ISR
  if (micBtnFell) {
    micBtnFell = false;
    pressStart = now;
    Serial.println("BTN: down edge");
  }

  if (micBtnRose) {
    micBtnRose = false;
    Serial.println("BTN: up edge");
    if (recording) {
      stopRecordingAndSend();
    }
  }

  // Read actual pin level
  bool physicalPressed = (digitalRead(MIC_BTN_PIN) == LOW);

  // Failsafe: if pin is HIGH and we are still recording, force a release
  if (!physicalPressed && recording && !micBtnRose) {
    Serial.println("BTN: up edge (failsafe)");
    stopRecordingAndSend();
  }

  // Start recording only if held for at least 100 ms
  if (physicalPressed && !recording && (now - pressStart > 100)) {
    startRecording();
  }

  // Auto-stop when buffer is full
  if (recording && sampleCount >= MAX_SAMPLES) {
    Serial.println("MicVoice: reached MAX_SAMPLES, auto-stopping");
    stopRecordingAndSend();
    return;
  }

  // Sampling while recording
  if (recording && sampleCount < MAX_SAMPLES) {
    unsigned long nowMicros = micros();
    unsigned long interval  = 1000000UL / SAMPLE_RATE;

    if (nowMicros - lastSampleMicros >= interval) {
      lastSampleMicros += interval;

      int raw = analogRead(MIC_PIN);  // 0..4095

      if (sampleCount % 1000 == 0) {
        Serial.print("SAMPLE: sampleCount=");
        Serial.print(sampleCount);
        Serial.print(" raw=");
        Serial.println(raw);
      }

      int16_t centered = (int16_t)(raw - MIC_DC_OFFSET);
      audioBuf[sampleCount++] = centered;
    }
  }
}

