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

/* File  : cntlr_trnsprt.h
 * Author: Rajesh Madabushi <rajesh.madabushi@freescale.com>
 * Date  : Feb, 2012  
 * Description: 
 *  This file contains transport layer data_structure and other related definitions.
 * History:
 */

#ifndef __CNTLR_TRNSPRT_H
#define __CNTLR_TRNSPRT_H

#include "cntrl_queue.h"
#include "ossl_typ.h"

/* * * * * * * * * * * * * * G l o b a l * * * * * * * * * * * * * * * * * * */
/******************************************************************************
 * * * * * * * * * * * M A C R O   D E F I N I T I O N S * * * * * * * * * * * * 
 *******************************************************************************/

#define OF_CONN_TABLE_MAX_NO_OF_NODES ((OF_MAX_NO_OF_CTRLR_INSTANCES    * \
			OF_MAX_NO_OF_DPs_EACH_CTRLR_INST  * \
			OF_MAX_NO_OF_CHANNELS_WITHIN_DP) + 50 )

#define OF_MAX_NUM_OF_OF_MSGS_IN_RING  (512)

#define OF_MAX_MSGS_IN_CHN_Q_RING  64
#define OF_MAX_MSGS_IN_CHN_Q_LIST  64


#define OF_TRNSPRT_MAX_MESSAGES   OF_MAX_NUM_OF_OF_MSGS_IN_RING + OF_MAX_NUM_OF_MSGS_IN_LIST
#define OF_TRNSPRT_STATIC_MESSAGES (OF_TRNSPRT_MAX_MESSAGES *4)/5 /* 80 % */

#define OF_TRNSPRT_MAX_CONN_ENTRIES        (OF_CONN_TABLE_MAX_NO_OF_NODES)
#define OF_TRNSPRT_STATIC_CONN_ENTRIES     (OF_TRNSPRT_MAX_CONN_ENTRIES * 4)/5 /* 80 % */


/* Transport layer thread commands*/
#define OF_TRNSPRT_MSG_RCVD     1
#define OF_APP_MSG_RCVD 2
#define OF_RTL_MSG_RCVD 3
#define OF_PETH_MSG_RCVD 4

/*Keep alive timer*/
#define OF_TRNSPRT_KEEP_ALIVE_TIME_PERIOD  60000000

#define OF_TRNSPRT_KEEP_ALIVE_RESP_WAIT_TIME  5000000


