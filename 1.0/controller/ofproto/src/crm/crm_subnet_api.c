/** Cloud resource manager Virtual Network source file */
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

/*
 *
 * File name: crm_subnet_api.c
 * Author: Varun Kumar Reddy <B36461@freescale.com>
 * Date:   11/10/2013
 * Description: 
*/





#include "crm_system_dependencies.h"
#include "crm_subnet_api.h"

extern void     *crm_subnet_mempool_g;
extern uint32_t crm_no_of_subnet_buckets_g;
extern struct   mchash_table* crm_subnet_table_g;
extern of_list_t crm_subnet_notifications[];
//void crm_attributes_free_attribute_entry_rcu(struct rcu_head *attribute_entry_p);

/**************************************************************************
Function:crm_get_subnet_handle
Description:
Returns subnet handle for given subnetname.
 **************************************************************************/
int32_t crm_get_subnet_handle(char *subnet_name_p, uint64_t *subnet_handle_p)
{
  uint32_t hashkey;
  uint32_t offset;

  struct crm_subnet *subnet_entry_p = NULL;

  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "crm_get_subnet_handle");
  if(crm_subnet_table_g == NULL)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "There is no subnet hash table!.");
    return CRM_FAILURE;
  }

  hashkey = crm_get_hashval_byname(subnet_name_p, crm_no_of_subnet_buckets_g);
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "bucket = %d",hashkey);


  CNTLR_RCU_READ_LOCK_TAKE();
  offset = SUBNETNODE_SUBNETTBL_OFFSET;
  MCHASH_BUCKET_SCAN(crm_subnet_table_g, 
		             hashkey, 
		             subnet_entry_p,
                     struct crm_subnet*,
		             offset
		            )
  {
    if(strcmp(subnet_entry_p->subnet_name, subnet_name_p))
      continue;
    CNTLR_RCU_READ_LOCK_RELEASE();
    *subnet_handle_p = subnet_entry_p->magic;
    *subnet_handle_p = ((*subnet_handle_p<<32) | (subnet_entry_p->index));
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "subnet name found and subnet handle:%llx",*subnet_handle_p);
    return CRM_SUCCESS;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "subnet name not found!.");
  return CRM_ERROR_SUBNET_NAME_INVALID;
}
/*************************************************************************************
Function:get_subnet_byhandle
Description:
        This function returns virtual network info for given vn_handle.
*************************************************************************************/
int32_t get_subnet_byhandle(uint64_t subnet_handle,struct crm_subnet **crm_subnet_info_p)
{
  /* Caller shall take RCU locks. */
  uint32_t index,magic;
  uint8_t status_b;

  magic = (uint32_t)(subnet_handle >>32);
  index = (uint32_t)subnet_handle;

  MCHASH_ACCESS_NODE(crm_subnet_table_g,index,magic,*crm_subnet_info_p,status_b);
  if(TRUE == status_b)
    return CRM_SUCCESS;
  return CRM_ERROR_INVALID_SUBNET_HANDLE;
}

int32_t crm_add_subnet(struct crm_subnet_config_info *sub_config_info_p, 
		uint64_t *output_subnet_handle_p)

