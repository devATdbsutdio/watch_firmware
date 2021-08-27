  unsigned long startMicros;
unsigned long currentMicros;
const unsigned long period = 10;  // the value is a number of Microseconds

void setupDisplay() {
  startMicros = micros();

  //  Cathode Pins for LEDS
  PORTA.DIRSET = 0b11111110; // [ PA 1-7 as Outputs]
  //  Anode Pins for LEDS
  //  PORTC.DIRSET = 0b00111100; // [ PC 2-5 (PC-4&5 for top displays and PC-2&3 for bottom display) as Outputs or in Arduino world, digital Pins 14-17 ]
  PORTB.DIRSET = 0b11110000;
}


uint8_t blankSignal[4] = { 10, 10, 10, 10 };

unsigned char num_array[11] = {
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
    // PORTC.OUTSET = 0b00111100 ;
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
  // ---- Clear all leds of a segment ---- //
  PORTA.OUTCLR = 0b11111110;
  // ---- Deactivatec all segments by setting them HIGH (My segments are in common Anode config) ---- //
  // PORTC.OUTSET = 0b00111100 ;
  PORTB.OUTSET = 0b11110000;
}
