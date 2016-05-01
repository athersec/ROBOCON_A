#include <stdbool.h>

/*
Auto control will use:
	TIM14
	The main thread
*/

#define TIM14_TIME 0.001f

extern bool auto_flag;
extern float old_pos_x, old_pos_y, old_rad;
extern float dest_pos_x, dest_pos_y, dest_rad;

void auto_control(void);
