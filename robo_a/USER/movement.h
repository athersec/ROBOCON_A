#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#include "usart.h"

/*
The world view:
	front = +y
	back = -y
	right = +x
	left = -x
	up = +z
	down = -z
*/

#define ZERO ((float) (1E-1))
#define PI 3.141592653589793F

#define W_DIAMETER ((float) 0.05)  // meter
#define F_DIAMETER ((float) 0.032)  // meter

#define USE_FOUR_WHEEL

#ifdef USE_THREE_WHEEL

#define DEFAULT_ARG_SPEED 2000
#define VECT_W0 (-1)  // the wheel on the left or right side
#define VECT_W1 (+1)  // the front wheel
#define VECT_W2 (-1)  // the back wheel

extern int16_t arg_speeds[3];

void t_move_x(float x);
void t_move_x_c(int16_t arg_spd);
void t_move_y(float y);
void t_move_y_c(int16_t arg_spd);
void t_move_xy_c(int8_t spd_x, int8_t spd_y);
void t_rotate_c(int8_t arg_spd);
void t_stop(void);

#endif

#ifdef USE_FOUR_WHEEL

#define DEFAULT_ARG_SPEED 4000
#define DEFAULT_MANUAL_ARG_SPEED 2000
#define DEFAULT_MANUAL_ROTATE_ARG_SPEED 1000
#define CAR_X_LENGTH 500
#define CAR_Y_LENGTH 650
#define VECT_W0 (-1)
#define VECT_W1 (-1)
#define VECT_W2 (+1)
#define VECT_W3 (+1)

extern int16_t arg_speeds[4];

void f_run_c(int8_t spd_x, int8_t spd_y, int8_t spd_c);
void f_move_arc(float y, float rad);
void f_move_xy(float x, float y);
void f_move_xy_c(int8_t spd_x, int8_t spd_y);
void f_rotate_r(float rad);
void f_rotate_r_c(int8_t spd);
void f_stop(void);
void f_send_speed(void);

#endif

void move_arc(float y, float rad);
void move_up(void);
void move_down(void);
void move_x(float x);
void move_x_c(int16_t arg_spd);
void move_y(float y);
void move_y_c(int16_t arg_spd);
void move_xy(float x, float y);
void move_xy_c(int8_t spd_x, int8_t spd_y);

void rotate(float rad);
void rotate_r(float rad);
void rotate_c(int8_t arg_spd);

void stop(void);
void stop_all(void);
void stop_move_up(void);
