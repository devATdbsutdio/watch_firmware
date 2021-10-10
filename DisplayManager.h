/*
Description: 7 segmebnt display manager for showing digits
Author: Saurabh datta.
Time: Aug, 2021.
Loca: Beijing, China.
License: MIT
*/


unsigned long startMicros;
unsigned long currentMicros;
const unsigned long period = 10;  // the value is a number of Microseconds



void setupDisplay() {
  //  Cathode Pin for last dot of the LED segment (used for battery low warning)
  PORTC.DIRSET = PIN5_bm; // use PC5 as an output
  PORTC.OUTCLR = PIN5_bm; // turn PC5 output off

  //  Cathode Pins for LEDS segments
  PORTA.DIRSET = 0b11111110; // [ PA 1-7 as Outputs]
  //  Anode Pins for LEDS
  PORTB.DIRSET = 0b11110000;

  //  startMicros = micros();
}


uint8_t blankSignal[4] = { 10, 10, 10, 10 };


/*
  SINGLE 7 SEGEMNT LED LAYOUT
  --A--
  |   |
  F   B
  |   |
  --G--
  |   |
  E   C
  |   |
  --D-- *
*/

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
  0b00010000 //_
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
