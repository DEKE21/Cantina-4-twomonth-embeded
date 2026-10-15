#ifndef TelemetryDispatcher_HPP
#define TelemetryDispatcher_HPP

#include "Utils/DataStructure.hpp" // Adjust to match your struct header name
#include <cstring>
#include <iostream>
#include <string>


class TelemetryDispatcher {
public:
  TelemetryDispatcher();
  std::string ParseDataStructure();
  std::string WriteTelemetry(); // Must say std::string here too!
  Telemetry::TelemetryData *GetTelemetryData();
  void UpdateTelemetry(
      const Telemetry::TelemetryData &newData); // Must have const & here too!

private:
  Telemetry::TelemetryData data;
};

#endif