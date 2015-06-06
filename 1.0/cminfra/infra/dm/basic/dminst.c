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
 * File name: dminst.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/15/2013
 * Description: This file contains source code implementing 
 *              instance handling API provided by configuration 
 *              middle-ware data model tree. 
*/

#ifdef CM_SUPPORT
#ifdef CM_DM_SUPPORT

#include "dmincld.h"

#define   likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)

/* One hash table across all instances of all nodes */
UCMDM_InstanceMapHashTables_t InstHashTables;
UCMDM_Inode_t *pDMInstTree;

/**************************************************************************
 Function Name : cm_dm_initInstanceTree
 Input Args    : None 
 Output Args   : None
 Description   : This API 
 Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cm_dm_initInstanceTree (void)
{
  int32_t i;

  /* Initialize Instance hash tables */
  for (i = 0; i < UCMDM_IDMAP_HASH_TABLE_MAX_SIZE; i++)
  {
    CM_DLLQ_INIT_LIST (&(InstHashTables.KeyHashTable[i]));
    CM_DLLQ_INIT_LIST (&(InstHashTables.IDHashTable[i]));
  }

  i = cmi_dm_create_root_instance ();
  if (unlikely (i != OF_SUCCESS))
  {
    return OF_FAILURE;
  }
  return OF_SUCCESS;
}



/* ID-KEY Map related API */
/**************************************************************************
 Function Name : cmi_dm_get_new_instance_id_using_name_path
 Input Args    : node_path_p - Pointer to DM Path
 Output Args   : pInstID - Pointer to New Instance ID
 Description   : This API used to Get New Instance ID using DM Instance Path 
 Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_get_new_instance_id_using_name_path (char * node_path_p,
                                              uint32_t * pInstID)
{
  UCMDM_Inode_t *pInode;

  if (unlikely ((pInstID == NULL) || (node_path_p == NULL)))
  {
    return OF_FAILURE;
  }

  if (unlikely (node_path_p[of_strlen (node_path_p) - 1] == '}'))
  {
    /* We can not create an instance for an instance */
    *pInstID = 0;
    return OF_FAILURE;
  }

  /* First get the Inode matching the path */
  pInode = cmi_dm_get_inode_by_name_path (node_path_p);
  if (unlikely ((pInode == NULL) || (pInode->pDMNode == NULL)))
  {
    *pInstID = 0;
    return OF_FAILURE;
  }

  return cmi_dm_get_new_instance_id_from_inode (pInode, pInstID);
}

/**************************************************************************
 Function Name : cmi_dm_get_new_instance_id_using_name_path
 Input Args    : id_path_p - Pointer to DM ID Path
 Output Args   : pInstID - Pointer to New Instance ID
 Description   : This API used to Get New Instance ID using DM ID Path 
 Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_get_new_instance_id_using_id_path (struct cm_dm_id_path * id_path_p,
                                            uint32_t * pInstanceID)
{
  UCMDM_Inode_t *pInode;

  if (unlikely ((id_path_p == NULL) || (pInstanceID == NULL)))
  {
    return OF_FAILURE;
  }

  if (unlikely (id_path_p->no_of_path_elements == 0))
  {
    *pInstanceID = 0;
    return OF_FAILURE;
  }

  /* First get the Inode matching the path */
  pInode = cmi_dm_get_inode_by_id_path (id_path_p);
  if (unlikely ((pInode == NULL) || (pInode->pDMNode == NULL)))
  {
    *pInstanceID = 0;
    return OF_FAILURE;
  }

  return cmi_dm_get_new_instance_id_from_inode (pInode, pInstanceID);
}

/**************************************************************************
 Function Name :
 Input Args    :
 Output Args   :
 Description   :
 Return Values :
 *************************************************************************/

/**************************************************************************
 Function Name : cmi_dm_add_instance_map_entry_using_name_path 
 Input Args    : node_path_p - Pointer to DM Instance Path
                : pMap - Pointer to InstanceMap Structure
 Output Args   : None
 Description   : This API Adds New Instance Entry into Data Model Instance Tree
                : using DM Instance Name path
 Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/

int32_t cmi_dm_add_instance_map_entry_using_name_path (char * node_path_p,
                                                 struct cm_dm_instance_map * pMap)
{
  UCMDM_Inode_t *pInode;
  UCMDM_Instance_t *pInstanceEntry;

  if (unlikely ((node_path_p == NULL) || (pMap == NULL)))
  {
    return OF_FAILURE;
  }

  if (unlikely (node_path_p[of_strlen (node_path_p) - 1] == '}'))
  {
    /* We can not create an instance for an instance */
    return OF_FAILURE;
  }

  /* First get the Inode matching the path */
  pInode = cmi_dm_get_inode_by_name_path (node_path_p);
  if (unlikely ((pInode == NULL) || (pInode->pDMNode == NULL)))
  {
    return OF_FAILURE;
  }

  /* Check if there is any duplicate, using Instance ID */
  pInstanceEntry = cmi_dm_get_instance_using_id (pInode, pMap->instance_id);
  if (unlikely (pInstanceEntry != NULL))
  {
    /* Duplicate entry found */
    return OF_FAILURE;
  }

  /* Check if there is any duplicate, using application key */
//  pInstanceEntry = cmi_dm_get_instance_using_key (pInode,
  //                                            pMap->key_p, pMap->key_type);
  pInstanceEntry = cmi_dm_get_instance_using_keyArray (pInode,
                                             &pMap->key_nv_pairs);
  if (unlikely (pInstanceEntry != NULL))
  {
    /* Duplicate entry found */
    return OF_FAILURE;
  }

  return cmi_dm_add_instance_map_to_inode (pInode, pMap);
}

#if 0
int32_t UCMDM_DeleteInstanceMapEntry_UsingNamePath (char * node_path_p,
                                                    struct cm_dm_instance_map * pMap)
{
  UCMDM_Inode_t *pInode;
  UCMDllQNode_t *pNode;
  UCMDllQ_t *pDll;
  UCMDM_Instance_t *pInstance;

  if (unlikely ((node_path_p == NULL) || (pMap == NULL)))
  {
    return OF_FAILURE;
  }

  if (unlikely (node_path_p[of_strlen (node_path_p) - 1] == '}'))
  {
    /* We can not create an instance for an instance */
    return OF_FAILURE;
  }

  /* First get the Inode matching the path */
  pInode = cmi_dm_get_inode_by_name_path (node_path_p);
  if (unlikely ((pInode == NULL) || (pInode->pDMNode == NULL)))
  {
    return OF_FAILURE;
  }

  pDll = &(pInode->ChildAccess.InstanceList);
  CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
  {
    pInstance = CM_DLLQ_LIST_MEMBER (pNode, UCMDM_Instance_t *, list_node);
//      if (pInstance->Map.instance_id == instance_id)
    {
      CM_DLLQ_DELETE_NODE (pDll, pNode);
      of_free (pInstance->Map.key_p);
      of_free (pInstance);

      return OF_SUCCESS;
    }
  }
  return OF_FAILURE;

}
#endif
/**************************************************************************
 Function Name : cmi_dm_add_instance_map_entry_using_id_path
 Input Args    : id_path_p - Pointer to DM ID Path
                : pMap - Pointer to InstanceMap Structure
 Output Args   : None
 Description   : This API Adds New Instance Entry into Data Model Instance Tree
                : using ID path
 Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_add_instance_map_entry_using_id_path (struct cm_dm_id_path * id_path_p,
                                               struct cm_dm_instance_map * pMap)
{
  UCMDM_Inode_t *pInode;
  UCMDM_Instance_t *pInstanceEntry;

  if (unlikely ((id_path_p == NULL) || (pMap == NULL)))
  {
    return OF_FAILURE;
  }

  if (unlikely (id_path_p->no_of_path_elements == 0))
  {
    return OF_FAILURE;
  }

  /* First get the Inode matching the path */
  pInode = cmi_dm_get_inode_by_id_path (id_path_p);
  if (unlikely ((pInode == NULL) || (pInode->pDMNode == NULL)))
  {
    return OF_FAILURE;
  }

  /* Check if there is any duplicate, using Instance ID */
  pInstanceEntry = cmi_dm_get_instance_using_id (pInode, pMap->instance_id);
  if (unlikely (pInstanceEntry != NULL))
  {
    /* Duplicate entry found */
    return OF_FAILURE;
  }

  /* Check if there is any duplicate, using application key */
  pInstanceEntry = cmi_dm_get_instance_using_key (pInode,
                                              pMap->key_p, pMap->key_type);
  if (unlikely (pInstanceEntry != NULL))
  {
    /* Duplicate entry found */
    return OF_FAILURE;
  }

  return cmi_dm_add_instance_map_to_inode (pInode, pMap);
}

