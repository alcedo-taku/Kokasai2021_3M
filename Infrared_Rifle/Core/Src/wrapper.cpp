#include "wrapper.hpp"

/* Include Begin */
#include "gpio.h"
#include "tim.h"
#if ORIGINAL_UART
#include "usart.h"
#endif
#include <bitset>
/* Include End */

#define ENABLE_DEBUG 0
#define ORIGINAL_UART 0

/* Enum Begin */
/* Enum End */

/* Struct Begin */
struct GPIO{
	GPIO_TypeDef * GPIOx;
	uint16_t GPIO_Pin;
};
GPIO indicator_LED[5] = {
		GPIO{GPIOA, GPIO_PIN_7},
		GPIO{GPIOA, GPIO_PIN_4},
		GPIO{GPIOA, GPIO_PIN_1},
		GPIO{GPIOA, GPIO_PIN_0},
		GPIO{GPIOC, GPIO_PIN_15}
};
/* Struct End */

/* Variable Begin */
constexpr uint8_t rifle_id = 3;
//const uint8_t number_of_high_bits = std::bitset<8>(rifle_id).count();
constexpr uint8_t number_of_high_bits = 2;
constexpr uint16_t NUMBER_OF_BULLETS = 1000;
uint16_t number_of_bullets_remaining = NUMBER_OF_BULLETS;
/* Variable End */

/* Class Constructor Begin */
/* Class Constructor End */

/* Function Prototype Begin */
/* Function Prototype End */

void init(void){
#if !ENABLE_DEBUG
	__HAL_TIM_SET_COMPARE(&htim21, TIM_CHANNEL_1, 421); // PWMを使った赤外線通信 送信
#else
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); // GPIOを使ったUART 送信
#endif
	for(uint8_t i=0; i<5; i++){
		HAL_GPIO_WritePin(indicator_LED[i].GPIOx, indicator_LED[i].GPIO_Pin, GPIO_PIN_SET);
	}
	HAL_TIM_Base_Start_IT(&htim2);
}

void loop(void){
}

/* Function Body Begin */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if(htim == &htim2){ // 1200Hz
    	static bool is_in_connecting = false;

    	// トリガー読み取り処理
		static uint8_t prescaler = 0;
		if( 120 <= prescaler ){ // 10Hzに分周
			static GPIO_PinState prv_buttun_status;

			if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14) == GPIO_PIN_RESET && prv_buttun_status == GPIO_PIN_SET && number_of_bullets_remaining != 0 ){
#if ORIGINAL_UART
		HAL_UART_Transmit(&huart2, (uint8_t *)&rifle_id, sizeof(rifle_id), 500);
#endif
//				HAL_TIM_Base_Start_IT(&htim2); // UART通信を開始
				is_in_connecting = true;
			}
			prv_buttun_status = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14);
			prescaler = 0;
		}
		prescaler++;

		// 発射処理
    	if(is_in_connecting){
			static int8_t uart_step = -1;
			static uint8_t number_of_rapid_fires = 40; // 1回に打つ回数
			if(uart_step == -1){ // スタートビット
	#if !ENABLE_DEBUG
				HAL_TIM_PWM_Start(&htim21, TIM_CHANNEL_1); // パルスを出す
	#else
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	#endif
				uart_step++;
			}
			else if(0 <= uart_step && uart_step <= 7){ // データﾋﾞｯﾄ
	#if !ENABLE_DEBUG
				( rifle_id&(1<<uart_step) ) >> uart_step ? // 送信ﾋﾞｯﾄが
						HAL_TIM_PWM_Stop(&htim21, TIM_CHANNEL_1): // 1ならパルスを止める
						HAL_TIM_PWM_Start(&htim21, TIM_CHANNEL_1); // 0ならパルスを出す
	#else
				( rifle_id&(1<<uart_step) ) >> uart_step ? // 送信ﾋﾞｯﾄが
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET):
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	#endif
				uart_step++;
			}
			else if(uart_step == 8){ // parity even(偶数)
	#if !ENABLE_DEBUG
				number_of_high_bits%2 ? // 1のﾋﾞｯﾄが
						HAL_TIM_PWM_Stop(&htim21, TIM_CHANNEL_1): // 奇数個ならパルスを止める
						HAL_TIM_PWM_Start(&htim21, TIM_CHANNEL_1); // 偶数個ならパルスを出す
	#else
				number_of_high_bits%2 ? // 1のﾋﾞｯﾄが
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET): // 奇数個なら
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // 偶数個なら
	#endif
				uart_step++;
			}
			else if(uart_step == 9){ // ストップビット
	#if !ENABLE_DEBUG
				HAL_TIM_PWM_Stop(&htim21, TIM_CHANNEL_1); // パルスを止める
	#else
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	#endif
				uart_step = -5; // 段階を初期化
				if(number_of_bullets_remaining != 0)
					number_of_bullets_remaining--;
//				HAL_TIM_Base_Stop_IT(&htim2); // タイマー割込みを止める
				if(number_of_rapid_fires != 0)
					number_of_rapid_fires--;
				if(number_of_rapid_fires == 0){
					is_in_connecting = false;
					number_of_rapid_fires = 40;
				}

			}
			else{
				uart_step++;
			}

			// 残り弾数のインジゲータ
			if(number_of_bullets_remaining == 0){
				HAL_GPIO_WritePin(indicator_LED[4].GPIOx, indicator_LED[4].GPIO_Pin, GPIO_PIN_RESET);
			}else if(number_of_bullets_remaining < NUMBER_OF_BULLETS*1/5.0){
				HAL_GPIO_WritePin(indicator_LED[3].GPIOx, indicator_LED[3].GPIO_Pin, GPIO_PIN_RESET);
			}else if(number_of_bullets_remaining < NUMBER_OF_BULLETS*2/5.0){
				HAL_GPIO_WritePin(indicator_LED[2].GPIOx, indicator_LED[2].GPIO_Pin, GPIO_PIN_RESET);
			}else if(number_of_bullets_remaining < NUMBER_OF_BULLETS*3/5.0){
				HAL_GPIO_WritePin(indicator_LED[1].GPIOx, indicator_LED[1].GPIO_Pin, GPIO_PIN_RESET);
			}else if(number_of_bullets_remaining < NUMBER_OF_BULLETS*4/5.0){
				HAL_GPIO_WritePin(indicator_LED[0].GPIOx, indicator_LED[0].GPIO_Pin, GPIO_PIN_RESET);
			}
    	}// is_in_connecting
    }
}
/* Function Body End */
