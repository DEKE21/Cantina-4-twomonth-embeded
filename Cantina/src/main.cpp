#include "Hardware/Input/Barometer.hpp"
// #include "Telemetry/CSVFileWriter.hpp"
#include "Hardware/Input/GPS.hpp"
#include "Hardware/Input/INS.hpp"
#include "Hardware/Output/ReleaseMechanism.hpp"
#include "Hardware/Output/SolarMechansim.hpp"
#include "Telemetry/TelemetryDispatcher.hpp"
#include "Utils/DataStructure.hpp"
#include <Arduino.h>
// Libraries
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

// Whatever data im sending
String info;
// Received command from Ground Station
String command;
// Some booleans used later on to change what my info string is sending
bool Mech_one = false;
bool Mech_two = false;
// THE MAC ADDRESS of Responder
uint8_t MAC[] = {0x68, 0x09, 0x47, 0x9D, 0xF3, 0x18};
Barometer barometer;
INS ins;
TelemetryDispatcher dispatcher;
ReleaseMechanism release(14);
SolarMechanism solarM(13);
GPS gps;
#define SENSOR 100;
#define TELEM 250;
enum FlightState { LAUNCH_PAD, ASCENT, APOGEE, DESCENT, LANDING };
FlightState currentFlight = FlightState::LAUNCH_PAD;
void LaunchPad() {
  // release.Release();
  if (barometer.GetAltitude() > 1) {
    currentFlight = FlightState::ASCENT;
  }
}
void Ascent() {

  if (barometer.GetAltitude() >= 2) {
    currentFlight = FlightState::APOGEE;
  }
}
// Release Mechanism
void Apogee() { release.Release(); }
// diode release & ers
FlightState Descent() {
  static double lastDescentTime = 0;
  lastDescentTime = millis();

  if (millis() - lastDescentTime >= 5000) {
    solarM.Release();
  }
  return (barometer.GetAltitude() < 100 && gps.GetVelocity() < 1)
             ? FlightState::LANDING
             : FlightState::DESCENT;
}

FlightState Landing() { return FlightState::LANDING; }

void HandleState(FlightState currentFlight) {
  switch (currentFlight) {
  case LAUNCH_PAD:
    LaunchPad();
    break;
  case ASCENT:
    Ascent();
    break;
  case APOGEE:
    Apogee();
    break;
  case DESCENT:
    Descent();
    break;
  case LANDING:
    Landing();
    break;
  default:
    break;
  }
}
// Changed first argument to 'const uint8_t *mac' to match your specific ESP32
// core version
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  // Safely null-terminate the incoming string data
  char buffer[len + 1];
  memcpy(buffer, incomingData, len);
  buffer[len] = '\0';

  command = String(buffer);
  Serial.println(command);

  // Check commands using evaluation operators (==)
  if (command == "CMD RELEASE") {
    if (Mech_one) {
      Mech_one = false;
    } else {
      Mech_one = true;
    }
  } else if (command == "CMD TX") {
    Mech_two = true;
  } else if (command == "CMD STOW") {
    Mech_one = false;
  } else if (command == "CMD CX") {
    Mech_two = false;
  }
}

// DataStructure myFunction(int, int);
const int BUTTON_PIN = 18;
bool lastButtonState = true;

void setup() {
  // put your setup code here, to run once:
  // int result = myFunction(2, 3);
  Serial.begin(115200);
  Wire.begin(26, 27);
  barometer.begin();
  ins.Begin();
  release.Begin();
  // solarM.Begin();
  gps.Begin();

  pinMode(BUTTON_PIN, INPUT);

  // Wifi stuff
  WiFi.mode(WIFI_STA);
  delay(500);
  Serial.print("WiFimode set");
  WiFi.disconnect();
  delay(800);
  Serial.print("WiFimode disconnect");
  // Long range made activated
  esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR);
  delay(400);
  Serial.println("LR activated");
  delay(200);
  Serial.print("Begin ESP Init");
  delay(2000);

  esp_now_peer_info_t peerInfo = {};
  // the int chan will be your team number.
  // Also with my error earlier, found something on basically a Redit site below
  // of how to fix the peer stuff.
  int chan = 2;
  ESP_ERROR_CHECK(esp_wifi_set_channel(chan, WIFI_SECOND_CHAN_NONE));
  if (esp_now_init() != ESP_OK) {
    ESP.restart();
    return;
  }
  peerInfo.channel = chan;
  memcpy(peerInfo.peer_addr, MAC, 6);
  if (esp_now_add_peer(&peerInfo) == ESP_OK) {
    Serial.printf("# Peer Added\r\n");
  } else {
    Serial.printf("# Unable to add peer \r\n");
  }

  delay(300);
  Serial.println("Start CB");
  delay(2000);
  // This will be the function playing in the background listening for any
  // commands coming in.
  esp_now_register_recv_cb(onDataRecv);
}

void loop() {
  bool currentButtonState = digitalRead(BUTTON_PIN);

  if (lastButtonState == true && currentButtonState == false) {
    if (release.GetPosition() >= 85 && release.GetPosition() <= 95)
      release.Release();
    else {
      release.Stow();
    }
  }
  lastButtonState = currentButtonState;
  // Serial.println(release.GetPosition());
  //  analogWrite(25, HIGH);

  // gps.UpdateValues();
  // Serial.println( ());
  // put your main code here, to run repeatedly:

  // solarM.Release();
  //  release.Stow();
  // 2  delay(30000);
  // solarM.Stow();
  //  release.Release();
  //  delay(30000);
  // gps.UpdateValues();

  // Serial.println(release.GetPosition());
  static uint32_t lastUpdateTime = 0;
  if (barometer.GetAltitude() >= 5) {
    Serial.println("Release Mechanism Activated");
  }
  HandleState(currentFlight);

  // Only poll the GPS once every 50 to 100 milliseconds
  if (millis() - lastUpdateTime > 100) {
    barometer.UpdateValues();
    ins.UpdateValues();
    gps.UpdateValues();
    lastUpdateTime = millis();
    Serial.print(currentFlight);
    Serial.print("\n \t Mech: ");
    Serial.print(Mech_one);
    /*
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
        ins.PrintValues();
    */
    barometer.PrintValues();
    Telemetry::TelemetryData telemetryData;
    telemetryData = *dispatcher.GetTelemetryData();
    telemetryData.missionTime = millis();
    telemetryData.packetCount += 1;
    telemetryData.state = FlightState::LAUNCH_PAD;
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
    // Serial.print("Telemetry Data: \n");
    // Serial.print(dispatcher.ParseDataStructure().c_str());
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
  if (!Mech_one) {
    info = "Container retracted";
    // release.Stow();
  }
  if (Mech_one && !Mech_two) {
    info = "Container released";
    // release.Release();
  }

  else if (Mech_two && !Mech_one) {
    info = "Mech_two released";
  } else {
    info = "Hello";
  }
  // Serial.println(info);
  //  function to send out the data
  //  MAC to where you sending
  //  The data being sent
  //  Length of data
  esp_now_send(MAC, (uint8_t *)info.c_str(), info.length() + 1);

  delay(500);
}

// put function definitions here:
// int myFunction(int x, int y) { return x + y; }