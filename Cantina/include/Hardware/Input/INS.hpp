#include "Adafruit_BNO055.h"
#include "Adafruit_Sensor.h"
// Inertial navigation system :nerd:
class INS {

  enum INSState { BOOT, CALIBRATION, FLIGHT_READY, SAFTEY_VALUES };

private:
  Adafruit_BNO055 IMU;
  float XYZ[3];
  unsigned long lastCycleTime = 0;
  uint8_t gyro, acc, mag, sys = 0;
  imu::Vector<3> acceleration;
  imu::Vector<3> gyroVector;
  imu::Vector<3> eulerRotation;
  imu::Quaternion quaternionRotation;
  uint16_t updateDelayMs = 250;
  bool calibrated = false;
  sensors_event_t event;
  INSState insState = INSState::BOOT;

public:
  INS();
  void Begin();
  // when the IMU boots the data can be incorrect, this wipes the first readings
  void CleanData();
  imu::Vector<3> GetGyro();
  imu::Vector<3> GetAcceleration();
  imu::Quaternion GetQuaternionRotation();
  imu::Vector<3> GetEulerRotation();
  float GetGravityVector();
  float GetLinearVelocity();
  int GetCalibration();
  bool IsCalibrated();
  void UpdateValues();
  void PrintValues();
  void Loop(unsigned long startTime);

  // states
  void HandleStates();
  void Boot() {};
  void Calibration();
  void FlightReady();
  void SafteyValues();
};
