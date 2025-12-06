#ifndef TIME_SYSTEM_H
#define TIME_SYSTEM_H

#include <Arduino.h>
#include <ESP32Time.h>

// Initialize the RTC system (tries NTP once, falls back safely)
void initRTC();

// Get formatted time string "HH:MM:SS"
String getTimeString();

// Get formatted date string
String getDateString();

// Get raw Unix epoch
long getUnixTime();

#endif
