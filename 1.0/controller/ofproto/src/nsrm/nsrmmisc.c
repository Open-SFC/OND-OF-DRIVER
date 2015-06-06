/*
** Netwrok Service resource manager tenant source file 
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
 * File name: nsrmmisc.c
 * Author: P.Vinod Sarma <B46178@freescale.com>
 * Date:   20/01/2014
 * Description: 
*/

#include "nsrm_system_dependencies.h"

#define RCUNODE_OFFSET_IN_MCHASH_TBLLNK offsetof(struct mchash_table_link, rcu_head)



/*nwservice_object Hash table*/
void     *nsrm_nwservice_object_mempool_g = NULL;
uint32_t nsrm_no_of_nwservice_object_buckets_g;
struct   mchash_table* nsrm_nwservice_object_table_g = NULL;

/*nschain_object Hash table*/
void     *nsrm_nschain_object_mempool_g = NULL;
uint32_t nsrm_no_of_nschain_object_buckets_g;
struct   mchash_table* nsrm_nschain_object_table_g = NULL;

/*nwservice attach to nschain hashtable with 1 bucket*/
void     *nsrm_nschain_nwservice_object_mempool_g = NULL;
uint32_t  nsrm_no_of_nschain_nwservice_object_buckets_g = 1;

/*nwservice instance  hashtable with 1 bucket*/
void     *nsrm_nwservice_instance_mempool_g = NULL;
uint32_t  nsrm_no_of_nwservice_instance_buckets_g = 1;


/*nschain_object Hash table*/
void     *nsrm_nschainset_object_mempool_g = NULL;
uint32_t nsrm_no_of_nschainset_object_buckets_g;
struct   mchash_table* nsrm_nschainset_object_table_g = NULL;

/*selection rules to nschainset hashtable with 1 bucket*/
void     *nsrm_selection_rule_mempool_g = NULL;
uint32_t  nsrm_no_of_selection_rule_buckets_g = 1;

/*bypass rules to nschain hashtable with 1 bucket*/
void     *nsrm_bypass_rule_mempool_g = NULL;
uint32_t  nsrm_no_of_bypass_rule_buckets_g = 1;

/*list for skipping nwservices in bypass rule*/
void     *nsrm_nwservice_skip_list_mempool_g = NULL;

/*l2_service_map Hash table*/
void     *nsrm_l2nw_service_map_mempool_g = NULL;
uint32_t nsrm_no_of_l2nw_service_map_buckets_g;
struct   mchash_table* nsrm_l2nw_service_map_table_g = NULL;

/*attributes mempool*/
void     *nsrm_attributes_mempool_g = NULL;

void     *nsrm_view_node_mempool_g = NULL;

/*Notifications mempool*/
void     *nsrm_notifications_mempool_g = NULL;

of_list_t nsrm_nwservice_object_notifications[NSRM_NWSERVICE_OBJECT_LAST_NOTIFICATION_TYPE + 1];
of_list_t nsrm_nschain_object_notifications[NSRM_NSCHAIN_OBJECT_LAST_NOTIFICATION_TYPE + 1];
of_list_t nsrm_nschainset_object_notifications[NSRM_NSCHAINSET_OBJECT_LAST_NOTIFICATION_TYPE + 1];
of_list_t nsrm_l2nw_service_map_notifications[NSRM_L2NW_SERVICE_MAP_LAST_NOTIFICATION_TYPE + 1];
of_list_t nsrm_selection_rule_notifications[NSRM_SELECTION_RULE_LAST_NOTIFICATION_TYPE + 1];
of_list_t nsrm_bypass_rule_notifications[NSRM_BYPASS_RULE_LAST_NOTIFICATION_TYPE + 1];


/************************************************************************************************/
void nsrm_notifications_free_app_entry_rcu(struct rcu_head *app_entry_p);
/*****************************************************************************************************/
void nsrm_get_nwservice_object_mempoolentries(uint32_t* nwservice_object_entries_max,
                                              uint32_t* nwservice_object_static_entries)
{
  *nwservice_object_entries_max    = NSRM_MAX_NWSERVICE_OBJECT_ENTRIES;
  *nwservice_object_static_entries = NSRM_MAX_NWSERVICE_OBJECT_STATIC_ENTRIES;
}

