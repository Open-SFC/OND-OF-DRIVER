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
 * File name:nsrm_srule.c
 * Author: P.Vinod Sarma <B46178@freescale.com>
 * Date:   25/01/2014
 * Description: 
*/

#include  "nsrm_system_dependencies.h"
#include  "crm_tenant_api.h"

#define RCUNODE_OFFSET_IN_MCHASH_TBLLNK offsetof(struct mchash_table_link, rcu_head)

extern void     *nsrm_selection_rule_mempool_g;
extern uint32_t nsrm_no_of_selection_rule_buckets_g;
extern void     *nsrm_notifications_mempool_g;

extern of_list_t nsrm_selection_rule_notifications[];
struct  database_view selection_rule_db_view[NSRM_MAX_VIEW_DATABASE];

void nsrm_free_selection_rule_entry_rcu(struct rcu_head *selection_rule_entry_p)
{
   struct   nsrm_nschain_selection_rule *app_info_p=NULL;
   uint32_t offset;
   int32_t ret_val;
   if(selection_rule_entry_p)
   {
     offset = NSRM_SELECTION_RULE_TBL_OFFSET;
     app_info_p = (struct nsrm_nschain_selection_rule *)(((char *)selection_rule_entry_p - RCUNODE_OFFSET_IN_MCHASH_TBLLNK) - offset);

     ret_val = mempool_release_mem_block(nsrm_selection_rule_mempool_g,
                                        (uchar8_t*)app_info_p,app_info_p->heap_b);
  if(ret_val != MEMPOOL_SUCCESS)
  {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to release selection rule memory block.");
  }
  else
  {
     OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Released selection rule memory block from the memory pool.");
  }
 }
  else
  {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Null entry to release",__FUNCTION__,__LINE__);
  }
}

int32_t nsrm_get_selection_rule_handle(char *rule_name_p, struct nsrm_nschainset_object *nschainset_object_p,uint64_t *selection_rule_handle_p)
{
   uint32_t hashkey;
   int32_t offset;
   struct nsrm_nschain_selection_rule* selection_rule_entry_p = NULL;
   if(nschainset_object_p->nschain_selection_rule_set == NULL)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "There is no hash table!.");
     return NSRM_FAILURE;
   }
   hashkey = 0;
   CNTLR_RCU_READ_LOCK_TAKE();
   offset = NSRM_SELECTION_RULE_TBL_OFFSET;

   MCHASH_BUCKET_SCAN(nschainset_object_p->nschain_selection_rule_set, hashkey, selection_rule_entry_p,struct nsrm_nschain_selection_rule*,offset)
   {
     if(strcmp(selection_rule_entry_p->name_p, rule_name_p))
     {
       continue;
     }
     *selection_rule_handle_p = selection_rule_entry_p->magic;
     *selection_rule_handle_p = ((*selection_rule_handle_p<<32) | (selection_rule_entry_p->index));
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_SUCCESS;
  }
    CNTLR_RCU_READ_LOCK_RELEASE();
    OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nschain object  name not found!.");
    return NSRM_ERROR_INVALID_SELECTION_RULE_NAME;
}
/***************************************************************************************************/
int32_t nsrm_get_selection_rule_byhandle(struct nsrm_nschainset_object *nschainset_object_p ,
                                         uint64_t selection_rule_handle ,
                                         struct nsrm_nschain_selection_rule  **selection_rule_p_p)
{
/* Caller shall take RCU locks. */
  uint32_t index,magic;
  uint8_t status_b;

  magic = (uint32_t)(selection_rule_handle >>32);
  index = (uint32_t)selection_rule_handle;

  MCHASH_ACCESS_NODE(nschainset_object_p->nschain_selection_rule_set,index,magic,*selection_rule_p_p,status_b);
  if(TRUE == status_b)
    return NSRM_SUCCESS;
  return NSRM_FAILURE;
}
/***************************************************************************************************/
int32_t nsrm_add_nschain_selection_rule_to_nschainset(
        struct  nsrm_nschain_selection_rule_key*           key_info_p,
        int32_t number_of_config_params,
        struct  nsrm_nschain_selection_rule_config_info*   config_info_p)
{
   struct nsrm_nschain_selection_rule  *selection_rule_p = NULL;
   struct nsrm_nschain_selection_rule  *selection_rule_scan_node_p = NULL;
   struct nsrm_nschainset_object       *nschainset_object_node_scan_p = NULL;
   struct nsrm_nschain_object          *nschain_object_node_scan_p = NULL;
   union  nsrm_nschain_selection_rule_notification_data  notification_data = {};
   struct nsrm_notification_app_hook_info *app_entry_p = NULL;

   uchar8_t* hashobj_p;
   uint64_t  hashkey ,offset,apphookoffset ;
   uint64_t  tenant_handle,nschainset_object_handle,nschain_object_handle,selection_rule_handle = 0;
   int32_t   ret_value , index =0 , magic =0 ;
   uint8_t   heap_b , status_b = FALSE , relative_node_found = FALSE , status = NSRM_SUCCESS;

