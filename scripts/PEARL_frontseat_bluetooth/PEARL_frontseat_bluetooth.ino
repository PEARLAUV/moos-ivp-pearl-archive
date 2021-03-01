
/*--------------------*/
//If true writes thrust commands to specified LED pins
//If false writes thrust commands to specified motor controller pins
bool TEST_MODE = true;

//Pin assignments
int rightForwardLED = 8;
int leftForwardLED = 9;
int rightBackwardLED = 10;
int leftBackwardLED = 11;
int leftMotorPin = 12;
int rightMotorPin = 13;
/*--------------------*/

#include <Wire.h>
#include <Adafruit_Sensor_Calibration.h>
#include <Adafruit_AHRS.h>
#include <ArduinoBlue.h>

/*----------Setup IMU and sensor fusion----------*/
Adafruit_Sensor *accelerometer, *gyroscope, *magnetometer;

#include "NXP_FXOS_FXAS.h"  // NXP 9-DoF breakout

// pick your filter! slower == better quality output
//Adafruit_NXPSensorFusion filter; // slowest
Adafruit_Madgwick filter;  // faster than NXP
//Adafruit_Mahony filter;  // fastest/smalleset

/*----------Load IMU calibrations----------*/
#if defined(ADAFRUIT_SENSOR_CALIBRATION_USE_EEPROM)
  Adafruit_Sensor_Calibration_EEPROM cal;
#else
  Adafruit_Sensor_Calibration_SDFat cal;
#endif



#define FILTER_UPDATE_RATE_HZ 50
#define PRINT_EVERY_N_UPDATES 5

uint32_t timestamp;
const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];
char nmeaHeader[numChars] = {0};
float thrustLeft = 0.0;
float thrustRight = 0.0;
int curLeft = 188.0;
int curRight = 188.0;
boolean newData = false;
int manualControl = 0;   //0 = manual control off, 1 = manual control on

const int THROTTLE_ZERO_THRESHOLD = 5;
const int STILL = 188;
const int MIN_THROTTLE = 3;
const int LEFT_FORWARD_MAX = 254;
const int LEFT_BACKWARD_MAX = 120;
const int RIGHT_FORWARD_MAX = LEFT_FORWARD_MAX;
const int RIGHT_BACKWARD_MAX = LEFT_BACKWARD_MAX;
const int LEFT_FORWARD_MIN = STILL + MIN_THROTTLE;
const int LEFT_BACKWARD_MIN = STILL - MIN_THROTTLE;
const int RIGHT_FORWARD_MIN = LEFT_FORWARD_MIN;
const int RIGHT_BACKWARD_MIN = LEFT_BACKWARD_MIN;
const int TURN_SPEED = 10;
bool JOYSTICK = false;

int button;

ArduinoBlue phone(Serial1);

 
void setup(void) 
{
  /*------Setup for motor control--------*/
  Serial1.begin(9600);
  while (!Serial1) {
    delay(1);   
  }
  if (TEST_MODE) {
    pinMode(rightForwardLED, OUTPUT);
    pinMode(leftForwardLED, OUTPUT);
    pinMode(rightBackwardLED, OUTPUT);
    pinMode(leftBackwardLED, OUTPUT);
    digitalWrite(rightForwardLED, LOW);
    digitalWrite(leftForwardLED, LOW);
    digitalWrite(rightBackwardLED, LOW);
    digitalWrite(leftBackwardLED, LOW);
  }
  else {
    pinMode(leftMotorPin, OUTPUT);
    pinMode(rightMotorPin, OUTPUT);
  }
  
  /*------Setup for front seat comms--------*/
  Serial.begin(115200);
  while (!Serial) {
    delay(1);   
  }

  /*------Setup for IMU--------*/
  cal.begin();
  cal.loadCalibration();
  init_sensors();
  setup_sensors();
  filter.begin(FILTER_UPDATE_RATE_HZ);
  timestamp = millis();
  Wire.setClock(400000); // 400KHz

}

const String PREFIX    = "PL";   // Device prefix: PL = PEARL
const String ID_EULER  = "IMU";  // Sentence ID for Euler angles generated from sensor fusion filter
const String ID_RAW    = "RAW";  // Sentence ID for raw IMU readings
const String ID_MOTOR  = "MOT";  // Sentence ID for notifying MOOS-IvP of current motor commands
 
