# clock_firmware

### Dependant libraries:
1. [RV8803-Tiny](https://github.com/dattasaurabh82/RV8803Tiny) (For My chosen RTC RV8803)
2. [TinyMegaI2C](https://github.com/dattasaurabh82/TinyMegaI2C) (For Less bloated I2C library for new ATTINY Series)

### Schematic:
1. Basic [ **uC + RTC** ]: [Here](https://github.com/dattasaurabh82/clock_firmware_production/files/7063914/uC_RTC_basic_schematic.pdf)
2. _Pro_ [ **uC + RTC + Serial COMM FTDI chip + LiPo charger = All in 1** ]: [Here](https://github.com/dattasaurabh82/clock_firmware_production/files/7063918/uC_RTC_FTDI_LIPO_pro_schematic.pdf)




### PCB details: 
_Coming soon ..._

### Development Environment:
Arduino-IDE, arduino-cli etc.

### Some more details:
1. uC: ATTINY1607-MFR clocked at 5MHz (necessary Freq for my main application later)
2. Operating Voltage: 3.7V from 120mAH LiPo Battery
3. BOD disabled in Fuse (From IDE's tools Menu)
4. Some UART funstionalities are required.
  So some pins are always connected to an FTDI board as such, i.e in _Pro_ version:
  ````
  GND of uC <-> FTDI GND
  [RXD] PB3 <-- FTDI TXD
  [TXD] PB2 --> FTDI RXD
````

Pseudo-code for the firmware written so far:
````
SETUP:
	Begin Serial.
	
	Set up RTC
	while RTC is not available, block. 
	when RTC is available, move to next step. 
	
	Setup pins to OUTPUT for the seven segment displays. (Using PORT manipulation)
	
	Setup pin for the button as INPUT_PULLUP (Using PORT manipulation) 
	
	Set all other unused pins as INPUT_PULLUP. 

	Setup sleep and power-down mode for ATTINY1607
	

LOOP:
	if button is pressed:
		interrupt is triggered:
			uC wakes up FOR 5 SECONDS (interrupt and ATTINY's Internal RTC based time keeping).
			Quarries the RV-8803 RTC to get latest time.
			If RTC doesn’t responds, it shows an error signal in seven segment display.
 			If it responds correctly, the uC then gets the time and shows the it on display. 
			
 			MeanWhile if a serial string is received, parse info and set time on RTC accordingly (if RTC is available). 
			
		after the 5-SECONDS
			Flush serial
			turn off all the LED pins from seven segments
                         Go to power down sleep mode	
````

### Notes for Collaborators:
1. Please check the codebase on "old" branch. I know I have a messy sense of naming branches
2. Please use [ExtraUtils.h](https://github.com/dattasaurabh82/clock_firmware_production/blob/old/ExtraUtils.h) for new Functions Suggestions.
3. Please comment to explain a noob like me, for any change suggestion in other files (For optimization for example ...)

