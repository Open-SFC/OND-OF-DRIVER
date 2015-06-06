
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
 * File name:nsrm_service.c
 * Author: P.Vinod Sarma <B46178@freescale.com>
 * Date:   21/01/2014
 * Description: 
*/

#include  "nsrm_system_dependencies.h"
#include  "crm_tenant_api.h"

extern void     *nsrm_nwservice_object_mempool_g;
extern uint32_t nsrm_no_of_nwservice_object_buckets_g;
extern void     *nsrm_notifications_mempool_g;
extern struct   mchash_table* nsrm_nwservice_object_table_g ;

extern of_list_t nsrm_nwservice_object_notifications[];
struct database_view nwservice_object_db_view[NSRM_MAX_VIEW_DATABASE];

/***********************************************************************************************/
int32_t nsrm_get_nwservice_object_handle(char *nwservice_object_name_p, uint64_t *nwservice_object_handle_p)
{
   uint32_t hashkey;
   int32_t  offset;
   struct nsrm_nwservice_object* nwservice_object_entry_p = NULL;

   if(nsrm_nwservice_object_table_g == NULL)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "There is no hash table!.");
     return NSRM_FAILURE;
   }
   hashkey = nsrm_get_hashval_byname(nwservice_object_name_p, nsrm_no_of_nwservice_object_buckets_g);
   CNTLR_RCU_READ_LOCK_TAKE();
   offset = NSRM_NWSERVICE_OBJECT_TBL_OFFSET;

   MCHASH_BUCKET_SCAN(nsrm_nwservice_object_table_g, hashkey, nwservice_object_entry_p,struct nsrm_nwservice_object*,offset)
   {
     if(strcmp(nwservice_object_entry_p->name_p, nwservice_object_name_p))
     {
       continue;
     }
     *nwservice_object_handle_p = nwservice_object_entry_p->magic;
     *nwservice_object_handle_p = (((*nwservice_object_handle_p)<<32) | (nwservice_object_entry_p->index));
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_SUCCESS;
  }
    CNTLR_RCU_READ_LOCK_RELEASE();
    OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Nwservice object  name not found!.");
    return NSRM_ERROR_NWSERVICE_OBJECT_NAME_INVALID;
}

int32_t nsrm_get_nwservice_byhandle(uint64_t nwservice_handle,struct nsrm_nwservice_object
                                                              **nwservice_object_p_p)
{
  /* Caller shall take RCU locks. */
  uint32_t index,magic;
  uint8_t status_b;

  magic = (uint32_t)(nwservice_handle >>32);
  index = (uint32_t)nwservice_handle;

  MCHASH_ACCESS_NODE(nsrm_nwservice_object_table_g,index,magic,*nwservice_object_p_p,status_b);
  if(TRUE == status_b)
    return NSRM_SUCCESS;
  return NSRM_FAILURE;
}
/***************************************************************************************************/

int32_t nsrm_add_nwservice_object(
        int32_t number_of_config_params,
        struct  nsrm_nwservice_object_key*         key_info_p,
        struct  nsrm_nwservice_object_config_info* config_info_p)
{
  
   struct nsrm_nwservice_object   *nwservice_object_p = NULL;
   struct nsrm_nwservice_object   *nwservice_object_node_scan_p = NULL;
   struct crm_tenant_config_info   crm_tenant_config_info_p = {};
   union  nsrm_nwservice_object_notification_data  notification_data = {};
   struct nsrm_notification_app_hook_info *app_entry_p = NULL;

   uint32_t  hashkey = 0,apphookoffset,index = 0,magic = 0;
   int32_t   offset;
   uint8_t   heap_b, status_b = FALSE;
   int32_t   status = NSRM_SUCCESS,ret_value;
   uchar8_t* hashobj_p = NULL;
   uint64_t  tenant_handle ,nwservice_object_handle;

