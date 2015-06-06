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
 * File name: mempool.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/23/2013
 * Description: This file contains the APIs which are used by applications for their
 *              memory management.
 * 
 */

#ifndef _MEMPOOL_C
#define _MEMPOOL_C


#include "cmincld.h"
#ifdef __KERNEL__
#include <linux/completion.h>
#include <linux/cpumask.h>
#include <linux/smp.h>
#include "kernwrp.h"
#endif
#include "memds.h"

/*
#ifdef __KERNEL__
#include "kernwrp.h"
#endif  __KERNEL__ 
*/

#define DSLIB_ID
#define TRACE_SEVERE

/*
 * DSLIB_MEMCHK is enabled to check the double frees and
 * some nodes not freed before calling the cm_delete_mempool
 */
#define DSLIB_MEMCHK 

/* This definition is used to find static pool memory corruption
 * or any double free.
 * 4 bytes extra memory is allocated to the application date size,
 * and certain manipulation is done on those extra bytes to find
 * any corruption or double free
 */
#ifdef DSLIB_MEMCHK
#define MEMP_STATIC_CHK_BYTES    4
#define MEMP_STATIC_MAGICNUM     0x9e3779b9
#endif

#ifdef DSLIB_DBG
UCMSllQ_t   MemPoolList_g; /* Linked list memory pools */
#else
uint32_t    ulMemLibPoolCnt_g;
#endif

/**************************************************************************/
/*                 FUNCTION PROTOTYPE DEFINITIONS                         */
/**************************************************************************/
 int32_t cm_mempool_delete_from_refill_list (UCMMemPool_t *pMemPool);

#ifdef DSLIB_DBG
/***************************************************************************
 * Function Name : cm_mempool_lib_init
 * Description   : Used to initialize global linked list
 * Input         : None.
 * Output        : None.
 * Return value  : OF_SUCCESS/OF_FAILURE
 ***************************************************************************/
int32_t cm_mempool_lib_init (void)
{
  CM_SLLQ_INIT_LIST (&MemPoolList_g);
  return OF_SUCCESS;
}
#endif

/***************************************************************************
 * Function Name : cm_create_mempool
 * Description   : Used to create memory pool
 * Input         : ulBlocks - number blocks needed in static mem pool
 *                 ulBlockSize - block size
 *                 ulMaxBlocks - max allowed number of blocks that
 *                 can be allocated. (total of heap and static)
 *                 bSemWanted - semaphore wanted (TRUE/FALSE)
 * Output        : None.
 * Return value  : OF_SUCCESS/OF_FAILURE
 ***************************************************************************/
