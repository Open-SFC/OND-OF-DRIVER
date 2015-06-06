/**
*  Copyright (c) 2012, 2013  Freescale.
*   
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at:
* 
*   http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
**/
/*
* File:        of_cntlr_msg.c
* 
* Description: 
* This file contains controller managing Open flow messages.
* That is Controller working as an application calls north bound
* messaging API to send and receive messages.
* Provides infrastruture to read , parse and process messessages
* received from datapath.
* This file  also provides open flow message memory management utilities.
* Basically to allocate and free message buffere that used  to store and manage 
* Open Flow  messages. It inturn uses message pool APIs defined in the of_msgpool.c
* to allocate and release message buffer
*
* Authors:     Rajesh Madabushi <rajesh.madabushi@freescale.com>
 */

/*INCLUDE_START  ********************************************************/
#include "controller.h"
#include "of_utilities.h"
#include "of_msgapi.h"
#include "of_multipart.h"
#include "of_msgpool.h"
#include "cntrl_queue.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "cntlr_xtn.h"
#include "cntlr_event.h"
#include "dprmldef.h"
#include "dll.h"
#include "pktmbuf.h"
#include "fsl_ext.h"
/*INCLUDE_END  **********************************************************/

/*DEFINE_START **********************************************************/
//#define of_debug printf

#define OF_MULTIPART_RPLY_MSG_NO_OF_DESC(multipart_msg_hdr,typecast) \
  ( (ntohs(multipart_msg_hdr->length) - sizeof(struct ofp_multipart_reply) ) / sizeof(typecast) )

/*Vinod*/
#define FSLX_MULTIPART_RPLY_MSG_NO_OF_DESC(multipart_msg_hdr,typecast) \
  ( (ntohs(multipart_msg_hdr->length) - sizeof(struct fslx_multipart_reply) ) / sizeof(typecast) )




#define OF_INST_PROP_NO_OF_DESC(length) \
  ( (length - sizeof(struct ofp_table_feature_prop_instructions))/sizeof(struct ofp_instruction) )

#define OF_NEXT_TBL_PROP_NO_OF_DESC(length) \
  ( (length - sizeof(struct ofp_table_feature_prop_next_tables))/sizeof(uint8_t) )

#define OF_ACTN_PROP_NO_OF_DESC(length) \
  ( (length - sizeof(struct ofp_table_feature_prop_actions))/(sizeof(struct ofp_action_header) - 4) )//minus "4" for pad[4] in ofp_action_header

#define OF_OXM_IDS_NO_OF_DESC(length) \
  ((length > sizeof(struct ofp_table_feature_prop_oxm)) ? ( (length - sizeof(struct ofp_table_feature_prop_oxm))/sizeof(uint32_t) ): 0) 

#define OF_READ_ASYNC_MSG(channel,msg_id,msg_hdr)  \
  if(p_channel->msg_state != OF_CHN_RCVD_COMPLETE_MSG)\
{\
  if(channel->auxiliary_id == OF_TRNSPRT_MAIN_CONN_ID)\
    channel->uiKeepAliveCount = 0; \
  if(of_alloc_and_read_async_message(channel,channel->datapath->domain,\
        &channel->datapath->domain->msg_type_mem_primitive[msg_id],\
        msg_hdr) == OF_FAILURE)\
  {\
    status = OF_FAILURE;\
    break;\
  }\
  if(p_channel->msg_state == OF_CHN_WAITING_FOR_COMPLETE_MSG)\
  break;\
}

#define MATCHFILED_PADDING(length, lengthwithpadding)\
  if( (length %8) == 0)\
lengthwithpadding = length;\
else if(length > 8) \
lengthwithpadding = ((8-(length %8))+length);\
else \
lengthwithpadding = 8;

#define OF_OXM_HDR_LEN   (4)
#define OF_TABLE_FEATURE_PROPERTY_PADDING_LEN(len) (((len) + 7)/8*8 - (len))
#define OF_MATCH_FIELD_PADDING_LEN(len) (((len) + 7)/8*8 - (len))

/*DEFINE_END   **********************************************************/

/* TYPEDEF_START  *******************************************************/
typedef struct ofp_port  of_port_t;
typedef void (*cntlr_msg_cbk_fn)(cntlr_channel_info_t       *channel,
    struct dprm_datapath_entry *datapath,
    struct ofp_header          *msg_hdr);

/* TYPEDEF_END    *******************************************************/

/* PROTOTYPE_START   ****************************************************/
extern   inline int32_t cntlr_send_msg_to_dp(struct of_msg               *msg,
    struct dprm_datapath_entry* datapath,
    cntlr_conn_table_t         *conn_table,
    cntlr_conn_node_saferef_t  *conn_safe_ref,
    void                       *callback_fn,
    void                       *clbk_arg1,
    void                       *clbk_arg2);


int32_t of_flush_msg_buf(cntlr_channel_info_t     *channel, uint16_t length);

int32_t of_flow_stats_frame_respone(char *msg, struct ofi_flow_entry_info  *flow_stats_p,
    uint32_t remaining_length);
int32_t of_frame_instruction_response(char *msg, struct ofi_instruction **instruction_entry_pp, 
    uint32_t *length);
extern void inform_event_to_app(struct dprm_datapath_entry *data_path,
    uint8_t                     event_type,
    void                       *event_info1,
    void                       *event_info2);

extern int32_t
handover_experimenter_event_to_app(uint64_t datapath_handle,
    struct dprm_domain_entry *domain,
    uint8_t                   table_id,
    uint8_t                   msg_id,
    struct of_msg            *msg,
    struct ofl_flow_removed  *flow_removed_msg);

extern int32_t
handover_flow_removed_event_to_app(uint64_t datapath_handle,
    struct dprm_domain_entry *domain,
    uint8_t                   table_id,
    uint8_t                   msg_id,
    struct of_msg            *msg,
    struct ofl_flow_removed  *flow_removed_msg);
extern int32_t
handover_pkt_in_msg_to_app(uint64_t datapath_handle,
    struct dprm_domain_entry *domain,
    uint8_t                   table_id,
    uint8_t                   msg_id,
    struct of_msg            *msg,
    struct ofl_packet_in     *pkt_in);
extern int32_t
handover_port_status_msg_to_app(uint64_t datapath_handle,
    struct dprm_domain_entry *domain,
    uint8_t                   table_id,
    uint8_t                   msg_id,
    struct of_msg            *msg,
    struct ofl_port_status   *port_status_msg);
int32_t
of_alloc_and_read_async_message(cntlr_channel_info_t     *channel,
    struct dprm_domain_entry  *p_dp_domain,
    struct of_async_msg_mem_primitive  *msg_type_mem_primitive,
    struct ofp_header        *msg_hdr_p);
int32_t
of_alloc_and_read_message_from_dp(cntlr_channel_info_t  *channel,
    struct ofp_header    *msg_hdr_p);

int32_t
cntlr_read_msg_recvd_from_dp(cntlr_channel_info_t  *channel,
    uint8_t               *buffer,
    uint16_t               length,
    uint16_t               *len_read);

void
of_msg_not_supported(cntlr_channel_info_t       *channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr);
void
of_echo_req_handler(cntlr_channel_info_t       *channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr);
void
of_echo_reply_handler(cntlr_channel_info_t       *channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr);

void of_handle_experimenter_event(cntlr_channel_info_t       *p_channel,
                                  struct dprm_datapath_entry *p_datapath,
                                  struct ofp_header          *p_msg_hdr);

void of_cfg_reply_handler(cntlr_channel_info_t       *channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr );
void
of_process_multipart_replymsg(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr);

void
of_process_barrier_replymsg(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr);

void
of_process_role_replymsg(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr);

void
of_process_async_replymsg(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr);



void
of_handle_pkt_in_msg(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr);

void
of_handle_error_msg_event(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr);

void
of_handle_port_status_msg_event(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr);
void
of_handle_flow_removed_event(cntlr_channel_info_t          *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr);

int32_t of_meter_frame_response(char *msg,
                                struct ofi_meter_rec_info *meter_entry_p,
                                uint32_t msg_length);
static inline int32_t
UpdateTableFlowEnryInstProp(struct ofi_table_features_info *pAppTbl,
    uint16_t                      length,
    struct ofp_table_feature_prop_instructions *pTblInsProp);
static inline int32_t
UpdateTableMissEnryInstProp(struct ofi_table_features_info *pAppTbl,
    uint16_t                                    length,
    struct ofp_table_feature_prop_instructions *pTblInsProp);
static inline int32_t
UpdateNextTableProp4FlowEntry(struct ofi_table_features_info *pAppTbl,
    uint16_t                                    length,
    struct ofp_table_feature_prop_next_tables  *pNextTblProp);
static inline int32_t
UpdateNextTableProp4MissEntry(struct ofi_table_features_info *pAppTbl,
    uint16_t              length,
    struct ofp_table_feature_prop_next_tables  *pNextTblProp);
static inline int32_t
UpdateTableFlowEnryWriteActnsProp(struct ofi_table_features_info *pAppTbl,
    uint16_t                               length,
    struct ofp_table_feature_prop_actions *pTblActnProp);
static inline int32_t
UpdateTableMissEnryWriteActnsProp(struct ofi_table_features_info *pAppTbl,
    uint16_t                               length,
    struct ofp_table_feature_prop_actions *pTblActnProp);
static inline int32_t
UpdateTableFlowEnryApplyActnsProp(struct ofi_table_features_info *pAppTbl,
    uint16_t                               length,
    struct ofp_table_feature_prop_actions *pTblActnProp);
static inline int32_t
UpdateTableMissEnryApplyActnsProp(struct ofi_table_features_info *pAppTbl,
    uint16_t                               length,
    struct ofp_table_feature_prop_actions *pTblActnProp);
static inline int32_t
UpdateTableMatchFiledProp(struct ofi_table_features_info *pAppTbl,
    uint16_t                               length,
    struct ofp_table_feature_prop_oxm     *pMatchFieldProp);
static inline int32_t
UpdateTableWildCardProp(struct ofi_table_features_info *pAppTbl,
    uint16_t                               length,
    struct ofp_table_feature_prop_oxm     *pMatchFieldProp);
static inline int32_t
UpdateTableWrSetfieldProp(struct ofi_table_features_info *pAppTbl,
    uint16_t                               length,
    struct ofp_table_feature_prop_oxm     *pMatchFieldProp);
static inline int32_t
UpdateTableWrSetfieldMissProp(struct ofi_table_features_info *pAppTbl,
    uint16_t                               length,
    struct ofp_table_feature_prop_oxm     *pMatchFieldProp);
static inline int32_t
UpdateTableApplySetfieldProp(struct ofi_table_features_info *pAppTbl,
    uint16_t                               length,
    struct ofp_table_feature_prop_oxm     *pMatchFieldProp);
static inline int32_t
UpdateTableApplySetfieldMissProp(struct ofi_table_features_info *pAppTbl,
    uint16_t                               length,
    struct ofp_table_feature_prop_oxm     *pMatchFieldProp);
extern void* 
cntlr_xtn_recrod_cleanup_func(void *cbk_arg1,
    void *cbk_arg2);
extern void of_handle_hello_message (cntlr_channel_info_t       *channel,
      struct dprm_datapath_entry *datapath,
      struct ofp_header          *msg_hdr);
extern void of_handle_feature_reply_message (cntlr_channel_info_t       *channel,
      struct dprm_datapath_entry *datapath,
      struct ofp_header          *msg_hdr);

extern int32_t pspBufCreateBufPools(void);
/* PROTOTYPE_START   ****************************************************/
/* GLOBAL_START  ********************************************************/


/*Configuratin details of controller maintained message pools.
*  * One need to tune as per application requirements
*   */

//cntlr_mem_app_pool_size_in_bytes_t cntlr_app_msg_pools [] = {200,2000,2000,2000,100,100,10,10,10,10};
//cntlr_mem_app_pool_size_in_bytes_t cntlr_app_msg_pools [] = {2000,10000,2000,2000,100,100,10,10,10,10};
#define MSG_MEM_POOL_MULTI_GRP_SUPPORT

#ifndef MSG_MEM_POOL_MULTI_GRP_SUPPORT  /** No 64K Packet size support*/
cntlr_mem_app_pool_size_in_bytes_t cntlr_app_msg_pools [] = {300,1500,300,300,300,20,20,10,10,10};
#define OF_TOTAL_NO_OF_CTRLR_APP_MSG_POOLS   sizeof(cntlr_app_msg_pools)/sizeof(cntlr_mem_app_pool_size_in_bytes_t)
#else  /** 64k packet size support*/
cntlr_mem_app_pool_size_in_bytes_t cntlr_app_msg_pools [] = {300,1500,300,300,300,20,20,10,10,10,10,
						             5,5,5,5,5,5,5,5,5,5,5,	
							     3,3,3,3,3,3,3,3,3,3,3};
cntlr_mem_pool_grp_granularity_t cntlr_mem_pool_grp_granularity [] = {128, 512, 2048};
#define OF_TOTAL_NO_OF_CTRLR_APP_MSG_POOLS   33
#define OF_TOTAL_NO_OF_CTRLR_MSG_MEM_POOL_GRPS   3
of_msg_mem_pool_grp_t msg_mem_pool_grp_g[OF_TOTAL_NO_OF_CTRLR_MSG_MEM_POOL_GRPS];
#endif

/*Global variable carrying information different message pools maintained controller*/
of_msg_pool_t CNTLR_PKTPROC_DATA of_cntlr_msg_pool_g;

/*TBD Currently one conroller instnace is supported, so this global variable difined
* temporarly, later need to drop*/
uint64_t controller_handle = 0; 

/*Memory pool used for application nodes to register asynchronous events*/
void *of_app_hook_mempool_g;


struct pkt_mbuf_conf PSP_PKTPROC_DATA pkt_mbuf_conf_g =  {0, /* HW area size */
                                         128, /* Pkt Prepend size */
                                         32, /* Pkt Append size */
                                         64, /* App scratch area size */
                                         32, /* 3rd part app buff area size */
                                         0 /* pool created - False. */
                                        };

                                                        //{1536, 4000},
                                                       // {64895,    5} //64k + 512



/*Following macros temporarily defined, once defined corresponding receiving handlers
* remove defintion*/
#define HELLO_CBK of_handle_hello_message
#define ERROR_CBK of_handle_error_msg_event
#define ECHO_REQUEST_CBK   of_echo_req_handler
#define ECHO_REPLY_CBK     of_echo_reply_handler
#define EXPERIMENTER_CBK   of_handle_experimenter_event
#define FEATURES_REQUEST_CBK  NULL
#define FEATURES_REPLY_CBK  of_handle_feature_reply_message
#define GET_CONFIG_REQUEST_CBK  NULL

#define GET_CONFIG_REPLY_CBK  of_cfg_reply_handler

#define SET_CONFIG_CBK  NULL
#define PACKET_IN_CBK of_handle_pkt_in_msg
#define FLOW_REMOVED_CBK of_handle_flow_removed_event
#define PORT_STATUS_CBK of_handle_port_status_msg_event
#define PACKET_OUT_CBK NULL
#define FLOW_MOD_CBK NULL
#define GROUP_MOD_CBK NULL
#define PORT_MOD_CBK NULL
#define TABLE_MOD_CBK NULL
#define MULTIPART_REQUEST_CBK NULL
#define MULTIPART_REPLY_CBK of_process_multipart_replymsg
#define BARRIER_REQUEST_CBK NULL
#define BARRIER_REPLY_CBK of_process_barrier_replymsg
#define QUEUE_GET_CONFIG_REQUEST_CBK  NULL
#define QUEUE_GET_CONFIG_REPLY_CBK of_msg_not_supported
#define ROLE_REQUEST_CBK NULL
#define ROLE_REPLY_CBK of_process_role_replymsg
#define GET_ASYNC_REQUEST_CBK NULL
#define GET_ASYNC_REPLY_CBK of_process_async_replymsg
#define SET_ASYNC_CBK NULL
#define METER_MOD_CBK NULL

/*Callback functions to receive messages received from datapath*/
cntlr_msg_cbk_fn CNTLR_PKTPROC_DATA cntlr_cbk[] = {
  HELLO_CBK, 
  ERROR_CBK,
  ECHO_REQUEST_CBK,  
  ECHO_REPLY_CBK,    
  EXPERIMENTER_CBK,  
  FEATURES_REQUEST_CBK, 
  FEATURES_REPLY_CBK, 
  GET_CONFIG_REQUEST_CBK, 
  GET_CONFIG_REPLY_CBK, 
  SET_CONFIG_CBK, 
  PACKET_IN_CBK,
  FLOW_REMOVED_CBK,
  PORT_STATUS_CBK,
  PACKET_OUT_CBK,
  FLOW_MOD_CBK,
  GROUP_MOD_CBK,
  PORT_MOD_CBK,
  TABLE_MOD_CBK,
  MULTIPART_REQUEST_CBK,
  MULTIPART_REPLY_CBK,
  BARRIER_REQUEST_CBK,
  BARRIER_REPLY_CBK,
  QUEUE_GET_CONFIG_REQUEST_CBK, 
  QUEUE_GET_CONFIG_REPLY_CBK,
  ROLE_REQUEST_CBK,
  ROLE_REPLY_CBK,
  GET_ASYNC_REQUEST_CBK, 
  GET_ASYNC_REPLY_CBK,
  SET_ASYNC_CBK,
  METER_MOD_CBK
};
#define OF_MSG_TYPE_LEN (sizeof(cntlr_cbk)/sizeof(cntlr_msg_cbk_fn))
int32_t of_drain_out_socket_buf(cntlr_channel_info_t  *channel, uint16_t length);
#define OF_SOCKET_BUFFER_READ_LENGTH_FOR_DRAIN_OUT 256

/* GLOBAL_END    ********************************************************/
/* FUNCTION_PROTOTYPE_START  ********************************************/

/* FUNCTION_PROTOTYPE_END    ********************************************/

/**This routine creates message buffer pools of different sizes*/
#ifndef MSG_MEM_POOL_MULTI_GRP_SUPPORT  /** No 64K Packet size support*/
  int32_t
of_cntlr_msg_init()
{
#if 0
  uint32_t                max_app_entries_static;
  uint32_t                max_app_entries;
#endif
  of_msgbuf_pool_entry_t *msg_pool_entry;
  uint32_t                ii;
  int32_t                   retval = OF_SUCCESS;
  int32_t                   status = OF_SUCCESS;

  do
  {
    of_cntlr_msg_pool_g.total_number_of_msg_pools = OF_TOTAL_NO_OF_CTRLR_APP_MSG_POOLS;
    of_cntlr_msg_pool_g.pool_entry = (of_msgbuf_pool_entry_t*)calloc( OF_TOTAL_NO_OF_CTRLR_APP_MSG_POOLS,
        sizeof(of_msgbuf_pool_entry_t) );

    for(ii = 0; ii < of_cntlr_msg_pool_g.total_number_of_msg_pools; ii++)
    {
      msg_pool_entry                       = of_cntlr_msg_pool_g.pool_entry + ii;
      msg_pool_entry->block_size           = (ii+1) * CNTLR_MEM_SIZE_GRANULARITY; 
      msg_pool_entry->number_of_blocks     = cntlr_app_msg_pools[ii];
      msg_pool_entry->mem_pool_id          = NULL;
    }

    retval = of_msg_pool_init(&of_cntlr_msg_pool_g);
    if(retval == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Init Open Flow message pool error \r\n", __FUNCTION__,__LINE__);
      status = OF_FAILURE;
      break;
    } 

#if 0
    of_msg_get_mempool_entries(&max_app_entries,&max_app_entries_static);
    retval  = mempool_create_pool("of_app_event_pool",
        sizeof(struct of_app_hook_info),
        max_app_entries,max_app_entries_static,
        max_app_entries_static/10, max_app_entries_static/3,
        max_app_entries_static/10,FALSE,FALSE,
        0,
        &of_app_hook_mempool_g);
    if(retval != MEMPOOL_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Create mempool failure Application pool registered for async message, retval=%d \r\n",
          __FUNCTION__,__LINE__,retval);
      status = OF_FAILURE;
      break;
    }
#endif
  }
  while(0);

  if(status == OF_FAILURE)
  {
    /*TBD cleanup*/

    return OF_FAILURE;
  }

  return OF_SUCCESS;
}
#else  /** 64k packet size support */
 int32_t
of_cntlr_msg_init()
{
  of_msgbuf_pool_entry_t *msg_pool_entry;
  uint32_t                ii = 0, pool = 0, pool_index = 0;
  int32_t                   retval = OF_SUCCESS;
  int32_t                   status = OF_SUCCESS;
  int32_t number_of_apps_per_pool=OF_TOTAL_NO_OF_CTRLR_APP_MSG_POOLS/OF_TOTAL_NO_OF_CTRLR_MSG_MEM_POOL_GRPS;
  int32_t jj;

  do
  {
    of_cntlr_msg_pool_g.pool_entry = (of_msgbuf_pool_entry_t*)calloc(OF_TOTAL_NO_OF_CTRLR_APP_MSG_POOLS,
        sizeof(of_msgbuf_pool_entry_t) );
    for(pool = 0; pool < OF_TOTAL_NO_OF_CTRLR_MSG_MEM_POOL_GRPS; pool++)
    {
      msg_mem_pool_grp_g[pool].number_of_apps_per_pool = number_of_apps_per_pool;

      if ( pool != 0 )
        msg_mem_pool_grp_g[pool].start_block = msg_mem_pool_grp_g[pool-1].start_block + (cntlr_mem_pool_grp_granularity[pool-1] * number_of_apps_per_pool);
      else
        msg_mem_pool_grp_g[pool].start_block = 0;

      msg_mem_pool_grp_g[pool].granularity = cntlr_mem_pool_grp_granularity[pool];

      of_cntlr_msg_pool_g.total_number_of_msg_pools += msg_mem_pool_grp_g[pool].number_of_apps_per_pool;

      pool_index = pool * msg_mem_pool_grp_g[pool].number_of_apps_per_pool;
      for(ii = pool_index; ii < of_cntlr_msg_pool_g.total_number_of_msg_pools; ii++)
      {
	      msg_pool_entry                       = of_cntlr_msg_pool_g.pool_entry + ii;
	      if ( pool == 0 )
		      msg_pool_entry->block_size           = (ii+1)  * msg_mem_pool_grp_g[pool].granularity;
	      else
	      {
		      jj= (ii+1) % number_of_apps_per_pool;
		      if (jj == 0)
			      jj=number_of_apps_per_pool;
		      msg_pool_entry->block_size           = ((jj * msg_mem_pool_grp_g[pool].granularity)  + (msg_mem_pool_grp_g[pool -1 ].start_block + (cntlr_mem_pool_grp_granularity[pool-1] * number_of_apps_per_pool)));
	      }
	      msg_pool_entry->number_of_blocks     = cntlr_app_msg_pools[ii];
	      msg_pool_entry->mem_pool_id          = NULL;
      }
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"poolid=%d number_of_apps_per_pool=%d start_block=%d granularity=%d total_number_of_msg_pools=%d\r\n",pool,msg_mem_pool_grp_g[pool].number_of_apps_per_pool,msg_mem_pool_grp_g[pool].start_block,msg_mem_pool_grp_g[pool].granularity,of_cntlr_msg_pool_g.total_number_of_msg_pools);
    }
    retval = of_msg_pool_init(&of_cntlr_msg_pool_g);
    if(retval == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Init Open Flow message pool error \r\n", __FUNCTION__,__LINE__);
      status = OF_FAILURE;
      break;
    } 
  }
  while(0);

  if(status == OF_FAILURE)
  {
    /*TBD cleanup*/
    return OF_FAILURE;
  }

  return OF_SUCCESS;
}
#endif
/*Callback function to release msg*/
void CNTLR_PKTPROC_FUNC ofu_free_message(struct of_msg *msg)
{
  of_msg_t      *of_msg_buf;

  if(msg != NULL)
  {
    of_msg_buf = (of_msg_t *)( (uint8_t*)msg - sizeof(of_msg_pool_pvt_info_t) ) ;
    of_msg_release(of_msg_buf);
  }
}

