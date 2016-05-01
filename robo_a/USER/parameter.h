#include "link_list.h"
//控制机器人的代码中，所需要调的参数都集中放在这个结构体(param_struct结构体,注意：所有参数都要实数型的)里
//以后需要添加参数，首先就要在这里(param_struct结构体)添加，然后在parameter.c文件里面的param_update()函数里面添加对应的宏PARAM_UPDATE()
//每当链表里面的节点数据被改变的时候，可以执行函数param_update_all()来更新patam_struct结构体中所有成员的数据
//或者一次改变的参数不多，则可以单独调用宏PARAM_UPDATE()来更新成员的数据


#define PARAM_FLASH_SETOR ((uint16_t)0x0020) //  扇区4
#define PARAM_FLASH_ADDR_START 0x08010000
#define PARAM_FLASH_ADDR_END 0x080104FC

typedef struct{
    float group;
	  float kp;
		float ki;
		float kd;
		float kp_r;
		float ki_r;
		float kd_r;
	
}param_struct;

extern link_list g_param_list;    //参数链表

#define PARAM_USARTx UART5     //parameter使用的串口
//根据链表上已有的参数数据来更新参数   USARTx 为输出错误信息的串口
#define PARAM_UPDATE(param_list,control_param_array,param_name) \
    if(list_search(&param_list,#param_name) != NULL){    \
        int i;                  \
        for(i = 0;i < PARAM_GROUP_LENGTH;i++){     \
            control_param_array[i].param_name = list_search(&param_list,#param_name)->data->param_value[i];  	\
            }      \
    }else{   \
        memset(param_value_array,0,sizeof(float)*PARAM_GROUP_LENGTH); \
        list_insert(&param_list,list_get_length(&param_list)+1,#param_name,param_value_array);  \
    }                                                     \
					
					
void param_init(param_struct **param);
void param_update_all(void);
void param_print(int param_group);
int param_set(char *param_name,float param_value);
int param_save_to_flash(void);
int	param_ld_from_flash(void);
int param_list_reset(void);
int param_switch(int group_num);  //切换参数组
int param_group_now(void);  //获得参数组数
list_node* get_param_list(void);
param_struct* get_param_struct(void);
