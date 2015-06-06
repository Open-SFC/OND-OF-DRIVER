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
 * File name: crmapi.h
 * Author: Varun Kumar Reddy <B36461@freescale.com>
 * Date:   11/10/2013
 * Description: 
*/


//! Cloud Resource Management API
 /*! This module provides Cloud Resource Management APIs
 * \addtogroup Cloud_Resource_Management_API
 */
//! Virtual Network Management API
 /*! This module provides Virtual Network Management APIs
 * \addtogroup Virtual_Network_Management_API
 */
//! Virtual Machine Management API
 /*! This module provides Virtual Machine Management APIs
 * \addtogroup Virtual_Machine_Management_API
 */
//! Tenant Management API
 /*! This module provides Tenant Management APIs
 * \addtogroup Tenant_Management_API
 */
//! Subnet Management API
 /*! This module provides Subnet Management APIs
 * \addtogroup Subnet_Management_API
 */
//! Error Management API
 /*! This module provides Error Management APIs
 * \addtogroup Error_Management_API
 */

/** Macros */

#ifndef __CRM_API_H
#define __CRM_API_H

#define CRM_SUCCESS 			 0

/** CRM ERROR MACROS **/
#define CRM_FAILURE 			-1
#define CRM_ERROR_NO_MORE_ENTRIES 	-2
#define CRM_ERROR_INVALID_PARAM_VALUE 	-3

/* CRM Virtual Machine Error Macros */
#define CRM_ERROR_INVALID_VM_TYPE       -4
#define CRM_ERROR_DUPLICATE_VM_RESOURCE -5
#define CRM_ERROR_INVALID_VM_HANDLE     -6
#define CRM_ERROR_VM_NAME_INVALID       -7
#define CRM_ERROR_VM_NAME_NULL          -8  

/* CRM Virtual Network Error  Macros */
#define CRM_ERROR_VN_NAME_INVALID            -9
#define CRM_ERROR_INVALID_VN_HANDLE         -10
#define CRM_ERROR_SUBNET_NAME_INVALID       -11
#define CRM_ERROR_VN_NETWORK_TYPE_INVALID   -12
#define CRM_ERROR_DUPLICATE_VN_RESOURCE     -13 
#define CRM_ERROR_VN_ADD_FAIL               -14
#define CRM_ERROR_VMS_FAILED_TO_ATTACH      -15
#define CRM_ERROR_DUPLICATE_SUBNET_RESOURCE -16
#define CRM_ERROR_SUBNET_ADD_FAIL           -17
#define CRM_ERROR_INVALID_SUBNET_HANDLE     -18

/* CRM Tenant Error Macros */
#define CRM_ERROR_TENANT_NAME_INVALID         -19
#define CRM_ERROR_DUPLICATE_TENANT_RESOURCE   -20
#define CRM_ERROR_TENANT_ADD_FAIL             -21

#define CRM_ERROR_DUPLICATE_COMPUTE_NODE      -22
#define CRM_ERROR_DUPLICATE_CRM_PORT_RESOURCE -23
#define CRM_ERROR_INVALID_TENANT_HANDLE       -24
#define CRM_ERROR_RESOURCE_NOTEMPTY           -25

/** \ingroup Error_Management
  * CRM_ERROR_ATTRIBUTE_NAME_NULL  
  * - Attribute name is not provided. 
  */
#define CRM_ERROR_ATTRIBUTE_NAME_NULL -26

/** \ingroup Error_Management
  * CRM_ERROR_ATTRIBUTE_VALUE_NULL  
  * - Attribute value is not provided. 
  */
#define CRM_ERROR_ATTRIBUTE_VALUE_NULL -27

/** \ingroup Error_Management 
  * CRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN  
  * - Attribute name is too long. 
  */
#define CRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN   -28

/** \ingroup Error_Management
  * CRM_ERROR_INVALID_ATTRIBUTE_VALUE_LEN  
  * -Attribute value is too long. 
  */
#define CRM_ERROR_INVALID_ATTRIBUTE_VALUE_LEN  -29
#define CRM_ERROR_DUPLICATE_RESOURCE -30

/** \ingroup Error_Management
  * CRM_ERROR_NO_MEM 
  * - No memory available to create the database record.
  */
#define CRM_ERROR_NO_MEM -31

/** \ingroup Error_Management
  *  DPRM_ERROR_INVALID_NAME 
  *  - No resource with this name is present in database.
  */
#define CRM_ERROR_INVALID_NAME    -32
#define CRM_ERROR_INVALID_SWITCH_HANDLE -33
#define CRM_ERROR_INVALID_PORT_HANDLE -34

#define CRM_MAX_APPL_NAME_LEN 8 

#define CRM_MAX_VM_NAME_LEN	       128
#define CRM_MAX_VM_DESC_LEN	       256
#define CRM_MAX_VN_NAME_LEN	       128
#define CRM_MAX_VN_DESC_LEN	       256
#define CRM_MAX_VN_PROVIDER_PHY_NW     256
#define CRM_MAX_DPS_IN_A_COMPUTE_NODE  256
#define CRM_MAX_TENANT_NAME_LEN        128
#define CRM_MAX_SUBNET_NAME_LEN        128
#define CRM_MAX_PORT_NAME_LEN           16
#define DPRM_MAX_SWITCH_NAME_LEN       128
#define CRM_LOGICAL_SWITCH_NAME_LEN    128
#define CRM_MAX_ATTRIBUTE_NAME_LEN     128 
#define CRM_MAX_ATTRIBUTE_VALUE_LEN    128 

/* CRM NOTIFICATIONS MACROS */

/* CRM Virtual Network Notifications */
#define CRM_VN_ALL_NOTIFICATIONS 0
#define CRM_VN_ADDED	 	 1
#define CRM_VN_DELETE		 2
#define CRM_VN_MODIFIED		 3
#define CRM_VN_VIEW_ADDED	 4
#define CRM_VN_VIEW_DELETE  	 5
#define CRM_VN_ATTRIBUTE_ADDED 	 6
#define CRM_VN_ATTRIBUTE_DELETE  7
#define CRM_VN_SUBNET_ADDED	 8
#define CRM_VN_SUBNET_DELETE	 9
#define CRM_VN_DETACH		10

/* CRM Port Notifications */
#define CRM_PORT_ALL_NOTIFICATIONS 0
#define CRM_PORT_ADDED             1
#define CRM_PORT_MODIFIED          2
#define CRM_PORT_DELETE            3
#define CRM_PORT_STATUS            4
#define CRM_PORT_ATTRIBUTE_ADDED   5
#define CRM_PORT_ATTRIBUTE_DELETE  6
/* CRM Subnet Notifications */
#define   CRM_SUBNET_ALL_NOTIFICATIONS 0
#define   CRM_SUBNET_ADDED             1
#define   CRM_SUBNET_DELETE            2 
#define   CRM_SUBNET_MODIFIED          3 
#define   CRM_SUBNET_ATTRIBUTE_ADDED   4
#define   CRM_SUBNET_ATTRIBUTE_DELETE  5
#define   CRM_SUBNET_DETACH	       6

/* CRM Virtual Machine Notifications */
#define   CRM_VM_ALL_NOTIFICATIONS 0
#define   CRM_VM_ADDED             1
#define   CRM_VM_DELETE            2 
#define   CRM_VM_MODIFIED          3 
#define   CRM_VM_VIEW_ADDED        4
#define   CRM_VM_VIEW_DELETE       5
#define   CRM_VM_ATTRIBUTE_ADDED   6
#define   CRM_VM_ATTRIBUTE_DELETE  7
#define   CRM_VM_LOGICAL_SWITCH_SIDE_PORT_ADDED   8
#define   CRM_VM_LOGICAL_SWITCH_SIDE_PORT_DELETE  9
#define   CRM_VM_DETACH		                  10

/* CRM Tenant Notifications */
#define   CRM_TENANT_ALL_NOTIFICATIONS 0
#define   CRM_TENANT_ADDED             1
#define   CRM_TENANT_DELETE            2 
#define   CRM_TENANT_MODIFIED          3 
#define   CRM_TENANT_ATTRIBUTE_ADDED   4
#define   CRM_TENANT_ATTRIBUTE_DELETE  5
#define   CRM_TENANT_DETACH	       6

/********** CRM VN CONFIG DECLARATIONS **************/
/* enums */
/* Virtual network types */
enum crm_virtual_network_type 
{
  NO_OVERLAYS = 0,
  VXLAN_TYPE  = 1,
  NVGRE_TYPE  = 2,
  VLAN_TYPE,       /* Only VXLAN and NVGRE Types are supported at present */
};

#define SUPPORTED_NW_TYPE_MIN  VXLAN_TYPE
#define SUPPORTED_NW_TYPE_MAX  NVGRE_TYPE

/* vxlan network structure. */    
struct crm_vxlan_nw 
{   
  uint32_t  nid;           /* VXLAN network identifier */
  uint16_t  service_port;  /* VXLAN Service port */
  uint32_t  remote_ip;     /* remote ip is non zero for broadcast network ports */   
}; 

struct crm_nvgre_nw
{
  uint32_t  nid;           /* VXLAN network identifier */
  uint16_t  service_port;  /* VXLAN Service port */
  uint32_t  remote_ip;     /* remote ip is non zero for broadcast network ports */
};

struct crm_vlan_nw
{
  uint16_t vlan_id; 
};

/* CRM  VN config structure */
struct crm_vn_config_info 
{
  char    nw_name[CRM_MAX_VN_NAME_LEN]; 
  char    tenant_name[CRM_MAX_TENANT_NAME_LEN]; 
  uint8_t nw_type;
  /* Virtual network parameters */
  union
  {
    struct crm_vxlan_nw vxlan_nw;
    struct crm_nvgre_nw nvgre_nw;
    struct crm_vlan_nw  vlan_nw;
  }nw_params;

  /* Optional parameters */
  char     nw_desc[CRM_MAX_VN_DESC_LEN];
  uint8_t  vn_operational_status_b;
  char     provider_physical_network[CRM_MAX_VN_PROVIDER_PHY_NW];
  uint8_t  vn_admin_state_up_b;
  uint8_t  router_external_b;
};

struct vn_runtime_info
{
  /** Number of attributes added sofar to the switch.*/
  uint32_t number_of_attributes;
};

