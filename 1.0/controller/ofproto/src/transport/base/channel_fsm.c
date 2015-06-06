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

/* File  :      channel_fsm.c
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
#include "pktmbuf.h"

/******************************************************************************
 * * * * * * * * * * * * Function Prototypes * * * * * * * * * * * * * * * * * *
 *******************************************************************************/

int32_t fsm_xprt_proto_channel_learn(cntlr_channel_info_t *channel, void *opaque);
int32_t fsm_xprt_proto_channel_read(cntlr_channel_info_t *channel, void *opaque);
int32_t fsm_xprt_proto_channel_read_error( cntlr_channel_info_t *channel, void *opaque);
int32_t fsm_xprt_proto_channel_write( cntlr_channel_info_t *channel, void *opaque);
int32_t fsm_xprt_proto_channel_write_error( cntlr_channel_info_t *channel, void *opaque);
int32_t fsm_xprt_proto_channel_close(cntlr_channel_info_t *channel, void *opaque);
int32_t fsm_xprt_proto_channel_error( cntlr_channel_info_t *channel, void *opaque);
int32_t fsm_xprt_proto_channel_learn_dp(cntlr_channel_info_t *channel,  void *opaque);
int32_t fsm_xprt_proto_channel_detach_dp( cntlr_channel_info_t *channel,  void *opaque);
int32_t fsm_xprt_proto_channel_read_msg_from_dp( cntlr_channel_info_t *channel,
      uint8_t               *buffer,
      uint16_t               length,
      uint16_t               *len_read);
int32_t fsm_xprt_proto_channel_write_msg_to_dp(cntlr_channel_info_t *channel,
      struct of_msg           *msg,
      uint16_t                len_already_sent,
      uint16_t                *len_sent_currently);

int32_t xprt_proto_channel_close( cntlr_channel_info_t *channel);
/******************************************************************************
 * * * * * * * * * * * * Global Variables * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
int32_t cntlr_xprt_channel_id_g=0;
extern uint32_t cntlr_conn_hash_table_init_value_g;
extern void *cntlr_channel_safe_ref_memPool_g;
extern cntlr_instance_t cntlr_instnace;
extern cntlr_thread_info_t master_thread_info;
extern __thread cntlr_thread_info_t *self_thread_info;
int32_t invalid_channel_state_g = 0;

char cnlr_xprt_proto_channel_states[OF_XPRT_PROTO_CHANNEL_STATE_MAX][XPRT_PROTO_MAX_STRING_LEN]={
   "INIT",
   "WAIT_FOR_HANDSHAKE",
   "STATE_BOUND_TO_DP",
   "CLOSE_CONN",
};

char cnlr_xprt_proto_channel_events[OF_XPRT_PROTO_CHANNEL_EVENT_MAX][XPRT_PROTO_MAX_STRING_LEN]={
   "LEARN_EVENT",
   "DP_DETACH",
   "READ_EVENT",
   "WRITE_EVENT",
   "READ_FAIL_EVENT",
   "WRITE_FAIL_EVENT",
   "ERROR_EVENT",
   "CLOSE_EVENT"
};

char  cntlr_xprt_protocols[CNTLR_TRANSPORT_TYPE_MAX][XPRT_PROTO_MAX_STRING_LEN]=
{
   "NONE",
   "TLS",
   "TCP",
   "DTLS",
   "UDP",
   "VIRTIO",
   "UDM_TCP",
   "UDM_UDP"
};


of_xprt_proto_channel_fsm_function cntlr_xprt_proto_channel_fsm_handler[OF_XPRT_PROTO_CHANNEL_STATE_MAX][OF_XPRT_PROTO_CHANNEL_EVENT_MAX]=
{
   /*init*/  
   {fsm_xprt_proto_channel_learn, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
   /*w8 handshake*/
   {fsm_xprt_proto_channel_learn_dp, fsm_xprt_proto_channel_detach_dp, fsm_xprt_proto_channel_read, fsm_xprt_proto_channel_write, fsm_xprt_proto_channel_read_error, fsm_xprt_proto_channel_write_error, fsm_xprt_proto_channel_error, fsm_xprt_proto_channel_close},
   /*bound 2 dp*/
   {NULL, fsm_xprt_proto_channel_detach_dp, fsm_xprt_proto_channel_read, fsm_xprt_proto_channel_write, fsm_xprt_proto_channel_read_error,fsm_xprt_proto_channel_write_error, fsm_xprt_proto_channel_error,fsm_xprt_proto_channel_close}, //Added read error instead of read at 5 and NULL at 6 --Vivek
   /*close*/ 
   {fsm_xprt_proto_channel_error, fsm_xprt_proto_channel_error, fsm_xprt_proto_channel_error,fsm_xprt_proto_channel_error,fsm_xprt_proto_channel_error, fsm_xprt_proto_channel_error, fsm_xprt_proto_channel_error,fsm_xprt_proto_channel_error}
};


