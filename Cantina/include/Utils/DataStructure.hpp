#ifndef SystemConstants_H
#define SystemConstants_H
#include <iostream>

namespace Telemetry {

struct __attribute__((packed)) TelemetryData {
  std::string teamId = "0004";
  unsigned long missionTime;
  int packetCount;
  int state;
  int mechState;
  float altitude;
  float temp;
  float batteryVoltage;
  float gpsLatitude;
  float gpsLongitude;
  int gpsSats;
  float gyroR;
  float gyroP;
  float gyroY;
  float accelX;
  float accelY;
  float accelZ;
  int solarState;
  float solarPanel1;
  float solarPanel2;
};

} // namespace Telemetry
#endif