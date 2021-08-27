#include <avr/sleep.h>
//#include <avr/power.h>

#include "ExtraUtils.h"
#include "RTCManager.h"
#include "DisplayManager.h"
#include "SerialReset.h"
#include "Buttons.h"

int stayAwakeFor = 5000;


void setup() {
  //  Serial.begin(115200);

  //--- Disable the Serial so that it doesn't draw curr in TX pin during sleep ---//
  //-- [In ExtraUtils.h] --//
  //  disableSerial();

  //--- Disable unused pins (i.e do not keep them floating) | For efficient low power in sleep mode ---//
  disableUnusedPins();

  //--- Seven segment display initialization ---//
  setupDisplay();

  //--- Seven segment display initialization ---//
  setupRTC();

  //--- Button Modes Enabled ---//
  setupButtons();

  //--- disable some ADC, SPI, TIMERS
  ADC0.CTRLA &= ~ADC_ENABLE_bm;

  //--- Sleep mode enablers ---//
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
}




void loop() {
  //------//
  watchButtons();


  if (showCurrTimePressed) {
    showCurrTimePressed = false;

    // -- ** Debug line remove later ** -- //
    //    Serial.print("\"Show Time\" button has been released. So show time for ");
    //    Serial.print(stayAwakeFor / 1000);
    //    Serial.println(" sec.");


    //-- Enable Serial [In ExtraUtils.h] --//
    Serial.begin(115200);
    //    enableSerial();

    //--- start the timer for how long to show [In Buttons.h] ---//
    RTC_DELAY_init(stayAwakeFor);

    while ( showTimePeriodOver == 0) {

      // Note: Serial should have been enabled in the Buttons.h [ in void watchButtons(){} ], when the button press interrupt was detected.
      // So set time over serial routine (If serial data is availbe in this time window)
      SetTimeOverSerial();

      // Anyways, "show time here" routine
      getAndShowTime();
    }

    // Reset Trigger for RTC delay
    showTimePeriodOver = 0;

    // Then go to sleep
    // -- ** Debug line remove later ** -- //
    //    Serial.println(F("Sleeping..."));
    turnOffDisplay();
    //    Serial.flush();                    // flush everything before going to sleep
    Serial.end();
    //    disableSerial();
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
      //      Serial.print(rtc.currTimeArray[0]);
      //      Serial.print(rtc.currTimeArray[1]);
      //      Serial.print(":");
      //      Serial.print(rtc.currTimeArray[2]);
      //      Serial.print(rtc.currTimeArray[3]);
      //      Serial.print(":");
      //      Serial.print(rtc.currTimeArray[4]);
      //      Serial.print(rtc.currTimeArray[5]);
      //      Serial.println();
    }

    startCountMillis = currentCountMillis;
  }

  // --- ** [TBD] corner case handler TBD ** --- //
  if (rtcAvailable && rtcReadable) showOnDisplay(rtc.currTimeArray);
  else showOnDisplay(blankSignal);
}
