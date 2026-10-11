#include <Arduino.h>

class SolarPanel {
public:
  SolarPanel(int pin1, int pin2);
  void Begin();
  void UpdateValues();
  std::array<float, 2> GetVoltage();
  
private:
  int pin1;
  float voltage1;
  int pin2;
  float voltage2;
};