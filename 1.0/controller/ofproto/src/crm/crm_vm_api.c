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

/*
 *
 * File name: crm_vm_api.c
 * Author: Varun Kumar Reddy <B36461@freescale.com>
 * Date:   11/10/2013
 * Description: 
*/

/** Cloud resource manager Virtual Machine source file */
/** Header files */
#include "crm_system_dependencies.h"
#include "crm_tenant_api.h"
#include "crm_vn_api.h"
#include "crm_vm_api.h"

/** Macros */

/** Declarations */
extern void*    crm_vm_mempool_g;
extern void*    crm_vm_port_mempool_g;
extern void*    crm_notifications_mempool_g;
extern uint32_t crm_no_of_vm_buckets_g;
extern struct   mchash_table* crm_vm_table_g;

extern of_list_t crm_vm_notifications[CRM_VM_LAST_NOTIFICATION_TYPE + 1];
//void crm_attributes_free_attribute_entry_rcu(struct rcu_head *attribute_entry_p);
struct database_view vm_db_view[CRM_MAX_VIEW_DATABASE];


/** Definitions */
void crm_vm_port_free_entry_rcu(struct rcu_head *crm_vm_port_node_entry_p)
{
  struct crm_vm_port *crm_vm_port_node_info_p=NULL;
  int32_t ret_value;

  if(crm_vm_port_node_entry_p)
  {
    crm_vm_port_node_info_p = (struct crm_vm_port *)(((char *)crm_vm_port_node_entry_p) - sizeof(of_list_node_t));
    ret_value = mempool_release_mem_block(crm_vm_port_mempool_g,
        (uchar8_t*)crm_vm_port_node_info_p,
        crm_vm_port_node_info_p->heap_b);
    if(ret_value != MEMPOOL_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Failed to release crm vm port node memory block");
    }
    else
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Deleted succesfully  crm vm port node from  the database");
    }
  }
  else
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "NULL passed for deletion");
  }
}
/*********************************************************************************
Function:crm_get_vm_handle
Description:
This function returns the virtual network handle for the given vn name
**********************************************************************************/
int32_t crm_get_vm_handle(char *vm_name_p, uint64_t *vm_handle_p)
{
  uint32_t hashkey;
  uint32_t offset;

  struct crm_virtual_machine *crm_vm_info_p=NULL;

  if(crm_vm_table_g == NULL)
    return CRM_FAILURE;

  hashkey = crm_get_hashval_byname(vm_name_p,crm_no_of_vm_buckets_g);
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "hashkey:%d\r\n",hashkey);
  CNTLR_RCU_READ_LOCK_TAKE();
  offset = VMNODE_VMTBL_OFFSET;

  MCHASH_BUCKET_SCAN(crm_vm_table_g, hashkey, crm_vm_info_p, struct crm_virtual_machine *,offset)
  {
    if(strcmp(crm_vm_info_p->vm_name,vm_name_p))
      continue;
    CNTLR_RCU_READ_LOCK_RELEASE();

    *vm_handle_p = crm_vm_info_p->magic;
    *vm_handle_p = ((*vm_handle_p <<32) | (crm_vm_info_p->index));

    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "vm_handle:%llx\r\n",*vm_handle_p);
    return CRM_SUCCESS;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return CRM_ERROR_VM_NAME_INVALID;
}
/*************************************************************************************
Function:get_vm_byhandle
Description:
        This function returns virtual network info for given vm_handle.
*************************************************************************************/
int32_t get_vm_byhandle(uint64_t vm_handle,struct crm_virtual_machine **crm_vm_info_p)
{
  /* Caller shall take RCU locks. */
  uint32_t index,magic;
  uint8_t status_b;

  magic = (uint32_t)(vm_handle >>32);
  index = (uint32_t)vm_handle;

  MCHASH_ACCESS_NODE(crm_vm_table_g,index,magic,*crm_vm_info_p,status_b);
  if(TRUE == status_b)
    return CRM_SUCCESS;
  return CRM_ERROR_INVALID_VM_HANDLE;
}
/***************************************************************************
Function:crm_add_virtual_machine
***************************************************************************/
int32_t crm_add_virtual_machine (struct crm_vm_config_info* config_info_p, 
                                 uint64_t* output_vm_handle_p)
{
  int32_t   ret_value = CRM_SUCCESS;
  uint32_t  hashkey,offset,index = 0, magic = 0;
  uint8_t   heap_b,status_b = FALSE;
  uchar8_t* hashobj_p = NULL;
  uint32_t  apphookoffset;
  uint64_t  switch_handle,tenant_handle;
  
  struct crm_virtual_machine*            crm_vm_node_p=NULL;
  struct crm_virtual_machine*            crm_vm_node_scan_p=NULL;
  struct crm_vm_notification_data        notification_data={};
  
  struct crm_notification_app_hook_info* app_entry_p;

  if(crm_vm_table_g ==  NULL)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "VM table doesnt exist");
    return CRM_FAILURE;
  }

  if((config_info_p->vm_name == NULL) || (strlen(config_info_p->vm_name)>=CRM_MAX_VM_NAME_LEN))
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "virtual machine name invalid");
    return CRM_ERROR_VM_NAME_INVALID;
  }

  if((config_info_p->tenant_name == NULL) || (config_info_p->switch_name == NULL))
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "name is empty");
    return  CRM_FAILURE;   
  }

  ret_value = crm_get_tenant_handle(config_info_p->tenant_name, &tenant_handle);
  if(ret_value != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Invalid tenant name");
    return CRM_ERROR_TENANT_NAME_INVALID;
  }

  ret_value = dprm_get_switch_handle(config_info_p->switch_name, &switch_handle);
  if(ret_value != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Invalid switch name");
    return CRM_ERROR_INVALID_NAME;
  }

  if(!((config_info_p->vm_type == VM_TYPE_NORMAL_APPLICATION) || (config_info_p->vm_type == VM_TYPE_NETWORK_SERVICE)))
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR,"virtual machine type invalid");
    return CRM_ERROR_INVALID_VM_TYPE;
  }

  CNTLR_RCU_READ_LOCK_TAKE();

  /** calculate hash key for the Virtual network name */
  hashkey = crm_get_hashval_byname(config_info_p->vm_name, crm_no_of_vm_buckets_g);
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "bucket = %d",hashkey);

  offset = VMNODE_VMTBL_OFFSET;
  /** Scan VM hash table for the name */
  MCHASH_BUCKET_SCAN(crm_vm_table_g, hashkey, crm_vm_node_scan_p, struct crm_virtual_machine*, offset)
  {
    if(strcmp(config_info_p->vm_name, crm_vm_node_scan_p->vm_name) != 0)
      continue;
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Duplicate virtual machine name :%s", config_info_p->vm_name);
    CNTLR_RCU_READ_LOCK_RELEASE();
    return CRM_ERROR_DUPLICATE_VM_RESOURCE;
  }
  /** Create memory for the entry */
  ret_value = mempool_get_mem_block(crm_vm_mempool_g, (uchar8_t **)&crm_vm_node_p, &heap_b);
  if(ret_value != MEMPOOL_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR,"failed to get memory block");
    ret_value = CRM_FAILURE;
    goto CRM_ADD_VM_EXIT;
  }
  crm_vm_node_p->heap_b = heap_b;
  strncpy(crm_vm_node_p->vm_name, config_info_p->vm_name, CRM_MAX_VM_NAME_LEN);
  strncpy(crm_vm_node_p->tenant_name, config_info_p->tenant_name, CRM_MAX_TENANT_NAME_LEN);
  crm_vm_node_p->switch_handle = switch_handle;
  strncpy(crm_vm_node_p->switch_name_dprm, config_info_p->switch_name, DPRM_MAX_SWITCH_NAME_LEN);
  strncpy(crm_vm_node_p->vm_desc, config_info_p->vm_desc, CRM_MAX_VM_DESC_LEN);
  crm_vm_node_p->vm_type =  config_info_p->vm_type;

  hashobj_p = (uchar8_t *)crm_vm_node_p + VMNODE_VMTBL_OFFSET;
  MCHASH_APPEND_NODE(crm_vm_table_g, hashkey, crm_vm_node_p, index, magic, hashobj_p, status_b);
  if(FALSE == status_b)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "failed to add vm node");
    ret_value = CRM_FAILURE;
    goto CRM_ADD_VM_EXIT;
  }

  *output_vm_handle_p = magic;
  *output_vm_handle_p = ((*output_vm_handle_p <<32) | (index));

  crm_vm_node_p->magic = magic;
  crm_vm_node_p->index = index;

  strcpy(notification_data.vm_name, crm_vm_node_p->vm_name);
  (crm_vm_node_p->vm_handle) = *output_vm_handle_p;

  OF_LIST_INIT(crm_vm_node_p->list_of_logical_switch_side_ports_connected, crm_vm_port_free_entry_rcu);
  OF_LIST_INIT(crm_vm_node_p->attributes, crm_attributes_free_attribute_entry_rcu);

  apphookoffset = CRM_NOTIFICATION_APP_HOOK_OFFSET;

  OF_LIST_SCAN(crm_vm_notifications[CRM_VM_ADDED], app_entry_p, struct crm_notification_app_hook_info *, apphookoffset)
  {
    ((crm_vm_notifier_fn)(app_entry_p->call_back))(CRM_VM_ADDED,
    *output_vm_handle_p,
    notification_data,
    app_entry_p->cbk_arg1,
    app_entry_p->cbk_arg2);
  }

  OF_LIST_SCAN(crm_vm_notifications[CRM_VM_ALL_NOTIFICATIONS], app_entry_p, struct crm_notification_app_hook_info *, apphookoffset )
  {
    ((crm_vm_notifier_fn)(app_entry_p->call_back))(CRM_VM_ADDED,
    *output_vm_handle_p,
    notification_data,
    app_entry_p->cbk_arg1,
    app_entry_p->cbk_arg2);
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "VM Added successfully");
  return CRM_SUCCESS;

