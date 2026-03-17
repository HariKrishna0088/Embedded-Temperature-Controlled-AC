<p align="center">
  <img src="https://img.shields.io/badge/Platform-Arduino-teal?style=for-the-badge&logo=arduino&logoColor=white" alt="Arduino"/>
  <img src="https://img.shields.io/badge/Sensor-DHT11-blue?style=for-the-badge" alt="DHT11"/>
  <img src="https://img.shields.io/badge/Control-Automatic-orange?style=for-the-badge" alt="Control"/>
  <img src="https://img.shields.io/badge/Category-Embedded-green?style=for-the-badge" alt="Embedded"/>
</p>

# â„ï¸ Temperature Controlled Air Conditioning System

> An Arduino-based automatic temperature control system with hysteresis-based cooling, compressor protection, setpoint adjustment, and multiple operating modes.

---

## ðŸ” Overview

This project implements an **intelligent temperature-based cooling control system** that automatically manages fan and compressor relays based on ambient temperature. It features hysteresis-based control to prevent rapid cycling, compressor protection timers, and three operating modes.

### Key Highlights
- ðŸŒ¡ï¸ **DHT11 Sensing** â€” Real-time temperature and humidity
- ðŸŽ¯ **Adjustable Setpoint** â€” Potentiometer control (18-32Â°C)
- ðŸ”„ **Hysteresis Control** â€” Prevents relay chattering
- ðŸ›¡ï¸ **Compressor Protection** â€” 3-minute minimum off-time
- ðŸŽ® **3 Modes** â€” Auto / Manual Cool / Off
- ðŸ’¾ **EEPROM Persistence** â€” Remembers last setpoint
- ðŸ“º **LCD Dashboard** â€” Live temp, setpoint, and status
- ðŸŒ¬ï¸ **Fan Rundown** â€” Fan continues 30s after compressor stops

---

## ðŸ—ï¸ Control Flow

```
â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”
â”‚                 AUTO MODE LOGIC                   â”‚
â”‚                                                    â”‚
â”‚   Temp > Setpoint + 2Â°C?                          â”‚
â”‚        â”‚                                          â”‚
â”‚    YES â–¼               NO                         â”‚
â”‚   â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”   â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”              â”‚
â”‚   â”‚ FULL_COOLING â”‚   â”‚ COOLING_OFF â”‚              â”‚
â”‚   â”‚ Fan + Comp   â”‚   â”‚ Everything  â”‚              â”‚
â”‚   â”‚ ON           â”‚   â”‚ OFF         â”‚              â”‚
â”‚   â””â”€â”€â”€â”€â”€â”€â”¬â”€â”€â”€â”€â”€â”€â”˜   â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜              â”‚
â”‚          â”‚                                        â”‚
â”‚   Temp < Setpoint - 2Â°C?                          â”‚
â”‚          â”‚                                        â”‚
â”‚      YES â–¼                                        â”‚
â”‚   â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”   â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”              â”‚
â”‚   â”‚ FAN_RUNDOWN  â”‚â”€â”€â–ºâ”‚ COOLING_OFF â”‚              â”‚
â”‚   â”‚ Fan ON 30s   â”‚   â”‚             â”‚              â”‚
â”‚   â”‚ Comp OFF     â”‚   â”‚             â”‚              â”‚
â”‚   â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜   â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜              â”‚
â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜
```

---

## ðŸ”Œ Circuit Connections

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

## ðŸ“ File Structure

```
Embedded-Temperature-Controlled-AC/
â”œâ”€â”€ src/
â”‚   â””â”€â”€ main.cpp
â”œâ”€â”€ docs/
â”œâ”€â”€ platformio.ini
â”œâ”€â”€ .gitignore
â”œâ”€â”€ LICENSE
â””â”€â”€ README.md
```

---

## ðŸš€ Getting Started

```bash
git clone https://github.com/Harikrishna_08/Embedded-Temperature-Controlled-AC.git
cd Embedded-Temperature-Controlled-AC
pio run --target upload
pio device monitor
```

---

## ðŸ‘¨â€ðŸ’» Author

**Daggolu Hari Krishna** â€” B.Tech ECE | JNTUA College of Engineering, Kalikiri

[![LinkedIn](https://img.shields.io/badge/LinkedIn-Connect-blue?style=flat-square&logo=linkedin)](https://www.linkedin.com/in/contacthari88/)
[![GitHub](https://img.shields.io/badge/GitHub-Harikrishna__08-black?style=flat-square&logo=github)](https://github.com/Harikrishna_08)

---

<p align="center">â­ Star this repo if you found it useful! â­</p>
