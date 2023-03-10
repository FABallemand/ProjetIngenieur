//====================================================
// Ingineering Project | TPS x Alcatel Lucent | 2023 |
//====================================================

/*====================================================
  This files contains the code that should be flashed
  on an Arduino Nano in order to collect road data.
  ====================================================*/

// #define BREAD_BOARD
#define GPS_BOARD
// #define NO_GPS_BOARD

// GPS ===============================================
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

#define swsTX 5
#define swsRX 4
SoftwareSerial GPS(swsTX, swsRX);
//Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

/*void displaySensorDetails(void)
  {
  sensor_t sensor;
  mag.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" uT");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" uT");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" uT");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
  }*/

// IMU ===============================================
/*====================================================
  NOTE: In addition to connection 5v, GND, SDA, and
  SCL, this sketch depends on the MPU-6050's INT pin
  being connected to the Arduino's external interrupt
  #0 pin.
  On the Arduino Nano, this is digital I/O pin 2.
  ====================================================*/
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

MPU6050 mpu; // Default adresses: AD0 low = 0x68, AD0 high = 0x69
#define INTERRUPT_PIN 2
#define LED_PIN 13
bool blinkState = false;
// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
// Orientation/motion vars
Quaternion q;        // [w, x, y, z]         quaternion container
VectorInt16 aa;      // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;  // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld; // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity; // [x, y, z]            gravity vector
float euler[3];      // [psi, theta, phi]    Euler angle container
float ypr[3];        // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
// Interruption detection routine
volatile bool mpuInterrupt = false; // indicates whether MPU interrupt pin has gone high
void dmpDataReady()
{
  mpuInterrupt = true;
}
// Output
// #define OUTPUT_READABLE_QUATERNION // quaternion components
// #define OUTPUT_READABLE_EULER // Euler angles (in degree) (http://en.wikipedia.org/wiki/Gimbal_lock)
// #define OUTPUT_READABLE_YAWPITCHROLL // Yaw/Pitch/Roll angles (in degree, also requires gravity vector calculations) (http://en.wikipedia.org/wiki/Gimbal_lock)
// #define OUTPUT_READABLE_REALACCEL // Acceleration components with gravity removed (not compensadted for orientation)
#define OUTPUT_READABLE_WORLDACCEL // Acceleration components with gravity removed and adjusted for the world frame of reference (yaw is relative to initial orientation, since no magnetometer is present in this case)

// Button ============================================
#define BUTTON_PIN 8
int buttonState = 0;
int previousButtonState = 0;

// SD Card ===========================================
#include <SPI.h>
#include <SD.h>

File dataFile;
#define CS 10
int recordingState = 0; // 0 => not using the SD card
int fileNumber = 0;

//====================================================
// Initial Set-up ====================================
//====================================================

void setup()
{
  // I2C Bus =========================================
  // Join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  // Serial ==========================================
  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("# Begin setup");

  // GPS =============================================
  // GPS
  GPS.begin(9600);
  // Magnetometer
  /*if (!mag.begin())
    {
    Serial.println("ERROR: magnetometer");
    while (1);
    }*/
  // displaySensorDetails();

  // IMU =============================================
  // Initialize device
  mpu.initialize();
  pinMode(INTERRUPT_PIN, INPUT);
  if (!mpu.testConnection())
  {
    Serial.println("ERROR: IMU");
  }

  // DMP (Digital Motion Processor) from IMU =========
  // Load DMP
  devStatus = mpu.dmpInitialize();
  // Configure with your own gyro offsets here, scaled for min sensitivity
#ifdef BREAD_BOARD
  mpu.setXAccelOffset(-2253);
  mpu.setYAccelOffset(-1387);
  mpu.setZAccelOffset(1552);
  mpu.setXGyroOffset(107);
  mpu.setYGyroOffset(-53);
  mpu.setZGyroOffset(-28);
#endif
#ifdef GPS_BOARD
  mpu.setXAccelOffset(-1616);
  mpu.setYAccelOffset(-500);
  mpu.setZAccelOffset(923);
  mpu.setXGyroOffset(37);
  mpu.setYGyroOffset(-18);
  mpu.setZGyroOffset(24);
#endif
#ifdef NO_GPS_BOARD // Not real values
  mpu.setXAccelOffset(-2253);
  mpu.setYAccelOffset(-1387);
  mpu.setZAccelOffset(1552);
  mpu.setXGyroOffset(107);
  mpu.setYGyroOffset(-53);
  mpu.setZGyroOffset(-28);
#endif
  // Working test (0 => OK)
  if (devStatus == 0)
  {
    // Calibration Time: generate offsets and calibrate our MPU6050
    mpu.CalibrateAccel(6); // ???
    mpu.CalibrateGyro(6);
    mpu.PrintActiveOffsets();
    // Turn on the DMP
    mpu.setDMPEnabled(true);
    // Enable Arduino interrupt detection
    /*Serial.print("Enabling interrupt detection (Arduino external interrupt ");
      Serial.print(digitalPinToInterrupt(INTERRUPT_PIN));
      Serial.println(")");*/
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();
    // Set DMP Ready flag
    dmpReady = true;
    // Get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
  }
  else
  {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    Serial.print("ERROR: DMP (code ");
    Serial.print(devStatus);
    Serial.println(")");
  }
  // Configure LED for output
  pinMode(LED_PIN, OUTPUT);

  // Button ==========================================
  pinMode(BUTTON_PIN, INPUT);

  // SD Card =========================================
  if (!SD.begin(CS)) {
    Serial.println("ERROR: SD Card");
    while (1);
  }

  Serial.println("# OK");
}

