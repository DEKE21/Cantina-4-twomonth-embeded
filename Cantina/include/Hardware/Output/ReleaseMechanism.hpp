#include <ESP32Servo.h>

class ReleaseMechanism {
  enum ReleaseState { IDLE, RELEASE, STOWED };

private:
  Servo releaseServo;
  float position;
  int pinout = 0;
  bool Safe = true;
  ReleaseState state = ReleaseState::IDLE;

public:
  ReleaseMechanism(int pin);
  void Begin();
  void Release();
  void Stow();
  double GetPosition();
  void UpdateLoop();
};
