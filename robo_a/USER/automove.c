#include <math.h>

#include <misc.h>
#include <stm32f4xx_tim.h>

#include "automove.h"
#include "debug.h"
#include "encoder.h"
#include "movement.h"
#include "mti.h"
#include "pid.h"

#ifdef USE_FOUR_WHEEL

void auto_clr_spd(void)
{
	for(uint8_t i = 0; i < 4; i++) {
		arg_speeds[i] = 0;
	}
}

#define ROTATE_DEFAULT_SPD 1000
void auto_rotate(float now_rad, float dest_rad)
{
	float drad = dest_rad - now_rad;

	static pid_t pr;
	pr.kp = 1;
	pr.kd = 0.1f;
	pr.ki = 0;
	pr.set_value = dest_rad;
	pr.actual_value = now_rad;

	float prout = pid_realize(&pr);

	#ifdef DEBUG_AUTO
	printf("pr:%f\n", prout);
	#endif

	float spd_r = prout * ROTATE_DEFAULT_SPD;
	if(spd_r > MAX_ROTATE_SPD)
		spd_r = MAX_ROTATE_SPD;

	arg_speeds[0] += VECT_W0 * spd_r;
	arg_speeds[1] += VECT_W1 * spd_r;
	arg_speeds[2] += -VECT_W2 * spd_r;
	arg_speeds[3] += -VECT_W3 * spd_r;
}

#define XY_DEFAULT_SPD 1000
void auto_move_xy(float x, float y, float dest_x, float dest_y, float now_rad)
{
	float coe_x = CAR_Y_LENGTH / (sqrtf(powf(CAR_X_LENGTH, 2) + powf(CAR_Y_LENGTH, 2)));
	float coe_y = CAR_X_LENGTH / (sqrtf(powf(CAR_X_LENGTH, 2) + powf(CAR_Y_LENGTH, 2)));
	float dx = dest_x - x, dy = dest_y - y;
	
	static pid_t px, py;
	float pxout, pyout;

	px.kp = 3;
	px.kd = 0.1f;
	px.ki = 0;
	px.set_value = dest_x;
	px.actual_value = x;
	pxout = pid_realize(&px);

	py.kp = 3;
	py.kd = 0.1f;
	py.ki = 0;
	py.set_value = dest_y;
	py.actual_value = y;
	pyout = pid_realize(&py);

	float spd_x = coe_x * pxout * XY_DEFAULT_SPD;
	float spd_y = coe_y * pyout * XY_DEFAULT_SPD;
	if(spd_x > MAX_XY_SPD)
		spd_x = MAX_XY_SPD;
	if(spd_y > MAX_XY_SPD)
		spd_y = MAX_XY_SPD;

	arg_speeds[0] += VECT_W0 * ((spd_x * cosf(now_rad) + spd_y * sinf(now_rad)) + (-spd_x * sinf(now_rad) + spd_y * cosf(now_rad)));
	arg_speeds[1] += VECT_W1 * (-(spd_x * cosf(now_rad) + spd_y * sinf(now_rad)) + (-spd_x * sinf(now_rad) + spd_y * cosf(now_rad)));
	arg_speeds[2] += VECT_W2 * ((spd_x * cosf(now_rad) + spd_y * sinf(now_rad)) + (-spd_x * sinf(now_rad) + spd_y * cosf(now_rad)));
	arg_speeds[3] += VECT_W3 * (-(spd_x * cosf(now_rad) + spd_y * sinf(now_rad)) + (-spd_x * sinf(now_rad) + spd_y * cosf(now_rad)));
}

void auto_send(void)
{
	uprintf(USART1, "\rAC10000\rDEC50\r");

	uprintf(USART1,\
		"\r0V%d\r1V%d\r2V%d\r5V%d\r",\
		arg_speeds[0],\
		arg_speeds[1],\
		arg_speeds[2],\
		arg_speeds[3]\
		);
	
	#ifdef DEBUG_AUTO
	printf("arg_speeds:\n");
	printf("\t0:%d\n", arg_speeds[0]);
	printf("\t1:%d\n", arg_speeds[1]);
	printf("\t2:%d\n", arg_speeds[2]);
	printf("\t3:%d\n", arg_speeds[3]);
	#endif
}

#endif

float gps_dest_x = 0, gps_dest_y = 0, gps_dest_rad = 0;
float gps_x = 0, gps_y = 0, gps_rad = 0;

void set_auto_dest(float x, float y, float rad)
{
	gps_dest_x = x;
	gps_dest_y = y;
	gps_dest_rad = rad;
}

bool near_auto_dest(void)
{
	if(fabsf(gps_dest_x - gps_x) <= AUTO_NEAR_DIST\
		&& fabsf(gps_dest_y - gps_y) <= AUTO_NEAR_DIST\
		&& fabsf(gps_dest_rad - gps_rad) <= AUTO_NEAR_DIST) {
		return true;
	} else {
		return false;
	}
}

void automove_daemon(void)
{
	static float old_x = 0, old_y = 0;
	float dx, dy;
	float x, y, rad;
	float tmp;

	rad = get_mti_value();

	// theoretical value
	// tmp = (cosf(fabsf(rad)) - 1) * fabsf(EX_X) + rad * fabsf(EX_Y);
	// measured value
	tmp = 0.511f * rad / 2 / PI;

	x = get_pos_x() - tmp;
	dx = x - old_x;

	// theoretical value
	// tmp = (cosf(fabsf(rad)) - 1) * fabsf(EY_Y) + rad * fabsf(EY_X);
	// measured value
	tmp = 1.195f * rad / 2 / PI;

	y = get_pos_y() - tmp;
	dy = y - old_y;

	old_x = x;
	old_y = y;

	gps_x += -dy * sinf(rad) + dx * cosf(rad);
	gps_y += dy * cosf(rad) + dx * sinf(rad);
	gps_rad = rad;

	auto_clr_spd();
	auto_rotate(gps_rad, gps_dest_rad);
	auto_move_xy(gps_x, gps_y, gps_dest_x, gps_dest_y, gps_rad);

	auto_send();
	
	#ifdef DEBUG_AUTO
	printf("%f %f\t%f %f\t%f %f\n\n", gps_x, gps_dest_x, gps_y, gps_dest_y, gps_rad, gps_dest_rad);
	#endif
}

float get_gps_x(void)
{
	return gps_x;
}

float get_gps_y(void)
{
	return gps_y;
}

float get_gps_rad(void)
{
	return gps_rad;
}

void tim10_config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);
	
	TIM_TimeBaseInitStructure.TIM_Period = 10000 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1680 - 1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	
	TIM_TimeBaseInit(TIM10, &TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM10, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM10, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void automove_config(void)
{
	tim10_config();
}
