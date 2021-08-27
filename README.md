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

### Notes for Collaborators:
1. Please check the codebase on "old" branch. I know I have a messy sense of naming branches
2. Please use [ExtraUtils.h](https://github.com/dattasaurabh82/clock_firmware_production/blob/old/ExtraUtils.h) for new Functions Suggestions.
3. Please comment to explain a noob like me, for any change suggestion in other files (For optimization for example ...)

