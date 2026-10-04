#include <ESP32Servo.h>

class SolarMechanism {

  enum SolarMechanismState { IDLE, DEPLOY, STOWED };

private:
  Servo solarServo;
  double position;
  int pinout = 0;
  bool Safe = true;
  SolarMechanismState state = SolarMechanismState::IDLE;

public:
  SolarMechanism(int pin);
  void Begin();
  void Release();
  void Stow();
  double GetPosition();
  SolarMechanismState decideState();
  void UpdateLoop();
};
