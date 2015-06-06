#ifndef __DPRM_H
#define __DPRM_H
/* 

Copyright  2012, 2013  Freescale Semiconductors


Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at:

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
 */

/* 
File name:  dprmapi.h
Date: 01/14/2013

Description: This file contains API functions to register datapath resources
with ON Director layer. It is expected that some external configuration 
applications magically figures out the virtual switches,physical switches, 
data path instances (data path switches) within virtual and physical switches, 
OF tables used by datapath switches and ports attached to datapath switches.
Such Applications register the learnt resources with the ON Director layer.  

One example of external configuration application using these API:

In case of Openstack based cloud orchestration tool,Quantum is the network 
portion of openstack.  Quantum agent in compute nodes is used to create
virtual switches and datapath switches in Linux environment using OVS
software package. Quantum plug-in (example OVS plug-in) running in openstack
ON Director communicates with all agents running in compute hosts 

- To create virtual ON Switch.
- To create bridges (datapath switches)
- To create virtual network ports (VLANs, VxLANs etc..)

OVS Interface driver in compute hosts (called by nova-compute in compute hosts)
creates ports meant to connect to virtual machines. Essentially, quantum
ON Director with the help of agents and interface driver in compute hosts 
knows the virtual ON Switch,  datapath switches and ports associated with
datapath switches.  

Quantum ON Director passes this information to ON Director configuration application
in some form and the configuration application registers these resources with the
OF ON Director using the API defined in this file.
*/
  
 //! Switch Resource Management API
 /*! This module provides Switch resource APIs
 * \addtogroup Switch_Management_API
 */
 //! Domain Resource Management API
 /*! This module provides Domain resource APIs
 * \addtogroup Domain_Management_API
 */
 //! Datapath Resource Management API
 /*! This module provides Datapath resource APIs
 * \addtogroup Datapath_Management_API
 */
 //! Controller Resource Management API
 /*! This module provides Controller resource APIs
 * \addtogroup Controller_Management_API
 */

//! DPRM Error Management
/*! This module lists all the error values returned by various DPRM and utility modules API
* \addtogroup Error_Management
  */

/** \ingroup Switch_Management_API
  * DPRM_MAX_ATTRUBUTE_NAME_LEN 
  * - Maximum length of the attribute name
  */
#define DPRM_MAX_ATTRIBUTE_NAME_LEN 16

/** \ingroup Switch_Management_API
  * DPRM_MAX_ATTRUBUTE_VALUE_LEN 
  * - Maximum length of the attribute value
  */
#define DPRM_MAX_ATTRIBUTE_VALUE_LEN 16

/** \ingroup Error_Management
  * DPRM_SUCCESS 
  * - DPRM API Functions return this value on successful execution
  */
#define DPRM_SUCCESS 0

/** \ingroup Error_Management
  * DPRM_FAILURE 
  * - DPRM API Functions return this value on general failure in execution
  */
#define DPRM_FAILURE -1

/** \ingroup Error_Management
  * DPRM_ERROR_NO_MEM 
  * - No memory available to create the database record.
  */
#define DPRM_ERROR_NO_MEM -2

/** \ingroup Error_Management
  * DPRM_ERROR_NAME_LENGTH_NOT_ENOUGH 
  * - Name of the resource must be atleast 8 characters.
  */
#define DPRM_ERROR_NAME_LENGTH_NOT_ENOUGH -3

/** \ingroup Error_Management
  * DPRM_ERROR_INVALID_SWITCH_TYPE 
  * - Type of the ON Switch is invalid.Valid types are PHYSICAL_SWITCH and VIRTUAL_SWITCH. 
  */
#define DPRM_ERROR_INVALID_SWITCH_TYPE -4

/** \ingroup Error_Management 
 * DPRM_ERROR_INVALID_IPADDR 
 * - IPV4 Address is invalid. 
 */
#define DPRM_ERROR_INVALID_IPADDR -5

/** \ingroup Error_Management
  * DPRM_ERROR_DUPLICATE_RESOURCE 
  * - A resource with this name is already present in database.
  */
#define DPRM_ERROR_DUPLICATE_RESOURCE -6

/** \ingroup Error_Management
  * DPRM_ERROR_INVALD_SWITCH_HANDLE 
  * - No ON Switch with this handle is present in database.
  */
#define DPRM_ERROR_INVALID_SWITCH_HANDLE  -7 

/** \ingroup Error_Management
  *  DPRM_ERROR_INVALID_NAME 
  *  - No resource with this name is present in database.
  */
#define DPRM_ERROR_INVALID_NAME    -8

/** \ingroup Error_Management
  *  DPRM_ERROR_NO_MORE_ENTRIES 
  *  - No more resource entries in the database.
  */
#define DPRM_ERROR_NO_MORE_ENTRIES -9 

/** \ingroup Error_Management
  *  DPRM_ERROR_RESOURCE_NOTEMPTY 
  *  - Need to delete the associated resources before un-registering the main resource. 
  */
#define DPRM_ERROR_RESOURCE_NOTEMPTY -10     

/** \ingroup Error_Management
 *  DPRM_ERROR_NO_OF_MATCH_FIELDS 
 *  - Not enough space in config structure to copy match field properties of the table. 
 */
#define DPRM_ERROR_NO_OF_MATCH_FIELDS -11

/** \ingroup Error_Management
  * DPRM_ERROR_INVALID_DATAPATH_HANDLE 
  * - Datapath handle provided as an input is invalid.
  */
#define DPRM_ERROR_INVALID_DATAPATH_HANDLE -12

/** \ingroup Error_Management
  * DPRM_ERROR_INVALID_DOMAIN_HANDLE 
  * - Domain handle provided as an input is invalid.
  */
#define DPRM_ERROR_INVALID_DOMAIN_HANDLE -13

#if 0
/** \ingroup Error_Management
  * DPRM_ERROR_INVALID_OFTABLE_HANDLE 
  * - Invalid oftable handle. 
  */
#define DPRM_ERROR_INVALID_OFTABLE_HANDLE -14
#endif

/** \ingroup Error_Management
  * DPRM_ERROR_INVALID_PORT_HANDLE 
  * - Invalid port handle. 
  */
#define DPRM_ERROR_INVALID_PORT_HANDLE -15

/** \ingroup Error_Management
  * DPRM_ERROR_INVALID_SWITCH_NOTIFICATION_TYPE 
  * - Switch notification type specified is invalid. 
  */
#define DPRM_ERROR_INVALID_SWITCH_NOTIFICATION_TYPE -16

/** \ingroup Error_Management
  * DPRM_ERROR_NULL_CALLBACK_SWITCH_NOTIFICATION_HOOK 
  * - Switch notification callback function specified is invalid. 
  */
#define DPRM_ERROR_NULL_CALLBACK_SWITCH_NOTIFICATION_HOOK -17

/** \ingroup Error_Management 
  * DPRM_ERROR_NO_SWITCH_NOTIFICATION  
  * No ON Switch notification registration was done earlier. 
  */
#define DPRM_ERROR_NO_SWITCH_NOTIFICATION -18

/** \ingroup Error_Management 
  * DPRM_ERROR_INVALID_DOMAIN_NOTIFICATION_TYPE 
  * - Domain notification type specified is invalid. 
  */
#define DPRM_ERROR_INVALID_DOMAIN_NOTIFICATION_TYPE -19

/** \ingroup Error_Management
  * DPRM_ERROR_NULL_CALLBACK_DOMAIN_NOTIFICATION_HOOK 
  * - Domain notification callback function specified is invalid. 
  */
#define DPRM_ERROR_NULL_CALLBACK_DOMAIN_NOTIFICATION_HOOK -20

/** \ingroup Error_Management
  * DPRM_ERROR_NO_DOMAIN_NOTIFICATION  
  * - No domain notification registration was done earlier. 
  */
#define DPRM_ERROR_NO_DOMAIN_NOTIFICATION -21

/** \ingroup Error_Management
  * DPRM_ERROR_INVALID_DATAPATH_NOTIFICATION_TYPE 
  * - Datapath notification type specified is invalid. 
  */
#define DPRM_ERROR_INVALID_DATAPATH_NOTIFICATION_TYPE -22

/** \ingroup Error_Management
  * DPRM_ERROR_NULL_CALLBACK_DATAPATH_NOTIFICATION_HOOK 
  * - Datapath notification callback function specified is invalid. 
  */
#define DPRM_ERROR_NULL_CALLBACK_DATAPATH_NOTIFICATION_HOOK -23

/** \ingroup Error_Management
  * DPRM_ERROR_NO_DATAPATH_NOTIFICATION  
  * - No datapath notification registration was done earlier. 
  */
#define DPRM_ERROR_NO_DATAPATH_NOTIFICATION -24

/** \ingroup Error_Management
  * DPRM_ERROR_ATTRIBUTE_NAME_NULL  
  * - Attribute name is not provided. 
  */
#define DPRM_ERROR_ATTRIBUTE_NAME_NULL -25

/** \ingroup Error_Management
  * DPRM_ERROR_ATTRIBUTE_VALUE_NULL  
  * - Attribute value is not provided. 
  */
#define DPRM_ERROR_ATTRIBUTE_VALUE_NULL -26

/** \ingroup Error_Management 
  * DPRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN  
  * - Attribute name is too long. 
  */
#define DPRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN   -27

/** \ingroup Error_Management
  * DPRM_ERROR_INVALID_ATTRIBUTE_VALUE_LEN  
  * -Attribute value is too long. 
  */
#define DPRM_ERROR_INVALID_ATTRIBUTE_VALUE_LEN  -28

/** \ingroup Error_Management 
  * DPRM_ERROR_INVALID_CONTROLLER_HANDLE 
  * - Controller Handle is not valid.
  */
#define DPRM_ERROR_INVALID_CONTROLLER_HANDLE -29

/** \ingroup Error_Management
  * DPRM_ERROR_INVALID_ENDPOINT_INFO 
  * - Invalid Transport endpoint.
  */
#define DPRM_ERROR_INVALID_ENDPOINT_INFO  -30

/** \ingroup Error_Management
  * DPRM_ERROR_INVALID_ENDPOINT_INFO 
  * - System Error, unable to add transport endpoint.
  */
#define DPRM_ERROR_FAILED_TO_ADD_ENDPOINT  -31


#define DPRM_ERROR_INVALID_NAMESPACE_HANDLE -32

/** Switch specific definitions */

/** \ingroup Switch_Management_API
  * VIRTUAL_SWITCH 
  * - Type of the ON Switch is virtual.
  */
#define VIRTUAL_SWITCH  0

/** \ingroup Switch_Management_API
  * PHYSICAL_SWITCH 
  * - Type of the ON Switch is physical.
  */
#define PHYSICAL_SWITCH 1

/** \ingroup Switch_Management_API
  * DPRM_MAX_SWITCH_NAME_LEN 
  * - Maximum length of the ON Switch name
  */
#define DPRM_MAX_SWITCH_NAME_LEN 128
#define DPRM_MAX_LOGICAL_SWITCH_NAME_LEN 128

/** notification_types that can be received from the ON Switch database */

/** \ingroup Switch_Management_API
  * DPRM_SWITCH_ALL_NOTIFICATIONS 
  * - All the Switch notifications types supported.
  */
#define DPRM_SWITCH_ALL_NOTIFICATIONS 0

/** \ingroup Switch_Management_API
  * DPRM_SWITCH_ADDED 
  * - A Switch is added to the data base.
  */
#define DPRM_SWITCH_ADDED  1

/** \ingroup Switch_Management_API
  * DPRM_SWITCH_DELETE 
  * - A Switch is deleted from the data base.
  */
#define DPRM_SWITCH_DELETE 2 

/** \ingroup Switch_Management_API
  * DPRM_SWITCH_MODIFIED 
  * - A Switch in the database is modified
  */
#define DPRM_SWITCH_MODIFIED 3

/** \ingroup Switch_Management_API
  * DPRM_SWITCH_ATTRIBUTE_ADDED 
  * - A new attribute is added to a Switch in the data base.
  */
#define DPRM_SWITCH_ATTRIBUTE_ADDED   4

/** \ingroup Switch_Management_API
  * DPRM_SWITCH_ATTRIBUTE_DELETE 
  * - An attribute is deleted from a Switch in the data base.
  */
#define DPRM_SWITCH_ATTRIBUTE_DELETE     5

/** \ingroup Switch_Management_API
  * DPRM_SWITCH_DATAPATH_ATTACHED 
  * - A new datapath is attached to a Switch in the data base.
  */
#define DPRM_SWITCH_DATAPATH_ATTACHED     6

/** \ingroup Switch_Management_API
  * DPRM_SWITCH_DATAPATH_DETACH 
  * - A datapath is deleted from a Switch in the data base.
  */
#define DPRM_SWITCH_DATAPATH_DETACH     7

/* namespace */
#define DPRM_MAX_NAMESPACE_NAME_LEN 16
#define DPRM_NAMESPACE_ALL_NOTIFICATIONS 0
#define DPRM_NAMESPACE_CREATED     1
#define DPRM_NAMESPACE_REMOVED    2
#define DPRM_NAMESPACE_DATAPATH_ATTACHED 3
#define DPRM_NAMESPACE_DATAPATH_DETACHED 4
#define DPRM_NAMESPACE_PORT_ATTACHED 5
#define DPRM_NAMESPACE_PORT_DETACHED 6

#define DPRM_NAMESPACE_FIRST_NOTIFICATION_TYPE DPRM_NAMESPACE_ALL_NOTIFICATIONS
#define DPRM_NAMESPACE_LAST_NOTIFICATION_TYPE  DPRM_NAMESPACE_PORT_DETACHED

/** \ingroup Switch_Management_API
  \struct dprm_namespace_info 
  \brief This structure carries the name of the namespaces.
  */
struct dprm_namespace_info{
  /** Name of the namespace which must be unique among the namespaces*/
  char name[DPRM_MAX_NAMESPACE_NAME_LEN + 1];
 };


/** \ingroup Switch_Management_API
  \struct ipv4_addr 
  \brief This structure carries the ipv4 Address of the ON Switch if configured.
  */
struct ipv4_addr{
   /** 1 = ipv4 Address Configured, 0 = ipv4 Address not Configured */
   int32_t baddr_set;
   /** ipv4 address in hexadecimal format */
   uint32_t addr;
 };

/** \ingroup Switch_Management_API
  \struct ipv6_addr 
  \brief This structure carries the ipv6 Address of the ON Switch if configured.
  */
struct ipv6_addr{
   /** 1 = ipv6 Address Configured, 0 = ipv6 Address not Configured */
   int32_t baddr_set;
   /** ipv6 address in different format */
   union {
     /** 8 bit format */
     uchar8_t ipv6Addr8[16];
     /** 16 bit format */
     uint16_t ipv6Addr16[8];
     /** 32 bit format */
     uint32_t ipv6Addr32[4];
   };
 };

/** \ingroup Switch_Management_API
  \struct dprm_switch_info 
  \brief Configuration parameters of a ON Switch.\n
  <b>Description</b>\n
  The ON Switch can either be a physical switch or a virtual switch.
  Switch is uniquely identified by its name.
  Network parameters include either FQDN or IPAddress.
  Switch supports both IPV4 and IPV6 address formats.\n
  */
