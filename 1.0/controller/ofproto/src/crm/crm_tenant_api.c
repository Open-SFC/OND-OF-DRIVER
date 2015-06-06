/** Cloud resource manager tenant source file 
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
 * File name: crm_tenant_api.c
 * Author: Varun Kumar Reddy <B36461@freescale.com>
 * Date:   11/10/2013
 * Description: 
*/


#include "crm_system_dependencies.h"
#include "crm_tenant_api.h"

extern void     *crm_tenant_mempool_g;
extern void     *crm_tenant_ns_chain_mempool_g;
extern void     *crm_tenant_vnnames_mempool_g;
extern void     *crm_notifications_mempool_g;
extern uint32_t crm_no_of_tenant_buckets_g;
extern struct   mchash_table* crm_tenant_table_g;

extern of_list_t crm_tenant_notifications[];
//void crm_attributes_free_attribute_entry_rcu(struct rcu_head *attribute_entry_p);
extern void     *nsrm_tenant_ns_chain_mempool_g;


void crm_tenant_vnnames_handle_free_entry_rcu(struct rcu_head *vn_names_entry_p);

#ifdef NSRM
void crm_tenant_ns_chain_free_entry_rcu(struct rcu_head *service_chain_entry_p)
{
  struct  crm_tenant_ns_chain_list *ns_chain_info_p=NULL;
  int32_t ret_value;

  if(service_chain_entry_p)
  {
    ns_chain_info_p = (struct crm_tenant_ns_chain_list *)(((char *)ns_chain_info_p) - sizeof(of_list_node_t));
    ret_value = mempool_release_mem_block(nsrm_tenant_ns_chain_mempool_g,
        (uchar8_t*)ns_chain_info_p,
        ns_chain_info_p->heap_b);
    if(ret_value != MEMPOOL_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Failed to release  memory block");
    }
    else
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "node deleted succesfully from the database");
    }
  }
  else
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "NULL passed for deletion");
  }
}


#endif

/**************************************************************************
Function:crm_get_tenant_handle
Description:
Returns tenant handle for given tenantname.
 **************************************************************************/
int32_t crm_get_tenant_handle(char *tenant_name_p, uint64_t *tenant_handle_p)
{
  uint32_t hashkey;
  uint32_t offset;
  struct crm_tenant *tenant_entry_p = NULL;

  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "crm_get_tenant_handle");
  if(crm_tenant_table_g == NULL)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "There is no tenant hash table!.");
    return CRM_FAILURE;
  }

  hashkey = crm_get_hashval_byname(tenant_name_p, crm_no_of_tenant_buckets_g);
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "bucket = %d",hashkey);


  CNTLR_RCU_READ_LOCK_TAKE();
  offset = TENANT_NODE_TENANT_TBL_OFFSET;
  MCHASH_BUCKET_SCAN(crm_tenant_table_g, hashkey, tenant_entry_p,struct crm_tenant*,offset)
  {
    if(strcmp(tenant_entry_p->tenant_name, tenant_name_p))
      continue;

    *tenant_handle_p = tenant_entry_p->magic;
    *tenant_handle_p = ((*tenant_handle_p<<32) | (tenant_entry_p->index));

    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "tenant name found and tenant handle:%llx",*tenant_handle_p);

    CNTLR_RCU_READ_LOCK_RELEASE();
    return CRM_SUCCESS;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "tenant  name not found!.");
  return CRM_ERROR_TENANT_NAME_INVALID;
}
/*************************************************************************************
Function:get_tenant_byhandle
Description:
        This function returns virtual network info for given vn_handle.
*************************************************************************************/
int32_t get_tenant_byhandle(uint64_t tenant_handle,struct crm_tenant **crm_tenant_info_p)
{
  /* Caller shall take RCU locks. */
  uint32_t index,magic;
  uint8_t status_b;

  magic = (uint32_t)(tenant_handle >>32);
  index = (uint32_t)tenant_handle;

  MCHASH_ACCESS_NODE(crm_tenant_table_g,index,magic,*crm_tenant_info_p,status_b);
  if(TRUE == status_b)
    return CRM_SUCCESS;
  return CRM_ERROR_INVALID_TENANT_HANDLE;
}

