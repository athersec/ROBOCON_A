#include <stdint.h>

void delay_init(uint8_t SYSCLK);
inline void delay_ms(uint32_t ms);
inline void delay_us(uint32_t us);
void system_clk_set(void);
void systick_config(void);
