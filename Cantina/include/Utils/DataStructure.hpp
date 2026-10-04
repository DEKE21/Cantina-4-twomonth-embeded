#ifndef SystemConstants_H
#define SystemConstants_H
#include <iostream>

namespace Telemetry {

/*
TEAM_ID, MISSION_TIME, PACKET_COUNT, STATE, MECH_STATE,
ALTITUDE, TEMP, BATTERY_VOLTAGE, GPS_LATITUDE,
GPS_LONGITUDE, GPS_SATS, GYRO_R, GYRO_P, GYRO_Y,
CHALLENGE_OPTION_DATA
*/

struct TelemetryData {
  std::string teamId = "CANTINA-#4";
  std::string missionTime; // 00:00:00
  int packetCount;         // 0000000
  std::string state;       // 0000000
  std::string mechState;   // 0000000
  float altitude;         // 000.00
  float temp;             // 000.00
  float batteryVoltage;   // 0.00
  float gpsLatitude;      // 0.0000
  float gpsLongitude;     // 0.0000
  int gpsSats;             // 00
  float gyroR;            // 000.00
  float gyroP;            // 000.00
  float gyroY;            // 000.00
  float accelX;
  float accelY;
  float accelZ;

  std::string solarState; // 00000000
  float solarPanel1;      // 00000000
  float solarPanel2;      // 00000000
                          // 0.00
                          // 0.00
};
} // namespace Telemetry
#endif