<p align="center">
  <img src="https://img.shields.io/badge/Platform-Arduino-teal?style=for-the-badge&logo=arduino&logoColor=white" alt="Arduino"/>
  <img src="https://img.shields.io/badge/Sensor-DHT11-blue?style=for-the-badge" alt="DHT11"/>
  <img src="https://img.shields.io/badge/Control-Automatic-orange?style=for-the-badge" alt="Control"/>
  <img src="https://img.shields.io/badge/Category-Embedded-green?style=for-the-badge" alt="Embedded"/>
</p>

# ❄️ Temperature Controlled Air Conditioning System

> An Arduino-based automatic temperature control system with hysteresis-based cooling, compressor protection, setpoint adjustment, and multiple operating modes.

---

## 🔍 Overview

This project implements an **intelligent temperature-based cooling control system** that automatically manages fan and compressor relays based on ambient temperature. It features hysteresis-based control to prevent rapid cycling, compressor protection timers, and three operating modes.

### Key Highlights
- 🌡️ **DHT11 Sensing** — Real-time temperature and humidity
- 🎯 **Adjustable Setpoint** — Potentiometer control (18-32°C)
- 🔄 **Hysteresis Control** — Prevents relay chattering
- 🛡️ **Compressor Protection** — 3-minute minimum off-time
- 🎮 **3 Modes** — Auto / Manual Cool / Off
- 💾 **EEPROM Persistence** — Remembers last setpoint
- 📺 **LCD Dashboard** — Live temp, setpoint, and status
- 🌬️ **Fan Rundown** — Fan continues 30s after compressor stops

---

## 🏗️ Control Flow

```
┌──────────────────────────────────────────────────┐
│                 AUTO MODE LOGIC                   │
│                                                    │
│   Temp > Setpoint + 2°C?                          │
│        │                                          │
│    YES ▼               NO                         │
│   ┌─────────────┐   ┌─────────────┐              │
│   │ FULL_COOLING │   │ COOLING_OFF │              │
│   │ Fan + Comp   │   │ Everything  │              │
│   │ ON           │   │ OFF         │              │
│   └──────┬──────┘   └─────────────┘              │
│          │                                        │
│   Temp < Setpoint - 2°C?                          │
│          │                                        │
│      YES ▼                                        │
│   ┌─────────────┐   ┌─────────────┐              │
│   │ FAN_RUNDOWN  │──►│ COOLING_OFF │              │
│   │ Fan ON 30s   │   │             │              │
│   │ Comp OFF     │   │             │              │
│   └─────────────┘   └─────────────┘              │
└──────────────────────────────────────────────────┘
```

---

## 🔌 Circuit Connections

| Component | Arduino Pin | Description |
|:---------:|:-----------:|:------------|
| DHT11 | D2 | Temperature sensor |
| Fan Relay | D8 | Fan control |
| Compressor Relay | D9 | Compressor control |
| Potentiometer | A0 | Setpoint adjustment |
| Mode Button | D3 | Mode cycling |
| Blue LED | D5 | Cooling active |
| Green LED | D6 | System idle |
| Red LED | D7 | Overheat warning |

---

## 📁 File Structure

```
Embedded-Temperature-Controlled-AC/
├── src/
│   └── main.cpp
├── docs/
├── platformio.ini
├── .gitignore
├── LICENSE
└── README.md
```

---

## 🚀 Getting Started

```bash
git clone https://github.com/Harikrishna_08/Embedded-Temperature-Controlled-AC.git
cd Embedded-Temperature-Controlled-AC
pio run --target upload
pio device monitor
```

---

## 👨‍💻 Author

**Daggolu Hari Krishna** — B.Tech ECE | JNTUA College of Engineering, Kalikiri

[![LinkedIn](https://img.shields.io/badge/LinkedIn-Connect-blue?style=flat-square&logo=linkedin)](https://linkedin.com/in/harikrishnadaggolu)
[![GitHub](https://img.shields.io/badge/GitHub-Harikrishna__08-black?style=flat-square&logo=github)](https://github.com/Harikrishna_08)

---

<p align="center">⭐ Star this repo if you found it useful! ⭐</p>