void* cm_create_mempool (uint32_t ulBlocks, uint32_t ulBlockSize,
                          uint32_t ulMaxBlocks,
                          unsigned char  bSemWanted)
{
  UCMMemPool_t     *pMemPool = NULL;
  uint32_t          ulHeapAlwdBlocks = ulMaxBlocks - ulBlocks;
  uint32_t          ulAllocCnt = 0;

  /* sizeof the block should be atleaset of 4bytes. This is to maintain
   * the linked list of free blocks.
   */
  if (ulBlockSize < 4)
  {
    printf(
      "cm_create_mempool: Allocation FAILED !!!: "
      "BLOCKSIZE should be atleast 4bytes\n\r");
    return (NULL);
  }

#ifdef DSLIB_DBG
  /* If the pool count reaches to the maximum allowed number pools,
   * then return
   */
  CM_MEM_POOL_CNT++;
  if (CM_MEM_POOL_CNT > CM_MEM_POOL_ALLOW_CNT)
  {
    CM_MEM_POOL_CNT--;
    return (NULL);
  }
#endif /* DSLIB_DBG */

  pMemPool = (UCMMemPool_t *) of_calloc (1, sizeof(UCMMemPool_t));
  if (pMemPool == NULL)
  {
    printf(
      "cm_create_mempool: Allocation FAILED !!!: "
      "Unable to create mem pool structure\n\r");
    return (NULL);
  }

#ifdef DSLIB_DBG
  CM_SLLQ_APPEND_NODE (&MemPoolList_g, (UCMSllQNode_t *) pMemPool);
#endif

  pMemPool->MemPoolStats.ulBlocksCnt = ulBlocks;
  pMemPool->MemPoolStats.ulBlockSize = ulBlockSize;
  /* derive threshold values for kernel space */
  cm_mempool_derive_threshold_vals (pMemPool);
  /* update the no.of blocks with maximum threshold,
   * in user/vx space, it is the passed argument.
   * in kernel space, it is the maximum threshold value
   */
  ulBlocks = pMemPool->MemPoolStats.ulMaxThreshold;
  pMemPool->MemPoolStats.ulHeapAlwdBlocks = ulHeapAlwdBlocks;
  pMemPool->MemPoolStats.ulHeapFreeCnt = ulHeapAlwdBlocks;

  pMemPool->pucHead = 
    cm_memalloc_and_chain_memchunk (pMemPool, ulBlocks, ulBlockSize, &ulAllocCnt);
  if (pMemPool->pucHead == NULL)
  {
    printf(
        "cm_create_mempool: Allocation FAILED !!!: "
        "Unable to Allocate Memory Chunk\n\r");
    cm_delete_mempool (pMemPool);
    return (NULL);
  }
  /* Update the counters with real ulAllocCnt */
  pMemPool->MemPoolStats.ulFreeCnt += ulAllocCnt;
  pMemPool->MemPoolStats.ulStaticAllocCnt += ulAllocCnt;

/* Get the semaphore here, if application wanted */
  if (bSemWanted == TRUE)
  {
    if (cm_mempool_lock_init (pMemPool) != OF_SUCCESS)
    {
      printf(
        "cm_create_mempool: Allocation FAILED !!!: "
        "Unable to Initialize Mutex Semaphore\n\r");
      cm_delete_mempool (pMemPool);
      return (NULL);
    }
  }
  return (pMemPool);
}

/***************************************************************************
 * Function Name : cm_delete_mempool
 * Description   : Deletes a memory pool data structure from
 *                 global linked list and frees the memory
 * Input         : pMemPool - memory pool pointer
 * Output        : None
 * Return value  : OF_SUCCESS
 ***************************************************************************/
int32_t cm_delete_mempool (void *pPool)
{
  UCMMemPool_t *pMemPool = NULL;
  int32_t       return_value = OF_FAILURE;

  if (pPool == NULL)
  {
    return OF_FAILURE;
  }

  pMemPool = (UCMMemPool_t *) pPool;
#ifdef DSLIB_DBG
  CM_SLLQ_DELETE_NODE (&MemPoolList_g, (UCMSllQNode_t *) pMemPool);
#endif
  return_value = cm_mempool_delete_from_refill_list (pMemPool);
  if (return_value == OF_SUCCESS)
  {
    return_value = ucm_delete_mempool (pMemPool);
    return (return_value);
  }
  return (OF_SUCCESS);
}

/***************************************************************************
 * Function Name : cm_delete_mempool
 * Description   : Deletes a memory pool data structure from
 *                 global linked list and frees the memory
 * Input         : pMemPool - memory pool pointer
 * Output        : None
 * Return value  : OF_SUCCESS
 ***************************************************************************/