CRM_ADD_VM_EXIT:
  if(crm_vm_node_p)
  {
    mempool_release_mem_block(crm_vm_mempool_g, (uchar8_t *)crm_vm_node_p, crm_vm_node_p->heap_b);
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "failed to add VM");
  return ret_value;
}

int32_t crm_modify_virtual_machine (struct crm_vm_config_info* config_info_p, 
    uint64_t* output_vm_handle_p)
{
  int32_t   ret_value =CRM_SUCCESS;
  uint32_t  hashkey, offset;
  uint32_t  apphookoffset;
  uint64_t  switch_handle,tenant_handle;
  struct    crm_virtual_machine            *crm_vm_node_scan_p=NULL;
  struct    crm_notification_app_hook_info *app_entry_p=NULL;
  struct    crm_vm_notification_data        notification_data={};
  uint32_t vm_node_found=0;


  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Entered");
  if(crm_vm_table_g ==  NULL)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "VM table null");
    return CRM_FAILURE;
  }
  if((config_info_p->vm_name == NULL) ||
      (strlen(config_info_p->vm_name)>=CRM_MAX_VM_NAME_LEN)
    )
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "virtual machine name invalid");
    return CRM_ERROR_VM_NAME_INVALID;
  }
  if((config_info_p->tenant_name == NULL)||
      (config_info_p->switch_name == NULL)
    )
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "name is empty");
    return  CRM_FAILURE;   
  }

  ret_value = crm_get_tenant_handle(config_info_p->tenant_name, &tenant_handle);
  if(ret_value != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Invalid tenant name");
    return CRM_ERROR_TENANT_NAME_INVALID;
  }

  ret_value = dprm_get_switch_handle(config_info_p->switch_name, &switch_handle);
  if(ret_value != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Invalid switch name");
    return CRM_ERROR_INVALID_NAME;
  }

  if(!((config_info_p->vm_type == VM_TYPE_NORMAL_APPLICATION)
        || (config_info_p->vm_type == VM_TYPE_NETWORK_SERVICE)))
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG,"virtual machine type invalid");
    return CRM_ERROR_INVALID_VM_TYPE;
  }

  CNTLR_RCU_READ_LOCK_TAKE();

  /** calculate hash key for the Virtual network name */
  hashkey = crm_get_hashval_byname(config_info_p->vm_name, crm_no_of_vm_buckets_g);
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "bucket = %d",hashkey);

  offset = VMNODE_VMTBL_OFFSET;
  /** Scan VM hash table for the name */
  MCHASH_BUCKET_SCAN(crm_vm_table_g, hashkey, crm_vm_node_scan_p, struct crm_virtual_machine*, offset)
  {
    if(!strcmp(config_info_p->vm_name, crm_vm_node_scan_p->vm_name))
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "virtual machine name :%s found to modify", config_info_p->vm_name);
      vm_node_found  =1;
      break;
    }
  }
  if(!vm_node_found)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "virtual machine name :%s not found to modify", config_info_p->vm_name);
    CNTLR_RCU_READ_LOCK_RELEASE();
    return CRM_ERROR_DUPLICATE_VM_RESOURCE;
  }
  strncpy(crm_vm_node_scan_p->tenant_name, config_info_p->tenant_name, CRM_MAX_TENANT_NAME_LEN);
  crm_vm_node_scan_p->switch_handle = switch_handle;
  strncpy(crm_vm_node_scan_p->switch_name_dprm, config_info_p->switch_name, DPRM_MAX_SWITCH_NAME_LEN);
  strncpy(crm_vm_node_scan_p->vm_desc, config_info_p->vm_desc, CRM_MAX_VM_DESC_LEN);
  crm_vm_node_scan_p->vm_type =  config_info_p->vm_type;

  strcpy(notification_data.vm_name, crm_vm_node_scan_p->vm_name);
  apphookoffset = CRM_NOTIFICATION_APP_HOOK_OFFSET;

  OF_LIST_SCAN(crm_vm_notifications[CRM_VM_MODIFIED], app_entry_p, struct crm_notification_app_hook_info *, apphookoffset)
  {
    ((crm_vm_notifier_fn)(app_entry_p->call_back))(CRM_VM_MODIFIED,
    (crm_vm_node_scan_p->vm_handle),
    notification_data,
    app_entry_p->cbk_arg1,
    app_entry_p->cbk_arg2);
  }

  OF_LIST_SCAN(crm_vm_notifications[CRM_VM_ALL_NOTIFICATIONS], app_entry_p, struct crm_notification_app_hook_info *, apphookoffset )
  {
    ((crm_vm_notifier_fn)(app_entry_p->call_back))(CRM_VM_MODIFIED,
    (crm_vm_node_scan_p->vm_handle),
    notification_data,
    app_entry_p->cbk_arg1,
    app_entry_p->cbk_arg2);
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "VM Modified successfully");
  return CRM_SUCCESS;
}
/*************************************************************************
@Function       crm_del_virtual_machine 
*************************************************************************/
int32_t crm_del_virtual_machine (char* vm_name)
{
  uint32_t  hashkey, offset;
  uint8_t   status_b = FALSE;
  uint8_t   vm_node_found=0;
  int32_t   ret_val;
  uint32_t  apphookoffset;

  struct    crm_notification_app_hook_info *app_entry_p;
  struct    crm_vm_notification_data  notification_data={};
  struct    crm_virtual_machine   *crm_vm_node_scan_p=NULL;

  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Entered");

  if(crm_vm_table_g ==  NULL)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "No vm db");
    return CRM_FAILURE;
  }

  if((vm_name == NULL))
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "vm name null");
    return CRM_ERROR_VM_NAME_NULL;
  }

  CNTLR_RCU_READ_LOCK_TAKE();
  /** calculate hash key for the Virtual network name */
  hashkey = crm_get_hashval_byname(vm_name, crm_no_of_vm_buckets_g);

  offset = VMNODE_VMTBL_OFFSET;

  /** Scan VM hash table for the name */
  MCHASH_BUCKET_SCAN(crm_vm_table_g, hashkey, crm_vm_node_scan_p, struct crm_virtual_machine*, offset)
  {
    if(!strcmp(vm_name, crm_vm_node_scan_p->vm_name))
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "vm name found to delete");
      vm_node_found = 1;
      break;
    }    
  }

  if(vm_node_found==1)
  {
    if(crm_vm_node_scan_p->no_of_ports)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Ports attached to vm node");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return CRM_ERROR_RESOURCE_NOTEMPTY;     
    }
    apphookoffset = CRM_NOTIFICATION_APP_HOOK_OFFSET;
    strcpy(notification_data.vm_name, vm_name);
    OF_LIST_SCAN(crm_vm_notifications[CRM_VM_DELETE], app_entry_p, struct crm_notification_app_hook_info *, apphookoffset )
    {
      ((crm_vm_notifier_fn)(app_entry_p->call_back))(CRM_VM_DELETE,
      crm_vm_node_scan_p->vm_handle,
      notification_data,
      app_entry_p->cbk_arg1,
      app_entry_p->cbk_arg2);
    }

    OF_LIST_SCAN(crm_vm_notifications[CRM_VM_ALL_NOTIFICATIONS], app_entry_p, struct crm_notification_app_hook_info *, apphookoffset )
    {
      ((crm_vm_notifier_fn)(app_entry_p->call_back))(CRM_VM_DELETE,
      crm_vm_node_scan_p->vm_handle,
      notification_data,
      app_entry_p->cbk_arg1,
      app_entry_p->cbk_arg2);
    }

    status_b = FALSE;
    offset = VMNODE_VMTBL_OFFSET;
    MCHASH_DELETE_NODE_BY_REF(crm_vm_table_g, crm_vm_node_scan_p->index, crm_vm_node_scan_p->magic,struct crm_virtual_machine *, offset, status_b);
    if(status_b == TRUE)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG,"virtual machine node deleted successfully");
      ret_val = CRM_SUCCESS;
    }
    else
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "failed to delete");
      ret_val= CRM_FAILURE;
    }
  }
  else
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "virtual machine name not found to delete", __FUNCTION__, __LINE__);
    ret_val=CRM_ERROR_VM_NAME_INVALID;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return ret_val;
}
/*****************************************************************************************************/
int32_t crm_register_vm_notifications (uint32_t notification_type, crm_vm_notifier_fn vm_notifier_fn,
                                       void* callback_arg1,void* callback_arg2)
{
  struct   crm_notification_app_hook_info *app_entry_p;
  uint8_t  heap_b;
  int32_t  retval = CRM_SUCCESS;
  uint32_t lstoffset;

  lstoffset = CRM_NOTIFICATION_APP_HOOK_OFFSET;

  if((notification_type < CRM_VM_FIRST_NOTIFICATION_TYPE) || (notification_type > CRM_VM_LAST_NOTIFICATION_TYPE))
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Invalid Notification type: %u", notification_type);
    return CRM_FAILURE;
  }

  if(vm_notifier_fn == NULL)
  {     
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "callback function  null");
    return CRM_FAILURE;
  }

  retval = mempool_get_mem_block(crm_notifications_mempool_g, (uchar8_t**)&app_entry_p, &heap_b);
  if(retval != MEMPOOL_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Failed to get memory block");
    return CRM_FAILURE;
  }

  app_entry_p->call_back = (void*)vm_notifier_fn;
  app_entry_p->cbk_arg1  = callback_arg1;
  app_entry_p->cbk_arg2  = callback_arg2;
  app_entry_p->heap_b    = heap_b;

  /* Add Application to the list of VM Applications maintained per notification type. */
  OF_APPEND_NODE_TO_LIST(crm_vm_notifications[notification_type], app_entry_p, lstoffset);

  if(retval != CRM_SUCCESS)
    mempool_release_mem_block(crm_notifications_mempool_g, (uchar8_t*)app_entry_p, app_entry_p->heap_b);

  return retval;
}
/******************************************************************************/
int32_t crm_get_vm_by_name(char*  vm_name,struct crm_vm_config_info* config_info_p,
                           struct vm_runtime_info* runtime_info)
{
  int32_t   ret_val=CRM_FAILURE;  
  uint8_t   vm_node_found=0;
  struct    crm_virtual_machine *crm_vm_node_scan_p=NULL;
  uint32_t  hashkey,offset;

