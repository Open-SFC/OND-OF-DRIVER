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
 * File name: ucmhashsll.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: This file contains the APIs prototypes and data structure definitions
 *              which are used by applications for their Hash Table with
 *              singly linked list operations.
 * 
 */

#ifndef _HASH_H
#define _HASH_H

#include "cmincld.h"

#define UCMHashBucketList_t UCMSllQ_t
#define UCMHashNode_t       UCMSllQNode_t

/***************************************************************************/
/*                  DATA STRUCTURE DEFINITIONS                             */
/***************************************************************************/
struct cm_hash_sll_table 
{
  uint32_t              ulBuckets;
  UCMHashBucketList_t   BucketList[1]; /* bucket list */
};

/***************************************************************************/
/*                      MACRO DEFINITIONS                                  */
/***************************************************************************/
#define CM_HASH_BUCKET_INIT(pHashTab, ulKey) \
            CM_SLLQ_INIT_LIST (&(pHashTab)->BucketList[(ulKey)])
#define CM_HASH_INIT_NODE CM_SLLQ_INIT_NODE
#define CM_HASH_BUCKET_COUNT(pHashTab, ulKey) \
            CM_SLLQ_COUNT (&(pHashTab)->BucketList[(ulKey)])
#define CM_HASH_BUCKET_LAST(pHashTab, ulKey) \
            CM_SLLQ_LAST (&(pHashTab)->BucketList[(ulKey)])
#define CM_HASH_BUCKET_FIRST(pHashTab, ulKey) \
            CM_SLLQ_FIRST (&(pHashTab)->BucketList[(ulKey)])
#define CM_HASH_BUCKET_NEXT(pHashTab, ulKey, pNode) \
            CM_SLLQ_NEXT (&(pHashTab)->BucketList[(ulKey)], (pNode))
#define CM_HASH_APPEND_NODE(pHashTab, ulKey, pNode) \
            CM_SLLQ_APPEND_NODE (&(pHashTab)->BucketList[(ulKey)], (pNode))
#define CM_HASH_PREPEND_NODE(pHashTab, ulKey, pNode) \
            CM_SLLQ_PREPEND_NODE (&(pHashTab)->BucketList[(ulKey)], (pNode))
#define CM_HASH_DELETE_NODE(pHashTab, ulKey, pNode) \
            CM_SLLQ_DELETE_NODE (&(pHashTab)->BucketList[(ulKey)], (pNode))
#define CM_HASH_INSERT_NODE(pHashTab, ulKey, pPrev, pNode, next_p) \
            CM_SLLQ_INSERT_NODE (&(pHashTab)->BucketList[(ulKey)],\
                                  (pPrev), (pNode), (next_p))
#define CM_HASH_BUCKET_SCAN(pHashTab, ulKey, pNode, typecast) \
            CM_SLLQ_SCAN (&(pHashTab)->BucketList[(ulKey)],(pNode), typecast)
#define CM_HASH_BUCKET_DYN_SCAN(pHashTab, ulKey, pNode, pTmp, typecast) \
            CM_SLLQ_DYN_SCAN (&(pHashTab)->BucketList[(ulKey)],\
                               (pNode), (pTmp),typecast)
#define CM_HASH_BUCKET_DYN_SCAN_FROM_NODE(pHashTab, ulKey, pNode, \
                                           pTmp, typecast) \
            CM_SLLQ_DYN_SCAN_FROM_NODE (&(pHashTab)->BucketList[(ulKey)],\
                                         (pNode), (pTmp),typecast)
#define CM_HASH_TABLE_SCAN(pHashTab, ulKey) \
            for ((ulKey = 0); (ulKey) < (pHashTab)->ulBuckets;\
                 (ulKey) = (ulKey) + 1)
#define CM_HASH_TABLE_SCAN_FROM_BUCKET(pHashTab, ulKey) \
            for (; (ulKey) < (pHashTab)->ulBuckets;\
                 (ulKey) = (ulKey) + 1)
