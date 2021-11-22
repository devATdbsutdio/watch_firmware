/*
  Description: [MAIN] Main sketch for the watch and the operation mechanism
  Author: Saurabh datta.
  Time: Aug, 2021.
  Loc: Beijing, China.
  License: MIT
*/

#include <avr/sleep.h>

#include "DisplayManager.h"
#include "ExtraUtils.h"
#include "RTCManager.h"
#include "SerialReset.h"
#include "Buttons.h"

int stayAwakeFor = 4000;
uint16_t unsafeLowVoltage = 28; // 2.7V
uint16_t safeLowVoltage = 30; //3.0V

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

  //--- disable ADC ---//
  //  ADC0.CTRLA &= ~(ADC_ENABLE_bm);

  //--- enable ADC ---//
  // ADC0.CTRLA |= ADC_ENABLE_bm;


  //--- disable SPI ---//
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

    //--- Detect self referenced Batt voltage ---//
    ADCVoltRefSetup();
    uint16_t currBattVolt = measuredVoltage();
    bool lowVoltageDetected = false;
    bool lowestVoltageDetected = false;
    // -- ** Debug line remove later ** -- //
    //    Buffer[0] = voltage / 10; Buffer[1] = voltage % 10;
    //    Serial.print(float(currBattVolt) / 10);
    //    Serial.println(" V");

    // Check if the voltage is higher that the safe voltage
    if (currBattVolt >= safeLowVoltage) {
      // Meaning that the voltage is in a good range for teh device to operate
      lowVoltageDetected = false;
      lowestVoltageDetected = false;
      batteryWarningLED_OFF();
    }
    // Check if the voltage is lower that the safe voltage
    if (currBattVolt < safeLowVoltage) {
      // Check if the voltage is still higher than the lowest un-safe voltage
      if (currBattVolt > unsafeLowVoltage) {
        lowVoltageDetected = true;
        lowestVoltageDetected = false;
      }
      // Check if the voltage is still lower than the lowest un-safe voltage
      if (currBattVolt <= unsafeLowVoltage) {
        lowVoltageDetected = true;
        lowestVoltageDetected = true;
      }
    }

    //    if (currBattVolt < unsafeLowVoltage) {
    //      lowVoltageDetected = true;
    //    } else {
    //      lowVoltageDetected = false;
    //      batteryWarningLED_OFF();
    //    }


    //--- start the timer for how long to show [In Buttons.h] ---//
    RTC_DELAY_init(stayAwakeFor);

    while ( showTimePeriodOver == 0) {
      // If battery voltage is above threshold and low voltage not detected
      if (!lowestVoltageDetected && !lowVoltageDetected) {
        // If data arrives over serial, it will check for data format and set time to RTC as it expects data to be the "setTime" data
        SetTimeOverSerial();
        // Anyways, "show time here" routine also runs
        getAndShowTime();
      } else if (!lowestVoltageDetected && lowVoltageDetected) {
        // If low voltgae detected, but not the lowest safe volatge, then show warning for some time, on button press.
        // Show warning for some time
        batteryWarningLED_ON();
        // batteryWarningLED_Blink();
        // TBD
        // ... for some time and then show the time
        // TBD
        // SetTimeOverSerial();
        // getAndShowTime();
      } else {
        // If lowest voltgae detected
        batteryWarningLED_ON();
      }
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

    // update Time i.e read registers, ** must for getting current time
    if (rtcAvailable && rtcReadable) {
      // *** Must push time read from registers to an int array for showing to segment display
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

  // --- ** corner case handler (In case time retreival was unsuccessful) ** --- //
  if (rtcAvailable && rtcReadable) showOnDisplay(rtc.currTimeArray);
  else showOnDisplay(blankSignal);
}
