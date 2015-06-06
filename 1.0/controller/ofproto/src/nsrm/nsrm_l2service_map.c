
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
 * File name:nsrm_l2service_map.c
 * Author: P.Vinod Sarma <B46178@freescale.com>
 * Date:   28/01/2014
 * Description: 
*/

#include  "nsrm_system_dependencies.h"
#include  "crm_tenant_api.h"
#include  "crm_vn_api.h"

extern void     *nsrm_l2nw_service_map_mempool_g;
extern uint32_t nsrm_no_of_l2nw_service_map_buckets_g;
extern void     *nsrm_notifications_mempool_g;
extern struct   mchash_table* nsrm_l2nw_service_map_table_g ;

extern of_list_t nsrm_l2nw_service_map_notifications[];
struct database_view       l2nw_map_db_view[NSRM_MAX_VIEW_DATABASE];
/***********************************************************************************************/
int32_t nsrm_get_l2nw_service_map_handle(char *l2nw_service_map_name_p, uint64_t *l2nw_service_map_handle_p)
{
   uint32_t hashkey;
   int32_t offset;
   struct nsrm_l2nw_service_map* l2nw_service_map_entry_p = NULL;

   if(nsrm_l2nw_service_map_table_g == NULL)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "There is no hash table!.");
     return NSRM_FAILURE;
   }
   hashkey = nsrm_get_hashval_byname(l2nw_service_map_name_p, nsrm_no_of_l2nw_service_map_buckets_g);
   CNTLR_RCU_READ_LOCK_TAKE();
   offset = NSRM_L2NW_SERVICE_MAP_TBL_OFFSET;

   MCHASH_BUCKET_SCAN(nsrm_l2nw_service_map_table_g, hashkey, l2nw_service_map_entry_p,struct nsrm_l2nw_service_map*,offset)
   {
     if(strcmp(l2nw_service_map_entry_p->map_name_p, l2nw_service_map_name_p))
     {
       continue;
     }
     *l2nw_service_map_handle_p = l2nw_service_map_entry_p->magic;
     *l2nw_service_map_handle_p = (((*l2nw_service_map_handle_p)<<32) | (l2nw_service_map_entry_p->index));
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_SUCCESS;
  }
    CNTLR_RCU_READ_LOCK_RELEASE();
    OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "l2nw service map  name not found!.");
    return NSRM_ERROR_L2NW_SERVICE_MAP_NAME_INVALID;
}
/***************************************************************************************************/
int32_t nsrm_get_l2_smap_record_byhandle(uint64_t l2map_handle,
                                         struct nsrm_l2nw_service_map **l2nw_service_map_p_p)

{
  /*Caller shall take RCU locks*/

  uint32_t index,magic;
  uint8_t status_b;

  magic = (uint32_t)(l2map_handle >>32);
  index = (uint32_t)l2map_handle;

  MCHASH_ACCESS_NODE(nsrm_l2nw_service_map_table_g,index,magic,*l2nw_service_map_p_p,status_b);
  if(TRUE == status_b)
    return NSRM_SUCCESS;
  return NSRM_FAILURE;
}
/***************************************************************************************************/
int32_t nsrm_add_l2nw_service_map_record(
        int32_t number_of_config_params,
        struct  nsrm_l2nw_service_map_key*         key_info_p,
        struct  nsrm_l2nw_service_map_config_info* config_info_p)
{
  
   struct nsrm_l2nw_service_map   *l2nw_service_map_p = NULL;
   struct nsrm_l2nw_service_map   *l2nw_service_map_node_scan_p = NULL;
   struct crm_tenant_config_info   crm_tenant_config_info_p = {};
   struct crm_virtual_network     *crm_vn_info_p = NULL;
   struct nsrm_nschainset_object  *nschainset_object_node_scan_p = NULL;
   union  nsrm_l2nw_service_map_notification_data  notification_data = {};
   struct nsrm_notification_app_hook_info *app_entry_p = NULL;

   uint32_t  hashkey,apphookoffset,index = 0,magic = 0;
   int32_t   offset;
   uint8_t   heap_b, status_b = FALSE;
   int32_t   status = NSRM_SUCCESS,ret_value;
   uchar8_t* hashobj_p = NULL;
   uint64_t  tenant_handle;
   uint64_t  l2nw_service_map_handle , nschainset_object_handle ,vn_handle;

