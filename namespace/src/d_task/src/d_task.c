/********************************************
*
*
*  任务管理
*
*
********************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/select.h>
#include <errno.h>

#include "d_task.h"
#include "d_sem.h"
#include "d_list.h"
#include "d_mem.h"
#include "d_lock.h"

typedef struct tagtaskinfo {
    UINT32        ulMagic;
	UINT32        ulParam;
	VOID *        pParam;
	VOID *        pResult;
	TASK_FUNC     pthreadFunc;
	pthread_t     pthreadID;
	CHAR          cTaskName[NS_MAX_NAME_LEN];
	NS_LIST_NODE stListNode;
    SemHanle      stSem;
    UINT32        ulStatus;
}TaskInfo;


typedef struct tagtaskctl {
    LockHandle      TaskCtlMutex;
	UINT32          ulCountNewTask;
	UINT32          ulCountDelTask;
	NS_LIST_NODE   stListHead;
}TaskCtl;

TaskCtl TaskContalBlock;

#define TASK_LOCK_MUTEX()     NS_MutexLock(TaskContalBlock.TaskCtlMutex)
#define TASK_UNLOCK_MUTEX()   NS_MutexUnLock(TaskContalBlock.TaskCtlMutex)


/* 外面需要加锁 */
VOID DmsDeleteTaskInfo(TaskInfo * pOneTaskInfo)
{
	TaskContalBlock.ulCountDelTask++;
	NS_LIST_REMOVE(&(pOneTaskInfo->stListNode));
	pOneTaskInfo->ulMagic = 0;
	NS_SemDelete(pOneTaskInfo->stSem);
	NS_MemFree(pOneTaskInfo);//释放内存
}


VOID * DmsTaskShell(TaskInfo * pOneTaskInfo)
{
    INT32 oldType;
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,&oldType);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&oldType);
    NS_SemP(pOneTaskInfo->stSem);
	/* 执行用户任务 */
	pOneTaskInfo->pthreadFunc(pOneTaskInfo->ulParam,pOneTaskInfo->pParam);
	/* 删除该任务信息 */
	TASK_LOCK_MUTEX();
	DmsDeleteTaskInfo(pOneTaskInfo);
	TASK_UNLOCK_MUTEX();
	/* 释放该线程所占用的内存资源（线程内核对象和线程堆栈）*/
	pthread_detach (pthread_self());
	return NULL;
}

UINT32 NS_TaskInit(char * keys)
{
	UINT32 ulRet;

	NS_LIST_INIT(&(TaskContalBlock.stListHead));

	TaskContalBlock.ulCountDelTask    = 0;
	TaskContalBlock.ulCountNewTask    = 0;

	/* 创建一个锁，用来保护并行创建删除任务 */
	ulRet = NS_MutexCreate(&TaskContalBlock.TaskCtlMutex);
	if(0 != ulRet)
	{
		return INVAL;
	}

	return OK;
}

UINT32 NS_TaskFini(VOID)
{
	return OK;
}

TASK_HANDLE NS_TaskCurrentGet()
{
    NS_LIST_NODE * pstListHead = &(TaskContalBlock.stListHead);
    NS_LIST_NODE * pstListNode;
	TaskInfo      * pTaskInfoTmp;
    pthread_t       pthreadID = pthread_self();

    TASK_LOCK_MUTEX();
    NS_LIST_TRAVERSAL(pstListNode,pstListHead)
    {
        pTaskInfoTmp = NS_LIST_OFFSET(pstListNode,TaskInfo,stListNode);

        if(pTaskInfoTmp->pthreadID == pthreadID)
        {
            TASK_UNLOCK_MUTEX();
            return pTaskInfoTmp;
        }
    }
    TASK_UNLOCK_MUTEX();

    return NULL;
}


UINT32 NS_TaskBlock(TASK_HANDLE TaskHandle , UINT32 ulMsec)
{
	TaskInfo * pTaskInfoTmp;
	UINT32     ulRet;

	if(NULL == TaskHandle)
	{
        return INVAL;
	}

	TASK_LOCK_MUTEX();
    pTaskInfoTmp = (TaskInfo *)TaskHandle;

    if(NS_TASK_HANDLE_MAGIC == pTaskInfoTmp->ulMagic)
    {
        if(0 == ulMsec)
        {
            pTaskInfoTmp->ulStatus = NS_TASK_STATUS_BLOCK;
            TASK_UNLOCK_MUTEX();

            ulRet = NS_SemP(pTaskInfoTmp->stSem);
            pTaskInfoTmp->ulStatus = NS_TASK_STATUS_RUN;

            return ulRet;
        }
        else
        {
            pTaskInfoTmp->ulStatus = NS_TASK_STATUS_BLOCK;
            TASK_UNLOCK_MUTEX();

            ulRet = NS_SemTimedP(pTaskInfoTmp->stSem,ulMsec);
            pTaskInfoTmp->ulStatus = NS_TASK_STATUS_RUN;

            return ulRet;
        }
    }

	TASK_UNLOCK_MUTEX();
	return INVAL;
}

