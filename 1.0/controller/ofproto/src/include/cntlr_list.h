/**************************************************************************
 *   Copyright 2011-2012, Freescale Semiconductor, Inc. All rights reserved.
 **************************************************************************/
/*
 *  File:        cntlr_list.h
 * 
 * Description: linked list operation macros, this takes care of locks and 
 *              RCU functionality. 
 *
 * Authors:     Rajesh Madabushi <rajesh.madabushi@freescale.com>
 * Modifier:    
 */
/*
 *  History
 *  10 October 2012 - Rajesh Madabushi - Initial Implementation.
 * 
 **************************************************************************/
#ifndef _CNTLR_LIST_H_
#define _CNTLR_LIST_H_

/* TYPEDEF_START  *********************************************************/
typedef struct of_list_node_s
{
  struct of_list_node_s *pNext;
} of_list_node_t;

typedef void (*frec_rcu_func_t)(struct rcu_head *node);

typedef struct of_list_s
{
  of_list_node_t  *head;
  of_list_node_t  *tail;
  uint32_t        count;
  cntlr_lock_t    lock;
  frec_rcu_func_t free_func_cbk;
} of_list_t;

#define OFLIST_NODE_OFFSET offsetof(of_list_node_t, pNext);
/* TYPEDEF_END    *********************************************************/

/*DEFINE_START ************************************************************/

#define OF_LIST_INIT(list,free_func_cbk_p)  \
{\
   list.head           = NULL;\
   list.tail           = NULL;\
   list.count          = 0;\
   list.free_func_cbk  = free_func_cbk_p;\
   CNTLR_LOCK_INIT(list.lock);\
}


#if 0
#define OF_APPEND_NODE_TO_LIST(list,node)\
{\
    CNTLR_LOCK_TAKE(list.lock);\
    if(list.head == NULL)\
    {\
       rcu_assign_pointer((list.head), (of_list_node_t*)node);\
    }\
    rcu_assign_pointer(list.tail,(of_list_node_t*)node);\
    list.count++;\
    CNTLR_LOCK_RELEASE(list.lock);\
}
#define OF_APPEND_NODE_TO_LIST_WITH_OFFSET(list,node,offset)\
{\
   of_list_node *node_p=(of_list_node_t*)(node+offset);
   OF_APPEND_NODE_TO_LIST(list,node_p)\
}
#else
#define OF_APPEND_NODE_TO_LIST(list,node,offset)\
{\
    of_list_node_t *node_p=(of_list_node_t *)((unsigned char*)node+offset);\
/*    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "while appenind..offset = %d",offset);*/\
    CNTLR_LOCK_TAKE(list.lock);\
    if(list.head == NULL)\
    {\
       rcu_assign_pointer((list.head), node_p);\
    }\
    else\
    {\
       rcu_assign_pointer(list.tail->pNext,node_p); \
    }\
    rcu_assign_pointer(list.tail,node_p);\
    list.count++;\
/*   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "list count =%d",list.count);*/\
    CNTLR_LOCK_RELEASE(list.lock);\
}
#endif

#define OF_LIST_INSERT_NODE(list,prev_node,node,next_node,offset)\
{\
   of_list_node_t *node_p=(of_list_node_t*)((unsigned char *)node+offset);\
   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "offset = %d",offset);\
   CNTLR_LOCK_TAKE(list.lock);\
   if(prev_node == NULL)\
   {\
      rcu_assign_pointer(list.head,node_p);\
   }\
   else\
   {\
       rcu_assign_pointer(((of_list_node_t*)((unsigned char *)prev_node+offset))->pNext,\
                            node_p); \
   }\
   if(next_node == NULL)\
   { \
    rcu_assign_pointer(list.tail,node_p);\
   } \
   else\
   {\
      rcu_assign_pointer(node_p->pNext,\
                              (of_list_node_t*)((unsigned char *)next_node+offset));\
   }\
   CNTLR_LOCK_RELEASE(list.lock);\
}

