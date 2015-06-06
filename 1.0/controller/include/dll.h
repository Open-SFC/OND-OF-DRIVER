/* @(#) DLL 1.0  18/10/04  */
/**************************************************************************
 * Copyright 2011-2012, Freescale Semiconductor, Inc. All rights reserved.
 ***************************************************************************/
/*
 * File:	dll.h
 *
 * Description:
 *  This file contains the APIs prototypes and data structure definitions
 *  which are used by applications for their doubly linked list operations
 *
 * Authors:	Madabhushi Rajesh Kumar <B38870@freescale.com>
 * Modifier:    Rajendran A S <rajendran.as@freescale.com>
 *
 */
/*
 * History
 * 21 Sep 2011 - Rajendran A S  - Took the header file from POC and modified
 *                                the data types.
 *
 */
/******************************************************************************/


#ifndef _DLL_H
#define _DLL_H


/***************************************************************************/
/*                  DATA STRUCTURE DEFINITIONS                             */
/***************************************************************************/
struct dll_queue_node
{
  struct dll_queue_node *pPrev;
  struct dll_queue_node *pNext;
};

struct dll_queue
{
  struct dll_queue_node      Head;
  uint32_t                   ulCount;
} ;

/***************************************************************************/
/*                      FUNCTION PROTOTYPE DECLERATIONS                    */
/***************************************************************************/
void  dll_queue_init_list (struct dll_queue *pDll);
void  dll_queue_delete_in_middle_rcu (struct dll_queue * pDll, struct dll_queue_node * pPrev,
                             struct dll_queue_node * pNode, struct dll_queue_node * pNext);
void  dll_queue_delete_in_middle (struct dll_queue * pDll, struct dll_queue_node * pPrev,
                             struct dll_queue_node * pNode, struct dll_queue_node * pNext);
void  dll_queue_insert_node_in_middle_rcu (struct dll_queue *pDll, struct dll_queue_node *pPrev,
                               struct dll_queue_node *pNode, struct dll_queue_node *pNext);
void  dll_queue_insert_in_middle (struct dll_queue *pDll, struct dll_queue_node *pPrev,
                               struct dll_queue_node *pNode, struct dll_queue_node *pNext);

int32_t dll_queue_insert_node_at_list_position_rcu (struct dll_queue *pDll, struct dll_queue_node *pNode, uint32_t uiListPos_p);
int32_t dll_queue_insert_node_at_list_position(struct dll_queue *pDll, struct dll_queue_node *pNode, uint32_t uiListPos_p);

void dll_queue_prepend_list(struct dll_queue* pNewListHead, struct dll_queue* pOldListHead);

/***************************************************************************/
/*                      MACRO DEFINITIONS                                  */
/***************************************************************************/

#define DLL_QUEUE_INIT_NODE(pNode) ((pNode)->pNext = (pNode)->pPrev = NULL)
#define DLL_QUEUE_INIT_LIST(pDll)    dll_queue_init_list ((pDll))
#define PSP_DLLQ_COUNT(pDll) ((pDll)->ulCount)
#ifdef CONFIG_PSP_DSLIB_DBG
#define DLL_QUEUE_FIRST(pDll) \
        ((PSP_DLLQ_COUNT((pDll)) == 0) ? NULL: (pDll)->Head.pNext)
#define DLL_QUEUE_LAST(pDll) \
        ((PSP_DLLQ_COUNT((pDll)) == 0) ? NULL : (pDll)->Head.pPrev)
#define DLL_QUEUE_NEXT(pDll,pNode) \
        (((pNode) == NULL) ? DLL_QUEUE_FIRST(pDll) : \
        (((pNode)->pNext == &(pDll)->Head) ? NULL : (pNode)->pNext))
#define DLL_QUEUE_PREV(pDll,pNode) \
        (((pNode) == NULL) ? DLL_QUEUE_LAST (pDll) : \
        (((pNode)->pPrev == &(pDll)->Head) ? NULL : (pNode)->pPrev))
#define DLL_QUEUE_IS_NODE_IN_LIST(pNode) \
                (((pNode)->pNext != NULL) && \
                ((pNode)->pPrev != NULL) && \
                ((pNode)->pNext->pPrev == pNode) && \
                ((pNode)->pPrev->pNext == pNode))
#define DLL_QUEUE_APPEND_NODE(pDll, pNode) \
            dll_queue_insert_in_middle ((pDll),(pDll)->Head.pPrev,(pNode),\
                                    &(pDll)->Head)
#define DLL_QUEUE_PREPEND_NODE(pDll, pNode) \
            dll_queue_insert_in_middle ((pDll),&(pDll)->Head, (pNode), \
                                   (pDll)->Head.pNext)
#define DLL_QUEUE_INSERT_NODE(pDll, pPrev, pNode) \
            dll_queue_insert_in_middle ((pDll), (pPrev), (pNode), ((pPrev)->pNext))
#define DLL_QUEUE_INSERT_BEFORE_NODE(pDll, pNext, pNode) \
            dll_queue_insert_in_middle ((pDll), ((pNext)->pPrev), (pNode), (pNext))
#define DLL_QUEUE_INSERT_NODE_AT_LISTPOS(pDll,pNode,ListPos) \
            dll_queue_insert_node_at_list_position((pDll),(pNode),(ListPos))
#else
#define DLL_QUEUE_FIRST(pDll) ((pDll)->Head.pNext)
#define DLL_QUEUE_LAST(pDll)  ((pDll)->Head.pPrev)
#define DLL_QUEUE_NEXT(pDll,pNode) ((pNode)->pNext)
#define DLL_QUEUE_PREV(pDll,pNode) ((pNode)->pPrev)
#define DLL_QUEUE_IS_NODE_IN_LIST(pNode) 1
#ifdef CONFIG_PSP_DSLIB_RCU_SUPPORT
#define DLL_QUEUE_FIRST_RCU(pDll) \
         RCU_DE_REF(DLL_QUEUE_FIRST(pDll))
#define DLL_QUEUE_LAST_RCU(pDll) \
         RCU_DE_REF(DLL_QUEUE_LAST(pDll))
#define DLL_QUEUE_NEXT_RCU(pDll, pNode) \
         RCU_DE_REF(DLL_QUEUE_NEXT(pDll, pNode))
#define DLL_QUEUE_SWAP_NODE_RCU(pDll, pOldNode, pNewNode)  \
         dll_queue_insert_node_in_middle_rcu((pDll), (pOldNode)->pPrev, (pNewNode), (pOldNode)->pNext)
