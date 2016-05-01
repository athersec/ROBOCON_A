#include "dma.h"
#include "string.h"

#define ANGLE_TO_RAD (float)0.01745

#define MTI_BUF_SIZE 19
#define MTI_SEND_SIZE 7

extern uint8_t mti_buffer[MTI_BUF_SIZE];
extern uint8_t mti_sendbuffer[MTI_SEND_SIZE];

extern const uint8_t mti_flag[4] ;
extern float mti_roll, mti_pitch, mti_yaw;
extern float mti_angle;
extern float mti_angle_old, mti_angle_new;
extern uint8_t mti_value_flag;

float mti(void);
int btol(uint8_t buffer[]);
float get_mti_value(void);
