#include "stm32f4xx_iwdg.h"

#include "usart.h"

void tim13_config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);

	TIM_TimeBaseInitStructure.TIM_Period = 168 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 30000 - 1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	
	TIM_TimeBaseInit(TIM13, &TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM13, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM13, ENABLE);


	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void watchdog_config(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetReload(0x80);
	IWDG_SetPrescaler(IWDG_Prescaler_32);
	IWDG_Enable();
	tim13_config();
}
