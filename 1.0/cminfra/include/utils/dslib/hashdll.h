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
 * File name: hashdll.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: This file contains the APIs prototypes and data structure definitions 
 *              which are used by applications for their Hash Table with 
 *              doubly linked list operations.
 * 
 */

#ifndef _HASHDLL_H
#define _HASHDLL_H

#include "cmincld.h"
#include "cmdll.h"

#define UCMHashDllBucketList_t  UCMDllQ_t
#define UCMHashDllNode_t        UCMDllQNode_t

/***************************************************************************/
/*                  DATA STRUCTURE DEFINITIONS                             */
/***************************************************************************/
struct cm_hash_dll_table
{
  uint32_t                  ulBuckets;
  UCMHashDllBucketList_t    BucketList[1]; /* bucket list */
};

/***************************************************************************/
/*                      MACRO DEFINITIONS                                  */
/***************************************************************************/
#define CM_HASH_DLL_BUCKET_INIT(pHashTab, ulKey) \
            CM_DLLQ_INIT_LIST (&(pHashTab)->BucketList[(ulKey)])
#define CM_HASH_DLL_INIT_NODE  CM_DLLQ_INIT_NODE
#define CM_HASH_DLL_BUCKET_COUNT(pHashTab, ulKey) \
            CM_DLLQ_COUNT ((UCMDllQ_t *)(&(pHashTab)->BucketList[(ulKey)]))
#define CM_HASH_DLL_BUCKET_LAST(pHashTab, ulKey) \
            CM_DLLQ_LAST (&(pHashTab)->BucketList[(ulKey)])
#define CM_HASH_DLL_BUCKET_FIRST(pHashTab, ulKey) \
            CM_DLLQ_FIRST ((&(pHashTab)->BucketList[(ulKey)]))
#define CM_HASH_DLL_BUCKET_NEXT(pHashTab, ulKey, pNode) \
            CM_DLLQ_NEXT (&(pHashTab)->BucketList[(ulKey)], (pNode))
#define CM_HASH_DLL_BUCKET_PREV(pHashTab, ulKey, pNode) \
            CM_DLLQ_PREV (&(pHashTab)->BucketList[(ulKey)], (pNode))
#define CM_HASH_DLL_APPEND_NODE(pHashTab, ulKey, pNode) \
            CM_DLLQ_APPEND_NODE (&(pHashTab)->BucketList[(ulKey)], (pNode))
#define CM_HASH_DLL_PREPEND_NODE(pHashTab, ulKey, pNode) \
            CM_DLLQ_PREPEND_NODE (&(pHashTab)->BucketList[(ulKey)], (pNode))
#define CM_HASH_DLL_DELETE_NODE(pHashTab, ulKey, pNode) \
            CM_DLLQ_DELETE_NODE (&(pHashTab)->BucketList[(ulKey)], (pNode))
#define CM_HASH_DLL_INSERT_NODE(pHashTab, ulKey, pPrev, pNode) \
            CM_DLLQ_INSERT_NODE (&(pHashTab)->BucketList[(ulKey)],\
                               (pPrev), (pNode))
#define CM_HASH_DLL_BUCKET_SCAN(pHashTab, ulKey, pNode, typecast) \
            CM_DLLQ_SCAN (&(pHashTab)->BucketList[(ulKey)],(pNode), typecast)
#define CM_HASH_DLL_BUCKET_DYN_SCAN(pHashTab, ulKey, pNode, pTmp, typecast) \
            CM_DLLQ_DYN_SCAN (&(pHashTab)->BucketList[(ulKey)],\
                               (pNode), (pTmp), typecast)
#define CM_HASH_DLL_BUCKET_DYN_SCAN_FROM_NODE(pHashTab, ulKey, pNode, \
                                               pTmp, typecast) \
            CM_DLLQ_DYN_SCAN_FROM_NODE (&(pHashTab)->BucketList[(ulKey)],\
                                         (pNode), (pTmp), typecast)
#define CM_HASH_DLL_TABLE_SCAN(pHashTab, ulKey) \
            for ((ulKey = 0); (ulKey) < (pHashTab)->ulBuckets;\
                 (ulKey) = (ulKey) + 1)
#define CM_HASH_DLL_TABLE_SCAN_FROM_BUCKET(pHashTab, ulKey) \
            for (; (ulKey) < (pHashTab)->ulBuckets;\
                 (ulKey) = (ulKey) + 1)
