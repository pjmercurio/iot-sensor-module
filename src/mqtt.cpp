#include "MQTT.h"
#include <ArduinoJson.h>
#include <Preferences.h>

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Broker settings
const char *mqtt_server = "192.168.68.100"; // Static IP address of the MQTT broker (Raspberry Pi)
String ipAddress;

void setupMqtt(String tankName, int retries)
{
    mqttClient.setServer(mqtt_server, 1883);
    ipAddress = WiFi.localIP().toString();

    // Connect to the MQTT broker
    int retriesRemaining = retries;
    while (!mqttClient.connected() && retriesRemaining > 0)
    {
        Serial.print("Attempting MQTT connection... ");

        // Create a unique client ID
        char clientId[25];
        snprintf(clientId, 25, "ESP32Client-%s", tankName);
        // snprintf(clientId, 25, "ESP32Client-%X", random(0xffff));

        if (mqttClient.connect(clientId))
            Serial.println("SUCCESS!");
        else
        {
            Serial.print("FAIL!, rc=");
            Serial.println(mqttClient.state());
            retriesRemaining--;
            delay(2000);
        }
    }
}

void sendSensorData(float temp, float par, String tankName)
{
    if (!mqttClient.connected())
    {
        Serial.println("Failed to send sensor data. MQTT client is not connected.");
        setupMqtt(tankName, 1);
    }
    if (!mqttClient.connected())
        return;

    JsonDocument doc;
    if (temp > -999)
    {
        doc["Temperature"] = temp;
    }
    if (par > -999)
    {
        doc["PAR"] = par;
    }
    doc["IPAddress"] = ipAddress;

    String payload;
    serializeJson(doc, payload);
    String sensorTopic = "Tanks/" + tankName + "/SensorData";
    mqttClient.publish(sensorTopic.c_str(), payload.c_str());
}

void mqttLoop()
{
    mqttClient.loop();
}
