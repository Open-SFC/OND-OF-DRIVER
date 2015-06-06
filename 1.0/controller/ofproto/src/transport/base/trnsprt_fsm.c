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

/* File  :      tranport_fsm.c
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
 * * * * * * * * * * * * Function Prototypes * * * * * * * * * * * * * * * * * *
 *******************************************************************************/

int32_t fsm_xprt_proto_init(cntlr_trnsprt_info_t *trnsprt_info,  void **opaque);
int32_t fsm_xprt_proto_open(cntlr_trnsprt_info_t *trnsprt_info,  void **opaque);
int32_t fsm_xprt_proto_read(cntlr_trnsprt_info_t *trnsprt_info,  void **opaque);
int32_t fsm_xprt_proto_write(cntlr_trnsprt_info_t *trnsprt_info, void **opaque);
int32_t fsm_xprt_proto_close(cntlr_trnsprt_info_t *trnsprt_info, void **opaque);
int32_t fsm_xprt_proto_error(cntlr_trnsprt_info_t *trnsprt_info,  void **opaque);

int32_t fsm_xprt_proto_read_msg_from_dp( cntlr_channel_info_t *channel,
      uint8_t               *buffer,
      uint16_t               length,
      uint16_t               *len_read);
int32_t fsm_xprt_proto_write_msg_to_dp(cntlr_channel_info_t *channel,
      struct of_msg           *msg,
      uint16_t                len_already_sent,
      uint16_t                *len_sent_currently);

int32_t cntrl_transprt_fsm_start(cntlr_trnsprt_info_t *trnsprt_info);
/******************************************************************************
 * * * * * * * * * * * * Global Variables * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
extern int32_t cntlr_xprt_channel_id_g;
extern uint32_t cntlr_conn_hash_table_init_value_g;
extern void *cntlr_trnsprt_channel_mempool_g;
extern void *cntlr_channel_safe_ref_memPool_g;
extern cntlr_instance_t cntlr_instnace;
extern cntlr_thread_info_t master_thread_info;
extern __thread cntlr_thread_info_t *self_thread_info;
extern char  cntlr_xprt_protocols[CNTLR_TRANSPORT_TYPE_MAX][XPRT_PROTO_MAX_STRING_LEN];

char cnlr_xprt_fsm_states[OF_XPRT_PROTO_STATE_MAX][XPRT_PROTO_MAX_STRING_LEN]={
   "INIT",
   "WAIT_FOR_CONFIRM_CONN",
   "CLOSE_TRANSPORT"
};

char cnlr_xprt_fsm_events[OF_XPRT_PROTO_EVENT_MAX][XPRT_PROTO_MAX_STRING_LEN]={
   "LEARN_PROTO_EVENT",
   "NEW_CONN_EVENT",
   "READ_EVENT",
   "WRITE_EVENT",
   "READ_FAIL_EVENT",
   "WRITE_FAIL_EVENT",
   "ERROR_EVENT",
   "CLOSE_EVENT",
   "DEL_PROTO_EVENT"
};


of_xprt_proto_fsm_function cntlr_xprt_proto_fsm_handler[OF_XPRT_PROTO_STATE_MAX][OF_XPRT_PROTO_EVENT_MAX]=
{
   /*init*/  
   {fsm_xprt_proto_init, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
   /*W8 for New conn */
   {NULL, fsm_xprt_proto_open, fsm_xprt_proto_read, fsm_xprt_proto_write,  NULL, NULL, NULL, NULL,fsm_xprt_proto_close},
   /*close transprt*/
   { NULL, fsm_xprt_proto_error, fsm_xprt_proto_error, fsm_xprt_proto_error, NULL, NULL, fsm_xprt_proto_error, NULL}
};
extern of_xprt_proto_channel_fsm_function cntlr_xprt_proto_channel_fsm_handler[OF_XPRT_PROTO_CHANNEL_STATE_MAX][OF_XPRT_PROTO_CHANNEL_EVENT_MAX];

/***********************************************************************************
 * * * * * * * * * * * * Function Definitions * * * * * * * * * * * * * * * * * * *
 ***********************************************************************************/

void   cntlr_transprt_fsm()
{
   int32_t ret_val = OF_SUCCESS;
   int32_t status = OF_SUCCESS;
   cntlr_trnsprt_info_t   *trnsprt_info;
   uchar8_t offset;


   offset = CNTLR_TRNSPRT_LISTNODE_OFFSET;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Entered");

   do
   {
      OF_LIST_SCAN(cntlr_instnace.tranprt_list,trnsprt_info, cntlr_trnsprt_info_t *,offset)
      {
         ret_val= cntrl_transprt_fsm_start(trnsprt_info);
         if (ret_val != OF_SUCCESS)
         {
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"State Machine initiallization failed");
            status=OF_FAILURE;
         //   break;
         }
      }

   }while(0);

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Exit");

   return;
}

