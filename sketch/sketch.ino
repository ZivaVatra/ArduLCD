#include <avr/io.h>
#include <util/delay.h>
#include <Arduino.h>
// vim: ts=4 ai
// based on info got from here:
// http://lcdproc.cvs.sourceforge.net/viewvc/lcdproc/lcdproc/server/drivers/hd44780-serial.h?content-type=text%2Fplain

#define LED_PIN 10		// This is the pin the backlight is controlled by, it must be a PWM pin
#define STARTUP_BRIGHTNESS 2  // What backlight brightness to start up with (50% by default).

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
/*	Note, while all the ardu documentation and schematics show 4-bit operation. 
	It seems the library actually supports both 8-bit and 4-bit mode. It attempts
	8-bit, then falls back to 4-bit */
LiquidCrystal lcd(12, 11, 2, 3, 4, 5, 6, 7, 8, 9);


void set_backlight(int value) {
	// We can control the backlight via PWM here, range from 0 to 255 values
	// 0 is "off", 255 is "on" (max brightness). 
	analogWrite(LED_PIN, value);	// pin 10 is PWM, so we can vary brightness this way
}

void setup() {
	pinMode(LED_PIN, OUTPUT);			// set pin to output
	// We first set the backlight to 50% brightness
	set_backlight(STARTUP_BRIGHTNESS);

	// set up the LCD's number of columns and rows:
	lcd.begin(LCDW, LCDH);
	// set up serial
	Serial.begin(BAUDRATE); 
	lcd.display();
	lcd.clear();
	lcd.write("Ready");
	lcd.home();

}

byte serial_read() {
	int result = -1;
	while(result == -1) {
		if(Serial.available() > 0) {
			result = Serial.read();
		}
	}
	return (byte)result;
}


void loop() {
	byte cmd; //will hold our sent command
	cmd = serial_read();
	switch(cmd) {
			case 0x11:
				// Write LCD instruction -- used by lcdproc driver
				lcd.command(serial_read());
				break;
			default:
				// By default we write to the LCD
				lcd.write(cmd);
				break;

}}

