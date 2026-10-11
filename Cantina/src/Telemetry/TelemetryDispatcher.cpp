#include <Telemetry/TelemetryDispatcher.hpp>
#include <stdio.h> // For snprintf

TelemetryDispatcher::TelemetryDispatcher() {}

std::string TelemetryDispatcher::ParseDataStructure() {
  char buffer[256];
  
  // snprintf safely formats the string directly into the buffer without dynamic memory
  snprintf(buffer, sizeof(buffer), 
      "%s,%lu,%d,%s,%d,%.2f,%.2f,%.2f,%.6f,%.6f,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
      data.teamId.c_str(), 
      data.missionTime,
      data.packetCount,
      data.state,  
      data.mechState,
      data.altitude,
      data.temp,
      data.batteryVoltage,
      data.gpsLatitude,
      data.gpsLongitude,
      data.gpsSats,
      data.gyroR, data.gyroP, data.gyroY,
      data.accelX, data.accelY, data.accelZ,
      data.solarPanel1,
      data.solarPanel2
  );

  return std::string(buffer);
}

std::string TelemetryDispatcher::WriteTelemetry() {
  // Call it ONLY once! 
  // Note: Serial.print is preferred over Serial.write for ASCII text
 // Serial2.print(ParseDataStructure().c_str()); 
  return ParseDataStructure();
}

Telemetry::TelemetryData* TelemetryDispatcher::GetTelemetryData() {
  return &data;
}

// Fixed: Passed by const reference to prevent copying the entire struct
void TelemetryDispatcher::UpdateTelemetry(const Telemetry::TelemetryData& newData) {
   data = newData;
}