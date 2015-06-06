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
#include "pktmbuf.h"
/******************************************************************************
 * * * * * * * * * * * * Global Variables * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
extern void *cntlr_trnsprt_channel_mempool_g;
extern void *cntlr_channel_safe_ref_memPool_g;
uint32_t cntlr_conn_hash_table_init_value_g;
extern int32_t cntlr_xprt_channel_id_g;

/******************************************************************************
 * * * * * * * * * * * * Function Prototypes * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
static inline void
cntlr_trnsprt_conn_free_rcu(struct rcu_head *node);

#define CNTLR_TRANSPORT_UDP_THREAD_ID 1
/******************************************************************************
 * * * * * * * * * * * * Function Definitions * * * * * * * * * * * * * * * * * *
 *******************************************************************************/

int32_t
cntlr_trnsprt_create_channel( 
      uint8_t                 trnsprt_type,
      int32_t                 trnsprt_fd,
      ippaddr_t               dp_ip,
      uint16_t                dp_port,
      cntlr_channel_info_t  **p_p_channel)
{
   cntlr_channel_info_t      *channel;
   uint8_t                    is_heap;
   uint32_t                   id;
   uint32_t                   thrd_assgn_2_chn;
   epoll_user_data_t          user_data;
   uint32_t                   intrested_events;
   int32_t                    ret_val = OF_SUCCESS;
   int32_t                    status = OF_SUCCESS;

   do
   {
      if(mempool_get_mem_block(cntlr_trnsprt_channel_mempool_g,
               (unsigned char **)&channel,
               (unsigned char *)&is_heap) != OF_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Channel memory alloc failure");
         status = OF_FAILURE;
         break;
      }

      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"creating channel for fd %d",trnsprt_fd);

      //memset(channel,0,sizeof(cntlr_channel_info_t));
      channel->is_heap            = is_heap;
      channel->is_chn_added_to_dp = FALSE;
      channel->trnsprt_type       = trnsprt_type;
      channel->trnsprt_fd         = trnsprt_fd;
      channel->auxiliary_id       = 0; 
      /* This will be updated later once received response to 
         feature request message*/

      channel->state        = -1;
      fsm_update_channel_state(channel, OF_XPRT_PROTO_STATE_INIT);
      OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(channel);

      channel->keep_alive_state= OF_TRNSPRT_CHN_CONN_KEEP_ALIVE_NOT_READY; /*Atmaram */


      //OF_LIST_INIT(channel->send_msg_list,of_send_msg_free_rcu);
      OF_LIST_INIT(channel->send_msg_list,NULL);
      channel->epoll_wr_enabled = FALSE;
#ifndef OF_XPRT_PROTO_UDP_LOADBALANCE_SUPPORT
      if ( trnsprt_type == CNTLR_TRANSPORT_TYPE_UDP )
      {
         thrd_assgn_2_chn = CNTLR_TRANSPORT_UDP_THREAD_ID;
      }
      else
      {
         /* Channel afinity to least loaded transport layer thread*/
         thrd_assgn_2_chn =
            (id = (++cntlr_info.curr_thrd_2_which_ch_assnd) % cntlr_info.number_of_threads) ?
            id : cntlr_info.number_of_threads;
      }
#else
      /* Channel afinity to least loaded transport layer thread*/
      thrd_assgn_2_chn =
         (id = (++cntlr_info.curr_thrd_2_which_ch_assnd) % cntlr_info.number_of_threads) ?
         id : cntlr_info.number_of_threads;
#endif

      channel->thread  =  cntlr_get_thread_info(thrd_assgn_2_chn);
      if(CNTLR_UNLIKELY(!channel->thread))
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"Invalid Transport layer thread(%d) to assign Channel ",
               thrd_assgn_2_chn);
         status = OF_FAILURE;
         break;
      }

      /*Create message Q box to receive messages from Controller */
      channel->msg_q_box = of_MsgQBoxCreate(OF_MAX_MSGS_IN_CHN_Q_LIST,(cntlr_info.number_of_threads+1));
      if(channel->msg_q_box == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"Unable to create Message QBox for the channel ");
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
            channel->msg_q_box->iLoopbackSockFd,
            intrested_events,
            &user_data,
            cntlr_trnsprt_handle_ctrl_thread_msgs);
      if(ret_val == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"Error in adding Channel(%d) Socket to EPOLL",
               channel->auxiliary_id);
         status = OF_FAILURE;
         break;
      }

