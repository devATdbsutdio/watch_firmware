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
	Disable serial related HW pins and registers
	Disable i2C realted Pins and registers

	Setup pins to OUTPUT for the seven segment displays. (Using PORT manipulation)
	Setup pin for the button as INPUT_PULLUP (Using PORT manipulation) 
	
	Set all other unused pins as INPUT_PULLUP. 

	start timers (we are going to avoid delay() ofc)

	Setup sleep and power-down mode for ATTINY1607
	

LOOP:
	if button is pressed:
		interrupt is triggered -> uC wakes up:
			
			1. Begin Serial

			2. Set up RTC (built in i2c in libray)
			    while RTC is not available, block. 
			
			3. Measure battery volate
                           if low:
                               Show low voltage warning [for 5 sec]
                           else:
                               Quarries the RV-8803 RTC to get latest time.
                               if RTC doesn’t responds:
                                   it shows an error signal in seven segment display.
                               else: 
                                   The uC then gets the time and shows the it on display. [for 5 sec]
			
 			MeanWhile, in parallel, if a serial string is received:
 			    Parse info:
 			        if in right format:
 			            Set time on RTC accordingly (if RTC is available).
			
		after the 5-SECONDS
                  Flush serial
                  Kill Serial
                  Put RX line to output and LOW
                  Disable i2c pins (set them to LOW)
                  turn off all the LED pins from seven segments
      Go to power down sleep mode	
````

### Notes for Collaborators:
1. Please use [ExtraUtils.h](https://github.com/dattasaurabh82/clock_firmware_production/blob/old/ExtraUtils.h) for new Functions Suggestions.
2. Please comment to explain, for any change suggestion in other files (For optimization for example ...)

