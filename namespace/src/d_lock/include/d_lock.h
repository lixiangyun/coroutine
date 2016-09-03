/********************************************
*
* Ëø£¬ÐÅºÅÁ¿
*
********************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#include "d_def.h"


#ifndef _NS_LOCK_H_
#define _NS_LOCK_H_


typedef VOID * LockHandle;

UINT32 NS_MutexCreate(LockHandle * pstMLock);

UINT32 NS_MutexLock(LockHandle pstMLock);

UINT32 NS_MutexUnLock(LockHandle pstMLock);

UINT32 NS_MutexDelete(LockHandle pstMLock);



#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

