
/**
 * Copyright (c) 2012, 2013  Freescale.
 *  
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/

/* File  :      tcp_api.c
 * Author:      Atmaram G <atmaram.g@freescale.com>
 * Date:   08/23/2013
 * Description: TCP plugin APIs.
 */

/******************************************************************************
 * * * * * * * * * * * * Include Files * * * * * * * * * * * * * * * * * *
 *******************************************************************************/

#include "controller.h"
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "sll.h"
#include "memapi.h"
#include "memldef.h"
#include "dll.h"
#include "pktmbuf.h"
#include "pethapi.h"

/******************************************************************************
 * * * * * * * * * * * * Global Variables * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
extern uint32_t      mempool_max_threads_g;

/******************************************************************************
 * * * * * * * * * * * * Function Prototypes * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
int mempool_create_mheap_pool (char *pool_name,
      struct mempool_params *pool_params_p,
      void*    mempool_p);

int32_t mempool_delete_mheap_pool(void* pool_p);

int32_t PSP_PKTPROC_FUNC mempool_mheap_get_mem_block(void* pool_p,
      uchar8_t **mem_block_p_p,
      uint8_t* heap_b_p);

int32_t mempool_mheap_release_mem_block(void*     pool_p,
      uchar8_t* mem_block_p,
      uchar8_t  heap_b);


/******************************************** **********************************
 * * * * * * * F U N C T I O N   D E F I N I T I O N S* * * * * * * * * * * * *
 ******************************************************************************/

/***************************************************************************
 * Function Name : mempool_cerate_mheap_pool
 * Description   : Used to create memory pool using 'mmap' memory.
 *                 Allocates 'mmap' memory, divides the memory into blocks of
 *                 block size and maintains the memory blocks in a mempool.
 * Input         :
 *                 block_size - block size
 *                 max_blocks    - Number of blocks to be allocated.
 * Output        : None.
 * Return value  : MEMPOOL_SUCCESS/MEMPOOL_FAILURE
 ***************************************************************************/
int mempool_create_mheap_pool (char *pool_name,
      struct mempool_params *pool_params_p,
      void*    mempool)
{
   struct memory_pool   *mempool_p =(struct memory_pool *)mempool;
   uint32_t      alloc_count = 0;
   uint32_t   heap_allowed_blocks = 0;
   uchar8_t      *tail_p = NULL;
   int32_t status=MEMPOOL_SUCCESS;


   /* sizeof the block should be atleast 8 bytes.
    * First word (4 bytes) is used to maintain the linked list of free blocks.
    * Second word is used to store physical address of mapped kernel memory
    */

   do
   {
      if ( pool_params_p == NULL )
      {
         OF_LOG_MSG (OF_LOG_UTIL, OF_LOG_ERROR, "invalid input");
         status=MEMPOOL_FAILURE;   
         break;
      }

      if(pool_params_p->block_size <= 8)
      {
         OF_LOG_MSG (OF_LOG_UTIL, OF_LOG_ERROR, "BLOCKSIZE is less than 8 bytes ");
         status= MEMPOOL_ERROR_BLOCKSIZE_TOO_SMALL;
         break;
      }

      heap_allowed_blocks = pool_params_p->max_blocks - pool_params_p->static_blocks;
      mempool_p->mempool_type=MEMPOOL_TYPE_HEAP;
      mempool_p->static_blocks = pool_params_p->static_blocks;
      mempool_p->heap_allowed_blocks_count = heap_allowed_blocks;
      mempool_p->heap_free_blocks_count = heap_allowed_blocks;

      mempool_p->head_p = memory_alloc_chain_memchunks (mempool_p,
            pool_params_p->threshold_max,
            pool_params_p->block_size, &alloc_count, 0, &tail_p);
      if(mempool_p->head_p == NULL)
      {
         free(mempool_p);
         status= MEMPOOL_ERROR_UNABLE_TO_ALLOCATE_MEMORY_CHUNK;
         break;
      }
      mempool_p->tail_p = tail_p;
      /* Update the counters with real ulAllocCnt */
      mempool_p->free_blocks_count += alloc_count;
      mempool_p->static_allocated_blocks_count += alloc_count;
      if (mempool_set_threshold_values (mempool_p, pool_params_p->threshold_min,
               pool_params_p->threshold_max, pool_params_p->replenish_count) !=
            MEMPOOL_SUCCESS)
      {
         mempool_delete_mheap_pool(mempool_p);
         status= MEMPOOL_ERROR_SETTING_THRESHOLD_VALUES;
         break;
      }

      if (set_mempool_attributes (mempool_p,
               pool_params_p->b_list_per_thread_required,
               pool_params_p->noof_blocks_to_move_to_thread_list,
               pool_params_p->b_memset_not_required) != MEMPOOL_SUCCESS)
      {
         mempool_delete_mheap_pool(mempool_p);
         status=MEMPOOL_FAILURE;
         break;
      }

      status= MEMPOOL_SUCCESS;
   }while(0);

   return status;
}


