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
		GPIO{GPIOA, GPIO_PIN_3},
		GPIO{GPIOA, GPIO_PIN_2},
		GPIO{GPIOA, GPIO_PIN_10},
		GPIO{GPIOB, GPIO_PIN_3},
		GPIO{GPIOB, GPIO_PIN_5},
		GPIO{GPIOB, GPIO_PIN_4},
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

constexpr std::array<GPIO,2> led_pin = {
		GPIO{GPIOB, GPIO_PIN_10},
		GPIO{GPIOA, GPIO_PIN_8},
};

/* Struct End */

/* Variable Begin */
constexpr uint8_t NUMBER_OF_TARGET = 15;
std::array<uint8_t,4> score;
constexpr std::array<uint8_t,NUMBER_OF_TARGET> SCORE_OF_TARGET = {1,1,1,1,1,1,1,1,1,1,1};
uint8_t target_debug;
uint8_t uart_success;
std::array<uint8_t,NUMBER_OF_TARGET> led_blinking_count;
/* Variable End */

/* Class Constructor Begin */
DFRobotDFPlayerMini DFPlayerMini;
std::array<UART_by_GPIO,NUMBER_OF_TARGET> uart_by_gpio = {UART_by_GPIO(), UART_by_GPIO()};
//UART_by_GPIO uart_by_gpio(uart_by_gpio[0].get_data(), GPIOB, GPIO_PIN_3);
/* Class Constructor End */

/* Function Prototype Begin */
/* Function Prototype End */

void init(void){
//	DFPlayerMini.begin(&huart1, false, false);
//	DFPlayerMini.next();
//	DFPlayerMini.Send_cmd(0x01, 0x00, 0x00);
	HAL_TIM_Base_Start_IT(&htim17);
	for(uint8_t i=0; NUMBER_OF_TARGET<i; i++){
		uart_by_gpio[i].init(uart_pin[i].GPIOx, uart_pin[i].GPIO_Pin);
	}
}

void loop(void){
}

/* Function Body Begin */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if(htim == &htim17){ // 1200Hz
    	static std::array<uint16_t,NUMBER_OF_TARGET> received_success_count = {0};

    	received_success_count[0]--;
    	if(received_success_count[0] == 0)
    		received_success_count[0] = 1;

    	uart_by_gpio[0].call_with_timer_interrupt();
    	if(uart_by_gpio[0].is_successful_reception()){
			target_debug = uart_by_gpio[0].get_data();
			if(0 <= uart_by_gpio[0].get_data() && uart_by_gpio[0].get_data() <=3){ // 受信値が銃のIDだったら
				received_success_count += 10*100; // 受信成功カウントを上げる
			}
    	}
    	if(10*1000 < received_success_count){ // 受信成功カウントが一定値を超えたら
			score[uart_by_gpio[0].get_data()] += SCORE_OF_TARGET[0]; // その時のIDに得点を入れる
			led_blinking_count[0] = 0;
    	}

    	if(uart_by_gpio[0].get_data() == 3){
    		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
    	}else{
    		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
    	}
    }
    else if(htim == &htim16){ // 4Hz
    	if(led_blinking_count[0] < 4){
    		HAL_GPIO_TogglePin(led_pin[0].GPIOx, led_pin[0].GPIO_Pin);
    		led_blinking_count[0]++;
    	}
    }
}
/* Function Body End */
