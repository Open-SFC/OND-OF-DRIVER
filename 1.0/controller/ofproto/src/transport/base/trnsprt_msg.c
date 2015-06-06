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

/* File  :      tranport_msg.c
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

//uint32_t  uiKeepAliveCount_g = 0; 
/******************************************************************************
 * * * * * * * * * * * * Function Prototypes * * * * * * * * * * * * * * * * * *
 *******************************************************************************/


int32_t
of_process_feature_reply_msg(cntlr_channel_info_t        *channel,
      struct ofp_switch_features  *sw_feature_msg);

int32_t
of_process_handshake_msg(cntlr_channel_info_t        *channel);

void CNTLR_PKTPROC_FUNC of_transprt_send_keep_alive_msg(void *cbk_arg1,
      void *cbk_arg2);

extern void
of_handle_dp_msg(int32_t fd,
      epoll_user_data_t *info,
      uint32_t received_events);
void of_handle_hello_message (cntlr_channel_info_t       *channel,
      struct dprm_datapath_entry *datapath,
      struct ofp_header          *msg_hdr);
void of_handle_feature_reply_message (cntlr_channel_info_t       *channel,
      struct dprm_datapath_entry *datapath,
      struct ofp_header          *msg_hdr);
typedef void (*cntlr_msg_cbk_fn)(cntlr_channel_info_t       *channel,
      struct dprm_datapath_entry *datapath,
      struct ofp_header          *msg_hdr);
void CNTLR_PKTPROC_FUNC of_transprt_check_keepalive_response_msg(void *cbk_arg1, void *cbk_arg2);
/******************************************************************************
 * * * * * * * * * * * * Global Variables * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
extern char  cntlr_xprt_protocols[CNTLR_TRANSPORT_TYPE_MAX][XPRT_PROTO_MAX_STRING_LEN];
extern char cnlr_xprt_proto_channel_events[OF_XPRT_PROTO_CHANNEL_EVENT_MAX][XPRT_PROTO_MAX_STRING_LEN];
extern char cnlr_xprt_proto_channel_states[OF_XPRT_PROTO_CHANNEL_STATE_MAX][XPRT_PROTO_MAX_STRING_LEN];
extern uint32_t cntlr_conn_hash_table_init_value_g;
extern void *cntlr_channel_safe_ref_memPool_g;
extern cntlr_msg_cbk_fn  cntlr_cbk[];
cntlr_msg_cbk_fn msg_handle;
extern cntlr_instance_t cntlr_instnace;
/******************************************************************************
 * * * * * * * * * * * * Function Definitions * * * * * * * * * * * * * * * * * 
 *******************************************************************************/

   int32_t
cntlr_trnsprt_send_handshake_msg(cntlr_channel_info_t  *channel)
{

   struct of_msg *msg = NULL;
   struct of_msg_descriptor *msg_desc;
   uint16_t  len_sent;
   struct ofp_header *handshake_msg;
   int32_t retval = OF_SUCCESS;
   int32_t status = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"sending handshake message");
   do
   {
      /*Allocate Message buffer*/
      msg  = ofu_alloc_message(OFPT_FEATURES_REQUEST,
            sizeof(struct ofp_header));
      if(msg == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR," Allocation OF Message failed");
         status = OF_FAILURE;
         break;
      }
      msg_desc = &msg->desc;

      /*Build message content*/
      handshake_msg = (struct ofp_header *)(msg_desc->start_of_data);
      handshake_msg->version  = OFP_VERSION;
      handshake_msg->type     = OFPT_FEATURES_REQUEST;
      handshake_msg->xid      = 0;
      msg_desc->data_len      = sizeof(struct ofp_header);
      handshake_msg->length   = htons(msg_desc->data_len);

      /*Send the message directly on the transport channel locally,
       * still channel is  not established , it is not pushed to
       * connection table and not added to DP.
       */

      /*Length sent is not verified ????*/
      retval = of_send_msg_to_xprt(channel,msg,0,&len_sent);
      if(retval == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR," Error in sending Handshake message");
         status = OF_FAILURE;
         break;
      }
   }
   while(0);

   if(msg != NULL)
      msg_desc->free_cbk(msg);

   return OF_SUCCESS;
}

   int32_t
