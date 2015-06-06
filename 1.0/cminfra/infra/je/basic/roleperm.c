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
 * File name: roleperm.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/25/2013
 * Description: 
*/

#ifdef CM_SUPPORT
#ifdef CM_ROLES_PERM_SUPPORT

#include "jeincld.h"
#include "jeldef.h"
#include "jelif.h"

#define CM_USERAUTHPLUGIN_LIB_PATH "/ofp/lib/authplugin"
cm_auth_user pRegLoginAuthFunPtr;

int32_t cm_auth_userDBInit(void);
int32_t UCMRegisterAuthUserFunction(cm_auth_user pAuthFunPtr);

/******************************************************************************
 * Function Name : cm_verify_role_permissions
 * Description   : Used to verify permissions for a given role , before performing any 
 operation on security application.
 * Input params  : dm_path_p   : points to DM tree node path.
role_name_p : points to role name for which to verify the permissions.
command_id : command name for which for whih to verify the permissions.
 * Output params : NONE
 * Return value  : OF_SUCCESS on successfull verification and OF_FAILURE for failure
 *****************************************************************************/

int32_t cm_verify_role_permissions (char * dm_path_p,
      char * role_name_p, uint32_t command_id)
{
   int32_t return_value;
   int32_t iResult = OF_FAILURE;
   uint32_t permissions = 0;
   char aDMTplPath[1024] = { };
   struct cm_dm_role_permission *role_perm_p = NULL;
   unsigned char bInstance=FALSE;
   //  char *pLastToken=NULL, *key_p=NULL;
   uint32_t uiTmpLen, uiDMPathLen;

   if (!dm_path_p)
   {
      CM_JE_DEBUG_PRINT ("DM  Path is Null");
      return OF_FAILURE;
   }

   if (!role_name_p)
   {
      CM_JE_DEBUG_PRINT ("Role Name is  Null");
      return OF_FAILURE;
   }

   if (role_name_p[0] == '\0')
   {
      CM_JE_DEBUG_PRINT ("Role Name is  Null");
      return OF_FAILURE;
   }

   if (cmi_dm_remove_keys_from_path (dm_path_p, aDMTplPath,
            of_strlen (dm_path_p)) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Remove KeyFrom Path Failed");
      return OF_FAILURE;
   }

   role_perm_p =
      (struct cm_dm_role_permission *) of_calloc (1, sizeof (struct cm_dm_role_permission));
   if (!role_perm_p)
   {
      CM_JE_DEBUG_PRINT ("Memory Allocatin failed for role_perm_p");
      return OF_FAILURE;
   }

   of_strcpy(role_perm_p->role,role_name_p);
   CM_JE_DEBUG_PRINT("Role is %s",role_perm_p->role);

   uiTmpLen = uiDMPathLen= of_strlen(dm_path_p);

   while(( dm_path_p[uiTmpLen--] != '.' )&& (uiTmpLen > 0))
   {
      if(dm_path_p[uiTmpLen]== '{')
      {
         bInstance=TRUE;
      }
   }

   if( command_id == CM_CMD_ADD_PARAMS)
      bInstance=FALSE;

   /* Use the DM API to read the permissions for the given node and role */
   if (bInstance == FALSE)
   {
      CM_JE_DEBUG_PRINT ("It is a template");
      return_value = cmi_dm_get_permissions (aDMTplPath, role_perm_p);
      permissions = role_perm_p->permissions;
      CM_JE_DEBUG_PRINT ("DMPath = %s Permission = %d", aDMTplPath, permissions);
      if (return_value == OF_FAILURE)
      {
         CM_JE_DEBUG_PRINT ("UCMDM_GetPermissionsByRole Failed");
         return OF_FAILURE;
      }
      if(permissions == CM_PERMISSION_NOACCESS)
      {
         return OF_FAILURE;
      }
   }

   if (bInstance == TRUE)
   {
      CM_JE_DEBUG_PRINT ("It is a Instance");
      return_value=cmi_dm_get_instance_role_permissions_by_role (dm_path_p,CM_VORTIQA_SUPERUSER_ROLE, role_perm_p);
      if (return_value == OF_FAILURE)
      {
         CM_JE_DEBUG_PRINT ("cmi_dm_get_instance_role_permissions_by_role Failed");
         if(command_id !=  CM_CMD_ADD_PARAMS)
         {
            return OF_FAILURE;
         }
         else
         {
            CM_JE_DEBUG_PRINT ("Instance is not created yet");
            return_value = cmi_dm_get_permissions (aDMTplPath, role_perm_p);
            if (return_value == OF_FAILURE)
            {
               CM_JE_DEBUG_PRINT ("UCMDM_GetPermissionsByRole Failed");
               return OF_FAILURE;
            }
         }
      }
      permissions = role_perm_p->permissions;
      CM_JE_DEBUG_PRINT ("DMPath = %s Permission = %d", dm_path_p, permissions);
   }

   /* Now Verify the permissions */
   switch (command_id)
	 {
			case CM_CMD_LOAD_CONFIG:
			case CM_CMD_SAVE_CONFIG:
			case CM_CMD_RESET_TO_FACTORY_DEFAULTS:
				 if (permissions >= CM_PERMISSION_READ_WRITE)
				 {
						iResult = OF_SUCCESS;
				 }
				 break;
      case CM_CMD_NOTIFY_JE:
         if (permissions >= CM_PERMISSION_READ_ONLY)
         {
            iResult = OF_SUCCESS;
         }
         break;
      case CM_CMD_GET_DELTA_VERSION_HISTORY:
         if (permissions >= CM_PERMISSION_READ_WRITE)
         {
            iResult = OF_SUCCESS;
         }
         break;
      case CM_CMD_ADD_PARAMS:
         if (permissions >= CM_PERMISSION_READ_WRITE)
         {
            iResult = OF_SUCCESS;
         }
         break;
      case CM_CMD_SET_PARAMS:
      case CM_CMD_CANCEL_TRANS_CMD:
      case CM_CMD_EXEC_TRANS_CMD:
      case CM_CMD_DEL_PARAMS:
      case CM_CMD_CANCEL_PREV_CMD:
      case CM_CMD_SET_PARAM_TO_DEFAULT:
      case CM_CMD_SET_DEFAULTS:
         if (permissions >= CM_PERMISSION_READ_WRITE)
         {
            iResult = OF_SUCCESS;
         }
         break;
      case CM_CMD_GET_CONFIG_SESSION:
      case CM_CMD_COMPARE_PARAM:
      case CM_CMD_GET_VERSIONS:
         if (permissions >= CM_PERMISSION_READ_ONLY)
         {
            iResult = OF_SUCCESS;
         }
         break;
      case CM_CMD_GET_FIRST_N_LOGS:
      case CM_CMD_GET_NEXT_N_LOGS:
      case CM_CMD_GET_FIRST_N_RECS:
      case CM_CMD_GET_NEXT_N_RECS:
      case CM_CMD_GET_EXACT_REC:
         if (permissions >= CM_PERMISSION_READ_ONLY)
         {
            iResult = OF_SUCCESS;
         }
         break;
      case CM_CMD_GET_DM_NODE_INFO:
      case CM_CMD_GET_DM_KEY_CHILD_INFO:
      case CM_CMD_GET_DM_FIRST_NODE_INFO:
      case CM_CMD_GET_DM_NEXT_NODE_INFO:
      case CM_CMD_GET_DM_NEXT_TABLE_ANCHOR_NODE_INFO:
         if (permissions >= CM_PERMISSION_READ_ONLY)
         {
            iResult = OF_SUCCESS;
         }
         break;
      case CM_CMD_GET_DM_NODE_AND_CHILD_INFO:
      case CM_CMD_GET_DM_KEY_CHILDS:
         if (permissions >= CM_PERMISSION_READ_ONLY)
         {
            iResult = OF_SUCCESS;
         }
         break;
      case CM_CMD_GET_DM_KEYS_ARRAY:
         if (permissions >= CM_PERMISSION_READ_ONLY)
         {
            iResult = OF_SUCCESS;
         }
         break;
      case CM_CMD_GET_DM_INSTANCE_MAPLIST_BY_NAMEPATH:
         if (permissions >= CM_PERMISSION_READ_ONLY)
         {
            iResult = OF_SUCCESS;
         }
         break;

      case CM_CMD_IS_DM_INSTANCE_USING_NAMEPATH_AND_KEY:
         if (permissions >= CM_PERMISSION_READ_ONLY)
         {
            iResult = OF_SUCCESS;
         }
         break;

      case CM_CMD_IS_DM_CHILD_NODE:
         if (permissions >= CM_PERMISSION_READ_ONLY)
         {
            iResult = OF_SUCCESS;
         }
         break;
      case CM_CMD_GET_SAVE_FILE_NAME:
      case CM_CMD_GET_SAVE_DIRECTORY_NAME:
         if (permissions >= CM_PERMISSION_READ_ONLY)
         {
            iResult = OF_SUCCESS;
         }
         CM_JE_DEBUG_PRINT ("CM_CMD_GET_SAVE_DIRECTORY_NAME");
         break;
      case CM_CMD_GET_DM_CHILD_NODE_INFO:
      case CM_CMD_GET_DM_CHILD_NODE_COUNT:
         iResult = OF_SUCCESS;
         break;
      case CM_CMD_GET_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_GET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
         if (role_perm_p->permissions >= CM_PERMISSION_READ_ROLEPERM)
         {
            iResult = OF_SUCCESS;
         }
         break;
      case CM_CMD_SET_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_DEL_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_SET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_DEL_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
         if (role_perm_p->permissions >= CM_PERMISSION_READ_WRITE_ROLEPERM)
         {
            iResult = OF_SUCCESS;
         }
         break;
      case CM_CMD_GET_DM_NODE_ROLE_ATTRIBS:
      case CM_CMD_GET_DM_INSTANCE_ROLE_ATTRIBS:
         if (role_perm_p->permissions >= CM_PERMISSION_READ_WRITE)
         {
            iResult = OF_SUCCESS;
         }
         break;
      default:
         break;
   }
   of_free(role_perm_p);
   return iResult;
}


