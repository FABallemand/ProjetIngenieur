// Button =============================================
const int buttonPin = 8;
int buttonState = 0;
int previousButtonState = 0;
int changeOnButtonRelease = 0;

void setup() {
  // Serial ========================================
  Serial.begin(9600);

  // Button ========================================
  pinMode(buttonPin, INPUT);

  Serial.println("Ready to test the button!");
}

void loop() {
  // Button ========================================
  buttonState = digitalRead(buttonPin);
  if (buttonState != previousButtonState) {
    previousButtonState = buttonState; // Update button state
    if (buttonState == LOW) { // The button went from on to off (released)
      changeOnButtonRelease = (changeOnButtonRelease == 0); // Change value
      Serial.println("BUTTON RELEASED (" + String(changeOnButtonRelease) + ")");
    } else {
      Serial.println("BUTTON PRESSED");
    }
  }
}
