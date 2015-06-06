
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

/* File  :      tranport_init.c
 * Author:      Atmaram G <atmaram.g@freescale.com>
 * Date:   08/23/2013
 * Description: 
 */



/******************************************************************************
 * * * * * * * * * * * * Include Files * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
#include "controller.h"
#include "cntlr_epoll.h"
#include "cntrl_queue.h"
#include "of_utilities.h"
#include "of_msgapi.h"
#include "of_multipart.h"
#include "dprm.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "cntlr_event.h"
#include "dprmldef.h"
#include "cntlr_xtn.h"
#include "timerapi.h"
#include "of_proto.h"



typedef int32_t (*CNTLR_PROTO_INIT_HOOK)();
/******************************************************************************
 * * * * * * * * * * * * Global Variables * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/

extern cntlr_instance_t cntlr_instnace;
void *cntlr_trnsprt_channel_mempool_g;
void *cntlr_channel_safe_ref_memPool_g;



/******************************************************************************
 * * * * * * * * * * * * Function Prototypes * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
int32_t transport_mempool_init(void);
int32_t  transport_library_init(void);
void of_tnsport_free_rcu(struct rcu_head *node);
static void
trnsprt_get_conn_mempool_entries(uint32_t *max_entries_p,
      uint32_t *max_static_entries_p);

/******************************************************************************
 * * * * * * * * * * * * Function Defintions * * * * * * * * * * * * * * * * * *
 *******************************************************************************/

int32_t  cntlr_transprt_init()
{

   int32_t ret_value = OF_SUCCESS;
   int32_t status = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"TCP Protocol Library Init");

   do
   {

      ret_value=transport_mempool_init();
      if(ret_value != OF_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR, "Mempool initialization failed");
         status=OF_FAILURE;
         break;
      }

      ret_value=transport_library_init();
      if(ret_value != OF_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"register protocols through library   - Failure");
         status = OF_FAILURE;
         break;
      }

      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"staring state machine for transport protocols");
      cntlr_transprt_fsm();

   }while(0);

   return status;
}


int32_t transport_mempool_init()
{
   int32_t status=OF_SUCCESS;
   int32_t ret_value=OF_SUCCESS;
   uint32_t   max_conn_static;
   uint32_t   max_conn_entries;
   struct mempool_params mempool_info={};

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "entered");

   do
   {
      /*memory pool for Controller's channel context*/
      trnsprt_get_conn_mempool_entries(&max_conn_entries,&max_conn_static);

      mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;  
      mempool_info.block_size = sizeof(cntlr_channel_info_t);
      mempool_info.max_blocks = max_conn_entries;
      mempool_info.static_blocks = max_conn_static;
      mempool_info.threshold_min = max_conn_static/10;
      mempool_info.threshold_max = max_conn_static/3;
      mempool_info.replenish_count = max_conn_static/10;
      mempool_info.b_memset_not_required =  FALSE;
      mempool_info.b_list_per_thread_required =  TRUE;
      mempool_info.noof_blocks_to_move_to_thread_list = 20;

      ret_value  = mempool_create_pool("of_channel_pool", &mempool_info,
            &cntlr_trnsprt_channel_mempool_g);
      if(ret_value != MEMPOOL_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR," Create mempool failure Transport channels, ret_value=%d ",ret_value);
         status=OF_FAILURE;
         break;
      }

      /*memory pool for Controller's channel safe reference*/
      mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;  
      mempool_info.block_size = sizeof(cntlr_ch_safe_ref_t);
      mempool_info.max_blocks = max_conn_entries;
      mempool_info.static_blocks = max_conn_static;
      mempool_info.threshold_min = max_conn_static/10;
      mempool_info.threshold_max = max_conn_static/3;
      mempool_info.replenish_count = max_conn_static/10;
      mempool_info.b_memset_not_required =  FALSE;
      mempool_info.b_list_per_thread_required =  TRUE;
      mempool_info.noof_blocks_to_move_to_thread_list = 20;

      ret_value  = mempool_create_pool("cntlr_ch_safe_ref_t", &mempool_info,
            &cntlr_channel_safe_ref_memPool_g);
      if(ret_value != MEMPOOL_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Create mempool failure Transport channels saferef, ret_value=%d ",
               ret_value);
         status=OF_FAILURE;
         break;
      }

      OF_LIST_INIT(cntlr_instnace.tranprt_list,of_tnsport_free_rcu);
   }while(0);

   return status;
}


   static void
trnsprt_get_conn_mempool_entries(uint32_t *max_entries_p,
      uint32_t *max_static_entries_p)
{
   if(max_entries_p)
      *max_entries_p = OF_TRNSPRT_MAX_CONN_ENTRIES;

   if(max_static_entries_p)
      *max_static_entries_p = OF_TRNSPRT_STATIC_CONN_ENTRIES;
}


   void
