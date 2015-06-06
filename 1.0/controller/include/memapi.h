#ifndef __MEMAPI_H
#define __MEMAPI_H

/* 
 *
 * Copyright  2012, 2013  Freescale Semiconductor
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
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
 * File name:  memapi.h
 * Author: Freescale Semiconductor    
 * Date:   01/20/2013
 * Description: This file contains API functions implementing memory pool functionality. 
 * API allows Applications to create mempools of any memory block size.
 * Each mempool can be configured to maintain thread specific lists of memblocks to avoid locking.
 * API to allocate and release memblocks from/to a given mempool is supported.
 * API to get statistics of memory blocks for a given mempool is supported. 
 */

//!  Multicore Mempool database API
/*! This module provides DP resource APIs
 * \addtogroup Memory_Management
 */

/** \ingroup Memory_Management
  * MEMPOOL_MAX_POOL_NAME_LEN
  * - Maximum length of the mempool name that is allowed.
 */
#define MEMPOOL_MAX_POOL_NAME_LEN  32

/** \ingroup Error_Management
  * MEMPOOL_SUCCESS
  * - MEMPOOL API return this value on successful execution.
 */
#define MEMPOOL_SUCCESS 0

/** \ingroup Error_Management 
  * MEMPOOL_FAILURE
  * - MEMPOOL API return this value on general failure in execution.
 */
#define MEMPOOL_FAILURE -1

/** \ingroup Error_Management 
  * MEMPOOL_ERROR_NULL_MEMPOOL_PASSED  
  * - NULL mempool pointer is passed to API.
  */
#define MEMPOOL_ERROR_NULL_MEMPOOL_PASSED -60 

/** \ingroup Error_Management 
  * MEMPOOL_ERROR_NULL_MEMBLOCK_PASSED
  * - memblock passed for release is NULL.
  */
#define MEMPOOL_ERROR_NULL_MEMBLOCK_PASSED  -61

/** \ingroup Error_Management 
  * MEMPOOL_ERROR_MEM_ALLOCATION_FROM_HEAP_FAILURE
  * - Memory block allocation from heap memory is failed.
  */
#define MEMPOOL_ERROR_MEM_ALLOCATION_FROM_HEAP_FAILURE -62

/** \ingroup Error_Management 
  * MEMPOOL_ERROR_PER_THREAD_MEM_ALLOCATION_FAILED
  * - Thread specific Memory block allocation failed.
  */
#define MEMPOOL_ERROR_PER_THREAD_MEM_ALLOCATION_FAILED -63

/** \ingroup Error_Management 
  * MEMPOOL_ERROR_BLOCKSIZE_TOO_SMALL
  * - Size of memory block passed to mempool_create_pool API is too small.
  */
#define MEMPOOL_ERROR_BLOCKSIZE_TOO_SMALL  -64

/** \ingroup Error_Management 
  * MEMPOOL_ERROR_NO_MEM
  * - Memory allocation for mempool creation failed.
  */
#define MEMPOOL_ERROR_NO_MEM  -65

/** \ingroup Error_Management
  * MEMPOOL_ERROR_INVALID_MEMPOOL_NAME
  * - Mem pool name passed to mempool_create_pool API is invalid.
  */
#define MEMPOOL_ERROR_INVALID_MEMPOOL_NAME  -66

/** \ingroup Error_Management 
  *  MEMPOOL_ERROR_UNABLE_TO_ALLOCATE_MEMORY_CHUNK
  * - Failed to allocate required memory blocks during mempool creation.
 */
#define MEMPOOL_ERROR_UNABLE_TO_ALLOCATE_MEMORY_CHUNK  -67

/** \ingroup Error_Management 
  * MEMPOOL_ERROR_SETTING_THRESHOLD_VALUES
  * - Invalid threshold values are passed for the mempool.
  */
#define MEMPOOL_ERROR_SETTING_THRESHOLD_VALUES  -68
#define MEMPOOL_ERROR_INVALID_PARAMS -69

