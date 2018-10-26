/**
  ******************************************************************************
  * @file    ../middlewares/inc/m_common.h
  * @author  yhangzzz
  * @version V1.0.0
  * @date    2010.10.22
  * @brief   m_common.h
  ******************************************************************************
  */ 


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COMMON_H
#define __COMMON_H

/* config */
#define M_COMMON_INT2STR 0
#define M_COMMON_STR2INT 0

/* Includes ------------------------------------------------------------------*/
#include "typesdef.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Common routines */
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

/* Exported functions ------------------------------------------------------- */
#if M_COMMON_INT2STR
void Int2Str(uint8_t* str,int32_t intnum);
#endif

#if M_COMMON_STR2INT
uint32_t Str2Int(uint8_t *inputstr,int32_t *intnum);
#endif

#endif  /* __COMMON_H */

/*******************(C)COPYRIGHT 2011 STMicroelectronics *****END OF FILE******/