  if(vm_name ==  NULL)
    return CRM_ERROR_VM_NAME_NULL;

  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "vm_name %s",vm_name);
  
  CNTLR_RCU_READ_LOCK_TAKE();

  /** calculate hash key for the Virtual network name */
  hashkey = crm_get_hashval_byname(vm_name, crm_no_of_vm_buckets_g);

  offset = VMNODE_VMTBL_OFFSET;

  /** Scan VM hash table for the name */
  MCHASH_BUCKET_SCAN(crm_vm_table_g, hashkey, crm_vm_node_scan_p, struct crm_virtual_machine*, offset)
  {
    if(!strcmp(vm_name, crm_vm_node_scan_p->vm_name))
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "virtual machine found");
      vm_node_found = 1;
      break;
    }
  }
  if(vm_node_found == 1)
  {
    strcpy(config_info_p->vm_name, crm_vm_node_scan_p->vm_name);
    config_info_p->vm_type = crm_vm_node_scan_p->vm_type;
    strcpy(config_info_p->switch_name, crm_vm_node_scan_p->switch_name_dprm);
    strcpy(config_info_p->tenant_name, crm_vm_node_scan_p->tenant_name);
    ret_val = CRM_SUCCESS;  
  }
  else
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "virtual machine not found");
    ret_val = CRM_ERROR_VM_NAME_INVALID;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return ret_val;
}
/***************************************************************************/
int32_t  crm_get_first_vm (struct    crm_vm_config_info* config_info_p,
                           struct    vm_runtime_info* runtime_info,
                           uint64_t* vm_handle_p)
{
  struct   crm_virtual_machine* crm_vm_node_info_p = NULL;
  uint32_t hashkey;
  uint32_t offset;

  CNTLR_RCU_READ_LOCK_TAKE();

  MCHASH_TABLE_SCAN(crm_vm_table_g, hashkey)
  {
    offset = VMNODE_VMTBL_OFFSET;
    MCHASH_BUCKET_SCAN(crm_vm_table_g,hashkey,crm_vm_node_info_p,struct crm_virtual_machine *, offset)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "copy info");
      strcpy(config_info_p->vm_name, crm_vm_node_info_p->vm_name);
      config_info_p->vm_type = crm_vm_node_info_p->vm_type;
      strcpy(config_info_p->switch_name, crm_vm_node_info_p->switch_name_dprm);
      strcpy(config_info_p->tenant_name, crm_vm_node_info_p->tenant_name);
      strcpy(config_info_p->vm_desc,crm_vm_node_info_p->vm_desc);

      //runtime_info_p->number_of_attributes = crm_vm_node_info_p->number_of_attributes;

      *vm_handle_p = crm_vm_node_info_p->magic;
      *vm_handle_p = ((*vm_handle_p <<32) | (crm_vm_node_info_p->index));

      CNTLR_RCU_READ_LOCK_RELEASE();
      return CRM_SUCCESS;
    }
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return CRM_ERROR_NO_MORE_ENTRIES;
}
/***************************************************************************/
int32_t crm_get_next_vm (struct crm_vm_config_info* config_info_p,
                         struct vm_runtime_info* runtime_info,
                         uint64_t* vm_handle_p)
{
  struct   crm_virtual_machine* crm_vm_node_info_p = NULL;
  uint32_t hashkey;
  uint8_t  current_entry_found_b;

  uint32_t offset = VMNODE_VMTBL_OFFSET;

  CNTLR_RCU_READ_LOCK_TAKE();
  current_entry_found_b = FALSE;

  MCHASH_TABLE_SCAN(crm_vm_table_g, hashkey)
  {
    offset = VMNODE_VMTBL_OFFSET;
    MCHASH_BUCKET_SCAN(crm_vm_table_g,hashkey,crm_vm_node_info_p,struct crm_virtual_machine *,offset)
    {
      if(current_entry_found_b == FALSE)
      {
        if(*vm_handle_p != (crm_vm_node_info_p->vm_handle))
          continue;
        else
        {
          current_entry_found_b = TRUE;
          continue;
        }

        /*Skip the First matching Switch and Get the next Switch */
      }
      strcpy(config_info_p->vm_name, crm_vm_node_info_p->vm_name);
      config_info_p->vm_type = crm_vm_node_info_p->vm_type;
      strcpy(config_info_p->switch_name, crm_vm_node_info_p->switch_name_dprm);
      strcpy(config_info_p->tenant_name, crm_vm_node_info_p->tenant_name);

      //runtime_info_p->number_of_attributes = crm_vm_node_info_p->number_of_attributes;

      *vm_handle_p = crm_vm_node_info_p->magic;
      *vm_handle_p = ((*vm_handle_p <<32) | (crm_vm_node_info_p->index));

      CNTLR_RCU_READ_LOCK_RELEASE();
      return CRM_SUCCESS;
    }
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  if(current_entry_found_b == TRUE)
    return CRM_ERROR_NO_MORE_ENTRIES;
  else
    return CRM_ERROR_INVALID_VM_HANDLE;
}
/***************************************************************************/
/* This function is called only internally from CRM API crm_add_crm_vmport() */
int32_t crm_attach_logical_switch_side_port_to_vm (char*    crm_port_name,
                                                   char*    vm_name,
                                                   char*    vn_name,
                                                   uint64_t crm_port_handle 
                                                  )
{
  int32_t  ret_val = CRM_SUCCESS;
  uint64_t vm_handle;
  uint8_t  heap_b;

  struct crm_vm_port *crm_vm_port_node_entry_p       = NULL;
  struct crm_vm_port *crm_vm_port_node_scan_entry_p  = NULL;
  struct crm_virtual_machine *crm_vm_node_p          = NULL;

  ret_val = crm_get_vm_handle(vm_name,&vm_handle);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR,"Failed to get vm handle");
    return CRM_FAILURE;
  }

  ret_val = get_vm_byhandle(vm_handle,&crm_vm_node_p);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR,"Failed to get vm node\n");
    return CRM_FAILURE;
  }

  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    OF_LIST_SCAN((crm_vm_node_p->list_of_logical_switch_side_ports_connected),
                  crm_vm_port_node_scan_entry_p,
                  struct crm_vm_port *,
                  CRM_VM_PORT_NODE_ENTRY_LISTNODE_OFFSET)
    {
      if(!strcmp(crm_vm_port_node_scan_entry_p->port_name, crm_port_name))
      {
        OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Duplicate crm vm port node found.");
        ret_val = CRM_FAILURE;
        break;
      }
    }
    /* Allocate memory for the crm compute node */
    ret_val = mempool_get_mem_block(crm_vm_port_mempool_g,(uchar8_t**)&crm_vm_port_node_entry_p,&heap_b);
    if(ret_val != MEMPOOL_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "memory block allocation failed for crm  vm port node.");
      ret_val = CRM_FAILURE;
      break;    
    }
    crm_vm_port_node_entry_p->heap_b = heap_b;
    strcpy(crm_vm_port_node_entry_p->port_name,crm_port_name);
    strcpy(crm_vm_port_node_entry_p->vn_name,vn_name);
    crm_vm_port_node_entry_p->port_saferef = crm_port_handle;
   
    OF_APPEND_NODE_TO_LIST((crm_vm_node_p->list_of_logical_switch_side_ports_connected),
                            crm_vm_port_node_entry_p,
                            CRM_VM_PORT_NODE_ENTRY_LISTNODE_OFFSET);
    
    crm_vm_node_p->no_of_ports++;

  }while(0);

  CNTLR_RCU_READ_LOCK_RELEASE();
  return ret_val;  
}
/****************************************************************************************************/
int32_t crm_detach_logical_switch_side_port_from_vm(char *port_name,char *vm_name,char *vn_name,uint8_t* no_of_ports_p)
{
  int32_t  ret_val = CRM_SUCCESS;
  uint64_t vm_handle;
  uint32_t lstoffset;

  struct crm_vm_port *crm_vm_port_node_p=NULL;
  struct crm_vm_port *crm_vm_port_prev_node_p=NULL;

  struct crm_virtual_machine *crm_vm_node_p=NULL;

  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG,  "Entered");
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG,  "port_name:%s,vm_name:%s",port_name,vm_name);

  if((port_name == NULL)|| (vm_name==NULL))
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR,  "Invalid  ports data supplied to add.\n");
    return CRM_FAILURE;
  }

  ret_val = crm_get_vm_handle(vm_name , &vm_handle);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR,  "failed to get vm handle\n");
    return CRM_FAILURE;
  }

  ret_val = get_vm_byhandle(vm_handle , &crm_vm_node_p);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR,  "failed to get vm node\n");
    return CRM_FAILURE;
  }

  lstoffset = CRM_VM_PORT_NODE_ENTRY_LISTNODE_OFFSET;
  CNTLR_RCU_READ_LOCK_TAKE();
  
  OF_LIST_SCAN((crm_vm_node_p->list_of_logical_switch_side_ports_connected),
                crm_vm_port_node_p,
                struct crm_vm_port *,
                lstoffset)
  {
    if(!strcmp(crm_vm_port_node_p->port_name, port_name))
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG,  "crm vm port node found to delete\n");
      OF_REMOVE_NODE_FROM_LIST(crm_vm_node_p->list_of_logical_switch_side_ports_connected, 
				 crm_vm_port_node_p,
                                 crm_vm_port_prev_node_p,
				 lstoffset 
				);

     OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG,  "no of ports : %d",crm_vm_node_p->no_of_ports);
     crm_vm_node_p->no_of_ports--;
     *no_of_ports_p = crm_vm_node_p->no_of_ports;
     OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG,  "no of ports after decrement : %d",*no_of_ports_p);
     CNTLR_RCU_READ_LOCK_RELEASE();
     return CRM_SUCCESS;
    }
    crm_vm_port_prev_node_p = crm_vm_port_node_p;	
  }
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR,  "crm vm port  node not found to delete\n");
  CNTLR_RCU_READ_LOCK_RELEASE();
  return CRM_FAILURE;  
}
/****************************************************************************************************************/
int32_t crm_add_attribute_to_vm(uint64_t vm_handle, struct crm_attribute_name_value_pair* attribute_info_p)
{
  int32_t  retval;
  struct   crm_virtual_machine*            vm_entry_p;
  struct   crm_vm_notification_data        notification_data={};
  struct   crm_notification_app_hook_info* app_entry_p;
  uchar8_t lstoffset;

  lstoffset = CRM_NOTIFICATION_APP_HOOK_OFFSET;

  CNTLR_RCU_READ_LOCK_TAKE();

  retval = get_vm_byhandle(vm_handle, &vm_entry_p);
  if(retval != CRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return retval;
  }

  retval = crm_add_attribute(&(vm_entry_p->attributes),attribute_info_p);
  if(retval != CRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return retval;
  }

  vm_entry_p->number_of_attributes++;
  crm_create_vm_view_entry(vm_handle, attribute_info_p->name_string, attribute_info_p->value_string);

  /* Send add attribute to switch notification to registered Applications. */
  strcpy(notification_data.vm_name, vm_entry_p->vm_name);
  strcpy(notification_data.attribute_name,attribute_info_p->name_string);
  strcpy(notification_data.attribute_value,attribute_info_p->value_string);
  OF_LIST_SCAN(crm_vm_notifications[CRM_VM_ATTRIBUTE_ADDED],app_entry_p,struct crm_notification_app_hook_info*,lstoffset )
  {
    ((crm_vm_notifier_fn)(app_entry_p->call_back))(CRM_VM_ATTRIBUTE_ADDED,
                                                   vm_handle,
                                                   notification_data,
                                                   app_entry_p->cbk_arg1,
                                                   app_entry_p->cbk_arg2);
  }

  OF_LIST_SCAN(crm_vm_notifications[CRM_VM_ALL_NOTIFICATIONS],app_entry_p,struct crm_notification_app_hook_info*,lstoffset )
  {
    ((crm_vm_notifier_fn)(app_entry_p->call_back))(CRM_VM_ATTRIBUTE_ADDED,
                                                   vm_handle,
                                                   notification_data,
                                                   app_entry_p->cbk_arg1,
                                                   app_entry_p->cbk_arg2);
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return CRM_SUCCESS;
}

int32_t crm_delete_attribute_from_vm(uint64_t vm_handle, struct crm_attribute_name_value_pair *attribute_info_p)
{
  int32_t  retval;
  struct   crm_virtual_machine* vm_entry_p;
  struct   crm_resource_attribute_entry *attribute_entry_p,*prev_app_entry_p = NULL;

  struct   crm_vm_notification_data   notification_data={};
  struct   crm_notification_app_hook_info* app_entry_p;

  uchar8_t lstattrnameoffset;
  lstattrnameoffset = CRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET;

  CNTLR_RCU_READ_LOCK_TAKE();

  retval = get_vm_byhandle(vm_handle, &vm_entry_p);
  if(retval != CRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return retval;
  }
    OF_LIST_SCAN(vm_entry_p->attributes, attribute_entry_p, struct crm_resource_attribute_entry*,lstattrnameoffset)
  {
    if(!strcmp(attribute_entry_p->name, attribute_info_p->name_string))
    {
      strcpy(notification_data.vm_name, vm_entry_p->vm_name);
      strcpy(notification_data.attribute_name,attribute_entry_p->name);
      strcpy(notification_data.attribute_value,attribute_entry_p->value);
      
      OF_LIST_SCAN(crm_vm_notifications[CRM_VM_ATTRIBUTE_DELETE],app_entry_p,struct crm_notification_app_hook_info*, lstattrnameoffset )
      {
        ((crm_vm_notifier_fn)(app_entry_p->call_back))(CRM_VM_ATTRIBUTE_DELETE,
                                                       vm_handle,
                                                       notification_data,
                                                       app_entry_p->cbk_arg1,
                                                       app_entry_p->cbk_arg2);
      }

      OF_LIST_SCAN(crm_vm_notifications[CRM_VM_ALL_NOTIFICATIONS],app_entry_p,struct crm_notification_app_hook_info*,lstattrnameoffset)
      {
        ((crm_vm_notifier_fn)(app_entry_p->call_back))(CRM_VM_ATTRIBUTE_DELETE,
                                                       vm_handle,
                                                       notification_data,
                                                       app_entry_p->cbk_arg1,
                                                       app_entry_p->cbk_arg2);
      }
      OF_REMOVE_NODE_FROM_LIST(vm_entry_p->attributes,attribute_entry_p,prev_app_entry_p, lstattrnameoffset);
      vm_entry_p->number_of_attributes--;
      crm_remove_vm_view_entry(attribute_entry_p->name, attribute_entry_p->value);
      CNTLR_RCU_READ_LOCK_RELEASE();
      return CRM_SUCCESS;
    }
    prev_app_entry_p = attribute_entry_p;
    continue;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return CRM_FAILURE;
}
/*******************************************************************************************************/
int32_t crm_get_vm_first_attribute(uint64_t vm_handle,
                                   struct crm_attribute_name_value_output_info* attribute_output_p)
{
  int32_t  retval;
  struct crm_virtual_machine *crm_vm_entry_p=NULL;
  if(attribute_output_p == NULL)
    return CRM_FAILURE;

  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    retval = get_vm_byhandle(vm_handle, &crm_vm_entry_p);
    if(retval != CRM_SUCCESS)
    { 
      retval = CRM_ERROR_INVALID_VM_HANDLE;
      break;
    }
    retval = crm_get_first_attribute(&(crm_vm_entry_p->attributes), attribute_output_p);

  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  return retval;
}
/*****************************************************************************************************/
int32_t crm_get_vm_next_attribute(uint64_t vm_handle,
                                  char*    current_attribute_name,
                                  struct   crm_attribute_name_value_output_info *attribute_output_p)
{
  int32_t retval;
  struct crm_virtual_machine* crm_vm_entry_p=NULL;

  if((attribute_output_p == NULL) || (current_attribute_name == NULL))
    return CRM_FAILURE;

  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    retval = get_vm_byhandle(vm_handle,&crm_vm_entry_p);
    if(retval != CRM_SUCCESS)
    {
      retval= CRM_ERROR_INVALID_VM_HANDLE;
      break;
    }
    retval = crm_get_next_attribute(&(crm_vm_entry_p->attributes), current_attribute_name, attribute_output_p);
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  return retval;
}
/******************************************************************************/
int32_t crm_get_vm_exact_attribute(uint64_t vm_handle,
                                   char* attribute_name_string,
                                   struct   crm_attribute_name_value_output_info *attribute_output_p)
{
  struct   crm_virtual_machine* crm_vm_entry_p=NULL;
  int32_t  retval;

  if(attribute_name_string == NULL)
   return CRM_ERROR_ATTRIBUTE_NAME_NULL;

  if(strlen(attribute_name_string) > CRM_MAX_ATTRIBUTE_NAME_LEN)
    return CRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN;

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
     retval = get_vm_byhandle(vm_handle, &crm_vm_entry_p);
     if(retval != CRM_SUCCESS)
     {
       retval= CRM_ERROR_INVALID_VM_HANDLE;
       break;	
     }
     retval = crm_get_exact_attribute(&(crm_vm_entry_p->attributes),  attribute_name_string,attribute_output_p);

   }while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return retval;
}
/************************************************************************************/
int32_t crm_create_vm_view(char *view_name)
{
  int32_t index = 0, ii;
  uint8_t empty_index_b = 0;

  for(ii = 0;ii<CRM_MAX_VIEW_DATABASE ; ii++)
  {
     if(!strcmp(vm_db_view[ii].view , view_name))
     {
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"View already created");
        return CRM_FAILURE;
     } 
     else if((empty_index_b == 0) && (!strcmp(vm_db_view[ii].view ,"")))    
     {
        empty_index_b = 1;
        index = ii;
     }
  } 
  strcpy(vm_db_view[index].view,view_name);
  crm_view_hash_table_init(&vm_db_view[index].db_viewtbl_p);
  vm_db_view[index].uniqueness_flag = FALSE; //of_view_get_uniqueness(view_name);/*TBD*/
  vm_db_view[index].valid_b = TRUE;
 
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "View node created succesfully");
  return CRM_SUCCESS;
}
  

