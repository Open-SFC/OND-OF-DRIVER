/* 
 *
 * Copyright  2012, 2013  Freescale Semiconductor
 *
 *
 * Licensed under the Apache License, version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
*/

/*
 *
 * File name: cmdll.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: This file contains the APIs prototypes and data structure definitions 
 *              which are used by applications for their doubly linked list operations.
 * 
 */

#ifndef UCMDLL_H
#define UCMDLL_H

/***************************************************************************/
/*                  DATA STRUCTURE DEFINITIONS                             */
/***************************************************************************/
typedef struct UCMDllQNode_s
{
  struct UCMDllQNode_s *pPrev;
  struct UCMDllQNode_s *next_p;
} UCMDllQNode_t;

typedef struct UCMDllQ_s
{
  UCMDllQNode_t Head;
  uint32_t ulCount;
} UCMDllQ_t;

/***************************************************************************/
/*                      FUNCTION PROTOTYPE DECLERATIONS                    */
/***************************************************************************/
void cm_dllq_init_list (UCMDllQ_t * pDll);
void cm_dllq_delete_in_middle (UCMDllQ_t * pDll, UCMDllQNode_t * pPrev,
                              UCMDllQNode_t * pNode, UCMDllQNode_t * next_p);
void cm_dllq_insert_in_middle (UCMDllQ_t * pDll, UCMDllQNode_t * pPrev,
                              UCMDllQNode_t * pNode, UCMDllQNode_t * next_p);

int32_t cm_dllq_insert_node_at_list_pos (UCMDllQ_t * pDll, UCMDllQNode_t * pNode,
                                    uint32_t uiListPos_p);

/***************************************************************************/
/*                      MACRO DEFINITIONS                                  */
/***************************************************************************/
#define CM_DLLQ_INIT_NODE(pNode) ((pNode)->next_p = (pNode)->pPrev = NULL)
#define CM_DLLQ_INIT_LIST(pDll)    cm_dllq_init_list ((pDll))
#define CM_DLLQ_COUNT(pDll) ((pDll)->ulCount)
#ifdef DSLIB_DBG
#define CM_DLLQ_FIRST(pDll) \
        ((CM_DLLQ_COUNT((pDll)) == 0) ? NULL: (pDll)->Head.next_p)
#define CM_DLLQ_LAST(pDll) \
        ((CM_DLLQ_COUNT((pDll)) == 0) ? NULL : (pDll)->Head.pPrev)
#define CM_DLLQ_NEXT(pDll,pNode) \
        (((pNode) == NULL) ? CM_DLLQ_FIRST(pDll) : \
        (((pNode)->next_p == &(pDll)->Head) ? NULL : (pNode)->next_p))
#define CM_DLLQ_PREV(pDll,pNode) \
        (((pNode) == NULL) ? CM_DLLQ_LAST (pDll) : \
        (((pNode)->pPrev == &(pDll)->Head) ? NULL : (pNode)->pPrev))
#define CM_DLLQ_IS_NODE_IN_LIST(pNode) \
                (((pNode)->next_p != NULL) && \
                ((pNode)->pPrev != NULL) && \
                ((pNode)->next_p->pPrev == pNode) && \
                ((pNode)->pPrev->next_p == pNode))
#define CM_DLLQ_APPEND_NODE(pDll, pNode) \
            cm_dllq_insert_in_middle ((pDll),(pDll)->Head.pPrev,(pNode),\
                                    &(pDll)->Head)
#define CM_DLLQ_PREPEND_NODE(pDll, pNode) \
            cm_dllq_insert_in_middle ((pDll),&(pDll)->Head, (pNode), \
                                   (pDll)->Head.next_p)
#define CM_DLLQ_INSERT_NODE(pDll, pPrev, pNode) \
            cm_dllq_insert_in_middle ((pDll), (pPrev), (pNode), ((pPrev)->next_p))
#define CM_DLLQ_INSERT_BEFORE_NODE(pDll, next_p, pNode) \
            cm_dllq_insert_in_middle ((pDll), ((next_p)->pPrev), (pNode), (next_p))
#define CM_DLLQ_INSERT_NODE_AT_LISTPOS(pDll,pNode,ListPos) \
            cm_dllq_insert_node_at_list_pos((pDll),(pNode),(ListPos))
#else
#define CM_DLLQ_FIRST(pDll) ((pDll)->Head.next_p)
#define CM_DLLQ_LAST(pDll)  ((pDll)->Head.pPrev)
#define CM_DLLQ_NEXT(pDll,pNode) ((pNode)->next_p)
#define CM_DLLQ_PREV(pDll,pNode) ((pNode)->pPrev)
#define CM_DLLQ_IS_NODE_IN_LIST(pNode) 1
#define CM_DLLQ_APPEND_NODE(pDll, pNode) \
            cm_dllq_insert_in_middle ((pDll),(pDll)->Head.pPrev,(pNode), NULL)