/* Following notification structure used to send reg apps when 
 * vn configuration done.
 */
struct crm_vn_notification_data 
{
  char      vn_name[CRM_MAX_VN_NAME_LEN + 1];
  uint64_t  crm_vn_handle;  // crm vn handle
  char      swname[DPRM_MAX_SWITCH_NAME_LEN+1];
  uint64_t  switch_handle;  // switch handle
  uint64_t  dp_handle;//logical switch handle
  //uint64_t  crm_vm_handle;//logical switch handle
 
  uint8_t   nw_type;
  uint32_t  tun_id;
 
  /* Virtual network parameters */
  union
  {
    struct crm_vxlan_nw vxlan_nw;
    struct crm_nvgre_nw nvgre_nw;
    struct crm_vlan_nw  vlan_nw;  
  }nw_params;  

  /* attribute name - valid for the attribute add and delete notification types */
  char attribute_name[CRM_MAX_ATTRIBUTE_NAME_LEN + 1];
  /* attribute value - valid for the attribute add and delete notification types */
  char attribute_value[CRM_MAX_ATTRIBUTE_VALUE_LEN + 1];
 
};

struct crm_port_notification_data
{
  char      port_name[CRM_MAX_PORT_NAME_LEN + 1];
  int32_t   crm_port_type;   /* VMSIDE_PORT,VMNS_PORT,NWSIDE_UNICAST_PORT,NWSIDE_BROADCAST_PORT */
  int8_t    crm_port_status; /* CRM_PORT_READY or CRM_PORT_NOT_READY */
  uint64_t  crm_port_handle; /* CRM Port db handle */
  uint32_t  old_port_id;     /* previous port id when port updates */
  uint32_t  new_port_id;     /* present port id */

  char      vn_name[CRM_MAX_VN_NAME_LEN + 1];
  uint64_t  crm_vn_handle;   /* virtual network handle */
  char      swname[DPRM_MAX_SWITCH_NAME_LEN+1];
  uint64_t  switch_handle;   /* system switch handle */
  uint64_t  dp_handle;       /* system logical switch handle */
  uint64_t  crm_vm_handle;   /* virtual machine handle */
  
  /* following two fileds will be appilcable if port is network side  .
   * remote ip has value when port broadcast side */
  uint8_t   nw_type;
  uint32_t  tun_id;
  /* Virtual network parameters */
  union
  {
    struct crm_vxlan_nw vxlan_nw;
    struct crm_nvgre_nw nvgre_nw; 
    struct crm_vlan_nw  vlan_nw;
  }nw_params;

  /* attribute name - valid for the attribute add and delete notification types */
  char attribute_name[CRM_MAX_ATTRIBUTE_NAME_LEN + 1];
  /* attribute value - valid for the attribute add and delete notification types */
  char attribute_value[CRM_MAX_ATTRIBUTE_VALUE_LEN + 1];
};

/* following structure used to config subnet */
struct crm_subnet_config_info
{
  char     subnet_name [CRM_MAX_SUBNET_NAME_LEN +1];   /* = "POC2Subnet" */
  char     vn_name[CRM_MAX_VN_NAME_LEN + 1];
  /* Allocation Pool */
  uint32_t pool_start_address;         /* 0x0a0b0b02 */
  uint32_t pool_end_address;   /* 0x0a0b0bfe */      
  /* char*   subnet_id [64];   *//* Can be added as an attribute to the subnet */
  uint8_t  enable_dhcp_b;                    /* TRUE, FALSE */
  uint32_t dns_nameservers [4];       /* {0xc0a80401, 0xc0a80602, 0, 0} */
  /* "host_routes": [], */
  uint32_t ip_version;    /* 4 */
  uint32_t gateway_ip;   /* 0x0a0b0b01 */
  uint32_t cidr_ip;          /* 0x0a0b0b00 */
  uint32_t cidr_mask;     /* 24 */
};

struct subnet_runtime_info
{   	
  /** Number of attributes added sofar to the switch.*/
  uint32_t number_of_attributes;  
};

/*Type of ports */
enum crm_port_type_e
{
  VM_PORT,
  VMNS_PORT,
  NW_UNICAST_PORT,
  NW_BROADCAST_PORT,
  VM_GUEST_PORT
};

/* Port type */
#if 0
enum crm_port_status
{
  SYSTEM_PORT_READY,
  SYSTEM_PORT_NOT_READY
};
#endif

/* following structure used to config nw side ports */
struct crm_nwport_config_info
{
  char       port_name[CRM_MAX_PORT_NAME_LEN+1];
  int32_t    port_type;
  uint32_t   portId;
  uint8_t    nw_type;
  char       switch_name[DPRM_MAX_SWITCH_NAME_LEN+1];
  char       br_name[CRM_LOGICAL_SWITCH_NAME_LEN +1];
  union
  {
    struct crm_vxlan_nw vxlan_info;
    struct crm_nvgre_nw nvgre_info;
    struct crm_vlan_nw  vlan_info; 
  }nw_port_config_info;
};

struct crm_port_config_info
{
  char     port_name[CRM_MAX_PORT_NAME_LEN+1];
  int32_t  port_type;
  uint32_t portId;
  char     vn_name[CRM_MAX_VN_NAME_LEN+1];
  char     switch_name[DPRM_MAX_SWITCH_NAME_LEN +1];
  char     vm_name[CRM_MAX_VM_NAME_LEN];
  uint8_t  vm_port_mac_p[6];
  char     br_name[CRM_LOGICAL_SWITCH_NAME_LEN +1];
};

/****   CRM VM CONFIG STRUCTURES ********/
/* enums */
/* vm types */
enum crm_virtual_machine_type
{
  VM_TYPE_NORMAL_APPLICATION,
  VM_TYPE_NETWORK_SERVICE   /* VM hosting a Network service */
};

/* Applications that add/modify virtual machines pass the required configuration information
* using this structure. This configuration structure is used by the API crm_add_virtual_machine
* and crm_modify_virtual_machine. 
* The members of this structure are explained as part of the API crm_add_virtual_machine description.
*/
struct crm_vm_config_info 
{
  char    vm_name[CRM_MAX_VM_NAME_LEN];
  int32_t vm_type;
  char    tenant_name[CRM_MAX_TENANT_NAME_LEN];
  char    switch_name[DPRM_MAX_SWITCH_NAME_LEN]; 
  /* Optional parameters */
  char    vm_desc[CRM_MAX_VM_DESC_LEN];
};

struct vm_runtime_info
{   	
  /** Number of attributes added sofar to the switch.*/
  uint32_t number_of_attributes;  
};

/*This structure is used while sending Virtual Machine related notifications.*/
struct crm_vm_notification_data 
{
  /* Name of the virtual machine - valid for all the notification types */
  char vm_name[CRM_MAX_VN_NAME_LEN + 1];
  /* attribute name - valid for the attribute add and delete notification types */
  char attribute_name[CRM_MAX_ATTRIBUTE_NAME_LEN + 1];
  /* attribute value - valid for the attribute add and delete notification types */
  char attribute_value[CRM_MAX_ATTRIBUTE_VALUE_LEN + 1];
  /* subnet name - valid for subnet add and delete notification types */
  /* port name - valid for port add and delete notification types */
};

/*** CRM TENANT CONFIG STRUCTURES ***/
/* CRM Tenant structure */
struct crm_tenant_config_info 
{
  char tenant_name[CRM_MAX_TENANT_NAME_LEN];
};

struct tenant_runtime_info
{
  /** Number of attributes added sofar to the switch.*/
  uint32_t number_of_attributes;
};

/*This structure is used while sending tenant related notifications.*/
struct crm_tenant_notification_data 
{
  /* Name of the tenant - valid for all the notification types */
  char tenant_name[CRM_MAX_VN_NAME_LEN + 1];
  /* attribute name - valid for the attribute add and delete notification types */
  char attribute_name[CRM_MAX_ATTRIBUTE_NAME_LEN + 1];
  /* attribute value - valid for the attribute add and delete notification types */
  char attribute_value[CRM_MAX_ATTRIBUTE_VALUE_LEN + 1];
  /* subnet name - valid for subnet add and delete notification types */
  /* port name - valid for port add and delete notification types */
};

/*This structure is used while sending subnet related notifications.*/
struct crm_subnet_notification_data 
{
  /* Name of the tenant - valid for all the notification types */
  char subnet_name[CRM_MAX_SUBNET_NAME_LEN + 1];
  /* attribute name - valid for the attribute add and delete notification types */
  char attribute_name[CRM_MAX_ATTRIBUTE_NAME_LEN + 1];
  /* attribute value - valid for the attribute add and delete notification types */
  char attribute_value[CRM_MAX_ATTRIBUTE_VALUE_LEN + 1];
  /* subnet name - valid for subnet add and delete notification types */
  /* port name - valid for port add and delete notification types */
};

/**
 \struct  crm_attribute_name_value_pair 
 \brief Provides attribute information passed as an argument to database API\n
 <b>Description</b>\n
 Attribute information includes the name of the attribute and its value.
 */
struct crm_attribute_name_value_pair
{
  /** Name of the attribute */
  char    name_string[CRM_MAX_ATTRIBUTE_NAME_LEN];
  /** length of the name string */
  int32_t name_length;
  /** value field of the attribute */
  char    value_string[CRM_MAX_ATTRIBUTE_VALUE_LEN];
  /** length of the value string */
  int32_t value_length;
};

/** 
  \struct dprm_attribute_name_value_output_info 
  \brief Provides attribute information returned from database API \n
  <b>Description</b>\n
  Attribute information includes the name of the attribute and its length and also
  the value of the attribute and its length. 
  */
struct crm_attribute_name_value_output_info{
  /** Name of the attribute */
  char    name_string[CRM_MAX_ATTRIBUTE_NAME_LEN];
  /** length of the name string */ 
  int32_t name_length;
  /** value field of the attribute */
  char    value_string[CRM_MAX_ATTRIBUTE_VALUE_LEN];
  /** length of the value string */
  int32_t value_length;
};