   if(nsrm_nwservice_object_table_g == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "No nwservice object hash table")
      return NSRM_FAILURE;
   }

   if(key_info_p->name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Nwservice object name is null");
      return NSRM_ERROR_NWSERVICE_OBJECT_NAME_INVALID;
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
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"tenant added is :%s",crm_tenant_config_info_p.tenant_name);

   }
   if(number_of_config_params == 0)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"No parameters to be configured");
      return NSRM_FAILURE;
   } 
  
   CNTLR_RCU_READ_LOCK_TAKE();
   
   /*Calculate hash value*/
   offset = NSRM_NWSERVICE_OBJECT_TBL_OFFSET;
   do
   {
      MCHASH_BUCKET_SCAN(nsrm_nwservice_object_table_g ,hashkey ,nwservice_object_node_scan_p,
                         struct nsrm_nwservice_object* ,offset)
      {
        if(strcmp(key_info_p->name_p,nwservice_object_node_scan_p->name_p) !=0)
          continue;
        OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Duplicate nwservie object name : %s",key_info_p->name_p);
        status = NSRM_ERROR_DUPLICATE_RESOURCE;
        break;
      }
     ret_value = mempool_get_mem_block(nsrm_nwservice_object_mempool_g,
                                       ( uchar8_t **)&nwservice_object_p,&heap_b);
     if(ret_value!=MEMPOOL_SUCCESS)
     {
        OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to get memory block");
        status =  NSRM_FAILURE;
        break;
     }
     
     hashkey = nsrm_get_hashval_byname(key_info_p->name_p,nsrm_no_of_nwservice_object_buckets_g);
     nwservice_object_p->heap_b = heap_b;
     nwservice_object_p->name_p = (char*)calloc(1,(strlen(key_info_p->name_p)+1));
     nwservice_object_p->tenant_name_p = (char*)calloc(1,(strlen(key_info_p->tenant_name_p)+1));
     strcpy(nwservice_object_p->name_p , key_info_p->name_p);
     strcpy(nwservice_object_p->tenant_name_p , key_info_p->tenant_name_p);
     
     if(config_info_p[5].value.high_threshold < config_info_p[6].value.low_threshold)
     {
       OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "High threshold should be not less than low threshold");
       status = NSRM_FAILURE;
       break;
     }
    
     nwservice_object_p->type_e = config_info_p[0].value.nwservice_object_form_factor_type_e;
     nwservice_object_p->admin_status_e = config_info_p[1].value.admin_status_e;
     nwservice_object_p->nwservice_type_e = config_info_p[2].value.nwservice_type_e;
     nwservice_object_p->load_sharing_algorithm_e = config_info_p[3].value.load_sharing_algorithm_e;
     nwservice_object_p->load_indication_type_e   = config_info_p[4].value.load_indication_type_e;
     nwservice_object_p->high_threshold           = config_info_p[5].value.high_threshold;
     nwservice_object_p->low_threshold            = config_info_p[6].value.low_threshold;

     nwservice_object_p->operational_status_e = OPER_DISABLE;
     hashobj_p = (uchar8_t *)nwservice_object_p + NSRM_NWSERVICE_OBJECT_TBL_OFFSET;
     MCHASH_APPEND_NODE(nsrm_nwservice_object_table_g, hashkey, nwservice_object_p,index, magic, hashobj_p, status_b);
 
     if(FALSE == status_b)
     {
        OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR," Failed to append nwservice object node to Hash table");
        status = NSRM_NWSERVICE_OBJECT_ADD_FAIL;
        break;
     }
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG," name: %s",nwservice_object_p->name_p);
     nwservice_object_handle   = magic;
     nwservice_object_handle   = (((nwservice_object_handle)<<32) | (index));
     nwservice_object_p->magic = magic;
     nwservice_object_p->index = index;
     nwservice_object_p->nwservice_object_handle = nwservice_object_handle;

     OF_LIST_INIT(nwservice_object_p->attributes, nsrm_attributes_free_attribute_entry_rcu);
      
     apphookoffset = NSRM_NOTIFICATION_APP_HOOK_OFFSET;
     notification_data.add_del.nwservice_object_name_p = (char*)calloc(1,(strlen(nwservice_object_p->name_p)+1));
     strcpy(notification_data.add_del.nwservice_object_name_p ,nwservice_object_p->name_p);
     notification_data.add_del.nwservice_object_handle = nwservice_object_p->nwservice_object_handle;

     OF_LIST_SCAN(nsrm_nwservice_object_notifications[NSRM_NWSERVICE_OBJECT_ADDED], 
                  app_entry_p,
                  struct nsrm_notification_app_hook_info *,
                  apphookoffset)
     {
        ((nsrm_nwservice_object_notifier)(app_entry_p->call_back))(NSRM_NWSERVICE_OBJECT_ADDED,
                                                                   nwservice_object_handle,
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
        ((nsrm_nwservice_object_notifier)(app_entry_p->call_back))(NSRM_NWSERVICE_OBJECT_ADDED,
                                                                   nwservice_object_handle,
                                                                   notification_data,
                                                                   app_entry_p->cbk_arg1,
                                                                   app_entry_p->cbk_arg2
                                                                   );
     }

     CNTLR_RCU_READ_LOCK_RELEASE();
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "nwservice object added successfully to the hashtable.");
     return NSRM_SUCCESS;
  }while(0);

  if(status != NSRM_SUCCESS)
  {
    if(nwservice_object_p)
    {
      free(nwservice_object_p->name_p);
      free(nwservice_object_p->tenant_name_p);
      if(mempool_release_mem_block(nsrm_nwservice_object_mempool_g,(uchar8_t*)&nwservice_object_p,
                                   nwservice_object_p->heap_b) != MEMPOOL_SUCCESS)
             
      { 
         OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to release  allocated memory block.");
      }
   }
 }
   CNTLR_RCU_READ_LOCK_RELEASE();
   return status;
}

 
int32_t nsrm_del_nwservice_object(
        struct nsrm_nwservice_object_key*   key_info_p)

{
   struct nsrm_nwservice_object   *nwservice_object_p = NULL;
   union  nsrm_nwservice_object_notification_data  notification_data = {};
   struct nsrm_notification_app_hook_info *app_entry_p = NULL;

   uint32_t  index,magic;
   int32_t   lstoffset,offset,ret_value;
   uint8_t   status_b = FALSE;
   uint64_t  nwservice_object_handle;

