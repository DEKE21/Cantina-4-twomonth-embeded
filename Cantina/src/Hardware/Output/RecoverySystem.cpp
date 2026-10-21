#include "Hardware/Input/RecoverySystem.hpp"

RecoverySystem::RecoverySystem(int buttonPin) {
  // this->LEDPin = LEDPin;
  this->buzzerPin = buttonPin;
}
void RecoverySystem::Begin() {
  // pinMode(this->LEDPin, OUTPUT);
  pinMode(this->buzzerPin, OUTPUT);
}
// half a second on half a second off
void RecoverySystem::LoopStep() {
  static double lastCycle = millis();
  if (millis() - lastCycle >= 500) {
    //  digitalWrite(LEDPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
  }
  if (millis() - lastCycle >= 1000) {
      //  digitalWrite(LEDPin, LOW);
    digitalWrite(buzzerPin, LOW);
    lastCycle = millis();
  }
}