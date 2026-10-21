#include <ESP32Servo.h>

class SolarMechanism {

  // enum ReleaseState { IDLE, RELEASE, STOWED };

private:
  Servo solarServo;
  float position;
  int pinout = 0;
  bool Safe = true;
  // ReleaseState state = ReleaseState::IDLE;

public:
  SolarMechanism(int pin);
  void Begin();
  void Deploy();
  void Stow();
  double GetPosition();
  void UpdateLoop();
};
