/*
  Description:
    Button press mechanism for waking up uC and handling multiple btn presses while it is awake.
    (Example: When awake (after 1st btn press), on pressing the button again, it can show date)
  Dependencies:
    1. RV8803Tiny   (latest, Ver: 0.0.8)
    2. TinyMegaI2C  (latest, Ver: 0.0.4)
  Author: Saurabh datta.
  Time: Aug, 2021.
  Loc: Beijing, China.
  License: MIT
*/

boolean BtnState;                       // Var used to check btn state upon wake-up

boolean wakeUp;                         // Var used in the main loop
volatile byte wakeUpTriggered;          // Var used in BTN ISR
volatile byte awakeTimePeriodOver;      // Var used in RTC ISR

/*------ Awake mode btn behavior -------*/
boolean awakeModeBtnReadingPrev;
boolean awakeModeBtnState;
int awakeModePressCounter = 0;

/*---- Debounce handling variables -----*/
unsigned long lastDebounceTime = 0;     // The last time the btn was toggled
unsigned long debounceDelay = 25;       // The debounce time; increase if the input changes quickly





/*------------- ISR for waking up from sleep mode --------------*/
ISR(PORTC_PORT_vect) {
  byte flags = VPORTC.INTFLAGS; // faster (TEST TBD)
  PORTC.INTFLAGS = flags; //clear flags
  wakeUpTriggered = 1;
}


/*--- uC's internal RTC & interrupt based based delay method ---*/
void RTC_DELAY_init(int RTCdelay) {
  RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;    // 32.768kHz Internal Crystal Oscillator (INT32K)
  while (RTC.STATUS > 0);               // Wait for all register to be synchronized
  RTC.PER = RTCdelay;                   // Set period for delay
  RTC.INTCTRL |= RTC_OVF_bm;            // Enable overflow Interrupt which will trigger ISR
  RTC.CTRLA = RTC_PRESCALER_DIV32_gc    // 32768 / 32 = 1024 (sec) ~ 1 ms
              | RTC_RTCEN_bm            // Enable: enabled
              | RTC_RUNSTDBY_bm;        // Run In Standby: enabled
}

ISR(RTC_CNT_vect) {
  RTC.INTFLAGS = RTC_OVF_bm;            // Clear flag by writing '1'
  awakeTimePeriodOver = 1;
}



/*------------------- Buttons initialization -------------------*/
void setupButton() {
  /*
      In INPUT pullup mode - it will trigger an async ISR,
      essential for waking up the uC from sleep.
  */
  PORTC.PIN2CTRL = 0b00001001;

  // Note: Alt approaches could be...
  /*
    Other ISR routines are:
    BOTHEDGES
    RISING
    FALLING
    LEVEL
  */
  // PORTC.PIN2CTRL = PORT_PULLUPEN_bm | PORT_ISC_LEVEL_gc;
}


void watchButtonForWakeup() {
  if (wakeUpTriggered == 1) {
    wakeUpTriggered = 0;

    // Read PC2 (By "input pullup" method)
    BtnState = PORTC.IN & PIN2_bm;

    /*
      As we are using "input pull" method for getting the pin state of the pin to which our button is attached,
      the read state is actually LOW when the btn is pressed (as contrary to what we might think),
      So we want to set the BOOL VAR, responsible for asking to show the time, as opposite of the btn's current state
    */
    wakeUp = !BtnState;
  }
}


void watchButtonForConseqPresses() {
  /*
   * 
  */
  boolean awakeModeBtnReading = PORTC.IN & PIN2_bm;
  
  /*
    As we are using "input pull" method for getting the pin state of the pin to which our button is attached,
    the read state is actually LOW when the btn is pressed (as contrary to what we might think),
    So we want to set the BOOL VAR, responsible for asking to show the time, as opposite of the btn's current state
  */
  awakeModeBtnReading = !awakeModeBtnReading;

  if (awakeModeBtnReading != awakeModeBtnReadingPrev) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (awakeModeBtnReading != awakeModeBtnState) {
      awakeModeBtnState = awakeModeBtnReading;

      if (awakeModeBtnState) {
        awakeModePressCounter++;
        /*
          awakeModePressCounter > 2 => (awakeModePressCounter = 1)
          awakeModePressCounter = 1 => show time
          awakeModePressCounter = 2 => show date
        */
        if (awakeModePressCounter > 2) {
          awakeModePressCounter = 1;
        }

        if (awakeModePressCounter == 1) {
          //          Serial.println("Show Time");
          showCurrDate = false;
          showCurrTime = true;
          // Note: if it is already showing time, don't show time again.
        }
        if (awakeModePressCounter == 2) {
          //          Serial.println("Show Date");
          showCurrTime = false;
          showCurrDate = true;
        }
      }
    }
  }
  awakeModeBtnReadingPrev = awakeModeBtnReading;
}
