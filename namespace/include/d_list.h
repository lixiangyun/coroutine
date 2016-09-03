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


#ifndef _NS_LIST_H_
#define _NS_LIST_H_


#include "d_def.h"

typedef struct stlistnode
{
	struct stlistnode * next;
	struct stlistnode * prev;
}NS_LIST_NODE;

#define NS_LIST_EMPTY(head)\
    ((head)->next == (head))

#define NS_LIST_NEXT(item)\
    ((item)->next)

#define NS_LIST_BEFORE(item)\
    ((item)->prev)

#define NS_LIST_INIT(head) \
	(head)->next = (head); \
	(head)->prev = (head);

#define NS_LIST_ADD(item,where) \
	(item)->next = (where)->next; \
	(item)->prev = (where); \
	(where)->next->prev = (item); \
	(where)->next = (item);

#define NS_LIST_ADD_BEFORE(item,where) \
	(item)->prev = (where)->prev; \
	(item)->next = (where); \
	(where)->prev->next = (item); \
	(where)->prev = (item);

#define NS_LIST_REMOVE(item) \
	(item)->next->prev = (item)->prev; \
	(item)->prev->next = (item)->next;

#define NS_LIST_OFFSET(item,struct,stNode) \
	(struct *)((UINTPTR)item - (UINTPTR)(&((struct *)0)->stNode))

#define NS_LIST_TRAVERSAL(pstItem,pstHead) \
    for( (pstItem) = (pstHead)->next; (pstItem) != (pstHead) ; (pstItem) = (pstItem)->next )


#define NS_LIST_TRAVERSAL_REMOVE(pstItem,pstTemp,pstHead) \
        for( (pstItem) = (pstHead)->next,(pstTemp) = (pstItem)->next;\
             (pstItem) != (pstHead) ;\
             (pstItem) = (pstTemp), (pstTemp) = (pstTemp)->next)


#endif /* _NS_LIST_H_ */


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
