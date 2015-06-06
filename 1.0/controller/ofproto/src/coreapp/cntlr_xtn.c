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

/* File  : cntlr_xtn.c
* Author: Rajesh Madabushi <rajesh.madabushi@freescale.com>
* Date  : December, 2012  
* Description: 
*  This file contains transaction processing.
* History:
 */

/* INCLUDE_START ************************************************************/
#include "controller.h"
#include "of_utilities.h"
#include "cntrl_queue.h"
#include "of_msgapi.h"
#include "of_multipart.h"
#include "dprm.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "cntlr_event.h"
#include "dprmldef.h"
#include "cntlr_xtn.h"
/* INCLUDE_END **************************************************************/

uint32_t           cntlr_xtn_hash_table_init_value_g;
uint32_t           cntlr_xtn_seq_no_g = 0;
cntlr_xtn_table_t  *cntlr_xtn_table;
void CNTLR_PKTPROC_DATA  *cntlr_xtn_nodes_mempool_g;
struct tmr_saferef  cntlr_xtn_table_timer_saferef;

void cntlr_free_xtn_node_rcu(struct rcu_head *node);
void
cntlr_get_xtn_node_mempool_entries(uint32_t *max_entries_p,
    uint32_t  *max_static_entries_p);

void* cntlr_xtn_recrods_cleanup_func(void *cbk_arg1,
    void *arg2);

void* 
cntlr_xtn_recrod_cleanup_func(void *cbk_arg1,
    void *arg2);

void clean_multipart_req_msgs(cntlr_transactn_rec_t *p_xtn_rec);

extern void mchash_insert_node_in_middle(struct mchash_bucket* bucket_p,struct mchash_dll_node *prev_p,
    struct mchash_dll_node *node_p, struct mchash_dll_node* next_p);

  int32_t
cntlr_xtn_table_init()
{
  uint32_t  max_entries;
  uint32_t  max_static_entries;
  int32_t   retval = OF_SUCCESS;
  int32_t   status = OF_SUCCESS;
  struct mempool_params mempool_info={};

  do
  {
    cntlr_get_xtn_node_mempool_entries(&max_entries,&max_static_entries);

    mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;
    mempool_info.block_size = sizeof(cntlr_transactn_rec_t);
    mempool_info.max_blocks = max_entries;
    mempool_info.static_blocks = max_static_entries;
    mempool_info.threshold_min = max_static_entries/10;
    mempool_info.threshold_max = max_static_entries/3;
    mempool_info.replenish_count = max_static_entries/10;
    mempool_info.b_memset_not_required = FALSE;
    mempool_info.b_list_per_thread_required = FALSE;
    mempool_info.noof_blocks_to_move_to_thread_list = 0;

    retval  = mempool_create_pool("of_Xtn_Nodes_pool", &mempool_info,
		    &cntlr_xtn_nodes_mempool_g);
    if(retval != MEMPOOL_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Create mempool failure Transaction record, retval=%d ",
          retval);
      status = OF_FAILURE;
      break;
    }


    cntlr_xtn_hash_table_init_value_g = rand();
    retval = mchash_table_create(((max_entries / 5)+1),
        max_entries,
        cntlr_free_xtn_node_rcu,
        &cntlr_xtn_table);
    if(retval != MCHASHTBL_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in creating Switch Nodes Hash table  ");
      status = OF_FAILURE;
      break;
    }

    /*TBD, Start Timer to do cleanup of expired transaction*/
    cntlr_xtn_seq_no_g = 0;

#if 0
    retval = timer_create_and_start(NULL,
        &cntlr_xtn_table_timer_saferef,
        CNTLR_PEIRODIC_TIMER,
        CNTLR_XTN_TABLE_CLEAN_UP_TIME_PERIOD,
        cntlr_xtn_recrods_cleanup_func,
        NULL,NULL);
    if(retval == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error in creating transaction records cleanup  timer\r\n", __FUNCTION__,__LINE__);
      status = OF_FAILURE;
      break;
    }
#endif
  }
  while(0);

  if(status == OF_FAILURE)
  {
    if(cntlr_xtn_nodes_mempool_g != NULL)
      mempool_delete_pool(cntlr_xtn_nodes_mempool_g);

    if(cntlr_xtn_table != NULL)
      mchash_table_delete(cntlr_xtn_table);

    return OF_FAILURE;
  }

  return OF_SUCCESS;
}

  void
