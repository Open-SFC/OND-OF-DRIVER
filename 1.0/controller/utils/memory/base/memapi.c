

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
#include "sll.h"
#include "memapi.h"
#include "memldef.h"

/******************************************************************************
 * * * * * * * * * * * * Function Prototypes * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
int32_t cntlr_register_memory_pool(int32_t mempool_type,
      struct of_mempool_callbacks *mempool_callbacks_p);

/******************************************************************************
 * * * * * * * * * * * * Macro Definitions * * * * * * * * * * * * * * * * * *
 *******************************************************************************/


/******************************************************************************
 * * * * * * * * * * * * Global Variables * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
extern cntlr_instance_t cntlr_instnace; 
cntlr_lock_t  mempool_lock_g; 
uint32_t      mempool_index_g = 0;
uint32_t      mempool_max_threads_g;
/* Linked list that holds all the memory pools */
struct cntlr_sll_q   mempool_list_g;

/******************************************************************************
 * * * * * * * * * * * * Function Definitions * * * * * * * * * * * * * * * * * 
 *******************************************************************************/


/***************************************************************************
 * Function Name : mempool_library_init
 * Description   : Used to initialize global linked list
 * Input         : None.
 * Output        : None.
 * Return value  : MEMPOOL_SUCCESS
 ***************************************************************************/
int32_t mempool_library_init(void)
{
   OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_DEBUG,"entered");
   OF_LIST_INIT(cntlr_instnace.mempool_list, NULL);
   CNTLR_SLLQ_INIT_LIST(&mempool_list_g);
   CNTLR_LOCK_INIT(mempool_lock_g);
// of_mheap_init( );
   return (MEMPOOL_SUCCESS);
}


int32_t cntlr_register_memory_pool(int32_t mempool_type,
      struct of_mempool_callbacks *mempool_callbacks_p)
{
   cntlr_mempool_info_t   *mempool_info_p;
   int32_t                 status = OF_SUCCESS;
   int32_t                 retval = OF_SUCCESS;
   int8_t offset=CNTLR_MEMPOOL_LISTNODE_OFFSET;

   OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_DEBUG,"entered");

   do
   {
      if(mempool_type   < 0)
      {
         OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_ERROR,"Invalid input");
         status = OF_FAILURE;
         break;
      }

      if ( mempool_callbacks_p == NULL )
      {
         OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_ERROR,"Invalid callback pointer");
         status = OF_FAILURE;
         break;
      }

      CNTLR_RCU_READ_LOCK_TAKE();
      OF_LIST_SCAN(cntlr_instnace.mempool_list,mempool_info_p, cntlr_mempool_info_t *,offset)
      {
         if (mempool_info_p->mempool_type == mempool_type)
         {
            OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_ERROR,"mempool already exists");
            status = OF_FAILURE;
            break;
         }

      }
      CNTLR_RCU_READ_LOCK_RELEASE();

      if ( status == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_ERROR,"mempool already exists");
         status = OF_FAILURE;
         break;
      }

      mempool_info_p = (cntlr_mempool_info_t*) of_mem_calloc(1,sizeof(cntlr_mempool_info_t));
      if(mempool_info_p == NULL)
      {
         OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_ERROR,"Memory Allocation failure");
         status = OF_FAILURE;
         break;
      }

      mempool_info_p->mempool_type = mempool_type;
      mempool_info_p->mempool_callbacks = mempool_callbacks_p;
      OF_APPEND_NODE_TO_LIST(cntlr_instnace.mempool_list,mempool_info_p,offset);
   }
   while(0);

   return status;
}



int32_t mempool_create_pool (char *pool_name,
      struct mempool_params *pool_params_p, void **mempool_pp)
{
   struct     memory_pool *mempool_p;
   struct     memory_pool *tmp_mempool_p = NULL;
   uint32_t   heap_allowed_blocks = 0;
   uint32_t   alloc_cnt = 0;
   uchar8_t*  tail_p = NULL;
   cntlr_mempool_info_t *mempool_info;
   int8_t offset=CNTLR_MEMPOOL_LISTNODE_OFFSET;
   int8_t mempool_found_b=TRUE;
   int32_t                 status = OF_SUCCESS;
   int32_t                 retval = MEMPOOL_SUCCESS;

