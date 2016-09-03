
/********************************************
* 配置文件读写模块
*
********************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#include <stdio.h>
#include <dlfcn.h>

#include "d_base.h"
#include "d_cfg.h"
#include "d_list.h"

typedef UINT32 (* NS_INIT_HOOK)(char * keys);

typedef UINT32 (* NS_FINI_HOOK)();

struct tagstartlist {
    NS_INIT_HOOK pinit;
    NS_FINI_HOOK pfini;
    char * name;
};

extern struct tagstartlist g_aStartList[];

char * cfglist  = NULL;

struct tagstartlisthook {
    NS_LIST_NODE  stList;
    NS_START_HOOK pfnHook;
};

NS_LIST_NODE stStartHookList;

UINT32 NS_CfgInit(char * keys)
{
    return OK;
}

UINT32 NS_CfgFini()
{
    return OK;
}

char * nsloadcfg(char * cfg)
{
    char   tmpbuf[4096];
    char * nbuf;
    UINT32 uiTemLen;
    UINT32 uiSize;
    FILE * fp;

    fp = fopen(cfg,"r");
    if(NULL == fp)
    {
        LOG("open %s file failed!",cfg);
        return NULL;
    }

    fseek(fp,1,SEEK_END);
    uiSize = ftell(fp) + 128;
    rewind(fp);

    nbuf = malloc(uiSize);
    if(NULL == nbuf)
    {
        fclose(fp);
        LOG("malloc size %s failed!",uiSize);
        return NULL;
    }

    uiTemLen = 0;
    while( NULL != fgets(tmpbuf,4096,fp) )
    {
        UINT32 slen = strlen(tmpbuf);
        if( uiSize <= slen )
        {
            break;
        }
        strncpy(nbuf + uiTemLen , tmpbuf , uiSize );
        uiSize   -= slen;
        uiTemLen += slen;
    }

    fclose(fp);

    nbuf[uiTemLen] = '\0';
    return nbuf;
}

UINT32 nsstartinit()
{
    UINT32 uiRet;
    struct tagstartlist * plist;

    for(plist = g_aStartList;plist->pinit != NULL;plist++)
    {
        uiRet = plist->pinit(plist->name);
        if(uiRet)
        {
            LOG("start module %s failed!(ret=%u)",plist->name,uiRet);
            return uiRet;
        }

        INFO("init module %s success!" , plist->name );
    }

    return OK;
}


UINT32 NS_Start(char * cfg)
{
    char * pcfg;

    if( NULL == cfg )
    {
        LOG("Invald Param!\n");
        return ERROR;
    }

    NS_LIST_INIT(&stStartHookList);

    pcfg = nsloadcfg(cfg);
    if(NULL == pcfg)
    {
        return ERROR;
    }

    INFO("load %s file success!", cfg);

    cfglist = pcfg;

    if( OK != nsstartinit() )
    {
        free(pcfg);
        return ERROR;
    }

    return OK;
}

/* d_mem.NS_MEM_SIZE=1048576 */
UINT32 NS_CfgUintValueGet(char * keys,char * name,UINT32 uiValue)
{
    char   nbuf[CFG_STR_VAL_LEN + CFG_KEY_NAME_LEN + 2];
    char * ptemp;
    UINT32 uiCfgValue;
    UINT32 uiLen;

    if( cfglist )
    {
        snprintf(nbuf,CFG_STR_VAL_LEN + CFG_KEY_NAME_LEN + 2,"%s.%s",keys,name);
        uiLen = strlen(nbuf);
        ptemp = strstr(cfglist,nbuf);
        if( ptemp )
        {
            /* 1表示等号 */
            ptemp += uiLen + 1;
            uiCfgValue = atoi(ptemp);
            return uiCfgValue;
        }
    }

    LOG("Can not find cfg %s!\n",nbuf);

    return uiValue;
}

VOID * NS_CfgPtrValueGet(char * keys,char * name,VOID * ptr)
{
    char   nbuf[CFG_STR_VAL_LEN + CFG_KEY_NAME_LEN + 2];
    char * ptemp;
    void * pcfgPtr;
    UINT32 uiLen;

    if( cfglist )
    {
        snprintf(nbuf,CFG_STR_VAL_LEN + CFG_KEY_NAME_LEN + 2,"%s.%s",keys,name);
        uiLen = strlen(nbuf);
        ptemp = strstr(cfglist,nbuf);
        if( ptemp )
        {
            ptemp += uiLen;
            pcfgPtr = dlsym(NULL,ptemp);
            if( pcfgPtr )
            {
                return pcfgPtr;
            }
        }
    }

    LOG("Can not find cfg %s!\n",nbuf);

    return ptr;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