#define CM_HASH_TABLE_TOTAL_COUNT(pHashTab, ulCount) \
        {\
          uint32_t ulKey = 0;\
          ulCount = 0;\
          CM_HASH_TABLE_SCAN(pHashTab, ulKey)\
            ulCount += CM_HASH_BUCKET_COUNT(pHashTab, ulKey); \
        }

/* macros that use read-write lock with write counter follows ... */
#define CM_HASH_APPEND_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, pNode,\
            typecast, next_pPtrName, Lock, writeCntr) \
          CM_SLLQ_APPEND_NODE_WITH_LOCK_WRITE_CNTR(\
            &(pHashTab)->BucketList[(ulKey)], (pNode), typecast,\
            (next_pPtrName), (Lock), (writeCntr))
#define CM_HASH_PREPEND_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, pNode,\
            typecast, next_pPtrName, Lock, writeCntr) \
          CM_SLLQ_PREPEND_NODE_WITH_LOCK_WRITE_CNTR(\
              &(pHashTab)->BucketList[(ulKey)], (pNode), typecast, \
            (next_pPtrName), (Lock), (writeCntr))
#define CM_HASH_DELETE_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, pPrev,\
            pNode, typecast, next_pPtrName, Lock, writeCntr) \
          CM_SLLQ_DELETE_NODE_WITH_LOCK_WRITE_CNTR(\
              &(pHashTab)->BucketList[(ulKey)], (pPrev), (pNode), \
            typecast, (next_pPtrName), (Lock), (writeCntr))
#define CM_HASH_INSERT_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, pPrev, \
            pNode, next_p,typecast, next_pPtrName, Lock, writeCntr) \
          CM_SLLQ_INSERT_NODE_WITH_LOCK_WRITE_CNTR(\
              &(pHashTab)->BucketList[(ulKey)], (pPrev), (pNode), (next_p),\
            typecast, (next_pPtrName), (Lock), (writeCntr))
#define CM_HASH_BUCKET_SCAN_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, pNode, \
            typecast, next_pPtrName,Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p) \
          CM_SLLQ_SCAN_WITH_LOCK_WRITE_CNTR(\
              &(pHashTab)->BucketList[(ulKey)], pNode, typecast, \
            next_pPtrName, (Lock), (writeCntr), (CmpAndIncRefCnt_fn), \
            (pArg1), (opaque_p))
#define CM_HASH_BUCKET_DYN_SCAN_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, pPrev,\
            pNode, next_p, typecast, next_pPtrName, Lock, writeCntr,\
            CmpAndIncRefCnt_fn, pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn) \
          CM_SLLQ_DYN_SCAN_WITH_LOCK_WRITE_CNTR(\
              &(pHashTab)->BucketList[(ulKey)], (pPrev), (pNode), (next_p), \
            typecast, (next_pPtrName), (Lock), (writeCntr), \
            (CmpAndIncRefCnt_fn), (pArg1), (opaque_p), \
            (UpdateRefCnt_fn), (Delete_fn))
#define CM_HASH_BUCKET_DYN_SCAN_FROM_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, \
            ulKey, pPrev,\
            pNode, next_p, typecast, next_pPtrName, Lock, writeCntr,\
            CmpAndIncRefCnt_fn, pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn) \
          CM_SLLQ_DYN_SCAN_FROM_NODE_WITH_LOCK_WRITE_CNTR(\
              &(pHashTab)->BucketList[(ulKey)], (pPrev), (pNode), (next_p), \
            typecast, (next_pPtrName), (Lock), (writeCntr), \
            (CmpAndIncRefCnt_fn), (pArg1), (opaque_p), \
            (UpdateRefCnt_fn), (Delete_fn))

