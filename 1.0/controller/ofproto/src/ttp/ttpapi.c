/** Table Type Patterns Module init source file 
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

/* File  : ttpapi.c
 */
/*
  * File name: ttpapi.c
 * Author: Varun Kumar Reddy <B36461@freescale.com>
 * Date:   12/17/2013
 * Description: 
*/

/* Header files */

#include "controller.h"
#include "cntlr_event.h"
#include "cntlr_tls.h"
#include "cntrl_queue.h"
#include "cntlr_transprt.h"
#include "of_multipart.h"
#include "dprmldef.h"
#include "dprm.h"
#include "ttpldef.h"
#include "ipv4fwdttp.h"
#include "fsl_ext.h"

#define round_up(v,s)   ((((v) + (s) - 1) / (s)) * (s))
/* Table Type Patterns global info*/
extern struct cntlr_ttp_entry ttp_info_g[];

/*Table type patterns  domain mempools */
extern void *ttp_domain_mempool_g;
/*Table type patterns  tables mempools */
extern void *ttp_table_mempool_g;
/*Table type patterns  table matchfields mempools */
extern void *ttp_table_match_field_mempool_g;
void
cntlr_handle_tbl_ftr_rpl_msg( struct of_msg *msg,
    uint64_t controller_handle,
    uint64_t domain_handle,
    uint64_t datapath_handle,
    uint8_t  table_id,
    void  *cbk_arg1,
    void  *cbk_arg2,
    uint8_t status,
    struct   ofp_table_features  *tbl_ftr_rply_p,
    uint8_t more);

int32_t
cntlr_ttp_dp_ready_event(uint64_t  controller_handle,
                      uint64_t   domain_handle,
                      uint64_t   datapath_handle,
                      void       *cbk_arg1,
                      void       *cbk_arg2);

int32_t ip_unres_pkt_from_dp(int64_t controller_handle,
                     uint64_t domain_handle,
                     uint64_t datapath_handle,
                     struct of_msg     *msg,
                     struct ofl_packet_in  *pkt_in,
                     void   *cbk_arg1,
                     void   *cbk_arg2);



/******************************************************************************/
/* RCU to release domain memory block from the domain mempool */
void ttp_free_domain_entry_rcu(struct rcu_head *domain_entry_p)
{
  struct ttp_domain_entry  *domain_info_p;
  int32_t ret_value;
  if(domain_entry_p)
  {
    domain_info_p = (struct ttp_domain_entry*)((char *)domain_entry_p - CNTLR_TTP_DOMAIN_LIST_RCU_HEAD_OFFSET);
    ret_value = mempool_release_mem_block(ttp_domain_mempool_g, (uchar8_t*)domain_info_p, domain_info_p->heap_b);
    if(ret_value != MEMPOOL_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Failed to release ttp domain memory block.");
    }
    else
    {
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Released ttp domain memory block from the memory pool.");
    }

  }
  else
  {
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "NULL passed for deletion.");
  }
}

/******************************************************************************/
/* RCU to release table memory block from the ttp table mempool */
void ttp_free_table_entry_rcu(struct rcu_head *table_entry_p)
{
  struct ttp_table_entry  *table_info_p=NULL;
  int32_t ret_value;
  if(table_entry_p)
  {
    table_info_p = (struct ttp_table_entry*)((char *)table_entry_p - CNTLR_TTP_TABLE_LIST_RCU_HEAD_OFFSET);
    ret_value = mempool_release_mem_block(ttp_table_mempool_g, (uchar8_t*)table_info_p, table_info_p->heap_b);
    if(ret_value != MEMPOOL_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Failed to release ttp table memory block.");
    }
    else
    {
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Released ttp table memory block from the memory pool.");
    }

  }
  else
  {
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "NULL passed for deletion.");
  }
}

/******************************************************************************/
/* RCU to release table memory block from the ttp table mempool */
void ttp_free_table_match_field_entry_rcu(struct rcu_head *table_match_filed_entry_p)
{
  struct table_match_field_entry  *table_match_field_info_p=NULL;
  int32_t ret_value;
  if(table_match_filed_entry_p)
  {
    table_match_field_info_p = (struct table_match_field_entry*)((char *)table_match_filed_entry_p - CNTLR_TTP_TABLE_MATCH_FIELD_LIST_RCU_HEAD_OFFSET);
    ret_value = mempool_release_mem_block(ttp_table_match_field_mempool_g, (uchar8_t*)table_match_filed_entry_p, table_match_field_info_p->heap_b);
    if(ret_value != MEMPOOL_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Failed to release ttp table match field_memory block.");
    }
    else
    {
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Released ttp table match field memory block from the memory pool.");
    }

  }
  else
  {
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "NULL passed for deletion.");
  }
}

/*********************************************************************/
int32_t get_number_of_ttp_tables(char *ttp_name_p, uint32_t *no_of_tbls_p)
{  
  int32_t ttp_index=-1;
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Entered(ttp_name:%s)",ttp_name_p);
  for(ttp_index=0; ttp_index<CNTLR_MAX_NUMBER_OF_TTPS; ttp_index++)
  {
    if(ttp_info_g[ttp_index].is_ttp_cfg == TRUE)
    {
      if(!strcmp(ttp_name_p, ttp_info_g[ttp_index].ttp_name))
      {
        *no_of_tbls_p=ttp_info_g[ttp_index].number_of_tables;
        return CNTLR_TTP_SUCCESS;
      }
    }
  }
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Failed to get ttp index");
  return CNTLR_TTP_FAILURE;
}

/*********************************************************************/

/******************************************************************************/
int32_t get_free_ttp_node_index(uint32_t *ttp_index_p)
{
  int32_t ttp_index=-1;  
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Entered");

  for(ttp_index=0; ttp_index<CNTLR_MAX_NUMBER_OF_TTPS; ttp_index++)
  {
    if(ttp_info_g[ttp_index].is_ttp_cfg != TRUE)
    {
        *ttp_index_p = ttp_index;
        return CNTLR_TTP_SUCCESS;      
    }
  }
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Failed to get ttp index");
  return CNTLR_TTP_FAILURE;
}