/*****************************************************************************************************/
void nsrm_get_notification_mempoolentries(uint32_t* notification_entries_max, uint32_t* notification_static_entries)
{
   *notification_entries_max    = NSRM_MAX_NOTIFICATION_ENTRIES;
   *notification_static_entries = NSRM_MAX_NOTIFICATION_STATIC_ENTRIES;
}
/*****************************************************************************************************/
void nsrm_get_nschain_object_mempoolentries(uint32_t* nschain_object_entries_max,
                                              uint32_t* nschain_object_static_entries)
{
  *nschain_object_entries_max    = NSRM_MAX_NSCHAIN_OBJECT_ENTRIES;
  *nschain_object_static_entries = NSRM_MAX_NSCHAIN_OBJECT_STATIC_ENTRIES;
}
/******************************************************************************************************/
void nsrm_get_nschain_nwservice_object_mempoolentries(uint32_t* nschain_nwservice_object_entries_max,
                                                      uint32_t* nschain_nwservice_object_static_entries)
{
  *nschain_nwservice_object_entries_max    = NSRM_MAX_NSCHAIN_NWSERVICE_OBJECT_ENTRIES;
  *nschain_nwservice_object_static_entries = NSRM_MAX_NSCHAIN_NWSERVICE_OBJECT_STATIC_ENTRIES;
}
/******************************************************************************************************/
void nsrm_get_nwservice_instance_mempoolentries(uint32_t*   nwservice_instance_entries_max,
                                                      uint32_t* nwservice_instance_static_entries)
{
  *nwservice_instance_entries_max    = NSRM_MAX_NWSERVICE_INSTANCE_ENTRIES;
  *nwservice_instance_static_entries = NSRM_MAX_NWSERVICE_INSTANCE_STATIC_ENTRIES;
}

/*******************************************************************************************************/
void nsrm_get_nschainset_object_mempoolentries(uint32_t* nschainset_object_entries_max,
                                              uint32_t* nschainset_object_static_entries)
{
  *nschainset_object_entries_max    = NSRM_MAX_NSCHAINSET_OBJECT_ENTRIES;
  *nschainset_object_static_entries = NSRM_MAX_NSCHAINSET_OBJECT_STATIC_ENTRIES;
}
/*******************************************************************************************************/
void nsrm_get_selection_rule_mempoolentries(uint32_t* selection_rule_entries_max,
                                              uint32_t* selection_rule_static_entries)
{
  *selection_rule_entries_max    =    NSRM_MAX_SELECTION_RULE_ENTRIES;
  *selection_rule_static_entries =    NSRM_MAX_SELECTION_RULE_STATIC_ENTRIES;
}
/******************************************************************************************************/
void nsrm_get_bypass_rule_mempoolentries(uint32_t* bypass_rule_entries_max,
                                              uint32_t* bypass_rule_static_entries)
{
  *bypass_rule_entries_max    =    NSRM_MAX_BYPASS_RULE_ENTRIES;
  *bypass_rule_static_entries =    NSRM_MAX_BYPASS_RULE_STATIC_ENTRIES;
}
/*****************************************************************************************************/
void nsrm_get_nwservice_skip_list_mempoolentries(uint32_t* nwservice_skip_list_entries_max,
                                              uint32_t* nwservice_skip_list_static_entries)
{
  *nwservice_skip_list_entries_max    =    NSRM_MAX_SKIP_LIST_ENTRIES;
  *nwservice_skip_list_static_entries =    NSRM_MAX_SKIP_LIST_STATIC_ENTRIES;
}

/*****************************************************************************************************/
void nsrm_get_l2nw_service_map_mempoolentries(uint32_t* l2nw_service_map_entries_max,
                                              uint32_t* l2nw_service_map_static_entries)
{
  *l2nw_service_map_entries_max    =  NSRM_MAX_L2NW_SERVICE_MAP_ENTRIES;
  *l2nw_service_map_static_entries =  NSRM_MAX_L2NW_SERVICE_MAP_STATIC_ENTRIES;
}
/*****************************************************************************************************/
void nsrm_get_attribute_mempoolentries(uint32_t *attribute_entries_max, uint32_t *attribute_static_entries)
{
  *attribute_entries_max    = NSRM_MAX_ATTRIBUTE_ENTRIES;
  *attribute_static_entries = NSRM_MAX_ATTRIBUTE_STATIC_ENTRIES;
}

/******************************************************************************************************/
void nsrm_get_view_node_mempoolentries(uint32_t* view_node_entries_max,uint32_t* view_node_static_entries)
{
  *view_node_entries_max    = NSRM_MAX_VIEW_NODE_ENTRIES;
  *view_node_static_entries = NSRM_MAX_VIEW_STATIC_NODE_ENTRIES;
}
/*****************************************************************************************************/
void nsrm_attributes_free_attribute_entry_rcu(struct rcu_head *attribute_entry_p)
{
  OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Entered");
  struct nsrm_resource_attribute_entry *attribute_info_p;
  int32_t ret_val;
  if(attribute_entry_p)
  {
    attribute_info_p = (struct nsrm_resource_attribute_entry *)(((char *)attribute_entry_p) - sizeof(of_list_node_t));
    ret_val = mempool_release_mem_block(nsrm_attributes_mempool_g,(uchar8_t*)attribute_info_p,attribute_info_p->heap_b);
    if(ret_val != MEMPOOL_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to release attr memory block.");
    }
    else
    {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Released attr memory block from the memory pool.");
    }

  }
  else
    OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "passed for deletion.");
}

