void disableUnusedPins() {
  // set them to input pull and do not keep them floating
  PORTC.PIN0CTRL = PORT_PULLUPEN_bm;
  PORTC.PIN1CTRL = PORT_PULLUPEN_bm;
  PORTC.PIN3CTRL = PORT_PULLUPEN_bm;
  PORTC.PIN4CTRL = PORT_PULLUPEN_bm;
  PORTC.PIN5CTRL = PORT_PULLUPEN_bm;
}

void disableSerialHWPins() {
  pinMode(8, OUTPUT); // RX
  //  pinMode(9, INPUT_PULLUP); // TX
  digitalWrite(8, LOW);
  //  digitalWrite(9, LOW);
}
