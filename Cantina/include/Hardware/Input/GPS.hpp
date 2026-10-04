#pragma once
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include <Wire.h>
#include <array>
#include <type_traits>
#include <Arduino.h>

class GPS {
private:
  SFE_UBLOX_GNSS gps;
  uint8_t sats = 0;
  long lat = 0; // Changed to int32_t to safely support negative coordinates
  long longit = 0; // Changed to int32_t
  long alt = 0;    // Changed to int32_t
  uint8_t fixType = 0;

public:
  GPS();
  void Begin();
  void UpdateValues();

  std::array<long, 3> GetPosition();
  long GetLat();
  long GetLongit();
  long GetAlt();
  uint8_t GetSats();
  uint8_t GetFixType(); // Added so you can track your fix status directly
  float GetVelocity();
};
