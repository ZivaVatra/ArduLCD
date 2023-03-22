#include <avr/io.h>
#include <util/delay.h>
#include <Arduino.h>
// vim: ts=4 ai
// based on info got from here:
// http://lcdproc.cvs.sourceforge.net/viewvc/lcdproc/lcdproc/server/drivers/hd44780-serial.h?content-type=text%2Fplain
// https://github.com/lcdproc/lcdproc/blob/master/server/drivers/hd44780-serial.c

#define LED_PIN 10		// This is the pin the backlight is controlled by, it must be a PWM pin
#define STARTUP_BRIGHTNESS 128 // What backlight brightness to start up with (50% by default).

# define BAUDRATE 57600  // What baudrate to use for the serial port

// Apprently you should not use Defines in C++. Even though I try to keep my code as much
// "C" as possible, the Arduino libraries are C++
const int LCDW = 20;
const int LCDH = 4;

// include the LCD library code:
#include <LiquidCrystal.h>

// And this is ths Hardware serial port (which is also bound to the UART->USB chip)
#include <HardwareSerial.cpp>

// initialize the library with the numbers of the interface pins
/*  Note, while all the ardu documentation and schematics show 4-bit operation. 
	It seems the library actually supports both 8-bit and 4-bit mode. It attempts
	8-bit, then falls back to 4-bit */
LiquidCrystal lcd(12, 11, 2, 3, 4, 5, 6, 7, 8, 9);


void set_backlight(int value) {
	// We can control the backlight via PWM here, range from 0 to 255 values
	// 0 is "off", 255 is "on" (max brightness). 
	analogWrite(LED_PIN, value); 	// pin 10 is PWM, so we can vary brightness this way
}

void setup() {
	pinMode(LED_PIN, OUTPUT);           // set pin to output
	// We first set the backlight to 50% brightness
	set_backlight(STARTUP_BRIGHTNESS);

	// set up the LCD's number of columns and rows:
	lcd.begin(LCDW, LCDH);
	// set up serial
	Serial.begin(BAUDRATE); 
	lcd.display();
	lcd.clear();
	lcd.write("Ready");

}

void loop() {
	// Todo: Investigate how to to interrupt driven serial I/O, like on PICs.
	// sitting in a tight loop like this is quite wasteful. 
	int x = 0;
	char cmd; //will hold our sent command
	int cursor = 0; //current cursor position (emulated)
	int scursor = 0; 
	char data[5]; //data buffer for (un)signed int printing

	while (Serial.available() > 0) {
		// Serial.read() is blocking, so we will wait until the serial buffer interrupt triggers
		cmd = Serial.read();

		/*
		so, serialVFD seems to be quite simple. There is an instruction escape character (0x1B)
		which tells us the next byte is an instruction byte. The other bytes are just passed through.
		
		We would need to capture a specific VFD instruction byte (0x4C + 00H to C0H) for setting VFD brightness, and
		pass it to our LCD backlight control.

		The other commands should be possible to just pass through to the LCD.
		*/
		

		switch(cmd) {
			// We got the escape command
			case 0x1B:
				while(1) {
					cmd = Serial.read();
					// Get the instruction command
					if (cmd != -1)
						break;
					/*
					Check if the instruction command is to set the backlight. Based on
					the docs it can be one of: 0x4C(25%), 0x8C(50%), 0xCC (75%), 0x10C(100%).
					The last one seems suspect, as it needs an extra byte, but we shall see).
					*/
					switch(cmd) {
						case 0x4C:
							//set brightness to 25%
							set_backlight(64);
							break;
						case 0x8C:
							//set brightness to 50%
							set_backlight(128);
							break;
						case 0xCC:
							//set brightness to 75%
							set_backlight(192);
							break;
						case 0x10C:
							//set brightness to 100%
							set_backlight(255);
							break;
						default:
							//other instructions we forward to the LCD
							lcd.write(cmd);
							break;
					}
				}
				break;
		}
	}
}
