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

/* File  :      tranport_api.c
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

/******************************************************************************
 * * * * * * * * * * * * Global Variables * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
extern int32_t cntlr_xprt_channel_id_g;
int32_t cntlr_xprt_protocol_id_g=0;
int32_t cntlr_xprt_fd_type[CNTLR_TRANSPORT_TYPE_MAX]=
{
   /*NONE*/ 0,
   /*tls*/ OF_TRANSPORT_PROTO_FD_TYPE_SHARED_AND_DEDICATED,
   /*tcp*/OF_TRANSPORT_PROTO_FD_TYPE_SHARED_AND_DEDICATED,
   /*dtls*/OF_TRANSPORT_PROTO_FD_TYPE_SHARED,
   /*udp*/OF_TRANSPORT_PROTO_FD_TYPE_SHARED,
   /*unixdm_tcp*/OF_TRANSPORT_PROTO_FD_TYPE_SHARED_AND_DEDICATED,
   /*unixdm_udp*/OF_TRANSPORT_PROTO_FD_TYPE_SHARED,
   /*virtio*/OF_TRANSPORT_PROTO_FD_TYPE_DEDICATED
};
int32_t   cntlr_xprt_socket_protocols[CNTLR_TRANSPORT_TYPE_MAX]=
{
   OF_IPPROTO_NONE,
   OF_IPPROTO_TCP,
   OF_IPPROTO_TCP,
   OF_IPPROTO_UDP,
   OF_IPPROTO_UDP,
   OF_IPPROTO_NONE,
   OF_IPPROTO_NONE,
   OF_IPPROTO_NONE,
};

/******************************************************************************
 * * * * * * * * * * * * Function Prototypes * * * * * * * * * * * * * * * * * *
 *******************************************************************************/

cntlr_trnsprt_info_t *cntlr_get_equal_tls_transport_info(  cntlr_trnsprt_info_t   *trnsprt_info);
/******************************************************************************
 * * * * * * * * * * * * Function Definitions * * * * * * * * * * * * * * * * * *
 *******************************************************************************/

int32_t cntlr_add_transport_proto( of_trnsprt_proto_info_t *trnsprt_proto_info_p)
{
   cntlr_trnsprt_info_t   *trnsprt_info;
   cntlr_trnsprt_info_t   *trnsprt_info_scan=NULL;
   int32_t                 status = OF_SUCCESS;
   int32_t                 ret_val = OF_SUCCESS;
   uchar8_t offset;
   int8_t proto_found_b=FALSE;
   int8_t hit = COMP_NO_MATCH;
   void *proto_spec = NULL;

   offset = CNTLR_TRNSPRT_LISTNODE_OFFSET;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"entered");
   do
   {
      if(trnsprt_proto_info_p == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Invalid input");
         status = OF_FAILURE;
         break;
      }

      CNTLR_RCU_READ_LOCK_TAKE();
      OF_LIST_SCAN(cntlr_instnace.tranprt_list,trnsprt_info_scan, cntlr_trnsprt_info_t *,offset)
      {
         if (trnsprt_info_scan->transport_type == trnsprt_proto_info_p->transport_type)
	{ 
            ret_val=trnsprt_info_scan->proto_callbacks->transport_compare_proto_spec_info(trnsprt_proto_info_p,trnsprt_info_scan->proto_spec,&hit);
            if ( ret_val != OF_SUCCESS )
            {
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"open callback failed");
               status=OF_FAILURE;
               break;
            }

            if(hit == COMP_MATCH)
	    {
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"protocol already exists");
               status = OF_FAILURE;
               break;
            }   
         }
      } 

      CNTLR_RCU_READ_LOCK_RELEASE();

      if ( status == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"protocol already exists");
         status = OF_FAILURE;
         break;
      }
      CNTLR_RCU_READ_LOCK_TAKE();
      OF_LIST_SCAN(cntlr_instnace.tranprt_list,trnsprt_info_scan, cntlr_trnsprt_info_t *,offset)
      {
         if (trnsprt_info_scan->transport_type == trnsprt_proto_info_p->transport_type)
         {
            proto_found_b=TRUE;
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"protocol found");
            break;
         }
      }
      CNTLR_RCU_READ_LOCK_RELEASE();
      if( proto_found_b== FALSE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"protocol not registered yet");
         status = OF_FAILURE;
         break;
      }
      trnsprt_info = (cntlr_trnsprt_info_t*) of_mem_calloc(1,sizeof(cntlr_trnsprt_info_t));
      if(trnsprt_info == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Memory Allocation failure");
         status = OF_FAILURE;
         break;
      }


      trnsprt_info->id= trnsprt_proto_info_p->id;
      trnsprt_info->transport_type = trnsprt_proto_info_p->transport_type;
      trnsprt_info->proto = cntlr_xprt_socket_protocols[trnsprt_info->transport_type];
      trnsprt_info->fd_type=cntlr_xprt_fd_type[trnsprt_proto_info_p->transport_type];
      trnsprt_info->proto_callbacks = trnsprt_info_scan->proto_callbacks;
      ret_val=trnsprt_info_scan->proto_callbacks->transport_alloc_and_set_proto_spec_info(trnsprt_proto_info_p,&proto_spec);
      if ( ret_val != OF_SUCCESS )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"open callback failed");
         status=OF_FAILURE;
         break;
      }
      if ( proto_spec == NULL )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"alloc proto spec info failed");
         status=OF_FAILURE;
         break;
      }
      trnsprt_info->proto_spec = proto_spec;


      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"starting state machine for the protocol");
      OF_APPEND_NODE_TO_LIST(cntlr_instnace.tranprt_list,trnsprt_info,offset);
      cntrl_transprt_fsm_start(trnsprt_info);
   }
   while(0);

   return status;
}