/*****************************************************************************
Function: crm_add_tenant
******************************************************************************/
int32_t crm_add_tenant(struct crm_tenant_config_info *crm_tenant_config_info_p, 
	      	       uint64_t *output_tenant_handle_p)
{

  uint32_t  hashkey ,index = 0,magic = 0 ;
  uchar8_t  tenantoffset;
  uchar8_t  apphookoffset;
  int32_t   ret_value;	
  uint8_t   heap_b, status_b = FALSE;
  uchar8_t* hashobj_p = NULL;

  struct crm_tenant *crm_tenant_node_p=NULL;
  struct crm_tenant *crm_tenant_node_scan_p=NULL;     
  struct crm_tenant_notification_data  notification_data={};
  struct crm_notification_app_hook_info *app_entry_p=NULL;


  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Entered");

  if(crm_tenant_table_g ==  NULL)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "no tenant hash table!.")
    return CRM_FAILURE;
  }

  if((crm_tenant_config_info_p->tenant_name == NULL)||
     (strlen(crm_tenant_config_info_p->tenant_name)>=CRM_MAX_TENANT_NAME_LEN)
    )
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "tenant name is invalid");
    return CRM_ERROR_TENANT_NAME_INVALID;
  }

  CNTLR_RCU_READ_LOCK_TAKE();

  /** calculate hash key for the sub network name */
  hashkey = crm_get_hashval_byname(crm_tenant_config_info_p->tenant_name, 
                                   crm_no_of_tenant_buckets_g);
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "bucket = %d",hashkey);

  tenantoffset = TENANT_NODE_TENANT_TBL_OFFSET;

  /** Scan tenant hash table for the name */ 
  MCHASH_BUCKET_SCAN(crm_tenant_table_g, hashkey, 
		     crm_tenant_node_scan_p, 
                     struct crm_tenant*, 
                     tenantoffset
                    )
  {
    if(strcmp(crm_tenant_config_info_p->tenant_name, crm_tenant_node_scan_p->tenant_name) != 0)
			continue;
		OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Duplicate tenantwork name(%s)", crm_tenant_config_info_p->tenant_name);
		ret_value = CRM_ERROR_DUPLICATE_TENANT_RESOURCE;
		goto CRM_ADD_TENANT_EXIT;
  }

  /** Create memory for the entry */
  ret_value = mempool_get_mem_block(crm_tenant_mempool_g, 
			            (uchar8_t **)&crm_tenant_node_p, 
                                    &heap_b
                                   );
  if(ret_value != MEMPOOL_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Failed to get memory block");
    goto CRM_ADD_TENANT_EXIT;
  } 

  crm_tenant_node_p->heap_b = heap_b;
  /** Copy Tenant info to the tenant database **/
  strncpy(crm_tenant_node_p->tenant_name, crm_tenant_config_info_p->tenant_name, CRM_MAX_TENANT_NAME_LEN);

  hashobj_p=(uchar8_t *)crm_tenant_node_p + TENANT_NODE_TENANT_TBL_OFFSET;
  MCHASH_APPEND_NODE(crm_tenant_table_g, hashkey, crm_tenant_node_p, index, magic, hashobj_p, status_b);
  if(FALSE == status_b)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG," Failed to append tenant node to Hash table");
    ret_value = CRM_ERROR_TENANT_ADD_FAIL;
    goto CRM_ADD_TENANT_EXIT;
  }

  *output_tenant_handle_p = magic;
  *output_tenant_handle_p = ((*output_tenant_handle_p <<32) | (index));

  crm_tenant_node_p->magic = magic;
  crm_tenant_node_p->index = index;
  (crm_tenant_node_p->tenant_handle) = *output_tenant_handle_p;


 OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "handle:%llx",crm_tenant_node_p->tenant_handle);

  OF_LIST_INIT(crm_tenant_node_p->list_of_virtual_networks, crm_tenant_vnnames_handle_free_entry_rcu);
  #ifdef NSRM
  OF_LIST_INIT(crm_tenant_node_p->list_of_ns_chains, crm_tenant_ns_chain_free_entry_rcu);
  #endif
  OF_LIST_INIT(crm_tenant_node_p->attributes, crm_attributes_free_attribute_entry_rcu);


  apphookoffset = CRM_NOTIFICATION_APP_HOOK_OFFSET;
  strcpy(notification_data.tenant_name, crm_tenant_node_p->tenant_name);

  OF_LIST_SCAN(crm_tenant_notifications[CRM_TENANT_ADDED], 
	       app_entry_p, 
               struct crm_notification_app_hook_info *, 
               apphookoffset )
  {
    ((crm_tenant_notifier_fn)(app_entry_p->call_back))(CRM_TENANT_ADDED,
    					               *output_tenant_handle_p,
					                notification_data,
					                app_entry_p->cbk_arg1,
					                app_entry_p->cbk_arg2
						      );
  }

  OF_LIST_SCAN(crm_tenant_notifications[CRM_TENANT_ALL_NOTIFICATIONS], 
	       app_entry_p, 
	       struct crm_notification_app_hook_info *, 
	       apphookoffset )
  {
    ((crm_tenant_notifier_fn)(app_entry_p->call_back))(CRM_TENANT_ADDED,
    					               *output_tenant_handle_p,
					                notification_data,
					                app_entry_p->cbk_arg1,
					                app_entry_p->cbk_arg2
						      );
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Tenant added successfully to the hashtable.");
  return CRM_SUCCESS;

CRM_ADD_TENANT_EXIT:
  if(crm_tenant_node_p)
  {
    if(mempool_release_mem_block(crm_tenant_mempool_g, 
				(uchar8_t *)crm_tenant_node_p,
				crm_tenant_node_p->heap_b)!=MEMPOOL_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Failed to release  allocated memory block.");
    }

  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return ret_value;
}
/***********************************************************************************************/
void crm_tenant_vnnames_handle_free_entry_rcu(struct rcu_head *vn_names_entry_p)
{       
  struct  crm_tenant_vn_list *vn_names_info_p=NULL;
  int32_t ret_value;    
        
  if(vn_names_entry_p)
  {
    vn_names_info_p = (struct crm_tenant_vn_list *)(((char *)vn_names_entry_p) - sizeof(of_list_node_t));
    ret_value = mempool_release_mem_block(crm_tenant_vnnames_mempool_g,
                                                      (uchar8_t*)vn_names_info_p,
                                                       vn_names_info_p->heap_b);
    if(ret_value != MEMPOOL_SUCCESS)
    {   
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Failed to release  memory block");
    }
    else
    {   
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "node deleted succesfully from the database");
    }   
  }     
  else  
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "NULL passed for deletion");
  }
}
/**********************************************************************************/
int32_t crm_add_vn_to_tenant(uint64_t tenant_handle, char *vn_name_p)
{
  int32_t   ret_value=CRM_FAILURE;
  uint32_t  index,magic;
  uint8_t   heap_b, status_b = FALSE;
 
  struct crm_tenant *tenant_info_p=NULL;
  struct crm_tenant_vn_list  *vn_list_p=NULL;
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Entered");

  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    magic = (uint32_t)(tenant_handle >>32);
    index = (uint32_t)tenant_handle;

    MCHASH_ACCESS_NODE(crm_tenant_table_g,index,magic, tenant_info_p, status_b);
    if(TRUE != status_b)   
      return CRM_ERROR_INVALID_TENANT_HANDLE;


    ret_value = mempool_get_mem_block(crm_tenant_vnnames_mempool_g,
                                        (uchar8_t**)&vn_list_p,
                                        &heap_b);
    if(ret_value != MEMPOOL_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "failed to get mem block.");
      ret_value = CRM_FAILURE;
      break;
    }
    vn_list_p->heap_b=heap_b;
 
    strcpy(vn_list_p->nw_name, vn_name_p);
    vn_list_p->tenant_entry_node_p = tenant_info_p;
    OF_APPEND_NODE_TO_LIST((tenant_info_p->list_of_virtual_networks),
                                vn_list_p,
                                CRM_TENANT_VN_LIST_HANDLE_ENTRY_LISTNODE_OFFSET);
    tenant_info_p->number_of_virtual_networks++;
    ret_value=CRM_SUCCESS;
  }while(0);
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Added vn to the tenant list successfully!."); 
  CNTLR_RCU_READ_LOCK_RELEASE();
  return ret_value;
}

