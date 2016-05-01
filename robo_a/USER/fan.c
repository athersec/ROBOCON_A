#include <math.h>

#include "auto_control.h"
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
float fan_height = 0;
float fan_up_length = 0;
pid_t fp;


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
	set_duty(FAN_ROLL_CHANNEL, (float)((float)0.12 - (float)0.05 * rad / ((float)PI / 2)));
	
	#ifdef DEBUG
	printf("\nfan_roll(%f)\n", rad);
	#endif
}

void fan_roll_r(int8_t dir)
{
	set_duty(FAN_ROLL_CHANNEL, 0.06F + dir * 0.06F);
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

void fan_up_auto(float pos)
{
	fan_up_flag = 1;
	fan_height = get_pos_fan();
	fan_up_length = pos;
	fp.kp = 20;
	fp.ki = 0;
	fp.kd = 0;
	pid_config(&fp);
	fp.set_value = fan_height + pos;
}

void fan_up_stop_auto(void)
{
	fan_up_flag = 0;
}
