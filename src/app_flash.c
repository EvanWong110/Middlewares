/**
 * @file
 * M_IAP flash interface implementations
 */

#include "app_flash.h"

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

uint32_t FLASH_If_Erase(uint32_t StartSector)
{
  return 0;
}

uint32_t FLASH_If_Write(__IO uint32_t* FlashAddress, uint32_t* Data, uint32_t DataLength)
{
  return 0;
}


