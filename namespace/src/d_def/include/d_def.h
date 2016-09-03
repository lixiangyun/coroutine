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

#ifndef _D_DEF_H_
#define _D_DEF_H_

#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 基本符号定义 */
typedef int8_t          INT8;
typedef uint8_t        UINT8;
typedef int16_t        INT16;
typedef uint16_t      UINT16;
typedef int32_t        INT32;
typedef uint32_t      UINT32;
typedef int64_t        INT64;
typedef uint64_t      UINT64;

typedef unsigned long UINTPTR;

typedef size_t         SIZE_T;

typedef char           CHAR;
typedef unsigned char UCHAR;

typedef void           VOID;

typedef float         FLOAT;
typedef double       DOUBLE;

typedef UINT32        BOOL;

/* 4字节对齐 */
#define ALIGNMENT_FOUR(ulNum)     ((ulNum + 3)&(~3))

/* 8字节对齐 */
#define ALIGNMENT_EIGHT(ulNum)    ((ulNum + 7)&(~7))

#define OK     0
#define ERROR -1

#define TRUE  1
#define FALSE 0


#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

