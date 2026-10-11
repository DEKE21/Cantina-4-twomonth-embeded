#include "Hardware/Input/SolarPanel.hpp"

SolarPanel::SolarPanel(int pin1, int pin2) {
    this->pin1 = pin1;
    this->pin2 = pin2;
    //prevent null values
    this->voltage1 = 0;
    this->voltage2 = 0;

}

void SolarPanel::Begin() {
  pinMode(pin1, INPUT);
  pinMode(pin2, INPUT);
}

void SolarPanel::UpdateValues() {
   float vRaw1 = analogRead(pin1);
   float vRaw2 = analogRead(pin2);
  voltage1 = (vRaw1  / 4095.0) * 3.3; 
  voltage2 = (vRaw2 / 4095.0) * 3.3; 
}

std::array<float, 2> SolarPanel::GetVoltage() {
  return {voltage1, voltage2};
}