/***************************************************************************************************/
void nsrm_free_nwservice_object_entry_rcu(struct rcu_head *nwservice_object_entry_p)
{
   struct nsrm_nwservice_object *app_info_p=NULL;
   uint32_t offset;
   int32_t ret_val;
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Entered");
   if(nwservice_object_entry_p)
   {
     offset = NSRM_NWSERVICE_OBJECT_TBL_OFFSET;
     app_info_p = (struct nsrm_nwservice_object *)(((char *)nwservice_object_entry_p - RCUNODE_OFFSET_IN_MCHASH_TBLLNK) - offset);

     ret_val = mempool_release_mem_block(nsrm_nwservice_object_mempool_g,
                                        (uchar8_t*)app_info_p,app_info_p->heap_b);
  if(ret_val != MEMPOOL_SUCCESS)
  {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to release nwservice object memory block.");
  }
  else
  {
     OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Released nwservice object memory block from the memory pool.");
  }
 }
  else
  {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Null entry to release",__FUNCTION__,__LINE__);
  }
}

void nsrm_free_nschain_object_entry_rcu(struct rcu_head *nschain_object_entry_p)
{
   struct nsrm_nschain_object *app_info_p=NULL;
   uint32_t offset;
   int32_t ret_val;
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Entered");
   if(nschain_object_entry_p)
   {
     offset = NSRM_NSCHAIN_OBJECT_TBL_OFFSET;
     app_info_p = (struct nsrm_nschain_object *)(((char *)nschain_object_entry_p - RCUNODE_OFFSET_IN_MCHASH_TBLLNK) - offset);

     ret_val = mempool_release_mem_block(nsrm_nschain_object_mempool_g,
                                        (uchar8_t*)app_info_p,app_info_p->heap_b);
  if(ret_val != MEMPOOL_SUCCESS)
  {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to release nschain object memory block.");
  }
  else
  {
     OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Released nschain object memory block from the memory pool.");
  }
 }
  else
  {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Null entry to release",__FUNCTION__,__LINE__);
  }
}


void nsrm_free_nschainset_object_entry_rcu(struct rcu_head *nschainset_object_entry_p)
{
   struct nsrm_nschainset_object *app_info_p=NULL;
   uint32_t offset;
   int32_t ret_val;
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Entered");
   if(nschainset_object_entry_p)
   {
     offset = NSRM_NSCHAINSET_OBJECT_TBL_OFFSET;
     app_info_p = (struct nsrm_nschainset_object *)(((char *)nschainset_object_entry_p - RCUNODE_OFFSET_IN_MCHASH_TBLLNK) - offset);

     ret_val = mempool_release_mem_block(nsrm_nschainset_object_mempool_g,
                                        (uchar8_t*)app_info_p,app_info_p->heap_b);
  if(ret_val != MEMPOOL_SUCCESS)
  {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to release nschainset object memory block.");
  }
  else
  {
     OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Released nschainset object memory block from the memory pool.");
  }
 }
  else
  {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Null entry to release",__FUNCTION__,__LINE__);
  }
}

void nsrm_free_l2nw_service_map_entry_rcu(struct rcu_head *l2nw_service_map_entry_p)
{
   struct nsrm_l2nw_service_map *app_info_p=NULL;
   uint32_t offset;
   int32_t ret_val;
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Entered");
   if(l2nw_service_map_entry_p)
   {
     offset = NSRM_L2NW_SERVICE_MAP_TBL_OFFSET;
     app_info_p = (struct nsrm_l2nw_service_map *)(((char *)l2nw_service_map_entry_p - RCUNODE_OFFSET_IN_MCHASH_TBLLNK) - offset);

     ret_val = mempool_release_mem_block(nsrm_l2nw_service_map_mempool_g,
                                        (uchar8_t*)app_info_p,app_info_p->heap_b);
  if(ret_val != MEMPOOL_SUCCESS)
  {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to release l2nw service map memory block.");
  }
  else
  {
     OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Released l2nw service map memory block from the memory pool.");
  }
 }
  else
  {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Null entry to release",__FUNCTION__,__LINE__);
  }
}

