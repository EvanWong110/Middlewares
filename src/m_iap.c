/**
 * @file
 * M_IAP implementations
 */

#include "m_iap.h"

//自动引导超时时间，单位，iap_tick,100ms
#define AUTOBOOT_TIME  50
#define M_IAP_TICK     100


extern uint8_t FileName[];

uint8_t tab_1024[1024] =
  {
    0
  };
    
pFunction Jump_To_Application;
uint32_t JumpAddress;

volatile uint32_t FlashProtection = 0;

MENUM_IAP_STATUS m_iap_status = M_IDLE;



//n秒无操作进入BOOT状态
void mfunc_autoboot_timeout()
{
	static uint32_t ns_time = 0;
	if(m_iap_status == M_IDLE)
	{
		ns_time ++;
		if(ns_time > AUTOBOOT_TIME)
			m_iap_status = M_BOOTING;
	}
	else 
		ns_time = 0;
}


static uint32_t m_iap_tick = 0;
static m_tm_tcb tm_iap_tick;
void tm_iap_tick_handler(void *arg)
{
	m_iap_tick++;
	mfunc_autoboot_timeout();
}

//flash解锁，初始化串口及定时器倒计时,时间到就进入APP
void m_iap_init(void)
{
  flash_if_init();
  
	m_iap_status = M_IDLE;
	
	tm_iap_tick.phandler = tm_iap_tick_handler;
	tm_iap_tick.time = M_IAP_TICK;
	tm_iap_tick.period = M_IAP_TICK;
	m_timeout_add(&tm_iap_tick);

  if(flash_if_getWRP() == 1)
    FlashProtection =1;
  else
    FlashProtection =0;
}

static void printf_menu(void)
{
  m_printf("\r\n%d milliseconds no operation will automatically boot the system\r\n\n", AUTOBOOT_TIME*M_IAP_TICK);
	m_printf("\r\n======================M_IAP MAIN MENU===================\r\n\n");
  m_printf("Show the main menu -----------------------------0\r\n\n");
	m_printf("Download IMage to the OBJ Internal Flash -------1\r\n\n");
	m_printf("Upload IMage from the OBJ Internal Flash -------2\r\n\n");
	m_printf("Execute the new program ------------------------3\r\n\n");
  if(FlashProtection == 1)
    m_printf("Disable the write protection ------------------------4\r\n\n");
  m_printf("=====================================================\r\n\n");
  
}

void SerialDownload(void)
{
  
uint8_t Number[10] = "          ";
  int32_t Size = 0;

  m_printf("Waiting for the file to be sent ... (press 'a' to abort)\n\r");
  Size = Ymodem_Receive(&tab_1024[0]);
  if (Size > 0)
  {
    m_printf("\n\n\r Programming Completed Successfully!\n\r--------------------------------\r\n Name: ");
    m_printf(FileName);
    Int2Str(Number, Size);
    m_printf("\n\r Size: ");
    m_printf(Number);
    m_printf(" Bytes\r\n");
    m_printf("-------------------\n");
  }
  else if (Size == -1)
  {
    m_printf("\n\n\rThe image size is higher than the allowed space memory!\n\r");
  }
  else if (Size == -2)
  {
    m_printf("\n\n\rVerification failed!\n\r");
  }
  else if (Size == -3)
  {
    m_printf("\r\n\nAborted by user.\n\r");
  }
  else
  {
    m_printf("\n\rFailed to receive the file!\n\r");
  }

}


void m_iap_process(void)
{
  uint8_t key;
  m_iap_init();
  
	printf_menu();

  while(1)
  {
    m_scanf("%c",&key);
    if(key == '1')
    {
      m_printf("\r\nDownload IMage to the OBJ Internal Flash ... \r\n\n");
      m_iap_status = M_WAITING_FILENAME; //准备升级，需要接收文件，此时应该定时发送 0x43 'C',请求发送端发送YMODEM报头
      SerialDownload();
      
    }
    if(key == '2')
    {
      m_printf("\r\nUpload IMage from the OBJ Internal Flash ... \r\n\n");
    }
    if(key == '3')
    {
      m_printf("\r\nExecute the new program ... \r\n\n");
      JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
      /* Jump to user application */
      Jump_To_Application = (pFunction) JumpAddress;
      /* Initialize user application's Stack Pointer */
      __set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
      Jump_To_Application();
    }
    if((key == '4')&& (FlashProtection ==1))
    {
      m_printf("\r\nDisable the write protection ... \r\n\n");
    }
    if(key == '0')
    {
      printf_menu();
    }
  }
  
}