/*Utitility API to allocate message*/
#ifndef MSG_MEM_POOL_MULTI_GRP_SUPPORT  /** No 64K Packet size support*/
struct of_msg* CNTLR_PKTPROC_FUNC ofu_alloc_message( uint8_t  type, uint16_t size)
{
  of_msg_t      *of_msg_buf;
  struct of_msg *msg;
  uint8_t        pool_index;

  size += ROUND_UP(size);
  pool_index = size/CNTLR_MEM_SIZE_GRANULARITY;
  // pool_index = size << 6;

  if(!(size % CNTLR_MEM_SIZE_GRANULARITY))
  {
    pool_index--;
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Pool index %d size %d", pool_index,size);
  }


  if(pool_index >= OF_TOTAL_NO_OF_CTRLR_APP_MSG_POOLS)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Recv mem alloc req for more than length(=%d) supported",
        size);
    return NULL;
  }

  if(of_msg_alloc(&of_cntlr_msg_pool_g, 
        pool_index,
        size,
        &of_msg_buf) == OF_FAILURE)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Alloc message buffer error for pool %d size %d", pool_index, size);
    return NULL;
  }

  /*Initializing type, start_of_data, start_of_buffer and free_cbk,
  *     * Not initializing all fields, b'coz allocated buffer reset with zeros
  *         */
  msg                       = &of_msg_buf->msg_desc;
  msg->desc.type            = type;
  msg->desc.buffer_len      = size + ROUND_UP(size);
  msg->desc.data_len        = 0;
  msg->desc.start_of_data   =
    msg->desc.start_of_buffer = msg->data;
  msg->desc.free_cbk        = ofu_free_message;

  return msg;
}
#else  /** 64k packet size support */
struct of_msg* CNTLR_PKTPROC_FUNC ofu_alloc_message( uint8_t  type, uint16_t size)
{
  of_msg_t      *of_msg_buf;
  struct of_msg *msg;
  uint8_t        pool_index, pool_found = 0;
  uint32_t       pool = 0, mem_pool_index = 0;
  uint16_t new_size;

  size += ROUND_UP(size);
  for(pool = OF_TOTAL_NO_OF_CTRLR_MSG_MEM_POOL_GRPS - 1; pool >= 0; pool--)
  {
    if(size >= msg_mem_pool_grp_g[pool].start_block)
    {
      mem_pool_index = pool;
      pool_found = 1;
      new_size=size - msg_mem_pool_grp_g[pool].start_block;
      break;
    }
  }
  
  if(pool_found == 0)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," cannot find pool index for size %d, returning failure", size); 
    return NULL;
  }
   
  pool_index = (new_size)/msg_mem_pool_grp_g[mem_pool_index].granularity;
  // pool_index = size << 6;
  pool_index += (pool * (OF_TOTAL_NO_OF_CTRLR_APP_MSG_POOLS/OF_TOTAL_NO_OF_CTRLR_MSG_MEM_POOL_GRPS));
  if(!(new_size % msg_mem_pool_grp_g[mem_pool_index].granularity))
  {
    pool_index--;
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," Pool index %d size %d with mem pool granularity %d\r\n", pool_index, size, msg_mem_pool_grp_g[mem_pool_index].granularity);
  }

  if(pool_index >= OF_TOTAL_NO_OF_CTRLR_APP_MSG_POOLS)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Recv mem alloc req for more than length(=%d) supported",
        __FUNCTION__,__LINE__,size);
    return NULL;
  }

  if(of_msg_alloc(&of_cntlr_msg_pool_g, 
        pool_index,
        size,
        &of_msg_buf) == OF_FAILURE)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Alloc message buffer error \r\n", __FUNCTION__,__LINE__);
    return NULL;
  }

  /*Initializing type, start_of_data, start_of_buffer and free_cbk,
  *     * Not initializing all fields, b'coz allocated buffer reset with zeros
  *         */
  msg                       = &of_msg_buf->msg_desc;
  msg->desc.type            = type;
  msg->desc.buffer_len      = size + ROUND_UP(size);
  msg->desc.data_len        = 0;
  msg->desc.start_of_data   =
    msg->desc.start_of_buffer = msg->data;
  msg->desc.free_cbk        = ofu_free_message;

  return msg;
}
#endif
/** WARNING  Depnding on this macro to get msg memory type primitives,
* but there is a posiblity of adding new async message type before packet_in
*
* We are depnding on this to have better peformance, but we need better logic
 */
#define OF_FIRST_ASYNC_MSG_TYPE_ID OFPT_PACKET_IN 

int32_t CNTLR_PKTPROC_FUNC of_alloc_and_read_async_message(cntlr_channel_info_t     *channel,
    struct dprm_domain_entry  *p_dp_domain,
    struct of_async_msg_mem_primitive  *msg_type_mem_primitive,  
    struct ofp_header        *msg_hdr_p)
{
   uint16_t                  msg_buf_len=0;
   struct of_msg_descriptor *msg_desc;
   uint16_t                  len_of_bytes_2_read;
   uint16_t                 len_recvd;
   int32_t                   status = OF_SUCCESS; 
   int32_t                   retval = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "msg state %d", channel->msg_state);
   do
   {
      if(channel->msg_state == OF_CHN_START_READING_MSG)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OF_CHN_START_READING_MSG");
         if(msg_type_mem_primitive->is_configured)
         {
            msg_buf_len = (msg_type_mem_primitive->head_room_size + ntohs(msg_hdr_p->length));
            if(msg_type_mem_primitive->msg_alloc_cbk != NULL)
               channel->msg_buf = msg_type_mem_primitive->msg_alloc_cbk(msg_hdr_p->type,msg_buf_len);
            else
            {
               channel->msg_buf = ofu_alloc_message(msg_hdr_p->type,msg_buf_len);
               channel->msg_buf->desc.start_of_data += msg_type_mem_primitive->head_room_size;
            }
         }
         else if(p_dp_domain->default_msg_mem_primitive.is_configured) 
         {
            msg_buf_len = (p_dp_domain->default_msg_mem_primitive.head_room_size + ntohs(msg_hdr_p->length));
            channel->msg_buf = p_dp_domain->default_msg_mem_primitive.msg_alloc_cbk(msg_hdr_p->type,msg_buf_len);
         }
         else
         {
            switch( msg_hdr_p->type)
            {
               case OFPT_PACKET_IN:
                  {
                     int32_t flags=0;
                     struct pkt_mbuf *mbuf = NULL;

                     of_alloc_pkt_mbuf_and_set_of_msg(mbuf, channel->msg_buf,OFPT_PACKET_IN, ntohs(msg_hdr_p->length),flags, status);
                     if(status == FALSE)
                     {
                        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "pkt_mbuf_alloc() failed");
                        status = OF_FAILURE;
                        break;
                     }
                  }
                  break;
               default:
                  channel->msg_buf = ofu_alloc_message(msg_hdr_p->type,ntohs(msg_hdr_p->length));
                  break;
            }
#if 0
            if ( status == OF_FAILURE)
            {
               break;
            }
#endif
         }
         if(channel->msg_buf == NULL)
         {
            uint16_t   len_temp = (msg_buf_len == 0)?ntohs(msg_hdr_p->length):msg_buf_len;
            //        if(of_flush_msg_buf(channel, len_temp - sizeof(struct ofp_header)) != OF_SUCCESS)
            if(of_drain_out_socket_buf(channel, len_temp - sizeof(struct ofp_header)) != OF_SUCCESS)
            {
               OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,
                "Flushing TCP Buufer failure, controller will not be able to read proper messages\r\n");
            }
            //        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d Received async message allocation failed\r\n",__FUNCTION__,__LINE__);
            status = OF_FAILURE;
            break;
         }
         msg_desc= &channel->msg_buf->desc;

         memcpy((void*)msg_desc->start_of_data, (void*)msg_hdr_p,sizeof(struct ofp_header));
         msg_desc->length1 = msg_desc->data_len  = sizeof(struct ofp_header);
         msg_desc->ptr1    = msg_desc->start_of_data + msg_desc->data_len;

         channel->msg_state = OF_CHN_WAITING_FOR_COMPLETE_MSG;

         len_of_bytes_2_read = ntohs(msg_hdr_p->length) - channel->msg_buf->desc.length1;
         if(len_of_bytes_2_read == 0)
         {
            channel->msg_state = OF_CHN_RCVD_COMPLETE_MSG;
            break;
         }

         retval = cntlr_read_msg_recvd_from_dp(channel,
               channel->msg_buf->desc.ptr1,
               len_of_bytes_2_read,
               &len_recvd);
         if(retval == OF_FAILURE)
         {
            status = OF_FAILURE;
            break;
         }
         if( len_recvd == len_of_bytes_2_read ) 
         {
            channel->msg_state = OF_CHN_RCVD_COMPLETE_MSG;
            channel->msg_buf->desc.data_len = (channel->msg_buf->desc.length1 + len_recvd);
         }
         else
         {
            //channel->msg_buf->desc.ptr1    += len_recvd;
            channel->msg_buf->desc.length1 += len_recvd;
         }
         if(channel->msg_state == OF_CHN_WAITING_FOR_COMPLETE_MSG)
         {
           channel->msg_len_read = len_recvd;
         }
         break;
      }
      else if((channel->msg_state == OF_CHN_WAITING_FOR_COMPLETE_MSG) || (channel->msg_state == OF_CHN_WAITING_FOR_MSG_HEADER))//LAX
      {
         len_of_bytes_2_read = ntohs(msg_hdr_p->length) - channel->msg_buf->desc.length1;
         retval = cntlr_read_msg_recvd_from_dp(channel,
               channel->msg_buf->desc.ptr1,
               len_of_bytes_2_read,
               &len_recvd);
         if(retval == OF_FAILURE)
         {
            status = OF_FAILURE;
            break;
         }

         if( len_recvd == len_of_bytes_2_read ) 
         {
            channel->msg_state = OF_CHN_RCVD_COMPLETE_MSG;
            channel->msg_buf->desc.data_len = (channel->msg_buf->desc.length1 + len_recvd);
         }
         else
         {
            channel->msg_buf->desc.ptr1    += len_recvd;
            channel->msg_buf->desc.length1 += len_recvd;
         }
         break;
      }
   }
   while(0);

   if(status == OF_FAILURE)
   {
      if(channel->msg_buf != NULL)
      {
         of_check_pkt_mbuf_and_free(channel->msg_buf);
         channel->msg_buf = NULL;
      }
   }

   return status;
}


int32_t CNTLR_PKTPROC_FUNC of_alloc_and_read_message_from_dp(cntlr_channel_info_t  *channel,
    struct ofp_header    *msg_hdr_p)
{
  struct of_msg      *p_msg=NULL; 
  struct of_msg_descriptor *msg_desc;
  uint16_t             len_of_bytes_2_read;
  uint16_t                 len_recvd;
  int32_t             status = OF_SUCCESS; 
  int32_t             retval = OF_SUCCESS; 

  do
  {
    if(channel->msg_state == OF_CHN_START_READING_MSG)
    {
      p_msg = ofu_alloc_message(msg_hdr_p->type,ntohs(msg_hdr_p->length));
      if(p_msg == NULL)
      {
        //          if(of_flush_msg_buf(channel, ntohs(msg_hdr_p->length) - sizeof(struct ofp_header)) != OF_SUCCESS)
        if( of_drain_out_socket_buf(channel, ntohs(msg_hdr_p->length) - sizeof(struct ofp_header)) != OF_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Flushing TCP Buufer failure, controller will not be able to read proper messages\r\n");
        }
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Received response message allocation failed");
        status = OF_FAILURE;
        break;
      }
      channel->msg_buf = p_msg;

      msg_desc = &p_msg->desc; 

      memcpy((void*)msg_desc->start_of_data, (void*)msg_hdr_p,sizeof(struct ofp_header));
      msg_desc->length1 = msg_desc->data_len  = sizeof(struct ofp_header);
      msg_desc->ptr1    = msg_desc->start_of_data + msg_desc->data_len;

      len_of_bytes_2_read = ntohs(msg_hdr_p->length) - channel->msg_buf->desc.length1;
      if(len_of_bytes_2_read == 0)
      {
        channel->msg_state = OF_CHN_RCVD_COMPLETE_MSG;
        break;
      }

      retval = cntlr_read_msg_recvd_from_dp(channel,
          channel->msg_buf->desc.ptr1,
          len_of_bytes_2_read,
          &len_recvd);
      if(retval == OF_FAILURE)
      { 
        status = OF_FAILURE;
        break;
      }
      if( len_recvd == len_of_bytes_2_read ) 
      {
        channel->msg_state = OF_CHN_RCVD_COMPLETE_MSG;
        channel->msg_buf->desc.data_len = (channel->msg_buf->desc.length1 + len_recvd);
      }
      else
      {
        channel->msg_state = OF_CHN_WAITING_FOR_COMPLETE_MSG;
        channel->msg_buf->desc.ptr1    += len_recvd;
        channel->msg_buf->desc.length1 += len_recvd;
      }
      break;
    }
    else if(channel->msg_state == OF_CHN_WAITING_FOR_COMPLETE_MSG)
    {
      len_of_bytes_2_read = ntohs(msg_hdr_p->length) - channel->msg_buf->desc.length1;
      retval = cntlr_read_msg_recvd_from_dp(channel,
          channel->msg_buf->desc.ptr1,
          len_of_bytes_2_read,
          &len_recvd);
      if(retval == OF_FAILURE)
      {
        status = OF_FAILURE;
        break;
      }

      if( len_recvd == len_of_bytes_2_read ) 
      {
        channel->msg_state = OF_CHN_RCVD_COMPLETE_MSG;
        channel->msg_buf->desc.data_len = (channel->msg_buf->desc.length1 + len_recvd);
      }
      else
      {
        channel->msg_buf->desc.ptr1    += len_recvd;
        channel->msg_buf->desc.length1 += len_recvd;
      }
      break;
    }
  }
  while(0);

  if(status == OF_FAILURE)
  {
    if(p_msg != NULL)
    {
      p_msg->desc.free_cbk(p_msg);
      channel->msg_buf = NULL;
    }
  }

  return status;
}

  void
of_msg_not_supported(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr)
{
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"Unspported Message type(=%d) recvd from dp(=%llx),flushing the buffer\r\n",
      p_datapath->dpid,p_msg_hdr->type);
  of_flush_msg_buf(p_channel, ntohs(p_msg_hdr->length));
}

void CNTLR_PKTPROC_FUNC of_handle_pkt_in_msg(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr)
{
  struct ofp_packet_in  *pkt_in_msg_rcvd;
  struct ofl_packet_in   pkt_in_msg;
  struct ofp_match      *match_field; 
  struct ttp_table_entry *ttp_tbl_info_p=NULL;
  uint16_t               pkt_offset;
  uint8_t         msg_id = (OFPT_PACKET_IN - OF_FIRST_ASYNC_MSG_TYPE_ID +1);
  int32_t         retval = OF_SUCCESS;
  int32_t         status = OF_SUCCESS;

  do
  {
    OF_READ_ASYNC_MSG(p_channel, msg_id, p_msg_hdr);

    //    CNTLR_DUMP_MSG_RCVD_ON_XPRT(p_datapath->dpid,p_channel->msg_buf);

    pkt_in_msg_rcvd  = (struct ofp_packet_in*)(p_channel->msg_buf->desc.start_of_data);

    if(CNTLR_UNLIKELY(pkt_in_msg_rcvd->table_id == OFPTT_ALL))
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d Not a usable Table ID = %d\r\n",__FUNCTION__,__LINE__,pkt_in_msg_rcvd->table_id);
      status = OF_FAILURE;
      break;
    }
#if 0
    if(p_datapath->domain->table_p[pkt_in_msg_rcvd->table_id] == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d Table(Id=%d) not registered for the domain\r\n",
          __FUNCTION__,__LINE__,pkt_in_msg_rcvd->table_id);
      status = OF_FAILURE;
      break;
    }
#endif
    if(get_ttp_tbl_info(p_datapath->domain->ttp_name, pkt_in_msg_rcvd->table_id, &ttp_tbl_info_p)!=OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d Table(Id=%d) not registered for the domain\r\n",
          __FUNCTION__,__LINE__,pkt_in_msg_rcvd->table_id);
      status = OF_FAILURE;
      break;
    }

    match_field = &pkt_in_msg_rcvd->match; 
    if(CNTLR_UNLIKELY(ntohs(match_field->type) != OFPMT_OXM))
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d Unsupported match field type=%x received \r\n",
          __FUNCTION__,__LINE__,match_field->type);
      status = OF_FAILURE;
      break;
    }
    pkt_in_msg.match_fields_length = ntohs(match_field->length);
    pkt_in_msg.match_fields        = (uint8_t*)&match_field->oxm_fields;

    /*Remember buffer_id so that application pass it back as a packet out*/
    pkt_in_msg.buffer_id  = ntohl(pkt_in_msg_rcvd->buffer_id);

    /*Remember channel number so that application pass it back as part of  packet out
    * same can be used as output channel  for sendng packet out msg*/
    pkt_in_msg.channel_no = p_channel->auxiliary_id; 

    pkt_in_msg.reason     = pkt_in_msg_rcvd->reason;
    pkt_in_msg.table_id   = pkt_in_msg_rcvd->table_id;
    /*TBD ntohll pkt_in_msg.cookie    = pkt_in_msg_rcvd->cookie;*/
    pkt_in_msg.cookie    = ntohll(pkt_in_msg_rcvd->cookie); 

    pkt_offset           = ntohs(pkt_in_msg_rcvd->header.length) - 
      (sizeof(struct ofp_packet_in) + ntohs(pkt_in_msg_rcvd->total_len));

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," PKT_OFFSET=%d pkt_in_msg_rcvd->total_len=%d",
        pkt_offset,ntohs(pkt_in_msg_rcvd->total_len));
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," table-id = %d",pkt_in_msg.table_id);

    pkt_in_msg.packet    = p_channel->msg_buf->desc.start_of_data + sizeof(struct ofp_packet_in)+pkt_offset; 
    pkt_in_msg.packet_length = ntohs(pkt_in_msg_rcvd->total_len);

#if 0
    CNTLR_PKT_IN_DATA(p_datapath->dpid,pkt_in_msg);
#endif

    retval = handover_pkt_in_msg_to_app(p_datapath->datapath_handle, p_datapath->domain, 
                                         pkt_in_msg_rcvd->table_id, msg_id, p_channel->msg_buf, &pkt_in_msg);
    if(retval == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d Failed to handover the packet in message to app\r\n",__FUNCTION__,__LINE__);
      status = OF_FAILURE;
      break;
    }

  }
  while(0);
  if(CNTLR_LIKELY(p_channel->msg_state == OF_CHN_RCVD_COMPLETE_MSG))
    OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(p_channel);
}

void of_handle_flow_removed_event(cntlr_channel_info_t       *p_channel,
    				  struct dprm_datapath_entry *p_datapath,
    				  struct ofp_header          *p_msg_hdr)
{
  struct  ofl_flow_removed  flow_removed_msg;
  struct  ofp_flow_removed *flow_removed_msg_read;
  struct  ofp_match         *match_field;
  struct ttp_table_entry *ttp_tbl_info_p=NULL;
  uint8_t msg_id = (OFPT_FLOW_REMOVED - OF_FIRST_ASYNC_MSG_TYPE_ID +1);
  int32_t retval = OF_SUCCESS;
  int32_t status = OF_SUCCESS;

  do
  {
    OF_READ_ASYNC_MSG(p_channel,msg_id,p_msg_hdr);

    flow_removed_msg_read  = (struct ofp_flow_removed*)(p_channel->msg_buf->desc.start_of_data);

    flow_removed_msg.table_id      = (flow_removed_msg_read->table_id);

    if(CNTLR_UNLIKELY(flow_removed_msg.table_id == OFPTT_ALL))
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d Not a usable Table ID = %d\r\n",__FUNCTION__,__LINE__,flow_removed_msg.table_id);
      status = OF_FAILURE;
      break;
    }
#if 0
    if(p_datapath->domain->table_p[flow_removed_msg.table_id] == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d Table(Id=%d) not registered for the domain\r\n",
          __FUNCTION__,__LINE__,flow_removed_msg.table_id);
      status = OF_FAILURE;
      break;
    }
#endif
    if(get_ttp_tbl_info(p_datapath->domain->ttp_name, flow_removed_msg.table_id, &ttp_tbl_info_p)!=OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d Table(Id=%d) not registered for the domain\r\n",
          __FUNCTION__,__LINE__,flow_removed_msg.table_id);
      status = OF_FAILURE;
      break;
    }

 
    /*Remember flow entry details of the flow removed*/
    flow_removed_msg.cookie        = ntohll(flow_removed_msg_read->cookie);
    flow_removed_msg.priority      = ntohs(flow_removed_msg_read->priority);
    flow_removed_msg.reason        = (flow_removed_msg_read->reason);
    flow_removed_msg.duration_sec  = ntohl(flow_removed_msg_read->duration_sec);
    flow_removed_msg.duration_nsec = ntohl(flow_removed_msg_read->duration_nsec);
    flow_removed_msg.idle_timeout  = ntohs(flow_removed_msg_read->idle_timeout);
    flow_removed_msg.hard_timeout  = ntohs(flow_removed_msg_read->hard_timeout);
    flow_removed_msg.packet_count  = ntohll(flow_removed_msg_read->packet_count);
    flow_removed_msg.byte_count    = ntohll(flow_removed_msg_read->byte_count);

    /*Rember the match fields of flow removed*/
    match_field = &flow_removed_msg_read->match; 
    flow_removed_msg.match_field_len = ntohs(match_field->length)-4; 
    flow_removed_msg.match_fields    = (uint8_t*)&match_field->oxm_fields;

    retval = handover_flow_removed_event_to_app(p_datapath->datapath_handle,
        					p_datapath->domain,
        					flow_removed_msg.table_id,
        					msg_id,
        					p_channel->msg_buf,
        					&flow_removed_msg);
    if(retval == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,
          "%s %d Failed to handover the flow removed message to app\r\n",__FUNCTION__,__LINE__);
      status = OF_FAILURE;
      break;
    }
  }
  while(0);
  if(CNTLR_LIKELY(p_channel->msg_state == OF_CHN_RCVD_COMPLETE_MSG))
    OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(p_channel);
}

/* 
* Function    : of_handle_experimenter_event
* Description : Receive message from datapath and duplicate it
* Parameters  :
* Return      :
*/

void of_handle_experimenter_event(cntlr_channel_info_t       *p_channel,
                                  struct dprm_datapath_entry *p_datapath,
                                  struct ofp_header          *p_msg_hdr)
{
  uint8_t   msg_id = OF_ASYNC_MSG_EXPERIMENTER_EVENT;
  int32_t   status = OF_SUCCESS;
 

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Entered\r\n");
  do
  {
    OF_READ_ASYNC_MSG(p_channel, msg_id, p_msg_hdr);

    inform_event_to_app(p_datapath, DP_EXPERIMENTER_MSG_EVENT,
                       (void*)p_channel->msg_buf, NULL);
  }
  while(0);
  if(CNTLR_LIKELY(p_channel->msg_state == OF_CHN_RCVD_COMPLETE_MSG))
    OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(p_channel);

  return;
}


void  of_handle_error_msg_event(cntlr_channel_info_t       *p_channel,
    				struct dprm_datapath_entry *p_datapath,
    				struct ofp_header          *p_msg_hdr)
{
  struct ofl_error_msg  error_msg;
  struct ofp_error_msg *error_msg_rcvd;
  cntlr_transactn_rec_t    *p_xtn_rec;
  uint8_t         msg_id = OF_ASYNC_MSG_ERROR_EVENT;
  int32_t         retval = OF_SUCCESS;
  int32_t         status = OF_SUCCESS;
  uint8_t         type, sub_type;
  struct fslx_multipart_request *fsl_req_msg;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Entered\r\n");

