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
 * File name: ucmsll.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: This file contains the APIs prototypes and data structure definitions
 *              which are used by applications for their Hash Table with
 *              singly linked list operations.
 * 
 */

#ifndef _SLL_H
#define _SLL_H

/***************************************************************************/
/*                  DATA STRUCTURE DEFINITIONS                             */
/***************************************************************************/
typedef struct UCMSllQNode_s
{
  struct UCMSllQNode_s *next_p;
} UCMSllQNode_t;

typedef struct UCMSllQ_s
{
  UCMSllQNode_t Head;
  UCMSllQNode_t *pTail;
  uint32_t ulCount;
} UCMSllQ_t;

/***************************************************************************/
/*                      FUNCTION PROTOTYPE DECLERATIONS                    */
/***************************************************************************/
void cm_sllq_init_list (UCMSllQ_t * pSll);
void cm_sllq_append_node (UCMSllQ_t * pSll, UCMSllQNode_t * pSllNode);
void cm_sllq_prepend_node (UCMSllQ_t * pSll, UCMSllQNode_t * pSllNode);
void cm_sllq_delete_node (UCMSllQ_t * pSll, UCMSllQNode_t * pSllNode);
void cm_sllq_insert_node (UCMSllQ_t * pSll, UCMSllQNode_t * pPrevNode,
                          UCMSllQNode_t * pSllNode, UCMSllQNode_t * next_node_p);
void cm_sllq_delete_node_in_middle (UCMSllQ_t * pSll, UCMSllQNode_t * pPrev,
                                  UCMSllQNode_t * pNode);
UCMSllQNode_t *cm_sllq_pop_node (UCMSllQ_t * pSll);
/***************************************************************************/
/*                      MACRO DEFINITIONS                                  */
/***************************************************************************/
#define CM_SLLQ_INIT_NODE(pNode) ((pNode)->next_p = NULL)
#define CM_SLLQ_INIT_LIST(pSll)    cm_sllq_init_list ((pSll))
#define CM_SLLQ_COUNT(pSll) ((pSll)->ulCount)
#ifdef DSLIB_DBG
#define CM_SLLQ_FIRST(pSll) ((CM_SLLQ_COUNT((pSll)) == 0) \
                              ? NULL : (pSll)->Head.next_p)
#define CM_SLLQ_LAST(pSll)  ((CM_SLLQ_COUNT((pSll)) == 0) \
                              ? NULL : (pSll)->pTail)
#define CM_SLLQ_NEXT(pSll,pNode)\
                ((pNode == NULL) ? \
                 CM_SLLQ_FIRST (pSll) : \
                 (((pNode)->next_p == &(pSll)->Head) ? NULL \
                                 : (pNode)->next_p))
#define CM_SLLQ_IS_NODE_IN_LIST(pNode) (pNode->next_p != NULL)
#else
#define CM_SLLQ_FIRST(pSll) ((pSll)->Head.next_p)
#define CM_SLLQ_LAST(pSll)  ((pSll)->pTail)
#define CM_SLLQ_NEXT(pSll,pNode) ((pNode)->next_p)
#define CM_SLLQ_IS_NODE_IN_LIST(pNode) 1
#endif
#define CM_SLLQ_APPEND_NODE(pSll, pNode) cm_sllq_append_node ((pSll), (pNode))
#define CM_SLLQ_PREPEND_NODE(pSll, pNode) cm_sllq_prepend_node ((pSll), (pNode))
#define CM_SLLQ_INSERT_NODE(pSll, pPrev, pNode, next_p) \
                     cm_sllq_insert_node ((pSll), (pPrev), (pNode), (next_p))
#define CM_SLLQ_DELETE_NODE(pSll, pNode) \
        (CM_SLLQ_IS_NODE_IN_LIST ((pNode)) != 0) ? \
        (cm_sllq_delete_node ((pSll), (pNode)), 1) : 0
#define CM_SLLQ_DELETE_NODE_IN_MIDDLE(pSll, pPrev, pNode) \
        (CM_SLLQ_IS_NODE_IN_LIST ((pNode)) != 0) ? \
        (cm_sllq_delete_node_in_middle ((pSll), (pPrev), (pNode)), 1) : 0
#define CM_SLLQ_SCAN(pSll, pNode, typecast) \
        for (pNode = (typecast) (CM_SLLQ_FIRST ((pSll)));\
             pNode != NULL;\
             pNode = \
             (typecast) CM_SLLQ_NEXT ((pSll), (UCMSllQNode_t *)(pNode)))

#define CM_SLLQ_DYN_SCAN(pSll, pNode, pTmp, typecast) \
        for (pNode = (typecast) (CM_SLLQ_FIRST((pSll))),\
             pTmp = ((pNode == NULL) ? NULL :\
               (typecast) (CM_SLLQ_NEXT((pSll), (UCMSllQNode_t *) (pNode))));\
             pNode != NULL;\
             pNode = pTmp,\
             pTmp = ((pNode == NULL) ? NULL :\
               (typecast) (CM_SLLQ_NEXT((pSll), (UCMSllQNode_t *) (pNode)))))

