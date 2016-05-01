#include <math.h>

#include "brake.h"
#include "clock.h"
#include "debug.h"
#include "encoder.h"
#include "fan.h"
#include "movement.h"
#include "pwm.h"
#include "utils.h"

uint8_t fan_status = 0;
uint8_t fan_up_flag = 0;
float fan_distance = 0;//（要走的）距离
float fan_position = 0;//（现在的）位置


void tim14_config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	
	TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 168 - 1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM14, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM8_TRG_COM_TIM14_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void start_fan(void)
{
	set_duty(FAN_CHANNEL, 0.065);
	fan_status = 1;
}

inline void stop_fan(void)
{
	set_duty(FAN_CHANNEL, 0.05);
	fan_status = 0;
}

inline void toggle_fan(void)
{
	if(fan_status != 0)
		stop_fan();
	else
		start_fan();
}


/*
	0xff means not working
	1 means going forwards
	0 means going backwards
*/


void fan_roll(float rad)
{
	set_duty(FAN_ROLL_CHANNEL, (float)((float)0.12 - (float)0.06 * rad / ((float)PI / 2)));
	
	#ifdef DEBUG
	printf("\nfan_roll(%f)\n", rad);
	#endif
}

void fan_roll_r(int8_t dir)
{
	set_duty(FAN_ROLL_CHANNEL, 0.13F + dir * 0.08F);
}

void fan_up(float speed)
{
	brake_release(0);
	if(speed > 10)
		set_duty(FAN_UPDOWN_CHANNEL, 0.065);
	else if(speed < -10)
		set_duty(FAN_UPDOWN_CHANNEL, 0.077);
	else set_duty(FAN_UPDOWN_CHANNEL, 0.071f - 0.0006f * speed);
}

void fan_up_r(void)
{
	fan_up(10);
	delay_ms(50);
	stop_fan_up_down();
}

void fan_down(float speed)
{
	brake_release(0);
	if(speed > 10)
		set_duty(FAN_UPDOWN_CHANNEL, 0.065);
	else if(speed < -10)
		set_duty(FAN_UPDOWN_CHANNEL, 0.077);
	else set_duty(FAN_UPDOWN_CHANNEL, 0.071f + 0.0006f * speed);
}

void fan_down_r(void)
{
	fan_down(10);
	delay_ms(50);
	stop_fan_up_down();
}

void stop_fan_up_down(void)
{
	set_duty(FAN_UPDOWN_CHANNEL, 0.071);
	brake(0);
}

void fan_up_auto(float dis)
{
	tim14_config();
	fan_up_flag = 1;
	fan_position = get_pos_fan();
	fan_distance = dis;
}

void fan_up_stop_auto(void)
{
	fan_up_flag = 0;
}
