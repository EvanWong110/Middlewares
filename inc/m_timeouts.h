/**
 * @file
 * Timer implementations
 */
 #ifndef M_TIMEOUTS_H
 #define M_TIMEOUTS_H


#include "stm32f4xx_hal.h"
#include "m_common.h"
//定时器回调函数
typedef void (* m_timeout_handler)(void *arg);

//定时器结构体
typedef struct m_tm_tcb_struct
{
	uint32_t time;	//初次触发时间
	uint32_t period;	//周期时间，如果是只执行1次，则设为0
	void *pdata;		//定时器私有参数
	m_timeout_handler phandler;  //定时器回调函数
	struct m_tm_tcb_struct *next;//链表
}m_tm_tcb;

//定时器初始化
void m_timeout_init(void);

//添加定时器
int8_t m_timeout_add(m_tm_tcb *tm);

//删除定时器
int8_t m_timeout_delete(m_tm_tcb *tm);

//定时器处理函数
void m_timeout_process(void);

#endif
