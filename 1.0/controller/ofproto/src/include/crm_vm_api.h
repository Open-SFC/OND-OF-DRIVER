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
 * File name: crm_vm_api.h
 * Author: Varun Kumar Reddy <B36461@freescale.com>
 * Date:   11/10/2013
 * Description: 

@Description   This file contains the CRM Virtual Machine API & related 
                      macros, data structures
*//***************************************************************************/

#ifndef __CRM_VM_API_H
#define __CRM_VM_API_H

struct crm_vm_port
{
  of_list_node_t next_node;
  struct         rcu_head rcu_head;
  uint8_t        heap_b;
  char           port_name[CRM_MAX_PORT_NAME_LEN];
  uint64_t       port_saferef;
  char           vn_name[CRM_MAX_VN_NAME_LEN];
};

/* CRM Virtual machine Struct */

struct crm_virtual_machine   
{
  char     vm_name [CRM_MAX_VM_NAME_LEN];    /* VM Name or vm hostname */
  char     vm_desc [CRM_MAX_VM_DESC_LEN];       /* VM description*/
  int32_t  vm_type; /*  enum crm_virtual_machine_type */
  char     tenant_name[CRM_MAX_TENANT_NAME_LEN];
  char     switch_name_dprm[DPRM_MAX_SWITCH_NAME_LEN]; /* Name of Switch maintained in DPRM database */
  uint64_t switch_handle;

  /* List of references of ports connected towards this VM on logical switch side (br-int)
    MAC address is supposed to be part of the Port repository in DPRM). */

  uint32_t  no_of_ports;
  of_list_t list_of_logical_switch_side_ports_connected;  
  /* Each connected port is represented by structure crm_port. */

  /* of list holding attributes added for this port.*/
  uint32_t  number_of_attributes;
  of_list_t attributes;


  struct mchash_table_link vm_tbl_link;
  uint32_t magic;
  uint32_t index;
  uint64_t vm_handle;


  /* Physical Server (Compute Node) to which this VM belongs.*/
  char* switch_name; 

  /** Boolean flag indicate whether this memory is allocated from heap or not*/
  uint8_t heap_b;
};


#define VMNODE_VMTBL_OFFSET offsetof(struct crm_virtual_machine, vm_tbl_link)
#define CRM_VM_PORT_NODE_ENTRY_LISTNODE_OFFSET offsetof(struct crm_vm_port, next_node)
/* Macros for Cloud Resource Manager Virtual Machine */
#define CRM_VM_LOGICAL_SWPORTS_CONNECTED_LIST_NODE_OFFSET offsetof(struct crm_virtual_machine, list_of_logical_switch_side_ports_connected)


#define CRM_VM_FIRST_NOTIFICATION_TYPE CRM_VM_ALL_NOTIFICATIONS
#define CRM_VM_LAST_NOTIFICATION_TYPE  CRM_VM_DETACH

int32_t crm_get_vm_handle(char *vm_name_p, uint64_t *vm_handle_p);
int32_t get_vm_byhandle(uint64_t vm_handle_p , struct crm_virtual_machine**);
int32_t crm_create_vm_view_entry(uint64_t vm_handle , char* view_name ,char* view_value);
int32_t crm_remove_vm_view_entry(char* view_name,char* view_value);
#endif
