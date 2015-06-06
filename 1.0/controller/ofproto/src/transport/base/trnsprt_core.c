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

/*!\file of_cntlr_trnsprt.c
 *\author Rajesh Madabushi <rajesh.madabushi@freescale.com>
 *\date November, 2012  
 * <b>Description:</b>\n 
 * This file contains transport layer functionality definitions
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
 * * * * * * * * * * * * Global Variables * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/

uint32_t  of_aux_conn_seq_no_g;

extern __thread cntlr_thread_info_t *self_thread_info;

extern uint32_t           cntlr_xtn_seq_no_g;

/******************************************************************************
 * * * * * * * * * * * * Function Prototypes * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
extern int32_t timer_create_and_start(struct saferef* container_saferef_p,struct tmr_saferef* tmr_saferef_p, 
      uint8_t periodic_b,tmr_time_t time_out_p, tmr_cbk_func func_p,
      void *arg1_p,void *arg2_p);
extern void mchash_insert_node_in_middle(struct mchash_bucket* bucket_p,struct mchash_dll_node *prev_p,
      struct mchash_dll_node *node_p, struct mchash_dll_node* next_p);

extern int32_t  timer_stop_tmr(struct saferef* container_saferef_p, struct tmr_saferef* tmr_node_saferef_p);



//#define of_debug printf

#define OF_EPOLL_CTXT_STATIC_FDS_SUPPORTED   20

#define OF_TRNSPRT_RECV_BUF_LEN   1024

/* Function Prototype_start *************************************************/



/******************************************************************************
 * * * * * * * * * * * * Function Definitions * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/




/*TBD Free Trnsprt RCU*/
/*TBD*/
   int32_t 
cntlr_remove_transport(of_transport_type_e     trnsprt_type)
{
   return OF_SUCCESS;
}

   int32_t
of_CtrlGetTrnsprtInfo(of_transport_type_e trnsprt_type)
{
   /*TBD MUST use RCU*/
   return OF_SUCCESS;
}

   void
of_send_msg_free_rcu(struct rcu_head *node)
{
   cntlr_send_msg_buf_t *msg_buf = (cntlr_send_msg_buf_t*)((uint8_t*)(node) - sizeof(of_list_node_t));

   if(msg_buf->msg)
      of_check_pkt_mbuf_and_free(msg_buf->msg);	

   of_mem_free(msg_buf); 
}


/*Following routine used for thransmitting OpenFlow message to DP
 * This API is used in case Application running in separate thread
 * instead of running as part of worker thread
 * 
 * Call MUST use RCU read lock and RCU read unlock
 *      */
