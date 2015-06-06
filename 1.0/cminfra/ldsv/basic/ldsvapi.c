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
 * File name:   ldsvapi.c
 * Author:      Freescale Semiconductor    
 * Date:        06/12/2013
 * Description: This file contains source code implementing 
 *              LDSV (persistent store manager) API
*/

#ifdef CM_LDSV_SUPPORT

#include "ldsvinc.h"
#include "dirent.h"
#include "sys/types.h"
#include "sys/param.h"
#include "sys/stat.h"
#include "unistd.h"
#include "fcntl.h"
#include "errno.h"
#include "hash.h"
#include "basic_ldsvinit.h"

//#ifndef VORTIQA_SYSLOG_SUPPORT
//#include "ucmmesg.h"
//#endif
#include "jesyslog.h"
#include "jeigwlog.h"
/* Global Variables */
#define CM_SLEEP_AFTER_DATAPATH_LOAD_CONFIG()  \
{\
if(!strcmp(pFileName, "datapath.xml"))\
{\
	CM_LDSV_DEBUG_PRINT("Sleeping 10 secs to establish channel.xml");\
	sleep(10);\
}\
}
void *pTransportChannelJE;    /* Transport connection with JE */

char aCurrLoadingVersionNo_g[80]="";

extern uint64_t ulVersionNo_g;
/**
  \ingroup LDSVAPI
  This API is used to save the Input Data. Save operation is initiated by UCM JE engine. This   process is in 2 ways,
  i)Explicit Save operation
  Administrator issues a “save configuration” command via any management engine.
  ii)Implicit Save operation
  UCM JE detected that there are N pending configuration versions from previous save operation

  LDSV engine acts same for both the above operation.When Save command is received by LDSV 
  engine,  it traverses through the data model elements using DM Path,and for each element it 
  invokes JE APIs to extract the configuration.The retrieved configuration data is then saved
  in to LDSV save configuration file.
  Saved configuration file is devided into 3 sections,
  1. Header information - Save file related information like file name, description, dmpath, instance path and version
  2. Dictionary section - Data Attributes for a table, comes from Data Model Attributes.
  3. Data section - Actual admin configuration data.
  Sample saved configuration file as shown below,

  <configuration>
	<header>
		<filename>vsg.xml</filename>
		<description>VSG Table</description>
		<version>1</version>
		<dmpath>igd.vsg</dmpath>
		<instancepath>igd.vsg</instancepath>
	</header>
  <body>
	<record name='vsg' type='1' relative_dmpath='vsg'/>
	<dictionary>
		<a0>vsgid  type=2</a0>
		<a1>nsid  type=2</a1>
		<a2>vsgname  type=1</a2>
	</dictionary>
		<row>
			<a0>0</a0>
			<a1>0</a1>
			<a2>general</a2>
		</row>
  </body>
  </configuration>

  <b>Input paramameters: </b>\n
  <b><i> dm_path_p: </i></b> Pointer to Configuration Data Model Path to Save the data.
  <b>Output Parameters: </b>
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */

/**************************************************************************
  Function Name :cm_ldsv_save_config
  Input Args    :dm_path_p - Data model Path, using this path we can traverse dm tree
  Output Args   :
Description   :This method is used to save configuration data into persistent store.
Return Values :OF_SUCCESS/OF_FAILURE
 *************************************************************************/
/*
 * The below steps explains the save logic.
 * Let us take this simple DM tree.

 igd
 +----------------------------------+
 |                                  |
 vsg (Table : VSG1, VSG2)           SysInfo
 |
 +----------+
 Firewall    NetObj
 |         |
 Acl_params   ipdb_param

 1) call --> cm_dm_get_first_node_info(...)
 Got "igd".
 As it is anchor, just Write the anchor line to the ldsv file.
 2) call --> cm_dm_get_next_node_info(...)
 got "vsg".
 As it is table, We need to get table rows to write to ldsv file.
 we need to traverse entire vsg subtree for VSG1, 
 traverse entire subtree for VSG2

 pOpaqueInfo contains enough information for DMtree to give you the next node.

 call --> UCMGetFirstNRecords(...)
 got  --> VSG1
 Write this VSG1 to ldsv file.  

 Now we need to traverse the entire subtree for VSG1. So push the
 VSG1 information, and also information to go to first element of 
 VSG subtree(opaque info) into a table_stack. 

 3) call --> cm_dm_get_next_node_info()
 Got "Firewall"
 As it is anchor, just Write the anchor line to the ldsv file.
 4) call --> cm_dm_get_next_node_info()
 Got "Acl_param".
 To read "Acl_param" data, we need its parent table keys. We have that
 key info in table_stack. Collect the keys from table_stack. As there
 is only one node, we got the key "VSG1".

 Now get the fw_param for VSG1.
 call GetExactRecord(...,
 keysarray = VSG1, 
 ...);

 write the received fw_param (of VSG1) to ldsv file.
 5) call --> cm_dm_get_next_node_info()
 Got "NetObj"
 As it is anchor, just Write the anchor line to the ldsv file.
 6) call --> cm_dm_get_next_node_info()
 Got "ipdb_param".

 To read "ipdb_param" data, we need its parent table keys. We have that
 key info in table_stack. Collect the keys from table_stack. As there
 is only one node, we got the key "VSG1".

 Now get the fw_param for VSG1.
 call UCMGetExactRecord(igd.vsg.ipsec.ipsec_param,
 keysarray = VSG1, 
 ...);

 write received ipsec_param (of VSG1) to ldsv file.
 7) call --> cm_dm_get_next_node_info()
 Got "SysInfo".

 we need to traverse the vsg subtree for instance VSG2. But
 got a node, out of VSG subtree. How do we stop this?

 Check whether "SysInfo" is a child of of last entry (vsg) in table stack node.
No. It is not. So Donot save SysInfo. Do the following
GetNextRecordInfo of (last_element_of_table_stack_node);
Got "VSG2".
update the instance info VSG2 in last element of table stack.

Now here is the tricky part. As we moved to next instance VSG2, we
have to traverse the entire vsg subtree for VSG2. To do this, we
have opaque info stored in the table_stack node. use it.

8) call --> cm_dm_get_next_node_info(...)
   Got "Firewall" for VSG2

   As it is anchor, just Write the anchor line to the ldsv file.
9) call --> cm_dm_get_next_node_info(...)
   Got "Acl_param". for VSG2
   To read "Acl_param" data, we need its parent table keys. We have that
   key info in table_stack. Collect the keys from table_stack. As there
   is only one node, we got the key "VSG2".

   Now get the fw_param for VSG2.
   call UCMGetExactRecord(igd.vsg.firewall.fw_param,
         keysarray = VSG2, 
         ...);

   write the received fw_param (of VSG2) to ldsv file.
10) call --> cm_dm_get_next_node_info()
   Got "NetObj"
   As it is anchor, just Write the anchor line to the ldsv file.

11) call --> cm_dm_get_next_node_info()
   Got "ipdb_param".

   To read "ipsec_param" data, we need its parent table keys. We have that
   key info in table_stack. Collect the keys from table_stack. As there
   is only one node, we got the key "VSG2".

   Now get the fw_param for VSG2.
   call UCMGetExactRecord(igd.vsg.ipsec.ipsec_param,
         keysarray = VSG2, 
         ...);

write received ipsec_param (of VSG2) to ldsv file.

12) call --> cm_dm_get_next_node_info()
   Got "SysInfo".

   Check whether "SysInfo" is a child of of last entry (vsg) in table stack node.
   No. It is not. Just check whether we have completed all our VSG instances or not.

   GetNextRecordInfo of (last_element_of_table_stack_node);
   Got "No more records".
   Popup vsg entry from table_stack.
   Fine, Then proceed with saving SysInfo.

   UCMGetExactRecord (igd.SysInfo,
         KeysArray = Any nodes in table stack? No. So NULL.
         ..);

   write the sysinfo to ldsv file.
   Got No more nodes.
   Done Saving.

   Sample saved configuration file is in the following format,

   <configuration>
   	<header>
		<filename>vsg.xml</filename>
		<description>VSG Table</description>
		<version>1</version>
		<dmpath>igd.vsg</dmpath>
		<instancepath>igd.vsg</instancepath>
	</header>
   <body>
	<record name='vsg' type='1' relative_dmpath='vsg'/>
	<dictionary>
		<a0>vsgid  type=2</a0>
		<a1>nsid  type=2</a1>
		<a2>vsgname  type=1</a2>
	</dictionary>
		<row>
			<a0>0</a0>
			<a1>0</a1>
			<a2>general</a2>
		</row>
   </body>
   </configuration>

   */


int32_t cm_save_config (char * dm_path_p, uint64_t uiVersionNo)
{
   int32_t iRet;
   int32_t iInstRet;
   char aVersionNo[20+1]; /*UINT64 Verison consists: UINT32 ->eng ver and config ver. this should be convertd as 'x.y'*/
   uint32_t uiRecordCount = 0;
   uint64_t ullTempVer=0;
   uint32_t iCount = 0;
   void *pTransportChannel = NULL;
   struct cm_dm_node_info *DMNodeAttrib = NULL;
   void *pOpaqueInfo = NULL;
   uint32_t uiOpaqueInfoLen = 0;
   struct cm_array_of_nv_pair *pArrayOfKeys = NULL;
   struct cm_array_of_nv_pair *pKeysArray = NULL;
   struct cm_array_of_nv_pair *pSecApplData = NULL;
   struct cm_array_of_nv_pair *pKeyNvPair = NULL;
   unsigned char bTableStackDone = FALSE;
   unsigned char bDMTplTraverseDone = FALSE;
   unsigned char bIsChildNode = TRUE;
   char *pDirPath = NULL;
   char *pTempDirPath = NULL;
   char *pDMInstancePath = NULL;
   char aRoleFullPath[LDSV_DMPATH_SIZE];
   char aFullPath[LDSV_DMPATH_SIZE];
   char aRmCmd[LDSV_DMPATH_SIZE]={};
   char aBuffer[LDSV_DMPATH_SIZE];
   char *pFileName;
   char *pDMFileName;
   char *pRoleFileName;
   char aDmPath[LDSV_MAX_BUFFER_SIZE];

   UCMDllQNode_t *pDllqNode;
   cm_ldsv_table_stack_node_t *pStackNode;
   UCMDllQ_t TableStackNode;
   UCMDllQ_t *pLdsvCfgStackDllQ = &TableStackNode;
   struct cm_role_info RoleInfo = { "VortiQa", CM_VORTIQA_SUPERUSER_ROLE };
   struct cm_array_of_nv_pair *dm_path_pArrayOfKeys = NULL;
   struct cm_hash_table *pDictonaryTable = NULL;
#ifdef CM_ROLES_PERM_SUPPORT
   struct cm_dm_array_of_role_permissions *pRolePermAry = NULL;
#endif
   CM_LDSV_DEBUG_PRINT("dm_path_p=%s", dm_path_p);

   /* Get the existing Transport Channel with JE */
   pTransportChannel = cm_ldsv_get_transport_channel();
   /* Validate transport channel */
   if (unlikely (pTransportChannel == NULL))
   {
      pTransportChannel = cm_tnsprt_create_channel (CM_IPPROTO_TCP, 0, 0,
            UCMJE_CONFIGREQUEST_PORT);
   }

   /*initializing double linked list */
   CM_DLLQ_INIT_LIST (&TableStackNode);
   /* 
      Save can be issued on any sub-tree of the DM tree. 
      So If it is issued on a sub-tree, and if there are some parent 
      table nodes to top of us, We need to build this stack with all 
      those parent nodes.
      */
   if ((dm_path_p != NULL) && (strlen(dm_path_p)>0))
   {
      if(strcmp(dm_path_p, CM_DM_ROOT_NODE_PATH)!=0)
      {
         /* If Dm Template Path passed 
          * frame Keys Array not needed for root node,
          */
         if (cm_dm_get_keys_array_from_name_path (pTransportChannel, dm_path_p,
                  CM_LDSV_MGMT_ENGINE_ID,
                  &RoleInfo,
                  &dm_path_pArrayOfKeys) != OF_SUCCESS)
         {
            CM_LDSV_DEBUG_PRINT ("Get Keys failed");
            return OF_FAILURE;
         }
      } 
   }
   else 
   {
      /* Don't return from here, if dmpath is null we need to save from root node*/
      CM_LDSV_DEBUG_PRINT ("DMPath is NULL");
   }
   /*******************************************
     Start Traversing the DM template tree.
    ******************************************/
   iRet = cm_dm_get_first_node_info (dm_path_p,
         UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
         pTransportChannel,
         CM_LDSV_MGMT_ENGINE_ID,
         &RoleInfo,
         &DMNodeAttrib, &pOpaqueInfo, &uiOpaqueInfoLen);
   if (DMNodeAttrib == NULL)
   {
      CM_LDSV_DEBUG_PRINT ("GetFirstNodeInfo failed.");
      return OF_FAILURE;
   }

   cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
         CM_SAVE_STARTED_EVENT, (void*) &uiVersionNo);
   pDictonaryTable = cm_hash_create (MAX_HASH_SIZE);
   if (pDictonaryTable == NULL)
   {
      CM_LDSV_DEBUG_PRINT ("Dictonary Table is not created");
      cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
            CM_SAVE_FAILED_EVENT, (void*) &uiVersionNo);
      return OF_FAILURE;
   }
   pDirPath = (char *) of_calloc (1, LDSV_DMPATH_SIZE);
   if (!pDirPath)
   {
      CM_LDSV_DEBUG_PRINT("Failed to create memory for pDirPath");
      cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo, uiOpaqueInfoLen);
      // call xmlhashfree
      if (cm_hash_size (pDictonaryTable) != 0)
      {
         CM_LDSV_DEBUG_PRINT ("cleaning hash entries.");
         cm_hash_free (pDictonaryTable, NULL);
      }
      cm_ldsv_clean_dm_node_attrib_info (DMNodeAttrib);
      cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
            CM_SAVE_FAILED_EVENT, (void*) &uiVersionNo);
