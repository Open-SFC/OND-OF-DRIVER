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
 * File name: dmrole.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/15/2013
 * Description: This file contains source code implementing 
 *              configuration middle-ware data model tree. 
*/

#ifdef CM_SUPPORT
#ifdef CM_DM_SUPPORT
#ifdef CM_ROLES_PERM_SUPPORT
#include "dmincld.h"

int32_t cmi_dm_role_perm_init (void)
{
   struct cm_dm_data_element *pNode = NULL;
   ucmdm_RolePermission_t *role_perm_p = NULL;
   struct cm_dm_role_permission VortiQaRolePerm = {  CM_VORTIQA_SUPERUSER_ROLE, CM_PERMISSION_READ_WRITE_ROLEPERM };
   struct cm_dm_role_permission AdminRolePerm = { CM_DM_ROLE1, CM_PERMISSION_READ_WRITE };
   struct cm_dm_role_permission GuestRolePerm = { CM_DM_ROLE2, CM_PERMISSION_READ_ONLY};
   struct cm_dm_role_permission UserRolePerm  = { CM_DM_ROLE3, CM_PERMISSION_READ_ONLY};

   pNode = cm_dm_get_root_node ();
   if (unlikely (!pNode))
   {
      return;
   }

   role_perm_p =
      (ucmdm_RolePermission_t *) of_calloc (1, sizeof (ucmdm_RolePermission_t));
   if (role_perm_p == NULL)
   {
      return;
   }

   role_perm_p->permissions = CM_PERMISSION_READ_WRITE_ROLEPERM;
   of_strncpy (role_perm_p->role, CM_DM_DEFAULT_ROLE, UCMDM_ADMIN_ROLE_MAX_LEN);

   /* Attach to the Role-Permission list */
   if (cmi_dm_attach_role_perm_entry (pNode, role_perm_p) != OF_SUCCESS)
   {
      of_free (role_perm_p);
      return;
   }

   if ((cmi_dm_set_role_permissions (CM_DM_ROOT_NODE_PATH, CM_DM_DEFAULT_ROLE, &VortiQaRolePerm)) !=
         OF_SUCCESS)
   {
      CM_DM_DEBUG_PRINT ("cmi_dm_set_role_permissions failed");
      return;
   }

   if ((cmi_dm_set_role_permissions (CM_DM_ROOT_NODE_PATH, CM_DM_DEFAULT_ROLE, &AdminRolePerm)) !=
         OF_SUCCESS)
   {
      CM_DM_DEBUG_PRINT ("cmi_dm_set_role_permissions failed");
      return;
   }

   if ((cmi_dm_set_role_permissions (CM_DM_ROOT_NODE_PATH, CM_DM_DEFAULT_ROLE, &GuestRolePerm)) !=
         OF_SUCCESS)
   {
      CM_DM_DEBUG_PRINT ("cmi_dm_set_role_permissions failed");
      return;
   }

   if ((cmi_dm_set_role_permissions (CM_DM_ROOT_NODE_PATH, CM_DM_DEFAULT_ROLE, &UserRolePerm)) !=
         OF_SUCCESS)
   {
      CM_DM_DEBUG_PRINT ("cmi_dm_set_role_permissions failed");
      return;
   }

   return;
}

int32_t cm_dm_register_role_permissions (char * path_p,
      struct cm_dm_role_permission * role_perm_p)
{
   struct cm_dm_data_element *pNode = NULL;
   ucmdm_RolePermission_t *pDMEleRolePerm = NULL;

   if ((unlikely ((!path_p) || (!role_perm_p))))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Get Parent Node based on input path */
   pNode = cmi_dm_get_node_by_namepath (path_p);
   if (unlikely (pNode == NULL))
   {
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   pDMEleRolePerm =
      (ucmdm_RolePermission_t *) of_calloc (1, sizeof (ucmdm_RolePermission_t));
   if (!pDMEleRolePerm)
   {
      return CM_DM_ERR_MEM_RESOURCES;
   }

   of_memcpy (pDMEleRolePerm->role, role_perm_p->role,
         UCMDM_ADMIN_ROLE_MAX_LEN);
   pDMEleRolePerm->permissions = role_perm_p->permissions;

   /* Attach to the Role-Permission list */
   if (cmi_dm_attach_role_perm_entry (pNode, pDMEleRolePerm) != OF_SUCCESS)
   {
      of_free (pDMEleRolePerm);
      return OF_FAILURE;
   }

   return OF_SUCCESS;
}

/**************************************************************************
  Function Name : UCMDM_Setrole_permissions 
  Input Args    : node_path_p - Pointer to DM Path
  : role_perm_p - Pointer to Roles and Permission structure
  Output Args   : None
Description   : This API used by security application to set Roles and Permissions
: of a particular Data Element Node for given DM Path 
Return Values : OF_SUCCESS or appropriate error code
 *************************************************************************/
int32_t cmi_dm_set_role_permissions (char * node_path_p,
      char * role_name_p,
      struct cm_dm_role_permission * role_perm_p)
{
   struct cm_dm_data_element *pNode = NULL;
   ucmdm_RolePermission_t *tmp_role_perm_p = NULL;
   uint32_t uiMaxTokenLen = 0;
   char *pToken = NULL;

