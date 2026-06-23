# 🔌 SyncSwitch v4.1.0 – Smart Switch Board

An advanced IoT-based smart switchboard system designed for **home automation + environmental safety monitoring**, built using ESP32 and hybrid cloud architecture.

---

## 📌 Project Overview

**SyncSwitch v4.1.0** addresses a major limitation in traditional smart switches — **single-point failure** — by implementing a **triple-redundant control system**.

### 🔁 Control Methods
- Physical switches  
- Infrared (IR) remote  
- Cloud control (Mobile + Voice)

### 🔥 Safety Features
- Gas & smoke detection using MQ-2 sensor  
- Real-time buzzer alert system  

### ☁️ Cloud Architecture
- **Sinric Pro** → Real-time device control  
- **Firebase RTDB** → Event logging & analytics  

---

## ⚙️ Hardware Components

- ESP32 (Wi-Fi + Bluetooth microcontroller)
- MQ-2 Gas & Smoke Sensor
- TSOP IR Receiver (38kHz)
- 3-Channel Relay Module
- Buzzer

---

## 🔌 ESP32 Pin Configuration

| Component | GPIO |
|----------|------|
| IR Receiver | 15 |
| Buzzer | 13 |
| Gas Sensor | 34 |
| Relay 1 | 2 |
| Relay 2 | 4 |
| Relay 3 | 5 |
| Switches | 14, 27, 26 |

---

## 🧠 Software Architecture

### 1. Sinric Pro (Control Layer)
- WebSocket communication
- Alexa & Google Assistant support
- Fast response (<500ms)

### 2. Firebase RTDB (Analytics Layer)
- Logs all events (switches, gas levels, connectivity)
- Uses REST API
- Timestamp synchronization using NTP

### 3. Local State Machine
- Non-blocking code using `millis()`
- Central state array for synchronization

---

## 🚀 Features

- Multi-mode control (Switch + IR + App)
- Real-time gas detection
- Audible alarm system
- Offline functionality (no Wi-Fi required for local control)
- Fast response time (<150ms local)
- Cloud logging system

---

## 📖 Working Principle

### 🔹 Startup
- MQ-2 sensor calibration (~60 seconds)
- Automatic Wi-Fi connection
- System boot logged to Firebase

### 🔹 Control Modes
- Mobile app (Sinric Pro)
- IR remote
- Manual switches

### 🔹 Safety Mode
- Gas value > 4100 → Alarm triggered
- Continuous buzzer alert
- Event logged in Firebase

---

## 🛠️ Maintenance

- Avoid rapid relay switching
- Keep MQ-2 sensor clean
- Device works even if Wi-Fi disconnects

---

## 🔮 Future Scope

- Dedicated mobile application
- AI-based gas prediction system
- Custom PCB design
- Additional sensors (temperature, motion, humidity)
- OTA (Over-the-Air) updates
- Enhanced security features

---

## 👥 Team Members (Group 4 – Section C)

| Name | Roll Number |
|------|------------|
| Ashwin Jain | 125AD0039 |
| Anshu Raj | 125EC0037 |
| Partha Biswas | 125AD0035 |
| Samridh Verma | 125ME0041 |
| Vasu Jangid | 125CS0044 |

---

## 🏫 Institution

**Indian Institute of Information Technology, Design and Manufacturing, Kurnool (IIITDM Kurnool)**

---

## 🙏 Acknowledgements

- Dr. Ravi Kumar (Course Instructor)
- Chitti Babu (Guiding Faculty)
- Teaching Assistants (TAs)

---

## 📜 License

This project is developed for academic purposes under **DM104 – Design Realization Practice**.
