/*
  Description: Time reset to RTC mechanism using serial data from computer
  Author: Saurabh datta.
  Time: Aug, 2021.
  Loc: Beijing, China.
  License: MIT
*/


boolean readyToReceive;
char incomingChar;
int idx             = 0;
char dataArray[21];
boolean handshakeReqArrived;
boolean newDataArrived;
int totalDelimators;


int dateToBeSet     = 0;
int monthToBeSet    = 0;
int yearToBeSet     = 0;
int weekdayToBeSet  = 0;
int hrToBeSet       = 0;
int minToBeSet      = 0;
int secToBeSet      = 0;


boolean setNewTime;


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

// 02:18:19:6:25:06:2021
// 02:18:19:6:25:06:2021:5
void parseDataArray() {

  if (newDataArrived) {
    newDataArrived = false;
    totalDelimators = 0;

    // Count how many delimators (in our case that is ':' of byte value 10) are there
    for (int i = 0; i < sizeof(dataArray); i++) {
      if (dataArray[i] == ':') {
        totalDelimators++;
      }
    }

    // Check received data's format & integrity
    if (totalDelimators == 6) {
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
      // ---- [DEBUG --- TBD]
      //      strtokIndx = strtok(NULL, ":");
      //      //    data strcture: "...:x" where x is in sec which needs to be converted in milli seconds; hence *1000
      //      int new_un_tested_stayAwakeFor = (atoi(strtokIndx)) * 1000;


      //      if (debug_log) {
      //        Serial.print("TIME: "); Serial.print(hrToBeSet); Serial.print("-"); Serial.print(minToBeSet); Serial.print("-"); Serial.println(secToBeSet);
      //        Serial.print("WEEKDAY: ");
      //        Serial.println(weekdayToBeSet);
      //        Serial.print("DATE: "); Serial.print(dateToBeSet); Serial.print("-"); Serial.print(monthToBeSet); Serial.print("-"); Serial.println(yearToBeSet);
      //     //     Serial.print("MS operational delay: "); Serial.println(new_un_tested_stayAwakeFor);
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



void SetTimeOverSerial() {
  fillDataArray();
  parseDataArray();
  setRTCToNewTime();
}
