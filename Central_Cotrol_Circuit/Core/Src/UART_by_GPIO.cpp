/*
 * UART_by_GPIO.cpp
 *
 *  Created on: 2021/09/29
 *      Author: takuj
 */

#include "UART_by_GPIO.hpp"

UART_by_GPIO::UART_by_GPIO(uint8_t &data, GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin):
	data(data),
	GPIOx(GPIOx),
	GPIO_Pin(GPIO_Pin)
{

}

void UART_by_GPIO::call_with_timer_interrupt(){
	successful_reception = false;
	if(uart_step == -1){
		if(HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_RESET){ // スタートビット
			uart_step++;
			data = 0;
		}
	}else if(0 <= uart_step && uart_step <= 7){ // データﾋﾞｯﾄ
//   	data |= (uint8_t)HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) << uart_step;
		if(HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_RESET){ // LOWだったら
			data |= (0<<uart_step);
		}else{
			data |= (1<<uart_step);
		}
		uart_step++;
	}else if(uart_step == 8){ // parity even(偶数)
		if( ( std::bitset<8>(data).count() + (uint8_t)HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) )%2 ){ // 1のビットの合計個数が奇数個だったら
			uart_step = -2;
		}else{ // 1のビットの合計個数が偶数個だったら
			uart_step++;
		}
	}else if(uart_step == 9){
		uart_step++;
		if(HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_SET){ // ストップビット
    		uart_step = -1; // 段階を初期化
    		successful_reception = true;
		}
	}else{ // 立下りエッジを見るための処理
		if(HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_SET){
			uart_step = -1;
		}else{
			uart_step = -2;
		}
	}
}

bool UART_by_GPIO::is_successful_reception(){
	return successful_reception;
}
