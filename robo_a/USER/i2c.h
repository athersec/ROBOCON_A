#include <stdint.h>


void i2c1_write(uint8_t addr, uint8_t reg, uint8_t data);
uint8_t i2c1_read(uint8_t addr, uint8_t reg);
void i2c1_config(void);
