# ArduLCD
Arduino based interface for HD44780 LCD to lcdproc and equivalent.

# Beginnings

As time goes on, it is harder and harder to connect HD44780 LCDs to computer systems. It used to be that every computer had a parallel port, and you just wired it up there.  However fewer and fewer machines have parallel ports, and in some cases the software won't work (e.g. In my experience, lcdproc on FreeBSD locks up if you try to use the parallel port).

So having bought an arduino, and realising how cheap and powerful it was, I thought it would be a good fit for a LCD to USB interface. It is also a relatively simple first project to get into Arduino programming.

# Design

The original design worked on emulating the old "pic-an-lcd" interface. This was a (proprietery) serial to HD44780 interface that was used in the late 90's. I knew of it back then, but never bothered due to the abundance of parallel ports. Being the only existing protocol I knew of, and being relatively simple, I made use of that for version 1 of ArduLCD. However as of 2019, the pic-an-lcd interface is actually being removed from software (e.g. lcd4linux no longer supports it), and generally its on its way out. Therefore I had to find a new serial protocol to emulate.

After some research, I discovered that a lot of VFDs have serial communication. On lcdproc this is known as the "serialVFD" interface. This is a more modern interface, it exists in silicon (meaning it isn't going to change anytime soon), and VFDs are still used in industry and amateur electronics projects, so as a protocol, it is still actively supported (and probably will be for a while).

There are more reasons to use the interface, such as:

1. It is pretty much generic now. There is an abundance of VFD devices that make use of it.
2. It supports intensity control on the VFD (which we will use for backlight intensity control)
3. It is pretty much supported by all lcd software.
4. It seems essentially to just be a serialisation around the standard HD44780 command set, with a bit of garnish.

Therefore, for version 2, we decided to emulate the "serialVFD" interface. As the Arduino is stupidly overpowered for such a simple task, I worked on making it fast. The HD44780 makes use of 8 bit transfers, and serial speed is at 57600 baud.

# Future ideas
	 - One future idea is to allow multiple LCD displays to be driven by ArduLCD. Due to the power of the Arduino even at 57600 baud it still spends a lot of time idle. As the serialVFD protocol does not support addressing, we would probably need to merge all the LCDs into one large "virtual LCD".

# Wiring Diagram

You can find this in the "resources" folder, with the name "wiring_schematic". You have both a png for general use/printing, and the original .sch file. It was done with gschem (http://www.geda-project.org/), and feel free to make improvements. The diagram includes a transistor, capacitor and wiring between the backlight and pin 10 for dynamic backlight brightness control. This is optional, and can be omitted if you are not interested in that capability.

-- 19/07/2019

Decided to make a new version. This one uses serialVFD protocol for communication. The old "pic-an-lcd" interface is no longer used.

-- 23/04/2019

After an issue raised about lack of wiring diagram, I decided to whip one up and provide it here. You can find it in the "resources" folder. It was done with gschem (http://www.geda-project.org/), and feel free to make improvements. The diagram includes a transistor, capacitor and wiring between the backlight and pin 10 for dynamic backlight brightness control. This is optional, and can be omitted if you are not interested in that capability.

-- 02/05/2016

We have a first working version of ArduLCD. Supports HD44780 displays using the pic-an-lcd driver of LCDd (tested on version 0.5.5).

-- 02/04/2016

Well, I found a user manual for the original PIC-an-LCD: http://www.phanderson.com/PIC-n-LCD/user_manual.html

Turns out the protocol is far more advanced than I expected, allowing for quite a bit of control, higher level LCD functions, and the ability to either output a tone from a speaker, or to fire off 4 GPIO pins.

In addition, after looking at the lcdproc source code, none of the advanced features are used. lcdproc basically just uses the "raw passthrough" escape characters to talk to the lcd directly.

# Known bugs

Currently there are no known bugs. It seems to work fine on my test rig, and the first proper implementation on my server seems to work fine (blue 20x4 LCD, LCDd 0.5.5 on FreeBSD). Feel free to give it a test yourselves to see if you come across any problems. Feel free to raise a ticket if you come across anything. 

# Building

I don't know about you, but I use "ino" on the command line. cd to the root of the repo dir, and run "ino build", when completed successfully, run "ino upload" to push to arduino.

commands to run for install (I use Devuan/Debian, alter to taste for your specific distro):
```
apt install arduino-mk
pip install ino --user 
```


## To Change the display size (default: 24x2)

At the moment you have to edit the code directly, there are two constants you have to change LCDW and LCDH, which govern the LCD dimensions in character width and height respectively

# Schematic

Find it in "resources", called "wiring_schematic.sch". There is a png version too if you just want to view without editing, "wiring_schematic.png"

- R1: Variable resistor 1k
- Q1: Any NPN transistor really (I used the faithful BC337
- C1: 47uF Capacitor (any type, but if you use electrolytic make sure you get the polarity right!) 

# lcdproc configuration

This is how to configure LCDd.conf to work with ArduLCD

```
Driver=hd44780


## Hitachi HD44780 driver ##
[hd44780]

# Select what type of connection. See documentation for availabe types.
ConnectionType=picanlcd

# Device of the serial, I2C, or SPI interface [default: /dev/lcd]
Device=/dev/ttyUSB0

# Bitrate of the serial port (0 for interface default)
Speed=57600

# If you have a keypad connected.
# You may also need to configure the keypad layout further on in this file.
Keypad=no

# Specifies the size of the LCD.
# In case of multiple combined displays, this should be the total size.
Size=24x2

# Character map to to map ISO-8859-1 to the LCD's character set
# [default: hd44780_default; legal: hd44780_default, hd44780_euro, ea_ks0073,
# sed1278f_0b, hd44780_koi8_r, hd44780_cp1251, hd44780_8859_5, upd16314 ]
# (hd44780_koi8_r, hd44780_cp1251, hd44780_8859_5 and upd16314 are possible if
# compiled with additional charmaps)
CharMap=hd44780_default

# If your display is slow and cannot keep up with the flow of data from
# LCDd, garbage can appear on the LCDd. Set this delay factor to 2 or 4
# to increase the delays. Default: 1.
#DelayMult=2

# Some displays (e.g. vdr-wakeup) need a message from the driver to that it
# is still alive. When set to a value bigger then null the character in the
# upper left corner is updated every <KeepAliveDisplay> seconds. Default: 0.
#KeepAliveDisplay=0

# If you experience occasional garbage on your display you can use this
# option as workaround. If set to a value bigger than null it forces a
# full screen refresh <RefreshDiplay> seconds. Default: 0.
#RefreshDisplay=5



```
