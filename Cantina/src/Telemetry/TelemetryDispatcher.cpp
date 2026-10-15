#include <Telemetry/TelemetryDispatcher.hpp>
#include <cstring>
#include <stdio.h>


TelemetryDispatcher::TelemetryDispatcher() {}

std::string TelemetryDispatcher::ParseDataStructure() {
  char buffer[256];

  // Changed the 4th specifier (for data.state) from %s to %d
  // Also includes %d for solarState right before solarPanel1
  snprintf(buffer, sizeof(buffer),
           "%s,%lu,%d,%d,%d,%.2f,%.2f,%.2f,%.6f,%.6f,%d,%.2f,%.2f,%.2f,%.2f,%."
           "2f,%.2f,%d,%.2f,%.2f\n",
           data.teamId.c_str(), data.missionTime, data.packetCount,
           data.state, // Now matches %d!
           data.mechState, data.altitude, data.temp, data.batteryVoltage,
           (double)data.gpsLatitude / 10000000.0,
           (double)data.gpsLongitude / 10000000.0, data.gpsSats, data.gyroR,
           data.gyroP, data.gyroY, data.accelX, data.accelY, data.accelZ,
           data.solarState, // Matches %d
           data.solarPanel1, data.solarPanel2);

  return std::string(buffer);
}

std::string TelemetryDispatcher::WriteTelemetry() {
  return ParseDataStructure();
}

Telemetry::TelemetryData *TelemetryDispatcher::GetTelemetryData() {
  return &data;
}

void TelemetryDispatcher::UpdateTelemetry(
    const Telemetry::TelemetryData &newData) {
  data = newData;
}