#define DLL_QUEUE_INSERT_BEFORE_NODE_RCU(pDll, pRelNode, pNode) \
         dll_queue_insert_node_in_middle_rcu ((pDll), ((pRelNode)->pPrev), (pNode), (pRelNode))

#define DLL_QUEUE_APPEND_NODE_RCU(pDll, pNode) \
            dll_queue_insert_node_in_middle_rcu ((pDll),(pDll)->Head.pPrev,(pNode), NULL)
#endif
#define DLL_QUEUE_APPEND_NODE(pDll, pNode) \
            dll_queue_insert_in_middle ((pDll),(pDll)->Head.pPrev,(pNode), NULL)
#ifdef CONFIG_PSP_DSLIB_RCU_SUPPORT
#define DLL_QUEUE_PREPEND_NODE_RCU(pDll, pNode) \
            dll_queue_insert_node_in_middle_rcu ((pDll),NULL, (pNode), (pDll)->Head.pNext)
#endif
#define DLL_QUEUE_PREPEND_NODE(pDll, pNode) \
            dll_queue_insert_in_middle ((pDll),NULL, (pNode), (pDll)->Head.pNext)
#define DLL_QUEUE_INSERT_NODE(pDll, pPrev, pNode) \
            ((pPrev != NULL) ?\
              (DLL_QUEUE_INSERT_NODE_IN_MIDDLE((pDll), (pPrev), (pNode), \
                                      ((pPrev)->pNext))) :\
              (DLL_QUEUE_INSERT_NODE_IN_MIDDLE ((pDll), NULL, (pNode),\
                                               (pDll)->Head.pNext)))
#define DLL_QUEUE_INSERT_BEFORE_NODE(pDll, pNext1, pNode)\
             ((pNext1 != NULL) ?\
              (DLL_QUEUE_INSERT_NODE_IN_MIDDLE((pDll), (pNext1)->pPrev, \
                                      (pNode), (pNext1))) : \
              (DLL_QUEUE_INSERT_NODE_IN_MIDDLE((pDll), \
                                      (pDll)->Head.pNext, (pNode), (pNext1))))
#define DLL_QUEUE_INSERT_NODE_AT_LISTPOS(pDll,pNode,ListPos) \
            dll_queue_insert_node_at_list_position((pDll),(pNode),(ListPos))

#endif
#ifdef CONFIG_PSP_DSLIB_RCU_SUPPORT
#define DLL_QUEUE_DELETE_NODE_RCU(pDll, pNode) \
        (DLL_QUEUE_IS_NODE_IN_LIST ((pNode)) != 0) ? \
        (dll_queue_delete_in_middle_rcu ((pDll),(pNode)->pPrev, \
                                 (pNode),(pNode)->pNext), 1) : 0
#endif
#define DLL_QUEUE_DELETE_NODE(pDll, pNode) \
        (DLL_QUEUE_IS_NODE_IN_LIST ((pNode)) != 0) ? \
        (dll_queue_delete_in_middle ((pDll),(pNode)->pPrev, \
                                 (pNode),(pNode)->pNext), 1) : 0
#define DLL_QUEUE_INSERT_NODE_IN_MIDDLE(pDll, pPrev, pNode, pNext) \
            dll_queue_insert_in_middle ((pDll), (pPrev), (pNode), (pNext))

#ifdef CONFIG_PSP_DSLIB_RCU_SUPPORT
#define DLL_QUEUE_SCAN_RCU(pDll, pNode, typecast) \
        for(pNode = (typecast)(RCU_DE_REF(DLL_QUEUE_FIRST ((pDll)))); \
            pNode != NULL; \
            pNode = (typecast)(RCU_DE_REF(DLL_QUEUE_NEXT ((pDll),((struct dll_queue_node *)(pNode))))))

#define DLL_QUEUE_DYN_SCAN_RCU(pDll, pNode, pTmp, typecast) \
        for (pNode = (typecast) (RCU_DE_REF(DLL_QUEUE_FIRST((pDll)))),\
             pTmp = ((pNode == NULL) ? NULL :\
               (typecast) (RCU_DE_REF(DLL_QUEUE_NEXT((pDll), (struct dll_queue_node *) (pNode)))));\
             pNode != NULL;\
             pNode = pTmp,\
             pTmp = ((pNode == NULL) ? NULL :\
               (typecast) (RCU_DE_REF(DLL_QUEUE_NEXT((pDll), (struct dll_queue_node *) (pNode))))))

/* TODO --LSR */
#define DLL_QUEUE_DYN_SCAN_FROM_NODE_RCU(pDll, pNode, pTmp, typecast) \
        for (pNode = (typecast) (pNode),\
             pTmp = ((pNode == NULL) ? NULL :\
               (typecast) (RCU_DE_REF(DLL_QUEUE_NEXT((pDll), (struct dll_queue_node *) (pNode)))));\
             pNode != NULL;\
             pNode = pTmp,\
             pTmp = ((pNode == NULL) ? NULL :\
               (typecast) (RCU_DE_REF(DLL_QUEUE_NEXT((pDll), (struct dll_queue_node *) (pNode))))))
#endif
#define DLL_QUEUE_SCAN(pDll, pNode, typecast) \
        for(pNode = (typecast)(DLL_QUEUE_FIRST ((pDll))); \
            pNode != NULL; \
            pNode = (typecast)(DLL_QUEUE_NEXT ((pDll),((struct dll_queue_node *)(pNode)))))
#define DLL_QUEUE_DYN_SCAN(pDll, pNode, pTmp, typecast) \
        for (pNode = (typecast) (DLL_QUEUE_FIRST((pDll))),\
             pTmp = ((pNode == NULL) ? NULL :\
               (typecast) (DLL_QUEUE_NEXT((pDll), (struct dll_queue_node *) (pNode))));\
             pNode != NULL;\
             pNode = pTmp,\
             pTmp = ((pNode == NULL) ? NULL :\
               (typecast) (DLL_QUEUE_NEXT((pDll), (struct dll_queue_node *) (pNode)))))

#define DLL_QUEUE_DYN_SCAN_FROM_NODE(pDll, pNode, pTmp, typecast) \
        for (pNode = (typecast) (pNode),\
             pTmp = ((pNode == NULL) ? NULL :\
               (typecast) (DLL_QUEUE_NEXT((pDll), (struct dll_queue_node *) (pNode))));\
             pNode != NULL;\
             pNode = pTmp,\
             pTmp = ((pNode == NULL) ? NULL :\
               (typecast) (DLL_QUEUE_NEXT((pDll), (struct dll_queue_node *) (pNode)))))

