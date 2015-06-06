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

/*!\file of_flow.c
* Author     : Kumara Swamy Ganji <kumar.sg@freescale.com>
* Date       : May, 2013  
* Description: 
 */
#include "controller.h"
#include "of_utilities.h"
#include "of_msgapi.h"
#include "cntrl_queue.h"
#include "of_msgpool.h"
#include "cntlr_event.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "dprmldef.h"
#include "oflog.h"
//#include "lxoswrp.h"
#include "of_flowinfo.h"
#include "of_flowmod.h"


int32_t of_flow_push_action(struct of_msg *msg, 
    struct ofi_flow_action *action);

int32_t of_datapath_add_static_flow(uint64_t datapath_handle,
    struct ofi_flow_mod_info *flow_entry_p)
{
  struct   dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;
  uchar8_t offset;
  offset = OF_FLOW_MOD_LISTNODE_OFFSET;

  if (flow_entry_p == NULL)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"flow_entry_p is NULL");
    return OF_FAILURE;
  }

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle( datapath_handle,&datapath_entry_p);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"datapath entry not found");
      status=OF_FAILURE;
      break;
    }
    flow_entry_p->cookie = 0;
    flow_entry_p->cookie_mask = 0;
    flow_entry_p->command = 0;
    flow_entry_p->priority = 0;
    flow_entry_p->idle_timeout = 0;
    flow_entry_p->hard_timeout = 0;
    flow_entry_p->table_id = 0xff;
    OF_LIST_INIT(flow_entry_p->match_field_list, NULL);
    OF_LIST_INIT(flow_entry_p->instruction_list, NULL);
    OF_LIST_INIT(flow_entry_p->action_list, NULL);
    status=OF_SUCCESS;
    break;
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  if ( status == OF_SUCCESS)
  {	
    OF_APPEND_NODE_TO_LIST(datapath_entry_p->flow_table,flow_entry_p,offset);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"succesfully added flow %d", flow_entry_p->flow_id); 	
  }	

  return status;
}

int32_t of_datapath_update_flow(uint64_t datapath_handle, struct ofi_flow_mod_info *flow_info_p)
{
  struct ofi_flow_mod_info *flow_entry;
  struct   dprm_datapath_entry *datapath_entry=NULL;
  int32_t status=OF_SUCCESS;
  int32_t retval;
  uchar8_t offset;
  offset = OF_FLOW_MOD_LISTNODE_OFFSET;

  if (flow_info_p == NULL)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"flow_info_p is NULL");
    return OF_FAILURE;
  }


  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle( datapath_handle, &datapath_entry);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"datapath entry not found");
      status=OF_FAILURE;
      break;
    }

    OF_LIST_SCAN(datapath_entry->flow_table, flow_entry, struct ofi_flow_mod_info *,offset)
    {
      if (flow_entry->flow_id == flow_info_p->flow_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"flow found updateing values");
        flow_entry->table_id = flow_info_p->table_id;
        flow_entry->priority =  flow_info_p->priority;
        flow_entry->command =  flow_info_p->command;
        flow_entry->flow_flags = flow_info_p->flow_flags;
	flow_entry->cookie  = flow_info_p->cookie;
	flow_entry->cookie_mask = flow_info_p->cookie_mask;
        flow_entry->idle_timeout =  flow_info_p->idle_timeout;
        flow_entry->hard_timeout =  flow_info_p->hard_timeout;
        status = OF_SUCCESS;
        break;
      }
    }

    if (flow_entry == NULL)
      status = OF_FAILURE;
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  return status;
}


int32_t of_flow_add_match_field_to_list(uint64_t datapath_handle, 
    struct ofi_flow_match_fields *match_info,
    uint32_t flow_id)
{
  struct ofi_flow_mod_info *flow_entry=NULL;
  struct dprm_datapath_entry *datapath_entry=NULL;
  int32_t status=OF_SUCCESS;
  int32_t retval;
  uchar8_t offset;
  offset = OF_FLOW_MOD_LISTNODE_OFFSET;


  if (match_info == NULL)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"match_entry is NULL");
    return OF_FAILURE;
  }

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle(datapath_handle,&datapath_entry);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"datapath entry not found");
      break;
    }

    OF_LIST_SCAN(datapath_entry->flow_table, flow_entry, struct ofi_flow_mod_info *,offset )
    {
      if (flow_entry->flow_id == flow_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"flow found");
        status=OF_SUCCESS;
        break;
      }
    }

    if (flow_entry == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"no flow found with id=%d",flow_id);
      status = OF_FAILURE;
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  if (status == OF_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"adding match field to list");
    OF_APPEND_NODE_TO_LIST(flow_entry->match_field_list, match_info,OF_FLOW_MATCH_FILED_LISTNODE_OFFSET);
  }
  return status;
}

int32_t of_flow_add_action_to_list(uint64_t datapath_handle, 
    struct ofi_flow_action *action_info,
    uint32_t flow_id)
{
  struct ofi_flow_mod_info *flow_entry=NULL;
  struct dprm_datapath_entry *datapath_entry=NULL;
  int32_t status=OF_SUCCESS;
  int32_t retval;


  if (action_info == NULL)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"action info is NULL");
    return OF_FAILURE;
  }

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle(datapath_handle,&datapath_entry);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"datapath entry not found");
      break;
    }

    OF_LIST_SCAN(datapath_entry->flow_table, flow_entry, struct ofi_flow_mod_info *,OF_FLOW_MOD_LISTNODE_OFFSET)
    {
      if (flow_entry->flow_id == flow_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"flow found");
        status = OF_SUCCESS;
        break;
      }
    }

    if (flow_entry == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"no flow found with id=%d",flow_id);
      status = OF_FAILURE;
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  if (status == OF_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"adding action to list");
    OF_APPEND_NODE_TO_LIST(flow_entry->action_list, action_info,OF_FLOW_ACTION_LISTNODE_OFFSET);
  }
  return status;
}

int32_t of_flow_add_instruction_to_list(uint64_t datapath_handle, 
    struct ofi_flow_instruction *instruction_info,
    uint32_t flow_id)
{
  struct ofi_flow_mod_info *flow_entry=NULL;
  struct dprm_datapath_entry *datapath_entry=NULL;
  int32_t status= OF_SUCCESS;
  int32_t retval;


  if (instruction_info == NULL)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"instruction info is NULL");
    return OF_FAILURE;
  }

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle(datapath_handle,&datapath_entry);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"datapath entry not found");
      break;
    }

    OF_LIST_SCAN(datapath_entry->flow_table, flow_entry, struct ofi_flow_mod_info *,OF_FLOW_MOD_LISTNODE_OFFSET)
    {
      if (flow_entry->flow_id == flow_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"flow found");
        status = OF_SUCCESS;
        break;
      }
    }

    if (flow_entry == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"no flow found with id=%d",flow_id);
      status = OF_FAILURE;
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  if (status == OF_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"adding inst to list");
    OF_APPEND_NODE_TO_LIST(flow_entry->instruction_list, instruction_info, OF_FLOW_INSTRUCTION_LISTNODE_OFFSET);
  }
  return status;
}


int32_t of_flow_instruction_add_action_to_list(uint64_t datapath_handle, 
    struct ofi_flow_action *action_info,
    uint32_t flow_id, uint16_t instruction_id)
{
  struct ofi_flow_mod_info *flow_entry=NULL;
  struct ofi_flow_instruction *inst_entry = NULL;
  struct dprm_datapath_entry *datapath_entry=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;


  if (action_info == NULL)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"instruction action info is NULL");
    return OF_FAILURE;
  }

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle(datapath_handle,&datapath_entry);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"datapath entry not found");
      break;
    }

    OF_LIST_SCAN(datapath_entry->flow_table, flow_entry, struct ofi_flow_mod_info *,OF_FLOW_MOD_LISTNODE_OFFSET)
    {
      if (flow_entry->flow_id == flow_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"flow found");
        OF_LIST_SCAN(flow_entry->instruction_list, inst_entry,
            struct ofi_flow_instruction *,OF_FLOW_INSTRUCTION_LISTNODE_OFFSET)
        {
          if (inst_entry->inst_id == instruction_id)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"flow instruction found");
            status=OF_SUCCESS;
            break;
          }
        }
        if (inst_entry == NULL)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,
              "no flow inst found with id=%d",instruction_id);
          status = OF_FAILURE;
        }
      }
      if(status == OF_SUCCESS)
        break;
    }

    if (flow_entry == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"no flow found with id=%d",flow_id);
      status = OF_FAILURE;
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  if (status == OF_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"added action to inst list");
    OF_APPEND_NODE_TO_LIST(inst_entry->action_list, action_info, OF_FLOW_ACTION_LISTNODE_OFFSET);
  }
  return status;
}

int32_t of_flow_instruction_remove_action_from_list(uint64_t datapath_handle, 
    struct ofi_flow_action *action_info,
    uint32_t flow_id, uint8_t instruction_id)
{
  struct ofi_flow_mod_info *flow_entry=NULL;
  struct ofi_flow_instruction *inst_entry = NULL;
  struct ofi_flow_action *action_entry = NULL, *prev_action_entry = NULL;
  struct dprm_datapath_entry *datapath_entry=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;


  if (action_info == NULL)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"instruction action info is NULL");
    return OF_FAILURE;
  }

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle(datapath_handle,&datapath_entry);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"datapath entry not found");
      break;
    }

    OF_LIST_SCAN(datapath_entry->flow_table, flow_entry, struct ofi_flow_mod_info *,OF_FLOW_MOD_LISTNODE_OFFSET)
    {
      if (flow_entry->flow_id == flow_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"flow found");
        OF_LIST_SCAN(flow_entry->instruction_list, inst_entry, struct ofi_flow_instruction *,OF_FLOW_INSTRUCTION_LISTNODE_OFFSET)
        {
          if (inst_entry->inst_id == instruction_id)
          {
            OF_LIST_SCAN(inst_entry->action_list, action_entry,
                struct ofi_flow_action *,OF_FLOW_ACTION_LISTNODE_OFFSET)
            {
              if (action_entry->action_id == action_info->action_id)
              {
                OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,
                    "instruction action found");
                OF_REMOVE_NODE_FROM_LIST(inst_entry->action_list,
                    action_entry, prev_action_entry,OF_FLOW_ACTION_LISTNODE_OFFSET);
                status = OF_SUCCESS;
                break;
              }
              prev_action_entry = action_entry;
            }

            if (action_entry == NULL)
            {
              OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,
                  "no inst actoin found with id=%d",action_info->action_type);
              status = OF_FAILURE;
              break;
            }
            if(status == OF_SUCCESS)
              break;
          }
        }

        if (inst_entry == NULL)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,
              "no flow inst found with id=%d",instruction_id);
          status = OF_FAILURE;
          break;
        }
        if(status == OF_SUCCESS)
          break;
      }
    }

    if(flow_entry == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"no flow found with id=%d",flow_id);
      status = OF_FAILURE;
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  if (status == OF_SUCCESS)
    free(action_entry);
  return status;
}


int32_t of_datapath_remove_static_flow(uint64_t datapath_handle, uint32_t flow_id)
{
  struct ofi_flow_mod_info *flow_entry_p = NULL, *prev_flow_entry_p = NULL;
  struct ofi_flow_action *action_entry_p;
  struct ofi_flow_instruction *instruction_entry_p;
  struct ofi_flow_match_fields *match_field_entry_p;
  struct dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_SUCCESS;
  int32_t retval;

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle(datapath_handle,&datapath_entry_p);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"datapath entry not found");
      break;
    }

    OF_LIST_SCAN(datapath_entry_p->flow_table,flow_entry_p, struct ofi_flow_mod_info *,OF_FLOW_MOD_LISTNODE_OFFSET)
    {
      if (flow_entry_p->flow_id == flow_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"flow found");
        OF_LIST_REMOVE_HEAD_AND_SCAN(flow_entry_p->match_field_list,
            match_field_entry_p,struct ofi_flow_match_fields *,OF_FLOW_MATCH_FILED_LISTNODE_OFFSET )
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"deleting match fields");
        }

        OF_LIST_SCAN(flow_entry_p->instruction_list,
            instruction_entry_p, struct ofi_flow_instruction *,OF_FLOW_INSTRUCTION_LISTNODE_OFFSET)
        {                   
          OF_LIST_REMOVE_HEAD_AND_SCAN(instruction_entry_p->action_list,
              action_entry_p,struct ofi_flow_action *,OF_FLOW_ACTION_LISTNODE_OFFSET)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"deleting instruction action");
          }
        }

        { 
          OF_LIST_REMOVE_HEAD_AND_SCAN(flow_entry_p->instruction_list,
              instruction_entry_p,struct ofi_flow_instruction *,OF_FLOW_INSTRUCTION_LISTNODE_OFFSET)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"deleting instruction");
          }
        }

        { 
          OF_LIST_REMOVE_HEAD_AND_SCAN(flow_entry_p->action_list,
              action_entry_p, struct ofi_flow_action *,OF_FLOW_ACTION_LISTNODE_OFFSET)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"deleting action");
          }
        }

        status=OF_SUCCESS;
        break;
      }
      prev_flow_entry_p=flow_entry_p;
    } 
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  if (status == OF_SUCCESS)
  {
    OF_REMOVE_NODE_FROM_LIST(datapath_entry_p->flow_table,flow_entry_p, prev_flow_entry_p, OF_FLOW_MOD_LISTNODE_OFFSET);
    free(flow_entry_p);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"succesfully deleted flow");
  }
  return status;
}

int32_t of_flow_remove_match_field_from_list(uint64_t datapath_handle, uint32_t flow_id, uint8_t match_field_id)
{
  struct ofi_flow_mod_info *flow_entry =NULL;
  struct ofi_flow_match_fields *match_entry = NULL, *prev_match_entry=NULL;
  struct dprm_datapath_entry *datapath_entry=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle( datapath_handle, &datapath_entry);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"datapath entry not found");
      break;
    }

    OF_LIST_SCAN(datapath_entry->flow_table, flow_entry, struct ofi_flow_mod_info *, OF_FLOW_MOD_LISTNODE_OFFSET)
    {
      if (flow_entry->flow_id == flow_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"flow found");
        OF_LIST_SCAN(flow_entry->match_field_list, match_entry, struct ofi_flow_match_fields *,OF_FLOW_MATCH_FILED_LISTNODE_OFFSET)
        {
          if (match_entry->field_id == match_field_id)
          {
            OF_REMOVE_NODE_FROM_LIST(flow_entry->match_field_list,
                match_entry, prev_match_entry, OF_FLOW_MATCH_FILED_LISTNODE_OFFSET);
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"deleting match field %d",match_field_id);
            status=OF_SUCCESS;
            break;
          }
          prev_match_entry = match_entry;
        }
      }
      if (status == OF_SUCCESS)
        break;
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  if (status == OF_SUCCESS)
    free(match_entry);
  return status;
}