#ifndef OF_XPRT_PROTO_UDP_LOADBALANCE_SUPPORT
      if ( trnsprt_type == CNTLR_TRANSPORT_TYPE_UDP )
      {
         channel_socket_recv_msgs_init(channel);
      }
#endif
      channel->state = OF_TRNSPRT_CHN_CONN_ESTBD;
   }
   while(0);

   if(status == OF_FAILURE)
   {
      if(channel != NULL)
      {
         cntlr_trnsprt_conn_close(channel);
         channel=NULL;
      }
      return OF_FAILURE;
   }

   *p_p_channel = channel;
   return OF_SUCCESS;
}

int32_t
cntlr_trnsprt_channel_init( 
      cntlr_channel_info_t  *channel_p,
      uint8_t                 trnsprt_type,
      int32_t                 trnsprt_fd,
      void                  *conn_spec_info   
      )
{
   uint8_t                    is_heap;
   uint32_t                   id;
   uint32_t                   thrd_assgn_2_chn;
   epoll_user_data_t          user_data;
   uint32_t                   intrested_events;
   int32_t                    ret_val = OF_SUCCESS;
   int32_t                    status = OF_SUCCESS;
   int8_t                    status_b;
   int32_t                     hash;


   do
   {
      CNTLR_XPRT_GENERATE_CHANNEL_ID(channel_p->id);
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_INFO,"creating channel=%d for fd %d",channel_p->id,trnsprt_fd);

      //memset(channel_p,0,sizeof(cntlr_channel_info_t));
      // channel_p->is_heap            = is_heap;
      channel_p->is_chn_added_to_dp = FALSE;
      channel_p->trnsprt_type       = trnsprt_type;
      channel_p->conn_spec_info     = conn_spec_info;   
      channel_p->trnsprt_fd         = trnsprt_fd;
      channel_p->auxiliary_id       = 0; 
      /* Auxiliary Id will be updated later once received response to 
         feature request message*/

      channel_p->state        = -1;
      fsm_update_channel_state(channel_p, OF_XPRT_PROTO_STATE_INIT);
      OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(channel_p);

      channel_p->keep_alive_state= OF_TRNSPRT_CHN_CONN_KEEP_ALIVE_NOT_READY; /*Atmaram */


      //OF_LIST_INIT(channel_p->send_msg_list,of_send_msg_free_rcu);
      OF_LIST_INIT(channel_p->send_msg_list,NULL);
      channel_p->epoll_wr_enabled = FALSE;

#ifndef OF_XPRT_PROTO_UDP_LOADBALANCE_SUPPORT
      if ( trnsprt_type == CNTLR_TRANSPORT_TYPE_UDP )
      {
         thrd_assgn_2_chn = CNTLR_TRANSPORT_UDP_THREAD_ID;
      }
      else
      {
         /* Channel afinity to least loaded transport layer thread*/
         thrd_assgn_2_chn =
            (id = (++cntlr_info.curr_thrd_2_which_ch_assnd) % cntlr_info.number_of_threads) ?
            id : cntlr_info.number_of_threads;
      }
#else
      /* Channel afinity to least loaded transport layer thread*/
      thrd_assgn_2_chn =
         (id = (++cntlr_info.curr_thrd_2_which_ch_assnd) % cntlr_info.number_of_threads) ?
         id : cntlr_info.number_of_threads;
#endif

      channel_p->thread  =  cntlr_get_thread_info(thrd_assgn_2_chn);
      if(CNTLR_UNLIKELY(!channel_p->thread))
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"Invalid Transport layer thread(%d) to assign Channel ",
               thrd_assgn_2_chn);
         status = OF_FAILURE;
         break;
      }

      /*Create message Q box to receive messages from Controller */
      channel_p->msg_q_box = of_MsgQBoxCreate(OF_MAX_MSGS_IN_CHN_Q_LIST,(cntlr_info.number_of_threads+1));
      if(channel_p->msg_q_box == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"Unable to create Message QBox for the channel_p ");
         status = OF_FAILURE;
         break;
      }


      /* Add Loopback FD to Poll table using API ‘epoll_add_fd’, 
       * This is to handle messages from controller applications running
       * in separate independent thread */
      memset(&user_data,0,sizeof(epoll_user_data_t));
      user_data.arg1 = (void *)channel_p;
      user_data.arg2 = (void *)channel_p->thread;
      intrested_events = ePOLL_POLLIN;
      ret_val = cntlr_epoll_add_fd_to_thread(channel_p->thread,
            channel_p->msg_q_box->iLoopbackSockFd,
            intrested_events,
            &user_data,
            cntlr_trnsprt_handle_ctrl_thread_msgs);
      if(ret_val == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"Error in adding Channel(%d) Socket to EPOLL",
               channel_p->auxiliary_id);
         status = OF_FAILURE;
         break;
      }

      /*Add Channel info to  hash table */

      ret_val = channel_p->trnsprt_info->proto_callbacks->transport_get_hash_value(channel_p->conn_spec_info,&hash);
      if ( ret_val != OF_SUCCESS )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Get hash Value failure");
         status=OF_FAILURE;
         break;
      }

      if(mempool_get_mem_block(cntlr_channel_safe_ref_memPool_g,
               (unsigned char **)&(channel_p->safe_ref),
               (unsigned char *)&is_heap) != OF_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR," Channel safe reference memory alloc failure");
         status = OF_FAILURE;
         break;
      }
      ret_val = channel_p->trnsprt_info->proto_callbacks->transport_get_hash_value(conn_spec_info,&hash);
      if ( ret_val != OF_SUCCESS )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Get hash Value failure");
         status=OF_FAILURE;
         break;
      }
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"hash value %d", hash);

      channel_p->safe_ref->is_heap=is_heap;
      MCHASH_APPEND_NODE(channel_p->trnsprt_info->connection_tbl,
            hash,
            channel_p,
            channel_p->safe_ref->index,
            channel_p->safe_ref->magic,
            ((uint8_t*)channel_p + CONNECTION_TBL_LINK_OFFSET),
            status_b);
      if(!status_b)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"Connection Node entry into hash table failed ");
         status = OF_FAILURE;
         break;
      }
      channel_p->is_chn_added_2_hash_tbl = TRUE;
