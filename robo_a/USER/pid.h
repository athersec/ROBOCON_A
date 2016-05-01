
#ifndef _PID_H
#define _PID_H

typedef struct {
    float set_value;
    float actual_value;
    float error;
    float error_old;
    float kp,ki,kd;
    float sum;
}pid_t;



typedef struct {
    float set_value;
    float actual_value;
	float out;
    float error;
    float error_oold;
    float error_old;
    float kp,ki,kd;
    float sum;
}pid_inc_t;

void pid_config(pid_t* pid_initstruct);
float pid_realize(pid_t* pid);
void pid_inc_config(pid_inc_t* pid_inc_initstruct);
float pid_inc_realize(pid_inc_t* pid);

#endif

/*example**************usage like GPIO_Init*******
#include "pid.h"

int main()
{
    printf("System begin \n");
    pid_t pid_initstructure;

    pid_initstructure.set_value = 100.0;
    pid_initstructure.kp = 0.2;
    pid_initstructure.ki = 0.015;
    pid_initstructure.kd = 0.2;
    pid_config(&pid_initstructure);

    int count=0;
    while(count<1000)
    {
        float actual = pid_realize(&pid_initstructure);
        printf("%f\r\n",actual);
        count++;
    }
    return 0;
}
*/