int32_t of_flow_remove_action_from_list(uint64_t datapath_handle,
    uint32_t flow_id, uint8_t action_id)
{
  struct ofi_flow_mod_info *flow_entry =NULL;
  struct ofi_flow_action *action_entry=NULL, *prev_action_entry=NULL;
  struct dprm_datapath_entry *datapath_entry=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle( datapath_handle, &datapath_entry);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"datapath entry not found");
      break;
    }

    OF_LIST_SCAN(datapath_entry->flow_table, flow_entry, struct ofi_flow_mod_info *,OF_FLOW_MOD_LISTNODE_OFFSET)
    {
      if( flow_entry->flow_id == flow_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"flow found");
        OF_LIST_SCAN(flow_entry->action_list, action_entry, struct ofi_flow_action *,OF_FLOW_ACTION_LISTNODE_OFFSET)
        {
          if (action_entry->action_id == action_id)
          {
            OF_REMOVE_NODE_FROM_LIST(flow_entry->action_list,
                action_entry, prev_action_entry, OF_FLOW_ACTION_LISTNODE_OFFSET);
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,
                "deleting action %d",action_id);
            status=OF_SUCCESS;
            break;
          }
          prev_action_entry = action_entry;
        }
      }
      if(status == OF_SUCCESS)
        break;
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  if (status == OF_SUCCESS)
    free(action_entry);
  return status;
}

int32_t of_flow_remove_instruction_from_list(uint64_t datapath_handle,
    uint32_t flow_id, uint8_t instruction_id)
{
  struct ofi_flow_mod_info *flow_entry =NULL;
  struct ofi_flow_instruction *instruction_entry=NULL, *prev_instruction_entry=NULL;
  struct dprm_datapath_entry *datapath_entry=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle( datapath_handle, &datapath_entry);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"datapath entry not found");
      break;
    }

    OF_LIST_SCAN(datapath_entry->flow_table, flow_entry, struct ofi_flow_mod_info *,OF_FLOW_MOD_LISTNODE_OFFSET)
    {
      if( flow_entry->flow_id == flow_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"flow found");
        OF_LIST_SCAN(flow_entry->instruction_list, instruction_entry,
            struct ofi_flow_instruction *,OF_FLOW_INSTRUCTION_LISTNODE_OFFSET)
        {
          if (instruction_entry->inst_id == instruction_id)
          {
            OF_REMOVE_NODE_FROM_LIST(flow_entry->instruction_list,
                instruction_entry, prev_instruction_entry,OF_FLOW_INSTRUCTION_LISTNODE_OFFSET);
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,
                "deleting instruction %d",instruction_id);
            status=OF_SUCCESS;
            break;
          }
          prev_instruction_entry = instruction_entry;
        }
      }
      if(status == OF_SUCCESS)
        break;
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  if (status == OF_SUCCESS)
    free(instruction_entry);
  return status;
}


int32_t of_flow_push_instruction(struct of_msg *msg, 
    struct ofi_flow_instruction *instruction)
{  
  uint8_t  starting_location;
  uint16_t length_of_actions;
  uint32_t ins_len;
  int32_t  retval = OF_SUCCESS;
  int32_t  status = OF_SUCCESS;

  do
  {
    switch(instruction->instruction_type)
    {
      case OFPIT_GOTO_TABLE: /* Setup the next table in the lookup pipeline */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPIT_GOTO_TABLE:data=%d",instruction->ui8_data);
          retval = ofu_push_go_to_table_instruction(msg, instruction->ui8_data);

          if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,
                "\nfailed to push goto table instruction");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPIT_WRITE_METADATA:  /* Setup the metadata field for use later in pipeline */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,
              "OFPIT_WRITE_METADATA:data=%llx mask=%llx",
              instruction->ui64_data, *(uint64_t *)instruction->mask);
          retval = ofu_push_write_meta_data_instruction(msg,
              instruction->ui64_data,
              *(uint64_t *)instruction->mask);
          if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)
          {
            status = OF_FAILURE;
          }
          break;
        }
      case OFPIT_APPLY_ACTIONS: /* Applies the action(s) immediately */			 
        {
          struct ofi_flow_action *action;

          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPIT_APPLY_ACTIONS:");
          ins_len = instruction->instruction_len;
          retval = ofu_start_pushing_apply_action_instruction(msg);
          if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Error in adding apply action instruction err = %d",retval);
            status = OF_FAILURE;
            break;
          }
          ofu_start_pushing_actions(msg);
          OF_LIST_SCAN(instruction->action_list, action, struct ofi_flow_action *,OF_FLOW_ACTION_LISTNODE_OFFSET)
          {   
            retval = of_flow_push_action(msg, action);
            if (retval != OF_SUCCESS)
            {
              OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Failed to push action=%d instruction list",action->action_type);
              status = OF_FAILURE;
              break;
            }
          }
          ofu_end_pushing_actions(msg, &starting_location, &length_of_actions);
          ((struct ofp_instruction_actions*)(msg->desc.ptr2))->len = htons(length_of_actions);
          break;
        }
      case OFPIT_WRITE_ACTIONS: /* Write the action(s) onto the datapath action set */			
        {
          struct ofi_flow_action *action;
          
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPIT_WRITE_ACTIONS:");
          ins_len = instruction->instruction_len;
          retval= ofu_start_pushing_write_action_instruction(msg);
          if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Error in adding write action instruction err = %d",retval);
            status = OF_FAILURE;
          } 
          ofu_start_pushing_actions(msg);
          OF_LIST_SCAN(instruction->action_list, action, struct ofi_flow_action *,OF_FLOW_ACTION_LISTNODE_OFFSET)
          {
            retval = of_flow_push_action(msg, action);
            if (retval != OF_SUCCESS)
            {
              OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Failed to push action=%d instruction list",action->action_type);
              status = OF_FAILURE;
              break;
            }
          }
          ofu_end_pushing_actions(msg, &starting_location, &length_of_actions);
          ((struct ofp_instruction_actions*)(msg->desc.ptr2))->len = htons(length_of_actions);
          break;
        }
      case OFPIT_CLEAR_ACTIONS: /* Clears all actions from the datapath action set */		   
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPIT_CLEAR_ACTIONS:");
          retval = ofu_push_clear_action_instruction(msg);
          if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Error in adding clear action instruction err = %d",retval);
            status = OF_FAILURE;
          }
          break;
        }
      case OFPIT_METER: /* Apply meter (rate limiter) */ 		  
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPIT_METER::data=%d",instruction->ui32_data);
          retval = ofu_push_meter_instruction(msg, instruction->ui32_data);
          if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "\nfailed to push meter instruction");
            status = OF_FAILURE;
          }
          break;
        }
#if 0         
      case OFPIT_EXPERIMENTER: /* Experimenter instruction */		   
        {
          break;
        }
#endif
      default:
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Invalid instruction");
          status = OF_FAILURE;
        }
    }
  }
  while(0);

  if ((status == OFU_INSTRUCTION_PUSH_SUCCESS) || (status == OF_SUCCESS))
    return OF_SUCCESS;
  else
    return status;
}


int32_t of_flow_push_action(struct of_msg *msg, 
    struct ofi_flow_action *action)
{  
  int32_t retval = OF_SUCCESS;
  int32_t status = OF_SUCCESS;

  do
  {
    switch(action->action_type)
    {
      case OFPAT_OUTPUT:  /* Output to switch port. */
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_OUTPUT");
        retval=ofu_push_output_action(msg, action->port_no, OFPCML_NO_BUFFER);
        if (retval != OFU_ACTION_PUSH_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"ofu_push_output_action failed");
          status=OF_FAILURE;
          break;
        }
        break;
      case OFPAT_COPY_TTL_OUT: /* Copy TTL "outwards" -- from next-to-outermost	 to outermost */
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_COPY_TTL_OUT");
        retval=
          ofu_push_copy_ttl_outward_action(msg);
        if (retval != OFU_ACTION_PUSH_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"ofu_push_copy_ttl_outward_action failed");
          status=OF_FAILURE;
          break;
        }
        break;
      case OFPAT_COPY_TTL_IN	: /* Copy TTL "inwards" -- from outermost to	next-to-outermost */
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_COPY_TTL_IN:");
        retval=ofu_push_copy_ttl_inward_action(msg);
        if (retval != OFU_ACTION_PUSH_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"fu_push_inward_action failed");
          status=OF_FAILURE;
          break;
        }
        break;
      case OFPAT_SET_MPLS_TTL : /* MPLS TTL */
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_SET_MPLS_TTL");
        retval= ofu_push_set_mpls_ttl_action(msg, (uint8_t)action->ttl);
        if (retval != OFU_ACTION_PUSH_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"ofu_push_set_mpls_ttl_action failed");
          status=OF_FAILURE;
          break;
        }
        break;
      case OFPAT_DEC_MPLS_TTL : /* Decrement MPLS TTL */
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_DEC_MPLS_TTL");
        retval= ofu_push_dec_mpls_ttl_action(msg);
        if (retval != OFU_ACTION_PUSH_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"ofu_push_dec_mpls_ttl_action failed");
          status=OF_FAILURE;
          break;
        }
        break;
      case OFPAT_PUSH_VLAN  : /* Push a new VLAN tag */
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_PUSH_VLAN");

        retval=ofu_push_push_vlan_header_action(msg, (uint16_t)action->ether_type);
        if (retval != OFU_ACTION_PUSH_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"ofu_push_push_vlan_header_action failed");
          status=OF_FAILURE;
          break;
        }
        break;
      case OFPAT_POP_VLAN : /* Pop the outer VLAN tag */
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_POP_VLAN:");

        retval=ofu_push_pop_vlan_header_action(msg);
        if (retval != OFU_ACTION_PUSH_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"ofu_push_pop_vlan_header_action failed");
          status=OF_FAILURE;
          break;
        }
        break;
      case OFPAT_PUSH_MPLS: /* Push a new MPLS tag */
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"OFPAT_PUSH_MPLS:");

        retval=
          ofu_push_push_mpls_header_action(msg, (uint16_t)action->ether_type);
        if (retval != OFU_ACTION_PUSH_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"ofu_push_push_mpls_header_action failed");
          status=OF_FAILURE;
          break;
        }
        break;
      case OFPAT_POP_MPLS : /* Pop the outer MPLS tag */
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_POP_MPLS:");
        retval=
          ofu_push_pop_mpls_header_action(msg, (uint16_t)action->ether_type);
        if (retval != OFU_ACTION_PUSH_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"ofu_push_pop_mpls_header_action failed");
          status=OF_FAILURE;
          break;
        }
        break;
      case OFPAT_SET_QUEUE: /* Set queue id when outputting to a port */
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_SET_QUE:");
        retval=ofu_push_set_queue_action(msg, action->queue_id);
        if (retval != OFU_ACTION_PUSH_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"fu_push_queue_action failed");
          status=OF_FAILURE;
          break;
        }
        break;
      case OFPAT_GROUP : /* Apply group. */
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_GROUP:");
        retval=ofu_push_group_action(msg, action->group_id);
        if (retval != OFU_ACTION_PUSH_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"fu_push_group_action failed");
          status=OF_FAILURE;
          break;
        }
        break;
      case OFPAT_SET_NW_TTL : /* IP TTL. */
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," OFPAT_SET_NW_TTL:");
        retval=
          ofu_push_set_ipv4_ttl_action(msg, action->ttl);
        if (retval != OFU_ACTION_PUSH_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"ofu_push_set_ipv4_ttl_action failed");
          status=OF_FAILURE;
          break;
        }

        break;
      case OFPAT_DEC_NW_TTL: /* Decrement IP TTL. */
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," OFPAT_DEC_NW_TTL:");
        retval=ofu_push_dec_ipv4_ttl_action(msg);
        if (retval != OFU_ACTION_PUSH_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"ofu_push_dec_ipv4_ttl_action failed");
          status=OF_FAILURE;
          break;
        }
        break;

      case OFPAT_SET_FIELD: /* Set a header field using OXM TLV format. */
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," OFPAT_SET_FIELD:");
	retval = of_flow_set_action_setfield(msg,action);
        if (retval != OFU_ACTION_PUSH_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," of_flow_set_action_setfield failed!");
          status=OF_FAILURE;
          break;
        }
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s(%d) msg->desc.length3:%d",__FUNCTION__,__LINE__,msg->desc.length3);
        break;
      case OFPAT_PUSH_PBB: /* Push a new PBB service tag (I-TAG) */
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"OFPAT_PUSH_PBB:");
        retval=
          ofu_push_push_pbb_header_action(msg, (uint16_t)action->ether_type);
        if (retval != OFU_ACTION_PUSH_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"ofu_push_push_pbb_header_action failed");
          status=OF_FAILURE;
          break;
        }
        break;
      case	 OFPAT_POP_PBB: /* Pop the outer PBB service tag (I-TAG) */
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"OFPAT_POP_PBB:");
        retval=
          ofu_push_pop_pbb_header_action(msg);
        if (retval != OFU_ACTION_PUSH_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"ofu_push_pop_pbb_header_action failed");
          status=OF_FAILURE;
          break;
        }
        break;
    }

  }
  while(0);

  if (status == OFU_ACTION_PUSH_SUCCESS)
    return OF_SUCCESS;
  else
    return status;
}