   if(key_info_p->name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "selection rule name is null");
      return NSRM_ERROR_NWSERVICE_OBJECT_NAME_INVALID;
   }

   if(key_info_p->nschainset_object_name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "nschainset object name is null");
      return NSRM_ERROR_NSCHAINSET_OBJECT_NAME_INVALID;
   }

   if(key_info_p->tenant_name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "tenant name is null");
      return NSRM_FAILURE;
   }
   
   ret_value = crm_get_tenant_handle(key_info_p->tenant_name_p,&tenant_handle);
   if(ret_value != NSRM_SUCCESS)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"tenant doesnt exist");
     return NSRM_FAILURE;
   }
   
   ret_value = nsrm_get_nschainset_object_handle(key_info_p->nschainset_object_name_p , &nschainset_object_handle);   
   if(ret_value != NSRM_SUCCESS)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"nschainset object doesnt exist");
     return NSRM_ERROR_NSCHAINSET_OBJECT_NAME_INVALID;
   }

   CNTLR_RCU_READ_LOCK_TAKE();
   
   ret_value = nsrm_get_nschainset_byhandle(nschainset_object_handle , &nschainset_object_node_scan_p);
   if(ret_value != NSRM_SUCCESS)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"failed to get nschainset object");
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_ERROR_NSCHAINSET_OBJECT_NAME_INVALID;
   }
  
   if(config_info_p[8].value.nschain_object_name_p != NULL) 
   {
     ret_value = nsrm_get_nschain_object_handle(config_info_p[8].value.nschain_object_name_p , &nschain_object_handle);   
     if(ret_value != NSRM_SUCCESS)
     {
       OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"nschain object doesnt exist");
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
     }

     ret_value = nsrm_get_nschain_byhandle(nschain_object_handle , &nschain_object_node_scan_p);
     if(ret_value != NSRM_SUCCESS)
     {
       OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"failed to get nschain object");
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
     }
   }
   
   if(nschainset_object_node_scan_p->nschain_selection_rule_set == NULL )
   {
     nschainset_object_node_scan_p->nschain_selection_rule_set = (struct mchash_table*)calloc(1,(NSRM_MAX_SELECTION_RULE_ENTRIES)*sizeof(struct nsrm_nschain_selection_rule));
     ret_value = mchash_table_create(1 ,
                                      NSRM_MAX_SELECTION_RULE_ENTRIES,
                                      nsrm_free_selection_rule_entry_rcu,
                                      &(nschainset_object_node_scan_p->nschain_selection_rule_set));
     if(ret_value != NSRM_SUCCESS)
     {
       OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"table not created");
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_FAILURE;
     }
   }
    
   if(nschainset_object_node_scan_p->nschain_selection_rule_set == NULL)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"Mem not allocated");
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_FAILURE;
   }
      
    hashkey = 0;
    offset = NSRM_SELECTION_RULE_TBL_OFFSET;
    MCHASH_TABLE_SCAN(nschainset_object_node_scan_p->nschain_selection_rule_set,hashkey)
    {
       MCHASH_BUCKET_SCAN(nschainset_object_node_scan_p->nschain_selection_rule_set,hashkey,
                       selection_rule_p,
                       struct nsrm_nschain_selection_rule*, offset)
      {
         if((strcmp(selection_rule_p->name_p,key_info_p->name_p)) == 0)           
         {
            OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"This selection rule is already present in this nschainset");
            CNTLR_RCU_READ_LOCK_RELEASE();
            return NSRM_FAILURE;
         }
         continue;
      } 
     break;
    }    
    ret_value = mempool_get_mem_block(nsrm_selection_rule_mempool_g,
                                      ( uchar8_t **)&selection_rule_p,&heap_b);
    if(ret_value != MEMPOOL_SUCCESS)
    {
       OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to get mempool");
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_FAILURE;
    } 

    selection_rule_p->name_p = (char *)calloc(1,128);
    selection_rule_p->nschain_object_name_p = (char *)calloc(1,128);
    strcpy(selection_rule_p->name_p ,key_info_p->name_p);
    strcpy(selection_rule_p->nschain_object_name_p ,config_info_p[8].value.nschain_object_name_p);
   
    selection_rule_p->nschain_object_saferef     =   nschain_object_handle;
    selection_rule_p->nschainset_object_saferef  =   nschainset_object_handle;
  
    selection_rule_p->src_mac.mac_address_type_e = config_info_p[0].value.src_mac.mac_address_type_e;
    memcpy(selection_rule_p->src_mac.mac,config_info_p[0].value.src_mac.mac,6);

    selection_rule_p->dst_mac.mac_address_type_e = config_info_p[1].value.dst_mac.mac_address_type_e;
    memcpy(selection_rule_p->dst_mac.mac,config_info_p[1].value.dst_mac.mac,6);
    
    selection_rule_p->eth_type.ethtype_type_e = config_info_p[2].value.eth_type.ethtype_type_e;
    selection_rule_p->eth_type.ethtype        = config_info_p[2].value.eth_type.ethtype;

    selection_rule_p->sip.ip_object_type_e    = config_info_p[3].value.sip.ip_object_type_e;
    if(selection_rule_p->sip.ip_object_type_e == 1)
    {
      selection_rule_p->sip.ipaddr_start        = config_info_p[3].value.sip.ipaddr_start;
      selection_rule_p->sip.ipaddr_end          = config_info_p[3].value.sip.ipaddr_start;
    }
    else
    {
      selection_rule_p->sip.ipaddr_start        = config_info_p[3].value.sip.ipaddr_start;
      selection_rule_p->sip.ipaddr_end          = config_info_p[3].value.sip.ipaddr_end;
    }
    if(selection_rule_p->sip.ipaddr_start > selection_rule_p->sip.ipaddr_end)
    {  
       OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"IP start address should be less than IP end address");
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_FAILURE;
    }

    selection_rule_p->dip.ip_object_type_e    = config_info_p[4].value.dip.ip_object_type_e; 
    if(selection_rule_p->dip.ip_object_type_e == 1)
    {
      selection_rule_p->dip.ipaddr_start        = config_info_p[4].value.dip.ipaddr_start;
      selection_rule_p->dip.ipaddr_end          = config_info_p[4].value.dip.ipaddr_start;
    }
    else
    {
      selection_rule_p->dip.ipaddr_start        = config_info_p[4].value.dip.ipaddr_start;
      selection_rule_p->dip.ipaddr_end          = config_info_p[4].value.dip.ipaddr_end;
    }
    if(selection_rule_p->dip.ipaddr_start > selection_rule_p->dip.ipaddr_end)
    {
       OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"IP start address should be less than IP end address");
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_FAILURE;
    }
 
    selection_rule_p->sp.port_object_type_e   = config_info_p[5].value.sp.port_object_type_e;
    if(selection_rule_p->sp.port_object_type_e == 1)
    { 
       selection_rule_p->sp.port_start           = config_info_p[5].value.sp.port_start;
       selection_rule_p->sp.port_end             = config_info_p[5].value.sp.port_end;
    }
    else
    {
      selection_rule_p->sp.port_start           = config_info_p[5].value.sp.port_start;
      selection_rule_p->sp.port_end             = config_info_p[5].value.sp.port_end;
    }
    if(selection_rule_p->sp.port_start > selection_rule_p->sp.port_end)
    {
       OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"Port start should be less than port end");
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_FAILURE;
    }
 
    selection_rule_p->dp.port_object_type_e   = config_info_p[6].value.dp.port_object_type_e;
    if(selection_rule_p->dp.port_object_type_e == 1)
    {
       selection_rule_p->dp.port_start           = config_info_p[6].value.dp.port_start;
       selection_rule_p->dp.port_end             = config_info_p[6].value.dp.port_start;
    }
    else
    {
       selection_rule_p->dp.port_start           = config_info_p[6].value.dp.port_start;
       selection_rule_p->dp.port_end             = config_info_p[6].value.dp.port_end;
    }
    if(selection_rule_p->dp.port_start > selection_rule_p->dp.port_end)
    {
       OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"Port start should be less than port end");
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_FAILURE;
    }

    selection_rule_p->ip_protocol             = config_info_p[7].value.ip_protocol;
    selection_rule_p->location_e              = key_info_p->location_e;
    if(config_info_p[9].value.relative_name_p != NULL)
    {
      selection_rule_p->relative_name_p = (char*)calloc(1,NSRM_MAX_NAME_LENGTH);
      strcpy(selection_rule_p->relative_name_p ,config_info_p[9].value.relative_name_p);
    }    
    selection_rule_p->admin_status_e          = config_info_p[10].value.admin_status_e;
    selection_rule_p->operational_status_e    = OPER_DISABLE;

    selection_rule_p->heap_b = heap_b; 
    hashobj_p = (uchar8_t *)selection_rule_p + NSRM_SELECTION_RULE_TBL_OFFSET;

    hashkey =0;
    offset = NSRM_SELECTION_RULE_TBL_OFFSET;
    if((config_info_p[9].value.relative_name_p == NULL) ||nschainset_object_node_scan_p->no_of_rules == 0        || selection_rule_p->location_e == LAST)
    {
       OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"This is the first selection rule to be added");
       MCHASH_APPEND_NODE(nschainset_object_node_scan_p->nschain_selection_rule_set,hashkey, selection_rule_p,index, magic, hashobj_p, status_b);
       if(FALSE == status_b)
       {
          OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR," Failed to insert selction rule before relative rule");
          free(selection_rule_p->name_p);
          free(selection_rule_p->nschain_object_name_p);
          CNTLR_RCU_READ_LOCK_RELEASE();
          if(mempool_release_mem_block(nsrm_selection_rule_mempool_g,
                                    (uchar8_t*)&selection_rule_p,
                                    selection_rule_p->heap_b) != MEMPOOL_SUCCESS)
          OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to release  allocated memory block.");
          return NSRM_SELECTION_RULE_ADD_FAIL;
       }  
       selection_rule_handle = magic;
       selection_rule_handle = (((selection_rule_handle)<<32) | (index));
       selection_rule_p->magic = magic;
       selection_rule_p->index = index;
       selection_rule_p->selection_rule_handle = selection_rule_handle;
       nschainset_object_node_scan_p->no_of_rules++;
       nschain_object_node_scan_p->nschain_usage_count++;
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_SUCCESS;
   }
 
   hashkey = 0;
   offset = NSRM_SELECTION_RULE_TBL_OFFSET;
   MCHASH_TABLE_SCAN(nschainset_object_node_scan_p->nschain_selection_rule_set,hashkey)   
   {
      MCHASH_BUCKET_SCAN(nschainset_object_node_scan_p->nschain_selection_rule_set,hashkey,
                         selection_rule_scan_node_p , struct nsrm_nschain_selection_rule *, offset)
      {
         if(selection_rule_scan_node_p->name_p != NULL)
         {     
    
         if((strcmp(config_info_p[9].value.relative_name_p , selection_rule_scan_node_p->name_p)) != 0)
         {
           if(selection_rule_p->location_e == FIRST)
           {
              relative_node_found = TRUE;
              break;
           }

           relative_node_found = FALSE;
           continue;
         }
        else
        {
           relative_node_found = TRUE;
           break;
        }
      }
     }
     break;
   }

   if(relative_node_found == FALSE)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"No relative record found ");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   }
  
   do
   {
     hashkey = 0;
     offset = NSRM_SELECTION_RULE_TBL_OFFSET;
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"relative record found "); 
     switch(selection_rule_p->location_e)
     {
       case BEFORE:
       case FIRST :
		   MCHASH_INSERT_NODE_BEFORE(nschainset_object_node_scan_p->nschain_selection_rule_set,
				   selection_rule_scan_node_p->selection_rule_tbl_link,
				   hashkey, selection_rule_p,index, magic, hashobj_p, status_b);
		   selection_rule_handle = magic;
		   selection_rule_handle = (((selection_rule_handle)<<32) | (index));
		   selection_rule_p->magic = magic;
		   selection_rule_p->index = index;

		   selection_rule_p->selection_rule_handle = selection_rule_handle;
		   nschainset_object_node_scan_p->no_of_rules++;
		   if(FALSE == status_b)
		   {
			   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR," Failed to insert selction rule before relative rule");
			   CNTLR_RCU_READ_LOCK_RELEASE();
			   if(mempool_release_mem_block(nsrm_selection_rule_mempool_g,
						   (uchar8_t*)&selection_rule_p,
						   selection_rule_p->heap_b) != MEMPOOL_SUCCESS)
				   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to release  allocated memory block.");
			   return NSRM_SELECTION_RULE_ADD_FAIL;
		   }
		   status =  NSRM_SUCCESS;
		   break;

       case AFTER :   
		   MCHASH_INSERT_NODE_AFTER(nschainset_object_node_scan_p->nschain_selection_rule_set,
				   selection_rule_scan_node_p->selection_rule_tbl_link,
				   hashkey, selection_rule_p,index, magic, hashobj_p, status_b);

		   selection_rule_handle = magic;
		   selection_rule_handle = (((selection_rule_handle)<<32) | (index));
		   selection_rule_p->magic = magic;
		   selection_rule_p->index = index;
		   selection_rule_p->selection_rule_handle = selection_rule_handle;
		   nschainset_object_node_scan_p->no_of_rules++;
		   nschain_object_node_scan_p->nschain_usage_count++;
		   if(FALSE == status_b)
		   {
		      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR," Failed to insert selction rule after relative rule");
	          CNTLR_RCU_READ_LOCK_RELEASE();
		      if(mempool_release_mem_block(nsrm_selection_rule_mempool_g,
						   (uchar8_t*)&selection_rule_p,
						   selection_rule_p->heap_b) != MEMPOOL_SUCCESS)
	               OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Failed to release  allocated memory block.");
		       return NSRM_SELECTION_RULE_ADD_FAIL;
		   }
		   status = NSRM_SUCCESS;
		   break;
     }

}while(0);

 
   if(status != NSRM_SUCCESS)
   {
       free(selection_rule_p->name_p);
       free(selection_rule_p->nschain_object_name_p);
       CNTLR_RCU_READ_LOCK_RELEASE();
       return NSRM_SELECTION_RULE_ADD_FAIL;
   } 
   apphookoffset = NSRM_NOTIFICATION_APP_HOOK_OFFSET;
   notification_data.add_del.nschainset_object_name_p = (char*)calloc(1,(strlen(key_info_p->nschainset_object_name_p)+1));
   notification_data.add_del.nschain_selection_rule_name_p = (char*)calloc(1,(strlen(key_info_p->name_p)+1));
   strcpy(notification_data.add_del.nschainset_object_name_p ,key_info_p->nschainset_object_name_p);
   strcpy(notification_data.add_del.nschain_selection_rule_name_p ,key_info_p->name_p);
   notification_data.add_del.nschainset_object_handle =  nschainset_object_handle;
   notification_data.add_del.nschain_selection_rule_handle =  selection_rule_handle;

   OF_LIST_SCAN(nsrm_selection_rule_notifications[NSRM_NSCHAIN_SELECTION_RULE_ADDED_TO_NSCHAIN_SET_OBJECT],
                  app_entry_p,
                  struct nsrm_notification_app_hook_info *,
                  apphookoffset)
   {
      ((nsrm_nschain_selection_rule_notifier)(app_entry_p->call_back))(NSRM_NSCHAIN_SELECTION_RULE_ADDED_TO_NSCHAIN_SET_OBJECT,
                                                                  selection_rule_handle,
                                                                   notification_data,
                                                                   app_entry_p->cbk_arg1,
                                                                   app_entry_p->cbk_arg2
                                                                   );
   }
   OF_LIST_SCAN(nsrm_selection_rule_notifications[NSRM_NSCHAIN_SELECTION_RULE_ALL] ,
                  app_entry_p,
                  struct nsrm_notification_app_hook_info *,
                  apphookoffset)
   {
        ((nsrm_nschain_selection_rule_notifier)(app_entry_p->call_back))(NSRM_NSCHAIN_SELECTION_RULE_ADDED_TO_NSCHAIN_SET_OBJECT,
                                                                   selection_rule_handle,
                                                                   notification_data,
                                                                   app_entry_p->cbk_arg1,
                                                                   app_entry_p->cbk_arg2
                                                                   );
   }

   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"succesfully attached selection rule");
   CNTLR_RCU_READ_LOCK_RELEASE();
   return NSRM_SUCCESS;
}

