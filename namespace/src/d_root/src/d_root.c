
/********************************************
* ÅäÖÃÎÄ¼þ¶ÁÐ´Ä£¿é
*
********************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <unistd.h>
#include <signal.h>

#include "d_base.h"
#include "d_cfg.h"
#include "d_sem.h"

SemHanle g_uiSema;

UINT32 NS_RootInit(char * keys)
{
    return OK;
}

UINT32 NS_RootFini()
{
    return OK;
}


void close_process(int no, siginfo_t * info, void * ptr )
{
    INFO("signal %u receive. ", no );
    (void)NS_SemV(g_uiSema);
    (void)NS_SemV(g_uiSema);
}

UINT32 register_signal()
{
    UINT32 uiRet;
    struct sigaction sig;

    sigfillset(&sig.sa_mask);
    sig.sa_flags   = SA_SIGINFO;
    sig.sa_handler = (void *)close_process;

    uiRet  = (UINT32)sigaction(SIGSEGV, &sig, NULL);
    if( OK != uiRet )
    {
        LOG("failed");
        return ERROR;
    }

    uiRet = (UINT32)sigaction(SIGINT,  &sig, NULL);
    if( OK != uiRet )
    {
        LOG("failed");
        return ERROR;
    }

    uiRet = (UINT32)sigaction(SIGQUIT, &sig, NULL);
    if( OK != uiRet )
    {
        LOG("failed");
        return ERROR;
    }

    uiRet = (UINT32)sigaction(SIGUSR1, &sig, NULL);
    if( OK != uiRet )
    {
        return ERROR;
    }

    return OK;
}

int main(int argv,char ** argc)
{
    BOOL   bFlag = FALSE;
    CHAR * cfgfile = "./ns.cfg";
    UINT32 uil;
    UINT32 uiRet;

    for( uil = 1; uil < argv ; uil++ )
    {
        if( strstr(argc[uil],"-c") )
        {
            if( uil + 1 < argv )
            {
                cfgfile = argc[uil + 1];
            }
        }
        if( strstr(argc[uil],"-d") )
        {
            bFlag = TRUE;
        }
    }

    if(OK != NS_Start(cfgfile))
    {
        return ERROR;
    }

    INFO("namespace start success.");

    uiRet = NS_SemCreate(&g_uiSema);
    if( OK != uiRet )
    {
        LOG("create sema failed!");
        return NOMEM;
    }

    if( bFlag )
    {
        uiRet = register_signal();
        if( OK != uiRet )
        {
            LOG("register signal failed!");
            return NOMEM;
        }

        NS_SemP(g_uiSema);

        INFO("receive shut down signal.");

        if(OK != NS_Stop())
        {
            return ERROR;
        }

        INFO("namespace exit success.");

        (VOID)NS_SemDelete(g_uiSema);
    }
    else
    {
        while(1)
        {
            NS_TaskDelay(1000);
        }
    }

	return OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