int32_t of_flow_set_action_setfield(struct of_msg *msg,
    struct ofi_flow_action *action)
{
  int32_t retval ;
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL," of_flow_set_action_setfield Entered .setfield_type:%d",action->setfield_type);


  do
  {
    switch (action->setfield_type)
    {
      case OFPXMT_OFB_IN_PORT:  /* Switch input port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," OFPXMT_OFB_IN_PORT:");
          retval = ofu_push_set_port_in_set_field_action(msg, &action->ui32_data);
          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action field failed...");
            retval = OF_FAILURE;
          }
          break;
        }

      case OFPXMT_OFB_IN_PHY_PORT: /* Switch physical input port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IN_PHY_PORT:");
          retval = ofu_push_set_in_physical_port_in_set_field_action(msg, &action->ui32_data); 			 
          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_ETH_DST: /* Ethernet destination address. */			
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ETH_DST:");
          retval = ofu_push_set_destination_mac_in_set_field_action(msg, action->ui8_data_array);

          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting dst mac match field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_ETH_SRC: /* Ethernet source address. */
        { 
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ETH_SRC: ");
          retval = ofu_push_set_source_mac_in_set_field_action(msg, action->ui8_data_array);

          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting src mac action field failed...");
            retval = OF_FAILURE;
          } 			
          break;
        }
      case OFPXMT_OFB_ETH_TYPE: /* Ethernet frame type. */			
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ETH_TYPE: ");
          retval = ofu_push_set_eth_type_in_set_field_action(msg, &action->ui16_data);

          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting ethtype action field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_VLAN_VID: /* VLAN id. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_VLAN_VID: ");
          retval = ofu_push_set_vlan_id_in_set_field_action(msg, &action->ui16_data);

          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Setting  action failed...");
            retval = OF_FAILURE;
          }
          break;
        }

      case OFPXMT_OFB_IP_PROTO: /* IP protocol. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IP_PROTO: ");
          retval = ofu_push_set_ip_protocol_in_set_field_action(msg, &action->ui8_data);

          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_IPV4_SRC: /* IPv4 source address. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV4_SRC: ");
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s(%d) msg->desc.length3:%d",__FUNCTION__,__LINE__,msg->desc.length3);
          retval = ofu_push_set_ipv4_src_addr_in_set_field_action(msg, &action->ui32_data);
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s(%d) msg->desc.length3:%d",__FUNCTION__,__LINE__,msg->desc.length3);

          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Setting action set  field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_IPV4_DST: /* IPv4 destination address. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV4_DST: ");
          retval = ofu_push_set_ipv4_dst_addr_in_set_field_action(msg, &action->ui32_data);

          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Setting action set  field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_TCP_SRC: /* TCP source port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_TCP_SRC: ");
          retval = ofu_push_set_tcp_src_port_in_set_field_action(msg, &action->ui16_data);

          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action set  field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_TCP_DST: /* TCP destination port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_TCP_DST: ");
          retval = ofu_push_set_tcp_dst_port_in_set_field_action(msg, &action->ui16_data);

          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action set  field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_UDP_SRC: /* UDP source port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_UDP_SRC: ");
          retval = ofu_push_set_udp_src_port_in_set_field_action(msg, &action->ui16_data);

          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action set field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_UDP_DST: /* UDP destination port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_UDP_DST: ");
          retval = ofu_push_set_udp_dst_port_in_set_field_action(msg, &action->ui16_data);

          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action set field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_SCTP_SRC: /* SCTP source port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_SCTP_SRC: ");
          retval = ofu_push_set_sctp_src_port_in_set_field_action(msg,&action->ui16_data);

          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action set  field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_SCTP_DST: /* SCTP destination port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_SCTP_DST: ");
          retval = ofu_push_set_sctp_dst_port_in_set_field_action(msg,&action->ui16_data);

          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action set  field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_ICMPV4_TYPE: /* ICMP type. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV4_TYPE:");
          retval = ofu_push_set_icmpv4_type_in_set_field_action(msg, &action->ui8_data);

          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action set  field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_ICMPV4_CODE: /* ICMP code. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV4_CODE: ");
          retval = ofu_push_set_icmpv4_code_in_set_field_action(msg, &action->ui8_data);

          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Setting action set  field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_ARP_OP: /* ARP opcode. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_OP: ");
          retval = ofu_push_set_arp_opcode_in_set_field_action(msg, &action->ui16_data);

          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Setting action set  field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_MPLS_LABEL: /* MPLS label. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_MPLS_LABEL: ");
          retval = ofu_push_set_mpls_label_in_set_field_action(msg, &action->ui32_data);
          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action set  field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_MPLS_TC: /* MPLS TC. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_MPLS_TC: ");
          retval = ofu_push_set_mpls_tc_in_set_field_action(msg, &action->ui8_data);
          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action set  field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_MPLS_BOS: /* MPLS BoS bit. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_MPLS_BOS: ");
          retval = ofu_push_set_mpls_bos_bits_in_set_field_action(msg, &action->ui8_data);
          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action set  field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_PBB_ISID: /* PBB I-SID. */
        { 
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_PBB_ISID:");
          retval = ofu_push_set_pbb_isid_in_set_field_action(msg, &action->ui8_data);
          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action set  field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_TUNNEL_ID: /* Logical Port Metadata. */
        { 
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_TUNNEL_ID:");
          retval = ofu_push_set_logical_port_meta_data_in_set_field_action(msg, &action->ui64_data);
          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, " Setting action set  field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
	case  OFPXMT_OFB_VLAN_PCP:
	{
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_VLAN_PCP:");
          retval = ofu_push_set_vlan_priority_bits_in_set_field_action(msg, &action->ui64_data);
          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, " Setting action set  field failed...");
            retval = OF_FAILURE;
          }
          break;
	}

      case OFPXMT_OFB_ARP_SPA: /* ARP source IPv4 address. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_SPA");
          retval = ofu_push_set_arp_source_ipv4_addr_in_set_field_action(msg, &action->ui32_data);
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting arp source ipv4 action field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_ARP_TPA: /* ARP target IPv4 address. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_TPA:");
          retval = ofu_push_set_arp_target_ipv4_addr_in_set_field_action(msg, &action->ui32_data);
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting arp dst ipv4 action field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_ARP_SHA: /* ARP source hardware address. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_SHA:");
          retval = ofu_push_set_arp_source_hw_addr_in_set_field_action(msg, action->ui8_data_array);
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting arp src mac action field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_ARP_THA: /* ARP target hardware address. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_THA:");
          retval = ofu_push_set_arp_target_hw_addr_in_set_field_action(msg, action->ui8_data_array);
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting arp dst mac action field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_IP_DSCP:  /* IP DSCP (6 bits in ToS field). */			  
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IP_DSCP:");
          retval = ofu_push_set_ip_dscp_bits_in_set_field_action(msg, &action->ui8_data);
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting ip DSCP action fieild failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_IP_ECN:  /* IP ECN (2 bits in ToS field). */			
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IP_ECN:");
          retval = ofu_push_set_ip_ecn_bits_in_set_field_action(msg, &action->ui8_data);
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting ip ECN action fieild failed...");
            retval = OF_FAILURE;
          }
          break;
        }
	case OFPXMT_OFB_IPV6_SRC:
	{
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IP_SRC:");
          retval = ofu_push_set_source_ipv6_addr_in_set_field_action(msg, &(action->ipv6_addr.ipv6Addr32));
          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting ipv6 src addr action fieild failed...");
            retval = OF_FAILURE;
          }
	  break;
	}
	case OFPXMT_OFB_IPV6_DST:
	{
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IP_DST:");
          retval = ofu_push_set_destination_ipv6_addr_in_set_field_action(msg, &(action->ipv6_addr.ipv6Addr32));
          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting ipv6 dest addr  action fieild failed...");
            retval = OF_FAILURE;
          }
	  break;
        }
        case OFPXMT_OFB_IPV6_FLABEL: /* IPv6 Flow Label */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_FLABEL:%x",action->ui32_data);
	  retval = ofu_push_set_ipv6_flow_label_in_set_field_action(msg, &action->ui32_data);
          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting ipv6 flow label  action field failed...");
            retval = OF_FAILURE;
          }
          break;
        }

        case OFPXMT_OFB_ICMPV6_TYPE: /* ICMPv6 type. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_ICMPV6_TYPE:");
          retval = ofu_push_set_icmpv6_type_in_set_field_action(msg, &action->ui8_data);
          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting icmpv6 type action field failed...");
            retval = OF_FAILURE;
          }
	  break;
	}
        case OFPXMT_OFB_ICMPV6_CODE: /* ICMPv6 code. */ 		   
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_ICMPV6_CODE:");
          retval = ofu_push_set_icmpv6_code_in_set_field_action(msg, &action->ui8_data);
          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Setting  icmpv6 code action field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
        case OFPXMT_OFB_IPV6_ND_TARGET: /* Target address for ND. */			
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV6_ND_TARGET ");
          retval = ofu_push_set_nd_target_ipv6_addr_in_set_field_action(msg, &action->ipv6_addr);
          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting  IPv6 target addr in ND action field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
        case OFPXMT_OFB_IPV6_ND_SLL: /*The source link-layer address option in an IPv6Neighbor Discovery message */			{
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_ND_SLL:");
          retval = ofu_push_set_nd_source_link_layer_addr_in_set_field_action(msg, action->ui8_data_array);

          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting source link layer action field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case  OFPXMT_OFB_IPV6_ND_TLL: /* The target link-layer address option in an IPv6Neighbor Discovery message */
        { 
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV6_ND_TLL: ");
          retval = ofu_push_set_nd_target_link_layer_addr_in_set_field_action(msg, action->ui8_data_array);

          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting target link layer action field failed...");
            retval = OF_FAILURE;
          } 			
          break;
        }
       case OFPXMT_OFB_IPV6_EXTHDR: /* IPv6 Extension Header pseudo-field */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV6_EXTHDR: ");
          retval = ofu_push_set_ipv6_exthdr_field_in_set_field_action(msg, &action->ui16_data);

          if (retval != OFU_ACTION_PUSH_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting ipv6 extension header action field failed...");
            retval = OF_FAILURE;
          } 			
          break;
        }
        
	
	
      default:
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Invalid action set  field Id");
          retval = OF_FAILURE;
        }
    }
  }while(0);

  return retval;
}


int32_t of_flow_set_match_field(struct of_msg *msg, 
    struct ofi_flow_match_fields *match_fields)
{
  int32_t retval = OF_SUCCESS;
  int32_t status = OF_SUCCESS;


  do
  {
    switch (match_fields->field_type)
    {
      case OFPXMT_OFB_IN_PORT:  /* Switch input port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," OFPXMT_OFB_IN_PORT:");
          retval = of_set_in_port(msg, &match_fields->ui32_data);
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_IN_PHY_PORT: /* Switch physical input port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IN_PHY_PORT:");
          retval = of_set_in_physical_port(msg, &match_fields->ui32_data); 			 
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_METADATA: /* Metadata passed between tables. */ 		   
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_METADATA:");

          retval = of_set_meta_data(msg, &match_fields->ui64_data,
              match_fields->is_mask_set,
              (uint64_t *)(match_fields->mask));

          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting meta data match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_ETH_DST: /* Ethernet destination address. */			
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ETH_DST:");
          retval = of_set_destination_mac(msg, match_fields->ui8_data_array,
              match_fields->is_mask_set, (uint8_t *)match_fields->mask);

          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting dst mac match field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_ETH_SRC: /* Ethernet source address. */
        { 
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ETH_SRC: ");
          retval = of_set_source_mac(msg, match_fields->ui8_data_array,
              match_fields->is_mask_set, (uint8_t *)match_fields->mask);

          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting src mac match field failed...");
            status = OF_FAILURE;
          } 			
          break;
        }
      case OFPXMT_OFB_ETH_TYPE: /* Ethernet frame type. */			
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ETH_TYPE: ");
          retval = of_set_ether_type(msg, &match_fields->ui16_data);

          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting ethtype match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_VLAN_VID: /* VLAN id. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_VLAN_VID: ");
          retval = of_set_vlan_id(msg, &match_fields->ui16_data, 0, NULL);

          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Setting match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_IP_PROTO: /* IP protocol. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IP_PROTO: ");
          retval = of_set_protocol(msg, &match_fields->ui8_data);

          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_IPV4_SRC: /* IPv4 source address. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV4_SRC: ");
          retval = of_set_ipv4_source(msg, &match_fields->ui32_data,
              match_fields->is_mask_set, 
              ((uint32_t *)match_fields->mask));

          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Setting match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_IPV4_DST: /* IPv4 destination address. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV4_DST: ");
          retval = of_set_ipv4_destination(msg, &match_fields->ui32_data,
              match_fields->is_mask_set, 
              ((uint32_t *)match_fields->mask));

          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Setting match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_TCP_SRC: /* TCP source port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_TCP_SRC: ");
          retval = of_set_tcp_source_port(msg, &match_fields->ui16_data);

          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_TCP_DST: /* TCP destination port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_TCP_DST: ");
          retval = of_set_tcp_destination_port(msg, &match_fields->ui16_data);

          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_UDP_SRC: /* UDP source port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_UDP_SRC: ");
          retval = of_set_udp_source_port(msg, &match_fields->ui16_data);

          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_UDP_DST: /* UDP destination port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_UDP_DST: ");
          retval = of_set_udp_destination_port(msg, &match_fields->ui16_data);

          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_SCTP_SRC: /* SCTP source port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_SCTP_SRC: ");
          retval = of_set_sctp_source_port(msg,&match_fields->ui16_data);

          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_SCTP_DST: /* SCTP destination port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_SCTP_DST: ");
          retval = of_set_sctp_destination_port(msg,&match_fields->ui16_data);

          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_ICMPV4_TYPE: /* ICMP type. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV4_TYPE:");
          retval = of_set_icmpv4_type(msg, &match_fields->ui8_data);

          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_ICMPV4_CODE: /* ICMP code. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV4_CODE: ");
          retval = of_set_icmpv4_code(msg, &match_fields->ui8_data);

          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Setting match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_ARP_OP: /* ARP opcode. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_OP: ");
          retval = of_set_arp_op_code(msg, &match_fields->ui16_data);

          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Setting match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_MPLS_LABEL: /* MPLS label. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_MPLS_LABEL: ");
          retval = of_set_mpls_label(msg, &match_fields->ui32_data);
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_MPLS_TC: /* MPLS TC. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_MPLS_TC: ");
          retval = of_set_mpls_tc(msg, &match_fields->ui8_data);
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_MPLS_BOS: /* MPLS BoS bit. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_MPLS_BOS: ");
          retval = of_set_mpls_bos(msg, &match_fields->ui8_data);
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_PBB_ISID: /* PBB I-SID. */
        { 
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_PBB_ISID:");
          retval = of_set_pbb_i_sid(msg, &match_fields->ui8_data,
              match_fields->is_mask_set,
              (uint8_t *)(match_fields->mask));
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_TUNNEL_ID: /* Logical Port Metadata. */
        { 
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_TUNNEL_ID:");
          retval = of_set_logical_port_meta_data(msg, &match_fields->ui64_data,
              match_fields->is_mask_set,
              (uint64_t *)(match_fields->mask));
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, " Setting match field failed...");
            status = OF_FAILURE;
          }
          break;
        }


      case OFPXMT_OFB_VLAN_PCP: /* VLAN priority. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_VLAN_PCP:");
          retval = of_set_vlan_priority_bits(msg, &match_fields->ui8_data);
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting match fieild failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_ARP_SPA: /* ARP source IPv4 address. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_SPA");
          retval = of_set_arp_source_ip_addr(msg, &match_fields->ui32_data,
              match_fields->is_mask_set,
              ((uint32_t *)match_fields->mask));
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting src ipv4 match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_ARP_TPA: /* ARP target IPv4 address. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_TPA:");
          retval = of_set_arp_target_ip_addr(msg, &match_fields->ui32_data,
              match_fields->is_mask_set,
              ((uint32_t *)match_fields->mask));
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting dst ipv4 address match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_ARP_SHA: /* ARP source hardware address. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_SHA:");
          retval = of_set_arp_source_hw_addr(msg, match_fields->ui8_data_array,
              match_fields->is_mask_set, (uint8_t *)match_fields->mask);

          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting arp src mac match field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_ARP_THA: /* ARP target hardware address. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_THA:");
          retval = of_set_arp_target_hw_addr(msg, match_fields->ui8_data_array,
              match_fields->is_mask_set, (uint8_t *)match_fields->mask);

          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting arp dst mac match field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_IP_DSCP:  /* IP DSCP (6 bits in ToS field). */			  
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IP_DSCP:");
          retval = of_set_ip_dscp_bits(msg, &match_fields->ui8_data);
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting match fieild failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_IP_ECN:  /* IP ECN (2 bits in ToS field). */			
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IP_ECN:");
          retval = of_set_ip_ecn_bits(msg, &match_fields->ui8_data);
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting match fieild failed...");
            status = OF_FAILURE;
          }
          break;
        }
#if 0 //will be uncommented once NBAPI implemented

      case OFPXMT_OFB_VLAN_PCP: /* VLAN priority. */
        {
          break;
        }
      case OFPXMT_OFB_IP_DSCP:  /* IP DSCP (6 bits in ToS field). */			  
        {
          break;
        }
      case OFPXMT_OFB_IP_ECN:  /* IP ECN (2 bits in ToS field). */			
        {
          break;
        }
      case OFPXMT_OFB_ARP_SPA: /* ARP source IPv4 address. */
        {
          break;
        }
      case OFPXMT_OFB_ARP_TPA: /* ARP target IPv4 address. */
        {
          break;
        }
      case OFPXMT_OFB_ARP_SHA: /* ARP source hardware address. */
        {
          break;
        }
      case OFPXMT_OFB_ARP_THA: /* ARP target hardware address. */
        {
          break;
        }