int32_t nsrm_del_nschain_selection_rule_from_nschainset(
        struct  nsrm_nschain_selection_rule_key*           key_info_p)

{
   struct nsrm_nschain_selection_rule  *selection_rule_p = NULL;
   struct nsrm_nschainset_object       *nschainset_object_scan_node_p = NULL; 
   struct nsrm_nschain_object          *nschain_object_node_scan_p = NULL; 
   union  nsrm_nschain_selection_rule_notification_data  notification_data = {};
   struct nsrm_notification_app_hook_info *app_entry_p = NULL;

   uint64_t  hashkey ,offset,apphookoffset ;
   uint64_t  nschainset_object_handle , nschain_object_handle;
   int32_t   ret_value = NSRM_FAILURE;
   uint8_t   status_b = FALSE , rule_node_found_b = FALSE;

   if(key_info_p->name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Seclection rule name is null");
      return NSRM_ERROR_INVALID_SELECTION_RULE_NAME;
   } 

   ret_value = nsrm_get_nschainset_object_handle(key_info_p->nschainset_object_name_p , &nschainset_object_handle);
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"nschainset object not valid");
      return NSRM_ERROR_NSCHAINSET_OBJECT_NAME_INVALID;
   }
   CNTLR_RCU_READ_LOCK_TAKE();
   
   ret_value = nsrm_get_nschainset_byhandle(nschainset_object_handle , &nschainset_object_scan_node_p);
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"nschainset object not valid");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_ERROR_NSCHAINSET_OBJECT_NAME_INVALID;
   }

   hashkey = 0;
   offset = NSRM_SELECTION_RULE_TBL_OFFSET;
   MCHASH_TABLE_SCAN(nschainset_object_scan_node_p->nschain_selection_rule_set , hashkey)     
   {
     MCHASH_BUCKET_SCAN(nschainset_object_scan_node_p->nschain_selection_rule_set,hashkey,
                        selection_rule_p,struct nsrm_nschain_selection_rule *, offset)
     {
        if( strcmp(selection_rule_p->name_p , key_info_p->name_p) == 0)
        {
           OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"node found to delete");
           rule_node_found_b = TRUE;
           break;
        }
     }
     break;
   }
   if(rule_node_found_b == FALSE)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Rule not found");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_ERROR_INVALID_SELECTION_RULE_NAME;
   }  
   apphookoffset = NSRM_NOTIFICATION_APP_HOOK_OFFSET;
   notification_data.add_del.nschainset_object_name_p = (char*)calloc(1,(strlen(key_info_p->nschainset_object_name_p)+1));
   notification_data.add_del.nschain_selection_rule_name_p = (char*)calloc(1,(strlen(key_info_p->name_p)+1));
   strcpy(notification_data.add_del.nschainset_object_name_p ,key_info_p->nschainset_object_name_p);
   strcpy(notification_data.add_del.nschain_selection_rule_name_p ,key_info_p->name_p);
   notification_data.add_del.nschainset_object_handle =  nschainset_object_handle;
   notification_data.add_del.nschain_selection_rule_handle =  selection_rule_p->selection_rule_handle;

   OF_LIST_SCAN(nsrm_selection_rule_notifications[NSRM_NSCHAIN_SELECTION_RULE_DELETED_FROM_NSCHAIN_SET_OBJECT],
                  app_entry_p,
                  struct nsrm_notification_app_hook_info *,
                  apphookoffset)
   {
      ((nsrm_nschain_selection_rule_notifier)(app_entry_p->call_back))(NSRM_NSCHAIN_SELECTION_RULE_DELETED_FROM_NSCHAIN_SET_OBJECT,
                                                                 selection_rule_p->selection_rule_handle,
                                                                   notification_data,
                                                                   app_entry_p->cbk_arg1,
                                                                   app_entry_p->cbk_arg2
                                                                   );
   }
   
   OF_LIST_SCAN(nsrm_selection_rule_notifications[NSRM_NSCHAIN_SELECTION_RULE_ALL] ,
                  app_entry_p,
                  struct nsrm_notification_app_hook_info *,
                  apphookoffset)
   {
        ((nsrm_nschain_selection_rule_notifier)(app_entry_p->call_back))(NSRM_NSCHAIN_SELECTION_RULE_DELETED_FROM_NSCHAIN_SET_OBJECT,
                                                                 selection_rule_p->selection_rule_handle,
                                                                   notification_data,
                                                                   app_entry_p->cbk_arg1,
                                                                   app_entry_p->cbk_arg2
                                                                   );
   }

   ret_value = nsrm_get_nschain_object_handle(selection_rule_p->nschain_object_name_p , &nschain_object_handle);
   if(ret_value != NSRM_SUCCESS)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"nschain object doesnt exist");
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
   }
   ret_value = nsrm_get_nschain_byhandle(nschain_object_handle , &nschain_object_node_scan_p);
   if(ret_value != NSRM_SUCCESS)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"failed to get nschain object");
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_ERROR_NSCHAIN_OBJECT_NAME_INVALID;
   }
   hashkey =0;
   offset = NSRM_SELECTION_RULE_TBL_OFFSET;
   MCHASH_DELETE_NODE_BY_REF(nschainset_object_scan_node_p->nschain_selection_rule_set, 
                             selection_rule_p->index, selection_rule_p->magic,
                             struct nsrm_nschain_selection_rule *,offset, status_b);
   CNTLR_RCU_READ_LOCK_RELEASE();
   if(status_b == TRUE)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "selection rule deleted successfully");
      nschain_object_node_scan_p->nschain_usage_count--;
      return NSRM_SUCCESS;
   }
   else
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "failed to delete selection rule");
      return NSRM_SELECTION_RULE_DEL_FAIL;
   }
}
  
int32_t nsrm_modify_nschain_selection_rule(
        struct  nsrm_nschain_selection_rule_key*           key_info_p,
        int32_t number_of_config_params,
        struct  nsrm_nschain_selection_rule_config_info*   config_info_p)
{
   struct nsrm_nschain_selection_rule  *selection_rule_p = NULL;
   struct nsrm_nschainset_object       *nschainset_object_scan_node_p = NULL; 

   uint64_t  hashkey ,offset ;
   uint64_t  nschainset_object_handle,nschain_object_handle = 0;
   int32_t   ret_value = NSRM_FAILURE ;
   uint8_t   rule_node_found_b = FALSE;