  do
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,
               "p_msg_hdr->type=%x p_msg_hdr->xid=%ld \r\n",p_msg_hdr->type,p_msg_hdr->xid);

    if (p_msg_hdr->type == OFPT_ERROR)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"transaction ID %ld ", p_msg_hdr->xid);
      OF_READ_ASYNC_MSG(p_channel,msg_id,p_msg_hdr);

      error_msg_rcvd   = (struct ofp_error_msg*)(p_channel->msg_buf->desc.start_of_data);
      error_msg.type   = ntohs(error_msg_rcvd->type);
      error_msg.code_or_exp_type   = ntohs(error_msg_rcvd->code);

      if(error_msg.type == OFPET_EXPERIMENTER)
      {
        struct ofp_error_experimenter_msg *exp_error_msg_recv;

        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,
                   "Received Experimenter Error Message, exp_type(%d) ",error_msg.code_or_exp_type);
        exp_error_msg_recv= (struct ofp_error_experimenter_msg *)(p_channel->msg_buf->desc.start_of_data);
        error_msg.vendor_id = ntohl(exp_error_msg_recv->experimenter);
        error_msg.length = ntohs(exp_error_msg_recv->header.length) - sizeof(struct ofp_error_experimenter_msg); 
        error_msg.data   = exp_error_msg_recv->data;
        type = error_msg.type;
      }
      else
      {
        error_msg.length = ntohs(error_msg_rcvd->header.length) - sizeof(struct ofp_error_msg); 
        error_msg.data   = error_msg_rcvd->data;

        struct ofp_header *ofp_header_read = (struct ofp_header*)error_msg.data;
        type = ofp_header_read->type;
  
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Internal data type %d ",type);
  
        if( (type == OFPT_ECHO_REQUEST) || (type == OFPT_MULTIPART_REQUEST ) ||
            (type == OFPT_BARRIER_REQUEST )  || (type == OFPT_GROUP_MOD ) ||
            (type == OFPT_GET_CONFIG_REQUEST  )  )
        {
          retval = cntlr_get_xtn_details(ntohl(p_msg_hdr->xid),
              p_datapath->datapath_id,
              &p_xtn_rec);
          if(retval == OF_FAILURE)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,
                       "%s:%d Invalid Transaction ID%d \r\n", __FUNCTION__,__LINE__,p_msg_hdr->xid);
            return;
          }
        }
  
        if(type ==OFPT_MULTIPART_REQUEST)
        {
          struct ofp_multipart_request *req_msg= (struct ofp_multipart_request  *)error_msg.data;
          uint16_t  multipart_type = ntohs(req_msg->type);
  
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Multipart error reply Type=%d \r\n",multipart_type);
  
          switch(multipart_type)
          {
            case OFPMP_PORT_DESC:
              ((of_port_desc_cbk_fn)(p_xtn_rec->callback_fn))(NULL,
              controller_handle,
              p_datapath->datapath_handle,
              p_datapath->domain->domain_handle,
              0,
              NULL,
              NULL,
              OF_RESPONSE_STATUS_ERROR,
              NULL,
              TRUE);
  
              break;
            case OFPMP_PORT_STATS:
              ((of_port_stats_cbk_fn)(p_xtn_rec->callback_fn))(
              controller_handle,
              p_datapath->domain->domain_handle,
              p_datapath->datapath_handle,
              0,
              NULL,
              NULL,
              NULL,
              OF_RESPONSE_STATUS_ERROR,
              NULL,
              TRUE);
  
              break;
            case OFPMP_TABLE:
              ((of_table_stats_cbk_fn)(p_xtn_rec->callback_fn))(
              controller_handle,
              p_datapath->domain->domain_handle,
              p_datapath->datapath_handle,
              0,
              NULL,
              NULL,
              NULL,
              OF_RESPONSE_STATUS_ERROR,
              NULL,
              TRUE);
              break;
  
          case OFPMP_FLOW:
              ((of_flow_entry_stats_cbk_fn)(p_xtn_rec->callback_fn))
                (controller_handle,
                 p_datapath->domain->domain_handle,
                 p_datapath->datapath_handle,
                 0,
                 NULL,
                 NULL,
                 NULL,
                 OF_RESPONSE_STATUS_ERROR,
                 NULL,
                 TRUE);
              break;
            case OFPMP_AGGREGATE:
              ((of_aggregate_stats_cbk_fn)(p_xtn_rec->callback_fn))
                (
                 controller_handle,
                 p_datapath->domain->domain_handle,
                 p_datapath->datapath_handle,
                 0,
                 NULL,
                 NULL,
                 NULL,
                 OF_RESPONSE_STATUS_ERROR,
                 NULL,
                 TRUE);
  
              break;
            case OFPMP_GROUP_DESC:
              ((of_group_desc_cbk_fn)(p_xtn_rec->callback_fn))(NULL,
              controller_handle,
              p_datapath->datapath_handle,
              p_datapath->domain->domain_handle,
              NULL,
              NULL,
              OF_RESPONSE_STATUS_ERROR,
              NULL,
              TRUE);
  
              break;
            case OFPMP_GROUP:
              ((of_group_stats_cbk_fn)(p_xtn_rec->callback_fn))(NULL,
              controller_handle,
              p_datapath->datapath_handle,
              p_datapath->domain->domain_handle,
              0,
              NULL,
              NULL,
              OF_RESPONSE_STATUS_ERROR,
              NULL,
              TRUE);
              break;
            case OFPMP_GROUP_FEATURES:
              ((of_group_features_cbk_fn)(p_xtn_rec->callback_fn))(NULL,
              controller_handle,
              p_datapath->datapath_handle,
              p_datapath->domain->domain_handle,
              NULL,
              NULL,
              OF_RESPONSE_STATUS_ERROR,
              NULL
              );
  
              break;
            case OFPMP_TABLE_FEATURES:
              ((of_table_features_cbk_fn)(p_xtn_rec->callback_fn))(
              controller_handle,
              p_datapath->domain->domain_handle,
              p_datapath->datapath_handle,
              0,
              NULL,
              NULL,
              NULL,
              OF_RESPONSE_STATUS_ERROR,
              NULL,
              TRUE);
              break;
            case OFPMP_DESC:
              ((of_switch_info_cbk_fn)(p_xtn_rec->callback_fn))
                (NULL,
                 0,
                 OF_RESPONSE_STATUS_ERROR,
                 NULL);
              break;
  
            case OFPMP_METER_CONFIG:
              ((of_meter_config_cbk_fn)(p_xtn_rec->callback_fn))(NULL,
              controller_handle,
              p_datapath->domain->domain_handle,
              p_datapath->datapath_handle,
              NULL,
              NULL,
              OF_RESPONSE_STATUS_ERROR,
              NULL,
              TRUE);
              break;
  
            case OFPMP_METER:
              ((of_meter_stats_cbk_fn)(p_xtn_rec->callback_fn))(NULL,
              controller_handle,
              p_datapath->domain->domain_handle,
              p_datapath->datapath_handle,
              NULL,
              NULL,
              OF_RESPONSE_STATUS_ERROR,
              0,
              NULL,
              TRUE);
              break;
  
            case OFPMP_METER_FEATURES:
              ((of_meter_features_cbk_fn)(p_xtn_rec->callback_fn))(NULL,
              controller_handle,
              p_datapath->domain->domain_handle,
              p_datapath->datapath_handle,
              NULL,
              NULL,
              OF_RESPONSE_STATUS_ERROR,
              NULL
              );
              break;
            
            case OFPMP_QUEUE:
              OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"Multipart not yet supported");
              break;

          case OFPMP_EXPERIMENTER:
            fsl_req_msg= (struct fslx_multipart_request  *)error_msg.data;
            sub_type = ntohs(fsl_req_msg->subtype);
               switch(sub_type)
               {
                  case FSLX_BIND_MOD :
                    ((of_bind_stats_cbk_fn)(p_xtn_rec->callback_fn))
                     (controller_handle,
                      p_datapath->domain->domain_handle,
                      p_datapath->datapath_handle,
                      0,
                      NULL,
                      NULL,
                      NULL,
                      OF_RESPONSE_STATUS_ERROR,
                      NULL,
                      TRUE);

                   break;
              }
           }
        }
        else
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"error to be handled\r\n");
        }
      }
    }

    if( (type == OFPT_ECHO_REQUEST) || (type == OFPT_MULTIPART_REQUEST ) ||
        (type == OFPT_BARRIER_REQUEST )  || (type == OFPT_GROUP_MOD ) ||
        (type == OFPT_GET_CONFIG_REQUEST  )  )
    {
      cntlr_del_xtn_with_rec(p_xtn_rec);
    }

    if((p_msg_hdr->type == OFPT_ERROR) && (error_msg.type == OFPET_EXPERIMENTER))
    {
      /* Handling Bind Mod Error Codes */
      if((error_msg.code_or_exp_type == FSLXBMFC_BIND_OBJ_EXISTS) ||
         (error_msg.code_or_exp_type == FSLXBMFC_INVALID_BIND_OBJ) ||
         (error_msg.code_or_exp_type == FSLXBMFC_UNKNOWN_BIND_OBJ) ||
         (error_msg.code_or_exp_type == FSLXBMFC_BAD_COMMAND) ||
         (error_msg.code_or_exp_type == FSLXBMFC_UNKNOWN_ERROR) ||
         (error_msg.code_or_exp_type == FSLXBMFC_INVALID_BIND_OBJ))
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"handling bind mod error msg");
        inform_event_to_app(p_datapath,
                            DP_BIND_MOD_ERROR_MSG_EVENT,
                            (void*)p_channel->msg_buf,
                            (void*)&error_msg);
      }
    }
    else
    {
      inform_event_to_app(p_datapath,
            DP_ERROR_MESSAGE,
            (void*)p_channel->msg_buf,
            (void*)&error_msg);
    }
  }
  while(0);
  if(CNTLR_LIKELY(p_channel->msg_state == OF_CHN_RCVD_COMPLETE_MSG))
    OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(p_channel);
}

  void
of_handle_port_status_msg_event(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr)
{
  struct ofl_port_status  port_status_msg;
  struct ofl_port_desc_info *port_info = &port_status_msg.port_info;
  struct ofp_port *port_desc_read;
  uint8_t         msg_id = (OFPT_PORT_STATUS - OF_FIRST_ASYNC_MSG_TYPE_ID +1);
  int32_t         status = OF_SUCCESS;

  do
  {
    OF_READ_ASYNC_MSG(p_channel,msg_id,p_msg_hdr);

    port_desc_read  = &((struct ofp_port_status*)(p_channel->msg_buf->desc.start_of_data))->desc;

    strncpy(port_info->name,port_desc_read->name,OFP_MAX_PORT_NAME_LEN);
    memcpy(port_info->hw_addr,port_desc_read->hw_addr,OFP_ETH_ALEN);
    port_info->port_no    = ntohl(port_desc_read->port_no);
    port_info->config     = ntohl(port_desc_read->config);
    port_info->state      = ntohl(port_desc_read->state);
    port_info->curr       = ntohl(port_desc_read->curr);
    port_info->advertised = ntohl(port_desc_read->advertised);
    port_info->supported  = ntohl(port_desc_read->supported);
    port_info->peer       = ntohl(port_desc_read->peer);
    port_info->curr_speed = ntohl(port_desc_read->curr_speed);
    port_info->max_speed  = ntohl(port_desc_read->max_speed);
    port_status_msg.reason = ((struct ofp_port_status*)(p_channel->msg_buf->desc.start_of_data))->reason;

    inform_event_to_app(p_datapath,
        DP_PORT_STATUS_CHANGE,
        (void*)p_channel->msg_buf,
        (void*)&port_status_msg);
  }
  while(0);
  if(CNTLR_LIKELY(p_channel->msg_state == OF_CHN_RCVD_COMPLETE_MSG))
    OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(p_channel);
}


  void 
of_switch_info_response(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr,
    cntlr_transactn_rec_t      *p_xtn_rec,
    uint8_t                     more_data)
{
  struct of_msg             *msg=NULL;
  struct ofp_desc    *swinfo_desc_read;
  struct ofi_switch_info  switch_info;
  uint16_t                   len_rcvd;
  int32_t                    retval = OF_SUCCESS;
  int32_t                    status = OF_SUCCESS;
  uint16_t   bytes_to_read;
  uint16_t   read_bytes=0;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"entered");
  bytes_to_read=ntohs(p_msg_hdr->length) - sizeof(struct ofp_multipart_reply);

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"bytes_to_read = %d \r\n",bytes_to_read);

  /* Allocate requried message buffer required to read the switch information*/
  msg = ofu_alloc_message(p_msg_hdr->type,sizeof(struct ofp_desc));
  if(msg == NULL)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"message allocation failed -of_switch_info_response \r\n");
    if( of_drain_out_socket_buf(p_channel, ntohs(p_msg_hdr->length) - sizeof(struct ofp_multipart_reply)) != OF_SUCCESS)
      //    if(of_flush_msg_buf(p_channel, ntohs(p_msg_hdr->length) - sizeof(struct ofp_multipart_reply)) != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Flushing TCP Buufer failure, controller will not be able to read proper messages");
    }
    status = OF_FAILURE;
    return;
  }

  swinfo_desc_read = (struct ofp_desc *)(msg->desc.start_of_data);    

  retval = cntlr_read_msg_recvd_from_dp(p_channel,(uint8_t*)swinfo_desc_read,sizeof(struct ofp_desc),&len_rcvd);
  if(retval != CNTLR_CONN_READ_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in reading switch info");
    status = OF_FAILURE;
    msg->desc.free_cbk(msg); 
  }
  read_bytes += len_rcvd;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"bytes_to_read = %d read_bytes=%d \r\n",bytes_to_read,read_bytes);

  strcpy(switch_info.mfr_desc, swinfo_desc_read->mfr_desc);
  strcpy(switch_info.hw_desc, swinfo_desc_read->hw_desc);
  strcpy(switch_info.sw_desc, swinfo_desc_read->sw_desc);
  strcpy(switch_info.serial_num, swinfo_desc_read->serial_num);
  strcpy(switch_info.dp_desc, swinfo_desc_read->dp_desc);

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"mfr_desc %s hw_desc=%s \r\n",switch_info.mfr_desc,switch_info.hw_desc);
  if( read_bytes == bytes_to_read)
    status = OF_SUCCESS;
  else
    status = OF_FAILURE;

  if (status == OF_SUCCESS)
  {
    ((of_switch_info_cbk_fn)(p_xtn_rec->callback_fn))
      (msg,
       p_datapath->datapath_handle,
       OF_RESPONSE_STATUS_SUCCESS,
       &switch_info);
  }
  if(status == OF_FAILURE)
  {
    ((of_switch_info_cbk_fn)(p_xtn_rec->callback_fn))
      (NULL,
       0,
       OF_RESPONSE_STATUS_ERROR,
       NULL);
  }
}

  void 
of_handle_group_desc_response(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr,
    cntlr_transactn_rec_t      *p_xtn_rec,
    uint8_t                     more_data)
{
  uint32_t                   group_no;
  uint32_t                   no_of_groups;
  uint8_t                    is_last_group;
  struct of_msg             *msg;
  struct ofp_group_desc_stats    *group_desc_read;
  struct ofi_group_desc_info  group_desc;
  uint16_t                   len_rcvd;
  int32_t                    retval = OF_SUCCESS;
  int32_t                    status = OF_SUCCESS;
  uint16_t   bytes_to_read;
  uint16_t   read_bytes=0;
  uint16_t   length=0;
  char buf[1024];

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"entered");
  no_of_groups = OF_MULTIPART_RPLY_MSG_NO_OF_DESC(p_msg_hdr,struct ofp_group_desc_stats);
  if(no_of_groups == 0)
  {
    status=OF_FAILURE;
  }
  else
  {
    bytes_to_read=ntohs(p_msg_hdr->length) - sizeof(struct ofp_multipart_reply);
  }

  for(group_no = 1;(( group_no <= no_of_groups) && (( bytes_to_read - read_bytes) >= sizeof(struct ofp_group_desc_stats)));
      group_no++)
  {
    /* Allocate requried message buffer required to read one port description */
    msg = ofu_alloc_message(p_msg_hdr->type,sizeof(struct ofp_group_desc_stats));
    if(msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"message allocation failed - group description multipart reply, groupno %d",group_no);
      status = OF_FAILURE;
      break;
    }

    group_desc_read = (struct ofp_group_desc_stats *)(msg->desc.start_of_data);    

    retval = cntlr_read_msg_recvd_from_dp(p_channel,(uint8_t*)group_desc_read,sizeof(struct ofp_group_desc_stats),&len_rcvd);
    if(retval != CNTLR_CONN_READ_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in reading port descption info for port %d ",
          group_no);
      status = OF_FAILURE;
      msg->desc.free_cbk(msg); 
      break;
    }
    read_bytes += len_rcvd;
    group_desc.group_id    = ntohl(group_desc_read->group_id);
    group_desc.group_type    = group_desc_read->type;

    length= ntohs(group_desc_read->length) - sizeof(struct ofp_group_desc_stats);
    if (length > 0)
    {
      /*Need to handle bucket and action infomration */
      /*skipping this information now... */
      retval = cntlr_read_msg_recvd_from_dp(p_channel,(uint8_t*)buf,length,&len_rcvd);
      if(retval != CNTLR_CONN_READ_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error in reading flow entry info for flow entry number %d",
            group_no);
        status = OF_FAILURE;
        msg->desc.free_cbk(msg);
        break;
      }
      retval= of_group_frame_response(buf, &group_desc, len_rcvd);
      if(retval != OF_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"of_group_frame_response failed");
        //		status = OF_FAILURE;
        //		msg->desc.free_cbk(msg);
        //		break;
      }
      read_bytes += len_rcvd;
    }


    if( (more_data != TRUE) && ((bytes_to_read - read_bytes ) <= sizeof(struct ofp_group_desc_stats)))
      is_last_group = TRUE;
    else
      is_last_group = FALSE;

    ((of_group_desc_cbk_fn)(p_xtn_rec->callback_fn))(msg,
    controller_handle,
    p_datapath->domain->domain_handle,
    p_datapath->datapath_handle,
    p_xtn_rec->app_pvt_arg1,
    p_xtn_rec->app_pvt_arg2,
    OF_RESPONSE_STATUS_SUCCESS,
    &group_desc,
    is_last_group);
  }

  if(status == OF_FAILURE)
  {
    ((of_group_desc_cbk_fn)(p_xtn_rec->callback_fn))(NULL,
    controller_handle,
    p_datapath->datapath_handle,
    p_datapath->domain->domain_handle,
    NULL,
    NULL,
    OF_RESPONSE_STATUS_ERROR,
    NULL,
    TRUE);
  }
}

  void 
of_handle_group_stats_response(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr,
    cntlr_transactn_rec_t      *p_xtn_rec,
    uint8_t                     more_data)
{
  uint32_t                   group_no;
  uint32_t                   no_of_groups;
  uint8_t                    is_last_group;
  struct of_msg             *msg;
  struct ofp_group_stats    *group_stat_read;
  struct ofi_group_stat  group_stat;
  uint16_t                   len_rcvd;
  int32_t                    retval = OF_SUCCESS;
  int32_t                    status = OF_SUCCESS;
  uint16_t   bytes_to_read;
  uint16_t   read_bytes=0;
  uint16_t   length=0;
  char buf[1024];

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"entered");
  no_of_groups = OF_MULTIPART_RPLY_MSG_NO_OF_DESC(p_msg_hdr,struct ofp_group_stats);
  if(no_of_groups == 0)
  {
    status=OF_FAILURE;
  }
  else
  {
    bytes_to_read=ntohs(p_msg_hdr->length) - sizeof(struct ofp_multipart_reply);
  }

  for(group_no = 1;(( group_no <= no_of_groups) && (( bytes_to_read - read_bytes) >= sizeof(struct ofp_group_stats)));
      group_no++)
  {
    msg = ofu_alloc_message(p_msg_hdr->type,sizeof(struct ofp_group_stats));
    if(msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"message allocation failed - group statistics multipart reply, groupno %d",group_no);
      status = OF_FAILURE;
      break;
    }

    group_stat_read = (struct ofp_group_stats *)(msg->desc.start_of_data);    

    retval = cntlr_read_msg_recvd_from_dp(p_channel,(uint8_t*)group_stat_read,sizeof(struct ofp_group_stats),&len_rcvd);
    if(retval != CNTLR_CONN_READ_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in reading port descption info for groupid %d ",
          group_no);
      status = OF_FAILURE;
      msg->desc.free_cbk(msg); 
      break;
    }
    read_bytes += len_rcvd;
    group_stat.group_id    = ntohl(group_stat_read->group_id);
    group_stat.ref_count    = ntohl(group_stat_read->ref_count);
    group_stat.duration_sec    = ntohl(group_stat_read->duration_sec);
    group_stat.duration_nsec    = ntohl(group_stat_read->duration_nsec);
    group_stat.packet_count    = ntohll(group_stat_read->packet_count);
    group_stat.byte_count    = ntohll(group_stat_read->byte_count);

    length= ntohs(group_stat_read->length) - sizeof(struct ofp_group_stats);
    if (length > 0)
    {
      /*Need to handle bucket counter */
      /*skipping this information now... */
      retval = cntlr_read_msg_recvd_from_dp(p_channel,(uint8_t*)buf,length,&len_rcvd);
      if(retval != CNTLR_CONN_READ_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error in reading flow entry info for flow entry number %d",
            group_no);
        status = OF_FAILURE;
        msg->desc.free_cbk(msg);
        break;
      }
      read_bytes += len_rcvd;
    }


    if( (more_data != TRUE) && ((bytes_to_read - read_bytes ) <= sizeof(struct ofp_group_stats)))
      is_last_group = TRUE;
    else
      is_last_group = FALSE;

    ((of_group_stats_cbk_fn)(p_xtn_rec->callback_fn))(msg,
    controller_handle,
    p_datapath->domain->domain_handle,
    p_datapath->datapath_handle,
    group_stat.group_id,
    p_xtn_rec->app_pvt_arg1,
    p_xtn_rec->app_pvt_arg2,
    OF_RESPONSE_STATUS_SUCCESS,
    &group_stat,
    is_last_group);
  }

  if(status == OF_FAILURE)
  {
    ((of_group_stats_cbk_fn)(p_xtn_rec->callback_fn))(NULL,
    controller_handle,
    p_datapath->datapath_handle,
    p_datapath->domain->domain_handle,
    0,
    NULL,
    NULL,
    OF_RESPONSE_STATUS_ERROR,
    NULL,
    TRUE);
  }
}


  void 
of_handle_group_features_response(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr,
    cntlr_transactn_rec_t      *p_xtn_rec,
    uint8_t                     more_data)
{
  uint32_t                   group_no;
  uint32_t                   no_of_groups;
  uint8_t                    is_last_group;
  struct of_msg             *msg;
  struct ofp_group_features    *group_feature_read;
  struct ofi_group_features_info  group_feature;
  uint16_t                   len_rcvd;
  int32_t                    retval = OF_SUCCESS;
  int32_t                    status = OF_SUCCESS;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"entered");
  no_of_groups = OF_MULTIPART_RPLY_MSG_NO_OF_DESC(p_msg_hdr,struct ofp_group_features);

  for(group_no = 1; group_no <= no_of_groups;group_no++)
  {
    msg = ofu_alloc_message(p_msg_hdr->type,sizeof(struct ofp_group_features));
    if(msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"message allocation failed - group features multipart reply, groupno %d",group_no);
      status = OF_FAILURE;
      break;
    }

    group_feature_read = (struct ofp_group_features *)(msg->desc.start_of_data);    

    retval = cntlr_read_msg_recvd_from_dp(p_channel,(uint8_t*)group_feature_read,sizeof(struct ofp_group_features),&len_rcvd);
    if(retval != CNTLR_CONN_READ_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error in reading port descption info for port %d ",
          group_no);
      status = OF_FAILURE;
      msg->desc.free_cbk(msg); 
      break;
    }
    group_feature.types    = ntohl(group_feature_read->types);
    group_feature.capabilities    = ntohl(group_feature_read->capabilities);
    group_feature.max_groups[0]    = ntohl(group_feature_read->max_groups[0]);
    group_feature.max_groups[1]    = ntohl(group_feature_read->max_groups[1]);
    group_feature.max_groups[2]    = ntohl(group_feature_read->max_groups[2]);
    group_feature.max_groups[3]    = ntohl(group_feature_read->max_groups[3]);
    group_feature.actions[0]    = ntohl(group_feature_read->actions[0]);
    group_feature.actions[1]    = ntohl(group_feature_read->actions[1]);
    group_feature.actions[2]    = ntohl(group_feature_read->actions[2]);
    group_feature.actions[3]    = ntohl(group_feature_read->actions[3]);

    if ((more_data != TRUE))
      is_last_group = TRUE;
    else
      is_last_group = FALSE;

    ((of_group_features_cbk_fn)(p_xtn_rec->callback_fn))(msg,
    controller_handle,
    p_datapath->domain->domain_handle,
    p_datapath->datapath_handle,
    p_xtn_rec->app_pvt_arg1,
    p_xtn_rec->app_pvt_arg2,
    OF_RESPONSE_STATUS_SUCCESS,
    &group_feature
    );
  }

  if(status == OF_FAILURE)
  {
    ((of_group_features_cbk_fn)(p_xtn_rec->callback_fn))(NULL,
    controller_handle,
    p_datapath->datapath_handle,
    p_datapath->domain->domain_handle,
    NULL,
    NULL,
    OF_RESPONSE_STATUS_ERROR,
    NULL
    );
  }
}

  void 
