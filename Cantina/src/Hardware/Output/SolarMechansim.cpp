#include "Hardware/Output/SolarMechanism.hpp"

SolarMechanism::SolarMechanism(int pinout) { SolarMechanism::pinout = pinout; }
void SolarMechanism::Begin() {
  solarServo.attach(pinout);

  solarServo.setPeriodHertz(50); // standard 50 hz servo
  UpdateLoop();
  if (position) {
    // state = (position == 180) ? SolarMechanism::RELEASE
    //        : (position == 0) ? SolarMechanism::STOWED
    //             : SolarMechanism::IDLE;
  } //
  // Stow();
}
double SolarMechanism::GetPosition() { return solarServo.read(); }

void SolarMechanism::UpdateLoop() { position = solarServo.read(); }
void SolarMechanism::Stow() { solarServo.write(100); }
void SolarMechanism::Deploy() { solarServo.write(175); }