#define CM_DLLQ_PREPEND_NODE(pDll, pNode) \
            cm_dllq_insert_in_middle ((pDll),NULL, (pNode), (pDll)->Head.next_p)
#define CM_DLLQ_INSERT_NODE(pDll, pPrev, pNode) \
            ((pPrev != NULL) ?\
              (CM_DLLQ_INSERT_NODE_IN_MIDDLE((pDll), (pPrev), (pNode), \
                                      ((pPrev)->next_p))) :\
              (CM_DLLQ_INSERT_NODE_IN_MIDDLE ((pDll), NULL, (pNode),\
                                               (pDll)->Head.next_p)))
#define CM_DLLQ_INSERT_BEFORE_NODE(pDll, next_p1, pNode)\
             ((next_p1 != NULL) ?\
              (CM_DLLQ_INSERT_NODE_IN_MIDDLE((pDll), (next_p1)->pPrev, \
                                      (pNode), (next_p1))) : \
              (CM_DLLQ_INSERT_NODE_IN_MIDDLE((pDll), \
                                      (pDll)->Head.next_p, (pNode), (next_p1))))
#define CM_DLLQ_INSERT_NODE_AT_LISTPOS(pDll,pNode,ListPos) \
            cm_dllq_insert_node_at_list_pos((pDll),(pNode),(ListPos))
/* This routine is implemnted as needed by Traffic Anomaly Module.
   reference tags : AD_ECO_SYSTEM_LOCK_SUPPORT
 */
void cm_dllq_prepend_list (UCMDllQ_t * pNewListHead, UCMDllQ_t * pOldListHead);

#endif

#define CM_DLLQ_DELETE_NODE(pDll, pNode) \
        (CM_DLLQ_IS_NODE_IN_LIST ((pNode)) != 0) ? \
        (cm_dllq_delete_in_middle ((pDll),(pNode)->pPrev, \
                                 (pNode),(pNode)->next_p), 1) : 0
#define CM_DLLQ_INSERT_NODE_IN_MIDDLE(pDll, pPrev, pNode, next_p) \
            cm_dllq_insert_in_middle ((pDll), (pPrev), (pNode), (next_p))
#define CM_DLLQ_SCAN(pDll, pNode, typecast) \
        for(pNode = (typecast)(CM_DLLQ_FIRST ((pDll))); \
            pNode != NULL; \
            pNode = (typecast)(CM_DLLQ_NEXT ((pDll),((UCMDllQNode_t *)(pNode)))))
#define CM_DLLQ_DYN_SCAN(pDll, pNode, pTmp, typecast) \
        for (pNode = (typecast) (CM_DLLQ_FIRST((pDll))),\
             pTmp = ((pNode == NULL) ? NULL :\
               (typecast) (CM_DLLQ_NEXT((pDll), (UCMDllQNode_t *) (pNode))));\
             pNode != NULL;\
             pNode = pTmp,\
             pTmp = ((pNode == NULL) ? NULL :\
               (typecast) (CM_DLLQ_NEXT((pDll), (UCMDllQNode_t *) (pNode)))))

#define CM_DLLQ_DYN_SCAN_FROM_NODE(pDll, pNode, pTmp, typecast) \
        for (pNode = (typecast) (pNode),\
             pTmp = ((pNode == NULL) ? NULL :\
               (typecast) (CM_DLLQ_NEXT((pDll), (UCMDllQNode_t *) (pNode))));\
             pNode != NULL;\
             pNode = pTmp,\
             pTmp = ((pNode == NULL) ? NULL :\
               (typecast) (CM_DLLQ_NEXT((pDll), (UCMDllQNode_t *) (pNode)))))

#define CM_DLLQ_LIST_MEMBER(pNode, typecast, member) \
      ((typecast)((unsigned char *)(pNode)-(ulong)(&((typecast)0)->member)))

/* macros that use read-write lock with write counter follows ... */

/*
 * Macro : CM_DLLQ_APPEND_NODE_WITH_LOCK_WRITE_CNTR
 *  Arguments: pDll - list
 *    pNode - node to be added
 *    typecast - data structure type
 *    next_pPtrName - member name of the next pointer
 *    Lock - lock variable
 *    writeCntr - atomic variable used to avoid write starvation
 */
#define CM_DLLQ_APPEND_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast,\
            next_pPtrName, Lock, writeCntr) \
        {\
          CM_ATOMIC_INC (writeCntr);\
          CM_WRITE_LOCK_TAKE (Lock);\
          CM_ATOMIC_DEC (writeCntr);\
          CM_DLLQ_APPEND_NODE((pDll), (pNode)); \
          CM_WRITE_LOCK_RELEASE (Lock);\
        }
