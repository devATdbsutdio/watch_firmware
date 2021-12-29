/*
  Description: Checking if RTC is connected well, can be read from and written to.
  Author: Saurabh datta.
  Time: Oct, 2021.
  Loca: Beijing, China.
  License: MIT
*/


#include <RV8803Tiny.h>
RV8803Tiny rtc;

int checking_rtc_counter = 0;
bool rtcAvailable;
bool rtcReadable;

bool rtcAvailabilityChecked;
bool rtcReadabilityChecked;

void reportRTCAvailability() {
  while (!rtc.begin()) {
    if (checking_rtc_counter < 20) {
      checking_rtc_counter += 1;
    } else {
      checking_rtc_counter = 0;
      break;
    }
  }

  if (!rtc.begin()) {
    rtcAvailable = false;
    Serial.println("RTC_ADDR_FOUND:0");
  } else {
    rtcAvailable = true;
    Serial.println("RTC_ADDR_FOUND:1");
  }
}

void rtcReadWriteChecker() {
  while (rtcAvailable) {
    if (rtc.updateTime()) {
      rtcReadable = true;
      // --- ** No need for this section according to a new method [ currTimeAsArray() ] in rtc lib ** --- //
      //      delay(100);
      //      rtc.updateTimeArray(); // "updateTimeArray()" function has been dissolved in the RTClibrary
      //-------------------------------------------------------------------------------------------- ------//
      delay(100);
      // Send string date & time
      Serial.println(String(rtc.stringTime()));
      Serial.println(String(rtc.stringDate()));
    } else {
      rtcReadable = false;
      Serial.println("RTC_READABLE:0");
    }
    break;
  }
  if (rtcReadable) {
    Serial.println("RTC_READABLE:1");
  } else {
    Serial.println("RTC_READABLE:0");
  }
}
