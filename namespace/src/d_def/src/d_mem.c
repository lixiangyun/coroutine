/********************************************
*
*  伙伴系统，内存管理模块
*
*  包括内存申请，释放，调测
*
********************************************/

#include "d_mem.h"
#include "d_base.h"

#include <stdlib.h>


VOID * NS_MemAlloc(UINT32 ulAllocMemSize)//内存分配
{
	return malloc(ulAllocMemSize);
}

UINT32 NS_MemFree(VOID * pFreeOne)//内存释放
{
    (VOID)free(pFreeOne);
	return OK;
}

