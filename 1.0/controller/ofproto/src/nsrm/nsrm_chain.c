
/** Network Service resource manager  source file 
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
 * File name:nsrm_chain.c
 * Author: P.Vinod Sarma <B46178@freescale.com>
 * Date:   24/01/2014
 * Description: 
*/

#include  "nsrm_system_dependencies.h"
#include  "crm_tenant_api.h"
#include  "crm_vm_api.h"
#include  "crm_vn_api.h"
#include  "crm_port_api.h"
#include  "rrm_crm_rtlns_mgr.h"
#include  "port_status_mgr.h"


#define RCUNODE_OFFSET_IN_MCHASH_TBLLNK offsetof(struct mchash_table_link, rcu_head)

extern void     *nsrm_nschain_object_mempool_g;
extern void     *nsrm_nschain_nwservice_object_mempool_g;
extern void     *nsrm_nwservice_instance_mempool_g;
extern uint32_t nsrm_no_of_nschain_object_buckets_g;
extern uint32_t nsrm_no_of_nwservice_object_buckets_g;
extern void     *nsrm_notifications_mempool_g;
extern struct   mchash_table* nsrm_nschain_object_table_g ;
extern struct   mchash_table* nsrm_nwservice_object_table_g ;

extern of_list_t nsrm_nschain_object_notifications[];
extern of_list_t nsrm_nwservice_object_notifications[];

struct database_view nschain_object_db_view[NSRM_MAX_VIEW_DATABASE];

void nsrm_free_nschain_nwservice_object_entry_rcu(struct rcu_head *nschain_nwservice_object_entry_p)
{
   struct   nschain_nwservice_object *app_info_p=NULL;
   uint32_t offset;
   int32_t ret_val;
   if(nschain_nwservice_object_entry_p)
   {
     offset = NSRM_NSCHAIN_NWSERVICE_OBJECT_TBL_OFFSET;
     app_info_p = (struct nschain_nwservice_object *)(((char *)nschain_nwservice_object_entry_p - RCUNODE_OFFSET_IN_MCHASH_TBLLNK) - offset);

     ret_val = mempool_release_mem_block(nsrm_nschain_nwservice_object_mempool_g,
                                        (uchar8_t*)app_info_p,app_info_p->heap_b);
     if(ret_val != MEMPOOL_SUCCESS)
     {
       OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to release nschain_nwservice object memory block.");
     }
     else
     {
       OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Released nschain_nwservice object memory block from the memory pool.");
     }  
   }
   else
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Null entry to release",__FUNCTION__,__LINE__);
   }
}

void nsrm_free_nwservice_instance_entry_rcu(struct rcu_head *nwservice_instance_entry_p)
{
   struct   nwservice_instance *app_info_p=NULL;
   uint32_t offset;
   int32_t ret_val;
   if(nwservice_instance_entry_p)
   {
     offset = NSRM_NWSERVICE_INSTANCE_TBL_OFFSET;
     app_info_p = (struct nwservice_instance *)(((char *)nwservice_instance_entry_p - RCUNODE_OFFSET_IN_MCHASH_TBLLNK) - offset);

     ret_val = mempool_release_mem_block(nsrm_nwservice_instance_mempool_g,
                                        (uchar8_t*)app_info_p,app_info_p->heap_b);
     if(ret_val != MEMPOOL_SUCCESS)
     {
       OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to release nwservice instance memory block.");
     }
     else
     {
       OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Released nwservice instance memory block from the memory pool.");
     } 
 }
   else
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Null entry to release",__FUNCTION__,__LINE__);
   }
}


/***********************************************************************************************/
int32_t nsrm_get_nschain_object_handle(char *nschain_object_name_p, uint64_t *nschain_object_handle_p)
{
   uint32_t hashkey;
   int32_t  offset;
   struct   nsrm_nschain_object* nschain_object_entry_p = NULL;
   if(nsrm_nschain_object_table_g == NULL)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "There is no hash table!.");
     return NSRM_FAILURE;
   }
   hashkey = nsrm_get_hashval_byname(nschain_object_name_p, nsrm_no_of_nschain_object_buckets_g);
   CNTLR_RCU_READ_LOCK_TAKE();
   offset = NSRM_NSCHAIN_OBJECT_TBL_OFFSET;
   MCHASH_BUCKET_SCAN(nsrm_nschain_object_table_g, hashkey, nschain_object_entry_p,struct nsrm_nschain_object*,offset)
   {
     if(strcmp(nschain_object_entry_p->name_p, nschain_object_name_p))
     {
       continue;
     }
     *nschain_object_handle_p = nschain_object_entry_p->magic;
     *nschain_object_handle_p = ((*nschain_object_handle_p<<32) | (nschain_object_entry_p->index));
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_SUCCESS;
  }
    CNTLR_RCU_READ_LOCK_RELEASE();
    OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nschain object  name not found!.");
    return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
}

int32_t nsrm_get_nschain_byhandle(uint64_t nschain_handle,struct nsrm_nschain_object
                                                              **nschain_object_p_p)
{
  /* Caller shall take RCU locks. */
  uint32_t index,magic;
  uint8_t status_b;

  magic = (uint32_t)(nschain_handle >>32);
  index = (uint32_t)nschain_handle;

  MCHASH_ACCESS_NODE(nsrm_nschain_object_table_g,index,magic,*nschain_object_p_p,status_b);
  if(TRUE == status_b)
    return NSRM_SUCCESS;
  return NSRM_FAILURE;
}
/*****************************************************************************************************************************/
int32_t nsrm_add_nschain_object(
        int32_t number_of_config_params,
        struct  nsrm_nschain_object_key*         key_info_p,
        struct  nsrm_nschain_object_config_info* config_info_p)
{
  
   struct nsrm_nschain_object                    *nschain_object_p = NULL;
   struct nsrm_nschain_object                    *nschain_object_node_scan_p = NULL;
   struct crm_tenant_config_info                  crm_tenant_config_info_p = {};
   union  nsrm_nschain_object_notification_data   notification_data = {};
   struct nsrm_notification_app_hook_info        *app_entry_p = NULL;

   uint32_t  hashkey,apphookoffset,index = 0,magic = 0;
   uint8_t   heap_b, status_b = FALSE;
   int32_t   status = NSRM_SUCCESS,ret_value;
   uchar8_t* hashobj_p = NULL;
   uint64_t  tenant_handle,offset;
   uint64_t  nschain_object_handle;

   if(nsrm_nschain_object_table_g == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "No nschain object hash table")
      return NSRM_FAILURE;
   }

   if(key_info_p->name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nschain object name is null");
      return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
   }
    
   ret_value = crm_get_tenant_handle(key_info_p->tenant_name_p,&tenant_handle);
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Tenant doesn't exist . So create tenant");
      strcpy(crm_tenant_config_info_p.tenant_name , key_info_p->tenant_name_p); 
      ret_value = crm_add_tenant(&crm_tenant_config_info_p , &tenant_handle);
      if(ret_value != NSRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Tenant could not be added");
         return NSRM_FAILURE;
      }
   }

   if(number_of_config_params == 0)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"No parameters to be configured");
      return NSRM_FAILURE;
   } 
  
   CNTLR_RCU_READ_LOCK_TAKE();
   
   /*Calculate hash value*/
   hashkey = nsrm_get_hashval_byname(key_info_p->name_p,nsrm_no_of_nschain_object_buckets_g);
   offset = NSRM_NSCHAIN_OBJECT_TBL_OFFSET;
   do
   {
      MCHASH_BUCKET_SCAN(nsrm_nschain_object_table_g ,hashkey ,nschain_object_node_scan_p,
                         struct nsrm_nschain_object* ,offset)
      {
        if(strcmp(key_info_p->name_p,nschain_object_node_scan_p->name_p) !=0)
          continue;
        OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Duplicate nschain object name : %s",key_info_p->name_p);
        status = NSRM_ERROR_DUPLICATE_RESOURCE;
        break;
      }
     ret_value = mempool_get_mem_block(nsrm_nschain_object_mempool_g,
                                       ( uchar8_t **)&nschain_object_p,&heap_b);
     if(ret_value!=MEMPOOL_SUCCESS)
     {
        OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to get memory block");
        status =  NSRM_FAILURE;
        break;
     }
     /*
     if(config_info_p[3].value.high_threshold < config_info_p[4].value.low_threshold)
     {
        OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "High threshold should be not less than low threshold");
        return NSRM_FAILURE;
     }
     NSM */
     nschain_object_p->heap_b = heap_b;
     nschain_object_p->name_p = (char*)calloc(1,(strlen(key_info_p->name_p)+1));
     nschain_object_p->tenant_name_p = (char*)calloc(1,(strlen(key_info_p->tenant_name_p)+1));
     strcpy(nschain_object_p->name_p , key_info_p->name_p);
     strcpy(nschain_object_p->tenant_name_p , key_info_p->tenant_name_p);
     
     /*
     nschain_object_p->nschain_type             = config_info_p[0].value.nschain_type;
     nschain_object_p->load_sharing_algorithm_e = config_info_p[1].value.load_sharing_algorithm_e;
     nschain_object_p->load_indication_type_e   = config_info_p[2].value.load_indication_type_e; 
     nschain_object_p->high_threshold           = config_info_p[3].value.high_threshold;
     nschain_object_p->low_threshold            = config_info_p[4].value.low_threshold;
     NSM */
     nschain_object_p->packet_fields_to_hash    = config_info_p[0].value.packet_fields_to_hash;
     nschain_object_p->admin_status_e           = config_info_p[1].value.admin_status_e;            
     nschain_object_p->operational_status_e     = OPER_DISABLE;
     nschain_object_p->no_of_nw_service_objects = 0;
     nschain_object_p->nwservice_objects_list     = NULL;
     nschain_object_p->nwservice_instance_count   = 0;
     nschain_object_p->nschain_usage_count        = 0;

     hashobj_p = (uchar8_t *)nschain_object_p + NSRM_NSCHAIN_OBJECT_TBL_OFFSET;
     MCHASH_APPEND_NODE(nsrm_nschain_object_table_g, hashkey, nschain_object_p,index, magic, hashobj_p, status_b);
 
     if(FALSE == status_b)
     {
        OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR," Failed to append nschain object node to Hash table");
        status = NSRM_NSCHAIN_OBJECT_ADD_FAIL;
        break;
     }
     nschain_object_handle   = magic;
     nschain_object_handle   = ((nschain_object_handle<<32) | (index));
     nschain_object_p->magic = magic;
     nschain_object_p->index = index;
     nschain_object_p->nschain_object_handle = nschain_object_handle;
      
     apphookoffset = NSRM_NOTIFICATION_APP_HOOK_OFFSET;
     OF_LIST_INIT(nschain_object_p->attributes, nsrm_attributes_free_attribute_entry_rcu);
     notification_data.add_del.nschain_object_name_p = (char*)calloc(1,(strlen(nschain_object_p->name_p)+1));
     strcpy(notification_data.add_del.nschain_object_name_p ,nschain_object_p->name_p);
     notification_data.add_del.nschain_object_handle = nschain_object_p->nschain_object_handle;
   
     OF_LIST_SCAN(nsrm_nschain_object_notifications[NSRM_NSCHAIN_OBJECT_ADDED], 
                  app_entry_p,
                  struct nsrm_notification_app_hook_info *,
                  apphookoffset)
     {
        ((nsrm_nschain_object_notifier)(app_entry_p->call_back))(NSRM_NSCHAIN_OBJECT_ADDED,
                                                                   nschain_object_handle,
                                                                   notification_data,
                                                                   app_entry_p->cbk_arg1,
                                                                   app_entry_p->cbk_arg2
                                                                   );
     }
   
     OF_LIST_SCAN(nsrm_nschain_object_notifications[NSRM_NSCHAIN_OBJECT_ALL] ,
                  app_entry_p,
                  struct nsrm_notification_app_hook_info *,
                  apphookoffset)
     {
        ((nsrm_nschain_object_notifier)(app_entry_p->call_back))(NSRM_NSCHAIN_OBJECT_ADDED,
                                                                   nschain_object_handle,
                                                                   notification_data,
                                                                   app_entry_p->cbk_arg1,
                                                                   app_entry_p->cbk_arg2
                                                                   );
     }

 
     CNTLR_RCU_READ_LOCK_RELEASE();
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "nschain object added successfully to the hashtable.");
     return NSRM_SUCCESS;
}while(0);

    if(status != NSRM_SUCCESS)
    {
      free(nschain_object_p->name_p);
      free(nschain_object_p->tenant_name_p);
      if(nschain_object_p)
      {
        if(mempool_release_mem_block(nsrm_nschain_object_mempool_g,(uchar8_t*)&nschain_object_p,
                                     nschain_object_p->heap_b) != MEMPOOL_SUCCESS)

        { 
           OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to release  allocated memory block.");
        }

     }
   }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return status;
}

 
int32_t nsrm_del_nschain_object(
        struct nsrm_nschain_object_key*   key_info_p)

{
   struct nsrm_nschain_object                    *nschain_object_p = NULL;
   union  nsrm_nschain_object_notification_data  notification_data = {};
   struct nsrm_notification_app_hook_info        *app_entry_p = NULL;

   uint32_t  index,magic;
   int32_t   lstoffset,offset,ret_value;
   uint8_t   status_b = FALSE;
   uint64_t  nschain_object_handle;