#define CM_SLLQ_DYN_SCAN_FROM_NODE(pSll, pNode, pTmp, typecast) \
        for (pNode = (typecast) (pNode),\
             pTmp = ((pNode == NULL) ? NULL :\
               (typecast) (CM_SLLQ_NEXT((pSll), (UCMSllQNode_t *) (pNode))));\
             pNode != NULL;\
             pNode = pTmp,\
             pTmp = ((pNode == NULL) ? NULL :\
               (typecast) (CM_SLLQ_NEXT((pSll), (UCMSllQNode_t *) (pNode)))))

#define CM_SLLQ_LIST_MEMBER(pNode, typecast, member) \
      ((typecast)((unsigned char *)(pNode)-(uint32_t)(&((typecast)0)->member)))

#define CM_SLLQ_POP_NODE_WITH_LOCK(pSll,Lock,pNode) \
{ \
  CM_LOCK_TAKE(Lock); \
  pNode = cm_sllq_pop_node(pSll); \
  CM_LOCK_RELEASE(Lock); \
}

/* macros that use read-write lock with write counter follows ... */

/*
 * Macro : CM_SLLQ_APPEND_NODE_WITH_LOCK_WRITE_CNTR
 *  Arguments: pSll - list
 *    pNode - node to be added
 *    typecast - data structure type
 *    next_pPtrName - member name of the next pointer
 *    Lock - lock variable
 *    writeCntr - atomic variable used to avoid write starvation
 */
#define CM_SLLQ_APPEND_NODE_WITH_LOCK_WRITE_CNTR(pSll, pNode, typecast,\
            next_pPtrName, Lock, writeCntr) \
        {\
          CM_ATOMIC_INC (writeCntr);\
          CM_WRITE_LOCK_TAKE (Lock);\
          CM_SLLQ_APPEND_NODE ((pSll), (pNode));\
          CM_WRITE_LOCK_RELEASE (Lock);\
          CM_ATOMIC_DEC (writeCntr);\
        }
#define CM_SLLQ_PREPEND_NODE_WITH_LOCK_WRITE_CNTR(pSll, pNode, typecast, \
            next_pPtrName, Lock, writeCntr) \
        {\
          CM_ATOMIC_INC (writeCntr);\
          CM_WRITE_LOCK_TAKE (Lock);\
          CM_SLLQ_PREPEND_NODE ((pSll), (pNode));\
          CM_WRITE_LOCK_RELEASE (Lock);\
          CM_ATOMIC_DEC (writeCntr);\
        }
/*
 * Macro : CM_SLLQ_INSERT_NODE_WITH_LOCK_WRITE_CNTR
 *  Arguments: pDll - list
 *    pPrev - previous pointer to new node
 *    pNode - node to be added
 *    next_p - Next pointer to the new node
 *    typecast - data structure type
 *    next_pPtrName - member name of the next pointer
 *    Lock - lock variable
 *    writeCntr - atomic variable used to avoid write starvation
 */
#define CM_SLLQ_INSERT_NODE_WITH_LOCK_WRITE_CNTR(pSll, pPrev, pNode, next_p,\
            typecast, next_pPtrName, Lock, writeCntr) \
        {\
          CM_ATOMIC_INC (writeCntr);\
          CM_WRITE_LOCK_TAKE (Lock);\
          CM_SLLQ_INSERT_NODE ((pSll), (pPrev), (pNode), (next_p));\
          CM_WRITE_LOCK_RELEASE (Lock);\
          CM_ATOMIC_DEC (writeCntr);\
        }
/*
 * Macro : CM_SLLQ_DELETE_NODE_WITH_LOCK_WRITE_CNTR
 *  Arguments: pSll - list
 *    pPrev - Previous of the deleted node
 *    pNode - node to be deleted
 *    typecast - data structure type
 *    next_pPtrName - member name of the next pointer
 *    Lock - lock variable
 *    writeCntr - atomic variable used to avoid write starvation
 *
 * This macro is used to delete the pNode.
 * If the pPrev is passed and pPrev->next_p is pNode, then
 *  without traversal it will delete the node
 * else
 *  it will traverse and search for the pNode and if found, it deletes it
 */
#define CM_SLLQ_DELETE_NODE_WITH_LOCK_WRITE_CNTR(pSll, pPrev, pNode, \
            typecast, next_pPtrName, Lock, writeCntr) \
        {\
          CM_ATOMIC_INC (writeCntr);\
          CM_WRITE_LOCK_TAKE (Lock);\
          if (CM_SLLQ_IS_NODE_IN_LIST ((pNode)) != 0) \
            CM_SLLQ_DELETE_NODE_IN_MIDDLE ((pSll), (pPrev), (pNode));\
          CM_WRITE_LOCK_RELEASE (Lock);\
          CM_ATOMIC_DEC (writeCntr);\
        }

