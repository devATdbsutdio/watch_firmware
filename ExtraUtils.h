void disableUnusedPins() {
  // set them to input pull and do not keep them floating
  PORTC.PIN0CTRL = PORT_PULLUPEN_bm;
  PORTC.PIN1CTRL = PORT_PULLUPEN_bm;
  PORTC.PIN3CTRL = PORT_PULLUPEN_bm;
  PORTC.PIN4CTRL = PORT_PULLUPEN_bm;
  PORTC.PIN5CTRL = PORT_PULLUPEN_bm;
}

void disableSerial() {
  //  Note for Dr.Azzy: In our brief discussion, Did you mean just set the RX TX pins set LOW or something else?

  //  For ATMega328P:
  //  UCSR0B &= ~bit (RXEN0);  // disable receiver
  //  UCSR0B &= ~bit (TXEN0);  // disable transmitter

  //  For ATTINY1607:
  //  USART0.CTRLB &= ~bit (USART0_RXC); // How to ??
  //  USART0.CTRLB &= ~bit (USART0_TXC); // How to ??
}

void enableSerial() {
  //  For ATMega328P:
  //  UCSR0B |= bit (RXEN0);  // disable receiver
  //  UCSR0B |= bit (TXEN0);  // disable transmitter

  //  For ATTINY1607:
  //  USART0.CTRLB |= bit (USART0_RXC); // How to ??
  //  USART0.CTRLB |= bit (USART0_TXC); // How to ??
}