   if(nsrm_nschain_object_table_g == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"nschain hash table doesnt exist");
      return NSRM_FAILURE;
   }

   if(key_info_p->name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"nschain object name is invalid");
      return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
   }
 
   ret_value = nsrm_get_nschain_object_handle(key_info_p->name_p ,&nschain_object_handle);
   if(ret_value != NSRM_SUCCESS)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"nschain object doesnt exist");
     return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
   }

   magic = (uint32_t)(nschain_object_handle >>32);
   index = (uint32_t)nschain_object_handle;

   CNTLR_RCU_READ_LOCK_TAKE();
   MCHASH_ACCESS_NODE(nsrm_nschain_object_table_g,index,magic,nschain_object_p,status_b);
   if(FALSE == status_b)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"no node found");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   }
   if((strcmp(key_info_p->tenant_name_p ,nschain_object_p->tenant_name_p) != 0))
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"NS chain not attached to this tenant");
      CNTLR_RCU_READ_LOCK_RELEASE(); 
      return NSRM_FAILURE;
   }
   if(nschain_object_p->nwservice_instance_count != 0)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"first delete the nwservices attached to this chain");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   }
   if(nschain_object_p->nschain_usage_count != 0)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"This nschain is used by a selection rule");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_NSCHAIN_USED_IN_SELECTION_RULE;
   } 
   lstoffset = NSRM_NOTIFICATION_APP_HOOK_OFFSET;
   notification_data.add_del.nschain_object_name_p = (char*)calloc(1,(strlen(nschain_object_p->name_p)+1));
   strcpy(notification_data.add_del.nschain_object_name_p ,nschain_object_p->name_p);
   notification_data.add_del.nschain_object_handle = nschain_object_p->nschain_object_handle;

   OF_LIST_SCAN(nsrm_nschain_object_notifications[NSRM_NSCHAIN_OBJECT_DELETED] ,
                  app_entry_p,
                  struct nsrm_notification_app_hook_info *,
                  lstoffset)
   {
      ((nsrm_nschain_object_notifier)(app_entry_p->call_back))(NSRM_NSCHAIN_OBJECT_DELETED,
                                                                   nschain_object_handle,
                                                                   notification_data,
                                                                   app_entry_p->cbk_arg1,
                                                                   app_entry_p->cbk_arg2
                                                                   );
   } 
   
   OF_LIST_SCAN(nsrm_nschain_object_notifications[NSRM_NSCHAIN_OBJECT_ALL] ,
                  app_entry_p,
                  struct nsrm_notification_app_hook_info *,
                  lstoffset)
   {
      ((nsrm_nschain_object_notifier)(app_entry_p->call_back))(NSRM_NSCHAIN_OBJECT_DELETED,
                                                                   nschain_object_handle,
                                                                   notification_data,
                                                                   app_entry_p->cbk_arg1,
                                                                   app_entry_p->cbk_arg2
                                                                   );
   }

   offset = NSRM_NSCHAIN_OBJECT_TBL_OFFSET;
   MCHASH_DELETE_NODE_BY_REF(nsrm_nschain_object_table_g, index, magic, struct nsrm_nschain_object *,
                            offset, status_b);
   CNTLR_RCU_READ_LOCK_RELEASE();
   if(status_b == TRUE)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "nschain object deleted successfully");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_SUCCESS;
   }
   else
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "failed to delete nschain object");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_NSCHAIN_OBJECT_DEL_FAIL;
   }
}

int32_t nsrm_modify_nschain_object(
          struct  nsrm_nschain_object_key*           key_info_p,
          int32_t number_of_config_params,
          struct  nsrm_nschain_object_config_info*   config_info_p)
{
   struct   nsrm_nschain_object     *nschain_object_node_scan_p = NULL;
   int32_t  ret_value , nschain_object_node_found = 0;
   uint64_t nschain_object_handle;
   
   if(nsrm_nschain_object_table_g == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"No hash table exists");
      return NSRM_FAILURE;
   }

   if(key_info_p->name_p == NULL || key_info_p->tenant_name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Invalid Key info");
      return NSRM_FAILURE;
   }

   ret_value = nsrm_get_nschain_object_handle(key_info_p->name_p , &nschain_object_handle);
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Invalid nschain object name");
      return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
   }

   ret_value = nsrm_get_nschain_byhandle(nschain_object_handle , &nschain_object_node_scan_p);
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Invalid nschain object name");
      return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
   } 
   if(strcmp(key_info_p->tenant_name_p,nschain_object_node_scan_p->tenant_name_p) != 0)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"NS chain not present in this tenant");
      return NSRM_FAILURE;
   }
   else
   {
      nschain_object_node_found =1;
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"nschain name found to modify %s",key_info_p->name_p);
   } 
   if(!nschain_object_node_found)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"nschain name NOT found to modify %s" , key_info_p->name_p);
     return NSRM_NSCHAIN_OBJECT_MOD_FAIL;
   }
   CNTLR_RCU_READ_LOCK_TAKE();

   nschain_object_node_scan_p->name_p = (char*)malloc(128);
   nschain_object_node_scan_p->tenant_name_p =(char*)malloc(128);
   strcpy(nschain_object_node_scan_p->name_p , key_info_p->name_p);
   strcpy(nschain_object_node_scan_p->tenant_name_p , key_info_p->tenant_name_p);
   
   /*
   nschain_object_node_scan_p->nschain_type             = config_info_p[0].value.nschain_type;
   nschain_object_node_scan_p->load_sharing_algorithm_e = config_info_p[1].value.load_sharing_algorithm_e;
   nschain_object_node_scan_p->load_indication_type_e   = config_info_p[2].value.load_indication_type_e;
   if(config_info_p[3].value.high_threshold < config_info_p[4].value.low_threshold)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "High threshold should not be less than low threshold"); 
      return NSRM_FAILURE;
   }
   nschain_object_node_scan_p->high_threshold           = config_info_p[3].value.high_threshold;
   nschain_object_node_scan_p->low_threshold            = config_info_p[4].value.low_threshold;
   NSM */
   
   nschain_object_node_scan_p->packet_fields_to_hash    = config_info_p[0].value.packet_fields_to_hash;
   nschain_object_node_scan_p->admin_status_e           = config_info_p[1].value.admin_status_e;

   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "nschain Modified Successfully!");
   CNTLR_RCU_READ_LOCK_RELEASE();
   return NSRM_SUCCESS;
}
  
     
int32_t nsrm_get_first_nschain_objects(
         int32_t  number_of_nschain_objects_requested,
         int32_t* number_of_nschain_objects_returned_p,
         struct   nsrm_nschain_object_record* recs_p)
{
   struct nsrm_nschain_object         *nschain_object_node_scan_p = NULL;
   
   uint64_t   hashkey = 0;
   uint64_t   offset;
   uint32_t   nschain_objects_returned = 0;
   uint32_t   nschain_objects_requested = number_of_nschain_objects_requested;
   int32_t    status = NSRM_FAILURE ;
 
   if(nsrm_nschain_object_table_g == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"No hash table exists");
      return NSRM_FAILURE;
   }
 
   CNTLR_RCU_READ_LOCK_TAKE();
   offset = NSRM_NSCHAIN_OBJECT_TBL_OFFSET;
   MCHASH_TABLE_SCAN(nsrm_nschain_object_table_g , hashkey)
   {
      MCHASH_BUCKET_SCAN(nsrm_nschain_object_table_g, hashkey, nschain_object_node_scan_p,
                         struct nsrm_nschain_object*,offset)
      {

         if((strlen(nschain_object_node_scan_p->name_p) > NSRM_MAX_NAME_LENGTH) || (strlen(nschain_object_node_scan_p->tenant_name_p) > NSRM_MAX_NAME_LENGTH))
         {
            CNTLR_RCU_READ_LOCK_RELEASE(); 
            return NSRM_ERROR_LENGTH_NOT_ENOUGH;
         }
         strcpy(recs_p[nschain_objects_returned].key.name_p , nschain_object_node_scan_p->name_p);
         strcpy(recs_p[nschain_objects_returned].key.tenant_name_p , nschain_object_node_scan_p->tenant_name_p);

         /*
         recs_p[nschain_objects_returned].info[0].value.nschain_type   = nschain_object_node_scan_p->nschain_type;
         recs_p[nschain_objects_returned].info[1].value.load_sharing_algorithm_e = nschain_object_node_scan_p->load_sharing_algorithm_e;
         recs_p[nschain_objects_returned].info[2].value.load_indication_type_e = nschain_object_node_scan_p->load_indication_type_e;
         recs_p[nschain_objects_returned].info[3].value.high_threshold = nschain_object_node_scan_p->high_threshold;
         recs_p[nschain_objects_returned].info[4].value.low_threshold  = nschain_object_node_scan_p->low_threshold ;
         NSM */

         recs_p[nschain_objects_returned].info[0].value.packet_fields_to_hash = nschain_object_node_scan_p->packet_fields_to_hash;
         recs_p[nschain_objects_returned].info[1].value.admin_status_e        = nschain_object_node_scan_p->admin_status_e;
 
         status = NSRM_SUCCESS;
         number_of_nschain_objects_requested--;
         nschain_objects_returned++;
         
         if(number_of_nschain_objects_requested)
           continue;
         else
           break;
      }
      if(number_of_nschain_objects_requested)
        continue;
      else
        break;
   }
   if(nschain_objects_requested != nschain_objects_returned)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Invalid number of objects returned / requested ");
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_FAILURE; 
   }
   if(status == NSRM_SUCCESS)  
   { 
      *number_of_nschain_objects_returned_p = nschain_objects_returned;
       CNTLR_RCU_READ_LOCK_RELEASE();
       return status;
   }
   CNTLR_RCU_READ_LOCK_RELEASE();
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "No More Entries in the nschain hash table!.");
   return NSRM_ERROR_NO_MORE_ENTRIES;

}
  
int32_t nsrm_get_next_nschain_objects(
        struct   nsrm_nschain_object_key* relative_record_key_p,
        int32_t  number_of_nschain_objects_requested,
        int32_t* number_of_nschain_objects_returned_p,
        struct   nsrm_nschain_object_record* recs_p)
{
   struct nsrm_nschain_object    *nschain_object_node_scan_p = NULL;

   uint32_t  hashkey = 0;
   uint64_t  offset ;
   uint8_t   current_entry_found_b;
   int32_t   nschain_objects_returned = 0; 
   int32_t   nschain_objects_requested = number_of_nschain_objects_requested;
   int32_t   status = NSRM_FAILURE; 
  
   offset = NSRM_NSCHAIN_OBJECT_TBL_OFFSET;
   current_entry_found_b = FALSE;
   CNTLR_RCU_READ_LOCK_TAKE();
   MCHASH_TABLE_SCAN(nsrm_nschain_object_table_g,hashkey)
   {
      MCHASH_BUCKET_SCAN(nsrm_nschain_object_table_g,hashkey,nschain_object_node_scan_p,
                         struct nsrm_nschain_object*, offset)
      {
         if(current_entry_found_b == FALSE)
         {
             if(((strcmp(relative_record_key_p->name_p , nschain_object_node_scan_p->name_p)) != 0) ||
        ((strcmp(relative_record_key_p->tenant_name_p,nschain_object_node_scan_p->tenant_name_p) !=0)))
                 continue; 
            else
            {
               OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "nschain entry found copy next nschain"); 
               current_entry_found_b = TRUE;
               continue;
            }
         }
         strcpy(recs_p[nschain_objects_returned].key.name_p , nschain_object_node_scan_p->name_p);
         strcpy(recs_p[nschain_objects_returned].key.tenant_name_p , nschain_object_node_scan_p->tenant_name_p);
         
         /*
         recs_p[nschain_objects_returned].info[0].value.nschain_type             = nschain_object_node_scan_p->nschain_type;
         recs_p[nschain_objects_returned].info[1].value.load_sharing_algorithm_e = nschain_object_node_scan_p->load_sharing_algorithm_e;
         recs_p[nschain_objects_returned].info[2].value.load_indication_type_e   = nschain_object_node_scan_p->load_indication_type_e;
         recs_p[nschain_objects_returned].info[3].value.high_threshold           = nschain_object_node_scan_p->high_threshold;
         recs_p[nschain_objects_returned].info[4].value.low_threshold            = nschain_object_node_scan_p->low_threshold ;
         NSM */

         recs_p[nschain_objects_returned].info[0].value.packet_fields_to_hash    = nschain_object_node_scan_p->packet_fields_to_hash;
         recs_p[nschain_objects_returned].info[1].value.admin_status_e           = nschain_object_node_scan_p->admin_status_e;
 
         status = NSRM_SUCCESS;
         number_of_nschain_objects_requested--;
         nschain_objects_returned++;
         if(number_of_nschain_objects_requested)
           continue;
         else
           break;
      }
      if(number_of_nschain_objects_requested)
        continue;
      else
        break;
   }
   if(nschain_objects_requested != nschain_objects_returned)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Invalid number of objects returned / requested ");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE; 
   }
   if(status == NSRM_SUCCESS)
   {
      *number_of_nschain_objects_returned_p = nschain_objects_returned;
      CNTLR_RCU_READ_LOCK_RELEASE();
      return status;
   }
  
   CNTLR_RCU_READ_LOCK_RELEASE();
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to copy nschain object Info.....");

   if(current_entry_found_b == TRUE)
     return NSRM_ERROR_NO_MORE_ENTRIES;
   else
     return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
}

  
int32_t nsrm_get_exact_nschain_object(
        struct   nsrm_nschain_object_key*   key_info_p,
        struct   nsrm_nschain_object_record*   rec_p)
{
   struct nsrm_nschain_object   *nschain_object_node_scan_p = NULL;

   uint64_t hashkey,offset;
   uint8_t  current_entry_found_b = FALSE;
   
   if(nsrm_nschain_object_table_g == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nschain object hash table doesnt exist");
      return NSRM_FAILURE;
   }     

  if(key_info_p->name_p == NULL || key_info_p->tenant_name_p == NULL)
  {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "such node doesnt exist");
     return NSRM_FAILURE;
  }
  
   hashkey = nsrm_get_hashval_byname(key_info_p->name_p,nsrm_no_of_nschain_object_buckets_g);
   CNTLR_RCU_READ_LOCK_TAKE();
   offset = NSRM_NSCHAIN_OBJECT_TBL_OFFSET;

   MCHASH_BUCKET_SCAN(nsrm_nschain_object_table_g, hashkey, nschain_object_node_scan_p,
                      struct nsrm_nschain_object*,offset)
   {  
      if((strcmp(key_info_p->name_p,nschain_object_node_scan_p->name_p) ==0) && (strcmp(key_info_p->tenant_name_p,nschain_object_node_scan_p->tenant_name_p) ==0))
      {
        current_entry_found_b = TRUE;
      }
      else
        continue;
      strcpy(rec_p->key.name_p , nschain_object_node_scan_p->name_p);
      strcpy(rec_p->key.tenant_name_p , nschain_object_node_scan_p->tenant_name_p);
      
      /*
      rec_p[0].info[0].value.nschain_type             = nschain_object_node_scan_p->nschain_type;
      rec_p[0].info[1].value.load_sharing_algorithm_e = nschain_object_node_scan_p->load_sharing_algorithm_e;
      rec_p[0].info[2].value.load_indication_type_e   = nschain_object_node_scan_p->load_indication_type_e;
      rec_p[0].info[3].value.high_threshold           = nschain_object_node_scan_p->high_threshold;
      rec_p[0].info[4].value.low_threshold            = nschain_object_node_scan_p->low_threshold ;
      */
  
      rec_p[0].info[0].value.packet_fields_to_hash    = nschain_object_node_scan_p->packet_fields_to_hash;
      rec_p[0].info[1].value.admin_status_e           = nschain_object_node_scan_p->admin_status_e;
    
      CNTLR_RCU_READ_LOCK_RELEASE(); 
      return NSRM_SUCCESS;
   }
   CNTLR_RCU_READ_LOCK_RELEASE(); 
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to copy nschain object Info.....");

   if(current_entry_found_b == TRUE)
     return NSRM_ERROR_NO_MORE_ENTRIES;
   else
     return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
}
int32_t nsrm_attach_nwservice_object_to_nschain_object(
          struct  nsrm_nwservice_attach_key* key_info_p)
{
   struct nschain_nwservice_object  *nschain_nwservice_object_p = NULL;
   struct nschain_nwservice_object  *nschain_nwservice_object_node_p = NULL;
   struct nsrm_nwservice_object     *nwservice_object_node_scan_p = NULL;
   struct nsrm_nschain_object       *nschain_object_node_scan_p = NULL;
   union  nsrm_nwservice_object_notification_data  notification_data = {};
   struct nsrm_notification_app_hook_info *app_entry_p = NULL;