/**********************************************************************************/
int32_t crm_del_vn_from_tenant(uint64_t tenant_handle, char *vn_name_p)
{
  int32_t   ret_val=CRM_FAILURE;
  uint32_t  index,magic,node_found=0;
  uint8_t   status_b = FALSE;
  
  struct crm_tenant *tenant_info_p=NULL;
  struct crm_tenant_vn_list  *vn_list_scan_p=NULL, *vn_list_prev_p=NULL;
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Entered");

  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    magic = (uint32_t)(tenant_handle >>32);
    index = (uint32_t)tenant_handle;

    MCHASH_ACCESS_NODE(crm_tenant_table_g,index,magic, tenant_info_p, status_b);
    if(TRUE != status_b)   
      return CRM_ERROR_INVALID_TENANT_HANDLE;

    OF_LIST_SCAN((tenant_info_p->list_of_virtual_networks), vn_list_scan_p, struct crm_tenant_vn_list *,
					                CRM_TENANT_VN_LIST_HANDLE_ENTRY_LISTNODE_OFFSET)
    {
      if(!strcmp(vn_list_scan_p->nw_name, vn_name_p))
      {
        OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "node  found to delete");
        node_found=1;
        break;
      }
      vn_list_prev_p = vn_list_scan_p;
    }
    if(node_found == 0)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "node not found to delete");
      ret_val=CRM_FAILURE;
      break;
    }

    OF_REMOVE_NODE_FROM_LIST((tenant_info_p->list_of_virtual_networks),
                              vn_list_scan_p,
	                      vn_list_prev_p,
	                     CRM_TENANT_VN_LIST_HANDLE_ENTRY_LISTNODE_OFFSET);
    tenant_info_p->number_of_virtual_networks--;	  
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "vn deleted successfully from the tenant");
    ret_val=CRM_SUCCESS;
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  return ret_val;
}
/***************************************************************************
Function:crm_del_tenant
**************************************************************************/