/******************************************************************************
 * * * * * * * * * * * * Function Definitions * * * * * * * * * * * * * * * * * * 
 *******************************************************************************/
int32_t fsm_xprt_proto_channel_learn(cntlr_channel_info_t *channel, void *opaque)
{
   int32_t ret_val = OF_SUCCESS;
   int32_t status = OF_SUCCESS;
   epoll_user_data_t          user_data;
   uint32_t              intrested_events;
   of_conn_properties_t *remote_conn_info_p=(of_conn_properties_t *)opaque;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Entered");

   do
   {
      ret_val=cntlr_trnsprt_channel_init
         (channel,
          remote_conn_info_p->trnsprt_type,
          remote_conn_info_p->trnsprt_fd,
          remote_conn_info_p->conn_spec_info
         );          
      if(ret_val == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Failed add channel");
         status = OF_FAILURE;
         break;
      }

      CNTLR_XPRT_PROTO_CHANNEL_PRINT_STATE( channel);
      if ((channel->trnsprt_info->fd_type ==  OF_TRANSPORT_PROTO_FD_TYPE_SHARED_AND_DEDICATED ) ||
            ( channel->trnsprt_info->fd_type == OF_TRANSPORT_PROTO_FD_TYPE_DEDICATED))
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Adding fd to poll");
         CNTLR_XPRT_PROTO_SET_CHANNEL_FSM_STATE(channel,OF_XPRT_PROTO_CHANNEL_STATE_WAIT_FOR_HANDSHAKE);
         intrested_events  = ePOLL_POLLIN|ePOLL_POLLERR|ePOLL_POLLHUP;
         user_data.arg1 = (void *)channel;
         user_data.arg2 = (void *)channel->thread;
         ret_val = cntlr_epoll_add_fd_to_thread(channel->thread,
               channel->trnsprt_fd,
               intrested_events,
               &user_data,
               cntlr_trnsprt_handle_handshake);
         if(ret_val == OF_FAILURE)
         {
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"Failed to Add FD to epoll context ");
            status = OF_FAILURE;
            break;
         }
      }

      CNTLR_XPRT_PROTO_SET_CHANNEL_FSM_STATE(channel,OF_XPRT_PROTO_CHANNEL_STATE_WAIT_FOR_HANDSHAKE);
      if (channel->trnsprt_info->fd_type ==  OF_TRANSPORT_PROTO_FD_TYPE_SHARED_AND_DEDICATED )
      { 
         ret_val = cntlr_trnsprt_send_hello_msg(channel);
         if(ret_val == OF_FAILURE)
         {
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Error in sending hello message");
            status = OF_FAILURE;
            break;
         }

         channel->state=OF_TRNSPRT_CHN_SENT_HELLO_TO_DP;
      }
   }while(0);

   return status;
}