{

  struct crm_subnet *crm_subnet_node_p=NULL;
  struct crm_subnet *crm_subnet_node_scan_p=NULL;     
  uint32_t hashkey , index = 0, magic = 0 ;
  uint32_t subnetoffset;
  int32_t ret_value;	
  uint8_t   heap_b, status_b = FALSE;
  uchar8_t* hashobj_p = NULL;
  uint64_t vn_handle;

  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Entered");

  if(crm_subnet_table_g ==  NULL)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "There is no subnet hash table!.")
    return CRM_FAILURE;
  }

  if((sub_config_info_p->subnet_name == NULL)||
     (strlen(sub_config_info_p->subnet_name)>=CRM_MAX_SUBNET_NAME_LEN)
    )
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Subnetwork name is invalid");
    return CRM_ERROR_SUBNET_NAME_INVALID;
  }

  ret_value = crm_get_vn_handle(sub_config_info_p->vn_name, &vn_handle);
  if(ret_value!=CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Virtual network name is invalid");
    return CRM_ERROR_VN_NAME_INVALID;	
  }

  CNTLR_RCU_READ_LOCK_TAKE();

  /** calculate hash key for the sub network name */
  hashkey = crm_get_hashval_byname(sub_config_info_p->subnet_name, 
                             	   crm_no_of_subnet_buckets_g
			                      );
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "bucket = %d",hashkey);
  subnetoffset = SUBNETNODE_SUBNETTBL_OFFSET;

  /** Scan subnet hash table for the name */ 
  MCHASH_BUCKET_SCAN(crm_subnet_table_g, hashkey, crm_subnet_node_scan_p, struct crm_subnet*, subnetoffset)
  {
    if(strcmp(sub_config_info_p->subnet_name, crm_subnet_node_scan_p->subnet_name) != 0)
	continue;
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Duplicate subnetwork name(%s)", sub_config_info_p->subnet_name);
    ret_value = CRM_ERROR_DUPLICATE_SUBNET_RESOURCE;
    goto CRM_ADD_SUBNET_EXIT;
  }

  /** Create memory for the entry */
  ret_value = mempool_get_mem_block(crm_subnet_mempool_g, (uchar8_t **)&crm_subnet_node_p, &heap_b);
  if(ret_value != MEMPOOL_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Failed to allocate memory for subnetwork node");
    goto CRM_ADD_SUBNET_EXIT;
  } 

  crm_subnet_node_p->heap_b = heap_b;
  /** Copy Subnet info to the subnet database **/

  strncpy(crm_subnet_node_p->subnet_name, sub_config_info_p->subnet_name, CRM_MAX_SUBNET_NAME_LEN);
  strncpy(crm_subnet_node_p->nw_name, sub_config_info_p->vn_name, CRM_MAX_VN_NAME_LEN);
  crm_subnet_node_p->enable_dhcp_b = sub_config_info_p->enable_dhcp_b;

  crm_subnet_node_p->pool_start_address = sub_config_info_p->pool_start_address;
  crm_subnet_node_p->pool_end_address = sub_config_info_p->pool_end_address;
  crm_subnet_node_p->ip_version = sub_config_info_p->ip_version;
  crm_subnet_node_p->gateway_ip = sub_config_info_p->gateway_ip;
  crm_subnet_node_p->cidr_ip = sub_config_info_p->cidr_ip;
  crm_subnet_node_p->cidr_mask = sub_config_info_p->cidr_mask;

  memcpy(&(crm_subnet_node_p->dns_nameservers), &(sub_config_info_p->dns_nameservers), sizeof(sub_config_info_p->dns_nameservers));

  hashobj_p=(uchar8_t *)crm_subnet_node_p + SUBNETNODE_SUBNETTBL_OFFSET;

  MCHASH_APPEND_NODE(crm_subnet_table_g, hashkey, crm_subnet_node_p, index, magic, hashobj_p, status_b);
  if(FALSE == status_b)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR," Failed to append sub-network node to Hash table");
    ret_value = CRM_ERROR_SUBNET_ADD_FAIL;
    goto CRM_ADD_SUBNET_EXIT;
  }
  OF_LIST_INIT(crm_subnet_node_p->attributes, crm_attributes_free_attribute_entry_rcu);

  *output_subnet_handle_p = magic;
  *output_subnet_handle_p = ((*output_subnet_handle_p <<32) | (index));

  crm_subnet_node_p->magic = magic;
  crm_subnet_node_p->index = index;
  (crm_subnet_node_p->subnet_handle) = *output_subnet_handle_p;

  if(crm_attach_subnet_handle_to_vn(vn_handle, 
				    sub_config_info_p->subnet_name,
				    (crm_subnet_node_p->subnet_handle)
				   )!=CRM_SUCCESS)   
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Attach subnet handle to vn failed!.");
  }
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG,   "Attach subnet handle to vn successfully!.");
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG,   "Subnet Record added to the DB");
  CNTLR_RCU_READ_LOCK_RELEASE();
  return CRM_SUCCESS;