int32_t CNTLR_PKTPROC_DATA cntlr_enqueue_msg_2_xprt_conn(struct of_msg           *msg,
      cntlr_conn_table_t         *conn_table,
      cntlr_conn_node_saferef_t  *conn_safe_ref)
{
   cntlr_channel_info_t       *channel;
   of_msg_q_box_t             *msg_q_box;
   uint8_t                     hash_tbl_access_status = FALSE;
   int32_t                     retval = OF_SUCCESS;
   int32_t                     status = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"enquing ");
   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      MCHASH_ACCESS_NODE(conn_table,conn_safe_ref->index,conn_safe_ref->magic,channel,hash_tbl_access_status);
      if(hash_tbl_access_status == FALSE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN," Channel safe reference not valid ");
         status = OF_FAILURE;
         break;
      }

      msg_q_box = channel->msg_q_box;
      if(of_MsgEnqueue(msg_q_box,cntlr_thread_index,(void*)msg,TRUE) == OF_FAILURE   )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Message enequeu to QBox failed");
         status = OF_FAILURE;
         break;
      }

      if(msg_q_box->bSendEvent)
      {
         int32_t                     socket;
         cntlr_thread_comm_info_t    msg_event;
         struct sockaddr_in          peer_address;

         socket = of_socket_create(OF_IPPROTO_UDP);
         if(socket == OF_FAILURE)
         {
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Socket creation error ");
            perror("socket");
            /*TBD, what 2 do with current msg queued*/
            status = OF_FAILURE;
            break;
         }

         msg_event.command              = OF_TRNSPRT_MSG_RCVD;
         msg_event.msg.sender_thread_id = cntlr_thread_index;

         peer_address.sin_family      = AF_INET;
         peer_address.sin_addr.s_addr = htonl(OF_LOOPBACK_SOCKET_IPADDR);
         peer_address.sin_port        = htons(msg_q_box->usLoopbackPort);

         msg_q_box->bSendEvent = FALSE;
         retval = sendto(socket,
               (char *)&msg_event,
               sizeof(cntlr_thread_comm_info_t),
               0,
               (struct sockaddr *)&peer_address, 
               sizeof(peer_address));
         if( retval <= 0 )
         {
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR," Event send to Transport layer thread failed !!!");
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



/* Sending message to DP,
 * Call MUST use RCU read lock and RCU read unlock
 */
inline int32_t CNTLR_PKTPROC_FUNC cntlr_send_msg_to_dp(struct of_msg               *msg,
      struct dprm_datapath_entry* datapath,
      cntlr_conn_table_t         *conn_table,
      cntlr_conn_node_saferef_t  *conn_safe_ref,
      void                       *callback_fn,
      void                       *clbk_arg1,
      void                       *clbk_arg2)

{
   uint8_t               is_rec_xtn = FALSE;          
   cntlr_xtn_node_saferef_t xtn_rec_saferef;
   int32_t               retval = OF_SUCCESS;
   int32_t               status = OF_SUCCESS;
   struct ofp_header* header;


   header = (struct ofp_header *)(msg->desc.start_of_data);

   do
   {
#if 1
      if( (header->type == OFPT_ECHO_REQUEST) ||
            (header->type == OFPT_MULTIPART_REQUEST ) ||
            (header->type == OFPT_BARRIER_REQUEST )  ||    
            (header->type == OFPT_GROUP_MOD )      ||
            (header->type == OFPT_METER_MOD )      ||
            (header->type == OFPT_GET_CONFIG_REQUEST  )  ||
            (header->type == OFPT_ROLE_REQUEST)   ||
            (header->type == OFPT_GET_ASYNC_REQUEST ))
#else

         if ( 1 )
#endif 
         {
            retval = cntlr_record_xtn_details(msg,    
                  datapath,
                  callback_fn,
                  clbk_arg1,
                  clbk_arg2,
                  &xtn_rec_saferef);
            if(retval == OF_FAILURE)
            {
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Error in recording transaction ");
               status = OF_FAILURE;
               break;
            }
            is_rec_xtn = TRUE;          
         }
         else
         {
            cntlr_xtn_seq_no_g++;
            if(cntlr_xtn_seq_no_g == 0)
               cntlr_xtn_seq_no_g = 1;
            header->xid = htonl(cntlr_xtn_seq_no_g);
         }

      if((cntlr_thread_index == 0) && (self_thread_info != &master_thread_info))
      {
         retval = cntlr_enqueue_msg_2_xprt_conn(msg,conn_table,conn_safe_ref);
         if(retval == OF_FAILURE)
         {
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Error in enqueuing message to Trnsport thread ");
            status = OF_FAILURE;
            break;
         }
      }
      else
      {
         retval = cntlr_transprt_send_to_dp_locally(msg,conn_table,conn_safe_ref);
         if(retval == OF_FAILURE)
         {
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR," Error in sending message locally ");
            status = OF_FAILURE;
            break;
         }
      }
   }
   while(0);

   if(status == OF_FAILURE)
   {
      if(is_rec_xtn == TRUE)
      {
         cntlr_del_xtn_with_safe_ref(&xtn_rec_saferef);
      }
   }
   return status;
}