   if(nsrm_nwservice_object_table_g == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"nwservice hash table doesnt exist");
      return NSRM_FAILURE;
   }

   if(key_info_p->name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"nwservice object name is invalid");
      return NSRM_ERROR_NWSERVICE_OBJECT_NAME_INVALID;
   }
 
   ret_value = nsrm_get_nwservice_object_handle(key_info_p->name_p ,&nwservice_object_handle);
   if(ret_value != NSRM_SUCCESS)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"nwservice object doesnt exist");
     return NSRM_ERROR_NWSERVICE_OBJECT_NAME_INVALID;
   }

   magic = (uint32_t)(nwservice_object_handle >>32);
   index = (uint32_t)nwservice_object_handle;

   CNTLR_RCU_READ_LOCK_TAKE();

   MCHASH_ACCESS_NODE(nsrm_nwservice_object_table_g,index,magic,nwservice_object_p,status_b);
   if(FALSE == status_b)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"no node found");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   }
   if(strcmp(key_info_p->tenant_name_p , nwservice_object_p->tenant_name_p) != 0)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"This nwservice is not present in this tenant");
      return NSRM_FAILURE;
   }
   if(nwservice_object_p->attach_status != 0)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"this nwservice is attached to a nschain. Detach it first");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   } 
   if(nwservice_object_p->bypass_attach_count != 0)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"this nwservice is attached to a bypass rule");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   }

   lstoffset = NSRM_NOTIFICATION_APP_HOOK_OFFSET;
   notification_data.add_del.nwservice_object_name_p = (char*)calloc(1,(strlen(nwservice_object_p->name_p)+1));
   strcpy(notification_data.add_del.nwservice_object_name_p ,nwservice_object_p->name_p);
   notification_data.add_del.nwservice_object_handle = nwservice_object_p->nwservice_object_handle;

   OF_LIST_SCAN(nsrm_nwservice_object_notifications[NSRM_NWSERVICE_OBJECT_DELETED] ,
                  app_entry_p,
                  struct nsrm_notification_app_hook_info *,
                  lstoffset)
   {
      ((nsrm_nwservice_object_notifier)(app_entry_p->call_back))(NSRM_NWSERVICE_OBJECT_DELETED,
                                                                   nwservice_object_handle,
                                                                   notification_data,
                                                                   app_entry_p->cbk_arg1,
                                                                   app_entry_p->cbk_arg2
                                                                   );
   } 
   
   OF_LIST_SCAN(nsrm_nwservice_object_notifications[NSRM_NWSERVICE_OBJECT_ALL] ,
                  app_entry_p,
                  struct nsrm_notification_app_hook_info *,
                  lstoffset)
   {
      ((nsrm_nwservice_object_notifier)(app_entry_p->call_back))(NSRM_NWSERVICE_OBJECT_DELETED,
                                                                   nwservice_object_handle,
                                                                   notification_data,
                                                                   app_entry_p->cbk_arg1,
                                                                   app_entry_p->cbk_arg2
                                                                   );
   }
   offset = NSRM_NWSERVICE_OBJECT_TBL_OFFSET;
   MCHASH_DELETE_NODE_BY_REF(nsrm_nwservice_object_table_g, index, magic, struct nsrm_nwservice_object *,
                            offset, status_b);
   CNTLR_RCU_READ_LOCK_RELEASE();
   if(status_b == TRUE)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "nwservice object deleted successfully");
      return NSRM_SUCCESS;
   }
   else
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "failed to delete nwservice object");
      return NSRM_NWSERVICE_OBJECT_DEL_FAIL;
   }
}


int32_t nsrm_modify_nwservice_object(
          struct  nsrm_nwservice_object_key*           key_info_p,
          int32_t number_of_config_params,
          struct  nsrm_nwservice_object_config_info*   config_info_p)
{
   struct nsrm_nwservice_object    *nwservice_object_node_scan_p = NULL;

   uint64_t   hashkey;
   int32_t    offset;
   int32_t    ret_value , nwservice_object_node_found = 0;
   uint64_t   nwservice_object_handle;
   
   if(nsrm_nwservice_object_table_g == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"No hash table exists");
      return NSRM_FAILURE;
   }

   if(key_info_p->name_p == NULL || key_info_p->tenant_name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Invalid Key info");
      return NSRM_FAILURE;
   }

   ret_value = nsrm_get_nwservice_object_handle(key_info_p->name_p , &nwservice_object_handle);
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Invalid nwservice object name");
      return NSRM_ERROR_NWSERVICE_OBJECT_NAME_INVALID;
   }
   CNTLR_RCU_READ_LOCK_TAKE();
   hashkey = nsrm_get_hashval_byname(key_info_p->name_p,nsrm_no_of_nwservice_object_buckets_g);
   
   offset = NSRM_NWSERVICE_OBJECT_TBL_OFFSET;
   MCHASH_BUCKET_SCAN(nsrm_nwservice_object_table_g ,hashkey ,nwservice_object_node_scan_p,
                      struct nsrm_nwservice_object* ,offset)
   {
      if((!strcmp(key_info_p->name_p , nwservice_object_node_scan_p-> name_p)) && 
         (!strcmp(key_info_p->tenant_name_p , nwservice_object_node_scan_p->tenant_name_p)))
      {
         nwservice_object_node_found =1;
      }
   }
 
   if(!nwservice_object_node_found)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"nwservice name NOT found to modify %s" , key_info_p->name_p);
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_NWSERVICE_OBJECT_MOD_FAIL;
   }

   nwservice_object_node_scan_p->name_p = (char*)calloc(1,(strlen(key_info_p->name_p)+1));
   nwservice_object_node_scan_p->tenant_name_p = (char*)calloc(1,(strlen(key_info_p->tenant_name_p)+1));
   strcpy(nwservice_object_node_scan_p->name_p , key_info_p->name_p);
   strcpy(nwservice_object_node_scan_p->tenant_name_p , key_info_p->tenant_name_p);

   nwservice_object_node_scan_p->type_e = config_info_p[0].value.nwservice_object_form_factor_type_e;
   nwservice_object_node_scan_p->admin_status_e = config_info_p[1].value.admin_status_e;
   nwservice_object_node_scan_p->nwservice_type_e = config_info_p[2].value.nwservice_type_e;
   nwservice_object_node_scan_p->load_sharing_algorithm_e = config_info_p[3].value.load_sharing_algorithm_e;
   nwservice_object_node_scan_p->load_indication_type_e   = config_info_p[4].value.load_indication_type_e;
   nwservice_object_node_scan_p->high_threshold           = config_info_p[5].value.high_threshold;
   nwservice_object_node_scan_p->low_threshold            = config_info_p[6].value.low_threshold;


   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "nwservice Modified Successfully!");
   CNTLR_RCU_READ_LOCK_RELEASE();
   return NSRM_SUCCESS;
}
  
     
int32_t nsrm_get_first_nwservice_objects(
         int32_t  number_of_nwservice_objects_requested,
         int32_t* number_of_nwservice_objects_returned_p,
         struct   nsrm_nwservice_object_record* recs_p_p)
{
   struct    nsrm_nwservice_object    *nwservice_object_node_scan_p = NULL;
   uint64_t  hashkey;
   int32_t   offset,nwservice_objects_returned = 0;
   int32_t   nwservice_objects_requested = number_of_nwservice_objects_requested;
   int32_t   status = NSRM_FAILURE;

