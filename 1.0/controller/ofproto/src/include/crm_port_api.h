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
 * File name: crm_port_api.h
 * Author: Varun Kumar Reddy <B36461@freescale.com>
 * Date:   11/10/2013
 * Description: 
@File          crm_port_api.h

@Description   This file contains the CRM Virtual Network API & related 
                      macros, data structures
*//***************************************************************************/

#ifndef __CRM_PORT_API_H
#define __CRM_PORT_API_H

#define CRM_ERROR_NWPORT_NAME_NULL 		     -2
#define CRM_ERROR_INCOMPLETE_RESOURCE_RELATIONSHIPS  -3

struct vm_port_info
{
  char      vm_name[CRM_MAX_VM_NAME_LEN];
  uint64_t  saferef_vm;
  uint8_t   vm_port_mac[OFP_ETH_ALEN];
};

/* crm_port is a phrase coined by this module.
 *
 * A global hash table is used to store all the crm_ports attached to all the Virtual 
 * Networks supported by the CRM module. The safe reference to each crm_port is added 
 * as an attribute to the corresponding port in logical switch maintained by DPRM database
 * for faster references in packet processing path. It is better if the safe reference to 
 * crm_port is added as a member of port structure of DPRM. */

struct crm_port
{
  struct    mchash_table_link  crm_port_tbl_link;	
  char      port_name[CRM_MAX_PORT_NAME_LEN];
  uint8_t   crm_port_type;   /* VMSIDE_PORT,VMNS_PORT,NWSIDE_UNICAST_PORT,NWSIDE_BROADCAST_PORT */
  uint32_t  port_id; 	
  uint8_t   port_status;  /** DPRM_PORT_READY, DPRM_PORT_NOT_READY */
  char      switch_name[DPRM_MAX_SWITCH_NAME_LEN+1];
  uint32_t  switch_ip;    /* remote ip has value when port type nwside broadcast */   
  uint64_t  switch_handle; 
  char      br1_name[CRM_LOGICAL_SWITCH_NAME_LEN+1];
  uint64_t  system_br_saferef; 
  char      vn_name[CRM_MAX_VN_NAME_LEN];  
  uint64_t  saferef_vn;  

  /** following field valid for VM PORTS*/
  struct    vm_port_info vmInfo;

  uint16_t  vlan_id_in;
  uint16_t  vlan_id_out;

  uint8_t   nw_type;  /* Valid for all paorts */
 
  union
  {
    struct crm_vxlan_nw vxlan_nw;
    struct crm_nvgre_nw nvgre_nw;
    struct crm_vlan_nw  vlan_nw;
  }nw_params;

  uint32_t  magic;
  uint32_t  index;
  uint8_t   heap_b;
  uint64_t  crm_port_handle;
  uint64_t  system_port_handle; //
  of_list_t attributes;
  uint32_t  number_of_attributes;
};

#define CRM_PORT_NODE_PORT_TBL_OFFSET offsetof(struct crm_port, crm_port_tbl_link)
int32_t crm_register_port_notifications (uint32_t notification_type,
                                         crm_port_notifier_fn port_notifier_fn,
                                         void* callback_arg1,
                                         void* callback_arg2);
int32_t crm_deregister_port_notifications(uint32_t notification_type,
                                          crm_port_notifier_fn port_notifier_fn);
int32_t crm_get_port_byname(char* port_name_p, struct crm_port ** port_info_p_p);
int32_t crm_get_port_handle(char *port_name_p, uint64_t *port_handle_p);
int32_t crm_get_port_byhandle(uint64_t port_handle,struct crm_port **crm_port_info_p);
void    crm_set_port_status(char* crm_port_name, uint64_t crm_port_handle,
                            uint64_t system_port_handle,
                            uint8_t  port_status,
                            uint32_t system_port_id);

int32_t crm_create_port_view_entry(uint64_t vm_handle , char* view_name ,char* view_value);
int32_t crm_remove_port_view_entry(char* view_name,char* view_value);
int32_t crm_add_guest_crm_vmport(uint8_t*  mac_p,uint32_t ipaddr,uint64_t*  output_crm_port_handle_p);

#define CRM_PORT_FIRST_NOTIFICATION_TYPE CRM_PORT_ALL_NOTIFICATIONS
#define CRM_PORT_LAST_NOTIFICATION_TYPE  CRM_PORT_STATUS

#endif