inline int32_t CNTLR_PKTPROC_FUNC cntlr_transprt_send_to_dp_locally(struct of_msg              *msg,
      cntlr_conn_table_t         *conn_table,
      cntlr_conn_node_saferef_t  *conn_safe_ref)
{
   uint8_t               hash_tbl_access_status = FALSE;
   cntlr_channel_info_t *channel;
   uint16_t              len_sent;
   int32_t               retval                 = OF_SUCCESS;
   int32_t               status                 = OF_SUCCESS;

   CNTLR_RCU_READ_LOCK_TAKE();
   //OF_LOG_MSG(OF_LOG_XPRT_PROTO,OF_LOG_MOD,"saferef is %d , magic num is %d",conn_safe_ref->index,conn_safe_ref->magic);
   do
   {

      /** Access connection table and get channel pointer in case safe reference pass is valid*/
      MCHASH_ACCESS_NODE(conn_table,conn_safe_ref->index,conn_safe_ref->magic,channel,hash_tbl_access_status);
      if(hash_tbl_access_status == FALSE   )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"Channel safe reference is not valid ");
         status = OF_FAILURE;
         break;
      }

      /*Send message to dp thru the channel opened*/
      retval = cntlr_tnsprt_send_msg_to_dp_channel(channel,msg,TRUE,0,&len_sent);
      if(retval == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Error in sending msg to channel ");
         status = OF_FAILURE;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

#if 0

   if(status == OF_FAILURE)
   {
      /*TBD not to good relase buffer, let it from create*/
      of_msg_release(msg);
   }

#endif

   //return OF_SUCCESS;
   return status;
}

   void
print_msg(struct of_msg *msg)
{
   unsigned char *ptr = msg->desc.start_of_data;
   uint32_t ii;

   ptr = msg->desc.start_of_data;
   for(ii = 0; ii < msg->desc.data_len;ii++,ptr++)
   {
      printf("%d %x ",ii,*ptr);
      if(!(ii%40)) printf("\r\n");
   }
   printf("\r\n");
}

   void
of_add_2_epoll_4_wr_event(cntlr_channel_info_t *channel,
      struct of_msg        *msg,
      uint8_t               is_new_msg,
      uint16_t              len_sent_currently)
{
   cntlr_send_msg_buf_t *send_msg_buf = NULL;
   epoll_user_data_t    user_data;
   uint32_t             intrested_events;
   int32_t              status = OF_SUCCESS;
   int32_t              retval = OF_SUCCESS;
   uchar8_t offset;
   offset = CNTLR_TRNSPRT_MSG_LISTNODE_OFFSET;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"add to epoll for write event");
   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      if(is_new_msg == TRUE)
      {
         send_msg_buf = (cntlr_send_msg_buf_t*)of_mem_calloc(1,sizeof(cntlr_send_msg_buf_t));
         if(send_msg_buf == NULL)
         {
            status = OF_FAILURE;
            break;
         }
         send_msg_buf->length_sent = len_sent_currently;
         send_msg_buf->msg         = msg;

         OF_APPEND_NODE_TO_LIST(channel->send_msg_list,send_msg_buf,offset);

      }

      if(!channel->epoll_wr_enabled)
      {
         intrested_events  = ePOLL_POLLIN | ePOLL_POLLOUT;
         user_data.arg1 = (void *)(channel->trnsprt_info->connection_tbl);
         user_data.arg2 = (void *)(channel->safe_ref);
         retval = cntlr_epoll_modify_fd(channel->trnsprt_fd,
               intrested_events,
               &user_data,
               of_handle_dp_msg);
         if(retval == OF_FAILURE)
         {
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Failed to Add Write FD to epoll context of transport thread ");
            status = OF_FAILURE;
            break;
         }
         channel->epoll_wr_enabled = TRUE;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   if(status == OF_FAILURE)
   {
      if(msg)
	 of_check_pkt_mbuf_and_free(msg);

      if(send_msg_buf != NULL)
         of_mem_free(send_msg_buf);
   }
}

   inline int32_t 
cntlr_tnsprt_send_msg_to_dp_channel(cntlr_channel_info_t *channel,
      struct of_msg *msg,
      uint8_t  is_new_msg,
      uint16_t msg_offset,
      uint16_t *len_sent_currently)
{
   int32_t  retval = OF_SUCCESS;

#if 0
   if(CNTLR_UNLIKELY(channel->State != OF_TRNSPRT_CHN_HANDSHAKE_WITH_CTRL_DONE))
   {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"%s:%d Channel(%d) connection is not alive  \n",__FUNCTION__,__LINE__,channel->auxiliary_id);
      return OF_FAILURE;
   }
#endif
   if(OF_LIST_COUNT(channel->send_msg_list) > 0)
   {
      of_add_2_epoll_4_wr_event(channel,msg,is_new_msg,msg_offset);
   }
   else
   {
      retval = cntlr_tnsprt_try_2_send_msg_on_line(channel,msg,is_new_msg,msg_offset,len_sent_currently);
   }

   return retval;
}

   int32_t