#ifdef CM_AUDIT_SUPPORT
      cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_SAVE_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
            CM_CMD_SAVE_CONFIG,dm_path_p,NULL, NULL, "Failed to create memory for Directory Path", uiVersionNo); 
#endif
      return OF_FAILURE;
   }
   pRoleFileName = (char *) of_calloc (1, LDSV_DMPATH_SIZE);
   if (pRoleFileName == NULL)
   {
      CM_LDSV_DEBUG_PRINT("Failed to create memory for pRoleFileName");
      of_free(pDirPath);
      cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo, uiOpaqueInfoLen);
      cm_ldsv_clean_dm_node_attrib_info (DMNodeAttrib);
      // call xmlhashfree
      if (cm_hash_size (pDictonaryTable) != 0)
      {
         CM_LDSV_DEBUG_PRINT ("cleaning hash entries.");
         cm_hash_free (pDictonaryTable, NULL);
      }
      cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
            CM_SAVE_FAILED_EVENT, (void*) &uiVersionNo);
#ifdef CM_AUDIT_SUPPORT
      cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_SAVE_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
            CM_CMD_SAVE_CONFIG,dm_path_p,NULL, NULL, "Failed to create memory for Role File Name", uiVersionNo); 
#endif
      return OF_FAILURE;
   }
   pFileName = (char *) of_calloc (1, LDSV_DMPATH_SIZE);
   if (pFileName == NULL)
   {
      CM_LDSV_DEBUG_PRINT("Failed to create memory for pRoleFileName");
      of_free(pDirPath);
      cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo, uiOpaqueInfoLen);
      cm_ldsv_clean_dm_node_attrib_info (DMNodeAttrib);
      // call xmlhashfree
      if (cm_hash_size (pDictonaryTable) != 0)
      {
         CM_LDSV_DEBUG_PRINT ("cleaning hash entries.");
         cm_hash_free (pDictonaryTable, NULL);
      }
      cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
            CM_SAVE_FAILED_EVENT, (void*) &uiVersionNo);
#ifdef CM_AUDIT_SUPPORT
      cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_SAVE_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
            CM_CMD_SAVE_CONFIG,dm_path_p,NULL, NULL, "Failed to create memory for Role File Name", uiVersionNo); 
#endif
      return OF_FAILURE;
   }
   pTempDirPath = (char *) of_calloc (1, LDSV_DMPATH_SIZE);
   if (pTempDirPath == NULL)
   {
      CM_LDSV_DEBUG_PRINT("Failed to create memory for pTempDirPath");
      of_free(pDirPath);
      of_free(pRoleFileName);
      cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo, uiOpaqueInfoLen);
      cm_ldsv_clean_dm_node_attrib_info (DMNodeAttrib);
      // call xmlhashfree
      if (cm_hash_size (pDictonaryTable) != 0)
      {
         CM_LDSV_DEBUG_PRINT ("cleaning hash entries.");
         cm_hash_free (pDictonaryTable, NULL);
      }
      cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
            CM_SAVE_FAILED_EVENT, (void*) &uiVersionNo);
#ifdef CM_AUDIT_SUPPORT
      cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_SAVE_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
            CM_CMD_SAVE_CONFIG,dm_path_p,NULL, NULL, "Failed to create memory for Directory Path", uiVersionNo); 
#endif
      return OF_FAILURE;
   }

   pDMInstancePath = (char *) of_calloc (1, LDSV_DMPATH_SIZE);
   if (pDMInstancePath == NULL)
   {
      CM_LDSV_DEBUG_PRINT("Failed to create memory for pDMInstancePath");
      of_free(pDirPath);
      of_free(pTempDirPath);
      of_free(pRoleFileName);
      cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo, uiOpaqueInfoLen);
      cm_ldsv_clean_dm_node_attrib_info (DMNodeAttrib);
      // call xmlhashfree
      if (cm_hash_size (pDictonaryTable) != 0)
      {
         CM_LDSV_DEBUG_PRINT ("cleaning hash entries.");
         cm_hash_free (pDictonaryTable, NULL);
      }
      cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
            CM_SAVE_FAILED_EVENT, (void*) &uiVersionNo);
#ifdef CM_AUDIT_SUPPORT
      cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_SAVE_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
            CM_CMD_SAVE_CONFIG,dm_path_p,NULL, NULL, "Failed to create memory for Instance Path", uiVersionNo); 
#endif
      return OF_FAILURE;
   }


   CM_READ_VERSION(uiVersionNo, aVersionNo) ;
#if 0
   ullTempVer = uiVersionNo;
   /* Get engine version first*/
   ullTempVer = ((ullTempVer>>32)&0x00000000ffffffff);
   /* If Version is 'zero'(0), then UCMitoa, prints NULL character. Instead of changing that function copying '0'.
   */
   if(ullTempVer !=0)
   {
      UCMitoa(ullTempVer, aVersionNo);
   }
   else
   {
      strcpy(aVersionNo,"0");
   }
   strcat(aVersionNo,".");
   ullTempVer = uiVersionNo;
   ullTempVer = ullTempVer&0x00000000ffffffff;
   if(ullTempVer !=0)
   {
      UCMitoa(ullTempVer, aVersionNo+strlen(aVersionNo));
   }
   else
   {
      strcpy(aVersionNo+strlen(aVersionNo),"0");
   }
#endif /*0*/
   memset (aFullPath, 0, sizeof(aFullPath));
   strncat(aFullPath, PERSISTENT_TRANSFERLIST_DIR, strlen(PERSISTENT_TRANSFERLIST_DIR));
   strncat(aFullPath, aVersionNo, strlen(aVersionNo));
   sprintf(aRmCmd,"rm -rf %s",aFullPath);
   /* Create original directory for the engver.configver*/
   memset (aBuffer, 0, sizeof(aBuffer));
   sprintf (aBuffer, "mkdir -p %s", aFullPath);
   iRet = system (aBuffer);
   if (iRet != OF_SUCCESS)
   {

      CM_LDSV_DEBUG_PRINT("Failed to create a directory. aBuffer:%s",aBuffer);
      // call xmlhashfree
      if (cm_hash_size (pDictonaryTable) != 0)
      {
         CM_LDSV_DEBUG_PRINT ("cleaning hash entries.");
         cm_hash_free (pDictonaryTable, NULL);
      }
      of_free(pDirPath);
      of_free(pTempDirPath);
      cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo, uiOpaqueInfoLen);
      cm_ldsv_clean_dm_node_attrib_info (DMNodeAttrib);
      of_free(pRoleFileName);
      of_free(pFileName);
      cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
            CM_SAVE_FAILED_EVENT, (void*) &uiVersionNo);
      CM_LDSV_DEBUG_PRINT("Removing Directory:%s", aRmCmd);
      iRet=system(aRmCmd);
      if (iRet != OF_SUCCESS)
      {
         CM_LDSV_DEBUG_PRINT("Command %s  failed",aRmCmd);
      }
#ifdef CM_AUDIT_SUPPORT
      cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_SAVE_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
            CM_CMD_SAVE_CONFIG,dm_path_p,NULL, NULL, "Failed to create directory", uiVersionNo); 
#endif
      return OF_FAILURE;
   }

   CM_LDSV_DEBUG_PRINT("\t %s.%d: Creating dir=%s:  SUCCESS \n",__FUNCTION__,__LINE__,aFullPath);
   while (iRet == OF_SUCCESS)
   {
      /* If there are some parent table nodes to us, we need to retrive
       *  the current element data - for every combination of parent table
       *  keys. for finding next node, we need to pass parent node, so we
       *  are managing list of parent nodes in a stack for future use.
       */

      /* Now Get the Data that need to be saved */
      pArrayOfKeys = cm_ldsv_collect_keys_from_table_stack (pLdsvCfgStackDllQ, 
            dm_path_pArrayOfKeys);
      if (DMNodeAttrib == NULL)
      {
         CM_LDSV_DEBUG_PRINT("DM Node Attributes are null");
         cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo, uiOpaqueInfoLen);
         cm_ldsv_clean_dm_node_attrib_info (DMNodeAttrib);
         cm_ldsv_delete_table_stack (pLdsvCfgStackDllQ);
         cm_ldsv_clean_nv_pair_array (pArrayOfKeys, TRUE);
         cm_ldsv_clean_nv_pair_array (dm_path_pArrayOfKeys, TRUE);
         cm_ldsv_clean_nv_pair_array (pKeysArray, TRUE);
         // call xmlhashfree
         if (cm_hash_size (pDictonaryTable) != 0)
         {
            CM_LDSV_DEBUG_PRINT ("cleaning hash entries.");
            cm_hash_free (pDictonaryTable, NULL);
         }
         of_free(pRoleFileName);
         of_free(pFileName);
         cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
               CM_SAVE_FAILED_EVENT, (void*) &uiVersionNo);

                 CM_LDSV_DEBUG_PRINT("Removing Directory:%s", aRmCmd);
         iRet=system(aRmCmd);
         if (iRet != OF_SUCCESS)
         {
            CM_LDSV_DEBUG_PRINT("Command %s  failed",aRmCmd);
         }
#ifdef CM_AUDIT_SUPPORT
         cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_SAVE_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
               CM_CMD_SAVE_CONFIG,dm_path_p,NULL, NULL, "Failed to get keys array", uiVersionNo); 
#endif
         return OF_FAILURE;
      }
      if( DMNodeAttrib->element_attrib.device_specific_b == FALSE &&
            (DMNodeAttrib->element_attrib.non_config_leaf_node_b == FALSE ||
             DMNodeAttrib->element_attrib.non_config_leaf_node_b == TRUE_ADD))
      {
         switch (DMNodeAttrib->element_attrib.element_type)
         {
            case CM_DMNODE_TYPE_ANCHOR:
               /* in ldsv save file, we are saving data model instance path along with
                * data model path. so, prepare dm instance path using datamodel Attributes,
                * since we have only datamodel path. 
                */
               cm_ldsv_prepare_instance_dm_path (DMNodeAttrib, pDMInstancePath,
                     dm_path_pArrayOfKeys,
                     pLdsvCfgStackDllQ);
               memset (pDirPath, 0, sizeof (pDirPath));
               memset(pFileName, 0, sizeof(pFileName));
               /* 
                * Creating ldsv save file directory path
                */
               cm_ldsv_create_directory_path (pDMInstancePath, pDirPath);
               /* Anchor nodes write it into a file directly. we are not doing any
                  operations with anchor nodes, like IGD, Firewall Node, VPN etc */
               CM_LDSV_CREATE_COMPLETE_FILE_PATH (aFullPath, pDirPath,
                     DMNodeAttrib->name_p, aVersionNo);
               CM_LDSV_CREATE_COMPLETE_DIR_PATH (pTempDirPath, pDirPath, aVersionNo);
               memset (aBuffer, 0, sizeof(aBuffer));
               sprintf (aBuffer, "mkdir -p %s", aFullPath);
               iRet = system (aBuffer);
               if (iRet != OF_SUCCESS)
               {
                  CM_LDSV_DEBUG_PRINT("Failed to create a directory.");
                  // call xmlhashfree
                  if (cm_hash_size (pDictonaryTable) != 0)
                  {
                     CM_LDSV_DEBUG_PRINT ("cleaning hash entries.");
                     cm_hash_free (pDictonaryTable, NULL);
                  }
                  of_free(pDirPath);
                  of_free(pTempDirPath);
                  cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo, uiOpaqueInfoLen);
                  cm_ldsv_clean_dm_node_attrib_info (DMNodeAttrib);
                  of_free(pRoleFileName);
                  of_free(pFileName);
                  cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
                        CM_SAVE_FAILED_EVENT, (void*) &uiVersionNo);

                 CM_LDSV_DEBUG_PRINT("Removing Directory:%s", aRmCmd);
                  iRet=system(aRmCmd);
                  if (iRet != OF_SUCCESS)
                  {
                     CM_LDSV_DEBUG_PRINT("Command %s  failed",aRmCmd);
                  }
#ifdef CM_AUDIT_SUPPORT
                  cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_SAVE_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
                        CM_CMD_SAVE_CONFIG,dm_path_p,NULL, NULL, "Failed to create a Directory", uiVersionNo); 
#endif
                  return OF_FAILURE;
               }

               if(DMNodeAttrib->element_attrib.scalar_group_b == TRUE)
               {
                  if ((DMNodeAttrib->file_name == NULL) || strlen(DMNodeAttrib->file_name) == 0) 
                  {
                     cm_dm_get_file_name(pTransportChannel, CM_LDSV_MGMT_ENGINE_ID,
                           DMNodeAttrib->dm_path_p, RoleInfo.admin_role, &pDMFileName);
                     if ((pDMFileName == NULL) || (strlen(pDMFileName) == 0))
                     {
                        //CM_LDSV_DEBUG_PRINT("File name is null, so can't proceed for saving");
                        break;
                     }
                     memcpy(pFileName, pDMFileName, strlen(pDMFileName)+1);
                  }
                  else
                  {
                     memcpy(pFileName, DMNodeAttrib->file_name, strlen(DMNodeAttrib->file_name)+1);
                  }
                  strcat (pFileName, ".xml");
                  CM_LDSV_CREATE_COMPLETE_FILE_PATH (aFullPath, pDirPath, pFileName, aVersionNo);

                  //CM_LDSV_DEBUG_PRINT (" Calling GetExactRecord");
                  iInstRet = cm_get_exact_record (pTransportChannel,
                        CM_LDSV_MGMT_ENGINE_ID,
                        CM_LDSV_ENG_ADMIN_ROLE,
                        DMNodeAttrib->dm_path_p,
                        pArrayOfKeys, &pSecApplData);
                  if ((iInstRet == OF_FAILURE) || (pSecApplData == NULL))
                  {
                     // CM_LDSV_DEBUG_PRINT ("Failed to get Exact record");
                     break;
                  }
                  cm_ldsv_write_header (pFileName, aFullPath, pTempDirPath, 
                        pDMInstancePath, DMNodeAttrib);

                  cm_ldsv_write_dictionary (DMNodeAttrib, pTransportChannel,
                        pDictonaryTable, aFullPath);
                  /* Write this first instance data to XML file */
                  cm_ldsv_write_to_file (pSecApplData, pDictonaryTable, 
                        aFullPath, DMNodeAttrib, pTransportChannel);
                  cm_ldsv_close_open_xml_tags (aFullPath);
               }
