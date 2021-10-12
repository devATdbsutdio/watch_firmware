# clock_project_description

I’m designing a small wrist watch that would display time for 5 seconds, when a button is clicked in HH:MM format on 2x(2x7 segment). When a button is clicked it shows time for 5 secs and after that goes to sleep to save battery (3.7V 120mAh battery will be used).

![Screenshot 2021-08-24 at 2 11 34 PM](https://user-images.githubusercontent.com/4619862/130568413-add3bca5-adc5-483b-94b4-566a70fd682a.png)

The uC used is [ATTINY1607-MFR](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny804-06-07-1604-06-07-DataSheet-DS40002312A.pdf). 

I'm using Arduino IDE and support for new ATTINY series uCs are being implemented by [megaTinyCore](https://github.com/SpenceKonde/megaTinyCore). 
It uses [UPDI](https://github.com/SpenceKonde/AVR-Guidance/blob/master/UPDI/jtag2updi.md) to program the uC. 

*Note: I use/used Arduino IDEto write dev code and using arduino-cli to create a rapid firmware uploading system to all the fianl devices that will be made and these are prefrred as they are cross platfiorm tools.*

The RTC used for time-keeping is [RV-8803](https://www.microcrystal.com/fileadmin/Media/Products/RTC/App.Manual/RV-8803-C7_App-Manual.pdf).

I have written an [optimised i2C library]() for it to be used by megaTinyCore ATTINYs and using that library I have also written [another Arduino library to interact with RV-8803 RTC](https://github.com/dattasaurabh82/RV8803Tiny). Both are heavily optimized for efficiency. But if recommended by experts to tweak or collaborate, they can be changed.


The Circuit has 1 button.

The circuit also has built in FTDI chip to communicate with uC via UART. I’m using this to set time on the RV-8803 RTC (it is intended to receive a string of format `HH:MM:SS:DAY:DD:MM:YYYY`)


## Problem seen so far:
**In `sleepmode` something is not going to sleep properly.**

When `Serial` methods are in use, in `sleep` mode, the current consumption is still very high, in **mA** range. 
![ppk-20210823T143246](https://user-images.githubusercontent.com/4619862/130575082-d9d38af0-3cb2-4cff-ba27-9a146b582328.png)

When `Serial` methods are not used, in `sleep` mode, the current consumption is low, in **4-5uA** range. But need to be lower.  
![ppk-20210823T143406](https://user-images.githubusercontent.com/4619862/130575161-4dda49d6-bf16-41d7-9e73-b67584dfcf7f.png)

when the uC is Awake and displaying time, the current consumption is in **30-50mA** (Doesn't matter serial methods are implemnted or not). This is expected as we have LEDS glowing and consuming power for that 5 sec period. But may be, code here, also in awake mode can be a bit better optimised to comsume less power.  
![ppk-20210823T141831](https://user-images.githubusercontent.com/4619862/130575222-c51d2718-da4e-4d91-b663-2b8f03a15eb6.png)


May be the problem is `setup()` and `loop()` of Arduino framework? 
May be the `sleep` methods of `megaTinyCore` are not optimized properly. 

### Info for megaTinyCore's sleep methods and issues. 
[link1](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/PowerSave.md), [link2](https://github.com/SpenceKonde/megaTinyCore/discussions/502), [link3](https://github.com/SpenceKonde/megaTinyCore/issues/158)

### Seeking:
1. Need R&D support to write best optimized firmware on the HW design to work as the pseudo-code above while giving bestpower saving performance. 
2. A sample assembled board with all the components populated can be send over on request for the developer to test their firmware. 
3. Serial method needs to be used for time reset as mentioned above. Any other suggested method can be discussed. 
4. Arduino IDE would be preferred as later production methods are using arrduino-cli for final version.  
5. Code needs to eb clean and readable.
6. We can setup a separate branch in git for this. Need to discuss how we can collaborate on this and share.  
7. If HW needs to be shipped, then it would take almost a month from my studio to your lab in EU. Only after that may be you can start testing the firmware. Need to discuss. 
8. Total period of final developemnt deleivery with full test should be at max 1 month after starting.