int32_t  crm_del_tenant(char* tenant_name_p)
{
  struct   crm_tenant   *tenant_info_p=NULL;
  uint32_t  lstoffset,index,magic,offset;
  uint8_t  status_b = FALSE;
  uint64_t tenant_handle;
  struct crm_tenant_notification_data notification_data;
  struct crm_notification_app_hook_info *app_entry_p=NULL;


  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Entered");
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "tenant_name:%s",tenant_name_p);

  if(crm_tenant_table_g ==  NULL)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "There is no tenant hash table!.")
    return CRM_FAILURE;
  }

  if((tenant_name_p == NULL))
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Tenantwork name is invalid");
    return CRM_ERROR_TENANT_NAME_INVALID;
  }

  if(crm_get_tenant_handle(tenant_name_p, &tenant_handle) !=CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "tenant doesn't exists with name %s",tenant_name_p);
    return CRM_ERROR_TENANT_NAME_INVALID;	
  }

  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "tenant_handle:%llx",tenant_handle);

  magic = (uint32_t)(tenant_handle >>32);
  index = (uint32_t)tenant_handle;



  CNTLR_RCU_READ_LOCK_TAKE();
  MCHASH_ACCESS_NODE(crm_tenant_table_g,index,magic,tenant_info_p,status_b);
  if(TRUE == status_b)
  {
    if((tenant_info_p->number_of_virtual_networks)|| (tenant_info_p->number_of_attributes))
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG," Delete all the vns and attributes associated with the tenant before deleting the tenant.");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return CRM_ERROR_RESOURCE_NOTEMPTY;
    }
  
   #ifdef NSRM_SUPPORT
   if(tenant_info_p->number_of_ns_chains)
   {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG," Delete all  ns chains  associated with the tenant before deleting the tenant.");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return CRM_ERROR_RESOURCE_NOTEMPTY;
    }