/*
 * Macro : CM_SLLQ_SCAN_WITH_LOCK_WRITE_CNTR
 *  Arguments: pSll - list
 *    pNode - node to be added
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
 *   - increment ref cnt (assuming that ref cnt is protected by bDelete LOCK)
 *   - release bDelete LOCK
 *   - return TRUE
 * pArg1 - 2nd argument to the CmpAndIncRefCnt_fn function
 * opaque_p - 3rd argument to the CmpAndIncRefCnt_fn function
 */
#define CM_SLLQ_SCAN_WITH_LOCK_WRITE_CNTR(pSll, pNode, typecast, \
            next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, pArg1, \
            opaque_p) \
        {\
          CM_WHILE_COUNTER_NOT_EQUAL_TO_ZERO(writeCntr);\
          CM_READ_LOCK_TAKE (Lock);\
          for (pNode = (typecast) (CM_SLLQ_FIRST ((pSll)));\
               pNode != NULL;\
               pNode = \
             (typecast) CM_SLLQ_NEXT ((pSll), (UCMSllQNode_t *)(pNode))) \
          {\
            /*if (CmpAndIncRefCnt_fn ((pNode), (pArg1), (opaque_p)) == TRUE)*/ \
            /*Refer to BugID:30742*/\
            if (CmpAndIncRefCnt_fn (CM_SLLQ_LIST_MEMBER(pNode,typecast,next_pPtrName), pArg1, opaque_p) == TRUE) \
              break;\
          }\
          CM_READ_LOCK_RELEASE (Lock);\
        }

/*
 * Macro : CM_SLLQ_DYN_SCAN_WITH_LOCK_WRITE_CNTR
 *  Arguments: pSll - list
 *    pPrev - previous pointer of the node to be deleted
 *    pNode - node to be deleted
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
 *    If bIncRecCnt is FALSE, it will decrement the reference count
 *    If bIncRecCnt is TRUE, it will increment the reference count
 * Delete_fn - function to check the ref cnt , remove the node from
 *  the list and do application specific operations (freeing the memory).
 *  THIS FUNCTION MUST NOT CALL MACROS which try to take the same locks
 *  THIS FUNCTION SHOULD NOT TAKE THE SAME LOCK
 *  THIS FUNCTION SHOULD NOT DECREMENT THE REFERENCE COUNT
 *  PROTOTYPE :  
 *   void (Delete_fn *)(pSll, pPrev, pNode)
 *     It should invoke CM_DllQ_DELETE_NODE, but not
 *         CM_DLLQ_DELETE_NODE_WITH_LOCK,etc.
 * This macro does the following
 *  - Wait till the writeCntr is zero
 *  - READ LOCK TAKE
 *  - for ()
 *  -- Search for the corresponding node
 *  -- if it is found
 *  --  increment the reference count of the previous node
 *       This is done  because:
 *******  Next pointer field in the previous pointer will be modified.
 *******  So, the previous pointer should not be deleted by another processor.
 *  --  incrementing the reference count of the node is done in CmpAndIncRefCnt_fn()
 *  --  and release the read lock
 *  -- This is done because:
 ***** we have to release read lock before acquiring the Write lock. 
 ***** After releasing the read lock and before acquiring the write lock,
 ***** there is a chance that another processor may perform some delete
 ***** operations on the same node. To avoid that the reference count
 ***** is incremented
 * -- increment the write counter
 * -- WRITE LOCK TAKE
 * -- Decrement the reference count of the pNode. Because we incremented in
 *     in CmpAndIncRefCnt_fn function
 * -- Call the delete function
 * -- Decrement the reference counct of the pPrev
 * -- RELEASE the WRITE LOCK
 * -- Decrement the WRITE counter
 * -- READ LOCK TAKE - this stmt added because after the for () loop,
 *     read lock release is done
 */
