#include <avr/sleep.h>
//#include <avr/power.h>

#include "ExtraUtils.h"
#include "RTCManager.h"
#include "DisplayManager.h"
#include "SerialReset.h"
#include "Buttons.h"

int stayAwakeFor = 4000;


void setup() {
  disableSerialHWPins();
  disableTWI();

  //--- Disable unused pins (i.e do not keep them floating) | For efficient low power in sleep mode ---//
  disableUnusedPins();

  //--- Seven segment display initialization ---//
  setupDisplay();
  turnOffDisplay();

  //--- Button Modes Enabled ---//
  setupButtons();

  //--- disable ADC
  ADC0.CTRLA &= ~(ADC_ENABLE_bm);
  //--- disable SPI
  SPI0.CTRLA &= ~(SPI_ENABLE_bm);

  // Enable interrupt
  sei();

  // Setup some counters...
  startCountMillis = millis(); // for the ext rtc
  startMicros = micros();      // for display fps

  //--- Sleep mode enablers ---//
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
}




void loop() {
  //------//
  watchButtons();


  if (showCurrTimePressed) {
    showCurrTimePressed = false;

    //--- Enable Serial for time setting over serial ---//
    Serial.begin(115200);
    //--- RV-8803 Ext RTC initialization ---//
    setupRTC();


    // -- ** Debug line remove later ** -- //
    //    Serial.print("\"Show Time\" button has been released. So show time for ");
    //    Serial.print(stayAwakeFor / 1000);
    //    Serial.println(" sec.");

    //--- start the timer for how long to show [In Buttons.h] ---//
    RTC_DELAY_init(stayAwakeFor);

    while ( showTimePeriodOver == 0) {
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
    Serial.flush();                    // flush everything before going to sleep
    Serial.end();
    disableSerialHWPins();
    disableTWI();
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
