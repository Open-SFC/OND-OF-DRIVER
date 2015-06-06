/** Table type pattern sd router source file 
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

/* File  :      l3_fwd_ttp.c
 */

/*
 *
 * File name: l3_fwd_ttp.c
 * Author: Varun Kumar Reddy <B36461@freescale.com>
 * Date:   11/10/2013
 * Description: 
*/
/* Header files */
#include "controller.h"
#include "l3_fwd_ttp.h"
#include "ipv4fwdttp.h"
#include "nicira_ext.h"
#include "cntlr_event.h"
#include "ttpldef.h"
#include "cntrlappcmn.h"


extern struct cntlr_ttp_entry ttp_info_g[];
extern void *ttp_table_mempool_g;

/******************************************************************************
* Function:l3_fwd_ttp_table_init
* Description:
*	This functions initializes the sd router tabel type patterns list.
*	initializes the sd router tables and associated match fields.
******************************************************************************/
int32_t l3_fwd_ttp_init()
{
  int32_t ret_val,ttp_index=-1;
  uint32_t table_id;
  uint8_t heap_b;
  struct  ttp_table_entry *ttp_table_node_p=NULL;
  struct  table_match_field_entry *table_match_field_node_p=NULL;


  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Entered");
  ret_val = get_free_ttp_node_index(&ttp_index);
  if(ret_val!=CNTLR_TTP_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Failed to get free ttp node index.");
    return CNTLR_TTP_FAILURE;
  }
  strcpy(ttp_info_g[ttp_index].ttp_name, "SAMPLE_L3_FWD_TTP");
  OF_LIST_INIT(ttp_info_g[ttp_index].table_list,ttp_free_table_entry_rcu);
  OF_LIST_INIT(ttp_info_g[ttp_index].domain_list,ttp_free_domain_entry_rcu);
  ttp_info_g[ttp_index].is_ttp_cfg=TRUE;


  for(table_id=0; table_id<CNTLR_SAMPLE_L3_FWD_TTP_TABLES_CNT; table_id++)
  {

    /** Create memory for the table */
    ret_val = mempool_get_mem_block(ttp_table_mempool_g, (uchar8_t **)&ttp_table_node_p, &heap_b);
    if(ret_val != MEMPOOL_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "memory block allocation failed .");
      ret_val = CNTLR_TTP_FAILURE;
      goto SAMPLE_L3_FWD_TTP_INIT_EXIT;
    }
    ttp_table_node_p->heap_b = heap_b;    
    OF_LIST_INIT(ttp_table_node_p->match_fields_list,ttp_free_table_match_field_entry_rcu);
    switch(table_id)
    {
      /* Table id 0:In Port Classification Table */
      case 0:
        ttp_table_node_p->table_id = IPV4_FWD_TTP_IN_PORT_CLASSIFY_TABLE_ID;
        strcpy(ttp_table_node_p->table_name, CNTLR_SAMPLE_L3_FWD_TTP_UNICAST_TABLE_ID0_NAME);    		
        ttp_table_node_p->max_columns    = CNTLR_SAMPLE_L3_FWD_TTP_UNICAST_TABLE_ID0_COLUMNS;
        ttp_table_node_p->max_rows       = CNTLR_SAMPLE_L3_FWD_TTP_UNICAST_TABLE_ID0_ROWS;
        ttp_table_node_p->is_wc_entry_present=FALSE;
        ttp_table_node_p->is_lpm_entry_present=FALSE;
        ttp_table_node_p->max_wcs=0;
        ttp_table_node_p->max_lpms=0;
    


        ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_ETH_TYPE, 
				    FALSE, 
				    FALSE);
        if(ret_val != MEMPOOL_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Add inport match field to InPortClassification table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SAMPLE_L3_FWD_TTP_INIT_EXIT;
        }
	break;	



      /*Table id 1:Ethernet Filtering table */	
      case 1:
        ttp_table_node_p->table_id = IPV4_FWD_TTP_ETH_FILTERING_TABLE_ID;
        strcpy(ttp_table_node_p->table_name,CNTLR_SAMPLE_L3_FWD_TTP_UNICAST_TABLE_ID1_NAME);    		
        ttp_table_node_p->max_columns    = CNTLR_SAMPLE_L3_FWD_TTP_UNICAST_TABLE_ID1_COLUMNS;
        ttp_table_node_p->max_rows = CNTLR_SAMPLE_L3_FWD_TTP_UNICAST_TABLE_ID1_ROWS;
        ttp_table_node_p->is_wc_entry_present=FALSE;
        ttp_table_node_p->is_lpm_entry_present=FALSE;
        ttp_table_node_p->max_wcs=0;
        ttp_table_node_p->max_lpms=0;

        ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_ETH_TYPE, 
				    FALSE,
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Add  match field  failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SAMPLE_L3_FWD_TTP_INIT_EXIT;
        }

        ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IPV4_DST, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Add  match field failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SAMPLE_L3_FWD_TTP_INIT_EXIT;
        }
        ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IP_PROTO, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Add  match field failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SAMPLE_L3_FWD_TTP_INIT_EXIT;
        }
	break;	


      /* Table id2: Ethernet Classification Table */
      case 2:	
        ttp_table_node_p->table_id = IPV4_FWD_TTP_ETH_CLASSIFY_TABLE_ID;
        strcpy(ttp_table_node_p->table_name,CNTLR_SAMPLE_L3_FWD_TTP_UNICAST_TABLE_ID2_NAME);    		
        ttp_table_node_p->max_columns    = CNTLR_SAMPLE_L3_FWD_TTP_UNICAST_TABLE_ID2_COLUMNS;//1
        ttp_table_node_p->max_rows = CNTLR_SAMPLE_L3_FWD_TTP_UNICAST_TABLE_ID2_ROWS;//10

        ttp_table_node_p->is_wc_entry_present=FALSE;
        ttp_table_node_p->is_lpm_entry_present=FALSE;
        ttp_table_node_p->max_wcs=0;
        ttp_table_node_p->max_lpms=0;


        ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_ETH_TYPE, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Add inport match field to Ethernet Filtering  table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SAMPLE_L3_FWD_TTP_INIT_EXIT;
        }

        ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IPV4_DST, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add EthernetDestination match field to Ethernet Filterning table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SAMPLE_L3_FWD_TTP_INIT_EXIT;
        }
        break;	



      case 3:	
        ttp_table_node_p->table_id = IPV4_FWD_TTP_ARP_INBOUND_TABLE_ID;
        strcpy(ttp_table_node_p->table_name,CNTLR_SAMPLE_L3_FWD_TTP_UNICAST_TABLE_ID3_NAME);    		
        ttp_table_node_p->max_columns    = CNTLR_SAMPLE_L3_FWD_TTP_UNICAST_TABLE_ID3_COLUMNS;
        ttp_table_node_p->max_rows = CNTLR_SAMPLE_L3_FWD_TTP_UNICAST_TABLE_ID3_ROWS;

        ttp_table_node_p->is_wc_entry_present=FALSE;
        ttp_table_node_p->is_lpm_entry_present=FALSE;
        ttp_table_node_p->max_wcs=0;
        ttp_table_node_p->max_lpms=0;

  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_ETH_TYPE, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SAMPLE_L3_FWD_TTP_INIT_EXIT;
        }	
        ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IP_PROTO, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add EthernetDestination match field to Ethernet Filterning table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SAMPLE_L3_FWD_TTP_INIT_EXIT;
        }

        ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IPV4_SRC, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add EthernetDestination match field to Ethernet Filterning table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SAMPLE_L3_FWD_TTP_INIT_EXIT;
        }
        ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IPV4_DST, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add EthernetDestination match field to Ethernet Filterning table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SAMPLE_L3_FWD_TTP_INIT_EXIT;
        }
        ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_TCP_SRC, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add EthernetDestination match field to Ethernet Filterning table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SAMPLE_L3_FWD_TTP_INIT_EXIT;
        }

        ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_TCP_DST, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add EthernetDestination match field to Ethernet Filterning table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SAMPLE_L3_FWD_TTP_INIT_EXIT;
        }

        ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_UDP_SRC, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add EthernetDestination match field to Ethernet Filterning table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SAMPLE_L3_FWD_TTP_INIT_EXIT;
        }

        ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_UDP_DST, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add EthernetDestination match field to Ethernet Filterning table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SAMPLE_L3_FWD_TTP_INIT_EXIT;
        }
        ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_ICMPV4_TYPE, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add EthernetDestination match field to Ethernet Filterning table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SAMPLE_L3_FWD_TTP_INIT_EXIT;
        }

        ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_ICMPV4_CODE, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add EthernetDestination match field to Ethernet Filterning table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SAMPLE_L3_FWD_TTP_INIT_EXIT;
        }
        break;	

     case 4:
        ttp_table_node_p->table_id = IPV4_FWD_TTP_IPV4_CLASSIFY_TABLE_ID;
        strcpy(ttp_table_node_p->table_name,CNTLR_SAMPLE_L3_FWD_TTP_UNICAST_TABLE_ID4_NAME);    		
        ttp_table_node_p->max_columns    = CNTLR_SAMPLE_L3_FWD_TTP_UNICAST_TABLE_ID4_COLUMNS;//2
        ttp_table_node_p->max_rows = CNTLR_SAMPLE_L3_FWD_TTP_UNICAST_TABLE_ID4_ROWS;//10

        ttp_table_node_p->is_wc_entry_present=FALSE;
        ttp_table_node_p->is_lpm_entry_present=TRUE;
        ttp_table_node_p->max_wcs=0;
        ttp_table_node_p->max_lpms=1;

  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_ETH_TYPE, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SAMPLE_L3_FWD_TTP_INIT_EXIT;
        }	
        
        ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IPV4_SRC, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add EthernetDestination match field to Ethernet Filterning table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SAMPLE_L3_FWD_TTP_INIT_EXIT;
        }

      	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				     OXM_OF_IPV4_DST_W, 
				    FALSE,
				    TRUE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SAMPLE_L3_FWD_TTP_INIT_EXIT;
        }	
	break;

     case 5:	
        ttp_table_node_p->table_id = IPV4_FWD_TTP_IPV4_PBR_TABLE_ID;
        strcpy(ttp_table_node_p->table_name,CNTLR_SAMPLE_L3_FWD_TTP_UNICAST_TABLE_ID5_NAME);    		
        ttp_table_node_p->max_columns    = CNTLR_SAMPLE_L3_FWD_TTP_UNICAST_TABLE_ID5_COLUMNS;//7
        ttp_table_node_p->max_rows = CNTLR_SAMPLE_L3_FWD_TTP_UNICAST_TABLE_ID5_ROWS;//4096

        ttp_table_node_p->is_wc_entry_present=FALSE;
        ttp_table_node_p->is_lpm_entry_present=FALSE;
        ttp_table_node_p->max_wcs=0;
        ttp_table_node_p->max_lpms=0;


	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_METADATA, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SAMPLE_L3_FWD_TTP_INIT_EXIT;
        } 
        break;	
    }	
    OF_APPEND_NODE_TO_LIST(ttp_info_g[ttp_index].table_list, ttp_table_node_p, CNTLR_TTP_TABLE_LIST_NODE_OFFSET);
    ttp_info_g[ttp_index].number_of_tables++;
  }
  return CNTLR_TTP_SUCCESS;
SAMPLE_L3_FWD_TTP_INIT_EXIT:
  ttp_tbl_match_field_list_deinit("SAMPLE_L3_FWD_TTP");
  return ret_val;
}


int32_t l3_fwd_ttp_deinit()
{
  ttp_tbl_match_field_list_deinit("SAMPLE_L3_FWD_TTP");
}

int32_t get_first_l3_fwd_ttp_table()
{
}
int32_t get_next_l3_fwd_ttp_table()
{
}