/******************************************************************************/
/*get_ttp_node_index: Get ttp node index using ttp name */
int32_t get_ttp_node_index(char *ttp_name_p, int32_t *ttp_index_p)
{
  int32_t ttp_index=-1;  
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Entered(ttp_name:%s)",ttp_name_p);

  for(ttp_index=0; ttp_index<CNTLR_MAX_NUMBER_OF_TTPS; ttp_index++)
  {
    if(ttp_info_g[ttp_index].is_ttp_cfg == TRUE)
    {
      if(!strcmp(ttp_name_p, ttp_info_g[ttp_index].ttp_name))
      {
         OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "ttp_name index:%d",ttp_index);
        *ttp_index_p = ttp_index;
        return CNTLR_TTP_SUCCESS;
      }
    }
  }
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Failed to get ttp index(ttp_name:%s).",ttp_name_p);
  return CNTLR_TTP_FAILURE;
}
/*****************************************************************************/
int32_t get_ttp_tbl_info(char *ttp_name_p, uint32_t table_id, struct ttp_table_entry **ttp_table_info_p)
{
  uchar8_t ttp_tbl_offset;
  ttp_tbl_offset = CNTLR_TTP_TABLE_LIST_NODE_OFFSET;
  uint32_t ttp_index;
  struct ttp_table_entry  *ttp_table_entry_p=NULL;
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "ttp name:%s and table_id:%d",ttp_name_p,table_id);

  for(ttp_index=0; ttp_index<CNTLR_MAX_NUMBER_OF_TTPS; ttp_index++)
  {
    if(ttp_info_g[ttp_index].is_ttp_cfg == TRUE)
    {
      if(!strcmp(ttp_name_p, ttp_info_g[ttp_index].ttp_name))
      {
        OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Found ttp index");
        OF_LIST_SCAN(ttp_info_g[ttp_index].table_list, ttp_table_entry_p, struct ttp_table_entry *, ttp_tbl_offset)
        {
	  if(ttp_table_entry_p->table_id == table_id)
          {
  	    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Found table id");
	    *ttp_table_info_p = ttp_table_entry_p; 	
	    return CNTLR_TTP_SUCCESS;
          }
        } 
      }
    }
  } 
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "TTP Table info not found.");
  return CNTLR_TTP_FAILURE;
}
/******************************************************************************/
int32_t add_match_field_to_table(of_list_t *ttp_table_list_p,
			uint32_t match_field,
			uint8_t is_wild_card,
			uint8_t is_lpm
			)
{
  uint8_t heap_b;
  int32_t ret_value;
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Entered"); 
  struct  table_match_field_entry *table_match_field_node_p;

  /** Create memory for the table matchfields */
  ret_value = mempool_get_mem_block(ttp_table_match_field_mempool_g,
 				    (uchar8_t **)&table_match_field_node_p,
				    &heap_b);
  if(ret_value != MEMPOOL_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "memory block allocation failed for the sd router table.");
    return CNTLR_TTP_FAILURE;
  }
   /* heap_b is not assigned to the node */
  table_match_field_node_p->heap_b= heap_b;
  table_match_field_node_p->match_field = match_field;  
  table_match_field_node_p->is_wild_card = is_wild_card;
  table_match_field_node_p->is_lpm= is_lpm;			
  OF_APPEND_NODE_TO_LIST((*ttp_table_list_p), 
			 table_match_field_node_p, 
			 CNTLR_TTP_TABLE_MATCH_FIELD_LIST_NODE_OFFSET);
  return CNTLR_TTP_SUCCESS;
}


/******************************************************************************/
int32_t ttp_tbl_match_field_list_deinit(char *ttp_name_p)
{
  int32_t ret_val,ttp_index=-1;
  struct  ttp_table_entry *ttp_table_entry_p, *prev_ttp_table_entry_p = NULL;
  struct  table_match_field_entry *table_match_field_entry_p, *prev_table_match_field_entry_p = NULL;

  uchar8_t ttp_tbl_offset;
  ttp_tbl_offset = CNTLR_TTP_TABLE_LIST_NODE_OFFSET;

  uchar8_t tbl_match_field_offset;
  tbl_match_field_offset = CNTLR_TTP_TABLE_MATCH_FIELD_LIST_NODE_OFFSET;

  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Entered"); 
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "ttp_name:%s",ttp_name_p); 
  ret_val = get_ttp_node_index(ttp_name_p, &ttp_index);
  if(ret_val!=CNTLR_TTP_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Failed to get sd router ttp node index.");
    return CNTLR_TTP_FAILURE;
  }
  OF_LIST_SCAN(ttp_info_g[ttp_index].table_list, ttp_table_entry_p, struct ttp_table_entry *, ttp_tbl_offset )
  {
    OF_LIST_SCAN(ttp_table_entry_p->match_fields_list, table_match_field_entry_p, struct  table_match_field_entry*,tbl_match_field_offset)
    {
      OF_REMOVE_NODE_FROM_LIST(ttp_table_entry_p->match_fields_list, table_match_field_entry_p, prev_table_match_field_entry_p,tbl_match_field_offset);
    }
    OF_REMOVE_NODE_FROM_LIST(ttp_info_g[ttp_index].table_list,ttp_table_entry_p,prev_ttp_table_entry_p,ttp_tbl_offset );
    ttp_info_g[ttp_index].number_of_tables--;
  }
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "success"); 
  return CNTLR_TTP_SUCCESS;
}

/******************************************************************************/
int32_t add_domain_to_ttp(char *ttp_name_p, char *domain_name_p, uint64_t domain_handle)
{ 
  int32_t ttp_index=-1,ret_val;
  struct ttp_domain_entry *ttp_domain_node_p;
  uint8_t heap_b;

  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Entered"); 
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "ttp_name:%s, domain_name:%s",ttp_name_p, domain_name_p); 
  ret_val = get_ttp_node_index(ttp_name_p, &ttp_index);
  if(ret_val!=CNTLR_TTP_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Failed to get  ttp node index(ttp_name:%s,domain_name:%s).",ttp_name_p,domain_name_p);
    return CNTLR_TTP_FAILURE;
  }
  /** Create memory for the domain */
  ret_val = mempool_get_mem_block(ttp_domain_mempool_g, (uchar8_t **)&ttp_domain_node_p, &heap_b);
  if(ret_val != MEMPOOL_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "memory block allocation failed.");
    return CNTLR_TTP_FAILURE;    
  }
  ttp_domain_node_p->heap_b = heap_b;
  strcpy(ttp_domain_node_p->domain_name, domain_name_p);
  ttp_domain_node_p->domain_handle=domain_handle;
  OF_APPEND_NODE_TO_LIST(ttp_info_g[ttp_index].domain_list, 
			 ttp_domain_node_p, 
			 CNTLR_TTP_DOMAIN_LIST_NODE_OFFSET);
  ttp_info_g[ttp_index].number_of_domains++;
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "added domain successfully"); 
  return CNTLR_TTP_SUCCESS;
}

