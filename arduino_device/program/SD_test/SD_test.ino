// Button =============================================
const int buttonPin = 8;
int buttonState = 0;
int previousButtonState = 0;

// SD ==============================================
#include <SPI.h>
#include <SD.h>
File dataFile;
#define CS 10
int recordingState = 0; // 0 => not using the SD card
int count = 0;

void setup() {
  // Serial ========================================
  Serial.begin(9600);

  // Button ========================================
  pinMode(buttonPin, INPUT);

  // SD ============================================
  if (!SD.begin(CS)) {
    Serial.println(F("ERROR: SD Card initialization failed"));
    while (1);
  }
  Serial.println(F("SD Card connexion successful"));

  Serial.println("Ready to test the SD!");
}

void loop() {
  // Button ========================================
  buttonState = digitalRead(buttonPin);
  if (buttonState != previousButtonState) {
    previousButtonState = buttonState; // Update button state
    if (buttonState == LOW) { // The button went from on to off (released)
      recordingState = (recordingState == 0); // Change recordingState
      Serial.println("BUTTON RELEASED");
    }
  }

  // SD ============================================
  if (recordingState != 0) {
    dataFile = SD.open("data.txt", FILE_WRITE);
    if (dataFile) {
      dataFile.println("Bonjour " + String(++count));
      dataFile.close(); // Test -> open/close file only when button is pushed
    }
    else {
      Serial.println("Error: unable to open data.txt");
    }
  }

}