cntlr_trnsprt_send_hello_msg(cntlr_channel_info_t  *channel)
{

   struct of_msg *msg = NULL;
   struct of_msg_descriptor *msg_desc;
   struct ofp_header *hello_msg;
   uint16_t len_sent;
   int32_t retval = OF_SUCCESS;
   int32_t status = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"sending hello message");
   do
   {
      /*Allocate Message buffer*/
      msg  = ofu_alloc_message(OFPT_HELLO,
            sizeof(struct ofp_header));
      if(msg == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Allocation OF Message failed");
         status = OF_FAILURE;
         break;
      }
      msg_desc = &msg->desc;

      /*Build message content*/
      hello_msg = (struct ofp_header *)(msg_desc->start_of_data);
      hello_msg->version  = OFP_VERSION;
      hello_msg->type     = OFPT_HELLO;
      hello_msg->xid      = 0;
      msg_desc->data_len  = sizeof(struct ofp_header);
      hello_msg->length   = htons(msg_desc->data_len);

      /*Send the message directly on the transport channel locally,
       * still channel is  not established , it is not pushed to
       * connection table and not added to DP.
       */

      /*Length sent is not verified ????*/
      retval = of_send_msg_to_xprt(channel,msg,0,&len_sent);
      if(retval == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR," Error in sending hello message");
         status = OF_FAILURE;
         break;
      }
   }
   while(0);

   if(msg != NULL)
      msg_desc->free_cbk(msg);

   return OF_SUCCESS;
}



   int32_t
of_process_feature_reply_msg(cntlr_channel_info_t        *channel,
      struct ofp_switch_features  *sw_feature_msg)
{
   epoll_user_data_t            user_data;
   int32_t                      retval = OF_SUCCESS;
   int32_t                      status = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"entered");
   do
   {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_INFO,"CHANNEL id %d fd %d saferef index %d magic %d",
            channel->id, channel->trnsprt_fd, channel->safe_ref->index, channel->safe_ref->magic);

      CNTLR_XPRT_PROTO_CHANNEL_PRINT_STATE( channel);
      if (channel->is_chn_added_to_dp == TRUE) 
      { 
         OF_LOG_MSG(OF_LOG_XPRT_PROTO,OF_LOG_WARN,"Channel=%d  is already added to datapath", channel->id);
         OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(channel);
         break;
      }  
      channel->auxiliary_id = sw_feature_msg->auxiliary_id;


      /*Add Channel info to  data_path*/
      OF_LOG_MSG(OF_LOG_XPRT_PROTO,OF_LOG_DEBUG,"Datapath handle is %llx",
            ntohll(sw_feature_msg->datapath_id));
      retval = of_add_channel_to_dp(channel->trnsprt_info->connection_tbl,
           (cntlr_conn_node_saferef_t *) (channel->safe_ref),
            sw_feature_msg,
            &(channel->datapath),
            &channel->dp_handle);
      if(retval != OF_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR," Error in adding Channel(%d) to  Datapath ", 
               sw_feature_msg->auxiliary_id);
         status = retval;
         break;
      }
      //      channel->is_chn_added_to_dp = TRUE;
      //  fsm_update_channel_state(channel, OF_XPRT_PROTO_CHANNEL_STATE_BOUND_TO_DP);
      fsm_send_channel_event(channel,OF_XPRT_PROTO_CHANNEL_DP_ATTACH_EVENT);

      OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(channel);
      if(channel->auxiliary_id == OF_TRNSPRT_MAIN_CONN_ID)
      { 

         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG," Configuring in case of main conn  dp handle is %d",channel->dp_handle); 
         /*In case of main connection configure switch*/
         retval= cntlr_configure_switch(channel->dp_handle);
         if(retval == OF_FAILURE)
         {
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR," Error in configuring switch");
            status = OF_FAILURE;
            break;
         }

         /*In case of main connection get port description of dp*/
         cntlr_send_port_desc_request(channel->dp_handle);

         /*Get the switch info after feature reply */
         cntlr_send_swinfo_desc_request_after_feature_reply(channel->dp_handle);

         channel->keep_alive_state = OF_TRNSPRT_CHN_CONN_KEEP_ALIVE_MSG_READ_TO_SEND;
         retval = timer_create_and_start(NULL,
               &channel->keepalive_timer,
               CNTLR_PEIRODIC_TIMER,
               OF_TRNSPRT_KEEP_ALIVE_TIME_PERIOD,
               of_transprt_send_keep_alive_msg,
               channel,NULL);
         if(retval == OF_FAILURE)
         {
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR," Error in creating transaction records cleanup  timer");
            status = OF_FAILURE;
            break;
         }

      }
      OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(channel);
   }
   while(0);

   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"status is %d",status);
   if(status == OF_FAILURE)
   {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR," Error : DP failure");
      fsm_send_channel_event(channel,OF_XPRT_PROTO_CHANNEL_CLOSE_EVENT);

      return OF_FAILURE;
   }
   else if(status == OF_FAILURE_DP_INVALID){
      OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(channel);
      fsm_send_channel_event(channel,OF_XPRT_PROTO_CHANNEL_CLOSE_EVENT);
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR," Error : Invalid DP or no DP available");
      return OF_FAILURE;   
   }

   return OF_SUCCESS;
}

   int32_t
