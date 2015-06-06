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
 * File name: crm_subnet_api.h
 * Author: Varun Kumar Reddy <B36461@freescale.com>
 * Date:   11/10/2013
 * Description: 

@Description   This file contains the CRM subnet API & related 
                      macros, data structures
*//***************************************************************************/


#ifndef __CRM_SUBNET_API_H
#define __CRM_SUBNET_API_H

#define CRM_SUBNET_FIRST_NOTIFICATION_TYPE CRM_SUBNET_ALL_NOTIFICATIONS
#define CRM_SUBNET_LAST_NOTIFICATION_TYPE  CRM_SUBNET_DETACH

/* A hash table is used to store the added subnets and subnet_name is the key. 
 * A subnet can be assigned to only one Virtual Network VN. At present dynamic 
 * views are not supported for crm_subnet. */

struct crm_subnet
{
  char   subnet_name [CRM_MAX_SUBNET_NAME_LEN ];   /* = "POC2Subnet" */
  /* char*   subnet_id [64];   *//* Can be added as an attribute to the subnet */
  char     nw_name [CRM_MAX_VN_NAME_LEN];

  uint8_t enable_dhcp_b;                    /* TRUE, FALSE */
  uint32_t dns_nameservers [4];       /* {0xc0a80401, 0xc0a80602, 0, 0} */

  /* Allocation Pool */
  uint32_t pool_start_address;         /* 0x0a0b0b02 */
  uint32_t pool_end_address;   /* 0x0a0b0bfe */ 

  /* "host_routes": [], */
  uint32_t ip_version;    /* 4 */    
  uint32_t gateway_ip;   /* 0x0a0b0b01 */
  uint32_t cidr_ip;          /* 0x0a0b0b00 */
  uint32_t cidr_mask;     /* 24 */

  /* of list holding attributes added for this sub Network Node */
  uint32_t  number_of_attributes;
  of_list_t attributes;  /* Each attribute may contain multiple attribute values */

  /** Boolean flag indicate whether this memory is allocated from heap or not*/
  uint8_t heap_b;
  struct mchash_table_link subnet_tbl_link;
  uint32_t magic;
  uint32_t index;
  uint64_t subnet_handle;	
};
#define SUBNETNODE_SUBNETTBL_OFFSET offsetof(struct crm_subnet, subnet_tbl_link)

int32_t crm_get_subnet_handle(char *subnet_name, uint64_t *subnet_handle);




#endif