CRM_ADD_SUBNET_EXIT:
  if(crm_subnet_node_p)
  {
    if(mempool_release_mem_block(crm_subnet_mempool_g, 
				(uchar8_t *)crm_subnet_node_p,
				crm_subnet_node_p->heap_b)!=MEMPOOL_SUCCESS)
    {

      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Failed to release  allocated memory block.");
    }

  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return ret_value;
}
int32_t crm_modify_subnet(struct crm_subnet_config_info *sub_config_info_p, 
		uint64_t *output_subnet_handle_p)

{
  struct crm_subnet *crm_subnet_node_scan_p=NULL;     
  uint32_t hashkey;
  uint32_t subnetoffset,subnet_node_found=0;
  int32_t ret_value;	
  uint64_t vn_handle;

  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Entered");

  if(crm_subnet_table_g ==  NULL)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "There is no subnet hash table!.")
    return CRM_FAILURE;
  }

  if((sub_config_info_p->subnet_name == NULL)||
     (strlen(sub_config_info_p->subnet_name)>=CRM_MAX_SUBNET_NAME_LEN)
    )
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Subnetwork name is invalid");
    return CRM_ERROR_SUBNET_NAME_INVALID;
  }

  ret_value = crm_get_vn_handle(sub_config_info_p->vn_name, &vn_handle);
  if(ret_value!=CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Virtual network name is invalid");
    return CRM_ERROR_VN_NAME_INVALID;	
  }

  CNTLR_RCU_READ_LOCK_TAKE();

  /** calculate hash key for the sub network name */
  hashkey = crm_get_hashval_byname(sub_config_info_p->subnet_name, 
			crm_no_of_subnet_buckets_g
			);
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "bucket = %d",hashkey);
  subnetoffset = SUBNETNODE_SUBNETTBL_OFFSET;

  /** Scan subnet hash table for the name */ 
  MCHASH_BUCKET_SCAN(crm_subnet_table_g, hashkey, crm_subnet_node_scan_p, struct crm_subnet*, subnetoffset)
  {
    if(!strcmp(sub_config_info_p->subnet_name, crm_subnet_node_scan_p->subnet_name))
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "subnetwork name(%s) found to modify", sub_config_info_p->subnet_name);
      subnet_node_found =1;
    }
  }

  if(!subnet_node_found)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "subnetwork name(%s) not found to modify", sub_config_info_p->subnet_name);
    CNTLR_RCU_READ_LOCK_RELEASE();
    return CRM_ERROR_DUPLICATE_SUBNET_RESOURCE;
  }

  if(crm_detach_subnet_handle_from_vn(crm_subnet_node_scan_p->subnet_name, crm_subnet_node_scan_p->nw_name)!=CRM_SUCCESS)   
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Detach subnet handle from vn failed!.");
    return CRM_FAILURE;
  }
  if(crm_attach_subnet_handle_to_vn(vn_handle, 
				    sub_config_info_p->subnet_name,
				    (crm_subnet_node_scan_p->subnet_handle)
				   )!=CRM_SUCCESS)   
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Attach subnet handle to vn failed!.");
    return CRM_FAILURE;
  }

  strncpy(crm_subnet_node_scan_p->nw_name, sub_config_info_p->vn_name, CRM_MAX_VN_NAME_LEN);
  crm_subnet_node_scan_p->enable_dhcp_b = sub_config_info_p->enable_dhcp_b;
  memcpy(&(crm_subnet_node_scan_p->dns_nameservers), &(sub_config_info_p->dns_nameservers), sizeof(sub_config_info_p->dns_nameservers));

  crm_subnet_node_scan_p->pool_start_address = sub_config_info_p->pool_start_address;
  crm_subnet_node_scan_p->pool_end_address = sub_config_info_p->pool_end_address;
  crm_subnet_node_scan_p->ip_version = sub_config_info_p->ip_version;
  crm_subnet_node_scan_p->gateway_ip = sub_config_info_p->gateway_ip;
  crm_subnet_node_scan_p->cidr_ip = sub_config_info_p->cidr_ip;
  crm_subnet_node_scan_p->cidr_mask = sub_config_info_p->cidr_mask;

 *output_subnet_handle_p = crm_subnet_node_scan_p->subnet_handle;
 
  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Subnet Modified Successfully!");
  CNTLR_RCU_READ_LOCK_RELEASE();
  return CRM_SUCCESS;
}
/***************************************************************************
Function:crm_del_subnet
**************************************************************************/