struct dprm_switch_info{
  /** Name of the ON Switch which must be unique among the switches*/  
  char name[DPRM_MAX_SWITCH_NAME_LEN + 1];     
  /** Switch type is either PHYSICAL_SWITCH or VIRTUAL_SWITCH*/
  uchar8_t switch_type;         
  /** FQDN of the ON Switch and is a string*/
  char switch_fqdn[96];                  
  /** IPV4 Address of the ON Switch which is optional */
  struct ipv4_addr ipv4addr;          
  /** IPV6 Address of the ON Switch which is optional */
  struct ipv6_addr ipv6addr;          
 };

/** \ingroup Switch_Management_API  
  \struct dprm_switch_runtime_info 
  \brief Runtime information of a ON Switch\n
  <b>Description</b>\n
  Runtime information of a ON Switch includes the number of datapaths and the number
   of attributes added sofar to the ON Switch. 
  */
struct dprm_switch_runtime_info{
   /** Number of datapaths added sofar to the ON Switch.*/
   uint32_t number_of_datapaths;
   /** Number of attributes added sofar to the ON Switch.*/
   uint32_t number_of_attributes;
};

/** \ingroup Switch_Management_API  
  \struct  dprm_attribute_name_value_pair 
  \brief Provides attribute information passed as an argument to database API\n
  <b>Description</b>\n
  Attribute information includes the name of the attribute and its value.
 */
struct dprm_attribute_name_value_pair{
   /** name of the attribute which must be unique */ 
   char name_string[DPRM_MAX_ATTRIBUTE_NAME_LEN]; 
   /** value of the attribute */
   char value_string[DPRM_MAX_ATTRIBUTE_VALUE_LEN];
};

/** \ingroup Switch_Management_API  
  \struct dprm_attribute_name_value_output_info 
  \brief Provides attribute information returned from database API \n
  <b>Description</b>\n
  Attribute information includes the name of the attribute and its length and also
  the value of the attribute and its length.
  */
struct dprm_attribute_name_value_output_info{
  /** Name of the attribute */
  char    name_string[DPRM_MAX_ATTRIBUTE_NAME_LEN];
  /** length of the name string */
  int32_t name_length;
  /** value field of the attribute */
  char    value_string[DPRM_MAX_ATTRIBUTE_VALUE_LEN];
  /** length of the value string */
  int32_t value_length;
};

/** \ingroup Switch_Management_API  
  \struct dprm_switch_notification_data 
  \brief Contains data specific to the notification received from the ON Switch database.\n
  <b>Description</b>\n
  This structure is common for all the ON Switch notification types.
  Validity of each field is based on the notification_type.
 */ 
struct dprm_switch_notification_data{
   /** Name of the ON Switch - valid for all the notification types */
   char switch_name[DPRM_MAX_SWITCH_NAME_LEN + 1];
   /** attribute name - valid for the attribute add and delete notification types */
   char attribute_name[DPRM_MAX_ATTRIBUTE_NAME_LEN + 1];
   /** attribute value - valid for the add and delete notification types */
   char attribute_value[DPRM_MAX_ATTRIBUTE_VALUE_LEN + 1];
   /** handle to the datapath - valid for the datapath attach and de-attach notification types */
   uint64_t  datapath_handle;
};

/** \ingroup Switch_Management_API
  \typedef dprm_switch_notifier_fn
  \brief Prototype for the Application callback function to receive change notifications
         from the ON Switch database.\n
  <b>Description</b>\n 
  Following are the notification types that can be received from ON Switch database.
  - DPRM_SWITCH_ALL_NOTIFICATIONS
  - DPRM_SWITCH_ADDED   
  - DPRM_SWITCH_DELETE   
  - DPRM_SWITCH_MODIFIED  
  - DPRM_SWITCH_ATTRIBUTE_ADDED 
  - DPRM_SWITCH_ATTRIBUTE_DELETE   
  - DPRM_SWITCH_DATAPATH_ATTACHED   
  - DPRM_SWITCH_DATAPATH_DETACH   

  \param[in] notification_type - Type of the notification 
  \param[in] switch_handle - Handle of the ON Switch for which the change notification is received.
  \param[in] notification_data - Data related to the notification received from the ON Switch database.
  \param[in] callback_arg1 - callback argument 1 passed during registration to receive notifications from database.
  \param[in] callback_arg2 - callback argument 2. 
  */
typedef void (*dprm_switch_notifier_fn)(uint32_t notification_type,
                                        uint64_t switch_handle,
                                        struct   dprm_switch_notification_data notification_data,
                                        void     *callback_arg1,
                                        void     *callback_arg2);
struct dprm_namespace_notification_data{
   /** Name of the namespace - valid for all the notification types */
   char namespace_name[DPRM_MAX_NAMESPACE_NAME_LEN + 1];
   char port_name[DPRM_MAX_NAMESPACE_NAME_LEN + 1];
   uint64_t  dp_id;
   uint16_t  dp_ns_id;
   uint32_t  port_id;
};


/** \ingroup Switch_Management_API
  \typedef dprm_namespace_notifier_fn
  \brief Prototype for the Application callback function to receive change notifications
         from the namespace database.\n
  <b>Description</b>\n 
  Notifications that can be received from the namespace database.  

  \param[in] notification_type - Type of the notification 
  \param[in] notification_data - Data related to the notification received from the namespace database.
  \param[in] callback_arg1 - callback argument 1 passed during registration to receive notifications from database.
  \param[in] callback_arg2 - callback argument 2. 
  */
typedef void (*dprm_namespace_notifier_fn)(uint32_t notification_type,
                                        struct   dprm_namespace_notification_data notification_data,
                                        void     *callback_arg1,
                                        void     *callback_arg2);

/** \ingroup Switch_Management_API
   API functions that help in registering virtual and physical switches to
   the ON Director layer. 
  */

 /** \ingroup Switch_Management_API
   \brief Register a physical or a virtual ON Switch with OF ON Director layer.\n
   <b>Description:</b>\n
   This API informs the ON Director layer of a virtual ON Switch or a physical ON Switch found in the
   network. It is required that the ON Switch is added before any datapath instances are registered.

   In general, Openflow connections are made from datapath instances to a ON Director.
   But in some cases, Openflow ON Director may need to initiate the connection for various reasons.
   FQDN and IP address information of the datapath instance is required in such cases.

   Output value:
   Returns the handle to this ON Switch entry created. This handle is required to be used
   to refer to this entry while calling other ON Switch API. The field 'name' can be used to get the
   handle if needed at a later time.

   Output value is defined as 64 bit integer to allow handle to have index and cookie values.

   \param[in]  switch_info_p  - It contains all the ON Switch parameters for registration.
   \param[out] handle_p       - Unique handle to the registered ON Switch for faster reference.

   \return DPRM_SUCCESS                       - In case of successful registration of the ON Switch.
   \return DPRM_ERROR_INVALID_SWITCH_TYPE     - Valid ON Switch types are PHYSICAL_SWITCH or VIRTUAL_SWITCH.
   \return DPRM_ERROR_NAME_LENGTH_NOT_ENOUGH  - Switch name shall contain atleast 8 characters.
   \return DPRM_ERROR_NO_MEM                  - If there is an error in allocating memory.
   \return DPRM_ERROR_DUPLICATE_RESOURCE      - A Switch resource with the same 'name' is already present in the database.
   \return DPRM_ERROR_INVALID_IP_ADDR         - If there is an error in parsing the IP address.
   \return DPRM_FAILURE                       - Any other error.                                                                                             
   */

int32_t  dprm_register_switch(struct dprm_switch_info* switch_info_p, uint64_t* handle_p);

/** \ingroup Switch_Management_API
  \brief Un-Register a physical or a virtual ON Switch from Controller layer.\n
  <b>Description:</b>\n
  This API deletes the ON Switch from the database. 

  \param[in] handle                        - handle to the ON Switch to be un-registered. 

  \return DPRM_SUCCESS                     - if deletion is successful.
  \return DPRM_ERROR_INVALID_SWITCH_HANDLE - Switch handle is invalid. This could happen, if the ON Switch\
					     was deleted or ON Switch is not found in the database.
  \return DPRM_ERROR_RESOURCE_NOTEMPTY     - Application shall delete associated datapaths and \
					     attributes before un-registering.                                     
**/
int dprm_unregister_switch(uint64_t handle);

/** \ingroup Switch_Management_API
  \brief Update the parameters of an already registered ON Switch.\n
  <b>Description:</b>\n
  Even though argument structure is same as dprm_register_switch() function, 
  name is not expected to be updated by the caller.  Hence 'name' value is
  ignored by this function and other ON Switch parameters are updated. 

  \param[in] switch_info_p            - It contains all the ON Switch parameters, some updated\
                                        and some original values. 
  \param[in] handle                   - Unique handle to the ON Switch which is to be updated.
  
  \return DPRM_SUCCESS                - If updation is successful.
  \return DPRM_ERROR_INVALID_SWITCH_HANDLE  - If handle is invalid. This could happen, if the\
                                        ON Switch was deleted or could not find the ON Switch entry in the\
                                        database.
**/
int32_t dprm_update_switch(uint64_t handle,struct dprm_switch_info* switch_info_p);

/** \ingroup Switch_Management_API
  \brief Add a new attribute to a registered ON Switch.\n
  <b>Description:</b>\n
  An attribute contains a name and value pair whose lengths may vary.
  Attributes extend the functionality without changing the function prototype or information structures.
  
  \param[in] switch_handle           - Unique handle to the ON Switch to which a new attribute is to be added.
  \param[in] attribute_info_p        - It contains the attribute information to be added to the ON Switch. 

  \return DPRM_SUCCESS               - If attribute addition is successful.
  \return DPRM_ERROR_NO_MEM          - If there is an error in allocating memory.
  \return DPRM_ERROR_INVALID_SWITCH_HANDLE         - If the ON Switch handle is invalid.
  \return DPRM_ERROR_ATTRIBUTE_NAME_NULL           - Name of the attribute is not specified.
  \return DPRM_ERROR_ATTRIBUTE_VALUE_NULL          - Value of the attribute is not specified.
  \return DPRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN    - Attribute name is too lengthy. 
  \return DPRM_ERROR_INVALID_ATTRIBUTE_VALUE_LEN   - Attribute value is too lengthy.
  \return DPRM_ERROR_DUPLICATE_RESOURCE            - ON Switch attribute already exists with the same name.
 */
int32_t dprm_add_attribute_to_switch(uint64_t switch_handle,
     struct dprm_attribute_name_value_pair* attribute_info_p);

/** \ingroup Switch_Management_API
  \brief Delete an attribute from a registered switch.\n
  <b>Description:</b>\n
  This API deletes an existing attribute name and value pair from the ON Switch.

  \param[in] switch_handle     - Unique handle to the ON Switch from which an attribute is to be deleted. 
  \param[in] attribute_name_p  - Name of the attribute to be deleted.
  
  \return DPRM_SUCCESS             - If attribute deletion is successful.
  \return DPRM_ERROR_INVALID_SWITCH_HANDLE  - If the ON Switch handle is invalid.
 */
int32_t dprm_delete_attribute_from_switch(uint64_t switch_handle, struct dprm_attribute_name_value_pair *attribute_info_p);

/** \ingroup Switch_Management_API
  \brief Get the ON Switch handle given the "name" of the ON Switch.\n
 
  \param[in]  name_p            - Name of the ON Switch.   
  \param[out] switch_handle_p   - Handle to the ON Switch.

  \return DPRM_ERROR_INVALID_NAME  - If no ON Switch entry is found with the given name.
  \return DPRM_SUCCESS             - If the ON Switch entry is found with the given name.
  */
int32_t dprm_get_switch_handle(char* name_p,  uint64_t* switch_handle_p);

/** \ingroup Switch_Management_API
  \brief Get the ON Switch information for the given ON Switch handle.\n
  <b>Description:</b>\n
  This API is used to get the ON Switch information in the database, whose handle is provided.

  \param[in]  switch_handle  - Handle to the ON Switch.
  \param[out] config_info_p  - Switch information that was configured during registration API.
  \param[out] runtime_info_p - Runtime ON Switch information like number of attached datapaths and ON Switch attributes.

  \return DPRM_SUCCESS                     - If a ON Switch is found in the database with the given handle.
  \return DPRM_ERROR_INVALID_SWITCH_HANDLE - If no ON Switch is found in the database with the given handle.
  */
int32_t dprm_get_exact_switch(uint64_t switch_handle,
                              struct dprm_switch_info* config_info_p,
                              struct dprm_switch_runtime_info* runtime_info_p);

/** \ingroup Switch_Management_API
  \brief Get the first ON Switch information.\n
  <b>Description:</b>\n
  This API is used to get the first ON Switch in the database. This function in conjunction
  with dprm_get_next_switch() can be used to get all the registered switches one by one.

  This API returns the following ON Switch information as output arguments.
  - Switch information that was configured during registration API.
  - Runtime ON Switch information like number of attached datapaths and ON Switch attributes.
  - Handle to the Switch.\n

  \param[out] config_info    - Configured ON Switch information.
  \param[out] runtime_info   - Contains the number of datapaths and the number of ON Switch attributes.
  \param[out] switch_handle  - Handle to the ON Switch.

  \return DPRM_SUCCESS                - If a ON Switch is found in the database.
  \return DPRM_ERROR_NO_MORE_ENTRIES  - If no ON Switch is found in the database.
 */
int32_t dprm_get_first_switch(struct  dprm_switch_info *config_info,
                              struct  dprm_switch_runtime_info *runtime_info,
                              uint64_t *switch_handle);

 /** \ingroup Switch_Management_API
   \brief Get the next ON Switch information given the ON Switch handle.\n
   <b>Description:</b>\n
   This API is used to get the next ON Switch in the database given the handle to the current ON Switch.
   This API in conjunction with dprm_get_first_switch() can be used to get all the registered
   switches one by one.

   This API returns the following ON Switch information as output arguments.
   - Switch information that was configured during registration API.
   - Runtime ON Switch information like number of attached datapaths and ON Switch attributes.
   - Handle to the Switch

   Note that the list order is not same as the way the switches are registered.
   List order is based on the way the ON Director layer arranges the ON Switch information nodes.\n 

   \param[out] config_info       - Configured ON Switch information.
   \param[out] runtime_info      - Contains the number of datapaths and the number of ON Switch attributes.
   \param[in,out] switch_handle  - Handle to the ON Switch. Used both as an input and output.
  
   \return DPRM_SUCCESS                - If a ON Switch is found in the database.
   \return DPRM_ERROR_NO_MORE_ENTRIES  - If no more entries are found in the database.
   \return DPRM_ERROR_INVALID_SWITCH_HANDLE   - If no ON Switch entry is found with the given handle.
   */
int32_t dprm_get_next_switch(struct   dprm_switch_info *config_info,
                             struct   dprm_switch_runtime_info *runtime_info,
                             uint64_t *switch_handle);

