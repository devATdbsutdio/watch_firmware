/*
Description: For checking inetrnal IOs can be set properly and the registers are working
Author: Saurabh datta.
Time: Oct, 2021.
Loca: Beijing, China.
License: MIT
*/


unsigned long pinTestModeStartMillis;
unsigned long pinTestModeCurrMillis;
const unsigned long testModeProgressPeriod = 100;
boolean PORTA_checked;
boolean PORTB_checked;
boolean PORTC_checked;
bool pinExaminationFinished;
bool pinStatus;

//-- Helper function **
void printBinary(byte b) {
  for (int i = 7; i >= 0; i-- )
  {
    Serial.print((b >> i) & 0X01);//shift and select first bit
  }
  Serial.println();
}

int pac = 1;
int pbc = 0;
int pcc = 0;

int portAStatus[7]; // PA1-PC7
int portBStatus[8]; // PB0-PB7
int portCStatus[6]; // PC0-PC5

/*
    portAStatus[0] corresponds to PA1's state
    portAStatus[1] corresponds to PA2's state
    portAStatus[2] corresponds to PA3's state
    portAStatus[3] corresponds to PA4's state
    portAStatus[4] corresponds to PA5's state
    portAStatus[5] corresponds to PA6's state
    portAStatus[6] corresponds to PA7's state

    portBStatus[0] corresponds to PB0's state
    portBStatus[1] corresponds to PB1's state
    portBStatus[2] corresponds to PB2's state - ignore
    portBStatus[3] corresponds to PB3's state - ignore
    portBStatus[4] corresponds to PB4's state
    portBStatus[5] corresponds to PB5's state
    portBStatus[6] corresponds to PB6's state
    portBStatus[7] corresponds to PB7's state

    portCStatus[0] corresponds to PC0's state
    portCStatus[1] corresponds to PC1's state
    portCStatus[2] corresponds to PC2's state
    portCStatus[3] corresponds to PC3's state
    portCStatus[4] corresponds to PC4's state
    portCStatus[5] corresponds to PC5's state
*/

bool firstState;
bool secondState;
boolean isSerialPin;