int32_t  crm_del_subnet(char* subnet_name_p)
{
	struct   crm_subnet   *crm_subnet_node_scan_p=NULL;
	uint32_t hashkey, offset;
	uint8_t  status_b = FALSE;
	uint8_t  subnet_node_found=0;
	int32_t  ret_val;

	OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Entered");

	if(crm_subnet_table_g ==  NULL)
	{
		OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "There is no subnet hash table!.")
			return CRM_FAILURE;
	}

	if((subnet_name_p == NULL))
	{
		OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Subnetwork name is invalid");
		return CRM_ERROR_SUBNET_NAME_INVALID;
	}

	CNTLR_RCU_READ_LOCK_TAKE();

	/** calculate hash key for the Virtual network name */
	hashkey = crm_get_hashval_byname(subnet_name_p, crm_no_of_subnet_buckets_g);

	OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "bucket = %d",hashkey);
	offset = SUBNETNODE_SUBNETTBL_OFFSET;

	/** Scan subnet hash table for the name */
	MCHASH_BUCKET_SCAN(crm_subnet_table_g, hashkey, crm_subnet_node_scan_p, struct crm_subnet*, offset)
	{
		if(!strcmp(subnet_name_p, crm_subnet_node_scan_p->subnet_name))
		{
			OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Subnetwork name found to delete");
			subnet_node_found = 1;
			break;
		}
	}

	if(subnet_node_found == 1)
	{
    if(crm_detach_subnet_handle_from_vn(subnet_name_p,(crm_subnet_node_scan_p->nw_name))!=CRM_SUCCESS)   
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "dettach subnet handle from vn failed!.");
    }

		status_b = FALSE;
		offset = SUBNETNODE_SUBNETTBL_OFFSET;
		MCHASH_DELETE_NODE_BY_REF(crm_subnet_table_g,
				crm_subnet_node_scan_p->index,
				crm_subnet_node_scan_p->magic,
				struct crm_subnet *, 
				offset, 
				status_b);

		if(status_b == TRUE)
		{
			OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Subnet  deleted successfully!");
			ret_val = CRM_SUCCESS;
		}
		else
		{
			OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Subnet  deletion failed!");
			ret_val= CRM_FAILURE;
		}

	}
	else
	{
		OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Subnetwork name not found to delete!");
		ret_val=CRM_ERROR_SUBNET_NAME_INVALID;
	}

	CNTLR_RCU_READ_LOCK_RELEASE();
	OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG," Subnetwork deleted successfully");
	return ret_val;
}
/***************************************************************************/
int32_t crm_get_subnet_by_name(char* subnet_name_p,
		struct  crm_subnet_config_info*  crm_subnet_config_info_p,
		struct  subnet_runtime_info  *runtime_info)
{
	int32_t ret_val=CRM_FAILURE;
	uint8_t subnet_node_found=0;
	struct    crm_subnet  *crm_subnet_node_scan_p=NULL;
	uint32_t  hashkey, offset;

	if(crm_subnet_table_g == NULL)
		return CRM_FAILURE;

	if(subnet_name_p ==  NULL)
		return CRM_ERROR_SUBNET_NAME_INVALID;

	OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "subnet_name %s",subnet_name_p);
	CNTLR_RCU_READ_LOCK_TAKE();

	/** calculate hash key for the Virtual network name */
	hashkey = crm_get_hashval_byname(subnet_name_p, crm_no_of_subnet_buckets_g);
	OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "bucket = %d",hashkey);

	offset = SUBNETNODE_SUBNETTBL_OFFSET;

	/** Scan subnet hash table for the name */
	MCHASH_BUCKET_SCAN(crm_subnet_table_g, hashkey, crm_subnet_node_scan_p, struct crm_subnet*, offset)
	{
		if(!strcmp(subnet_name_p, crm_subnet_node_scan_p->subnet_name))
		{
			OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Subnetwork  found");
			subnet_node_found = 1;

			break;
		}

	}
	if(subnet_node_found == 1)
	{
		OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Copying Subnetwork Info...");
		strncpy(crm_subnet_config_info_p->subnet_name, crm_subnet_node_scan_p->subnet_name,CRM_MAX_SUBNET_NAME_LEN);
			strncpy(crm_subnet_config_info_p->vn_name, crm_subnet_node_scan_p->nw_name,CRM_MAX_VN_NAME_LEN);
		crm_subnet_config_info_p->enable_dhcp_b = crm_subnet_node_scan_p->enable_dhcp_b;

		mempcpy(&(crm_subnet_config_info_p->dns_nameservers), &(crm_subnet_node_scan_p->dns_nameservers),
				sizeof(crm_subnet_node_scan_p->dns_nameservers));

		crm_subnet_config_info_p->pool_start_address = crm_subnet_node_scan_p->pool_start_address;
		crm_subnet_config_info_p->pool_end_address = crm_subnet_node_scan_p->pool_end_address;
		crm_subnet_config_info_p->ip_version = crm_subnet_node_scan_p->ip_version;
		crm_subnet_config_info_p->gateway_ip = crm_subnet_node_scan_p->gateway_ip;
		crm_subnet_config_info_p->cidr_ip = crm_subnet_node_scan_p->cidr_ip;
		crm_subnet_config_info_p->cidr_mask = crm_subnet_node_scan_p->cidr_mask;


		ret_val = CRM_SUCCESS;

	}
	else
	{
		OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Subnetwork not found\r\n",__FUNCTION__, __LINE__);
		ret_val = CRM_ERROR_SUBNET_NAME_INVALID;
	}
	CNTLR_RCU_READ_LOCK_RELEASE();
	return ret_val;
}
/***************************************************************************/
int32_t  crm_get_first_subnet (struct  crm_subnet_config_info*  crm_subnet_config_info_p,
		struct  subnet_runtime_info  *runtime_info,
		uint64_t *subnet_handle_p)
{
	struct    crm_subnet   *crm_subnet_node_scan_p=NULL;
	uint32_t hashkey;

	uint32_t offset;

	OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Entered");

	if(crm_subnet_table_g == NULL)
		return CRM_FAILURE;

	CNTLR_RCU_READ_LOCK_TAKE();

	MCHASH_TABLE_SCAN(crm_subnet_table_g, hashkey)
	{
		offset = SUBNETNODE_SUBNETTBL_OFFSET;
		MCHASH_BUCKET_SCAN(crm_subnet_table_g, hashkey, crm_subnet_node_scan_p, struct    crm_subnet *, offset)
		{
			OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Copying Subnetwork Info...");
			strncpy(crm_subnet_config_info_p->subnet_name, crm_subnet_node_scan_p->subnet_name,CRM_MAX_SUBNET_NAME_LEN);
		
			strncpy(crm_subnet_config_info_p->vn_name, crm_subnet_node_scan_p->nw_name,CRM_MAX_VN_NAME_LEN);
			crm_subnet_config_info_p->enable_dhcp_b = crm_subnet_node_scan_p->enable_dhcp_b;

			mempcpy(&(crm_subnet_config_info_p->dns_nameservers), &(crm_subnet_node_scan_p->dns_nameservers),
					sizeof(crm_subnet_node_scan_p->dns_nameservers));

			crm_subnet_config_info_p->pool_start_address = crm_subnet_node_scan_p->pool_start_address;
			crm_subnet_config_info_p->pool_end_address = crm_subnet_node_scan_p->pool_end_address;
			crm_subnet_config_info_p->ip_version = crm_subnet_node_scan_p->ip_version;
			crm_subnet_config_info_p->gateway_ip = crm_subnet_node_scan_p->gateway_ip;
			crm_subnet_config_info_p->cidr_ip = crm_subnet_node_scan_p->cidr_ip;
			crm_subnet_config_info_p->cidr_mask = crm_subnet_node_scan_p->cidr_mask;



			//runtime_info_p->number_of_attributes = crm_vm_node_info_p->number_of_attributes;

			*subnet_handle_p = crm_subnet_node_scan_p->magic;
			*subnet_handle_p = ((*subnet_handle_p <<32) | (crm_subnet_node_scan_p->index));

			CNTLR_RCU_READ_LOCK_RELEASE();
			return CRM_SUCCESS;
		}
	}       
	CNTLR_RCU_READ_LOCK_RELEASE();
	return CRM_ERROR_NO_MORE_ENTRIES;
}
/***************************************************************************/
int32_t  crm_get_next_subnet (struct  crm_subnet_config_info*  crm_subnet_config_info_p,
		struct  subnet_runtime_info  *runtime_info,
		uint64_t *subnet_handle_p)
{
	struct    crm_subnet   *crm_subnet_node_scan_p=NULL;
	uint32_t hashkey;
	uint8_t current_entry_found_b;

	uint32_t offset = SUBNETNODE_SUBNETTBL_OFFSET;
	OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Entered");

	CNTLR_RCU_READ_LOCK_TAKE();
	current_entry_found_b = FALSE;

	MCHASH_TABLE_SCAN(crm_subnet_table_g, hashkey)
	{
		offset = SUBNETNODE_SUBNETTBL_OFFSET;
		MCHASH_BUCKET_SCAN(crm_subnet_table_g, hashkey, crm_subnet_node_scan_p, struct crm_subnet *,offset)
		{
			if(current_entry_found_b == FALSE)
			{
				if(*subnet_handle_p != (crm_subnet_node_scan_p->subnet_handle))
					continue;

				else
				{
					OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Subnetwork entry found copy next subnetwork!.");
					current_entry_found_b = TRUE;
					continue;
				}

				/*Skip the First matching Switch and Get the next Switch */
			}
			OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Copying Subnetwork Info...");
			strncpy(crm_subnet_config_info_p->subnet_name, crm_subnet_node_scan_p->subnet_name,CRM_MAX_SUBNET_NAME_LEN);
			crm_subnet_config_info_p->enable_dhcp_b = crm_subnet_node_scan_p->enable_dhcp_b;
			strncpy(crm_subnet_config_info_p->vn_name, crm_subnet_node_scan_p->nw_name,CRM_MAX_VN_NAME_LEN);

			mempcpy(&(crm_subnet_config_info_p->dns_nameservers), &(crm_subnet_node_scan_p->dns_nameservers),
					sizeof(crm_subnet_node_scan_p->dns_nameservers));

			crm_subnet_config_info_p->pool_start_address = crm_subnet_node_scan_p->pool_start_address;
			crm_subnet_config_info_p->pool_end_address = crm_subnet_node_scan_p->pool_end_address;
			crm_subnet_config_info_p->ip_version = crm_subnet_node_scan_p->ip_version;
			crm_subnet_config_info_p->gateway_ip = crm_subnet_node_scan_p->gateway_ip;
			crm_subnet_config_info_p->cidr_ip = crm_subnet_node_scan_p->cidr_ip;
			crm_subnet_config_info_p->cidr_mask = crm_subnet_node_scan_p->cidr_mask;



			//runtime_info_p->number_of_attributes = crm_vm_node_info_p->number_of_attributes;

			*subnet_handle_p = crm_subnet_node_scan_p->magic;
			*subnet_handle_p = ((*subnet_handle_p <<32) | (crm_subnet_node_scan_p->index));


			CNTLR_RCU_READ_LOCK_RELEASE();
			return CRM_SUCCESS;
		}
	}
	CNTLR_RCU_READ_LOCK_RELEASE();
	OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Failed to copy Subnetwork Info.....");
	if(current_entry_found_b == TRUE)
		return CRM_ERROR_NO_MORE_ENTRIES;
	else
		return CRM_ERROR_INVALID_SUBNET_HANDLE;
}
/****************************************************************************************************************/
int32_t crm_add_attribute_to_subnet(uint64_t subnet_handle, struct crm_attribute_name_value_pair* attribute_info_p)
{
  int32_t  retval;
  struct   crm_subnet            *subnet_entry_p;
  struct   crm_subnet_notification_data        notification_data={};
  struct   crm_notification_app_hook_info *app_entry_p;
  uchar8_t lstoffset;
 lstoffset = CRM_NOTIFICATION_APP_HOOK_OFFSET;


  CNTLR_RCU_READ_LOCK_TAKE();

  retval = get_subnet_byhandle(subnet_handle, &subnet_entry_p);
  if(retval != CRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return retval;
  }

  retval = crm_add_attribute(&(subnet_entry_p->attributes),attribute_info_p);
  if(retval != CRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return retval;
  }

  subnet_entry_p->number_of_attributes++;
  of_create_crm_view_entry(subnet_handle, attribute_info_p->name_string, attribute_info_p->value_string);
  /* Send add attribute to switch notification to registered Applications. */
  strcpy(notification_data.subnet_name, subnet_entry_p->subnet_name);
  strcpy(notification_data.attribute_name,attribute_info_p->name_string);
  strcpy(notification_data.attribute_value,attribute_info_p->value_string);
  OF_LIST_SCAN(crm_subnet_notifications[CRM_SUBNET_ATTRIBUTE_ADDED],app_entry_p,struct crm_notification_app_hook_info*,lstoffset )
  {
    ((crm_subnet_notifier_fn)(app_entry_p->call_back))(CRM_SUBNET_ATTRIBUTE_ADDED,
                                                        subnet_handle,
                                                        notification_data,
                                                        app_entry_p->cbk_arg1,
                                                        app_entry_p->cbk_arg2);
  }

  OF_LIST_SCAN(crm_subnet_notifications[CRM_SUBNET_ALL_NOTIFICATIONS],app_entry_p,struct crm_notification_app_hook_info*,lstoffset )
  {
    ((crm_subnet_notifier_fn)(app_entry_p->call_back))(CRM_SUBNET_ATTRIBUTE_ADDED,
                                                        subnet_handle,
                                                        notification_data,
                                                        app_entry_p->cbk_arg1,
                                                        app_entry_p->cbk_arg2);
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return CRM_SUCCESS;
}


int32_t crm_delete_attribute_from_subnet(uint64_t subnet_handle, struct crm_attribute_name_value_pair *attribute_info_p)
{

  int32_t  retval;
  struct   crm_subnet            *subnet_entry_p;
  struct   crm_resource_attribute_entry *attribute_entry_p,*prev_app_entry_p = NULL;

  struct   crm_subnet_notification_data   notification_data={};
  struct   crm_notification_app_hook_info *app_entry_p;
  uchar8_t lstattrnameoffset;
  lstattrnameoffset = CRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET;


  CNTLR_RCU_READ_LOCK_TAKE();

  retval = get_subnet_byhandle(subnet_handle, &subnet_entry_p);
  if(retval != CRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    return retval;
  }
   OF_LIST_SCAN(subnet_entry_p->attributes, attribute_entry_p, struct crm_resource_attribute_entry*,lstattrnameoffset)
  {
    if(!strcmp(attribute_entry_p->name, attribute_info_p->name_string))
    {

      strcpy(notification_data.subnet_name, subnet_entry_p->subnet_name);
      strcpy(notification_data.attribute_name,attribute_entry_p->name);
      strcpy(notification_data.attribute_value,attribute_entry_p->value);
       OF_LIST_SCAN(crm_subnet_notifications[CRM_SUBNET_ATTRIBUTE_DELETE],app_entry_p,struct crm_notification_app_hook_info*, lstattrnameoffset )
            {
              ((crm_subnet_notifier_fn)(app_entry_p->call_back))(CRM_SUBNET_ATTRIBUTE_DELETE,
                                                            subnet_handle,
                                                            notification_data,
                                                            app_entry_p->cbk_arg1,
                                                            app_entry_p->cbk_arg2);
           }

           OF_LIST_SCAN(crm_subnet_notifications[CRM_SUBNET_ALL_NOTIFICATIONS],app_entry_p,struct crm_notification_app_hook_info*,lstattrnameoffset)
      {
              ((crm_subnet_notifier_fn)(app_entry_p->call_back))(CRM_SUBNET_ATTRIBUTE_DELETE,
                                                            subnet_handle,
                                                            notification_data,
                                                            app_entry_p->cbk_arg1,
                                                            app_entry_p->cbk_arg2);
           }


         OF_REMOVE_NODE_FROM_LIST(subnet_entry_p->attributes,attribute_entry_p,prev_app_entry_p, lstattrnameoffset);
         subnet_entry_p->number_of_attributes--;
         of_remove_crm_view_entry(attribute_entry_p->name, attribute_entry_p->value);
        CNTLR_RCU_READ_LOCK_RELEASE();
        return CRM_SUCCESS;
    }
    prev_app_entry_p = attribute_entry_p;
    continue;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return CRM_FAILURE;
}

/*******************************************************************************************************/
int32_t crm_get_subnet_first_attribute(uint64_t subnet_handle,
struct   crm_attribute_name_value_output_info *attribute_output_p)
{
  int32_t  retval;
  struct crm_subnet *crm_subnet_entry_p=NULL;
  if(attribute_output_p == NULL)
    return CRM_FAILURE;



  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    retval = get_subnet_byhandle(subnet_handle, &crm_subnet_entry_p);
    if(retval != CRM_SUCCESS)
    { 
      retval = CRM_ERROR_INVALID_SUBNET_HANDLE;
      break;
    }
    retval = crm_get_first_attribute(&(crm_subnet_entry_p->attributes), attribute_output_p);

  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  return retval;
}

/*****************************************************************************************************/
int32_t crm_get_subnet_next_attribute(uint64_t subnet_handle,
      char*    current_attribute_name,
      struct   crm_attribute_name_value_output_info *attribute_output_p)
{
  int32_t retval;
  struct crm_subnet *crm_subnet_entry_p=NULL;

  if((attribute_output_p == NULL) || (current_attribute_name == NULL))
    return CRM_FAILURE;

  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    retval = get_subnet_byhandle(subnet_handle,&crm_subnet_entry_p);
    if(retval != CRM_SUCCESS)
    {
      retval= CRM_ERROR_INVALID_SUBNET_HANDLE;
      break;
    }
    retval = crm_get_next_attribute(&(crm_subnet_entry_p->attributes), current_attribute_name, attribute_output_p);
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  return retval;
}

/******************************************************************************/
int32_t crm_get_subnet_exact_attribute(uint64_t subnet_handle,
      char *attribute_name_string,struct crm_attribute_name_value_output_info *attribute_output)
{
  struct   crm_subnet *crm_subnet_entry_p=NULL;
  int32_t  retval;

  if(attribute_name_string == NULL)
   return CRM_ERROR_ATTRIBUTE_NAME_NULL;

  if(strlen(attribute_name_string) > CRM_MAX_ATTRIBUTE_NAME_LEN)
    return CRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN;

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
     retval = get_subnet_byhandle(subnet_handle, &crm_subnet_entry_p);
     if(retval != CRM_SUCCESS)
     {
       retval= CRM_ERROR_INVALID_SUBNET_HANDLE;
       break;	
     }
     retval = crm_get_exact_attribute(&(crm_subnet_entry_p->attributes),  attribute_name_string,attribute_output);
   }while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return retval;
}

