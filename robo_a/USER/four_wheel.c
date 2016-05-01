#include <math.h>

#include "clock.h"
#include "database.h"
#include "debug.h"
#include "encoder.h"
#include "movement.h"
#include "utils.h"

#ifdef USE_FOUR_WHEEL

int16_t arg_speeds[4] = {0};

void f_stop(void)
{
	for(uint8_t i = 0; i < 4; i++){
		arg_speeds[i] = 0;
	}

	uprintf(USART1,\
		"\r0V%d\r1V%d\r2V%d\r5V%d\r",\
		arg_speeds[0],\
		arg_speeds[1],\
		arg_speeds[2],\
		arg_speeds[3]\
		);
}

void f_rotate_c(int8_t spd)
{
	int16_t arg_spd = (float) spd / 128 * DEFAULT_ARG_SPEED;
	arg_speeds[0] = VECT_W0 * arg_spd;
	arg_speeds[1] = VECT_W1 * arg_spd;
	arg_speeds[2] = -VECT_W2 * arg_spd;
	arg_speeds[3] = -VECT_W3 * arg_spd;
	
	#ifdef DEBUG
	printf("arg_speeds:\n");
	printf("\t0:%d\n", arg_speeds[0]);
	printf("\t1:%d\n", arg_speeds[1]);
	printf("\t2:%d\n", arg_speeds[2]);
	printf("\t3:%d\n", arg_speeds[3]);
	#endif

	uprintf(USART1,\
		"\r0V%d\r1V%d\r2V%d\r5V%d\r",\
		arg_speeds[0],\
		arg_speeds[1],\
		arg_speeds[2],\
		arg_speeds[3]\
		);
}

void f_move_arc(float y, float rad)
{
	float r = y / sinf(rad);
	float r1, r2;
	float coe_y = CAR_X_LENGTH / (sqrtf( pow( CAR_X_LENGTH, 2) + powf( CAR_Y_LENGTH, 2)));
	float div_rad;
	db_read("div_rad", (uint8_t*)&div_rad);
	if(0 == div_rad) {
		printf("\nSet div_rad!\n");
		return;
	}

	int16_t arg_y = DEFAULT_ARG_SPEED;
	int16_t arg_r = rad / div_rad * DEFAULT_ARG_SPEED;

	if(rad > 0) {
		r1 = (r - CAR_X_LENGTH / 2) / r;
		r2 = (r + CAR_X_LENGTH / 2) / r;
		arg_speeds[0] = VECT_W0 * (coe_y * arg_y) * r2;
		arg_speeds[1] = VECT_W1 * (-coe_y * arg_y) * r2;
		arg_speeds[2] = VECT_W2 * (coe_y * arg_y) * r1;
		arg_speeds[3] = VECT_W3 * (-coe_y * arg_y) * r1;
	} else if(rad < 0) {
		r1 = (r + CAR_X_LENGTH / 2) / fabs(r);
		r2 = (r / CAR_X_LENGTH / 2) / fabs(r);
	} else {
		r1 = 1;
		r2 = 1;
	}
	
	arg_speeds[0] = VECT_W0 * (coe_y * arg_y) * r2;
	arg_speeds[1] = VECT_W1 * (-coe_y * arg_y) * r2;
	arg_speeds[2] = VECT_W2 * (coe_y * arg_y) * r1;
	arg_speeds[3] = VECT_W3 * (-coe_y * arg_y) * r1;

	#ifdef DEBUG
	printf("arg_speeds:\n");
	printf("\t0:%d\n", arg_speeds[0]);
	printf("\t1:%d\n", arg_speeds[1]);
	printf("\t2:%d\n", arg_speeds[2]);
	printf("\t3:%d\n", arg_speeds[3]);
	#endif

	uprintf(USART1,\
		"\r0V%d\r1V%d\r2V%d\r5V%d\r",\
		arg_speeds[0],\
		arg_speeds[1],\
		arg_speeds[2],\
		arg_speeds[3]\
		);
}

void f_move_xy_c(int8_t spd_x, int8_t spd_y)
{
	int16_t arg_x, arg_y;

	float coe_x = CAR_Y_LENGTH / (sqrtf( powf( CAR_X_LENGTH, 2) + powf( CAR_Y_LENGTH, 2)));
	float coe_y = CAR_X_LENGTH / (sqrtf( powf( CAR_X_LENGTH, 2) + powf( CAR_Y_LENGTH, 2)));


	arg_x = (int16_t) (((float) spd_x) / 128 * DEFAULT_ARG_SPEED);
	arg_y = (int16_t) (((float) spd_y) / 128 * DEFAULT_ARG_SPEED);

	arg_speeds[0] = VECT_W0 * (coe_x * arg_x + coe_y * arg_y);
	arg_speeds[1] = VECT_W1 * (-coe_x * arg_x + coe_y * arg_y);
	arg_speeds[2] = VECT_W2 * (coe_x * arg_x + coe_y * arg_y);
	arg_speeds[3] = VECT_W3 * (-coe_x * arg_x + coe_y * arg_y);

	#ifdef DEBUG
	printf("arg_speeds:\n");
	printf("\t0:%d\n", arg_speeds[0]);
	printf("\t1:%d\n", arg_speeds[1]);
	printf("\t2:%d\n", arg_speeds[2]);
	printf("\t3:%d\n", arg_speeds[3]);
	#endif

	uprintf(USART1,\
		"\r0V%d\r1V%d\r2V%d\r5V%d\r",\
		arg_speeds[0],\
		arg_speeds[1],\
		arg_speeds[2],\
		arg_speeds[3]\
		);
}

#endif
