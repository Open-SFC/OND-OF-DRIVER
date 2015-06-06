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

/* File  :      dprmns.c
 * Author:      Atmaram G <atmaram.g@freescale.com>
 * Description: A network namespace is logically another copy of the network stack, with its own routes, firewall rules, and network devices.This file provides API to create namespace in linux. It also provides APIs to list/delete namespaces. DPRM creates  separate hash list and mempool list for namespace maintanance.
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



void     *dprm_namespace_mempool_g = NULL;
void *dprm_notifications_mempool_g=NULL;
uint32_t dprm_no_of_namespace_buckets_g;
struct   mchash_table* dprm_namespace_table_g = NULL;
of_list_t namespace_notifications[DPRM_NAMESPACE_LAST_NOTIFICATION_TYPE + 1];
#define DPRM_MAX_NAMESPACE_ENTRIES              64
#define DPRM_MAX_NAMESPACE_STATIC_ENTRIES         16
#define DPRM_MAX_NAMESPACE_NOTIFICATION_ENTRIES 64
#define DPRM_MAX_NAMESPACE_NOTIFICATION_STATIC_ENTRIES 16
#define NETNS_ADD_COMMAND "ip netns add"
#define NETNS_DELETE_COMMAND "ip netns delete"
#define NETNS_EXEC_COMMAND "ip netns exec"
#define NETNS_ARP_REPLY_COMMAND "echo 1 > /proc/sys/net/ipv4/conf/all/arp_accept"
#define NETNS_EXIT_COMMAND "exit"
#define DPRM_LINUX_COMMAND_MAX_LEN 128
#define DPRM_NS_FILENAME_MAX_LEN 200
#define NS_FILE_PATH "/var/run/netns/"


int32_t dprm_namespace_mempool_init();
int32_t dprm_namespace_mchash_table_init();
void dprm_get_namespace_mempoolentries(uint32_t* namespace_entries_max,uint32_t* namespace_static_entries);
void namespace_get_notification_mempoolentries(uint32_t *notification_entries_max,uint32_t *notification_static_entries);

int32_t  dprm_create_namespace(char *namespace_p);
int32_t  dprm_validate_namespace(char *namespace_p);
int32_t  dprm_delete_namespace(char *namespace_p);
void dprm_free_namespace_rcu(struct rcu_head *namespace_rcu_p);
void namespace_notifications_free_app_entry_rcu(struct rcu_head *app_entry_p);

int32_t dprm_namespace_init()
{
  int32_t status=DPRM_SUCCESS;
  int32_t ret_value;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  do
  {
    ret_value=dprm_namespace_mempool_init();
    if(ret_value != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Mempool initialization failed"); 
      status=DPRM_FAILURE;
      break;
    }
    ret_value=dprm_namespace_mchash_table_init();
    if(ret_value != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Hash table initialization failed"); 
      status=DPRM_FAILURE;
      break;
    }
    ret_value=dprm_namespace_notification_init();
    if(ret_value != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "notifier  initialization failed"); 
      status=DPRM_FAILURE;
      break;
    }
  }while(0);
  return status;
}

int32_t dprm_namespace_mempool_init()
{
  int32_t status=DPRM_SUCCESS;
  uint32_t namespace_entries_max,namespace_static_entries;
  int32_t ret_value;
  struct mempool_params mempool_info={};

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  dprm_get_namespace_mempoolentries(&namespace_entries_max,&namespace_static_entries);

  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;
  mempool_info.block_size = sizeof(struct dprm_namespace_entry);
  mempool_info.max_blocks = namespace_entries_max;
  mempool_info.static_blocks = namespace_static_entries;
  mempool_info.threshold_min = namespace_static_entries/10;
  mempool_info.threshold_max = namespace_static_entries/3;
  mempool_info.replenish_count = namespace_static_entries/10;
  mempool_info.b_memset_not_required =  FALSE;
  mempool_info.b_list_per_thread_required =  FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("dprm_namespace_pool", &mempool_info,
		  &dprm_namespace_mempool_g
		  );

  if(ret_value != MEMPOOL_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "mempool creation failed"); 
    status=DPRM_FAILURE;
  }
  return status;
}

int32_t dprm_namespace_mchash_table_init()
{
  uint32_t namespace_entries_max,namespace_static_entries;
  int32_t ret_value;
  int32_t status=DPRM_SUCCESS;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  dprm_get_namespace_mempoolentries(&namespace_entries_max,&namespace_static_entries);

  ret_value = mchash_table_create(((namespace_entries_max / 5)+1),
      namespace_entries_max,
      dprm_free_namespace_rcu,
      &dprm_namespace_table_g);
  if(ret_value != MCHASHTBL_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "hash table creation failed"); 
    status=DPRM_FAILURE;
  }

  dprm_no_of_namespace_buckets_g = (namespace_entries_max / 5) + 1;
  return status;

}

int32_t dprm_namespace_notification_init()
{
  uint32_t notification_entries_max,notification_static_entries;
  int32_t ret_value,ii;
  int32_t status=DPRM_SUCCESS;
  struct mempool_params mempool_info={};

  do
  {
    namespace_get_notification_mempoolentries(&notification_entries_max,&notification_static_entries);

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
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "mempool table creation failed"); 
      status=DPRM_FAILURE;
      break;
    }

    for(ii=0;ii<DPRM_NAMESPACE_LAST_NOTIFICATION_TYPE;ii++)
      OF_LIST_INIT(namespace_notifications[ii],namespace_notifications_free_app_entry_rcu);

  }while(0);

  return status;
}

void namespace_notifications_free_app_entry_rcu(struct rcu_head *app_entry_p)
{
  struct dprm_notification_app_hook_info *app_info_p;
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entry");
  if(app_entry_p)
  {
    app_info_p = (struct dprm_notification_app_hook_info *)((char *)app_entry_p - sizeof(of_list_node_t));
    mempool_release_mem_block(dprm_notifications_mempool_g,(uchar8_t*)app_info_p,app_info_p->heap_b);
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "succesful deletion");
    return;
  }
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "NULL Passed for deletion");
}


void dprm_free_namespace_rcu(struct rcu_head *namespace_rcu_p)
{
  struct dprm_namespace_entry *namespace_entry_p;
  uchar8_t offset;
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entry");
  if(namespace_rcu_p)
  {
    offset = NSNODE_NSTBL_OFFSET;
    namespace_entry_p = (struct dprm_namespace_entry *) ( (char *)namespace_rcu_p -(RCUNODE_OFFSET_IN_MCHASH_TBLLNK)-offset);
    mempool_release_mem_block(dprm_namespace_mempool_g,(uchar8_t*)namespace_entry_p,namespace_entry_p->heap_b);
    return;
  }  
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "NULL Passed for deletion");
}

void dprm_get_namespace_mempoolentries(uint32_t* namespace_entries_max,uint32_t* namespace_static_entries)
{
  *namespace_entries_max    = DPRM_MAX_NAMESPACE_ENTRIES;
  *namespace_static_entries = DPRM_MAX_NAMESPACE_STATIC_ENTRIES;
}

void namespace_get_notification_mempoolentries(uint32_t *notification_entries_max,uint32_t *notification_static_entries)
{
  *notification_entries_max    = DPRM_MAX_NAMESPACE_NOTIFICATION_ENTRIES;
  *notification_static_entries = DPRM_MAX_NAMESPACE_NOTIFICATION_STATIC_ENTRIES;
}


int32_t dprm_register_namespace(struct    dprm_namespace_info *namespace_info_p,uint64_t*  output_handle_p)
{
  int32_t  status,ret_value;
  uint32_t hashkey,offset;
  struct   dprm_namespace_entry* namespace_entry_p      = NULL; 
  uchar8_t  heap_b;
  uchar8_t* hashobj_p = NULL;
  uint32_t  index,magic;
  struct    dprm_namespace_notification_data notification_data;
  struct    dprm_notification_app_hook_info *app_entry_p;
  uint8_t status_b;


  status = DPRM_SUCCESS;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  do
  {
    if(dprm_namespace_table_g == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "DPRM Namespace Table is NULL"); 
      status=DPRM_FAILURE;
      break;
    }
    if(namespace_info_p == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "input null"); 
      status=DPRM_FAILURE;
      break;
    }
    if(strlen(namespace_info_p->name) < 3)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "name length is not enough"); 
      status=DPRM_ERROR_NAME_LENGTH_NOT_ENOUGH;
      break;
    }



   
    hashkey = dprm_get_hashval_byname(namespace_info_p->name,dprm_no_of_namespace_buckets_g);  
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "hashkey = %d",hashkey); 

    ret_value=dprm_get_namespace_handle(namespace_info_p->name, output_handle_p);
    if ( ret_value == DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "resource already exists"); 
      status = DPRM_ERROR_DUPLICATE_RESOURCE;
      break;
    }

    ret_value = mempool_get_mem_block(dprm_namespace_mempool_g,(uchar8_t**)&namespace_entry_p,&heap_b);
    if(ret_value != MEMPOOL_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "mempool get block failed"); 
      status = DPRM_FAILURE;
      break;
    }

    /* copy parameters */
    strcpy(namespace_entry_p->name,namespace_info_p->name);
    namespace_entry_p->heap_b = heap_b;
    ret_value= dprm_create_namespace(namespace_entry_p->name);
    if(ret_value != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "create namespace failed in linux"); 
      status = DPRM_FAILURE;
      break;
    }
    CNTLR_RCU_READ_LOCK_TAKE();
    offset = NSNODE_NSTBL_OFFSET;
    hashobj_p = (uchar8_t *)namespace_entry_p + NSNODE_NSTBL_OFFSET;
    MCHASH_APPEND_NODE(dprm_namespace_table_g,hashkey,namespace_entry_p,index,magic,hashobj_p,status_b);
    if(FALSE == status_b)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "append node failed"); 
      status = DPRM_FAILURE;
      CNTLR_RCU_READ_LOCK_RELEASE();
      break;
    }

    *output_handle_p = magic; 
    *output_handle_p = ((*output_handle_p <<32) | (index));

    namespace_entry_p->magic = magic;
    namespace_entry_p->index = index; 

    (namespace_entry_p->namespace_handle) = *output_handle_p;

    strcpy(notification_data.namespace_name,namespace_info_p->name);

    /* Send namespace add notification to the registered Applications. */
    OF_LIST_SCAN(namespace_notifications[DPRM_NAMESPACE_CREATED],app_entry_p,struct dprm_notification_app_hook_info*,DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET )
    {
      ((dprm_namespace_notifier_fn)(app_entry_p->call_back))(DPRM_NAMESPACE_CREATED,
      notification_data,
      app_entry_p->cbk_arg1,
      app_entry_p->cbk_arg2);
    }   

    OF_LIST_SCAN(namespace_notifications[DPRM_NAMESPACE_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET )
    {
      ((dprm_namespace_notifier_fn)(app_entry_p->call_back))(DPRM_NAMESPACE_CREATED,
      notification_data,
      app_entry_p->cbk_arg1,
      app_entry_p->cbk_arg2);
    }

    CNTLR_RCU_READ_LOCK_RELEASE();

  }while(0);

  if ( status != DPRM_SUCCESS)
  {
    if(namespace_entry_p) 
    {
      mempool_release_mem_block(dprm_namespace_mempool_g,(uchar8_t *)namespace_entry_p,namespace_entry_p->heap_b);
    }
  }
  return status;
}

