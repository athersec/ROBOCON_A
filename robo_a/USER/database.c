#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "database.h"
#include "debug.h"
#include "flash.h"


#define NAME_MAX_LEN 32
#define CMD_BUF_LEN 64
#define DATA_MSG_LEN sizeof(uint32_t)


char db_cmd_buf[CMD_BUF_LEN] = {0};
char_queue db_cmd_queue;

static uint8_t db_buf[DB_SECTOR_LEN + NAME_MAX_LEN]; // NAME_MAX_LEN for not to overflow
static uint32_t db_index = 0;

static bool db_init_done = false;


uint32_t get_len_by_pos(uint32_t pos)
{
	uint32_t i, data_len = 0;

	for(i = 1; i <= DATA_MSG_LEN; i++){
		data_len <<= 8;
		data_len |= db_buf[pos - i];
	}

	return data_len;
}

void db_init(void)
{
	uint32_t i;

	flreadn(DB_SECTOR, (uint32_t*)db_buf, DB_SECTOR_LEN);
	if(db_buf[0] != '\n') {
		db_buf[0] = '\n';
		db_index = 1;
	} else {
		for(i = DB_SECTOR_LEN - 1; i > 1; i--) {
			if(db_buf[i] == 0xff && db_buf[i - 1] == '\n') {
				db_index = i;
				break;
			}
		}
	}

	init_char_queue(&db_cmd_queue, db_cmd_buf, CMD_BUF_LEN);
	db_init_done = true;
}

void db_clear_init(void)
{
	flerase(DB_SECTOR);
	db_init();
}

void db_save(const char* const name, uint8_t* data, uint32_t data_len)
{
	if(false == db_init_done)
		return;

	if(0 != db_find(name))
		db_delete(name);

	uint32_t i, j;
	uint8_t name_len = strlen(name);

	i = db_index;

	for(j = 0; j < name_len; j++) {
		db_buf[i + j] = name[j];
	}
	db_buf[i + j] = '=';

	i = i + j + 1;
	for(j = 0; j < DATA_MSG_LEN; j++) {
		db_buf[i + j] = (uint8_t)(data_len >> (j << 3));
	}

	i = i + j;
	for(j = 0; j < data_len; j++) {
		db_buf[i + j] = data[j];
	}
	db_buf[i + j] = '\n';

	db_index = i + j + 1;
	return;
}

void db_sync(void)
{
	if(false == db_init_done)
		return;

	flerase(DB_SECTOR);
	flwriten(DB_SECTOR, (uint32_t*)db_buf, DB_SECTOR_LEN);
}


uint32_t db_find(const char* const name)
{
	static char tmp[NAME_MAX_LEN];
	uint32_t i, j;
	for(i = 0; i < db_index; i++) {
		if('\n' == db_buf[i] && 0xff == db_buf[i + 1]) {
			return 0;
		}

		if('\n' == db_buf[i]) {
			for(j = 0; j < db_index; j++) {
				tmp[j] = db_buf[i + j + 1];
				if('=' == tmp[j]) {
					tmp[j] = 0;
					if(strcmp(tmp, name) == 0) {
						return i + j + 2 + DATA_MSG_LEN;
					}
				}
			}
			
		}
	}

	return 0;
}

void db_delete(const char* const name)
{
	uint32_t i, pos, start, end, len;
	uint32_t data_len = 0;

	pos = db_find(name);
	if(0 == pos)
		return;

	data_len = get_len_by_pos(pos);

	for(start = pos; '\n' != db_buf[start]; start--);
	end = pos + data_len;
	len = end - start;

	for(i = end; i < db_index; i++) {
		db_buf[i - len] = db_buf[i];
	}
	for(i = i - len; i < db_index; i++) {
		db_buf[i] = 0xff;
	}

	db_index -= len;
}

void db_read(const char* const name, uint8_t* data)
{
	if(false == db_init_done)
		return;

	uint32_t i, j, data_len;
	
	i = db_find(name);
	if(i == 0) {
		data[0] = 0;
		return;
	}

	data_len = get_len_by_pos(i);

	for(j = 0; j < data_len; j++) {
		data[j] = db_buf[i + j];
	}
	data[j] = 0;
}


void db_exec(char cmd[])
{
	uint32_t tmp;
	uint32_t i, pos;
	uint32_t len = strlen(cmd);
	bool is_valid = false;
	char name[CMD_BUF_LEN];
	uint8_t data[CMD_BUF_LEN];

	switch(cmd[0]) {
		case 'h': 
			for(i = 2; i < NAME_MAX_LEN; i++) {
				if('=' == cmd[i]) {
					is_valid = true;
					cmd[i] = 0;
					break;
				}
			}
			if(is_valid) {
				for(i = 0; 0 != cmd[i + 1]; i++) {
					name[i] = cmd[i + 1];
				}
				sscanf(cmd + 2 + i, "%32x", &tmp);
				db_save(name, (uint8_t*)(&tmp), sizeof(uint32_t));
				tmp = 0;
				db_read(name, (uint8_t*)(&tmp));
				printf("\nname:%s\nhex:0x%x saved.\n", name, tmp);
			} else {
				printf("\nInvalid hex\n");
			}
			break;
		case 'w':
			for(i = 1; i < NAME_MAX_LEN; i++) {
				if('=' == cmd[i]) {
					is_valid = true;
					cmd[i] = 0;
					break;
				}
			}
			if(is_valid) {
				for(i = 1; cmd[i] != 0; i++) {
					name[i - 1] = cmd[i];
				}
				name[i - 1] = 0;

				pos = i + 1;
				for(i = 0; '\n' != cmd[pos + i]; i++) {
					data[i] = cmd[pos + i];
				}
				data[i] = 0;

				printf("\nname:%s,data:%s\n", (char*)name, (char*)data);

				db_save((char*)name, data, i);
			} else {
				printf("\nWrong code to write\n");
			}
			break;
		case 'r':
			for(i = 1; i < NAME_MAX_LEN; i++) {
				name[i - 1] = cmd[i];
				if('\n' == cmd[i]) {
					name[i - 1] = 0;
					break;
				}
			}
			db_read((char*)name, data);
			printf("\n%s=%s\n", name, data);
			break;
		case 'd':
			for(i = 1; i < NAME_MAX_LEN; i++) {
				name[i - 1] = cmd[i];
				if('\n' == cmd[i]) {
					name[i - 1] = 0;
					break;
				}
			}
			db_delete((char*)name);
			printf("\n%s deleted\n", name);
			break;
		case 's':
			db_sync();
			printf("\nsync done\n");
			break;
		default:
			#ifdef DEBUG_DB_EXEC
			printf("\nUnrecognised code\n");
			#endif
			break;
	}
	
}

void db_queue_exec(void)
{
	uint16_t i;
	static char cmd[CMD_BUF_LEN];
	for(i = 0; i < CMD_BUF_LEN; i++) {
		out_char_queue(&db_cmd_queue, (cmd + i));
		if('\n' == cmd[i]) {
			cmd[i + 1] = 0;
			break;
		}
	}
	db_exec(cmd);
}
