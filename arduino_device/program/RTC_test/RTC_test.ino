#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

// RTC ===============================================
tmElements_t tm;
const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

void setup() {
  // RTC ===========================================
  bool parse = false;
  bool config = false;
  // Get the date and time
  if (getDate(__DATE__) && getTime(__TIME__)) {
    parse = true;
    // Configure RTC
    if (RTC.write(tm)) {
      config = true;
    }
  }

  // Serial ========================================
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  if (!(parse && config)) {
    Serial.println("ERROR: RTC");
  }

  /*if (parse && config) {
    Serial.print("DS1307 configured Time=");
    Serial.print(__TIME__);
    Serial.print(", Date=");
    Serial.println(__DATE__);
    } else if (parse) {
    Serial.println("DS1307 Communication Error :-{");
    Serial.println("Please check your circuitry");
    } else {
    Serial.print("Could not parse info from the compiler, Time=\"");
    Serial.print(__TIME__);
    Serial.print("\", Date=\"");
    Serial.print(__DATE__);
    Serial.println("\"");
    }*/

  Serial.println("Ready to test RTC!");
}

void loop() {
  // RTC ===========================================
  String dataString = "";
  if (RTC.read(tm)) {
    dataString += tmYearToCalendar(tm.Year);
    dataString += "/";
    dataString += String(tm.Month);
    dataString += "/";
    dataString += String(tm.Day);
    dataString += ";";
    dataString += String(tm.Hour);
    dataString += ":";
    dataString += String(tm.Minute);
    dataString += ":";
    dataString += String(tm.Second);
    dataString += ";";
  } else {
    if (RTC.chipPresent()) {
      Serial.println(F("ERROR: RTC stopped"));
    } else {
      Serial.println(F("ERROR: RTC disconnected"));
    }
  }
  Serial.println(dataString);

  /*if (RTC.read(tm)) {
    Serial.print("Ok, Time = ");
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.print(", Date (D/M/Y) = ");
    Serial.print(tm.Day);
    Serial.write('/');
    Serial.print(tm.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.println();
    } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(9000);
    }*/
  delay(1000);
}