   OF_LOG_MSG (OF_LOG_UTIL, OF_LOG_DEBUG, "entered");

   do
   {
      if ((!pool_name) || (!pool_params_p) || (!mempool_pp))
      {
         OF_LOG_MSG (OF_LOG_UTIL, OF_LOG_ERROR, "invalid inputs");
         status=MEMPOOL_ERROR_INVALID_PARAMS;
         break;
      }

      heap_allowed_blocks = pool_params_p->max_blocks - pool_params_p->static_blocks;

      /* sizeof the block should be atleast 8 bytes.
       * First word (4 bytes) is used to maintain the linked list of free blocks.
       * Second word is used to store physical address of mapped kernel memory
       */
      if(pool_params_p->block_size <= 8)
      {
         OF_LOG_MSG (OF_LOG_UTIL, OF_LOG_ERROR," BLOCKSIZE is less than 8 bytes ");
         status=MEMPOOL_ERROR_BLOCKSIZE_TOO_SMALL;
         break;
      }

      mempool_p = (struct memory_pool *) calloc (1, sizeof(struct memory_pool));
      if(mempool_p == NULL)
      {
         OF_LOG_MSG (OF_LOG_UTIL, OF_LOG_ERROR,"mempool allocation failed");
         status= MEMPOOL_ERROR_NO_MEM;
         break;
      }
      strncpy(mempool_p->mempool_name, pool_name, MEMPOOL_MAX_POOL_NAME_LEN);

      /* Check for duplicate name and append the global variable 
       * such that duplicate names can be avoided. */
      CNTLR_RCU_READ_LOCK_TAKE();
      CNTLR_SLLQ_SCAN(&mempool_list_g, tmp_mempool_p, struct memory_pool *)
      {
         if(!strcmp(tmp_mempool_p->mempool_name, pool_name))
         {
            /*Check for MEMPOOL_MAX_MEMPOOL_NAME_LEN */
            sprintf(mempool_p->mempool_name,"%s#%u",pool_name,++mempool_index_g);
            break;
         }
      }
      CNTLR_RCU_READ_LOCK_RELEASE();

      mempool_p->total_blocks_count = pool_params_p->max_blocks; /*static_blocks;*/
      mempool_p->block_size  = pool_params_p->block_size;

      CNTLR_LOCK_INIT(mempool_p->mempool_lock);
      mempool_p->lock_b = TRUE;

      CNTLR_RCU_READ_LOCK_TAKE();
      OF_LIST_SCAN(cntlr_instnace.mempool_list,mempool_info, cntlr_mempool_info_t *,offset)
      {
         if (pool_params_p->mempool_type == mempool_info->mempool_type)
         {
            mempool_found_b= TRUE;
            break;
         }
      }
      if ( mempool_found_b == TRUE)
      {
         retval = mempool_info->mempool_callbacks->mempool_create (pool_name, pool_params_p,
               (void *) mempool_p);
         if (retval != MEMPOOL_SUCCESS)
         {
            OF_LOG_MSG (OF_LOG_UTIL, OF_LOG_ERROR, "mempool create callback for memtype %d returned failure %d",mempool_info->mempool_type,  retval);
            free(mempool_p);
            status=retval;
            CNTLR_RCU_READ_LOCK_RELEASE();
            break;
         }
      }
      else
      {
         OF_LOG_MSG (OF_LOG_UTIL, OF_LOG_ERROR,"mempool type %d not registered", mempool_p->mempool_type );
         free(mempool_p);
         status=MEMPOOL_FAILURE;
         CNTLR_RCU_READ_LOCK_RELEASE();
         break;
      }
      CNTLR_RCU_READ_LOCK_RELEASE();

      CNTLR_LOCK_TAKE(mempool_lock_g);
      CNTLR_SLLQ_APPEND_NODE (&mempool_list_g, (struct cntlr_sll_q_node *)mempool_p);
      CNTLR_LOCK_RELEASE(mempool_lock_g);
	*mempool_pp=mempool_p;

   }while(0);
   return MEMPOOL_SUCCESS;
}