#ifdef CM_ROLES_PERM_SUPPORT
               memset(pRoleFileName, 0, sizeof(pRoleFileName));
               strncat(pRoleFileName, DMNodeAttrib->name_p, strlen(DMNodeAttrib->name_p)+1);
               strcat(pRoleFileName, "Roles.xml");
               CM_LDSV_CREATE_COMPLETE_FILE_PATH (aRoleFullPath, pDirPath, pRoleFileName, aVersionNo);

               UCMDM_GetRolePermissions (pTransportChannel,
                     CM_LDSV_MGMT_ENGINE_ID, pDMInstancePath,
                     CM_VORTIQA_SUPERUSER_ROLE,
                     &pRolePermAry);
               if (pRolePermAry != NULL)
               {
                  cm_ldsv_create_backup (pRoleFileName, pTempDirPath);
                  cm_write_role_permissions_info(pRolePermAry, aRoleFullPath, pDMInstancePath);
               }
#endif
               break;

            case CM_DMNODE_TYPE_TABLE:
               /* Call GetFirst, GetNext Functions */
               {
                  cm_ldsv_prepare_instance_dm_path (DMNodeAttrib, pDMInstancePath,
                        dm_path_pArrayOfKeys,
                        pLdsvCfgStackDllQ);
                  memset (pDirPath, 0, sizeof (pDirPath));

                  cm_ldsv_create_directory_path (pDMInstancePath, pDirPath);
                  iCount = iCount + 1;
                  uiRecordCount = 1;
                  memset(pFileName, 0, sizeof(pFileName));
                  if (((DMNodeAttrib->file_name) == NULL) || strlen(DMNodeAttrib->file_name) == 0)
                  {
                     cm_dm_get_file_name(pTransportChannel, CM_LDSV_MGMT_ENGINE_ID,
                           DMNodeAttrib->dm_path_p, RoleInfo.admin_role, &pDMFileName);
                     if ((pDMFileName == NULL) || (strlen(pDMFileName) == 0))
                     {
                        //CM_LDSV_DEBUG_PRINT("File name is null, so can't proceed for saving");
                        break;
                     }
                     strncat(pFileName, pDMFileName, strlen(pDMFileName));
                  }
                  else
                  {
                     strncat(pFileName, DMNodeAttrib->file_name, strlen(DMNodeAttrib->file_name));
                  }
                  strcat (pFileName, ".xml");

                  CM_LDSV_CREATE_COMPLETE_FILE_PATH (aFullPath, pDirPath, pFileName, aVersionNo);
                  CM_LDSV_CREATE_COMPLETE_DIR_PATH (pTempDirPath, pDirPath, aVersionNo);

                  /* get instace tree table records */
                  /*CM_LDSV_DEBUG_PRINT ("Calling GetFirstN Records for %s",
                    DMNodeAttrib->dm_path_p);*/
                  iInstRet =
                     cm_get_first_n_records (pTransportChannel, CM_LDSV_MGMT_ENGINE_ID,
                           CM_LDSV_ENG_ADMIN_ROLE, DMNodeAttrib->dm_path_p,
                           pArrayOfKeys, &uiRecordCount, &pSecApplData);
                  if ((iInstRet == OF_FAILURE) || (pSecApplData == NULL) ||(uiRecordCount == 0) || (pSecApplData->count_ui) == 0)
                  {
                     /* No need to traverse any sub trees,
                      * since no nodes found in instance tree of the given DM path */
                     CM_LDSV_DEBUG_PRINT (" Get First N Records failed for ::%s", DMNodeAttrib->dm_path_p);
                     break;
                  }
                  cm_ldsv_write_header (pFileName, aFullPath, pTempDirPath, pDMInstancePath,
                        DMNodeAttrib);
                  cm_ldsv_write_dictionary (DMNodeAttrib, pTransportChannel,
                        pDictonaryTable, aFullPath);

                  /* Write this first instance data to XML file */
                  cm_ldsv_write_to_file (pSecApplData, pDictonaryTable, 
                        aFullPath, DMNodeAttrib, pTransportChannel);

                  // this condition is only for closing tags of vsg file
                  // when we try to save from root node.
#if 0
                  if((iCount==1) && (dm_path_pArrayOfKeys==NULL))
                  {
                     cm_ldsv_close_open_xml_tags (aFullPath);
                     // call xmlhashfree
                     if (cm_hash_size (pDictonaryTable) != 0)
                     {
                        CM_LDSV_DEBUG_PRINT ("cleaning hash entries.");
                        cm_hash_free (pDictonaryTable, NULL);
                     }
                     iCount = iCount+1;
                     pDictonaryTable = cm_hash_create (MAX_HASH_SIZE);
                     if (pDictonaryTable == NULL)
                     {
                        CM_LDSV_DEBUG_PRINT ("Dictonary Table is not created");
                        cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
                              CM_SAVE_FAILED_EVENT, (void*) &uiVersionNo);
                        return OF_FAILURE;
                     }
                  }
#endif /*0*/
                  /*
                     Findout the key of current record. This is useful to get next
                     record in future.
                     */
                  pKeyNvPair = cm_ldsv_find_key_nv_pair (DMNodeAttrib, pSecApplData,
                        pTransportChannel);
                  if (unlikely (pKeyNvPair == NULL))
                  {
                     /* if no key value pairs found, clean the necessasary
                      * information and return T_FAULURE*/
                     cm_ldsv_close_open_xml_tags (aFullPath);
                     cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo, uiOpaqueInfoLen);
                     cm_ldsv_clean_dm_node_attrib_info (DMNodeAttrib);
                     cm_ldsv_delete_table_stack (pLdsvCfgStackDllQ);
                     cm_ldsv_clean_nv_pair_array (pArrayOfKeys, TRUE);
                     cm_ldsv_clean_nv_pair_array (dm_path_pArrayOfKeys, TRUE);
                     cm_ldsv_clean_array_of_nv_pair_array (pSecApplData, uiRecordCount);
                     of_free (pDirPath);
                     of_free (pTempDirPath);
                     of_free(pDMInstancePath);
                     // call xmlhashfree
                     if (cm_hash_size (pDictonaryTable) != 0)
                     {
                        CM_LDSV_DEBUG_PRINT ("cleaning hash entries.");
                        cm_hash_free (pDictonaryTable, NULL);
                     }
                     of_free(pRoleFileName);
                     of_free(pFileName);
                     cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
                           CM_SAVE_FAILED_EVENT, (void*) &uiVersionNo);

                 CM_LDSV_DEBUG_PRINT("Removing Directory:%s", aRmCmd);
                     iRet=system(aRmCmd);
                     if (iRet != OF_SUCCESS)
                     {
                        CM_LDSV_DEBUG_PRINT("Command %s  failed",aRmCmd);
                     }
#ifdef CM_AUDIT_SUPPORT
                     cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_SAVE_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
                           CM_CMD_SAVE_CONFIG,dm_path_p,NULL, NULL, "Failed to get key Nv Pair array", uiVersionNo); 
#endif
                     return OF_FAILURE;
                  }

                  /* Push the first table instance to stack */
                  pStackNode =
                     cm_ldsv_create_table_stack_node (DMNodeAttrib, DMNodeAttrib->dm_path_p,
                           DMNodeAttrib->name_p, pOpaqueInfo,
                           uiOpaqueInfoLen, pKeyNvPair);

                  if (unlikely (pStackNode == NULL))
                  {
                     // asssuming table inside table records, so don;t return from here.
                     break;
                  }
#ifdef CM_ROLES_PERM_SUPPORT
                  memset(aDmPath, 0, sizeof(aDmPath));
                  memset(pRoleFileName, 0, sizeof(pRoleFileName));
                  strcpy (aDmPath, pDMInstancePath);
                  strncat(pRoleFileName, DMNodeAttrib->file_name, strlen(DMNodeAttrib->file_name)+1);
                  strcat(pRoleFileName, "Roles.xml");
                  CM_LDSV_CREATE_COMPLETE_FILE_PATH (aRoleFullPath, pDirPath, pRoleFileName, aVersionNo);
                  UCMDM_GetRolePermissions (pTransportChannel,
                        CM_LDSV_MGMT_ENGINE_ID, aDmPath,
                        CM_VORTIQA_SUPERUSER_ROLE,
                        &pRolePermAry);
                  if (pRolePermAry != NULL)
                  {
                     cm_ldsv_create_backup (pRoleFileName, pTempDirPath);
                     cm_write_role_permissions_info(pRolePermAry, aRoleFullPath, aDmPath);
                  }

                  memset(aDmPath, 0, sizeof(aDmPath));
                  memset(pRoleFileName, 0, sizeof(pRoleFileName));
                  strcpy (aDmPath, pDMInstancePath);
                  strncat (aDmPath, "{", 1);
                  strncat (aDmPath, pKeyNvPair->NvPairs[0].value_p,
                        pKeyNvPair->NvPairs[0].value_length);
                  strncat (aDmPath, "}", 1); 
                  strncat(pRoleFileName, DMNodeAttrib->file_name, strlen(DMNodeAttrib->file_name)+1);
                  strcat(pRoleFileName, "Roles.xml");
                  CM_LDSV_CREATE_COMPLETE_FILE_PATH (aRoleFullPath, pDirPath, pRoleFileName, aVersionNo);
                  UCMDM_GetInstanceRolePermissions (pTransportChannel,
                        CM_LDSV_MGMT_ENGINE_ID, aDmPath,
                        CM_VORTIQA_SUPERUSER_ROLE, &pRolePermAry);

                  if (pRolePermAry != NULL)
                  {
                     cm_ldsv_create_backup (pRoleFileName, pTempDirPath);
                     cm_write_role_permissions_info(pRolePermAry, aRoleFullPath, aDmPath);
                  }
#endif
                  memset(aDmPath, 0, sizeof(aDmPath));
                  strcpy (aDmPath, pDMInstancePath);

                  /* Now Cleanup the last Record data */
                  cm_ldsv_clean_array_of_nv_pair_array (pSecApplData, uiRecordCount);
                  pSecApplData = NULL;
                  /* Now append the node to table stack */
                  CM_DLLQ_APPEND_NODE (pLdsvCfgStackDllQ, &(pStackNode->ListNode));
                  break;
               }
            case CM_DMNODE_TYPE_SCALAR_VAR:
               /* Call Get-Exact Function 
                  Write to the XML file 
                  skip all non-configurable leaf nodes */

               CM_LDSV_DEBUG_PRINT (" Calling GetExactRecord");
               iInstRet = cm_get_exact_record (pTransportChannel,
                     CM_LDSV_MGMT_ENGINE_ID,
                     CM_LDSV_ENG_ADMIN_ROLE,
                     DMNodeAttrib->dm_path_p,
                     pArrayOfKeys, &pSecApplData);
               if (iInstRet == OF_FAILURE)
               {
                  CM_LDSV_DEBUG_PRINT ("Failed to get Exact record");
                  break;
               }
               cm_ldsv_write_to_file (pSecApplData, pDictonaryTable, aFullPath, 
                     DMNodeAttrib, pTransportChannel);

#ifdef CM_ROLES_PERM_SUPPORT
               UCMDM_GetRolePermissions (pTransportChannel,
                     CM_LDSV_MGMT_ENGINE_ID,  DMNodeAttrib->dm_path_p,
                     CM_VORTIQA_SUPERUSER_ROLE,
                     &pRolePermAry);
               if (pRolePermAry != NULL)
                  cm_write_role_permissions_info(pRolePermAry, aRoleFullPath, DMNodeAttrib->dm_path_p);
#endif
               break;

            default:
               break;
         }                           /* switch */
      }

      /* Now get the next node from DM template tree */
      iRet = cm_dm_get_next_table_n_anchor_node_info (dm_path_p,
            UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
            pTransportChannel,
            CM_LDSV_MGMT_ENGINE_ID,
            &RoleInfo,
            &DMNodeAttrib,
            &pOpaqueInfo, &uiOpaqueInfoLen);
      if (iRet == OF_FAILURE)
      {
         CM_LDSV_DEBUG_PRINT (" Tree Completed");
         bDMTplTraverseDone = TRUE;
         if (CM_DLLQ_COUNT (pLdsvCfgStackDllQ) == 0)
         {
            CM_LDSV_DEBUG_PRINT (" Stack is empty ");
            /*Clean up runtime data */
            cm_ldsv_delete_table_stack (pLdsvCfgStackDllQ);
            cm_ldsv_clean_dm_node_attrib_info (DMNodeAttrib);
            cm_ldsv_clean_nv_pair_array (pArrayOfKeys, TRUE);
            cm_ldsv_clean_nv_pair_array (dm_path_pArrayOfKeys, TRUE);
            cm_ldsv_clean_nv_pair_array (pKeysArray, TRUE);
            of_free (pDirPath);
            of_free (pTempDirPath);
            of_free(pDMInstancePath);
            of_free(pRoleFileName);
            // call xmlhashfree
            CM_LDSV_DEBUG_PRINT ("cleaning hash entries.");
            cm_hash_free (pDictonaryTable, NULL);

            memset(aFullPath, 0, sizeof(aFullPath));
            CM_LDSV_CREATE_COMPLETE_FILE_PATH (aFullPath, "", CM_CONFIGSYNC_CONFIG_VER_FILE, "");
            if(cm_ldsv_write_version_number_to_file(aFullPath, uiVersionNo) != OF_SUCCESS)
            {
               CM_LDSV_DEBUG_PRINT("Write version no into file is failed.");
            }
            cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
                  CM_SAVE_COMPLETED_EVENT, (void*) &uiVersionNo);
#ifdef CM_AUDIT_SUPPORT
            cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_SAVE_SUCCESS, RoleInfo.aAdminName, RoleInfo.admin_role,
                  CM_CMD_SAVE_CONFIG,dm_path_p,NULL, NULL, NULL, uiVersionNo); 
