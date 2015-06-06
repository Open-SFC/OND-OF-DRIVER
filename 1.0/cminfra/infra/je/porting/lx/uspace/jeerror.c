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
 * File name: jeerror.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/05/2013
 * Description: 
*/

#ifdef CM_SUPPORT
//#ifdef CM_JE_SUPPORT

#include "jeincld.h"
#include "jeldef.h"
#include "jelif.h"
#include "jewrap.h"

/******************************************************************************
 * * * * * * * * * * * * * GLOBAL VARIABLES * * * * * * * * * * * * * * * * 
 *****************************************************************************/

char *UCMJEErrMsgs[] = {

  /* Name and Value Pairs */
  "Nv Pairs Name is NULL",      /* 0 */
  "Nv Pairs Value is NULL",
  ERROR_RESERVED,
  ERROR_RESERVED,
  ERROR_RESERVED,

  /*Element Data Value type */
  "Element Value is not a valid integer",       /* 5 */
  "Element Value is not a valild unsigned integer",
  "Element Value is not a valid string",
  "Element Value is not a valid date and time",
  "Element Value is not a valid IP address",
  "Element Value is not a valid Boolean value", /*10 */
  "Element Value's type is invalid",    
  "Element Value is not a valid string. XML chars are not allowed",
  ERROR_RESERVED,
  ERROR_RESERVED,

/*Element Data Value Semantic type */
  "Element Value is not in a defined integer range",    /* 15 */
  "Element Value is not in a defined unsigned integer range",
  "Element Value is not in a defined integer enumerators",
  "Element Value is not in a defined unsigned integer enumerators",
  "Element Value is not in a defined string range",
  "Element Value is not in a defined string enumerators",       /* 20 */
  "Element Value is not in a defined ip address range",
  "Element Value is not in a defined ip address enumerators",
  "UCMJE_DATA_ATTRIB_INVALID",  /* 23 */
  ERROR_RESERVED,
  ERROR_RESERVED,               /* 25 */
  ERROR_RESERVED,
  ERROR_RESERVED,
  ERROR_RESERVED,
  ERROR_RESERVED,

  /*je errors */
  "Configuration Session already exists",       /* 30 */
  "Configuration Session start failed",
  "Unknown Configuration Session",
  "Configuration Session is null",
  "Unknown sub command for update",
  ERROR_RESERVED,               /* 35 */
  ERROR_RESERVED,
  ERROR_RESERVED,
  ERROR_RESERVED,
  ERROR_RESERVED,

	/* Transport Channel */
  "Unknown Transport Channel",  /* 40 */
  "Unknown Transport Channel Sub Command",
  "UCMJE_ERROR_MEMORY_ALLOCATION_FAILED",
  "UCMJE_ERROR_VALIDATION_FAILED",
  "Referrence not found",
  "UCMJE_ERROR_GET_INSTANCES_FAILED",   /* 45 */
  "UCMJE_ERROR_VERIFICATION_FAILED",
  "UCMJE_ERROR_CONSOLIDATION_FAILED",
  ERROR_RESERVED,
  ERROR_RESERVED,

  /* Application Callbacks */
  "Application Callbacks are NULL",     /* 50 */
  "Test Callback is NULL",
  "Test Callback returned Failure",
  "Invalid combination for Begin and End Callbacks",
  "Begin Callback is NULL",
  "Add Callback is NULL",       /* 55 */
  "Modify Callback is NULL",
  "Delete Callback is NULL",
  "End Callback is NULL",
  "Begin Callback returned Failure",

  "Add Callback returned failure",      /* 60 */
  "Modify Callback returned failure",
  "Delete Callback returned failure",
  "End Callback returned failure",
  "Get First Callback is NULL",
  "Get Next Callback is NULL",  /* 65 */
  "Get Exact Callback is NULL",
  "Get First N Records Application Callback returned failure",
  "Get Next N Records Application Callback returned failure",
  "Get Exact Record Application Callback returned failure",

  "Invalid Application Get Command",    /* 70 */
  "Request to fetch zero records",
  "Set Default Value Failed",
  "Mandatory Parameter is not set",
  "Command to be cancelled not found in configuration session",
  "Key Parameter could not be unset in configuration session",      /* 75 */
  "Mandatory Parameter not allowed to set default Values",
  ERROR_RESERVED,
  ERROR_RESERVED,
  ERROR_RESERVED,

  "Get New Configuration version Failed",       /* 80 */
  "Adding Commands to New Configuration version failed",
  "UCMJE_ERROR_CONFIG_CACHE",
  "UCMJE_ERROR_GENERATE_AUDITLOG_FAILED",
  "UCMJE_ERROR_NOTIFICATION_CONFIGUPDATE_FAILED",
  ERROR_RESERVED,               /* 85 */
  ERROR_RESERVED,
  ERROR_RESERVED,
  ERROR_RESERVED,
  ERROR_RESERVED,

  "Adding Instance Entry to DM Instance Tree Failed",   /* 90 */
  "Get New Instance ID failed",
  "Set Instance Id failed",
  "Get Instances List for ID failed",
  "Delete Instance Failed",
  "Delete Inode Failed", /*95*/
  "Create NvPair From Instance Entry Failed",
  "One of the keys in instance path is NULL",
  ERROR_RESERVED,
  ERROR_RESERVED,

  "Data Model Path is empty / NULL",    /* 100 */
  "Invalid Data Model Path",
  "Removing Keys from Data Model path failed",
  "Get DM Nodel by Path failed",
  "Get DM Model Node's Child by its Name failed",
  "Get DM Model Node's Child by its Id failed", /* 105 */
  "Get DM Model Keys array from its Name path failed",
  "Create NamePath from DM Node failed",
  "Create InstancePath from Referrence Node failed",
  "Permission Denied",

  "Get DM Node Info Failed",    /* 110 */
  "Get DM Node And Child Info Failed",
  "Get DM Key Child Info Failed",
  "Get DM First Node Info Info Failed",
  "Get DM Next Node Info Info Failed",

  "Get DM Keys Array Failed",  /* 115 */
  "Get DM Child Node Info Failed",
  "Get DM Child Count Failed",
  "Get Save File Name Failed",               
  "Get Save Directory Name Failed",   
  "Is not a Child Node for a given node", /*120 */
  "Unknown DM Sub Command",
  "Wrong data model Elements, Key is true but mandatory is false for an element, both should be TRUE in case of table or False in case of Grouped scalar",
  "Wrong data model element, Grouped scalar element is defined as mandatory",
  ERROR_RESERVED,
  ERROR_RESERVED, /*125 */
  ERROR_RESERVED,
  ERROR_RESERVED,
  ERROR_RESERVED,
  ERROR_RESERVED,


  "Get DM Instance Id Failed", /* 130 */
  "Add DM Instance Entry Failed",
  "Get DM Instance List by Name Failed",
  "Delete DM Instance Entry Failed",
  "Is not a DM Instance",
  "Is not a DM Instance", /*135*/
  "Maximum Allowed Instances Reached",
  ERROR_RESERVED,
  ERROR_RESERVED,
  ERROR_RESERVED,

	/* Roles */
	"Get DM Node Role Attributes Failed", /*140 */
	"Get DM Node Role Attributes By Role Failed", 
	"Set DM Node Role Attributes By Role Failed",
  "Delete DM Node Role Attributes By Role Failed",
  "Get DM Instance Role Attributes Failed",
  "Set DM Instance Role Attributes Failed",/*145*/
  "Get DM Instance Role Attributes By Role Failed",
  "Set DM DM Instance Role Attributes By Role Failed",  
  "Delete DM Instance Role Attributes By Role Failed",
  ERROR_RESERVED,


/*LDSV*/
  "Send Command to LDSV failed",        /* 150 */
  "Unknown LDSV Sub-Command",
  "Creating Transport Channel with LDSV failed",
  "No configuration changes after previous save. Saved configuration already exists.",
  "Save is already in progress",
  "Save configuration failed",
  "Load is already in progress",
  "Loading the configuration failed",

	/*Authentication*/
  "UCMJE_ERROR_PERMISSION_DENIED",
};

