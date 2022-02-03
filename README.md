# watch_firmware

![example workflow](https://github.com/devATdbsutdio/watch_firmware/actions/workflows/build.yml/badge.svg)

### Dependant libraries:
1. [RV8803-Tiny](https://github.com/dattasaurabh82/RV8803Tiny) (Lib for helper class and functions for RV-8803 Real Time Clock)
2. [TinyMegaI2C](https://github.com/dattasaurabh82/TinyMegaI2C) (A less bloated I2C library for new ATTINY Series to communicate with RTC - forked version)

### Schematic:
1. Basic [ **uC + RTC** ]: [Here](https://github.com/dattasaurabh82/clock_firmware_production/files/7063914/uC_RTC_basic_schematic.pdf)
2. _Pro_ [ **uC + RTC + Serial COMM FTDI chip + LiPo charger = All in 1** ]: [Here](https://github.com/dattasaurabh82/clock_firmware_production/files/7063918/uC_RTC_FTDI_LIPO_pro_schematic.pdf)

### Development Environment:
Arduino-IDE, arduino-cli etc.

__Note:__ Compiled binaries (build/[filename].hex) can also be uploaded using _pyupdi_. Instructions below. 

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

<details><summary>Pseudo-code for the firmware written so far.</summary>

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

</details>

---

<details><summary>  Arduino IDE setup and steps </summary>
<p>

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
</p>
</details>

---

<details><summary> Build And upload using arduino-cli </summary>
<p>

[More info here](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Arduino-cli.md)

Configure arduino-cli for building the project:
```shell
arduino-cli core update-index 
arduino-cli config init
arduino-cli config add board_manager.additional_urls http://drazzy.com/package_drazzy.com_index.json
arduino-cli config set library.enable_unsafe_install true
arduino-cli core update-index
arduino-cli config dump
arduino-cli core install megaTinyCore:megaavr
arduino-cli core update-index
arduino-cli lib install TinyMegaI2C
arduino-cli lib install RV8803Tiny
```

Build the sketch:

__NOTE__: All the setting and fuses for the FQBN can be found using board details command against a specifc chip series: 
```shell
arduino-cli board details -b megaTinyCore:megaavr:atxy7
```

Now, from the root level of the project Directory: 
```shell
rm -rf build
mv clock_firmware_production.ino watch_firmware.ino
FQBN_ARG="-b megaTinyCore:megaavr:atxy7:chip=1607,clock=5internal,bodvoltage=1v8,bodmode=disabled,eesave=enable,millis=enabled,resetpin=UPDI,startuptime=0,wiremode=mors,printf=default,attach=allenabled"
OUTPUT_ARG="--output-dir $(pwd)/build"
arduino-cli compile $FQBN_ARG $OUTPUT_ARG
mv watch_firmware.ino clock_firmware_production.ino
```


To build and upload: 
```shell
rm -rf build
mv clock_firmware_production.ino watch_firmware.ino
FQBN_ARG="-b megaTinyCore:megaavr:atxy7:chip=1607,clock=5internal,bodvoltage=1v8,bodmode=disabled,eesave=enable,millis=enabled,resetpin=UPDI,startuptime=0,wiremode=mors,printf=default,attach=allenabled"
OUTPUT_ARG="--output-dir $(pwd)/build"
arduino-cli compile $FQBN_ARG $OUTPUT_ARG
mv watch_firmware.ino clock_firmware_production.ino
arduino-cli compile $FQBN_ARG $OUTPUT_ARG -u -p <your upload port> -P serialupdi -t
```
I chose `serialupdi` as my __PROGRAMMER__ for the argument, in that last command. 

You can see the available options for this and other settings using: `arduino-cli board details -b megaTinyCore:megaavr:atxy7`
And read more aboput them [here](https://github.com/SpenceKonde/AVR-Guidance/blob/master/UPDI/jtag2updi.md)

</p>
</details>


---


### Notes for Collaborators:
1. Please use [ExtraUtils.h](https://github.com/dattasaurabh82/clock_firmware_production/blob/old/ExtraUtils.h) for new Functions Suggestions.
2. Please comment to explain, for any change suggestion in other files (For optimization for example ...)
3. TBD
