boolean SW_OneState;
boolean showCurrTimePressed;
volatile byte wakeUpTriggered;
volatile byte showTimePeriodOver;



//--- ISR for waking up from sleep mode ---//
ISR(PORTC_PORT_vect) {
  byte flags = PORTC.INTFLAGS;
  PORTC.INTFLAGS = flags; //clear flags

  wakeUpTriggered = 1;
}



//--- uC's internal RTC & interrupt based based delay method ---//
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
}


void watchButtons() {
  if (wakeUpTriggered == 1) {
    wakeUpTriggered = 0;
    // -- ** Debug line remove later ** -- //
    //    Serial.println(F("Interrupt-1 fired. Awaking device & reading Pins.."));

    // Read PC2
    SW_OneState = PORTC.IN & PIN2_bm;

    if (SW_OneState) {
      showCurrTimePressed = false;
    } else {
      showCurrTimePressed = true;
    }
  }
}
