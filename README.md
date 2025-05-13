

# 🌿 EcoPulse: Intelligent Soil Moisture Control & Light-Augmented Growth System

EcoPulse is a smart agriculture project powered by the ESP8266 microcontroller. It automates irrigation based on real-time soil moisture analysis and simulates photoperiod conditions by triggering artificial light during low-light scenarios. The system integrates weather API data and cloud-based remote monitoring using Arduino IoT Cloud.

---

## 🚀 Features

* 🌱 Soil moisture sensing via resistive analog sensor
* 💧 Automatic water pump control via relay (based on soil dryness threshold)
* 🌡️ Real-time temperature updates using WeatherAPI
* ☁️ Remote monitoring via Arduino IoT Cloud (temperature, moisture, pump status)
* 🔆 Night-mode photosynthesis support (via external photosensor-controlled lighting)
* 📲 View sensor data and control system from your smartphone (via Arduino IoT Remote app)

---

## 📦 Components Used

| Component                     | Purpose                               |
| ----------------------------- | ------------------------------------- |
| ESP8266 (NodeMCU)             | Wi-Fi-enabled microcontroller         |
| Soil Moisture Sensor (Analog) | Measures volumetric water content     |
| Relay Module (5V)             | Controls irrigation pump              |
| Submersible Pump              | Irrigates the soil                    |
| 5V LED Grow Lights            | Mimics daylight for photosynthesis    |
| LDR / Photosensor Circuit     | Detects ambient light for night logic |
| WeatherAPI                    | Fetches ambient temperature remotely  |
| Arduino IoT Cloud             | Syncs variables & remote dashboard    |

---

## 🧠 System Architecture

```
[ Soil Sensor ] ───┐
                   ├──→ [ ESP8266 NodeMCU ] ──→ [ Relay Module ] ──→ [ Pump ]
[ Weather API ] ───┘         ↓
                        [ Arduino Cloud ]
                             ↑
[ LDR Sensor + Light Logic ] (External: Auto night light for photosynthesis)
```

---

## 📲 Remote Monitoring Dashboard

Once connected to Arduino IoT Cloud, you can:

* Visualize real-time soil moisture and temperature
* Manually toggle the irrigation pump
* Receive environmental telemetry on your mobile device

All variables are synced and displayed via the Arduino IoT Remote app.

---

## 🛠️ Setup Instructions

1. Clone the repository:

   ```bash
   git clone https://github.com/yourusername/ecopulse.git
   ```

2. Open the .ino file in the Arduino IDE.

3. Install required libraries:

   * ArduinoIoTCloud
   * Arduino\_ConnectionHandler
   * ESP8266WiFi
   * ESP8266HTTPClient
   * ArduinoJson

4. Replace the placeholder strings in the code:

   ```cpp
   const char SSID[] = "your-wifi-ssid";
   const char PASS[] = "your-wifi-password";
   const String apiKey = "your-weatherapi-key";
   const char DEVICE_LOGIN_NAME[] = "your-device-id";
   const char DEVICE_KEY[] = "your-device-key";
   ```

5. Upload the code to your ESP8266 board.

6. Open Arduino IoT Cloud and configure your variables:

   * soil\_Moisture (int)
   * pumpStatus (bool)
   * temperature (float)

---

## 🔆 Photoperiod Extension (Light Logic)

Although the ESP8266 code doesn’t directly control grow lights, a separate light-sensitive subsystem can activate grow LEDs when ambient illumination drops (e.g., during night).

This is achieved via:

* LDR module
* Transistor/MOSFET switch for LED array
* Optional manual override logic

For more advanced integration, a second ESP8266 or analog comparator circuit may be used.

---

## 🔐 Security Notes

* Avoid pushing sensitive credentials (API keys, Wi-Fi passwords) to public repos.
* Use environment variables or a separate config file like secrets.h and add it to .gitignore.

---

## 🌾 Future Improvements

* Add water level sensing in tank
* Use DHT11/22 for local temperature/humidity fallback
* Integrate RTC module for time-based lighting
* Solar-powered operation with battery backup

---



## 📜 License

This project is licensed under the MIT License. Feel free to use, modify, and distribute with attribution.

-