int32_t ucm_delete_mempool (UCMMemPool_t *pMemPool)
{
  unsigned char     *pucHead = NULL, *pTmp = NULL;

  /* assumption: when delete mem pool is called, all the nodes would have been
   * released by calling cm_mempool_release_block, hence release the remaining
   * free blocks using pucHead
   */
  pucHead = pMemPool->pucHead;
  while (pucHead)
  {
    pTmp = ((UCMMemLinkNode_t *)pucHead)->next_p;
    of_free (pucHead);
    pucHead = pTmp;
  }
  pMemPool->pucHead = NULL;
  
  if (pMemPool->MemPoolStats.ulFreeCnt != 
      pMemPool->MemPoolStats.ulStaticAllocCnt)
  {
    printf("%s, *********** LEAK INFO BEGIN ************\n\r", __FUNCTION__);
    printf("%s, Leak in static blocks(%u), %s",
            __FUNCTION__, 
           (pMemPool->MemPoolStats.ulStaticAllocCnt - 
             pMemPool->MemPoolStats.ulFreeCnt),
            "possibly appln didnt release the mem allocated\r\n");
    printf("%s, *********** LEAK INFO END   ************\n\r", __FUNCTION__);
    #ifdef DSLIB_MEMCHK
    #ifdef __KERNEL__
      KernDumpStack();
    #endif /* __KERNEL__ */
    return OF_FAILURE;
    #endif /* DSLIB_MEMCHK */
  }

  if (pMemPool->MemPoolStats.ulHeapAlwdBlocks !=
      pMemPool->MemPoolStats.ulHeapFreeCnt)
  {
    printf("%s, *********** LEAK INFO BEGIN ************\n\r", __FUNCTION__);
    printf("%s, Leak in dynamic blocks(%u), %s",
            __FUNCTION__,
          (pMemPool->MemPoolStats.ulHeapAlwdBlocks - 
             pMemPool->MemPoolStats.ulHeapFreeCnt),
            " possibly appln didnt release the mem allocated\r\n");
    printf("%s, *********** LEAK INFO END   ************\n\r", __FUNCTION__);
    #ifdef DSLIB_MEMCHK
    #ifdef __KERNEL__
      KernDumpStack();
    #endif /* __KERNEL__ */
    return OF_FAILURE;
    #endif /* DSLIB_MEMCHK */
 
  }

  /* Destroy the semaphore */
  cm_mempool_lock_del (pMemPool);
  of_free (pMemPool);
  
  printf(
    "cm_delete_mempool: Memory Pool Deleted Successfully !!!\n\r");
  return (OF_SUCCESS);
}

/***************************************************************************
 * Function Name : cm_mempool_get_block
 * Description   : Used to get a memory block from the given pool
 * Input         : pPool - mem pool pointer
 *                 heap_b - flag that indicates whether allocation from heap
 *                 is wanted or not (true - wanted, false - unwanted) once
 *                 mem pool is exhausted
 * Output        : ppucBlock - address of the allocated mem block
 *                 heap_b - updated heap flag (true/false)
 * Return value  : OF_SUCCESS/OF_FAILURE/CM_MEM_DRAINED_OUT
 ***************************************************************************/
