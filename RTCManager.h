/*
Description: Time mamnagement mechanism for the specific RTC (custom library)
Author: Saurabh datta.
Time: Oct, 2021.
Loca: Beijing, China.
License: MIT
*/

#include <RV8803Tiny.h>
RV8803Tiny rtc;

bool rtcAvailable;
bool rtcReadable;

unsigned long startCountMillis;
unsigned long currentCountMillis;
const unsigned long secPeriod = 1000;  //the value is a number of milliseconds



void setupRTC() {
  while (rtc.begin() == false) { // un-shifted default address for RV-8803 is 0x32. Check library!
    rtcAvailable = false;
  }
  rtcAvailable = true;
  //  startCountMillis = millis();
}
