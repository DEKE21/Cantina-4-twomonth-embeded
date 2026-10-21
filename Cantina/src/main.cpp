#include "Hardware/Input/Barometer.hpp"
// #include "Telemetry/CSVFileWriter.hpp"
#include "Hardware/Input/BatteryVoltage.hpp"
#include "Hardware/Input/GPS.hpp"
#include "Hardware/Input/INS.hpp"
#include "Hardware/Input/RecoverySystem.hpp"
#include "Hardware/Input/SolarPanel.hpp"
#include "Hardware/Output/ReleaseMechanism.hpp"
#include "Hardware/Output/SolarMechanism.hpp"
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
bool newReleaseCMD = false;
bool newSolarCMD = false;
// THE MAC ADDRESS of Responder
uint8_t MAC[] = {0x68, 0x09, 0x47, 0x9D, 0xF3, 0x18};
Barometer barometer;
INS ins;
TelemetryDispatcher dispatcher;
ReleaseMechanism release(14);  // 14
SolarMechanism solarM(13);     // 13
SolarPanel solarPanel(36, 32); // 36 32
RecoverySystem recovery(25);   // 18
BatteryVoltage battery(34);    // 34
GPS gps;
std::string flightStateStr = "BOOT";
#define SENSOR 100
#define TELEM 250
enum FlightState { LAUNCH_PAD, ASCENT, APOGEE, DESCENT, LANDING };
FlightState currentFlight = FlightState::LAUNCH_PAD;
void LaunchPad() {
  // release.Release();
  if (barometer.GetAltitude() > 10) {
    currentFlight = FlightState::ASCENT;
  }
}
void Ascent() {
  if (barometer.GetAltitude() >= 500) {
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
  static double lastDescentTime = millis();
  // -1 second because apogee is held a second
  if (millis() - lastDescentTime >= 4000) {
    solarM.Deploy();
  }
  if (barometer.GetAltitude() < 50 && gps.GetVelocity() < 1) {
    recovery.LoopStep();
    solarM.Deploy();

    currentFlight = FlightState::LANDING;
  }
}

void Landing() { recovery.LoopStep(); }

void HandleState(FlightState currentFlight) {
  switch (currentFlight) {
  case LAUNCH_PAD:
    LaunchPad();
    flightStateStr = "LAUNCH_PAD";
    break;
  case ASCENT:
    Ascent();
    flightStateStr = "ASCENT";
    break;
  case APOGEE:
    Apogee();
    flightStateStr = "APOGEE";
    break;
  case DESCENT:
    Descent();
    flightStateStr = "DESCENT";
    break;
  case LANDING:
    Landing();
    flightStateStr = "LANDING";
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
  command.trim();
  Serial.println(command);

  if (command == "CMD RELEASE") {
    releaseStatus = true;
    newReleaseCMD = true;
  } else if (command == "CMD STOW") {
    releaseStatus = false;
    newReleaseCMD = true;

  } else if (command == "CMD DEPLOY") {
    solarMechStatus = true;
    newSolarCMD = true;

  } else if (command == "CMD RETRACT") {
    solarMechStatus = false;
    newSolarCMD = true;

  } else if (command == "CMD CX START") {
    SendTelemetry = true;

  } else if (command == "CMD CX STOP") {
    SendTelemetry = false;
  }
  if (command == "AT") {
  }
}

void TelemetryUpdateLoop() {
  imu::Vector<3> accel = ins.GetAcceleration();
  imu::Vector<3> gyro = ins.GetEulerRotation();

  dispatcher.GetTelemetryData()->missionTime = millis();
  // dispatcher.GetTelemetryData()->state = flightStateStr;
  strncpy(dispatcher.GetTelemetryData()->state, flightStateStr.c_str(),
          sizeof(dispatcher.GetTelemetryData()->state) - 1);
  // dispatcher.GetTelemetryData()
  //    ->state[sizeof(dispatcher.GetTelemetryData()->state) - 1] = '\0';
  // this is really dumb, but see it converting bool to int to satisfy type will
  // fix telem status
  dispatcher.GetTelemetryData()->mechState = solarMechStatus ? 1 : 0;
  dispatcher.GetTelemetryData()->solarPanel1 = solarPanel.GetVoltage()[0];
  dispatcher.GetTelemetryData()->solarPanel2 = solarPanel.GetVoltage()[1];
  dispatcher.GetTelemetryData()->accelX = accel.x();
  dispatcher.GetTelemetryData()->accelY = accel.y();
  dispatcher.GetTelemetryData()->accelZ = accel.z();
  dispatcher.GetTelemetryData()->gyroP = gyro.x();
  dispatcher.GetTelemetryData()->gyroR = gyro.y();
  dispatcher.GetTelemetryData()->gyroY = gyro.z();
  dispatcher.GetTelemetryData()->altitude = barometer.GetAltitude();
  dispatcher.GetTelemetryData()->temp = barometer.GetTemperature();
  dispatcher.GetTelemetryData()->batteryVoltage = battery.GetVoltage();
  dispatcher.GetTelemetryData()->gpsLatitude = gps.GetLongitude();
  dispatcher.GetTelemetryData()->gpsLongitude = gps.GetLatitude();
  dispatcher.GetTelemetryData()->gpsSats = gps.GetSats();
}

void setup() {
  // Wire & Serial init
  Serial.begin(115200);
  Wire.begin(26, 27);
  Wire.setClock(400000);
  Serial2.begin(9600, SERIAL_8N1, 2, 4);
  solarPanel.Begin();
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
  // the int chan  is team number.
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

  esp_now_register_recv_cb(onDataRecv);

  // Initialize the sensors after delay for espNOW to prevent race conditions.
  ins.Begin();
  release.Begin();
  solarM.Begin();
  gps.Begin();
  barometer.begin();
  battery.Begin();
  recovery.Begin();
}

void loop() {
  gps.UpdateValues();
  static uint32_t lastUpdateTime = 0;
  HandleState(currentFlight);
  // Only poll the sensors once every 100 milliseconds
  if (millis() - lastUpdateTime >= 100) {
    solarPanel.UpdateValues();
    barometer.UpdateValues();
    battery.UpdateValues();
    ins.UpdateValues();
    lastUpdateTime = millis();
    // barometer.PrintValues();
    // ins.PrintValues();
    // gps.PrintValues();
    // Serial.print(dispatcher.ParseDataStructure().c_str());
    TelemetryUpdateLoop();
  }
  if (newReleaseCMD) {

    if (releaseStatus) {
      release.Release();
    } else {
      release.Stow();
    }
    newReleaseCMD = false;
  }
  if (newSolarCMD) {

    if (solarMechStatus) {
      solarM.Deploy();
    } else {
      solarM.Stow();
    }
    newSolarCMD = false;
  }

  if (Serial2.available()) {
    static uint32_t lastTransmissionTime = 0;
    if (millis() - lastTransmissionTime >= 250) {
      info = dispatcher.WriteTelemetry().c_str();

      Serial2.write(info.c_str());
      lastTransmissionTime = millis();
      info = "";
    }
  }
  static uint32_t lastTransmissionTime = 0;
  if (millis() - lastTransmissionTime >= 250) {
    dispatcher.GetTelemetryData()->packetCount += 1;
    info = (SendTelemetry) ? dispatcher.WriteTelemetry().c_str()
                           : "CX STOPPED Heartbeat";
    esp_now_send(MAC, (uint8_t *)info.c_str(), info.length() + 1);
    lastTransmissionTime = millis();
  }
  info = "";
}