int32_t crm_create_vm_view_entry(uint64_t vm_handle , char* view_name ,char* view_value)
{
  struct   database_view_node *view_node_entry_p;
  int32_t  view_index;
  uint32_t index,magic;
  int32_t  status = CRM_SUCCESS;
  uint32_t hashkey;
  uint8_t  status_b,view_found_b = 0;

  do
  {
     for(view_index = 0;view_index < CRM_MAX_VIEW_DATABASE ; view_index++)
     {
       if(!strcmp(vm_db_view[view_index].view ,view_name))
       {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "View entry can be made.");
         view_found_b = 1;
         break;
       }
     }
     if(view_found_b == 0)
     {
       OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"View hash table not initialized");
       status = CRM_FAILURE;
     }
     
     hashkey = crm_get_hashval_byname(view_value,crm_no_of_vm_buckets_g);
     CNTLR_RCU_READ_LOCK_TAKE();
     view_node_entry_p = crm_alloc_view_entry_and_set_values(vm_handle, view_name, view_value);
     if (view_node_entry_p == NULL)
     {
       status = CRM_FAILURE;
       break;
     }
     MCHASH_APPEND_NODE(vm_db_view[view_index].db_viewtbl_p,hashkey,view_node_entry_p,index,magic,view_node_entry_p,status_b);
     if(FALSE == status_b)
     {
       status = CRM_FAILURE;
       break;
     }
     view_node_entry_p->magic = magic;
     view_node_entry_p->index = index;
     OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"View entry succesfuuly added");
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  return status;
}

