#ifndef SystemConstants_H
#define SystemConstants_H
#include <iostream>

namespace Telemetry {

struct __attribute__((packed)) TelemetryData {
  std::string teamId = "0004";
  unsigned long missionTime;
  int packetCount;
  char state[15];
  int mechState;
  float altitude;
  float temp;
  float batteryVoltage;
  double gpsLatitude;
  double gpsLongitude;
  int gpsSats;
  float gyroR;
  float gyroP;
  float gyroY;
  float accelX;
  float accelY;
  float accelZ;
  float solarPanel1;
  float solarPanel2;
};

} // namespace Telemetry
#endif