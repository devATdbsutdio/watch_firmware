#include <avr/sleep.h>

#include "RTCManager.h"
#include "DisplayManager.h"
#include "SerialReset.h"
#include "Buttons.h"

int stayAwakeFor = 5000;


void setup() {
  // -- ** Debug code remove later ** -- //
  Serial.begin(115200);

  //--- Seven segment display initialization ---//
  setupDisplay();

  //--- Seven segment display initialization ---//
  setupRTC();

  //--- Buttn Modes Enabled ---//
  setupButtons();

  //--- Sleep mode enablers ---//
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
}




void loop() {
  //  getAndShowTime();

  //------//
  watchButtons();


  if (showCurrTimePressed) {
    showCurrTimePressed = false;

    // -- ** Debug line remove later ** -- //
    //    Serial.print("\"Show Time\" button has been released. So show time for ");
    //    Serial.print(stayAwakeFor / 1000);
    //    Serial.println(" sec.");

    //--- start the timer for how long to show [Int RTC method] ---//
    RTC_DELAY_init(stayAwakeFor);
    while ( showTimePeriodOver == 0) {
      // set time over serial
      //      if (enableSerialTransport) {
      SetTimeOverSerial();
      //      enableSerialTransport = false;
      //      }

      // "show time" routine
      getAndShowTime();
    }
    // Reset Trigger for RTC delay
    showTimePeriodOver = 0;

    // Then go to sleep
    Serial.println(F("Sleeping..."));
    turnOffDisplay();
    Serial.flush();                    // flush everything before going to sleep
    delay(10);
    sleep_cpu();
  }
}



void getAndShowTime() {
  currentCountMillis = millis();

  if (currentCountMillis - startCountMillis >= secPeriod) {

    if (rtcAvailable) {
      // updateTime i.e read registers, ** must for getting current time
      if (rtc.updateTime()) {
        rtcReadable = true;
      } else {
        rtcReadable = false;
      }
    } else {
      rtcReadable = false;
    }

    // updateTime i.e read registers, ** must for getting current time
    if (rtcAvailable && rtcReadable) {
      // ** Must push time read from registers to an int array for showing to segment display
      rtc.updateTimeArray();

      // -- ** Debug code remove later ** -- //
      Serial.print(rtc.currTimeArray[0]);
      Serial.print(rtc.currTimeArray[1]);
      Serial.print(":");
      Serial.print(rtc.currTimeArray[2]);
      Serial.print(rtc.currTimeArray[3]);
      Serial.print(":");
      Serial.print(rtc.currTimeArray[4]);
      Serial.print(rtc.currTimeArray[5]);
      Serial.println();
    }

    startCountMillis = currentCountMillis;
  }


  //  showOnDisplay(minimalTime);

  // --- ** [TBD] corner case handler TBD ** --- //
  if (rtcAvailable && rtcReadable) showOnDisplay(rtc.currTimeArray);
  else showOnDisplay(blankSignal);
}