#define OF_XPRT_TO_READ_BUFFER_LEN_TO_GET_REMOTE_IP 1
int32_t fsm_xprt_proto_channel_read(cntlr_channel_info_t *channel, void *opaque)
{
   int32_t ret_val = OF_SUCCESS;
   int32_t status = OF_SUCCESS;
   uint16_t len_read=0;
   uint8_t              peer_closed_conn=FALSE;
   int32_t flags=0;
   int8_t offset;

   offset = CNTLR_TRNSPRT_LISTNODE_OFFSET;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Entered");
   CNTLR_XPRT_PROTO_CHANNEL_PRINT_STATE( channel);

   do
   {
      if (opaque)
      {
         flags=*(int32_t *)opaque;
      }
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"channel msg state %d hdlen read %d",channel->msg_state,channel->hdr_len_read);
      switch (channel->msg_state)
      {
         case  OF_CHN_WAITING_FOR_NEW_MSG:
            {
               ret_val=channel->trnsprt_info->proto_callbacks->transport_read(channel->trnsprt_fd, channel->conn_spec_info,  (void*)&channel->msg_hdr, sizeof(struct ofp_header),  flags, &len_read,
                     &peer_closed_conn,NULL);
               if ( ret_val != CNTLR_CONN_READ_SUCCESS )
               {
                  OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"read callback failed");
                  if ( peer_closed_conn == TRUE )
                  {  
                     channel->fsm_event= OF_XPRT_PROTO_CHANNEL_DP_DETACH_EVENT;
                     cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, NULL);
                     channel->fsm_event= OF_XPRT_PROTO_CHANNEL_CLOSE_EVENT;
                  }
                  else
                  {
                     channel->fsm_event= OF_XPRT_PROTO_CHANNEL_READ_FAIL_EVENT;
                  }
                  cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, NULL);
                  status=OF_FAILURE;
                  break;
               }
               //0      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"length to be read %d length  read %d", sizeof(struct ofp_header), len_read);

               channel->msg_len_read = 0;
               if(len_read == sizeof(struct ofp_header))
               {
                  uint8_t *tmp=&channel->msg_hdr;
                  CNTLR_XPRT_PRINT_PKT_DATA(tmp,8);
                  CNTLR_XPRT_PRINT_CHANNEL_MSG_HEADER(channel);
                  /* Header received waiting for actual message*/
                  channel->msg_state    = OF_CHN_START_READING_MSG;
                  channel->hdr_len_read = len_read;
                  channel->msg_buf      = NULL;
               }
               else
               {
                  /*Complete header information is not available, lets wait for it to arive*/
                  channel->hdr_len_read = len_read;
                  channel->msg_state    = OF_CHN_WAITING_FOR_MSG_HEADER;
                  break;
               }
            }
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"channel msg state %d lenread %d hdlen read %d",channel->msg_state, len_read, channel->hdr_len_read);

            break;
         case OF_CHN_WAITING_FOR_MSG_HEADER:
            {
               ret_val=channel->trnsprt_info->proto_callbacks->transport_read(channel->trnsprt_fd, channel->conn_spec_info, ((void*)(&channel->msg_hdr) + channel->hdr_len_read),
                     (sizeof(struct ofp_header)-channel->hdr_len_read),
                     flags, &len_read,
                     &peer_closed_conn,NULL);
               if ( ret_val != CNTLR_CONN_READ_SUCCESS )
               {
                  OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"read callback failed");
                  if ( peer_closed_conn == TRUE )
                  {
                     channel->fsm_event= OF_XPRT_PROTO_CHANNEL_DP_DETACH_EVENT;
                     cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, NULL);
                     channel->fsm_event= OF_XPRT_PROTO_CHANNEL_CLOSE_EVENT;
                  }
                  else
                  {
                     channel->fsm_event= OF_XPRT_PROTO_CHANNEL_READ_FAIL_EVENT;
                  }
                  cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, NULL);
                  status=OF_FAILURE;
                  break;
               }

               //if(len_read == sizeof(struct ofp_header))
               {
                  if((len_read+channel->hdr_len_read) == sizeof(struct ofp_header))
                  {
                     /* Header received waiting for actual message*/
                     channel->msg_state = OF_CHN_START_READING_MSG;
                     channel->hdr_len_read += len_read;
                     channel->msg_buf      = NULL;
                  }
                  else
                  {
                     /*Still waiting for messsage header to receive*/
                     channel->hdr_len_read += len_read;
                     break;
                  }
               }
            }
            break; 
         case OF_CHN_START_READING_MSG:
            // channel->recv_bytes += len_read;
            invalid_channel_state_g++;
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,
                  "something wrong here msg state OF_CHN_START_READING_MSG count %d",
                  invalid_channel_state_g);
            if(invalid_channel_state_g >= 5)
            {
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,
                     "Invalid Channel State due to malformed packets,Closing the connection to the Datapath=%llx ", channel->datapath->dpid);
               fsm_send_channel_event(channel,OF_XPRT_PROTO_CHANNEL_CLOSE_EVENT);
               //               exit(1);
            }
            break;
         case OF_CHN_WAITING_FOR_COMPLETE_MSG:
            {
               uint32_t len_2_read = (channel->msg_hdr.length - channel->msg_buf->desc.length1);
               if(CNTLR_UNLIKELY(!channel->msg_buf|| !channel->msg_hdr.length || !channel->msg_len_read))
               {
                  invalid_channel_state_g++;
                  OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,
                        "something wrong here msg state OF_CHN_START_READING_MSG count %d",
                        invalid_channel_state_g);
                  if(invalid_channel_state_g >= 10)
                  {
                     OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,
                           "Invalid Channel State due to malformed packets,Closing the connection to the Datapath=%llx",channel->datapath->dpid);
                     fsm_send_channel_event(channel,OF_XPRT_PROTO_CHANNEL_CLOSE_EVENT);
                     //exit(2);
                  }
                  break;
               }
               if(len_2_read == 0)
               {
                  /* Header received waiting for actual message*/
                  channel->msg_state = OF_CHN_RCVD_COMPLETE_MSG ;
                  break;
               }

               ret_val=channel->trnsprt_info->proto_callbacks->transport_read(channel->trnsprt_fd, channel->conn_spec_info, ((void*)(channel->msg_buf->desc.ptr1) + channel->msg_len_read),
                     len_2_read,
                     flags, &len_read,
                     &peer_closed_conn,NULL);
               if ( ret_val != CNTLR_CONN_READ_SUCCESS )
               {
                  channel->msg_state = OF_CHN_DRAIN_OUT_COMPLETE_MSG;
                  if ( peer_closed_conn == TRUE )
                  {
                     channel->fsm_event= OF_XPRT_PROTO_CHANNEL_CLOSE_EVENT;
                  }
                  else
                  {
                     channel->fsm_event= OF_XPRT_PROTO_CHANNEL_READ_FAIL_EVENT;
                  }
                  OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"read callback failed");
                  channel->drain_out_bytes = channel->msg_hdr.length - sizeof(struct ofp_header);
                  cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, NULL);
                  if(channel->msg_buf != NULL)
                  {
                     of_check_pkt_mbuf_and_free(channel->msg_buf);
                     channel->msg_buf = NULL;
                  }
                  status=OF_FAILURE;
                  break;
               }
               if(len_read == len_2_read)
               {
                  /* Header received waiting for actual message*/
                  channel->msg_buf->desc.data_len = (channel->msg_buf->desc.length1 + len_read);
                  channel->msg_state = OF_CHN_RCVD_COMPLETE_MSG ;
                  break;
               }
               else
               {
                  /*Still waiting for messsage header to receive*/
                  channel->msg_buf->desc.length1 += len_read;
                  channel->msg_len_read += len_read;
                  break;
               }
            }
            break;
         default:
            invalid_channel_state_g++;
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,
                  "Invalid_channel_state_g count = %d, channel->msg_state = %d\n",
                  invalid_channel_state_g, channel->msg_state);
            if(invalid_channel_state_g >= 5)
            {
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,
                     "Invalid Channel State due to malformed packets,Closing the connection to the Datapath=%llx ",channel->datapath->dpid);
               fsm_send_channel_event(channel,OF_XPRT_PROTO_CHANNEL_CLOSE_EVENT);
               //exit(1);
            }
            break;
      }

   }while(0);

   return status; 
}