#define CM_DLLQ_PREPEND_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast,\
            next_pPtrName, Lock, writeCntr) \
        {\
          CM_ATOMIC_INC (writeCntr);\
          CM_WRITE_LOCK_TAKE (Lock);\
          CM_ATOMIC_DEC (writeCntr);\
          CM_DLLQ_PREPEND_NODE((pDll), (pNode)); \
          CM_WRITE_LOCK_RELEASE (Lock);\
        }
/*
 * Macro : CM_DLLQ_DELETE_NODE_WITH_LOCK_WRITE_CNTR
 *  Arguments: pDll - list
 *    pNode - node to be deleted
 *    typecast - data structure type
 *    next_pPtrName - member name of the next pointer
 *    Lock - lock variable
 *    writeCntr - atomic variable used to avoid write starvation
 */
#define CM_DLLQ_DELETE_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast,\
            next_pPtrName, Lock, writeCntr) \
        {\
          CM_ATOMIC_INC (writeCntr);\
          CM_WRITE_LOCK_TAKE (Lock);\
          CM_ATOMIC_DEC (writeCntr);\
          CM_DLLQ_DELETE_NODE ((pDll), (pNode));\
          CM_WRITE_LOCK_RELEASE (Lock);\
        }
/*
 * Macro : CM_DLLQ_INSERT_NODE_WITH_LOCK_WRITE_CNTR
 *  Arguments: pDll - list
 *    pPrev - previous pointer to new node
 *    pNode - node to be added
 *    typecast - data structure type
 *    next_pPtrName - member name of the next pointer
 *    Lock - lock variable
 *    writeCntr - atomic variable used to avoid write starvation
 */
#define CM_DLLQ_INSERT_NODE_WITH_LOCK_WRITE_CNTR(pDll, pPrev, pNode,\
            typecast, next_pPtrName, Lock, writeCntr) \
        {\
          CM_ATOMIC_INC (writeCntr);\
          CM_WRITE_LOCK_TAKE (Lock);\
          CM_ATOMIC_DEC (writeCntr);\
          CM_DLLQ_INSERT_NODE((pDll), (pPrev), (pNode));\
          CM_WRITE_LOCK_RELEASE (Lock);\
        }
/*Prashant*/
/*
 * Macro : CM_DLLQ_INSERT_BEFORE_NODE_WITH_LOCK_WRITE_CNTR
 *  Arguments: pDll - list
 *    pPrev - previous pointer to new node
 *    pNode - node to be added
 *    typecast - data structure type
 *    next_pPtrName - member name of the next pointer
 *    Lock - lock variable
 *    writeCntr - atomic variable used to avoid write starvation
 */
#define CM_DLLQ_INSERT_BEFORE_NODE_WITH_LOCK_WRITE_CNTR(pDll, next_p, pNode,\
            typecast, next_pPtrName, Lock, writeCntr) \
        {\
          CM_ATOMIC_INC (writeCntr);\
          CM_WRITE_LOCK_TAKE (Lock);\
          CM_ATOMIC_DEC (writeCntr);\
          CM_DLLQ_INSERT_BEFORE_NODE((pDll), (next_p), (pNode));\
          CM_WRITE_LOCK_RELEASE (Lock);\
        }
/*
 * Macro : CM_DLLQ_SCAN_WITH_LOCK_WRITE_CNTR
 *  Arguments: pDll - list
 *    pNode - returned node
 *    typecast - data structure type
 *    next_pPtrName - member name of the next pointer
 *    Lock - lock variable
 *    writeCntr - atomic variable used to avoid write starvation
 *    CmpAndIncRefCnt_fn - function that is used to search for a
 *         corresponding node. 
 *PROTOTYPE:
 *     unsigned char (CmpAndIncRefCnt_fn *)(pNode, pArg1, opaque_p)
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
 *   - increment ref cnt (assuming that ref cnt is protected  by bDelete LOCK)
 *   - release bDelete LOCK
 *   - return TRUE
 * pArg1 - 2nd argument to the CmpAndIncRefCnt_fn function
 * opaque_p - 3rd argument to the CmpAndIncRefCnt_fn function
 */
#define CM_DLLQ_SCAN_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast, \
            next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p) \
        {\
          CM_WHILE_COUNTER_NOT_EQUAL_TO_ZERO(writeCntr);\
          CM_READ_LOCK_TAKE (Lock);\
          for(pNode = (typecast)(CM_DLLQ_FIRST ((pDll))); \
              pNode != NULL; \
              pNode = (typecast)\
                  (CM_DLLQ_NEXT ((pDll),((UCMDllQNode_t *)(pNode)))))\
          {\
            /*if (CmpAndIncRefCnt_fn ((pNode), (pArg1), (opaque_p)) == TRUE) \*/\
            /*Refer to BugID:30742*/\
            if (CmpAndIncRefCnt_fn (CM_DLLQ_LIST_MEMBER(pNode,typecast,next_pPtrName), pArg1, opaque_p) == TRUE) \
              break;\
          }\
          CM_READ_LOCK_RELEASE (Lock);\
        }

