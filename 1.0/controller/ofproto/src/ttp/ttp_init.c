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

/* File  : ttp_init.c
 */
/*
  * File name: ttp_initc
 * Author: Varun Kumar Reddy <B36461@freescale.com>
 * Date:   12/17/2013
 * Description: 
*/

/* Header files */
#include "controller.h"
#include "ttpldef.h"
#include "dprm.h"

/* Table Type Patterns global info*/
struct cntlr_ttp_entry ttp_info_g[CNTLR_MAX_NUMBER_OF_TTPS];
/*Table type patterns  domain mempools */
void *ttp_domain_mempool_g=NULL;
/*Table type patterns  tables mempools */
void *ttp_table_mempool_g=NULL;
/*Table type patterns  table matchfields mempools */
void *ttp_table_match_field_mempool_g=NULL;

void cntlr_ttp_domain_event_cbk(uint32_t notification_type,
                             uint64_t domain_handle,
                             struct   dprm_domain_notification_data notification_data,
                             void     *callback_arg1,
                             void     *callback_arg2);
/* ttp names . supporting following ttps currently*/
char *ttp_names[]={"SD_ROUTER_TTP",
                   "DATA_CENTER_VIRTUAL_SWITCH_TTP",
                   "SAMPLE_L3_FWD_TTP",
                   "SAMPLE_L2_FWD_TTP",
                   "CBENCH_TTP"
                  };


/* ttp_init:Table type pattern init function */
int32_t ttp_init()
{
  int32_t ret_value=CNTLR_TTP_SUCCESS;
  int32_t index=0;
  uint32_t ttp_domain_entries_max, ttp_domain_static_entries;
  uint32_t ttp_table_entries_max, ttp_table_static_entries;
  uint32_t ttp_table_match_field_entries_max, ttp_table_match_field_static_entries;
  struct mempool_params mempool_info={};

  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Entered");
  /* Domains pools list init **/
  dprm_get_domain_mempoolentries(&ttp_domain_entries_max,&ttp_domain_static_entries);

  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;  
  mempool_info.block_size = sizeof(struct ttp_table_entry);
  mempool_info.max_blocks = ttp_domain_entries_max;
  mempool_info.static_blocks = ttp_domain_static_entries;
  mempool_info.threshold_min = ttp_domain_static_entries/10;
  mempool_info.threshold_max = ttp_domain_static_entries/3;
  mempool_info.replenish_count = ttp_domain_static_entries/10;
  mempool_info.b_memset_not_required =  FALSE;
  mempool_info.b_list_per_thread_required =  FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("ttp_domain_pool", &mempool_info,
		  &ttp_domain_mempool_g);

  if(ret_value != MEMPOOL_SUCCESS)
  {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "TTP Domain mempool init failed.");
	  goto TTP_INIT_EXIT;
  }

  /** ttp table  memory pools init**/
  ttp_table_entries_max= (CNTLR_MAX_NUMBER_OF_TTPS*DPRM_MAX_DMNODE_OFTABLE_ENTRIES);
  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;  
  mempool_info.block_size = sizeof(struct ttp_table_entry);
  mempool_info.max_blocks = ttp_table_entries_max;
  mempool_info.static_blocks = ttp_table_entries_max;
  mempool_info.threshold_min = ttp_table_entries_max/10;
  mempool_info.threshold_max = ttp_table_entries_max/3;
  mempool_info.replenish_count = ttp_table_entries_max/10;
  mempool_info.b_memset_not_required =  FALSE;
  mempool_info.b_list_per_thread_required =  FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("ttp_table_pool", &mempool_info,
		  &ttp_table_mempool_g);

  if(ret_value != MEMPOOL_SUCCESS)
  {
	  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "TTP table mempool init failed.");
	  goto TTP_INIT_EXIT;
  }

  /** ttp table  match fields memory pools init**/
  ttp_table_match_field_entries_max= (CNTLR_MAX_NUMBER_OF_TTPS*DPRM_MAX_DMNODE_OFTABLE_ENTRIES*CNTLR_MAX_NUMBER_OF_MATCH_FIELDS);

  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;  
  mempool_info.block_size = sizeof(struct ttp_table_entry);
  mempool_info.max_blocks = ttp_table_match_field_entries_max;
  mempool_info.static_blocks = ttp_table_match_field_entries_max;
  mempool_info.threshold_min = ttp_table_match_field_entries_max/10;
  mempool_info.threshold_max = ttp_table_match_field_entries_max/3;
  mempool_info.replenish_count = ttp_table_match_field_entries_max/10;
  mempool_info.b_memset_not_required =  FALSE;
  mempool_info.b_list_per_thread_required =  FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("ttp_table_match_fields_pool", &mempool_info,
		  &ttp_table_match_field_mempool_g);

  if(ret_value != MEMPOOL_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "TTP table match field mempool init failed.");
    goto TTP_INIT_EXIT;
  }

  /* Initialization for SD ROUTER TTP */
  sd_router_ttp_init();
  /* Initialization for SAMPLE L3 FWD TTP */
  l3_fwd_ttp_init();
  /* Initialization for SAMPLE L3 FWD TTP */
  l2_fwd_ttp_init();
  /* Initialization for CBENCH TTP */
  cbench_ttp_init();
  if (dprm_register_forwarding_domain_notifications(DPRM_DOMAIN_ALL_NOTIFICATIONS,
            cntlr_ttp_domain_event_cbk,
                NULL, NULL)!=OF_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR,"Registering callback to forward domain failed....");
    return CNTLR_TTP_FAILURE;
  }
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "ttp init success");
  return CNTLR_TTP_SUCCESS;
TTP_INIT_EXIT:
  mempool_delete_pool(ttp_domain_mempool_g);
  mempool_delete_pool(ttp_table_mempool_g);
  mempool_delete_pool(ttp_table_match_field_mempool_g);
  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Table Type Pattern Init failed!.");
  return CNTLR_TTP_FAILURE;
}

int32_t ttp_deinit()
{
  int32_t ret_value=FALSE;
  uint32_t index;

  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "Entered.");

  ret_value = mempool_delete_pool(ttp_domain_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    return ret_value;

  ret_value = mempool_delete_pool(ttp_table_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
      return ret_value;

  ret_value = mempool_delete_pool(ttp_table_match_field_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
      return ret_value;

  OF_LOG_MSG(OF_LOG_TTP, OF_LOG_ERROR, "TTP De-init success.");
  return CNTLR_TTP_SUCCESS;
}