int32_t fsm_xprt_proto_channel_write( cntlr_channel_info_t *channel, void *opaque)
{
   int32_t               status = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Entered");
   CNTLR_XPRT_PROTO_CHANNEL_PRINT_STATE( channel);
   return status; 
}

int32_t fsm_xprt_proto_channel_close( cntlr_channel_info_t *channel, void *opaque)
{
   int32_t               status = OF_SUCCESS;
   int32_t               ret_val = OF_SUCCESS;
   cntlr_channel_info_t *aux_channel=NULL;
   dprm_dp_channel_entry_t        *dp_channel = NULL;
   dprm_dp_channel_entry_t        *dp_channel_tmp = NULL;
   int8_t hash_tbl_access_status=FALSE;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Entered");
   CNTLR_XPRT_PROTO_CHANNEL_PRINT_STATE( channel);
   do
   {
      if (  CNTLR_UNLIKELY(channel == NULL) )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"invalid input");
         status=OF_FAILURE;
         break;
      }

      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"channle id %d", channel->id);

      if(channel->auxiliary_id == OF_TRNSPRT_MAIN_CONN_ID)
      {
         if (( channel->is_chn_added_to_dp == TRUE) &&  (channel->datapath->aux_channels))
         {
            CNTLR_LOCK_TAKE(channel->datapath->aux_ch_list_lock);
            while ( channel->datapath->curr_no_of_aux_chanels > 0 )
            {
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"aux channelx exists for channle id %d", channel->id);
               dp_channel=rcu_dereference(channel->datapath->aux_chn_tail);

               MCHASH_ACCESS_NODE(dp_channel->conn_table,dp_channel->conn_safe_ref.index,dp_channel->conn_safe_ref.magic,aux_channel,hash_tbl_access_status);
               if (hash_tbl_access_status == FALSE)
               {
                  OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"Channel safe reference is not valid");
                  status = OF_FAILURE;
                  break;
               }
               ret_val=xprt_proto_channel_close(aux_channel);
               if ( ret_val != OF_SUCCESS)
               {
                  OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"aux Channel id %d close failed", aux_channel->id);
                  status = OF_FAILURE;
                  break;
               }
               if ( rcu_dereference(channel->datapath->aux_chn_tail) != rcu_dereference(channel->datapath->aux_channels))
               {
                  CNTLR_RCU_ASSIGN_POINTER(dp_channel_tmp, dp_channel->prev);
                  CNTLR_RCU_ASSIGN_POINTER(channel->datapath->aux_chn_tail,dp_channel_tmp);
                  CNTLR_RCU_ASSIGN_POINTER(channel->datapath->aux_chn_tail->next, channel->datapath->aux_channels);
               }
               of_mem_free(dp_channel);
               dp_channel=NULL;
               channel->datapath->curr_no_of_aux_chanels--;
               if ( channel->datapath->curr_no_of_aux_chanels == 0)
               {
                  channel->datapath->aux_channels = NULL;
                  channel->datapath->aux_chn_tail=NULL;
               }
            }
            CNTLR_LOCK_RELEASE(channel->datapath->aux_ch_list_lock);
         }