int32_t dprm_update_namespace(uint64_t handle,struct dprm_namespace_info* namespace_info_p)
{
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "not support modification of namespace"); 
  return DPRM_FAILURE;
}

int32_t dprm_unregister_namespace(uint64_t handle)
{
  uint32_t index,magic;
  int32_t status=DPRM_ERROR_INVALID_NAMESPACE_HANDLE;
  struct   dprm_namespace_entry* namespace_entry_p = NULL;
  uint8_t  status_b = FALSE;
  uchar8_t offset;

  struct   dprm_namespace_notification_data notification_data;
  struct   dprm_notification_app_hook_info *app_entry_p;
  int32_t ret_value;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  magic = (uint32_t)(handle >>32);
  index = (uint32_t)handle;

  do
  {
    if(dprm_namespace_table_g == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "DPRM Namespace Table is NULL"); 
      status=DPRM_FAILURE;
      break;
    }
    CNTLR_RCU_READ_LOCK_TAKE();

    MCHASH_ACCESS_NODE(dprm_namespace_table_g,index,magic,namespace_entry_p,status_b);
    if(TRUE == status_b)
    {
      /* Send namespace delete notification to the registered Applications. */
      ret_value= dprm_delete_namespace(namespace_entry_p->name);
      if(ret_value != DPRM_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "delete namespace in linux failed"); 
        status = DPRM_FAILURE;
        CNTLR_RCU_READ_LOCK_RELEASE();
        break;
      }

      strcpy(notification_data.namespace_name,namespace_entry_p->name);
      OF_LIST_SCAN(namespace_notifications[DPRM_NAMESPACE_REMOVED],app_entry_p,struct dprm_notification_app_hook_info*,DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET )
      {
        ((dprm_namespace_notifier_fn)(app_entry_p->call_back))(DPRM_NAMESPACE_REMOVED,
        notification_data,
        app_entry_p->cbk_arg1,
        app_entry_p->cbk_arg2);
      }

      OF_LIST_SCAN(namespace_notifications[DPRM_NAMESPACE_ALL_NOTIFICATIONS],app_entry_p,struct dprm_notification_app_hook_info*,DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET )
      {
        ((dprm_namespace_notifier_fn)(app_entry_p->call_back))(DPRM_NAMESPACE_REMOVED,
        notification_data,
        app_entry_p->cbk_arg1,
        app_entry_p->cbk_arg2);
      }

      status_b = FALSE;
      offset = NSNODE_NSTBL_OFFSET;
      MCHASH_DELETE_NODE_BY_REF(dprm_namespace_table_g,index,magic,struct dprm_namespace_entry *,offset,status_b);
    //  CNTLR_RCU_READ_LOCK_RELEASE(); LSR
      if(status_b == TRUE)
      {
        status= DPRM_SUCCESS;
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "deleted success from hash table"); 
      }
      else
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "delete failed  from hash table"); 
        status= DPRM_FAILURE;
      }
    }
    CNTLR_RCU_READ_LOCK_RELEASE();
  }while(0);

  return status;
}

