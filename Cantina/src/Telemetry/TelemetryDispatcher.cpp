#include <Telemetry/TelemetryDispatcher.hpp>

TelemetryDispatcher::TelemetryDispatcher() {}
std::string TelemetryDispatcher::ParseDataStructure() {
  std::stringstream ss;

  ss << data.teamId << ',' << data.missionTime << ',' << data.packetCount << ','
     << data.state << ',' << data.mechState << ',' << data.altitude << ','
     << data.temp << ',' << data.batteryVoltage << ',' << data.gpsLatitude
     << ',' << data.gpsLongitude << ',' << data.gpsSats << ',' << data.gyroR
     << ',' << data.gyroP << ',' << data.gyroY << ',' << data.accelX << ','
     << data.accelY << ',' << data.accelZ << ',' << data.solarPanel1 << ','
     << data.solarPanel2 << '\n';
  return ss.str();
}
// serial2 or somthing
void TelemetryDispatcher::WriteTelemetry() {
  ParseDataStructure();
  // Serial2.write(ParseDataStructure().c_str());
}

void TelemetryDispatcher::UpdateTelemetry(Telemetry::TelemetryData newData) {
  newData = data;
}