int32_t cntlr_modify_transport_proto( of_trnsprt_proto_info_t *trnsprt_proto_info_p)
{
   cntlr_trnsprt_info_t   *trnsprt_info_p=NULL;
   int32_t                 status = OF_SUCCESS;
   int32_t                 ret_val = OF_SUCCESS;
   uchar8_t offset;
   int8_t proto_found_b=FALSE;
   void *proto_spec = NULL;   

   offset = CNTLR_TRNSPRT_LISTNODE_OFFSET;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"entered");
   do
   {
      if(trnsprt_proto_info_p == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Invalid input");
         status = OF_FAILURE;
         break;
      }

      CNTLR_RCU_READ_LOCK_TAKE();
      OF_LIST_SCAN(cntlr_instnace.tranprt_list,trnsprt_info_p, cntlr_trnsprt_info_t *,offset)
      {
         if (trnsprt_info_p->id == trnsprt_proto_info_p->id)
         {
            proto_found_b=TRUE;
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"protocol found");
            break;
         }
      }
      CNTLR_RCU_READ_LOCK_RELEASE();
      if( proto_found_b== FALSE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"protocol not registered yet");
         status = OF_FAILURE;
         break;
      }

      trnsprt_info_p->id= trnsprt_proto_info_p->id;
      trnsprt_info_p->transport_type = trnsprt_proto_info_p->transport_type;
      trnsprt_info_p->proto = cntlr_xprt_socket_protocols[trnsprt_proto_info_p->transport_type];
      trnsprt_info_p->fd_type=cntlr_xprt_fd_type[trnsprt_proto_info_p->transport_type];
      ret_val=trnsprt_info_p->proto_callbacks->transport_alloc_and_set_proto_spec_info(trnsprt_proto_info_p,&proto_spec);
      if ( ret_val != OF_SUCCESS )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"open callback failed");
         status=OF_FAILURE;
         break;
      }
      if ( proto_spec == NULL )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"alloc proto spec info failed");
         status=OF_FAILURE;
         break;
      }
      trnsprt_info_p->proto_spec = proto_spec;

      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"starting state machine for the protocol");
      cntrl_transprt_fsm_start(trnsprt_info_p);
   }
   while(0);

   return status;
}

