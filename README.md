# ArduLCD
Arduino based interface for HD44780 LCD to lcdproc/lcd4linux and equivalent.

# Beginnings

As time goes on, it is harder and harder to connected HD44780 LCDs to computer systems. It used to be that every computer had a parallel port, and you just wired it up there.  However fewer and fewer machines have parallel ports, and in some cases the software won't work (e.g. lcdproc on FreeBSD locks up if you try to use the parallel port). 

So having bought an arduino, and realising how cheap and powerful it was, I thought it would be a good fit for a LCD to USB interface. It is also a relatively simple first project. 

# Design

I decided to emulate the old "pic-an-lcd" interface. This was a serial to HD44780 interface that was used in the late 90's. I knew of it back then, but never bothered due to the abundance of parallel ports.  I decided to emulate this because:

1. It is old enough that it is supposed by most LCD software. In particular the software I tend to use (LCDproc and lcd4linux)
2. It is serial based, so I can make use of the existing USB->Serial interface provided by the arduino. 
3. It is a relatively simple protocol, being not much more than a serial encapsulation of HD44780 commands and data. 

Because the Arduino is stupidly overpowered for such a simple task, I worked on making it fast. The HD44780 makes use of 8 bit transfers, and serial speed is at 115200.

This code is nowhere near good, it is more like a hack than a proper development, but posted here in case someone else is interested in such a thing, and saves them the time to reverse engineer the protocol (the original "pic-an-lcd" page seems to have dropped of the internet, so had to work out what it is doing from the LCDproc docs).


# Current status

Well... it works. However on my testbench the LCD gets corrupted after a while. Not sure if that is due to the software timings being off, or due the length of wiring + noise (or the LCD itself, which was pulled from a junked telephone). So will have to investigate further. 

# Building

I don't know about you, but I use "ino" on the command line. cd to the root of the repor dir, and run "ino build", when completed successfully, run "ino upload" to push to arduino. 

## To Change the display size (default: 24x2)

At the moment you have to edit the code directly, there are two defines you have to change LCDW and LCDH, which govern the LCD dimensions in character width and height respectively

# Schematic

Todo: Add schematic here for how to wire up the arduino to a HD44780 LCD
