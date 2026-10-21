#include <Arduino.h>
class BatteryVoltage {
private:
  int pin;
  float voltage;
  const float R1 = 10000.0f;
  const float R2 = 10000.0f;

public:
  BatteryVoltage(int pin);
  void Begin();
  void UpdateValues();
  float GetVoltage();
};