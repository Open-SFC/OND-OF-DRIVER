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
 * * * * * * * * * * * * Global Variables * * * * * * * * * * * * * * * * * *
 *******************************************************************************/

extern cntlr_instance_t cntlr_instnace; 

/******************************************************************************
 * * * * * * * * * * * * Function Definitions * * * * * * * * * * * * * * * * * 
 *******************************************************************************/
int32_t cntlr_register_memory_pool(int32_t mempool_type,
      struct of_mempool_callbacks *mempool_callbacks_p)
{
   cntlr_mempool_info_t   *mempool_info_p;
   int32_t                 status = OF_SUCCESS;
   int32_t                 ret_val = OF_SUCCESS;
   uchar8_t offset;

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

      offset = CNTLR_MEMPOOL_LISTNODE_OFFSET;
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