/**
\ingroup Virtual_Network_Management_API 

<b>Description:</b>\n
   		This API adds virtual network to the virtual database.
		It creates a hash table and adds the new virtual network node represented 
		by the data structure "struct crm_vn_config_info" to the table. This hash 
		table provides a static view to the VN database using the argument nw_name.
	       	Separate API is provided to add dynamic views to the VN database by adding 
		attributes to the virtual network nodes.
		The input arguments tenant_name and subnet_name are used to obtain the safe 
		references of tenant node and subnet network respectively. A handle to the 
		added VN node is returned so that it can be used for faster reference. 

		A VN notification of the type CRM_VN_ADDED is sent to the registered Applications.

		The argument config_info contains the parameters required for configuring the 
		Virtual Network.

\Param[in]	config_info_p - Virtual Network configuration information. 
\Param[in]	nw_name - Name of the virtual network to be added. This shall be unique. 
\Param[in]	tenant_name -  Tenant to which this VN belongs.
\Param[in]	nw_type -  The network types, supported at present are VLAN_TYPE and VXLAN_TYPE.
\Param[in]	nw_id - VLAN_ID or VXLAN_VNI based on nw_type.
\Param[in]	vxlan_service_port - Service port if nw_type is VXLAN.
\Param[in] 	subnet_name - IP Address range assigned to the virtual network. It may be added using modify API.

\Param[in]	optional - nw_desc - Description of the virtual network to be added.
\Param[in]	optional - vn_operational_status -  VN_ACTIVE = 1, VN_INACTIVE = 0 
\Param[in]	optional - provider_physical_network_p - Name of the provider's physical network say "physnet1"
\Param[in]	optional - vn_admin_state_up - TRUE or FALSE
\Param[in] 	optional - router_external - TRUE or FALSE 

\Param[out]	output_vn_handle_p - Handle to the virtual network node.

\Return		Success or Failure
		Success, if the entry is added. Else one of the following 
		error code is returned.
		- Invalid parameter value.
		- Memory allocation failure to create entry
		
*/
int32_t crm_add_virtual_network (struct crm_vn_config_info* config_info_p, uint64_t* output_vn_handle_p);

/**
\ingroup Virtual_Network_Management_API

<b>Description:</b>\n
	        This API deletes an existing Virtual Network (VN) Node.
		It searches the VN database for the VN node with the given network name.
		Before calling this API, all the attributes associated with the VN node 
		shall be deleted using the API crm_delete_attribute_from_vn which also 
		deletes all the dynamic views of the VN node.

		A VN notification of the type CRM_VN_DELETE is sent to the registered 
		Applications.

\Param[in]	nw_name_p - Name of the virtual network to be deleted.

\Return		Success or Failure
		Success, if the entry is deleted. Else the following 
		error code is returned.
		- Virtual network name not found.

*/
int32_t crm_del_virtual_network (char*  nw_name_p);

/**
\ingroup Virtual_Network_Management_API

<b>Description:</b>\n
		This API updates an existing Virtual Network node with the new 
		configuration values provided. The input argument field_flags can be
	       	used to specify the fields to be updated. The parameters vn_name, nw_type, 
		nw_id shall not be allowed to be modified. Name of the Virtual Network  
		present in the input argument config_info is used to find the VN node. 
		If a subnet is added, a VN notification of the type CRM_VN_SUBNET_ADDED
	       	is sent to the registered Applications. 

		A VN notification of the type CRM_VN_MODIFIED is sent to the 
		registered Applications.

\Param[in]	config_info_p - Contains the VN parameters to be modified.
\Param[in] 	field_flags - Bit fields to identify the parameters to be updated.

\Return		Success or Failure
		Success, if the entry is updated. Else one of the following 
		error code is returned.
		- Invalid parameter value.
		- Could not find the Virtual network to modify.
	
*/
int32_t  crm_modify_virtual_network (struct crm_vn_config_info* config_info_p, uint32_t  field_flags );

/**
\ingroup Virtual_Network_Management_API

<b>Description:</b>\n
	        This API returns the VN node information other than the attached 
		crm_compute_nodes, crm_logical_switches and the associated crm_ports 
		by taking name of the virtual network as input. 
		The remaining information can be obtained using separate API.


\Param[in]      vn_name - Name of the Virtual Network.	
\Param[out]     config_info - Configured Virtual Network node information
\Param[out]	vn_runtime_info - Contains the number of Virtual Networks and the number of VN attributes

\Return		Success or Failure
		Success, if the entry is found. Else the following 
		error code is returned.
		- Could not find the Virtual Network with the given vn_name.

*/
int32_t crm_get_vn_by_name(char* vn_name,  struct  crm_vn_config_info*  config_info_p,
                                                   struct  vn_runtime_info  *vn_runtime_info);

/**
\ingroup Virtual_Network_Management_API

<b>Description:</b>\n 
	        This API returns the first VN node information other than the attached 
		crm_compute_ nodes, crm_logical_switches and the associated crm_ports. 
		It returns a handle to the first Virtual Network and is required to be passed 
		to get the next VN node in the database. The remaining information can be 
		obtained using separate API.

\Param[out]     config_info - Configured VN node information
\Param[out]	vn_runtime_info - Contains the number of Virtual Networks and the number of VN attributes
\Param[out]	vn_handle - Handle to the First Virtual Network

\Return		Success or Failure
		Success, if the entry is found. Else error code is returned.
	
*/
int32_t  crm_get_first_virtual_network (struct  crm_vn_config_info*  config_info_p,
                                        struct  vn_runtime_info  *vn_runtime_info,
                                        uint64_t *  vn_handle_p);
/**
\ingroup Virtual_Network_Management_API 

<b>Description:</b>\n
	        This API returns the information of the VN node which is next to the 
		VN node in the database whose handle is given as the input. 
		All information other than the attached crm_compute_nodes, 
		crm_logical_switches and the associated crm_ports is returned. 
		It also returns a handle to the Virtual Network which can be used 
		to get the next VN node. 

\Param[out]     config_info - Configured VN node information
\Param[out]	vn_runtime_info - Contains the number of Virtual Networks and the number of VN attributes
\Param[in/out]	vn_handle - Handle to the Virtual Network

\Return		Success or Failure
		Success, if the entry is found. Else error code is returned.

*/
int32_t  crm_get_next_virtual_network (struct  crm_vn_config_info*  config_info_p,
                                       struct  vn_runtime_info  *vn_runtime_info,
                                       uint64_t *  vn_handle_p);
/**
\ingroup Subnet_Management_API

<b>Description:</b>\n
	        This API adds a Subnet node to the Subnet database using subnet_name
		as the key. Subnet database is implemented using the hash table data 
		structure. The Safe reference to the Subnode added is returned. 

\Param[in]	subnet_name -  Name of the Subnet to be added to the database. Has to be unique across Subnets.  
\Param[in]      pool_start_address, pool_end_end_address -  Range of IP Addresses in the Subnet.  
\Param[out]     output_subnet_handle - Safe reference to the Subnet node created.

\Return		Success or Failure
		Success, if the entry is added. Else error code is returned.

*/

int32_t crm_add_subnet(struct crm_subnet_config_info *sub_config_info_p, uint64_t *output_subnet_handle_p);

/**
\ingroup Subnet_Management_API

<b>Description:</b>\n
    	        This API deletes the Subnet node from the Subnet database.

\Param[in]	subnet_name_p -  Name of the Subnet to be deleted.

\Return		Success or Failure
		Success, if the entry is deleted. Else the following error is returned.
		- Could not find the subnet.

*/
int32_t  crm_del_subnet(char* subnet_name_p);
int32_t crm_modify_subnet(struct crm_subnet_config_info *sub_config_info_p,
                uint64_t *output_subnet_handle_p);

/**
\ingroup Subnet_Management_API

<b>Description:</b>\n
	        This API returns the Subnet node information  
		by taking name of the Subnet as input. 

\Param[in]      subnet_name - Name of the Subnet.	
\Param[out]     config_info - Subnet node information
\Param[out]	runtime_info - Contains the number of Subnets and the number of Subnet attributes

\Return		Success or Failure
		Success, if the Subnet is found. Else the following 
		error code is returned.
		- Could not find the Subnet with the given subnet_name.
*/
int32_t crm_get_subnet_by_name(char* subnet_name,
                        struct  crm_subnet_config_info*  config_info_p,
                        struct  subnet_runtime_info  *runtime_info);

/**
\ingroup Subnet_Management_API

<b>Description:</b>\n
	        This API returns the first subnet node information  
		by taking name of the Subnet handle as input. 

\Param[in]      subnet_handle_p - Handle to the Subnet node.	
\Param[out]     config_info - Subnet node information
\Param[out]	runtime_info - Contains the number of Subnets and the number of Subnet attributes

\Return		Success or Failure
		Success, if the Subnet is found. Else the following 
		error code is returned.
		- Could not find the Subnet.
*/
int32_t  crm_get_first_subnet (struct  crm_subnet_config_info*  config_info_p,
                           struct  subnet_runtime_info  *runtime_info,
                           uint64_t *subnet_handle_p);

/**
\ingroup Subnet_Management_API

<b>Description:</b>\n
	        This API returns the next subnet node information  
		by taking name of the Subnet handle as input. 

\Param[in]      subnet_handle_p - Handle to the Subnet node.	
\Param[out]     config_info - Subnet node information
\Param[out]	runtime_info - Contains the number of Subnets and the number of Subnet attributes

\Return		Success or Failure
		Success, if the Subnet is found. Else the following 
		error code is returned.
		- Could not find the Subnet.
*/
int32_t  crm_get_next_subnet (struct  crm_subnet_config_info*  config_info_p,
                           struct  subnet_runtime_info  *runtime_info,
                           uint64_t *subnet_handle_p);

/**
\ingroup Subnet_Management_API

<b>Description:</b>\n
		Subnet notifier callback function that should be registered by the
		the applications to get CRM events.

\Param[in]      notification_type - Type of the notification 
\Param[in]	subnet_handle - Handle of the Subnet node for which the change notification is received.
\Param[in]	notification_data - Data related to the notification received from the Subnet database.
\Param[in]	callback_arg1 - callback arg 1 passed during registration to receive 
                notifications from database.
\Param[in]	callback_arg2 - callback argument 2.

\Return		Success or Failure
		Success, if the notifier is registered successfully. Else the following 
		error code is returned.
		- Could not find Subnet handle.
*/
typedef void (*crm_subnet_notifier_fn)(uint32_t notification_type, uint64_t subnet_handle,
                                   struct  crm_subnet_notification_data  notification_data,
                                 void   *callback_arg1, void  *callback_arg2);