   if(nsrm_nwservice_object_table_g == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"No hash table exists");
      return NSRM_FAILURE;
   }
 
   CNTLR_RCU_READ_LOCK_TAKE();
   offset = NSRM_NWSERVICE_OBJECT_TBL_OFFSET;
   MCHASH_TABLE_SCAN(nsrm_nwservice_object_table_g , hashkey)
   {
      MCHASH_BUCKET_SCAN(nsrm_nwservice_object_table_g, hashkey, nwservice_object_node_scan_p,
                           struct nsrm_nwservice_object*,offset)
      {
         if((strlen(nwservice_object_node_scan_p->name_p) > NSRM_MAX_NAME_LENGTH) || (strlen(nwservice_object_node_scan_p->tenant_name_p) > NSRM_MAX_NAME_LENGTH))
           return NSRM_ERROR_LENGTH_NOT_ENOUGH;
         strcpy(recs_p_p[nwservice_objects_returned].key.name_p , nwservice_object_node_scan_p->name_p);
         strcpy(recs_p_p[nwservice_objects_returned].key.tenant_name_p , nwservice_object_node_scan_p->tenant_name_p);
         recs_p_p[nwservice_objects_returned].info[0].value.nwservice_object_form_factor_type_e = nwservice_object_node_scan_p->type_e;
         recs_p_p[nwservice_objects_returned].info[1].value.admin_status_e = nwservice_object_node_scan_p->admin_status_e;
         
         recs_p_p[nwservice_objects_returned].info[2].value.nwservice_type_e = nwservice_object_node_scan_p->nwservice_type_e;
         recs_p_p[nwservice_objects_returned].info[3].value.load_sharing_algorithm_e = nwservice_object_node_scan_p->load_sharing_algorithm_e;
         recs_p_p[nwservice_objects_returned].info[4].value.load_indication_type_e   = nwservice_object_node_scan_p->load_indication_type_e;
         recs_p_p[nwservice_objects_returned].info[5].value.high_threshold   = nwservice_object_node_scan_p->high_threshold;
         recs_p_p[nwservice_objects_returned].info[6].value.low_threshold    = nwservice_object_node_scan_p->low_threshold;

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
  
int32_t nsrm_get_next_nwservice_object(
        struct   nsrm_nwservice_object_key* relative_record_key_p,
        int32_t  number_of_nwservice_objects_requested,
        int32_t* number_of_nwservice_objects_returned_p,
        struct   nsrm_nwservice_object_record* recs_p_p)
{
   struct    nsrm_nwservice_object    *nwservice_object_node_scan_p = NULL;
   uint64_t  hashkey;
   uint8_t   current_entry_found_b;
   int32_t   nwservice_objects_returned = 0,offset; 
   int32_t   nwservice_objects_requested = number_of_nwservice_objects_requested;
   int32_t   status = NSRM_FAILURE; 
  
   offset = NSRM_NWSERVICE_OBJECT_TBL_OFFSET;
   current_entry_found_b = FALSE;   
 
   CNTLR_RCU_READ_LOCK_TAKE();
   MCHASH_TABLE_SCAN(nsrm_nwservice_object_table_g,hashkey)
   {
      MCHASH_BUCKET_SCAN(nsrm_nwservice_object_table_g,hashkey,nwservice_object_node_scan_p,
                         struct nsrm_nwservice_object*, offset)
      {
         if(current_entry_found_b == FALSE)
         {
            if(((strcmp(relative_record_key_p->name_p , nwservice_object_node_scan_p->name_p)) != 0) ||
        ((strcmp(relative_record_key_p->tenant_name_p,nwservice_object_node_scan_p->tenant_name_p) !=0)))
                 continue; 
            else
            {
               OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "nwservice entry found copy next nwservice");                      current_entry_found_b = TRUE;
               continue;
            }
         }
         strcpy(recs_p_p[nwservice_objects_returned].key.name_p , nwservice_object_node_scan_p->name_p);
         strcpy(recs_p_p[nwservice_objects_returned].key.tenant_name_p , nwservice_object_node_scan_p->tenant_name_p);
         recs_p_p[nwservice_objects_returned].info[0].value.nwservice_object_form_factor_type_e = nwservice_object_node_scan_p->type_e;
         recs_p_p[nwservice_objects_returned].info[1].value.admin_status_e = nwservice_object_node_scan_p->admin_status_e;
 
         recs_p_p[nwservice_objects_returned].info[2].value.nwservice_type_e = nwservice_object_node_scan_p->nwservice_type_e;
         recs_p_p[nwservice_objects_returned].info[3].value.load_sharing_algorithm_e = nwservice_object_node_scan_p->load_sharing_algorithm_e;
         recs_p_p[nwservice_objects_returned].info[4].value.load_indication_type_e   = nwservice_object_node_scan_p->load_indication_type_e;
         recs_p_p[nwservice_objects_returned].info[5].value.high_threshold   = nwservice_object_node_scan_p->high_threshold;
         recs_p_p[nwservice_objects_returned].info[6].value.low_threshold    = nwservice_object_node_scan_p->low_threshold;

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
   return status;
   if(status == NSRM_SUCCESS)
   {
      *number_of_nwservice_objects_returned_p = nwservice_objects_returned;
      CNTLR_RCU_READ_LOCK_RELEASE();
      return status;
   }
  
   CNTLR_RCU_READ_LOCK_RELEASE();
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to copy nwservice object Info.....");

   if(current_entry_found_b == TRUE)
     return NSRM_ERROR_NO_MORE_ENTRIES;
   else
     return NSRM_ERROR_NWSERVICE_OBJECT_NAME_INVALID;
}

  
int32_t nsrm_get_exact_nwservice_object(
        struct   nsrm_nwservice_object_key*   key_info_p,
        struct   nsrm_nwservice_object_record*   rec_p)
{
   struct nsrm_nwservice_object   *nwservice_object_node_scan_p = NULL;

   int32_t  offset;
   uint64_t hashkey;
   uint8_t  current_entry_found_b = FALSE;

   if(nsrm_nwservice_object_table_g == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nwservice object hash table doesnt exist");
      return NSRM_FAILURE;
   }

  if(key_info_p->name_p == NULL || key_info_p->tenant_name_p == NULL)
  {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "such node doesnt exist");
     return NSRM_FAILURE;
  }
  
   hashkey = nsrm_get_hashval_byname(key_info_p->name_p,nsrm_no_of_nwservice_object_buckets_g);
   offset = NSRM_NWSERVICE_OBJECT_TBL_OFFSET;

   CNTLR_RCU_READ_LOCK_TAKE();
   MCHASH_BUCKET_SCAN(nsrm_nwservice_object_table_g, hashkey, nwservice_object_node_scan_p,
                      struct nsrm_nwservice_object*,offset)
   {  
      if((strcmp(key_info_p->name_p,nwservice_object_node_scan_p->name_p) == 0) && (strcmp(key_info_p->tenant_name_p,nwservice_object_node_scan_p->tenant_name_p) ==0 ))
      {
        current_entry_found_b = TRUE;
      }
      else
        continue;
      strcpy(rec_p[0].key.name_p , nwservice_object_node_scan_p->name_p);
      strcpy(rec_p[0].key.tenant_name_p , nwservice_object_node_scan_p->tenant_name_p);
      rec_p[0].info[0].value.nwservice_object_form_factor_type_e = nwservice_object_node_scan_p->type_e;
      rec_p[0].info[1].value.admin_status_e = nwservice_object_node_scan_p->admin_status_e;
      rec_p[0].info[2].value.nwservice_type_e = nwservice_object_node_scan_p->nwservice_type_e;
      rec_p[0].info[3].value.load_sharing_algorithm_e = nwservice_object_node_scan_p->load_sharing_algorithm_e;
      rec_p[0].info[4].value.load_indication_type_e   = nwservice_object_node_scan_p->load_indication_type_e;
      rec_p[0].info[5].value.high_threshold   = nwservice_object_node_scan_p->high_threshold;
      rec_p[0].info[6].value.low_threshold    = nwservice_object_node_scan_p->low_threshold;

      CNTLR_RCU_READ_LOCK_RELEASE(); 
      return NSRM_SUCCESS;
   }
  CNTLR_RCU_READ_LOCK_RELEASE(); 
  OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to copy nwservice object Info.....");

  if(current_entry_found_b == TRUE)
    return NSRM_ERROR_NO_MORE_ENTRIES;
  else
    return NSRM_ERROR_NWSERVICE_OBJECT_NAME_INVALID;
}
       
int32_t nsrm_register_nwservice_object_notifications(
                          uint8_t notification_type,
                          nsrm_nwservice_object_notifier  nwservice_notifier,
                          void* callback_arg1,
                          void* callback_arg2)    
{
   struct  nsrm_notification_app_hook_info *app_entry_p=NULL;
   uint8_t heap_b;
   int32_t retval = NSRM_SUCCESS;
   uchar8_t lstoffset;
   lstoffset = NSRM_NOTIFICATION_APP_HOOK_OFFSET;
   
   if((notification_type < NSRM_NWSERVICE_OBJECT_FIRST_NOTIFICATION_TYPE) ||
     (notification_type > NSRM_NWSERVICE_OBJECT_LAST_NOTIFICATION_TYPE))
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Invalid Notification type:%d", notification_type);
      return NSRM_FAILURE;
   }
   if(nwservice_notifier == NULL)
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

   app_entry_p->call_back = (void*)nwservice_notifier;
   app_entry_p->cbk_arg1  = callback_arg1;
   app_entry_p->cbk_arg2  = callback_arg2;
   app_entry_p->heap_b    = heap_b;
  
   /* Add Application to the list of nwservice object notifications */
   OF_APPEND_NODE_TO_LIST(nsrm_nwservice_object_notifications[notification_type],app_entry_p, lstoffset); 
   if(retval != NSRM_SUCCESS)
     mempool_release_mem_block(nsrm_notifications_mempool_g,(uchar8_t*)app_entry_p, app_entry_p->heap_b);

   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "callback function reg with nwservice object success");
   return retval;
}