#define OF_REMOVE_NODE_FROM_LIST(list,node,prev_node,offset)                      \
{                                                                           \
    of_list_node_t *node_p=(of_list_node_t*)((unsigned char *)node+offset);\
/*   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "offset = %d",offset);*/\
    CNTLR_LOCK_TAKE(list.lock);                                               \
    if(OF_LIST_COUNT(list) == 1)                                            \
    {                                                                       \
       rcu_assign_pointer(list.head,NULL);                        \
       rcu_assign_pointer(list.tail,NULL);                             \
    }                                                                       \
    else                                                                    \
    {                                                                       \
       if( node_p == list.head)                       \
       {                                                                    \
          rcu_assign_pointer(list.head,list.head->pNext); \
       }                                                                    \
       else                                                                 \
       {                                                                    \
          rcu_assign_pointer( ((of_list_node_t*)((unsigned char *)prev_node+offset))->pNext,        \
                                 node_p->pNext);           \
       }                                                                    \
       if(node_p == list.tail)                            \
       {                                                                    \
	  rcu_assign_pointer(list.tail,(of_list_node_t*)((unsigned char *)prev_node+offset));   \
       }                                                                    \
    }                                                                       \
    list.count--;                                                         \
    if(list.free_func_cbk)                                                    \
      CNTLR_CALL_RCU((struct rcu_head *)node_p, list.free_func_cbk);             \
    CNTLR_LOCK_RELEASE(list.lock);                                            \
}

/*List addition, remove and scan operation shoul happen in the same thread
  i.e. Following Macro shouble called from the same thread in which the list
  was formed */
#if 1
#define OF_LIST_REMOVE_HEAD_AND_SCAN(list, node, typecast, offset) \
  of_list_node_t *node_p=NULL;\
for ( \
		node_p = (OF_LIST_FIRST ((list)));\
		((node_p != NULL) && (node=(typecast)((unsigned char *)node_p - offset)));\
		CNTLR_LOCK_TAKE(list.lock),\
		rcu_assign_pointer(list.head,list.head->pNext),\
		((node_p == list.tail) ? rcu_assign_pointer(list.tail,NULL): NULL),   \
		list.count--,\
		((list.free_func_cbk) ? call_rcu((struct rcu_head *)node_p, list.free_func_cbk): of_mem_free(node)), \
		CNTLR_LOCK_RELEASE(list.lock),\
		node_p =  OF_LIST_FIRST (list))
#else
#define OF_LIST_REMOVE_HEAD_AND_SCAN(list, node, typecast, offset) \
	of_list_node_t *node_p=NULL;\
for ( \
		node_p = (OF_LIST_FIRST ((list)));\
		((node_p != NULL) && (node=(typecast)(node_p - offset)));\
		OF_REMOVE_NODE_FROM_LIST(list,node,NULL,offset),\
		of_mem_free(node), \
		node_p = (OF_LIST_FIRST ((list))))
#endif

#define OF_LIST_COUNT(list) (list.count)

/*Caller Must use RCU_READ_LOCK and RCU_READ_UNLOCK*/
#define OF_LIST_FIRST(list) rcu_dereference(list.head)

/*Caller Must use RCU_READ_LOCK and RCU_READ_UNLOCK*/
#define OF_LIST_LAST(list) rcu_dereference(list.tail)

/*Caller Must use RCU_READ_LOCK and RCU_READ_UNLOCK*/
#define OF_LIST_NEXT(list,node) rcu_dereference(((of_list_node_t *)node)->pNext)
#define OF_LIST_NEXT_WITH_OFFSET(list,node,offset) rcu_dereference(((of_list_node_t *)((unsigned char *)node+offset)->pNext))

/*Caller Must use RCU_READ_LOCK and RCU_READ_UNLOCK*/
#define OF_LIST_SCAN(list, node, typecast,offset) \
/*   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "in scanning...offset = %d",offset);*/\
  for (node =(typecast)(OF_LIST_FIRST ((list))); \
             ((node != NULL) && (node=(typecast)((unsigned char*)node - offset)));\
             node =  (typecast)OF_LIST_NEXT (list, (of_list_node_t *)((unsigned char*)node+offset)))


#define OF_LIST_MEMBER(node, typecast, member) \
      ((typecast)((unsigned char *)(node)-(uint32_t)(&((typecast)0)->member)))

#define OF_LIST_DYN_SCAN(list, node, tmp, typecast, offset) \
  for (node = (typecast) (OF_LIST_FIRST((list))),\
      tmp = ((node == NULL) ? NULL :\
	(typecast) (OF_LIST_NEXT((list), (of_list_node_t *) (unsigned char*)node+offset)));\
      ((node != NULL) && (node=(typecast)((unsigned char*)node - offset)));\
      node = tmp,\
      tmp = ((node == NULL) ? NULL :\
	(typecast) (OF_LIST_NEXT((list), (of_list_node_t *) ((unsigned char*)node)))))

#endif /* _CNTLR_LIST_H_ */