/* Default hugepage size should be a power of 2.
 * Assuming 4MB is the deault hugepage size.
 * <Revist> Default hugepage size should be obtained through an API
 * instead of macro.
 */
#define CNTLR_DEFAULT_HUGEPAGE_SIZE	(4 * 1024 *1024)


enum mem_pool_type {
        MEMPOOL_TYPE_HEAP = 0,
        MEMPOOL_TYPE_MMAP
};

struct mempool_params {
        uint8_t   mempool_type;
        uint32_t  block_size;
        uint32_t  max_blocks;
        uint8_t   b_memset_not_required;
        uint8_t   b_list_per_thread_required;
        uint32_t  noof_blocks_to_move_to_thread_list;
        uint32_t  static_blocks;
        uint32_t  threshold_min;
        uint32_t  threshold_max;
        uint32_t  replenish_count;
};


/** \ingroup Memory_Management
 \struct mempool_stats
 \brief This structure is used to pass mempool statistics to Applications\n
 <b>Description </b>\n
 It contains various statistics counters listing the free/allocated/total memory blocks for a given memory pool
 */
 struct mempool_stats{
   /** Number of memory blocks available for Application usage */
   uint32_t  free_blocks_count;                  
   /** Number of memory blocks allocated to Application */
   uint32_t  allocated_blocks_count;            
   /** Number of memory blocks released by Application */
   uint32_t  released_blocks_count;            
   /** Number of failures in allocation of memory blocks */ 
   uint32_t  allocation_fail_blocks_count;    
   /** Number of failures in releasing memory blocks */ 
   uint32_t  released_fail_blocks_count;     
   /** Number of memory blocks that can still be allocated from heap */
   uint32_t  heap_free_blocks_count;        
   /** Number of memory blocks allocated from heap to Application */
   uint32_t  heap_allocated_blocks_count;  
   /** Number of memory blocks released by Application to heap */
   uint32_t  heap_released_blocks_count; 
   /** Number of failures in allocation of memory blocks from heap */
   uint32_t  heap_allocation_fail_blocks_count; 
   /** Number of memory blocks allocated from heap to Application */
   uint32_t  heap_allowed_blocks_count;      
   /** Total Number of memory blocks */  
   uint32_t  total_blocks_count;              
   /** Number of static blocks configured */
   uint32_t  static_blocks;
   /** Size of each memory block */
   uint32_t  block_size;                     
   /** Number of memory blocks allocated from internal lists of mempool to Application.*/
   uint32_t  static_allocated_blocks_count;
   /** Maximum threshold value for the global list */
   uint32_t  threshold_max;
   /** Minimum threshold value for the global list */
   uint32_t  threshold_min;
   /** Replenish count for refilling the global list */
   uint32_t  replenish_count;
  char mempool_name[MEMPOOL_MAX_POOL_NAME_LEN + 1];
 };


struct mem_region_info {
        uint64_t        start_phys_addr;
        void            *start_user_va_p;
        void            *start_kern_va_p;
	uint32_t	mem_size;
};

/** \ingroup Memory_Management
  \brief This function creates mmap region.\n
  \return  MEMPOOL_SUCCESS - Successfully created the mmap region.
  \return  MEMPOOL_FAILURE - Failed to create the mmap region.
*/ 
int create_mmap_region (struct mem_region_info *mem_region_p,
                        int32_t char_drv_fd);

/** \ingroup Memory_Management
  \brief This function deletes mmap region.\n
  \return  MEMPOOL_SUCCESS - Successfully deleted the mmap region.
  \return  MEMPOOL_FAILURE - Failed to delete the mmap region.
*/ 
int delete_mmap_region (struct mem_region_info *mem_region_p,
                        int32_t char_drv_fd);

/** \ingroup Memory_Management
  \brief This function initializes the memory pool library.\n\n
  <b>Description</b>\n
  This Module init function is to be called by Applications before they use the other API.
  \return  MEMPOOL_SUCCESS - On successful initialization of the module.
  \return  MEMPOOL_FAILURE - Unsuccessful initialization of the module.
*/ 
 int32_t mempool_library_init(void);

