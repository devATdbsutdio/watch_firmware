/*
  Description: 7 segment display manager for showing digits
  Author: Saurabh datta.
  Time: Aug, 2021.
  Loc: Beijing, China.
  License: MIT
*/


unsigned long startMicros;
unsigned long currentMicros;
const unsigned long period  = 10;   // the value is a number of Microseconds



uint16_t warning_blink_gap  = 125;  // The warning LED will blink (toggle at 100 ms) for a total time of 1000 ms (1s) total with 5/2=2.5 (approx 2) 2 blinks 
int max_blinks              = 5;    // this will become 5*2 = 10 and then 10*warning_blink_gap = 1000 ms (because the clock is also ticking at 1000 ms)

int do_blink                = 1;
int blink_count             = 0;
bool tog                    = false;

unsigned long startWarningCountMillis;
unsigned long currentWarningCountMillis;


void setupDisplay() {
  //  Cathode Pin for last dot of the LED segment (used for battery low warning)
  PORTC.DIRSET = PIN5_bm;    // Use PC5 as an output
  PORTC.OUTCLR = PIN5_bm;    // Turn PC5 output off

  //  Cathode Pins for LEDS segments
  PORTA.DIRSET = 0b11111110; // [PA 1-7 as Outputs]

  //  Anode Pins for LEDS
  PORTB.DIRSET = 0b11110000; // [PB 4-7 as Outputs]
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
    // ---- Deactivate all segments by setting them HIGH (My segments are in common cathode config) ---- //
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
  cli();
  PORTC.OUTCLR = PIN5_bm;     // Turn PC5 (our LED dot's CATHODE for batter warning LED) off
  PORTA.OUTCLR = 0b11111110;  // turn OFF all the cathode pins of the LED sugemnt by setting them to HIGH bits (in common ANODE config)
  PORTB.OUTSET = 0b11110000;  // Deactivate all on PORTB bits to which the ANODE pins of our LED segment are connected, which are  by setting them HIGH (in common ANODE config)
  sei();
}



void batteryWarningLED_ON() {
  cli();                      // Detach interrupt (to avoid any glitch)
  PORTB.OUTTGL = 0b00010000;  // Toggle the ANODE bit responsible for our LED dot in the matrix
  VPORTC.OUT = PIN5_bm;       // Quickly set the pin PC5 to high with this spl method.
  sei();                      // Attach interrupt again
}



void batteryWarningLED_OFF() {
  cli();                      // Detach interrupt (to avoid any glitch)
  PORTB.OUTSET = 0b11110000;  // Deactivate all on PORTB bits to which the ANODE pins of our LED segment are connected, which are  by setting them HIGH
  PORTC.OUTCLR = PIN5_bm;     // Turn PC5 (our LED dot's CATHODE for batter warning LED) off
  sei();                      // Attach interrupt again
}




// This function show a warning LED for (total ON period)/2 seconds
// and then shows the time...
void low_voltage_warn() {
  // Block code to only blink warning LED
  while (do_blink == 1) {
    currentWarningCountMillis = millis();
    if (currentWarningCountMillis - startWarningCountMillis >= warning_blink_gap) {

      startWarningCountMillis = currentWarningCountMillis;

      blink_count++;

      if (blink_count >= max_blinks) {
        //--- Clear and Stop the blinking LED ---//
        batteryWarningLED_OFF();
        blink_count = 0;
        do_blink = 0;
        break;
      }

      //--- Toggle PC5 (Battery warning LED dot) output on and off mechanism ---//
      byte dot_pin_mask = 0b00010000; // mask to make the cathide bit, responsible for our led dot to toggle
      tog = !tog;
      if (tog) {
        dot_pin_mask = 0b00010000;
      } else {
        dot_pin_mask = 0b00000000;
      }
      
      PORTB.OUTSET = 0b11110000;    // Deactivate all on PORTB bits to which the COMMON CATHODE pins of our LED segment are connected, which are  by setting them HIGH
      cli();                        // Detach interrupt (to avoid any glitch)
      PORTB.OUTTGL = dot_pin_mask;  // Toggle the cathode bit responsible for our LED dot in the matrix
      VPORTC.OUT = PIN5_bm;         // Quickly set the pin to high with this spl method.
      sei();                        // Attach interrupt again
    }
  }
}