int32_t cntlr_register_transport_proto(of_trnsprt_proto_info_t  *trnsprt_proto_info_p,
      struct of_transprt_proto_callbacks *proto_callbacks_p)
{
   cntlr_trnsprt_info_t   *trnsprt_info;
   cntlr_trnsprt_info_t   *trnsprt_info_scan;
   int32_t                 status = OF_SUCCESS;
   int32_t                 ret_val = OF_SUCCESS;
   uchar8_t offset;
   int32_t id;
   offset = CNTLR_TRNSPRT_LISTNODE_OFFSET;
   int8_t  hit = COMP_NO_MATCH;   
   void *proto_spec = NULL;


   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"entered");
   do
   {
      if(trnsprt_proto_info_p == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Invalid input");
         status = OF_FAILURE;
         break;
      }

      if ( proto_callbacks_p == NULL )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Invalid callback pointer");
         status = OF_FAILURE;
         break;
      }
      CNTLR_RCU_READ_LOCK_TAKE();
      OF_LIST_SCAN(cntlr_instnace.tranprt_list,trnsprt_info_scan, cntlr_trnsprt_info_t *,offset)
      {
         if (trnsprt_info_scan->transport_type == trnsprt_proto_info_p->transport_type)
	{
            ret_val=trnsprt_info_scan->proto_callbacks->transport_compare_proto_spec_info(trnsprt_proto_info_p,trnsprt_info_scan->proto_spec,&hit);
            if ( ret_val != OF_SUCCESS )
            {
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"compare proto spec callback failed");
               status=OF_FAILURE;
               break;
            }

            if(hit == COMP_MATCH)
	    {
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"protocol already exists");
               status = OF_FAILURE;
               break;
            }
         }

      }


      CNTLR_RCU_READ_LOCK_RELEASE();
      if ( status == OF_FAILURE )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"protocol already exists");
         break;
      }
      trnsprt_info = (cntlr_trnsprt_info_t*) of_mem_calloc(1,sizeof(cntlr_trnsprt_info_t));
      if(trnsprt_info == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Memory Allocation failure");
         status = OF_FAILURE;
         break;
      }
      CNTLR_XPRT_GENERATE_PROTOCOL_ID(id);
      trnsprt_info->id=id;
      trnsprt_info->transport_type = trnsprt_proto_info_p->transport_type;
      trnsprt_info->proto= cntlr_xprt_socket_protocols[ trnsprt_proto_info_p->transport_type];
      trnsprt_info->fd_type=cntlr_xprt_fd_type[trnsprt_proto_info_p->transport_type];
      trnsprt_info->proto_callbacks = proto_callbacks_p;
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"id=%d protocol=%d  fdtype=%d",id,cntlr_xprt_socket_protocols[ trnsprt_proto_info_p->transport_type],cntlr_xprt_fd_type[trnsprt_proto_info_p->transport_type]);

      ret_val=trnsprt_info->proto_callbacks->transport_alloc_and_set_proto_spec_info(trnsprt_proto_info_p,&proto_spec);
      if ( ret_val != OF_SUCCESS )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"open callback failed");
         status=OF_FAILURE;
         break;
      }
      if ( proto_spec == NULL )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"alloc proto spec info failed");
         status=OF_FAILURE;
         break;
      }
      trnsprt_info->proto_spec = proto_spec;


      OF_APPEND_NODE_TO_LIST(cntlr_instnace.tranprt_list,trnsprt_info,offset);
      trnsprt_info->is_enabled = TRUE;

   }while(0);


   return status;
}
int32_t cntlr_delete_transport_proto( of_trnsprt_proto_info_t *trnsprt_proto_info_p)
{
   cntlr_trnsprt_info_t   *trnsprt_info_scan=NULL;
   cntlr_trnsprt_info_t   *prev_trnsprt_info_scan=NULL;
   int32_t                 status = OF_SUCCESS;
   int32_t                 ret_val = OF_SUCCESS;
   uchar8_t offset;
   int8_t proto_found_b=FALSE;

   offset = CNTLR_TRNSPRT_LISTNODE_OFFSET;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"entered");
   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      if(trnsprt_proto_info_p == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Invalid input");
         status = OF_FAILURE;
         break;
      }

      OF_LIST_SCAN(cntlr_instnace.tranprt_list,trnsprt_info_scan, cntlr_trnsprt_info_t *,offset)
      {
            if  ( trnsprt_info_scan->id == trnsprt_proto_info_p->id)   
            {
               proto_found_b=TRUE;
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"protocol found");
               break;
            }
         prev_trnsprt_info_scan=trnsprt_info_scan;
      }

      if( proto_found_b== FALSE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"protocol not registered yet");
         status = OF_FAILURE;
         break;
      }
      cntrl_transprt_fsm_stop(trnsprt_info_scan);

      OF_REMOVE_NODE_FROM_LIST(cntlr_instnace.tranprt_list, trnsprt_info_scan,
            prev_trnsprt_info_scan,offset);
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return status;
}