int32_t nsrm_add_attribute_to_nwservice_object(
          char*     nwservice_object_name_p,
          struct    nsrm_attribute_name_value_pair *attribute_info_p)
{
  int32_t  ret_val;
  struct   nsrm_nwservice_object                         *nwservice_object_entry_p;
  union    nsrm_nwservice_object_notification_data        notification_data={};
  struct   nsrm_notification_app_hook_info               *app_entry_p;
  int32_t  apphookoffset;
  uint64_t nwservice_object_handle;
  apphookoffset = NSRM_NOTIFICATION_APP_HOOK_OFFSET;

  CNTLR_RCU_READ_LOCK_TAKE();
  ret_val = nsrm_get_nwservice_object_handle(nwservice_object_name_p,&nwservice_object_handle);
  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return ret_val;
  }
  ret_val = nsrm_get_nwservice_byhandle(nwservice_object_handle,&nwservice_object_entry_p);
  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return ret_val;
  }

  ret_val = nsrm_add_attribute(&(nwservice_object_entry_p->attributes),attribute_info_p);
  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return ret_val;
  }
  OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "added attr");
  nwservice_object_entry_p->number_of_attributes++;
  nsrm_create_nwservice_object_view_entry(nwservice_object_handle, attribute_info_p->attribute_name_p, attribute_info_p->attribute_value_p);

  notification_data.attribute.nwservice_object_name_p = (char *)calloc(1,strlen(nwservice_object_entry_p->name_p)+1);
  notification_data.attribute.nwservice_attribute_name_p = (char *)calloc(1,strlen(attribute_info_p->attribute_name_p)+1);
  notification_data.attribute.nwservice_attribute_value_p = (char *)calloc(1,strlen(attribute_info_p->attribute_value_p)+1);
  strcpy(notification_data.attribute.nwservice_object_name_p, nwservice_object_entry_p->name_p);
  strcpy(notification_data.attribute.nwservice_attribute_name_p,attribute_info_p->attribute_name_p);
  strcpy(notification_data.attribute.nwservice_attribute_value_p,attribute_info_p->attribute_value_p);
  notification_data.attribute.nwservice_object_handle = nwservice_object_handle;

  OF_LIST_SCAN(nsrm_nwservice_object_notifications[NSRM_NWSERVICE_OBJECT_ATTRIBUTE_ADDED],
               app_entry_p,
               struct nsrm_notification_app_hook_info *,
               apphookoffset)
  {
     ((nsrm_nwservice_object_notifier)(app_entry_p->call_back))(NSRM_NWSERVICE_OBJECT_ATTRIBUTE_ADDED,
                                                                nwservice_object_handle,
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
        ((nsrm_nwservice_object_notifier)(app_entry_p->call_back))(NSRM_NWSERVICE_OBJECT_ATTRIBUTE_ADDED,
                                                                   nwservice_object_handle,
                                                                   notification_data,
                                                                   app_entry_p->cbk_arg1,
                                                                   app_entry_p->cbk_arg2
                                                                   );
   }

  OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "success");
  CNTLR_RCU_READ_LOCK_RELEASE();
  return NSRM_SUCCESS;
}
 

