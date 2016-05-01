/*
Auto control will use:
	TIM10
*/

#include <stdbool.h>

#define MAX_ROTATE_SPD 1000
#define MAX_XY_SPD 2000
#define AUTO_NEAR_DIST 0.1f

void automove_config(void);
void automove_daemon(void);
bool near_auto_dest(void);

float get_gps_x(void);
float get_gps_y(void);
float get_gps_rad(void);
void set_auto_dest(float x, float y, float rad);
