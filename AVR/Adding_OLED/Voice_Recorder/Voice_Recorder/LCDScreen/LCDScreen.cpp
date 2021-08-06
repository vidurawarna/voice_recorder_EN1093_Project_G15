#include "LCDScreen.h"
#include <inttypes.h>
//#include <Arduino.h>
#include <util/delay.h>
#include <Wire.h>

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set:
//    DL = 1; 8-bit interface data
//    N = 0; 1-line display
//    F = 0; 5x8 dot character font
// 3. Display on/off control:
//    D = 0; Display off
//    C = 0; Cursor off
//    B = 0; Blinking off
// 4. Entry mode set:
//    I/D = 1; Increment by 1
//    S = 0; No shift
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

/* LCDScreen::LCDScreen(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t charsize)
{
	_addr = lcd_addr;
	_cols = lcd_cols;
	_rows = lcd_rows;
	_charsize = charsize;
	_backlightval = LCD_BACKLIGHT;
} */
LCDScreen::LCDScreen(uint8_t lcd_addr)
{
	_addr = lcd_addr;
}
void LCDScreen::begin() {
	Wire.begin();
	_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	_displayfunction |= LCD_2LINE;
	/* if (2 > 1) {
		_displayfunction |= LCD_2LINE;
	} */

	// for some 1 line displays you can select a 10 pixel high font
	/* if ((0x00 != 0) && (2 == 1)) {
		_displayfunction |= LCD_5x10DOTS;
	} */

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	_delay_ms(50);

	// Now we pull both RS and R/W low to begin commands
	expanderWrite(0x08);	// reset expanderand turn backlight off (Bit 8 =1)
	_delay_ms(1000);

	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46

	// we start in 8bit mode, try to set 4 bit mode
	write4bits(0x03 << 4);
	_delay_us(4500); // wait min 4.1ms

	// second try
	write4bits(0x03 << 4);
	_delay_us(4500); // wait min 4.1ms

	// third go!
	write4bits(0x03 << 4);
	_delay_us(150);

	// finally, set to 4-bit interface
	write4bits(0x02 << 4);

	// set # lines, font size, etc.
	command(LCD_FUNCTIONSET | _displayfunction);

	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	display();

	// clear it off
	clear();

	// Initialize to default text direction (for roman languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

	// set the entry mode
	command(LCD_ENTRYMODESET | _displaymode);

	home();
}

/********** high level commands, for the user! */
void LCDScreen::clear(){
	command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	_delay_us(2000);  // this command takes a long time!
}

void LCDScreen::home(){
	command(LCD_RETURNHOME);  // set cursor position to zero
	_delay_us(2000);  // this command takes a long time!
} 

void LCDScreen::setCursor(uint8_t col, uint8_t row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if (row > 2) {
		row = 1;    // we count rows starting w/0
	}
	command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}


void LCDScreen::display() {
	_displaycontrol |= LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

/*********** mid level commands, for sending data/cmds */

inline void LCDScreen::command(uint8_t value) {
	send(value, 0);
}

inline size_t LCDScreen::write(uint8_t value) {
	send(value, Rs);
	return 1;
}


/************ low level data pushing commands **********/

// write either command or data
void LCDScreen::send(uint8_t value, uint8_t mode) {
	uint8_t highnib=value&0xf0;
	uint8_t lownib=(value<<4)&0xf0;
	write4bits((highnib)|mode);
	write4bits((lownib)|mode);
}

void LCDScreen::write4bits(uint8_t value) {
	expanderWrite(value);
	pulseEnable(value);
}

void LCDScreen::expanderWrite(uint8_t _data){
	Wire.beginTransmission(_addr);
	Wire.write((int)(_data) | 0x08);
	Wire.endTransmission();
}

void LCDScreen::pulseEnable(uint8_t _data){
	expanderWrite(_data | En);	// En high
	_delay_us(1);		// enable pulse must be >450ns

	expanderWrite(_data & ~En);	// En low
	_delay_us(50);		// commands need > 37us to settle
}