   uchar8_t* hashobj_p;
   uint64_t hashkey ,offset,nwservice_object_handle,apphookoffset , attach_handle,nschain_object_handle;
   int32_t  ret_value , index =0 , magic =0 ;
   uint8_t  heap_b , status_b = FALSE;
   uint8_t  found_b = FALSE;

   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Entered");

   if(key_info_p->name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nwservice object name is null");
      return NSRM_ERROR_NWSERVICE_OBJECT_NAME_INVALID;
   }

   if(key_info_p->nschain_object_name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nschain object name is null");
      return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
   }

   ret_value = nsrm_get_nschain_object_handle(key_info_p->nschain_object_name_p ,&nschain_object_handle);
   if(ret_value != NSRM_SUCCESS)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nschain object doesnt exist");
     return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
   }

   ret_value = nsrm_get_nschain_byhandle(nschain_object_handle , &nschain_object_node_scan_p);
   if(ret_value != NSRM_SUCCESS)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nschain object doesnt exist");
     return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
   }
   
   ret_value = nsrm_get_nwservice_object_handle(key_info_p->name_p , &nwservice_object_handle);
   if(ret_value != NSRM_SUCCESS)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nwservice object doesnt exist");
     return NSRM_ERROR_NWSERVICE_OBJECT_NAME_INVALID;
   }
   
   ret_value = nsrm_get_nwservice_byhandle(nwservice_object_handle , &nwservice_object_node_scan_p);
   if(ret_value != NSRM_SUCCESS)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nwservice object doesnt exist");
     return NSRM_ERROR_NWSERVICE_OBJECT_NAME_INVALID;
   }

   CNTLR_RCU_READ_LOCK_TAKE(); 
  
   if(nschain_object_node_scan_p->nwservice_objects_list == NULL )
   {
     nschain_object_node_scan_p->nwservice_objects_list = (struct mchash_table*)calloc(1,(NSRM_MAX_NSCHAIN_NWSERVICE_OBJECT_ENTRIES)*sizeof(struct nschain_nwservice_object));
     ret_value = mchash_table_create(1 ,
                                    NSRM_MAX_NSCHAIN_NWSERVICE_OBJECT_ENTRIES,
                                    nsrm_free_nschain_nwservice_object_entry_rcu,
                                    &(nschain_object_node_scan_p->nwservice_objects_list));
     if(ret_value != NSRM_SUCCESS)
     {
        OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"table not created");
        CNTLR_RCU_READ_LOCK_RELEASE();
        return NSRM_FAILURE;
     }
   } 
    
  hashkey = 0;
  offset = NSRM_NSCHAIN_NWSERVICE_OBJECT_TBL_OFFSET;
  MCHASH_TABLE_SCAN(nschain_object_node_scan_p->nwservice_objects_list,hashkey)
  {
    MCHASH_BUCKET_SCAN(nschain_object_node_scan_p->nwservice_objects_list,hashkey,
                       nschain_nwservice_object_p,
                       struct nschain_nwservice_object*, offset)
    {
       if(nschain_nwservice_object_p->sequence_number == key_info_p->sequence_number)
       {
          OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"Some other nwservice is occupying this sequence number");
          CNTLR_RCU_READ_LOCK_RELEASE();
          return NSRM_FAILURE;
       }
       else if((strcmp(nschain_nwservice_object_p->name_p,key_info_p->name_p)) == 0) 
       {
           OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"The nwservice is already attached to this nschain");
           CNTLR_RCU_READ_LOCK_RELEASE();
           return NSRM_FAILURE;
       }
       else
       {
         OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"conitnuing");
         continue;
       }
    } 
      break;
  }    
  
   ret_value = mempool_get_mem_block(nsrm_nschain_nwservice_object_mempool_g,
                                      ( uchar8_t **)&nschain_nwservice_object_p,&heap_b);
   if(ret_value != MEMPOOL_SUCCESS)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to get mempool");
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_FAILURE;
   } 
   nschain_nwservice_object_p->name_p = (char *)calloc(1,(strlen(nwservice_object_node_scan_p->name_p)+1));
   strcpy(nschain_nwservice_object_p->name_p , nwservice_object_node_scan_p->name_p); 
   nschain_nwservice_object_p->nwservice_object_saferef = nwservice_object_node_scan_p->nwservice_object_handle;
   nschain_nwservice_object_p->sequence_number          = key_info_p->sequence_number;
   nwservice_object_node_scan_p->attach_status          = 1;

   nschain_nwservice_object_p->heap_b = heap_b; 
   hashobj_p = (uchar8_t *)nschain_nwservice_object_p + NSRM_NSCHAIN_NWSERVICE_OBJECT_TBL_OFFSET;
   hashkey = 0;
   offset = NSRM_NSCHAIN_NWSERVICE_OBJECT_TBL_OFFSET;
   if(nschain_object_node_scan_p->no_of_nw_service_objects != 0)
   {
     MCHASH_TABLE_SCAN(nschain_object_node_scan_p->nwservice_objects_list,hashkey)
     {
       MCHASH_BUCKET_SCAN(nschain_object_node_scan_p->nwservice_objects_list,hashkey,
                           nschain_nwservice_object_node_p,
                           struct nschain_nwservice_object*, offset)
       {
         if((nschain_nwservice_object_node_p->sequence_number) < key_info_p->sequence_number)
            continue;
         else 
         {
            found_b = TRUE;
            break;          
         }
      } 
     break;
    } 
  }
  if(nschain_object_node_scan_p->no_of_nw_service_objects == 0 || found_b == FALSE)
  {
    MCHASH_APPEND_NODE(nschain_object_node_scan_p->nwservice_objects_list,hashkey, nschain_nwservice_object_p,index, magic, hashobj_p, status_b);
    if(status_b == FALSE)
    {
       OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"Failed to append node");
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_FAILURE;
    }
    nschain_object_node_scan_p->no_of_nw_service_objects++;  /* NSM Added on March 29th 2014 */
    attach_handle = magic;
    attach_handle = (((attach_handle)<<32) | (index));
    nschain_nwservice_object_p->magic = magic;
    nschain_nwservice_object_p->index = index;
    nschain_nwservice_object_p->nwservice_object_saferef = attach_handle;

    CNTLR_RCU_READ_LOCK_RELEASE();
    return NSRM_SUCCESS;
   }
  
   if(found_b == TRUE)
   {  
     MCHASH_INSERT_NODE_BEFORE(nschain_object_node_scan_p->nwservice_objects_list,
                             nschain_nwservice_object_node_p->nschain_nwservice_object_tbl_link,
                             hashkey, nschain_nwservice_object_p,index, magic, hashobj_p, status_b);
    
     if(FALSE == status_b)
     {
        OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR," Failed to append nschain object node to Hash table");
        CNTLR_RCU_READ_LOCK_RELEASE();
        if(mempool_release_mem_block(nsrm_nschain_nwservice_object_mempool_g,
                                    (uchar8_t*)&nschain_nwservice_object_p,
                                    nschain_nwservice_object_p->heap_b) != MEMPOOL_SUCCESS)
        OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to release  allocated memory block.");
        return NSRM_NWSERVICE_OBJECT_ATTACH_TO_NSCHAIN_FAIL;
     }
    }
    attach_handle = magic;
    attach_handle = (((attach_handle)<<32) | (index));
    nschain_nwservice_object_p->magic = magic;
    nschain_nwservice_object_p->index = index;
    nschain_object_node_scan_p->no_of_nw_service_objects++; /* NSM Added on March 29th 2014 */
    nschain_nwservice_object_p->nwservice_object_saferef = attach_handle;
        
    apphookoffset = NSRM_NOTIFICATION_APP_HOOK_OFFSET;
    notification_data.attach.nwservice_object_name_p = (char*)calloc(1,(strlen(nwservice_object_node_scan_p->name_p)+1));
    notification_data.attach.nschain_object_name_p = (char*)calloc(1,(strlen(nschain_object_node_scan_p->name_p)+1));
    strcpy(notification_data.attach.nwservice_object_name_p ,nwservice_object_node_scan_p->name_p);
    strcpy(notification_data.attach.nschain_object_name_p ,nschain_object_node_scan_p->name_p);
    notification_data.attach.nwservice_object_handle = nwservice_object_node_scan_p->nwservice_object_handle;
    notification_data.attach.nschain_object_handle = nschain_object_node_scan_p->nschain_object_handle;

    OF_LIST_SCAN(nsrm_nwservice_object_notifications[NSRM_NWSERVICE_OBJECT_ATTACHED_TO_NSCHAIN_OBJECT], 
                  app_entry_p,
                  struct nsrm_notification_app_hook_info *,
                  apphookoffset)
    {
        ((nsrm_nwservice_object_notifier)(app_entry_p->call_back))(NSRM_NWSERVICE_OBJECT_ATTACHED_TO_NSCHAIN_OBJECT,
                                                                   nwservice_object_node_scan_p->nwservice_object_handle,
                                                                   notification_data,
                                                                   app_entry_p->cbk_arg1,
                                                                   app_entry_p->cbk_arg2
                                                                   );
    }
   
    OF_LIST_SCAN(nsrm_nwservice_object_notifications[NSRM_NWSERVICE_OBJECT_ALL] ,
                  app_entry_p,
                  struct nsrm_notification_app_hook_info *,
                  apphookoffset)
    {
        ((nsrm_nwservice_object_notifier)(app_entry_p->call_back))(NSRM_NWSERVICE_OBJECT_ATTACHED_TO_NSCHAIN_OBJECT,
                                                                   nwservice_object_node_scan_p->nwservice_object_handle,
                                                                   notification_data,
                                                                   app_entry_p->cbk_arg1,
                                                                   app_entry_p->cbk_arg2
                                                                   );
    }
  
    CNTLR_RCU_READ_LOCK_RELEASE();
    return NSRM_SUCCESS;
}  

int32_t nsrm_detach_nwservice_object_from_nschain_object(
          struct  nsrm_nwservice_attach_key* key_info_p)
{
   struct nschain_nwservice_object  *nschain_nwservice_object_p = NULL;
   struct nsrm_nwservice_object     *nwservice_object_node_scan_p = NULL;
   struct nsrm_nschain_object       *nschain_object_node_scan_p = NULL;
   union  nsrm_nwservice_object_notification_data  notification_data = {};
   struct nsrm_notification_app_hook_info *app_entry_p = NULL;
  
   int32_t   ret_value;
   uint64_t  offset,apphookoffset;
   uint8_t   status_b = FALSE, node_found_b = 0;  
   uint64_t  hashkey,nschain_object_handle, nwservice_object_handle;
 
   if(key_info_p->name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nwservice object name is null");
      return NSRM_ERROR_NWSERVICE_OBJECT_NAME_INVALID;
   }

   if(key_info_p->nschain_object_name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nschain object name is null");
      return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
   }

