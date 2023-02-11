/*#include <SoftwareSerial.h>

// GPS ================================================
#define swsTX 4
#define swsRX 5
SoftwareSerial GPS(swsTX, swsRX);

void setup() {
  // Serial ========================================
  Serial.begin(9600);
  //Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // GPS ===========================================
  // GPS
  GPS.begin(9600);

  Serial.println("Ready to test GPS!");
}


void loop() {
  // GPS ===========================================
  while (GPS.available() > 0) {
    Serial.write(GPS.read());
  }
  delay(500);
}*/


#include <SoftwareSerial.h>

  #include <Wire.h>
  #include <Adafruit_Sensor.h>
  #include <Adafruit_HMC5883_U.h>

  // GPS ================================================
  #define swsTX 5
  #define swsRX 4
  SoftwareSerial GPS(swsTX, swsRX);

  Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

  void displaySensorDetails(void)
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
  }

  void setup() {
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
  if (!mag.begin())
  {
    Serial.println("ERROR: magnetometer");
    while (1);
  }
  // displaySensorDetails();

  Serial.println("Ready to test GPS!");
  }


  void loop() {
  // GPS ===========================================
  while (GPS.available() > 0) {
    Serial.write(GPS.read());
  }

  // Get a new sensor event
  sensors_event_t event;
  mag.getEvent(&event);

  // Display the results (magnetic vector values are in micro-Tesla (uT))
  Serial.print("X: "); Serial.print(event.magnetic.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(event.magnetic.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(event.magnetic.z); Serial.print("  "); Serial.println("uT");

  // Hold the module so that Z is pointing 'up' and you can measure the heading with x&y
  // Calculate heading when the magnetometer is level, then correct for signs of axis.
  float heading = atan2(event.magnetic.y, event.magnetic.x);

  // Once you have your heading, you must then add your 'Declination Angle', which is the 'Error' of the magnetic field in your location.
  // Find yours here: http://www.magnetic-declination.com/
  // Mine is: -13* 2' W, which is ~13 Degrees, or (which we need) 0.22 radians
  // If you cannot find your Declination, comment out these two lines, your compass will be slightly off.
  float declinationAngle = (2.5333 * M_PI) / 180;
  heading += declinationAngle;

  // Correct for when signs are reversed.
  if (heading < 0)
    heading += 2 * PI;

  // Check for wrap due to addition of declination.
  if (heading > 2 * PI)
    heading -= 2 * PI;

  // Convert radians to degrees for readability.
  float headingDegrees = heading * 180 / M_PI;

  Serial.print("Heading (degrees): "); Serial.println(headingDegrees);

  delay(500);
  }
