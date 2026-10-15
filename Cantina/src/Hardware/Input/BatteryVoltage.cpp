#include "Hardware/Input/BatteryVoltage.hpp"
// 34
float R1 = 10000.0; // Resistor value in ohms
float R2 = 10000.0; // Resistor value in ohms
BatteryVoltage::BatteryVoltage(int pin) {
  this->pin = pin;
  this->voltage = 0.0f; // Initialize voltage to 0.0
}

void BatteryVoltage::Begin() {
  pinMode(pin, INPUT);
  UpdateValues(); // Read initial voltage
}

void BatteryVoltage::UpdateValues() {
  int rawValue = analogRead(pin);
  float vOut = (rawValue / 4095.0) * 3.3;

  // Calculate the original input voltage using the voltage divider formula
  float vIn = vOut * ((R1 + R2) / R2);
  voltage = vIn;
}
