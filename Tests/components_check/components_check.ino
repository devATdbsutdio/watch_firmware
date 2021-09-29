/*
  1. check rtc clock.
  2. check curr factory default time.
  3. check button.
  4. check display.
*/

#include <RV8803Tiny.h>
RV8803Tiny rtc;

int checking_rtc_counter = 0;
bool rtcAvailable;
bool rtcReadable;

unsigned long btnWatchTimerStart;
unsigned long btnWatchTimerCurr;
int btnWatcherFlag = 0;
int watchBtnFor = 10 ; // sec
bool SW_OneState;
bool old_SW_OneState;

unsigned long startMicros;
unsigned long currentMicros;
const unsigned long period = 10;  // the value is a number of Microseconds

uint8_t digits_[4] = {0, 1, 2, 3};
unsigned long startCountMillis;
unsigned long currentCountMillis;
const unsigned long countdownPeriod = 4000;  //the value is a number of milliseconds
int displayCycleCounter = 0;
int prevCounter = 0;



void setup() {
  Serial.begin(115200);
  while (!Serial) {
    // trying to connect to serial...
  }

  //--- Buttons initialization ---//
  //  PORTC.PIN2CTRL = 0b00001001;
  pinMode(14, INPUT_PULLUP);

  //--- Display initialization ---//
  setupDisplay();
  turnOffDisplay();

  delay(1000);

  btnWatchTimerStart = millis();
  startMicros = micros();
  startCountMillis = millis();
}


bool printHeader;
bool rtcAvailabilityChecked;
bool rtcReadabilityChecked;
bool btnConnChecked;
bool displayChecked;

void loop() {

  /* ----  1. Check rtc clock. ---- */
  if (!printHeader) {
    Serial.println("CHECKING RTC's CONDITION:");
    Serial.println("--------------------------");
    printHeader = true;
    delay(3000);
  }

  if (!rtcAvailabilityChecked) {

    while (!rtc.begin()) {
      if (checking_rtc_counter < 20) {
        checking_rtc_counter += 1;
      } else {
        checking_rtc_counter = 0;
        break;
      }
    }
    if (!rtc.begin()) {
      rtcAvailable = false;
      Serial.println("RTC_ADDR_FOUND:0");
      //    Serial.println(); rtc addr
    } else {
      rtcAvailable = true;
      Serial.println("RTC_ADDR_FOUND:1");
    }

    rtcAvailabilityChecked = true;
    printHeader = false;
  }


  /* ---- 2. Check curr. factory default time ---- */
  if (!printHeader) {
    Serial.println("\nCHECKING RTC's AVAILABILITY:");
    Serial.println("----------------------------");
    printHeader = true;
    delay(3000);
  }

  if (rtcAvailabilityChecked && !rtcReadabilityChecked) {

    if (rtcAvailable) {
      if (rtc.updateTime()) {
        rtcReadable = true;
        Serial.println("RTC_READABLE:1");

        delay(100);

        rtc.updateTimeArray();

        delay(100);

        // Send string date & time
        Serial.println(String(rtc.stringTime()));
        Serial.println(String(rtc.stringDate()));
      } else {
        rtcReadable = false;
        Serial.println("RTC_READABLE:0");
      }
    }

    rtcReadabilityChecked = true;
    printHeader = false;
  }


  /* ---- 3. check button for two presses ---- */
  if (!printHeader) {
    Serial.println("\nCHECKING BUTTON's CONDITION:");
    Serial.println("----------------------------");
    printHeader = true;
    delay(3000);
  }

  if (rtcReadabilityChecked && !btnConnChecked) {

    btnWatchTimerCurr = millis();

    if ( btnWatchTimerCurr - btnWatchTimerStart >= 1000) {

      btnWatcherFlag++;
      SW_OneState = digitalRead(14);

      Serial.print("SW_STATE:");
      Serial.print(SW_OneState);
      Serial.print(" [");
      Serial.print(10 - btnWatcherFlag);
      Serial.println(" sec left]");

      if (btnWatcherFlag >= watchBtnFor) {
        btnConnChecked = true;
        btnWatcherFlag = 0;
        printHeader = false;
      }

      btnWatchTimerStart = btnWatchTimerCurr;
    }
  }


  /* ---- 4. check display routine ---- */
  if (!printHeader) {
    Serial.println("\nCHECKING DISPLAY's CONDITION:");
    Serial.println("-----------------------------");
    delay(3000);
    Serial.println("Watch if you see the below pattern \nin the segments\n(Starting in 10 sec):");
    Serial.println("0, 1");
    Serial.println("2, 3");
    Serial.println("delay(4000);");
    Serial.println("4, 5");
    Serial.println("6, 7");
    Serial.println("delay(4000);");
    Serial.println("8, 9");
    Serial.println("0, 0");
    Serial.println("delay(4000);");
    Serial.println("Finally display will Turn OFF");
    printHeader = true;
    delay(10000);
    //    Serial.println(displayCycleCounter);
  }

  if (btnConnChecked && !displayChecked) {

    showOnDisplay(digits_);

    if (displayCycleCounter != prevCounter) {

      //      Serial.println(displayCycleCounter);

      if (displayCycleCounter > 1 && displayCycleCounter <= 3) {
        for (int i = 0; i < sizeof(digits_); i++) {
          digits_[i] += 4;
          if (digits_[i] > 9) digits_[i] = 0;
        }
      }

      if (displayCycleCounter > 3) {
        turnOffDisplay();
        displayCycleCounter = 0;
        prevCounter = 0;
        displayChecked = true;

        Serial.println("\nReport DISPLAY OKAY by pressing '1'");
        Serial.println("Report DISPLAY NOT OKAY by pressing '0'");
      }

      prevCounter = displayCycleCounter;
    }


    currentCountMillis = millis();
    if (currentCountMillis - startCountMillis >= countdownPeriod) {
      displayCycleCounter++;
      startCountMillis = currentCountMillis;
    }
  }
}