/******************************************************************************/

int32_t detach_domain_from_ttp(char *ttp_name_p, char *domain_name_p)
{
  int32_t ttp_index=-1,ret_val;
  struct ttp_domain_entry *ttp_domain_entry_p=NULL,*ttp_domain_prev_node_p=NULL;
  uint32_t ttp_domain_offset=CNTLR_TTP_DOMAIN_LIST_NODE_OFFSET;

  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Entered"); 
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "ttp_name:%s, domain_name:%s",ttp_name_p, domain_name_p); 
  ret_val = get_ttp_node_index(ttp_name_p, &ttp_index);
  if(ret_val!=CNTLR_TTP_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Failed to get sd router ttp node index.");
    return CNTLR_TTP_FAILURE;
  }
  OF_LIST_SCAN(ttp_info_g[ttp_index].domain_list, ttp_domain_entry_p, struct  ttp_domain_entry*,ttp_domain_offset)
  {
    if(!strcmp(domain_name_p, ttp_domain_entry_p->domain_name))
    {
      OF_REMOVE_NODE_FROM_LIST(ttp_info_g[ttp_index].domain_list, ttp_domain_entry_p, ttp_domain_prev_node_p,ttp_domain_offset);
      ttp_info_g[ttp_index].number_of_domains--;
      return CNTLR_TTP_SUCCESS;
    }
    ttp_domain_prev_node_p=ttp_domain_entry_p;
  }
  return CNTLR_TTP_FAILURE;
}
/***************************************************************************/
void cntlr_ttp_domain_event_cbk(uint32_t notification_type,
                             uint64_t domain_handle,
                             struct   dprm_domain_notification_data notification_data,
                             void     *callback_arg1,
                             void     *callback_arg2)
{
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "domain name = %s and Table Type Pattern=%s",notification_data.domain_name, notification_data.ttp_name);
 switch(notification_type)
  {
    case DPRM_DOMAIN_ADDED:
    {
        OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG,"DPRM_DOMAIN_ADDED event");
	if(add_domain_to_ttp(notification_data.ttp_name, notification_data.domain_name, domain_handle)!=CNTLR_TTP_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR,"Add domain to ttp failed!.");        
          return;
        }
        OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG,"Domain added to ttp.");
        if(cntlr_register_asynchronous_event_hook(domain_handle,
              DP_READY_EVENT,
              16,
              cntlr_ttp_dp_ready_event,
              NULL, NULL) == OF_FAILURE)
        {
          OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR,"Register DP Ready event cbk failed");        
	}
    }
    break;
   case DPRM_DOMAIN_DELETE:
      //need to unreg handler for DP_READY_EVENT
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG,"DPRM_DOMAIN_DELETE event");
      {
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG,"DPRM_DOMAIN_DELETED");
        break;
      }
  }
}
/******************************************************************************
* Func:cntlr_ttp_tbl_feature_len
* Description:
******************************************************************************/
int32_t cntlr_ttp_tbl_feature_len(int32_t ttp_index, uint16_t *msg_len_p)
{
  uint16_t msg_len=0, tbl_ftr_req_len =0,match_prop_len=0, wc_prop_len=0,lpm_prop_len=0;
  struct   ttp_table_entry         *ttp_table_entry_p  = NULL;
  struct   table_match_field_entry *table_match_field_entry_p = NULL;
  uchar8_t ttp_tbl_offset = CNTLR_TTP_TABLE_LIST_NODE_OFFSET;
  uchar8_t tbl_match_field_offset = CNTLR_TTP_TABLE_MATCH_FIELD_LIST_NODE_OFFSET;

  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Entered");

  msg_len = OFU_TABLE_FEATURES_REQ_MESSAGE_LEN;//multipart req hdr.


  OF_LIST_SCAN(ttp_info_g[ttp_index].table_list, ttp_table_entry_p, struct ttp_table_entry *, ttp_tbl_offset)
  {

    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Table = %s ",ttp_table_entry_p->table_name);      
    if(!(ttp_table_entry_p->max_columns))
    {
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Table (%s) has no match fields.",ttp_table_entry_p->table_name);      
      continue;
    }

    /* Add Table Feature Header Len */
    tbl_ftr_req_len = sizeof(struct ofp_table_features); 
   
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "tbl_ftr_hdr_len:%d",tbl_ftr_req_len);
    /* Propery len  . match fields have property header and no of match fields.*/
    match_prop_len = ((sizeof(struct ofp_table_feature_prop_oxm)) + ((ttp_table_entry_p->max_columns)*sizeof(uint32_t))); 

    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "match_prop_len:%d,round_up(match_prop_len):%d",match_prop_len,round_up(match_prop_len,8));
    tbl_ftr_req_len += round_up(match_prop_len, 8);
    
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "match fields:%d, match_hdr_len:%d,tbl_ftr_req_len:%d",ttp_table_entry_p->max_columns,sizeof(struct ofp_table_feature_prop_oxm),tbl_ftr_req_len);


    /* if wc entries presents, add to len */
    if(ttp_table_entry_p->is_wc_entry_present == TRUE)
    {
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "wild card entry presents in the table");
      wc_prop_len = ((sizeof(struct ofp_table_feature_prop_oxm)) +((ttp_table_entry_p->max_wcs)*sizeof(uint32_t)));

      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "wc_prop_len:%d,round_up(wc_prop_len):%d",wc_prop_len,round_up(wc_prop_len,8));
      tbl_ftr_req_len += round_up(wc_prop_len, 8);
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "no of wcs:%d,wc_hdr_len:%d,tbl_ftr_req_len:%d",ttp_table_entry_p->max_wcs,sizeof(struct ofp_table_feature_prop_oxm),tbl_ftr_req_len);
    }


    /* lpm entries presents add to len */
    if(ttp_table_entry_p->is_lpm_entry_present == TRUE)
    {
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "lpm entry presents in the table");
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "no of lpms:%d",ttp_table_entry_p->max_lpms);
      lpm_prop_len = ((sizeof(struct ofp_table_feature_prop_experimenter)) +(ttp_table_entry_p->max_lpms*sizeof(uint32_t)));
     tbl_ftr_req_len += round_up(lpm_prop_len, 8);
     OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "lpm_prop_len:%d,round_up(lpm_prop_len):%d",lpm_prop_len,round_up(lpm_prop_len,8));
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "no of lpms:%d,lpm_hdr_len:%d,tbl_ftr_req_len:%d",ttp_table_entry_p->max_lpms,(sizeof(struct ofp_table_feature_prop_experimenter)),tbl_ftr_req_len);
    }
    msg_len += tbl_ftr_req_len ;
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "tbl_ftr_req_len :%d",tbl_ftr_req_len);
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "msg_len :%d",msg_len);
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "########################");
  }   
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "total msg_len:%d",msg_len);
  *msg_len_p = msg_len;
  return CNTLR_TTP_SUCCESS;
}