   if(nsrm_l2nw_service_map_table_g == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "No l2nw_service_map hash table")
      return NSRM_FAILURE;
   }

   if(key_info_p->map_name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "l2nw_service_map name is null");
      return NSRM_ERROR_L2NW_SERVICE_MAP_NAME_INVALID;
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

   if(key_info_p->vn_name_p != NULL)
   {
     ret_value = crm_get_vn_handle(key_info_p->vn_name_p,&vn_handle);
     if(ret_value != NSRM_SUCCESS)
     {
        OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "VN doesn't exist ");
        return NSRM_FAILURE;
     } 
   
   ret_value = crm_get_vn_byhandle(vn_handle,&crm_vn_info_p);
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "VN doesn't exist ");
      return NSRM_FAILURE;
   }
  }

   if(config_info_p[0].value.nschainset_object_name_p != NULL) 
   { 
     ret_value = nsrm_get_nschainset_object_handle(config_info_p[0].value.nschainset_object_name_p,&nschainset_object_handle);
     if(ret_value != NSRM_SUCCESS)
     {
        OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Nschain set doesn't exist ");
        return NSRM_ERROR_NSCHAINSET_OBJECT_NAME_INVALID;
     }
     CNTLR_RCU_READ_LOCK_TAKE();
     ret_value = nsrm_get_nschainset_byhandle(nschainset_object_handle ,&nschainset_object_node_scan_p);
     if(ret_value != NSRM_SUCCESS)
     {
        OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Nschain set doesn't exist ");
        CNTLR_RCU_READ_LOCK_RELEASE();
        return NSRM_ERROR_NSCHAINSET_OBJECT_NAME_INVALID;
     }
   }
   if(number_of_config_params == 0)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"No parameters to be configured");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   } 
   offset = NSRM_L2NW_SERVICE_MAP_TBL_OFFSET;
   do
   {
      MCHASH_TABLE_SCAN(nsrm_l2nw_service_map_table_g , hashkey)
      {
      MCHASH_BUCKET_SCAN(nsrm_l2nw_service_map_table_g ,hashkey ,l2nw_service_map_node_scan_p,
                         struct nsrm_l2nw_service_map* ,offset)
      {
        if((strcmp(key_info_p->map_name_p,l2nw_service_map_node_scan_p->map_name_p) ==0) || (strcmp(key_info_p->vn_name_p,l2nw_service_map_node_scan_p->vn_name_p) == 0))
        {
          OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"Duplicate resource found");
          status = NSRM_ERROR_DUPLICATE_RESOURCE;
          break;
        }
        else
          continue;
        OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Duplicate l2nwservice map name : %s",key_info_p->map_name_p);
        status = NSRM_ERROR_DUPLICATE_RESOURCE;
        break;
      }
    }
     if(status == NSRM_ERROR_DUPLICATE_RESOURCE)
     {
        OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"Duplicate resource found");
        CNTLR_RCU_READ_LOCK_RELEASE();
        return status;
     }
     
     hashkey = nsrm_get_hashval_byname(key_info_p->map_name_p,nsrm_no_of_l2nw_service_map_buckets_g);
     ret_value = mempool_get_mem_block(nsrm_l2nw_service_map_mempool_g,
                                       ( uchar8_t **)&l2nw_service_map_p,&heap_b);
     if(ret_value!=MEMPOOL_SUCCESS)
     {
        OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to get memory block");
        status =  NSRM_FAILURE;
        break;
     }
   
     l2nw_service_map_p->heap_b = heap_b;
     l2nw_service_map_p->map_name_p = (char*)calloc(1,128);
     l2nw_service_map_p->tenant_name_p = (char*)calloc(1,128);
     l2nw_service_map_p->vn_name_p = (char*)calloc(1,128);
     l2nw_service_map_p->nschainset_object_name_p = (char*)calloc(1,128);
     strcpy(l2nw_service_map_p->map_name_p , key_info_p->map_name_p);
     strcpy(l2nw_service_map_p->tenant_name_p , key_info_p->tenant_name_p);
     if(config_info_p[0].value.nschainset_object_name_p != NULL)
     {
       strcpy(l2nw_service_map_p->nschainset_object_name_p ,config_info_p[0].value.nschainset_object_name_p);
       l2nw_service_map_p->nschainset_object_saferef = nschainset_object_node_scan_p->nschainset_object_handle;
     }
     if(key_info_p->vn_name_p != NULL)
       strcpy(l2nw_service_map_p->vn_name_p ,key_info_p->vn_name_p);
     l2nw_service_map_p->admin_status_e = config_info_p[1].value.admin_status_e;
     l2nw_service_map_p->vn_saferef           = vn_handle ;
     l2nw_service_map_p->operational_status_e = OPER_DISABLE;
 
     hashobj_p = (uchar8_t *)l2nw_service_map_p + NSRM_L2NW_SERVICE_MAP_TBL_OFFSET;
     MCHASH_APPEND_NODE(nsrm_l2nw_service_map_table_g, hashkey, l2nw_service_map_p,index, magic, hashobj_p, status_b);
     if(FALSE == status_b)
     {
        OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR," Failed to append l2nw node to Hash table");
        status = NSRM_L2NW_SERVICE_MAP_ADD_FAIL;
        break;
     }
     l2nw_service_map_handle   = magic;
     l2nw_service_map_handle   = (((l2nw_service_map_handle)<<32) | (index));
     l2nw_service_map_p->magic = magic;
     l2nw_service_map_p->index = index;
     l2nw_service_map_p->l2nw_service_map_handle = l2nw_service_map_handle;
     apphookoffset = NSRM_NOTIFICATION_APP_HOOK_OFFSET;
     notification_data.add_del.l2nw_service_map_name_p = (char*)calloc(1,(strlen(l2nw_service_map_p->map_name_p)+1));
     strcpy(notification_data.add_del.l2nw_service_map_name_p ,l2nw_service_map_p->map_name_p);
     notification_data.add_del.l2nw_service_map_handle = l2nw_service_map_p->l2nw_service_map_handle;
     notification_data.add_del.vn_handle = vn_handle;
     notification_data.add_del.nschainset_object_handle = nschainset_object_handle;

     OF_LIST_SCAN(nsrm_l2nw_service_map_notifications[NSRM_L2NW_SERVICE_MAP_ADDED], 
                  app_entry_p,
                  struct nsrm_notification_app_hook_info *,
                  apphookoffset)
     {
        ((nsrm_l2nw_service_map_notifier)(app_entry_p->call_back))(NSRM_L2NW_SERVICE_MAP_ADDED,
                                                                   l2nw_service_map_handle,
                                                                   notification_data,
                                                                   app_entry_p->cbk_arg1,
                                                                   app_entry_p->cbk_arg2
                                                                   );
     }
   
     OF_LIST_SCAN(nsrm_l2nw_service_map_notifications[NSRM_L2NW_SERVICE_MAP_ALL] ,
                  app_entry_p,
                  struct nsrm_notification_app_hook_info *,
                  apphookoffset)
     {
        ((nsrm_l2nw_service_map_notifier)(app_entry_p->call_back))(NSRM_L2NW_SERVICE_MAP_ADDED,
                                                                   l2nw_service_map_handle,
                                                                   notification_data,
                                                                   app_entry_p->cbk_arg1,
                                                                   app_entry_p->cbk_arg2
                                                                   );
     }

     CNTLR_RCU_READ_LOCK_RELEASE();
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "l2nwservice map added successfully to the hashtable.");
     return NSRM_SUCCESS;
}while(0);

    if(status != NSRM_SUCCESS)
    {
      free(l2nw_service_map_p->map_name_p);
      free(l2nw_service_map_p->vn_name_p);
      free(l2nw_service_map_p->tenant_name_p);
      free(l2nw_service_map_p->nschainset_object_name_p);
      if(l2nw_service_map_p)
      {
        if(mempool_release_mem_block(nsrm_l2nw_service_map_mempool_g,(uchar8_t*)&l2nw_service_map_p,
                                     l2nw_service_map_p->heap_b) != MEMPOOL_SUCCESS)

        { 
           OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to release  allocated memory block.");
        }
     }
   }
   CNTLR_RCU_READ_LOCK_RELEASE();
   return status;
}