#define DLL_QUEUE_LIST_MEMBER(pNode, typecast, member) \
      ((typecast)((uchar8_t *)(pNode)-(uint32_t)(&((typecast)0)->member)))

/* macros that use read-write lock with write counter follows ... */

/*
 * Macro : DLL_QUEUE_APPEND_NODE_WITH_LOCK_WRITE_CNTR
 *  Arguments: pDll - list
 *    pNode - node to be added
 *    typecast - data structure type
 *    pNextPtrName - member name of the next pointer
 *    Lock - lock variable
 *    writeCntr - atomic variable used to avoid write starvation
 */
#define DLL_QUEUE_APPEND_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast,\
            pNextPtrName, Lock, writeCntr) \
        {\
          ATOMIC_INC (writeCntr);\
          WRITE_LOCK_TAKE (Lock);\
          ATOMIC_DEC (writeCntr);\
          DLL_QUEUE_APPEND_NODE((pDll), (pNode)); \
          WRITE_LOCK_RELEASE (Lock);\
        }
#define DLL_QUEUE_PREPEND_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast,\
            pNextPtrName, Lock, writeCntr) \
        {\
          ATOMIC_INC (writeCntr);\
          WRITE_LOCK_TAKE (Lock);\
          ATOMIC_DEC (writeCntr);\
          DLL_QUEUE_PREPEND_NODE((pDll), (pNode)); \
          WRITE_LOCK_RELEASE (Lock);\
        }
/*
 * Macro : DLL_QUEUE_DELETE_NODE_WITH_LOCK_WRITE_CNTR
 *  Arguments: pDll - list
 *    pNode - node to be deleted
 *    typecast - data structure type
 *    pNextPtrName - member name of the next pointer
 *    Lock - lock variable
 *    writeCntr - atomic variable used to avoid write starvation
 */
#define DLL_QUEUE_DELETE_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast,\
            pNextPtrName, Lock, writeCntr) \
        {\
          ATOMIC_INC (writeCntr);\
          WRITE_LOCK_TAKE (Lock);\
          ATOMIC_DEC (writeCntr);\
          DLL_QUEUE_DELETE_NODE ((pDll), (pNode));\
          WRITE_LOCK_RELEASE (Lock);\
        }
/*
 * Macro : DLL_QUEUE_INSERT_NODE_WITH_LOCK_WRITE_CNTR
 *  Arguments: pDll - list
 *    pPrev - previous pointer to new node
 *    pNode - node to be added
 *    typecast - data structure type
 *    pNextPtrName - member name of the next pointer
 *    Lock - lock variable
 *    writeCntr - atomic variable used to avoid write starvation
 */
#define DLL_QUEUE_INSERT_NODE_WITH_LOCK_WRITE_CNTR(pDll, pPrev, pNode,\
            typecast, pNextPtrName, Lock, writeCntr) \
        {\
          ATOMIC_INC (writeCntr);\
          WRITE_LOCK_TAKE (Lock);\
          ATOMIC_DEC (writeCntr);\
          DLL_QUEUE_INSERT_NODE((pDll), (pPrev), (pNode));\
          WRITE_LOCK_RELEASE (Lock);\
        }
/*Prashant*/
/*
 * Macro : DLL_QUEUE_INSERT_BEFORE_NODE_WITH_LOCK_WRITE_CNTR
 *  Arguments: pDll - list
 *    pPrev - previous pointer to new node
 *    pNode - node to be added
 *    typecast - data structure type
 *    pNextPtrName - member name of the next pointer
 *    Lock - lock variable
 *    writeCntr - atomic variable used to avoid write starvation
 */
#define DLL_QUEUE_INSERT_BEFORE_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNext, pNode,\
            typecast, pNextPtrName, Lock, writeCntr) \
        {\
          ATOMIC_INC (writeCntr);\
          WRITE_LOCK_TAKE (Lock);\
          ATOMIC_DEC (writeCntr);\
          DLL_QUEUE_INSERT_BEFORE_NODE((pDll), (pNext), (pNode));\
          WRITE_LOCK_RELEASE (Lock);\
        }
/*
 * Macro : DLL_QUEUE_SCAN_WITH_LOCK_WRITE_CNTR
 *  Arguments: pDll - list
 *    pNode - returned node
 *    typecast - data structure type
 *    pNextPtrName - member name of the next pointer
 *    Lock - lock variable
 *    writeCntr - atomic variable used to avoid write starvation
 *    CmpAndIncRefCnt_fn - function that is used to search for a
 *         corresponding node.
 *PROTOTYPE:
 *     PSP_BOOL8 (CmpAndIncRefCnt_fn *)(pNode, pArg1, pOpaquePtr)
 * This function may do the following
 *  This function may have to take node lock (optional)
 *   - Take bDelete LOCK
 *   - if bDelete is TRUE, release bDelete Lock, return false
 *   - release bDelete LOCK
 *   - do comparison
 *   - if comparison fails return false
 *  following check is added, may be while doing the comparison, delete bit might have set by some other processor
 *   - Take bDelete LOCK
 *   - if bDelete is TRUE, release bDelete Lock, return false
 *   - do some additional operations like decrementing timeout
 *   - increment ref cnt (assuming that ref cnt is protected	by bDelete LOCK)
 *   - release bDelete LOCK
 *   - return TRUE
 * pArg1 - 2nd argument to the CmpAndIncRefCnt_fn function
 * pOpaquePtr - 3rd argument to the CmpAndIncRefCnt_fn function
 */
#define DLL_QUEUE_SCAN_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast, \
            pNextPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, pOpaquePtr) \
        {\
          PSP_WHILE_COUNTER_NOT_EQUAL_TO_ZERO(writeCntr);\
          PSP_READ_LOCK_TAKE (Lock);\
          for(pNode = (typecast)(DLL_QUEUE_FIRST ((pDll))); \
              pNode != NULL; \
              pNode = (typecast)\
                  (DLL_QUEUE_NEXT ((pDll),((struct dll_queue_node *)(pNode)))))\
          {\
            /*if (CmpAndIncRefCnt_fn ((pNode), (pArg1), (pOpaquePtr)) == TRUE) \*/\
            /*Refer to BugID:30742*/\
            if (CmpAndIncRefCnt_fn (DLL_QUEUE_LIST_MEMBER(pNode,typecast,pNextPtrName), pArg1, pOpaquePtr) == TRUE) \
              break;\
          }\
          PSP_READ_LOCK_RELEASE (Lock);\
        }