#if 0
#define CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,datapath,conn_table,conn_safe_ref,callback_fn,clbk_arg1,clbk_arg2,status)\
{\
	/*struct barrier_state_msg_queue_node *msg_node_4_barrier_q = NULL;\*/\
	/*In case DP is barrier state enqueue the msg to barrier wait queue*/\
	if(datapath->barrier_state != DP_BARRIER_STATE)\
	{\
		/* Set DP in barrier state mode in case of sending barreir message so that   
		   subsequent message will be enqueued locally untill barrier response is received*/\
		/* if(((struct ofp_header*)(msg->desc.start_of_data))->type == OFPT_BARRIER_REQUEST)\
		   {\
		   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"sending barrier request updating state to DP_BARRIER_STATE");\
		   datapath->barrier_state = DP_BARRIER_STATE;\
		   }*/ \
		if(cntlr_send_msg_to_dp(msg,datapath,conn_table,&conn_safe_ref,callback_fn,clbk_arg1,clbk_arg2) != OF_SUCCESS)\
			status = FALSE;\
	}\
	else\
	{\
		struct barrier_state_msg_queue_node* msg_node_4_barrier_q=NULL;\
			msg_node_4_barrier_q = (struct barrier_state_msg_queue_node*)of_mem_calloc(1, sizeof(struct barrier_state_msg_queue_node));\
			if(msg_node_4_barrier_q == NULL)\
			{\
				if(msg != NULL)\
					msg->desc.free_cbk(msg);\
						status = FALSE;\
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error in allocating memory 4 node 2 append barrier message queue");\
			}\
		msg_node_4_barrier_q->msg = msg;\
			msg_node_4_barrier_q->datapath = datapath;\
			msg_node_4_barrier_q->conn_table = conn_table;\
			/*      msg_node_4_barrier_q->conn_safe_ref = conn_safe_ref;*/\
			memcpy(&msg_node_4_barrier_q->conn_safe_ref,&conn_safe_ref, sizeof(cntlr_conn_node_saferef_t));\
			msg_node_4_barrier_q->cbk_fn = callback_fn;\
			msg_node_4_barrier_q->cbk_arg1 = clbk_arg1;\
			msg_node_4_barrier_q->cbk_arg2 = clbk_arg2;\
			OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"enquing to barrier queue");\
			OF_APPEND_NODE_TO_LIST(datapath->msg_queue_in_barrier_state,msg_node_4_barrier_q);\
			OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"no of msgs %d",OF_LIST_COUNT(datapath->msg_queue_in_barrier_state));\
	}\
}
#endif

typedef enum {
	CNTLR_TRANSPORT_TYPE_TLS  = DPRM_TRANSPORT_TYPE_TLS,
	CNTLR_TRANSPORT_TYPE_TCP  = DPRM_TRANSPORT_TYPE_TCP,
	CNTLR_TRANSPORT_TYPE_DTLS = DPRM_TRANSPORT_TYPE_DTLS,
	CNTLR_TRANSPORT_TYPE_UDP  = DPRM_TRANSPORT_TYPE_UDP,
	CNTLR_TRANSPORT_TYPE_VIRTIO  = DPRM_TRANSPORT_TYPE_VIRTIO,
	CNTLR_TRANSPORT_TYPE_UNIX_DM_TCP  = DPRM_TRANSPORT_TYPE_UNIX_DM_TCP,
	CNTLR_TRANSPORT_TYPE_UNIX_DM_UDP  = DPRM_TRANSPORT_TYPE_UNIX_DM_UDP,
	CNTLR_TRANSPORT_TYPE_MAX  = DPRM_TRANSPORT_TYPE_UNIX_DM_UDP+1
} of_transport_type_e;

/*Default Openflow  TRANSPORT Connection Type */
#define   OF_CONN_DFLT_TRANSPORT_TYPE (CNTLR_TRANSPORT_TYPE_TCP)

/*Default Openflow Port*/
#define   OF_CONN_DFLT_PORT  (6653)

/* TLS/SSL Port */
#define   OF_CONN_TLS_PORT   (6654)

/*Channel data read status*/
#define CNTLR_CONN_READ_SUCCESS 0 /*Read data successfully*/
#define CNTLR_CONN_READ_ERROR  -1 /* error in reading data*/
#define CNTLR_CONN_NO_DATA     1 /* No data exist to read on socket*/

#define CNTLR_TCP_KEEP_ALIVE_IDLE_TIME       180; /*seconds*/
#define CNTLR_TCP_KEEP_ALIVE_PROBE_INTERVAL  60; /*Seconds*/
#define CNTLR_TCP_KEEP_ALIVE_PROBE_CNT       5


/******************************************************************************
 * * * * * * * * * * * S T R U C T U R E   D E F I N I T I O N S * * * * * * * * * 
 *******************************************************************************/

typedef struct mchash_noderef  cntlr_conn_node_saferef_t;

typedef struct mchash_table    cntlr_conn_table_t;

/** transports on which controller listening */
typedef struct cntlr_trnsprt_info_s
{
	int32_t id;
	of_list_node_t           next_node; /** Next node of transport opend*/
	struct rcu_head          rcu_head; /** Used in RCU operation of transport list modications*/
	int32_t                  fd; /** File descriptor of the channel opend*/
	of_transport_type_e      transport_type; /** Transport type TCP/TLS/UDP/DTLS*/
	uint16_t                 proto; /** Port number of transport listening*/
	uint8_t                 fd_type;
	uint8_t                  is_enabled; /* is this transport record enabled*/
	int8_t		    fsm_state; /** proto state*/
	int8_t		    fsm_event;
	cntlr_conn_table_t      *connection_tbl; /** connections opened by data_paths for this transport*/
	void *proto_spec;
	struct of_transprt_proto_callbacks *proto_callbacks;
} cntlr_trnsprt_info_t;

#define CNTLR_TRNSPRT_LISTNODE_OFFSET offsetof(cntlr_trnsprt_info_t, next_node)

typedef struct cntlr_ch_safe_ref_s
{
	uint32_t  index;            
	uint32_t  magic; 
	uint8_t   is_heap;
} cntlr_ch_safe_ref_t;

typedef struct cntlr_send_msg_buf_s
{
	of_list_node_t     next_msg;
	struct rcu_head     rcu_head;
	uint16_t            length_sent;
	struct of_msg      *msg;
} cntlr_send_msg_buf_t;

#define CNTLR_TRNSPRT_MSG_LISTNODE_OFFSET offsetof(cntlr_send_msg_buf_t, next_msg)
/*Details of channels accepted from data_path*/

typedef struct of_conn_properties_s
{
	uint8_t                 trnsprt_type;
	int32_t                 trnsprt_fd;
	void *conn_spec_info;
}of_conn_properties_t;

typedef struct  cntlr_channel_info_s
{
	struct mchash_table_link     next_conn_link; /* link to next connection node details*/
	cntlr_trnsprt_info_t        *trnsprt_info; /** transport details, from which this channels  are created*/ 
	int32_t id; 
	uint32_t  		         auxiliary_id; /** Auxiliary ID of the channel opened from data_path*/ 
	cntlr_thread_info_t         *thread; /* Thread afined to handle msgs received on channel*/
	of_msg_q_box_t              *msg_q_box; /* Open Flow message queue used by Ctrl interface 2 enqueue msgs*/ 
	of_msg_q_box_t		*dp_recv_msg_q_box;
	uint8_t                      trnsprt_type; /** Transport type TCP/TLS/UDP/DTLS*/
	uint16_t		         local_port; /* Local Port on which controler accept the connection*/
	int32_t                      trnsprt_fd; /** File descriptor of the connection */
	uint8_t                      keep_alive_state; /*Indicate the state of keep alive message state by controller*/
	struct tmr_saferef           keepalive_timer; /*Timer used to send keep alive messages to dp*/
	struct tmr_saferef           keepalive_resptimer; /*Timer used to check keep alive response */
	int8_t		         state; /** Connection state, state of te chanel,it is open,handshake,ready,etc*/
	int8_t		         fsm_state; /** Connection state, with respect to state machine*/
	int8_t		         fsm_event; /** Connection state, with respect to state machine*/
	int8_t                       msg_state; /*Current state of the message,it can be waiting for msg,waiting of header,
						  waiting for actual msg,etc*/   
	struct ofp_header            msg_hdr; /*Message header of current message read on the channel*/
	uint8_t                      hdr_len_read; /*Lenght of current message hader read so far*/
	uint8_t                      msg_len_read; /*Lenght of current message read so far*/
	struct of_msg               *msg_buf; /*Buffer used to read current message*/
	uint8_t                      epoll_wr_enabled;
	of_list_t                    send_msg_list; /* Messages waiting on the channels to send*/
//	cntlr_ssl_info_t 	        *pssl_info; /** SSL details*/
	/*TBD TmrSafeRef_t              timer_handle;*/ /** Timer handle of the channel*/
	uint32_t		         time_out_val;
	uint8_t                      is_dp_ready;
	uint8_t                      is_chn_added_to_dp;
	uint8_t                      is_chn_added_2_hash_tbl;
	cntlr_ch_safe_ref_t         *safe_ref; /** Safe reference of channel stored in the hash table*/ 
	uint64_t                     dp_handle; /** Datapath handle to which this channel attached*/
	struct dprm_datapath_entry   *datapath;/* Data path to which this channel is assigned*/
	uint8_t                      is_heap;
	uint64_t                     recv_bytes; /** Number of bytes received*/
	uint64_t                     tx_bytes; /** Number of bytes transmitted*/
	uint32_t                     drain_out_bytes; /** Number of bytes to be drained from message*/
	uint64_t                     ssl_match_dp_id;
        uint32_t                     uiKeepAliveCount;
	void *conn_spec_info;
}  cntlr_channel_info_t;

struct of_cntlr_conn_info{
	cntlr_conn_node_saferef_t    conn_safe_ref;
	cntlr_conn_table_t          *conn_table;
};

#ifdef OF_XML_SUPPORT

struct  transport_runtime_info {
    int32_t transport_starting_core_id;
    int32_t transport_noof_cores;
    int32_t number_of_transport_threads;
    int32_t peth_starting_core_id;
    int32_t peth_number_of_cores;
    int32_t number_of_peth_threads;
};

#endif


#define CONNECTION_TBL_LINK_OFFSET offsetof(cntlr_channel_info_t,next_conn_link)


/******************************************************************************
 * * * * * * * * * F U N C T I O N   P R O T O T Y P E S * * * * * * * * * * * *  
 *******************************************************************************/




/* * * * * * * * * * * * * * L o c a l  * * * * * * * * * * * * * * * * * * */
/******************************************************************************
* * * * * * * * * * S T R U C T U R E   D E F I N I T I O N S * * * * * * * * * 
*******************************************************************************/
typedef struct cntlr_ssl_info_s
{
  SSL     *ssl;
  SSL_CTX *ctx;
} cntlr_ssl_info_t;

/******************************************************************************
* * * * * * * * * * * M A C R O   D E F I N I T I O N S * * * * * * * * * * * *  
*******************************************************************************/
#define OF_CONN_TABLE_SIZE (OF_CONN_TABLE_MAX_NO_OF_NODES / 6)

#define OF_CONN_TABLE_HASH(param1,param2,param3,hash)   \
{\
  OF_DOBBS_WORDS3 ((uint32_t)param1,(uint32_t)param2,(uint32_t)param3,   \
                       cntlr_conn_hash_table_init_value_g, \
                       OF_CONN_TABLE_SIZE, hash);\
}

#define OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(channel)\
{\
    channel->msg_buf       = NULL;\
    channel->msg_state     = OF_CHN_WAITING_FOR_NEW_MSG;\
    channel->hdr_len_read  = 0;\
};

/* In case barrier state is enabled, means barrier message has been sent,
 * packet will be queued to barrier qeueu, otherwise the message wille be
 * send to data_path*/
#define CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,datapath,conn_table,conn_safe_ref,callback_fn,clbk_arg1,clbk_arg2,status)\
{\
   status = TRUE;\
   if(cntlr_send_msg_to_dp(msg,datapath,conn_table,&conn_safe_ref,callback_fn,clbk_arg1,clbk_arg2) != OF_SUCCESS)\
     status = FALSE;\
}


#define OF_TRNSPRT_MAIN_CONN_ID     (0)

#define CNTLR_TCP_ACCEPT_Q_LEN  (DPRM_MAX_DATAPATH_ENTRIES *\
                OF_MAX_NO_OF_CHANNELS_WITHIN_DP) 

/* * * * * * * * * * Keep Alive states* * * * * * * * * */
#define OF_TRNSPRT_CHN_CONN_KEEP_ALIVE_NOT_READY  (0)
#define OF_TRNSPRT_CHN_CONN_KEEP_ALIVE_MSG_READ_TO_SEND  (1)
#define OF_TRNSPRT_CHN_CONN_KEEP_ALIVE_MSG_SENT        (2)
#define OF_TRNSPRT_CHN_CONN_KEEP_ALIVE_MSG_RESP_RCVD   (4)

/* * * * * * * * * * Channel Alive states* * * * * * * * * */
#define OF_TRNSPRT_CHN_CONN_INPROGRESS                    (1)
#define OF_TRNSPRT_CHN_CONN_REQ_HANDSHAKE_WITH_DP         (2)
#define OF_TRNSPRT_CHN_CONN_ESTBD                         (3)
#define OF_TRNSPRT_CHN_HANDSHAKE_WITH_DP_DONE             (4)
#define OF_TRNSPRT_CHN_CLOSE_INITIATED                    (5)
#define OF_TRNSPRT_CHN_CONN_TIMEDOUT                      (6)
#define OF_TRNSPRT_CHN_CONN_CLOSED                        (7)
#define OF_TRNSPRT_CHN_SENT_HELLO_TO_DP                   (8)
#if 1 /* atmaram -NEED TO VERIFY */
/*Error Macros */
#define OF_FAILURE_DP_INVALID   (-11)
#define OF_FAILURE_CHN_DEL     (-22)
#endif

/* * * * * * * * * Message states, state of message reading on the channel* * * * * */
#define OF_CHN_WAITING_FOR_NEW_MSG          (1)
#define OF_CHN_WAITING_FOR_MSG_HEADER       (2)
#define OF_CHN_START_READING_MSG            (3)
#define OF_CHN_WAITING_FOR_COMPLETE_MSG     (4)
#define OF_CHN_RCVD_COMPLETE_MSG            (5)
#define OF_CHN_DRAIN_OUT_COMPLETE_MSG       (6)


/*TBD Currently channels are assigned on round robin fashion, need more effecient mechanism*/
/* Caller must use CNTLR_RCU_READ_LOCK_TAKE()() and CNTLR_RCU_READ_LOCK_RELEASE()*/
#define CNTLR_GET_LEAST_LOADED_CHANNEL_SAFE_REF(pDPIface,SafeRef)\
{\
   cntlr_dp_channelInfo_t  *dp_channel;\
   cntlr_assert(data_path->curr_chn_used_2_q_msg);\
   CNTLR_LOCK_TAKE(data_path->aux_ch_list_lock);\
   CNTLR_RCU_DE_REFERENCE(data_path->curr_chn_used_2_q_msg,dp_channel);\
   safe_ref.uiIndex = dp_channel->conn_safe_ref.uiIndex;\
   safe_ref.uiMagic = dp_channel->conn_safe_ref.uiMagic;\
   CNTLR_RCU_ASSIGN_POINTER(data_path->curr_chn_used_2_q_msg,data_path->curr_chn_used_2_q_msg->next);\
   CNTLR_LOCK_RELEASE(data_path->aux_ch_list_lock);\
}
/* atmaram -move this channel related macros to transport local include */
/*Main channel is static memory so RCU operation is not required*/
#define CNTLR_GET_MAIN_CHANNEL_INFO(data_path,conn_info)\
{\
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"is _ main conn present %d",data_path->is_main_conn_present);\
        if ((data_path != NULL) && (data_path->is_main_conn_present == TRUE) ) \
        { \
                conn_info=(struct of_cntlr_conn_info *)calloc(1,sizeof(struct of_cntlr_conn_info));\
		if ( conn_info)\
		{\
                conn_info->conn_safe_ref.index = data_path->main_channel.conn_safe_ref.index;\
                conn_info->conn_safe_ref.magic = data_path->main_channel.conn_safe_ref.magic;\
                conn_info->conn_table = data_path->main_channel.conn_table;\
		}\
		else \
		{\
        		conn_info = NULL;\
		}\
        }\
        else\
        conn_info = NULL;\
}

#define CNTLR_GET_MAIN_CHANNEL_SAFE_REF(data_path,saferef,conntable)\
{\
   saferef.index = data_path->main_channel.conn_safe_ref.index;\
   saferef.magic = data_path->main_channel.conn_safe_ref.magic;\
   conntable     = data_path->main_channel.conn_table;\
}

#define CNTLR_GET_AUX_CHANNEL_INFO(data_path, conn_info)\
{\
	if(datapath->aux_channels == NULL)\
	{\
		OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"no auxilary channels");\
	}\
	else\
	{\
		conn_info=(struct of_cntlr_conn_info *)calloc(1,sizeof(struct of_cntlr_conn_info));\
		if (conn_info) \
		{\
		conn_info->conn_safe_ref.index = data_path->curr_chn_used_2_q_msg->conn_safe_ref.index;\
		conn_info->conn_safe_ref.magic = data_path->curr_chn_used_2_q_msg->conn_safe_ref.magic;\
		conn_info->conn_table = data_path->curr_chn_used_2_q_msg->conn_table;\
		}\
		else \
		{\
        		conn_info = NULL;\
		}\
	}\
}
#define CNTLR_GET_AUX_CHANNEL_IF_EXISTS_ELSE_GET_MAIN_CHANNEL(data_path, conn_info)\
{\
	if(datapath->aux_channels == NULL)\
	{\
		 CNTLR_GET_MAIN_CHANNEL_INFO(data_path,conn_info);\
	}\
	else\
	{\
		 CNTLR_GET_AUX_CHANNEL_INFO(data_path,conn_info);\
	}\
}
/*TBD of getting least loaded channel, following is just temporary def*/
//#define CNTLR_GET_LEAST_LOADED_CHANNEL_SAFE_REF CNTLR_GET_MAIN_CHANNEL_SAFE_REF