void setupDisplay() {
  //  Cathode Pin for last dot of the LED segment (used for battery low warning)
  PORTC.DIRSET = PIN5_bm; // use PC5 as an output
  PORTC.OUTCLR = PIN5_bm; // turn PC5 output off

  //  Cathode Pins for LEDS segments
  PORTA.DIRSET = 0b11111110; // [ PA 1-7 as Outputs]
  //  Anode Pins for LEDS
  PORTB.DIRSET = 0b11110000;
}


unsigned char num_array[11] = {
  //GFEDCBA0
  0b01111110, //0
  0b00001100, //1
  0b10110110, //2
  0b10011110, //3
  0b11001100, //4
  0b11011010, //5
  0b11111010, //6
  0b00001110, //7
  0b11111110, //8
  0b11011110, //9
  0b00010000  //_
};

int c = 0;

void showOnDisplay(uint8_t * digits) {
  currentMicros = micros();

  if (currentMicros - startMicros >= period) {

    // ---- Clear all leds of a segment ---- //
    PORTA.OUTCLR = 0b11111110;
    // ---- Deactivatec all segments by setting them HIGH (My segments are in common cathode config) ---- //
    PORTB.OUTSET = 0b11110000;

    // ---- Activate one segment at a time ---- //
    cli(); // Interrupts off so an interrupt can't fire in the middle of these two.
    byte mask = (0b00010001 << c) & 0xF0;   // We need to toggle 2 bits in PORTB.OUT to increment the digit right
    PORTB.OUTTGL = mask;                    // Toggle those bits
    // Immediately do a direct write of the PORTA value using the (fast) VPORT register
    byte this_digit = num_array[digits[c]];
    VPORTA.OUT = this_digit;
    sei(); // Interrupts back on

    c++;
    if (c > 3) c = 0;

    startMicros = currentMicros;
  }
}

void turnOffDisplay() {
  PORTC.OUTCLR = PIN5_bm; // toggle PC5 OFF

  // ---- Clear all leds of a segment ---- //
  PORTA.OUTCLR = 0b11111110;
  // ---- Deactivatec all segments by setting them HIGH (My segments are in common Anode config) ---- //
  PORTB.OUTSET = 0b11110000;
}


void batteryWarningLED_ON() {
  PORTC.OUTCLR = PIN5_bm;
  PORTB.OUTSET = 0b11110000;

  cli();
  PORTB.OUTTGL = 0b00010000;
  VPORTC.OUT = PIN5_bm;
  sei();
}

void batteryWarningLED_OFF() {
  cli();
  // Turn PC5 (Battery warning LED dot) output off
  PORTC.OUTCLR = PIN5_bm;
  sei();
}
