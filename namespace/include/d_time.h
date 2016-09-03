/********************************************
*  系统时间模块
*
********************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "d_base.h"

#ifndef __D_TIME_H__
#define __D_TIME_H__
#endif

#define NS_TIME_TO_STRING_MAX_LENGTH 20

/*

The members of the tm structure are:

tm_sec    The number of seconds after the minute, normally in the range 0 to  59,
          but can be up to 60 to allow for leap seconds.
tm_min    The number of minutes after the hour, in the range 0 to 59.
tm_hour   The number of hours past midnight, in the range 0 to 23.
tm_mday   The day of the month, in the range 1 to 31.
tm_mon    The number of months since January, in the range 0 to 11.
tm_year   The number of years since 1900.
tm_wday   The number of days since Sunday, in the range 0 to 6.
tm_yday   The number of days since January 1, in the range 0 to 365.

*/

typedef struct tagSysTime {
    UINT32 ulYear;       /* 1900 - ~ */
    UINT32 ulMonth;      /* 1 - 12   */
    UINT32 ulMDay;       /* 1 - 31   */
    UINT32 ulWeek;       /* 0 - 6    */
    UINT32 ulHour;       /* 0 - 23   */
    UINT32 ulMinute;     /* 0 - 59   */
    UINT32 ulSecond;     /* 0 - 60   */
    UINT32 ulYDay;       /* 0 - 365  */
}SYSTIME;


typedef struct stCpuTick {
	UINT32 ulLow;
	UINT32 ulHigh;
}CPUTICK;


UINT32 NS_GetSysTime(SYSTIME * pstSysTime);

CHAR * NS_GetSysTimeStr(SYSTIME * pstSysTime);

CHAR * NS_GetSysTimeNowStr(VOID);



UINT32 NS_SubCpuTick(CPUTICK * pSubed,CPUTICK * pSub);

UINT32 NS_AddCpuTick(CPUTICK * pAdded,CPUTICK * pAdd);

UINT32 NS_GetCpuTick(CPUTICK * pCpuTick);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