   if(key_info_p->name_p == NULL)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Selection rule name is null");
      return NSRM_ERROR_INVALID_SELECTION_RULE_NAME;
   } 

   ret_value = nsrm_get_nschainset_object_handle(key_info_p->nschainset_object_name_p , &nschainset_object_handle);
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"nschainset object not valid");
      return NSRM_ERROR_NSCHAINSET_OBJECT_NAME_INVALID;
   }

   CNTLR_RCU_READ_LOCK_TAKE();
   ret_value = nsrm_get_nschainset_byhandle(nschainset_object_handle , &nschainset_object_scan_node_p);
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"nschainset object not valid");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_ERROR_NSCHAINSET_OBJECT_NAME_INVALID;
   }

   hashkey = 0;
   offset = NSRM_SELECTION_RULE_TBL_OFFSET;
   MCHASH_TABLE_SCAN(nschainset_object_scan_node_p->nschain_selection_rule_set , hashkey)
   {
      MCHASH_BUCKET_SCAN(nschainset_object_scan_node_p->nschain_selection_rule_set , hashkey ,
                         selection_rule_p, struct nsrm_nschain_selection_rule * , offset)
      {
         if((strcmp(selection_rule_p->name_p , key_info_p->name_p) == 0))
         {
            rule_node_found_b = TRUE;
            break;
         }
      }
     break;
   }
                                      
   if(rule_node_found_b == FALSE)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"No rule found to be modified");
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_ERROR_INVALID_SELECTION_RULE_NAME;
   }
 
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"node found to modify");
   selection_rule_p->name_p = (char *)calloc(1,(strlen(key_info_p->name_p)+1));
   selection_rule_p->nschain_object_name_p = (char *)calloc(1,(strlen(config_info_p[8].value.nschain_object_name_p)+1));
   
   strcpy(selection_rule_p->name_p ,key_info_p->name_p);
   strcpy(selection_rule_p->nschain_object_name_p ,config_info_p[8].value.nschain_object_name_p);
   if(config_info_p[9].value.relative_name_p != NULL)
   {
     selection_rule_p->relative_name_p = (char *)calloc(1,(strlen(config_info_p[9].value.relative_name_p)+1)); 
     strcpy(selection_rule_p->relative_name_p ,config_info_p[9].value.relative_name_p);
   }
   selection_rule_p->nschain_object_saferef      =   nschain_object_handle;
   selection_rule_p->nschainset_object_saferef   =   nschainset_object_handle;
  
   selection_rule_p->src_mac.mac_address_type_e = config_info_p[0].value.src_mac.mac_address_type_e;
   memcpy(selection_rule_p->src_mac.mac,config_info_p[0].value.src_mac.mac,6);

   selection_rule_p->dst_mac.mac_address_type_e = config_info_p[1].value.dst_mac.mac_address_type_e;
   memcpy(selection_rule_p->dst_mac.mac,config_info_p[1].value.dst_mac.mac,6);
   
    
   selection_rule_p->eth_type.ethtype_type_e = config_info_p[2].value.eth_type.ethtype_type_e;
   selection_rule_p->eth_type.ethtype        = config_info_p[2].value.eth_type.ethtype;

   selection_rule_p->sip.ip_object_type_e    = config_info_p[3].value.sip.ip_object_type_e;
   if(selection_rule_p->sip.ip_object_type_e == 1)
   {
      selection_rule_p->sip.ipaddr_start        = config_info_p[3].value.sip.ipaddr_start;
      selection_rule_p->sip.ipaddr_end          = config_info_p[3].value.sip.ipaddr_start;
   }
   else
   {
      selection_rule_p->sip.ipaddr_start        = config_info_p[3].value.sip.ipaddr_start;
      selection_rule_p->sip.ipaddr_end          = config_info_p[3].value.sip.ipaddr_end;
   }
   if(selection_rule_p->sip.ipaddr_start > selection_rule_p->sip.ipaddr_end)
   {  
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"IP start address should be less than IP end address");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   }

   selection_rule_p->dip.ip_object_type_e    = config_info_p[4].value.dip.ip_object_type_e;
   if(selection_rule_p->dip.ip_object_type_e == 1)
   {
     selection_rule_p->dip.ipaddr_start        = config_info_p[4].value.dip.ipaddr_start;
     selection_rule_p->dip.ipaddr_end          = config_info_p[4].value.dip.ipaddr_start;
   }
   else
   {
     selection_rule_p->dip.ipaddr_start        = config_info_p[4].value.dip.ipaddr_start;
     selection_rule_p->dip.ipaddr_end          = config_info_p[4].value.dip.ipaddr_end;
   }
   if(selection_rule_p->dip.ipaddr_start > selection_rule_p->dip.ipaddr_end)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"IP start address should be less than IP end address");
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_FAILURE;
   }
   selection_rule_p->sp.port_object_type_e   = config_info_p[5].value.sp.port_object_type_e;
   if(selection_rule_p->sp.port_object_type_e == 1)
   { 
     selection_rule_p->sp.port_start           = config_info_p[5].value.sp.port_start;
     selection_rule_p->sp.port_end             = config_info_p[5].value.sp.port_end;
   }
   else
   {
     selection_rule_p->sp.port_start           = config_info_p[5].value.sp.port_start;
     selection_rule_p->sp.port_end             = config_info_p[5].value.sp.port_end;
   }
   if(selection_rule_p->sp.port_start > selection_rule_p->sp.port_end)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"Port start should be less than port end");
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_FAILURE;
   }

   selection_rule_p->dp.port_object_type_e   = config_info_p[6].value.dp.port_object_type_e;
   if(selection_rule_p->dp.port_object_type_e == 1)
   {
     selection_rule_p->dp.port_start           = config_info_p[6].value.dp.port_start;
     selection_rule_p->dp.port_end             = config_info_p[6].value.dp.port_start;
   }
   else
   {
     selection_rule_p->dp.port_start           = config_info_p[6].value.dp.port_start;
     selection_rule_p->dp.port_end             = config_info_p[6].value.dp.port_end;
   }
   if(selection_rule_p->dp.port_start > selection_rule_p->dp.port_end)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"Port start should be less than port end");
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_FAILURE;
   }

   selection_rule_p->ip_protocol             = config_info_p[7].value.ip_protocol;
   selection_rule_p->location_e              = key_info_p->location_e;
   selection_rule_p->admin_status_e          = config_info_p[10].value.admin_status_e;
   selection_rule_p->operational_status_e    = OPER_DISABLE;

   CNTLR_RCU_READ_LOCK_RELEASE();
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"Selection rule modified successfully");
   return NSRM_SUCCESS;    
  
}
 
int32_t nsrm_get_first_selection_rules_from_nschainset_object (
        struct   nsrm_nschainset_object_key*     nschainset_object_key_p,
        int32_t  number_of_selection_rules_requested,
        int32_t* number_of_selection_rules_returned_p,
        struct   nsrm_nschain_selection_rule_record*  recs_p)
{
   struct    nsrm_nschain_selection_rule             *selection_rule_node_scan_p = NULL;
   struct    nsrm_nschainset_object                  *nschainset_object_scan_node_p = NULL;

   uint64_t  hashkey , nschainset_object_handle;
   int32_t   offset , selection_rule_returned = 0;
   int32_t   selection_rule_requested = number_of_selection_rules_requested;
   int32_t   status = NSRM_FAILURE , ret_value = NSRM_FAILURE;

   ret_value = nsrm_get_nschainset_object_handle(nschainset_object_key_p->name_p , &nschainset_object_handle);    
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"NSchainset not found");
      return NSRM_ERROR_NSCHAINSET_OBJECT_NAME_INVALID;
   }
   CNTLR_RCU_READ_LOCK_TAKE();
   ret_value = nsrm_get_nschainset_byhandle(nschainset_object_handle ,&nschainset_object_scan_node_p);
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"NSchainset not found");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_ERROR_NSCHAINSET_OBJECT_NAME_INVALID;
   } 

   if(nschainset_object_scan_node_p->no_of_rules == 0)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"No rules attached to this chainset");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   }
  
   hashkey = 0;
   offset = NSRM_SELECTION_RULE_TBL_OFFSET; 
   MCHASH_TABLE_SCAN(nschainset_object_scan_node_p->nschain_selection_rule_set,hashkey)
   {
      MCHASH_BUCKET_SCAN(nschainset_object_scan_node_p->nschain_selection_rule_set,hashkey,
                        selection_rule_node_scan_p,struct nsrm_nschain_selection_rule *, offset)
      {
         strcpy(recs_p[selection_rule_returned].key.name_p , selection_rule_node_scan_p->name_p);
         strcpy(recs_p[selection_rule_returned].info[8].value.nschain_object_name_p , selection_rule_node_scan_p->nschain_object_name_p);
         if(selection_rule_node_scan_p->relative_name_p != NULL)
         {
           strcpy(recs_p[selection_rule_returned].info[9].value.relative_name_p , selection_rule_node_scan_p->relative_name_p);
         }
         recs_p[selection_rule_returned].info[0].value.src_mac.mac_address_type_e = selection_rule_node_scan_p->src_mac.mac_address_type_e;
     	 memcpy(recs_p[selection_rule_returned].info[0].value.src_mac.mac,selection_rule_node_scan_p->src_mac.mac,6);
     	 recs_p[selection_rule_returned].info[1].value.dst_mac.mac_address_type_e=selection_rule_node_scan_p->dst_mac.mac_address_type_e;
    	 memcpy(recs_p[selection_rule_returned].info[1].value.dst_mac.mac,selection_rule_node_scan_p->dst_mac.mac,6);
     	 recs_p[selection_rule_returned].info[2].value.eth_type.ethtype_type_e = selection_rule_node_scan_p->eth_type.ethtype_type_e;
         recs_p[selection_rule_returned].info[2].value.eth_type.ethtype        = selection_rule_node_scan_p->eth_type.ethtype; 
         recs_p[selection_rule_returned].info[3].value.sip.ip_object_type_e = selection_rule_node_scan_p->sip.ip_object_type_e;
         recs_p[selection_rule_returned].info[3].value.sip.ipaddr_start     = selection_rule_node_scan_p->sip.ipaddr_start;
         recs_p[selection_rule_returned].info[3].value.sip.ipaddr_end       = selection_rule_node_scan_p->sip.ipaddr_end;
         recs_p[selection_rule_returned].info[4].value.dip.ip_object_type_e = selection_rule_node_scan_p->dip.ip_object_type_e;
         recs_p[selection_rule_returned].info[4].value.dip.ipaddr_start     = selection_rule_node_scan_p->dip.ipaddr_start;
         recs_p[selection_rule_returned].info[4].value.dip.ipaddr_end       = selection_rule_node_scan_p->dip.ipaddr_end;
         recs_p[selection_rule_returned].info[5].value.sp.port_object_type_e= selection_rule_node_scan_p->sp.port_object_type_e;
         recs_p[selection_rule_returned].info[5].value.sp.port_start        = selection_rule_node_scan_p->sp.port_start;
         recs_p[selection_rule_returned].info[5].value.sp.port_end          = selection_rule_node_scan_p->sp.port_end;
         recs_p[selection_rule_returned].info[6].value.dp.port_object_type_e = selection_rule_node_scan_p->dp.port_object_type_e;
         recs_p[selection_rule_returned].info[6].value.dp.port_start         = selection_rule_node_scan_p->dp.port_start;
         recs_p[selection_rule_returned].info[6].value.dp.port_end           = selection_rule_node_scan_p->dp.port_end;
         recs_p[selection_rule_returned].info[7].value.ip_protocol           = selection_rule_node_scan_p->ip_protocol;
         recs_p[selection_rule_returned].key.location_e                        = selection_rule_node_scan_p->location_e;
         recs_p[selection_rule_returned].info[10].value.admin_status_e       = selection_rule_node_scan_p->admin_status_e;
         status = NSRM_SUCCESS;
         number_of_selection_rules_requested--;
         selection_rule_returned++;
         if(number_of_selection_rules_requested)
           continue;
         else
           break;
      }
      if(number_of_selection_rules_requested)
        continue;
      else
        break;
   }
   if(selection_rule_requested != selection_rule_returned)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Invalid number of objects returned / requested ");
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_FAILURE; 
   }
   if(status == NSRM_SUCCESS)  
   { 
      *number_of_selection_rules_returned_p = selection_rule_returned;
       CNTLR_RCU_READ_LOCK_RELEASE();
       return status;
   }
   CNTLR_RCU_READ_LOCK_RELEASE();
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "No More Entries in the selection hash table!.");
   return NSRM_ERROR_NO_MORE_ENTRIES;
}
  
          
int32_t nsrm_get_next_selection_rules_from_nschainset_object (
        struct   nsrm_nschainset_object_key*     nschainset_object_key_p,
        struct   nsrm_nschain_selection_rule_key*  relative_record_p,
        int32_t  number_of_selection_rules_requested,
        int32_t* number_of_selection_rules_returned_p,
        struct   nsrm_nschain_selection_rule_record* recs_p)
{
   struct    nsrm_nschain_selection_rule             *selection_rule_node_scan_p = NULL;
   struct    nsrm_nschainset_object                  *nschainset_object_scan_node_p = NULL;
   uint64_t  hashkey , nschainset_object_handle;
   int32_t   offset , selection_rule_returned = 0;
   int32_t   selection_rule_requested = number_of_selection_rules_requested;
   int32_t   status = NSRM_FAILURE , ret_value = NSRM_FAILURE;
   uint8_t   relative_node_found_b = FALSE;

