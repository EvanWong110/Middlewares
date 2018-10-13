/**
 * @file
 * m_printf implementations
 */

#ifndef M_PRINTF_H
#define M_PRINTF_H

#define __DEBUG__  
  
#ifdef __DEBUG__  
#define DEBUG(format,...) debug_printf("File: "__FILE__", Line: %05d: "format"\n", __LINE__, ##__VA_ARGS__)  
#else  
#define DEBUG(format,...)  
#endif 

void debug_printf(const char *format, ...);

void m_printf(const char *format, ...);
int m_scanf(const char *format, ...);



#endif

