#include "Hardware/Input/GPS.hpp"

GPS::GPS() {}

void GPS::Begin() {
  if (gps.begin()) {
    gps.setI2COutput(COM_TYPE_UBX);
    gps.setNavigationFrequency(4); // 4 Hz (250ms)

    gps.assumeAutoPVT(true);
    gps.setAutoPVT(true);

    Serial.println("GPS initialized successfully!");
  } else {
    Serial.println("BAD: GPS not found at address 0x42");
  }
}

void GPS::UpdateValues() {
  // REMOVED: millis() timer.
  // getPVT() is non-blocking with AutoPVT and must be called as fast as
  // possible to clear incoming I2C/Serial buffers and catch the 4Hz updates.
  if (gps.getPVT()) {
    fixType = gps.getFixType();
    sats = gps.getSIV();

    if (gps.getGnssFixOk()) {
      lat = gps.getLatitude();
      longit = gps.getLongitude();
      alt = gps.getAltitude();
      velocity = gps.getGroundSpeed() / 1000.0f; // mm/s to m/s
    }
  }
}

long GPS::GetLatitude() { return lat; }
long GPS::GetLongitude() { return longit; }
long GPS::GetAltitude() { return alt; }
uint8_t GPS::GetSats() { return sats; }
uint8_t GPS::GetFixType() { return fixType; }
float GPS::GetVelocity() { return velocity; }

std::array<long, 3> GPS::GetPosition() { return {longit, lat, alt}; }