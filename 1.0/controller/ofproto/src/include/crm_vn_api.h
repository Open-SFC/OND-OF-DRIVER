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
 * File name: crm_vn_api.h
 * Author: Varun Kumar Reddy <B36461@freescale.com>
 * Date:   11/10/2013
 * Description: 
@File          crm_vn_api.h

@Description   This file contains the CRM Virtual Network API & related 
                      macros, data structures
*//***************************************************************************/

#ifndef __CRM_VN_API_H
#define __CRM_VN_API_H

/* Macros for CRM Virtual Network */

#define  NO_OF_CRM_VN_APPLICATIONS 2

/* Virtual Network  structure */
struct crm_virtual_network
{
  char     nw_name [CRM_MAX_VN_NAME_LEN];          /* Virtual Network Name */
  char     tenant_name [CRM_MAX_TENANT_NAME_LEN];  /* Tenant Name */
  uint64_t saferef_tenant;                         /* Tenant Handle*/

  /* virtual network type. VLAN_TYPE or VXLAN_TYPE, OR NVGRE_TYPE */
  uint8_t  nw_type;   

  /* Associated Virtual network parameters */
  union 
  {
    struct crm_vxlan_nw vxlan_nw; 
    struct crm_nvgre_nw nvgre_nw;
    struct crm_vlan_nw  vlan_nw;
  }nw_params;   

  /* Optional parameters */   
  char     nw_desc [CRM_MAX_VN_DESC_LEN];  /* Virtual Network Description */
  uint8_t  vn_operational_status_b;        /* VN_ACTIVE = 1, VN_INACTIVE = 0 */
  char     provider_physical_network[CRM_MAX_VN_PROVIDER_PHY_NW];    /* “physnet1” */
  uint8_t  vn_admin_state_up_b;            /* TRUE , FALSE */
  uint8_t  router_external_b;              /* TRUE , FALSE */

  /* of list holding attributes added for this Virtual Network Node */
  /* Each attribute may contain multiple attribute values */
  uint32_t  number_of_attributes;
  of_list_t attributes;  

  /* List of crm_compute_nodes.Each node is represented by structure crm_compute_node */
  /* Each crm_compute_node contains an array crm_logical_switch_nodes.*/
  uint32_t   no_of_compute_nodes;
  of_list_t  crm_compute_nodes;

  /* List of references to OF VMs - UUIDs of VMs are stored as references */
  /* Reference to each VM is stored using the structure crm_virtual_machine_handle(vmname_saferef); */
  uint32_t   no_of_vms;
  of_list_t  crm_vm_handles; 

  /* Range of IP Addresses. One Address from the range is assigned to each VM on this VN. */
  /* Ensure that the same subnet is not assigned to multiple VN nodes */
  /* subnet list. each node contains subnet name and handle.Each node is represented by crm_subnet_handle*/
  uint32_t   no_of_subnets;
  of_list_t  crm_subnet_handles;

  uint8_t  heap_b;
  struct   mchash_table_link vn_tbl_link;
  uint32_t magic;
  uint32_t index;
  uint64_t vn_handle;
};

/*  vm handles list in vn node*/
struct crm_virtual_machine_handle
{
  of_list_node_t  next_node;
  struct rcu_head rcu_head;                           
  uint8_t         heap_b;
  char            vm_name[CRM_MAX_VM_NAME_LEN];
  uint64_t        vm_handle;
};

/* Subnet handle lists in vn vn node*/
struct crm_subnet_handle
{
  of_list_node_t  next_node;
  struct          rcu_head rcu_head;                           
  uint8_t         heap_b;
  char            subnet_name[CRM_MAX_SUBNET_NAME_LEN];
  uint64_t        subnet_handle;
};

#define VNNODE_VNTBL_OFFSET                  offsetof(struct crm_virtual_network,      vn_tbl_link)
#define CRM_VM_HANDLE_LISTNODE_OFFSET        offsetof(struct crm_virtual_machine_handle, next_node)
#define CRM_SUBNET_HANDLE_LISTNODE_OFFSET    offsetof(struct crm_subnet_handle,          next_node)

/* crm port handles list in logical switch */
struct crm_port_ref
{
  of_list_node_t  next_node;   /* next crm ports node in the list */
  struct          rcu_head rcu_head;
  uint8_t         heap_b;
  char            port_name[CRM_MAX_PORT_NAME_LEN];
  int32_t         port_type;
  uint64_t        port_saferef;
};
#define CRM_LOGICAL_SWITCH_PORT_NODE_OFFSET offsetof(struct crm_port_ref, next_node)