int32_t mempool_delete_mheap_pool(void* pool_p)
{
   uchar8_t* head_p = NULL, *tmp_p = NULL,*per_thread_head_p = NULL;
   int32_t  ii;
   struct memory_pool   *mempool_p =(struct memory_pool *)pool_p;

   OF_LOG_MSG (OF_LOG_PETH_APP, OF_LOG_DEBUG, "entered");

   if(pool_p == NULL)
      return MEMPOOL_ERROR_NULL_MEMPOOL_PASSED;


   if(mempool_p->per_thread_list_p)
   {
      for(ii=0;ii<mempool_max_threads_g;ii++)
      {
         per_thread_head_p = mempool_p->per_thread_list_p[ii].per_thread_head_p;
         while(per_thread_head_p)
         {
            /* Adding back are the nodes from cpu free list back to general list */
            tmp_p = ((struct mem_link_node *)per_thread_head_p)->next_p;
            ((struct mem_link_node *)per_thread_head_p)->next_p = mempool_p->head_p;
            mempool_p->head_p = per_thread_head_p;
            mempool_p->released_blocks_count++;
            mempool_p->free_blocks_count++;
            per_thread_head_p = tmp_p;
         }
      }
      free(mempool_p->per_thread_list_p);
   }

   head_p = mempool_p->head_p;
   while(head_p)
   {
      tmp_p = ((struct mem_link_node *)head_p)->next_p;
#ifdef CNTLR_DSMEMPOOL_STATIC_DEBUG
      free(head_p - MEMP_STATIC_PRE_SIGNATURE_BYTES);
#else
      free(head_p);
#endif
      head_p = tmp_p;
   }
   mempool_p->head_p = NULL;

   if(mempool_p->free_blocks_count !=
         mempool_p->static_allocated_blocks_count)
   {
#ifdef CNTLR_DSMEMPOOL_STATIC_DEBUG
      //of_ShowStackTrace();
      return MEMPOOL_FAILURE;
#endif /* CNTLR_DSMEMPOOL_STATIC_DEBUG */
   }

   if(mempool_p->heap_allowed_blocks_count !=
         mempool_p->heap_free_blocks_count)
   {
#ifdef CNTLR_DSMEMPOOL_STATIC_DEBUG
      return MEMPOOL_FAILURE;
#endif /* CNTLR_DSMEMPOOL_STATIC_DEBUG */
   }
   free(mempool_p);
   return MEMPOOL_SUCCESS;
}



/***********************************************************************************************/
int32_t PSP_PKTPROC_FUNC mempool_mheap_get_mem_block(void* pool_p,
      uchar8_t **mem_block_p_p,
      uint8_t* heap_b_p)