   CNTLR_RCU_READ_LOCK_TAKE();
   ret_value = nsrm_get_nwservice_object_handle(key_info_p->name_p , &nwservice_object_handle);
   if(ret_value != NSRM_SUCCESS)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nwservice object doesnt exist");
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_ERROR_NWSERVICE_OBJECT_NAME_INVALID;
   }

   ret_value = nsrm_get_nwservice_byhandle(nwservice_object_handle , &nwservice_object_node_scan_p);
   if(ret_value != NSRM_SUCCESS)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nwservice object doesnt exist");
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_ERROR_NWSERVICE_OBJECT_NAME_INVALID;
   }

    if(nwservice_object_node_scan_p->attach_status == 0)
     {
        OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"The nwservice is not attached to this nschain");
        CNTLR_RCU_READ_LOCK_RELEASE();
        return NSRM_FAILURE;
     }
   
    if(nwservice_object_node_scan_p->bypass_attach_count != 0)
    {
       OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"The nwservice is attached to a bypass rule");
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_NWSERVICE_USED_IN_BYPASS_RULE;
    }
    ret_value = nsrm_get_nschain_object_handle(key_info_p->nschain_object_name_p,&nschain_object_handle);
    if(ret_value != NSRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nschain object doesnt exist");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
    }

    ret_value = nsrm_get_nschain_byhandle(nschain_object_handle , &nschain_object_node_scan_p);
    if(ret_value != NSRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nschain object doesnt exist");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
    }
 
    hashkey = 0;
    offset = NSRM_NSCHAIN_NWSERVICE_OBJECT_TBL_OFFSET;
    MCHASH_TABLE_SCAN(nschain_object_node_scan_p->nwservice_objects_list,hashkey)
    {
      MCHASH_BUCKET_SCAN(nschain_object_node_scan_p->nwservice_objects_list,hashkey,
                         nschain_nwservice_object_p,struct nschain_nwservice_object *,offset)
      {      
        if(((strcmp(nschain_nwservice_object_p->name_p,key_info_p->name_p)) == 0) &&
            (nschain_nwservice_object_p->nwservice_object_saferef == 
             nwservice_object_node_scan_p->nwservice_object_handle)) 
        {
           OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"The intended nwservice is found");
           node_found_b = 1;
           break;
        }
     }
   }
    if(node_found_b == 0)
    {
       OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"Node not found");
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_FAILURE;
    }
    apphookoffset = NSRM_NOTIFICATION_APP_HOOK_OFFSET;
    notification_data.attach.nwservice_object_name_p = (char*)calloc(1,(strlen(nwservice_object_node_scan_p->name_p)+1));
    notification_data.attach.nschain_object_name_p = (char*)calloc(1,(strlen(nschain_object_node_scan_p->name_p)+1));
    strcpy(notification_data.attach.nwservice_object_name_p ,nwservice_object_node_scan_p->name_p);
    strcpy(notification_data.attach.nschain_object_name_p ,nschain_object_node_scan_p->name_p);
    notification_data.attach.nwservice_object_handle = nwservice_object_node_scan_p->nwservice_object_handle;
    notification_data.attach.nschain_object_handle = nschain_object_node_scan_p->nschain_object_handle;
    OF_LIST_SCAN(nsrm_nwservice_object_notifications[NSRM_NWSERVICE_OBJECT_DETACHED_FROM_NSCHAIN_OBJECT],                  app_entry_p,
                  struct nsrm_notification_app_hook_info *,
                  apphookoffset)
    {
        ((nsrm_nwservice_object_notifier)(app_entry_p->call_back))(NSRM_NWSERVICE_OBJECT_DETACHED_FROM_NSCHAIN_OBJECT,
                                                  nwservice_object_node_scan_p->nwservice_object_handle,
                                                  notification_data,
                                                  app_entry_p->cbk_arg1,
                                                  app_entry_p->cbk_arg2
                                                                   );
    }

    OF_LIST_SCAN(nsrm_nwservice_object_notifications[NSRM_NWSERVICE_OBJECT_ALL] ,
                  app_entry_p,
                  struct nsrm_notification_app_hook_info *,
                  apphookoffset)
    {
        ((nsrm_nwservice_object_notifier)(app_entry_p->call_back))(NSRM_NWSERVICE_OBJECT_DETACHED_FROM_NSCHAIN_OBJECT,
                                                 nwservice_object_node_scan_p->nwservice_object_handle,
                                                 notification_data,
                                                 app_entry_p->cbk_arg1,
                                                 app_entry_p->cbk_arg2
                                                                   );
    }
   

   MCHASH_DELETE_NODE_BY_REF((nschain_object_node_scan_p->nwservice_objects_list),nschain_nwservice_object_p->index,nschain_nwservice_object_p->magic,
                             struct nschain_nwservice_object*,offset, status_b);
   
   nschain_object_node_scan_p->no_of_nw_service_objects--;   /* NSM Added on March 29th 2014 */
   CNTLR_RCU_READ_LOCK_RELEASE();
   if(status_b == TRUE)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "nwservice object detached successfully");
      return NSRM_SUCCESS;
   }
   else
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "failed to detach nwservice object");
      return NSRM_NWSERVICE_OBJECT_DETACH_FROM_NSCHAIN_FAIL;
   }
}

int32_t nsrm_get_first_nwservice_objects_in_nschain_object(
             struct   nsrm_nschain_object_key* nschain_object_key_p,
             int32_t  number_of_nwservice_objects_requested,
             int32_t* number_of_nwservice_objects_returned_p,
             struct   nsrm_nwservice_attach_key* recs_p)
{
   struct nsrm_nschain_object         *nschain_object_node_scan_p = NULL;
   struct nschain_nwservice_object    *nschain_nwservice_object_p = NULL;
   
   uint64_t   hashkey = 0;
   uint64_t   lstoffset,nschain_object_handle;
   uint32_t   nwservice_objects_returned = 0;
   uint32_t   nwservice_objects_requested = number_of_nwservice_objects_requested;
   int32_t    status = NSRM_FAILURE,ret_value ;
   
   if(nschain_object_key_p->name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nwservice object name is null");
      return NSRM_ERROR_NWSERVICE_OBJECT_NAME_INVALID;
   }

   ret_value = nsrm_get_nschain_object_handle(nschain_object_key_p->name_p,&nschain_object_handle);
   if(ret_value != NSRM_SUCCESS)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nschain object doesnt exist");
     return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
   }

   ret_value = nsrm_get_nschain_byhandle(nschain_object_handle , &nschain_object_node_scan_p);
   if(ret_value != NSRM_SUCCESS)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nschain object doesnt exist");
     return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
   }

   if(nschain_object_node_scan_p->nwservice_objects_list == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"List of nwservie objects is null");
      return NSRM_FAILURE;
   }
   
   hashkey = 0;
   CNTLR_RCU_READ_LOCK_TAKE();
   lstoffset = NSRM_NSCHAIN_NWSERVICE_OBJECT_TBL_OFFSET;
   MCHASH_TABLE_SCAN(nschain_object_node_scan_p->nwservice_objects_list , hashkey)
   {
      MCHASH_BUCKET_SCAN(nschain_object_node_scan_p->nwservice_objects_list, hashkey, 
                         nschain_nwservice_object_p,
                         struct nschain_nwservice_object*,lstoffset)
      {
         strcpy(recs_p[nwservice_objects_returned].name_p , nschain_nwservice_object_p->name_p);
         recs_p[nwservice_objects_returned].sequence_number = nschain_nwservice_object_p->sequence_number;
         status = NSRM_SUCCESS;
         number_of_nwservice_objects_requested--;
         nwservice_objects_returned++;
         if(number_of_nwservice_objects_requested)
           continue;
         else
           break;
      }
      if(number_of_nwservice_objects_requested)
        continue;
      else
        break;
   }
   if(nwservice_objects_requested != nwservice_objects_returned)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Invalid number of objects returned / requested ");
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_FAILURE;
   }
   if(status == NSRM_SUCCESS)
   {
      *number_of_nwservice_objects_returned_p = nwservice_objects_returned;
       CNTLR_RCU_READ_LOCK_RELEASE();
       return status;
   }
   CNTLR_RCU_READ_LOCK_RELEASE();
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "No More Entries in the nwservice hash table!.");
   return NSRM_ERROR_NO_MORE_ENTRIES;

}

int32_t nsrm_get_next_nwservice_object_in_nschain_object(
            struct   nsrm_nschain_object_key* nschain_object_key_p,
            struct   nsrm_nwservice_object_key* relative_record_key_p,
            int32_t  number_of_nwservice_objects_requested,
            int32_t* number_of_nwservice_objects_returned_p,
            struct   nsrm_nwservice_attach_key* recs_p)
{

   struct nsrm_nschain_object         *nschain_object_node_scan_p = NULL;
   struct nschain_nwservice_object    *nschain_nwservice_object_p = NULL;
   
   uint64_t   hashkey = 0 , nwservice_object_handle,nschain_object_handle,lstoffset;
   uint32_t   nwservice_objects_returned = 0;
   uint32_t   nwservice_objects_requested = number_of_nwservice_objects_requested;
   int32_t    status = NSRM_FAILURE,ret_value ;
   uint8_t    entry_found_b = FALSE;
   
   if(nschain_object_key_p->name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nschain object name is null");
      return NSRM_ERROR_NWSERVICE_OBJECT_NAME_INVALID;
   }

   ret_value = nsrm_get_nschain_object_handle(nschain_object_key_p->name_p,&nschain_object_handle);
   if(ret_value != NSRM_SUCCESS)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nschain object doesnt exist");
     return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
   }

   ret_value = nsrm_get_nschain_byhandle(nschain_object_handle , &nschain_object_node_scan_p);
   if(ret_value != NSRM_SUCCESS)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nschain object doesnt exist");
     return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
   }


   if(nschain_object_node_scan_p->nwservice_objects_list == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"List of nwservie objects is null");
      return NSRM_FAILURE;
   }
  
   ret_value = nsrm_get_nwservice_object_handle(relative_record_key_p->name_p,&nwservice_object_handle);
   if(ret_value != NSRM_SUCCESS)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"nwservice doesnt exist");
     return NSRM_ERROR_NWSERVICE_OBJECT_NAME_INVALID;
   }

   hashkey = 0;
   CNTLR_RCU_READ_LOCK_TAKE();
   lstoffset = NSRM_NSCHAIN_NWSERVICE_OBJECT_TBL_OFFSET;
   MCHASH_TABLE_SCAN(nschain_object_node_scan_p->nwservice_objects_list , hashkey)
   {
      MCHASH_BUCKET_SCAN(nschain_object_node_scan_p->nwservice_objects_list, hashkey, 
                         nschain_nwservice_object_p,
                         struct nschain_nwservice_object*,lstoffset)
      {
         if(entry_found_b == FALSE)
         {
           if(strcmp(relative_record_key_p->name_p, nschain_nwservice_object_p->name_p))
              continue;
         
           else
           {
             entry_found_b = TRUE;
             continue;
           }
         } 
         if(entry_found_b != TRUE)
         {
            OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"no more recoreds found");
            CNTLR_RCU_READ_LOCK_RELEASE();
            return NSRM_FAILURE;
         }
         strcpy(recs_p[nwservice_objects_returned].name_p , nschain_nwservice_object_p->name_p);
         recs_p[nwservice_objects_returned].sequence_number =  nschain_nwservice_object_p->sequence_number;
         status = NSRM_SUCCESS;
         number_of_nwservice_objects_requested--;
         nwservice_objects_returned++;
         
         if(number_of_nwservice_objects_requested)
           continue;
         else
           break;
      }
      if(nschain_nwservice_object_p == NULL)
      {
       CNTLR_RCU_READ_LOCK_RELEASE();   
       return NSRM_FAILURE;
      }    
      if(number_of_nwservice_objects_requested)
        continue;
      else
        break;
   }

   if(nwservice_objects_requested != nwservice_objects_returned)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Invalid number of objects returned / requested ");
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_FAILURE;
   }
   if(status == NSRM_SUCCESS)
   {
      *number_of_nwservice_objects_returned_p = nwservice_objects_returned;
       CNTLR_RCU_READ_LOCK_RELEASE();
       return status;
   }
   CNTLR_RCU_READ_LOCK_RELEASE();
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "No More Entries in the nwservice hash table!.");
   return NSRM_ERROR_NO_MORE_ENTRIES;

}
     