#endif
            return OF_SUCCESS;
         }
      }

      /*-----------------------------------------------------------*/
      if (CM_DLLQ_COUNT (pLdsvCfgStackDllQ) != 0)
      {
         pDllqNode = CM_DLLQ_LAST (pLdsvCfgStackDllQ);
         pStackNode =
            CM_DLLQ_LIST_MEMBER (pDllqNode, cm_ldsv_table_stack_node_t *, ListNode);
         if (pStackNode == NULL)
         {
            cm_ldsv_delete_table_stack (pLdsvCfgStackDllQ);
            cm_ldsv_clean_dm_node_attrib_info (DMNodeAttrib);
            cm_ldsv_clean_nv_pair_array (pArrayOfKeys, TRUE);
            cm_ldsv_clean_nv_pair_array (pKeysArray, TRUE);
            cm_ldsv_clean_nv_pair_array (dm_path_pArrayOfKeys, TRUE);
            of_free (pDirPath);
            of_free (pTempDirPath);
            of_free (pDMInstancePath);

            // call xmlhashfree
            if (cm_hash_size (pDictonaryTable) != 0)
            {
               CM_LDSV_DEBUG_PRINT ("cleaning hash entries.");
               cm_hash_free (pDictonaryTable, NULL);
            }

            of_free(pRoleFileName);
            of_free(pFileName);
            cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
                  CM_SAVE_FAILED_EVENT, (void*) &uiVersionNo);

                 CM_LDSV_DEBUG_PRINT("Removing Directory:%s", aRmCmd);
            iRet=system(aRmCmd);
            if (iRet != OF_SUCCESS)
            {
               CM_LDSV_DEBUG_PRINT("Command %s  failed",aRmCmd);
            }
#ifdef CM_AUDIT_SUPPORT
            cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_SAVE_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
                  CM_CMD_SAVE_CONFIG,dm_path_p,NULL, NULL,"Stack Node is NULL", uiVersionNo); 
#endif
            return OF_FAILURE;
         }
         /*  Trying to find out, whether the Node is child of Previous element */
         if (bDMTplTraverseDone == FALSE)
         {
            if ((DMNodeAttrib == NULL) || (pStackNode->dm_path_p == NULL))
            {
               CM_LDSV_DEBUG_PRINT("DM Node Attrib is null.");
               cm_ldsv_delete_table_stack (pLdsvCfgStackDllQ);
               cm_ldsv_clean_dm_node_attrib_info (DMNodeAttrib);
               cm_ldsv_clean_nv_pair_array (pArrayOfKeys, TRUE);
               cm_ldsv_clean_nv_pair_array (dm_path_pArrayOfKeys, TRUE);
               cm_ldsv_clean_nv_pair_array (pKeysArray, TRUE);
               of_free (pDirPath);
               of_free (pTempDirPath);
               of_free (pDMInstancePath);
               // call xmlhashfree
               if (cm_hash_size (pDictonaryTable) != 0)
               {
                  CM_LDSV_DEBUG_PRINT ("cleaning hash entries.");
                  cm_hash_free (pDictonaryTable, NULL);
               }
               of_free(pRoleFileName);
               of_free(pFileName);
               cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
                     CM_SAVE_FAILED_EVENT, (void*) &uiVersionNo);

                 CM_LDSV_DEBUG_PRINT("Removing Directory:%s", aRmCmd);
               iRet=system(aRmCmd);
               if (iRet != OF_SUCCESS)
               {
                  CM_LDSV_DEBUG_PRINT("Command %s  failed",aRmCmd);
               }
#ifdef CM_AUDIT_SUPPORT
               cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_SAVE_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
                     CM_CMD_SAVE_CONFIG,dm_path_p,NULL, NULL,"DM node Attribute is NULL", uiVersionNo); 
#endif
               return OF_FAILURE;
            }
            CM_LDSV_DEBUG_PRINT (" DMNodeAttrib->dm_path_p::%s,  pStackNode->dm_path_p :%s", DMNodeAttrib->dm_path_p, pStackNode->dm_path_p);
            bIsChildNode = cm_dm_is_child_element (DMNodeAttrib->dm_path_p,
                  pStackNode->dm_path_p,
                  pTransportChannel,
                  CM_LDSV_MGMT_ENGINE_ID,
                  &RoleInfo);
         }
         if ((bDMTplTraverseDone == TRUE) ||
               (bIsChildNode == FALSE)) 
         {
            if (pStackNode == NULL)
            {
               cm_ldsv_delete_table_stack (pLdsvCfgStackDllQ);
               cm_ldsv_clean_dm_node_attrib_info (DMNodeAttrib);
               cm_ldsv_clean_nv_pair_array (pKeysArray, TRUE);
               cm_ldsv_clean_nv_pair_array (dm_path_pArrayOfKeys, TRUE);
               cm_ldsv_clean_nv_pair_array (pArrayOfKeys, TRUE);
               of_free (pDirPath);
               of_free (pTempDirPath);
               of_free (pDMInstancePath);

               // call xmlhashfree
               if (cm_hash_size (pDictonaryTable) != 0)
               {
                  CM_LDSV_DEBUG_PRINT ("cleaning hash entries.");
                  cm_hash_free (pDictonaryTable, NULL);
               }
               of_free(pRoleFileName);
               of_free(pFileName);
               cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
                     CM_SAVE_FAILED_EVENT, (void*) &uiVersionNo);

                 CM_LDSV_DEBUG_PRINT("Removing Directory:%s", aRmCmd);
               iRet=system(aRmCmd);
               if (iRet != OF_SUCCESS)
               {
                  CM_LDSV_DEBUG_PRINT("Command %s  failed",aRmCmd);
               }
#ifdef CM_AUDIT_SUPPORT
               cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_SAVE_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
                     CM_CMD_SAVE_CONFIG,dm_path_p,NULL, NULL,"stack node is NULL", uiVersionNo); 
#endif
               return OF_FAILURE;
            }
            CM_LDSV_DEBUG_PRINT("Both are in different path");
            bTableStackDone = FALSE;
            /* 
               We need to move to next instance of youngest node of
               parent table stack.
               */
            uiRecordCount = 1;
            /* get next instance tree node of the template tree node(VSG, ACl...), 
             * using dm_path_p, we can start traversing */
            CM_LDSV_DEBUG_PRINT ("Calling GetNextN Records for %s",
                  pStackNode->dm_path_p);

            pKeysArray = cm_ldsv_prepare_array_of_keys(pStackNode);
            if (pKeysArray == NULL)
            {
               CM_LDSV_DEBUG_PRINT("Previous keys are null.");
               cm_ldsv_delete_table_stack (pLdsvCfgStackDllQ);
               cm_ldsv_clean_dm_node_attrib_info (DMNodeAttrib);
               cm_ldsv_cleanup_table_stack_node (pStackNode);
               cm_ldsv_clean_nv_pair_array (pSecApplData, TRUE);
               cm_ldsv_clean_nv_pair_array (dm_path_pArrayOfKeys, TRUE);
               cm_ldsv_clean_nv_pair_array (pKeysArray, TRUE);
               of_free (pDirPath);
               of_free (pTempDirPath);
               of_free (pDMInstancePath);

               // call xmlhashfree
               if (cm_hash_size (pDictonaryTable) != 0)
               {
                  CM_LDSV_DEBUG_PRINT ("cleaning hash entries.");
                  cm_hash_free (pDictonaryTable, NULL);
               }
               of_free(pRoleFileName);
               of_free(pFileName);
               cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
                     CM_SAVE_FAILED_EVENT, (void*) &uiVersionNo);

                 CM_LDSV_DEBUG_PRINT("Removing Directory:%s", aRmCmd);
               iRet=system(aRmCmd);
               if (iRet != OF_SUCCESS)
               {
                  CM_LDSV_DEBUG_PRINT("Command %s  failed",aRmCmd);
               }
#ifdef CM_AUDIT_SUPPORT
               cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_SAVE_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
                     CM_CMD_SAVE_CONFIG,dm_path_p,NULL, NULL,"keys array is NULL", uiVersionNo); 
#endif
               return OF_FAILURE;
            }
            iInstRet =
               cm_get_next_n_records (pTransportChannel, CM_LDSV_MGMT_ENGINE_ID,
                     CM_LDSV_ENG_ADMIN_ROLE, pStackNode->dm_path_p,
                     pKeysArray, pArrayOfKeys,
                     &uiRecordCount, &pSecApplData);
            if (iInstRet == OF_FAILURE)
            {
               CM_LDSV_DEBUG_PRINT (" GetNextNRecs Failed");
               cm_ldsv_close_open_xml_tags (aFullPath);
               cm_ldsv_prepare_instance_dm_path (pStackNode->pDMNodeAttrib, pDMInstancePath,
                     dm_path_pArrayOfKeys,
                     pLdsvCfgStackDllQ);
               memset (pDirPath, 0, sizeof (pDirPath));

               cm_ldsv_create_directory_path (pDMInstancePath, pDirPath);
               memset(pFileName, 0, sizeof(pFileName));
               strncat(pFileName, pStackNode->pDMNodeAttrib->file_name, strlen(pStackNode->pDMNodeAttrib->file_name));
               strcat (pFileName, ".xml");
               CM_LDSV_CREATE_COMPLETE_FILE_PATH (aFullPath, pDirPath, pFileName, aVersionNo);
               cm_ldsv_close_open_xml_tags (aFullPath);

               /* Time to pop out one element from the table stack */
               CM_DLLQ_DELETE_NODE (pLdsvCfgStackDllQ, pDllqNode);
               cm_ldsv_cleanup_table_stack_node (pStackNode);
               cm_ldsv_clean_nv_pair_array (pSecApplData, TRUE);

               if (CM_DLLQ_COUNT (pLdsvCfgStackDllQ) == 0)
               {
                  CM_LDSV_DEBUG_PRINT (" Stack is EMPTY");
                  if (bDMTplTraverseDone == TRUE)
                  {
                     CM_LDSV_DEBUG_PRINT
                        ("Template tree Traversal completed and Stack is EMPTY");
                  }
                  else
                  {
                     iRet = OF_SUCCESS;
                  }
               }
               else
               {
                  if (CM_DLLQ_COUNT (pLdsvCfgStackDllQ) != 0)
                  {
                     pDllqNode = CM_DLLQ_LAST (pLdsvCfgStackDllQ);
                     pStackNode =
                        CM_DLLQ_LIST_MEMBER (pDllqNode, cm_ldsv_table_stack_node_t *, ListNode);
                     if (pStackNode == NULL)
                     {
                        cm_ldsv_delete_table_stack (pLdsvCfgStackDllQ);
                        cm_ldsv_clean_dm_node_attrib_info (DMNodeAttrib);
                        cm_ldsv_clean_nv_pair_array (pArrayOfKeys, TRUE);
                        cm_ldsv_clean_nv_pair_array (pKeysArray, TRUE);
                        cm_ldsv_clean_nv_pair_array (dm_path_pArrayOfKeys, TRUE);
                        of_free (pDirPath);
                        of_free (pTempDirPath);
                        of_free (pDMInstancePath);

                        // call xmlhashfree
                        if (cm_hash_size (pDictonaryTable) != 0)
                        {
                           CM_LDSV_DEBUG_PRINT ("cleaning hash entries.");
                           cm_hash_free (pDictonaryTable, NULL);
                        }
                        of_free(pRoleFileName);
                        of_free(pFileName);
                        cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel,
                              CM_SAVE_FAILED_EVENT, (void*) &uiVersionNo);

                 CM_LDSV_DEBUG_PRINT("Removing Directory:%s", aRmCmd);
                        iRet=system(aRmCmd);
                        if (iRet != OF_SUCCESS)
                        {
                           CM_LDSV_DEBUG_PRINT("Command %s  failed",aRmCmd);
                        }
#ifdef CM_AUDIT_SUPPORT
                        cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_SAVE_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
                              CM_CMD_SAVE_CONFIG,dm_path_p,NULL, NULL,"stack node is NULL", uiVersionNo); 
#endif
                        return OF_FAILURE;
                     } 
                     bIsChildNode = cm_dm_is_child_element (DMNodeAttrib->dm_path_p,
                           pStackNode->dm_path_p,
                           pTransportChannel,
                           CM_LDSV_MGMT_ENGINE_ID,
                           &RoleInfo);
                     if(bIsChildNode == FALSE)
                     {
                        pKeysArray = cm_ldsv_prepare_array_of_keys(pStackNode);
                        pArrayOfKeys = cm_ldsv_collect_keys_from_table_stack (pLdsvCfgStackDllQ,
                              dm_path_pArrayOfKeys);
                        if ((pKeysArray == NULL) || (pArrayOfKeys == NULL))
                        {
                           CM_LDSV_DEBUG_PRINT("Previous keys are null.");
                           cm_ldsv_delete_table_stack (pLdsvCfgStackDllQ);
                           cm_ldsv_clean_dm_node_attrib_info (DMNodeAttrib);
                           cm_ldsv_cleanup_table_stack_node (pStackNode);
                           cm_ldsv_clean_nv_pair_array (pSecApplData, TRUE);
                           cm_ldsv_clean_nv_pair_array (dm_path_pArrayOfKeys, TRUE);
                           cm_ldsv_clean_nv_pair_array (pKeysArray, TRUE);
                           of_free (pDirPath);
                           of_free (pTempDirPath);
                           of_free (pDMInstancePath);

                           // call xmlhashfree
                           if (cm_hash_size (pDictonaryTable) != 0)
                           {
                              CM_LDSV_DEBUG_PRINT ("cleaning hash entries.");
                              cm_hash_free (pDictonaryTable, NULL);
                           }
                           of_free(pRoleFileName);
                           of_free(pFileName);
                           cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
                                 CM_SAVE_FAILED_EVENT, (void*) &uiVersionNo);

                 CM_LDSV_DEBUG_PRINT("Removing Directory:%s", aRmCmd);
                           iRet=system(aRmCmd);
                           if (iRet != OF_SUCCESS)
                           {
                              CM_LDSV_DEBUG_PRINT("Command %s  failed",aRmCmd);
                           }
#ifdef CM_AUDIT_SUPPORT
                           cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_SAVE_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
                                 CM_CMD_SAVE_CONFIG,dm_path_p,NULL, NULL,"keys array is NULL", uiVersionNo); 
#endif
                           return OF_FAILURE;
                        }
                        CM_LDSV_DEBUG_PRINT (" Both are different Path");
                        CM_LDSV_DEBUG_PRINT (" Calling get Next N for ::%s:", pStackNode->dm_path_p);
                        iInstRet =
                           cm_get_next_n_records(pTransportChannel, CM_LDSV_MGMT_ENGINE_ID,
                                 CM_LDSV_ENG_ADMIN_ROLE, pStackNode->dm_path_p,
                                 pKeysArray, pArrayOfKeys,
                                 &uiRecordCount, &pSecApplData);
                        if (iInstRet == OF_FAILURE)
                        {
                           cm_ldsv_close_open_xml_tags (aFullPath);
                           cm_ldsv_prepare_instance_dm_path (pStackNode->pDMNodeAttrib, pDMInstancePath,
                                 dm_path_pArrayOfKeys,
                                 pLdsvCfgStackDllQ);
                           memset (pDirPath, 0, sizeof (pDirPath));

                           cm_ldsv_create_directory_path (pDMInstancePath, pDirPath);
                           memset(pFileName, 0, sizeof(pFileName));
                           strncat(pFileName, pStackNode->pDMNodeAttrib->file_name, strlen(pStackNode->pDMNodeAttrib->file_name));
                           strcat (pFileName, ".xml");
                           CM_LDSV_CREATE_COMPLETE_FILE_PATH (aFullPath, pDirPath, pFileName, aVersionNo);
                           cm_ldsv_close_open_xml_tags (aFullPath);

                           CM_DLLQ_DELETE_NODE (pLdsvCfgStackDllQ, pDllqNode);
                           cm_ldsv_cleanup_table_stack_node (pStackNode);
                        }
                        else
                        {
                           CM_LDSV_DEBUG_PRINT (" GetNextN Success:%s:", pStackNode->dm_path_p);
                        }
                     }
                  }
                  /* Get keys information from the stack */
                  pArrayOfKeys =
                     cm_ldsv_collect_keys_from_table_stack (pLdsvCfgStackDllQ, 
                           dm_path_pArrayOfKeys);
                  if (pArrayOfKeys == NULL)
                  {
                     CM_LDSV_DEBUG_PRINT ("Array of Keys Null");
                     /* Clean DM template tree and LDSV runtime data */
                     cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo,
                           uiOpaqueInfoLen);
                     cm_ldsv_clean_dm_node_attrib_info (DMNodeAttrib);
                     cm_ldsv_delete_table_stack (pLdsvCfgStackDllQ);
                     of_free (pDirPath);
                     of_free (pTempDirPath);
                     of_free (pDMInstancePath);
                     // call xmlhashfree
                     if (cm_hash_size (pDictonaryTable) != 0)
                     {
                        CM_LDSV_DEBUG_PRINT ("cleaning hash entries.");
                        cm_hash_free (pDictonaryTable, NULL);
                     }
                     of_free(pRoleFileName);
                     of_free(pFileName);
                     of_free(pDMFileName);
                     cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
                           CM_SAVE_FAILED_EVENT, (void*) &uiVersionNo);

                 CM_LDSV_DEBUG_PRINT("Removing Directory:%s", aRmCmd);
                     iRet=system(aRmCmd);
                     if (iRet != OF_SUCCESS)
                     {
                        CM_LDSV_DEBUG_PRINT("Command %s  failed",aRmCmd);
                     }