/***************************************************************************/
int32_t mempool_delete_pool(void* pool_p)
{
   struct  memory_pool*  mempool_p = NULL;
   cntlr_mempool_info_t *mempool_info;
   int8_t mempool_found_b=TRUE;
   int8_t offset=CNTLR_MEMPOOL_LISTNODE_OFFSET;
   int32_t                 status = MEMPOOL_SUCCESS;
   int32_t                 retval;

   OF_LOG_MSG (OF_LOG_UTIL, OF_LOG_DEBUG, "entered");

   do
   {
      if(pool_p == NULL)
      {
         OF_LOG_MSG (OF_LOG_UTIL, OF_LOG_ERROR, "invalid input");
         status= MEMPOOL_ERROR_NULL_MEMPOOL_PASSED;
         break;
      }
      mempool_p = (struct memory_pool *)pool_p;

      CNTLR_LOCK_TAKE(mempool_lock_g);
      CNTLR_SLLQ_DELETE_NODE (&mempool_list_g, (struct cntlr_sll_q_node *) mempool_p);
      CNTLR_LOCK_RELEASE(mempool_lock_g);

      CNTLR_RCU_READ_LOCK_TAKE();
      OF_LIST_SCAN(cntlr_instnace.mempool_list,mempool_info, cntlr_mempool_info_t *,offset)
      {
         if (mempool_p->mempool_type == mempool_info->mempool_type)
         {
            mempool_found_b= TRUE;
            break;
         }
      }

      if ( mempool_found_b == TRUE)
      {
         retval = mempool_info->mempool_callbacks->mempool_delete (mempool_p);
         if (retval != MEMPOOL_SUCCESS)
         {
            OF_LOG_MSG (OF_LOG_UTIL, OF_LOG_ERROR, "mempool delete callback for memtype %d returned failure %d",mempool_info->mempool_type,  retval);
            free(mempool_p);
            status=retval;
            CNTLR_RCU_READ_LOCK_RELEASE();
            break;
         }
      }
      else
      {
         OF_LOG_MSG (OF_LOG_UTIL, OF_LOG_ERROR,"mempool type %d not registered", mempool_p->mempool_type );
         free(mempool_p);
         status=MEMPOOL_FAILURE;
         CNTLR_RCU_READ_LOCK_RELEASE();
         break;
      }
      CNTLR_RCU_READ_LOCK_RELEASE();
   }while(0);
   return status;
}

/***********************************************************************************************/
int32_t PSP_PKTPROC_FUNC mempool_get_mem_block(void* pool_p,
      uchar8_t **mem_block_pp,
      uint8_t* heap_b_p)

{
   struct memory_pool* mempool_p = (struct memory_pool *)pool_p;
   uchar8_t  *block_p = NULL, *temp_block_p = NULL;
   uchar8_t  thread_num = 0;
   uint32_t  ii,count;
   cntlr_mempool_info_t *mempool_info;
   int8_t mempool_found_b=TRUE;
   int8_t offset=CNTLR_MEMPOOL_LISTNODE_OFFSET;
   int32_t                 status = OF_SUCCESS;
   int32_t                 retval = OF_SUCCESS;

   *mem_block_pp = NULL;

   do
   {
      /* Check if mempool is valid */
      if(CNTLR_UNLIKELY(!mempool_p))
      {
         status=MEMPOOL_ERROR_NULL_MEMPOOL_PASSED;
         break;
      }

      CNTLR_RCU_READ_LOCK_TAKE();
      OF_LIST_SCAN(cntlr_instnace.mempool_list,mempool_info, cntlr_mempool_info_t *,offset)
      {
         if (mempool_p->mempool_type == mempool_info->mempool_type)
         {
            mempool_found_b= TRUE;
            break;
         }
      }
      if ( mempool_found_b == TRUE)
      {
         retval = mempool_info->mempool_callbacks->mempool_get_mem_block (pool_p, mem_block_pp, heap_b_p); 
         if (retval != MEMPOOL_SUCCESS)
         {
            OF_LOG_MSG (OF_LOG_UTIL, OF_LOG_ERROR, "mempool get block callback for memtype %d returned failure %d",mempool_info->mempool_type,  retval);
            status=retval;
            CNTLR_RCU_READ_LOCK_RELEASE();
            break;
         }
      }
      else
      {
         OF_LOG_MSG (OF_LOG_UTIL, OF_LOG_ERROR,"mempool type %d not registered", mempool_p->mempool_type );
         status=MEMPOOL_FAILURE;
         CNTLR_RCU_READ_LOCK_RELEASE();
         break;
      }
      CNTLR_RCU_READ_LOCK_RELEASE();
   }while(0);
   return status;
}