/** \ingroup Switch_Management_API
  \brief Get the first attribute of a ON Switch.\n
  <b>Description:</b>\n
  This API returns the first attribute name and value pair to the caller.
  Caller is expected to allocate memory for name_string and value_string. Lengths
  of those buffers are expected to be passed in 'name_length' and 'value_length'.
  If this API does not find name and value buffers are insufficient, then 
  it returns error. Also, it updates the name_length and value_length fields 
  with the lengths required.
  
  \param[in]   switch_handle      -Handle of the ON Switch whose first attribute is required.
  \param[out]  attribute_output   -name and value pair of the first attribute.

  \return DPRM_SUCCESS                       - First attribute of the ON Switch is found.
  \return DPRM_ERROR_INVALID_SWITCH_HANDLE         - Invalid ON Switch handle. 
  \return DPRM_ERROR_NO_MORE_ENTRIES               - No Attributes are configured for the ON Switch.
  
  \return DPRM_ERROR_ATTRIBUTE_NAME_NULL           - No memory space provided for copying the attribute name.
  \return DPRM_ERROR_ATTRIBUTE_VALUE_NULL          - No memory space provided for copying the attribute value.
  \return DPRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN    - Memory space provided for copying next attribute name is not sufficient. 
  \return DPRM_ERROR_INVALID_ATTRIBUTE_VALUE_LEN   - Memory space provided for copying next attribute value is not sufficient. 
*/
int32_t dprm_get_first_switch_attribute(uint64_t switch_handle,
                 struct dprm_attribute_name_value_output_info *attribute_output);
   
/** \ingroup Switch_Management_API
  \brief Get the next attribute of a ON Switch given an attribute.\n
  <b>Description:</b>\n
  This API returns the next attribute name and value pair.Current_attribute_name is the
  relative attribute.This function first finds the attribute based on 'current_attribute_name'
  and returns the attribute information next to it.\n

  \param[in]   switch_handle          - Handle of the ON Switch whose next attribute is required.
  \param[in]   current_attribute_name - Name of the current attribute. 
  \param[out]  attribute_output       - name and value pair of the next attribute.

  \return DPRM_SUCCESS                       - Next attribute of the ON Switch is found.
  \return DPRM_ERROR_INVALID_SWITCH_HANDLE         - Invalid ON Switch handle.
  \return DPRM_ERROR_NO_MORE_ENTRIES               - No more attributes are configured for the ON Switch.
  \return DPRM_ERROR_INVALID_NAME                  - Name of the input Attribute is not valid.
  
  \return DPRM_ERROR_ATTRIBUTE_NAME_NULL           - No memory space provided for copying the attribute name.
  \return DPRM_ERROR_ATTRIBUTE_VALUE_NULL          - No memory space provided for copying the attribute value.
  \return DPRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN    - Memory space provided for copying next attribute name is not sufficient. 
  \return DPRM_ERROR_INVALID_ATTRIBUTE_VALUE_LEN   - Memory space provided for copying next attribute value is not sufficient.
*/
int32_t dprm_get_next_switch_attribute(uint64_t switch_handle,
                                       char *current_attribute_name,
    struct dprm_attribute_name_value_output_info *attribute_output);

/** \ingroup Switch_Management_API
  \brief Registration API for Applications to receive notifications from ON Switch database.\n
  <b>Description:</b>\n
  This API can be used by applications to register notifier functions to receive notifications regarding changes in the
  ON Switch database.

  Applications may pass two callback arguments. These arguments are passed back when application 
  notifier function is called.

  Multiple applications may register their notifier functions using this API.
  The registered callback functions are called in the order in which they are registered.\n
  
  \param[in] notification_type   - Specifies the event for which the callback function shall be called.
  \param[in] switch_notifier_fn  - Callback function pointer registered by the Application.
  \param[in] callback_arg1       - Argument 1 passed by Application.
  \param[in] callback_arg2       - Argument 2 passed by Application.

  \return DPRM_SUCCESS                                      - upon successful registration  
  \return DPRM_ERROR_NO_MEM                                 - Error in allocating memory
  \return DPRM_ERROR_INVALID_SWITCH_NOTIFICATION_TYPE       - Switch notification type is invalid.
  \return DPRM_ERROR_NULL_CALLBACK_SWITCH_NOTIFICATION_HOOK - callback function is NULL.
 */
int32_t dprm_register_switch_notifications(uint32_t notification_type,
                                           dprm_switch_notifier_fn switch_notifier_fn,
                                           void *callback_arg1,
                                           void *callback_arg2);

/** \ingroup Switch_Management_API
  \brief Deregisters notification function with ON Switch database. \n
  <b>Description:</b>\n
  This API can be used by applications to de-register the notification function pointers 
  with the ON Switch database so that no further notifications are received.

  \param[in] notification_type   - For which the Application wishes not to receive any further notifications.
  \param[in] switch_notifier_fn  - The function pointer that was registered earlier.
  \return DPRM_SUCCESS - De-registration successful.
  \return DPRM_ERROR_NO_SWITCH_NOTIFICATION - No such registration earlier with the database. 
*/  
int32_t dprm_deregister_switch_notifications(uint32_t notification_type,
                                             dprm_switch_notifier_fn switch_notifier_fn);

/** \ingroup Domain_Management_API
  * DPRM_MAX_FORWARDING_DOMAIN_LEN 
  * - Maximum length of the Domain name
  */
#define DPRM_MAX_FORWARDING_DOMAIN_LEN 16

/** \ingroup Domain_Management_API 
  * DPRM_DOMAIN_MAX_X509_SUBJECT_LEN 
  * - Maximum length of X509_subject name for a Domain.
  */
#define DPRM_DOMAIN_MAX_X509_SUBJECT_LEN 256

/** \ingroup Domain_Management_API
  * DPRM_MAX_LEN_OF_TABLE_NAME 
  * - Maximum length of the table name.
  */
#define DPRM_MAX_LEN_OF_TABLE_NAME 32

/** \ingroup Domain_Management_API
  * DPRM_MAX_NUMBER_OF_TABLES  
  * - Maximum number of tables supported.
  */
#define DPRM_MAX_NUMBER_OF_TABLES 256

/** \ingroup Domain_Management_API
  * DPRM_DOMAIN_ALL_NOTIFICATIONS 
  * - All the Domain notification types supported.
  */
#define DPRM_DOMAIN_ALL_NOTIFICATIONS 0

/** \ingroup Domain_Management_API 
  * DPRM_DOMAIN_ADDED 
  * - A Domain is added to the data base.
  */
#define DPRM_DOMAIN_ADDED 1

/** \ingroup Domain_Management_API
  * DPRM_DOMAIN_DELETE 
  * - A Domain is deleted from the data base.
  */
#define DPRM_DOMAIN_DELETE  2

/** \ingroup Domain_Management_API
  * DPRM_DOMAIN_MODIFIED 
  * - A Domain in the database is modified.
  */
#define DPRM_DOMAIN_MODIFIED 3
#if 0
/** \ingroup Domain_Management_API
  * DPRM_DOMAIN_OFTABLE_ADDED 
  * - A new OFTABLE  is added to a Domain in the data base.
  */
#define DPRM_DOMAIN_OFTABLE_ADDED 4

/** \ingroup Domain_Management_API
  * DPRM_DOMAIN_OFTABLE_DELETE 
  * - An oftable is deleted from a Domain in the data base.
  */
#define DPRM_DOMAIN_OFTABLE_DELETE 5
#endif

/** \ingroup Domain_Management_API
  * DPRM_DOMAIN_ATTRIBUTE_ADDED 
  * - A new attribute is added to a Domain in the data base.
  */
#define DPRM_DOMAIN_ATTRIBUTE_ADDED  6

/** \ingroup Domain_Management_API
  * DPRM_DOMAIN_ATTRIBUTE_DELETE 
  * - An attribute is deleted from a Domain in the data base.
  */
#define DPRM_DOMAIN_ATTRIBUTE_DELETE   7

/** \ingroup Domain_Management_API
  * DPRM_DOMAIN_DATAPATH_ATTACHED 
  * - A new datapath is attached to a Domain in the data base.
  */
#define DPRM_DOMAIN_DATAPATH_ATTACHED   8

/** \ingroup Domain_Management_API
  * DPRM_DOMAIN_DATAPATH_DETACH 
  * - A datapath is deleted from a Domain in the data base.
  */
#define DPRM_DOMAIN_DATAPATH_DETACH   9

/**API functions to manage distributed domains */ 

/** \ingroup Domain_Management_API
  \struct dprm_match_field
  \brief  This structure is used while adding a table to a domain resource.\n
  <b>Description</b>\n
  It conatins type of the match field supported and whether it supports mask.
  */
struct dprm_match_field{
  /** match field type */
  uint8_t type;
  /** specifies whether the match field supports mask */
  uint8_t mask;
};

/** \ingroup Domain_Management_API
  \struct dprm_table_info
  \brief  Table Information contains the "name" and the "table-id" of a table.\n
  <b>Description</b>\n
  This structure is part of "dprm_distributed_forwarding_domain_info".
  */
struct dprm_table_info{
   /** Name of the table. */
   char table_name[DPRM_MAX_LEN_OF_TABLE_NAME + 1];
   /** ID of the table. */
   uint8_t table_id;
   /** Number of match fields supported by this table */
   uint8_t no_of_match_fields;
   /** Type and mask associated with each match field supported */
   struct  dprm_match_field  match_field_p[]; 
};

/** \ingroup Domain_Management_API
  \struct dprm_distributed_forwarding_domain_info
  \brief Configuration parameters of a Domain.  \n
  <b>Description</b>\n
  A domain is uniquely identified by its "name".
 
  A datapath can be attached to a domain only if the datapath and the domain contain the
  same number of tables, identical table names and table ids. 

  If a datapath is configured with a subjectname,it is used to verify the authenticity 
  of the remote datapath.Otherwise,the subjectname configured for domain is used for
  authenticating the remote datapath.\n 
 */
#define CNTLR_MAX_TTP_NAME_LEN 64
struct dprm_distributed_forwarding_domain_info{
    /** Name of the domain which must be unique among the domains */
    char name[DPRM_MAX_FORWARDING_DOMAIN_LEN + 1];  
    /** Name of the table type pattern to which this domain belongs */
    char ttp_name[CNTLR_MAX_TTP_NAME_LEN+1];
    /** Subject name used to Authenticate the remote datapath*/
    char expected_subject_name_for_authentication[DPRM_DOMAIN_MAX_X509_SUBJECT_LEN + 1];
    /** number of name based tables added so far to the domain. Used as an input parameter */ 
    uint8_t number_of_tables;
};

/** \ingroup Domain_Management_API  
  \struct dprm_distributed_forwarding_domain_runtime_info 
  \brief Runtime information  of a domain.\n
  <b>Description</b>\n
  Runtime information of a domain includes the number of datapaths and the number
  of attributes added sofar to the domain. 
 */
struct dprm_distributed_forwarding_domain_runtime_info{
    /** Number of datapaths added sofar to the domain.*/
    int32_t number_of_datapaths;
    /** Number of attributes added sofar to the domain.*/
    int32_t number_of_attributes;
    /** number of id based tables added so far to the domain.Used as an input parameter */
    int32_t number_of_tables;
};

/** \ingroup Domain_Management_API 
  \struct dprm_domain_notification_data 
  \brief This notification_data is common to all the notification types received from domain database.\n
  <b>Description</b>\n
  This structure is common for all the notifications types.
  Validity of each field is based on the notification_type.
 */
struct dprm_domain_notification_data{
   /** Name of the domain - valid for all the notification types */
   char  domain_name[DPRM_MAX_FORWARDING_DOMAIN_LEN +1];
   /** Name of the Table Type pattern - valid for all the notification types */
   char  ttp_name[CNTLR_MAX_TTP_NAME_LEN +1];
   /** table name - valid for the oftable add and delete notification types */
   char  table_name[DPRM_MAX_LEN_OF_TABLE_NAME + 1];
   /** table id - valid for the oftable add and delete notification types */
   uint8_t  table_id;
   /** table handle - valid for the oftable add and delete notification types */
   uint64_t table_handle;
   /** attribute name - valid for the attribute add and delete notification types */
   char  attribute_name[DPRM_MAX_ATTRIBUTE_NAME_LEN + 1];
   /** attribute value - valid for the add and delete notification types */
   char  attribute_value[DPRM_MAX_ATTRIBUTE_VALUE_LEN + 1];
   /** handle to the datapath - valid for the datapath attach and de-attach notification types */
   uint64_t  datapath_handle;
};

/** \ingroup Domain_Management_API
  \typedef dprm_domain_notifier_fn
  \brief Prototype for the domain notifier function. \n
  <b>Description</b>\n
  This registered notifier function is used by the domain database module to notify the
  Appilication about any changes in the domain database.

  Following are the notification types that can be notified by the domain data base.
  - DPRM_DOMAIN_ALL_NOTIFICATIONS
  - DPRM_DOMAIN_ADDED   
  - DPRM_DOMAIN_DELETE   
  - DPRM_DOMAIN_MODIFIED  
  - DPRM_DOMAIN_OFTABLE_ADDED
  - DPRM_DOMAIN_OFTABLE_DELETE
  - DPRM_DOMAIN_ATTRIBUTE_ADDED 
  - DPRM_DOMAIN_ATTRIBUTE_DELETE   
  - DPRM_DOMAIN_DATAPATH_ATTACHED   
  - DPRM_DOMAIN_DATAPATH_DETACH   
 
  \param[in] notification_type - Type of the notification. 
  \param[in] domain_handle - Handle of the domain for which the change notification is received.
  \param[in] notification_data - contains the data received from the domain database.
  \param[in] callback_arg1 - This registered argument 1 is passed as an argument in the notification.
  \param[in] callback_arg2 - Same as argument 1. 
  */
typedef void (*dprm_domain_notifier_fn)(uint32_t notification_type,
                                        uint64_t domain_handle,
                                        struct   dprm_domain_notification_data notification_data,
                                        void     *callback_arg1,
                                        void     *callback_arg2);

/** \ingroup Domain_Management_API
  \brief Adds a domain to the database.\n
  <b>Description:</b>\n
  This API adds a domain to the database.The "name" of the domain must be unique.

  A Handle to the domain entry created in the database is returned. Its value is not supposed
  to be interpreted by the caller. This handle is expected to be passed in all other domain
  specific API.
  
  A datapath can be attached to a domain only if the datapath and the domain contain the
  same number of tables, identical table names and table ids. 

  If a datapath is configured with a subjectname, it is used to verify the authenticity 
  of the remote datapath. Otherwise the subjectname configured for domain is used for
  authenticating the remote datapath.\n 

  \param[in]  domain_info               - It contains all the domain parameters for registration. 
  \param[out] output_handle             - Handle to domain created.
 
  \return DPRM_SUCCESS                  - If the domain is successfully created.
  \return DPRM_ERROR_NAME_LENGTH_NOT_ENOUGH  - Domain name length shall be of at least 8 characters.
  \return DPRM_ERROR_NO_MEM             - If no memory available to create the domain record.
  \return DPRM_ERROR_DUPLICATE_RESOURCE - If the domain "name" is not unique. 
 */
int32_t dprm_create_distributed_forwarding_domain(
        struct  dprm_distributed_forwarding_domain_info *domain_info,
        uint64_t *output_handle);

/** \ingroup Domain_Management_API
  \brief Deletes a domain from the database.\n
  
  \param[in] domain_handle              - Handle of the domain to be deleted.
  \return DPRM_SUCCESS                  - If deletion of the domain from the database is successful.
  \return DPRM_INVALID_DOMAIN_HANDLE    - If the domain handle is invalid.
  \return DPRM_ERROR_RESOURCE_NOTEMPTY  - Need to delete associated datapaths and attributes before un-registering.   
 */
