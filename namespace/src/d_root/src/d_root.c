
/********************************************
* ÅäÖÃÎÄ¼þ¶ÁÐ´Ä£¿é
*
********************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "d_base.h"
#include "d_cfg.h"


int main(int argv,char ** argc)
{
    BOOL bDaemon = TRUE;
    CHAR * cfgfile = "./ns.cfg";
    UINT32 uil;

    for( uil = 1; uil < argv ; uil++ )
    {
        if( strstr(argc[uil],"-c") )
        {
            if( uil + 1 < argv )
            {
                cfgfile = argc[uil + 1];
                break;
            }
        }
    }

    if(OK != NS_Start(cfgfile))
    {
        return ERROR;
    }

    INFO("namespace start success. \r\n");

    while(bDaemon)
    {
        sleep(1000);
    }

	return OK;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


