void disableUnusedPins() {
  // set them to input pull and do not keep them floating
  PORTC.PIN0CTRL = PORT_PULLUPEN_bm;
  PORTC.PIN1CTRL = PORT_PULLUPEN_bm;
  PORTC.PIN3CTRL = PORT_PULLUPEN_bm;
  PORTC.PIN4CTRL = PORT_PULLUPEN_bm;
  PORTC.PIN5CTRL = PORT_PULLUPEN_bm;
}

void disableSerialHWPins() {
  //  pinMode(8, OUTPUT); // RX
  //  pinMode(9, INPUT_PULLUP); // TX
  //  digitalWrite(8, LOW);
  //  digitalWrite(9, LOW);

  PORTB.DIRSET = PIN3_bm;
  cli();
  PORTB.OUT &= ~PIN3_bm;
  sei();
}



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

  PORTB.DIRSET = PIN0_bm;
  PORTB.DIRSET = PIN1_bm;
  cli(); // interrupts off
  PORTB.OUT &= ~PIN0_bm;
  PORTB.OUT &= ~PIN1_bm;
  sei(); // Interrupts back on

  //  TWI0.MCTRLA &= ~(TWI_ENABLE_bm);
}
