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
constexpr std::array<GPIO,15> uart_pin = {
		GPIO{GPIOA, GPIO_PIN_10},
		GPIO{GPIOB, GPIO_PIN_3},
		GPIO{GPIOB, GPIO_PIN_5},
		GPIO{GPIOB, GPIO_PIN_4},
		GPIO{GPIOB, GPIO_PIN_10},
		GPIO{GPIOA, GPIO_PIN_8},
		GPIO{GPIOB, GPIO_PIN_13},
		GPIO{GPIOB, GPIO_PIN_14},
		GPIO{GPIOB, GPIO_PIN_15},
		GPIO{GPIOB, GPIO_PIN_1},
		GPIO{GPIOB, GPIO_PIN_2},
		GPIO{GPIOB, GPIO_PIN_11},
		GPIO{GPIOB, GPIO_PIN_12},
		GPIO{GPIOA, GPIO_PIN_11},
		GPIO{GPIOA, GPIO_PIN_12},
};

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
std::array<UART_by_GPIO,15> uart_by_gpio = {UART_by_GPIO(), UART_by_GPIO()};
//		UART_by_GPIO(target[0], uart_pin[0].GPIOx, uart_pin[0].GPIO_Pin),
//		UART_by_GPIO(target[1], uart_pin[1].GPIOx, uart_pin[1].GPIO_Pin)
//};
//UART_by_GPIO uart_by_gpio(target[0], GPIOB, GPIO_PIN_3);
/* Class Constructor End */

/* Function Prototype Begin */
/* Function Prototype End */

void init(void){
//	DFPlayerMini.begin(&huart2, false, false);
//	DFPlayerMini.next();
//	DFPlayerMini.Send_cmd(0x01, 0x00, 0x00);
	HAL_TIM_Base_Start_IT(&htim17);
	for(uint8_t i=0; 15<i; i++){
		uart_by_gpio[i].init(target[i], uart_pin[i].GPIOx, uart_pin[i].GPIO_Pin);
	}
}

void loop(void){
}

/* Function Body Begin */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if(htim == &htim17){
    	static uint16_t received_success_count = 0;

    	received_success_count--;
    	if(received_success_count == 0)
    		received_success_count = 0;

    	uart_by_gpio[0].call_with_timer_interrupt();
    	if(uart_by_gpio[0].is_successful_reception()){
    		static uint8_t prev_target;
			target_debug = target[0];
			if(0 <= target[0] && target[0] <=3){ // 受信値が銃のIDだったら
				received_success_count += 10*100; // 受信成功カウントを上げる
			}
    	}
    	if(10*1000 < received_success_count){ // 受信成功カウントが一定値を超えたら
			score[target[0]] += SCORE_OF_TARGET[0]; // その時のIDに得点を入れる
    	}

    	if(target[0] == 3){
    		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
    	}else{
    		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
    	}
    }
}
/* Function Body End */