int32_t dprm_unregister_distributed_forwarding_domain(uint64_t domain_handle);


/** \ingroup Domain_Management_API
  \brief Adds a new attribute to a registered domain.\n
  <b>Description:</b>\n
   An attribute contains a name and value pair whose lengths may vary. Attributes extend the
   functionality without changing the function prototype or information structures. 
 
  Adds a new attribute name & value pair to the domain\n

  \param[in] domain_handle     - Unique handle to the domain to which a new attribute is to be added.
                      
  \param[in] attribute_info    - It contains the attribute information to be added to the domain. 
  
  \return DPRM_SUCCESS         - If attribute addition is successful.
  \return DPRM_ERROR_NO_MEM    - If there is an error in allocating memory.
  \return DPRM_ERROR_INVALID_DOMAIN_HANDLE        - If the domain handle is invalid.
  \return DPRM_ERROR_ATTRIBUTE_NAME_NULL          - Name of the attribute is not specified.
  \return DPRM_ERROR_ATTRIBUTE_VALUE_NULL         - Value of the attribute is not specified.
  \return DPRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN   - Attribute name is too lengthy. 
  \return DPRM_ERROR_INVALID_ATTRIBUTE_VALUE_LEN  - Attribute value  is too lengthy.
  \return DPRM_ERROR_DUPLICATE_RESOURCE           - Attribute already exists with the same name.
  */
int32_t dprm_add_attribute_to_forwarding_domain(uint64_t domain_handle,
                struct dprm_attribute_name_value_pair *attribute_info);
  
/** \ingroup Domain_Management_API
  \brief Deletes an attribute from a registered domain.\n
  <b>Description:</b>\n
  This API deletes an existing attribute name and value pair from the domain.

  \param[in] domain_handle     - Unique handle to the domain from which the attribute has to be deleted. 
  \param[in] attribute_name    - Name of the attribute to be deleted.
             
  \return DPRM_SUCCESS               - If attribute deletion is successful.
  \return DPRM_ERROR_INVALID_DOMAIN_HANDLE  - If the domain handle is invalid.
  */
int32_t dprm_delete_attribute_from_forwarding_domain(uint64_t domain_handle, struct dprm_attribute_name_value_pair *attribute_info_p);

/** \ingroup Domain_Management_API
  \brief Get the handle for a forwarding domain using its "name".\n
    
  \param[in]  name - Name of the forwarding domain.   
  \param[out] domain_handle -  Handle to the forwarding domain.
 
  \return DPRM_ERROR_INVALID_NAME - If no domain entry is found with the given name.
  \return DPRM_SUCCESS            - If domain entry is found with the given name.
  */
int32_t dprm_get_forwarding_domain_handle(char *name,  uint64_t *domain_handle);

/** \ingroup Domain_Management_API
  \brief Get the forwarding domain in the database with its handle provided as an input.\n
  <b>Description:</b>\n
  This API is used to get the exact domain in the database whose handle is provided.

  This API returns the following domain information as output arguments.
  - Domain information that was configured during registration API.
  - Runtime domain information like number of attached datapaths and domain attributes.
  It takes the following arguments as input.
  - Handle to the domain.\n

  \param[in]  domain_handle  - Handle to the domain.
  \param[out] config_info_p  - Configured domain information.
  \param[out] runtime_info_p - Contains the number of datapaths and the number of domain attributes.
                                
  \return DPRM_SUCCESS              - If a domain is found in the database with the given handle.
  \return DPRM_ERROR_INVALID_DOMAIN_HANDLE  - If no domain is found in the database with the given handle.
  */
int32_t dprm_get_exact_forwarding_domain(uint64_t domain_handle,
                                         struct dprm_distributed_forwarding_domain_info* config_info_p,
                                         struct dprm_distributed_forwarding_domain_runtime_info* runtime_info_p);

/** \ingroup Domain_Management_API
  \brief Get the first domain in the database.\n
  <b>Description:</b>\n
  This API is used to get the first domain in the database.
  This function in conjunction with dprm_get_next_forwarding_domain() can be 
  used to get all the registered domains one by one.

  This API returns the following domain information as output arguments.
   - Domain information that was configured during registration API.
   - Runtime domain information like number of attached datapaths and domain attributes.
   - Handle to the domain.\n

  \param[out] config_info    - Configured domain information.
  \param[out] runtime_info   - Contains the number of datapaths and the number of domain attributes.
  \param[out] domain_handle  - Handle to the domain.
  
  \return DPRM_SUCCESS                - If domain is found in the database with the given handle.
  \return DPRM_ERROR_NO_MORE_ENTRIES  - If no domain is found in the database with the given handle.
 */
int32_t dprm_get_first_forwarding_domain(
     struct dprm_distributed_forwarding_domain_info *config_info,
     struct dprm_distributed_forwarding_domain_runtime_info *runtime_info,
     uint64_t *domain_handle);

/** \ingroup Domain_Management_API
  \brief Get the next domain information given a domain handle.\n
  <b>Description:</b>\n
  This function is used to get the next domain in the database given the handle to the \n
  current domain.
  This function in conjunction with dprm_get_first_forwarding_domain() 
  can be used to get all the registered domains one by one.

  Note that the list order is not same as the way the domains are registered. List order
  is based on the way the ON Director layer arranges the domain information nodes.\n

  This API returns the following domain information as output arguments.
   - Domain information that was configured during registration API.
   - Runtime domain information like number of attached datapaths and domain attributes.
   - Handle to the domain.\n

  \param[out] config_info       - Configured domain information.
  \param[out] runtime_info      - Contains the number of datapaths and the number of domain attributes.
  \param[in,out] domain_handle  - Handle to the domain.It is used as input/output parameter.
                       
  \return DPRM_SUCCESS                - If a domain is found in the database.
  \return DPRM_ERROR_NO_MORE_ENTRIES  - If no more entries are found in the database.
  \return DPRM_ERROR_INVALID_DOMAIN_HANDLE  - If no domain entry is found with the given handle.
  */
int32_t dprm_get_next_forwarding_domain(
    struct   dprm_distributed_forwarding_domain_info          *config_info,
    struct   dprm_distributed_forwarding_domain_runtime_info  *runtime_info,
    uint64_t *domain_handle);

/** \ingroup Domain_Management_API
  \brief Get the first attribute information of a domain using its handle.\n
  <b>Description:</b>\n
  This API returns the first attribute name and value pair to the caller.
  Caller is expected to allocate memory for name_string and value_string. Lengths
  of those buffers are expected to be passed in 'name_length' and 'value_length'.
  If this function does not find name and value buffers are insufficient, then 
  it returns error. Also, it updates the name_length and value_length fields 
  with the lengths required.
                     
  \param[in]   domain_handle                 -Handle of the domain whose first attribute is required.
  \param[out]  attribute_output              -name and value pair of the first attribute.
 
  \return DPRM_SUCCESS                       - First attribute of the domain is found.
  \return DPRM_ERROR_INVALID_DOMAIN_HANDLE         - Invalid domain handle. 
  \return DPRM_ERROR_NO_MORE_ENTRIES               - No Attributes are configured for the domain.
  
  \return DPRM_ERROR_ATTRIBUTE_NAME_NULL           - No memory space provided for copying the attribute name.
  \return DPRM_ERROR_ATTRIBUTE_VALUE_NULL          - No memory space provided for copying the attribute value.
  \return DPRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN    - Memory space provided for copying next attribute name is not sufficient. 
  \return DPRM_ERROR_INVALID_ATTRIBUTE_VALUE_LEN   - Memory space provided for copying next attribute value is not sufficient. 
  */
int32_t dprm_get_first_forwarding_domain_attribute(uint64_t domain_handle,
          struct dprm_attribute_name_value_output_info *attribute_output);

/** \ingroup Domain_Management_API
  \brief Get the next attribute of a domain given an attribute and the domain handle.\n
  <b>Description:</b>\n
  This API gets the next attribute name and value.This function first finds the
  attribute based on 'current_attribute_name' and returns the attribute information next to it.\n
  
  \param[in]   domain_handle          - Handle of the domain whose next attribute is required.
  \param[in]   current_attribute_name - Name of the current attribute. 
  \param[out]  attribute_output       - name and value pair of the next attribute.
  
  \return DPRM_SUCCESS                       - Next attribute of the domain is found.
  \return DPRM_INVALID_DOMAIN_HANDLE               - Invalid domain handle. 
  \return DPRM_ERROR_NO_MORE_ENTRIES               - No more attributes are configured for the domain.
  \return DPRM_ERROR_INVALID_NAME                  - Name of the input Attribute is not valid.

  \return DPRM_ERROR_ATTRIBUTE_NAME_NULL           - No memory space provided for copying the attribute name.
  \return DPRM_ERROR_ATTRIBUTE_VALUE_NULL          - No memory space provided for copying the attribute value.
  \return DPRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN    - Memory space provided for copying next attribute name is not sufficient. 
  \return DPRM_ERROR_INVALID_ATTRIBUTE_VALUE_LEN   - Memory space provided for copying next attribute value is not sufficient. 

  */
int32_t dprm_get_next_forwarding_domain_attribute(uint64_t domain_handle,
                                       char *current_attribute_name,
                                       struct dprm_attribute_name_value_output_info *attribute_output);

/** \ingroup Domain_Management_API
  \brief Registration API for Applications to receive notifications from domain database.\n
  <b>Description:</b>\n
  This API is used by applications to register notifier functions to receive notifications regarding changes in the
  domain database.

  Applications may pass two callback arguments. These arguments are passed back when application 
  notifier function is called.

  Multiple applications may register their notifier functions with it. The registered callback functions 
  are called in the order in which they are registered. 
                       
  \param[in] notification_type  - Specifies the event for which the callback function shall be called.
  \param[in] domain_notifier_fn - Callback function pointer registered by the Application.
  \param[in] callback_arg1   - Argument 1 passed by Application.
  \param[in] callback_arg2   - Argument 2 passed by Application.
 
  \return DPRM_SUCCESS       - Upon successful registration  
  \return DPRM_ERROR_NO_MEM  - Error in allocating memory
  \return DPRM_ERROR_INVALID_DOMAIN_NOTIFICATION_TYPE       - Domain notification type is invalid.
  \return DPRM_ERROR_NULL_CALLBACK_DOMAIN_NOTIFICATION_HOOK - Callback function is NULL.
  */
int32_t dprm_register_forwarding_domain_notifications(uint32_t notification_type,
                                                      dprm_domain_notifier_fn domain_notifier_fn,
                                                      void *callback_arg1,
                                                      void *callback_arg2);

/** \ingroup Domain_Management_API
  \brief Deregisters notification function with domain database. \n
  <b>Description:</b>\n
  This API can be used to de-register the notification function pointer 
  with the domain database so that no further notifications are received.

  \param[in] notification_type   - For which the Application wishes not to receive any further notifications.  
  \param[in] domain_notifier_fn  - function pointer that was registered earlier.
  \return DPRM_SUCCESS - De-registration successful.
  \return DPRM_ERROR_NO_DOMAIN_NOTIFICATION - No such registration earlier with the database. 
  */
int32_t dprm_deregister_forwarding_domain_notifications(uint32_t notification_type,
                                                        dprm_domain_notifier_fn domain_notifier_fn);

#if 0

/** \ingroup Domain_Management_API
  \brief Add an oftable to a given domain.\n
  <b>Description</b>\n
  This API adds an oftable to a domain resource by using the name of the oftable.
  
  \param[in]  domain_handle     - Handle of the domain to which oftable is added.
  \param[in]  dprm_table_p      - oftable information including its unique name.
  \param[out] oftable_handle_p  - Handle to the oftable added. 
                                 
  \return DPRM_SUCCESS                     - Successfully added the oftable.  
  \return DPRM_FAILURE                     - Failed to add the oftable to the given domain.
  \return DPRM_ERROR_NO_MEM                - Unable to allocate memory for the resource.
  \return DPRM_ERROR_INVALID_DOMAIN_HANDLE - Invalid domain handle.
  \return DPRM_ERROR_DUPLICATE_RESOURCE    - Duplicate table name.
  */
int32_t dprm_add_oftable_to_domain(uint64_t domain_handle,
                                   struct   dprm_table_info *dprm_table_p,
                                   uint64_t *oftable_handle_p);

/** \ingroup Domain_Management_API
  \brief Delete an oftable from a given domain.\n
  <b>Description</b>\n
  This API deletes an oftable from a domain resource by using handle of the oftable.
           
  \param[in]  domain_handle   - Handle of the domain to which oftable belongs.
  \param[in]  oftable_handle  - Handle of the oftable to be deleted. 
                          
  \return DPRM_SUCCESS                      - Successfully deleted the oftable.  
  \return DPRM_FAILURE                      - Failed to delete the oftable from the given domain.
  \return DPRM_ERROR_INVALID_DOMAIN_HANDLE  - Invalid domain handle.
  \return DPRM_ERROR_INVALID_OFTABLE_HANDLE - Invalid oftable handle.
  */
int32_t dprm_delete_oftable_from_domain(uint64_t domain_handle,uint64_t oftable_handle);

/** \ingroup Domain_Management_API
  \brief Get the exact oftable information associated with a given domain.\n
  <b>Description:</b>\n
  This function is used to get the exact oftable information using the oftable handle and the domain handle.
  It returns oftable information.
             
  \param[in]   domain_handle     - Handle of the domain.
  \param[out]  config_info_p     - oftable information returned.
  \param[in]   oftable_handle_p  - Handle of the exact oftable.

  \return  DPRM_SUCCESS                      - Successfully returned the exact oftable information.
  \return  DPRM_INVALID_DOMAIN_HANDLE        - Domain handle is invalid.
  \return  DPRM_ERROR_INVALID_OFTABLE_HANDLE - oftable handle is invalid.
  \return  DPRM_ERROR_NO_OF_MATCH_FIELDS     - Not enough space to copy match fields.
  */
int32_t dprm_get_exact_domain_oftable(uint64_t domain_handle,
                                      struct   dprm_table_info *config_info_p,
                                      uint64_t oftable_handle_p);

/** \ingroup Domain_Management_API
  \brief Get the oftable handle given the oftable name.\n
  <b>Description:</b>\n
  This function is used to return the handle to a oftable using the name of the table and the domain handle.
  
  \param[in]   domain_handle     - Handle of the domain.
  \param[in]   oftable_name_p    - name of the oftable.
  \param[out]  oftable_handle_p  - Handle of the oftable returned.

  \return  DPRM_SUCCESS                      - Successfully returned the handle to the oftable.
  \return  DPRM_ERROR_INVALID_DOMAIN_HANDLE  - Domain handle is invalid.
  \return  DPRM_ERROR_INVALID_NAME           - oftable is not present with the given oftable name.
  */
int32_t dprm_get_oftable_handle(uint64_t domain_handle,char* oftable_name_p,uint64_t* oftable_handle_p);
  
