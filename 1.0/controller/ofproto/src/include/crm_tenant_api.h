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
 * File name: crm_tenant_api.h
 * Author: Varun Kumar Reddy <B36461@freescale.com>
 * Date:   11/10/2013
 * Description: 

@Description   This file contains the CRM tenant API & related 
                      macros, data structures
*//***************************************************************************/

#ifndef __CRM_TENANT_API_H
#define __CRM_TENANT_API_H

/* Macros for Cloud Resource Manager Tenant */
#define CRM_TENANT_VNS_LIST_NODE_OFFSET offsetof(struct crm_tenant, list_of_virtual_networks)
#define CRM_TENANT_NS_CHAIN_LIST_NODE_OFFSET  offset(struct crm_tenant, list_of_ns_chains)

/* enums */


struct crm_tenant 
{
 char      tenant_name[CRM_MAX_TENANT_NAME_LEN];
 uint32_t  number_of_virtual_networks;
 of_list_t list_of_virtual_networks;   /* Only VN names are sufficient */
 /* of list holding attributes added for this tenant Node */
 uint32_t  number_of_attributes;
 of_list_t attributes;  /* Each attribute may contain multiple attribute values */
 #ifdef NSRM_SUPPORT
 uint32_t number_of_ns_chains;
 of_list_t list_of_ns_chains;
 #endif

 /** Boolean flag indicate whether this memory is allocated from heap or not*/
 uint8_t   heap_b;
 struct mchash_table_link tenant_tbl_link;
 uint32_t  magic;
 uint32_t  index;
 uint64_t  tenant_handle;
};

struct crm_tenant_vn_list
{
  of_list_node_t next_node;
  struct   rcu_head rcu_head;
  uint8_t  heap_b;
  char  nw_name [CRM_MAX_VN_NAME_LEN];
  struct crm_tenant *tenant_entry_node_p;
};
#ifdef NSRM_SUPPORT
struct crm_tenant_ns_chain_list
{
  of_list_node_t next_node;
  struct   rcu_head rcu_head;
  uint8_t  heap_b;
  char  ns_chain_name[128];
  struct crm_tenant *tenant_ns_chain_entry_p;
};
#endif


#define TENANT_NODE_TENANT_TBL_OFFSET offsetof(struct crm_tenant, tenant_tbl_link)
#define CRM_TENANT_VN_LIST_HANDLE_ENTRY_LISTNODE_OFFSET offsetof(struct crm_tenant_vn_list, next_node)
#ifdef NSRM_SUPPORT
#define CRM_TENANT_NS_CHAIN_LIST_HANDLE_ENTRY_LISTNODE_OFFSET offsetof(struct crm_tenant_ns_chain_list, next_node)
#endif

int32_t crm_get_tenant_by_name(char* tenant_name,  
        struct  crm_tenant_config_info*  config_info_p,
        struct  tenant_runtime_info  *runtime_info);

int32_t crm_get_tenant_handle(char *tenant_name, uint64_t *tenant_handle);

void crm_tenant_vnnames_handle_free_entry_rcu(struct rcu_head *vn_names_entry_p);


#define CRM_TENANT_FIRST_NOTIFICATION_TYPE CRM_TENANT_ALL_NOTIFICATIONS
#define CRM_TENANT_LAST_NOTIFICATION_TYPE  CRM_TENANT_DETACH
#endif