int32_t crm_remove_vm_view_entry(char* view_name,char* view_value)
{
  struct   database_view_node *view_node_entry_p;
  int32_t  index , offset = 0;
  int32_t  status = CRM_SUCCESS;
  uint32_t hashkey;
  uint8_t  status_b,view_found_b = 0;
  
  do
  {
     for(index = 0;index<CRM_MAX_VIEW_DATABASE;index++)
     {
       if(!strcmp(vm_db_view[index].view,view_name))
       {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"View found for this view name");
         view_found_b = 1;
         break;
       }
     }
     if(view_found_b == 0)
     {
       OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"View not initiliazed for the view name.So no entries exist");
       status = CRM_FAILURE;
     }
     hashkey = crm_get_hashval_byname(view_value,crm_no_of_vm_buckets_g);
     CNTLR_RCU_READ_LOCK_TAKE(); 
     MCHASH_BUCKET_SCAN(vm_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
     {
      if ((!strcmp(view_node_entry_p->view_value, view_value)) &&
          (!strcmp(view_node_entry_p->node_name, view_name)))
      {
        status_b = FALSE;
        offset = 0;
        MCHASH_DELETE_NODE_BY_REF(vm_db_view[index].db_viewtbl_p,view_node_entry_p->index,view_node_entry_p->magic,struct database_view_node *,offset,status_b);
        if (status_b == FALSE)
        {
          status = CRM_FAILURE;
          break;
        }
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"View entry deleted successfully");
        status = CRM_SUCCESS;
        break;
      }
    }
  }while(0);
  
  CNTLR_RCU_READ_LOCK_RELEASE();
  return status;
}
 