int32_t dprm_get_namespace_handle(char* name_p, uint64_t* namespace_handle_p)
{
  uint32_t hashkey;
  uchar8_t offset;

  struct dprm_namespace_entry* namespace_entry_p = NULL;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  if(dprm_namespace_table_g == NULL)
    return DPRM_FAILURE;

  hashkey = dprm_get_hashval_byname(name_p,dprm_no_of_namespace_buckets_g);

  CNTLR_RCU_READ_LOCK_TAKE();
  offset = NSNODE_NSTBL_OFFSET;
  MCHASH_BUCKET_SCAN(dprm_namespace_table_g,hashkey,namespace_entry_p,struct dprm_namespace_entry *,offset)
  {
    if(strcmp(namespace_entry_p->name,name_p))
      continue;
    CNTLR_RCU_READ_LOCK_RELEASE();

    *namespace_handle_p = namespace_entry_p->magic;
    *namespace_handle_p = ((*namespace_handle_p <<32) | (namespace_entry_p->index));

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "get handle success"); 
    return DPRM_SUCCESS;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "get handle failed"); 
  return DPRM_ERROR_INVALID_NAME;
}


int32_t dprm_get_first_namespace(struct    dprm_namespace_info *namespace_info_p,
    uint64_t  *namespace_handle_p)
{

  struct dprm_namespace_entry* namespace_entry_p = NULL;
  uint32_t hashkey;

  uchar8_t offset;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  if(dprm_namespace_table_g == NULL)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "DPRM Namespace Table is NULL"); 
    return DPRM_FAILURE;
  }
  CNTLR_RCU_READ_LOCK_TAKE();
  MCHASH_TABLE_SCAN(dprm_namespace_table_g,hashkey)
  {
    offset = NSNODE_NSTBL_OFFSET;
    MCHASH_BUCKET_SCAN(dprm_namespace_table_g,hashkey,namespace_entry_p,struct dprm_namespace_entry *,offset)
    {
      strcpy(namespace_info_p->name,namespace_entry_p->name);

      *namespace_handle_p = namespace_entry_p->magic;
      *namespace_handle_p = ((*namespace_handle_p <<32) | (namespace_entry_p->index));

      CNTLR_RCU_READ_LOCK_RELEASE();
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "get first namespace success"); 
      return DPRM_SUCCESS;
    }
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return DPRM_ERROR_NO_MORE_ENTRIES;
}


