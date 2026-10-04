#include "Hardware/Output/ReleaseMechanism.hpp"

ReleaseMechanism::ReleaseMechanism(int pinout) {
  ReleaseMechanism::pinout = pinout;
}
void ReleaseMechanism::Begin() {
  releaseServo.attach(pinout);
  	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	releaseServo.setPeriodHertz(50);    // standard 50 hz servo
  UpdateLoop();
  if (position) {
    state = (position == 180) ? ReleaseState::RELEASE
            : (position == 0) ? ReleaseState::STOWED
                              : ReleaseState::IDLE;
  }
  Stow();
}   
double ReleaseMechanism::GetPosition() { return releaseServo.read(); }

void ReleaseMechanism::UpdateLoop() { position = releaseServo.read(); }
void ReleaseMechanism::Stow() {
    releaseServo.write(90);
  
}
void ReleaseMechanism::Release() {
    releaseServo.write(180);

  
}
