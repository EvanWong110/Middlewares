
#include "m_printf.h"
#include "stm32f4xx_hal.h"

extern UART_HandleTypeDef huart1;
extern HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
uint8_t serial_write(char *pdata)
{
    return HAL_UART_Transmit(&huart1,(uint8_t *)pdata,strlen(pdata),100);
}
void m_printf(const char *format, ...)
{
	static char buff[256];
	va_list args;
	int i;
	
	va_start(args,format);
	i = vsprintf(buff,format,args);
	va_end(args);
	
	buff[i] = '\0';
	serial_write(buff);
}

int m_scanf(const char *format, ...)
{
  int cnt;
  
  va_list args;
  va_start(args,format);
  cnt = vscanf(format,args);
  va_end(args);
  return cnt;
}