/**
\ingroup Subnet_Management_API

<b>Description:</b>\n
	        This API adds attributes to the subnet.

\Param[in]	subnet_name_p -  Name of the Subnet.
\Param[in]	attribute_info_p - Attributes information.

\Return		Success or Failure
		Success, if the entry is added. Else the following error code is returned.
		- Could not find the subnet.
*/
int32_t  crm_add_attribute_to_subnet(uint64_t subnet_handle,
			struct crm_attribute_name_value_pair* attribute_info_p);

/**
\ingroup Subnet_Management_API

<b>Description:</b>\n
	        This API deletes attributes from the subnet.

\Param[in]	subnet_name_p -  Name of the Subnet.
\Param[in]	attribute_info_p - Attributes information.

\Return		Success or Failure
		Success, if the subnet is deleted. Else the following error code is returned.
		- Could not find the subnet.
*/
int32_t crm_delete_attribute_from_subnet(uint64_t subnet_handle, 
                        struct crm_attribute_name_value_pair* attribute_info_p);

/**
\ingroup Subnet_Management_API

<b>Description:</b>\n
	 	This API returns the first attribute name and value pair to the caller.  
		Caller is expected to allocate memory for name_string and value_string. 
		Lengths of those buffers are expected to be passed in 'name_length' and 
		'value_length'.

\Param[in]	subnet_handle  -  Handle of the Subnet whose first attribute is required.
\Param[out]	attribute_output_p - name and value pair of the first attribute returned.

\Return		Success or Failure
		Success, if the attribute is found. Else the following error code is returned.
		- Could not find subnet.
*/
int32_t crm_get_subnet_first_attribute(uint64_t subnet_handle,
      struct   crm_attribute_name_value_output_info *attribute_output_p);

/**
\ingroup Subnet_Management_API

<b>Description:</b>\n
		This API returns the next attribute name and value pair. This function finds the 			attribute based on 'current_attribute_name' and returns the attribute information next 			to it. It is to be noted that an attribute may contain attribute name and multiple
                attribute values. 

\Param[in]	subnet_handle  - Handle of the subnet whose next attribute is required.
\Param[in]	current_attribute_name - Name of the current attribute. 
\Param[out] 	attribute_output_p - name and value pair of the next attribute and is an output parameter.

\Return		Success or Failure
		Success, if the entry is found. Else the following error code is returned.
		- Could not find subnet.
*/
int32_t crm_get_subnet_next_attribute(uint64_t subnet_handle,
      char*    current_attribute_name,
      struct   crm_attribute_name_value_output_info *attribute_output_p);

/**
\ingroup Subnet_Management_API

<b>Description:</b>\n
		This API returns the get exact Subnet attribute. This function finds the 			attribute based on 'attribute_name_string' and returns the attribute. 

\Param[in]	subnet_handle  - Handle of the subnet whose get exact attribute is required.
\Param[out] 	attribute_name_string - name of the get exact attribute and is an output parameter.

\Return		Success or Failure
		Success, if the entry is found. Else the following error code is returned.
		- Could not find subnet attribute.
*/
int32_t crm_get_subnet_exact_attribute(uint64_t subnet_handle,
      char *attribute_name_string,struct   crm_attribute_name_value_output_info *attribute_output_p);

/**
\ingroup Virtual_Network_Management_API

<b>Description:</b>\n
	        This API creates a new view for the Virtual Network (VN) database.
		The argument vn_view is the name of an attribute to the VN database 
		and the attribute can be added to all or some of the VN nodes for 
		which such a view required. CRD module may create a view based on UUID.

		Check the global array of vn_dynamic_views to find whether such a view 
		for the VN database is already created using vn_view argument. If not, 
		add the view to the global array and create a hash table for this view.
	       	A view is identified by the name of an attribute that is added to a 
		VN node after creating the VN node.	
		
		Scan the default hash table of VN nodes and find nodes containing 
		attributes with this vn_view_p string as attribute name. For each 
		attribute value of all such attributes in the identified nodes, 
		add one new node (struct database_view_node) to the newly created
	       	view hash table.

		A VN notification of the type CRM_VN_VIEW_ADDED is sent to the 
		registered Applications. A view based on vn-uuid can be created. 
		

\Param[in]	vn_view_p - A string identifying the view. Normally an Attribute name 
		is passed to identify the view.
\Param[in]	uniqueness_flag: Set to 1, if multiple VN nodes contain the same 
		attribute value for this view (Attribute name). 

\Return		Success or Failure
		Success, if the view is created. Else the following error code is returned.
		- Invalid input parameter.
*/
int32_t crm_create_vn_view(char *vn_view_p);

/**
\ingroup Virtual_Network_Management_API 

<b>Description:</b>\n
                This API deletes an existing view for the Virtual Network (VN) database.
		Check the global array of vn_dynamic_views to find whether a view identified 
		by the attribute name passed exists. If such a view is not present, return error. 

		Get the hash table handle for the view identified by the attribute name from the 
		global array of vn_dynamic_views. 

		Delete all the database_view_nodes from the hash table. Then delete the hash table itself.
		A VN notification of the type CRM_VN_VIEW_DELETE  is sent to the registered Applications
		The vn_dynamic_array is updated by removing the view identified by vn_view. 

\Param[in]	vn_view_p - A string identifying the view. Normally an Attribute name 
		is passed to identify the view.

\Return		Success or Failure
		Success, if the entry is deleted. Else the following error code is returned.
		- Could not find vn view.
*/
int32_t crm_delete_vn_view (char *vn_view_p);

/**
\ingroup Virtual_Network_Management_API

<b>Description:</b>\n
		Add the attribute to VN node identified by the argument vn_name.
	        A VN notification of the type CRM_VN_ATTRIBUTE_ADDED is sent to the registered 
		Applications.

\Param[in]	vn_name_p -  Name of the Virtual Network (VN) node.
\Param[in]	Attribute_info_p - Attribute name value pair to be added to the VN node.

\Return		Success or Failure
		Success, if the attribute is added. Else one of the following 
		error code is returned.
		- Failed to add attributes to VN.
		- Could not find VN Name.
*/
int32_t crm_add_attribute_to_vn(uint64_t vn_handle,
                                struct crm_attribute_name_value_pair* attribute_info_p);

/**
\ingroup Virtual_Network_Management_API

<b>Description:</b>\n
	 	This API deletes the given attribute from the VN node identified
		by the argument vn_name.
     		A VN notification of the type CRM_VN_ATTRIBUTE_DELETE is sent to the registered Applications.

\Param[in]	vn_name_p -  Name of the Virtual Network (VN) node.
\Param[in]	Attribute_info_p - Attribute name value pair to be deleted from the VN node.

\Return		Success or Failure
		Success, if the attribute is deleted. Else one of the following 
		error code is returned.
		- Failed to delete attributes from VN.
		- Could not find VN Name.
*/
int32_t crm_delete_attribute_from_vn(uint64_t vn_handle, 
                                     struct crm_attribute_name_value_pair* attribute_info_p);

/**
\ingroup Virtual_Network_Management_API

<b>Description:</b>\n
	 	This API returns the first attribute name and value pair to the caller.  
		Caller is expected to allocate memory for name_string and value_string. 
		Lengths of those buffers are expected to be passed in 'name_length' and 
		'value_length'.

\Param[in]	vn_handle  -  Handle of the virtual network whose first attribute is required.
\Param[out]	attribute_output - name and value pair of the first attribute returned.

\Return		Success or Failure
		Success, if the attribute is found. Else the following error code is returned.
		- Could not find virtual network handle.
*/
int32_t crm_get_vn_first_attribute(uint64_t vn_handle,
      struct   crm_attribute_name_value_output_info *attribute_output_p);

/**
\ingroup Virtual_Network_Management_API

<b>Description:</b>\n
		This API returns the next attribute name and value pair. This function finds the 			attribute based on 'current_attribute_name' and returns the attribute information next 			to it. It is to be noted that an attribute may contain attribute name and multiple
                attribute values. 

\Param[in]	vn_handle  - Handle of the switch whose next attribute is required.
\Param[in]	current_attribute_name - Name of the current attribute. 
\Param[out] 	attribute_output - name and value pair of the next attribute and is an output parameter.

\Return		Success or Failure
		Success, if the entry is found. Else the following error code is returned.
		- Could not find virtual network handle.
*/
int32_t crm_get_vn_next_attribute(uint64_t vn_handle,
      char*    current_attribute_name,
      struct   crm_attribute_name_value_output_info *attribute_output_p);

/**
\ingroup Virtual_Network_Management_API

<b>Description:</b>\n
		This API returns the get exact VN attribute. This function finds the 			attribute based on 'attribute_name_string' and returns the attribute. 

\Param[in]	vn_handle  - Handle of the VN whose get exact attribute is required.
\Param[out] 	attribute_name_string - name of the get exact attribute and is an output parameter.

\Return		Success or Failure
		Success, if the entry is found. Else the following error code is returned.
		- Could not find VN attribute.
*/
int32_t crm_get_vn_exact_attribute(uint64_t vn_handle,
      char *attribute_name_string,struct   crm_attribute_name_value_output_info *attribute_output_p);

/**
\ingroup Virtual_Network_Management_API

<b>Description:</b>\n
		Virtual network notifier callback function that should be registered by the
		the applications to get CRM events.

\Param[in]      notification_type - Type of the notification 
\Param[in]	vn_handle - Handle of the VN node for which the change notification is received.
\Param[in]	notification_data - Data related to the notification received from the vn database.
\Param[in]	callback_arg1 - callback arg 1 passed during registration to receive 
                notifications from database.
\Param[in]	callback_arg2 - callback argument 2.

\Return		Success or Failure
		Success, if the notifier is registered successfully. Else the following 
		error code is returned.
		- Could not find virtual network handle.
*/
typedef void (*crm_vn_notifier_fn)(uint32_t notification_type, uint64_t vn_handle,
                                              struct  crm_vn_notification_data  notification_data,
                                              void   *callback_arg1, void  *callback_arg2);