void nsrm_free_view_node_rcu(struct rcu_head *view_node_p)
{
   struct database_view_node *database_view_node_p=(struct database_view_node *)view_node_p;
   if(database_view_node_p)
   {
     mempool_release_mem_block(nsrm_view_node_mempool_g,(uchar8_t*)database_view_node_p,FALSE);
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

/****************************************************************************************************/
int32_t nsrm_get_hashval_byname(char* name_p,uint32_t no_of_buckets)
{
   uint32_t hashkey,param1,param2,param3,param4;
   char name_hash[16];
   memset(name_hash,0,16);
   strncpy(name_hash,name_p,16);
   param1 = *(uint32_t *)(name_hash +0);
   param2 = *(uint32_t *)(name_hash +4);
   param3 = *(uint32_t *)(name_hash +8);
   param4 = *(uint32_t *)(name_hash +12);
   NSRM_COMPUTE_HASH(param1,param2,param3,param4,no_of_buckets,hashkey);
   return hashkey;
}

/*****************************************************************************************************/
int32_t nsrm_init()
{
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "entered nsrm_init()");
   int ii,ret_val;
   uint32_t nwservice_object_entries_max, nwservice_object_static_entries;
   uint32_t nschain_object_entries_max, nschain_object_static_entries;
   uint32_t nschain_nwservice_object_entries_max, nschain_nwservice_object_static_entries;
   uint32_t nwservice_instance_entries_max, nwservice_instance_static_entries;
   uint32_t nschainset_object_entries_max, nschainset_object_static_entries;
   uint32_t selection_rule_entries_max, selection_rule_static_entries;
   uint32_t bypass_rule_entries_max, bypass_rule_static_entries;
   uint32_t nwservice_skip_list_entries_max, nwservice_skip_list_static_entries;
   uint32_t l2nw_service_map_entries_max, l2nw_service_map_static_entries;
   uint32_t attribute_entries_max,attribute_static_entries;
   uint32_t notification_entries_max , notification_static_entries;
   struct mempool_params mempool_info={};

   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "entered nsrm_init()");
   

   /* Creating mempool and hash table for nwservice object*/
   nsrm_get_nwservice_object_mempoolentries(&nwservice_object_entries_max ,
		   &nwservice_object_static_entries);
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "after nsrm_get_nwservice_object_mempoolentries");
   mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;  
   mempool_info.block_size = sizeof(struct nsrm_nwservice_object);
   mempool_info.max_blocks = nwservice_object_entries_max;
   mempool_info.static_blocks = nwservice_object_static_entries;
   mempool_info.threshold_min = nwservice_object_static_entries/10;
   mempool_info.threshold_max = nwservice_object_static_entries/3;
   mempool_info.replenish_count = nwservice_object_static_entries/10;
	   mempool_info.b_memset_not_required =  FALSE;
   mempool_info.b_list_per_thread_required =  FALSE;
   mempool_info.noof_blocks_to_move_to_thread_list = 0;

   ret_val = mempool_create_pool("nsrm_nwservice_object_pool", &mempool_info,
		   &nsrm_nwservice_object_mempool_g );
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"after mempool_create_pool");
   if(ret_val != MEMPOOL_SUCCESS)
   {
	   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Memory Pool Creation failed for nwservcie object %d", ret_val);
	   goto NSRMINIT_EXIT;
   }

   ret_val = mchash_table_create(((nwservice_object_entries_max/5) +1) ,
		   nwservice_object_entries_max,
		   nsrm_free_nwservice_object_entry_rcu,
		   &nsrm_nwservice_object_table_g);
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "after mchash_table_create");
   if(ret_val != MCHASHTBL_SUCCESS)
   {
	   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Hash table Creation failed for nwservice object ");
	   goto NSRMINIT_EXIT;
   }
   nsrm_no_of_nwservice_object_buckets_g = (nwservice_object_entries_max/5)+1;



   /* Creating mempool and hash table for nschain object*/
   nsrm_get_nschain_object_mempoolentries(&nschain_object_entries_max ,
		   &nschain_object_static_entries);
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "after nsrm_get_nschain_object_mempoolentries");

   mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;  
   mempool_info.block_size = sizeof(struct nsrm_nschain_object);
   mempool_info.max_blocks = nschain_object_entries_max;
   mempool_info.static_blocks = nschain_object_static_entries;
   mempool_info.threshold_min = nschain_object_static_entries/10;
   mempool_info.threshold_max = nschain_object_static_entries/3;
   mempool_info.replenish_count = nschain_object_static_entries/10;
   mempool_info.b_memset_not_required =  FALSE;
   mempool_info.b_list_per_thread_required =  FALSE;
   mempool_info.noof_blocks_to_move_to_thread_list = 0;

   ret_val = mempool_create_pool("nsrm_nschain_object_pool", &mempool_info,
		   &nsrm_nschain_object_mempool_g );
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"after mempool_create_pool");
   if(ret_val != MEMPOOL_SUCCESS)
   {
	   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Memory Pool Creation failed for nschain object %d", ret_val);
	   goto NSRMINIT_EXIT;
   }

   ret_val = mchash_table_create(((nschain_object_entries_max/5) +1) ,
		   nschain_object_entries_max,
		   nsrm_free_nschain_object_entry_rcu,
		   &nsrm_nschain_object_table_g);
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "after mchash_table_create");
   if(ret_val != MCHASHTBL_SUCCESS)
   {
	   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Hash table Creation failed for nschain object ");
	   goto NSRMINIT_EXIT;
   }
   nsrm_no_of_nschain_object_buckets_g = (nschain_object_entries_max/5)+1;

   /* Creating hash table for nschain_nwservice object*/
   nsrm_get_nschain_nwservice_object_mempoolentries(&nschain_nwservice_object_entries_max ,
		   &nschain_nwservice_object_static_entries);
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "after nsrm_get_nschain_nwservice_object_mempoolentries");

   mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;  
   mempool_info.block_size = sizeof(struct nschain_nwservice_object);
   mempool_info.max_blocks = nschain_nwservice_object_entries_max;
   mempool_info.static_blocks = nschain_nwservice_object_static_entries;
   mempool_info.threshold_min = nschain_nwservice_object_static_entries/10;
   mempool_info.threshold_max = nschain_nwservice_object_static_entries/3;
   mempool_info.replenish_count = nschain_nwservice_object_static_entries/10;
   mempool_info.b_memset_not_required =  FALSE;
   mempool_info.b_list_per_thread_required =  FALSE;
   mempool_info.noof_blocks_to_move_to_thread_list = 0;

   ret_val = mempool_create_pool("nsrm_nschain_nwservice_object_pool", &mempool_info,
		   &nsrm_nschain_nwservice_object_mempool_g );
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"after mempool_create_pool");
   if(ret_val != MEMPOOL_SUCCESS)
   {
	   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Memory Pool Creation failed for nschain object %d", ret_val);
	   goto NSRMINIT_EXIT;
   }

   /* Creating mempool for nwservice instance*/      
   nsrm_get_nwservice_instance_mempoolentries(&nwservice_instance_entries_max ,
		   &nwservice_instance_static_entries);
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "after nsrm_get_nwservice_instance_mempoolentries");

   mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;  
   mempool_info.block_size = sizeof(struct nwservice_instance);
   mempool_info.max_blocks = nwservice_instance_entries_max;
   mempool_info.static_blocks = nwservice_instance_static_entries;
   mempool_info.threshold_min = nwservice_instance_static_entries/10;
   mempool_info.threshold_max = nwservice_instance_static_entries/3;
   mempool_info.replenish_count = nwservice_instance_static_entries/10;
   mempool_info.b_memset_not_required =  FALSE;
   mempool_info.b_list_per_thread_required =  FALSE;
   mempool_info.noof_blocks_to_move_to_thread_list = 0;

   ret_val = mempool_create_pool("nsrm_nwservice_instance_pool", &mempool_info,
		   &nsrm_nwservice_instance_mempool_g );
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"after mempool_create_pool");
   if(ret_val != MEMPOOL_SUCCESS)
   {
	   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Memory Pool Creation failed for nwservice instance: %d", ret_val);
	   goto NSRMINIT_EXIT;
   }



   /* Creating mempool and hash table for nschainset object*/
   nsrm_get_nschainset_object_mempoolentries(&nschainset_object_entries_max ,
		   &nschainset_object_static_entries);
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "after nsrm_get_nschain_object_mempoolentries");

   mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;  
   mempool_info.block_size = sizeof(struct nsrm_nschainset_object);
   mempool_info.max_blocks = nschainset_object_entries_max;
   mempool_info.static_blocks = nschainset_object_static_entries;
   mempool_info.threshold_min = nschainset_object_static_entries/10;
   mempool_info.threshold_max = nschainset_object_static_entries/3;
   mempool_info.replenish_count = nschainset_object_static_entries/10;
   mempool_info.b_memset_not_required =  FALSE;
   mempool_info.b_list_per_thread_required =  FALSE;
   mempool_info.noof_blocks_to_move_to_thread_list = 0;

   ret_val = mempool_create_pool("nsrm_nschainset_object_pool", &mempool_info,
		   &nsrm_nschainset_object_mempool_g );
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"after mempool_create_pool");
   if(ret_val != MEMPOOL_SUCCESS)
   {
	   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Memory Pool Creation failed for nschainset object %d", ret_val);
	   goto NSRMINIT_EXIT;
   }

   ret_val = mchash_table_create(((nschainset_object_entries_max/5) +1) ,
		   nschainset_object_entries_max,
		   nsrm_free_nschainset_object_entry_rcu,
		   &nsrm_nschainset_object_table_g);
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "after mchash_table_create");
   if(ret_val != MCHASHTBL_SUCCESS)
   {
	   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Hash table Creation failed for nschainset object ");
	   goto NSRMINIT_EXIT;
   }
   nsrm_no_of_nschainset_object_buckets_g = (nschainset_object_entries_max/5)+1;


   /* Creating mempool for selection rule*/
   nsrm_get_selection_rule_mempoolentries(&selection_rule_entries_max ,
		   &selection_rule_static_entries);
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "after nsrm_get_selection_rule_mempoolentries");

   mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;  
   mempool_info.block_size = sizeof(struct nsrm_nschain_selection_rule);
   mempool_info.max_blocks = selection_rule_entries_max;
   mempool_info.static_blocks = selection_rule_static_entries;
   mempool_info.threshold_min = selection_rule_static_entries/10;
   mempool_info.threshold_max = selection_rule_static_entries/3;
   mempool_info.replenish_count = selection_rule_static_entries/10;
   mempool_info.b_memset_not_required =  FALSE;
   mempool_info.b_list_per_thread_required =  FALSE;
   mempool_info.noof_blocks_to_move_to_thread_list = 0;

   ret_val = mempool_create_pool("nsrm_selection_rule_pool", &mempool_info,
		   &nsrm_selection_rule_mempool_g );
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"after mempool_create_pool");
   if(ret_val != MEMPOOL_SUCCESS)
   {
	   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Memory Pool Creation failed for selection rule %d", ret_val);
	   goto NSRMINIT_EXIT;
   }


   /* Creating mempool for bypass rule*/
   nsrm_get_bypass_rule_mempoolentries(&bypass_rule_entries_max ,
		   &bypass_rule_static_entries);
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "after nsrm_get_bypass_rule_mempoolentries");

   mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;  
   mempool_info.block_size = sizeof(struct nsrm_nwservice_bypass_rule);
   mempool_info.max_blocks = bypass_rule_entries_max;
   mempool_info.static_blocks = bypass_rule_static_entries;
   mempool_info.threshold_min = bypass_rule_static_entries/10;
   mempool_info.threshold_max = bypass_rule_static_entries/3;
   mempool_info.replenish_count = bypass_rule_static_entries/10;
   mempool_info.b_memset_not_required =  FALSE;
   mempool_info.b_list_per_thread_required =  FALSE;
   mempool_info.noof_blocks_to_move_to_thread_list = 0;

   ret_val = mempool_create_pool("nsrm_bypass_rule_pool", &mempool_info,
		   &nsrm_bypass_rule_mempool_g );
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"after mempool_create_pool");
   if(ret_val != MEMPOOL_SUCCESS)
   {
	   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Memory Pool Creation failed for selection rule %d", ret_val);
	   goto NSRMINIT_EXIT;
   }


   /* Creating mempool for skip list inside bypass rule*/
   nsrm_get_nwservice_skip_list_mempoolentries(&nwservice_skip_list_entries_max ,
		   &nwservice_skip_list_static_entries);
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "after nsrm_get__mempoolentries");

   mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;  
   mempool_info.block_size = sizeof(struct nsrm_nwservice_bypass_rule);
   mempool_info.max_blocks = nwservice_skip_list_entries_max;
   mempool_info.static_blocks = nwservice_skip_list_static_entries;
   mempool_info.threshold_min = nwservice_skip_list_static_entries/10;
   mempool_info.threshold_max = nwservice_skip_list_static_entries/3;
   mempool_info.replenish_count = nwservice_skip_list_static_entries/10;
   mempool_info.b_memset_not_required =  FALSE;
   mempool_info.b_list_per_thread_required =  FALSE;
   mempool_info.noof_blocks_to_move_to_thread_list = 0;

   ret_val = mempool_create_pool("nsrm_nwservice_skip_list_pool", &mempool_info,
		   &nsrm_nwservice_skip_list_mempool_g );
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"after mempool_create_pool");
   if(ret_val != MEMPOOL_SUCCESS)
   {
	   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Memory Pool Creation failed for selection rule %d", ret_val);
	   goto NSRMINIT_EXIT;
   }


   /* Creating mempool and hash table for l2nw service map*/
   nsrm_get_nschain_object_mempoolentries(&l2nw_service_map_entries_max ,
		   &l2nw_service_map_static_entries);
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "after nsrm_l2nw_service_mempoolentries");

   mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;  
   mempool_info.block_size = sizeof(struct nsrm_l2nw_service_map);
   mempool_info.max_blocks = l2nw_service_map_entries_max;
   mempool_info.static_blocks = l2nw_service_map_static_entries;
   mempool_info.threshold_min = l2nw_service_map_static_entries/10;
   mempool_info.threshold_max = l2nw_service_map_static_entries/3;
   mempool_info.replenish_count = l2nw_service_map_static_entries/10;
   mempool_info.b_memset_not_required =  FALSE;
   mempool_info.b_list_per_thread_required =  FALSE;
   mempool_info.noof_blocks_to_move_to_thread_list = 0;

   ret_val = mempool_create_pool("nsrm_l2nw_service_map_pool", &mempool_info,
		   &nsrm_l2nw_service_map_mempool_g );
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"after mempool_create_pool");
   if(ret_val != MEMPOOL_SUCCESS)
   {
	   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Memory Pool Creation failed for l2nw_service_map %d", ret_val);
	   goto NSRMINIT_EXIT;
   }

   ret_val = mchash_table_create(((l2nw_service_map_entries_max/5) +1) ,
		   l2nw_service_map_entries_max,
		   nsrm_free_l2nw_service_map_entry_rcu,
		   &nsrm_l2nw_service_map_table_g);
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "after mchash_table_create");
   if(ret_val != MCHASHTBL_SUCCESS)
   {
	   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Hash table Creation failed for l2nw_service_map object ");
	   goto NSRMINIT_EXIT;
   }
   nsrm_no_of_l2nw_service_map_buckets_g = (l2nw_service_map_entries_max/5)+1;


   /** attributes memory pool **/
   nsrm_get_attribute_mempoolentries(&attribute_entries_max,&attribute_static_entries);

   mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;  
   mempool_info.block_size = sizeof(struct nsrm_resource_attribute_entry);
   mempool_info.max_blocks = attribute_entries_max;
   mempool_info.static_blocks = attribute_static_entries;
   mempool_info.threshold_min = attribute_static_entries/10;
   mempool_info.threshold_max = attribute_static_entries/3;
   mempool_info.replenish_count = attribute_static_entries/10;
   mempool_info.b_memset_not_required =  FALSE;
   mempool_info.b_list_per_thread_required =  FALSE;
   mempool_info.noof_blocks_to_move_to_thread_list = 0;


   ret_val = mempool_create_pool("nsrm_attributes_pool", &mempool_info,
		   &nsrm_attributes_mempool_g);

   if(ret_val != MEMPOOL_SUCCESS)
	   goto NSRMINIT_EXIT;


   /*Notifications mempool*/
   nsrm_get_notification_mempoolentries(&notification_entries_max,&notification_static_entries);

   mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;  
   mempool_info.block_size = sizeof(struct nsrm_notification_app_hook_info);
   mempool_info.max_blocks = notification_entries_max;
   mempool_info.static_blocks = notification_static_entries;
   mempool_info.threshold_min = notification_static_entries/10;
   mempool_info.threshold_max = notification_static_entries/3;
   mempool_info.replenish_count = notification_static_entries/10;
   mempool_info.b_memset_not_required =  FALSE;
   mempool_info.b_list_per_thread_required =  FALSE;
   mempool_info.noof_blocks_to_move_to_thread_list = 0;

   ret_val = mempool_create_pool("nsrm_notifications_pool", &mempool_info,
		   &nsrm_notifications_mempool_g);

   if(ret_val != MEMPOOL_SUCCESS)
      goto NSRMINIT_EXIT;

   for(ii = 0; ii < NSRM_NWSERVICE_OBJECT_LAST_NOTIFICATION_TYPE; ii++)
     OF_LIST_INIT(nsrm_nwservice_object_notifications[ii], nsrm_notifications_free_app_entry_rcu);
 
   for(ii = 0; ii < NSRM_NSCHAIN_OBJECT_LAST_NOTIFICATION_TYPE; ii++)
     OF_LIST_INIT(nsrm_nschain_object_notifications[ii], nsrm_notifications_free_app_entry_rcu);

   for(ii = 0; ii < NSRM_NSCHAINSET_OBJECT_LAST_NOTIFICATION_TYPE; ii++)
     OF_LIST_INIT(nsrm_nschainset_object_notifications[ii], nsrm_notifications_free_app_entry_rcu);

   for(ii = 0; ii < NSRM_L2NW_SERVICE_MAP_LAST_NOTIFICATION_TYPE; ii++)
     OF_LIST_INIT(nsrm_l2nw_service_map_notifications[ii], nsrm_notifications_free_app_entry_rcu);

   for(ii = 0; ii < NSRM_SELECTION_RULE_LAST_NOTIFICATION_TYPE; ii++)
     OF_LIST_INIT(nsrm_selection_rule_notifications[ii], nsrm_notifications_free_app_entry_rcu);

   for(ii = 0; ii < NSRM_BYPASS_RULE_LAST_NOTIFICATION_TYPE; ii++)
     OF_LIST_INIT(nsrm_bypass_rule_notifications[ii], nsrm_notifications_free_app_entry_rcu);




   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "NSRM Module Initialized successfully!.");
   return NSRM_SUCCESS;


