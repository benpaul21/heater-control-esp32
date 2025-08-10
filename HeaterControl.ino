
#include <OneWire.h>
#include <DallasTemperature.h>

// =================== Pin Configuration ===================
constexpr uint8_t PIN_ONEWIRE  = 4;   // DS18B20 data pin
constexpr uint8_t PIN_HEATER   = 16;  // Heater LED (Red)
constexpr uint8_t PIN_STATUS   = 17;  // Status LED (Green)
constexpr uint8_t PIN_BUZZER   = 18;  // Alarm buzzer

// =================== Temperature Settings ===================
constexpr float TARGET_TEMP     = 60.0; // °C – target temperature
constexpr float HYSTERESIS      = 1.5;  // °C – to prevent rapid switching
constexpr float OVERHEAT_LIMIT  = 85.0; // °C – safety cutoff

// =================== Timing Settings ===================
constexpr unsigned long SAMPLE_INTERVAL_MS = 1000;  // 1 second
constexpr unsigned long STABILIZE_MS       = 30000; // 30 seconds

// =================== DS18B20 Setup ===================
OneWire oneWire(PIN_ONEWIRE);
DallasTemperature sensors(&oneWire);

// =================== State Machine ===================
enum class State { IDLE, HEATING, STABILIZING, TARGET_REACHED, OVERHEAT };
State currentState = State::IDLE;

unsigned long lastSampleTime     = 0;
unsigned long stabilizeStartTime = 0;

// =================== Helper Functions ===================
float readTemperatureC() {
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);
  if (temp == DEVICE_DISCONNECTED_C) {
    Serial.println("ERROR: DS18B20 not detected!");
    return NAN;
  }
  return temp;
}

void setHeater(bool on) {
  digitalWrite(PIN_HEATER, on ? HIGH : LOW);
  digitalWrite(PIN_STATUS, on ? HIGH : LOW); // Green LED mirrors heater state
}

void setAlarm(bool on) {
  digitalWrite(PIN_BUZZER, on ? HIGH : LOW);
}

void logStatus(float temp, State state) {
  static const char* stateNames[] = {
    "IDLE", "HEATING", "STABILIZING", "TARGET_REACHED", "OVERHEAT"
  };
  
  Serial.printf("%8lu ms | Temp: %.1f°C | Target: %.1f°C | State: %-15s | Heater: %s\n",
                millis(), temp, TARGET_TEMP, stateNames[(int)state],
                digitalRead(PIN_HEATER) ? "ON" : "OFF");
}

// =================== Setup ===================
void setup() {
  Serial.begin(115200);

  pinMode(PIN_HEATER, OUTPUT);
  pinMode(PIN_STATUS, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  setHeater(false);
  setAlarm(false);

  sensors.begin();
  Serial.println("=== ESP32 Heater Control System Started ===");
}

// =================== Main Loop ===================
void loop() {
  unsigned long now = millis();
  if (now - lastSampleTime < SAMPLE_INTERVAL_MS) return; // Wait until next sample
  lastSampleTime = now;

  float temp = readTemperatureC();
  if (isnan(temp)) return; // Skip if sensor error

  // Overheat Safety
  if (temp >= OVERHEAT_LIMIT) {
    currentState = State::OVERHEAT;
    setHeater(false);
    setAlarm(true);
    logStatus(temp, currentState);
    return;
  }
  setAlarm(false);

  // State Machine
  switch (currentState) {
    case State::IDLE:
      if (temp < TARGET_TEMP - HYSTERESIS) {
        currentState = State::HEATING;
        setHeater(true);
      } else {
        setHeater(false);
      }
      break;

    case State::HEATING:
      if (temp >= TARGET_TEMP) {
        currentState = State::STABILIZING;
        stabilizeStartTime = now;
        setHeater(false);
      } else {
        setHeater(true);
      }
      break;

    case State::STABILIZING:
      setHeater(false);
      if (now - stabilizeStartTime >= STABILIZE_MS) {
        currentState = State::TARGET_REACHED;
      }
      break;

    case State::TARGET_REACHED:
      if (temp < TARGET_TEMP - HYSTERESIS) {
        currentState = State::HEATING;
        setHeater(true);
      } else {
        setHeater(false);
      }
      break;

    case State::OVERHEAT:
      setHeater(false);
      setAlarm(true);
      break;
  }

  logStatus(temp, currentState);
}