/* macros that use SPIN lock follows ... */
#define CM_HASH_APPEND_NODE_WITH_LOCK(pHashTab, ulKey, pNode,\
            typecast, next_pPtrName, Lock) \
          CM_SLLQ_APPEND_NODE_WITH_LOCK(\
            &(pHashTab)->BucketList[(ulKey)], (pNode), typecast,\
            (next_pPtrName), (Lock))
#define CM_HASH_PREPEND_NODE_WITH_LOCK(pHashTab, ulKey, pNode,\
            typecast, next_pPtrName, Lock) \
          CM_SLLQ_PREPEND_NODE_WITH_LOCK(\
            &(pHashTab)->BucketList[(ulKey)], (pNode), typecast,\
            (next_pPtrName), (Lock))
#define CM_HASH_DELETE_NODE_WITH_LOCK(pHashTab, ulKey, pPrev,\
            pNode, typecast, next_pPtrName, Lock) \
          CM_SLLQ_DELETE_NODE_WITH_LOCK(\
              &(pHashTab)->BucketList[(ulKey)], (pPrev), (pNode), \
            typecast, (next_pPtrName), (Lock))
#define CM_HASH_INSERT_NODE_WITH_LOCK(pHashTab, ulKey, pPrev, pNode, next_p,\
            typecast, next_pPtrName, Lock) \
          CM_SLLQ_INSERT_NODE_WITH_LOCK(\
              &(pHashTab)->BucketList[(ulKey)], (pPrev), (pNode), (next_p),\
            typecast, (next_pPtrName), (Lock))
#define CM_HASH_BUCKET_SCAN_WITH_LOCK(pHashTab, ulKey, pNode, \
            typecast, next_pPtrName,Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p) \
          CM_SLLQ_SCAN_WITH_LOCK(\
              &(pHashTab)->BucketList[(ulKey)], (pNode), typecast, \
            (next_pPtrName), (Lock), (CmpAndIncRefCnt_fn), \
            (pArg1), (opaque_p))
#define CM_HASH_BUCKET_DYN_SCAN_WITH_LOCK(pHashTab, ulKey, pPrev, pNode, \
            next_p, typecast, next_pPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn) \
          CM_SLLQ_DYN_SCAN_WITH_LOCK(&(pHashTab)->BucketList[(ulKey)], \
            (pPrev), (pNode), (next_p), typecast, \
            (next_pPtrName), (Lock), (CmpAndIncRefCnt_fn), (pArg1), \
            (opaque_p), (UpdateRefCnt_fn), (Delete_fn))
#define CM_HASH_BUCKET_DYN_SCAN_FROM_NODE_WITH_LOCK(pHashTab, ulKey, \
            pPrev, pNode, \
            next_p, typecast, next_pPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn) \
          CM_SLLQ_DYN_SCAN_FROM_NODE_WITH_LOCK(\
            &(pHashTab)->BucketList[(ulKey)], \
            (pPrev), (pNode), (next_p), typecast, \
            (next_pPtrName), (Lock), (CmpAndIncRefCnt_fn), (pArg1), \
            (opaque_p), (UpdateRefCnt_fn), (Delete_fn))

#ifdef CONFIG_SMP
typedef UCMSMPSllQ_t            UCMSMPHashBucketList_t;
typedef UCMSMPSllQNode_t        UCMSMPHashNode_t;
typedef struct cm_hash_table          UCMSMPHashTable_t;

#define CM_SMP_HASH_BUCKET_INIT(pHashTab, ulKey) \
          CM_HASH_BUCKET_INIT(pHashTab, ulKey)
#define CM_SMP_HASH_INIT_NODE CM_HASH_INIT_NODE
#define CM_SMP_HASH_BUCKET_COUNT(pHashTab, ulKey) \
          CM_HASH_BUCKET_COUNT(pHashTab, ulKey)
#define CM_SMP_HASH_BUCKET_LAST(pHashTab, ulKey) \
          CM_HASH_BUCKET_LAST(pHashTab, ulKey)