/* Macro to get channel get safe reference of given channel ID
 * call need to apply RCU LOCK and RCU_READ_UNLOCK*
 */
#define CNTLR_GET_GIVEN_CHANNEL_INFO(data_path,channel_no,conn_info)\
{\
	if(datapath->main_channel.auxiliary_id == channel_no)\
	{\
		conn_info=(struct of_cntlr_conn_info *)calloc(1,sizeof(struct of_cntlr_conn_info));\
		if (conn_info) \
		{\
			conn_info->conn_safe_ref.index = data_path->main_channel.conn_safe_ref.index;\
			conn_info->conn_safe_ref.magic = data_path->main_channel.conn_safe_ref.magic;\
			conn_info->conn_table = data_path->main_channel.conn_table;\
		}\
		else \
		{\
			conn_info = NULL;\
		}\
	}\
}

#define CNTLR_GET_GIVEN_CHANNEL_SAFE_REF(data_path,channel_no,saferef,conntable)\
{\
   /*TBD of search of Multiiple channels, only main channe search hepening*/\
   if(datapath->main_channel.auxiliary_id == channel_no)\
   {\
     saferef.index = data_path->main_channel.conn_safe_ref.index;\
     saferef.magic = data_path->main_channel.conn_safe_ref.magic;\
     conntable     = data_path->main_channel.conn_table;\
   }\
}


/******************************************************************************
* * * * * * * * * * * F U N C T I O N   M A C R O S * * * * * * * * * * * * * * *
*******************************************************************************/
#define cntlr_read_msg_recvd_from_dp( channel, buffer,  length, len_read) \
 fsm_xprt_proto_channel_read_msg_from_dp(channel,buffer, length,len_read)

#define of_send_msg_to_xprt(channel,msg, len_already_sent,len_sent_currently)\
  fsm_xprt_proto_channel_write_msg_to_dp(channel,msg, len_already_sent,len_sent_currently)


/******************************************************************************
* * * * * * * * * F U N C T I O N   P R O T O T Y P E S * * * * * * * * * * * 
*******************************************************************************/

#endif /*_CNTLR_TRNSPRT_H*/
