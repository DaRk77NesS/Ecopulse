#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include <ArduinoJson.h>

// Credentials (keep outside source code in production)
const char SSID[] = "your-ssid";
const char PASS[] = "your-password";
const char DEVICE_LOGIN_NAME[] = "your-device-id";
const char DEVICE_KEY[] = "your-device-key";

// API
const String apiKey = "your-api-key";
const String location = "Jessore,BD";

// Pins
const int moisturePin = A0;
const int relayPin = D1;

// Cloud Variables
int soil_Moisture;
bool pumpStatus;
float temperature;

void onSoilMoistureChange() {}
void onPumpStatusChange() {}
void onTemperatureChange() {}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);

void initProperties() {
  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);
  ArduinoCloud.addProperty(soil_Moisture, READWRITE, ON_CHANGE, onSoilMoistureChange);
  ArduinoCloud.addProperty(pumpStatus, READWRITE, ON_CHANGE, onPumpStatusChange);
  ArduinoCloud.addProperty(temperature, READ, ON_CHANGE, onTemperatureChange);
}

void setup() {
  Serial.begin(115200);
  pinMode(moisturePin, INPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);  // Ensure pump is off at boot

  connectToWiFiWithFailSafe();
  if (WiFi.status() == WL_CONNECTED) {
    initProperties();
    ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    offlineFailSafeIrrigation();
  } else {
    ArduinoCloud.update();

    int raw = analogRead(moisturePin);
    soil_Moisture = map(raw, 1023, 0, 0, 100);
    soil_Moisture = constrain(soil_Moisture, 0, 100);

    if (soil_Moisture < 30 && !pumpStatus) {
      pumpStatus = true;
    } else if (soil_Moisture >= 30 && pumpStatus) {
      pumpStatus = false;
    }
    digitalWrite(relayPin, pumpStatus ? HIGH : LOW);

    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 60000) {
      getWeatherTemperature();
      lastUpdate = millis();
    }

    delay(1000);
  }
}

void connectToWiFiWithFailSafe() {
  WiFi.begin(SSID, PASS);
  Serial.print("Connecting to Wi-Fi");
  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 30000) {
    Serial.print(".");
    delay(1000);
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi connected");
  } else {
    Serial.println("\nWi-Fi failed. Entering offline mode.");
  }
}

void offlineFailSafeIrrigation() {
  static unsigned long lastPumpTime = 0;
  static bool inPumpCycle = false;
  const unsigned long pumpDuration = 10000;      // Run pump for 10 seconds
  const unsigned long offlineCycleDelay = 30UL * 60UL * 1000UL; // 30 minutes

  unsigned long now = millis();

  if (!inPumpCycle && now - lastPumpTime > offlineCycleDelay) {
    Serial.println("Offline Mode: Activating emergency irrigation");
    digitalWrite(relayPin, HIGH);
    inPumpCycle = true;
    lastPumpTime = now;
  }

  if (inPumpCycle && now - lastPumpTime > pumpDuration) {
    digitalWrite(relayPin, LOW);
    inPumpCycle = false;
    Serial.println("Offline Mode: Irrigation cycle complete");
  }

  delay(1000); // Minor delay to reduce CPU churn
}

void getWeatherTemperature() {
  WiFiClient client;
  HTTPClient http;

  String url = "http://api.weatherapi.com/v1/current.json?key=" + apiKey + "&q=" + location + "&aqi=no";
  http.begin(client, url);
  int httpCode = http.GET();

  if (httpCode == 200) {
    String payload = http.getString();
    StaticJsonDocument<1024> doc;
    if (!deserializeJson(doc, payload)) {
      temperature = doc["current"]["temp_c"];
    }
  }
  http.end();
}
