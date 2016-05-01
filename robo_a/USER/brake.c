#include <stdio.h>

#include "stm32f4xx_gpio.h"

#include "brake.h"
#include "debug.h"

uint16_t brake_pins[BRAKE_CHANNEL_NUM] = {GPIO_Pin_12};

void brake_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	for(uint8_t i = 0; i < 3;i++) {
		GPIO_InitStructure.GPIO_Pin = brake_pins[i];
		GPIO_Init(GPIOG, &GPIO_InitStructure);
	}
	
	for(uint8_t i = 0; i < BRAKE_CHANNEL_NUM; i++) {
		GPIO_WriteBit(GPIOG, brake_pins[i], Bit_SET);
	}
}

void brake(uint8_t channel)
{
	GPIO_WriteBit(GPIOG, brake_pins[channel], Bit_SET);
	
	#ifdef DEBUG_BRAKE
	printf("brake(%d)\n", channel);
	#endif
}

void brake_release(uint8_t channel)
{
	GPIO_WriteBit(GPIOG, brake_pins[channel], Bit_RESET);
	
	#ifdef DEBUG_BRAKE
	printf("brake_release(%d)\n", channel);
	#endif
}