/*
 * Macro : CM_DLLQ_DYN_SCAN_WITH_LOCK_WRITE_CNTR
 *  Arguments: pDll - list
 *    pNode - node to be added
 *    next_p - Next Pointer
 *    typecast - data structure type
 *    next_pPtrName - member name of the next pointer
 *    Lock - lock variable
 *    writeCntr - atomic variable used to avoid write starvation
 *    CmpAndIncRefCnt_fn - function that is used to search for a
 *         corresponding node. 
 *PROTOTYPE:
 *  unsigned char (CmpAndIncRefCnt_fn *)(pNode, pArg1, opaque_p)
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
 *   - increment ref cnt (assuming that ref cnt is protected  by bDelete LOCK)
 *   - release bDelete LOCK
 *   - return TRUE
 * pArg1 - 2nd argument to the CmpAndIncRefCnt_fn function
 * opaque_p - 3rd argument to the CmpAndIncRefCnt_fn function
 * UpdateRefCnt_fn - function to update the previous pointer reference count
 * PROTOTYPE :  
 *   void (UpdateRefCnt_fn *)(pNode, unsigned char bIncRecCnt)
 *   If bIncRecCnt is false, it will decrement the reference count
 *   If bIncRecCnt is TRUE, it will increment the reference count
 * Delete_fn - function to check the ref cnt , remove the node from
 *  the list and application specific operations (freeing the memory).
 *  THIS FUNCTION MUST NOT CALL MACROS which try to take the same locks
 *  THIS FUNCTION SHOULD NOT TAKE THE SAME LOCK
 *  PROTOTYPE :  
 *   void (Delete_fn *)(pDll, pNode)
 *     It should invoke CM_DllQ_DELETE_NODE, but not
 *         CM_DLLQ_DELETE_NODE_WITH_LOCK,etc.
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
#define CM_DLLQ_DYN_SCAN_WITH_LOCK_WRITE_CNTR(pDll, pNode, next_p,\
            typecast, next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn) \
        {\
          CM_WHILE_COUNTER_NOT_EQUAL_TO_ZERO(writeCntr);\
          CM_READ_LOCK_TAKE (Lock);\
          for (pNode = (typecast) (CM_DLLQ_FIRST((pDll))),\
               next_p = ((pNode == NULL) ? NULL :\
               (typecast) (CM_DLLQ_NEXT((pDll), (UCMDllQNode_t *) (pNode))));\
               pNode != NULL;\
               pNode = next_p,\
               next_p = ((pNode == NULL) ? NULL :\
               (typecast) (CM_DLLQ_NEXT((pDll), (UCMDllQNode_t *) (pNode)))))\
          {\
            /*if (CmpAndIncRefCnt_fn ((pNode), (pArg1), (opaque_p)) == TRUE)*/ \
            /*Refer to BugID:30742*/\
            if (CmpAndIncRefCnt_fn (CM_DLLQ_LIST_MEMBER(pNode,typecast,next_pPtrName), pArg1, opaque_p) == TRUE) \
            {\
              CM_READ_LOCK_RELEASE (Lock);\
              CM_ATOMIC_INC (writeCntr);\
              CM_WRITE_LOCK_TAKE (Lock);\
              CM_ATOMIC_DEC (writeCntr);\
              UpdateRefCnt_fn ((pNode), FALSE);\
              Delete_fn ((pDll),(pNode));\
              CM_WRITE_LOCK_RELEASE (Lock);\
              CM_READ_LOCK_TAKE (Lock);\
              break;\
            }\
          }\
          CM_READ_LOCK_RELEASE (Lock);\
        }

#define CM_DLLQ_DYN_SCAN_FROM_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, next_p,\
            typecast, next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn) \
        {\
          CM_WHILE_COUNTER_NOT_EQUAL_TO_ZERO(writeCntr);\
          CM_READ_LOCK_TAKE (Lock);\
          for (pNode = (typecast) (pNode),\
               next_p = ((pNode == NULL) ? NULL :\
               (typecast) (CM_DLLQ_NEXT((pDll), (UCMDllQNode_t *) (pNode))));\
               pNode != NULL;\
               pNode = next_p,\
               next_p = ((pNode == NULL) ? NULL :\
               (typecast) (CM_DLLQ_NEXT((pDll), (UCMDllQNode_t *) (pNode)))))\
          {\
            /*if (CmpAndIncRefCnt_fn ((pNode), (pArg1), (opaque_p)) == TRUE)*/ \
            /*Refer to BugID:30742*/\
            if (CmpAndIncRefCnt_fn (CM_DLLQ_LIST_MEMBER(pNode,typecast,next_pPtrName), pArg1, opaque_p) == TRUE) \
            {\
              CM_READ_LOCK_RELEASE (Lock);\
              CM_ATOMIC_INC (writeCntr);\
              CM_WRITE_LOCK_TAKE (Lock);\
              CM_ATOMIC_DEC (writeCntr);\
              UpdateRefCnt_fn ((pNode), FALSE);\
              Delete_fn ((pDll),(pNode));\
              CM_WRITE_LOCK_RELEASE (Lock);\
              CM_READ_LOCK_TAKE (Lock);\
              break;\
            }\
          }\
          CM_READ_LOCK_RELEASE (Lock);\
        }

