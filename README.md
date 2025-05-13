# Ecopulse
🌱 EcoPulse: Intelligent Soil Moisture Control & Photoperiod Emulation System
EcoPulse is a sophisticated edge-IoT system engineered to optimize plant vitality through real-time soil hydration monitoring and artificial photoperiod simulation. This embedded project, powered by the ESP8266 microcontroller, bridges precision agriculture with smart automation using cloud-integrated data exchange, environment-aware irrigation logic, and adaptive light-based photosynthesis support.

🧠 Project Overview
The EcoPulse system consists of multiple integrated subsystems, each designed to monitor and react to environmental conditions autonomously:

Smart Irrigation Control:
A resistive soil moisture sensor continuously samples the volumetric water content of the surrounding soil. When the calibrated threshold is breached (e.g., moisture < 30%), a relay module activates an electric water pump to irrigate the soil until adequate hydration is restored. The pump status is controlled both autonomously and remotely via Arduino IoT Cloud.

Weather-Responsive Intelligence:
To provide contextual awareness of macroclimatic conditions, the system fetches real-time meteorological data from the WeatherAPI. Specifically, it parses the ambient temperature of a specified field location (Jessore, BD), which is then published to the cloud interface for remote visualization via a mobile dashboard.

Photonic Growth Support (External Subsystem):
Outside the ESP8266's domain but integrated into the broader ecosystem, a dedicated photosensor array detects daylight levels. Once ambient illumination falls below a photometric threshold (e.g., nightfall), an external circuit activates narrow-spectrum artificial grow lights (potentially red/blue wavelength LEDs). These lights emulate natural sunlight to prolong photosynthetic activity during dark cycles—a mechanism inspired by controlled greenhouse environments.

🛠️ Key Technologies & Components
ESP8266 NodeMCU (Wi-Fi-enabled microcontroller)

Soil Moisture Sensor (analog, resistive type)

Relay Module (for pump switching)

DHT11/DHT22 (optional) for physical temperature/humidity sensing

Weather API Integration (RESTful JSON-based HTTP requests)

Arduino IoT Cloud (variable syncing, device management, remote visualization)

Photosensor & LED Driver Circuitry (external to this ESP module)

Power Supply (e.g., solar panel or regulated 5V USB)

Mobile Dashboard (Arduino IoT Remote App)

⚙️ Control Logic Summary
Reads moisture → Decides pump ON/OFF

Syncs temperature → Sends to Arduino Cloud

Remote control enabled via cloud variables

Polls external weather API every 60 seconds

External photosensor subsystem lights up night-time plants

💡 Potential Applications
Precision agriculture in rural farmlands

Remote-controlled irrigation for urban rooftop gardens

Controlled-environment greenhouses

Plant research and botany experiments

🔐 Notes
Ensure secure storage of your API key and Wi-Fi credentials in production deployments.

Consider modularizing the photosensor logic via I2C or SPI if integrating with the main ESP8266 board.

This project is intended for educational and experimental prototyping.