/**************************************************************************
 Function Name : cmi_dm_get_instance_map_entry_using_name_path_and_key 
 Input Args    : node_path_p - Pointer to DM Node Path
                : pMap - Pointer to InstanceMap Structure
 Output Args   : None
 Description   : This API Adds New Instance Entry into Data Model Instance Tree
                : using Name path and Key
 Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_get_instance_map_entry_using_name_path_and_key (char * node_path_p,
                                                       struct cm_dm_instance_map *
                                                       pMap)
{
  UCMDM_Inode_t *pInode;
  UCMDM_Instance_t *pInstanceEntry;

  if (unlikely ((node_path_p == NULL) || (pMap == NULL)))
  {
    return OF_FAILURE;
  }

  /* Fixme : We can accept key also as part of the NodePath,
     and separate it here, and use it. Caller would be more happy
     with that kind of support from this API. 

     Right now, we only accept the key of the instance via 
     pMap structure only.
   */
  if (unlikely (node_path_p[of_strlen (node_path_p) - 1] == '}'))
  {
    /* We can not create an instance for an instance */
    return OF_FAILURE;
  }

  /* First get the Inode that matches the path */
  pInode = cmi_dm_get_inode_by_name_path (node_path_p);
  if (unlikely ((pInode == NULL) || (pInode->pDMNode == NULL)))
  {
    return OF_FAILURE;
  }

  pInstanceEntry = cmi_dm_get_instance_using_key (pInode,
                                              pMap->key_p, pMap->key_type);
  if (unlikely (pInstanceEntry == NULL))
  {
    return OF_FAILURE;
  }

  of_memcpy (pMap, &(pInstanceEntry->Map), sizeof (struct cm_dm_instance_map));
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cmi_dm_get_instance_map_entry_using_name_path_and_inst_id 
 Input Args    : node_path_p - Pointer to DM Instance Path
 In/Out Args   : pMap - Pointer to InstanceMap Structure
 Output Args   : None
 Description   : This API Gets Instance Entry into Data Model Instance Tree
                : using DM Instance Name path
 Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_get_instance_map_entry_using_name_path_and_inst_id (char * node_path_p,
                                                          struct cm_dm_instance_map *
                                                          pMap)
{
  UCMDM_Inode_t *pInode;
  UCMDM_Instance_t *pInstanceEntry;

  if (unlikely ((node_path_p == NULL) || (pMap == NULL)))
  {
    return OF_FAILURE;
  }

  /* Fixme : We can accept key also as part of the NodePath,
     and separate it here, and use it. Caller would be more happy
     with that kind of support from this API. 
   */
  if (unlikely (node_path_p[of_strlen (node_path_p) - 1] == '}'))
  {
    /* We can not create an instance for an instance */
    return OF_FAILURE;
  }

  /* First get the Inode matching the path */
  pInode = cmi_dm_get_inode_by_name_path (node_path_p);
  if (unlikely ((pInode == NULL) || (pInode->pDMNode == NULL)))
  {
    return OF_FAILURE;
  }

  pInstanceEntry = cmi_dm_get_instance_using_id (pInode, pMap->instance_id);
  if (unlikely (pInstanceEntry == NULL))
  {
    return OF_FAILURE;
  }

  of_memcpy (pMap, &(pInstanceEntry->Map), sizeof (struct cm_dm_instance_map));
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : UCMDM_GetInstanceMapEntry_UsingIDPathAndKey
 Input Args    : node_path_p - Pointer to DM Instance Path
 In/Out Args   : pMap - Pointer to InstanceMap Structure
 Output Args   : None
 Description   : This API Gets Instance Entry into Data Model Instance Tree
                : using DM ID path and Key
 Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_get_instance_map_entry_using_id_path_and_key (struct cm_dm_id_path * id_path_p,
                                                     struct cm_dm_instance_map * pMap)
{
  UCMDM_Inode_t *pInode;
  UCMDM_Instance_t *pInstanceEntry;

  if (unlikely ((id_path_p == NULL) || (pMap == NULL)))
  {
    return OF_FAILURE;
  }

  if (unlikely (id_path_p->no_of_path_elements == 0))
  {
    return OF_FAILURE;
  }

  /* First get the Inode matching the path */
  pInode = cmi_dm_get_inode_by_id_path (id_path_p);
  if (unlikely ((pInode == NULL) || (pInode->pDMNode == NULL)))
  {
    return OF_FAILURE;
  }

  pInstanceEntry = cmi_dm_get_instance_using_key (pInode,
                                              pMap->key_p, pMap->key_type);
  if (unlikely (pInstanceEntry == NULL))
  {
    return OF_FAILURE;
  }

  of_memcpy (pMap, &(pInstanceEntry->Map), sizeof (struct cm_dm_instance_map));
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cmi_dm_get_instance_map_entry_using_id_path_and_inst_id
 Input Args    : node_path_p - Pointer to DM Instance Path
 In/Out Args   : pMap - Pointer to InstanceMap Structure
 Output Args   : None
 Description   : This API Gets Instance Entry into Data Model Instance Tree
                : using DM ID path and Instance ID
 Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_get_instance_map_entry_using_id_path_and_inst_id (struct cm_dm_id_path *
                                                        id_path_p,
                                                        struct cm_dm_instance_map *
                                                        pMap)
{
  UCMDM_Inode_t *pInode;
  UCMDM_Instance_t *pInstanceEntry;

  if (unlikely ((id_path_p == NULL) || (pMap == NULL)))
  {
    return OF_FAILURE;
  }

  if (unlikely (id_path_p->no_of_path_elements == 0))
  {
    return OF_FAILURE;
  }

  /* First get the Inode matching the path */
  pInode = cmi_dm_get_inode_by_id_path (id_path_p);
  if (unlikely ((pInode == NULL) || (pInode->pDMNode == NULL)))
  {
    return OF_FAILURE;
  }

  pInstanceEntry = cmi_dm_get_instance_using_id (pInode, pMap->instance_id);
  if (unlikely (pInstanceEntry == NULL))
  {
    return OF_FAILURE;
  }

  of_memcpy (pMap, &(pInstanceEntry->Map), sizeof (struct cm_dm_instance_map));
  return OF_SUCCESS;
}

#if 0
/**************************************************************************
 Function Name :
 Input Args    :
 Output Args   :
 Description   :
 Return Values :
 *************************************************************************/
int32_t cm_dm_set_instance_map_Attribs_UsingNamePathAndInstID (char * node_path_p,
                                                            struct cm_dm_instance_map
                                                            * pMap)
{
  UCMDM_Inode_t *pInode;
  UCMDM_Instance_t *pInstanceEntry;

  if (unlikely ((node_path_p == NULL) || (pMap == NULL)))
  {
    return OF_FAILURE;
  }

  /* Fixme : We can accept key also as part of the NodePath,
     and separate it here, and use it. Caller would be more happy
     with that kind of support from this API. 
   */
  if (unlikely (node_path_p[of_strlen (node_path_p) - 1] == '}'))
  {
    /* We can not create an instance for an instance */
    return OF_FAILURE;
  }

  /* First get the Inode that matches the path */
  pInode = cmi_dm_get_inode_by_name_path (node_path_p);
  if (unlikely ((pInode == NULL) || (pInode->pDMNode == NULL)))
  {
    return OF_FAILURE;
  }

  pInstanceEntry = cmi_dm_get_instance_using_id (pInode, pMap->instance_id);
  if (unlikely (pInstanceEntry == NULL))
  {
    return OF_FAILURE;
  }

  of_memcpy (&(pInstanceEntry->Map.Attrib), pMap,
            sizeof (cm_dm_instance_attrib_t));
  return OF_SUCCESS;

}

/**************************************************************************
 Function Name :
 Input Args    :
 Output Args   :
 Description   :
 Return Values :
 *************************************************************************/
int32_t cm_dm_set_instance_map_Attribs_UsingNamePathAndInstKey (char *
                                                             node_path_p,
                                                             struct cm_dm_instance_map
                                                             * pMap)
{
  UCMDM_Inode_t *pInode;
  UCMDM_Instance_t *pInstanceEntry;

  if (unlikely ((node_path_p == NULL) || (pMap == NULL)))
  {
    return OF_FAILURE;
  }

  /* Fixme : We can accept key also as part of the NodePath,
     and separate it here, and use it. Caller would be more happy
     with that kind of support from this API. 
   */
  if (unlikely (node_path_p[of_strlen (node_path_p) - 1] == '}'))
  {
    /* We can not create an instance for an instance */
    return OF_FAILURE;
  }

  /* First get the Inode that matches the path */
  pInode = cmi_dm_get_inode_by_name_path (node_path_p);
  if (unlikely ((pInode == NULL) || (pInode->pDMNode == NULL)))
  {
    return OF_FAILURE;
  }

  pInstanceEntry = cmi_dm_get_instance_using_key (pInode,
                                              pMap->key_p, pMap->key_type);
  if (unlikely (pInstanceEntry == NULL))
  {
    return OF_FAILURE;
  }

  of_memcpy (&(pInstanceEntry->Map.Attrib), pMap,
            sizeof (cm_dm_instance_attrib_t));
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name :
 Input Args    :
 Output Args   :
 Description   :
 Return Values :
 *************************************************************************/
int32_t cm_dm_set_instance_map_Attribs_UsingIDPathAndInstID (struct cm_dm_id_path *
                                                          id_path_p,
                                                          struct cm_dm_instance_map *
                                                          pMap)
{
  UCMDM_Inode_t *pInode;
  UCMDM_Instance_t *pInstanceEntry;

  if (unlikely ((id_path_p == NULL) || (pMap == NULL)))
  {
    return OF_FAILURE;
  }

  if (unlikely (id_path_p->no_of_path_elements == 0))
  {
    return OF_FAILURE;
  }

  /* First get the Inode matching the path */
  pInode = cmi_dm_get_inode_by_id_path (id_path_p);
  if (unlikely ((pInode == NULL) || (pInode->pDMNode == NULL)))
  {
    return OF_FAILURE;
  }

  pInstanceEntry = cmi_dm_get_instance_using_id (pInode, pMap->instance_id);
  if (unlikely (pInstanceEntry == NULL))
  {
    return OF_FAILURE;
  }

  of_memcpy (&(pInstanceEntry->Map.Attrib), pMap,
            sizeof (cm_dm_instance_attrib_t));
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name :
 Input Args    :
 Output Args   :
 Description   :
 Return Values :
 *************************************************************************/
int32_t cm_dm_set_instance_map_Attribs_UsingIDPathAndInstKey (struct cm_dm_id_path *
                                                           id_path_p,
                                                           struct cm_dm_instance_map *
                                                           pMap)
{
  UCMDM_Inode_t *pInode;
  UCMDM_Instance_t *pInstanceEntry;

  if (unlikely ((id_path_p == NULL) || (pMap == NULL)))
  {
    return OF_FAILURE;
  }

  if (unlikely (id_path_p->no_of_path_elements == 0))
  {
    return OF_FAILURE;
  }

  /* First get the Inode matching the path */
  pInode = cmi_dm_get_inode_by_id_path (id_path_p);
  if (unlikely ((pInode == NULL) || (pInode->pDMNode == NULL)))
  {
    return OF_FAILURE;
  }

  pInstanceEntry = cmi_dm_get_instance_using_key (pInode,
                                              pMap->key_p, pMap->key_type);
  if (unlikely (pInstanceEntry == NULL))
  {
    return OF_FAILURE;
  }

  of_memcpy (&(pInstanceEntry->Map.Attrib), pMap,
            sizeof (cm_dm_instance_attrib_t));
  return OF_SUCCESS;
}
#endif

/* Internal routines */

/**************************************************************************
 Function Name : cmi_dm_create_root_instance 
 Input Args    : None
 Output Args   : None
 Description   : This API Creates Data Model Tree Root Instance and initializes
 Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/

int32_t cmi_dm_create_root_instance (void)
{
  pDMInstTree = (UCMDM_Inode_t *) of_calloc (1, sizeof (UCMDM_Inode_t));
  if (unlikely (pDMInstTree == NULL))
  {
    return OF_FAILURE;
  }

  CM_DLLQ_INIT_LIST (&(pDMInstTree->ChildAccess.InstanceList));
  CM_DLLQ_INIT_NODE (&(pDMInstTree->list_node));
  /* Fixme : is this reference really needed? */
  pDMInstTree->pDMNode = cm_dm_get_root_node ();
  pDMInstTree->uiNextFreeID = 1;
  pDMInstTree->bIDWrapped = FALSE;

  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cmi_dm_get_inode_by_name_path 
 Input Args    : name_path_p - Pointer to Data Model Instance Path
 Output Args   : None
 Description   : This API Gets Instance Node using Data Model Instance Path
 Return Values : Pointer to Instance Node or NULL
 *************************************************************************/
UCMDM_Inode_t *cmi_dm_get_inode_by_name_path (char * name_path_p)
{
  struct cm_dm_data_element *pDMNode;
  UCMDM_Inode_t *pInode;
  UCMDM_Inode_t *parent_inode_p;
  char *pToken;
  uint32_t uiMaxTokenLen;
  uint32_t uindex_i = 0;
  struct cm_dm_data_element *pNode;
  UCMDllQ_t *pChildInodeList = NULL;
  char *key_p;
  char *pBadKey;
  char *pEndOfKey;
  UCMDM_Instance_t *pInstance = NULL;
  unsigned char bPrevTokenIsTable;
  //uint32_t key_type;
  void *KeyVal = NULL;
  uint32_t uiParentType = UCMDM_INODE_PARENT_TYPE_INODE;
  uint32_t return_value = UCMDM_PATH_PARSING_REMAINING;
  //uint32_t i = 0;

	struct cm_array_of_nv_pair KeysArray={};
	uint32_t uiKeyCnt=0, ii, uiNvIndex;


  if (unlikely (name_path_p == NULL))
  {
    return NULL;
  }

  if ((!name_path_p) || (of_strlen (name_path_p) == 0))
    //     || (name_path_p[0] != '/'))
  {
    return NULL;
  }

  uiMaxTokenLen = of_strlen (name_path_p) + 1;

  /* Allocate token buffer */
  pToken = (char *) of_calloc (1, uiMaxTokenLen);
  if (!pToken)
  {
    return NULL;
  }
  /* Verify whether the data path is valid accroding to DMTree */
  if (cmi_dm_remove_keys_from_path (name_path_p, pToken, of_strlen (name_path_p)) !=
      OF_SUCCESS)
  {
    of_free (pToken);
    return NULL;
  }
  /* Get DM Node */
  pDMNode = cmi_dm_get_node_by_namepath (pToken);
  if (unlikely (pDMNode == NULL))
  {
    of_free (pToken);
    return NULL;
  }

  /* Tokenize the input string */
  pInode = pDMInstTree;
  pChildInodeList = &(pInode->ChildAccess.ChildInodeList);
  bPrevTokenIsTable = FALSE;
  of_memset (pToken, 0, uiMaxTokenLen);
  while (return_value == UCMDM_PATH_PARSING_REMAINING)
  {
    return_value = cmi_dm_get_path_token (name_path_p, pToken, uiMaxTokenLen, &uindex_i);
    CM_DM_DEBUG_PRINT ("token : %s", pToken);

    /* Move on to matching Inode */
    if (bPrevTokenIsTable == FALSE)
    {
      parent_inode_p = pInode;
      pInode = cmi_dm_get_child_inode_by_name (pChildInodeList, pToken);
      if (!pInode)
      {
        CM_DM_DEBUG_PRINT ("Creating new node : %s", pToken);
        /* Time to create a brand new Inode */
        pInode = (UCMDM_Inode_t *) of_calloc (1, sizeof (UCMDM_Inode_t));
        if (unlikely (pInode == NULL))
        {
          break;
        }
        CM_DLLQ_INIT_LIST (&(pInode->ChildAccess.InstanceList));
        CM_DLLQ_INIT_NODE (&(pInode->list_node));

        pInode->pDMNode = cmi_dm_get_dm_node_from_instance_path (name_path_p, uindex_i);
        if (unlikely (pInode->pDMNode == NULL))
        {
          CM_DM_DEBUG_PRINT ("Unable to get DM node for token : %s", pToken);
          of_free (pInode);
          pInode = NULL;
          break;
        }
        pInode->uiNextFreeID = 1;
        pInode->bIDWrapped = FALSE;

        /* Setup back pointer to parent. Useful for traversing */
        pInode->uiParentType = uiParentType;
        if (uiParentType == UCMDM_INODE_PARENT_TYPE_INSTANCE)
        {
          pInode->ParentAccess.parent_instance_p = pInstance;
        }
        else
        {
          pInode->ParentAccess.parent_inode_p = parent_inode_p;
        }
        CM_DLLQ_APPEND_NODE (pChildInodeList, &(pInode->list_node));
      }

      pChildInodeList = &(pInode->ChildAccess.ChildInodeList);
      if (pInode->pDMNode->element_attrib.element_type == CM_DMNODE_TYPE_TABLE)
      {
        bPrevTokenIsTable = TRUE;
      }
      else
      {
        bPrevTokenIsTable = FALSE;
      }

      uiParentType = UCMDM_INODE_PARENT_TYPE_INODE;
    }
    else
    {
      /* This token must be an application key */
      if (pToken[0] == '{')
      {
				uiKeyCnt=0;
        key_p = pToken + 1;

        /* Just a safety check. we dont expect more keys in one token */
        pBadKey = of_strchr (key_p, '{');
        if (pBadKey != NULL)
        {
          pNode = NULL;
          break;
        }

        /* Find the end of the key */
        pEndOfKey = of_strchr (key_p, '}');
        if (unlikely (pEndOfKey == NULL))
        {
          pNode = NULL;
          break;
        }

        /* Just a safety check. we dont expect corrupted key in any token */
        pBadKey = of_strchr (pEndOfKey + 1, '}');
        if (pBadKey != NULL)
        {
          pNode = NULL;
          break;
        }

        *pEndOfKey = '\0';

        /* Using key, Find the matching child instance */

				ii=0;
				while ((key_p[ii] != '}')&&(key_p[ii] != '\0'))
				{
					 if(key_p[ii] == ',')
					 {
							uiKeyCnt++;
					 }
					 ii++;
				}
				if (ii > 0 )
				{
					 uiKeyCnt++;
				}
				KeysArray.count_ui=uiKeyCnt;
				uiNvIndex=0;
				KeysArray.nv_pairs=(struct cm_nv_pair *)of_calloc(KeysArray.count_ui, sizeof(struct cm_nv_pair));
				cmi_dm_get_key_array_from_namepathToken(key_p,pInode->pDMNode, &KeysArray, &uiNvIndex);
				pInstance=cmi_dm_get_instance_using_keyArray(pInode, &KeysArray);
				CM_FREE_NVPAIR_ARRAY(KeysArray, KeysArray.count_ui);
#if 0
        key_type = cmi_dm_get_key_child_datatype (pInode->pDMNode);
        switch (key_type)
        {
          case CM_DATA_TYPE_INT:
          case CM_DATA_TYPE_UINT:
//#if 0
            /* Changed by Atmaram */
            i = of_atoi (key_p);
            KeyVal = (void *) & i;
            pInstance = cmi_dm_get_instance_using_key (pInode, KeyVal, key_type);
            break;
//#endif
          case CM_DATA_TYPE_STRING:

            KeyVal = (void *) of_calloc (1, of_strlen (key_p) + 1);
            if (!KeyVal)
            {
              CM_DM_DEBUG_PRINT ("Memory Allocation Failed");
              /* De allocate memory */
              break;
            }
            of_memcpy (KeyVal, key_p, of_strlen (key_p));
            pInstance = cmi_dm_get_instance_using_key (pInode, KeyVal, key_type);
            of_free (KeyVal);
            break;
          default:
            of_free (pToken);
            return NULL;
        }
#endif
      }
      else
      {
        /* This is a TR069 type of Key (integer) */
        key_p = pToken;
        KeyVal = (void *) of_atoi (pToken);
        pInstance = cmi_dm_get_instance_using_id (pInode, (uint32_t) KeyVal);
      }

			if (!pInstance)
			{
				 CM_DM_DEBUG_PRINT ("Instance is null");
				 pInode = NULL;
				 break;
			}

      bPrevTokenIsTable = FALSE;
      uiParentType = UCMDM_INODE_PARENT_TYPE_INSTANCE;

      /* Get the child Inode list in handy. Useful for next token */
      pChildInodeList = &(pInstance->ChildInodeList);
    }

    if (return_value == UCMDM_PATH_PARSING_COMPLETED)
    {
      CM_DM_DEBUG_PRINT ("parsing completed");
      break;
    }
  }

  of_free (pToken);
  return pInode;
}

/**************************************************************************
 Function Name : cmi_dm_get_inode_or_instance_node_by_name_path 
 Description   : This API Gets Instance or Inode using Data Model Instance Path
 Input Args    : name_path_p - Pointer to Data Model Instance Path
 Output Args   : pnode_type_ui - Pointer to Node Type Inode or Instance None
               : pOutNode    - Pointer to Output NOde.
 Return Values : Pointer to Instance Node or NULL
 *************************************************************************/
int32_t cmi_dm_get_inode_or_instance_node_by_name_path (char * name_path_p,
                                                int32_t * pnode_type_ui,
                                                void ** pOutNode)
{
  struct cm_dm_data_element *pDMNode;
  UCMDM_Inode_t *pInode;
  UCMDM_Inode_t *parent_inode_p;
  char *pToken;
  uint32_t uiMaxTokenLen;
  uint32_t uindex_i = 0;
  struct cm_dm_data_element *pNode;
  UCMDllQ_t *pChildInodeList = NULL;
  char *key_p;
  char *pBadKey;
  char *pEndOfKey;
  UCMDM_Instance_t *pInstance = NULL;
  unsigned char bPrevTokenIsTable;
  uint32_t key_type;
  void *KeyVal = NULL;
  uint32_t uiParentType = UCMDM_INODE_PARENT_TYPE_INODE;
  uint32_t return_value = UCMDM_PATH_PARSING_REMAINING;
  //uint32_t i = 0;

  if (unlikely (name_path_p == NULL))
  {
    return OF_FAILURE;
  }

  if ((!name_path_p) || (of_strlen (name_path_p) == 0))
    //     || (name_path_p[0] != '/'))
  {
    return OF_FAILURE;
  }

  uiMaxTokenLen = of_strlen (name_path_p) + 1;

  /* Allocate token buffer */
  pToken = (char *) of_calloc (1, uiMaxTokenLen);
  if (!pToken)
  {
    return OF_FAILURE;
  }
  /* Verify whether the data path is valid accroding to DMTree */
  if (cmi_dm_remove_keys_from_path (name_path_p, pToken, of_strlen (name_path_p)) !=
      OF_SUCCESS)
  {
    of_free (pToken);
    return OF_FAILURE;
  }
  /* Get DM Node */
  pDMNode = cmi_dm_get_node_by_namepath (pToken);
  if (unlikely (pDMNode == NULL))
  {
    of_free (pToken);
    return OF_FAILURE;
  }

  /* Tokenize the input string */
  pInode = pDMInstTree;
  pChildInodeList = &(pInode->ChildAccess.ChildInodeList);
  bPrevTokenIsTable = FALSE;
  of_memset (pToken, 0, uiMaxTokenLen);
  while (return_value == UCMDM_PATH_PARSING_REMAINING)
  {
    return_value = cmi_dm_get_path_token (name_path_p, pToken, uiMaxTokenLen, &uindex_i);
    CM_DM_DEBUG_PRINT (" token : %s", pToken);

    /* Move on to matching Inode */
    if (bPrevTokenIsTable == FALSE)
    {
      parent_inode_p = pInode;
      pInode = cmi_dm_get_child_inode_by_name (pChildInodeList, pToken);
      if (!pInode)
      {
        CM_DM_DEBUG_PRINT ("Inode not found for token %s", pToken);
        of_free (pToken);
        return OF_FAILURE;
#if 0
        CM_DM_DEBUG_PRINT ("Creating new node : %s", pToken);
        /* Time to create a brand new Inode */
        pInode = (UCMDM_Inode_t *) of_calloc (1, sizeof (UCMDM_Inode_t));
        if (unlikely (pInode == NULL))
        {
          break;
        }
        CM_DLLQ_INIT_LIST (&(pInode->ChildAccess.InstanceList));
        CM_DLLQ_INIT_NODE (&(pInode->list_node));

        pInode->pDMNode = cmi_dm_get_dm_node_from_instance_path (name_path_p, uindex_i);
        if (unlikely (pInode->pDMNode == NULL))
        {
          CM_DM_DEBUG_PRINT ("Unable to get DM node for token : %s", pToken);
          of_free (pInode);
          pInode = NULL;
          break;
        }
        pInode->uiNextFreeID = 1;
        pInode->bIDWrapped = FALSE;

        /* Setup back pointer to parent. Useful for traversing */
        pInode->uiParentType = uiParentType;
        if (uiParentType == UCMDM_INODE_PARENT_TYPE_INSTANCE)
        {
          pInode->ParentAccess.parent_instance_p = pInstance;
        }
        else
        {
          pInode->ParentAccess.parent_inode_p = parent_inode_p;
        }
        CM_DLLQ_APPEND_NODE (pChildInodeList, &(pInode->list_node));
#endif
      }

      pChildInodeList = &(pInode->ChildAccess.ChildInodeList);
      if (pInode->pDMNode->element_attrib.element_type == CM_DMNODE_TYPE_TABLE)
      {
        bPrevTokenIsTable = TRUE;
      }
      else
      {
        bPrevTokenIsTable = FALSE;
      }

      uiParentType = UCMDM_INODE_PARENT_TYPE_INODE;
    }
    else
    {
      /* This token must be an application key */
      if (pToken[0] == '{')
      {
        key_p = pToken + 1;

        /* Just a safety check. we dont expect more keys in one token */
        pBadKey = of_strchr (key_p, '{');
        if (pBadKey != NULL)
        {
          pNode = NULL;
          break;
        }

        /* Find the end of the key */
        pEndOfKey = of_strchr (key_p, '}');
        if (unlikely (pEndOfKey == NULL))
        {
          pNode = NULL;
          break;
        }

        /* Just a safety check. we dont expect corrupted key in any token */
        pBadKey = of_strchr (pEndOfKey + 1, '}');
        if (pBadKey != NULL)
        {
          pNode = NULL;
          break;
        }

        *pEndOfKey = '\0';

        /* Using key, Find the matching child instance */
        key_type = cmi_dm_get_key_child_datatype (pInode->pDMNode);
        switch (key_type)
        {
          case CM_DATA_TYPE_INT:
          case CM_DATA_TYPE_UINT:
          case CM_DATA_TYPE_INT64:
          case CM_DATA_TYPE_UINT64:
            /* Changed by Atmaram */
         //   i = of_atoi (key_p);
           // KeyVal = (void *) & i;
            //pInstance = cmi_dm_get_instance_using_key (pInode, KeyVal, key_type);
            pInstance = cmi_dm_get_instance_using_key (pInode, key_p, key_type);
            break;
          case CM_DATA_TYPE_STRING:

            KeyVal = (void *) of_calloc (1, of_strlen (key_p) + 1);
            if (!KeyVal)
            {
              CM_DM_DEBUG_PRINT ("Memory Allocation Failed");
              /* De allocate memory */
              break;
            }
            of_memcpy (KeyVal, key_p, of_strlen (key_p));
            pInstance = cmi_dm_get_instance_using_key (pInode, KeyVal, key_type);
            of_free (KeyVal);
            break;
          default:
            of_free (pToken);
            return OF_FAILURE;
        }

      }
      else
      {
        /* This is a TR069 type of Key (integer) */
        key_p = pToken;
        KeyVal = (void *) of_atoi (pToken);
        pInstance = cmi_dm_get_instance_using_id (pInode, (uint32_t) KeyVal);
      }

      if (!pInstance)
      {
        pInode = NULL;
        break;
      }

      bPrevTokenIsTable = FALSE;
      uiParentType = UCMDM_INODE_PARENT_TYPE_INSTANCE;

      /* Get the child Inode list in handy. Useful for next token */
      pChildInodeList = &(pInstance->ChildInodeList);
    }

    if (return_value == UCMDM_PATH_PARSING_COMPLETED)
    {
      CM_DM_DEBUG_PRINT ("parsing completed");
      break;
    }
  }

  if (uiParentType == UCMDM_INODE_PARENT_TYPE_INSTANCE)
  {
    *pOutNode = (void *) pInstance;
    *pnode_type_ui = UCMDM_INODE_PARENT_TYPE_INSTANCE;
  }
  else if (uiParentType == UCMDM_INODE_PARENT_TYPE_INODE)
  {
    *pOutNode = (void *) pInode;
    *pnode_type_ui = UCMDM_INODE_PARENT_TYPE_INODE;
  }
  of_free (pToken);
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cmi_dm_get_inode_by_id_path
 Input Args    : id_path_p - Pointer to Data Model ID Path
 Output Args   : None
 Description   : This API Gets Instance Node using Data Model ID Path
 Return Values : Pointer to Instance Node or NULL
 *************************************************************************/
UCMDM_Inode_t *cmi_dm_get_inode_by_id_path (struct cm_dm_id_path * id_path_p)
{
  struct cm_dm_data_element *pDMNode;
  UCMDM_Inode_t *pInode;
  UCMDM_Inode_t *parent_inode_p;
  struct cm_array_of_uints IDArray;
  uint32_t *uint_arr_p;
  uint32_t i;
  UCMDllQ_t *pChildInodeList = NULL;
  UCMDM_Instance_t *pInstance;

  CM_DM_DEBUG_PRINT ("entry");

  if (unlikely (id_path_p == NULL))
  {
    return NULL;
  }

  uint_arr_p =
    (uint32_t *) of_calloc (id_path_p->no_of_path_elements, sizeof (uint32_t));
  if (unlikely (uint_arr_p == NULL))
  {
    return NULL;
  }
  for (i = 0; i < id_path_p->no_of_path_elements; i++)
  {
    uint_arr_p[i] = id_path_p->id_path_arr_p[i].element_id;
  }

  IDArray.count_ui = id_path_p->no_of_path_elements;
  IDArray.uint_arr_p = uint_arr_p;

  /* Verify whether the data path is valid accroding to DMTree */
  pDMNode = cmi_dm_get_node_by_idpath (&IDArray);
  if (unlikely (pDMNode == NULL))
  {
    of_free (uint_arr_p);
    return NULL;
  }

  if (unlikely
      (id_path_p->id_path_arr_p[0].element_id != pDMInstTree->pDMNode->id_ui))
  {
    of_free (uint_arr_p);
    return NULL;
  }

  /* Our root is confirmed. Start with root Inode */
  pInode = pDMInstTree;
  pChildInodeList = &(pInode->ChildAccess.ChildInodeList);

  for (i = 1; i < id_path_p->no_of_path_elements; i++)
  {
    parent_inode_p = pInode;
    pInode =
      cmi_dm_get_child_inode_by_mid (pChildInodeList,
                                 id_path_p->id_path_arr_p[i].element_id);
    if (!pInode)
    {
      CM_DM_DEBUG_PRINT ("Creating new node : %u",
                          id_path_p->id_path_arr_p[i].element_id);

      /* Time to create a brand new Inode */
      pInode = (UCMDM_Inode_t *) of_calloc (1, sizeof (UCMDM_Inode_t));
      if (unlikely (pInode == NULL))
      {
        break;
      }
      CM_DLLQ_INIT_LIST (&(pInode->ChildAccess.InstanceList));
      CM_DLLQ_INIT_NODE (&(pInode->list_node));

      /* Get the DM Node based on partial path */
      IDArray.count_ui = i + 1;
      IDArray.uint_arr_p = uint_arr_p;
      pInode->pDMNode = cmi_dm_get_node_by_idpath (&IDArray);
      if (unlikely (pInode->pDMNode == NULL))
      {
        CM_DM_DEBUG_PRINT
          ("Unable to get DM node for ID : %u",
           id_path_p->id_path_arr_p[i].element_id);
        of_free (pInode);
        pInode = NULL;
        break;
      }

      pInode->uiNextFreeID = 1;
      pInode->bIDWrapped = FALSE;

      CM_DLLQ_APPEND_NODE (pChildInodeList, &(pInode->list_node));
    }

    /* For a table node, Make sure that we have a key */
    if (pInode->pDMNode->element_attrib.element_type == CM_DMNODE_TYPE_TABLE)
    {
      if (unlikely
          (id_path_p->id_path_arr_p[i].instance_key_type == CM_DATA_TYPE_UNKNOWN))
      {
        pInode = NULL;
        break;
      }

      /* Child Inodes should be accessed via instance map entries.
         So Now Get the instance map entry based on key. 
         Note : Just use the id_path_p->id_path_arr_p[i].instance_key.string_key_p,
         as it also points to UINT key.
       */
      pInstance = cmi_dm_get_instance_using_key (pInode,
                                             id_path_p->id_path_arr_p[i].instance_key.
                                             string_key_p,
                                             id_path_p->id_path_arr_p[i].
                                             instance_key_type);
      if (!pInstance)
      {
        pInode = NULL;
        break;
      }
      pChildInodeList = &(pInstance->ChildInodeList);
    }
    else
    {
      /* Child Inodes are directly accessible from parent Inode */
      pChildInodeList = &(pInode->ChildAccess.ChildInodeList);
    }
  }

  of_free (uint_arr_p);
  return pInode;
}

/**************************************************************************
 Function Name : cmi_dm_get_dm_node_from_instance_path
 Input Args    : name_path_p - Pointer to Data Model Path
                : uiValidPathLen - Lengh of DM Path 
 Output Args   : None
 Description   : This API Gets DM Node using Data Model Path and Path Length
 Return Values : Pointer to DM Node or NULL
 *************************************************************************/
struct cm_dm_data_element *cmi_dm_get_dm_node_from_instance_path (char * name_path_p,
                                                      uint32_t uiValidPathLen)
{
  char *dm_path_p;
  struct cm_dm_data_element *pDMNode;

  dm_path_p = (char *) of_calloc (1, uiValidPathLen + 1);
  if (unlikely (!dm_path_p))
  {
    return NULL;
  }

	if (cmi_dm_remove_keys_from_path (name_path_p, dm_path_p, uiValidPathLen) != OF_SUCCESS)
	{
		 CM_DM_DEBUG_PRINT("cmi_dm_remove_keys_from_path Failed");
		 of_free (dm_path_p);
		 return NULL;
	}
	pDMNode = cmi_dm_get_node_by_namepath (dm_path_p);
  of_free (dm_path_p);
  return pDMNode;
}
/**
  \ingroup DMAPI
	This API Deletes a Instance Node from DM Instance Tree using Key Value and Value Type.
 
 	<b>Input paramameters: </b>\n
  <b><i>pInode:</i></b> Pointer to Data Model Inode.
  <b><i>key_p:</i></b> Pointer to Key Value in DM Instance Tree
  <b><i>key_type:</i></b> Key Value Type
  <b>Output Parameters: </b> None \n
  <b>Return Value:</b> OF_SUCCESS in Success
    OF_FAILURE in Failure case.\n
 */
/**************************************************************************
 Function Name : cmi_dm_delete_instance_using_key
 Description   : This API Deletes a Instance Node from DM Instance Tree using
								 Key Value and Value Type 
 Input Args    : pInode  - Pointer to DM Inode
               : key_p - Pointer Key Value in Instance Tree
							 : key_type - Key Value Type
 Output Args   : None
 Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_delete_instance_using_key (UCMDM_Inode_t * pInode,
                                      void * key_p, uint32_t key_type)
{
  UCMDllQNode_t *pNode, *pTmp;
  UCMDllQNode_t *pNode2, *pTmp2;
  UCMDllQ_t *pDll;
  UCMDllQ_t *pDll2;
  UCMDM_Instance_t *pInstance, *parent_instance_p;
  UCMDM_Inode_t *parent_inode_p;
  UCMDM_Inode_t *pInode2;
  ulong instance_id;
  int32_t iKeyID = 0;
	int32_t uiKeyID = 0;
	unsigned char bNodeFound = FALSE;

  if (unlikely (pInode == NULL))
  {
    return OF_FAILURE;
  }

  /* The child Inode must be of a table type */
  if (unlikely ((pInode->pDMNode == NULL) ||
                (pInode->pDMNode->element_attrib.element_type !=
                 CM_DMNODE_TYPE_TABLE)))
  {
    return OF_FAILURE;
  }

  /* 
     Now go through the instance list of this node, and get the 
     matching instance.
   */
  pDll = &(pInode->ChildAccess.InstanceList);
  CM_DLLQ_DYN_SCAN (pDll, pNode, pTmp, UCMDllQNode_t *)
  {
    pInstance = CM_DLLQ_LIST_MEMBER (pNode, UCMDM_Instance_t *, list_node);

    if ((key_type == CM_DATA_TYPE_INT) &&
        (pInstance->Map.key_type == CM_DATA_TYPE_INT))
    {
      instance_id = *(ulong *) key_p;
      /* Changed by Atmaram */
      iKeyID = of_atoi (pInstance->Map.key_p);
      //if (instance_id == iKeyID)
      if (!of_strcmp (((char *) key_p), ((char *) pInstance->Map.key_p)))
      {
        bNodeFound = TRUE;
        break;
      }
    }
    else if ((key_type == CM_DATA_TYPE_UINT) &&
             (pInstance->Map.key_type == CM_DATA_TYPE_UINT))
    {
      /* Changed by Atmaram */
      instance_id = (ulong) key_p;
      uiKeyID = of_atoi (pInstance->Map.key_p);
//      if (instance_id == uiKeyID)
      if (!of_strcmp (((char *) key_p), ((char *) pInstance->Map.key_p)))
      {
        bNodeFound = TRUE;
        break;
      }
    }
    else if ((key_type == CM_DATA_TYPE_INT64) &&
             (pInstance->Map.key_type == CM_DATA_TYPE_INT64))
    {
      //instance_id = *(int64_t *) pKey;
      //iKeyID = of_atol (pInstance->Map.pKey);
      if (!of_strcmp (((char *) key_p), ((char *) pInstance->Map.key_p)))
      {
        bNodeFound = TRUE;
        break;
      }
    }
    else if ((key_type == CM_DATA_TYPE_UINT64) &&
        (pInstance->Map.key_type == CM_DATA_TYPE_UINT64))
    {
      //instance_id = *(uint64_t *)pKey;
      //uiKeyID = of_atol (pInstance->Map.pKey);
      //      if (instance_id == uiKeyID)
      if (!of_strcmp (((char *) key_p), ((char *) pInstance->Map.key_p)))
      {
        bNodeFound = TRUE;
        break;
      }
    }

    else if ((key_type == CM_DATA_TYPE_STRING) &&
             (pInstance->Map.key_type == CM_DATA_TYPE_STRING))
    {
      if (!of_strcmp (((char *) key_p), ((char *) pInstance->Map.key_p)))
      {
        bNodeFound = TRUE;
        break;
      }
    }
    else if ((key_type == CM_DATA_TYPE_IPADDR) &&
             (pInstance->Map.key_type == CM_DATA_TYPE_IPADDR))
    {
      if (!of_strcmp (((char *) key_p), ((char *) pInstance->Map.key_p)))
      {
        bNodeFound = TRUE;
        break;
      }
    }
  }
  if (bNodeFound == TRUE)
	{
		 CM_DLLQ_DELETE_NODE (pDll, (UCMDllQNode_t *) pNode);
#ifdef CM_ROLES_PERM_SUPPORT
		 cmi_dm_delete_perm_list_from_instance(pInstance);
#endif
		 CM_DM_DEBUG_PRINT ("Instance Mapy Entry = %s Deleted successfully", (char *)key_p);
		 of_free (pInstance->Map.key_p);
		 of_free (pInstance);
		 pInstance = NULL;

		 if (CM_DLLQ_COUNT (&(pInode->ChildAccess.InstanceList)) == 0)
    {
      CM_DM_DEBUG_PRINT ("Instance List is zero for Inode.. deleting Inode");
      if (pInode->uiParentType == UCMDM_INODE_PARENT_TYPE_INODE)
      {
        parent_inode_p = pInode->ParentAccess.parent_inode_p;
        pDll2 = &(parent_inode_p->ChildAccess.ChildInodeList);
      }
      else
      {
        parent_instance_p = pInode->ParentAccess.parent_instance_p;
        pDll2 = &(parent_instance_p->ChildInodeList);

      }
      /* Delete Inode from its Parent List */
      CM_DLLQ_DYN_SCAN (pDll2, pNode2, pTmp2, UCMDllQNode_t *)
      {
        pInode2 = CM_DLLQ_LIST_MEMBER (pNode2, UCMDM_Inode_t *, list_node);
        if (pInode2 == pInode)
        {
          //CM_DLLQ_DELETE_NODE( pDll2, (UCMDllQNode_t *)pInode2);
          CM_DLLQ_DELETE_NODE (pDll2, pNode2);
          of_free (pInode2);
          pInode2 = NULL;
          CM_DM_DEBUG_PRINT ("Inode also deleted");
        }
      }
    }
    return OF_SUCCESS;
  }
  return OF_FAILURE;
}

int32_t cmi_dm_delete_instance_using_keyArray (UCMDM_Inode_t * pInode,
                                      struct cm_array_of_nv_pair *key_nv_pairs_p)
{
  UCMDllQNode_t *pNode, *pTmp;
  UCMDllQNode_t *pNode2, *pTmp2;
  UCMDllQ_t *pDll;
  UCMDllQ_t *pDll2;
  UCMDM_Instance_t *pInstance, *parent_instance_p;
  UCMDM_Inode_t *parent_inode_p;
  UCMDM_Inode_t *pInode2;
  unsigned char bNodeFound = FALSE;
	char *key_p, *instance_key_p;
	uint32_t uiKeyCnt, key_type, uiInstanceType, uiInstance;
	unsigned char key_bFound;

  if (unlikely (pInode == NULL))
  {
    return OF_FAILURE;
  }

  /* The child Inode must be of a table type */
  if (unlikely ((pInode->pDMNode == NULL) ||
                (pInode->pDMNode->element_attrib.element_type !=
                 CM_DMNODE_TYPE_TABLE)))
  {
    return OF_FAILURE;
  }

  /* 
     Now go through the instance list of this node, and get the 
     matching instance.
   */
  pDll = &(pInode->ChildAccess.InstanceList);
  CM_DLLQ_DYN_SCAN (pDll, pNode, pTmp, UCMDllQNode_t *)
	{
		 pInstance = CM_DLLQ_LIST_MEMBER (pNode, UCMDM_Instance_t *, list_node);
		 if(pInstance->Map.key_nv_pairs.count_ui == key_nv_pairs_p->count_ui)
		 {
				key_bFound=TRUE;
				for(uiKeyCnt=0; (uiKeyCnt < key_nv_pairs_p->count_ui) && (key_bFound == TRUE); uiKeyCnt++)
				{
					 key_type=key_nv_pairs_p->nv_pairs[uiKeyCnt].value_type;
					 key_p= (char *) key_nv_pairs_p->nv_pairs[uiKeyCnt].value_p;
					 key_bFound=FALSE;
					 for(uiInstance=0; (uiInstance < pInstance->Map.key_nv_pairs.count_ui)  && (key_bFound == FALSE); uiInstance++)
					 {
							uiInstanceType= pInstance->Map.key_nv_pairs.nv_pairs[uiInstance].value_type;
							instance_key_p= pInstance->Map.key_nv_pairs.nv_pairs[uiInstance].value_p;
							if (!of_strcmp ( key_p, instance_key_p))
							{
								 key_bFound=TRUE;
							}

#if 0
							if ((key_type == CM_DATA_TYPE_INT) && 
										(uiInstanceType == CM_DATA_TYPE_INT))
							{
								 if (!of_strcmp ( key_p, instance_key_p))
								 {
										key_bFound=TRUE;
								 }
							}
							else if ((key_type == CM_DATA_TYPE_UINT) &&
										(uiInstanceType == CM_DATA_TYPE_UINT))
							{
								 if (!of_strcmp (( key_p), ( instance_key_p)))
								 {
										key_bFound=TRUE;
								 }
							}
							else if ((key_type == CM_DATA_TYPE_STRING) &&
										(uiInstanceType == CM_DATA_TYPE_STRING))
							{
								 if (!of_strcmp (( key_p), ( instance_key_p)))
								 {
										key_bFound=TRUE;
								 }
							}
							else if ((key_type == CM_DATA_TYPE_IPADDR) &&
										(uiInstanceType == CM_DATA_TYPE_IPADDR))
							{
								 if (!of_strcmp (( key_p), ( instance_key_p)))
								 {
										key_bFound=TRUE;
								 }
							}
#endif
					 }
				}
				if(key_bFound == TRUE)
				{
					 bNodeFound=TRUE;
					 break;
				}
		 }
	}
  if (bNodeFound == TRUE)
	{
		 CM_DLLQ_DELETE_NODE (pDll, (UCMDllQNode_t *) pNode);
#ifdef CM_ROLES_PERM_SUPPORT
		 cmi_dm_delete_perm_list_from_instance(pInstance);
#endif
		 CM_DM_DEBUG_PRINT ("Instance Mapy Entry = %s Deleted successfully", (char *)key_p);
		 of_free (pInstance->Map.key_p);
		 of_free (pInstance);
		 pInstance = NULL;

		 if (CM_DLLQ_COUNT (&(pInode->ChildAccess.InstanceList)) == 0)
    {
      CM_DM_DEBUG_PRINT ("Instance List is zero for Inode.. deleting Inode");
      if (pInode->uiParentType == UCMDM_INODE_PARENT_TYPE_INODE)
      {
        parent_inode_p = pInode->ParentAccess.parent_inode_p;
        pDll2 = &(parent_inode_p->ChildAccess.ChildInodeList);
      }
      else
      {
        parent_instance_p = pInode->ParentAccess.parent_instance_p;
        pDll2 = &(parent_instance_p->ChildInodeList);

      }
      /* Delete Inode from its Parent List */
      CM_DLLQ_DYN_SCAN (pDll2, pNode2, pTmp2, UCMDllQNode_t *)
      {
        pInode2 = CM_DLLQ_LIST_MEMBER (pNode2, UCMDM_Inode_t *, list_node);
        if (pInode2 == pInode)
        {
          //CM_DLLQ_DELETE_NODE( pDll2, (UCMDllQNode_t *)pInode2);
          CM_DLLQ_DELETE_NODE (pDll2, pNode2);
          of_free (pInode2);
          pInode2 = NULL;
          CM_DM_DEBUG_PRINT ("Inode also deleted");
        }
      }
    }
    return OF_SUCCESS;
  }
  return OF_FAILURE;
}
/**
  \ingroup DMAPI
	This API Deletes a Instance Node from DM Instance Tree 
 	<b>Input paramameters: </b>\n
  <b><i>pInstance:</i></b> Pointer to Data Model Inode.
  <b>Output Parameters: </b> None \n
  <b>Return Value:</b> OF_SUCCESS in Success
    OF_FAILURE in Failure case.\n
 */
/**************************************************************************
 Function Name : cmi_dm_delete_instance 
 Description   : This API Deletes a given Instance from DM Instance Tree
 Input Args    : pInstanceNode - Pointer to DM Instance Node in Instance Tree
 Output Args   : None
 Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_delete_instance (UCMDM_Instance_t * pInstanceNode)
{
  UCMDllQNode_t *pNode, *pTmp;
  UCMDllQ_t *pDll;
  UCMDM_Instance_t *pInstance;
  UCMDM_Inode_t *pInode;

  if (unlikely (pInstanceNode == NULL))
  {
    return OF_FAILURE;
  }

  /* The child Inode must be of a table type */
  if (unlikely ((pInstanceNode->pInode == NULL) ||
                (pInstanceNode->pInode->pDMNode == NULL) ||
                (pInstanceNode->pInode->pDMNode->element_attrib.element_type !=
                 CM_DMNODE_TYPE_TABLE)))
  {
    return OF_FAILURE;
  }

  /* 
     Now go through the instance list of this node, and get the 
     matching instance.
   */
  pDll = &(pInstanceNode->pInode->ChildAccess.InstanceList);
  pInode = pInstanceNode->pInode;

  CM_DLLQ_DYN_SCAN (pDll, pNode, pTmp, UCMDllQNode_t *)
  {
    pInstance = CM_DLLQ_LIST_MEMBER (pNode, UCMDM_Instance_t *, list_node);
    if (pInstance == pInstanceNode)
    {
      CM_DLLQ_DELETE_NODE (pDll, (UCMDllQNode_t *) pNode);
      CM_DM_DEBUG_PRINT ("Instance Mapy Entry = %s Deleted successfully",
                          (char *)pInstance->Map.key_p);
#ifdef CM_ROLES_PERM_SUPPORT
			cmi_dm_delete_perm_list_from_instance(pInstance);
#endif
      of_free (pInstance->Map.key_p);
      of_free (pInstance);
      pInstance = NULL;
      return OF_SUCCESS;
    }
  }
  return OF_FAILURE;
}

/* Here Deleting Inode where Inode contains zero instances */
/**
  \ingroup DMAPI
	This API Deletes a Inode from DM Instance Tree 
 	<b>Input paramameters: </b>\n
  <b><i>pInode:</i></b> Pointer to Data Model Inode.
  <b>Output Parameters: </b> None \n
  <b>Return Value:</b> OF_SUCCESS in Success
    OF_FAILURE in Failure case.\n
 */
/**************************************************************************
 Function Name : cmi_dm_delete_inode
 Description   : This API Deletes a given Inode from DM Instance Tree
 Input Args    : pInode - Pointer to DM Inode in Instance Tree
 Output Args   : None
 Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_delete_inode (UCMDM_Inode_t * pInode)
{
  UCMDllQNode_t *pNode, *pTmp;
  UCMDllQ_t *pDll;
  UCMDM_Instance_t *parent_instance_p;
  UCMDM_Inode_t *parent_inode_p, *pInode2;

  if (unlikely (pInode == NULL))
  {
    return OF_FAILURE;
  }

  /* The child Inode must be of a table type */
  if (unlikely (pInode->pDMNode == NULL))
  {
    return OF_FAILURE;
  }

  /* 
     Now go through the instance list of this node, and get the 
     matching instance.
   */
  if (CM_DLLQ_COUNT (&(pInode->ChildAccess.InstanceList)) == 0)
  {
    if (pInode->uiParentType == UCMDM_INODE_PARENT_TYPE_INODE)
    {
      parent_inode_p = pInode->ParentAccess.parent_inode_p;
      pDll = &(parent_inode_p->ChildAccess.ChildInodeList);
    }
    else
    {
      parent_instance_p = pInode->ParentAccess.parent_instance_p;
      pDll = &(parent_instance_p->ChildInodeList);

    }
    /* Delete Inode from its Parent List */
    CM_DLLQ_DYN_SCAN (pDll, pNode, pTmp, UCMDllQNode_t *)
    {
      pInode2 = CM_DLLQ_LIST_MEMBER (pNode, UCMDM_Inode_t *, list_node);
      if (pInode2 == pInode)
      {
        CM_DM_DEBUG_PRINT ("Deleting Inode %s", pInode2->pDMNode->name_p);
        CM_DLLQ_DELETE_NODE (pDll, pNode);
        of_free (pInode2);
        pInode2 = NULL;
      }
    }
    return OF_SUCCESS;
  }

  return OF_FAILURE;
}

/**************************************************************************
 Function Name : cmi_dm_get_instance_using_key
 Input Args    : pInode - Pointer to Data Model  INode
                : key_p - Pointer to Key Value
               : key_type - Data Type of Key
 Output Args   : None
 Description   : This API Gets DM Instance Node using Data Model Inode and Key
                : field Information
 Return Values : Pointer to DM Instance Node or NULL
 *************************************************************************/
UCMDM_Instance_t *cmi_dm_get_instance_using_key (UCMDM_Inode_t * pInode,
                                             void * key_p, uint32_t key_type)
{
  UCMDllQNode_t *pNode;
  UCMDllQ_t *pDll;
  UCMDM_Instance_t *pInstance;

  if (unlikely (pInode == NULL))
  {
    return NULL;
  }

  /* The child Inode must be of a table type */
  if (unlikely ((pInode->pDMNode == NULL) ||
                (pInode->pDMNode->element_attrib.element_type !=
                 CM_DMNODE_TYPE_TABLE)))
  {
    return NULL;
  }

  /* 
     Now go through the instance list of this node, and get the 
     matching instance.
   */
  pDll = &(pInode->ChildAccess.InstanceList);
  CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
  {
    pInstance = CM_DLLQ_LIST_MEMBER (pNode, UCMDM_Instance_t *, list_node);

    if ((key_type == CM_DATA_TYPE_INT) &&
        (pInstance->Map.key_type == CM_DATA_TYPE_INT))
    {
      if (!of_strcmp (((char *) key_p), ((char *) pInstance->Map.key_p)))
      {
        return pInstance;
      }
    }
    else if ((key_type == CM_DATA_TYPE_UINT) &&
             (pInstance->Map.key_type == CM_DATA_TYPE_UINT))
    {
      if (!of_strcmp (((char *) key_p), ((char *) pInstance->Map.key_p)))
      {
        return pInstance;
      }
    }
    else if ((key_type == CM_DATA_TYPE_INT64) &&
        (pInstance->Map.key_type == CM_DATA_TYPE_INT64))
    {
      if (!of_strcmp (((char *) key_p), ((char *) pInstance->Map.key_p)))
      {
        return pInstance;
      }
    }
    else if ((key_type == CM_DATA_TYPE_UINT64) &&
        (pInstance->Map.key_type == CM_DATA_TYPE_UINT64))
    {
      if (!of_strcmp (((char *) key_p), ((char *) pInstance->Map.key_p)))
      {
        return pInstance;
      }
    }
    else if ((key_type == CM_DATA_TYPE_STRING) &&
             (pInstance->Map.key_type == CM_DATA_TYPE_STRING))
    {
      if (!of_strcmp (((char *) key_p), ((char *) pInstance->Map.key_p)))
			{
				 return pInstance;
			}
    }
    else if ((key_type == CM_DATA_TYPE_IPADDR) &&
             (pInstance->Map.key_type == CM_DATA_TYPE_IPADDR))
    {
      if (!of_strcmp (((char *) key_p), ((char *) pInstance->Map.key_p)))
			{
				 return pInstance;
			}
    }
  }

  return NULL;
}

UCMDM_Instance_t *cmi_dm_get_instance_using_keyArray (UCMDM_Inode_t * pInode,
                                             struct cm_array_of_nv_pair *key_nv_pairs_p)
{
	UCMDllQNode_t *pNode;
	UCMDllQ_t *pDll;
	UCMDM_Instance_t *pInstance;
	char *key_p, *instance_key_p;
	uint32_t iKey,iInstKey;
	uint32_t uiKey,uiInstKey;
	uint32_t key_type, uiInstanceType, uiKeyCnt, uiInstance;//CID 545
	unsigned char key_bFound;

	if (unlikely (pInode == NULL))
	{
		CM_DM_DEBUG_PRINT("pInode is NULL");
		return NULL;
	}

	if (unlikely (key_nv_pairs_p == NULL))
	{
		CM_DM_DEBUG_PRINT("key array is  NULL");
		return NULL;
	}

	/* The child Inode must be of a table type */
	if (unlikely ((pInode->pDMNode == NULL) ||
				(pInode->pDMNode->element_attrib.element_type !=
				 CM_DMNODE_TYPE_TABLE)))
	{
		CM_DM_DEBUG_PRINT("(pInode->pDMNode is NULL Or its not a table");
		return NULL;
	}

	/* 
	   Now go through the instance list of this node, and get the 
	   matching instance.
	 */
	pDll = &(pInode->ChildAccess.InstanceList);
	CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
	{
		pInstance = CM_DLLQ_LIST_MEMBER (pNode, UCMDM_Instance_t *, list_node);

		if(pInstance->Map.key_nv_pairs.count_ui == key_nv_pairs_p->count_ui)
		{
			key_bFound=TRUE;
			for(uiKeyCnt=0; (uiKeyCnt < key_nv_pairs_p->count_ui) && (key_bFound == TRUE); uiKeyCnt++)
			{
				key_type= key_nv_pairs_p->nv_pairs[uiKeyCnt].value_type;
				key_p=key_nv_pairs_p->nv_pairs[uiKeyCnt].value_p;
				CM_DM_DEBUG_PRINT("key array key %s",key_p);
				key_bFound=FALSE;
				for(uiInstance=0; (uiInstance < pInstance->Map.key_nv_pairs.count_ui)  && (key_bFound == FALSE); uiInstance++)
				{
					uiInstanceType=pInstance->Map.key_nv_pairs.nv_pairs[uiInstance].value_type;
					instance_key_p= (char *) pInstance->Map.key_nv_pairs.nv_pairs[uiInstance].value_p;
#if 0
					if (!of_strcmp (( key_p), ( instance_key_p)))
					{
						key_bFound=TRUE;
					}
#endif
					CM_DM_DEBUG_PRINT("Instance Key %s",(char *) pInstance->Map.key_nv_pairs.nv_pairs[uiInstance].value_p);

					if ((key_type == CM_DATA_TYPE_INT) && 
							(uiInstanceType == CM_DATA_TYPE_INT))
					{
						iKey = of_atoi(key_p);
						iInstKey = of_atoi(instance_key_p);

						if (iKey == iInstKey)
						{
							key_bFound=TRUE;
						}
					}
					else if ((key_type == CM_DATA_TYPE_UINT) &&
							(uiInstanceType == CM_DATA_TYPE_UINT))
					{
						uiKey = of_atoi(key_p);
						uiInstKey = of_atoi(instance_key_p);
						if (uiKey == uiInstKey)
						{
							key_bFound=TRUE;
						}
					}
					else if ((key_type == CM_DATA_TYPE_INT64) && 
							(uiInstanceType == CM_DATA_TYPE_INT64))
					{
						iKey = of_atol(key_p);
						iInstKey = of_atol(instance_key_p);

						if (iKey == iInstKey)
						{
							key_bFound=TRUE;
						}
					}
					else if ((key_type == CM_DATA_TYPE_UINT64) &&
							(uiInstanceType == CM_DATA_TYPE_UINT64))
					{

						uiKey = char_To64bitNum(key_p);
						uiInstKey = char_To64bitNum(instance_key_p);
						if (uiKey == uiInstKey)
						{
							key_bFound=TRUE;
						}
					}
					else 
					{
						if (!of_strcmp (( key_p), ( instance_key_p)))
						{
							key_bFound=TRUE;
						}
					}
				}
			}
			if(key_bFound == TRUE)
			{
				return pInstance;
			}
		}
	}

	return NULL;
}
/**************************************************************************
 Function Name : cmi_dm_get_instance_using_id
 Input Args    : pInode - Pointer to Data Model  INode
               : instance_id - Instance ID value
 Output Args   : None
 Description   : This API Gets DM Instance Node using Data Model Inode and Instance Id
 Return Values : Pointer to DM Instance Node or NULL
 *************************************************************************/
UCMDM_Instance_t *cmi_dm_get_instance_using_id (UCMDM_Inode_t * pInode,
                                            uint32_t instance_id)
{
  UCMDllQNode_t *pNode;
  UCMDllQ_t *pDll;
  UCMDM_Instance_t *pInstance;

  if (unlikely (pInode == NULL))
  {
    return NULL;
  }

  /* The child Inode must be of a table type */
  if (unlikely ((pInode->pDMNode == NULL) ||
                (pInode->pDMNode->element_attrib.element_type !=
                 CM_DMNODE_TYPE_TABLE)))
  {
    return NULL;
  }

  /* 
     Now go through the instance list of this node, and get the 
     matching instance.
   */
  pDll = &(pInode->ChildAccess.InstanceList);
  CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
  {
    pInstance = CM_DLLQ_LIST_MEMBER (pNode, UCMDM_Instance_t *, list_node);
    if (pInstance->Map.instance_id == instance_id)
    {
      return pInstance;
    }
  }

  return NULL;
}

/**************************************************************************
 Function Name : cmi_dm_get_child_inode_by_name 
 Input Args    : pChildInodeDll - Pointer to DLLQ contains child Inodes
               : pChildName     - Name of DM Child Node
 Output Args   : None
 Description   : This API Gets DM INode using DillQ and DM ChildNode Name
 Return Values : Pointer to DM INode or NULL
 *************************************************************************/
UCMDM_Inode_t *cmi_dm_get_child_inode_by_name (UCMDllQ_t * pChildInodeDll,
                                          char * pChildName)
{
  UCMDllQNode_t *pNode;
  UCMDM_Inode_t *pChildInode = NULL;

  if ((!pChildInodeDll) || (!pChildName))
  {
    return NULL;
  }

  CM_DLLQ_SCAN (pChildInodeDll, pNode, UCMDllQNode_t *)
  {
    pChildInode = CM_DLLQ_LIST_MEMBER (pNode, UCMDM_Inode_t *, list_node);
    if (!of_strcmp (pChildInode->pDMNode->name_p, pChildName))
    {
      return pChildInode;
    }
  }

  return NULL;
}

/**************************************************************************
 Function Name : cmi_dm_get_child_inode_by_mid
 Input Args    : pChildInodeDll - Pointer to DLLQ contains child Inodes
               : uiDMID         - ID of DM Child Node
 Output Args   : None
 Description   : This API Gets DM INode using DillQ and DM ChildNode ID
 Return Values : Pointer to DM INode or NULL
 *************************************************************************/
UCMDM_Inode_t *cmi_dm_get_child_inode_by_mid (UCMDllQ_t * pChildInodeDll,
                                          uint32_t uiDMID)
{
  UCMDllQNode_t *pNode;
  UCMDM_Inode_t *pChildInode = NULL;

  if (!pChildInodeDll)
  {
    return NULL;
  }

  CM_DLLQ_SCAN (pChildInodeDll, pNode, UCMDllQNode_t *)
  {
    pChildInode = CM_DLLQ_LIST_MEMBER (pNode, UCMDM_Inode_t *, list_node);
    if (pChildInode->pDMNode->id_ui == uiDMID)
    {
      return pChildInode;
    }
  }

  return NULL;
}

/**************************************************************************
 Function Name : cmi_dm_remove_keys_from_path 
 Input Args    : pInPath  - Pointer to Input Data Model Instance Path
               : pOutPath - Pointer to Output Data Model Template Path
               : uiPathLen - Length of Input DM Path
 Output Args   : None
 Description   : This API generates Data Model Template path for given Instance Path
 Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_remove_keys_from_path (char * pInPath, char * pOutPath,
                                  uint32_t uiPathLen)
{
  uint32_t i;
  uint32_t j = 0;
  unsigned char bSkip = FALSE;

  if (unlikely ((pInPath == NULL) || (pOutPath == NULL)))
  {
    return OF_FAILURE;
  }

  // uiPathLen = of_strlen(pInPath);
  for (i = 0; i < uiPathLen; i++)
  {
    if (pInPath[i] == '{')
    {
      bSkip = TRUE;
    }
    else if (pInPath[i] == '}')
    {
      bSkip = FALSE;
      continue;
    }

    if (bSkip == FALSE)
    {
      pOutPath[j++] = pInPath[i];
    }
  }

  pOutPath[j] = '\0';

  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cmi_dm_get_new_instance_id_from_inode 
 Input Args    : pInode  - Pointer to DM Inode
 Output Args   : pInstID - New Instance ID
 Description   : This API gets Latest Instance ID for given DM Inode
 Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_get_new_instance_id_from_inode (UCMDM_Inode_t * pInode,
                                         uint32_t * pInstID)
{
  unsigned char bFound;
  uint32_t uiStartIndex;

  if (unlikely ((pInode == NULL) || (pInode->pDMNode == NULL)))
  {
    *pInstID = 0;
    return OF_FAILURE;
  }

  if (unlikely
      (pInode->pDMNode->element_attrib.element_type != CM_DMNODE_TYPE_TABLE))
  {
    *pInstID = 0;
    return OF_FAILURE;
  }

  if (unlikely (pInode->bIDWrapped == TRUE))
  {
    /* look through already created instances to find a free one */
    bFound = FALSE;

    uiStartIndex = pInode->uiNextFreeID - 1;
    if (uiStartIndex == 0)
    {
      uiStartIndex--;
    }
    while (uiStartIndex != pInode->uiNextFreeID)
    {
      if (cmi_dm_get_instance_using_id (pInode, pInode->uiNextFreeID) == NULL)
      {
        bFound = TRUE;
        break;
      }
      pInode->uiNextFreeID++;
      if (pInode->uiNextFreeID == 0)
      {
        pInode->uiNextFreeID++;
      }
    }

    if (bFound == FALSE)
    {
      *pInstID = 0;
      return OF_FAILURE;
    }
  }

  *pInstID = pInode->uiNextFreeID;

  pInode->uiNextFreeID++;
  if (pInode->uiNextFreeID == 0)
  {
    pInode->uiNextFreeID++;
  }

  if (pInode->uiNextFreeID < *pInstID)
  {
    pInode->bIDWrapped = TRUE;
  }

  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cmi_dm_add_instance_map_to_inode 
 Input Args    : pInode  - Pointer to DM Inode
                : pMap - Pointer to Instance Map Entry
 Output Args   : None
 Description   : This API add Instace Map Entry to Inode
 Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/

int32_t cmi_dm_add_instance_map_to_inode (UCMDM_Inode_t * pInode,
                                     struct cm_dm_instance_map * pMap)
{
  UCMDM_Instance_t *pInstanceEntry = NULL;

  if (unlikely ((pInode == NULL) || (pInode->pDMNode == NULL)) ||
      (pMap == NULL))
  {
    return OF_FAILURE;
  }

  if (unlikely
      (pInode->pDMNode->element_attrib.element_type != CM_DMNODE_TYPE_TABLE))
  {
    return OF_FAILURE;
  }

  pInstanceEntry = (UCMDM_Instance_t *) of_calloc (1, sizeof (UCMDM_Instance_t));
  if (unlikely (pInstanceEntry == NULL))
  {
    return OF_FAILURE;
  }

  pInstanceEntry->pInode = pInode;
  CM_DLLQ_INIT_LIST (&(pInstanceEntry->ChildInodeList));
  CM_DLLQ_INIT_NODE (&(pInstanceEntry->list_node));
  of_memcpy (&(pInstanceEntry->Map), pMap, sizeof (struct cm_dm_instance_map));
#if 0
  pInstanceEntry->Map.key_p = (char *) of_calloc (1, pMap->key_length + 1);
  if (pInstanceEntry->Map.key_p)
  {
    of_memcpy (pInstanceEntry->Map.key_p, pMap->key_p, pMap->key_length);
  }
#endif
  CM_DLLQ_APPEND_NODE (&(pInode->ChildAccess.InstanceList),
                        &(pInstanceEntry->list_node));
  CM_DM_DEBUG_PRINT
    ("Instance succesfully added pInstanceEntry->Map.instance_id = %u pInstanceEntry->Map.key_type = %u pInstanceEntry->Map.key_p = %s",
     pInstanceEntry->Map.instance_id, pInstanceEntry->Map.key_nv_pairs.nv_pairs[0].value_type,
     (char *)pInstanceEntry->Map.key_nv_pairs.nv_pairs[0].value_p);

  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cmi_dm_get_instance_map_entry_list_using_name_path
 Input Args    : node_path_p - Pointer to DM Node Path
                : pMap - Pointer to InstanceMap Structure
 Output Args   : None
 Description   : This API Adds New Instance Entry into Data Model Instance Tree
                : using Name path and Key
 Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_get_instance_map_entry_list_using_name_path (char * node_path_p,
                                                     uint32_t * pcount_ui,
                                                     struct cm_dm_instance_map **
                                                     map_pp)
{
  UCMDM_Inode_t *pInode;
  UCMDM_Instance_t *pInstance;
  uint32_t uiInstanceCount = 0;
  struct cm_dm_instance_map *pInstanceMap;
  int32_t i;
  UCMDllQNode_t *pDllQNode;

  if (unlikely (node_path_p == NULL))
  {
    return OF_FAILURE;
  }

  /* Cannot get Instance list for a given Instance */
  if (unlikely (node_path_p[of_strlen (node_path_p) - 1] == '}'))
  {
    return OF_FAILURE;
  }

  /* First get the Inode that matches the path */
  pInode = cmi_dm_get_inode_by_name_path (node_path_p);
  if (unlikely ((pInode == NULL) || (pInode->pDMNode == NULL)))
  {
    return OF_FAILURE;
  }
  
  uiInstanceCount = CM_DLLQ_COUNT (&pInode->ChildAccess.InstanceList);
  pInstanceMap = (struct cm_dm_instance_map *) of_calloc (uiInstanceCount,
                                                   sizeof
                                                   (struct cm_dm_instance_map));
  if (!pInstanceMap)
  {
    CM_DM_DEBUG_PRINT ("Memory allocation failed");
    return OF_FAILURE;
  }

  i = 0;
  CM_DLLQ_SCAN (&pInode->ChildAccess.InstanceList, pDllQNode, UCMDllQNode_t *)
  {
    pInstance = CM_DLLQ_LIST_MEMBER (pDllQNode, UCMDM_Instance_t *, list_node);
    of_memcpy (&pInstanceMap[i], &pInstance->Map, sizeof (pInstance->Map));
    if (pInstance->Map.key_p)
    {
      pInstanceMap[i].key_p = (void *) of_calloc (1,
                                                  pInstance->Map.key_length + 1);
      if (pInstanceMap[i].key_p)
      {
        of_memcpy (pInstanceMap[i].key_p, pInstance->Map.key_p,
                  pInstance->Map.key_length);
      }
    }
    i++;
  }
  *pcount_ui = uiInstanceCount;
  *map_pp = pInstanceMap;

  CM_DM_DEBUG_PRINT ("%d Instances found for DM Path %s ", *pcount_ui,
                      node_path_p);
  return OF_SUCCESS;
}
/**************************************************************************
 Function Name : cmi_dm_add_instance_map_to_inode 
 Input Args    : pInode  - Pointer to DM Inode
                : pMap - Pointer to Instance Map Entry
 Output Args   : None
 Description   : This API add Instace Map Entry to Inode
 Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
unsigned char cmi_dm_is_instance_map_entry_using_key_and_name_path (char * node_path_p,
                                                      void * key_p,
                                                      uint32_t key_type)
{
  UCMDM_Inode_t *pInode = NULL;
  UCMDM_Instance_t *pInstanceEntry=NULL;

  if (unlikely (node_path_p == NULL))
  {
    return FALSE;
  }

  /* First get the Inode that matches the path */
  pInode = cmi_dm_get_inode_by_name_path (node_path_p);
  if (unlikely ((pInode == NULL) || (pInode->pDMNode == NULL)))
  {
    CM_DM_DEBUG_PRINT ("Inode or DMNode in Inode is NULL");
    return FALSE;
  }

  pInstanceEntry = cmi_dm_get_instance_using_key (pInode, key_p, key_type);
  if (pInstanceEntry == NULL)
  {
    CM_DM_DEBUG_PRINT ("Instance Entry is NULL");
    return FALSE;
  }

  return TRUE;
}

unsigned char cmi_dm_is_instance_map_entry_using_key_array_and_name_path (char * node_path_p,
                                              struct cm_array_of_nv_pair *pnv_pair_array)
{
  UCMDM_Inode_t *pInode = NULL;
  UCMDM_Instance_t *pInstanceEntry=NULL;

  if (unlikely (node_path_p == NULL))
  {
    return FALSE;
  }

  /* First get the Inode that matches the path */
  pInode = cmi_dm_get_inode_by_name_path (node_path_p);
  if (unlikely ((pInode == NULL) || (pInode->pDMNode == NULL)))
  {
    CM_DM_DEBUG_PRINT ("Inode or DMNode in Inode is NULL");
    return FALSE;
  }

  pInstanceEntry = cmi_dm_get_instance_using_keyArray(pInode, pnv_pair_array);
  if (pInstanceEntry == NULL)
  {
    CM_DM_DEBUG_PRINT ("Instance Entry is NULL");
    return FALSE;
  }

  return TRUE;
}
/**
  \ingroup DMAPI
	This API Create a Name Value Pair using Instance Map Entry Details for
	a Data Model Element Node.
 	<b>Input paramameters: </b>\n
  <b><i>pMap:</i></b> Pointer to Instance Map Entry.
  <b><i>pDMNode:</i></b> Pointer to Data Model Element Node.
  <b>Output Parameters: </b> None \n
  <b><i>pNvPair:</i></b> Pointer to Name Value Pair.
  <b>Return Value:</b> OF_SUCCESS in Success
    OF_FAILURE in Failure case.\n
 */
/**************************************************************************
 Function Name : cmi_dm_create_nv_pair_from_imap_entry_and_dm_node 
 Input Args    : pMap - Pointer to InstanceMap Structure
               : pDMNode - Pointer to Data Model Element Node
 Output Args   : pNvPair - Pointer to Name Value Pair
 Description   : This API Creates Name Value Pair using Instance Entry for a 
 								 Data Element Node.
 Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_create_nv_pair_from_imap_entry_and_dm_node (struct cm_dm_instance_map * pMap,
                                                  struct cm_dm_data_element * pDMNode,
                                                  struct cm_nv_pair * pNvPair)
{
  struct cm_dm_data_element *key_child_info_p = NULL;

  CM_DM_DEBUG_PRINT ("Entered");
  if ((!pMap) || (!pDMNode) || (!pNvPair))
  {
    return OF_FAILURE;
  }

  pNvPair->value_type = pMap->key_type;
  pNvPair->value_length = pMap->key_length;
  pNvPair->value_p = of_calloc (1, pNvPair->value_length + 1);
  of_strncpy (pNvPair->value_p, pMap->key_p, pNvPair->value_length);

  key_child_info_p = cmi_dm_get_key_child_element (pDMNode);
  if (!key_child_info_p)
  {
    CM_DM_DEBUG_PRINT ("cmi_dm_get_key_child_info_UsingNode failed");
    return OF_FAILURE;
  }
  pNvPair->name_length = of_strlen (key_child_info_p->name_p);
  pNvPair->name_p = of_calloc (1, pNvPair->name_length + 1);
  if (!pNvPair->name_p)
  {
    CM_DM_DEBUG_PRINT ("Memory Allocation Failed");
    return OF_FAILURE;
  }
  of_strncpy (pNvPair->name_p, key_child_info_p->name_p, pNvPair->name_length);

  return OF_SUCCESS;
}

/**
  \ingroup DMAPI
	This API Fills Data Model Instance Information in KeysArray structure from
	a given Head Node to pINode in Instance Tree.

	<b>Input paramameters: </b>\n
  <b><i>pHeadNode:</i></b> Pointer to Data Model Inode.
  <b><i>pINode:</i></b> Pointer to Data Model Inode.
  <b>Output Parameters: </b> None \n
  <b><i>keys_array_p:</i></b> Pointer to  Array of structure.
  <b>Return Value:</b> OF_SUCCESS in Success
    OF_FAILURE in Failure case.\n
 */
/**************************************************************************
 Function Name : cmi_dm_create_keys_array_from_instance_node
 Description   : This API Creates Instance Information structure(keys_array_p)
 								 in a Datal Model Instance from a given Head Node to pINode.
 Input Args    : pHeadNode - POinter to Data Model Inode
               : pINode  - Pointer to Data Model Inode
 Output Args   : keys_array_p - Pointer to  Array of Structures
 Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_create_keys_array_from_inode (UCMDM_Inode_t *pHeadNode,
                                   			UCMDM_Inode_t * pINode,
                                        struct cm_array_of_structs * keys_array_p)
{
  UCMDM_Inode_t *pTmpINode = pINode;
  UCMDM_Instance_t *pTmpInstanceNode = NULL;
  struct cm_nv_pair *pNvPair;
  uint32_t uiInstanceCount, tmp_count_ui;

  if ((!pHeadNode) || (!pINode) || (!keys_array_p))
  {
    CM_DM_DEBUG_PRINT ("Invalid Inputs");
    return OF_FAILURE;
  }

  uiInstanceCount = 0;
  while ((pTmpINode) && ( pTmpINode != pHeadNode))
  {
    if (pTmpINode->uiParentType == UCMDM_INODE_PARENT_TYPE_INODE)
      pTmpINode = pTmpINode->ParentAccess.parent_inode_p;
    else
    {
      pTmpInstanceNode = pTmpINode->ParentAccess.parent_instance_p;
      uiInstanceCount++;
      pTmpINode = pTmpInstanceNode->pInode;
    }
  }

  CM_DM_DEBUG_PRINT ("Record Count %d", uiInstanceCount);
  /* Allocate memory for output */
  if (uiInstanceCount == 0)
  {
    keys_array_p->count_ui = 0;
    return OF_SUCCESS;
  }
  pNvPair = (struct cm_nv_pair *) of_calloc (uiInstanceCount, sizeof (struct cm_nv_pair));
  if (pNvPair == NULL)
  {
    keys_array_p->count_ui = 0;
    return OF_FAILURE;
  }

  /* Just traverse back on ancestors */
  pTmpINode = pINode;
  pTmpInstanceNode = NULL;
  tmp_count_ui = uiInstanceCount;
  while ((pTmpINode) && ( pTmpINode != pHeadNode))
  {
    if (pTmpINode->uiParentType == UCMDM_INODE_PARENT_TYPE_INODE)
      pTmpINode = pTmpINode->ParentAccess.parent_inode_p;
    else
    {
      pTmpInstanceNode = pTmpINode->ParentAccess.parent_instance_p;
      pTmpINode = pTmpInstanceNode->pInode;
      cmi_dm_create_nv_pair_from_imap_entry_and_dm_node (&pTmpInstanceNode->Map,
                                                pTmpINode->pDMNode,
                                                &pNvPair[--tmp_count_ui]);
    }
  }

  keys_array_p->struct_arr_p = (void *) pNvPair;
  keys_array_p->count_ui = uiInstanceCount;
  return OF_SUCCESS;
}

