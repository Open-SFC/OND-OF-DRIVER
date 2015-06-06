/** Cloud resource manager Virtual Network source file 
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

/* File  :      crm_vn_api.c
 */

/*
 *
 * File name: crm_vn_api.c
 * Author: Varun Kumar Reddy <B36461@freescale.com>
 * Date:   11/10/2013
 * Description: 
*/

/** Header files */
#include "crm_system_dependencies.h"
#include "crm_tenant_api.h"
#include "crm_vn_api.h"
#include "crm_vm_api.h"
#include "port_status_mgr.h"
#include "rrm_crm_rtlns_mgr.h" 

/** Declarations */
extern void     *crm_vn_mempool_g;
extern void     *crm_port_ref_mempool_g;
extern void     *crm_subnet_handle_mempool_g;
extern void     *crm_vm_handle_mempool_g;
extern void     *crm_vn_compute_nodes_mempool_g;
extern void     *crm_port_mempool_g;
extern void     *crm_notifications_mempool_g;

extern uint32_t crm_no_of_vn_buckets_g;
extern uint32_t crm_no_of_port_buckets_g;
extern struct   mchash_table* crm_vn_table_g;
extern struct   mchash_table* crm_vm_table_g;
extern struct   mchash_table* crm_port_table_g;

extern  of_list_t crm_vn_notifications[CRM_VN_LAST_NOTIFICATION_TYPE + 1];
//int32_t crm_get_vn_byhandle(uint64_t vn_handle, struct crm_virtual_network **crm_vn_info_p);
int32_t crm_del_vn_from_tenant(uint64_t tenant_handle, char *vn_name_p);
struct database_view vn_db_view[CRM_MAX_VIEW_DATABASE];

char crm_vn_application_names[NO_OF_CRM_VN_APPLICATIONS][CRM_MAX_APPL_NAME_LEN + 1];


/** Definitions */
/***********************************************************************************************/
void crm_vm_handle_free_entry_rcu(struct rcu_head *vm_handle_entry_p)
{
  struct crm_virtual_machine_handle* vm_handle_info_p = NULL;
  int32_t ret_value;	

  if(vm_handle_entry_p)
  {
    vm_handle_info_p = (struct crm_virtual_machine_handle *)(((char *)vm_handle_entry_p) - sizeof(of_list_node_t));
    ret_value = mempool_release_mem_block(crm_vm_handle_mempool_g,
      					  (uchar8_t*)vm_handle_info_p,
        			  	  vm_handle_info_p->heap_b);
    if(ret_value != MEMPOOL_SUCCESS)		
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Failed to release vmname_reference memory block");
    }
    else
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Deleted succesfully  vmname_reference node from  the database");
    }
  }
  else
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "NULL passed for deletion");
  }
}
/***************************************************************************************************/
void crm_vn_logical_switch_ports_free_entry_rcu(struct rcu_head *logical_switch_ports_entry_p)
{
  struct crm_port_ref  *logical_switch_ports_info_p=NULL;
  int32_t ret_value;	

  if(logical_switch_ports_entry_p)
  {
    logical_switch_ports_info_p = (struct crm_port_ref *)(((char *)logical_switch_ports_entry_p) - sizeof(of_list_node_t));
    ret_value = mempool_release_mem_block(crm_port_ref_mempool_g, (uchar8_t*)logical_switch_ports_info_p,logical_switch_ports_info_p->heap_b);
    if(ret_value != MEMPOOL_SUCCESS)		
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Failed to release crm logical switch ports  memory block");
    }
    else
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG,"Released succesfully  crm logical switch ports memory block");
    }   
  }
  else
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG,"NULL passed for deletion.");
}
/***********************************************************************************************/
void crm_compute_node_free_entry_rcu(struct rcu_head *compute_nodes_entry_p)
{
  struct  crm_compute_node *crm_compute_node_info_p=NULL;
  int32_t ret_value;	

  if(compute_nodes_entry_p)
  {
    crm_compute_node_info_p = (struct crm_compute_node *)(((char *)compute_nodes_entry_p) - sizeof(of_list_node_t));
    ret_value = mempool_release_mem_block(crm_vn_compute_nodes_mempool_g,
        (uchar8_t*)crm_compute_node_info_p,
        crm_compute_node_info_p->heap_b);
    if(ret_value != MEMPOOL_SUCCESS)		
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Failed to release crm compute node memory block");
    }
    else
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Deleted succesfully  crm compute node from  the database");
    }
  }
  else
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "NULL passed for deletion");
  }
}
/***********************************************************************************************/
void crm_subnet_handle_node_free_entry_rcu(struct rcu_head *subnet_handle_entry_p)
{
  struct crm_subnet_handle  *subnet_handle_info_p=NULL;
  int32_t ret_value;	

  if(subnet_handle_entry_p)
  {
    subnet_handle_info_p = (struct crm_subnet_handle *)(((char *)subnet_handle_entry_p) - sizeof(of_list_node_t));
    ret_value = mempool_release_mem_block(crm_subnet_handle_mempool_g,
        (uchar8_t*)subnet_handle_info_p,
        subnet_handle_info_p->heap_b);
    if(ret_value != MEMPOOL_SUCCESS)		
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Failed to release subnet handle memory block");
    }
    else
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Deleted succesfully  subnet handle node from  the database");
    }
  }
  else
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "NULL passed for deletion");
  }
}
/******************************************************************************/
int32_t crm_detach_subnet_handle_from_vn(char *subnet_name_p, char *vn_name_p)
{
  uint32_t hashkey,vn_node_found=0,subnet_handle_found=0,offset;
  uint32_t lstoffset;

  struct crm_subnet_handle *subnet_handle_node_prev_entry_p=NULL;
  struct crm_subnet_handle *subnet_handle_node_scan_entry_p=NULL;

  struct crm_virtual_network *crm_vn_info_p=NULL;

  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Entered");
  if(vn_name_p == NULL)return CRM_FAILURE;
  if(subnet_name_p == 0)return CRM_FAILURE;

  hashkey = crm_get_hashval_byname(vn_name_p, crm_no_of_vn_buckets_g);
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "hashkey:%d\r\n",hashkey);

  CNTLR_RCU_READ_LOCK_TAKE();
  offset = VNNODE_VNTBL_OFFSET;
  MCHASH_BUCKET_SCAN(crm_vn_table_g, hashkey, crm_vn_info_p, struct crm_virtual_network *,offset)
  {
    if(!strcmp(crm_vn_info_p->nw_name,vn_name_p))
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Virtual network found");
      vn_node_found = 1;
      break;
    }
  }
  if(!vn_node_found)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Virtual network not found");
    CNTLR_RCU_READ_LOCK_RELEASE();
    return CRM_FAILURE;
  }

  lstoffset = CRM_SUBNET_HANDLE_LISTNODE_OFFSET;
  OF_LIST_SCAN((crm_vn_info_p->crm_subnet_handles),
                subnet_handle_node_scan_entry_p,
                struct crm_subnet_handle *,             
                lstoffset)
  {
    if(!strcmp(subnet_handle_node_scan_entry_p->subnet_name, subnet_name_p))
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "subnet handle found in  VN node to detach.");
      subnet_handle_found=1;
      break;
    }    
    subnet_handle_node_prev_entry_p=subnet_handle_node_scan_entry_p;
  }

  if(!subnet_handle_found)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "subnet handle not found in  VN node to detach.");
    CNTLR_RCU_READ_LOCK_RELEASE();
    return CRM_FAILURE;   
  }

  OF_REMOVE_NODE_FROM_LIST((crm_vn_info_p->crm_subnet_handles), 
                            subnet_handle_node_scan_entry_p, 
                            subnet_handle_node_prev_entry_p,
                            lstoffset
                          );
  crm_vn_info_p->no_of_subnets--;

  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Subnet  handle  deleted success fully from the Virtual Network");
  CNTLR_RCU_READ_LOCK_RELEASE();
  return CRM_SUCCESS; 
}
/**************************************************************************************************
* Function:crm_attach_subnet_handle_to_vn
* Description:
*	Every virtual network node has subnet handles s. it will be add
*	from the add_subnet function.
**************************************************************************************************/
int32_t crm_attach_subnet_handle_to_vn(uint64_t vn_handle,
                                       char *subnet_name_p,
                                       uint64_t subnet_handle)
{
  int32_t ret_val;
  uint8_t heap_b;
  struct crm_subnet_handle* subnet_handle_info_p = NULL;
  struct crm_subnet_handle* subnet_handle_node_scan_entry_p = NULL;
  struct crm_virtual_network* crm_vn_info_p = NULL;

  CNTLR_RCU_READ_LOCK_TAKE();
  ret_val = crm_get_vn_byhandle(vn_handle, &crm_vn_info_p);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "crm_get_vn_byhandle failed!.");
    CNTLR_RCU_READ_LOCK_RELEASE();
    return CRM_ERROR_INVALID_VN_HANDLE;
  }		
  OF_LIST_SCAN((crm_vn_info_p->crm_subnet_handles),
      subnet_handle_node_scan_entry_p,
      struct crm_subnet_handle *,             
      CRM_SUBNET_HANDLE_LISTNODE_OFFSET)
  {
    if(!strcmp(subnet_handle_node_scan_entry_p->subnet_name, subnet_name_p))
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "subnet handle already attached to VN.");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return CRM_FAILURE;	
    }
  }

  ret_val = mempool_get_mem_block(crm_subnet_handle_mempool_g,
  	 		          (uchar8_t**)&subnet_handle_info_p,
  				  &heap_b);
  if(ret_val != MEMPOOL_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "memory block allocation failed.");	
    CNTLR_RCU_READ_LOCK_RELEASE();
    return CRM_FAILURE;	
  }
  subnet_handle_info_p->heap_b = heap_b;
  strncpy(subnet_handle_info_p->subnet_name, subnet_name_p, CRM_MAX_SUBNET_NAME_LEN);
  subnet_handle_info_p->subnet_handle = subnet_handle;

  OF_APPEND_NODE_TO_LIST((crm_vn_info_p->crm_subnet_handles), 
      subnet_handle_info_p,
      CRM_SUBNET_HANDLE_LISTNODE_OFFSET);

  crm_vn_info_p->no_of_subnets++;

  CNTLR_RCU_READ_LOCK_RELEASE();
  return CRM_SUCCESS;	
}
/***********************************************************************************************/
int32_t crm_dettach_vm_handle_from_vn(char *vm_name_p, char *vn_name_p)
{
  uint32_t hashkey,vn_node_found=0,vm_handle_found=0,offset,lstoffset;
  struct crm_virtual_machine_handle *vm_handle_node_prev_entry_p=NULL;
  struct crm_virtual_machine_handle *vm_handle_node_scan_entry_p=NULL;
  struct crm_virtual_network *crm_vn_info_p=NULL;

  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Entered");
  if(vn_name_p == NULL)return CRM_FAILURE;
  if(vm_name_p == 0)return CRM_FAILURE;

  hashkey = crm_get_hashval_byname(vn_name_p, crm_no_of_vn_buckets_g);
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "hashkey:%d\r\n",hashkey);

  CNTLR_RCU_READ_LOCK_TAKE();
  offset = VNNODE_VNTBL_OFFSET;
  MCHASH_BUCKET_SCAN(crm_vn_table_g, hashkey, crm_vn_info_p, struct crm_virtual_network *,offset)
  {
    if(!strcmp(crm_vn_info_p->nw_name,vn_name_p))
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Virtual network found");
      vn_node_found = 1;
      break;
    }
  }
  if(!vn_node_found)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Virtual network not found");
    CNTLR_RCU_READ_LOCK_RELEASE();
    return CRM_FAILURE;
  }

  lstoffset = CRM_VM_HANDLE_LISTNODE_OFFSET;
  OF_LIST_SCAN((crm_vn_info_p->crm_vm_handles),
  		vm_handle_node_scan_entry_p,
 		struct crm_virtual_machine_handle *,             
     		lstoffset)
  {
    if(!strcmp(vm_handle_node_scan_entry_p->vm_name, vm_name_p))
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "VM handle found in  VN node to detach.");
      vm_handle_found=1;
      break;
    }    
    vm_handle_node_prev_entry_p=vm_handle_node_scan_entry_p;
  }

  if(!vm_handle_found)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "VM handle not found in  VN node to detach.");
    CNTLR_RCU_READ_LOCK_RELEASE();
    return CRM_FAILURE;   
  }

  OF_REMOVE_NODE_FROM_LIST((crm_vn_info_p->crm_vm_handles), 
  		            vm_handle_node_scan_entry_p, 
                            vm_handle_node_prev_entry_p,
                            lstoffset
                           );
  crm_vn_info_p->no_of_vms--;

  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Virtual Machine handle  deleted success fully from the vn");

  CNTLR_RCU_READ_LOCK_RELEASE();
  return CRM_SUCCESS;   
}
/**************************************************************************************************
* Function:crm_attach_vm_handle_to_vn
* Description:
*	Every virtual network node has vm handles s. it will be add
*	from the vm module.
**************************************************************************************************/
int32_t crm_attach_vm_handle_to_vn(char *vn_name_p,char *vm_name_p,uint64_t vm_handle)
{
  int32_t  ret_val;
  uint32_t hashkey,vn_node_found=0,offset;
  uint8_t  heap_b;

  struct crm_virtual_machine_handle *vm_handle_info_p = NULL;
  struct crm_virtual_machine_handle *vm_handle_node_scan_entry_p = NULL;
  struct crm_virtual_network *crm_vn_info_p = NULL;

  if(vn_name_p == NULL)
    return CRM_FAILURE;
  if(vm_handle == 0)
    return CRM_FAILURE;

  hashkey = crm_get_hashval_byname(vn_name_p,crm_no_of_vn_buckets_g);
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "hashkey:%d\r\n",hashkey);
  CNTLR_RCU_READ_LOCK_TAKE();
  offset = VNNODE_VNTBL_OFFSET;

  MCHASH_BUCKET_SCAN(crm_vn_table_g, hashkey, crm_vn_info_p, struct crm_virtual_network *,offset)
  {
    if(!strcmp(crm_vn_info_p->nw_name,vn_name_p))
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Virtual network found");
      vn_node_found = 1;
      break;
    }
  }
  if(!vn_node_found)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Virtual network not found");
    CNTLR_RCU_READ_LOCK_RELEASE();
    return CRM_FAILURE;
  }

  OF_LIST_SCAN((crm_vn_info_p->crm_vm_handles),
                vm_handle_node_scan_entry_p,
                struct crm_virtual_machine_handle *,             
                CRM_VM_HANDLE_LISTNODE_OFFSET)
  {
    if(!strcmp(vm_handle_node_scan_entry_p->vm_name, vm_name_p))
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "VM handle already attached to VN.");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return CRM_FAILURE;	
    }
  }

  ret_val = mempool_get_mem_block(crm_vm_handle_mempool_g,(uchar8_t**)&vm_handle_info_p,&heap_b);
  if(ret_val != MEMPOOL_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "memory block allocation failed.");	
    CNTLR_RCU_READ_LOCK_RELEASE();
    return CRM_FAILURE;	
  }

  vm_handle_info_p->heap_b    = heap_b;
  strncpy(vm_handle_info_p->vm_name, vm_name_p, CRM_MAX_VM_NAME_LEN);
  vm_handle_info_p->vm_handle = vm_handle;

  OF_APPEND_NODE_TO_LIST((crm_vn_info_p->crm_vm_handles),vm_handle_info_p,CRM_VM_HANDLE_LISTNODE_OFFSET);
 
  crm_vn_info_p->no_of_vms++;

  CNTLR_RCU_READ_LOCK_RELEASE();
  return CRM_SUCCESS;	
}
/*************************************************************************************
Function:crm_crm_get_vn_byhandle
Description:
This function returns virtual network info for given vn_handle.
*************************************************************************************/
int32_t crm_get_vn_byhandle(uint64_t vn_handle,struct crm_virtual_network **crm_vn_info_p)
{
  /* Caller shall take RCU locks. */
  uint32_t index,magic;
  uint8_t status_b;

  CNTLR_RCU_READ_LOCK_TAKE();
  magic = (uint32_t)(vn_handle >>32);
  index = (uint32_t)vn_handle;

  MCHASH_ACCESS_NODE(crm_vn_table_g,index,magic,*crm_vn_info_p,status_b);
  CNTLR_RCU_READ_LOCK_RELEASE();
  if(TRUE == status_b)
    return CRM_SUCCESS;
  return CRM_ERROR_INVALID_VN_HANDLE;
}
/*********************************************************************************
Function:crm_get_vn_handle
Description:
This function returns the virtual network handle for the given vn name
**********************************************************************************/
int32_t crm_get_vn_handle(char *vn_name_p, uint64_t *vn_handle_p)
{
  uint32_t hashkey;
  uint32_t offset;

  struct crm_virtual_network *crm_vn_info_p=NULL;

  if(crm_vn_table_g == NULL)
    return CRM_FAILURE;

  hashkey = crm_get_hashval_byname(vn_name_p,crm_no_of_vn_buckets_g);
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "hashkey:%d\r\n",hashkey);
  CNTLR_RCU_READ_LOCK_TAKE();
  offset = VNNODE_VNTBL_OFFSET;

  MCHASH_BUCKET_SCAN(crm_vn_table_g, hashkey, crm_vn_info_p, struct crm_virtual_network *,offset)
  {
    if(strcmp(crm_vn_info_p->nw_name,vn_name_p))
      continue;

    *vn_handle_p = crm_vn_info_p->magic;
    *vn_handle_p = ((*vn_handle_p <<32) | (crm_vn_info_p->index));

    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "*vn_handle_p:%x\r\n",*vn_handle_p);
    CNTLR_RCU_READ_LOCK_RELEASE();
    return CRM_SUCCESS;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return CRM_ERROR_VN_NAME_INVALID;
}
/*****************************************************************************
Function: crm_add_virtual_network
Description:
This function adds vn config info to the vn database
******************************************************************************/
int32_t crm_add_virtual_network (struct crm_vn_config_info *config_info_p, 
				 uint64_t *output_vn_handle_p)
{
  int32_t   ret_value = CRM_FAILURE;
  struct    crm_virtual_network *crm_vn_node_p=NULL,*crm_vn_node_scan_p=NULL;
  uint32_t  nid = 0,hashkey, vnoffset, index, magic,apphookoffset;
  uint8_t   heap_b, status_b = FALSE;
  struct    crm_notification_app_hook_info *app_entry_p=NULL;
  struct    crm_vn_notification_data  notification_data={};
  struct    crm_tenant_config_info crm_tenant_config_info={};
  uint64_t  tenant_handle;
  uchar8_t* hashobj_p = NULL;

  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Entered");

  if(crm_vn_table_g == NULL)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "No hash table to add crm_vn record\r\n");
    return CRM_FAILURE;
  }	

  if((config_info_p->nw_name == NULL)|| 
      (strlen(config_info_p->nw_name) >= CRM_MAX_VN_NAME_LEN))
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "network name is invalid");
    return CRM_ERROR_VN_NAME_INVALID;
  }

  if((config_info_p->tenant_name == NULL)||
      (strlen(config_info_p->tenant_name)>=CRM_MAX_TENANT_NAME_LEN))
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "tenant name is invalid");
    return CRM_ERROR_TENANT_NAME_INVALID;
  }

