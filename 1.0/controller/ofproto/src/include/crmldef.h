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
 * File name: crmldef.h
 * Author: Varun Kumar Reddy <B36461@freescale.com>
 * Date:   11/10/2013
 * Description: 

@Description   This file contains the CRM API & related 
                      macros, data structures
*//***************************************************************************/


#ifndef __CRMLDEF_API_H
#define __CRMLDEF_API_H

/* Cloud resource manader local definitions headr file */
/** Macros */
#define crm_debug printf

typedef pthread_mutex_t cntlr_lock_t;
typedef unsigned char uchar8_t;

#define DPRM_MAX_PORT_NAME_LEN 16
#define DPRM_MAX_ATTRIBUTE_NAME_LEN 16

#if 0
/*This structure is re-defined here only to remove warnings while compiling crm. It is actually defined in dprm.h .Since we dont include dprm files in crm ,it is necessary to define this structure again here.*/
struct dprm_datapath_notification_data{
   /** ID of the datapath - valid for all the notification types */
   uint64_t dpid;
   /** Name of the port - valid for port add/delete and port attribute add/delete notification types */
   char port_name[DPRM_MAX_PORT_NAME_LEN + 1];
   /** ID of the por  t - valid for port add/delete and port attribute add/delete notification types */
   uint32_t port_id;
   /** port handle - valid for port add/delete and port attribute add/delete notification types */
   uint64_t port_handle;
   /** attribute name - valid for the attribute add delete notification types */
   char  attribute_name[DPRM_MAX_ATTRIBUTE_NAME_LEN + 1];
   /** attribute value - valid for the attribute add delete notification types */
   char  attribute_value[DPRM_MAX_ATTRIBUTE_NAME_LEN + 1];
};
#endif


#define CRM_DOBBS_MIX(a,b,c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}
#define CRM_DOBBS_WORDS4(word_a, word_b, word_c, word_d, initval, \
    hashmask, hash) \
{ \
  register unsigned long temp_a, temp_b, temp_c; \
  temp_a = temp_b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */\
  temp_c = initval;         /* random value*/ \
  \
  temp_a += word_a; \
  temp_b += word_b; \
  temp_c += word_c; \
  \
  CRM_DOBBS_MIX(temp_a, temp_b, temp_c); \
  \
  temp_a += word_d; \
  CRM_DOBBS_MIX(temp_a, temp_b, temp_c); \
  \
  hash = temp_c & (hashmask-1); \
}

#define CRM_COMPUTE_HASH(param1,param2,parm3,parm4,param5,hashkey) \
  CRM_DOBBS_WORDS4(param1,param2,param3,param4,0xa2956174,param5,hashkey) 




/** Attributes structures **/

#define CRM_MAX_ATTRIBUTE_NAME_LEN     128 
#define CRM_MAX_ATTRIBUTE_VALUE_LEN    128 

#define CRM_MAX_VIEW_DATABASE 10

struct crm_resource_attribute_entry
{ 
  of_list_node_t next_attribute;
  struct   rcu_head rcu_head;
  uint8_t  heap_b;
  char     name[CRM_MAX_ATTRIBUTE_NAME_LEN];
  char     value[CRM_MAX_ATTRIBUTE_VALUE_LEN];
};
#define CRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET offsetof(struct crm_resource_attribute_entry, next_attribute)




struct crm_notification_app_hook_info
{  
  /** to traverse next application registered for a CRM resource notifications */
  of_list_node_t  next_app;
  /** for RCU operation*/
  struct rcu_head  rcu_head;
  uint8_t             priority; /** Priority of the application to invoke, higher prioirty value
                                  application will be called first.*/

  /** Application callback function to call*/
  void  *call_back;
  /** Application specific private info*/
  void  *cbk_arg1;
  /** Application specific private info*/
  void  *cbk_arg2;
  /** Boolean flag indicate whether this memory is allocated from heap or not*/
  uint8_t heap_b;
};

#define CRM_NOTIFICATION_APP_HOOK_OFFSET   offsetof(struct crm_notification_app_hook_info, next_app)