int32_t nsrm_del_attribute_from_nwservice_object(
                                                 char* nwservice_object_name_p,
                                                 char* attribute_name_p)
{
  int32_t  ret_val;
  struct   nsrm_nwservice_object                         *nwservice_object_entry_p;
  union    nsrm_nwservice_object_notification_data        notification_data={};
  struct   nsrm_notification_app_hook_info               *app_entry_p;
  struct   nsrm_resource_attribute_entry *attribute_entry_p,*prev_attr_entry_p = NULL;
  int32_t  apphookoffset;
  uint64_t nwservice_object_handle;
  apphookoffset = NSRM_NOTIFICATION_APP_HOOK_OFFSET;
  uint8_t  attr_node_found = 0;

  CNTLR_RCU_READ_LOCK_TAKE();
  ret_val = nsrm_get_nwservice_object_handle(nwservice_object_name_p,&nwservice_object_handle);
  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return ret_val;
  }
  ret_val = nsrm_get_nwservice_byhandle(nwservice_object_handle,&nwservice_object_entry_p);
  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return ret_val;
  }
  
  OF_LIST_SCAN(nwservice_object_entry_p->attributes, attribute_entry_p, struct nsrm_resource_attribute_entry*,apphookoffset)
  {
    if(!strcmp(attribute_name_p, attribute_entry_p->name))
    {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "attr name %s found to delete",attribute_entry_p->name);
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
   notification_data.attribute.nwservice_object_name_p = (char *)calloc(1,strlen(nwservice_object_entry_p->name_p)+1);
  notification_data.attribute.nwservice_attribute_name_p = (char *)calloc(1,strlen(attribute_name_p)+1);
  /*
  notification_data.attribute.nwservice_attribute_value_p = (char *)calloc(1,strlen(attribute_info_p->attribute_value_p)+1);
  */
  strcpy(notification_data.attribute.nwservice_object_name_p, nwservice_object_entry_p->name_p);
  strcpy(notification_data.attribute.nwservice_attribute_name_p,attribute_name_p);
//  strcpy(notification_data.attribute.nwservice_attribute_value_p,attribute_info_p->attribute_value_p);
  notification_data.attribute.nwservice_object_handle = nwservice_object_handle;

  OF_LIST_SCAN(nsrm_nwservice_object_notifications[NSRM_NWSERVICE_OBJECT_ATTRIBUTE_ADDED],
               app_entry_p,
               struct nsrm_notification_app_hook_info *,
               apphookoffset)
  {
     ((nsrm_nwservice_object_notifier)(app_entry_p->call_back))(NSRM_NWSERVICE_OBJECT_ATTRIBUTE_ADDED,
                                                                nwservice_object_handle,
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
        ((nsrm_nwservice_object_notifier)(app_entry_p->call_back))(NSRM_NWSERVICE_OBJECT_ATTRIBUTE_ADDED,
                                                                   nwservice_object_handle,
                                                                   notification_data,
                                                                   app_entry_p->cbk_arg1,
                                                                   app_entry_p->cbk_arg2
                                                                   );
   }

   OF_REMOVE_NODE_FROM_LIST(nwservice_object_entry_p->attributes,attribute_entry_p,prev_attr_entry_p,apphookoffset);
   nwservice_object_entry_p->number_of_attributes--;
   nsrm_remove_nwservice_object_view_entry(attribute_entry_p->name, attribute_entry_p->value);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return NSRM_SUCCESS;
}


int32_t nsrm_get_first_nwservice_object_attribute(
          char*   nwservice_object_name_p,
          struct  nsrm_attribute_name_value_output_info *attribute_output_p) 
{
  struct   nsrm_nwservice_object      *nwservice_object_entry_p;
  uint64_t nwservice_object_handle;
  int32_t ret_val = NSRM_SUCCESS;
  
  CNTLR_RCU_READ_LOCK_TAKE();

  do
  {
    ret_val = nsrm_get_nwservice_object_handle(nwservice_object_name_p,&nwservice_object_handle);
    if(ret_val != NSRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"No nwservice exists");  
      break;
    }
    ret_val = nsrm_get_nwservice_byhandle(nwservice_object_handle,&nwservice_object_entry_p);
    if(ret_val != NSRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"No nwservice exists");  
      break;
    }
   
    ret_val = nsrm_get_first_attribute(&(nwservice_object_entry_p->attributes), attribute_output_p);
    if(ret_val != NSRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"nsrm_get_first_attribute failure");  
      break;
    }
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
  return ret_val;
} 