/* macros that use SPIN lock follows ... */
/*
 * Macro : CM_DLLQ_APPEND_NODE_WITH_LOCK
 *  Arguments: pDll - list
 *    pNode - node to be added
 *    typecast - data structure type
 *    next_pPtrName - member name of the next pointer
 *    Lock - lock variable
 */
#define CM_DLLQ_APPEND_NODE_WITH_LOCK(pDll, pNode, typecast,\
            next_pPtrName, Lock) \
        {\
          CM_LOCK_TAKE (Lock);\
          CM_DLLQ_APPEND_NODE((pDll), (pNode)); \
          CM_LOCK_RELEASE (Lock);\
        }
#define CM_DLLQ_PREPEND_NODE_WITH_LOCK(pDll, pNode, typecast,\
            next_pPtrName, Lock) \
        {\
          CM_LOCK_TAKE (Lock);\
          CM_DLLQ_PREPEND_NODE((pDll), (pNode)); \
          CM_LOCK_RELEASE (Lock);\
        }
/*
 * Macro : CM_DLLQ_DELETE_NODE_WITH_LOCK
 *  Arguments: pDll - list
 *    pNode - node to be deleted
 *    typecast - data structure type
 *    next_pPtrName - member name of the next pointer
 *    Lock - lock variable
 */
#define CM_DLLQ_DELETE_NODE_WITH_LOCK(pDll, pNode, typecast,\
            next_pPtrName, Lock) \
        {\
          CM_LOCK_TAKE (Lock);\
          CM_DLLQ_DELETE_NODE ((pDll), (pNode));\
          CM_LOCK_RELEASE (Lock);\
        }
/*
 * Macro : CM_DLLQ_INSERT_NODE_WITH_LOCK
 *  Arguments: pDll - list
 *    pPrev - previous pointer to new node
 *    pNode - node to be added
 *    typecast - data structure type
 *    next_pPtrName - member name of the next pointer
 *    Lock - lock variable
 */
#define CM_DLLQ_INSERT_NODE_WITH_LOCK(pDll, pPrev, pNode,\
            typecast, next_pPtrName, Lock) \
        {\
          CM_LOCK_TAKE (Lock);\
          CM_DLLQ_INSERT_NODE((pDll), (pPrev), (pNode));\
          CM_LOCK_RELEASE (Lock);\
        }

/*
 * Macro : CM_DLLQ_SCAN_WITH_LOCK
 *  Arguments: pDll - list
 *    pNode - returned node
 *    typecast - data structure type
 *    next_pPtrName - member name of the next pointer
 *    Lock - lock variable
 *    CmpAndIncRefCnt_fn - function that is used to search for a
 *         corresponding node. 
 *PROTOTYPE:
 *  unsigned char (CmpAndIncRefCnt_fn *)(pNode, pArg1, opaque_p)
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
 *   - increment ref cnt (assuming that ref cnt is protected  by bDelete LOCK)
 *   - release bDelete LOCK
 *   - return TRUE
 * pArg1 - 2nd argument to the CmpAndIncRefCnt_fn function
 * opaque_p - 3rd argument to the CmpAndIncRefCnt_fn function
 */
#define CM_DLLQ_SCAN_WITH_LOCK(pDll, pNode, typecast, \
            next_pPtrName, Lock, CmpAndIncRefCnt_fn, pArg1, opaque_p) \
        {\
          CM_LOCK_TAKE (Lock);\
          for(pNode = (typecast)(CM_DLLQ_FIRST ((pDll))); \
              pNode != NULL; \
              pNode = (typecast)\
                  (CM_DLLQ_NEXT ((pDll),((UCMDllQNode_t *)(pNode)))))\
          {\
            /*if (CmpAndIncRefCnt_fn ((pNode), (pArg1), (opaque_p)) == TRUE)*/ \
            /*Refer to BugID:30742*/\
            if (CmpAndIncRefCnt_fn (CM_DLLQ_LIST_MEMBER(pNode,typecast,next_pPtrName), pArg1, opaque_p) == TRUE) \
              break;\
          }\
          CM_LOCK_RELEASE (Lock);\
        }
