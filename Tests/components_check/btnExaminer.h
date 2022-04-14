/*
Description: Button press reader and reporter for test sketch
Author: Saurabh datta.
Time: Oct, 2021.
Loca: Beijing, China.
License: MIT
*/

unsigned long btnWatchTimerStart;
unsigned long btnWatchTimerCurr;
int btnWatcherFlag = 0;
//int watchBtnFor = 10 ; // sec
#define BTN_PIN 14
#define watchBtnFor 10   // sec
bool SW_OneState;
bool old_SW_OneState;

boolean btnInitialized;
bool btnConnChecked;

void initBtnPin(int Pin) {
  if (!btnInitialized) {
    //--- Buttons initialization ---//
    //  PORTC.PIN2CTRL = 0b00001001;
    pinMode(Pin, INPUT_PULLUP);
    btnInitialized = true;
  }
}

void btnChecker() {
  while (true) {
    btnWatchTimerCurr = millis();
    if ( btnWatchTimerCurr - btnWatchTimerStart >= 1000) {

      btnWatcherFlag++;
      SW_OneState = digitalRead(14);

      Serial.print("SW_STATE: ");
      Serial.print(!SW_OneState);
      Serial.print(" [");
      Serial.print(10 - btnWatcherFlag);
      Serial.println(" sec left]");

      if (btnWatcherFlag >= watchBtnFor) {
        btnWatcherFlag = 0;
        //        btnConnChecked = true;
        //        printHeader = false;
        break;
      }

      btnWatchTimerStart = btnWatchTimerCurr;
    }
  }
}