/** \ingroup Domain_Management_API
  \brief Get the first oftable information associated with a given domain.\n
  <b>Description:</b>\n
  This function returns information of the first oftable associated with a domain.
 
  \param[in]   domain_handle     - Handle of the domain.
  \param[out]  config_info_p     - oftable information returned.
  \param[in]   oftable_handle_p  - Handle of the exact oftable.

  \return  DPRM_SUCCESS                      - Successfully returned the first oftable information.
  \return  DPEM_FAILURE                      - Failed to retrieve the first oftable.
  \return  DPRM_INVALID_DOMAIN_HANDLE        - Domain handle is invalid.
  \return  DPRM_ERROR_NO_MORE_ENTRIES        - oftable entries not found in the database.
  \return  DPRM_ERROR_NO_OF_MATCH_FIELDS     - Not enough space to copy match fields.
  */
int32_t dprm_get_first_domain_oftable(uint64_t domain_handle,
                                      struct   dprm_table_info *config_info_p,
                                      uint64_t *oftable_handle_p);

/** \ingroup Domain_Management_API
  \brief Get the next oftable information associated with a given domain.\n
  <b>Description:</b>\n
  This function returns information of the next oftable associated with a domain
  relative to a given oftable whose handle is given as an input.

  \param[in]      domain_handle     - Handle of the domain.
  \param[out]     config_info_p     - oftable information returned.
  \param[in,out]  oftable_handle_p  - Handle of the exact oftable.

  \return  DPRM_SUCCESS                      - Successfully returned the next oftable information.
  \return  DPEM_FAILURE                      - Failed to retrieve the next oftable.

  \return  DPRM_INVALID_DOMAIN_HANDLE        - Domain handle is invalid.
  \return  DPRM_ERROR_INVALID_OFTABLE_HANDLE - oftable handle for the current oftable is invalid.
  \return  DPRM_ERROR_NO_MORE_ENTRIES        - No oftable entries found in the database.
  \return  DPRM_ERROR_NO_OF_MATCH_FIELDS     - Not enough space to copy match fields.
  */
int32_t dprm_get_next_domain_oftable(uint64_t domain_handle,
                                     struct   dprm_table_info *config_info_p,
                                     uint64_t *oftable_handle_p);
#endif

/** \ingroup Datapath_Management_API 
  * DPRM_MAX_X509_SUBJECT_LEN 
  * - Maximum length of X509_subject name.
  */
#define DPRM_MAX_X509_SUBJECT_LEN 256
#define DPRM_MAX_DATAPATH_NAME_LEN 128

/** \ingroup Datapath_Management_API
  * DPRM_MAX_PORT_NAME_LEN 
  * - Maximum length of a port name.
  */
#define DPRM_MAX_PORT_NAME_LEN    16

/** notification_types that can be received from the datapath database */

/** \ingroup Datapath_Management_API
  * DPRM_DATAPATH_ALL_NOTIFICATIONS
  * - All notifications from the datapath database.
  */
#define DPRM_DATAPATH_ALL_NOTIFICATIONS 0

/** \ingroup Datapath_Management_API
  * DPRM_DATAPATH_ADDED 
  * - A datapath is added to the data base.
  */
#define DPRM_DATAPATH_ADDED  1

/** \ingroup Datapath_Management_API
  * DPRM_DATAPATH_DELETE 
  * - A datapath is deleted from the data base.
  */
#define DPRM_DATAPATH_DELETE  2

/** \ingroup Datapath_Management_API
  * DPRM_DATAPATH_ATTRIBUTE_ADDED 
  * - A new attribute is added to a datapath in the data base.
  */
#define DPRM_DATAPATH_ATTRIBUTE_ADDED  3

/** \ingroup Datapath_Management_API
  * DPRM_DATAPATH_ATTRIBUTE_DELETE 
  * - An attribute is deleted from a datapath in the data base.
  */
#define DPRM_DATAPATH_ATTRIBUTE_DELETE   4

/** \ingroup Datapath_Management_API
  * DPRM_DATAPATH_PORT_ADDED 
  * - A new port is attached to a datapath in the data base.
  */
#define DPRM_DATAPATH_PORT_ADDED     5

/** \ingroup Datapath_Management_API
  * DPRM_DATAPATH_PORT_DELETE 
  * - A port is deleted from a datapath in the data base.
  */
#define DPRM_DATAPATH_PORT_DELETE   6

/** \ingroup Datapath_Management_API
  * DPRM_DATAPATH_PORT_ATTRIBUTED_ADDED 
  * - An attribute is added to a port associated with a datapath.
  */
#define DPRM_DATAPATH_PORT_ATTRIBUTE_ADDED   7

/** \ingroup Datapath_Management_API
  * DPRM_DATAPATH_PORT_ATTRIBUTE_DELETE 
  * - An attribute is deleted from a port associated with a datapath.
  */
#define DPRM_DATAPATH_PORT_ATTRIBUTE_DELETE  8


 /** \ingroup Datapath_Management_API
  * DPRM_DATAPATH_PORT_DESC_UPDATE 
  * - port is updated from  port description message
  */
#define DPRM_DATAPATH_PORT_DESC_UPDATE  9



/** \ingroup Datapath_Management_API  
  \struct dprm_datapath_general_info 
  \brief Contains general information of a datapath.\n
  <b>Description</b>\n
  Datapath ID must be unique among datapaths.
  */
struct dprm_datapath_general_info{
   /** ID of the datapath */
   uint64_t dpid; 
   /** Subject name used to authenticate the remote datapath */
   char expected_subject_name_for_authentication[DPRM_MAX_X509_SUBJECT_LEN+1];

   /** datapath name **/
   char datapath_name[DPRM_MAX_DATAPATH_NAME_LEN+1];
  
   /** Name of the domain which must be unique among the domains */
   char domain_name[DPRM_MAX_FORWARDING_DOMAIN_LEN];
   /** Name of the physical switch in which this datapath exists*/ 
   char switch_name[DPRM_MAX_SWITCH_NAME_LEN];
   /** Flag to indicate whether the datapath supports multiple namespaces or not*/
   uint8_t  multi_namespaces;	
   uint8_t  is_multi_ns_config;
};

/** \ingroup Datapath_Management_API  
  \struct dprm_port_info 
  \brief Contains port information.\n
  <b>Description</b>\n
   Information about the port in a datapath \n
   port_id must be unique with in a datapath.\n
*/  
struct dprm_port_info{

 /** Id of the port */
  uint32_t port_id;


 /** Name of the port */
  char port_name[DPRM_MAX_PORT_NAME_LEN + 1];
 /** structure describing the port details*/
  struct ofl_port_desc_info   port_desc;

};

/** \ingroup Datapath_Management_API  
  \struct dprm_datapath_runtime_info 
  \brief Contains runtime information of a datapath.\n
  <b>Description</b>\n
  Runtime information of a datapath includes the number of ports and the attributes
  associated with the datapath.\n
 */
struct dprm_datapath_runtime_info{
   /** Number of ports associated with a datapath */ 
   uint8_t number_of_ports;
   /** Number of datapath attributes */
   uint8_t number_of_attributes;
};

/** \ingroup Datapath_Management_API
  \struct dprm_port_runtime_info
  \brief Contains runtime information of a port.\n
  <b>Description</b>\n
  Structure is added for future use.
 */

struct dprm_port_runtime_info{
   /** Future use */
   uint32_t dummy;
};

/** \ingroup Datapath_Management_API  
  \struct dprm_datapath_notification_data 
  \brief Contains data specific to the notification received from the datapath database.\n
  <b>Description</b>\n
  This structure is common for all the notifications types.
  Validity of each field is based on the notification_type.
 */
struct dprm_datapath_notification_data{
   /** ID of the datapath - valid for all the notification types */
   uint64_t dpid;
   /** Name of the port - valid for port add/delete and port attribute add/delete notification types */
   char port_name[DPRM_MAX_PORT_NAME_LEN + 1];
   /** ID of the por  t - valid for port add/delete and port attribute add/delete notification types */
   uint32_t port_id;
   /** port handle - valid for port add/delete and port attribute add/delete notification types */
   uint64_t port_handle;
   /** attribute name - valid for the attribute add delete notification types */
   char  attribute_name[DPRM_MAX_ATTRIBUTE_NAME_LEN + 1];
   /** attribute value - valid for the attribute add delete notification types */
   char  attribute_value[DPRM_MAX_ATTRIBUTE_NAME_LEN + 1];
};

/** \ingroup Datapath_Management_API
  \typedef dprm_datapath_notifier_fn
  \brief Prototype for the Application callback function to receive change notifications 
  from the datapath database.\n
  <b>Description</b>\n 
  Following are the notification types that can be received from datapath data base.
  - DPRM_DATAPATH_ALL_NOTIFICATIONS
  - DPRM_DATAPATH_ADDED
  - DPRM_DATAPATH_DELETE
  - DPRM_DATAPATH_ATTRIBUTE_ADDED
  - DPRM_DATAPATH_ATTRIBUTE_DELETE
  - DPRM_DATAPATH_PORT_ADDED
  - DPRM_DATAPATH_PORT_DELETE
  - DPRM_DATAPATH_PORT_ATTRIBUTED_ADDED
  - DPRM_DATAPATH_PORT_ATTRIBUTE_DELETE\n

  \param[in] notification_type - Type of the notification 
  \param[in] datapath_handle   - Handle of the datapath for which the change notification is received.
  \param[in] notification_data - Data related to the notification received from the datapath database.
  \param[in] callback_arg1     - Application's private information passed during registration.
  \param[in] callback_arg2     - Application's private information passed during registration. 
  */
typedef void (*dprm_datapath_notifier_fn)(uint32_t notification_type,
                                          uint64_t datapath_handle,
                                          struct   dprm_datapath_notification_data notification_data,
                                          void     *callback_arg1,
                                          void     *callback_arg2);

/** DATAPATH REGISTRATION API functions */

/** \ingroup Datapath_Management_API
  \brief Registers datapath with the database.\n
  <b>Description</b>\n
  This function adds the datapath to the database in the on_director.

  Datapath instance might have its own subject name. In which case, datapath subject name
  is used to verify the remote datapath. If datapath instance does not have subject name
  configured, then domain subject name is used to verify the SSL authentication.

  A datapath can be attached to a domain, a ON Director and a ON Switch using the respective handles.

  A  Handle to the create datapath entry returned. Its value is not supposed to be
  interpreted by the caller. This handle is expected to be passed in all other datapath specific API.\n

  \param[in]  datapath_info_p - General information of the datapath.
  \param[in]  switch_handle - Handle of the ON Switch to which the datapath is added.
  \param[in]  domain_handle - Handle of the domain to which the datapath is added.
  \param[out] output_handle_p - Handle to the datapath registered.
  
  \return DPRM_ERROR_NO_MEM             - If there is no memory to create this record.
  \return DPRM_ERROR_DUPLICATE_RESOURCE - if datapath id i.e. 'dpid' passed is not unique
 */
int32_t dprm_register_datapath(
   struct dprm_datapath_general_info* datapath_info_p,
   uint64_t switch_handle,uint64_t domain_handle,
   uint64_t* output_handle_p);

/** \ingroup Datapath_Management_API
  \brief Updates or modifies the datapath information.\n
  <b>Description</b>\n
  This function updates the datapath information in the database in on_director.

  \param[in]  datapath_info_p - General information of the datapath.
  \param[in]  switch_handle - Handle of the ON Switch to which the datapath is added.
  \param[in]  domain_handle - Handle of the domain to which the datapath is added.
  
  \return DPRM_SUCCESS - If update is successful.
  \return DPRM_FAILURE - On failure to update.
  \return DPRM_ERROR_INVALID_DATAPATH_HANDLE - If datapath not found
 */
int32_t dprm_update_datapath(
   struct dprm_datapath_general_info* datapath_info_p,
   uint64_t switch_handle,uint64_t domain_handle);

/** \ingroup Datapath_Management_API
  \brief Unregister a datapath instance.\n
  <b>Description</b>\n
  This API function deletes the datapath record from the database in the on_director.\n

  \param[in] dp_handle         - Handle of the datapath.

  \return DPRM_SUCCESS                  - If the datapath is successfully un-registered.
  \return DPRM_INVALID_DATAPATH_HANDLE  - If the handle passed is invalid.
  \return DPRM_ERROR_RESOURCE_NOTEMPTY  - Need to delete associated resources before un-registering.   
  */
int32_t dprm_unregister_datapath(uint64_t dp_handle);

/** \ingroup Datapath_Management_API
  \brief Adds a new attribute to a registered datapath.\n
  <b>Description:</b>\n
  This facility is provided for all DPRM resources to add new information to the datapath in future.\n
  It is good way to extend the functionality without changing the function prototype or information structures.
           
  \param[in] datapath_handle  - Unique handle to the datapath to which a new attribute is to be added.
                           
  \param[in] attribute_info    - It contains the attribute information to be added to the datapath. 

  \return DPRM_SUCCESS         - If attribute addition is successful.
  \return DPRM_ERROR_NO_MEM    - If there is an error in allocating memory.
  \return DPRM_ERROR_INVALID_DATAPATH_HANDLE       - If the datapath handle is invalid.
  \return DPRM_ERROR_ATTRIBUTE_NAME_NULL           - Name of the attribute is not specified.
  \return DPRM_ERROR_ATTRIBUTE_VALUE_NULL          - Value of the attribute is not specified.
  \return DPRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN    - Attribute name is too lengthy. 
  \return DPRM_ERROR_INVALID_ATTRIBUTE_VALUE_LEN   - Attribute value  is too lengthy.
  \return DPRM_ERROR_DUPLICATE_RESOURCE            - Attribute already exists with the same name.
  */
int32_t dprm_add_attribute_to_datapath(
        uint64_t datapath_handle,
        struct dprm_attribute_name_value_pair *attribute_info);

/** \ingroup Datapath_Management_API
  \brief Deletes an  attribute from a registered datapath.\n
  <b>Description:</b>\n
  This function deletes an existing attribute name and value pair from a given datapath.

  \param[in] datapath_handle   - Unique handle to the datapath from which an attribute is to be deleted. 
  \param[in] attribute_name    - Name of the attribute to be deleted.
              
  \return DPRM_SUCCESS             - If attribute deletion is successful.
  \return DPRM_INVALID_DATAPATH_HANDLE  - If the datapath handle is invalid.
  */
int32_t dprm_delete_attribute_from_datapath(uint64_t datapath_handle, struct dprm_attribute_name_value_pair *attribute_info_p);
/** \ingroup Datapath_Management_API
  \brief Deletes an all  attribute from a registered datapath.\n
  <b>Description:</b>\n
  This function deletes an existing attribute name from a given datapath.

  \param[in] datapath_handle   - Unique handle to the datapath from which an attribute is to be deleted. 
  \param[in] attribute_name    - Name of the attribute to be deleted.
              
  \return DPRM_SUCCESS             - If attribute deletion is successful.
  \return DPRM_INVALID_DATAPATH_HANDLE  - If the datapath handle is invalid.
  */
int32_t dprm_delete_all_attribute_values_from_datapath(uint64_t datapath_handle, struct dprm_attribute_name_value_pair *attribute_info_p);

/** \ingroup Datapath_Management_API
  \brief Get the datapath handle given the ID of the datapath.\n
    
  \param[in]  dpid             - ID of the datapath.   
  \param[out] datapath_handle  - Handle to the datapath.

  \return DPRM_ERROR_INVALID_DATAPATH_HANDLE - If no datapath entry is found with the given datapath id.
  \return DPRM_SUCCESS              - If datapath entry is found with the given datapath id.
  */
int32_t dprm_get_datapath_handle(uint64_t dpid, uint64_t *datapath_handle);