int32_t cntlr_close_transport_proto( int32_t type)
{
   cntlr_trnsprt_info_t   *trnsprt_info_scan=NULL;
   cntlr_trnsprt_info_t   *prev_trnsprt_info_scan=NULL;
   int32_t                 status = OF_SUCCESS;
   uchar8_t offset;
   int8_t proto_found_b=FALSE;

   offset = CNTLR_TRNSPRT_LISTNODE_OFFSET;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"entered");
   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      OF_LIST_SCAN(cntlr_instnace.tranprt_list,trnsprt_info_scan, cntlr_trnsprt_info_t *,offset)
      {
         if (trnsprt_info_scan->transport_type == type)
         {
            proto_found_b=TRUE;
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"protocol found");
            break;
         }
         prev_trnsprt_info_scan=trnsprt_info_scan;
      }

      // cntrl_transprt_fsm_stop(trnsprt_info_scan);
      if (proto_found_b) 
      {
         trnsprt_info_scan->proto_callbacks->transport_close (trnsprt_info_scan->fd, NULL);   
         OF_REMOVE_NODE_FROM_LIST(cntlr_instnace.tranprt_list, trnsprt_info_scan,
               prev_trnsprt_info_scan,offset);
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return status;
}
int32_t cntlr_get_first_transport_proto( of_trnsprt_proto_info_t *trnsprt_proto_info_p)
{
   cntlr_trnsprt_info_t   *trnsprt_info_p=NULL;
   int32_t                 status = OF_SUCCESS;
   int32_t                 ret_val = OF_SUCCESS;
   uchar8_t offset;
   int8_t proto_found_b=FALSE;

   offset = CNTLR_TRNSPRT_LISTNODE_OFFSET;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"entered");
   do
   {
      if(trnsprt_proto_info_p == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Invalid input");
         status = OF_FAILURE;
         break;
      }

      CNTLR_RCU_READ_LOCK_TAKE();

      OF_LIST_SCAN(cntlr_instnace.tranprt_list,trnsprt_info_p, cntlr_trnsprt_info_t *,offset)
      {
         proto_found_b=TRUE;
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"protocol found");
         break;
      }

      if( proto_found_b== FALSE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"protocol not registered yet");
         status = OF_FAILURE;
         CNTLR_RCU_READ_LOCK_RELEASE();
         break;
      }
#if 0
      trnsprt_info = (cntlr_trnsprt_info_t*) of_mem_calloc(1,sizeof(cntlr_trnsprt_info_t));
      if(trnsprt_info == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Memory Allocation failure");
         status = OF_FAILURE;
         CNTLR_RCU_READ_LOCK_RELEASE();
         break;
      }