int32_t nsrm_del_l2nw_service_map_record(
        struct  nsrm_l2nw_service_map_key* key_info_p)
{
   struct nsrm_l2nw_service_map                    *l2nw_service_map_p = NULL;
   union  nsrm_l2nw_service_map_notification_data   notification_data = {};
   struct nsrm_notification_app_hook_info          *app_entry_p = NULL;
   struct crm_virtual_network                      *vn_info_p = NULL;

   uint32_t  apphookoffset,index = 0,magic = 0;
   int32_t   offset;
   uint8_t   status_b = FALSE;
   int32_t   ret_value;
   uint64_t  l2nw_service_map_handle ;

   if(nsrm_l2nw_service_map_table_g == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"nsrm_l2nw_service_map hash table doesnt exist");
      return NSRM_FAILURE;
   }

   if(key_info_p->map_name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"nsrm_l2nw_service_map name is invalid");
      return NSRM_ERROR_L2NW_SERVICE_MAP_NAME_INVALID;
   }

   ret_value = nsrm_get_l2nw_service_map_handle(key_info_p->map_name_p ,&l2nw_service_map_handle);
   if(ret_value != NSRM_SUCCESS)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"l2nw_service_map doesnt exist");
     return NSRM_ERROR_L2NW_SERVICE_MAP_NAME_INVALID;
   }
   magic = (uint32_t)(l2nw_service_map_handle >>32);
   index = (uint32_t)l2nw_service_map_handle;

   CNTLR_RCU_READ_LOCK_TAKE();
   MCHASH_ACCESS_NODE(nsrm_l2nw_service_map_table_g,index,magic,l2nw_service_map_p,status_b);
   if(FALSE == status_b)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"no node found");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   }
   if(strcmp(key_info_p->tenant_name_p , l2nw_service_map_p->tenant_name_p) != 0)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"l2nw doesnt exist in this tenant");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   }
   ret_value = crm_get_vn_byhandle(l2nw_service_map_p->vn_saferef , &vn_info_p);
   if(ret_value != NSRM_SUCCESS)
   {
       CNTLR_RCU_READ_LOCK_RELEASE();
       OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"VN doesnt exist");
       return NSRM_FAILURE;
   }

   apphookoffset = NSRM_NOTIFICATION_APP_HOOK_OFFSET;
   notification_data.add_del.l2nw_service_map_name_p = (char*)calloc(1,(strlen(l2nw_service_map_p->map_name_p)+1));
   strcpy(notification_data.add_del.l2nw_service_map_name_p ,l2nw_service_map_p->map_name_p);
   notification_data.add_del.l2nw_service_map_handle = l2nw_service_map_p->l2nw_service_map_handle;
   OF_LIST_SCAN(nsrm_l2nw_service_map_notifications[NSRM_L2NW_SERVICE_MAP_DELETED] ,
                  app_entry_p,
                  struct nsrm_notification_app_hook_info *,
                  apphookoffset)
   {
      ((nsrm_l2nw_service_map_notifier)(app_entry_p->call_back))(NSRM_L2NW_SERVICE_MAP_DELETED,
                                                                   l2nw_service_map_handle,
                                                                   notification_data,
                                                                   app_entry_p->cbk_arg1,
                                                                   app_entry_p->cbk_arg2
                                                                   );
   }

   OF_LIST_SCAN(nsrm_l2nw_service_map_notifications[NSRM_L2NW_SERVICE_MAP_ALL] ,
                  app_entry_p,
                  struct nsrm_notification_app_hook_info *,
                  apphookoffset)
   {
      ((nsrm_l2nw_service_map_notifier)(app_entry_p->call_back))(NSRM_L2NW_SERVICE_MAP_DELETED,
                                                                  l2nw_service_map_handle,
                                                                   notification_data,
                                                                   app_entry_p->cbk_arg1,
                                                                   app_entry_p->cbk_arg2
                                                                   );
   }

   offset = NSRM_L2NW_SERVICE_MAP_TBL_OFFSET;
   MCHASH_DELETE_NODE_BY_REF(nsrm_l2nw_service_map_table_g, index, magic, struct nsrm_l2nw_service_map *,
                            offset, status_b);
   CNTLR_RCU_READ_LOCK_RELEASE();
   if(status_b == TRUE)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "l2nw service deleted successfully");
      return NSRM_SUCCESS;
   }
   else
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "failed to delete l2nw service map");
      return NSRM_L2NW_SERVICE_MAP_DEL_FAIL;
   }
}

int32_t nsrm_modify_l2nw_service_map_record(
        struct  nsrm_l2nw_service_map_key* key_info_p,
        int32_t number_of_config_params,
        struct  nsrm_l2nw_service_map_config_info* config_info_p)
{

   struct nsrm_l2nw_service_map   *l2nw_service_map_node_scan_p = NULL;
   struct crm_virtual_network     *crm_vn_mod_info_p = NULL;
   struct nsrm_nschainset_object  *nschainset_object_node_scan_p = NULL;

   uint32_t  hashkey;
   int32_t   offset;
   uint8_t   current_entry_found_b = FALSE;
   int32_t   ret_value;
   uint64_t  vn_handle ,nschainset_object_handle;