#ifdef CM_AUDIT_SUPPORT
                     cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_SAVE_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
                           CM_CMD_SAVE_CONFIG,dm_path_p,NULL, NULL,"keys array is NULL", uiVersionNo); 
#endif
                     return OF_FAILURE;
                  }
               }
               // This is Return Value of GetFirstNode/GetNextNode
               iRet = OF_SUCCESS;
            }

            /* trying to findout whether temporary stack information pushed
             * to the persistent store or not, after pushing successfully
             * removing from the stack,Since we are completed one instance */
            if (iInstRet == OF_SUCCESS)
            {
               cm_ldsv_prepare_instance_dm_path (pStackNode->pDMNodeAttrib, pDMInstancePath,
                     dm_path_pArrayOfKeys,
                     pLdsvCfgStackDllQ);
               memset (pDirPath, 0, sizeof (pDirPath));

               cm_ldsv_create_directory_path (pDMInstancePath, pDirPath);
               memset(pFileName, 0, sizeof(pFileName));
               if (((pStackNode->pDMNodeAttrib->file_name) == NULL) || strlen(pStackNode->pDMNodeAttrib->file_name) == 0)
               {
                  cm_dm_get_file_name(pTransportChannel, CM_LDSV_MGMT_ENGINE_ID,
                        pStackNode->pDMNodeAttrib->dm_path_p, RoleInfo.admin_role, &pDMFileName);
                  {
                     if ((pDMFileName == NULL) || (strlen(pDMFileName) == 0))
                     {
                        CM_LDSV_DEBUG_PRINT("File name is null, so can't proceed for saving");
                        break;
                     }
                     strncat(pFileName, pDMFileName, strlen(pDMFileName)+1);
                  }
               }
               else
               {
                  strncat(pFileName, pStackNode->pDMNodeAttrib->file_name, strlen(pStackNode->pDMNodeAttrib->file_name)+1);
               }
               strcat (pFileName, ".xml");

               CM_LDSV_CREATE_COMPLETE_FILE_PATH (aFullPath, pDirPath, pFileName, aVersionNo);
               //CM_LDSV_CREATE_COMPLETE_DIR_PATH (pTempDirPath, pDirPath);

               /* Write this record to LDSV-xml File */
               cm_ldsv_write_to_file (pSecApplData, pDictonaryTable, aFullPath, 
                     pStackNode->pDMNodeAttrib, pTransportChannel);
               /*
                  Now we need to start traversing the entire child tree for
                  this new instance. Replace the old instance in TableStack
                  with this new instance info.
                  */
               pKeyNvPair =
                  cm_ldsv_find_key_nv_pair (pStackNode->pDMNodeAttrib, pSecApplData,
                        pTransportChannel);
               if (unlikely (pKeyNvPair == NULL))
               {
                  /* Clean Data model template tree traversal runtime data */
                  cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo, uiOpaqueInfoLen);
                  cm_ldsv_clean_dm_node_attrib_info (DMNodeAttrib);
                  /* Clean LDSV runtime data */
                  cm_ldsv_delete_table_stack (pLdsvCfgStackDllQ);
                  cm_ldsv_clean_nv_pair_array (pArrayOfKeys, TRUE);
                  cm_ldsv_clean_nv_pair_array (dm_path_pArrayOfKeys, TRUE);
                  cm_ldsv_clean_nv_pair_array (pKeysArray, TRUE);
                  cm_ldsv_clean_array_of_nv_pair_array (pSecApplData, uiRecordCount);
                  of_free (pDirPath);
                  of_free (pTempDirPath);
                  of_free (pDMInstancePath);
                  // call xmlhashfree
                  if (cm_hash_size (pDictonaryTable) != 0)
                  {
                     CM_LDSV_DEBUG_PRINT ("cleaning hash entries.");
                     cm_hash_free (pDictonaryTable, NULL);
                  }
                  of_free(pRoleFileName);
                  of_free(pFileName);
                  cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
                        CM_SAVE_FAILED_EVENT, (void*) &uiVersionNo);

                 CM_LDSV_DEBUG_PRINT("Removing Directory:%s", aRmCmd);
                  iRet=system(aRmCmd);
                  if (iRet != OF_SUCCESS)
                  {
                     CM_LDSV_DEBUG_PRINT("Command %s  failed",aRmCmd);
                  }
#ifdef CM_AUDIT_SUPPORT
                  cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_SAVE_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
                        CM_CMD_SAVE_CONFIG,dm_path_p,NULL, NULL,"keys array is NULL", uiVersionNo); 
#endif
                  return OF_FAILURE;
               }
#ifdef CM_ROLES_PERM_SUPPORT
               memset(aDmPath, 0, sizeof(aDmPath));
               strcpy (aDmPath, pDMInstancePath);
               strncat (aDmPath, "{", 1);
               strncat (aDmPath, pKeyNvPair->NvPairs[0].value_p,
                     pKeyNvPair->NvPairs[0].value_length);
               strncat (aDmPath, "}", 1);

               CM_LDSV_CREATE_COMPLETE_FILE_PATH (aRoleFullPath, pDirPath, pRoleFileName, aVersionNo);
               UCMDM_GetInstanceRolePermissions (pTransportChannel,
                     CM_LDSV_MGMT_ENGINE_ID, aDmPath,
                     CM_VORTIQA_SUPERUSER_ROLE, &pRolePermAry);

               if (pRolePermAry != NULL)
               {
                  cm_write_role_permissions_info(pRolePermAry, aRoleFullPath, aDmPath);
                  memset(aDmPath, 0, sizeof(aDmPath));
                  strcpy (aDmPath, pDMInstancePath);
               }
#endif
               /* Push this table instance to stack
                * Stack contains instace key value combinations*/
               if (cm_ldsv_update_table_stack_node_key_nv_pair (pStackNode,
                        pKeyNvPair) == OF_FAILURE)
               {
                  /* Clean Data model template tree traversal runtime data, since unable to
                   * push table instance into stack */
                  cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo, uiOpaqueInfoLen);
                  cm_ldsv_clean_dm_node_attrib_info (DMNodeAttrib);

                  /* Clean LDSV runtime data */
                  cm_ldsv_delete_table_stack (pLdsvCfgStackDllQ);
                  cm_ldsv_clean_nv_pair_array (pArrayOfKeys, TRUE);
                  cm_ldsv_clean_nv_pair_array (dm_path_pArrayOfKeys, TRUE);
                  cm_ldsv_clean_nv_pair_array (pKeysArray, TRUE);
                  cm_ldsv_clean_array_of_nv_pair_array (pSecApplData, uiRecordCount);
                  of_free (pDirPath);
                  of_free (pTempDirPath);
                  of_free (pDMInstancePath);
                  // call xmlhashfree
                  if (cm_hash_size (pDictonaryTable) != 0)
                  {
                     CM_LDSV_DEBUG_PRINT ("cleaning hash entries.");
                     cm_hash_free (pDictonaryTable, NULL);
                  }
                  of_free(pRoleFileName);
                  of_free(pFileName);
                  of_free(pDMFileName);
                  cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
                        CM_SAVE_FAILED_EVENT, (void*) &uiVersionNo);

                 CM_LDSV_DEBUG_PRINT("Removing Directory:%s", aRmCmd);
                  iRet=system(aRmCmd);
                  if (iRet != OF_SUCCESS)
                  {
                     CM_LDSV_DEBUG_PRINT("Command %s  failed",aRmCmd);
                  }
#ifdef CM_AUDIT_SUPPORT
                  cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_SAVE_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
                        CM_CMD_SAVE_CONFIG,dm_path_p,NULL, NULL,"keys array is NULL", uiVersionNo); 
#endif
                  return OF_FAILURE;
               }

               /* Now Cleanup the last Record data */
               cm_ldsv_clean_array_of_nv_pair_array (pSecApplData, uiRecordCount);
               pSecApplData = NULL;

               /* 
                  As we need to traverse the entire child tree from here 
                  for the new table instance, We need to get the first child
                  of this table node again. So use the OpaqueInfo from the
                  most recent TableStackNode, instead of recent opaque info.
                  */

               if (pOpaqueInfo)
               {
                  /* clean opaque information */
                  cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo, uiOpaqueInfoLen);
               }
               pOpaqueInfo =
                  (void *) of_calloc (pStackNode->uiOpaqueInfoLen,
                        sizeof (uint32_t));
               if (unlikely (pOpaqueInfo == NULL))
               {
                  /* Cleanup everything, and quit from here */
                  cm_ldsv_clean_dm_node_attrib_info (DMNodeAttrib);
                  cm_ldsv_delete_table_stack (pLdsvCfgStackDllQ);
                  cm_ldsv_clean_nv_pair_array (pArrayOfKeys, TRUE);
                  cm_ldsv_clean_nv_pair_array (pKeysArray, TRUE);
                  cm_ldsv_clean_nv_pair_array (dm_path_pArrayOfKeys, TRUE);
                  of_free (pDirPath);
                  of_free (pTempDirPath);
                  of_free (pDMInstancePath);
                  // call xmlhashfree
                  if (cm_hash_size (pDictonaryTable) != 0)
                  {
                     CM_LDSV_DEBUG_PRINT ("cleaning hash entries.");
                     cm_hash_free (pDictonaryTable, NULL);
                  }
                  of_free(pRoleFileName);
                  of_free(pFileName);
                  of_free(pDMFileName);
                  cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
                        CM_SAVE_FAILED_EVENT, (void*) &uiVersionNo);

                 CM_LDSV_DEBUG_PRINT("Removing Directory:%s", aRmCmd);
                  iRet=system(aRmCmd);
                  if (iRet != OF_SUCCESS)
                  {
                     CM_LDSV_DEBUG_PRINT("Command %s  failed",aRmCmd);
                  }
#ifdef CM_AUDIT_SUPPORT
                  cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_SAVE_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
                        CM_CMD_SAVE_CONFIG,dm_path_p,NULL, NULL,"memory allocation failed for opaqueinfo", uiVersionNo); 
