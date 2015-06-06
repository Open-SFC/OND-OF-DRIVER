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
 * File name: cbkerror.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: 
 * 
 */

#ifdef OF_CM_SUPPORT 

#include"cbcmninc.h"
/******************************************************************************
 * * * * * * * * * * * * * GLOBAL VARIABLES * * * * * * * * * * * * * * * * 
 *****************************************************************************/


char* UCMGluErrMsgs[] = {
  /* Generic error messages */ 
  "Specify a valid ip range" , /* 0 */  
  "Specify a valid port range",
  "IP address start range should be lower than end range", 
  "Port start range should be lower than end range",
  "Specify a valid ip address", 
  "Specify a valid port value",
  "Specify a valid subnet/mask address",
  "Specify a valid domain name",   
  "Memory allocation failed",
  "Input is NULL", 
  "No such record found", /* 10 */
  "Specify a valid start port",
  "Specify a valid end port",

  /* Snet related error messages */
   "Invalid Snet name", /* 13 */
   "No Snet exist with this name",
   "Add Snet record failed at security application",

   
   /* IPDB related error messages */  
   "Duplicate IPDB record", /* 16 */
   "IPDB record does not exist",
   "Invalid IPDB name",  
   "Specify a valid IPDB record type" ,
   "Begin transaction failed for IPDB",
   "End Transaction failed for IPDB",    
   "Add IPDB failed", /*22*/
   "Modify IPDB failed",
   "Delete IPDB failed", 
   "Get IPDB failed",

  /* Service DB related erro messages */
   "Duplicate Service DB record", /* 26 */
   "Service DB record does not exist",
   "Invalid Service DB name",  
   "Specify a valid Service DB port type" ,
   "Begin transaction failed for Service DB",
   "End Transaction failed for Service DB",    
   "Add Service DB failed", /*32*/
   "Modify Service DB failed",
   "Delete Service DB failed", 
   "Get Service DB failed",
   "Autosync set status failed",

   
  /*  ACL related error messages */
  "Duplicate ACL record", /* 36*/ 
  "ACL record doesn't exist ",
  "Rule id out of range",  
  "Specify a valid rule id",
  "Specify a valid source ip type", 
  "Specify a valid destination ip type",
  "Specify a valid source ip address", /* 42 */
  "Specify a valid destination ip address",
  "Specify a valid source ip name",
  "Specify a valid destination ip name",
  "Specify a valid source ip start/end range",
  "Specify a valid destination ip start/end range",
  "Specify a valid source ip subnet address",
  "Specify a valid source ip mask",
  "Specify a valid destination ip subnet address",
  "Specify a valid destination ip mask",
  "Specify a valid source net type", /* 52*/
  "Specify a valid destination net type",
  "Specify a valid insert before rule id",
  "Specify a valid insert after rule id",
  "Specify a valid insertion position", 
  "Source ip start range is greater than end range",
  "Destination ip start range is greater than end range",
  "Source port start range is greater than end range", 
  "Destination port start range is greater than end range",
  "Specify a valid source port start range",
  "Specify a valid source port end range", /* 62 */
  "Specify a valid destination port start range",
  "Specify a valid destination port end range",  
  "Specify a valid source port range",
  "Specify a valid destination port range",
  "If protocol is other than tcp/udp ports should not be configured ",
  "Protocol should not be configured if Destination Port is ServiceRecord ",

   /* Ethernet related error messages */
   "Specify a valid interface name", /* 69 */
   "Changing the status of an alias interface is not allowed",
   "WAN interface configuration is not allowed",
   "Specify a valid default route option", /* 72 */
   "Hardware Address configuration is not allowed",
   
   /* VLAN related error messages */
   "VLAN Glue failed", /* 74 */
   "Add VLAN interface failed", /* 75 */
   "Set VLAN Nametype failed",  
   "Set VLAN flag failed",
   "Specify a valid SNet type",
   "Specify a VLAN name to delete",
   "VLAN physical interface not modifiable",
   "VLAN SNet type not modifiable",

   /* TimeWindow related error messages */
   "Specify a valid time window name ", /* 82 */
   "Duplicate time window record", /* 83 */
   "Specify a valid opening day",
   "Specify a valid closing day",
   "Specify a valid opening hour",
   "Specify a valid closing hour",

   /* Alias related error messages */
   "Specify a valid Alias Name", /* 88 */
   "Add Alias Interface Failed",
   "Set Alias Interface IP Address Failed",
   "Delete Alias Interface Failed",

   /* ARP Entry related error messages */
   "Unknown host", /* 91 */
   "Invalid hardware address", /* 92 */

   /* Proxy ARP related error messages */
    "Specify a valid Proxy ARP record name", /* 93 */
    "Duplicate Proxy ARP record",
    "Invalid start ip address",
    "Invalid end ip address",

		/* Association Reservation*/
   "Specify a valid secure network type", /*97 */
   "Specify a valid ip type",
   "Specify a valid association reservation count",

   /* Ratelimiting */
   "RateLimit record addition failed", /* 100 */
   "RateLimit record modification failed",
   "Ratelimit record modify is not allowed",/* 102*/
   "RuleID, SnetName, Connection, Network type are required to delete a record",
   "Invalid Limit type", /* 104*/
   
   /* Port Trigger */
   "Invalid port trigger name",
   "Invalid trigger protocol",
   "Invalid trigger port range",
   
   /* VSG Zones related error messages */   
   "Changing Zone ID is not Allowed",
   "Invalid zone name",
   "Duplicate zone name found",
   "Zone name is not modifiable",
   "Zone modification failed",
   "Failed to Associate Interface to VSG Zone.",
   "Failed to De-Associate Interface to VSG Zone.",
    
   /* SSL error messages */
   "CA Certificate file name is null.",
   "Self Certificate file name is null.",
   "Private Key Certificate file name is null.",
      
/* Namespace error messages */
  "Namespace is null",
  "Namespace addition failed",
  "Namespace does not exist",
  "Namespace deletion failed",
  "Namespace modification failed",
  "Namespace name length exceeded",

   /* Datapath error messages */
    "Domain Name is null.",
    "Datapath ID is null.",
    "Invalid Datapath ID.",
    "Failed to delete the datapath record.",

   /* Domain error messages */
    "Domain add failed.",
    "Domain delete failed.",
    "Unable to find the domain record.",
    "Subject Name is null.",
    "Number of tables is null.",
    "Number of tables is exceeding the maximum allowed number.",

   /* Switch error messages */
   "Switch Name is null.",
   "Switch add failed.",
   "Switch does not exist with the entered name.",
   "Unable to find the matching record.",
   "Failed to delete the Switch record.",
   "Switch edit failed.",
   "Fully Qualified Domain Name (FQDN) is null.",
   "Switch Name is exceeding the maximum allowed length.",
   "Baddr IP is null",
   /* Group error messages */
   "Datapath does not exist with entered id.",    
   "Group add failed.",
   "Group edit failed.",
   "Group does not exist with entered id.",
   "Failed to send group add message to the switch.", 
   "Group type is not valid.",
   "Weight Parameter is required for Group Type SELECTED",
   "Watch port or Watch Group need to be specified for Group Type FAST FAILOVER",
   "Only ONE Bucket is allowed for Group Type INDIRECT",

  
   /* Port error messages */
   "Port Name is null.",
   "Port ID is null.",
   "Invalid Port ID entered.",
   "Datapath port add failed.",
   "Failed to delete the port record.",
   "Unable to find the matching port record.",

   /* Group description */
   "Group Description type is not valid.",
   
   /* Flowmod error messages */
   "Static Flow add failed.",
   "Flow add failed.",
   "Flow does not exist with entered name.",
   "Failed to send flow add message to switch.",
   "Priority,Cookie and Cookie Mask Required for modify_strict/delete_strict.",
   "Priority required for add command",
   "Match field should be empty for a miss entry",
   "GoToTable id must be less than the current table id",

   /* Bucket error messages*/
   "Bucket add to the group failed.",
   "Bucket update to the group failed.",
   "Bucket does not exist with entered id.",
   
   /* Action error messages */ 
   "Action add to the group failed.",
   "Action does not exist with entered type.",
   "Action type is invalid.",
   "Port Number or Maximum Length is not configured.",
   "TTL value is not configured.",
   "Ether Type is not configured.",
   "Queue ID is not configured.",
   "Group ID is not configured.",
   
   /* Meter error messages */
   "Datapath does not exist with entered id.",    
   "Meter add failed.",
   "Meter edit failed.",
   "Meter does not exist with entered id.",
   "Failed to send Meter add message to the switch.", 
   "Meter type is not valid.",
   "Rate is Required",
   "Rate and prec_level is required",

   /* Meter Config */
   "Meter Config type is not valid.",
   
   /* Band error messages*/
   "Band add to the meter failed.",
   "Band update to the meter failed.",
   "Band does not exist with entered id.",
   
   /* Few common messages */   
   "Failed to set the mandatory parameters.",
   "Failed to set the optional parameters.",
   "Table ID is null.",
   "Memory allocation failed.",
   "Validation check failed for the optional parameters.",

   /* Port Attribute Messages  */
   "Attribute addition failed.",
   "Attribute modification failed.",
   "Get First Attribute name failed.",
   "Attribute deletion failed.",
   "Controller role does not exist.",
   "Slave cannot set the async message configuration!",
   "Set async message options failed!",
   "Set async configuration message failed!",
   "Get name space count fail!",
   "Get name space flag fail!",
   "Multiple-namespace Enabled and Already namespace present!",

   /* CRM Error Messages */
   "Tenant addition failed!",
   "Tenant deletion failed!",   
   "Tenant Name is NULL",
   "Tenant Name is invalid",
   "Trace Module must be Specified",
   "Virtual Machine add failed!",
   "Virtual Machine delete failed!",
   "Virtual Machine name is NULL",
   "Virtual Machine type is NULL",
   "Switch Name is null",
   "Virtual Network add failed",
   "Virtual Network Modify failed",
   "Virtual Network delete failed",
   "Virtual Network name is null",
   "Virtual Network name is invalid",
   "Virtual Network type is null",
   "Virtual Network type is invalid",
   "Virtual Network segment id is null",
   "Virtual Network vxlan service port null",
   "vlan network .enter segementation id",
   "vxlan network .enter segementation id and service port",
   "Virtual Network description is null",
   "Virtual Network description is invalid",
   "Compute node name null",
   "Bridge name null",
   "Compute Node Addition failed",
   "Compute Node Deletion failed",
   "Subnet add failed",
   "Subnet delete failed",
   "Subnet name is NULL",
   "Subnet name is invalid",
   "Subnet pool start ID is null",
   "Subnet pool end ID is null",
   "Subnet IP is null",
   "subnet mask is null",
   "Subnet Gateway IP is NULL",
   "Subnet IP version NULL",
   "Subnet IP version invalid",
   "Subnet Enable DHCP is NULL",
   "Subnet Enable DHCP is invalid",
   "Subnet DNS Server1 is NULL",
   "Subnet DNS Server2 is NULL",
   "Subnet DNS Server3 is NULL",
   "Subnet DNS Server4 is NULL",
   "Attribute Name Null",
   "Attribute Value Null",
   "Invalid port type",
   "Invalid port name",
   "crm port add failed.",
   "port type null",
   "vm side port mac null",
   "bridge name invalid",
   "vm name invalid",
   "network type invalid",
   "port name invalid",
   "network id null",
   "switch name invalid",
   "vlan  null",
   "vxlan vni null",
   "Service port null",
   "Remote ip null",
   "selected vlan network. enter vlan id",
   "selected vxlan network . enter vni id,serviceport and remote ip",

   /*Transport Error Strings */
   "Invalid Protocol ID , must be between 0 to 255 and value not define before",
   /*Experimenter bindstats , spd and sa stats error messages*/
   "IPSEC direction is null",
   "SPD policy ID is null",
   "SA SPI is null",
   "SA destination IP is null",
   "SA protocol is null",
   /*NSRM*/
   "NSchain selection rule add failed",
   "NSchain selection rule delete failed",
   "NSchain selection rule modify failed",
   "NSchain selection rule name is null",
   "NSchain selection rule name is invalid",

   "Nwservice bypass rule add failed",
   "Nwservice bypass rule delete failed",
   "Nwservice bypass rule modify failed",
   "Nwservice bypass rule name is null",
   "Nwservice bypass rule name is invalid",
   "Nwservice attached to bypass rule is null",
   "Previous nwservice name not set",

   "NSchain set add failed",
   "NSchain set delete failed",
   "NSchain set modify failed",
   "NSchain set name is null",
   "NSchainset name invalid",

   "NS chain add failed",
   "Ns chain delete failed",
   "Ns chain modify failed",
   "Ns chain name null",
   "Ns chain name is invalid",
   "Ns chain type is null",
   "Ns chain type is invalid",
   "Tenant is invalid",

   "L2NW map  add failed",
   "L2NW map delete failed",
   "L2NW map modify failed",
   "L2NW map name is null",
   "L2NW map name is invalid",
   "This L2NW map , maps to some other VN",

   "Nwservice add failed",
   "Nwservice delete failed",
   "Nwservice modify failed",
   "Nwservice name is null",
   "Nwservice name is invalid",
   

   "Service instance add failed",
   "Service instance del failed",
   "Service instance name null",
   
  };