   if(nschainset_object_key_p->name_p == NULL)
   {
       OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"NSchainset name is NULL");
       return NSRM_ERROR_NSCHAINSET_OBJECT_NAME_INVALID;
   }
   ret_value = nsrm_get_nschainset_object_handle(nschainset_object_key_p->name_p , &nschainset_object_handle);    
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"NSchainset not found");
      return NSRM_ERROR_NSCHAINSET_OBJECT_NAME_INVALID;
   }

   CNTLR_RCU_READ_LOCK_TAKE();
   ret_value = nsrm_get_nschainset_byhandle(nschainset_object_handle ,&nschainset_object_scan_node_p);
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"NSchainset not found");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_ERROR_NSCHAINSET_OBJECT_NAME_INVALID;
   } 

   if(nschainset_object_scan_node_p->no_of_rules == 0)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"No rules attached to this chainset");
      CNTLR_RCU_READ_LOCK_RELEASE(); 
      return NSRM_FAILURE;
   }
   hashkey = 0;
   offset = NSRM_SELECTION_RULE_TBL_OFFSET; 
   MCHASH_TABLE_SCAN(nschainset_object_scan_node_p->nschain_selection_rule_set,hashkey)
   {
      MCHASH_BUCKET_SCAN(nschainset_object_scan_node_p->nschain_selection_rule_set,hashkey,
                        selection_rule_node_scan_p,struct nsrm_nschain_selection_rule *, offset)
      {
         if(relative_node_found_b == FALSE)
         {
            if(strcmp(relative_record_p->name_p , selection_rule_node_scan_p->name_p) != 0)
            {
              continue;
            }
            else 
            {
              
              relative_node_found_b = TRUE;
              continue;
            }
         }
         if(relative_node_found_b != TRUE)
         {
            CNTLR_RCU_READ_LOCK_RELEASE();
            return NSRM_FAILURE;
         }
         strcpy(recs_p[selection_rule_returned].key.name_p , selection_rule_node_scan_p->name_p);
         strcpy(recs_p[selection_rule_returned].info[8].value.nschain_object_name_p , selection_rule_node_scan_p->nschain_object_name_p);
         if(selection_rule_node_scan_p->relative_name_p != NULL) 
         {
           strcpy(recs_p[selection_rule_returned].info[9].value.relative_name_p , selection_rule_node_scan_p->relative_name_p);
         }
         recs_p[selection_rule_returned].info[0].value.src_mac.mac_address_type_e = selection_rule_node_scan_p->src_mac.mac_address_type_e;
    	 memcpy(recs_p[selection_rule_returned].info[0].value.src_mac.mac,selection_rule_node_scan_p->src_mac.mac,6);
     	 recs_p[selection_rule_returned].info[1].value.dst_mac.mac_address_type_e=selection_rule_node_scan_p->dst_mac.mac_address_type_e;
    	 memcpy(recs_p[selection_rule_returned].info[1].value.dst_mac.mac,selection_rule_node_scan_p->dst_mac.mac,6);
     	 recs_p[selection_rule_returned].info[2].value.eth_type.ethtype_type_e = selection_rule_node_scan_p->eth_type.ethtype_type_e;
         recs_p[selection_rule_returned].info[2].value.eth_type.ethtype        = selection_rule_node_scan_p->eth_type.ethtype; 
         recs_p[selection_rule_returned].info[3].value.sip.ip_object_type_e = selection_rule_node_scan_p->sip.ip_object_type_e;
         recs_p[selection_rule_returned].info[3].value.sip.ipaddr_start     = selection_rule_node_scan_p->sip.ipaddr_start;
         recs_p[selection_rule_returned].info[3].value.sip.ipaddr_end       = selection_rule_node_scan_p->sip.ipaddr_end;
         recs_p[selection_rule_returned].info[4].value.dip.ip_object_type_e = selection_rule_node_scan_p->dip.ip_object_type_e;
         recs_p[selection_rule_returned].info[4].value.dip.ipaddr_start     = selection_rule_node_scan_p->dip.ipaddr_start;
         recs_p[selection_rule_returned].info[4].value.dip.ipaddr_end       = selection_rule_node_scan_p->dip.ipaddr_end;
         recs_p[selection_rule_returned].info[5].value.sp.port_object_type_e= selection_rule_node_scan_p->sp.port_object_type_e;
         recs_p[selection_rule_returned].info[5].value.sp.port_start        = selection_rule_node_scan_p->sp.port_start;
         recs_p[selection_rule_returned].info[5].value.sp.port_end          = selection_rule_node_scan_p->sp.port_end;
         recs_p[selection_rule_returned].info[6].value.dp.port_object_type_e = selection_rule_node_scan_p->dp.port_object_type_e;
         recs_p[selection_rule_returned].info[6].value.dp.port_start         = selection_rule_node_scan_p->dp.port_start;
         recs_p[selection_rule_returned].info[6].value.dp.port_end           = selection_rule_node_scan_p->dp.port_end;
         recs_p[selection_rule_returned].info[7].value.ip_protocol           = selection_rule_node_scan_p->ip_protocol;
         recs_p[selection_rule_returned].key.location_e                        = selection_rule_node_scan_p->location_e;
         recs_p[selection_rule_returned].info[10].value.admin_status_e       = selection_rule_node_scan_p->admin_status_e;
         status = NSRM_SUCCESS;
         number_of_selection_rules_requested--;
         selection_rule_returned++;
         if(number_of_selection_rules_requested)
           continue;
         else
           break;
      }
      if(number_of_selection_rules_requested)
        continue;
      else
        break;
   }
   if(selection_rule_requested != selection_rule_returned)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"Invalid number of objects returned / requested ");
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_FAILURE; 
   }
   if(status == NSRM_SUCCESS)  
   { 
      *number_of_selection_rules_returned_p = selection_rule_returned;
       CNTLR_RCU_READ_LOCK_RELEASE();
       return status;
   }
   CNTLR_RCU_READ_LOCK_RELEASE();
   return status; /*just to avoid warning this return is put*/
}
 
int32_t nsrm_get_exact_selection_rule_from_nschainset_object(
        struct   nsrm_nschainset_object_key*     nschainset_object_key_p,
        struct   nsrm_nschain_selection_rule_key       *key_info_p,
        struct   nsrm_nschain_selection_rule_record*    rec_p)
{
   struct    nsrm_nschain_selection_rule             *selection_rule_node_scan_p = NULL;
   struct    nsrm_nschainset_object                  *nschainset_object_scan_node_p = NULL;

   uint64_t  hashkey , nschainset_object_handle;
   int32_t   offset;
   int32_t   ret_value = NSRM_FAILURE;
   uint8_t   current_node_found_b = FALSE;