char *UCMJESuccessMsgs[] = {
	 "Version history reached its maximum limit. Please issue save command to avoid version history loss.",
	 "Version history will be lost, if save and runtime configuration difference is 100. To avoid this, please issue Save command",
};
/******************************************************************************
 * * * * * * * * * * * * * FUNCTION DEFINITIONS * * * * * * * * * * * * * * * * 
 *****************************************************************************/
/**
  \ingroup JEWRPAPI
   This API is used to Get Proper Error Message for given error code.
   <b>Input Parameters: </b>\n
   <b><i>err_index_i:</i></b> Error Code
   <b>Output Params</b>\n
   <b><i>err_arr_p:</b></i> Pointer to Error Message
   <b>Return Value:</b>OF_SUCCESS or OF_FAILUREE\n
 **/

/******************************************************************************
 ** Function Name : cm_je_get_err_msg
 ** Description   : This API is used to Get Error Message for give Error code
 ** Input Params  : err_index_i - Error Code
 ** Output Params : err_arr_p -  Pointer to Error Message
 ** Return value  : OF_SUCCESS     - In Success case
 **               : OF_FAILURE     - In Failure case
 *****************************************************************************/

int32_t cm_je_get_err_msg (int32_t err_index_i, char * err_arr_p)
{
  if (err_index_i == OF_FAILURE)
  {
    snprintf (err_arr_p, UCMJE_ERROR_STRING_LENGTH - 1, "%s",
                "cm_je_get_err_msg : Unknown Error");
    return OF_FAILURE;
  }
  else if ((err_index_i >= UCMJE_ERROR_MAX_ERROR) ||
           (err_index_i < CM_JE_ERROR_BASE))
  {
    snprintf (err_arr_p, UCMJE_ERROR_STRING_LENGTH - 1, "%s",
                "cm_je_get_err_msg : Index is Out of UCMJE Error Msgs Limits");
    return OF_FAILURE;
  }
  else
  {
    err_index_i -= CM_JE_ERROR_BASE;
    snprintf (err_arr_p, UCMJE_ERROR_STRING_LENGTH - 1,
                "%s", UCMJEErrMsgs[err_index_i]);
  }
  return OF_SUCCESS;
}

int32_t cm_je_get_success_msg (int32_t success_code_i, char * success_msg_p)
{
  if (success_code_i == OF_FAILURE)
  {
    snprintf (success_msg_p, UCMJE_SUCCESS_STRING_LENGTH - 1, "%s",
                "cm_je_get_success_msg : Unknown Error");
    return OF_FAILURE;
  }
  else if ((success_code_i >= UCMJE_SUCCESS_MAX_SUCCESSCODE) ||
           (success_code_i < CM_JE_SUCCESS_BASE))
  {
    snprintf (success_msg_p, UCMJE_SUCCESS_STRING_LENGTH - 1, "%s",
                "cm_je_get_success_msg : Index is Out of UCMJE Success Msgs Limits");
    return OF_FAILURE;
  }
  else
  {
    success_code_i -= CM_JE_SUCCESS_BASE;
    snprintf (success_msg_p, UCMJE_SUCCESS_STRING_LENGTH - 1,
                "%s", UCMJESuccessMsgs[success_code_i]);
  }
  return OF_SUCCESS;
}
//#endif /* CM_JE_SUPPORT */
#endif /* CM_SUPPORT */
