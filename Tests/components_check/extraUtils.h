/*
Description: Extra place to hold helper functions like imnformative header printer, var initializer etc. 
Author: Saurabh datta.
Time: Oct, 2021.
Loca: Beijing, China.
License: MIT
*/


bool printHeader;

//-- Init functions **
void initAllTheTimeTrackers() {
  pinTestModeStartMillis = millis();
  btnWatchTimerStart = millis();
  startDisplayFPSMicros = micros();
  startCountMillis = millis();
}

void printStepTitle(String Title, int holdDelay) {
  if (!printHeader) {
    Serial.println("\n" + Title + ":");
    String underline = "-";
    for (int i = 0; i < Title.length(); i++) {
      underline = underline + "-";
    }
    Serial.println(underline);
    printHeader = true;
    delay(holdDelay);
  }
}