int32_t
cntlr_ttp_dp_ready_event(uint64_t  controller_handle,
                         uint64_t   domain_handle,
                         uint64_t   datapath_handle,
                         void       *cbk_arg1,
                         void       *cbk_arg2)
{
  struct   of_msg                        *msg=NULL;
  struct   dprm_datapath_entry           *datapath_info_p=NULL;
  struct   dprm_domain_entry             *domain_info_p=NULL;  
  struct   ofp_multipart_request         *multipart_tbl_feature_req_msg_p=NULL;
  struct   ofp_table_features            *tbl_ftr_req_p=NULL;
  struct   ttp_table_entry               *ttp_table_entry_p = NULL;
  struct   table_match_field_entry       *table_match_field_entry_p = NULL;
  //struct ofp_table_feature_prop_oxm *oxm_wc_capabilities=NULL; 
  cntlr_conn_node_saferef_t  conn_safe_ref;
  cntlr_conn_table_t        *conn_table=NULL;
  uchar8_t ttp_tbl_offset= CNTLR_TTP_TABLE_LIST_NODE_OFFSET;
  uchar8_t tbl_match_field_offset = CNTLR_TTP_TABLE_MATCH_FIELD_LIST_NODE_OFFSET;
  uint8_t  send_status = TRUE;
  int32_t  status = OF_SUCCESS;
  uint32_t  ii;
  int32_t  ttp_index;
  uint16_t msg_len=0;
 
 
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Entered");

  CNTLR_RCU_READ_LOCK_TAKE();

  do{
  if(get_datapath_byhandle(datapath_handle, &datapath_info_p) != DPRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "No DP Entry with given datapath_handle=%llu",datapath_handle);
    status = CNTLR_TTP_FAILURE;
    break;
  }

  if(get_domain_byhandle(domain_handle, &domain_info_p) != DPRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "No domain Entry with given domain_handle=%llu",domain_handle);
    status =  CNTLR_TTP_FAILURE;
    break;
  }

  /* Get the TTP node index wid name */
  if(get_ttp_node_index(domain_info_p->ttp_name, &ttp_index)!=CNTLR_TTP_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Failed to get ttp node index");
    status= CNTLR_TTP_FAILURE;
    break;
  }
 
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "ttp_name:%s and index:%d",domain_info_p->ttp_name,ttp_index);

  if(!datapath_info_p->is_main_conn_present)
  {
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR," Error in sending message to datapath,channel closed");
    status = CNTLR_CHN_CLOSED;
    break;
  }
  CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath_info_p,conn_safe_ref,conn_table);
  if ( conn_table == NULL )
  {
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR,"conn table NULL");
    status = OF_FAILURE;
    break;
  }

  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "ttp_index:%d,nooftables:%d",ttp_index,ttp_info_g[ttp_index].number_of_tables);

  /* Check the Number tables presents in the ttp */
  if(!(ttp_info_g[ttp_index].number_of_tables))
  {
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "no of tables zero");
    status= CNTLR_TTP_FAILURE;
    break;
  }


  /* calculate the table feature msg len */
  if(cntlr_ttp_tbl_feature_len(ttp_index, &msg_len)!=CNTLR_TTP_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Failed to get ttp feature len.");
    status= CNTLR_TTP_FAILURE;
    break;
  }

  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "msg_len:%d",msg_len);
  msg = ofu_alloc_message(OFPT_MULTIPART_REQUEST, msg_len);
  if (msg == NULL)
  {
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR,"OF message alloc error ");
    status = OF_FAILURE;
    break;
  }

  /*Build Message Content */

  /* Multipart request header*/
  multipart_tbl_feature_req_msg_p = (struct ofp_multipart_request*)(msg->desc.start_of_data);

  multipart_tbl_feature_req_msg_p->header.version  = OFP_VERSION;
  multipart_tbl_feature_req_msg_p->header.type     = OFPT_MULTIPART_REQUEST;
  multipart_tbl_feature_req_msg_p->header.xid      = 0;
  multipart_tbl_feature_req_msg_p->type     = htons(OFPMP_TABLE_FEATURES);
  msg->desc.data_len = sizeof(struct ofp_multipart_request);

  tbl_ftr_req_p = (struct ofp_table_features*)(multipart_tbl_feature_req_msg_p->body);

  OF_LIST_SCAN(ttp_info_g[ttp_index].table_list, ttp_table_entry_p, struct ttp_table_entry *, ttp_tbl_offset )
  {
    if(!(ttp_table_entry_p->max_columns))
    {
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Table(id:%d,name:%s) null",ttp_table_entry_p->table_id,ttp_table_entry_p->table_name);
	continue;
    }
    /* Table feature msg hdr */

    /* fill table id */
    tbl_ftr_req_p->table_id = ttp_table_entry_p->table_id;
    /* table name */
    strncpy(tbl_ftr_req_p->name, ttp_table_entry_p->table_name, OFP_MAX_TABLE_NAME_LEN);     
    tbl_ftr_req_p->metadata_match=0xffffffffffffffff;
    tbl_ftr_req_p->metadata_write=0xffffffffffffffff;
    /* max rows presents in the table */
    tbl_ftr_req_p->max_entries = htonl(ttp_table_entry_p->max_rows);
    //tbl_ftr_req_p->config = OFPTC_TABLE_MISS_CONTROLLER;
    tbl_ftr_req_p->length =  sizeof(struct ofp_table_features);


    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Table name = %s ",ttp_table_entry_p->table_name);
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "tbl_ftr_hdr_len:%d",tbl_ftr_req_p->length);

    /*Add  match fields property to the table feature message*/
    struct ofp_table_feature_prop_oxm *oxm_capabilities = (struct ofp_table_feature_prop_oxm *)(tbl_ftr_req_p->properties);

    oxm_capabilities->type = htons(OFPTFPT_MATCH);
    oxm_capabilities->length = sizeof(struct ofp_table_feature_prop_oxm);
    ii=0;
    OF_LIST_SCAN(ttp_table_entry_p->match_fields_list, table_match_field_entry_p, struct  table_match_field_entry*,tbl_match_field_offset)
    {
      oxm_capabilities->oxm_ids[ii] = htonl(table_match_field_entry_p->match_field);	
      oxm_capabilities->length += sizeof(uint32_t);
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "oxm_capabilities->oxm_ids[ii]:%x\n",oxm_capabilities->oxm_ids[ii]);
      ii++;
    }