/**
  \ingroup DMAPI
	This API Fills Data Model Instance Information in KeysArray structure from
	a given Head Instance Node to pINode in Instance Tree.

	<b>Input paramameters: </b>\n
  <b><i>pHeadNode:</i></b> Pointer to Data Model Inode.
  <b><i>pINode:</i></b> Pointer to Data Model Inode.
  <b>Output Parameters: </b> None \n
  <b><i>keys_array_p:</i></b> Pointer to  Array of structure.
  <b>Return Value:</b> OF_SUCCESS in Success
    OF_FAILURE in Failure case.\n
 */
/**************************************************************************
 Function Name : cmi_dm_create_keys_array_from_instance_node
 Description   : This API Fills  Instance Information structure in KeysArray
 								 from a given Head Instance Node to pINode.
 Input Args    : pHeadNode - Pointer to Data Model Instance Node
               : pINode  - Pointer to Data Model Inode
 Output Args   : keys_array_p - Pointer to  Array of Structures
 Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_create_keys_array_from_instance_node (UCMDM_Instance_t * pHeadNode,
                                               UCMDM_Inode_t *pINode,
                                               struct cm_array_of_structs * keys_array_p)
{
	 UCMDM_Inode_t *pTmpINode = pINode;
	 UCMDM_Instance_t *pTmpInstanceNode = NULL;
	 struct cm_nv_pair *pNvPair;
  uint32_t uiInstanceCount, tmp_count_ui;

	 CM_DM_DEBUG_PRINT ("Creating Keys Array from InstanceNode ");
	 if ((!pHeadNode) || (!pINode)||(!keys_array_p))
	 {
			return OF_FAILURE;
	 }

	 uiInstanceCount = 0;
	 CM_DM_DEBUG_PRINT ("Head Node is Instance Node");
	 while ((pTmpINode) && ( pTmpINode != pHeadNode->pInode))
	 {
			if (pTmpINode->uiParentType == UCMDM_INODE_PARENT_TYPE_INODE)
			{
				 pTmpINode = pTmpINode->ParentAccess.parent_inode_p;
			}
			else
			{
				 pTmpInstanceNode = pTmpINode->ParentAccess.parent_instance_p;
				 uiInstanceCount++;
				 pTmpINode = pTmpInstanceNode->pInode;
			}
	 }

	 /* Allocate memory for output */
	 if (uiInstanceCount == 0)
	 {
			keys_array_p->count_ui = 0;
			return OF_SUCCESS;
	 }
	 pNvPair =
			(struct cm_nv_pair *) of_calloc (1, sizeof (struct cm_nv_pair) * uiInstanceCount);
	 if (pNvPair == NULL)
	 {
			keys_array_p->count_ui = 0;
			return OF_FAILURE;
	 }

	 /* Just traverse back on ancestors */
	 pTmpINode = pINode;
	 pTmpInstanceNode = NULL;
	 tmp_count_ui = uiInstanceCount;
	 while ((pTmpINode) && ( pTmpINode != pHeadNode->pInode))
	 {
			if (pTmpINode->uiParentType == UCMDM_INODE_PARENT_TYPE_INODE)
				 pTmpINode = pTmpINode->ParentAccess.parent_inode_p;
			else
			{
				 pTmpInstanceNode = pTmpINode->ParentAccess.parent_instance_p;
				 pTmpINode = pTmpInstanceNode->pInode;
				 cmi_dm_create_nv_pair_from_imap_entry_and_dm_node (&pTmpInstanceNode->Map,
							 pTmpINode->pDMNode,
							 &pNvPair[--tmp_count_ui]);
			}
	 }
	 keys_array_p->struct_arr_p = pNvPair;
	 keys_array_p->count_ui = uiInstanceCount;
	 return OF_SUCCESS;
}
/**
  \ingroup DMAPI
   This API	returns a First Inode in Data Model Instance Tree from given DM Path
	 by traversing Top to Bottom or Bottom to Top. This API also fills opaque Info
	 rmation of output DM Inode in opaque_info_pp.

	<b>Input paramameters: </b>\n
  <b><i>head_node_path_p:</i></b> Pointer to Data Model Path.
  <b><i>traversal_type_ui:</i></b> UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM/
									UCMDM_TRAVERSAL_TYPE_BOTTOM_TO_TOP.
  <b>Output Parameters: </b> None \n
	<b><i>ppDMInode</i></b> double pointer to Data Model Inode.
	<b><i>opaque_info_pp</i></b> double pointer to Opaque Inforation of Output
	                            Inode.
  <b>Return Value:</b> OF_SUCCESS in Success
    OF_FAILURE in Failure case.\n
 */