#endif
      case OFPXMT_OFB_IPV6_SRC: /* IPv6 source address. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV6_SRC: ");
          retval = of_set_ipv6_source(msg, &match_fields->ipv6_addr,
              match_fields->is_mask_set, ((ipv6_addr_t *)match_fields->mask));

          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Setting match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_IPV6_DST: /* IPv6 source address. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV6_DST: ");
          retval = of_set_ipv6_destination(msg, &match_fields->ipv6_addr,
              match_fields->is_mask_set, ((ipv6_addr_t *)match_fields->mask));

          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Setting match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_IPV6_FLABEL: /* IPv6 Flow Label */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_FLABEL:%x",match_fields->ui32_data);
	  retval = of_set_ipv6_flow_label(msg,&match_fields->ui32_data, 
			match_fields->is_mask_set, ((uint32_t *)match_fields->mask)); 	
          break;
        }
      case OFPXMT_OFB_ICMPV6_TYPE: /* ICMPv6 type. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_ICMPV6_TYPE:");
          retval = of_set_icmpv6_type(msg, &match_fields->ui8_data);
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
      case OFPXMT_OFB_ICMPV6_CODE: /* ICMPv6 code. */ 		   
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_ICMPV6_CODE:");
          retval = of_set_icmpv6_code(msg, &match_fields->ui8_data);
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Setting match field failed...");
            status = OF_FAILURE;
          }
          break;
        }

      case OFPXMT_OFB_IPV6_ND_TARGET: /* Target address for ND. */			
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV6_ND_TARGET ");
          retval = of_set_ipv6_nd_target(msg, &match_fields->ipv6_addr);

          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Setting match field failed...");
            status = OF_FAILURE;
          }
          break;
        }
    case OFPXMT_OFB_IPV6_ND_SLL: /*The source link-layer address option in an IPv6Neighbor Discovery message */			
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_ND_SLL:");
          retval = of_set_ipv6_nd_source_hw_addr(msg, match_fields->ui8_data_array);

          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting source link layer match field failed...");
            retval = OF_FAILURE;
          }
          break;
        }
      case  OFPXMT_OFB_IPV6_ND_TLL: /* The target link-layer address option in an IPv6Neighbor Discovery message */
        { 
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV6_ND_TLL: ");
          retval = of_set_ipv6_nd_target_hw_addr(msg, match_fields->ui8_data_array);

          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting target link layer mac match field failed...");
            status = OF_FAILURE;
          } 			
          break;
        }
      case OFPXMT_OFB_IPV6_EXTHDR: /* IPv6 Extension Header pseudo-field */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV6_EXTHDR: ");
          retval = of_set_ipv6_extension_hdr(msg, &match_fields->ui16_data,
              match_fields->is_mask_set,
              (uint16_t *)(match_fields->mask));

          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting target link layer mac match field failed...");
            status = OF_FAILURE;
          } 			
          break;
        }
 
#if 0
      case OFPXMT_OFB_IPV6_DST: /* IPv6 destination address. */
        {
          break;
        }
      case OFPXMT_OFB_IPV6_FLABEL: /* IPv6 Flow Label */
        {
          break;
        }
      case OFPXMT_OFB_ICMPV6_TYPE: /* ICMPv6 type. */
        {
          break;
        }
      case OFPXMT_OFB_ICMPV6_CODE: /* ICMPv6 code. */ 		   
        {
          break;
        }
      case OFPXMT_OFB_IPV6_ND_TARGET: /* Target address for ND. */			
        {
          break;
        }
      case OFPXMT_OFB_IPV6_ND_SLL: /* Source link-layer for ND. */			
        {
          break;
        }
      case OFPXMT_OFB_IPV6_ND_TLL: /* Target link-layer for ND. */			
        {
          break;
        }
      case OFPXMT_OFB_IPV6_EXTHDR: /* IPv6 Extension Header pseudo-field */
        {
          break;
        }
#endif
      default:
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Invalid match field Id");
          status = OF_FAILURE;
        }
    }
  }while(0);

  if (status == OFU_SET_FIELD_SUCCESS)
    return OF_SUCCESS;
  else
    return status;
}

int32_t of_flow_frame_message(struct of_msg *msg, uint8_t command,
    uint16_t priority,
    struct ofi_flow_mod_info *flow_entry_p)
{
  struct ofi_flow_match_fields *match_field;
  struct ofi_flow_action *action;
  struct ofi_flow_instruction *instruction;
  uint8_t starting_location;
  uint16_t length_of_instructions=0;
  uint16_t match_field_len=0;
  int32_t  retval = OF_SUCCESS;
  int32_t  status = OF_SUCCESS;


  if (msg == NULL)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"invalid message");
    return OF_FAILURE;
  }

  do
  { 
    ofu_start_setting_match_field_values(msg);
    if ((command == DEL_FLOW) && (!priority))
    {
      ofu_end_setting_match_field_values(msg, &starting_location,&length_of_instructions);
    }
   else if((flow_entry_p->command == OFPFC_ADD) &&
	   (OF_LIST_COUNT(flow_entry_p->match_field_list) >= 1) &&
	  (flow_entry_p->priority == 0)) 	
    {
	OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "priority=%d command = %d , match_count = %d",flow_entry_p->priority,flow_entry_p->command,OF_LIST_COUNT(flow_entry_p->match_field_list));
	status = OF_FLOWMOD_ERROR_INVALID_MISSENTRY;    
	return status;
    }
    else
    {
      OF_LIST_SCAN(flow_entry_p->match_field_list, match_field, struct ofi_flow_match_fields *, OF_FLOW_MATCH_FILED_LISTNODE_OFFSET)
      {
        retval = of_flow_set_match_field(msg, match_field);
        if (retval != OF_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting match field=%d failed",match_field->field_type);
          status = OF_FAILURE;
        }
      }
      ofu_end_setting_match_field_values(msg, &starting_location,&match_field_len);
    }

    ofu_start_pushing_instructions(msg,match_field_len);
    if((command == MODIFY_FLOW) || (command == ADD_FLOW))
    {
      OF_LIST_SCAN(flow_entry_p->instruction_list, instruction, struct ofi_flow_instruction *, OF_FLOW_INSTRUCTION_LISTNODE_OFFSET)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"calling push instruction");
	// check for lower table id for goToTable type
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"instruction type %d , ui8_data %d , flow table id %d ",instruction->instruction_type,instruction->ui8_data,flow_entry_p->table_id);
        if((instruction->instruction_type == OFPIT_GOTO_TABLE) && (instruction->ui8_data <= flow_entry_p->table_id))
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"ERROR :of gototable should always point to next table only");
          return OF_FLOWMOD_GOTOTABLE_INVALID;
        }

        retval = of_flow_push_instruction(msg, instruction);

        if (retval != OF_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"of_flow_push_instruction failed");
          status = OF_FAILURE;
        }
      }
    }        
    ofu_end_pushing_instructions(msg, &starting_location,
        &length_of_instructions); 
#if 0

    ofu_start_pushing_actions(msg);
    if((command == MODIFY_FLOW) || (command == ADD_FLOW))
    {
      OF_LIST_SCAN(flow_entry_p->action_list, action, struct ofi_flow_action *,offset)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," calling of_flow_push_action");
        retval = of_flow_push_action(msg, action);

        if (retval != OF_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"of_flow_push_action failed");
          status = OF_FAILURE;
        }
      }
    }
    ofu_end_pushing_actions(msg, &starting_location,
        &length_of_instructions); 
#endif
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s(%d) msg->desc.length3:%d",__FUNCTION__,__LINE__,msg->desc.length3)
  }while(0);
  return status;
}

int32_t of_flow_mod_frame_and_send_message(uint64_t datapath_handle, uint32_t flow_id,
    uint8_t  table_id, uint16_t priority,
    uint8_t command)
{
  struct of_msg *msg;
  uint16_t  msg_len;
  struct ofi_flow_mod_info *flow_entry_p = NULL;
  int32_t retval;
  int32_t status=OF_SUCCESS;
  struct dprm_datapath_entry *datapath_entry_p=NULL;
  uint16_t flags = 0;
  uint64_t cookie = 0, cookie_mask;
  void *conn_info_p;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"entered");
#if 0
  if (command == DEL_FLOW)
  {
    msg_len = OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN;
  }
  else
#endif
  {
    msg_len = 300;
  }

  msg = ofu_alloc_message(OFPT_FLOW_MOD, msg_len);
  if (msg == NULL)   
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"OF message alloc error ");
    status = OF_FAILURE;
    return status;
  }

  do
  {
    CNTLR_RCU_READ_LOCK_TAKE(); 
    retval= get_datapath_byhandle( datapath_handle,&datapath_entry_p);
    if (retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"datapath entry not found");
      status=OF_FAILURE;
      break;
    }

    OF_LIST_SCAN(datapath_entry_p->flow_table, flow_entry_p, struct ofi_flow_mod_info *, OF_FLOW_MOD_LISTNODE_OFFSET)
    {
      if(flow_entry_p->flow_id == flow_id)
      {
        /*
	cookie = (uint64_t)(OFU_FLOW_COOKIE_START + flow_id);
	*/
        //cookie_mask = (uint64_t)OFU_FLOW_COOKIE_MASK;
        cookie_mask = 0;
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"flow found");
        break;
      }
    }

    if(flow_entry_p == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"flow not found");
      status=OF_FAILURE;
      break;
    }
   if(flow_entry_p->cookie == 0){
	    flow_entry_p->cookie = cookie;
    }
    if(flow_entry_p->cookie_mask == 0){
	    flow_entry_p->cookie_mask = cookie_mask;
    }
   OF_LOG_MSG(OF_LOG_MOD,OF_LOG_DEBUG,"flow_flags are  %d", flow_entry_p->flow_flags);

    if(flow_entry_p->flow_flags == 0){
	    flow_entry_p->flow_flags = flags;
    }
   OF_LOG_MSG(OF_LOG_MOD,OF_LOG_DEBUG,"flow_flags changed to  %d", flow_entry_p->flow_flags);

    CNTLR_RCU_READ_LOCK_RELEASE();

 OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"flow_entry->idle_timeout=%d flow_entry->hard_timeout=%d ",flow_entry_p->idle_timeout,flow_entry_p->hard_timeout);
	         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "flowmod_Strict id=%d table_id=%d priority=%d command =%d  cookie=%llx mask=%llx", flow_entry_p->flow_id,flow_entry_p->table_id,flow_entry_p->priority,flow_entry_p->command,flow_entry_p->cookie,flow_entry_p->cookie_mask);

    switch(command)
    {
      case ADD_FLOW:
        {
          flow_entry_p->flow_flags |= OFPFF_RESET_COUNTS;

	         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "flowmod_Strict id=%d table_id=%d priority=%d command =%d  cookie=%llx mask=%llx", flow_entry_p->flow_id,flow_entry_p->table_id,flow_entry_p->priority,flow_entry_p->command,flow_entry_p->cookie,flow_entry_p->cookie_mask);

	         retval = of_create_add_flow_entry_msg(msg,
					               datapath_handle,
					               flow_entry_p->table_id,
					               flow_entry_p->priority,
					               flow_entry_p->command,
					               flow_entry_p->cookie,
					               flow_entry_p->cookie_mask,
					               OFP_NO_BUFFER,
					               flow_entry_p->flow_flags,
					               flow_entry_p->hard_timeout,
					               flow_entry_p->idle_timeout,
					               &conn_info_p
					                );
          if (retval != OFU_ADD_FLOW_ENTRY_TO_TABLE_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"addition of flow_mod for tableid=%d failed",flow_entry_p->table_id);
          }
          break;
        }
     #if 0
      case MODIFY_FLOW:
        {
          flags |= OFPFF_RESET_COUNTS;
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Mod flow ");

          if (priority)
          {
            retval = 
              of_create_modify_flow_entries_msg_of_table_with_strict_match(msg,
                  datapath_handle,
                  flow_entry_p->table_id,
                  priority,
                  flags,
                  cookie,
                  cookie_mask,
	      &conn_info_p);
            if (retval != OFU_MODIFY_FLOW_ENTRY_TO_TABLE_SUCCESS)
            {
             OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "modification of flow_mod for tableid=%d failed",flow_entry_p->table_id);
            }
          }
          else
          {
            retval = of_modify_flow_entries_of_table(msg,
                datapath_handle,
                flow_entry_p->table_id,
                flags,
                cookie,
                cookie_mask,
	      &conn_info_p);
            if (retval != OFU_MODIFY_FLOW_ENTRY_TO_TABLE_SUCCESS)
            {
              OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"modification of flow_mod for tableid=%d failed",flow_entry_p->table_id);
            }
          }
          break;
        }
      case DEL_FLOW:
        {
          flags |= OFPFF_SEND_FLOW_REM;
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Delete flow ");
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"deleting flowmod id=%d table_id=%d prio=%d mask=%x",
              flow_id,table_id,priority,
              cookie_mask);

          if (priority)
          {
            retval = of_create_delete_flow_entries_msg_of_table_with_strict_match(msg,
                datapath_handle,
                flow_entry_p->table_id,
                priority,
                flags,
                cookie,
                cookie_mask,
                OFPP_NONE,
                0,
	      &conn_info_p);
            if (retval != OFU_DELETE_FLOW_ENTRY_TO_TABLE_SUCCESS)
            {
              OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "deletion of flow_mod for tableid=%d failed",table_id);
            }
          }
          else
          {

            retval = of_create_delete_flow_entries_msg_of_table(msg,
                datapath_handle,
                flow_entry_p->table_id,
                flags,
                cookie,
                cookie_mask,
                OFPP_NONE,
                0,
	      &conn_info_p);
            if (retval != OFU_DELETE_FLOW_ENTRY_TO_TABLE_SUCCESS)
            {
              OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "deletion of flow_mod for tableid=%d failed",table_id);
            }
          }

          if(retval == OFU_DELETE_FLOW_ENTRY_TO_TABLE_SUCCESS)
          {
            retval=of_datapath_remove_static_flow(datapath_handle, flow_id);
            if(retval != OF_SUCCESS)
            {
              OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"removing static flow from dp handle failed");
              status = OF_FAILURE;
            }
          }

          break;
        }
     #endif
      default:
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"invalid command");
          status=OF_FAILURE;
        }
    }

    CNTLR_RCU_READ_LOCK_TAKE(); 
    retval= of_flow_frame_message(msg, command, priority, flow_entry_p);
    if(retval != OF_SUCCESS)
    {
     if(retval == OF_FLOWMOD_ERROR_INVALID_MISSENTRY){
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Invalid Missentry");
	status = OF_FLOWMOD_ERROR_INVALID_MISSENTRY;
	}
     else if(retval == OF_FLOWMOD_GOTOTABLE_INVALID){
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Invalid gototable entry");
        status = OF_FLOWMOD_GOTOTABLE_INVALID;
        }

      else {	
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"framing message failed");
      status=OF_FAILURE;
      }
      break;
    }
    CNTLR_RCU_READ_LOCK_RELEASE();

        retval=of_cntrlr_send_request_message(
        msg,
        datapath_handle,
        conn_info_p,
        NULL,
        NULL);
    if (retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error adding flow entry in route table = %d",retval);
      status = OF_FAILURE;
    }

  }while(0);

  if (status == OF_FAILURE)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"status failed");
    return OF_FAILURE;
  }
  return status;
}

unsigned char atox(char *cPtr)
{
  unsigned char ucbyte = 0;
  char str[] = "0123456789ABCDEF";
  unsigned int digit;
  char cCmp;

  if (strlen(cPtr) > 2)
  {
    *(cPtr + 2) = '\0';
  }

  while (*(cPtr))
  {
    if (isalpha(*cPtr) && islower(*cPtr))
    {
      cCmp = toupper(*cPtr);
    }
    else
    {
      cCmp = *(cPtr);
    }

    for (digit = 0; digit < (strlen(str)); digit++)
    {
      if (str[digit] == cCmp)
      {
        if (!(*(cPtr+1)))
          ucbyte += digit;
        else
          ucbyte += 16*digit;

        break;
      }
    }
    cPtr++;
  }
  return ucbyte;
}


