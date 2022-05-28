/*
Description: The operator shows on display, nums, 0-9, thus relying on user to commit if the display is working okay or not
Author: Saurabh datta.
Time: Oct, 2021.
Loca: Beijing, China.
License: MIT
*/

unsigned long startDisplayFPSMicros;
unsigned long currDisplayFPSMicros;
const unsigned long period = 10;  // the value is a number of Microseconds

uint8_t digits_[4] = {0, 1, 2, 3};
unsigned long startCountMillis;
unsigned long currentCountMillis;
const unsigned long countdownPeriod = 4000;  //the value is a number of milliseconds
int displayCycleCounter = 0;
int prevCounter = 0;

bool displayChecked;

bool showdisplayobservinginstructions;

void printDisplayObservingInstructions() {
  if (!showdisplayobservinginstructions) {
    Serial.println("Watch if you see the below pattern");
    Serial.println("in the segments");
    Serial.println("(Starting in 5 sec):");
    Serial.println("0, 1, 2, 3");
    Serial.println("delay(4000);");
    Serial.println("4, 5, 6, 7");
    Serial.println("delay(4000);");
    Serial.println("8, 9, 0, 0");
    Serial.println("delay(4000);");
    Serial.println("Finally display will Turn OFF");

    delay(5000);

    showdisplayobservinginstructions = true;
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


void turnOffDisplay() {
  PORTC.OUTCLR = PIN5_bm; // toggle PC5 OFF

  // ---- Clear all leds of a segment ---- //
  PORTA.OUTCLR = 0b11111110;
  // ---- Deactivatec all segments by setting them HIGH (My segments are in common Anode config) ---- //
  PORTB.OUTSET = 0b11110000;
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
  currDisplayFPSMicros = micros();

  if (currDisplayFPSMicros - startDisplayFPSMicros >= period) {

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

    startDisplayFPSMicros = currDisplayFPSMicros;
  }
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



bool displayPinsInitialized;
void initSegmentDisplay() {
  if (!displayPinsInitialized) {
    setupDisplay();
    turnOffDisplay();

    displayPinsInitialized = true;
  }
}

void operateDisplay() {
  while (true) {
    showOnDisplay(digits_);

    if (displayCycleCounter != prevCounter) {
      //  Serial.println(displayCycleCounter);
      if (displayCycleCounter > 1 && displayCycleCounter <= 3) {
        for (int i = 0; i < int(sizeof(digits_)); i++) {
          digits_[i] += 4;
          if (digits_[i] > 9) digits_[i] = 0;
        }
      }

      if (displayCycleCounter > 3) {
        turnOffDisplay();
        displayCycleCounter = 0;
        prevCounter = 0;

        break;
        //--
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
