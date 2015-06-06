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
 * File name: crmapi.c
 * Author: Varun Kumar Reddy <B36461@freescale.com>
 * Date:   11/10/2013
 * Description: 
*/


/** Cloud Resource Manager Module */

/** Header file includes */
#include "crm_system_dependencies.h"
#include "crm_vn_api.h"
#include "crm_port_api.h"
#include "crm_subnet_api.h"
#include "crm_vm_api.h"
#include "crm_tenant_api.h"

#define RCUNODE_OFFSET_IN_MCHASH_TBLLNK offsetof(struct mchash_table_link, rcu_head)



/** Data structures */

/** Macros */

/** Declarations */

/** Virtual Machine Hash table */
void*    crm_vm_mempool_g = NULL;
uint32_t crm_no_of_vm_buckets_g;
struct   mchash_table* crm_vm_table_g = NULL;

/** crm ports hash table **/
void*    crm_port_mempool_g = NULL;
uint32_t crm_no_of_port_buckets_g;
struct   mchash_table* crm_port_table_g = NULL;

/** Virtual Network Hash table */
void*	 crm_vn_mempool_g = NULL;
uint32_t crm_no_of_vn_buckets_g;
struct   mchash_table* crm_vn_table_g = NULL;

/** Subnets Hash Table **/
void*	 crm_subnet_mempool_g = NULL;
uint32_t crm_no_of_subnet_buckets_g;
struct   mchash_table* crm_subnet_table_g = NULL;

/** Tenant Hash table */
void*	 crm_tenant_mempool_g = NULL;
uint32_t crm_no_of_tenant_buckets_g;
struct   mchash_table* crm_tenant_table_g = NULL;


/* mempool for attributes*/
void*	crm_attributes_mempool_g;

void *crm_view_node_mempool_g = NULL;

/** Notifications */
void*   crm_notifications_mempool_g;

void*   crm_vm_handle_mempool_g        = NULL;
void*	crm_tenant_vnnames_mempool_g   = NULL;
void*   crm_vn_compute_nodes_mempool_g = NULL;
void*	crm_port_ref_mempool_g         = NULL;
void*	crm_subnet_handle_mempool_g    = NULL;
void*	crm_vm_port_mempool_g          = NULL;

of_list_t crm_vn_notifications[CRM_VN_LAST_NOTIFICATION_TYPE + 1];
of_list_t crm_port_notifications[CRM_PORT_LAST_NOTIFICATION_TYPE + 1];
of_list_t crm_vm_notifications[CRM_VM_LAST_NOTIFICATION_TYPE + 1];
of_list_t crm_tenant_notifications[CRM_TENANT_LAST_NOTIFICATION_TYPE + 1];
of_list_t crm_subnet_notifications[CRM_SUBNET_LAST_NOTIFICATION_TYPE + 1];