   if (unlikely ((!node_path_p) || (!role_perm_p)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   if (of_strlen(role_perm_p->role) == 0)
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Allocate token buffer */
   uiMaxTokenLen = of_strlen (node_path_p) + 1;
   pToken = (char *) of_calloc (1, uiMaxTokenLen);

   /* Verify whether the data path is valid accroding to DMTree */
   if (cmi_dm_remove_keys_from_path (node_path_p, pToken, (uiMaxTokenLen - 1)) !=
         OF_SUCCESS)
   {
      of_free (pToken);
      return OF_FAILURE;
   }

   /* Get Parent Node based on input path */
   pNode = cmi_dm_get_node_by_namepath (pToken);
   if (unlikely (!pNode))
   {
      of_free (pToken);
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   if ((cmi_dm_verify_role_set_privileges (pNode, role_name_p)) != OF_SUCCESS)
   {
      CM_DM_DEBUG_PRINT ("Admin has no permissions to set roles");
      of_free (pToken);
      return OF_FAILURE;
   }

   if ((cmi_dm_verify_role_permissions (role_perm_p)) != OF_SUCCESS)
   {
      of_free (pToken);
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Check whether this role is already assigned or not */
   tmp_role_perm_p = cmi_dm_get_role_perms_entry_by_role (pNode, role_perm_p->role);
   if (tmp_role_perm_p != NULL)
   {
      CM_DM_DEBUG_PRINT ("Role already existed ... hence deleting role");
      if (!of_strcmp(role_name_p, role_perm_p->role))
      {
         CM_DM_DEBUG_PRINT ("Trying to modify same role");
         of_free (pToken);
         return OF_FAILURE;
      }
      //  if (role_perm_p->permissions > tmp_role_perm_p->permissions)
      //  {
      tmp_role_perm_p->permissions = role_perm_p->permissions;
      //  }
      //   cmi_dm_dettach_role_perm_entry (pNode, tmp_role_perm_p);
      of_free (pToken);
      return OF_SUCCESS;
   }

   /* Create a new role-permission entry */
   tmp_role_perm_p =
      (ucmdm_RolePermission_t *) of_calloc (1, sizeof (ucmdm_RolePermission_t));
   if (tmp_role_perm_p == NULL)
   {
      of_free (pToken);
      return CM_DM_ERR_MEM_RESOURCES;
   }

   cmi_dm_copy_role_perms (tmp_role_perm_p, role_perm_p, CM_COPY_FROM_EXT_STRUCT);

   /* Attach to the Role-Permission list */
   if (cmi_dm_attach_role_perm_entry (pNode, tmp_role_perm_p) != OF_SUCCESS)
   {
      of_free (pToken);
      of_free (tmp_role_perm_p);
      return OF_FAILURE;
   }

   of_free (pToken);
   return OF_SUCCESS;
}

/**************************************************************************
  Function Name : UCMDM_GetPermissionsByRole 
  Input Args    : node_path_p - Pointer to DM Path
  : role_perm_p - Pointer to Roles and Permission structure
  Output Args   : None
Description   : This API used by security application to Get Roles and Permissions
: of a particular Data Element Node for given DM Path 
Return Values : OF_SUCCESS or appropriate error code
 *************************************************************************/
int32_t cmi_dm_get_permissions_by_role (char * node_path_p,
      char * role_name_p,
      struct cm_dm_role_permission * role_perm_p)
{
   struct cm_dm_data_element *pNode, *pTmpNode;
   ucmdm_RolePermission_t *tmp_role_perm_p;
   uint32_t uiMaxTokenLen = 0;
   char *pToken = NULL;

   if (unlikely ((!node_path_p) || (!role_perm_p)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Allocate token buffer */
   uiMaxTokenLen = of_strlen (node_path_p) + 1;
   pToken = (char *) of_calloc (1, uiMaxTokenLen);

   /* Verify whether the data path is valid accroding to DMTree */
   if (cmi_dm_remove_keys_from_path (node_path_p, pToken, (uiMaxTokenLen - 1)) !=
         OF_SUCCESS)
   {
      of_free (pToken);
      return OF_FAILURE;
   }

   /* Get Parent Node based on input path */
   pNode = cmi_dm_get_node_by_namepath (pToken);
   if (unlikely (!pNode))
   {
      of_free (pToken);
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   if (cmi_dm_verify_role_get_privileges (pNode, role_name_p) != OF_SUCCESS)
   {
      CM_DM_DEBUG_PRINT ("Admin has no permissions to Get roles");
      of_free (pToken);
      return OF_FAILURE;
   }

   /* Check whether this role is already assigned or not */
   pTmpNode = pNode;
   while (pTmpNode)
   {
      tmp_role_perm_p = cmi_dm_get_role_perms_entry_by_role (pTmpNode, role_perm_p->role);
      if (tmp_role_perm_p != NULL)
      {
         cmi_dm_copy_role_perms (tmp_role_perm_p, role_perm_p, CM_COPY_TO_EXT_STRUCT);
         of_free (pToken);
         return OF_SUCCESS;
      }
      pTmpNode = pTmpNode->parent_p;
   }
   of_free (pToken);
   return OF_FAILURE;
}

int32_t cmi_dm_get_role_permissions (char * node_path_p,
      char * role_name_p,
      struct cm_dm_array_of_role_permissions **
      role_perm_ary_pp)
{
   struct cm_dm_data_element *pNode;
   struct cm_dm_array_of_role_permissions *role_perm_ary_p;
   UCMDllQ_t *pDll;
   UCMDllQNode_t *pDllQNode;
   uint32_t uiRoleCnt, uiCnt;
   ucmdm_RolePermission_t *role_perm_p;
   uint32_t uiMaxTokenLen = 0;
   char *pToken = NULL;

   if (unlikely ((!node_path_p) || (!role_name_p)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Allocate token buffer */
   uiMaxTokenLen = of_strlen (node_path_p) + 1;
   pToken = (char *) of_calloc (1, uiMaxTokenLen);

   /* Verify whether the data path is valid accroding to DMTree */
   if (cmi_dm_remove_keys_from_path (node_path_p, pToken, (uiMaxTokenLen - 1)) !=
         OF_SUCCESS)
   {
      of_free (pToken);
      return OF_FAILURE;
   }

   /* Get Node based on input path */
   pNode = cmi_dm_get_node_by_namepath (pToken);
   if (unlikely (!pNode))
   {
      of_free (pToken);
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   if (cmi_dm_verify_role_get_privileges (pNode, role_name_p) != OF_SUCCESS)
   {
      CM_DM_DEBUG_PRINT ("Admin has no permissions to Get roles");
      of_free (pToken);
      return OF_FAILURE;
   }

   role_perm_ary_p =
      (struct cm_dm_array_of_role_permissions *) of_calloc (1,
            sizeof
            (struct cm_dm_array_of_role_permissions));
   if (!role_perm_ary_p)
   {
      CM_DM_DEBUG_PRINT ("Memory Allocation Failed");
      of_free (pToken);
      return OF_FAILURE;
   }
   role_perm_ary_p->count_ui = 0;

   pDll = &(pNode->role_perm_list);
   uiRoleCnt = CM_DLLQ_COUNT (pDll);
   if (uiRoleCnt > 0)
   {
      role_perm_ary_p->count_ui = uiRoleCnt;
      role_perm_ary_p->role_permissions = (struct cm_dm_role_permission *) of_calloc
         (role_perm_ary_p->count_ui, sizeof (struct cm_dm_role_permission));
      if (!role_perm_ary_p->role_permissions)
      {
         CM_DM_DEBUG_PRINT ("Memory Allocation Failed");
         of_free (role_perm_ary_p);
         of_free (pToken);
         return OF_FAILURE;
      }
      uiCnt = 0;
      CM_DLLQ_SCAN (pDll, pDllQNode, UCMDllQNode_t *)
      {
         role_perm_p =
            CM_DLLQ_LIST_MEMBER (pDllQNode, ucmdm_RolePermission_t *, list_node);
         of_strcpy (role_perm_ary_p->role_permissions[uiCnt].role, role_perm_p->role);
         role_perm_ary_p->role_permissions[uiCnt].permissions =
            role_perm_p->permissions;
         uiCnt++;
      }
   }
   *role_perm_ary_pp = role_perm_ary_p;
   of_free (pToken);
   return OF_SUCCESS;
}

int32_t cmi_dm_get_permissions (char * node_path_p,
      struct cm_dm_role_permission * role_perm_p)
{
   struct cm_dm_data_element *pNode, *pTmpNode;
   ucmdm_RolePermission_t *tmp_role_perm_p;

   if (unlikely ((!node_path_p) || (!role_perm_p)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Get Parent Node based on input path */
   pNode = cmi_dm_get_node_by_namepath (node_path_p);
   if (unlikely (!pNode))
   {
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   /* Check whether this role is already assigned or not */
   pTmpNode = pNode;
   while (pTmpNode)
   {
      tmp_role_perm_p = cmi_dm_get_role_perms_entry_by_role (pTmpNode, role_perm_p->role);
      if (tmp_role_perm_p != NULL)
      {
         cmi_dm_copy_role_perms (tmp_role_perm_p, role_perm_p, CM_COPY_TO_EXT_STRUCT);
         return OF_SUCCESS;
      }
      pTmpNode = pTmpNode->parent_p;
   }

   return OF_FAILURE;
}

/**************************************************************************
  Function Name : UCMDM_DelPermissionsByRole
  Input Args    : node_path_p - Pointer to DM Path
  : role_perm_p - Pointer to Roles and Permission structure
  Output Args   : None
Description   : This API used by security application to Delete Roles and Permissions
: of a particular Data Element Node for given DM Path 
Return Values : OF_SUCCESS or appropriate error code
 *************************************************************************/
int32_t cmi_dm_del_permissions_by_role (char * node_path_p,
      char * role_name_p,
      struct cm_dm_role_permission * role_perm_p)
{
   struct cm_dm_data_element *pNode = NULL;
   ucmdm_RolePermission_t *tmp_role_perm_p = NULL;
   uint32_t uiMaxTokenLen = 0;
   char *pToken = NULL;

   if (unlikely ((!node_path_p) || (!role_perm_p)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Allocate token buffer */
   uiMaxTokenLen = of_strlen (node_path_p) + 1;
   pToken = (char *) of_calloc (1, uiMaxTokenLen);

   /* Verify whether the data path is valid accroding to DMTree */
   if (cmi_dm_remove_keys_from_path (node_path_p, pToken, (uiMaxTokenLen - 1)) !=
         OF_SUCCESS)
   {
      of_free (pToken);
      return OF_FAILURE;
   }

   /* Get Parent Node based on input path */
   pNode = cmi_dm_get_node_by_namepath (pToken);
   if (unlikely (!pNode))
   {
      of_free(pToken);
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   if (cmi_dm_verify_role_set_privileges (pNode, role_name_p) != OF_SUCCESS)
   {
      CM_DM_DEBUG_PRINT ("Admin has no permissions to Delete roles");
      of_free(pToken);
      return OF_FAILURE;
   }

   /* Check whether this role is already assigned or not */
   tmp_role_perm_p = cmi_dm_get_role_perms_entry_by_role (pNode, role_perm_p->role);
   if (tmp_role_perm_p != NULL)
   {
      cmi_dm_dettach_role_perm_entry (pNode, tmp_role_perm_p);
      of_free (tmp_role_perm_p);
      of_free(pToken);
      return OF_SUCCESS;
   }
   of_free(pToken);
   return OF_FAILURE;
}

/**************************************************************************
  Function Name : cmi_dm_attach_role_perm_entry
  Input Args    : pDMNode -Pointer to  DM Node 
  : role_perm_p - Pointer to Role and Permission structure
  Output Args   : None
Description   : This API is used to Attach Role and Permission structure 
: to DM Node
Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/

int32_t cmi_dm_attach_role_perm_entry (struct cm_dm_data_element * pDMNode,
      ucmdm_RolePermission_t * role_perm_p)
{
   if (pDMNode && role_perm_p)
   {
      CM_DLLQ_INIT_NODE (&(role_perm_p->list_node));
      CM_DLLQ_APPEND_NODE (&(pDMNode->role_perm_list), &(role_perm_p->list_node));
      return OF_SUCCESS;
   }

   return OF_FAILURE;
}

/**************************************************************************
  Function Name : cmi_dm_dettach_role_perm_entry
  Input Args    : pDMNode -Pointer to  DM Node 
  : role_perm_p - Pointer to Role and Permission structure
  Output Args   : None
Description   : This API is used to Detach Role and Permission structure 
: from its DM Node
Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_dettach_role_perm_entry (struct cm_dm_data_element * pDMNode,
      ucmdm_RolePermission_t * role_perm_p)
{
   if (pDMNode && role_perm_p)
   {
      CM_DLLQ_DELETE_NODE (&(pDMNode->role_perm_list), &(role_perm_p->list_node));
      return OF_SUCCESS;
   }

   return OF_FAILURE;
}

/**************************************************************************
  Function Name : cmi_dm_get_role_perms_entry_by_role
  Input Args    : pDMNode -Pointer to  DM Node 
  : role_perm_p - Pointer to Role
  Output Args   : None
Description   : This API is used to Get Role and Permission structure 
: based on Role for a given DMNode
Return Values : Pointer to role_info structure
 *************************************************************************/
ucmdm_RolePermission_t *cmi_dm_get_role_perms_entry_by_role (struct cm_dm_data_element *
      pDMNode, char * pRole)
{
   UCMDllQ_t *pDll;
   UCMDllQNode_t *pNode = NULL;
   ucmdm_RolePermission_t *role_perm_p = NULL;

   if (!pDMNode)
   {
      return NULL;
   }
   pDll = &(pDMNode->role_perm_list);
   CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
   {
      role_perm_p =
         CM_DLLQ_LIST_MEMBER (pNode, ucmdm_RolePermission_t *, list_node);
      if (!of_strcmp (role_perm_p->role, pRole))
      {
         return role_perm_p;
      }
   }

   return NULL;
}

/**************************************************************************
  Function Name : cmi_dm_copy_role_perms
  Input Args    : pInternalEntry - Pointer to Internal Role and Permission Entry
  : pExternalEntry - Pointer to External Role and Permission Entry
  : uiCopyDir -  type of copy direction 
  :            CM_COPY_FROM_EXT_STRUCT or CM_COPY_TO_EXT_STRUCT
  Output Args   : None
Description   : This API is used to copy Role and Permission Information
: from Internal to External structure and vice versa.
Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/

int32_t cmi_dm_copy_role_perms (ucmdm_RolePermission_t * pInternalEntry,
      struct cm_dm_role_permission * pExternalEntry,
      uint32_t uiCopyDir)
{
   int32_t return_value = OF_SUCCESS;

   if ((!pInternalEntry) || (!pExternalEntry))
   {
      return OF_FAILURE;
   }

   switch (uiCopyDir)
   {
      case CM_COPY_FROM_EXT_STRUCT:
         of_strncpy (pInternalEntry->role, pExternalEntry->role,
               UCMDM_ADMIN_ROLE_MAX_LEN);
         pInternalEntry->permissions = pExternalEntry->permissions;
         break;

      case CM_COPY_TO_EXT_STRUCT:
         of_strncpy (pExternalEntry->role, pInternalEntry->role,
               UCMDM_ADMIN_ROLE_MAX_LEN);
         pExternalEntry->permissions = pInternalEntry->permissions;
         break;

      default:
         return_value = OF_FAILURE;
         break;
   }

   return return_value;
}

/**************************************************************************
  Function Name : cmi_dm_verify_role_permissions
  Input Args    : role_perm_p - Pointer to Role and Permission structure
  Output Args   : None
Description   : This API verifies the given permission is valid
Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_verify_role_permissions (struct cm_dm_role_permission * role_perm_p)
{
   int32_t return_value = OF_FAILURE;

   if (!role_perm_p)
   {
      return return_value;
   }
   /* We do not care about roles. Just verify permissions */
   switch (role_perm_p->permissions)
   {
      case CM_PERMISSION_NOACCESS:
      case CM_PERMISSION_READ_ONLY:
      case CM_PERMISSION_READ_WRITE:
      case CM_PERMISSION_READ_ROLEPERM:
      case CM_PERMISSION_READ_WRITE_ROLEPERM:
         return_value = OF_SUCCESS;
         break;
      default:
         return_value = OF_FAILURE;
         break;
   }
   return return_value;
}

int32_t cmi_dm_verify_role_set_privileges (struct cm_dm_data_element * pDMNode,
      char * admin_role_p)
{
   int32_t return_value = OF_FAILURE;
   struct cm_dm_data_element *pTmpNode = pDMNode;
   ucmdm_RolePermission_t *admin_role_pPerm;
   unsigned char bAdminPrivileges = FALSE;

   if (!pTmpNode)
   {
      return return_value;
   }

   /* Check whether this role is already assigned or not */
   while ((pTmpNode) && (bAdminPrivileges == FALSE))
   {
      admin_role_pPerm = cmi_dm_get_role_perms_entry_by_role (pTmpNode, admin_role_p);
      if (admin_role_pPerm == NULL)
      {
         pTmpNode = pTmpNode->parent_p;
      }
      else
      {
         CM_DM_DEBUG_PRINT ("Admin Role Found");
         bAdminPrivileges = TRUE;
      }
   }

   if (bAdminPrivileges == FALSE)
   {
      CM_DM_DEBUG_PRINT ("Role1 has NO permissions to set a role");
      return OF_FAILURE;
   }

   if (admin_role_pPerm->permissions != CM_PERMISSION_READ_WRITE_ROLEPERM)
   {
      CM_DM_DEBUG_PRINT ("Role1 has Read/Write Role Permissions");
      return OF_FAILURE;
   }

   return OF_SUCCESS;
}

int32_t cmi_dm_verify_role_get_privileges (struct cm_dm_data_element * pDMNode,
      char * admin_role_p)
{
   int32_t return_value = OF_FAILURE;
   struct cm_dm_data_element *pTmpNode = pDMNode;
   ucmdm_RolePermission_t *admin_role_pPerm = NULL;
   unsigned char bAdminPrivileges = FALSE;

   if ((!pTmpNode) || (!admin_role_p))
   {
      return return_value;
   }

   /* Check whether this role is already assigned or not */
   while ((pTmpNode) && (bAdminPrivileges == FALSE))
   {
      admin_role_pPerm = cmi_dm_get_role_perms_entry_by_role (pTmpNode, admin_role_p);
      if (admin_role_pPerm == NULL)
      {
         pTmpNode = pTmpNode->parent_p;
      }
      else
      {
         CM_DM_DEBUG_PRINT ("Admin Role Found");
         bAdminPrivileges = TRUE;
      }
   }

   if (bAdminPrivileges == FALSE)
   {
      CM_DM_DEBUG_PRINT ("Role1 has NO permissions to set a role");
      return OF_FAILURE;
   }

   if ((admin_role_pPerm->permissions == CM_PERMISSION_READ_WRITE_ROLEPERM) ||
         (admin_role_pPerm->permissions == CM_PERMISSION_READ_ROLEPERM))
   {
      CM_DM_DEBUG_PRINT ("Role1 has Read/Write Role Permissions");
      return OF_SUCCESS;
   }
   return OF_FAILURE;
}

/****************************************************************************
 * * * * * * * * I N S T A N C E S * * * * * * * * * * * * * * * * * * * * *
 ***************************************************************************/

int32_t cmi_dm_set_instance_role_permissions (char * node_path_p,
      char * role_name_p,
      struct cm_dm_role_permission * role_perm_p)
{
   UCMDM_Inode_t *pINode = NULL;
   ucmdm_RolePermission_t *tmp_role_perm_p = NULL;
   struct cm_dm_data_element *pDMNode;
   UCMDM_Instance_t *pInstanceNode;
   struct cm_array_of_nv_pair *array_nv_pair_p;
   uint32_t uiKeyCnt;
   //void *opaque_info_p;

   if (unlikely ((!node_path_p) || (!role_perm_p)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   //if (cmi_dm_get_key_array_from_namepath (node_path_p, &array_nv_pair_p) != OF_SUCCESS)
   if ( cmi_dm_get_instance_keys_from_name_path (node_path_p, &array_nv_pair_p) != OF_SUCCESS)
   {
      return OF_FAILURE;
   }

   if( array_nv_pair_p && array_nv_pair_p->count_ui == 0 )
   {
      CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
      return OF_FAILURE;
   }

   uiKeyCnt = array_nv_pair_p->count_ui;

#if 0
   if (cmi_dm_is_instance_map_entry_using_key_and_name_path
         (node_path_p, array_nv_pair_p->nv_pairs[uiKeyCnt - 1].value_p,
          array_nv_pair_p->nv_pairs[uiKeyCnt - 1].value_type) != TRUE)
   {
      CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
      return OF_FAILURE;
   }
#endif

   if (cmi_dm_is_instance_map_entry_using_key_array_and_name_path
         (node_path_p, array_nv_pair_p) != TRUE)
   {
      if(array_nv_pair_p)
      {
         CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
      }
      return OF_FAILURE;
   }
#if 0
   /* Get Parent Node based on input path */
   if (cmi_dm_get_first_inode_using_instance_path (node_path_p,
            UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
            &pINode, &opaque_info_p) != OF_SUCCESS)
   {
      CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
      return OF_FAILURE;
   }
#endif
   pINode=cmi_dm_get_inode_by_name_path(node_path_p);

   if (unlikely (!pINode))
   {
      if(array_nv_pair_p)
         CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }
#if 0
   cmi_dm_free_instance_opaque_info (opaque_info_p);
#endif
   pInstanceNode =
      cmi_dm_get_instance_using_keyArray (pINode, array_nv_pair_p);
   if (unlikely (!pInstanceNode))
   {
      if(array_nv_pair_p)
         CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   if (cmi_dm_verify_instance_role_set_privileges (pInstanceNode, role_name_p) !=
         OF_SUCCESS)
   {
      CM_DM_DEBUG_PRINT
         ("Admin has no permissions to set roles in Instance Tree");
      pDMNode =
         (struct cm_dm_data_element *) cmi_dm_get_dm_node_from_instance_path (node_path_p,
               of_strlen
               (node_path_p));
      if ((cmi_dm_verify_role_set_privileges (pDMNode, role_name_p)) != OF_SUCCESS)
      {
         CM_DM_DEBUG_PRINT
            ("Admin has no permissions to set roles in Template Tree");

         if(array_nv_pair_p)
            CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
         return OF_FAILURE;
      }
   }

   if (cmi_dm_verify_role_permissions (role_perm_p) != OF_SUCCESS)
   {
      if(array_nv_pair_p)
         CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Check whether this role is already assigned or not */
   tmp_role_perm_p =
      cmi_dm_get_instance_role_perms_entry_by_role (pInstanceNode, role_perm_p->role);
   if (tmp_role_perm_p != NULL)
   {
      if (!of_strcmp(role_name_p, role_perm_p->role))
      {
         CM_DM_DEBUG_PRINT        ("Trying to modify same role");
         if(array_nv_pair_p)
            CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
         return OF_FAILURE;
      }
      //cmi_dm_copy_role_perms (tmp_role_perm_p, role_perm_p, CM_COPY_FROM_EXT_STRUCT);
      //    if (role_perm_p->permissions > tmp_role_perm_p->permissions)
      //    {
      tmp_role_perm_p->permissions = role_perm_p->permissions;
      //  }
      if(array_nv_pair_p)
         CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
      return OF_SUCCESS;
   }

   /* Create a new role-permission entry */
   tmp_role_perm_p =
      (ucmdm_RolePermission_t *) of_calloc (1, sizeof (ucmdm_RolePermission_t));
   if (tmp_role_perm_p == NULL)
   {
      if(array_nv_pair_p)
         CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
      return CM_DM_ERR_MEM_RESOURCES;
   }

   cmi_dm_copy_role_perms (tmp_role_perm_p, role_perm_p, CM_COPY_FROM_EXT_STRUCT);

   /* Attach to the Role-Permission list */
   if (cmi_dm_attach_instance_role_perm_entry (pInstanceNode, tmp_role_perm_p) !=
         OF_SUCCESS)
   {
      of_free (tmp_role_perm_p);
      if(array_nv_pair_p)
         CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
      return OF_FAILURE;
   }

   if(array_nv_pair_p)
      CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);

   return OF_SUCCESS;
}

int32_t cmi_dm_get_instance_role_permissions_by_role (char * node_path_p,
      char * role_name_p,
      struct cm_dm_role_permission * role_perm_p)
{
   ucmdm_RolePermission_t *tmp_role_perm_p;
   UCMDM_Inode_t *pINode;
   UCMDM_Instance_t *pInstanceNode;
   struct cm_array_of_nv_pair *array_nv_pair_p;
   struct cm_dm_data_element *pDMNode, *pTmpNode;
   uint32_t uiKeyCnt;
   //  void *opaque_info_p;

   if (unlikely ((!node_path_p) || (!role_perm_p)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   CM_DM_DEBUG_PRINT("DMPath %s AdminRole %s userRole %s",node_path_p,role_name_p,role_perm_p->role); 
   //if (cmi_dm_get_key_array_from_namepath (node_path_p, &array_nv_pair_p) != OF_SUCCESS)
   if ( cmi_dm_get_instance_keys_from_name_path (node_path_p, &array_nv_pair_p) != OF_SUCCESS)
   {
      return OF_FAILURE;
   }

   uiKeyCnt = array_nv_pair_p->count_ui;
   if (cmi_dm_is_instance_map_entry_using_key_array_and_name_path
         (node_path_p, array_nv_pair_p) != TRUE)
   {
      if(array_nv_pair_p)
         CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
      return OF_FAILURE;
   }

#if 0
   /* Get Parent Node based on input path */
   if (cmi_dm_get_first_inode_using_instance_path (node_path_p,
            UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
            &pINode, &opaque_info_p) != OF_SUCCESS)
   {
      CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
      return OF_FAILURE;
   }
#endif
   pINode=cmi_dm_get_inode_by_name_path(node_path_p);
   if (unlikely (!pINode))
   {
      if(array_nv_pair_p)
         CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }
#if 0
   cmi_dm_free_instance_opaque_info (opaque_info_p);
#endif
   pInstanceNode = cmi_dm_get_instance_using_keyArray (pINode,
         array_nv_pair_p);
   if (unlikely (!pInstanceNode))
   {
      if(array_nv_pair_p)
         CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   if(array_nv_pair_p)
      CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui); //CID 337
   if (cmi_dm_verify_instance_role_get_privileges (pInstanceNode, role_name_p) !=
         OF_SUCCESS)
   {
      CM_DM_DEBUG_PRINT ("Admin has no permissions to Get roles in Instance");
      pDMNode =
         (struct cm_dm_data_element *) cmi_dm_get_dm_node_from_instance_path (node_path_p,
               of_strlen
               (node_path_p));
      if ((cmi_dm_verify_role_get_privileges (pDMNode, role_name_p)) != OF_SUCCESS)
      {
         CM_DM_DEBUG_PRINT
            ("Admin has no permissions to Get roles in Template Tree");
         /*      CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui); CID 337*/
         return OF_FAILURE;
      }
   }

   if (cmi_dm_verify_role_permissions (role_perm_p) != OF_SUCCESS)
   {
      /*    CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui); CID 337*/
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Check whether this role is already assigned or not */
   tmp_role_perm_p = cmi_dm_get_instance_tree_role_perm_by_role (pInstanceNode,
         role_perm_p->role);
   if (tmp_role_perm_p != NULL)
   {
      CM_DM_DEBUG_PRINT("%s Role Found at instance with perm %d",role_perm_p->role, tmp_role_perm_p->permissions);
      cmi_dm_copy_role_perms (tmp_role_perm_p, role_perm_p, CM_COPY_TO_EXT_STRUCT);
      /*    CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui); CID 337*/
      return OF_SUCCESS;
   }

   pTmpNode = pINode->pDMNode;
   while (pTmpNode)
   {
      tmp_role_perm_p = cmi_dm_get_role_perms_entry_by_role (pTmpNode, role_perm_p->role);
      if (tmp_role_perm_p != NULL)
      {
         CM_DM_DEBUG_PRINT("%s Role Found at %s with perm %d",role_perm_p->role,pTmpNode->name_p, tmp_role_perm_p->permissions);
         cmi_dm_copy_role_perms (tmp_role_perm_p, role_perm_p, CM_COPY_TO_EXT_STRUCT);
         return OF_SUCCESS;
      }
      pTmpNode = pTmpNode->parent_p;
   }
   /*  CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui); CID 337*/
   return OF_FAILURE;
}

int32_t cmi_dm_get_instance_role_permissions (char * node_path_p,
      char * role_name_p,
      struct cm_dm_array_of_role_permissions **
      role_perm_ary_pp)
{
   struct cm_dm_data_element *pDMNode;
   struct cm_dm_array_of_role_permissions *role_perm_ary_p;
   UCMDM_Inode_t *pINode;
   UCMDM_Instance_t *pInstanceNode;
   struct cm_array_of_nv_pair *array_nv_pair_p;
   uint32_t uiKeyCnt;
   //   void *opaque_info_p;
   UCMDllQ_t *pDll;
   UCMDllQNode_t *pDllQNode;
   uint32_t uiRoleCnt, uiCnt;
   ucmdm_RolePermission_t *role_perm_p;

   if (unlikely ((!node_path_p) || (!role_name_p)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   // if (cmi_dm_get_key_array_from_namepath (node_path_p, &array_nv_pair_p) != OF_SUCCESS)
   if ( cmi_dm_get_instance_keys_from_name_path (node_path_p, &array_nv_pair_p) != OF_SUCCESS)
   {
      return OF_FAILURE;
   }

   if(  array_nv_pair_p && array_nv_pair_p->count_ui == 0 )
   {
      CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
      return OF_FAILURE;
   }

   uiKeyCnt = array_nv_pair_p->count_ui;

   if (cmi_dm_is_instance_map_entry_using_key_array_and_name_path
         (node_path_p, array_nv_pair_p) != TRUE)
   {
      if(array_nv_pair_p)
         CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
      return OF_FAILURE;
   }
#if 0
   /* Get Parent Node based on input path */
   if (cmi_dm_get_first_inode_using_instance_path (node_path_p,
            UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
            &pINode, &opaque_info_p) != OF_SUCCESS)
   {
      CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
      return OF_FAILURE;
   }
#endif
   pINode=cmi_dm_get_inode_by_name_path(node_path_p);
   if (unlikely (!pINode))
   {
      if(array_nv_pair_p)
         CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }
#if 0
   cmi_dm_free_instance_opaque_info (opaque_info_p);
#endif
   pInstanceNode = cmi_dm_get_instance_using_keyArray (pINode,
         array_nv_pair_p);
   if (unlikely (!pInstanceNode))
   {
      if(array_nv_pair_p)
         CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   if (cmi_dm_verify_instance_role_get_privileges (pInstanceNode, role_name_p) !=
         OF_SUCCESS)
   {
      CM_DM_DEBUG_PRINT ("Admin has no permissions to Get roles in Instance");
      pDMNode =
         (struct cm_dm_data_element *) cmi_dm_get_dm_node_from_instance_path (node_path_p,
               of_strlen
               (node_path_p));

      if ((cmi_dm_verify_role_get_privileges (pDMNode, role_name_p)) != OF_SUCCESS)
      {
         CM_DM_DEBUG_PRINT
            ("Admin has no permissions to Get roles in Template Tree");
         if(array_nv_pair_p)
            CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
         return OF_FAILURE;
      }
   }

   role_perm_ary_p =
      (struct cm_dm_array_of_role_permissions *) of_calloc (1,
            sizeof
            (struct cm_dm_array_of_role_permissions));
   if (!role_perm_ary_p)
   {
      CM_DM_DEBUG_PRINT ("Memory Allocation Failed");
      if(array_nv_pair_p)
         CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
      return OF_FAILURE;
   }
   role_perm_ary_p->count_ui = 0;

   pDll = &(pInstanceNode->role_perm_list);
   uiRoleCnt = CM_DLLQ_COUNT (pDll);
   if (uiRoleCnt > 0)
   {
      role_perm_ary_p->count_ui = uiRoleCnt;
      role_perm_ary_p->role_permissions = (struct cm_dm_role_permission *) of_calloc
         (role_perm_ary_p->count_ui, sizeof (struct cm_dm_role_permission));
      if (!role_perm_ary_p->role_permissions)
      {
         CM_DM_DEBUG_PRINT ("Memory Allocation Failed");
         if(array_nv_pair_p)
            CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
         of_free (role_perm_ary_p);
         return OF_FAILURE;
      }
      uiCnt = 0;
      CM_DLLQ_SCAN (pDll, pDllQNode, UCMDllQNode_t *)
      {
         role_perm_p =
            CM_DLLQ_LIST_MEMBER (pDllQNode, ucmdm_RolePermission_t *, list_node);
         of_strcpy (role_perm_ary_p->role_permissions[uiCnt].role, role_perm_p->role);
         role_perm_ary_p->role_permissions[uiCnt].permissions =
            role_perm_p->permissions;
         uiCnt++;
      }
   }
   if(array_nv_pair_p)
      CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
   *role_perm_ary_pp = role_perm_ary_p;
   return OF_SUCCESS;
}

int32_t cmi_dm_del_instance_permissions_by_role (char * node_path_p,
      char * role_name_p,
      struct cm_dm_role_permission * role_perm_p)
{
   UCMDM_Instance_t *pInstanceNode = NULL;
   UCMDM_Inode_t *pINode = NULL;
   ucmdm_RolePermission_t *tmp_role_perm_p = NULL;
   struct cm_array_of_nv_pair *array_nv_pair_p;
   struct cm_dm_data_element *pDMNode;
   uint32_t uiKeyCnt;
   //  void *opaque_info_p = NULL;

   if (unlikely ((!node_path_p) || (!role_perm_p)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   //if (cmi_dm_get_key_array_from_namepath (node_path_p, &array_nv_pair_p) != OF_SUCCESS)
   if ( cmi_dm_get_instance_keys_from_name_path (node_path_p, &array_nv_pair_p) != OF_SUCCESS)
   {
      return OF_FAILURE;
   }

   uiKeyCnt = array_nv_pair_p->count_ui;

   if (cmi_dm_is_instance_map_entry_using_key_array_and_name_path
         (node_path_p, array_nv_pair_p) != TRUE)
   {
      if(array_nv_pair_p)
         CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
      return OF_FAILURE;
   }

   /* Get Parent Node based on input path */
   pINode=cmi_dm_get_inode_by_name_path(node_path_p);
#if 0
   cmi_dm_get_first_inode_using_instance_path (node_path_p,
         UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
         &pINode, &opaque_info_p);
#endif

   if (unlikely (!pINode))
   {
      if(array_nv_pair_p)
         CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   //  cmi_dm_free_instance_opaque_info (opaque_info_p);
   pInstanceNode = cmi_dm_get_instance_using_keyArray (pINode,
         array_nv_pair_p);
   if (unlikely (!pInstanceNode))
   {
      if(array_nv_pair_p)
         CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   if (cmi_dm_verify_instance_role_set_privileges (pInstanceNode, role_name_p) !=
         OF_SUCCESS)
   {
      CM_DM_DEBUG_PRINT ("Admin has no permissions to set roles in Instance");
      pDMNode =
         (struct cm_dm_data_element *) cmi_dm_get_dm_node_from_instance_path (node_path_p,
               of_strlen
               (node_path_p));

      if ((cmi_dm_verify_role_set_privileges (pDMNode, role_name_p)) != OF_SUCCESS)
      {
         CM_DM_DEBUG_PRINT
            ("Admin has no permissions to Set roles in Template Tree");
         if(array_nv_pair_p)
            CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
         return OF_FAILURE;
      }
   }
   /* Check whether this role is already assigned or not */
   tmp_role_perm_p =
      cmi_dm_get_instance_role_perms_entry_by_role (pInstanceNode, role_perm_p->role);
   if (tmp_role_perm_p != NULL)
   {
      cmi_dm_dettach_instance_role_perm_entry (pInstanceNode, tmp_role_perm_p);
      of_free (tmp_role_perm_p);
      if(array_nv_pair_p)
         CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
      return OF_SUCCESS;
   }

   if(array_nv_pair_p)
      CM_FREE_PTR_NVPAIR_ARRAY (array_nv_pair_p, array_nv_pair_p->count_ui);
   return OF_FAILURE;
}

#if UCMDM_INSTANCE_ROLE_AND_PERMISSIONS_USING_KEY
int32_t cmi_dm_set_instance_role_permissions (char * node_path_p,
      char * role_name_p,
      void * key_p, uint32_t key_type,
      struct cm_dm_role_permission * role_perm_p)
{
   UCMDM_Inode_t *pINode = NULL;
   ucmdm_RolePermission_t *tmp_role_perm_p = NULL;
   UCMDM_Instance_t *pInstanceNode;
   void *opaque_info_p;

   if (unlikely ((!node_path_p) || (!role_perm_p)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Get Parent Node based on input path */
   if (cmi_dm_get_first_inode_using_instance_path (node_path_p,
            UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
            &pINode, &opaque_info_p) != OF_SUCCESS)
   {
      return OF_FAILURE;
   }
   if (unlikely (!pINode))
   {
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }
   cmi_dm_free_instance_opaque_info (opaque_info_p);

   pInstanceNode = cmi_dm_get_instance_using_key (pINode, key_p, key_type);
   if (unlikely (!pInstanceNode))
   {
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   if (cmi_dm_verify_instance_role_set_privileges (pInstanceNode, role_name_p) !=
         OF_SUCCESS)
   {
      CM_DM_DEBUG_PRINT ("Admin has no permissions to set roles");
      return OF_FAILURE;
   }

   if (cmi_dm_verify_role_permissions (role_perm_p) != OF_SUCCESS)
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Check whether this role is already assigned or not */
   tmp_role_perm_p =
      cmi_dm_get_instance_role_perms_entry_by_role (pInstanceNode, role_perm_p->role);
   if (tmp_role_perm_p != NULL)
   {
      cmi_dm_copy_role_perms (tmp_role_perm_p, role_perm_p, CM_COPY_FROM_EXT_STRUCT);
      return OF_SUCCESS;
   }

   /* Create a new role-permission entry */
   tmp_role_perm_p =
      (ucmdm_RolePermission_t *) of_calloc (1, sizeof (ucmdm_RolePermission_t));
   if (tmp_role_perm_p == NULL)
   {
      return CM_DM_ERR_MEM_RESOURCES;
   }

   cmi_dm_copy_role_perms (tmp_role_perm_p, role_perm_p, CM_COPY_FROM_EXT_STRUCT);

   /* Attach to the Role-Permission list */
   if (cmi_dm_attach_instance_role_perm_entry (pInstanceNode, tmp_role_perm_p) !=
         OF_SUCCESS)
   {
      of_free (tmp_role_perm_p);
      return OF_FAILURE;
   }

   return OF_SUCCESS;
}

int32_t cmi_dm_get_instance_role_permissions_by_role (char * node_path_p,
      char * role_name_p, void * key_p,
      uint32_t key_type,
      struct cm_dm_role_permission * role_perm_p)
{
   ucmdm_RolePermission_t *tmp_role_perm_p;
   UCMDM_Inode_t *pINode;
   UCMDM_Instance_t *pInstanceNode;
   void *opaque_info_p;

   if (unlikely ((!node_path_p) || (!role_perm_p)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Get Parent Node based on input path */
   if (cmi_dm_get_first_inode_using_instance_path (node_path_p,
            UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
            &pINode, &opaque_info_p) != OF_SUCCESS)
   {
      return OF_FAILURE;
   }

   if (unlikely (!pINode))
   {
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   cmi_dm_free_instance_opaque_info (opaque_info_p);

   pInstanceNode = cmi_dm_get_instance_using_key (pINode, key_p, key_type);
   if (unlikely (!pInstanceNode))
   {
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   if (cmi_dm_verify_instance_role_set_privileges (pInstanceNode, role_name_p) !=
         OF_SUCCESS)
   {
      CM_DM_DEBUG_PRINT ("Admin has no permissions to set roles");
      return OF_FAILURE;
   }

   if (cmi_dm_verify_role_permissions (role_perm_p) != OF_SUCCESS)
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Check whether this role is already assigned or not */
   tmp_role_perm_p = cmi_dm_get_instance_role_perms_entry_by_role (pInstanceNode,
         role_perm_p->role);
   if (tmp_role_perm_p != NULL)
   {
      cmi_dm_copy_role_perms (tmp_role_perm_p, role_perm_p, CM_COPY_TO_EXT_STRUCT);
      return OF_SUCCESS;
   }

   return OF_FAILURE;
}

int32_t cmi_dm_del_instance_permissions_by_role (char * node_path_p,
      char * role_name_p, char * key_p,
      uint32_t key_type,
      struct cm_dm_role_permission * role_perm_p)
{
   UCMDM_Instance_t *pInstanceNode = NULL;
   UCMDM_Inode_t *pINode = NULL;
   ucmdm_RolePermission_t *tmp_role_perm_p = NULL;
   void *opaque_info_p = NULL;

   if (unlikely ((!node_path_p) || (!role_perm_p)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Get Parent Node based on input path */
   cmi_dm_get_first_inode_using_instance_path (node_path_p,
         UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
         &pINode, &opaque_info_p);
   if (unlikely (!pINode))
   {
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   cmi_dm_free_instance_opaque_info (opaque_info_p);
   pInstanceNode = cmi_dm_get_instance_using_key (pINode, key_p, key_type);
   if (unlikely (!pInstanceNode))
   {
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   if (cmi_dm_verify_instance_role_set_privileges (pInstanceNode, role_name_p) !=
         OF_SUCCESS)
   {
      CM_DM_DEBUG_PRINT ("Admin has no permissions to set roles");
      return OF_FAILURE;
   }
   /* Check whether this role is already assigned or not */
   tmp_role_perm_p =
      cmi_dm_get_instance_role_perms_entry_by_role (pInstanceNode, role_perm_p->role);
   if (tmp_role_perm_p != NULL)
   {
      cmi_dm_dettach_instance_role_perm_entry (pInstanceNode, tmp_role_perm_p);
      of_free (tmp_role_perm_p);
      return OF_SUCCESS;
   }

   return OF_FAILURE;
}
#endif
/**************************************************************************
  Function Name : cmi_dm_attach_role_perm_entry
  Input Args    : pDMNode -Pointer to  DM Node 
  : role_perm_p - Pointer to Role and Permission structure
  Output Args   : None
Description   : This API is used to Attach Role and Permission structure 
: to DM Node
Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/

int32_t cmi_dm_attach_instance_role_perm_entry (UCMDM_Instance_t * pInstanceNode,
      ucmdm_RolePermission_t * role_perm_p)
{
   if (pInstanceNode && role_perm_p)
   {
      CM_DLLQ_INIT_NODE (&(role_perm_p->list_node));
      CM_DLLQ_APPEND_NODE (&(pInstanceNode->role_perm_list),
            &(role_perm_p->list_node));
      return OF_SUCCESS;
   }

   return OF_FAILURE;
}

/**************************************************************************
  Function Name : cmi_dm_dettach_role_perm_entry
  Input Args    : pDMNode -Pointer to  DM Node 
  : role_perm_p - Pointer to Role and Permission structure
  Output Args   : None
Description   : This API is used to Detach Role and Permission structure 
: from its DM Node
Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_dettach_instance_role_perm_entry (UCMDM_Instance_t * pInstanceNode,
      ucmdm_RolePermission_t * role_perm_p)
{
   if (pInstanceNode && role_perm_p)
   {
      CM_DLLQ_DELETE_NODE (&(pInstanceNode->role_perm_list),
            &(role_perm_p->list_node));
      return OF_SUCCESS;
   }

   return OF_FAILURE;
}

int32_t cmi_dm_delete_perm_list_from_instance (UCMDM_Instance_t * pInstanceNode)
{
   UCMDllQNode_t *pNode, *pTmp;
   UCMDllQ_t *pDll;
   ucmdm_RolePermission_t *role_perm_p;

   pDll = &(pInstanceNode->role_perm_list);

   CM_DLLQ_DYN_SCAN (pDll, pNode, pTmp, UCMDllQNode_t *)
   {
      role_perm_p =
         CM_DLLQ_LIST_MEMBER (pNode, ucmdm_RolePermission_t *, list_node);
      CM_DLLQ_DELETE_NODE (pDll, (UCMDllQNode_t *) pNode);
      of_free (role_perm_p);
   }
   return OF_SUCCESS;
}

/**************************************************************************
  Function Name : cmi_dm_get_role_perms_entry_by_role
  Input Args    : pDMNode -Pointer to  DM Node 
  : role_perm_p - Pointer to Role
  Output Args   : None
Description   : This API is used to Get Role and Permission structure 
: based on Role for a given DMNode
Return Values : Pointer to role_info structure
 *************************************************************************/

ucmdm_RolePermission_t *cmi_dm_get_instance_role_perms_entry_by_role (UCMDM_Instance_t
      * pInstanceNode,
      char * pRole)
{
   UCMDllQ_t *pDll;
   UCMDllQNode_t *pNode;
   ucmdm_RolePermission_t *role_perm_p = NULL;

   if (!pInstanceNode)
   {
      return NULL;
   }
   pDll = &(pInstanceNode->role_perm_list);
   CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
   {
      role_perm_p =
         CM_DLLQ_LIST_MEMBER (pNode, ucmdm_RolePermission_t *, list_node);
      if (!of_strcmp (role_perm_p->role, pRole))
      {
         return role_perm_p;
      }
   }

   return NULL;
}

int32_t cmi_dm_verify_instance_role_set_privileges (UCMDM_Instance_t *
      pDMInstanceNode,
      char * admin_role_p)
{
   ucmdm_RolePermission_t *admin_role_pPerm = NULL;

   if (!admin_role_p)
   {
      return OF_FAILURE;
   }

   admin_role_pPerm=cmi_dm_get_instance_tree_role_perm_by_role(pDMInstanceNode,admin_role_p);
   /* Check whether this role is already assigned or not */

   if (admin_role_pPerm ==  NULL)
   {
      CM_DM_DEBUG_PRINT ("No Role found");
      return OF_FAILURE;
   }

   if (admin_role_pPerm->permissions != CM_PERMISSION_READ_WRITE_ROLEPERM)
   {
      CM_DM_DEBUG_PRINT ("Role1 has Read/Write Role Permissions");
      return OF_FAILURE;
   }

   return OF_SUCCESS;
}

int32_t cmi_dm_verify_instance_role_get_privileges (UCMDM_Instance_t *
      pDMInstanceNode,
      char * admin_role_p)
{
   ucmdm_RolePermission_t *admin_role_pPerm;

   if (!admin_role_p)
   {
      return OF_FAILURE;
   }

   admin_role_pPerm=cmi_dm_get_instance_tree_role_perm_by_role(pDMInstanceNode,admin_role_p);
   /* Check whether this role is already assigned or not */

   if (admin_role_pPerm ==  NULL)
   {
      CM_DM_DEBUG_PRINT ("No Role found");
      return OF_FAILURE;
   }


   if (admin_role_pPerm->permissions >= CM_PERMISSION_READ_ROLEPERM)
   {
      CM_DM_DEBUG_PRINT ("Role1 has Read/Write Role Permissions");
      return OF_SUCCESS;
   }
   return OF_FAILURE;
}

ucmdm_RolePermission_t* cmi_dm_get_instance_tree_role_perm_by_role(UCMDM_Instance_t *pDMInstanceNode, char * admin_role_p)
{
   UCMDM_Instance_t *pTmpInstanceNode = pDMInstanceNode;
   UCMDM_Inode_t *pTmpInode;
   unsigned char bRoleFound = FALSE;
   ucmdm_RolePermission_t *admin_role_pPerm = NULL;

   if (!admin_role_p)
   {
      return NULL;
   }

   /* Check whether this role is already assigned or not */
   while ((pTmpInstanceNode) && (bRoleFound == FALSE))
   {
      admin_role_pPerm = cmi_dm_get_instance_role_perms_entry_by_role
         (pTmpInstanceNode, admin_role_p);
      if (admin_role_pPerm == NULL)
      {
         pTmpInode = pTmpInstanceNode->pInode;
         pTmpInstanceNode = NULL;
         while (pTmpInode)
         {
            if (pTmpInode->uiParentType == UCMDM_INODE_PARENT_TYPE_INODE)
            {
               pTmpInode = pTmpInode->ParentAccess.parent_inode_p;
            }
            else
            {
               break;
            }
         }
         if (pTmpInode)
         {
            pTmpInstanceNode = pTmpInode->ParentAccess.parent_instance_p;
            pTmpInode = NULL;
         }
      }
      else
      {
         CM_DM_DEBUG_PRINT ("Admin Role Found");
         bRoleFound = TRUE;
      }
   }

   if (bRoleFound == FALSE)
   {
      CM_DM_DEBUG_PRINT ("Role not found");
      return NULL;
   }

   return admin_role_pPerm;
}
#endif /* CM_ROLES_PERM_SUPPORT */
#endif /* CM_DM_SUPPORT */
#endif /* CM_SUPPORT */
