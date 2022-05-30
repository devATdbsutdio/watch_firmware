/*
  Description: MAIN Main sketch for the watch and the operation mechanism
  Author: Saurabh datta.
  Time: Aug, 2021.
  Loc: Beijing, China.
  License: MIT
*/

//#define DEBUG // comment this for production

#include <avr/sleep.h>

#include "DisplayManager.h"
#include "ExtraUtils.h"
#include "RTCManager.h"
#include "SerialReset.h"
#include "Buttons.h"


const uint16_t unsafeLowVoltage = 28; // 2.8V
const uint16_t safeLowVoltage   = 31; // 3.0V

uint8_t currTime[6];

void setup() {
  disableSerialHWPins();
  disableTWI();

  //--- Disable unused pins (i.e do not keep them floating) | For efficient low power in sleep mode ---//
  disableUnusedPins();

  //--- Seven segment display initialization ---//
  setupDisplay();
  turnOffDisplay();

  //--- Button Modes Enabled ---//
  setupButton();


  //--- Disable ADC [TBD doesn't do much] ---//
  ADC0.CTRLA &= ~ADC_ENABLE_bm;
  //  ADC0.CTRLB &= ~ADC_ENABLE_bm;
  //  ADC0.CTRLC &= ~ADC_ENABLE_bm;
  //--- Note: this is how you can re-enable ADC ---//
  // ADC0.CTRLA |= ADC_ENABLE_bm;


  //--- disable SPI ---//
  SPI0.CTRLA &= ~(SPI_ENABLE_bm);

  // Enable interrupt
  sei();

  // Setup some counters...
  startCountMillis = millis();        // For the ext rtc
  startMicros = micros();             // For display fps
  startWarningCountMillis = millis(); // For battery low voltage warning LED blinking

  // Get the delay value (for which watch will stay awake), from EEPROM
  EEPROM.get(eeprom_addr, new_stayAwakeFor);
  if (new_stayAwakeFor == -1) {
    new_stayAwakeFor = 5100;
  }

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

    // Just before the awake cycle begins, if the RTC_DELAY_init value has changed (as Read from EEPROM in setup) set it to new value.
    if (new_stayAwakeFor != stayAwakeFor) {
      stayAwakeFor = new_stayAwakeFor;
    }

    RTC_DELAY_init(stayAwakeFor); // Start the timer for keeping track of time for how long to keep the uC awake and do it's business (5000 ms)

    while (showTimePeriodOver == 0) {
      // If voltage detected is lower than the safe operational voltage threshold!
      if (currBattVolt <= unsafeLowVoltage) {
        batteryWarningLED_ON();
      }

      // If voltage detected is low but not critically low and below safest threshold!
      if (currBattVolt > unsafeLowVoltage && currBattVolt < safeLowVoltage) {
        // Blocks and Blinks a dot LED, 2 times (in 1250 ms) as the warning to show that the battery voltage is falling.
        low_voltage_warn();
        // In the next remaining period [ (5000-1250)ms ] of the whole awake period, it continues to show the time.
        getAndShowTime();
      }

      // If voltage detected is OK! and above operational and safe threshold!
      if (currBattVolt >= safeLowVoltage) {
        // If data arrives over serial,
        // it will check for data format and set time to RTC
        // Anyways, and then "show time here" routine also runs after that!
        SetTimeOverSerial();
        getAndShowTime();
      }
    }

    // Reset Trigger for RTC delay
    showTimePeriodOver = 0;

    // Then go to sleep
    turnOffDisplay();
    Serial.flush();                    // flush everything before going to sleep
    Serial.end();
    disableSerialHWPins();
    disableTWI();
    sleep_cpu();
  }
}