/** Definitions*/
/*****************************************************************************************************/
void crm_get_vn_mempoolentries(uint32_t* vn_entries_max,  
                               uint32_t* vn_static_entries)
{
  *vn_entries_max    = CRM_MAX_VN_ENTRIES;
  *vn_static_entries = CRM_MAX_VN_STATIC_ENTRIES;
}
/*****************************************************************************************************/
void crm_get_subnet_mempoolentries(uint32_t* subnet_entries_max,  
                                   uint32_t* subnet_static_entries)
{
  *subnet_entries_max    = CRM_MAX_SUBNET_ENTRIES;
  *subnet_static_entries = CRM_MAX_SUBNET_STATIC_ENTRIES;
}
/*****************************************************************************************************/
void crm_get_vm_mempoolentries(uint32_t* vm_entries_max,
                               uint32_t* vm_static_entries)
{
  *vm_entries_max    = CRM_MAX_VM_ENTRIES;
  *vm_static_entries = CRM_MAX_VM_STATIC_ENTRIES;
}
/*****************************************************************************************************/
void crm_get_tenant_mempoolentries(uint32_t* tenant_entries_max, 
                                   uint32_t* tenant_static_entries)
{
  *tenant_entries_max    = CRM_MAX_TENANT_ENTRIES;
  *tenant_static_entries = CRM_MAX_TENANT_STATIC_ENTRIES;
}
/*****************************************************************************************************/
void crm_get_notification_mempoolentries(uint32_t* notification_entries_max,
                                         uint32_t* notification_static_entries)
{
  *notification_entries_max    = CRM_MAX_NOTIFICATION_ENTRIES;
  *notification_static_entries = CRM_MAX_NOTIFICATION_STATIC_ENTRIES;
}
/*****************************************************************************************************/
void crm_get_vn_compute_node_mempoolentries(uint32_t *vn_compute_nodes_max_entries,
                                            uint32_t *vn_compute_nodes_static_entries)
{
  *vn_compute_nodes_max_entries = CRM_MAX_COMPUTE_NODES_ENTRIES;
  *vn_compute_nodes_static_entries = CRM_MAX_COMPUTE_NODES_STATIC_ENTRIES;
}
/***************************************************************************************************/
void crm_get_vn_port_mempoolentries(uint32_t *vn_port_max_entries,
				    uint32_t *vn_port_static_entries)
{
  *vn_port_max_entries = CRM_MAX_VN_PORT_ENTRIES;
  *vn_port_static_entries = CRM_MAX_VN_PORT_STATIC_ENTRIES;
}
/***************************************************************************************************/
void crm_get_vm_port_mempoolentries(uint32_t *vm_port_max_entries,
		         	    uint32_t *vm_port_static_entries)
{
  *vm_port_max_entries = CRM_MAX_VM_PORT_ENTRIES;
  *vm_port_static_entries = CRM_MAX_VM_STATIC_ENTRIES;
}
/*****************************************************************************************************/
void crm_get_port_mempoolentries(uint32_t *crm_port_max_entries, 
				 uint32_t *crm_port_static_entries)
{
  *crm_port_max_entries = CRM_MAX_PORT_NODES_ENTRIES;
  *crm_port_static_entries = CRM_MAX_PORT_NODES_STATIC_ENTRIES;
}
/*****************************************************************************************************/
void crm_get_attribute_mempoolentries(uint32_t *attribute_entries_max,
                                      uint32_t *attribute_static_entries)
{
  *attribute_entries_max    = CRM_MAX_ATTRIBUTE_ENTRIES;
  *attribute_static_entries = CRM_MAX_ATTRIBUTE_STATIC_ENTRIES;
}
/*****************************************************************************************************/
void crm_get_view_node_mempoolentries(uint32_t* view_node_entries_max,uint32_t* view_node_static_entries)
{
  *view_node_entries_max    = CRM_MAX_VIEW_NODE_ENTRIES;
  *view_node_static_entries = CRM_MAX_VIEW_STATIC_NODE_ENTRIES;
}
/***************************************************************************************************/

