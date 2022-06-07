/*
  Description: Time reset to RTC mechanism using serial data from computer
  Dependencies:
    1. RV8803Tiny   (latest, Ver: 0.0.8)
    2. TinyMegaI2C  (latest, Ver: 0.0.4)
  Author: Saurabh datta.
  Time: Aug, 2021.
  Loc: Beijing, China.
  License: MIT
*/

#include <EEPROM.h>

//-- Addr to which delay value will be written for next cycle --//
/*
  This is to help remember what was the last "stay awake" period, 
  if in any case, the main power source to the uC is lost.
*/
const int eeprom_addr = 1;

bool readyToReceive;
char incomingChar;
int idx = 0;


// This is out data structure pattern, example = 02:18:19:6:25:06:2021:5  (totalDelimators == 7 and 23 bytes of data)
const int sizeOfDataStructure = int(sizeof(char) * 23);
char dataArray[sizeOfDataStructure];

bool handshakeReqArrived;
bool newDataArrived;
int totalDelimators;
bool awakePeriodChanged  = false;

uint8_t dateToBeSet         = 0;
uint8_t monthToBeSet        = 0;
uint8_t yearToBeSet         = 0;
uint8_t weekdayToBeSet      = 0;
uint8_t hrToBeSet           = 0;
uint8_t minToBeSet          = 0;
uint8_t secToBeSet          = 0;

int stayAwakeFor            = 5000;
int new_stayAwakeFor        = 5100;

bool setNewTime;


void fillDataArray() {
  newDataArrived = false;
  while (Serial.available() > 0) {
    incomingChar = Serial.read();
    if (incomingChar == '\n') {
      newDataArrived = true;
      idx = 0;
    } else {
      newDataArrived = false;
      dataArray[idx] = incomingChar;
      idx++;
    }
  }
}


void parseDataArray() {
  if (newDataArrived) {
    newDataArrived = false;
    totalDelimators = 0;

    // Count how many delimators (in our case that is ':') are there
    for (int i = 0; i < sizeOfDataStructure; i++) {
      if (dataArray[i] == ':') {
        totalDelimators++;
      }
    }

    // Check received data's format & integrity
    if (totalDelimators >= 6) {  /* or 6/7/8 based on the stream ends with year value or with additional delay value or with enable tilt flag */
      char * strtokIndx;                      // This is used by strtok() as an index.
      strtokIndx = strtok(dataArray, ":");    // Get the first part - the string.

      hrToBeSet = atoi(strtokIndx);           // Convert this part to an integer.
      strtokIndx = strtok(NULL, ":");         // This continues where the previous call left off.
      minToBeSet = atoi(strtokIndx);          // Convert this part to an integer.
      strtokIndx = strtok(NULL, ":");
      secToBeSet = atoi(strtokIndx);
      strtokIndx = strtok(NULL, ":");
      weekdayToBeSet = atoi(strtokIndx);
      strtokIndx = strtok(NULL, ":");
      dateToBeSet = atoi(strtokIndx);
      strtokIndx = strtok(NULL, ":");
      monthToBeSet = atoi(strtokIndx);
      strtokIndx = strtok(NULL, ":");
      yearToBeSet = atoi(strtokIndx);
      strtokIndx = strtok(NULL, ":");
      int new_val = (atoi(strtokIndx)) * 1000; // This is watch's new "stay awakwe" period. It needs to be converted to millis.
      
      // Also write this "stay awakwe" period's value at a specific location in EEPROM
      EEPROM.put(eeprom_addr, new_val);
      new_stayAwakeFor = new_val;
      setNewTime = true;
    }
  }
  else {
    setNewTime = false;
  }
}



void setRTCToNewTime() {
  if (setNewTime) {
    if (rtc.setTime(secToBeSet, minToBeSet, hrToBeSet, weekdayToBeSet, dateToBeSet, monthToBeSet, yearToBeSet) == false) {
      //#ifdef DEBUG
      //      Serial.println("Something went wrong setting the time");
      //      if (debug_log) Serial.println("Something went wrong setting the time");
      //#endif
    }
    setNewTime = false;
  }
}


void SetTimeOverSerial() {
  fillDataArray();
  parseDataArray();
  setRTCToNewTime();
}
