#include <math.h>

#include "stm32f4xx_gpio.h"

#include "encoder.h"
#include "movement.h"


int32_t g_rotary_x = 0;
int32_t g_rotary_y = 0;
int32_t g_rotary_fan = 0;
int32_t g_rotary_fan_updown = 0;
int32_t g_rotary_magnet = 0;
int32_t g_rotary_tim8 = 0;


const uint32_t EncoderAPB1TIMs[ENCODER_CHANNEL_NUM] = {\
				RCC_APB1Periph_TIM3,\
				RCC_APB1Periph_TIM4, RCC_APB1Periph_TIM5\
				};
const uint32_t EncoderAHBPorts[ENCODER_CHANNEL_NUM] = {\
				RCC_AHB1Periph_GPIOA, RCC_AHB1Periph_GPIOC,\
				RCC_AHB1Periph_GPIOD, RCC_AHB1Periph_GPIOE\
				};
GPIO_TypeDef * EncoderPorts[2 * ENCODER_CHANNEL_NUM] = {\
				GPIOA, GPIOA,\
				GPIOA, GPIOA,\
				GPIOD, GPIOD,\
				GPIOA, GPIOA,\
				GPIOC, GPIOC\
				};
const uint16_t EncoderPinsources[2 * ENCODER_CHANNEL_NUM] = {\
				GPIO_PinSource8, GPIO_PinSource9,\
				GPIO_PinSource6, GPIO_PinSource7,\
				GPIO_PinSource12, GPIO_PinSource13,\
				GPIO_PinSource0, GPIO_PinSource1,\
				GPIO_PinSource6, GPIO_PinSource7\
				};
const uint8_t EncoderAFTIMs[2 * ENCODER_CHANNEL_NUM] = {\
				GPIO_AF_TIM1, GPIO_AF_TIM1,\
				GPIO_AF_TIM3, GPIO_AF_TIM3,\
				GPIO_AF_TIM4, GPIO_AF_TIM4,\
				GPIO_AF_TIM5, GPIO_AF_TIM5,\
				GPIO_AF_TIM8, GPIO_AF_TIM8\
				};
const uint16_t EncoderPins[2 * ENCODER_CHANNEL_NUM] = {\
				GPIO_Pin_8, GPIO_Pin_9,\
				GPIO_Pin_6, GPIO_Pin_7,\
				GPIO_Pin_12, GPIO_Pin_13,\
				GPIO_Pin_0, GPIO_Pin_1,\
				GPIO_Pin_6, GPIO_Pin_7\
				};
TIM_TypeDef * EncoderTIMs[ENCODER_CHANNEL_NUM] = {\
				TIM1, TIM3,\
				TIM4, TIM5, TIM8\
				};


void encoder_test(void)
{
	printf("\r\n%d	%d	%d	%d	%d\n", g_rotary_magnet, g_rotary_fan, g_rotary_x, g_rotary_y, g_rotary_tim8);
}

/*
	meter
*/
float get_pos_x(void)
{
	return (float)((double) g_rotary_x / 2000 * W_DIAMETER * PI * VECT_EX);
}

/*
	meter
*/
float get_pos_y(void)
{
	return (float)((double)g_rotary_y / 2000 * W_DIAMETER * PI * VECT_EY);
}

/*
	100 degree
*/
float get_pos_fan(void)
{
	return (float)((double)g_rotary_fan / 2000 * PI * VECT_EFAN);
}

/*
	meter
*/
float get_pos_magnet(void)
{
	return (float)((double)g_rotary_magnet / 2000 *  W_DIAMETER * PI * VECT_EMAGNET);
}

/*
	meter
*/
float get_pos_tim8(void)
{
	return (float)((double)g_rotary_tim8 / 2000 *  W_DIAMETER * PI * VECT_ETIM8);
}

void timer7_config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;


	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);


	TIM_TimeBaseStructure.TIM_Prescaler = 168 - 1;	
	TIM_TimeBaseStructure.TIM_Period = 1000 - 1;	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);

	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM7, ENABLE);	


	NVIC_InitStructure.NVIC_IRQChannel=TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


void encoder_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	for(int8_t i = 0; i < ENCODER_CHANNEL_NUM - 2; i++) {
		RCC_APB1PeriphClockCmd(EncoderAPB1TIMs[i], ENABLE);
	}
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);


	for(int8_t i = 0; i < 5; i++) {
		RCC_AHB1PeriphClockCmd(EncoderAHBPorts[i], ENABLE);
	}


	for(int8_t i = 0; i < 2 * ENCODER_CHANNEL_NUM; i++) {
		GPIO_PinAFConfig(EncoderPorts[i], EncoderPinsources[i], EncoderAFTIMs[i]);
	}


	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	for(int8_t i = 0; i < 2 * ENCODER_CHANNEL_NUM; i++) {
		GPIO_InitStructure.GPIO_Pin = EncoderPins[i];
		GPIO_Init(EncoderPorts[i], &GPIO_InitStructure);
	}


	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0;
	TIM_TimeBaseStructure.TIM_Period = 29999;	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 6;
	for(int8_t i = 0; i < ENCODER_CHANNEL_NUM; i++) {
		TIM_TimeBaseInit(EncoderTIMs[i], &TIM_TimeBaseStructure);
		TIM_EncoderInterfaceConfig(EncoderTIMs[i],TIM_EncoderMode_TI12,TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
		TIM_ICInit(EncoderTIMs[i], &TIM_ICInitStructure);
		EncoderTIMs[i]->CNT=4000;
		TIM_ClearFlag(EncoderTIMs[i], TIM_FLAG_Update);
		TIM_ITConfig(EncoderTIMs[i], TIM_IT_Update, ENABLE);
		TIM_Cmd(EncoderTIMs[i], ENABLE);
	}


	timer7_config();
}