void crm_attributes_free_attribute_entry_rcu(struct rcu_head *attribute_entry_p)
{
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Entered");
  struct crm_resource_attribute_entry *attribute_info_p;
  int32_t ret_val;
  if(attribute_entry_p)
  {
    attribute_info_p = (struct crm_resource_attribute_entry *)(((char *)attribute_entry_p) - sizeof(of_list_node_t));
    ret_val = mempool_release_mem_block(crm_attributes_mempool_g,(uchar8_t*)attribute_info_p,attribute_info_p->heap_b);
    if(ret_val != MEMPOOL_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Failed to release attr memory block.");
    }
    else
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Released attr memory block from the memory pool.");
    } 
  }
  else
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "passed for deletion.");
}
/***************************************************************************************************/
void crm_free_vnentry_rcu(struct rcu_head *vn_entry_p)
{
  struct crm_virtual_network *vn_info_p=NULL;
  uint32_t offset;
  int32_t ret_val;

  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Entered");
  if(vn_entry_p)
  {
    offset = VNNODE_VNTBL_OFFSET;
    vn_info_p = (struct crm_virtual_network *)(((char *)vn_entry_p - RCUNODE_OFFSET_IN_MCHASH_TBLLNK) - offset);
    ret_val = mempool_release_mem_block(crm_vn_mempool_g,(uchar8_t*)vn_info_p,vn_info_p->heap_b);

    if(ret_val != MEMPOOL_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Failed to release virtual network memory block.");
    }
    else
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Released virtual network memory block from the memory pool.");
    }
  }  
  else
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Null entry to release",__FUNCTION__,__LINE__);
  }
}
/***************************************************************************************************/
void crm_free_port_entry_rcu(struct rcu_head *crm_port_entry_p)
{
  struct crm_port *port_info_p=NULL;
  uint32_t offset;
  int32_t ret_value;
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Entered");

  if(crm_port_entry_p)
  {
    offset = CRM_PORT_NODE_PORT_TBL_OFFSET;
    port_info_p = (struct crm_port *)(((char *)crm_port_entry_p - RCUNODE_OFFSET_IN_MCHASH_TBLLNK) - offset);
    ret_value = mempool_release_mem_block(crm_port_mempool_g,(uchar8_t*)port_info_p,port_info_p->heap_b);

    if(ret_value != MEMPOOL_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Failed to release crm port memory block.");
    }
    else
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Released crm port memory block from the memory pool.");
    }
  }  
  else
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Null entry to release");
  }
}
/*****************************************************************************************************/
void crm_free_subnetentry_rcu(struct rcu_head *subnet_entry_p)
{
  struct crm_subnet *subnet_info_p=NULL;
  uint32_t  offset;
  int32_t ret_value;
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Entered");

  if(subnet_entry_p)
  {
    offset = SUBNETNODE_SUBNETTBL_OFFSET;
    subnet_info_p = (struct crm_subnet *)(((char *)subnet_entry_p - RCUNODE_OFFSET_IN_MCHASH_TBLLNK) - offset);
    ret_value = mempool_release_mem_block(crm_subnet_mempool_g,(uchar8_t*)subnet_info_p,subnet_info_p->heap_b);

    if(ret_value != MEMPOOL_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Failed to release subnet memory block.");
    }
    else
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Released subnet memory block from the memory pool.");
    }
  }  
  else
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Null entry to release");
  }
}
/*****************************************************************************************************/
void crm_free_vmentry_rcu(struct rcu_head *vm_entry_p)
{
  struct crm_virtual_machine *vm_info_p;
  uint32_t offset;
  int32_t ret_value;
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Entered");
  if(vm_entry_p)
  {
    offset = VMNODE_VMTBL_OFFSET;
    vm_info_p = (struct crm_virtual_machine*)(((char *)vm_entry_p - RCUNODE_OFFSET_IN_MCHASH_TBLLNK) - offset);
    ret_value = mempool_release_mem_block(crm_vm_mempool_g,(uchar8_t*)vm_info_p,vm_info_p->heap_b);

    if(ret_value != MEMPOOL_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Failed to release virtual machine memory block.");
    }
    else
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Released virtual machine memory block from the memory pool.");
    }
  }
  else
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "NULL passed for deletion.");
  }
}
/*****************************************************************************************************/
void crm_free_tenantentry_rcu(struct rcu_head *tenant_entry_p)
{
  struct crm_tenant *tenant_info_p;
  uchar8_t offset=TENANT_NODE_TENANT_TBL_OFFSET;
  int32_t ret_value;
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Entered");

  if(tenant_entry_p)
  {
    tenant_info_p = (struct crm_tenant*) (((char *)tenant_entry_p - RCUNODE_OFFSET_IN_MCHASH_TBLLNK) - offset);
    ret_value = mempool_release_mem_block(crm_tenant_mempool_g,(uchar8_t*)tenant_info_p, tenant_info_p->heap_b);
    if(ret_value != MEMPOOL_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Failed to release tenant memory block.");
    }
    else
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Released tenant memory block from the memory pool.");
    }
  }  
  else
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "NULL passed for deletion.");
  }
}
/*****************************************************************************************************/
void crm_notifications_free_app_entry_rcu(struct rcu_head *app_entry_p)
{
  struct crm_notification_app_hook_info *app_info_p;
  int32_t ret_value;
  if(app_entry_p)  
  {
    app_info_p = (struct crm_notification_app_hook_info *)((char *)app_entry_p - sizeof(of_list_node_t));
    ret_value = mempool_release_mem_block(crm_notifications_mempool_g, (uchar8_t*)app_info_p, app_info_p->heap_b);
    if(ret_value != MEMPOOL_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Failed to release notification memory block.");
    }
    else
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Released notification memory block from the memory pool.");
    }
  }  
  else
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "NULL passed for deletion.");
  }
} 

