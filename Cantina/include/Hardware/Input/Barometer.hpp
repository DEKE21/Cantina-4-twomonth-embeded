#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP3XX.h>
#include <Wire.h>
#include <Arduino.h>

class Barometer{
//bp390
    private:
        Adafruit_BMP3XX bmp;
        public:
        Barometer();
        double localPresure=1013.25;
    void begin();
    float GetAltitude();
    float GetTemperature();
    float GetPressure();
    void PrintValues()  ;
    void UpdateValues();

};