/***********************************************************************************************/
int32_t mempool_release_mem_block(void*     pool_p,
      uchar8_t* mem_block_p,
      uchar8_t  heap_b)

{
   struct memory_pool* mempool_p = (struct memory_pool *)pool_p;
   uchar8_t thread_num = 0;
   cntlr_mempool_info_t *mempool_info;
   int8_t mempool_found_b=TRUE;
   int8_t offset=CNTLR_MEMPOOL_LISTNODE_OFFSET;
   int32_t                 status = OF_SUCCESS;
   int32_t                 retval = OF_SUCCESS;

   do
   {
      /* Check if mempool is valid */
      if(CNTLR_UNLIKELY(!mempool_p))
      {
         status=MEMPOOL_ERROR_NULL_MEMPOOL_PASSED;
         break;
      }

      if(CNTLR_UNLIKELY(mem_block_p == NULL))
      {
         CNTLR_LOCK_TAKE(mempool_p->mempool_lock);
         mempool_p->released_fail_blocks_count++;
         CNTLR_LOCK_RELEASE(mempool_p->mempool_lock);
         status= MEMPOOL_ERROR_NULL_MEMBLOCK_PASSED;
         break;
      }
      CNTLR_RCU_READ_LOCK_TAKE();
      OF_LIST_SCAN(cntlr_instnace.mempool_list,mempool_info, cntlr_mempool_info_t *,offset)
      {
         if (mempool_p->mempool_type == mempool_info->mempool_type)
         {
            mempool_found_b= TRUE;
            break;
         }
      }
      if ( mempool_found_b == TRUE)
      {
         retval = mempool_info->mempool_callbacks->mempool_release_mem_block (pool_p, mem_block_p, heap_b);
         if (retval != MEMPOOL_SUCCESS)
         {
            OF_LOG_MSG (OF_LOG_UTIL, OF_LOG_ERROR, "mempool release block callback for memtype %d returned failure %d",mempool_info->mempool_type,  retval);
            status=retval;
            CNTLR_RCU_READ_LOCK_RELEASE();
            break;
         }
      }
      else
      {
         status=MEMPOOL_FAILURE;
         CNTLR_RCU_READ_LOCK_RELEASE();
         break;
      }
      CNTLR_RCU_READ_LOCK_RELEASE();
   }while(0);
   return status;

}

/***************************************************************************
 * Function Name : mempool_get_stats
 * Description   : Used to get statistics of memory pool
 * Input         : pool_p  - mem pool pointer
 *                 stats_p - pointer to statistics structure
 * Output        : None
 * Return value  : MEMPOOL_SUCCESS/MEMPOOL_ERROR_NULL_MEMPOOL_PASSED
 ***************************************************************************/
