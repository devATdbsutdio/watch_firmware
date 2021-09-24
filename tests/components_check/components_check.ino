/*
  1. check rtc clock.
  2. check curr factory default time.
  3. check button.
  4. check display.
*/

#include <RV8803Tiny.h>
RV8803Tiny rtc;

int checking_rtc_counter = 0;
bool rtcAvailable;
bool rtcReadable;
boolean checkedFactoryDefTime;

bool watchButtonPeriodOver;
int watchButtonForPeriod = 10000;
bool SW_OneState;
bool old_SW_OneState;

unsigned long startMicros;
unsigned long currentMicros;
const unsigned long period = 10;  // the value is a number of Microseconds



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
  watchButtonPeriodOver = true;
}



void setup() {
  Serial.begin(115200);
  while (!Serial) {
    // trying to connect to serial...
  }


  //--- Buttons initialization ---//
  PORTC.PIN2CTRL = 0b00001001;

  //--- Display initialization ---//
  //  Cathode Pin for last dot of the LED segment (used for battery low warning)
  PORTC.DIRSET = PIN5_bm; // use PC5 as an output
  PORTC.OUTCLR = PIN5_bm; // turn PC5 output off
  //  Cathode Pins for LEDS segments
  PORTA.DIRSET = 0b11111110; // [ PA 1-7 as Outputs]
  //  Anode Pins for LEDS
  PORTB.DIRSET = 0b11110000;

  delay(10000);

  startMicros = micros();
}




void loop() {

  /* ----  1. Check rtc clock. ---- */
  while (!rtc.begin()) {
    if (checking_rtc_counter < 20) {
      checking_rtc_counter += 1;
    } else {
      checking_rtc_counter = 0;
      break;
    }
  }
  if (!rtc.begin()) {
    rtcAvailable = false;
    Serial.println("RTC_ADDR_FOUND:0");
    //    Serial.println(); rtc addr
  } else {
    rtcAvailable = true;
    Serial.println("RTC_ADDR_FOUND:1");
  }

  /* ---- 2. Check curr. factory default time ---- */
  if (rtcAvailable) {
    if (rtc.updateTime()) {
      rtcReadable = true;
      Serial.println("RTC_READABLE:1");

      delay(2000);

      rtc.updateTimeArray();

      delay(1000);

      // Send string date & time
      Serial.println(String(rtc.stringTime()));
      Serial.println(String(rtc.stringDate()));

      checkedFactoryDefTime = true;
    } else {
      rtcReadable = false;
      Serial.println("RTC_READABLE:0");
      checkedFactoryDefTime = false;
    }
  }

  /* ---- 3. check button for two presses ---- */
  watchButtonPeriodOver = false;
  RTC_DELAY_init(watchButtonForPeriod);
  while (!watchButtonPeriodOver) {
    // check button presses
    SW_OneState = PORTC.IN & PIN2_bm;
    if (old_SW_OneState != SW_OneState) {
      Serial.print("SW_STATE:");
      Serial.println(SW_OneState);
      old_SW_OneState = SW_OneState;
    }
  }

  /* ---- 4. check display routine ---- */

}
