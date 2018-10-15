/**
 * @file
 * m_printf implementations
 */

#ifndef M_PRINTF_H
#define M_PRINTF_H

#include "stdarg.h"
#include "stdio.h"

#define __DEBUG__  
  
#ifdef 0
#define DEBUG(format,...) debug_printf("File: "__FILE__", Line: %05d: "format"", __LINE__, ##__VA_ARGS__)  
#else  
#define DEBUG(format,...)  
#endif 

void m_printf(const char *format, ...);
int m_scanf(const char *format, ...);



#endif