#define CM_SLLQ_DYN_SCAN_WITH_LOCK_WRITE_CNTR(pSll, pPrev, pNode, next_p, \
            typecast, next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn) \
        {\
          CM_WHILE_COUNTER_NOT_EQUAL_TO_ZERO(writeCntr);\
          CM_READ_LOCK_TAKE (Lock);\
          for (pNode = (typecast) (CM_SLLQ_FIRST((pSll))),\
             next_p = ((pNode == NULL) ? NULL :\
               (typecast) (CM_SLLQ_NEXT((pSll), (UCMSllQNode_t *) (pNode))));\
             pNode != NULL;\
             pPrev = pNode, pNode = next_p, \
             next_p = ((pNode == NULL) ? NULL :\
               (typecast) (CM_SLLQ_NEXT((pSll), (UCMSllQNode_t *) (pNode)))))\
          {\
            /*if (CmpAndIncRefCnt_fn ((pNode), (pArg1), (opaque_p)) == TRUE)*/ \
            /*Refer to BugID:30742*/\
            if (CmpAndIncRefCnt_fn (CM_SLLQ_LIST_MEMBER(pNode,typecast,next_pPtrName), pArg1, opaque_p) == TRUE) \
            {\
              UpdateRefCnt_fn ((pPrev), TRUE);\
              CM_READ_LOCK_RELEASE (Lock);\
              CM_ATOMIC_INC (writeCntr);\
              CM_WRITE_LOCK_TAKE (Lock);\
              UpdateRefCnt_fn ((pNode), FALSE);\
              Delete_fn ((pSll), (pPrev), (pNode));\
              UpdateRefCnt_fn ((pPrev), FALSE);\
              CM_WRITE_LOCK_RELEASE (Lock);\
              CM_ATOMIC_DEC (writeCntr);\
              CM_READ_LOCK_TAKE (Lock);\
              break;\
            }\
          }\
          CM_READ_LOCK_RELEASE (Lock);\
        }

#define CM_SLLQ_DYN_SCAN_FROM_NODE_WITH_LOCK_WRITE_CNTR(pSll, pPrev, pNode, \
            next_p, \
            typecast, next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn) \
        {\
          CM_WHILE_COUNTER_NOT_EQUAL_TO_ZERO(writeCntr);\
          CM_READ_LOCK_TAKE (Lock);\
          for (pNode = (typecast) (pNode),\
             next_p = ((pNode == NULL) ? NULL :\
               (typecast) (CM_SLLQ_NEXT((pSll), (UCMSllQNode_t *) (pNode))));\
             pNode != NULL;\
             pPrev = pNode, pNode = next_p, \
             next_p = ((pNode == NULL) ? NULL :\
               (typecast) (CM_SLLQ_NEXT((pSll), (UCMSllQNode_t *) (pNode)))))\
          {\
            /*if (CmpAndIncRefCnt_fn ((pNode), (pArg1), (opaque_p)) == TRUE)*/ \
            /*Refer to BugID:30742*/\
            if (CmpAndIncRefCnt_fn (CM_SLLQ_LIST_MEMBER(pNode,typecast,next_pPtrName), pArg1, opaque_p) == TRUE) \
            {\
              UpdateRefCnt_fn ((pPrev), TRUE);\
              CM_READ_LOCK_RELEASE (Lock);\
              CM_ATOMIC_INC (writeCntr);\
              CM_WRITE_LOCK_TAKE (Lock);\
              UpdateRefCnt_fn ((pNode), FALSE);\
              Delete_fn ((pSll), (pPrev), (pNode));\
              UpdateRefCnt_fn ((pPrev), FALSE);\
              CM_WRITE_LOCK_RELEASE (Lock);\
              CM_ATOMIC_DEC (writeCntr);\
              CM_READ_LOCK_TAKE (Lock);\
              break;\
            }\
          }\
          CM_READ_LOCK_RELEASE (Lock);\
        }

/* macros that use SPIN lock follows ... */

/*
 * Macro : CM_SLLQ_APPEND_NODE_WITH_LOCK
 *  Arguments: pSll - list
 *    pNode - node to be added
 *    typecast - data structure type
 *    next_pPtrName - member name of the next pointer
 *    Lock - lock variable
 */
#define CM_SLLQ_APPEND_NODE_WITH_LOCK(pSll, pNode, typecast,\
            next_pPtrName, Lock) \
        {\
          CM_LOCK_TAKE (Lock);\
          CM_SLLQ_APPEND_NODE ((pSll), (pNode));\
          CM_LOCK_RELEASE (Lock);\
        }
#define CM_SLLQ_PREPEND_NODE_WITH_LOCK(pSll, pNode, typecast, \
            next_pPtrName, Lock) \
        {\
          CM_LOCK_TAKE (Lock);\
          CM_SLLQ_PREPEND_NODE ((pSll), (pNode));\
          CM_LOCK_RELEASE (Lock);\
        }

/*
 * Macro : CM_SLLQ_INSERT_NODE_WITH_LOCK
 *  Arguments: pDll - list
 *    pPrev - previous pointer to new node
 *    pNode - node to be added
 *    next_p - Next pointer to the new node
 *    typecast - data structure type
 *    next_pPtrName - member name of the next pointer
 *    Lock - lock variable
 */
#define CM_SLLQ_INSERT_NODE_WITH_LOCK(pSll, pPrev, pNode, next_p,\
            typecast, next_pPtrName, Lock) \
        {\
          CM_LOCK_TAKE (Lock);\
          CM_SLLQ_INSERT_NODE ((pSll), (pPrev), (pNode), (next_p));\
          CM_LOCK_RELEASE (Lock);\
        }