typedef void (*crm_port_notifier_fn)(uint32_t notification_type, uint64_t crm_port_handle,
                                              struct  crm_port_notification_data  notification_data,
                                              void   *callback_arg1, void  *callback_arg2);

/**
\ingroup Virtual_Network_Management_API 

<b>Description:</b>\n
		This API allows multiple applications to register with CRM module 
		to receive notifications whenever a VN node is created/deleted/modified,
	       	a new view is added/deleted to/from a VN node, an attribute is added or 
		deleted to/from VN node, a port is added or deleted to/from a logical switch 
		associated with a VN node. Notifications are intimated to the Applications 
		by calling their registered callback functions by passing the registered
	       	callback arguments, handle to VN node, notification type, and notification data
	       	containing various parameters related to the notification. Applications may register for a specfic notification or all the notifications. 

		The registered Applications information is stored in the following list.
           	of_list_t vn_notifications [CRM_VN_LAST_NOTIFICATION_TYPE + 1];
		The list node is of type "struct crm_notification_app_hook_info".

\Param[in]	notification_type- Type of the notification.
                       Following are the notification types that can be received from VN data base.
                       - CRM_VN_ALL_NOTIFICATIONS
                       - CRM_VN_ADDED   
                       - CRM_VN_DELETE   
                       - CRM_VN_MODIFIED  
                       - CRM_VN_VIEW_ADDED
                       - CRM_VN_VIEW_DELETE  
                       - CRM_VN_ATTRIBUTE_ADDED 
                       - CRM_VN_ATTRIBUTE_DELETE  
                       - CRM_VN_SUBNET_ADDED
                       - CRM_VN_SUBNET_DELETE
\Param[in]	vn_notifier_fn - callback function pointer that should be called on getting
		CRM events.
\Param[in]	callback_arg1 - callback arg 1 passed during registration to receive 
                notifications from database.
\Param[in]	callback_arg2 - callback argument 2.

\Return		Success or Failure
		Success, if the registration is successfull. Else error code is returned.
*/
int32_t crm_register_vn_notifications (uint32_t notification_type,
                                       crm_vn_notifier_fn vn_notifier_fn,
                                       void     *callback_arg1,
                                       void     *callback_arg2);

/**
\ingroup Virtual_Network_Management_API

<b>Description:</b>\n
		This API adds one crm_compute_node to a given VN. Maximum number of
		bridges allowed is defined by the macro CRM_MAX_DPS_IN_A_CRM_COMPUTE_NODE.
 
		A new crm compute node  is instantiated using the data structure "struct crm_compute_node"
	       	and the provided parameters are copied. The Safe reference for the VN node is obtained from
	       	the VN database using the argument vn_name and that of the Physical/Virtual switch is
	       	obtained from the SW database using the argument switch_name by employing DPRM API. 
		The new crm compute node is then added to the identified VN node. Separate API is 
		provided to add ports to a given crm compute node. 

		The added compute node is assumed to support OpenFlow protocol. 

\Param[in]	vn_name - Name of the Virtual Network to which the compute node is to be added.
\Param[in]	switch_name - Name of the Physical/Virtual Switch (compute node in OpenStack terminology). 
                This switch node shall already be present in the SW database of DPRM module.  
\Param[in]	number_of_bridges - Number of logical switches hosted by the compute node.
\Param[in]	bridge_names_p_p - Names of the logical switches hosted by the compute node. OpenStack
		creates logical switches using OVS-SW and assign names like br-int and br-tun.

\Return		Success or Failure
		Success, if the entry is added. Else one of the following 
		error code is returned.
		- Could not find virtual network.
		- Invalid parameters
*/
int32_t  crm_add_crm_compute_node_to_vn(char* vn_name_p,
			char*     switch_name_p, 
			char    *bridge_name_p);
 
 
/**
\ingroup Virtual_Network_Management_API

<b>Description:</b>\n
	 	The argument vn_name is used to obtain the VN node. The crm compute node
		to be deleted is obtained from the VN Node by using sw_name from the list 
		of crm compute nodes. The attached ports are first deleted from global 
		crm_port hash table before deleting the crm compute node itself.

\Param[in]	vn_name - Name of the Virtual Network to which the compute node is to be deleted.
\Param[in]	switch_name - Name of the Physical/Virtual Switch (compute node in OpenStack terminology). 
                This switch node shall already be present in the SW database of DPRM module.  
\Param[in]	number_of_bridges - Number of logical switches hosted by the compute node.
\Param[in]	bridge_names_p_p - Names of the logical switches hosted by the compute node.
		OpenStack creates logical switches using OVS-SW and assign names like br-int and br-tun.

\Return		Success or Failure
		Success, if the entry is deleted. Else one of the following 
		error code is returned.
		- Could not find virtual network.
		- Invalid parameters
*/
int32_t  crm_del_crm_compute_node_from_vn(char*    vn_name,
					char*    sw_name_p);

/**
\ingroup Virtual_Network_Management_API 

<b>Description:</b>\n
		This API adds the given port to the appropriate port list of the 
		crm logical switch which is already added to the given crm compute node.
	       	This API shall be called only after calling crm_add_crm_compute_node_to_vn.   

		The input parameter switch_name is used to identify the crm compute node from 
		the list of crm_compute_nodes  present in the given VN. The bridge name br_name 
		is used to identify the crm logical switch to which the port is to be added.

		The bridge name br1_name and the port_name are used to obtain br1_saferef and port_saferef 
		using  DPRM API. 

		A new crm_port is instantiated using  "struct crm_port" and all the required parameters are copied. 

		If port type is VMSIDE_PORT, copy vm_name and its safe reference also.

		The crm_port created is added to the global hash table for crm_ports. The safe reference of 
		the new port is added to the list of crm_ports for the crm logical switch with 
		name br_name associated with crm_compute_node. 
		
		A VN notification of the type CRM_VN_PORT_ADDED  is sent to the registered Applications.

		A patch port is to be added twice by using this API as a patch port connects two bridges.

\Param[in]	port_name - Name of the Port
\Param[in]	port_type - Type of port
\Param[in]	vn_name - Name of the Virtual Network.
\Param[in]	switch_name - Name of the Physical/Virtual Switch (compute node in OpenStack terminology). 
                This switch node shall already be present in the SW database of DPRM module.  
\Param[in]	vm_name - Name of the Virtual Machine.
\Param[in]	br_name - Names of the logical switches hosted by the compute node. OpenStack creates
                logical switches using OVS-SW and assign names like br-int and br-tun.

\Return		Success or Failure
		Success, if the port is added. Else one of the following 
		error code is returned.
		- Could not find virtual network.
		- Could not find switch.
		- Could not find virtual machine.
		- Could not find bridge.
		- Invalid parameters
*/
int32_t crm_add_crm_vmport(char*      port_name,
                           char*      switch_name,
                           char*      br_name,
                           int32_t    port_type,
                           char*      vn_name,
                           char*      vm_name,
                           uint8_t*   vm_port_mac_p,
                           uint64_t*  output_crm_port_handle_p
                          );

/**
\ingroup Virtual_Network_Management_API 

<b>Description:</b>\n
		This API adds the given network port to the CRM database.

\Param[in]	nw_port_info - Name of the Network Port
\Param[out]	output_crm_port_handle_p - Port Handle

\Return		Success or Failure
		Success, if the port is added. Else error is returned.

*/
int32_t crm_add_crm_nwport( struct   crm_nwport_config_info *nw_port_info,
                            uint64_t   *output_crm_port_handle_p
                          );

/**
\ingroup Virtual_Network_Management_API

<b>Description:</b>\n
		This API deletes the given crm port from the appropriate port list of 
		the crm_compute_node which is associated with the given VN. 

		The input parameter sw_name is used to identify the crm_compute_node from list 
		of crm_compute_nodes present in the given VN. 
		br_name is used to identify the logical switch from which the port is deleted. 

		A patch port is to be deleted twice by using this API as a patch port connects two bridges.
		A VN notification of the type CRM_VN_PORT_DELETE is sent to the registered Applications.
		The crm_logical_switch itself is deleted if all the associated ports are deleted.

\Param[in]	port_name - Name of the Port
\Param[in]	port_type - Type of port
\Param[in]	vn_name - Name of the Virtual Network.
\Param[in]	switch_name - Name of the Physical/Virtual Switch (compute node in OpenStack terminology). 
                This switch node shall already be present in the SW database of DPRM module.  
\Param[in]	vm_name - Name of the Virtual Machine.
\Param[in]	br_name - Names of the logical switches hosted by the compute node. OpenStack creates
                logical switches using OVS-SW and assign names like br-int and br-tun.

\Return		Success or Failure
		Success, if the entry is deleted. Else one of the following 
		error code is returned.
		- Could not find virtual network.
		- Could not find switch.
		- Could not find virtual machine.
		- Could not find bridge.
		- Invalid parameters
*/
int32_t crm_del_crm_port(char*     port_name,
			 int32_t   port_type, 
			 char*     vn_name,
			 char*     sw_name,
			 char*     vm_name,
			 char*     br_name);

int32_t crm_del_crm_nwport(char* nwport_name_p);

int32_t crm_get_first_nw_crm_port(char* port_name,
                                  struct crm_nwport_config_info *crm_nwport_info);

int32_t crm_get_next_nw_crm_port(char* port_name,
                                 struct crm_nwport_config_info *crm_nwport_info);

int32_t crm_get_exact_nwport(uint64_t handle,
                             struct  crm_nwport_config_info *crm_nwport_info_p);

int32_t crm_get_first_crm_vmport(char* vn_name, char* switch_name,
                                 char* logical_switch_name, char* port_name,
                                 struct crm_port_config_info *crm_port_info);

int32_t crm_get_next_crm_vmport(char* vn_name, char* switch_name,
                                char* logical_switch_name, char* port_name,
                                struct crm_port_config_info *crm_port_info);

int32_t crm_get_exact_crm_vmport(char* vn_name, char* switch_name,
                                 char* logical_switch_name, char* port_name,
                                 struct crm_port_config_info *crm_port_info);
int32_t crm_del_crm_vmport(char*     port_name,
                           int32_t   port_type,
                           char*     vn_name,
                           char*     switch_name,
                           char*     vm_name,
                           char*     br_name);