int32_t cm_mempool_get_block (void *pPool, unsigned char **ppucBlock,
                            unsigned char *heap_b)
{
  UCMMemPool_t     *pMemPool = NULL;
  unsigned char         *pucBlock = NULL;

  *ppucBlock = NULL;

  pMemPool = (UCMMemPool_t *) pPool;

  if (pMemPool == NULL)
  {
    printf(
      "cm_mempool_get_block: Invalid Memory pool pointer accessed \n\r");
    return (OF_FAILURE);
  }

  cm_mempool_locktake (pMemPool);
  /* Check whether this memory pool is to be deleted?? */
  if (CM_MEM_POOL_CHECK_BDEL (pMemPool) == TRUE)
  {
    /* memory pool is due for deletion */
    cm_mempool_lock_give (pMemPool);
    printf(
        "cm_mempool_get_block: MEMORY POOL is DUE for DELETION !!!\r\n");
    return (OF_FAILURE);
  }

  /* increment reference count here - delete safe */
  CM_MEM_POOL_INC_REFCNT(pMemPool);

  /* if free count reaches minimum threshold or zero, then wakeup
   * the kernel thread
   */
  if (((pMemPool->MemPoolStats.ulFreeCnt <=
       pMemPool->MemPoolStats.ulMinThreshold) ||
      (pMemPool->MemPoolStats.ulFreeCnt == 0)) &&
      (pMemPool->MemPoolStats.ulBlocksCnt) &&
      (pMemPool->MemPoolStats.ulStaticAllocCnt < 
       pMemPool->MemPoolStats.ulBlocksCnt))
  {
    cm_mempool_wakeup_kernel_thread (pMemPool);
  }

  /* Get the block pointed by the head and if its valid, return
   * the address of that block and adjust the head pointer to point
   * to the next memory block
   */
  pucBlock = pMemPool->pucHead;
  if (pucBlock)
  {
    /*pMemPool->pucHead = ((UCMMemLinkNode_t *)pucBlock)->next_p;
    ((UCMMemLinkNode_t *)pucBlock)->next_p = NULL;*/
  }
  else
  {
    /* no more memory blocks are available in the static pool,
     * then check whether memory can be allocated from heap,
     * if it can be allocated, then allocated and return the address
     */
    if (*heap_b == TRUE)
    {
      if (pMemPool->MemPoolStats.ulHeapFreeCnt == 0)
      {
        pMemPool->MemPoolStats.ulHeapAllocFailCnt++;
        CM_MEM_POOL_DEC_REFCNT(pMemPool);
        cm_mempool_lock_give (pMemPool);
        printf(
          "cm_mempool_get_block: "
          "No more memory blocks for [%p] available...\n\r", pMemPool);
        return (OF_FAILURE);
      }

      pMemPool->MemPoolStats.ulHeapAllockBlocks++;
      pMemPool->MemPoolStats.ulHeapFreeCnt--;

      cm_mempool_lock_give (pMemPool);
      pucBlock = (unsigned char *) of_calloc (1, pMemPool->MemPoolStats.ulBlockSize);
      if (pucBlock == NULL)
      {
        printf(
          "cm_mempool_get_block: Can't allocate memory from HEAP\n\r");
        cm_mempool_locktake (pMemPool);
        pMemPool->MemPoolStats.ulHeapAllockBlocks--;
        pMemPool->MemPoolStats.ulHeapFreeCnt++;
        pMemPool->MemPoolStats.ulHeapAllocFailCnt++;
        CM_MEM_POOL_DEC_REFCNT(pMemPool);
        /* Check whether this memory pool is to be deleted?? */
        if (CM_MEM_POOL_CHECK_BDEL_NOREF (pMemPool) == TRUE)
        {
          /* Reference count becomes zero, hence delete this memory pool */
          cm_mempool_lock_give (pMemPool);
          ucm_delete_mempool (pMemPool);
          printf(
              "cm_mempool_get_block: MEMORY POOL has been DELETED !!!\r\n");
          return (OF_FAILURE);
        }
        cm_mempool_lock_give (pMemPool);
        return (OF_FAILURE);
      }
      *heap_b = TRUE;
      *ppucBlock = pucBlock;
      cm_mempool_locktake (pMemPool);
      CM_MEM_POOL_DEC_REFCNT(pMemPool);
      /* Check whether this memory pool is to be deleted?? */
      if (CM_MEM_POOL_CHECK_BDEL_NOREF (pMemPool) == TRUE)
      {
        /* Reference count becomes zero, hence delete this memory pool */
        cm_mempool_lock_give (pMemPool);
        ucm_delete_mempool (pMemPool);
        printf(
            "cm_mempool_get_block: MEMORY POOL has been DELETED !!!\r\n");
        return (OF_FAILURE);
      }
      cm_mempool_lock_give (pMemPool);
      return (OF_SUCCESS);
    }
    else
    {
      pMemPool->MemPoolStats.ulAllocFailCnt++;
      CM_MEM_POOL_DEC_REFCNT(pMemPool);
      cm_mempool_lock_give (pMemPool);
      printf( "cm_mempool_get_block: Memory pool "
          "- DRAINED OUT -\r\n");
      return (CM_MEM_DRAINED_OUT);
    }
  }

  #ifdef DSLIB_MEMCHK
  /* set the magic number */
  *((uint32_t *)(pucBlock+(pMemPool->MemPoolStats.ulBlockSize))) = 
                    MEMP_STATIC_MAGICNUM;
  #endif
  *heap_b = FALSE;
  *ppucBlock = pucBlock;
  CM_MEM_POOL_DEC_REFCNT(pMemPool);
  pMemPool->MemPoolStats.ulAllocCnt++;
  pMemPool->MemPoolStats.ulFreeCnt--;

  /* update the linked lists of static blocks */
  pMemPool->pucHead = ((UCMMemLinkNode_t *)pucBlock)->next_p;
  ((UCMMemLinkNode_t *)pucBlock)->next_p = NULL;

  cm_mempool_lock_give (pMemPool);
  return (OF_SUCCESS);
}

