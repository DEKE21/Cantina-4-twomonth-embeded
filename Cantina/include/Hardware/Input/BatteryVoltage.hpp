#include <Arduino.h>
class BatteryVoltage {
private:
  int pin;
  float voltage;

public:
  BatteryVoltage(int pin);
  void Begin();
  void UpdateValues();
  float GetVoltage();
};