#include "wrapper.hpp"

/* Include Begin */
#include "gpio.h"
#include "usart.h"
/* Include End */

/* Enum Begin */
/* Enum End */

/* Struct Begin */
/* Struct End */

/* Variable Begin */
const uint8_t rifle_id = 3;
/* Variable End */

/* Class Constructor Begin */
/* Class Constructor End */

/* Function Prototype Begin */
/* Function Prototype End */

void init(void){

}

void loop(void){
	if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14) == GPIO_PIN_RESET ){
		HAL_UART_Transmit(&huart2, (uint8_t *)&rifle_id, sizeof(rifle_id), 500);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	}else{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	}
}

/* Function Body Begin */
/* Function Body End */