   if(nsrm_l2nw_service_map_table_g == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "No l2nw_service_map hash table")
      return NSRM_FAILURE;
   }

   if(key_info_p->map_name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "l2nw_service_map name is null");
      return NSRM_ERROR_L2NW_SERVICE_MAP_NAME_INVALID;
   }
  
   if(number_of_config_params == 0)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"No parameters to be configured");
      return NSRM_FAILURE;
   } 
   /*Calculate hash value*/
   hashkey = nsrm_get_hashval_byname(key_info_p->map_name_p,nsrm_no_of_l2nw_service_map_buckets_g);
   offset = NSRM_L2NW_SERVICE_MAP_TBL_OFFSET;

   CNTLR_RCU_READ_LOCK_TAKE();
   MCHASH_BUCKET_SCAN(nsrm_l2nw_service_map_table_g ,hashkey ,l2nw_service_map_node_scan_p,
                         struct nsrm_l2nw_service_map* ,offset)
   {
     if((strcmp(key_info_p->map_name_p,l2nw_service_map_node_scan_p->map_name_p) == 0) && (strcmp(key_info_p->vn_name_p,l2nw_service_map_node_scan_p->vn_name_p) == 0) && (strcmp(key_info_p->tenant_name_p,l2nw_service_map_node_scan_p->tenant_name_p) == 0))
      {
          current_entry_found_b = TRUE;
      }
    }
    if(current_entry_found_b == FALSE)
    {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"node not found to modify");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_ERROR_L2NW_SERVICE_MAP_NAME_INVALID;
    }
               
    l2nw_service_map_node_scan_p->map_name_p = (char*)calloc(1,(strlen(key_info_p->map_name_p)+1));
    l2nw_service_map_node_scan_p->tenant_name_p = (char*)calloc(1,(strlen(key_info_p->tenant_name_p)+1));
    l2nw_service_map_node_scan_p->vn_name_p = (char*)calloc(1,(strlen(key_info_p->vn_name_p)+1));
    l2nw_service_map_node_scan_p->nschainset_object_name_p = (char*)calloc(1,strlen(config_info_p[0].value.nschainset_object_name_p)+1);
    strcpy(l2nw_service_map_node_scan_p->map_name_p , key_info_p->map_name_p);
    strcpy(l2nw_service_map_node_scan_p->tenant_name_p , key_info_p->tenant_name_p);
    strcpy(l2nw_service_map_node_scan_p->nschainset_object_name_p ,config_info_p[0].value.nschainset_object_name_p);
    strcpy(l2nw_service_map_node_scan_p->vn_name_p ,key_info_p->vn_name_p);
    l2nw_service_map_node_scan_p->admin_status_e = config_info_p[1].value.admin_status_e;

    ret_value = nsrm_get_nschainset_object_handle(config_info_p[0].value.nschainset_object_name_p,&nschainset_object_handle);
    if(ret_value != NSRM_SUCCESS)
    {
       OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Nschain set doesn't exist ");
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_FAILURE;
    }
    ret_value = nsrm_get_nschainset_byhandle(nschainset_object_handle ,&nschainset_object_node_scan_p);
    if(ret_value != NSRM_SUCCESS)
    {
       OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Nschain set doesn't exist ");
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_FAILURE;
    }

     /*Put new nschainset objects saferef */ 
    l2nw_service_map_node_scan_p->nschainset_object_saferef = nschainset_object_node_scan_p->nschainset_object_handle;

    ret_value = crm_get_vn_handle(key_info_p->vn_name_p,&vn_handle);
    if(ret_value != NSRM_SUCCESS)
    {
       OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "VN doesn't exist ");
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_FAILURE;
    }

    ret_value = crm_get_vn_byhandle(vn_handle,&crm_vn_mod_info_p);
    if(ret_value != NSRM_SUCCESS)
    {
       OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "VN doesn't exist ");
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_FAILURE;
    }
 
     l2nw_service_map_node_scan_p->vn_saferef           = vn_handle ;
     l2nw_service_map_node_scan_p->operational_status_e = OPER_DISABLE;
       
     CNTLR_RCU_READ_LOCK_RELEASE();
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "l2nwservice map modified successfully .");
     return NSRM_SUCCESS;
} 

int32_t nsrm_get_first_l2nw_service_maps(
        int32_t  number_of_l2_service_maps_requested,
        int32_t* number_of_l2_service_maps_returned_p,
        struct   nsrm_l2nw_service_map_record* recs_p)
{  
   struct    nsrm_l2nw_service_map         *l2nw_service_map_node_scan_p = NULL;
   uint64_t  hashkey;
   int32_t   offset,l2nw_map_returned = 0;
   int32_t   l2nw_map_requested = number_of_l2_service_maps_requested;
   int32_t   status = NSRM_FAILURE;

   if(nsrm_l2nw_service_map_table_g == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"No hash table exists");
      return NSRM_FAILURE;
   }

   CNTLR_RCU_READ_LOCK_TAKE();
   offset = NSRM_L2NW_SERVICE_MAP_TBL_OFFSET;
   MCHASH_TABLE_SCAN(nsrm_l2nw_service_map_table_g , hashkey)
   {
      MCHASH_BUCKET_SCAN(nsrm_l2nw_service_map_table_g, hashkey, l2nw_service_map_node_scan_p,
                           struct nsrm_l2nw_service_map*,offset)
      {
         strcpy(recs_p[l2nw_map_returned].key.map_name_p,l2nw_service_map_node_scan_p->map_name_p);
         strcpy(recs_p[l2nw_map_returned].key.tenant_name_p,l2nw_service_map_node_scan_p->tenant_name_p);
         strcpy(recs_p[l2nw_map_returned].key.vn_name_p , l2nw_service_map_node_scan_p->vn_name_p);
         strcpy(recs_p[l2nw_map_returned].info[0].value.nschainset_object_name_p , l2nw_service_map_node_scan_p->nschainset_object_name_p);
         recs_p[l2nw_map_returned].info[1].value.admin_status_e = l2nw_service_map_node_scan_p->admin_status_e;
         status = NSRM_SUCCESS;
         number_of_l2_service_maps_requested--;
         l2nw_map_returned++;

         if(number_of_l2_service_maps_requested)
           continue;
         else
           break;
      }
      if(number_of_l2_service_maps_requested)
        continue;
      else
        break;
   }
   if(l2nw_map_requested != l2nw_map_returned)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Invalid number of objects returned / requested ");
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_FAILURE;
   }
   if(status == NSRM_SUCCESS)
   {
      *number_of_l2_service_maps_returned_p = l2nw_map_returned;
       CNTLR_RCU_READ_LOCK_RELEASE();
       return status;
   }
   CNTLR_RCU_READ_LOCK_RELEASE();
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "No More Entries in the l2nwservice map hash table!.");
   return NSRM_ERROR_NO_MORE_ENTRIES;

}