/***************************************************************************
 * Function Name : cm_mempool_release_block
 * Description   : Used to release a memory block to the given pool
 * Input         : pPool - mem pool pointer
 *                 pucBlock - memory block to be released
 *                 heap_b - flag that indicates whether allocation is done
 *                 from heap or not (true - done from heap, false - not)
 * Output        : None
 * Return value  : OF_SUCCESS/OF_FAILURE
 ***************************************************************************/
int32_t cm_mempool_release_block (void *pPool, unsigned char *pucBlock,
                                unsigned char heap_b)
{
  UCMMemPool_t     *pMemPool = NULL;
#ifdef DSLIB_DBG
  UCMMemLinkNode_t *pMemLinkNode = NULL;
#endif

  pMemPool = (UCMMemPool_t *) pPool;

  if (pMemPool == NULL)
  {
    printf(
      "cm_mempool_release_block: Invalid Memory pool pointer accessed \n\r");
    return (OF_FAILURE);
  }

  if (pucBlock == NULL)
  {
    pMemPool->MemPoolStats.ulReleaseFailCnt++;
    printf(
      "cm_mempool_release_block: NULL pointer passed for Release\n\r");
    return (OF_FAILURE);
  }

  cm_mempool_locktake (pMemPool);
  /* increment reference count here - delete safe */
  CM_MEM_POOL_INC_REFCNT(pMemPool);
  cm_mempool_lock_give (pMemPool);

  /* If the memory block is within the memory pool boundary, then adjust the
   * head pointer of the memory blocks.
   */
  if (heap_b == TRUE)
  {
    /* allocated from heap */
    of_free (pucBlock);
    cm_mempool_locktake (pMemPool);
    pMemPool->MemPoolStats.ulHeapReleaseCnt++;
    pMemPool->MemPoolStats.ulHeapFreeCnt++;
    CM_MEM_POOL_DEC_REFCNT(pMemPool);
    /* Check whether this memory pool is to be deleted?? */
    if (CM_MEM_POOL_CHECK_BDEL_NOREF (pMemPool) == TRUE)
    {
      /* Reference count becomes zero, hence delete this memory pool */
    cm_mempool_lock_give (pMemPool);
    ucm_delete_mempool (pMemPool);
    printf(
          "cm_mempool_release_block: MEMORY POOL has been DELETED !!!\r\n");
      return (OF_FAILURE);
    }
    cm_mempool_lock_give (pMemPool);
    return (OF_SUCCESS);
  }

  #ifdef DSLIB_MEMCHK
  if ((*((uint32_t *)(pucBlock+(pMemPool->MemPoolStats.ulBlockSize))))
      != MEMP_STATIC_MAGICNUM)
  {
    printf(
      "cm_mempool_release_block: Possible Memory Corruption may happen(d)"
      "[%p]\n\r", pucBlock);
    #ifdef __KERNEL__
      KernDumpStack();
    #endif /* __KERNEL__ */
    cm_mempool_locktake (pMemPool);
    CM_MEM_POOL_DEC_REFCNT(pMemPool);
    /* Check whether this memory pool is to be deleted?? */
    if (CM_MEM_POOL_CHECK_BDEL_NOREF (pMemPool) == TRUE)
    {
      /* Reference count becomes zero, hence delete this memory pool */
      cm_mempool_lock_give (pMemPool);
      ucm_delete_mempool (pMemPool);
      printf(
          "cm_mempool_release_block: MEMORY POOL has been DELETED !!!\r\n");
      return (OF_FAILURE);
    }
    cm_mempool_lock_give (pMemPool);
    return (OF_FAILURE);
  }
  /* reset the magic number */
  *((uint32_t *)(pucBlock+(pMemPool->MemPoolStats.ulBlockSize))) = 0;
  #endif

  cm_mempool_locktake (pMemPool);
  pMemPool->MemPoolStats.ulReleaseCnt++;
  pMemPool->MemPoolStats.ulFreeCnt++;
  if (pMemPool->MemPoolStats.ulFreeCnt >
      pMemPool->MemPoolStats.ulMaxThreshold)
  {
    /* if memory pool already have maximum threshold value blocks,
     * then release this memory to the heap, not to the pool.
     * this is to ensure, only upto maximum threshold blocks will be
     * present in the pool
     */
    pMemPool->MemPoolStats.ulFreeCnt--;
    pMemPool->MemPoolStats.ulStaticAllocCnt--;
    CM_MEM_POOL_DEC_REFCNT(pMemPool);
    /* Check whether this memory pool is to be deleted?? */
    if (CM_MEM_POOL_CHECK_BDEL_NOREF (pMemPool) == TRUE)
    {
      /* Reference count becomes zero, hence delete this memory pool */
      cm_mempool_lock_give (pMemPool);
      of_free (pucBlock);
      ucm_delete_mempool (pMemPool);
      printf(
          "cm_mempool_release_block: MEMORY POOL has been DELETED !!!\r\n");
      return (OF_FAILURE);
    }
    cm_mempool_lock_give (pMemPool);
    of_free (pucBlock);
    return (OF_SUCCESS);
  }

  of_memset(pucBlock, 0, pMemPool->MemPoolStats.ulBlockSize);
  ((UCMMemLinkNode_t *)pucBlock)->next_p = pMemPool->pucHead;
  pMemPool->pucHead = pucBlock;
  CM_MEM_POOL_DEC_REFCNT(pMemPool);
  /* Check whether this memory pool is to be deleted?? */
  if (CM_MEM_POOL_CHECK_BDEL_NOREF (pMemPool) == TRUE)
  {
    /* Reference count becomes zero, hence delete this memory pool */
    cm_mempool_lock_give (pMemPool);
    ucm_delete_mempool (pMemPool);
    printf(
        "cm_mempool_release_block: MEMORY POOL has been DELETED !!!\r\n");
    return (OF_FAILURE);
  }
  cm_mempool_lock_give (pMemPool);
  return (OF_SUCCESS);
}