#ifndef OF_XPRT_PROTO_UDP_LOADBALANCE_SUPPORT
      if ( trnsprt_type == CNTLR_TRANSPORT_TYPE_UDP )
      {
         channel_socket_recv_msgs_init(channel_p);
      }
#endif
      channel_p->state = OF_TRNSPRT_CHN_CONN_ESTBD;
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_INFO,"NEW CHANNEL id %d fd %d saferef index %d magic %d",
            channel_p->id, channel_p->trnsprt_fd, channel_p->safe_ref->index, channel_p->safe_ref->magic);
   }
   while(0);

   if(status == OF_FAILURE)
   {
      if(channel_p != NULL)
      {
         /* TBD - call state machine function here */
         cntlr_trnsprt_conn_close(channel_p);
         channel_p=NULL;
      }
      return OF_FAILURE;
   }

   return OF_SUCCESS;
}


/*Caller MUST use RCU_READ_LOCK and RCU_READ_UNLOCK*/
int32_t
of_add_channel_to_dp( cntlr_conn_table_t          *conn_table,
                      cntlr_conn_node_saferef_t   *conn_safe_ref,
                      struct ofp_switch_features  *sw_feature_msg,
                      struct dprm_datapath_entry  **datapath_p,
                      uint64_t                    *dp_handle_p)
{
   of_data_path_id_t               dp_id;
   uint64_t                        dp_handle = 0;
   dprm_dp_channel_entry_t        *dp_channel = NULL;
   struct dprm_datapath_entry     *datapath = NULL;
   uint8_t                         is_main_conn;
   int32_t                         retval = OF_SUCCESS;
   int32_t                         status = OF_SUCCESS;
  struct dprm_datapath_general_info config_info;
   struct dprm_datapath_runtime_info runtime_info;


   OF_NTOHLL_DP_ID(dp_id,sw_feature_msg->datapath_id);


   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      /** Get Datapath Handle of dp stored in the hsh table by giving datapath ID*/
      OF_LOG_MSG(OF_LOG_MOD,OF_LOG_DEBUG,"Datapath id is %llx",dp_id.val);	
      retval = dprm_get_datapath_handle(dp_id.val,&dp_handle);
      if(retval  != DPRM_SUCCESS)
      {
/* Atmaram - need to be removed ...its a temporary fix */
	  retval = dprm_get_first_datapath( &config_info, &runtime_info, &dp_handle);
	  if(retval != DPRM_SUCCESS){
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error : Invalid DP or no DP found")
             status = OF_FAILURE_DP_INVALID;
             break;
	   }
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error in getting datapath handle retval =%d ",
                                                   retval);
          status = OF_FAILURE;
          break;
      }
                                        
      /** Get Datapath information by giving datapath handle */
      retval = get_datapath_byhandle(dp_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error in getting datapath datails, retval %d",retval);
          status = OF_FAILURE;
          break;
      }

      /*Is current channel addition is for main connection or not */
      is_main_conn = (datapath->is_main_conn_present) ? FALSE:TRUE;

      if((sw_feature_msg->auxiliary_id != OF_TRNSPRT_MAIN_CONN_ID) && is_main_conn)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," Auxilliar channled ID %d  opend before 'main' creation",
                                      sw_feature_msg->auxiliary_id);
         status = OF_FAILURE;
         break;
      }

      /*Add channel to Data path channel list*/
      if(!is_main_conn)
      {
	  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"is a auxiliary connection");
          dp_channel = (dprm_dp_channel_entry_t*)of_mem_calloc(1,sizeof(dprm_dp_channel_entry_t));
          if(dp_channel == NULL)
          {
             OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Memory Allocation Failed - Datapath Channel ");
             status = OF_FAILURE;
             break;
          }

         /* Add channel to Aux connection list*/
         CNTLR_LOCK_TAKE(datapath->aux_ch_list_lock);
         if(datapath->aux_channels == NULL)
         {
            CNTLR_RCU_ASSIGN_POINTER(datapath->aux_channels,dp_channel);
            CNTLR_RCU_ASSIGN_POINTER(datapath->curr_chn_used_2_q_msg,dp_channel);
            CNTLR_RCU_ASSIGN_POINTER(dp_channel->prev,datapath->aux_channels);
         }
         else
         {
            CNTLR_RCU_ASSIGN_POINTER(datapath->aux_chn_tail->next,dp_channel);
            CNTLR_RCU_ASSIGN_POINTER(dp_channel->prev,datapath->aux_chn_tail);
         }
         CNTLR_RCU_ASSIGN_POINTER(datapath->aux_chn_tail,dp_channel);
         CNTLR_RCU_ASSIGN_POINTER(datapath->aux_chn_tail->next, datapath->aux_channels);
         datapath->curr_no_of_aux_chanels++;
         CNTLR_LOCK_RELEASE(datapath->aux_ch_list_lock);
 
         dp_channel->auxiliary_id    = sw_feature_msg->auxiliary_id; 
      }
      else
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"is a main connection")
          /* Initialize the table list in the data_path, basically allocates memory for
            * table entries*/
          retval = dprm_dp_table_init(sw_feature_msg->n_tables,&(datapath->tables) ); 
          if(retval == OF_FAILURE)
          {
              OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," datapath table  init failed ");
              status = OF_FAILURE;
              break;
          }

          /**Store SW feature details*/
          datapath->no_of_buffers        = ntohl(sw_feature_msg->n_buffers);
          datapath->no_of_tables         = sw_feature_msg->n_tables;
          datapath->capabilities         = ntohl(sw_feature_msg->capabilities);
          dp_channel                     = &datapath->main_channel;
          dp_channel->auxiliary_id       = OF_TRNSPRT_MAIN_CONN_ID; 
          datapath->is_main_conn_present = TRUE; 
      }

      /**Remeber Connection table information to which the connection pushed*/
      dp_channel->conn_table            = conn_table; 

      /**Remeber dp handle in the channel */
      dp_channel->dp_handle          = dp_handle; 
      dp_channel->datapath           = datapath; 

      /** Remember channle connection reference*/
      dp_channel->conn_safe_ref.index = conn_safe_ref->index;
      dp_channel->conn_safe_ref.magic = conn_safe_ref->magic;

     OF_LOG_MSG(OF_LOG_XPRT_PROTO,OF_LOG_DEBUG,"dp channel pointer %p dp_handle is %d, safe ref  index %d , magic num %d",dp_channel,dp_channel->dp_handle,dp_channel->conn_safe_ref.index,dp_channel->conn_safe_ref.magic);	

      /*TBD, Need to start timer  to monitor inactivity on the channel*/
      /* also need to set connection state*/
  }
  while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  if(status != OF_SUCCESS)
  {
       if((!is_main_conn) &&(dp_channel != NULL))
          of_mem_free(dp_channel);
#if 0
       if ((datapath) && datapath->tables != NULL))
          of_dp_table_de_init(datapath->tables);