#define CM_HASH_DLL_TABLE_TOTAL_COUNT(pHashTab, ulCount) \
        {\
          uint32_t  ulKey = 0;\
          ulCount = 0;\
          CM_HASH_DLL_TABLE_SCAN(pHashTab, ulKey) \
            ulCount += CM_HASH_DLL_BUCKET_COUNT(pHashTab, ulKey); \
        }

/* macros that use read-write lock with write counter follows ... */
#define CM_HASH_DLL_APPEND_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, \
            pNode, typecast, next_pPtrName, Lock, writeCntr) \
          CM_DLLQ_APPEND_NODE_WITH_LOCK_WRITE_CNTR(\
            &(pHashTab)->BucketList[(ulKey)], pNode, typecast,\
            next_pPtrName, Lock, writeCntr)
#define CM_HASH_DLL_PREPEND_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, \
            pNode, typecast, next_pPtrName, Lock, writeCntr) \
          CM_DLLQ_PREPEND_NODE_WITH_LOCK_WRITE_CNTR(\
            &(pHashTab)->BucketList[(ulKey)], pNode, typecast,\
            next_pPtrName, Lock, writeCntr)
#define CM_HASH_DLL_DELETE_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, \
            pNode, typecast, next_pPtrName, Lock, writeCntr) \
          CM_DLLQ_DELETE_NODE_WITH_LOCK_WRITE_CNTR(\
            &(pHashTab)->BucketList[(ulKey)], pNode, typecast,\
            next_pPtrName, Lock, writeCntr)
#define CM_HASH_DLL_INSERT_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, \
            pPrev, pNode, typecast, next_pPtrName, Lock, writeCntr) \
          CM_DLLQ_INSERT_NODE_WITH_LOCK_WRITE_CNTR(\
            &(pHashTab)->BucketList[(ulKey)], pPrev, pNode,\
            typecast, next_pPtrName, Lock, writeCntr)
#define CM_HASH_DLL_BUCKET_SCAN_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, pNode, \
            typecast, next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p) \
          CM_DLLQ_SCAN_WITH_LOCK_WRITE_CNTR(\
            &(pHashTab)->BucketList[(ulKey)], pNode, typecast, \
            next_pPtrName, (Lock), (writeCntr), (CmpAndIncRefCnt_fn), \
            (pArg1), (opaque_p))
#define CM_HASH_DLL_BUCKET_DYN_SCAN_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, \
            pNode, next_p, typecast, next_pPtrName, Lock, writeCntr, \
            CmpAndIncRefCnt_fn, pArg1, opaque_p, \
            UpdateRefCnt_fn, Delete_fn) \
          CM_DLLQ_DYN_SCAN_WITH_LOCK_WRITE_CNTR(\
            &(pHashTab)->BucketList[(ulKey)], pNode, next_p,\
            typecast, next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn)
#define CM_HASH_DLL_BUCKET_DYN_SCAN_FROM_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, \
            ulKey, \
            pNode, next_p, typecast, next_pPtrName, Lock, writeCntr, \
            CmpAndIncRefCnt_fn, pArg1, opaque_p, \
            UpdateRefCnt_fn, Delete_fn) \
          CM_DLLQ_DYN_SCAN_FROM_NODE_WITH_LOCK_WRITE_CNTR(\
            &(pHashTab)->BucketList[(ulKey)], pNode, next_p,\
            typecast, next_pPtrName, Lock, writeCntr, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn)

/* macros that use SPIN lock follows ... */
#define CM_HASH_DLL_APPEND_NODE_WITH_LOCK(pHashTab, ulKey, pNode, typecast,\
            next_pPtrName, Lock) \
          CM_DLLQ_APPEND_NODE_WITH_LOCK(&(pHashTab)->BucketList[(ulKey)], \
            pNode, typecast, next_pPtrName, Lock)
#define CM_HASH_DLL_PREPEND_NODE_WITH_LOCK(pHashTab, ulKey, pNode, typecast,\
            next_pPtrName, Lock) \
          CM_DLLQ_PREPEND_NODE_WITH_LOCK(&(pHashTab)->BucketList[(ulKey)], \
            pNode, typecast, next_pPtrName, Lock)
#define CM_HASH_DLL_DELETE_NODE_WITH_LOCK(pHashTab, ulKey, pNode, typecast,\
            next_pPtrName, Lock) \
          CM_DLLQ_DELETE_NODE_WITH_LOCK(&(pHashTab)->BucketList[(ulKey)], \
            pNode, typecast, next_pPtrName, Lock)
