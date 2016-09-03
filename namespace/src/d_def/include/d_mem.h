/********************************************
*
*
*
* linimbus
*
********************************************/

#include <stdlib.h>
#include <stdio.h>
#include "d_def.h"


#ifndef  __D_MEM_H__
#define  __D_MEM_H__


extern VOID * NS_MemAlloc(UINT32 ulAllocMemSize);

extern UINT32 NS_MemFree(VOID * pFreeOne);



#endif  /*  __D_MEM_H__ */


