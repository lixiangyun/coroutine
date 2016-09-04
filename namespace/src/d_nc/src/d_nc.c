/********************************************
*
*  Á´±í²Ù×÷
*
********************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <error.h>
#include <errno.h>

#include <sys/types.h>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "d_list.h"
#include "d_base.h"
#include "d_cfg.h"
#include "d_task.h"
#include "d_lock.h"
#include "d_sem.h"
#include "d_mem.h"


/*
config.append(['HTTP_PORT',8080])
config.append(['MAX_CONNECT',2048])
config.append(['LOG_FILE','./ns.log'])
*/

#define NAME_SERVER_MAX_CONNECT  1000
#define NAME_SERVER_DEFAULT_PORT 8000
#define NAME_SERVER_THREAD_NUM   2
#define NAME_SERVER_THREAD_MAX_NUM   100

UINT32 g_uiPort = NAME_SERVER_DEFAULT_PORT;
UINT32 g_uiMaxConnect;
CHAR * g_uiLogfile;
UINT32 g_uiThreadNum;

int server_sock;

typedef struct tagClientCtl {
    int client_sock;
    struct sockaddr_in client_addr;
    NS_LIST_NODE list;
}NS_CLIENT_CTL;

NS_CLIENT_CTL * g_stclient_ctl = NULL;

NS_LIST_NODE g_stClientFreeList;
NS_LIST_NODE g_stClientUserList;


LockHandle g_nsLock;
SemHanle   g_nsSema;

#define NSLOCK()   (VOID)NS_MutexLock(g_nsLock)
#define NSUNLK()   (VOID)NS_MutexUnLock(g_nsLock)

#define NSSMP()   (VOID)NS_SemP(g_nsSema)
#define NSSMPT(ms)  (VOID)NS_SemTimedP(g_nsSema,ms)
#define NSSMV()   (VOID)NS_SemV(g_nsSema)


TASK_HANDLE g_uilistentaskhandle;
TASK_HANDLE g_uiCilenttaskhandle[NAME_SERVER_THREAD_MAX_NUM];

extern VOID NS_ListenTask(UINT32 ulParam,VOID * pParam);
extern VOID NS_CilentTask(UINT32 ulParam,VOID * pParam);

extern NS_CLIENT_CTL * NS_ClientNodeAlloc();
extern NS_CLIENT_CTL * NS_ClientUserGet();

extern VOID NS_ClientNodeFree(NS_CLIENT_CTL * pfree);
extern VOID NS_ClientUserAdd(NS_CLIENT_CTL * padd);








UINT32 NS_NameServerConfigInit(char * keys)
{
    g_uiPort       = NS_CfgUintValueGet(keys, "HTTP_PORT", NAME_SERVER_DEFAULT_PORT);
    g_uiMaxConnect = NS_CfgUintValueGet(keys, "MAX_CONNECT", NAME_SERVER_MAX_CONNECT);
    g_uiLogfile    = NS_CfgStrValueGet (keys, "LOG_FILE", "ns-server.log");
    g_uiThreadNum  = NS_CfgUintValueGet(keys, "THREAD_NUM", NAME_SERVER_THREAD_NUM);

    if( g_uiThreadNum > NAME_SERVER_THREAD_MAX_NUM )
    {
        g_uiThreadNum = NAME_SERVER_THREAD_MAX_NUM;
    }

    return OK;
}

UINT32 NS_NameServerSocketInit()
{
    int ret;
    struct sockaddr_in server_addr;

    server_sock = socket(AF_INET,SOCK_STREAM,0);
    if( server_sock < 0 )
    {
        LOG("create socket failed!");
        return ERROR;
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(g_uiPort);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    ret = bind( server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr) );
    if( ret < 0 )
    {
        LOG("bind socket %d faield!", server_sock );
        return ERROR;
    }

    ret = listen( server_sock, g_uiMaxConnect);
    if( ret < 0)
    {
        LOG("listen socket %d faield!", server_sock );
        return ERROR;
    }

    return OK;
}

UINT32 NS_NameServerTaskInit()
{
    UINT32 uiRet;
    UINT32 uil;

    uiRet = NS_TaskCreate(&g_uilistentaskhandle,
                          "listen_task",
                          NS_ListenTask,
                          g_uiMaxConnect,
                          0);
    if( uiRet != OK )
    {
        LOG("create listen task failed!");
        return NOMEM;
    }

    for( uil = 0 ; uil < g_uiThreadNum ; uil++ )
    {
        char buf[128];

        snprintf(buf, sizeof(buf), "Cilent_task_%u", uil);

        uiRet = NS_TaskCreate(&g_uiCilenttaskhandle[uil],
                               buf,
                               NS_CilentTask,
                               uil,
                               0);
        if( uiRet != OK )
        {
            LOG("create cilent task %u failed!", uil );
            return NOMEM;
        }
    }

    return OK;
}