of_handle_meter_config_response(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr,
    cntlr_transactn_rec_t      *p_xtn_rec,
    uint8_t                     more_data)
{
  uint32_t                   meter_no;
  uint32_t                   no_of_meters;
  uint8_t                    is_last_meter;
  struct of_msg              *msg;
  struct ofp_meter_config    *meter_config_read;
  struct ofi_meter_rec_info  meter_config_local;
  uint16_t                   len_rcvd;
  int32_t                    retval = OF_SUCCESS;
  int32_t                    status = OF_SUCCESS;
  uint16_t   bytes_to_read;
  uint16_t   read_bytes=0;
  uint16_t   length=0;
  char buf[1024];

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"entered");
  no_of_meters = OF_MULTIPART_RPLY_MSG_NO_OF_DESC(p_msg_hdr,struct ofp_meter_config);
  if(no_of_meters == 0)
  {
    status=OF_FAILURE;
  }
  else
  {
    bytes_to_read=ntohs(p_msg_hdr->length) - sizeof(struct ofp_multipart_reply);
  }

  for(meter_no = 1;(( meter_no <= no_of_meters) && (( bytes_to_read - read_bytes) >= sizeof(struct ofp_meter_config)));meter_no++)
  {
    /* Allocate requried message buffer required to read one meter description */
    msg = ofu_alloc_message(p_msg_hdr->type,sizeof(struct ofp_meter_config));
    if(msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"message allocation failed - meter config multipart reply, meterno %d",meter_no);
      status = OF_FAILURE;
      break;
    }

    meter_config_read = (struct ofp_meter_config *)(msg->desc.start_of_data);    

    retval = cntlr_read_msg_recvd_from_dp(p_channel,(uint8_t*)meter_config_read,sizeof(struct ofp_meter_config),&len_rcvd);

    if(retval != CNTLR_CONN_READ_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in reading meter config info for meter %d ",meter_no);
      status = OF_FAILURE;
      msg->desc.free_cbk(msg); 
      break;
    }
    read_bytes += len_rcvd;
    meter_config_local.meter_id = ntohl(meter_config_read->meter_id);
    meter_config_local.flags    = ntohs(meter_config_read->flags);

    length= ntohs(meter_config_read->length) - sizeof(struct ofp_meter_config);
    if (length > 0)
    {
      retval = cntlr_read_msg_recvd_from_dp(p_channel,(uint8_t*)buf,length,&len_rcvd);
      if(retval != CNTLR_CONN_READ_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error in reading flow entry info for flow entry number %d",
            meter_no);
        status = OF_FAILURE;
        msg->desc.free_cbk(msg);
        break;
      }
      retval= of_meter_frame_response(buf, &meter_config_local, len_rcvd);
      if(retval != OF_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"of_meter_frame_response failed");
      }
      read_bytes += len_rcvd;
    }

    if( (more_data != TRUE) && ((bytes_to_read - read_bytes ) <= sizeof(struct ofp_meter_config)))
      is_last_meter = TRUE;
    else
      is_last_meter = FALSE;

    ((of_meter_config_cbk_fn)(p_xtn_rec->callback_fn))(msg,
    controller_handle,
    p_datapath->domain->domain_handle,
    p_datapath->datapath_handle,
    p_xtn_rec->app_pvt_arg1,
    p_xtn_rec->app_pvt_arg2,
    OF_RESPONSE_STATUS_SUCCESS,
    &meter_config_local,
    is_last_meter);
  }

  if(status == OF_FAILURE)
  {
    ((of_meter_config_cbk_fn)(p_xtn_rec->callback_fn))(NULL,
    controller_handle,
    p_datapath->datapath_handle,
    p_datapath->domain->domain_handle,
    NULL,
    NULL,
    OF_RESPONSE_STATUS_ERROR,
    NULL,
    TRUE);
  }
}

  void 
of_handle_meter_stats_response(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr,
    cntlr_transactn_rec_t      *p_xtn_rec,
    uint8_t                     more_data)
{
  uint32_t                   meter_no;
  uint32_t                   no_of_meters;
  uint8_t                    is_last_meter;
  struct of_msg             *msg;
  struct ofp_meter_stats    *meter_stats_read;
  struct ofi_meter_stats_info  meter_stats_local;
  uint16_t                   len_rcvd;
  int32_t                    retval = OF_SUCCESS;
  int32_t                    status = OF_SUCCESS;
  uint16_t   bytes_to_read;
  uint16_t   read_bytes=0;
  uint16_t   length=0;
  char buf[1024];

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"entered");
  no_of_meters = OF_MULTIPART_RPLY_MSG_NO_OF_DESC(p_msg_hdr,struct ofp_meter_stats);
  if(no_of_meters == 0)
  {
    status=OF_FAILURE;
  }
  else
  {
    bytes_to_read=ntohs(p_msg_hdr->length) - sizeof(struct ofp_multipart_reply);
  }

  for(meter_no = 1;(( meter_no <= no_of_meters) && (( bytes_to_read - read_bytes) >= sizeof(struct ofp_meter_stats)));meter_no++)
  {
    msg = ofu_alloc_message(p_msg_hdr->type,sizeof(struct ofp_meter_stats));
    if(msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"message allocation failed - meter statistics multipart reply, meterno %d",meter_no);
      status = OF_FAILURE;
      break;
    }

    meter_stats_read = (struct ofp_meter_stats *)(msg->desc.start_of_data);    

    retval = cntlr_read_msg_recvd_from_dp(p_channel,(uint8_t*)meter_stats_read,sizeof(struct ofp_meter_stats),&len_rcvd);
    if(retval != CNTLR_CONN_READ_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in reading port descption info for meterid %d ",
          meter_no);
      status = OF_FAILURE;
      msg->desc.free_cbk(msg); 
      break;
    }
    read_bytes += len_rcvd;
    meter_stats_local.meter_id    = ntohl(meter_stats_read->meter_id);
    meter_stats_local.flow_count  = ntohl(meter_stats_read->flow_count);
    meter_stats_local.packet_in_count = ntohll(meter_stats_read->packet_in_count);
    meter_stats_local.byte_in_count   = ntohll(meter_stats_read->byte_in_count);
    meter_stats_local.duration_sec    = ntohl(meter_stats_read->duration_sec);
    meter_stats_local.duration_nsec   = ntohl(meter_stats_read->duration_nsec);

    length= ntohs(meter_stats_read->len) - sizeof(struct ofp_meter_stats);
    if (length > 0)
    {
      retval = cntlr_read_msg_recvd_from_dp(p_channel,(uint8_t*)buf,length,&len_rcvd);
      if(retval != CNTLR_CONN_READ_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error in reading flow entry info for flow entry number %d",
           meter_no);
        status = OF_FAILURE;
        msg->desc.free_cbk(msg);
        break;
      }
      read_bytes += len_rcvd;
    }

    if( (more_data != TRUE) && ((bytes_to_read - read_bytes ) <= sizeof(struct ofp_meter_stats)))
      is_last_meter = TRUE;
    else
      is_last_meter = FALSE;

    ((of_meter_stats_cbk_fn)(p_xtn_rec->callback_fn))(msg,
    controller_handle,
    p_datapath->domain->domain_handle,
    p_datapath->datapath_handle,
    p_xtn_rec->app_pvt_arg1,
    p_xtn_rec->app_pvt_arg2,
    OF_RESPONSE_STATUS_SUCCESS,
    no_of_meters,
    &meter_stats_local,
    is_last_meter);
  }

  if(status == OF_FAILURE)
  {
    ((of_meter_stats_cbk_fn)(p_xtn_rec->callback_fn))(NULL,
    controller_handle,
    p_datapath->datapath_handle,
    p_datapath->domain->domain_handle,
    NULL,
    NULL,
    OF_RESPONSE_STATUS_ERROR,
    0,
    NULL,
    TRUE);
  }
}


  void 
of_handle_meter_features_response(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr,
    cntlr_transactn_rec_t      *p_xtn_rec,
    uint8_t                     more_data)
{
  struct of_msg             *msg;
  struct ofp_meter_features    *meter_features_read;
  struct ofi_meter_features_info meter_features_local;
  uint16_t                   len_rcvd;
  int32_t                    retval = OF_SUCCESS;
  int32_t                    status = OF_SUCCESS;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"entered");

  msg = ofu_alloc_message(p_msg_hdr->type,sizeof(struct ofp_meter_features));
  if(msg == NULL)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"message allocation failed - meter features multipart reply");
    status = OF_FAILURE;
    return ;
  }

  meter_features_read = (struct ofp_meter_features *)(msg->desc.start_of_data);    

  retval = cntlr_read_msg_recvd_from_dp(p_channel,(uint8_t *)meter_features_read,sizeof(struct ofp_meter_features),&len_rcvd);
  if(retval != CNTLR_CONN_READ_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error in reading port descption info for port");
    status = OF_FAILURE;
    msg->desc.free_cbk(msg);
    return;
  }
  
  meter_features_local.max_meter = ntohl(meter_features_read->max_meter);
  meter_features_local.band_types = ntohl(meter_features_read->band_types);
  meter_features_local.capabilities = ntohl(meter_features_read->capabilities);
  meter_features_local.max_bands = meter_features_read->max_bands;
  meter_features_local.max_color = meter_features_read->max_color;

  if(status == OF_SUCCESS)
  {
    ((of_meter_features_cbk_fn)(p_xtn_rec->callback_fn))(msg,
    controller_handle,
    p_datapath->domain->domain_handle,
    p_datapath->datapath_handle,
    p_xtn_rec->app_pvt_arg1,
    p_xtn_rec->app_pvt_arg2,
    OF_RESPONSE_STATUS_SUCCESS,
    &meter_features_local
    );
  }

  if(status == OF_FAILURE)
  {
    ((of_meter_features_cbk_fn)(p_xtn_rec->callback_fn))(NULL,
    controller_handle,
    p_datapath->datapath_handle,
    p_datapath->domain->domain_handle,
    NULL,
    NULL,
    OF_RESPONSE_STATUS_ERROR,
    NULL
    );
  }
}
  void 
of_handle_port_status_response(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr,
    cntlr_transactn_rec_t      *p_xtn_rec,
    uint8_t                     more_data)
{
  uint32_t                   port_no;
  uint32_t                   no_of_ports;
  uint8_t                    is_last_port;
  struct of_msg             *msg;
  struct ofp_port           *port_desc_read;
  struct ofl_port_desc_info  port_desc;
  uint16_t                   len_rcvd;
  int32_t                    retval = OF_SUCCESS;
  int32_t                    status = OF_SUCCESS;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered");

  no_of_ports = OF_MULTIPART_RPLY_MSG_NO_OF_DESC(p_msg_hdr,struct ofp_port);

  for(port_no = 1; port_no <= no_of_ports;port_no++)
  {
    /* Allocate requried message buffer required to read one port description */
    msg = ofu_alloc_message(p_msg_hdr->type,sizeof(struct ofp_port));
    if(msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d message allocation failed - port description multipart reply,portno %d\r\n",
          __FUNCTION__,__LINE__,port_no);
      status = OF_FAILURE;
      break;
    }

    port_desc_read = (struct ofp_port *)(msg->desc.start_of_data);    

    retval = cntlr_read_msg_recvd_from_dp(p_channel,(uint8_t*)port_desc_read,sizeof(struct ofp_port),&len_rcvd);
    if(retval != CNTLR_CONN_READ_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error in reading port descption info for port %d \r\n",
          __FUNCTION__,__LINE__,port_no);
      status = OF_FAILURE;
      msg->desc.free_cbk(msg); 
      break;
    }

    strncpy(port_desc.name,port_desc_read->name,OFP_MAX_PORT_NAME_LEN);
    memcpy(port_desc.hw_addr,port_desc_read->hw_addr,OFP_ETH_ALEN);
    port_desc.port_no    = ntohl(port_desc_read->port_no);
    port_desc.config     = ntohl(port_desc_read->config);
    port_desc.state      = ntohl(port_desc_read->state);
    port_desc.curr       = ntohl(port_desc_read->curr);
    port_desc.advertised = ntohl(port_desc_read->advertised);
    port_desc.supported  = ntohl(port_desc_read->supported);
    port_desc.peer       = ntohl(port_desc_read->peer);
    port_desc.curr_speed = ntohl(port_desc_read->curr_speed);
    port_desc.max_speed  = ntohl(port_desc_read->max_speed);

    if(port_no == no_of_ports)
      is_last_port = TRUE;
    else
      is_last_port = FALSE;

    ((of_port_desc_cbk_fn)(p_xtn_rec->callback_fn))(msg,
    controller_handle,
    p_datapath->domain->domain_handle,
    p_datapath->datapath_handle,
    port_desc.port_no,
    p_xtn_rec->app_pvt_arg1,
    p_xtn_rec->app_pvt_arg2,
    OF_RESPONSE_STATUS_SUCCESS,
    &port_desc,
    is_last_port);
    if (is_last_port == TRUE) 
    {
       if ((p_channel->auxiliary_id == OF_TRNSPRT_MAIN_CONN_ID) && (p_channel->is_dp_ready != TRUE))
       {
          p_channel->is_dp_ready = TRUE;
          inform_event_to_app(p_channel->datapath,
                DP_READY_EVENT,NULL,NULL);

       }
    }
  }

  if(status == OF_FAILURE)
  {
    ((of_port_desc_cbk_fn)(p_xtn_rec->callback_fn))(NULL,
    controller_handle,
    p_datapath->datapath_handle,
    p_datapath->domain->domain_handle,
    0, 
    NULL,
    NULL,
    OF_RESPONSE_STATUS_ERROR,
    NULL,
    TRUE);
  }
}

  void 
of_handle_table_stats_response(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr,
    cntlr_transactn_rec_t      *p_xtn_rec,
    uint8_t                     more_data)
{
  uint32_t                   table_no;
  uint32_t                   no_of_tables;
  uint8_t                    is_last_table;
  struct of_msg             *msg;
  struct ofp_table_stats  *table_stats_read;
  struct ofi_table_stats_info  table_stats;
  uint16_t                   len_rcvd;
  int32_t                    retval = OF_SUCCESS;
  int32_t                    status = OF_SUCCESS;

  no_of_tables = OF_MULTIPART_RPLY_MSG_NO_OF_DESC(p_msg_hdr,struct ofp_table_stats);

  if(no_of_tables== 0)
  {
    status=OF_FAILURE;
  }
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s %d response num of table %d\r\n",__FUNCTION__,__LINE__, no_of_tables);
  for(table_no = 1; table_no <= no_of_tables;table_no++)
  {
    /* Allocate requried message buffer required to read one table description */
    msg = ofu_alloc_message(p_msg_hdr->type,sizeof(struct ofp_table_stats));
    if(msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d message allocation failed - table description multipart reply,tableno %d\r\n",
          __FUNCTION__,__LINE__,table_no);
      status = OF_FAILURE;
      break;
    }

    table_stats_read = (struct ofp_table_stats *)(msg->desc.start_of_data);    

    retval = cntlr_read_msg_recvd_from_dp(p_channel,(uint8_t*)table_stats_read,sizeof(struct ofp_table_stats),&len_rcvd);
    if(retval != CNTLR_CONN_READ_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error in reading table descption info for table %d \r\n",
          __FUNCTION__,__LINE__,table_no);
      status = OF_FAILURE;
      msg->desc.free_cbk(msg); 
      break;
    }
#if 0
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d tbleid %d actvcnt %d lookup %lld matched %lld \r\n",
        __FUNCTION__,__LINE__, 
        table_stats_read->table_id, table_stats_read->active_count,table_stats_read->lookup_count,table_stats_read->matched_count);
#endif
    table_stats.table_id    = table_stats_read->table_id;
    table_stats.active_count  = ntohl(table_stats_read->active_count);
    table_stats.lookup_count  = ntohll(table_stats_read->lookup_count);
    table_stats.matched_count   = ntohll(table_stats_read->matched_count);

    if((more_data != TRUE) && (table_no == no_of_tables))
      is_last_table = TRUE;
    else
      is_last_table = FALSE;
#if 1
    ((of_table_stats_cbk_fn)(p_xtn_rec->callback_fn))(
    controller_handle,
    p_datapath->domain->domain_handle,
    p_datapath->datapath_handle,
    table_no,
    msg,
    p_xtn_rec->app_pvt_arg1,
    p_xtn_rec->app_pvt_arg2,
    OF_RESPONSE_STATUS_SUCCESS,
    &table_stats,
    is_last_table);
#endif
  }

  if(status == OF_FAILURE)
  {
#if 1
    ((of_table_stats_cbk_fn)(p_xtn_rec->callback_fn))(
    controller_handle,
    p_datapath->domain->domain_handle,
    p_datapath->datapath_handle,
    table_no,
    NULL,
    NULL,
    NULL,
    OF_RESPONSE_STATUS_ERROR,
    NULL,
    TRUE);
#endif
  }
}

  void 
of_handle_flow_entry_stats_response(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr,
    cntlr_transactn_rec_t      *p_xtn_rec,
    uint8_t                     more_data)
{
  uint32_t                   flowentry_no;
  uint32_t                   no_of_flowstats;
  uint8_t                    is_last_flowentry;
  struct ofp_flow_stats     *flow_stats_read; 
  struct ofi_flow_entry_info  flow_stats;
  struct of_msg             *msg;
  uint16_t                   len_rcvd;
  int32_t                    retval = OF_SUCCESS;
  int32_t                    status = OF_SUCCESS;
  uint16_t   bytes_to_read=0;
  uint16_t   read_bytes=0;
  uint16_t   length=0;
  char buf[1024];


  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"entered \r\n");
  no_of_flowstats = OF_MULTIPART_RPLY_MSG_NO_OF_DESC(p_msg_hdr,struct ofp_flow_stats);
  if(no_of_flowstats ==0 )
  {
    status=OF_FAILURE;
  }
  else
  {
    bytes_to_read=ntohs(p_msg_hdr->length) - sizeof(struct ofp_multipart_reply) ;
  }
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," bytes_to_read = %d  read_bytes =%d \r\n", bytes_to_read,read_bytes);


  for(flowentry_no = 1; ((flowentry_no <= no_of_flowstats) && (( bytes_to_read - read_bytes) >= sizeof(struct ofp_flow_stats)));flowentry_no++)
  {
    memset(&flow_stats,0, sizeof(struct ofi_flow_entry_info));
    OF_LIST_INIT(flow_stats.match_field_list, NULL);
    OF_LIST_INIT(flow_stats.instruction_list, NULL);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"bytes_to_read = %d  read_bytes =%d \r\n", bytes_to_read,read_bytes);
    /* Allocate requried message buffer required to read one port description */
    msg = ofu_alloc_message(p_msg_hdr->type,sizeof(struct ofp_flow_stats));
    if(msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," message allocation failed - flow entry stats failed, flow entry no: %d\r\n",flowentry_no);
      status = OF_FAILURE;
      break;
    }

    flow_stats_read= (struct ofp_flow_stats *)(msg->desc.start_of_data);    

    /*Reading till byte count only, sizeof(struct ofp_flow_stats)-sizeof(struct ofp_match) = 48 bytes
    * After that we will have TLV format fileds which will read individually*/

    retval = cntlr_read_msg_recvd_from_dp(p_channel,(uint8_t*)flow_stats_read, (sizeof(struct ofp_flow_stats)-sizeof(struct ofp_match)), &len_rcvd);

    if(retval != CNTLR_CONN_READ_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in reading flow entry info for flow entry number %d \r\n",flowentry_no);
      status = OF_FAILURE;
      msg->desc.free_cbk(msg); 
      break;
    }
    read_bytes += len_rcvd;

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"\r\n len_rcvd =%d",len_rcvd);

    flow_stats.table_id =flow_stats_read->table_id;        
    flow_stats.priority = ntohs(flow_stats_read->priority);        
    flow_stats.hard_timeout =  ntohs(flow_stats_read->hard_timeout);         
    flow_stats.Idle_timeout = ntohs(flow_stats_read->idle_timeout);   
    flow_stats.alive_sec = ntohl(flow_stats_read->duration_sec);    
    flow_stats.alive_nsec  =  ntohl(flow_stats_read->duration_nsec); 
    flow_stats.cookie = ntohll(flow_stats_read->cookie);    
    flow_stats.packet_count = ntohll(flow_stats_read->packet_count);
    flow_stats.byte_count = ntohll(flow_stats_read->byte_count); 

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"table_id=%d alive_sec=%ld priority=%ld \r\n",flow_stats.table_id,flow_stats.alive_sec,flow_stats.priority);

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"\r\n flow_stats_read->length=%ld  len_rcvd =%d",flow_stats_read->length,len_rcvd);
    /*Now read the remaining part of the message which contains match field,
    * instruction, action ..etc */
    length= ntohs(flow_stats_read->length) - len_rcvd;
    if (length > 0 )
    {

      retval = cntlr_read_msg_recvd_from_dp(p_channel,(uint8_t*)buf,length,&len_rcvd);
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"\r\n length=%ld len_rcvd =%d", length,len_rcvd);
      if(retval != CNTLR_CONN_READ_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s:%d Error in reading flow entry info for flow entry number %d \r\n",
            __FUNCTION__,__LINE__,flowentry_no);
        status = OF_FAILURE;
        msg->desc.free_cbk(msg); 
        break;
      }
      retval= of_flow_stats_frame_respone(buf, &flow_stats, len_rcvd);
      if(retval != OF_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"of_flow_stats_frame_respone failed");
      }
      read_bytes += len_rcvd;
    }

    if( (more_data != TRUE) && ((bytes_to_read - read_bytes ) <= sizeof(struct ofp_flow_stats) ))
      is_last_flowentry = TRUE;
    else
      is_last_flowentry = FALSE;

    ((of_flow_entry_stats_cbk_fn)(p_xtn_rec->callback_fn))
      (controller_handle,
       p_datapath->domain->domain_handle,
       p_datapath->datapath_handle,
       flow_stats_read->table_id,
       msg,
       p_xtn_rec->app_pvt_arg1,
       p_xtn_rec->app_pvt_arg2,
       OF_RESPONSE_STATUS_SUCCESS,
       &flow_stats,
       is_last_flowentry
      );
  }
  if(status == OF_FAILURE)
  {
    ((of_flow_entry_stats_cbk_fn)(p_xtn_rec->callback_fn))
      (controller_handle,
       p_datapath->domain->domain_handle,
       p_datapath->datapath_handle,
       0, 
       NULL,
       NULL,
       NULL,
       OF_RESPONSE_STATUS_ERROR,
       NULL,
       TRUE);
  }
}

/*Vinod*/
  void
