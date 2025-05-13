#include <ArduinoIoTCloud.h>            // IoT Cloud SDK for remote monitoring and control
#include <Arduino_ConnectionHandler.h>  // Handles WiFi + cloud communications
#include <ESP8266WiFi.h>                // Native ESP8266 WiFi functions
#include <ESP8266HTTPClient.h>          // HTTP client for REST API consumption
#include <WiFiClient.h>
#include <ArduinoJson.h>                // For JSON parsing of weather API response

// ─────────────────────────────────────
// Device + Cloud Identity Configuration
// ─────────────────────────────────────
const char DEVICE_LOGIN_NAME[] = "your-device-id-here"; // Replace with your Arduino IoT Cloud board ID
const char DEVICE_KEY[]        = "your-secret-device-key"; // Secret auth key for secure cloud sync

// ─────────────────────────────────────
// WiFi Network Configuration (Securely store outside source code in practice)
// ─────────────────────────────────────
const char SSID[] = "your-wifi-ssid";           // Replace with your WiFi SSID
const char PASS[] = "your-wifi-password";       // Replace with your WiFi password

// ─────────────────────────────────────
// Weather API Configuration
// ─────────────────────────────────────
const String apiKey = "your-weatherapi-key";   // Obtain your free API key from weatherapi.com
const String location = "Jessore,BD";          // Target location for weather data

// ─────────────────────────────────────
// Cloud-Synchronized Variables
// ─────────────────────────────────────
int soil_Moisture;       // Cloud-synced soil moisture percentage (0–100%)
bool pumpStatus;         // Reflects real-time irrigation state
float temperature;       // Ambient temperature (°C) from cloud or API

// ─────────────────────────────────────
// Hardware Interface Pin Assignments
// ─────────────────────────────────────
const int moisturePin = A0;   // Analog pin for resistive soil moisture sensor
const int relayPin    = D1;   // GPIO controlling the irrigation pump relay

// ─────────────────────────────────────
// Forward Declaration of Cloud Event Handlers
// ─────────────────────────────────────
void onSoilMoistureChange();
void onPumpStatusChange();
void onTemperatureChange();

// ─────────────────────────────────────
// Cloud Variable Registration and Setup
// ─────────────────────────────────────
void initProperties() {
  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);

  ArduinoCloud.addProperty(soil_Moisture, READWRITE, ON_CHANGE, onSoilMoistureChange);
  ArduinoCloud.addProperty(pumpStatus, READWRITE, ON_CHANGE, onPumpStatusChange);
  ArduinoCloud.addProperty(temperature, READ, ON_CHANGE, onTemperatureChange);
}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);

// ─────────────────────────────────────
// Initialization Sequence
// ─────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(1500);  // Wait for serial console to initialize

  pinMode(moisturePin, INPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);  // Ensure pump is OFF initially

  // WiFi Initialization with retry logic
  WiFi.begin(SSID, PASS);
  int attempts = 0;
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi connected. IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("\nWi-Fi connection failed!");
    return;
  }

  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
}

// ─────────────────────────────────────
// Main Execution Loop (Runs every ~1s)
// ─────────────────────────────────────
void loop() {
  ArduinoCloud.update();

  // ── Moisture Sensing & Conversion ──
  int rawValue = analogRead(moisturePin);
  soil_Moisture = map(rawValue, 1023, 0, 0, 100);  // Map dry-to-wet scale
  soil_Moisture = constrain(soil_Moisture, 0, 100);

  // ── Conditional Irrigation Logic ──
  if (soil_Moisture < 30 && !pumpStatus) {
    pumpStatus = true;
  } else if (soil_Moisture >= 30 && pumpStatus) {
    pumpStatus = false;
  }
  digitalWrite(relayPin, pumpStatus ? HIGH : LOW);

  // ── Temperature Update from API (Every 60 seconds) ──
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 60000) {
    getWeatherTemperature();  // Async HTTP JSON call
    lastUpdate = millis();
  }

  delay(1000);
}

// ─────────────────────────────────────
// RESTful Weather API Consumption Logic
// ─────────────────────────────────────
void getWeatherTemperature() {
  WiFiClient client;
  HTTPClient http;

  String url = "http://api.weatherapi.com/v1/current.json?key=" + apiKey + "&q=" + location + "&aqi=no";
  Serial.println("Requesting: " + url);

  http.begin(client, url);
  int httpCode = http.GET();

  Serial.print("HTTP code: ");
  Serial.println(httpCode);

  if (httpCode == 200) {
    String payload = http.getString();
    Serial.println("Received JSON:");
    Serial.println(payload);

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
      temperature = doc["current"]["temp_c"];
      Serial.println("Parsed Temperature: " + String(temperature));
    } else {
      Serial.print("JSON parse error: ");
      Serial.println(error.c_str());
    }
  } else {
    Serial.print("HTTP error: ");
    Serial.println(httpCode);
  }

  http.end();
}

// ─────────────────────────────────────
// Callback Functions (Triggered on cloud variable changes)
// ─────────────────────────────────────
void onSoilMoistureChange() {
  Serial.println("Soil moisture: " + String(soil_Moisture) + "%");
}

void onPumpStatusChange() {
  Serial.println(pumpStatus ? "Pump turned ON (remote)" : "Pump turned OFF (remote)");
}

void onTemperatureChange() {
  Serial.println("Temp (Cloud Updated): " + String(temperature));
}