int32_t crm_add_attribute_to_port(char* port_name_p, struct crm_attribute_name_value_pair* attribute_info_p);

int32_t crm_delete_attribute_from_port(char* port_name_p, struct crm_attribute_name_value_pair *attribute_info_p);

int32_t crm_get_port_first_attribute(char* port_name_p,
                                     struct   crm_attribute_name_value_output_info *attribute_output_p);

int32_t crm_get_port_next_attribute(char*    port_name_p,
                                    char*    current_attribute_name,
                                    struct   crm_attribute_name_value_output_info *attribute_output_p);

int32_t crm_get_port_exact_attribute(char* port_name_p,
                                     char* attribute_name_string,
                                     struct   crm_attribute_name_value_output_info *attribute_output_p);




/**
\ingroup Cloud_Resource_Management_API

<b>Description</b>\n
		Given the Virtual Network, this API returns the first crm_compute_node attached.

\Param[in]	vn_name - Name of the Virtual Network.
\Param[in]	switch_name - Name of the Physical/Virtual Switch (compute node in OpenStack terminology). 
                This switch node shall already be present in the SW database of DPRM module.  

\Return		Success or Failure
		Success, if the compute node is found. Else one of the following 
		error code is returned.
		- Could not find virtual network.
		- Could not find switch.
		- Invalid parameters
*/
int32_t crm_get_first_crm_compute_node(char *vn_name, char* switch_name);

/**
\ingroup Cloud_Resource_Management_API 

<b>Description:</b>\n
	 	Given the virtual network and a given crm_compute_node, this API returns
		the next crm_compute_node attached for the given virtual network. 

\Param[in]	vn_name - Name of the Virtual Network.
\Param[in]	switch_name - Name of the Physical/Virtual Switch (compute node in OpenStack terminology). 
                This switch node shall already be present in the SW database of DPRM module.  

\Return		Success or Failure
		Success, if the entry is found. Else one of the following 
		error code is returned.
		- Could not find virtual network.
		- Could not find switch.
		- Invalid parameters
*/
int32_t crm_get_next_crm_compute_node (char *vn_name, char* switch_name);

/**
\ingroup Cloud_Resource_Management_API

<b>Description:</b>\n
	 	Given the crm_compute_node represented by virtual network and Physical/Virtual
		Switch, this API returns the first crm_logical_switch name.

\Param[in]	vn_name - Name of the Virtual Network.
\Param[in]	switch_name - Name of the Physical/Virtual Switch (compute node in OpenStack terminology). 
                This switch node shall already be present in the SW database of DPRM module.  
\Param[out]	logical_switch_name - First Logical Switch Name

\Return		Success or Failure
		Success, if the entry is found. Else one of the following 
		error code is returned.
		- Could not find virtual network.
		- Could not find switch.
		- Invalid parameters
*/
int32_t crm_get_first_crm_logical_switch(char* vn_name, char* switch_name,
                                                                 char* logical_switch_name);

/**
\ingroup Cloud_Resource_Management_API

<b>Description:</b>\n
		Given the crm_compute_node represented by a virtual network and a 
		Physical/Virtual Switch and a crm_logical_switch name, this API returns 
		the next crm_logical_switch name.

\Param[in]	vn_name - Name of the Virtual Network.
\Param[in]	switch_name - Name of the Physical/Virtual Switch (compute node in OpenStack terminology). 
                This switch node shall already be present in the SW database of DPRM module.  
\Param[out]	logical_switch_name - Next CRM Logical Switch Name

\Return		Success or Failure
		Success, if the entry is found. Else one of the following 
		error code is returned.
		- Could not find virtual network.
		- Could not find switch.
		- Invalid parameters
*/
int32_t crm_get_next_crm_logical_switch (char* vn_name, char* switch_name,
                                                          char* logical_switch_name);

/**
\ingroup Cloud_Resource_Management_API

<b>Description:</b>\n
	 	Given the crm_logical_switch represented by a Virtual Network, a 
		Physical/Virtual Switch and a logical switch name, this API returns 
		the first crm_port name and the crm_port structure. 

\Param[in]	vn_name - Name of the Virtual Network.
\Param[in]	switch_name - Name of the Physical/Virtual Switch (compute node in OpenStack terminology). 
                This switch node shall already be present in the SW database of DPRM module.  
\Param[in]	logical_switch_name - Name of the CRM Logical Switch.
\Param[out]	port_name - Name of the First Port.
\Param[out]	crm_port - CRM port information of the first port.

\Return		Success or Failure
		Success, if the port is found. Else one of the following 
		error code is returned.
		- Could not find virtual network.
		- Could not find switch.
		- Could not find logical switch.
		- Invalid parameters
*/
int32_t crm_get_first_crm_port(char* vn_name, char* switch_name, 
			char* logical_switch_name, char* port_name,
                        struct crm_port_config_info *crm_port_info);

/**
\ingroup Cloud_Resource_Management_API

<b>Description:</b>\n
		Given the crm_logical_switch represented by a Virtual Network, a Physical/Virtual 
		Switch and a logical switch name and crm_port, this API returns the next 
		crm_port name and the next crm_port structure. 

\Param[in]	vn_name - Name of the Virtual Network.
\Param[in]	switch_name - Name of the Physical/Virtual Switch (compute node in OpenStack terminology). 
                This switch node shall already be present in the SW database of DPRM module.  
\Param[in]	logical_switch_name - Name of the CRM Logical Switch.
\Param[in/out]	port_name - Name of the previous Port, Name of the next port is returned.
\Param[out]	crm_port - CRM port information of the next port.

\Return		Success or Failure
		Success, if the port is found. Else one of the following 
		error code is returned.
		- Could not find virtual network.
		- Could not find switch.
		- Could not find logical switch.
		- Invalid parameters
*/
int32_t crm_get_next_crm_port(char* vn_name, char* switch_name, 
                              char* logical_switch_name, char* port_name,
                              struct crm_port_config_info *crm_port_info);


/**
\ingroup Virtual_Machine_Management_API 

<b>Description:</b>\n
	 	This API adds a new virtual machine (VM) to CRM-VM database. It creates a hash table 
		and adds the new virtual machine node represented by the data structure 
		"struct crm_virtual_machine" to the table. 

		This hash table provides a static view to the VM database using the argument vm_name_p.
		Separate API is provided to add dynamic views (like vm-uuid view) to the VM database by 
		adding attributes to the virtual machine nodes. A handle to the added VM node is returned
	       	so that it can be used for faster reference. 

		A VM notification of the type CRM_VM_ADDED is sent to the registered Applications.

		The argument config_info_p contains the parameters required for configuring the virtual machine.

\Param[in]	vm_name - name of the virtual machine to be added.
\Param[in]	vm_desc - description of the virtual machine to be added. 
\Param[in]	tenant_name - Name of the tenant.
\Param[in]	vm_type - One of vm types specified by enum crm_virtual_machine_type.
\Param[in]	switch_name - Name of the Physical/Virtual Switch. SW database is maintained by DPRM.   

\Param[out]	output_handle_name_p: Handle for reference to view the VM database using vm_name.

\Return		Success or Failure
		Success, if the entry is added. Else one of the following 
		error code is returned.
		- Invalid parameter value.
		- Memory allocation failure to create entry
*/
int32_t crm_add_virtual_machine (struct crm_vm_config_info* config_info_p,
                                                       uint64_t* output_vm_handle_p);

/**
\ingroup Virtual_Machine_Management_API 

<b>Description:</b>\n
		This API deletes a given virtual machine using the vm_name provided.

\Param[in]	vm_name_p - name of the virtual machine to be deleted.

\Return		Success or Failure
		Success, if the entry is deleted. Else the following 
		error code is returned.
		- Virtual machine name could not be found.
*/
int32_t crm_del_virtual_machine (char* vm_name_p);

int32_t  crm_modify_virtual_machine(struct crm_vm_config_info* config_info_p, uint64_t*  vm_handle );

/**
\ingroup Virtual_Machine_Management_API

<b>Description:</b>\n
		This API attaches a logical switch side port which is connected towards
		the given VM to the VM database. In case of ovs switch, this port lies on
	       	the integration bridge br-int. This port is represented by the data structure 
		"crm_port" and the port type is VMSIDE_PORT. 
		
		An instance of this port is created and filled with the API input parameters and 
		added to the list of ports for the VM identified by the arguments vm_saferef_name or vm_saferef_uuid. 

\Param[in]	port_name - Name of the port to be added.
\Param[in]	logical_sw_name - On which the port is present.
\Param[in]	vn_name - virtual network name.
\Param[in]	vm_name - virtual machine name.

\Param[out]	output_handle_p - Handle to the port created and added. 

\Return		Success or Failure
		Success, if the port is attached successfully. Else one of the following 
		error code is returned.
		- Invalid parameter value.
		- Memory allocation failure to create entry
*/

int32_t crm_attach_logical_switch_side_port_to_vm (char*    port_name,
                                                   char*    vm_name,
                                                   char*    vn_name,
                                                   uint64_t port_handle  /* Can be VM_NS port handle also */
                                                  );
/**
\ingroup Virtual_Machine_Management_API

<b>Description:</b>\n
		This API deletes a port lying on a logical switch which is connected 
		towards a given VM from the VM database.

\Param[in]	handle_vm_br_int_port - Handle to the Logical swith side port of VM

\Return		Success or Failure
		Success, if the port is deleted. Else the following error code is returned.
		- Invalid parameter value.
*/
int32_t crm_detach_logical_switch_side_port_from_vm (char *port_name, char *vm_name, char *vn_name, uint8_t* no_of_ports);

 
/**
\ingroup Virtual_Machine_Management_API

<b>Description:</b>\n
		This API gets the VM configuration information by taking the
		virtual machine name as the input parameter.

\Param[in]	vm_name - Virtual Machine name whose information should be retrieved.
\Param[out]     config_info_p - Configured VM node information
\Param[out]	runtime_info - Contains the number of Virtual Machines and the number of VM attributes

\Return		Success or Failure
		Success, if the VM is found. Else one of the following 
		error code is returned.
		- Invalid parameter value.
		- Virtual machine does not exist.
*/
int32_t crm_get_vm_by_name(char* vm_name,  
			struct  crm_vm_config_info*  config_info_p,
			struct  vm_runtime_info  *runtime_info);