#endif
                  return OF_FAILURE;
               }

               /* each instance of the tree, Firewall Node will be 
                * repeated for VSG1, VSG2 etc..Traversing from VSG 
                * to multiple Firewall nodes we need to know which 
                * instances of firewall are traversed and which one 
                * is pending. For resolving this issue we are maintining 
                * opaque information */
               memcpy (pOpaqueInfo, pStackNode->pOpaqueInfo,
                     (sizeof (uint32_t) * pStackNode->uiOpaqueInfoLen));
               uiOpaqueInfoLen = pStackNode->uiOpaqueInfoLen;

               /* Get the next node for the Parent Table Node again
                * This gives us template tree table next node */
               iRet = cm_dm_get_next_node_info (pStackNode->dm_path_p,
                     UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
                     pTransportChannel,
                     CM_LDSV_MGMT_ENGINE_ID,
                     &RoleInfo,
                     &DMNodeAttrib,
                     &pOpaqueInfo, &uiOpaqueInfoLen);
               if (iRet != OF_SUCCESS)
               {
                  memset(aFullPath, 0, sizeof(aFullPath));
                  CM_LDSV_CREATE_COMPLETE_FILE_PATH (aFullPath, "", CM_CONFIGSYNC_CONFIG_VER_FILE, "");
                  if(cm_ldsv_write_version_number_to_file(aFullPath, uiVersionNo) != OF_SUCCESS)
                  {
                     CM_LDSV_DEBUG_PRINT("Write version no into file is failed.");
                  }
                  /* Clean up runtime data and return Success message */
                  cm_ldsv_clean_dm_node_attrib_info (DMNodeAttrib);
                  cm_ldsv_clean_nv_pair_array (pArrayOfKeys, FALSE);
                  cm_ldsv_clean_nv_pair_array (pKeysArray, TRUE);
                  cm_ldsv_clean_nv_pair_array (dm_path_pArrayOfKeys, TRUE);
                  cm_ldsv_delete_table_stack (pLdsvCfgStackDllQ);
                  of_free (pDirPath);
                  of_free (pTempDirPath);
                  of_free (pDMInstancePath);
                  // call xmlhashfree
                  CM_LDSV_DEBUG_PRINT ("cleaning hash entries.");
                  cm_hash_free (pDictonaryTable, NULL);
                  of_free(pDMFileName);

                  memset(aFullPath, 0, sizeof(aFullPath));
                  CM_LDSV_CREATE_COMPLETE_FILE_PATH (aFullPath, "", CM_CONFIGSYNC_CONFIG_VER_FILE, "");
                  if(cm_ldsv_write_version_number_to_file(aFullPath, uiVersionNo) != OF_SUCCESS)
                  {
                     CM_LDSV_DEBUG_PRINT("Write version no into file is failed.");
                  }
                  cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
                        CM_SAVE_COMPLETED_EVENT, (void*) &uiVersionNo);
#ifdef CM_AUDIT_SUPPORT
                  cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_SAVE_SUCCESS, RoleInfo.aAdminName, RoleInfo.admin_role,
                        CM_CMD_SAVE_CONFIG,dm_path_p,NULL, NULL, NULL, uiVersionNo); 
#endif
                  return OF_SUCCESS;
               }
               bDMTplTraverseDone = FALSE;
               CM_LDSV_DEBUG_PRINT ("GetNext Node= %s:", DMNodeAttrib->dm_path_p);

            }                       /* if (TableStack is NOT done) */

         }                         /* if (Node is NOT child of Recent Table Stack Entry) */
      }                           /* if (TableStack is NOT empty) */

      /* We will build the Keys fresh every time */
      if (pArrayOfKeys)
         cm_ldsv_clean_nv_pair_array (pArrayOfKeys, TRUE);
      /*-----------------------------------------------------------*/

   }                             /* DM Tree Traversal While Loop */

   memset(aFullPath, 0, sizeof(aFullPath));
   CM_LDSV_CREATE_COMPLETE_FILE_PATH (aFullPath, "", CM_CONFIGSYNC_CONFIG_VER_FILE, "");
   if(cm_ldsv_write_version_number_to_file(aFullPath, uiVersionNo) != OF_SUCCESS)
   {
      CM_LDSV_DEBUG_PRINT("Write version no into file is failed.");
   }
   cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
         CM_SAVE_COMPLETED_EVENT, (void*) &uiVersionNo);
#ifdef CM_AUDIT_SUPPORT
   cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_SAVE_SUCCESS, RoleInfo.aAdminName, RoleInfo.admin_role,
         CM_CMD_SAVE_CONFIG,dm_path_p,NULL, NULL, NULL, uiVersionNo); 
#endif
   return OF_SUCCESS;
}

/**
  \ingroup LDSVAPI
  This API is used to set configuration into factory defaults. Factory defaults operation is
  initiated by UCM JE engine.

  This API is executed by LDSV daemon when it receives Factory Reset on the LDSV main
  socket. As data model path is supplied as one of the input parameter, it helps LDSV daemon 
  to read the configuration elements and their configuration. 
  The result of the factory reset operation is encoded into generic Transport channel message format and sends on the given TCP socket Fd.

  <b>Input paramameters: </b>\n
  <b><i> dm_path_p: </i></b> Pointer to Configuration Data Model Path to reset factory defaults
  The complete path of the head of the data model node for which factory reset operation need to be executed. All the child node configurations also get configured to factory defaults.
  <b>Output Parameters: </b>
  <b>Return Value:</b> OF_SUCCESS in Success OF_FAILURE in Failure case.\n
  */

/**************************************************************************
  Function Name :cm_ldsv_factory_reset
  Input Args    :dm_path_p - Data model Path, using this path we can traverse dm tree
  Output Args   :
Description   :This method is used to reset configuration data into default 
Cofiguration.
Return Values :OF_SUCCESS/OF_FAILURE
 *************************************************************************/

int32_t cm_factory_reset (char * dm_path_p)
{
   int32_t iRet;

   /*
    * Call load function of ldsv
    */
   iRet = cm_load_config(dm_path_p, TRUE);
   if (iRet == OF_FAILURE)
   {
      CM_LDSV_DEBUG_PRINT("Failed to load default configuration files.");
      return iRet;
   }
   return OF_SUCCESS;
}

/**
  \ingroup LDSVAPI
  This API is be used by UCM JE process to send a configuration load request to LDSV daemon. 
  As data model path is supplied as one of the input parameter, it helps LDSV daemon to read the 
  configuration elements and locate their data in Saved configuration file.

  The result of the Load command will be sent on same TCP socket. So to receive the result of the   
  Load command, caller has to poll for response on the same transport channel that is used to send 
  the load request.
  Sample saved configuration file is shown below,

  <configuration>
	<header>
		<filename>vsg.xml</filename>
		<description>VSG Table</description>
		<version>1</version>
		<dmpath>igd.vsg</dmpath>
		<instancepath>igd.vsg</instancepath>
	</header>
  <body>
	<record name='vsg' type='1' relative_dmpath='vsg'/>
	<dictionary>
		<a0>vsgid  type=2</a0>
		<a1>nsid  type=2</a1>
		<a2>vsgname  type=1</a2>
	</dictionary>
		<row>
			<a0>0</a0>
			<a1>0</a1>
			<a2>general</a2>
		</row>
  </body>
  </configuration>

  <b>Input paramameters: </b>\n
  <b><i> dm_path_p: </i></b> Pointer to Configuration Data Model Path to Load Configuration
  The complete path of the head of the data model node for which Load Configuration need to be 
  executed. All the child node configurations also gets Loaded.
  <b>Output Parameters: </b>
  <b>Return Value:</b> OF_SUCCESS in Success OF_FAILURE in Failure case.\n
  */
/**************************************************************************
  Function Name :cm_ldsv_load_config
  Input Args    :dm_path_p - Data model Path, using this path we can traverse dm tree
  Output Args   :
Description   :This method is used to load the configuration data.
Return Values :OF_SUCCESS/OF_FAILURE
 *************************************************************************/
/* 
 * file directory organization as like as our instance tree as shown below.
 *         igd
 *         +----------------------------------+
 *         |                                  |
 *         vsg (Table : VSG1, VSG2)           SysInfo
 *         |
 *         +----------+
 *       Firewall    NetObj
 *         |         |
 *       Acl.xml    Ipdb.xml
 *
 * Each istance is treated as a seperate directory. Ex: VSG1, VSG2....
 * Under VSG1, Firewall, NetObj, IPS, IPSec directories are there.
 * Under Firewall Directory Acl.xml and Under NetObj directory Ipdb.xml 
 * files are stored.
 * For navigating different directories in a proper order, we are traversing
 * template tree as like save logic. 
 *
 1. call --> cm_dm_get_first_node_info(...)
 Got "igd".
 2. call --> cm_dm_get_next_node_info(...)
 got "vsg".

 as it is table node, prepare instance path.
 2.1 call ---> cm_ldsv_prepare_instance_dm_path(...)
 this routine will fetch instance from table stack 
 and prepare instance path for navigating directories like,
 Initially stack node is null, it returns
 igd.vsg
 2.2 call --> cm_ldsv_create_directory_path(...) 
 from DMNode Attributes, we will get file name as Vsg.xml.
 cm_ldsv_create_directory_path returns igd/, concate file name with directory,
 ige/Vsg.xml, load igd/Vsg.xml file. Vsg.xml file has divided into Header, Dictionary and Data Part.
 Tokenize Header, Dictionary and Data part and form name value pairs and send it to JE
 as add command. Add vsg information as VsgId, VsgName, opaque information, DMPath to stack node.
 3. call --> cm_dm_get_next_node_info(...)
 got "NetObj".
 3.1 call ---> cm_ldsv_prepare_instance_dm_path(...)
 igd.vsg{VSG1}.NetObj
 3.2 call --> cm_ldsv_create_directory_path(...) 
 igd/VSG1/NetObj/Ipdb.xml
 load Ipdb.xm file, add instances into stack.

 4. call --> cm_dm_get_next_node_info(...)
 got "Firewall"
 4.1 call ---> cm_ldsv_prepare_instance_dm_path(...)
 igd.vsg{VSG1}.Firewall
 4.2 call --> cm_ldsv_create_directory_path(...)        
 igd/VSG1/Firewall/Acl.xml
 load Acl.xml file and instances into stack.

 Delete one instacne from the stack after one iteration of traversing templete tree has been over.
 For start traversing from next instance, get stack node from the stack and use opaque information 
 and DMPath stored from the stack. 
 Repeat the above process untill table stack becomes null.

 Saved configuration file is in the following format,
  <configuration>
	<header>
		<filename>vsg.xml</filename>
		<description>VSG Table</description>
		<version>1</version>
		<dmpath>igd.vsg</dmpath>
		<instancepath>igd.vsg</instancepath>
	</header>
  <body>
	<record name='vsg' type='1' relative_dmpath='vsg'/>
	<dictionary>
		<a0>vsgid  type=2</a0>
		<a1>nsid  type=2</a1>
		<a2>vsgname  type=1</a2>
	</dictionary>
		<row>
			<a0>0</a0>
			<a1>0</a1>
			<a2>general</a2>
		</row>
  </body>
  </configuration>
 */    