   ret_value = nsrm_get_nschainset_object_handle(nschainset_object_key_p->name_p , &nschainset_object_handle);    
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"NSchainset not found");
      return NSRM_ERROR_NSCHAINSET_OBJECT_NAME_INVALID;
   }
   CNTLR_RCU_READ_LOCK_TAKE();
   ret_value = nsrm_get_nschainset_byhandle(nschainset_object_handle ,&nschainset_object_scan_node_p);
   if(ret_value != NSRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR,"NSchainset not found");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_ERROR_NSCHAINSET_OBJECT_NAME_INVALID;
   } 

   if(nschainset_object_scan_node_p->no_of_rules == 0)
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"No rules attached to this chainset");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return NSRM_FAILURE;
   }
   hashkey = 0;
   offset = NSRM_SELECTION_RULE_TBL_OFFSET; 
   MCHASH_TABLE_SCAN(nschainset_object_scan_node_p->nschain_selection_rule_set,hashkey)
   {
      MCHASH_BUCKET_SCAN(nschainset_object_scan_node_p->nschain_selection_rule_set,hashkey,
                        selection_rule_node_scan_p,struct nsrm_nschain_selection_rule *, offset)
      {
        if(strcmp(key_info_p->name_p , selection_rule_node_scan_p->name_p) != 0)
          continue;
        else 
        {
          current_node_found_b = TRUE;
          break;
        }
      }
     break;
   }
   if(current_node_found_b == FALSE)
   {
     OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"no selection rule with such name exists");
     CNTLR_RCU_READ_LOCK_RELEASE();
     return NSRM_ERROR_INVALID_SELECTION_RULE_NAME;
   }
   strcpy(rec_p[0].key.name_p , selection_rule_node_scan_p->name_p);
   strcpy(rec_p[0].info[8].value.nschain_object_name_p , selection_rule_node_scan_p->nschain_object_name_p);
   if(selection_rule_node_scan_p->relative_name_p != NULL)
   {
	   strcpy(rec_p[0].info[9].value.relative_name_p , selection_rule_node_scan_p->relative_name_p);
   }  
   rec_p[0].info[0].value.src_mac.mac_address_type_e = selection_rule_node_scan_p->src_mac.mac_address_type_e;
   memcpy(rec_p[0].info[0].value.src_mac.mac,selection_rule_node_scan_p->src_mac.mac,6);
   rec_p[0].info[1].value.dst_mac.mac_address_type_e=selection_rule_node_scan_p->dst_mac.mac_address_type_e;
   memcpy(rec_p[0].info[1].value.dst_mac.mac,selection_rule_node_scan_p->dst_mac.mac,6);
   rec_p[0].info[2].value.eth_type.ethtype_type_e = selection_rule_node_scan_p->eth_type.ethtype_type_e;
   rec_p[0].info[2].value.eth_type.ethtype        = selection_rule_node_scan_p->eth_type.ethtype; 
   rec_p[0].info[3].value.sip.ip_object_type_e = selection_rule_node_scan_p->sip.ip_object_type_e;
   rec_p[0].info[3].value.sip.ipaddr_start     = selection_rule_node_scan_p->sip.ipaddr_start;
   rec_p[0].info[3].value.sip.ipaddr_end       = selection_rule_node_scan_p->sip.ipaddr_end;
   rec_p[0].info[4].value.dip.ip_object_type_e = selection_rule_node_scan_p->dip.ip_object_type_e;
   rec_p[0].info[4].value.dip.ipaddr_start     = selection_rule_node_scan_p->dip.ipaddr_start;
   rec_p[0].info[4].value.dip.ipaddr_end       = selection_rule_node_scan_p->dip.ipaddr_end;
   rec_p[0].info[5].value.sp.port_object_type_e= selection_rule_node_scan_p->sp.port_object_type_e;
   rec_p[0].info[5].value.sp.port_start        = selection_rule_node_scan_p->sp.port_start;
   rec_p[0].info[5].value.sp.port_end          = selection_rule_node_scan_p->sp.port_end;
   rec_p[0].info[6].value.dp.port_object_type_e= selection_rule_node_scan_p->dp.port_object_type_e;
   rec_p[0].info[6].value.dp.port_start         = selection_rule_node_scan_p->dp.port_start;
   rec_p[0].info[6].value.dp.port_end           = selection_rule_node_scan_p->dp.port_end;
   rec_p[0].info[7].value.ip_protocol           = selection_rule_node_scan_p->ip_protocol;
   rec_p[0].key.location_e                        = selection_rule_node_scan_p->location_e;
   rec_p[0].info[10].value.admin_status_e       = selection_rule_node_scan_p->admin_status_e;

   CNTLR_RCU_READ_LOCK_RELEASE();
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "Exact entry found and returned!.");
   return NSRM_SUCCESS;

}
 
int32_t nsrm_register_nschain_selection_rule_notifications(
                          uint8_t notification_type,
                          nsrm_nschain_selection_rule_notifier  selection_rule_notifier,
                          void* callback_arg1,
                          void* callback_arg2)    
{
   struct  nsrm_notification_app_hook_info *app_entry_p=NULL;
   uint8_t heap_b;
   int32_t retval = NSRM_SUCCESS;
   uchar8_t lstoffset;
   lstoffset = NSRM_NOTIFICATION_APP_HOOK_OFFSET;
   
   if((notification_type < NSRM_SELECTION_RULE_FIRST_NOTIFICATION_TYPE) ||
     (notification_type > NSRM_SELECTION_RULE_LAST_NOTIFICATION_TYPE))
   {
      OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_ERROR, "Invalid Notification type:%d", notification_type);
      return NSRM_FAILURE;
   }
   if(selection_rule_notifier == NULL)
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

   app_entry_p->call_back = (void*)selection_rule_notifier;
   app_entry_p->cbk_arg1  = callback_arg1;
   app_entry_p->cbk_arg2  = callback_arg2;
   app_entry_p->heap_b    = heap_b;
  
   /* Add Application to the list of nschain object notifications */
   OF_APPEND_NODE_TO_LIST(nsrm_selection_rule_notifications[notification_type],app_entry_p, lstoffset); 
   if(retval != NSRM_SUCCESS)
     mempool_release_mem_block(nsrm_notifications_mempool_g,(uchar8_t*)app_entry_p, app_entry_p->heap_b);

   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "callback function reg with selection rule success");
   return retval;
}

int32_t nsrm_add_attribute_to_nschain_selection_rule(
          char*     nschain_selection_rule_name_p,
          char*     nschainset_object_name_p,
          struct    nsrm_attribute_name_value_pair *attribute_info_p)
{
  int32_t  ret_val;
  struct   nsrm_nschainset_object                              *nschainset_object_p;
  struct   nsrm_nschain_selection_rule                         *selection_rule_entry_p;
  union    nsrm_nschain_selection_rule_notification_data        notification_data={};
  struct   nsrm_notification_app_hook_info               *app_entry_p;
  int32_t  apphookoffset;
  uint64_t selection_rule_handle , nschainset_object_handle;
  apphookoffset = NSRM_NOTIFICATION_APP_HOOK_OFFSET;

  CNTLR_RCU_READ_LOCK_TAKE();
  ret_val = nsrm_get_nschainset_object_handle(nschainset_object_name_p,&nschainset_object_handle);
  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return ret_val;
  }
  ret_val = nsrm_get_nschainset_byhandle(nschainset_object_handle,&nschainset_object_p);
  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return ret_val;
  }
  ret_val = nsrm_get_selection_rule_handle(nschain_selection_rule_name_p,nschainset_object_p,&selection_rule_handle);
  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return ret_val;
  }
  ret_val = nsrm_get_selection_rule_byhandle(nschainset_object_p,selection_rule_handle,&selection_rule_entry_p);
  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return ret_val;
  }
  ret_val = nsrm_add_attribute(&(selection_rule_entry_p->attributes),attribute_info_p);
  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return ret_val;
  }
  OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "added attr");
  selection_rule_entry_p->number_of_attributes++;

  nsrm_create_selection_rule_view_entry(selection_rule_handle, attribute_info_p->attribute_name_p, attribute_info_p->attribute_value_p);

  notification_data.attribute.nschain_selection_rule_name_p = (char *)calloc(1,strlen(selection_rule_entry_p->name_p)+1);
  notification_data.attribute.nschain_selection_rule_attribute_name_p = (char *)calloc(1,strlen(attribute_info_p->attribute_name_p)+1);
  notification_data.attribute.nschain_selection_rule_attribute_value_p = (char *)calloc(1,strlen(attribute_info_p->attribute_value_p)+1);
  strcpy(notification_data.attribute.nschain_selection_rule_name_p, selection_rule_entry_p->name_p);
  strcpy(notification_data.attribute.nschain_selection_rule_attribute_name_p,attribute_info_p->attribute_name_p);
  strcpy(notification_data.attribute.nschain_selection_rule_attribute_value_p,attribute_info_p->attribute_value_p);
  notification_data.attribute.nschain_selection_rule_handle = selection_rule_handle;

  OF_LIST_SCAN(nsrm_selection_rule_notifications[NSRM_NSCHAIN_SELECTION_RULE_ATTRIBUTE_ADDED],
               app_entry_p,
               struct nsrm_notification_app_hook_info *,
               apphookoffset)
  {
     ((nsrm_nschain_selection_rule_notifier)(app_entry_p->call_back))(NSRM_NSCHAIN_SELECTION_RULE_ATTRIBUTE_ADDED,
                                                                selection_rule_handle,
                                                                notification_data,
                                                                app_entry_p->cbk_arg1,
                                                                 app_entry_p->cbk_arg2
                                                                   );
  }

   OF_LIST_SCAN(nsrm_selection_rule_notifications[NSRM_NSCHAIN_SELECTION_RULE_ALL] ,
                app_entry_p,
                struct nsrm_notification_app_hook_info *,
                apphookoffset)
   {
        ((nsrm_nschain_selection_rule_notifier)(app_entry_p->call_back))(NSRM_NSCHAIN_SELECTION_RULE_ATTRIBUTE_DELETED,
                                                                   selection_rule_handle,
                                                                   notification_data,
                                                                   app_entry_p->cbk_arg1,
                                                                   app_entry_p->cbk_arg2
                                                                   );
   }

  OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG, "success");
  CNTLR_RCU_READ_LOCK_RELEASE();
  return NSRM_SUCCESS;
}
 