uint64_t atox_64(char *cPtr)
{
  uint8_t ucbyte = 0;
  char char_byte[3] = {};
  uint64_t ui64_data = 0;
  char *data = cPtr;
  int ii;
  uint8_t isOddNum = 0;

  if (*(data) == '\0')
  {
    return 0;
  }
  isOddNum = strlen(data) % 2;

  for (ii = 0; ii < 8; ii++)
  {
    if ((ii==0) && (isOddNum))
    {
      strncat(char_byte, "0", 1);
      strncat(char_byte+1, data, 1);
      data += 1;
    }
    else
    {
      strncpy(char_byte, data, 2);
      data += 2;
    }
    ucbyte = atox(char_byte);

    ui64_data = ui64_data << 8;
    ui64_data |= ((uint64_t)ucbyte & 0x00000000000000ff);

    if (*(data) == '\0')
    {
      break;
    }
    ucbyte = 0;
  }

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "returning2 %llx",ui64_data);
  return ui64_data;
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "returning %llx",ui64_data);
}


uint32_t atox_32(char *cPtr)
{
  uint8_t ucbyte = 0;
  char char_byte[3] = {};
  uint32_t ui32_data = 0;
  char *data = cPtr;
  int ii;
  uint8_t isOddNum = 0;

  isOddNum = strlen(data) % 2;/* if odd no. of chars in the string*/

  for (ii = 0; ii < 4; ii++)
  {
    if ((ii==0) && (isOddNum))
    {
      strncat(char_byte, "0", 1);
      strncat(char_byte+1, data, 1);
      data += 1;
    }
    else
    {
      strncpy(char_byte, data, 2);
      data += 2;
    }
    ucbyte = atox(char_byte);
    ui32_data |= ((uint32_t)ucbyte << (8 * ((sizeof(uint32_t) - 1)-ii)));

    if (*(data) == '\0')
    {
      break;
    }
    ucbyte = 0;
  }

  ui32_data = (ui32_data >> (8 * ((sizeof(uint32_t) - 1) - ii)));
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "returning %lx",ui32_data);
  return ui32_data;
}

int32_t of_flow_match_atox_mac_addr(char *data, unsigned char *mac_addr)
{
  char *token;
  unsigned char ucByte;
  int ii = 0;

  token = strtok(data,":");

  while (token)
  {
    ucByte = (unsigned char)atox(token);
    memcpy((mac_addr+ii), &ucByte, sizeof(ucByte));
    token = (char *)strtok(NULL,":");
    ii++;
  }

  puts("\n");
  for(ii=0; ii<6; ii++)
    printf(" %x",*(mac_addr+ii));

  return 0;
}

int32_t of_flow_set_instruction_data_mask(struct ofi_flow_instruction *inst_info,
    char *data_mask, uint32_t mask_len)
{
  uint8_t is_mask_set = inst_info->is_mask_set;
  uint8_t *mask = inst_info->mask;
  uint8_t ucByte;
  char str[3] = {};
  uint64_t ui64_data = 0;
  int ii = 0;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Entered with mask %s",data_mask); 
  if (is_mask_set)
  {
    if ((*data_mask == '0') &&
        ((*(data_mask+1) == 'x') || (*(data_mask+1) == 'X')))
    {
      data_mask += 2;
    }

    ui64_data = atox_64(data_mask);
    memcpy(mask,&ui64_data,8);
    while (ii < mask_len)
    {
      strncpy(str, data_mask+ii, 2);
      ucByte = atox(str);
      ii += 2;
    }
    inst_info->mask_len = ii/2;
  }
  return OF_SUCCESS;
}

int32_t of_flow_set_match_field_mask(struct ofi_flow_match_fields *match_info,
    char *field_mask, uint32_t mask_len)
{
  uint64_t ui64_data = 0;
  uint8_t is_mask_set = match_info->is_mask_set;
  uint8_t *mask = match_info->mask;
  uint8_t ucByte;
  char str[3] = {};
  int ii = 0;
  uint32_t ui32_data = 0;
  ipv6_addr_t   ipv6_addr={};
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Entered with mask %s",field_mask); 

  if (is_mask_set)
  {
    if ((*field_mask == '0') &&
        ((*(field_mask+1) == 'x') || (*(field_mask+1) == 'X')))
    {
      field_mask += 2; 
    }

    if ((match_info->field_type == OFPXMT_OFB_IPV4_SRC) ||
        (match_info->field_type == OFPXMT_OFB_IPV4_DST)||
        (match_info->field_type == OFPXMT_OFB_ARP_SPA)||
	(match_info->field_type == OFPXMT_OFB_ARP_TPA))
    {
      ui32_data = inet_network(field_mask);
      memcpy(match_info->mask,&ui32_data,sizeof(ui32_data));
      match_info->mask_len = 4;
    }
    else if((match_info->field_type == OFPXMT_OFB_IPV6_SRC)||
	     (match_info->field_type == OFPXMT_OFB_IPV6_DST))
    {
          inet_pton(AF_INET6, field_mask, &(ipv6_addr.ipv6Addr32));
          memcpy(match_info->mask, &(ipv6_addr.ipv6Addr32), sizeof(ipv6_addr.ipv6Addr32));
      	  match_info->mask_len = 16;	
    }
    else if((match_info->field_type == OFPXMT_OFB_ARP_SHA)||
	    (match_info->field_type == OFPXMT_OFB_ARP_THA)||
	    (match_info->field_type == OFPXMT_OFB_ETH_DST)||
	    (match_info->field_type == OFPXMT_OFB_ETH_SRC))
    {
      ui64_data = atox_64(field_mask);
      ui64_data = (uint64_t)(ui64_data<<16);
      memcpy(mask,&ui64_data,sizeof(ui64_data));
      while (ii < mask_len)
      {
        strncpy(str, field_mask+ii, 2);
        ucByte = atox(str);
        ii += 2;
      }
      match_info->mask_len = ii/2;
     
    }		
    else
    {
      ui64_data = atox_64(field_mask);
      memcpy(mask,&ui64_data,sizeof(ui64_data));
      while (ii < mask_len)
      {
        strncpy(str, field_mask+ii, 2);
        ucByte = atox(str);
        ii += 2;
      }
      match_info->mask_len = ii/2;
      	
    }
  }
  return OF_SUCCESS;
}


int32_t of_flow_set_match_field_data(struct ofi_flow_match_fields *match_info,
    char *data, uint32_t data_len)
{
  int32_t retval = OF_SUCCESS;
  uint32_t field_type = match_info->field_type;

  do
  {
    switch (field_type)
    {
      case OFPXMT_OFB_IN_PORT: /* Switch input port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," OFPXMT_OFB_IN_PORT:");
          match_info->ui32_data = (uint32_t)atoi(data);
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "INPORT=%d",match_info->ui32_data);
          break;
        }
      case OFPXMT_OFB_IN_PHY_PORT: /* Switch physical input port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IN_PHY_PORT:");
          match_info->ui32_data = (uint32_t)atoi(data);
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "INPhy-port=%d",match_info->ui32_data);

          break;
        }
      case OFPXMT_OFB_METADATA: /* Metadata passed between tables. */			  
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_METADATA:");         
          if ((*data == '0') && ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            match_info->ui64_data = atox_64(data+2);
          }
          else
          {
            match_info->ui64_data = (uint64_t)atoll(data);
          }
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "metadata=%llx",match_info->ui64_data);
          break;
        }
      case OFPXMT_OFB_ETH_DST: /* Ethernet destination address. */ 		   
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ETH_DST:");
          if ((*data == '0') && ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            of_flow_match_atox_mac_addr(data+2, match_info->ui8_data_array);
          } 
          else
          {
            of_flow_match_atox_mac_addr(data, match_info->ui8_data_array);
          }
          break;
        }
      case OFPXMT_OFB_ETH_SRC: /* Ethernet source address. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ETH_SRC: ");
          if ((*data == '0') && 
              ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            of_flow_match_atox_mac_addr(data+2, match_info->ui8_data_array);
          } 
          else
          {
            of_flow_match_atox_mac_addr(data, match_info->ui8_data_array);
          }
          break;
        }
      case OFPXMT_OFB_ETH_TYPE: /* Ethernet frame type. */ 		   
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ETH_TYPE: ");
          if ((*data == '0') && 
              ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            match_info->ui16_data = (uint16_t)atox_32(data+2);
          }
          else
          {
            match_info->ui16_data = (uint16_t)atoi(data);
          }
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"ethtype=%x",match_info->ui16_data);
          break;
        }
      case OFPXMT_OFB_VLAN_VID: /* VLAN id. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_VLAN_VID: ");
          match_info->ui16_data = (uint16_t)atoi(data);
          break;
        }
      case OFPXMT_OFB_IP_PROTO: /* IP protocol. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IP_PROTO: ");
          match_info->ui8_data = (uint8_t)atoi(data);
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "protocol=%d",match_info->ui8_data);
          break;
        }
      case OFPXMT_OFB_IPV4_SRC: /* IPv4 source address. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV4_SRC: ");
          match_info->ui32_data = (uint32_t)inet_network(data);
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"src ip=%x",match_info->ui32_data);
          break;
        }
      case OFPXMT_OFB_IPV4_DST: /* IPv4 destination address. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV4_DST: ");
          match_info->ui32_data = (uint32_t)inet_network(data);
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "dest-ip=%x",match_info->ui32_data);
          break;
        }
      case OFPXMT_OFB_TCP_SRC: /* TCP source port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_TCP_SRC: ");
          match_info->ui16_data = (uint16_t)atoi(data);
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "tcp srcport=%d",match_info->ui16_data);
          break;
        }
      case OFPXMT_OFB_TCP_DST: /* TCP destination port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_TCP_DST: ");
          match_info->ui16_data = (uint16_t)atoi(data);
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "tcp destport=%d",match_info->ui16_data);
          break;
        }
      case OFPXMT_OFB_UDP_SRC: /* UDP source port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_UDP_SRC: ");
          match_info->ui16_data = (uint16_t)atoi(data);

          break;
        }
      case OFPXMT_OFB_UDP_DST: /* UDP destination port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_UDP_DST: ");
          match_info->ui16_data = (uint16_t)atoi(data);

          break;
        }
      case OFPXMT_OFB_SCTP_SRC: /* SCTP source port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_SCTP_SRC: ");
          match_info->ui16_data = (uint16_t)atoi(data);

          break;
        }
      case OFPXMT_OFB_SCTP_DST: /* SCTP destination port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_SCTP_DST: ");
          match_info->ui16_data = (uint16_t)atoi(data);

          break;
        }
      case OFPXMT_OFB_ICMPV4_TYPE: /* ICMP type. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV4_TYPE:");
          match_info->ui8_data = (uint8_t)atoi(data);

          break;
        }
      case OFPXMT_OFB_ICMPV4_CODE: /* ICMP code. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV4_CODE: ");
          match_info->ui8_data = (uint8_t)atoi(data);
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "icmp-code=%d",match_info->ui8_data);
          break;
        }
      case OFPXMT_OFB_ARP_OP: /* ARP opcode. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_OP: ");
          match_info->ui16_data = (uint16_t)atoi(data);
          break;
        }
      case OFPXMT_OFB_MPLS_LABEL: /* MPLS label. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_MPLS_LABEL:");
          match_info->ui32_data = (uint32_t)atoi(data);
          break;
        }
      case OFPXMT_OFB_MPLS_TC: /* MPLS TC. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_MPLS_TC:");
          match_info->ui8_data = (uint8_t)atoi(data);
          break;
        }
      case OFPXMT_OFB_MPLS_BOS: /* MPLS BoS bit. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_MPLS_BOS:");
          match_info->ui8_data = (uint8_t)atoi(data);
          break;
        }
      case OFPXMT_OFB_PBB_ISID: /* PBB I-SID. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_PBB_ISID:");
          match_info->ui8_data = (uint8_t)atoi(data);
          break;
        }
      case OFPXMT_OFB_TUNNEL_ID: /* Logical Port Metadata. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_TUNNEL_ID:");
          match_info->ui64_data = (uint64_t)atoll(data); 
          break;
        }
      case OFPXMT_OFB_VLAN_PCP:
	{
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_VLAN_PCP");
          match_info->ui8_data = (uint8_t)atoi(data);
          break;
	}
      case OFPXMT_OFB_ARP_SPA: /* IPv4 source address. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_SPA: ");
          match_info->ui32_data = (uint32_t)inet_network(data);
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"arp src ip=%x",match_info->ui32_data);
          break;
        }
      case OFPXMT_OFB_ARP_TPA: /* IPv4 source address. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_TPA: ");
          match_info->ui32_data = (uint32_t)inet_network(data);
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"arp dst ip=%x",match_info->ui32_data);
          break;
        }
       case OFPXMT_OFB_ARP_SHA:
	{
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_ARP_SHA: ");
          if ((*data == '0') && 
              ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            of_flow_match_atox_mac_addr(data+2, match_info->ui8_data_array);
          } 
          else
          {
            of_flow_match_atox_mac_addr(data, match_info->ui8_data_array);
          }
          break;
	}	
       case OFPXMT_OFB_ARP_THA:
	{
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_ARP_THA: ");
          if ((*data == '0') && 
              ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            of_flow_match_atox_mac_addr(data+2, match_info->ui8_data_array);
          } 
          else
          {
            of_flow_match_atox_mac_addr(data, match_info->ui8_data_array);
          }
          break;
	}
	case OFPXMT_OFB_IP_DSCP:
	{
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IP_DSCP: ");
          if ((*data == '0') && 
              ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            match_info->ui8_data = (uint8_t)atox_32(data+2);
          }
          else
          {
            match_info->ui8_data = (uint8_t)atoi(data);
          }
	  break;
	}
	case OFPXMT_OFB_IP_ECN:
	{
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IP_ECN: ");
          if ((*data == '0') && 
              ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            match_info->ui8_data = (uint8_t)atox_32(data+2);
          }
          else
          {
            match_info->ui8_data = (uint8_t)atoi(data);
          }
	  break;
	}
	case OFPXMT_OFB_IPV6_SRC:
	{
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_SRC: ");
          if(inet_pton(AF_INET6,data,&(match_info->ipv6_addr.ipv6Addr32)) != 1)
	  {
          	OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Invalid Ipv6 src address!. ");
	  }
	  break;
	}
	case OFPXMT_OFB_IPV6_DST:
	{
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_DST: ");
          if(inet_pton(AF_INET6,data,&(match_info->ipv6_addr.ipv6Addr32)) != 1)
	  {
          	OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Invalid Ipv6 dst address!. ");
	  }
	  break;
	}
        case OFPXMT_OFB_ICMPV6_TYPE: /* ICMP type. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV6_TYPE:");
          if ((*data == '0') && 
              ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            match_info->ui8_data = (uint8_t)atox_32(data+2);
          }
          else
          {
            match_info->ui8_data = (uint8_t)atoi(data);
          }
          break;
        }
      case OFPXMT_OFB_ICMPV6_CODE: /* ICMP code. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV6_CODE: ");
          if ((*data == '0') && 
              ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            match_info->ui8_data = (uint8_t)atox_32(data+2);
          }
          else
          {
            match_info->ui8_data = (uint8_t)atoi(data);
          }
          break;
        }
        case OFPXMT_OFB_IPV6_FLABEL:
	{
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_FLABEL");
          if ((*data == '0') && 
              ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            match_info->ui32_data = (uint32_t)atox_32(data+2);
          }
          else
          {
            match_info->ui32_data = (uint32_t)atoi(data);
          }
	  break;
	}
	case OFPXMT_OFB_IPV6_ND_TARGET:
	{
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_ND_TARGET: ");
          if(inet_pton(AF_INET6,data,&(match_info->ipv6_addr.ipv6Addr32)) != 1)
	  {
          	OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Invalid Ipv6  address!. ");
	  }
	  break;
	}

      case OFPXMT_OFB_IPV6_ND_SLL: /* The source link-layer address option in an IPv6Neighbor Discovery message.*/ 		       {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_ND_SLL:");
          if ((*data == '0') && ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            of_flow_match_atox_mac_addr(data+2, match_info->ui8_data_array);
          } 
          else
          {
            of_flow_match_atox_mac_addr(data, match_info->ui8_data_array);
          }
          break;
        }
      case OFPXMT_OFB_IPV6_ND_TLL: /* The target link-layer address option in an IPv6Neighbor Discovery message.*/
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," OFPXMT_OFB_IPV6_ND_TLL: ");
          if ((*data == '0') && 
              ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            of_flow_match_atox_mac_addr(data+2, match_info->ui8_data_array);
          } 
          else
          {
            of_flow_match_atox_mac_addr(data, match_info->ui8_data_array);
          }
          break;
        }
      case OFPXMT_OFB_IPV6_EXTHDR:
	{
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_EXTHDR: ");
          if ((*data == '0') && 
              ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            match_info->ui16_data = (uint16_t)atox_32(data+2);
          }
          else
          {
            match_info->ui16_data = (uint16_t)atoi(data);
          }
	  break;
	}
		
    }
  }
  while(0);

  return retval;
}