cntlr_get_xtn_node_mempool_entries(uint32_t *max_entries_p,
    uint32_t  *max_static_entries_p)
{
  if(max_entries_p)
    *max_entries_p = OF_TRANSACTION_TABLE_MAX_NO_OF_NODES;

  if(max_static_entries_p)
    *max_static_entries_p = OF_TRANSACTION_TABLE_STATIC_NODES;
}
  int32_t
cntlr_record_xtn_details(struct of_msg               *msg,
    struct dprm_datapath_entry  *datapath,
    void                        *callback_fn,
    void                        *app_arg1,
    void                        *app_arg2,
    cntlr_xtn_node_saferef_t    *p_xtn_rec_saferef)
{
  cntlr_transactn_rec_t    *transaction_rec;
  uint32_t                  hash;
  //of_data_path_id_t         dp_id;
  uint8_t                   success;
  uint8_t                   is_heap;
  int32_t   retval = OF_SUCCESS;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"Entry");
  if(mempool_get_mem_block(cntlr_xtn_nodes_mempool_g,
        (uint8_t **)&transaction_rec,
        (uint8_t *)&is_heap) != OF_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Xtn Record Entry Memory allocation failure");
    return OF_FAILURE;
  }
  transaction_rec->is_heap = is_heap;

  cntlr_xtn_seq_no_g++;
  if(cntlr_xtn_seq_no_g == 0)
    cntlr_xtn_seq_no_g = 1;

  transaction_rec->xid = cntlr_xtn_seq_no_g;
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"transaction_rec->xid = 0x%x");

  /* Stamp XID on the Openflow message */

  ((struct ofp_header*)(msg->desc.start_of_data))->xid = htonl(transaction_rec->xid);

  //OF_STORE_DATAPATH_ID(dp_id,dp_id_val);

  /*Record all transaction details*/
  //transaction_rec->msg          = msg;
  transaction_rec->msg          = NULL;
  transaction_rec->dp_handle    = datapath->datapath_handle;
  transaction_rec->msg_type     = ((struct ofp_header*)(msg->desc.start_of_data))->type;
  if(transaction_rec->msg_type == OFPT_MULTIPART_REQUEST)
  {
    transaction_rec->msg_subtype = ((struct ofp_multipart_request *)(msg->desc.start_of_data))->type;
  }
  transaction_rec->callback_fn  = callback_fn;
  transaction_rec->app_pvt_arg1 = app_arg1;
  transaction_rec->app_pvt_arg2 = app_arg2;
#if 0
  /*Record Time stamp*/
  gettimeofday(&transaction_rec->time_stamp,NULL);

#else
  retval = timer_create_and_start(NULL,
      &transaction_rec->timer_saferef,
      CNTLR_NOT_PEIRODIC_TIMER,
      CNTLR_XTN_NODE_TIMEOUT,
      cntlr_xtn_recrod_cleanup_func,
      (void *)transaction_rec,NULL);
  if(retval == OF_FAILURE)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error in creating transaction records cleanup  timer\r\n", __FUNCTION__,__LINE__);
    return OF_FAILURE;
  }

#endif
  /*Store transaction in transaction table*/
  OF_TRANSACTION_TABLE_HASH(transaction_rec->xid,
      datapath->datapath_id.ul[0],
      datapath->datapath_id.ul[1],
      hash);
  MCHASH_APPEND_NODE(cntlr_xtn_table,
      hash,
      transaction_rec,
      transaction_rec->rec_safe_ref.index,
      transaction_rec->rec_safe_ref.magic,
      (uint8_t *)transaction_rec + TRANSACTION_TBL_LINK_OFFSET,
      success);
  if(!success)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"transaction record entry into hash table failed");
    return OF_FAILURE;
  }
#if 0
   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"transanction node appended: index: %d, magic :%d ",
      transaction_rec->rec_safe_ref.index,
      transaction_rec->rec_safe_ref.magic);
#endif
  p_xtn_rec_saferef->index = transaction_rec->rec_safe_ref.index; //Deepthi
  p_xtn_rec_saferef->magic = transaction_rec->rec_safe_ref.magic; //Deepthi
  return OF_SUCCESS;
}

/* Caller MUST use RCU REAd Lock and unlock*/
  int32_t