of_process_handshake_msg(cntlr_channel_info_t        *channel)

{
   int32_t                     ret_val = OF_SUCCESS;
   int32_t                      status = OF_SUCCESS;
   struct ofp_header *p_msg_hdr=&(channel->msg_hdr);
   uint8_t *pkt=&(channel->msg_hdr);
#if 0
   cntlr_trnsprt_info_t   *trnsprt_info;
#endif

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "entered");

   do
   {
      if(p_msg_hdr->version != OFP_VERSION)
      {
#if 0
         /* Check whether it is ssl message */
         if ( (*(pkt+0) == 0x16)&& (*(pkt+1) == 0x03) && ((pkt+2) == 0x00|| *(pkt+2)== 0x01 || *(pkt+2) == 0x02))
         {
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"might be ssl message");
            trnsprt_info=cntlr_get_equal_tls_transport_info(channel->trnsprt_info);
            if ( trnsprt_info == NULL )
            {
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "transport info not found");
            }
            //CNTLR_XPRT_PROTO_SET_TRNSPRT_INFO_FOR_CHANNEL(channel,trnsprt_info);
            CNTLR_XPRT_PROTO_SET_TRANSPORT_FSM_EVENT(trnsprt_info, OF_XPRT_PROTO_NEW_CONN_EVENT);
            ret_val=cntlr_xprt_proto_fsm_handler[trnsprt_info->fsm_state][trnsprt_info->fsm_event](trnsprt_info,  NULL);
            if(ret_val == OF_FAILURE)
            {
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"cntlr_xprt_proto_fsm_handler failed");
               status = OF_FAILURE;
               break;
            }

         }
#endif
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"unsupported Open Flow message version = %d is received closing connection",
               p_msg_hdr->version);
         fsm_send_channel_event(channel,OF_XPRT_PROTO_CHANNEL_CLOSE_EVENT);
         status=OF_FAILURE;
         break;
      }
      if (!((p_msg_hdr->type ==   OFPT_HELLO) ||
               (p_msg_hdr->type == OFPT_FEATURES_REPLY ) || 
               (p_msg_hdr->type == OFPT_ECHO_REQUEST )))
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "invalid message");
         fsm_send_channel_event(channel, OF_XPRT_PROTO_CHANNEL_CLOSE_EVENT);
         status=OF_FAILURE; 
         break;
      }
