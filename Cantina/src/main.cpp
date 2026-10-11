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
bool releaseStatus = false;
bool SendTelemetry = true;
bool solarMechStatus = false;
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
void Apogee() {

  release.Release();
  static double lastDescentTime = 0;
  if (millis() - lastDescentTime >= 1000) {
    currentFlight = FlightState::DESCENT;
  }
}
// diode release & ers
void Descent() {
  static double lastDescentTime = 0;
  lastDescentTime = millis();
  // -1 second because apogee is held a second
  if (millis() - lastDescentTime >= 4000) {
    solarM.Release();
  }
  currentFlight = (barometer.GetAltitude() < 50 && gps.GetVelocity() < 1)
                      ? FlightState::LANDING
                      : FlightState::DESCENT;
}

FlightState Landing() { /// return FlightState::LANDING;
}

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
    releaseStatus = true;
  }
  if (command == "CMD STOW") {
    releaseStatus = false;
  }
  if (command == "CMD DEPLOY") {
    solarMechStatus = true;
  }
  if (command == "CMD RETRACT") {
    solarMechStatus = false;
  }
  if (command == "CMD CX START") {
    SendTelemetry = true;
  }
  if (command == "CMD CX STOP") {
    SendTelemetry = false;
  }
}

void TelemetryUpdateLoop() {
  imu::Vector<3> accel = ins.GetAcceleration();
  imu::Vector<3> gyro = ins.GetEulerRotation();

  dispatcher.GetTelemetryData()->missionTime = millis();
  dispatcher.GetTelemetryData()->packetCount += 1;
  dispatcher.GetTelemetryData()->state = currentFlight;
  dispatcher.GetTelemetryData()->mechState = releaseStatus;
  dispatcher.GetTelemetryData()->solarState = SendTelemetry;
  dispatcher.GetTelemetryData()->solarPanel1 = 0.0;
  dispatcher.GetTelemetryData()->solarPanel2 = 0.0;
  dispatcher.GetTelemetryData()->accelX = accel.x();
  dispatcher.GetTelemetryData()->accelY = accel.y();
  dispatcher.GetTelemetryData()->accelZ = accel.z();
  dispatcher.GetTelemetryData()->gyroP = gyro.x();
  dispatcher.GetTelemetryData()->gyroR = gyro.y();
  dispatcher.GetTelemetryData()->gyroY = gyro.z();
  dispatcher.GetTelemetryData()->altitude = barometer.GetAltitude();
  dispatcher.GetTelemetryData()->temp = barometer.GetTemperature();
  dispatcher.GetTelemetryData()->batteryVoltage = 0.0;
  dispatcher.GetTelemetryData()->gpsLatitude = gps.GetLongitude();
  dispatcher.GetTelemetryData()->gpsLongitude = gps.GetLatitude();
  dispatcher.GetTelemetryData()->gpsSats = gps.GetSats();
}
const int BUTTON_PIN = 18;
bool lastButtonState = true;

void setup() {

  Serial.begin(115200);
  Wire.begin(26, 27);
  // hope these are the default pins :(
  // Serial1.setPins()
  Serial1.begin(115200);

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
  int chan = 4;
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
  // Initialize the sensors after delay for espNOW to prevent race conditions.
  ins.Begin();
  release.Begin();
  // solarM.Begin();
  gps.Begin();

  barometer.begin();
}

void loop() {
  bool currentButtonState = digitalRead(BUTTON_PIN);

  if (lastButtonState == true && currentButtonState == false) {
    if (release.GetPosition() >= 85 && release.GetPosition() <= 95)
      releaseStatus = true;
    else {
      releaseStatus = false;
    }
  }
  lastButtonState = currentButtonState;
  // Serial.println(release.GetPosition());
  //  analogWrite(25, HIGH);

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
    Serial.print(releaseStatus);

    barometer.PrintValues();

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
  if (!releaseStatus) {
     release.Stow();
  }
  if (releaseStatus ) {
     release.Release();
  }
  if (solarMechStatus) {}
  if (!solarMechStatus) {}
  
  // Serial.println(info);
  //  function to send out the data
  //  MAC to where you sending
  //  The data being sent
  //  Length of data
  if (SendTelemetry) {
    info = dispatcher.WriteTelemetry().c_str();
    static uint32_t lastTransmissionTime = 0;
    if (millis() - lastTransmissionTime >= 250) {
      esp_now_send(MAC, (uint8_t *)info.c_str(), info.length() + 1);
      lastTransmissionTime = millis();
    }
  }
}

// put function definitions here:
// int myFunction(int x, int y) { return x + y; }