{
   struct memory_pool* mempool_p = (struct memory_pool *)pool_p;
   uchar8_t  *block_p = NULL, *temp_block_p = NULL;
   uchar8_t  thread_num = 0;
   uint32_t  ii,count;

   *mem_block_p_p = NULL;

   /* Check if mempool is valid */
   if(CNTLR_UNLIKELY(!mempool_p))
      return MEMPOOL_ERROR_NULL_MEMPOOL_PASSED;

   thread_num = cntlr_thread_index;

   /* Check per thread mempool*/
   if(CNTLR_LIKELY(thread_num && mempool_p->per_thread_list_p)) 
   {
      struct per_thread_mem_node* th_list_p;

      thread_num--;
      th_list_p = &mempool_p->per_thread_list_p[thread_num];

      if(CNTLR_LIKELY(th_list_p->free_count))
      {
         /* Free blocks are available in thread list. 
          * Allocate a node to the application.
          */
         *mem_block_p_p = th_list_p->per_thread_head_p;
         th_list_p->per_thread_head_p = 
            ((struct mem_link_node *)(*mem_block_p_p))->next_p;
         th_list_p->free_count--;
         th_list_p->alloc_count++;
         ((struct mem_link_node *)(*mem_block_p_p))->next_p = NULL;
         MEMP_INSERT_DEBUG_INFO_IN_STATIC_BLOCK((*mem_block_p_p));
         *heap_b_p = FALSE;
         return MEMPOOL_SUCCESS;
      }
      else
      {
         /* No free blocks in thread list. Move static blocks to the thread list.
          * And allocate a node from the thread list to the application.
          */
         CNTLR_LOCK_TAKE(mempool_p->mempool_lock);
         //mempool_dbg("\r\n %s: Thread Num = %u, Per Thread free count = %u   ",__FUNCTION__,ucThreadNum+1,pMemPool->pPerThreadList[ucThreadNum].uiFreeCount);
         if(mempool_p->per_thread_list_p[thread_num].free_count)
         {
            *mem_block_p_p = mempool_p->per_thread_list_p[thread_num].per_thread_head_p;
            mempool_p->per_thread_list_p[thread_num].per_thread_head_p = 
               ((struct mem_link_node *)(*mem_block_p_p))->next_p;
            mempool_p->per_thread_list_p[thread_num].free_count--;
            mempool_p->per_thread_list_p[thread_num].alloc_count++;
            CNTLR_LOCK_RELEASE(mempool_p->mempool_lock);
            ((struct mem_link_node *)(*mem_block_p_p))->next_p = NULL;
            MEMP_INSERT_DEBUG_INFO_IN_STATIC_BLOCK((*mem_block_p_p));
            *heap_b_p = FALSE;
            return MEMPOOL_SUCCESS;
         }

         if(MEMP_CHECK_STATIC_FREECNT_AND_REFILL(mempool_p))
            mempool_check_and_refill(mempool_p);

         temp_block_p = mempool_p->head_p;
#if 0
         if(!pucTempBlock)
            printf("\r\n %s:No more static allocations possible...",__FUNCTION__);
#endif
         if(temp_block_p && mempool_p->block_count_for_moving_to_threadlist)
         {
            /*mempool_dbg("Thread Num = %u, BMV cnt = %u, Global pool Free Count = %u\n",
              ucThreadNum+1,pMemPool->uiBlockMoveCountToThreadList,pMemPool->MemPoolStats.ulFreeCnt);*/
            if(mempool_p->block_count_for_moving_to_threadlist < 
                  mempool_p->free_blocks_count)
            {
               /*mempool_dbg("\r\n %s: Thread Num = %u,  ",__FUNCTION__,thread_num); */
               count = mempool_p->block_count_for_moving_to_threadlist;
               block_p = temp_block_p;
               ii = count - 1;
               while(ii > 0)
               {
                  block_p = ((struct mem_link_node *)block_p)->next_p;
                  ii--;
               }
               mempool_p->head_p = ((struct mem_link_node *)block_p)->next_p;
               ((struct mem_link_node *)block_p)->next_p = NULL;
            }
            else
            {
               count = mempool_p->free_blocks_count;
               mempool_p->head_p = NULL;
               mempool_p->tail_p = NULL;
            }

            mempool_p->allocated_blocks_count += count;
            mempool_p->free_blocks_count -= count;
            // mempool_dbg("\r\n %s: Thread Num = %u, Global Pool Free Count = %u   ",__FUNCTION__,ucThreadNum+1,pMemPool->MemPoolStats.ulFreeCnt); 

            mempool_p->per_thread_list_p[thread_num].per_thread_head_p = 
               ((struct mem_link_node *)temp_block_p)->next_p;

            mempool_p->per_thread_list_p[thread_num].free_count = (count - 1);
            //mempool_dbg("\r\n %s: Thread Num = %u, Per Thread free count = %u   ",__FUNCTION__,ucThreadNum+1,pMemPool->pPerThreadList[ucThreadNum].uiFreeCount);
            CNTLR_LOCK_RELEASE(mempool_p->mempool_lock);
            ((struct mem_link_node *)temp_block_p)->next_p = NULL;
            MEMP_INSERT_DEBUG_INFO_IN_STATIC_BLOCK(temp_block_p);
            *mem_block_p_p  = temp_block_p;
            *heap_b_p = FALSE;
            return MEMPOOL_SUCCESS;
         }
      }
   }
   else
   {
      //mempool_dbg("\r\n %s: Get Block from Global Pool .....",__FUNCTION__);
      /*Check free list */
      CNTLR_LOCK_TAKE(mempool_p->mempool_lock);

      if(MEMP_CHECK_STATIC_FREECNT_AND_REFILL(mempool_p)) 
         mempool_check_and_refill(mempool_p);

      temp_block_p = mempool_p->head_p;
      if(CNTLR_LIKELY(temp_block_p))
      {
         mempool_p->head_p = ((struct mem_link_node *)temp_block_p)->next_p;
         if(CNTLR_UNLIKELY(!(mempool_p->head_p)))
         {
            mempool_p->tail_p = NULL;
         }
         mempool_p->allocated_blocks_count++;
         mempool_p->free_blocks_count--;
         ((struct mem_link_node *)temp_block_p)->next_p = NULL;
         CNTLR_LOCK_RELEASE(mempool_p->mempool_lock);
         MEMP_INSERT_DEBUG_INFO_IN_STATIC_BLOCK(temp_block_p);
         *heap_b_p = FALSE;
         *mem_block_p_p = temp_block_p;
         return MEMPOOL_SUCCESS;
      }
      else
      {
         /*mempool_dbg("\r\n %s: No Blocks in Global Pool !!!!! \r\n",__FUNCTION__);*/
      }
   }
   /* Allocate from Heap */
   if (/*CNTLR_UNLIKELY*/((*heap_b_p) && (mempool_p->heap_free_blocks_count)))
   {
      /* These two counters below are updated here only, as to avoid taking lock
       * again to update these flags after the memory successfully allocated.
       * If memory allocation fails, then these flags are reverted back
       */
      mempool_p->heap_allocated_blocks_count++;
      mempool_p->heap_free_blocks_count--;
      CNTLR_LOCK_RELEASE(mempool_p->mempool_lock);
      *mem_block_p_p = (uchar8_t *)calloc (1, mempool_p->block_size);
      if(CNTLR_LIKELY(*mem_block_p_p)) {
         *heap_b_p = TRUE;
         return MEMPOOL_SUCCESS;
      }

      CNTLR_LOCK_TAKE(mempool_p->mempool_lock);
      mempool_p->heap_allocation_fail_blocks_count++;
      //mempool_p->heap_allocated_blocks_count--;
      //mempool_p->heap_free_blocks_count++;
      CNTLR_LOCK_RELEASE(mempool_p->mempool_lock);
      return MEMPOOL_ERROR_MEM_ALLOCATION_FROM_HEAP_FAILURE;
   }

   mempool_p->allocation_fail_blocks_count++;
   CNTLR_LOCK_RELEASE(mempool_p->mempool_lock);
   return MEMPOOL_FAILURE;
}