int32_t cm_load_config (char * dm_path_p, unsigned char bFactResetFlag)
{
   char *pDirPath;
   int32_t iRet;
   void *pTransportChannel = NULL;
   char *pFileName = NULL;  //CID 553
   char *pRoleFileName;
   struct cm_role_info RoleInfo = { "VortiQa", CM_VORTIQA_SUPERUSER_ROLE };
   struct cm_dm_node_info *pDMNdAttrib = NULL, *pPrevDMNdAttrib=NULL;
   uint32_t uiOpaqueInfoLen = 0;
   void *pOpaqueInfo = NULL;
   UCMDllQ_t LoadTableStackNode;
   UCMDllQ_t *pLoadCfgStackDllQ = &LoadTableStackNode;
   struct cm_array_of_nv_pair *pKeyNvPair = NULL;
   char aDMInstancePath[LDSV_DMPATH_SIZE];
   char aFullPath[LDSV_DMPATH_SIZE];
   char *pTempDirPath = NULL;
   struct cm_array_of_nv_pair *dm_path_pLoadArrayOfKeys = NULL;
   FILE *pFile;
   DIR *pDir;
   uint64_t ullSavedConfigVer;
   unsigned char bMultiInstance=FALSE;
   cm_ldsv_table_stack_node_t *pStackNode;
   UCMDllQNode_t *pDllqNode;
   unsigned char bIsChildNode = TRUE;

   if ((pDir = opendir(PERSISTENT_TRANSFERLIST_DEFAULT_DIR)) == NULL)
   {
      CM_LDSV_DEBUG_PRINT("Default directory doesn't exist.");
      return OF_FAILURE;
   }
   if (closedir(pDir) == -1)
   {
      CM_LDSV_DEBUG_PRINT("Failed to close opened directory.");
   }

   /*initializing double linked list */
   CM_DLLQ_INIT_LIST (&LoadTableStackNode);

   /* Get the existing Transport Channel with JE */
   pTransportChannel = cm_ldsv_get_transport_channel();
   /* Validate transport channel */
   if (unlikely (pTransportChannel == NULL))
   {
      pTransportChannel = cm_tnsprt_create_channel (CM_IPPROTO_TCP, 0, 0,
            UCMJE_CONFIGREQUEST_PORT);
   }

   CM_LDSV_DEBUG_PRINT ("DMPath= %s",dm_path_p);
   if (dm_path_p != NULL)
   {
      if (cm_dm_get_keys_array_from_name_path (pTransportChannel,
               dm_path_p,
               CM_LDSV_MGMT_ENGINE_ID,
               &RoleInfo,
               &dm_path_pLoadArrayOfKeys) != OF_SUCCESS)
      {
         CM_LDSV_DEBUG_PRINT ("Get Keys Array Failed");
         return OF_FAILURE;
      }
   }

   iRet = cm_dm_get_first_node_info (dm_path_p,
         UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
         pTransportChannel,
         CM_LDSV_MGMT_ENGINE_ID,
         &RoleInfo,
         &pDMNdAttrib, &pOpaqueInfo, &uiOpaqueInfoLen);
   if ((pDMNdAttrib == NULL) || (iRet == OF_FAILURE))
   {
      CM_LDSV_DEBUG_PRINT ("GetFirstNodeInfo failed.");
      return OF_FAILURE;
   }
   CM_LDSV_DEBUG_PRINT ("DMPath= %s, DMNode=%s, %s",dm_path_p,pDMNdAttrib->name_p,pDMNdAttrib->dm_path_p);
   pDirPath = (char *) of_calloc (1, LDSV_DMPATH_SIZE);
   if (pDirPath == NULL)
   {
      CM_LDSV_DEBUG_PRINT ("Failed to create memory.");
      return OF_FAILURE;
   }
   pTempDirPath = (char *) of_calloc (1, LDSV_DMPATH_SIZE);
   if (pTempDirPath == NULL)
   {
      CM_LDSV_DEBUG_PRINT ("Failed to create memory.");
      of_free(pDirPath);
      return OF_FAILURE;
   }
   pRoleFileName = (char *) of_calloc (1, CM_SAVE_FILE_NAME_MAX_LEN);
   if (pRoleFileName == NULL)
   {
      CM_LDSV_DEBUG_PRINT ("Failed to create memory forRole File Name.");
      of_free(pDirPath);
      of_free(pTempDirPath);
      return OF_FAILURE;
   }
   memset(aFullPath, 0, sizeof(aFullPath));
   CM_LDSV_CREATE_COMPLETE_FILE_PATH (aFullPath, "", CM_CONFIGSYNC_CONFIG_VER_FILE, "");
   if (bFactResetFlag == TRUE)
   {
      ullSavedConfigVer=ulVersionNo_g;
   }
   else
   {
      if ((pFile = fopen (aFullPath, "r")) != NULL)
      {
         memset(aCurrLoadingVersionNo_g, 0, sizeof(aCurrLoadingVersionNo_g));
         cm_ldsv_get_version_number_from_file(pFile, aCurrLoadingVersionNo_g, &ullSavedConfigVer);
         //cm_ldsv_get_version_number_from_file(pFile, aCurrLoadingVersionNo_g); 
      }
   }
   CM_LDSV_DEBUG_PRINT ("aFullPath=%s.",aFullPath);
   /*if((sscanf(aCurrLoadingVersionNo_g, "%lu.%lu",&uiSavedEngConfigVer, &uiSavedConfigVer))< 2)
     {
     CM_LDSV_DEBUG_PRINT("\n Failed to transform the value.");
     return OF_FAILURE;
     }
     ullSavedEngCfgVer  =  uiSavedEngConfigVer;
     ullSavedConfigVer =  ullSavedEngCfgVer<<32 |uiSavedConfigVer;*/
   //printf("%s.%d: SavedConfigVer=%llu. eng configver=%lu, config ver=%lu:\n",__FUNCTION__,__LINE__,ullSavedConfigVer,uiSavedEngConfigVer,uiSavedConfigVer);
   cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
         CM_LOAD_STARTED_EVENT, (void*) &ullSavedConfigVer);
   while (iRet == OF_SUCCESS)
   {
      if (pDMNdAttrib == NULL)
      {
#ifdef CM_AUDIT_SUPPORT
         cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_LOAD_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
               CM_CMD_LOAD_CONFIG,dm_path_p,NULL, NULL,"DM Attirb is NULL", ullSavedConfigVer); 
#endif
         cm_ldsv_clean_dm_node_attrib_info (pDMNdAttrib);
         cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo, 
               uiOpaqueInfoLen);
         cm_ldsv_delete_table_stack (pLoadCfgStackDllQ);
         of_free(pDirPath);
         of_free(pTempDirPath);
         of_free(pFileName);
         of_free(pRoleFileName);
         CM_LDSV_DEBUG_PRINT("Done loading.");
         cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
               CM_LOAD_FAILED_EVENT, (void*)&ullSavedConfigVer);
         return OF_FAILURE;
      }
      if (pDMNdAttrib->element_attrib.parent_trans_b == FALSE)
      {
         switch (pDMNdAttrib->element_attrib.element_type)
         {
            case CM_DMNODE_TYPE_TABLE:
               memset (pDirPath, 0, sizeof (pDirPath));
               memset(aDMInstancePath, 0, sizeof(aDMInstancePath));
               cm_ldsv_prepare_load_instance_dm_path (pDMNdAttrib, aDMInstancePath,
                     dm_path_pLoadArrayOfKeys,
                     pLoadCfgStackDllQ,NULL);
               cm_ldsv_create_directory_path (aDMInstancePath, pDirPath);
               CM_LDSV_DEBUG_PRINT ("bFactResetFlag=%d,DMInstancePath= %s, DirPath=%s",bFactResetFlag,aDMInstancePath, pDirPath);
               pFileName = (char *) of_calloc (1, CM_SAVE_FILE_NAME_MAX_LEN);
               if (pFileName == NULL)
               {
                  CM_LDSV_DEBUG_PRINT ("Failed to create memory for File Name.");
#ifdef CM_AUDIT_SUPPORT
                  cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_LOAD_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
                        CM_CMD_LOAD_CONFIG,pDMNdAttrib->dm_path_p,NULL, NULL,"Memory allocation Failed", ullSavedConfigVer); 
#endif
                  cm_ldsv_clean_dm_node_attrib_info (pDMNdAttrib);
                  cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo, 
                        uiOpaqueInfoLen);
                  cm_ldsv_delete_table_stack (pLoadCfgStackDllQ);
                  of_free(pDirPath);
                  of_free(pTempDirPath);
                  cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
                        CM_LOAD_FAILED_EVENT, (void*) &ullSavedConfigVer);
                  return OF_FAILURE;
               }

               if (bFactResetFlag == FALSE)  // load config request
               {
                  memset(pFileName, 0, sizeof(pFileName));
                  if ((pDMNdAttrib->file_name) == NULL)
                  {
                     if (cm_dm_get_file_name(pTransportChannel, CM_LDSV_MGMT_ENGINE_ID,
                              pDMNdAttrib->dm_path_p, RoleInfo.admin_role, &pFileName) == OF_FAILURE)
                     {
                        if ((pFileName == NULL) || (strlen(pFileName) < 0))
                        {
                           CM_LDSV_DEBUG_PRINT("File name is null, so can't proceed for saving");
#ifdef CM_AUDIT_SUPPORT
                           cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_LOAD_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
                                 CM_CMD_LOAD_CONFIG,pDMNdAttrib->dm_path_p,NULL, NULL,"Geting File Name Failed", ullSavedConfigVer); 
#endif
                           of_free(pDirPath);
                           of_free(pTempDirPath);
                           cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo, uiOpaqueInfoLen);
                           cm_ldsv_clean_dm_node_attrib_info (pDMNdAttrib);
                           cm_ldsv_delete_table_stack (pLoadCfgStackDllQ);
                           of_free(pRoleFileName);
                           cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
                                 CM_LOAD_FAILED_EVENT, (void*) &ullSavedConfigVer);
                           return OF_FAILURE;
                        }
                     }
                  }
                  else
                  {
                     memcpy(pFileName, pDMNdAttrib->file_name, strlen(pDMNdAttrib->file_name));
                  }
                  strcat (pFileName, ".xml");
                  CM_LDSV_CREATE_COMPLETE_FILE_PATH (aFullPath, pDirPath, pFileName, aCurrLoadingVersionNo_g);
                  CM_LDSV_DEBUG_PRINT ("pFileName= %s,aFullPath=%s,pDirPath=%s.",pFileName,aFullPath,pDirPath);
                  if ((pFile = fopen (aFullPath, "r")) == NULL)
                  {
                     /* loading from config-default directory*/
                     CM_LDSV_CREATE_COMPLETE_DEFAULT_FILE_PATH (aFullPath, pDirPath, pFileName);
                     if ((pFile = fopen (aFullPath, "r")) != NULL)
                     {
                        if (cm_ldsv_load_file (pFile, pDMNdAttrib, 
                                 pKeyNvPair, pOpaqueInfo,
                                 uiOpaqueInfoLen, pLoadCfgStackDllQ) == OF_FAILURE)
                        {
                           CM_LDSV_DEBUG_PRINT("Failed to load configuration");
#ifdef CM_AUDIT_SUPPORT
                           cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_LOAD_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
                                 CM_CMD_LOAD_CONFIG,pDMNdAttrib->dm_path_p,NULL, NULL,"Load File Failed", ullSavedConfigVer); 
#endif
                           cm_ldsv_clean_dm_node_attrib_info (pDMNdAttrib);
                           cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo, 
                                 uiOpaqueInfoLen);
                           cm_ldsv_delete_table_stack (pLoadCfgStackDllQ);
                           of_free(pDirPath);
                           of_free(pTempDirPath);
                           of_free(pFileName);
                           of_free(pRoleFileName);
                           fclose(pFile);
                           cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
                                 CM_LOAD_FAILED_EVENT, (void*) &ullSavedConfigVer);
                           return OF_FAILURE; 
                        }
                        fclose(pFile);

		        CM_SLEEP_AFTER_DATAPATH_LOAD_CONFIG();	
#ifdef CM_ROLES_PERM_SUPPORT
                        memset(pRoleFileName, 0, sizeof(pRoleFileName));
                        strncat(pRoleFileName, pDMNdAttrib->file_name, strlen(pDMNdAttrib->file_name)+1);
                        strcat(pRoleFileName, "Roles.xml");
                        CM_LDSV_CREATE_COMPLETE_DEFAULT_FILE_PATH (aFullPath, pDirPath, pRoleFileName);
                        if ((pFile = fopen(aFullPath, "r")) != NULL)
                        {
                           cm_ldsv_load_roles_and_permission(pFile, pTransportChannel, FALSE);
                           fclose(pFile);
                        }
                        else
                        {
                           CM_LDSV_CREATE_COMPLETE_FILE_PATH (aFullPath, pDirPath, pRoleFileName, aCurrLoadingVersionNo_g);
                           CM_LDSV_DEBUG_PRINT ("pFileName= %s,aFullPath=%s,pDirPath=%s.",pRoleFileName,aFullPath,pDirPath);
                           if ((pFile = fopen(aFullPath, "r")) != NULL)
                           {
                              cm_ldsv_load_roles_and_permission(pFile, pTransportChannel, FALSE);
                              fclose(pFile);
                           }
                        }
#endif
                     }

		  CM_SLEEP_AFTER_DATAPATH_LOAD_CONFIG();	
                  }
                  else  /* loading from config directory.*/
                  {
                     if (cm_ldsv_load_file (pFile, pDMNdAttrib,
                              pKeyNvPair, pOpaqueInfo,
                              uiOpaqueInfoLen, pLoadCfgStackDllQ) == OF_FAILURE)
                     {
                        CM_LDSV_DEBUG_PRINT("Failed to load configuration");
#ifdef CM_AUDIT_SUPPORT
                        cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_LOAD_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
                              CM_CMD_LOAD_CONFIG,pDMNdAttrib->dm_path_p,NULL, NULL,"Load File Failed", ullSavedConfigVer); 
#endif
                        cm_ldsv_clean_dm_node_attrib_info (pDMNdAttrib);
                        cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo,
                              uiOpaqueInfoLen);
                        of_free(pDirPath);
                        of_free(pTempDirPath);
                        of_free(pFileName);
                        of_free(pRoleFileName);
                        fclose(pFile);
                        cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
                              CM_LOAD_FAILED_EVENT, (void*) &ullSavedConfigVer);
                        return OF_FAILURE;
                     }
                     fclose(pFile);
		     CM_SLEEP_AFTER_DATAPATH_LOAD_CONFIG();	
#ifdef CM_ROLES_PERM_SUPPORT
                     memset(pRoleFileName, 0, sizeof(pRoleFileName));
                     strncat(pRoleFileName, pDMNdAttrib->file_name, strlen(pDMNdAttrib->file_name)+1);
                     strcat(pRoleFileName, "Roles.xml");
                     CM_LDSV_CREATE_COMPLETE_FILE_PATH (aFullPath, pDirPath, pRoleFileName, aCurrLoadingVersionNo_g );
                     CM_LDSV_DEBUG_PRINT ("pFileName= %s,aFullPath=%s,pDirPath=%s.",pRoleFileName,aFullPath,pDirPath);
                     if ((pFile = fopen(aFullPath, "r")) != NULL)
                     {
                        cm_ldsv_load_roles_and_permission(pFile, pTransportChannel, FALSE);
                        fclose(pFile);
                     }
                     else
                     {
                        CM_LDSV_CREATE_COMPLETE_DEFAULT_FILE_PATH (aFullPath, pDirPath, pRoleFileName);
                        if ((pFile = fopen(aFullPath, "r")) != NULL)
                        {
                           cm_ldsv_load_roles_and_permission(pFile, pTransportChannel, FALSE);
                           fclose(pFile);
                        }
                     }
