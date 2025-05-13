#define BLYNK_TEMPLATE_ID "YourTemplateID"
#define BLYNK_TEMPLATE_NAME "EcoPulse"
#define BLYNK_AUTH_TOKEN "YourAuthToken"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

// Replace with your Wi-Fi credentials
char ssid[] = "YourWiFiSSID";      
char pass[] = "YourWiFiPassword";  

// Blynk Auth Token
char auth[] = BLYNK_AUTH_TOKEN;

// Weather API key & location
const String apiKey = "your-weatherapi-key";  
const String location = "Jessore,BD";

// Hardware pins
const int moisturePin = A0;
const int relayPin = D1;

// Global values
int soilMoisture = 0;
float temperature = 0.0;
bool pumpState = false;
bool manualOverride = false;

// Blynk virtual pin handlers
BLYNK_WRITE(V3) {
  manualOverride = param.asInt();  // Read manual switch
  pumpState = manualOverride;
  digitalWrite(relayPin, pumpState ? HIGH : LOW);
  Blynk.virtualWrite(V2, pumpState);  // Reflect pump state
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(moisturePin, INPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");

  Blynk.begin(auth, ssid, pass);

  // Optional: Run initial temperature fetch
  getWeatherTemperature();
}

void loop() {
  Blynk.run();
  updateSoilAndPump();

  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 60000) {  // Every 60 seconds
    getWeatherTemperature();
    lastUpdate = millis();
  }

  delay(1000);
}

void updateSoilAndPump() {
  int rawValue = analogRead(moisturePin);
  soilMoisture = map(rawValue, 1023, 0, 0, 100);
  soilMoisture = constrain(soilMoisture, 0, 100);
  Blynk.virtualWrite(V0, soilMoisture);

  // Auto pump logic only if manual override is off
  if (!manualOverride) {
    if (soilMoisture < 30 && !pumpState) {
      pumpState = true;
    } else if (soilMoisture >= 30 && pumpState) {
      pumpState = false;
    }
    digitalWrite(relayPin, pumpState ? HIGH : LOW);
    Blynk.virtualWrite(V2, pumpState);
    Blynk.virtualWrite(V3, pumpState);  // Keep manual toggle in sync
  }

  Serial.print("Soil Moisture: ");
  Serial.print(soilMoisture);
  Serial.print("%, Pump: ");
  Serial.println(pumpState ? "ON" : "OFF");
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
    DeserializationError error = deserializeJson(doc, payload);
    if (!error) {
      temperature = doc["current"]["temp_c"];
      Blynk.virtualWrite(V1, temperature);
      Serial.println("WeatherAPI Temperature: " + String(temperature) + "°C");
    } else {
      Serial.println("JSON parse error");
    }
  } else {
    Serial.print("HTTP error code: ");
    Serial.println(httpCode);
  }
  http.end();
}