int32_t nsrm_get_next_l2nw_service_maps(
        struct   nsrm_l2nw_service_map_key* relative_record_key_p,
        int32_t  number_of_l2_service_maps_requested,
        int32_t* number_of_l2_service_maps_returned_p,
        struct   nsrm_l2nw_service_map_record* recs_p)
{
   struct    nsrm_l2nw_service_map         *l2nw_service_map_node_scan_p = NULL;
  
   uint64_t  hashkey;
   int32_t   offset,l2nw_map_returned = 0;
   int32_t   l2nw_map_requested = number_of_l2_service_maps_requested;
   int32_t   status = NSRM_FAILURE;
   uint8_t   current_entry_found_b = FALSE;

   if(nsrm_l2nw_service_map_table_g == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"No hash table exists");
      return NSRM_FAILURE;
   }

   hashkey = nsrm_get_hashval_byname(relative_record_key_p->map_name_p,nsrm_no_of_l2nw_service_map_buckets_g);
   CNTLR_RCU_READ_LOCK_TAKE();
   offset = NSRM_L2NW_SERVICE_MAP_TBL_OFFSET;
   MCHASH_TABLE_SCAN(nsrm_l2nw_service_map_table_g , hashkey)
   {
      MCHASH_BUCKET_SCAN(nsrm_l2nw_service_map_table_g,hashkey,l2nw_service_map_node_scan_p,
                         struct nsrm_l2nw_service_map*, offset)
      {
         if(current_entry_found_b == FALSE)
         {
            if(((strcmp(relative_record_key_p->map_name_p , l2nw_service_map_node_scan_p->map_name_p)) != 0) ||
        ((strcmp(relative_record_key_p->tenant_name_p,l2nw_service_map_node_scan_p->tenant_name_p) !=0)))
                 continue;
            else
            {
               OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "l2nw service map entry found copy next ");                        current_entry_found_b = TRUE;
               continue;
            }
         }
         strcpy(recs_p[l2nw_map_returned].key.map_name_p , l2nw_service_map_node_scan_p->map_name_p);
         strcpy(recs_p[l2nw_map_returned].key.tenant_name_p , l2nw_service_map_node_scan_p->tenant_name_p);
         strcpy(recs_p[l2nw_map_returned].key.vn_name_p , l2nw_service_map_node_scan_p->vn_name_p);
         strcpy(recs_p[l2nw_map_returned].info[0].value.nschainset_object_name_p , l2nw_service_map_node_scan_p->nschainset_object_name_p);
         recs_p[l2nw_map_returned].info[1].value.admin_status_e = l2nw_service_map_node_scan_p->admin_status_e;
         status = NSRM_SUCCESS;
         number_of_l2_service_maps_requested--;
         l2nw_map_returned++;

         if(number_of_l2_service_maps_requested)
           continue;
         else
           break;
      }
       if(number_of_l2_service_maps_requested)
          continue;
      else
        break;
   }
   if(l2nw_map_requested != l2nw_map_returned)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Invalid number of objects returned / requested ");
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_FAILURE;
   }
   if(status == NSRM_SUCCESS)
   {
      *number_of_l2_service_maps_returned_p = l2nw_map_returned;
       CNTLR_RCU_READ_LOCK_RELEASE();
       return status;
   }
   CNTLR_RCU_READ_LOCK_RELEASE();
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "No More Entries in the l2nwservice map hash table!.");
   return NSRM_ERROR_NO_MORE_ENTRIES;

}
 
int32_t nsrm_get_exact_l2nw_service_map(
        struct  nsrm_l2nw_service_map_key* key_info_p,
        struct  nsrm_l2nw_service_map_record* rec_p)
{
   struct    nsrm_l2nw_service_map         *l2nw_service_map_node_scan_p = NULL;
  
   uint64_t  hashkey;
   int32_t   offset;
   uint8_t   current_entry_found_b = FALSE;

   if(nsrm_l2nw_service_map_table_g == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"No hash table exists");
      return NSRM_FAILURE;
   }

   CNTLR_RCU_READ_LOCK_TAKE();
   offset = NSRM_L2NW_SERVICE_MAP_TBL_OFFSET;
   hashkey = nsrm_get_hashval_byname(key_info_p->map_name_p,nsrm_no_of_l2nw_service_map_buckets_g);
   MCHASH_BUCKET_SCAN(nsrm_l2nw_service_map_table_g,hashkey,l2nw_service_map_node_scan_p,
                      struct nsrm_l2nw_service_map*, offset)
   {
     if(current_entry_found_b == FALSE)
     {
        if(((strcmp(key_info_p->map_name_p , l2nw_service_map_node_scan_p->map_name_p)) == 0) &&
           (strcmp(key_info_p->tenant_name_p , l2nw_service_map_node_scan_p->tenant_name_p) == 0) &&
            (strcmp(key_info_p->vn_name_p , l2nw_service_map_node_scan_p->vn_name_p) == 0)) 
        {
           OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "l2nw service map entry found ");
           current_entry_found_b = TRUE;
           break;
        }
       
     }
   }
   if(current_entry_found_b == FALSE)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"No node matched");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   }
    strcpy(rec_p[0].key.map_name_p , l2nw_service_map_node_scan_p->map_name_p);
    strcpy(rec_p[0].key.tenant_name_p , l2nw_service_map_node_scan_p->tenant_name_p);
    strcpy(rec_p[0].key.vn_name_p , l2nw_service_map_node_scan_p->vn_name_p);
    strcpy(rec_p[0].info[0].value.nschainset_object_name_p , l2nw_service_map_node_scan_p->nschainset_object_name_p);
    rec_p[0].info[1].value.admin_status_e = l2nw_service_map_node_scan_p->admin_status_e;

    CNTLR_RCU_READ_LOCK_RELEASE();
    OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Found exact entry!.");
    return NSRM_SUCCESS;
}

