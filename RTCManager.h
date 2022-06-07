/*
  Description: Time mamnagement mechanism for the specific RTC (custom library)
  Dependencies:
    1. RV8803Tiny   (latest, Ver: 0.0.8)
    2. TinyMegaI2C  (latest, Ver: 0.0.4)
  Author: Saurabh datta.
  Time: Aug, 2021.
  Loc: Beijing, China.
  License: MIT
*/

#include <RV8803Tiny.h>
RV8803Tiny rtc;

bool rtcAvailable;
bool rtcReadable;

unsigned long startMillisTimeUpdateCount;
unsigned long currentMillisTimeUpdateCount;
unsigned long startMillisDateUpdateCount;
unsigned long currentMillisDateUpdateCount;

const unsigned long secPeriod = 500; // The value is a number of milliseconds

/*--- Display time or date selection ---*/
boolean showCurrTime;
boolean showCurrDate;

void setupRTC() {
  /*
    For few times check rtc's availability by checking against Un-shifted default address for RV-8803 i.e 0x32.
    Check library!
  */
  for (int e = 0; e < 10; e++) {
    if (rtc.begin() == false) {
      rtcAvailable = false;
    } else {
      rtcAvailable = true;
      break;
    }
  }
  /*
    Even if rtc is un-available, proceed as we have a pattern that we show in display to communicate that rtc is not working.
    This good to notify what has broken even in the finished product.
  */
}


void getAndShowTimeOrDate() {
  currentMillisTimeUpdateCount = millis();
  if (currentMillisTimeUpdateCount - startMillisTimeUpdateCount >= secPeriod) {
    if (rtcAvailable) {
      // UpdateTime i.e read registers, ** must for getting current time.
      // Note: Just do a couple of updates ...
      rtc.updateTime();
      rtc.updateTime();
      if (rtc.updateTime()) rtcReadable = true;
      else rtcReadable = false;
    } else {
      rtcReadable = false;
    }
    startMillisTimeUpdateCount = currentMillisTimeUpdateCount;
  }

  // Display Time / Date, depending on if we have pressed the button again and again, when the watch is awake.
  if (rtcReadable) {
    if (showCurrTime && !showCurrDate) {
      showOnDisplay(rtc.currTimeAsArray());
    } else if (showCurrDate && !showCurrTime) {
      showOnDisplay(rtc.currDateAsArray());
    } else { /* corner case handler (In case button is not working) */
      showOnDisplay(blankSignal);
    }
  } else { /* corner case handler (In case time retreival was unsuccessful) */
    showOnDisplay(blankSignal);
  }
}