int32_t nsrm_del_attribute_from_nschain_selection_rule(
                                                 char* nschain_selection_rule_name_p,
                                                 char* nschainset_object_name_p,
                                                 char* attribute_name_p)
{
  struct   nsrm_nschainset_object                              *nschainset_object_p;
  struct   nsrm_nschain_selection_rule                         *selection_rule_entry_p;
  union    nsrm_nschain_selection_rule_notification_data        notification_data={};
  struct   nsrm_notification_app_hook_info               *app_entry_p;
  struct   nsrm_resource_attribute_entry *attribute_entry_p,*prev_attr_entry_p = NULL;

  int32_t  apphookoffset,ret_val;
  uint64_t selection_rule_handle , nschainset_object_handle;
  uint8_t  attr_node_found = 0;

  apphookoffset = NSRM_NOTIFICATION_APP_HOOK_OFFSET;
  CNTLR_RCU_READ_LOCK_TAKE();

  ret_val = nsrm_get_nschainset_object_handle(nschainset_object_name_p,&nschainset_object_handle);
  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return ret_val;
  }
  ret_val = nsrm_get_nschainset_byhandle(nschainset_object_handle,&nschainset_object_p);
  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return ret_val;
  }
  ret_val = nsrm_get_selection_rule_handle(nschain_selection_rule_name_p,nschainset_object_p,&selection_rule_handle);
  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return ret_val;
  }
  ret_val = nsrm_get_selection_rule_byhandle(nschainset_object_p,selection_rule_handle,&selection_rule_entry_p);
  if(ret_val != NSRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return ret_val;
  }
  
  OF_LIST_SCAN(selection_rule_entry_p->attributes, attribute_entry_p, struct nsrm_resource_attribute_entry*,apphookoffset)
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
   notification_data.attribute.nschain_selection_rule_name_p = (char *)calloc(1,strlen(selection_rule_entry_p->name_p)+1);
  notification_data.attribute.nschain_selection_rule_attribute_name_p = (char *)calloc(1,strlen(attribute_name_p)+1);
  /*
  notification_data.attribute.nschain_selection_rule_attribute_value_p = (char *)calloc(1,strlen(attribute_info_p->attribute_value_p)+1);
  */
  strcpy(notification_data.attribute.nschain_selection_rule_name_p, selection_rule_entry_p->name_p);
  strcpy(notification_data.attribute.nschain_selection_rule_attribute_name_p,attribute_name_p);
//  strcpy(notification_data.attribute.nschain_selection_rule_attribute_value_p,attribute_info_p->attribute_value_p);
  notification_data.attribute.nschain_selection_rule_handle = selection_rule_handle;

  OF_LIST_SCAN(nsrm_selection_rule_notifications[NSRM_NSCHAIN_SELECTION_RULE_ATTRIBUTE_DELETED],
               app_entry_p,
               struct nsrm_notification_app_hook_info *,
               apphookoffset)
  {
     ((nsrm_nschain_selection_rule_notifier)(app_entry_p->call_back))(NSRM_NSCHAIN_SELECTION_RULE_ATTRIBUTE_DELETED,
                                                                selection_rule_handle,
                                                                notification_data,
                                                                app_entry_p->cbk_arg1,
                                                                 app_entry_p->cbk_arg2
                                                                   );
  }

   OF_LIST_SCAN(nsrm_selection_rule_notifications[NSRM_NSCHAIN_SELECTION_RULE_ALL] ,
                app_entry_p,
                struct nsrm_notification_app_hook_info *,
                apphookoffset)
   {
        ((nsrm_nschain_selection_rule_notifier)(app_entry_p->call_back))(NSRM_NSCHAIN_SELECTION_RULE_ATTRIBUTE_DELETED,
                                                                   selection_rule_handle,
                                                                   notification_data,
                                                                   app_entry_p->cbk_arg1,
                                                                   app_entry_p->cbk_arg2
                                                                   );
   }

   OF_REMOVE_NODE_FROM_LIST(selection_rule_entry_p->attributes,attribute_entry_p,prev_attr_entry_p,apphookoffset);
   selection_rule_entry_p->number_of_attributes--;
   nsrm_remove_selection_rule_view_entry(attribute_entry_p->name, attribute_entry_p->value);

   CNTLR_RCU_READ_LOCK_RELEASE();
   return NSRM_SUCCESS;
}

int32_t nsrm_get_first_selection_rule_attribute(
          char*   nschain_selection_rule_name_p,
          char*   nschainset_object_name_p,
          struct  nsrm_attribute_name_value_output_info *attribute_output_p) 
{
  struct   nsrm_nschain_selection_rule      *selection_rule_entry_p;
  struct   nsrm_nschainset_object           *nschainset_object_p;

  uint64_t selection_rule_handle , nschainset_object_handle;
  int32_t ret_val = NSRM_SUCCESS;
  
  CNTLR_RCU_READ_LOCK_TAKE();

