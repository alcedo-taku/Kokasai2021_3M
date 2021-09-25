#include "wrapper.hpp"

/* Include Begin */
#include "gpio.h"
#include "usart.h"
#include "tim.h"
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
	__HAL_TIM_SET_COMPARE(&htim21, TIM_CHANNEL_1, 421); // PWMを使った赤外線通信 送信
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); // GPIOを使ったUART 送信
}

void loop(void){
	if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14) == GPIO_PIN_RESET ){
//		HAL_UART_Transmit(&huart2, (uint8_t *)&rifle_id, sizeof(rifle_id), 500);
		HAL_TIM_Base_Start_IT(&htim2); // UART通信を開始
	}else{
	}
}

/* Function Body Begin */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if(htim == &htim2){
    	static int8_t uart_step = -1;
    	if(uart_step == -1){ // スタートビット
    		HAL_TIM_PWM_Start(&htim21, TIM_CHANNEL_1); // パルスを出す
    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
    		uart_step++;
    	}else if(0 <= uart_step && uart_step <= 7){ // データﾋﾞｯﾄ
    		if( ( rifle_id&(1<<uart_step) ) >> uart_step == 0){ // 送信ﾋﾞｯﾄが0だったら
    			HAL_TIM_PWM_Start(&htim21, TIM_CHANNEL_1); // パルスを出す
    			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
    		}else{ // 送信ﾋﾞｯﾄが1だったら
    			HAL_TIM_PWM_Stop(&htim21, TIM_CHANNEL_1); // パルスを止める
    			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
    		}
    		uart_step++;
    	}else{ // ストップビット
    		HAL_TIM_PWM_Stop(&htim21, TIM_CHANNEL_1); // パルスを止める
    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
    		uart_step = -1; // 段階を初期化
    		HAL_TIM_Base_Stop_IT(&htim2); // タイマー割込みを止める
    	}
    }
}
/* Function Body End */