/*
 * Macro : CM_SLLQ_DELETE_NODE_WITH_LOCK
 *  Arguments: pSll - list
 *    pPrev - Previous of the deleted node
 *    pNode - node to be deleted
 *    typecast - data structure type
 *    next_pPtrName - member name of the next pointer
 *    Lock - lock variable
 *
 * This macro is used to delete the pNode.
 * If the pPrev is passed and pPrev->next_p is not NULL, then
 *  without traversal it will delete the node
 * else
 *  it will traverse and search for the pNode and if found, it deletes it
 */
#define CM_SLLQ_DELETE_NODE_WITH_LOCK(pSll, pPrev, pNode, typecast, \
            next_pPtrName, Lock) \
        {\
          CM_LOCK_TAKE (Lock);\
          if (CM_SLLQ_IS_NODE_IN_LIST ((pNode)) != 0) \
            CM_SLLQ_DELETE_NODE_IN_MIDDLE ((pSll), (pPrev), (pNode));\
          CM_LOCK_RELEASE (Lock);\
        }

/*
 * Macro : CM_SLLQ_SCAN_WITH_LOCK
 *  Arguments: pSll - list
 *    pNode - node to be added
 *    typecast - data structure type
 *    next_pPtrName - member name of the next pointer
 *    Lock - lock variable
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
#define CM_SLLQ_SCAN_WITH_LOCK(pSll, pNode, typecast, next_pPtrName, \
            Lock, CmpAndIncRefCnt_fn, pArg1, opaque_p) \
        {\
          CM_LOCK_TAKE (Lock);\
          for (pNode = (typecast) (CM_SLLQ_FIRST ((pSll)));\
               pNode != NULL;\
               pNode = \
             (typecast) CM_SLLQ_NEXT ((pSll), (UCMSllQNode_t *)(pNode))) \
          {\
            /*if (CmpAndIncRefCnt_fn ((pNode), (pArg1), (opaque_p)) == TRUE)*/ \
            /*Refer to BugID:30742*/\
            if (CmpAndIncRefCnt_fn (CM_SLLQ_LIST_MEMBER(pNode,typecast,next_pPtrName), pArg1, opaque_p) == TRUE) \
              break;\
          }\
          CM_LOCK_RELEASE (Lock);\
        }
/*
 * Macro : CM_SLLQ_DYN_SCAN_WITH_LOCK
 *  Arguments: pSll - list
 *    pPrev - variable used to store the previous pointer
 *    pNode - node that is deleted
 *    next_p - variable used to store the Next pointer
 *    typecast - data structure type
 *    next_pPtrName - member name of the next pointer
 *    Lock - lock variable
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
 * UpdateRefCnt_fn - function to update the reference count
 * PROTOTYPE :  
 *   void (UpdateRefCnt_fn *)(pNode, unsigned char bIncRecCnt)
 *    If bIncRecCnt is FALSE, it will decrement the reference count
 *    If bIncRecCnt is TRUE, it will increment the reference count
 * Delete_fn - function to check the ref cnt , remove the node from
 *  the list and do application specific operations (freeing the memory).
 *  THIS FUNCTION MUST NOT CALL MACROS which try to take the same locks
 *  THIS FUNCTION SHOULD NOT TAKE THE SAME LOCK
 *  THIS FUNCTION SHOULD NOT DECREMENT THE REFERENCE COUNT
 *  PROTOTYPE :  
 *   void (Delete_fn *)(pSll, pPrev, pNode)
 *     It should invoke CM_DllQ_DELETE_NODE, but not
 *         CM_DLLQ_DELETE_NODE_WITH_LOCK,etc.
 * This macro does the following
 *  - LOCK TAKE
 *  - for ()
 *  -- Search for the corresponding node
 *  -- if it is found
 * -- Decrement the reference count of the pNode. Because we incremented in
 *     in CmpAndIncRefCnt_fn function
 * -- Call the delete function
 * -- RELEASE the LOCK
 */
#define CM_SLLQ_DYN_SCAN_WITH_LOCK(pSll, pPrev, pNode, next_p, typecast, \
            next_pPtrName, Lock, CmpAndIncRefCnt_fn, pArg1, \
            opaque_p, UpdateRefCnt_fn, Delete_fn) \
        {\
          CM_LOCK_TAKE (Lock);\
          for (pNode = (typecast) (CM_SLLQ_FIRST((pSll))),\
             next_p = ((pNode == NULL) ? NULL :\
               (typecast) (CM_SLLQ_NEXT((pSll), (UCMSllQNode_t *) (pNode))));\
             pNode != NULL;\
             pPrev = pNode, pNode = next_p, \
             next_p = ((pNode == NULL) ? NULL :\
               (typecast) (CM_SLLQ_NEXT((pSll), (UCMSllQNode_t *) (pNode)))))\
          {\
            /*if (CmpAndIncRefCnt_fn ((pNode), (pArg1), (opaque_p)) == TRUE)*/ \
            /*Refer to BugID:30742*/\
            if (CmpAndIncRefCnt_fn (CM_SLLQ_LIST_MEMBER(pNode,typecast,next_pPtrName), pArg1, opaque_p) == TRUE) \
            {\
              UpdateRefCnt_fn(pNode, FALSE);\
              Delete_fn (pSll, pPrev, pNode);\
              break; \
            }\
          }\
          CM_LOCK_RELEASE (Lock);\
        }