int32_t nsrm_get_exact_nwservice_object_in_nschain_object(   
            struct   nsrm_nschain_object_key* nschain_object_key_p,
            struct   nsrm_nwservice_object_key*   key_info_p,
            struct   nsrm_nwservice_attach_key*   rec_p
          )
{
   struct nsrm_nschain_object         *nschain_object_node_scan_p = NULL;
   struct nschain_nwservice_object    *nschain_nwservice_object_p = NULL;
   
   uint64_t   hashkey = 0 , nwservice_object_handle;
   uint32_t   lstoffset;
   int32_t    status = NSRM_FAILURE,ret_value = NSRM_SUCCESS;
   uint64_t   nschain_object_handle;
   uint8_t    nschain_entry_found_b = FALSE;
   
   if(nschain_object_key_p->name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nwservice object name is null");
      return NSRM_ERROR_NWSERVICE_OBJECT_NAME_INVALID;
   }
   
   ret_value = nsrm_get_nschain_object_handle(nschain_object_key_p->name_p,&nschain_object_handle);
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Chain doesnt exist");
      return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
   }
   
   ret_value = nsrm_get_nschain_byhandle(nschain_object_handle , &nschain_object_node_scan_p);
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Chain doesnt exist");
      return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
   }

   if(nschain_object_node_scan_p->nwservice_objects_list == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"List of nwservie objects is null");
      return NSRM_FAILURE;
   }
 
   ret_value = nsrm_get_nwservice_object_handle(key_info_p->name_p,&nwservice_object_handle);
   if(ret_value != NSRM_SUCCESS)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"nwservice doesnt exist");
     return NSRM_ERROR_NWSERVICE_OBJECT_NAME_INVALID;
   }
   
   hashkey = 0;
   CNTLR_RCU_READ_LOCK_TAKE();
   lstoffset = NSRM_NSCHAIN_NWSERVICE_OBJECT_TBL_OFFSET;
   MCHASH_TABLE_SCAN(nschain_object_node_scan_p->nwservice_objects_list , hashkey)
   {
      MCHASH_BUCKET_SCAN(nschain_object_node_scan_p->nwservice_objects_list, hashkey, 
                         nschain_nwservice_object_p,
                         struct nschain_nwservice_object*,lstoffset)
      {
         if(!strcmp(key_info_p->name_p,nschain_nwservice_object_p->name_p))
         {
            nschain_entry_found_b = TRUE;
            break;
         }
      }
      break;
   }

   if(nschain_entry_found_b == FALSE)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Exact entry not found");
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_FAILURE;
   }
 
   strcpy(rec_p->name_p , nschain_nwservice_object_p->name_p);
   rec_p->sequence_number = nschain_nwservice_object_p->sequence_number;
   status = NSRM_SUCCESS;
   if(status == NSRM_SUCCESS)
   {
       CNTLR_RCU_READ_LOCK_RELEASE();
       return status;
   }
   CNTLR_RCU_READ_LOCK_RELEASE();
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Entry not found in the nwservice hash table!.");
   return NSRM_FAILURE;
}
int32_t nsrm_register_launched_nwservice_object_instance(struct nsrm_nwservice_instance_key *key)
{
   struct nwservice_instance        *nwservice_instance_p = NULL;
   struct nsrm_nschain_object       *nschain_object_scan_node_p = NULL;
   struct nschain_nwservice_object  *nschain_nwservice_object_p = NULL;
   union  nsrm_nwservice_object_notification_data  notification_data = {};
   struct nsrm_notification_app_hook_info *app_entry_p = NULL;

   struct  crm_port* port_info_p = NULL;

   uint32_t  index = 0,magic = 0;
   uchar8_t* hashobj_p = NULL;
   int32_t   ret_value;
   uint64_t  apphookoffset;
   uint8_t   heap_b,status_b = FALSE ;
   uint64_t  tenant_handle,nschain_object_handle, nwservice_object_handle;
   uint64_t  vm_handle ,nwservice_instance_handle,port_saferef;
   uint32_t  hashkey,lstoffset,offset; /* NSM changed */

   if(key->nwservice_instance_name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Service instance name is null");
      return NSRM_ERROR_NWSERVICE_INSTANCE_NAME_NULL;
   }

   if(key->tenant_name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"tenant name is null");
      return NSRM_FAILURE;
   }

   if(key->nschain_object_name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"nschain object name is null");
      return NSRM_FAILURE;
   }

   if(key->nwservice_object_name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"nwservice object name is null");
      return NSRM_FAILURE;
   }
    if(key->vm_name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"vm name is null");
      return NSRM_FAILURE;
   }

   ret_value = crm_get_tenant_handle(key->tenant_name_p,&tenant_handle);
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Tenant doesn't exist ");
      return NSRM_FAILURE;
   }

   CNTLR_RCU_READ_LOCK_TAKE();
   ret_value = crm_get_vm_handle(key->vm_name_p,&vm_handle);
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"vm doesnt exist");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   }
   ret_value = nsrm_get_nschain_object_handle(key->nschain_object_name_p,&nschain_object_handle);
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"nschain doesnt exist");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
   }

   ret_value = nsrm_get_nschain_byhandle(nschain_object_handle,&nschain_object_scan_node_p);
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"nschain doesnt exist");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   }

   ret_value = strcmp(nschain_object_scan_node_p->tenant_name_p,key->tenant_name_p);
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"The nschain is not attached to this tenant");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   }
   lstoffset = NSRM_NSCHAIN_NWSERVICE_OBJECT_TBL_OFFSET;
   hashkey = 0;
   MCHASH_TABLE_SCAN((nschain_object_scan_node_p->nwservice_objects_list) , hashkey)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"hashkey:%d",hashkey);
      MCHASH_BUCKET_SCAN(nschain_object_scan_node_p->nwservice_objects_list, hashkey,
                         nschain_nwservice_object_p,
                         struct nschain_nwservice_object*,lstoffset)
      {
         ret_value = nsrm_get_nwservice_object_handle(key->nwservice_object_name_p,&nwservice_object_handle);
         if(ret_value != NSRM_SUCCESS)
         {
            OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"nwservice doesnt exist");
            return NSRM_ERROR_NWSERVICE_OBJECT_NAME_INVALID;
         }
         if((strcmp(key->nwservice_object_name_p,nschain_nwservice_object_p->name_p)) == 0)
         {
            OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"nwservice found");
            break;
         }
      }
     break;
   }

   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"nwservice_instance_count  :%d",nschain_object_scan_node_p->nwservice_instance_count);
   if(nschain_nwservice_object_p->nwservice_instance_list == NULL)
   {
      nschain_nwservice_object_p->nwservice_instance_list = (struct mchash_table*)calloc(1,(NSRM_MAX_NWSERVICE_INSTANCE_ENTRIES)*sizeof(struct nwservice_instance));
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"creating table");

      ret_value = mchash_table_create(1 ,
                                      NSRM_MAX_NWSERVICE_INSTANCE_ENTRIES,
                                      nsrm_free_nwservice_instance_entry_rcu,
                                      &(nschain_nwservice_object_p->nwservice_instance_list));
      if(ret_value != NSRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"table not created");
         CNTLR_RCU_READ_LOCK_RELEASE();
         return NSRM_FAILURE;
       }
   }
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG," after instance check");
   offset = NSRM_NWSERVICE_INSTANCE_TBL_OFFSET;
   hashkey = 0;
   MCHASH_TABLE_SCAN(nschain_nwservice_object_p->nwservice_instance_list,hashkey)
    {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"inside table scan instance list");
     MCHASH_BUCKET_SCAN(nschain_nwservice_object_p->nwservice_instance_list,
                        hashkey,
                        nwservice_instance_p,
                        struct nwservice_instance*, offset)
     
     {
         if(strcmp(nwservice_instance_p->name_p,key->nwservice_instance_name_p) == 0)
         {
             OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"This network service is already instantiated");
             CNTLR_RCU_READ_LOCK_RELEASE();
             return NSRM_FAILURE;
         }
    }
  }
   #if 1 
   ret_value  =  rrm_get_vm_logicalswitch_side_ports(NULL,vm_handle,
                                                     NULL,&port_saferef);
   if(ret_value != NSRM_SUCCESS)
   {
     OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR,"failed to get port saferef");
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_FAILURE;
   }

   ret_value = crm_get_port_byhandle(port_saferef,&port_info_p);
   if(ret_value != NSRM_SUCCESS)
   {
     OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR,"failed to get port info");
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_FAILURE;
   }
  
   if(port_info_p->port_status != SYSTEM_PORT_READY)
   {
     OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR,"Port status is PORT_STATUS_NOT_READY return");
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_FAILURE;
   }
   #endif
    OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"before mempool");
    ret_value = mempool_get_mem_block(nsrm_nwservice_instance_mempool_g,
                                      (uchar8_t **)&nwservice_instance_p,&heap_b);
    if(ret_value != MEMPOOL_SUCCESS)
    {
       OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to get mempool");
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_FAILURE;
    }
    OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"after mempool");

    nwservice_instance_p->name_p                  = (char *)calloc(1,(strlen(key->nwservice_instance_name_p)+1));
    nwservice_instance_p->tenant_name_p           = (char *)calloc(1,(strlen(key->tenant_name_p)+1));
    nwservice_instance_p->nschain_object_name_p   = (char *)calloc(1,(strlen(key->nschain_object_name_p)+1));
    nwservice_instance_p->nwservice_object_name_p = (char *)calloc(1,(strlen(key->nwservice_object_name_p)+1));
    nwservice_instance_p->vm_name_p               = (char *)calloc(1,(strlen(key->vm_name_p)+1));

    strcpy(nwservice_instance_p->name_p,key->nwservice_instance_name_p);
    strcpy(nwservice_instance_p->vm_name_p,key->vm_name_p);
    strcpy(nwservice_instance_p->tenant_name_p,key->tenant_name_p);
    strcpy(nwservice_instance_p->nschain_object_name_p,key->nschain_object_name_p);
    strcpy(nwservice_instance_p->nwservice_object_name_p,key->nwservice_object_name_p);
    nwservice_instance_p->vlan_id_in  = key->vlan_id_in;
    nwservice_instance_p->vlan_id_out = key->vlan_id_out;
    nwservice_instance_p->vm_saferef  = vm_handle;
   
   strcpy(nwservice_instance_p->vm_ns_port_name_p,port_info_p->port_name);
   strcpy(nwservice_instance_p->switch_name,port_info_p->switch_name);
   

   nwservice_instance_p->vm_ns_port_saferef = port_saferef;
   nwservice_instance_p->port_id            = port_info_p->port_id;

   /* Line Added to support addition of flows at once NSMSRINI */
   nwservice_instance_p->system_br_saferef  = port_info_p->system_br_saferef;

   nwservice_instance_p->heap_b = heap_b;
   hashobj_p = (uchar8_t *)nwservice_instance_p + NSRM_NWSERVICE_INSTANCE_TBL_OFFSET;

   hashkey = 0;
   
   MCHASH_APPEND_NODE(nschain_nwservice_object_p->nwservice_instance_list,
                      hashkey, nwservice_instance_p,index, magic, hashobj_p, status_b);
  
   if(FALSE == status_b)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR," Failed to append nwservice instance node to Hash table");
      if(mempool_release_mem_block(nsrm_nwservice_instance_mempool_g,
                                  (uchar8_t*)&nwservice_instance_p,
                                  nwservice_instance_p->heap_b) != MEMPOOL_SUCCESS)
        OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to release  allocated memory block.");
        CNTLR_RCU_READ_LOCK_RELEASE();
        return NSRM_NWSERVICE_INSTANCE_LAUNCH_FAIL;
   }

    nwservice_instance_handle = magic;
    nwservice_instance_handle = (((nwservice_instance_handle)<<32) | index);
    nwservice_instance_p->magic = magic;
    nwservice_instance_p->index = index;
    nwservice_instance_p->srf_nwservice_instance = nwservice_instance_handle;
    /*Incrementing the nwservice_instance_count*/
    nschain_nwservice_object_p->nwservice_instance_count++;

    apphookoffset = NSRM_NOTIFICATION_APP_HOOK_OFFSET;
    notification_data.launch.nwservice_object_name_p = (char*)calloc(1,(strlen(key->nwservice_object_name_p)+1));
    notification_data.launch.nschain_object_name_p   = (char*)calloc(1,(strlen(key->nschain_object_name_p)+1));
    notification_data.launch.vm_name_p               = (char*)calloc(1,(strlen(key->vm_name_p)+1));
    strcpy(notification_data.launch.nwservice_object_name_p ,key->nwservice_object_name_p);
    strcpy(notification_data.launch.nschain_object_name_p ,key->nschain_object_name_p);
    notification_data.launch.nwservice_object_handle =  nschain_nwservice_object_p->nwservice_object_saferef;
    notification_data.launch.nschain_object_handle   =  nschain_object_scan_node_p->nschain_object_handle;
    notification_data.launch.vm_handle               =  vm_handle;
    notification_data.launch.port_handle             =  nwservice_instance_p->vm_ns_port_saferef;
    notification_data.launch.vlan_id_in              =  key->vlan_id_in;
    notification_data.launch.vlan_id_out             =  key->vlan_id_out;
    OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG," name: %s",notification_data.launch.nwservice_object_name_p);
    OF_LIST_SCAN(nsrm_nwservice_object_notifications[NSRM_NWSERVICE_OBJECT_LAUNCHED],
                  app_entry_p,
                  struct nsrm_notification_app_hook_info *,
                  apphookoffset)
    {
        ((nsrm_nwservice_object_notifier)(app_entry_p->call_back))(NSRM_NWSERVICE_OBJECT_LAUNCHED,
                                                            nwservice_instance_p->srf_nwservice_instance,
                                                            notification_data,
                                                            app_entry_p->cbk_arg1,
                                                            app_entry_p->cbk_arg2
                                                                   );
    }


    OF_LIST_SCAN(nsrm_nwservice_object_notifications[NSRM_NWSERVICE_OBJECT_ALL] ,
                  app_entry_p,
                  struct nsrm_notification_app_hook_info *,
                  apphookoffset)
    {
        ((nsrm_nwservice_object_notifier)(app_entry_p->call_back))(NSRM_NWSERVICE_OBJECT_LAUNCHED,
                                                            nwservice_instance_p->srf_nwservice_instance,
                                                            notification_data,
                                                            app_entry_p->cbk_arg1,
                                                            app_entry_p->cbk_arg2
                                                                   );
    }

    OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"nwservice instantiated successfully");
    CNTLR_RCU_READ_LOCK_RELEASE();
    return NSRM_SUCCESS;
}

int32_t nsrm_register_delaunch_nwswervice_object_instance(struct nsrm_nwservice_instance_key *key)
{
   struct nwservice_instance        *nwservice_instance_p = NULL;
   struct nsrm_nschain_object       *nschain_object_scan_node_p = NULL;
   struct nschain_nwservice_object  *nschain_nwservice_object_p = NULL;
   union  nsrm_nwservice_object_notification_data  notification_data = {};
   struct nsrm_notification_app_hook_info *app_entry_p = NULL;

   int32_t   ret_value;
   uint64_t  lstoffset,offset,apphookoffset;
   uint8_t   status_b = FALSE ;
   uint64_t  hashkey,tenant_handle,nschain_object_handle, nwservice_object_handle;

   if(key->nwservice_instance_name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Service instance name is null");
      return NSRM_ERROR_NWSERVICE_INSTANCE_NAME_NULL;
   }

   if(key->tenant_name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"tenant name is null");
      return NSRM_FAILURE;
   }

   if(key->nschain_object_name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"nschain object name is null");
      return NSRM_FAILURE;
   }
 
   if(key->nwservice_object_name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"nwservice object name is null");
      return NSRM_FAILURE;
   }
  
   CNTLR_RCU_READ_LOCK_TAKE(); 
   ret_value = crm_get_tenant_handle(key->tenant_name_p,&tenant_handle);
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Tenant doesn't exist ");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   }
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"nschain name : %s",key->nschain_object_name_p); 
   ret_value = nsrm_get_nschain_object_handle(key->nschain_object_name_p,&nschain_object_handle);
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"nschain doesnt exist");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
   }
  
   ret_value = nsrm_get_nschain_byhandle(nschain_object_handle,&nschain_object_scan_node_p);
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"nschain doesnt exist");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   }
   
   if(nschain_object_scan_node_p->nwservice_objects_list == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"nwservice object list in this nschain is NULL");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   }

   ret_value = strcmp(nschain_object_scan_node_p->tenant_name_p,key->tenant_name_p);
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"The nschain is not attached to this tenant");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   }

   lstoffset = NSRM_NSCHAIN_NWSERVICE_OBJECT_TBL_OFFSET;
   MCHASH_TABLE_SCAN(nschain_object_scan_node_p->nwservice_objects_list , hashkey)
   {
      MCHASH_BUCKET_SCAN(nschain_object_scan_node_p->nwservice_objects_list, hashkey, 
                         nschain_nwservice_object_p,
                         struct nschain_nwservice_object*,lstoffset)
      {
         ret_value = nsrm_get_nwservice_object_handle(key->nwservice_object_name_p,&nwservice_object_handle);
         if(ret_value != NSRM_SUCCESS)
         {
            OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"nwservice doesnt exist");
            CNTLR_RCU_READ_LOCK_RELEASE();
            return NSRM_ERROR_NWSERVICE_OBJECT_NAME_INVALID;
         } 
         if((strcmp(key->nwservice_object_name_p,nschain_nwservice_object_p->name_p)) == 0)
            break;
      }
     break;
   }
   if(nschain_nwservice_object_p->nwservice_instance_count == 0)
   {
       OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"The nschain doesnt have any instantiated nwservices");
       return NSRM_FAILURE;
   }

   if(nschain_nwservice_object_p->nwservice_instance_list == NULL)
   {
       OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"No nwservice objet is instantiated");
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_FAILURE;
   }
  
   hashkey = 0;
   offset = NSRM_NWSERVICE_INSTANCE_TBL_OFFSET;
   MCHASH_TABLE_SCAN(nschain_nwservice_object_p->nwservice_instance_list,hashkey)
   {
   MCHASH_BUCKET_SCAN(nschain_nwservice_object_p->nwservice_instance_list,hashkey,nwservice_instance_p,struct nwservice_instance *,offset)
   {      
      if((strcmp(nwservice_instance_p->name_p,key->nwservice_instance_name_p)) == 0) 
       {
           OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"index : %d, magic : %d",nwservice_instance_p->index,nwservice_instance_p->magic);
           OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"The intended nwservice instance is found");
           break;
       }
    }
   break;
  }
 
    OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"node found is : %s",nwservice_instance_p->name_p); 
    apphookoffset = NSRM_NOTIFICATION_APP_HOOK_OFFSET;
    notification_data.launch.nwservice_object_name_p = (char*)calloc(1,(strlen(key->nwservice_object_name_p)+1));
    notification_data.launch.nschain_object_name_p = (char*)calloc(1,(strlen(key->nschain_object_name_p)+1));
    notification_data.launch.vm_name_p = (char*)calloc(1,(strlen(nwservice_instance_p->vm_name_p)+1));
    strcpy(notification_data.launch.nwservice_object_name_p ,key->nwservice_object_name_p);
    strcpy(notification_data.launch.nschain_object_name_p ,key->nschain_object_name_p);
    notification_data.launch.nwservice_object_handle = nschain_nwservice_object_p->nwservice_object_saferef;
    notification_data.launch.nschain_object_handle =  nschain_object_scan_node_p->nschain_object_handle;
    notification_data.launch.port_handle           =  nwservice_instance_p->vm_ns_port_saferef;
    notification_data.launch.vm_handle             =  nwservice_instance_p->vm_saferef;
    notification_data.launch.vlan_id_in            =  nwservice_instance_p->vlan_id_in;
    notification_data.launch.vlan_id_out           =  nwservice_instance_p->vlan_id_out;

    OF_LIST_SCAN(nsrm_nwservice_object_notifications[NSRM_NWSERVICE_OBJECT_DE_LAUNCHED], 
                  app_entry_p,
                  struct nsrm_notification_app_hook_info *,
                  apphookoffset)
    {
        ((nsrm_nwservice_object_notifier)(app_entry_p->call_back))(NSRM_NWSERVICE_OBJECT_DE_LAUNCHED,
                                                            nwservice_instance_p->srf_nwservice_instance,
                                                            notification_data,
                                                            app_entry_p->cbk_arg1,
                                                            app_entry_p->cbk_arg2
                                                                   );
    }
   
    OF_LIST_SCAN(nsrm_nwservice_object_notifications[NSRM_NWSERVICE_OBJECT_ALL] ,
                  app_entry_p,
                  struct nsrm_notification_app_hook_info *,
                  apphookoffset)
    {
        ((nsrm_nwservice_object_notifier)(app_entry_p->call_back))(NSRM_NWSERVICE_OBJECT_DE_LAUNCHED,
                                                            nwservice_instance_p->srf_nwservice_instance,
                                                            notification_data,
                                                            app_entry_p->cbk_arg1,
                                                            app_entry_p->cbk_arg2
                                                                   );
    }


   offset = NSRM_NWSERVICE_INSTANCE_TBL_OFFSET; 
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"index : %d , magic : %d",nschain_nwservice_object_p->index,nschain_nwservice_object_p->magic);
   MCHASH_DELETE_NODE_BY_REF((nschain_nwservice_object_p->nwservice_instance_list),nwservice_instance_p->index,nwservice_instance_p->magic,
                             struct nwservice_instance*,offset, status_b);
   
   CNTLR_RCU_READ_LOCK_RELEASE();
   if(status_b == TRUE)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "nwservice instance delaunched successfully");
      nschain_nwservice_object_p->nwservice_instance_count--;
      return NSRM_SUCCESS;
   }
   else
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "failed to delaunch nwservice instance");
      return NSRM_NWSERVICE_INSTANCE_DELAUNCH_FAIL;
   }
}