/* The above Macro is provided without the Lock. Some modules may not have reference count.
 * For them it is needed to take the lock prior to calling this function.
 * Then this will be helpful. As there is no reference count, the 'Cmp' function will
 * peform only compare. It can not do increment of reference count.
 */
#define DLL_QUEUE_SCAN_WITHOUT_LOCK(pDll, pNode, typecast, \
	pNextPtrName, Cmp_fn, pArg1, pOpaquePtr) \
	{\
          for(pNode = (typecast)(DLL_QUEUE_FIRST ((pDll))); \
              pNode != NULL; \
              pNode = (typecast)\
                  (DLL_QUEUE_NEXT ((pDll),((struct dll_queue_node *)(pNode)))))\
          {\
            if (Cmp_fn (DLL_QUEUE_LIST_MEMBER(pNode,typecast,pNextPtrName), pArg1, pOpaquePtr) == TRUE) \
              break;\
          }\
        }
/*
 * Macro : DLL_QUEUE_DYN_SCAN_WITH_LOCK_WRITE_CNTR
 *  Arguments: pDll - list
 *    pNode - node to be added
 *    pNext - Next Pointer
 *    typecast - data structure type
 *    pNextPtrName - member name of the next pointer
 *    Lock - lock variable
 *    writeCntr - atomic variable used to avoid write starvation
 *    CmpAndIncRefCnt_fn - function that is used to search for a
 *         corresponding node.
 *PROTOTYPE:
 *  PSP_BOOL8 (CmpAndIncRefCnt_fn *)(pNode, pArg1, pOpaquePtr)
 *  This function may do the following
 *  This function may have to take node lock (optional)
 *   - Take bDelete LOCK
 *   - if bDelete is TRUE, release bDelete Lock, return false
 *   - release bDelete LOCK
 *   - do comparison
 *   - if comparison fails return false
 *  following check is added, may be while doing the comparison, delete bit might have set by some other processor
 *   - Take bDelete LOCK
 *   - if bDelete is TRUE, release bDelete Lock, return false
 *   - do some additional operations like decrementing timeout
 *   - increment ref cnt (assuming that ref cnt is protected	by bDelete LOCK)
 *   - release bDelete LOCK
 *   - return TRUE
 * pArg1 - 2nd argument to the CmpAndIncRefCnt_fn function
 * pOpaquePtr - 3rd argument to the CmpAndIncRefCnt_fn function
 * UpdateRefCnt_fn - function to update the previous pointer reference count
 * PROTOTYPE :
 *   void (UpdateRefCnt_fn *)(pNode, PSP_BOOL8 bIncRecCnt)
 *   If bIncRecCnt is false, it will decrement the reference count
 *   If bIncRecCnt is TRUE, it will increment the reference count
 * Delete_fn - function to check the ref cnt , remove the node from
 *  the list and application specific operations (freeing the memory).
 *  THIS FUNCTION MUST NOT CALL MACROS which try to take the same locks
 *  THIS FUNCTION SHOULD NOT TAKE THE SAME LOCK
 *  PROTOTYPE :
 *   void (Delete_fn *)(pDll, pNode)
 *     It should invoke PSP_DllQ_DELETE_NODE, but not
 *         DLL_QUEUE_DELETE_NODE_WITH_LOCK,etc.
 * This macro does the following
 *  - Wait till the writeCntr is zero
 *  - READ LOCK TAKE
 *  - for ()
 *  -- Search for the corresponding node
 *  -- if it is found
 *  -- incrementing the reference count of the node is done in CmpAndIncRefCnt_fn
 *     and release the read lock
 *  -- This is done because:
 ***** we have to release read lock before acquiring the Write lock.
 ***** After releasing the read lock and before acquiring the write lock,
 ***** there is a chance that another processor may perform some delete
 ***** operations on the same node. To avoid that the reference count
 ***** is incremented
 * -- increment the write counter
 * -- WRITE LOCK TAKE
 * -- Decrement the reference count of the pNode
 * -- Call the delete function
 * -- RELEASE the WRITE LOCK
 * -- Decrement the WRITE counter
 * -- READ LOCK TAKE - this stmt added because after the for () loop,
 *     read lock release is done
 */
#define DLL_QUEUE_SCAN_WITH_LOCK_MATCHING_FN_WITH_NINE_ARGS( \
          pDll, pNode, typecast, pNextPtrName, Lock, CmpAndIncRefCnt_fn, \
          pArg1, pArg2, pArg3, pArg4, pArg5, pArg6, pArg7, pArg8, pArg9, bMode) \
        { \
           PSP_READ_LOCK_TAKE (Lock); \
           for(pNode = (typecast)(DLL_QUEUE_FIRST (pDll));pNode != NULL;pNode = (typecast) \
                       (DLL_QUEUE_NEXT ((pDll),((struct dll_queue_node *)(pNode))))) \
           { \
              if (CmpAndIncRefCnt_fn(DLL_QUEUE_LIST_MEMBER(pNode,typecast,pNextPtrName), \
                  pArg1, pArg2, pArg3, pArg4, pArg5, pArg6, pArg7, pArg8, pArg9, bMode) == TRUE)\
                  break; \
           } \
           PSP_READ_LOCK_RELEASE (Lock); \
        }


#define DLL_QUEUE_SCAN_WITH_LOCK_TAKE_AND_INC_REFCNT_FN(pDll, pNode, pNext, typecast, Lock, \
                             IncRefCnt_fn, pNodeMem, pNodeMemArg) \
          PSP_READ_LOCK_TAKE(Lock); \
          for (pNode = (typecast) (DLL_QUEUE_FIRST((pDll))),\
               pNext = ((pNode == NULL) ? NULL :\
               (typecast) (DLL_QUEUE_NEXT((pDll), (struct dll_queue_node *) (pNode))));\
               pNode != NULL;\
               pNode = pNext,\
               pNext = ((pNode == NULL) ? NULL :\
               (typecast) (DLL_QUEUE_NEXT((pDll), (struct dll_queue_node *) (pNode)))))\
          { \
              IncRefCnt_fn(&pNode->pNodeMem->pNodeMemArg);\


#define DLL_QUEUE_SCAN_END_WITH_LOCK_RELEASE(Lock) \
          }\
          PSP_READ_LOCK_RELEASE(Lock);



