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

#define BTN_PIN 14
#define watchBtnFor 10   // sec

bool SW_OneState;
bool old_SW_OneState;

bool btnInitialized;
bool btnConnChecked;

void initBtnPin(int Pin) {
  if (!btnInitialized) {
    //--- Buttons initialization ---//
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
      
      Serial.println("SW_STATE: " + String(!SW_OneState) + " [" + String(10 - btnWatcherFlag) + " sec left]");

      if (btnWatcherFlag >= watchBtnFor) {
        btnWatcherFlag = 0;
        break;
      }
      btnWatchTimerStart = btnWatchTimerCurr;
    }
  }
}