of_handle_bind_entry_stats_response(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header         *p_msg_hdr,
    cntlr_transactn_rec_t      *p_xtn_rec,
    uint8_t                     more_data)
{
  uint32_t                             bindentry_no;
  uint32_t                             no_of_bindstats;
  uint8_t                              is_last_bindentry;
  struct fslx_bind_obj_info_reply      *bind_stats_read;
  struct ofi_bind_entry_info           bind_stats;
  struct of_msg                        *msg;
  struct fslx_header                  *fsl_msg_hdr;
  uint16_t                             len_rcvd;
  int32_t                              retval = OF_SUCCESS;
  int32_t                              status = OF_SUCCESS;
  uint16_t   bytes_to_read=0;
  uint16_t   read_bytes=0;
  uint16_t   length=0;
  char buf[1024];
  


  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"entered \r\n");
  no_of_bindstats = FSLX_MULTIPART_RPLY_MSG_NO_OF_DESC(p_msg_hdr,struct fslx_bind_obj_info_reply);
  if(no_of_bindstats ==0 )
  {
    status=OF_FAILURE;
  }
  else
  {
    bytes_to_read=ntohs(p_msg_hdr->length) - sizeof(struct fslx_multipart_reply) ;
  }
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," bytes_to_read = %d  read_bytes =%d \r\n", bytes_to_read,read_bytes);
  for(bindentry_no = 1; ((bindentry_no <= no_of_bindstats) && (( bytes_to_read - read_bytes) >= sizeof(struct fslx_bind_obj_info_reply)));bindentry_no++)
  {
    memset(&bind_stats,0, sizeof(struct ofi_bind_entry_info));
    //OF_LIST_INIT(flow_stats.match_field_list, NULL);
      OF_LIST_INIT(bind_stats.instruction_list, NULL);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"bytes_to_read = %d  read_bytes =%d \r\n", bytes_to_read,read_bytes);
    /* Allocate requried message buffer required to read one port description */
    if(p_msg_hdr->type == OFPMP_EXPERIMENTER)
    {
      fsl_msg_hdr = (struct fslx_header*)p_msg_hdr;
    }
    msg = ofu_alloc_message(fsl_msg_hdr->subtype,sizeof(struct fslx_bind_obj_info_reply));
    if(msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," message allocation failed - flow entry stats failed, flow entry no: %d\r\n",bindentry_no);
      status = OF_FAILURE;
      break;
    }

    bind_stats_read= (struct fslx_bind_obj_info_reply *)(msg->desc.start_of_data);

    /*Reading till byte count only, sizeof(struct ofp_flow_stats)-sizeof(struct ofp_match) = 48 bytes
    * After that we will have TLV format fileds which will read individually*/

    retval = cntlr_read_msg_recvd_from_dp(p_channel,(uint8_t*)bind_stats_read, (sizeof(struct fslx_bind_obj_info_reply)), &len_rcvd);

    if(retval != CNTLR_CONN_READ_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in reading bind entry info for bind entry number %d \r\n",bindentry_no);
      status = OF_FAILURE;
      msg->desc.free_cbk(msg);
      break;
    }
    read_bytes += len_rcvd;

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"\r\n len_rcvd =%d",len_rcvd);

    bind_stats.bind_obj_id = bind_stats_read->bind_obj_id;
    bind_stats.cookie = ntohll(bind_stats_read->cookie);    
    bind_stats.type = bind_stats_read->type;    
    bind_stats.number_of_flows = ntohs(bind_stats_read->number_of_flows);
    bind_stats.packet_count = ntohll(bind_stats_read->packet_count);
    bind_stats.byte_count = ntohll(bind_stats_read->byte_count);
    bind_stats.error_count = ntohll(bind_stats_read->error_count);
/*
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"table_id=%d alive_sec=%ld priority=%ld \r\n",flow_stats.table_id,flow_stats.alive_sec,flow_stats.priority);

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"\r\n flow_stats_read->length=%ld  len_rcvd =%d",flow_stats_read->length,len_rcvd);
*/
    /*Now read the remaining part of the message which contains match field,
    * instruction, action ..etc */
    length= ntohs(bind_stats_read->length) - len_rcvd;
    if (length > 0 )
    {
      retval = cntlr_read_msg_recvd_from_dp(p_channel,(uint8_t*)buf,length,&len_rcvd);
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"\r\n length=%ld len_rcvd =%d", length,len_rcvd);
      if(retval != CNTLR_CONN_READ_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s:%d Error in reading bind entry info for bind entry number %d \r\n",
            __FUNCTION__,__LINE__,bindentry_no);
        status = OF_FAILURE;
        msg->desc.free_cbk(msg);
       break;
      }
      retval= of_bind_stats_frame_respone(buf, &bind_stats, len_rcvd);
      if(retval != OF_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"of_bind_stats_frame_respone failed");
      }
      read_bytes += len_rcvd;
    }

    if( (more_data != TRUE) && ((bytes_to_read - read_bytes ) <= sizeof(struct fslx_bind_obj_info_reply) ))
      is_last_bindentry = TRUE;
    else
      is_last_bindentry = FALSE;

    ((of_bind_stats_cbk_fn)(p_xtn_rec->callback_fn))
      (controller_handle,
       p_datapath->domain->domain_handle,
       p_datapath->datapath_handle,
       bind_stats_read->bind_obj_id,
       msg,
       p_xtn_rec->app_pvt_arg1,
       p_xtn_rec->app_pvt_arg2,
       OF_RESPONSE_STATUS_SUCCESS,
       &bind_stats,
       is_last_bindentry
      );
  }
    if(status == OF_FAILURE)
  {
    ((of_bind_stats_cbk_fn)(p_xtn_rec->callback_fn))
      (controller_handle,
       p_datapath->domain->domain_handle,
       p_datapath->datapath_handle,
       0,
       NULL,
       NULL,
       NULL,
       OF_RESPONSE_STATUS_ERROR,
       NULL,
       TRUE);
  }
}

void 
of_handle_aggregate_stats_response(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr,
    cntlr_transactn_rec_t      *p_xtn_rec,
    uint8_t                     more_data)
{
  uint32_t                   aggrstats_no;
  uint32_t                   no_of_aggrstats;
  uint8_t                    is_last_aggrstats;
  struct of_msg             *msg;
  struct ofp_aggregate_stats_reply *aggr_stats_read; 
  struct ofi_aggregate_flow_stats aggr_stats;
  uint16_t                   len_rcvd;
  int32_t                    retval = OF_SUCCESS;
  int32_t                    status = OF_SUCCESS;


  no_of_aggrstats = OF_MULTIPART_RPLY_MSG_NO_OF_DESC(p_msg_hdr,struct ofp_aggregate_stats_reply);
  if(no_of_aggrstats==0)
  {
    status=OF_FAILURE;
  }
  for(aggrstats_no = 1; aggrstats_no <= no_of_aggrstats;aggrstats_no++)
  {
    /* Allocate requried message buffer required to read one port description */
    msg = ofu_alloc_message(p_msg_hdr->type,sizeof(struct ofp_aggregate_stats_reply));
    if(msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d message allocation failed - aggregate stats fail for stats no: %d\r\n",
          __FUNCTION__,__LINE__,aggrstats_no);
      status = OF_FAILURE;
      break;
    }

    aggr_stats_read= (struct ofp_aggregate_stats_reply *)(msg->desc.start_of_data);    

    retval = cntlr_read_msg_recvd_from_dp(p_channel,(uint8_t*)aggr_stats_read,sizeof(struct ofp_aggregate_stats_reply),&len_rcvd);
    if(retval != CNTLR_CONN_READ_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s:%d Error in reading aggregate stats info for aggregate no %d \r\n",
          __FUNCTION__,__LINE__,aggrstats_no);
      status = OF_FAILURE;
      msg->desc.free_cbk(msg); 
      break;
    }

    aggr_stats.packet_count = ntohll(aggr_stats_read->packet_count);
    aggr_stats.byte_count = ntohll(aggr_stats_read->byte_count);    
    aggr_stats.flow_count = ntohl(aggr_stats_read->flow_count);      

    if((more_data != TRUE) && ( aggrstats_no == no_of_aggrstats))
      is_last_aggrstats = TRUE;
    else
      is_last_aggrstats = FALSE;

    ((of_aggregate_stats_cbk_fn)(p_xtn_rec->callback_fn))
      (
       controller_handle,
       p_datapath->domain->domain_handle,
       p_datapath->datapath_handle,
       aggrstats_no,// 0,  
       msg,
       p_xtn_rec->app_pvt_arg1,
       p_xtn_rec->app_pvt_arg2,
       OF_RESPONSE_STATUS_SUCCESS,
       &aggr_stats,
       is_last_aggrstats);
  }
  if(status == OF_FAILURE)
  {
    ((of_aggregate_stats_cbk_fn)(p_xtn_rec->callback_fn))
      (
       controller_handle,
       p_datapath->domain->domain_handle,
       p_datapath->datapath_handle,
       0, 
       NULL,
       NULL,
       NULL,
       OF_RESPONSE_STATUS_ERROR,
       NULL,
       TRUE);
  }
}


  void 
of_handle_port_stats_response(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr,
    cntlr_transactn_rec_t      *p_xtn_rec,
    uint8_t                     more_data)
{
  uint32_t                   port_no;
  uint32_t                   no_of_ports;
  uint8_t                    is_last_port;
  struct of_msg             *msg;
  struct ofp_port_stats  *port_stats_read;
  //struct ofp_port_stats  port_stats;
  struct ofi_port_stats_info  port_stats;
  uint16_t                   len_rcvd;
  int32_t                    retval = OF_SUCCESS;
  int32_t                    status = OF_SUCCESS;

  no_of_ports = OF_MULTIPART_RPLY_MSG_NO_OF_DESC(p_msg_hdr,struct ofp_port_stats);
  if(no_of_ports==0)
  {
    status=OF_FAILURE;
  }

  for(port_no = 1; port_no <= no_of_ports;port_no++)
  {
    /* Allocate requried message buffer required to read one port description */
    msg = ofu_alloc_message(p_msg_hdr->type,sizeof(struct ofp_port_stats));
    if(msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d message allocation failed - port description multipart reply,portno %d\r\n",
          __FUNCTION__,__LINE__,port_no);
      status = OF_FAILURE;
      break;
    }

    port_stats_read = (struct ofp_port_stats *)(msg->desc.start_of_data);    
    memset(port_stats_read, '\0', sizeof(struct ofp_port_stats));

    retval = cntlr_read_msg_recvd_from_dp(p_channel,(uint8_t*)port_stats_read,sizeof(struct ofp_port_stats),&len_rcvd);
    if(retval != CNTLR_CONN_READ_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error in reading port descption info for port %d \r\n",
          __FUNCTION__,__LINE__,port_no);
      status = OF_FAILURE;
      msg->desc.free_cbk(msg); 
      break;
    }
#if 0
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d portno %d rxp %lld tx %lld rxb %lld txb %lld rxd %lld txd %lld rxe %lld txe %lld \r\n",
        __FUNCTION__,__LINE__,
        port_stats_read->port_no,
        port_stats_read->rx_packets,
        port_stats_read->tx_packets,
        port_stats_read->rx_bytes,
        port_stats_read->tx_bytes,
        port_stats_read->rx_dropped,
        port_stats_read->tx_dropped,
        port_stats_read->rx_errors,
        port_stats_read->tx_errors);
#endif
    memset(&port_stats, '\0', sizeof(struct ofi_port_stats_info));
    port_stats.port_no    = ntohl(port_stats_read->port_no);
    port_stats.rx_packets     = ntohll(port_stats_read->rx_packets);
    port_stats.tx_packets     = ntohll(port_stats_read->tx_packets);
    port_stats.rx_bytes     = ntohll(port_stats_read->rx_bytes);
    port_stats.tx_bytes     = ntohll(port_stats_read->tx_bytes);
    port_stats.rx_dropped     = ntohll(port_stats_read->rx_dropped);
    port_stats.tx_dropped     = ntohll(port_stats_read->tx_dropped);
    port_stats.rx_errors     = ntohll(port_stats_read->rx_errors);
    port_stats.tx_errors     = ntohll(port_stats_read->tx_errors);
    port_stats.rx_frame_err     = ntohll(port_stats_read->rx_frame_err);
    port_stats.rx_over_err     = ntohll(port_stats_read->rx_over_err);
    port_stats.rx_crc_err     = ntohll(port_stats_read->rx_crc_err);
    port_stats.collisions     = ntohll(port_stats_read->collisions);
    port_stats.duration_sec     = ntohl(port_stats_read->duration_sec);
    port_stats.duration_nsec     = ntohl(port_stats_read->duration_nsec);

    if((more_data != TRUE) && (port_no == no_of_ports))
      is_last_port = TRUE;
    else
      is_last_port = FALSE;
#if 1
    ((of_port_stats_cbk_fn)(p_xtn_rec->callback_fn))(
    controller_handle,
    p_datapath->domain->domain_handle,
    p_datapath->datapath_handle,
    port_no,
    msg,
    p_xtn_rec->app_pvt_arg1,
    p_xtn_rec->app_pvt_arg2,
    OF_RESPONSE_STATUS_SUCCESS,
    &port_stats,
    is_last_port);
#endif
  }

  if(status == OF_FAILURE)
  {
#if 1
    ((of_port_stats_cbk_fn)(p_xtn_rec->callback_fn))(
    controller_handle,
    p_datapath->domain->domain_handle,
    p_datapath->datapath_handle,
    port_no,
    NULL,
    NULL,
    NULL,
    OF_RESPONSE_STATUS_ERROR,
    NULL,
    TRUE);
#endif
  }
}

  void
of_process_barrier_replymsg(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr)
{
  cntlr_transactn_rec_t    *p_xtn_rec;
  struct barrier_state_msg_queue_node *barrier_q_msg_node;
  struct dprm_domain_entry   *p_dp_domain = p_datapath->domain;
  int32_t                   retval = OF_SUCCESS;
  int32_t                   status = OF_SUCCESS;
  uint8_t type;
  int32_t                   no_of_msgs;
  uchar8_t offset;
  offset = BARRIER_STATE_MSG_QUEUE_NODE_LISTNODE_OFFSET;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"ENTERED");
  do
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Transaction ID %d",p_msg_hdr->xid);
    /* Get record of the transaction to which this response received*/
    retval = cntlr_get_xtn_details(ntohl(p_msg_hdr->xid),
        p_datapath->datapath_id,
        &p_xtn_rec);
    if(retval == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Invalid Transaction ID%d, while retrieving barrier reply", 
          p_msg_hdr->xid);
      status =  OF_FAILURE;
      break;
    }


    no_of_msgs=OF_LIST_COUNT(p_datapath->msg_queue_in_barrier_state);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"no_of_msgs in queue %d",no_of_msgs);
    OF_LIST_REMOVE_HEAD_AND_SCAN(p_datapath->msg_queue_in_barrier_state,barrier_q_msg_node,struct barrier_state_msg_queue_node*,offset)
    {
      type=((struct ofp_header*)barrier_q_msg_node->msg->desc.start_of_data)->type;
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Sending message from queue");
      retval=cntlr_send_msg_to_dp(barrier_q_msg_node->msg,
          barrier_q_msg_node->datapath,
          barrier_q_msg_node->conn_table,
          &(barrier_q_msg_node->conn_safe_ref),
          barrier_q_msg_node->cbk_fn,
          barrier_q_msg_node->cbk_arg1,
          barrier_q_msg_node->cbk_arg2);
      if(retval == OF_FAILURE)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"cntlr_send_msg_to_dp  fail, error=%d",retval);
        barrier_q_msg_node->msg->desc.free_cbk(barrier_q_msg_node->msg);
        status =  OF_FAILURE;
        break;
      }
      if ( type == OFPT_BARRIER_REQUEST)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"message in queue is barrier request");
        break;
      }
    }

    no_of_msgs=OF_LIST_COUNT(p_datapath->msg_queue_in_barrier_state);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"no_of_msgs in queue %d",no_of_msgs);

    if( no_of_msgs == 0)
    {
      p_datapath->barrier_state = DP_NOT_BARRIER_STATE;
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"barrier queue is empty..updating state");
    }

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"barrier reply notification fun  %p", p_xtn_rec->callback_fn);

    ((of_barrier_reply_notification_fn)(p_xtn_rec->callback_fn))(controller_handle,
    p_dp_domain->domain_handle,
    p_datapath->datapath_handle,
    NULL, 
    p_xtn_rec->app_pvt_arg1,
    p_xtn_rec->app_pvt_arg2,
    OF_RESPONSE_STATUS_SUCCESS);
  }
  while(0);

  if(status  == OF_FAILURE)
    ( (of_barrier_reply_notification_fn)(p_xtn_rec->callback_fn) )(controller_handle,
        p_dp_domain->domain_handle,
        p_datapath->datapath_handle,
        NULL, 
        NULL,
        NULL,
        OF_RESPONSE_STATUS_ERROR);
  cntlr_del_xtn_with_rec(p_xtn_rec);
  OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(p_channel);
}

void 
of_handle_experimenter_multipart_reply_message(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr,
    cntlr_transactn_rec_t      *p_xtn_rec,
    uint8_t                     more_data)
{
  struct fslx_multipart_reply *mp_reply;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"ENTRY");

  mp_reply = (struct fslx_multipart_reply *)p_msg_hdr;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," subtype = %d", ntohs(mp_reply->subtype));

  switch(ntohs(mp_reply->subtype))
  {  
    case FSLX_MP_BO_INFO:
      of_handle_bind_entry_stats_response(p_channel, p_datapath, p_msg_hdr,
                               p_xtn_rec, more_data);
      break;
  }
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"EXIT");
  return;
}

  static inline int32_t
ProcessTableFeatureProp(struct ofp_table_feature_prop_header *pTblpropHeader,
                        uint32_t table_feature_len,
                        cntlr_transactn_rec_t      *pXtnRec,
                        struct ofi_table_features_info *pOutTblFeature)
{
  struct ofp_table_feature_prop_header *pPropHdr;
  uint16_t                              FeatureMsgLen   = table_feature_len;
  uint16_t                              ProcessedMsgLen = 0;
  uint16_t                              PropLength      = 0;
  int32_t                               retval = OF_SUCCESS;
  int32_t                               status = OF_SUCCESS;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," FeatureMsgLen=%d table_feature_prop_hdr->length=%d \r\n", FeatureMsgLen, ntohs(pTblpropHeader->length));
  while( FeatureMsgLen > 64 )
  {
    pPropHdr      =(struct ofp_table_feature_prop_header* ) ((char*)pTblpropHeader + ProcessedMsgLen);

    PropLength = ntohs(pPropHdr->length);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"pPropHdr->type=%d PropLength=%d \r\n",ntohs(pPropHdr->type), ntohs(pPropHdr->length));
    if(PropLength == 0)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Invalid Table feature length %d received\r\n",
          __FUNCTION__,__LINE__,PropLength);
      break;
    }
    switch(ntohs(pPropHdr->type))
    {
#if 1
      case OFPTFPT_INSTRUCTIONS:
        if(PropLength > sizeof(struct ofp_table_feature_prop_instructions))
        {
          retval = UpdateTableFlowEnryInstProp( pOutTblFeature,
              PropLength,
              (struct ofp_table_feature_prop_instructions*)((char*)pTblpropHeader+ProcessedMsgLen) );
          if(retval == OF_FAILURE)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d update flow entry instruction failed \r\n",__FUNCTION__,__LINE__);
            status = OF_FAILURE;
          }
        }
        break;
      case OFPTFPT_INSTRUCTIONS_MISS:
        if(PropLength > sizeof(struct ofp_table_feature_prop_instructions))
        {
          retval = UpdateTableMissEnryInstProp( pOutTblFeature,
              PropLength,
              (struct ofp_table_feature_prop_instructions*)((char*)pTblpropHeader+ProcessedMsgLen) );
          if(retval == OF_FAILURE)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d update miss entry instruction failed \r\n",__FUNCTION__,__LINE__);
            status = OF_FAILURE;
          }
        }
        break;
      case OFPTFPT_NEXT_TABLES:
        if( PropLength  > sizeof(struct ofp_table_feature_prop_next_tables))
        {
          retval = UpdateNextTableProp4FlowEntry( pOutTblFeature,
              PropLength,
              (struct ofp_table_feature_prop_next_tables*)((char*)pTblpropHeader+ProcessedMsgLen) );
          if(retval == OF_FAILURE)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d update next table prop 4 flow entry failed \r\n",__FUNCTION__,__LINE__);
            status = OF_FAILURE;
          }
        }
        break;
      case OFPTFPT_NEXT_TABLES_MISS:
        if( PropLength  > sizeof(struct ofp_table_feature_prop_next_tables))
        {
          retval = UpdateNextTableProp4MissEntry( pOutTblFeature,
              PropLength,
              (struct ofp_table_feature_prop_next_tables*)((char*)pTblpropHeader+ProcessedMsgLen) );
          if(retval == OF_FAILURE)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d update next table prop 4 flow entry failed \r\n",__FUNCTION__,__LINE__);
            status = OF_FAILURE;
          }
        }
        break;
      case OFPTFPT_WRITE_ACTIONS:     
        if(PropLength > sizeof(struct ofp_table_feature_prop_actions))
        {
          retval = UpdateTableFlowEnryWriteActnsProp( pOutTblFeature,
              PropLength,
              (struct ofp_table_feature_prop_actions*)((char*)pTblpropHeader+ProcessedMsgLen) );
          if(retval == OF_FAILURE)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d update Write actions for miss entry failed \r\n",__FUNCTION__,__LINE__);
            status = OF_FAILURE;
          }
        }
        break;
      case OFPTFPT_WRITE_ACTIONS_MISS:
        if(PropLength > sizeof(struct ofp_table_feature_prop_actions))
        {
          retval = UpdateTableMissEnryWriteActnsProp(pOutTblFeature ,
              PropLength,
              (struct ofp_table_feature_prop_actions*)((char*)pTblpropHeader+ProcessedMsgLen) );
          if(retval == OF_FAILURE)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d update Write actions for miss entry failed \r\n",__FUNCTION__,__LINE__);
            status = OF_FAILURE;
          }
        }
        break;
      case OFPTFPT_APPLY_ACTIONS:
        if(PropLength > sizeof(struct ofp_table_feature_prop_actions))
        {
          retval = UpdateTableFlowEnryApplyActnsProp( pOutTblFeature,
              PropLength,
              (struct ofp_table_feature_prop_actions*)((char*)pTblpropHeader+ProcessedMsgLen) );
          if(retval == OF_FAILURE)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d update Apply actions for flow entry failed \r\n",__FUNCTION__,__LINE__);
            status = OF_FAILURE;
          }
        }
        break;
      case OFPTFPT_APPLY_ACTIONS_MISS:
        if(PropLength > sizeof(struct ofp_table_feature_prop_actions))
        {
          retval = UpdateTableMissEnryApplyActnsProp( pOutTblFeature,
              PropLength,
              (struct ofp_table_feature_prop_actions*)((char*)pTblpropHeader+ProcessedMsgLen) );
          if(retval == OF_FAILURE)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d update Apply actions for miss entry failed \r\n",__FUNCTION__,__LINE__);
            status = OF_FAILURE;
          }
        }
        break;
      case OFPTFPT_MATCH:
        retval = UpdateTableMatchFiledProp( pOutTblFeature,
            PropLength,
            (struct ofp_table_feature_prop_oxm*)((char*)pTblpropHeader+ProcessedMsgLen) );
        if(retval == OF_FAILURE)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d update match field prop failed \r\n",__FUNCTION__,__LINE__);
          status = OF_FAILURE;
        }

        break;
      case OFPTFPT_WILDCARDS:
        retval = UpdateTableWildCardProp( pOutTblFeature,
            PropLength,
            (struct ofp_table_feature_prop_oxm*)((char*)pTblpropHeader+ProcessedMsgLen) );
        if(retval == OF_FAILURE)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d update wild card prop failed \r\n",__FUNCTION__,__LINE__);
          status = OF_FAILURE;
        }
        break;
      case OFPTFPT_WRITE_SETFIELD:
        retval = UpdateTableWrSetfieldProp( pOutTblFeature,
            PropLength,
            (struct ofp_table_feature_prop_oxm*)((char*)pTblpropHeader+ProcessedMsgLen) );
        if(retval == OF_FAILURE)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d update set field oxm ids of flow entry prop failed \r\n",__FUNCTION__,__LINE__);
          status = OF_FAILURE;
        }
        break;
      case OFPTFPT_WRITE_SETFIELD_MISS:
        retval = UpdateTableWrSetfieldMissProp( pOutTblFeature,
            PropLength,
            (struct ofp_table_feature_prop_oxm*)((char*)pTblpropHeader+ProcessedMsgLen) );
        if(retval == OF_FAILURE)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d update set fields oxm ids of miss entry prop failed \r\n",__FUNCTION__,__LINE__);
          status = OF_FAILURE;
        }
        break;
      case OFPTFPT_APPLY_SETFIELD:        
        retval = UpdateTableApplySetfieldProp(pOutTblFeature ,
            PropLength,
            (struct ofp_table_feature_prop_oxm*)((char*)pTblpropHeader+ProcessedMsgLen) );
        if(retval == OF_FAILURE)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d update apply set field oxm ids failed \r\n",__FUNCTION__,__LINE__);
          status = OF_FAILURE;
        }

        break;
      case OFPTFPT_APPLY_SETFIELD_MISS:
        retval = UpdateTableApplySetfieldMissProp(pOutTblFeature ,
            PropLength,
            (struct ofp_table_feature_prop_oxm*)((char*)pTblpropHeader+ProcessedMsgLen) );
        if(retval == OF_FAILURE)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d update apply set field oxm ids for miss entry prop failed \r\n",__FUNCTION__,__LINE__);
          status = OF_FAILURE;
        }
        break;
      case OFPTFPT_EXPERIMENTER:
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Table feature type Experimenter not supported\r\n",__FUNCTION__,__LINE__);
        break;
      case OFPTFPT_EXPERIMENTER_MISS:
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Table feature type Experimenter not supported\r\n",__FUNCTION__,__LINE__);
        break;
      default :
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Invalid Table feature type %d received\r\n",
            __FUNCTION__,__LINE__,pPropHdr->type);
        return OF_FAILURE;
