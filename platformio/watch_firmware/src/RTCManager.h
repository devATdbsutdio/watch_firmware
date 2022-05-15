/*
  Description: Time mamnagement mechanism for the specific RTC (custom library)
  Author: Saurabh datta.
  Time: Aug, 2021.
  Loc: Beijing, China.
  License: MIT
*/

#include <RV8803Tiny.h>

RV8803Tiny rtc;

bool rtcAvailable;
bool rtcReadable;

unsigned long startCountMillis;
unsigned long currentCountMillis;
const unsigned long secPeriod = 1000; // the value is a number of milliseconds

void setupRTC()
{
    while (rtc.begin() == false)
    { // un-shifted default address for RV-8803 is 0x32. Check library!
        rtcAvailable = false;
    }
    rtcAvailable = true;
}

void getAndShowTime()
{
    currentCountMillis = millis();
    if (currentCountMillis - startCountMillis >= secPeriod)
    {
        if (rtcAvailable)
        {
            // updateTime i.e read registers, ** must for getting current time
            if (rtc.updateTime())
                rtcReadable = true;
            else
                rtcReadable = false;
        }
        else
        {
            rtcReadable = false;
        }
        startCountMillis = currentCountMillis;
    }

    // :: Display Time :: //
    // --- ** corner case handler (In case time retreival was unsuccessful) ** --- //
    if (rtcAvailable && rtcReadable)
        showOnDisplay(rtc.currTimeAsArray());
    else
        showOnDisplay(blankSignal);
}
