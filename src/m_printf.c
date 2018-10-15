
#include "m_printf.h"
extern void console_write(const char *format);

void m_printf(const char *format, ...)
{
	static char buff[256];
	va_list args;
	int i;
	
	va_start(args,format);
	i = vsprintf(buff,format,args);
	va_end(args);
	
	buff[i] = '\0';
	console_write(buff);
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
