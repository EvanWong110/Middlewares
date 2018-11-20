/**
  ******************************************************************************
  * @file    ../middlewares/inc/m_common.h
  * @author  yhangzzz
  * @version V1.0.0
  * @date    2018.10.22
  * @brief   m_common.h
  ******************************************************************************
  */ 


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COMMON_H
#define __COMMON_H

/* config */
#define M_COMMON_INT2STR 0
#define M_COMMON_STR2INT 1

#define M_SCHEDULER_ENABLED 1
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stddef.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

//linux list.h 使用；如果使用linux内核链表，需要编译时开启GNU扩展；
#define m_offsetof(TYPE,MEMBER) ((size_t)&((TYPE *)0)->MEMBER)
#define container_of(ptr,type,member) ( {\
    const typeof( ((type*)0)->member ) *__mptr=(ptr);\
    (type*)( (char*)__mptr - offsetof(type,member) );} )

#define IS_AF(c)  ((c >= 'A') && (c <= 'F'))
#define IS_af(c)  ((c >= 'a') && (c <= 'f'))
#define IS_09(c)  ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)  IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)  IS_09(c)
#define CONVERTDEC(c)  (c - '0')

#define CONVERTHEX_alpha(c)  (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))

#define M_ENTER_CRITICAL()

#define M_EXIT_CRITICAL()

//静态断言
#define STATIC_ASSERT(bExpression) typedef uint8_t assert_failed[(bExpression)?1 : -1]


/* Exported functions ------------------------------------------------------- */
#if M_COMMON_INT2STR
void Int2Str(uint8_t* str,int32_t intnum);
#endif

#if M_COMMON_STR2INT
uint32_t Str2Int(uint8_t *inputstr,int32_t *intnum);
#endif

int16_t m_memfind(const char*buff, char* mem, int16_t len);

#endif  /* __COMMON_H */

/*******************(C)COPYRIGHT 2011 STMicroelectronics *****END OF FILE******/