#if 0
         else
         {   
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"not attached dp..aux channle id %d", channel->id);
            ret_val=xprt_proto_channel_close(channel);
            if ( ret_val != OF_SUCCESS)
            {
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"aux Channel id %d close failed", channel->id);
               status = OF_FAILURE;
               break;
            }
         }
#endif
      }

      ret_val=xprt_proto_channel_close(channel);
      if ( ret_val != OF_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN," Channel id %d close failed", channel->id);
         status = OF_FAILURE;
         break;
      }

   }while(0);

   return status;

}

int32_t fsm_xprt_proto_channel_error( cntlr_channel_info_t *channel,  void *opaque)
{
   int32_t status = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Entered");

   do
   {
      CNTLR_XPRT_PROTO_CHANNEL_PRINT_STATE( channel);
      /* TBD: */
   }while(0);

   return status;
}

int32_t fsm_xprt_proto_channel_learn_dp( cntlr_channel_info_t *channel,  void *opaque)
{
   epoll_user_data_t            user_data;
   uint32_t                     intrested_events;
   int32_t ret_val = OF_SUCCESS;
   int32_t status = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG," Entered");
   do
   {
      channel->is_chn_added_to_dp= TRUE;
      channel->fsm_state=OF_XPRT_PROTO_CHANNEL_STATE_BOUND_TO_DP;
      CNTLR_XPRT_PROTO_CHANNEL_PRINT_STATE( channel);
      if ((channel->trnsprt_info->fd_type ==  OF_TRANSPORT_PROTO_FD_TYPE_DEDICATED)  ||
            (channel->trnsprt_info->fd_type ==   OF_TRANSPORT_PROTO_FD_TYPE_SHARED_AND_DEDICATED) )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Dedicated fd");
         /*Delete current FD handshake Handler from epoll*/
         cntlr_epoll_del_fd(channel->trnsprt_fd);

         /*And then attach run time FD handler to receive messages on the channeld of DP*/
         intrested_events  = ePOLL_POLLIN;
         user_data.arg1 = (void *)channel->trnsprt_info->connection_tbl;
         user_data.arg2 = (void *)channel->safe_ref;
         ret_val = cntlr_epoll_add_fd(channel->trnsprt_fd,
               intrested_events,
               &user_data,
               of_handle_dp_msg);
         if(ret_val == OF_FAILURE)
         {  
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR," Failed to Add FD to epoll context of transport thread ");
            status = OF_FAILURE;
            break;
         }
      }

   }while(0);

   return status;
}