#define CM_SMP_HASH_BUCKET_FIRST(pHashTab, ulKey) \
          CM_HASH_BUCKET_FIRST(pHashTab, ulKey)
#define CM_SMP_HASH_BUCKET_NEXT(pHashTab, ulKey, pNode) \
          CM_HASH_BUCKET_NEXT(pHashTab, ulKey, pNode)
#define CM_SMP_HASH_APPEND_NODE(pHashTab, ulKey, pNode) \
          CM_HASH_APPEND_NODE(pHashTab, ulKey, pNode)
#define CM_SMP_HASH_PREPEND_NODE(pHashTab, ulKey, pNode) \
          CM_HASH_PREPEND_NODE(pHashTab, ulKey, pNode)
#define CM_SMP_HASH_DELETE_NODE(pHashTab, ulKey, pNode) \
          CM_HASH_DELETE_NODE(pHashTab, ulKey, pNode)
#define CM_SMP_HASH_INSERT_NODE(pHashTab, ulKey, pPrev, pNode, next_p) \
          CM_HASH_INSERT_NODE(pHashTab, ulKey, pPrev, pNode, next_p)
#define CM_SMP_HASH_BUCKET_SCAN(pHashTab, ulKey, pNode, typecast) \
          CM_HASH_BUCKET_SCAN(pHashTab, ulKey, pNode, typecast)
#define CM_SMP_HASH_BUCKET_DYN_SCAN(pHashTab, ulKey, pNode, pTmp, typecast) \
          CM_HASH_BUCKET_DYN_SCAN(pHashTab, ulKey, pNode, pTmp, typecast)
#define CM_SMP_HASH_BUCKET_DYN_SCAN_FROM_NODE(pHashTab, ulKey, \
                                               pNode, pTmp, typecast) \
          CM_HASH_BUCKET_DYN_SCAN_FROM_NODE(pHashTab, ulKey, \
                                             pNode, pTmp, typecast)
#define CM_SMP_HASH_TABLE_SCAN(pHashTab, ulKey) \
          CM_HASH_TABLE_SCAN(pHashTab, ulKey)
#define CM_SMP_HASH_TABLE_SCAN_FROM_BUCKET(pHashTab, ulKey) \
          CM_HASH_TABLE_SCAN_FROM_BUCKET(pHashTab, ulKey)

/* macros that use read-write lock with write counter follows ... */
#define CM_SMP_HASH_APPEND_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, pNode,\
            typecast, next_pPtrName, Lock, writeCntr) \
          CM_HASH_APPEND_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, pNode,\
            typecast, next_pPtrName, Lock, writeCntr)
#define CM_SMP_HASH_PREPEND_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, pNode,\
            typecast, next_pPtrName, Lock, writeCntr) \
          CM_HASH_PREPEND_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, pNode,\
            typecast, next_pPtrName, Lock, writeCntr)
#define CM_SMP_HASH_DELETE_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, pPrev,\
            pNode, typecast, next_pPtrName, Lock, writeCntr) \
          CM_HASH_DELETE_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, pPrev,\
            pNode, typecast, next_pPtrName, Lock, writeCntr)
#define CM_SMP_HASH_INSERT_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, pPrev, \
            pNode, next_p,typecast, next_pPtrName, Lock, writeCntr) \
          CM_HASH_INSERT_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, pPrev, \
            pNode, next_p,typecast, next_pPtrName, Lock, writeCntr)
#define CM_SMP_HASH_BUCKET_SCAN_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, pNode, \
            typecast, next_pPtrName,Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p) \
          CM_HASH_BUCKET_SCAN_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, pNode, \
            typecast, next_pPtrName,Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p)