void crm_free_view_node_rcu(struct rcu_head *view_node_p)
{
   struct database_view_node *database_view_node_p=(struct database_view_node *)view_node_p;
   if(database_view_node_p)
   {
     mempool_release_mem_block(crm_view_node_mempool_g,(uchar8_t*)database_view_node_p,FALSE);
     if( database_view_node_p->node_name)
     {
       free(database_view_node_p->node_name);
     }
     if( database_view_node_p->view_value)
     {
        free(database_view_node_p->view_value);
     }
   }
   else
   {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," NULL passed for deletion.");
   }
}


/************************************************************************************************/
uint32_t crm_get_hashval_byname(char* name_p,uint32_t no_of_buckets)
{
  uint32_t hashkey,param1,param2,param3,param4;
  char name_hash[16];
  memset(name_hash,0,16);
  strncpy(name_hash,name_p,16);
  param1 = *(uint32_t *)(name_hash +0);
  param2 = *(uint32_t *)(name_hash +4);
  param3 = *(uint32_t *)(name_hash +8);
  param4 = *(uint32_t *)(name_hash +12);
  CRM_COMPUTE_HASH(param1,param2,param3,param4,no_of_buckets,hashkey);
  return hashkey;
}
/*****************************************************************************************************/
uint32_t crm_get_hashval_bynames(char* name1_p,char *name2_p,uint32_t no_of_buckets)
{
  uint32_t hashkey,param1,param2,param3,param4;
  char name1_hash[8];
  char name2_hash[8];
  memset(name1_hash,0,8);
  memset(name2_hash,0,8);
  strncpy(name1_hash,name2_p,8);
  strncpy(name2_hash,name2_p,8);
  param1 = *(uint32_t *)(name1_hash +0);
  param2 = *(uint32_t *)(name1_hash +4);
  param3 = *(uint32_t *)(name2_hash +0);
  param4 = *(uint32_t *)(name2_hash +4);
  CRM_COMPUTE_HASH(param1,param2,param3,param4,no_of_buckets,hashkey);
  return hashkey;
}
/*******************************************************************************************************/
int32_t crm_init()
{
  int32_t  ii, ret_value;
  uint32_t vn_entries_max, vn_static_entries;
  uint32_t vn_compute_nodes_entries_max, vn_compute_nodes_static_entries;
  uint32_t port_entries_max, port_static_entries;
  uint32_t vm_port_entries_max, vm_port_static_entries;
  uint32_t vm_entries_max, vm_static_entries;
  uint32_t subnet_entries_max, subnet_static_entries;
  uint32_t tenant_entries_max, tenant_static_entries;
  uint32_t notification_entries_max, notification_static_entries;
  uint32_t attribute_entries_max, attribute_static_entries;
  uint32_t view_node_entries_max,view_node_static_entries;
  struct mempool_params mempool_info={}; 
 
  /** creating mempool and hash table for Virtual Networks Database **/
  crm_get_vn_mempoolentries(&vn_entries_max,&vn_static_entries);
  
  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;
  mempool_info.block_size = sizeof(struct crm_virtual_network) + (NO_OF_CRM_VN_APPLICATIONS * sizeof(void *));
  mempool_info.max_blocks = vn_entries_max;
  mempool_info.static_blocks = vn_static_entries;
  mempool_info.threshold_min = vn_static_entries/10;
  mempool_info.threshold_max = vn_static_entries/3;
  mempool_info.replenish_count = vn_static_entries/10;
  mempool_info.b_memset_not_required = FALSE;
  mempool_info.b_list_per_thread_required = FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("crm_virtual_network_pool", &mempool_info,
                                  &crm_vn_mempool_g    
                                 );
  if(ret_value != MEMPOOL_SUCCESS)
  {  
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Memory Pool Creation failed for virtual networks!.");
    goto CRMINIT_EXIT;
  } 
  ret_value = mchash_table_create(((vn_entries_max / 5)+1),
                                    vn_entries_max,
                                    crm_free_vnentry_rcu,
                                    &crm_vn_table_g);
  if(ret_value != MCHASHTBL_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Hash table Creation failed for virtual networks!.");
    goto CRMINIT_EXIT; 
  }
  crm_no_of_vn_buckets_g = (vn_entries_max / 5) + 1;




  /** creating  memory pool  for crm  virtual network compute nodes **/
  crm_get_vn_compute_node_mempoolentries(&vn_compute_nodes_entries_max, 
					 &vn_compute_nodes_static_entries);

  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;
  mempool_info.block_size = sizeof(struct crm_compute_node);
  mempool_info.max_blocks = vn_compute_nodes_entries_max;
  mempool_info.static_blocks = vn_compute_nodes_static_entries;
  mempool_info.threshold_min = vn_compute_nodes_static_entries/10;
  mempool_info.threshold_max = vn_compute_nodes_static_entries/3;
  mempool_info.replenish_count = vn_compute_nodes_static_entries/10;
  mempool_info.b_memset_not_required = FALSE;
  mempool_info.b_list_per_thread_required = FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("crm_compute_nodes_pool", &mempool_info,
                        &crm_vn_compute_nodes_mempool_g
                        );
  if(ret_value != MEMPOOL_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Memory Pool Creation failed for virtual network compute nodes!.");
    goto CRMINIT_EXIT;
  } 
    
  /** creating memory pool  to maintain   crm_ports handles
      which are attached to virtual network **/
  crm_get_vn_port_mempoolentries(&port_entries_max, &port_static_entries);

  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;
  mempool_info.block_size = sizeof(struct crm_port_ref);
  mempool_info.max_blocks = port_entries_max;
  mempool_info.static_blocks = port_static_entries;
  mempool_info.threshold_min = port_static_entries/10;
  mempool_info.threshold_max = port_static_entries/3;
  mempool_info.replenish_count = port_static_entries/10;
  mempool_info.b_memset_not_required = FALSE;
  mempool_info.b_list_per_thread_required = FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("crm_port_handle_pool", &mempool_info,
		  &crm_port_ref_mempool_g
                        );
  if(ret_value != MEMPOOL_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Memory Pool Creation failed for crm_port handles!.");
    goto CRMINIT_EXIT;
  }   

  /** creating memory pool  to maintain   subnet handles in virtual network node  **/
  crm_get_subnet_mempoolentries(&subnet_entries_max, &subnet_static_entries);
  
  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;
  mempool_info.block_size = sizeof(struct crm_subnet_handle);
  mempool_info.max_blocks = subnet_entries_max;
  mempool_info.static_blocks = subnet_static_entries;
  mempool_info.threshold_min = subnet_static_entries/10;
  mempool_info.threshold_max = subnet_static_entries/3;
  mempool_info.replenish_count = subnet_static_entries/10;
  mempool_info.b_memset_not_required = FALSE;
  mempool_info.b_list_per_thread_required = FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("crm_subnet_handle_pool", &mempool_info,
                        &crm_subnet_handle_mempool_g
                        );
  if(ret_value != MEMPOOL_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Memory Pool Creation failed for crm subnet handles!.");
    goto CRMINIT_EXIT;
  }   

  /** creating memory pool  to maintain   vm handles in virtual network node  **/
  crm_get_vm_mempoolentries(&vm_entries_max, &vm_static_entries);

  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;
  mempool_info.block_size = sizeof(struct crm_virtual_machine_handle);
  mempool_info.max_blocks = vm_entries_max;
  mempool_info.static_blocks = vm_static_entries;
  mempool_info.threshold_min = vm_static_entries/10;
  mempool_info.threshold_max = vm_static_entries/3;
  mempool_info.replenish_count = vm_static_entries/10;
  mempool_info.b_memset_not_required = FALSE;
  mempool_info.b_list_per_thread_required = FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("crm_virtual_machine_handle_pool", &mempool_info,
                        &crm_vm_handle_mempool_g
                        );
  if(ret_value != MEMPOOL_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Memory Pool Creation failed for crm vm handles!.");
    goto CRMINIT_EXIT;
  }   

  /***********************************************************************/

  /** creating mempool and hash table for CRM subnet Database  **/
  crm_get_subnet_mempoolentries(&subnet_entries_max,&subnet_static_entries);

  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;
  mempool_info.block_size = sizeof(struct crm_subnet);
  mempool_info.max_blocks = subnet_entries_max;
  mempool_info.static_blocks = subnet_static_entries;
  mempool_info.threshold_min = subnet_static_entries/10;
  mempool_info.threshold_max = subnet_static_entries/3;
  mempool_info.replenish_count = subnet_static_entries/10;
  mempool_info.b_memset_not_required = FALSE;
  mempool_info.b_list_per_thread_required = FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("crm_subnet_pool", &mempool_info,
                                  &crm_subnet_mempool_g    
                                 );

  if(ret_value != MEMPOOL_SUCCESS)
    goto CRMINIT_EXIT; 
  ret_value = mchash_table_create(((subnet_entries_max / 5)+1),
                                    subnet_entries_max,
                                    crm_free_subnetentry_rcu,
                                    &crm_subnet_table_g);
  if(ret_value != MCHASHTBL_SUCCESS)
    goto CRMINIT_EXIT;
  crm_no_of_subnet_buckets_g = (subnet_entries_max / 5) + 1;


 /** creating mempool and hash table for CRM port Database  **/
  crm_get_port_mempoolentries(&port_entries_max,&port_static_entries);

  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;
  mempool_info.block_size = sizeof(struct crm_port);
  mempool_info.max_blocks = port_entries_max;
  mempool_info.static_blocks = port_static_entries;
  mempool_info.threshold_min = port_static_entries/10;
  mempool_info.threshold_max = port_static_entries/3;
  mempool_info.replenish_count = port_static_entries/10;
  mempool_info.b_memset_not_required = FALSE;
  mempool_info.b_list_per_thread_required = FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("crm_port_pool", &mempool_info,
                                  &crm_port_mempool_g
                                 );
  if(ret_value != MEMPOOL_SUCCESS)
    goto CRMINIT_EXIT; 
  ret_value = mchash_table_create(((port_entries_max / 5)+1),
                                    port_entries_max,
                                    crm_free_port_entry_rcu,
                                    &crm_port_table_g);
  if(ret_value != MCHASHTBL_SUCCESS)
    goto CRMINIT_EXIT;
  crm_no_of_port_buckets_g = (port_entries_max / 5) + 1;


  /** creating mempool and hash table for CRM Virtual Machines Database*/ 
  crm_get_vm_mempoolentries(&vm_entries_max,&vm_static_entries);
  
  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;
  mempool_info.block_size = sizeof(struct crm_virtual_machine);
  mempool_info.max_blocks = vm_entries_max;
  mempool_info.static_blocks = vm_static_entries;
  mempool_info.threshold_min = vm_static_entries/10;
  mempool_info.threshold_max = vm_static_entries/3;
  mempool_info.replenish_count = vm_static_entries/10;
  mempool_info.b_memset_not_required = FALSE;
  mempool_info.b_list_per_thread_required = FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("crm_vm_pool", &mempool_info,
                                  &crm_vm_mempool_g    
                                 );
  if(ret_value != MEMPOOL_SUCCESS)
    goto CRMINIT_EXIT; 
  ret_value = mchash_table_create(((vm_entries_max / 5)+1),
                                    vm_entries_max,
                                    crm_free_vmentry_rcu,
                                    &crm_vm_table_g);
  if(ret_value != MCHASHTBL_SUCCESS)
    goto CRMINIT_EXIT; 
  crm_no_of_vm_buckets_g = (vm_entries_max / 5) + 1;


  /** create memory pool  to maintain   crm_ports 
      which are attached to virtual machine **/
  crm_get_vm_port_mempoolentries(&vm_port_entries_max, &vm_port_static_entries);

  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;
  mempool_info.block_size = sizeof(struct crm_vm_port);
  mempool_info.max_blocks = vm_port_entries_max;
  mempool_info.static_blocks = vm_port_static_entries;
  mempool_info.threshold_min = vm_port_static_entries/10;
  mempool_info.threshold_max = vm_port_static_entries/3;
  mempool_info.replenish_count = vm_port_static_entries/10;
  mempool_info.b_memset_not_required =  FALSE;
  mempool_info.b_list_per_thread_required =  FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("crm_virtual_machine_ports_pool", &mempool_info,
                        &crm_vm_port_mempool_g
                        );
  if(ret_value != MEMPOOL_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "failed to create pool for virtual machin ports");
    goto CRMINIT_EXIT;
  }  

  /** creating mempool and hash table for CRM tenant Database**/
  crm_get_tenant_mempoolentries(&tenant_entries_max,&tenant_static_entries);
   
  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;
  mempool_info.block_size = sizeof(struct crm_tenant);
  mempool_info.max_blocks = tenant_entries_max;
  mempool_info.static_blocks = tenant_static_entries;
  mempool_info.threshold_min = tenant_static_entries/10;
  mempool_info.threshold_max = tenant_static_entries/3;
  mempool_info.replenish_count = tenant_static_entries/10;
  mempool_info.b_memset_not_required =  FALSE;
  mempool_info.b_list_per_thread_required =  FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("crm_tenant_pool", &mempool_info,
                                  &crm_tenant_mempool_g    
                                 );
  if(ret_value != MEMPOOL_SUCCESS)
    goto CRMINIT_EXIT; 
  ret_value = mchash_table_create(((tenant_entries_max / 5)+1),
                                    tenant_entries_max,
                                    crm_free_tenantentry_rcu,
                                    &crm_tenant_table_g);
  if(ret_value != MCHASHTBL_SUCCESS)
    goto CRMINIT_EXIT; 

  crm_no_of_tenant_buckets_g = (tenant_entries_max / 5) + 1;

  crm_get_vn_mempoolentries(&vn_entries_max, &vn_static_entries);
  /** create memory pool for vmname-saferref**/

  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;
  mempool_info.block_size = sizeof(struct crm_tenant_vn_list);
  mempool_info.max_blocks = vn_entries_max;
  mempool_info.static_blocks = vn_static_entries;
  mempool_info.threshold_min = vn_static_entries/10;
  mempool_info.threshold_max = vn_static_entries/3;
  mempool_info.replenish_count = vn_static_entries/10;
  mempool_info.b_memset_not_required =  FALSE;
  mempool_info.b_list_per_thread_required =  FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("crm_tenant_vnnames_pool", &mempool_info,
                        &(crm_tenant_vnnames_mempool_g)
                        );

  if(ret_value != MEMPOOL_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Memory Pool Creation failed for tenant-vnnames list.");
    goto CRMINIT_EXIT; 
  }



  /** attributes memory pool **/
  crm_get_attribute_mempoolentries(&attribute_entries_max,&attribute_static_entries);

  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;
  mempool_info.block_size = sizeof(struct crm_resource_attribute_entry);
  mempool_info.max_blocks = attribute_entries_max;
  mempool_info.static_blocks = attribute_static_entries;
  mempool_info.threshold_min = attribute_static_entries/10;
  mempool_info.threshold_max = attribute_static_entries/3;
  mempool_info.replenish_count = attribute_static_entries/10;
  mempool_info.b_memset_not_required =  FALSE;
  mempool_info.b_list_per_thread_required =  FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("crm_attributes_pool", &mempool_info,
                                   &crm_attributes_mempool_g);

  if(ret_value != MEMPOOL_SUCCESS)
    goto CRMINIT_EXIT;


  /*************************************************************************/
  crm_get_notification_mempoolentries(&notification_entries_max,&notification_static_entries);
  
  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;
  mempool_info.block_size = sizeof(struct crm_notification_app_hook_info);
  mempool_info.max_blocks = notification_entries_max;
  mempool_info.static_blocks = notification_static_entries;
  mempool_info.threshold_min = notification_static_entries/10;
  mempool_info.threshold_max = notification_static_entries/3;
  mempool_info.replenish_count = notification_static_entries/10;
  mempool_info.b_memset_not_required =  FALSE;
  mempool_info.b_list_per_thread_required =  FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("crm_notifications_pool", &mempool_info,
                                   &crm_notifications_mempool_g);

  if(ret_value != MEMPOOL_SUCCESS)
    goto CRMINIT_EXIT;
 
  crm_get_view_node_mempoolentries(&view_node_entries_max,&view_node_static_entries);

    mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;
    mempool_info.block_size = sizeof(struct database_view_node);
    mempool_info.max_blocks = view_node_entries_max;
    mempool_info.static_blocks = view_node_static_entries;
    mempool_info.threshold_min = view_node_static_entries/10;
    mempool_info.threshold_max = view_node_static_entries/3;
    mempool_info.replenish_count = view_node_static_entries/10;
    mempool_info.b_memset_not_required =  FALSE;
    mempool_info.b_list_per_thread_required =  FALSE;
    mempool_info.noof_blocks_to_move_to_thread_list = 0;

    ret_value = mempool_create_pool("crm_view_node_pool", &mempool_info,
                  &crm_view_node_mempool_g
                  );
    if(ret_value != MEMPOOL_SUCCESS)
      goto CRMINIT_EXIT;


 
  for(ii = 0; ii < CRM_VN_LAST_NOTIFICATION_TYPE; ii++)
    OF_LIST_INIT(crm_vn_notifications[ii], crm_notifications_free_app_entry_rcu);

  for(ii = 0; ii < CRM_PORT_LAST_NOTIFICATION_TYPE; ii++)
    OF_LIST_INIT(crm_port_notifications[ii], crm_notifications_free_app_entry_rcu);

  for(ii = 0; ii < CRM_VM_LAST_NOTIFICATION_TYPE; ii++)
    OF_LIST_INIT(crm_vm_notifications[ii], crm_notifications_free_app_entry_rcu);

  for(ii = 0; ii < CRM_TENANT_LAST_NOTIFICATION_TYPE; ii++)
    OF_LIST_INIT(crm_tenant_notifications[ii], crm_notifications_free_app_entry_rcu);

  return CRM_SUCCESS;