/** \ingroup Datapath_Management_API
  \brief Add a port to a given datapath.\n
  <b>Description</b>\n
   This API adds a port to a datapath. port_id is expected to be same as the
   id of the port in the remote datapath.

   if port_id is not given, ofproto fills this up as part of PORT_DESCRIPTION
   message reply.  If it is given, compliancy verification happens during
   port description reply message processing.

   \param[in]  datapath_handle - Handle of the datapath to which port is added.
   \param[in]  dprm_port       - Port information.
   \param[out] port_handle     - Handle to the port added. 
  
   \return DPRM_SUCCESS                        - Successfully added the port.  
   \return DPRM_ERROR_INVALID_DATAPATH_HANDLE  - Invalid datapath handle.
   \return DPRM_ERROR_DUPLICATE_RESOURCE       - Duplicate port id.
 */
int32_t dprm_add_port_to_datapath(
        uint64_t datapath_handle,
        struct   dprm_port_info *dprm_port,
        uint64_t *port_handle);

/** \ingroup Datapath_Management_API
  \brief Delete a port from a given datapath.\n
  <b>Description</b>\n
  This API Deletes the port identified by port_handle from datapath identified 
  by the datapath_handle.
 
  \param[in]  datapath_handle - Handle of the datapath from which the port has to be deleted.
  \param[out] port_handle     - Handle of the port to be deleted. 

  \return DPRM_SUCCESS - Successfully deleted the port from the datapath.
  \return DPRM_ERROR_INVALID_PORT_HANDLE      - Invalid port Handle.
  \return DPRM_ERROR_INVALID_DATAPATH_HANDLE  - Invalid datapath handle.
 */
int32_t dprm_delete_port_from_datapath(
        uint64_t datapath_handle,
        uint64_t port_handle);

/** \ingroup Datapath_Management_API
  \brief Add attribute to a port associated with a datapath.\n
  <b>Description</b>\n
  This facility is provided for all DPRM resources to add new information to 
  the  datapath in future. It is good way to extend the functionality without 
  chaining the function prototype or information structures.
 
  \param[in]  datapath_handle - Handle of the datapath to which port is associated.
  \param[in]  port_handle     - Handle of the port to which attribute is to be added. 
  \param[in]  attribute_info  - Name and value pair of the attribute. 
 
  \return DPRM_SUCCESS                             - If attribute addition is successful.
  \return DPRM_ERROR_NO_MEM                  - If there is an error in allocating memory.
  \return DPRM_ERROR_INVALID_PORT_HANDLE           - If the port handle is invalid.
  \return DPRM_ERROR_ATTRIBUTE_NAME_NULL           - Name of the attribute is not specified.
  \return DPRM_ERROR_ATTRIBUTE_VALUE_NULL          - Value of the attribute is not specified.
  \return DPRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN    - Attribute name is too lengthy. 
  \return DPRM_ERROR_INVALID_ATTRIBUTE_VALUE_LEN   - Attribute value  is too lengthy.
  \return DPRM_ERROR_DUPLICATE_RESOURCE            - A port attribute already exists with the same name.
 */
int32_t dprm_add_attribute_to_datapath_port(
        uint64_t datapath_handle,
        uint64_t port_handle,
        struct   dprm_attribute_name_value_pair *attribute_info);

/** \ingroup Datapath_Management_API
  \brief Delete attribute from a port associated with a datapath.\n
  <b>Description</b>\n
  This API deletes an existing attribute form a port which is associated
  with a given datapath.
  
  \param[in]  datapath_handle - Handle of the datapath to which port is associated.
  \param[in]  port_handle     - Handle of the port from which attribute is to be deleted. 
  \param[in]  attribute_name  - Name of the attribute.

  \return DPRM_SUCCESS                        - Successfully deleted the attribute from port.
  \return DPRM_ERROR_INVALID_DATAPATH_HANDLE  - If datapath handle is invalid. 
  \return DPRM_ERROR_INVALID_PORT_HANDLE      - If port handle is invalid.
 */ 
int32_t dprm_delete_attribute_from_datapath_port(uint64_t datapath_handle,
                                                 uint64_t port_handle,
                                                 struct dprm_attribute_name_value_pair *attribute_info_p);

/** \ingroup Datapath_Management_API
  \brief Get the datapath information given the datapath handle.\n
  <b>Description:</b>\n

  This API is used to get the exact datapath in the database whose handle is
  provided as an input.
            
  This API returns the following datapath information as output arguments.
  - Datapath information that was configured during registration API.
  - Runtime Datapath information like number of attached ports and datapath attributes.
  It takes the following as input arguments.
  - Handle to the datapath.\n

  \param[in]  handle            - Handle to the exact datapath in the database.
  \param[in]  sw_handle_p       - Handle to the ON Switch to which datapath belongs. 
  \param[in]  dm_handle_p       - Handle to the domain to which datapath belongs.
  \param[out] config_info_p     - Configured datapath general information.
  \param[out] runtime_info_p    - Contains the number of ports and the number of datapath attributes.
                     
  \return DPRM_SUCCESS               - If a datapath is found in the database with the given handle.
  \return DPRM_ERROR_INVALID_DATAPATH_HANDLE  - If datapath is not found in the database with the given handle.
  */
int32_t dprm_get_exact_datapath(uint64_t  handle,
                                uint64_t* sw_handle_p,
                                uint64_t* dm_handle_p,
                                struct dprm_datapath_general_info* config_info_p,
                                struct dprm_datapath_runtime_info* runtime_info_p);

/** \ingroup Datapath_Management_API
  \brief Get the first datapath information.\n
  <b>Description:</b>\n

  This function is used to get the first datapath in the database.
  This function in conjunction with dprm_get_next_datapath() can be 
  used to get all the registered datapaths one by one.
 
  This API returns the following datapath information as output arguments.
   - Datapath information that was configured during registration API.
   - Runtime Datapath information like number of attached ports and datapath attributes.
   - Handle to the datapath.\n

  \param[out] config_info       - Configured datapath general information.
  \param[out] runtime_info      - Contains the number of ports and the number of datapath attributes.
  \param[out] datapath_handle   - Handle to the first datapath in the database.
 
  \return DPRM_SUCCESS          - If a datapath is found in the database.
  \return DPRM_ERROR_NO_MORE_ENTRIES  - If no datapath is found in the database.
  */
int32_t dprm_get_first_datapath(
        struct dprm_datapath_general_info *config_info,
        struct dprm_datapath_runtime_info *runtime_info,
        uint64_t *datapath_handle);

/** \ingroup Datapath_Management_API
  \brief Get the next datapath information.\n
  <b>Description:</b>\n
  This function is used to get the next datapath in the database given a datapath.
  This function in conjunction with dprm_get_next_datapath() can be used to get all the
  registered datapaths one by one.
             
  This API returns the following datapath information as output arguments.
   - Datapath information that was configured during registration API.
   - Runtime Datapath information like number of attached ports and datapath attributes.
   - Handle to the datapath.\n

  Note that the list order is not same as the way the datapaths are registered.
  List order is based on the way the ON Director layer arranges the datapath
  information nodes.\n

  \param[out] config_info          - Configured datapath general information of the next datapath.
  \param[out] runtime_info         - Contains the number of ports and the number of datapath attributes.
  \param[in,out] datapath_handle   - Handle to the datapath. Used both as an input and output.
                                
  \return DPRM_SUCCESS                - If next datapath is found in the database.
  \return DPRM_ERROR_NO_MORE_ENTRIES  - If next datapath is not found in the database.
  */
int32_t dprm_get_next_datapath(
        struct   dprm_datapath_general_info *config_info,
        struct   dprm_datapath_runtime_info *runtime_info,
        uint64_t *datapath_handle);

 /** \ingroup Datapath_Management_API
   \brief Get the first attribute information of a datapath given its handle.\n
   <b>Description:</b>\n
   This function returns the first attribute name and value pair to the caller.
   Caller is expected to allocate memory for name_string and value_string. Lengths
   of those buffers are expected to be passed in 'name_length' and 'value_length'.
   If this function does not find name and value buffers are insufficient, then 
   it returns error. Also, it updates the name_length and value_length fields 
   with the lengths required.
                     
   \param[in]   datapath_handle      -Handle of the datapath whose first attribute is required.
   \param[out]  attribute_output     -name and value pair of the first attribute.
  
   \return DPRM_SUCCESS                       - First attribute of the datapath is found.
   \return DPRM_ERROR_INVALID_DATAPATH_HANDLE       - Invalid datapath handle. 
   \return DPRM_ERROR_NO_MORE_ENTRIES               - No Attributes are configured for the datapath.
   
   \return DPRM_ERROR_ATTRIBUTE_NAME_NULL           - No memory space provided for copying the attribute name.
   \return DPRM_ERROR_ATTRIBUTE_VALUE_NULL          - No memory space provided for copying the attribute value.
   \return DPRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN    - Memory space provided for copying next attribute name is not sufficient. 
   \return DPRM_ERROR_INVALID_ATTRIBUTE_VALUE_LEN   - Memory space provided for copying next attribute value is not sufficient. 
   */
int32_t dprm_get_first_datapath_attribute(uint64_t datapath_handle,
                     struct dprm_attribute_name_value_output_info *attribute_output);

/** \ingroup Datapath_Management_API
  \brief Get the next attribute of a datapath given an attribute and the datapath handle.\n
  <b>Description:</b>\n
  This API gets the next attribute name and value.This function first finds the attribute
  based on 'current_attribute_name' and returns the attribute information next to it.\n
 
  \param[in]   datapath_handle          - Handle of the datapath whose next attribute is required.
  \param[in]   current_attribute_name   - Name of the current attribute. 
  \param[out]  attribute_output         - name and value pair of the next attribute.
 
  \return DPRM_SUCCESS                       - Next attribute of the datapath is found.
  \return DPRM_INVALID_DATAPATH_HANDLE       - Invalid datapath handle. 
  \return DPRM_ERROR_NO_MORE_ENTRIES         - No more attributes are configured for the domain.
  \return DPRM_ERROR_INVALID_NAME            - Name of the input Attribute is not valid.

  \return DPRM_ERROR_ATTRIBUTE_NAME_NULL           - No memory space provided for copying the attribute name.
  \return DPRM_ERROR_ATTRIBUTE_VALUE_NULL          - No memory space provided for copying the attribute value.
  \return DPRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN    - Memory space provided for copying next attribute name is not sufficient. 
  \return DPRM_ERROR_INVALID_ATTRIBUTE_VALUE_LEN   - Memory space provided for copying next attribute value is not sufficient. 
  */
int32_t dprm_get_next_datapath_attribute(uint64_t datapath_handle,
                                         char *current_attribute_name,
                                         struct dprm_attribute_name_value_output_info *attribute_output);

/** \ingroup Datapath_Management_API
   \brief Get the exact port information associated with a given datapath.\n
   <b>Description:</b>\n
   This function is used to get the exact port information using the port handle and the datapath handle.
   It returns port information.
  
   \param[in]   datapath_handle      - Handle of the datapath.
   \paraom[out] port_info_p          - port information returned.
   \param[out]  runtime_info_p       - port run time information.
   \param[out]  port_handle_p        - Handle of the exact port.
  
   \return  DPRM_SUCCESS                  - Successfully returned the port information.
   \return  DPRM_INVALID_DATAPATH_HANDLE  - Datapath handle is invalid.
   \return  DPRM_ERROR_NO_MORE_ENTRIES    - No port entries in the datapath.
   */
int32_t dprm_get_exact_datapath_port(uint64_t datapath_handle,
                                     struct   dprm_port_info *port_info_p,
                                     struct   dprm_port_runtime_info *runtime_info_p,
                                     uint64_t port_handle_p);

/** \ingroup Datapath_Management_API
  \brief Get the first port information associated with a given datapath.\n
  <b>Description:</b>\n
  This function is used to get the port list in conjunction with the API dprm_get_next_datapath_port
  for a given datapath identified by datapath_handle. It returns port information and port handle.

  \param[in]   datapath_handle      - Handle of the datapath.
  \param[out]  port_info            - port information returned.
  \param[out]  runtime_info         - port run time information.
  \param[out]  output_port_handle   - Handle of the first port.

  \return  DPRM_SUCCESS                  - Successfully returned the port information.
  \return  DPRM_INVALID_DATAPATH_HANDLE  - Datapath handle is invalid.
  \return  DPRM_ERROR_NO_MORE_ENTRIES    - No port entries in the datapath.
*/ 
int32_t dprm_get_first_datapath_port(uint64_t datapath_handle,
                                     struct   dprm_port_info *port_info,
                                     struct   dprm_port_runtime_info *runtime_info,
                                     uint64_t *output_port_handle);

/** \ingroup Datapath_Management_API
  \brief Get the next port information associated with a given datapath.\n
  <b>Description:</b>\n
  This function is used to get the port list in conjunction with the API dprm_get_first_datapath_port
  for a given datapath identified by datapath_handle. It returns port information and port handle.\n

  \param[in]      datapath_handle       - Handle of the datapath.
  \param[out]     port_info             - port information returned.
  \param[out]     runtime_info          - port run time information.
  \param[in,out]  port_handle           - Handle of the port. Used both as an input and output.
  
  \return DPRM_SUCCESS                  - Successfully returned the port information.
  \return DPRM_INVALID_DATAPATH_HANDLE  - Datapath handle is invalid.
  \return DPRM_ERROR_NO_MORE_ENTRIES    - No more port entries in the datapath.
  */
int32_t dprm_get_next_datapath_port(uint64_t  datapath_handle,
                                    struct    dprm_port_info *port_info,
                                    struct    dprm_port_runtime_info *runtime_info,
                                    uint64_t  *port_handle);

/** \ingroup Datapath_Management_API
  \brief Get the first attribute of a port given the handles to the port and the datapath.\n
  <b>Description:</b>\n
  This function returns the first attribute name and value pair to the caller.
  Caller is expected to allocate memory for name_string and value_string. Lengths
  of those buffers are expected to be passed in 'name_length' and 'value_length'.
  If this function does not find name and value buffers are insufficient, then 
  it returns error. Also, it updates the name_length and value_length fields 
  with the lengths required.\n

  \param[in]      datapath_handle    - Handle of the datapath.
  \param[in]      port_handle        - Handle of the port.
  \param[out]     attribute_output   - name and value pair of the first attribute returned.

  \return DPRM_SUCCESS                             - First attribute of the port is found.
  \return DPRM_ERROR_INVALID_DATAPATH_HANDLE       - Invalid datapath handle. 
  \return DPRM_INVALID_PORT_HANDLE                 - Invalid Port handle. 
  \return DPRM_ERROR_NO_MORE_ENTRIES               - No Attributes are configured for the port.

  \return DPRM_ERROR_ATTRIBUTE_NAME_NULL           - No memory space provided for copying the attribute name.
  \return DPRM_ERROR_ATTRIBUTE_VALUE_NULL          - No memory space provided for copying the attribute value.
  \return DPRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN    - Memory space provided for copying next attribute name is not sufficient. 
  \return DPRM_ERROR_INVALID_ATTRIBUTE_VALUE_LEN   - Memory space provided for copying next attribute value is not sufficient. 
 */
int32_t dprm_get_first_datapath_port_attribute(uint64_t datapath_handle,
                                               uint64_t port_handle,
           struct dprm_attribute_name_value_output_info *attribute_output);
   