#define CM_SMP_HASH_BUCKET_DYN_SCAN_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, \
            pPrev, pNode, next_p, typecast, next_pPtrName, Lock, writeCntr,\
            CmpAndIncRefCnt_fn, pArg1, opaque_p, UpdateRefCnt_fn, \
            Delete_fn) \
          CM_HASH_BUCKET_DYN_SCAN_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, \
            pPrev, pNode, next_p, typecast, next_pPtrName, Lock, writeCntr,\
            CmpAndIncRefCnt_fn, pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn)
#define CM_SMP_HASH_BUCKET_DYN_SCAN_FROM_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, \
            ulKey, \
            pPrev, pNode, next_p, typecast, next_pPtrName, Lock, writeCntr,\
            CmpAndIncRefCnt_fn, pArg1, opaque_p, UpdateRefCnt_fn, \
            Delete_fn) \
          CM_HASH_BUCKET_DYN_SCAN_FROM_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, \
            ulKey, \
            pPrev, pNode, next_p, typecast, next_pPtrName, Lock, writeCntr,\
            CmpAndIncRefCnt_fn, pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn)

/* macros that use SPIN lock follows ... */
#define CM_SMP_HASH_APPEND_NODE_WITH_LOCK(pHashTab, ulKey, pNode,\
            typecast, next_pPtrName, Lock) \
          CM_HASH_APPEND_NODE_WITH_LOCK(pHashTab, ulKey, pNode,\
            typecast, next_pPtrName, Lock)
#define CM_SMP_HASH_PREPEND_NODE_WITH_LOCK(pHashTab, ulKey, pNode,\
            typecast, next_pPtrName, Lock) \
          CM_HASH_PREPEND_NODE_WITH_LOCK(pHashTab, ulKey, pNode,\
            typecast, next_pPtrName, Lock)
#define CM_SMP_HASH_DELETE_NODE_WITH_LOCK(pHashTab, ulKey, pPrev,\
            pNode, typecast, next_pPtrName, Lock) \
          CM_HASH_DELETE_NODE_WITH_LOCK(pHashTab, ulKey, pPrev,\
            pNode, typecast, next_pPtrName, Lock)
#define CM_SMP_HASH_INSERT_NODE_WITH_LOCK(pHashTab, ulKey, pPrev, pNode, \
            next_p, typecast, next_pPtrName, Lock) \
          CM_HASH_INSERT_NODE_WITH_LOCK(pHashTab, ulKey, pPrev, pNode, next_p,\
            typecast, next_pPtrName, Lock)
#define CM_SMP_HASH_BUCKET_SCAN_WITH_LOCK(pHashTab, ulKey, pNode, \
            typecast, next_pPtrName,Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p) \
          CM_HASH_BUCKET_SCAN_WITH_LOCK(pHashTab, ulKey, pNode, \
            typecast, next_pPtrName,Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p)
#define CM_SMP_HASH_BUCKET_DYN_SCAN_WITH_LOCK(pHashTab, ulKey, pPrev, pNode, \
            next_p, typecast, next_pPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn) \
          CM_HASH_BUCKET_DYN_SCAN_WITH_LOCK(pHashTab, ulKey, pPrev, pNode, \
            next_p, typecast, next_pPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn)
#define CM_SMP_HASH_BUCKET_DYN_SCAN_FROM_NODE_WITH_LOCK(pHashTab, \
            ulKey, pPrev, pNode, \
            next_p, typecast, next_pPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn) \
          CM_HASH_BUCKET_DYN_SCAN_FROM_NODE_WITH_LOCK(pHashTab, \
            ulKey, pPrev, pNode, \
            next_p, typecast, next_pPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn)

#else /* CONFIG_SMP */
typedef struct UCMSMPHashBucketList_s {} UCMSMPHashBucketList_t;
typedef struct UCMSMPHashNode_s {} UCMSMPHashNode_t;
typedef struct UCMSMPHashTable_s {}UCMSMPHashTable_t;