/***************************************************************************
 * Function Name : cm_mempool_get_stats
 * Description   : Used to get statistics of memory pool
 * Input         : pPool - mem pool pointer
 *                 pStats - pointer to statistics structure
 * Output        : None
 * Return value  : OF_SUCCESS/OF_FAILURE
 ***************************************************************************/
int32_t cm_mempool_get_stats (void *pPool, struct cm_mempool_stats *pStats)
{
  UCMMemPool_t     *pMemPool = NULL;

  pMemPool = (UCMMemPool_t *) pPool;

  if (pMemPool == NULL)
  {
    printf(
      "cm_mempool_get_stats: Invalid Memory pool pointer accessed \n\r");
    return (OF_FAILURE);
  }

  pStats->ulFreeCnt         = pMemPool->MemPoolStats.ulFreeCnt;
  pStats->ulAllocCnt        = pMemPool->MemPoolStats.ulAllocCnt;
  pStats->ulReleaseCnt      = pMemPool->MemPoolStats.ulReleaseCnt;
  pStats->ulAllocFailCnt    = pMemPool->MemPoolStats.ulAllocFailCnt;
  pStats->ulReleaseFailCnt  = pMemPool->MemPoolStats.ulReleaseFailCnt;
  pStats->ulHeapFreeCnt     = pMemPool->MemPoolStats.ulHeapFreeCnt;
  pStats->ulHeapAllockBlocks= pMemPool->MemPoolStats.ulHeapAllockBlocks;
  pStats->ulHeapReleaseCnt  = pMemPool->MemPoolStats.ulHeapReleaseCnt;
  pStats->ulHeapAllocFailCnt= pMemPool->MemPoolStats.ulHeapAllocFailCnt;
  pStats->ulHeapAlwdBlocks  = pMemPool->MemPoolStats.ulHeapAlwdBlocks;
  pStats->ulBlocksCnt       = pMemPool->MemPoolStats.ulBlocksCnt;
  pStats->ulBlockSize       = pMemPool->MemPoolStats.ulBlockSize;
  pStats->ulMinThreshold    = pMemPool->MemPoolStats.ulMinThreshold;
  pStats->ulMaxThreshold    = pMemPool->MemPoolStats.ulMaxThreshold;
  pStats->ulReplenishCnt    = pMemPool->MemPoolStats.ulReplenishCnt;
  pStats->ulStaticAllocCnt  = pMemPool->MemPoolStats.ulStaticAllocCnt;
  pStats->ulThreadWokenUpCnt= pMemPool->MemPoolStats.ulThreadWokenUpCnt;

  return (OF_SUCCESS);
}

