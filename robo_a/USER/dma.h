#include "stm32f4xx.h"
#include "stm32f4xx_dma.h"

#define MTI_BUF_SIZE 19
#define MTI_SEND_SIZE 7

extern uint8_t mti_buffer[MTI_BUF_SIZE];
extern uint8_t mti_sendbuffer[MTI_SEND_SIZE];

void dma_config(void);