int32_t nsrm_register_l2nw_service_map_notifications(
                          uint8_t notification_type,
                          nsrm_l2nw_service_map_notifier  l2nw_notifier,
                          void* callback_arg1,
                          void* callback_arg2)
{
   struct  nsrm_notification_app_hook_info *app_entry_p=NULL;
   uint8_t heap_b;
   int32_t retval = NSRM_SUCCESS;
   uchar8_t lstoffset;
   lstoffset = NSRM_NOTIFICATION_APP_HOOK_OFFSET;
   
   if((notification_type < NSRM_L2NW_SERVICE_MAP_FIRST_NOTIFICATION_TYPE) ||
     (notification_type > NSRM_L2NW_SERVICE_MAP_LAST_NOTIFICATION_TYPE))
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Invalid Notification type:%d", notification_type);
      return NSRM_FAILURE;
   }
   if(l2nw_notifier == NULL)
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

   app_entry_p->call_back = (void*)l2nw_notifier;
   app_entry_p->cbk_arg1  = callback_arg1;
   app_entry_p->cbk_arg2  = callback_arg2;
   app_entry_p->heap_b    = heap_b;

   /* Add Application to the list of notifications */
   OF_APPEND_NODE_TO_LIST(nsrm_l2nw_service_map_notifications[notification_type],app_entry_p, lstoffset);
   if(retval != NSRM_SUCCESS)
     mempool_release_mem_block(nsrm_notifications_mempool_g,(uchar8_t*)app_entry_p, app_entry_p->heap_b);

   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "callback function reg with l2nwservice map success");
   return retval;
}

int32_t nsrm_add_attribute_to_l2nw_service_map(
          char*     l2nw_service_map_name_p,
          struct    nsrm_attribute_name_value_pair *attribute_info_p)
{
  struct   nsrm_l2nw_service_map                         *l2map_entry_p;
  union    nsrm_l2nw_service_map_notification_data        notification_data = {};
  struct   nsrm_notification_app_hook_info               *app_entry_p;
  int32_t  apphookoffset ,ret_val;
  uint64_t l2map_handle;

  apphookoffset = NSRM_NOTIFICATION_APP_HOOK_OFFSET;

  CNTLR_RCU_READ_LOCK_TAKE();
  ret_val = nsrm_get_l2nw_service_map_handle(l2nw_service_map_name_p,&l2map_handle);
  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return ret_val;
  }
  ret_val = nsrm_get_l2_smap_record_byhandle(l2map_handle , &l2map_entry_p);
  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return ret_val;
  }

  ret_val = nsrm_add_attribute(&(l2map_entry_p->attributes),attribute_info_p);
  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return ret_val;
  }
  OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "added attr");
  l2map_entry_p->number_of_attributes++;
  nsrm_create_l2nw_map_view_entry(l2map_handle, attribute_info_p->attribute_name_p, attribute_info_p->attribute_value_p);
  notification_data.attribute.l2nw_service_map_name_p = (char *)calloc(1,strlen(l2map_entry_p->map_name_p)+1);
  notification_data.attribute.l2nw_service_map_attribute_name_p = (char *)calloc(1,strlen(attribute_info_p->attribute_name_p)+1);
  notification_data.attribute.l2nw_service_map_attribute_value_p = (char *)calloc(1,strlen(attribute_info_p->attribute_value_p)+1);
  strcpy(notification_data.attribute.l2nw_service_map_name_p, l2map_entry_p->map_name_p);
  strcpy(notification_data.attribute.l2nw_service_map_attribute_name_p,attribute_info_p->attribute_name_p);
  strcpy(notification_data.attribute.l2nw_service_map_attribute_value_p,attribute_info_p->attribute_value_p);
  notification_data.attribute.l2nw_service_map_handle = l2map_handle;

  OF_LIST_SCAN(nsrm_l2nw_service_map_notifications[NSRM_L2NW_SERVICE_MAP_ATTRIBUTE_ADDED],
               app_entry_p,
               struct nsrm_notification_app_hook_info *,
               apphookoffset)
  {
     ((nsrm_l2nw_service_map_notifier)(app_entry_p->call_back))(NSRM_L2NW_SERVICE_MAP_ATTRIBUTE_ADDED,
                                                                l2map_handle,
                                                                notification_data,
                                                                app_entry_p->cbk_arg1,
                                                                app_entry_p->cbk_arg2
                                                                   );
  }

   OF_LIST_SCAN(nsrm_l2nw_service_map_notifications[NSRM_L2NW_SERVICE_MAP_ALL] ,
                app_entry_p,
                struct nsrm_notification_app_hook_info *,
                apphookoffset)
   {
        ((nsrm_l2nw_service_map_notifier)(app_entry_p->call_back))(NSRM_L2NW_SERVICE_MAP_ATTRIBUTE_ADDED,
                                                                   l2map_handle,
                                                                   notification_data,
                                                                   app_entry_p->cbk_arg1,
                                                                   app_entry_p->cbk_arg2
                                                                   );
   }

  OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "success");
  CNTLR_RCU_READ_LOCK_RELEASE();
  return NSRM_SUCCESS;
}
 