/*Note: CRD module may add UUID as an attribute to the DPRM switch database and a create a UUID view. */

/* crm_logical_switch is a phrase coined by this module. It contains a list of all
 *  the associating ports associated with the logical switch. This structure is a
 *  member of the structure crm_logical_switch.
 *
 *  Each crm_logical_switch contains its name and the safe reference to the logical
 *  switch in DPRM database. It also contains a list of safe references to crm_ports.
*/

struct crm_logical_switch  
{
  char        logical_switch_name[CRM_LOGICAL_SWITCH_NAME_LEN];
  uint64_t    saferef_logical_switch;
  uint32_t    no_of_ports;         /* connected to the logical switch */
  uint32_t    no_of_vmside_ports;  /* connected to the logical switch */
  of_list_t   vmside_ports;        /* Each list node contains safe reference to one crm_port. */
  uint32_t    no_of_nwside_unicast_ports;    /* connected to the logical switch */
  of_list_t   nwside_unicast_ports;          /* Each list node contains safe reference to one crm_port. */
  uint32_t    no_of_nwside_broadcast_ports;  /* connected to the logical switch */
  of_list_t   nwside_broadcast_ports;        /* Each list node contains safe reference to one crm_port. */
  uint32_t    no_of_vmns_ports;  /* connected to the logical switch */
  of_list_t   vmns_ports;        /* Each list node contains safe reference to one crm_port. */  
};

/* crm_compute_node is a phrase coined by this module. It contains all the 
 * logical switches and associating ports in a given Physical/Virtual switch
 *  (Compute node in case of OpenStack terminology) which belong to one Virtual 
 *  Network. All Compute Nodes are assumed to support OF protocol.
 *  Virtual Networks are maintained in this CRM-VN database and switch database 
 *  is maintained by DPRM module. Every Virtual Network node contains a list of crm_compute_nodes.*/

struct crm_compute_node 
{
  of_list_node_t   next_node;   /* next crm compute node in the list */
  struct   rcu_head rcu_head;
  uint8_t  heap_b;
  char     switch_name[DPRM_MAX_SWITCH_NAME_LEN+1]; 
  uint64_t saferef_switch;      /* DPRM API is used to get the safe reference to switch. */
  char     vn_name[CRM_MAX_VN_NAME_LEN];
  uint64_t saferef_vn;

  uint32_t no_of_bridges;
   /* Array of logical switches within a crm_compute_node (Ex:  br-int, br-tun created by OVS ) */
   /* Each crm_logical switch contains a safe reference to the datapath in DPRM database and a list of 
      Safe references to the associated crm_ports */   
  struct crm_logical_switch  dp_references [CRM_MAX_DPS_IN_A_COMPUTE_NODE];
};

#define CRM_VN_COMPUTE_NODE_ENTRY_LISTNODE_OFFSET offsetof(struct crm_compute_node, next_node)

void    crm_vm_handle_free_entry_rcu(struct rcu_head *vm_handle_entry_p);
int32_t crm_add_vms_handle_to_vn(struct crm_virtual_network *crm_vn_node_p);
int32_t crm_get_vn_handle(char *vn_name_p, uint64_t *vn_handle_p);
int32_t crm_get_vn_byhandle(uint64_t vn_handle,struct crm_virtual_network **crm_vn_info_p);

int32_t crm_vn_register_application(char *name_p , uint32_t *offset);
int32_t crm_vn_unregister_application(char *name_p);

int32_t crm_create_vn_view_entry(uint64_t vn_handle , char* view_name ,char* view_value);
int32_t crm_remove_vn_view_entry(char* view_name,char* view_value);
int32_t crm_attach_vm_handle_to_vn(char *vn_name_p,char *vm_name_p,uint64_t vm_handle);
int32_t crm_dettach_vm_handle_from_vn(char *vm_name_p, char *vn_name_p);

int32_t crm_get_exact_crm_compute_node_by_name(char *vn_name_p, char *switch_name_p);

#define CRM_VN_FIRST_NOTIFICATION_TYPE CRM_VN_ALL_NOTIFICATIONS
#define CRM_VN_LAST_NOTIFICATION_TYPE  CRM_VN_DETACH

#endif
