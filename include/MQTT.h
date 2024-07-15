#ifndef MQTT_H
#define MQTT_H

#include <PubSubClient.h>
#include <WiFi.h>

// Function declarations
void setupMqtt(String tankName, int retries = 3);
void sendSensorData(float temp, float par, String tankName);
void mqttLoop();

// MQTT settings
extern WiFiClient espClient;
extern PubSubClient mqttClient;
extern const char *mqtt_server;
extern String temperatureTopic;
extern String parTopic;

#endif
