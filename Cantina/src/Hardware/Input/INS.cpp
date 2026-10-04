#include "Hardware/Input/INS.hpp"

INS::INS() {
  // Warning: must figure out address being running
  IMU = Adafruit_BNO055(55);
}
void INS::Begin() {
  IMU.begin(adafruit_bno055_opmode_t::OPERATION_MODE_IMUPLUS);
}

// ignore magnetometer values
int INS::GetCalibration() { IMU.getCalibration(&sys, &gyro, &acc, &mag); }
bool INS::IsCalibrated() { return IMU.isFullyCalibrated(); }
imu::Vector<3> INS::GetGyro() { return gyroVector; }
imu::Vector<3> INS::GetAcceleration() { return acceleration; }
imu::Quaternion INS::GetQuaternionRotation() { return quaternionRotation; }
imu::Vector<3> INS::GetEulerRotation() { return eulerRotation; }
void INS::PrintValues() {
  static uint32_t lastPoll = 0;

  if (millis() - lastPoll > 500) {
    lastPoll = millis();
    Serial.print("Gyro: ");
    imu::Vector<3> gyro = GetGyro();
    Serial.print(gyro.x());
    Serial.print(", ");
    Serial.print(gyro.y());
    Serial.print(", ");
    Serial.println(gyro.z());

    Serial.print("Accel: ");
    imu::Vector<3> accel = GetAcceleration();
    Serial.print(accel.x());
    Serial.print(", ");
    Serial.print(accel.y());
    Serial.print(", ");
    Serial.println(accel.z());

    imu::Quaternion quat = GetQuaternionRotation();
    Serial.print("Quat: ");
    Serial.print(quat.w());
    Serial.print(", ");
    Serial.print(quat.x());
    Serial.print(", ");
    Serial.print(quat.y());
    Serial.print(", ");
    Serial.println(quat.z());

    imu::Vector<3> euler = GetEulerRotation();
    euler.toDegrees();
    Serial.print("Euler: ");
    Serial.print(euler.x());
    Serial.print(", ");
    Serial.print(euler.y());
    Serial.print(", ");
    Serial.println(euler.z());
  }
}
void INS::UpdateValues() {
  gyroVector = IMU.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
  quaternionRotation = IMU.getQuat();
  acceleration = IMU.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  eulerRotation = quaternionRotation.toEuler();
}