/** \ingroup Memory_Management
  \brief This function creates a memory pool.\n 
  <b>Description:</b>\n
  This function creates a memory pool with a unique name. Size of each memory block, total number of memory blocks required by the Application and other associated input parameters as explained in parameter list.
 
  \param[in] pool_name        - Name of the memory pool
  \param[in] block_size       - Size of each memory block required
  \param[in] max_blocks       - Total Number of memory blocks required which include allocations from memory pool lists and from Heap.
  \param[in] static_blocks    - Maximum number of memory blocks allocated and managed by the mempool in its internal lists.
  \param[in] threshold_min    - Minimum number of free memory blocks maintained in freelists by mempool at a given time.         
  \param[in] threshold_max    - Maximum number of free memory blocks maintained in freelists of mempool at a given time.
  \param[in] replenish_count  - Number of memory blocks allocated for re-filling mempool freelists whenever they fall below threshold_min 
  \param[in] no_memset_required_b                  - When set, the released memblocks are not initialized with 0.
  \param[in] list_per_thread_required_b            - When set, a separate list of free memblocks is maintained for each thread.
  \param[in] noof_blocks_to_move_to_thread_list    - Number of memory blocks moved from common free list to per thread list when the later is empty.
  \param[out] mempool_p_p                          - Double pointer to return void pointer to the created mempool for later reference.  

  \return MEMPOOL_ERROR_INVALID_MEMPOOL_NAME             - Mempool name is not unique or is invalid.
  \return MEMPOOL_ERROR_BLOCKSIZE_TOO_SMALL              - Mempool block size is too small.
  \return MEMPOOL_ERROR_SETTING_THRESHOLD_VALUES         - Threshold values and replenish count value are not properly specified.
  \return MEMPOOL_ERROR_PER_THREAD_MEM_ALLOCATION_FAILED - Per thread lists of memory blocks could not be created. 
  \return MEMPOOL_ERROR_NO_MEM                           - No memory available for creating the mempool structure. 
  \return MEMPOOL_ERROR_UNABLE_TO_ALLOCATE_MEMORY_CHUNK  - Required memory blocks could not be allocated during mempool creation.   
  */
int32_t mempool_create_pool(char*     pool_name,
			    struct mempool_params *,
                            void**    mempool_p_p 
                           );
/** \ingroup Memory_Management
  \brief This function deletes a given memory pool.\n
  <b>Description:</b>\n
  Application is required to release all the allocated memory blocks before calling this API to free the memory pool.
  
  \param[in] pool_p                          - Handle of the memory pool to be deleted
  \return MEMPOOL_SUCCESS                    - Upon successful deletion.
  \return MEMPOOL_ERROR_NULL_MEMPOOL_PASSED  - In case of NULL mempool pointer passed.
  \return MEMPOOL_FAILURE                    - Upon failure in deletion.
 */
int32_t mempool_delete_pool(void* pool_p);

/** \ingroup Memory_Management
  \brief This function allocates one memory block to the Application from a given mempool.\n
  <b>Description:</b>\n
  \param[in]  pool_p         - Handle to the memory pool from which a memory block is to be allocated
  \param[out] mem_block_p_p  - Double pointer variable used to return the allocated memory block
  \param[out] heap_b_p       - Set by mempool module. Set to 1, if the memory block is allocated from heap. Set to 0, if allocated from internal lists.

  \return  MEMPOOL_SUCCESS   - In case of successful allocation of memory block
  \return  MEMPOOL_ERROR_NULL_MEMPOOL_PASSED               - In case of NULL mempool pointer passed.
  \return  MEMPOOL_ERROR_MEM_ALLOCATION_FROM_HEAP_FAILURE  - Memory from heap could not be allocated.
  \return  MEMPOOL_FAILURE   - In case of any other error conditions
 */
int32_t mempool_get_mem_block(void* pool_p,uchar8_t** mem_block_p_p,uint8_t* heap_b_p);

