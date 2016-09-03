/********************************************
*  系统时间
*
********************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "d_time.h"
#include <time.h>


UINT32 NS_TimeInit(char * keys)
{
    return OK;
}

UINT32 NS_TimeFini()
{
    return OK;
}


UINT32 NS_GetCpuTick(CPUTICK * pCpuTick)
{
	UINT32 ulLow,ulHigh;

	if(NULL != pCpuTick)
	{
		__asm__ __volatile__ ("RDTSC" : "=a"(ulLow) , "=d"(ulHigh): );
		pCpuTick->ulHigh = ulHigh;
		pCpuTick->ulLow  = ulLow;
		return OK;
	}
	return INVAL;
}


UINT32 NS_AddCpuTick(CPUTICK * pAdded,CPUTICK * pAdd)
{
	if(( NULL == pAdded )||( NULL == pAdd ))
	{
		return INVAL;
	}

	pAdded->ulHigh += pAdd->ulHigh;
	if( pAdded->ulLow > ( pAdded->ulLow + pAdd->ulLow ) )
	{
		pAdded->ulHigh ++ ;
	}

	pAdded->ulLow = pAdded->ulLow + pAdd->ulLow ;
	return OK;
}

UINT32 NS_SubCpuTick(CPUTICK * pSubed,CPUTICK * pSub)
{
	if(( NULL == pSubed )||( NULL == pSub ))
	{
		return INVAL;
	}

	if( pSubed->ulHigh > pSub->ulHigh )
	{
		pSubed->ulHigh -= pSub->ulHigh;
		if( pSubed->ulLow < pSub->ulLow )
		{
			pSubed->ulHigh -- ;
		}
	}
	else if( pSubed->ulHigh == pSub->ulHigh )
	{
		pSubed->ulHigh = 0;
		if( pSubed->ulLow < pSub->ulLow )
		{
			return INVAL;
		}
	}
	else
	{
		return INVAL;
	}

	pSubed->ulLow -= pSub->ulLow;

	return OK;
}


UINT32 NS_GetSysTime(SYSTIME * pstSysTime)
{
    time_t now;
    struct tm * timenow;

    if(NULL == pstSysTime)
    {
        return INVAL;
    }

    time(&now);

    timenow = localtime(&now);

    pstSysTime->ulSecond = timenow->tm_sec;
    pstSysTime->ulMinute = timenow->tm_min;
    pstSysTime->ulHour   = timenow->tm_hour;
    pstSysTime->ulMDay   = timenow->tm_mday;
    pstSysTime->ulMonth  = timenow->tm_mon  + 1;
    pstSysTime->ulYear   = timenow->tm_year + 1900;
    pstSysTime->ulWeek   = timenow->tm_wday;
    pstSysTime->ulYDay   = timenow->tm_yday + 1;

    return OK;
}

CHAR * NS_GetSysTimeStr(SYSTIME * pstSysTime)
{
    static CHAR cSysTimeToStr[NS_TIME_TO_STRING_MAX_LENGTH];

    if(NULL == pstSysTime)
    {
        return NULL;
    }

    snprintf(cSysTimeToStr,NS_TIME_TO_STRING_MAX_LENGTH,"%04u-%02u-%02u %02u-%02u-%02u",
             pstSysTime->ulYear,
             pstSysTime->ulMonth,
             pstSysTime->ulMDay,
             pstSysTime->ulHour,
             pstSysTime->ulMinute,
             pstSysTime->ulSecond);

    return cSysTimeToStr;
}

CHAR * NS_GetSysTimeNowStr(VOID)
{
    SYSTIME stSysTime;
    (VOID)NS_GetSysTime(&stSysTime);
    return NS_GetSysTimeStr(&stSysTime);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