void loop(void) 
{
  static uint8_t counter = 0;
  /*------Read IMU data and package in NMEA sentence--------*/
  float roll, pitch, heading;
  float ax, ay, az;
  float gx, gy, gz;
  float mx, my, mz;

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

  ax = accel.acceleration.x;
  ay = accel.acceleration.y;
  az = accel.acceleration.z;
  
  // Gyroscope needs to be converted from Rad/s to Degree/s
  gx = gyro.gyro.x * SENSORS_RADS_TO_DPS;
  gy = gyro.gyro.y * SENSORS_RADS_TO_DPS;
  gz = gyro.gyro.z * SENSORS_RADS_TO_DPS;

  mx = mag.magnetic.x;
  my = mag.magnetic.y;
  mz = mag.magnetic.z;

  // Update the SensorFusion filter
  filter.update(gx, gy, gz, 
                ax, ay, az, 
                mx, my, mz);

  roll = filter.getRoll();
  pitch = filter.getPitch();
  heading = filter.getYaw();

  /*------Read commands from bluetooth--------*/
  button = phone.getButton();  //ID of button pressed

  if (button==1) {     //manual override ON
    manualControl = 1;
    phone.sendMessage("Manual Control ON");
    curLeft = STILL;
    curRight = STILL;
  }
  else if (button==2) {     //manual override OFF
    manualControl = 0;
    phone.sendMessage("Manual Control OFF");
  }

  if (manualControl==1) {
    if (button==3) {     //rotate right
      curLeft = LEFT_FORWARD_MIN + TURN_SPEED;
      curRight = RIGHT_BACKWARD_MIN - TURN_SPEED;
      if (!TEST_MODE) {
        analogWrite(leftMotorPin, curLeft);
        analogWrite(rightMotorPin, curRight); }
    }
    else if (button==4) {
      curLeft = LEFT_BACKWARD_MIN - TURN_SPEED;
      curRight = RIGHT_FORWARD_MIN + TURN_SPEED;
      if (!TEST_MODE) { 
        analogWrite(leftMotorPin, curLeft);
        analogWrite(rightMotorPin, curRight); }
    }
    else if (button==5) {
      JOYSTICK = true;
      phone.sendMessage("Joystick ON");
    }
    else if (button==6) {
      JOYSTICK = false;
      phone.sendMessage("Joystick OFF");
    }
  
    // throttle and steering values go from 0 to 99
    // throttle 0 -> maximum backwards throttle
    // throttle 49 -> no throttle (stay still)
    // throttle 99 -> maximum forwards throttle
    // steering 0 -> maximum left turn
    // steering 49 -> no turn (straight)
    // steering 99 -> maximum right turn
    if (JOYSTICK) {
      int throttle = phone.getThrottle();
      int steering = phone.getSteering();
      handleDriving(throttle, steering);
    }

    if (button==7) {
      curLeft = STILL;
      curRight = STILL;
      if (!TEST_MODE) {
        analogWrite(leftMotorPin, curLeft);
        analogWrite(rightMotorPin, curRight); }
    }
  }


  /*-------Read NMEA sentence from serial port, convert to PWM, and send to motor controllers-------*/

  recvWithStartEndMarkers();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    parseNMEA();
    newData = false;
    if (manualControl==0) {
      float leftVal, rightVal;
      // Map left thrust value to PWM
      if (thrustLeft > 0.05) {
        leftVal = mapFloat(thrustLeft, 0.0, 100.0, 191.0, 254.0);
      }
      else if (thrustLeft < 0.05) {
        leftVal = mapFloat(thrustLeft, -100.0, 0.0, 120.0, 185.0);
      }
      else {
        leftVal = 188;
      }
      // Map right thrust value to PWM
      if (thrustRight > 0.05) {
        rightVal = mapFloat(thrustRight, 0.0, 100.0, 191, 254);
      }
      else if (thrustRight < 0.05) {
        rightVal = mapFloat(thrustRight, -100.0, 0.0, 120, 185);
      }
      else {
        rightVal = 188;
      }
      curLeft = round(leftVal);
      curRight = round(rightVal);

      if (!TEST_MODE) {
        analogWrite(leftMotorPin, curLeft);
        analogWrite(rightMotorPin, curRight); }
    }
  }
  
  // convert PWM values to thrust percentages to report back to MOOS-IvP
  // if TEST_MODE is true then also commands LED brightness
  float leftSend, rightSend;
  if (curLeft > 190) {
    leftSend = mapFloat(float(curLeft), 191.0, 254.0, 0.0, 100.0);
    if (TEST_MODE) {
      analogWrite(9, map(curLeft,191,254,0,255));
      analogWrite(11, 0); }
  }
  else if (curLeft < 186) {
    leftSend = mapFloat(float(curLeft), 120.0, 185.0, -100.0, 0.0);
    if (TEST_MODE) {
      analogWrite(11, map(curLeft,185,120,0,255));
      analogWrite(9, 0); }
  }
  else {
    leftSend = 0.0;
    if (TEST_MODE) {
      analogWrite(9, 0);
      analogWrite(11, 0); }
  }
  // Map right thrust value to PWM
  if (curRight > 190) {
    rightSend = mapFloat(float(curRight), 191.0, 254.0, 0.0, 100.0);
    if (TEST_MODE) {
      analogWrite(8, map(curRight,191,254,0,255));
      analogWrite(10, 0); }
  }
  else if (curRight < 186) {
    rightSend = mapFloat(float(curRight), 120.0, 185.0, -100.0, 0.0);
    if (TEST_MODE) {
      analogWrite(10, map(curRight,185,120,0,255));
      analogWrite(8, 0); }
  }
  else {
    rightSend = 0.0;
    if (TEST_MODE) {
      analogWrite(8, 0);
      analogWrite(10, 0); }
  }

  // only print the calculated output once in a while
  if (counter++ <= PRINT_EVERY_N_UPDATES) {
    return;
  }
  // reset the counter
  counter = 0;

  //Euler angle NMEA string
  String PAYLOAD_EULER = String(manualControl) + "," + String(heading) + "," + String(pitch) + "," + String(roll);  
  String NMEA_EULER = generateNMEAString(PAYLOAD_EULER, PREFIX, ID_EULER);
  Serial.println(NMEA_EULER);

  //Raw IMU data NMEA string
  String PAYLOAD_RAW = String(ax) + "," + String(ay) + "," + String(az) + "," + 
                       String(gx) + "," + String(gy) + "," + String(gz) + "," + 
                       String(mx) + "," + String(my) + "," + String(mz);
  String NMEA_RAW = generateNMEAString(PAYLOAD_RAW, PREFIX, ID_RAW);
  Serial.println(NMEA_RAW);

  //Last motor commands NMEA string
  String PAYLOAD_MOTOR = String(leftSend) + "," + String(rightSend);
  String NMEA_MOTOR = generateNMEAString(PAYLOAD_MOTOR, PREFIX, ID_MOTOR);
  Serial.println(NMEA_MOTOR);

}