/**************************************************************************
 Function Name : cmi_dm_get_first_inode_using_instance_path 
 Description   : This API returns a First Inode in Data Model Instance Tree from
 									given DM Path by traversing Top to Bottom or Bottom to Top.
 Input Args    : head_node_path_p - Pointer to Data Model Path
 							 : traversal_type_ui - UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM/
							 										 UCMDM_TRAVERSAL_TYPE_BOTTOM_TO_TOP
 Output Args   : ppDMInode - double pointer to Data Model Inode.
  	           : opaque_info_pp - double pointer to Opaque Inforation of returned
							 										Inode.
 Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_get_first_inode_using_instance_path (char * head_node_path_p,
                                          uint32_t traversal_type_ui,
                                          UCMDM_Inode_t ** ppDMInode,
                                          void ** opaque_info_pp)
{
  struct cm_array_of_structs *keys_array_p = NULL;
  void *pInstanceOrINode = NULL;
  UCMDM_Inode_t *pINode = NULL;
  UCMDM_Instance_t *pInstanceNode = NULL;
  uint32_t node_type_ui=UCMDM_INODE_PARENT_TYPE_INODE;
  UCMDllQNode_t *pDllqNode;
  int32_t return_value = OF_FAILURE;

  CM_DM_DEBUG_PRINT ("Entered");
  if ((head_node_path_p == NULL) || (of_strlen (head_node_path_p) == 0))
  {
    pINode = cmi_dm_get_root_inode ();
    if (unlikely (!pINode))
    {
      // Send specific error
      return OF_FAILURE;
    }
		node_type_ui=UCMDM_INODE_PARENT_TYPE_INODE;
		pInstanceOrINode=(void *)pINode;
  }
  else
  {
    /* First get the Inode matching the path */
    return_value = cmi_dm_get_inode_or_instance_node_by_name_path (head_node_path_p, &node_type_ui,
                                                   &pInstanceOrINode);
    if ((unlikely (pInstanceOrINode == NULL)) || (return_value == OF_FAILURE))
    {
      return OF_FAILURE;
    }
  }

  switch (traversal_type_ui)
  {
    case UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM:
      /* Pre-Order Traversal : Just get the head node
       * We are already there.
       */
      if (node_type_ui == UCMDM_INODE_PARENT_TYPE_INSTANCE)
      {
/*        if (CM_DLLQ_COUNT
            (&(((UCMDM_Instance_t *) pInstanceOrINode)->ChildInodeList)) != 0)
        {
          pDllqNode =
            CM_DLLQ_FIRST (&
                            (((UCMDM_Instance_t *)
                              pInstanceOrINode)->ChildInodeList));
          pINode = CM_DLLQ_LIST_MEMBER (pDllqNode, UCMDM_Inode_t *, list_node);
        }
        else*/
        {
          pINode = ((UCMDM_Instance_t *) pInstanceOrINode)->pInode;
        }
      }
      else
        pINode =(UCMDM_Inode_t *) pInstanceOrINode;
      break;
    case UCMDM_TRAVERSAL_TYPE_BOTTOM_TO_TOP:
      /* Post-Order Traversal :  Get
       * the bottom most node */
      if (node_type_ui == UCMDM_INODE_PARENT_TYPE_INSTANCE)
      {
        pInstanceNode = pInstanceOrINode;
        if (CM_DLLQ_COUNT
            (&(((UCMDM_Instance_t *) pInstanceOrINode)->ChildInodeList)) != 0)
        {
          pDllqNode =
            CM_DLLQ_FIRST (&
                            (((UCMDM_Instance_t *)
                              pInstanceOrINode)->ChildInodeList));
          pINode = CM_DLLQ_LIST_MEMBER (pDllqNode, UCMDM_Inode_t *, list_node);
        }
        else
        {
          pINode = ((UCMDM_Instance_t *) pInstanceOrINode)->pInode;
        }
      }
      else
        pINode = pInstanceOrINode;

      if (pINode)
      {
        while (CM_DLLQ_COUNT (&(pINode->ChildAccess.ChildInodeList)) != 0)
        {
          if (pINode->pDMNode->element_attrib.element_type ==
              CM_DMNODE_TYPE_TABLE)
          {
            /* Traverse Instances */
            if (CM_DLLQ_COUNT (&(pINode->ChildAccess.InstanceList)) != 0)
            {
              pDllqNode = CM_DLLQ_FIRST (&(pINode->ChildAccess.InstanceList));
              pInstanceNode =
                CM_DLLQ_LIST_MEMBER (pDllqNode, UCMDM_Instance_t *, list_node);
              if (CM_DLLQ_COUNT (&(pInstanceNode->ChildInodeList)) != 0)
              {
                pDllqNode = CM_DLLQ_FIRST (&(pInstanceNode->ChildInodeList));
                pINode =
                  CM_DLLQ_LIST_MEMBER (pDllqNode, UCMDM_Inode_t *, list_node);
              }
              /* If Instances are not present break traversing */
              else
              {
                break;
              }
            }
            else
            {
              /* No Instances are present for a table type. 
               * Hence break traversing*/
              break;
            }
          }
          else
          {
            /* It is not a table type
             * So traverse Child Inodes
             */
            if (CM_DLLQ_COUNT (&(pINode->ChildAccess.ChildInodeList)) != 0)
            {
              pDllqNode =
                CM_DLLQ_FIRST (&(pINode->ChildAccess.ChildInodeList));
              pINode =
                CM_DLLQ_LIST_MEMBER (pDllqNode, UCMDM_Inode_t *, list_node);
            }
            else
            {
              /* It is not a table type
               * No Child Inodes are present
               * Stop traversing
               */
              break;
            }
          }
        }
      }
      else
      {
        CM_DM_DEBUG_PRINT ("No Inode found");
        return OF_FAILURE;
      }
      break;
  }

  /* Construct the OpaqueInfo */
  keys_array_p =
    (struct cm_array_of_structs *) of_calloc (1, sizeof (struct cm_array_of_structs));
  if (!keys_array_p)
  {
    CM_DM_DEBUG_PRINT ("Memory Allocation failed ");
    return OF_FAILURE;
	}

	if (node_type_ui == UCMDM_INODE_PARENT_TYPE_INODE)
	{
		 if (cmi_dm_create_keys_array_from_inode ( (UCMDM_Inode_t *)pInstanceOrINode,
							pINode, keys_array_p) != OF_SUCCESS)
		 {
				of_free(keys_array_p);
				return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
		 }
	}
	else
	{
		 if (cmi_dm_create_keys_array_from_instance_node ( (UCMDM_Instance_t *)pInstanceOrINode,
							pINode, keys_array_p) != OF_SUCCESS)
		 {
				of_free(keys_array_p);
				return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
		 }
	}

  *opaque_info_pp = (void *) keys_array_p;
  *ppDMInode = pINode;
  return OF_SUCCESS;
}