#define CM_HASH_DLL_INSERT_NODE_WITH_LOCK(pHashTab, ulKey, pPrev, pNode,\
            typecast, next_pPtrName, Lock) \
          CM_DLLQ_INSERT_NODE_WITH_LOCK(&(pHashTab)->BucketList[(ulKey)], \
            pPrev, pNode, typecast, next_pPtrName, Lock)
#define CM_HASH_DLL_BUCKET_SCAN_WITH_LOCK(pHashTab, ulKey, pNode, typecast, \
            next_pPtrName, Lock, CmpAndIncRefCnt_fn, pArg1, opaque_p) \
          CM_DLLQ_SCAN_WITH_LOCK(&(pHashTab)->BucketList[(ulKey)], pNode, \
            typecast, next_pPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p)
#define CM_HASH_DLL_BUCKET_DYN_SCAN_WITH_LOCK(pHashTab, ulKey, pNode, \
            next_p, typecast, next_pPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn) \
          CM_DLLQ_DYN_SCAN_WITH_LOCK(&(pHashTab)->BucketList[(ulKey)], \
            pNode, next_p, typecast, next_pPtrName, Lock, \
            CmpAndIncRefCnt_fn, pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn)
#define CM_HASH_DLL_BUCKET_DYN_SCAN_FROM_NODE_WITH_LOCK(pHashTab, \
            ulKey, pNode, \
            next_p, typecast, next_pPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn) \
          CM_DLLQ_DYN_SCAN_FROM_NODE_WITH_LOCK(\
            &(pHashTab)->BucketList[(ulKey)], \
            pNode, next_p, typecast, next_pPtrName, Lock, \
            CmpAndIncRefCnt_fn, pArg1, opaque_p, UpdateRefCnt_fn, Delete_fn)

#ifdef CONFIG_SMP
typedef UCMSMPDllQ_t              UCMSMPHashDllBucketList_t;
typedef UCMSMPDllQNode_t          UCMSMPHashDllNode_t;
typedef struct cm_hash_dll_table         UCMSMPHashDllTable_t;

#define CM_HASH_DLL_IS_NODE_IN_LIST(pNode) \
                (((pNode)->next_p != NULL) && \
                ((pNode)->pPrev != NULL) && \
                ((pNode)->next_p->pPrev == pNode) && \
                ((pNode)->pPrev->next_p == pNode))


#define CM_HASH_DLL_REMOVE_NODES_WITH_DEC_REFCNT_FN(pHashTab, ulKey1, ulKey2,\
                                                     pNode1, pNode2, \
                                                     DecRefCnt_fn, pArg1)\
 {\
        do{\
            CM_HASH_DLL_DELETE_NODE(pHashTab, \
            ulKey1, (UCMHashDllNode_t *)pNode1); \
            DecRefCnt_fn(pArg1); \
            CM_HASH_DLL_DELETE_NODE(pHashTab, \
            ulKey2, (UCMHashDllNode_t *)pNode2); \
            DecRefCnt_fn(pArg1); \
          }while(0); \
 }

#define CM_HASH_DLL_DELETE_NODE_WITH_LOCK_DEC_REFCNT_FN(pHashTab, ulKey, pNode,\
                                                         typecast, Lock, DecRefCnt_fn,\
                                                         pArg1)\
    {\
           CM_WRITE_LOCK_TAKE(Lock); \
            do{ \
                 CM_HASH_DLL_DELETE_NODE(pHashTab, \
                 ulKey, (UCMHashDllNode_t *)pNode); \
              }while(0); \
           CM_WRITE_LOCK_RELEASE(Lock); \
           DecRefCnt_fn(pArg1); \
    }


#define CM_HASH_DLL_DELETE_NODE_WITH_DEC_REFCNT_FN(pHashTab, ulKey, pNode,\
                                                    typecast, DecRefCnt_fn,\
                                                                     pArg1)\
    { \
            do{ \
                 CM_HASH_DLL_DELETE_NODE(pHashTab, \
                 ulKey, (UCMHashDllNode_t *)pNode); \
              }while(0); \
              DecRefCnt_fn(pArg1); \
    }

#define CM_HASH_DLL_ADD_NODE_WITH_LOCK_INC_REFCNT_FN(pHashTab, ulKey, pNode,\
                                                      typecast, Lock, IncRefCnt_fn,\
                                                      pArg1)\
        {\
           IncRefCnt_fn(pArg1);\
           CM_WRITE_LOCK_TAKE(Lock);\
           do{ \
                CM_HASH_DLL_APPEND_NODE(\
                pHashTab, ulKey, (UCMHashDllNode_t *)pNode);\
             }while(0); \
           CM_WRITE_LOCK_RELEASE(Lock);\
        }

