#include "wrapper.hpp"

/* Include Begin */
#include "DFRobotDFPlayerMini.hpp"
#include "UART_by_GPIO.hpp"
#include "usart.h"
#include "gpio.h"
#include "tim.h"
#include <array>
#include <bitset>
/* Include End */

/* Enum Begin */
/* Enum End */

/* Struct Begin */
struct GPIO{
	GPIO_TypeDef * GPIOx;
	uint16_t GPIO_Pin;
};
GPIO uart_test = {GPIOB, GPIO_PIN_3};
/* Struct End */

/* Variable Begin */
constexpr uint8_t NUMBER_OF_TARGET = 15;
std::array<uint8_t,4> score;
constexpr std::array<uint8_t,NUMBER_OF_TARGET> SCORE_OF_TARGET = {1,1,1,1,1,1,1,1,1,1,1};
std::array<uint8_t,NUMBER_OF_TARGET> target; // 受信用変数
uint8_t target_debug;
uint8_t uart_success;
/* Variable End */

/* Class Constructor Begin */
DFRobotDFPlayerMini DFPlayerMini;
//std::array<UART_by_GPIO,3> uart_by_gpio = { {target[0], GPIOB, GPIO_PIN_3} };
UART_by_GPIO uart_by_gpio(target[0], GPIOB, GPIO_PIN_3);
/* Class Constructor End */

/* Function Prototype Begin */
/* Function Prototype End */

void init(void){
//	DFPlayerMini.begin(&huart2, false, false);
//	DFPlayerMini.next();
//	DFPlayerMini.Send_cmd(0x01, 0x00, 0x00);
	HAL_TIM_Base_Start_IT(&htim17);
}

void loop(void){
}

/* Function Body Begin */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if(htim == &htim17){
//   	target[0] |= (uint8_t)HAL_GPIO_ReadPin(uart_test.GPIOx, uart_test.GPIO_Pin) << uart_step;
    	uart_by_gpio.call_with_timer_interrupt();
    	if(uart_by_gpio.is_successful_reception()){
//    		score[target[0]] += SCORE_OF_TARGET[0];
			uart_success++;
			target_debug = target[0];
			if(target[0] == 3){
				HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
			}else{
				HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
			}
    	}
    }
}
/* Function Body End */
