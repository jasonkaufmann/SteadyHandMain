/*
 * DigitalPin.h
 *
 *  Created on: Jan 28, 2021
 *      Author: Jason Kaufmann
 */

#ifndef SRC_DIGITALPIN_H_
#define SRC_DIGITALPIN_H_

#include "main.h"

class DigitalPin {
public:
    GPIO_TypeDef* _channel;
    uint16_t _pinNumber;
	DigitalPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
	void write(int);
	GPIO_PinState read(void);
};

#endif /* SRC_DIGITALPIN_H_ */
