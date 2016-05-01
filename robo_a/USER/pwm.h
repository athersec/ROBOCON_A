#include <stdint.h>

#include "stm32f4xx_gpio.h"

// #define USE_HPWM

#define PWM_PRESCALE (84 - 1)

#ifdef USE_HPWM
#include "hpwm.h"
#endif

#ifndef USE_HPWM

#define PWM_FREQ 100000UL
#define PWM_CHANNEL_NUM 5

extern uint32_t PWMHighTime[PWM_CHANNEL_NUM];
extern uint32_t PWMTotal[PWM_CHANNEL_NUM];
extern uint16_t PWMPins[PWM_CHANNEL_NUM];
extern GPIO_TypeDef *PWMPorts[PWM_CHANNEL_NUM];


void set_duties(float duties[PWM_CHANNEL_NUM]);
void set_freq(uint8_t channel, unsigned long freq);
void set_freqs(unsigned long freqs[PWM_CHANNEL_NUM]);

#endif

void pwm_config(void);

/*
	0	TIM1_CH1	PA8
	1	TIM1_CH2	PA9
	2	TIM1_CH3	PA10
	3	TIM1_CH4	PA11
	4	TIM8_CH1	PC6
	5	TIM8_CH2	PC7
	6	TIM8_CH3	PC8
	7	TIM8_CH4	PC9
*/
void set_duty(uint8_t channel, float duty);
