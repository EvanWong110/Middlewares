/**
 * @file
 * Timer implementations
 */
 
#include "m_timeouts.h"
 
static m_tm_tcb *ptm_list_header;
static uint32_t m_timeouts_last_time;	//上次触发的时间。

uint32_t tm_get_now(void)
{
	return HAL_GetTick();
}

//定时器初始化
void m_timeout_init(void)
{
	ptm_list_header = NULL;
}
 
 //添加定时器，单次运行；
int8_t m_timeout_add(m_tm_tcb *tm)
{
	uint32_t diff=0,now,msecs;
	m_tm_tcb *p;
	
	now = tm_get_now();
	
	//链表为空
	M_ENTER_CRITICAL();
	if(ptm_list_header == NULL)
	{
		m_timeouts_last_time = now;
		ptm_list_header = tm;
		tm->next = NULL;
		M_EXIT_CRITICAL();
		return 0;
	}
	else
	{
		diff = now - m_timeouts_last_time;
		msecs = tm->time;
		tm->time += diff;
	}

	if(ptm_list_header->time > tm->time)
	{
		ptm_list_header->time -= tm->time;
		tm->next = ptm_list_header;
		ptm_list_header = tm;
	}
	else
	{
		for(p = ptm_list_header; p!=NULL; p=p->next)
		{
			tm->time -= p->time;
			if(p->next == NULL || p->next->time > tm->time)
			{
				if(p->next != NULL)
				{
					p->next->time -= tm->time;
					
				}
				else if(tm->time > msecs)
				{
					tm->time = msecs+ptm_list_header->time;
				}
				tm->next = p->next;
				p->next = tm;
				break;
			}
		}
	}
	M_EXIT_CRITICAL();
	return 0;
	
}

//删除定时器
int8_t m_timeout_delete(m_tm_tcb *tm)
{
	m_tm_tcb *prev, *t;
	M_ENTER_CRITICAL();
	for(t=ptm_list_header, prev=NULL; t!=NULL; prev=t, t=t->next)
	{
		if(t == tm)
		{
			if(prev == NULL)
			{
				ptm_list_header = t->next;
			}
			else
			{
				prev->next = t->next;
			}
			M_EXIT_CRITICAL();
			return 0;
		}
	}
	
	M_EXIT_CRITICAL();
	return -1;
}

//定时器处理函数
void m_timeout_process(void)
{
	m_tm_tcb *tmptm = ptm_list_header;
	
	uint32_t diff = tm_get_now() - m_timeouts_last_time;
	
	while(tmptm && (diff >= tmptm->time))
	{
		diff -= tmptm->time;
		
		M_ENTER_CRITICAL();
		m_timeouts_last_time += tmptm->time;
		ptm_list_header = tmptm->next;
		M_EXIT_CRITICAL();
		
		if(tmptm->period)
		{
			tmptm->time = tmptm->period;
			m_timeout_add(tmptm);
		}
    
		if(tmptm->phandler)
			tmptm->phandler(tmptm->pdata);
		
		tmptm = ptm_list_header;
	}
}

