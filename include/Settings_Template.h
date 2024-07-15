// COPY THIS FILE IN THIS DIRECTORY AND RENAME IT Settings.h, THEN ADJUST THE VALUES AS NEEDED
#include <Arduino.h>

#ifndef SETTINGS_H
#define SETTINGS_H

// Global Constants
#define SECOND 1000
#define MINUTE 60000
#define LED_BUILTIN 2
#define ONE_WIRE_BUS 23
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Wi-Fi Settings (2.4 GHz only)
const char *ssid = "Your Network Here";         // Your Wi-Fi SSID
const char *password = "Network Password Here"; // Your Wi-Fi Password

// Application Settings (modify these values as needed)
const String tankName = "Q2";
const unsigned long sensorReadInterval = 2 * SECOND;      // 2 seconds (Rolling average over 20 seconds)
const unsigned long notificationDelay = 15 * MINUTE;      // 15 minutes
const unsigned int notificationParThreshold = 100;        // PAR threshold for triggering notification
const unsigned int notificationTemperatureThreshold = 30; // Temperature threshold for triggering notification

#endif