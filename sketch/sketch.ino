#include <avr/io.h>
#include <util/delay.h>
#include <Arduino.h>
// vim: ts=4 ai
// based on info got from here:
// http://lcdproc.cvs.sourceforge.net/viewvc/lcdproc/lcdproc/server/drivers/hd44780-serial.h?content-type=text%2Fplain

#define LED_PIN 10		// This is the pin the backlight is controlled by, it must be a PWM pin
#define STARTUP_BRIGHTNESS 128	// What backlight brightness to start up with (50% by default).

# define BAUDRATE 57600  // What baudrate to use for the serial port

// There was a reason I did not use these as defines, but I can't
// remember it :-/
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
		cmd = Serial.read();
		switch(cmd) {
			case 0x00:
                while (1) {
					cmd = Serial.read();
					if (cmd != -1) { break; }
                }   
                lcd.write(cmd);
				break;
			case 0x1:
				// set cursor position, next byte is pos
	            while (Serial.available() == 0) {
	                delay(1);
	            }
	            cmd = Serial.read();
				lcd.command(LCD_SETDDRAMADDR | cmd);
				cursor = cmd;
				break;
			case 0x2:
				//Home the cursor (set to position 0)
				lcd.home();
				cursor = 0;
				break;
			case 0x3:
				// shift cursor right
				lcd.command(LCD_MOVERIGHT);
				cursor++;
				break;
			case 0x4:
				// shift cursor left
				lcd.command(LCD_MOVELEFT);
				cursor--;
				break;
			case 0x5:
				// save cursor position
				scursor = cursor;
				break;
			case 0x6:
				// restore cursor position
				cursor = scursor;
				break;
			case 0x7:
				// Rings bell (fires 1000Hz tone through pin).
				// This is not implemented in our version
				break;
			case 0x8:
				// destructive backspace
				lcd.command(LCD_MOVELEFT);
				lcd.write(' ');
				lcd.command(LCD_MOVELEFT);
				break;
			case 0x9:
				// tab command -> 4 spaces to the right
				lcd.write('    ');
				cursor += 4;
				break;
			case 0xA:
				// Line feed: The cursor is moved to the same position on the 
				// next line. This is usually used in conjunction with control
				// code $0D to implement a "new line" function. 

				// the lcd wraps, so if we add LCDW to current position, we will
				// be on the same spot on next line (but next write goes to +1, so
				// we do -1 to move back one stop).
				cursor += (LCDW - 1)  ; 
				lcd.command(LCD_SETDDRAMADDR | cursor);
				break;
			case 0xB:
				// Vertical Tab.
				/* 	The display is scrolled up one line and the bottom line is cleared.
					The cursor remains in the same location. */
				cursor -= LCDW;
				lcd.command(LCD_SETDDRAMADDR | cursor);
				for ( x = 0; x != LCDW; x++) 
					lcd.write(' '); //clear the line up to orig cursor
					cursor++;
				break;
			case 0xC:
				/*  Form Feed.

				The display is cleared and the cursor is moved to the upper left of the display.*/
				lcd.clear();
				cursor = 0;
				break;
			case 0xD:
				// Carriage return, cursor is moved to beginning of current line
				while ( cursor % LCDW != 0) {
					lcd.command(LCD_MOVELEFT);
					cursor--;
					}
				break;
			case 0xE:
				// Shift display to the left
				lcd.scrollDisplayLeft();
				break;
			case 0xF:
				// Shift display to the right
				lcd.scrollDisplayRight();
				break;
			case 0x11:
				// Write LCD instruction -- used by lcdproc driver
                while (Serial.available() == 0) {
                    delay(1);
                }   
                cmd = Serial.read();
                lcd.command(cmd);
				break;
			case 0x12:
				// Write LCD Data -- used by lcdproc driver
                while (Serial.available() == 0) {
                    delay(1);
                }   
                cmd = Serial.read();
                lcd.write(cmd);
				cursor++;
				break;
			case 0x13:
				// Set LCD geometry. Code structure here, but does nothing
				// as I am unsure if this is needed
				while (Serial.available() == 0) {
                    delay(1);
                }   
                cmd = Serial.read();
				switch (cmd) {
					case 0:
						//no formatting
						break;
					case 1:
						//format for a one line display
						break;
					case 2:
						//format for a two line display
						break;
					case 4:
						//format for a four line display (default)
						break;
				}
			case 0x15:
				/* This was originally to set GPIOs. In our case
				 * we are using it to set the value of the backlight
				 * In both cases, you would send an 8-bit command, followed
				 * by 8-bit data. Just that before the 8-bit data was to control
				 * 8 1-bit channels, where here we take it as a 8-bit PWM value
				 */
				cmd = Serial.read();
				set_backlight(cmd);
				break;
			case 0x16:
				// print signed decimal number
				while (Serial.available() == 0) {
                    delay(1);
                }   
                cmd = Serial.read();
				sprintf(data,"%4d",cmd);
				lcd.print( data );
				cursor += 4;
				break;
			case 0x17:
				// print unsigned decimal number
				while (Serial.available() == 0) {
                    delay(1);
                }   
                cmd = Serial.read();
				sprintf(data,"%4u",cmd);
				lcd.print(data);
				cursor += 4;
				break;
			case 0x18:
				// Set cursor type
				while (Serial.available() == 0) {
                    delay(1);
                }   
                cmd = Serial.read();
				switch (cmd) {
					case 0:
						lcd.noCursor(); //no cursor
						lcd.noBlink(); //no blink
					case 1:
						lcd.blink(); //blinking cursor (block?)
					case 3:
						lcd.cursor(); //just underline
						lcd.noBlink();
					case 4:
						lcd.cursor(); //underline and blink
						lcd.blink();
					default:
						lcd.noCursor();
					}
				break;
			default:
				// By default we write to the LCD
				lcd.write(cmd);
				cursor++;
				break;



		}}}