UCMDM_Inode_t *cmi_dm_get_root_inode (void)
{
  return pDMInstTree;
}

void cmi_dm_free_instance_opaque_info(void *opaque_info_p)
{
	 struct cm_array_of_nv_pair *arr_nv_pair_p=(struct cm_array_of_nv_pair *)opaque_info_p;

	 if(!arr_nv_pair_p)
	 {
			CM_DM_DEBUG_PRINT("Invalid input");
			return;
	 }
	 CM_FREE_PTR_NVPAIR_ARRAY(arr_nv_pair_p, arr_nv_pair_p->count_ui);
	 return;
}



char *cmi_dm_prepare_dm_instance_path(char *path_p, struct cm_array_of_iv_pairs *keys_array_p)
{
	 char *pNewPath=NULL;
	 uint32_t uiKeyCnt=0;
   struct cm_dm_data_element *parent_node_p=NULL;
   int32_t iTokenRet=UCMDM_PATH_PARSING_REMAINING, return_value;
	 unsigned char bTokenComplete=FALSE;
	 uint32_t uiMaxTokenLen=0, uindex_i=0;
	 char *pToken=NULL;
	 struct cm_array_of_structs *pChildInfoArr=NULL;
	 struct cm_dm_node_info *key_child_p=NULL;
	 uint32_t   ii,jj;
	 char chNewPath[CM_DM_MAX_PATH_LEN] ;

	 if((!path_p) || (!keys_array_p))
	 {
			CM_DM_DEBUG_PRINT("Invalid input");
				 return NULL;
	}

	of_memset(chNewPath, 0, sizeof(chNewPath));

	 /* Allocate token buffer */
	 uiMaxTokenLen = of_strlen (path_p) + 1;
	 pToken = (char *) of_calloc (1, uiMaxTokenLen);
	 if (!pToken)
	 {
//			of_free (pNewPath);
			return NULL;
	 }

	 while(iTokenRet== UCMDM_PATH_PARSING_REMAINING)
	 {
      iTokenRet = cmi_dm_get_path_token (path_p, pToken, uiMaxTokenLen, &uindex_i);

			if(parent_node_p == NULL)
			{
				 parent_node_p = cmi_dm_get_node_by_namepath (pToken);
			}
			else
			{
				 parent_node_p=cmi_dm_get_child_by_name (parent_node_p, pToken);
			}
		
			if (!parent_node_p)
			{
				 of_free (pToken);
			//	 of_free (pNewPath);
				 return NULL;
			}

			if(bTokenComplete==TRUE)
			{
				 of_strncat(chNewPath,".",1);
				 bTokenComplete=FALSE;
			}

			of_strncat(chNewPath, pToken, of_strlen(pToken));

			if( parent_node_p->element_attrib.element_type == CM_DMNODE_TYPE_TABLE)
			{
				 //key_child_p=cmi_dm_get_key_child_element(parent_node_p);
				 return_value=cmi_dm_create_key_child_info_array(parent_node_p, &pChildInfoArr);
				 if (return_value != OF_SUCCESS)
				 {
						of_free (pToken);
				//		of_free (pNewPath);
						return NULL;
				 }

				 if(keys_array_p->count_ui != uiKeyCnt)
				 {
						if(pChildInfoArr->count_ui > 0 )
						{
							 of_strncat(chNewPath,"{",1);
							 for (ii=0; ii < pChildInfoArr->count_ui; ii++)
							 {
									key_child_p=(struct cm_dm_node_info *)(pChildInfoArr->struct_arr_p) + ii;
									for (jj=0; jj < keys_array_p->count_ui; jj++)
									{
										 if(key_child_p->id_ui == keys_array_p->iv_pairs[jj].id_ui)
										 {
												if(pChildInfoArr->count_ui > 1)
												{
													 of_strncat(chNewPath,key_child_p->name_p, of_strlen(key_child_p->name_p));
													 of_strncat(chNewPath,"=",1);
												}
												of_strncat(chNewPath,keys_array_p->iv_pairs[jj].value_p, keys_array_p->iv_pairs[jj].value_length);
												if(ii != (pChildInfoArr->count_ui-1 ))
												{
													 of_strncat(chNewPath,",",1);
												}
												uiKeyCnt++;
										 }
									}
							 }
							 of_strncat(chNewPath,"}",1);
						}
						else
						{
							 CM_DM_DEBUG_PRINT("Key should be matched");
						}
				 }
				 cmi_dm_free_node_infoArray(pChildInfoArr);
			}
			bTokenComplete=TRUE;
	 }

	 of_free (pToken);
	 pNewPath=(char *)of_calloc(1, of_strlen(chNewPath)+1);
	 if(!pNewPath)
	 {
			return NULL;
	 }
	 of_strncpy(pNewPath, chNewPath, of_strlen(chNewPath));
	 return pNewPath;
}