//    oxm_capabilities->length = round_up(oxm_capabilities->length, 8);
    tbl_ftr_req_p->length +=  round_up(oxm_capabilities->length,8);
    oxm_capabilities->length = htons(oxm_capabilities->length);
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "oxm_capabilities->length:%d",oxm_capabilities->length);

    if(ttp_table_entry_p->is_wc_entry_present == TRUE)
    {
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "wild card present in the table");
      /* wild card field property */  
      oxm_capabilities = (struct ofp_table_feature_prop_oxm *)((char*)tbl_ftr_req_p + tbl_ftr_req_p->length);     
      oxm_capabilities->type = OFPTFPT_WILDCARDS;
      oxm_capabilities->length = sizeof(struct ofp_table_feature_prop_oxm);      
      ii=0;
      OF_LIST_SCAN(ttp_table_entry_p->match_fields_list, table_match_field_entry_p, struct  table_match_field_entry*,tbl_match_field_offset)
      {
        if(table_match_field_entry_p->is_wild_card == TRUE)
	{
	  oxm_capabilities->oxm_ids[ii] = htonl(table_match_field_entry_p->match_field);	
	  oxm_capabilities->length += sizeof(uint32_t);
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "oxm_capabilities->oxm_ids:%x",oxm_capabilities->oxm_ids[ii]);
	  ii++;
	}
      }	
//      oxm_capabilities->length = round_up(oxm_capabilities->length, 8);
      tbl_ftr_req_p->length += round_up(oxm_capabilities->length,8);
      oxm_capabilities->length = htons(oxm_capabilities->length);
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "oxm_capabilities->length:%d",oxm_capabilities->length);
    }


    if(ttp_table_entry_p->is_lpm_entry_present == TRUE)
    {
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "lpm  present in the table");
      struct ofp_table_feature_prop_experimenter *oxm_lpm_capabilities=NULL;

      oxm_lpm_capabilities = (struct ofp_table_feature_prop_experimenter *)((char*)tbl_ftr_req_p + tbl_ftr_req_p->length);     

      oxm_lpm_capabilities->type = htons(OFPTFPT_EXPERIMENTER);
      oxm_lpm_capabilities->length = sizeof(struct ofp_table_feature_prop_experimenter);      
      ii=0;
      oxm_lpm_capabilities->experimenter = htonl(FSLX_VENDOR_ID);
      oxm_lpm_capabilities->exp_type = htonl(FSLXTFPT_LPM); 	
      OF_LIST_SCAN(ttp_table_entry_p->match_fields_list, table_match_field_entry_p, struct  table_match_field_entry*,tbl_match_field_offset)
      {
        if(table_match_field_entry_p->is_lpm == TRUE)
	{
	  oxm_lpm_capabilities->experimenter_data[ii] = htonl(table_match_field_entry_p->match_field);	
	  oxm_lpm_capabilities->length += sizeof(uint32_t);
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, " oxm_lpm_capabilities->experimenter_data[ii]:%x",oxm_lpm_capabilities->experimenter_data[ii]);
	  ii++;
	}
      }	
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "oxm_lpm_capabilities->length:%d",oxm_lpm_capabilities->length);
//      oxm_lpm_capabilities->length = round_up(oxm_lpm_capabilities->length,8);
      tbl_ftr_req_p->length += round_up(oxm_lpm_capabilities->length,8);
      oxm_lpm_capabilities->length = htons(oxm_lpm_capabilities->length);
    }
    msg->desc.data_len += tbl_ftr_req_p->length;

    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "tbl_ftr_req_len:%d,msg_desc_len:%d",tbl_ftr_req_p->length,msg->desc.data_len);
    tbl_ftr_req_p->length= htons(tbl_ftr_req_p->length);
    tbl_ftr_req_p = (struct ofp_table_features*)((msg->desc.start_of_data) + (msg->desc.data_len));    
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "*************************");
  }

  multipart_tbl_feature_req_msg_p->header.length = htons(msg->desc.data_len);
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "multipart_req_msg_len:%d",multipart_tbl_feature_req_msg_p->header.length);
  CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,datapath_info_p,conn_table,conn_safe_ref,
       (void*)cntlr_handle_tbl_ftr_rpl_msg,cbk_arg1,cbk_arg2,send_status);
  if(send_status == FALSE)
  {
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR," Error in sending message to datapath ");
    status = CNTLR_SEND_MSG_TO_DP_ERROR;
    break;
  }
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Successfully sent table feature msg!.");
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  return status;
}

