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
#include <string.h>
#include <semaphore.h>
#include <sys/time.h>

#include "d_sem.h"
#include "d_base.h"

typedef struct stSemValue {
    UINT32 uiMagic;
	sem_t  semaphore;
	VOID * pData;
}SemValue,*PSemValue;


UINT32 NS_SemInit(char * keys)
{
    return OK;
}

UINT32 NS_SemFini()
{
    return OK;
}


UINT32 NS_SemP(SemHanle pSemv)
{
    PSemValue pstSem = pSemv;

	if(NULL != pstSem)
	{
		return sem_wait(&pstSem->semaphore);
	}
	return INVAL;
}

UINT32 NS_SemTryP(SemHanle pSemv)
{
    PSemValue pstSem = pSemv;
	if(NULL != pstSem)
	{
		return sem_trywait(&pstSem->semaphore);
	}
	return INVAL;
}

UINT32 NS_SemTimedP(SemHanle pSemv , UINT32 ulMsec )
{
    PSemValue pstSem = pSemv;
    struct timespec time;
    UINT32 nsec;

	if(NULL != pstSem)
	{
        clock_gettime(CLOCK_REALTIME, &time);

        time.tv_sec  += ulMsec / 1000;
        nsec = (ulMsec % 1000) * 1000000;

        /* 毫秒转纳秒 */
        if(999999999 < (time.tv_nsec + nsec) )
        {
            time.tv_sec += 1;
            time.tv_nsec = time.tv_nsec + nsec - 999999999;
        }
        else
        {
            time.tv_nsec += nsec;
        }

		return sem_timedwait(&pstSem->semaphore,&time);
	}
	return INVAL;
}

UINT32 NS_SemV(SemHanle pSemv)
{
    PSemValue pstSem = pSemv;
	if(NULL != pstSem)
	{
		return sem_post(&pstSem->semaphore);
	}
	return INVAL;
}

UINT32 NS_SemDelete(SemHanle pSemv)
{
    UINT32    uiReturn;
    PSemValue pstSem = pSemv;
	if(NULL != pstSem)
	{
	    uiReturn = sem_destroy(&pstSem->semaphore);
	    free(pstSem);
		return uiReturn;
	}
	return INVAL;
}

UINT32 NS_SemCreate(SemHanle * pSemv)
{
    UINT32    uiReturn;
    PSemValue pstSem;

    if( NULL == pSemv )
    {
        return INVAL;
    }

    pstSem = malloc(sizeof(SemValue));
    if(NULL == pstSem )
    {
        return NOMEM;
    }

    uiReturn = sem_init(&pstSem->semaphore,0,0);
    if(uiReturn)
    {
        free(pstSem);
        return uiReturn;
    }

    *pSemv = pstSem;

	return OK;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

