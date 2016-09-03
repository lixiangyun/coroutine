


/********************************************
*
*
********************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#include "d_base.h"


#define MAX_FORMAT_LEN 1024

typedef VOID (* OSAL_LOG_WRITE_HOOK)(UINT32 ulLevel,
                                     UINT32 ulErrId,
                                     const CHAR * pFile,
                                     UINT32 ulLine,
                                     const CHAR * pFormat,
                                     ...);

OSAL_LOG_WRITE_HOOK m_pfnOsalLogWriteHook = NULL;

VOID nslogwrite(const char * func,
                   const char * file,
                   UINT32 uiLine,
                   const char * pformat,
                   ...)
{
    char     nbuf[1024];
    va_list  argptr;
    int      cnt;

    snprintf(nbuf,MAX_FORMAT_LEN,"\r\n[LOG-ERROR]:func(%s),file(%s),line(%d): ",func,file,uiLine);

    cnt = strlen(nbuf);

    va_start(argptr, pformat);
    cnt = vsnprintf(nbuf + cnt ,MAX_FORMAT_LEN - cnt,pformat, argptr);
    va_end(argptr);

    cnt = strlen(nbuf);

    write(2,nbuf,cnt);

}

VOID nsloginfo(const char * func,
                   const char * file,
                   UINT32 uiLine,
                   const char * pformat,
                   ...)
{
    char     nbuf[1024];
    va_list  argptr;
    int      cnt;

    snprintf(nbuf,MAX_FORMAT_LEN,"\r\n[LOG-INFO]:func(%s),file(%s),line(%d): ",func,file,uiLine);

    cnt = strlen(nbuf);

    va_start(argptr, pformat);
    cnt = vsnprintf(nbuf + cnt , MAX_FORMAT_LEN - cnt,pformat, argptr);
    va_end(argptr);

    cnt = strlen(nbuf);

    write(1,nbuf,cnt);

}


VOID NSLogWirteHookSet(VOID * pfnLogWriteFunc )
{
    m_pfnOsalLogWriteHook = (OSAL_LOG_WRITE_HOOK)pfnLogWriteFunc;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


