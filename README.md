# ArduLCD
Arduino based USB interface for HD44780 LCD to lcdproc and equivalent programs.

# Beginnings

As time goes on it is harder and harder to connected HD44780 LCDs to computer systems. It used to be that every computer had a parallel port and you just wired it up there.
However fewer and fewer machines have parallel ports and in some cases the software won't work (e.g. lcdproc on FreeBSD locks up if you try to use the parallel port).

So having bought an Arduino and realising how cheap and powerful it was, I thought it would be a good fit for a LCD to USB interface. It is also a relatively simple first project to learn with.

# Design

Originally using an implementation of the pic-an-lcd protocol, further investigation of lcdproc's drivers indicated that 99% of the protocol was unused and therefore superfluous. In addition the bits it did use were not the most efficient, so I looked at some of the other protocols supported by the driver and found the "los-panel" (For "LCD on Serial) protocol not only simpler to implement, it had features that would be nice to include, such as backlight control. So version3 switched to this protocol, and after testing and use by myself, it is has been promoted to the stable "master" branch.

As the Arduino is stupidly overpowered for such a simple task, I worked on making it fast. The HD44780 makes use of full 8 bit transfers and serial speed is at 57600 baud. I saw no real improvement running at faster baud rates as LCD display itself cannot refresh that quickly, however it should work if you want to go even faster (just change the BAUDRATE define).


# Current status
### 14/05/2024

- Version3 has been merged to master. This is now the new stable implementation. It makes use of "los-panel" driver, which is newer, cleaner and better supported.
- Added example LCDd.conf for FreeBSD
- Added some photos of my prototype, so you can see ArduLCD in action. The case I made out of some old 3.5" HDD sleds. Does the job fine. 

![display](/resources/20231124_190621.jpg "LCD Display")
![front](/resources/20231124_190616.jpg "Prototype front")

This is the reference implementation (so matches the schematic directly) that I use for testing and development of ArduLCD.

### 22/03/2023

- Added LCDd.conf example for los-panel
- Switched over from ino (which is no longer under development), to the official Arduino IDE
- Main development switching to "version3" branch, which will replace picanlcd protocol completely. The old master branch will be renamed "picanlcd", in case anyone specifically wants to use an implementation of that protocol in future.

### 23/04/2019

After an issue raised about lack of wiring diagram, I decided to whip one up and provide it here. You can find it in the "resources" folder. It was done with gschem (http://www.geda-project.org/), and feel free to make improvements. The diagram includes a transistor, capacitor and wiring between the backlight and pin 10 for dynamic backlight brightness control. This is optional, and can be omitted if you are not interested in that capability.

If you do build in the PWM control circuitry, then you can utilise the old "GPIO" command in pic-an-lcd. This will allow you to vary the brightness of the backlight from 0 (off) to 255 (100%) via a serial command. Otherwise you can fix the brightness of your backlight at compile time by changing the "STARTUP_BRIGHTNESS" define in the code.


### 02/05/2016

We have a first working version of ArduLCD. Supports HD44780 displays using the pic-an-lcd driver of LCDd (tested on version 0.5.5).

### 02/04/2016

Well, I found a user manual for the original PIC-an-LCD: http://www.phanderson.com/PIC-n-LCD/user_manual.html

Turns out the protocol is far more advanced than I expected, allowing for quite a bit of control, higher level LCD functions, and the ability to either output a tone from a speaker, or to fire off 4 GPIO pins.

In addition, after looking at the lcdproc source code, none of the advanced features are used. lcdproc basically just uses the "raw passthrough" escape characters to talk to the lcd directly.

# Known bugs

Currently there are no known bugs. It seems to work fine on my test rig, and the first proper implementation on my server seems to work fine (blue 20x4 LCD, LCDd 0.5.5 on FreeBSD). Feel free to give it a test yourselves to see if you come across any problems, and raise a ticket if you come across any issues.

# Building

Building is using the standard Arduino IDE, making sure that the libraries needed (e.g. LiquidCrystal) are available.

## To Change the display size (default: 24x2)

At the moment you have to edit the code directly, there are two constants you have to change LCDW and LCDH, which govern the LCD dimensions in character width and height respectively

# Schematic

Find it in "resources", called "wiring_schematic.sch". There is a png version too if you just want to view without editing, "wiring_schematic.png"

- R1: Variable resistor 10k
- Q1: Any NPN transistor really (I used the faithful BC337)
- C1: I found that Capacitor value varies, depending on the power draw of the backlight. I'm using a 22uF electrolytic as that is what I had to hand for my test rig.

# lcdproc configuration

There is a sample "LCDd.conf" in the resources section