#endif

   /* Send sw delete notification to the registered Applications. */
    strcpy(notification_data.tenant_name, tenant_info_p->tenant_name);

    lstoffset = CRM_NOTIFICATION_APP_HOOK_OFFSET;
    OF_LIST_SCAN(crm_tenant_notifications[CRM_TENANT_DELETE],app_entry_p,struct crm_notification_app_hook_info*,lstoffset )
    {
      ((crm_tenant_notifier_fn)(app_entry_p->call_back))(CRM_TENANT_DELETE,
                                                          tenant_handle,
                                                          notification_data,
                                                          app_entry_p->cbk_arg1,
                                                          app_entry_p->cbk_arg2);
    }

    OF_LIST_SCAN(crm_tenant_notifications[CRM_TENANT_ALL_NOTIFICATIONS],app_entry_p,struct crm_notification_app_hook_info*,lstoffset )
    {
      ((crm_tenant_notifier_fn)(app_entry_p->call_back))(CRM_TENANT_DELETE,
                                                          tenant_handle,
                                                          notification_data,
                                                          app_entry_p->cbk_arg1,
                                                          app_entry_p->cbk_arg2);
    }

    
    status_b = FALSE;
    offset = TENANT_NODE_TENANT_TBL_OFFSET;
    MCHASH_DELETE_NODE_BY_REF(crm_tenant_table_g,index,magic,struct crm_tenant_entry *,offset,status_b);
    CNTLR_RCU_READ_LOCK_RELEASE();
    if(status_b == TRUE)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "tenant deleted successfully");
      return CRM_SUCCESS;
    }
    else
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "failed to delete tenant");
      return CRM_FAILURE;
    }
 }
  CNTLR_RCU_READ_LOCK_RELEASE();
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "invalid  tenant handle");
  return CRM_ERROR_INVALID_TENANT_HANDLE;
}
/***************************************************************************/
int32_t crm_get_tenant_by_name(char* tenant_name_p,
		struct  crm_tenant_config_info*  crm_tenant_config_info_p,
		struct  tenant_runtime_info  *runtime_info)
{
	int32_t ret_val=CRM_FAILURE;
	uint8_t tenant_node_found=0;
	struct    crm_tenant  *crm_tenant_node_scan_p=NULL;
	uint32_t  hashkey, offset;

	if(crm_tenant_table_g == NULL)
		return CRM_FAILURE;

	if(tenant_name_p ==  NULL)
		return CRM_ERROR_TENANT_NAME_INVALID;

	OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "tenant_name %s",tenant_name_p);
	CNTLR_RCU_READ_LOCK_TAKE();

	/** calculate hash key for the Virtual network name */
	hashkey = crm_get_hashval_byname(tenant_name_p, crm_no_of_tenant_buckets_g);
	OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "bucket = %d",hashkey);

	offset = TENANT_NODE_TENANT_TBL_OFFSET;

	/** Scan tenant hash table for the name */
	MCHASH_BUCKET_SCAN(crm_tenant_table_g, hashkey, crm_tenant_node_scan_p, struct crm_tenant*, offset)
	{
		if(!strcmp(tenant_name_p, crm_tenant_node_scan_p->tenant_name))
		{
			OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Tenant  found");
			tenant_node_found = 1;
			break;
		}

	}
	if(tenant_node_found == 1)
	{
		OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Copying Tenant Info...");
		strncpy(crm_tenant_config_info_p->tenant_name, crm_tenant_node_scan_p->tenant_name,CRM_MAX_TENANT_NAME_LEN);
	
		ret_val = CRM_SUCCESS;

	}
	else
	{
		OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Tenant not found\r\n",__FUNCTION__, __LINE__);
		ret_val = CRM_ERROR_TENANT_NAME_INVALID;
	}
	CNTLR_RCU_READ_LOCK_RELEASE();
	return ret_val;
}
/***************************************************************************/
int32_t  crm_get_first_tenant (struct  crm_tenant_config_info*  crm_tenant_config_info_p,
		struct  tenant_runtime_info  *runtime_info,
		uint64_t *tenant_handle_p)
{
	struct    crm_tenant   *crm_tenant_node_scan_p=NULL;
	uint32_t hashkey;

	uint32_t offset;

	OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "crm_get_first_tenant");

	if(crm_tenant_table_g == NULL)
		return CRM_FAILURE;

	CNTLR_RCU_READ_LOCK_TAKE();

	MCHASH_TABLE_SCAN(crm_tenant_table_g, hashkey)
	{
		offset = TENANT_NODE_TENANT_TBL_OFFSET;
		MCHASH_BUCKET_SCAN(crm_tenant_table_g, hashkey, crm_tenant_node_scan_p, struct    crm_tenant *, offset)
		{
			OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Copying Tenant Info...");
			OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "tenant name:%s",crm_tenant_node_scan_p->tenant_name);
			strncpy(crm_tenant_config_info_p->tenant_name, crm_tenant_node_scan_p->tenant_name,CRM_MAX_TENANT_NAME_LEN);
			//runtime_info_p->number_of_attributes = crm_vm_node_info_p->number_of_attributes;

			*tenant_handle_p = crm_tenant_node_scan_p->magic;
			*tenant_handle_p = ((*tenant_handle_p <<32) | (crm_tenant_node_scan_p->index));

			CNTLR_RCU_READ_LOCK_RELEASE();
			return CRM_SUCCESS;
		}
	}       
	CNTLR_RCU_READ_LOCK_RELEASE();
	OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "No More Entries in the tenantwork hash table!.");
	return CRM_ERROR_NO_MORE_ENTRIES;
}
/***************************************************************************/
int32_t  crm_get_next_tenant (struct  crm_tenant_config_info*  crm_tenant_config_info_p,
		struct  tenant_runtime_info  *runtime_info,
		uint64_t *tenant_handle_p)
{
	struct    crm_tenant   *crm_tenant_node_scan_p=NULL;
	uint32_t hashkey;
	uint8_t current_entry_found_b;

	uint32_t offset = TENANT_NODE_TENANT_TBL_OFFSET;
	OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Entered");

	CNTLR_RCU_READ_LOCK_TAKE();
	current_entry_found_b = FALSE;

	MCHASH_TABLE_SCAN(crm_tenant_table_g, hashkey)
	{
		offset = TENANT_NODE_TENANT_TBL_OFFSET;
		MCHASH_BUCKET_SCAN(crm_tenant_table_g, hashkey, crm_tenant_node_scan_p, struct crm_tenant *,offset)
		{
			if(current_entry_found_b == FALSE)
			{
				if(*tenant_handle_p != (crm_tenant_node_scan_p->tenant_handle))
					continue;

				else
				{
					OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Tenant entry found copy next tenant.");
					current_entry_found_b = TRUE;
					continue;
				}

				/*Skip the First matching Switch and Get the next Switch */
			}
			OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Copying Tenant Info...");
			OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "tenant name:%s",crm_tenant_node_scan_p->tenant_name);
			
			strncpy(crm_tenant_config_info_p->tenant_name, crm_tenant_node_scan_p->tenant_name,CRM_MAX_TENANT_NAME_LEN);
			//runtime_info_p->number_of_attributes = crm_vm_node_info_p->number_of_attributes;

			*tenant_handle_p = crm_tenant_node_scan_p->magic;
			*tenant_handle_p = ((*tenant_handle_p <<32) | (crm_tenant_node_scan_p->index));


			CNTLR_RCU_READ_LOCK_RELEASE();
			return CRM_SUCCESS;
		}
	}
	CNTLR_RCU_READ_LOCK_RELEASE();
	OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Failed to copy Tenant Info.....");
	if(current_entry_found_b == TRUE)
		return CRM_ERROR_NO_MORE_ENTRIES;
	else
		return CRM_ERROR_INVALID_TENANT_HANDLE;
}
/***************************************************************************/
int32_t crm_register_tenant_notifications (uint32_t notification_type,
                                crm_tenant_notifier_fn tenant_notifier_fn,
                                void     *callback_arg1,
                                void     *callback_arg2)
{
  struct  crm_notification_app_hook_info *app_entry_p=NULL;
  uint8_t heap_b;
  int32_t retval = CRM_SUCCESS;
  uchar8_t lstoffset;
  lstoffset = CRM_NOTIFICATION_APP_HOOK_OFFSET;

  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Entered");
  if((notification_type < CRM_TENANT_FIRST_NOTIFICATION_TYPE) ||
     (notification_type > CRM_TENANT_LAST_NOTIFICATION_TYPE)
    )
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Invalid Notification type:%d", notification_type);
    return CRM_FAILURE;
  }

  if(tenant_notifier_fn == NULL)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "callback function is null");
    return CRM_FAILURE;
  }

  retval = mempool_get_mem_block(crm_notifications_mempool_g, (uchar8_t**)&app_entry_p, &heap_b);
  if(retval != MEMPOOL_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Failed to get mempool");
    return CRM_FAILURE;
  }

  app_entry_p->call_back = (void*)tenant_notifier_fn;
  app_entry_p->cbk_arg1  = callback_arg1;
  app_entry_p->cbk_arg2  = callback_arg2;
  app_entry_p->heap_b    = heap_b;

  /* Add Application to the list of tenant notifications */
  OF_APPEND_NODE_TO_LIST(crm_tenant_notifications[notification_type], app_entry_p, lstoffset);

  if(retval != CRM_SUCCESS)
    mempool_release_mem_block(crm_notifications_mempool_g, (uchar8_t*)app_entry_p, app_entry_p->heap_b);

  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "callback function reg with tenant success");
  return retval;
}
/****************************************************************************************************************/
int32_t crm_add_attribute_to_tenant(uint64_t tenant_handle, struct crm_attribute_name_value_pair* attribute_info_p)
{
  int32_t  retval;
  struct   crm_tenant            *tenant_entry_p;
  struct   crm_tenant_notification_data        notification_data={};
  struct   crm_notification_app_hook_info *app_entry_p;
  uchar8_t lstoffset;
  lstoffset = CRM_NOTIFICATION_APP_HOOK_OFFSET;


  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Entered");
  CNTLR_RCU_READ_LOCK_TAKE();

  retval = get_tenant_byhandle(tenant_handle, &tenant_entry_p);
  if(retval != CRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return retval;
  }

  retval = crm_add_attribute(&(tenant_entry_p->attributes),attribute_info_p);
  if(retval != CRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return retval;
  }

  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "added attr");
  tenant_entry_p->number_of_attributes++;
  of_create_crm_view_entry(tenant_handle, attribute_info_p->name_string, attribute_info_p->value_string);

  /* Send add attribute to switch notification to registered Applications. */
  strcpy(notification_data.tenant_name, tenant_entry_p->tenant_name);
  strcpy(notification_data.attribute_name,attribute_info_p->name_string);
  strcpy(notification_data.attribute_value,attribute_info_p->value_string);
  OF_LIST_SCAN(crm_tenant_notifications[CRM_TENANT_ATTRIBUTE_ADDED],app_entry_p,struct crm_notification_app_hook_info*,lstoffset )
  {
    ((crm_tenant_notifier_fn)(app_entry_p->call_back))(CRM_TENANT_ATTRIBUTE_ADDED,
                                                        tenant_handle,
                                                        notification_data,
                                                        app_entry_p->cbk_arg1,
                                                        app_entry_p->cbk_arg2);
  }

  OF_LIST_SCAN(crm_tenant_notifications[CRM_TENANT_ALL_NOTIFICATIONS],app_entry_p,struct crm_notification_app_hook_info*,lstoffset )
  {
    ((crm_tenant_notifier_fn)(app_entry_p->call_back))(CRM_TENANT_ATTRIBUTE_ADDED,
                                                        tenant_handle,
                                                        notification_data,
                                                        app_entry_p->cbk_arg1,
                                                        app_entry_p->cbk_arg2);
  }
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "success");
  CNTLR_RCU_READ_LOCK_RELEASE();
  return CRM_SUCCESS;
}


