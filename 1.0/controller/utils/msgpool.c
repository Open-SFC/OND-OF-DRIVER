/**
 *  Copyright (c) 2012, 2013  Freescale.
 *   
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 **/
/*
 * File:        of_msgpool.c
 * 
 * Description: This file  provides message pool APIs that used  to creates pools  of
 *              different sizes. 
 *
 * Authors:     Rajesh Madabushi <rajesh.madabushi@freescale.com>
 *
 */

/* INCLUDE_START */
#include "controller.h"
#include "of_msgpool.h"
#include "oflog.h"
/* INCLUDE_END */


    
uint32_t allocated_count[10]={};
uint32_t released_count[10]={};

//#define of_debug printf

/** Create and initalize message pools of different sizes as defined in msg_pool input.
 * The same 'msg_pool' needs to pass to subsequent alloc and free calls.
 */
int32_t
of_msg_pool_init(of_msg_pool_t *msg_pool)
{
   uint32_t                ii;
   uint32_t                kk;
   char                    msg_buf_pool_name[25];
   uint32_t                max_static_msgs;
   of_msgbuf_pool_entry_t *msg_buf_entry_ptr;
   int32_t                retval=OF_SUCCESS;
   int32_t number_of_blocks;
   struct mempool_params mempool_info={};

   /*Creating memory pools of different sizes for actual OpenFlow Messages*/
   for(ii = 0 ; ii < msg_pool->total_number_of_msg_pools; ii++)
   {
	   msg_buf_entry_ptr = (msg_pool->pool_entry + ii);
	   sprintf(msg_buf_pool_name,"of_MsgBuf_pool_%d",ii);
	   number_of_blocks=(msg_buf_entry_ptr->number_of_blocks) * (CNTLR_NO_OF_THREADS_IN_SYSTEM + 1);
	   max_static_msgs = (number_of_blocks*3)/4;;
	   OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_DEBUG," pos %d MAX blocks %d static blocks %d Block size %d",ii,number_of_blocks,max_static_msgs, msg_buf_entry_ptr->block_size); 

           memset(&mempool_info, 0, sizeof(struct mempool_params));
	   mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;  
	   mempool_info.block_size = (sizeof(of_msg_t) + msg_buf_entry_ptr->block_size);
	   mempool_info.max_blocks = number_of_blocks;
	   mempool_info.static_blocks = max_static_msgs;
	   mempool_info.threshold_min = max_static_msgs/10;
	   mempool_info.threshold_max = max_static_msgs/3;
	   mempool_info.replenish_count = max_static_msgs/10;
	   mempool_info.b_memset_not_required =  FALSE;
	   mempool_info.b_list_per_thread_required =  TRUE;
	   mempool_info.noof_blocks_to_move_to_thread_list = max_static_msgs/10;

	   retval = mempool_create_pool(msg_buf_pool_name, &mempool_info,
			   &msg_buf_entry_ptr->mem_pool_id);
	   if(retval != MEMPOOL_SUCCESS)
	   {
		   OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_DEBUG,"%s:%d Create mempool failure Message mem pools, retval=%d \r\n",
				   __FUNCTION__,__LINE__,retval);
		   mempool_delete_pool( msg_buf_entry_ptr->mem_pool_id);
		   for(kk = 0 ; kk < ii; kk++)
		   {
			   mempool_delete_pool(msg_pool->pool_entry[kk].mem_pool_id);
		   }
		   return OF_FAILURE;
	   }
   }

   return OF_SUCCESS;
}

inline int32_t 
of_msg_alloc(of_msg_pool_t *msg_pool,
             uint8_t        pool_index,
             uint32_t       size, 
             of_msg_t    **p_p_msg)
{
   of_msg_t               *msg;
   uint8_t                 is_heap;
   of_msg_pool_pvt_info_t *pool_pvt_info;

#if 0
   uint8_t     *buffer;

   buffer = (uint8_t*)calloc(1,2048);
   if(buffer == NULL)
   {
      printf("Alloc error \r\n");
      return OF_FAILURE;
   }
   msg = (of_msg_t*)buffer;
   *p_p_msg = msg;
   return OF_SUCCESS;
#endif

   if(mempool_get_mem_block(msg_pool->pool_entry[pool_index].mem_pool_id,
                            (unsigned char  **)&msg,
                            (unsigned char *)&is_heap) != OF_SUCCESS)
   {
       OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_DEBUG," Message Buffer poolindex %d memory alloc size %d failure",pool_index,size);
       return OF_FAILURE;
   }

    pool_pvt_info                   = &msg->pool_pvt_info;
    pool_pvt_info->ref_cnt          = 1;
    pool_pvt_info->is_heap          = is_heap;
    pool_pvt_info->mem_pool_id      = msg_pool->pool_entry[pool_index].mem_pool_id;
    pool_pvt_info->pool_index      =  pool_index;
    pool_pvt_info->buf_len          = (msg_pool->pool_entry + pool_index)->block_size;

    *p_p_msg = msg;

    allocated_count[pool_index]++;
    //OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_DEBUG,"PoolIndex=%d  allocated_count=%d\r\n",pool_index,allocated_count[pool_index]);
    return OF_SUCCESS;
}

/*Release message to pool*/
inline void 
of_msg_release(of_msg_t *msg)
{
#if 0
   free((void*)msg);
#endif
   msg->pool_pvt_info.ref_cnt--;
   if(msg->pool_pvt_info.ref_cnt == 0)
   {
     released_count[msg->pool_pvt_info.pool_index]++;
     //OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_DEBUG,"Poolindex=%d released_count=%d\r\n",msg->pool_pvt_info.pool_index,released_count[msg->pool_pvt_info.pool_index]);
     mempool_release_mem_block (msg->pool_pvt_info.mem_pool_id,
                             (unsigned char*)msg,
                             msg->pool_pvt_info.is_heap);
   }
}

/*Just increase refence and return the same message,
 * to avoid copy in the message dup*/
inline  void
of_msg_dup(of_msg_t *p_org_msg, 
           of_msg_t **p_p_dup_msg)
{
    p_org_msg->pool_pvt_info.ref_cnt++;
   
   *p_p_dup_msg = p_org_msg;
}