#endif 
    }
    if(status == OF_FAILURE)
      break;
    ProcessedMsgLen += PropLength + OF_TABLE_FEATURE_PROPERTY_PADDING_LEN(PropLength) ;
    FeatureMsgLen -= PropLength + OF_TABLE_FEATURE_PROPERTY_PADDING_LEN(PropLength) ;
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"FeatureMsgLen=%d  ProcessedMsgLen=%d \r\n",FeatureMsgLen,ProcessedMsgLen);
  } 

  if(status == OF_FAILURE)
  {
    /*TBD cleaning of already recorded properities, we can leave it as is but free the allocated memory*/
    return OF_FAILURE;
  }

  return  OF_SUCCESS;
}
  void 
of_table_feature_response(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr,
    cntlr_transactn_rec_t      *p_xtn_rec,
    uint8_t                     more_data)
{
  uint32_t                   rec_len=0, RemTblLen;
  uint32_t                   rec_no=0;
  uint8_t                    is_last_rec;
  struct of_msg             *msg;
  struct ofp_table_features *table_feature_read;
  struct ofi_table_features_info table_feature; 
  uint16_t                   len_rcvd;
  struct dprm_table_info oftable_info = {};
  struct dprm_domain_entry   *p_dp_domain = p_datapath->domain;
  int32_t                    retval = OF_SUCCESS;
  int32_t                    status = OF_SUCCESS;
  uint64_t  table_handle;
  char buf[4096];

  RemTblLen = (ntohs(p_msg_hdr->length) - sizeof(struct ofp_multipart_reply));
  while(RemTblLen >= sizeof (struct ofp_table_features))
  {
    /* Allocate requried message buffer required to read one port description */
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "RemTblLen=%d \r\n",RemTblLen);
    msg = ofu_alloc_message(p_msg_hdr->type,sizeof(struct ofp_table_features));
    if(msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d message allocation failed - table feature multipart reply,recno %d\r\n",
          __FUNCTION__,__LINE__,rec_no);
      status = OF_FAILURE;
      break;
    }

    table_feature_read = (struct ofp_table_features*)(msg->desc.start_of_data);    

    retval = cntlr_read_msg_recvd_from_dp(p_channel,(uint8_t*)table_feature_read ,sizeof(struct ofp_table_features),&len_rcvd);
    if(retval != CNTLR_CONN_READ_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error in reading table features%d \r\n",
          __FUNCTION__,__LINE__,rec_no);
      status = OF_FAILURE;
      msg->desc.free_cbk(msg); 
      break;
    }

    memset(&table_feature, 0, sizeof(struct ofi_table_features_info));
    strcpy(table_feature.name,table_feature_read->name); 
    table_feature.table_id	= table_feature_read->table_id;
    table_feature.metadata_write = ntohll(table_feature_read->metadata_write);	
    table_feature.metadata_match = ntohll(table_feature_read->metadata_match);	
    table_feature.metadata_match = ntohll(table_feature_read->metadata_match);	
    table_feature.max_entries = ntohl(table_feature_read->max_entries);
    rec_len = ntohs(table_feature_read->length);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"rec_len=%d table_feature_read->length=%d \r\n",rec_len,ntohs(table_feature_read->length));
    
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"table_feature_read->name=%s table_feature_read->table_id=%d \r\n",table_feature_read->name,table_feature_read->table_id);

    if(rec_len  > sizeof(struct	ofp_table_features))
    {
      retval = cntlr_read_msg_recvd_from_dp(p_channel,(uint8_t*)buf ,(rec_len-sizeof(struct ofp_table_features)), &len_rcvd);
      if(retval != CNTLR_CONN_READ_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error in reading table features%d \r\n",
            __FUNCTION__,__LINE__,rec_no);
        status = OF_FAILURE;
        msg->desc.free_cbk(msg); 
        break;
      }
    }
    
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "len_rcvd=%d \r\n",len_rcvd);
    ProcessTableFeatureProp(buf, rec_len, p_xtn_rec, &table_feature);

    if(RemTblLen > rec_len) 
      RemTblLen -= rec_len; 
    else
      RemTblLen = 0;
    if((more_data != TRUE) && (RemTblLen < sizeof(struct ofp_table_features) ))
      is_last_rec = TRUE;
    else
      is_last_rec = FALSE;
#if 1
    ((of_table_features_cbk_fn)(p_xtn_rec->callback_fn))(
    controller_handle,
    p_datapath->domain->domain_handle,
    p_datapath->datapath_handle,
    table_feature.table_id,
    msg,
    p_xtn_rec->app_pvt_arg1,
    p_xtn_rec->app_pvt_arg2,
    OF_RESPONSE_STATUS_SUCCESS,
    &table_feature,
    is_last_rec);
#endif
  }

  if(status == OF_FAILURE)
  {
    ((of_table_features_cbk_fn)(p_xtn_rec->callback_fn))(
    controller_handle,
    p_datapath->domain->domain_handle,
    p_datapath->datapath_handle,
    rec_no,
    NULL,
    NULL,
    NULL,
    OF_RESPONSE_STATUS_ERROR,
    NULL,
    TRUE);
  }
}


/******************************************************************************/
	void
of_process_role_replymsg(cntlr_channel_info_t       *p_channel,
		struct dprm_datapath_entry *p_datapath,
		struct ofp_header          *p_msg_hdr)
{
	struct ofp_role_request  *of_role_reply=NULL;
	cntlr_transactn_rec_t    *p_xtn_rec=NULL;
	uint8_t                  more_data = FALSE;
	uint16_t                 len_rcvd;
	int32_t                  retval = OF_SUCCESS;
	struct of_msg      *msg=NULL; 



	if( p_msg_hdr->type  != OFPT_ROLE_REPLY )
	{
		OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error - role reply message \r\n", __FUNCTION__,__LINE__);
		return;
	}

	if(p_channel->msg_state != OF_CHN_RCVD_COMPLETE_MSG)
	{
		retval = of_alloc_and_read_message_from_dp(p_channel, p_msg_hdr);
		if(retval == OF_FAILURE)
		{
		  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,
                             "%s: %d Error in alloc message data \r\n",__FUNCTION__,__LINE__);
			return;
		}
		if(p_channel->msg_state == OF_CHN_WAITING_FOR_COMPLETE_MSG)
		{
  			OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(p_channel);
			return;
		}
	}

	of_role_reply = (struct ofp_role_request  *)(p_channel->msg_buf->desc.start_of_data);

	/* Get record of the transaction to which this response received*/
	retval = cntlr_get_xtn_details(ntohl(p_msg_hdr->xid),
			p_datapath->datapath_id,
			&p_xtn_rec);
	if(retval == OF_FAILURE)
	{
	  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,
                  "%s:%d Invalid Transaction ID%d \r\n", __FUNCTION__,__LINE__,p_msg_hdr->xid);
	  return;
	}

	if(p_xtn_rec->callback_fn!=NULL){
		OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL,"calling callback....\n");

		((of_controller_role_change_notification_fn)p_xtn_rec->callback_fn)(0, 
			0,
			p_datapath->datapath_handle,
			NULL,
			NULL,
			NULL,
			OF_RESPONSE_STATUS_SUCCESS, 
			ntohl(of_role_reply->role));
	}

	cntlr_del_xtn_with_rec(p_xtn_rec);
	OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(p_channel);

}

/******************************************************************************/
	void
of_process_async_replymsg(cntlr_channel_info_t       *p_channel,
		struct dprm_datapath_entry *p_datapath,
		struct ofp_header          *p_msg_hdr)
{
	struct ofp_async_config      *of_async_config_msg=NULL;
	cntlr_transactn_rec_t    *p_xtn_rec=NULL;
	uint8_t                  more_data = FALSE;
	uint16_t                 len_rcvd;
	int32_t                  retval = OF_SUCCESS;
	struct of_msg      *msg=NULL; 


	OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "%s:%d  \r\n", __FUNCTION__,__LINE__);

	if( p_msg_hdr->type  != OFPT_GET_ASYNC_REPLY )
	{
		OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error - role reply message \r\n", __FUNCTION__,__LINE__);
		return;
	}

	if(p_channel->msg_state != OF_CHN_RCVD_COMPLETE_MSG)
	{
		retval = of_alloc_and_read_message_from_dp(p_channel, p_msg_hdr);
		if(retval == OF_FAILURE)
		{
			OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s: %d Error in alloc message data \r\n",__FUNCTION__,__LINE__);
			return;
		}
		if(p_channel->msg_state == OF_CHN_WAITING_FOR_COMPLETE_MSG)
			return;
	}

	of_async_config_msg = (struct ofp_async_config  *)(p_channel->msg_buf->desc.start_of_data);




	/* Get record of the transaction to which this response received*/
	retval = cntlr_get_xtn_details(ntohl(p_msg_hdr->xid),
			p_datapath->datapath_id,
			&p_xtn_rec);
	if(retval == OF_FAILURE)
	{
		OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Invalid Transaction ID%d \r\n", __FUNCTION__,__LINE__,p_msg_hdr->xid);
		return;
	}



	if(p_xtn_rec->callback_fn!=NULL){
		OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL,"calling callback....\n");

		((of_async_config_cbk_fn)p_xtn_rec->callback_fn)(0, 
			0,
			p_datapath->datapath_handle,
			of_async_config_msg,
			NULL,
			NULL,
			OF_RESPONSE_STATUS_SUCCESS 
			);
	}

	cntlr_del_xtn_with_rec(p_xtn_rec);
	OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(p_channel);

}

/*******************************************************************************************************/
  void
of_process_multipart_replymsg(cntlr_channel_info_t       *p_channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr)
{
  of_multipart_reply_hdr_t multipart_hdr;
  cntlr_transactn_rec_t    *p_xtn_rec;
  uint8_t                  more_data = FALSE;
  uint16_t                 len_rcvd;
  int32_t                  retval = OF_SUCCESS;


  /* Get record of the transaction to which this response received*/
  if( p_msg_hdr->type  == OFPT_MULTIPART_REPLY )
  {
    retval = cntlr_get_xtn_details(ntohl(p_msg_hdr->xid),
        p_datapath->datapath_id,
        &p_xtn_rec);
    if(retval == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Invalid Transaction ID%d \r\n", __FUNCTION__,__LINE__,p_msg_hdr->xid);
      return;
    }
    sleep(1); //Added to avoid sending response before finishing the multipart request processed by UCM.
  }
  retval = cntlr_read_msg_recvd_from_dp(p_channel,(uint8_t*)&multipart_hdr,sizeof(of_multipart_reply_hdr_t),&len_rcvd);
  if(retval != CNTLR_CONN_READ_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error in reading multipart heder \r\n", __FUNCTION__,__LINE__);
    return;
  }

  switch(ntohs(multipart_hdr.type))
  {
    case OFPMP_PORT_DESC:
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"port description recieved");
      of_handle_port_status_response(p_channel,
          p_datapath,
          p_msg_hdr, 
          p_xtn_rec,
          (more_data = (ntohs(multipart_hdr.flags) == OFPMPF_REPLY_MORE)));
      break;


    case OFPMP_PORT_STATS:
      of_handle_port_stats_response(p_channel,
          p_datapath,
          p_msg_hdr, 
          p_xtn_rec,
          (more_data = (ntohs(multipart_hdr.flags) == OFPMPF_REPLY_MORE)));

      break;

    case OFPMP_TABLE:
      of_handle_table_stats_response(p_channel,
          p_datapath,
          p_msg_hdr, 
          p_xtn_rec,
          (more_data = (ntohs(multipart_hdr.flags) == OFPMPF_REPLY_MORE)));

      break;

    case OFPMP_FLOW:
      of_handle_flow_entry_stats_response(p_channel,
          p_datapath,
          p_msg_hdr,
          p_xtn_rec,
          (more_data = (ntohs(multipart_hdr.flags) == OFPMPF_REPLY_MORE)));
      break;

    case OFPMP_AGGREGATE: 
      of_handle_aggregate_stats_response(p_channel,
          p_datapath,
          p_msg_hdr,
          p_xtn_rec,
          (more_data = (ntohs(multipart_hdr.flags) == OFPMPF_REPLY_MORE)));
      break;

    case OFPMP_GROUP_DESC:
      of_handle_group_desc_response(p_channel,
          p_datapath,
          p_msg_hdr,
          p_xtn_rec,
          (more_data = (ntohs(multipart_hdr.flags) == OFPMPF_REPLY_MORE)));
      break;

    case OFPMP_GROUP:
      of_handle_group_stats_response(p_channel,
          p_datapath,
          p_msg_hdr,
          p_xtn_rec,
          (more_data = (ntohs(multipart_hdr.flags) == OFPMPF_REPLY_MORE)));
      break;

    case OFPMP_GROUP_FEATURES:
      of_handle_group_features_response(p_channel,
          p_datapath,
          p_msg_hdr,
          p_xtn_rec,
          (more_data = (ntohs(multipart_hdr.flags) == OFPMPF_REPLY_MORE)));

      break;

    case OFPMP_TABLE_FEATURES:
      of_table_feature_response(p_channel,
          p_datapath,
          p_msg_hdr,
          p_xtn_rec,
          (more_data = (ntohs(multipart_hdr.flags) == OFPMPF_REPLY_MORE)));
      break;

    case OFPMP_DESC:
      of_switch_info_response(p_channel,
          p_datapath,
          p_msg_hdr,
          p_xtn_rec,
          (more_data = (ntohs(multipart_hdr.flags) == OFPMPF_REPLY_MORE)));
      break;
    
    case OFPMP_METER_CONFIG:
      of_handle_meter_config_response(p_channel,
          p_datapath,
          p_msg_hdr,
          p_xtn_rec,
          (more_data = (ntohs(multipart_hdr.flags) == OFPMPF_REPLY_MORE)));

      break;

    case OFPMP_METER:
      of_handle_meter_stats_response(p_channel,
          p_datapath,
          p_msg_hdr,
          p_xtn_rec,
          (more_data = (ntohs(multipart_hdr.flags) == OFPMPF_REPLY_MORE)));

      break;

    case OFPMP_METER_FEATURES:
      of_handle_meter_features_response(p_channel,
          p_datapath,
          p_msg_hdr,
          p_xtn_rec,
          (more_data = (ntohs(multipart_hdr.flags) == OFPMPF_REPLY_MORE)));

      break;

    case OFPMP_EXPERIMENTER:
      of_handle_experimenter_multipart_reply_message(p_channel,
          p_datapath,
          p_msg_hdr,
          p_xtn_rec,
          (more_data = (ntohs(multipart_hdr.flags) == OFPMPF_REPLY_MORE)));

    default:
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Invalid Multipart Msg reply type %d \r\n",
          __FUNCTION__,__LINE__,multipart_hdr.type);
  }

  if(p_channel->keep_alive_state == OF_TRNSPRT_CHN_CONN_KEEP_ALIVE_MSG_SENT)
    p_channel->keep_alive_state = p_channel->keep_alive_state|OF_TRNSPRT_CHN_CONN_KEEP_ALIVE_MSG_RESP_RCVD;

  OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(p_channel);

  if(more_data)
  {
    /*More responses are expected, re-record time stamp so that transactionr record  will not timedout.
    * Since reply to an OpenFlow request must be made on the same connection it came in,
    * and channel is affined to one thread, it is assumed that for given transaction record one thread 
    * is one owner, thereby no need to apply locks for transaction record modification
     */
    // GetJiffies(p_xtn_rec->time_stamp); /*TBD Time stamp*/
      
    timer_restart_tmr(NULL,&p_xtn_rec->timer_saferef,
        CNTLR_NOT_PEIRODIC_TIMER,
        CNTLR_XTN_NODE_TIMEOUT,
        cntlr_xtn_recrod_cleanup_func,
        (void *)p_xtn_rec,
        NULL);
  }
  else
  {
    cntlr_del_xtn_with_rec(p_xtn_rec);
  }
}
/** WARNING  We are depnding on this macro to get domain symmetric app callbacks,
*  but there is a posiblity of adding new symmetric message type before packet_in
*  
* We are depnding on this to have better peformance, better logic is TBD
**/
#define OF_FIRST_SYMMETRIC_MSG_TYPE_ID OF_SYMMETRIC_MSG_HELLO 
void CNTLR_PKTPROC_FUNC of_echo_req_handler(cntlr_channel_info_t       *channel,
    					    struct dprm_datapath_entry *p_datapath,
    					    struct ofp_header          *p_msg_hdr)
{
  struct dprm_domain_entry   *p_dp_domain = p_datapath->domain;
  struct ofl_echo_req_recv_msg echo_req_msg; 
  uint8_t msg_id = (OF_SYMMETRIC_MSG_ECHO_REQ - OF_FIRST_SYMMETRIC_MSG_TYPE_ID+1);
  echo_req_clbk_fn callback  = (echo_req_clbk_fn)(p_dp_domain->symmetric_msg_handle[msg_id].callback);
  int32_t   retval = OF_SUCCESS;

  if(channel->msg_state != OF_CHN_RCVD_COMPLETE_MSG)
  {
    retval = of_alloc_and_read_message_from_dp(channel, p_msg_hdr);
    if(retval == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d Error in alloc message data \r\n",__FUNCTION__,__LINE__);
      return;
    }
    if(channel->msg_state == OF_CHN_WAITING_FOR_COMPLETE_MSG)
      return;
  }

  echo_req_msg.data   = (uint8_t*)(channel->msg_buf->desc.start_of_data) + sizeof(struct ofp_header);
  echo_req_msg.length = channel->msg_buf->desc.data_len - sizeof(struct ofp_header); 

  if(callback != NULL)
  {
    callback(
        controller_handle,
        p_dp_domain->domain_handle,
        p_datapath->datapath_handle,
        channel->msg_buf,&echo_req_msg,NULL,NULL);

#if 0
    p_dp_domain->symmetric_msg_handle[msg_id].cbk_arg1,
      p_dp_domain->symmetric_msg_handle[msg_id].cbk_arg2);
#endif
  }
  else
  {
    channel->msg_buf->desc.free_cbk(channel->msg_buf);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d No echo request callback registeredd \r\n", __FUNCTION__,__LINE__);
  }

  OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(channel);
}

void of_cfg_reply_handler(cntlr_channel_info_t       *channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr )
{
  cntlr_transactn_rec_t    *p_xtn_rec=NULL;
  int32_t                   retval = OF_SUCCESS;
  struct dprm_domain_entry *p_dp_domain = p_datapath->domain;
  struct ofp_switch_config *switch_cnfg_rcv;
  struct ofl_switch_config switch_config_info;



  if(channel->msg_state != OF_CHN_RCVD_COMPLETE_MSG)
  {
    retval = of_alloc_and_read_message_from_dp(channel, p_msg_hdr);
    if(retval == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s: %d Error in alloc message data \r\n",__FUNCTION__,__LINE__);
      return;
    }
    if(channel->msg_state == OF_CHN_WAITING_FOR_COMPLETE_MSG)
      return;
  }

  switch_cnfg_rcv = (struct ofp_switch_config*)(channel->msg_buf->desc.start_of_data);
  switch_config_info.miss_send_len = ntohs(switch_cnfg_rcv->miss_send_len);
  switch_config_info.flags = ntohs(switch_cnfg_rcv->flags);

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"flags:%x,len:%d\n",switch_config_info.flags,switch_config_info.miss_send_len);

  retval = cntlr_get_xtn_details(ntohl(p_msg_hdr->xid),
      p_datapath->datapath_id,
      &p_xtn_rec);
  if(retval == OF_FAILURE)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Invalid Transaction ID%d \r\n", __FUNCTION__,__LINE__,p_msg_hdr->xid);
  OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(channel);
	return;
  }


  if(p_xtn_rec->callback_fn != NULL){
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"calling callback");
    ((of_switch_config_response_cbk_fn)(p_xtn_rec->callback_fn))(controller_handle,
    p_dp_domain->domain_handle,
    p_datapath->datapath_handle,
    channel->msg_buf, 
    p_xtn_rec->app_pvt_arg1,
    p_xtn_rec->app_pvt_arg2,
    OF_RESPONSE_STATUS_SUCCESS, &switch_config_info);
  }

  cntlr_del_xtn_with_rec(p_xtn_rec);
  OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(channel);
}


  void
of_echo_reply_handler(cntlr_channel_info_t       *channel,
    struct dprm_datapath_entry *p_datapath,
    struct ofp_header          *p_msg_hdr)
{
  cntlr_transactn_rec_t    *p_xtn_rec;
  int32_t                   retval = OF_SUCCESS;
  struct dprm_domain_entry *p_dp_domain = p_datapath->domain;
  echo_reply_clbk_fn  callback;


  if( p_msg_hdr->type  != OFPT_ECHO_REPLY )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,
              "%s:%d Error - not a echo reply message \r\n", __FUNCTION__,__LINE__);
    return;
  }

  if(channel->msg_state != OF_CHN_RCVD_COMPLETE_MSG)
  {
    retval = of_alloc_and_read_message_from_dp(channel, p_msg_hdr);
    if(retval == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,
      "%s: %d Error in alloc message data \r\n",__FUNCTION__,__LINE__);
      return;
    }
    if(channel->msg_state == OF_CHN_WAITING_FOR_COMPLETE_MSG)
      return;
  }

#if 0

  echo_req_msg.data   = (uint8_t*)(channel->msg_buf->desc.start_of_data) + sizeof(struct ofp_header);
  echo_req_msg.length = channel->msg_buf->desc.data_len - sizeof(struct ofp_header); 

  /*TBD of handling echo response data*/
#endif

  retval = cntlr_get_xtn_details(ntohl(p_msg_hdr->xid),
			         p_datapath->datapath_id,
      				&p_xtn_rec);
  if(retval == OF_FAILURE)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,
             "%s:%d Invalid Transaction ID%d \r\n", __FUNCTION__,__LINE__,p_msg_hdr->xid);
    OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(channel);
    return;
  }

  callback = (echo_reply_clbk_fn)(p_xtn_rec->callback_fn);
  if(callback != NULL)
    callback(controller_handle,
        p_dp_domain->domain_handle,
        p_datapath->datapath_handle,
        channel->msg_buf, 
        NULL, 
        p_xtn_rec->app_pvt_arg1,
        p_xtn_rec->app_pvt_arg2);

  if(channel->msg_buf != NULL)
  {
     channel->msg_buf->desc.free_cbk(channel->msg_buf);
     channel->msg_buf = NULL;
  }
  cntlr_del_xtn_with_rec(p_xtn_rec);
  OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(channel);
}

inline int32_t CNTLR_PKTPROC_FUNC of_process_dp_message(cntlr_channel_info_t *channel,
    struct ofp_header    *p_msg_hdr)
{
  cntlr_msg_cbk_fn msg_handle;