int32_t mempool_get_stats(void* pool_p,struct mempool_stats* stats_p)
{
   struct memory_pool* mempool_p = NULL;
   mempool_p = (struct memory_pool *)pool_p;

   if(mempool_p == NULL)
      return MEMPOOL_ERROR_NULL_MEMPOOL_PASSED;

   stats_p->free_blocks_count                  = mempool_p->free_blocks_count;
   stats_p->allocated_blocks_count             = mempool_p->allocated_blocks_count;
   stats_p->released_blocks_count              = mempool_p->released_blocks_count;
   stats_p->allocation_fail_blocks_count       = mempool_p->allocation_fail_blocks_count;
   stats_p->released_fail_blocks_count         = mempool_p->released_fail_blocks_count;
   stats_p->heap_free_blocks_count             = mempool_p->heap_free_blocks_count;
   stats_p->heap_allocated_blocks_count        = mempool_p->heap_allocated_blocks_count;
   stats_p->heap_released_blocks_count         = mempool_p->heap_released_blocks_count;
   stats_p->heap_allocation_fail_blocks_count  = mempool_p->heap_allocation_fail_blocks_count;
   stats_p->heap_allowed_blocks_count          = mempool_p->heap_allowed_blocks_count;
   stats_p->total_blocks_count                 = mempool_p->total_blocks_count;
   stats_p->block_size                         = mempool_p->block_size;
   stats_p->threshold_min                      = mempool_p->threshold_min;
   stats_p->threshold_max                      = mempool_p->threshold_max;
   stats_p->replenish_count                    = mempool_p->replenish_count;
   stats_p->static_allocated_blocks_count      = mempool_p->static_allocated_blocks_count;
   stats_p->static_blocks                      = mempool_p->static_blocks;
   strcpy(stats_p->mempool_name, mempool_p->mempool_name);
   return (MEMPOOL_SUCCESS);
}

/***************************************************************************
 * Function Name : mempool_get_first_stats
 * Description   : Used to get first statistics of memory pool
 * Input         : pool_p  - mem pool pointer
 *                 stats_p - pointer to statistics structure
 * Output        : None
 * Return value  : MEMPOOL_SUCCESS/MEMPOOL_ERROR_NULL_MEMPOOL_PASSED
 ***************************************************************************/
int32_t mempool_get_first_stats(struct mempool_stats* stats_p)
{
   struct memory_pool *mempool_p = NULL;

   CNTLR_RCU_READ_LOCK_TAKE();
   CNTLR_SLLQ_SCAN(&mempool_list_g, mempool_p, struct memory_pool *)
   {
      break;
   }
   CNTLR_RCU_READ_LOCK_RELEASE();

   return mempool_get_stats(mempool_p, stats_p);

}

/***************************************************************************
 * Function Name : mempool_get_next_stats
 * Description   : Used to get next statistics of memory pool
 * Input         : pool_p  - mem pool pointer
 *                 stats_p - pointer to statistics structure
 * Output        : None
 * Return value  : MEMPOOL_SUCCESS/MEMPOOL_ERROR_NULL_MEMPOOL_PASSED
 ***************************************************************************/
int32_t mempool_get_next_stats(char *mempool_name, struct mempool_stats* stats_p)
{
   struct memory_pool *mempool_p = NULL;
   char mempool_found =0;

   CNTLR_RCU_READ_LOCK_TAKE();
   CNTLR_SLLQ_SCAN(&mempool_list_g, mempool_p, struct memory_pool *)
   {
      if(!mempool_found)
      {
         if(!strcmp(mempool_p->mempool_name, mempool_name))
         {
            mempool_found = 1;       
         }
      }
      else if(mempool_found == 1)
         break;
   }             

   CNTLR_RCU_READ_LOCK_RELEASE();

   return mempool_get_stats(mempool_p, stats_p);

}

/***************************************************************************
 * Function Name : memory_alloc_chain_memchunks
 * Description   : Allocates memory chunks and chains the memory blocks
 ***************************************************************************/
