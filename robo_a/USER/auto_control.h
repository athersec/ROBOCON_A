#include <stdbool.h>

/*
Auto control will use:
	TIM10 TIM14
	The main thread
*/
extern bool auto_flag;

void auto_control(void);
