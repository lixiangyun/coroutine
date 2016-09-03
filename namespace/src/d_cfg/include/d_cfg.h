
/********************************************
* ÅäÖÃÎÄ¼þ¶ÁÐ´Ä£¿é
*
********************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#include "d_def.h"


#ifndef __CFG_H__
#define __CFG_H__


UINT32 NS_Start(char * cfg);

#define CFG_KEY_NAME_LEN 32

#define CFG_STR_VAL_LEN 128

typedef UINT32 (* NS_START_HOOK)(VOID);

UINT32 NS_StartHookSet(NS_START_HOOK pfnHook);



UINT32 NS_CfgUintValueGet(char * keys,char * name,UINT32 uiValue);

VOID * NS_CfgPtrValueGet(char * keys,char * name,VOID * ptr);


#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


