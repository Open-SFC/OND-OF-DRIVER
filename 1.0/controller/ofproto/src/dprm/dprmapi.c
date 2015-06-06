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

/* File  :      dprmapi.c
 * Author:      Rajesh Madabushi <rajesh.madabushi@freescale.com>
 * Description: 
 */

#include "controller.h"
#include "dprm.h"
#include "cntlr_epoll.h"
#include "cntrl_queue.h"
#include "of_utilities.h"
#include "of_msgapi.h"
#include "of_multipart.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "cntlr_event.h"
#include "dprmldef.h"
#include "of_view.h"

void     *dprm_switch_mempool_g = NULL;
uint32_t dprm_no_of_switch_buckets_g;
struct   mchash_table* dprm_switch_table_g = NULL;
uint32_t dprm_no_of_swnode_datapath_buckets_g;

void     *dprm_domain_mempool_g = NULL;
uint32_t dprm_no_of_domain_buckets_g;
struct   mchash_table* dprm_domain_table_g = NULL;
uint32_t dprm_no_of_dmnode_oftable_buckets_g; 
uint32_t dprm_no_of_dmnode_datapath_buckets_g;

void     *dprm_datapath_mempool_g = NULL;
uint32_t dprm_no_of_datapath_buckets_g;
struct   mchash_table* dprm_datapath_table_g = NULL;
uint32_t dprm_no_of_dpnode_port_buckets_g;

void     *dprm_attributes_mempool_g;
void     *dprm_attributes_value_mempool_g;
void     *dprm_notifications_mempool_g;

of_list_t switch_notifications[DPRM_SWITCH_LAST_NOTIFICATION_TYPE + 1];
of_list_t domain_notifications[DPRM_DOMAIN_LAST_NOTIFICATION_TYPE + 1];
of_list_t datapath_notifications[DPRM_DATAPATH_LAST_NOTIFICATION_TYPE + 1];
extern of_list_t namespace_notifications[DPRM_NAMESPACE_LAST_NOTIFICATION_TYPE + 1];
extern inline dprm_dp_channel_entry_t* of_get_first_aux_channel(struct dprm_datapath_entry *datapath);
extern inline void 
of_async_msg_table_init(struct dprm_oftable_entry *table);
extern int32_t cntlr_add_transport_endpoint(uint64_t controller_handle,
                                            uint16_t endpoint_port,
                                            uint8_t  endpoint_conn_type);

extern int32_t cntlr_domain_init(struct dprm_domain_entry *p_domain);
extern int32_t cntlr_datapath_init(uint64_t                    datapath_id,
                                   struct dprm_datapath_entry* datapath,
                                   struct dprm_domain_entry*   domain_p);

extern void mchash_insert_node_in_middle(struct mchash_bucket* bucket_p,struct mchash_dll_node *prev_p,
                                  struct mchash_dll_node *node_p, struct mchash_dll_node* next_p);
extern void dprm_get_datapath_mempoolentries(uint32_t* datapath_entries_max,uint32_t* datapath_static_entries);
int32_t get_domain_table_byname(uint64_t domain_handle,char* table_name_p,struct dprm_oftable_entry **table_info_p_p);
int32_t get_datapath_port_byhandle(uint64_t datapath_handle,uint64_t port_handle,struct dprm_port_entry** port_info_p_p);


/********************************************************************************************************/
int32_t dprm_add_attribute(of_list_t *attributes_p,struct dprm_attribute_name_value_pair* attribute_info_p);






int32_t dprm_get_first_attribute_name( of_list_t *attributes_p,
struct dprm_attribute_name_value_output_info  *attribute_output_p);


int32_t dprm_get_attribute_first_value(of_list_t *attributes_p, 
struct dprm_attribute_name_value_output_info *attribute_output_p);



int32_t dprm_get_attribute_next_value(of_list_t *attributes_p,
char *current_attribute_name, char *current_attribute_value, 
struct dprm_attribute_name_value_output_info *attribute_output_p);




int32_t dprm_get_next_attribute_name(of_list_t *attributes,char *current_attribute_name,
                                struct dprm_attribute_name_value_output_info *attribute_output);
void dprm_notifications_free_app_entry_rcu(struct rcu_head *app_entry_p);
void dprm_attributes_free_attribute_value_entry_rcu(struct rcu_head *attribute_value_entry_p);
int32_t del_domain_oftable_from_idbased_list(uint64_t dm_handle,uint8_t table_id);
int32_t get_domain_oftable_from_idbased_list(uint64_t dm_handle,uint8_t table_id, 
                                             struct dprm_oftable_entry  **oftable_p_p);
void dprm_free_domainentry_rcu(struct rcu_head *domain_entry_p);
void dprm_get_domain_mempoolentries(uint32_t* domain_entries_max,uint32_t* domain_static_entries);
void dprm_free_datapathentry_rcu(struct rcu_head *datapath_entry_p);
void  dprm_get_attribute_mempoolentries(uint32_t *attribute_entries_max, uint32_t *attribute_static_entries);
void  dprm_get_attribute_value_mempoolentries(uint32_t *attribute_value_entries_max, uint32_t *attribute_value_static_entries);
void dprm_get_notification_mempoolentries(uint32_t *notification_entries_max,uint32_t *notification_static_entries);
int32_t dprm_uninit();
int32_t dprm_get_attribute_first_value(of_list_t *attributes_p,
      struct dprm_attribute_name_value_output_info *attribute_output_p);
int32_t dprm_get_attribute_next_value(of_list_t *attributes_p,char *current_attribute_name, char *current_attribute_value, struct dprm_attribute_name_value_output_info *attribute_output_p);
int32_t dprm_get_exact_attribute_name(of_list_t *attributes_p,
      char  *attribute_name_string);
int32_t dprm_get_exact_attribute_value(of_list_t *attributes_p, char *attribute_name_string,
      char  *attribute_name_value);

