#include <RV8803Tiny.h>
RV8803Tiny rtc;

bool rtcAvailable;
bool rtcReadable;

unsigned long startCountMillis;
unsigned long currentCountMillis;
const unsigned long secPeriod = 1000;  //the value is a number of milliseconds


//void enableTWI() {
//  //  on 1607, set the SDA SCL Pins to input pull-up
//  PORTB.PIN1CTRL |= PORT_PULLUPEN_bm;
//  PORTB.PIN0CTRL |= PORT_PULLUPEN_bm;
//  PORTB.OUTCLR = 0x03; //bits 1 and 0.
//
//  TWI0.MCTRLA = TWI_ENABLE_bm; // Enable as master, no interrupts
//}

void disableTWI() {
  //  on 1607, set the SDA SCL Pins to Output and LOW
  //  pinMode(11, OUTPUT);
  //  pinMode(10, OUTPUT);
  //  digitalWrite(11, LOW);
  //  digitalWrite(10, LOW);

  //  TWI0.MCTRLA &= ~(TWI_ENABLE_bm);

  PORTB.DIRSET = PIN0_bm;
  PORTB.DIRSET = PIN1_bm;
  cli(); // interrupts off
  PORTB.OUT &= ~PIN0_bm;
  PORTB.OUT &= ~PIN1_bm;
  sei(); // Interrupts back on
}

void setupRTC() {
  while (rtc.begin() == false) { // un-shifted default address for RV-8803 is 0x32. Check library!
    rtcAvailable = false;
  }
  rtcAvailable = true;

  //  startCountMillis = millis();
}
