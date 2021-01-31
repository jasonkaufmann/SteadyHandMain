/*
 * LCDController.h
 *
 *  Created on: Jan 28, 2021
 *      Author: Jason Kaufmann
 */

#ifndef SRC_LCDCONTROLLER_H_
#define SRC_LCDCONTROLLER_H_

#include "DigitalPin.h"
#include <string>

class LCDController {
public:
	DigitalPin *rs, *rw, *e, *db0, *db1, *db2, *db3, *db4, *db5, *db6, *db7;
	uint8_t lines_bit, font_bit;
    uint8_t LCDMapping[4][20];
	LCDController(GPIO_TypeDef *GPIOx, uint16_t rsPin, uint16_t rwPin, uint16_t ePin,
            uint16_t db0Pin, uint16_t db1Pin, uint16_t db2Pin, uint16_t db3Pin,
            uint16_t db4Pin, uint16_t db5Pin, uint16_t db6Pin, uint16_t db7Pin);
	void rows(int);
	void font(int);
	void write(int,int);
	void data(uint8_t);
	void instruction(uint8_t);
	void clear();
	void home();
	void str_at(const char *, uint8_t, uint8_t);
	void row0(const char *, uint8_t);
	void row1(const char *, uint8_t);
	void str(const char *);
	void on();
	void off();
	void cgram(uint8_t);
	void ddram(uint8_t);
	void init(void);
	void print(std::string);
	void shift(int);
	void shiftMultiple(int, int);
	void setCursor(int, int);
};

#endif /* SRC_LCDCONTROLLER_H_ */