/*************************************switch**********************************************************/
int32_t dprm_register_switch(struct dprm_switch_info* switch_info_p,uint64_t*  output_handle_p)
{
  int32_t  status,ret_value;
  uint32_t hashkey;
  struct   dprm_switch_entry* switch_entry_p      = NULL; 
  struct   dprm_switch_entry* switch_entry_scan_p = NULL;
  char*    fqdn_p = NULL;
  
  uchar8_t  offset,heap_b;
  uchar8_t* hashobj_p = NULL;
  uint32_t  index,magic;
  uint32_t  swnode_dptable_entries_max,swnode_dptable_static_entries;
  
  struct    dprm_switch_notification_data notification_data;
  struct    dprm_notification_app_hook_info *app_entry_p;
  uchar8_t lstoffset;
  lstoffset = DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET;

  ret_value = DPRM_SUCCESS;

  if(dprm_switch_table_g == NULL)
    return DPRM_FAILURE;
 
  if(strlen(switch_info_p->name) < 8)
    return DPRM_ERROR_NAME_LENGTH_NOT_ENOUGH;

  if((switch_info_p->switch_type != VIRTUAL_SWITCH) && (switch_info_p->switch_type != PHYSICAL_SWITCH))
    return DPRM_ERROR_INVALID_SWITCH_TYPE;
  
  if((switch_info_p->switch_fqdn == NULL) && (switch_info_p->ipv4addr.baddr_set == FALSE)
     &&(switch_info_p->ipv6addr.baddr_set == FALSE))
    return DPRM_ERROR_INVALID_IPADDR;

  if(switch_info_p->switch_fqdn != NULL)
  {
    fqdn_p = (char *)malloc(strlen(switch_info_p->switch_fqdn)+1);
    if(fqdn_p == NULL)
      return DPRM_ERROR_NO_MEM;
  }  

  hashkey = dprm_get_hashval_byname(switch_info_p->name,dprm_no_of_switch_buckets_g);  
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, " hashkey = %d",hashkey); 
  
  CNTLR_RCU_READ_LOCK_TAKE();
  offset = SWNODE_SWTBL_OFFSET; 
  MCHASH_BUCKET_SCAN(dprm_switch_table_g,hashkey,switch_entry_scan_p,struct dprm_switch_entry *,offset)
  {
    if(!strcmp(switch_entry_scan_p->name,switch_info_p->name))
    {
      ret_value = DPRM_ERROR_DUPLICATE_RESOURCE;
      goto DPRMEXIT1; 
    }  
    else 
      continue;
  }
  ret_value = mempool_get_mem_block(dprm_switch_mempool_g,(uchar8_t**)&switch_entry_p,&heap_b);
  if(ret_value != MEMPOOL_SUCCESS)
    goto DPRMEXIT1;

  /* copy parameters */

  strcpy(switch_entry_p->name,switch_info_p->name);
  switch_entry_p->switch_fqdn_p = fqdn_p;  
  strcpy(switch_entry_p->switch_fqdn_p,switch_info_p->switch_fqdn); 
  switch_entry_p->ipv4addr.baddr_set  = switch_info_p->ipv4addr.baddr_set;
  switch_entry_p->ipv4addr.addr = switch_info_p->ipv4addr.addr;
  switch_entry_p->switch_type = switch_info_p->switch_type;
  switch_entry_p->heap_b = heap_b;
  /* TBD copy ipv6 address */
  
  dprm_get_swnode_datapath_mempoolentries(&swnode_dptable_entries_max,&swnode_dptable_static_entries);

  ret_value = mchash_table_create(((swnode_dptable_entries_max / 5)+1),
                                    swnode_dptable_entries_max,
                                    dprm_free_swnode_dptbl_entry_rcu,
                                    &switch_entry_p->dprm_swnode_dptable_p);
  if(ret_value != MCHASHTBL_SUCCESS)
    goto DPRMEXIT1;
  
  dprm_no_of_swnode_datapath_buckets_g = (swnode_dptable_entries_max / 5) + 1;
  
  hashobj_p = (uchar8_t *)switch_entry_p + SWNODE_SWTBL_OFFSET;

  MCHASH_APPEND_NODE(dprm_switch_table_g,hashkey,switch_entry_p,index,magic,hashobj_p,status);
  if(FALSE == status)
  {
    ret_value = DPRM_FAILURE;
    goto DPRMEXIT1;
  }

  *output_handle_p = magic; 
  *output_handle_p = ((*output_handle_p <<32) | (index));
  
  switch_entry_p->magic = magic;
  switch_entry_p->index = index; 

  (switch_entry_p->switch_handle) = *output_handle_p;

  OF_LIST_INIT(switch_entry_p->attributes,dprm_attributes_free_attribute_entry_rcu);
  /* Send sw add notification to the registered Applications. */
  strcpy(notification_data.switch_name,switch_info_p->name);
  
  OF_LIST_SCAN(switch_notifications[DPRM_SWITCH_ADDED],app_entry_p,struct dprm_notification_app_hook_info*, lstoffset)
  {
    ((dprm_switch_notifier_fn)(app_entry_p->call_back))(DPRM_SWITCH_ADDED,
                                                        *output_handle_p,
                                                        notification_data,
                                                        app_entry_p->cbk_arg1,
                                                        app_entry_p->cbk_arg2);
  }   
  
  OF_LIST_SCAN(switch_notifications[DPRM_SWITCH_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset)
  {
    ((dprm_switch_notifier_fn)(app_entry_p->call_back))(DPRM_SWITCH_ADDED,
                                                        *output_handle_p,
                                                         notification_data,
                                                         app_entry_p->cbk_arg1,
                                                         app_entry_p->cbk_arg2);
  } 
  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_SUCCESS;
DPRMEXIT1:
  if(fqdn_p)
    free(fqdn_p);

  CNTLR_RCU_READ_LOCK_RELEASE();
  if(switch_entry_p) 
  {
    mchash_table_delete(switch_entry_p->dprm_swnode_dptable_p);
    mempool_release_mem_block(dprm_switch_mempool_g,(uchar8_t *)switch_entry_p,switch_entry_p->heap_b);
  }
  return ret_value;
}
/*******************************************************************************************************/
int32_t dprm_get_switch_handle(char* name_p, uint64_t* switch_handle_p)
{
  uint32_t hashkey;
  uchar8_t offset;
 
  struct dprm_switch_entry* switch_entry_p = NULL;

  if(dprm_switch_table_g == NULL)
    return DPRM_FAILURE; 

  hashkey = dprm_get_hashval_byname(name_p,dprm_no_of_switch_buckets_g);

  CNTLR_RCU_READ_LOCK_TAKE();
  offset = SWNODE_SWTBL_OFFSET;
  MCHASH_BUCKET_SCAN(dprm_switch_table_g,hashkey,switch_entry_p,struct dprm_switch_entry *,offset)
  {
    if(strcmp(switch_entry_p->name,name_p))
      continue;
    CNTLR_RCU_READ_LOCK_RELEASE();

    *switch_handle_p = switch_entry_p->magic;
    *switch_handle_p = ((*switch_handle_p <<32) | (switch_entry_p->index));

    return DPRM_SUCCESS;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_ERROR_INVALID_NAME;
} 
/********************************************************************************************************/
int32_t dprm_get_switch_data_ip(char* name_p, uint32_t* switch_data_ip_p)
{
  uint32_t hashkey;
  uchar8_t offset;

  struct dprm_switch_entry* switch_entry_p = NULL;

  if(dprm_switch_table_g == NULL)
    return DPRM_FAILURE;

  hashkey = dprm_get_hashval_byname(name_p,dprm_no_of_switch_buckets_g);

  CNTLR_RCU_READ_LOCK_TAKE();
  offset = SWNODE_SWTBL_OFFSET;
  MCHASH_BUCKET_SCAN(dprm_switch_table_g,hashkey,switch_entry_p,struct dprm_switch_entry *,offset)
  {
    if(strcmp(switch_entry_p->name,name_p))
      continue;
    CNTLR_RCU_READ_LOCK_RELEASE();

    *switch_data_ip_p = switch_entry_p->ipv4addr.addr;

    return DPRM_SUCCESS;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_ERROR_INVALID_NAME;
}
/********************************************************************************************************/
int32_t dprm_update_switch(uint64_t handle,struct dprm_switch_info* switch_info_p)
{
  uint32_t index,magic;
  struct   dprm_switch_entry* switch_entry_p;
  char     *fqdn_p = NULL;  

  struct   dprm_switch_notification_data notification_data;
  struct   dprm_notification_app_hook_info *app_entry_p;

  uint8_t  status_b;
  int32_t  ret_value;

  ret_value = DPRM_SUCCESS;

  if(dprm_switch_table_g == NULL)
    return DPRM_FAILURE; 

  if(strlen(switch_info_p->name) < 8)
    return DPRM_ERROR_NAME_LENGTH_NOT_ENOUGH;

  if((switch_info_p->switch_type != VIRTUAL_SWITCH) && (switch_info_p->switch_type != PHYSICAL_SWITCH))
    return DPRM_ERROR_INVALID_SWITCH_TYPE;

  if((switch_info_p->switch_fqdn == NULL) && (switch_info_p->ipv4addr.baddr_set = FALSE)
     &&(switch_info_p->ipv6addr.baddr_set))
    return DPRM_ERROR_INVALID_IPADDR;

  if(switch_info_p->switch_fqdn != NULL)
  {
    fqdn_p = ((char *)malloc(strlen(switch_info_p->switch_fqdn)+1));
    if(fqdn_p == NULL )
      return DPRM_ERROR_NO_MEM;
  }

  magic = (uint32_t)(handle >>32);
  index = (uint32_t)handle;

  CNTLR_RCU_READ_LOCK_TAKE();

  MCHASH_ACCESS_NODE(dprm_switch_table_g,index,magic,switch_entry_p,status_b);
  if(CNTLR_LIKELY(status_b))
  {
    switch_entry_p->switch_type = switch_info_p->switch_type;
    switch_entry_p->switch_fqdn_p = fqdn_p;  
    strcpy(switch_entry_p->switch_fqdn_p,switch_info_p->switch_fqdn); 
    switch_entry_p->ipv4addr.baddr_set = switch_info_p->ipv4addr.baddr_set;
    switch_entry_p->ipv4addr.addr = switch_info_p->ipv4addr.addr;
    /* TBD Pass IPv6 information */

    /* Send sw modify notification to the registered Applications. */
    strcpy(notification_data.switch_name,switch_info_p->name);

    OF_LIST_SCAN(switch_notifications[DPRM_SWITCH_MODIFIED],app_entry_p,struct dprm_notification_app_hook_info*,DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET)
    {
      ((dprm_switch_notifier_fn)(app_entry_p->call_back))(DPRM_SWITCH_MODIFIED,
                                                          handle,
                                                          notification_data,
                                                          app_entry_p->cbk_arg1,
                                                          app_entry_p->cbk_arg2);
    }  
      
    OF_LIST_SCAN(switch_notifications[DPRM_SWITCH_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*, DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET )
    {
      ((dprm_switch_notifier_fn)(app_entry_p->call_back))(DPRM_SWITCH_MODIFIED,
                                                          handle,
                                                          notification_data,
                                                          app_entry_p->cbk_arg1,
                                                          app_entry_p->cbk_arg2);
    }
    CNTLR_RCU_READ_LOCK_RELEASE();    
    return DPRM_SUCCESS;
  }
  ret_value = DPRM_ERROR_INVALID_SWITCH_HANDLE;

  if(fqdn_p)
    free(fqdn_p);
  CNTLR_RCU_READ_LOCK_RELEASE();
  return ret_value;
}  
/*******************************************************************************************************/
int32_t get_switch_byhandle(uint64_t switch_handle,struct dprm_switch_entry** switch_info_p_p)
{
  /* Caller shall take RCU locks. */
  uint32_t index,magic;  
  uint8_t status_b;

  magic = (uint32_t)(switch_handle >>32);
  index = (uint32_t)switch_handle;

  MCHASH_ACCESS_NODE(dprm_switch_table_g,index,magic,*switch_info_p_p,status_b);
  if(status_b == TRUE)
    return DPRM_SUCCESS;
  return DPRM_ERROR_INVALID_SWITCH_HANDLE;
}
/******************************************************************************************************/
int32_t  dprm_get_exact_switch(uint64_t handle,
                               struct dprm_switch_info* config_info_p,
                               struct dprm_switch_runtime_info* runtime_info_p)
{
  uint32_t index,magic;
  struct   dprm_switch_entry* switch_info_p;
  uint8_t  status_b;

  magic = (uint32_t)(handle >>32);
  index = (uint32_t)handle;

  CNTLR_RCU_READ_LOCK_TAKE();
  
  MCHASH_ACCESS_NODE(dprm_switch_table_g,index,magic,switch_info_p,status_b);
  if(status_b == TRUE)
  {
    strcpy(config_info_p->name,switch_info_p->name);
    config_info_p->switch_type = switch_info_p->switch_type;
    strcpy(config_info_p->switch_fqdn,switch_info_p->switch_fqdn_p);
    config_info_p->ipv4addr.baddr_set = switch_info_p->ipv4addr.baddr_set;
    config_info_p->ipv4addr.addr = switch_info_p->ipv4addr.addr;
    /* TBD Pass IPv6 information */
    
    runtime_info_p->number_of_datapaths  = switch_info_p->number_of_datapaths;
    runtime_info_p->number_of_attributes = switch_info_p->number_of_attributes;

    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_SUCCESS;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_ERROR_INVALID_SWITCH_HANDLE; 
}  
/******************************************************************************************************/  
int32_t dprm_unregister_switch(uint64_t handle)
{
  uint32_t index,magic;
  struct   dprm_switch_entry* switch_info_p = NULL;
  uint8_t  status_b = FALSE;
  uchar8_t offset;

  struct   dprm_switch_notification_data notification_data;
  struct   dprm_notification_app_hook_info *app_entry_p;
  uchar8_t lstoffset;
  lstoffset = DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET;

  magic = (uint32_t)(handle >>32);
  index = (uint32_t)handle;

  if(dprm_switch_table_g == NULL)
    return DPRM_FAILURE;

  CNTLR_RCU_READ_LOCK_TAKE();

  MCHASH_ACCESS_NODE(dprm_switch_table_g,index,magic,switch_info_p,status_b);
  if(status_b == TRUE)
  {
    if((switch_info_p->number_of_datapaths)|| (switch_info_p->number_of_attributes))
    {
      dprm_debug("%s:%d Delete all the datapaths and attributes associated with the switch before unregistering the switch.\r\n",__FUNCTION__,__LINE__);
      CNTLR_RCU_READ_LOCK_RELEASE();
      return DPRM_ERROR_RESOURCE_NOTEMPTY; 
    }  

    /* Send sw delete notification to the registered Applications. */
    strcpy(notification_data.switch_name,switch_info_p->name);

    OF_LIST_SCAN(switch_notifications[DPRM_SWITCH_DELETE],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
    {
      ((dprm_switch_notifier_fn)(app_entry_p->call_back))(DPRM_SWITCH_DELETE,
                                                          handle,
                                                          notification_data,
                                                          app_entry_p->cbk_arg1,
                                                          app_entry_p->cbk_arg2);
    }

    OF_LIST_SCAN(switch_notifications[DPRM_SWITCH_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
    {
      ((dprm_switch_notifier_fn)(app_entry_p->call_back))(DPRM_SWITCH_DELETE,
                                                          handle,
                                                          notification_data,
                                                          app_entry_p->cbk_arg1,
                                                          app_entry_p->cbk_arg2);
    }
    status_b = FALSE;
    offset = SWNODE_SWTBL_OFFSET;
    MCHASH_DELETE_NODE_BY_REF(dprm_switch_table_g,index,magic,struct dprm_switch_entry *,offset,status_b);
    CNTLR_RCU_READ_LOCK_RELEASE();
    if(status_b == TRUE)
      return DPRM_SUCCESS;
    else
      return DPRM_FAILURE;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_ERROR_INVALID_SWITCH_HANDLE;
}
/********************************************************************************************************/
void dprm_notifications_free_app_entry_rcu(struct rcu_head *app_entry_p)
{
  struct dprm_notification_app_hook_info *app_info_p;
  if(app_entry_p)  
  {
    app_info_p = (struct dprm_notification_app_hook_info *)((char *)app_entry_p - sizeof(of_list_node_t));
    mempool_release_mem_block(dprm_notifications_mempool_g,(uchar8_t*)app_info_p,app_info_p->heap_b);
  }  
  else
    dprm_debug("%s:%d NULL passed for deletion.\r\n",__FUNCTION__,__LINE__);
}  
/********************************************************************************************************/
void dprm_free_swentry_rcu(struct rcu_head *switch_entry_p)
{
  struct dprm_switch_entry *sw_info_p;
  uchar8_t offset;
  if(switch_entry_p)
  {
    offset = SWNODE_SWTBL_OFFSET;
    sw_info_p = (struct dprm_switch_entry *)( ( (char *)switch_entry_p - (2*(sizeof(struct mchash_dll_node *))))-offset);
    mempool_release_mem_block(dprm_switch_mempool_g,(uchar8_t*)sw_info_p,sw_info_p->heap_b);
  }  
  else
    dprm_debug("%s:%d NULL passed for deletion.\r\n",__FUNCTION__,__LINE__);
}
/*****************************************************************************************************/
void dprm_attributes_free_attribute_entry_rcu(struct rcu_head *attribute_entry_p)
{
  struct dprm_resource_attribute_entry *attribute_info_p;
  
  if(attribute_entry_p)
  {
    attribute_info_p = (struct dprm_resource_attribute_entry *)(((char *)attribute_entry_p) - sizeof(of_list_node_t));
    mempool_release_mem_block(dprm_attributes_mempool_g,(uchar8_t*)attribute_info_p,attribute_info_p->heap_b);
  }
  else
   dprm_debug("%s:%d NULL passed for deletion.\r\n",__FUNCTION__,__LINE__);
}  

/***************************************************************************************************/
void dprm_attributes_free_attribute_value_entry_rcu(struct rcu_head *attribute_value_entry_p)
{
  struct dprm_resource_attribute_value_entry *attribute_value_info_p;
  
  if(attribute_value_entry_p)
  {
    attribute_value_info_p = (struct dprm_resource_attribute_value_entry *)(((char *)attribute_value_entry_p) - sizeof(of_list_node_t));
    mempool_release_mem_block(dprm_attributes_value_mempool_g,(uchar8_t*)attribute_value_info_p,attribute_value_info_p->heap_b);
  }
  else
   dprm_debug("%s:%d NULL passed for deletion.\r\n",__FUNCTION__,__LINE__);
}
/*****************************************************************************************************/
int32_t dprm_get_first_switch(struct    dprm_switch_info *config_info_p,
                              struct    dprm_switch_runtime_info *runtime_info_p,
                              uint64_t  *switch_handle_p)
{

  struct dprm_switch_entry* switch_info_p = NULL;
  uint32_t hashkey;
  
  uchar8_t offset;

  if(dprm_switch_table_g == NULL)
    return DPRM_FAILURE;
  
  CNTLR_RCU_READ_LOCK_TAKE();
 
  MCHASH_TABLE_SCAN(dprm_switch_table_g,hashkey)
  {
    offset = SWNODE_SWTBL_OFFSET;
    MCHASH_BUCKET_SCAN(dprm_switch_table_g,hashkey,switch_info_p,struct dprm_switch_entry *,offset)
    {
      strcpy(config_info_p->name,switch_info_p->name);
      config_info_p->switch_type = switch_info_p->switch_type;
      strcpy(config_info_p->switch_fqdn,switch_info_p->switch_fqdn_p);
      config_info_p->ipv4addr.baddr_set = switch_info_p->ipv4addr.baddr_set;
      config_info_p->ipv4addr.addr = switch_info_p->ipv4addr.addr;

      /* TBD Pass IPv6 information */

      runtime_info_p->number_of_datapaths  = switch_info_p->number_of_datapaths;
      runtime_info_p->number_of_attributes = switch_info_p->number_of_attributes;

      *switch_handle_p = switch_info_p->magic;
      *switch_handle_p = ((*switch_handle_p <<32) | (switch_info_p->index));

      CNTLR_RCU_READ_LOCK_RELEASE();
      return DPRM_SUCCESS;
    }
  }  
  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_ERROR_NO_MORE_ENTRIES;
}
/********************************************************************************************************/
int32_t dprm_get_next_switch(struct   dprm_switch_info *config_info_p,
                             struct   dprm_switch_runtime_info *runtime_info_p,
                             uint64_t *switch_handle_p)
{
  struct dprm_switch_entry* switch_info_p = NULL;
  uint32_t hashkey;
  uint8_t current_entry_found_b;

  uchar8_t offset;

  CNTLR_RCU_READ_LOCK_TAKE();
  current_entry_found_b = FALSE;

  MCHASH_TABLE_SCAN(dprm_switch_table_g,hashkey)
  {
    offset = SWNODE_SWTBL_OFFSET;
    MCHASH_BUCKET_SCAN(dprm_switch_table_g,hashkey,switch_info_p,struct dprm_switch_entry *,offset)
    {
      if(current_entry_found_b == FALSE)
      {
        if(*switch_handle_p != (switch_info_p->switch_handle))
          continue;
    
        else
        {
          current_entry_found_b = TRUE;
          continue;
        }
        
       /*Skip the First matching Switch and Get the next Switch */ 
      }  
      strcpy(config_info_p->name,switch_info_p->name);
      config_info_p->switch_type = switch_info_p->switch_type;
      strcpy(config_info_p->switch_fqdn,switch_info_p->switch_fqdn_p);
      config_info_p->ipv4addr.baddr_set = switch_info_p->ipv4addr.baddr_set;
      config_info_p->ipv4addr.addr = switch_info_p->ipv4addr.addr;
      /* TBD Pass IPv6 information */

      runtime_info_p->number_of_datapaths  = switch_info_p->number_of_datapaths;
      runtime_info_p->number_of_attributes = switch_info_p->number_of_attributes;

      *switch_handle_p = switch_info_p->magic;
      *switch_handle_p = ((*switch_handle_p <<32) | (switch_info_p->index));

      CNTLR_RCU_READ_LOCK_RELEASE();
      return DPRM_SUCCESS;
    }
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  if(current_entry_found_b == TRUE)
    return DPRM_ERROR_NO_MORE_ENTRIES;
  else
    return DPRM_ERROR_INVALID_SWITCH_HANDLE;
} 
/*********************************************************************************************************/
int32_t dprm_register_switch_notifications(uint32_t notification_type,
                                           dprm_switch_notifier_fn switch_notifier_fn,
                                           void     *callback_arg1,
                                           void     *callback_arg2)
{
  struct  dprm_notification_app_hook_info *app_entry_p;
  uint8_t heap_b;
  int32_t retval = DPRM_SUCCESS;;
  uchar8_t lstoffset;
  lstoffset = DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET;

  if((notification_type < DPRM_SWITCH_FIRST_NOTIFICATION_TYPE) || (notification_type > DPRM_SWITCH_LAST_NOTIFICATION_TYPE))
    return DPRM_ERROR_INVALID_SWITCH_NOTIFICATION_TYPE;
 
  if(switch_notifier_fn == NULL)
    return DPRM_ERROR_NULL_CALLBACK_SWITCH_NOTIFICATION_HOOK;
 
  retval = mempool_get_mem_block(dprm_notifications_mempool_g,(uchar8_t**)&app_entry_p,&heap_b);
  if(retval != MEMPOOL_SUCCESS)
    return DPRM_ERROR_NO_MEM;

  app_entry_p->call_back = (void*)switch_notifier_fn;
  app_entry_p->cbk_arg1  = callback_arg1;
  app_entry_p->cbk_arg2  = callback_arg2;
  app_entry_p->heap_b    = heap_b;
    /* Add Application to the list of switch Applications maintained per notification type. */
  OF_APPEND_NODE_TO_LIST(switch_notifications[notification_type],app_entry_p,lstoffset);

  if(retval != OF_SUCCESS)
    mempool_release_mem_block(dprm_notifications_mempool_g,(uchar8_t*)app_entry_p,app_entry_p->heap_b); 
  return retval; 
} 
int32_t dprm_deregister_switch_notifications(uint32_t notification_type,
                                             dprm_switch_notifier_fn switch_notifier_fn)
{
  struct dprm_notification_app_hook_info *app_entry_p,*prev_app_entry_p = NULL;
  uchar8_t lstoffset;
  lstoffset = DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET;
  
  CNTLR_RCU_READ_LOCK_TAKE();
  OF_LIST_SCAN(switch_notifications[notification_type],app_entry_p,struct dprm_notification_app_hook_info *,lstoffset )
  {
    if(app_entry_p->call_back != switch_notifier_fn)
    {
      prev_app_entry_p = app_entry_p;
      continue;
    }
    OF_REMOVE_NODE_FROM_LIST(switch_notifications[notification_type],app_entry_p,prev_app_entry_p,lstoffset );
    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_SUCCESS;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_ERROR_NO_SWITCH_NOTIFICATION;
}  
/*********************************************************************************************************/
int32_t dprm_create_distributed_forwarding_domain(struct  dprm_distributed_forwarding_domain_info* domain_info_p,
                                                  uint64_t* output_handle_p)
{
  struct dprm_domain_entry* domain_entry_p = NULL;
  struct dprm_domain_entry* domain_entry_scan_p = NULL;
  
  uint32_t dmnode_oftable_entries_max; 
  uint32_t index,magic;
  uint32_t hashkey,ii;

  struct   dprm_domain_notification_data notification_data;
  struct   dprm_notification_app_hook_info *app_entry_p;
  
  uchar8_t  offset;
  uint8_t   heap_b;
  uchar8_t* hashobj_p = NULL;
  int32_t   status,ret_value;

  uint32_t dmnode_dptable_entries_max,dmnode_dptable_static_entries;

  uchar8_t lstoffset;
  lstoffset = DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET;

  ret_value = DPRM_SUCCESS;

  if(dprm_domain_table_g == NULL)
    return DPRM_FAILURE;

  if(strlen(domain_info_p->name) < 8)
    return DPRM_ERROR_NAME_LENGTH_NOT_ENOUGH;

  hashkey = dprm_get_hashval_byname(domain_info_p->name,dprm_no_of_domain_buckets_g);

  CNTLR_RCU_READ_LOCK_TAKE();
  offset = DMNODE_DMTBL_OFFSET;
  MCHASH_BUCKET_SCAN(dprm_domain_table_g,hashkey,domain_entry_scan_p,struct dprm_domain_entry *,offset)
  {
    if(strcmp(domain_info_p->name,domain_entry_scan_p->name))
      continue;
    ret_value = DPRM_ERROR_DUPLICATE_RESOURCE;
    goto DPRMEXIT2;
  }

  ret_value = mempool_get_mem_block(dprm_domain_mempool_g,(uchar8_t**)&domain_entry_p,&heap_b);
  if(ret_value != MEMPOOL_SUCCESS)
    goto DPRMEXIT2;

  /* copy parameters */
  strcpy(domain_entry_p->name,domain_info_p->name);
  strcpy(domain_entry_p->ttp_name,domain_info_p->ttp_name);
  if(strlen(domain_info_p->expected_subject_name_for_authentication))
  {
    strncpy(domain_entry_p->expected_subject_name_for_authentication,
            domain_info_p->expected_subject_name_for_authentication,
            DPRM_DOMAIN_MAX_X509_SUBJECT_LEN);
  } 
  domain_entry_p->heap_b = heap_b;
#if 0
  for(ii = 0;ii < DPRM_MAX_NUMBER_OF_TABLES;ii++)
    domain_entry_p->table_p[ii] = NULL;
#endif
  
  dprm_get_dmnode_datapath_mempoolentries(&dmnode_dptable_entries_max,&dmnode_dptable_static_entries);

  ret_value = mchash_table_create(((dmnode_dptable_entries_max / 5)+1),
                                    dmnode_dptable_entries_max,
                                    dprm_free_dmnode_dptbl_entry_rcu,
                                    &(domain_entry_p->dprm_dmnode_dptable_p)
                                    );
  if(ret_value != MCHASHTBL_SUCCESS)
    goto DPRMEXIT2;

  dprm_no_of_dmnode_datapath_buckets_g = (dmnode_dptable_entries_max / 5) + 1;
#if 0

  dmnode_oftable_entries_max = DPRM_MAX_DMNODE_OFTABLE_ENTRIES; 

  /* create hash table to add tables later */
  
  ret_value = mempool_create_pool("dprm_dmnode_oftable_pool",
                                   sizeof(struct dprm_oftable_entry),
                                   dmnode_oftable_entries_max,dmnode_oftable_entries_max,
                                   dmnode_oftable_entries_max/10,dmnode_oftable_entries_max/3,
                                   dmnode_oftable_entries_max/10,FALSE,FALSE,0,
                                   &domain_entry_p->dprm_dmnode_oftable_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    goto DPRMEXIT2;
 
  ret_value = mchash_table_create(dmnode_oftable_entries_max,
                                  dmnode_oftable_entries_max,
                                  dprm_free_dmnode_oftbl_entry_rcu,
                                  &(domain_entry_p->dprm_dmnode_oftable_p)
                                  );
  if(ret_value != MCHASHTBL_SUCCESS)
    goto DPRMEXIT2;
  
  dprm_no_of_dmnode_oftable_buckets_g = (dmnode_oftable_entries_max / 5) + 1;
#endif
  hashobj_p = ((uchar8_t *)domain_entry_p) + DMNODE_DMTBL_OFFSET;
  MCHASH_APPEND_NODE(dprm_domain_table_g,hashkey,domain_entry_p,index,magic,hashobj_p,status);
  if(FALSE == status)
  {
    ret_value = DPRM_FAILURE;
    goto DPRMEXIT2;
  }
 
  *output_handle_p = magic;
  *output_handle_p = ((*output_handle_p <<32) | (index));

  domain_entry_p->magic = magic;
  domain_entry_p->index = index;

  (domain_entry_p->domain_handle) = *output_handle_p;

  OF_LIST_INIT(domain_entry_p->attributes,dprm_attributes_free_attribute_entry_rcu);
  
  if(cntlr_domain_init(domain_entry_p) != OF_SUCCESS)
    goto DPRMEXIT2;

  /* Send domain add notification to the registered Applications. */
  strcpy(notification_data.domain_name,domain_info_p->name);
  strcpy(notification_data.ttp_name,domain_info_p->ttp_name);

  OF_LIST_SCAN(domain_notifications[DPRM_DOMAIN_ADDED],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
  {
    ((dprm_domain_notifier_fn)(app_entry_p->call_back))(DPRM_DOMAIN_ADDED,
                                                        *output_handle_p,
                                                        notification_data,
                                                        app_entry_p->cbk_arg1,
                                                        app_entry_p->cbk_arg2);
  }

  OF_LIST_SCAN(domain_notifications[DPRM_DOMAIN_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
  {
    ((dprm_domain_notifier_fn)(app_entry_p->call_back))(DPRM_DOMAIN_ADDED,
                                                        *output_handle_p,
                                                        notification_data,
                                                        app_entry_p->cbk_arg1,
                                                        app_entry_p->cbk_arg2);
  }

  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_SUCCESS;

DPRMEXIT2:
   
  if(domain_entry_p)
  {
    if(domain_entry_p->dprm_dmnode_dptable_p)      
      mchash_table_delete(domain_entry_p->dprm_dmnode_dptable_p);
#if 0
    if(domain_entry_p->dprm_dmnode_oftable_p)
      mchash_table_delete(domain_entry_p->dprm_dmnode_oftable_p);
#endif

    mempool_release_mem_block(dprm_domain_mempool_g,(uchar8_t*)domain_entry_p,domain_entry_p->heap_b);
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return ret_value;
}
/*******************************************************************************************************/
int32_t dprm_get_forwarding_domain_handle(char* name_p, uint64_t* domain_handle_p)
{
  uint32_t hashkey;
  uchar8_t offset;


  struct dprm_domain_entry* domain_entry_p = NULL;

  hashkey = dprm_get_hashval_byname(name_p,dprm_no_of_domain_buckets_g);

  if(dprm_domain_table_g == NULL)
    return DPRM_FAILURE;

  CNTLR_RCU_READ_LOCK_TAKE();
  offset = DMNODE_DMTBL_OFFSET;
  MCHASH_BUCKET_SCAN(dprm_domain_table_g,hashkey,domain_entry_p,struct dprm_domain_entry *,offset)
  {
    if(strcmp(domain_entry_p->name,name_p))
      continue;
    CNTLR_RCU_READ_LOCK_RELEASE();
  
    *domain_handle_p = domain_entry_p->magic;
    *domain_handle_p = ((*domain_handle_p <<32) | (domain_entry_p->index));
 
    return DPRM_SUCCESS;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_ERROR_INVALID_NAME;
}
/********************************************************************************************************/
int32_t get_domain_byhandle(uint64_t domain_handle,struct dprm_domain_entry** domain_info_p_p)
{
  /* Caller shall take RCU locks. */
  uint32_t index,magic;
  uint8_t status_b;

  magic = (uint32_t)(domain_handle >>32);
  index = (uint32_t)domain_handle;

  MCHASH_ACCESS_NODE(dprm_domain_table_g,index,magic,*domain_info_p_p,status_b);
  if(status_b == TRUE)
    return DPRM_SUCCESS;
  return DPRM_ERROR_INVALID_DOMAIN_HANDLE;
}
/******************************************************************************************************/
int32_t dprm_get_exact_forwarding_domain(uint64_t handle,
                              struct dprm_distributed_forwarding_domain_info* config_info_p,
                              struct dprm_distributed_forwarding_domain_runtime_info* runtime_info_p)
{
  uint32_t index,magic;
  struct   dprm_domain_entry* domain_info_p;
  uint8_t  status_b;

  magic = (uint32_t)(handle >>32);
  index = (uint32_t)handle;

  CNTLR_RCU_READ_LOCK_TAKE();

  MCHASH_ACCESS_NODE(dprm_domain_table_g,index,magic,domain_info_p,status_b);
  if(status_b == TRUE)
  {
    strcpy(config_info_p->name,domain_info_p->name);
    strcpy(config_info_p->ttp_name,domain_info_p->ttp_name);
    strncpy(config_info_p->expected_subject_name_for_authentication,
            domain_info_p->expected_subject_name_for_authentication,
            DPRM_DOMAIN_MAX_X509_SUBJECT_LEN);
    
   // runtime_info_p->number_of_tables     = domain_info_p->number_of_name_based_tables;
    runtime_info_p->number_of_datapaths  = domain_info_p->number_of_datapaths;
    runtime_info_p->number_of_attributes = domain_info_p->number_of_attributes;

    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_SUCCESS;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_ERROR_INVALID_DOMAIN_HANDLE;
}
#if 0
/******************************************************************************************************/
int32_t dprm_add_oftable_to_domain(uint64_t domain_handle,
                                   struct   dprm_table_info *dprm_table_p,
                                   uint64_t *oftable_handle_p)
{
  struct    dprm_domain_entry* domain_info_p = NULL;
  struct    dprm_oftable_entry* oftable_entry_p = NULL;
  struct    dprm_match_field* match_field_p = NULL; 
  uint32_t  index,magic;
  uchar8_t  *hashobj_p = NULL;
  uint8_t   heap_b,status_b = FALSE;
  int32_t   ret_value;
  uint32_t  ii,hashkey,offset;
  char      *name;

  //struct    dprm_domain_notification_data notification_data;
  //struct    dprm_notification_app_hook_info *app_entry_p;

  
  name = dprm_table_p->table_name;

  magic = (uint32_t)(domain_handle >>32);
  index = (uint32_t)domain_handle;

  CNTLR_RCU_READ_LOCK_TAKE();

  MCHASH_ACCESS_NODE(dprm_domain_table_g,index,magic,domain_info_p,status_b);
  if(status_b == FALSE)
  {
    ret_value = DPRM_ERROR_INVALID_DOMAIN_HANDLE;
    goto DPRMEXIT_TABLE;
  }

  if(dprm_table_p->no_of_match_fields != 0)
  {

  match_field_p = (struct dprm_match_field *)calloc(dprm_table_p->no_of_match_fields,sizeof(struct dprm_match_field));
      
  if(match_field_p == NULL)
  {
    ret_value = DPRM_ERROR_NO_MEM;
    goto DPRMEXIT_TABLE;
  }  
  }
  else
    match_field_p = NULL;

  /* Check for duplicate table with table name */
  hashkey = dprm_get_hashval_byname(name,dprm_no_of_dmnode_oftable_buckets_g);
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "bucket = %d",hashkey);

  offset = DMNODE_DMTBL_OFFSET;

  MCHASH_BUCKET_SCAN(domain_info_p->dprm_dmnode_oftable_p,hashkey,oftable_entry_p,struct dprm_oftable_entry *,offset)
  {
    if(strcmp(name,oftable_entry_p->table_name))
      continue;
    ret_value = DPRM_ERROR_DUPLICATE_RESOURCE;
    goto DPRMEXIT_TABLE;
  }

  ret_value = mempool_get_mem_block(domain_info_p->dprm_dmnode_oftable_mempool_g,(uchar8_t**)&oftable_entry_p,&heap_b);
  if(ret_value != MEMPOOL_SUCCESS)
    goto DPRMEXIT_TABLE;

  /* Add table entry to the table hash table */
  strncpy(oftable_entry_p->table_name,name, DPRM_MAX_LEN_OF_TABLE_NAME);
  oftable_entry_p->table_id = dprm_table_p->table_id;

  /* copy match fields */
  oftable_entry_p->match_field_p = match_field_p;
  oftable_entry_p->no_of_match_fields = dprm_table_p->no_of_match_fields;
  for(ii = 0;(ii < dprm_table_p->no_of_match_fields);ii++)
  {
    match_field_p[ii].type = dprm_table_p->match_field_p[ii].type;
    match_field_p[ii].mask = dprm_table_p->match_field_p[ii].mask;  
  }

  hashobj_p = ((uchar8_t *)oftable_entry_p) + OFTABLENODE_OFTABLETBL_OFFSET;
  MCHASH_APPEND_NODE(domain_info_p->dprm_dmnode_oftable_p,hashkey,oftable_entry_p,index,magic,hashobj_p,status_b);
  if(FALSE == status_b)
  {
    ret_value = DPRM_FAILURE;
    goto DPRMEXIT_TABLE;
  }
  *oftable_handle_p = magic;
  *oftable_handle_p = ((*oftable_handle_p <<32) | (index));

  oftable_entry_p->oftable_handle = *oftable_handle_p;
  
  oftable_entry_p->magic = magic;
  oftable_entry_p->index = index;

  oftable_entry_p->domain_entry_p = domain_info_p;
  domain_info_p->number_of_name_based_tables++;

  OF_LIST_INIT(oftable_entry_p->attributes,dprm_attributes_free_attribute_entry_rcu);
#if 0
  /* Send table attached to domain notification to the registered Applications. */
  strcpy(notification_data.domain_name,domain_info_p->name);
  strcpy(notification_data.table_name,name);
  notification_data.table_id     = dprm_table_p->table_id;
  notification_data.table_handle = *oftable_handle_p;

  OF_LIST_SCAN(domain_notifications[DPRM_DOMAIN_OFTABLE_ADDED],app_entry_p,struct dprm_notification_app_hook_info*)
  {
    ((dprm_domain_notifier_fn)(app_entry_p->call_back))(DPRM_DOMAIN_OFTABLE_ADDED,
                                                        domain_handle,
                                                        notification_data,
                                                        app_entry_p->cbk_arg1,
                                                        app_entry_p->cbk_arg2);
  }

  OF_LIST_SCAN(domain_notifications[DPRM_DOMAIN_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*)
  {
    ((dprm_domain_notifier_fn)(app_entry_p->call_back))(DPRM_DOMAIN_OFTABLE_ADDED,
                                                        domain_handle,
                                                        notification_data,
                                                        app_entry_p->cbk_arg1,
                                                        app_entry_p->cbk_arg2);
  }
#endif
  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_SUCCESS;
DPRMEXIT_TABLE:
  if(oftable_entry_p)
    mempool_release_mem_block(domain_info_p->dprm_dmnode_oftable_mempool_g,(uchar8_t*)oftable_entry_p,oftable_entry_p->heap_b);
  if(match_field_p)
   free(match_field_p); 
  CNTLR_RCU_READ_LOCK_RELEASE();
  return ret_value;
}
/******************************************************************************************************/
int32_t dprm_get_exact_domain_oftable(uint64_t domain_handle,
                                      struct   dprm_table_info *config_info_p,
                                      uint64_t oftable_handle_p)
 {
   struct   dprm_domain_entry*  domain_info_p;
   struct   dprm_oftable_entry* oftable_entry_p;
   uint32_t ii,index,magic;
   int32_t  status_b;

   magic = (uint32_t)(domain_handle >>32);
   index = (uint32_t)domain_handle;

   CNTLR_RCU_READ_LOCK_TAKE();

   MCHASH_ACCESS_NODE(dprm_domain_table_g,index,magic,domain_info_p,status_b);
   if(status_b == FALSE)
   {
     CNTLR_RCU_READ_LOCK_RELEASE();
     return DPRM_ERROR_INVALID_DOMAIN_HANDLE;
   }
 
   magic = (uint32_t)(oftable_handle_p >>32);
   index = (uint32_t)(oftable_handle_p);
   MCHASH_ACCESS_NODE(domain_info_p->dprm_dmnode_oftable_p,index,magic,oftable_entry_p,status_b);
   if(status_b)
   {
     config_info_p->table_id = oftable_entry_p->table_id;
     strcpy(config_info_p->table_name,oftable_entry_p->table_name);
  
     /* Pass table information */
     if(oftable_entry_p->no_of_match_fields > config_info_p->no_of_match_fields)
     {
       config_info_p->no_of_match_fields = oftable_entry_p->no_of_match_fields;
       CNTLR_RCU_READ_LOCK_RELEASE();
       /* Not enough space in config structure to copy match field properties of the table */
       return DPRM_ERROR_NO_OF_MATCH_FIELDS; 
     }
     
     for(ii = 0;(ii < oftable_entry_p->no_of_match_fields);ii++)
     {
       config_info_p->match_field_p[ii].type = oftable_entry_p->match_field_p[ii].type;
       config_info_p->match_field_p[ii].mask = oftable_entry_p->match_field_p[ii].mask;
     }
     config_info_p->no_of_match_fields = oftable_entry_p->no_of_match_fields;     
     
     CNTLR_RCU_READ_LOCK_RELEASE();
     return DPRM_SUCCESS;
   }
   CNTLR_RCU_READ_LOCK_RELEASE();
   return DPRM_ERROR_INVALID_OFTABLE_HANDLE;
 }
/******************************************************************************************************/
int32_t dprm_delete_oftable_from_domain(uint64_t domain_handle,uint64_t oftable_handle)
{
  struct   dprm_domain_entry*  domain_info_p;
  struct   dprm_oftable_entry* oftable_info_p;
  uint32_t index,magic;
  int32_t  status_b;
  uchar8_t offset;

  struct   dprm_domain_notification_data notification_data;
  struct   dprm_notification_app_hook_info *app_entry_p;
  uchar8_t lstoffset;
  lstoffset = DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET;

  if(dprm_domain_table_g == NULL)
    return DPRM_FAILURE;

  magic = (uint32_t)(domain_handle >>32);
  index = (uint32_t)domain_handle;

  CNTLR_RCU_READ_LOCK_TAKE();

  MCHASH_ACCESS_NODE(dprm_domain_table_g,index,magic,domain_info_p,status_b);
  if(status_b == FALSE)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_ERROR_INVALID_DOMAIN_HANDLE;
  }

  magic = (uint32_t)(oftable_handle >>32);
  index = (uint32_t)oftable_handle;

  MCHASH_ACCESS_NODE(domain_info_p->dprm_dmnode_oftable_p,index,magic,oftable_info_p,status_b);
  if(status_b == FALSE)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_ERROR_INVALID_OFTABLE_HANDLE;
  }
  
  /* Send table detached to domain notification to the registered Applications. */
  
  strcpy(notification_data.domain_name,domain_info_p->name);
  strcpy(notification_data.table_name,oftable_info_p->table_name);
  notification_data.table_id     = oftable_info_p->table_id;
  notification_data.table_handle = oftable_handle;

  OF_LIST_SCAN(domain_notifications[DPRM_DOMAIN_OFTABLE_DELETE],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
  {
    ((dprm_domain_notifier_fn)(app_entry_p->call_back))(DPRM_DOMAIN_OFTABLE_DELETE,
                                                        domain_handle,
                                                        notification_data,
                                                        app_entry_p->cbk_arg1,
                                                        app_entry_p->cbk_arg2);
  }

  OF_LIST_SCAN(domain_notifications[DPRM_DOMAIN_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
  {
    ((dprm_domain_notifier_fn)(app_entry_p->call_back))(DPRM_DOMAIN_OFTABLE_DELETE,
                                                        domain_handle,
                                                        notification_data,
                                                        app_entry_p->cbk_arg1,
                                                        app_entry_p->cbk_arg2);
  }
  free(oftable_info_p->match_field_p);
  
  dprm_debug("%s:%d About to delete a oftable from domain \r\n",__FUNCTION__,__LINE__);
  offset = OFTABLENODE_OFTABLETBL_OFFSET;
  MCHASH_DELETE_NODE_BY_REF(domain_info_p->dprm_dmnode_oftable_p,index,magic,struct dprm_oftable_entry *,offset,status_b);
  if(status_b == TRUE)
  {
    domain_info_p->number_of_name_based_tables--;
    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_SUCCESS;
  }
  else
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_FAILURE;
  }
}
/******************************************************************************************************/  
int32_t dprm_get_oftable_handle(uint64_t domain_handle,char* oftable_name_p,uint64_t* oftable_handle_p)
{
  /* Caller shall take RCU locks. */
  struct   dprm_domain_entry* domain_info_p;
  struct   dprm_oftable_entry*  oftable_entry_p = NULL;
  uchar8_t offset;
  uint32_t hashkey;

  int32_t ret_value;

  ret_value = get_domain_byhandle(domain_handle,&domain_info_p);
  if(ret_value == DPRM_ERROR_INVALID_DOMAIN_HANDLE)
    return ret_value;

  hashkey = dprm_get_hashval_byname(oftable_name_p,dprm_no_of_dmnode_oftable_buckets_g);

  offset = OFTABLENODE_OFTABLETBL_OFFSET;
  MCHASH_BUCKET_SCAN(domain_info_p->dprm_dmnode_oftable_p,hashkey,oftable_entry_p,struct dprm_oftable_entry *,offset)
  {
    if(!strcmp(oftable_entry_p->table_name,oftable_name_p))
    {
      *oftable_handle_p = oftable_entry_p->magic;
      *oftable_handle_p = ((*oftable_handle_p <<32) | (oftable_entry_p->index));
      return DPRM_SUCCESS;
    }
    continue;
  }
  return DPRM_ERROR_INVALID_NAME;
}
/******************************************************************************************************/  
int32_t get_domain_oftable_byhandle(uint64_t domain_handle,uint64_t oftable_handle_p,struct dprm_oftable_entry **table_info_p_p)
{
  /* Caller shall take RCU locks. */
  uint32_t index,magic;
  struct dprm_domain_entry* domain_info_p;
  uint8_t status_b;

  magic = (uint32_t)(domain_handle >>32);
  index = (uint32_t)domain_handle;

  MCHASH_ACCESS_NODE(dprm_domain_table_g,index,magic,domain_info_p,status_b);
  if(CNTLR_LIKELY(status_b))
  {
    magic = (uint32_t)(oftable_handle_p >>32);
    index = (uint32_t)oftable_handle_p;

    MCHASH_ACCESS_NODE(domain_info_p->dprm_dmnode_oftable_p,index,magic,*table_info_p_p,status_b);
    if(status_b == TRUE)
      return DPRM_SUCCESS;
    else
      return DPRM_ERROR_INVALID_OFTABLE_HANDLE;
  }
  return DPRM_ERROR_INVALID_DOMAIN_HANDLE;
}  
/******************************************************************************************************/
int32_t get_domain_table_byname(uint64_t domain_handle,char* oftable_name_p,struct dprm_oftable_entry **table_info_p_p)
{
  /* Caller shall take RCU locks. */
  uint64_t oftable_handle;
  int32_t  retval;

  retval = dprm_get_oftable_handle(domain_handle,oftable_name_p,&oftable_handle);
  if(retval == DPRM_SUCCESS)
    retval = get_domain_oftable_byhandle(domain_handle,oftable_handle,table_info_p_p);
  return retval;
 }
/******************************************************************************************************/
int32_t dprm_get_first_domain_oftable(uint64_t domain_handle,
                                      struct   dprm_table_info *config_info_p,
                                      uint64_t *oftable_handle_p)
{
  struct dprm_domain_entry*  domain_info_p;
  struct dprm_oftable_entry* oftable_entry_p = NULL;
  uint32_t ii,index,magic;
  int32_t  status_b;
  uint32_t hashkey;

  uchar8_t offset;

  if(dprm_domain_table_g == NULL)
    return DPRM_FAILURE;

  magic = (uint32_t)(domain_handle >>32);
  index = (uint32_t)domain_handle;

  CNTLR_RCU_READ_LOCK_TAKE();

  MCHASH_ACCESS_NODE(dprm_domain_table_g,index,magic,domain_info_p,status_b);
  if(status_b == FALSE)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_ERROR_INVALID_DOMAIN_HANDLE;
  }

  if(domain_info_p->dprm_dmnode_oftable_p == NULL)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_FAILURE;
  }

  offset = OFTABLENODE_OFTABLETBL_OFFSET;
  MCHASH_TABLE_SCAN(domain_info_p->dprm_dmnode_oftable_p,hashkey)
  {
    MCHASH_BUCKET_SCAN(domain_info_p->dprm_dmnode_oftable_p,hashkey,oftable_entry_p,struct dprm_oftable_entry *,offset)
    {
      *oftable_handle_p = oftable_entry_p->oftable_handle;
      config_info_p->table_id = oftable_entry_p->table_id;
      strcpy(config_info_p->table_name,oftable_entry_p->table_name);

      /* Pass table information */
      if(oftable_entry_p->no_of_match_fields > config_info_p->no_of_match_fields)
      {
        config_info_p->no_of_match_fields = oftable_entry_p->no_of_match_fields;
        CNTLR_RCU_READ_LOCK_RELEASE();
        /* Not enough space in config structure to copy match field properties of the table */
        return DPRM_ERROR_NO_OF_MATCH_FIELDS;
      }

      for(ii = 0;(ii < oftable_entry_p->no_of_match_fields);ii++)
      {
        config_info_p->match_field_p[ii].type = oftable_entry_p->match_field_p[ii].type;
        config_info_p->match_field_p[ii].mask = oftable_entry_p->match_field_p[ii].mask;
      }
      config_info_p->no_of_match_fields = oftable_entry_p->no_of_match_fields;
      CNTLR_RCU_READ_LOCK_RELEASE();
      return DPRM_SUCCESS;
    }
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_ERROR_NO_MORE_ENTRIES;
}
/******************************************************************************************************/
int32_t dprm_get_next_domain_oftable(uint64_t domain_handle,
                                     struct   dprm_table_info *config_info_p,
                                     uint64_t  *oftable_handle_p)
{
  struct   dprm_domain_entry*  domain_info_p;
  struct   dprm_oftable_entry* oftable_entry_p = NULL;
  uint32_t ii,index,magic;
  int32_t  status_b;
  uint8_t  current_entry_found_b;
  uint32_t hashkey;

  uchar8_t offset;

  magic = (uint32_t)(domain_handle >>32);
  index = (uint32_t)domain_handle;

  CNTLR_RCU_READ_LOCK_TAKE();

  MCHASH_ACCESS_NODE(dprm_domain_table_g,index,magic,domain_info_p,status_b);
  if(CNTLR_UNLIKELY(status_b == FALSE))
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_ERROR_INVALID_DOMAIN_HANDLE;
  }
  
  offset = OFTABLENODE_OFTABLETBL_OFFSET;
  current_entry_found_b = 0;
  MCHASH_TABLE_SCAN(domain_info_p->dprm_dmnode_oftable_p,hashkey)
  {
    MCHASH_BUCKET_SCAN(domain_info_p->dprm_dmnode_oftable_p,hashkey,oftable_entry_p,struct dprm_oftable_entry *,offset)
    {
      if(current_entry_found_b == 0)
      {
        if(*oftable_handle_p != oftable_entry_p->oftable_handle)
          continue;
        else
        {
          current_entry_found_b = 1;
          continue;
        }
        /*Skip the First matching oftable and Get the next oftable */
      }
      *oftable_handle_p = oftable_entry_p->oftable_handle;
      config_info_p->table_id = oftable_entry_p->table_id;
      strcpy(config_info_p->table_name,oftable_entry_p->table_name);
                                                            
      /* Pass table information */
      if(oftable_entry_p->no_of_match_fields > config_info_p->no_of_match_fields)
      {
        config_info_p->no_of_match_fields = oftable_entry_p->no_of_match_fields;
        CNTLR_RCU_READ_LOCK_RELEASE();
        /* Not enough space in config structure to copy match field properties of the table */
        return DPRM_ERROR_NO_OF_MATCH_FIELDS;
      }

      for(ii = 0;(ii < oftable_entry_p->no_of_match_fields);ii++)
      {
        config_info_p->match_field_p[ii].type = oftable_entry_p->match_field_p[ii].type;
        config_info_p->match_field_p[ii].mask = oftable_entry_p->match_field_p[ii].mask;
      }
      config_info_p->no_of_match_fields = oftable_entry_p->no_of_match_fields; 
      CNTLR_RCU_READ_LOCK_RELEASE();
      return DPRM_SUCCESS;
    }
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  if(current_entry_found_b == 1)
    return DPRM_ERROR_NO_MORE_ENTRIES;
  else
    return DPRM_ERROR_INVALID_OFTABLE_HANDLE;
}
/******************************************************************************************************/
int32_t add_domain_oftable_to_idbased_list(uint64_t dm_handle,uint8_t table_id,struct dprm_oftable_entry *table_info_p)
{
  int32_t ret_val;
  struct  dprm_domain_entry* domain_entry_p;
  struct    dprm_domain_notification_data notification_data;
  struct    dprm_notification_app_hook_info *app_entry_p;
  uchar8_t lstoffset;
  lstoffset =DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET;
 
  if(table_id >= DPRM_MAX_NUMBER_OF_TABLES)
    return DPRM_FAILURE;
  
  ret_val = get_domain_byhandle(dm_handle,&domain_entry_p);
  
  if(ret_val != DPRM_SUCCESS)
    return DPRM_FAILURE;

  if(domain_entry_p->table_p[table_id] != NULL)
    return DPRM_FAILURE;
  
  domain_entry_p->table_p[table_id] = table_info_p;  

  of_async_msg_table_init(domain_entry_p->table_p[table_id]);

#if 1
  /* Send table attached to domain notification to the registered Applications. */
  strcpy(notification_data.domain_name,domain_entry_p->name);
  strcpy(notification_data.table_name,table_info_p->table_name);
  notification_data.table_id     = table_id;
  notification_data.table_handle = table_info_p->oftable_handle;

  OF_LIST_SCAN(domain_notifications[DPRM_DOMAIN_OFTABLE_ADDED],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
  {
    ((dprm_domain_notifier_fn)(app_entry_p->call_back))(DPRM_DOMAIN_OFTABLE_ADDED,
                                                        dm_handle,
                                                        notification_data,
                                                        app_entry_p->cbk_arg1,
                                                        app_entry_p->cbk_arg2);
  }

  OF_LIST_SCAN(domain_notifications[DPRM_DOMAIN_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
  {
    ((dprm_domain_notifier_fn)(app_entry_p->call_back))(DPRM_DOMAIN_OFTABLE_ADDED,
                                                        dm_handle,
                                                        notification_data,
                                                        app_entry_p->cbk_arg1,
                                                        app_entry_p->cbk_arg2);
  }
#endif
  return DPRM_SUCCESS;
}
/***************************************************************************************************/
int32_t del_domain_oftable_from_idbased_list(uint64_t dm_handle,uint8_t table_id)
{
  int32_t ret_val;
  struct  dprm_domain_entry* domain_entry_p;
       
  if(table_id >= DPRM_MAX_NUMBER_OF_TABLES)
    return DPRM_FAILURE;

  ret_val = get_domain_byhandle(dm_handle,&domain_entry_p);
           
  if(ret_val != DPRM_SUCCESS)
    return DPRM_FAILURE;

  if(domain_entry_p->table_p[table_id] == NULL)
    return DPRM_FAILURE;

  domain_entry_p->table_p[table_id] = NULL;
  return DPRM_SUCCESS;
}  

/***************************************************************************************************/
int32_t get_domain_oftable_from_idbased_list(uint64_t dm_handle,uint8_t table_id, 
                                             struct dprm_oftable_entry  **oftable_p_p)
{
  int32_t ret_val;
  struct  dprm_domain_entry* domain_entry_p;

  if(table_id >= DPRM_MAX_NUMBER_OF_TABLES)
    return DPRM_FAILURE;
  
  ret_val = get_domain_byhandle(dm_handle,&domain_entry_p);

  if(ret_val != DPRM_SUCCESS)
    return DPRM_FAILURE;
  
  if(domain_entry_p->table_p[table_id] == NULL)
    return DPRM_FAILURE;
  
  *oftable_p_p = domain_entry_p->table_p[table_id];
  return DPRM_SUCCESS;
}  
#endif
/***************************************************************************************************/
int32_t dprm_unregister_distributed_forwarding_domain(uint64_t handle)
{
  struct   dprm_domain_entry* domain_info_p = NULL;
  uint32_t index,magic;
  uint8_t  status_b = FALSE;
  uchar8_t offset; 

  struct   dprm_domain_notification_data notification_data;
  struct   dprm_notification_app_hook_info *app_entry_p;
  uchar8_t lstoffset;
  lstoffset = DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET;

  magic = (uint32_t)(handle >>32);
  index = (uint32_t)handle;

  if(dprm_domain_table_g == NULL)
    return DPRM_FAILURE;      

  CNTLR_RCU_READ_LOCK_TAKE();

  MCHASH_ACCESS_NODE(dprm_domain_table_g,index,magic,domain_info_p,status_b);
  if(status_b == TRUE)
  {
    if((domain_info_p->number_of_datapaths)|| (domain_info_p->number_of_attributes)
#if 0
        || (domain_info_p->number_of_name_based_tables) || (domain_info_p->number_of_tables_id_based)
#endif
)
    {
      dprm_debug("%s:%d Delete all the datapaths,attributes and oftables associated with the domain before unregistering the domain.\r\n",__FUNCTION__,__LINE__);
      CNTLR_RCU_READ_LOCK_RELEASE();
      return DPRM_ERROR_RESOURCE_NOTEMPTY;
    }

    /* Send domain delete notification to the registered Applications. */
    strcpy(notification_data.domain_name,domain_info_p->name);
    strcpy(notification_data.ttp_name,domain_info_p->ttp_name);
  
    OF_LIST_SCAN(domain_notifications[DPRM_DOMAIN_DELETE],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
    {
      ((dprm_domain_notifier_fn)(app_entry_p->call_back))(DPRM_DOMAIN_DELETE,
                                                          handle,
                                                          notification_data,
                                                          app_entry_p->cbk_arg1,
                                                          app_entry_p->cbk_arg2);
    }

    OF_LIST_SCAN(domain_notifications[DPRM_DOMAIN_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
    {
      ((dprm_domain_notifier_fn)(app_entry_p->call_back))(DPRM_DOMAIN_DELETE,
                                                          handle,
                                                          notification_data,
                                                          app_entry_p->cbk_arg1,
                                                          app_entry_p->cbk_arg2);
    }
    status_b = FALSE;
    offset = DMNODE_DMTBL_OFFSET;
    MCHASH_DELETE_NODE_BY_REF(dprm_domain_table_g,index,magic,struct dprm_domain_entry *,offset,status_b);
    CNTLR_RCU_READ_LOCK_RELEASE();
    if(status_b == TRUE)
      return DPRM_SUCCESS;
    else
      return DPRM_FAILURE;
  }
  else
    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_ERROR_INVALID_DOMAIN_HANDLE;
}
/********************************************************************************************************/
void dprm_free_domainentry_rcu(struct rcu_head *domain_entry_p)
{
  struct   dprm_domain_entry *domain_info_p;
  int32_t  ret_value;
  uchar8_t offset;

  if(domain_entry_p)
  {
    offset = DMNODE_DMTBL_OFFSET;
    domain_info_p = (struct dprm_domain_entry *)(((char *)domain_entry_p) - (2*(sizeof(struct mchash_dll_node *)))-offset);
    ret_value = mempool_release_mem_block(dprm_domain_mempool_g,(uchar8_t*)domain_info_p,domain_info_p->heap_b);
    if(ret_value != MEMPOOL_SUCCESS)
      dprm_debug("%s:%d Failed to release domain node  memory block.\r\n",__FUNCTION__,__LINE__);
    else
      dprm_debug("%s:%d Deleted succesfully the domain node from database.\r\n",__FUNCTION__,__LINE__);
  }
  else
   dprm_debug("%s:%d NULL passed for deletion.\r\n",__FUNCTION__,__LINE__);
}
#if 0
void dprm_free_dmnode_oftbl_entry_rcu(struct rcu_head *oftable_entry_p)
{
  struct dprm_oftable_entry *oftable_info_p;
  int32_t  ret_value;
  uchar8_t offset = 0;

  if(oftable_entry_p)
  {
    oftable_info_p = (struct dprm_oftable_entry *)(((char *)oftable_entry_p) - (2*(sizeof(struct mchash_dll_node *)))-offset);
    ret_value = mempool_release_mem_block(oftable_info_p->domain_entry_p->dprm_dmnode_oftable_mempool_g,(uchar8_t*)oftable_info_p,oftable_info_p->heap_b);
    if(ret_value != MEMPOOL_SUCCESS)
      dprm_debug("%s:%d Failed to release oftable node memory block.\r\n",__FUNCTION__,__LINE__);
    else
      dprm_debug("%s:%d Deleted succesfully the oftable node from database.\r\n",__FUNCTION__,__LINE__);
  }
  else
   dprm_debug("%s:%d NULL passed for deletion.\r\n",__FUNCTION__,__LINE__);
}
#endif
/*****************************************************************************************************/
void dprm_get_domain_mempoolentries(uint32_t* domain_entries_max,uint32_t* domain_static_entries)
{
  *domain_entries_max    = DPRM_MAX_DOMAIN_ENTRIES;
  *domain_static_entries = DPRM_MAX_DOMAIN_STATIC_ENTRIES;
}
/********************************************************************************************************/
int32_t dprm_get_first_forwarding_domain(struct    dprm_distributed_forwarding_domain_info* config_info_p,
                                         struct    dprm_distributed_forwarding_domain_runtime_info* runtime_info_p,
                                         uint64_t  *domain_handle_p)
{

  struct   dprm_domain_entry* domain_info_p = NULL;
  uint32_t hashkey;

  uchar8_t offset;

  CNTLR_RCU_READ_LOCK_TAKE();
  offset = DMNODE_DMTBL_OFFSET;

  if(dprm_domain_table_g == NULL)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();  
    return DPRM_FAILURE;
  }

  MCHASH_TABLE_SCAN(dprm_domain_table_g,hashkey)
  {
    MCHASH_BUCKET_SCAN(dprm_domain_table_g,hashkey,domain_info_p,struct dprm_domain_entry*,offset)
    {
      strcpy(config_info_p->name,domain_info_p->name);
      strcpy(config_info_p->ttp_name,domain_info_p->ttp_name);
      strncpy(config_info_p->expected_subject_name_for_authentication,
             domain_info_p->expected_subject_name_for_authentication,
             DPRM_DOMAIN_MAX_X509_SUBJECT_LEN);
      //config_info_p->number_of_tables  = domain_info_p->number_of_name_based_tables;

      runtime_info_p->number_of_datapaths  = domain_info_p->number_of_datapaths;
      runtime_info_p->number_of_attributes = domain_info_p->number_of_attributes;
      //runtime_info_p->number_of_tables     = domain_info_p->number_of_tables_id_based;
      *domain_handle_p = domain_info_p->magic;
      *domain_handle_p = ((*domain_handle_p <<32) | (domain_info_p->index));

      CNTLR_RCU_READ_LOCK_RELEASE();
      return DPRM_SUCCESS;
    }
  }  
  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_ERROR_NO_MORE_ENTRIES;
}
/********************************************************************************************************/
int32_t dprm_get_next_forwarding_domain(struct    dprm_distributed_forwarding_domain_info* config_info_p,
                                        struct    dprm_distributed_forwarding_domain_runtime_info* runtime_info_p,
                                        uint64_t  *domain_handle_p)
{
  struct   dprm_domain_entry* domain_info_p = NULL;
  uint32_t hashkey;
  uint8_t  current_entry_found_b;
  uchar8_t offset;

  CNTLR_RCU_READ_LOCK_TAKE();
  offset = DMNODE_DMTBL_OFFSET;

  current_entry_found_b = 0;

  MCHASH_TABLE_SCAN(dprm_domain_table_g,hashkey)
  {
    MCHASH_BUCKET_SCAN(dprm_domain_table_g,hashkey,domain_info_p,struct dprm_domain_entry*,offset)
    {
      if(current_entry_found_b == 0)
      {
        if(*domain_handle_p != domain_info_p->domain_handle)
          continue;
        else
        {
          current_entry_found_b = 1;
          continue;
        }
        /*Skip the First matching Domain and Get the next Domain */
      }
      strcpy(config_info_p->name,domain_info_p->name);
      strcpy(config_info_p->ttp_name,domain_info_p->ttp_name);
      strncpy(config_info_p->expected_subject_name_for_authentication,
             domain_info_p->expected_subject_name_for_authentication,
             DPRM_DOMAIN_MAX_X509_SUBJECT_LEN);
      //config_info_p->number_of_tables  = domain_info_p->number_of_name_based_tables;

      runtime_info_p->number_of_datapaths  = domain_info_p->number_of_datapaths;
      runtime_info_p->number_of_attributes = domain_info_p->number_of_attributes;
      //runtime_info_p->number_of_tables     = domain_info_p->number_of_tables_id_based; 
      *domain_handle_p = domain_info_p->magic;
      *domain_handle_p = ((*domain_handle_p <<32) | (domain_info_p->index));

      CNTLR_RCU_READ_LOCK_RELEASE();
      return DPRM_SUCCESS;
    }
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  if(current_entry_found_b == 1)
    return DPRM_ERROR_NO_MORE_ENTRIES;
  else
    return DPRM_ERROR_INVALID_DOMAIN_HANDLE;
}
/*******************************************************************************************************/
int32_t dprm_register_forwarding_domain_notifications(uint32_t notification_type,
                                                      dprm_domain_notifier_fn domain_notifier_fn,
                                                      void *callback_arg1,
                                                      void *callback_arg2)
{
  struct  dprm_notification_app_hook_info *app_entry_p;
  uint8_t heap_b;
  int32_t retval;
  long lstoffset;
  lstoffset = DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET;

  if((notification_type < DPRM_DOMAIN_FIRST_NOTIFICATION_TYPE) || (notification_type > DPRM_DOMAIN_LAST_NOTIFICATION_TYPE))
    return DPRM_ERROR_INVALID_DOMAIN_NOTIFICATION_TYPE;
  
  if(domain_notifier_fn == NULL)
    return DPRM_ERROR_NULL_CALLBACK_DOMAIN_NOTIFICATION_HOOK;
  
  retval = mempool_get_mem_block(dprm_notifications_mempool_g,(uchar8_t**)&app_entry_p,&heap_b);
  if(retval != MEMPOOL_SUCCESS)
    return DPRM_ERROR_NO_MEM;
  
  app_entry_p->call_back = (void*)domain_notifier_fn;
  app_entry_p->cbk_arg1  = callback_arg1;
  app_entry_p->cbk_arg2  = callback_arg2;
  app_entry_p->heap_b    = heap_b;

  /* Add Application to the list of domain Applications maintained per notification type. */

  OF_APPEND_NODE_TO_LIST(domain_notifications[notification_type],app_entry_p,lstoffset);

  if(retval != OF_SUCCESS)
    mempool_release_mem_block(dprm_notifications_mempool_g,(uchar8_t*)app_entry_p,app_entry_p->heap_b);
  return retval;
} 
int32_t dprm_deregister_domain_notifications(uint32_t notification_type,
                                             dprm_domain_notifier_fn domain_notifier_fn)
{
  struct dprm_notification_app_hook_info *app_entry_p,*prev_app_entry_p  = NULL;
  uchar8_t lstoffset;
  lstoffset = DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET;
 
  CNTLR_RCU_READ_LOCK_TAKE();
  OF_LIST_SCAN(domain_notifications[notification_type],app_entry_p,struct dprm_notification_app_hook_info *,lstoffset )
  {
    if(app_entry_p->call_back != domain_notifier_fn)
    {
      prev_app_entry_p = app_entry_p;
      continue;
    }
    OF_REMOVE_NODE_FROM_LIST(domain_notifications[notification_type],app_entry_p,prev_app_entry_p,lstoffset );
    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_SUCCESS;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_ERROR_NO_DOMAIN_NOTIFICATION;
}
/*******************************************************************************************************/
int32_t dprm_register_datapath(struct dprm_datapath_general_info* datapath_info_p,
                               uint64_t switch_handle, uint64_t domain_handle,
                               uint64_t* output_handle_p)
{

  struct dprm_datapath_entry* datapath_entry_p;
  struct dprm_datapath_entry* datapath_entry_scan_p = NULL;
  struct dprm_switch_entry* switch_info_p = NULL;
  struct dprm_domain_entry* domain_info_p = NULL;
  dprm_dp_table_entry_t *dp_table_info_p = NULL;
  uint32_t number_of_domain_tables=0;
  uint32_t ii=0;

  uint32_t  dpnode_porttable_entries_max,dpnode_porttable_static_entries;

  uint32_t  index = 0,magic = 0,index1 = 0,magic1 = 0,index2 = 0,magic2 = 0;
  uint32_t  hashkey;

  struct    dprm_switch_notification_data    notification_data;
  struct    dprm_domain_notification_data    notification_data1;
  struct    dprm_datapath_notification_data  notification_data2;
  struct    dprm_notification_app_hook_info  *app_entry_p;
  struct mempool_params mempool_info={};

  uchar8_t  offset;
  uchar8_t* hashobj_p = NULL;
  uint8_t   status_b = FALSE,status1_b = FALSE,status2_b = FALSE,heap_b;
  int32_t   ret_value;
  uchar8_t lstoffset;
  lstoffset = DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET;

  ret_value = DPRM_SUCCESS;

  if(dprm_datapath_table_g == NULL)
    return DPRM_FAILURE;

  hashkey = dprm_get_hashval_byid(datapath_info_p->dpid,dprm_no_of_datapath_buckets_g);

  CNTLR_RCU_READ_LOCK_TAKE();
  offset = DPNODE_DPTBL_OFFSET;
  MCHASH_BUCKET_SCAN(dprm_datapath_table_g,hashkey,datapath_entry_scan_p,struct dprm_datapath_entry *,offset)
  {
    if(datapath_info_p->dpid != datapath_entry_scan_p->dpid)
      continue;
    ret_value = DPRM_ERROR_DUPLICATE_RESOURCE;
    goto DPRMEXIT3;
  }
  ret_value = mempool_get_mem_block(dprm_datapath_mempool_g,(uchar8_t **)&datapath_entry_p,&heap_b);
  if(ret_value != MEMPOOL_SUCCESS)
    goto DPRMEXIT3;

  /* copy parameters */
  datapath_entry_p->dpid = datapath_info_p->dpid;


  if(datapath_info_p->is_multi_ns_config == 1)
	  datapath_entry_p->multi_namespaces = datapath_info_p->multi_namespaces;


  if(strlen(datapath_info_p->expected_subject_name_for_authentication))
  {
    strncpy(datapath_entry_p->expected_subject_name_for_authentication,
            datapath_info_p->expected_subject_name_for_authentication,
            DPRM_MAX_X509_SUBJECT_LEN);
  }

  if(strlen(datapath_info_p->datapath_name))
  {
    strncpy(datapath_entry_p->datapath_name,
            datapath_info_p->datapath_name,
            DPRM_MAX_DATAPATH_NAME_LEN);
  }


  datapath_entry_p->heap_b = heap_b;
 
  dprm_get_dpnode_port_mempoolentries(&dpnode_porttable_entries_max,&dpnode_porttable_static_entries);

  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;
  mempool_info.block_size = sizeof(struct dprm_port_entry);
  mempool_info.max_blocks = dpnode_porttable_entries_max;
  mempool_info.static_blocks = dpnode_porttable_static_entries;
  mempool_info.threshold_min = dpnode_porttable_static_entries/10;
  mempool_info.threshold_max = dpnode_porttable_static_entries/3;
  mempool_info.replenish_count = dpnode_porttable_static_entries/10;
  mempool_info.b_memset_not_required =  FALSE;
  mempool_info.b_list_per_thread_required =  FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("dprm_dpnode_port_pool", &mempool_info,
                                   &datapath_entry_p->dprm_dpnode_port_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    goto DPRMEXIT3;

  ret_value = mchash_table_create(((dpnode_porttable_entries_max / 5)+1),
                                    dpnode_porttable_entries_max,
                                    dprm_free_dpnode_porttbl_entry_rcu,
                                    &datapath_entry_p->dprm_dpnode_porttable_p);
  if(ret_value != MCHASHTBL_SUCCESS)
    goto DPRMEXIT3;

  dprm_no_of_dpnode_port_buckets_g = (dpnode_porttable_entries_max / 5) + 1;

  /* Add datapath entry to the datapath hash table */
  hashobj_p = ((uchar8_t *)datapath_entry_p) + DPNODE_DPTBL_OFFSET;
  MCHASH_APPEND_NODE(dprm_datapath_table_g,hashkey,datapath_entry_p,index,magic,hashobj_p,status_b);
  if(FALSE == status_b)
  {
    ret_value = DPRM_FAILURE;
    goto DPRMEXIT3;
  }
  *output_handle_p = magic;
  *output_handle_p = ((*output_handle_p <<32) | (index));

  datapath_entry_p->magic = magic;
  datapath_entry_p->index = index;

  (datapath_entry_p->datapath_handle) = *output_handle_p;

  /* Add datapath entry to the dp hash table in switch node whose handle is given as an input */
  
  magic1 = (uint32_t)(switch_handle >>32);
  index1 = (uint32_t)(switch_handle);
  datapath_entry_p->sw_handle_magic = magic1;
  datapath_entry_p->sw_handle_index = index1; 

  magic2 = (uint32_t)(domain_handle >>32);
  index2 = (uint32_t)(domain_handle);
  datapath_entry_p->dm_handle_magic = magic2;
  datapath_entry_p->dm_handle_index = index2;


  MCHASH_ACCESS_NODE(dprm_switch_table_g,index1,magic1,switch_info_p,status1_b);
  MCHASH_ACCESS_NODE(dprm_domain_table_g,index2,magic2,domain_info_p,status2_b);
 
  if((status1_b == TRUE) && (status2_b == TRUE))
  {
    status1_b = FALSE;
    status2_b = FALSE;
    if(cntlr_datapath_init(datapath_info_p->dpid,datapath_entry_p,domain_info_p) != OF_SUCCESS)
       goto DPRMEXIT3;

    hashkey = dprm_get_hashval_byid(datapath_info_p->dpid,dprm_no_of_swnode_datapath_buckets_g);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "bucket = %d",hashkey);

    hashobj_p = ((uchar8_t *)datapath_entry_p) + DPNODE_SWTBL_OFFSET;
    MCHASH_APPEND_NODE(switch_info_p->dprm_swnode_dptable_p,hashkey,datapath_entry_p,index,magic,hashobj_p,status1_b);
    if(status1_b == TRUE)
    {
      datapath_entry_p->swnode_dp_output_handle_p = magic;
      datapath_entry_p->swnode_dp_output_handle_p = ((datapath_entry_p->swnode_dp_output_handle_p <<32) | (index));
      datapath_entry_p->swnode_dp_saferef_magic   = magic;
      datapath_entry_p->swnode_dp_saferef_index   = index;
    } 
 
    /* Add datapath entry to the dp hash table in domain node whose handle is given as an input */

    hashkey = dprm_get_hashval_byid(datapath_info_p->dpid,dprm_no_of_dmnode_datapath_buckets_g);
    hashobj_p = ((uchar8_t *)datapath_entry_p) + DPNODE_DMTBL_OFFSET;
    MCHASH_APPEND_NODE(domain_info_p->dprm_dmnode_dptable_p,hashkey,datapath_entry_p,index1,magic1,hashobj_p,status2_b);
    if(status2_b == TRUE)
    {
      datapath_entry_p->dmnode_dp_output_handle_p = magic1;
      datapath_entry_p->dmnode_dp_output_handle_p = ((datapath_entry_p->dmnode_dp_output_handle_p <<32) | (index1));
      datapath_entry_p->dmnode_dp_saferef_magic   = magic1;
      datapath_entry_p->dmnode_dp_saferef_index   = index1;
    }
  }
  
  if((status_b == FALSE) ||(status1_b == FALSE) || (status2_b == FALSE))  
  {
    ret_value = DPRM_FAILURE;
    goto DPRMEXIT3;
  }
 
  switch_info_p->number_of_datapaths++;
  domain_info_p->number_of_datapaths++;
  get_number_of_ttp_tables(domain_info_p->ttp_name,&number_of_domain_tables); 
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, " number_of_domain_tables=%d", number_of_domain_tables);

  for(ii=0; ii<number_of_domain_tables; ii++)
  {  
    dp_table_info_p = (dprm_dp_table_entry_t * ) calloc(1,sizeof(dprm_dp_table_entry_t ));
    if ( dp_table_info_p == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "dp_table_info_p Memalloc failure");
      break;
    }
    dp_table_info_p->dp_table_id = ii;
    dp_table_info_p->domain_table_id = ii;
    OF_APPEND_NODE_TO_LIST(datapath_entry_p->table_list, dp_table_info_p, DPRM_DP_TABLE_ENTRY_OFFSET);
  }
  OF_LIST_INIT(datapath_entry_p->attributes,dprm_attributes_free_attribute_entry_rcu);
  /* Send datapath attached to switch notification to the registered Applications. */
  strcpy(notification_data.switch_name,switch_info_p->name);
  strcpy(datapath_entry_p->switch_name,switch_info_p->name); 
  notification_data.datapath_handle = *output_handle_p;

  OF_LIST_SCAN(switch_notifications[DPRM_SWITCH_DATAPATH_ATTACHED],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
  {
    ((dprm_switch_notifier_fn)(app_entry_p->call_back))(DPRM_SWITCH_DATAPATH_ATTACHED,
                                                        switch_handle,
                                                        notification_data,
                                                        app_entry_p->cbk_arg1,
                                                        app_entry_p->cbk_arg2);
  }  
    
  OF_LIST_SCAN(switch_notifications[DPRM_SWITCH_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
  {
    ((dprm_switch_notifier_fn)(app_entry_p->call_back))(DPRM_SWITCH_DATAPATH_ATTACHED,
                                                        switch_handle,
                                                        notification_data,
                                                        app_entry_p->cbk_arg1,
                                                        app_entry_p->cbk_arg2);
  }             

  /* Send datapath attached to domain notification to the registered Applications. */
  strcpy(notification_data1.domain_name,domain_info_p->name);
  strcpy(notification_data1.ttp_name,domain_info_p->ttp_name);
  notification_data1.datapath_handle = *output_handle_p;

  OF_LIST_SCAN(domain_notifications[DPRM_DOMAIN_DATAPATH_ATTACHED],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
  {
    ((dprm_domain_notifier_fn)(app_entry_p->call_back))(DPRM_DOMAIN_DATAPATH_ATTACHED,
                                                        domain_handle,
                                                        notification_data1,
                                                        app_entry_p->cbk_arg1,
                                                        app_entry_p->cbk_arg2);
  }

  OF_LIST_SCAN(domain_notifications[DPRM_DOMAIN_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
  {
    ((dprm_domain_notifier_fn)(app_entry_p->call_back))(DPRM_DOMAIN_DATAPATH_ATTACHED,
                                                        domain_handle,
                                                        notification_data1,
                                                        app_entry_p->cbk_arg1,
                                                        app_entry_p->cbk_arg2);
  }

  /* Send datapath added notification to the registered Applications. */

  notification_data2.dpid = datapath_info_p->dpid;

  OF_LIST_SCAN(datapath_notifications[DPRM_DATAPATH_ADDED],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
  {
    ((dprm_datapath_notifier_fn)(app_entry_p->call_back))(DPRM_DATAPATH_ADDED,
                                                          *output_handle_p,
                                                          notification_data2,
                                                          app_entry_p->cbk_arg1,
                                                          app_entry_p->cbk_arg2);
  }

  OF_LIST_SCAN(datapath_notifications[DPRM_DATAPATH_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
  {
    ((dprm_datapath_notifier_fn)(app_entry_p->call_back))(DPRM_DATAPATH_ADDED,
                                                          *output_handle_p,
                                                          notification_data2,
                                                          app_entry_p->cbk_arg1,
                                                          app_entry_p->cbk_arg2);
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_SUCCESS;
DPRMEXIT3:

  if(datapath_entry_p)
  {
    if(datapath_entry_p->dprm_dpnode_port_mempool_g)
      mempool_delete_pool(datapath_entry_p->dprm_dpnode_port_mempool_g);  
    if(datapath_entry_p->dprm_dpnode_porttable_p) 
      mchash_table_delete(datapath_entry_p->dprm_dpnode_porttable_p);
    if(status1_b == TRUE)
    {
      status1_b = FALSE;
      offset = DPNODE_SWTBL_OFFSET;
      MCHASH_DELETE_NODE_BY_REF(switch_info_p->dprm_swnode_dptable_p,index1,magic1,struct dprm_datapath_entry *,offset,status1_b);
    }
    if(status2_b == TRUE)
    {
      status2_b = FALSE;
      offset = DPNODE_DMTBL_OFFSET;
      MCHASH_DELETE_NODE_BY_REF(domain_info_p->dprm_dmnode_dptable_p,index2,magic2,struct dprm_datapath_entry *,offset,status2_b);
    }
      
    if(status_b == TRUE)
    {    
      status_b = FALSE;
      offset = DPNODE_DPTBL_OFFSET;
      MCHASH_DELETE_NODE_BY_REF(dprm_datapath_table_g,index,magic,struct dprm_datapath_entry *,offset,status_b);
    }
    mempool_release_mem_block(dprm_datapath_mempool_g,(uchar8_t*)datapath_entry_p,datapath_entry_p->heap_b);
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return ret_value;
} 
/*******************************************************************************************************/
int32_t dprm_update_datapath(struct dprm_datapath_general_info* datapath_info_p,
                               uint64_t switch_handle, uint64_t domain_handle)
{

	struct dprm_datapath_entry* datapath_entry_scan_p = NULL;
	struct dprm_switch_entry* switch_info_p = NULL;
	struct dprm_domain_entry* domain_info_p = NULL;


	uint32_t  index = 0,magic = 0,index1 = 0,magic1 = 0,index2 = 0,magic2 = 0;
	uint32_t  hashkey;

	struct    dprm_switch_notification_data    notification_data;
	struct    dprm_domain_notification_data    notification_data1;
	struct    dprm_datapath_notification_data  notification_data2;
	struct    dprm_notification_app_hook_info  *app_entry_p;

	uchar8_t  offset;
	uchar8_t* hashobj_p = NULL;
	uint8_t   status_b = FALSE,status1_b = FALSE,status2_b = FALSE,datapath_found=0;
	int32_t   ret_value;
	uchar8_t lstoffset;
	lstoffset = DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET;

	ret_value = DPRM_SUCCESS;

    	OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "dprm_update_datapath");

	if(dprm_datapath_table_g == NULL)
		return DPRM_FAILURE;


	hashkey = dprm_get_hashval_byid(datapath_info_p->dpid, dprm_no_of_datapath_buckets_g);

	CNTLR_RCU_READ_LOCK_TAKE();

	offset = DPNODE_DPTBL_OFFSET;
	MCHASH_BUCKET_SCAN(dprm_datapath_table_g,hashkey,datapath_entry_scan_p,struct dprm_datapath_entry *,offset)
	{
		if(datapath_info_p->dpid == datapath_entry_scan_p->dpid)
		{
    			OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "datapath found");
			datapath_found = 1;			
			break;
		}
		else 
			continue;	

	}


	if(datapath_found != 1)
	{
    		OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "datapath not found");
		CNTLR_RCU_READ_LOCK_RELEASE();
		return  DPRM_ERROR_INVALID_DATAPATH_HANDLE;
	}


	/* copy parameters */
	if(strlen(datapath_info_p->expected_subject_name_for_authentication))
	{
		strncpy(datapath_entry_scan_p->expected_subject_name_for_authentication,
				datapath_info_p->expected_subject_name_for_authentication,
				DPRM_MAX_X509_SUBJECT_LEN);
	}

    if(datapath_info_p->is_multi_ns_config == 1)
		datapath_entry_scan_p->multi_namespaces = datapath_info_p->multi_namespaces;

	magic1   = datapath_entry_scan_p->sw_handle_magic;
	index1   = datapath_entry_scan_p->sw_handle_index;
	status_b = FALSE;

	MCHASH_ACCESS_NODE(dprm_switch_table_g,index1,magic1,switch_info_p,status_b);

	if((status_b == TRUE) && (switch_info_p->switch_handle != switch_handle))
	{

		{
			status_b = FALSE;   
			magic1   = (uint32_t)(datapath_entry_scan_p->swnode_dp_output_handle_p>>32);
			index1   = (uint32_t)(datapath_entry_scan_p->swnode_dp_output_handle_p);
			offset = DPNODE_SWTBL_OFFSET;
    			OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "Deleting datapath from switch database");
			MCHASH_DELETE_NODE_BY_REF(switch_info_p->dprm_swnode_dptable_p,index1,magic1,struct dprm_switch_entry *,offset,status_b);
			switch_info_p->number_of_datapaths--;
		}

		magic1 = (uint32_t)(switch_handle >>32);
		index1 = (uint32_t)(switch_handle);
		datapath_entry_scan_p->sw_handle_magic = magic1;
		datapath_entry_scan_p->sw_handle_index = index1; 




		status1_b = FALSE;
		MCHASH_ACCESS_NODE(dprm_switch_table_g,index1,magic1,switch_info_p,status1_b);

		if((status1_b == TRUE))
		{
			status1_b = FALSE;

			hashkey = dprm_get_hashval_byid(datapath_entry_scan_p->dpid,dprm_no_of_swnode_datapath_buckets_g);
			OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "bucket = %d",hashkey);

			hashobj_p = ((uchar8_t *)datapath_entry_scan_p) + DPNODE_SWTBL_OFFSET;
    			OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "Adding new  to switch database");
			MCHASH_APPEND_NODE(switch_info_p->dprm_swnode_dptable_p,hashkey,datapath_entry_scan_p,index,magic,hashobj_p,status1_b);
			if(status1_b == TRUE)
			{
				datapath_entry_scan_p->swnode_dp_output_handle_p = magic;
				datapath_entry_scan_p->swnode_dp_output_handle_p = ((datapath_entry_scan_p->swnode_dp_output_handle_p <<32) | (index));
				datapath_entry_scan_p->swnode_dp_saferef_magic   = magic;
				datapath_entry_scan_p->swnode_dp_saferef_index   = index;
			} 
			switch_info_p->number_of_datapaths++;
		}
	}



	magic1   = datapath_entry_scan_p->dm_handle_magic;
	index1   = datapath_entry_scan_p->dm_handle_index;
	status_b = FALSE;
	MCHASH_ACCESS_NODE(dprm_domain_table_g,index1,magic1,domain_info_p,status_b);
	if((status_b == TRUE)&&(domain_info_p->domain_handle!=domain_handle))
	{
		status_b = FALSE;
		magic1   = (uint32_t)(datapath_entry_scan_p->dmnode_dp_output_handle_p >>32);
		index1   = (uint32_t)(datapath_entry_scan_p->dmnode_dp_output_handle_p);

		offset = DPNODE_DMTBL_OFFSET;
		OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "Deleting datapath from domain database");
		MCHASH_DELETE_NODE_BY_REF(domain_info_p->dprm_dmnode_dptable_p,index1,magic1,struct dprm_domain_entry *,offset,status_b);
		domain_info_p->number_of_datapaths--;


		magic2 = (uint32_t)(domain_handle >>32);
		index2 = (uint32_t)(domain_handle);
		datapath_entry_scan_p->dm_handle_magic = magic2;
		datapath_entry_scan_p->dm_handle_index = index2;
		MCHASH_ACCESS_NODE(dprm_domain_table_g,index2,magic2,domain_info_p,status2_b);
		if(status2_b == TRUE)
		{
			hashkey = dprm_get_hashval_byid(datapath_info_p->dpid,dprm_no_of_dmnode_datapath_buckets_g);
			hashobj_p = ((uchar8_t *)datapath_entry_scan_p) + DPNODE_DMTBL_OFFSET;
    			OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "Adding new  to domain database");
			MCHASH_APPEND_NODE(domain_info_p->dprm_dmnode_dptable_p,hashkey,datapath_entry_scan_p,index1,magic1,hashobj_p,status2_b);
			if(status2_b == TRUE)
			{
				datapath_entry_scan_p->dmnode_dp_output_handle_p = magic1;
				datapath_entry_scan_p->dmnode_dp_output_handle_p = ((datapath_entry_scan_p->dmnode_dp_output_handle_p <<32) | (index1));
				datapath_entry_scan_p->dmnode_dp_saferef_magic   = magic1;
				datapath_entry_scan_p->dmnode_dp_saferef_index   = index1;
			}
			domain_info_p->number_of_datapaths++;
		}  
	}

	/* Send datapath attached to switch notification to the registered Applications. */
	strcpy(notification_data.switch_name,switch_info_p->name);
	notification_data.datapath_handle = datapath_entry_scan_p->datapath_handle;

	OF_LIST_SCAN(switch_notifications[DPRM_SWITCH_DATAPATH_ATTACHED],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
	{
		((dprm_switch_notifier_fn)(app_entry_p->call_back))(DPRM_SWITCH_DATAPATH_ATTACHED,
		switch_handle,
		notification_data,
		app_entry_p->cbk_arg1,
		app_entry_p->cbk_arg2);
	}  

	OF_LIST_SCAN(switch_notifications[DPRM_SWITCH_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
	{
		((dprm_switch_notifier_fn)(app_entry_p->call_back))(DPRM_SWITCH_DATAPATH_ATTACHED,
		switch_handle,
		notification_data,
		app_entry_p->cbk_arg1,
		app_entry_p->cbk_arg2);
	}             

	/* Send datapath attached to domain notification to the registered Applications. */
	strcpy(notification_data1.domain_name,domain_info_p->name);
	notification_data1.datapath_handle = datapath_entry_scan_p->datapath_handle;

	OF_LIST_SCAN(domain_notifications[DPRM_DOMAIN_DATAPATH_ATTACHED],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
	{
		((dprm_domain_notifier_fn)(app_entry_p->call_back))(DPRM_DOMAIN_DATAPATH_ATTACHED,
		domain_handle,
		notification_data1,
		app_entry_p->cbk_arg1,
		app_entry_p->cbk_arg2);
	}

	OF_LIST_SCAN(domain_notifications[DPRM_DOMAIN_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
	{
		((dprm_domain_notifier_fn)(app_entry_p->call_back))(DPRM_DOMAIN_DATAPATH_ATTACHED,
		domain_handle,
		notification_data1,
		app_entry_p->cbk_arg1,
		app_entry_p->cbk_arg2);
	}

	/* Send datapath added notification to the registered Applications. */

	notification_data2.dpid = datapath_info_p->dpid;

	OF_LIST_SCAN(datapath_notifications[DPRM_DATAPATH_ADDED],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
	{
		((dprm_datapath_notifier_fn)(app_entry_p->call_back))(DPRM_DATAPATH_ADDED,
		datapath_entry_scan_p->datapath_handle,
		notification_data2,
		app_entry_p->cbk_arg1,
		app_entry_p->cbk_arg2);
	}

	OF_LIST_SCAN(datapath_notifications[DPRM_DATAPATH_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
	{
		((dprm_datapath_notifier_fn)(app_entry_p->call_back))(DPRM_DATAPATH_ADDED,
		datapath_entry_scan_p->datapath_handle,
		notification_data2,
		app_entry_p->cbk_arg1,
		app_entry_p->cbk_arg2);
	}
	CNTLR_RCU_READ_LOCK_RELEASE();
	return DPRM_SUCCESS;
}
/**************************************************************************************************/
int32_t dprm_get_datapath_multinamespaces(uint64_t dpid, uint8_t *multinamespaces)
{
	uint64_t datapath_handle;
	struct   dprm_datapath_entry *datapath_info_p=NULL;
	int32_t  ret_value=DPRM_FAILURE;

	OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "Entered ");

	ret_value = dprm_get_datapath_handle(dpid,&datapath_handle);
	if(ret_value == DPRM_SUCCESS)
	{  
		CNTLR_RCU_READ_LOCK_TAKE();
		ret_value = get_datapath_byhandle(datapath_handle,&datapath_info_p);

		if(ret_value == DPRM_SUCCESS)
		{
			*multinamespaces=datapath_info_p->multi_namespaces;
			OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "datapath_info_p->multi_namespaces:%d",datapath_info_p->multi_namespaces);
		} 
	        CNTLR_RCU_READ_LOCK_RELEASE();	
	}
	return ret_value;
}
/******************************************************************************************************/
int32_t dprm_get_datapath_namespace_count(uint64_t dpid, uint32_t *namespacescount)
{
	uint64_t datapath_handle;
	struct   dprm_datapath_entry *datapath_info_p=NULL;
	struct   dprm_resource_attribute_entry *attribute_entry_scan_p=NULL;
	int32_t  ret_value=DPRM_FAILURE;
	uchar8_t lstoffset = DPRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET;

	OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "Entered ");
	ret_value = dprm_get_datapath_handle(dpid,&datapath_handle);
	if(ret_value == DPRM_SUCCESS)
	{  
		CNTLR_RCU_READ_LOCK_TAKE();
		ret_value = get_datapath_byhandle(datapath_handle,&datapath_info_p);

		if((ret_value == DPRM_SUCCESS) )
		{

			if((datapath_info_p->number_of_attributes)>0)
			{

				OF_LIST_SCAN(datapath_info_p->attributes, attribute_entry_scan_p, struct dprm_resource_attribute_entry*, lstoffset )
				{
					if(!strcmp(attribute_entry_scan_p->name, "namespace"))
					{
						*namespacescount = attribute_entry_scan_p->number_of_attribute_values;
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "attribute name found and namespace count:%d ",*namespacescount);
						ret_value = DPRM_SUCCESS;
						break;
					}
				}
			}
			else
			{
				OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "attributes are not present in the database");
				*namespacescount=0;
			}
		} 
	        CNTLR_RCU_READ_LOCK_RELEASE();	
	}
	return ret_value;
}

/******************************************************************************************************/
int32_t dprm_get_datapath_port_namespace_count(uint64_t dpid, char *portname, uint32_t *namespacescount)
{
	uint64_t datapath_handle,port_handle;
   	struct   dprm_port_entry *port_entry_p;
	struct   dprm_resource_attribute_entry *attribute_entry_scan_p=NULL;
	int32_t  ret_value=DPRM_FAILURE;
	uchar8_t lstoffset = DPRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET;

	OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "Entered ");

	CNTLR_RCU_READ_LOCK_TAKE();

	ret_value = dprm_get_datapath_handle(dpid, &datapath_handle);
	if (ret_value != DPRM_SUCCESS)
	{
		OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "dprm_get_datapath_handle failed!");
	        CNTLR_RCU_READ_LOCK_RELEASE();	
		return DPRM_FAILURE;
	}

	ret_value=dprm_get_port_handle(datapath_handle, portname, &port_handle);
	if (ret_value != DPRM_SUCCESS)
	{
		OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "dprm_get_datapath_port_handle failed!");
	        CNTLR_RCU_READ_LOCK_RELEASE();	
		return DPRM_FAILURE;
	}

	ret_value = get_datapath_port_byhandle(datapath_handle,port_handle,&port_entry_p);
	if(ret_value != DPRM_SUCCESS)
	{
		CNTLR_RCU_READ_LOCK_RELEASE();
		return DPRM_FAILURE;
	}



	if((port_entry_p->number_of_attributes)>0)
	{
		OF_LIST_SCAN(port_entry_p->attributes, attribute_entry_scan_p, 
		struct dprm_resource_attribute_entry*, lstoffset )
			{
				if(!strcmp(attribute_entry_scan_p->name, "namespace"))
				{
					*namespacescount = attribute_entry_scan_p->number_of_attribute_values;
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "attribute name found and namespace count:%d ",*namespacescount);
					ret_value = DPRM_SUCCESS;
					break;
				}
			}
	}
	else
	{
			OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "attributes are not present in the database");
			*namespacescount=0;
	}

	
	CNTLR_RCU_READ_LOCK_RELEASE();	
	return ret_value;
}



/*******************************************************************************************************/
int32_t dprm_get_datapath_handle(uint64_t dpid, uint64_t* datapath_handle_p)
{
  uint32_t hashkey;
  uchar8_t offset;

  struct dprm_datapath_entry* datapath_entry_p = NULL;

  if(dprm_datapath_table_g == NULL)
    return DPRM_FAILURE;

  hashkey = dprm_get_hashval_byid(dpid,dprm_no_of_datapath_buckets_g);

  CNTLR_RCU_READ_LOCK_TAKE();
  offset = DPNODE_DPTBL_OFFSET;
  MCHASH_BUCKET_SCAN(dprm_datapath_table_g,hashkey,datapath_entry_p,struct dprm_datapath_entry *,offset)
  {
    if(datapath_entry_p->dpid != dpid)
      continue;
    
    *datapath_handle_p = datapath_entry_p->magic;
    *datapath_handle_p = ((*datapath_handle_p <<32) | (datapath_entry_p->index));
   
    CNTLR_RCU_READ_LOCK_RELEASE();  
    return DPRM_SUCCESS;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_ERROR_INVALID_DATAPATH_HANDLE;
}
/*******************************************************************************************************/
int32_t dprm_get_exact_datapath(uint64_t  handle,
                                uint64_t* sw_handle_p,
                                uint64_t* dm_handle_p, 
                                struct dprm_datapath_general_info* config_info_p,
                                struct dprm_datapath_runtime_info* runtime_info_p)
{
  uint32_t index,magic;
  struct   dprm_datapath_entry* datapath_info_p;
  uint8_t  status_b = FALSE;
  struct dprm_domain_entry  *domain_info_p;
  struct dprm_switch_entry  *switch_info_p;


  magic = (uint32_t)(handle >>32);
  index = (uint32_t)handle;

  CNTLR_RCU_READ_LOCK_TAKE();

  MCHASH_ACCESS_NODE(dprm_datapath_table_g,index,magic,datapath_info_p,status_b);
  if(status_b == TRUE)
  {
    config_info_p->dpid = datapath_info_p->dpid;
    
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"datapath_info_p->dpid %llx",datapath_info_p->dpid);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"config_info_p->dpid %llx",config_info_p->dpid);

    strncpy(config_info_p->expected_subject_name_for_authentication,
            datapath_info_p->expected_subject_name_for_authentication,
            DPRM_MAX_X509_SUBJECT_LEN);

    strncpy(config_info_p->datapath_name,
              datapath_info_p->datapath_name,
              DPRM_MAX_DATAPATH_NAME_LEN);

    runtime_info_p->number_of_ports      = datapath_info_p->number_of_ports;
    runtime_info_p->number_of_attributes = datapath_info_p->number_of_attributes;

    *sw_handle_p = datapath_info_p->sw_handle_magic;
    *sw_handle_p = ((*sw_handle_p << 32) | (datapath_info_p->sw_handle_index));

    *dm_handle_p = datapath_info_p->dm_handle_magic;
    *dm_handle_p = ((*dm_handle_p <<32) | (datapath_info_p->dm_handle_index));   

    if (get_domain_byhandle(*dm_handle_p, &domain_info_p) != DPRM_SUCCESS)
    {
      CNTLR_RCU_READ_LOCK_RELEASE();
      return DPRM_FAILURE;
    }

    if(get_switch_byhandle(*sw_handle_p, &switch_info_p) != DPRM_SUCCESS)
    {
       CNTLR_RCU_READ_LOCK_RELEASE();
       return DPRM_FAILURE;
    }

    strcpy(config_info_p->domain_name, domain_info_p->name);
    strcpy(config_info_p->switch_name, switch_info_p->name);

    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_SUCCESS;
  } 
  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_ERROR_INVALID_DATAPATH_HANDLE;
}
/*******************************************************************************************************/
int32_t get_datapath_byhandle(uint64_t datapath_handle,struct dprm_datapath_entry** datapath_info_p_p)
{
  /* Caller shall take RCU locks. */
  uint32_t index,magic;
  uint8_t status_b;

  magic = (uint32_t)(datapath_handle >>32);
  index = (uint32_t)datapath_handle;

  MCHASH_ACCESS_NODE(dprm_datapath_table_g,index,magic,*datapath_info_p_p,status_b);
  if(status_b == TRUE)
    return DPRM_SUCCESS;
  else
    return DPRM_ERROR_INVALID_DATAPATH_HANDLE;
}
/*****************************************************************************************************/
int32_t get_swname_by_dphandle(uint64_t datapath_handle,char** sw_name_p_p)
{
  int32_t retval;
  struct dprm_datapath_entry *datapath_info_p;

  retval = get_datapath_byhandle(datapath_handle,&datapath_info_p);

  if(retval != DPRM_SUCCESS)
    return DPRM_ERROR_INVALID_DATAPATH_HANDLE;

  *sw_name_p_p = datapath_info_p->switch_name;
  return DPRM_SUCCESS;
}
/***************************************************************************************************/
int32_t dprm_get_datapath_id_byhandle(uint64_t datapath_handle,uint64_t *dp_id)
{
  /* Caller shall take RCU locks. */
  uint32_t index,magic;
  uint8_t status_b;
  struct dprm_datapath_entry* datapath_info_p;

  magic = (uint32_t)(datapath_handle >>32);
  index = (uint32_t)datapath_handle;

  MCHASH_ACCESS_NODE(dprm_datapath_table_g,index,magic,datapath_info_p,status_b);
  if(status_b == TRUE)
  {
    *dp_id= datapath_info_p->dpid;
    return DPRM_SUCCESS;
  } 
  else
    return DPRM_ERROR_INVALID_DATAPATH_HANDLE;
}
/****************************************************************************************************/
int32_t dprm_get_datapath_controller_role(uint64_t datapath_handle, uint8_t *controller_role)
{
	int32_t iRetVal;
	struct dprm_datapath_entry   *datapath=NULL;
	iRetVal = get_datapath_byhandle(datapath_handle, &datapath);
	if(iRetVal  != DPRM_SUCCESS)
	{
		OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Error in getting datapath,Err=%d", iRetVal);
		return DPRM_FAILURE;
	}
	*controller_role = datapath->controller_role;
	return DPRM_SUCCESS;
}

/******************************************************************************************************/
int32_t dprm_unregister_datapath(uint64_t handle)
{
  uint32_t   index,magic,index1,magic1;
  struct     dprm_datapath_entry* datapath_info_p; 
  uint8_t    status_b = FALSE;
  uchar8_t   offset;

  struct     dprm_switch_entry* switch_info_p;
  struct     dprm_domain_entry* domain_info_p;

  struct     dprm_switch_notification_data    notification_data;
  struct     dprm_domain_notification_data    notification_data1;
  struct     dprm_datapath_notification_data  notification_data2;
  struct     dprm_notification_app_hook_info  *app_entry_p;
  uint64_t   sw_handle,domain_handle;
  uchar8_t lstoffset;
  lstoffset = DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET;

  if(dprm_datapath_table_g == NULL)
    return DPRM_FAILURE;

  magic = (uint32_t)(handle >>32);
  index = (uint32_t)handle;

  /* TBD delete datapath from Switch and Domain Hash Table and unregister from controller.Delete the ports and the datapath attributes */

  CNTLR_RCU_READ_LOCK_TAKE();

  MCHASH_ACCESS_NODE(dprm_datapath_table_g,index,magic,datapath_info_p,status_b);
  if(status_b == TRUE)
  {
    if((datapath_info_p->number_of_ports)||(datapath_info_p->number_of_attributes)) 
    {
      CNTLR_RCU_READ_LOCK_RELEASE();
      dprm_debug("%s:%d Datapath not empty to be deleted.\r\n",__FUNCTION__,__LINE__);
      return DPRM_ERROR_RESOURCE_NOTEMPTY;
    }  
  }
  else
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_ERROR_INVALID_DATAPATH_HANDLE;
  }

  magic1 = datapath_info_p->sw_handle_magic;    
  index1 = datapath_info_p->sw_handle_index;   
  status_b = FALSE;
  MCHASH_ACCESS_NODE(dprm_switch_table_g,index1,magic1,switch_info_p,status_b);
  if(status_b == TRUE)
  {
    status_b = FALSE;   
    magic1   = (uint32_t)(datapath_info_p->swnode_dp_output_handle_p>>32);
    index1   = (uint32_t)(datapath_info_p->swnode_dp_output_handle_p);

    offset = DPNODE_SWTBL_OFFSET;
    MCHASH_DELETE_NODE_BY_REF(switch_info_p->dprm_swnode_dptable_p,index1,magic1,struct dprm_switch_entry *,offset,status_b);
    switch_info_p->number_of_datapaths--;
  }
  else
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_ERROR_INVALID_SWITCH_HANDLE;
  }

  magic1   = datapath_info_p->dm_handle_magic;
  index1   = datapath_info_p->dm_handle_index;
  status_b = FALSE;
  MCHASH_ACCESS_NODE(dprm_domain_table_g,index1,magic1,domain_info_p,status_b);
  if(status_b == TRUE)
  {
    status_b = FALSE;
    magic1   = (uint32_t)(datapath_info_p->dmnode_dp_output_handle_p >>32);
    index1   = (uint32_t)(datapath_info_p->dmnode_dp_output_handle_p);

    offset = DPNODE_DMTBL_OFFSET;
    MCHASH_DELETE_NODE_BY_REF(domain_info_p->dprm_dmnode_dptable_p,index1,magic1,struct dprm_domain_entry *,offset,status_b);
    domain_info_p->number_of_datapaths--;
  }
  else
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_ERROR_INVALID_DOMAIN_HANDLE;
  }
  
  /* Send datapath detach from sw notification to the registered Applications. */
  strcpy(notification_data.switch_name,switch_info_p->name);
    
  sw_handle = datapath_info_p->sw_handle_magic;
  sw_handle = ((sw_handle <<32) | (datapath_info_p->sw_handle_index));

     
  OF_LIST_SCAN(switch_notifications[DPRM_SWITCH_DATAPATH_DETACH],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
  {
    ((dprm_switch_notifier_fn)(app_entry_p->call_back))(DPRM_SWITCH_DATAPATH_DETACH,
                                                        sw_handle,
                                                        notification_data,
                                                        app_entry_p->cbk_arg1,
                                                        app_entry_p->cbk_arg2);
  }           

  OF_LIST_SCAN(switch_notifications[DPRM_SWITCH_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
  {
    ((dprm_switch_notifier_fn)(app_entry_p->call_back))(DPRM_SWITCH_DATAPATH_DETACH,
                                                        sw_handle,
                                                        notification_data,
                                                        app_entry_p->cbk_arg1,
                                                        app_entry_p->cbk_arg2);
  } 

  /* Send datapath detach from domain notification to the registered Applications. */
  strcpy(notification_data1.domain_name,domain_info_p->name);

  domain_handle = datapath_info_p->dm_handle_magic;
  domain_handle = ((domain_handle <<32) | (datapath_info_p->dm_handle_index));

  notification_data1.datapath_handle=handle;
  OF_LIST_SCAN(domain_notifications[DPRM_DOMAIN_DATAPATH_DETACH],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
  {
    ((dprm_domain_notifier_fn)(app_entry_p->call_back))(DPRM_DOMAIN_DATAPATH_DETACH,
                                                        domain_handle,
                                                        notification_data1,
                                                        app_entry_p->cbk_arg1,
                                                        app_entry_p->cbk_arg2);
  }

  OF_LIST_SCAN(domain_notifications[DPRM_DOMAIN_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
  {
    ((dprm_domain_notifier_fn)(app_entry_p->call_back))(DPRM_DOMAIN_DATAPATH_DETACH,
                                                        domain_handle,
                                                        notification_data1,
                                                        app_entry_p->cbk_arg1,
                                                        app_entry_p->cbk_arg2);
  }
  
  /* Send datapath deleted notification to the registered Applications. */

  notification_data2.dpid = datapath_info_p->dpid;

  OF_LIST_SCAN(datapath_notifications[DPRM_DATAPATH_DELETE],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
  {
    ((dprm_datapath_notifier_fn)(app_entry_p->call_back))(DPRM_DATAPATH_DELETE,
                                                          handle,
                                                          notification_data2,
                                                          app_entry_p->cbk_arg1,
                                                          app_entry_p->cbk_arg2);
  }

  OF_LIST_SCAN(datapath_notifications[DPRM_DATAPATH_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
  {
    ((dprm_datapath_notifier_fn)(app_entry_p->call_back))(DPRM_DATAPATH_DELETE,
                                                          handle,
                                                          notification_data2,
                                                          app_entry_p->cbk_arg1,
                                                          app_entry_p->cbk_arg2);
  }

  dprm_free_datapath_channels(datapath_info_p);
  status_b = FALSE;
  offset = DPNODE_DPTBL_OFFSET;
  MCHASH_DELETE_NODE_BY_REF(dprm_datapath_table_g,index,magic,struct dprm_datapath_entry *,offset,status_b);
    
  if(status_b == TRUE)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_SUCCESS;
  }
  else
  {  
    CNTLR_RCU_READ_LOCK_RELEASE();
     return DPRM_FAILURE;
  }  
}
/********************************************************************************************************/
void dprm_free_datapathentry_rcu(struct rcu_head *datapath_entry_p)
{
  struct   dprm_datapath_entry *datapath_info_p;
  int32_t  ret_value;
  uchar8_t offset;

  if(datapath_entry_p)
  {
    offset = DPNODE_DPTBL_OFFSET; 
    datapath_info_p = (struct dprm_datapath_entry *)(((char *)datapath_entry_p) - (2*(sizeof(struct mchash_dll_node *)))-offset);
    //cntrlr_datapath_resource_deinit(uint64_t dpid); /*TBD Rajesj */
    ret_value = mempool_release_mem_block(dprm_datapath_mempool_g,(uchar8_t*)datapath_info_p,datapath_info_p->heap_b);
    if(ret_value != MEMPOOL_SUCCESS)
      dprm_debug("%s:%d Failed to release datapath node  memory block.\r\n",__FUNCTION__,__LINE__);
    else
      dprm_debug("%s:%d Deleted succesfully the datapath node from database.\r\n",__FUNCTION__,__LINE__);
  }
  else
    dprm_debug("%s:%d NULL passed for deletion.\r\n",__FUNCTION__,__LINE__);
}
/*******************************************************************************************************/
void dprm_free_swnode_dptbl_entry_rcu(struct rcu_head *datapath_entry_p)
{
  dprm_debug("%s:%d About to delete a datapath node from hash table in a switch node\r\n",__FUNCTION__,__LINE__);
}
void dprm_free_dmnode_dptbl_entry_rcu(struct rcu_head *datapath_entry_p)
{
  dprm_debug("%s:%d About to delete a datapath node from hash table in a domain node\r\n",__FUNCTION__,__LINE__);
} 
void  dprm_free_dpnode_porttbl_entry_rcu(struct rcu_head *port_entry_p)
{
  struct dprm_port_entry *port_info_p;
  int32_t ret_value;

  if(port_entry_p)
  {
    port_info_p = (struct dprm_port_entry *)(((char *)port_entry_p) - (2*(sizeof(struct mchash_dll_node *))));
    ret_value = mempool_release_mem_block(port_info_p->datapath_entry_p->dprm_dpnode_port_mempool_g,(uchar8_t*)port_info_p,port_info_p->heap_b);
    if(ret_value != MEMPOOL_SUCCESS)
      dprm_debug("%s:%d Failed to release port node memory block.\r\n",__FUNCTION__,__LINE__);
    else
      dprm_debug("%s:%d Deleted succesfully the port node from database.\r\n",__FUNCTION__,__LINE__);
  }
  else
    dprm_debug("%s:%d NULL passed for deletion.\r\n",__FUNCTION__,__LINE__);
}  
/*******************************************************************************************************/
int32_t dprm_get_first_datapath(struct dprm_datapath_general_info *config_info_p,
                                struct dprm_datapath_runtime_info *runtime_info_p,
                                uint64_t *datapath_handle_p)
{
  struct   dprm_datapath_entry* datapath_info_p = NULL;
  uint32_t hashkey;
  int32_t  iretval;
  uint64_t domain_handle;
  uint64_t switch_handle;
  struct dprm_domain_entry  *domain_info_p;
  struct dprm_switch_entry  *switch_info_p;

  uchar8_t offset;

  CNTLR_RCU_READ_LOCK_TAKE();
  offset = DPNODE_DPTBL_OFFSET;

  if(dprm_datapath_table_g == NULL)
  {
    CNTLR_RCU_READ_LOCK_RELEASE(); 
    return DPRM_FAILURE; 
  }
 
  MCHASH_TABLE_SCAN(dprm_datapath_table_g,hashkey)
  {
    MCHASH_BUCKET_SCAN(dprm_datapath_table_g,hashkey,datapath_info_p,struct dprm_datapath_entry*,offset)
    {
      config_info_p->dpid = datapath_info_p->dpid;
      strncpy(config_info_p->expected_subject_name_for_authentication,
              datapath_info_p->expected_subject_name_for_authentication,
              DPRM_MAX_X509_SUBJECT_LEN);

      strncpy(config_info_p->datapath_name,
              datapath_info_p->datapath_name,
              DPRM_MAX_DATAPATH_NAME_LEN);


      runtime_info_p->number_of_ports      = datapath_info_p->number_of_ports;
      runtime_info_p->number_of_attributes = datapath_info_p->number_of_attributes;

      *datapath_handle_p = datapath_info_p->magic;
      *datapath_handle_p = ((*datapath_handle_p <<32) | (datapath_info_p->index));
  
       switch_handle = datapath_info_p->sw_handle_magic;
       switch_handle = ((switch_handle <<32) | (datapath_info_p->sw_handle_index));

       domain_handle = datapath_info_p->dm_handle_magic;
       domain_handle = ((domain_handle <<32) | (datapath_info_p->dm_handle_index));

       iretval = get_domain_byhandle(domain_handle, &domain_info_p);
       if(iretval != DPRM_SUCCESS)
       {
         CNTLR_RCU_READ_LOCK_RELEASE();
         return DPRM_FAILURE;
       }

       iretval = get_switch_byhandle(switch_handle, &switch_info_p);
       if(iretval != DPRM_SUCCESS)
       {
         CNTLR_RCU_READ_LOCK_RELEASE();
         return DPRM_FAILURE;
       }

       strcpy(config_info_p->domain_name, domain_info_p->name);
       strcpy(config_info_p->switch_name, switch_info_p->name);
       config_info_p->multi_namespaces = datapath_info_p->multi_namespaces;
	
       CNTLR_RCU_READ_LOCK_RELEASE();
       return DPRM_SUCCESS;
    }
  }     
  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_ERROR_NO_MORE_ENTRIES;
}
/*******************************************************************************************************/
int32_t dprm_get_next_datapath(struct dprm_datapath_general_info *config_info_p,
                               struct dprm_datapath_runtime_info *runtime_info_p,
                               uint64_t *datapath_handle_p)
{
  struct   dprm_datapath_entry* datapath_info_p = NULL;
  uint32_t hashkey;
  uint8_t  current_entry_found_b;
  uchar8_t offset;
  int32_t  iretval;
  uint64_t domain_handle;
  uint64_t switch_handle;
  struct dprm_domain_entry  *domain_info_p;
  struct dprm_switch_entry  *switch_info_p;

  CNTLR_RCU_READ_LOCK_TAKE();
  offset = DPNODE_DPTBL_OFFSET;
  current_entry_found_b = FALSE;

  MCHASH_TABLE_SCAN(dprm_datapath_table_g,hashkey)
  {
    MCHASH_BUCKET_SCAN(dprm_datapath_table_g,hashkey,datapath_info_p,struct dprm_datapath_entry* ,offset)
    {
      if(current_entry_found_b == FALSE)
      {
        if(*datapath_handle_p != datapath_info_p->datapath_handle)
          continue;
        else
        {
          current_entry_found_b = TRUE;
          continue;
        }
        /*Skip the First matching Switch and Get the next Switch */
      }
      config_info_p->dpid = datapath_info_p->dpid;
      strncpy(config_info_p->expected_subject_name_for_authentication,
              datapath_info_p->expected_subject_name_for_authentication,
              DPRM_MAX_X509_SUBJECT_LEN);

      strncpy(config_info_p->datapath_name,
              datapath_info_p->datapath_name,
              DPRM_MAX_DATAPATH_NAME_LEN);



      runtime_info_p->number_of_ports      = datapath_info_p->number_of_ports;
      runtime_info_p->number_of_attributes = datapath_info_p->number_of_attributes;

      *datapath_handle_p = datapath_info_p->magic;
      *datapath_handle_p = ((*datapath_handle_p <<32) | (datapath_info_p->index));
      
      switch_handle = datapath_info_p->sw_handle_magic;
      switch_handle = ((switch_handle <<32) | (datapath_info_p->sw_handle_index));

      domain_handle = datapath_info_p->dm_handle_magic;
      domain_handle = ((domain_handle <<32) | (datapath_info_p->dm_handle_index));

      iretval = get_domain_byhandle(domain_handle, &domain_info_p);
      if(iretval != DPRM_SUCCESS)
      {
         CNTLR_RCU_READ_LOCK_RELEASE();
         return DPRM_FAILURE;
      }

      iretval = get_switch_byhandle(switch_handle, &switch_info_p);
      if(iretval != DPRM_SUCCESS)
      {
        CNTLR_RCU_READ_LOCK_RELEASE();
        return DPRM_FAILURE;
      }

      strcpy(config_info_p->domain_name, domain_info_p->name);
      strcpy(config_info_p->switch_name, switch_info_p->name);
      config_info_p->multi_namespaces = datapath_info_p->multi_namespaces;
      CNTLR_RCU_READ_LOCK_RELEASE();
      return DPRM_SUCCESS;
    }
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  if(current_entry_found_b == TRUE)
    return DPRM_ERROR_NO_MORE_ENTRIES;
  else
    return DPRM_ERROR_INVALID_DATAPATH_HANDLE;
}

int32_t of_update_switch_info(uint64_t datapath_handle,  struct ofi_switch_info *switch_info)
{
  struct dprm_datapath_entry* datapath_info_p = NULL;
  uint32_t  index,magic;
  uint8_t   status_b = FALSE;
  int32_t   retvalue = OF_FAILURE;

  magic = (uint32_t)(datapath_handle >>32);
  index = (uint32_t)datapath_handle;

  CNTLR_RCU_READ_LOCK_TAKE();

  MCHASH_ACCESS_NODE(dprm_datapath_table_g,index,magic,datapath_info_p,status_b);
  if(status_b == FALSE)
  {
    retvalue = DPRM_ERROR_INVALID_DATAPATH_HANDLE;
    CNTLR_RCU_READ_LOCK_RELEASE();
    return retvalue;
  }

  strcpy(datapath_info_p->mfr_desc, switch_info->mfr_desc);
  strcpy(datapath_info_p->hw_desc,  switch_info->hw_desc);
  strcpy(datapath_info_p->sw_desc,  switch_info->sw_desc);
  strcpy(datapath_info_p->serial_num, switch_info->serial_num);
  strcpy(datapath_info_p->dp_desc, switch_info->dp_desc);


  CNTLR_RCU_READ_LOCK_RELEASE();
  return OF_SUCCESS;
}

int32_t of_get_switch_info(uint64_t datapath_handle,  struct ofi_switch_info *switch_info)
{
  struct dprm_datapath_entry* datapath_info_p = NULL;
  uint32_t  index,magic;
  uint8_t   status_b = FALSE;
  int32_t   retvalue = OF_FAILURE;

  magic = (uint32_t)(datapath_handle >>32);
  index = (uint32_t)datapath_handle;

  CNTLR_RCU_READ_LOCK_TAKE();

  MCHASH_ACCESS_NODE(dprm_datapath_table_g,index,magic,datapath_info_p,status_b);
  if(status_b == FALSE)
  {
    retvalue = DPRM_ERROR_INVALID_DATAPATH_HANDLE;
    CNTLR_RCU_READ_LOCK_RELEASE();
    return retvalue;
  }

  strcpy(switch_info->mfr_desc, datapath_info_p->mfr_desc);
  strcpy(switch_info->hw_desc,  datapath_info_p->hw_desc);
  strcpy(switch_info->sw_desc,  datapath_info_p->sw_desc);
  strcpy(switch_info->serial_num, datapath_info_p->serial_num);
  strcpy(switch_info->dp_desc, datapath_info_p->dp_desc);

  CNTLR_RCU_READ_LOCK_RELEASE();
  return OF_SUCCESS;
}

/******************************************************************************************************/
int32_t dprm_add_port_to_datapath(uint64_t datapath_handle,
                                  struct   dprm_port_info *dprm_port_p,
                                  uint64_t *port_handle_p)
{
  struct    dprm_datapath_entry* datapath_info_p = NULL;
  struct    dprm_port_entry* port_entry_p = NULL;
  uint32_t  index,magic;
  uchar8_t* hashobj_p = NULL;
  uint8_t   heap_b,status_b = FALSE;
  int32_t   ret_value;
  uint32_t  hashkey,offset;
  char      *name;
  uchar8_t lstoffset;
  struct    dprm_datapath_notification_data  notification_data;
  struct    dprm_notification_app_hook_info  *app_entry_p;
  struct    dprm_namespace_notification_data namespace_notification_data;

  lstoffset = DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET;


  name = dprm_port_p->port_name;

#if 0
  if(strlen(name) < 8)
  {
    ret_value = DPRM_ERROR_NAME_LENGTH_NOT_ENOUGH;
    goto DPRMEXIT_PORT;
  }

#endif
  magic = (uint32_t)(datapath_handle >>32);
  index = (uint32_t)datapath_handle;

  CNTLR_RCU_READ_LOCK_TAKE();

  MCHASH_ACCESS_NODE(dprm_datapath_table_g,index,magic,datapath_info_p,status_b);
  if(status_b == FALSE)
  {
    ret_value = DPRM_ERROR_INVALID_DATAPATH_HANDLE;
    goto DPRMEXIT_PORT;
  }
  
  /* Check for duplicate port with port name */ 
  hashkey = dprm_get_hashval_byname(name,dprm_no_of_dpnode_port_buckets_g);
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "bucket = %d\n",hashkey);

  offset = DPNODE_DPTBL_OFFSET;
  MCHASH_BUCKET_SCAN(datapath_info_p->dprm_dpnode_porttable_p,hashkey,port_entry_p,struct dprm_port_entry *,offset)
  {
    if(strcmp(name,port_entry_p->port_name))
      continue;
    ret_value = DPRM_ERROR_DUPLICATE_RESOURCE;
    goto DPRMEXIT_PORT;
  }
  
  ret_value = mempool_get_mem_block(datapath_info_p->dprm_dpnode_port_mempool_g,(uchar8_t**)&port_entry_p,&heap_b);
  if(ret_value != MEMPOOL_SUCCESS)
    goto DPRMEXIT_PORT;

  /* Add port entry to the port hash table */
  strcpy(port_entry_p->port_name,name);
  port_entry_p->port_id = dprm_port_p->port_id; 

  hashobj_p = ((uchar8_t *)port_entry_p) + PORTNODE_PORTTBL_OFFSET;
  MCHASH_APPEND_NODE(datapath_info_p->dprm_dpnode_porttable_p,hashkey,port_entry_p,index,magic,hashobj_p,status_b);
  if(FALSE == status_b)
  {
    ret_value = DPRM_FAILURE;
    goto DPRMEXIT_PORT;
  }
  *port_handle_p = magic;
  *port_handle_p = ((*port_handle_p <<32) | (index));

  port_entry_p->port_handle = *port_handle_p;

  port_entry_p->magic = magic;
  port_entry_p->index = index;
  
  port_entry_p->datapath_entry_p = datapath_info_p; 

  datapath_info_p->number_of_ports++;

  OF_LIST_INIT(port_entry_p->attributes,dprm_attributes_free_attribute_entry_rcu);
  /* Send port attached to domain notification to the registered Applications. */

  notification_data.dpid        = datapath_info_p->dpid;
  strcpy(notification_data.port_name,name);
  notification_data.port_id     = dprm_port_p->port_id;
  notification_data.port_handle = *port_handle_p;
#if 0
  OF_LIST_SCAN(datapath_notifications[DPRM_DATAPATH_PORT_ADDED],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
  {
    ((dprm_datapath_notifier_fn)(app_entry_p->call_back))(DPRM_DATAPATH_PORT_ADDED,
                                                          datapath_handle,
                                                          notification_data,
                                                          app_entry_p->cbk_arg1,
                                                          app_entry_p->cbk_arg2);
  }

  OF_LIST_SCAN(datapath_notifications[DPRM_DATAPATH_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
  {
    ((dprm_datapath_notifier_fn)(app_entry_p->call_back))(DPRM_DATAPATH_PORT_ADDED,
                                                          datapath_handle,
                                                          notification_data,
                                                          app_entry_p->cbk_arg1,
                                                          app_entry_p->cbk_arg2);
  }
#endif
  dprm_send_port_update_events(datapath_info_p, port_entry_p);
  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_SUCCESS;
DPRMEXIT_PORT:
  if(port_entry_p)
    mempool_release_mem_block(datapath_info_p->dprm_dpnode_port_mempool_g,(uchar8_t*)port_entry_p,port_entry_p->heap_b);
  CNTLR_RCU_READ_LOCK_RELEASE();
  return ret_value;
}
/*****************************************************************************************************/
int32_t dprm_delete_port_from_datapath(uint64_t datapath_handle,uint64_t port_handle)
{

  struct   dprm_datapath_entry* datapath_info_p;
  struct   dprm_port_entry* port_info_p;
  uint32_t index,magic;
  int32_t  status_b;
  uchar8_t offset; 

  struct   dprm_datapath_notification_data  notification_data;
  struct   dprm_notification_app_hook_info  *app_entry_p;
  uchar8_t lstoffset;
  lstoffset = DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET;
  
  if(dprm_datapath_table_g == NULL)
    return DPRM_FAILURE;
  
  magic = (uint32_t)(datapath_handle >>32);
  index = (uint32_t)datapath_handle;

  CNTLR_RCU_READ_LOCK_TAKE();

  MCHASH_ACCESS_NODE(dprm_datapath_table_g,index,magic,datapath_info_p,status_b);
  if(CNTLR_UNLIKELY(status_b == FALSE))
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_ERROR_INVALID_DATAPATH_HANDLE;
  }

  magic = (uint32_t)(port_handle >>32);
  index = (uint32_t)port_handle;

  /* may require to use access node to inspect port structure before deleting it like attributes */

  MCHASH_ACCESS_NODE(datapath_info_p->dprm_dpnode_porttable_p,index,magic,port_info_p,status_b);
  if(CNTLR_UNLIKELY(status_b == FALSE))
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_ERROR_INVALID_PORT_HANDLE;
  }

  if(port_info_p->number_of_attributes)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_ERROR_RESOURCE_NOTEMPTY;
  }  

  /* Send port detached to domain notification to the registered Applications. */

  notification_data.dpid        = datapath_info_p->dpid;
  strcpy(notification_data.port_name,port_info_p->port_name);
  notification_data.port_id     = port_info_p->port_id;
  notification_data.port_handle = port_handle;

  OF_LIST_SCAN(datapath_notifications[DPRM_DATAPATH_PORT_DELETE],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
  {
    ((dprm_datapath_notifier_fn)(app_entry_p->call_back))(DPRM_DATAPATH_PORT_DELETE,
                                                          datapath_handle,
                                                          notification_data,
                                                          app_entry_p->cbk_arg1,
                                                          app_entry_p->cbk_arg2);
  }

  OF_LIST_SCAN(datapath_notifications[DPRM_DATAPATH_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
  {
    ((dprm_datapath_notifier_fn)(app_entry_p->call_back))(DPRM_DATAPATH_PORT_DELETE,
                                                          datapath_handle,
                                                          notification_data,
                                                          app_entry_p->cbk_arg1,
                                                          app_entry_p->cbk_arg2);
  }

  dprm_debug("%s:%d About to delete a portnode from datapath hash table\r\n",__FUNCTION__,__LINE__);
  offset = PORTNODE_PORTTBL_OFFSET;
  MCHASH_DELETE_NODE_BY_REF(datapath_info_p->dprm_dpnode_porttable_p,index,magic,struct dprm_port_entry *,offset,status_b);
  
  if(status_b == TRUE)
  {
    datapath_info_p->number_of_ports--;
    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_SUCCESS;
  }
  else
  {
    CNTLR_RCU_READ_LOCK_RELEASE();  
    return DPRM_FAILURE;
  }  
}
/******************************************************************************************************/
int32_t dprm_get_port_handle(uint64_t dp_handle,char* port_name_p,uint64_t* port_handle_p)
{
  struct   dprm_datapath_entry* datapath_info_p;
  struct   dprm_port_entry* port_entry_p = NULL;
  uchar8_t offset;
  uint32_t hashkey;
  int32_t  ret_value;

  ret_value = get_datapath_byhandle(dp_handle,&datapath_info_p);
  if(ret_value == DPRM_ERROR_INVALID_DATAPATH_HANDLE)
    return ret_value;

  hashkey = dprm_get_hashval_byname(port_name_p,dprm_no_of_dpnode_port_buckets_g);

  offset = PORTNODE_PORTTBL_OFFSET;
  MCHASH_BUCKET_SCAN(datapath_info_p->dprm_dpnode_porttable_p,hashkey,port_entry_p,struct dprm_port_entry *,offset)
  {
    if(!strcmp(port_entry_p->port_name,port_name_p))
    {
      *port_handle_p = port_entry_p->magic;
      *port_handle_p = ((*port_handle_p <<32) | (port_entry_p->index));
      return DPRM_SUCCESS;
    }
    continue;
  }
  return DPRM_ERROR_INVALID_NAME;
} 
/******************************************************************************************************/
int32_t  dprm_get_port_handle_by_port_id(uint64_t dp_handle, uint16_t port_id, uint64_t *port_handle_ptr)
{
	struct   dprm_datapath_entry* datapath_info_p;
	int32_t   retval=DPRM_FAILURE;
	uint32_t index,magic;
	uint32_t hashkey;
	int32_t  status_b;
	struct   dprm_port_entry* port_entry_p = NULL;
	uchar8_t offset;

	if(dprm_datapath_table_g == NULL)
		return DPRM_FAILURE;

	magic = (uint32_t)(dp_handle >>32);
	index = (uint32_t)dp_handle;

	CNTLR_RCU_READ_LOCK_TAKE();
	MCHASH_ACCESS_NODE(dprm_datapath_table_g,index,magic,datapath_info_p,status_b);
	if(CNTLR_UNLIKELY(status_b == FALSE))
	{
		CNTLR_RCU_READ_LOCK_RELEASE();
		return DPRM_ERROR_INVALID_DATAPATH_HANDLE;
	}
	if(datapath_info_p->dprm_dpnode_porttable_p == NULL)
	{
		CNTLR_RCU_READ_LOCK_RELEASE();
		return DPRM_FAILURE;
	}

	offset = PORTNODE_PORTTBL_OFFSET;
	MCHASH_TABLE_SCAN(datapath_info_p->dprm_dpnode_porttable_p,hashkey)
	{
		MCHASH_BUCKET_SCAN(datapath_info_p->dprm_dpnode_porttable_p,hashkey,port_entry_p,struct dprm_port_entry *,offset)
		{
			if (port_entry_p->port_id == port_id)
			{
				*port_handle_ptr = port_entry_p->port_handle;
				retval=DPRM_SUCCESS;
				break;
			}
			else
				continue;

		}
		if(retval == DPRM_SUCCESS)
			break;
	}
	CNTLR_RCU_READ_LOCK_RELEASE();	

	OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s: given port_id  =%d retval=%d\r\n",__FUNCTION__,port_id,retval);
	return retval;
}

/******************************************************************************************************/
int32_t get_datapath_port_byhandle(uint64_t datapath_handle,uint64_t port_handle,struct dprm_port_entry** port_info_p_p)
{
  /* Caller shall take RCU locks. */
  uint32_t index,magic;
  struct   dprm_datapath_entry* datapath_info_p; 
  uint8_t  status_b;

  magic = (uint32_t)(datapath_handle >>32);
  index = (uint32_t)datapath_handle;
 
  MCHASH_ACCESS_NODE(dprm_datapath_table_g,index,magic,datapath_info_p,status_b);
  if(CNTLR_LIKELY(status_b))
  {
    magic = (uint32_t)(port_handle >>32);
    index = (uint32_t)port_handle;

    MCHASH_ACCESS_NODE(datapath_info_p->dprm_dpnode_porttable_p,index,magic,*port_info_p_p,status_b);
    if(status_b == TRUE)
    {
      return DPRM_SUCCESS;
    }
    else
      return DPRM_ERROR_INVALID_PORT_HANDLE;
  } 
  return DPRM_ERROR_INVALID_DATAPATH_HANDLE;
}
/****************************************************************************************************/
int32_t dprm_get_exact_datapath_port(uint64_t datapath_handle,
                                     struct   dprm_port_info *port_info_p,
                                     struct   dprm_port_runtime_info *runtime_info_p,
                                     uint64_t port_handle_p) 
{
  struct   dprm_datapath_entry* datapath_info_p;
  struct   dprm_port_entry* port_entry_p;

  uint32_t index,magic;
  int32_t  status_b;

  magic = (uint32_t)(datapath_handle >>32);
  index = (uint32_t)datapath_handle;

  CNTLR_RCU_READ_LOCK_TAKE();

  MCHASH_ACCESS_NODE(dprm_datapath_table_g,index,magic,datapath_info_p,status_b);
  if(CNTLR_UNLIKELY(status_b == FALSE))
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_ERROR_INVALID_DATAPATH_HANDLE;
  }
  magic = (uint32_t)(port_handle_p >>32);
  index = (uint32_t)(port_handle_p);
  MCHASH_ACCESS_NODE(datapath_info_p->dprm_dpnode_porttable_p,index,magic,port_entry_p,status_b);
  if(CNTLR_LIKELY(status_b))
  {
     port_info_p->port_id = port_entry_p->port_id;
     strcpy(port_info_p->port_name,port_entry_p->port_name);
     runtime_info_p->dummy = 0;
     CNTLR_RCU_READ_LOCK_RELEASE();
     return DPRM_SUCCESS;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_ERROR_INVALID_PORT_HANDLE;
}
/******************************************************************************************************/
int32_t get_port_byname(struct dprm_datapath_entry* datapath_info_p,char* port_name_p,struct dprm_port_entry** port_info_p_p)
{
  uint32_t hashkey,offset; 
  struct dprm_port_entry* port_info1_p;  
  
  hashkey = dprm_get_hashval_byname(port_name_p,dprm_no_of_dpnode_port_buckets_g);

  offset = PORTNODE_PORTTBL_OFFSET;
  MCHASH_BUCKET_SCAN(datapath_info_p->dprm_dpnode_porttable_p,hashkey,port_info1_p,struct dprm_port_entry *,offset)
  {
    if(!strcmp(port_info1_p->port_name,port_name_p))
      return DPRM_SUCCESS;
    continue;
  }
  return DPRM_FAILURE;
}
/*****************************************************************************************************/
int32_t dprm_get_first_datapath_port(uint64_t datapath_handle,
                                     struct   dprm_port_info *port_info_p,
                                     struct   dprm_port_runtime_info *runtime_info_p,
                                     uint64_t *port_handle_p)
{
  struct   dprm_datapath_entry* datapath_info_p;
  struct   dprm_port_entry* port_entry_p = NULL;
  uint32_t index,magic;
  int32_t  status_b;
  uint32_t hashkey;

  uchar8_t offset;

  if(dprm_datapath_table_g == NULL)
    return DPRM_FAILURE;

  magic = (uint32_t)(datapath_handle >>32);
  index = (uint32_t)datapath_handle;

  CNTLR_RCU_READ_LOCK_TAKE();

  MCHASH_ACCESS_NODE(dprm_datapath_table_g,index,magic,datapath_info_p,status_b);
  if(CNTLR_UNLIKELY(status_b == FALSE))
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_ERROR_INVALID_DATAPATH_HANDLE;
  }

  if(datapath_info_p->dprm_dpnode_porttable_p == NULL)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_FAILURE;
  }

  offset = PORTNODE_PORTTBL_OFFSET;
  MCHASH_TABLE_SCAN(datapath_info_p->dprm_dpnode_porttable_p,hashkey)
  {
    MCHASH_BUCKET_SCAN(datapath_info_p->dprm_dpnode_porttable_p,hashkey,port_entry_p,struct dprm_port_entry *,offset)
    {
      *port_handle_p = port_entry_p->port_handle;
      port_info_p->port_id = port_entry_p->port_id;
      strcpy(port_info_p->port_name,port_entry_p->port_name);
      memcpy(&port_info_p->port_desc,&port_entry_p->port_info, sizeof(struct    ofl_port_desc_info));

      runtime_info_p->dummy = 0;

      CNTLR_RCU_READ_LOCK_RELEASE();
      return DPRM_SUCCESS;
    }
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_ERROR_NO_MORE_ENTRIES;
}  
/*****************************************************************************************************/
int32_t dprm_get_next_datapath_port(uint64_t  datapath_handle,
                                    struct    dprm_port_info *port_info_p,
                                    struct    dprm_port_runtime_info *runtime_info_p,
                                    uint64_t  *port_handle_p)
{
  struct   dprm_datapath_entry* datapath_info_p;
  struct   dprm_port_entry* port_entry_p = NULL;
  uint32_t index,magic;
  int32_t  status_b;
  uint8_t  current_entry_found_b;
  uint32_t hashkey;

  uchar8_t offset;

  magic = (uint32_t)(datapath_handle >>32);
  index = (uint32_t)datapath_handle;

  CNTLR_RCU_READ_LOCK_TAKE();

  MCHASH_ACCESS_NODE(dprm_datapath_table_g,index,magic,datapath_info_p,status_b);
  if(CNTLR_UNLIKELY(status_b == FALSE))
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_ERROR_INVALID_DATAPATH_HANDLE;
  }
  
  offset = PORTNODE_PORTTBL_OFFSET;
  current_entry_found_b = 0; 
  MCHASH_TABLE_SCAN(datapath_info_p->dprm_dpnode_porttable_p,hashkey)
  {
    MCHASH_BUCKET_SCAN(datapath_info_p->dprm_dpnode_porttable_p,hashkey,port_entry_p,struct dprm_port_entry *,offset)
    {
      if(current_entry_found_b == 0)
      {
        if(*port_handle_p != port_entry_p->port_handle)
          continue;
        else
        {
          current_entry_found_b = 1;
          continue;
        }
        /*Skip the First matching Port and Get the next Port */
      }

      *port_handle_p = port_entry_p->port_handle;
      port_info_p->port_id = port_entry_p->port_id;
      strcpy(port_info_p->port_name,port_entry_p->port_name);
      memcpy(&port_info_p->port_desc,&port_entry_p->port_info, sizeof(struct    ofl_port_desc_info));

      runtime_info_p->dummy = 0;
      CNTLR_RCU_READ_LOCK_RELEASE();
      return DPRM_SUCCESS;
    }
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  if(current_entry_found_b == 1)
    return DPRM_ERROR_NO_MORE_ENTRIES;
  else
    return DPRM_ERROR_INVALID_PORT_HANDLE;
}
/*********************************************************************************************************/
int32_t dprm_register_datapath_notifications(uint32_t notification_type,
                                             dprm_datapath_notifier_fn datapath_notifier_fn,
                                             void     *callback_arg1,
                                             void     *callback_arg2)
{
  struct  dprm_notification_app_hook_info *app_entry_p;
  uint8_t heap_b;
  int32_t retval;
  uchar8_t lstoffset;
  lstoffset = DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET;

  if((notification_type < DPRM_DATAPATH_FIRST_NOTIFICATION_TYPE) || (notification_type > DPRM_DATAPATH_LAST_NOTIFICATION_TYPE))
    return DPRM_ERROR_INVALID_DATAPATH_NOTIFICATION_TYPE;
  if(datapath_notifier_fn == NULL)
    return DPRM_ERROR_NULL_CALLBACK_DATAPATH_NOTIFICATION_HOOK;
   
  retval = mempool_get_mem_block(dprm_notifications_mempool_g,(uchar8_t**)&app_entry_p,&heap_b);
  if(retval != MEMPOOL_SUCCESS)
    return DPRM_ERROR_NO_MEM;

  app_entry_p->call_back = (void*)datapath_notifier_fn;
  app_entry_p->cbk_arg1  = callback_arg1;
  app_entry_p->cbk_arg2  = callback_arg2;
  app_entry_p->heap_b    = heap_b;

  /* Add Application to the list of switch Applications maintained per notification type. */
  OF_APPEND_NODE_TO_LIST(datapath_notifications[notification_type],app_entry_p,lstoffset);

  if(retval != OF_SUCCESS)
    mempool_release_mem_block(dprm_notifications_mempool_g,(uchar8_t*)app_entry_p,app_entry_p->heap_b);
  return retval;
}
int32_t dprm_deregister_datapath_notifications(uint32_t notification_type,
                                               dprm_datapath_notifier_fn datapath_notifier_fn)
{
  struct dprm_notification_app_hook_info *app_entry_p,*prev_app_entry_p = NULL;
  uchar8_t lstoffset;
  lstoffset = DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET;
  
  CNTLR_RCU_READ_LOCK_TAKE();
  OF_LIST_SCAN(datapath_notifications[notification_type],app_entry_p,struct dprm_notification_app_hook_info *,lstoffset )
  {
    if(app_entry_p->call_back != datapath_notifier_fn)
    {
      prev_app_entry_p = app_entry_p;
      continue;
    }
    OF_REMOVE_NODE_FROM_LIST(datapath_notifications[notification_type],app_entry_p,prev_app_entry_p,lstoffset );
    CNTLR_RCU_READ_LOCK_RELEASE();
    return DPRM_SUCCESS;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_ERROR_NO_DATAPATH_NOTIFICATION;
}
/*****************************************************************************************************/
void  dprm_get_attribute_mempoolentries(uint32_t *attribute_entries_max, uint32_t *attribute_static_entries)
{
  *attribute_entries_max    = DPRM_MAX_ATTRIBUTE_ENTRIES;
  *attribute_static_entries = DPRM_MAX_ATTRIBUTE_STATIC_ENTRIES;
}
void  dprm_get_attribute_value_mempoolentries(uint32_t *attribute_value_entries_max, uint32_t *attribute_value_static_entries)
{
  *attribute_value_entries_max    = DPRM_MAX_ATTRIBUTE_ENTRIES;
  *attribute_value_static_entries = DPRM_MAX_ATTRIBUTE_STATIC_ENTRIES;
}  
/*****************************************************************************************************/
void dprm_get_notification_mempoolentries(uint32_t *notification_entries_max,uint32_t *notification_static_entries)
{
  *notification_entries_max    = DPRM_MAX_NOTIFICATION_ENTRIES; 
  *notification_static_entries = DPRM_MAX_NOTIFICATION_STATIC_ENTRIES; 
}  
/*****************************************************************************************************/
void dprm_get_datapath_mempoolentries(uint32_t* datapath_entries_max,uint32_t* datapath_static_entries)
{
  *datapath_entries_max    = DPRM_MAX_DATAPATH_ENTRIES;
  *datapath_static_entries = DPRM_MAX_DATAPATH_STATIC_ENTRIES;
}
void dprm_get_dpnode_port_mempoolentries(uint32_t* port_entries_max, uint32_t* port_static_entries)
{  
  *port_entries_max    = DPRM_MAX_DPNODE_PORTTBL_ENTRIES;
  *port_static_entries = DPRM_MAX_DPNODE_PORTTBL_STATIC_ENTRIES;
}
/*****************************************************************************************************/
void dprm_get_switch_mempoolentries(uint32_t* switch_entries_max,uint32_t* switch_static_entries)
{
  *switch_entries_max    = DPRM_MAX_SWITCH_ENTRIES;
  *switch_static_entries = DPRM_MAX_SWITCH_STATIC_ENTRIES;
}
/*****************************************************************************************************/
void dprm_get_swnode_datapath_mempoolentries(uint32_t* swnode_dptable_entries_max, uint32_t* swnode_dptable_static_entries)
{
  *swnode_dptable_entries_max     = DPRM_MAX_SWNODE_DPTBL_ENTRIES;
  *swnode_dptable_static_entries  = DPRM_MAX_SWNODE_DPTBL_STATIC_ENTRIES;
}
/*****************************************************************************************************/
void  dprm_get_dmnode_datapath_mempoolentries(uint32_t* dmnode_dptable_entries_max, uint32_t* dmnode_dptable_static_entries)
{
  *dmnode_dptable_entries_max     = DPRM_MAX_DMNODE_DPTBL_ENTRIES;
  *dmnode_dptable_static_entries  = DPRM_MAX_DMNODE_DPTBL_STATIC_ENTRIES;
}
/*****************************************************************************************************/
uint32_t hashkey_func(void *pHashArgs,uint32_t ulBuckets)
{
    return DPRM_SUCCESS;
}
uint32_t matchnode_func(void *pObjNode,uint32_t Arg1,uint32_t Arg2,uint32_t Arg3,
        uint32_t Arg4,uint32_t Arg5,uint32_t Arg6,uint32_t Arg7)
{
    return DPRM_SUCCESS;
}
/***************************************************************************************************/
uint32_t dprm_get_hashval_byname(char* name_p,uint32_t no_of_buckets)
{
  uint32_t hashkey,param1,param2,param3,param4;
  char name_hash[16];
  memset(name_hash,0,16);
  strncpy(name_hash,name_p,16);
  param1 = *(uint32_t *)(name_hash +0);
  param2 = *(uint32_t *)(name_hash +4);
  param3 = *(uint32_t *)(name_hash +8);
  param4 = *(uint32_t *)(name_hash +12);
  DPRM_COMPUTE_HASH(param1,param2,param3,param4,no_of_buckets,hashkey);
  return hashkey;
}
/**************************************************************************************************/
uint32_t dprm_get_hashval_byid(uint64_t dpid,uint32_t no_of_buckets)
{
  uint32_t hashkey,param1,param2,param3,param4;

  param1 = (uint32_t)(dpid >>32);
  param2 = (uint32_t)dpid;
  param3 = 1;
  param4 = 1;
  DPRM_COMPUTE_HASH(param1,param2,param3,param4,no_of_buckets,hashkey);
 return hashkey;
}
/*************************************************************************************************/

#if 0
int32_t dprm_add_transport_endpoint(uint64_t controller_handle,
     struct controller_transport_endpoint_info *endpoint_info)
{
     int32_t  retval = OF_SUCCESS;

     if(endpoint_info == NULL)
        return DPRM_ERROR_INVALID_ENDPOINT_INFO;

     retval = cntlr_add_transport_endpoint(controller_handle,
                                           endpoint_info->port,
                                           endpoint_info->connection_type);
    if(retval ==  OF_FAILURE)
      return DPRM_ERROR_FAILED_TO_ADD_ENDPOINT;
    else
      return DPRM_SUCCESS;
}
#endif 
/*************************************************************************************************/
int32_t dprm_init()
{
  int32_t  ii,ret_value;
  uint32_t notification_entries_max,notification_static_entries;
  uint32_t attribute_entries_max,attribute_static_entries;
  uint32_t attribute_value_entries_max,attribute_value_static_entries;
  uint32_t switch_entries_max,switch_static_entries;
  uint32_t domain_entries_max,domain_static_entries;
  uint32_t datapath_entries_max,datapath_static_entries;
  struct mempool_params mempool_info={};

  ret_value = dprm_namespace_init();
  if(ret_value != DPRM_SUCCESS)
    return ret_value;

  dprm_get_switch_mempoolentries(&switch_entries_max,&switch_static_entries);

  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;
  mempool_info.block_size = sizeof(struct dprm_switch_entry);
  mempool_info.max_blocks = switch_entries_max;
  mempool_info.static_blocks = switch_static_entries;
  mempool_info.threshold_min = switch_static_entries/10;
  mempool_info.threshold_max = switch_static_entries/3;
  mempool_info.replenish_count = switch_static_entries/10;
  mempool_info.b_memset_not_required =  FALSE;
  mempool_info.b_list_per_thread_required =  FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("dprm_switch_pool", &mempool_info,
                                  &dprm_switch_mempool_g    
                                 );

  if(ret_value != MEMPOOL_SUCCESS)
    goto DPRMINIT_EXIT;
 
  ret_value = mchash_table_create(((switch_entries_max / 5)+1),
                                    switch_entries_max,
                                    dprm_free_swentry_rcu,
                                    &dprm_switch_table_g);
  if(ret_value != MCHASHTBL_SUCCESS)
    goto DPRMINIT_EXIT; 

  dprm_no_of_switch_buckets_g = (switch_entries_max / 5) + 1;

  dprm_get_domain_mempoolentries(&domain_entries_max,&domain_static_entries);

  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;
  mempool_info.block_size = sizeof(struct dprm_domain_entry);
  mempool_info.max_blocks = domain_entries_max;
  mempool_info.static_blocks = domain_static_entries;
  mempool_info.threshold_min = domain_static_entries/10;
  mempool_info.threshold_max = domain_static_entries/3;
  mempool_info.replenish_count = domain_static_entries/10;
  mempool_info.b_memset_not_required =  FALSE;
  mempool_info.b_list_per_thread_required =  FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("dprm_domain_pool", &mempool_info,
                                   &dprm_domain_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    goto DPRMINIT_EXIT;
 
  ret_value = mchash_table_create(((domain_entries_max / 5)+1),
                                    domain_entries_max,
                                    dprm_free_domainentry_rcu,
                                    &dprm_domain_table_g);
  if(ret_value != MCHASHTBL_SUCCESS)
    goto DPRMINIT_EXIT;
  
  dprm_no_of_domain_buckets_g = (domain_entries_max / 5) + 1;

  dprm_get_datapath_mempoolentries(&datapath_entries_max,&datapath_static_entries);

  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;
  mempool_info.block_size = sizeof(struct dprm_datapath_entry);
  mempool_info.max_blocks = datapath_entries_max;
  mempool_info.static_blocks = datapath_static_entries;
  mempool_info.threshold_min = datapath_static_entries/10;
  mempool_info.threshold_max = datapath_static_entries/3;
  mempool_info.replenish_count = datapath_static_entries/10;
  mempool_info.b_memset_not_required =  FALSE;
  mempool_info.b_list_per_thread_required =  FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("dprm_datapath_pool", &mempool_info,
                                   &dprm_datapath_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    goto DPRMINIT_EXIT;
 
  ret_value = mchash_table_create(((datapath_entries_max / 5)+1),
                                    datapath_entries_max,
                                    dprm_free_datapathentry_rcu,
                                    &dprm_datapath_table_g);
  if(ret_value != MCHASHTBL_SUCCESS)
    goto DPRMINIT_EXIT;
  
  dprm_no_of_datapath_buckets_g = (datapath_entries_max / 5) + 1;

  dprm_get_notification_mempoolentries(&notification_entries_max,&notification_static_entries);

  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;
  mempool_info.block_size = sizeof(struct dprm_notification_app_hook_info);
  mempool_info.max_blocks = notification_entries_max;
  mempool_info.static_blocks = notification_static_entries;
  mempool_info.threshold_min = notification_static_entries/10;
  mempool_info.threshold_max = notification_static_entries/3;
  mempool_info.replenish_count = notification_static_entries/10;
  mempool_info.b_memset_not_required =  FALSE;
  mempool_info.b_list_per_thread_required =  FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("dprm_notifications_pool", &mempool_info,
                                   &dprm_notifications_mempool_g);

  if(ret_value != MEMPOOL_SUCCESS)
    goto DPRMINIT_EXIT;
  
  for(ii=0;ii<DPRM_SWITCH_LAST_NOTIFICATION_TYPE;ii++)
    OF_LIST_INIT(switch_notifications[ii],dprm_notifications_free_app_entry_rcu);

  for(ii=0;ii<DPRM_DOMAIN_LAST_NOTIFICATION_TYPE;ii++)
        OF_LIST_INIT(domain_notifications[ii],dprm_notifications_free_app_entry_rcu);

  for(ii=0;ii<DPRM_DATAPATH_LAST_NOTIFICATION_TYPE;ii++)
        OF_LIST_INIT(datapath_notifications[ii],dprm_notifications_free_app_entry_rcu);

  dprm_get_attribute_mempoolentries(&attribute_entries_max,&attribute_static_entries);

  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;
  mempool_info.block_size = sizeof(struct dprm_resource_attribute_entry);
  mempool_info.max_blocks = attribute_entries_max;
  mempool_info.static_blocks = attribute_static_entries;
  mempool_info.threshold_min = attribute_static_entries/10;
  mempool_info.threshold_max = attribute_static_entries/3;
  mempool_info.replenish_count = attribute_static_entries/10;
  mempool_info.b_memset_not_required =  FALSE;
  mempool_info.b_list_per_thread_required =  FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("dprm_attributes_pool", &mempool_info,
                                   &dprm_attributes_mempool_g);

  if(ret_value != MEMPOOL_SUCCESS)
    goto DPRMINIT_EXIT;

  dprm_get_attribute_value_mempoolentries(&attribute_value_entries_max,&attribute_value_static_entries);

  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;
  mempool_info.block_size = sizeof(struct dprm_resource_attribute_value_entry);
  mempool_info.max_blocks = attribute_value_entries_max;
  mempool_info.static_blocks = attribute_value_static_entries;
  mempool_info.threshold_min = attribute_value_static_entries/10;
  mempool_info.threshold_max = attribute_value_static_entries/3;
  mempool_info.replenish_count = attribute_value_static_entries/10;
  mempool_info.b_memset_not_required =  FALSE;
  mempool_info.b_list_per_thread_required =  FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("dprm_attributes_value_pool", &mempool_info,
                                   &dprm_attributes_value_mempool_g);

  if(ret_value != MEMPOOL_SUCCESS)
    goto DPRMINIT_EXIT;



  return DPRM_SUCCESS;

DPRMINIT_EXIT:
  mempool_delete_pool(dprm_switch_mempool_g); 
  mempool_delete_pool(dprm_domain_mempool_g);
  mempool_delete_pool(dprm_datapath_mempool_g);
  mempool_delete_pool(dprm_notifications_mempool_g);
  mempool_delete_pool(dprm_attributes_mempool_g);
  mempool_delete_pool(dprm_attributes_value_mempool_g);

  mchash_table_delete(dprm_switch_table_g);
  mchash_table_delete(dprm_domain_table_g);
  mchash_table_delete(dprm_datapath_table_g);
  
  return ret_value;  
}
/******************************************************************************************************/
int32_t dprm_uninit()
{
 
  int32_t ret_value = FALSE;

  ret_value = mchash_table_delete(dprm_switch_table_g);
  if(ret_value != MCHASHTBL_SUCCESS)
    return ret_value;

  ret_value = mchash_table_delete(dprm_domain_table_g);
  if(ret_value != MCHASHTBL_SUCCESS)
    return ret_value;

  ret_value = mchash_table_delete(dprm_datapath_table_g);
  if(ret_value != MCHASHTBL_SUCCESS)
    return ret_value;

  mempool_delete_pool(dprm_switch_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    return ret_value;

  mempool_delete_pool(dprm_domain_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    return ret_value;

  mempool_delete_pool(dprm_datapath_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    return ret_value;

  return DPRM_SUCCESS;
}
/******************************************************************************************************/
/*Test routines to test internal APIs and are called from dprmtest.c as they need private members */
/******************************************************************************************************/
void get_datapath(void)
{
  uint64_t dpid2;
  uint64_t datapath_handle;
  struct   dprm_datapath_entry* datapath_info_p;
  int32_t  lRet_value0;
   
  dpid2 = 200;
  lRet_value0 = dprm_get_datapath_handle(dpid2,&datapath_handle);
  if(lRet_value0 == DPRM_SUCCESS)
  {  
    CNTLR_RCU_READ_LOCK_TAKE();
    lRet_value0 = get_datapath_byhandle(datapath_handle,&datapath_info_p);
  
    if(lRet_value0 == DPRM_SUCCESS)
    {
      printf("\r\n dpid = %llx",datapath_info_p->dpid);
      printf("\r\n subject name = %s",datapath_info_p->expected_subject_name_for_authentication);

      printf("\r\n number of tables = %d",datapath_info_p->number_of_ports);
      printf("\r\n number of attributes = %d",datapath_info_p->number_of_attributes);
    } 
    CNTLR_RCU_READ_LOCK_RELEASE();
  }
  printf("\r\n Failed to get datapath\n");
}
/*****************************************************************************************************/
void get_domain(void)
{
  char*    domain_name;
  uint64_t domain_handle;
  struct   dprm_domain_entry* domain_info_p;
  int32_t  lRet_value0;

  domain_name = "mspmurthydom";
  lRet_value0 = dprm_get_forwarding_domain_handle(domain_name,&domain_handle);
 
  CNTLR_RCU_READ_LOCK_TAKE();
  lRet_value0 = get_domain_byhandle(domain_handle,&domain_info_p);
                  
  if(lRet_value0 == DPRM_SUCCESS)
  {
    printf("\r\n domain name  = %s",domain_info_p->name);
    printf("\r\n subject name = %s",domain_info_p->expected_subject_name_for_authentication);
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
}
/*****************************************************************************************************/
#if 0
void get_table(uint64_t domain_handle )
{
  char    *table_name_p;
  struct  dprm_oftable_entry* table_info_p;
  int32_t lRet_value0;

  table_name_p = "nsmurthy";
  lRet_value0 = get_domain_table_byname(domain_handle,table_name_p,&table_info_p);
  if(lRet_value0 == DPRM_SUCCESS)
  {
    printf("\r\n table name  = %s",(table_info_p->table_name));
    printf("\r\n table id    = %d",table_info_p->table_id);
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
}
#endif
/*****************************************************************************************************/
void get_switch(void)
{
  char*    switch_name;
  uint64_t switch_handle;
  struct   dprm_switch_entry* switch_info_p;
  int32_t  lRet_value0;

  switch_name = "nsmurthysw";
 
  CNTLR_RCU_READ_LOCK_TAKE();
  lRet_value0 = dprm_get_switch_handle(switch_name,&switch_handle);
  
  if(lRet_value0 ==DPRM_SUCCESS) 
  {
    lRet_value0 = get_switch_byhandle(switch_handle,&switch_info_p);

    if(lRet_value0 == DPRM_SUCCESS)
    {
      printf("\r\n switch name  = %s",switch_info_p->name);
      printf("\r\n subject name = %s",switch_info_p->switch_fqdn_p);
    }
  }
  else 
    printf("\r\n Switch handle invalid ");  
  CNTLR_RCU_READ_LOCK_RELEASE();
}
/*****************************************************************************************************/
void get_dp_port(uint64_t dp_handle)
{
  char*    port_name_p;
  uint64_t port_handle;
  struct   dprm_port_entry* port_info_p;
  int32_t  lRet_value0;

  port_name_p = "nsmfprt1";
  
  CNTLR_RCU_READ_LOCK_TAKE();
  lRet_value0 = dprm_get_port_handle(dp_handle,port_name_p,&port_handle);
  
  if(lRet_value0 == DPRM_SUCCESS)
  {
    lRet_value0 = get_datapath_port_byhandle(dp_handle,port_handle,&port_info_p);
    if(lRet_value0 == DPRM_SUCCESS)
    {
      printf("\r\n port name = %s",(port_info_p->port_name));
      printf("\r\n port id   = %d",port_info_p->port_id);
    }
  }  
  CNTLR_RCU_READ_LOCK_RELEASE();
}
#if 0
/*******************************************************************************************************/
int32_t perform_tests_on_idbased_table()
{
  int32_t  lRet_value0;
  uint64_t dm_handle;
  struct   dprm_oftable_entry *table_info_p;

  lRet_value0 =  dprm_get_forwarding_domain_handle("mspmurthydom",&dm_handle);
  if(lRet_value0 != DPRM_SUCCESS)
  {
    printf("\r\n Failed to get the domain handle");
    return DPRM_FAILURE;
  }  
                          
  lRet_value0 = get_domain_table_byname(dm_handle,"dprmtabl2",&table_info_p); 
  if(lRet_value0 != DPRM_SUCCESS)
  {
    printf("\r\n Failed to get the table structure");
    return DPRM_FAILURE;
  }                          
                                  
  lRet_value0 = add_domain_oftable_to_idbased_list(dm_handle,1,table_info_p);
  if(lRet_value0 != DPRM_SUCCESS)  
  {
    printf("\r\n Failed to add table to idbased table list");
    return DPRM_FAILURE;
  }

  table_info_p = NULL;
  lRet_value0 = get_domain_oftable_from_idbased_list(dm_handle,1,&table_info_p);
  if(lRet_value0 == DPRM_SUCCESS)
    printf("\r\n table name = %s",table_info_p->table_name);
  else
    printf("\r\n Failed to get oftable from idbased list");

  lRet_value0 = del_domain_oftable_from_idbased_list(dm_handle,1);
  if(lRet_value0 != DPRM_SUCCESS)
    printf("\r\n Failed to delete table from idbased list");
  else
  {
    printf("\r\n Successfully deleted the table form idbased list");
    table_info_p = NULL;
    lRet_value0 = get_domain_oftable_from_idbased_list(dm_handle,1,&table_info_p);
    if(lRet_value0 == DPRM_SUCCESS)
      printf("\r\n table name = %s",table_info_p->table_name);
    else
      printf("\r\n Failed to get oftable from idbased list");
  }  
  return DPRM_SUCCESS;
}
#endif
/*******************************************************************************************************/
/******************************************** Attributes section ***************************************/
int32_t dprm_add_attribute(of_list_t *attributes_p,struct dprm_attribute_name_value_pair* attribute_info_p)
{
  struct  dprm_resource_attribute_entry *attribute_entry_p,*attribute_entry_scan_p;
  struct  dprm_resource_attribute_value_entry *attribute_value_entry_p,*attribute_value_entry_scan_p;
  uint8_t heap_b = 0;
  int32_t retval;

  /* Check whether name of the attribute is unique within the switch */

   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered");
  if(attribute_info_p == NULL)
    return DPRM_FAILURE;

  if(attribute_info_p->name_string == NULL)
    return DPRM_ERROR_ATTRIBUTE_NAME_NULL;

  if(attribute_info_p->value_string == NULL)
    return DPRM_ERROR_ATTRIBUTE_VALUE_NULL;

  if(strlen(attribute_info_p->name_string) > DPRM_MAX_ATTRIBUTE_NAME_LEN)
    return DPRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN;

  if(strlen(attribute_info_p->value_string) > DPRM_MAX_ATTRIBUTE_VALUE_LEN)
    return DPRM_ERROR_INVALID_ATTRIBUTE_VALUE_LEN;
  
  OF_LIST_SCAN(*attributes_p, attribute_entry_scan_p, struct dprm_resource_attribute_entry*, DPRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET )
  {
    if(!strcmp(attribute_entry_scan_p->name, attribute_info_p->name_string))
    {

   OF_LIST_SCAN(attribute_entry_scan_p->attribute_value_list, attribute_value_entry_scan_p, struct dprm_resource_attribute_value_entry*, DPRM_RESOURCE_ATTRIBUTE_VALUE_ENTRY_LISTNODE_OFFSET )
        {
          if(!strcmp(attribute_value_entry_scan_p->value, attribute_info_p->value_string))
       {
              return DPRM_ERROR_DUPLICATE_RESOURCE;
       }
        }
        retval = mempool_get_mem_block(dprm_attributes_value_mempool_g,(uchar8_t**)&attribute_value_entry_p,&heap_b);
   if(retval != MEMPOOL_SUCCESS)
     return  DPRM_ERROR_NO_MEM;

   strcpy(attribute_value_entry_p->value, attribute_info_p->value_string);
   attribute_value_entry_p->heap_b = heap_b;

   /* Add attribute to the list of datapath attributes */
   OF_APPEND_NODE_TO_LIST(attribute_entry_scan_p->attribute_value_list, attribute_value_entry_p,DPRM_RESOURCE_ATTRIBUTE_VALUE_ENTRY_LISTNODE_OFFSET);
   attribute_entry_scan_p->number_of_attribute_values++;
   return DPRM_SUCCESS;
   
    }
  }

  retval = mempool_get_mem_block(dprm_attributes_mempool_g,(uchar8_t**)&attribute_entry_p,&heap_b);
  if(retval != MEMPOOL_SUCCESS)
    return  DPRM_ERROR_NO_MEM;

  strcpy(attribute_entry_p->name, attribute_info_p->name_string);
  attribute_entry_p->heap_b = heap_b;

  /* Add attribute to the list of datapth attributes */
  OF_APPEND_NODE_TO_LIST((*attributes_p), attribute_entry_p, DPRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET);


  OF_LIST_INIT(attribute_entry_p->attribute_value_list, dprm_attributes_free_attribute_value_entry_rcu);
  retval = mempool_get_mem_block(dprm_attributes_value_mempool_g,(uchar8_t**)&attribute_value_entry_p,&heap_b);
  if(retval != MEMPOOL_SUCCESS)
    return  DPRM_ERROR_NO_MEM;
  strcpy(attribute_value_entry_p->value, attribute_info_p->value_string);
  attribute_value_entry_p->heap_b = heap_b;

  /* Add attr value to the list of  datapath attributes */
  OF_APPEND_NODE_TO_LIST(attribute_entry_p->attribute_value_list, attribute_value_entry_p, DPRM_RESOURCE_ATTRIBUTE_VALUE_ENTRY_LISTNODE_OFFSET);
  attribute_entry_p->number_of_attribute_values++;



  return DPRM_SUCCESS;
}
/*******************************************************************************************************/
int32_t dprm_get_first_attribute_name( of_list_t *attributes_p,
                                 struct dprm_attribute_name_value_output_info  *attribute_output_p)
{ 

   struct   dprm_resource_attribute_entry       *attribute_entry_p=NULL;
   uint8_t  entry_found_b = 0;

   if(attributes_p == NULL)
      return DPRM_ERROR_ATTRIBUTE_NAME_NULL;

   OF_LIST_SCAN(*attributes_p, attribute_entry_p, struct dprm_resource_attribute_entry*, DPRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET )
   {
         entry_found_b = 1;
         break;
   }

   if(entry_found_b == 0 )
      return DPRM_ERROR_ATTRIBUTE_NAME_NULL;

   if(attribute_output_p->name_string == NULL)
      return DPRM_ERROR_ATTRIBUTE_NAME_NULL;

   if(strlen(attribute_entry_p->name) > (attribute_output_p->name_length))
   {
      attribute_output_p->name_length  = strlen(attribute_entry_p->name)  +1;
      return DPRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN;
   }

   strcpy(attribute_output_p->name_string, attribute_entry_p->name);
   attribute_output_p->name_length  = strlen(attribute_entry_p->name)  +1;
   return DPRM_SUCCESS;
}

/*******************************************************************************************************/
int32_t dprm_get_next_attribute_name(of_list_t *attributes,char *current_attribute_name,
                                struct dprm_attribute_name_value_output_info *attribute_output)
{
  uint8_t current_entry_found_b = 0;
  struct  dprm_resource_attribute_entry *attribute_entry_p;
  int32_t retval;

  if((attribute_output == NULL) || (current_attribute_name == NULL))
    return DPRM_FAILURE;

  if(attribute_output->name_string == NULL)
    return DPRM_ERROR_ATTRIBUTE_NAME_NULL;

  if(attribute_output->value_string == NULL)
    return DPRM_ERROR_ATTRIBUTE_VALUE_NULL;

  retval = DPRM_ERROR_NO_MORE_ENTRIES;

  OF_LIST_SCAN(*attributes,attribute_entry_p,struct dprm_resource_attribute_entry*, DPRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET )
  {
    if(current_entry_found_b == 0)
    {
      if(!strcmp(attribute_entry_p->name,current_attribute_name))
      {
        current_entry_found_b = 1;
        continue;
      }
      else
       continue;
    }
    else
    {
    /* skip the first matching attribute name */
      if(strlen(attribute_entry_p->name) > (attribute_output->name_length))
      {
        attribute_output->name_length  = strlen(attribute_entry_p->name) +1;
        retval = DPRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN;
        break;
      }
      strcpy(attribute_output->name_string,attribute_entry_p->name);
      attribute_output->name_length  = strlen(attribute_entry_p->name) +1;

      retval = DPRM_SUCCESS;
      break;

    }
  }
  if(current_entry_found_b == 0)
    retval = DPRM_ERROR_INVALID_NAME;

  return retval;

}
/****************************************************************************************************************/
int32_t dprm_add_attribute_to_switch(uint64_t switch_handle,
                                     struct   dprm_attribute_name_value_pair* attribute_info_p)
{
  int32_t  retval; 
  struct   dprm_switch_entry *switch_entry_p;

  struct   dprm_switch_notification_data   notification_data;
  struct   dprm_notification_app_hook_info *app_entry_p;
  uchar8_t lstoffset;
  lstoffset = DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET;
 

  CNTLR_RCU_READ_LOCK_TAKE();
    
  retval = get_switch_byhandle(switch_handle,&switch_entry_p);
  if(retval != DPRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return retval;
  }
  
  retval = dprm_add_attribute(&(switch_entry_p->attributes),attribute_info_p);
  if(retval != DPRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return retval;
  }

  switch_entry_p->number_of_attributes++;   
  /* Send add attribute to switch notification to registered Applications. */
  strcpy(notification_data.switch_name,switch_entry_p->name);
  strcpy(notification_data.attribute_name,attribute_info_p->name_string);
  strcpy(notification_data.attribute_value,attribute_info_p->value_string);

  #if 1 
  OF_LIST_SCAN(switch_notifications[DPRM_SWITCH_ATTRIBUTE_ADDED],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
  {
    ((dprm_switch_notifier_fn)(app_entry_p->call_back))(DPRM_SWITCH_ATTRIBUTE_ADDED,
                                                        switch_handle,
                                                        notification_data,
                                                        app_entry_p->cbk_arg1,
                                                        app_entry_p->cbk_arg2);
  }

  OF_LIST_SCAN(switch_notifications[DPRM_SWITCH_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
  {
    ((dprm_switch_notifier_fn)(app_entry_p->call_back))(DPRM_SWITCH_ATTRIBUTE_ADDED,
                                                        switch_handle,
                                                        notification_data,
                                                        app_entry_p->cbk_arg1,
                                                        app_entry_p->cbk_arg2);
  }
  #endif
  CNTLR_RCU_READ_LOCK_RELEASE(); 
  return DPRM_SUCCESS; 
}
/****************************************************************************************************/
int32_t dprm_delete_attribute_from_switch(uint64_t switch_handle, struct dprm_attribute_name_value_pair *attribute_info_p)
{
  
  int32_t  retval;
  struct   dprm_switch_entry *switch_entry_p;
  struct   dprm_resource_attribute_entry *attribute_entry_p,*prev_app_entry_p = NULL; 
  struct   dprm_resource_attribute_value_entry *attribute_value_entry_p,*prev_app_value_entry_p=NULL;
  
  struct   dprm_switch_notification_data   notification_data;
  struct   dprm_notification_app_hook_info *app_entry_p;
  uchar8_t lstattrnameoffset,lstattrvaloffset;
  lstattrnameoffset = OFLIST_NODE_OFFSET;
  lstattrvaloffset = OFLIST_NODE_OFFSET;


  CNTLR_RCU_READ_LOCK_TAKE();

  retval = get_switch_byhandle(switch_handle,&switch_entry_p);
  if(retval != DPRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return retval;
  }
  OF_LIST_SCAN(switch_entry_p->attributes,attribute_entry_p,struct dprm_resource_attribute_entry*,lstattrnameoffset)
  {
    if(!strcmp(attribute_entry_p->name, attribute_info_p->name_string))
    {

      strcpy(notification_data.switch_name,switch_entry_p->name);
      strcpy(notification_data.attribute_name,attribute_entry_p->name);
      //strcpy(notification_data.attribute_value,attribute_entry_p->value);
      prev_app_value_entry_p=NULL;
      OF_LIST_SCAN(attribute_entry_p->attribute_value_list, attribute_value_entry_p, struct dprm_resource_attribute_value_entry*,lstattrvaloffset)
       {
     if(!strcmp(attribute_value_entry_p->value, attribute_info_p->value_string))
     {

             strcpy(notification_data.attribute_value,attribute_value_entry_p->value);
       OF_LIST_SCAN(switch_notifications[DPRM_SWITCH_ATTRIBUTE_DELETE],app_entry_p,struct dprm_notification_app_hook_info*, lstattrnameoffset )
            {
              ((dprm_switch_notifier_fn)(app_entry_p->call_back))(DPRM_SWITCH_ATTRIBUTE_DELETE,
                                                            switch_handle,
                                                            notification_data,
                                                            app_entry_p->cbk_arg1,
                                                            app_entry_p->cbk_arg2);
           }

           OF_LIST_SCAN(switch_notifications[DPRM_SWITCH_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,lstattrnameoffset)
      {
              ((dprm_switch_notifier_fn)(app_entry_p->call_back))(DPRM_SWITCH_ATTRIBUTE_DELETE,
                                                            switch_handle,
                                                            notification_data,
                                                            app_entry_p->cbk_arg1,
                                                            app_entry_p->cbk_arg2);
           }

     
          OF_REMOVE_NODE_FROM_LIST(attribute_entry_p->attribute_value_list, attribute_value_entry_p,
               prev_app_value_entry_p,lstattrvaloffset);

      attribute_entry_p->number_of_attribute_values--;   

      
          if(OF_LIST_COUNT(attribute_entry_p->attribute_value_list)== 0)
          {
           
         OF_REMOVE_NODE_FROM_LIST(switch_entry_p->attributes,attribute_entry_p,prev_app_entry_p, lstattrnameoffset);
      

              switch_entry_p->number_of_attributes--;

      }

           
           CNTLR_RCU_READ_LOCK_RELEASE();
           return DPRM_SUCCESS;
     }
        prev_app_value_entry_p = attribute_value_entry_p;
     continue;
       }   
    }
    prev_app_entry_p = attribute_entry_p;
    continue;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_FAILURE;  
}
#if 0
/****************************************************************************************************/
int32_t dprm_get_first_switch_attribute(uint64_t switch_handle,
                                        struct   dprm_attribute_name_value_output_info *attribute_output_p)
{
  int32_t  retval;
  struct   dprm_switch_entry *switch_entry_p;
  struct   dprm_resource_attribute_entry *attribute_entry_p;

  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {  
    retval = get_switch_byhandle(switch_handle,&switch_entry_p);
    if(retval != DPRM_SUCCESS)
      break;
  
    attribute_entry_p = (struct dprm_resource_attribute_entry *)(OF_LIST_FIRST(switch_entry_p->attributes)); 

    if(attribute_entry_p == NULL)
    {
      retval = DPRM_ERROR_NO_MORE_ENTRIES;  
      break;
    }
    retval = dprm_get_first_attribute(attribute_entry_p,attribute_output_p);
  }while(0);
  
  CNTLR_RCU_READ_LOCK_RELEASE();
  return retval;
}  
/****************************************************************************************************/
int32_t dprm_get_next_switch_attribute(uint64_t switch_handle,
                                       char     *current_attribute_name,
                                       struct   dprm_attribute_name_value_output_info *attribute_output)
{
  int32_t  retval;
  struct   dprm_switch_entry *switch_entry_p;

  CNTLR_RCU_READ_LOCK_TAKE();
  do 
  {  
    retval = get_switch_byhandle(switch_handle,&switch_entry_p);
    if(retval != DPRM_SUCCESS)
      break;
    retval = dprm_get_next_attribute(switch_entry_p->attributes,current_attribute_name,attribute_output);
  }while(0);
  
  CNTLR_RCU_READ_LOCK_RELEASE();
  return retval;
}
#endif
/****************************************************************************************************/
int32_t dprm_add_attribute_to_forwarding_domain(uint64_t domain_handle,
                                                struct   dprm_attribute_name_value_pair *attribute_info_p)
{
  int32_t  retval;
  struct   dprm_domain_entry *domain_entry_p;

  struct   dprm_domain_notification_data   notification_data;
  struct   dprm_notification_app_hook_info *app_entry_p;
  uchar8_t lstoffset;
  lstoffset = DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET;

  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    retval = get_domain_byhandle(domain_handle,&domain_entry_p);
    if(retval != DPRM_SUCCESS)
      break;

    retval = dprm_add_attribute(&(domain_entry_p->attributes),attribute_info_p);
    if(retval != DPRM_SUCCESS)
      break;

    domain_entry_p->number_of_attributes++;
    /* Send add attribute to switch notification to registered Applications. */
    strcpy(notification_data.domain_name,domain_entry_p->name);
    strcpy(notification_data.attribute_name,attribute_info_p->name_string);
    strcpy(notification_data.attribute_value,attribute_info_p->value_string);

    OF_LIST_SCAN(domain_notifications[DPRM_DOMAIN_ATTRIBUTE_ADDED],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
    {
      ((dprm_domain_notifier_fn)(app_entry_p->call_back))(DPRM_DOMAIN_ATTRIBUTE_ADDED,
                                                          domain_handle,
                                                          notification_data,
                                                          app_entry_p->cbk_arg1,
                                                          app_entry_p->cbk_arg2);
    }

    OF_LIST_SCAN(domain_notifications[DPRM_DOMAIN_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
    {
      ((dprm_domain_notifier_fn)(app_entry_p->call_back))(DPRM_DOMAIN_ATTRIBUTE_ADDED,
                                                          domain_handle,
                                                          notification_data,
                                                          app_entry_p->cbk_arg1,
                                                          app_entry_p->cbk_arg2);
    }
    retval = DPRM_SUCCESS;
  }while(0);

    CNTLR_RCU_READ_LOCK_RELEASE();
    return retval;
}
/****************************************************************************************************/
int32_t dprm_delete_attribute_from_forwarding_domain(uint64_t domain_handle, struct dprm_attribute_name_value_pair *attribute_info_p)
{
  int32_t  retval;
  struct   dprm_domain_entry *domain_entry_p;
  struct   dprm_resource_attribute_entry *attribute_entry_p,*prev_app_entry_p = NULL;
  struct   dprm_resource_attribute_value_entry *attribute_value_entry_p,*prev_app_value_entry_p=NULL;

  struct   dprm_domain_notification_data   notification_data;
  struct   dprm_notification_app_hook_info *app_entry_p;

  CNTLR_RCU_READ_LOCK_TAKE();

  retval = get_domain_byhandle(domain_handle,&domain_entry_p);
  if(retval != DPRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return retval;
  }
  OF_LIST_SCAN(domain_entry_p->attributes,attribute_entry_p,struct dprm_resource_attribute_entry*,DPRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET )
  {
    if(!strcmp(attribute_entry_p->name, attribute_info_p->name_string))
    {
       strcpy(notification_data.domain_name,domain_entry_p->name);
       strcpy(notification_data.attribute_name,attribute_entry_p->name);
//      strcpy(notification_data.attribute_value,attribute_entry_p->value);
       prev_app_value_entry_p=NULL;   
       OF_LIST_SCAN(attribute_entry_p->attribute_value_list, attribute_value_entry_p, struct dprm_resource_attribute_value_entry*,DPRM_RESOURCE_ATTRIBUTE_VALUE_ENTRY_LISTNODE_OFFSET)
       {
     if(!strcmp(attribute_value_entry_p->value, attribute_info_p->value_string))
     {
             strcpy(notification_data.attribute_value,attribute_value_entry_p->value);
      OF_LIST_SCAN(domain_notifications[DPRM_DOMAIN_ATTRIBUTE_DELETE],app_entry_p,struct dprm_notification_app_hook_info*,DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET )
      {
         ((dprm_domain_notifier_fn)(app_entry_p->call_back))(DPRM_DOMAIN_ATTRIBUTE_DELETE,
         domain_handle,
         notification_data,
         app_entry_p->cbk_arg1,
         app_entry_p->cbk_arg2);
      }
      OF_LIST_SCAN(domain_notifications[DPRM_DOMAIN_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*, DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET )
      {
         ((dprm_domain_notifier_fn)(app_entry_p->call_back))(DPRM_DOMAIN_ATTRIBUTE_DELETE,
         domain_handle,
         notification_data,
         app_entry_p->cbk_arg1,
         app_entry_p->cbk_arg2);
      }
             OF_REMOVE_NODE_FROM_LIST(attribute_entry_p->attribute_value_list, attribute_value_entry_p, prev_app_value_entry_p, DPRM_RESOURCE_ATTRIBUTE_VALUE_ENTRY_LISTNODE_OFFSET );
      
      attribute_entry_p->number_of_attribute_values--;   
      if(OF_LIST_COUNT(attribute_entry_p->attribute_value_list)== 0)
          {
           
         OF_REMOVE_NODE_FROM_LIST(domain_entry_p->attributes,attribute_entry_p,prev_app_entry_p, DPRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET);
              domain_entry_p->number_of_attributes--;

      }

           CNTLR_RCU_READ_LOCK_RELEASE();
        return DPRM_SUCCESS;

     }   
          prev_app_value_entry_p = attribute_value_entry_p;
     continue;   
       }   
   }
    prev_app_entry_p = attribute_entry_p;
    continue;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_FAILURE;
}

#if 0
/********************************************************************************************************/
int32_t dprm_get_first_forwarding_domain_attribute(uint64_t domain_handle,
                                                   struct   dprm_attribute_name_value_output_info *attribute_output_p)
{
  int32_t  retval;
  struct   dprm_domain_entry *domain_entry_p;
  struct   dprm_resource_attribute_entry *attribute_entry_p;

  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    retval = get_domain_byhandle(domain_handle,&domain_entry_p);
    if(retval != DPRM_SUCCESS)
      break;

    attribute_entry_p = (struct dprm_resource_attribute_entry *)(OF_LIST_FIRST(domain_entry_p->attributes));

    if(attribute_entry_p == NULL)
    {
      retval = DPRM_ERROR_NO_MORE_ENTRIES;
       break;
    }
    retval = dprm_get_first_attribute(attribute_entry_p,attribute_output_p);
  }while(0);

  CNTLR_RCU_READ_LOCK_RELEASE();
  return retval;
}
/******************************************************************************************************/
int32_t dprm_get_next_forwarding_domain_attribute(uint64_t domain_handle,
                                                  char     *current_attribute_name,
                                                  struct   dprm_attribute_name_value_output_info *attribute_output)
{
  int32_t  retval;
  struct   dprm_domain_entry *domain_entry_p;

  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    retval = get_domain_byhandle(domain_handle,&domain_entry_p);
    if(retval != DPRM_SUCCESS)
      break;
    retval = dprm_get_next_attribute(domain_entry_p->attributes,current_attribute_name,attribute_output);
  }while(0);

  CNTLR_RCU_READ_LOCK_RELEASE();
  return retval;
}
#endif
/********************************************************************************************************/
int32_t dprm_add_attribute_to_datapath(uint64_t datapath_handle,
                                       struct   dprm_attribute_name_value_pair *attribute_info_p)
{
   int32_t  retval;
   struct   dprm_datapath_entry *datapath_entry_p;

   struct   dprm_datapath_notification_data notification_data;
   struct    dprm_namespace_notification_data namespace_notification_data;
   struct   dprm_notification_app_hook_info *app_entry_p;
   uchar8_t lstoffset;
   lstoffset = DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET;


   CNTLR_RCU_READ_LOCK_TAKE();

   retval = get_datapath_byhandle(datapath_handle,&datapath_entry_p);
   if(retval != DPRM_SUCCESS)
   {
      CNTLR_RCU_READ_LOCK_RELEASE();
      return retval;
   }
   retval = dprm_add_attribute(&(datapath_entry_p->attributes),attribute_info_p);
   if(retval != DPRM_SUCCESS)
   {
      CNTLR_RCU_READ_LOCK_RELEASE();
      return retval;
   }
   datapath_entry_p->number_of_attributes++;
   of_create_view(OF_VIEW_NSNAME_DPID);
   of_create_namespace_dpid_view_entry( datapath_handle, datapath_entry_p->dpid, attribute_info_p->value_string);

   /* Send add attribute to switch notification to registered Applications. */
   notification_data.dpid = datapath_entry_p->dpid;
   strcpy(notification_data.attribute_name,attribute_info_p->name_string);
   strcpy(notification_data.attribute_value,attribute_info_p->value_string);
   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "name %s value %s", attribute_info_p->name_string, attribute_info_p->value_string);
   if (!strcmp( attribute_info_p->name_string,"namespace"))
   {
      strcpy(namespace_notification_data.namespace_name,attribute_info_p->value_string);
      namespace_notification_data.dp_id= datapath_entry_p->dpid;
      /* Send namespace add notification to the registered Applications. */
      OF_LIST_SCAN(namespace_notifications[DPRM_NAMESPACE_DATAPATH_ATTACHED],app_entry_p,struct dprm_notification_app_hook_info*, lstoffset)
      {
         ((dprm_namespace_notifier_fn)(app_entry_p->call_back))(DPRM_NAMESPACE_DATAPATH_ATTACHED,
         namespace_notification_data,
         app_entry_p->cbk_arg1,
         app_entry_p->cbk_arg2);
      }
      OF_LIST_SCAN(namespace_notifications[DPRM_NAMESPACE_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset)
      {
         ((dprm_namespace_notifier_fn)(app_entry_p->call_back))(DPRM_NAMESPACE_DATAPATH_ATTACHED,
         namespace_notification_data,
         app_entry_p->cbk_arg1,
         app_entry_p->cbk_arg2);
      }
   }   

   OF_LIST_SCAN(datapath_notifications[DPRM_DATAPATH_ATTRIBUTE_ADDED],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
   {
      ((dprm_datapath_notifier_fn)(app_entry_p->call_back))(DPRM_DATAPATH_ATTRIBUTE_ADDED,
      datapath_handle,
      notification_data,
      app_entry_p->cbk_arg1,
      app_entry_p->cbk_arg2);
   }

   OF_LIST_SCAN(datapath_notifications[DPRM_DATAPATH_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
   {
      ((dprm_datapath_notifier_fn)(app_entry_p->call_back))(DPRM_DATAPATH_ATTRIBUTE_ADDED,
      datapath_handle,
      notification_data,
      app_entry_p->cbk_arg1,
      app_entry_p->cbk_arg2);
   }
   CNTLR_RCU_READ_LOCK_RELEASE();
   return DPRM_SUCCESS;
}
/*******************************************************************************************************/
int32_t dprm_delete_attribute_from_datapath(uint64_t datapath_handle, struct dprm_attribute_name_value_pair *attribute_info_p)
{
   int32_t  retval;
   struct   dprm_datapath_entry *datapath_entry_p;
   struct   dprm_resource_attribute_entry *attribute_entry_p,*prev_app_entry_p = NULL;
   struct   dprm_resource_attribute_value_entry *attribute_value_entry_p,*prev_app_value_entry_p=NULL;
   struct   dprm_datapath_notification_data  notification_data;
   struct    dprm_namespace_notification_data namespace_notification_data;
   struct   dprm_notification_app_hook_info  *app_entry_p;


   CNTLR_RCU_READ_LOCK_TAKE();

   retval = get_datapath_byhandle(datapath_handle,&datapath_entry_p);
   if(retval != DPRM_SUCCESS)
   {
      CNTLR_RCU_READ_LOCK_RELEASE();
      return retval;
   }

   OF_LIST_SCAN(datapath_entry_p->attributes, attribute_entry_p, struct dprm_resource_attribute_entry*, DPRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET )
   {
      if(!strcmp(attribute_entry_p->name, attribute_info_p->name_string))
      {
         notification_data.dpid = datapath_entry_p->dpid;
         strcpy(notification_data.attribute_name,attribute_entry_p->name);

         OF_LIST_SCAN(attribute_entry_p->attribute_value_list, attribute_value_entry_p, struct dprm_resource_attribute_value_entry*, DPRM_RESOURCE_ATTRIBUTE_VALUE_ENTRY_LISTNODE_OFFSET)
         {
            if(!strcmp(attribute_value_entry_p->value, attribute_info_p->value_string))
            {
               if (!strcmp( attribute_info_p->name_string,"namespace"))
               {
                  strcpy(namespace_notification_data.namespace_name,attribute_info_p->value_string);
                  namespace_notification_data.dp_id= datapath_entry_p->dpid;
                  /* Send namespace add notification to the registered Applications. */
                  OF_LIST_SCAN(namespace_notifications[DPRM_NAMESPACE_DATAPATH_DETACHED],app_entry_p,struct dprm_notification_app_hook_info*,DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET )
                  {
                     ((dprm_namespace_notifier_fn)(app_entry_p->call_back))(DPRM_NAMESPACE_DATAPATH_DETACHED,
                     namespace_notification_data,
                     app_entry_p->cbk_arg1,
                     app_entry_p->cbk_arg2);
                  }
                  OF_LIST_SCAN(namespace_notifications[DPRM_NAMESPACE_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET )
                  {
                     ((dprm_namespace_notifier_fn)(app_entry_p->call_back))(DPRM_NAMESPACE_DATAPATH_DETACHED,
                     namespace_notification_data,
                     app_entry_p->cbk_arg1,
                     app_entry_p->cbk_arg2);
                  }
               }   
               strcpy(notification_data.attribute_value,attribute_value_entry_p->value);
               OF_LIST_SCAN(datapath_notifications[DPRM_DATAPATH_ATTRIBUTE_DELETE],app_entry_p,struct dprm_notification_app_hook_info*,  DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET )
               {
                  ((dprm_datapath_notifier_fn)(app_entry_p->call_back))(DPRM_DATAPATH_ATTRIBUTE_DELETE,
                  datapath_handle,
                  notification_data,
                  app_entry_p->cbk_arg1,
                  app_entry_p->cbk_arg2);
               }
               OF_LIST_SCAN(datapath_notifications[DPRM_DATAPATH_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*, DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET )
               {
                  ((dprm_datapath_notifier_fn)(app_entry_p->call_back))(DPRM_DATAPATH_ATTRIBUTE_DELETE,
                  datapath_handle,
                  notification_data,
                  app_entry_p->cbk_arg1,
                  app_entry_p->cbk_arg2);
               }
               OF_REMOVE_NODE_FROM_LIST(attribute_entry_p->attribute_value_list, attribute_value_entry_p,
                     prev_app_value_entry_p,DPRM_RESOURCE_ATTRIBUTE_VALUE_ENTRY_LISTNODE_OFFSET );
               attribute_entry_p->number_of_attribute_values--;   

               if(OF_LIST_COUNT(attribute_entry_p->attribute_value_list)== 0)
               {
                  OF_REMOVE_NODE_FROM_LIST(datapath_entry_p->attributes,attribute_entry_p,prev_app_entry_p, DPRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET );
                  datapath_entry_p->number_of_attributes--;
               }
               CNTLR_RCU_READ_LOCK_RELEASE();
               return DPRM_SUCCESS;
            }   
            prev_app_value_entry_p = attribute_value_entry_p;
            continue;
         }   

      }
      prev_app_entry_p = attribute_entry_p;
      continue;
   }
   CNTLR_RCU_READ_LOCK_RELEASE();
   return DPRM_FAILURE;
}

/*******************************************************************************************************/

int32_t dprm_delete_all_attribute_values_from_datapath(uint64_t datapath_handle, struct dprm_attribute_name_value_pair *attribute_info_p)
{
   int32_t  retval;
   struct   dprm_datapath_entry *datapath_entry_p;
   struct   dprm_resource_attribute_entry *attribute_entry_p,*prev_app_entry_p = NULL;
   struct   dprm_resource_attribute_value_entry *attribute_value_entry_p,*prev_app_value_entry_p=NULL;
   struct   dprm_datapath_notification_data  notification_data;
   struct    dprm_namespace_notification_data namespace_notification_data;
   struct   dprm_notification_app_hook_info  *app_entry_p;


   CNTLR_RCU_READ_LOCK_TAKE();

   retval = get_datapath_byhandle(datapath_handle,&datapath_entry_p);
   if(retval != DPRM_SUCCESS)
   {
      CNTLR_RCU_READ_LOCK_RELEASE();
      return retval;
   }

   OF_LIST_SCAN(datapath_entry_p->attributes, attribute_entry_p, struct dprm_resource_attribute_entry*, DPRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET )
   {
      if(!strcmp(attribute_entry_p->name, attribute_info_p->name_string)){
       OF_LOG_MSG(OF_LOG_MOD,OF_LOG_DEBUG,"Entered name is %s",attribute_entry_p->name);
      
         notification_data.dpid = datapath_entry_p->dpid;
         strcpy(notification_data.attribute_name,attribute_entry_p->name);

         OF_LIST_SCAN(attribute_entry_p->attribute_value_list, attribute_value_entry_p, struct dprm_resource_attribute_value_entry*, DPRM_RESOURCE_ATTRIBUTE_VALUE_ENTRY_LISTNODE_OFFSET)
         {
          //  if(!strcmp(attribute_value_entry_p->value, attribute_info_p->value_string))
           // {
               if (!strcmp( attribute_info_p->name_string,"namespace"))
               {
                  strcpy(namespace_notification_data.namespace_name,attribute_info_p->value_string);
                  namespace_notification_data.dp_id= datapath_entry_p->dpid;
                  /* Send namespace add notification to the registered Applications. */
                  OF_LIST_SCAN(namespace_notifications[DPRM_NAMESPACE_DATAPATH_DETACHED],app_entry_p,struct dprm_notification_app_hook_info*,DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET )
                  {
                     ((dprm_namespace_notifier_fn)(app_entry_p->call_back))(DPRM_NAMESPACE_DATAPATH_DETACHED,
                     namespace_notification_data,
                     app_entry_p->cbk_arg1,
                     app_entry_p->cbk_arg2);
                  }
                  OF_LIST_SCAN(namespace_notifications[DPRM_NAMESPACE_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET )
                  {
                     ((dprm_namespace_notifier_fn)(app_entry_p->call_back))(DPRM_NAMESPACE_DATAPATH_DETACHED,
                     namespace_notification_data,
                     app_entry_p->cbk_arg1,
                     app_entry_p->cbk_arg2);
                  }
               }   
               strcpy(notification_data.attribute_value,attribute_value_entry_p->value);
               OF_LIST_SCAN(datapath_notifications[DPRM_DATAPATH_ATTRIBUTE_DELETE],app_entry_p,struct dprm_notification_app_hook_info*,  DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET )
               {
                  ((dprm_datapath_notifier_fn)(app_entry_p->call_back))(DPRM_DATAPATH_ATTRIBUTE_DELETE,
                  datapath_handle,
                  notification_data,
                  app_entry_p->cbk_arg1,
                  app_entry_p->cbk_arg2);
               }
               OF_LIST_SCAN(datapath_notifications[DPRM_DATAPATH_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*, DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET )
               {
                  ((dprm_datapath_notifier_fn)(app_entry_p->call_back))(DPRM_DATAPATH_ATTRIBUTE_DELETE,
                  datapath_handle,
                  notification_data,
                  app_entry_p->cbk_arg1,
                  app_entry_p->cbk_arg2);
               }
               OF_REMOVE_NODE_FROM_LIST(attribute_entry_p->attribute_value_list, attribute_value_entry_p,
                     prev_app_value_entry_p,DPRM_RESOURCE_ATTRIBUTE_VALUE_ENTRY_LISTNODE_OFFSET );
               attribute_entry_p->number_of_attribute_values--;   
 	#if 0	
               if(OF_LIST_COUNT(attribute_entry_p->attribute_value_list)== 0)
               {
                  OF_REMOVE_NODE_FROM_LIST(datapath_entry_p->attributes,attribute_entry_p,prev_app_entry_p, DPRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET );
                  datapath_entry_p->number_of_attributes--;
              CNTLR_RCU_READ_LOCK_RELEASE();
              return DPRM_SUCCESS;
               }
	#endif
           // }   
            prev_app_value_entry_p = attribute_value_entry_p;
         }
	 if(OF_LIST_COUNT(attribute_entry_p->attribute_value_list)== 0)
               {
                OF_REMOVE_NODE_FROM_LIST(datapath_entry_p->attributes,attribute_entry_p,prev_app_entry_p,
			 DPRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET );
                datapath_entry_p->number_of_attributes--;
                CNTLR_RCU_READ_LOCK_RELEASE();
                return DPRM_SUCCESS;
               }   

      }
      prev_app_entry_p = attribute_entry_p;
   }
   CNTLR_RCU_READ_LOCK_RELEASE();
   return DPRM_SUCCESS;
}  
/*******************************************************************************************************/

/*******************************************************************************************************/
int32_t dprm_get_datapath_first_attribute_name(uint64_t datapath_handle,
      struct   dprm_attribute_name_value_output_info *attribute_output_p)
{
   int32_t  retval;
struct dprm_datapath_entry *datapath_entry_p;

   if(attribute_output_p == NULL)
      return DPRM_FAILURE;



   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_datapath_byhandle(datapath_handle, &datapath_entry_p);
      if(retval != DPRM_SUCCESS)
      {
         CNTLR_RCU_READ_LOCK_RELEASE();
         return DPRM_ERROR_INVALID_DATAPATH_HANDLE;
      }
#if 0
      attribute_entry_p = (struct dprm_resource_attribute_entry *)(OF_LIST_FIRST(datapath_entry_p->attributes));

      if(attribute_entry_p == NULL)
      {
         retval= DPRM_ERROR_NO_MORE_ENTRIES;
         break;   
      }
#endif
      retval = dprm_get_first_attribute_name(&(datapath_entry_p->attributes), attribute_output_p);   

   }while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return retval;
}
/****************************************************************************************/
int32_t dprm_get_switch_first_attribute_name(uint64_t switch_handle,
      struct   dprm_attribute_name_value_output_info *attribute_output_p)
{
  int32_t  retval;
  struct   dprm_switch_entry *switch_entry_p;

  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {  
    retval = get_switch_byhandle(switch_handle,&switch_entry_p);
    if(retval != DPRM_SUCCESS)
    {
      CNTLR_RCU_READ_LOCK_RELEASE();
      return DPRM_ERROR_INVALID_SWITCH_HANDLE;
    }
 #if 0 
    attribute_entry_p = (struct dprm_resource_attribute_entry *)(OF_LIST_FIRST(switch_entry_p->attributes)); 

    if(attribute_entry_p == NULL)
    {
      retval = DPRM_ERROR_NO_MORE_ENTRIES;  
      break;
    }
#endif
    retval = dprm_get_first_attribute_name(&(switch_entry_p->attributes),attribute_output_p);
  }while(0);
  
  CNTLR_RCU_READ_LOCK_RELEASE();
  return retval;

}
/**************************************************************************************/
int32_t dprm_get_domain_first_attribute_name(uint64_t domain_handle,
      struct   dprm_attribute_name_value_output_info *attribute_output_p)
{
  int32_t  retval;
  struct   dprm_domain_entry *domain_entry_p;

  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {  
    retval = get_domain_byhandle(domain_handle, &domain_entry_p);
    if(retval != DPRM_SUCCESS)
      break;
 #if 0 
    attribute_entry_p = (struct dprm_resource_attribute_entry *)(OF_LIST_FIRST(domain_entry_p->attributes)); 

    if(attribute_entry_p == NULL)
    {
      retval = DPRM_ERROR_NO_MORE_ENTRIES;  
      break;
    }
#endif
    retval = dprm_get_first_attribute_name(&(domain_entry_p->attributes),attribute_output_p);
  }while(0);
  
  CNTLR_RCU_READ_LOCK_RELEASE();
  return retval;

}
/***************************************************************************************************/
int32_t dprm_get_datapath_port_first_attribute_name(uint64_t datapath_handle,
                                               uint64_t port_handle,
                                               struct   dprm_attribute_name_value_output_info *attribute_output_p)
{
   int32_t  retval;
   struct   dprm_port_entry *port_entry_p;

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_datapath_port_byhandle(datapath_handle,port_handle,&port_entry_p);
      if(retval != DPRM_SUCCESS)
         break;
#if 0
      attribute_entry_p = (struct dprm_resource_attribute_entry *)(OF_LIST_FIRST(port_entry_p->attributes));
      if(attribute_entry_p == NULL)
      {
         retval = DPRM_ERROR_NO_MORE_ENTRIES;
         break;
      }
#endif
      retval = dprm_get_first_attribute_name(&(port_entry_p->attributes),attribute_output_p);
   }while(0);

   CNTLR_RCU_READ_LOCK_RELEASE();
   return retval;
}
/****************************************************************************************************/
int32_t dprm_get_datapath_attribute_first_value(uint64_t datapath_handle,
      struct   dprm_attribute_name_value_output_info *attribute_output_p)
{
   int32_t  retval=DPRM_FAILURE;
   struct   dprm_datapath_entry *datapath_entry_p=NULL;


   if(attribute_output_p == NULL)
      return DPRM_FAILURE;


   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
	   retval = get_datapath_byhandle(datapath_handle, &datapath_entry_p);
	   if(retval != DPRM_SUCCESS)
		   break;
      retval = dprm_get_attribute_first_value(&(datapath_entry_p->attributes), attribute_output_p);

   }while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return retval;


}
/****************************************************************************************************/
int32_t dprm_get_switch_attribute_first_value(uint64_t switch_handle,
      struct   dprm_attribute_name_value_output_info *attribute_output_p)
{
   int32_t  retval=DPRM_FAILURE;
   struct   dprm_switch_entry *switch_entry_p=NULL;

   if(attribute_output_p == NULL)
      return DPRM_FAILURE;


   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_switch_byhandle(switch_handle, &switch_entry_p);
      if(retval != DPRM_SUCCESS)
         break;
      retval = dprm_get_attribute_first_value(&(switch_entry_p->attributes), attribute_output_p);

   }while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return retval;
}
/****************************************************************************************************/
int32_t dprm_get_domain_attribute_first_value(uint64_t domain_handle,
      struct   dprm_attribute_name_value_output_info *attribute_output_p)
{
   int32_t  retval=DPRM_FAILURE;
   struct   dprm_domain_entry *domain_entry_p=NULL;

   if(attribute_output_p == NULL)
      return DPRM_FAILURE;


   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_domain_byhandle(domain_handle, &domain_entry_p);
      if(retval != DPRM_SUCCESS)
         break;
      retval = dprm_get_attribute_first_value(&(domain_entry_p->attributes), attribute_output_p);

   }while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return retval;
}
/**************************************************************************************/
int32_t dprm_get_datapath_port_attribute_first_value(uint64_t datapath_handle, uint64_t port_handle,
      struct   dprm_attribute_name_value_output_info *attribute_output_p)
{
   int32_t  retval=DPRM_FAILURE;
   struct   dprm_port_entry *port_entry_p=NULL;

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_datapath_port_byhandle(datapath_handle, port_handle, &port_entry_p);
      if(retval != DPRM_SUCCESS)
         break;

      retval = dprm_get_attribute_first_value(&(port_entry_p->attributes), attribute_output_p);
   }while(0);

   CNTLR_RCU_READ_LOCK_RELEASE();
   return retval;

}
/******************************************************************************/
int32_t dprm_get_attribute_first_value(of_list_t *attributes_p, 
      struct dprm_attribute_name_value_output_info *attribute_output_p)
{
	uint8_t  current_entry_found_b = 0;
	struct   dprm_resource_attribute_entry       *attribute_entry_p=NULL;
	struct   dprm_resource_attribute_value_entry *attribute_value_entry_p=NULL;

	OF_LIST_SCAN(*attributes_p, attribute_entry_p, struct dprm_resource_attribute_entry*, DPRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET )
	{
		if(!strcmp(attribute_entry_p->name, attribute_output_p->name_string))
		{
			current_entry_found_b = 1;
			break;
		}
	}
	if(current_entry_found_b == 0)
	{
		OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "no entries found ");
		return DPRM_ERROR_INVALID_NAME;
	}
	if(attribute_entry_p == NULL)
	{
		return DPRM_ERROR_NO_MORE_ENTRIES;
	}


	current_entry_found_b = 0;
	OF_LIST_SCAN(attribute_entry_p->attribute_value_list, attribute_value_entry_p, struct dprm_resource_attribute_value_entry*, DPRM_RESOURCE_ATTRIBUTE_VALUE_ENTRY_LISTNODE_OFFSET )
	{
		current_entry_found_b = 1;
		break;
	}

	if(current_entry_found_b == 0)
	{
		OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "no entries found ");
		return DPRM_ERROR_INVALID_NAME;
	}

	if(attribute_value_entry_p == NULL)
	{
		OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "value entry null");
		return DPRM_ERROR_NO_MORE_ENTRIES;
	}
	if(strlen(attribute_value_entry_p->value) > (attribute_output_p->value_length))
	{
		attribute_output_p->value_length  = strlen(attribute_value_entry_p->value)  +1;
		return DPRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN;
	}
	strcpy(attribute_output_p->value_string, attribute_value_entry_p->value);
	attribute_output_p->value_length  = strlen(attribute_value_entry_p->value)  +1;   
	return DPRM_SUCCESS;
}

/*****************************************************************************************/
int32_t dprm_get_datapath_attribute_next_value(uint64_t datapath_handle, char *current_attribute_name, char *current_attribute_value, struct   dprm_attribute_name_value_output_info *attribute_output_p)
{
   struct   dprm_datapath_entry *datapath_entry_p=NULL;
   int32_t retval;


   if(attribute_output_p == NULL)
      return DPRM_FAILURE;

   if(current_attribute_name == NULL)
      return DPRM_FAILURE;   

   if(current_attribute_value == NULL)
      return DPRM_FAILURE;   



   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_datapath_byhandle(datapath_handle, &datapath_entry_p);
      if(retval != DPRM_SUCCESS)
         break;

      retval = dprm_get_attribute_next_value(&(datapath_entry_p->attributes), current_attribute_name, current_attribute_value,attribute_output_p);   
   }while(0);

   CNTLR_RCU_READ_LOCK_RELEASE();
   return retval;



}
/****************************************************************************************************/
int32_t dprm_get_switch_attribute_next_value(uint64_t switch_handle, char *current_attribute_name, char *current_attribute_value, struct   dprm_attribute_name_value_output_info *attribute_output_p)
{
   struct   dprm_switch_entry *switch_entry_p=NULL;
   int32_t retval;


   if(attribute_output_p == NULL)
      return DPRM_FAILURE;

   if(current_attribute_name == NULL)
      return DPRM_FAILURE;   

   if(current_attribute_value == NULL)
      return DPRM_FAILURE;   



   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_switch_byhandle(switch_handle, &switch_entry_p);
      if(retval != DPRM_SUCCESS)
         break;

      retval = dprm_get_attribute_next_value(&(switch_entry_p->attributes), current_attribute_name, current_attribute_value,attribute_output_p);   
   }while(0);

   CNTLR_RCU_READ_LOCK_RELEASE();
   return retval;



}
/****************************************************************************************************/
int32_t dprm_get_domain_attribute_next_value(uint64_t domain_handle, char *current_attribute_name, char *current_attribute_value, struct   dprm_attribute_name_value_output_info *attribute_output_p)
{
   struct   dprm_domain_entry *domain_entry_p=NULL;
   int32_t retval;


   if(attribute_output_p == NULL)
      return DPRM_FAILURE;

   if(current_attribute_name == NULL)
      return DPRM_FAILURE;   

   if(current_attribute_value == NULL)
      return DPRM_FAILURE;   



   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_domain_byhandle(domain_handle, &domain_entry_p);
      if(retval != DPRM_SUCCESS)
         break;

      retval = dprm_get_attribute_next_value(&(domain_entry_p->attributes), current_attribute_name, current_attribute_value,attribute_output_p);   
   }while(0);

   CNTLR_RCU_READ_LOCK_RELEASE();
   return retval;



}
/***************************************************************************************************/
int32_t dprm_get_datapath_port_attribute_next_value(uint64_t datapath_handle, uint64_t port_handle, char *current_attribute_name, char *current_attribute_value, struct   dprm_attribute_name_value_output_info *attribute_output_p)
{
   struct   dprm_port_entry *port_entry_p=NULL;
   int32_t retval;


   if(attribute_output_p == NULL)
      return DPRM_FAILURE;

   if(current_attribute_name == NULL)
      return DPRM_FAILURE;   

   if(current_attribute_value == NULL)
      return DPRM_FAILURE;   



   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_datapath_port_byhandle(datapath_handle, port_handle, &port_entry_p);
      if(retval != DPRM_SUCCESS)
         break;
      retval = dprm_get_attribute_next_value(&(port_entry_p->attributes), current_attribute_name, current_attribute_value, attribute_output_p);   
   }while(0);

   CNTLR_RCU_READ_LOCK_RELEASE();
   return retval;
}

/*****************************************************************************************************/
int32_t dprm_get_attribute_next_value(of_list_t *attributes_p,char *current_attribute_name, char *current_attribute_value, struct dprm_attribute_name_value_output_info *attribute_output_p)
{
   uint8_t  current_entry_found_b = 0;
   struct   dprm_resource_attribute_entry       *attribute_entry_p=NULL;
   struct   dprm_resource_attribute_value_entry *attribute_value_entry_p=NULL;
   int32_t  retval;


   OF_LIST_SCAN(*attributes_p, attribute_entry_p, struct dprm_resource_attribute_entry*, DPRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET )
   {
      if(!strcmp(attribute_entry_p->name, current_attribute_name))
      {
         current_entry_found_b = 1;
         break;
      }
   }
   if((attribute_entry_p == NULL)||(current_entry_found_b == 0))
   {
      return DPRM_ERROR_INVALID_NAME;
   }

   retval = DPRM_ERROR_NO_MORE_ENTRIES; 
   current_entry_found_b = 0;
   OF_LIST_SCAN(attribute_entry_p->attribute_value_list, attribute_value_entry_p, struct dprm_resource_attribute_value_entry*, DPRM_RESOURCE_ATTRIBUTE_VALUE_ENTRY_LISTNODE_OFFSET )
   {
      if(current_entry_found_b == 0)
      {
         if(!strcmp(attribute_value_entry_p->value, current_attribute_value))
         {
            current_entry_found_b = 1;
            continue;
         }
         else
            continue;
      }
      else
      {
         if(strlen(attribute_value_entry_p->value) > attribute_output_p->value_length)
         {
            attribute_output_p->value_length = strlen(attribute_value_entry_p->value) + 1;
            retval = DPRM_ERROR_INVALID_ATTRIBUTE_VALUE_LEN;
            break;
         }
         strcpy(attribute_output_p->value_string,attribute_value_entry_p->value);
         attribute_output_p->value_length = strlen(attribute_value_entry_p->value) +1;;

         retval = DPRM_SUCCESS;
         break;
      }   
   }
   if(current_entry_found_b == 0)
   {
      return DPRM_ERROR_ATTRIBUTE_VALUE_NULL;
   }     
   return retval;
} 
/*******************************************************************************************************/
int32_t dprm_get_datapath_next_attribute_name(uint64_t datapath_handle,
      char*    current_attribute_name,
      struct   dprm_attribute_name_value_output_info *attribute_output_p)
{
   int32_t retval;
   struct   dprm_datapath_entry *datapath_entry_p;

   if((attribute_output_p == NULL) || (current_attribute_name == NULL))
      return DPRM_FAILURE;

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_datapath_byhandle(datapath_handle,&datapath_entry_p);
      if(retval != DPRM_SUCCESS)
      {

         CNTLR_RCU_READ_LOCK_RELEASE();
         return DPRM_ERROR_INVALID_DATAPATH_HANDLE;
      }

      retval = dprm_get_next_attribute_name(&(datapath_entry_p->attributes), current_attribute_name, attribute_output_p);   
   }while(0);

   CNTLR_RCU_READ_LOCK_RELEASE();
   return retval;
}
/*****************************************************************************************************/
int32_t dprm_get_switch_next_attribute_name(uint64_t switch_handle,
      char*    current_attribute_name,
      struct   dprm_attribute_name_value_output_info *attribute_output_p)
{
   int32_t retval;
   struct   dprm_switch_entry *switch_entry_p;

   if((attribute_output_p == NULL) || (current_attribute_name == NULL))
      return DPRM_FAILURE;

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_switch_byhandle(switch_handle,&switch_entry_p);
      if(retval != DPRM_SUCCESS)
      {
         CNTLR_RCU_READ_LOCK_RELEASE();
         return DPRM_ERROR_INVALID_DATAPATH_HANDLE;
      }

      retval = dprm_get_next_attribute_name(&(switch_entry_p->attributes), current_attribute_name, attribute_output_p);   
   }while(0);

   CNTLR_RCU_READ_LOCK_RELEASE();
   return retval;
}
/*****************************************************************************************************/
int32_t dprm_get_domain_next_attribute_name(uint64_t domain_handle,
      char*    current_attribute_name,
      struct   dprm_attribute_name_value_output_info *attribute_output_p)
{
   int32_t retval;
   struct   dprm_domain_entry *domain_entry_p;

   if((attribute_output_p == NULL) || (current_attribute_name == NULL))
      return DPRM_FAILURE;

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_domain_byhandle(domain_handle,&domain_entry_p);
      if(retval != DPRM_SUCCESS)
      {
         CNTLR_RCU_READ_LOCK_RELEASE();
         return DPRM_ERROR_INVALID_DOMAIN_HANDLE;
      }

      retval = dprm_get_next_attribute_name(&(domain_entry_p->attributes), current_attribute_name, attribute_output_p);   
   }while(0);

   CNTLR_RCU_READ_LOCK_RELEASE();
   return retval;
}
/*****************************************************************************************************/
int32_t dprm_get_datapath_port_next_attribute_name(uint64_t datapath_handle, uint64_t port_handle, 
char *current_attribute_name, struct   dprm_attribute_name_value_output_info *attribute_output_p)
{
   int32_t retval=OF_FAILURE;
   struct   dprm_port_entry *port_entry_p=NULL;

   if((attribute_output_p == NULL) || (current_attribute_name == NULL))
      return DPRM_FAILURE;

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_datapath_port_byhandle(datapath_handle,
                      port_handle,
                      &port_entry_p);
      if(retval != DPRM_SUCCESS)
      {
         CNTLR_RCU_READ_LOCK_RELEASE();
         return DPRM_ERROR_INVALID_PORT_HANDLE;
      }

      retval = dprm_get_next_attribute_name(&(port_entry_p->attributes), 
                        current_attribute_name, 
                        attribute_output_p);   
   }while(0);

   CNTLR_RCU_READ_LOCK_RELEASE();
   return retval;
}

/*****************************************************************************************************/
int32_t dprm_get_exact_attribute_name(of_list_t *attributes_p,
      char  *attribute_name_string)
{
   struct   dprm_resource_attribute_entry *attribute_entry_scan_p=NULL;
   uchar8_t lstoffset = DPRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET;

   OF_LIST_SCAN(*attributes_p, attribute_entry_scan_p, struct dprm_resource_attribute_entry*, lstoffset )
   {
      if(!strcmp(attribute_entry_scan_p->name, attribute_name_string))
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "attribute name found ");
         return DPRM_SUCCESS;
      }
   }
   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "attribute name not found ");
   return DPRM_FAILURE;
}
/**************************************************************************************************/
int32_t dprm_get_exact_attribute_value(of_list_t *attributes_p, char *attribute_name_string,
      char  *attribute_name_value)
{
   struct   dprm_resource_attribute_entry *attribute_entry_scan_p=NULL;
   struct dprm_resource_attribute_value_entry *attribute_value_entry_p=NULL;


   OF_LIST_SCAN(*attributes_p, attribute_entry_scan_p, struct dprm_resource_attribute_entry*, DPRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET )
   {
      if(!strcmp(attribute_entry_scan_p->name, attribute_name_string))
      {
         OF_LIST_SCAN(attribute_entry_scan_p->attribute_value_list, attribute_value_entry_p, struct dprm_resource_attribute_value_entry*, DPRM_RESOURCE_ATTRIBUTE_VALUE_ENTRY_LISTNODE_OFFSET )
         {
            if(!strcmp(attribute_value_entry_p->value, attribute_name_value))
            {
               return DPRM_SUCCESS;
            }
         }
      }      
      
   }
   return DPRM_FAILURE;
}

/******************************************************************************/
int32_t dprm_get_datapath_exact_attribute_name(uint64_t datapath_handle, 
      char *attribute_name_string)
{
   struct   dprm_datapath_entry *datapath_entry_p=NULL;
   int32_t  retval;

   /* Check whether name of the attribute is unique within the switch */


        if(attribute_name_string == NULL)
       return DPRM_ERROR_ATTRIBUTE_NAME_NULL;

   if(strlen(attribute_name_string) > DPRM_MAX_ATTRIBUTE_NAME_LEN)
      return DPRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN;

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {

      retval = get_datapath_byhandle(datapath_handle,&datapath_entry_p);
      if(retval != DPRM_SUCCESS)
      {
         CNTLR_RCU_READ_LOCK_RELEASE();
         return DPRM_ERROR_INVALID_DATAPATH_HANDLE;
      }
      retval = dprm_get_exact_attribute_name(&(datapath_entry_p->attributes),  attribute_name_string);   
   }while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return retval;
}  
/*******************************************************************************************************/
int32_t dprm_get_switch_exact_attribute_name(uint64_t switch_handle, 
      char *attribute_name_string)
{
   struct   dprm_switch_entry *switch_entry_p=NULL;
   int32_t  retval;

   

   /* Check whether name of the attribute is unique within the switch */
   if(attribute_name_string == NULL)
       return DPRM_ERROR_ATTRIBUTE_NAME_NULL;

   if(strlen(attribute_name_string) > DPRM_MAX_ATTRIBUTE_NAME_LEN)
      return DPRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN;

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {

      retval = get_switch_byhandle(switch_handle, &switch_entry_p);
      if(retval != DPRM_SUCCESS)
      {
         CNTLR_RCU_READ_LOCK_RELEASE();
         return DPRM_ERROR_INVALID_DATAPATH_HANDLE;
      }
      retval = dprm_get_exact_attribute_name(&(switch_entry_p->attributes),  attribute_name_string);   
   }while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return retval;
}
/******************************************************************************************************/
int32_t dprm_get_domain_exact_attribute_name(uint64_t domain_handle, 
      char *attribute_name_string)
{
   struct   dprm_domain_entry *domain_entry_p=NULL;
   int32_t  retval;

   

   /* Check whether name of the attribute is unique within the switch */
   if(attribute_name_string == NULL)
       return DPRM_ERROR_ATTRIBUTE_NAME_NULL;

   if(strlen(attribute_name_string) > DPRM_MAX_ATTRIBUTE_NAME_LEN)
      return DPRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN;

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {

      retval = get_domain_byhandle(domain_handle, &domain_entry_p);
      if(retval != DPRM_SUCCESS)
      {
         CNTLR_RCU_READ_LOCK_RELEASE();
         return DPRM_ERROR_INVALID_DOMAIN_HANDLE;
      }
      retval = dprm_get_exact_attribute_name(&(domain_entry_p->attributes),  attribute_name_string);   
   }while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return retval;
}
/******************************************************************************************************/
int32_t dprm_get_datapath_port_exact_attribute_name(uint64_t datapath_handle, uint64_t port_handle,
      char *attribute_name_string)
{
   struct   dprm_port_entry *port_entry_p=NULL;
   int32_t  retval =OF_FAILURE;

   

   /* Check whether name of the attribute is unique within the switch */
   if(attribute_name_string == NULL)
       return DPRM_ERROR_ATTRIBUTE_NAME_NULL;

   if(strlen(attribute_name_string) > DPRM_MAX_ATTRIBUTE_NAME_LEN)
      return DPRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN;

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {

      retval = get_datapath_port_byhandle(datapath_handle, 
                      port_handle, 
                     &port_entry_p);
      if(retval != DPRM_SUCCESS)
      {
         CNTLR_RCU_READ_LOCK_RELEASE();
         return DPRM_ERROR_INVALID_DOMAIN_HANDLE;
      }

      retval = dprm_get_exact_attribute_name(&(port_entry_p->attributes),  attribute_name_string);   
   }while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return retval;
}
/*******************************************************************************************************/
int32_t dprm_add_attribute_to_datapath_port(uint64_t datapath_handle,
                                            uint64_t port_handle,
                                            struct   dprm_attribute_name_value_pair *attribute_info_p)
{
   int32_t  retval;
   struct   dprm_port_entry *port_entry_p;
   struct   dprm_datapath_entry *datapath_entry_p;
   struct    dprm_namespace_notification_data namespace_notification_data;
   struct   dprm_datapath_notification_data notification_data;
   struct   dprm_notification_app_hook_info *app_entry_p;
   uchar8_t lstoffset;
   lstoffset = DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET;

   CNTLR_RCU_READ_LOCK_TAKE();

   retval = get_datapath_byhandle(datapath_handle,&datapath_entry_p);
   if(retval != DPRM_SUCCESS)
   {
      CNTLR_RCU_READ_LOCK_RELEASE();
      return retval;
   }
   retval = get_datapath_port_byhandle(datapath_handle,port_handle,&port_entry_p);
   if(retval != DPRM_SUCCESS)
   {
      CNTLR_RCU_READ_LOCK_RELEASE();
      return retval;
   }
   retval = dprm_add_attribute(&(port_entry_p->attributes),attribute_info_p);
   if(retval != DPRM_SUCCESS)
   {
      CNTLR_RCU_READ_LOCK_RELEASE();
      return retval;
   }
   port_entry_p->number_of_attributes++;
   /* Send add attribute to port notification to registered Applications. */
   strcpy(notification_data.port_name,port_entry_p->port_name);
   notification_data.port_id = port_entry_p->port_id;
   notification_data.port_handle = port_handle;
   strcpy(notification_data.attribute_name,attribute_info_p->name_string);
   strcpy(notification_data.attribute_value,attribute_info_p->value_string);

   if (!strcmp( attribute_info_p->name_string,"namespace"))
   {
      strcpy(namespace_notification_data.namespace_name,attribute_info_p->value_string);
      strcpy(namespace_notification_data.port_name,port_entry_p->port_name);
      namespace_notification_data.dp_id= datapath_entry_p->dpid;
      /* Send namespace add notification to the registered Applications. */
      OF_LIST_SCAN(namespace_notifications[DPRM_NAMESPACE_PORT_ATTACHED],app_entry_p,struct dprm_notification_app_hook_info*, lstoffset)
      {
         ((dprm_namespace_notifier_fn)(app_entry_p->call_back))(DPRM_NAMESPACE_PORT_ATTACHED,
         namespace_notification_data,
         app_entry_p->cbk_arg1,
         app_entry_p->cbk_arg2);
      }
      OF_LIST_SCAN(namespace_notifications[DPRM_NAMESPACE_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*, lstoffset)
      {
         ((dprm_namespace_notifier_fn)(app_entry_p->call_back))(DPRM_NAMESPACE_PORT_ATTACHED,
         namespace_notification_data,
         app_entry_p->cbk_arg1,
         app_entry_p->cbk_arg2);
      }
   }

   OF_LIST_SCAN(datapath_notifications[DPRM_DATAPATH_PORT_ATTRIBUTE_ADDED],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
   {
      ((dprm_datapath_notifier_fn)(app_entry_p->call_back))(DPRM_DATAPATH_PORT_ATTRIBUTE_ADDED,
      datapath_handle,
      notification_data,
      app_entry_p->cbk_arg1,
      app_entry_p->cbk_arg2);
   }

   OF_LIST_SCAN(datapath_notifications[DPRM_DATAPATH_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,lstoffset )
   {
      ((dprm_datapath_notifier_fn)(app_entry_p->call_back))(DPRM_DATAPATH_PORT_ATTRIBUTE_ADDED,
      datapath_handle,
      notification_data,
      app_entry_p->cbk_arg1,
      app_entry_p->cbk_arg2);
   }
   CNTLR_RCU_READ_LOCK_RELEASE();
   return DPRM_SUCCESS;
}  
/******************************************************************************************************/
int32_t dprm_delete_attribute_from_datapath_port(uint64_t datapath_handle,
      uint64_t port_handle,
      struct dprm_attribute_name_value_pair *attribute_info_p)
{
   int32_t  retval;
   struct   dprm_port_entry *port_entry_p;
   struct   dprm_resource_attribute_entry *attribute_entry_p,*prev_app_entry_p = NULL;
   struct   dprm_resource_attribute_value_entry *attribute_value_entry_p,*prev_app_value_entry_p=NULL;
   struct   dprm_datapath_entry *datapath_entry_p;
   struct    dprm_namespace_notification_data namespace_notification_data;

   struct   dprm_datapath_notification_data  notification_data;
   struct   dprm_notification_app_hook_info  *app_entry_p;

   CNTLR_RCU_READ_LOCK_TAKE();

   retval = get_datapath_byhandle(datapath_handle,&datapath_entry_p);
   if(retval != DPRM_SUCCESS)
   {
      CNTLR_RCU_READ_LOCK_RELEASE();
      return retval;
   }
   retval = get_datapath_port_byhandle(datapath_handle,port_handle,&port_entry_p);
   if(retval != DPRM_SUCCESS)
   {
      CNTLR_RCU_READ_LOCK_RELEASE();
      return retval;
   }

   OF_LIST_SCAN(port_entry_p->attributes,attribute_entry_p,struct dprm_resource_attribute_entry*, DPRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET)
   {
      if(!strcmp(attribute_entry_p->name, attribute_info_p->name_string))
      {
         if (!strcmp( attribute_info_p->name_string,"namespace"))
         {
            strcpy(namespace_notification_data.namespace_name,attribute_info_p->value_string);
            strcpy(namespace_notification_data.port_name,port_entry_p->port_name);
            namespace_notification_data.dp_id= datapath_entry_p->dpid;
            /* Send namespace add notification to the registered Applications. */
            OF_LIST_SCAN(namespace_notifications[DPRM_NAMESPACE_PORT_ATTACHED],app_entry_p,struct dprm_notification_app_hook_info*,DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET )
            {
               ((dprm_namespace_notifier_fn)(app_entry_p->call_back))(DPRM_NAMESPACE_PORT_DETACHED,
               namespace_notification_data,
               app_entry_p->cbk_arg1,
               app_entry_p->cbk_arg2);
            }
            OF_LIST_SCAN(namespace_notifications[DPRM_NAMESPACE_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET )
            {
               ((dprm_namespace_notifier_fn)(app_entry_p->call_back))(DPRM_NAMESPACE_PORT_DETACHED,
               namespace_notification_data,
               app_entry_p->cbk_arg1,
               app_entry_p->cbk_arg2);
            }
         }
         strcpy(notification_data.port_name,port_entry_p->port_name);
         notification_data.port_id = port_entry_p->port_id;
         notification_data.port_handle = port_handle;
         strcpy(notification_data.attribute_name,attribute_entry_p->name);
         //      strcpy(notification_data.attribute_value,attribute_entry_p->value);
         prev_app_value_entry_p=NULL;
         OF_LIST_SCAN(attribute_entry_p->attribute_value_list, attribute_value_entry_p, struct dprm_resource_attribute_value_entry*, DPRM_RESOURCE_ATTRIBUTE_VALUE_ENTRY_LISTNODE_OFFSET)
         {
            if(!strcmp(attribute_value_entry_p->value, attribute_info_p->value_string))
            {
               strcpy(notification_data.attribute_value,attribute_value_entry_p->value);
               OF_LIST_SCAN(datapath_notifications[DPRM_DATAPATH_PORT_ATTRIBUTE_DELETE],app_entry_p,struct dprm_notification_app_hook_info*,  DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET )
               {
                  ((dprm_datapath_notifier_fn)(app_entry_p->call_back))(DPRM_DATAPATH_PORT_ATTRIBUTE_DELETE,
                  datapath_handle,
                  notification_data,
                  app_entry_p->cbk_arg1,
                  app_entry_p->cbk_arg2);
               }
               OF_LIST_SCAN(datapath_notifications[DPRM_DATAPATH_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,  DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET )
               {
                  ((dprm_datapath_notifier_fn)(app_entry_p->call_back))(DPRM_DATAPATH_PORT_ATTRIBUTE_DELETE,
                  datapath_handle,
                  notification_data,
                  app_entry_p->cbk_arg1,
                  app_entry_p->cbk_arg2);
               }

               OF_REMOVE_NODE_FROM_LIST(attribute_entry_p->attribute_value_list, attribute_value_entry_p, prev_app_value_entry_p, DPRM_RESOURCE_ATTRIBUTE_VALUE_ENTRY_LISTNODE_OFFSET );

               attribute_entry_p->number_of_attribute_values--;   
               if(OF_LIST_COUNT(attribute_entry_p->attribute_value_list)== 0)
               {
                  OF_REMOVE_NODE_FROM_LIST(port_entry_p->attributes,attribute_entry_p,prev_app_entry_p, DPRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET );
                  port_entry_p->number_of_attributes--;
               }
               CNTLR_RCU_READ_LOCK_RELEASE();
               return DPRM_SUCCESS;

            }   
            prev_app_value_entry_p = attribute_value_entry_p;
            continue;
         }
      }
      prev_app_entry_p = attribute_entry_p;
      continue;
   }
   CNTLR_RCU_READ_LOCK_RELEASE();
   return DPRM_FAILURE;
}  
#if 0
/******************************************************************************************************/
int32_t dprm_get_first_datapath_port_attribute(uint64_t datapath_handle,
                                               uint64_t port_handle,
                                               struct   dprm_attribute_name_value_output_info *attribute_output_p)
{
  int32_t  retval;
  struct   dprm_port_entry *port_entry_p;
  struct   dprm_resource_attribute_entry *attribute_entry_p;

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
     retval = get_datapath_port_byhandle(datapath_handle,port_handle,&port_entry_p);
     if(retval != DPRM_SUCCESS)
       break;

     attribute_entry_p = (struct dprm_resource_attribute_entry *)(OF_LIST_FIRST(port_entry_p->attributes));
     if(attribute_entry_p == NULL)
     {
       retval = DPRM_ERROR_NO_MORE_ENTRIES;
       break;
     }
     retval = dprm_get_first_attribute(attribute_entry_p,attribute_output_p);
   }while(0);

   CNTLR_RCU_READ_LOCK_RELEASE();
   return retval;
}  
/******************************************************************************************************/
int32_t dprm_get_next_datapath_port_attribute(uint64_t datapath_handle,
                                              uint64_t port_handle,
                                              char     *current_attribute_name,
                                              struct   dprm_attribute_name_value_output_info *attribute_output)
{
  int32_t  retval;
  struct   dprm_port_entry *port_entry_p;

  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    retval = get_datapath_port_byhandle(datapath_handle,port_handle,&port_entry_p);
    if(retval != DPRM_SUCCESS)
      break;
    retval = dprm_get_next_attribute(port_entry_p->attributes,current_attribute_name,attribute_output);
  }while(0);

  CNTLR_RCU_READ_LOCK_RELEASE();
  return retval;
}
#endif
/******************************************************************************************************/
int32_t dprm_get_first_channel_info(struct dprm_datapath_entry *datapath,
                                    struct cntlr_channel_info_s *channel_info)
{
  uint8_t hash_tbl_access_status = FALSE;
  dprm_dp_channel_entry_t *dp_channel_copy=NULL;
  struct cntlr_channel_info_s *channel_p=NULL;

  CNTLR_RCU_READ_LOCK_TAKE();
  if(datapath->is_main_conn_present)
  {
    dp_channel_copy = &datapath->main_channel;
  }

  if(dp_channel_copy == NULL)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"no channels exists");
    return OF_FAILURE;
  }

  MCHASH_ACCESS_NODE(dp_channel_copy->conn_table,
                     dp_channel_copy->conn_safe_ref.index,
                     dp_channel_copy->conn_safe_ref.magic,
                     channel_p, hash_tbl_access_status);

  if(hash_tbl_access_status == FALSE   )
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Channel safe reference is not valid  ");
    return OF_FAILURE;
  }
/* Atmaram - need to add transport type and proto spece info */
  //channel_info->dp_port = channel_p->dp_port;
  //  channel_info->local_port = channel_p->trnsprt_info->port_number;
  //channel_info->dp_ip = channel_p->dp_ip;
  channel_info->recv_bytes = channel_p->recv_bytes;
  channel_info->tx_bytes = channel_p->tx_bytes;

  CNTLR_RCU_READ_LOCK_RELEASE();
  return OF_SUCCESS;
}

int32_t dprm_get_first_auxchannel_info(struct dprm_datapath_entry *datapath,
                                    struct cntlr_channel_info_s *channel_info)
{
  uint8_t hash_tbl_access_status = FALSE;
  dprm_dp_channel_entry_t *dp_channel_copy=NULL;
  struct cntlr_channel_info_s *channel_p=NULL;

  CNTLR_RCU_READ_LOCK_TAKE();
  
  CNTLR_RCU_DE_REFERENCE(datapath->aux_channels,dp_channel_copy);

  if(dp_channel_copy == NULL)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"no channels exists");
    return OF_FAILURE;
  }

  MCHASH_ACCESS_NODE(dp_channel_copy->conn_table,
      dp_channel_copy->conn_safe_ref.index,
      dp_channel_copy->conn_safe_ref.magic,
      channel_p, hash_tbl_access_status);

  if(hash_tbl_access_status == FALSE   )
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Channel safe reference is not valid  ");
    return OF_FAILURE;
  }
  
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"channel_p->auxiliary_id=%d",channel_p->auxiliary_id);

  channel_info->auxiliary_id = channel_p->auxiliary_id;
/* Atmaram - need to add transport type and proto spece info */
  //channel_info->dp_port = channel_p->dp_port;
 // channel_info->local_port = channel_p->trnsprt_info->port_number;
  //channel_info->dp_ip = channel_p->dp_ip;
  channel_info->recv_bytes = channel_p->recv_bytes;
  channel_info->tx_bytes = channel_p->tx_bytes;

  CNTLR_RCU_READ_LOCK_RELEASE();
  return OF_SUCCESS;
}

int32_t dprm_get_next_auxchannel_info(struct dprm_datapath_entry *datapath, uint32_t auxiliary_id,
                                    struct cntlr_channel_info_s *channel_info)
{
  uint8_t hash_tbl_access_status = FALSE;
  dprm_dp_channel_entry_t *dp_channel_copy=NULL;
  struct cntlr_channel_info_s *channel_p=NULL;
  dprm_dp_channel_entry_t     *curr_dp_channel;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"auxiliary_id=%d",auxiliary_id);

  if(auxiliary_id >= datapath->curr_no_of_aux_chanels)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Max number reached");
    return OF_FAILURE;
  }

  CNTLR_RCU_READ_LOCK_TAKE();
  curr_dp_channel = datapath->aux_channels;
  do
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"curr_dp_channel->auxiliary_id=%d",curr_dp_channel->auxiliary_id);
    if(curr_dp_channel->auxiliary_id == auxiliary_id)
    {
      curr_dp_channel = curr_dp_channel->next;
      CNTLR_RCU_DE_REFERENCE(curr_dp_channel, dp_channel_copy);
      break;
    }
    curr_dp_channel = curr_dp_channel->next;
  }while(curr_dp_channel != datapath->aux_channels);

  if(dp_channel_copy == NULL)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"no channels exists");
    return OF_FAILURE;
  }

  MCHASH_ACCESS_NODE(dp_channel_copy->conn_table,
      dp_channel_copy->conn_safe_ref.index,
      dp_channel_copy->conn_safe_ref.magic,
      channel_p, hash_tbl_access_status);

  if(hash_tbl_access_status == FALSE   )
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Channel safe reference is not valid  ");
    return OF_FAILURE;
  }

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"channel_p->auxiliary_id=%d",channel_p->auxiliary_id);
  channel_info->auxiliary_id = channel_p->auxiliary_id;
/* Atmaram - need to add transport type and proto spece info */
  //channel_info->dp_port = channel_p->dp_port;
//  channel_info->local_port = channel_p->trnsprt_info->port_number;
  //channel_info->dp_ip = channel_p->dp_ip;
  channel_info->recv_bytes = channel_p->recv_bytes;
  channel_info->tx_bytes = channel_p->tx_bytes;

  CNTLR_RCU_READ_LOCK_RELEASE();
  return OF_SUCCESS;
}


int32_t dprm_get_dpid_from_ns_attributevalue(char *ns_attribute_value, uint64_t *dpid)
{
  struct   dprm_datapath_entry* datapath_info_p = NULL;
  uint32_t hashkey;
  int32_t  retval;

  uchar8_t offset;

  CNTLR_RCU_READ_LOCK_TAKE();
  offset = DPNODE_DPTBL_OFFSET;

  if(dprm_datapath_table_g == NULL)
  {
    CNTLR_RCU_READ_LOCK_RELEASE(); 
    return DPRM_FAILURE; 
  }
 
  MCHASH_TABLE_SCAN(dprm_datapath_table_g, hashkey)
  {
    MCHASH_BUCKET_SCAN(dprm_datapath_table_g,hashkey,datapath_info_p,struct dprm_datapath_entry*,offset)
    {
      if(!datapath_info_p->number_of_attributes )
      {
        continue;      
      }
      retval = dprm_get_exact_attribute_value(&(datapath_info_p->attributes), "namespace", ns_attribute_value);
      if(retval == DPRM_SUCCESS)
      {
           CNTLR_RCU_READ_LOCK_RELEASE();
         *dpid = datapath_info_p->dpid;      
         return OF_SUCCESS;
      }
    }
  }
 
  CNTLR_RCU_READ_LOCK_RELEASE();
  return OF_FAILURE;   
}

int32_t dprm_get_dpid_and_portname_from_ns_attributevalue(char *ns_attribute_value, uint64_t *dpid, char *port_name, uint32_t *port_id)
{
   struct   dprm_datapath_entry* datapath_info_p = NULL;
   struct   dprm_port_entry* port_entry_p = NULL;
   uint32_t hashkey, hashkey2;
   int32_t  retval;

   uchar8_t offset, offset2;

   CNTLR_RCU_READ_LOCK_TAKE();
   offset = DPNODE_DPTBL_OFFSET;

   if(dprm_datapath_table_g == NULL)
   {
      CNTLR_RCU_READ_LOCK_RELEASE(); 
      return DPRM_FAILURE; 
   }

   MCHASH_TABLE_SCAN(dprm_datapath_table_g, hashkey)
   {
      MCHASH_BUCKET_SCAN(dprm_datapath_table_g,hashkey,datapath_info_p,struct dprm_datapath_entry*,offset)
      {
         if(!datapath_info_p->number_of_ports )
         {
            continue;      
         }
         if(datapath_info_p->dprm_dpnode_porttable_p == NULL )
         {
            continue;      
         }
         offset2 = PORTNODE_PORTTBL_OFFSET;
         MCHASH_TABLE_SCAN(datapath_info_p->dprm_dpnode_porttable_p,hashkey2)
         {
            MCHASH_BUCKET_SCAN(datapath_info_p->dprm_dpnode_porttable_p,hashkey2,port_entry_p,struct dprm_port_entry *,offset2)
            {
               retval = dprm_get_exact_attribute_value(&(port_entry_p->attributes), "namespace", ns_attribute_value);
               if(retval == DPRM_SUCCESS)
               {
                  CNTLR_RCU_READ_LOCK_RELEASE();
                  *dpid = datapath_info_p->dpid;     
                  *port_id = port_entry_p->port_id;
	          strcpy(port_name, port_entry_p->port_name); 
                  return OF_SUCCESS;
               }
            }
         }
      }
   }

   CNTLR_RCU_READ_LOCK_RELEASE();
   return OF_FAILURE;   
}
/********************************************************************
* Function:dprm_get_logical_switch_handle
* Description:
*	This function to get the datapath handle for the given 
*	switch handle and datapath name.
********************************************************************/
int32_t dprm_get_logical_switch_handle(uint64_t switch_handle, 
				       char *datapath_name,
				       uint64_t *datapath_handle_p)
{
  struct   dprm_switch_entry   *switch_info_p=NULL;
  struct   dprm_datapath_entry *datapath_info_p=NULL;  
  int32_t  retval;

  uchar8_t offset;
  uint32_t hashkey;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Entererd(datapath name:%s)",datapath_name);
  CNTLR_RCU_READ_LOCK_TAKE();
    
  retval = get_switch_byhandle(switch_handle,&switch_info_p);
  if(retval != DPRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Get switch handle failed.");
    return retval;
  }

  offset = DPNODE_SWTBL_OFFSET;
  MCHASH_TABLE_SCAN(switch_info_p->dprm_swnode_dptable_p, hashkey)
  {
    MCHASH_BUCKET_SCAN(switch_info_p->dprm_swnode_dptable_p, hashkey ,datapath_info_p, struct dprm_datapath_entry *,offset)
    {
      if(!strcmp(datapath_info_p->datapath_name, datapath_name))
      {
  	OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Datapath info found with switch handle and datapath name");
        *datapath_handle_p = datapath_info_p->magic;
        *datapath_handle_p = (((*datapath_handle_p) <<32) | (datapath_info_p->index));
        CNTLR_RCU_READ_LOCK_RELEASE();
        return DPRM_SUCCESS;		
      }		
    }
  }
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Datapath info not found with switch handle and datapath name");
  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_FAILURE; 
}

/********************************************************************
* Function: get_dptableid_by_domaintableid
* Description: 
*	This function to get the datapath table-id for the given 
*	domain table-id.
********************************************************************/
int32_t get_dptableid_by_domaintableid(uint64_t datapath_handle, uint8_t domain_table_id)
{
  struct dprm_datapath_entry *pdatapath_info=NULL;
  dprm_dp_table_entry_t *table_entry=NULL;
  uint8_t offset;

  if (get_datapath_byhandle(datapath_handle, &pdatapath_info) != DPRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN, "No DP Entry with given datapath_handle=%llu",datapath_handle);
    return OF_FAILURE;
  }

  //OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"domain_table_id=%d",domain_table_id);
  offset = DPRM_DP_TABLE_ENTRY_OFFSET;
  OF_LIST_SCAN(pdatapath_info->table_list, table_entry, dprm_dp_table_entry_t*, offset)
  {
    if(table_entry->domain_table_id == domain_table_id)
    {
//      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"table_entry->dp_table_id=%d",table_entry->dp_table_id);
      return table_entry->dp_table_id;
    }
  }
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"dp table id not found for domain_table_id=%d",domain_table_id);
  return OF_FAILURE;
}

/********************************************************************
* Function: get_domaintableid_by_dptableid
* Description: 
*	This function to get the domain table-id for the given 
*	datapath table-id.
********************************************************************/
int32_t get_domaintableid_by_dptableid(uint64_t datapath_handle, uint8_t dp_table_id)
{
  struct dprm_datapath_entry *pdatapath_info=NULL;
  dprm_dp_table_entry_t *table_entry=NULL;
  uint8_t offset;

  if (get_datapath_byhandle(datapath_handle, &pdatapath_info) != DPRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN, "No DP Entry with given datapath_handle=%llu",datapath_handle);
    return OF_FAILURE;
  }

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"dp_table_id=%d",dp_table_id);
  
  offset = DPRM_DP_TABLE_ENTRY_OFFSET;
  OF_LIST_SCAN(pdatapath_info->table_list, table_entry, dprm_dp_table_entry_t*, offset)
  {
    if(table_entry->dp_table_id == dp_table_id)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"table_entry->domain_table_id=%d",table_entry->domain_table_id);
      return table_entry->domain_table_id;
    }
  }
  return OF_FAILURE;
}

/********************************************************************
* Function: dprm_clean_ports_from_datapath
* Description:
*       This function deletes all the ports attached to the datapath .
********************************************************************/
int32_t dprm_datapath_detach_handler(uint64_t datapath_handle)
{
   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"entry");
   int32_t ret_value;
   ret_value=dprm_clean_ports_from_datapath(datapath_handle);
   if(ret_value != OF_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "port deletion from datapath failed for datapath_handle %llx", datapath_handle);
   }

   return ret_value;
}
int32_t dprm_clean_ports_from_datapath(uint64_t datapath_handle)
{
   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"entry");
   int32_t ret_value,status=OF_FAILURE;
   struct dprm_port_info port_info;
   uint64_t port_handle;
   struct dprm_port_runtime_info runtime_info; 
   if(datapath_handle == 0)
      return OF_FAILURE;
   do{
      /*get first port*/
      CNTLR_RCU_READ_LOCK_TAKE();
      ret_value=dprm_get_first_datapath_port(datapath_handle, &port_info, &runtime_info, &port_handle);
      if (ret_value != OF_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Get frst DPRM port failed");
         if(ret_value == DPRM_ERROR_NO_MORE_ENTRIES)
         {
            CNTLR_RCU_READ_LOCK_RELEASE();
            return OF_SUCCESS;
         }
         CNTLR_RCU_READ_LOCK_RELEASE();
         return OF_FAILURE;
      }
      while(ret_value == OF_SUCCESS)
      {
         ret_value=dprm_delete_port_from_datapath(datapath_handle,port_handle);
         if (ret_value != OF_SUCCESS)
         {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"delete port failed  ret_value %d",ret_value);
            CNTLR_RCU_READ_LOCK_RELEASE();
            return OF_FAILURE;
         }
         ret_value=dprm_get_first_datapath_port(datapath_handle, &port_info, &runtime_info, &port_handle);
         if (ret_value != OF_SUCCESS)
         {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Get frst DPRM port failed");
            if(ret_value == DPRM_ERROR_NO_MORE_ENTRIES)
            {
               CNTLR_RCU_READ_LOCK_RELEASE();
               return OF_SUCCESS;
            }
            CNTLR_RCU_READ_LOCK_RELEASE();
            return OF_FAILURE;
         }
      }
   }while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return status;
}
