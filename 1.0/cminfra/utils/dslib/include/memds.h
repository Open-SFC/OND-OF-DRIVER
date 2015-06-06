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
 * File name: memds.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/23/2013
 * Description: This file contains the data types and function prototypes for 
 *              memory management APIs
 * 
 */

#ifndef _MEM_H
#define _MEM_H

/***************************************************************************/
/*                      MACRO DEFINITIONS                                  */
/***************************************************************************/
#define CM_MEM_DRAINED_OUT     (-2)
/* TOBE REMOVED */
#define UCMMemPoolSetKernelThreadVals cm_mempool_set_threshold_vals
#ifdef DSLIB_DBG
#define CM_MEM_POOL_ALLOW_CNT  1000
#endif /* DSLIB_DBG */
/* Memory pool data structure */
/* BitMap values */
#define MEMP_REFILL_PENDING   0x00000001 /* refilling is pending */
#define MEMP_INIT_TIME_REFILL 0x00000002 /* refilling is at init time */

typedef struct UCMMemPool_s
{
  UCMSllQNode_t     next_p;/* next pointer */
  UCMSllQNode_t     pRefill; /* pointer for refilling list */
  struct cm_mempool_stats MemPoolStats;/* pool statistics */
  union
  {
    cm_lock_t       MemPoolKLock;
    t_sem_id_t       MemPoolULock; /* Union of Locks for User/Kernel space */
  }MemPoolLock;
  unsigned char         *pucHead; /* next free mem block & head of memblocks sll */
  uint32_t          ulBitMap; /* bit map */
  uint32_t          ulRefCnt;
  unsigned char           bDel;
  unsigned char           bLock; /* TRUE, if lock is wanted for memory pool */
  unsigned char          aucPad[2]; /* padding */
} UCMMemPool_t;

/* This structure is used to link the memory blocks */
typedef struct UCMMemLinkNode_s
{
  unsigned char         *next_p; /* points to the next memory block */
} UCMMemLinkNode_t;

/***************************************************************************/
/*                      GLOBAL VARIABLE DEFINITIONS                        */
/***************************************************************************/
#ifdef DSLIB_DBG
extern UCMSllQ_t   MemPoolList_g; /* Linked list memory pools */
#else
extern uint32_t    ulMemLibPoolCnt_g;
#endif
/***************************************************************************/
/*                      FUNCTION PROTOTYPE DECLERATIONS                    */
/***************************************************************************/
unsigned char * cm_memalloc_and_chain_memchunk (UCMMemPool_t *pMemPool,
                                        uint32_t ulBlocks,
                                        uint32_t ulBlockSize,
                                        uint32_t *pulAllocCnt);
int32_t ucm_delete_mempool (UCMMemPool_t *pMemPool);
int32_t cm_mempool_derive_threshold_vals (UCMMemPool_t *pMemPool);
int32_t cm_mempool_wakeup_kernel_thread (UCMMemPool_t *pPool);
int32_t cm_mempool_lock_init (UCMMemPool_t *pMemPool);
int32_t cm_mempool_locktake (UCMMemPool_t *pMemPool);
int32_t cm_mempool_lock_give (UCMMemPool_t *pMemPool);
int32_t cm_mempool_lock_del  (UCMMemPool_t *pMemPool);

int32_t cm_mempool_get_stats (void * pPool, struct cm_mempool_stats * pStats);
/***************************************************************************/
/*                      MACRO DEFINITIONS                                  */
/***************************************************************************/
#ifdef DSLIB_DBG
#define CM_MEM_POOL_CNT        CM_SLLQ_COUNT (&MemPoolList_g)
#else
#define CM_MEM_POOL_CNT        ulMemLibPoolCnt_g
#endif

#define CM_MEM_POOL_INC_REFCNT(pMemPool) (pMemPool->ulRefCnt++)
#define CM_MEM_POOL_DEC_REFCNT(pMemPool) (pMemPool->ulRefCnt--)
#define CM_MEM_POOL_IS_REFCNT_ZERO(pMemPool) \
                    (pMemPool->ulRefCnt == 0) ? TRUE : FALSE
#define CM_MEM_POOL_SET_BDEL(pMemPool) pMemPool->bDel |= TRUE
#define CM_MEM_POOL_CHECK_BDEL(pMemPool) \
                    (pMemPool->bDel == TRUE) ? TRUE : FALSE
#define CM_MEM_POOL_CHECK_BDEL_NOREF(pMemPool) \
                    ((pMemPool->bDel == TRUE) && (pMemPool->ulRefCnt == 0)) \
                    ? TRUE : FALSE

#ifdef  __KERNEL__
#define DSLIB_TAKE_MEMPOOL_LOCK(pMemPool) \
{ \
  if (pMemPool->bLock == TRUE) \
  { \
    CM_LOCK_TAKE(pMemPool->MemPoolLock.MemPoolKLock); \
  } \
}

#define DSLIB_MEMPOOL_ENABLE_LOCAL_BH local_bh_enable()
#define DSLIB_MEMPOOL_DISABLE_LOCAL_BH local_bh_disable()

#define DSLIB_RELEASE_MEMPOOL_LOCK(pMemPool) \
{ \
  if (pMemPool->bLock == TRUE) \
  { \
    CM_LOCK_RELEASE(pMemPool->MemPoolLock.MemPoolKLock); \
  } \
}

#define MEMPOOL_KERN_PANIC(msg_p) \
{ \
  KernDumpStack(); \
  /*KernPanic(msg_p);*/ \
}

#define MEMP_KERN_PANIC(msg_p) \
{ \
  KernDumpStack(); \
  printf("\n\n ******* The block previously freed in the below path ********\n\n"); \
  print_symbol("  %s \n",*((uint32_t *)(pucBlock+pMemPool->MemPoolStats.ulBlockSize+12))); \
  print_symbol("  %s \n",*((uint32_t *)(pucBlock+pMemPool->MemPoolStats.ulBlockSize+8))); \
  print_symbol("  %s \n",*((uint32_t *)(pucBlock+pMemPool->MemPoolStats.ulBlockSize+4))); \
  printf("\n *********************************\n\n"); \
  /*KernPanic(msg_p); */ \
}
#define MEMP_GET_CURRENT_CPU(ucCoreNum)  \
          ucCoreNum = smp_processor_id();
#endif

#endif /* _MEM_H */