#define CM_HASH_DLL_ADD_NODE_WITH_INC_REFCNT_FN(pHashTab, ulKey, pNode,\
                                                 typecast, IncRefCnt_fn, pArg1)\
        {\
           IncRefCnt_fn(pArg1);\
           do{ \
                CM_HASH_DLL_APPEND_NODE(\
                pHashTab, ulKey, (UCMHashDllNode_t *)pNode);\
             }while(0); \
        }


#define  CM_HASH_DLL_BUCKET_SCAN_WITH_LOCK_MATCHING_FN_WITH_NINE_ARGS(\
            pHashTab, ulKey, pNode, typecast, next_p, Lock, CmpAndIncRefCnt_fn,\
            pArg1, pArg2, pArg3, pArg4, pArg5, pArg6, pArg7, pArg8, pArg9, bMode)\
         {\
             CM_DLLQ_SCAN_WITH_LOCK_MATCHING_FN_WITH_NINE_ARGS(\
                        &(pHashTab)->BucketList[(ulKey)], pNode,\
                      typecast, next_p, Lock, CmpAndIncRefCnt_fn,\
                      pArg1, pArg2, pArg3, pArg4, pArg5, pArg6, pArg7, pArg8, pArg9, bMode)\
         }


#define CM_HASH_DLL_BUCKET_SCAN_WITH_LOCK_TAKE_AND_INC_REFCNT_FN(pHashTab, ulKey,\
           pNode, next_p, typecast, Lock, IncRefCnt_fn, pNodeMem, pNodeMemArg)\
         CM_DLLQ_SCAN_WITH_LOCK_TAKE_AND_INC_REFCNT_FN(&(pHashTab)->BucketList[(ulKey)],\
           pNode, next_p, typecast, Lock, IncRefCnt_fn, pNodeMem, pNodeMemArg)

#define CM_HASH_DLL_BUCKET_SCAN_END_WITH_LOCK_RELEASE(Lock)\
         CM_DLLQ_SCAN_END_WITH_LOCK_RELEASE(Lock)

#define CM_IS_NODE_IN_HASH_DLL_BUCKET(pNode, typecast)\
          CM_HASH_DLL_IS_NODE_IN_LIST((typecast)pNode)

/* macros that use read-write lock with write counter follows ... */
#define CM_SMP_HASH_DLL_APPEND_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, \
            pNode, typecast, next_pPtrName, Lock, writeCntr) \
          CM_HASH_DLL_APPEND_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, \
            pNode, typecast, next_pPtrName, Lock, writeCntr)
#define CM_SMP_HASH_DLL_PREPEND_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, \
            pNode, typecast, next_pPtrName, Lock, writeCntr) \
          CM_HASH_DLL_PREPEND_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, \
            pNode, typecast, next_pPtrName, Lock, writeCntr)
#define CM_SMP_HASH_DLL_DELETE_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, \
            pNode, typecast, next_pPtrName, Lock, writeCntr) \
          CM_HASH_DLL_DELETE_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, \
            pNode, typecast, next_pPtrName, Lock, writeCntr)
#define CM_SMP_HASH_DLL_INSERT_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, \
            pPrev, pNode, typecast, next_pPtrName, Lock) \
          CM_HASH_DLL_INSERT_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, \
            pPrev, pNode, typecast, next_pPtrName, Lock)
#define CM_SMP_HASH_DLL_BUCKET_SCAN_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, \
            pNode, typecast, next_pPtrName, Lock, writeCntr, \
            CmpAndIncRefCnt_fn, pArg1, opaque_p) \
          CM_HASH_DLL_BUCKET_SCAN_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, \
            pNode, typecast, next_pPtrName, Lock, writeCntr, \
            CmpAndIncRefCnt_fn, pArg1, opaque_p)
#define CM_SMP_HASH_DLL_BUCKET_DYN_SCAN_WITH_LOCK_WRITE_CNTR(pHashTab, \
            ulKey, pNode, next_p, typecast, next_pPtrName, Lock, \
            writeCntr, CmpAndIncRefCnt_fn, pArg1, opaque_p, \
            UpdateRefCnt_fn, Delete_fn) \
        CM_HASH_DLL_BUCKET_DYN_SCAN_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, \
            pNode, next_p, typecast, next_pPtrName, Lock, writeCntr, \
            CmpAndIncRefCnt_fn, pArg1, opaque_p, \
            UpdateRefCnt_fn, Delete_fn)
