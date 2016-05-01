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
	set_duty(FAN_ROLL_CHANNEL, (float)((float)0.14 - (float)0.06 * rad / ((float)PI / 2)));
	
	#ifdef DEBUG
	printf("\nfan_roll(%f)\n", rad);
	#endif
}

void fan_roll_r(int8_t dir)
{
	
	#ifdef DEBUG_FAN_ROLL_R
	printf("fan_roll_r(%d)\n", dir);
	#endif
	
	static float current_rad;
	current_rad += dir * FAN_RROLL_DIST;
	fan_roll(current_rad);
}

void fan_up(void)
{
	brake_release(0);
	set_duty(FAN_UPDOWN_CHANNEL, 0.063);
}

void fan_up_r(void)
{
	fan_up();
	delay_ms(50);
	stop_fan_up_down();
}

void fan_down(void)
{
	brake_release(0);
	set_duty(FAN_UPDOWN_CHANNEL, 0.074);
}

void fan_down_r(void)
{
	fan_down();
	delay_ms(50);
	stop_fan_up_down();
}

void stop_fan_up_down(void)
{
	set_duty(FAN_UPDOWN_CHANNEL, 0.071);
	brake(0);
}
