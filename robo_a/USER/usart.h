#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#include "stm32f4xx_usart.h"

int fgetc(FILE *fp);
int fputc(int c, FILE *f);

void uart1_config(uint32_t baud);
void uart2_config(uint32_t baud);
void uart3_config(uint32_t baud);
void uart4_config(uint32_t baud);
void uart_config(uint32_t baud);
void uprintf(USART_TypeDef* USARTx, char *fmt, ...);