/** \ingroup Datapath_Management_API
  \brief Get  next attribute of a port given an attribute and handles to the port and the datapath.\n
  <b>Description:</b>\n
  This function returns  the next attribute name and value. This function first finds the
  attribute based on 'current_attribute_name' and returns the attribute information  next to it.\n

  \param[in]      datapath_handle           - Handle of the datapath.
  \param[in]      port_handle               - Handle of the port.
  \param[in]      current_attribute_name    - "name" of the current attribute.
  \param[out]     attribute_output          - name and value pair of the next attribute returned.

  \return DPRM_SUCCESS                        - Successfully returned the first attribute.
  \return DPRM_INVALID_DATAPATH_HANDLE        - If datapath handle is invalid.
  \return DPRM_INVALID_PORT_HANDLE            - If port handle is invalid.
  \return DPRM_ERROR_NO_MORE_ENTRIES          - No more attributes are configured for the domain.
  \return DPRM_ERROR_INVALID_NAME             - Name of the input Attribute is not valid.

  \return DPRM_ERROR_ATTRIBUTE_NAME_NULL           - No memory space provided for copying the attribute name.
  \return DPRM_ERROR_ATTRIBUTE_VALUE_NULL          - No memory space provided for copying the attribute value.
  \return DPRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN    - Memory space provided for copying next attribute name is not sufficient. 
  \return DPRM_ERROR_INVALID_ATTRIBUTE_VALUE_LEN   - Memory space provided for copying next attribute value is not sufficient. 
 */
int32_t dprm_get_next_datapath_port_attribute(uint64_t datapath_handle,
                                              uint64_t port_handle,
                                              char     *current_attribute_name,
          struct dprm_attribute_name_value_output_info *attribute_output);


/** datapath Notification registrations **/

/** \ingroup Datapath_Management_API
  \brief Registration API to receive notifications from datapath database.\n
  <b>Description:</b>\n
  This API can be used by applications to receive notifications regarding changes in the datapath database.

  This API takes two callback arguments from Applications which will be passed back as arguments
  to the notifier function.
  
  Multiple applications may register their notifier functions using this API.
  The registered callback functions are called in the order in which they are registered.\n

  \param[in] notification_type    - Specifies the event for which the notification is required. 
  \param[in] datapath_notifier_fn - Callback function pointer registered by the Application.
  \param[in] callback_arg1   - Argument 1 passed by Application.
  \param[in] callback_arg2   - Argument 2 passed by Application.
 
 \return DPRM_SUCCESS       - upon successful registration  
 \return DPRM_ERROR_NO_MEM  - Error in allocating memory
 \return DPRM_ERROR_INVALID_DATAPATH_NOTIFICATION_TYPE       - Datapath notification type is invalid.
 \return DPRM_ERROR_NULL_CALLBACK_DATAPATH_NOTIFICATION_HOOK - callback function is NULL.
 */
int32_t dprm_register_datapath_notifications(uint32_t notification_type,
                                             dprm_datapath_notifier_fn datapath_notifier_fn,
                                             void     *callback_arg1,
                                             void     *callback_arg2);

/** \ingroup Datapath_Management_API
  \brief De-registers notification function with datapath database. \n
  <b>Description:</b>\n
  This API can be used by applications to de-register the notification callback function with the 
  datapath database so that no further notifications are received.

  \param[in] notification_type     - For which the Application wishes not to receive notifications.
  \param[in] datapath_notifier_fn  - The function pointer that was registered earlier.

  \return DPRM_SUCCESS                        - De-registration successful.
  \return DPRM_ERROR_NO_DATAPATH_NOTIFICATION - No such registration earlier with the database. 
  */
int32_t dprm_deregister_datapath_notifications(uint32_t notification_type,
                                               dprm_datapath_notifier_fn datapath_notifier_fn);

/** Controller API */

/** connection status of a Transport end point.*/

/** \ingroup Controller_Management_API
  * DPRM_TRANSPORT_ENDPOINT_CONNECTION_ACTIVE 
  * - Connction with the Transport endpoint is Active.
  */
#define DPRM_TRANSPORT_ENDPOINT_CONNECTION_ACTIVE   1

/** \ingroup Controller_Management_API
  * DPRM_TRANSPORT_ENDPOINT_CONNECTION_CLOSED 
  * - Connction with the Transport endpoint is Closed.
  */
#define DPRM_TRANSPORT_ENDPOINT_CONNECTION_CLOSED   2


/** \ingroup Controller_Management_API
  * DPRM_TRANSPORT_ENDPOINT_CONNECTION_TIMEDOUT 
  * - Connction with the Transport endpoint is timed out.
  */
#define DPRM_TRANSPORT_ENDPOINT_CONNECTION_TIMEDOUT 3

/** \ingroup Controller_Management_API
  * DPRM_TRANSPORT_TYPE_TLS 
  * - Macro used to indicate transport mode as TLS. This macro can be used as 
  *   a transport mode parameter during creation of transport endpoint, mode type as TLS
 */
#define DPRM_TRANSPORT_TYPE_TLS  1

/** \ingroup Controller_Management_API
  * DPRM_TRANSPORT_TYPE_TCP 
  * - Macro used to indicate transport mode as TCP. This macro can be used as 
  *   a transport mode parameter during creation of transport endpoint, mode type as TCP.
 */
#define DPRM_TRANSPORT_TYPE_TCP  2

/** \ingroup Controller_Management_API
  * DPRM_TRANSPORT_TYPE_DTLS 
  * - Macro used to indicate transport mode as DTLS. This macro can be used as 
  *   a transport mode parameter during creation of transport endpoint, mode type as DTLS
*/
#define DPRM_TRANSPORT_TYPE_DTLS 3

/** \ingroup Controller_Management_API
  * DPRM_TRANSPORT_TYPE_UDP 
  * - Macro used to indicate transport mode as UDP. This macro can be used as 
  *   a transport mode parameter during creation of transport endpoint, mode type as UDP
*/
#define DPRM_TRANSPORT_TYPE_UDP  4
/** \ingroup Controller_Management_API
  * DPRM_TRANSPORT_TYPE_VIRTIO 
  * - Macro used to indicate transport mode as VIRTIO. This macro can be used as 
  *   a transport mode parameter during creation of transport endpoint, mode type as UDP
*/
#define DPRM_TRANSPORT_TYPE_VIRTIO  5

/** \ingroup Controller_Management_API
  * DPRM_TRANSPORT_TYPE_UNIX_DOMAIN_TCP
  * - Macro used to indicate transport mode as TCP for Unix domain sockets. 
  *   This macro can be used as a transport mode parameter during creation of 
  *   transport endpoint, mode type as UDP
*/
#define DPRM_TRANSPORT_TYPE_UNIX_DM_TCP  6

/** \ingroup Controller_Management_API
 * DPRM_TRANSPORT_TYPE_UNIX_DOMAIN_UDP
 * - Macro used to indicate transport mode as UDPi for Unix domain sockets. 
 *   This macro can be used as a transport mode parameter during creation of 
 *   transport endpoint, mode type as UDP
*/
#define DPRM_TRANSPORT_TYPE_UNIX_DM_UDP  7


/** \ingroup Controller_Management_API  
  \struct controller_transport_endpoint_info 
  \brief Details of a Transport endpoint.\n
  <b>Description</b>\n
  It is used as input parameter to add transport end point the system.
  It contains connection type,port and the subject name to be used in certificates.
  Application need to fill these parameters and call dprm_add_transport_endpoint() 
  API to create transport end point.
  */
struct controller_transport_endpoint_info{
   /** Connection tranport mode as TCP or TLS or UDP or DTLS */       
   uint8_t  connection_type;
   /** Port number(TCP or UDP) to be used by the connection */
   uint16_t port;
   /** Subject name to be used by the peer in its certificate */
   char subjectName_of_certificate_to_use[DPRM_MAX_X509_SUBJECT_LEN + 1];
};


/** \ingroup Controller_Management_API  
  \struct controller_transport_end_point_status 
  \brief Status of the Transport end point. \n
  <b>Description</b>\n
  It includes the connection type,ip addresses remote port
  and connection status of the Transport end point.
 */

/** \ingroup Controller_Management_API
  \brief Adds a transport endpoint to a ON Director.\n
  <b>Description</b>\n
  This API adds a transport endpoint to the ON Director. 
  The transport end points are supported by datapaths in remote switches.
  The endpoint information contains the connection type, port number to be used
  and the subject name to be used by the endpoint in its certificate.
  Possible connection types are TCP, UDP, SSL, DTLS.\n
  
  \param[in] controller_handle - Handle to the ON Director to which endpoint is to be added.
  \param[in] endpoint_info     - Information of the Transport endpoint to be added.

  \return DPRM_SUCCESS - Successfully added the end point to the ON Director.
  \return DPRM_ERROR_INVALID_CONTROLLER_HANDLE - Invalid ON Director.
  \return DPRM_ERROR_INVALID_ENDPOINT_INFO     - Endpoint information is incorrect.
 */
int32_t dprm_add_transport_endpoint(uint64_t controller_handle,
     struct controller_transport_endpoint_info *endpoint_info);

/** \ingroup Controller_Management_API
  \brief Deletes a transport endpoint from a ON Director.\n
  <b>Description</b>\n
  The transport endpoint identified by the connection type and the port is deleted from
  the given ON Director identified by the ON Director handle. 
 
  \param[in] controller_handle - Handle to the ON Director to which endpoint is to be added.
  \param[in] connection_type   - Possible tranport connection modes are TCP,UDP,SSL,DTLS.
  \param[in] port              - Port used by the endpoint.
  
  \return DPRM_SUCCESS - Successfully deleted the end point from the ON Director.
  \return DPRM_ERROR_INVALID_CONTROLLER_HANDLE - Invalid ON Director.
  \return DPRM_FAILURE - Failed to delete the end point from the ON Director.
  */
int32_t dprm_delete_transport_endpoint(uint64_t controller_handle,
                                       uint8_t  connection_type,
                                       uint16_t port);

/** \ingroup Controller_Management_API 
  \brief Get the first transport endpoint from a given ON Director.\n
  <b>Description</b>\n
  Get the first transport endpoint associated with a given ON Director.
  \param[in]  controller_handle - Handle to the ON Director.

  \param[out] endpoint_info - Information of the first Transport endpoint of the given ON Director.
  
  \return DPRM_SUCCESS - Successfully found the first Transport end point .
  \return DPRM_ERROR_INVALID_CONTROLLER_HANDLE - Invalid ON Director.
  */
int32_t dprm_get_first_transport_endpoint(uint64_t controller_handle,
                                          struct controller_transport_endpoint_info *endpoint_info);

/** \ingroup Controller_Management_API
  \brief Get the next transport endpoint from a given ON Director.\n
  <b>Description</b>\n
  Get the next transport endpoint associated with a given ON Director.
  \param[in] controller_handle - Handle to the ON Director.
  \param[in] relative_record_connection_type - connection mode of the reference end point. 
  \param[in] relative_record_port - port of the reference end point.
  \param[out] endpoint_info - Information of the next Transport endpoint of the given ON Director.
  \return DPRM_SUCCESS - Successfully found the next Transport end point .
  \return DPRM_NO_NEXT_TRANSPORT_END_POINT - There is no next tranport endpoint exists.
  \return DPRM_ERROR_INVALILD_TRANSPORT_END_POINT - Invalid transport endpoint details passed.
  \return DPRM_ERROR_INVALID_CONTROLLER_HANDLE - Invalid ON Director.
  */
int32_t dprm_get_next_transport_endpoint(uint64_t controller_handle,
                           uint8_t  relative_record_connection_type,
                           uint16_t relative_record_port,
        struct controller_transport_endpoint_info *endpoint_info);


/** \ingroup Controller_Management_API
  \brief Get number of connections opened for the transport endpoint \n
  <b>Description</b>\n
  This API provides number of connections opened for the given transport endpoint. 
  Application can call dprm_get_first_runtime_info_of_transport_endpoint() and 
  dprm_get_next_runtime_info_of_transport_endpoint() to get runtime connection details.

  \param[in] controller_handle    - Handle to the ON Director.
  \param[in] connection_type      - Transport connection mode of the end point.
  \param[in] connection_port      - Port number of end point. 
  \param[out] number_of_conns     - Number of connections opened for the given transport end point. 
  \return DPRM_SUCCESS - Successfully returned number of connections 
  \return DPRM_ERROR_INVALID_CONTROLLER_HANDLE   - Invalid ON Director.
  \return DPRM_ERROR_INVALILD_TRANSPORT_END_POINT - Invalid transport endpoint details passed.
  */
int32_t dprm_get_no_of_connections_opened(uint64_t controller_handle,
                                          uint8_t  connection_type,
                                          uint16_t port,
                                          uint32_t number_of_conns);
/** \ingroup Controller_Management_API  
  \struct controller_transport_endpoint_runtime_info 
  \brief Runtime information of a Transport endpoint.\n
  <b>Description</b>\n
  It contains details of each connections opened for the given transport end point. 
  */
struct controller_transport_endpoint_runtime_info {
   /** IP Address details  of data_path connected to this endpoint.*/
   union {
     /** ipv4 address of the remote Transport endpoint if received */
     struct ipv4_addr remote_ipv4_addr; 

     /** ipv6 addr of the remote Transport endpoint if received */
     struct ipv6_addr remote_ipv6_addr;
   };

   /** Port number of data_path Transport endpoint. */
   uint16_t remote_port;

   /** status of the connection:ACTIVE,CLOSED,TIMED_OUT */ 
   uint32_t connection_status;  
 };

/** \ingroup Controller_Management_API
  \brief Get first runtime connection information of given transport endpoint associated to ON Director. \n
  <b>Description</b>\n

  \param[in] controller_handle    - Handle to the ON Director.
  \param[in] connection_type      - Transport connection mode of the end point.
  \param[in] connection_port      - Port number of end point. 
  \param[out] runtime_info        - First runtime connection details of the given end point. 
  \return DPRM_SUCCESS - Successfully returned number of connections 
  \return DPRM_ERROR_INVALID_CONTROLLER_HANDLE   - Invalid ON Director.
  \return DPRM_ERROR_INVALILD_TRANSPORT_END_POINT - Invalid transport endpoint details passed.
  */
int32_t dprm_get_first_runtime_info_of_transport_endpoint(uint64_t controller_handle,
                                                         uint8_t  connection_type,
                                                         uint16_t connection_port,
        struct controller_transport_endpoint_runtime_info *runtime_info);

/** \ingroup Controller_Management_API
  \brief Get next runtime connection information of given transport endpoint assoociated to ON Director. 

  \param[in]  controller_handle    - Handle to the ON Director.
  \param[in]  connection_type      - Transport connection mode of the end point.
  \param[in]  connection_port      - Port number of end point. 
  \param[in]  curr_runtime_info    - Current runtime connection details of the given end point. 
  \param[out] next_runtime_info    - Next runtime connection details of the given end point. 
  \return DPRM_SUCCESS - Successfully returned number of connections 
  \return DPRM_ERROR_INVALID_CONTROLLER_HANDLE   - Invalid ON Director.
  \return DPRM_ERROR_INVALILD_TRANSPORT_END_POINT - Invalid transport endpoint details passed.
  */