int32_t nsrm_get_next_nwservice_object_attribute(
          char*   nwservice_object_name_p,
          char*   current_attribute_name_p,
          struct  nsrm_attribute_name_value_output_info *attribute_output_p) 
{
  struct   nsrm_nwservice_object      *nwservice_object_entry_p;
  uint64_t nwservice_object_handle;
  int32_t ret_val = NSRM_SUCCESS;
  
  CNTLR_RCU_READ_LOCK_TAKE();

  do
  {
    ret_val = nsrm_get_nwservice_object_handle(nwservice_object_name_p,&nwservice_object_handle);
    if(ret_val != NSRM_SUCCESS)
      break;
    ret_val = nsrm_get_nwservice_byhandle(nwservice_object_handle,&nwservice_object_entry_p);
    if(ret_val != NSRM_SUCCESS)
      break;
   
    ret_val = nsrm_get_next_attribute(&(nwservice_object_entry_p->attributes),current_attribute_name_p,
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

  return ret_val;
} 


int32_t nsrm_get_exact_nwservice_object_attribute(
          char*  nwservice_object_name_p,
          char*  attribute_name_p,
          struct nsrm_attribute_name_value_output_info *attribute_output_p
          )
{
  struct   nsrm_nwservice_object      *nwservice_object_entry_p;
  uint64_t nwservice_object_handle;
  int32_t ret_val = NSRM_SUCCESS;
  
  CNTLR_RCU_READ_LOCK_TAKE();

  if(nwservice_object_name_p == NULL)
  {
    OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"nwservice name is null");
    return NSRM_FAILURE;
  }
  do
  {
    ret_val = nsrm_get_nwservice_object_handle(nwservice_object_name_p,&nwservice_object_handle);
    if(ret_val != NSRM_SUCCESS)
      break;
    ret_val = nsrm_get_nwservice_byhandle(nwservice_object_handle,&nwservice_object_entry_p);
    if(ret_val != NSRM_SUCCESS)
      break;
   
    ret_val = nsrm_get_exact_attribute(&(nwservice_object_entry_p->attributes),attribute_name_p,attribute_output_p);
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

  return ret_val;
}
/******************************************************************************************************/
int32_t nsrm_create_nwservice_object_view(char *view_name)
{
  int32_t index =0 , ii;
  uint8_t empty_index_b = 0;

  for(ii = 0;ii<NSRM_MAX_VIEW_DATABASE ; ii++)
  {
     if(!strcmp(nwservice_object_db_view[ii].view , view_name))
     {
        OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"View already created");
        return NSRM_FAILURE;
     } 
     else if((empty_index_b == 0) && (!strcmp(nwservice_object_db_view[ii].view ,"")))    
     {
        empty_index_b = 1;
        index = ii;
     }
  } 
  strcpy(nwservice_object_db_view[index].view,view_name);
  nsrm_view_hash_table_init(&nwservice_object_db_view[index].db_viewtbl_p);
  nwservice_object_db_view[index].uniqueness_flag = FALSE; 
  nwservice_object_db_view[index].valid_b = TRUE;
 
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "View node created succesfully");
  return NSRM_SUCCESS;
}
  

