/*
  Description: Main sketch for running test sequence and reporting over serial
  Author: Saurabh datta.
  Time: Oct, 2021.
  Loca: Beijing, China.
  License: MIT

  0. check all the internal IO states of uc
  0.5: TBD: if the traces are correct??
  1. check rtc clock.
  2. check curr factory default time.
  3. check button.
  4. check display.
*/

#include "internalIOExaminer.h"
#include "rtcExaminer.h"
#include "btnExaminer.h"
#include "displayOperator.h"
#include "extraUtils.h"

#define HOLD_DELAY 2000

bool sentSerialcheckFlag;


void setup() {
  Serial.begin(115200);
  while (!Serial) {
    // trying to connect to serial...
  }
  delay(5000);
  initAllTheTimeTrackers();
}


void loop() {
  /* --- 0. Send Serial check flag = 1 ---*/
  if (!sentSerialcheckFlag) {
    Serial.println("SERIAL:1");
    sentSerialcheckFlag = true;
  }

  /* ----  1. Check ATTINY's Internal digital IO Pin Status. ---- */
  printStepTitle("CHECKING ATTINY's INT IOs", HOLD_DELAY);

  if (!pinExaminationFinished) {
    examinePins();

    pinExaminationFinished = true;
    printHeader = false;
  }


  /* ----  2. Check rtc clock addr. ---- */
  printStepTitle("CHECKING RTC's AVAILABILITY", HOLD_DELAY);

  if (pinExaminationFinished && !rtcAvailabilityChecked) {
    reportRTCAvailability();
    rtcAvailabilityChecked = true;
    printHeader = false;
  }


  /* ---- 3. Check curr. factory default time ---- */
  printStepTitle("CHECKING RTC's R/W FUNC", HOLD_DELAY);

  if (rtcAvailabilityChecked && !rtcReadabilityChecked) {
    rtcReadWriteChecker();

    rtcReadabilityChecked = true;
    printHeader = false;
  }


  /* ---- 4. check button for presses ---- */
  printStepTitle("CHECKING BUTTON COND [Press]", HOLD_DELAY);
  initBtnPin(BTN_PIN);

  if (rtcReadabilityChecked && !btnConnChecked) {
    btnChecker();

    btnConnChecked = true;
    printHeader = false;
  }


  /* ---- 5. check display routine ---- */
  printStepTitle("CHECKING DISPLAY's CONDITION", HOLD_DELAY);
  printDisplayObservingInstructions();
  initSegmentDisplay();

  if (btnConnChecked && !displayChecked) {
    operateDisplay();

    Serial.println("\nDISPLAY WORKING? (write below)");
    Serial.println("!");

    displayChecked = true;
  }
}