#endif
       return status;
  }

  *dp_handle_p = dp_handle;
  *datapath_p = datapath;
  return OF_SUCCESS;
}


   static inline void
cntlr_trnsprt_conn_free_rcu(struct rcu_head *node)
{     
   cntlr_channel_info_t *channel = NULL;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"freeing transport channel");
   channel = (cntlr_channel_info_t *)((uint8_t*)node  - RCUNODE_OFFSET_IN_MCHASH_TBLLNK - CONNECTION_TBL_LINK_OFFSET );
   cntlr_trnsprt_conn_close(channel);
}

void cntlr_trnsprt_conn_close(cntlr_channel_info_t   *channel)
{
   int32_t                    ret_val = OF_SUCCESS;
   int32_t                    status = OF_SUCCESS;
   struct of_msg               *msg        = NULL;
#if 0
   int i;
#endif
   cntlr_send_msg_buf_t       *msg_buf;
   uchar8_t offset;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "entered");
   do
   {
      if(CNTLR_UNLIKELY(channel == NULL))
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR," nvalid input");
         status = OF_FAILURE;
         break;
      }

      if(channel->msg_q_box)
      {
         CNTLR_RCU_READ_LOCK_TAKE();
         channel->msg_q_box->bSendEvent = FALSE;
#if 0
         for ( i=0; i< CNTLR_NO_OF_THREADS_IN_SYSTEM; i++)
         {
            while(!of_MsgDequeue(channel->msg_q_box,i,(void**)&msg,TRUE))
            {
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"deleting message");
               if (msg)
               {
			of_check_pkt_mbuf_and_free(msg);
               }
		msg=NULL;
            }
         }
