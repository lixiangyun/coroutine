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


/*
config.append(['HTTP_PORT',8080])
config.append(['MAX_CONNECT',2048])
config.append(['LOG_FILE','./ns.log'])
*/

#define NAME_SERVER_MAX_CONNECT  1000
#define NAME_SERVER_DEFAULT_PORT 8000
#define NAME_SERVER_THREAD_NUM   2

UINT32 g_uiPort = NAME_SERVER_DEFAULT_PORT;
UINT32 g_uiMaxConnect;
CHAR * g_uiLogfile;
UINT32 g_uiThreadNum;

int server_sock;

typedef struct tagClientCtl {
    int client_sock;
    struct sockaddr_in client_addr;
    TASK_HANDLE taskhandle;
}NS_CLIENT_CTL;

NS_CLIENT_CTL * g_stclient_ctl;


TASK_HANDLE g_uilistentaskhandle;

extern VOID NS_ListenTask(UINT32 ulParam,VOID * pParam);
extern VOID NS_CilentTask(UINT32 ulParam,VOID * pParam);


UINT32 NS_NameServerInit(char * keys)
{
    UINT32 size;
    UINT32 uiRet;
    int ret;
    struct sockaddr_in server_addr;

    g_uiPort       = NS_CfgUintValueGet(keys, "HTTP_PORT", NAME_SERVER_DEFAULT_PORT);
    g_uiMaxConnect = NS_CfgUintValueGet(keys, "MAX_CONNECT", NAME_SERVER_MAX_CONNECT);
    g_uiLogfile    = NS_CfgStrValueGet (keys, "LOG_FILE", "ns-server.log");
    g_uiThreadNum  = NS_CfgUintValueGet(keys, "THREAD_NUM", NAME_SERVER_THREAD_NUM);

    size = g_uiMaxConnect * sizeof(NS_CLIENT_CTL);
    g_stclient_ctl = (NS_CLIENT_CTL *)NS_MemAlloc(size);
    if( NULL == g_stclient_ctl )
    {
        LOG("malloc size %s failed!", size );
        return NOMEM;
    }

    memset(g_stclient_ctl, 0, size);

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

    uiRet = NS_TaskCreate(&g_uilistentaskhandle,
                          "listen_task",
                          NS_ListenTask,
                          g_uiMaxConnect,
                          0);
    if( uiRet != OK )
    {
        LOG("create task failed!");
        return ERROR;
    }

    return OK;
}

UINT32 NS_NameServerFini()
{
    close(server_sock);
    NS_TaskDelete(g_uilistentaskhandle);
    return OK;
}

VOID NS_ListenTask( UINT32 ulMaxConnect ,VOID * pParam)
{
    struct sockaddr_in client_addr;
    struct sockaddr_in sin;
    int client_sock;
    char buf[20000];

    int ret;
    unsigned int clientlen = sizeof(client_addr);

    for(;;)
    {
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &clientlen );
        if( client_sock < 0)
        {
            LOG("accept failed!");
            continue;
        }

        ret = read(client_sock, buf, 20000);

        if( ret < 0)
        {
            perror("read data from client");
            exit(1);
        }

        printf("%s",buf);

        write(client_sock,buf,strlen(buf));

        close(client_sock);
    }
}



VOID NS_CilentTask(UINT32 ulParam,VOID * pParam)
{
    for(;;)
    {



    }
}




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
