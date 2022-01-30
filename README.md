# watch_firmware

### Dependant libraries:
1. [RV8803-Tiny](https://github.com/dattasaurabh82/RV8803Tiny) (Lib for helper class and functions for RV-8803 Real Time Clock)
2. [TinyMegaI2C](https://github.com/dattasaurabh82/TinyMegaI2C) (A less bloated I2C library for new ATTINY Series to communicate with RTC - forked version)

### Schematic:
1. Basic [ **uC + RTC** ]: [Here](https://github.com/dattasaurabh82/clock_firmware_production/files/7063914/uC_RTC_basic_schematic.pdf)
2. _Pro_ [ **uC + RTC + Serial COMM FTDI chip + LiPo charger = All in 1** ]: [Here](https://github.com/dattasaurabh82/clock_firmware_production/files/7063918/uC_RTC_FTDI_LIPO_pro_schematic.pdf)

### Development Environment:
Arduino-IDE, arduino-cli etc.

__Note:__ Compiled binaries (build/[filename].hex) can alos be uploaded using pyupdi. Instructions below. 

### Some more details:
1. uC: ATTINY1607-MFR clocked at 5MHz (necessary Freq for my main application later)
2. Operating Voltage: 3.7V from 120mAH LiPo Battery
3. BOD disabled in Fuse (From IDE's tools Menu)
4. Some UART funstionalities are required.

__So some pins are always connected to an FTDI board as such, i.e in _Pro_ version:__
```
  GND of uC <-> FTDI GND
  [RXD] PB3 <-- FTDI TXD
  [TXD] PB2 --> FTDI RXD
```

Pseudo-code for the firmware written so far:
<p>
	
```
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
                           if somewhat low but still safe volatge to operate:
                               Show low voltage warning [for 1 sec] (as reminder to charge)
			       Then show time
                           else if voltage is very low for the batteries health:
			       Show a dor as "Non-Operational" indicator (Warning : charge now)
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
```
</p>

### Arduino IDE setup and steps:

---
Install the above mentioned "Dependent libraries", using Ardunio IDE's built in Library Manager. 

Once Installed, open the sketch and set the below settings from the __Tools__ menu. 

__Options:__
```
chip=1607
clock=5internal
bodvoltage=1v8
bodmode=disabled
eesave=enable
millis=enabled
resetpin=UPDI
startuptime=0
wiremode=mors
printf=minimal
attach=allenabled
```

And then simply click __compile__. 

To upload select a suitable programmer from the list. My setup uses the programmer:
``` 
SerialUPDI - 230400 baud, 2.7V+ (Mac/Linux: usually CH340 only, Win: most adapters)), select your port or upload (ofc don't forget to select the port)
```

### Build And upload using arduino-cli 
---
[Please read here](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Arduino-cli.md)

### Upload using pyupdi:
Make sure that the binaries are compiled, either using arduino-cli, like from the previous step. 

Enter the build directory where the .hex file is. 

Issue the following command: 



### Notes for Collaborators:
1. Please use [ExtraUtils.h](https://github.com/dattasaurabh82/clock_firmware_production/blob/old/ExtraUtils.h) for new Functions Suggestions.
2. Please comment to explain, for any change suggestion in other files (For optimization for example ...)
3. TBD
