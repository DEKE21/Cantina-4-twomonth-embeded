#include "Utils/Datastructure.hpp"
#include "Telemetry/CSVFileWriter.hpp"

using namespace Telemetry;

CSVFileWriter::CSVFileWriter() {}
/// Convert to CSV type
void CSVFileWriter::WriteData(TelemetryData data) {

  file << data.teamId<< ','<< data.missionTime<< ','<< data.packetCount
      << ','<< data.state<< ','<< data.mechState<< ','<< data.altitude
      << ','<< data.temp<< ','<< data.batteryVoltage<< ','
      << data.gpsLatitude<< ','<< data.gpsLongitude<< ','<< data.gpsSats
      << ','<< data.gyroR<< ','<< data.gyroP<< ','<< data.gyroY<< ','
      << '\n';
}

void CSVFileWriter::StartWriting() {

  try {
    file.open("Telemetry", std::ios_base::out);

  } catch (...) {
    std::cerr << "Error opening file";
  }
  file << "TEAM_ID,MISSION_TIME,PACKET_COUNT,STATE,MECH_STATE,ALTITUDE,"
          "TEMP,BATTERY_VOLTAGE,GPS_LATITUDE,GPS_LONGITUDE,GPS_SATS,"
          "GYRO_R,GYRO_P,GYRO_Y,CHALLENGE_OPTION_DATA"
       << '\n';
}