int32_t of_flow_set_action_data(struct ofi_flow_action *action_info,
    char *data, uint32_t data_len)
{
  switch (action_info->action_type)
  {
    case OFPAT_OUTPUT :
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_OUTPUT : ");
        if ((*data == '0') &&
            ((*(data+1) == 'x') || (*(data+1) == 'X')))
        {
          action_info->port_no = (uint32_t)atox_64(data+2);
        }
        else
        {
          action_info->port_no = (uint32_t)atoi(data);
        }
        break;
      }
    case OFPAT_COPY_TTL_OUT:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_COPY_TTL_OUT: ");
        break;
      }
    case OFPAT_COPY_TTL_IN:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_COPY_TTL_IN:");
        break;
      }
    case OFPAT_SET_MPLS_TTL:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_SET_MPLS_TTL:");
        action_info->ttl = (uint8_t)atoi(data);
        break;
      }
    case OFPAT_DEC_MPLS_TTL:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_DEC_MPLS_TTL:");
        break;
      }
    case OFPAT_PUSH_VLAN:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_PUSH_VLAN:");
        if ((*data == '0') && ((*(data+1) == 'x') || (*(data+1) == 'X')))
        {
          action_info->ether_type = (uint16_t)atox_32(data+2);
        }
        else
        {
          action_info->ether_type = (uint16_t)atoi(data);
        }
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"ethertype=%x",action_info->ether_type);
        break;
      }
    case OFPAT_POP_VLAN:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_POP_VLAN:");
        break;
      }
    case OFPAT_PUSH_MPLS:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_PUSH_MPLS:");
        if ((*data == '0') && ((*(data+1) == 'x') || (*(data+1) == 'X')))
        {
          action_info->ether_type = (uint16_t)atox_32(data+2);
        }
        else
        {
          action_info->ether_type = (uint16_t)atoi(data);
        }
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"ethertype=%x",action_info->ether_type);
        break;
      }
    case OFPAT_POP_MPLS:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_POP_MPLS:");
        if ((*data == '0') && ((*(data+1) == 'x') || (*(data+1) == 'X')))
        {
          action_info->ether_type = (uint16_t)atox_32(data+2);
        }
        else
        {
          action_info->ether_type = (uint16_t)atoi(data);
        }
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"ethertype=%x",action_info->ether_type);
        break;
      }
    case OFPAT_SET_QUEUE:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_SET_QUEUE: ");
        if ((*data == '0') &&
            ((*(data+1) == 'x') || (*(data+1) == 'X')))
        {
          action_info->queue_id = (uint32_t)atox_64(data+2);
        }
        else
        {
          action_info->queue_id = (uint32_t)atoi(data);
        }
        break;
      }
    case OFPAT_GROUP:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_GROUP: ");
        if ((*data == '0') &&
            ((*(data+1) == 'x') || (*(data+1) == 'X')))
        {
          action_info->group_id = (uint32_t)atox_64(data+2);
        }
        else
        {
          action_info->group_id = (uint32_t)atoi(data);
        }
        break;
      }
    case OFPAT_DEC_NW_TTL:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_DEC_NW_TTL: ");
        break;
      }
    case OFPAT_SET_NW_TTL:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_SET_NW_TTL: ");
        action_info->ttl = (uint8_t)atoi(data);
        break;
      }
    case OFPAT_PUSH_PBB:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_PUSH_PBB:");
        if ((*data == '0') && ((*(data+1) == 'x') || (*(data+1) == 'X')))
        {
          action_info->ether_type = (uint16_t)atox_32(data+2);
        }
        else
        {
          action_info->ether_type = (uint16_t)atoi(data);
        }
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"ethertype=%x",action_info->ether_type);
        break;
      }
    case OFPAT_POP_PBB:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_POP_PBB:")
        break;
      }
    default:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "unknow action type %d", action_info->action_type);
        return OF_FAILURE;
      }
  }

  return OF_SUCCESS;
}