#define DLL_QUEUE_DYN_SCAN_WITH_LOCK_WRITE_CNTR(pDll, pNode, pNext,\
            typecast, pNextPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, pOpaquePtr, UpdateRefCnt_fn, Delete_fn) \
        {\
          PSP_WHILE_COUNTER_NOT_EQUAL_TO_ZERO(writeCntr);\
          PSP_READ_LOCK_TAKE (Lock);\
          for (pNode = (typecast) (DLL_QUEUE_FIRST((pDll))),\
               pNext = ((pNode == NULL) ? NULL :\
               (typecast) (DLL_QUEUE_NEXT((pDll), (struct dll_queue_node *) (pNode))));\
               pNode != NULL;\
               pNode = pNext,\
               pNext = ((pNode == NULL) ? NULL :\
               (typecast) (DLL_QUEUE_NEXT((pDll), (struct dll_queue_node *) (pNode)))))\
          {\
            /*if (CmpAndIncRefCnt_fn ((pNode), (pArg1), (pOpaquePtr)) == TRUE)*/ \
            /*Refer to BugID:30742*/\
            if (CmpAndIncRefCnt_fn (DLL_QUEUE_LIST_MEMBER(pNode,typecast,pNextPtrName), pArg1, pOpaquePtr) == TRUE) \
            {\
              PSP_READ_LOCK_RELEASE (Lock);\
              ATOMIC_INC (writeCntr);\
              WRITE_LOCK_TAKE (Lock);\
              ATOMIC_DEC (writeCntr);\
              UpdateRefCnt_fn ((pNode), FALSE);\
              Delete_fn ((pDll),(pNode));\
              WRITE_LOCK_RELEASE (Lock);\
              PSP_READ_LOCK_TAKE (Lock);\
              break;\
            }\
          }\
          PSP_READ_LOCK_RELEASE (Lock);\
        }

#define DLL_QUEUE_DYN_SCAN_FROM_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, pNext,\
            typecast, pNextPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, pOpaquePtr, UpdateRefCnt_fn, Delete_fn) \
        {\
          PSP_WHILE_COUNTER_NOT_EQUAL_TO_ZERO(writeCntr);\
          PSP_READ_LOCK_TAKE (Lock);\
          for (pNode = (typecast) (pNode),\
               pNext = ((pNode == NULL) ? NULL :\
               (typecast) (DLL_QUEUE_NEXT((pDll), (struct dll_queue_node *) (pNode))));\
               pNode != NULL;\
               pNode = pNext,\
               pNext = ((pNode == NULL) ? NULL :\
               (typecast) (DLL_QUEUE_NEXT((pDll), (struct dll_queue_node *) (pNode)))))\
          {\
            /*if (CmpAndIncRefCnt_fn ((pNode), (pArg1), (pOpaquePtr)) == TRUE)*/ \
            /*Refer to BugID:30742*/\
            if (CmpAndIncRefCnt_fn (DLL_QUEUE_LIST_MEMBER(pNode,typecast,pNextPtrName), pArg1, pOpaquePtr) == TRUE) \
            {\
              PSP_READ_LOCK_RELEASE (Lock);\
              ATOMIC_INC (writeCntr);\
              WRITE_LOCK_TAKE (Lock);\
              ATOMIC_DEC (writeCntr);\
              UpdateRefCnt_fn ((pNode), FALSE);\
              Delete_fn ((pDll),(pNode));\
              WRITE_LOCK_RELEASE (Lock);\
              PSP_READ_LOCK_TAKE (Lock);\
              break;\
            }\
          }\
          PSP_READ_LOCK_RELEASE (Lock);\
        }

/* macros that use SPIN lock follows ... */
/*
 * Macro : DLL_QUEUE_APPEND_NODE_WITH_LOCK
 *  Arguments: pDll - list
 *    pNode - node to be added
 *    typecast - data structure type
 *    pNextPtrName - member name of the next pointer
 *    Lock - lock variable
 */
#define DLL_QUEUE_APPEND_NODE_WITH_WRITE_LOCK(pDll, pNode, typecast, Lock) \
        { \
             WRITE_LOCK_TAKE (Lock); \
             DLL_QUEUE_APPEND_NODE((pDll), (typecast)(pNode)); \
             WRITE_LOCK_RELEASE (Lock); \
        }

#define DLL_QUEUE_DELETE_NODE_WITH_WRITE_LOCK(pDll, pNode, typecast, Lock) \
        { \
             WRITE_LOCK_TAKE (Lock); \
             DLL_QUEUE_DELETE_NODE((pDll), (typecast)pNode); \
             WRITE_LOCK_RELEASE (Lock); \
        }


#define DLL_QUEUE_APPEND_NODE_WITH_WRITE_LOCK_AND_INC_REFCNT_FN( \
              pDll, pNode, typecast, Lock, IncRefCnt_fn, pArg1) \
          {\
                 IncRefCnt_fn(pArg1);\
                 WRITE_LOCK_TAKE (Lock); \
                 DLL_QUEUE_APPEND_NODE((pDll), (typecast)pNode); \
                 WRITE_LOCK_RELEASE (Lock); \
          }

#define DLL_QUEUE_DELETE_NODE_WITH_WRITE_LOCK_AND_DEC_REFCNT_FN( \
              pDll, pNode, typecast, Lock, DecRefCnt_fn, pArg1) \
          {\
                 WRITE_LOCK_TAKE (Lock); \
                 DLL_QUEUE_DELETE_NODE((pDll), (typecast)pNode); \
                 WRITE_LOCK_RELEASE (Lock); \
                 DecRefCnt_fn(pArg1);\
          }


#define DLL_QUEUE_APPEND_NODE_WITH_LOCK(pDll, pNode, typecast,\
            pNextPtrName, Lock) \
        {\
          PSP_LOCK_TAKE (Lock);\
          DLL_QUEUE_APPEND_NODE((pDll), (pNode)); \
          PSP_LOCK_RELEASE (Lock);\
        }
#define DLL_QUEUE_PREPEND_NODE_WITH_LOCK(pDll, pNode, typecast,\
            pNextPtrName, Lock) \
        {\
          PSP_LOCK_TAKE (Lock);\
          DLL_QUEUE_PREPEND_NODE((pDll), (pNode)); \
          PSP_LOCK_RELEASE (Lock);\
        }
/*
 * Macro : DLL_QUEUE_DELETE_NODE_WITH_LOCK
 *  Arguments: pDll - list
 *    pNode - node to be deleted
 *    typecast - data structure type
 *    pNextPtrName - member name of the next pointer
 *    Lock - lock variable
 */
#define DLL_QUEUE_DELETE_NODE_WITH_LOCK(pDll, pNode, typecast,\
            pNextPtrName, Lock) \
        {\
          PSP_LOCK_TAKE (Lock);\
          DLL_QUEUE_DELETE_NODE ((pDll), (pNode));\
          PSP_LOCK_RELEASE (Lock);\
        }