int32_t crm_init(void);
int32_t crm_uninit();
void crm_get_vn_mempoolentries(unsigned int* vn_entries_max,unsigned int* vn_static_entries);
void crm_get_subnet_mempoolentries(uint32_t* subnet_entries_max, uint32_t* subnet_static_entries);
void crm_get_vm_mempoolentries(unsigned int* vm_entries_max,unsigned int* vm_static_entries);
void crm_get_tenant_mempoolentries(unsigned int* tenant_entries_max,unsigned int* tenant_static_entries);
void crm_get_notification_mempoolentries(uint32_t* notification_entries_max, uint32_t* notification_static_entries);
void crm_get_vn_compute_node_mempoolentries(uint32_t *vn_compute_nodes_max_entries,
uint32_t *vn_compute_nodes_static_entries);
void crm_get_vn_port_mempoolentries(uint32_t *vn_port_max_entries,
uint32_t *vn_port_static_entries);
void crm_get_vm_port_mempoolentries(uint32_t *vm_port_max_entries,
uint32_t *vm_port_static_entries);
void crm_get_port_mempoolentries(uint32_t *crm_port_max_entries,
uint32_t *crm_port_static_entries);
void crm_get_attribute_mempoolentries(uint32_t *attribute_entries_max, uint32_t *attribute_static_entries);
void crm_attributes_free_attribute_entry_rcu(struct rcu_head *attribute_entry_p);
void crm_free_vnentry_rcu(struct rcu_head *vn_entry_p);
void crm_free_port_entry_rcu(struct rcu_head *crm_port_entry_p);
void crm_free_subnetentry_rcu(struct rcu_head *subnet_entry_p);
void crm_free_vmentry_rcu(struct rcu_head *vm_entry_p);
void crm_free_tenantentry_rcu(struct rcu_head *tenant_entry_p);
void crm_free_tenantentry_rcu(struct rcu_head *tenant_entry_p);

uint32_t crm_get_hashval_byname(char* name_p,uint32_t no_of_buckets);
//int32_t crm_reg_dprm_port_events();

#if 0
void dp_port_notifications_cbk(uint32_t dprm_notification_type,
                               uint64_t datapath_handle,
                               struct   dprm_datapath_notification_data ,
                               void     *callback_arg1,
                               void     *callback_arg2);
#endif
int32_t crm_add_vn_to_tenant(uint64_t tenant_handle, char *vn_name_p);
int32_t crm_get_vm_handle(char *vm_name_p, uint64_t *vm_handle_p);
int32_t crm_get_vn_handle(char *vn_name_p, uint64_t *vn_handle_p);
int32_t crm_get_tenant_handle(char *tenant_name_p, uint64_t *tenant_handle_p);
int32_t crm_attach_subnet_handle_to_vn(uint64_t vn_handle,  char *subnet_name_p,uint64_t subnet_handle);
int32_t crm_detach_subnet_handle_from_vn(char *subnet_name_p, char *vn_name_p);
int32_t crm_dettach_vm_handle_from_vn(char *vm_name_p, char *vn_name_p);
int32_t crm_attach_vm_handle_to_vn(char *vn_name_p, char *vm_name_p,  uint64_t vm_handle );
int32_t crm_add_attribute(of_list_t *attributes_p, struct crm_attribute_name_value_pair* attribute_info_p);
int32_t crm_get_exact_attribute(of_list_t *attributes_p,
    char  *attribute_name_string, struct   crm_attribute_name_value_output_info *attribute_output_p);
int32_t crm_get_first_attribute( of_list_t *attributes_p,
    struct crm_attribute_name_value_output_info  *attribute_output_p);
int32_t crm_get_next_attribute(of_list_t *attributes,char *current_attribute_name,
    struct crm_attribute_name_value_output_info *attribute_output);

int32_t crm_view_hash_table_init(struct mchash_table **view_node_table);
struct database_view_node *crm_alloc_view_entry_and_set_values(uint64_t saferef, char *node_name, char *view_value);
int32_t crm_view_hash_table_deinit(struct mchash_table* view_node_table);
extern int32_t dprm_get_logical_switch_handle(uint64_t switch_handle,
                                       char *datapath_name,
                                       uint64_t *datapath_handle_p);

int32_t  of_create_crm_view_entry(uint64_t crm_handle, char *view_name_p, char *view_value_p);
int32_t  of_remove_crm_view_entry(char *view_name_p, char *view_value_p );
void crm_get_view_node_mempoolentries(uint32_t* view_node_entries_max,uint32_t*
                      view_node_static_entries);
extern int32_t dprm_get_switch_handle(char* name_p, uint64_t* switch_handle_p);

#endif