#endif
      trnsprt_proto_info_p->id= trnsprt_info_p->id;
      trnsprt_proto_info_p->proto= trnsprt_info_p->proto;
      trnsprt_proto_info_p->transport_type = trnsprt_info_p->transport_type;
      ret_val=trnsprt_info_p->proto_callbacks->transport_get_proto_spec_info(trnsprt_proto_info_p,trnsprt_info_p->proto_spec);
      if ( ret_val != OF_SUCCESS )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"open callback failed");
         status=OF_FAILURE;
         CNTLR_RCU_READ_LOCK_RELEASE();
         break;
      }

      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"succesfully retrieved");
      CNTLR_RCU_READ_LOCK_RELEASE();
   }
   while(0);

   return status;
}


int32_t cntlr_get_next_transport_proto( of_trnsprt_proto_info_t *trnsprt_proto_info_p)
{
   cntlr_trnsprt_info_t   *trnsprt_info_p=NULL;
   int32_t                 status = OF_SUCCESS;
   int32_t                 ret_val = OF_SUCCESS;
   uchar8_t offset;
   int8_t current_proto_found_b=FALSE;

   offset = CNTLR_TRNSPRT_LISTNODE_OFFSET;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"entered");
   do
   {
      if(trnsprt_proto_info_p == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Invalid input");
         status = OF_FAILURE;
         break;
      }

      CNTLR_RCU_READ_LOCK_TAKE();
      OF_LIST_SCAN(cntlr_instnace.tranprt_list,trnsprt_info_p, cntlr_trnsprt_info_t *,offset)
      {

         if ( current_proto_found_b == FALSE )
         {
            if  ( trnsprt_info_p->id != trnsprt_proto_info_p->id)  
            { 
               continue;
            }
            else
            {
               current_proto_found_b=TRUE;
               continue;
            }
         }
         else
         {
            break;
         }
      }

      if( current_proto_found_b== FALSE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"current protocol not found yet");
         status = OF_FAILURE;
         CNTLR_RCU_READ_LOCK_RELEASE();
         break;
      }

      if( trnsprt_info_p ==  NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"next protocol is not there");
         status = OF_FAILURE;
         CNTLR_RCU_READ_LOCK_RELEASE();
         break;
      }
#if 0
      trnsprt_info = (cntlr_trnsprt_info_t*) of_mem_calloc(1,sizeof(cntlr_trnsprt_info_t));
      if(trnsprt_info == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Memory Allocation failure");
         status = OF_FAILURE;
         CNTLR_RCU_READ_LOCK_RELEASE();
         break;
      }
#endif
      trnsprt_proto_info_p->id= trnsprt_info_p->id;
      trnsprt_proto_info_p->proto= trnsprt_info_p->proto;
      trnsprt_proto_info_p->transport_type = trnsprt_info_p->transport_type;
      // get protoinfo for void -- using callback
      ret_val=trnsprt_info_p->proto_callbacks->transport_get_proto_spec_info(trnsprt_proto_info_p,trnsprt_info_p->proto_spec);
      if ( ret_val != OF_SUCCESS )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"get proto spec failed");
         status=OF_FAILURE;
         CNTLR_RCU_READ_LOCK_RELEASE();
         break;
      }


      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"succesfully retrieved");
      CNTLR_RCU_READ_LOCK_RELEASE();
   }
   while(0);

   return status;
}

int32_t cntlr_get_exact_transport_proto( of_trnsprt_proto_info_t *trnsprt_proto_info_p)
{
   cntlr_trnsprt_info_t   *trnsprt_info_p=NULL;
   int32_t                 status = OF_SUCCESS;
   int32_t                 ret_val = OF_SUCCESS;
   uchar8_t offset;
   int8_t proto_found_b=FALSE;

   offset = CNTLR_TRNSPRT_LISTNODE_OFFSET;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"entered");
   do
   {
      if(trnsprt_proto_info_p == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Invalid input");
         status = OF_FAILURE;
         break;
      }
      CNTLR_RCU_READ_LOCK_TAKE();
      OF_LIST_SCAN(cntlr_instnace.tranprt_list,trnsprt_info_p, cntlr_trnsprt_info_t *,offset)
      {
         if  ( trnsprt_info_p->id == trnsprt_proto_info_p->id)   
         {
            proto_found_b=TRUE;
            break;
         }
      }

      if( proto_found_b== FALSE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"protocol not registered yet");
         status = OF_FAILURE;

         CNTLR_RCU_READ_LOCK_RELEASE();
         break;
      }