#endif
         CNTLR_RCU_READ_LOCK_RELEASE();
         of_MsgQBoxDelete(channel->msg_q_box);
         channel->msg_q_box = NULL;
      }

      offset = CNTLR_TRNSPRT_MSG_LISTNODE_OFFSET;
      if ( OF_LIST_COUNT(channel->send_msg_list) > 0)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"pending messages exists in channel");
         OF_LIST_REMOVE_HEAD_AND_SCAN(channel->send_msg_list, msg_buf,cntlr_send_msg_buf_t*,offset)
         {
            if (msg_buf->msg)
            {
              	 of_check_pkt_mbuf_and_free(msg_buf->msg);
            }
         }
      }

      if ( channel->msg_buf!=NULL)
      {
         of_check_pkt_mbuf_and_free(channel->msg_buf);
      }

      if(channel->safe_ref)
         mempool_release_mem_block (cntlr_channel_safe_ref_memPool_g,(unsigned char*)channel->safe_ref,channel->safe_ref->is_heap);

      channel->state = OF_TRNSPRT_CHN_CONN_CLOSED;
      mempool_release_mem_block (cntlr_trnsprt_channel_mempool_g,(unsigned char*)channel,channel->is_heap);
   }while(0);

   return ;
}

int32_t cntlr_trnsprt_mchash_table_init(cntlr_trnsprt_info_t *trnsprt_info)
{
   int32_t ret_val=OF_SUCCESS;
   int32_t status=OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "entered");
   do
   {
      if(trnsprt_info == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Error in creating connection hash table");
         status = OF_FAILURE;
         break;
      }

      cntlr_conn_hash_table_init_value_g = rand();
      ret_val = mchash_table_create(OF_CONN_TABLE_SIZE,
            OF_CONN_TABLE_MAX_NO_OF_NODES,
            cntlr_trnsprt_conn_free_rcu,
            &trnsprt_info->connection_tbl);
      if(ret_val != MCHASHTBL_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Error in creating connection hash table");
         status = OF_FAILURE;
         break;
      }

   }while(0);

   return status;

}

   void