int32_t crm_delete_attribute_from_tenant(uint64_t tenant_handle, struct crm_attribute_name_value_pair *attribute_info_p)
{

  int32_t  retval;
  uint32_t attr_node_found=0;
  struct   crm_tenant            *tenant_entry_p;
  struct   crm_resource_attribute_entry *attribute_entry_p,*prev_attr_entry_p = NULL;

  struct   crm_tenant_notification_data   notification_data={};
  struct   crm_notification_app_hook_info *app_entry_p;
  uchar8_t lstattrnameoffset;
  lstattrnameoffset = CRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET;

  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Entered");

  CNTLR_RCU_READ_LOCK_TAKE();

  retval = get_tenant_byhandle(tenant_handle, &tenant_entry_p);
  if(retval != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "get_tenant_byhandle failed");
    CNTLR_RCU_READ_LOCK_RELEASE();
    return retval;
  }
   OF_LIST_SCAN(tenant_entry_p->attributes, attribute_entry_p, struct crm_resource_attribute_entry*,lstattrnameoffset)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "attribute_entry_p->name:%s",attribute_entry_p->name);
    if(!strcmp(attribute_entry_p->name, attribute_info_p->name_string))
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "attr name %s found to delete",attribute_entry_p->name);
      attr_node_found =1;
      break;
    }
    prev_attr_entry_p = attribute_entry_p;
  }

  if(attr_node_found == 0)
   {
     OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "attr name:%s not found to delete",attribute_info_p->name_string);
     CNTLR_RCU_READ_LOCK_RELEASE();
     return CRM_FAILURE;
   }

      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "attr name %s found to delete",attribute_entry_p->name);
      strcpy(notification_data.tenant_name, tenant_entry_p->tenant_name);
      strcpy(notification_data.attribute_name,attribute_entry_p->name);
      strcpy(notification_data.attribute_value,attribute_entry_p->value);
      OF_LIST_SCAN(crm_tenant_notifications[CRM_TENANT_ATTRIBUTE_DELETE],app_entry_p,struct crm_notification_app_hook_info*, CRM_NOTIFICATION_APP_HOOK_OFFSET)
            {
              ((crm_tenant_notifier_fn)(app_entry_p->call_back))(CRM_TENANT_ATTRIBUTE_DELETE,
                                                            tenant_handle,
                                                            notification_data,
                                                            app_entry_p->cbk_arg1,
                                                            app_entry_p->cbk_arg2);
           }

           OF_LIST_SCAN(crm_tenant_notifications[CRM_TENANT_ALL_NOTIFICATIONS],app_entry_p,struct crm_notification_app_hook_info*,CRM_NOTIFICATION_APP_HOOK_OFFSET)
      {
              ((crm_tenant_notifier_fn)(app_entry_p->call_back))(CRM_TENANT_ATTRIBUTE_DELETE,
                                                            tenant_handle,
                                                            notification_data,
                                                            app_entry_p->cbk_arg1,
                                                            app_entry_p->cbk_arg2);
           }


        OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "deleting attr %s",attribute_entry_p->name);
        OF_REMOVE_NODE_FROM_LIST(tenant_entry_p->attributes,attribute_entry_p,prev_attr_entry_p, lstattrnameoffset);
        tenant_entry_p->number_of_attributes--;
        of_remove_crm_view_entry(attribute_entry_p->name, attribute_entry_p->value);


        CNTLR_RCU_READ_LOCK_RELEASE();
        return CRM_SUCCESS;
}

