#include <math.h>
#include <stdlib.h>

#include "clock.h"
#include "debug.h"
#include "encoder.h"
#include "movement.h"
#include "utils.h"

#ifdef USE_THREE_WHEEL

int16_t arg_speeds[3] = {0};

void t_stop(void)
{
	arg_speeds[0] = 0;
	arg_speeds[1] = 0;
	arg_speeds[2] = 0;
	
	uprintf(USART1,\
		"\r0V%d\r1V%d\r2V%d\r",\
		arg_speeds[0],\
		arg_speeds[1],\
		arg_speeds[2]\
		);
}

void t_rotate_c(int8_t spd)
{
	int16_t arg_spd = (float) spd / 128 * DEFAULT_ARG_SPEED;
	arg_speeds[0] = -VECT_W0 * arg_spd;
	arg_speeds[1] = VECT_W1 * arg_spd;
	arg_speeds[2] = VECT_W2 * arg_spd;
	
	#ifdef DEBUG
	printf("arg_speeds:\n");
	printf("\t0:%d\n", arg_speeds[0]);
	printf("\t1:%d\n", arg_speeds[1]);
	printf("\t2:%d\n", arg_speeds[2]);
	#endif

	uprintf(USART1,\
		"\r0V%d\r1V%d\r2V%d\r",\
		arg_speeds[0],\
		arg_speeds[1],\
		arg_speeds[2]\
		);
}

void t_move_xy_c(int8_t spd_x, int8_t spd_y)
{
	int16_t arg_x, arg_y;

	arg_x = (int16_t) (((float) spd_x) / 128 * DEFAULT_ARG_SPEED * 3);
	arg_y = (int16_t) (((float) spd_y) / 128 * DEFAULT_ARG_SPEED * 8);

	arg_speeds[0] = VECT_W0 * ( 0 + arg_y / 1.68 );// 4.071 );		//4.071是计算结果，1.68是调试结果；可走x/y方向直线
	arg_speeds[1] = VECT_W1 * (   arg_x + arg_y / 2.296 );
	arg_speeds[2] = VECT_W2 * ( - 1.07 * arg_x + arg_y / 2.296 );	//1是计算结果，1.07是调试结果

	#ifdef DEBUG
	printf("arg_speeds:\n");
	printf("\t0:%d\n", arg_speeds[0]);
	printf("\t1:%d\n", arg_speeds[1]);
	printf("\t2:%d\n", arg_speeds[2]);
	#endif

	uprintf(USART1,\
		"\r0V%d\r1V%d\r2V%d\r",\
		arg_speeds[0],\
		arg_speeds[1],\
		arg_speeds[2]\
		);
}

void t_move_x_c(int16_t arg_spd)
{
	t_move_xy_c(arg_spd * 128.0F / DEFAULT_ARG_SPEED, 0);
}

void t_move_y_c(int16_t arg_spd)
{
	t_move_xy_c(0, arg_spd * 128.0F / DEFAULT_ARG_SPEED);
}

void t_move_x(float x)
{
	float pos_x;
	float old_pos[2];
	float relative_mov;
	uint8_t dir;

	old_pos[0] = get_pos_x();
	old_pos[1] = get_pos_y();
	
	relative_mov = x - old_pos[0];
	
	if(fabs(relative_mov) < ZERO) {
		return;
	} else if(relative_mov > 0) {
		dir = 1;
		t_move_x_c(DEFAULT_ARG_SPEED);
	} else {
		dir =0;
		t_move_x_c(-DEFAULT_ARG_SPEED);
	}
	
	while(1) {
		pos_x = get_pos_x();

		#ifdef DEBUG
		printf("\npos_x:%f\n", pos_x);
		#endif
		
		if(dir) {
			if(pos_x - x - old_pos[1] > ZERO)
				break;
		} else {
			if(pos_x - x - old_pos[1] < ZERO)
				break;
		}

	}
	
	t_stop();
}

void t_move_y(float y)
{
	float pos_y;
	float old_pos[2];
	float relative_mov;
	uint8_t dir;

	old_pos[0] = get_pos_x();
	old_pos[1] = get_pos_y();
	
	relative_mov = y - old_pos[1];
	
	if(fabs(relative_mov) < ZERO) {
		return;
	} else if(relative_mov > 0) {
		dir = 1;
		t_move_y_c(DEFAULT_ARG_SPEED);
	} else {
		dir = 0;
		t_move_y_c(-DEFAULT_ARG_SPEED);
	}
	
	while(1) {
		pos_y = get_pos_y();

		#ifdef DEBUG
		printf("\npos_y:%f\n", pos_y);
		#endif

		if(dir) {
			if(pos_y - y - old_pos[1] > ZERO)
				break;
		} else {
			if(pos_y - y - old_pos[1] < ZERO)
				break;
		}
	}
	
	t_stop();
}

#endif