#define CM_SLLQ_DYN_SCAN_FROM_NODE_WITH_LOCK(pSll, pPrev, pNode, \
            next_p, typecast, \
            next_pPtrName, Lock, CmpAndIncRefCnt_fn, pArg1, \
            opaque_p, UpdateRefCnt_fn, Delete_fn) \
        {\
          CM_LOCK_TAKE (Lock);\
          for (pNode = (typecast) (pNode),\
             next_p = ((pNode == NULL) ? NULL :\
               (typecast) (CM_SLLQ_NEXT((pSll), (UCMSllQNode_t *) (pNode))));\
             pNode != NULL;\
             pPrev = pNode, pNode = next_p, \
             next_p = ((pNode == NULL) ? NULL :\
               (typecast) (CM_SLLQ_NEXT((pSll), (UCMSllQNode_t *) (pNode)))))\
          {\
            /*if (CmpAndIncRefCnt_fn ((pNode), (pArg1), (opaque_p)) == TRUE)*/ \
            /*Refer to BugID:30742*/\
            if (CmpAndIncRefCnt_fn (CM_SLLQ_LIST_MEMBER(pNode,typecast,next_pPtrName), pArg1, opaque_p) == TRUE) \
            {\
              UpdateRefCnt_fn(pNode, FALSE);\
              Delete_fn (pSll, pPrev, pNode);\
              break; \
            }\
          }\
          CM_LOCK_RELEASE (Lock);\
        }

#ifdef CONFIG_SMP
typedef UCMSllQNode_t UCMSMPSllQNode_t;
typedef UCMSllQ_t UCMSMPSllQ_t;
#define  CM_SMP_SLLQ_INIT_NODE(pNode)          CM_SLLQ_INIT_NODE(pNode)
#define  CM_SMP_SLLQ_INIT_LIST(pSll)           CM_SLLQ_INIT_LIST(pSll)
#define  CM_SMP_SLLQ_COUNT(pSll)               CM_SLLQ_COUNT(pSll)
#define  CM_SMP_SLLQ_FIRST(pSll)               CM_SLLQ_FIRST(pSll)
#define  CM_SMP_SLLQ_LAST(pSll)                CM_SLLQ_LAST(pSll)
#define  CM_SMP_SLLQ_IS_NODE_IN_LIST(pNode)    CM_SLLQ_IS_NODE_IN_LIST(pNode)
#define  CM_SMP_SLLQ_NEXT(pSll,pNode)          CM_SLLQ_NEXT(pSll,pNode)
#define  CM_SMP_SLLQ_APPEND_NODE(pSll, pNode) \
                    CM_SLLQ_APPEND_NODE(pSll, pNode)
#define  CM_SMP_SLLQ_PREPEND_NODE(pSll, pNode) \
                    CM_SLLQ_PREPEND_NODE(pSll, pNode)
#define  CM_SMP_SLLQ_INSERT_NODE(pSll, pPrev, pNode, next_p) \
                    CM_SLLQ_INSERT_NODE(pSll, pPrev, pNode, next_p)
#define  CM_SMP_SLLQ_DELETE_NODE(pSll, pNode)  \
                    CM_SLLQ_DELETE_NODE(pSll, pNode)
#define  CM_SMP_SLLQ_SCAN(pSll, pNode, typecast) \
                    CM_SLLQ_SCAN(pSll, pNode, typecast)
#define  CM_SMP_SLLQ_DYN_SCAN(pSll, pNode, pTmp, typecast) \
                    CM_SLLQ_DYN_SCAN(pSll, pNode, pTmp, typecast)
#define  CM_SMP_SLLQ_DYN_SCAN_FROM_NODE(pSll, pNode, pTmp, typecast) \
                    CM_SLLQ_DYN_SCAN_FROM_NODE(pSll, pNode, pTmp, typecast)

#define CM_SMP_SLLQ_POP_NODE_WITH_LOCK(pSll,pLock,pNode) \
              CM_SLLQ_POP_NODE_WITH_LOCK(pSll,pLock,pNode)

/* macros that use read-write lock with write counter follows ... */
#define CM_SMP_SLLQ_APPEND_NODE_WITH_LOCK_WRITE_CNTR(pSll, pNode, typecast,\
            next_pPtrName, Lock, writeCntr) \
          CM_SLLQ_APPEND_NODE_WITH_LOCK_WRITE_CNTR(pSll, pNode, typecast,\
            next_pPtrName, Lock, writeCntr)
