#include <RV8803Tiny.h>
RV8803Tiny rtc;

bool rtcAvailable;
bool rtcReadable;

//test comment

int sec = 0;
//int compensation_sec = 10; // compensation sec for delays in startinga nd uploading till setting time
int minute = 40;
int hour = 19; // Set value in 24 hour mode
int date = 24;
int month = 7;
int year = 2021;
int weekday = 6;

unsigned long startCountMillis;
unsigned long currentCountMillis;
const unsigned long secPeriod = 1000;  //the value is a number of milliseconds



void setupRTC() {
  //  Serial.println("\nChecking RTC...");
  //  Serial.println("\nChecking Serial Set Time...");

  while (rtc.begin() == false) { // un-shifted default address for RV-8803 is 0x32. Check library
    rtcAvailable = false;
    // -- ** Debug line remove later ** -- //
    //    Serial.println(F("Something went wrong, check wiring"));
    delay(1000);
  }
  //  Serial.println(F("RTC online!\n"));
  rtcAvailable = true;

  // -- ** Debug line remove later ** -- //
  //  Serial.println(F("Setting to compiler time..."));
  //  if (rtc.setToCompilerTime() == false) {
  //    // -- ** Debug line remove later ** -- //
  //    Serial.println("Something went wrong setting the time");
  //    while (true) {}
  //  }
  //  // -- ** Debug line remove later ** -- //
  //  Serial.println(F("Compiler Time set!"));

  // -- ** Debug line remove later ** -- //
  //  Serial.println(F("Setting custom time..."));
  //  if (rtc.setTime(sec, minute, hour, weekday, date, month, year) == false) {
  //    // -- ** Debug line remove later ** -- //
  //    Serial.println("Something went wrong setting the time");
  //    while (true) {}
  //  }
  //  // -- ** Debug line remove later ** -- //
  //  Serial.println(F("Custom Time set!"));

  //  delay(1000);

  startCountMillis = millis();
}
