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

/******************************************************************************
 * * * * * * * * * * * * Global Variables * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/

/******************************************************************************
 * * * * * * * * * * * * Function Prototypes * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
void
cntlr_trnsprt_handle_ctrl_thread_msgs(int32_t            fd,
      epoll_user_data_t *info,
      uint32_t           received_events);

int32_t CNTLR_PKTPROC_DATA cntlr_enqueue_msg_2_channel_thread(cntlr_channel_info_t *channel);
void
cntlr_trnsprt_channel_handle_socket_recv_msgs(int32_t            fd,
      epoll_user_data_t *info,
      uint32_t           received_events);

/******************************************************************************
 * * * * * * * * * * * * Function Definitions * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
   void
cntlr_trnsprt_handle_ctrl_thread_msgs(int32_t            fd,
      epoll_user_data_t *info,
      uint32_t           received_events)
{
   struct sockaddr_in           remote_addr;
   uint32_t                     addr_length;
   struct of_msg               *msg        = NULL;
   cntlr_channel_info_t        *channel    = NULL;
   cntlr_thread_comm_info_t     msg_event;
   uint16_t                     len_sent;
   int32_t                      ret_val     = OF_SUCCESS;

   if(info == NULL)
      return;
   channel    = (cntlr_channel_info_t*)info->arg1;
   /*thread_info = (cntlr_thread_info_t*)pInfo->arg2;*/

   addr_length = sizeof(remote_addr);
   ret_val = recvfrom(fd,
         (char*)&msg_event,
         sizeof(cntlr_thread_comm_info_t),
         0,
         (struct sockaddr *)&remote_addr,
         (socklen_t *)&addr_length);
   if(ret_val < 0)
   {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"error in reading the data on controller msg FD");
      perror("read");
      return;
   }

   /* Currently only following command is supported*/
   if(CNTLR_LIKELY(msg_event.command == OF_TRNSPRT_MSG_RCVD))
   {
      CNTLR_RCU_READ_LOCK_TAKE();
      while(!of_MsgDequeue(channel->msg_q_box,msg_event.msg.sender_thread_id,(void**)&msg,TRUE))
      {
         ret_val = cntlr_tnsprt_send_msg_to_dp_channel(channel,msg,TRUE,0,&len_sent);
         if(ret_val == OF_FAILURE)
         {
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR," Error in sending message to DP");
            channel->msg_q_box->bSendEvent = TRUE;
            CNTLR_RCU_READ_LOCK_RELEASE();
            return;
         }
      }
      channel->msg_q_box->bSendEvent = TRUE;
      CNTLR_RCU_READ_LOCK_RELEASE();
   }
}

int32_t channel_socket_recv_msgs_init(cntlr_channel_info_t *channel)
{
   int32_t ret_val = OF_SUCCESS;
   int32_t status = OF_SUCCESS;
   epoll_user_data_t user_data;
   uint32_t           intrested_events;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Entered");

   do
   {
      /*Create message Q box to receive messages from Controller */
      channel->dp_recv_msg_q_box = of_MsgQBoxCreate(OF_MAX_MSGS_IN_CHN_Q_LIST,(cntlr_info.number_of_threads+1));
      if(channel->dp_recv_msg_q_box == NULL)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"Unable to create Message QBox for the channel ");
         status = OF_FAILURE;
         break;
      }

      /* Add Loopback FD to Poll table using API ‘epoll_add_fd’, 
       * This is to handle messages from controller applications running
       * in separate independent thread */
      memset(&user_data,0,sizeof(epoll_user_data_t));
      user_data.arg1 = (void *)channel;
      user_data.arg2 = (void *)channel->thread;
      intrested_events = ePOLL_POLLIN;
      ret_val = cntlr_epoll_add_fd_to_thread(channel->thread,
            channel->dp_recv_msg_q_box->iLoopbackSockFd,
            intrested_events,
            &user_data,
            cntlr_trnsprt_channel_handle_socket_recv_msgs);
      if(ret_val == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"Error in adding Channel(%d) Socket to EPOLL",
               channel->auxiliary_id);
         status = OF_FAILURE;
         break;
      }
   }while(0);

   return status;
}
   void
cntlr_trnsprt_channel_handle_socket_recv_msgs(int32_t            fd,
      epoll_user_data_t *info,
      uint32_t           received_events)
{
   struct sockaddr_in           remote_addr;
   uint32_t                     addr_length;
   cntlr_channel_info_t        *channel    = NULL;
   cntlr_thread_comm_info_t     msg_event;
   int32_t                      ret_val     = OF_SUCCESS;

   if(CNTLR_UNLIKELY(info == NULL))
      return;

   channel    = (cntlr_channel_info_t*)info->arg1;
   /*thread_info = (cntlr_thread_info_t*)pInfo->arg2;*/

   addr_length = sizeof(remote_addr);
   ret_val = recvfrom(fd,
         (char*)&msg_event,
         sizeof(cntlr_thread_comm_info_t),
         0,
         (struct sockaddr *)&remote_addr,
         (socklen_t *)&addr_length);
   if(ret_val < 0)
   {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"error in reading the data on controller msg FD");
      perror("read");
      return;
   }
   /* Currently only following command is supported*/
   if(CNTLR_LIKELY(msg_event.command == OF_TRNSPRT_MSG_RCVD))
   {
      CNTLR_RCU_READ_LOCK_TAKE();
      ret_val = of_process_dp_message(channel,&channel->msg_hdr);
      if(ret_val == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"error in processing dp message");
         channel->dp_recv_msg_q_box->bSendEvent = TRUE;
         CNTLR_RCU_READ_LOCK_RELEASE();
         return;
      }
      channel->dp_recv_msg_q_box->bSendEvent = TRUE;
      CNTLR_RCU_READ_LOCK_RELEASE();
   }
}

int32_t CNTLR_PKTPROC_DATA cntlr_enqueue_msg_2_channel_thread(cntlr_channel_info_t *channel)
{

   of_msg_q_box_t             *dp_recv_msg_q_box;
   int32_t                     ret_val = OF_SUCCESS;
   int32_t                     status = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"enquing ");
   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      dp_recv_msg_q_box = channel->dp_recv_msg_q_box;

      if(dp_recv_msg_q_box->bSendEvent)
      {
         int32_t                     socket;
         cntlr_thread_comm_info_t    msg_event;
         struct sockaddr_in          peer_address;

         socket = of_socket_create(OF_IPPROTO_UDP);
         if(socket == OF_FAILURE)
         {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Socket creation error \r\n", __FUNCTION__,__LINE__);
            perror("socket");
            /*TBD, what 2 do with current msg queued*/
            status = OF_FAILURE;
            break;
         }

         msg_event.command              = OF_TRNSPRT_MSG_RCVD;
         msg_event.msg.sender_thread_id = cntlr_thread_index;

         peer_address.sin_family      = AF_INET;
         peer_address.sin_addr.s_addr = htonl(OF_LOOPBACK_SOCKET_IPADDR);
         peer_address.sin_port        = htons(dp_recv_msg_q_box->usLoopbackPort);

         dp_recv_msg_q_box->bSendEvent = FALSE;
         ret_val = sendto(socket,
               (char *)&msg_event,
               sizeof(cntlr_thread_comm_info_t),
               0,
               (struct sockaddr *)&peer_address,
               sizeof(peer_address));
         if( ret_val <= 0 )
         {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Event send to Channel thread thread failed ");
            /*TBD, what 2 do with current msg queued*/
            close(socket);
            status = OF_FAILURE;
            break;
         }
         close(socket);
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   return status;
}