/** \ingroup Memory_Management
  \brief This function releases one memory block received from the Application to the given mempool.

  \param[in] pool_p       - Handle to the memory pool to which the memory block is to be released
  \param[in] mem_block_p  - Memory block to be released
  \param[in] heap_b       - Application shall pass this flag as it was received during the API call mempool_get_mem_block.

  \return  MEMPOOL_SUCCESS  - In case of successful release of memory block.
  \return  MEMPOOL_ERROR_NULL_MEMPOOL_PASSED  - In case of NULL mempool pointer passed.
  \return  MEMPOOL_ERROR_NULL_MEMBLOCK_PASSED - Memory block to be released is NULL.
  \return  MEMPOOL_FAILURE  - In case of any error condition.
*/
int32_t mempool_release_mem_block(void* pool_p,uchar8_t* mem_block_p,uchar8_t heap_b);

/** \ingroup Memory_Management
  \brief  This function returns statistics regarding creation, release of memory blocks for a given mempool

  \param[in]  pool_p   - Handle to the memory pool for which statistics is returned
  \param[out] stats_p  - Variable used to return statistics for which memory is to be assigned by Application  
 
  \return MEMPOOL_SUCCESS                    - In case of successful return of statistics
  \return MEMPOOL_ERROR_NULL_MEMPOOL_PASSED  - In case of NULL mempool pointer passed
*/
int32_t mempool_get_stats(void* pool_p,struct mempool_stats* stats_p);

/** \ingroup Memory_Management
  \brief  This function is used to get first statistics of memory pool

  \param[out] stats_p  - Pointer to returned statistics structure  
 
  \return MEMPOOL_SUCCESS                    - In case of successful return of statistics
  \return MEMPOOL_ERROR_NULL_MEMPOOL_PASSED  - In case of NULL mempool pointer passed
*/
int32_t mempool_get_first_stats(struct mempool_stats* stats_p);

/** \ingroup Memory_Management
  \brief  This function is used to get next statistics of memory pool

  \param[in] mempool_name  - Pointer to memory pool  
  \param[out] stats_p      - Pointer to returned statistics structure  
 
  \return MEMPOOL_SUCCESS                    - In case of successful return of statistics
  \return MEMPOOL_ERROR_NULL_MEMPOOL_PASSED  - In case of NULL mempool pointer passed
*/
int32_t mempool_get_next_stats(char *mempool_name, struct mempool_stats* stats_p);

/******************************************************************************
 * * * * * * * *  Registration Function Prototypes * * * * * * * * * * * * * * *
 ******************************************************************************/
typedef int32_t (*mempool_create_p) (char *,
      struct mempool_params *,
      void*    );

typedef int32_t (*mempool_delete_p)(void* );

typedef int32_t  (*mempool_get_mem_block_p)(void*,
		uchar8_t **,
		uint8_t* );

typedef int32_t (*mempool_release_mem_block_p)(void* ,
		uchar8_t* ,
		uchar8_t );

struct of_mempool_callbacks
{
	mempool_create_p  mempool_create;
	mempool_delete_p  mempool_delete;
	mempool_get_mem_block_p mempool_get_mem_block;
	mempool_release_mem_block_p mempool_release_mem_block;
};

int32_t set_mempool_attributes(void*    pool_p,
      uint8_t     b_list_per_thread_required,
      uint32_t noof_blocks_to_move_to_thread_list,
      uint8_t     b_memset_not_required);

int32_t mempool_set_threshold_values(void*    pool_p,
      uint32_t threshold_min,
      uint32_t threshold_max,
      uint32_t replenish_count);

int32_t mempool_check_and_refill(void * mempool_p);

uchar8_t* memory_alloc_chain_memchunks(void * mempool_p,
      uint32_t blocks,
      uint32_t block_size,
      uint32_t *alloc_cnt_p,
      uint32_t alignment,
      uchar8_t **tail_p_p);
#endif /**_MEMAPI_H*/
