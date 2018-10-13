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


typedef enum {
	M_IDLE = 0,
	M_WAITING_FILENAME,
	M_DOWNLOAD,
	M_PROGRAM,
	M_UPLOAD,
	M_BOOTING,
	M_ERROR
}MENUM_IAP_STATUS;

typedef  void (*pFunction)(void);


void m_iap_init(void);

void m_iap_process(void);

#endif
