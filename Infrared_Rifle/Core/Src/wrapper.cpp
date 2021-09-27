#include "wrapper.hpp"

/* Include Begin */
#include "gpio.h"
#include "usart.h"
#include "tim.h"
#include <bitset>
/* Include End */

#define debug 0

/* Enum Begin */
/* Enum End */

/* Struct Begin */
/* Struct End */

/* Variable Begin */
constexpr uint8_t rifle_id = 3;
//const uint8_t number_of_high_bits = std::bitset<8>(rifle_id).count();
constexpr uint8_t number_of_high_bits = 2;
/* Variable End */

/* Class Constructor Begin */
/* Class Constructor End */

/* Function Prototype Begin */
/* Function Prototype End */

void init(void){
#if !debug
	__HAL_TIM_SET_COMPARE(&htim21, TIM_CHANNEL_1, 421); // PWMを使った赤外線通信 送信
#else
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); // GPIOを使ったUART 送信
#endif
}

void loop(void){
	if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14) == GPIO_PIN_RESET ){
//		HAL_UART_Transmit(&huart2, (uint8_t *)&rifle_id, sizeof(rifle_id), 500);
		HAL_TIM_Base_Start_IT(&htim2); // UART通信を開始
	}
}

/* Function Body Begin */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if(htim == &htim2){
    	static int8_t uart_step = -1;
    	if(uart_step == -1){ // スタートビット
#if !debug
    		HAL_TIM_PWM_Start(&htim21, TIM_CHANNEL_1); // パルスを出す
#else
    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
#endif
    		uart_step++;
    	}else if(0 <= uart_step && uart_step <= 7){ // データﾋﾞｯﾄ
    		if( ( rifle_id&(1<<uart_step) ) >> uart_step == 0){ // 送信ﾋﾞｯﾄが0だったら
#if !debug
    			HAL_TIM_PWM_Start(&htim21, TIM_CHANNEL_1); // パルスを出す
#else
    			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
#endif
    		}else{ // 送信ﾋﾞｯﾄが1だったら
#if !debug
    			HAL_TIM_PWM_Stop(&htim21, TIM_CHANNEL_1); // パルスを止める
#else
    			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
#endif
    		}
    		uart_step++;
    	}else if(uart_step == 8){ // parity even(偶数)
    		if( number_of_high_bits%2 ){ // 1のﾋﾞｯﾄが奇数個だったら
#if !debug
    			HAL_TIM_PWM_Stop(&htim21, TIM_CHANNEL_1); // パルスを止める
#else
    			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
#endif
    		}else{ // 1のﾋﾞｯﾄが偶数個だったら
#if !debug
    			HAL_TIM_PWM_Start(&htim21, TIM_CHANNEL_1); // パルスを出す
#else
    			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
#endif
    		}
    		uart_step++;
    	}else if(uart_step == 9){ // ストップビット
#if !debug
    		HAL_TIM_PWM_Stop(&htim21, TIM_CHANNEL_1); // パルスを止める
#else
    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
#endif
    		uart_step = -5; // 段階を初期化
    		HAL_TIM_Base_Stop_IT(&htim2); // タイマー割込みを止める
    	}else{
    		uart_step++;
    	}
    }
}
/* Function Body End */