/*
 * Macro : CM_DLLQ_DYN_SCAN_WITH_LOCK
 *  Arguments: pDll - list
 *    pNode - variable to traverse the list
 *    next_p - Next Pointer
 *    typecast - data structure type
 *    next_pPtrName - member name of the next pointer
 *    Lock - lock variable
 *    CmpAndIncRefCnt_fn - function that is used to search for a
 *         corresponding node. 
 *PROTOTYPE:
 * unsigned char (CmpAndIncRefCnt_fn *)(pNode, pArg1, opaque_p)
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
 *   - increment ref cnt (assuming that ref cnt is protected  by bDelete LOCK)
 *   - release bDelete LOCK
 *   - return TRUE
 * pArg1 - 2nd argument to the CmpAndIncRefCnt_fn function
 * opaque_p - 3rd argument to the CmpAndIncRefCnt_fn function
 * UpdateRefCnt_fn - function to update the reference count
 * PROTOTYPE :  
 *   void (UpdateRefCnt_fn *)(pNode, unsigned char bIncRecCnt)
 *   If bIncRecCnt is false, it will decrement the reference count
 *   If bIncRecCnt is TRUE, it will increment the reference count
 * Delete_fn - function to check the ref cnt , remove the node from
 *  the list and application specific operations (freeing the memory).
 *  THIS FUNCTION MUST NOT CALL MACROS which try to take the same locks
 *  THIS FUNCTION SHOULD NOT TAKE THE SAME LOCK
 *  PROTOTYPE :  
 *   void (Delete_fn *)(pDll, pNode)
 *     It should invoke CM_DllQ_DELETE_NODE, but not
 *         CM_DLLQ_DELETE_NODE_WITH_LOCK,etc.
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
#define CM_DLLQ_DYN_SCAN_WITH_LOCK(pDll, pNode, next_p,\
            typecast, next_pPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn) \
        {\
          CM_LOCK_TAKE (Lock);\
          for (pNode = (typecast) (CM_DLLQ_FIRST((pDll))),\
               next_p = ((pNode == NULL) ? NULL :\
               (typecast) (CM_DLLQ_NEXT((pDll), (UCMDllQNode_t *) (pNode))));\
               pNode != NULL;\
               pNode = next_p,\
               next_p = ((pNode == NULL) ? NULL :\
               (typecast) (CM_DLLQ_NEXT((pDll), (UCMDllQNode_t *) (pNode)))))\
          {\
            /*if (CmpAndIncRefCnt_fn ((pNode), (pArg1), (opaque_p)) == TRUE)*/ \
            /*Refer to BugID:30742*/\
            if (CmpAndIncRefCnt_fn (CM_DLLQ_LIST_MEMBER(pNode,typecast,next_pPtrName), pArg1, opaque_p) == TRUE) \
            {\
              Delete_fn (pDll, pNode);\
              break;\
            }\
          }\
          CM_LOCK_RELEASE (Lock);\
        }
#define CM_DLLQ_DYN_SCAN_FROM_NODE_WITH_LOCK(pDll, pNode, next_p,\
            typecast, next_pPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn) \
        {\
          CM_LOCK_TAKE (Lock);\
          for (pNode = (typecast) (pNode),\
               next_p = ((pNode == NULL) ? NULL :\
               (typecast) (CM_DLLQ_NEXT((pDll), (UCMDllQNode_t *) (pNode))));\
               pNode != NULL;\
               pNode = next_p,\
               next_p = ((pNode == NULL) ? NULL :\
               (typecast) (CM_DLLQ_NEXT((pDll), (UCMDllQNode_t *) (pNode)))))\
          {\
            /*if (CmpAndIncRefCnt_fn ((pNode), (pArg1), (opaque_p)) == TRUE)*/ \
            /*Refer to BugID:30742*/\
            if (CmpAndIncRefCnt_fn (CM_DLLQ_LIST_MEMBER(pNode,typecast,next_pPtrName), pArg1, opaque_p) == TRUE) \
            {\
              Delete_fn (pDll, pNode);\
              break;\
            }\
          }\
          CM_LOCK_RELEASE (Lock);\
        }

#ifdef CONFIG_SMP
typedef UCMDllQNode_t UCMSMPDllQNode_t;
typedef UCMDllQ_t UCMSMPDllQ_t;

#define CM_SMP_DLLQ_INIT_NODE(pNode) CM_DLLQ_INIT_NODE(pNode)
#define CM_SMP_DLLQ_INIT_LIST(pDll) CM_DLLQ_INIT_LIST(pDll)
#define CM_SMP_DLLQ_COUNT(pDll) CM_DLLQ_COUNT(pDll)
#define CM_SMP_DLLQ_FIRST(pDll) CM_DLLQ_FIRST(pDll)
#define CM_SMP_DLLQ_LAST(pDll) CM_DLLQ_LAST(pDll)
#define CM_SMP_DLLQ_NEXT(pDll,pNode) CM_DLLQ_NEXT(pDll,pNode)
#define CM_SMP_DLLQ_PREV(pDll,pNode) CM_DLLQ_PREV(pDll,pNode)
#define CM_SMP_DLLQ_IS_NODE_IN_LIST(pNode) CM_DLLQ_IS_NODE_IN_LIST(pNode)
#define CM_SMP_DLLQ_APPEND_NODE(pDll, pNode) \
          CM_DLLQ_APPEND_NODE(pDll, pNode)
