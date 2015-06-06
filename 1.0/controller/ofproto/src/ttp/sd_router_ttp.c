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

/* File  :      sd_router_ttp.c
 */

/*
 *
 * File name: sd_router_ttp.c
 * Author: Varun Kumar Reddy <B36461@freescale.com>
 * Date:   11/10/2013
 * Description: 
*/
/* Header files */
#include "controller.h"
#include "sd_router_ttp.h"
#include "ipv4fwdttp.h"
#include "nicira_ext.h"
#include "cntlr_event.h"
#include "ttpldef.h"
#include "cntrlappcmn.h"


extern struct cntlr_ttp_entry ttp_info_g[];
extern void *ttp_table_mempool_g;

/******************************************************************************
* Function:sd_router_ttp_table_init
* Description:
*	This functions initializes the sd router tabel type patterns list.
*	initializes the sd router tables and associated match fields.
******************************************************************************/
int32_t sd_router_ttp_init()
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
  strcpy(ttp_info_g[ttp_index].ttp_name, "SD_ROUTER_TTP");
  OF_LIST_INIT(ttp_info_g[ttp_index].table_list,ttp_free_table_entry_rcu);
  OF_LIST_INIT(ttp_info_g[ttp_index].domain_list,ttp_free_domain_entry_rcu);
  ttp_info_g[ttp_index].is_ttp_cfg=TRUE;


  for(table_id=0; table_id<CNTLR_SD_ROUTER_TTP_TABLES_CNT; table_id++)
  {

    /** Create memory for the table */
    ret_val = mempool_get_mem_block(ttp_table_mempool_g, (uchar8_t **)&ttp_table_node_p, &heap_b);
    if(ret_val != MEMPOOL_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "memory block allocation failed for the sd router table.");
      ret_val = CNTLR_TTP_FAILURE;
      goto SD_ROUER_INIT_EXIT;
    }
    ttp_table_node_p->heap_b = heap_b;    
    OF_LIST_INIT(ttp_table_node_p->match_fields_list,ttp_free_table_match_field_entry_rcu);
    switch(table_id)
    {
      /* Table id 0:In Port Classification Table */
      case IPV4_FWD_TTP_IN_PORT_CLASSIFY_TABLE_ID:
        ttp_table_node_p->table_id = IPV4_FWD_TTP_IN_PORT_CLASSIFY_TABLE_ID;
        strcpy(ttp_table_node_p->table_name, CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID0_NAME);    		
        ttp_table_node_p->max_columns    = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID0_COLUMNS;
        ttp_table_node_p->max_rows       = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID0_ROWS;
        ttp_table_node_p->is_wc_entry_present=FALSE;
        ttp_table_node_p->is_lpm_entry_present=FALSE;
        ttp_table_node_p->max_wcs=0;
        ttp_table_node_p->max_lpms=0;
    


        ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IN_PORT, 
				    FALSE, 
				    FALSE);
        if(ret_val != MEMPOOL_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Add inport match field to InPortClassification table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
	break;	



      /*Table id 1:Ethernet Filtering table */	
      case IPV4_FWD_TTP_ETH_FILTERING_TABLE_ID:
        ttp_table_node_p->table_id = IPV4_FWD_TTP_ETH_FILTERING_TABLE_ID;
        strcpy(ttp_table_node_p->table_name,CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID1_NAME);    		
        ttp_table_node_p->max_columns    = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID1_COLUMNS;
        ttp_table_node_p->max_rows = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID1_ROWS;
        ttp_table_node_p->is_wc_entry_present=TRUE;
        ttp_table_node_p->is_lpm_entry_present=FALSE;
        ttp_table_node_p->max_wcs=1;
        ttp_table_node_p->max_lpms=0;

        ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IN_PORT, 
				    TRUE, //in port may be wild card if dst mac addr isbroadcast mac address 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Add inport match field to Ethernet Filtering  table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }

        ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_ETH_DST, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add EthernetDestination match field to Ethernet Filterning table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
	break;	


      /* Table id2: Ethernet Classification Table */
      case IPV4_FWD_TTP_ETH_CLASSIFY_TABLE_ID:	
        ttp_table_node_p->table_id = IPV4_FWD_TTP_ETH_CLASSIFY_TABLE_ID;
        strcpy(ttp_table_node_p->table_name,CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID2_NAME);    		
        ttp_table_node_p->max_columns    = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID2_COLUMNS;//1
        ttp_table_node_p->max_rows = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID2_ROWS;//10

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
	  goto SD_ROUER_INIT_EXIT;
        }	
        break;	



      case IPV4_FWD_TTP_ARP_INBOUND_TABLE_ID:	
        ttp_table_node_p->table_id = IPV4_FWD_TTP_ARP_INBOUND_TABLE_ID;
        strcpy(ttp_table_node_p->table_name,CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID3_NAME);    		
        ttp_table_node_p->max_columns    = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID3_COLUMNS;
        ttp_table_node_p->max_rows = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID3_ROWS;

        ttp_table_node_p->is_wc_entry_present=TRUE;
        ttp_table_node_p->is_lpm_entry_present=FALSE;
        ttp_table_node_p->max_wcs=2;
        ttp_table_node_p->max_lpms=0;

  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_ETH_TYPE, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }	
 
	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_ARP_OP, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }	
 
 	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IN_PORT, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }	

        ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_ARP_TPA, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }	
       break;	

     case IPV4_FWD_TTP_IPV4_CLASSIFY_TABLE_ID:
        ttp_table_node_p->table_id = IPV4_FWD_TTP_IPV4_CLASSIFY_TABLE_ID;
        strcpy(ttp_table_node_p->table_name,CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID4_NAME);    		
        ttp_table_node_p->max_columns    = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID4_COLUMNS;//2
        ttp_table_node_p->max_rows = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID4_ROWS;//10

        ttp_table_node_p->is_wc_entry_present=TRUE;
        ttp_table_node_p->is_lpm_entry_present=FALSE;
        ttp_table_node_p->max_wcs=1;
        ttp_table_node_p->max_lpms=0;

      	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				     OXM_OF_ETH_TYPE, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }	
 
	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				     OXM_OF_IPV4_DST_W, 
				    TRUE, //making ipv4 classification table type acl  for now.we can make it lpm table later.
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }	
	break;

     case IPV4_FWD_TTP_IPV4_PBR_TABLE_ID:	
        ttp_table_node_p->table_id = IPV4_FWD_TTP_IPV4_PBR_TABLE_ID;
        strcpy(ttp_table_node_p->table_name,CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID5_NAME);    		
        ttp_table_node_p->max_columns    = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID5_COLUMNS;//7
        ttp_table_node_p->max_rows = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID5_ROWS;//4096

        ttp_table_node_p->is_wc_entry_present=TRUE;
        ttp_table_node_p->is_lpm_entry_present=FALSE;
        ttp_table_node_p->max_wcs=6;
        ttp_table_node_p->max_lpms=0;


 	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				     OXM_OF_METADATA_W, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        } 

 	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				     OXM_OF_IN_PORT, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }	

  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_ETH_TYPE, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }	

  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IPV4_SRC_W, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }	

   	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IPV4_DST_W, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }	 

   	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IP_DSCP, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }	 


	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    NXM_NX_REG2, //fw mark
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }

 
        break;	

     case IPV4_FWD_TTP_IPV4_ROUTING_TABLE_ID:
        ttp_table_node_p->table_id = IPV4_FWD_TTP_IPV4_ROUTING_TABLE_ID;
        strcpy(ttp_table_node_p->table_name,CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID6_NAME);    		
        ttp_table_node_p->max_columns    = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID6_COLUMNS;//5
        ttp_table_node_p->max_rows = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID6_ROWS;//16*1024

        ttp_table_node_p->is_wc_entry_present=TRUE;
        ttp_table_node_p->is_lpm_entry_present=TRUE;
        ttp_table_node_p->max_wcs=1;
        ttp_table_node_p->max_lpms=1;

   	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_METADATA_W, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    NXM_NX_REG0, //routing table id
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_ETH_TYPE, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
 	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IPV4_DST_W, 
				    FALSE, 
				    TRUE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
 	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IP_DSCP, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }	 
	break;

  case IPV4_FWD_TTP_ARP_RESOLUTION_TABLE_ID:
        ttp_table_node_p->table_id = IPV4_FWD_TTP_ARP_RESOLUTION_TABLE_ID;
        strcpy(ttp_table_node_p->table_name,CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID7_NAME);    		
        ttp_table_node_p->max_columns    = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID7_COLUMNS;//1
        ttp_table_node_p->max_rows = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID7_ROWS;//1024

        ttp_table_node_p->is_wc_entry_present=TRUE;
        ttp_table_node_p->is_lpm_entry_present=FALSE;
        ttp_table_node_p->max_wcs=0;
        ttp_table_node_p->max_lpms=0;

   	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_METADATA_W, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
	break;	

  case IPV4_FWD_TTP_IPV4_MCAST_GROUP_TABLE_ID:
        ttp_table_node_p->table_id = IPV4_FWD_TTP_IPV4_MCAST_GROUP_TABLE_ID;
        strcpy(ttp_table_node_p->table_name, CNTLR_SD_ROUTER_TTP_MULTICAST_TABLE_ID8_NAME);    		
        ttp_table_node_p->max_columns    = CNTLR_SD_ROUTER_TTP_MULTICAST_TABLE_ID8_COLUMNS;//1
        ttp_table_node_p->max_rows = CNTLR_SD_ROUTER_TTP_MULTICAST_TABLE_ID8_ROWS;//1024

        ttp_table_node_p->is_wc_entry_present=FALSE;
        ttp_table_node_p->is_lpm_entry_present=FALSE;
        ttp_table_node_p->max_wcs=0;
        ttp_table_node_p->max_lpms=0;

   	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_METADATA_W, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }

   	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IPV4_DST, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
	
	break;	
 
  case IPV4_FWD_TTP_IPV4_MCAST_ROUTING_TABLE_ID:
        ttp_table_node_p->table_id = IPV4_FWD_TTP_IPV4_MCAST_ROUTING_TABLE_ID;
        strcpy(ttp_table_node_p->table_name, CNTLR_SD_ROUTER_TTP_MULTICAST_TABLE_ID9_NAME);    		
        ttp_table_node_p->max_columns    = CNTLR_SD_ROUTER_TTP_MULTICAST_TABLE_ID9_COLUMNS;
        ttp_table_node_p->max_rows = CNTLR_SD_ROUTER_TTP_MULTICAST_TABLE_ID9_ROWS;

        ttp_table_node_p->is_wc_entry_present=FALSE;
        ttp_table_node_p->is_lpm_entry_present=FALSE;
        ttp_table_node_p->max_wcs=0;
        ttp_table_node_p->max_lpms=0;

   	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_METADATA_W, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }

   	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_ETH_TYPE, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }

   	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IPV4_SRC, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
   	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IPV4_DST, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
	
	break;
  case IPSEC_APP_SELF_CLASSIFICATION_TABLE_ID:
        OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Self Ipv4 Classification Table...");
	/* Self Ipv4 Classification Table */
        ttp_table_node_p->table_id = IPSEC_APP_SELF_CLASSIFICATION_TABLE_ID;
        strcpy(ttp_table_node_p->table_name, CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID11_NAME);    		
        ttp_table_node_p->max_columns    = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID11_COLUMNS;//3
        ttp_table_node_p->max_rows       = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID11_ROWS;//1024
        ttp_table_node_p->is_wc_entry_present=TRUE;
        ttp_table_node_p->is_lpm_entry_present=FALSE;
        ttp_table_node_p->max_wcs=0;
        ttp_table_node_p->max_lpms=0;

   	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_METADATA_W, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }

  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_ETH_TYPE, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }	
  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IP_PROTO, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        } 
	break;	

  case IPSEC_APP_PACKET_CLASSIFICATION_TABLE_ID:	
        /* IPSec Packet Classification Table */
        ttp_table_node_p->table_id = IPSEC_APP_PACKET_CLASSIFICATION_TABLE_ID;
        strcpy(ttp_table_node_p->table_name,CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID12_NAME);    		
        ttp_table_node_p->max_columns    = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID12_COLUMNS;//1
        ttp_table_node_p->max_rows = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID12_ROWS;//1024

        ttp_table_node_p->is_wc_entry_present=TRUE;
        ttp_table_node_p->is_lpm_entry_present=FALSE;
        ttp_table_node_p->max_wcs=0;
        ttp_table_node_p->max_lpms=0;

   	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_METADATA_W, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
        break;	

 
  case IPSEC_APP_UDP_ENCAP_NATT_TABLE_ID:
    	/* Self UDP Classification Table */
        ttp_table_node_p->table_id = IPSEC_APP_UDP_ENCAP_NATT_TABLE_ID;
        strcpy(ttp_table_node_p->table_name,CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID13_NAME);    		
        ttp_table_node_p->max_columns    = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID13_COLUMNS;//5
        ttp_table_node_p->max_rows = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID13_ROWS;//1024

        ttp_table_node_p->is_wc_entry_present=TRUE;
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
	  goto SD_ROUER_INIT_EXIT;
        }
	
  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IP_PROTO, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        } 


  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_UDP_DST, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        } 

  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    NXM_NX_REG5, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        } 
	
  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    NXM_NX_REG6, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }	
	break;
	
  case IPSEC_APP_IN_SA_TABLE_ID:
	/* Ipsec Inbound SA Table */	
        ttp_table_node_p->table_id = IPSEC_APP_IN_SA_TABLE_ID;
        strcpy(ttp_table_node_p->table_name,CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID14_NAME);    		
        ttp_table_node_p->max_columns    = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID14_COLUMNS;//4
        ttp_table_node_p->max_rows = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID14_ROWS;//1024

        ttp_table_node_p->is_wc_entry_present=TRUE;
        ttp_table_node_p->is_lpm_entry_present=FALSE;
        ttp_table_node_p->max_wcs=0;
        ttp_table_node_p->max_lpms=0;

	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_METADATA_W, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
	

  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_ETH_TYPE, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
	
  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IP_PROTO, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        } 

	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IPV4_DST_W, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
  	break;	

  case IPSEC_APP_IN_POLICY_TABLE_ID:
	/* IPSec Inbound policy table */	
        ttp_table_node_p->table_id = IPSEC_APP_IN_POLICY_TABLE_ID;
        strcpy(ttp_table_node_p->table_name,CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID15_NAME);    		
        ttp_table_node_p->max_columns    = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID15_COLUMNS;//11
        ttp_table_node_p->max_rows = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID15_ROWS;//1024

        ttp_table_node_p->is_wc_entry_present=TRUE;
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
	  goto SD_ROUER_INIT_EXIT;
        }
	

  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_ETH_TYPE, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IPV4_SRC_W, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
	
	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IPV4_DST_W, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
	
  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IP_PROTO, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        } 

	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_UDP_SRC, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }


  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_UDP_DST, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
	
  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_TCP_SRC, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_TCP_DST, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
	

  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_ICMPV4_TYPE, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
	

  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_ICMPV4_CODE, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
  	break;	
  case IPSEC_APP_OUT_POLICY_TABLE_ID:	
	/* IPSec Outbound policy table */	
        ttp_table_node_p->table_id = IPSEC_APP_OUT_POLICY_TABLE_ID;
        strcpy(ttp_table_node_p->table_name,CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID16_NAME);    		
        ttp_table_node_p->max_columns    = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID16_COLUMNS;//12
        ttp_table_node_p->max_rows = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID16_ROWS;//1024

        ttp_table_node_p->is_wc_entry_present=TRUE;
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
	  goto SD_ROUER_INIT_EXIT;
        }
	

  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_ETH_TYPE, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IPV4_SRC_W, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
	
	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IPV4_DST_W, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
	
  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IP_PROTO, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        } 
	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IP_DSCP, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        } 


	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_UDP_SRC, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }


  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_UDP_DST, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
	
  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_TCP_SRC, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_TCP_DST, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
	

  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_ICMPV4_TYPE, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
	

  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_ICMPV4_CODE, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
  	break;	

  case IPSEC_APP_OUT_SA_TABLE_ID:	
	/* IPSec Outbound SA table */	
        ttp_table_node_p->table_id = IPSEC_APP_OUT_SA_TABLE_ID;
        strcpy(ttp_table_node_p->table_name,CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID17_NAME);    		
        ttp_table_node_p->max_columns    = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID17_COLUMNS;//12
        ttp_table_node_p->max_rows = CNTLR_SD_ROUTER_TTP_UNICAST_TABLE_ID17_ROWS;//1024

        ttp_table_node_p->is_wc_entry_present=TRUE;
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
	  goto SD_ROUER_INIT_EXIT;
        }
	

  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_ETH_TYPE, 
				    FALSE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IPV4_SRC_W, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
	
	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IPV4_DST_W, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
	
  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IP_PROTO, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        } 
	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_IP_DSCP, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        } 


	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_UDP_SRC, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }


  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_UDP_DST, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
	
  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_TCP_SRC, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_TCP_DST, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
	

  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_ICMPV4_TYPE, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
	

  	ret_val = add_match_field_to_table(&(ttp_table_node_p->match_fields_list),
				    OXM_OF_ICMPV4_CODE, 
				    TRUE, 
				    FALSE);
        if(ret_val != CNTLR_TTP_SUCCESS)
        {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "add match field to table failed.");
	  ret_val = CNTLR_TTP_FAILURE;
	  goto SD_ROUER_INIT_EXIT;
        }
  	break;	 
    }	
    OF_APPEND_NODE_TO_LIST(ttp_info_g[ttp_index].table_list, ttp_table_node_p, CNTLR_TTP_TABLE_LIST_NODE_OFFSET);
    ttp_info_g[ttp_index].number_of_tables++;
  }
  return CNTLR_TTP_SUCCESS;
SD_ROUER_INIT_EXIT:
  ttp_tbl_match_field_list_deinit("SD_ROUTER_TTP");
  return ret_val;
}


int32_t sd_router_ttp_deinit()
{
  ttp_tbl_match_field_list_deinit("SD_ROUTER_TTP");
}

int32_t get_first_sd_router_ttp_table()
{
}
int32_t get_next_sd_router_ttp_table()
{
}