void
cntlr_handle_tbl_ftr_rpl_msg( struct of_msg *msg,
    uint64_t controller_handle,
    uint64_t domain_handle,
    uint64_t datapath_handle,
    uint8_t  table_id,
    void  *cbk_arg1,
    void  *cbk_arg2,
    uint8_t status,
    struct   ofp_table_features  *tbl_ftr_rply_p,
    uint8_t more)
{
  
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Entered");
  if(msg != NULL)
    msg->desc.free_cbk(msg);
}


int32_t  get_first_ttp(struct ttp_config_info  *ttp_config_info_p)
{
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Entered");
  strcpy(ttp_config_info_p->name, ttp_info_g[0].ttp_name);
  ttp_config_info_p->no_of_tbls = ttp_info_g[0].number_of_tables;
  ttp_config_info_p->no_of_domains =  ttp_info_g[0].number_of_domains;
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "name:%s,tbls:%d,domains:%d",ttp_config_info_p->name, ttp_config_info_p->no_of_tbls, ttp_config_info_p->no_of_domains);
  return CNTLR_TTP_SUCCESS;
}

int32_t  get_next_ttp(char *ttp_name_p,struct  ttp_config_info  *ttp_config_info_p)
{
  uint32_t ii;
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Entered.ttp_name:%s",ttp_name_p);
  for(ii=0;ii<CNTLR_MAX_NUMBER_OF_TTPS;ii++)
  {
    if(!strcmp(ttp_info_g[ii].ttp_name,ttp_name_p))
    {
      ii++;
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "ii:%d",ii);
      if (!strlen(ttp_info_g[ii].ttp_name))
         return CNTLR_TTP_FAILURE;
      strcpy(ttp_config_info_p->name, ttp_info_g[ii].ttp_name);
      ttp_config_info_p->no_of_tbls = ttp_info_g[ii].number_of_tables;
      ttp_config_info_p->no_of_domains =  ttp_info_g[ii].number_of_domains;
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "name:%s,tbls:%d,domains:%d",ttp_config_info_p->name, ttp_config_info_p->no_of_tbls, ttp_config_info_p->no_of_domains);
      return CNTLR_TTP_SUCCESS;
    }
  }
  return CNTLR_TTP_FAILURE;
}
int32_t  get_exact_ttp(char *ttp_name_p,struct  ttp_config_info  *ttp_config_info_p)
{
  uint32_t ii;
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Entered.ttp_name:%s",ttp_name_p);
  for(ii=0;ii<CNTLR_MAX_NUMBER_OF_TTPS;ii++)
  {
    if(!strcmp(ttp_info_g[ii].ttp_name,ttp_name_p))
    {
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "ii:%d",ii);
      if(ii==CNTLR_MAX_NUMBER_OF_TTPS)return CNTLR_TTP_FAILURE;
      strcpy(ttp_config_info_p->name, ttp_info_g[ii].ttp_name);
      ttp_config_info_p->no_of_tbls = ttp_info_g[ii].number_of_tables;
      ttp_config_info_p->no_of_domains =  ttp_info_g[ii].number_of_domains;
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "name:%s,tbls:%d,domains:%d",ttp_config_info_p->name, ttp_config_info_p->no_of_tbls, ttp_config_info_p->no_of_domains);
      return CNTLR_TTP_SUCCESS;
    }
  }
}



int32_t get_first_ttp_tbl(char *ttp_name_p, struct ttp_tbl_info  *ttp_tbl_info_p)
{
  struct  ttp_table_entry *ttp_table_node_p=NULL;
  uint8_t  entry_found_b = 0;
  uint32_t ii;
  
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Entered.ttp_name:%s",ttp_name_p);
  if ((ttp_name_p == NULL) || strlen(ttp_name_p) == 0)
    return CNTLR_TTP_FAILURE;
  for(ii=0;ii<CNTLR_MAX_NUMBER_OF_TTPS;ii++)
  {
    if(!strcmp(ttp_info_g[ii].ttp_name,ttp_name_p))
    {
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "ii:%d",ii);
      if(ii==CNTLR_MAX_NUMBER_OF_TTPS)
          return CNTLR_TTP_FAILURE;
      OF_LIST_SCAN(ttp_info_g[ii].table_list, ttp_table_node_p, struct  ttp_table_entry *, CNTLR_TTP_TABLE_LIST_NODE_OFFSET)
      {
        entry_found_b = 1;
         break;
      }
      if(entry_found_b == 0 )
        return CNTLR_TTP_FAILURE;

      if ((ttp_table_node_p->table_name == NULL) || (strlen(ttp_table_node_p->table_name) == 0))
        return CNTLR_TTP_FAILURE;
         
      ttp_tbl_info_p->tbl_id = ttp_table_node_p->table_id;
      strcpy(ttp_tbl_info_p->tbl_name, ttp_table_node_p->table_name);
      ttp_tbl_info_p->max_rows    = ttp_table_node_p->max_rows;
      ttp_tbl_info_p->max_columns = ttp_table_node_p->max_columns;
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "tbl_id:%d,tbl_name:%s,rows:%d,columns:%d",ttp_tbl_info_p->tbl_id ,ttp_tbl_info_p->tbl_name, ttp_tbl_info_p->max_rows,ttp_tbl_info_p->max_columns);
      return CNTLR_TTP_SUCCESS;
    }
  }
  return CNTLR_TTP_FAILURE;
}