#define CM_SMP_DLLQ_PREPEND_NODE(pDll, pNode) \
          CM_DLLQ_PREPEND_NODE(pDll, pNode)
#define CM_SMP_DLLQ_DELETE_NODE(pDll, pNode) \
          CM_DLLQ_DELETE_NODE(pDll, pNode)
#define CM_SMP_DLLQ_INSERT_NODE_IN_MIDDLE(pDll, pPrev, pNode, next_p) \
          CM_DLLQ_INSERT_NODE_IN_MIDDLE(pDll, pPrev, pNode, next_p)
#define CM_SMP_DLLQ_INSERT_NODE(pDll, pPrev, pNode) \
          CM_DLLQ_INSERT_NODE(pDll, pPrev, pNode)
#define CM_SMP_DLLQ_SCAN(pDll, pNode, typecast) \
          CM_DLLQ_SCAN(pDll, pNode, typecast)
#define CM_SMP_DLLQ_DYN_SCAN(pDll, pNode, pTmp, typecast) \
          CM_DLLQ_DYN_SCAN(pDll, pNode, pTmp, typecast)
#define CM_SMP_DLLQ_DYN_SCAN_FROM_NODE(pDll, pNode, pTmp, typecast) \
          CM_DLLQ_DYN_SCAN_FROM_NODE(pDll, pNode, pTmp, typecast)

/* macros that use read-write lock with write counter follows ... */
#define CM_SMP_DLLQ_APPEND_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast,\
            next_pPtrName, Lock, writeCntr) \
          CM_DLLQ_APPEND_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast,\
            next_pPtrName, Lock, writeCntr)
#define CM_SMP_DLLQ_PREPEND_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast,\
            next_pPtrName, Lock, writeCntr) \
          CM_DLLQ_PREPEND_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast,\
            next_pPtrName, Lock, writeCntr)
#define CM_SMP_DLLQ_DELETE_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast,\
            next_pPtrName, Lock, writeCntr) \
          CM_DLLQ_DELETE_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast,\
            next_pPtrName, Lock, writeCntr)
#define CM_SMP_DLLQ_INSERT_NODE_WITH_LOCK_WRITE_CNTR(pDll, pPrev, pNode,\
            typecast, next_pPtrName, Lock, writeCntr) \
          CM_DLLQ_INSERT_NODE_WITH_LOCK_WRITE_CNTR(pDll, pPrev, pNode,\
            typecast, next_pPtrName, Lock, writeCntr)
#define CM_SMP_DLLQ_SCAN_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast, \
            next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p) \
          CM_DLLQ_SCAN_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast, \
            next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p)
#define CM_SMP_DLLQ_DYN_SCAN_WITH_LOCK_WRITE_CNTR(pDll, pNode, next_p,\
            typecast, next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn) \
          CM_DLLQ_DYN_SCAN_WITH_LOCK_WRITE_CNTR(pDll, pNode, next_p,\
            typecast, next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn)
#define CM_SMP_DLLQ_DYN_SCAN_FROM_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, \
            next_p,\
            typecast, next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn) \
          CM_DLLQ_DYN_SCAN_FROM_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, next_p,\
            typecast, next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn)

/* macros that use SPIN lock follows ... */
#define CM_SMP_DLLQ_APPEND_NODE_WITH_LOCK(pDll, pNode, typecast,\
            next_pPtrName, Lock) \
          CM_DLLQ_APPEND_NODE_WITH_LOCK(pDll, pNode, typecast,\
            next_pPtrName, Lock)
#define CM_SMP_DLLQ_PREPEND_NODE_WITH_LOCK(pDll, pNode, typecast,\
            next_pPtrName, Lock) \
          CM_DLLQ_PREPEND_NODE_WITH_LOCK(pDll, pNode, typecast,\
            next_pPtrName, Lock)
#define CM_SMP_DLLQ_DELETE_NODE_WITH_LOCK(pDll, pNode, typecast,\
            next_pPtrName, Lock) \
          CM_DLLQ_DELETE_NODE_WITH_LOCK(pDll, pNode, typecast,\
            next_pPtrName, Lock)
#define CM_SMP_DLLQ_INSERT_NODE_WITH_LOCK(pDll, pPrev, pNode,\
            typecast, next_pPtrName, Lock) \
          CM_DLLQ_INSERT_NODE_WITH_LOCK(pDll, pPrev, pNode,\
            typecast, next_pPtrName, Lock)
#define CM_SMP_DLLQ_SCAN_WITH_LOCK(pDll, pNode, typecast, \
            next_pPtrName, Lock, CmpAndIncRefCnt_fn, pArg1, opaque_p) \
          CM_DLLQ_SCAN_WITH_LOCK(pDll, pNode, typecast, \
            next_pPtrName, Lock, CmpAndIncRefCnt_fn, pArg1, opaque_p)