int32_t dprm_get_next_namespace(struct   dprm_namespace_info *namespace_info_p,
    uint64_t *namespace_handle_p)
{
  struct dprm_namespace_entry* namespace_entry_p = NULL;
  uint32_t hashkey;
  uint8_t current_entry_found_b;
  uchar8_t offset;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  CNTLR_RCU_READ_LOCK_TAKE();
  current_entry_found_b = FALSE;

  MCHASH_TABLE_SCAN(dprm_namespace_table_g,hashkey)
  {
    offset = NSNODE_NSTBL_OFFSET;
    MCHASH_BUCKET_SCAN(dprm_namespace_table_g,hashkey,namespace_entry_p,struct dprm_namespace_entry *,offset)
    {
      if(current_entry_found_b == FALSE)
      {
        if(*namespace_handle_p != (namespace_entry_p->namespace_handle))
          continue;

        else
        {
          current_entry_found_b = TRUE;
          continue;
        }

        /*Skip the First matching Namespace and Get the next Namespace */
      }
      strcpy(namespace_info_p->name,namespace_entry_p->name);

      *namespace_handle_p = namespace_entry_p->magic;
      *namespace_handle_p = ((*namespace_handle_p <<32) | (namespace_entry_p->index));

      CNTLR_RCU_READ_LOCK_RELEASE();
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "get next namespace success"); 
      return DPRM_SUCCESS;
    }
  }
  CNTLR_RCU_READ_LOCK_RELEASE();

  if(current_entry_found_b == TRUE)
    return DPRM_ERROR_NO_MORE_ENTRIES;
  else
    return DPRM_ERROR_INVALID_NAMESPACE_HANDLE;
}


