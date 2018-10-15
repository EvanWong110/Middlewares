/**
 * @file
 * M_IAP implementations
 */

#include "m_iap.h"

//自动引导超时时间，单位，iap_tick,100ms
#define M_IAP_TICK     (100)

#define AUTOBOOT_TIME  (50)


extern uint8_t FileName[FILE_NAME_LENGTH];

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
	static uint32_t ns_time = AUTOBOOT_TIME;
	if(m_iap_status == M_IDLE)
	{
		if(ns_time%10 == 0)
			m_printf("%d\r\n",ns_time/10);	
		
		ns_time--;
			
		if(ns_time > AUTOBOOT_TIME)
		{
			/*中断内调整会造成APP内所有跟中断相关的功能失灵，
			应该跟中断退出时出栈（对应进中断时的压栈操作），
			cotex-m 进中断时由硬件执行各寄存器压栈及出栈操作
			操作未正常执行有关。*/
			m_iap_status = M_BOOTING; 
		}
	}
	else 
		ns_time = AUTOBOOT_TIME;
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
	m_printf("\r\n=================M_IAP MAIN MENU_v0.1==================\r\n\n");
  m_printf("Show the main menu ------------------------------------0\r\n\n");
	m_printf("Download IMage(*.bin) to the OBJ Internal Flash -------1\r\n\n");
	m_printf("Upload IMage(*.bin) from the OBJ Internal Flash -------2\r\n\n");
	m_printf("Execute the new program -------------------------------3\r\n\n");
  if(FlashProtection == 1)
    m_printf("Disable the write protection ------------------------4\r\n\n");
  m_printf("=======================================================\r\n\n");
  
}


void SerialDownload(void)
{
  
uint8_t Number[10] = "          ";
  int32_t Size = 0;
	uint32_t tick = 0;
  m_printf("Waiting for the file to be sent ... (press 'a' to abort)\n\r");
  Size = Ymodem_Receive(&tab_1024[0]);
	tick = HAL_GetTick();
	while(HAL_GetTick() - tick < 3000);
  if (Size > 0)
  {
    m_printf("\n\n\r Programming Completed Successfully!\n\r--------------------------------\r\n Name: ");
    m_printf("%s",FileName);
    Int2Str(Number, Size);
    m_printf("\n\r Size: ");
    m_printf("%s",Number);
    m_printf(" Bytes\r\n");
    m_printf("-------------------\r\n");
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
void SerialUpload(void)
{
  uint8_t key, status = 0 ; 

  m_printf("\n\n\rSelect Receive File\n\r");

//	m_scanf("%c",&key);
	Receive_Byte(&key,HAL_MAX_DELAY);
	
  if (key == CRC16)
  {
    /* Transmit the flash image through ymodem protocol */
    status = Ymodem_Transmit((uint8_t*)APPLICATION_ADDRESS, (const uint8_t*)"UploadedFlashImage.bin", USER_FLASH_SIZE);

    if (status != 0) 
    {
      m_printf("\n\rError Occurred while Transmitting File\n\r");
    }
    else
    {
      m_printf("\n\rFile uploaded successfully \n\r");
    }
  }
}
extern int32_t Receive_Byte (uint8_t *c, uint32_t Timeout);
void m_iap_process(void)
{
  volatile uint8_t key;
  m_iap_init();
  
	m_printf("\r\n%d milliseconds no operation will automatically boot the system\r\n\n", AUTOBOOT_TIME*M_IAP_TICK);
	
	printf_menu();

  while(1)
  {
		key = 0;
		Receive_Byte(&key,100);
			//continue;
    //m_scanf("%c",&key); //完全阻塞进程，中断中也不能使用printf
    if(key == '1')
    {
      m_printf("\r\nDownload IMage(*.bin) to the OBJ Internal Flash ... \r\n\n");
      m_iap_status = M_WAITING; //准备升级，需要接收文件
      SerialDownload();
			//m_iap_status = M_IDLE;
    }
    if(key == '2')
    {
      m_printf("\r\nUpload IMage(*.bin) from the OBJ Internal Flash ... \r\n\n");
			m_iap_status = M_UPLOAD;
			SerialUpload();
			//m_iap_status = M_WAITING;
    }
    if((key == '3') || (m_iap_status==M_BOOTING))
    {
			m_iap_status = M_BOOTING;
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
			m_iap_status = M_WAITING;
    }
    if(key == '0')
    {
      printf_menu();
			m_iap_status = M_WAITING;
    }
  }
  
}