int32_t of_flow_set_action_setfield_type(struct ofi_flow_action *action_info,
    char *setfield_type, uint32_t setfield_len)
{
   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL,"of_flow_set_action_setfield_type Entered & set_field_type:%s ",setfield_type);

  if (!strncmp(setfield_type, "srcIpV4Addr", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_IPV4_SRC;
  }
  else if (!strncmp(setfield_type, "dstIpV4Addr", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_IPV4_DST;
  }
  else if (!strncmp(setfield_type, "udpSrcPort", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_UDP_SRC;
  }
  else if (!strncmp(setfield_type, "udpDstPort", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_UDP_DST;
  }
  else if (!strncmp(setfield_type, "tcpSrcPort", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_TCP_SRC;
  }
  else if (!strncmp(setfield_type, "tcpDstPort", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_TCP_DST;
  }
  else if (!strncmp(setfield_type, "sctpSrcPort", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_SCTP_SRC;
  }
  else if (!strncmp(setfield_type, "sctpDstPort", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_SCTP_DST;
  }
  else if (!strncmp(setfield_type, "protocol", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_IP_PROTO;
  }
  else if (!strncmp(setfield_type, "ethType", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_ETH_TYPE;
  }
  else if (!strncmp(setfield_type, "srcMacAddr", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_ETH_SRC;
  }
  else if (!strncmp(setfield_type, "dstMacAddr", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_ETH_DST;
  }
  else if (!strncmp(setfield_type, "icmpType", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_ICMPV4_TYPE;
  }
  else if (!strncmp(setfield_type, "icmpCode", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_ICMPV4_CODE;
  }
  else if (!strncmp(setfield_type, "inPort", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_IN_PORT;
  }
  else if (!strncmp(setfield_type, "inPhyPort", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_IN_PHY_PORT;
  }
  else if (!strncmp(setfield_type, "tableMetaData", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_METADATA;
  }
  else if (!strncmp(setfield_type, "mplsLabel", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_MPLS_LABEL;
  }
  else if (!strncmp(setfield_type, "mplsTC", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_MPLS_TC;
  }
  else if (!strncmp(setfield_type, "mplsBos", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_MPLS_BOS;
  }
  else if (!strncmp(setfield_type, "pbbIsid", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_PBB_ISID;
  }
  else if (!strncmp(setfield_type, "tunnelId", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_TUNNEL_ID;
  }
  else if (!strncmp(setfield_type, "VlanId", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_VLAN_VID;
  }
  else if (!strncmp(setfield_type, "VlanPriority", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_VLAN_PCP;
  }
  else if (!strncmp(setfield_type, "arpSrcIpv4Addr", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_ARP_SPA;
  }
  else if (!strncmp(setfield_type, "arpDstIpv4Addr", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_ARP_TPA;
  }
  else if (!strncmp(setfield_type, "arpSrcMacAddr", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_ARP_SHA;
  }
  else if (!strncmp(setfield_type, "arpDstMacAddr", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_ARP_THA;
  }
  else if (!strncmp(setfield_type, "arpOpcode", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_ARP_OP;
  }
  else if (!strncmp(setfield_type, "IpDiffServCodePointBits", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_IP_DSCP;
  }
  else if (!strncmp(setfield_type, "IpECNBits", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_IP_ECN;
  }
  else if (!strncmp(setfield_type, "srcIpv6Addr", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_IPV6_SRC;
  }
  else if (!strncmp(setfield_type, "dstIpv6Addr", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_IPV6_DST;
  }
  else if (!strncmp(setfield_type, "icmpv6Type", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_ICMPV6_TYPE;
  }
  else if (!strncmp(setfield_type, "icmpv6Code", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_ICMPV6_CODE;
  }
  else if (!strncmp(setfield_type, "ipv6NDTarget", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_IPV6_ND_TARGET;
  }
  else if (!strncmp(setfield_type, "ipv6NDSrcLinkLayerAddr", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_IPV6_ND_SLL;
  }
  else if (!strncmp(setfield_type, "ipv6NDTargetLinkLayerAddr", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_IPV6_ND_TLL;
  }
  else if (!strncmp(setfield_type, "Ipv6flowlabel", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_IPV6_FLABEL;
  }
  else if (!strncmp(setfield_type, "ipv6ExtnHdrPseudofield", setfield_len))
  {
    action_info->setfield_type = OFPXMT_OFB_IPV6_EXTHDR;
  }
  else
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Invalide set field type ....");
    return OF_FAILURE;
  }
  return OF_SUCCESS;

}
int32_t of_flow_set_action_setfieldtype_value(struct ofi_flow_action *action_info,
    char *data, uint32_t data_len)
{
  int32_t retval = OF_SUCCESS;
  uint32_t field_type = action_info->setfield_type;
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL," of_flow_set_action_setfieldtype_value Entered & setfieldtype_value:%s",data);

  do
  {
    switch (field_type)
    {
      case OFPXMT_OFB_IN_PORT: /* Switch input port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," OFPXMT_OFB_IN_PORT:");
          action_info->ui32_data = (uint32_t)atoi(data);
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "INPORT=%d",action_info->ui32_data);
          break;
        }
      case OFPXMT_OFB_IN_PHY_PORT: /* Switch physical input port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IN_PHY_PORT:");
          action_info->ui32_data = (uint32_t)atoi(data);
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "INPhy-port=%d",action_info->ui32_data);

          break;
        }
      case OFPXMT_OFB_METADATA: /* Metadata passed between tables. */			  
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_METADATA:");         
          if ((*data == '0') && ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            action_info->ui64_data = atox_64(data+2);
          }
          else
          {
            action_info->ui64_data = (uint64_t)atoll(data);
          }
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "metadata=%llx",action_info->ui64_data);
          break;
        }
      case OFPXMT_OFB_ETH_DST: /* Ethernet destination address. */ 		   
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ETH_DST:");
          if ((*data == '0') && ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            of_flow_match_atox_mac_addr(data+2, action_info->ui8_data_array);
          } 
          else
          {
            of_flow_match_atox_mac_addr(data, action_info->ui8_data_array);
          }
          break;
        }
      case OFPXMT_OFB_ETH_SRC: /* Ethernet source address. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ETH_SRC: ");
          if ((*data == '0') && 
              ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            of_flow_match_atox_mac_addr(data+2, action_info->ui8_data_array);
          } 
          else
          {
            of_flow_match_atox_mac_addr(data, action_info->ui8_data_array);
          }
          break;
        }
      case OFPXMT_OFB_ETH_TYPE: /* Ethernet frame type. */ 		   
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ETH_TYPE: ");
          if ((*data == '0') && 
              ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            action_info->ui16_data = (uint16_t)atox_32(data+2);
          }
          else
          {
            action_info->ui16_data = (uint16_t)atoi(data);
          }
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"ethtype=%x",action_info->ui16_data);
          break;
        }
      case OFPXMT_OFB_VLAN_VID: /* VLAN id. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_VLAN_VID: ");
          action_info->ui16_data = (uint16_t)atoi(data);
          break;
        }
      case OFPXMT_OFB_IP_PROTO: /* IP protocol. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IP_PROTO: ");
          action_info->ui8_data = (uint8_t)atoi(data);
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "protocol=%d",action_info->ui8_data);
          break;
        }
      case OFPXMT_OFB_IPV4_SRC: /* IPv4 source address. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV4_SRC: ");
          action_info->ui32_data = (uint32_t)inet_network(data);
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"src ip=%x",action_info->ui32_data);
          break;
        }
      case OFPXMT_OFB_IPV4_DST: /* IPv4 destination address. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV4_DST: ");
          action_info->ui32_data = (uint32_t)inet_network(data);
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "dest-ip=%x",action_info->ui32_data);
          break;
        }
      case OFPXMT_OFB_TCP_SRC: /* TCP source port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_TCP_SRC: ");
          action_info->ui16_data = (uint16_t)atoi(data);
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "tcp srcport=%d",action_info->ui16_data);
          break;
        }
      case OFPXMT_OFB_TCP_DST: /* TCP destination port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_TCP_DST: ");
          action_info->ui16_data = (uint16_t)atoi(data);
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "tcp destport=%d",action_info->ui16_data);
          break;
        }
      case OFPXMT_OFB_UDP_SRC: /* UDP source port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_UDP_SRC: ");
          action_info->ui16_data = (uint16_t)atoi(data);

          break;
        }
      case OFPXMT_OFB_UDP_DST: /* UDP destination port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_UDP_DST: ");
          action_info->ui16_data = (uint16_t)atoi(data);

          break;
        }
      case OFPXMT_OFB_SCTP_SRC: /* SCTP source port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_SCTP_SRC: ");
          action_info->ui16_data = (uint16_t)atoi(data);

          break;
        }
      case OFPXMT_OFB_SCTP_DST: /* SCTP destination port. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_SCTP_DST: ");
          action_info->ui16_data = (uint16_t)atoi(data);

          break;
        }
      case OFPXMT_OFB_ICMPV4_TYPE: /* ICMP type. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV4_TYPE:");
          action_info->ui8_data = (uint8_t)atoi(data);

          break;
        }
      case OFPXMT_OFB_ICMPV4_CODE: /* ICMP code. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV4_CODE: ");
          action_info->ui8_data = (uint8_t)atoi(data);
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "icmp-code=%d",action_info->ui8_data);
          break;
        }
      case OFPXMT_OFB_ARP_OP: /* ARP opcode. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_OP: ");
          action_info->ui16_data = (uint16_t)atoi(data);
          break;
        }
      case OFPXMT_OFB_MPLS_LABEL: /* MPLS label. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_MPLS_LABEL:");
          action_info->ui32_data = (uint32_t)atoi(data);
          break;
        }
      case OFPXMT_OFB_MPLS_TC: /* MPLS TC. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_MPLS_TC:");
          action_info->ui8_data = (uint8_t)atoi(data);
          break;
        }
      case OFPXMT_OFB_MPLS_BOS: /* MPLS BoS bit. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_MPLS_BOS:");
          action_info->ui8_data = (uint8_t)atoi(data);
          break;
        }
      case OFPXMT_OFB_PBB_ISID: /* PBB I-SID. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_PBB_ISID:");
          action_info->ui8_data = (uint8_t)atoi(data);
          break;
        }
      case OFPXMT_OFB_TUNNEL_ID: /* Logical Port Metadata. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_TUNNEL_ID:");
          action_info->ui64_data = (uint64_t)atoll(data); 
          break;
        }
      case OFPXMT_OFB_VLAN_PCP:
	{
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_VLAN_PCP");
          action_info->ui8_data = (uint8_t)atoi(data);
          break;
        }
      case OFPXMT_OFB_ARP_SPA: /* IPv4 source address. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_SPA: ");
          action_info->ui32_data = (uint32_t)inet_network(data);
          break;
        }
      case OFPXMT_OFB_ARP_TPA: /* IPv4 source address. */
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_TPA: ");
          action_info->ui32_data = (uint32_t)inet_network(data);
          break;
        }
       case OFPXMT_OFB_ARP_SHA:
	{
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_ARP_SHA: ");
          if ((*data == '0') && 
              ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            of_flow_match_atox_mac_addr(data+2, action_info->ui8_data_array);
          } 
          else
          {
            of_flow_match_atox_mac_addr(data, action_info->ui8_data_array);
          }
          break;
	}	
       case OFPXMT_OFB_ARP_THA:
	{
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_ARP_THA: ");
          if ((*data == '0') && 
              ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            of_flow_match_atox_mac_addr(data+2, action_info->ui8_data_array);
          } 
          else
          {
            of_flow_match_atox_mac_addr(data, action_info->ui8_data_array);
          }
          break;
	}
	case OFPXMT_OFB_IP_DSCP:
	{
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IP_DSCP: ");
          action_info->ui8_data = (uint8_t)atoi(data);
	  break;
	}
	case OFPXMT_OFB_IP_ECN:
	{
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IP_ECN: ");
          action_info->ui8_data = (uint8_t)atoi(data);
	  break;
	}
	case OFPXMT_OFB_IPV6_SRC:
	{
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_SRC");
          if(inet_pton(AF_INET6, data, &(action_info->ipv6_addr.ipv6Addr32)) != 1)
	  {
          	OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Invalid Ipv6 src address!. ");
		return OF_FAILURE;
	  }
	  break;
	}
	case OFPXMT_OFB_IPV6_DST:
	{
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_DST");
          if(inet_pton(AF_INET6, data, &(action_info->ipv6_addr.ipv6Addr32)) != 1)
	  {
          	OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Invalid Ipv6 dst address!. ");
		return OF_FAILURE;
	  }
	  break;
       }
       case OFPXMT_OFB_IPV6_FLABEL:
       {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_FLABEL ");
          if ((*data == '0') && 
              ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            action_info->ui32_data = (uint32_t)atox_32(data+2);
          }
          else
          {
            action_info->ui32_data = (uint32_t)atoi(data);
          }
	  break;
       }	
       case OFPXMT_OFB_ICMPV6_TYPE: /* ICMP type. */
       {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV6_TYPE:");
          if ((*data == '0') && 
              ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            action_info->ui8_data = (uint8_t)atox_32(data+2);
          }
          else
          {
            action_info->ui8_data = (uint8_t)atoi(data);
          }
          break;
       }
       case OFPXMT_OFB_ICMPV6_CODE: /* ICMP code. */
       {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV6_CODE: ");
          if ((*data == '0') && 
              ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            action_info->ui8_data = (uint8_t)atox_32(data+2);
          }
          else
          {
            action_info->ui8_data = (uint8_t)atoi(data);
          }
          action_info->ui8_data = (uint8_t)atoi(data);
          break;
       }
       case OFPXMT_OFB_IPV6_ND_TARGET:
       {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_ND_TARGET: ");
          
          if(inet_pton(AF_INET6, data, &(action_info->ipv6_addr.ipv6Addr32)) != 1)
	  {
          	OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Invalid Ipv6  address!. ");
	  }
	  break;
       }
       case OFPXMT_OFB_IPV6_ND_SLL: /* The source link-layer address option in an IPv6Neighbor Discovery message.*/ 		       {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_ND_SLL:");
          if ((*data == '0') && ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            of_flow_match_atox_mac_addr(data+2, action_info->ui8_data_array);
          } 
          else
          {
            of_flow_match_atox_mac_addr(data, action_info->ui8_data_array);
          }
          break;
        }
        case OFPXMT_OFB_IPV6_ND_TLL: /* The target link-layer address option in an IPv6Neighbor Discovery message.*/
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," OFPXMT_OFB_IPV6_ND_TLL: ");
          if ((*data == '0') && 
              ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            of_flow_match_atox_mac_addr(data+2, action_info->ui8_data_array);
          } 
          else
          {
            of_flow_match_atox_mac_addr(data, action_info->ui8_data_array);
          }
          break;
        }
        case OFPXMT_OFB_IPV6_EXTHDR:
	{
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_EXTHDR: ");
          if ((*data == '0') && 
              ((*(data+1) == 'x') || (*(data+1) == 'X')))
          {
            action_info->ui16_data = (uint16_t)atox_32(data+2);
          }
          else
          {
            action_info->ui16_data = (uint16_t)atoi(data);
          }
	  break;
	}		
    }
  }
  while(0);

  return retval;
}


int32_t of_flow_set_instruction_data(struct ofi_flow_instruction *inst_info,
    char *data, uint32_t data_len)
{


  switch (inst_info->instruction_type)
  {
    case OFPIT_GOTO_TABLE: /* Setup the next table in the lookup*/
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPIT_GOTO_TABLE: ");
        inst_info->ui8_data = (uint8_t)atoi(data);
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "go to table id = %d",inst_info->ui8_data);
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"\n #### go to table id = %d",inst_info->ui8_data);
        break;
      }
    case OFPIT_WRITE_METADATA:  /* Setup the metadata field for use later in*/
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPIT_WRITE_METADATA: ");
        if ((*data == '0') &&
            ((*(data+1) == 'x') || (*(data+1) == 'X')))
        {
          inst_info->ui64_data = (uint64_t)atox_64(data+2);
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"\n HEX VALUE meta_data_val = %llx",inst_info->ui64_data);
        }
        else
        {
          inst_info->ui64_data = (uint64_t)atoll(data);
        }
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "meta_data_val = %llx",inst_info->ui64_data);
        break;
      }
#if 0
    case OFPIT_APPLY_ACTIONS: /* Applies the action(s) immediately */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPIT_APPLY_ACTIONS: ");
      }
#endif
    case OFPIT_WRITE_ACTIONS:   /* Write the action(s) onto the datapath action */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPIT_WRITE_ACTIONS: ");

        break;
      }
    case OFPIT_CLEAR_ACTIONS: /* Clears all actions from the datapath */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPIT_CLEAR_ACTIONS:");
        break;
      }
    case OFPIT_METER:    /* Apply meter (rate limiter) */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPIT_METER:");
        inst_info->ui32_data = (uint32_t)atoi(data);
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"\n #### meter id = %d",inst_info->ui32_data);
        break;
      }
    default:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "unknown instruction type...");
        return OF_FAILURE; 
      }
  }
  return OF_SUCCESS;
}

int32_t of_flow_set_match_field_type(
    struct ofi_flow_match_fields *match_info,
    char *match_type, uint32_t match_len)
{


  if (!strncmp(match_type, "srcIpV4Addr", match_len))
  {
    match_info->field_type = OFPXMT_OFB_IPV4_SRC;
  }
  else if (!strncmp(match_type, "dstIpV4Addr", match_len))
  {
    match_info->field_type = OFPXMT_OFB_IPV4_DST;
  }
  else if (!strncmp(match_type, "udpSrcPort", match_len))
  {
    match_info->field_type = OFPXMT_OFB_UDP_SRC;
  }
  else if (!strncmp(match_type, "udpDstPort", match_len))
  {
    match_info->field_type = OFPXMT_OFB_UDP_DST;
  }  
  else if (!strncmp(match_type, "tcpSrcPort", match_len))
  {
    match_info->field_type = OFPXMT_OFB_TCP_SRC;
  }
  else if (!strncmp(match_type, "tcpDstPort", match_len))
  {
    match_info->field_type = OFPXMT_OFB_TCP_DST;
  }
  else if (!strncmp(match_type, "sctpSrcPort", match_len))
  {
    match_info->field_type = OFPXMT_OFB_SCTP_SRC;
  }
  else if (!strncmp(match_type, "sctpDstPort", match_len))
  {
    match_info->field_type = OFPXMT_OFB_SCTP_DST;
  }
  else if (!strncmp(match_type, "protocol", match_len))
  {
    match_info->field_type = OFPXMT_OFB_IP_PROTO;
  }
  else if (!strncmp(match_type, "ethType", match_len))  
  {
    match_info->field_type = OFPXMT_OFB_ETH_TYPE;
  }
  else if (!strncmp(match_type, "srcMacAddr", match_len))
  {
    match_info->field_type = OFPXMT_OFB_ETH_SRC;
  }   
  else if (!strncmp(match_type, "dstMacAddr", match_len))
  {
    match_info->field_type = OFPXMT_OFB_ETH_DST;
  }
  else if (!strncmp(match_type, "icmpType", match_len))
  {
    match_info->field_type = OFPXMT_OFB_ICMPV4_TYPE;
  }  
  else if (!strncmp(match_type, "icmpCode", match_len))
  {
    match_info->field_type = OFPXMT_OFB_ICMPV4_CODE;
  }
  else if (!strncmp(match_type, "inPort", match_len))
  {
    match_info->field_type = OFPXMT_OFB_IN_PORT;
  }
  else if (!strncmp(match_type, "inPhyPort", match_len))
  {
    match_info->field_type = OFPXMT_OFB_IN_PHY_PORT;
  }
  else if (!strncmp(match_type, "tableMetaData", match_len))
  {
    match_info->field_type = OFPXMT_OFB_METADATA;
  }
  else if (!strncmp(match_type, "mplsLabel", match_len))
  {
    match_info->field_type = OFPXMT_OFB_MPLS_LABEL;
  }
  else if (!strncmp(match_type, "mplsTC", match_len))
  {
    match_info->field_type = OFPXMT_OFB_MPLS_TC;
  }
  else if (!strncmp(match_type, "mplsBos", match_len))
  {
    match_info->field_type = OFPXMT_OFB_MPLS_BOS;
  }
  else if (!strncmp(match_type, "pbbIsid", match_len))
  {
    match_info->field_type = OFPXMT_OFB_PBB_ISID;
  }
  else if (!strncmp(match_type, "tunnelId", match_len))
  {
    match_info->field_type = OFPXMT_OFB_TUNNEL_ID;
  }
  else if (!strncmp(match_type, "VlanId", match_len))
  {
    match_info->field_type = OFPXMT_OFB_VLAN_VID;
  }
  else if (!strncmp(match_type, "VlanPriority", match_len))
  {
    match_info->field_type = OFPXMT_OFB_VLAN_PCP;
  }
  else if (!strncmp(match_type, "arpSrcIpv4Addr", match_len))
  {
    match_info->field_type = OFPXMT_OFB_ARP_SPA;
  }
  else if (!strncmp(match_type, "arpDstIpv4Addr", match_len))
  {
    match_info->field_type = OFPXMT_OFB_ARP_TPA;
  }
  else if (!strncmp(match_type, "arpSrcMacAddr", match_len))
  {
    match_info->field_type = OFPXMT_OFB_ARP_SHA;
  }
  else if (!strncmp(match_type, "arpDstMacAddr", match_len))
  {
    match_info->field_type = OFPXMT_OFB_ARP_THA;
  }
  else if (!strncmp(match_type, "arpOpcode", match_len))
  {
    match_info->field_type = OFPXMT_OFB_ARP_OP;
  }
  else if (!strncmp(match_type, "IpDiffServCodePointBits", match_len))
  {
    match_info->field_type = OFPXMT_OFB_IP_DSCP;
  }
  else if (!strncmp(match_type, "IpECNBits", match_len))
  {
    match_info->field_type = OFPXMT_OFB_IP_ECN;
  }
  else if (!strncmp(match_type, "srcIpv6Addr", match_len))
  {
    match_info->field_type = OFPXMT_OFB_IPV6_SRC;
  }
  else if (!strncmp(match_type, "dstIpv6Addr", match_len))
  {
    match_info->field_type = OFPXMT_OFB_IPV6_DST;
  }
  else if (!strncmp(match_type, "icmpv6Type", match_len))
  {
    match_info->field_type = OFPXMT_OFB_ICMPV6_TYPE;
  }
  else if (!strncmp(match_type, "icmpv6Code", match_len))
  {
    match_info->field_type = OFPXMT_OFB_ICMPV6_CODE;
  }
  else if (!strncmp(match_type, "ipv6NDTarget", match_len))
  {
    match_info->field_type = OFPXMT_OFB_IPV6_ND_TARGET;
  }
  else if (!strncmp(match_type, "ipv6NDSrcLinkLayerAddr", match_len))
  {
    match_info->field_type = OFPXMT_OFB_IPV6_ND_SLL;
  }
  else if (!strncmp(match_type, "ipv6NDTargetLinkLayerAddr", match_len))
  {
    match_info->field_type = OFPXMT_OFB_IPV6_ND_TLL;
  }
  else if (!strncmp(match_type, "Ipv6flowlabel", match_len))
  {
    match_info->field_type = OFPXMT_OFB_IPV6_FLABEL;
  }
  else if (!strncmp(match_type, "ipv6ExtnHdrPseudofield", match_len))
  {
    match_info->field_type = OFPXMT_OFB_IPV6_EXTHDR;
  }
  else
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Invalide match field type ....");
    return OF_FAILURE; 
  }
  return OF_SUCCESS; 
}


int32_t of_flow_set_instruction_type(
    struct ofi_flow_instruction *instruction_info,
    char *instruction_type, uint32_t instruction_len)
{

  if (!strncmp(instruction_type, "applyAction", instruction_len))
  {
    instruction_info->instruction_type= OFPIT_APPLY_ACTIONS;
  }
  else if (!strncmp(instruction_type, "writeAction", instruction_len))
  {
    instruction_info->instruction_type= OFPIT_WRITE_ACTIONS;
  }
  else if (!strncmp(instruction_type, "clearActions", instruction_len))
  {
    instruction_info->instruction_type= OFPIT_CLEAR_ACTIONS;
  }
  else if (!strncmp(instruction_type, "goToTable", instruction_len))
  {
    instruction_info->instruction_type= OFPIT_GOTO_TABLE;
  }
  else if (!strncmp(instruction_type, "writeMetaData", instruction_len))
  {
    instruction_info->instruction_type= OFPIT_WRITE_METADATA;
  }
  else if(!strncmp(instruction_type, "applyMeter", instruction_len))
  {
    instruction_info->instruction_type= OFPIT_METER;
  }
  else
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Invalid instruction type");
    return OF_FAILURE; 
  }

  return OF_SUCCESS; 
}


int32_t of_flow_set_action_type(
    struct ofi_flow_action *action_info,
    char *action_type, uint32_t action_len)
{

  if (!strncmp(action_type, "output", action_len))
  {
    action_info->action_type = OFPAT_OUTPUT;
  }
  else if (!strncmp(action_type, "copyTTLout", action_len))
  {
    action_info->action_type= OFPAT_COPY_TTL_OUT;
  }
  else if (!strncmp(action_type, "copyTTLin", action_len))
  {
    action_info->action_type= OFPAT_COPY_TTL_IN;
  }
  else if (!strncmp(action_type, "setMPLSTTL", action_len))
  {
    action_info->action_type= OFPAT_SET_MPLS_TTL;
  }
  else if (!strncmp(action_type, "decMPLSTTL", action_len))
  {
    action_info->action_type= OFPAT_DEC_MPLS_TTL;
  }
  else if (!strncmp(action_type, "pushVLAN", action_len))
  {
    action_info->action_type= OFPAT_PUSH_VLAN;
  }
  else if (!strncmp(action_type, "popVLAN", action_len))
  {
    action_info->action_type= OFPAT_POP_VLAN;
  }
  else if (!strncmp(action_type, "pushMPLS", action_len))
  {
    action_info->action_type= OFPAT_PUSH_MPLS;
  }
  else if (!strncmp(action_type, "popMPLS", action_len))
  {
    action_info->action_type= OFPAT_POP_MPLS;
  }
  else if (!strncmp(action_type, "applyGroup", action_len))
  {
    action_info->action_type= OFPAT_GROUP;
  }  
  else if (!strncmp(action_type, "setTTL", action_len))
  {
    action_info->action_type= OFPAT_SET_NW_TTL;
  }
  else if (!strncmp(action_type, "decTTL", action_len))
  {
    action_info->action_type= OFPAT_DEC_NW_TTL;
  }
  else if (!strncmp(action_type, "setQueue", action_len))
  {
    action_info->action_type= OFPAT_SET_QUEUE;
  }
  else if (!strncmp(action_type, "setField", action_len))
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL,"actin type:setfield");
    action_info->action_type = OFPAT_SET_FIELD;
    action_info->setfield_type = -1;
  }
  else if (!strncmp(action_type, "pushPBB", action_len))
  {
    action_info->action_type= OFPAT_PUSH_PBB;
  }
  else if (!strncmp(action_type, "popPBB", action_len))
  {
    action_info->action_type= OFPAT_POP_PBB;
  }
  else
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Unknown action type ....");
    return OF_FAILURE;
  }

  return OF_SUCCESS;  
}

int32_t of_get_flow(uint64_t datapath_handle,  uint32_t flow_id,
    struct ofi_flow_mod_info **flow_entry_pp)
{
  struct ofi_flow_mod_info *flow_entry_p;
  struct   dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;


  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"entered");

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle( datapath_handle,&datapath_entry_p);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:datapath entry not found",__FUNCTION__);
      status=OF_FAILURE;
      break;
    }
    OF_LIST_SCAN(datapath_entry_p->flow_table, flow_entry_p, struct ofi_flow_mod_info *,OF_FLOW_MOD_LISTNODE_OFFSET)
    {
      if (flow_entry_p->flow_id == flow_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s: flow found ",__FUNCTION__);
        status=OF_SUCCESS;
        break;
      }
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  if (status == OF_SUCCESS )
  {
    *flow_entry_pp = flow_entry_p;
  }
  return status;
}

int32_t of_get_flow_inst(uint64_t datapath_handle,  uint32_t flow_id, uint32_t inst_id,
    struct ofi_flow_instruction **inst_entry_pp)
{
  struct ofi_flow_mod_info *flow_entry_p;
  struct ofi_flow_instruction *inst_info;
  struct   dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;


  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"entered");

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle( datapath_handle,&datapath_entry_p);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s:datapath entry not found",__FUNCTION__);
      status=OF_FAILURE;
      break;
    }
    OF_LIST_SCAN(datapath_entry_p->flow_table, flow_entry_p, struct ofi_flow_mod_info *,OF_FLOW_MOD_LISTNODE_OFFSET)
    {
      if (flow_entry_p->flow_id == flow_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s: flow found ",__FUNCTION__);
        OF_LIST_SCAN(flow_entry_p->instruction_list, inst_info, struct ofi_flow_instruction *,OF_FLOW_INSTRUCTION_LISTNODE_OFFSET)
        {
          if (inst_info->inst_id == inst_id) 
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s: inst found ",__FUNCTION__);
            status=OF_SUCCESS;
            break;
          }
        }
        if (status == OF_SUCCESS)
          break;
      }
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  if (status == OF_SUCCESS)
  {
    *inst_entry_pp = inst_info;
  }
  return status;
}


int32_t of_get_flow_match_field(uint64_t datapath_handle,  uint32_t flow_id, uint32_t field_id,
    struct ofi_flow_match_fields **match_entry_pp)
{
  struct ofi_flow_mod_info *flow_entry_p;
  struct ofi_flow_match_fields *match_info;
  struct   dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;


  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"entered");

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle(datapath_handle,&datapath_entry_p);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:datapath entry not found",__FUNCTION__);
      status=OF_FAILURE;
      break;
    }
    OF_LIST_SCAN(datapath_entry_p->flow_table, flow_entry_p, struct ofi_flow_mod_info *,OF_FLOW_MOD_LISTNODE_OFFSET)
    {
      if (flow_entry_p->flow_id == flow_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s: flow found ",__FUNCTION__);
        OF_LIST_SCAN(flow_entry_p->match_field_list, match_info, struct ofi_flow_match_fields *,OF_FLOW_MATCH_FILED_LISTNODE_OFFSET)
        {
          if (match_info->field_id == field_id) 
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s: field found ",__FUNCTION__);
            status=OF_SUCCESS;
            break;
          }
        }
        if (status == OF_SUCCESS)
          break;
      }
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  if (status == OF_SUCCESS)
  {
    *match_entry_pp = match_info;
  }
  return status;
}



int32_t of_get_flow_action_from_actionlist(uint64_t datapath_handle,  uint32_t flow_id,
    uint32_t inst_id, uint32_t action_id,
    struct ofi_flow_action **action_entry_pp)
{
  struct ofi_flow_mod_info *flow_entry_p;
  struct ofi_flow_instruction *inst_info;
  struct ofi_flow_action *action_info;
  struct   dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;


  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"entered");

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle( datapath_handle,&datapath_entry_p);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s:datapath entry not found",__FUNCTION__);
      status=OF_FAILURE;
      break;
    }

    OF_LIST_SCAN(datapath_entry_p->flow_table, flow_entry_p, struct ofi_flow_mod_info *,OF_FLOW_MOD_LISTNODE_OFFSET)
    {
      if (flow_entry_p->flow_id == flow_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s: flow found ",__FUNCTION__);
        OF_LIST_SCAN(flow_entry_p->instruction_list, inst_info, struct ofi_flow_instruction *,OF_FLOW_INSTRUCTION_LISTNODE_OFFSET)
        {
          if (inst_info->inst_id == inst_id) 
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s: inst found ",__FUNCTION__);
            OF_LIST_SCAN(inst_info->action_list, action_info, struct ofi_flow_action *,OF_FLOW_ACTION_LISTNODE_OFFSET)
            {
              if (action_info->action_id == action_id)
              {
                OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s: action found ",__FUNCTION__);
                status=OF_SUCCESS;
                break;
              }
            }
            if (status == OF_SUCCESS)
              break;
          }
        }
        if (status == OF_SUCCESS)
          break;
      }
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  if (status == OF_SUCCESS)
  {
    *action_entry_pp = action_info;
  }
  return status;
}

int32_t of_get_flow_action(uint64_t datapath_handle,  uint32_t flow_id, uint32_t action_id,
    struct ofi_flow_action **action_entry_pp)
{
  struct ofi_flow_mod_info *flow_entry_p;
  struct ofi_flow_action *action_info;
  struct   dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;


  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"entered");

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle(datapath_handle,&datapath_entry_p);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s:datapath entry not found",__FUNCTION__);
      status=OF_FAILURE;
      break;
    }
    OF_LIST_SCAN(datapath_entry_p->flow_table, flow_entry_p, struct ofi_flow_mod_info *,OF_FLOW_MOD_LISTNODE_OFFSET)
    {
      if (flow_entry_p->flow_id == flow_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s: flow found ",__FUNCTION__);
        OF_LIST_SCAN(flow_entry_p->action_list, action_info, struct ofi_flow_action *,OF_FLOW_ACTION_LISTNODE_OFFSET)
        {
          if (action_info->action_id == action_id) 
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s: action found ",__FUNCTION__);
            status=OF_SUCCESS;
            break;
          }
        }
        if (status == OF_SUCCESS)
          break;
      }
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  if (status == OF_SUCCESS)
  {
    *action_entry_pp = action_info;
  }
  return status;
}

void of_flow_copy_instruction(struct ofi_flow_instruction *dst_inst_info,
    struct ofi_flow_instruction *src_inst_info)
{
  dst_inst_info->instruction_type = src_inst_info->instruction_type;
  dst_inst_info->instruction_len = src_inst_info->instruction_len;

  dst_inst_info->ui8_data = src_inst_info->ui8_data;
  dst_inst_info->ui16_data = src_inst_info->ui16_data;
  dst_inst_info->ui32_data = src_inst_info->ui32_data;
  dst_inst_info->ui64_data = src_inst_info->ui64_data;
  memcpy(dst_inst_info->ui8_data_array, src_inst_info->ui8_data_array, 6);

  dst_inst_info->is_mask_set = src_inst_info->is_mask_set;
  dst_inst_info->mask_len = src_inst_info->mask_len;
  memcpy(dst_inst_info->mask,src_inst_info->mask, 8);
}


void of_flow_copy_action(struct ofi_flow_action *dst_action_info,
    struct ofi_flow_action *src_action_info)
{
  dst_action_info->action_type = src_action_info->action_type;
  dst_action_info->action_len = src_action_info->action_len;

  dst_action_info->group_id = src_action_info->group_id;
  dst_action_info->queue_id = src_action_info->queue_id;
  dst_action_info->port_no = src_action_info->port_no;
  dst_action_info->max_len = src_action_info->max_len;
  dst_action_info->ether_type = src_action_info->ether_type;
  dst_action_info->ttl = src_action_info->ttl;
}


int32_t of_flow_update_inst(uint64_t datapath_handle,  uint32_t flow_id,
    struct ofi_flow_instruction *inst_entry_p)
{
  struct ofi_flow_mod_info *flow_entry;
  struct ofi_flow_instruction *inst_info;
  struct dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;


  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"entered");

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval = get_datapath_byhandle(datapath_handle,&datapath_entry_p);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:datapath entry not found",__FUNCTION__);
      status=OF_FAILURE;
      break;
    }
    OF_LIST_SCAN(datapath_entry_p->flow_table, flow_entry, struct ofi_flow_mod_info *,OF_FLOW_MOD_LISTNODE_OFFSET)
    {
      if (flow_entry->flow_id == flow_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s: flow found ",__FUNCTION__);
        OF_LIST_SCAN(flow_entry->instruction_list, inst_info, struct ofi_flow_instruction *,OF_FLOW_INSTRUCTION_LISTNODE_OFFSET)
        {
          if (inst_info->inst_id == inst_entry_p->inst_id) 
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s: inst found ",__FUNCTION__);
            of_flow_copy_instruction(inst_info, inst_entry_p);
            status=OF_SUCCESS;
            break;
          }
        }

        if (status == OF_SUCCESS)
        {
          break;
        }
        else
        {
          inst_info = 
            (struct ofi_flow_instruction *)calloc(1, sizeof(struct ofi_flow_instruction)); 
          if (inst_info == NULL)
            break;
          of_flow_copy_instruction(inst_info, inst_entry_p);
          OF_APPEND_NODE_TO_LIST(flow_entry->instruction_list,inst_info,OF_FLOW_INSTRUCTION_LISTNODE_OFFSET);
          status = OF_SUCCESS;
          break;
        }
      }
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  return status;
}


