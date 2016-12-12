# RotaryEncoder
<img src="https://cdn.sparkfun.com//assets/parts/6/2/1/9/10982-01a.jpg" width="175" align="right"/> *Arduino* library which defines 
methods for accessing the 3-Color LED Rotary Encoder. This is manufactured by Top-Up and other companies and is available from many 
sources on the internet. The manufacturer's serial number is EC12PSD-017 and it can be found referenced at sources such as 
https://www.sparkfun.com/products/10982 or https://www.proto-pic.co.uk/rotary-encoder-illuminated-rgb.html along with numerous other 
places on eBay or Amazon. The encoder sends out rotational information on 2 pins using a quadrature system and also has a pushbutton.

While I've seen a couple of libraries that will work for this encoder, I wanted to use this in a project and opted to create a specific
library for it. It not only supports setting colors for the clockwise, counterclockwise and pushbutton along with a pleasing fade; but
is also designed to work with Atmel's/Arduino's interrupts so that any program using this class can process normally and only query the
pushbutton state and the current rotational value when it needs to.  Using interrupts limits the class to those pins which support 
hardware interrupt, but that limitation is offset by the efficiency with which the class works.

The hardware interrupt pins vary from one Atmel processor to another and need to be checked; the example programs for this class use 
those pins available for the Arduino Micro (ATmega32U4). Newer processor impose less restriction on which hardware interrupts can be used.
Likewise the 3 pins used for the Red, Green and Blue LED lights need to be PWM-capable pins, which vary from processor to processor. The
rotar encoder uses a common cathode (+) pin, so the 3 lights are driven to ground for full power and high for "OFF"; meaning an analog 
write of "255" turns the respective LED off and an analog write of "0" turns it on at full power.

Fading of the LEDs is done by piggy-backing off the timer0, which is used by the Arduino millis() function. 

See the [Wiki pages for details](https://github.com/SV-Zanshin/MicrochipSRAM/wiki) of the class and the variables / functions accessible in it.

![Zanshin Logo](https://www.sv-zanshin.com/images/gif/zanshinkanjitiny.gif) <img src="https://www.sv-zanshin.com/images/gif/zanshintext.gif" width="75"/>
