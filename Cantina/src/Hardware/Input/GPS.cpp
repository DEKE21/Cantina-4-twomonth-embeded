#include "Hardware/Input/GPS.hpp" 

GPS::GPS() {}

void GPS::Begin() {
  if (gps.begin()) {
    gps.setI2COutput(COM_TYPE_UBX);
    gps.setNavigationFrequency(10);
    gps.assumeAutoPVT(true);
    gps.setAutoPVT(true);

    Serial.println("GPS initialized successfully!");
   } else {
    Serial.println("BAD: GPS not found at address 0x42");
  }
}

void GPS::UpdateValues() {
  static uint32_t lastPoll = 0;
  if (millis() - lastPoll > 100) {
    lastPoll = millis();
    if (gps.getPVT()) {
      fixType = gps.getFixType();
      sats = gps.getSIV();

      fixType = gps.getFixType();

      if (gps.getGnssFixOk()) {
        lat = gps.getLatitude();
        longit = gps.getLongitude();
        alt = gps.getAltitude();
      }
    }
  }
}

long GPS::GetLat() { return lat; }
long GPS::GetLongit() { return longit; }
long GPS::GetAlt() { return alt; }
uint8_t GPS::GetSats() { return sats; }
uint8_t GPS::GetFixType() { return fixType; }
float GPS::GetVelocity() { return gps.getGroundSpeed()/1000; }

std::array<long, 3> GPS::GetPosition() { return {longit, lat, alt}; }