UCMDM_Instance_t *cmi_dm_get_first_instance_using_name_path(char * node_path_p)
{
  UCMDM_Inode_t *pInode;
  UCMDM_Instance_t *pInstanceEntry;

  if (unlikely ((node_path_p == NULL) ))
  {
    return NULL;
  }

  /* First get the Inode that matches the path */
  pInode = cmi_dm_get_inode_by_name_path (node_path_p);
  if (unlikely ((pInode == NULL) || (pInode->pDMNode == NULL)))
  {
    return NULL;
  }

  pInstanceEntry = (UCMDM_Instance_t *)cmi_dm_get_first_instance (pInode);
  if (unlikely (pInstanceEntry == NULL))
  {
    return NULL;
  }

  return  pInstanceEntry;
}

UCMDM_Instance_t *cmi_dm_get_next_instance_using_name_path(char * node_path_p, struct cm_array_of_nv_pair *nv_pair_arr_p)
{
  UCMDM_Inode_t *pInode;
  UCMDM_Instance_t *pInstanceEntry;

  if (unlikely ((node_path_p == NULL)||(nv_pair_arr_p == NULL) ))
  {
    return NULL;
  }

  /* First get the Inode that matches the path */
  pInode = cmi_dm_get_inode_by_name_path (node_path_p);
  if (unlikely ((pInode == NULL) || (pInode->pDMNode == NULL)))
  {
    return NULL;
  }

  pInstanceEntry = cmi_dm_get_next_instance (pInode, nv_pair_arr_p);
  if (unlikely (pInstanceEntry == NULL))
  {
    return NULL;
  }

  return  pInstanceEntry;
}

