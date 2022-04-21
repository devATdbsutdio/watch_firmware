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


boolean debug_log = false;


uint16_t unsafeLowVoltage = 28; // 2.8V
uint16_t safeLowVoltage   = 31; //3.0V

uint8_t currTime[6];

void setup() {
  disableSerialRXPin();
  disableTWI();

  //--- Disable unused pins (i.e do not keep them floating) | For efficient low power in sleep mode ---//
  disableUnusedPins();

  //--- Seven segment display initialization ---//
  setupDisplay();
  turnOffDisplay();

  //--- Button Modes Enabled ---//
  setupButton();


  //--- Disable ADC ---//
  ADC0.CTRLA &= ~ADC_ENABLE_bm;

  //--- Note: this is how you can re-enable ADC ---//
  // ADC0.CTRLA |= ADC_ENABLE_bm;


  //--- disable SPI ---//
  SPI0.CTRLA &= ~SPI_ENABLE_bm;

  // Enable interrupt
  sei();
  // Setup some counters...
  startCountMillis = millis();        // For the ext rtc
  startMicros = micros();             // For display fps
  startWarningCountMillis = millis(); // For battery low voltage warning LED blinking

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

    //--- Detect self referenced Batt voltage ---//
    ADCVoltRefSetup();
    uint16_t currBattVolt = measuredVoltage();


    batteryWarningLED_OFF();      // On wake up, initialize the warning led dot of the segment to be OFF
    turnOffDisplay();             // On wake up, initialize the whole display segment to be OFF
    do_blink = 1;                 // On wake up, initializing the variable for low voltage warning blinking action.


    RTC_DELAY_init(stayAwakeFor); // Start the timer for keeping track of time for how long to keep the uC awake and do it's business (5000 ms)

    boolean proceed = false;

    while (showTimePeriodOver == 0) {
      // If voltage detected is lower than the safe operational voltage threshold!
      if (currBattVolt < safeLowVoltage && currBattVolt <= unsafeLowVoltage) {
        batteryWarningLED_ON();
        proceed = false;
      }

      // If voltage detected is low but not critically low and below safest threshold!
      if (currBattVolt < safeLowVoltage && currBattVolt > unsafeLowVoltage) {
        // Blocks and Blinks a dot LED, 2 times (in 1250 ms) as the warning to show that the battery voltage is falling.
        low_voltage_warn();
        // In the next remaining period [ (5000-1250) ms ] of the whole awake period, it continues to show the time.
        getAndShowTime();
        proceed = true;
      }

      // If voltage detected is OK! and above operational and safe threshold!
      if (currBattVolt >= safeLowVoltage && currBattVolt > unsafeLowVoltage) {
        proceed = true;
      }

      if (proceed) {
        // If data arrives over serial, it will check for data format
        // and set time to RTC and then "show time" routine also runs after that!
        // (Anyways) "show time" routine will run even if serial data isn't there
        SetTimeOverSerial();
        getAndShowTime();
      }
    }

    // Reset Trigger for RTC delay
    showTimePeriodOver = 0;

    // Just before the next awake cycle begins, if the RTC_DELAY_init value has changed set it to new value.
    if (new_stayAwakeFor != stayAwakeFor) {
      stayAwakeFor = new_stayAwakeFor;
    }

    // Then go to sleep
    turnOffDisplay();
    Serial.flush();                    // flush everything before going to sleep
    Serial.end();
    disableSerialRXPin();
    disableTWI();
    sleep_cpu();
  }
}
