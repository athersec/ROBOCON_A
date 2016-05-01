#include "flash.h"
#include "usart.h" 


uint32_t flread(uint32_t faddr)
{
	return *(uint32_t*)faddr; 
}  


uint16_t flash_get_sector(uint32_t addr)
{
	if(addr<ADDR_FLASH_SECTOR_1)return FLASH_Sector_0;
	else if(addr<ADDR_FLASH_SECTOR_2)return FLASH_Sector_1;
	else if(addr<ADDR_FLASH_SECTOR_3)return FLASH_Sector_2;
	else if(addr<ADDR_FLASH_SECTOR_4)return FLASH_Sector_3;
	else if(addr<ADDR_FLASH_SECTOR_5)return FLASH_Sector_4;
	else if(addr<ADDR_FLASH_SECTOR_6)return FLASH_Sector_5;
	else if(addr<ADDR_FLASH_SECTOR_7)return FLASH_Sector_6;
	else if(addr<ADDR_FLASH_SECTOR_8)return FLASH_Sector_7;
	else if(addr<ADDR_FLASH_SECTOR_9)return FLASH_Sector_8;
	else if(addr<ADDR_FLASH_SECTOR_10)return FLASH_Sector_9;
	else if(addr<ADDR_FLASH_SECTOR_11)return FLASH_Sector_10; 
	return FLASH_Sector_11;	
}


void flerase(uint32_t addrx)
{
	if(addrx < STM32_FLASH_BASE || addrx % 4)
		return;
	FLASH_Unlock();
	FLASH_DataCacheCmd(DISABLE);

	if(addrx<0X1FFF0000)
	{   
		FLASH_EraseSector(flash_get_sector(addrx),VoltageRange_3);
	}

	FLASH_DataCacheCmd(ENABLE);
	FLASH_Lock();
}


void flwriten(uint32_t addrx,uint32_t *buf,uint32_t data_len)	
{ 
	FLASH_Status status = FLASH_COMPLETE;
	uint32_t endaddr = addrx + data_len / 4;	
	if(addrx < STM32_FLASH_BASE || addrx % 4)
		return;
	FLASH_Unlock();
	FLASH_DataCacheCmd(DISABLE);

	if(status == FLASH_COMPLETE)
	{
		while(addrx<endaddr)
		{
			if(FLASH_ProgramWord(addrx, *buf) != FLASH_COMPLETE)
			{ 
				break;
			}
			addrx += 4;
			buf++;
		} 
	}
	FLASH_DataCacheCmd(ENABLE);
	FLASH_Lock();
} 


void flreadn(uint32_t addrx,uint32_t *buf,uint32_t data_len)   	
{
	uint32_t i;
	for(i=0;i<data_len;i++)
	{
		buf[i]=flread(addrx);
		addrx+=4;	
	}
}
