/*
 * UART_by_GPIO.hpp
 *
 *  Created on: 2021/09/29
 *      Author: takuj
 */

#ifndef SRC_UART_BY_GPIO_HPP_
#define SRC_UART_BY_GPIO_HPP_

#include <stdint.h>
#include <bitset>
#include "gpio.h"

class UART_by_GPIO {
public:
	UART_by_GPIO(uint8_t &data, GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin);
	void call_with_timer_interrupt();
	bool is_successful_reception();
private:
	GPIO_TypeDef * GPIOx;
	uint16_t GPIO_Pin;
	int8_t uart_step = -1;
	uint8_t &data;
	bool successful_reception;
};

#endif /* SRC_UART_BY_GPIO_HPP_ */