int32_t UCMRegisterAuthUserFunction(cm_auth_user pAuthFunPtr)
{
   pRegLoginAuthFunPtr = pAuthFunPtr;
   return OF_SUCCESS;
}

int32_t cm_auth_userDBInit(void)
{
/*   int32_t lRetVal=OF_FAILURE;*/
   DIR *pDir =NULL;
   struct dirent *dp;
   char  path[64]={'\0'};

   of_strcpy(path,CM_USERAUTHPLUGIN_LIB_PATH);

   pDir = opendir(path);
   if(!pDir)
   {
      CM_JE_DEBUG_PRINT("%s():diropen for %s failed\n\r",__FUNCTION__,path);
      return OF_FAILURE;
   }

   while((dp = readdir(pDir))!= NULL)
   { 
      CM_JE_DEBUG_PRINT("%s: filename=%s\n",__FUNCTION__,dp->d_name);
      if(!(strcmp(dp->d_name,"."))||!(strcmp(dp->d_name,"..")))
      {
         continue;
      }

      sprintf (path,
            "%s/%s",CM_USERAUTHPLUGIN_LIB_PATH,dp->d_name);

      CM_JE_DEBUG_PRINT("%s(): path:%s\n\r",__FUNCTION__,path);

      /*     if((lRetVal =  cm_dl_open(path,"cm_auth_userInit"))!=OF_SUCCESS)
             {
             CM_JE_DEBUG_PRINT("%s:failed \n",__FUNCTION__);
             closedir(pDir);
             exit(1);
             }  
             */
   }
   closedir(pDir);
   return OF_SUCCESS;
}
#endif
#endif
