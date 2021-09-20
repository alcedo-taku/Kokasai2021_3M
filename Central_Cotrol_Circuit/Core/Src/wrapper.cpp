#include "wrapper.hpp"

/* Include Begin */
#include "DFRobotDFPlayerMini.hpp"
#include "usart.h"
#include <array>
/* Include End */

/* Enum Begin */
/* Enum End */

/* Struct Begin */
/* Struct End */

/* Variable Begin */
std::array<uint8_t,4> score;
/* Variable End */

/* Class Constructor Begin */
DFRobotDFPlayerMini DFPlayerMini;
/* Class Constructor End */

/* Function Prototype Begin */
/* Function Prototype End */

void init(void){
	DFPlayerMini.begin(&huart2, false, false);
	DFPlayerMini.next();
	DFPlayerMini.Send_cmd(0x01, 0x00, 0x00);
}

void loop(void){
	std::array<uint8_t,4> target;
	HAL_UART_Receive(&huart1, &target[0], sizeof(target[0]), 500);
	if(target[0] == 3){
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
	}else{
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
	}
	target = {0};
}

/* Function Body Begin */
/* Function Body End */