int32_t nsrm_create_nwservice_object_view_entry(uint64_t nwservice_handle , char* view_name ,char* view_value)
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
       if(!strcmp(nwservice_object_db_view[view_index].view ,view_name))
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
     
     hashkey = nsrm_get_hashval_byname(view_value,nsrm_no_of_nwservice_object_buckets_g);
     CNTLR_RCU_READ_LOCK_TAKE();
     view_node_entry_p = nsrm_alloc_view_entry_and_set_values(nwservice_handle, view_name, view_value);
     if (view_node_entry_p == NULL)
     {
       status = NSRM_FAILURE;
       break;
     }
     MCHASH_APPEND_NODE(nwservice_object_db_view[view_index].db_viewtbl_p,hashkey,view_node_entry_p,index,magic,view_node_entry_p,status_b);
     if(FALSE == status_b)
     {
       status = NSRM_FAILURE;
       CNTLR_RCU_READ_LOCK_RELEASE();
       break;
     }
     view_node_entry_p->magic = magic;
     view_node_entry_p->index = index;
     OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"View entry succesfuuly added");
     CNTLR_RCU_READ_LOCK_RELEASE();
  }while(0);

  return status;
}

 
int32_t nsrm_remove_nwservice_object_view_entry(char* view_name,char* view_value)
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
       if(!strcmp(nwservice_object_db_view[index].view,view_name))
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
     hashkey = nsrm_get_hashval_byname(view_value,nsrm_no_of_nwservice_object_buckets_g);
     CNTLR_RCU_READ_LOCK_TAKE(); 
     MCHASH_BUCKET_SCAN(nwservice_object_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
     {
      if ((!strcmp(view_node_entry_p->view_value, view_value)) &&
          (!strcmp(view_node_entry_p->node_name, view_name)))
      {
        status_b = FALSE;
        offset = 0;
        MCHASH_DELETE_NODE_BY_REF(nwservice_object_db_view[index].db_viewtbl_p,view_node_entry_p->index,view_node_entry_p->magic,struct database_view_node *,offset,status_b);
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
 
int32_t nsrm_remove_nwservice_object_view(char *view_name)
{
  int32_t  index , ret_val;
  uint8_t  node_found_b = 0 ;
  uint64_t hashkey ,offset ;
  struct   database_view_node *view_node_entry_p;

  for(index = 0;index<NSRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(nwservice_object_db_view[index].view , view_name))
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
  
  MCHASH_TABLE_SCAN(nwservice_object_db_view[index].db_viewtbl_p,hashkey)
  {
    MCHASH_BUCKET_SCAN(nwservice_object_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
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
  ret_val = nsrm_view_hash_table_deinit(nwservice_object_db_view[index].db_viewtbl_p);
  if(ret_val != NSRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_ERROR,"Hash Table deinit failed");
    return NSRM_FAILURE;
  }
  strcpy(nwservice_object_db_view[index].view,"");
  nwservice_object_db_view[index].uniqueness_flag = FALSE; 
  nwservice_object_db_view[index].valid_b = FALSE;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "View node deleted succesfully");
  return NSRM_SUCCESS;
}


int32_t nsrm_get_first_nwservice_object_view_entry(char *view_name , struct database_view_node *vn_view_node)
{
  int32_t  index;
  uint8_t  node_found_b = 0 ;
  int32_t  hashkey ,offset ;
  struct   database_view_node *view_node_entry_p;

  for(index = 0;index<NSRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(nwservice_object_db_view[index].view , view_name))
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
  MCHASH_TABLE_SCAN(nwservice_object_db_view[index].db_viewtbl_p ,hashkey)
  {
    MCHASH_BUCKET_SCAN(nwservice_object_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
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
   
int32_t nsrm_get_next_nwservice_object_view_entry(char *view_name , char *view_value ,struct database_view_node *vn_view_node)

{
  int32_t  index;
  uint8_t  node_found_b = 0 ;
  int32_t  hashkey ,offset ;
  struct   database_view_node *view_node_entry_p;

  for(index = 0;index<NSRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(nwservice_object_db_view[index].view , view_name))
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
  MCHASH_TABLE_SCAN(nwservice_object_db_view[index].db_viewtbl_p , hashkey)
  {
    MCHASH_BUCKET_SCAN(nwservice_object_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
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

int32_t nsrm_get_exact_nwservice_object_view_entry(char *view_name , char *view_value ,struct database_view_node *vn_view_node)
{
  int32_t  index;
  uint8_t  node_found_b = 0 ;
  int32_t  hashkey ,offset ;
  struct   database_view_node *view_node_entry_p;

  for(index = 0;index<NSRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(nwservice_object_db_view[index].view , view_name))
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
  MCHASH_TABLE_SCAN(nwservice_object_db_view[index].db_viewtbl_p ,hashkey)
  {
    MCHASH_BUCKET_SCAN(nwservice_object_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
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
 
int32_t nsrm_get_nwservice_object_by_view(char *view_name , char *view_value ,struct   nsrm_nwservice_object_record* recs_p)
{
  int32_t  index;
  uint8_t  node_found_b = 0 ;
  int32_t  hashkey ,offset ,ret_val;
  struct   database_view_node      *view_node_entry_p;
  struct   nsrm_nwservice_object   *nwservice_object_node_p;

  for(index = 0;index<NSRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(nwservice_object_db_view[index].view , view_name))
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
  MCHASH_TABLE_SCAN(nwservice_object_db_view[index].db_viewtbl_p ,hashkey)
  {
    MCHASH_BUCKET_SCAN(nwservice_object_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
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
  OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"handle : %llx",view_node_entry_p->node_saferef);
  ret_val = nsrm_get_nwservice_byhandle(view_node_entry_p->node_saferef , &nwservice_object_node_p);
  if(ret_val != NSRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Node not found");
    CNTLR_RCU_READ_LOCK_RELEASE();
    return NSRM_FAILURE;
  }
  strcpy(recs_p->key.name_p , nwservice_object_node_p->name_p);
  recs_p->info[0].value.nwservice_object_form_factor_type_e = nwservice_object_node_p->type_e;
  strcpy(recs_p->key.tenant_name_p , nwservice_object_node_p->tenant_name_p);
  recs_p->info[1].value.admin_status_e = nwservice_object_node_p->admin_status_e;

  recs_p->info[2].value.nwservice_type_e = nwservice_object_node_p->nwservice_type_e;
  recs_p->info[3].value.load_sharing_algorithm_e = nwservice_object_node_p->load_sharing_algorithm_e;
  recs_p->info[4].value.load_indication_type_e   = nwservice_object_node_p->load_indication_type_e;
  recs_p->info[5].value.high_threshold   = nwservice_object_node_p->high_threshold;
  recs_p->info[6].value.low_threshold    = nwservice_object_node_p->low_threshold;

  CNTLR_RCU_READ_LOCK_RELEASE();
  OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"nwservice found by view");
  return NSRM_SUCCESS;
} 