UCMDM_Instance_t *cmi_dm_get_first_instance (UCMDM_Inode_t * pInode)

{
	 UCMDllQNode_t *pNode;
	 UCMDllQ_t *pDll;
	 UCMDM_Instance_t *pInstance;

	 if (unlikely (pInode == NULL))
	 {
			return NULL;
	 }

	 /* The child Inode must be of a table type */
	 if (unlikely ((pInode->pDMNode == NULL) ||
						(pInode->pDMNode->element_attrib.element_type !=
						 CM_DMNODE_TYPE_TABLE)))
	 {
			return NULL;
	 }

	 /* 
			Now go through the instance list of this node, and get the 
			matching instance.
			*/
	 pDll = &(pInode->ChildAccess.InstanceList);
   pNode = (UCMDllQNode_t *)(CM_DLLQ_FIRST ((pDll))); 
//	 CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
	 {
			pInstance = CM_DLLQ_LIST_MEMBER (pNode, UCMDM_Instance_t *, list_node);
      return pInstance;
	 }

	 return NULL;
}

UCMDM_Instance_t *cmi_dm_get_next_instance (UCMDM_Inode_t * pInode,
                                             struct cm_array_of_nv_pair *key_nv_pairs_p)
{
	 UCMDllQNode_t *pNode;
	 UCMDllQ_t *pDll;
	 UCMDM_Instance_t *pInstance;
	 char *key_p, *instance_key_p;
	 uint32_t uiCnt=0,key_type, uiInstanceType, uiKeyCnt, uiInstance;  //CID 546
	 unsigned char key_bFound;

	 if (unlikely (pInode == NULL))
	 {
			return NULL;
	 }

	 if (unlikely (key_nv_pairs_p == NULL))
	 {
			return NULL;
	 }

	 /* The child Inode must be of a table type */
	 if (unlikely ((pInode->pDMNode == NULL) ||
						(pInode->pDMNode->element_attrib.element_type !=
						 CM_DMNODE_TYPE_TABLE)))
	 {
			return NULL;
	 }

	 /* 
			Now go through the instance list of this node, and get the 
			matching instance.
			*/
	 pDll = &(pInode->ChildAccess.InstanceList);
	 CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
	 {
			pInstance = CM_DLLQ_LIST_MEMBER (pNode, UCMDM_Instance_t *, list_node);

			if(pInstance->Map.key_nv_pairs.count_ui == key_nv_pairs_p->count_ui)
			{
				 key_bFound=TRUE;
				 for(uiKeyCnt=0; (uiKeyCnt < key_nv_pairs_p->count_ui) && (key_bFound == TRUE); uiKeyCnt++)
				 {
						key_type=pInstance->Map.key_nv_pairs.nv_pairs[uiCnt].value_type;
						key_p= (char *) pInstance->Map.key_nv_pairs.nv_pairs[uiCnt].value_p;
						key_bFound=FALSE;
						for(uiInstance=0; (uiInstance < pInstance->Map.key_nv_pairs.count_ui)  && (key_bFound == FALSE); uiInstance++)
						{
							 uiInstanceType= key_nv_pairs_p->nv_pairs[uiInstance].value_type;
							 instance_key_p=key_nv_pairs_p->nv_pairs[uiInstance].value_p;

							 if ((key_type == CM_DATA_TYPE_INT) && 
										 (uiInstanceType == CM_DATA_TYPE_INT))
							 {
									if (!of_strcmp ( key_p, instance_key_p))
									{
										 key_bFound=TRUE;
									}
							 }
							 else if ((key_type == CM_DATA_TYPE_UINT) &&
										 (uiInstanceType == CM_DATA_TYPE_UINT))
							 {
									if (!of_strcmp (( key_p), ( instance_key_p)))
									{
										 key_bFound=TRUE;
									}
							 }
          else if ((key_type == CM_DATA_TYPE_INT64) && 
                   (uiInstanceType == CM_DATA_TYPE_INT64))
          {
            if (!of_strcmp ( key_p, instance_key_p))
            {
              key_bFound=TRUE;
            }
          }
          else if ((key_type == CM_DATA_TYPE_UINT64) &&
                   (uiInstanceType == CM_DATA_TYPE_UINT64))
          {
            if (!of_strcmp (( key_p), ( instance_key_p)))
            {
              key_bFound=TRUE;
            }
          }
							 else if ((key_type == CM_DATA_TYPE_STRING) &&
										 (uiInstanceType == CM_DATA_TYPE_STRING))
							 {
									if (!of_strcmp (( key_p), ( instance_key_p)))
									{
										 key_bFound=TRUE;
									}
							 }
							 else if ((key_type == CM_DATA_TYPE_IPADDR) &&
										 (uiInstanceType == CM_DATA_TYPE_IPADDR))
							 {
									if (!of_strcmp (( key_p), ( instance_key_p)))
									{
										 key_bFound=TRUE;
									}
							 }
						}
				 }
				 if(key_bFound == TRUE)
				 {
            pNode = (UCMDllQNode_t *)(CM_DLLQ_NEXT ((pDll),((UCMDllQNode_t *)(pNode))));
			      pInstance = CM_DLLQ_LIST_MEMBER (pNode, UCMDM_Instance_t *, list_node);
						return pInstance;
				 }
			}
	 }

