#include "misc.h"
#include "stm32f4xx_gpio.h"

#include "switch.h"
/*
	0 PE8  fan_down_stop
	1 PF11 fan_up_stop
	2 PF3  light_electricity(12V)
*/

const uint32_t SwitchAHBPorts[SWITCH_CHANNEL_NUM] = {\
				RCC_AHB1Periph_GPIOE, RCC_AHB1Periph_GPIOF,\
				RCC_AHB1Periph_GPIOF\
				};
const uint16_t SwitchPins[SWITCH_CHANNEL_NUM] = {\
				GPIO_Pin_8, GPIO_Pin_11,\
				GPIO_Pin_3\
				};
GPIO_TypeDef *SwitchPorts[SWITCH_CHANNEL_NUM] = {\
				GPIOE, GPIOF,\
				GPIOF\
				};
const uint8_t SwitchEXTIPorts[SWITCH_CHANNEL_NUM] = {\
				EXTI_PortSourceGPIOE, EXTI_PortSourceGPIOF,\
				EXTI_PortSourceGPIOF\
				};
const uint8_t SwitchEXTIPinsources[SWITCH_CHANNEL_NUM] = {\
				EXTI_PinSource8, EXTI_PinSource11,\
				EXTI_PinSource3\
				};
const uint32_t SwitchEXTILines[SWITCH_CHANNEL_NUM] = {\
				EXTI_Line8, EXTI_Line11,\
				EXTI_Line3\
				};
const enum IRQn SwitchNVICPins[SWITCH_CHANNEL_NUM] = {\
				EXTI9_5_IRQn, EXTI15_10_IRQn,\
				EXTI3_IRQn\
				};


void switch_gpio_config(void)
{
	uint8_t i;
	GPIO_InitTypeDef  GPIO_InitStructure;

	for(i = 0; i < SWITCH_CHANNEL_NUM; i++) {
		RCC_AHB1PeriphClockCmd(SwitchAHBPorts[i], ENABLE);
	}

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	for(i = 0; i < SWITCH_CHANNEL_NUM; i++) {
		GPIO_InitStructure.GPIO_Pin = SwitchPins[i];
		GPIO_Init(SwitchPorts[i], &GPIO_InitStructure);
	}
}

void switch_exti_config(void)
{
	uint8_t i;
	EXTI_InitTypeDef  EXTI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	for(i = 0; i < SWITCH_CHANNEL_NUM; i++) {
		SYSCFG_EXTILineConfig(SwitchEXTIPorts[i],  SwitchEXTIPinsources[i]);
	}

	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	for(i = 0; i < SWITCH_CHANNEL_NUM; i++) {
		EXTI_InitStructure.EXTI_Line = SwitchEXTILines[i];
		EXTI_Init(&EXTI_InitStructure);
	}
}

void switch_nvic_config(void)
{
	uint8_t i;
	NVIC_InitTypeDef  NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	for(i = 0; i < SWITCH_CHANNEL_NUM; i++) {
		NVIC_InitStructure.NVIC_IRQChannel = SwitchNVICPins[i];
		NVIC_Init(&NVIC_InitStructure);
	}

}
void switch_config(void)
{
	switch_gpio_config();
	switch_exti_config();
	switch_nvic_config();
}
