#include <stdint.h>

#define FAN_CHANNEL 0
#define FAN_UPDOWN_CHANNEL 1
#define FAN_ROLL_CHANNEL 2
#define FAN_RMOVE_DIST ((float) 0.1)
#define FAN_RROLL_DIST ((float) 0.1)

void start_fan(void);
void stop_fan(void);
void toggle_fan(void);

void fan_up(void);
void fan_up_r(void);
void fan_down(void);
void fan_down_r(void);
void stop_fan_up_down(void);


void fan_roll(float rad);
void fan_roll_r(int8_t dir);  // +1 or -1
