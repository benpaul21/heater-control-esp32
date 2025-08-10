# ESP32 Heater Control System

## Overview
This project implements a heater control system using the **ESP32** microcontroller and a **DS18B20** digital temperature sensor. It controls a simulated heater (red LED), a status LED (green), and a buzzer for overheat alarms. The system uses a state machine and periodic tasks via FreeRTOS. It also broadcasts the current heating state via BLE advertising.

## Features
- DS18B20 temperature sensing
- State machine:
  - **IDLE** – waiting for temperature drop
  - **HEATING** – heater ON until target reached
  - **STABILIZING** – brief wait to avoid overshoot
  - **TARGET_REACHED** – heater off, maintaining temperature
  - **OVERHEAT** – heater off, buzzer ON
- BLE advertising with current state in manufacturer data
- FreeRTOS task for periodic 1-second updates
- LED and buzzer indicators
- Overheat safety limit

## Hardware
**Components**
- ESP32 Dev Module
- DS18B20 temperature sensor + 4.7kΩ pull-up resistor
- Red LED + 220Ω resistor (heater indicator)
- Green LED + 220Ω resistor (status indicator)
- Active buzzer

**Wiring**
| Component         | ESP32 GPIO | Notes                                |
|-------------------|------------|--------------------------------------|
| DS18B20 DQ        | GPIO 4     | 4.7kΩ pull-up to 3.3V                |
| Red LED (Heater)  | GPIO 16    | Heater simulation                    |
| Green LED (Status)| GPIO 17    | Mirrors heater state                 |
| Buzzer (+)        | GPIO 18    | Active buzzer, - to GND              |
| DS18B20 VCC       | 3.3V       | ---                                  |
| DS18B20 GND       | GND        | ---                                  |

**Block Diagram**

```
[DS18B20] --> [ESP32] --> [State Machine] --> [Heater LED] & [Status LED] & [Buzzer]
                                                                             --> [BLE Advertising]
```

## Software
- **Language:** Arduino C++
- **Libraries:** `OneWire`, `DallasTemperature`, `ESP32 BLE Arduino`
- **Structure:**
  - `setup()` initializes peripherals, DS18B20, BLE
  - `SensorTask` (FreeRTOS) runs every second, reads temperature, updates state, logs via Serial, updates BLE advertisement
- **BLE:** Advertises `HTR:<STATE>` in manufacturer data, visible in BLE scanner apps

## State Transitions
- **IDLE → HEATING** if `temp < target - hysteresis`
- **HEATING → STABILIZING** if `temp >= target`
- **STABILIZING → TARGET_REACHED** after 30s
- **TARGET_REACHED → HEATING** if `temp < target - hysteresis`
- **Any → OVERHEAT** if `temp >= overheat limit`

## Future Improvements
- BLE GATT characteristic with notifications
- Web-based dashboard (ESP32 Wi‑Fi)
- Persistent temperature logging to SD card

## Safety Notes
- This simulation uses LEDs as heater loads; in a real heater, use proper power electronics and safety features (fuses, relays, watchdogs).