#define CM_SMP_SLLQ_PREPEND_NODE_WITH_LOCK_WRITE_CNTR(pSll, pNode, typecast, \
            next_pPtrName, Lock, writeCntr) \
          CM_SLLQ_PREPEND_NODE_WITH_LOCK_WRITE_CNTR(pSll, pNode, typecast, \
            next_pPtrName, Lock, writeCntr)
#define CM_SMP_SLLQ_INSERT_NODE_WITH_LOCK_WRITE_CNTR(pSll, pPrev, pNode, \
            next_p, typecast, next_pPtrName, Lock, writeCntr) \
          CM_SLLQ_INSERT_NODE_WITH_LOCK_WRITE_CNTR(pSll, pPrev, pNode, next_p,\
            typecast, next_pPtrName, Lock, writeCntr)
#define CM_SMP_SLLQ_DELETE_NODE_WITH_LOCK_WRITE_CNTR(pSll, pPrev, pNode, \
            typecast, next_pPtrName, Lock, writeCntr) \
          CM_SLLQ_DELETE_NODE_WITH_LOCK_WRITE_CNTR(pSll, pPrev, pNode, \
            typecast, next_pPtrName, Lock, writeCntr)
#define CM_SMP_SLLQ_SCAN_WITH_LOCK_WRITE_CNTR(pSll, pNode, typecast, \
            next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, pArg1, \
            opaque_p) \
          CM_SLLQ_SCAN_WITH_LOCK_WRITE_CNTR(pSll, pNode, typecast, \
            next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, pArg1, \
            opaque_p)
#define CM_SMP_SLLQ_DYN_SCAN_WITH_LOCK_WRITE_CNTR(pSll, pPrev, pNode, next_p, \
            typecast, next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn) \
          CM_SLLQ_DYN_SCAN_WITH_LOCK_WRITE_CNTR(pSll, pPrev, pNode, next_p, \
            typecast, next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn)
#define CM_SMP_SLLQ_DYN_SCAN_FROM_NODE_WITH_LOCK_WRITE_CNTR(pSll, pPrev, \
            pNode, next_p, \
            typecast, next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn) \
          CM_SLLQ_DYN_SCAN_FROM_NODE_WITH_LOCK_WRITE_CNTR(pSll, pPrev, \
            pNode, next_p, \
            typecast, next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn)

/* macros that use SPIN lock follows ... */
#define CM_SMP_SLLQ_APPEND_NODE_WITH_LOCK(pSll, pNode, typecast,\
            next_pPtrName, Lock) \
          CM_SLLQ_APPEND_NODE_WITH_LOCK(pSll, pNode, typecast,\
            next_pPtrName, Lock)
#define CM_SMP_SLLQ_PREPEND_NODE_WITH_LOCK(pSll, pNode, typecast, \
            next_pPtrName, Lock) \
          CM_SLLQ_PREPEND_NODE_WITH_LOCK(pSll, pNode, typecast, \
            next_pPtrName, Lock)
#define CM_SMP_SLLQ_INSERT_NODE_WITH_LOCK(pSll, pPrev, pNode, next_p,\
            typecast, next_pPtrName, Lock) \
          CM_SLLQ_INSERT_NODE_WITH_LOCK(pSll, pPrev, pNode, next_p,\
            typecast, next_pPtrName, Lock)
#define CM_SMP_SLLQ_DELETE_NODE_WITH_LOCK(pSll, pPrev, pNode, typecast, \
            next_pPtrName, Lock) \
          CM_SLLQ_DELETE_NODE_WITH_LOCK(pSll, pPrev, pNode, typecast, \
            next_pPtrName, Lock)
#define CM_SMP_SLLQ_SCAN_WITH_LOCK(pSll, pNode, typecast, next_pPtrName, \
            Lock, CmpAndIncRefCnt_fn, pArg1, opaque_p) \
          CM_SLLQ_SCAN_WITH_LOCK(pSll, pNode, typecast, next_pPtrName, \
            Lock, CmpAndIncRefCnt_fn, pArg1, opaque_p)
#define CM_SMP_SLLQ_DYN_SCAN_WITH_LOCK(pSll, pPrev, pNode, next_p, typecast, \
            next_pPtrName, Lock, CmpAndIncRefCnt_fn, pArg1, \
            opaque_p, Delete_fn, bProceedLoop) \
          CM_SLLQ_DYN_SCAN_WITH_LOCK(pSll, pPrev, pNode, next_p, typecast, \
            next_pPtrName, Lock, CmpAndIncRefCnt_fn, pArg1, \
            opaque_p, Delete_fn, bProceedLoop)