#if 0
      if (p_msg_hdr->type >= OF_MSG_TYPE_LEN)
      {      
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Invalid message type=%d ",p_msg_hdr->type);
         status=OF_FAILURE; 
         break;
      }
#endif            
      msg_handle = cntlr_cbk[p_msg_hdr->type];
      if(msg_handle == NULL)
      {

         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Invalid message handlei for messate type %d",p_msg_hdr->type);
         status=OF_FAILURE; 
         break;
      }
      msg_handle(channel,channel->datapath,p_msg_hdr);
   }while(0);

   return status;
}


void of_handle_hello_message (cntlr_channel_info_t       *channel,
      struct dprm_datapath_entry *datapath,
      struct ofp_header          *p_msg_hdr)
{
   int32_t ret_val = OF_SUCCESS;
   int32_t status = OF_SUCCESS;
   char buff[1024];
   uint16_t                     len_msg_rcv;
   uint16_t length;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "entered");
   do
   {
      if ( p_msg_hdr->type != OFPT_HELLO)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"not a hellow message... error");
         status=OF_FAILURE;
         break;

      }
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"hellow received");

      if(p_msg_hdr->version != OFP_VERSION)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," unsupported Open Flow message version = %d ",
               channel->msg_hdr.version);
         fsm_send_channel_event(channel, OF_XPRT_PROTO_CHANNEL_ERROR_EVENT);
         return;
      }
      length=ntohs(p_msg_hdr->length);
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"header length %d", length);
      if(length > sizeof(struct ofp_header))
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"hellow message contain info other than header");
         ret_val = fsm_xprt_proto_channel_read_msg_from_dp( channel,
               (uint8_t *)buff, length- sizeof(struct ofp_header),
               &len_msg_rcv );
         if(ret_val != OF_SUCCESS)
         {
            status=OF_FAILURE;
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"error in reading feature reply message ");
            break;
         }
         /* need to handle secure connections here -TBD*/
      }

      OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(channel);
 
     /*Send handshake message to datpath*/
      if ( channel->state == OF_TRNSPRT_CHN_SENT_HELLO_TO_DP )
      {
         ret_val = cntlr_trnsprt_send_handshake_msg(channel);
         if(ret_val == OF_FAILURE)
         {
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Error in sending Handshake message");
            status=OF_FAILURE;
            break;
         }

         channel->state = OF_TRNSPRT_CHN_CONN_REQ_HANDSHAKE_WITH_DP;
      } 
      else 
      { 
         cntlr_trnsprt_send_hello_msg(channel);      
         channel->state = OF_TRNSPRT_CHN_SENT_HELLO_TO_DP;     
         OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(channel);

         ret_val = cntlr_trnsprt_send_handshake_msg(channel);
         if(ret_val == OF_FAILURE)
         {
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Error in sending Handshake message");
            status=OF_FAILURE;
            break;
         }
         channel->state = OF_TRNSPRT_CHN_CONN_REQ_HANDSHAKE_WITH_DP;
      }
      OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(channel);

   }while (0);
   return ;
}


void of_handle_feature_reply_message (cntlr_channel_info_t       *channel,
      struct dprm_datapath_entry *datapath,
      struct ofp_header          *p_msg_hdr)
{
   int32_t ret_val = OF_SUCCESS;
   int32_t status = OF_SUCCESS;
   struct ofp_switch_features   sw_feature_msg;
   uint16_t                     len_msg_rcv;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "entered");
   do
   {
      if ( p_msg_hdr->type != OFPT_FEATURES_REPLY)
      {

         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"not a feature reply message... error");
         status=OF_FAILURE;
         break;

      }
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"feature reply received");
      ret_val = fsm_xprt_proto_channel_read_msg_from_dp( channel,
            (uint8_t*)((char*)&sw_feature_msg + sizeof(struct ofp_header)),
            (sizeof(struct ofp_switch_features) - sizeof(struct ofp_header)),
            &len_msg_rcv );
      if(ret_val != OF_SUCCESS)
      {
         status=OF_FAILURE;
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"error in reading feature reply message ");
         break;
      }
      ret_val = of_process_feature_reply_msg(channel,&sw_feature_msg);
      if(ret_val == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Error in processing feature reply ");
         status=OF_FAILURE;
         break;
      }
      channel->state = OF_TRNSPRT_CHN_HANDSHAKE_WITH_DP_DONE;
      break;

   }while(0);

   return;
}


