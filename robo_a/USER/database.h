#include <stdbool.h>
#include <stdint.h>

#include "utils.h"

#define DB_SECTOR ADDR_FLASH_SECTOR_5
#define DB_SECTOR_LEN 16384

extern char_queue db_cmd_queue;

void db_init(void);
void db_clear_init(void);
void db_delete(const char* const name);
void db_exec(char cmd[]);
uint32_t db_find(const char* const name);
void db_queue_exec(void);
bool db_read(const char* const name, uint8_t* data);
void db_save(const char* const name, uint8_t* data, uint32_t data_len);
void db_sync(void);