  do
  {
    ret_val = nsrm_get_nschainset_object_handle(nschainset_object_name_p,&nschainset_object_handle);
    if(ret_val != NSRM_SUCCESS)
      break;
    ret_val = nsrm_get_nschainset_byhandle(nschainset_object_handle,&nschainset_object_p);
    if(ret_val != NSRM_SUCCESS)
      break;
    ret_val = nsrm_get_selection_rule_handle(nschain_selection_rule_name_p,nschainset_object_p,&selection_rule_handle);
    if(ret_val != NSRM_SUCCESS)
      break;
    ret_val = nsrm_get_selection_rule_byhandle(nschainset_object_p,selection_rule_handle,&selection_rule_entry_p);
    if(ret_val != NSRM_SUCCESS)
      break;
   
    ret_val = nsrm_get_first_attribute(&(selection_rule_entry_p->attributes), attribute_output_p);
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

int32_t nsrm_get_next_selection_rule_attribute(
          char*   nschain_selection_rule_name_p,
          char*   nschainset_object_name_p,
          char*   current_attribute_name_p,
          struct  nsrm_attribute_name_value_output_info *attribute_output_p) 
{
  struct   nsrm_nschain_selection_rule      *selection_rule_entry_p;
  struct   nsrm_nschainset_object           *nschainset_object_p;
 
  uint64_t selection_rule_handle , nschainset_object_handle;
  int32_t ret_val = NSRM_SUCCESS;
  
  CNTLR_RCU_READ_LOCK_TAKE();

  do
  {
    ret_val = nsrm_get_nschainset_object_handle(nschainset_object_name_p,&nschainset_object_handle);
    if(ret_val != NSRM_SUCCESS)
      break;
    ret_val = nsrm_get_nschainset_byhandle(nschainset_object_handle,&nschainset_object_p);
    if(ret_val != NSRM_SUCCESS)
      break;
    ret_val = nsrm_get_selection_rule_handle(nschain_selection_rule_name_p,nschainset_object_p,&selection_rule_handle);
    if(ret_val != NSRM_SUCCESS)
      break;
    ret_val = nsrm_get_selection_rule_byhandle(nschainset_object_p,selection_rule_handle,&selection_rule_entry_p);
    if(ret_val != NSRM_SUCCESS)
      break;
   
    ret_val = nsrm_get_next_attribute(&(selection_rule_entry_p->attributes),current_attribute_name_p,
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

int32_t nsrm_get_exact_selection_rule_attribute(
          char*  nschain_selection_rule_name_p,
          char*  nschainset_object_name_p,
          char*  attribute_name_p,
          struct nsrm_attribute_name_value_output_info *attribute_output_p
          )
{
  struct   nsrm_nschain_selection_rule      *selection_rule_entry_p;
  struct   nsrm_nschainset_object           *nschainset_object_p;

  uint64_t selection_rule_handle , nschainset_object_handle;
  int32_t ret_val = NSRM_SUCCESS;
  
  CNTLR_RCU_READ_LOCK_TAKE();

  do
  {
    ret_val = nsrm_get_nschainset_object_handle(nschainset_object_name_p,&nschainset_object_handle);
    if(ret_val != NSRM_SUCCESS)
      break;
    ret_val = nsrm_get_nschainset_byhandle(nschainset_object_handle,&nschainset_object_p);
    if(ret_val != NSRM_SUCCESS)
      break;
    ret_val = nsrm_get_selection_rule_handle(nschain_selection_rule_name_p,nschainset_object_p,&selection_rule_handle);
    if(ret_val != NSRM_SUCCESS)
      break;
    ret_val = nsrm_get_selection_rule_byhandle(nschainset_object_p,selection_rule_handle,&selection_rule_entry_p);
    if(ret_val != NSRM_SUCCESS)
      break;
   
    ret_val = nsrm_get_exact_attribute(&(selection_rule_entry_p->attributes),attribute_name_p,attribute_output_p);
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
int32_t nsrm_create_selection_rule_view(char *view_name)
{
  int32_t index = 0, ii;
  uint8_t empty_index_b = 0;

  for(ii = 0;ii<NSRM_MAX_VIEW_DATABASE ; ii++)
  {
     if(!strcmp(selection_rule_db_view[ii].view , view_name))
     {
        OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"View already created");
        return NSRM_FAILURE;
     } 
     else if((empty_index_b == 0) && (!strcmp(selection_rule_db_view[ii].view ,"")))    
     {
        empty_index_b = 1;
        index = ii;
     }
  } 
  strcpy(selection_rule_db_view[index].view,view_name);
  nsrm_view_hash_table_init(&selection_rule_db_view[index].db_viewtbl_p);
  selection_rule_db_view[index].uniqueness_flag = FALSE; 
  selection_rule_db_view[index].valid_b = TRUE;
 
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "View node created succesfully");
  return NSRM_SUCCESS;
}
  

int32_t nsrm_create_selection_rule_view_entry(uint64_t selection_handle , char* view_name ,char* view_value)
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
       if(!strcmp(selection_rule_db_view[view_index].view ,view_name))
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
     
     hashkey = nsrm_get_hashval_byname(view_value,nsrm_no_of_selection_rule_buckets_g);
     CNTLR_RCU_READ_LOCK_TAKE();
     view_node_entry_p = nsrm_alloc_view_entry_and_set_values(selection_handle, view_name, view_value);
     if (view_node_entry_p == NULL)
     {
       status = NSRM_FAILURE;
       break;
     }
     MCHASH_APPEND_NODE(selection_rule_db_view[view_index].db_viewtbl_p,hashkey,view_node_entry_p,index,magic,view_node_entry_p,status_b);
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

 
int32_t nsrm_remove_selection_rule_view_entry(char* view_name,char* view_value)
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
       if(!strcmp(selection_rule_db_view[index].view,view_name))
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
     hashkey = nsrm_get_hashval_byname(view_value,nsrm_no_of_selection_rule_buckets_g);
     CNTLR_RCU_READ_LOCK_TAKE(); 
     MCHASH_BUCKET_SCAN(selection_rule_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
     {
      if ((!strcmp(view_node_entry_p->view_value, view_value)) &&
          (!strcmp(view_node_entry_p->node_name, view_name)))
      {
        status_b = FALSE;
        offset = 0;
        MCHASH_DELETE_NODE_BY_REF(selection_rule_db_view[index].db_viewtbl_p,view_node_entry_p->index,view_node_entry_p->magic,struct database_view_node *,offset,status_b);
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
 
int32_t nsrm_remove_selection_rule_view(char *view_name)
{
  int32_t  index , ret_val;
  uint8_t  node_found_b = 0 ;
  uint64_t hashkey ,offset ;
  struct   database_view_node *view_node_entry_p;

  for(index = 0;index<NSRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(selection_rule_db_view[index].view , view_name))
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
  MCHASH_TABLE_SCAN(selection_rule_db_view[index].db_viewtbl_p,hashkey)
  {
    MCHASH_BUCKET_SCAN(selection_rule_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
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
  ret_val = nsrm_view_hash_table_deinit(selection_rule_db_view[index].db_viewtbl_p);
  if(ret_val != NSRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_ERROR,"Hash Table deinit failed");
    CNTLR_RCU_READ_LOCK_RELEASE();
    return NSRM_FAILURE;
  } 
  strcpy(selection_rule_db_view[index].view,"");
  selection_rule_db_view[index].uniqueness_flag = FALSE; 
  selection_rule_db_view[index].valid_b = FALSE;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "View node deleted succesfully");
  CNTLR_RCU_READ_LOCK_RELEASE();
  return NSRM_SUCCESS;
}


int32_t nsrm_get_first_selection_rule_view_entry(char *view_name , struct database_view_node *srule_view_node)
{
  int32_t  index;
  uint8_t  node_found_b = 0 ;
  int32_t  hashkey ,offset ;
  struct   database_view_node *view_node_entry_p;

  for(index = 0;index<NSRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(selection_rule_db_view[index].view , view_name))
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
  MCHASH_TABLE_SCAN(selection_rule_db_view[index].db_viewtbl_p ,hashkey)
  {
    MCHASH_BUCKET_SCAN(selection_rule_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
    {
      strcpy(srule_view_node->view_value , view_node_entry_p->view_value);
      strcpy(srule_view_node->node_name , view_node_entry_p->node_name);
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
   
int32_t nsrm_get_next_selection_rule_view_entry(char *view_name , char *view_value ,struct database_view_node *srule_view_node)

{
  int32_t  index;
  uint8_t  node_found_b = 0 ;
  int32_t  hashkey ,offset ;
  struct   database_view_node *view_node_entry_p;

  for(index = 0;index<NSRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(selection_rule_db_view[index].view , view_name))
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
  MCHASH_TABLE_SCAN(selection_rule_db_view[index].db_viewtbl_p , hashkey)
  {
    MCHASH_BUCKET_SCAN(selection_rule_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
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
      strcpy(srule_view_node->view_value , view_node_entry_p->view_value);
      strcpy(srule_view_node->node_name , view_node_entry_p->node_name);
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

int32_t nsrm_get_exact_selection_rule_view_entry(char *view_name , char *view_value ,struct database_view_node *srule_view_node)

{
  int32_t  index;
  uint8_t  node_found_b = 0 ;
  int32_t  hashkey ,offset ;
  struct   database_view_node *view_node_entry_p;

  for(index = 0;index<NSRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(selection_rule_db_view[index].view , view_name))
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
  MCHASH_TABLE_SCAN(selection_rule_db_view[index].db_viewtbl_p ,hashkey)
  {
    MCHASH_BUCKET_SCAN(selection_rule_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
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
  strcpy(srule_view_node->view_value , view_node_entry_p->view_value);
  strcpy(srule_view_node->node_name , view_node_entry_p->node_name);
  OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Node entry found");
  return NSRM_SUCCESS;
}
int32_t nsrm_get_selection_rule_by_view(char* nschainset_object_name ,char *view_name , char *view_value ,struct   nsrm_nschain_selection_rule_record* recs_p)
{
  int32_t  index;
  uint64_t nschainset_object_handle;
  uint8_t  node_found_b = 0 ;
  int32_t  hashkey ,offset ,ret_val;
  struct   database_view_node              *view_node_entry_p;
  struct   nsrm_nschainset_object          *nschainset_object_node_p;
  struct   nsrm_nschain_selection_rule     *selection_rule_node_p;

  for(index = 0;index<NSRM_MAX_VIEW_DATABASE ; index++)
  {
     if(!strcmp(selection_rule_db_view[index].view , view_name))
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
  MCHASH_TABLE_SCAN(selection_rule_db_view[index].db_viewtbl_p ,hashkey)
  {
    MCHASH_BUCKET_SCAN(selection_rule_db_view[index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
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
  ret_val = nsrm_get_nschainset_object_handle(nschainset_object_name , &nschainset_object_handle);
  if(ret_val != NSRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Chainset not found");
    CNTLR_RCU_READ_LOCK_RELEASE();
    return NSRM_FAILURE;
  }
  ret_val = nsrm_get_nschainset_byhandle(nschainset_object_handle , &nschainset_object_node_p);
  if(ret_val != NSRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Chainset not found");
    CNTLR_RCU_READ_LOCK_RELEASE();
    return NSRM_FAILURE;
  }
  ret_val = nsrm_get_selection_rule_byhandle(nschainset_object_node_p ,view_node_entry_p->node_saferef, 
                                             &selection_rule_node_p);
  if(ret_val != NSRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"Node not found");
    CNTLR_RCU_READ_LOCK_RELEASE();
    return NSRM_FAILURE;
  }
  strcpy(recs_p->key.name_p , selection_rule_node_p->name_p);
  strcpy(recs_p->info[8].value.nschain_object_name_p , selection_rule_node_p->nschain_object_name_p);
  if(selection_rule_node_p->relative_name_p != NULL)
  {
	  strcpy(recs_p->info[9].value.relative_name_p , selection_rule_node_p->relative_name_p);
  }
  recs_p->info[0].value.src_mac.mac_address_type_e = selection_rule_node_p->src_mac.mac_address_type_e;
  memcpy(recs_p->info[0].value.src_mac.mac,selection_rule_node_p->src_mac.mac,6);
  recs_p->info[1].value.dst_mac.mac_address_type_e = selection_rule_node_p->dst_mac.mac_address_type_e;
  memcpy(recs_p->info[1].value.dst_mac.mac,selection_rule_node_p->dst_mac.mac,6);
  recs_p->info[2].value.eth_type.ethtype_type_e    = selection_rule_node_p->eth_type.ethtype_type_e;
  recs_p->info[2].value.eth_type.ethtype           = selection_rule_node_p->eth_type.ethtype;
  recs_p->info[3].value.sip.ip_object_type_e       = selection_rule_node_p->sip.ip_object_type_e;
  recs_p->info[3].value.sip.ipaddr_start           = selection_rule_node_p->sip.ipaddr_start;
  recs_p->info[3].value.sip.ipaddr_end             = selection_rule_node_p->sip.ipaddr_end;
  recs_p->info[4].value.dip.ip_object_type_e       = selection_rule_node_p->dip.ip_object_type_e;
  recs_p->info[4].value.dip.ipaddr_start           = selection_rule_node_p->dip.ipaddr_start;
  recs_p->info[4].value.dip.ipaddr_end             = selection_rule_node_p->dip.ipaddr_end;
  recs_p->info[5].value.sp.port_object_type_e      = selection_rule_node_p->sp.port_object_type_e;
  recs_p->info[5].value.sp.port_start              = selection_rule_node_p->sp.port_start;
  recs_p->info[5].value.sp.port_end                = selection_rule_node_p->sp.port_end;
  recs_p->info[6].value.dp.port_object_type_e      = selection_rule_node_p->dp.port_object_type_e;
  recs_p->info[6].value.dp.port_start              = selection_rule_node_p->dp.port_start;
  recs_p->info[6].value.dp.port_end                = selection_rule_node_p->dp.port_end;
  recs_p->info[7].value.ip_protocol                = selection_rule_node_p->ip_protocol;
  recs_p->key.location_e                           = selection_rule_node_p->location_e;
  recs_p->info[10].value.admin_status_e            = selection_rule_node_p->admin_status_e;

  CNTLR_RCU_READ_LOCK_RELEASE();
  OF_LOG_MSG(OF_LOG_NSRM,OF_LOG_DEBUG,"selection rule found by view");
  return NSRM_SUCCESS;
}

 


