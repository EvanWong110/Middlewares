/**
 * @file
 * M_IAP flash interface implementations
 */
#ifndef APP_FLASH_H
#define APP_FLASH_H

#include "stm32f4xx_hal.h"


/* End of the Flash address */
#define USER_FLASH_END_ADDRESS        0x080FFFFF
/* Define the user application size */
#define USER_FLASH_SIZE   (USER_FLASH_END_ADDRESS - APPLICATION_ADDRESS + 1)

#define APPLICATION_ADDRESS   (uint32_t)0x08004000 
#define APP_START_SECTOR        (1)


void flash_if_init(void);

uint16_t flash_if_getWRP(void);
uint32_t FLASH_If_Erase(uint32_t StartSector);
uint32_t FLASH_If_Write(__IO uint32_t* FlashAddress, uint32_t* Data, uint32_t DataLength);

#endif