int32_t nsrm_get_first_nwservice_instance(
                         struct   nsrm_nwservice_attach_key    *nschain_nwservice_object_p,
        		 int32_t  number_of_nwservice_instances_requested,
         		 int32_t* number_of_nwservice_instances_returned_p,
        		 struct   nsrm_nwservice_instance_record* recs_p) 
{
   struct     nwservice_instance         *nwservice_instance_node_scan_p = NULL;
   struct     nschain_nwservice_object   *nschain_nwservice_attach_p = NULL;
   uint64_t   hashkey = 0;
   uint64_t   lstoffset , chain_handle, nwservice_handle;
   uint32_t   nwservice_instances_returned = 0;
   uint32_t   nwservice_instances_requested = number_of_nwservice_instances_requested;
   int32_t    status = NSRM_FAILURE ,return_value;
    


   if (nschain_nwservice_object_p->nschain_object_name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "chain name is null");
      return NSRM_FAILURE;
   }

   CNTLR_RCU_READ_LOCK_TAKE();
   
   return_value = nsrm_get_nschain_object_handle(nschain_nwservice_object_p->nschain_object_name_p , &chain_handle);
   if(return_value != NSRM_SUCCESS)
   {
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_FAILURE;
   }

   return_value = nsrm_get_attached_nwservice_handle(chain_handle,nschain_nwservice_object_p->name_p,&nwservice_handle);
   if(return_value != NSRM_SUCCESS)
   {
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_FAILURE;
   }

   return_value = nsrm_get_attached_nwservice_byhandle(nwservice_handle,chain_handle,&nschain_nwservice_attach_p);
   if(return_value != NSRM_SUCCESS)
   {
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_FAILURE;
   }

   hashkey = 0;
   
   if(nschain_nwservice_attach_p->nwservice_instance_count == 0)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "No instances exist");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   }

   if(nschain_nwservice_attach_p->nwservice_instance_list == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "No instances exist");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   }

   lstoffset = NSRM_NWSERVICE_INSTANCE_TBL_OFFSET;
   MCHASH_TABLE_SCAN(nschain_nwservice_attach_p->nwservice_instance_list , hashkey)
   {
      MCHASH_BUCKET_SCAN(nschain_nwservice_attach_p->nwservice_instance_list, hashkey, 
                         nwservice_instance_node_scan_p,
                         struct nwservice_instance*,lstoffset)
      {
         strcpy(recs_p[nwservice_instances_returned].key.nwservice_instance_name_p , nwservice_instance_node_scan_p->name_p);
         strcpy(recs_p[nwservice_instances_returned].key.tenant_name_p , nwservice_instance_node_scan_p->tenant_name_p);
         strcpy(recs_p[nwservice_instances_returned].key.nschain_object_name_p , nwservice_instance_node_scan_p->nschain_object_name_p);
         strcpy(recs_p[nwservice_instances_returned].key.nwservice_object_name_p , nwservice_instance_node_scan_p->nwservice_object_name_p);
         strcpy(recs_p[nwservice_instances_returned].key.vm_name_p , nwservice_instance_node_scan_p->vm_name_p);
         //strcpy(recs_p[nwservice_instances_returned].key.switch_name , nwservice_instance_node_scan_p->switch_name);
         recs_p[nwservice_instances_returned].key.vlan_id_in  = nwservice_instance_node_scan_p->vlan_id_in;
         recs_p[nwservice_instances_returned].key.vlan_id_out = nwservice_instance_node_scan_p->vlan_id_out;
         status = NSRM_SUCCESS;
         number_of_nwservice_instances_requested--;
         nwservice_instances_returned++;
         if(number_of_nwservice_instances_requested)
           continue;
         else
           break;
      }
      if(number_of_nwservice_instances_requested)
        continue;
      else
        break;
   }
   if(nwservice_instances_requested != nwservice_instances_returned)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Invalid number of objects returned / requested ");
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_FAILURE;
   }
   if(status == NSRM_SUCCESS)
   {
      *number_of_nwservice_instances_returned_p = nwservice_instances_returned;
       CNTLR_RCU_READ_LOCK_RELEASE();
       return status;
   }
   CNTLR_RCU_READ_LOCK_RELEASE();
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "No More Entries in the nwservice hash table!.");
   return NSRM_ERROR_NO_MORE_ENTRIES;
}
 

int32_t nsrm_get_next_nwservice_instance(
                         struct   nsrm_nwservice_attach_key    *nschain_nwservice_object_p,
                         struct   nsrm_nwservice_instance_key   *relative_record_key_p,
        		 int32_t  number_of_nwservice_instances_requested,
         		 int32_t* number_of_nwservice_instances_returned_p,
        		 struct   nsrm_nwservice_instance_record* recs_p) 
{
   struct     nwservice_instance         *nwservice_instance_node_scan_p = NULL;
   struct     nschain_nwservice_object   *nschain_nwservice_attach_p = NULL;
   uint64_t   hashkey = 0 ;
   uint64_t   lstoffset ,chain_handle, nwservice_handle;
   uint32_t   nwservice_instances_returned = 0;
   uint32_t   nwservice_instances_requested = number_of_nwservice_instances_requested;
   int32_t    status = NSRM_FAILURE , return_value;
   uint8_t    node_found_b = FALSE;
    
   return_value = nsrm_get_nschain_object_handle(nschain_nwservice_object_p->nschain_object_name_p , &chain_handle);
   if(return_value != NSRM_SUCCESS)
   {
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_FAILURE;
   }

   return_value = nsrm_get_attached_nwservice_handle(chain_handle,nschain_nwservice_object_p->name_p,&nwservice_handle);
   if(return_value != NSRM_SUCCESS)
   {
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_FAILURE;
   }

   return_value = nsrm_get_attached_nwservice_byhandle(nwservice_handle,chain_handle,&nschain_nwservice_attach_p);
   if(return_value != NSRM_SUCCESS)
   {
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_FAILURE;
   }

   if(nschain_nwservice_attach_p->nwservice_instance_count == 0)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "No instances exist");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   }

   if(nschain_nwservice_attach_p->nwservice_instance_list == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "No instances exist");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   }
 
   hashkey = 0;

   lstoffset = NSRM_NWSERVICE_INSTANCE_TBL_OFFSET;
   MCHASH_TABLE_SCAN(nschain_nwservice_attach_p->nwservice_instance_list , hashkey)
   {
      MCHASH_BUCKET_SCAN(nschain_nwservice_attach_p->nwservice_instance_list, hashkey, 
                         nwservice_instance_node_scan_p,
                         struct nwservice_instance*,lstoffset)
      {
         if(node_found_b == FALSE)
         { 
           if(strcmp(relative_record_key_p->nwservice_instance_name_p , nwservice_instance_node_scan_p->name_p) != 0)
           {
              continue;
           }
           else
           {
              node_found_b = TRUE;
              continue;
           }
        }
        if(node_found_b != TRUE)
          return NSRM_FAILURE;
        strcpy(recs_p[nwservice_instances_returned].key.nwservice_instance_name_p , nwservice_instance_node_scan_p->name_p);
        strcpy(recs_p[nwservice_instances_returned].key.tenant_name_p , nwservice_instance_node_scan_p->tenant_name_p);
        strcpy(recs_p[nwservice_instances_returned].key.nschain_object_name_p , nwservice_instance_node_scan_p->nschain_object_name_p);
        strcpy(recs_p[nwservice_instances_returned].key.nwservice_object_name_p , nwservice_instance_node_scan_p->nwservice_object_name_p);
        strcpy(recs_p[nwservice_instances_returned].key.vm_name_p , nwservice_instance_node_scan_p->vm_name_p);
        //strcpy(recs_p[nwservice_instances_returned].key.switch_name , nwservice_instance_node_scan_p->switch_name);
        recs_p[nwservice_instances_returned].key.vlan_id_in  = nwservice_instance_node_scan_p->vlan_id_in;
        recs_p[nwservice_instances_returned].key.vlan_id_out = nwservice_instance_node_scan_p->vlan_id_out;
        status = NSRM_SUCCESS;
        number_of_nwservice_instances_requested--;
        nwservice_instances_returned++;
        if(number_of_nwservice_instances_requested)
          continue;
        else
          break;
      }
      if(number_of_nwservice_instances_requested)
        continue;
      else
        break;
   }
   if(nwservice_instances_requested != nwservice_instances_returned)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Invalid number of objects returned / requested ");
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_FAILURE;
   }
   if(status == NSRM_SUCCESS)
   {
      *number_of_nwservice_instances_returned_p = nwservice_instances_returned;
       CNTLR_RCU_READ_LOCK_RELEASE();
       return status;
   }
   CNTLR_RCU_READ_LOCK_RELEASE();
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "No More Entries in the nwservice hash table!.");
   return NSRM_ERROR_NO_MORE_ENTRIES;
}
 

int32_t nsrm_get_exact_nwservice_instance(
        struct   nsrm_nwservice_attach_key         *nschain_nwservice_object_p,
        struct   nsrm_nwservice_instance_key*      key_info_p,
        struct   nsrm_nwservice_instance_record*   rec_p)

{
   struct     nwservice_instance         *nwservice_instance_node_scan_p = NULL;
   struct     nschain_nwservice_object   *nschain_nwservice_attach_p = NULL;
   uint64_t   hashkey = 0;
   uint64_t   lstoffset,chain_handle,nwservice_handle;
   int32_t    ret_value,nwservice_instances_returned = 0 ;
   uint8_t    node_found_b = FALSE;
    
   ret_value = nsrm_get_nschain_object_handle(nschain_nwservice_object_p->nschain_object_name_p , &chain_handle);
   if(ret_value != NSRM_SUCCESS)
   {
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_FAILURE;
   }

   ret_value = nsrm_get_attached_nwservice_handle(chain_handle,nschain_nwservice_object_p->name_p,&nwservice_handle);
   if(ret_value != NSRM_SUCCESS)
   {
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_FAILURE;
   }

   ret_value = nsrm_get_attached_nwservice_byhandle(nwservice_handle,chain_handle,&nschain_nwservice_attach_p);
   if(ret_value != NSRM_SUCCESS)
   {
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_FAILURE;
   }