#define CM_SMP_SLLQ_DYN_SCAN_FROM_NODE_WITH_LOCK(pSll, pPrev, pNode, \
            next_p, typecast, \
            next_pPtrName, Lock, CmpAndIncRefCnt_fn, pArg1, \
            opaque_p, Delete_fn, bProceedLoop) \
          CM_SLLQ_DYN_SCAN_FROM_NODE_WITH_LOCK(pSll, pPrev, pNode, \
            next_p, typecast, \
            next_pPtrName, Lock, CmpAndIncRefCnt_fn, pArg1, \
            opaque_p, Delete_fn, bProceedLoop)
#else /* CONFIG_SMP */
typedef struct UCMSMPSllQNode_s
{
} UCMSMPSllQNode_t;
typedef struct UCMSMPSllQ_s
{
} UCMSMPSllQ_t;

#define  CM_SMP_SLLQ_POP_NODE_WITH_LOCK(pSll,pLock,pNode)
#define  CM_SMP_SLLQ_INIT_NODE(pNode)
#define  CM_SMP_SLLQ_INIT_LIST(pSll)
#define  CM_SMP_SLLQ_COUNT(pSll)
#define  CM_SMP_SLLQ_FIRST(pSll)
#define  CM_SMP_SLLQ_LAST(pSll)
#define  CM_SMP_SLLQ_IS_NODE_IN_LIST(pNode)
#define  CM_SMP_SLLQ_NEXT(pSll,pNode)
#define  CM_SMP_SLLQ_APPEND_NODE(pSll, pNode)
#define  CM_SMP_SLLQ_PREPEND_NODE(pSll, pNode)
#define  CM_SMP_SLLQ_INSERT_NODE(pSll, pPrev, pNode, next_p)
#define  CM_SMP_SLLQ_DELETE_NODE(pSll, pNode)
#define  CM_SMP_SLLQ_SCAN(pSll, pNode, typecast)
#define  CM_SMP_SLLQ_DYN_SCAN(pSll, pNode, pTmp, typecast)
#define  CM_SMP_SLLQ_DYN_SCAN_FROM_NODE(pSll, pNode, pTmp, typecast)

/* macros that use read-write lock with write counter follows ... */
#define CM_SMP_SLLQ_APPEND_NODE_WITH_LOCK_WRITE_CNTR(pSll, pNode, typecast,\
            next_pPtrName, Lock, writeCntr)
#define CM_SMP_SLLQ_PREPEND_NODE_WITH_LOCK_WRITE_CNTR(pSll, pNode, typecast, \
            next_pPtrName, Lock, writeCntr)
#define CM_SMP_SLLQ_INSERT_NODE_WITH_LOCK_WRITE_CNTR(pSll, pPrev, pNode, \
            next_p, typecast, next_pPtrName, Lock, writeCntr)
#define CM_SMP_SLLQ_DELETE_NODE_WITH_LOCK_WRITE_CNTR(pSll, pPrev, pNode, \
            typecast, next_pPtrName, Lock, writeCntr)
#define CM_SMP_SLLQ_SCAN_WITH_LOCK_WRITE_CNTR(pSll, pNode, typecast, \
            next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, pArg1, \
            opaque_p)
#define CM_SMP_SLLQ_DYN_SCAN_WITH_LOCK_WRITE_CNTR(pSll, pPrev, pNode, next_p, \
            typecast, next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn)
#define CM_SMP_SLLQ_DYN_SCAN_FROM_NODE_WITH_LOCK_WRITE_CNTR(pSll, pPrev, \
            pNode, next_p, \
            typecast, next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn)

/* macros that use SPIN lock follows ... */
#define CM_SMP_SLLQ_APPEND_NODE_WITH_LOCK(pSll, pNode, typecast,\
            next_pPtrName, Lock)
#define CM_SMP_SLLQ_PREPEND_NODE_WITH_LOCK(pSll, pNode, typecast, \
            next_pPtrName, Lock)
#define CM_SMP_SLLQ_INSERT_NODE_WITH_LOCK(pSll, pPrev, pNode, next_p,\
            typecast, next_pPtrName, Lock)
#define CM_SMP_SLLQ_DELETE_NODE_WITH_LOCK(pSll, pPrev, pNode, typecast, \
            next_pPtrName, Lock)
#define CM_SMP_SLLQ_SCAN_WITH_LOCK(pSll, pNode, typecast, next_pPtrName, \
            Lock, CmpAndIncRefCnt_fn, pArg1, opaque_p)
#define CM_SMP_SLLQ_DYN_SCAN_WITH_LOCK(pSll, pPrev, pNode, next_p, typecast, \
            next_pPtrName, Lock, CmpAndIncRefCnt_fn, pArg1, \
            opaque_p, Delete_fn, bProceedLoop)
#define CM_SMP_SLLQ_DYN_SCAN_FROM_NODE_WITH_LOCK(pSll, pPrev, \
            pNode, next_p, typecast, \
            next_pPtrName, Lock, CmpAndIncRefCnt_fn, pArg1, \
            opaque_p, Delete_fn, bProceedLoop)
#endif /* CONFIG_SMP */

#endif /* _SLL_H */
