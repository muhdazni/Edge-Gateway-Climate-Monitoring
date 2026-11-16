<h1 align="center">🌡️ AtmosSense IoT</h1>
<h3 align="center">Secure Edge-to-Cloud Climate Monitoring Gateway</h3>

<p align="center">
  <img src="https://img.shields.io/badge/ESP32-E7352C?style=for-the-badge&logo=espressif&logoColor=white" alt="ESP32" />
  <img src="https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=cplusplus&logoColor=white" alt="C++" />
  <img src="https://img.shields.io/badge/MQTT-660066?style=for-the-badge&logo=mqtt&logoColor=white" alt="MQTT" />
  <img src="https://img.shields.io/badge/EMQX-00B173?style=for-the-badge&logo=emqx&logoColor=white" alt="EMQX" />
  <img src="https://img.shields.io/badge/Node--RED-8F0000?style=for-the-badge&logo=nodered&logoColor=white" alt="Node-RED" />
  <img src="https://img.shields.io/badge/MongoDB_Atlas-47A248?style=for-the-badge&logo=mongodb&logoColor=white" alt="MongoDB Atlas" />
</p>

<p align="center">
  An end-to-end IoT system for real-time microclimate monitoring: sensor data travels from an <b>ESP32</b> edge node, through an <b>EMQX</b> MQTT broker and <b>Node-RED</b> middleware on a cloud VM, into <b>MongoDB Atlas</b> with live dashboards in <b>MongoDB Charts</b>.
</p>

---

## 📑 Table of contents

- [System overview](#-system-overview)
- [Architecture](#-architecture)
- [Hardware & services](#-hardware--services)
- [How it works](#-how-it-works)
- [Repository structure](#-repository-structure)
- [Getting started](#-getting-started)

---

## 📸 System overview

<p align="center">
  <img src="images/Assignment%20DataBase%20n%20cloud%20system.png" alt="System architecture diagram" width="85%" />
</p>

<p align="center">
  <img src="images/Node_RED.jpg" alt="Node-RED flow" width="85%" />
  <br/><sub><i>Node-RED flow: MQTT ingest → device verification → MongoDB storage → dashboard</i></sub>
</p>

---

## 🏗️ Architecture

```
 ┌──────────────┐   MQTT :1883   ┌──────────────┐          ┌──────────────┐          ┌─────────────────┐
 │  ESP32 +     │ ─────────────▶ │  EMQX broker │ ───────▶ │   Node-RED   │ ───────▶ │  MongoDB Atlas  │
 │  sensors     │ ◀───────────── │  (cloud VM)  │ ◀─────── │  :1880       │          │  + Charts       │
 └──────────────┘    commands    └──────────────┘          └──────────────┘          └─────────────────┘
     Edge layer                       Messaging                 Middleware                 Storage & UI
```

---

## 🛠️ Hardware & services

| Layer | Component | Role |
|---|---|---|
| **Edge** | ESP32 (NodeMCU / DevKit) | Reads sensors, publishes JSON telemetry over Wi-Fi |
| **Edge** | DHT11 / soil moisture sensor | Temperature, humidity and soil moisture readings |
| **Cloud** | Public cloud VM (Azure / AWS) | Hosts the broker and middleware; firewall opens ports `1883` and `1880` |
| **Messaging** | EMQX MQTT broker | Containerised broker with client authentication |
| **Middleware** | Node-RED | Parses, verifies and routes data; sends commands back to the device |
| **Database** | MongoDB Atlas | `device_metadata` (authorised devices) and `environment_telemetry` (time-series) |
| **Dashboard** | MongoDB Charts | Live gauges, historical line charts and raw data tables |

---

## ⚙️ How it works

### 1️⃣ Telemetry path (edge → cloud)

1. **Acquire:** the ESP32 reads the sensors and packs the values into a JSON payload.
2. **Publish:** the payload is sent to the EMQX broker on port `1883` using authenticated credentials.
3. **Verify:** Node-RED subscribes to the telemetry topic and checks the device against `device_metadata` in MongoDB Atlas.
4. **Store:** verified data is written to the `environment_telemetry` time-series collection through a write-only (RBAC) database user.
5. **Visualise:** MongoDB Charts reads the collection and shows live and historical data.

### 2️⃣ Command path (cloud → edge)

1. **Inject:** a command is triggered from the Node-RED flow (e.g. toggle an LED or relay).
2. **Route:** Node-RED publishes the command to a dedicated topic on EMQX.
3. **Execute:** the ESP32, subscribed to that topic, receives the command and changes the physical state.

---

## 📁 Repository structure

```
Edge-Gateway-Climate-Monitoring/
├── Soil_Monitoring_ESP32.ino   # ESP32 firmware (C++ / Arduino)
├── flows.json                  # Node-RED flow export
├── images/                     # Architecture diagram and Node-RED screenshot
└── README.md
```

---

## 🚀 Getting started

1. **Clone** the repository
   ```bash
   git clone https://github.com/muhdazni/Edge-Gateway-Climate-Monitoring.git
   ```
2. **Configure the firmware.** In `Soil_Monitoring_ESP32.ino`, replace the placeholders with your own values:
   ```cpp
   const char* ssid          = "YOUR_WIFI_SSID";
   const char* password      = "YOUR_WIFI_PASSWORD";
   const char* mqtt_server   = "YOUR_BROKER_IP";
   const char* mqtt_user     = "YOUR_MQTT_USERNAME";
   const char* mqtt_password = "YOUR_MQTT_PASSWORD";
   ```
3. **Flash** the firmware to your ESP32 using the Arduino IDE (libraries: `WiFi`, `PubSubClient`).
4. **Import** `flows.json` into Node-RED, then set your MongoDB connection string in the MongoDB config node (replace `<DB_USER>` and `<DB_PASSWORD>`).
5. **Set up MongoDB Atlas** with RBAC database users and an IP access list that allows your VM.

> ⚠️ Never commit real Wi-Fi, MQTT or database credentials. Keep them only in your local copy.

---

<p align="center">
  Built by <a href="https://github.com/muhdazni">Namiee</a> · Computer Engineering, UTeM 🎓
</p>
