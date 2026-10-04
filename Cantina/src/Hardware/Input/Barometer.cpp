

#include "Hardware/Input/Barometer.hpp"

Barometer::Barometer() {}
void Barometer::begin() {

  if (!bmp.begin_I2C()) {

    Serial.print("BAD BAD BAD");
  };
bmp.performReading(); 
localPresure= bmp.readPressure()/100;
   bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
   bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
    bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    bmp.setOutputDataRate(BMP3_ODR_50_HZ);
}

float Barometer::GetAltitude() { return bmp.readAltitude(localPresure); }
float Barometer::GetTemperature() { return bmp.temperature; }
float Barometer::GetPressure() { return bmp.pressure;}
void Barometer::PrintValues() {
  static uint32_t lastPoll = 0;
  if (millis() - lastPoll > 500) {
    lastPoll = millis();
    Serial.print("Pressure: ");
    Serial.print(bmp.pressure);
    Serial.print(" Pa, ");
    Serial.print("Temperature: ");
    Serial.print(bmp.temperature);
    Serial.print(" C, ");
    Serial.print("Altitude: ");
    Serial.print(bmp.readAltitude(localPresure));
    Serial.println(" m");
  }
}
void Barometer::UpdateValues() { bmp.performReading(); }