#if 0
  if((config_info_p->subnet_name == NULL)||
      (strlen(config_info_p->subnet_name)>=CRM_MAX_SUBNET_NAME_LEN)
    )
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "subnet name is invalid");
    return CRM_ERROR_SUBNET_NAME_INVALID;
  }
#endif	

  if(!((config_info_p->nw_type == VLAN_TYPE)  ||
       (config_info_p->nw_type == VXLAN_TYPE) ||
       (config_info_p->nw_type == NVGRE_TYPE)))                           
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "network type invalid\r\n", __FUNCTION__, __LINE__);
    return CRM_ERROR_VN_NETWORK_TYPE_INVALID;
  }

  /*get tenant handle. if not present in db create tenant*/
  ret_value = crm_get_tenant_handle(config_info_p->tenant_name, &tenant_handle);
  if(ret_value != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Tenant not in db. create tenant.");
    strcpy(crm_tenant_config_info.tenant_name, config_info_p->tenant_name);
    ret_value = crm_add_tenant(&crm_tenant_config_info, &tenant_handle);
    if(ret_value!=CRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Failed to add tenant");
      return CRM_FAILURE;
    }
  }

  CNTLR_RCU_READ_LOCK_TAKE();

  /** calculate hash key for the Virtual network name **/
  hashkey = crm_get_hashval_byname(config_info_p->nw_name, crm_no_of_vn_buckets_g);
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "hashkey:%d",hashkey);


  vnoffset = VNNODE_VNTBL_OFFSET;

  /** Scan VN hash table for the name **/ 
  MCHASH_BUCKET_SCAN(crm_vn_table_g, hashkey, crm_vn_node_scan_p, struct crm_virtual_network*, vnoffset)
  {
    if(strcmp(config_info_p->nw_name, crm_vn_node_scan_p->nw_name) != 0)
      continue;
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Duplicate virtual network name %s",config_info_p->nw_name);
    ret_value = CRM_ERROR_DUPLICATE_VN_RESOURCE;
    goto CRM_ADD_VN_EXIT;
  }

  /** Create memory for the entry */
  ret_value = mempool_get_mem_block(crm_vn_mempool_g, (uchar8_t **)&crm_vn_node_p, &heap_b);
  if(ret_value != MEMPOOL_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "memory block allocation failed for the virtual network node.");
    ret_value = CRM_FAILURE;
    goto CRM_ADD_VN_EXIT;
  }
  
  memset(crm_vn_node_p , 0 , sizeof(crm_vn_node_p) + (NO_OF_CRM_VN_APPLICATIONS * sizeof(void *)));
  crm_vn_node_p->heap_b = heap_b;

  /* virtual network name*/
  strncpy(crm_vn_node_p->nw_name, config_info_p->nw_name, CRM_MAX_VN_NAME_LEN);

  /** copy vn info to vn database**/ 

  strncpy(crm_vn_node_p->tenant_name, config_info_p->tenant_name, CRM_MAX_TENANT_NAME_LEN);
  crm_vn_node_p->saferef_tenant = tenant_handle;

  /* network type */ 
  crm_vn_node_p->nw_type = config_info_p->nw_type;

  if(config_info_p->nw_type ==  VXLAN_TYPE)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "vxlan type");
    crm_vn_node_p->nw_params.vxlan_nw.nid = config_info_p->nw_params.vxlan_nw.nid;
    nid = config_info_p->nw_params.vxlan_nw.nid;
    crm_vn_node_p->nw_params.vxlan_nw.service_port = config_info_p->nw_params.vxlan_nw.service_port;
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "%d:%d",config_info_p->nw_params.vxlan_nw.nid,config_info_p->nw_params.vxlan_nw.service_port);
  }
  else if(config_info_p->nw_type == NVGRE_TYPE)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "nvgre type");
    crm_vn_node_p->nw_params.nvgre_nw.nid = config_info_p->nw_params.nvgre_nw.nid;
    nid = config_info_p->nw_params.nvgre_nw.nid;
    crm_vn_node_p->nw_params.nvgre_nw.service_port = config_info_p->nw_params.nvgre_nw.service_port;
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "%d:%d",config_info_p->nw_params.nvgre_nw.nid,config_info_p->nw_params.nvgre_nw.service_port);
  }
  else if(config_info_p->nw_type ==  VLAN_TYPE)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "vlan type");
    crm_vn_node_p->nw_params.vlan_nw.vlan_id = config_info_p->nw_params.vlan_nw.vlan_id;
    nid = config_info_p->nw_params.vlan_nw.vlan_id;
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "%d",config_info_p->nw_params.vlan_nw.vlan_id);
  }


  /*  init for computenodes,vm handles,attributes,subnet handles */
  OF_LIST_INIT(crm_vn_node_p->crm_compute_nodes,  crm_compute_node_free_entry_rcu);
  OF_LIST_INIT(crm_vn_node_p->crm_subnet_handles, crm_subnet_handle_node_free_entry_rcu);
  OF_LIST_INIT(crm_vn_node_p->attributes,         crm_attributes_free_attribute_entry_rcu);
  OF_LIST_INIT(crm_vn_node_p->crm_vm_handles,     crm_vm_handle_free_entry_rcu);

  strncpy(crm_vn_node_p->nw_desc, config_info_p->nw_desc, CRM_MAX_VN_DESC_LEN);   
  crm_vn_node_p->vn_operational_status_b = config_info_p->vn_operational_status_b;
  strncpy(crm_vn_node_p->provider_physical_network, config_info_p->provider_physical_network,CRM_MAX_VN_PROVIDER_PHY_NW);
  crm_vn_node_p->vn_admin_state_up_b = config_info_p->vn_admin_state_up_b;
  crm_vn_node_p->router_external_b = config_info_p->router_external_b;

  hashobj_p = (uchar8_t *)crm_vn_node_p + VNNODE_VNTBL_OFFSET;

  MCHASH_APPEND_NODE(crm_vn_table_g, hashkey, crm_vn_node_p, index, magic, hashobj_p, status_b);
  if(FALSE == status_b)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, " failed to append virtual network node to hash table");
    ret_value = CRM_ERROR_VN_ADD_FAIL;
    goto CRM_ADD_VN_EXIT;
  }

  *output_vn_handle_p = magic;
  *output_vn_handle_p = ((*output_vn_handle_p <<32) | (index));

  crm_vn_node_p->magic = magic;
  crm_vn_node_p->index = index;
  (crm_vn_node_p->vn_handle) = *output_vn_handle_p;

  if(crm_add_vn_to_tenant(tenant_handle, crm_vn_node_p->nw_name) != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "add vn to tenant failed!.");
  }
  apphookoffset = CRM_NOTIFICATION_APP_HOOK_OFFSET;
  strcpy(notification_data.vn_name, crm_vn_node_p->nw_name);
  notification_data.nw_type = crm_vn_node_p->nw_type; 
  notification_data.tun_id  = nid;

  OF_LIST_SCAN(crm_vn_notifications[CRM_VN_ADDED], app_entry_p, struct crm_notification_app_hook_info *, apphookoffset )
  {
    ((crm_vn_notifier_fn)(app_entry_p->call_back))(CRM_VN_ADDED,
  			  *output_vn_handle_p,
   			   notification_data,
 			   app_entry_p->cbk_arg1,
 			   app_entry_p->cbk_arg2);
  }  

  OF_LIST_SCAN(crm_vn_notifications[CRM_VN_ALL_NOTIFICATIONS], app_entry_p, struct crm_notification_app_hook_info *, apphookoffset )
  {
    ((crm_vn_notifier_fn)(app_entry_p->call_back))(CRM_VN_ADDED,
  			  *output_vn_handle_p,
  			  notification_data,
			  app_entry_p->cbk_arg1,
    	                  app_entry_p->cbk_arg2);
  }             
  CNTLR_RCU_READ_LOCK_RELEASE();
  OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "virtual network added successfully to the vn-db");
  return CRM_SUCCESS;

