/**
 *  Copyright (c) 2012, 2013  Freescale.
 *    
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *  
 *  http://www.apache.org/licenses/LICENSE-2.0
 * 
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
 /*
 * File:        of_msgpool.h
 * 
 * Description: This file contains definitions of data-structures of Open Flow
 *              message mempory pools.
 * Authors:     Rajesh Madabushi <rajesh.madabushi@freescale.com>
 **/

#ifndef __OF_MSGPOOL_H
#define __OF_MSGPOOL_H

#include "of_utilities.h"
//#include "of_msgapi.h"

/* DEFINE_START **************************************************************/

#define OF_MAX_MSG_ENTRIES          (100) 
#define OF_MSG_STATIC_CONN_ENTRIES   (OF_TRNSPRT_MAX_CONN_ENTRIES * 4)/5 /* 80 % */

#define CNTLR_MEM_SIZE_GRANULARITY  (128)
#define CNTLR_MEM_NUMBER_OF_APP_POOLS 8

/* DEFINE_END    *************************************************************/

/* TYPEDEF_START *************************************************************/

typedef struct of_msg_pool_pvt_info_s
{
  void         *mem_pool_id; /** Pool Id from which this message buffer was allocated*/
  uint32_t     ref_cnt; /** number of times same message buffer referred, used in message dup operation*/
  uint8_t      is_heap; /** Is memory allocated from heap ?*/
  uint8_t      pool_index; /*pool index from which the buffer was allocated*/ 
  uint16_t     buf_len; /** Length of allocated message buffer*/
}of_msg_pool_pvt_info_t;
/* Following data_structure used by msg pool*/
typedef struct of_msg_s
{
  of_msg_pool_pvt_info_t pool_pvt_info; /** Memory pool managed private info*/
  struct of_msg msg_desc; /** Open flow message with message descriptor and buffer contains message*/
} of_msg_t;

/*Following data-structure stores message pools private data
 * this need to remember as part of 'struct of_msg'
 * some room will be allocated for this
 */
typedef struct of_msg_buf_pvt_data_s
{
}of_msg_buf_pvt_data_t;

typedef struct of_msg_buf_pool_entry_s
{
   uint32_t    block_size;  /** Size of the buffer requested to allocate pool*/
   uint32_t    number_of_blocks; /** Number of buffers to allocate for  the requested size */
   void       *mem_pool_id; /** Pool ID of used by mempool library */
}of_msgbuf_pool_entry_t;

/** Pool handle, contains details of pools of different sizes, used 
 * input paramter for init pools, alloc message buffer.
 */
typedef struct of_msg_pool_s
{ 
   uint32_t                total_number_of_msg_pools;/** Total number of messager buffer wants to create*/
   of_msgbuf_pool_entry_t *pool_entry; /** Details of each of message buffer pool size*/
}of_msg_pool_t;

/** Memory pool group data structure.
 * 
 */
typedef struct of_msg_mem_pool_grp_s
{ 
   uint32_t    number_of_apps_per_pool;/** Total number of applications */
   uint32_t    granularity;  /** Size of the buffer requested to allocate pool*/
   uint32_t    start_block;
}of_msg_mem_pool_grp_t;


/** Following data_structure help in configuring pools of different sizes*/
typedef struct of_msg_pool_cfg_s
{
   uint32_t   number;/** Number of buffers wants to create of requested size*/ 
} of_msg_pool_cfg_t;

typedef uint32_t  cntlr_mem_app_pool_size_in_bytes_t;
typedef uint32_t  cntlr_mem_pool_grp_granularity_t;

/* TYPEDEF_END  *************************************************************/

/* FUNCTION_PROOTYPE_START  *************************************************/
int32_t of_msg_pool_init(of_msg_pool_t *msg_pool);
inline int32_t of_msg_alloc(of_msg_pool_t *msg_pool, uint8_t pool_index,uint32_t size,of_msg_t **p_p_msg);
inline void of_msg_release(of_msg_t *p_msg);
inline  void of_msg_dup(of_msg_t *p_org_msg, of_msg_t **p_p_dup_msg);
/* FUNCTION_PROOTYPE_END    *************************************************/
#endif /*OF_MSGPOOL_H*/
