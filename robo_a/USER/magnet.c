#include <stm32f4xx_gpio.h>

#include "clock.h"
#include "magnet.h"
#include "pwm.h"

void mag_in(void)
{
	set_duty(MAG_IN_CHANNEL, 0.10);
}

void mag_out(void)
{
	set_duty(MAG_IN_CHANNEL, 0.03);
}

void mag_near(void)
{
	set_duty(MAG_NEAR_CHANNEL, 0.083);
}

void mag_far(void)
{
	set_duty(MAG_NEAR_CHANNEL, 0.056);
}
