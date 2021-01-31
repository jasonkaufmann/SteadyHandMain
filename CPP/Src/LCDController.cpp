/*
 * LCDController.cpp
 *
 *  Created on: Jan 28, 2021
 *      Author: Jason Kaufmann
 */

#include "LCDController.h"
#include "main.h"
#include "string.h"
#include <string>

LCDController::LCDController(GPIO_TypeDef *GPIOx, uint16_t rsPin, uint16_t rwPin, uint16_t ePin,
        uint16_t db0Pin, uint16_t db1Pin, uint16_t db2Pin, uint16_t db3Pin,
        uint16_t db4Pin, uint16_t db5Pin, uint16_t db6Pin, uint16_t db7Pin) :
        LCDMapping{{0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13},
                   {0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,0x50,0x51,0x52,0x53},
                   {0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27},
                   {0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67}}
{
	rs = new DigitalPin(GPIOx, rsPin);
	rw = new DigitalPin(GPIOx, rwPin);
	e = new DigitalPin(GPIOx, ePin);
	db0 = new DigitalPin(GPIOx, db0Pin);
	db1 = new DigitalPin(GPIOx, db1Pin);
	db2 = new DigitalPin(GPIOx, db2Pin);
	db3 = new DigitalPin(GPIOx, db3Pin);
	db4 = new DigitalPin(GPIOx, db4Pin);
	db5 = new DigitalPin(GPIOx, db5Pin);
	db6 = new DigitalPin(GPIOx, db6Pin);
	db7 = new DigitalPin(GPIOx, db7Pin);

	font_bit = 0;
	lines_bit = 0;

	HAL_Delay(50);          // LCD initialisation takes 40ms, be a little conservative.

}

void LCDController::init(void) {
	e->write(0);
	instruction(0x30);
	HAL_Delay(30);
	instruction(0x30);
	HAL_Delay(10);
	instruction(0x30);
	HAL_Delay(10);
	instruction(0x38);
	instruction(0x10);
	instruction(0x0c);
	instruction(0x06);
	data(0x00); //need to burn one for some reason
}

void LCDController::setCursor(int x, int y) {
    uint8_t index = LCDMapping[y][x];
    instruction(0x80 | index);
}

void LCDController::print(std::string sentence) {
	for(int i=0; i < sentence.length(); i++) {
		char letter = sentence[i];
		data((uint8_t)letter);
	}
}

void LCDController::shift(int leftOrRight) {
	instruction(0x18 | leftOrRight << 2);
}

void LCDController::shiftMultiple(int leftOrRight, int numberOfShifts) {

	for (int i=0; i<numberOfShifts; i++) {
		instruction(0x10 | leftOrRight << 2);
	}
}

void LCDController::rows(int l) {
    if(l == 1)
        lines_bit = 0;
    else if (l == 2)
        lines_bit = 8;

    instruction(32 | 16 | lines_bit | font_bit);
}

void LCDController::font(int f) {
    if(f == 0)
        font_bit = 0;
    else if (f == 1)
        font_bit = 4;

    instruction(32 | 16 | lines_bit | font_bit);
}


void LCDController::write(int c, int nrs) {
    int old_rs = rs->read();

    db0->write(c & 1);
	db1->write(c>>1 & 1);
	db2->write(c>>2 & 1);
	db3->write(c>>3 & 1);
	db4->write(c>>4 & 1);
	db5->write(c>>5 & 1);
	db6->write(c>>6 & 1);
	db7->write(c>>7 & 1);

    // Should check Busy Flag here.
    rs->write(nrs);
    rw->write(0);

    e->write(1);  // E must be on for min 480ns then drop to zero.  Trigger is on falling signal.

    // added extra wait state
    HAL_Delay(1);  // Needed the equivalent of this on the Raspberry Pi.

    // Tdsw Data Setup Width time at least 80ns.  No need for delay on slow processor.
    e->write(0); //
    rs->write(old_rs);
}

void LCDController::data(uint8_t c) {
    write(c, 1);
}
void LCDController::instruction(uint8_t c) {
    write(c, 0);
}

void LCDController::clear(void) {
    instruction(1);
    delayMicroseconds(1520); // clear takes 1.52 ms.
}
void LCDController::home(void) {
    instruction(2);
    delayMicroseconds(1520); // home takes 1.52 ms.
}

void LCDController::str_at(const char *str, uint8_t x = 0, uint8_t y = 0) {
    uint8_t *c = (uint8_t *) str;
    ddram(y * 0x40 + x);
    for (uint8_t i = 0; i < 80 && (*c); i++) { // Will never write more than 80 chars.
        data(*c);
        c++;
    }
}
void LCDController::row0(const char *str, uint8_t x = 0) {
	str_at(str, x, 0);
}

void LCDController::row1(const char *str, uint8_t x = 0) {
	str_at(str, x, 1);
}

void LCDController::str(const char *str) {
	str_at(str);
}

void LCDController::on() {
    instruction(0x0C); // 0000 1DCB Display=1 Cursor=0 Blink=0

}

void LCDController::off() {
    instruction(0x08); // 0000 1DCB Display=0 Cursor=0 Blink=0
}

void LCDController::cgram(uint8_t a) {
    instruction(0x40 | a);
}

 void LCDController::ddram(uint8_t a) {
    instruction(0x80 | a);
}