int32_t  dprm_get_exact_namespace(uint64_t handle,
    struct dprm_namespace_info* namespace_info_p)
{
  uint32_t index,magic;
  struct   dprm_namespace_entry* namespace_entry_p;
  uint8_t  status_b;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  magic = (uint32_t)(handle >>32);
  index = (uint32_t)handle;

  CNTLR_RCU_READ_LOCK_TAKE();

  MCHASH_ACCESS_NODE(dprm_namespace_table_g,index,magic,namespace_entry_p,status_b);
  if(TRUE == status_b)
  {
    strcpy(namespace_info_p->name,namespace_entry_p->name);

    CNTLR_RCU_READ_LOCK_RELEASE();
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "get exact namespace success"); 
    return DPRM_SUCCESS;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();

  return DPRM_ERROR_INVALID_NAMESPACE_HANDLE;
}


int32_t  dprm_validate_namespace(char *namespace_p)
{
  int32_t  status=DPRM_SUCCESS;;
  int32_t ret_value;
  char ns_filename[DPRM_NS_FILENAME_MAX_LEN];
  do
  { 
   sprintf(ns_filename,"%s%s",NS_FILE_PATH,namespace_p);	
   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "ns file path is %s",ns_filename); 
   if(access(ns_filename,F_OK) != -1)
	{
	  status = DPRM_FAILURE;
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Namespace already Exists"); 
	  break;	
	}
   else
	{
	  status = DPRM_SUCCESS;
	}	
  }while(0);