int32_t cntrl_transprt_fsm_start(cntlr_trnsprt_info_t *trnsprt_info)
{

   int32_t ret_val = OF_SUCCESS;
   int32_t status = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Entered");

   do
   {
      if ( CNTLR_UNLIKELY(trnsprt_info == NULL) )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"invalid input");
         status=OF_FAILURE;
         break;
      }

      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"initializing fsm for of proto %s ", cntlr_xprt_protocols[trnsprt_info->transport_type]);

      CNTLR_XPRT_PROTO_SET_TRANSPORT_FSM_STATE(trnsprt_info, OF_XPRT_PROTO_STATE_INIT);
      CNTLR_XPRT_PROTO_SET_TRANSPORT_FSM_EVENT(trnsprt_info, OF_XPRT_PROTO_LEARN_PROTO_EVENT);

      ret_val=   cntlr_xprt_proto_fsm_handler[trnsprt_info->fsm_state][trnsprt_info->fsm_event](trnsprt_info, NULL);
      if ( ret_val != OF_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"fsm function failed");
         status=OF_FAILURE;
         break;
      }
   }while(0);

   return status;
}

int32_t cntrl_transprt_fsm_stop(cntlr_trnsprt_info_t *trnsprt_info)
{

   int32_t ret_val = OF_SUCCESS;
   int32_t status = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Entered");

   do
   {
      if ( CNTLR_UNLIKELY(trnsprt_info == NULL) )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"invalid input");
         status=OF_FAILURE;
         break;
      }

      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"De-initializing fsm for of proto %d", trnsprt_info->proto);

      CNTLR_XPRT_PROTO_SET_TRANSPORT_FSM_EVENT(trnsprt_info, OF_XPRT_PROTO_DEL_PROTO_EVENT);

      ret_val=cntlr_xprt_proto_fsm_handler[trnsprt_info->fsm_state][trnsprt_info->fsm_event](trnsprt_info, NULL);

      if ( ret_val != OF_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"fsm function failed");
         status=OF_FAILURE;
         break;
      }
   }while(0);

   return status;
}

int32_t fsm_xprt_proto_init(cntlr_trnsprt_info_t *trnsprt_info,  void **opaque)
{
   int32_t ret_val = OF_SUCCESS;
   int32_t status = OF_SUCCESS;
   epoll_user_data_t     user_data;
   uint32_t              intrested_events;
   uchar8_t offset;
   offset = CNTLR_TRNSPRT_LISTNODE_OFFSET;
   of_transport_proto_properties_t proto_properties={0};
   of_conn_properties_t remote_conn_details={};
   cntlr_channel_info_t  *channel;
   uint8_t                    is_heap;


   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Entered");

   CNTLR_XPRT_PROTO_PRINT_STATE(trnsprt_info);
   do
   {
      if ( CNTLR_UNLIKELY(trnsprt_info == NULL) )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"invalid input");
         status=OF_FAILURE;
         break;
      }

      proto_properties.proto=trnsprt_info->proto;

      proto_properties.transport_type=trnsprt_info->transport_type;
      proto_properties.proto_spec=trnsprt_info->proto_spec;
      proto_properties.fd_type= trnsprt_info->fd_type;
      ret_val=trnsprt_info->proto_callbacks->transport_init(&proto_properties);
      if ( ret_val != OF_SUCCESS )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"init callback failed");
         status=OF_FAILURE;
         break;
      }

      ret_val = cntlr_trnsprt_mchash_table_init(trnsprt_info);
      if(ret_val == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Error in creating connection hash table");
         status = OF_FAILURE;
         break;
      }

      trnsprt_info->fd=proto_properties.fd;
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"fd created is %d",trnsprt_info->fd);
      intrested_events  = ePOLL_POLLIN|ePOLL_POLLERR|ePOLL_POLLHUP;
      user_data.arg1 = (void *)trnsprt_info;
      user_data.arg2 = NULL;

      if(trnsprt_info->fd_type ==  OF_TRANSPORT_PROTO_FD_TYPE_SHARED_AND_DEDICATED)
         ret_val = cntlr_epoll_add_fd_to_thread(&master_thread_info,
               trnsprt_info->fd,
               intrested_events,
               &user_data,
               cntlr_listen_for_new_connections);
      else if (trnsprt_info->fd_type ==  OF_TRANSPORT_PROTO_FD_TYPE_SHARED)
         ret_val = cntlr_epoll_add_fd_to_thread(&master_thread_info,
               trnsprt_info->fd,
               intrested_events,
               &user_data,
               cntlr_listen_for_dp_msgs_for_shared_fd);
      else if  (trnsprt_info->fd_type ==  OF_TRANSPORT_PROTO_FD_TYPE_DEDICATED)
      {
         remote_conn_details.trnsprt_type=trnsprt_info->transport_type;
         remote_conn_details.trnsprt_fd=trnsprt_info->fd;
         CNTLR_XPRT_CHANNEL_ALLOC(channel, is_heap);
         channel->trnsprt_info = trnsprt_info;
         CNTLR_XPRT_PROTO_SET_CHANNEL_FSM_STATE(channel,OF_XPRT_PROTO_STATE_INIT);
         CNTLR_XPRT_PROTO_SET_CHANNEL_FSM_EVENT(channel, OF_XPRT_PROTO_CHANNEL_LEARN_EVENT);
         cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, &remote_conn_details);
      }
      if(ret_val == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,
               "Failed to Add FD to epoll context of master thread");
         perror("epollAdd");
         status = OF_FAILURE;
         break;
      }

      CNTLR_XPRT_PROTO_SET_TRANSPORT_FSM_STATE(trnsprt_info,OF_XPRT_PROTO_STATE_READY_FOR_CONN);

   }while(0);

   return status; 
}

