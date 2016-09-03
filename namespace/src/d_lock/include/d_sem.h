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


#ifndef _NS_SEM_H_
#define _NS_SEM_H_


typedef VOID * SemHanle;

UINT32 NS_SemP(SemHanle pSemv);

UINT32 NS_SemTryP(SemHanle pSemv);

UINT32 NS_SemTimedP(SemHanle pSemv , UINT32 ulMsec );

UINT32 NS_SemV(SemHanle pSemv);

UINT32 NS_SemDelete(SemHanle pSemv);

UINT32 NS_SemCreate(SemHanle * pSemv);


#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