	 return NULL;
}
#if 0
int32_t ucmDMGetRolesInTree (char * pHeadDMPath,
			                      char *pReadRoleName,
														char *role_name_p
                            struct cm_result * result_p)
{
	 struct cm_dm_data_element *pHeadDMNode = NULL;
	 struct cm_dm_data_element *data_element_p = NULL;
	 struct cm_array_of_iv_pairs *pIvPairArray=NULL;
	 struct cm_array_of_nv_pair *dmpath_key_nv_pairs_p = NULL;
	 struct cm_array_of_iv_pairs *dm_path_keys_iv_array_p = NULL;
	 struct cm_array_of_iv_pairs *array_of_keys_p = NULL;
	 int32_t return_value;
	 UCMDllQNode_t *pDllQNode;
	 unsigned char bDMTplDone = FALSE;
	 struct je_stack_node *pStackNode;
	 int32_t error_code;
	 uint32_t channel_id_ui, appl_id_ui;
	 uint32_t uiRecCount = 1;
	 struct cm_array_of_iv_pairs *result_iv_array_p = NULL;
	 struct cm_array_of_iv_pairs *key_array_iv_pair_p = NULL;
	 void *opaque_info_p = NULL;
	 uint32_t opaque_info_length = 0;
	 unsigned char bIsChildNode = FALSE;
	 unsigned char bGotNewNode=FALSE;
	 char *dm_path_p;
	 struct cm_array_of_nv_pair *key_array_nv_pair_p=NULL;


	 CM_JE_DEBUG_PRINT ("Entered");
	 CM_DLLQ_INIT_LIST (&je_del_stack_list);

	 dm_path_p=pHeadDMPath;
	 if (!dm_path_p)
	 {
			CM_JE_DEBUG_PRINT ("DM Path is NULL");
			result_p->result.error.error_code = UCMJE_ERROR_DMPATH_NULL;
			return OF_FAILURE;
	 }

	 pHeadDMNode = (struct cm_dm_data_element *) cmi_dm_get_dm_node_from_instance_path
			(dm_path_p, of_strlen (dm_path_p));

	 CM_JE_DEBUG_PRINT ("Node Name = %s", pHeadDMNode->name_p);

	 /* Get Keys from DM Path */
	 if ((return_value = cmi_dm_get_key_array_from_namepath (dm_path_p, &dmpath_key_nv_pairs_p))
				 != OF_SUCCESS)
	 {
			CM_JE_DEBUG_PRINT ("Keys Iv Array From Name Path failed");
			UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
						UCMJE_ERROR_DMGET_KEYS_FROM_NAMEPATH_FAILED,
						dm_path_p);
			return OF_FAILURE;
	 }

	 if (cmi_dm_verify_role_get_privileges (pHeadDMNode, role_name_p) != OF_SUCCESS)
	 {
			CM_DM_DEBUG_PRINT ("Admin has no permissions to Get roles");
			return OF_FAILURE;
	 }

	 UCMJE_FREE_PTR_NVPAIR_ARRAY(dmpath_key_nv_pairs_p, dmpath_key_nv_pairs_p->count_ui);


	 /*If pnv_pair_array is NULL  it is a factory Reset 
		* else delete command
		*/
	 return_value = cmi_dm_get_first_element_info (dm_path_p, UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
				 &data_element_p, &opaque_info_p,
				 &opaque_info_length);
	 if (!data_element_p)
	 {
			CM_JE_DEBUG_PRINT ("pDMNode is NULL");
			UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
						UCMJE_ERROR_GETNODE_BYDMPATH,
						dm_path_p);
			return OF_FAILURE;
	 }

	 pHeadInode=cmi_dm_get_inode_by_name_path(dm_path_p);
	 CM_JE_DEBUG_PRINT ("Node Name = %s", data_element_p->name_p);


	 while ((return_value == OF_SUCCESS)  || (bDMTplDone == TRUE))
	 {
			bGotNewNode=FALSE;
			bIsChildNode = TRUE;
			uiRecCount=1;
			dm_path_p = cmi_dm_create_name_path_from_node (data_element_p);
			if (CM_DLLQ_COUNT (&je_del_stack_list) != 0)
			{
				 pDllQNode = CM_DLLQ_LAST (&je_del_stack_list);
				 pStackNode =
						CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_stack_node *, list_node);
				 bIsChildNode = cmi_dm_is_child_element (dm_path_p, pStackNode->dm_path_p);
			}
			if (bDMTplDone == TRUE)
			{
				 if (CM_DLLQ_COUNT (&je_del_stack_list) == 0)
				 {
						CM_JE_DEBUG_PRINT("Template Traversal Completed");
						return OF_SUCCESS;
				 }
			}

			CM_JE_DEBUG_PRINT ("Name =%s -- path=%s", data_element_p->name_p, dm_path_p);
			if  ((( bIsChildNode == TRUE) || (CM_DLLQ_COUNT (&je_del_stack_list) == 0)) && (bDMTplDone == FALSE))
			{
				 if(pStackNode)
				 {
						CM_JE_DEBUG_PRINT ("%s and %s are in same path", dm_path_p,
									pStackNode->dm_path_p);
				 }
				 else
				 {
						CM_JE_DEBUG_PRINT ("DMPATH %s and No stack Node found", dm_path_p);
				 }

				 switch (pInode->pDMNode->element_attrib.element_type)
				 {
						case CM_DMNODE_TYPE_ANCHOR:
						case CM_DMNODE_TYPE_SCALAR_VAR:
						case CM_DMNODE_TYPE_INVALID:
							 break;
						case CM_DMNODE_TYPE_TABLE:
							 /*Temporary fix*/
							 if(data_element_p->element_attrib.non_config_leaf_node_b == TRUE)
									break;
							 pInstanceNode =  cmi_dm_get_first_instance_using_name_path (dm_path_p);
							 if (pInstanceNode == NULL )
							 {
									CM_JE_DEBUG_PRINT ("Get FistNRecords Failed");
							 }
							 else
							 {
									CM_JE_DEBUG_PRINT ("Succesfully recieved Get First Instance");
									CM_JE_PRINT_IVPAIR_ARRAY(result_iv_array_p);
									if((return_value=ucmDMRoleCreateAndPushStackNode (data_element_p, opaque_info_p,
															opaque_info_length, pInstanceNode)) != OF_SUCCESS)
									{
										 error_code=OF_FAILURE;
										 break;
									}
							 }
				 }
				 if(error_code == OF_FAILURE)
				 {
						break;
				 }
			}
			else
			{
				 CM_JE_DEBUG_PRINT ("%s and %s are not in same path", dm_path_p,
							 pStackNode->dm_path_p);

				 CM_JE_DEBUG_PRINT ("Getting Next Records for %s", pStackNode->pDMNode->name_p);
				 array_of_keys_p =
						(struct cm_array_of_iv_pairs *) je_collect_keys_from_stack (&je_del_stack_list,
									dm_path_keys_iv_array_p);
				 error_code =
				     cmi_dm_get_next_instance_using_name_path(pStackNode->dm_path_p, pStackNode->pInstanceNode->pMap.nv_pair_array);
				 if ((error_code != OF_SUCCESS))	
				 {
						CM_JE_DEBUG_PRINT ("GET Next N Records Callback failed");
						CM_JE_DEBUG_PRINT ("Deleting Instance = %s",
									(char *) array_of_keys_p->iv_pairs[array_of_keys_p->count_ui -1].value_p);
						if ((return_value =
										 cmje_execute_delete_command (pStackNode->pDMNode, array_of_keys_p,
												result_p)) != OF_SUCCESS)
						{
							 CM_JE_DEBUG_PRINT ("Execute Delete Command Failed for Key=%s",
										 (char *) array_of_keys_p->iv_pairs[array_of_keys_p->count_ui -1].value_p);
							 error_code = OF_FAILURE;
							 break;
						}
						if(pStackNode->pDMInode != NULL)
						{
							 key_array_nv_pair_p =
									(struct cm_array_of_nv_pair *)of_calloc(1,sizeof(struct cm_array_of_nv_pair));
							 je_copy_ivpairarr_to_nvpairarr(pStackNode->dm_path_p, pStackNode->pIvPairArr, key_array_nv_pair_p);
							 if ((return_value = cmi_dm_delete_instance_using_keyArray (pStackNode->pDMInode,
													 key_array_nv_pair_p)) !=	OF_SUCCESS)
							 {
									CM_JE_DEBUG_PRINT ("Delete Instance Using Key failed");
									UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
												UCMJE_ERROR_DELETE_INSTANCE_FAILED,
												dm_path_p);
									error_code=UCMJE_ERROR_DELETE_INSTANCE_FAILED;
									break;
							 }
							 if(key_array_nv_pair_p)
									UCMJE_FREE_PTR_NVPAIR_ARRAY(key_array_nv_pair_p, key_array_nv_pair_p->count_ui);	

							 if (CM_DLLQ_COUNT (&(pStackNode->pDMInode->ChildAccess.InstanceList)) == 0)
							 {
									CM_JE_DEBUG_PRINT ("Inode Instances are zero.. so deleting Inode");
									if ((return_value = cmi_dm_delete_inode (pStackNode->pDMInode)) != OF_SUCCESS)
									{
										 CM_JE_DEBUG_PRINT ("Delete INode Failed");
										 UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
													 UCMJE_ERROR_DELETE_INODE_FAILED,
													 dm_path_p);
										 error_code = UCMJE_ERROR_DELETE_INODE_FAILED;
										 break;
									}
							 }
						}
						CM_DLLQ_DELETE_NODE(&je_del_stack_list,&(pStackNode->list_node));
						je_cleanup_stack_node(pStackNode);
						if(result_iv_array_p)
						{
							 UCMJE_FREE_PTR_IVPAIR_ARRAY(result_iv_array_p, result_iv_array_p->count_ui);
						}
						bGotNewNode = TRUE;
				 }
				 else
				 {
						if(opaque_info_p)
							 of_free(opaque_info_p);
						opaque_info_p =
							 (uint32_t *) of_calloc (1, pStackNode->opaque_info_length*	sizeof (uint32_t));
						of_memcpy (opaque_info_p, pStackNode->opaque_info_p,
									(sizeof (uint32_t) * pStackNode->opaque_info_length));
						opaque_info_length = pStackNode->opaque_info_length;
						CM_JE_DEBUG_PRINT ("Deleting Instance = %s",
									(char *) array_of_keys_p->iv_pairs[array_of_keys_p->count_ui -1].value_p);
						if ((return_value =
										 cmje_execute_delete_command (pStackNode->pDMNode, array_of_keys_p,
												result_p)) != OF_SUCCESS)
						{
							 CM_JE_DEBUG_PRINT ("Execute Delete Command Failed for Key=%s",
										 (char *) array_of_keys_p->iv_pairs[array_of_keys_p->count_ui -1].value_p);
							 error_code = OF_FAILURE;
							 break;
						}

						if(pStackNode->pDMInode != NULL)
						{
							 key_array_nv_pair_p =
									(struct cm_array_of_nv_pair *)of_calloc(1,sizeof(struct cm_array_of_nv_pair));
							 je_copy_ivpairarr_to_nvpairarr(pStackNode->dm_path_p, pStackNode->pIvPairArr, key_array_nv_pair_p);
							 if ((return_value = cmi_dm_delete_instance_using_keyArray (pStackNode->pDMInode,
													 key_array_nv_pair_p)) !=	OF_SUCCESS)
							 {
									CM_JE_DEBUG_PRINT ("Delete Instance Using Key failed");
									UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
												UCMJE_ERROR_DELETE_INSTANCE_FAILED,
												dm_path_p);
									error_code=UCMJE_ERROR_DELETE_INSTANCE_FAILED;
									break;
							 }
							 if(key_array_nv_pair_p)
									UCMJE_FREE_PTR_NVPAIR_ARRAY(key_array_nv_pair_p, key_array_nv_pair_p->count_ui);
						}
						key_array_iv_pair_p =
							 (struct cm_array_of_iv_pairs *)je_find_key_ivpair_array (pStackNode->pDMNode, result_iv_array_p);
						je_update_stack_node (pStackNode,dm_path_keys_iv_array_p, key_array_iv_pair_p);
						UCMJE_FREE_PTR_IVPAIR_ARRAY(result_iv_array_p, result_iv_array_p->count_ui);
						return_value =
							 cmi_dm_get_next_element_info (pHeadDMPath, UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
										 &data_element_p, &opaque_info_p, &opaque_info_length);
						bGotNewNode=TRUE;
						bDMTplDone=FALSE;
				 }
				 if(array_of_keys_p)
				 {
						UCMJE_FREE_PTR_IVPAIR_ARRAY(array_of_keys_p,array_of_keys_p->count_ui);
				 }
			}

			/* If we have taken NewNode from stack or we have already taken
			 * processed stack node's next node
			 * we can skip call to getnextnodeinfo */
			if( bGotNewNode == FALSE)
			{
				 return_value =
						cmi_dm_get_next_element_info (pHeadDMPath, UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
									&data_element_p, &opaque_info_p, &opaque_info_length);
				 if (return_value == OF_FAILURE)
				 {
						CM_JE_DEBUG_PRINT ("Tree Completed");
						bDMTplDone = TRUE;
						if (CM_DLLQ_COUNT (&je_del_stack_list) == 0)
						{
							 CM_JE_DEBUG_PRINT ("Stack is empty ");
							 je_delete_table_stack (&je_del_stack_list);
							 return OF_SUCCESS;
						}
				 }
			}
			if(dm_path_p)
				 of_free(dm_path_p);
			error_code=OF_SUCCESS;
	 }

	 if(error_code != OF_SUCCESS)
	 {
			CM_JE_DEBUG_PRINT ("DELETE sub tree failed");
			if(array_of_keys_p)
			{
				 UCMJE_FREE_PTR_IVPAIR_ARRAY(array_of_keys_p,array_of_keys_p->count_ui);
			}
			if(key_array_nv_pair_p)
				 UCMJE_FREE_PTR_NVPAIR_ARRAY(key_array_nv_pair_p, key_array_nv_pair_p->count_ui);
			je_delete_table_stack(&je_del_stack_list);
			return OF_FAILURE;
	 }
	 CM_JE_DEBUG_PRINT ("DELETE sub tree completed");
	 je_delete_table_stack(&je_del_stack_list);
	 return OF_SUCCESS;
}                             /* ( while return_value == OF_SUCCESS) */
#endif
#endif /* CM_DM_SUPPORT */
#endif /* CM_SUPPORT */