int32_t mempool_mheap_release_mem_block(void*     pool_p,
      uchar8_t* mem_block_p,
      uchar8_t  heap_b)

{
   struct memory_pool* mempool_p = (struct memory_pool *)pool_p;
   uchar8_t thread_num = 0;


   if(CNTLR_LIKELY(heap_b != TRUE))
   {
      MEMP_CHECK_DEBUG_INFO_IN_STATIC_BLOCK(mem_block_p);

#ifndef CNTLR_DSMEMPOOL_STATIC_DEBUG
      if(CNTLR_UNLIKELY(!(mempool_p->flags & CNTLR_MEMPOOL_DONOT_MEMSET)))
         memset(mem_block_p, 0, mempool_p->block_size);
#endif

      thread_num = cntlr_thread_index;

      if(CNTLR_LIKELY(thread_num  &&   
               (mempool_p->per_thread_list_p) &&
               (mempool_p->per_thread_list_p[thread_num-1].free_count <
                mempool_p->block_count_for_moving_to_threadlist)))
      {
         struct per_thread_mem_node* th_list_p;

         thread_num--; 
         th_list_p = &mempool_p->per_thread_list_p[thread_num];
         if(mem_block_p == th_list_p->per_thread_head_p)
         {
            //of_ShowStackTrace();
         }
         ((struct mem_link_node *)mem_block_p)->next_p = th_list_p->per_thread_head_p;
         th_list_p->per_thread_head_p = mem_block_p;
         th_list_p->free_count++;
         th_list_p->release_count++;
         return MEMPOOL_SUCCESS;
      }
      else
      {
         //mempool_dbg("\r\n %s: Release Block to Global Pool .....",__FUNCTION__);
         CNTLR_LOCK_TAKE(mempool_p->mempool_lock);
         if (CNTLR_UNLIKELY(mempool_p->free_blocks_count >=
                  mempool_p->threshold_max))
         {
            /* if memory pool already have maximum threshold value blocks, then 
             * release this memory to the heap, not to the pool. This is to ensure,
             * only upto maximum threshold blocks will be present in the pool
             */
            mempool_p->static_allocated_blocks_count--;
            mempool_p->released_blocks_count++;
            CNTLR_LOCK_RELEASE(mempool_p->mempool_lock);
#ifdef CNTLR_DSMEMPOOL_STATIC_DEBUG
            free(mem_block_p - MEMP_STATIC_PRE_SIGNATURE_BYTES);
#else
            free(mem_block_p);
#endif

            return MEMPOOL_SUCCESS;
         }

         ((struct mem_link_node *)mem_block_p)->next_p = NULL; 
         if(CNTLR_LIKELY(mempool_p->tail_p))
         {
            ((struct mem_link_node *)(mempool_p->tail_p))->next_p = mem_block_p;
         }
         else
         {
            mempool_p->head_p = mem_block_p;
         }
         mempool_p->tail_p = mem_block_p;
         mempool_p->released_blocks_count++;
         mempool_p->free_blocks_count++;
         CNTLR_LOCK_RELEASE(mempool_p->mempool_lock);
         return MEMPOOL_SUCCESS;
      }
   }
   else
   {
      free(mem_block_p);
      CNTLR_LOCK_TAKE(mempool_p->mempool_lock);
      mempool_p->heap_released_blocks_count++;
      mempool_p->heap_free_blocks_count++;
      CNTLR_LOCK_RELEASE(mempool_p->mempool_lock);
      return MEMPOOL_SUCCESS;
   }
   return MEMPOOL_FAILURE;
}



