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

void fan_down_1(void)
{
	fan_down(10);
}

struct coordinate_t coord[] = {
	{2812.9, 0, 0, start_fan_1},\
	{2812.9, 1072.85, PI/12, fan_up_1},\
	{2550.44, 2133.29, PI/6},\
	{2130.4, 2860.83, PI/4, fan_up_2},\
	{1361.84, 3647.91, PI/6},\
	{800, 5090.82, PI/12, fan_up_3},\
	{700, 6734.66, 0, roll_fan_1},\
	{700, 9500, 0, fan_down_1},\
	{1500, 10500, PI},\
	{4960, 11800, PI},\
	{5900, 11800, PI - 0.2f},\
	{6500, 11800, PI - 0.4f},\
	{7100, 11800, PI - 0.6f},\
	{0, 0, 0}
};

void auto_start(void)
{
	automove_config();

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

void (*auto_steps[])(void) = {auto_start, auto_stop, 0};

void auto_control(void)
{
	for(uint16_t i = 0; auto_steps[i] != 0; i++) {
		auto_steps[i]();
	}
}