int32_t nsrm_del_attribute_from_l2nw_service_map (
                                                  char*    l2nw_service_map_name_p,
                                                  char*    attribute_name_p)
{
  struct   nsrm_l2nw_service_map                         *l2map_entry_p;
  union    nsrm_l2nw_service_map_notification_data        notification_data = {};
  struct   nsrm_notification_app_hook_info               *app_entry_p;
  struct   nsrm_resource_attribute_entry *attribute_entry_p,*prev_attr_entry_p = NULL;
  int32_t  apphookoffset ,ret_val;
  uint64_t l2map_handle;
  uint8_t  attr_node_found = 0;

  apphookoffset = NSRM_NOTIFICATION_APP_HOOK_OFFSET;

  OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Entered");
  CNTLR_RCU_READ_LOCK_TAKE();

  ret_val = nsrm_get_l2nw_service_map_handle(l2nw_service_map_name_p,&l2map_handle);
  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return ret_val;
  }
  ret_val = nsrm_get_l2_smap_record_byhandle(l2map_handle , &l2map_entry_p);
  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return ret_val;
  }

  OF_LIST_SCAN(l2map_entry_p->attributes, attribute_entry_p, struct nsrm_resource_attribute_entry*,apphookoffset)
  {
    OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "attribute_entry_p->name:%s",attribute_entry_p->name);
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
  
  notification_data.attribute.l2nw_service_map_name_p = (char *)calloc(1,strlen(l2map_entry_p->map_name_p)+1);
  notification_data.attribute.l2nw_service_map_attribute_name_p = (char *)calloc(1,strlen(attribute_name_p)+1);
  //notification_data.attribute.l2nw_service_map_attribute_value_p = (char *)calloc(1,strlen(attribute_info_p->attribute_value_p)+1);
  strcpy(notification_data.attribute.l2nw_service_map_name_p, l2map_entry_p->map_name_p);
  strcpy(notification_data.attribute.l2nw_service_map_attribute_name_p,attribute_name_p);
 // strcpy(notification_data.attribute.l2nw_service_map_attribute_value_p,attribute_info_p->attribute_value_p);
  notification_data.attribute.l2nw_service_map_handle = l2map_handle;

  OF_LIST_SCAN(nsrm_l2nw_service_map_notifications[NSRM_L2NW_SERVICE_MAP_ATTRIBUTE_DELETED],
               app_entry_p,
               struct nsrm_notification_app_hook_info *,
               apphookoffset)
  {
     ((nsrm_l2nw_service_map_notifier)(app_entry_p->call_back))(NSRM_L2NW_SERVICE_MAP_ATTRIBUTE_DELETED,
                                                                l2map_handle,
                                                                notification_data,
                                                                app_entry_p->cbk_arg1,
                                                                app_entry_p->cbk_arg2
                                                                 );
  }

   OF_LIST_SCAN(nsrm_l2nw_service_map_notifications[NSRM_L2NW_SERVICE_MAP_ALL] ,
                app_entry_p,
                struct nsrm_notification_app_hook_info *,
                apphookoffset)
   {
        ((nsrm_l2nw_service_map_notifier)(app_entry_p->call_back))(NSRM_L2NW_SERVICE_MAP_ATTRIBUTE_ADDED,
                                                                   l2map_handle,
                                                                   notification_data,
                                                                   app_entry_p->cbk_arg1,
                                                                   app_entry_p->cbk_arg2
                                                                   );
   }

 
   OF_REMOVE_NODE_FROM_LIST(l2map_entry_p->attributes,attribute_entry_p,prev_attr_entry_p,apphookoffset);
   l2map_entry_p->number_of_attributes--;
   nsrm_remove_l2nw_map_view_entry(attribute_entry_p->name, attribute_entry_p->value);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return NSRM_SUCCESS;
}

int32_t nsrm_get_first_l2nw_service_map_attribute(
          char*  l2nw_service_map_name_p,
          struct nsrm_attribute_name_value_output_info *attribute_output_p)
{
  struct   nsrm_l2nw_service_map       *l2map_entry_p;
  uint64_t l2map_handle;
  int32_t  ret_val = NSRM_SUCCESS;
  
  CNTLR_RCU_READ_LOCK_TAKE();

