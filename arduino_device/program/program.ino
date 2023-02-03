//====================================================
// Ingineering Project | TPS x Alcatel Lucent | 2023 |
//====================================================


/*====================================================
  This files contains the code that should be flash in
  an Arduino Nano in order to collect road data.
  ====================================================*/

// Libraries =========================================

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

#include <SPI.h>
#include <SD.h>

// GPS ================================================
#define swsTX 4
#define swsRX 5
SoftwareSerial GPS(swsRX, swsTX);
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

MPU6050 mpu; // Default adresses: AD0 low = 0x68, AD0 high = 0x69
#define INTERRUPT_PIN 3
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
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
// Interruption detection routine
volatile bool mpuInterrupt = false; // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
}
// Output
//#define OUTPUT_READABLE_QUATERNION // quaternion components
//#define OUTPUT_READABLE_EULER // Euler angles (in degree) (http://en.wikipedia.org/wiki/Gimbal_lock)
#define OUTPUT_READABLE_YAWPITCHROLL // Yaw/Pitch/Roll angles (in degree, also requires gravity vector calculations) (http://en.wikipedia.org/wiki/Gimbal_lock)
//#define OUTPUT_READABLE_REALACCEL // Acceleration components with gravity removed (not compensadted for orientation)
//#define OUTPUT_READABLE_WORLDACCEL // Acceleration components with gravity removed and adjusted for the world frame of reference (yaw is relative to initial orientation, since no magnetometer is present in this case)

// Button =============================================
const int buttonPin = 2;
int buttonState = 0;
int previousButtonState = 0;

// SD Card ============================================
File dataFile;
#define MOSI 11
#define MISO 12
#define CLK 13
#define CS 10
int recordingState = 0; // 0 => not using the SD card

//====================================================
// Initial Set-up ====================================
//====================================================

void setup() {
  // I2C Bus ======================================
  // Join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  // Serial ========================================
  Serial.begin(9600);
  // Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // GPS ===========================================
  // GPS
  GPS.begin(9600);
  // Magnetometer
  /*if (!mag.begin())
  {
    Serial.println("ERROR: magnetometer");
    while (1);
  }
  // displaySensorDetails();*/

  // IMU ===========================================
  // Initialize device
  mpu.initialize();
  pinMode(INTERRUPT_PIN, INPUT);
  if (!mpu.testConnection()) {
    Serial.println("ERROR: IMU");
  }

  // Wait for ready (BUTTON IN THE FUTURE) =========
  /*Serial.println(F("\nSend any character to begin DMP programming and demo: "));
    while (Serial.available() && Serial.read()); // empty buffer
    while (!Serial.available());                 // wait for data
    while (Serial.available() && Serial.read()); // empty buffer again*/

  // DMP (Digital Motion Processor) from IMU =======
  // Load DMP
  devStatus = mpu.dmpInitialize();
  // Configure with your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788); // 1688 factory default for my test chip
  // Working test (0 => OK)
  if (devStatus == 0) {
    // Calibration Time: generate offsets and calibrate our MPU6050
    mpu.CalibrateAccel(6);
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
  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    Serial.print("ERROR: DMP (code ");
    Serial.print(devStatus);
    Serial.println(")");
  }
  // configure LED for output
  pinMode(LED_PIN, OUTPUT);

  // Button ========================================
  pinMode(buttonPin, INPUT);

  // SD Card =======================================
  /*if (!SD.begin(CS)) {
    Serial.println(F("ERROR: SD Card initialization failed");
    while (1);
    }
    Serial.println(F("SD Card connexion successful"));*/

  Serial.println("= OK =");
}

//====================================================
// Main Program ======================================
//====================================================

void loop() {
  // If initialisation failed... ===================
  if (!dmpReady) return;

  String dataString = "";

  // GPS ===========================================
  if (GPS.available() > 0) {
    //dataString.write(GPS.read());
    //dataString += String(GPS.read());
    dataString += "GPS_DATA;";
  }
  // Get a new sensor event
  /*sensors_event_t event;
  mag.getEvent(&event);*/

  // Display the results (magnetic vector values are in micro-Tesla (uT))
  /*Serial.print("X: "); Serial.print(event.magnetic.x); Serial.print("  ");
    Serial.print("Y: "); Serial.print(event.magnetic.y); Serial.print("  ");
    Serial.print("Z: "); Serial.print(event.magnetic.z); Serial.print("  "); Serial.println("uT");*/

  // Compute heading angle (taking into account the declination angle)
  /*float heading = atan2(event.magnetic.y, event.magnetic.x);
  float declinationAngle = (2.5333 * M_PI) / 180;
  heading += declinationAngle;
  if (heading < 0)
    heading += 2 * PI;
  if (heading > 2 * PI)
    heading -= 2 * PI;
  float headingDegrees = heading * 180 / M_PI;
  dataString += String(headingDegrees);
  dataString += ";";*/

  // IMU ===========================================
  // Read the latest packet from FIFO
  if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
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
    Serial.print("ypr\t");
    Serial.print(ypr[0] * 180 / M_PI);
    dataString += String(ypr[0] * 180 / M_PI);
    dataString += ";";
    Serial.print("\t");
    Serial.print(ypr[1] * 180 / M_PI);
    dataString += String(ypr[1] * 180 / M_PI);
    dataString += ";";
    Serial.print("\t");
    Serial.println(ypr[2] * 180 / M_PI);
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
    Serial.print("aworld\t");
    Serial.print(aaWorld.x);
    Serial.print("\t");
    Serial.print(aaWorld.y);
    Serial.print("\t");
    Serial.println(aaWorld.z);
#endif

    // Blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
  }

  // Button ========================================
  buttonState = digitalRead(buttonPin);
  if (buttonState != previousButtonState) {
    if (buttonState == LOW) { // The button went from on to off (released)
      recordingState = (recordingState == 0); // Change recordingState
      Serial.println("BUTTON RELEASED");
    }
  }

  // SD Card =======================================
  Serial.println(dataString);
  /*if (recordingState != 0) {
    dataFile = SD.open("data.txt", FILE_WRITE);
    if (dataFile) {
      dataFile.println(dataString);
      dataFile.close(); // Test -> open/close file only when button is pushed
      Serial.println(dataString); // For testing purpose only
    }
    else {
      Serial.println("Error: unable to open data.txt");
    }
    }*/
}