cntlr_tnsprt_try_2_send_msg_on_line(cntlr_channel_info_t *channel,
      struct of_msg *msg,
      uint8_t  is_new_msg,
      uint16_t msg_offset,
      uint16_t *len_sent_currently)
{
   uint16_t len_sent = 0;
   int32_t  retval = OF_SUCCESS;

   retval = of_send_msg_to_xprt(channel,msg,msg_offset,&len_sent);
   if(retval == OF_FAILURE)
   {
      if( (errno == EAGAIN) || (errno == ENOMEM) || (errno == EINTR) )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,
               "Unable to send message adding to epoll to send it at later point of time");
         of_add_2_epoll_4_wr_event(channel,msg,is_new_msg,len_sent);
      }
      else
      {
         //     OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,
         //         "failure case ....deleting message");
         if(msg)
         {
         	 of_check_pkt_mbuf_and_free(msg);
         }
         return OF_FAILURE;
      }

   }
   else
   {
      if(len_sent < msg->desc.data_len)
         of_add_2_epoll_4_wr_event(channel,msg,is_new_msg,len_sent);
      else
      {
         //        OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,
         //            "success case ....deleting message");
	of_check_pkt_mbuf_and_free(msg);
      }
   }

   *len_sent_currently = len_sent;

   return OF_SUCCESS;
}

   inline void 
of_send_all_msgs_waiting_on_chn_to_dp(cntlr_channel_info_t *channel) 
{
   cntlr_send_msg_buf_t       *msg_buf;
   epoll_user_data_t           user_data;
   uint32_t                    intrested_events;
   uint16_t                   len_sent_currently;
   int32_t                     retval = OF_SUCCESS;
   int32_t cnt;

   uchar8_t offset;
   offset = CNTLR_TRNSPRT_MSG_LISTNODE_OFFSET;

   cnt = 0;
   OF_LIST_REMOVE_HEAD_AND_SCAN(channel->send_msg_list, msg_buf,cntlr_send_msg_buf_t*,offset)
   {
      cnt++;

      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"%d  %p %d", cnt,msg_buf->msg,msg_buf->length_sent);
      cntlr_tnsprt_try_2_send_msg_on_line(channel,
            msg_buf->msg,
            FALSE,
            msg_buf->length_sent,
            &len_sent_currently);
      if(len_sent_currently == 0)
      {
	if(msg_buf->msg->desc.data_len == 0)
         {
         continue;
         }
         break;
      }

      msg_buf->length_sent += len_sent_currently;
      if(msg_buf->msg->desc.data_len == msg_buf->length_sent)
      {
         continue;
      }
      else
      {
         if(msg_buf->msg->desc.data_len == 0)
         {
         continue;
         }
         break;
      }
   }
   if(OF_LIST_COUNT(channel->send_msg_list) == 0)
   {
      intrested_events  = ePOLL_POLLIN;
      user_data.arg1 = (void *)(channel->trnsprt_info->connection_tbl);
      user_data.arg2 = (void *)(channel->safe_ref);
      retval = cntlr_epoll_modify_fd(channel->trnsprt_fd,
            intrested_events,
            &user_data,
            of_handle_dp_msg);
      if(retval == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR," Failed to Modify 2 remove Write FD to epoll context of transport thread ");
      }
      channel->epoll_wr_enabled = FALSE;
   }
}


   void
cntlr_trnsprt_close_fds(cntlr_channel_info_t   *channel)
{
   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"entered");
   /* todo AS PER fsm */

   channel->trnsprt_fd = -1;
}
#if 0
   static int32_t 
