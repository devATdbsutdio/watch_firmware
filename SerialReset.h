/*
  Description: Time reset to RTC mechanism using serial data from computer
  Author: Saurabh datta.
  Time: Aug, 2021.
  Loc: Beijing, China.
  License: MIT
*/


boolean readyToReceive;
char incomingChar;
int idx                     = 0;

// 02:18:19:6:25:06:2021    (totalDelimators == 6 and 21 bytes of data)
// 02:18:19:6:25:06:2021:5  (totalDelimators == 7 and 23 bytes of data)
// 02:18:19:6:25:06:2021:5:1(totalDelimators == 8 and 25 bytes of data)

const int sizeOfDataStructure = int(sizeof(char) * 23);
char dataArray[sizeOfDataStructure];

boolean handshakeReqArrived;
boolean newDataArrived;
int totalDelimators;
boolean awakePeriodChanged  = false;

uint8_t dateToBeSet             = 0;
uint8_t monthToBeSet            = 0;
uint8_t yearToBeSet             = 0;
uint8_t weekdayToBeSet          = 0;
uint8_t hrToBeSet               = 0;
uint8_t minToBeSet              = 0;
uint8_t secToBeSet              = 0;

int stayAwakeFor            = 4000;
int new_stayAwakeFor        = 4100;

boolean setNewTime;
//int enableTiltFunc = 0;


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

// 02:18:19:6:25:06:2021    (totalDelimators == 6)
// 02:18:19:6:25:06:2021:5  (totalDelimators == 7)
// 02:18:19:6:25:06:2021:5:1(totalDelimators == 8)
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
    if (totalDelimators >= 6) {  // or 6/7/8 based on the stream ends with year value or with additional delay value or with enable tilt flag
      char * strtokIndx; // this is used by strtok() as an index
      strtokIndx = strtok(dataArray, ":"); // get the first part - the string

      hrToBeSet = atoi(strtokIndx);        // convert this part to an integer
      strtokIndx = strtok(NULL, ":");      // this continues where the previous call left off
      minToBeSet = atoi(strtokIndx);       // convert this part to an integer
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
      new_stayAwakeFor = (atoi(strtokIndx))*1000; // in millis
      //      strtokIndx = strtok(NULL, ":");
      //      enableTiltFunc = atoi(strtokIndx);

      //      if (debug_log) {
      //        Serial.print("TIME: "); Serial.print(hrToBeSet); Serial.print("-"); Serial.print(minToBeSet); Serial.print("-"); Serial.println(secToBeSet);
      //        Serial.print("WEEKDAY: ");
      //        Serial.println(weekdayToBeSet);
      //        Serial.print("DATE: "); Serial.print(dateToBeSet); Serial.print("-"); Serial.print(monthToBeSet); Serial.print("-"); Serial.println(yearToBeSet);
      //        Serial.print("MS operational delay: "); Serial.println(new_stayAwakeFor);
      //        Serial.print("Tilt SW enable Flag"); Serial.println(enableTiltFunc);
      //      }

      setNewTime = true;
    }
  } else {
    setNewTime = false;
  }
}



void setRTCToNewTime() {
  if (setNewTime) {
    //    if (debug_log) Serial.println(F("Setting custom time..."));

    if (rtc.setTime(secToBeSet, minToBeSet, hrToBeSet, weekdayToBeSet, dateToBeSet, monthToBeSet, yearToBeSet) == false) {
      //      if (debug_log) Serial.println("Something went wrong setting the time");
    }

    //    if (debug_log) Serial.println(F("Custom Time set!"));
    setNewTime = false;
  }
}


void testParser() {
}

void SetTimeOverSerial() {
  fillDataArray();
  parseDataArray();
  setRTCToNewTime();
}