int32_t fsm_xprt_proto_open(cntlr_trnsprt_info_t *trnsprt_info,  void **opaque)
{
   int32_t ret_val = OF_SUCCESS;
   int32_t status = OF_SUCCESS;
   of_conn_properties_t remote_conn_details={};
   cntlr_channel_info_t  *channel;
   uint8_t                    is_heap;


   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Entered");

   CNTLR_XPRT_PROTO_PRINT_STATE(trnsprt_info);
   do
   {
      if ( CNTLR_UNLIKELY(trnsprt_info == NULL) )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"invalid input");
         status=OF_FAILURE;
         break;
      }

      ret_val=trnsprt_info->proto_callbacks->transport_open(trnsprt_info->fd, &remote_conn_details );
      if ( ret_val != OF_SUCCESS )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"open callback failed");
         status=OF_FAILURE;
         break;
      }
      CNTLR_XPRT_CHANNEL_ALLOC(channel, is_heap);
      channel->is_heap=is_heap;
      channel->trnsprt_info = trnsprt_info;
      CNTLR_XPRT_PROTO_SET_CHANNEL_FSM_STATE(channel, OF_XPRT_PROTO_STATE_INIT); 
      CNTLR_XPRT_PROTO_SET_CHANNEL_FSM_EVENT(channel,OF_XPRT_PROTO_CHANNEL_LEARN_EVENT);
      cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, &remote_conn_details);
   }while(0);

   return status;
}

#define OF_XPRT_TO_READ_BUFFER_LEN_TO_GET_REMOTE_IP 1
int32_t fsm_xprt_proto_read(cntlr_trnsprt_info_t *trnsprt_info,  void **opaque_pp)
{
   int32_t ret_val = OF_SUCCESS;
   int32_t status = OF_SUCCESS;
   uchar8_t offset;
   uint16_t len_read=0;
   uint8_t              peer_closed_conn=FALSE;
   of_conn_properties_t remote_conn_details={};
   uint8_t                    is_heap;
   cntlr_channel_info_t  *channel=NULL;
   int32_t      hash_val;
   int8_t       hit;
   struct ofp_header            msg_hdr;
   void *opaque_p = NULL;
   void *conn_spec_info = NULL;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Entered");
   offset = CNTLR_TRNSPRT_LISTNODE_OFFSET;
   CNTLR_XPRT_PROTO_PRINT_STATE(trnsprt_info);
   do
   {
      if (CNTLR_UNLIKELY (CNTLR_UNLIKELY(trnsprt_info == NULL) ))
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"invalid input");
         status=OF_FAILURE;
         break;
      }

      if(trnsprt_info->fd_type !=  OF_TRANSPORT_PROTO_FD_TYPE_SHARED) 
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"nothing to read here");
         break;
      }
      ret_val=trnsprt_info->proto_callbacks->transport_read(trnsprt_info->fd,NULL,(void*)&msg_hdr,OF_XPRT_TO_READ_BUFFER_LEN_TO_GET_REMOTE_IP,MSG_PEEK,&len_read, 
            &peer_closed_conn, &opaque_p);
      if ( ret_val != OF_SUCCESS )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"read callback failed");
         status=OF_FAILURE;
         break;
      }
      if(opaque_p)
      {
         conn_spec_info = opaque_p;
      }
      else
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Opaque_pp is NULL");
      }   
      // get_hash api specific function   

      ret_val = trnsprt_info->proto_callbacks->transport_get_hash_value(conn_spec_info,&hash_val);
      if ( ret_val != OF_SUCCESS )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Get hash Value failure");
         status=OF_FAILURE;
         break;
      }
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"hash value %d", hash_val);
      offset =   CONNECTION_TBL_LINK_OFFSET;
      MCHASH_BUCKET_SCAN(trnsprt_info->connection_tbl,hash_val, channel,  cntlr_channel_info_t *, offset)
      {
         ret_val = trnsprt_info->proto_callbacks->transport_compare_conn_spec_info(conn_spec_info,channel->conn_spec_info,&hit);
         if ( ret_val != OF_SUCCESS )
         {
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"compare function returned failure");
            status=OF_FAILURE;
            break;
         }
	 if(hit == COMP_MATCH) 
	 {
		 /* Atmaram -TBD:Need to free conn_spec_info here */
                 OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"compare matched");
		 free(conn_spec_info);
         	 *opaque_pp=(void *)channel;
		 break;
	 }
      }

      if ( channel == NULL )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Channel doesn't exists, creating new channel");
         remote_conn_details.trnsprt_type= trnsprt_info->transport_type;
         remote_conn_details.trnsprt_fd=trnsprt_info->fd;
         remote_conn_details.conn_spec_info = conn_spec_info;

         CNTLR_XPRT_CHANNEL_ALLOC(channel, is_heap);
         CNTLR_XPRT_PROTO_SET_TRNSPRT_INFO_FOR_CHANNEL(channel, trnsprt_info);
         CNTLR_XPRT_PROTO_SET_CHANNEL_FSM_STATE( channel,  OF_XPRT_PROTO_STATE_INIT);
         CNTLR_XPRT_PROTO_SET_CHANNEL_FSM_EVENT( channel, OF_XPRT_PROTO_CHANNEL_LEARN_EVENT);
         cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, &remote_conn_details);

         *opaque_pp=(void *)channel;
      }


   }while(0);

   return status; 
}