int32_t crm_remove_vm_view(char *view_name)
{ 
  struct   database_view_node *view_node_entry_p; 
  int32_t index , offset ,hashkey , ret_val;
  uint8_t node_found_b = 0;

  for(index = 0;index<CRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(vm_db_view[index].view , view_name))
     {
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"View found to deleted");
        node_found_b = 1;
        break;
     }
  }
  if(node_found_b == 0)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_ERROR,"Node not found ");
    return CRM_FAILURE;
  }
  node_found_b = 0;
  offset = 0;

  MCHASH_TABLE_SCAN(vm_db_view[index].db_viewtbl_p,hashkey)
  {
    MCHASH_BUCKET_SCAN(vm_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
    {
      if(strcmp(view_node_entry_p->view_value , ""))
      {
         node_found_b = 1;
         break;
      }
    }
  }
  if(node_found_b == 1)
  {
     OF_LOG_MSG(OF_LOG_CRM,OF_LOG_ERROR,"Remove view entires to remove the entire view");
     return CRM_FAILURE;
  }
  ret_val = crm_view_hash_table_deinit(vm_db_view[index].db_viewtbl_p);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_ERROR,"Hash Table deinit failed");
    return CRM_FAILURE;
  }

  strcpy(vm_db_view[index].view,"");
  vm_db_view[index].uniqueness_flag = FALSE; //of_view_get_uniqueness(view_name);/*TBD*/
  vm_db_view[index].valid_b = FALSE;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "View node deleted succesfully");
  return CRM_SUCCESS;
}
 