int32_t dprm_get_next_runtime_info_of_tansport_endpoint(uint64_t controller_handle,
                                                        uint8_t  connection_type,
                                                        uint16_t connection_port,
                                                        struct controller_transport_endpoint_runtime_info  *curr_runtime_info,
                                                        struct controller_transport_endpoint_runtime_info  *next_runtime_info);

int32_t dprm_register_namespace_notifications(uint32_t notification_type,
                                             dprm_namespace_notifier_fn namespace_notifier_fn,
                                             void     *callback_arg1,
                                             void     *callback_arg2);

int32_t dprm_namespace_init();
void dprm_namespace_deinit(void);
int32_t dprm_register_namespace(struct    dprm_namespace_info *namespace_info_p,uint64_t*  output_handle_p);
int32_t dprm_unregister_namespace(uint64_t handle);
int32_t dprm_update_namespace(uint64_t handle,struct dprm_namespace_info* namespace_info_p);
int32_t dprm_get_first_namespace(struct    dprm_namespace_info *namespace_info_p,
    uint64_t  *namespace_handle_p);
int32_t dprm_get_next_namespace(struct   dprm_namespace_info *namespace_info_p,
    uint64_t *namespace_handle_p);
int32_t  dprm_get_exact_namespace(uint64_t handle,
    struct dprm_namespace_info* namespace_info_p);
int32_t dprm_get_namespace_handle(char* name_p, uint64_t* namespace_handle_p);
int32_t dprm_delete_all_namespaces(void);

int32_t dprm_get_dpid_from_ns_attributevalue(char *ns_attribute_value, uint64_t *dpid);
int32_t dprm_get_dpid_and_portname_from_ns_attributevalue(char *ns_attribute_value, uint64_t *dpid, char *port_name, uint32_t *port_id);


/** \ingroup Datapath_Management_API
  \brief Get first attribute name from a registered datapath.\n
  <b>Description:</b>\n
  This function is used to get the first attribute name from a given datapath.

  \param[in] datapath_handle        - Unique handle to the datapath. 
  \param[out] attribute_output_p    - Returned get first attribute.
              
  \return DPRM_SUCCESS             - If get first attribute is successful.
  \return DPRM_FAILURE             - Failed to get the first attribute.
  \return DPRM_INVALID_DATAPATH_HANDLE  - If the datapath handle is invalid.
  */
int32_t dprm_get_datapath_first_attribute_name(uint64_t datapath_handle,
 struct dprm_attribute_name_value_output_info *attribute_output_p);


/** \ingroup Switch_Management_API
  \brief Get first attribute name from a registered ON Switch.\n
  <b>Description:</b>\n
  This function is used to get the first attribute name from a given ON Switch.

  \param[in] switch_handle          - Unique handle to the ON Switch. 
  \param[out] attribute_output_p    - Returned get first attribute.
              
  \return DPRM_SUCCESS             - If get first attribute is successful.
  \return DPRM_FAILURE             - Failed to get the first attribute.
  \return DPRM_INVALID_SWITCH_HANDLE  - If the ON Switch handle is invalid.
  */
int32_t dprm_get_switch_first_attribute_name(uint64_t switch_handle,
      struct   dprm_attribute_name_value_output_info *attribute_output_p);

/** \ingroup Domain_Management_API
  \brief Get first attribute name from a registered domain.\n
  <b>Description:</b>\n
  This function is used to get the first attribute name from a given domain.

  \param[in] domain_handle          - Unique handle to the domain. 
  \param[out] attribute_output_p    - Returned get first attribute.
              
  \return DPRM_SUCCESS             - If get first attribute is successful.
  \return DPRM_FAILURE             - Failed to get the first attribute.
  \return DPRM_INVALID_DOMAIN_HANDLE  - If the domain handle is invalid.
  */
int32_t dprm_get_domain_first_attribute_name(uint64_t domain_handle,
      struct   dprm_attribute_name_value_output_info *attribute_output_p);

/** \ingroup Datapath_Management_API
  \brief Get first attribute name from a registered datapath port.\n
  <b>Description:</b>\n
  This function is used to get the first attribute name from a given datapath port.

  \param[in] datapath_handle          - Unique handle to the datapath. 
  \param[in] port_handle              - Unique handle to the port. 
  \param[out] attribute_output_p      - Returned get first attribute.
              
  \return DPRM_SUCCESS             - If get first attribute is successful.
  \return DPRM_FAILURE             - Failed to get the first attribute.
  \return DPRM_INVALID_DATAPATH_HANDLE  - If the datapath handle is invalid.
  \return DPRM_INVALID_PORT_HANDLE      - If the port handle is invalid.
  */
int32_t dprm_get_datapath_port_first_attribute_name(uint64_t datapath_handle,
                                               uint64_t port_handle,
                                               struct   dprm_attribute_name_value_output_info *attribute_output_p);

/** \ingroup Datapath_Management_API
  \brief Get first attribute value from a registered datapath.\n
  <b>Description:</b>\n
  This function is used to get the first attribute value from a given datapath.

  \param[in] datapath_handle        - Unique handle to the datapath. 
  \param[out] attribute_output_p    - Returned get first attribute.
              
  \return DPRM_SUCCESS             - If get first attribute is successful.
  \return DPRM_FAILURE             - Failed to get the first attribute.
  \return DPRM_INVALID_DATAPATH_HANDLE  - If the datapath handle is invalid.
  */
int32_t dprm_get_datapath_attribute_first_value(uint64_t datapath_handle,
struct   dprm_attribute_name_value_output_info *attribute_output_p);

/** \ingroup Switch_Management_API
  \brief Get first attribute value from a registered ON Switch.\n
  <b>Description:</b>\n
  This function is used to get the first attribute value from a given ON Switch.

  \param[in] switch_handle          - Unique handle to the ON Switch. 
  \param[out] attribute_output_p    - Returned get first attribute.
              
  \return DPRM_SUCCESS             - If get first attribute is successful.
  \return DPRM_FAILURE             - Failed to get the first attribute.
  \return DPRM_INVALID_SWITCH_HANDLE  - If the ON Switch handle is invalid.
  */
int32_t dprm_get_switch_attribute_first_value(uint64_t switch_handle,
      struct   dprm_attribute_name_value_output_info *attribute_output_p);

/** \ingroup Domain_Management_API
  \brief Get first attribute value from a registered domain.\n
  <b>Description:</b>\n
  This function is used to get the first attribute value from a given domain.

  \param[in] domain_handle          - Unique handle to the domain. 
  \param[out] attribute_output_p    - Returned get first attribute.
              
  \return DPRM_SUCCESS             - If get first attribute is successful.
  \return DPRM_FAILURE             - Failed to get the first attribute.
  \return DPRM_INVALID_DOMAIN_HANDLE  - If the domain handle is invalid.
  */
int32_t dprm_get_domain_attribute_first_value(uint64_t domain_handle,
      struct   dprm_attribute_name_value_output_info *attribute_output_p);

/** \ingroup Datapath_Management_API
  \brief Get first attribute value from a registered datapath port.\n
  <b>Description:</b>\n
  This function is used to get the first attribute value from a given datapath port.

  \param[in] datapath_handle          - Unique handle to the datapath. 
  \param[in] port_handle              - Unique handle to the port. 
  \param[out] attribute_output_p      - Returned get first attribute.
              
  \return DPRM_SUCCESS             - If get first attribute is successful.
  \return DPRM_FAILURE             - Failed to get the first attribute.
  \return DPRM_INVALID_DATAPATH_HANDLE  - If the datapath handle is invalid.
  */
int32_t dprm_get_datapath_port_attribute_first_value(uint64_t datapath_handle, uint64_t port_handle,
      struct   dprm_attribute_name_value_output_info *attribute_output_p);

/** \ingroup Datapath_Management_API
  \brief Get next attribute value from a registered datapath.\n
  <b>Description:</b>\n
  This function is used to get the next attribute value from a given datapath.

  \param[in]  datapath_handle           - Unique handle to the datapath. 
  \param[in]  current_attribute_name    - Current attribute name. 
  \param[in]  current_attribute_value   - Current attribute value. 
  \param[out] attribute_output    - Returned get next attribute.
              
  \return DPRM_SUCCESS             - If get next attribute is successful.
  \return DPRM_FAILURE             - Failed to get the next attribute.
  \return DPRM_INVALID_DATAPATH_HANDLE  - If the datapath handle is invalid.
  */
int32_t dprm_get_datapath_attribute_next_value(uint64_t datapath_handle,
 char *current_attribute_name, char *current_attribute_value,
 struct   dprm_attribute_name_value_output_info *attribute_output);

/** \ingroup Switch_Management_API
  \brief Get next attribute value from a registered ON Switch.\n
  <b>Description:</b>\n
  This function is used to get the next attribute value from a given ON Switch.

  \param[in]  switch_handle             - Unique handle to the ON Switch. 
  \param[in]  current_attribute_name    - Current attribute name. 
  \param[in]  current_attribute_value   - Current attribute value. 
  \param[out] attribute_output_p        - Returned get next attribute.
              
  \return DPRM_SUCCESS                - If get next attribute is successful.
  \return DPRM_FAILURE                - Failed to get the next attribute.
  \return DPRM_INVALID_SWITCH_HANDLE  - If the ON Switch handle is invalid.
  */
int32_t dprm_get_switch_attribute_next_value(uint64_t switch_handle, char *current_attribute_name, char *current_attribute_value, struct   dprm_attribute_name_value_output_info *attribute_output_p);

/** \ingroup Domain_Management_API
  \brief Get next attribute value from a registered domain.\n
  <b>Description:</b>\n
  This function is used to get the next attribute value from a given domain.

  \param[in]  domain_handle             - Unique handle to the domain. 
  \param[in]  current_attribute_name    - Current attribute name. 
  \param[in]  current_attribute_value   - Current attribute value. 
  \param[out] attribute_output_p        - Returned get next attribute.
              
  \return DPRM_SUCCESS                - If get next attribute is successful.
  \return DPRM_FAILURE                - Failed to get the next attribute.
  \return DPRM_INVALID_DOMAIN_HANDLE  - If the domain handle is invalid.
  */
int32_t dprm_get_domain_attribute_next_value(uint64_t domain_handle, char *current_attribute_name, char *current_attribute_value, struct   dprm_attribute_name_value_output_info *attribute_output_p);

/** \ingroup Datapath_Management_API
  \brief Get next attribute value from a registered datapath port.\n
  <b>Description:</b>\n
  This function is used to get the next attribute value from a given datapath port.

  \param[in]  datapath_handle           - Unique handle to the datapath. 
  \param[in]  port_handle               - Unique handle to the port. 
  \param[in]  current_attribute_name    - Current attribute name. 
  \param[in]  current_attribute_value   - Current attribute value. 
  \param[out] attribute_output_p        - Returned get next attribute.
              
  \return DPRM_SUCCESS                - If get next attribute is successful.
  \return DPRM_FAILURE                - Failed to get the next attribute.
  \return DPRM_INVALID_DATAPATH_HANDLE  - If the datapath handle is invalid.
  \return DPRM_INVALID_PORT_HANDLE      - If the port handle is invalid.
  */
int32_t dprm_get_datapath_port_attribute_next_value(uint64_t datapath_handle, uint64_t port_handle, char *current_attribute_name, char *current_attribute_value, struct   dprm_attribute_name_value_output_info *attribute_output_p);

/** \ingroup Datapath_Management_API
  \brief Get next attribute name from a registered datapath.\n
  <b>Description:</b>\n
  This function is used to get the next attribute name from a given datapath.

  \param[in]  datapath_handle           - Unique handle to the datapath. 
  \param[in]  current_attribute_name    - Current attribute name. 
  \param[out] attribute_output_p        - Returned get next attribute.
              
  \return DPRM_SUCCESS                - If get next attribute is successful.
  \return DPRM_FAILURE                - Failed to get the next attribute.
  \return DPRM_INVALID_DATAPATH_HANDLE  - If the datapath handle is invalid.
  */
int32_t dprm_get_datapath_next_attribute_name(uint64_t datapath_handle,
      char*    current_attribute_name,
      struct   dprm_attribute_name_value_output_info *attribute_output_p);

/** \ingroup Switch_Management_API
  \brief Get next attribute name from a registered ON Switch.\n
  <b>Description:</b>\n
  This function is used to get the next attribute name from a given ON Switch.

  \param[in]  switch_handle             - Unique handle to the datapath. 
  \param[in]  current_attribute_name    - Current attribute name. 
  \param[out] attribute_output_p        - Returned get next attribute.
              
  \return DPRM_SUCCESS                - If get next attribute is successful.
  \return DPRM_FAILURE                - Failed to get the next attribute.
  \return DPRM_INVALID_SWITCH_HANDLE  - If the ON Switch handle is invalid.
  */
int32_t dprm_get_switch_next_attribute_name(uint64_t switch_handle,
      char*    current_attribute_name,
      struct   dprm_attribute_name_value_output_info *attribute_output_p);

/** \ingroup Domain_Management_API
  \brief Get next attribute name from a registered domain.\n
  <b>Description:</b>\n
  This function is used to get the next attribute name from a given domain.

  \param[in]  domain_handle             - Unique handle to the domain. 
  \param[in]  current_attribute_name    - Current attribute name. 
  \param[out] attribute_output_p        - Returned get next attribute.
              
  \return DPRM_SUCCESS                - If get next attribute is successful.
  \return DPRM_FAILURE                - Failed to get the next attribute.
  \return DPRM_INVALID_DOMAIN_HANDLE  - If the domain handle is invalid.
  */
int32_t dprm_get_domain_next_attribute_name(uint64_t domain_handle,
      char*    current_attribute_name,
      struct   dprm_attribute_name_value_output_info *attribute_output_p);

/** \ingroup Datapath_Management_API
  \brief Get next attribute name from a registered datapath port.\n
  <b>Description:</b>\n
  This function is used to get the next attribute name from a given datapath port.

  \param[in]  datapath_handle           - Unique handle to the datapath. 
  \param[in]  port_handle               - Unique handle to the port. 
  \param[in]  current_attribute_name    - Current attribute name. 
  \param[out] attribute_output_p        - Returned get next attribute.
              
  \return DPRM_SUCCESS                - If get next attribute is successful.
  \return DPRM_FAILURE                - Failed to get the next attribute.
  \return DPRM_INVALID_DATAPATH_HANDLE  - If the datapath handle is invalid.
  \return DPRM_INVALID_PORT_HANDLE      - If the port handle is invalid.
  */
int32_t dprm_get_datapath_port_next_attribute_name(uint64_t datapath_handle, uint64_t port_handle,
char *current_attribute_name, struct   dprm_attribute_name_value_output_info *attribute_output_p);

#ifdef OF_XML_SUPPORT

struct dprm_resource_runtime_info {
    int32_t number_of_datapaths;
    int32_t number_of_physical_switches;
    int32_t number_of_namespaces;
};

struct ipsec_resource_runtime_info {
    int32_t max_policy_nodes;
    int32_t max_sa_nodes;
    int32_t genid_max_instaceid_value;
};
#endif /* OF_XML_SUPPORT */

int32_t dprm_get_switch_data_ip(char* name_p, uint32_t* switch_data_ip_p);
int32_t get_swname_by_dphandle(uint64_t datapath_handle,char** sw_name_p_p);

#endif /*__DPRM_H*/