  if (p_msg_hdr->type < OF_MSG_TYPE_LEN)
  {
    msg_handle = cntlr_cbk[p_msg_hdr->type];
  }
  else
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Invalid message type=%d \r\n",__FUNCTION__,__LINE__,p_msg_hdr->type);
    if(channel->msg_buf != NULL)
    {
	    of_check_pkt_mbuf_and_free(channel->msg_buf);
    }
    OF_TRNSPRT_CHN_RESET_2_READ_NEW_MSG(channel);
    return OF_FAILURE;
  }

  if( (msg_handle != NULL) && (channel->is_chn_added_to_dp) )
    msg_handle(channel,channel->datapath,p_msg_hdr);

  return OF_SUCCESS;
}

/*TBD create static buffer and flush it to same buffer in a loop
* also need to read complete buffer, it may need to run it multipe times
 */ 
int32_t of_flush_msg_buf(cntlr_channel_info_t  *channel, uint16_t length)
{
  uint8_t     *buffer = NULL;
  uint16_t	read_len = 0;
  int32_t	retval = 0;

  buffer = (uint8_t*)calloc(1,length);
  if(buffer == NULL)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, 
        "%s %d Memory Alloc error Something critical ............\r\n",__FUNCTION__,__LINE__);
    return OF_FAILURE;
  }

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s %d Flushing message buffer of length:%d.............\r\n",__FUNCTION__,__LINE__,length);
  retval = cntlr_read_msg_recvd_from_dp(channel, buffer, length, &read_len);
  if((length - read_len) != 0)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Doesn't read full lentgth, critical .............\r\n",__FUNCTION__,__LINE__);
  }
  free(buffer);
  return OF_SUCCESS;
}

int32_t of_drain_out_socket_buf(cntlr_channel_info_t  *channel, uint16_t length)
{
  uint8_t     buffer[OF_SOCKET_BUFFER_READ_LENGTH_FOR_DRAIN_OUT];
  uint16_t	read_len = 0;
  int32_t	retval = 0;
  int32_t bytes_to_read=length;
  int32_t	status=OF_SUCCESS;


  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"entered");

  while ( bytes_to_read > 0 )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"bytes to read :%d in message", bytes_to_read);
    if ( bytes_to_read > OF_SOCKET_BUFFER_READ_LENGTH_FOR_DRAIN_OUT) 
    {
      retval = cntlr_read_msg_recvd_from_dp(channel, buffer, OF_SOCKET_BUFFER_READ_LENGTH_FOR_DRAIN_OUT, &read_len);
      bytes_to_read -= read_len;
      if(retval == CNTLR_CONN_NO_DATA)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "EAGAIN error, left to epoll bytes_to_read=%d read_len=%d",bytes_to_read, read_len);
        channel->drain_out_bytes = bytes_to_read; 
        return OF_FAILURE;
      }
      if(retval == OF_FAILURE)
      {
	      channel->msg_state =OF_CHN_WAITING_FOR_NEW_MSG;
	      channel->drain_out_bytes = 0;
	      return OF_FAILURE;
      } 
      if( read_len != OF_SOCKET_BUFFER_READ_LENGTH_FOR_DRAIN_OUT)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Doesn't read full lentgth,read_len =%d",read_len);
      }
      if(bytes_to_read == 0)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Drainout bytes are zero");
        channel->msg_state =OF_CHN_WAITING_FOR_NEW_MSG;
        channel->drain_out_bytes = 0; 
      }
    }
    else
    {
      retval = cntlr_read_msg_recvd_from_dp(channel, buffer, bytes_to_read, &read_len);
      bytes_to_read -= read_len;
      if(retval == CNTLR_CONN_NO_DATA)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "EAGAIN error, left to epoll bytes_to_read=%d read_len=%d",bytes_to_read, read_len);
        channel->drain_out_bytes = bytes_to_read; 
        return OF_FAILURE;
      }
      if(retval == OF_FAILURE)
      {
	      channel->msg_state =OF_CHN_WAITING_FOR_NEW_MSG;
	      channel->drain_out_bytes = 0;
        return OF_FAILURE;
      }
      if( (read_len != bytes_to_read) && (read_len != 0) )
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Doesn't read full lentgth, readlen=%d", read_len);
      }
      if(bytes_to_read == 0)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Drainout bytes are zero");
        channel->msg_state =OF_CHN_WAITING_FOR_NEW_MSG;
        channel->drain_out_bytes = 0; 
      }
    }
  }
  return status;
}


int32_t of_flow_stats_frame_respone(char *msg, struct ofi_flow_entry_info  *flow_stats_p, uint32_t remaining_length)
{

  struct ofi_instruction *instruction_entry_p;
  struct ofi_action *action_entry_p;
  struct ofi_match_field *match_entry_p; 
  struct ofp_match *match_field_p;
  
  uint32_t actions_len=0;
  uint32_t matchfield_len=0;
  uint32_t matchfield_total_len=0;
  uint32_t matchfield_padding_len=0;
  uint32_t length=0;
  int32_t return_value;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"flow stats frame response remaining_length %d \r\n", remaining_length);

  /*Read match field from the msg*/
  match_field_p = (struct ofp_match *)msg;

  MATCHFILED_PADDING(ntohs(match_field_p->length), matchfield_total_len);
  matchfield_len = ntohs(match_field_p->length);
  matchfield_padding_len = matchfield_total_len - matchfield_len;
    
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"match_len=%d padding_len=%d", matchfield_len, matchfield_padding_len);

  msg += 4; /*Moving till OXM value*/
  matchfield_len -=4;

  while(matchfield_len > 0)
  {
    length=0;
    return_value = of_frame_match_field_response(msg, &match_entry_p, &length);
    if (return_value != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"frame match field response failed");
    }
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"each match field length=%d", length);
    msg += length;
    matchfield_len -= length;
    OF_APPEND_NODE_TO_LIST(flow_stats_p->match_field_list, match_entry_p, OF_MATCH_LISTNODE_OFFSET);
  }
  
  msg += matchfield_padding_len;
  remaining_length -= matchfield_total_len;

  /*Now read the instruction, which has the action init */

  while (remaining_length > 0)
  {
    length=0;
    return_value = of_frame_instruction_response(msg, &instruction_entry_p, &length);
    if (return_value != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"frame instruction response failed");
    }
    msg += length;
    remaining_length -= length;

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," instruction length %d remaining_length %d \r\n", length,remaining_length);

    if(instruction_entry_p->type == OFPIT_APPLY_ACTIONS)
    {
      actions_len= instruction_entry_p->len - sizeof(struct ofp_instruction_actions);

      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," actions_len %d remaining_length %d \r\n", actions_len,remaining_length);

      while (actions_len > 0 )
      {
        length=0;
        return_value=of_group_frame_action_response(msg, &action_entry_p, &length);
        if (return_value != OF_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"frame action response failed");
        }
        OF_APPEND_NODE_TO_LIST(instruction_entry_p->action_list, action_entry_p,OF_ACTION_LISTNODE_OFFSET);

        actions_len -= length;  
        remaining_length -= length;
        msg += length;
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"actions_len=%d remaining_length=%d \r\n", actions_len,remaining_length)
      }
    }
    else
    {
      /* updating the details for other actions as part of
      *  of_frame_instruction_response */
    }
    OF_APPEND_NODE_TO_LIST(flow_stats_p->instruction_list, instruction_entry_p,OF_INSTRUCTION_LISTNODE_OFFSET);
  }
  return OF_SUCCESS;
}

/*Vinod*/
int32_t of_bind_stats_frame_respone(char *msg, struct ofi_bind_entry_info  *bind_stats_p, uint32_t remaining_length)
{

  struct ofi_instruction   *instruction_entry_p;
  struct ofi_action        *action_entry_p;
//  struct ofi_match_field   *match_entry_p; 
//  struct ofp_match         *match_field_p;

  uint32_t actions_len=0;
//  uint32_t matchfield_len=0;
//  uint32_t matchfield_total_len=0;
//  uint32_t matchfield_padding_len=0;
  uint32_t length=0;
  int32_t return_value;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"bind stats frame response remaining_length %d \r\n", remaining_length);

  /*Read match field from the msg*/
#if 0
  match_field_p = (struct ofp_match *)msg;

  MATCHFILED_PADDING(ntohs(match_field_p->length), matchfield_total_len);
  matchfield_len = ntohs(match_field_p->length);
  matchfield_padding_len = matchfield_total_len - matchfield_len;
    
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"match_len=%d padding_len=%d", matchfield_len, matchfield_padding_len);

  msg += 4; /*Moving till OXM value*/
  matchfield_len -=4;

  while(matchfield_len > 0)
  {
    length=0;
     return_value = of_frame_match_field_response(msg, &match_entry_p, &length);
    if (return_value != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"frame match field response failed");
    }
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"each match field length=%d", length);
    msg += length;
    matchfield_len -= length;
    OF_APPEND_NODE_TO_LIST(flow_stats_p->match_field_list, match_entry_p, OF_MATCH_LISTNODE_OFFSET);
  }
  
  msg += matchfield_padding_len;
  remaining_length -= matchfield_total_len;
#endif
  /*Now read the instruction, which has the action init */

  while (remaining_length > 0)
  {
    length=0;
    return_value = of_frame_instruction_response(msg, &instruction_entry_p, &length);
    if (return_value != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"frame instruction response failed");
    }
    msg += length;
    remaining_length -= length;

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," instruction length %d remaining_length %d \r\n", length,remaining_length);

    if(instruction_entry_p->type == OFPIT_APPLY_ACTIONS)
    {
      actions_len= instruction_entry_p->len - sizeof(struct ofp_instruction_actions);

      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," actions_len %d remaining_length %d \r\n", actions_len,remaining_length);
     while (actions_len > 0 )
      {
        length=0;
        return_value=of_group_frame_action_response(msg, &action_entry_p, &length);
        if (return_value != OF_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"frame action response failed");
        }
        OF_APPEND_NODE_TO_LIST(instruction_entry_p->action_list, action_entry_p,OF_ACTION_LISTNODE_OFFSET);

        actions_len -= length;
        remaining_length -= length;
        msg += length;
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"actions_len=%d remaining_length=%d \r\n", actions_len,remaining_length)
      }
    }
    else
    {
      /* updating the details for other actions as part of
      *  of_frame_instruction_response */
    }
    OF_APPEND_NODE_TO_LIST(bind_stats_p->instruction_list, instruction_entry_p,OF_INSTRUCTION_LISTNODE_OFFSET);
  }
  return OF_SUCCESS;
}

int32_t of_frame_match_field_response(char *msg, struct ofi_match_field **match_field_entry_pp, uint32_t *length)
{
  struct ofi_match_field *match_entry_p; 
  struct oxm_match_field *match_read_p;
  uint8_t is_mask_set;
  uint16_t class;

  match_read_p  =(struct oxm_match_field *)msg;
  match_entry_p =(struct ofi_match_field *)calloc(1,sizeof(struct ofi_match_field));
  match_entry_p->match_class = ntohs(match_read_p->match_class);
  match_entry_p->field_type = match_read_p->field_type >>1;
  match_entry_p->field_len = match_read_p->length;

  OF_LOG_MSG(OF_LOG_MOD,OF_LOG_DEBUG,"field len value  is %d class %d",match_entry_p->field_len,class);

  is_mask_set = (match_read_p->field_type & (0x01))?1:0;

  OF_LOG_MSG(OF_LOG_MOD,OF_LOG_DEBUG,"maskset is %d ",is_mask_set);

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"match field type %d Length %d \r\n", match_entry_p->field_type, match_entry_p->field_len);

if(match_entry_p->match_class == OFPXMC_OPENFLOW_BASIC)
{
  switch (match_entry_p->field_type)
  {
    case OFPXMT_OFB_IN_PORT: /* Switch input port. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," OFPXMT_OFB_IN_PORT:");
        match_entry_p->ui32_data = ntohl(*(uint32_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "INPORT=%d", match_entry_p->ui32_data);
        break;
      }
    case OFPXMT_OFB_IN_PHY_PORT: /* Switch physical input port. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IN_PHY_PORT:");
        match_entry_p->ui32_data = ntohl(*(uint32_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "In-PhyPort=%d", match_entry_p->ui32_data);
        break;
      }
    case OFPXMT_OFB_METADATA: /* Metadata passed between tables. */			  
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_METADATA:");
        match_entry_p->ui64_data = ntohll(*(uint64_t *)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Metadata=%llx",match_entry_p->ui64_data);
        break;
      }
    case OFPXMT_OFB_ETH_DST: /* Ethernet destination address. */ 		   
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ETH_DST:");
        memcpy(match_entry_p->ui8_data_array, (msg + OF_OXM_HDR_LEN), 6);
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Eth_Dest =%lx", match_entry_p->ui8_data_array);
        break;
      }
    case OFPXMT_OFB_ETH_SRC: /* Ethernet source address. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ETH_SRC: ");
        memcpy(match_entry_p->ui8_data_array, (msg + OF_OXM_HDR_LEN), 6);
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Eth_Src =%lx", match_entry_p->ui8_data_array);
        break;
      }
    case OFPXMT_OFB_ETH_TYPE: /* Ethernet frame type. */ 		   
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ETH_TYPE: ");
        match_entry_p->ui16_data = ntohs(*(uint16_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Eth type=%x", match_entry_p->ui16_data);
        break;
      }
    case OFPXMT_OFB_VLAN_VID: /* VLAN id. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_VLAN_VID: ");
        match_entry_p->ui16_data = ntohs(*(uint16_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "valn id=%d", match_entry_p->ui16_data);
        break;
      }
    case OFPXMT_OFB_IP_PROTO: /* IP protocol. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IP_PROTO: ");
        match_entry_p->ui8_data = (*(uint8_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "IP Protocol=%d",match_entry_p->ui8_data);
        break;
      }
    case OFPXMT_OFB_IPV4_SRC: /* IPv4 source address. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV4_SRC: ");
        match_entry_p->ui32_data = ntohl(*(uint32_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "IPv4 src IP=%x",match_entry_p->ui32_data);
        break;
      }
    case OFPXMT_OFB_IPV4_DST: /* IPv4 destination address. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV4_DST: ");
        match_entry_p->ui32_data = ntohl(*(uint32_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "IPv4 Dest IP=%x",match_entry_p->ui32_data);
        break;
      }
    case OFPXMT_OFB_TCP_SRC: /* TCP source port. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_TCP_SRC: ");
        match_entry_p->ui16_data = ntohs(*(uint16_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "tcp src port=%d", match_entry_p->ui16_data);
        break;
      }
    case OFPXMT_OFB_TCP_DST: /* TCP destination port. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_TCP_DST: ");
        match_entry_p->ui16_data = ntohs(*(uint16_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "tcp dest port=%d", match_entry_p->ui16_data);
        break;
      }
    case OFPXMT_OFB_UDP_SRC: /* UDP source port. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_UDP_SRC: ");
        match_entry_p->ui16_data = ntohs(*(uint16_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "udp src port=%d", match_entry_p->ui16_data);
        break;
      }
    case OFPXMT_OFB_UDP_DST: /* UDP destination port. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_UDP_DST: ");
        match_entry_p->ui16_data = ntohs(*(uint16_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "udp dest port=%d", match_entry_p->ui16_data);
        break;
      }
    case OFPXMT_OFB_SCTP_SRC: /* SCTP source port. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_SCTP_SRC: ");
        match_entry_p->ui16_data = ntohs(*(uint16_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "sctp src port=%d", match_entry_p->ui16_data);
        break;
      }
    case OFPXMT_OFB_SCTP_DST: /* SCTP destination port. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_SCTP_DST: ");
        match_entry_p->ui16_data = ntohs(*(uint16_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "sctp dest port=%d", match_entry_p->ui16_data);
        break;
      }
    case OFPXMT_OFB_ICMPV4_TYPE: /* ICMP type. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV4_TYPE:");
        match_entry_p->ui8_data = (*(uint8_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "ICMP type=%d",match_entry_p->ui8_data);
        break;
      }
    case OFPXMT_OFB_ICMPV4_CODE: /* ICMP code. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV4_CODE: ");
        match_entry_p->ui8_data = (*(uint8_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "ICMP code=%d",match_entry_p->ui8_data);
        break;
      }
    case OFPXMT_OFB_ARP_OP: /* ARP opcode. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_OP: ");
        match_entry_p->ui16_data = ntohs(*(uint16_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "arp op code =%d", match_entry_p->ui16_data);
        break;
      }
    case OFPXMT_OFB_MPLS_LABEL: /* MPLS label. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_MPLS_LABEL:");
        match_entry_p->ui32_data = ntohl(*(uint32_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "MPLS Label=%d",match_entry_p->ui32_data);
        break;
      }
    case OFPXMT_OFB_MPLS_TC: /* MPLS TC. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_MPLS_TC:");
        match_entry_p->ui8_data = (*(uint8_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "MPLS TC=%d",match_entry_p->ui8_data);
        break;
      }
    case OFPXMT_OFB_MPLS_BOS: /* MPLS BoS bit. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_MPLS_BOS:");
        match_entry_p->ui8_data = (*(uint8_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "MPLS BOS=%d",match_entry_p->ui8_data);
        break;
      }
    case OFPXMT_OFB_PBB_ISID: /* PBB I-SID. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_PBB_ISID:");
        match_entry_p->ui8_data = (*(uint8_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "PBB ISID=%d",match_entry_p->ui8_data);
        break;
      }
    case OFPXMT_OFB_TUNNEL_ID: /* Logical Port Metadata. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_TUNNEL_ID:");
        match_entry_p->ui64_data = ntohll(*(uint64_t *)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Tunnel ID=%llx",match_entry_p->ui64_data);
        break;
      }
    case OFPXMT_OFB_VLAN_PCP:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_VLAN_PCP");
        match_entry_p->ui8_data = (*(uint8_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "VLAN PCP=%d",match_entry_p->ui8_data);
        break;
      }
    case OFPXMT_OFB_ARP_SPA: /* IPv4 source address. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_SPA: ");
        match_entry_p->ui32_data = ntohl(*(uint32_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "ARP SIP=%x",match_entry_p->ui32_data);
        break;
      }
    case OFPXMT_OFB_ARP_TPA: /* IPv4 source address. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_TPA: ");
        match_entry_p->ui32_data = ntohl(*(uint32_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "ARP DIP=%x",match_entry_p->ui32_data);
        break;
      }
    case OFPXMT_OFB_ARP_SHA:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_ARP_SHA: ");
        memcpy(match_entry_p->ui8_data_array, (msg + OF_OXM_HDR_LEN), 6);
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "ARP SRC MAC =%lx", match_entry_p->ui8_data_array);
        break;
      }	
    case OFPXMT_OFB_ARP_THA:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_ARP_THA: ");
        memcpy(match_entry_p->ui8_data_array, (msg + OF_OXM_HDR_LEN), 6);
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "ARP DST MAC =%lx", match_entry_p->ui8_data_array);
        break;
      }
    case OFPXMT_OFB_IP_DSCP:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IP_DSCP: ");
        match_entry_p->ui8_data = (*(uint8_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "IP DSCP=%d",match_entry_p->ui8_data);
        break;
      }
    case OFPXMT_OFB_IP_ECN:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IP_ECN: ");
        match_entry_p->ui8_data = (*(uint8_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "IP ECN=%d",match_entry_p->ui8_data);
        break;
      }
    case OFPXMT_OFB_IPV6_SRC:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_SRC: ");
        break;
      }
    case OFPXMT_OFB_IPV6_DST:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_DST: ");
        break;
      }
    case OFPXMT_OFB_ICMPV6_TYPE: /* ICMP type. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV6_TYPE:");
        match_entry_p->ui8_data = (*(uint8_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "ICMPV6 Type=%d",match_entry_p->ui8_data);
        break;
      }
    case OFPXMT_OFB_ICMPV6_CODE: /* ICMP code. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV6_CODE: ");
        match_entry_p->ui8_data = (*(uint8_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "ICMPV6 Code=%d",match_entry_p->ui8_data);
        break;
      }
    case OFPXMT_OFB_IPV6_FLABEL:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_FLABEL");
        match_entry_p->ui32_data = ntohl(*(uint32_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "IPV6 FLABEL=%x",match_entry_p->ui32_data);
        break;
      }
    case OFPXMT_OFB_IPV6_ND_TARGET:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_ND_TARGET: ");
        break;
      }
    case OFPXMT_OFB_IPV6_ND_SLL: /* The source link-layer address option in an IPv6Neighbor Discovery message.*/
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_ND_SLL:");
         break;
      }
    case OFPXMT_OFB_IPV6_ND_TLL: /* The target link-layer address option in an IPv6Neighbor Discovery message.*/
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," OFPXMT_OFB_IPV6_ND_TLL: ");
        break;
      }
    case OFPXMT_OFB_IPV6_EXTHDR:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_EXTHDR: ");
        break;
      }
    default :
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Invalid Match Field Type");
        break;
      }
  }
}
else if(match_entry_p->match_class == OFPXMC_EXPERIMENTER)
{
                        struct ofp_oxm_experimenter_header *exp_class_oxm = (struct ofp_oxm_experimenter_header *)msg;
                        match_entry_p->ui32_data = exp_class_oxm->experimenter;
		OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMC_EXPERIMENTER ");

}
else if(match_entry_p->match_class == OFPXMC_NXM_1)
{
		match_entry_p->ui64_data = (*(uint64_t *)((char *)msg + 4));
		OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMC_NXM_1  data %llX",match_entry_p->ui64_data);
}
else
{
	OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "UNKNOWN_CLASS ");
}


  if(is_mask_set){
        memcpy(match_entry_p->mask, (msg + OF_OXM_HDR_LEN + ((match_entry_p->field_len)/2)), match_entry_p->field_len/2);
        match_entry_p->is_mask_set = 1;
        match_entry_p->mask_len = match_entry_p->field_len/2;
        OF_LOG_MSG(OF_LOG_MOD,OF_LOG_DEBUG,"maskset is %d  value  is %llX,len is %d ntos val %llX",match_entry_p->is_mask_set,match_entry_p->mask,match_entry_p->mask_len,ntohl((msg + OF_OXM_HDR_LEN + ((match_entry_p->field_len)/2))));
  }

  *length += match_entry_p->field_len+sizeof(struct oxm_match_field);
  *match_field_entry_pp=match_entry_p;
  return OF_SUCCESS;

}

