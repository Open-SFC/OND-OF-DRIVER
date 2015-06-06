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

/******************************************************************************
 * * * * * * * * * * * * Function Prototypes * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
extern int mempool_create_mheap_pool (char *pool_name,
      struct mempool_params *pool_params_p,
      void*    mempool_p);

extern int32_t mempool_delete_mheap_pool(void* pool_p);

extern int32_t PSP_PKTPROC_FUNC mempool_mheap_get_mem_block(void* pool_p,
      uchar8_t **mem_block_p_p,
      uint8_t* heap_b_p);

extern int32_t mempool_mheap_release_mem_block(void*     pool_p,
      uchar8_t* mem_block_p,
      uchar8_t  heap_b);

struct of_mempool_callbacks  mheap_callbacks=
{
   mempool_create_mheap_pool,
   mempool_delete_mheap_pool,
   mempool_mheap_get_mem_block,
   mempool_mheap_release_mem_block
};


/******************************************** **********************************
 * * * * * * * F U N C T I O N   D E F I N I T I O N S* * * * * * * * * * * * *
 ******************************************************************************/
#ifdef CNTRL_INFRA_LIB_SUPPORT
void cntlr_shared_lib_init()
{
   int32_t status=OF_SUCCESS;
   int32_t ret_val=OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_DEBUG,"Loading MHeap Infra Library");

   do
   {
      /** Initialize NEM Infrastructure */
      ret_val=of_mheap_init();
      if(ret_val != OF_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_DEBUG, "MHeap Infra initialization failed");
         status= OF_FAILURE;
         break;
      }

   }while(0);

   return;
}
#endif




int32_t of_mheap_init( )
{
   int32_t status=OF_SUCCESS;
   int32_t ret_val=OF_SUCCESS;

   OF_LOG_MSG (OF_LOG_UTIL, OF_LOG_DEBUG,"entered");

   do
   {
      ret_val = register_mheap_with_cntlr();
      if(ret_val != OF_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_ERROR,"registartion with mmp failed");
         status = OF_FAILURE;
         break;
      }

   }while(0);
   return status;
}

int32_t register_mheap_with_cntlr()
{
   int32_t ret_val = OF_SUCCESS;
   int32_t status = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_DEBUG,"entered");
   do
   {
      ret_val = cntlr_register_memory_pool( MEMPOOL_TYPE_HEAP, &mheap_callbacks);
      if(ret_val != OF_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_ERROR,"registration memory pool - Failure");
         status = OF_FAILURE;
         break;
      }

   }while(0);

   return status;
}


