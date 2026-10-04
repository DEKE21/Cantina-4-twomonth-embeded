#include <Arduino.h>
#include <Utils/DataStructure.hpp>
#include <string>
#include <sstream>

class TelemetryDispatcher {
private:

  Telemetry::TelemetryData data;

public:
  TelemetryDispatcher();
  std::string ParseDataStructure();
  void UpdateTelemetry(Telemetry::TelemetryData newData);
  void WriteTelemetry();
};