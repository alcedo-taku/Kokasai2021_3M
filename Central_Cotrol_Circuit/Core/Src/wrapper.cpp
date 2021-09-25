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
constexpr uint8_t NUMBER_OF_TARGET = 15;
std::array<uint8_t,4> score;
constexpr std::array<uint8_t,NUMBER_OF_TARGET> SCORE_OF_TARGET = {1,1,1,1,1,1,1,1,1,1,1};
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
	std::array<int8_t,NUMBER_OF_TARGET> target; // 受信用変数
	target.fill(-1); // すべての値を-1にする
	HAL_UART_Receive(&huart1, (uint8_t*)&target[0], sizeof(target[0]), 500);
	if(target[0] == 3){
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
	}else{
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
	}

	for(uint8_t i = 0; i<NUMBER_OF_TARGET; i++){
		score[target[i]] += SCORE_OF_TARGET[i];
	}
}

/* Function Body Begin */
/* Function Body End */