uchar8_t* memory_alloc_chain_memchunks(void * pool,
      uint32_t blocks,
      uint32_t block_size,
      uint32_t *alloc_cnt_p,
      uint32_t alignment,
      uchar8_t **tail_p_p)
{
   struct memory_pool *mempool_p=(struct memory_pool*)pool;
   uchar8_t *head_p = NULL, *mem_block_p = NULL, *prev_p = NULL;
   uint32_t   alloc_cnt = 0;
   int32_t    ii = 0,retval;

#ifdef CNTLR_DSMEMPOOL_STATIC_DEBUG
   printf("\r\n NSM Block size is increased");

   block_size += MEMP_STATIC_PRE_SIGNATURE_BYTES;
   block_size += MEMP_STATIC_POST_SIGNATURE_BYTES;
   block_size += 12;/*These 12 bytes are used to store 3 levels of function names
            when a memory block is released */
#endif

   /* Allocate and Chain the free pool structs in a singly linked list */
   for (ii=0; ii < blocks; ii++)
   {
      if(alignment)
      {
         retval  = posix_memalign((void**)&mem_block_p,alignment, block_size);
         if(retval)
         {
            mem_block_p = NULL;
         }  
      }
      else
      {
         mem_block_p = (uchar8_t*) calloc (1, block_size);
      }

      if(mem_block_p == NULL)
      {
         /* mem allocation fail */
         if(prev_p)
         {
            /* some memory blocks are allocated, hence update the statistics
             * and return the chunk head pointer
             */
            ((struct mem_link_node *)prev_p)->next_p = NULL;
            *alloc_cnt_p = alloc_cnt;
         }
         *tail_p_p = prev_p;
         return (head_p);
      }
#ifdef CNTLR_DSMEMPOOL_STATIC_DEBUG
      mem_block_p += MEMP_STATIC_PRE_SIGNATURE_BYTES;
      MEMP_SET_PRE_SIGNATURE(mem_block_p,MEMP_INACTIVE_PRE_SIGNATURE);
      MEMP_SET_POST_SIGNATURE(mem_block_p,MEMP_INACTIVE_POST_SIGNATURE,mempool_p);
      memset(mem_block_p+4,MEMP_DATA_EXP,(mempool_p->block_size - 4)) ;


#endif
      alloc_cnt++;
      if(prev_p == NULL)
      {
         head_p = prev_p = mem_block_p;
         continue;
      }
      ((struct mem_link_node *) prev_p)->next_p = mem_block_p;
      prev_p = mem_block_p;
   }
   if(mem_block_p)
   {
      /* this condition fails, when ulBlocks is zero */
      ((struct mem_link_node *) mem_block_p)->next_p = NULL;
      *alloc_cnt_p = alloc_cnt;
   }
   *tail_p_p = prev_p;
   return (head_p);
}

/*************************************************************************/
int32_t set_mempool_attributes(void*    pool_p,
      uint8_t  b_list_per_thread_required,
      uint32_t noof_blocks_to_move_to_thread_list,
      uint8_t  b_memset_not_required)
{
   struct memory_pool* mempool_p = NULL;

   mempool_p = (struct memory_pool *)pool_p;
   if(b_memset_not_required)
   {
      mempool_p->flags |= CNTLR_MEMPOOL_DONOT_MEMSET;
   }
   if(b_list_per_thread_required)
   {
      mempool_max_threads_g = CNTLR_NO_OF_THREADS_IN_SYSTEM + 1;
      mempool_p->per_thread_list_p = calloc(mempool_max_threads_g,sizeof(struct per_thread_mem_node));
      if(mempool_p->per_thread_list_p == NULL)
      {
         mempool_delete_pool(mempool_p);
         return MEMPOOL_ERROR_PER_THREAD_MEM_ALLOCATION_FAILED;
      }
      mempool_p->block_count_for_moving_to_threadlist = noof_blocks_to_move_to_thread_list;
   }
   return MEMPOOL_SUCCESS;
}

/***************************************************************************
 * Function Name : mempool_check_and_refill
 * Description   : Checks whether refill is possible or not based on statistics
 *                 [NOTE]: This function is assumed to be called after
 *                 taking Lock for the memory pool
 * Input         : pMemPool - mempool pointer.
 * Output        : None
 * Return value  : Number of nodes can be refilled
 ***************************************************************************/
