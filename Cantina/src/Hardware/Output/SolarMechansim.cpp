#include "Hardware/Output/SolarMechansim.hpp"

SolarMechanism::SolarMechanism(int pinout) { SolarMechanism::pinout = pinout; }
void SolarMechanism::Begin() {
  solarServo.attach(pinout);
  UpdateLoop();
 
}
double SolarMechanism::GetPosition() { solarServo.read(); }

void SolarMechanism::UpdateLoop() { position = solarServo.read(); }
void SolarMechanism::Stow() {
  if (Safe) {
    solarServo.write(0);
  }
}
void SolarMechanism::Release() {
  if (Safe) {
    solarServo.write(90);
  }
  //determine possible ranges for servo positions 
//  if(solarServo.read() <= 90 && solarServo.read() >=85){}
}
