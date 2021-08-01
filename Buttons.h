
bool SW_OneState;
bool enableSerialTransport;

bool SW_TwoState;
bool showCurrTimePressed;
volatile byte wakeUpTriggered;
volatile byte showTimePeriodOver;


void disableUnusedButtons() {
  // set them to input pull and do not keep them floating
  PORTC.PIN0CTRL = PORT_PULLUPEN_bm;
  PORTC.PIN1CTRL = PORT_PULLUPEN_bm;
  PORTC.PIN3CTRL = PORT_PULLUPEN_bm;
  PORTC.PIN4CTRL = PORT_PULLUPEN_bm;
  PORTC.PIN5CTRL = PORT_PULLUPEN_bm;
}



//--- ISR for waking up from sleep mode ---//
ISR(PORTC_PORT_vect) {
  byte flags = PORTC.INTFLAGS;
  PORTC.INTFLAGS = flags; //clear flags

  wakeUpTriggered = 1;
}



//--- uC's internal RTC based interrupt based delay method ---//
void RTC_DELAY_init(int RTCdelay) {
  RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;    // 32.768kHz Internal Crystal Oscillator (INT32K)

  while (RTC.STATUS > 0);               // Wait for all register to be synchronized
  RTC.PER = RTCdelay;                   // Set period for delay
  RTC.INTCTRL |= RTC_OVF_bm;            // Enable overflow Interrupt which will trigger ISR
  RTC.CTRLA = RTC_PRESCALER_DIV32_gc    // 32768 / 32 = 1024 (sec) ~ 1 ms
              | RTC_RTCEN_bm                        // Enable: enabled
              | RTC_RUNSTDBY_bm;                    // Run In Standby: enabled
}

ISR(RTC_CNT_vect) {
  RTC.INTFLAGS = RTC_OVF_bm;            // Clear flag by writing '1':
  showTimePeriodOver = 1;
}



void setupButtons() {
  //--- Buttons initialization ---//
  PORTC.PIN2CTRL = 0b00001001; // in INPUT pullup mode - will trigger an async ISR // for wakeup from sleep

  //--- Disable unused pins (i.e do not keep them floating) | For efficient low power in sleep mode ---//
  disableUnusedButtons();
}


void watchButtons() {
  if (wakeUpTriggered == 1) {
    wakeUpTriggered = 0;
    //    Serial.println(F("Interrupt-1 fired. Awaking device & reading Pins.."));

    // Read PC2
    SW_OneState = PORTC.IN & PIN2_bm;

    if (SW_OneState) {
      showCurrTimePressed = false;
      enableSerialTransport = true;
    } else {
      showCurrTimePressed = true;
      enableSerialTransport = false;
    }
  }
}
