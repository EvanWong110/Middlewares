/**
 * @file
 * M_IAP implementations
 */

#include "m_iap.h"

//自动引导超时时间，单位，iap_tick,100ms
#define M_IAP_TICK     (100)

#define AUTOBOOT_TIME  (50)

extern int32_t Receive_Byte (uint8_t *c, uint32_t Timeout);
extern uint32_t Send_Byte (uint8_t c);
extern uint8_t FileName[FILE_NAME_LENGTH];

uint8_t tab_1024[1024] =
  {
    0
  };
    
pFunction Jump_To_Application;
uint32_t JumpAddress;

volatile uint32_t FlashProtection = 0;

MENUM_IAP_STATUS m_iap_status = M_IDLE;

//支持的指令
int32_t WrAtAddrFunc(int32_t argc, uint8_t (*argv)[]);
int32_t RdAtAddrFunc(int32_t argc, uint8_t (*argv)[]);

int32_t WrAtAddrFunc(int32_t argc, uint8_t (*argv)[])
{
    int32_t addr=0, value=0;
    uint8_t (*p)[PARA_LENGTH] = (uint8_t (*)[PARA_LENGTH])argv;
    if(argc != 2)
    {
        m_printf("\r\nError para number\r\n");
        return -1;
    }
    else
    {
        if(Str2Int(p[0],&addr) && Str2Int(p[1],&value))
        {
            if(0==EE_WriteVariable((uint16_t)addr,(uint16_t)value))
            {
                m_printf("\r\nWrite value success\r\n");
                return 0;
            }
            else
                m_printf("\r\nWrite value failure\r\n");
        }
        else
        {
            m_printf("\r\nError parameter!\r\n");
            return -1;  
        }
    }
}


int32_t RdAtAddrFunc(int32_t argc, uint8_t (*argv)[])
{
    int32_t addr=0;
    uint16_t value=0;
    int8_t status;
    
    uint8_t (*p)[PARA_LENGTH] = (uint8_t (*)[PARA_LENGTH])argv;
    
    if(argc != 1)
		{
				m_printf("\r\nError para number\r\n");
        return -1;
		}
    else
    {
        if(Str2Int(p[0],&addr))
        {
            status = EE_ReadVariable((uint16_t)addr,&value);
            if(status != 0)
            {
                m_printf("\r\nValue at addr:%d not found!\r\n",addr);
                return -1;
            }
        }
        else
        {
            m_printf("\r\nError parameter!\r\n");
            return -1;  
        }
    }
    m_printf("\r\nRead value = %d, at addr= %d\r\n",value, addr);
    return 0;
}



sSET_TCB sWr = {"wr",sWrAtAddr,WrAtAddrFunc};

sSET_TCB sRd = {"rd",sRdAtAddr,RdAtAddrFunc};

uint16_t sCount = 2;
sSET_TCB *sList[] = {&sWr,&sRd};


#define EEPROMTEST 1
#if EEPROMTEST

#endif 

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
    EE_Init();
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
    m_printf("Set private parameter   -------------------------------4\r\n\n");
    m_printf("EEPROM TEST             -------------------------------5\r\n\n");
    if(FlashProtection == 1)
        m_printf("Disable the write protection ------------------------9\r\n\n");
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

uint16_t Receive_string()
{
    for(uint16_t i=0; i<1024; i++)
    {
        Receive_Byte(&tab_1024[i],HAL_MAX_DELAY);

        Send_Byte(tab_1024[i]);
        //使用空格替代\r \t;
        if((tab_1024[i] == '\n')||(tab_1024[i] == '\t'))
            tab_1024[i] = ' ';
        
        if(tab_1024[i] == '\r')
        {
            tab_1024[i++] = ' ';
            tab_1024[i] = 0;
            return i;
        }
        
        if(i>0)
        {
            //消除多余空格及回车换行符
            if((tab_1024[i] == ' ')&&(tab_1024[i-1]==' '))
                i--;
        }
    }
			return 0;
}

//内部指令解析，并执行相应函数
void SetParameter()
{
    uint16_t setPos,paraPos;
	
    uint16_t len;
	
    int16_t strPos,strPos_bk;
    
    char separator = ' '; 
    
    //保存解析的参数，最多支持8个
    uint8_t rsvPara[PARA_MAXNUM][PARA_LENGTH];
    
    //解析出的指令：
    eSET_VALUE set = sERROR;
    
setparastart:
    len =  Receive_string();
    if(len && (tab_1024[0]== 'a'))
        return;
    
    for(setPos=0; setPos<sCount; setPos++)
    {

        strPos = m_memfind((const char*)&tab_1024[0], sList[setPos]->setString, strlen(sList[setPos]->setString));
        if(strPos != 0)
        {
            continue;
        }
        set = sList[setPos]->v;
        strPos_bk = strlen(sList[setPos]->setString)+1;
        break;
    }
    if(set == sERROR)
        goto setparastart;

    for(paraPos=0; paraPos<PARA_MAXNUM; paraPos++)
    {
        strPos = m_memfind(&tab_1024[strPos_bk],&separator,1);
        if(strPos < 0 || strPos>=PARA_LENGTH-strPos_bk)
            break;
        
        memcpy(rsvPara[paraPos],&tab_1024[strPos_bk],strPos);
        rsvPara[paraPos][strPos] = 0;
        strPos_bk += (strPos+1);
    }

    //执行函数
    sList[setPos]->func(paraPos,rsvPara);

    goto setparastart;

}

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
    
    if(key == '4')
    {
        m_printf("Press 'a' abort, 'wr addr value' write value at addr\r\n\n");

        SetParameter();

        m_printf("Abort, m set!\r\n\n");
        printf_menu();
        m_iap_status = M_WAITING;
    }

    if(key == '5')
    {
        
#if EEPROMTEST
        m_printf("Test EEPROM Wr 128k data at 128 addr---------5\r\n\n");

        for(int i =0; i <(0x80<<10);i++)
        {
            EE_WriteVariable((uint16_t)128,i%(0xffff));
        }
        m_printf("Test EEPROM Wr 128k data at 128 addr done!\r\n\n");
#endif
        m_iap_status = M_WAITING;
    }
    
    if((key == '9')&& (FlashProtection ==1))
    {
      m_printf("\r\nDisable the write protection ... \r\n\n");
			m_iap_status = M_WAITING;
    }
    if((key == '0')||(key=='\t'))
    {
      printf_menu();
			m_iap_status = M_WAITING;
    }
  }
  
}
