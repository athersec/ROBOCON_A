#include <stdbool.h>
#include <stdio.h>

#include "misc.h"
#include "stm32f4xx_gpio.h"

#include "clock.h"
#include "suart.h"

#define SU_BUFFER_LEN 512

#define RXB(i) GPIO_ReadInputDataBit(su_rx_ports[i], su_rx_pins[i])

typedef unsigned char byte;
typedef unsigned int word;

uint16_t su_tx_pins[SU_CHANNEL_NUM] = {GPIO_Pin_0, GPIO_Pin_13};
GPIO_TypeDef *su_tx_ports[SU_CHANNEL_NUM] = {GPIOB, GPIOF};

uint16_t su_rx_pins[SU_CHANNEL_NUM] = {GPIO_Pin_1, GPIO_Pin_15};
GPIO_TypeDef *su_rx_ports[SU_CHANNEL_NUM] = {GPIOB, GPIOF};

byte TBUF[SU_CHANNEL_NUM] = {0}, RBUF[SU_CHANNEL_NUM] = {0};
byte TDAT[SU_CHANNEL_NUM] = {0}, RDAT[SU_CHANNEL_NUM] = {0};
byte TCNT[SU_CHANNEL_NUM] = {0}, RCNT[SU_CHANNEL_NUM] = {0};
byte TBIT[SU_CHANNEL_NUM] = {0}, RBIT[SU_CHANNEL_NUM] = {0};
bool TING[SU_CHANNEL_NUM] = {0}, RING[SU_CHANNEL_NUM] = {0};
bool TEND[SU_CHANNEL_NUM] = {1, 1}, REND[SU_CHANNEL_NUM] = {1, 1};

void suart_config(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);


	GPIO_InitTypeDef GPIO_InitStructure;

	for(uint8_t i = 0; i < SU_CHANNEL_NUM; i++) {
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Pin = su_tx_pins[i];
		GPIO_Init(su_tx_ports[i], &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Pin = su_rx_pins[i];
		GPIO_Init(su_rx_ports[i], &GPIO_InitStructure);
	}


	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11, ENABLE);
	
	TIM_TimeBaseInitStructure.TIM_Period = 729 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 8 - 1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	
	TIM_TimeBaseInit(TIM11, &TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM11, TIM_FLAG_Update);
	
	TIM_ITConfig(TIM11, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM11, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_TRG_COM_TIM11_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void send_uart(uint8_t channel, char data)
{
	if(TEND[channel])
	{
		TEND[channel] = 0;
		TBUF[channel] = data;
		TING[channel] = 1;
	}
}

void suputchar(uint8_t channel, char c)
{
	while(!TEND[channel]);
	send_uart(channel, c);
}

char receive_uart(uint8_t channel)
{
	char tmp = RBUF[channel];
	REND[channel] = 0;
	return tmp;
}

char sugetchar(uint8_t channel)
{
	while(!REND[channel]);
	return receive_uart(channel);
}

void suprintf(uint8_t channel, char *fmt, ...)
{

	char buffer[SU_BUFFER_LEN + 1];
	u8 i = 0;
	
	va_list arg_ptr;
	
	va_start(arg_ptr, fmt);  
	
	vsnprintf(buffer, SU_BUFFER_LEN + 1, fmt, arg_ptr);
	
	while ((i < SU_BUFFER_LEN) && buffer[i])
	{
		suputchar(channel, (uint8_t)buffer[i++]); 
	}
	
	va_end(arg_ptr);
	
	delay_ms(2);
}

void suart_check(void)
{
	for(uint8_t i = 0; i < SU_CHANNEL_NUM; i++) {
		if(RING[i])
		{
			if(--RCNT[i] == 0)
			{
				RCNT[i] = 3;
				if(--RBIT[i] == 0)
				{
					RBUF[i] = RDAT[i];
					RING[i] = 0;
					REND[i] = 1;
				}
				else
				{
					RDAT[i] >>= 1;
					if(RXB(i))
						RDAT[i] |= 0x80;
				}
			}
		}
		else if(!RXB(i))
		{
			RING[i] = 1;
			RCNT[i] = 4;
			RBIT[i] = 9;
		}

		if(--TCNT[i] == 0)
		{
			TCNT[i] = 3;
			if(TING[i])
			{
				if(TBIT[i] == 0)
				{
					GPIO_WriteBit(su_tx_ports[i], su_tx_pins[i], Bit_RESET);
					TDAT[i] = TBUF[i];
					TBIT[i] = 9;
				}
				else
				{
					if(--TBIT[i] == 0)
					{
						GPIO_WriteBit(su_tx_ports[i], su_tx_pins[i], Bit_SET);
						TING[i] = 0;
						TEND[i] = 1;
					}
					else
					{
						if(TDAT[i] & 0x01)
							GPIO_WriteBit(su_tx_ports[i], su_tx_pins[i], Bit_SET);
						else
							GPIO_WriteBit(su_tx_ports[i], su_tx_pins[i], Bit_RESET);
						TDAT[i] >>= 1;
					}
				}
			}
		}
	}
}