int32_t crm_get_first_vm_view_entry(char *view_name , struct database_view_node *vm_view_node)
{
  int32_t  index;
  uint8_t  node_found_b = 0 ;
  int32_t  hashkey ,offset = 0;
  struct   database_view_node *view_node_entry_p;

  for(index = 0;index<CRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(vm_db_view[index].view , view_name))
     {
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"View found ");
        node_found_b = 1;
        break;
     }
  }
  if(node_found_b == 0)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_ERROR,"Node not found ");
    return CRM_FAILURE;
  }
  node_found_b = 0;
  offset = 0;
  CNTLR_RCU_READ_LOCK_TAKE();
  MCHASH_TABLE_SCAN(vm_db_view[index].db_viewtbl_p ,hashkey)
  {
    MCHASH_BUCKET_SCAN(vm_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
    {
      strcpy(vm_view_node->view_value , view_node_entry_p->view_value);
      strcpy(vm_view_node->node_name , view_node_entry_p->node_name);
      node_found_b = 1;
      break;
    }
    if(node_found_b)
      break;
  }
  if(node_found_b == 0)
  { 
     CNTLR_RCU_READ_LOCK_RELEASE();
     OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"Node entry NOT found");
     return CRM_FAILURE;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"Node entry found");
  return CRM_SUCCESS;

}

