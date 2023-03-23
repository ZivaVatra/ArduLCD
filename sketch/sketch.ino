#include <avr/io.h>
#include <util/delay.h>
#include <Arduino.h>
// vim: ts=4 ai

#define LED_PIN 10		// This is the pin the backlight is controlled by, it must be a PWM pin
#define STARTUP_BRIGHTNESS 2  // What backlight brightness to start up with (50% by default).
#define BAUDRATE 57600 // What baudrate to use for the serial port
#define LCDW 20  // LCD column length
#define LCDH 4   // LCD number of rows

// include the LCD library code:
#include <LiquidCrystal.h>

// And this is ths Hardware serial port (which is also bound to the UART->USB chip)
#include <HardwareSerial.cpp>

// initialize the library with the numbers of the interface pins
/*	Note, while all the ardu documentation and schematics show 4-bit operation. 
	It seems the library actually supports both 8-bit and 4-bit mode. It attempts
	8-bit, then falls back to 4-bit */
LiquidCrystal lcd(12, 11, 2, 3, 4, 5, 6, 7, 8, 9);


byte cmd; //will hold our sent command


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
	char welcome[LCDW];
	sprintf(welcome, "%dx%d Ready", LCDW, LCDH);
	lcd.write(welcome);
	lcd.home();

}

int serial_read() {
	int result = -1;
	while(result == -1) {
		if(Serial.available() > 0) {
			result = Serial.read();
		}
	}
	return result;
}

/**
 * From : https://github.com/lcdproc/lcdproc/blob/master/server/drivers.c
 * All the serial connections supported by driver
 *

static const struct hd44780_SerialInterface serial_interfaces[] = {
//        type                  instr ms  data     v     ^   pre bitrate bits  K   esc   cmd  B  Besc  Boff   Bon Multi  End
	{ HD44780_CT_PICANLCD,      0x11,  0, 0x12, 0x00, 0x1F,    0,   9600,   8, 0, 0x00,    0, 0,    0,    0,    0,   0,    0 },
	{ HD44780_CT_LCDSERIALIZER, 0xFE,  0,    0, 0x00, 0x00,    0,   9600,   8, 0, 0x00,    0, 0,    0,    0,    0,   0,    0 },
	{ HD44780_CT_LOS_PANEL,     0xFE,  0,    0, 0x00, 0x00,    0,   9600,   4, 1, 0xFE,    0, 1, 0xFD,    0, 0xFF,   0,    0 },
	{ HD44780_CT_VDR_LCD,       0xFE,  0,    0, 0x00, 0x00,    0,   9600,   4, 0, 0x00,    0, 0,    0,    0,    0,   0,    0 },
	{ HD44780_CT_VDR_WAKEUP,    0xC0,  0, 0xC4, 0xC0, 0xCF,    0,   9600,   4, 0, 0x00,    0, 1,    0, 0xC9, 0xC8,   1, 0xCF },
	{ HD44780_CT_PERTELIAN,     0xFE,  0,    0, 0x00, 0x00,    0,   9600,   8, 0, 0x00,    0, 1, 0xFE, 0x02, 0x03,   0,    0 },
	{ HD44780_CT_EZIO,          0xFE, 40,    0, 0x00, 0x00, 0x28,   2400,   4, 1, 0xFD, 0x06, 0,    0,    0,    0,   0,    0 }
};

Additional resources:

https://github.com/lcdproc/lcdproc/blob/master/server/drivers/hd44780-serial.c
https://lcdproc.sourceforge.net/docs/lcdproc-0-5-6-user.html#los-panel
*/

void loop() {
	cmd = serial_read();
	switch(cmd) {
			case 0xFE:
				lcd.command(serial_read());
				break;
			case 0xFD:
				// backlight control
				set_backlight(serial_read());
				break;
			default:
				// By default we write to the LCD
				lcd.write(cmd);
				break;
	}
}

