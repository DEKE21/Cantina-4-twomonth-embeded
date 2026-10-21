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
      latitude = gps.getLatitude() ;
      longitude = gps.getLongitude() ;
      altitude = gps.getAltitude() ;    // mm 
      velocity = gps.getGroundSpeed() ; // mm/s 
    }
  }
}
void GPS::PrintValues() {
  Serial.print("GPS: ");
  Serial.print("Fix Type: ");
  Serial.print(fixType);
  Serial.print(", Sats: ");
  Serial.print(sats);
  Serial.print(", Lat: ");
  Serial.print(latitude); // Convert to degrees
  Serial.print(", Long: ");
  Serial.print(longitude); // Convert to degrees
  Serial.print(", Alt: ");
  Serial.print(altitude); // Convert to meters
  Serial.print(" m, Vel: ");
  Serial.print(velocity, 2);
  Serial.println(" m/s");
}
int32_t GPS::GetLatitude() { return latitude; }
int32_t GPS::GetLongitude() { return longitude; }
long GPS::GetAltitude() { return altitude; }
uint8_t GPS::GetSats() { return sats; }
uint8_t GPS::GetFixType() { return fixType; }
int32_t GPS::GetVelocity() { return velocity; }

std::array<long, 3> GPS::GetPosition() {
  return {longitude, latitude, altitude};
}