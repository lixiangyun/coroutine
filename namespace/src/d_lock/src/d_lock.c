/********************************************
*
* 线程信号量
*
********************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "d_lock.h"
#include "d_base.h"

extern int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);

typedef struct tagMutexLock {
    UINT32 uiMagic;
    pthread_mutex_t stlock;
    VOID * pData;
}MLOCK;


UINT32 NS_LockInit(char * keys)
{
    return OK;
}

UINT32 NS_LockFini()
{
    return OK;
}


UINT32 NS_MutexCreate(LockHandle * plock)
{
    pthread_mutexattr_t attr;
    MLOCK * pstMLock;

    if(NULL == plock)
    {
        LOG("The Param is null\n");
        return INVAL;
    }

    pstMLock = malloc(sizeof(MLOCK));
    if(NULL == pstMLock)
    {
        LOG("malloc %u failed!\n",sizeof(MLOCK));
        return NOMEM;
    }

    (void)pthread_mutexattr_init(&attr);
    (void)pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE_NP);

    if(OK != pthread_mutex_init(&(pstMLock->stlock),&attr))
    {
        free(pstMLock);
        LOG("pthread_mutex_init failed!\n");
        return INVAL;
    }

    *plock = pstMLock;

    return OK;
}

UINT32 NS_MutexLock(LockHandle plock)
{
    MLOCK * pstMLock = plock;

    if(NULL != pstMLock)
    {
        return pthread_mutex_lock(&(pstMLock->stlock));
    }
    return INVAL;
}

UINT32 NS_MutexUnLock(LockHandle plock)
{
    MLOCK * pstMLock = plock;

    if(NULL != pstMLock)
    {
        return pthread_mutex_unlock(&(pstMLock->stlock));
    }
    return INVAL;
}

UINT32 NS_MutexDelete(LockHandle plock)
{
    UINT32  uiReturn;
    MLOCK * pstMLock = plock;

    if(NULL == pstMLock)
    {
        return INVAL;
    }

    uiReturn = pthread_mutex_destroy(&pstMLock->stlock);
    if( OK != uiReturn )
    {
        return uiReturn;
    }

    free(pstMLock);
    return OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

