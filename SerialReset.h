boolean readyToReceive;
char incomingChar;
int idx = 0;
char dataArray[21];
boolean handshakeReqArrived;
boolean newDataArrived;
int totalDelimators;


int dateToBeSet = 0;
int monthToBeSet = 0;
int yearToBeSet = 0;
int weekdayToBeSet = 0;
int hrToBeSet = 0;
int minToBeSet = 0;
int secToBeSet = 0;

boolean setCustomTime;

void fillDataArray() {
  newDataArrived = false;

  while (Serial.available() > 0) {
    incomingChar = Serial.read();
    //    Serial.print(incomingChar);
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

      // -- ** Debug code remove later ** -- //
      //      Serial.print("HOUR- ");
      //      Serial.println(hrToBeSet);
      //      Serial.print("MIN- ");
      //      Serial.println(minToBeSet);
      //      Serial.print("SEC- ");
      //      Serial.println(secToBeSet);
      //      Serial.print("WEEKDAY ");
      //      Serial.println(weekdayToBeSet);
      //      Serial.print("DATE- ");
      //      Serial.println(dateToBeSet);
      //      Serial.print("MONTH- ");
      //      Serial.println(monthToBeSet);
      //      Serial.print("YEAR- ");
      //      Serial.println(yearToBeSet);

      setCustomTime = true;
    }
  } else {
    setCustomTime = false;
  }
}

void setRTCToNewTime() {
  if (setCustomTime) {
    //    Serial.println(F("Setting custom time..."));
    if (rtc.setTime(secToBeSet, minToBeSet, hrToBeSet, weekdayToBeSet, dateToBeSet, monthToBeSet, yearToBeSet) == false) {
      // -- ** Debug code remove later ** -- //
      //      Serial.println("Something went wrong setting the time");
    }
    // -- ** Debug code remove later ** -- //
    //    Serial.println(F("Custom Time set!"));
    setCustomTime = false;
  }
}



void SetTimeOverSerial() {
  //  //-------- HandShake ---------//
  //  if (Serial.available() > 0 && !readyToReceive) {
  //    incomingChar = Serial.read();
  //    if (incomingChar == '[') { // if received HS req.
  //      // send response
  //      Serial.println("Received hand shake req.");
  //      Serial.println("Sending Response");
  //      Serial.println(']');
  //      // & make sys ready
  //      readyToReceive = true;
  //    } else {
  //      readyToReceive = false;
  //    }
  //  }
  //
  //  if (readyToReceive) {
  //    if (printOnce) {
  //      Serial.println("Sys ready to receive Data");
  //      printOnce = false;
  //    }
  //
  //    fillDataArray();
  //    parseDataArray();
  //  }

  fillDataArray();
  parseDataArray();
  setRTCToNewTime();
}