of_SendDescReqMsg(cntlr_channel_info_t *channel)
{
   of_msg_t *pMsg = NULL;
   int32_t   retval = OF_FAILURE;

   retval = of_BuildDescReqMsg(&pMsg);
   if(retval == OF_FAILURE)
   {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"%s:%d Error in building Desc req message\r\n", __FUNCTION__,__LINE__);
      return OF_FAILURE;
   }

   CNTLR_RCU_READ_LOCK_TAKE();
   retval = cntlr_trnsprt_send_msg_to_dp_channel( pMsg,rcu_dereference(channel),NULL,NULL);
   if(retval == OF_FAILURE)
   {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"%s:%d Error in sending Handshake message\r\n", __FUNCTION__,__LINE__);
      CNTLR_RCU_READ_LOCK_RELEASE();
      return OF_FAILURE;
   }
   CNTLR_RCU_READ_LOCK_RELEASE();

   return OF_SUCCESS;
}
   static int32_t 
of_SendPortDescReqMsg(cntlr_channel_info_t *channel)
{
   of_msg_t *pMsg=NULL;
   int32_t   retval = OF_FAILURE;

   retval = of_BuildPortDescReqMsg(&pMsg);
   if(retval == OF_FAILURE)
   {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"%s:%d Error in building Desc req message\r\n", __FUNCTION__,__LINE__);
      return OF_FAILURE;
   }

   CNTLR_RCU_READ_LOCK_TAKE();
   retval = cntlr_trnsprt_send_msg_to_dp_channel( pMsg,rcu_dereference(channel),NULL,NULL);
   if(retval == OF_FAILURE)
   {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"%s:%d Error in sending Handshake message\r\n", __FUNCTION__,__LINE__);
      CNTLR_RCU_READ_LOCK_RELEASE();
      return OF_FAILURE;
   }
   CNTLR_RCU_READ_LOCK_RELEASE();

   return OF_SUCCESS;
}                          


   static inline int32_t 
of_SendReq2GetDPResources(cntlr_channel_info_t *channel)
{
   int32_t retval = OF_SUCCESS;

#if 0
   retval = of_SendDescReqMsg(channel);
   if(retval == OF_FAILURE)
      return OF_FAILURE;
#endif

   retval = of_SendPortDescReqMsg(channel);
   if(retval == OF_FAILURE)
      return OF_FAILURE;

   return OF_SUCCESS;
}

   int32_t
of_read_msg(int32_t  fd,
      of_msg_t **msg_hdr_p)
{
   of_msg_t *msg;
   of_msg_t *p_recv_buf = NULL;
   uint32_t  peer_ip;
   uint16_t  peer_port;
   int32_t   len_recv;   
   uint8_t   close_conn = FALSE;
   do
   {
      p_recv_buf = channel->pRecvBuf;
      len_recv  = SocketRecvFrom(fd,
            p_recv_buf->pBuffer,
            OF_TRNSPRT_RECV_BUF_LEN,
            0,
            &peer_ip,
            &peer_port);
      if( len_recv <= 0 )
      {
         perror("read");
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"%s:%d Error in reading the data recvd from datapath \r\n",__FUNCTION__,__LINE__);
         /*TBD, Assumed some error cases will not occur, if occur, it is bug need to fix*/
         if( (len_recv == 0) || (errno == ENOTCONN) || (errno == ETIMEDOUT) || (errno == ECONNRESET) ) 
         {
            close_conn = TRUE;
            status = OF_FAILURE;
            break;
         }
         pMsgHdr = (struct ofp_header*)(p_recv_buf->pBuffer); 
         msgLen = ntohs(pMsgHdr->length);
         if(msgLen > p_recv_buf->BufLen)
         {
            of_msg_t      *pCurrRecvBuf = channel->pRecvBuf;

            retval = of_MsgAlloc(&of_CtrlMsgPool_g,msgLen,&channel->pRecvBuf);
            if(retval == OF_FAILURE)
            {
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"%s:%d Memory Allocation Failed - Receive DP msg \n",__FUNCTION__,__LINE__);
               close_conn = TRUE; /*TBD, might be opt better mechanism*/
               status = OF_FAILURE;
               break;
            }
            p_recv_buf                 = channel->pRecvBuf;
            p_recv_buf->TotalDataLen   = msgLen;
            memcpy(p_recv_buf->pBuffer,pCurrRecvBuf->pBuffer,len_recv);
            p_recv_buf->DataLen      = lenRecv;
            of_MsgRelease(pCurrRecvBuf);
         }
      }
   }
   while(0);
}

   int32_t
buffer_msg(cntlr_channel_info_t  *p_channel)
{
   of_msg_t *recv_buf;
}

#endif