int32_t fsm_xprt_proto_channel_detach_dp( cntlr_channel_info_t *channel,  void *opaque)
{
   epoll_user_data_t            user_data;
   uint32_t                     intrested_events;
   int32_t ret_val = OF_SUCCESS;
   int32_t status = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG," Entered");

   do
   {

      if (channel->is_chn_added_to_dp ==FALSE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_INFO,"channel %d not yet added to dp", channel->id);
         break;
      }
      if(channel->auxiliary_id == OF_TRNSPRT_MAIN_CONN_ID)
      {
         //channel->datapath->is_main_conn_present = FALSE;

         if ( channel->keep_alive_state == OF_TRNSPRT_CHN_CONN_KEEP_ALIVE_MSG_SENT)
         {
            ret_val= timer_stop_tmr(NULL, &channel->keepalive_resptimer);
            if(ret_val == OF_FAILURE)
            {
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR," Error in stoping keepalive_resptimer");
               //      status = OF_FAILURE;
               //      break;
            }
         }
         if ( channel->keep_alive_state   >= OF_TRNSPRT_CHN_CONN_KEEP_ALIVE_MSG_READ_TO_SEND)
         {
            ret_val= timer_stop_tmr(NULL, &channel->keepalive_timer);
            if(ret_val == OF_FAILURE)
            {
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR," Error in stoping keepalive_resptimer");
               //      status = OF_FAILURE;
               //      break;
            }
         }
         /* NSM  moved the next two lines below to fix port issues in controller stop */
         //channel->datapath->is_main_conn_present = FALSE;
         //memset(&(channel->datapath->main_channel),0,sizeof(dprm_dp_channel_entry_t));
         OF_LOG_MSG(OF_LOG_TSC, OF_LOG_DEBUG, "NSM1 %d ",channel->datapath->datapath_handle);
         inform_event_to_app(channel->datapath,
               DP_DETACH_EVENT,NULL,NULL);
         ret_val=dprm_datapath_detach_handler(channel->datapath->datapath_handle);
         if(ret_val != OF_SUCCESS)
         {
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR, "datapath detach handler failedfor datapath_handle %d ",channel->datapath->datapath_handle);
         }

      }
      /*in else need to handle other than main connection */
      if ((channel->trnsprt_info->fd_type ==  OF_TRANSPORT_PROTO_FD_TYPE_SHARED_AND_DEDICATED ) ||
            ( channel->trnsprt_info->fd_type == OF_TRANSPORT_PROTO_FD_TYPE_DEDICATED))
      {

         if ( channel->trnsprt_fd > 0 )
         {
            cntlr_epoll_del_fd_from_thread(channel->thread,channel->trnsprt_fd);
            //cntlr_epoll_del_fd(channel->trnsprt_fd);

            intrested_events  = ePOLL_POLLIN|ePOLL_POLLERR|ePOLL_POLLHUP;
            user_data.arg1 = (void *)channel;
            user_data.arg2 = (void *)channel->thread;

            ret_val = cntlr_epoll_add_fd_to_thread(channel->thread,
                  channel->trnsprt_fd,
                  intrested_events,
                  &user_data,
                  cntlr_trnsprt_handle_handshake);
            if(ret_val == OF_FAILURE)
            {
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"Failed to Add FD %d to epoll context",  channel->trnsprt_fd);
               status = OF_FAILURE;
               break;
            }

         }
      }
      /* NSM  moved the next two lines from above to fix port deletion and flow deletion issues in controller stop */
      channel->datapath->is_main_conn_present = FALSE;
      memset(&(channel->datapath->main_channel),0,sizeof(dprm_dp_channel_entry_t));
      OF_LOG_MSG(OF_LOG_TSC, OF_LOG_DEBUG, "NSM2");
      channel->state = OF_TRNSPRT_CHN_CONN_CLOSED;
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"Detaching Dp from channel");
      //  memset(channel->safe_ref,0,sizeof(cntlr_ch_safe_ref_t));
      channel->datapath=NULL; /* conn table , safe ref need to be reset */
      channel->dp_handle=0;
      channel->is_chn_added_to_dp =FALSE;
      channel->fsm_state=OF_XPRT_PROTO_CHANNEL_STATE_WAIT_FOR_HANDSHAKE;
   }while(0);

   return status;
}

