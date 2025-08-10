# ESP32 Heater Control System

## Description
ESP32-based heater control using DS18B20 sensor, BLE advertising, FreeRTOS tasks, and LED/Buzzer indicators.

## Features
- Temperature-based state machine
- BLE advertising of current state
- FreeRTOS periodic task
- Overheat alarm

## Requirements
- ESP32 Dev Module
- Arduino IDE with ESP32 board support
- Libraries:
  - OneWire
  - DallasTemperature
  - ESP32 BLE Arduino

## Wiring
See DESIGN.md for wiring table.

## Usage
1. Open `src/heater_control_esp32.ino` in Arduino IDE.
2. Select **ESP32 Dev Module**.
3. Install required libraries.
4. Upload to ESP32.
5. Monitor output via Serial (115200 baud).
6. Scan BLE devices with a phone — find `Heater-Control`, view manufacturer data for current state.

## Simulation
Wokwi simulates the DS18B20 and ESP32; BLE advertising may not be visible in the simulator.

## License
MIT License