/***************************************************************************
 * Function Name : cm_memalloc_and_chain_memchunk
 * Description   : Checks for the validitry of memory block
 * Input         : pPool - memory pool pointer
 *                 pucBlock - memory block
 *                 heap_b - allocated from heap/pool (true/false)
 * Output        : pulAllocCnt - allocated number of blocks
 * Return value  : OF_SUCCESS/OF_FAILURE
 ***************************************************************************/
unsigned char * cm_memalloc_and_chain_memchunk (UCMMemPool_t *pMemPool,
                                        uint32_t ulBlocks,
                                        uint32_t ulBlockSize,
                                        uint32_t *pulAllocCnt)
{
  unsigned char   *pucHead = NULL, *pucBlock = NULL, *pucPrev = NULL;
  uint32_t    ulAllocCnt = 0;
  int32_t     ii = 0;

#ifdef DSLIB_MEMCHK
  ulBlockSize += MEMP_STATIC_CHK_BYTES;
#endif

  /* Allocate and Chain the free pool structs in a singly linked list */
  for (ii=0; ii < ulBlocks; ii++)
  {
    pucBlock = (unsigned char*) of_calloc (1, ulBlockSize);
    if (pucBlock == NULL)
    {
      /* mem allocation fail */
      printf(
        "cm_memalloc_and_chain_memchunk: Allocation FAILED !!!: "
        "Unable to allocate required memory\n\r");
      if (pucPrev)
      {
        /* some memory blocks are allocated, hence update the statistics
         * and return the chunk head pointer
         */
        ((UCMMemLinkNode_t *) pucPrev)->next_p = NULL;
        *pulAllocCnt = ulAllocCnt;
      }
      return (pucHead);
    }
    ulAllocCnt++;
    if (pucPrev == NULL)
    {
      pucHead = pucPrev = pucBlock;
      continue;
    }
    ((UCMMemLinkNode_t *) pucPrev)->next_p = pucBlock;
    pucPrev = pucBlock;
  }
  if (pucBlock)
  {
    /* this condition fails, when ulBlocks is zero */
    ((UCMMemLinkNode_t *) pucBlock)->next_p = NULL;
    *pulAllocCnt = ulAllocCnt;
  }
  return (pucHead);
}

/***************************************************************************
 * Function Name : cm_mempool_delete_from_refill_list
 * Description   : Deletes a particular memory pool from refill list
 * Input         : pPool - memory pool pointer
 * Output        : None
 * Return value  : OF_SUCCESS - when pool can be deleted
 *                 OF_FAILURE - when pool cannot deleted.
 ***************************************************************************/
 int32_t cm_mempool_delete_from_refill_list (UCMMemPool_t *pMemPool)
{

  /* The comment is applicable for LINUX
   * Delete the memory pool from refill list.
   * Here we are waiting on Global Lock which is taken at the beginning of
   * processing the refill list. If all the memory pools in the list are
   * processed at once, then this mempool would have been already deleted, hence
   * taking and releasing lock will be sufficient.
   * But to scale, its not advised to process all the memory pools at once,
   * hence we can issue the below call to delete it from refill list
   */
  cm_mempool_locktake (pMemPool);
  if (CM_MEM_POOL_IS_REFCNT_ZERO (pMemPool) == TRUE)
  {
    /* Reference count is zero, pool can be deleted */
    cm_mempool_lock_give (pMemPool);
    return (OF_SUCCESS);
  }
  /* Reference count is non-zero, set the bDel flag */
  CM_MEM_POOL_SET_BDEL (pMemPool);
  cm_mempool_lock_give (pMemPool);
  return (OF_FAILURE);
}

#endif /* _MEMPOOL_C */