int32_t fsm_xprt_proto_channel_read_msg_from_dp( cntlr_channel_info_t *channel,
      uint8_t               *msg,
      uint16_t               length,
      uint16_t               *len_read)
{

   int32_t ret_val = OF_SUCCESS;
   int32_t status = CNTLR_CONN_READ_SUCCESS;
   uint8_t              peer_closed_conn = FALSE;


   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Entered");
   CNTLR_XPRT_PROTO_CHANNEL_PRINT_STATE(channel);

   do
   {
      if (  CNTLR_UNLIKELY(channel == NULL) )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"invalid input");
         status=OF_FAILURE;
         break;
      }
      ret_val=channel->trnsprt_info->proto_callbacks->transport_read(channel->trnsprt_fd,channel->conn_spec_info, msg, length,  0, len_read, 
            &peer_closed_conn,NULL);
      if ( ret_val != CNTLR_CONN_READ_SUCCESS )
      {
         channel->msg_state = OF_CHN_DRAIN_OUT_COMPLETE_MSG;
         if ( peer_closed_conn == TRUE )
         {
            channel->fsm_event= OF_XPRT_PROTO_CHANNEL_CLOSE_EVENT;
         }
         else
         {

            if (channel->fsm_event== OF_XPRT_PROTO_CHANNEL_READ_FAIL_EVENT)
            {
               status=OF_FAILURE;
               break;

            }
            channel->fsm_event= OF_XPRT_PROTO_CHANNEL_READ_FAIL_EVENT;
         }
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"read callback failed");
         channel->drain_out_bytes = channel->msg_hdr.length - sizeof(struct ofp_header);
         cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, NULL);
         /* do clean up */
         status=OF_FAILURE;
         break;
      }

      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"length to be read %d length  read %d", length, *len_read);
      channel->recv_bytes += *len_read;

   }while(0);

   return status;
}

int32_t fsm_xprt_proto_channel_read_error( cntlr_channel_info_t *channel, void *opaque)
{

   int32_t ret_val = OF_SUCCESS;
   int32_t status = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Entered");
   CNTLR_XPRT_PROTO_CHANNEL_PRINT_STATE( channel);

   do
   {
      if (  CNTLR_UNLIKELY(channel == NULL) )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"invalid input");
         status=OF_FAILURE;
         break;
      }
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"channel->drain_out_bytes %d", channel->drain_out_bytes);
      if ( channel->msg_state != OF_CHN_DRAIN_OUT_COMPLETE_MSG)
      {
         break;
      }    
      ret_val=of_drain_out_socket_buf(channel, channel->drain_out_bytes);
      if ( ret_val != OF_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG," of_drain_out_socket_buf failed");
         status=OF_FAILURE;
         break;
      }

   }while(0);

   return status;
}
int32_t fsm_xprt_proto_channel_write_error( cntlr_channel_info_t *channel, void *opaque)
{

   int32_t ret_val = OF_SUCCESS;
   int32_t status = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Entered");
   CNTLR_XPRT_PROTO_CHANNEL_PRINT_STATE( channel);

   do
   {
      if (  CNTLR_UNLIKELY(channel == NULL) )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"invalid input");
         status=OF_FAILURE;
         break;
      }

      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"nothing to do here");

   }while(0);

   return status;
}
int32_t fsm_xprt_proto_channel_write_msg_to_dp(cntlr_channel_info_t *channel,
      struct of_msg           *msg,
      uint16_t                len_already_sent,
      uint16_t                *len_sent_currently
      )
{
   int32_t ret_val = OF_SUCCESS;
   int32_t status = OF_SUCCESS;
   int32_t flags;
   uint8_t              peer_closed_conn = FALSE;

   //OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Entered");
   //CNTLR_XPRT_PROTO_CHANNEL_PRINT_STATE(channel);

   do
   {
      if ( CNTLR_UNLIKELY(channel == NULL))
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"invalid input");
         status=OF_FAILURE;
         break;
      }
      if ( channel->trnsprt_fd == -1 )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"channel fd might be already closed");
         status=OF_FAILURE;
         break;
      }

      ret_val=channel->trnsprt_info->proto_callbacks->transport_write(channel->trnsprt_fd,
            channel->conn_spec_info,
            (msg->desc.start_of_data+len_already_sent),
            (msg->desc.data_len-len_already_sent),MSG_NOSIGNAL,len_sent_currently,
            &peer_closed_conn);
      if (ret_val != OF_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"socket write  callback failed");
         if ( peer_closed_conn == TRUE )
         {
            channel->fsm_event= OF_XPRT_PROTO_CHANNEL_DP_DETACH_EVENT;
            cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, NULL);
            channel->fsm_event= OF_XPRT_PROTO_CHANNEL_CLOSE_EVENT;
         }
         else
         {
            channel->fsm_event= OF_XPRT_PROTO_CHANNEL_WRITE_FAIL_EVENT;
         }

         cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, NULL);
         status=OF_FAILURE;
         break;
      }
      channel->tx_bytes += *len_sent_currently;

   }while(0);

   return status;
}