return status;
}

int32_t  dprm_create_namespace(char *namespace_p)
{
  int32_t  status=DPRM_SUCCESS;;
  int32_t ret_value;
  char command[DPRM_LINUX_COMMAND_MAX_LEN];

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered");
  do
  {
    if(namespace_p == NULL)
    {
      status=DPRM_FAILURE;
      break;
    }

    memset(command,0,DPRM_LINUX_COMMAND_MAX_LEN);
    sprintf(command, "%s %s",NETNS_ADD_COMMAND,namespace_p);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "executing command %s",command); 
    ret_value=system(command);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "ret val is %d ",ret_value); 
    if ( ret_value == DPRM_FAILURE) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "namespace = %s creation failed",namespace_p); 
      status=DPRM_FAILURE;
      break;
    }
    memset(command,0,DPRM_LINUX_COMMAND_MAX_LEN);
    sprintf(command, "%s %s bash; %s; %s",NETNS_EXEC_COMMAND,namespace_p, NETNS_ARP_REPLY_COMMAND, NETNS_EXIT_COMMAND);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "executing command %s",command); 
    ret_value=system(command);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "namespace = %s succesfully created",namespace_p); 

  }while(0);

  return status;
}

int32_t  dprm_delete_namespace(char *namespace_p)
{
  int32_t  status=DPRM_SUCCESS;;
  int32_t ret_value;
  char command[DPRM_LINUX_COMMAND_MAX_LEN];

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered");
  do
  { 
    if(namespace_p == NULL)
    {
      status=DPRM_FAILURE;
      break;
    }

    memset(command,0,DPRM_LINUX_COMMAND_MAX_LEN);
    sprintf(command, "%s %s",NETNS_DELETE_COMMAND,namespace_p);
    ret_value=system(command);
    if (ret_value == DPRM_FAILURE) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "namespace = %s deletion failed",namespace_p); 
      status=DPRM_FAILURE;
      break;
    }
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "namespace = %s succesfully deleted",namespace_p);

  }while(0);

  return status;
}

int32_t dprm_register_namespace_notifications(uint32_t notification_type,
                                             dprm_namespace_notifier_fn namespace_notifier_fn,
                                             void     *callback_arg1,
                                             void     *callback_arg2)
{
  struct  dprm_notification_app_hook_info *app_entry_p;
  uint8_t heap_b;
  int32_t retval,status=OF_SUCCESS;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered");
  do
  {
    if((notification_type < DPRM_NAMESPACE_FIRST_NOTIFICATION_TYPE) || (notification_type > DPRM_NAMESPACE_LAST_NOTIFICATION_TYPE))
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "invalid notification type %d",notification_type); 
      status=DPRM_ERROR_INVALID_DATAPATH_NOTIFICATION_TYPE;
      break;
    }
  
    if(namespace_notifier_fn == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "null callback"); 
      status=DPRM_ERROR_NULL_CALLBACK_DATAPATH_NOTIFICATION_HOOK;
      break;
    }
  
    retval = mempool_get_mem_block(dprm_notifications_mempool_g,(uchar8_t**)&app_entry_p,&heap_b);
    if(retval != MEMPOOL_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "get mempool failed"); 
      status=DPRM_ERROR_NO_MEM;
      break;
    }
   
    app_entry_p->call_back = (void*)namespace_notifier_fn;
    app_entry_p->cbk_arg1  = callback_arg1;
    app_entry_p->cbk_arg2  = callback_arg2;
    app_entry_p->heap_b    = heap_b;

    /* Add Application to the list of switch Applications maintained per notification type. */
    OF_APPEND_NODE_TO_LIST(namespace_notifications[notification_type],app_entry_p,DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET);

  }while(0);
