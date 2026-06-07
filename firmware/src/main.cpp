// Firefly Blue Ghost IMU — carrier firmware (SCAFFOLD / compile-check)
//
// Purpose of this file: verify that the Adafruit nRF52 toolchain + the sensor
// libraries + Bluefruit BLE compile in CI (x86_64). The real fusion logic and
// the 20-byte BLE packet are ported later from the existing sensor_fusion.h —
// see firefly-imu-carrier-design-brief.md §11.
//
//   U1: XIAO nRF52840 Sense Plus  (onboard LSM6DS3TR-C accel+gyro @ 0x6A)
//   U2: external LIS3MDL magnetometer @ 0x1C  (user I2C bus: D4=SDA, D5=SCL)

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
  // The onboard IMU is powered via P1.08 on the Sense; drive it high.
  pinMode(PIN_LSM6DS3TR_C_POWER, OUTPUT);
  digitalWrite(PIN_LSM6DS3TR_C_POWER, HIGH);
  delay(50);
#endif

  Wire.begin();
  imu.begin_I2C(0x6A);   // LSM6DS3TR-C
  mag.begin_I2C(0x1C);   // LIS3MDL (CS->VDD_IO, SA1->GND)

  // BLE: keep the existing UUIDs/packet when porting the real logic (§11).
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

  // TODO: 9-DOF Madgwick fusion -> quaternion -> BLE GATT notify (20 bytes).
  delay(20);
}
