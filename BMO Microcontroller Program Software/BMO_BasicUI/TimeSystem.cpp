#include "TimeSystem.h"

#include <WiFi.h>
#include <time.h>

// allow TimeSystem to call your UI redraw
extern void drawScreen();

// Global RTC object (UTC offset 0 inside ESP32Time)
ESP32Time rtc(0);

// Phone hotspot credentials
const char* ssid     = "Ridwan";     // your hotspot SSID
const char* password = "Derp2414";  // your hotspot password

// NTP configuration: Eastern Time (UTC-5, no DST handling)
const char* ntpServer      = "pool.ntp.org";
const long  gmtOffset_sec  = -5 * 3600;  // UTC-5 hours
const int   daylightOffset = 0;          // no DST

void initRTC() {
  // Fallback time so UI always has something
  rtc.setTime(0, 0, 12, 1, 1, 2025);

  if (!Serial) {
    Serial.begin(115200);
    delay(50);
  }
  Serial.print("\nConnecting to WiFi");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Try up to ~8 seconds to connect
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 8000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWiFi NOT connected, keeping default 12:00:00.");
    return;
  }

  Serial.println("\nConnected to WiFi!");
  Serial.print("Local IP Address: ");
  Serial.println(WiFi.localIP());

  // Ask NTP for current time (Eastern)
  configTime(gmtOffset_sec, daylightOffset, ntpServer);

  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 5000)) {   // 5 s timeout
    rtc.setTimeStruct(timeinfo);         // copy system time into ESP32Time
    Serial.println("NTP time set.");

    // Redraw UI so the top-bar time matches NTP
    drawScreen();
  } else {
    Serial.println("NTP getLocalTime() failed, keeping default 12:00:00.");
  }

  WiFi.disconnect(true);   // optional: turn off WiFi after sync
}

// Return "HH:MM"
String getTimeString() {
  return rtc.getTime("%H:%M");
}

// Return date string (from ESP32Time, e.g. "Sun, Jan 17 2021")
String getDateString() {
  return rtc.getDate();
}

// Return raw Unix epoch
long getUnixTime() {
  return rtc.getEpoch();
}
