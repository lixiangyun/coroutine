


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

#define ns_stdin  0
#define ns_stdout 1
#define ns_stderr 2

#define MAX_FORMAT_LEN 128

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
        char     nbuf[MAX_FORMAT_LEN] = {0};
    va_list  argptr;
    int      cnt;
    int      ret;

    ret = snprintf(nbuf,MAX_FORMAT_LEN,"\r\n[LOG-ERROR] func(%s) line(%d):",func,uiLine);
    if( ret < 0 )
    {
        return;
    }

    cnt = ret;

    va_start(argptr, pformat);
    ret = vsnprintf(nbuf + cnt , MAX_FORMAT_LEN - cnt,pformat, argptr);
    va_end(argptr);

    cnt += ret;

    write(ns_stderr, nbuf, cnt);
}

VOID nsloginfo(const char * func,
                   const char * file,
                   UINT32 uiLine,
                   const char * pformat,
                   ...)
{
    char     nbuf[MAX_FORMAT_LEN] = {0};
    va_list  argptr;
    int      cnt;
    int      ret;

    ret = snprintf(nbuf,MAX_FORMAT_LEN,"\r\n[LOG-INFO] func(%s) line(%d):",func,uiLine);
    if( ret < 0 )
    {
        return;
    }

    cnt = ret;

    va_start(argptr, pformat);
    ret = vsnprintf(nbuf + cnt , MAX_FORMAT_LEN - cnt,pformat, argptr);
    va_end(argptr);

    cnt += ret;

    write(ns_stdout,nbuf,cnt);
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