/*
 * Macro : DLL_QUEUE_INSERT_NODE_WITH_LOCK
 *  Arguments: pDll - list
 *    pPrev - previous pointer to new node
 *    pNode - node to be added
 *    typecast - data structure type
 *    pNextPtrName - member name of the next pointer
 *    Lock - lock variable
 */
#define DLL_QUEUE_INSERT_NODE_WITH_LOCK(pDll, pPrev, pNode,\
            typecast, pNextPtrName, Lock) \
        {\
          PSP_LOCK_TAKE (Lock);\
          DLL_QUEUE_INSERT_NODE((pDll), (pPrev), (pNode));\
          PSP_LOCK_RELEASE (Lock);\
        }

/*
 * Macro : DLL_QUEUE_SCAN_WITH_LOCK
 *  Arguments: pDll - list
 *    pNode - returned node
 *    typecast - data structure type
 *    pNextPtrName - member name of the next pointer
 *    Lock - lock variable
 *    CmpAndIncRefCnt_fn - function that is used to search for a
 *         corresponding node.
 *PROTOTYPE:
 *  PSP_BOOL8 (CmpAndIncRefCnt_fn *)(pNode, pArg1, pOpaquePtr)
 * This function may do the following
 * This function may have to take node lock (optional)
 *   - Take bDelete LOCK
 *   - if bDelete is TRUE, release bDelete Lock, return false
 *   - release bDelete LOCK
 *   - do comparison
 *   - if comparison fails return false
 *  following check is added, may be while doing the comparison, delete bit might have set by some other processor
 *   - Take bDelete LOCK
 *   - if bDelete is TRUE, release bDelete Lock, return false
 *   - do some additional operations like decrementing timeout
 *   - increment ref cnt (assuming that ref cnt is protected	by bDelete LOCK)
 *   - release bDelete LOCK
 *   - return TRUE
 * pArg1 - 2nd argument to the CmpAndIncRefCnt_fn function
 * pOpaquePtr - 3rd argument to the CmpAndIncRefCnt_fn function
 */
#define DLL_QUEUE_SCAN_WITH_LOCK(pDll, pNode, typecast, \
            pNextPtrName, Lock, CmpAndIncRefCnt_fn, pArg1, pOpaquePtr) \
        {\
          PSP_LOCK_TAKE (Lock);\
          for(pNode = (typecast)(DLL_QUEUE_FIRST ((pDll))); \
              pNode != NULL; \
              pNode = (typecast)\
                  (DLL_QUEUE_NEXT ((pDll),((struct dll_queue_node *)(pNode)))))\
          {\
            /*if (CmpAndIncRefCnt_fn ((pNode), (pArg1), (pOpaquePtr)) == TRUE)*/ \
            /*Refer to BugID:30742*/\
            if (CmpAndIncRefCnt_fn (DLL_QUEUE_LIST_MEMBER(pNode,typecast,pNextPtrName), pArg1, pOpaquePtr) == TRUE) \
              break;\
          }\
          PSP_LOCK_RELEASE (Lock);\
        }
/*
 * Macro : DLL_QUEUE_DYN_SCAN_WITH_LOCK
 *  Arguments: pDll - list
 *    pNode - variable to traverse the list
 *    pNext - Next Pointer
 *    typecast - data structure type
 *    pNextPtrName - member name of the next pointer
 *    Lock - lock variable
 *    CmpAndIncRefCnt_fn - function that is used to search for a
 *         corresponding node.
 *PROTOTYPE:
 * PSP_BOOL8 (CmpAndIncRefCnt_fn *)(pNode, pArg1, pOpaquePtr)
 *  This function may do the following
 *  This function may have to take node lock (optional)
 *   - Take bDelete LOCK
 *   - if bDelete is TRUE, release bDelete Lock, return false
 *   - release bDelete LOCK
 *   - do comparison
 *   - if comparison fails return false
 *  following check is added, may be while doing the comparison, delete bit might have set by some other processor
 *   - Take bDelete LOCK
 *   - if bDelete is TRUE, release bDelete Lock, return false
 *   - do some additional operations like decrementing timeout
 *   - increment ref cnt (assuming that ref cnt is protected	by bDelete LOCK)
 *   - release bDelete LOCK
 *   - return TRUE
 * pArg1 - 2nd argument to the CmpAndIncRefCnt_fn function
 * pOpaquePtr - 3rd argument to the CmpAndIncRefCnt_fn function
 * UpdateRefCnt_fn - function to update the reference count
 * PROTOTYPE :
 *   void (UpdateRefCnt_fn *)(pNode, PSP_BOOL8 bIncRecCnt)
 *   If bIncRecCnt is false, it will decrement the reference count
 *   If bIncRecCnt is TRUE, it will increment the reference count
 * Delete_fn - function to check the ref cnt , remove the node from
 *  the list and application specific operations (freeing the memory).
 *  THIS FUNCTION MUST NOT CALL MACROS which try to take the same locks
 *  THIS FUNCTION SHOULD NOT TAKE THE SAME LOCK
 *  PROTOTYPE :
 *   void (Delete_fn *)(pDll, pNode)
 *     It should invoke PSP_DllQ_DELETE_NODE, but not
 *         DLL_QUEUE_DELETE_NODE_WITH_LOCK,etc.
 * This macro does the following
 *  - LOCK TAKE
 *  - for ()
 *  -- Search for the corresponding node
 *  -- if it is found
 *  -- decrement the reference count of pNode, because its incremented in
 *      in CmpAndIncRefCnt_fn()
 * -- Call the delete function
 * -- LOCK RELEASE
 */
#define DLL_QUEUE_DYN_SCAN_WITH_LOCK(pDll, pNode, pNext,\
            typecast, pNextPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, pOpaquePtr, UpdateRefCnt_fn, Delete_fn) \
        {\
          PSP_LOCK_TAKE (Lock);\
          for (pNode = (typecast) (DLL_QUEUE_FIRST((pDll))),\
               pNext = ((pNode == NULL) ? NULL :\
               (typecast) (DLL_QUEUE_NEXT((pDll), (struct dll_queue_node *) (pNode))));\
               pNode != NULL;\
               pNode = pNext,\
               pNext = ((pNode == NULL) ? NULL :\
               (typecast) (DLL_QUEUE_NEXT((pDll), (struct dll_queue_node *) (pNode)))))\
          {\
            /*if (CmpAndIncRefCnt_fn ((pNode), (pArg1), (pOpaquePtr)) == TRUE)*/ \
            /*Refer to BugID:30742*/\
            if (CmpAndIncRefCnt_fn (DLL_QUEUE_LIST_MEMBER(pNode,typecast,pNextPtrName), pArg1, pOpaquePtr) == TRUE) \
            {\
              Delete_fn (pDll, pNode);\
              break;\
            }\
          }\
          PSP_LOCK_RELEASE (Lock);\
        }