/*******************************************************************************************************/
int32_t crm_get_tenant_first_attribute(uint64_t tenant_handle,
struct   crm_attribute_name_value_output_info *attribute_output_p)
{
  int32_t  retval;
  struct crm_tenant *crm_tenant_entry_p=NULL;
  if(attribute_output_p == NULL)
    return CRM_FAILURE;



  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    retval = get_tenant_byhandle(tenant_handle, &crm_tenant_entry_p);
    if(retval != CRM_SUCCESS)
    { 
      retval = CRM_ERROR_INVALID_TENANT_HANDLE;
      break;
    }
    retval = crm_get_first_attribute(&(crm_tenant_entry_p->attributes), attribute_output_p);

  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  return retval;
}
/*****************************************************************************************************/
int32_t crm_get_tenant_next_attribute(uint64_t tenant_handle,
      char*    current_attribute_name,
      struct   crm_attribute_name_value_output_info *attribute_output_p)
{
  int32_t retval;
  struct crm_tenant *crm_tenant_entry_p=NULL;

  if((attribute_output_p == NULL) || (current_attribute_name == NULL))
    return CRM_FAILURE;

  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    retval = get_tenant_byhandle(tenant_handle,&crm_tenant_entry_p);
    if(retval != CRM_SUCCESS)
    {
      retval= CRM_ERROR_INVALID_TENANT_HANDLE;
      break;
    }
    retval = crm_get_next_attribute(&(crm_tenant_entry_p->attributes), current_attribute_name, attribute_output_p);
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  return retval;
}
/******************************************************************************/
int32_t crm_get_tenant_exact_attribute(uint64_t tenant_handle,
      char *attribute_name_string ,struct   crm_attribute_name_value_output_info *attribute_output_p)
{
  struct   crm_tenant *crm_tenant_entry_p=NULL;
  int32_t  retval;

  if(attribute_name_string == NULL)
   return CRM_ERROR_ATTRIBUTE_NAME_NULL;

  if(strlen(attribute_name_string) > CRM_MAX_ATTRIBUTE_NAME_LEN)
    return CRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN;

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
     retval = get_tenant_byhandle(tenant_handle, &crm_tenant_entry_p );
     if(retval != CRM_SUCCESS)
     {
       retval= CRM_ERROR_INVALID_TENANT_HANDLE;
       break;	
     }
     retval = crm_get_exact_attribute(&(crm_tenant_entry_p->attributes),  attribute_name_string,attribute_output_p);
   }while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return retval;
}