UINT32 NS_TaskWakeup(TASK_HANDLE TaskHandle)
{
	TaskInfo * pTaskInfoTmp;
    UINT32     ulRet = OK;

	if(NULL == TaskHandle)
	{
        return INVAL;
	}

	TASK_LOCK_MUTEX();
    pTaskInfoTmp = (TaskInfo *)TaskHandle;

    if(NS_TASK_HANDLE_MAGIC == pTaskInfoTmp->ulMagic)
    {
        if( NS_TASK_STATUS_BLOCK == pTaskInfoTmp->ulStatus )
        {
            ulRet = NS_SemV(pTaskInfoTmp->stSem);
        }

        TASK_UNLOCK_MUTEX();
        return ulRet;
    }

	TASK_UNLOCK_MUTEX();
	return INVAL;

}


UINT32 NS_TaskCreateEx(TASK_HANDLE     * pTaskHandle,
    			        CHAR            * pcTaskName,
                        NS_TASK_SCHEDULE eTaskModule,
                        NS_TASK_PRIORITY eTaskPrior,
    				    TASK_FUNC         pTaskFunc,
    				    UINT32            ulParam,
    				    VOID            * pParam)
{
    UINT32          ulRet;
	TaskInfo      * pOneTaskInfo;
    NS_LIST_NODE * pstListHead = &(TaskContalBlock.stListHead);
    NS_LIST_NODE * pstListNode;

    pthread_attr_t attr;
    struct sched_param param;

	if((NULL == pTaskHandle)||(NULL == pTaskFunc)||(NULL == pcTaskName))
	{
		return INVAL;
	}

    TASK_LOCK_MUTEX();

    NS_LIST_TRAVERSAL(pstListNode,pstListHead)
    {
        pOneTaskInfo = NS_LIST_OFFSET(pstListNode,TaskInfo,stListNode);
        if(0 == strcmp(pcTaskName, pOneTaskInfo->cTaskName))
        {
            TASK_UNLOCK_MUTEX();
            *pTaskHandle = NULL;
            return EXIST;
        }
    }

	pOneTaskInfo = (TaskInfo *)NS_MemAlloc(sizeof(TaskInfo));
	if(NULL == pOneTaskInfo)
	{
        TASK_UNLOCK_MUTEX();
        *pTaskHandle = NULL;
        return NOMEM;
	}

    ulRet = NS_SemCreate(&(pOneTaskInfo->stSem));
    if( OK != ulRet)
    {
        TASK_UNLOCK_MUTEX();
        *pTaskHandle = NULL;
        NS_MemFree(pOneTaskInfo);
        return ulRet;
    }

    (VOID)pthread_attr_init(&attr);
    ulRet = pthread_attr_setschedpolicy(&attr,eTaskModule);
    if(OK != ulRet)
    {
        TASK_UNLOCK_MUTEX();
        *pTaskHandle = NULL;
        NS_SemDelete(pOneTaskInfo->stSem);
		NS_MemFree(pOneTaskInfo);
		return ulRet;
    }

    param.sched_priority = eTaskPrior;
    ulRet = pthread_attr_setschedparam(&attr, &param);
    if(OK != ulRet)
    {
        TASK_UNLOCK_MUTEX();
        *pTaskHandle = NULL;
        NS_SemDelete(pOneTaskInfo->stSem);
		NS_MemFree(pOneTaskInfo);
		return ulRet;
    }

	pOneTaskInfo->ulParam     = ulParam;
	pOneTaskInfo->pParam      = pParam;
	pOneTaskInfo->pthreadFunc = pTaskFunc;
	pOneTaskInfo->ulMagic     = NS_TASK_HANDLE_MAGIC;
	pOneTaskInfo->ulStatus    = NS_TASK_STATUS_RUN;

	ulRet = pthread_create( &(pOneTaskInfo->pthreadID),
							&attr,
							(VOID*(*)(VOID*))DmsTaskShell,
							pOneTaskInfo );
	if(OK != ulRet)
	{
        TASK_UNLOCK_MUTEX();
        *pTaskHandle = NULL;
        NS_SemDelete(pOneTaskInfo->stSem);
		NS_MemFree(pOneTaskInfo);
		return INVAL;
	}

	(VOID)pthread_attr_destroy(&attr);

	TaskContalBlock.ulCountNewTask++;

    strncpy(pOneTaskInfo->cTaskName, pcTaskName, NS_MAX_NAME_LEN);

	NS_LIST_ADD(&(pOneTaskInfo->stListNode), &(TaskContalBlock.stListHead));
	*pTaskHandle = pOneTaskInfo;

    NS_SemV(pOneTaskInfo->stSem);

	TASK_UNLOCK_MUTEX();
	return OK;
}


UINT32 NS_TaskCreate(TASK_HANDLE *  pTaskHandle,
					  CHAR        *  pcTaskName,
					  TASK_FUNC      pTaskFunc,
					  UINT32         ulParam,
					  VOID        *  pParam)
{
	return NS_TaskCreateEx(pTaskHandle,
                            pcTaskName,
                            NS_TASK_SCH_NORMAL,
                            NS_TASK_PRIORITY_0,
       				        pTaskFunc,
       				        ulParam,
       				        pParam);
}