CRM_ADD_VN_EXIT:
  if(crm_vn_node_p)
  {
    mempool_release_mem_block(crm_vn_mempool_g, (uchar8_t *)crm_vn_node_p, crm_vn_node_p->heap_b);
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "virtual network failed  to add vn-db");
  return ret_value;
}
/*****************************************************************************
function: crm_mod_virtual_network
description:
this function adds vn config info to the vn database
******************************************************************************/
int32_t crm_modify_virtual_network (struct crm_vn_config_info *config_info_p, 
				    uint32_t field_flags)
{
  int32_t   ret_value=CRM_FAILURE;
  struct    crm_virtual_network *crm_vn_node_scan_p=NULL;
  uint32_t  hashkey, vnoffset,apphookoffset;
  struct    crm_notification_app_hook_info *app_entry_p=NULL;
  struct    crm_vn_notification_data  notification_data={};
  uint64_t  tenant_handle;

  OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "entered");

  if(crm_vn_table_g == NULL)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "no hash table to add crm_vn record\r\n");
    return CRM_FAILURE;
  }	

  if((config_info_p->nw_name == NULL)|| 
      (strlen(config_info_p->nw_name)>=CRM_MAX_VN_NAME_LEN))
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "network name is invalid");
    return CRM_ERROR_VN_NAME_INVALID;
  }

  if((config_info_p->tenant_name == NULL)||
      (strlen(config_info_p->tenant_name)>=CRM_MAX_TENANT_NAME_LEN))
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "tenant name is invalid");
    return CRM_ERROR_TENANT_NAME_INVALID;
  }

  if(!((config_info_p->nw_type == VLAN_TYPE)||
        (config_info_p->nw_type ==VXLAN_TYPE)))
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "network type invalid\r\n", __FUNCTION__, __LINE__);
    return CRM_ERROR_VN_NETWORK_TYPE_INVALID;
  }

  /** calculate hash key for the virtual network name **/
  hashkey = crm_get_hashval_byname(config_info_p->nw_name, crm_no_of_vn_buckets_g);
  OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "hashkey:%d",hashkey);

  vnoffset = VNNODE_VNTBL_OFFSET;

  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {  
    /** scan vn hash table for the name **/ 
    MCHASH_BUCKET_SCAN(crm_vn_table_g, hashkey, crm_vn_node_scan_p, struct crm_virtual_network*, vnoffset)
    {
      if(strcmp(config_info_p->nw_name, crm_vn_node_scan_p->nw_name) != 0)
        continue;
      OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "virtual network name %s found",config_info_p->nw_name);
    }

    /** copy vn info to vn database**/ 
    /* verify tenant*/
    if(strcmp(crm_vn_node_scan_p->tenant_name,config_info_p->tenant_name)!=0)
    {
      if(crm_del_vn_from_tenant(crm_vn_node_scan_p->saferef_tenant, crm_vn_node_scan_p->nw_name) != CRM_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "del vn from tenant failed!.");
        ret_value = CRM_FAILURE;
        break;
      } 
      ret_value = crm_get_tenant_handle(config_info_p->tenant_name, &tenant_handle);
      if(ret_value != CRM_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "tenant name invalid");
        ret_value = CRM_ERROR_TENANT_NAME_INVALID;
        break;
      }
      strncpy(crm_vn_node_scan_p->tenant_name, config_info_p->tenant_name, CRM_MAX_TENANT_NAME_LEN);
      crm_vn_node_scan_p->saferef_tenant = tenant_handle;
      if(crm_add_vn_to_tenant(tenant_handle, crm_vn_node_scan_p->nw_name) != CRM_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "add vn to tenant failed!.");
      } 
    }

    {
      /* network type */ 
      crm_vn_node_scan_p->nw_type = config_info_p->nw_type;
      if(config_info_p->nw_type ==  VLAN_TYPE)
      {
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "vlan type");
        crm_vn_node_scan_p->nw_params.vlan_nw.vlan_id = config_info_p->nw_params.vlan_nw.vlan_id;
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "%d",config_info_p->nw_params.vlan_nw.vlan_id);
      }
      else if(config_info_p->nw_type ==  VXLAN_TYPE)
      {
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "vxlan type");
        crm_vn_node_scan_p->nw_params.vxlan_nw.nid = config_info_p->nw_params.vxlan_nw.nid;
        crm_vn_node_scan_p->nw_params.vxlan_nw.service_port = config_info_p->nw_params.vxlan_nw.service_port;
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "%d:%d",config_info_p->nw_params.vxlan_nw.nid,config_info_p->nw_params.vxlan_nw.service_port);
      }
      strncpy(crm_vn_node_scan_p->nw_desc, config_info_p->nw_desc, CRM_MAX_VN_DESC_LEN);   
      crm_vn_node_scan_p->vn_operational_status_b = config_info_p->vn_operational_status_b;
      strncpy(crm_vn_node_scan_p->provider_physical_network, config_info_p->provider_physical_network,CRM_MAX_VN_PROVIDER_PHY_NW);
      crm_vn_node_scan_p->vn_admin_state_up_b = config_info_p->vn_admin_state_up_b;
      crm_vn_node_scan_p->router_external_b = config_info_p->router_external_b;
    }
    
    apphookoffset = CRM_NOTIFICATION_APP_HOOK_OFFSET;
    strcpy(notification_data.vn_name, crm_vn_node_scan_p->nw_name);
    OF_LIST_SCAN(crm_vn_notifications[CRM_VN_MODIFIED], app_entry_p, struct crm_notification_app_hook_info *, apphookoffset )
    {
      ((crm_vn_notifier_fn)(app_entry_p->call_back))(CRM_VN_MODIFIED,
   			   crm_vn_node_scan_p->vn_handle,
      		           notification_data,
                           app_entry_p->cbk_arg1,
                           app_entry_p->cbk_arg2);
    }  

    OF_LIST_SCAN(crm_vn_notifications[CRM_VN_ALL_NOTIFICATIONS], app_entry_p, struct crm_notification_app_hook_info *, apphookoffset )
    {
      ((crm_vn_notifier_fn)(app_entry_p->call_back))(CRM_VN_MODIFIED,
                            crm_vn_node_scan_p->vn_handle,
                            notification_data,
                            app_entry_p->cbk_arg1,
                            app_entry_p->cbk_arg2);
    }             
    ret_value=CRM_SUCCESS;

  }while(0);

  CNTLR_RCU_READ_LOCK_RELEASE();
  OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "virtual network modified successfully in the vn-db");

  return ret_value;
}
/*****************************************************************************************************
* function : crm_register_vn_notifications
* description:
*	this api provide interface  to other apps to register callbacks . so that when ever
*	any vn node adds/modifies/deletes callbacks will get invoke.
*****************************************************************************************************/
int32_t crm_register_vn_notifications (uint32_t notification_type, crm_vn_notifier_fn vn_notifier_fn,
				       void* callback_arg1, void* callback_arg2)
{
  struct  crm_notification_app_hook_info *app_entry_p;
  uint8_t heap_b;
  int32_t retval = CRM_SUCCESS;
  uchar8_t lstoffset;
  lstoffset = CRM_NOTIFICATION_APP_HOOK_OFFSET;

  if((notification_type < CRM_VN_FIRST_NOTIFICATION_TYPE) ||
      (notification_type > CRM_VN_LAST_NOTIFICATION_TYPE)
    )
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "invalid notification type:%d", notification_type);
    return CRM_FAILURE;
  }

  if(vn_notifier_fn == NULL)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "callback function is NULL");
    return CRM_FAILURE;
  }

  retval = mempool_get_mem_block(crm_notifications_mempool_g, (uchar8_t**)&app_entry_p, &heap_b);
  if(retval != MEMPOOL_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "failed to get mempool");
    return CRM_FAILURE;
  }

  app_entry_p->call_back = (void*)vn_notifier_fn;
  app_entry_p->cbk_arg1  = callback_arg1;
  app_entry_p->cbk_arg2  = callback_arg2;
  app_entry_p->heap_b    = heap_b;

  /* add application to the  of virtual network notifications */
  OF_APPEND_NODE_TO_LIST(crm_vn_notifications[notification_type], app_entry_p, lstoffset);

  if(retval != CRM_SUCCESS)
    mempool_release_mem_block(crm_notifications_mempool_g, (uchar8_t*)app_entry_p, app_entry_p->heap_b); 
  return retval; 
}
/****************************************************************************************************************/
int32_t crm_deregister_vn_notifications(uint32_t notification_type,
                                        crm_vn_notifier_fn vn_notifier_fn)
{
  struct crm_notification_app_hook_info *app_entry_p,*prev_app_entry_p = NULL;
  uchar8_t lstoffset;
  lstoffset = CRM_NOTIFICATION_APP_HOOK_OFFSET;

  if((notification_type < CRM_VN_FIRST_NOTIFICATION_TYPE) ||
     (notification_type > CRM_VN_LAST_NOTIFICATION_TYPE))
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "invalid vn notification type:%d", notification_type);
    return CRM_FAILURE;
  }

  if(vn_notifier_fn == NULL)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "callback function is NULL");
    return CRM_FAILURE;
  }

  CNTLR_RCU_READ_LOCK_TAKE();
  OF_LIST_SCAN(crm_vn_notifications[notification_type],app_entry_p,struct crm_notification_app_hook_info *,lstoffset )
  {
    if(app_entry_p->call_back != vn_notifier_fn)
    {
      prev_app_entry_p = app_entry_p;
      continue;
    }
    OF_REMOVE_NODE_FROM_LIST(crm_vn_notifications[notification_type],app_entry_p,prev_app_entry_p,lstoffset );
    CNTLR_RCU_READ_LOCK_RELEASE();
    return CRM_SUCCESS;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return CRM_FAILURE;
}
/***************************************************************************************************************/
int32_t crm_del_virtual_network (char*  nw_name_p)
{
  int32_t  ret_val;
  int32_t  vn_node_found=0;
  uint32_t offset, hashkey,magic,index;
  uint8_t status_b;

  struct crm_virtual_network *crm_vn_node_scan_p=NULL;

  if(nw_name_p == NULL)
    return CRM_ERROR_VM_NAME_NULL;

  if(crm_vn_table_g == NULL)
    return CRM_FAILURE;

  OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "nw_name %s",nw_name_p);

  CNTLR_RCU_READ_LOCK_TAKE();

  do
  {
    /** calculate hash key for the virtual network name */
    hashkey = crm_get_hashval_byname(nw_name_p, crm_no_of_vn_buckets_g);
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "hashkey:%d\r\n",hashkey);

    offset = VNNODE_VNTBL_OFFSET;

    /** scan vn hash table for the name */
    MCHASH_BUCKET_SCAN(crm_vn_table_g, hashkey, crm_vn_node_scan_p, struct crm_virtual_network*, offset)
    {
      if(!strcmp(nw_name_p, crm_vn_node_scan_p->nw_name))
      {
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "virtual network found to delete");
        vn_node_found = 1;
        break;
      }
    }

    if(vn_node_found == 0)
    {
      OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "virtual network not found to delete");
      ret_val= CRM_ERROR_VN_NAME_INVALID;
      break;	
    }

    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "no of compute_nodes:%d , no of attributes:%d,no of vms:%d,no of subnets:%d",crm_vn_node_scan_p->no_of_compute_nodes,crm_vn_node_scan_p->number_of_attributes,crm_vn_node_scan_p->no_of_vms,crm_vn_node_scan_p->no_of_subnets);

    if((crm_vn_node_scan_p->no_of_compute_nodes)||
        (crm_vn_node_scan_p->number_of_attributes)||
        (crm_vn_node_scan_p->no_of_subnets)||
        (crm_vn_node_scan_p->no_of_vms)
      )
    {
      OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, " virtual network has attached . first delete it.");
      ret_val= CRM_ERROR_RESOURCE_NOTEMPTY;
      break;
    }

    if(crm_del_vn_from_tenant(crm_vn_node_scan_p->saferef_tenant, crm_vn_node_scan_p->nw_name) != CRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "del vn from tenant failed!.");
      ret_val = CRM_FAILURE;
      break;
    } 

    status_b = FALSE;
    offset = VNNODE_VNTBL_OFFSET;
    magic = (crm_vn_node_scan_p->magic);
    index = (crm_vn_node_scan_p->index);
    MCHASH_DELETE_NODE_BY_REF(crm_vn_table_g,index,magic,struct crm_virtual_network*, offset, status_b);
    if(status_b == TRUE)
      ret_val = CRM_SUCCESS;
    else
      ret_val = CRM_FAILURE;        
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  return ret_val;  
}
/*************************************************************************/
int32_t crm_get_vn_by_name(char* nw_name,  
			   struct crm_vn_config_info* config_info_p,
    		           struct vn_runtime_info* runtime_info)
{
  int32_t   ret_val=CRM_FAILURE;
  uint8_t   vn_node_found=0;	
  uint32_t  hashkey, offset;
  struct    crm_virtual_network *crm_vn_node_scan_p=NULL;

