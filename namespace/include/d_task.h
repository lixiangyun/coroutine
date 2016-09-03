/********************************************
*
*
*  任务管理模块
*
*
********************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#include <pthread.h>
#include "d_base.h"


#ifndef _TASK_H_
#define _TASK_H_

typedef VOID * TASK_HANDLE;

#define NS_MAX_NAME_LEN 32

#define NS_ERR_TASK_SAME_NAME    NS_ERROR_OFFSET(NS_TASK_MODULE,1)
#define NS_ERR_TASK_NO_FIND      NS_ERROR_OFFSET(NS_TASK_MODULE,2)


///"TASK"
#define NS_TASK_HANDLE_MAGIC      0x5441534B

#define NS_TASK_STATUS_BLOCK      0x1
#define NS_TASK_STATUS_RUN        0x2
#define NS_TASK_STATUS_SLEEP      0x3


/*

线程的调度有三种策略：SCHED_OTHER、SCHED_RR和SCHED_FIFO。Policy
用于指明使用哪种策略。下面简单的说明一下这三种调度策略。

SCHED_OTHER

它是默认的线程分时调度策略，所有的线程的优先级别都是0，
线程的调度是通过分时来完成的。简单地说，如果系统使用这种调度策略，
程序将无法设置线程的优先级。请注意，这种调度策略也是抢占式的，
当高优先级的线程准备运行的时候，当前线程将被抢占并进入等待队列。
这种调度策略仅仅决定线程在可运行线程队列中的具有相同优先级的线程的运行次序。

SCHED_FIFO

它是一种实时的先进先出调用策略，且只能在超级用户下运行。这种调用策略
仅仅被使用于优先级大于0的线程。它意味着，使用SCHED_FIFO的可运行线程将
一直抢占使用SCHED_OTHER的运行线程J。此外SCHED_FIFO是一个非分时的简单
调度策略，当一个线程变成可运行状态，它将被追加到对应优先级队列的尾部
((POSIX 1003.1)。当所有高优先级的线程终止或者阻塞时，它将被运行。对于
相同优先级别的线程，按照简单的先进先运行的规则运行。我们考虑一种很坏的
情况，如果有若干相同优先级的线程等待执行，然而最早执行的线程无终止或者
阻塞动作，那么其他线程是无法执行的，除非当前线程调用如pthread_yield之
类的函数，所以在使用SCHED_FIFO的时候要小心处理相同级别线程的动作。

SCHED_RR

鉴于SCHED_FIFO调度策略的一些缺点，SCHED_RR对SCHED_FIFO做出了一些增强
功能。从实质上看，它还是SCHED_FIFO调用策略。它使用最大运行时间来限制
当前进程的运行，当运行时间大于等于最大运行时间的时候，当前线程将被切
换并放置于相同优先级队列的最后。这样做的好处是其他具有相同级别的线程
能在“自私“线程下执行。

*/

typedef enum tagTaskModule {
    /* 两个实时调度策略 */
    NS_TASK_SCH_FIFO   = SCHED_FIFO,
    NS_TASK_SCH_RR     = SCHED_RR,
    /* 默认一般调度策略 */
    NS_TASK_SCH_NORMAL = SCHED_OTHER,
}NS_TASK_SCHEDULE;



typedef enum tagTaskPriority {
    /* 实时调度的优先级 */
    NS_TASK_PRIORITY_9  = 99,
    NS_TASK_PRIORITY_8  = 80,
    NS_TASK_PRIORITY_7  = 70,
    NS_TASK_PRIORITY_6  = 60,
    NS_TASK_PRIORITY_5  = 50,
    NS_TASK_PRIORITY_4  = 40,
    NS_TASK_PRIORITY_3  = 30,
    NS_TASK_PRIORITY_2  = 20,
    NS_TASK_PRIORITY_1  = 10,
    NS_TASK_PRIORITY_0  =  0,
}NS_TASK_PRIORITY;


typedef VOID (*TASK_FUNC)(UINT32 ulParam,VOID * pParam);



TASK_HANDLE NS_TaskCurrentGet();

UINT32 NS_TaskBlock(TASK_HANDLE TaskHandle , UINT32 ulMsec);

UINT32 NS_TaskWakeup(TASK_HANDLE TaskHandle);

CHAR * NS_TaskNameGet(TASK_HANDLE TaskHandle);

UINT32 NS_TaskHandleValid(TASK_HANDLE TaskHandle);

UINT32 NS_TaskIDGet(CHAR * pcTaskName,TASK_HANDLE * pTaskHandle);

UINT32 NS_TaskDelete(TASK_HANDLE TaskHandle);

UINT32 NS_TaskJoin(TASK_HANDLE TaskHandle);

UINT32 NS_TaskCreate(TASK_HANDLE *  pTaskHandle,
					  CHAR        *  pcTaskName,
					  TASK_FUNC      pTaskFunc,
					  UINT32         ulParam,
					  VOID        *  pParam);

UINT32 NS_TaskCreateEx(TASK_HANDLE     * pTaskHandle,
    			        CHAR            * pcTaskName,
                        NS_TASK_SCHEDULE eTaskModule,
                        NS_TASK_PRIORITY eTaskPrior,
    				    TASK_FUNC         pTaskFunc,
    				    UINT32            ulParam,
    				    VOID            * pParam);

VOID NS_TaskDelay(UINT32 ulMillisecond);


#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