void examinePins() {
  while (true) {
    pinTestModeCurrMillis = millis();

    if (pinTestModeCurrMillis - pinTestModeStartMillis >= testModeProgressPeriod) {

      //---  For Port A (Before Port B & C) ---//
      if (!PORTA_checked && !PORTB_checked && !PORTC_checked) {
        //      Serial.print("Checking: PA-");
        //      Serial.print(pac);
        //      Serial.print(" : ");

        // byte portAddrMask = (0b00000001 << pac);
        byte portAddrMask = 0b00000001;
        portAddrMask <<= pac;
        // printBinary(portAddrMask);

        // Set as OUTPUT & HIGH
        cli();
        PORTA.DIRSET = portAddrMask;
        PORTA.OUTSET = portAddrMask;
        // Set as INPUT and read STATE
        PORTA.DIRCLR = portAddrMask;
        firstState = PORTA.IN & portAddrMask;
        //      Serial.println(firstState);
        // Set as OUTPUT & LOW
        PORTA.DIRSET = portAddrMask;
        PORTA.OUTCLR = portAddrMask;
        // Set as INPUT and read STATE, again
        PORTA.DIRCLR = portAddrMask;
        secondState = PORTA.IN & portAddrMask;
        // Serial.println(secondState);

        // [DEBUG] [TEST] **Simulation for non-working pins
        if (pac == 4 || pac == 6) {
          secondState = HIGH;
        }
        //------------------------------------------------//
        sei();

        //      Serial.print(String(firstState) + " " + String(secondState));

        //  Mark that pin as "working: 1, if you see the pin has toggled!"
        if (firstState == HIGH && secondState == LOW) portAStatus[pac - 1] = 1;
        else portAStatus[pac - 1] = 0;


        pac++;
        if (pac > sizeof(portAStatus) / 2) {
          pac = 1;
          PORTA_checked = true;
          Serial.println("PORT-A IO check finished!");

          int total_working_pins = 0;

          // ** [BUG] [TBD] - size fo array is 14 , should be 7 ??
          //    Serial.print("Size of array: ");
          //    Serial.println(sizeof(portAStatus));
          // ** Duct tape solution
          for (int i = 0; i < sizeof(portAStatus) / 2; i++) {
            //          Serial.print("PA");
            //          Serial.print(i + 1);
            //          Serial.print(": ");
            //          Serial.println(portAStatus[i]);

            if (portAStatus[i] == 0) {
              Serial.print("[ERR] Internally, PA");
              Serial.print(i + 1);
              Serial.println(" not working!");
            } else {
              total_working_pins += 1;
            }
          }

          if (total_working_pins == sizeof(portAStatus) / 2) {
            Serial.println("[/] In PORT A (PA1-PA7),");
            Serial.println("All pins are working!\n");
          } else {
            Serial.println("[X] In PORT A (PA1-PA7), a total");
            Serial.println("of only " + String(total_working_pins) + " pins are working!\n");
          }
        }
      }

      //---  For Port B (After Port A) ---//
      if (PORTA_checked && !PORTB_checked && !PORTC_checked) {
        byte portAddrMask = 0b00000001; // pbc staring at 4

        // PB2 and PB3 are serial, assuming they are working

        if (pbc == 2 || pbc == 3) {
          isSerialPin = true;
          //  Serial.println("Not bitshifting, it is serial pin!");

          // Just for filling in hardcoded data for later
          firstState = LOW;
          secondState = LOW;
        } else {
          isSerialPin = false;
        }

        if (!isSerialPin) {
          // bit shift and iterate through PORT
          portAddrMask <<= pbc;
          //        printBinary(portAddrMask);

          // Set as OUTPUT & HIGH
          cli();
          PORTB.DIRSET = portAddrMask;
          PORTB.OUTSET = portAddrMask;
          // Set as INPUT and read STATE
          PORTB.DIRCLR = portAddrMask;
          firstState = PORTB.IN & portAddrMask;
          //        Serial.println(firstState);
          // Set as OUTPUT & LOW
          PORTB.DIRSET = portAddrMask;
          PORTB.OUTCLR = portAddrMask;
          // Set as INPUT and read STATE, again!
          PORTB.DIRCLR = portAddrMask;
          secondState = PORTB.IN & portAddrMask;
          //        Serial.println(secondState);

          // [DEBUG] [TEST] **Simulation for non-working pins
          if (pbc == 5) {
            secondState = HIGH;
          }
          //------------------------------------------------//
          sei();
        }


        //  Mark that pin as "working: 1, if you see the pin has toggled!"
        if (firstState == HIGH && secondState == LOW) portBStatus[pbc] = 1;
        else portBStatus[pbc] = 0;


        pbc++;
        if (pbc > sizeof(portBStatus) / 2) {
          pbc = 0;
          PORTB_checked = true;
          Serial.println("PORT-B IO check finished!");

          int total_working_pins = 0;

          for (int i = 0; i < sizeof(portBStatus) / 2; i++) { // ignoring RX TX pin's registered states
            //  Serial.println(i);
            if ((portBStatus[i] == 0) && i != 2 && i != 3) {
              Serial.print("[ERR] Internally, PB");
              Serial.print(i);
              Serial.println(" not working!");
            } else if ((portBStatus[i] == 0) && (i == 2 || i == 3)) {
              //  Serial.println("Ignoring Hardcoded values for Serial Pins!");
            } else {
              total_working_pins += 1;
            }
          }

          if (total_working_pins == sizeof(portBStatus) / 2 - 2) { // ignoring RX TX pins
            Serial.println("[/] In PORT B (PB0-PB1 & PB4-PB7),");
            Serial.println("All pins are working!\n");
          } else {
            Serial.println("[X] In PORT B (PB0-PB1 & PB4-PB7), a total");
            Serial.println("of only " + String(total_working_pins) + " pins are working!\n");
          }
        }
      }

      //  For Port C (After Port A & Port B)
      if (PORTA_checked && PORTB_checked && !PORTC_checked) {

        byte portAddrMask = 0b00000001;
        portAddrMask <<= pcc;
        //      printBinary(portAddrMask);

        // Set as OUTPUT & HIGH
        cli();
        PORTC.DIRSET = portAddrMask;
        PORTC.OUTSET = portAddrMask;
        // Set as INPUT and read STATE
        PORTC.DIRCLR = portAddrMask;
        firstState = PORTC.IN & portAddrMask;
        //      Serial.println(firstState);
        // Set as OUTPUT & LOW
        PORTC.DIRSET = portAddrMask;
        PORTC.OUTCLR = portAddrMask;
        // Set as INPUT and read STATE, again
        PORTC.DIRCLR = portAddrMask;
        secondState = PORTC.IN & portAddrMask;
        //      Serial.println(secondState);

        // [DEBUG] [TEST] **Simulation for non-working pins
        //        if (pcc == 4) {
        //          secondState = HIGH;
        //        }
        //------------------------------------------------//
        sei();


        if (firstState == HIGH && secondState == LOW) {
          //  Means that particular pin state is changing and working
          //  Mark that pin as "working: 1"
          //  Serial.println("Working!");
          portCStatus[pcc] = 1;
        } else {
          //  Mark that pin as "not-working: 0"
          //  Serial.println("Not Working!");
          portCStatus[pcc] = 0;
        }

        pcc++;
        if (pcc > 5) {
          pcc = 0;
          PORTC_checked = true;
          Serial.println("PORT-C IO check finished!");

          int total_working_pins = 0;

          for (int i = 0; i < sizeof(portCStatus) / 2; i++) {
            if (portCStatus[i] == 0) {
              Serial.print("[ERR] Internally, PC");
              Serial.print(i);
              Serial.println(" not working!");
            } else {
              total_working_pins += 1;
            }
          }

          if (total_working_pins == sizeof(portCStatus) / 2) {
            Serial.println("[/] In PORT C (PC0-PC5),");
            Serial.println("All pins are working!\n");
          } else {
            Serial.println("[X] In PORT C (PC0-PC5), a total");
            Serial.println("of only " + String(total_working_pins) + " pins are working!\n");
          }
        }
      }



      if (PORTA_checked && PORTB_checked && PORTC_checked) {
        Serial.println("\nATTINY Digital IO Test finished!\n");
        break;
      }

      pinTestModeStartMillis = pinTestModeCurrMillis;
    }
  }
}
