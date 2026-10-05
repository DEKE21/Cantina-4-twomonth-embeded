#include "Hardware/Input/Barometer.hpp"
// #include "Telemetry/CSVFileWriter.hpp"
#include "Hardware/Input/GPS.hpp"
#include "Hardware/Input/INS.hpp"
// #include "Hardware/Output/ReleaseMechanism.hpp"
// #include "Hardware/Output/SolarMechansim.hpp"
#include "Telemetry/TelemetryDispatcher.hpp"
 #include "Utils/DataStructure.hpp"
#include <Arduino.h>

Barometer barometer;
INS ins;
TelemetryDispatcher dispatcher;
// ReleaseMechanism release(14);
// SolarMechanism solarM(13);
GPS gps;
#define SENSOR 100;
#define TELEM 250;
enum FlightState { LAUNCH_PAD, ASCENT, APOGEE, DESCENT, LANDING };
FlightState LaunchPad() {
  (barometer.GetAltitude() > 10 &&  gps.GetVelocity() > 3) ? FlightState::ASCENT : FlightState::LAUNCH_PAD;
  
  return FlightState::ASCENT;
}
FlightState Ascent() { return FlightState::APOGEE; }
FlightState Apogee() { return FlightState::DESCENT; }
FlightState Descent() { return FlightState::LANDING; }
FlightState Landing() { return FlightState::LANDING; }
FlightState currentFlight = FlightState::LAUNCH_PAD;

void HandleState(FlightState currentFlight) {
  switch (currentFlight) {
  case LAUNCH_PAD:
    currentFlight = LaunchPad();
  case ASCENT:
    break;
  case APOGEE:
    break;
  case DESCENT:
    break;
  case LANDING:
    break;

  default:
    break;
  }
}
// DataStructure myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  // int result = myFunction(2, 3);
  Serial.begin(115200);
  Wire.begin(26, 27);
  barometer.begin();
  ins.Begin();
  // release.Begin();
  // solarM.Begin();
  gps.Begin();
}

void loop() {
 // analogWrite(25, HIGH);

  // gps.UpdateValues();
  // Serial.println( ());
  // put your main code here, to run repeatedly:

  // solarM.Release();
  // release.Release();
  // delay(1500);
  // solarM.Stow();
  // release.Stow();
  // delay(1500);
  // gps.UpdateValues();

  // Serial.println(release.GetPosition());
  static uint32_t lastUpdateTime = 0;

  // Only poll the GPS once every 50 to 100 milliseconds
  if (millis() - lastUpdateTime > 100) {
    barometer.UpdateValues();
    ins.UpdateValues();
    gps.UpdateValues();
    lastUpdateTime = millis();

    Serial.print("\nSATS: ");
    Serial.print(gps.GetSats());
    Serial.print("\nAltit: ");
    Serial.print(gps.GetAlt());
    Serial.print("\nLAT: ");
    Serial.print(gps.GetLat() / 10000000.0, 7);
    Serial.print("\nLong: ");
    Serial.print(gps.GetLongit() / 10000000.0, 7);
    Serial.print("\n");
        Serial.print("gorund sppeed: ");

      Serial.print(gps.GetVelocity(), 7);
    Serial.print("\n");
  //  ins.PrintValues();

   // barometer.PrintValues();
   Telemetry::TelemetryData telemetryData;
   telemetryData = *dispatcher.GetTelemetryData();
   telemetryData.missionTime = millis();
   telemetryData.packetCount +=1;
   telemetryData.state =  FlightState::LAUNCH_PAD;
   telemetryData.mechState = 0;
   telemetryData.solarState = 0;
   telemetryData.solarPanel1 = 0.0;
   telemetryData.solarPanel2 = 0.0;
   imu::Vector<3> accel = ins.GetAcceleration();
   telemetryData.accelX = accel.x();
   telemetryData.accelY = accel.y();
   telemetryData.accelZ = accel.z();
   imu::Vector<3> gyro = ins.GetEulerRotation();
   telemetryData.gyroP = gyro.x();
   telemetryData.gyroR = gyro.y();
   telemetryData.gyroY = gyro.z();
   telemetryData.altitude = barometer.GetAltitude();
   telemetryData.temp = barometer.GetTemperature();
   telemetryData.batteryVoltage = 0.0;
   dispatcher.UpdateTelemetry(telemetryData);
  Serial.print("Telemetry Data: \n");
  Serial.print(dispatcher.ParseDataStructure().c_str());
  /*
  Serial.print("\nMission Time: ");
  Serial.print(telemetryData.missionTime);
  Serial.print("\n Packet Count: ");
  Serial.print(telemetryData.packetCount);
 Serial.print("\n gyroP: ");
  Serial.print(telemetryData.gyroP);
  Serial.print("\n");

  Serial.print("\n Mech State: ");
  Serial.print(telemetryData.mechState);
  Serial.print("\n Solar State: ");
  Serial.print(telemetryData.solarState);
  Serial.print("\n Solar Panel 1: ");
  Serial.print(telemetryData.solarPanel1);
  Serial.print(" Solar Panel 2: ");
  Serial.print(telemetryData.solarPanel2);
   */
  }

  // Serial.println("meow");
  // Serial.println(gps.GetAlt());

  /*

   Telemetry::CSVFileWriter csv = Telemetry::CSVFileWriter();
   Telemetry::TelemetryData data = Telemetry::TelemetryData();
   data.altitude = 100;
   data.batteryVoltage = 0.01;
   data.challengeOptionData = "ChallegeOpData";
   data.gpsLatitude = 1;
   data.gpsLongitude = 0;
   data.gpsSats = 1;
   data.gyroP = 1;
   data.gyroR = 2;
   data.gyroY = 4;
   data.mechState = "MechState";
   data.missionTime = "0:00:00";
   data.packetCount = 1;
   data.state = "State";
   // data.teamId = "name";
   data.temp = 11;
   csv.StartWriting();
   csv.WriteData(data);
   csv.WriteData(data);
   csv.StopWriting();
   */
}

// put function definitions here:
// int myFunction(int x, int y) { return x + y; }