UINT32 NS_TaskDelete(TASK_HANDLE TaskHandle)
{
	TaskInfo      * pTaskInfoTmp;
	UINT32          ulRet;

	if(NULL == TaskHandle)
	{
        return INVAL;
	}

	TASK_LOCK_MUTEX();
    pTaskInfoTmp = (TaskInfo *)TaskHandle;

    if(NS_TASK_HANDLE_MAGIC == pTaskInfoTmp->ulMagic)
    {
        ulRet = pthread_cancel(pTaskInfoTmp->pthreadID);
        if(OK != ulRet )
        {
            TASK_UNLOCK_MUTEX();
            return ulRet;
        }
        pthread_detach (pTaskInfoTmp->pthreadID);
        DmsDeleteTaskInfo(pTaskInfoTmp);
        TASK_UNLOCK_MUTEX();
        return OK;
    }

	TASK_UNLOCK_MUTEX();
	return INVAL;
}


UINT32 NS_TaskJoin(TASK_HANDLE TaskHandle)
{
	TaskInfo      * pTaskInfoTmp;
	VOID          * pRetVal = NULL;
	UINT32          ulRet;
    pthread_t       pthreadIDtmp;

	if(NULL == TaskHandle)
	{
        return INVAL;
	}

	TASK_LOCK_MUTEX();
    pTaskInfoTmp = (TaskInfo *)TaskHandle;

    if(NS_TASK_HANDLE_MAGIC == pTaskInfoTmp->ulMagic)
    {
        pthreadIDtmp = pTaskInfoTmp->pthreadID;
        TASK_UNLOCK_MUTEX();
        ulRet = pthread_join(pthreadIDtmp, &pRetVal);
        if(OK != ulRet )
        {
            return NOEXIST;
        }
        return OK;
    }

	TASK_UNLOCK_MUTEX();
	return INVAL;
}


UINT32 NS_TaskIDGet(CHAR * pcTaskName,TASK_HANDLE * pTaskHandle)
{
    NS_LIST_NODE * pstListHead = &(TaskContalBlock.stListHead);
    NS_LIST_NODE * pstListNode;
	TaskInfo      * pTaskInfoTmp;
    pthread_t       pthreadID;

    if(NULL == pTaskHandle)
    {
        return INVAL;
    }

	if(NULL == pcTaskName)
	{
        pthreadID = pthread_self();
	}

	TASK_LOCK_MUTEX();
    NS_LIST_TRAVERSAL(pstListNode,pstListHead)
    {
        pTaskInfoTmp = NS_LIST_OFFSET(pstListNode,TaskInfo,stListNode);
        if(NULL == pcTaskName)
        {
            if(pTaskInfoTmp->pthreadID == pthreadID)
            {
                *pTaskHandle = pTaskInfoTmp;
                TASK_UNLOCK_MUTEX();
                return OK;
            }
        }
        else
        {
            if(0 == strcmp(pcTaskName, pTaskInfoTmp->cTaskName))
            {
                *pTaskHandle = pTaskInfoTmp;
                TASK_UNLOCK_MUTEX();
                return OK;
            }
        }
    }
    TASK_UNLOCK_MUTEX();
    return NOEXIST;
}



UINT32 NS_TaskHandleValid(TASK_HANDLE TaskHandle)
{
	TaskInfo  * pTaskInfoTmp;
    UINT32      ulRet;

	if(NULL == TaskHandle)
	{
        return INVAL;
	}

	TASK_LOCK_MUTEX();
    pTaskInfoTmp = (TaskInfo *)TaskHandle;

    if(NS_TASK_HANDLE_MAGIC == pTaskInfoTmp->ulMagic)
    {
        ulRet = pthread_kill(pTaskInfoTmp->pthreadID,0);
        if(OK != ulRet)
        {
            ulRet = NOEXIST;
        }
        TASK_UNLOCK_MUTEX();
        return ulRet;
    }

	TASK_UNLOCK_MUTEX();
	return INVAL;
}

CHAR * NS_TaskNameGet(TASK_HANDLE TaskHandle)
{
	CHAR          * pcTaskName = NULL;
	TaskInfo      * pTaskInfoTmp;

	if(NULL == TaskHandle)
	{
        return pcTaskName;
	}

	TASK_LOCK_MUTEX();
    pTaskInfoTmp = (TaskInfo *)TaskHandle;
    if(NS_TASK_HANDLE_MAGIC == pTaskInfoTmp->ulMagic)
    {
        pcTaskName = pTaskInfoTmp->cTaskName;
    }
    TASK_UNLOCK_MUTEX();

    return pcTaskName;
}


/*

struct timeval {
    long tv_sec;       // seconds
    long tv_usec;      // microseconds
};

*/

VOID NS_TaskDelay(UINT32 ulMillisecond)
{
    int ret;
    struct timeval tv;
    tv.tv_sec  = (ulMillisecond / 1000);
    tv.tv_usec = (ulMillisecond % 1000) * 1000;

    do
    {
        ret = select(0, NULL, NULL, NULL, &tv);
    }
    while(-1 == ret && EINTR == errno);
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

