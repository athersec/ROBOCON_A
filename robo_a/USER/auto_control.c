#include <stdint.h>

#include "automove.h"
#include "auto_control.h"
#include "clock.h"
#include "fan.h"
#include "interpreter.h"
#include "magnet.h"
#include "movement.h"
#include "mti.h"
#include "push_rod.h"
#include "pwm.h"
#include "whiteline.h"

bool auto_flag = false;
float old_pos_x = 0, old_pos_y = 0, old_rad = 0;
float dest_pos_x = 0, dest_pos_y = 0, dest_rad = 0;

struct coordinate_t {
	float x;
	float y;
	float rad;
	void (*callback)(void);
};

void start_fan_1(void)
{
	start_fan();
	
	delay_ms(2000);
	
	automove_flag = false;
	wl_run();
	automove_flag = true;
	
	set_auto_dest(get_gps_x(), get_gps_y(), get_gps_rad());
	while(!near_auto_dest());
	
	set_auto_dest(get_gps_x(), get_gps_y() - 0.2f, - PI - 0.2f);
	while(!near_auto_dest());
	
	set_auto_dest(7.0f, get_gps_y()- 0.2f, - PI - 0.25f);
	while(!near_auto_dest());
}

void fan_up_1(void)
{
	fan_up_auto(0.2f);
}

void fan_up_2(void)
{
	fan_up_auto(0.2f);
}

void fan_up_3(void)
{
	fan_up_auto(0.1f);
}

void roll_fan_1(void)
{
	stop_fan();
	set_duty(2, 0.05f);
	fan_up_stop_auto();
	delay_ms(300);
	set_duty(FAN_CHANNEL, 0.06);
	delay_ms(1700);
	set_duty(FAN_CHANNEL, 0.065);
	delay_ms(1000);
	stop_fan();
}

void line_1(void)
{
	automove_disable();
}

struct coordinate_t coord[] = {
	{2812.9, 0, 0, start_fan_1},\
	/*
	{2812.9, 1072.85, PI/12, fan_up_1},\
	{2550.44, 2133.29, PI/6},\
	{2130.4, 2860.83, PI/4, fan_up_2},\
	{1361.84, 3647.91, PI/6},\
	{800, 5090.82, PI/12, fan_up_3},\
	{700, 6734.66, 0, roll_fan_1},\
	{700, 7000, 0, line_1},\
	*/
	{0, 0, 0}
};

void auto_start(void)
{
	automove_config();
	automove_flag = true;

	printf("\nAuto controlling.\n");
	auto_flag = true;

	for(int i = 0; 0 != coord[i].x || 0 != coord[i].y || 0 != coord[i].rad || 0 != coord[i].callback; i++) {
		set_auto_dest(coord[i].x / 1000, coord[i].y / 1000, coord[i].rad);
		while(!near_auto_dest());
		if(0 != coord[i].callback)
			(coord[i].callback)();
	}
}

void auto_stop(void)
{
	stop_all();
	stop_fan();

	auto_flag = false;
	
	printf("\nAuto control stopped.\n");
	while(1) {
		check_cmd();
	}
}

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


void (*auto_steps[])(void) = {auto_start, auto_stop, 0};

void auto_control(void)
{
	tim14_config();

	for(uint16_t i = 0; auto_steps[i] != 0; i++) {
		auto_steps[i]();
	}
}