int32_t fsm_xprt_proto_read_error( cntlr_trnsprt_info_t *trnsprt_info,  void **opaque)
{
   int32_t status = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Entered");

   CNTLR_XPRT_PROTO_PRINT_STATE(trnsprt_info);
   do
   {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"do cleanup");
   }while(0);
   return status;	

}

int32_t fsm_xprt_proto_write(cntlr_trnsprt_info_t *trnsprt_info, void **opaque)
{
   int32_t               status = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Entered");
   CNTLR_XPRT_PROTO_PRINT_STATE(trnsprt_info);
   return status; 
}

int32_t fsm_xprt_proto_close(cntlr_trnsprt_info_t *trnsprt_info, void **opaque)
{
   int32_t               status = OF_SUCCESS;
   int32_t               ret_val = OF_SUCCESS;
   uint32_t hashkey;   
   cntlr_channel_info_t *channel;
   int8_t offset;


   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Entered");
   CNTLR_XPRT_PROTO_PRINT_STATE(trnsprt_info);

   do
   {
      if ( CNTLR_UNLIKELY(trnsprt_info == NULL) )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"invalid input");
         status=OF_FAILURE;
         break;
      }
      MCHASH_TABLE_SCAN(trnsprt_info->connection_tbl,hashkey)
      {
         offset=CONNECTION_TBL_LINK_OFFSET;
         MCHASH_BUCKET_SCAN(trnsprt_info->connection_tbl,hashkey,channel,cntlr_channel_info_t *,offset)
         {
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Hashvalue is %d",hashkey);
            CNTLR_XPRT_PROTO_SET_CHANNEL_FSM_EVENT(channel,OF_XPRT_PROTO_CHANNEL_DP_DETACH_EVENT);  
            ret_val = cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, NULL); 
            CNTLR_XPRT_PROTO_SET_CHANNEL_FSM_EVENT(channel,OF_XPRT_PROTO_CHANNEL_CLOSE_EVENT);  
            ret_val = cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, NULL); 
            if(ret_val != OF_SUCCESS)
            {
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Channel Close failed");
               status = OF_FAILURE_CHN_DEL; /* Atmaram - reverify TBD*/
               break;
            }   
         }
         if(status == OF_FAILURE_CHN_DEL) /* Atmaram -reverify TBD */
         {
            status = OF_FAILURE;
            break;
         }
      }
      ret_val=trnsprt_info->proto_callbacks->transport_deinit(trnsprt_info->fd);
      if ( ret_val != OF_SUCCESS )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"deinit callback failed");
         status=OF_FAILURE;
         break;
      }

   }while(0);

   return status;
}

int32_t fsm_xprt_proto_error(cntlr_trnsprt_info_t *trnsprt_info, void **opaque)
{
   int32_t status = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Entered");

   do
   {
      CNTLR_XPRT_PROTO_PRINT_STATE(trnsprt_info);

      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"error for of proto %d fd %d ", trnsprt_info->proto, trnsprt_info->fd);
      CNTLR_XPRT_PROTO_SET_TRANSPORT_FSM_EVENT(trnsprt_info, OF_XPRT_PROTO_CLOSE_EVENT);

   }while(0);
   return status;
}





