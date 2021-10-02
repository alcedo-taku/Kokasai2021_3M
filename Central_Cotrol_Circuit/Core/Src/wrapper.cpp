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

constexpr uint8_t NUMBER_OF_TARGET = 15;

/* Enum Begin */
/* Enum End */

/* Struct Begin */
struct GPIO{
	GPIO_TypeDef * GPIOx;
	uint16_t GPIO_Pin;
};
GPIO uart_test = {GPIOB, GPIO_PIN_3};
constexpr std::array<GPIO,NUMBER_OF_TARGET> uart_pin = {
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
std::array<uint8_t,4> score;
constexpr std::array<uint8_t,NUMBER_OF_TARGET> SCORE_OF_TARGET = {1,1,1,1,1,1,1,1,1,1,1}; // 各的の点数
uint8_t target_debug;
std::array<uint8_t,NUMBER_OF_TARGET> led_blinking_count; // LEDの点滅回数をカウント
/* Variable End */

/* Class Constructor Begin */
DFRobotDFPlayerMini DFPlayerMini;
std::array<UART_by_GPIO,NUMBER_OF_TARGET> uart_by_gpio = {UART_by_GPIO(), UART_by_GPIO()};
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

    	for(uint8_t i=0; i<NUMBER_OF_TARGET; i++){ // 的の数だけループ
        	received_success_count[i]--; // 毎回、受信成功カウントを少しずつ減らす
        	if(received_success_count[i] == 0) // アンダーフロー防止処理
        		received_success_count[i] = 1;

        	uart_by_gpio[i].call_with_timer_interrupt();
        	if(uart_by_gpio[i].is_successful_reception()){
    			target_debug = uart_by_gpio[i].get_data();
    			if(0 <= uart_by_gpio[i].get_data() && uart_by_gpio[i].get_data() <=3){ // 受信値が銃のIDだったら
    				received_success_count[i] += 10*100; // 受信成功カウントを増やす
    			}
        	}

        	if(10*1000 < received_success_count[i]){ // 受信成功カウントが一定値を超えたら
    			score[uart_by_gpio[i].get_data()] += SCORE_OF_TARGET[i]; // その時のIDに得点を入れる
    			led_blinking_count[i] = 0;
        	}
    	}

    	// Debug用
    	if(uart_by_gpio[0].get_data() == 3){
    		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
    	}else{
    		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
    	}
    }
    else if(htim == &htim16){ // 4Hz
    	// 的に当たった時、LEDを点滅させる
    	for(uint8_t i=0; i<NUMBER_OF_TARGET; i++){ // 的の数だけループ
        	if(led_blinking_count[i] < 4){
        		HAL_GPIO_TogglePin(led_pin[i].GPIOx, led_pin[i].GPIO_Pin);
        		led_blinking_count[i]++;
        	}
    	}
    }
}
/* Function Body End */