dprm_free_datapath_channels(struct dprm_datapath_entry  *datapath_p)
{
   cntlr_channel_info_t        *channel;
   uint8_t                     is_ch_access_status = FALSE;
   dprm_dp_channel_entry_t        *dp_channel = NULL;
   epoll_user_data_t          user_data;
   uint32_t                   intrested_events;
   int32_t                    ret_val = OF_SUCCESS;
   int32_t                    status = OF_SUCCESS;


   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "entered");
   do
   {
      if(CNTLR_UNLIKELY(datapath_p == NULL))
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "input is null");
         break;
      }

      if (datapath_p->is_main_conn_present)
      {
         dp_channel                     = &datapath_p->main_channel;

         MCHASH_ACCESS_NODE(dp_channel->conn_table,dp_channel->conn_safe_ref.index, dp_channel->conn_safe_ref.magic,channel,is_ch_access_status);
         if( CNTLR_UNLIKELY(is_ch_access_status == FALSE) )
         {
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"channel fd %d saferef index %d magic %d", channel->trnsprt_fd, dp_channel->conn_safe_ref.index, dp_channel->conn_safe_ref.magic);
            break;
         }
         fsm_send_channel_event(channel, OF_XPRT_PROTO_CHANNEL_DP_DETACH_EVENT);
#if 0
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

         if ( channel->trnsprt_fd > 0 )
         {
            cntlr_epoll_del_fd_from_thread(channel->thread,channel->trnsprt_fd);
            //cntlr_epoll_del_fd(channel->trnsprt_fd);

            intrested_events  = ePOLL_POLLIN;
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
         //if(channel->is_chn_added_to_dp) /* must be true -Atmaram*/
         of_remove_channel_from_dp(dp_channel->dp_handle, dp_channel->auxiliary_id);
         channel->state = OF_TRNSPRT_CHN_CONN_ESTBD;
         channel->datapath=NULL;
         channel->dp_handle=0;
         channel->is_chn_added_to_dp =FALSE;
#endif
         /* NOW... clean auxilery channels . :TBD*/
      }
      else
      {

         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "main connecton is not present.. no channel cleanup is required");
      }


   }while(0);

   return;
}