  if(crm_vn_table_g == NULL)
    return CRM_FAILURE;

  if(nw_name ==  NULL)
    return CRM_ERROR_VM_NAME_NULL;

  OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "%s:%d nw_name %s\r\n",__FUNCTION__, __LINE__,nw_name);
  CNTLR_RCU_READ_LOCK_TAKE();

  /** calculate hash key for the virtual network name */
  hashkey = crm_get_hashval_byname(nw_name, crm_no_of_vn_buckets_g);
  OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "hashkey:%d\r\n",hashkey);

  offset = VNNODE_VNTBL_OFFSET;

  /** scan vn hash table for the name */
  MCHASH_BUCKET_SCAN(crm_vn_table_g, hashkey, crm_vn_node_scan_p, struct crm_virtual_network*, offset)
  {
    if(!strcmp(nw_name, crm_vn_node_scan_p->nw_name))
    {
      OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "%s:%d virtual network found\r\n",__FUNCTION__, __LINE__);
      vn_node_found = 1;

      break;
    }
  }
  if(vn_node_found == 1)
  {
    strcpy(config_info_p->nw_name, crm_vn_node_scan_p->nw_name);
    strcpy(config_info_p->tenant_name, crm_vn_node_scan_p->tenant_name);
    config_info_p->nw_type = crm_vn_node_scan_p->nw_type;
    /* network type */ 
    if(crm_vn_node_scan_p->nw_type ==  VLAN_TYPE)
    {
      config_info_p->nw_params.vlan_nw.vlan_id = crm_vn_node_scan_p->nw_params.vlan_nw.vlan_id;
    }
    else if(crm_vn_node_scan_p->nw_type ==  VXLAN_TYPE)
    {
      config_info_p->nw_params.vxlan_nw.nid = crm_vn_node_scan_p->nw_params.vxlan_nw.nid;
      config_info_p->nw_params.vxlan_nw.service_port = crm_vn_node_scan_p->nw_params.vxlan_nw.service_port;
    }
    strncpy(config_info_p->nw_desc, crm_vn_node_scan_p->nw_desc, CRM_MAX_VN_DESC_LEN);   


    config_info_p->vn_operational_status_b = crm_vn_node_scan_p->vn_operational_status_b;
    strncpy(config_info_p->provider_physical_network, crm_vn_node_scan_p->provider_physical_network,CRM_MAX_VN_PROVIDER_PHY_NW);
    config_info_p->vn_admin_state_up_b = crm_vn_node_scan_p->vn_admin_state_up_b;
    config_info_p->router_external_b = crm_vn_node_scan_p->router_external_b;		
    ret_val = CRM_SUCCESS;
  }
  else
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "%s:%d virtual network not found\r\n",__FUNCTION__, __LINE__);
    ret_val = CRM_ERROR_VN_NAME_INVALID;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return ret_val;
}
/***************************************************************************/
int32_t  crm_get_first_virtual_network (struct crm_vn_config_info* config_info_p,
   					struct vn_runtime_info* vn_runtime_info,
  					uint64_t* vn_handle_p)
{
  struct    crm_virtual_network   *crm_vn_node_scan_p=NULL;
  uint32_t hashkey;

  uint32_t offset;
  OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "%s:%d \r\n",__FUNCTION__, __LINE__);

  if(crm_vn_table_g == NULL)
    return CRM_FAILURE;

  CNTLR_RCU_READ_LOCK_TAKE();

  MCHASH_TABLE_SCAN(crm_vn_table_g, hashkey)
  {
    offset = VNNODE_VNTBL_OFFSET;
    MCHASH_BUCKET_SCAN(crm_vn_table_g, hashkey, crm_vn_node_scan_p, struct    crm_virtual_network *, offset)
    {
      OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "copying vn config\r\n");
      strncpy(config_info_p->nw_name, crm_vn_node_scan_p->nw_name,CRM_MAX_VN_NAME_LEN);
      strncpy(config_info_p->tenant_name, crm_vn_node_scan_p->tenant_name,CRM_MAX_TENANT_NAME_LEN);

      config_info_p->nw_type = crm_vn_node_scan_p->nw_type;
      /* network type */ 
      if(crm_vn_node_scan_p->nw_type ==  VLAN_TYPE)
      {
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "vlan type\r\n");
        config_info_p->nw_params.vlan_nw.vlan_id = crm_vn_node_scan_p->nw_params.vlan_nw.vlan_id;
      }
      else if(crm_vn_node_scan_p->nw_type ==  VXLAN_TYPE)
      {
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "vxlan type\r\n");
        config_info_p->nw_params.vxlan_nw.nid = crm_vn_node_scan_p->nw_params.vxlan_nw.nid;
        config_info_p->nw_params.vxlan_nw.service_port = crm_vn_node_scan_p->nw_params.vxlan_nw.service_port;
      }
      strncpy(config_info_p->nw_desc, crm_vn_node_scan_p->nw_desc, CRM_MAX_VN_DESC_LEN);
      config_info_p->vn_operational_status_b = crm_vn_node_scan_p->vn_operational_status_b;
      strncpy(config_info_p->provider_physical_network, crm_vn_node_scan_p->provider_physical_network,
          CRM_MAX_VN_PROVIDER_PHY_NW);
      config_info_p->vn_admin_state_up_b = crm_vn_node_scan_p->vn_admin_state_up_b;
      config_info_p->router_external_b = crm_vn_node_scan_p->router_external_b;

      //runtime_info_p->number_of_attributes = crm_vm_node_info_p->number_of_attributes;

      *vn_handle_p = crm_vn_node_scan_p->magic;
      *vn_handle_p = ((*vn_handle_p <<32) | (crm_vn_node_scan_p->index));

      CNTLR_RCU_READ_LOCK_RELEASE();
      return CRM_SUCCESS;
    }
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return CRM_ERROR_NO_MORE_ENTRIES;
}
/***************************************************************************/
int32_t crm_get_next_virtual_network(struct crm_vn_config_info* config_info_p,
   				      struct vn_runtime_info* vn_runtime_info,
   			              uint64_t* vn_handle_p)
{
  struct    crm_virtual_network   *crm_vn_node_scan_p=NULL;
  uint32_t hashkey;
  uint8_t current_entry_found_b;
  uint32_t offset = VNNODE_VNTBL_OFFSET;

  OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "%s(%d)\r\n",__FUNCTION__,__LINE__);

  CNTLR_RCU_READ_LOCK_TAKE();
  current_entry_found_b = FALSE;

  MCHASH_TABLE_SCAN(crm_vn_table_g, hashkey)
  {
    offset = VNNODE_VNTBL_OFFSET;
    MCHASH_BUCKET_SCAN(crm_vn_table_g, hashkey, crm_vn_node_scan_p, struct    crm_virtual_network *,offset)
    {
      if(current_entry_found_b == FALSE)
      {
        if(*vn_handle_p != (crm_vn_node_scan_p->vn_handle))
          continue;
        else
        {
          current_entry_found_b = TRUE;
          continue;
        }
      }
      OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "%s:%d copying vn config\r\n",__FUNCTION__, __LINE__);
      strncpy(config_info_p->nw_name, crm_vn_node_scan_p->nw_name,CRM_MAX_VN_NAME_LEN);
      strncpy(config_info_p->tenant_name, crm_vn_node_scan_p->tenant_name,CRM_MAX_TENANT_NAME_LEN);

      config_info_p->nw_type = crm_vn_node_scan_p->nw_type;
      /* network type */ 
      if(crm_vn_node_scan_p->nw_type ==  VLAN_TYPE)
      {
        config_info_p->nw_params.vlan_nw.vlan_id = crm_vn_node_scan_p->nw_params.vlan_nw.vlan_id;
      }
      else if(crm_vn_node_scan_p->nw_type ==  VXLAN_TYPE)
      {
        config_info_p->nw_params.vxlan_nw.nid = crm_vn_node_scan_p->nw_params.vxlan_nw.nid;
        config_info_p->nw_params.vxlan_nw.service_port = crm_vn_node_scan_p->nw_params.vxlan_nw.service_port;
      }
      strncpy(config_info_p->nw_desc, crm_vn_node_scan_p->nw_desc, CRM_MAX_VN_DESC_LEN);
      config_info_p->vn_operational_status_b = crm_vn_node_scan_p->vn_operational_status_b;
      strncpy(config_info_p->provider_physical_network, crm_vn_node_scan_p->provider_physical_network,
          CRM_MAX_VN_PROVIDER_PHY_NW);
      config_info_p->vn_admin_state_up_b = crm_vn_node_scan_p->vn_admin_state_up_b;
      config_info_p->router_external_b = crm_vn_node_scan_p->router_external_b;

      //runtime_info_p->number_of_attributes = crm_vm_node_info_p->number_of_attributes;

      *vn_handle_p = crm_vn_node_scan_p->magic;
      *vn_handle_p = ((*vn_handle_p <<32) | (crm_vn_node_scan_p->index));
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
/*********************************************************************************
* function: crm_add_crm_compute_node_to_vn
* description:
*	this function attaches the compute nodes to the virtual network node
*	.it checks vn ,switch and logical switch(bridge name) exsist or not.
*	it there it keeps all the handles.	
*********************************************************************************/
int32_t crm_add_crm_compute_node_to_vn(char *vn_name_p,
 				       char *switch_name_p,
  			               char *bridge_name_p)
{
  int32_t  ret_val=CRM_SUCCESS,index,crm_compute_node_found=0;
  uint64_t vn_handle, switch_handle,logical_switch_handle;
  uint8_t  heap_b;
  struct   crm_virtual_network *crm_vn_info_p=NULL;
  struct   crm_compute_node *crm_compute_node_entry_p = NULL;
  uint32_t insert_index=0;

  if(crm_vn_table_g == NULL)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "no hash table to add records\r\n");
    return CRM_FAILURE;	
  }

  /*  vn_name validation */ 
  if((vn_name_p == NULL) || (strlen(vn_name_p)>=CRM_MAX_VN_NAME_LEN))
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "virtual network name is invalid");
    return CRM_ERROR_VN_NAME_INVALID;
  }

  /* get the vn handle and vn_info */
  ret_val = crm_get_vn_handle(vn_name_p, &vn_handle);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "crm_get_vn_handle failed!.");
    return CRM_ERROR_VN_NAME_INVALID;
  }

  ret_val = crm_get_vn_byhandle(vn_handle, &crm_vn_info_p);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "crm_get_vn_byhandle failed!.");
    return CRM_ERROR_INVALID_VN_HANDLE;
  }	

  /* switch name validation */
  if((switch_name_p == NULL)||
      (strlen(switch_name_p)>= DPRM_MAX_SWITCH_NAME_LEN)
    )
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "switch name is invalid");
    return CRM_ERROR_INVALID_NAME;
  }

  /* switch in dprm db or not */
  ret_val = dprm_get_switch_handle(switch_name_p, &switch_handle);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "switch name is invalid");
    return CRM_ERROR_INVALID_NAME;
  }

  /* logical switches validation */
  if((bridge_name_p == NULL)||
      (strlen(bridge_name_p)>= CRM_LOGICAL_SWITCH_NAME_LEN))
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "birdige name are invalid");
    return CRM_FAILURE;
  }
  ret_val = dprm_get_logical_switch_handle(switch_handle,bridge_name_p,&logical_switch_handle);
  if(ret_val!=CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "get logical switch handle failed for logical switch name:%s!",bridge_name_p);
    return CRM_FAILURE;
  }

  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    OF_LIST_SCAN((crm_vn_info_p->crm_compute_nodes),
        crm_compute_node_entry_p,
        struct crm_compute_node *,		
        CRM_VN_COMPUTE_NODE_ENTRY_LISTNODE_OFFSET)

    {
      if(!strcmp(crm_compute_node_entry_p->switch_name, switch_name_p))
      {
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "compute node found . no of bridges:%d",crm_compute_node_entry_p->no_of_bridges);
        crm_compute_node_found = 1;
        /** check max limit **/
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "1");
        if((crm_compute_node_entry_p->no_of_bridges) == CRM_MAX_DPS_IN_A_COMPUTE_NODE)
        {
          OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "max no of bridges per computenode reached.");
          CNTLR_RCU_READ_LOCK_RELEASE();
          return CRM_FAILURE;
        }
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "2");
        /* to avoid duplicate bridge names */
        for(index=0; index<(crm_compute_node_entry_p->no_of_bridges); index++)
        {

          OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "logical_switch_name:%s",(crm_compute_node_entry_p->dp_references[index]).logical_switch_name);
          if(!strcmp((crm_compute_node_entry_p->dp_references[index]).logical_switch_name, bridge_name_p))
          {
            OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "duplicate bridge %s found.",bridge_name_p);
            CNTLR_RCU_READ_LOCK_RELEASE();
            return CRM_SUCCESS;  /* ns-br this is required as this call is repeated for every port added */
            // return CRM_FAILURE;
          }
        }
        insert_index = crm_compute_node_entry_p->no_of_bridges;
        break;
      }
    } 
    if(!crm_compute_node_found)
    { 
      OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "creating new crm computenode");
      /* allocate the memory for the crm compute node */
      ret_val = mempool_get_mem_block(crm_vn_compute_nodes_mempool_g,
          (uchar8_t**)&crm_compute_node_entry_p,
          &heap_b);
      if(ret_val != MEMPOOL_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "memory block allocation failed for crm compute node.");
        ret_val = CRM_FAILURE;
        break;	
      }
      crm_compute_node_entry_p->heap_b = heap_b;

      /* copy vn name and vn handle */ 
      strcpy(crm_compute_node_entry_p->vn_name, vn_name_p); 
      crm_compute_node_entry_p->saferef_vn = vn_handle;

      /* copy switch and switch handle */
      strcpy(crm_compute_node_entry_p->switch_name, switch_name_p); 
      crm_compute_node_entry_p->saferef_switch = switch_handle;
    }

    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "insert_index:%d",insert_index);
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "bridge_name_p:%s",bridge_name_p);
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "vn_name_p:%s",vn_name_p);
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "switch_name_p:%s",switch_name_p);

    strcpy(((crm_compute_node_entry_p->dp_references[insert_index]).logical_switch_name),bridge_name_p);

    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "crm_compute_node_entry_p->dp_references[insert_index]).logical_switch_name:%s",(crm_compute_node_entry_p->dp_references[insert_index]).logical_switch_name);
    (crm_compute_node_entry_p->dp_references[insert_index]).saferef_logical_switch = logical_switch_handle;

    OF_LIST_INIT((crm_compute_node_entry_p->dp_references[insert_index]).vmside_ports, crm_vn_logical_switch_ports_free_entry_rcu);

    OF_LIST_INIT((crm_compute_node_entry_p->dp_references[insert_index]).nwside_unicast_ports, crm_vn_logical_switch_ports_free_entry_rcu);

    OF_LIST_INIT((crm_compute_node_entry_p->dp_references[insert_index]).nwside_broadcast_ports, crm_vn_logical_switch_ports_free_entry_rcu);	

    OF_LIST_INIT((crm_compute_node_entry_p->dp_references[insert_index]).vmns_ports, crm_vn_logical_switch_ports_free_entry_rcu);

    crm_compute_node_entry_p->no_of_bridges++; 

    if(!crm_compute_node_found)
    {
      OF_APPEND_NODE_TO_LIST((crm_vn_info_p->crm_compute_nodes), 
          crm_compute_node_entry_p,
          CRM_VN_COMPUTE_NODE_ENTRY_LISTNODE_OFFSET);
      crm_vn_info_p->no_of_compute_nodes++;    
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  return ret_val;
}
/***************************************************************************/
int32_t  crm_del_crm_compute_node_from_vn(char* vn_name_p, char* sw_name_p)
{
  int32_t ret_val=CRM_SUCCESS,lstoffset;
  uint64_t vn_handle;
  struct crm_virtual_network *crm_vn_info_p=NULL;
  struct crm_compute_node *crm_compute_node_scan_entry_p = NULL;
  struct crm_compute_node *crm_compute_node_prev_node_p = NULL;
  uint32_t compute_node_found =0,ii;

  if((vn_name_p == NULL)||
      (sw_name_p ==  NULL)
    )
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "vn or switch name invalid ");
    return CRM_FAILURE;
  }

  if(crm_vn_table_g == NULL)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "no hash table to add records\r\n");
    return CRM_FAILURE;	
  }

  /* get the vn handle and vn_info */
  ret_val = crm_get_vn_handle(vn_name_p, &vn_handle);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "crm_get_vn_handle failed!.");
    return CRM_ERROR_VN_NAME_INVALID;
  }

  ret_val = crm_get_vn_byhandle(vn_handle, &crm_vn_info_p);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "crm_get_vn_byhandle failed!.");
    return CRM_ERROR_INVALID_VN_HANDLE;
  }	

  lstoffset = CRM_VN_COMPUTE_NODE_ENTRY_LISTNODE_OFFSET; 
  CNTLR_RCU_READ_LOCK_TAKE();

  do
  {
    OF_LIST_SCAN((crm_vn_info_p->crm_compute_nodes),
        crm_compute_node_scan_entry_p,
        struct crm_compute_node *,		
        lstoffset)
    {
      if(!strcmp(crm_compute_node_scan_entry_p->switch_name, sw_name_p))
      {
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "compute node found to delete");
        compute_node_found=1;
        break;	
      }
      crm_compute_node_prev_node_p = crm_compute_node_scan_entry_p;	
    }
    if(compute_node_found == 0)
    {
      OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "compute node not found to delete");
      ret_val=CRM_FAILURE;
      break;	
    }

    for(ii=0;ii<(crm_compute_node_scan_entry_p->no_of_bridges);ii++)
    {
      if((crm_compute_node_scan_entry_p->dp_references[ii].no_of_ports) >0)
      {
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "ports attached to bridge. we cannot delete compute node. detach ports from brdige first");

        CNTLR_RCU_READ_LOCK_RELEASE();
        return CRM_ERROR_RESOURCE_NOTEMPTY;
      }
    }

    OF_REMOVE_NODE_FROM_LIST((crm_vn_info_p->crm_compute_nodes), 
        crm_compute_node_scan_entry_p, 
        crm_compute_node_prev_node_p,
        lstoffset
        );
    crm_vn_info_p->no_of_compute_nodes--;  
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "compute node deleted success fully from the vn");

  }while(0);  
  CNTLR_RCU_READ_LOCK_RELEASE();
  return ret_val; 
}
/****************************************************************************************/
int32_t crm_get_first_crm_compute_node(char *vn_name_p, char* switch_name_p)
{
  int32_t ret_val,lstoffset;
  uint64_t vn_handle;
  struct crm_virtual_network *crm_vn_info_p=NULL;
  struct crm_compute_node *crm_compute_node_scan_entry_p = NULL;

  if((vn_name_p == NULL)||
      (switch_name_p ==  NULL)
    )
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "vn or switch name invalid ");
    return CRM_FAILURE;
  }

  if(crm_vn_table_g == NULL)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "no hash table to add records\r\n");
    return CRM_FAILURE;
  }

  /* get the vn handle and vn_info */
  ret_val = crm_get_vn_handle(vn_name_p, &vn_handle);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "crm_get_vn_handle failed!.");
    return CRM_ERROR_VN_NAME_INVALID;
  }

  ret_val = crm_get_vn_byhandle(vn_handle, &crm_vn_info_p);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "crm_get_vn_byhandle failed!.");
    return CRM_ERROR_INVALID_VN_HANDLE;
  }

  if(crm_vn_info_p->no_of_compute_nodes ==  0)
  {
    return CRM_ERROR_NO_MORE_ENTRIES;
  }

  ret_val = CRM_ERROR_NO_MORE_ENTRIES;
  lstoffset = CRM_VN_COMPUTE_NODE_ENTRY_LISTNODE_OFFSET;
  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    OF_LIST_SCAN((crm_vn_info_p->crm_compute_nodes),
        crm_compute_node_scan_entry_p,
        struct crm_compute_node *,
        lstoffset)
    {
      break;
    }
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "switch_name:%s",crm_compute_node_scan_entry_p->switch_name);
    strcpy(switch_name_p, crm_compute_node_scan_entry_p->switch_name);
    ret_val=CRM_SUCCESS;
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  return ret_val;
}
/***************************************************************************************/
int32_t crm_get_next_crm_compute_node (char *vn_name_p, char* switch_name_p)
{
  int32_t ret_val,lstoffset;
  uint64_t vn_handle;
  struct crm_virtual_network *crm_vn_info_p=NULL;
  struct crm_compute_node *crm_compute_node_scan_entry_p = NULL;
  uint32_t current_entry_found_b=0;

  OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "switch_name:%s",switch_name_p);
  if((vn_name_p == NULL)||
      (switch_name_p ==  NULL)
    )
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "vn or switch name invalid ");
    return CRM_FAILURE;
  }

  if(crm_vn_table_g == NULL)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "no hash table to add records\r\n");
    return CRM_FAILURE;
  }

  /* get the vn handle and vn_info */
  ret_val = crm_get_vn_handle(vn_name_p, &vn_handle);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "crm_get_vn_handle failed!.");
    return CRM_ERROR_VN_NAME_INVALID;
  }

  ret_val = crm_get_vn_byhandle(vn_handle, &crm_vn_info_p);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "crm_get_vn_byhandle failed!.");
    return CRM_ERROR_INVALID_VN_HANDLE;
  }
  if(crm_vn_info_p->no_of_compute_nodes ==  0)
  {
    return CRM_FAILURE;
  }

  lstoffset = CRM_VN_COMPUTE_NODE_ENTRY_LISTNODE_OFFSET;
  CNTLR_RCU_READ_LOCK_TAKE();


  ret_val = CRM_ERROR_NO_MORE_ENTRIES;
  do{
    OF_LIST_SCAN((crm_vn_info_p->crm_compute_nodes),
        crm_compute_node_scan_entry_p,
        struct crm_compute_node *,
        lstoffset)
    {
      if(current_entry_found_b == 0)
      {
        if(!strcmp(crm_compute_node_scan_entry_p->switch_name, switch_name_p))
        {
          OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "switch entry found");
          current_entry_found_b = 1;
          continue;
        }
        else
          continue;
      }    
      else
      {
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "next switch_name:%s",crm_compute_node_scan_entry_p->switch_name);
        strcpy(switch_name_p, crm_compute_node_scan_entry_p->switch_name);
        ret_val=CRM_SUCCESS;
        break;
      }
    }
    if(current_entry_found_b == 0)
    {
      OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "switch entry not found");
      ret_val = CRM_ERROR_INVALID_NAME;
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  return ret_val;
}
/*****************************************************************************/
int32_t crm_get_exact_crm_compute_node_by_name(char *vn_name_p, char *switch_name_p)
{
  int32_t ret_val=CRM_FAILURE,lstoffset;
  uint64_t vn_handle;
  struct crm_virtual_network *crm_vn_info_p=NULL;
  struct crm_compute_node *crm_compute_node_scan_entry_p = NULL;

  if((vn_name_p == NULL)|| (switch_name_p ==  NULL)
    )
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "vn or switch name invalid ");
    return CRM_FAILURE;
  }

  /* get the vn handle and vn_info */
  ret_val = crm_get_vn_handle(vn_name_p, &vn_handle);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "crm_get_vn_handle failed!.");
    return CRM_ERROR_VN_NAME_INVALID;
  }

  ret_val = crm_get_vn_byhandle(vn_handle, &crm_vn_info_p);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "crm_get_vn_byhandle failed!.");
    return CRM_ERROR_INVALID_VN_HANDLE;
  }
  if(crm_vn_info_p->no_of_compute_nodes ==  0)
  {
    return CRM_FAILURE;
  }

  lstoffset = CRM_VN_COMPUTE_NODE_ENTRY_LISTNODE_OFFSET;
  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    OF_LIST_SCAN((crm_vn_info_p->crm_compute_nodes),
        crm_compute_node_scan_entry_p,
        struct crm_compute_node *,
        lstoffset)
    {
      if(!strcmp(crm_compute_node_scan_entry_p->switch_name, switch_name_p))
      {
        ret_val=CRM_SUCCESS;
        break;
      }
    }    
  }while(0);

  CNTLR_RCU_READ_LOCK_RELEASE();
  return ret_val;
}
/***********************************************************************************/
int32_t crm_get_first_crm_logical_switch(char* vn_name_p, 
    					 char* switch_name_p,
    					 char* logical_switch_name_p)
{
  int32_t ret_val=CRM_SUCCESS,lstoffset;
  uint64_t vn_handle;
  struct crm_virtual_network *crm_vn_info_p=NULL;
  struct crm_compute_node *crm_compute_node_scan_entry_p = NULL;
  uint32_t compute_node_entry_found_b=FALSE;

  OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "entered(vn_name:%s,switch_name:%s,logical_switch_name:%s)",vn_name_p,switch_name_p,logical_switch_name_p);
  if((vn_name_p == NULL)||
      (switch_name_p ==  NULL)
    )
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "vn or switch name invalid ");
    return CRM_FAILURE;
  }

  if(crm_vn_table_g == NULL)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "no hash table to add records\r\n");
    return CRM_FAILURE;
  }

  /* get the vn handle and vn_info */
  ret_val = crm_get_vn_handle(vn_name_p, &vn_handle);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "crm_get_vn_handle failed!.");
    return CRM_ERROR_VN_NAME_INVALID;
  }

  ret_val = crm_get_vn_byhandle(vn_handle, &crm_vn_info_p);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "crm_get_vn_byhandle failed!.");
    return CRM_ERROR_INVALID_VN_HANDLE;
  }
  if(crm_vn_info_p->no_of_compute_nodes ==  0)
  {
    return CRM_FAILURE;
  }

  lstoffset = CRM_VN_COMPUTE_NODE_ENTRY_LISTNODE_OFFSET;
  CNTLR_RCU_READ_LOCK_TAKE();

  do
  {
    OF_LIST_SCAN((crm_vn_info_p->crm_compute_nodes),
        crm_compute_node_scan_entry_p,
        struct crm_compute_node *,
        lstoffset)
    {
      if(!strcmp(crm_compute_node_scan_entry_p->switch_name, switch_name_p))
      {
        compute_node_entry_found_b = TRUE;
        break;      
      }    
    }
    if(compute_node_entry_found_b == FALSE)
    {
      ret_val = CRM_ERROR_DUPLICATE_COMPUTE_NODE;
      break; 
    }
    else
    {
      strcpy(logical_switch_name_p,(crm_compute_node_scan_entry_p->dp_references[0]).logical_switch_name);
      OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "logical_switch_name:%s",logical_switch_name_p);
      ret_val=CRM_SUCCESS;
      break;    
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  return ret_val;
}
/***************************************************************************/
int32_t crm_get_next_crm_logical_switch(char* vn_name_p, 
    char* switch_name_p,
    char* logical_switch_name_p)
{
  int32_t ret_val=CRM_SUCCESS,lstoffset;
  uint64_t vn_handle;
  struct crm_virtual_network *crm_vn_info_p=NULL;
  struct crm_compute_node *crm_compute_node_scan_entry_p = NULL;
  uint32_t compute_node_entry_found_b=FALSE,index;
  uint32_t bridge_entry_found=FALSE;

  OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "entered(vn_name:%s,switch_name:%s,logical_switch_name:%s)",vn_name_p,switch_name_p,logical_switch_name_p);
  if((vn_name_p == NULL)||(switch_name_p ==  NULL))
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "vn or switch name invalid ");
    return CRM_FAILURE;
  }

  /* get the vn handle and vn_info */
  ret_val = crm_get_vn_handle(vn_name_p, &vn_handle);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "crm_get_vn_handle failed!.");
    return CRM_ERROR_VN_NAME_INVALID;
  }

  ret_val = crm_get_vn_byhandle(vn_handle, &crm_vn_info_p);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "crm_get_vn_byhandle failed!.");
    return CRM_ERROR_INVALID_VN_HANDLE;
  }
  if(crm_vn_info_p->no_of_compute_nodes ==  0)
  {
    return CRM_FAILURE;
  }

  lstoffset = CRM_VN_COMPUTE_NODE_ENTRY_LISTNODE_OFFSET;
  CNTLR_RCU_READ_LOCK_TAKE();

  do
  {
    OF_LIST_SCAN((crm_vn_info_p->crm_compute_nodes),
        crm_compute_node_scan_entry_p,
        struct crm_compute_node *,
        lstoffset)
    {
      if(!strcmp(crm_compute_node_scan_entry_p->switch_name, switch_name_p))
      {
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "compute node found!");
        compute_node_entry_found_b = 1;
        break;      
      }    
    }
    if(compute_node_entry_found_b == 0)
    {
      ret_val = CRM_ERROR_DUPLICATE_COMPUTE_NODE;
      break; 
    }
    else
    {
      for(index=0; index<(crm_compute_node_scan_entry_p->no_of_bridges); index++)
      {
        if(!strcmp(logical_switch_name_p,(crm_compute_node_scan_entry_p->dp_references[index]).logical_switch_name))
        {
          OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "brdige found!");
          bridge_entry_found =TRUE;
          break;
        }
      }
      if((bridge_entry_found==TRUE)&&(strlen((crm_compute_node_scan_entry_p->dp_references[index+1]).logical_switch_name)))
      {
        strcpy(logical_switch_name_p, (crm_compute_node_scan_entry_p->dp_references[index+1]).logical_switch_name);
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "next logical_switch_name:%s",logical_switch_name_p);
        ret_val=CRM_SUCCESS;
        break;
      }
      else
      {
        ret_val=CRM_ERROR_NO_MORE_ENTRIES;
        break;
      }
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  return ret_val;
}
/*****************************************************************************************************************/
/** attributes apis **/
/****************************************************************************************************************/
int32_t crm_add_attribute_to_vn(uint64_t vn_handle, struct crm_attribute_name_value_pair* attribute_info_p)
{
  int32_t  retval;
  struct   crm_virtual_network            *vn_entry_p;
  struct   crm_vn_notification_data        notification_data;
  struct   crm_notification_app_hook_info *app_entry_p;
  uchar8_t lstoffset;
  lstoffset = CRM_NOTIFICATION_APP_HOOK_OFFSET;

  OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "entered");

  CNTLR_RCU_READ_LOCK_TAKE();

  retval = crm_get_vn_byhandle(vn_handle, &vn_entry_p);
  if(retval != CRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return retval;
  }

  retval = crm_add_attribute(&(vn_entry_p->attributes),attribute_info_p);
  if(retval != CRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return retval;
  }

  vn_entry_p->number_of_attributes++;
  crm_create_vn_view_entry(vn_handle, attribute_info_p->name_string, attribute_info_p->value_string);

  /* send add attribute to switch notification to registered applications. */
  strcpy(notification_data.vn_name, vn_entry_p->nw_name);
  strcpy(notification_data.attribute_name,attribute_info_p->name_string);
  strcpy(notification_data.attribute_value,attribute_info_p->value_string);
  OF_LIST_SCAN(crm_vn_notifications[CRM_VN_ATTRIBUTE_ADDED],app_entry_p,struct crm_notification_app_hook_info*,lstoffset )
  {
    ((crm_vn_notifier_fn)(app_entry_p->call_back))(CRM_VN_ATTRIBUTE_ADDED,
    vn_handle,
    notification_data,
    app_entry_p->cbk_arg1,
    app_entry_p->cbk_arg2);
  }

  OF_LIST_SCAN(crm_vn_notifications[CRM_VN_ALL_NOTIFICATIONS],app_entry_p,struct crm_notification_app_hook_info*,lstoffset )
  {
    ((crm_vn_notifier_fn)(app_entry_p->call_back))(CRM_VN_ATTRIBUTE_ADDED,
    vn_handle,
    notification_data,
    app_entry_p->cbk_arg1,
    app_entry_p->cbk_arg2);
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return CRM_SUCCESS;
}
/**************************************************************************/
int32_t crm_delete_attribute_from_vn(uint64_t vn_handle, struct crm_attribute_name_value_pair *attribute_info_p)
{
  int32_t  retval;
  struct   crm_virtual_network            *vn_entry_p;
  struct   crm_resource_attribute_entry *attribute_entry_p,*prev_app_entry_p = NULL;

  struct   crm_vn_notification_data   notification_data;
  struct   crm_notification_app_hook_info *app_entry_p;
  uchar8_t lstattrnameoffset;
  lstattrnameoffset = CRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET;

  OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "entered");

  CNTLR_RCU_READ_LOCK_TAKE();

  retval = crm_get_vn_byhandle(vn_handle, &vn_entry_p);
  if(retval != CRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return retval;
  }
  OF_LIST_SCAN(vn_entry_p->attributes, attribute_entry_p, struct crm_resource_attribute_entry*,lstattrnameoffset)
  {
    if(!strcmp(attribute_entry_p->name, attribute_info_p->name_string))
    {
      strcpy(notification_data.vn_name, vn_entry_p->nw_name);
      strcpy(notification_data.attribute_name,attribute_entry_p->name);

      strcpy(notification_data.attribute_value,attribute_entry_p->value);
      OF_LIST_SCAN(crm_vn_notifications[CRM_VN_ATTRIBUTE_DELETE], app_entry_p, struct crm_notification_app_hook_info*,
          lstattrnameoffset )
      {
        ((crm_vn_notifier_fn)(app_entry_p->call_back))(CRM_VN_ATTRIBUTE_DELETE,
        vn_handle,
        notification_data,
        app_entry_p->cbk_arg1,
        app_entry_p->cbk_arg2);
      }

      OF_LIST_SCAN(crm_vn_notifications[CRM_VN_ALL_NOTIFICATIONS],app_entry_p,struct crm_notification_app_hook_info*,lstattrnameoffset)
      {
        ((crm_vn_notifier_fn)(app_entry_p->call_back))(CRM_VN_ATTRIBUTE_DELETE,
        vn_handle,
        notification_data,
        app_entry_p->cbk_arg1,
        app_entry_p->cbk_arg2);
      }
      OF_REMOVE_NODE_FROM_LIST(vn_entry_p->attributes,attribute_entry_p,prev_app_entry_p, lstattrnameoffset);
      vn_entry_p->number_of_attributes--;

      crm_remove_vn_view_entry(attribute_entry_p->name, attribute_entry_p->value);
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
int32_t crm_get_vn_first_attribute(uint64_t vn_handle,
    struct   crm_attribute_name_value_output_info *attribute_output_p)
{
  int32_t  retval;
  struct crm_virtual_network *crm_vn_entry_p=NULL;

  if(attribute_output_p == NULL)
    return CRM_FAILURE;

  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    retval = crm_get_vn_byhandle(vn_handle, &crm_vn_entry_p);
    if(retval != CRM_SUCCESS)
    { 
      retval = CRM_ERROR_INVALID_VN_HANDLE;
      break;
    }
    retval = crm_get_first_attribute(&(crm_vn_entry_p->attributes), attribute_output_p);

  }while(0);

  CNTLR_RCU_READ_LOCK_RELEASE();
  return retval;
}
/*****************************************************************************************************/
int32_t crm_get_vn_next_attribute(uint64_t vn_handle,
    				  char*   current_attribute_name,
    				  struct  crm_attribute_name_value_output_info *attribute_output_p)
{
  int32_t retval;
  struct crm_virtual_network *crm_vn_entry_p=NULL;

  if((attribute_output_p == NULL) || (current_attribute_name == NULL))
    return CRM_FAILURE;

  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    retval = crm_get_vn_byhandle(vn_handle,&crm_vn_entry_p);
    if(retval != CRM_SUCCESS)
    {
      retval= CRM_ERROR_INVALID_VN_HANDLE;
      break;
    }
    retval = crm_get_next_attribute(&(crm_vn_entry_p->attributes), current_attribute_name, attribute_output_p);
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  return retval;
}
/******************************************************************************/
int32_t crm_get_vn_exact_attribute(uint64_t vn_handle,char *attribute_name_string, struct   crm_attribute_name_value_output_info *attribute_output_p)
{
  struct   crm_virtual_network *crm_vn_entry_p=NULL;
  int32_t  retval;

  if(attribute_name_string == NULL)
    return CRM_ERROR_ATTRIBUTE_NAME_NULL;

  if(strlen(attribute_name_string) > CRM_MAX_ATTRIBUTE_NAME_LEN)
    return CRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN;

  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    retval = crm_get_vn_byhandle(vn_handle,&crm_vn_entry_p);
    if(retval != CRM_SUCCESS)
    {
      retval= CRM_ERROR_INVALID_VN_HANDLE;
      break;	
    }
    retval = crm_get_exact_attribute(&(crm_vn_entry_p->attributes),  attribute_name_string,attribute_output_p);
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  return retval;
}
/***************************************************************************************************************/
int32_t crm_vn_register_application(char* appln_name_p , uint32_t* offset_p)
{
   int8_t ii =0 , found_b = FALSE;

   if(appln_name_p == NULL)
   {
     OF_LOG_MSG(OF_LOG_CRM,OF_LOG_ERROR,"Application name is NULL");
     return CRM_FAILURE;
   }
   if(strlen(appln_name_p) > CRM_MAX_APPL_NAME_LEN )
   {
     OF_LOG_MSG(OF_LOG_CRM,OF_LOG_ERROR,"Application name length max limit exceeds");
     return CRM_FAILURE;
   }

   for( ii = 0 ; ii < NO_OF_CRM_VN_APPLICATIONS ; ii++)
   {
     if(strcmp(crm_vn_application_names[ii] , appln_name_p) == 0)
     {
       OF_LOG_MSG(OF_LOG_CRM,OF_LOG_ERROR,"Application already registered");
       return CRM_FAILURE;
     }
   }

   for( ii = 0 ; ii < NO_OF_CRM_VN_APPLICATIONS ; ii++)
   {
     if(strcmp(crm_vn_application_names[ii] , "") == 0)
     {
       strncpy(crm_vn_application_names[ii] , appln_name_p , CRM_MAX_APPL_NAME_LEN);
       found_b = TRUE;
       break;
     }
   }
   if(found_b == FALSE)
   {
     OF_LOG_MSG(OF_LOG_CRM,OF_LOG_ERROR, "Application registration failure");
     return CRM_FAILURE;
   }

   *offset_p = sizeof(struct crm_virtual_network)  + (ii * sizeof(void *));

   OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"application %s registered succesfully",crm_vn_application_names[ii]);
   return CRM_SUCCESS;
}
/************************************************************************************************************/
int32_t crm_vn_unregister_application(char* appname_p)
{
  uint8_t ii = 0 , found_b = FALSE;

  if(appname_p == NULL)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_ERROR,"Application name is NULL");
    return CRM_FAILURE;
  }
  for( ii = 0 ; ii < NO_OF_CRM_VN_APPLICATIONS ; ii++)
  {
    if(strcmp(crm_vn_application_names[ii] ,appname_p) == 0)
    {
      found_b = TRUE;
      break;
    }
  }
  if(found_b == TRUE)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"Application Node to be deleted is found");
    strcpy(crm_vn_application_names[ii], "");
    return CRM_SUCCESS;
  }
  else
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"Application Node to be deleted is NOT found");
    return CRM_FAILURE;
  }
}
/******************************************************************************************************/
int32_t crm_create_vn_view(char *view_name)
{
  int32_t index , ii;
  uint8_t empty_index_b = 0;

  for(ii = 0;ii<CRM_MAX_VIEW_DATABASE ; ii++)
  {
     if(!strcmp(vn_db_view[ii].view , view_name))
     {
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"View already created");
        return CRM_FAILURE;
     } 
     else if((empty_index_b == 0) && (!strcmp(vn_db_view[ii].view ,"")))    
     {
        empty_index_b = 1;
        index = ii;
     }
  } 
  if(empty_index_b == 0)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_ERROR,"View array full . Cannot create new view");
    return CRM_FAILURE;
  }
  strcpy(vn_db_view[index].view,view_name);
  crm_view_hash_table_init(&vn_db_view[index].db_viewtbl_p);
  vn_db_view[index].uniqueness_flag = FALSE; 
  vn_db_view[index].valid_b = TRUE;
 
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "View node created succesfully");
  return CRM_SUCCESS;
}
  