int32_t of_flow_update_actionset(uint64_t datapath_handle,
    uint32_t flow_id,
    struct ofi_flow_action *action_entry_p)
{
  struct ofi_flow_mod_info *flow_entry;
  struct ofi_flow_action *action_info;
  struct dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;


  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"entered");
  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval = get_datapath_byhandle(datapath_handle,&datapath_entry_p);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s:datapath entry not found",__FUNCTION__);
      status=OF_FAILURE;
      break;
    }

    OF_LIST_SCAN(datapath_entry_p->flow_table, flow_entry, struct ofi_flow_mod_info *,OF_FLOW_MOD_LISTNODE_OFFSET)
    {
      if (flow_entry->flow_id == flow_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s: flow found ",__FUNCTION__);
        OF_LIST_SCAN(flow_entry->action_list, action_info, struct ofi_flow_action *,OF_FLOW_ACTION_LISTNODE_OFFSET)
        {
          if (action_info->action_id == action_entry_p->action_id) 
          {
            of_flow_copy_action(action_info, action_entry_p);
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s: action found ",__FUNCTION__);
            status=OF_SUCCESS;
            break;
          }
        }

        if (status == OF_SUCCESS)
        {
          break;
        }
        else
        {
          action_info = 
            (struct ofi_flow_action *)calloc(1, sizeof(struct ofi_flow_action)); 
          if (action_info == NULL)
            break;
          of_flow_copy_action(action_info, action_entry_p);
          OF_APPEND_NODE_TO_LIST(flow_entry->action_list, action_info,OF_FLOW_ACTION_LISTNODE_OFFSET);
          status = OF_SUCCESS;
          break;
        }
      }
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  return status;
}

int32_t of_flow_update_actionlist(uint64_t datapath_handle,
    uint32_t flow_id, uint32_t inst_id,
    struct ofi_flow_action *action_entry_p)
{
  struct ofi_flow_mod_info *flow_entry;
  struct ofi_flow_action *action_info;
  struct ofi_flow_instruction *inst_info;
  struct dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;


  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"entered");
  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval = get_datapath_byhandle(datapath_handle, &datapath_entry_p);
    if (retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s:datapath entry not found",__FUNCTION__);
      status=OF_FAILURE;
      break;
    }

    OF_LIST_SCAN(datapath_entry_p->flow_table, flow_entry, struct ofi_flow_mod_info *,OF_FLOW_MOD_LISTNODE_OFFSET)
    {
      if (flow_entry->flow_id == flow_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s: flow found ",__FUNCTION__);
        OF_LIST_SCAN(flow_entry->instruction_list, inst_info, struct ofi_flow_instruction *,OF_FLOW_INSTRUCTION_LISTNODE_OFFSET)
        {
          if (inst_info->inst_id == inst_id) 
          {
            OF_LIST_SCAN(flow_entry->action_list, action_info, struct ofi_flow_action *,OF_FLOW_ACTION_LISTNODE_OFFSET)
            {
              if (action_info->action_id == action_entry_p->action_id)
              {
                of_flow_copy_action(action_info, action_entry_p);
                OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s: action found ",__FUNCTION__);
                status=OF_SUCCESS;
                break;
              }
            }

            if (status == OF_SUCCESS)
            {
              CNTLR_RCU_READ_LOCK_RELEASE();  //  <<<<<<<< LSR
              return OF_SUCCESS;
            }	
            else
            {
              action_info = 
                (struct ofi_flow_action *)calloc(1, sizeof(struct ofi_flow_action)); 
              if (action_info == NULL)
              {
                CNTLR_RCU_READ_LOCK_RELEASE();  //  <<<<<<<< LSR
                return OF_FAILURE;
              }

              of_flow_copy_action(action_info, action_entry_p);
              OF_APPEND_NODE_TO_LIST(inst_info->action_list, action_info,OF_FLOW_ACTION_LISTNODE_OFFSET);
              CNTLR_RCU_READ_LOCK_RELEASE();  //  <<<<<<<< LSR
              return OF_SUCCESS;
            }
          }
        }
      }
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  return status;
}