cntlr_get_xtn_details(uint32_t           xid,
    of_data_path_id_t  dp_id,
    cntlr_transactn_rec_t **p_p_transaction_rec)
{
  cntlr_transactn_rec_t *transaction_rec;
  uint32_t                hash;

  OF_TRANSACTION_TABLE_HASH(xid,
      dp_id.ul[0],
      dp_id.ul[1],
      hash);
  MCHASH_BUCKET_SCAN(cntlr_xtn_table,
      hash,
      transaction_rec,
      cntlr_transactn_rec_t*,
      TRANSACTION_TBL_LINK_OFFSET) 
  {
    if(transaction_rec->xid == xid)
    {
      CNTLR_RCU_DE_REFERENCE(transaction_rec, *p_p_transaction_rec);
      return OF_SUCCESS;
    }
  }
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"Invaid Xid (XID = %#x)", xid);
  return OF_FAILURE;
}

  void
cntlr_free_xtn_node_rcu(struct rcu_head *node)
{
  cntlr_transactn_rec_t *transaction_rec = NULL;
//    (cntlr_transactn_rec_t*)((uint8_t*)node - 2 *sizeof(struct mchash_dll_node*));

  transaction_rec= (cntlr_transactn_rec_t *) ((unsigned char *)node - RCUNODE_OFFSET_IN_MCHASH_TBLLNK  -TRANSACTION_TBL_LINK_OFFSET);
  timer_stop_tmr(NULL, &transaction_rec->timer_saferef);
  //transaction_rec->msg->desc.free_cbk(transaction_rec->msg);

  mempool_release_mem_block (cntlr_xtn_nodes_mempool_g,(unsigned char*)transaction_rec,transaction_rec->is_heap);
}


  int32_t
cntlr_del_xtn_rec(uint32_t        xid,
    of_data_path_id_t *dp_id)
{
  cntlr_transactn_rec_t *transaction_rec;
  uint32_t               hash;
  uint8_t                is_del_status;

  CNTLR_RCU_READ_LOCK_TAKE();

  OF_TRANSACTION_TABLE_HASH( xid,
      dp_id->ul[0],
      dp_id->ul[1],
      hash);

  MCHASH_BUCKET_SCAN(cntlr_xtn_table,
      hash,transaction_rec,cntlr_transactn_rec_t*,TRANSACTION_TBL_LINK_OFFSET)
  {
    if(transaction_rec->xid == xid)
    {
      MCHASH_DELETE_NODE(cntlr_xtn_table,transaction_rec,TRANSACTION_TBL_LINK_OFFSET,is_del_status);
      if(is_del_status == FALSE)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Failed to delete Delete Xid ");
        CNTLR_RCU_READ_LOCK_RELEASE();
        return OF_FAILURE;
      }
    }
  }

  CNTLR_RCU_READ_LOCK_RELEASE();

  return OF_SUCCESS;
}

  int32_t
cntlr_del_xtn_with_rec(cntlr_transactn_rec_t *transaction_rec)
{
  uint8_t  is_del_status;


  MCHASH_DELETE_NODE(cntlr_xtn_table,transaction_rec,TRANSACTION_TBL_LINK_OFFSET,is_del_status);
  if(is_del_status == FALSE)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Failed to delete Delete Xid");
    return OF_FAILURE;
  }

  return OF_SUCCESS;
}

inline  int32_t cntlr_del_xtn_with_safe_ref(cntlr_xtn_node_saferef_t *p_xtn_rec_saferef)
{
  uint8_t  is_del_status;
  cntlr_assert(p_xtn_rec_saferef);

  MCHASH_DELETE_NODE_BY_REF(cntlr_xtn_table,
      p_xtn_rec_saferef->index,
      p_xtn_rec_saferef->magic,
      cntlr_transactn_rec_t,
      TRANSACTION_TBL_LINK_OFFSET,
      is_del_status);
  if(is_del_status == FALSE)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Failed to delete Delete Xid ");
    return OF_FAILURE;
  }
  return OF_SUCCESS;
}

  void