#define CM_SMP_HASH_DLL_BUCKET_DYN_SCAN_FROM_NODE_WITH_LOCK_WRITE_CNTR(\
            pHashTab, \
            ulKey, pNode, next_p, typecast, next_pPtrName, Lock, \
            writeCntr, CmpAndIncRefCnt_fn, pArg1, opaque_p, \
            UpdateRefCnt_fn, Delete_fn) \
        CM_HASH_DLL_BUCKET_DYN_SCAN_FROM_NODE_WITH_LOCK_WRITE_CNTR(\
            pHashTab, ulKey, \
            pNode, next_p, typecast, next_pPtrName, Lock, writeCntr, \
            CmpAndIncRefCnt_fn, pArg1, opaque_p, \
            UpdateRefCnt_fn, Delete_fn)
#define CM_SMP_HASH_DLL_TABLE_SCAN(pHashTab, ulKey) \
          CM_HASH_DLL_TABLE_SCAN(pHashTab, ulKey)
#define CM_SMP_HASH_DLL_TABLE_SCAN_FROM_BUCKET(pHashTab, ulKey) \
          CM_HASH_DLL_TABLE_SCAN_FROM_BUCKET(pHashTab, ulKey)

/* macros that use SPIN lock follows ... */
#define CM_SMP_HASH_DLL_APPEND_NODE_WITH_LOCK(pHashTab, ulKey, pNode, \
            typecast, next_pPtrName, Lock) \
          CM_HASH_DLL_APPEND_NODE_WITH_LOCK(pHashTab, ulKey, pNode, \
            typecast, next_pPtrName, Lock)
#define CM_SMP_HASH_DLL_PREPEND_NODE_WITH_LOCK(pHashTab, ulKey, pNode, \
            typecast, next_pPtrName, Lock) \
          CM_HASH_DLL_PREPEND_NODE_WITH_LOCK(pHashTab, ulKey, pNode, \
            typecast, next_pPtrName, Lock)
#define CM_SMP_HASH_DLL_DELETE_NODE_WITH_LOCK(pHashTab, ulKey, pNode, \
            typecast, next_pPtrName, Lock) \
          CM_HASH_DLL_DELETE_NODE_WITH_LOCK(pHashTab, ulKey, pNode, \
            typecast, next_pPtrName, Lock)
#define CM_SMP_HASH_DLL_INSERT_NODE_WITH_LOCK(pHashTab, ulKey, pPrev, pNode,\
            typecast, next_pPtrName, Lock) \
          CM_HASH_DLL_INSERT_NODE_WITH_LOCK(pHashTab, ulKey, pPrev, pNode,\
            typecast, next_pPtrName, Lock)
#define CM_SMP_HASH_DLL_BUCKET_SCAN_WITH_LOCK(pHashTab, ulKey, pNode, \
            typecast, next_pPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p) \
          CM_HASH_DLL_BUCKET_SCAN_WITH_LOCK(pHashTab, ulKey, pNode, typecast, \
            next_pPtrName, Lock, CmpAndIncRefCnt_fn, pArg1, opaque_p)
#define CM_SMP_HASH_DLL_BUCKET_DYN_SCAN_WITH_LOCK(pHashTab, ulKey,  \
            pNode, next_p, typecast, next_pPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, Delete_fn) \
          CM_HASH_DLL_BUCKET_DYN_SCAN_WITH_LOCK(pHashTab, ulKey, pNode, \
            next_p, typecast, next_pPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, Delete_fn)
#define CM_SMP_HASH_DLL_BUCKET_DYN_SCAN_FROM_NODE_WITH_LOCK(pHashTab, ulKey, \
            pNode, next_p, typecast, next_pPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, Delete_fn) \
          CM_HASH_DLL_BUCKET_DYN_SCAN_FROM_NODE_WITH_LOCK(pHashTab, \
            ulKey, pNode, \
            next_p, typecast, next_pPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, Delete_fn)
#else /* CONFIG_SMP */
typedef struct UCMSMPHashDllBucketList_s {} UCMSMPHashDllBucketList_t;
typedef struct UCMSMPHashDllNode_s {} UCMSMPHashDllNode_t;
typedef struct UCMSMPHashDllTable_s {}UCMSMPHashDllTable_t;

