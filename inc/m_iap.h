/**
 * @file
 * M_IAP implementations
 */
#ifndef M_IAP_H
#define M_IAP_H 

#include "m_timeouts.h"
#include "m_printf.h"
#include "app_flash.h"
#include "ymodem.h"
#include "eeprom.h"

typedef enum {
	M_IDLE = 0,
	M_WAITING,
	M_DOWNLOAD,
	M_PROGRAM,
	M_UPLOAD,
	M_BOOTING,
	M_ERROR
}MENUM_IAP_STATUS;

typedef  void (*pFunction)(void);


//命令行指令
typedef enum
{
    sERROR = 0,
    sWrAtAddr,
    sRdAtAddr
}eSET_VALUE;


//最大参数个数
#define PARA_MAXNUM             (8)

//参数最大长度
#define PARA_LENGTH             (32)

typedef  int32_t (*S_FUNC)(int32_t argc, uint8_t (*argv)[]);

typedef struct
{
    uint8_t setString[PARA_LENGTH];
    eSET_VALUE v;
    S_FUNC func;
}sSET_TCB;


void m_iap_init(void);

void m_iap_process(void);

#endif