UINT32 NS_NameServerResourceInit()
{
    UINT32 size;
    UINT32 uiRet;
    UINT32 uil;

    NS_LIST_INIT(&g_stClientFreeList);
    NS_LIST_INIT(&g_stClientUserList);

    uiRet = NS_MutexCreate(&g_nsLock);
    if( OK != uiRet )
    {
        LOG("mutex lock create failed");
        return ERROR;
    }

    uiRet = NS_SemCreate(&g_nsSema);
    if( OK != uiRet )
    {
        LOG("semaphore create failed");
        return ERROR;
    }

    size = ( g_uiMaxConnect + 1) * sizeof(NS_CLIENT_CTL);

    g_stclient_ctl = (NS_CLIENT_CTL *)NS_MemAlloc(size);
    if( NULL == g_stclient_ctl )
    {
        LOG("malloc size %s failed!", size );
        return NOMEM;
    }

    memset(g_stclient_ctl, 0, size);

    for( uil = 0 ; uil < g_uiMaxConnect + 1 ; uil++ )
    {
        NS_LIST_ADD(&(g_stclient_ctl[uil].list), &g_stClientFreeList);
    }

    return OK;
}

UINT32 NS_NameServerInit(char * keys)
{
    UINT32 uiRet;

    uiRet = NS_NameServerConfigInit(keys);
    if( OK != uiRet )
    {
        return ERROR;
    }

    uiRet = NS_NameServerResourceInit(keys);
    if( OK != uiRet )
    {
        return ERROR;
    }

    uiRet = NS_NameServerSocketInit(keys);
    if( OK != uiRet )
    {
        return ERROR;
    }

    uiRet = NS_NameServerTaskInit(keys);
    if( OK != uiRet )
    {
        return ERROR;
    }

    return OK;
}

UINT32 NS_NameServerFini()
{
    (void)NS_MutexDelete(g_nsLock);
    (void)NS_SemDelete(g_nsSema);
    (void)close(server_sock);

    (void)NS_TaskDelete(g_uilistentaskhandle);

    (void)NS_MemFree(g_stclient_ctl);
    return OK;
}

VOID NS_ListenTask( UINT32 ulMaxConnect ,VOID * pParam)
{
    struct sockaddr_in client_addr;
    int client_sock;
    UINT32 clientlen;
    NS_CLIENT_CTL * ptemp;

    for(;;)
    {
        clientlen = sizeof(client_addr);

        client_sock = accept(server_sock,
                            (struct sockaddr *)&client_addr,
                            &clientlen );
        if( client_sock < 0)
        {
            LOG("accept failed!");

            continue;
        }

        for(;;)
        {
            ptemp = NS_ClientNodeAlloc();
            if( NULL != ptemp )
            {
                break;
            }
            NS_TaskDelay(10);
        }

        ptemp->client_sock = client_sock;

        (VOID)memcpy(&ptemp->client_addr, &client_addr, sizeof(client_addr));

        NS_ClientUserAdd(ptemp);

        NSSMV();
    }
}


NS_CLIENT_CTL * NS_ClientNodeAlloc()
{
    NS_LIST_NODE * ptemp;

    NSLOCK();
    if( NS_LIST_EMPTY(&g_stClientFreeList) )
    {
        NSUNLK();
        return NULL;
    }
    ptemp = g_stClientFreeList.next;
    NS_LIST_REMOVE(ptemp);
    NSUNLK();

    return NS_LIST_OFFSET(ptemp, NS_CLIENT_CTL, list);
}

VOID NS_ClientNodeFree(NS_CLIENT_CTL * pfree)
{
    NSLOCK();
    NS_LIST_ADD(&(pfree->list), &g_stClientFreeList);
    NSUNLK();
}

NS_CLIENT_CTL * NS_ClientUserGet()
{
    NS_LIST_NODE * ptemp;

    NSLOCK();
    if( NS_LIST_EMPTY(&g_stClientUserList) )
    {
        NSUNLK();
        return NULL;
    }
    ptemp = g_stClientUserList.next;
    NS_LIST_REMOVE(ptemp);
    NSUNLK();

    return NS_LIST_OFFSET(ptemp, NS_CLIENT_CTL, list);
}

VOID NS_ClientUserAdd(NS_CLIENT_CTL * padd)
{
    NSLOCK();
    NS_LIST_ADD_BEFORE(&(padd->list), &g_stClientUserList);
    NSUNLK();
}

VOID NS_CilentTask(UINT32 ulParam,VOID * pParam)
{
    INT32 ret;
    NS_CLIENT_CTL * ptemp;
    CHAR buf[20000];

    for(;;)
    {
        NSSMP();

        ptemp = NS_ClientUserGet();

        if( NULL == ptemp )
        {
            continue;
        }

        ret = read(ptemp->client_sock, buf, sizeof(buf));

        if( ret < 0)
        {
            perror("read data from client");

            close(ptemp->client_sock);
            NS_ClientNodeFree(ptemp);
            continue;
        }

        printf("%s",buf);

        write(ptemp->client_sock,buf,strlen(buf));

        close(ptemp->client_sock);

        NS_ClientNodeFree(ptemp);
    }
}




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
