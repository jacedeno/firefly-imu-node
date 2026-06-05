// Firefly Blue Ghost IMU — carrier firmware (SCAFFOLD / compile-check)
//
// Propósito de este archivo: validar que el toolchain Adafruit nRF52 + las
// librerías de sensores + Bluefruit BLE compilan en CI (x86_64). La lógica
// real de fusión y el paquete BLE de 20 bytes se portan luego desde el
// sensor_fusion.h existente — ver firefly-imu-carrier-design-brief.md §11.
//
//   U1: XIAO nRF52840 Sense Plus  (onboard LSM6DS3TR-C accel+gyro @ 0x6A)
//   U2: LIS3MDL magnetómetro externo @ 0x1C  (I2C user bus: D4=SDA, D5=SCL)

#include <Arduino.h>
#include <Wire.h>
#include <bluefruit.h>
#include <Adafruit_LIS3MDL.h>
#include <Adafruit_LSM6DS3TRC.h>

static Adafruit_LSM6DS3TRC imu;   // onboard accel + gyro  (U1)
static Adafruit_LIS3MDL    mag;   // external magnetometer (U2)

void setup() {
  Serial.begin(115200);

#ifdef PIN_LSM6DS3TR_C_POWER
  // El IMU onboard se alimenta por P1.08 en el Sense; ponerlo en alto.
  pinMode(PIN_LSM6DS3TR_C_POWER, OUTPUT);
  digitalWrite(PIN_LSM6DS3TR_C_POWER, HIGH);
  delay(50);
#endif

  Wire.begin();
  imu.begin_I2C(0x6A);   // LSM6DS3TR-C
  mag.begin_I2C(0x1C);   // LIS3MDL (CS->VDD_IO, SA1->GND)

  // BLE: conservar UUIDs/paquete existentes al portar la lógica real (§11).
  Bluefruit.begin();
  Bluefruit.setName("Firefly-IMU");
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.start(0);
}

void loop() {
  sensors_event_t accel, gyro, temp, magfield;
  imu.getEvent(&accel, &gyro, &temp);
  mag.getEvent(&magfield);

  // TODO: fusión Madgwick 9-DOF -> cuaternión -> BLE GATT notify (20 bytes).
  delay(20);
}
