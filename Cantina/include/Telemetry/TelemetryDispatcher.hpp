#include <Arduino.h>
#include <Utils/DataStructure.hpp>
#include <sstream>
#include <string>


class TelemetryDispatcher {
private:
  Telemetry::TelemetryData data;

public:
  TelemetryDispatcher();
  std::string ParseDataStructure();
  std::string WriteTelemetry();
  Telemetry::TelemetryData *GetTelemetryData();
  void UpdateTelemetry(const Telemetry::TelemetryData &newData);
};