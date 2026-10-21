#include "Hardware/Input/BatteryVoltage.hpp"
#include <Arduino.h>

BatteryVoltage::BatteryVoltage(int pin) {
  this->pin = pin;
  this->voltage = 0.0f;
}

void BatteryVoltage::Begin() {
  pinMode(pin, INPUT);
  UpdateValues();
}

void BatteryVoltage::UpdateValues() {
  int rawValue = analogRead(pin);

  float vOut = (rawValue / 4095.0f) * 3.3f;

  voltage = vOut * ((R1 + R2) / R2);
}

float BatteryVoltage::GetVoltage() { return voltage; }