int32_t get_next_ttp_tbl(char *ttp_name_p, char *tbl_name_p,  struct ttp_tbl_info  *ttp_tbl_info_p)
{
  struct  ttp_table_entry *ttp_table_node_p=NULL;
  uint8_t  entry_found_b = 0;
  uint32_t ii;
  
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Entered.ttp_name:%s,table_name_p=%s",ttp_name_p,tbl_name_p);
  for(ii=0;ii<CNTLR_MAX_NUMBER_OF_TTPS;ii++)
  {
    if(!strcmp(ttp_info_g[ii].ttp_name,ttp_name_p))
    {
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "ii:%d",ii);
      if(ii==CNTLR_MAX_NUMBER_OF_TTPS)return CNTLR_TTP_FAILURE;
      OF_LIST_SCAN(ttp_info_g[ii].table_list, ttp_table_node_p, struct  ttp_table_entry *, CNTLR_TTP_TABLE_LIST_NODE_OFFSET)
      {
        if(entry_found_b == 0)
        {  
	  if(!strcmp(ttp_table_node_p->table_name,tbl_name_p))
          {
 	    entry_found_b=1;
	    continue;//break;	
          }
          else
	    continue;
        }
        else
        {
          ttp_tbl_info_p->tbl_id = ttp_table_node_p->table_id;
          strcpy(ttp_tbl_info_p->tbl_name, ttp_table_node_p->table_name);
          ttp_tbl_info_p->max_rows    = ttp_table_node_p->max_rows;
          ttp_tbl_info_p->max_columns = ttp_table_node_p->max_columns;
          OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "tbl_id:%d,tbl_name:%s,rows:%d,columns:%d",ttp_tbl_info_p->tbl_id ,ttp_tbl_info_p->tbl_name, ttp_tbl_info_p->max_rows,ttp_tbl_info_p->max_columns);
          return CNTLR_TTP_SUCCESS;
        }
      }
      if(entry_found_b == 0)
       return CNTLR_TTP_FAILURE;
      /*else
      {
          ttp_tbl_info_p->tbl_id = ttp_table_node_p->table_id;
          strcpy(ttp_tbl_info_p->tbl_name, ttp_table_node_p->table_name);
          ttp_tbl_info_p->max_rows    = ttp_table_node_p->max_rows;
          ttp_tbl_info_p->max_columns = ttp_table_node_p->max_columns;
          OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "tbl_id:%d,tbl_name:%s,rows:%d,columns:%d",ttp_tbl_info_p->tbl_id ,ttp_tbl_info_p->tbl_name, ttp_tbl_info_p->max_rows,ttp_tbl_info_p->max_columns);
          return CNTLR_TTP_SUCCESS;
      }*/
    }
  }
  return CNTLR_TTP_FAILURE;
}
int32_t get_exact_ttp_tbl(char *ttp_name_p, char *tbl_name_p,  struct ttp_tbl_info  *ttp_tbl_info_p)
{
	struct  ttp_table_entry *ttp_table_node_p=NULL;
	uint8_t  entry_found_b = 0;
	uint32_t ii;

	OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Entered.ttp_name:%s,table_name_p",ttp_name_p,tbl_name_p);
	for(ii=0;ii<CNTLR_MAX_NUMBER_OF_TTPS;ii++)
	{
		if(!strcmp(ttp_info_g[ii].ttp_name,ttp_name_p))
		{
			OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "ii:%d",ii);
			if(ii==CNTLR_MAX_NUMBER_OF_TTPS)return CNTLR_TTP_FAILURE;
			OF_LIST_SCAN(ttp_info_g[ii].table_list, ttp_table_node_p, struct  ttp_table_entry *, CNTLR_TTP_TABLE_LIST_NODE_OFFSET)
			{
				if(!strcmp(ttp_table_node_p->table_name,tbl_name_p))
				{
					ttp_tbl_info_p->tbl_id = ttp_table_node_p->table_id;
					strcpy(ttp_tbl_info_p->tbl_name, ttp_table_node_p->table_name);
					ttp_tbl_info_p->max_rows    = ttp_table_node_p->max_rows;
					ttp_tbl_info_p->max_columns = ttp_table_node_p->max_columns;
					OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "tbl_id:%d,tbl_name:%s,rows:%d,columns:%d",ttp_tbl_info_p->tbl_id ,ttp_tbl_info_p->tbl_name, ttp_tbl_info_p->max_rows,ttp_tbl_info_p->max_columns);
					return CNTLR_TTP_SUCCESS;
				}
			}
		}
	}
	return CNTLR_TTP_FAILURE;
}

int32_t get_first_ttp_tbl_match_field(char *ttp_name_p,  char *tbl_name_p, struct match_field_info  *match_field_info_p)
{
  struct  ttp_table_entry *ttp_table_node_p=NULL;
  struct  table_match_field_entry *table_match_field_node_p=NULL;
  uint8_t  entry_found_b = 0;
  uint32_t ii;
  
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Entered.ttp_name:%s,table_name_p=%s",ttp_name_p,tbl_name_p);
  for(ii=0;ii<CNTLR_MAX_NUMBER_OF_TTPS;ii++)
  {
    if(!strcmp(ttp_info_g[ii].ttp_name,ttp_name_p))
    {
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "ii:%d",ii);
      if(ii==CNTLR_MAX_NUMBER_OF_TTPS)
        return CNTLR_TTP_FAILURE;
      OF_LIST_SCAN(ttp_info_g[ii].table_list, ttp_table_node_p, struct  ttp_table_entry *, CNTLR_TTP_TABLE_LIST_NODE_OFFSET)
      {
	  if(!strcmp(ttp_table_node_p->table_name,tbl_name_p))
          {
	    entry_found_b = 1;
	    break;	
          }
          
      }
      
      if(entry_found_b == 0)
	      return CNTLR_TTP_FAILURE;
      entry_found_b=0;
      OF_LIST_SCAN(ttp_table_node_p->match_fields_list, table_match_field_node_p, struct  table_match_field_entry *, CNTLR_TTP_TABLE_MATCH_FIELD_LIST_NODE_OFFSET)
      {
	      entry_found_b = 1;
	      break;
      }
      if(entry_found_b == 0)
	      return CNTLR_TTP_FAILURE;
      match_field_info_p->match_field_id=OXM_FIELD(table_match_field_node_p->match_field);
      match_field_info_p->is_wild_card=table_match_field_node_p->is_wild_card;
      match_field_info_p->is_lpm = table_match_field_node_p->is_lpm;
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Matchfield found match_field_id=%d",match_field_info_p->match_field_id);
      return CNTLR_TTP_SUCCESS;
    }
  }
  return CNTLR_TTP_FAILURE;
}