void fsm_update_channel_state(cntlr_channel_info_t *channel,int8_t fsm_state)
{
   int32_t ret_val=OF_SUCCESS;
   int32_t status=OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Entered");

   channel->fsm_state = fsm_state;
   switch(fsm_state)
   {
      case OF_XPRT_PROTO_CHANNEL_STATE_INIT:
         {
            channel->fsm_state= OF_XPRT_PROTO_CHANNEL_STATE_INIT;
         }
         break;

      case OF_XPRT_PROTO_CHANNEL_STATE_BOUND_TO_DP:
         CNTLR_XPRT_PROTO_SET_CHANNEL_FSM_STATE(channel, OF_XPRT_PROTO_CHANNEL_STATE_BOUND_TO_DP);
         channel->fsm_event= OF_XPRT_PROTO_CHANNEL_LEARN_EVENT;
         cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, NULL);
         break;

      case OF_XPRT_PROTO_CHANNEL_STATE_CLOSE_CONN:
         channel->fsm_event= OF_XPRT_PROTO_CHANNEL_CLOSE_EVENT;
         cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, NULL);
         channel->fsm_state=OF_XPRT_PROTO_CHANNEL_STATE_CLOSE_CONN;
      default:
         break;
   } 
}

void fsm_send_channel_event(cntlr_channel_info_t *channel,int8_t fsm_event)
{

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG," received event is %s", cnlr_xprt_proto_channel_events[fsm_event]);
   channel->fsm_event=fsm_event;
   CNTLR_XPRT_PROTO_CHANNEL_PRINT_STATE(channel);
   switch(fsm_event)
   {
      case  OF_XPRT_PROTO_CHANNEL_LEARN_EVENT:
         //  case  OF_XPRT_PROTO_CHANNEL_DP_ATTACH_EVENT:
         cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, NULL);
         break;
      case   OF_XPRT_PROTO_CHANNEL_READ_EVENT:
         cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, NULL);
         break;
      case   OF_XPRT_PROTO_CHANNEL_WRITE_EVENT:
         cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, NULL);
         break;
      case   OF_XPRT_PROTO_CHANNEL_READ_FAIL_EVENT:
         cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, NULL);
         break;
      case   OF_XPRT_PROTO_CHANNEL_WRITE_FAIL_EVENT:
         cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, NULL);
         break;
      case   OF_XPRT_PROTO_CHANNEL_ERROR_EVENT:
         cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, NULL);
         break;
      case   OF_XPRT_PROTO_CHANNEL_CLOSE_EVENT:
         cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, NULL);
         break;
      case  OF_XPRT_PROTO_CHANNEL_DP_DETACH_EVENT:
         cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, NULL);
         CNTLR_XPRT_PROTO_SET_CHANNEL_FSM_STATE( channel, OF_XPRT_PROTO_CHANNEL_STATE_WAIT_FOR_HANDSHAKE);
         break;
   }
}

int32_t xprt_proto_channel_close( cntlr_channel_info_t *channel)
{
   int32_t               status = OF_SUCCESS;
   int32_t               ret_val = OF_SUCCESS;
   int8_t status_b;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Entered");
   CNTLR_XPRT_PROTO_CHANNEL_PRINT_STATE( channel);

   do
   {
      if (  CNTLR_UNLIKELY(channel == NULL) )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"invalid input");
         status=OF_FAILURE;
         break;
      }

      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"closing channel %d", channel->id);
      channel->fsm_event= OF_XPRT_PROTO_CHANNEL_DP_DETACH_EVENT;
      cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, NULL);
      ret_val=channel->trnsprt_info->proto_callbacks->transport_close(channel->trnsprt_fd, channel->conn_spec_info);
      if ( ret_val != OF_SUCCESS )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"close callback failed");
         status=OF_FAILURE;
         break;
      }
      //      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Setting channel to close state");
      CNTLR_XPRT_PROTO_SET_CHANNEL_FSM_STATE(channel,OF_XPRT_PROTO_CHANNEL_STATE_CLOSE_CONN);
      if((channel->state != OF_TRNSPRT_CHN_CLOSE_INITIATED) && (channel->is_chn_added_2_hash_tbl == TRUE))
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"deleting mchash node");
         MCHASH_DELETE_NODE_BY_REF(channel->trnsprt_info->connection_tbl,
               channel->safe_ref->index,
               channel->safe_ref->magic,
               cntlr_channel_info_t*,
               CONNECTION_TBL_LINK_OFFSET,
               status_b);
         channel->is_chn_added_2_hash_tbl=FALSE;
         channel->state = OF_TRNSPRT_CHN_CLOSE_INITIATED;

         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"mchash deleted channel saferef index %d magic %d", channel->safe_ref->index, channel->safe_ref->magic);
      }

   }while(0);
   return status;
}