of_tnsport_free_rcu(struct rcu_head *node)
{
   cntlr_trnsprt_info_t *trnsprt_info = (cntlr_trnsprt_info_t*)((uint8_t*)node - CNTLR_TRNSPRT_LISTNODE_OFFSET);

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "entered");
   if(trnsprt_info)
     of_mem_free(trnsprt_info);
}




#define CNTLR_TANSPORT_PROTO_DIR     "/lib/proto"
int32_t  transport_library_init(void)
{
   char dir_path[CNTLR_MAX_DIR_PATH_LEN+1];
   char    *error;
   int32_t ii,no_of_files;
   struct  dirent **a_file_names_p_p;
   char    path[CNTLR_MAX_DIR_PATH_LEN+1]={'\0'};
   void    *proto_library_hndl_p;

   CNTLR_PROTO_INIT_HOOK proto_init_hook;


   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "entered");
   strcpy(dir_path, CNTLR_DIR);
   strcat(dir_path, CNTLR_TANSPORT_PROTO_DIR);

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "Shared Lib DIR Path = %s",dir_path);
   no_of_files = scandir(dir_path,&a_file_names_p_p, 0, alphasort);
   if( no_of_files < 0  )
   {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN, "scandir failed for DIR path: %s", dir_path);
      return OF_FAILURE;
   }

   for( ii = 0; ii < no_of_files; ii++)
   {
      /*Get the next file */
      if(!(strcmp( a_file_names_p_p[ii]->d_name,"."))  || !(strcmp(a_file_names_p_p[ii]->d_name,"..")))
      {
         free(a_file_names_p_p[ii]);
         continue;
      }
      if(a_file_names_p_p[ii]->d_name[0] != 's')
      {
         //OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "first character is not s but %c",a_file_names_p_p[ii]->d_name[0]);
         continue;
      }
      if((a_file_names_p_p[ii]->d_name[1] < '0') || (a_file_names_p_p[ii]->d_name[1] > '9'))
      {
         //OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "2nd character is not a digit but %c",a_file_names_p_p[ii]->d_name[1]);
         continue;
      }
      if((a_file_names_p_p[ii]->d_name[2] < '0') || (a_file_names_p_p[ii]->d_name[2] > '9'))
      {
         //OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "3rd character is not a digit but %c",a_file_names_p_p[ii]->d_name[2]);
         continue;
      }

      snprintf (path,CNTLR_MAX_DIR_PATH_LEN,"%s/%s",dir_path,a_file_names_p_p[ii]->d_name);
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"opening library:%s",path);

      proto_library_hndl_p = dlopen(path,RTLD_NOW|RTLD_GLOBAL);

      if(!proto_library_hndl_p)
      {
         if ((error = dlerror()) != NULL)
         {
            fprintf (stderr, "%s:%s\n",__FUNCTION__,error);
         }
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "dlopen failed for path:%s",path);
         continue;
      }

      /* Get the Registration Handler */
      proto_init_hook = ( CNTLR_PROTO_INIT_HOOK)dlsym(proto_library_hndl_p,"of_transport_proto_init");
      if ((error = dlerror()) != NULL)
      {
         fprintf (stderr, "%s:dlsym() returned error: %s\n",__FUNCTION__,error);
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "dlsym failed for path:%s",path);
         continue;
      }

      if(proto_init_hook)
         proto_init_hook();
   }

   free(a_file_names_p_p);
   return OF_SUCCESS;
}

int32_t  cntlr_transprt_deinit()
{

   int32_t ret_val = OF_SUCCESS;
   int32_t status = OF_SUCCESS;
   cntlr_trnsprt_info_t   *trnsprt_info_p=NULL;
   uchar8_t offset;


   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"TCP Protocol Library De-Init");

   do
   {
      offset = CNTLR_TRNSPRT_LISTNODE_OFFSET;
      OF_LIST_SCAN(cntlr_instnace.tranprt_list,trnsprt_info_p, cntlr_trnsprt_info_t *,offset)
      {
         ret_val = cntrl_transprt_fsm_stop(trnsprt_info_p);          
         if(ret_val != OF_SUCCESS){
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"FSM stop Failed");
            status = OF_FAILURE;
            break;
         }
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"transport type %d",trnsprt_info_p->transport_type);
         trnsprt_info_p->proto_callbacks->transport_close (trnsprt_info_p->fd, NULL);
         OF_REMOVE_NODE_FROM_LIST(cntlr_instnace.tranprt_list, trnsprt_info_p,
               NULL,offset);

         /* TODO: any missing cleanup activity); */
         of_mem_free(trnsprt_info_p);
      }   

   }while(0);

   return status;
}