//  if(retval != OF_SUCCESS)
  //  mempool_release_mem_block(dprm_notifications_mempool_g,(uchar8_t*)app_entry_p,app_entry_p->heap_b);
  return status;
}


int32_t dprm_send_port_update_events( struct    dprm_datapath_entry* datapath_info_p,
                struct    dprm_port_entry* dprm_port_p)
{

	struct   dprm_attribute_name_value_output_info dprm_attribute_output={};
	struct    dprm_datapath_notification_data  notification_data;
	struct    dprm_notification_app_hook_info  *app_entry_p;
	struct    dprm_namespace_notification_data namespace_notification_data;
	int8_t lstoffset;
	int32_t status=OF_SUCCESS;
	int32_t ret_value=OF_SUCCESS;
	lstoffset = DPRM_NOTIFICATIONS_APP_HOOK_LISTNODE_OFFSET;
	uint64_t datapath_handle;

	do
	{
		if (( datapath_info_p == NULL ) || ( dprm_port_p == NULL ))
		{
			OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "invalid input");
			status=OF_SUCCESS;
			break;
		}
		datapath_handle= datapath_info_p->magic;
		datapath_handle= ( datapath_info_p->index | (datapath_handle << 32));

		if (datapath_info_p->multi_namespaces == TRUE)
		{
			OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "multiple namespace support");
			status=OF_SUCCESS;
			break;
		}

		if ((!strncmp(dprm_port_p->port_name,"br",2)) || (!strncmp(dprm_port_p->port_name,"patch",5)))
		{
			OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "ignore ports");
			status=OF_SUCCESS;
			break;
		}

		strcpy(dprm_attribute_output.name_string,"namespace");
		dprm_attribute_output.name_length = DPRM_MAX_ATTRIBUTE_NAME_LEN -1;
		dprm_attribute_output.value_length = DPRM_MAX_ATTRIBUTE_VALUE_LEN -1;
		ret_value = dprm_get_datapath_attribute_first_value(datapath_handle, &dprm_attribute_output);
		if (ret_value != OF_SUCCESS)
		{
			OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "dprm_get_datapath_attribute_first_value failed");
			status=OF_FAILURE;
			break;
		}
		strcpy(namespace_notification_data.namespace_name,dprm_attribute_output.value_string);
		strcpy(namespace_notification_data.port_name,dprm_port_p->port_name);

		namespace_notification_data.dp_id= datapath_info_p->dpid;
		namespace_notification_data.port_id= dprm_port_p->port_id;

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
	}while(0);

	return status;
}


int32_t dprm_delete_all_namespaces(void)
{
  struct dprm_namespace_entry* namespace_entry_p = NULL;
  uint32_t hashkey;

  int32_t ret_value=OF_SUCCESS;
  int32_t status=OF_SUCCESS;

  uchar8_t offset;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  if(dprm_namespace_table_g == NULL)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "DPRM Namespace Table is NULL"); 
    return DPRM_FAILURE;
  }
  CNTLR_RCU_READ_LOCK_TAKE();
  MCHASH_TABLE_SCAN(dprm_namespace_table_g,hashkey)
  {
    offset = NSNODE_NSTBL_OFFSET;
    MCHASH_BUCKET_SCAN(dprm_namespace_table_g,hashkey,namespace_entry_p,struct dprm_namespace_entry *,offset)
    {
      ret_value= dprm_delete_namespace(namespace_entry_p->name);
      if(ret_value != DPRM_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "delete namespace in linux failed"); 
        status = DPRM_FAILURE;
        break;
      }
    }
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return status;
}

void dprm_namespace_deinit()
{
   int32_t status=DPRM_SUCCESS;
   int32_t ret_value;

   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered");
   do
   {
      ret_value= dprm_delete_all_namespaces();
      if(ret_value != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "delete all namespaces failed");
         status=DPRM_FAILURE;
         break;
      }
   }while(0);
   return;
}


