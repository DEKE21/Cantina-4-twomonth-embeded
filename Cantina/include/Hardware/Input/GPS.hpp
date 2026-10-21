#pragma once
#include <Arduino.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include <Wire.h>
#include <array>
#include <type_traits>

class GPS {
private:
  SFE_UBLOX_GNSS gps;
  uint8_t sats = 0;
  int32_t latitude = 0;
  int32_t longitude = 0;
  long altitude = 0;
  int32_t velocity = 0.0;
  uint8_t fixType = 0;

public:
  GPS();
  void Begin();
  void UpdateValues();
  void PrintValues();
  std::array<long, 3> GetPosition();
  int32_t GetLatitude();
  int32_t GetLongitude();
  long GetAltitude();
  int32_t GetVelocity();

  uint8_t GetFixType();
  uint8_t GetSats();
};