CRMINIT_EXIT:
  mempool_delete_pool(crm_vn_mempool_g); 
  mempool_delete_pool(crm_vn_compute_nodes_mempool_g); 
  mempool_delete_pool(crm_tenant_vnnames_mempool_g); 
  mempool_delete_pool(crm_port_ref_mempool_g); 
  mempool_delete_pool(crm_port_mempool_g); 
  mempool_delete_pool(crm_vm_mempool_g); 
  mempool_delete_pool(crm_tenant_mempool_g); 
  mempool_delete_pool(crm_subnet_mempool_g); 
  mempool_delete_pool(crm_notifications_mempool_g);
  mempool_delete_pool(crm_attributes_mempool_g);
  mempool_delete_pool(crm_view_node_mempool_g);
  
  
  mchash_table_delete(crm_vn_table_g);
  mchash_table_delete(crm_port_table_g);
  mchash_table_delete(crm_vm_table_g);
  mchash_table_delete(crm_subnet_table_g);
  mchash_table_delete(crm_tenant_table_g);
  
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "CRM Module Failed to Init!.");
  return CRM_FAILURE;
}
/*****************************************************************************************************/
int32_t crm_uninit()
{
  int32_t ret_value = FALSE;

  ret_value = mchash_table_delete(crm_vn_table_g);
  if(ret_value != MCHASHTBL_SUCCESS)
    return ret_value;

  ret_value = mchash_table_delete(crm_port_table_g);
  if(ret_value != MCHASHTBL_SUCCESS)
    return ret_value;

  ret_value = mchash_table_delete(crm_subnet_table_g);
  if(ret_value != MCHASHTBL_SUCCESS)
    return ret_value;

  ret_value = mchash_table_delete(crm_vm_table_g);
  if(ret_value != MCHASHTBL_SUCCESS)
    return ret_value;

  ret_value = mchash_table_delete(crm_tenant_table_g);
  if(ret_value != MCHASHTBL_SUCCESS)
    return ret_value;

  ret_value = mempool_delete_pool(crm_vn_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    return ret_value;

  ret_value = mempool_delete_pool(crm_vn_compute_nodes_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    return ret_value;

  ret_value = mempool_delete_pool(crm_tenant_vnnames_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    return ret_value;

  ret_value = mempool_delete_pool(crm_port_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    return ret_value;

  ret_value = mempool_delete_pool(crm_port_ref_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    return ret_value;

  ret_value = mempool_delete_pool(crm_vm_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    return ret_value;

  ret_value = mempool_delete_pool(crm_subnet_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    return ret_value;

  ret_value = mempool_delete_pool(crm_tenant_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    return ret_value;

  ret_value = mempool_delete_pool(crm_notifications_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    return ret_value;

 ret_value = mempool_delete_pool(crm_attributes_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    return ret_value;

 return CRM_SUCCESS;
}
/******************************************************************************************************/