   if(nschain_nwservice_attach_p->nwservice_instance_count == 0)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "No instances exist");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   }

   if(nschain_nwservice_attach_p->nwservice_instance_list == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "No instances exist");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   } 

   hashkey = 0;

   lstoffset = NSRM_NWSERVICE_INSTANCE_TBL_OFFSET;
   MCHASH_TABLE_SCAN(nschain_nwservice_attach_p->nwservice_instance_list , hashkey)
   {
      MCHASH_BUCKET_SCAN(nschain_nwservice_attach_p->nwservice_instance_list, hashkey, 
                         nwservice_instance_node_scan_p,
                         struct nwservice_instance*,lstoffset)
      {
         if(node_found_b == FALSE)
         { 
           if(strcmp(key_info_p->nwservice_instance_name_p , nwservice_instance_node_scan_p->name_p) == 0)
           {
              OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"node found");
              node_found_b = TRUE;
              break;
           }
           else
           {
              node_found_b = FALSE;
              continue;
           }
        }
     }
     break; 
   }
        if(node_found_b == FALSE)
        {
           OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"could NOT find exact record");
           CNTLR_RCU_READ_LOCK_RELEASE();
           return NSRM_FAILURE;
        }       
        OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"node found is : %s",nwservice_instance_node_scan_p->name_p); 
        strcpy(rec_p[nwservice_instances_returned].key.nwservice_instance_name_p , nwservice_instance_node_scan_p->name_p);
        strcpy(rec_p[nwservice_instances_returned].key.tenant_name_p , nwservice_instance_node_scan_p->tenant_name_p);
        strcpy(rec_p[nwservice_instances_returned].key.nschain_object_name_p , nwservice_instance_node_scan_p->nschain_object_name_p);
        strcpy(rec_p[nwservice_instances_returned].key.nwservice_object_name_p , nwservice_instance_node_scan_p->nwservice_object_name_p);
        strcpy(rec_p[nwservice_instances_returned].key.vm_name_p , nwservice_instance_node_scan_p->vm_name_p);
        //strcpy(rec_p[nwservice_instances_returned].key.switch_name , nwservice_instance_node_scan_p->switch_name);
        rec_p[nwservice_instances_returned].key.vlan_id_in  = nwservice_instance_node_scan_p->vlan_id_in;
        rec_p[nwservice_instances_returned].key.vlan_id_out = nwservice_instance_node_scan_p->vlan_id_out;
        CNTLR_RCU_READ_LOCK_RELEASE();
        OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"Get exact succesfully");
        return NSRM_SUCCESS;
}
 
   
int32_t nsrm_register_nschain_object_notifications(
                          uint8_t notification_type,
                          nsrm_nschain_object_notifier  nschain_notifier,
                          void* callback_arg1,
                          void* callback_arg2)    
{
   struct  nsrm_notification_app_hook_info *app_entry_p=NULL;
   uint8_t heap_b;
   int32_t retval = NSRM_SUCCESS;
   uchar8_t lstoffset;
   lstoffset = NSRM_NOTIFICATION_APP_HOOK_OFFSET;
  
   if((notification_type < NSRM_NSCHAIN_OBJECT_FIRST_NOTIFICATION_TYPE) ||
     (notification_type > NSRM_NSCHAIN_OBJECT_LAST_NOTIFICATION_TYPE))
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Invalid Notification type:%d", notification_type);
      return NSRM_FAILURE;
   }
   if(nschain_notifier == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "callback function is null");
      return NSRM_FAILURE;
   }

   retval = mempool_get_mem_block(nsrm_notifications_mempool_g, (uchar8_t**)&app_entry_p, &heap_b);
   if(retval != MEMPOOL_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to get mempool");
      return NSRM_FAILURE;
   }

   app_entry_p->call_back = (void*)nschain_notifier;
   app_entry_p->cbk_arg1  = callback_arg1;
   app_entry_p->cbk_arg2  = callback_arg2;
   app_entry_p->heap_b    = heap_b;
  
   /* Add Application to the list of nschain object notifications */
   OF_APPEND_NODE_TO_LIST(nsrm_nschain_object_notifications[notification_type],app_entry_p, lstoffset); 
   if(retval != NSRM_SUCCESS)
     mempool_release_mem_block(nsrm_notifications_mempool_g,(uchar8_t*)app_entry_p, app_entry_p->heap_b);

   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "callback function reg with nschain object success");
   return retval;
}

/* This API gets attached nwservice in a chain by its handle.*/

int32_t nsrm_get_attached_nwservice_handle(uint64_t nschain_handle,
                                             char* nschain_nwservice_name,
                                             uint64_t *nschain_nwservice_handle)
{ 
  /* Caller shall take RCU locks. */
  int32_t ret_val,offset,hashkey;
  struct nsrm_nschain_object *nschain_object_p = NULL;
  struct nschain_nwservice_object* nschain_nwservice_object_p = NULL;
  ret_val = nsrm_get_nschain_byhandle(nschain_handle,&nschain_object_p);
  if(ret_val != NSRM_SUCCESS)
  {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "error ");
     return NSRM_FAILURE;
  }
  offset = NSRM_NSCHAIN_NWSERVICE_OBJECT_TBL_OFFSET;
  hashkey = 0;
  MCHASH_BUCKET_SCAN(nschain_object_p->nwservice_objects_list, hashkey, nschain_nwservice_object_p,struct nschain_nwservice_object*,offset)
  {
    if(strcmp(nschain_nwservice_object_p->name_p, nschain_nwservice_name))
    {
      continue;
    }
    OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "name found is : %s",nschain_nwservice_object_p->name_p);
    *nschain_nwservice_handle = nschain_nwservice_object_p->magic;
    *nschain_nwservice_handle = (((*nschain_nwservice_handle)<<32) | (nschain_nwservice_object_p->index));
    return NSRM_SUCCESS;
  }
  return NSRM_FAILURE;
}

int32_t nsrm_get_attached_nwservice_byhandle(uint64_t nwservice_handle,
                                             uint64_t nschain_handle,
                                             struct nschain_nwservice_object **nschain_nwservice_object_p_p)
{
   /* Caller shall take RCU locks. */
  uint32_t index,magic;
  uint8_t status_b;
  int32_t ret_value;
  struct nsrm_nschain_object* nschain_object_p = NULL;
  struct nsrm_nwservice_object *nwservice_object_p;

   ret_value = nsrm_get_nschain_byhandle(nschain_handle , &nschain_object_p);
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nschain object name is null");
      return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
   }
   
   ret_value = nsrm_get_nwservice_byhandle(nwservice_handle , &nwservice_object_p);
    if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nwservice object name is invalid");
      return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
   }
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nwservice name : %s",nwservice_object_p->name_p);
   
   magic = (uint32_t)(nwservice_handle >>32);
   index = (uint32_t)(nwservice_handle);
   MCHASH_ACCESS_NODE(nschain_object_p->nwservice_objects_list,index,magic,*nschain_nwservice_object_p_p,status_b);
   if(TRUE == status_b)
     return NSRM_SUCCESS;
   return NSRM_FAILURE;
}

/* This API gets nwservice instance of the attached nwservice object by its handle.*/

int32_t nsrm_get_nwservice_instance_byhandle(uint64_t nschain_handle,
                                             uint64_t nschain_nwservice_object_handle,
                                             uint64_t nwservice_instance_handle,
                                             struct   nwservice_instance **nwservice_instance_p_p)
{
   /* Caller shall take RCU locks. */
  uint32_t index,magic;
  uint8_t status_b;
  int32_t ret_value;
  struct  nschain_nwservice_object *nschain_nwservice_object_p = NULL;
   
   ret_value = nsrm_get_attached_nwservice_byhandle(nschain_nwservice_object_handle ,nschain_handle, &nschain_nwservice_object_p);
   if(ret_value != NSRM_SUCCESS)
   {  
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nwservice object name is null");
      return NSRM_ERROR_NWSERVICE_OBJECT_NAME_INVALID;
   }
   magic = (uint32_t)(nwservice_instance_handle >>32);
   index = (uint32_t)nwservice_instance_handle;

  MCHASH_ACCESS_NODE(nschain_nwservice_object_p->nwservice_instance_list,index,magic,*nwservice_instance_p_p,status_b);
   if(TRUE == status_b)
     return NSRM_SUCCESS;
 return NSRM_FAILURE;
}
   
int32_t nsrm_add_attribute_to_nschain_object(
          char*     nschain_object_name_p,
          struct    nsrm_attribute_name_value_pair *attribute_info_p)
{
  int32_t  ret_val;
  struct   nsrm_nschain_object                         *nschain_object_entry_p;
  union    nsrm_nschain_object_notification_data        notification_data={};
  struct   nsrm_notification_app_hook_info               *app_entry_p;
  int32_t  apphookoffset;
  uint64_t nschain_object_handle;
  apphookoffset = NSRM_NOTIFICATION_APP_HOOK_OFFSET;

  CNTLR_RCU_READ_LOCK_TAKE();
  ret_val = nsrm_get_nschain_object_handle(nschain_object_name_p,&nschain_object_handle);
  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return ret_val;
  }
  ret_val = nsrm_get_nschain_byhandle(nschain_object_handle,&nschain_object_entry_p);
  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return ret_val;
  }

  ret_val = nsrm_add_attribute(&(nschain_object_entry_p->attributes),attribute_info_p);
  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return ret_val;
  }
  OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "added attr");
  nschain_object_entry_p->number_of_attributes++;

  nsrm_create_nschain_object_view_entry(nschain_object_handle, attribute_info_p->attribute_name_p, attribute_info_p->attribute_value_p);

  notification_data.attribute.nschain_object_name_p = (char *)calloc(1,strlen(nschain_object_entry_p->name_p)+1);
  notification_data.attribute.nschain_attribute_name_p = (char *)calloc(1,strlen(attribute_info_p->attribute_name_p)+1);
  notification_data.attribute.nschain_attribute_value_p = (char *)calloc(1,strlen(attribute_info_p->attribute_value_p)+1);
  strcpy(notification_data.attribute.nschain_object_name_p, nschain_object_entry_p->name_p);
  strcpy(notification_data.attribute.nschain_attribute_name_p,attribute_info_p->attribute_name_p);
  strcpy(notification_data.attribute.nschain_attribute_value_p,attribute_info_p->attribute_value_p);
  notification_data.attribute.nschain_object_handle = nschain_object_handle;

  OF_LIST_SCAN(nsrm_nschain_object_notifications[NSRM_NSCHAIN_OBJECT_ATTRIBUTE_ADDED],
               app_entry_p,
               struct nsrm_notification_app_hook_info *,
               apphookoffset)
  {
     ((nsrm_nschain_object_notifier)(app_entry_p->call_back))(NSRM_NSCHAIN_OBJECT_ATTRIBUTE_ADDED,
                                                                nschain_object_handle,
                                                                notification_data,
                                                                app_entry_p->cbk_arg1,
                                                                 app_entry_p->cbk_arg2
                                                                   );
  }

   OF_LIST_SCAN(nsrm_nschain_object_notifications[NSRM_NSCHAIN_OBJECT_ALL] ,
                app_entry_p,
                struct nsrm_notification_app_hook_info *,
                apphookoffset)
   {
        ((nsrm_nschain_object_notifier)(app_entry_p->call_back))(NSRM_NSCHAIN_OBJECT_ATTRIBUTE_ADDED,
                                                                   nschain_object_handle,
                                                                   notification_data,
                                                                   app_entry_p->cbk_arg1,
                                                                   app_entry_p->cbk_arg2
                                                                   );
   }

  OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "success");
  CNTLR_RCU_READ_LOCK_RELEASE();
  return NSRM_SUCCESS;
}
 


int32_t nsrm_del_attribute_from_nschain_object(
                                                 char* nschain_object_name_p,
                                                 char* attribute_name_p)
{
  int32_t  ret_val;
  struct   nsrm_nschain_object                         *nschain_object_entry_p;
  union    nsrm_nschain_object_notification_data        notification_data={};
  struct   nsrm_notification_app_hook_info               *app_entry_p;
  struct   nsrm_resource_attribute_entry *attribute_entry_p,*prev_attr_entry_p = NULL;
  int32_t  apphookoffset;
  uint64_t nschain_object_handle;
  apphookoffset = NSRM_NOTIFICATION_APP_HOOK_OFFSET;
  uint8_t  attr_node_found = 0;

  CNTLR_RCU_READ_LOCK_TAKE();
  ret_val = nsrm_get_nschain_object_handle(nschain_object_name_p,&nschain_object_handle);
  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return ret_val;
  }
  ret_val = nsrm_get_nschain_byhandle(nschain_object_handle,&nschain_object_entry_p);
  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return ret_val;
  }
  
  OF_LIST_SCAN(nschain_object_entry_p->attributes, attribute_entry_p, struct nsrm_resource_attribute_entry*,apphookoffset)
  {
    if(!strcmp(attribute_name_p, attribute_entry_p->name))
    {
      attr_node_found =1;
      break;
    }
    prev_attr_entry_p = attribute_entry_p;
  }

  if(attr_node_found == 0)
   {
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_FAILURE;
   }
   notification_data.attribute.nschain_object_name_p = (char *)calloc(1,strlen(nschain_object_entry_p->name_p)+1);
  notification_data.attribute.nschain_attribute_name_p = (char *)calloc(1,strlen(attribute_name_p)+1);
  /*
  notification_data.attribute.nschain_attribute_value_p = (char *)calloc(1,strlen(attribute_info_p->attribute_value_p)+1);
  */
  strcpy(notification_data.attribute.nschain_object_name_p, nschain_object_entry_p->name_p);
  strcpy(notification_data.attribute.nschain_attribute_name_p,attribute_name_p);
//  strcpy(notification_data.attribute.nschain_attribute_value_p,attribute_info_p->attribute_value_p);
  notification_data.attribute.nschain_object_handle = nschain_object_handle;

  OF_LIST_SCAN(nsrm_nschain_object_notifications[NSRM_NSCHAIN_OBJECT_ATTRIBUTE_ADDED],
               app_entry_p,
               struct nsrm_notification_app_hook_info *,
               apphookoffset)
  {
     ((nsrm_nschain_object_notifier)(app_entry_p->call_back))(NSRM_NSCHAIN_OBJECT_ATTRIBUTE_ADDED,
                                                                nschain_object_handle,
                                                                notification_data,
                                                                app_entry_p->cbk_arg1,
                                                                 app_entry_p->cbk_arg2
                                                                   );
  }

   OF_LIST_SCAN(nsrm_nschain_object_notifications[NSRM_NSCHAIN_OBJECT_ALL] ,
                app_entry_p,
                struct nsrm_notification_app_hook_info *,
                apphookoffset)
   {
        ((nsrm_nschain_object_notifier)(app_entry_p->call_back))(NSRM_NSCHAIN_OBJECT_ATTRIBUTE_ADDED,
                                                                   nschain_object_handle,
                                                                   notification_data,
                                                                   app_entry_p->cbk_arg1,
                                                                   app_entry_p->cbk_arg2
                                                                   );
   }

   OF_REMOVE_NODE_FROM_LIST(nschain_object_entry_p->attributes,attribute_entry_p,prev_attr_entry_p,apphookoffset);
   nschain_object_entry_p->number_of_attributes--;
   nsrm_remove_nschain_object_view_entry(attribute_entry_p->name, attribute_entry_p->value);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return NSRM_SUCCESS;
}


int32_t nsrm_get_first_nschain_object_attribute(
          char*   nschain_object_name_p,
          struct  nsrm_attribute_name_value_output_info *attribute_output_p) 
{
  struct   nsrm_nschain_object      *nschain_object_entry_p;
  uint64_t nschain_object_handle;
  int32_t ret_val = NSRM_SUCCESS;
  