/**
\ingroup Virtual_Machine_Management_API 

<b>Description:</b>\n
		This API gets the first Virtual Machine information and 
		it also returns a handle to the first Virtual machine which is required to passed 
		to get the next VM node in the database.

\Param[out]     config_info_p - Configured VM node information
\Param[out]	runtime_info - Contains the number of Virtual Machines and the number of VM attributes
\Param[out]	vm_handle_p - Handle to the First Virtual Machine 

\Return		Success or Failure
		Success, if the VM is found. Else the following error code is returned.
		- Invalid parameter value.
*/
int32_t  crm_get_first_vm (struct  crm_vm_config_info*  config_info_p,
			   struct  vm_runtime_info  *runtime_info,
			   uint64_t *vm_handle_p);

/**
\ingroup Virtual_Machine_Management_API

<b>Description:</b>\n
	        This API returns the information of the VM node which is next to the 
		VM node in the database whose handle is given as the input. 
		All information related to the next VM node is returned along with its
		handle.

\Param[out]     config_info_p - Configured VM node information
\Param[out]	runtime_info - Contains the number of Virtual Machines and the number of VM attributes
\Param[in/out]	vm_handle - Handle to the Virtual Machine; Using this handle the next VM node is retrieved.

\Return		Success or Failure
		Success, if the VM is found. Else the following error code is returned.
		- Invalid parameter value.
*/
int32_t  crm_get_next_vm (struct  crm_vm_config_info*  config_info_p,
                                                     struct  vm_runtime_info  *runtime_info,
                                                     uint64_t *  vm_handle_p);

/**
\ingroup Virtual_Machine_Management_API

<b>Description:</b>\n
	        This API creates a new view for the Virtual Machine(VM) database.
		The argument vm_view_p is the name of an attribute to the VM database 
		and the attribute can be added to all or some of the VM nodes for 
		which such a view required. CRD module may create a view based on UUID.

		A VM notification of the type CRM_VM_VIEW_ADDED is sent to the 
		registered Applications. A view based on vm-uuid can be created. 

\Param[in]	vm_view_p - A string identifying the view. Normally an Attribute name 
		is passed to identify the view.
\Param[in]	uniqueness_flag: Set to 1, if multiple VM nodes contain the same 
		attribute value for this view (Attribute name). 

\Return		Success or Failure
		Success, if the view is created. Else the following error code is returned.
		- Invalid parameter value.
*/
int32_t crm_create_vm_view(char *vm_view_p);

/**
\ingroup Virtual_Machine_Management_API

<b>Description:</b>\n
	        This API deletes view of the Virtual Machine(VM) database.
		A VM notification of the type CRM_VM_VIEW_DELETE is sent to the 
		registered Applications. 

\Param[in]	vm_view_p - A string identifying the view. Normally an Attribute name 	
		is passed to identify the view.

\Return		Success or Failure
		Success, if the view is deleted. Else the following error code is returned.
		- Invalid parameter value.
*/
int32_t crm_delete_vm_view (char *vm_view_p);

/**
\ingroup Virtual_Machine_Management_API

<b>Description:</b>\n
		Add the attribute to VM node identified by the argument vm_name.
		A VM notification of the type CRM_VM_ATTRIBUTE_ADDED is sent to the registered 
		Applications.

\Param[in]	vm_name_p -  Name of the Virtual machine (VM) node.
\Param[in]	attribute_info_p - Attribute name value pair to be added to the VM node.

\Return		Success or Failure
		Success, if the attribute is added. Else the following error code is returned.
		- Invalid parameter value.
*/
int32_t crm_add_attribute_to_vm(uint64_t vm_handle,
			struct crm_attribute_name_value_pair* attribute_info_p);

/**
\ingroup Virtual_Machine_Management_API

<b>Description:</b>\n
	 	This API deletes the given attribute from the VM node identified
		by the argument vm_name.
     		A VM notification of the type CRM_VM_ATTRIBUTE_DELETE is sent to the registered Applications.

\Param[in]	vm_name_p -  Name of the Virtual machine (VM) node.
\Param[in]	attribute_info_p - Attribute name value pair to be added to the VM node.

\Return		Success or Failure
		Success, if the attribute is deleted. Else the following error code is returned.
		- Invalid parameter value.
*/
int32_t crm_delete_attribute_from_vm(uint64_t vm_handle, 
    struct crm_attribute_name_value_pair* attribute_info_p);

/**
\ingroup Virtual_Machine_Management_API

<b>Description:</b>\n
	 	This API returns the first attribute name and value pair to the caller.  
		Caller is expected to allocate memory for name_string and value_string. 
		Lengths of those buffers are expected to be passed in 'name_length' and 
		'value_length'.

\Param[in]	vm_handle  -  Handle of the virtual machine whose first attribute is required.
\Param[out]	attribute_output   - name and value pair of the first attribute returned.

\Return		Success or Failure
		Success, if the attribute is found. Else the following error code is returned.
		- Could not find virtual machine handle.
*/
int32_t crm_get_vm_first_attribute(uint64_t vn_handle,
      struct   crm_attribute_name_value_output_info *attribute_output_p);

/**
\ingroup Virtual_Machine_Management_API

<b>Description:</b>\n
		This API returns the next attribute name and value pair. This function first finds
		the attribute based on 'current_attribute_name' and returns the attribute information
		next to it. It is to be noted that an attribute may contain attribute name and multiple attribute values. 

\Param[in]	vm_handle  - Handle of the virtual machine whose next attribute is required.
\Param[in]	current_attribute_name - Name of the current attribute. 
\Param[out] 	attribute_output - name and value pair of the next attribute and is an output parameter.

\Return		Success or Failure
		Success, if the attribute is found. Else the following error is returned.
		- Could not find virtual machine handle.
*/
int32_t crm_get_vm_next_attribute(uint64_t vn_handle,
      char*    current_attribute_name,
      struct   crm_attribute_name_value_output_info *attribute_output_p);

/**
\ingroup Virtual_Machine_Management_API

<b>Description:</b>\n
		This API returns the get exact attribute name and value pair.

\Param[in]	vm_handle  - Handle of the virtual machine whose get exact attribute is required.
\Param[in, out] attribute_name_string - name and value pair of the get exact attribute and is an output parameter.

\Return		Success or Failure
		Success, if the attribute is found. Else the following error is returned.
		- Could not find virtual machine handle.
*/
int32_t crm_get_vm_exact_attribute(uint64_t vn_handle,
      char *attribute_name_string,struct   crm_attribute_name_value_output_info *attribute_output_p);

/**
\ingroup Virtual_Machine_Management_API

<b>Description:</b>\n
		Virtual machine notifier callback function that should be registered by the
		the applications to get CRM events.

\Param[in]      notification_type - Type of the notification 
\Param[in]	vm_handle - Handle of the VM node for which the change notification is received.
\Param[in]	notification_data - Data related to the notification received from the vm database.
\Param[in]	callback_arg1 - callback arg 1 passed during registration to receive 
                notifications from database.
\Param[in]	callback_arg2 - callback argument 2.

\Return		Success or Failure
		Success, if the notifier is registered. Else the following error code is returned.
		- Invalid parameter value.
*/
typedef void (*crm_vm_notifier_fn)(uint32_t notification_type, uint64_t vm_handle,
                                              struct  crm_vm_notification_data  notification_data,
                                              void   *callback_arg1, void  *callback_arg2);
/**
\ingroup Virtual_Machine_Management_API

<b>Description:</b>\n
		This API allows multiple applications to register with CRM module 
		to receive notifications whenever a VM node is created/deleted/modified,
	       	a new view is added/deleted to/from a VM node, an attribute is added or 
		deleted to/from VM node, a port is added or deleted to/from a logical switch 
		associated with a VM node. Notifications are intimated to the Applications 
		by calling their registered callback functions by passing the registered
	       	callback arguments, handle to VM node, notification type, and notification data
	       	containing various parameters related to the notification. Applications may
	       	register for a specific notification or all the notifications. 

		The registered Applications information is stored in the following list.
           	of_list_t vm_notifications [CRM_VM_LAST_NOTIFICATION_TYPE + 1];
		The list node is of type "struct crm_notification_app_hook_info".

\Param[in]	notification_type- Type of the notification.
                       Following are the notification types that can be received from VM database.
                       - CRM_VM_ALL_NOTIFICATIONS
                       - CRM_VM_ADDED   
                       - CRM_VM_DELETE   
                       - CRM_VM_MODIFIED  
                       - CRM_VM_VIEW_ADDED
                       - CRM_VM_VIEW_DELETE  
                       - CRM_VM_ATTRIBUTE_ADDED 
                       - CRM_VM_ATTRIBUTE_DELETE
                       - CRM_VM_LOGICAL_SWITCH_SIDE_PORT_ADDED
                       - CRM_VM_LOGICAL_SWITCH_SIDE_PORT_DELETE 

\Param[in]	vm_notifier_fn - callback function pointer that should be called on getting
		CRM events.
\Param[in]	callback_arg1 - callback arg 1 passed during registration to receive 
                notifications from database.
\Param[in]	callback_arg2 - callback argument 2.

\Return		Success or Failure
		Success, if the notifier is registered. Else error is returned.
*/
int32_t crm_register_vm_notifications (uint32_t notification_type,
                                      crm_vm_notifier_fn vm_notifier_fn,
                                      void     *callback_arg1,
                                      void     *callback_arg2);

/**
\ingroup Tenant_Management_API

<b>Description:</b>\n
	 	This API adds a new tenant node to CRM Tenant database. It creates a hash table 
		and adds the new tenant node represented by the data structure 
		"struct crm_tenant" to the table. 

		A notification of the type CRM_TENANT_ADDED is sent to the registered Applications.

		The argument config_info_P contains the parameters required for configuring the tenant.

\Param[in]	tenant_name_p - name of the tenant
\Param[in]	config_info_p - tenant information such as tenant name and list of VN nodes in the tenant.   

\Param[out]	output_tenant_handle_p: Handle for reference to view the tenant database using tenant_name.

\Return		Success or Failure
		Success, if the tenant is added. Else one of the following 
		error code is returned.
		- Invalid parameter value.
		- Memory allocation failure to create entry
*/
int32_t crm_add_tenant(struct crm_tenant_config_info* config_info_p,
			uint64_t* output_tenant_handle_p);