int32_t get_next_ttp_tbl_match_field(char *ttp_name_p, char *tbl_name_p,  uint32_t match_field_id, struct match_field_info *match_field_info_p)
{
  struct  ttp_table_entry *ttp_table_node_p=NULL;
  struct  table_match_field_entry *table_match_field_node_p=NULL;
  uint8_t  entry_found_b = 0,match_entry_found=0;
  uint32_t ii;
  
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Entered.ttp_name:%s,table_name_p=%s ::match_field_id=%d ",ttp_name_p,tbl_name_p,match_field_id);
  for(ii=0;ii<CNTLR_MAX_NUMBER_OF_TTPS;ii++)
  {
    if(!strcmp(ttp_info_g[ii].ttp_name,ttp_name_p))
    {
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "ii:%d",ii);
      if(ii==CNTLR_MAX_NUMBER_OF_TTPS)return CNTLR_TTP_FAILURE;
      OF_LIST_SCAN(ttp_info_g[ii].table_list, ttp_table_node_p, struct  ttp_table_entry *, CNTLR_TTP_TABLE_LIST_NODE_OFFSET)
      {
         if(!strcmp(ttp_table_node_p->table_name,tbl_name_p))
         {
            OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Entered.ttp_name:%s,table_name_p=%s ",ttp_name_p,tbl_name_p);
	    entry_found_b = 1;
		break;
         }
      }
      if(entry_found_b == 0)
	      return CNTLR_TTP_FAILURE;

          entry_found_b=0;
	  OF_LIST_SCAN(ttp_table_node_p->match_fields_list, table_match_field_node_p, struct  table_match_field_entry *, CNTLR_TTP_TABLE_MATCH_FIELD_LIST_NODE_OFFSET)
          {
            if(entry_found_b ==0)
            {
	      if(table_match_field_node_p->match_field==match_field_id)
              {
                OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Match found");
	    	entry_found_b = 1;
	        continue;	
              }
              else
	        continue;
            }
            else
            {
       	      match_field_info_p->match_field_id=OXM_FIELD(table_match_field_node_p->match_field);
              match_field_info_p->is_wild_card==table_match_field_node_p->is_wild_card;
              match_field_info_p->is_lpm = table_match_field_node_p->is_lpm;
             OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Matchfield found match_field_id=%d",match_field_info_p->match_field_id);
              return CNTLR_TTP_SUCCESS;
            }
          }
      }
      if(entry_found_b == 0)
      {
        OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Matchfield not found match_field_id=%d",match_field_id);
        return CNTLR_TTP_FAILURE;
      }
  }
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Matchfield not found");
  return CNTLR_TTP_FAILURE;
}

int32_t get_first_ttp_domain(char *ttp_name_p,  char *domain_name_p)
{
  struct ttp_domain_entry *ttp_domain_node_p=NULL;
  uint8_t  entry_found_b = 0;
  uint32_t ii;
  
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Entered.ttp_name:%s",ttp_name_p);
  for(ii=0;ii<CNTLR_MAX_NUMBER_OF_TTPS;ii++)
  {
    if(!strcmp(ttp_info_g[ii].ttp_name,ttp_name_p))
    {
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "ii:%d",ii);
      if(ii==CNTLR_MAX_NUMBER_OF_TTPS)return CNTLR_TTP_FAILURE;
      OF_LIST_SCAN(ttp_info_g[ii].domain_list, ttp_domain_node_p, struct  ttp_domain_entry *, CNTLR_TTP_DOMAIN_LIST_NODE_OFFSET)
      {
        entry_found_b = 1;
         break;
      }
      if(entry_found_b == 0 )
        return CNTLR_TTP_FAILURE;
     strcpy(domain_name_p, ttp_domain_node_p->domain_name);
     OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "domain_name_p:%s",domain_name_p);
     return CNTLR_TTP_SUCCESS;
    }
  }
  return CNTLR_TTP_FAILURE;
}
int32_t get_next_ttp_domain(char *ttp_name_p, char *domain_name_p,  char *next_domain_name_p)
{
  struct ttp_domain_entry *ttp_domain_node_p=NULL;
  uint8_t  entry_found_b = 0;
  uint32_t ii;
  
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Entered.ttp_name:%s,domain_name_p:%s",ttp_name_p,domain_name_p);
  for(ii=0;ii<CNTLR_MAX_NUMBER_OF_TTPS;ii++)
  {
    if(!strcmp(ttp_info_g[ii].ttp_name,ttp_name_p))
    {
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "ii:%d",ii);
      if(ii==CNTLR_MAX_NUMBER_OF_TTPS)return CNTLR_TTP_FAILURE;
      OF_LIST_SCAN(ttp_info_g[ii].domain_list, ttp_domain_node_p, struct  ttp_domain_entry *, CNTLR_TTP_DOMAIN_LIST_NODE_OFFSET)
      {
        if(entry_found_b == 0)
        {  
	  if(!strcmp(ttp_domain_node_p->domain_name,domain_name_p))
          {
 	    entry_found_b=1;
	    continue;	
          }
          else
	    continue;
        }
        else
        {
	  strcpy(next_domain_name_p,  ttp_domain_node_p->domain_name);
          OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "next_domain_name_p:%s",next_domain_name_p);
          return CNTLR_TTP_SUCCESS;
        }
      }
      if(entry_found_b == 0)
       return CNTLR_TTP_FAILURE;
    }
  }
  return CNTLR_TTP_FAILURE;
}
int32_t get_exact_ttp_domain(char *ttp_name_p, char *domain_name_p)
{
  struct ttp_domain_entry *ttp_domain_node_p=NULL;
  uint8_t  entry_found_b = 0;
  uint32_t ii;
  
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "Entered.ttp_name:%s,domain_name_p:%s",ttp_name_p,domain_name_p);
  for(ii=0;ii<CNTLR_MAX_NUMBER_OF_TTPS;ii++)
  {
    if(!strcmp(ttp_info_g[ii].ttp_name,ttp_name_p))
    {
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_DEBUG, "ii:%d",ii);
      if(ii==CNTLR_MAX_NUMBER_OF_TTPS)return CNTLR_TTP_FAILURE;
      OF_LIST_SCAN(ttp_info_g[ii].domain_list, ttp_domain_node_p, struct  ttp_domain_entry *, CNTLR_TTP_DOMAIN_LIST_NODE_OFFSET)
      {
	if(!strcmp(ttp_domain_node_p->domain_name,domain_name_p))
        {
	  return CNTLR_TTP_SUCCESS;
        }        
      }
    }
  }
  return CNTLR_TTP_FAILURE;
}