  do
  {
    ret_val = nsrm_get_l2nw_service_map_handle(l2nw_service_map_name_p,&l2map_handle);
    if(ret_val != NSRM_SUCCESS)
      break;
  
    ret_val = nsrm_get_l2_smap_record_byhandle(l2map_handle , &l2map_entry_p);
    if(ret_val != NSRM_SUCCESS)
      break;
   
    ret_val = nsrm_get_first_attribute(&(l2map_entry_p->attributes), attribute_output_p);
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


int32_t nsrm_get_next_l2nw_service_map_attribute(
          char*  l2nw_service_map_name_p,
          char*  current_attribute_name_p,
          struct nsrm_attribute_name_value_output_info *attribute_output_p)
{
  struct   nsrm_l2nw_service_map       *l2map_entry_p;
  uint64_t l2map_handle;
  int32_t  ret_val = NSRM_SUCCESS;
  
  CNTLR_RCU_READ_LOCK_TAKE();

  do
  {
    ret_val = nsrm_get_l2nw_service_map_handle(l2nw_service_map_name_p,&l2map_handle);
    if(ret_val != NSRM_SUCCESS)
      break;
  
    ret_val = nsrm_get_l2_smap_record_byhandle(l2map_handle , &l2map_entry_p);
    if(ret_val != NSRM_SUCCESS)
      break;
 
    ret_val = nsrm_get_next_attribute(&(l2map_entry_p->attributes),current_attribute_name_p,
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

int32_t nsrm_get_exact_l2nw_service_map_attribute(
            char*  l2nw_service_map_name_p,
            char*   attribute_name_p,
            struct  nsrm_attribute_name_value_output_info *attribute_output_p
          )
{
  struct   nsrm_l2nw_service_map       *l2map_entry_p;
  uint64_t l2map_handle;
  int32_t ret_val = NSRM_SUCCESS;
  
  CNTLR_RCU_READ_LOCK_TAKE();

  do
  {
    ret_val = nsrm_get_l2nw_service_map_handle(l2nw_service_map_name_p,&l2map_handle);
    if(ret_val != NSRM_SUCCESS)
      break;
    ret_val = nsrm_get_l2_smap_record_byhandle(l2map_handle , &l2map_entry_p);
    if(ret_val != NSRM_SUCCESS)
      break;
    ret_val = nsrm_get_exact_attribute(&(l2map_entry_p->attributes),attribute_name_p,attribute_output_p);
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
int32_t nsrm_create_l2nw_map_view(char *view_name)
{
  int32_t index = 0 , ii;
  uint8_t empty_index_b = 0;

  for(ii = 0;ii<NSRM_MAX_VIEW_DATABASE ; ii++)
  {
     if(!strcmp(l2nw_map_db_view[ii].view , view_name))
     {
        OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"View already created");
        return NSRM_FAILURE;
     } 
     else if((empty_index_b == 0) && (!strcmp(l2nw_map_db_view[ii].view ,"")))    
     {
        empty_index_b = 1;
        index = ii;
     }
  } 
  strcpy(l2nw_map_db_view[index].view,view_name);
  nsrm_view_hash_table_init(&l2nw_map_db_view[index].db_viewtbl_p);
  l2nw_map_db_view[index].uniqueness_flag = FALSE; 
  l2nw_map_db_view[index].valid_b = TRUE;
 
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "View node created succesfully");
  return NSRM_SUCCESS;
}
  

int32_t nsrm_create_l2nw_map_view_entry(uint64_t l2nw_map_handle , char* view_name ,char* view_value)
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
       if(!strcmp(l2nw_map_db_view[view_index].view ,view_name))
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
     
     hashkey = nsrm_get_hashval_byname(view_value,nsrm_no_of_l2nw_service_map_buckets_g);
     CNTLR_RCU_READ_LOCK_TAKE();
     view_node_entry_p = nsrm_alloc_view_entry_and_set_values(l2nw_map_handle, view_name, view_value);
     if (view_node_entry_p == NULL)
     {
       status = NSRM_FAILURE;
       break;
     }
     MCHASH_APPEND_NODE(l2nw_map_db_view[view_index].db_viewtbl_p,hashkey,view_node_entry_p,index,magic,view_node_entry_p,status_b);
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

 
int32_t nsrm_remove_l2nw_map_view_entry(char* view_name,char* view_value)
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
       if(!strcmp(l2nw_map_db_view[index].view,view_name))
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
     hashkey = nsrm_get_hashval_byname(view_value,nsrm_no_of_l2nw_service_map_buckets_g);
     CNTLR_RCU_READ_LOCK_TAKE(); 
     MCHASH_BUCKET_SCAN(l2nw_map_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
     {
      if ((!strcmp(view_node_entry_p->view_value, view_value)) &&
          (!strcmp(view_node_entry_p->node_name, view_name)))
      {
        status_b = FALSE;
        offset = 0;
        MCHASH_DELETE_NODE_BY_REF(l2nw_map_db_view[index].db_viewtbl_p,view_node_entry_p->index,view_node_entry_p->magic,struct database_view_node *,offset,status_b);
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
 
int32_t nsrm_remove_l2nw_map_view(char *view_name)
{
  int32_t  index , ret_val;
  uint8_t  node_found_b = 0 ;
  uint64_t hashkey ,offset ;
  struct   database_view_node *view_node_entry_p;

  for(index = 0;index<NSRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(l2nw_map_db_view[index].view , view_name))
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
  CNTLR_RCU_READ_LOCK_TAKE(); 
  MCHASH_TABLE_SCAN(l2nw_map_db_view[index].db_viewtbl_p,hashkey)
  {
    MCHASH_BUCKET_SCAN(l2nw_map_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
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
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_FAILURE;
  }
  ret_val = nsrm_view_hash_table_deinit(l2nw_map_db_view[index].db_viewtbl_p);
  if(ret_val != NSRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_ERROR,"Hash Table deinit failed");
    CNTLR_RCU_READ_LOCK_RELEASE(); 
    return NSRM_FAILURE;
  }

  strcpy(l2nw_map_db_view[index].view,"");
  l2nw_map_db_view[index].uniqueness_flag = FALSE; 
  l2nw_map_db_view[index].valid_b = FALSE;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "View node deleted succesfully");
  CNTLR_RCU_READ_LOCK_RELEASE();
  return NSRM_SUCCESS;
}


int32_t nsrm_get_first_l2nw_map_view_entry(char *view_name , struct database_view_node *vn_view_node)
{
  int32_t  index;
  uint8_t  node_found_b = 0 ;
  int32_t  hashkey ,offset ;
  struct   database_view_node *view_node_entry_p;

  for(index = 0;index<NSRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(l2nw_map_db_view[index].view , view_name))
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
  MCHASH_TABLE_SCAN(l2nw_map_db_view[index].db_viewtbl_p ,hashkey)
  {
    MCHASH_BUCKET_SCAN(l2nw_map_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
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
   
int32_t nsrm_get_next_l2nw_map_view_entry(char *view_name , char *view_value ,struct database_view_node *vn_view_node)

{
  int32_t  index;
  uint8_t  node_found_b = 0 ;
  int32_t  hashkey ,offset ;
  struct   database_view_node *view_node_entry_p;

  for(index = 0;index<NSRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(l2nw_map_db_view[index].view , view_name))
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
  MCHASH_TABLE_SCAN(l2nw_map_db_view[index].db_viewtbl_p , hashkey)
  {
    MCHASH_BUCKET_SCAN(l2nw_map_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
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

int32_t nsrm_get_exact_l2nw_map_view_entry(char *view_name , char *view_value ,struct database_view_node *vn_view_node)

{
  int32_t  index;
  uint8_t  node_found_b = 0 ;
  int32_t  hashkey ,offset ;
  struct   database_view_node *view_node_entry_p;

  for(index = 0;index<NSRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(l2nw_map_db_view[index].view , view_name))
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
  MCHASH_TABLE_SCAN(l2nw_map_db_view[index].db_viewtbl_p ,hashkey)
  {
    MCHASH_BUCKET_SCAN(l2nw_map_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
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
int32_t nsrm_get_l2nw_map_by_view(char *view_name , char *view_value ,struct nsrm_l2nw_service_map_record* recs_p)
{
  int32_t  index;
  uint8_t  node_found_b = 0 ;
  int32_t  hashkey ,offset ,ret_val;
  struct   database_view_node      *view_node_entry_p;
  struct   nsrm_l2nw_service_map   *l2nw_service_map_p = NULL;

  for(index = 0;index<NSRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(l2nw_map_db_view[index].view , view_name))
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
  MCHASH_TABLE_SCAN(l2nw_map_db_view[index].db_viewtbl_p ,hashkey)
  {
    MCHASH_BUCKET_SCAN(l2nw_map_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
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
  ret_val = nsrm_get_l2_smap_record_byhandle(view_node_entry_p->node_saferef , &l2nw_service_map_p);
  if(ret_val != NSRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Node not found");
    CNTLR_RCU_READ_LOCK_RELEASE();
    return NSRM_FAILURE;
  }
  strcpy(recs_p->key.map_name_p,l2nw_service_map_p->map_name_p);
  strcpy(recs_p->key.tenant_name_p,l2nw_service_map_p->tenant_name_p);
  strcpy(recs_p->key.vn_name_p , l2nw_service_map_p->vn_name_p);
  strcpy(recs_p->info[0].value.nschainset_object_name_p , l2nw_service_map_p->nschainset_object_name_p);
  recs_p->info[1].value.admin_status_e = l2nw_service_map_p->admin_status_e;

  CNTLR_RCU_READ_LOCK_RELEASE();
  OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"nschainset found by view");
  return NSRM_SUCCESS;
}

 