int32_t crm_get_next_vm_view_entry(char *view_name , char *view_value ,struct database_view_node *vm_view_node)

{
  int32_t  index;
  uint8_t  node_found_b = 0 ;
  int32_t  hashkey ,offset ;
  struct   database_view_node *view_node_entry_p;

  for(index = 0;index<CRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(vm_db_view[index].view , view_name))
     {
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"View found ");
        node_found_b = 1;
        break;
     }
  }
  if(node_found_b == 0)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_ERROR,"Node not found ");
    return CRM_FAILURE;
  }
  node_found_b =0;
  offset = 0;
  CNTLR_RCU_READ_LOCK_TAKE();
  MCHASH_TABLE_SCAN(vm_db_view[index].db_viewtbl_p ,hashkey)
  {
    MCHASH_BUCKET_SCAN(vm_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
    {
      if(node_found_b == 0)
      {
        if(!strcmp(view_node_entry_p->node_name , view_name) &&
          (!strcmp(view_node_entry_p->view_value , view_value)))
        {
           node_found_b = 1;
           continue;
        }
        else
        {
          continue;
        }
      }
      strcpy(vm_view_node->view_value , view_node_entry_p->view_value);
      strcpy(vm_view_node->node_name , view_node_entry_p->node_name);
      break;
    }
  }
  if(node_found_b == 0)
  {
     CNTLR_RCU_READ_LOCK_RELEASE();
     OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"Node entry NOT found");
     return CRM_FAILURE;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"Node entry found");
  return CRM_SUCCESS;
}


int32_t crm_get_exact_vm_view_entry(char *view_name , char *view_value ,struct database_view_node *vm_view_node)

{
  int32_t  index;
  uint8_t  node_found_b = 0 ;
  int32_t  hashkey ,offset ;
  struct   database_view_node *view_node_entry_p;

  for(index = 0;index<CRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(vm_db_view[index].view , view_name))
     {
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"View found ");
        node_found_b = 1;
        break;
     }
  }
  if(node_found_b == 0)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_ERROR,"Node not found ");
    return CRM_FAILURE;
  }
  node_found_b =0;
  offset = 0;
  CNTLR_RCU_READ_LOCK_TAKE();
  MCHASH_TABLE_SCAN(vm_db_view[index].db_viewtbl_p ,hashkey)
  {
    MCHASH_BUCKET_SCAN(vm_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
    {
       if(!strcmp(view_node_entry_p->node_name , view_name) &&
         (!strcmp(view_node_entry_p->view_value , view_value)))
       {
          node_found_b = 1;
          break;
       }
    }
    if(node_found_b)
      break;
  }
  if(node_found_b == 0)
  {
     CNTLR_RCU_READ_LOCK_RELEASE();
     OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"Node entry NOT found");
     return CRM_FAILURE;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  strcpy(vm_view_node->view_value , view_node_entry_p->view_value);
  strcpy(vm_view_node->node_name , view_node_entry_p->node_name);
  OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"Node entry found");
  return CRM_SUCCESS;
}
                                                                                      
int32_t crm_get_vm_by_view(char *view_name , char *view_value ,struct crm_vm_config_info* config_info_p)
{
  int32_t  index ,ret_val;
  uint8_t  node_found_b = 0 ;
  int32_t  hashkey ,offset ;
  struct   database_view_node  *view_node_entry_p = NULL;
  struct   crm_virtual_machine *crm_vm_node_info_p = NULL;

  for(index = 0;index<CRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(vm_db_view[index].view , view_name))
     {
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"View found ");
        node_found_b = 1;
        break;
     }
  }
  if(node_found_b == 0)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_ERROR,"Node not found ");
    return CRM_FAILURE;
  }
  node_found_b =0;
  offset = 0;
  CNTLR_RCU_READ_LOCK_TAKE();
  MCHASH_TABLE_SCAN(vm_db_view[index].db_viewtbl_p ,hashkey)
  {
    MCHASH_BUCKET_SCAN(vm_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
    {
       if(!strcmp(view_node_entry_p->node_name , view_name) &&
         (!strcmp(view_node_entry_p->view_value , view_value)))
       {
          node_found_b = 1;
          break;
       }
    }
    if(node_found_b)
      break;
  }
  if(node_found_b == 0)
  {
     CNTLR_RCU_READ_LOCK_RELEASE();
     OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"Node entry NOT found");
     return CRM_FAILURE;
  }
  ret_val = get_vm_byhandle(view_node_entry_p->node_saferef , &crm_vm_node_info_p);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"No VM exists");
    return CRM_FAILURE;
  }
  strcpy(config_info_p->vm_name, crm_vm_node_info_p->vm_name);
  strcpy(config_info_p->switch_name, crm_vm_node_info_p->switch_name_dprm);
  strcpy(config_info_p->tenant_name, crm_vm_node_info_p->tenant_name);
  strcpy(config_info_p->vm_desc,crm_vm_node_info_p->vm_desc);
  config_info_p->vm_type = crm_vm_node_info_p->vm_type;
 
  CNTLR_RCU_READ_LOCK_RELEASE();
  OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"VM found by view");
  return CRM_SUCCESS;
}