#define CM_SMP_HASH_BUCKET_INIT(pHashTab, ulKey) 
#define CM_SMP_HASH_INIT_NODE CM_SLLQ_INIT_NODE
#define CM_SMP_HASH_BUCKET_COUNT(pHashTab, ulKey) 
#define CM_SMP_HASH_BUCKET_LAST(pHashTab, ulKey) 
#define CM_SMP_HASH_BUCKET_FIRST(pHashTab, ulKey) 
#define CM_SMP_HASH_BUCKET_NEXT(pHashTab, ulKey, pNode) 
#define CM_SMP_HASH_APPEND_NODE(pHashTab, ulKey, pNode) 
#define CM_SMP_HASH_PREPEND_NODE(pHashTab, ulKey, pNode) 
#define CM_SMP_HASH_DELETE_NODE(pHashTab, ulKey, pNode) 
#define CM_SMP_HASH_INSERT_NODE(pHashTab, ulKey, pPrev, pNode, next_p) 
#define CM_SMP_HASH_BUCKET_SCAN(pHashTab, ulKey, pNode, typecast) 
#define CM_SMP_HASH_BUCKET_DYN_SCAN(pHashTab, ulKey, pNode, pTmp, typecast) 
#define CM_SMP_HASH_TABLE_SCAN(pHashTab, ulKey) 
#define CM_SMP_HASH_TABLE_SCAN_FROM_BUCKET(pHashTab, ulKey) 

/* macros that use read-write lock with write counter follows ... */
#define CM_SMP_HASH_APPEND_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, pNode,\
            typecast, next_pPtrName, Lock, writeCntr) 
#define CM_SMP_HASH_PREPEND_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, pNode,\
            typecast, next_pPtrName, Lock, writeCntr) 
#define CM_SMP_HASH_DELETE_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, pPrev,\
            pNode, typecast, next_pPtrName, Lock, writeCntr) 
#define CM_SMP_HASH_INSERT_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, pPrev, \
            pNode, next_p,typecast, next_pPtrName, Lock, writeCntr) 
#define CM_SMP_HASH_BUCKET_SCAN_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, pNode, \
            typecast, next_pPtrName,Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p) 
#define CM_SMP_HASH_BUCKET_DYN_SCAN_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, \
            pPrev,pNode, next_p, typecast, next_pPtrName, Lock, writeCntr,\
            CmpAndIncRefCnt_fn, pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn) 

/* macros that use SPIN lock follows ... */
#define CM_SMP_HASH_APPEND_NODE_WITH_LOCK(pHashTab, ulKey, pNode,\
            typecast, next_pPtrName, Lock) 
#define CM_SMP_HASH_PREPEND_NODE_WITH_LOCK(pHashTab, ulKey, pNode,\
            typecast, next_pPtrName, Lock) 
#define CM_SMP_HASH_DELETE_NODE_WITH_LOCK(pHashTab, ulKey, pPrev,\
            pNode, typecast, next_pPtrName, Lock) 
#define CM_SMP_HASH_INSERT_NODE_WITH_LOCK(pHashTab, ulKey, pPrev, pNode, \
            next_p, typecast, next_pPtrName, Lock) 
#define CM_SMP_HASH_BUCKET_SCAN_WITH_LOCK(pHashTab, ulKey, pNode, \
            typecast, next_pPtrName,Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p) 
#define CM_SMP_HASH_BUCKET_DYN_SCAN_WITH_LOCK(pHashTab, ulKey, pPrev, pNode, \
            next_p, typecast, next_pPtrName, Lock, CmpAndIncRefCnt_fn,\
            pArg1, opaque_p, UpdateRefCnt_fn) 
#endif /* CONFIG_SMP */

/***************************************************************************/
/*                      FUNCTION PROTOTYPE DECLERATIONS                    */
/***************************************************************************/
struct cm_hash_sll_table     *cm_hash_table_create (uint32_t ulBuckets);
void              cm_hash_table_delete (struct cm_hash_sll_table * pHashTab);

#endif /* _HASH_H */
