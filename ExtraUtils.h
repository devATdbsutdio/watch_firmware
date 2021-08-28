void disableUnusedPins() {
  // set them to input pull and do not keep them floating
  PORTC.PIN0CTRL = PORT_PULLUPEN_bm;
  PORTC.PIN1CTRL = PORT_PULLUPEN_bm;
  PORTC.PIN3CTRL = PORT_PULLUPEN_bm;
  PORTC.PIN4CTRL = PORT_PULLUPEN_bm;
  PORTC.PIN5CTRL = PORT_PULLUPEN_bm;
}

void disableSerial() {
  //  USART0.CTRLB &= ~(USART_RXEN_bm);
  //  USART0.CTRLB &= ~(USART_TXEN_bm);
  // Also set TX pin to INPUT_PULLUP (Do we need to?)
  PORTB.PIN2CTRL = PORT_PULLUPEN_bm; //i.e TX_PIN PB2 to INPUT_PULLUP
  // Also set RX pin to INPUT_PULLUP (Do we need to?)
  //  PORTB.PIN3CTRL = PORT_PULLUPEN_bm; //i.e RX_PIN PB3 to INPUT_PULLUP (By default it is set to OUTPUT)
  // --------------------- //
  // Test curr consumption by setting RX TX to output instead of inputpullup
  //  PORTB.DIRSET = PIN2_bm;
  //  PORTB.DIRSET = PIN3_bm;
}

void enableSerial() {
  USART0.CTRLB |= (USART_RXEN_bm); // enable receiver
  USART0.CTRLB |= (USART_TXEN_bm); // enable Transmitter
  // Also set TX pin to INPUT_PULLUP (Do we need to?)
  PORTB.PIN2CTRL = PORT_PULLUPEN_bm; //i.e TX_PIN PB2 to INPUT_PULLUP (By default it is)
  // Also set RX pin to OUTPUT (Do we need to?)
  PORTB.DIRSET = PIN3_bm;
}
