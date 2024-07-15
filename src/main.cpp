#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "Splash_Image.h"
#include "Settings.h"
#include "MQTT.h"

// Remotely configuable settings
AsyncWebServer server(80);
Preferences preferences;
String tankName = defaultTankName;
unsigned long sensorReadInterval = defaultSensorReadInterval;

// Peripherals
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
BH1750 lightMeter;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Spash image
unsigned long splashStartTime = 0;
const unsigned long splashDuration = 3 * SECOND;
bool splashDisplayed = false;
bool wifiSetupCompleted = false;
bool mqttSetupCompleted = false;
bool canRefresh = false;

// Sensor readings
unsigned long lastSensorReadTime = 0;
unsigned long lastNotificationTime = 0;

// Function declarations
float readTemperature();
float readLightSensor();
void setupPreferences();
void setupWebServer();
void setupWiFi(int retries = 5);
void setupDisplay();
void showSplashImage();
void refreshDisplay();

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  lightMeter.begin();
  sensors.begin(); // Initialize the DS18B20 sensor
  setupDisplay();
  setupPreferences();
  setupWiFi();
  setupWebServer();
}

void setupPreferences()
{
  preferences.begin("app_prefs", false);
  // Tank Name
  if (!preferences.isKey("tankName"))
  {
    preferences.putString("tankName", defaultTankName);
  }
  else
  {
    tankName = preferences.getString("tankName");
  }
  // Sensor Read Interval
  if (!preferences.isKey("sensorReadInterval"))
  {
    preferences.putULong("sensorReadInterval", defaultSensorReadInterval);
  }
  else
  {
    sensorReadInterval = preferences.getULong("sensorReadInterval");
  }
  preferences.end();
}

void setupWiFi(int retries)
{
  int retriesRemaining = retries;
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi... ");
  while (WiFi.status() != WL_CONNECTED && retriesRemaining > 0)
  {
    delay(1000);
    retriesRemaining--;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.print("SUCCESS! Connected to the WiFi network with IP: ");
    Serial.println(WiFi.localIP());
    setupMqtt(tankName);
  }
  else
  {
    Serial.println("FAIL!");
  }

  wifiSetupCompleted = true;
  mqttSetupCompleted = true;
}

void setupWebServer()
{
  server.on("/setTankName", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    // Prepare headers for CORS
    AsyncWebServerResponse *response;

    if (request->hasParam("name", true)) {
        // Extract the parameter
        String name = request->getParam("name", true)->value();
        tankName = name;

        // Save the new name in preferences
        preferences.begin("app_prefs", false);
        preferences.putString("tankName", name);
        preferences.end();

        response = request->beginResponse(200, "text/plain", "Tank name updated to: " + name);
    } else {
        response = request->beginResponse(400, "text/plain", "Missing name parameter");
    }

    // Send the response with CORS headers
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
    request->send(response); });

  server.on("/setSensorReadInterval", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    if (request->hasParam("interval", true))
    {
      unsigned long interval = request->getParam("interval", true)->value().toInt();
      sensorReadInterval = interval;
      preferences.begin("app_prefs", false);
      preferences.putULong("sensorReadInterval", interval);
      preferences.end();
      request->send(200, "text/plain", "Sensor read interval updated to: " + String(interval));
    }
    else
    {
      request->send(400, "text/plain", "Missing interval parameter");
    } });
  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  unsigned long currentMillis = millis();

  // Check if splash screen has been displayed for the required duration
  if (splashDisplayed && (currentMillis - splashStartTime >= splashDuration) && wifiSetupCompleted && mqttSetupCompleted)
  {
    splashDisplayed = false;
    canRefresh = true;
  }

  // Non-blocking delay for sensor reading
  if (currentMillis - lastSensorReadTime >= sensorReadInterval)
  {
    mqttLoop();
    refreshDisplay();
    lastSensorReadTime = currentMillis;
  }
}

void setupDisplay()
{
  // Initialize with the I2C addr 0x3C (for the 128x64)
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  showSplashImage();
}

void showSplashImage()
{
  splashStartTime = millis();
  display.clearDisplay();
  display.drawBitmap(0, 0, epd_bitmap_allArray[0], 128, 64, 1);
  display.display();
  splashDisplayed = true;
}

void refreshDisplay()
{
  if (!canRefresh)
    return;

  // Read sensor data
  float temperature = readTemperature();
  float parValue = readLightSensor();

  // Send readings
  if (WiFi.status() != WL_CONNECTED)
    setupWiFi(1);
  if (WiFi.status() == WL_CONNECTED)
    sendSensorData(temperature, parValue, tankName);

  // Title
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(16, 0);
  display.println("Array " + tankName);

  // Measurement headers
  display.setTextSize(1);
  display.setCursor(12, 26);
  display.print("PAR:");
  display.setCursor(90, 26);
  display.print("TEMP:");

  // Measurement values
  display.setTextSize(2);
  display.setCursor(0, 36);
  display.print(String(parValue, 1));
  display.setCursor(80, 36);
  display.print(String(temperature, 1));
  display.display();
}

float readTemperature()
{
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  if (temperatureC == DEVICE_DISCONNECTED_C)
  {
    Serial.println("Error: Could not read temperature data");
    return -999;
  }
  Serial.print("Current Temperature: ");
  Serial.println(temperatureC);
  return temperatureC;
}

float readLightSensor()
{
  float lux = lightMeter.readLightLevel();
  float par = lux / 3.05; // Very rough approximation
  if (par < 0)
  {
    Serial.println("Error: Could not read PAR data");
    return -999;
  }
  Serial.print("Current PAR: ");
  Serial.println(par);
  return par;
}