//====================================================
// Main Program ======================================
//====================================================

void loop()
{
  // If initialisation failed... =====================
  if (!dmpReady)
    return;

  String dataString = "";

  // GPS =============================================
  while (GPS.available() > 0)
  {
    dataString += GPS.read();
  }
  dataString += ";";
  // Get a new sensor event
  /*sensors_event_t event;
    mag.getEvent(&event);*/

  // Display the results (magnetic vector values are in micro-Tesla (uT))
  /*Serial.print("X: "); Serial.print(event.magnetic.x); Serial.print("  ");
    Serial.print("Y: "); Serial.print(event.magnetic.y); Serial.print("  ");
    Serial.print("Z: "); Serial.print(event.magnetic.z); Serial.print("  "); Serial.println("uT");*/

  // Compute heading angle (taking into account the declination angle)
  /*float heading = atan2(event.magnetic.y, event.magnetic.x);
    float declinationAngle = (2.5333 * PI) / 180;
    heading += declinationAngle;
    if (heading < 0)
    heading += 2 * PI;
    if (heading > 2 * PI)
    heading -= 2 * PI;
    float headingDegrees = heading * 180 / M_PI;
    dataString += String(headingDegrees);
    dataString += ";";*/

  // IMU =============================================
  // Read the latest packet from FIFO
  if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer))
  {
#ifdef OUTPUT_READABLE_QUATERNION
    // Display quaternion values in easy matrix form: w x y z
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    Serial.print("quat\t");
    Serial.print(q.w);
    Serial.print("\t");
    Serial.print(q.x);
    Serial.print("\t");
    Serial.print(q.y);
    Serial.print("\t");
    Serial.println(q.z);
#endif

#ifdef OUTPUT_READABLE_EULER
    // Display Euler angles in degrees
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetEuler(euler, &q);
    Serial.print("euler\t");
    Serial.print(euler[0] * 180 / M_PI);
    Serial.print("\t");
    Serial.print(euler[1] * 180 / M_PI);
    Serial.print("\t");
    Serial.println(euler[2] * 180 / M_PI);
#endif

#ifdef OUTPUT_READABLE_YAWPITCHROLL
    // Display Euler angles in degrees
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    /*Serial.print("ypr\t");
      Serial.print(ypr[0] * 180 / M_PI);
      Serial.print("\t");
      Serial.print(ypr[1] * 180 / M_PI);
      Serial.print("\t");
      Serial.println(ypr[2] * 180 / M_PI);*/
    dataString += String(ypr[0] * 180 / M_PI);
    dataString += ";";
    dataString += String(ypr[1] * 180 / M_PI);
    dataString += ";";
    dataString += String(ypr[2] * 180 / M_PI);
    dataString += ";";
#endif

#ifdef OUTPUT_READABLE_REALACCEL
    // Display real acceleration, adjusted to remove gravity
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetAccel(&aa, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
    Serial.print("areal\t");
    Serial.print(aaReal.x);
    Serial.print("\t");
    Serial.print(aaReal.y);
    Serial.print("\t");
    Serial.println(aaReal.z);
#endif

#ifdef OUTPUT_READABLE_WORLDACCEL
    // Display initial world-frame acceleration, adjusted to remove gravity
    // and rotated based on known orientation from quaternion
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetAccel(&aa, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
    mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
    /*Serial.print("aworld\t");
      Serial.print(aaWorld.x);
      Serial.print("\t");
      Serial.print(aaWorld.y);
      Serial.print("\t");
      Serial.println(aaWorld.z);*/
    dataString += String(aaWorld.x);
    dataString += ";";
    dataString += String(aaWorld.y);
    dataString += ";";
    dataString += String(aaWorld.z);
#endif

    // Blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
  }

  // Button ==========================================
  buttonState = digitalRead(BUTTON_PIN);
  if (buttonState != previousButtonState)
  {
    previousButtonState = buttonState;
    if (buttonState == LOW)
    { // The button went from on to off (released)
      recordingState = (recordingState == 0); // Change recordingState
      if (recordingState)
      {
        ++fileNumber;
        Serial.println("-> Start recording " + String(fileNumber));
      } else {
        Serial.println("-> Stop recording " + String(fileNumber));
      }
    }
  }

  // SD Card =========================================
  // Serial.println(dataString);
  if (recordingState != 0) {
    dataFile = SD.open("data" + String(fileNumber) + ".txt", FILE_WRITE);
    if (dataFile) {
      dataFile.println(dataString);
      dataFile.close(); // Test -> open/close file only when button is pushed
      Serial.println(dataString); // For testing purpose only
    }
    else {
      Serial.println("Error: unable to open file");
    }
  }

  delay(10); // For testing purpose only
}