cntlr_time_stamp_xtn_rec(uint32_t        xid,
    of_data_path_id_t *dp_id)
{
  cntlr_transactn_rec_t *transaction_rec;
  uint32_t          hash;

  CNTLR_RCU_READ_LOCK_TAKE();

  OF_TRANSACTION_TABLE_HASH(xid,
      dp_id->ul[0],
      dp_id->ul[1],
      hash);
  MCHASH_BUCKET_SCAN(cntlr_xtn_table,
      hash,transaction_rec,
      cntlr_transactn_rec_t*,
      TRANSACTION_TBL_LINK_OFFSET)
  {
    if(transaction_rec->xid == xid)
    {
      /*Record Time stamp*/
      gettimeofday(&transaction_rec->time_stamp,NULL);
    }
  }

  CNTLR_RCU_READ_LOCK_RELEASE();
}


  void* 
cntlr_xtn_recrods_cleanup_func(void *cbk_arg1,
    void *cbk_arg2)
{
  uint32_t               hashkey;
  uint64_t               elapsed_time;
  struct timeval         curr_time;
  cntlr_transactn_rec_t *transaction_rec;

  return NULL;
  MCHASH_TABLE_SCAN(cntlr_xtn_table,hashkey)
  {
    MCHASH_BUCKET_SCAN(cntlr_xtn_table,hashkey,transaction_rec,cntlr_transactn_rec_t*,TRANSACTION_TBL_LINK_OFFSET)
    {
      gettimeofday(&curr_time,NULL);
      elapsed_time = (curr_time.tv_sec  - transaction_rec->time_stamp.tv_sec)*1000 + 
        (curr_time.tv_usec - transaction_rec->time_stamp.tv_usec)/1000.0;

      if(elapsed_time >= CNTLR_XTN_REC_TIEMOUT)
        cntlr_del_xtn_with_safe_ref(&transaction_rec->rec_safe_ref);
    }
  }
}


  void* 
cntlr_xtn_recrod_cleanup_func(void *cbk_arg1,
    void *cbk_arg2)
{
  uint32_t               hashkey;
  uint64_t               elapsed_time;
  struct timeval         curr_time;
  cntlr_transactn_rec_t *transaction_rec= (cntlr_transactn_rec_t *)cbk_arg1;

  if(transaction_rec)
  {
  //  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"cleaning trans rec %d", transaction_rec->xid);

    if(transaction_rec->msg_type ==OFPT_MULTIPART_REQUEST)
      clean_multipart_req_msgs(transaction_rec);

    cntlr_del_xtn_with_safe_ref(&transaction_rec->rec_safe_ref);
  }
  else
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"trans rec is null");
  }
  return;
}

void clean_multipart_req_msgs(cntlr_transactn_rec_t *p_xtn_rec)
{

  switch(p_xtn_rec->msg_subtype)
  {
    case OFPMP_PORT_DESC:
      ((of_port_desc_cbk_fn)(p_xtn_rec->callback_fn))(NULL,
      controller_handle,
      0,
      p_xtn_rec->dp_handle,
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
      0,
      p_xtn_rec->dp_handle,
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
      0,
      p_xtn_rec->dp_handle,
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
         0,
         p_xtn_rec->dp_handle,
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
         0,
         p_xtn_rec->dp_handle,
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
      0,
      p_xtn_rec->dp_handle,
      NULL,
      NULL,
      OF_RESPONSE_STATUS_ERROR,
      NULL,
      TRUE);
      break;

    case OFPMP_GROUP:
      ((of_group_stats_cbk_fn)(p_xtn_rec->callback_fn))(NULL,
      controller_handle,
      0,
      p_xtn_rec->dp_handle,
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
      0,
      p_xtn_rec->dp_handle,
      NULL,
      NULL,
      OF_RESPONSE_STATUS_ERROR,
      NULL
      );
      break;

    case OFPMP_TABLE_FEATURES:
      ((of_table_features_cbk_fn)(p_xtn_rec->callback_fn))(
      controller_handle,
      0,
      p_xtn_rec->dp_handle,
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
      0,
      p_xtn_rec->dp_handle,
      NULL,
      NULL,
      OF_RESPONSE_STATUS_ERROR,
      NULL,
      TRUE);
      break;

    case OFPMP_METER:
      ((of_meter_stats_cbk_fn)(p_xtn_rec->callback_fn))(NULL,
      controller_handle,
      0,
      p_xtn_rec->dp_handle,
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
      0,
      p_xtn_rec->dp_handle,
      NULL,
      NULL,
      OF_RESPONSE_STATUS_ERROR,
      NULL
      );
      break;

    case OFPMP_QUEUE:
   case OFPMP_EXPERIMENTER:
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"Multipart not yet supported");
      break;
  }
  return;
}
