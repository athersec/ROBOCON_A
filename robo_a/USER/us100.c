#include "suart.h"
#include "us100.h"

// Return distance in milimeters
uint16_t us100_get_distance(uint8_t su_channel)
{
	uint16_t result;

	suputchar(su_channel, 0x55);

	result = sugetchar(su_channel) << 8;
	result |= sugetchar(su_channel);
	
	/*
		If the distance is out of range, return 0
	*/
	if(result < 200 || result > 4500)
		result = 0;

	return result;
}