#define CM_SMP_HASH_DLL_BUCKET_INIT(pHashTab, ulKey)
#define CM_SMP_HASH_DLL_INIT_NODE
#define CM_SMP_HASH_DLL_BUCKET_COUNT(pHashTab, ulKey) 0
#define CM_SMP_HASH_DLL_BUCKET_LAST(pHashTab, ulKey) NULL
#define CM_SMP_HASH_DLL_BUCKET_FIRST(pHashTab, ulKey) NULL
#define CM_SMP_HASH_DLL_BUCKET_NEXT(pHashTab, ulKey, pNode) NULL
#define CM_SMP_HASH_DLL_BUCKET_PREV(pHashTab, ulKey, pNode) NULL
#define CM_SMP_HASH_DLL_APPEND_NODE(pHashTab, ulKey, pNode)
#define CM_SMP_HASH_DLL_PREPEND_NODE(pHashTab, ulKey, pNode)
#define CM_SMP_HASH_DLL_DELETE_NODE(pHashTab, ulKey, pNode)
#define CM_SMP_HASH_DLL_INSERT_NODE(pHashTab, ulKey, pPrev, pNode)
#define CM_SMP_HASH_DLL_BUCKET_SCAN(pHashTab, ulKey, pNode, typecast)
#define CM_SMP_HASH_DLL_BUCKET_DYN_SCAN(pHashTab, ulKey, pNode, \
                                        pTmp, typecast)
#define CM_SMP_HASH_DLL_TABLE_SCAN(pHashTab, ulKey)
#define CM_SMP_HASH_DLL_TABLE_SCAN_FROM_BUCKET(pHashTab, ulKey)
/* macros that use read-write lock with write counter follows ... */
#define CM_SMP_HASH_DLL_APPEND_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, \
            pNode, typecast, next_pPtrName, Lock, writeCntr)
#define CM_SMP_HASH_DLL_PREPEND_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, \
            pNode, typecast, next_pPtrName, Lock, writeCntr)
#define CM_SMP_HASH_DLL_DELETE_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, \
            pNode, typecast, next_pPtrName, Lock, writeCntr)
#define CM_SMP_HASH_DLL_INSERT_NODE_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, \
            pPrev, pNode, typecast, next_pPtrName, Lock, writeCntr)
#define CM_SMP_HASH_DLL_BUCKET_SCAN_WITH_LOCK_WRITE_CNTR(pHashTab, ulKey, \
            pNode, typecast, next_pPtrName, Lock, writeCntr, \
            CmpAndIncRefCnt_fn, pArg1, opaque_p)
#define CM_SMP_HASH_DLL_BUCKET_DYN_SCAN_WITH_LOCK_WRITE_CNTR(pHashTab, \
            ulKey, pNode, next_p, typecast, next_pPtrName, Lock, \
            writeCntr, CmpAndIncRefCnt_fn, pArg1, opaque_p, \
            UpdateRefCnt_fn, Delete_fn)
/* macros that use SPIN lock follows ... */
#define CM_SMP_HASH_DLL_APPEND_NODE_WITH_LOCK(pHashTab, ulKey, pNode, \
            typecast, next_pPtrName, Lock)
#define CM_SMP_HASH_DLL_PREPEND_NODE_WITH_LOCK(pHashTab, ulKey, pNode, \
            typecast, next_pPtrName, Lock)
#define CM_SMP_HASH_DLL_DELETE_NODE_WITH_LOCK(pHashTab, ulKey, pNode, \
            typecast, next_pPtrName, Lock)
#define CM_SMP_HASH_DLL_INSERT_NODE_WITH_LOCK(pHashTab, ulKey, pPrev, pNode,\
            typecast, next_pPtrName, Lock)
#define CM_SMP_HASH_DLL_BUCKET_SCAN_WITH_LOCK(pHashTab, ulKey, pNode, \
            typecast, next_pPtrName, Lock, CmpAndIncRefCnt_fn, pArg1,opaque_p)
#define CM_SMP_HASH_DLL_BUCKET_DYN_SCAN_WITH_LOCK(pHashTab, ulKey, \
            pNode, next_p, typecast, next_pPtrName, Lock, CmpAndIncRefCnt_fn, \
            pArg1, opaque_p, Delete_fn)
#endif /* CONFIG_SMP */

/***************************************************************************/
/*                      FUNCTION PROTOTYPE DECLERATIONS                    */
/***************************************************************************/
struct cm_hash_dll_table  *cm_hash_dll_table_create (uint32_t ulBuckets);
void              cm_hash_dll_table_delete (struct cm_hash_dll_table * pHashTab);

#endif /* _HASHDLL_H */