#if 0
      trnsprt_info = (cntlr_trnsprt_info_t*) of_mem_calloc(1,sizeof(cntlr_trnsprt_info_t));
      if(trnsprt_info == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Memory Allocation failure");
         status = OF_FAILURE;
         CNTLR_RCU_READ_LOCK_RELEASE();
         break;
      }
#endif
      trnsprt_proto_info_p->id= trnsprt_info_p->id;
      trnsprt_proto_info_p->proto= trnsprt_info_p->proto;
      trnsprt_proto_info_p->transport_type = trnsprt_info_p->transport_type;
      ret_val=trnsprt_info_p->proto_callbacks->transport_get_proto_spec_info(trnsprt_proto_info_p,trnsprt_info_p->proto_spec);
      if ( ret_val != OF_SUCCESS )
      {
         CNTLR_RCU_READ_LOCK_RELEASE();
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"get proto spec failed");
         status=OF_FAILURE;
         break;
      }


      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"succesfully retrieved");
      CNTLR_RCU_READ_LOCK_RELEASE();
   }
   while(0);

   return status;
}


int32_t cntlr_deregister_transport_proto(of_trnsprt_proto_info_t  *trnsprt_proto_info_p)
{
   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"entered");
   return (cntlr_delete_transport_proto(trnsprt_proto_info_p));
}

int32_t cntlr_delete_transport_by_proto( int8_t   transport_type)
{
   cntlr_trnsprt_info_t   *trnsprt_info_scan=NULL;
   cntlr_trnsprt_info_t   *prev_trnsprt_info_scan=NULL, *next = NULL;
   int32_t                 status = OF_SUCCESS;
   int32_t                 ret_val = OF_SUCCESS;
   uchar8_t offset;

   offset = CNTLR_TRNSPRT_LISTNODE_OFFSET;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"entered");
   do
   {
      CNTLR_RCU_READ_LOCK_TAKE();
      OF_LIST_DYN_SCAN(cntlr_instnace.tranprt_list,trnsprt_info_scan, next, cntlr_trnsprt_info_t *,offset)
      {
         if (trnsprt_info_scan->transport_type ==  transport_type)
         {
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"protocol found for deletion");
            ret_val=cntrl_transprt_fsm_stop(trnsprt_info_scan);
            OF_REMOVE_NODE_FROM_LIST(cntlr_instnace.tranprt_list, trnsprt_info_scan,
                  prev_trnsprt_info_scan,offset);
         }
         else
            prev_trnsprt_info_scan=trnsprt_info_scan;
      }
      CNTLR_RCU_READ_LOCK_RELEASE();
      /* TODO: any missing cleanup activity); */
   }
   while(0);

   return status;
}

#if 0
cntlr_trnsprt_info_t *cntlr_get_equal_tls_transport_info(  cntlr_trnsprt_info_t   *trnsprt_info)
{

   int32_t                 status = OF_FAILURE;
   int32_t                 ret_val = OF_SUCCESS;

   CNTLR_RCU_READ_LOCK_TAKE();
   OF_LIST_SCAN(cntlr_instnace.tranprt_list,trnsprt_info_scan, cntlr_trnsprt_info_t *,offset)
   {
      if ((trnsprt_info_scan->transport_type == CNTLR_TRANSPORT_TYPE_TLS) &&
            (trnsprt_info_scan->port_number == trnsprt_nfo->port_number))
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"protocol already exists");
         status = OF_SUCCESS;
         break;
      }
   }

   CNTLR_RCU_READ_LOCK_RELEASE();

   if ( status == OF_FAILURE)
   {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"tls protocol not found");
      return NULL;
   }   
   return trnsprt_info_scan;
}
#endif
