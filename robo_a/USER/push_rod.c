#include <stdint.h>

#include "stm32f4xx_tim.h"
#include "clock.h"
#include "push_rod.h"

/*if push and pull error, exchange this array*/
// 0 PG14/15 靠近核心板
// 1 PG11/13 板子少的一边

uint16_t PUSHPins[2 * PUSH_ROD_CHANNEL_NUM] = {\
	GPIO_Pin_1, GPIO_Pin_0,\
	GPIO_Pin_13, GPIO_Pin_11\
	};

GPIO_TypeDef *PUSHPorts[2 * PUSH_ROD_CHANNEL_NUM] = {\
	GPIOE, GPIOE,\
	GPIOG, GPIOG\
	};


void push_rod_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

	for(uint8_t i = 0; i < 2* PUSH_ROD_CHANNEL_NUM; i++) {
		GPIO_InitStructure.GPIO_Pin = PUSHPins[i];
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(PUSHPorts[i], &GPIO_InitStructure);
	}


	for(uint8_t i = 0; i < 2* PUSH_ROD_CHANNEL_NUM; i++) {
		GPIO_WriteBit(PUSHPorts[i], PUSHPins[i], Bit_RESET);
	}
}

void push_rod_c(uint8_t dir, uint8_t num)
{
	switch(dir){
		case 0:{
			GPIO_WriteBit(PUSHPorts[2 * num], PUSHPins[2 * num], Bit_SET);
			GPIO_WriteBit(PUSHPorts[2 * num + 1], PUSHPins[2 * num + 1], Bit_RESET);
			break;
		}
		case 1:{
			GPIO_WriteBit(PUSHPorts[2 * num], PUSHPins[2 * num], Bit_RESET);
			GPIO_WriteBit(PUSHPorts[2 * num + 1], PUSHPins[2 * num + 1], Bit_SET);
			break;
		}
		case 0xff:{
			GPIO_WriteBit(PUSHPorts[2 * num], PUSHPins[2 * num], Bit_RESET);
			GPIO_WriteBit(PUSHPorts[2 * num + 1], PUSHPins[2 * num + 1], Bit_RESET);
			break;
		}
		default:break;
	}
}

void push_rod(uint8_t dir, uint8_t num)
{
	switch(dir){
		case 0:{
			push_rod_c(PUSH_ROD_PULL, num);
			if(1 == num) delay_ms(PUSH_ROD_TIME);
			else delay_ms(PUSH_ROD_TIME / 5);
			break;
		}
		case 1:{
			push_rod_c(PUSH_ROD_PUSH, num);
			if(1 == num) delay_ms(PUSH_ROD_TIME);
			else delay_ms(PUSH_ROD_TIME / 5);
			break;
		}
		case 0xff:{
			push_rod_c(PUSH_ROD_STOP, num);
			break;
		}
		default:break;
	}
	push_rod_c(PUSH_ROD_STOP, num);
}