/******************************************************************************
 * * * * * * * * * * * * * FUNCTION DEFINITIONS * * * * * * * * * * * * * * * * 
 *****************************************************************************/
/**
  \ingroup 
   This API is used to Get Proper Error Message for given error code.
   <b>Input Parameters: </b>\n
   <b><i>err_index_i:</i></b> Error Code
   <b>Output Params</b>\n
   <b><i>err_arr_p:</b></i> Pointer to Error Message
   <b>Return Value:</b>OF_SUCCESS or OF_FAILUREE\n
 **/

/******************************************************************************
 ** Function Name : UCMAppGluGetErrMsg
 ** Description   : This API is used to Get Error Message for give Error code
 ** Input Params  : err_index_i - Error Code
 ** Output Params : err_arr_p -  Pointer to Error Message
 ** Return value  : OF_SUCCESS     - In Success case
 **               : OF_FAILURE     - In Failure case
 *****************************************************************************/

int32_t UCMAppGluGetErrMsg (int32_t err_index_i, char * err_arr_p)
{
   if (err_index_i == OF_FAILURE)
   {
      snprintf (err_arr_p, CM_SECAPPGLU_ERROR_STRING_LENGTH - 1, "%s",
            "UCMAppGluErrMsg : Unknown Error");
      return OF_FAILURE;
   }
   else if ((err_index_i >= CM_GLU_MAX_ERR) ||
         (err_index_i < CM_SECAPPGLU_ERROR_BASE))
   {
      snprintf (err_arr_p, CM_SECAPPGLU_ERROR_STRING_LENGTH - 1, "%s",
            "UCMAppGluErrMsg : Index is Out of UCMAppGlu Error Msgs Limits");
      return OF_FAILURE;
   }
   else
   {
      err_index_i -= CM_SECAPPGLU_ERROR_BASE;
      snprintf (err_arr_p, CM_SECAPPGLU_ERROR_STRING_LENGTH - 1,
            "%s", UCMGluErrMsgs[err_index_i]);
   }
   return OF_SUCCESS;
}
#endif