int32_t crm_create_vn_view_entry(uint64_t vn_handle , char* view_name ,char* view_value)
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
       if(!strcmp(vn_db_view[view_index].view ,view_name))
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
     
     hashkey = crm_get_hashval_byname(view_value,crm_no_of_vn_buckets_g);
     CNTLR_RCU_READ_LOCK_TAKE();
     view_node_entry_p = crm_alloc_view_entry_and_set_values(vn_handle, view_name, view_value);
     if (view_node_entry_p == NULL)
     {
       status = CRM_FAILURE;
       break;
     }
     MCHASH_APPEND_NODE(vn_db_view[view_index].db_viewtbl_p,hashkey,view_node_entry_p,index,magic,view_node_entry_p,status_b);
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

 
int32_t crm_remove_vn_view_entry(char* view_name,char* view_value)
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
       if(!strcmp(vn_db_view[index].view,view_name))
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
     hashkey = crm_get_hashval_byname(view_value,crm_no_of_vn_buckets_g);
     CNTLR_RCU_READ_LOCK_TAKE(); 
     MCHASH_BUCKET_SCAN(vn_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
     {
      if ((!strcmp(view_node_entry_p->view_value, view_value)) &&
          (!strcmp(view_node_entry_p->node_name, view_name)))
      {
        status_b = FALSE;
        offset = 0;
        MCHASH_DELETE_NODE_BY_REF(vn_db_view[index].db_viewtbl_p,view_node_entry_p->index,view_node_entry_p->magic,struct database_view_node *,offset,status_b);
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
 
int32_t crm_remove_vn_view(char *view_name)
{
  int32_t  index , ret_val;
  uint8_t  node_found_b = 0 ;
  uint64_t hashkey ,offset ;
  struct   database_view_node *view_node_entry_p;

  for(index = 0;index<CRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(vn_db_view[index].view , view_name))
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
  
  MCHASH_TABLE_SCAN(vn_db_view[index].db_viewtbl_p,hashkey)
  {
    MCHASH_BUCKET_SCAN(vn_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
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
  ret_val = crm_view_hash_table_deinit(vn_db_view[index].db_viewtbl_p); 
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_ERROR,"Hash Table deinit failed");
    return CRM_FAILURE;
  }
  strcpy(vn_db_view[index].view,"");
  vn_db_view[index].uniqueness_flag = FALSE; 
  vn_db_view[index].valid_b = FALSE;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "View node deleted succesfully");
  return CRM_SUCCESS;
}


int32_t crm_get_first_vn_view_entry(char *view_name , struct database_view_node *vn_view_node)
{
  int32_t  index;
  uint8_t  node_found_b = 0 ;
  int32_t  hashkey ,offset ;
  struct   database_view_node *view_node_entry_p;

  for(index = 0;index<CRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(vn_db_view[index].view , view_name))
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
  MCHASH_TABLE_SCAN(vn_db_view[index].db_viewtbl_p ,hashkey)
  {
    MCHASH_BUCKET_SCAN(vn_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
    {
      strcpy(vn_view_node->view_value , view_node_entry_p->view_value);
      strcpy(vn_view_node->node_name , view_node_entry_p->node_name);
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
   
int32_t crm_get_next_vn_view_entry(char *view_name , char *view_value ,struct database_view_node *vn_view_node)

{
  int32_t  index;
  uint8_t  node_found_b = 0 ;
  int32_t  hashkey ,offset ;
  struct   database_view_node *view_node_entry_p;

  for(index = 0;index<CRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(vn_db_view[index].view , view_name))
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
  MCHASH_TABLE_SCAN(vn_db_view[index].db_viewtbl_p , hashkey)
  {
    MCHASH_BUCKET_SCAN(vn_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
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
      strcpy(vn_view_node->view_value , view_node_entry_p->view_value);
      strcpy(vn_view_node->node_name , view_node_entry_p->node_name);
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

int32_t crm_get_exact_vn_view_entry(char *view_name , char *view_value ,struct database_view_node *vn_view_node)

{
  int32_t  index;
  uint8_t  node_found_b = 0 ;
  int32_t  hashkey ,offset ;
  struct   database_view_node *view_node_entry_p;

  for(index = 0;index<CRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(vn_db_view[index].view , view_name))
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
  MCHASH_TABLE_SCAN(vn_db_view[index].db_viewtbl_p ,hashkey)
  {
    MCHASH_BUCKET_SCAN(vn_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
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
  strcpy(vn_view_node->view_value , view_node_entry_p->view_value);
  strcpy(vn_view_node->node_name , view_node_entry_p->node_name);
  OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"Node entry found");
  return CRM_SUCCESS;
}
 
int32_t crm_get_vn_by_view(char *view_name,char *view_value ,struct crm_vn_config_info *config_info_p)
{
  struct  crm_virtual_network *vn_info_p = NULL;
  struct  database_view_node  *view_node_entry_p = NULL;
  int32_t index ,ret_val , hashkey,offset;
  uint8_t node_found_b = 0;

  for(index = 0;index<CRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(vn_db_view[index].view , view_name))
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
  MCHASH_TABLE_SCAN(vn_db_view[index].db_viewtbl_p ,hashkey)
  {
    MCHASH_BUCKET_SCAN(vn_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
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
  ret_val = crm_get_vn_byhandle(view_node_entry_p->node_saferef , &vn_info_p);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_ERROR,"No VN found for the given handle");
    return CRM_FAILURE;
  }
  strncpy(config_info_p->nw_name, vn_info_p->nw_name,CRM_MAX_VN_NAME_LEN);
  strncpy(config_info_p->tenant_name, vn_info_p->tenant_name,CRM_MAX_TENANT_NAME_LEN);
  config_info_p->nw_type = vn_info_p->nw_type;
  /* network type */ 
  if(vn_info_p->nw_type ==  VLAN_TYPE)
  {
    config_info_p->nw_params.vlan_nw.vlan_id = vn_info_p->nw_params.vlan_nw.vlan_id;
  }
  else if(vn_info_p->nw_type ==  VXLAN_TYPE)
  {
    config_info_p->nw_params.vxlan_nw.nid          = vn_info_p->nw_params.vxlan_nw.nid;
    config_info_p->nw_params.vxlan_nw.service_port = vn_info_p->nw_params.vxlan_nw.service_port;
  }
  strncpy(config_info_p->nw_desc, vn_info_p->nw_desc, CRM_MAX_VN_DESC_LEN);
  strncpy(config_info_p->provider_physical_network, vn_info_p->provider_physical_network,
                                                    CRM_MAX_VN_PROVIDER_PHY_NW);
  config_info_p->vn_admin_state_up_b      = vn_info_p->vn_admin_state_up_b;
  config_info_p->vn_operational_status_b  = vn_info_p->vn_operational_status_b;
  config_info_p->router_external_b        = vn_info_p->router_external_b;
 
  CNTLR_RCU_READ_LOCK_RELEASE();
  OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"VN found for the given view");
  return CRM_SUCCESS;
} 