String generateNMEAString(String payload, String prefix, String id)
{
  String nmea = "";
  nmea = prefix + id + "," + payload;
  return "$" + nmea + "*";    // Prefixed with $
}

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '$';
    char endMarker = '*';
    char rc;
 
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void parseNMEA() {      // split the data into its parts

    char * strtokIndx; // this is used by strtok() as an index

    strtokIndx = strtok(tempChars,",");      // get the first part - the string
    strcpy(nmeaHeader, strtokIndx); // copy it to nmeaHeader

    strtokIndx = strtok(NULL,",");
    thrustLeft = atof(strtokIndx);     // convert the second entry to the left thrust percentage command

    strtokIndx = strtok(NULL,",");
    thrustRight = atof(strtokIndx);     // convert the third entry to the right thrust percentage command
}

// Re-maps a float number from one range to another.
// That is, a value of fromLow would get mapped to toLow, a value of fromHigh to toHigh, values in-between to values in-between, etc.
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void handleDriving(int throttle, int steering) {
    // Refer to comment in setup() function.
  // Take away 49 from both so that
  // throttle 0 -> no throttle
  // steering 0 -> no turn (straight)
  throttle -= 49;
  steering -= 49;

  int leftVal, rightVal;
  double throttleReduction;

  // If the throttle is close enough to zero, the robot stays still.
  if (abs(throttle) < THROTTLE_ZERO_THRESHOLD) {
    leftVal = STILL;
    rightVal = STILL;
  }
  // If the throttle is greater than zero, then the robot goes forward
  else if (throttle > 0) {
    // turn left forward
    if (steering <= 0) {
      // reduce throttle in the left servo to go left forwards.
      throttleReduction = mapFloat(steering, 0, -49, 1, 0); 
      leftVal = map(throttle*throttleReduction, 0, 50, LEFT_FORWARD_MIN, LEFT_FORWARD_MAX);
      rightVal = map(throttle, 0, 50, RIGHT_FORWARD_MIN, RIGHT_FORWARD_MAX);
    }
    // turn right forward
    else {
      // reduce throttle in the right servo to go right forward.
      throttleReduction = mapFloat(steering, 1, 50, 1, 0); 
      leftVal = map(throttle, 0, 50, LEFT_FORWARD_MIN, LEFT_FORWARD_MAX);
      rightVal = map(throttle*throttleReduction, 0, 50, RIGHT_FORWARD_MIN, RIGHT_FORWARD_MAX);
    }
  }
  // backward
  else {
    // TODO: implement this
    // turn left backward
    if (steering <= 0) {
      // reduce throttle in the left servo to go left backwards.
      throttleReduction = mapFloat(steering, 0, -49, 1, 0); 
      leftVal = map(throttle*throttleReduction, 0, -49, LEFT_BACKWARD_MIN, LEFT_BACKWARD_MAX);
      rightVal = map(throttle, 0, -49, RIGHT_BACKWARD_MIN, RIGHT_BACKWARD_MAX);
    }
    // turn right backward
    else {
      // reduce throttle in the right servo to go right backwards.
      throttleReduction = mapFloat(steering, 1, 50, 1, 0); 
      leftVal = map(throttle, 0, -49, LEFT_BACKWARD_MIN, LEFT_BACKWARD_MAX);
      rightVal = map(throttle*throttleReduction, 0, -49, RIGHT_BACKWARD_MIN, RIGHT_BACKWARD_MAX);
    }
  }

  curLeft = leftVal;
  curRight = rightVal;

  if (!TEST_MODE) {
    analogWrite(leftMotorPin, leftVal);
    analogWrite(rightMotorPin, rightVal); }
  
}