#define DLL_QUEUE_DYN_SCAN_FROM_NODE_WITH_LOCK(pDll, pNode, pNext,\
            typecast, pNextPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, pOpaquePtr, UpdateRefCnt_fn, Delete_fn) \
        {\
          PSP_LOCK_TAKE (Lock);\
          for (pNode = (typecast) (pNode),\
               pNext = ((pNode == NULL) ? NULL :\
               (typecast) (DLL_QUEUE_NEXT((pDll), (struct dll_queue_node *) (pNode))));\
               pNode != NULL;\
               pNode = pNext,\
               pNext = ((pNode == NULL) ? NULL :\
               (typecast) (DLL_QUEUE_NEXT((pDll), (struct dll_queue_node *) (pNode)))))\
          {\
            /*if (CmpAndIncRefCnt_fn ((pNode), (pArg1), (pOpaquePtr)) == TRUE)*/ \
            /*Refer to BugID:30742*/\
            if (CmpAndIncRefCnt_fn (DLL_QUEUE_LIST_MEMBER(pNode,typecast,pNextPtrName), pArg1, pOpaquePtr) == TRUE) \
            {\
              Delete_fn (pDll, pNode);\
              break;\
            }\
          }\
          PSP_LOCK_RELEASE (Lock);\
        }

#ifdef PSP_CONFIG_SMP

#define DLL_QUEUE_SMP_INIT_NODE(pNode) DLL_QUEUE_INIT_NODE(pNode)
#define DLL_QUEUE_SMP_INIT_LIST(pDll) DLL_QUEUE_INIT_LIST(pDll)
#define DLL_QUEUE_SMP_COUNT(pDll) PSP_DLLQ_COUNT(pDll)
#define DLL_QUEUE_SMP_FIRST(pDll) DLL_QUEUE_FIRST(pDll)
#define DLL_QUEUE_SMP_LAST(pDll) DLL_QUEUE_LAST(pDll)
#define DLL_QUEUE_SMP_NEXT(pDll,pNode) DLL_QUEUE_NEXT(pDll,pNode)
#define DLL_QUEUE_SMP_PREV(pDll,pNode) DLL_QUEUE_PREV(pDll,pNode)
#define DLL_QUEUE_SMP_IS_NODE_IN_LIST(pNode) DLL_QUEUE_IS_NODE_IN_LIST(pNode)
#define DLL_QUEUE_SMP_APPEND_NODE(pDll, pNode) \
          DLL_QUEUE_APPEND_NODE(pDll, pNode)
#define DLL_QUEUE_SMP_PREPEND_NODE(pDll, pNode) \
          DLL_QUEUE_PREPEND_NODE(pDll, pNode)
#define DLL_QUEUE_SMP_DELETE_NODE(pDll, pNode) \
          DLL_QUEUE_DELETE_NODE(pDll, pNode)
#define DLL_QUEUE_SMP_INSERT_NODE_IN_MIDDLE(pDll, pPrev, pNode, pNext) \
          DLL_QUEUE_INSERT_NODE_IN_MIDDLE(pDll, pPrev, pNode, pNext)
#define DLL_QUEUE_SMP_INSERT_NODE(pDll, pPrev, pNode) \
          DLL_QUEUE_INSERT_NODE(pDll, pPrev, pNode)
#define DLL_QUEUE_SMP_SCAN(pDll, pNode, typecast) \
          DLL_QUEUE_SCAN(pDll, pNode, typecast)
#define DLL_QUEUE_SMP_DYN_SCAN(pDll, pNode, pTmp, typecast) \
          DLL_QUEUE_DYN_SCAN(pDll, pNode, pTmp, typecast)
#define DLL_QUEUE_SMP_DYN_SCAN_FROM_NODE(pDll, pNode, pTmp, typecast) \
          DLL_QUEUE_DYN_SCAN_FROM_NODE(pDll, pNode, pTmp, typecast)

/* macros that use read-write lock with write counter follows ... */
#define DLL_QUEUE_SMP_APPEND_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast,\
            pNextPtrName, Lock, writeCntr) \
          DLL_QUEUE_APPEND_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast,\
            pNextPtrName, Lock, writeCntr)
#define DLL_QUEUE_SMP_PREPEND_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast,\
            pNextPtrName, Lock, writeCntr) \
          DLL_QUEUE_PREPEND_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast,\
            pNextPtrName, Lock, writeCntr)
#define DLL_QUEUE_SMP_DELETE_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast,\
            pNextPtrName, Lock, writeCntr) \
          DLL_QUEUE_DELETE_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast,\
            pNextPtrName, Lock, writeCntr)
#define DLL_QUEUE_SMP_INSERT_NODE_WITH_LOCK_WRITE_CNTR(pDll, pPrev, pNode,\
            typecast, pNextPtrName, Lock, writeCntr) \
          DLL_QUEUE_INSERT_NODE_WITH_LOCK_WRITE_CNTR(pDll, pPrev, pNode,\
            typecast, pNextPtrName, Lock, writeCntr)
#define DLL_QUEUE_SMP_SCAN_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast, \
            pNextPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, pOpaquePtr) \
          DLL_QUEUE_SCAN_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast, \
            pNextPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, pOpaquePtr)
#define DLL_QUEUE_SMP_DYN_SCAN_WITH_LOCK_WRITE_CNTR(pDll, pNode, pNext,\
            typecast, pNextPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, pOpaquePtr, UpdateRefCnt_fn, Delete_fn) \
          DLL_QUEUE_DYN_SCAN_WITH_LOCK_WRITE_CNTR(pDll, pNode, pNext,\
            typecast, pNextPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, pOpaquePtr, UpdateRefCnt_fn, Delete_fn)
#define DLL_QUEUE_SMP_DYN_SCAN_FROM_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, \
            pNext,\
            typecast, pNextPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, pOpaquePtr, UpdateRefCnt_fn, Delete_fn) \
          DLL_QUEUE_DYN_SCAN_FROM_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, pNext,\
            typecast, pNextPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, pOpaquePtr, UpdateRefCnt_fn, Delete_fn)