#endif
                  }
               }
               else // factory reset request, always load configuration from config-default directoy
               {
                  memset(pFileName, 0, sizeof(pFileName));
                  if ((pDMNdAttrib->file_name) == NULL)
                  {
                     if (cm_dm_get_file_name(pTransportChannel, CM_LDSV_MGMT_ENGINE_ID,
                              pDMNdAttrib->dm_path_p, RoleInfo.admin_role, &pFileName) == OF_FAILURE)
                     {
                        if ((pFileName == NULL) || (strlen(pFileName) < 0))
                        {
                           CM_LDSV_DEBUG_PRINT("File name is null, so can't proceed further");
#ifdef CM_AUDIT_SUPPORT
                           cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_LOAD_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
                                 CM_CMD_LOAD_CONFIG,pDMNdAttrib->dm_path_p,NULL, NULL,"Get File Name Failed", ullSavedConfigVer); 
#endif
                           of_free(pDirPath);
                           of_free(pTempDirPath);
                           cm_ldsv_delete_table_stack (pLoadCfgStackDllQ);
                           cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo, uiOpaqueInfoLen);
                           cm_ldsv_clean_dm_node_attrib_info (pDMNdAttrib);
                           of_free(pRoleFileName);
                           cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
                                 CM_LOAD_FAILED_EVENT, (void*) &ullSavedConfigVer);
                           return OF_FAILURE;
                        }
                     }
                  }
                  else
                  {
                     memset(pFileName, 0, sizeof(pFileName));
                     strncat(pFileName, pDMNdAttrib->file_name, strlen(pDMNdAttrib->file_name));
                  }
                  strcat (pFileName, ".xml");
                  CM_LDSV_DEBUG_PRINT ("pFileName= %s",pFileName);
                  CM_LDSV_CREATE_COMPLETE_DEFAULT_FILE_PATH (aFullPath, pDirPath, pFileName);
                  if ((pFile = fopen(aFullPath, "r")) != NULL)
                  {
                     cm_ldsv_load_file (pFile, pDMNdAttrib, 
                           pKeyNvPair, pOpaqueInfo,
                           uiOpaqueInfoLen, pLoadCfgStackDllQ);
                     fclose(pFile);
		
		     CM_SLEEP_AFTER_DATAPATH_LOAD_CONFIG();	
#ifdef CM_ROLES_PERM_SUPPORT
                     memset(pRoleFileName, 0, sizeof(pRoleFileName));
                     strncat(pRoleFileName, pDMNdAttrib->file_name, strlen(pDMNdAttrib->file_name)+1);
                     strcat(pRoleFileName, "Roles.xml");
                     CM_LDSV_CREATE_COMPLETE_DEFAULT_FILE_PATH (aFullPath, pDirPath, pRoleFileName);
                     if ((pFile = fopen(aFullPath, "r")) != NULL)
                     {
                        cm_ldsv_load_roles_and_permission(pFile, pTransportChannel, FALSE);
                        fclose(pFile);
                     }
#endif
                  }
                  CM_LDSV_CREATE_COMPLETE_DIR_PATH (pTempDirPath, pDirPath, aCurrLoadingVersionNo_g);
                  cm_ldsv_create_backup (pFileName, pTempDirPath);
               }
               break;

#ifdef CM_ROLES_PERM_SUPPORT
            case CM_DMNODE_TYPE_ANCHOR:
               memset (pDirPath, 0, sizeof (pDirPath));
               memset(aDMInstancePath, 0, sizeof(aDMInstancePath));
               cm_ldsv_prepare_load_instance_dm_path (pDMNdAttrib, aDMInstancePath,
                     dm_path_pLoadArrayOfKeys,
                     pLoadCfgStackDllQ,NULL);
               cm_ldsv_create_directory_path (aDMInstancePath, pDirPath);
               CM_LDSV_DEBUG_PRINT ("pDirPath=%s,aDMInstancePath=%s:",pDirPath,aDMInstancePath);

               memset(pRoleFileName, 0, sizeof(pRoleFileName));
               if(pDMNdAttrib->element_attrib.scalar_group_b == TRUE)
               {
                  strncat(pRoleFileName, pDMNdAttrib->file_name, strlen(pDMNdAttrib->file_name));
                  strcat(pRoleFileName, ".xml");
                  CM_LDSV_CREATE_COMPLETE_FILE_PATH (aFullPath, pDirPath, pRoleFileName,  aCurrLoadingVersionNo_g);
                  CM_LDSV_DEBUG_PRINT ("pFileName= %s,aFullPath=%s,pDirPath=%s.",pRoleFileName,aFullPath,pDirPath);
                  if ((pFile = fopen (aFullPath, "r")) != NULL)
                  {
                     cm_ldsv_load_file (pFile, pDMNdAttrib, 
                           pKeyNvPair, pOpaqueInfo,
                           uiOpaqueInfoLen, pLoadCfgStackDllQ);
                     memset(pRoleFileName, 0, sizeof(pRoleFileName));
                     fclose(pFile);
		     CM_SLEEP_AFTER_DATAPATH_LOAD_CONFIG();	
                  }
                  else 
                  {
                     CM_LDSV_CREATE_COMPLETE_DEFAULT_FILE_PATH (aFullPath, pDirPath, pRoleFileName);
                     if ((pFile = fopen (aFullPath, "r")) != NULL)
                     {
                        cm_ldsv_load_file (pFile, pDMNdAttrib, 
                              pKeyNvPair, pOpaqueInfo,
                              uiOpaqueInfoLen, pLoadCfgStackDllQ);
                        memset(pRoleFileName, 0, sizeof(pRoleFileName));
                        fclose(pFile);
		        CM_SLEEP_AFTER_DATAPATH_LOAD_CONFIG();	
                     }
                  }
               }
               memset(pRoleFileName, 0, sizeof(pRoleFileName)); /********AVIJAY****** How much critical?*/
               strncat(pRoleFileName, pDMNdAttrib->name_p, strlen(pDMNdAttrib->name_p)+1);
               strcat(pRoleFileName, "Roles.xml");
               CM_LDSV_CREATE_COMPLETE_FILE_PATH (aFullPath, pDirPath, pRoleFileName, aCurrLoadingVersionNo_g);
               CM_LDSV_DEBUG_PRINT ("pFileName= %s,aFullPath=%s,pDirPath=%s.",pRoleFileName,aFullPath,pDirPath);
               // load from config directory
               if ((pFile = fopen(aFullPath, "r")) != NULL)
               {
                  cm_ldsv_load_roles_and_permission(pFile, pTransportChannel, TRUE);
                  fclose(pFile);
               }
               else
               {
                  //load from config-default directory
                  CM_LDSV_CREATE_COMPLETE_DEFAULT_FILE_PATH (aFullPath, pDirPath, pRoleFileName);
                  if ((pFile = fopen(aFullPath, "r")) != NULL)
                  {
                     cm_ldsv_load_roles_and_permission(pFile, pTransportChannel, TRUE);
                     fclose(pFile);
                  }
               }
               break;
#endif
         }
         pPrevDMNdAttrib = pDMNdAttrib;
      }
      /* Now get the next node from DM template tree */
      iRet = cm_dm_get_next_table_n_anchor_node_info (dm_path_p,
            UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
            pTransportChannel,
            CM_LDSV_MGMT_ENGINE_ID,
            &RoleInfo,
            &pDMNdAttrib, &pOpaqueInfo, &uiOpaqueInfoLen);

      if (iRet == OF_FAILURE)
      {
         CM_LDSV_DEBUG_PRINT ("Get Next Node info failed.");
         if (CM_DLLQ_COUNT(pLoadCfgStackDllQ)>0)
         {
            pDllqNode = CM_DLLQ_LAST (pLoadCfgStackDllQ);
            pStackNode =
               CM_DLLQ_LIST_MEMBER (pDllqNode, cm_ldsv_table_stack_node_t *, ListNode);
            if (pStackNode != NULL)
            {
               bMultiInstance = FALSE;
               cm_ldsv_delete_one_stack_node_using_dm_path(pLoadCfgStackDllQ, pStackNode->dm_path_p, 
                     pDMNdAttrib, pTransportChannel, &bMultiInstance);
               if ((bMultiInstance == TRUE) && (CM_DLLQ_COUNT (pLoadCfgStackDllQ) != 0))
               {
                  // Last node is nothing but, recently added record.
                  pDllqNode = CM_DLLQ_LAST (pLoadCfgStackDllQ);
                  pStackNode =
                     CM_DLLQ_LIST_MEMBER (pDllqNode, cm_ldsv_table_stack_node_t *, ListNode);
                  if (pStackNode != NULL)
                  {
                     memcpy (pOpaqueInfo, pStackNode->pOpaqueInfo,
                           (sizeof (uint32_t) * pStackNode->uiOpaqueInfoLen));
                     uiOpaqueInfoLen = pStackNode->uiOpaqueInfoLen;
                     CM_LDSV_DEBUG_PRINT("Invoking GetNextNodeInfo using StackNode DMPath:%s", pStackNode->dm_path_p);
                     iRet = cm_dm_get_next_node_info (pStackNode->dm_path_p,
                           UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
                           pTransportChannel,
                           CM_LDSV_MGMT_ENGINE_ID,
                           &RoleInfo,
                           &pDMNdAttrib, &pOpaqueInfo,
                           &uiOpaqueInfoLen);
                  }
               }
            }
         }
         else
         {

            //clean runtime data
            while (CM_DLLQ_COUNT (pLoadCfgStackDllQ) > 0)
            {
               cm_ldsv_delete_load_stack_node (pLoadCfgStackDllQ);
            }
            CM_LDSV_DEBUG_PRINT ("Completed loading all the files.");
            cm_ldsv_clean_dm_node_attrib_info (pDMNdAttrib);
            cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo, 
                  uiOpaqueInfoLen);
            of_free(pDirPath);
            of_free(pTempDirPath);
            of_free(pFileName);
            of_free(pRoleFileName);
            cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
                  CM_LOAD_COMPLETED_EVENT, (void*) &ullSavedConfigVer);
#ifdef CM_AUDIT_SUPPORT
            cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_LOAD_SUCCESS, RoleInfo.aAdminName, RoleInfo.admin_role,
                  CM_CMD_LOAD_CONFIG,dm_path_p,NULL, NULL,NULL, ullSavedConfigVer); 
#endif
            return OF_SUCCESS;
         }
      }                           //end if (iRet == OF_FAILURE)
      CM_LDSV_DEBUG_PRINT ("dm_path_p=%s,pDMNdAttrib=%s",dm_path_p,pDMNdAttrib->name_p);
      if (CM_DLLQ_COUNT (pLoadCfgStackDllQ) != 0)
      {
         CM_LDSV_DEBUG_PRINT("DMNdAttrib:%s  ::  PrevDMNdAttrib:%s", pDMNdAttrib->dm_path_p, pPrevDMNdAttrib->dm_path_p);
         bIsChildNode = cm_dm_is_child_element (pDMNdAttrib->dm_path_p,
               pPrevDMNdAttrib->dm_path_p,
               pTransportChannel,
               CM_LDSV_MGMT_ENGINE_ID,
               &RoleInfo);
         if (bIsChildNode == FALSE) 
         {
            CM_LDSV_DEBUG_PRINT("Both are in different path::%s :: %s", pPrevDMNdAttrib->dm_path_p, pDMNdAttrib->dm_path_p);
            // Get last added instances from stack using DMPath and Delete one by one
            if(cm_ldsv_delete_load_stack_node_using_dm_path(pLoadCfgStackDllQ, 
                     pPrevDMNdAttrib->dm_path_p) != OF_SUCCESS)
            {
#ifdef CM_AUDIT_SUPPORT
               cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_LOAD_FAIL, RoleInfo.aAdminName, RoleInfo.admin_role,
                     CM_CMD_LOAD_CONFIG,pDMNdAttrib->dm_path_p,NULL, NULL,"Load File Failed", ullSavedConfigVer); 
#endif
               cm_ldsv_clean_dm_node_attrib_info (pDMNdAttrib);
               cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo, 
                     uiOpaqueInfoLen);
               of_free(pDirPath);
               of_free(pTempDirPath);
               of_free(pFileName);
               of_free(pRoleFileName);
               cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
                     CM_LOAD_FAILED_EVENT, (void*) &ullSavedConfigVer);
               return OF_FAILURE;
            }
            if (CM_DLLQ_COUNT(pLoadCfgStackDllQ)>0)
            {
               pDllqNode = CM_DLLQ_LAST (pLoadCfgStackDllQ);
               pStackNode =
                  CM_DLLQ_LIST_MEMBER (pDllqNode, cm_ldsv_table_stack_node_t *, ListNode);
               if (pStackNode != NULL)
               {
                  bIsChildNode = cm_dm_is_child_element (pDMNdAttrib->dm_path_p,
                        pStackNode->dm_path_p,
                        pTransportChannel,
                        CM_LDSV_MGMT_ENGINE_ID,
                        &RoleInfo);
                  if (bIsChildNode == FALSE)
                  {
                     CM_LDSV_DEBUG_PRINT("DMNode Attributs DMPath:%s  ::  pStackNode DMPath:%s", pDMNdAttrib->dm_path_p, pStackNode->dm_path_p);
                     bMultiInstance = FALSE;
                     cm_ldsv_delete_one_stack_node_using_dm_path(pLoadCfgStackDllQ, pStackNode->dm_path_p, 
                           pDMNdAttrib, pTransportChannel, &bMultiInstance);
                     if ((bMultiInstance == TRUE) && (CM_DLLQ_COUNT (pLoadCfgStackDllQ) != 0))
                     {
                        // Last node is nothing but, recently added record.
                        pDllqNode = CM_DLLQ_LAST (pLoadCfgStackDllQ);
                        pStackNode =
                           CM_DLLQ_LIST_MEMBER (pDllqNode, cm_ldsv_table_stack_node_t *, ListNode);
                        if (pStackNode != NULL)
                        {
                           memcpy (pOpaqueInfo, pStackNode->pOpaqueInfo,
                                 (sizeof (uint32_t) * pStackNode->uiOpaqueInfoLen));
                           uiOpaqueInfoLen = pStackNode->uiOpaqueInfoLen;
                           CM_LDSV_DEBUG_PRINT("Invoking GetNextNodeInfo using StackNode DMPath:%s", pStackNode->dm_path_p);
                           iRet = cm_dm_get_next_node_info (pStackNode->dm_path_p,
                                 UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
                                 pTransportChannel,
                                 CM_LDSV_MGMT_ENGINE_ID,
                                 &RoleInfo,
                                 &pDMNdAttrib, &pOpaqueInfo,
                                 &uiOpaqueInfoLen);
                        }
                     }
                  }
               }
            }
         }                         /* if (Node is NOT child of Recent Table Stack Entry) */
      }
   }                             //end while
   /**kc -CID 372*/
   of_free(pDirPath);  
   of_free(pTempDirPath);
   if(pFileName) 
      of_free(pFileName);
   of_free(pRoleFileName);
   /**/
   cm_send_notification_to_je (CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, pTransportChannel, 
         CM_LOAD_COMPLETED_EVENT, (void*) &ullSavedConfigVer);
#ifdef CM_AUDIT_SUPPORT
   cm_ldsv_frame_and_send_log_message(JE_MSG_CONFIG_LOAD_SUCCESS, RoleInfo.aAdminName, RoleInfo.admin_role,
         CM_CMD_LOAD_CONFIG,dm_path_p,NULL, NULL,NULL, ullSavedConfigVer); 
#endif
   return OF_SUCCESS;
}

#endif /* CM_LDSV_SUPPORT */
