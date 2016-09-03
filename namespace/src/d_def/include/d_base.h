/********************************************
*
*  基本符号定义，函数定义
*
********************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */



#ifndef _D_BASE_H_
#define _D_BASE_H_
#endif

#include "d_def.h"
#include "d_errno.h"

extern void nslogwrite(const char * func,
              const char * file,
              UINT32 uiLine,
              const char * pformat,
              ...);

extern void nsloginfo(const char * func,
              const char * file,
              UINT32 uiLine,
              const char * pformat,
              ...);

#define LOG(pformat,x...)\
    do {\
        nslogwrite(__FUNCTION__,__FILE__,__LINE__,pformat,##x);\
    }while(0)

#define INFO(pformat,x...)\
    do {\
        nsloginfo(__FUNCTION__,__FILE__,__LINE__,pformat,##x);\
    }while(0)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