int32_t of_frame_instruction_response(char *msg, struct ofi_instruction **instruction_entry_pp, uint32_t *length)
{
  struct ofi_instruction *instruction_entry_p;
  struct ofp_instruction *instruction_read_p;

  instruction_read_p  =(struct ofp_instruction *)msg;
  instruction_entry_p =(struct ofi_instruction *)calloc(1,sizeof(struct ofi_instruction));
  instruction_entry_p->type=ntohs(instruction_read_p->type);
  instruction_entry_p->len=ntohs(instruction_read_p->len);

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"instruction type and len %d  %d \r\n", instruction_entry_p->type, instruction_entry_p->len);
  switch(instruction_entry_p->type)
  {
    case OFPIT_GOTO_TABLE:
      {
        struct ofp_instruction_goto_table *instruction_gototable_read_p;
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPIT_GOTO_TABLE:");
        instruction_gototable_read_p = (struct ofp_instruction_goto_table *)msg;
        instruction_entry_p->table_id = instruction_gototable_read_p->table_id;
        *length +=  sizeof(struct ofp_instruction_goto_table);
        break;
      }

    case OFPIT_WRITE_METADATA: 
      {
        struct ofp_instruction_write_metadata *instruction_write_metadata_read_p;
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPIT_WRITE_METADATA:");
        instruction_write_metadata_read_p = (struct ofp_instruction_write_metadata *)msg;
        instruction_entry_p->metadata = ntohll(instruction_write_metadata_read_p->metadata);
        instruction_entry_p->metadata_mask = ntohll(instruction_write_metadata_read_p->metadata_mask);
        *length +=  sizeof(struct ofp_instruction_write_metadata);
        break;
      }

    case OFPIT_WRITE_ACTIONS:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPIT_WRITE_ACTIONS:");
        *length +=  instruction_entry_p->len;
        break;
      }

    case OFPIT_APPLY_ACTIONS:
      {
        struct ofp_instruction_actions *instruction_apply_action_read_p;
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPIT_APPLY_ACTIONS:");
        OF_LIST_INIT(instruction_entry_p->action_list, NULL);
        instruction_apply_action_read_p = (struct ofp_instruction_actions *)msg;
        *length +=  sizeof(struct ofp_instruction_actions);
        break;
      }

    case OFPIT_CLEAR_ACTIONS:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPIT_CLEAR_ACTIONS:");
        *length +=  instruction_entry_p->len;
        break;
      }

    case OFPIT_METER:
      {
        //struct ofp_instruction_meter *instruction_meter_readp;
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPIT_METER:");
        *length +=  sizeof(struct ofp_instruction_meter);
        break;
      }

    case OFPIT_EXPERIMENTER:
      {
       /*
        struct ofp_instruction_experimenter *instruction_experimenter;
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPIT_EXPERIMENTER:");
        instruction_experimenter = (struct ofp_instruction_experimenter *)msg;
        instruction_entry_p->vendorid = ntohl(instruction_experimenter->experimenter);
        instruction_entry_p->subtype  = ntohs(*(uint16_t*)(msg + 8));
        *length +=  instruction_entry_p->len;
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"vendorid=%x subtype=%d instruction_entry_p->len=%d", instruction_entry_p->vendorid,instruction_entry_p->subtype,instruction_entry_p->len);
        break;*/

        struct fslx_action_header *fslx_inst =
                            (struct fslx_action_header *)((char *)msg);
         instruction_entry_p->subtype = fslx_inst->subtype;
         instruction_entry_p->vendorid = fslx_inst->vendor;
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," instruction_entry_p->subtype %d vendor id %x",instruction_entry_p->subtype,instruction_entry_p->vendorid);
           switch(instruction_entry_p->subtype)
          {
                  case FSLXIT_APPLY_ACTIONS_ON_CREATE:
                        break;
                  case FSLXIT_APPLY_ACTIONS_ON_DELETE:
                        break;
                  case FSLXIT_WAIT_TO_COMPLETE_PKT_OUTS:
                        {
                                struct fslxit_wait_to_cmplete_cntlr_pkts *fslxit = (struct fslxit_wait_to_cmplete_cntlr_pkts *)((char *)msg);
                                instruction_entry_p->value1 = fslxit->state;
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," FSLXIT_WAIT_TO_COMPLETE_PKT_OUTS state %d should be 1 or 2",instruction_entry_p->value1);
                        }
                        break;
                  case FSLXIT_WRITE_PRIORITY_REGISTER:
                        {
                            struct fslxit_write_prio_register *fslxit = (struct fslxit_write_prio_register *)((char *)msg);
                                instruction_entry_p->value1 = fslxit->priority_value;
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," FSLXIT_WRITE_PRIORITY_REGISTER priority_value %d ",instruction_entry_p->value1);

                        }
                        break;
                  case FSLXIT_WRITE_PRIORITY_REG_FROM_CURRENT_FLOW:
                        break;
                  case FSLXIT_RE_LOOKUP:
                        {
                                 struct fslxit_re_lookup *fslxit = (struct fslxit_re_lookup *)((char *)msg);
                                instruction_entry_p->value1 = fslxit->prio_reg_type;
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," FSLXIT_RE_LOOKUP prio_reg_type %d ",instruction_entry_p->value1);
                        }
                        break;
                  case FSLXIT_RE_INJECT_TO:
                        {
                                struct fslxit_re_inject_to *fslxit = (struct fslxit_re_inject_to *)((char *)msg);
                                instruction_entry_p->value1 = fslxit->prio_reg_type;
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," FSLXIT_RE_LOOKUP prio_reg_type %d ",instruction_entry_p->value1);
                                instruction_entry_p->table_id = fslxit->table_id;
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," FSLXIT_RE_LOOKUP table_id %d ",instruction_entry_p->table_id);
                        }
                        break;
 case FSLXIT_APPLY_GROUP:
                        {

                                 struct fslx_instruction_group *group;
                                group = (struct fslx_instruction_group *)((char *)msg);
                                  instruction_entry_p->group_id = group->group_id;
                                OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"instr FSLXIT_APPLY_GROUP group->group_id %d\n",group->group_id);

                        }
                        break;
                  case FSLXIT_WRITE_GROUP:
                        {
                                 struct fslx_instruction_group *group;
                                group = (struct fslx_instruction_group *)((char *)msg);
                                  instruction_entry_p->group_id = group->group_id;
                                OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"instr FSLXIT_WRITE_GROUP group->group_id %d\n",group->group_id);
                        }
                        break;
                  case FSLXIT_WRITE_PRIORITY_REG_FROM_NEXT_FLOW:
                        break;
                  default:
                        break;

          }//switch

         *length +=  instruction_entry_p->len;
        break;
      }

    default:
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"unknown action %d", instruction_entry_p->type);
  }

  *instruction_entry_pp=instruction_entry_p;
  return OF_SUCCESS;

}

#if 1
static inline int32_t UpdateTableFlowEnryInstProp(struct ofi_table_features_info *pAppTbl, 
    uint16_t                      length,
    struct ofp_table_feature_prop_instructions *pTblInsProp)
{
  uint8_t                inst_no, no_of_tables;
  struct ofp_instruction *pInstrctnProp;
  pAppTbl->no_of_flow_inst= no_of_tables = OF_INST_PROP_NO_OF_DESC(length); 
  if(pAppTbl->no_of_flow_inst > 0)
  {
    pAppTbl->flow_instructions= (uint16_t*) of_mem_calloc( no_of_tables,
        sizeof(uint16_t) );
    if(pAppTbl->flow_instructions == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Memory Allocation Failed - Flow Tbl feature instn\n",__FUNCTION__,__LINE__);
      return OF_FAILURE;
    }
    pInstrctnProp = (struct ofp_instruction *) (pTblInsProp->instruction_ids);
    for(inst_no = 0; inst_no < no_of_tables; inst_no++)
    {
      ((uint16_t*)(pAppTbl->flow_instructions))[inst_no] = ntohs((pInstrctnProp + inst_no)->type);
    }
  }
  return OF_SUCCESS;
}

  static inline int32_t
UpdateTableMissEnryInstProp(struct ofi_table_features_info *pAppTbl,
    uint16_t                                    length,
    struct ofp_table_feature_prop_instructions *pTblInsProp)
{
  uint8_t                inst_no;
  struct ofp_instruction *pInstrctnProp;

  pAppTbl->no_of_miss_inst= OF_INST_PROP_NO_OF_DESC(length);  
  if(pAppTbl->no_of_miss_inst > 0)
  {
    pAppTbl->miss_entry_instructions= (uint16_t*) of_mem_calloc( pAppTbl->no_of_miss_inst,
        sizeof(uint16_t) );
    if(pAppTbl->miss_entry_instructions == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Memory Allocation Failed - Flow Tbl feature instn for miss entry\n",__FUNCTION__,__LINE__);
      return OF_FAILURE;
    }
    pInstrctnProp = (struct ofp_instruction *) (pTblInsProp->instruction_ids);
    for(inst_no = 0; inst_no < pAppTbl->no_of_miss_inst; inst_no++)
      ((uint16_t*)(pAppTbl->miss_entry_instructions))[inst_no] = ntohs((pInstrctnProp + inst_no)->type);
  }
  return OF_SUCCESS;
}

  static inline int32_t
UpdateNextTableProp4FlowEntry(struct ofi_table_features_info *pAppTbl,
    uint16_t                                    length,
    struct ofp_table_feature_prop_next_tables  *pNextTblProp)
{
  uint8_t next_tbl_no;

  pAppTbl->no_of_next_tbl= OF_NEXT_TBL_PROP_NO_OF_DESC(length);  
  if(pAppTbl->no_of_next_tbl > 0)
  {
    pAppTbl->next_tbl= (uint8_t*) of_mem_calloc( pAppTbl->no_of_next_tbl,
        sizeof(uint8_t) );
    if(pAppTbl->next_tbl == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Memory Allocation Failed - Next Tbl 4 Entry feature instn\n",__FUNCTION__,__LINE__);
      return OF_FAILURE;
    }

    for(next_tbl_no = 0; next_tbl_no < pAppTbl->no_of_next_tbl; next_tbl_no++)
      ((uint8_t*)pAppTbl->next_tbl)[next_tbl_no] = pNextTblProp->next_table_ids[next_tbl_no];
  }
  return OF_SUCCESS;
}

  static inline int32_t
UpdateNextTableProp4MissEntry(struct ofi_table_features_info *pAppTbl,
    uint16_t              length,
    struct ofp_table_feature_prop_next_tables  *pNextTblProp)
{
  uint8_t next_tbl_no;

  pAppTbl->no_of_next_tbls_4_miss= OF_NEXT_TBL_PROP_NO_OF_DESC(length);
  if(pAppTbl->no_of_next_tbls_4_miss > 0)
  {
    pAppTbl->next_tbl_4_flow_miss = (uint8_t*) of_mem_calloc( pAppTbl->no_of_next_tbls_4_miss,
        sizeof(uint8_t) );
    if(pAppTbl->next_tbl_4_flow_miss == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Memory Allocation Failed - Next Tbl 4 Entry feature instn\n",__FUNCTION__,__LINE__);
      return OF_FAILURE;
    }

    for(next_tbl_no = 0; next_tbl_no < pAppTbl->no_of_next_tbls_4_miss; next_tbl_no++)
      ((uint8_t*)pAppTbl->next_tbl_4_flow_miss)[next_tbl_no] = pNextTblProp->next_table_ids[next_tbl_no];
  }
  return OF_SUCCESS;
}

  static inline int32_t
UpdateTableFlowEnryWriteActnsProp(struct ofi_table_features_info *pAppTbl,
    uint16_t                               length,
    struct ofp_table_feature_prop_actions *pTblActnProp)
{
  uint8_t actn_no;

  pAppTbl->no_of_write_actns = OF_ACTN_PROP_NO_OF_DESC(length);
  if(pAppTbl->no_of_write_actns > 0)
  {
    pAppTbl->flow_entry_write_actns = (uint16_t*) of_mem_calloc( pAppTbl->no_of_write_actns,
        sizeof(uint16_t) );
    if(pAppTbl->flow_entry_write_actns== NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Memory Allocation Failed - Flow Tbl Write Actions \n",__FUNCTION__,__LINE__);
      return OF_FAILURE;
    }
    for(actn_no = 0; actn_no < pAppTbl->no_of_write_actns; actn_no++)
      ((uint16_t*)pAppTbl->flow_entry_write_actns)[actn_no] = ntohs((pTblActnProp->action_ids + actn_no)->type);
  }
  return OF_SUCCESS;
}

  static inline int32_t
UpdateTableMissEnryWriteActnsProp(struct ofi_table_features_info *pAppTbl,
    uint16_t                               length,
    struct ofp_table_feature_prop_actions *pTblActnProp)
{
  uint8_t                actn_no;

  pAppTbl->no_of_write_actns_4_miss = OF_ACTN_PROP_NO_OF_DESC(length);
  if(pAppTbl->no_of_write_actns_4_miss > 0)
  {
    pAppTbl->miss_entry_write_actns = (uint16_t*) of_mem_calloc( pAppTbl->no_of_write_actns_4_miss,
        sizeof(uint16_t) );
    if(pAppTbl->miss_entry_write_actns == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Memory Allocation Failed - Flow Tbl Write actions  miss \r\n",__FUNCTION__,__LINE__);
      return OF_FAILURE;
    }

    for(actn_no = 0; actn_no < pAppTbl->no_of_write_actns_4_miss; actn_no++)
      ((uint16_t*)pAppTbl->miss_entry_write_actns)[actn_no] = ntohs((pTblActnProp->action_ids + actn_no)->type);
  }
  return OF_SUCCESS;
}
  static inline int32_t
UpdateTableFlowEnryApplyActnsProp(struct ofi_table_features_info *pAppTbl,
    uint16_t                               length,
    struct ofp_table_feature_prop_actions *pTblActnProp)
{
  uint8_t                actn_no;

  pAppTbl->no_of_apply_actns = OF_ACTN_PROP_NO_OF_DESC(length);
  if(pAppTbl->no_of_apply_actns > 0)
  {
    pAppTbl->flow_entry_apply_actns = (uint16_t*) of_mem_calloc( pAppTbl->no_of_apply_actns,
        sizeof(uint16_t) );
    if(pAppTbl->flow_entry_apply_actns == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Memory Allocation Failed - Flow Tbl apply action \r\n",__FUNCTION__,__LINE__);
      return OF_FAILURE;
    }

    for(actn_no = 0; actn_no < pAppTbl->no_of_apply_actns; actn_no++)
      ((uint16_t*)pAppTbl->flow_entry_apply_actns)[actn_no] = ntohs((pTblActnProp->action_ids + actn_no)->type);
  }
  return OF_SUCCESS;
}

  static inline int32_t
UpdateTableMissEnryApplyActnsProp(struct ofi_table_features_info *pAppTbl,
    uint16_t                               length,
    struct ofp_table_feature_prop_actions *pTblActnProp)
{
  uint8_t                actn_no;

  pAppTbl->no_of_apply_actns_4_miss = OF_ACTN_PROP_NO_OF_DESC(length);
  if(pAppTbl->no_of_apply_actns_4_miss > 0)
  {
    pAppTbl->miss_entry_apply_actns = (uint16_t*) of_mem_calloc( pAppTbl->no_of_apply_actns_4_miss,
        sizeof(uint16_t) );
    if(pAppTbl->miss_entry_apply_actns== NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Memory Allocation Failed - Flow Tbl applly action miss \r\n",__FUNCTION__,__LINE__);
      return OF_FAILURE;
    }

    for(actn_no = 0; actn_no < pAppTbl->no_of_apply_actns_4_miss; actn_no++)
      ((uint16_t*)pAppTbl->miss_entry_apply_actns)[actn_no] = ntohs((pTblActnProp->action_ids + actn_no)->type);
  }
  return OF_SUCCESS;
}

  static inline int32_t
UpdateTableMatchFiledProp(struct ofi_table_features_info *pAppTbl,
    uint16_t                               length,
    struct ofp_table_feature_prop_oxm     *pMatchFieldProp)
{
  return OF_SUCCESS;
#if 0
  uint8_t                    oxmid_no;
  of_DP_Table_Matchfields_t *pMatchField;
  of_FieldInfo_t            *pAppMatchFields;
  uint32_t                   OXMTLVBufLen = 0;
  uint16_t                   Size;


  pAppTbl->no_of_match_fields = OF_OXM_IDS_NO_OF_DESC(length);
  if(pAppTbl->no_of_match_fields > 0)
  {
    pAppTbl->match_field_info=(uint32_t*) of_mem_calloc(pAppTbl->no_of_match_fields,
        sizeof(uint32_t) );
    //pAppTbl->match_field_info=(of_FieldInfo_t*) of_mem_calloc(pAppTbl->no_of_match_fields,
    //                                                                      sizeof(of_MatchFields_t) );Deepthi
    if(pAppTbl->match_field_info == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Memory Allocation Failed - match fileds \r\n",__FUNCTION__,__LINE__);
      return OF_FAILURE;
    }
    for(oxmid_no= 0; oxmid_no< pAppTbl->no_of_match_fields; oxmid_no++)
    {
      ((uint32_t*)pAppTbl->match_field_info)[oxmid_no] = ntohl(pMatchFieldProp->oxm_ids+oxmid_no);
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d match fileds table %d, property: %d\r\n",__FUNCTION__,__LINE__, oxmid_no, ntohl(pMatchFieldProp->oxm_ids+oxmid_no) );
    }
  }

  return OF_SUCCESS;
  pAppTbl->Matchfields = (of_DP_Table_Matchfields_t*) of_mem_calloc( pAppTbl->no_of_match_fields,
      sizeof(of_DP_Table_Matchfields_t) );
  if(pAppTbl->Matchfields == NULL)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Memory Allocation Failed - match fileds \r\n",__FUNCTION__,__LINE__);
    return OF_FAILURE;
  }

  Size = sizeof(of_FieldInfo_t)*(pAppTbl->NoOfMatchFields); 
  for(oxmid_no = 0; oxmid_no < pAppTbl->NoOfMatchFields; oxmid_no++)
  {
    pMatchField = (pAppTbl->Matchfields + oxmid_no);
    pMatchField->OXMHeader   = pMatchFieldProp->oxm_ids[oxmid_no];
    pMatchField->Class       = OXM_CLASS(pMatchField->OXMHeader);
    pMatchField->Field       = OXM_FIELD(pMatchField->OXMHeader);
    pMatchField->Length      = OXM_LENGTH(pMatchField->OXMHeader);
    pMatchField->HashMask    = OXM_HASMASK(pMatchField->OXMHeader);
    pMatchField->bWildcard   = FALSE;

    pAppMatchFields = (pAppTbl->Features.Matchfields.pFields + oxmid_no);
    pAppMatchFields->Type          = OXM_FIELD(pMatchField->OXMHeader);
    Size += pAppMatchFields->Length = OXM_LENGTH(pMatchField->OXMHeader);
    pAppMatchFields->bMask          = OXM_HASMASK(pMatchField->OXMHeader);
    if(pMatchField->HashMask)
      pAppMatchFields->Length /=2;
    OXMTLVBufLen += (4/*OXM TLV Header*/+pMatchField->Length/*Payload length*/);
  }

  //   pAppTbl->MatchfieldsBufLen = Size;

  //  return of_CreateTblMatchFieldMempools(pAppTbl,(OXMTLVBufLen+4/*Flow MatchHeader*/));
#endif
}

  static inline int32_t
UpdateTableWildCardProp(struct ofi_table_features_info *pAppTbl,
    uint16_t                               length,
    struct ofp_table_feature_prop_oxm     *pMatchFieldProp)
{
  return OF_SUCCESS;
  uint8_t  oxmid_no;

  pAppTbl->no_of_wildcard_fields = OF_OXM_IDS_NO_OF_DESC(length);
  if(pAppTbl->no_of_wildcard_fields > 0)
  {
    pAppTbl->wildcard_field_info = (uint32_t*) of_mem_calloc( pAppTbl->no_of_wildcard_fields, sizeof(uint32_t) );
    if(pAppTbl->wildcard_field_info== NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Memory Allocation Failed - wild card \r\n",__FUNCTION__,__LINE__);
      return OF_FAILURE;
    }

    for(oxmid_no = 0; oxmid_no < pAppTbl->no_of_wildcard_fields; oxmid_no++)
    {
      ((uint32_t*)pAppTbl->wildcard_field_info)[oxmid_no] = ntohl(pMatchFieldProp->oxm_ids[oxmid_no]);
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d wildcard field %d, property: %d\r\n",__FUNCTION__,__LINE__, oxmid_no, ntohl(pMatchFieldProp->oxm_ids+oxmid_no) );
    }
  }
  return OF_SUCCESS;
}

  static inline int32_t
UpdateTableWrSetfieldProp(struct ofi_table_features_info *pAppTbl,
    uint16_t                               length,
    struct ofp_table_feature_prop_oxm     *pMatchFieldProp)
{
  return OF_SUCCESS;
  uint8_t  oxmid_no;

  pAppTbl->no_of_wr_set_fields = OF_OXM_IDS_NO_OF_DESC(length);
  if(pAppTbl->no_of_wr_set_fields > 0)
  {
    pAppTbl->flow_wr_set_field_info = (uint32_t*) of_mem_calloc( pAppTbl->no_of_wr_set_fields,
        sizeof(uint32_t) );
    if(pAppTbl->flow_wr_set_field_info == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Memory Allocation Failed - write set field \r\n",__FUNCTION__,__LINE__);
      return OF_FAILURE;
    }

    for(oxmid_no = 0; oxmid_no < pAppTbl->no_of_wr_set_fields; oxmid_no++)
    {
      ((uint32_t*)pAppTbl->flow_wr_set_field_info)[oxmid_no] = ntohl(pMatchFieldProp->oxm_ids[oxmid_no]);
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d write set field %d, property: %d\r\n",__FUNCTION__,__LINE__, oxmid_no, ntohl(pMatchFieldProp->oxm_ids+oxmid_no) );
    }
  }
  return OF_SUCCESS;
}

  static inline int32_t
UpdateTableWrSetfieldMissProp(struct ofi_table_features_info *pAppTbl,
    uint16_t                               length,
    struct ofp_table_feature_prop_oxm     *pMatchFieldProp)
{
  return OF_SUCCESS;
  uint8_t  oxmid_no;

  pAppTbl->no_of_wr_set_fields_miss = OF_OXM_IDS_NO_OF_DESC(length);
  if(pAppTbl->no_of_wr_set_fields_miss > 0)
  {
    pAppTbl->miss_wr_entry_set_field_info = (uint32_t*) of_mem_calloc( pAppTbl->no_of_wr_set_fields_miss,
        sizeof(uint32_t) );
    if(pAppTbl->miss_wr_entry_set_field_info == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Memory Allocation Failed - write set field miss entry \r\n",__FUNCTION__,__LINE__);
      return OF_FAILURE;
    }

    for(oxmid_no = 0; oxmid_no < pAppTbl->no_of_wr_set_fields_miss; oxmid_no++)
    {
      ((uint32_t*)pAppTbl->miss_wr_entry_set_field_info)[oxmid_no] = ntohl(pMatchFieldProp->oxm_ids[oxmid_no]);
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d MISS write set field %d, property: %d\r\n",__FUNCTION__,__LINE__, oxmid_no, ntohl(pMatchFieldProp->oxm_ids+oxmid_no) );
    }
  }
  return OF_SUCCESS;
}

  static inline int32_t
UpdateTableApplySetfieldProp(struct ofi_table_features_info *pAppTbl,
    uint16_t                               length,
    struct ofp_table_feature_prop_oxm     *pMatchFieldProp)
{
  return OF_SUCCESS;
  uint8_t  oxmid_no;

  pAppTbl->no_of_ap_set_fields = OF_OXM_IDS_NO_OF_DESC(length);
  if(pAppTbl->no_of_ap_set_fields  > 0)
  {
    pAppTbl->flow_ap_set_field_info  = (uint32_t*) of_mem_calloc( pAppTbl->no_of_ap_set_fields,
        sizeof(uint32_t) );
    if(pAppTbl->flow_ap_set_field_info== NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Memory Allocation Failed - apply set feild \r\n",__FUNCTION__,__LINE__);
      return OF_FAILURE;
    }

    for(oxmid_no = 0; oxmid_no < pAppTbl->no_of_ap_set_fields; oxmid_no++)
    {
      ((uint32_t*)pAppTbl->flow_ap_set_field_info)[oxmid_no] = ntohl(pMatchFieldProp->oxm_ids[oxmid_no]);
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d flow set field %d, property: %d\r\n",__FUNCTION__,__LINE__, oxmid_no, ntohl(pMatchFieldProp->oxm_ids+oxmid_no) );
    }
  }
  return OF_SUCCESS;
}

  static inline int32_t
UpdateTableApplySetfieldMissProp(struct ofi_table_features_info *pAppTbl,
    uint16_t                               length,
    struct ofp_table_feature_prop_oxm     *pMatchFieldProp)
{
  return OF_SUCCESS;
  uint8_t  oxmid_no;

  pAppTbl->no_of_ap_set_fields_miss = OF_OXM_IDS_NO_OF_DESC(length);
  if(pAppTbl->no_of_ap_set_fields_miss > 0)
  {
    pAppTbl->miss_ap_entry_set_field_info = (uint32_t*) of_mem_calloc( pAppTbl->no_of_ap_set_fields_miss,
        sizeof(uint32_t) );
    if(pAppTbl->miss_ap_entry_set_field_info == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Memory Allocation Failed - apply set field miss entry\r\n",__FUNCTION__,__LINE__);
      return OF_FAILURE;
    }

    for(oxmid_no = 0; oxmid_no < pAppTbl->no_of_ap_set_fields_miss; oxmid_no++)
    {
      ((uint32_t*)pAppTbl->miss_ap_entry_set_field_info)[oxmid_no] = ntohl(pMatchFieldProp->oxm_ids[oxmid_no]);
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d MISS flow set field %d, property: %d\r\n",__FUNCTION__,__LINE__, oxmid_no, ntohl(pMatchFieldProp->oxm_ids+oxmid_no) );
    }
  }
  return OF_SUCCESS;
}
#endif
