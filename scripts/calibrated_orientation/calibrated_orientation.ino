// Full orientation sensing using NXP/Madgwick/Mahony and a range of 9-DoF
// sensor sets.
// You *must* perform a magnetic calibration before this code will work.
//
// To view this data, use the Arduino Serial Monitor to watch the
// scrolling angles, or run the OrientationVisualiser example in Processing.
// Based on  https://github.com/PaulStoffregen/NXPMotionSense with adjustments
// to Adafruit Unified Sensor interface

#include <Adafruit_Sensor_Calibration.h>
#include <Adafruit_AHRS.h>

Adafruit_Sensor *accelerometer, *gyroscope, *magnetometer;

// uncomment one combo 9-DoF!
//#include "LSM6DS_LIS3MDL.h"  // can adjust to LSM6DS33, LSM6DS3U, LSM6DSOX...
//#include "LSM9DS.h"           // LSM9DS1 or LSM9DS0
#include "NXP_FXOS_FXAS.h"  // NXP 9-DoF breakout

// pick your filter! slower == better quality output
//Adafruit_NXPSensorFusion filter; // slowest
Adafruit_Madgwick filter;  // faster than NXP
//Adafruit_Mahony filter;  // fastest/smalleset

#if defined(ADAFRUIT_SENSOR_CALIBRATION_USE_EEPROM)
  Adafruit_Sensor_Calibration_EEPROM cal;
#else
  Adafruit_Sensor_Calibration_SDFat cal;
#endif

#define FILTER_UPDATE_RATE_HZ 50
#define PRINT_EVERY_N_UPDATES 5

uint32_t timestamp;

void setup() {
  Serial.begin(115200);
  while (!Serial) yield();

  if (!cal.begin()) {
    Serial.println("Failed to initialize calibration helper");
  } else if (! cal.loadCalibration()) {
    Serial.println("No calibration loaded/found");
  }

  if (!init_sensors()) {
    Serial.println("Failed to find sensors");
    while (1) delay(10);
  }

  setup_sensors();
  filter.begin(FILTER_UPDATE_RATE_HZ);
  timestamp = millis();

  Wire.setClock(400000); // 400KHz
}


void loop() {
  float roll, pitch, heading;
  float gx, gy, gz;
  static uint8_t counter = 0;

  if ((millis() - timestamp) < (1000 / FILTER_UPDATE_RATE_HZ)) {
    return;
  }
  timestamp = millis();
  // Read the motion sensors
  sensors_event_t accel, gyro, mag;
  accelerometer->getEvent(&accel);
  gyroscope->getEvent(&gyro);
  magnetometer->getEvent(&mag);

  cal.calibrate(mag);
  cal.calibrate(accel);
  cal.calibrate(gyro);
  // Gyroscope needs to be converted from Rad/s to Degree/s
  // the rest are not unit-important
  gx = gyro.gyro.x * SENSORS_RADS_TO_DPS;
  gy = gyro.gyro.y * SENSORS_RADS_TO_DPS;
  gz = gyro.gyro.z * SENSORS_RADS_TO_DPS;

  // Update the SensorFusion filter
  filter.update(gx, gy, gz, 
                accel.acceleration.x, accel.acceleration.y, accel.acceleration.z, 
                mag.magnetic.x, mag.magnetic.y, mag.magnetic.z);

  // only print the calculated output once in a while
  if (counter++ <= PRINT_EVERY_N_UPDATES) {
    return;
  }
  // reset the counter
  counter = 0;

  roll = filter.getRoll();
  pitch = filter.getPitch();
  heading = filter.getYaw();

  //-----Uncomment below to plot X and Y acceleration-----//
//  Serial.print(accel.acceleration.x, 4); Serial.print("\t");
//  Serial.println(accel.acceleration.y, 4);

  //-----Uncomment below to plot Z acceleration-----//
//  Serial.println(accel.acceleration.z, 4);

  //-----Uncomment below to plot gyroscope data in deg/s-----//
//  Serial.print(gx, 4); Serial.print("\t");
//  Serial.print(gy, 4); Serial.print("\t");
//  Serial.println(gz, 4);

  //-----Uncomment below to plot gyroscope data in rad/s (x100 to make viewable on plot)-----//
//  Serial.print(gyro.gyro.x*100, 4); Serial.print("\t");
//  Serial.print(gyro.gyro.y*100, 4); Serial.print("\t");
//  Serial.println(gyro.gyro.z*100, 4);

  //-----Uncomment below to plot magnetometer data-----//
//  Serial.print(mag.magnetic.x, 4); Serial.print("\t");
//  Serial.print(mag.magnetic.y, 4); Serial.print("\t");
//  Serial.println(mag.magnetic.z, 4);

  //-----Uncomment below to plot Euler angle data-----//
  Serial.print(heading); Serial.print("\t");
  Serial.print(pitch); Serial.print("\t");
  Serial.println(roll);

}
