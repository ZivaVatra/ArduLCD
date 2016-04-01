#include <avr/io.h>
#include <util/delay.h>
#include <Arduino.h>

// based on info got from here:
// http://lcdproc.cvs.sourceforge.net/viewvc/lcdproc/lcdproc/server/drivers/hd44780-serial.h?content-type=text%2Fplain


// include the LCD library code:
#include "LiquidCrystal.h"

// And this is ths Hardware serial port (which is also bound to the UART->USB chip)
#include <HardwareSerial.cpp>

// initialize the library with the numbers of the interface pins
/*  Note, while all the ardu documentation and schematics show 4-bit operation. 
	It seems the library actually support both 8-bit and 4-bit mode. It attempts
	8-bit, then falls back to 4-bit */
LiquidCrystal lcd(12, 11, 9,8,7,6, 5, 4, 3, 2);

void setup() {
	// set up the LCD's number of columns and rows:
	lcd.begin(24, 2);
	// set up serial
	Serial.begin(115200); //for debugging, was 115200
	lcd.display();
}

void loop() {
	/*  Coming from a microchip PIC background, I thought the Atmel would raise
		a hardware interrupt when a byte has been received on the UART, and is in
		the buffer, allowing you to make use of interrupt driven serial I/O. 
		However apparently the Atmel in the Uno only has two hardware interrupt pins,
		none of which tied to the UART,  so you have to sit in a loop waiting for data
		from I/O. I may be misunderstanding (sounds nuts to have no UART interrupts on a uC)
		but until I understand Atmels enough, will stick to the way everyone else does this.
	*/


	int x = 0;
	char cmd; //will hold our sent command
	while (Serial.available() > 0) {
		cmd = Serial.read();
		if (cmd == 0x11) {
			// We got escape char, next byte is a command
			while (Serial.available() == 0) {
				delay(1);
			}
			cmd = Serial.read();
			lcd.command(cmd);
			continue;
		}
		// If not command, data, just write out
		lcd.write(cmd);
	
	}
}
