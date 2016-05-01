#include <stdint.h>


#define ENCODER_CHANNEL_NUM 5

#define VECT_EX (+1)  // encoder of x
#define VECT_EY (-1)  // encoder of y
#define VECT_EFAN (-1)  // encoder for the fan
#define VECT_EMAGNET (+1)	// encoder for the up and down movements of the magnet at tim1
#define VECT_ETIM8 (+1)	// encoder for the tim8


extern int32_t g_rotary_x;
extern int32_t g_rotary_y;
extern int32_t g_rotary_fan;
extern int32_t g_rotary_magnet;
extern int32_t g_rotary_tim8;


void encoder_config(void);
void encoder_test(void);
float get_pos_fan(void);
float get_pos_x(void);
float get_pos_y(void);
float get_pos_magnet(void);
float get_pos_tim8(void);