/* macros that use SPIN lock follows ... */
#define DLL_QUEUE_SMP_APPEND_NODE_WITH_LOCK(pDll, pNode, typecast,\
            pNextPtrName, Lock) \
          DLL_QUEUE_APPEND_NODE_WITH_LOCK(pDll, pNode, typecast,\
            pNextPtrName, Lock)
#define DLL_QUEUE_SMP_PREPEND_NODE_WITH_LOCK(pDll, pNode, typecast,\
            pNextPtrName, Lock) \
          DLL_QUEUE_PREPEND_NODE_WITH_LOCK(pDll, pNode, typecast,\
            pNextPtrName, Lock)
#define DLL_QUEUE_SMP_DELETE_NODE_WITH_LOCK(pDll, pNode, typecast,\
            pNextPtrName, Lock) \
          DLL_QUEUE_DELETE_NODE_WITH_LOCK(pDll, pNode, typecast,\
            pNextPtrName, Lock)
#define DLL_QUEUE_SMP_INSERT_NODE_WITH_LOCK(pDll, pPrev, pNode,\
            typecast, pNextPtrName, Lock) \
          DLL_QUEUE_INSERT_NODE_WITH_LOCK(pDll, pPrev, pNode,\
            typecast, pNextPtrName, Lock)
#define DLL_QUEUE_SMP_SCAN_WITH_LOCK(pDll, pNode, typecast, \
            pNextPtrName, Lock, CmpAndIncRefCnt_fn, pArg1, pOpaquePtr) \
          DLL_QUEUE_SCAN_WITH_LOCK(pDll, pNode, typecast, \
            pNextPtrName, Lock, CmpAndIncRefCnt_fn, pArg1, pOpaquePtr)
#define DLL_QUEUE_SMP_DYN_SCAN_WITH_LOCK(pDll, pNode, pNext,\
            typecast, pNextPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, pOpaquePtr, Delete_fn) \
          DLL_QUEUE_DYN_SCAN_WITH_LOCK(pDll, pNode, pNext,\
            typecast, pNextPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, pOpaquePtr, Delete_fn)
#define DLL_QUEUE_SMP_DYN_SCAN_FROM_NODE_WITH_LOCK(pDll, pNode, pNext,\
            typecast, pNextPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, pOpaquePtr, Delete_fn) \
          DLL_QUEUE_DYN_SCAN_FROM_NODE_WITH_LOCK(pDll, pNode, pNext,\
            typecast, pNextPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, pOpaquePtr, Delete_fn)
#else /* PSP_CONFIG_SMP */

#define DLL_QUEUE_SMP_INIT_NODE(pNode)
#define DLL_QUEUE_SMP_INIT_LIST(pDll)
#define DLL_QUEUE_SMP_COUNT(pDll)
#define DLL_QUEUE_SMP_FIRST(pDll)
#define DLL_QUEUE_SMP_LAST(pDll)
#define DLL_QUEUE_SMP_NEXT(pDll,pNode)
#define DLL_QUEUE_SMP_PREV(pDll,pNode)
#define DLL_QUEUE_SMP_IS_NODE_IN_LIST(pNode)
#define DLL_QUEUE_SMP_APPEND_NODE(pDll, pNode)
#define DLL_QUEUE_SMP_PREPEND_NODE(pDll, pNode)
#define DLL_QUEUE_SMP_DELETE_NODE(pDll, pNode)
#define DLL_QUEUE_SMP_INSERT_NODE_IN_MIDDLE(pDll, pPrev, pNode, pNext)
#define DLL_QUEUE_SMP_INSERT_NODE(pDll, pPrev, pNode)
#define DLL_QUEUE_SMP_SCAN(pDll, pNode, typecast)
#define DLL_QUEUE_SMP_DYN_SCAN(pDll, pNode, pTmp, typecast)
#define DLL_QUEUE_SMP_DYN_SCAN_FROM_NODE(pDll, pNode, pTmp, typecast)

/* macros that use read-write lock with write counter follows ... */
#define DLL_QUEUE_SMP_APPEND_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast,\
            pNextPtrName, Lock, writeCntr)
#define DLL_QUEUE_SMP_PREPEND_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast,\
            pNextPtrName, Lock, writeCntr)
#define DLL_QUEUE_SMP_DELETE_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast,\
            pNextPtrName, Lock, writeCntr)
#define DLL_QUEUE_SMP_INSERT_NODE_WITH_LOCK_WRITE_CNTR(pDll, pPrev, pNode,\
            typecast, pNextPtrName, Lock, writeCntr)
#define DLL_QUEUE_SMP_SCAN_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast, \
            pNextPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, pOpaquePtr)
#define DLL_QUEUE_SMP_DYN_SCAN_WITH_LOCK_WRITE_CNTR(pDll, pNode, pNext,\
            typecast, pNextPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, pOpaquePtr, UpdateRefCnt_fn, Delete_fn)
#define DLL_QUEUE_SMP_DYN_SCAN_FROM_NODE_WITH_LOCK_WRITE_CNTR(pDll, \
            pNode, pNext,\
            typecast, pNextPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, pOpaquePtr, UpdateRefCnt_fn, Delete_fn)

/* macros that use SPIN lock follows ... */
#define DLL_QUEUE_SMP_APPEND_NODE_WITH_LOCK(pDll, pNode, typecast,\
            pNextPtrName, Lock)
#define DLL_QUEUE_SMP_PREPEND_NODE_WITH_LOCK(pDll, pNode, typecast,\
            pNextPtrName, Lock)
#define DLL_QUEUE_SMP_DELETE_NODE_WITH_LOCK(pDll, pNode, typecast,\
            pNextPtrName, Lock)
#define DLL_QUEUE_SMP_INSERT_NODE_WITH_LOCK(pDll, pPrev, pNode,\
            typecast, pNextPtrName, Lock)
#define DLL_QUEUE_SMP_SCAN_WITH_LOCK(pDll, pNode, typecast, \
            pNextPtrName, Lock, CmpAndIncRefCnt_fn, pArg1, pOpaquePtr)
#define DLL_QUEUE_SMP_DYN_SCAN_WITH_LOCK(pDll, pNode, pNext,\
            typecast, pNextPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, pOpaquePtr, Delete_fn)
#define DLL_QUEUE_SMP_DYN_SCAN_FROM_NODE_WITH_LOCK(pDll, pNode, pNext,\
            typecast, pNextPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, pOpaquePtr, Delete_fn)
#endif /* PSPSPONFIG_SMP */

#endif /* _DLL_H */