  CNTLR_RCU_READ_LOCK_TAKE();

  do
  {
    ret_val = nsrm_get_nschain_object_handle(nschain_object_name_p,&nschain_object_handle);
    if(ret_val != NSRM_SUCCESS)
      break;
    
    ret_val = nsrm_get_nschain_byhandle(nschain_object_handle,&nschain_object_entry_p);
    if(ret_val != NSRM_SUCCESS)
      break;
    
   
    ret_val = nsrm_get_first_attribute(&(nschain_object_entry_p->attributes), attribute_output_p);
    if(ret_val != NSRM_SUCCESS)
      break;
  }while(0);

  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Get first attribute failed");
    return ret_val;
  }
  else
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Get first attribute success");
    return ret_val;
  }
} 


int32_t nsrm_get_next_nschain_object_attribute(
          char*   nschain_object_name_p,
          char*   current_attribute_name_p,
          struct  nsrm_attribute_name_value_output_info *attribute_output_p) 
{
  struct   nsrm_nschain_object      *nschain_object_entry_p;
  uint64_t nschain_object_handle;
  int32_t ret_val = NSRM_SUCCESS;
  
  CNTLR_RCU_READ_LOCK_TAKE();

  do
  {
    ret_val = nsrm_get_nschain_object_handle(nschain_object_name_p,&nschain_object_handle);
    if(ret_val != NSRM_SUCCESS)
      break;
    
    ret_val = nsrm_get_nschain_byhandle(nschain_object_handle,&nschain_object_entry_p);
    if(ret_val != NSRM_SUCCESS)
      break;
    
   
    ret_val = nsrm_get_next_attribute(&(nschain_object_entry_p->attributes),current_attribute_name_p,
                                        attribute_output_p);
    if(ret_val != NSRM_SUCCESS)
      break;
    
  }while(0);

  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Get next attribute failed");
    return ret_val;
  }
  else
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Get next attribute success");
    return ret_val;
  }
} 


int32_t nsrm_get_exact_nschain_object_attribute(
          char*  nschain_object_name_p,
          char*  attribute_name_p,
          struct nsrm_attribute_name_value_output_info *attribute_output_p
          )
{
  struct   nsrm_nschain_object      *nschain_object_entry_p;
  uint64_t nschain_object_handle;
  int32_t ret_val = NSRM_SUCCESS;
  
  CNTLR_RCU_READ_LOCK_TAKE();

  do
  {
    ret_val = nsrm_get_nschain_object_handle(nschain_object_name_p,&nschain_object_handle);
    if(ret_val != NSRM_SUCCESS)
      break;
    
    ret_val = nsrm_get_nschain_byhandle(nschain_object_handle,&nschain_object_entry_p);
    if(ret_val != NSRM_SUCCESS)
      break;
   
    ret_val = nsrm_get_exact_attribute(&(nschain_object_entry_p->attributes),attribute_name_p,attribute_output_p);
    if(ret_val != NSRM_SUCCESS)
      break;
  }while(0);

  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Get exact attribute failed");
    return ret_val;
  }
  else
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Get exact attribute success");
    return ret_val;
  }
}
/******************************************************************************************************/
int32_t nsrm_create_nschain_object_view(char *view_name)
{
  int32_t index = 0 , ii;
  uint8_t empty_index_b = 0;

  for(ii = 0;ii<NSRM_MAX_VIEW_DATABASE ; ii++)
  {
     if(!strcmp(nschain_object_db_view[ii].view , view_name))
     {
        OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"View already created");
        return NSRM_FAILURE;
     } 
     else if((empty_index_b == 0) && (!strcmp(nschain_object_db_view[ii].view ,"")))    
     {
        empty_index_b = 1;
        index = ii;
     }
  } 
  strcpy(nschain_object_db_view[index].view,view_name);
  nsrm_view_hash_table_init(&nschain_object_db_view[index].db_viewtbl_p);
  nschain_object_db_view[index].uniqueness_flag = FALSE; 
  nschain_object_db_view[index].valid_b = TRUE;
 
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "View node created succesfully");
  return NSRM_SUCCESS;
}
  

int32_t nsrm_create_nschain_object_view_entry(uint64_t nschain_handle , char* view_name ,char* view_value)
{
  struct   database_view_node *view_node_entry_p;
  int32_t  view_index;
  uint32_t index,magic;
  int32_t  status = NSRM_SUCCESS;
  uint32_t hashkey;
  uint8_t  status_b,view_found_b = 0;

  do
  {
     for(view_index = 0;view_index < NSRM_MAX_VIEW_DATABASE ; view_index++)
     {
       if(!strcmp(nschain_object_db_view[view_index].view ,view_name))
       {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "View entry can be made.");
         view_found_b = 1;
         break;
       }
     }
     if(view_found_b == 0)
     {
       OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"View hash table not initialized");
       status = NSRM_FAILURE;
     }
     
     hashkey = nsrm_get_hashval_byname(view_value,nsrm_no_of_nschain_object_buckets_g);
     CNTLR_RCU_READ_LOCK_TAKE();
     view_node_entry_p = nsrm_alloc_view_entry_and_set_values(nschain_handle, view_name, view_value);
     if (view_node_entry_p == NULL)
     {
       status = NSRM_FAILURE;
       break;
     }
     MCHASH_APPEND_NODE(nschain_object_db_view[view_index].db_viewtbl_p,hashkey,view_node_entry_p,index,magic,view_node_entry_p,status_b);
     if(FALSE == status_b)
     {
       status = NSRM_FAILURE;
       break;
     }
     view_node_entry_p->magic = magic;
     view_node_entry_p->index = index;
     OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"View entry succesfuuly added");
  }while(0);
  
  CNTLR_RCU_READ_LOCK_RELEASE();
  return status;
}

 
int32_t nsrm_remove_nschain_object_view_entry(char* view_name,char* view_value)
{
  struct   database_view_node *view_node_entry_p;
  int32_t  index , offset = 0;
  int32_t  status = NSRM_SUCCESS;
  uint32_t hashkey;
  uint8_t  status_b,view_found_b = 0;
  
  do
  {
     for(index = 0;index<NSRM_MAX_VIEW_DATABASE;index++)
     {
       if(!strcmp(nschain_object_db_view[index].view,view_name))
       {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"View found for this view name");
         view_found_b = 1;
         break;
       }
     }
     if(view_found_b == 0)
     {
       OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"View not initiliazed for the view name.So no entries exist");
       status = NSRM_FAILURE;
     }
     hashkey = nsrm_get_hashval_byname(view_value,nsrm_no_of_nschain_object_buckets_g);
     CNTLR_RCU_READ_LOCK_TAKE(); 
     MCHASH_BUCKET_SCAN(nschain_object_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
     {
      if ((!strcmp(view_node_entry_p->view_value, view_value)) &&
          (!strcmp(view_node_entry_p->node_name, view_name)))
      {
        status_b = FALSE;
        offset = 0;
        MCHASH_DELETE_NODE_BY_REF(nschain_object_db_view[index].db_viewtbl_p,view_node_entry_p->index,view_node_entry_p->magic,struct database_view_node *,offset,status_b);
        if (status_b == FALSE)
        {
          status = NSRM_FAILURE;
          break;
        }
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"View entry deleted successfully");
        status = NSRM_SUCCESS;
        break;
      }
    }
  }while(0);
  
  CNTLR_RCU_READ_LOCK_RELEASE();
  return status;
}
 
int32_t nsrm_remove_nschain_object_view(char *view_name)
{
  int32_t  index , ret_val;
  uint8_t  node_found_b = 0 ;
  uint64_t hashkey ,offset ;
  struct   database_view_node *view_node_entry_p;

  for(index = 0;index<NSRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(nschain_object_db_view[index].view , view_name))
     {
        OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"View found to deleted");
        node_found_b = 1;
        break;
     }
  }
  if(node_found_b == 0)
  {
    OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_ERROR,"Node not found ");
    return NSRM_FAILURE;
  }

  node_found_b = 0;
  offset = 0;
  
  MCHASH_TABLE_SCAN(nschain_object_db_view[index].db_viewtbl_p,hashkey)
  {
    MCHASH_BUCKET_SCAN(nschain_object_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
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
     OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_ERROR,"Remove view entires to remove the entire view");
     return NSRM_FAILURE;
  }
  ret_val = nsrm_view_hash_table_deinit(nschain_object_db_view[index].db_viewtbl_p);
  if(ret_val != NSRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_ERROR,"Hash Table deinit failed");
    return NSRM_FAILURE;
  }

  strcpy(nschain_object_db_view[index].view,"");
  nschain_object_db_view[index].uniqueness_flag = FALSE; 
  nschain_object_db_view[index].valid_b = FALSE;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "View node deleted succesfully");
  return NSRM_SUCCESS;
}


int32_t nsrm_get_first_nschain_object_view_entry(char *view_name , struct database_view_node *vn_view_node)
{
  int32_t  index;
  uint8_t  node_found_b = 0 ;
  int32_t  hashkey ,offset ;
  struct   database_view_node *view_node_entry_p;

  for(index = 0;index<NSRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(nschain_object_db_view[index].view , view_name))
     {
        OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"View found ");
        node_found_b = 1;
        break;
     }
  }
  if(node_found_b == 0)
  {
    OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_ERROR,"Node not found ");
    return NSRM_FAILURE;
  }
  node_found_b = 0; 
  offset = 0;
  CNTLR_RCU_READ_LOCK_TAKE();
  MCHASH_TABLE_SCAN(nschain_object_db_view[index].db_viewtbl_p ,hashkey)
  {
    MCHASH_BUCKET_SCAN(nschain_object_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
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
     OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Node entry NOT found");
     return NSRM_FAILURE;
  }

  CNTLR_RCU_READ_LOCK_RELEASE();
  OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Node entry found");
  return NSRM_SUCCESS;
}
   
int32_t nsrm_get_next_nschain_object_view_entry(char *view_name , char *view_value ,struct database_view_node *vn_view_node)

{
  int32_t  index;
  uint8_t  node_found_b = 0 ;
  int32_t  hashkey ,offset ;
  struct   database_view_node *view_node_entry_p;

  for(index = 0;index<NSRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(nschain_object_db_view[index].view , view_name))
     {
        OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"View found ");
        node_found_b = 1;
        break;
     }
  }
  if(node_found_b == 0)
  {
    OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_ERROR,"Node not found ");
    return NSRM_FAILURE;
  }
  node_found_b = 0; 
  offset = 0;
  CNTLR_RCU_READ_LOCK_TAKE();
  MCHASH_TABLE_SCAN(nschain_object_db_view[index].db_viewtbl_p , hashkey)
  {
    MCHASH_BUCKET_SCAN(nschain_object_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
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
     OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Node entry NOT found");
     return NSRM_FAILURE;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Node entry found");
  return NSRM_SUCCESS;
}

int32_t nsrm_get_exact_nschain_object_view_entry(char *view_name , char *view_value ,struct database_view_node *vn_view_node)

{
  int32_t  index;
  uint8_t  node_found_b = 0 ;
  int32_t  hashkey ,offset ;
  struct   database_view_node *view_node_entry_p;

  for(index = 0;index<NSRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(nschain_object_db_view[index].view , view_name))
     {
        OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"View found ");
        node_found_b = 1;
        break;
     }
  }
  if(node_found_b == 0)
  {
    OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_ERROR,"Node not found ");
    return NSRM_FAILURE;
  }
  node_found_b =0; 
  offset = 0;
  CNTLR_RCU_READ_LOCK_TAKE();
  MCHASH_TABLE_SCAN(nschain_object_db_view[index].db_viewtbl_p ,hashkey)
  {
    MCHASH_BUCKET_SCAN(nschain_object_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
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
     OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Node entry NOT found");
     return NSRM_FAILURE;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  strcpy(vn_view_node->view_value , view_node_entry_p->view_value);
  strcpy(vn_view_node->node_name , view_node_entry_p->node_name);
  OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Node entry found");
  return NSRM_SUCCESS;
}
 
int32_t nsrm_get_nschain_object_by_view(char *view_name , char *view_value ,struct   nsrm_nschain_object_record* recs_p)
{
  int32_t  index;
  uint8_t  node_found_b = 0 ;
  int32_t  hashkey ,offset ,ret_val;
  struct   database_view_node      *view_node_entry_p;
  struct   nsrm_nschain_object     *nschain_object_node_p;

  for(index = 0;index<NSRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(nschain_object_db_view[index].view , view_name))
     {
        OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"View found ");
        node_found_b = 1;
        break;
     }
  }
  if(node_found_b == 0)
  {
    OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_ERROR,"Node not found ");
    return NSRM_FAILURE;
  }
  node_found_b =0; 
  offset = 0;
  CNTLR_RCU_READ_LOCK_TAKE();
  MCHASH_TABLE_SCAN(nschain_object_db_view[index].db_viewtbl_p ,hashkey)
  {
    MCHASH_BUCKET_SCAN(nschain_object_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
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
     OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Node entry NOT found");
     return NSRM_FAILURE;
  }
  ret_val = nsrm_get_nschain_byhandle(view_node_entry_p->node_saferef , &nschain_object_node_p);
  if(ret_val != NSRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Node not found");
    CNTLR_RCU_READ_LOCK_RELEASE();
    return NSRM_FAILURE;
  }
  strcpy(recs_p->key.name_p , nschain_object_node_p->name_p);
  strcpy(recs_p->key.tenant_name_p , nschain_object_node_p->tenant_name_p);
  
  /*
  recs_p->info[0].value.nschain_type             = nschain_object_node_p->nschain_type;
  recs_p->info[1].value.load_sharing_algorithm_e = nschain_object_node_p->load_sharing_algorithm_e;
  recs_p->info[2].value.load_indication_type_e   = nschain_object_node_p->load_indication_type_e;
  recs_p->info[3].value.high_threshold           = nschain_object_node_p->high_threshold;
  recs_p->info[4].value.low_threshold            = nschain_object_node_p->low_threshold ;
  */
  
  recs_p->info[0].value.packet_fields_to_hash    = nschain_object_node_p->packet_fields_to_hash;
  recs_p->info[1].value.admin_status_e           = nschain_object_node_p->admin_status_e;

  CNTLR_RCU_READ_LOCK_RELEASE();
  OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"nschain found by view");
  return NSRM_SUCCESS;
}
  

