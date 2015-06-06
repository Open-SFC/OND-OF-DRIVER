/* 
 *
 * Copyright  2012, 2013  Freescale Semiconductor
 *
 *
 * Licensed under the Apache License, version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
*/
/* 
 *
 * Copyright  2012, 2013  Freescale Semiconductor
 *
 *
 * Licensed under the Apache License, version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
*/
/* 
 *
 * Copyright  2012, 2013  Freescale Semiconductor
 *
 *
 * Licensed under the Apache License, version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
*/

/*
*
* File name: ttpldef.h
* Author: Varun Kumar Reddy <B36461@freescale.com>
* Date:   12/17/2013
* Description: 
       This file contains the TTP(Table Type Pattern) module macros, data structures.
***************************************************************************/
#ifndef __TTPLDEF_API_H
#define __TTPLDEF_API_H

#define CNTLR_TTP_SUCCESS  0//success return
#define CNTLR_TTP_FAILURE -1//failure return

/*Max no of table type patterns controller supports
  1.SD_ROUTER_TTP
  2.DATA_CENTER_VIRTUAL_SWITCH_TTP
  3.SAMPLE_L3_FWD_TTP
  4.SAMPLE_L2_FWD_TTP
  */
#define CNTLR_MAX_NUMBER_OF_TTPS 5 
#define CNTLR_MAX_NUMBER_OF_MATCH_FIELDS 40
#define CNTLR_MAX_TTP_NAME_LEN 64 


enum ttp_table_types
{
  TABLE_EXACT_MATCH=0,
  ACL,
  LPM
};

enum match_field_types
{
  MATCH_EXACT_MATCH=0,
};

struct cntlr_ttp_entry
{
  char ttp_name[CNTLR_MAX_TTP_NAME_LEN];  
  uint8_t is_ttp_cfg;//true if ttp init else false.
  /* number of tables attached to ttp */
  uint32_t number_of_tables;
  /* table list */
  of_list_t table_list;

  /* number of domains attached to ttp*/
  uint32_t number_of_domains;
  /* domains list */
  of_list_t domain_list;
};

/* MAXIMUM Number of async messages or events supported*/
#define OF_FIRST_ASYNC_EVENT      OF_ASYNC_MSG_PACKET_IN_EVENT /* ASSUMPTION  - This value is always 1*/
#define OF_LAST_ASYNC_EVENT       OF_ASYNC_MSG_ERROR_EVENT
#define OF_MAX_NUMBER_ASYNC_MSGS (OF_LAST_ASYNC_EVENT - OF_FIRST_ASYNC_EVENT) + 1
/* Applications detils for the given async message event. Appliations might register
 * to revieve events with priority. 
 **/
struct async_event_info
{
   uint8_t   type; /** Message or event type*/

   /** Details of each application registered, list of nodes type 'struct of_app_hook_info'
   *  values*/
   of_list_t app_list;
};
struct ttp_table_entry
{
  of_list_node_t  next_node;
  struct rcu_head rcu_head;
  uint8_t         heap_b;
  uint8_t         table_id;
  char            table_name[OFP_MAX_TABLE_NAME_LEN];
  uint32_t        max_rows;
  uint32_t        max_columns;// no of match fileds
  uint8_t         is_wc_entry_present;//true if wildcards presents in the table.
  uint32_t        max_wcs;//no of wild card entries.
  uint8_t         is_lpm_entry_present;//true if lpm presents in the table.
  uint32_t        max_lpms;//no of lpm entries.
  of_list_t       match_fields_list;     
  struct    async_event_info event_info[OF_MAX_NUMBER_ASYNC_MSGS + 1];
};

#define CNTLR_TTP_TABLE_LIST_NODE_OFFSET offsetof(struct ttp_table_entry,next_node)
#define CNTLR_TTP_TABLE_LIST_RCU_HEAD_OFFSET offsetof(struct ttp_table_entry,rcu_head)


struct table_match_field_entry
{
  of_list_node_t  next_node;
  struct rcu_head rcu_head;
  uint8_t         heap_b;
  uint32_t match_field;
  //uint32_t match_type;
  uint8_t  is_wild_card;
  uint8_t  is_lpm;
};
#define CNTLR_TTP_TABLE_MATCH_FIELD_LIST_NODE_OFFSET offsetof(struct table_match_field_entry, next_node)
#define CNTLR_TTP_TABLE_MATCH_FIELD_LIST_RCU_HEAD_OFFSET offsetof(struct table_match_field_entry, rcu_head)

struct ttp_domain_entry
{
  of_list_node_t  next_node;
  struct rcu_head rcu_head;
  uint8_t         heap_b;
  char            domain_name[DPRM_MAX_FORWARDING_DOMAIN_LEN];
  uint64_t        domain_handle;
};
#define CNTLR_TTP_DOMAIN_LIST_NODE_OFFSET offsetof(struct ttp_domain_entry, next_node)
#define CNTLR_TTP_DOMAIN_LIST_RCU_HEAD_OFFSET offsetof(struct ttp_domain_entry, rcu_head)


struct ttp_config_info
{
  char     name[CNTLR_MAX_TTP_NAME_LEN];
  uint32_t no_of_tbls;
  uint32_t no_of_domains;
};

struct ttp_tbl_info  
{
  uint8_t tbl_id;
  char tbl_name[OFP_MAX_TABLE_NAME_LEN];
  uint32_t max_rows,max_columns;
};
struct  match_field_info  
{
  uint32_t match_field_id;
  uint8_t is_wild_card; 
  uint8_t is_lpm ;
};




int32_t ttp_init();
int32_t ttp_deinit();
int32_t get_ttp_node_index(char *ttp_name, int32_t *index);
int32_t get_free_ttp_node_index(uint32_t *ttp_index_p);
void ttp_free_table_entry_rcu(struct rcu_head *table_entry_p);
void ttp_free_domain_entry_rcu(struct rcu_head *domain_entry_p);
void ttp_free_table_match_field_entry_rcu(struct rcu_head *table_match_filed_entry_p);


int32_t add_match_field_to_table(of_list_t *ttp_table_list_p,
                        uint32_t match_field,
                        uint8_t is_wild_card,
                        uint8_t is_lpm
                        );
int32_t ttp_tbl_match_field_list_deinit(char *ttp_name_p);
int32_t add_domain_to_ttp(char *ttp_name_p, char *domain_name_p, uint64_t domain_handle);
int32_t detach_domain_from_ttp(char *ttp_name_p, char *domain_name_p);


#endif//__TTPLDEF_API_H