/**
\ingroup Tenant_Management_API

<b>Description:</b>\n
	 	This API deletes tenant node from CRM Tenant database.

		A notification of the type CRM_TENANT_DELETE is sent to the registered Applications.


\Param[in]	tenant_name_p - name of the tenant

\Return		Success or Failure
		Success, if the tenant is deleted. Else the following error code is returned.
		- Invalid parameter value.
*/
int32_t crm_del_tenant(char*  tunnel_name_p);

/**
\ingroup Tenant_Management_API

<b>Description:</b>\n
		This API gets Tenant Configuration information by taking the 
		tenant name as the input parameter.

\Param[in]	tenant_name - name of the tenant.
\Param[out]	config_info_p - Tenant configuration information.
\Param[out]	runtime_info - Contains the number of tenants and the number of tenant attributes.

\Return		Success or Failure
		Success, if the tenant is found. Else the following error code is returned.
		- Invalid parameter value.
*/
int32_t crm_get_tenant_by_name(char* tenant_name,  
	struct  crm_tenant_config_info*  config_info_p,
	struct  tenant_runtime_info  *runtime_info);

/**
\ingroup Tenant_Management_API 

<b>Description:</b>\n
		This API the information of the first tenant information and 
		it also returns a handle to the first tenant.

\Param[out]     config_info_p - Configured Tenant node information
\Param[out]	runtime_info - Contains the number of tenants and the number of tenant attributes
\Param[out]	tenant_handle_p - Handle to the First tenant

\Return		Success or Failure
		Success, if the tenant is found. Else the following error code is returned.
		- Invalid parameter value.
*/
int32_t  crm_get_first_tenant (struct  crm_tenant_config_info*  config_info_p,
	struct  tenant_runtime_info  *runtime_info,
	uint64_t *  tenant_handle_p);

/**
\ingroup Tenant_Management_API

<b>Description:</b>\n
	        This API returns the information of the tenant node which is next to the 
		tenant node in the database whose handle is given as the input. 
		All information related to the next tenant node is returned along with its
		handle.

\Param[out]     config_info_p - Configured tenant node information
\Param[out]	runtime_info - Contains the number of tenant and the number of tenant attributes
\Param[in/out]	tenant_handle - Handle to the tenant; Using this handle the next tenant node is retrieved.

\Return		Success or Failure
		Success, if the tenant is found. Else the following error is returned.
		- Invalid parameter value.
*/
int32_t  crm_get_next_tenant (struct  crm_tenant_config_info*  config_info_p,
			struct  tenant_runtime_info  *runtime_info,
			uint64_t *  tenant_handle_p);

/**
\ingroup Tenant_Management_API

<b>Description:</b>\n
	        This API creates a new view for the Tenant database.
		The argument tenant_view_p is the name of an attribute to the Tenant database 
		and the attribute can be added to all or some of the tenant nodes for 
		which such a view required. CRD module may create a view based on UUID.

		A tenant notification of the type CRM_TENANT_VIEW_ADDED is sent to the 
		registered Applications. A view based on tenant-uuid can be created. 

\Param[in]	tenant_view_p - A string identifying the view. Normally an Attribute name 
		is passed to identify the view.
\Param[in]	uniqueness_flag: Set to 1, if multiple tenant nodes contain the same 
		attribute value for this view (Attribute name). 

\Return		Success or Failure
		Success, if the view is created. Else the following error code is returned.
		- Invalid parameter value.
*/
int32_t crm_create_tenant_view(char *tenant_view_p, uint8_t uniqueness_flag);

/**
\ingroup Tenant_Management_API

<b>Description:</b>\n
	        This API deletes view of the tenant database.
		The argument tenant_view_p is the name of an attribute to the tenant database 
		and the attribute can be deleted all or some of the tenant nodes for 
		which such a view exists.

		A tenant notification of the type CRM_TENANT_VIEW_DELETE is sent to the 
		registered Applications. 

\Param[in]	tenant_view_p - A string identifying the view. Normally an Attribute name 	
		is passed to identify the view.

\Return		Success or Failure
		Success, if the view is deleted. Else the following error is returned.
		- Invalid parameter value.
*/
int32_t crm_delete_tenant_view (char *tenant_view_p);

/**
\ingroup Tenant_Management_API

<b>Description:</b>\n
		Add the attribute to tenant node identified by the argument tenant_name_p.
		A Tenant notification of the type CRM_TENANT_ATTRIBUTE_ADDED is sent to the registered 
		Applications.

\Param[in]	tenant_name_p -  Name of the tenant node.
\Param[in]	attribute_info_p - Attribute name value pair to be added to the tenant node.

\Return		Success or Failure
		Success, if the attribute is added. Else the following error is returned.
		- Invalid parameter value.
*/
int32_t  crm_add_attribute_to_tenant(uint64_t tenant_handle,
			struct crm_attribute_name_value_pair* attribute_info_p);

/**
\ingroup Tenant_Management_API

<b>Description:</b>\n
	 	This API deletes the given attribute from the tenant node identified
		by the argument tenant_name_p.
     		A tenant notification of the type CRM_TENANT_ATTRIBUTE_DELETE is sent to the registered Applications.

\Param[in]	tenant_name_p -  Name of the tenant node.
\Param[in]	attribute_info_p - Attribute name value pair to be added to the tenant node.

\Return		Success or Failure
		Success, if the attribute is deleted. Else the following error is returned.
		- Invalid parameter value.
*/
int32_t crm_delete_attribute_from_tenant(uint64_t tenant_handle, 
			struct crm_attribute_name_value_pair* attribute_info_p);

/**
\ingroup Tenant_Management_API

<b>Description:</b>\n
	 	This API returns the first attribute name and value pair to the caller.  
		Caller is expected to allocate memory for name_string and value_string. 
		Lengths  of those buffers are expected to be passed in 'name_length' and 
		'value_length'.

\Param[in]	tenant_handle  -  Handle of the tenant whose first attribute is required.
\Param[out]	attribute_output   - name and value pair of the first attribute returned.

\Return		Success or Failure
		Success, if the attribute is found. Else the following error is returned.
		- Could not find tenant handle.
*/
int32_t crm_get_tenant_first_attribute(uint64_t tenant_handle,
                                     struct   crm_attribute_name_value_output_info *attribute_output_p);

/**
\ingroup Tenant_Management_API

<b>Description:</b>\n
		This API returns the next attribute name and value pair. The arguments
		current_attribute_name and current_attribute_value constitute the relative 
		attribute. This function first finds the attribute based on 'current_attribute_name'
	       	and current_attribute_value and returns the attribute information next to it.
	       	It is to be noted that an attribute may contain attribute name and multiple attribute values. 

\Param[in]	tenant_handle  - Handle of the tenant whose next attribute is required.
\Param[in]	current_attribute_name - Name of the current attribute. 
\Param[in]	current_attribute_value - Value of the current attribute.
\Param[out] 	attribute_output - name and value pair of the next attribute and is an output parameter.

\Return		Success or Failure
		Success, if the next attribute is found. Else the following error is returned.
		- Could not find tenant handle.
*/
int32_t crm_get_tenant_next_attribute(uint64_t tenant_handle,
      char*    current_attribute_name,
      struct   crm_attribute_name_value_output_info *attribute_output_p);

int32_t crm_get_tenant_exact_attribute(uint64_t tenant_handle,
      char *attribute_name_string, struct   crm_attribute_name_value_output_info *attribute_output_p);


/**
\ingroup Tenant_Management_API

<b>Description:</b>\n
		Tenant notifier callback function that should be registered by the
		the applications to get CRM events.

\Param[in]      notification_type - Type of the notification 
\Param[in]	tenant_handle - Handle of the tenant node for which the change notification is received.
\Param[in]	notification_data - Data related to the notification received from the tenant database.
\Param[in]	callback_arg1 - callback argument 1 passed during registration to receive 
                notifications from database.
\Param[in]	callback_arg2 - callback argument 2.

\Return		Success or Failure
		Success, if the notifier is registered. Else the following error is returned.
		- Invalid parameter value.
*/
typedef void (*crm_tenant_notifier_fn)(uint32_t notification_type, 
                                                     uint64_t tenant_handle,
                                                    struct  crm_tenant_notification_data  notification_data,
                                                    void   *callback_arg1, void  *callback_arg2);

/**
\ingroup Tenant_Management_API 

<b>Description:</b>\n
		This API allows multiple applications to register with CRM module 
		to receive notifications whenever a tenant node is created/deleted/modified,
	       	a new view is added/deleted to/from a tenant node, an attribute is added or 
		deleted to/from tenant node, a port is added or deleted to/from a logical switch 
		associated with a tenant node. Notifications are intimated to the Applications 
		by calling their registered callback functions by passing the registered
	       	callback arguments, handle to tenant node, notification type, and notification data
	       	containing various parameters related to the notification. Applications may
	       	register for a specific notification or all the notifications. 

		The registered Applications information is stored in the following list.
           	of_list_t tenant_notifications [CRM_TENANT_LAST_NOTIFICATION_TYPE + 1];
		The list node is of type "struct crm_notification_app_hook_info".

\Param[in]	notification_type - Type of the notification.
                       Following are the notification types that can be received from tenant database.
                       - CRM_TENANT_ALL_NOTIFICATIONS
                       - CRM_TENANT_ADDED   
                       - CRM_TENANT_DELETE   
                       - CRM_TENANT_MODIFIED  
                       - CRM_TENANT_VIEW_ADDED
                       - CRM_TENANT_VIEW_DELETE
                       - CRM_TENANT_ATTRIBUTE_ADDED 
                       - CRM_TENANT_ATTRIBUTE_DELETE  
\Param[in]	tenant_notifier_fn - callback function pointer that should be called on getting
		CRM events.
\Param[in]	callback_arg1 - callback argument 1 passed during registration to receive 
                notifications from database.
\Param[in]	callback_arg2 - callback argument 2.

\Return		Success or Failure
		Success, if the notifier is registered. Else error is returned.
*/
int32_t crm_register_tenant_notifications (uint32_t notification_type,
				crm_tenant_notifier_fn tenant_notifier_fn,
				void     *callback_arg1,
				void     *callback_arg2);



#endif