#define CM_SMP_DLLQ_DYN_SCAN_WITH_LOCK(pDll, pNode, next_p,\
            typecast, next_pPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, Delete_fn) \
          CM_DLLQ_DYN_SCAN_WITH_LOCK(pDll, pNode, next_p,\
            typecast, next_pPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, Delete_fn)
#define CM_SMP_DLLQ_DYN_SCAN_FROM_NODE_WITH_LOCK(pDll, pNode, next_p,\
            typecast, next_pPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, Delete_fn) \
          CM_DLLQ_DYN_SCAN_FROM_NODE_WITH_LOCK(pDll, pNode, next_p,\
            typecast, next_pPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, Delete_fn)
#else /* CONFIG_SMP */
typedef struct UCMSMPDllQNode_s
{
} UCMSMPDllQNode_t;
typedef struct UCMSMPDllQ_s
{
} UCMSMPDllQ_t;

#define CM_SMP_DLLQ_INIT_NODE(pNode)
#define CM_SMP_DLLQ_INIT_LIST(pDll)
#define CM_SMP_DLLQ_COUNT(pDll)
#define CM_SMP_DLLQ_FIRST(pDll)
#define CM_SMP_DLLQ_LAST(pDll)
#define CM_SMP_DLLQ_NEXT(pDll,pNode)
#define CM_SMP_DLLQ_PREV(pDll,pNode)
#define CM_SMP_DLLQ_IS_NODE_IN_LIST(pNode)
#define CM_SMP_DLLQ_APPEND_NODE(pDll, pNode)
#define CM_SMP_DLLQ_PREPEND_NODE(pDll, pNode)
#define CM_SMP_DLLQ_DELETE_NODE(pDll, pNode)
#define CM_SMP_DLLQ_INSERT_NODE_IN_MIDDLE(pDll, pPrev, pNode, next_p)
#define CM_SMP_DLLQ_INSERT_NODE(pDll, pPrev, pNode)
#define CM_SMP_DLLQ_SCAN(pDll, pNode, typecast)
#define CM_SMP_DLLQ_DYN_SCAN(pDll, pNode, pTmp, typecast)
#define CM_SMP_DLLQ_DYN_SCAN_FROM_NODE(pDll, pNode, pTmp, typecast)

/* macros that use read-write lock with write counter follows ... */
#define CM_SMP_DLLQ_APPEND_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast,\
            next_pPtrName, Lock, writeCntr)
#define CM_SMP_DLLQ_PREPEND_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast,\
            next_pPtrName, Lock, writeCntr)
#define CM_SMP_DLLQ_DELETE_NODE_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast,\
            next_pPtrName, Lock, writeCntr)
#define CM_SMP_DLLQ_INSERT_NODE_WITH_LOCK_WRITE_CNTR(pDll, pPrev, pNode,\
            typecast, next_pPtrName, Lock, writeCntr)
#define CM_SMP_DLLQ_SCAN_WITH_LOCK_WRITE_CNTR(pDll, pNode, typecast, \
            next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p)
#define CM_SMP_DLLQ_DYN_SCAN_WITH_LOCK_WRITE_CNTR(pDll, pNode, next_p,\
            typecast, next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn)
#define CM_SMP_DLLQ_DYN_SCAN_FROM_NODE_WITH_LOCK_WRITE_CNTR(pDll, \
            pNode, next_p,\
            typecast, next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn)

/* macros that use SPIN lock follows ... */
#define CM_SMP_DLLQ_APPEND_NODE_WITH_LOCK(pDll, pNode, typecast,\
            next_pPtrName, Lock)
#define CM_SMP_DLLQ_PREPEND_NODE_WITH_LOCK(pDll, pNode, typecast,\
            next_pPtrName, Lock)
#define CM_SMP_DLLQ_DELETE_NODE_WITH_LOCK(pDll, pNode, typecast,\
            next_pPtrName, Lock)
#define CM_SMP_DLLQ_INSERT_NODE_WITH_LOCK(pDll, pPrev, pNode,\
            typecast, next_pPtrName, Lock)
#define CM_SMP_DLLQ_SCAN_WITH_LOCK(pDll, pNode, typecast, \
            next_pPtrName, Lock, CmpAndIncRefCnt_fn, pArg1, opaque_p)
#define CM_SMP_DLLQ_DYN_SCAN_WITH_LOCK(pDll, pNode, next_p,\
            typecast, next_pPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, Delete_fn)
#define CM_SMP_DLLQ_DYN_SCAN_FROM_NODE_WITH_LOCK(pDll, pNode, next_p,\
            typecast, next_pPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, Delete_fn)
#endif /* CONFIG_SMP */

#endif /* UCMDLL_H */