//static inline int32_t mempool_check_and_refill(struct memory_pool* mempool_p)
int32_t mempool_check_and_refill(void *pool)
{
   struct memory_pool* mempool_p=(void *)pool;
   uint32_t mem_chunk = 0;
   uchar8_t* head_p    = NULL;
   uchar8_t* tail_p    = NULL;
   uint32_t alloc_cnt = 0;

   if(mempool_p->free_blocks_count >= mempool_p->threshold_max)
   {
      /* already we have maximum threshold number of nodes in the pool,
       * no need of allocating nodes */
      return MEMPOOL_FAILURE;
   }

   mem_chunk = ((mempool_p->replenish_count +
            mempool_p->static_allocated_blocks_count) <=
         mempool_p->static_blocks /* total_blocks_count */)
      ? mempool_p->replenish_count
      : (mempool_p->static_blocks /* total_blocks_count */ -
            mempool_p->static_allocated_blocks_count);

   if(mem_chunk)
   {  
      /* now see, whether this crosses the maximum threshold */
      mem_chunk = ((mempool_p->free_blocks_count + mem_chunk) >
            mempool_p->threshold_max)
         ? (mempool_p->threshold_max - mempool_p->free_blocks_count)
         : mem_chunk;
   } 
   else
      return MEMPOOL_FAILURE; 

   head_p = memory_alloc_chain_memchunks(mempool_p,mem_chunk,
         mempool_p->block_size,
         &alloc_cnt,0,&tail_p);
   if(head_p == NULL)
   {
      /* since lock is given above, there is a chance that this
       * memory pool can be set for deletion, hence check for it
       */
      /* mem allocation fail */
      return MEMPOOL_FAILURE;
   }

   /* Update the counters with real ulAllocCnt */
   mempool_p->free_blocks_count += alloc_cnt;
   mempool_p->static_allocated_blocks_count += alloc_cnt;

   /* append to the existing tail node */
   if(CNTLR_LIKELY(mempool_p->tail_p))
      ((struct mem_link_node *)(mempool_p->tail_p))->next_p = head_p;
   else
      mempool_p->head_p = head_p;

   mempool_p->tail_p = tail_p;

   return (MEMPOOL_SUCCESS);
}

/***************************************************************************
 * Function Name : mempool_set_threshold_values
 * Description   : Sets the controllable values for a memory pool
 * Input         : pool_p          - memory pool for which values to be set
 *                 threshold_min   - mimimum threshold value
 *                 threshold_max   - maximum threshold value
 *                 replenish_count - replenished node count
 * Output        : None
 * Return value  : MEMPOOL_SUCCESS/MEMPOOL_FAILURE
 ***************************************************************************/
int32_t mempool_set_threshold_values(void*    pool_p,
      uint32_t threshold_min,
      uint32_t threshold_max,
      uint32_t replenish_count)
{
   struct memory_pool* mempool_p = (struct memory_pool*)pool_p;

   CNTLR_LOCK_TAKE(mempool_p->mempool_lock);

   /* validate the values given */
   if((threshold_max == 0) ||
         (threshold_max < threshold_min) ||
         (threshold_max < replenish_count) ||
         (threshold_max > mempool_p->total_blocks_count))
   {
      CNTLR_LOCK_RELEASE(mempool_p->mempool_lock);
      return (MEMPOOL_FAILURE);
   }

   mempool_p->threshold_min   = threshold_min;
   mempool_p->replenish_count = replenish_count;

   /* Static blocks are allocated after all modules are initialized.
      So adding this mempool to the Refill list. 
    */
   mempool_p->threshold_max = threshold_max;

   /* set the bitmap */
   mempool_p->bit_map |= MEMP_REFILL_PENDING;
   mempool_p->bit_map |= MEMP_INIT_TIME_REFILL;

   CNTLR_LOCK_RELEASE(mempool_p->mempool_lock);
   return MEMPOOL_SUCCESS;
}

