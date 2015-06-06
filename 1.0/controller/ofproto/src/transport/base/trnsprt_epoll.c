
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
 * Description: A network map_entry is logical mapping between network elements.This file provides datapath ID and PortID to NamespaceID and PethInterface  mapping APIs. It also provides APIs to getfirst/getnext/getexact/delete map_entrys. NEM maintains  separate hash table and mempool list for map_entry maintanance for this database.
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
#include "fsl_ext.h"
#include "pktmbuf.h"
/******************************************************************************
 * * * * * * * * * * * * Global Variables * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
extern char cntlr_xprt_protocols[OF_XPRT_PROTO_CHANNEL_STATE_MAX][XPRT_PROTO_MAX_STRING_LEN];
extern char cnlr_xprt_proto_channel_events[OF_XPRT_PROTO_CHANNEL_EVENT_MAX][XPRT_PROTO_MAX_STRING_LEN];

/******************************************************************************
 * * * * * * * * * * * * Function Prototypes * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
void
of_read_and_process_msg_from_dp(cntlr_channel_info_t *channel);
extern of_xprt_proto_fsm_function cntlr_xprt_proto_fsm_handler[OF_XPRT_PROTO_STATE_MAX][OF_XPRT_PROTO_EVENT_MAX];
extern of_xprt_proto_channel_fsm_function cntlr_xprt_proto_channel_fsm_handler[OF_XPRT_PROTO_CHANNEL_STATE_MAX][OF_XPRT_PROTO_CHANNEL_EVENT_MAX];
extern char cnlr_xprt_proto_channel_states[OF_XPRT_PROTO_CHANNEL_STATE_MAX][XPRT_PROTO_MAX_STRING_LEN];

/******************************************************************************
 * * * * * * * * * * * * Function Definitions * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
   void
cntlr_listen_for_new_connections( int32_t   fd,
      epoll_user_data_t *user_data,
      uint32_t           received_events
      )
{
   cntlr_trnsprt_info_t     *trnsprt_info  = (cntlr_trnsprt_info_t*)(user_data->arg1);
   int32_t               status        = OF_SUCCESS;
   int32_t               ret_val        = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "Entered");

   if(CNTLR_UNLIKELY( !(received_events & ePOLL_POLLIN)))
   {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Invalid events on %#x on listen socket",
            received_events);
      return ;
   }

   do
   {
      if(CNTLR_UNLIKELY(trnsprt_info == NULL ))
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"tansprt info is invalid");
         status = OF_FAILURE;
         break;
      }
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"initializing fsm for of proto %s", cntlr_xprt_protocols[trnsprt_info->transport_type]);
      CNTLR_XPRT_PROTO_SET_TRANSPORT_FSM_EVENT(trnsprt_info, OF_XPRT_PROTO_NEW_CONN_EVENT);
      ret_val=cntlr_xprt_proto_fsm_handler[trnsprt_info->fsm_state][trnsprt_info->fsm_event](trnsprt_info,  NULL);
      if(ret_val == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"cntlr_xprt_proto_fsm_handler failed");
         status = OF_FAILURE;
         break;
      }

   } while(0);


   return;
}

   void
cntlr_listen_for_dp_msgs_for_shared_fd( int32_t   fd,
      epoll_user_data_t *user_data,
      uint32_t           received_events
      )
{
   cntlr_trnsprt_info_t     *trnsprt_info  = (cntlr_trnsprt_info_t*)(user_data->arg1);
   int32_t               status        = OF_SUCCESS;
   int32_t               ret_val        = OF_SUCCESS;
   cntlr_channel_info_t *channel=NULL;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "Entered");
   if(CNTLR_UNLIKELY( !(received_events & ePOLL_POLLIN)))
   {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Invalid events on %#x on listen socket",
            received_events);
      return ;
   }

   do
   {
      if(CNTLR_UNLIKELY(trnsprt_info == NULL ))
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"tansprt info is invalid ");
         status = OF_FAILURE;
         break;
      }
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"recieved event  for  proto %s", cntlr_xprt_protocols[trnsprt_info->transport_type]);
      CNTLR_XPRT_PROTO_SET_TRANSPORT_FSM_EVENT(trnsprt_info, OF_XPRT_PROTO_READ_EVENT);

      ret_val=cntlr_xprt_proto_fsm_handler[trnsprt_info->fsm_state][trnsprt_info->fsm_event](trnsprt_info,(void **) &channel);
      if ((channel  == NULL) || ( ret_val == OF_FAILURE))
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"cntlr_xprt_proto_fsm_handler failed");
         status = OF_FAILURE;
         break;
      }

      CNTLR_XPRT_PROTO_SET_CHANNEL_FSM_EVENT(channel,OF_XPRT_PROTO_CHANNEL_READ_EVENT);
      CNTLR_XPRT_PROTO_CHANNEL_PRINT_STATE( channel);
      ret_val=cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, NULL);
      if(channel->msg_hdr.version != OFP_VERSION)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"Invalid Open Flow message version = %d is received",
               channel->msg_hdr.version);
         status = OF_FAILURE;
         break;
      }
#ifdef OF_XPRT_PROTO_UDP_LOADBALANCE_SUPPORT
      ret_val=cntlr_enqueue_msg_2_channel_thread(channel);
      if(ret_val == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"error in enqueing message to channel thread"); 
         status = OF_FAILURE;
         break;
      }
#else
      ret_val = of_process_dp_message(channel,&channel->msg_hdr);
      if(ret_val == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"error in processing dp message"); 
         status = OF_FAILURE;
         break;
      }
#endif
   } while(0);

   return;
}


   void
of_handle_dp_msg(int32_t fd,
      epoll_user_data_t *info,
      uint32_t received_events)
{

   cntlr_conn_table_t            *conn_table  = NULL;
   cntlr_ch_safe_ref_t           *ch_safe_ref = NULL;
   cntlr_channel_info_t       *channel     = NULL;
   uint8_t                     is_ch_access_status = FALSE;
   //uint8_t                     is_ch_delete_status = FALSE;
   int32_t               status        = OF_SUCCESS;
   int32_t               ret_val        = OF_SUCCESS;

   cntlr_assert(info);

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"entered");

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      conn_table   = (cntlr_conn_table_t*)info->arg1;
      ch_safe_ref  = (cntlr_ch_safe_ref_t*)info->arg2;

      MCHASH_ACCESS_NODE(conn_table,ch_safe_ref->index,ch_safe_ref->magic,channel,is_ch_access_status);
      if( CNTLR_UNLIKELY(is_ch_access_status == FALSE) )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"Channel not available safe reference, might be already deleted ");
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"channel fd %d saferef index %d magic %d", fd, ch_safe_ref->index, ch_safe_ref->magic);
         break;
      }

      if( (received_events & ePOLL_POLLHUP) || (received_events & ePOLL_POLLERR) )
      {
         CNTLR_XPRT_PROTO_SET_CHANNEL_FSM_EVENT(channel,OF_XPRT_PROTO_CLOSE_EVENT);
         CNTLR_XPRT_PROTO_CHANNEL_PRINT_STATE( channel);
         ret_val=cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, NULL);
         if(ret_val == OF_FAILURE)
         {
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"cntlr_xprt_proto_fsm_handler failed");
            status = OF_FAILURE;
            break;
         }
      }

      if(received_events & ePOLL_POLLIN)
         of_read_and_process_msg_from_dp(channel);
      else if(received_events & ePOLL_POLLOUT)
         of_send_all_msgs_waiting_on_chn_to_dp(channel);

   } while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   return;
}

   void
of_read_and_process_msg_from_dp(cntlr_channel_info_t *channel)
{

   int32_t                     ret_val = OF_SUCCESS;
   int32_t                     status = OF_SUCCESS;
   uint16_t                    len_recv;
   cntlr_trnsprt_info_t *trnsprt_info;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_INFO,"Entered");

   do

   {
      if ( CNTLR_UNLIKELY(channel == NULL ))
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"invalid input");
         status = OF_FAILURE;
         break;
      }
      trnsprt_info= channel->trnsprt_info; 
      CNTLR_XPRT_PROTO_SET_CHANNEL_FSM_EVENT(channel, OF_XPRT_PROTO_CHANNEL_READ_EVENT);
      CNTLR_XPRT_PROTO_CHANNEL_PRINT_STATE( channel);
      // CHannel close_state handling -- Vivek
      if(channel->fsm_state == OF_XPRT_PROTO_CHANNEL_STATE_CLOSE_CONN)
	{
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Channel in close conn state");
         status = OF_FAILURE;
         break;
	}	
     ret_val=cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, NULL);
      if(ret_val == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"cntlr_xprt_proto_fsm_handler failed");
         status = OF_FAILURE;
         break;
      }

      if((channel->msg_hdr.version != OFP_VERSION) &&
        (channel->msg_hdr.version != FSLX_VERSION))
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"Invalid Open Flow message version = %d is received",
               channel->msg_hdr.version);
         OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(channel);
         return;
      }
      if((channel->msg_hdr.type != OFPT_EXPERIMENTER) &&
        (channel->msg_hdr.version == FSLX_VERSION))
      {
	      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"Invalid Open Flow message msg type = %d for FSLX version received",
			      channel->msg_hdr.type);
	      if((channel->msg_hdr.length)&& ((channel->msg_hdr.length - sizeof(struct ofp_header)) > 0))
	      {
		      if(of_drain_out_socket_buf(channel, channel->msg_hdr.length - sizeof(struct ofp_header)) != OF_SUCCESS)
		      {
			      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,
					      "Flushing TCP Buufer failure, controller will not be able to read proper messages\r\n");
		      }
	      }
	      if(channel->msg_buf != NULL)
	      {
		      of_check_pkt_mbuf_and_free(channel->msg_buf);
	      }

	      OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(channel);
         return;
      }
      if((channel->msg_state  == OF_CHN_START_READING_MSG) || (channel->msg_state == OF_CHN_RCVD_COMPLETE_MSG))
      {
      ret_val = of_process_dp_message(channel,&channel->msg_hdr);
      }
      else if(channel->msg_state == OF_CHN_WAITING_FOR_COMPLETE_MSG)       
      {
	      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"OF_CHN_WAITING_FOR_COMPLETE_MSG");
	      return;
      }

      if(ret_val == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"error in processing dp message");
         return;
      }
   }while(0);
}


   void        
cntlr_trnsprt_handle_handshake(int32_t            fd,
      epoll_user_data_t  *info,
      uint32_t            received_events)
{           
   int32_t                     ret_val = OF_SUCCESS;
   int32_t                     status = OF_SUCCESS;
   struct ofp_switch_features   sw_feature_msg;
   cntlr_thread_info_t         *thread_info;
   cntlr_channel_info_t        *channel;
   int32_t flags;

   if(CNTLR_UNLIKELY(info == NULL))
      return;

   channel     = (cntlr_channel_info_t*)(info->arg1);
   thread_info = (cntlr_thread_info_t*)(info->arg2);

   do
   {
      if (CNTLR_UNLIKELY( channel == NULL ))
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"invalid input");
         status = OF_FAILURE;
         break;
      }
      if (( received_events & ePOLL_POLLHUP) || (received_events & ePOLL_POLLERR))
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"recieved poll error/hup events");
         status = OF_FAILURE;
         break;
      }

      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"handling handshake for proto %s", cntlr_xprt_protocols[channel->trnsprt_info->transport_type]);
      CNTLR_XPRT_PROTO_SET_CHANNEL_FSM_EVENT(channel,OF_XPRT_PROTO_CHANNEL_READ_EVENT);

      CNTLR_XPRT_PROTO_CHANNEL_PRINT_STATE( channel);

      if ( channel->state == OF_TRNSPRT_CHN_SENT_HELLO_TO_DP )
      {
	      flags=MSG_PEEK;
	      ret_val=cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, &flags);
      }
      else
      {
	      ret_val=cntlr_xprt_proto_channel_fsm_handler[channel->fsm_state][channel->fsm_event](channel, NULL);
      }
      if(ret_val == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"cntlr_xprt_proto_fsm_handler failed");
         status = OF_FAILURE;
         break;
      }

      ret_val=of_process_handshake_msg(channel);
      if(ret_val == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"of_process_handshake_msg failed");
         status = OF_FAILURE;
         break;
      }


   } while(0);


   return;
}