NSRMINIT_EXIT:
   mempool_delete_pool(nsrm_nwservice_object_mempool_g);
   mempool_delete_pool(nsrm_nschain_object_mempool_g);
   mempool_delete_pool(nsrm_nschain_nwservice_object_mempool_g);
   mempool_delete_pool(nsrm_nwservice_instance_mempool_g);
   mempool_delete_pool(nsrm_nschainset_object_mempool_g);
   mempool_delete_pool(nsrm_bypass_rule_mempool_g);
   mempool_delete_pool(nsrm_nwservice_skip_list_mempool_g );
   mempool_delete_pool(nsrm_selection_rule_mempool_g);
   mempool_delete_pool(nsrm_l2nw_service_map_mempool_g);
   mempool_delete_pool(nsrm_attributes_mempool_g);
   mempool_delete_pool(nsrm_notifications_mempool_g);

   mchash_table_delete(nsrm_nwservice_object_table_g);
   mchash_table_delete(nsrm_nschain_object_table_g);
   mchash_table_delete(nsrm_nschainset_object_table_g);
   mchash_table_delete(nsrm_l2nw_service_map_table_g);


   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "NSRM Module Failed to Init!.");
   return NSRM_FAILURE;
}
/*********************************/
int32_t nsrm_uninit()
{
  int32_t ret_value = FALSE;
  ret_value = mchash_table_delete(nsrm_nwservice_object_table_g);
  if(ret_value != MCHASHTBL_SUCCESS)
    return ret_value;

  ret_value = mchash_table_delete(nsrm_nschain_object_table_g);
  if(ret_value != MCHASHTBL_SUCCESS)
    return ret_value;

  ret_value = mchash_table_delete(nsrm_nschainset_object_table_g);
  if(ret_value != MCHASHTBL_SUCCESS)
    return ret_value;

  ret_value = mchash_table_delete(nsrm_l2nw_service_map_table_g);
  if(ret_value != MCHASHTBL_SUCCESS)
    return ret_value;


  ret_value = mempool_delete_pool(nsrm_nwservice_object_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    return ret_value;

  ret_value = mempool_delete_pool(nsrm_nschain_object_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    return ret_value;

  ret_value = mempool_delete_pool(nsrm_nschain_nwservice_object_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    return ret_value;

  ret_value = mempool_delete_pool(nsrm_nwservice_instance_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    return ret_value;

  ret_value = mempool_delete_pool(nsrm_nschainset_object_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    return ret_value;

  ret_value = mempool_delete_pool(nsrm_bypass_rule_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    return ret_value;

  ret_value = mempool_delete_pool(nsrm_nwservice_skip_list_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    return ret_value;

  ret_value = mempool_delete_pool(nsrm_selection_rule_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    return ret_value;

  ret_value = mempool_delete_pool(nsrm_l2nw_service_map_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    return ret_value;

  ret_value = mempool_delete_pool(nsrm_notifications_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    return ret_value;
 
  ret_value = mempool_delete_pool(nsrm_attributes_mempool_g);
  if(ret_value != MEMPOOL_SUCCESS)
    return ret_value;

  return NSRM_SUCCESS;
}


/*****************************************************************************************************/
void nsrm_notifications_free_app_entry_rcu(struct rcu_head *app_entry_p)
{
  struct nsrm_notification_app_hook_info *app_info_p;
  int32_t ret_value;
  OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"entered nsrm_notifications_free_app_entry_rcu");
  if(app_entry_p)
  {
    app_info_p =(struct nsrm_notification_app_hook_info *)((char *)app_entry_p - sizeof(of_list_node_t));
    ret_value = mempool_release_mem_block(nsrm_notifications_mempool_g,
                                          (uchar8_t*)app_info_p, app_info_p->heap_b);
  if(ret_value != MEMPOOL_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to release notification memory block.");
  }
  else
  {
    OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Released notification memory block from the memory pool.");
  }
}
  else
  {
    OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "NULL passed for deletion.");
  }
}