void CNTLR_PKTPROC_FUNC of_transprt_send_keep_alive_msg(void *cbk_arg1,
      void *cbk_arg2)
{
   cntlr_channel_info_t    *channel = (cntlr_channel_info_t*)cbk_arg1;
   int32_t                  retval = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Datapath ID %llx ",   channel->datapath->dpid);

   if(channel->keep_alive_state != OF_TRNSPRT_CHN_CONN_KEEP_ALIVE_MSG_READ_TO_SEND)
   {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR," Error in channel KEEP alive state");
   }
   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG," Channel dp handle is %d",channel->dp_handle);
   retval = of_send_echo_msg_on_channel(channel->dp_handle,0);
   if(retval != OF_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR," Error in channel KEEP alive state");
      return;
   }

   channel->keep_alive_state = OF_TRNSPRT_CHN_CONN_KEEP_ALIVE_MSG_SENT;

   retval = timer_create_and_start(NULL,
         &channel->keepalive_resptimer,
         CNTLR_NOT_PEIRODIC_TIMER,
         OF_TRNSPRT_KEEP_ALIVE_RESP_WAIT_TIME,
         of_transprt_check_keepalive_response_msg,
         channel,NULL);
   if(retval == OF_FAILURE)
   {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,
                 "Error in creating  find keep alive reponse status timer");
      return;
   }

   return;
}


void CNTLR_PKTPROC_FUNC of_transprt_check_keepalive_response_msg(void *cbk_arg1, void *cbk_arg2)
{
   cntlr_channel_info_t    *channel = (cntlr_channel_info_t*)cbk_arg1;
   int32_t               status = OF_SUCCESS;
   int32_t               retval = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_INFO,"Entered");
   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," uiKeepAliveCount : %d \r\n",channel->uiKeepAliveCount);
   do
   {
      if( (!(channel->keep_alive_state & OF_TRNSPRT_CHN_CONN_KEEP_ALIVE_MSG_RESP_RCVD)) &&
          (channel->keep_alive_state & OF_TRNSPRT_CHN_CONN_KEEP_ALIVE_MSG_SENT)     &&
          (channel->uiKeepAliveCount > 4)
            )
      {

         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"keep alive failed, sending channel close event");
         printf("Keep Alive failed, Closing the connection to the Datapath=%llx \r\n", channel->datapath->dpid);
         fsm_send_channel_event(channel, OF_XPRT_PROTO_CHANNEL_CLOSE_EVENT);
         /* No keep-alive response received, closing connection*/
         return;
      }
      else
      {
	      retval= timer_stop_tmr(NULL, &channel->keepalive_resptimer);
	      if(retval == OF_FAILURE)
	      {
		      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR," Error in stoping keepalive_resptimer");
		      status = OF_FAILURE;
		      break;
	      }
	      if(channel->keep_alive_state & OF_TRNSPRT_CHN_CONN_KEEP_ALIVE_MSG_RESP_RCVD)
	      {
		      channel->uiKeepAliveCount=0;
		      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"uiKeepAliveCount_g set to Zero : %d \r\n",channel->uiKeepAliveCount);
	      }
	      else
		      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"uiKeepAliveCount_g : %d \r\n",channel->uiKeepAliveCount);
	      channel->keep_alive_state = OF_TRNSPRT_CHN_CONN_KEEP_ALIVE_MSG_READ_TO_SEND;
      }
   }while(0);

   return;
}
