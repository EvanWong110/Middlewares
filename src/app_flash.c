/**
 * @file
 * M_IAP flash interface implementations
 */

#include "app_flash.h"
#include "stm32f4xx_hal_flash_ex.h"
/**
  * @brief  Gets the sector of a given address
  * @param  Address: Flash address  FLASH_SECTOR_0
  * @retval The sector of a given address
  */
static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_SECTOR_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_SECTOR_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_SECTOR_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_SECTOR_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_SECTOR_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_SECTOR_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_SECTOR_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_SECTOR_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_SECTOR_10;  
  }
  else/*(Address < FLASH_END_ADDR) && (Address >= FLASH_SECTOR_11))*/
  {
    sector = FLASH_SECTOR_11;  
  }
    return sector;
}

void flash_if_init(void)
{
  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
}

//return 0: app用flash无保护， 1：有保护
uint16_t flash_if_getWRP(void)
{
  if((*(__IO uint16_t *)(OPTCR_BYTE2_ADDRESS))>> APP_START_SECTOR == 0xfff>>APP_START_SECTOR)
    return 0; //无保护
  else
    return 1; //有保护
}

//return 0：erase complete；
//return ！0；error occur
uint32_t FLASH_If_Erase(uint32_t StartSector)
{
	uint32_t errcode = 0;
	for(uint32_t i = StartSector; i <= FLASH_SECTOR_11; i++)
	{
		errcode = FLASH_WaitForLastOperation(5000);
		if(errcode)
			return errcode;
		FLASH_Erase_Sector(i,FLASH_VOLTAGE_RANGE_3);
	}
  return errcode;
}

uint32_t FLASH_If_Write(__IO uint32_t* FlashAddress, uint32_t* Data, uint32_t DataLength)
{
	for(uint32_t i = 0; i<DataLength; i++)
	{
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,*FlashAddress,Data[i]) == HAL_OK)
		{
			if(*(uint32_t*)*FlashAddress != Data[i])
				return 2;
		}
		*FlashAddress += 4;
	}
  return 0;
}


