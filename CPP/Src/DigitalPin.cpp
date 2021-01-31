/*
 * DigitalPin.cpp
 *
 *  Created on: Jan 28, 2021
 *      Author: Jason Kaufmann
 */

#include "DigitalPin.h"
#include "stm32l0xx_hal.h"

 //
// Created by Jason Kaufmann on 1/26/2021.
//

#define HANDSHAKE_DIGITALOUT_H

DigitalPin::DigitalPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
	_channel = GPIOx;
	_pinNumber = GPIO_Pin;
}

void DigitalPin::write(int setOrReset) {
        if (setOrReset) {
            HAL_GPIO_WritePin(_channel, _pinNumber, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(_channel, _pinNumber, GPIO_PIN_RESET);
        }
    }
GPIO_PinState DigitalPin::read(void) {
        return HAL_GPIO_ReadPin(_channel, _pinNumber);
    }

