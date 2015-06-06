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
 * File name: dmtree.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/15/2013
 * Description: This file contains source code implementing 
 *              configuration middle-ware data model tree. 
*/

#ifdef CM_SUPPORT
#ifdef CM_DM_SUPPORT

#include "dmincld.h"

#define   likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)

#define CM_DM_ROOT_NODE_ID 1
/* Root node */
struct cm_dm_channel_callbacks *cm_channels_p[CM_MAX_CHANNEL_ID];
struct cm_dm_data_element DMRootNode = {
  /* Reserved section */
  NULL,                         /* Reserved : parent_p  */
  NULL,                         /* Reserved : call_backs_p */
  {{NULL, NULL}
   , 0}
  ,                             /* Reserved : dllq_t : childlist */
  {NULL, NULL}
  ,                             /* Reserved : dllnode_t : peer list */
  {{NULL, NULL}
   , 0}
  ,                             /* Reserved : dllq_t : role&perm list */

  //"IGD",                            /* node name */
  CM_DM_ROOT_NODE_NAME,        /* node name */
  NULL,
  CM_DM_ROOT_NODE_ID,          /* Unique ID */
  NULL,                         /* reference name array */
  NULL,                         /* Validate Fn Ptr */
  "UCM Data Model Root Node",   /* Description */
  "",                           /* Save file name */
  "",                           /* Save Dir name */

  {
   CM_DMNODE_TYPE_ANCHOR,      /* node type */
   FALSE,                     /* visible or not? */
   FALSE,                     /* key or not? */
   FALSE,                     /* mandatory or not? */
   FALSE,                     /* ordered list or not? */
   FALSE,                     /* Table priority node or not? */
   FALSE,                     /* notify change or not? */
   FALSE,                     /* notify attrib : active/passive */
   FALSE,                     /* sticky to parent or not? */
   FALSE,                     /* Create save directories for kids */
   /*FALSE, */                     /*      Save this element in parent instance's
                                   directory. For example, firewall is 
                                   saved in "vsg-1", etc. */
   /*FALSE,*/                     /* This is a non-configurable leaf node. Like structure members. */
   CM_ACCESS_ALL,              /* 0 = Allowed from ALL
                                   1 = HTTP Can not Edit
                                   2 = CLI Can not Edit */
   FALSE,   /*Grouped scalar */
   FALSE,   /*Global transcation */
   FALSE,   /*Parent transaction */
   FALSE,   /*System command */
   FALSE,   /*Stats Command */
   0,                           /* Unused bits */
   FALSE,                      /*      Save this element in parent instance's
                                   directory. For example, firewall is 
                                   saved in "vsg-1", etc. */
   FALSE,                     /* This is a non-configurable leaf node. Like structure members. */
   FALSE,
   0,                           /* Unused bits */
   NULL,   /*command name */
   NULL,    /*Identifier */
   NULL,   /*Identifier help*/
   }
  ,

  {
   CM_DATA_TYPE_UNKNOWN,
   CM_DATA_ATTRIB_NONE,

   /* Data/value Attributes */
   {
    {0,
     0}
    }
   ,

   /* Default values */
   {
    0,
    }
   }
};

/* Root node */
struct cm_dm_data_element DMIGDNode = {
   /* Reserved section */
   NULL,                         /* Reserved : parent_p  */
   NULL,                         /* Reserved : call_backs_p */
   {{NULL, NULL}
      , 0}
      ,                             /* Reserved : dllq_t : childlist */
         {NULL, NULL}
      ,                             /* Reserved : dllnode_t : peer list */
         {{NULL, NULL}
            , 0}
            ,                             /* Reserved : dllq_t : role&perm list */

               //"IGD",                        /* node name */
               CM_DM_ROOT_NODE_NAME,        /* node name */
               NULL,
               CM_DM_ROOT_NODE_ID,          /* Unique ID */
               NULL,                         /* reference name array */
               NULL,                         /* Validate Fn Ptr */
               "UCM Data Model Root Node",   /* Description */
               "",                           /* Save file name */
               "",                           /* Save Dir name */

               {
                  CM_DMNODE_TYPE_ANCHOR,      /* node type */
                  FALSE,                     /* visible or not? */
                  FALSE,                     /* key or not? */
                  FALSE,                     /* mandatory or not? */
                  FALSE,                     /* ordered list or not? */
                  FALSE,                     /* Table priority node or not? */
                  FALSE,                     /* notify change or not? */
                  FALSE,                     /* notify attrib : active/passive */
                  FALSE,                     /* sticky to parent or not? */
                  FALSE,                     /* Create save directories for kids */
               }
            ,

               {
                  CM_DATA_TYPE_UNKNOWN,
                  CM_DATA_ATTRIB_NONE,

                  /* Data/value Attributes */
                  {
                     {0,
                        0}
                  }
                  ,

                     /* Default values */
                     {
                        0,
                     }
               }
};

struct cm_dm_data_element RelativityNode = {
  /* Reserved section */
  NULL,                         /* Reserved : internal use */
  NULL,                         /* Reserved : internal use */
  {{NULL, NULL}
   , 0}
  ,                             /* Reserved : internal use */
  {NULL, NULL}
  ,                             /* Reserved : internal use */
  {{NULL, NULL}
   , 0}
  ,                             /* Reserved : internal use */

  /* User section */
  CM_DM_ELEMENT_RELATIVITY_NAME,       /* Element name. Not an FQN */
  NULL,
  CM_DM_ELEMENT_RELATIVITY_ID, /* Unique ID with in siblings */
  NULL,                         /* Reference name array */
  NULL,                         /* Function ptr to validate data */
  "Contains insert position [before/after/begin/end]",    /* Short description */
  "",                           /* Dirctory name for save */
  NULL,                         /* If NULL, use parent's filename */
  {
   CM_DMNODE_TYPE_SCALAR_VAR,  /* anchor/table/struct/variable */
   TRUE,                      /* should we expose for configuration? */
   FALSE,                     /* Is this a key for the container? */
   FALSE,                     /* Is this a mandatory param? */
   FALSE,                     /* Is this an ordered list? (table) */
   FALSE,                     /* Is this specifies priority of container table? */
   TRUE,                      /* Inform if any config change on this element */
   CM_PASSIVE_NOTIFICATION,    /* 1 = ActiveNotify, 
                                   2 = PassiveNotify, 
                                   4 = Not changeable by ACS */

   FALSE,                     /* Reboot required if modified */
   TRUE,                      /* This node config should go along with parent config */
   /*TRUE,*/                      /*      Save this element in parent instance's
                                   directory. For example, firewall is 
                                   saved in "vsg-1", etc. */
   /*FALSE,*/                     /* This is a non-configurable leaf node. Like structure members. */
   CM_ACCESS_ALL,              /* 0 = Allowed from ALL
                                   1 = HTTP Can not Edit
                                   2 = CLI Can not Edit */

   FALSE,   /*Grouped scalar */
   FALSE,   /*Global transcation */
   FALSE,   /*Parent transaction */
   FALSE,   /*Is system Command*/
   FALSE,   /*Is stats command*/
   0,                           /* Unused bits */
   TRUE,                      /*      Save this element in parent instance's
                                   directory. For example, firewall is 
                                   saved in "vsg-1", etc. */
   FALSE,                     /* This is a non-configurable leaf node. Like structure members. */
   0,                           /* Unused bits */
   NULL,   /*command name */
   NULL,    /*Identifier */
   NULL,   /*Identifier help*/
   }
  ,
  {
   CM_DATA_TYPE_STRING,        /* int/string/ip_addr/etc */
   CM_DATA_ATTRIB_STR_ENUM,    /* range/enum/etc */
   {.string_enum.count_ui = 4,
    .string_enum.array[0] = "before",
    .string_enum.array[1] = "after",
    .string_enum.array[2] = "begin",
    .string_enum.array[3] = "end",
    .string_enum.aFrdArr[0]="",
    .string_enum.aFrdArr[1]="",
    .string_enum.aFrdArr[2]="",
    .string_enum.aFrdArr[3]="",
    }
   ,
   {
    .default_string = "after",
    }
   ,
   }
  ,
};

/* Global Variables */
struct cm_dm_data_element *pDMTree = &DMRootNode;

 int32_t ucmdm_CopyNodeInfointoArray (struct cm_dm_node_info * node_info_p,
      struct cm_dm_node_info * node_info_array_p,
      uint32_t uindex_i);

/**************************************************************************
  Function Name : UCMDM_Init
  Input Args    : none
  Output Args   : none
Description   : Data Model Tree Initialization
Return Values : OF_SUCCESS/OF_FAILURE
 *************************************************************************/
int32_t cm_dm_init (void)
{
   int32_t return_value;

   return_value = cm_dm_initInstanceTree ();
   if (return_value != OF_SUCCESS)
   {
      CM_DM_DEBUG_PRINT ("cm_dm_initInstanceTree Failed : %d", return_value);
      return OF_FAILURE;
   }

   return OF_SUCCESS;
}

typedef struct UCMPathApplChannId_s
{
   char dm_path_p[100];
   uint32_t appl_id_ui;
   uint32_t channel_id_ui;
   struct UCMPathApplChannId_s *next_p;

} UCMPathApplChannId_t;

UCMPathApplChannId_t *UCMPathApplChannIdHead = NULL;

int32_t cmi_add_dm_path_appl_channid (char * dm_path_p, uint32_t appl_id_ui,
      uint32_t channel_id_ui)
{
   UCMPathApplChannId_t *pTemp;
   pTemp = UCMPathApplChannIdHead;
   while (pTemp)
   {
      if (of_strcmp (dm_path_p, pTemp->dm_path_p) == 0)
         break;
      pTemp = pTemp->next_p;
   }
   if (pTemp)
      return OF_FAILURE;
   pTemp = (UCMPathApplChannId_t *) of_calloc (1, sizeof (UCMPathApplChannId_t));
   if (pTemp == NULL)
      return OF_FAILURE;
   CM_DM_DEBUG_PRINT ("Path=%s::appl_id=%d::ChaId=%d", dm_path_p, appl_id_ui,
         channel_id_ui);
   of_strcpy (pTemp->dm_path_p, dm_path_p);
   pTemp->appl_id_ui = appl_id_ui;
   pTemp->channel_id_ui = channel_id_ui;
   pTemp->next_p = UCMPathApplChannIdHead;
   UCMPathApplChannIdHead = pTemp;
   return OF_SUCCESS;

}

int32_t cm_get_channel_id_and_appl_id (char * dm_path_p, uint32_t * appl_id_ui,
      uint32_t * channel_id_ui)
{
   UCMPathApplChannId_t *pTemp;
   pTemp = UCMPathApplChannIdHead;
   char *pTemdm_path_p;
   int32_t uiValidPathLen = 0;

   uiValidPathLen = of_strlen (dm_path_p);
   pTemdm_path_p = (char *) of_calloc (1, uiValidPathLen + 1);
   if (unlikely (!pTemdm_path_p))
   {
      return OF_FAILURE;
   }
   if (cmi_dm_remove_keys_from_path (dm_path_p, pTemdm_path_p, uiValidPathLen) !=
         OF_SUCCESS)
   {
      CM_DM_DEBUG_PRINT ("cmi_dm_remove_keys_from_path Failed");
      of_free (pTemdm_path_p);
      return OF_FAILURE;
   }
   while (pTemp)
   {
      if (of_strcmp (pTemdm_path_p, pTemp->dm_path_p) == 0)
         break;
      pTemp = pTemp->next_p;
   }
   if (pTemp == NULL)
   {
      of_free (pTemdm_path_p);
      return OF_FAILURE;
   }
   if(pTemp->channel_id_ui == 0)
   {

      of_free (pTemdm_path_p);
      CM_DM_DEBUG_PRINT ("Path=%s::appl_id=%d::ChaId=%d::it is a ANCHOR", dm_path_p, *appl_id_ui,*channel_id_ui);
      return OF_FAILURE;
   }
   *appl_id_ui = pTemp->appl_id_ui;
   *channel_id_ui = pTemp->channel_id_ui;
   CM_DM_DEBUG_PRINT ("Path=%s::appl_id=%d::ChaId=%d", dm_path_p, *appl_id_ui,
         *channel_id_ui);
   of_free (pTemdm_path_p);
   return OF_SUCCESS;
}

/**************************************************************************
  Function Name : cm_dm_register_data_elements 
  Input Args    : path_p - Pointer to DM Path
  : uiElemCount - Number of Data Elements to be registered
  : element_array_p - Pointer to Data Element Array 
  Output Args   : none
Description   : This API used by Security Applications to Register Data Elements
: with UCM Data Model Tree 
Return Values : OF_SUCCESS or appropriate Error Code on Failure
 *************************************************************************/
int32_t cm_dm_register_data_elements (uint32_t appl_id_ui, uint32_t chann_id_ui,
      char * path_p, uint32_t uiElemCount,
      struct cm_dm_data_element * element_array_p)
{
   uint32_t i;
   struct cm_dm_data_element *parent_node_p = NULL;
   struct cm_dm_data_element *pNode = NULL;
   UCMDllQNode_t *pDllQNode, *pTmpNode;
   struct cm_dm_data_element *pChildNode = NULL;
   int32_t return_value;

   if (unlikely ((!path_p) || (uiElemCount == 0) || (element_array_p == NULL)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Get Parent Node based on input path */
   parent_node_p = cmi_dm_get_node_by_namepath (path_p);
   if (unlikely (!parent_node_p))
   {
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   /* Check any duplicate child node exists for this parent */
   for (i = 0; i < uiElemCount; i++)
   {
      pNode = cmi_dm_get_child_by_name (parent_node_p, element_array_p[i].name_p);
      if (unlikely (pNode != NULL))
      {
         return CM_DM_ERR_DUPLICATE_NODE;
      }
   }

   /* Attach each child */
   for (i = 0; i < uiElemCount; i++)
   {
      /* Initialize the internal parameters of the node */
      return_value =
         cm_dm_initDataElementInternals (&(element_array_p[i]),
               element_array_p[i].app_cbks_p);
      if (unlikely (return_value != OF_SUCCESS))
      {
         /* Fixme : Cleanup all the attached nodes? */
         CM_DLLQ_DYN_SCAN (&(parent_node_p->child_list), pDllQNode, pTmpNode,
               UCMDllQNode_t *)
         {
            pChildNode =
               CM_DLLQ_LIST_MEMBER (pDllQNode, struct cm_dm_data_element *, list_node);
            if (pChildNode)
               CM_DLLQ_DELETE_NODE (&(parent_node_p->child_list), pDllQNode);
         }
         return return_value;
      }
      /* OK, now just attach our node as a new child */
      if (cmi_dm_attach_child (parent_node_p, &(element_array_p[i])) != OF_SUCCESS)
      {
         /* Clean up Attached Childs */
         CM_DLLQ_DYN_SCAN (&(parent_node_p->child_list), pDllQNode, pTmpNode,
               UCMDllQNode_t *)
         {
            pChildNode =
               CM_DLLQ_LIST_MEMBER (pDllQNode, struct cm_dm_data_element *, list_node);
            if (pChildNode)
               CM_DLLQ_DELETE_NODE (&(parent_node_p->child_list), pDllQNode);
         }
         return CM_DM_ERR_INVALID_ELEMENT;
      }
   }
   cmi_add_dm_path_appl_channid (path_p, appl_id_ui, chann_id_ui);
   return OF_SUCCESS;
}

int32_t cm_dm_register_scalar_appl_id (uint32_t appl_id_ui, uint32_t chann_id_ui,
      char * path_p,
      struct cm_dm_data_element * scalar_element_p)
{
   struct cm_dm_data_element *pNode = NULL;
   int32_t return_value;

   if (unlikely ((!path_p) || (scalar_element_p == NULL)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Get Parent Node based on input path */
   pNode = cmi_dm_get_node_by_namepath (path_p);
   if (unlikely (!pNode))
   {
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   if (pNode->parent_p->element_attrib.element_type != CM_DMNODE_TYPE_ANCHOR)
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   if (scalar_element_p->element_attrib.element_type != CM_DMNODE_TYPE_SCALAR_VAR)
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Add to Channel List */
   return_value = cmi_add_dm_path_appl_channid (path_p, appl_id_ui, chann_id_ui);
   return return_value;
}

/**************************************************************************
  Function Name : UCMDM_GetChildIDByName 
  Input Args    : path_p - Pointer to DM Path
  : pNodeName - Data Element Node Name
  Output Args   : None
Description   : This API returns Data Element ID based on DM Path and Node Name
Return Values : Node ID or OF_FAILURE
 *************************************************************************/

int32_t cm_dm_get_child_id_by_name (char * path_p, char * pNodeName)
{
   struct cm_dm_data_element *parent_node_p = NULL;
   struct cm_dm_data_element *pNode = NULL;
   uint32_t uiMaxTokenLen = 0;
   char *pToken = NULL;

   if (unlikely ((!path_p) || (!pNodeName)))
   {
      return OF_FAILURE;
   }

   /* Allocate token buffer */
   uiMaxTokenLen = of_strlen (path_p) + 1;

   pToken = (char *) of_calloc (1, uiMaxTokenLen);
   if (!pToken)
   {
      return OF_FAILURE;
   }

   /* Verify whether the data path is valid accroding to DMTree */
   if (cmi_dm_remove_keys_from_path (path_p, pToken, (uiMaxTokenLen - 1)) !=
         OF_SUCCESS)
   {
      of_free (pToken);
      return OF_FAILURE;
   }

   /* Get Parent Node based on input path */
   parent_node_p = cmi_dm_get_node_by_namepath (pToken);
   if (unlikely (!parent_node_p))
   {
      of_free (pToken);
      return OF_FAILURE;
   }

   /* Get Child Node */
   pNode = cmi_dm_get_child_by_name (parent_node_p, pNodeName);
   if (unlikely (!pNode))
   {
      of_free (pToken);
      return OF_FAILURE;
   }

   of_free (pToken);
   return (pNode->id_ui);
}

int32_t cmi_dm_get_child_id_by_name_from_dm_path (char * path_p, char * pNodeName)
{
   struct cm_dm_data_element *parent_node_p = NULL;
   struct cm_dm_data_element *pNode = NULL;
   uint32_t uiMaxTokenLen = 0;
   char *pToken = NULL;
   char *tm_name_path_p = NULL;
   uint32_t uindex_i = 0;
   uint32_t return_value = UCMDM_PATH_PARSING_REMAINING;

   if (unlikely ((!path_p) || (!pNodeName)))
   {
      return OF_FAILURE;
   }

   /* Allocate token buffer */
   uiMaxTokenLen = of_strlen (path_p) + 1;

   tm_name_path_p = (char *) of_calloc (1, uiMaxTokenLen);
   if (!tm_name_path_p)
   {
      return OF_FAILURE;
   }

   pToken = (char *) of_calloc (1, uiMaxTokenLen);
   if (!pToken)
   {
      of_free (tm_name_path_p);
      return OF_FAILURE;
   }

   /* Verify whether the data path is valid accroding to DMTree */
   if (cmi_dm_remove_keys_from_path (path_p, tm_name_path_p, (uiMaxTokenLen - 1)) !=
         OF_SUCCESS)
   {
      of_free (pToken);
      of_free (tm_name_path_p);
      return OF_FAILURE;
   }

   return_value = cmi_dm_get_path_token (tm_name_path_p, pToken, uiMaxTokenLen, &uindex_i);
   /* Find the matching child */
   parent_node_p = cmi_dm_get_node_by_namepath (pToken);
   if (!parent_node_p)
   {
      of_free (pToken);
      of_free (tm_name_path_p);
      return OF_FAILURE;
   }

   while (return_value == UCMDM_PATH_PARSING_REMAINING)
   {
      pNode = cmi_dm_get_child_by_name (parent_node_p, pNodeName);
      if (pNode)
      {
         of_free (pToken);
         of_free (tm_name_path_p);
         return (pNode->id_ui);
      }
      else
      {
         return_value = cmi_dm_get_path_token (tm_name_path_p, pToken, uiMaxTokenLen, &uindex_i);
         /* Find the matching child */
         pNode = cmi_dm_get_child_by_name (parent_node_p, pToken);
         if (!pNode)
         {
            break;
         }
         if (return_value == UCMDM_PATH_PARSING_COMPLETED)
         {
            break;
         }
         parent_node_p = pNode;
      }
   }

   of_free (pToken);
   of_free (tm_name_path_p);
   return OF_FAILURE;
}

struct cm_dm_data_element *cmi_dm_get_child_node_by_name_from_dm_path (char * path_p,
      char * pNodeName)
{
   struct cm_dm_data_element *parent_node_p = NULL;
   struct cm_dm_data_element *pNode = NULL;
   uint32_t uiMaxTokenLen = 0;
   char *pToken = NULL;
   char *tm_name_path_p = NULL;
   uint32_t uindex_i = 0;
   uint32_t return_value = UCMDM_PATH_PARSING_REMAINING;

   if (unlikely ((!path_p) || (!pNodeName)))
   {
      return NULL;
   }

   /* Allocate token buffer */
   uiMaxTokenLen = of_strlen (path_p) + 1;

   tm_name_path_p = (char *) of_calloc (1, uiMaxTokenLen);
   if (!tm_name_path_p)
   {
      return NULL;
   }

   pToken = (char *) of_calloc (1, uiMaxTokenLen);
   if (!pToken)
   {
      of_free (tm_name_path_p);
      return NULL;
   }

   /* Verify whether the data path is valid accroding to DMTree */
   if (cmi_dm_remove_keys_from_path (path_p, tm_name_path_p, (uiMaxTokenLen - 1)) !=
         OF_SUCCESS)
   {
      of_free (pToken);
      of_free (tm_name_path_p);
      return NULL;
   }

   return_value = cmi_dm_get_path_token (tm_name_path_p, pToken, uiMaxTokenLen, &uindex_i);
   /* Find the matching child */
   parent_node_p = cmi_dm_get_node_by_namepath (pToken);
   if (!parent_node_p)
   {
      of_free (pToken);
      of_free (tm_name_path_p);
      return NULL;
   }

   while (return_value == UCMDM_PATH_PARSING_REMAINING)
   {
      pNode = cmi_dm_get_child_by_name (parent_node_p, pNodeName);
      if (pNode)
      {
         of_free (pToken);
         of_free (tm_name_path_p);
         return pNode;
      }
      else
      {
         return_value = cmi_dm_get_path_token (tm_name_path_p, pToken, uiMaxTokenLen, &uindex_i);
         /* Find the matching child */
         pNode = cmi_dm_get_child_by_name (parent_node_p, pToken);
         if (!pNode)
         {
            break;
         }
         if (return_value == UCMDM_PATH_PARSING_COMPLETED)
         {
            break;
         }
         parent_node_p = pNode;
      }
   }

   of_free (pToken);
   of_free (tm_name_path_p);
   return NULL;
}

struct cm_dm_data_element *cmi_dm_get_child_node_by_id_from_dm_path (char * path_p,
      uint32_t id_ui)
{
   struct cm_dm_data_element *parent_node_p = NULL;
   struct cm_dm_data_element *pNode = NULL;
   uint32_t uiMaxTokenLen = 0;
   char *pToken = NULL;
   char *tm_name_path_p = NULL;
   uint32_t uindex_i = 0;
   uint32_t return_value = UCMDM_PATH_PARSING_REMAINING;

   if (unlikely (!path_p))
   {
      return NULL;
   }

   if (unlikely (id_ui < 0))
   {
      return NULL;
   }
   /* Allocate token buffer */
   uiMaxTokenLen = of_strlen (path_p) + 1;

   tm_name_path_p = (char *) of_calloc (1, uiMaxTokenLen);
   if (!tm_name_path_p)
   {
      return NULL;
   }

   pToken = (char *) of_calloc (1, uiMaxTokenLen);
   if (!pToken)
   {
      of_free (tm_name_path_p);
      return NULL;
   }

   /* Verify whether the data path is valid accroding to DMTree */
   if (cmi_dm_remove_keys_from_path (path_p, tm_name_path_p, (uiMaxTokenLen - 1)) !=
         OF_SUCCESS)
   {
      of_free (pToken);
      of_free (tm_name_path_p);
      return NULL;
   }

   return_value = cmi_dm_get_path_token (tm_name_path_p, pToken, uiMaxTokenLen, &uindex_i);
   /* Find the matching child */
   parent_node_p = cmi_dm_get_node_by_namepath (pToken);
   if (!parent_node_p)
   {
      of_free (pToken);
      of_free (tm_name_path_p);
      return NULL;
   }

   while (return_value == UCMDM_PATH_PARSING_REMAINING)
   {
      pNode = cmi_dm_get_child_by_id (parent_node_p, id_ui);
      if (pNode)
      {
         of_free (pToken);
         of_free (tm_name_path_p);
         return pNode;
      }
      else
      {
         return_value = cmi_dm_get_path_token (tm_name_path_p, pToken, uiMaxTokenLen, &uindex_i);
         /* Find the matching child */
         pNode = cmi_dm_get_child_by_name (parent_node_p, pToken);
         if (!pNode)
         {
            break;
         }
         if (return_value == UCMDM_PATH_PARSING_COMPLETED)
         {
            break;
         }
         parent_node_p = pNode;
      }
   }

   of_free (pToken);
   of_free (tm_name_path_p);
   return NULL;
}

/**************************************************************************
  Function Name : UCMDM_GetChildInfoByName 
  Input Args    : path_p - Pointer to DM Path
  : pNodeName - Data Element Node Name
  Output Args   : None
Description   : This API returns Pointer to Child Data Element for given 
: DM Path and Parent Node Name
Return Values : Pointer to Data Element or NULL
 *************************************************************************/
struct cm_dm_data_element *cmi_dm_get_child_info_by_name (char * path_p,
      char * pNodeName)
{
   struct cm_dm_data_element *parent_node_p = NULL;
   struct cm_dm_data_element *pNode = NULL;
   uint32_t uiMaxTokenLen = 0;
   char *pToken = NULL;

   if (unlikely ((!path_p) || (!pNodeName)))
   {
      return NULL;
   }

   /* Allocate token buffer */
   uiMaxTokenLen = of_strlen (path_p) + 1;

   pToken = (char *) of_calloc (1, uiMaxTokenLen);
   if (!pToken)
   {
      return NULL;
   }

   /* Verify whether the data path is valid accroding to DMTree */
   if (cmi_dm_remove_keys_from_path (path_p, pToken, (uiMaxTokenLen - 1)) !=
         OF_SUCCESS)
   {
      of_free (pToken);
      return NULL;
   }

   /* Get Parent Node based on input path */
   parent_node_p = cmi_dm_get_node_by_namepath (pToken);
   if (unlikely (!parent_node_p))
   {
      of_free (pToken);
      return NULL;
   }

   /* Get Child Node */
   pNode = cmi_dm_get_child_by_name (parent_node_p, pNodeName);
   if (unlikely (!pNode))
   {
      of_free (pToken);
      return NULL;
   }

   of_free (pToken);
   return (pNode);
}

/**************************************************************************
  Function Name : UCMDM_GetChildNameByID 
  Input Args    : path_p - Pointer to DM Path
  : id_ui -  Data Element Node Identifier.
  Output Args   : None
Description   : This API returns Data Element Node Name for given 
: DM Path and Node ID
Return Values : Pointer to Data Element Name or NULL
 *************************************************************************/
char *cmi_dm_get_child_name_by_id (char * path_p, uint32_t id_ui)
{
   struct cm_dm_data_element *parent_node_p = NULL;
   struct cm_dm_data_element *pNode = NULL;
   uint32_t uiMaxTokenLen = 0;
   char *pToken = NULL;

   if (unlikely (!path_p))
   {
      return NULL;
   }

   /* Allocate token buffer */
   uiMaxTokenLen = of_strlen (path_p) + 1;

   pToken = (char *) of_calloc (1, uiMaxTokenLen);
   if (!pToken)
   {
      return NULL;
   }

   /* Verify whether the data path is valid accroding to DMTree */
   if (cmi_dm_remove_keys_from_path (path_p, pToken, (uiMaxTokenLen - 1)) !=
         OF_SUCCESS)
   {
      of_free (pToken);
      return NULL;
   }

   /* Get Parent Node based on input path */
   parent_node_p = cmi_dm_get_node_by_namepath (pToken);
   if (unlikely (!parent_node_p))
   {
      of_free (pToken);
      return NULL;
   }

   /* Get Child Node */
   pNode = cmi_dm_get_child_by_id (parent_node_p, id_ui);
   if (unlikely (!pNode))
   {
      of_free (pToken);
      return NULL;
   }

   of_free (pToken);
   return (pNode->name_p);
}

/**************************************************************************
  Function Name : UCMDM_GetChildInfoByID
  Input Args    : path_p - Pointer to DM Path
  : id_ui - Data Element Node ID
  Output Args   : None
Description   : This API returns Pointer to Child Data Element for given 
: DM Path and Parent Node Id
Return Values : Pointer to Data Element or NULL
 *************************************************************************/
struct cm_dm_data_element *cmi_dm_get_child_info_by_id (char * path_p, uint32_t id_ui)
{
   struct cm_dm_data_element *parent_node_p = NULL;
   struct cm_dm_data_element *pNode = NULL;
   uint32_t uiMaxTokenLen = 0;
   char *pToken = NULL;

   if (unlikely (!path_p))
   {
      return NULL;
   }

   /* Allocate token buffer */
   uiMaxTokenLen = of_strlen (path_p) + 1;

   pToken = (char *) of_calloc (1, uiMaxTokenLen);
   if (!pToken)
   {
      return NULL;
   }

   /* Verify whether the data path is valid accroding to DMTree */
   if (cmi_dm_remove_keys_from_path (path_p, pToken, (uiMaxTokenLen - 1)) !=
         OF_SUCCESS)
   {
      of_free (pToken);
      return NULL;
   }

   /* Get Parent Node based on input path */
   parent_node_p = cmi_dm_get_node_by_namepath (pToken);
   if (unlikely (!parent_node_p))
   {
      of_free (pToken);
      return NULL;
   }

   /* Get Child Node */
   pNode = cmi_dm_get_child_by_id (parent_node_p, id_ui);
   if (unlikely (!pNode))
   {
      of_free (pToken);
      return NULL;
   }

   of_free (pToken);
   return (pNode);
}

/**************************************************************************
  Function Name :
  Input Args    :
  Output Args   :
Description   :
Return Values :
 *************************************************************************/

/**************************************************************************
  Function Name : cm_dm_register_app_callback_fns 
  Input Args    : path_p - Pointer to DM Path
  : app_cbks_p - Pointer to Application Callback structure
  Output Args   : none
Description   : This API used by Security Applications to Register Application
: Callbacks used to Configure/View Security application database
Return Values : OF_SUCCESS or appropriate Error Code on Failure
 *************************************************************************/

int32_t cm_dm_register_app_callback_fns (char * path_p,
      struct cm_dm_app_callbacks * app_cbks_p)
{
   struct cm_dm_data_element *pNode = NULL;

   if (unlikely ((!path_p) || (!app_cbks_p)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Get Parent Node based on input path */
   pNode = cmi_dm_get_node_by_namepath (path_p);
   if (unlikely (pNode == NULL))
   {
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   if (pNode->app_cbks_p == NULL)
   {
      pNode->app_cbks_p =
         (struct cm_dm_app_callbacks *) of_calloc (1, sizeof (struct cm_dm_app_callbacks));
      if (unlikely (pNode->app_cbks_p == NULL))
      {
         return CM_DM_ERR_MEM_RESOURCES;
      }
   }

   pNode->app_cbks_p->cm_start_xtn =
      app_cbks_p->cm_start_xtn;
   pNode->app_cbks_p->cm_add_rec = app_cbks_p->cm_add_rec;
   pNode->app_cbks_p->cm_modify_rec = app_cbks_p->cm_modify_rec;
   pNode->app_cbks_p->cm_delete_rec = app_cbks_p->cm_delete_rec;
   pNode->app_cbks_p->cm_end_xtn =
      app_cbks_p->cm_end_xtn;
   pNode->app_cbks_p->cm_getfirst_nrecs =
      app_cbks_p->cm_getfirst_nrecs;
   pNode->app_cbks_p->cm_getnext_nrecs =
      app_cbks_p->cm_getnext_nrecs;
   pNode->app_cbks_p->cm_getexact_rec =
      app_cbks_p->cm_getexact_rec;
   pNode->app_cbks_p->cm_test_config =
      app_cbks_p->cm_test_config;

   return OF_SUCCESS;
}

int32_t cm_register_channel (uint32_t channel_id,
      struct cm_dm_channel_callbacks * chan_callbacks_p)
{
   struct cm_dm_channel_callbacks *pTemp;
   if (channel_id >= CM_MAX_CHANNEL_ID)
      return OF_FAILURE;
   if (cm_channels_p[channel_id] != NULL)
      return OF_FAILURE;

   pTemp = of_calloc (1, sizeof (struct cm_dm_channel_callbacks));
   if (pTemp == NULL)
      return OF_FAILURE;
   CM_DM_DEBUG_PRINT ("ChannelId=%d", channel_id);
   cm_channels_p[channel_id] = pTemp;
   pTemp->cm_start_xtn = chan_callbacks_p->cm_start_xtn;
   pTemp->cm_add_rec = chan_callbacks_p->cm_add_rec;
   pTemp->cm_modify_rec = chan_callbacks_p->cm_modify_rec;
   pTemp->cm_delete_rec = chan_callbacks_p->cm_delete_rec;
   pTemp->cm_end_xtn = chan_callbacks_p->cm_end_xtn;
   pTemp->cm_getfirst_nrecs = chan_callbacks_p->cm_getfirst_nrecs;
   pTemp->cm_getnext_nrecs = chan_callbacks_p->cm_getnext_nrecs;
   pTemp->cm_getexact_rec = chan_callbacks_p->cm_getexact_rec;
   pTemp->cm_test_config = chan_callbacks_p->cm_test_config;
   return OF_SUCCESS;
}

/* Node information retrieval API */

/**************************************************************************
  Function Name : cm_dm_get_node_info
  Input Args    : node_path_p - Pointer to DM Path
  Output Args   : node_info_pp - double pointer to Node Info structure
Description   : This API fills Node Info structure for for given DM Path
Return Values : OF_SUCCESS or appropriate error code
 *************************************************************************/
int32_t cmi_dm_get_node_info (char * node_path_p, struct cm_dm_node_info ** node_info_pp)
{
   struct cm_dm_data_element *pNode = NULL;
   struct cm_dm_node_info *node_info_p = NULL;
   uint32_t uiParentMaxTokenLen;
   char *parent_token_p;

   if (unlikely (!node_info_pp) || (!node_path_p))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Allocate token buffer */
   uiParentMaxTokenLen = of_strlen (node_path_p) + 1;

   parent_token_p = (char *) of_calloc (1, uiParentMaxTokenLen);
   if (!parent_token_p)
   {
      return CM_DM_ERR_MEM_RESOURCES;
   }

   /* Verify whether the data path is valid accroding to DMTree */
   if (cmi_dm_remove_keys_from_path
         (node_path_p, parent_token_p, (uiParentMaxTokenLen - 1)) != OF_SUCCESS)
   {
      of_free (parent_token_p);
      return OF_FAILURE;
   }
   /* Get Parent Node based on input path */
   pNode = cmi_dm_get_node_by_namepath (parent_token_p);
   if (unlikely (!pNode))
   {
      of_free (parent_token_p);
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   node_info_p = cmi_dm_create_node_info_struct (pNode, parent_token_p);
   if (unlikely (node_info_p == NULL))
   {
      *node_info_pp = NULL;
      of_free (parent_token_p);
      return CM_DM_ERR_MEM_RESOURCES;
   }

   *node_info_pp = node_info_p;
   of_free (parent_token_p);
   return OF_SUCCESS;
}

/**************************************************************************
  Function Name :
  Input Args    :
  Output Args   :
Description   :
Return Values :
 *************************************************************************/
/**************************************************************************
  Function Name :
  Input Args    :
  Output Args   :
Description   :
Return Values :
 *************************************************************************/

/**************************************************************************
  Function Name : cm_dm_get_node_children
  Input Args    : node_path_p - Pointer to DM Path
  Output Args   : pChildNames - double pointer to Array of Strings 
Description   : This API fills ChildNode Names for given DM Path
Return Values : OF_SUCCESS or appropriate error code
 *************************************************************************/

int32_t cm_dm_get_node_children (char * node_path_p,
      struct cm_array_of_strings ** pChildNames)
{
   struct cm_dm_data_element *pNode = NULL;

   if (unlikely (!pChildNames))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Get Parent Node based on input path */
   pNode = cmi_dm_get_node_by_namepath (node_path_p);
   if (unlikely (!pNode))
   {
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   return cmi_dm_create_child_name_array (pNode, pChildNames);
}

/**************************************************************************
  Function Name : cmi_dm_get_node_and_child_info 
  Input Args    : node_path_p - Pointer to DM Path
  : pNodeName - Data Element Node Name
  Output Args   : node_info_pp - Double pointer to Node Info structure
  : pChildInfoArr - Double Pointer to Array of Structures
  :  Here it is array of Node Info structure

Description   : This API returns Node Info and its Children Nodes info for given
: DM Path 
Return Values : OF_SUCCESS or appropriate error code
 *************************************************************************/
int32_t cmi_dm_get_node_and_child_info (char * node_path_p,
      struct cm_dm_node_info ** node_info_pp,
      struct cm_array_of_structs ** pChildInfoArr)
{
   int32_t return_value;
   struct cm_dm_data_element *pNode = NULL;
   char *parent_token_p = NULL;
   struct cm_dm_node_info *node_info_p = NULL;
   uint32_t uiParentMaxTokenLen;

   if (unlikely ((!node_path_p) || (!node_info_pp) || (!pChildInfoArr)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Allocate token buffer */
   uiParentMaxTokenLen = of_strlen (node_path_p) + 1;

   parent_token_p = (char *) of_calloc (1, uiParentMaxTokenLen);
   if (!parent_token_p)
   {
      return OF_FAILURE;
   }

   /* Verify whether the data path is valid accroding to DMTree */
   if (cmi_dm_remove_keys_from_path
         (node_path_p, parent_token_p, (uiParentMaxTokenLen - 1)) != OF_SUCCESS)
   {
      of_free (parent_token_p);
      return OF_FAILURE;
   }

   /* Get Parent Node based on input path */
   pNode = cmi_dm_get_node_by_namepath (parent_token_p);
   if (unlikely (!pNode))
   {
      of_free (parent_token_p);
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   node_info_p = cmi_dm_create_node_info_struct (pNode, parent_token_p);
   if (unlikely (node_info_p == NULL))
   {
      *node_info_pp = NULL;
      of_free (parent_token_p);
      return CM_DM_ERR_MEM_RESOURCES;
   }

   return_value = cmi_dm_create_child_info_array (pNode, pChildInfoArr);
   if (unlikely (return_value != OF_SUCCESS))
   {
      cmi_dm_free_node_info (node_info_p);
      *node_info_pp = NULL;
      of_free (parent_token_p);
      return CM_DM_ERR_MEM_RESOURCES;
   }

   *node_info_pp = node_info_p;
   of_free (parent_token_p);
   return OF_SUCCESS;
}

int32_t cmi_dm_get_key_child_nodes (char * node_path_p,
      struct cm_array_of_structs ** pChildInfoArr)
{
   int32_t return_value;
   struct cm_dm_data_element *pNode = NULL;
   char *parent_token_p = NULL;
   uint32_t uiParentMaxTokenLen;

   if (unlikely ((!node_path_p) ||(!pChildInfoArr)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Allocate token buffer */
   uiParentMaxTokenLen = of_strlen (node_path_p) + 1;

   parent_token_p = (char *) of_calloc (1, uiParentMaxTokenLen);
   if (!parent_token_p)
   {
      return OF_FAILURE;
   }

   /* Verify whether the data path is valid accroding to DMTree */
   if (cmi_dm_remove_keys_from_path
         (node_path_p, parent_token_p, (uiParentMaxTokenLen - 1)) != OF_SUCCESS)
   {
      of_free (parent_token_p);
      return OF_FAILURE;
   }

   /* Get Parent Node based on input path */
   pNode = cmi_dm_get_node_by_namepath (parent_token_p);
   if (unlikely (!pNode))
   {
      of_free (parent_token_p);
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   return_value = cmi_dm_create_key_child_info_array (pNode, pChildInfoArr);
   if (unlikely (return_value != OF_SUCCESS))
   {
      of_free (parent_token_p);
      return CM_DM_ERR_MEM_RESOURCES;
   }

   of_free (parent_token_p);
   return OF_SUCCESS;
}
/**************************************************************************
  Function Name : cm_dm_get_first_node_info 
  Input Args    : head_node_path_p - Pointer to DM Path
  : traversal_type_ui- Typeof Traversal in Data Model Tree
  :            UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM or
  :                UCMDM_TRAVERSAL_TYPE_BOTTOM_TO_TOP      
  Output Args   : node_info_pp - Double Pointer to Node Info Structure
  : opaque_info_pp - Double Pointer to Opaque Info structure which contains
  Node IDs of Nodes across dm path
  : opaque_len_p - Number of Nodes across dm path
Description   : This API used by Management Engines to Get Node Info and IDs for given
: DM Path
Return Values : OF_SUCCESS or appropriate error code
 *************************************************************************/

int32_t cmi_dm_get_first_node_info (char * head_node_path_p,
      uint32_t traversal_type_ui,
      struct cm_dm_node_info ** node_info_pp,
      void ** opaque_info_pp, uint32_t * opaque_len_p)
{
   struct cm_dm_data_element *pNode = NULL;
   struct cm_array_of_uints IDArray;
   UCMDllQNode_t *pDllqNode = NULL;
   uint32_t uiMaxTokenLen = 0;
   char *pToken = NULL;

   if (unlikely ((!node_info_pp) || (!opaque_info_pp) || (!opaque_len_p)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Path Name is optional. If it is missing, we traverse entire tree
      from head.
      */
   if ((head_node_path_p == NULL) || (of_strlen (head_node_path_p) == 0))
   {
      pNode = cm_dm_get_root_node ();
      if (unlikely (!pNode))
      {
         // Send specific error
         return OF_FAILURE;
      }
   }
   else
   {
      /* Allocate token buffer */
      uiMaxTokenLen = of_strlen (head_node_path_p) + 1;
      pToken = (char *) of_calloc (1, uiMaxTokenLen);
      if (!pToken)
      {
         return OF_FAILURE;
      }

      /* Verify whether the data path is valid accroding to DMTree */
      if (cmi_dm_remove_keys_from_path (head_node_path_p, pToken, (uiMaxTokenLen - 1)) !=
            OF_SUCCESS)
      {
         of_free (pToken);
         return OF_FAILURE;
      }

      pNode = cmi_dm_get_node_by_namepath (pToken);
      if (unlikely (!pNode))
      {
         of_free (pToken);
         return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
      }
   }

   switch (traversal_type_ui)
   {
      case UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM:
         /* Pre-Order Traversal : Just get the head node
            We are already there.
            */
         //    pDllqNode = CM_DLLQ_FIRST (&(pNode->child_list));
         //   pNode = CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *, list_node);

         break;

      case UCMDM_TRAVERSAL_TYPE_BOTTOM_TO_TOP:
         /* Post-Order Traversal :  Get the bottom most node */
         while (CM_DLLQ_COUNT (&(pNode->child_list)) != 0)
         {
#if 0
            if (IDArray.count_ui >= UCMDM_MAX_TREE_DEPTH)
            {
               of_free (IDArray.uint_arr_p);
               return CM_DM_ERR_PATH_LEN_EXCEEDS;
            }
#endif
            pDllqNode = CM_DLLQ_FIRST (&(pNode->child_list));
            pNode =
               CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *, list_node);
#if 0
            IDArray.uint_arr_p[IDArray.count_ui++] = pNode->id_ui;
#endif
         }
         break;

      default:
         of_free (pToken);
         return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Construct the OpaqueInfo */
   if (cmi_dm_create_id_path_from_node (pNode, &IDArray) != OF_SUCCESS)
   {
      of_free (pToken);
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   /* Copy all the attributes */
   *node_info_pp = cmi_dm_create_node_info_struct (pNode, NULL);
   if (*node_info_pp == NULL)
   {
      of_free (pToken);
      of_free (IDArray.uint_arr_p);
      return OF_FAILURE;
   }

   *opaque_info_pp = (void *) IDArray.uint_arr_p;
   *opaque_len_p = IDArray.count_ui;

   of_free (pToken);
   return OF_SUCCESS;
}

/**************************************************************************
  Function Name : cm_dm_get_next_node_info 
  Input Args    : head_node_path_p - Pointer to DM Path
  : traversal_type_ui- Typeof Traversal in Data Model Tree
  :            UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM or
  :                UCMDM_TRAVERSAL_TYPE_BOTTOM_TO_TOP      
  Output Args   : node_info_pp - Double Pointer to Node Info Structure
  In/Out Args   : opaque_info_pp - Double Pointer to Opaque Info structure which contains
  Node IDs of Nodes across dm path
  : opaque_len_p - Number of Nodes across dm path
Description   : This API used by Management Engines to Get Next Node Info and IDs for
:  given DM Path and Node IDs
Return Values : OF_SUCCESS or appropriate error code
 *************************************************************************/
int32_t cmi_dm_get_next_node_info (char * head_node_path_p,
      uint32_t traversal_type_ui,
      struct cm_dm_node_info ** node_info_pp,
      void ** opaque_info_pp, uint32_t * opaque_len_p)
{
   struct cm_dm_data_element *pNode = NULL;
   struct cm_dm_data_element *pHeadNode = NULL;
   struct cm_array_of_uints IDArray;
   UCMDllQNode_t *pDllqNode = NULL;
   int32_t return_value = OF_FAILURE;
   uint32_t uiMaxTokenLen = 0;
   char *pToken = NULL;

   if (unlikely ((!node_info_pp) || (!opaque_info_pp) || (!opaque_len_p)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Get the head node. We stop travering when we reach this */
   if ((head_node_path_p == NULL) || (of_strlen (head_node_path_p) == 0))
   {
      pHeadNode = cm_dm_get_root_node ();
      if (unlikely (!pHeadNode))
      {
         /* return error code */
         return OF_FAILURE;
      }
   }
   else
   {
      /* Allocate token buffer */
      uiMaxTokenLen = of_strlen (head_node_path_p) + 1;
      pToken = (char *) of_calloc (1, uiMaxTokenLen);
      if (!pToken)
      {
         return OF_FAILURE;
      }

      /* Verify whether the data path is valid accroding to DMTree */
      if (cmi_dm_remove_keys_from_path (head_node_path_p, pToken, (uiMaxTokenLen - 1)) !=
            OF_SUCCESS)
      {
         of_free (pToken);
         return OF_FAILURE;
      }

      pHeadNode = cmi_dm_get_node_by_namepath (pToken);
      if (unlikely (!pHeadNode))
      {
         of_free (pToken);
         return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
      }
   }

#if 0
   /* Allocate a fresh ID path array, to hold more number of elements.
      Caller might have allocated this exactly same size of its number
      of elements.
      */
   IDArray.uint_arr_p =
      (uint32_t *) of_calloc (1, sizeof (int32_t) * UCMDM_MAX_TREE_DEPTH);
   if (unlikely (IDArray.uint_arr_p == NULL))
   {
      return OF_FAILURE;
   }
   for (i = 0; i < ((int32_t) opaque_len_p); i++)
   {
      IDArray.uint_arr_p[i] = (*((int32_t **) opaque_info_pp))[i];
   }
   IDArray.count_ui = *((int32_t *) opaque_len_p);
#endif

   IDArray.uint_arr_p = (int32_t *) (*opaque_info_pp);
   IDArray.count_ui = *((int32_t *) opaque_len_p);
   *opaque_info_pp = NULL;
   *opaque_len_p = 0;

   /* Now get the current node based on OpaqueInfo : that is ID path */
   pNode = cmi_dm_get_node_by_idpath (&IDArray);
   if (unlikely (!pNode))
   {
      of_free (pToken);
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   of_free (IDArray.uint_arr_p);
   IDArray.uint_arr_p = NULL;

   switch (traversal_type_ui)
   {
      case UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM:
         /* Pre-Order Traversal */

         /* If child list is there, get first child */
         if (CM_DLLQ_COUNT (&(pNode->child_list)) != 0)
         {
            pDllqNode = CM_DLLQ_FIRST (&(pNode->child_list));
            pNode =
               CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *, list_node);
            if (cmi_dm_create_id_path_from_node (pNode, &IDArray) != OF_SUCCESS)
            {
               of_free (pToken);
               return OF_FAILURE;
            }
            *node_info_pp = cmi_dm_create_node_info_struct (pNode, NULL);
            if (*node_info_pp == NULL)
            {
               of_free (pToken);
               of_free (IDArray.uint_arr_p);
               return OF_FAILURE;
            }
            return_value = OF_SUCCESS;
            break;
         }

         /* Otherwise get the next peer, if exists */
         if (CM_DLLQ_NEXT (&(pNode->parent_p->child_list), &(pNode->list_node)) !=
               NULL)
         {
            pDllqNode =
               CM_DLLQ_NEXT (&(pNode->parent_p->child_list), &(pNode->list_node));
            pNode =
               CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *, list_node);
            if (pNode == pHeadNode)
            {
               /* It is same node as DMPath Node */
               of_free (pToken);
               of_free (IDArray.uint_arr_p);
               return OF_FAILURE;
            }
#if 0
            IDArray.count_ui--;
            IDArray.uint_arr_p[IDArray.count_ui++] = pNode->id_ui;
#endif
            if (cmi_dm_create_id_path_from_node (pNode, &IDArray) != OF_SUCCESS)
            {
               of_free (pToken);
               return OF_FAILURE;
            }
            *node_info_pp = cmi_dm_create_node_info_struct (pNode, NULL);
            if (*node_info_pp == NULL)
            {
               of_free (pToken);
               of_free (IDArray.uint_arr_p);
               return OF_FAILURE;
            }
            return_value = OF_SUCCESS;
            break;
         }
         else
         {
            /* There are no childs, no peers. Just move on to parent */
            while (pNode->parent_p != NULL)
            {
               /* If parent is Head Node, shouldn't go for its siblings.. */
               if (pNode->parent_p == pHeadNode)
               {
                  of_free (pToken);
                  return OF_FAILURE;
               }
               if ((pNode->parent_p->parent_p != NULL) &&
                     (CM_DLLQ_NEXT
                      (&(pNode->parent_p->parent_p->child_list),
                       &(pNode->parent_p->list_node)) != NULL))
               {
                  pDllqNode =
                     CM_DLLQ_NEXT (&(pNode->parent_p->parent_p->child_list),
                           &(pNode->parent_p->list_node));
                  pNode =
                     CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *, list_node);

#if 0
                  /* We have moved one step up, one stop to side */
                  IDArray.count_ui -= 2;
                  IDArray.uint_arr_p[IDArray.count_ui++] = pNode->id_ui;
#endif
                  if (cmi_dm_create_id_path_from_node (pNode, &IDArray) != OF_SUCCESS)
                  {
                     of_free (pToken);
                     return OF_FAILURE;
                  }
                  *node_info_pp = cmi_dm_create_node_info_struct (pNode, NULL);
                  if (*node_info_pp == NULL)
                  {
                     of_free (pToken);
                     of_free (IDArray.uint_arr_p);
                     return OF_FAILURE;
                  }
                  return_value = OF_SUCCESS;
                  break;
               }
               else
               {
                  pNode = pNode->parent_p;
#if 0
                  IDArray.count_ui--;
#endif
                  if (pNode == pHeadNode)
                  {
                     return_value = OF_FAILURE;
                     break;
                  }
               }
            }
         }
         break;

      case UCMDM_TRAVERSAL_TYPE_BOTTOM_TO_TOP:
         /* Post-Order Traversal */
         if (CM_DLLQ_NEXT (&(pNode->parent_p->child_list), &(pNode->list_node)) !=
               NULL)
         {
            pDllqNode =
               CM_DLLQ_NEXT (&(pNode->parent_p->child_list), &(pNode->list_node));
            pNode =
               CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *, list_node);

#if 0
            IDArray.count_ui--;
            IDArray.uint_arr_p[IDArray.count_ui++] = pNode->id_ui;
#endif

            while (CM_DLLQ_COUNT (&(pNode->child_list)) != 0)
            {
               pDllqNode = CM_DLLQ_FIRST (&(pNode->child_list));
               pNode =
                  CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *, list_node);
#if 0
               IDArray.uint_arr_p[IDArray.count_ui++] = pNode->id_ui;
#endif
            }

            if (cmi_dm_create_id_path_from_node (pNode, &IDArray) != OF_SUCCESS)
            {
               return OF_FAILURE;
            }
            *node_info_pp = cmi_dm_create_node_info_struct (pNode, NULL);
            if (*node_info_pp == NULL)
            {
               of_free (pToken);
               of_free (IDArray.uint_arr_p);
               return OF_FAILURE;
            }
            return_value = OF_SUCCESS;
            break;
         }
         else
         {
            while (pNode->parent_p != NULL)
            {
               if (pNode == pHeadNode)
               {
                  return_value = OF_FAILURE;
                  break;
               }

#if 0
               IDArray.count_ui--;
               IDArray.uint_arr_p[IDArray.count_ui - 1] = pNode->parent_p->id_ui;
#endif

               if (cmi_dm_create_id_path_from_node (pNode, &IDArray) != OF_SUCCESS)
               {
                  of_free (pToken);
                  return OF_FAILURE;
               }
               *node_info_pp = cmi_dm_create_node_info_struct (pNode, NULL);
               if (*node_info_pp == NULL)
               {
                  of_free (pToken);
                  of_free (IDArray.uint_arr_p);
                  return OF_FAILURE;
               }
               return_value = OF_SUCCESS;
               break;
            }
         }
         break;

      default:
         of_free (pToken);
         return OF_FAILURE;
   }

   *opaque_info_pp = (void *) IDArray.uint_arr_p;
   *opaque_len_p = IDArray.count_ui;

   of_free (pToken);
   return return_value;
}

int32_t cmi_dm_get_next_table_n_anchor_node_info (char * head_node_path_p,
      uint32_t traversal_type_ui,
      struct cm_dm_node_info **node_info_pp,
      void **opaque_info_pp, uint32_t *opaque_len_p)
{
   int32_t return_value = OF_FAILURE;
   //char *parent_path_p=NULL;
   //struct cm_dm_data_element *pDMNode = NULL;

   if (unlikely ((!node_info_pp) || (!opaque_info_pp) || (!opaque_len_p)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }
   return_value = cmi_dm_get_next_node_info (head_node_path_p, traversal_type_ui,
         node_info_pp, opaque_info_pp, opaque_len_p);
   while(return_value == OF_SUCCESS)
   {
      if(((*node_info_pp)->element_attrib.element_type == CM_DMNODE_TYPE_ANCHOR) ||
            ((*node_info_pp)->element_attrib.element_type == CM_DMNODE_TYPE_TABLE))
      {
         return OF_SUCCESS;
      }
      return_value = cmi_dm_get_next_node_info (head_node_path_p, traversal_type_ui,
            node_info_pp, opaque_info_pp, opaque_len_p);
   }
   if (return_value == OF_FAILURE)
      return OF_FAILURE;
}


int32_t cmi_dm_get_next_table_n_anchor_element_info (char * head_node_path_p,
      uint32_t traversal_type_ui,
      struct cm_dm_data_element **data_element_p,
      void **opaque_info_pp, uint32_t *opaque_len_p)
{
   int32_t return_value = OF_FAILURE;

   if (unlikely ((!data_element_p) || (!opaque_info_pp) || (!opaque_len_p)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }
   return_value = cmi_dm_get_next_element_info (head_node_path_p, traversal_type_ui,
         data_element_p, opaque_info_pp, opaque_len_p);
   while(return_value == OF_SUCCESS)
   {
      if(((*data_element_p)->element_attrib.element_type == CM_DMNODE_TYPE_ANCHOR) ||
            ((*data_element_p)->element_attrib.element_type == CM_DMNODE_TYPE_TABLE))
      {
         return OF_SUCCESS;
      }
      return_value = cmi_dm_get_next_element_info (head_node_path_p, traversal_type_ui,
            data_element_p, opaque_info_pp, opaque_len_p);
   }
   if (return_value == OF_FAILURE)
      return OF_FAILURE;
}

int32_t cmi_dm_get_next_last_child_table_n_anchor_element_info (char * head_node_path_p,
      uint32_t traversal_type_ui,
      struct cm_dm_data_element **data_element_p,
      void **opaque_info_pp, uint32_t *opaque_len_p)
{
   int32_t return_value = OF_FAILURE;

   if (unlikely ((!data_element_p) || (!opaque_info_pp) || (!opaque_len_p)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }
   return_value = cmi_dm_get_next_last_child_element_info (head_node_path_p, traversal_type_ui,
         data_element_p, opaque_info_pp, opaque_len_p);
   while(return_value == OF_SUCCESS)
   {
      if(((*data_element_p)->element_attrib.element_type == CM_DMNODE_TYPE_ANCHOR) ||
            ((*data_element_p)->element_attrib.element_type == CM_DMNODE_TYPE_TABLE))
      {
         return OF_SUCCESS;
      }
      return_value = cmi_dm_get_next_last_child_element_info (head_node_path_p, traversal_type_ui,
            data_element_p, opaque_info_pp, opaque_len_p);
   }
   if (return_value == OF_FAILURE)
      return OF_FAILURE;
}
int32_t cmi_dm_get_first_element_info (char * head_node_path_p,
      uint32_t traversal_type_ui,
      struct cm_dm_data_element ** data_element_p,
      void ** opaque_info_pp, uint32_t * opaque_len_p)
{
   struct cm_dm_data_element *pNode = NULL;
   struct cm_array_of_uints IDArray;
   UCMDllQNode_t *pDllqNode = NULL;
   uint32_t uiMaxTokenLen = 0;
   char *pToken = NULL;

   if (unlikely ((!data_element_p) || (!opaque_info_pp) || (!opaque_len_p)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Path Name is optional. If it is missing, we traverse entire tree
      from head.
      */
   if ((head_node_path_p == NULL) || (of_strlen (head_node_path_p) == 0))
   {
      pNode = cm_dm_get_root_node ();
      if (unlikely (!pNode))
      {
         // Send specific error
         return OF_FAILURE;
      }
   }
   else
   {
      /* Allocate token buffer */
      uiMaxTokenLen = of_strlen (head_node_path_p) + 1;
      pToken = (char *) of_calloc (1, uiMaxTokenLen);
      if (!pToken)
      {
         return OF_FAILURE;
      }

      /* Verify whether the data path is valid accroding to DMTree */
      if (cmi_dm_remove_keys_from_path (head_node_path_p, pToken, (uiMaxTokenLen - 1)) !=
            OF_SUCCESS)
      {
         of_free (pToken);
         return OF_FAILURE;
      }

      pNode = cmi_dm_get_node_by_namepath (pToken);
      if (unlikely (!pNode))
      {
         of_free (pToken);
         return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
      }
   }

   switch (traversal_type_ui)
   {
      case UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM:
         /* Pre-Order Traversal : Just get the head node
            We are already there.
            */
         //    pDllqNode = CM_DLLQ_FIRST (&(pNode->child_list));
         //   pNode = CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *, list_node);

         break;

      case UCMDM_TRAVERSAL_TYPE_BOTTOM_TO_TOP:
         /* Post-Order Traversal :  Get the bottom most node */
         while (CM_DLLQ_COUNT (&(pNode->child_list)) != 0)
         {
            pDllqNode = CM_DLLQ_FIRST (&(pNode->child_list));
            pNode =
               CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *, list_node);
         }
         break;

      default:
         of_free (pToken);
         return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Construct the OpaqueInfo */
   if (cmi_dm_create_id_path_from_node (pNode, &IDArray) != OF_SUCCESS)
   {
      of_free (pToken);
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   /* Copy all the attributes */
   *data_element_p = pNode;
   if (*data_element_p == NULL)
   {
      of_free (pToken);
      of_free (IDArray.uint_arr_p);
      return OF_FAILURE;
   }

   *opaque_info_pp = (void *) IDArray.uint_arr_p;
   *opaque_len_p = IDArray.count_ui;

   of_free (pToken);
   return OF_SUCCESS;
}

int32_t cmi_dm_get_first_last_child_element_info (char * head_node_path_p,
      uint32_t traversal_type_ui,
      struct cm_dm_data_element ** data_element_p,
      void ** opaque_info_pp, uint32_t * opaque_len_p)
{
   struct cm_dm_data_element *pNode = NULL;
   struct cm_array_of_uints IDArray;
   UCMDllQNode_t *pDllqNode = NULL;
   uint32_t uiMaxTokenLen = 0;
   char *pToken = NULL;

   if (unlikely ((!data_element_p) || (!opaque_info_pp) || (!opaque_len_p)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Path Name is optional. If it is missing, we traverse entire tree
      from head.
      */
   if ((head_node_path_p == NULL) || (of_strlen (head_node_path_p) == 0))
   {
      pNode = cm_dm_get_root_node ();
      if (unlikely (!pNode))
      {
         // Send specific error
         return OF_FAILURE;
      }
   }
   else
   {
      /* Allocate token buffer */
      uiMaxTokenLen = of_strlen (head_node_path_p) + 1;
      pToken = (char *) of_calloc (1, uiMaxTokenLen);
      if (!pToken)
      {
         return OF_FAILURE;
      }

      /* Verify whether the data path is valid accroding to DMTree */
      if (cmi_dm_remove_keys_from_path (head_node_path_p, pToken, (uiMaxTokenLen - 1)) !=
            OF_SUCCESS)
      {
         of_free (pToken);
         return OF_FAILURE;
      }

      pNode = cmi_dm_get_node_by_namepath (pToken);
      if (unlikely (!pNode))
      {
         of_free (pToken);
         return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
      }
   }

   switch (traversal_type_ui)
   {
      case UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM:
         /* Pre-Order Traversal : Just get the head node
            We are already there.
            */
         //    pDllqNode = CM_DLLQ_FIRST (&(pNode->child_list));
         //   pNode = CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *, list_node);

         break;

      case UCMDM_TRAVERSAL_TYPE_BOTTOM_TO_TOP:
         /* Post-Order Traversal :  Get the bottom most node */
         while (CM_DLLQ_COUNT (&(pNode->child_list)) != 0)
         {
            pDllqNode = CM_DLLQ_LAST (&(pNode->child_list));
            pNode =
               CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *, list_node);
         }
         break;

      default:
         of_free (pToken);
         return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Construct the OpaqueInfo */
   if (cmi_dm_create_id_path_from_node (pNode, &IDArray) != OF_SUCCESS)
   {
      of_free (pToken);
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   /* Copy all the attributes */
   *data_element_p = pNode;
   if (*data_element_p == NULL)
   {
      of_free (pToken);
      of_free (IDArray.uint_arr_p);
      return OF_FAILURE;
   }

   *opaque_info_pp = (void *) IDArray.uint_arr_p;
   *opaque_len_p = IDArray.count_ui;

   of_free (pToken);
   return OF_SUCCESS;
}

int32_t cmi_dm_get_next_element_info (char * head_node_path_p,
      uint32_t traversal_type_ui,
      struct cm_dm_data_element ** data_element_p,
      void ** opaque_info_pp, uint32_t * opaque_len_p)
{
   struct cm_dm_data_element *pNode = NULL;
   struct cm_dm_data_element *pHeadNode = NULL;
   struct cm_array_of_uints IDArray;
   UCMDllQNode_t *pDllqNode = NULL;
   int32_t return_value = OF_FAILURE;
   uint32_t uiMaxTokenLen = 0;
   char *pToken = NULL;

   if (unlikely ((!data_element_p) || (!opaque_info_pp) || (!opaque_len_p)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Get the head node. We stop travering when we reach this */
   if ((head_node_path_p == NULL) || (of_strlen (head_node_path_p) == 0))
   {
      pHeadNode = cm_dm_get_root_node ();
      if (unlikely (!pHeadNode))
      {
         /* return error code */
         return OF_FAILURE;
      }
   }
   else
   {
      /* Allocate token buffer */
      uiMaxTokenLen = of_strlen (head_node_path_p) + 1;
      pToken = (char *) of_calloc (1, uiMaxTokenLen);
      if (!pToken)
      {
         return OF_FAILURE;
      }

      /* Verify whether the data path is valid accroding to DMTree */
      if (cmi_dm_remove_keys_from_path (head_node_path_p, pToken, (uiMaxTokenLen - 1)) !=
            OF_SUCCESS)
      {
         of_free (pToken);
         return OF_FAILURE;
      }

      pHeadNode = cmi_dm_get_node_by_namepath (pToken);
      if (unlikely (!pHeadNode))
      {
         of_free (pToken);
         return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
      }
   }

   IDArray.uint_arr_p = (uint32_t *) (*opaque_info_pp);
   IDArray.count_ui = *((uint32_t *) opaque_len_p);
   *opaque_info_pp = NULL;
   *opaque_len_p = 0;

   /* Now get the current node based on OpaqueInfo : that is ID path */
   pNode = cmi_dm_get_node_by_idpath (&IDArray);
   if (unlikely (!pNode))
   {
      of_free (pToken);
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   of_free (IDArray.uint_arr_p);
   IDArray.uint_arr_p = NULL;

   switch (traversal_type_ui)
   {
      case UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM:
         /* Pre-Order Traversal */

         /* If child list is there, get first child */
         if (CM_DLLQ_COUNT (&(pNode->child_list)) != 0)
         {
            pDllqNode = CM_DLLQ_FIRST (&(pNode->child_list));
            pNode =
               CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *, list_node);
            if (cmi_dm_create_id_path_from_node (pNode, &IDArray) != OF_SUCCESS)
            {
               of_free (pToken);
               return OF_FAILURE;
            }
            *data_element_p = pNode;
            if (*data_element_p == NULL)
            {
               of_free (pToken);
               of_free (IDArray.uint_arr_p);
               return OF_FAILURE;
            }
            return_value = OF_SUCCESS;
            break;
         }

         /* Otherwise get the next peer, if exists */
         if (CM_DLLQ_NEXT (&(pNode->parent_p->child_list), &(pNode->list_node)) !=
               NULL)
         {
            pDllqNode =
               CM_DLLQ_NEXT (&(pNode->parent_p->child_list), &(pNode->list_node));
            pNode =
               CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *, list_node);
            if (pNode == pHeadNode)
            {
               /* It is same node as DMPath Node */
               of_free (pToken);
               of_free (IDArray.uint_arr_p);
               return OF_FAILURE;
            }
            if (cmi_dm_create_id_path_from_node (pNode, &IDArray) != OF_SUCCESS)
            {
               of_free (pToken);
               return OF_FAILURE;
            }
            *data_element_p = pNode;
            if (*data_element_p == NULL)
            {
               of_free (pToken);
               of_free (IDArray.uint_arr_p);
               return OF_FAILURE;
            }
            return_value = OF_SUCCESS;
            break;
         }
         else
         {
            /* There are no childs, no peers. Just move on to parent */
            while (pNode->parent_p != NULL)
            {
               /* If parent is Head Node, shouldn't go for its siblings.. */
               if (pNode->parent_p == pHeadNode)
               {
                  of_free (pToken);
                  return OF_FAILURE;
               }
               if ((pNode->parent_p->parent_p != NULL) &&
                     (CM_DLLQ_NEXT
                      (&(pNode->parent_p->parent_p->child_list),
                       &(pNode->parent_p->list_node)) != NULL))
               {
                  pDllqNode =
                     CM_DLLQ_NEXT (&(pNode->parent_p->parent_p->child_list),
                           &(pNode->parent_p->list_node));
                  pNode =
                     CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *, list_node);

                  if (cmi_dm_create_id_path_from_node (pNode, &IDArray) != OF_SUCCESS)
                  {
                     of_free (pToken);
                     return OF_FAILURE;
                  }
                  *data_element_p = pNode;
                  if (*data_element_p == NULL)
                  {
                     of_free (pToken);
                     of_free (IDArray.uint_arr_p);
                     return OF_FAILURE;
                  }
                  return_value = OF_SUCCESS;
                  break;
               }
               else
               {
                  pNode = pNode->parent_p;
                  if (pNode == pHeadNode)
                  {
                     return_value = OF_FAILURE;
                     break;
                  }
               }
            }
         }
         break;

      case UCMDM_TRAVERSAL_TYPE_BOTTOM_TO_TOP:
         /* Post-Order Traversal */
         if (CM_DLLQ_NEXT (&(pNode->parent_p->child_list), &(pNode->list_node)) !=
               NULL)
         {
            pDllqNode =
               CM_DLLQ_NEXT (&(pNode->parent_p->child_list), &(pNode->list_node));
            pNode =
               CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *, list_node);


            while (CM_DLLQ_COUNT (&(pNode->child_list)) != 0)
            {
               pDllqNode = CM_DLLQ_FIRST (&(pNode->child_list));
               pNode =
                  CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *, list_node);
            }

            if (cmi_dm_create_id_path_from_node (pNode, &IDArray) != OF_SUCCESS)
            {
               return OF_FAILURE;
            }
            *data_element_p = pNode;
            if (*data_element_p == NULL)
            {
               of_free (pToken);
               of_free (IDArray.uint_arr_p);
               return OF_FAILURE;
            }
            return_value = OF_SUCCESS;
            break;
         }
         else
         {
            while (pNode->parent_p != NULL)
            {
               if (pNode == pHeadNode)
               {
                  return_value = OF_FAILURE;
                  break;
               }
               if (cmi_dm_create_id_path_from_node (pNode, &IDArray) != OF_SUCCESS)
               {
                  of_free (pToken);
                  return OF_FAILURE;
               }
               *data_element_p = pNode;
               if (*data_element_p == NULL)
               {
                  of_free (pToken);
                  of_free (IDArray.uint_arr_p);
                  return OF_FAILURE;
               }
               return_value = OF_SUCCESS;
               break;
            }
         }
         break;

      default:
         of_free (pToken);
         return OF_FAILURE;
   }

   *opaque_info_pp = (void *) IDArray.uint_arr_p;
   *opaque_len_p = IDArray.count_ui;

   of_free (pToken);
   return return_value;
}


int32_t cmi_dm_get_next_last_child_element_info (char * head_node_path_p,
      uint32_t traversal_type_ui,
      struct cm_dm_data_element ** data_element_p,
      void ** opaque_info_pp, uint32_t * opaque_len_p)
{
   struct cm_dm_data_element *pNode = NULL;
   struct cm_dm_data_element *pHeadNode = NULL;
   struct cm_array_of_uints IDArray;
   UCMDllQNode_t *pDllqNode = NULL;
   int32_t return_value = OF_FAILURE;
   uint32_t uiMaxTokenLen = 0;
   char *pToken = NULL;

   if (unlikely ((!data_element_p) || (!opaque_info_pp) || (!opaque_len_p)))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Get the head node. We stop travering when we reach this */
   if ((head_node_path_p == NULL) || (of_strlen (head_node_path_p) == 0))
   {
      pHeadNode = cm_dm_get_root_node ();
      if (unlikely (!pHeadNode))
      {
         /* return error code */
         return OF_FAILURE;
      }
   }
   else
   {
      /* Allocate token buffer */
      uiMaxTokenLen = of_strlen (head_node_path_p) + 1;
      pToken = (char *) of_calloc (1, uiMaxTokenLen);
      if (!pToken)
      {
         return OF_FAILURE;
      }

      /* Verify whether the data path is valid accroding to DMTree */
      if (cmi_dm_remove_keys_from_path (head_node_path_p, pToken, (uiMaxTokenLen - 1)) !=
            OF_SUCCESS)
      {
         of_free (pToken);
         return OF_FAILURE;
      }

      pHeadNode = cmi_dm_get_node_by_namepath (pToken);
      if (unlikely (!pHeadNode))
      {
         of_free (pToken);
         return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
      }
   }

   IDArray.uint_arr_p = (uint32_t *) (*opaque_info_pp);
   IDArray.count_ui = *((uint32_t *) opaque_len_p);
   *opaque_info_pp = NULL;
   *opaque_len_p = 0;

   /* Now get the current node based on OpaqueInfo : that is ID path */
   pNode = cmi_dm_get_node_by_idpath (&IDArray);
   if (unlikely (!pNode))
   {
      of_free (pToken);
      return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
   }

   of_free (IDArray.uint_arr_p);
   IDArray.uint_arr_p = NULL;

   switch (traversal_type_ui)
   {
      case UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM:
         /* Pre-Order Traversal */

         /* If child list is there, get first child */
         if (CM_DLLQ_COUNT (&(pNode->child_list)) != 0)
         {
            pDllqNode = CM_DLLQ_LAST (&(pNode->child_list));
            pNode =
               CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *, list_node);
            if (cmi_dm_create_id_path_from_node (pNode, &IDArray) != OF_SUCCESS)
            {
               of_free (pToken);
               return OF_FAILURE;
            }
            *data_element_p = pNode;
            if (*data_element_p == NULL)
            {
               of_free (pToken);
               of_free (IDArray.uint_arr_p);
               return OF_FAILURE;
            }
            return_value = OF_SUCCESS;
            break;
         }

         /* Otherwise get the next peer, if exists */
         if (CM_DLLQ_PREV (&(pNode->parent_p->child_list), &(pNode->list_node)) !=
               NULL)
         {
            pDllqNode =
               CM_DLLQ_PREV (&(pNode->parent_p->child_list), &(pNode->list_node));
            pNode =
               CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *, list_node);
            if (pNode == pHeadNode)
            {
               /* It is same node as DMPath Node */
               of_free (pToken);
               of_free (IDArray.uint_arr_p);
               return OF_FAILURE;
            }
            if (cmi_dm_create_id_path_from_node (pNode, &IDArray) != OF_SUCCESS)
            {
               of_free (pToken);
               return OF_FAILURE;
            }
            *data_element_p = pNode;
            if (*data_element_p == NULL)
            {
               of_free (pToken);
               of_free (IDArray.uint_arr_p);
               return OF_FAILURE;
            }
            return_value = OF_SUCCESS;
            break;
         }
         else
         {
            /* There are no childs, no peers. Just move on to parent */
            while (pNode->parent_p != NULL)
            {
               /* If parent is Head Node, shouldn't go for its siblings.. */
               if (pNode->parent_p == pHeadNode)
               {
                  of_free (pToken);
                  return OF_FAILURE;
               }
               if ((pNode->parent_p->parent_p != NULL) &&
                     (CM_DLLQ_PREV
                      (&(pNode->parent_p->parent_p->child_list),
                       &(pNode->parent_p->list_node)) != NULL))
               {
                  pDllqNode =
                     CM_DLLQ_PREV (&(pNode->parent_p->parent_p->child_list),
                           &(pNode->parent_p->list_node));
                  pNode =
                     CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *, list_node);

                  if (cmi_dm_create_id_path_from_node (pNode, &IDArray) != OF_SUCCESS)
                  {
                     of_free (pToken);
                     return OF_FAILURE;
                  }
                  *data_element_p = pNode;
                  if (*data_element_p == NULL)
                  {
                     of_free (pToken);
                     of_free (IDArray.uint_arr_p);
                     return OF_FAILURE;
                  }
                  return_value = OF_SUCCESS;
                  break;
               }
               else
               {
                  pNode = pNode->parent_p;
                  if (pNode == pHeadNode)
                  {
                     return_value = OF_FAILURE;
                     break;
                  }
               }
            }
         }
         break;

      case UCMDM_TRAVERSAL_TYPE_BOTTOM_TO_TOP:
         /* Post-Order Traversal */
         if (CM_DLLQ_PREV (&(pNode->parent_p->child_list), &(pNode->list_node)) !=
               NULL)
         {
            pDllqNode =
               CM_DLLQ_PREV (&(pNode->parent_p->child_list), &(pNode->list_node));
            pNode =
               CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *, list_node);


            while (CM_DLLQ_COUNT (&(pNode->child_list)) != 0)
            {
               pDllqNode = CM_DLLQ_LAST (&(pNode->child_list));
               pNode =
                  CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *, list_node);
            }

            if (cmi_dm_create_id_path_from_node (pNode, &IDArray) != OF_SUCCESS)
            {
               return OF_FAILURE;
            }
            *data_element_p = pNode;
            if (*data_element_p == NULL)
            {
               of_free (pToken);
               of_free (IDArray.uint_arr_p);
               return OF_FAILURE;
            }
            return_value = OF_SUCCESS;
            break;
         }
         else
         {
            while (pNode->parent_p != NULL)
            {
               if (pNode == pHeadNode)
               {
                  return_value = OF_FAILURE;
                  break;
               }
               if (cmi_dm_create_id_path_from_node (pNode, &IDArray) != OF_SUCCESS)
               {
                  of_free (pToken);
                  return OF_FAILURE;
               }
               *data_element_p = pNode;
               if (*data_element_p == NULL)
               {
                  of_free (pToken);
                  of_free (IDArray.uint_arr_p);
                  return OF_FAILURE;
               }
               return_value = OF_SUCCESS;
               break;
            }
         }
         break;

      default:
         of_free (pToken);
         return OF_FAILURE;
   }

   *opaque_info_pp = (void *) IDArray.uint_arr_p;
   *opaque_len_p = IDArray.count_ui;

   of_free (pToken);
   return return_value;
}
/**************************************************************************

/**************************************************************************
  Function Name : cm_dm_cleanup_array_of_node_info
  Input Args    : arr_node_info_structs_p -Pointer to Array of NOde Info structures
  Output Args   : None
Description   : This API used to Free the Array of NodeInfo structure and its members
Return Values : None
 *************************************************************************/
void cmi_dm_cleanup_array_of_node_info (struct cm_array_of_structs * arr_node_info_structs_p)
{
   int32_t i;
   struct cm_dm_node_info *node_info_p = NULL;

   if (arr_node_info_structs_p)
   {
      if (arr_node_info_structs_p->struct_arr_p)
      {
         node_info_p = arr_node_info_structs_p->struct_arr_p;
         for (i = 0; i < arr_node_info_structs_p->count_ui; i++)
         {
            if (node_info_p[i].name_p)
               of_free (node_info_p[i].name_p);

            if (node_info_p[i].friendly_name_p)
               of_free (node_info_p[i].friendly_name_p);

            if (node_info_p[i].description_p)
               of_free (node_info_p[i].description_p);

            if (node_info_p[i].dm_path_p)
               of_free (node_info_p[i].dm_path_p);
         }
         of_free (arr_node_info_structs_p->struct_arr_p);
      }
      of_free (arr_node_info_structs_p);
   }
}

/**************************************************************************
  Function Name : UCMDM_FreeNodeInfo
  Input Args    : node_info_p - Pointer to NodeInfo structure
  Output Args   : None
Description   : This API is used to free NodeInfo structure
Return Values : None
 *************************************************************************/
void cmi_dm_free_node_info (struct cm_dm_node_info * node_info_p)
{
   if (node_info_p)
   {
      if (node_info_p->name_p)
         of_free (node_info_p->name_p);

      if (node_info_p->friendly_name_p)
         of_free (node_info_p->friendly_name_p);

      if (node_info_p->description_p)
         of_free (node_info_p->description_p);

      if (node_info_p->dm_path_p)
         of_free (node_info_p->dm_path_p);

      of_free (node_info_p);
   }
}

void cmi_dm_free_node_infoArray (struct cm_array_of_structs * array_of_node_info_p)
{
   int32_t ii;
   struct cm_dm_node_info *node_info_p;

   for (ii = 0; ii < array_of_node_info_p->count_ui; ii++)
   {
      node_info_p=(struct cm_dm_node_info *) array_of_node_info_p->struct_arr_p +ii;
      if (node_info_p)
      {
         if (node_info_p->name_p)
            of_free (node_info_p->name_p);
         if (node_info_p->friendly_name_p)
            of_free (node_info_p->friendly_name_p);

         if (node_info_p->description_p)
            of_free (node_info_p->description_p);

         if (node_info_p->dm_path_p)
            of_free (node_info_p->dm_path_p);
      }
   }
   of_free (array_of_node_info_p->struct_arr_p);
   of_free (array_of_node_info_p);
}

/*-----------------------------------------------------------------------  
  Internal Routines 
  -----------------------------------------------------------------------*/

/**************************************************************************
  Function Name : cm_dm_get_root_node
  Input Args    : None
  Output Args   : None
Description   : This API used to Get root Node of DM Template Tree
Return Values : Pointer to Root Node
 *************************************************************************/
struct cm_dm_data_element *cm_dm_get_root_node (void)
{
   return pDMTree;
}

/**************************************************************************
  Function Name : cmi_dm_get_node_by_namepath
  Input Args    : name_path_p - Pointer to Name Path
  Output Args   : None
Description   : This API is used to Get DM Node by Name Path
Return Values : Pointer to DM Node
 *************************************************************************/
struct cm_dm_data_element *cmi_dm_get_node_by_namepath (char * name_path_p)
{
   char *pToken = NULL;
   uint32_t uiMaxTokenLen;
   uint32_t uindex_i = 0;
   struct cm_dm_data_element *pNode = NULL;
   uint32_t return_value = UCMDM_PATH_PARSING_REMAINING;
   char *tm_name_path_p = NULL;

   if ((!name_path_p))
   {
      return NULL;
   }

   uiMaxTokenLen = of_strlen (name_path_p) + 1;
   if (uiMaxTokenLen == 1)
   {
      return NULL;
   }
   /* As we distroy path while tokenizing, let us operate on a duplicate
      path entry. So that input will not be modified, and caller can
      use it later.
      */
   tm_name_path_p = (char *) of_calloc (1, uiMaxTokenLen);
   if (!tm_name_path_p)
   {
      return NULL;
   }

   /* Allocate token buffer */
   pToken = (char *) of_calloc (1, uiMaxTokenLen);
   if (!pToken)
   {
      of_free (tm_name_path_p);
      return NULL;
   }

   of_strncpy (tm_name_path_p, name_path_p, (uiMaxTokenLen - 1));

   /* Tokenize the input string */
   pNode = pDMTree;
   if (!pNode)
   {
      of_free (tm_name_path_p);
      of_free (pToken);
      return NULL;
   }
   if (!of_strcmp (name_path_p, pNode->name_p))
   {
      of_free (tm_name_path_p);
      of_free (pToken);
      return pNode;
   }
   while (return_value == UCMDM_PATH_PARSING_REMAINING)
   {
      return_value = cmi_dm_get_path_token (tm_name_path_p, pToken, uiMaxTokenLen, &uindex_i);
      /* Root Node */

      /*first token is root node name */
      if (!of_strcmp (pNode->name_p, pToken))
      {
         if (pDMTree != pNode)
         {
            pNode = NULL;
            break;
         }
      }
      else
      {
         /* Find the matching child */
         pNode = cmi_dm_get_child_by_name (pNode, pToken);
         if (!pNode)
         {
            break;
         }
         if (return_value == UCMDM_PATH_PARSING_COMPLETED)
         {
            break;
         }
      }
   }

   of_free (tm_name_path_p);
   of_free (pToken);
   return pNode;
}

char *cmi_dm_make_instance_path (char * pSrcInstPath, char * pTplPath)
{
   char *pToken = NULL;
   char *pTplToken = NULL;
   uint32_t uiMaxTokenLen;
   uint32_t uiResultLen;
   uint32_t uiTplPathLen;
   uint32_t uiInstIndex = 0;
   uint32_t uiTplIndex = 0;
   uint32_t return_value1 = UCMDM_PATH_PARSING_REMAINING;
   uint32_t return_value2 = UCMDM_PATH_PARSING_REMAINING;
   uint32_t iPrevRet2 = UCMDM_PATH_PARSING_REMAINING;
   char *tm_name_path_p = NULL;
   char *pTmpTplPath = NULL;
   char *result_path_p, *result_tmp_path_p;

   if ((!pSrcInstPath) || (!pTplPath))
   {
      return NULL;
   }

   uiMaxTokenLen = of_strlen (pSrcInstPath) + 1;
   if (uiMaxTokenLen == 1)
   {
      return NULL;
   }

   uiResultLen = of_strlen (pSrcInstPath) + of_strlen (pTplPath) + 1;

   /* As we distroy path while tokenizing, let us operate on a duplicate
      path entry. So that input will not be modified, and caller can
      use it later.
      */
   tm_name_path_p = (char *) of_calloc (1, uiMaxTokenLen);
   if (!tm_name_path_p)
   {
      return NULL;
   }
   uiTplPathLen = of_strlen (pTplPath) + 1;

   pTmpTplPath = of_calloc (1, uiTplPathLen);
   if (!pTmpTplPath)
   {
      of_free (tm_name_path_p);
      return NULL;
   }

   result_path_p = of_calloc (1, uiResultLen);
   if (!result_path_p)
   {
      of_free (tm_name_path_p);
      of_free (pTmpTplPath);
      return NULL;
   }
   result_tmp_path_p = result_path_p;

   /* Allocate token buffer */
   pToken = (char *) of_calloc (1, uiMaxTokenLen);
   if (!pToken)
   {
      of_free (tm_name_path_p);
      of_free (pTmpTplPath);
      of_free (result_path_p);
      return NULL;
   }

   pTplToken = (char *) of_calloc (1, uiTplPathLen);
   if (!pTplToken)
   {
      of_free (pToken);
      of_free (tm_name_path_p);
      of_free (pTmpTplPath);
      of_free (result_path_p);
      return NULL;

   }

   of_strncpy (tm_name_path_p, pSrcInstPath, (uiMaxTokenLen - 1));
   of_strncpy (pTmpTplPath, pTplPath, (uiTplPathLen - 1));

   return_value1 =
      cmi_dm_get_path_token (tm_name_path_p, pToken, uiMaxTokenLen, &uiInstIndex);
   return_value2 = cmi_dm_get_path_token (pTplPath, pTplToken, uiTplPathLen, &uiTplIndex);

   while ((return_value1 == UCMDM_PATH_PARSING_REMAINING)
         && (return_value2 == UCMDM_PATH_PARSING_REMAINING))
   {
      /* Root Node */
      if (!of_strcmp (pToken, pTplToken))
      {
         of_strncat (result_tmp_path_p, pToken, of_strlen (pToken));
         iPrevRet2 = return_value2;
         return_value1 =
            cmi_dm_get_path_token (tm_name_path_p, pToken, uiMaxTokenLen, &uiInstIndex);
         return_value2 =
            cmi_dm_get_path_token (pTplPath, pTplToken, uiTplPathLen, &uiTplIndex);
         if (iPrevRet2 == UCMDM_PATH_PARSING_REMAINING)
         {
            if (*pToken == '{')
            {
               of_strncat (result_tmp_path_p, pToken, of_strlen (pToken));
               return_value1 =
                  cmi_dm_get_path_token (tm_name_path_p, pToken, uiMaxTokenLen,
                        &uiInstIndex);
            }
            of_strncat (result_tmp_path_p, ".", 1);
         }
      }
      else
      {
         break;
      }
   }

   if ((return_value1 == UCMDM_PATH_PARSING_REMAINING)
         && (return_value2 == UCMDM_PATH_PARSING_REMAINING))

   {
      of_strncat (result_tmp_path_p, pTplToken, of_strlen (pTplToken));
      of_strncat (result_tmp_path_p, ".", 1);
      of_strncat (result_tmp_path_p, &pTplPath[uiTplIndex],
            uiTplPathLen - uiTplIndex);
   }
   else if (return_value2 == UCMDM_PATH_PARSING_COMPLETED)
   {
      of_strncat (result_tmp_path_p, pTplToken, of_strlen (pTplToken));
   }
   else if (return_value1 == UCMDM_PATH_PARSING_COMPLETED)
   {
      of_strncat (result_tmp_path_p, &pTplPath[uiTplIndex],
            uiTplPathLen - uiTplIndex);
   }

   of_free (pToken);
   of_free (tm_name_path_p);
   of_free (pTmpTplPath);
   of_free(pTplToken); //CID 336
   return result_path_p;
}

/**************************************************************************
  Function Name : cmi_dm_get_node_by_idpath
  Input Args    : pIDArray - Pointer to DM Node ID Array
  Output Args   : None
Description   : This API is used to Get DM Node by ID Path
Return Values : Pointer to DM Node
 *************************************************************************/

struct cm_dm_data_element *cmi_dm_get_node_by_idpath (struct cm_array_of_uints * pIDArray)
{
   uint32_t i;
   struct cm_dm_data_element *pNode = NULL;

   if (unlikely ((pIDArray == NULL) || (pIDArray->count_ui == 0)))
   {
      return NULL;
   }

   if (unlikely ((pDMTree == NULL) || (pIDArray->uint_arr_p[0] != pDMTree->id_ui)))
   {
      return NULL;
   }

   pNode = pDMTree;

   for (i = 1; i < pIDArray->count_ui; i++)
   {
      pNode = cmi_dm_get_child_by_id (pNode, pIDArray->uint_arr_p[i]);
      if (!pNode)
      {
         break;
      }
   }

   return pNode;
}

/**************************************************************************
  Function Name : cmi_dm_get_child_by_id
  Input Args    : parent_node_p - Pointer to Parent DM Node 
  : uiChildID - DM Child Node ID
  Output Args   : None
Description   : This API is used to Get Child DM Node, given Child Node ID and
: Parent Node pointer
Return Values : Pointer to DM Child Node
 *************************************************************************/
struct cm_dm_data_element *cmi_dm_get_child_by_id (struct cm_dm_data_element * parent_node_p,
      uint32_t uiChildID)
{
   UCMDllQNode_t *pNode = NULL;
   UCMDllQ_t *pDll = NULL;
   struct cm_dm_data_element *pChildNode = NULL;

   if (unlikely (parent_node_p == NULL))
   {
      return NULL;
   }

   pDll = &(parent_node_p->child_list);

   CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
   {
      pChildNode = CM_DLLQ_LIST_MEMBER (pNode, struct cm_dm_data_element *, list_node);
      if (!pChildNode)
      {
         return NULL;
      }
      if (pChildNode->id_ui == uiChildID)
      {
         return pChildNode;
      }
   }

   return NULL;
}

/**************************************************************************
  Function Name : cmi_dm_get_child_by_name
  Input Args    : parent_node_p - Pointer to Parent DM Node 
  : pChildName - DM Child Node Name
  Output Args   : None
Description   : This API is used to Get Child DM Node, given Child Node Name and
: Parent Node pointer
Return Values : Pointer to DM Child Node
 *************************************************************************/
struct cm_dm_data_element *cmi_dm_get_child_by_name (struct cm_dm_data_element * parent_node_p,
      char * pChildName)
{
   UCMDllQNode_t *pNode;
   UCMDllQ_t *pDll;
   struct cm_dm_data_element *pChildNode = NULL;

   if ((!parent_node_p) || (!pChildName))
   {
      return NULL;
   }

   pDll = &(parent_node_p->child_list);

   CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
   {
      pChildNode = CM_DLLQ_LIST_MEMBER (pNode, struct cm_dm_data_element *, list_node);
      if (!pChildNode)
      {
         return NULL;
      }
      if (!of_strcmp (pChildNode->name_p, pChildName))
      {
         return pChildNode;
      }
   }

   return NULL;
}

/**************************************************************************
  Function Name : cmi_dm_get_key_child_datatype
  Input Args    : parent_node_p - Pointer to Parent DM Node 
  Output Args   : None
Description   : This API is used to Get Data Type of Key Child DM Node for given
: Parent Node 
Return Values : Key Child DM Node Data Type
 *************************************************************************/
uint32_t cmi_dm_get_key_child_datatype (struct cm_dm_data_element * parent_node_p)
{
   UCMDllQNode_t *pNode = NULL;
   UCMDllQ_t *pDll = NULL;
   struct cm_dm_data_element *pChildNode = NULL;

   if (!parent_node_p)
   {
      return CM_DATA_TYPE_UNKNOWN;
   }

   pDll = &(parent_node_p->child_list);

   CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
   {
      pChildNode = CM_DLLQ_LIST_MEMBER (pNode, struct cm_dm_data_element *, list_node);
      if (pChildNode->element_attrib.key_b == TRUE)
      {
         return pChildNode->data_attrib.data_type;
      }
   }

   return CM_DATA_TYPE_UNKNOWN;
}

struct cm_dm_data_element *cmi_dm_get_key_child_element (struct cm_dm_data_element *
      parent_node_p)
{
   UCMDllQNode_t *pNode = NULL;
   UCMDllQ_t *pDll = NULL;
   struct cm_dm_data_element *pChildNode = NULL;

   if (!parent_node_p)
   {
      return NULL;
   }

   pDll = &(parent_node_p->child_list);

   CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
   {
      pChildNode = CM_DLLQ_LIST_MEMBER (pNode, struct cm_dm_data_element *, list_node);
      if (pChildNode->element_attrib.key_b == TRUE)
      {
         return pChildNode;
      }
   }

   return NULL;
}

/**************************************************************************
  Function Name : cmi_dm_get_key_child_info 
  Input Args    : node_path_p - Pointer to DM Path 
  Output Args   : key_child_info_p - Double pointer to ChildNode Info structure
Description   : This API is used to get the KeyChildNode Info from the given
: DM Path.
Return Values : OF_SUCCESS / Appropriate Error code
 *************************************************************************/
int32_t cmi_dm_get_key_child_info (char * node_path_p,
      struct cm_dm_node_info ** key_child_info_p)
{
   struct cm_dm_data_element *pNode, *pChildNode;
   struct cm_dm_node_info *node_info_p = NULL;
   UCMDllQNode_t *pDllQNode;
   UCMDllQ_t *pDll;
   char *pToken;
   uint32_t uiMaxTokenLen;
   unsigned char bChildExists = FALSE;

   if (unlikely (!node_path_p))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   /* Allocate token buffer */
   uiMaxTokenLen = of_strlen (node_path_p) + 1;

   pToken = (char *) of_calloc (1, uiMaxTokenLen);
   if (!pToken)
   {
      return OF_FAILURE;
   }

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

   if (pNode->element_attrib.element_type == CM_DMNODE_TYPE_TABLE)
   {
      pDll = &(pNode->child_list);

      CM_DLLQ_SCAN (pDll, pDllQNode, UCMDllQNode_t *)
      {
         pChildNode =
            CM_DLLQ_LIST_MEMBER (pDllQNode, struct cm_dm_data_element *, list_node);

         if(pChildNode)
            bChildExists = TRUE;
         if (pChildNode->element_attrib.key_b == TRUE)
         {
            node_info_p = cmi_dm_create_node_info_struct (pChildNode, pToken);
            if (unlikely (node_info_p == NULL))
            {
               *key_child_info_p = NULL;
               of_free (pToken);
               return CM_DM_ERR_MEM_RESOURCES;
            }
         }
      }
      if (node_info_p)
      {
         *key_child_info_p = node_info_p;
         of_free (pToken);
         return OF_SUCCESS;
      }
   }

   of_free (pToken);
   if(bChildExists == TRUE)
   {
      return CM_COMMON_DM_JE_NO_KEY_FOR_TABLE;
   }
   else
   {
      return CM_DM_ERR_INVALID_ELEMENT;
   }
}

#if 0
int32_t cmi_dm_get_key_child_info_UsingNode (struct cm_dm_data_element * pNode,
      struct cm_dm_node_info ** key_child_info_p)
{
   struct cm_dm_data_element *pChildNode;
   struct cm_dm_node_info *node_info_p = NULL;
   UCMDllQNode_t *pDllQNode;
   UCMDllQ_t *pDll;
   char *pToken;

   if (unlikely (!pNode))
   {
      return CM_DM_ERR_INVALID_INPUTS;
   }

   if (pNode->element_attrib.element_type == CM_DMNODE_TYPE_TABLE)
   {
      pDll = &(pNode->child_list);

      CM_DLLQ_SCAN (pDll, pDllQNode, UCMDllQNode_t *)
      {
         pChildNode =
            CM_DLLQ_LIST_MEMBER (pDllQNode, struct cm_dm_data_element *, list_node);
         if (pChildNode->element_attrib.key_b == TRUE)
         {
            node_info_p = cmi_dm_create_node_info_struct (pChildNode, pToken);
            if (unlikely (node_info_p == NULL))
            {
               *key_child_info_p = NULL;
               of_free (pToken);
               return CM_DM_ERR_MEM_RESOURCES;
            }
         }
      }
      if (node_info_p)
      {
         *key_child_info_p = node_info_p;
         of_free (pToken);
         return OF_SUCCESS;
      }
   }

   return CM_DM_ERR_INVALID_ELEMENT;
}
#endif
/**************************************************************************
  Function Name : cmi_dm_attach_child 
  Input Args    : parent_node_p - Pointer to Parent DM Node 
  : pChildNode - Pointer to Child Node
  Output Args   : None
Description   : This API is used to Attach Child Node to its parent in DM Tree
Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_attach_child (struct cm_dm_data_element * parent_node_p,
      struct cm_dm_data_element * pChildNode)
{
   ucmdm_RolePermission_t *role_perm_p = NULL;

   if (parent_node_p && pChildNode)
   {
      pChildNode->parent_p = parent_node_p;
      CM_DLLQ_APPEND_NODE (&(parent_node_p->child_list), &(pChildNode->list_node));
      if (pChildNode->element_attrib.device_specific_b == TRUE)
      {
         role_perm_p =
            (ucmdm_RolePermission_t *) of_calloc (1, sizeof (ucmdm_RolePermission_t));
         if (role_perm_p == NULL)
         {
            return OF_SUCCESS;
         }
         role_perm_p->permissions = CM_PERMISSION_READ_WRITE;
         of_strncpy (role_perm_p->role, CM_DM_DEFAULT_ROLE, UCMDM_ADMIN_ROLE_MAX_LEN);
	 #ifdef CM_ROLES_PERM_SUPPORT         
         /* Attach to the Role-Permission list */
         if (cmi_dm_attach_role_perm_entry (pChildNode, role_perm_p) != OF_SUCCESS)
         {
            of_free (role_perm_p);
            return OF_SUCCESS;
         }
	 #endif
      }
      return OF_SUCCESS;
   }

   return OF_FAILURE;
}

/**************************************************************************
  Function Name : cmi_dm_traverse_dm_tree
  Input Args    : head_node_path_p - Pointer to Head DM Node Path 
  : traversal_type_ui- UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM or
  :          UCMDM_TRAVERSAL_TYPE_BOTTOM_TO_TOP 
  : pCallbackFnPtr - Pointer to  Function to be exeucuted at each Node
  Output Args   : None
Description   : This API is used by Management Engine or Applications  to Traverse
: DM Tree in top to bottom or bottom to top  approach  from given Head
: Node Path and executes Callback function at each Node.  
Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_traverse_dm_tree (char * head_node_path_p,
      uint32_t traversal_type_ui,
      UCMDM_TRAVERSE_CALLBK_FN pCallbackFnPtr)
{
   struct cm_dm_data_element *pNode = NULL;
   struct cm_dm_data_element *pHeadNode = NULL;
   struct cm_dm_node_info *node_info_p = NULL;
   UCMDllQNode_t *pDllqNode = NULL;
   unsigned char bDone;

   if (unlikely (pCallbackFnPtr == NULL))
   {
      return OF_FAILURE;
   }

   if ((head_node_path_p == NULL) || (of_strlen (head_node_path_p) == 0))
   {
      pHeadNode = cm_dm_get_root_node ();
   }
   else
   {
      pHeadNode = cmi_dm_get_node_by_namepath (head_node_path_p);
      if (unlikely (!pHeadNode))
      {
         return CM_DM_ERR_PATH_DOES_NOT_EXISTS;
      }
   }

   switch (traversal_type_ui)
   {
      case UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM:
         pNode = pHeadNode;
         bDone = FALSE;

         do
         {
            node_info_p = cmi_dm_create_node_info_struct (pNode, NULL);
            if (node_info_p == NULL)
            {
               return OF_FAILURE;
            }
            pCallbackFnPtr (node_info_p);
            while (CM_DLLQ_COUNT (&(pNode->child_list)) != 0)
            {
               pDllqNode = CM_DLLQ_FIRST (&(pNode->child_list));
               pNode =
                  CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *, list_node);
               node_info_p = cmi_dm_create_node_info_struct (pNode, NULL);
               if (node_info_p == NULL)
               {
                  return OF_FAILURE;
               }
               pCallbackFnPtr (node_info_p);
            }

            if (CM_DLLQ_NEXT (&(pNode->parent_p->child_list), &(pNode->list_node)) !=
                  NULL)
            {
               pDllqNode =
                  CM_DLLQ_NEXT (&(pNode->parent_p->child_list), &(pNode->list_node));
               pNode =
                  CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *, list_node);
            }
            else
            {
               while (pNode->parent_p != NULL)
               {
                  if ((pNode->parent_p->parent_p != NULL) &&
                        (CM_DLLQ_NEXT
                         (&(pNode->parent_p->parent_p->child_list),
                          &(pNode->parent_p->list_node)) != NULL))
                  {
                     pDllqNode =
                        CM_DLLQ_NEXT (&(pNode->parent_p->parent_p->child_list),
                              &(pNode->parent_p->list_node));
                     pNode =
                        CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *,
                              list_node);
                     break;
                  }
                  else
                  {
                     pNode = pNode->parent_p;
                     if (pNode == pHeadNode)
                     {
                        bDone = TRUE;
                        break;
                     }
                  }
               }
            }

            if (bDone == TRUE)
            {
               break;
            }
         }
         while (pNode != NULL);
         break;

      case UCMDM_TRAVERSAL_TYPE_BOTTOM_TO_TOP:
         pNode = pHeadNode;
         bDone = FALSE;

         do
         {
            while (CM_DLLQ_COUNT (&(pNode->child_list)) != 0)
            {
               pDllqNode = CM_DLLQ_FIRST (&(pNode->child_list));
               pNode =
                  CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *, list_node);
            }

            node_info_p = cmi_dm_create_node_info_struct (pNode, NULL);
            if (node_info_p == NULL)
            {
               return OF_FAILURE;
            }
            pCallbackFnPtr (node_info_p);

            if (CM_DLLQ_NEXT (&(pNode->parent_p->child_list), &(pNode->list_node)) !=
                  NULL)
            {
               pDllqNode =
                  CM_DLLQ_NEXT (&(pNode->parent_p->child_list), &(pNode->list_node));
               pNode =
                  CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *, list_node);
            }
            else
            {
               while (pNode->parent_p != NULL)
               {
                  node_info_p = cmi_dm_create_node_info_struct (pNode, NULL);
                  if (node_info_p == NULL)
                  {
                     return OF_FAILURE;
                  }
                  pCallbackFnPtr (node_info_p);

                  if ((pNode->parent_p->parent_p != NULL) &&
                        (CM_DLLQ_NEXT
                         (&(pNode->parent_p->parent_p->child_list),
                          &(pNode->parent_p->list_node)) != NULL))
                  {
                     pDllqNode =
                        CM_DLLQ_NEXT (&(pNode->parent_p->parent_p->child_list),
                              &(pNode->parent_p->list_node));
                     pNode =
                        CM_DLLQ_LIST_MEMBER (pDllqNode, struct cm_dm_data_element *,
                              list_node);
                     break;
                  }
                  else
                  {
                     pNode = pNode->parent_p;
                     if (pNode == pHeadNode)
                     {
                        bDone = TRUE;
                        break;
                     }
                  }
               }
            }

            if (bDone == TRUE)
            {
               break;
            }

         }
         while (pNode != NULL);
         break;

      default:
         break;
   }

   of_free (node_info_p);

   return OF_SUCCESS;
}

/**************************************************************************
  Function Name : cmi_dm_create_child_name_array
  Input Args    : parent_node_p - Pointer to Parent DM Node
  Output Args   : arr_str_pp - Double Pointer to Array of Strings used to hold
  :             Child Node Names
Description   : This API extracts child node names for given parent dm node.
Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_create_child_name_array (struct cm_dm_data_element * parent_node_p,
      struct cm_array_of_strings ** arr_str_pp)
{
   struct cm_array_of_strings *arr_str_p = NULL;
   UCMDllQ_t *pDll;
   UCMDllQNode_t *pNode = NULL;
   struct cm_dm_data_element *pChildNode = NULL;
   uint32_t uindex_i = 0;
   int32_t return_value = OF_SUCCESS;

   if ((!parent_node_p) || (!arr_str_pp))
   {
      return OF_FAILURE;
   }
   do
   {
      arr_str_p =
         (struct cm_array_of_strings *) of_calloc (1, sizeof (struct cm_array_of_strings));
      if (unlikely (arr_str_p == NULL))
      {
         return_value = CM_DM_ERR_MEM_RESOURCES;
         break;
      }

      arr_str_p->count_ui = CM_DLLQ_COUNT (&(parent_node_p->child_list));
      if (arr_str_p->count_ui)
      {
         arr_str_p->string_arr_p =
            (char **) of_calloc (arr_str_p->count_ui, sizeof (char *));
         if (unlikely (arr_str_p->string_arr_p == NULL))
         {
            return_value = CM_DM_ERR_MEM_RESOURCES;
            break;
         }

         uindex_i = 0;
         pDll = &(parent_node_p->child_list);
         CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
         {
            pChildNode =
               CM_DLLQ_LIST_MEMBER (pNode, struct cm_dm_data_element *, list_node);
            arr_str_p->string_arr_p[uindex_i] =
               (char *) of_calloc (1, of_strlen (pChildNode->name_p) + 1);
            if (!arr_str_p->string_arr_p[uindex_i])
            {
               return_value = CM_DM_ERR_MEM_RESOURCES;
               break;
            }
            of_strcpy (arr_str_p->string_arr_p[uindex_i], pChildNode->name_p);
            uindex_i++;
         }
      }
   }
   while (0);

   if ((return_value != OF_SUCCESS) && (arr_str_p != NULL))
   {
      cmi_dm_cleanup_array_of_strings (arr_str_p);
      *arr_str_pp = NULL;
   }
   else
   {
      *arr_str_pp = arr_str_p;
   }

   return return_value;
}

/**************************************************************************
  Function Name : cmi_dm_create_child_info_array
  Input Args    : parent_node_p - Pointer to Parent DM Node
  Output Args   : ppChildInfoArr - Double Pointer to Array of Structures used to hold
  :             Child Node Info
Description   : This API extracts child node Info for given parent dm node.
Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_create_child_info_array (struct cm_dm_data_element * parent_node_p,
      struct cm_array_of_structs ** ppChildInfoArr)
{
   struct cm_array_of_structs *arr_structs_p = NULL;
   UCMDllQ_t *pDll = NULL;
   UCMDllQNode_t *pNode = NULL;
   struct cm_dm_data_element *pChildNode = NULL;
   struct cm_dm_node_info *node_info_p = NULL;
   struct cm_dm_node_info *node_info_array_p = NULL;
   uint32_t uindex_i = 0;
   int32_t return_value = OF_SUCCESS;
   unsigned char ordered_table_b = FALSE;
   unsigned char bCpyKeyToRelNode = FALSE;

   if ((!parent_node_p) || (!ppChildInfoArr))
   {
      return OF_FAILURE;
   }

   do
   {
      arr_structs_p =
         (struct cm_array_of_structs *) of_calloc (1, sizeof (struct cm_array_of_structs));
      if (unlikely (arr_structs_p == NULL))
      {
         return_value = CM_DM_ERR_MEM_RESOURCES;
         break;
      }

      arr_structs_p->count_ui = CM_DLLQ_COUNT (&(parent_node_p->child_list));
      if (arr_structs_p->count_ui == 0)
      {
         break;
      }
      if (parent_node_p->element_attrib.ordered_table_b == TRUE)
      {
         if (parent_node_p->element_attrib.element_type == CM_DMNODE_TYPE_TABLE)
         {
            ordered_table_b = TRUE;
            arr_structs_p->count_ui = arr_structs_p->count_ui + 2;
         }
      }

      if (arr_structs_p->count_ui)
      {
         node_info_array_p = (struct cm_dm_node_info *) of_calloc (arr_structs_p->count_ui,
               sizeof (struct cm_dm_node_info));
         if (unlikely (node_info_array_p == NULL))
         {
            return_value = CM_DM_ERR_MEM_RESOURCES;
            break;
         }

         uindex_i = 0;
         pDll = &(parent_node_p->child_list);
         CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
         {
            pChildNode =
               CM_DLLQ_LIST_MEMBER (pNode, struct cm_dm_data_element *, list_node);
            node_info_p = cmi_dm_create_node_info_struct (pChildNode, NULL);
            if (node_info_p == NULL)
            {
               arr_structs_p->struct_arr_p = (void *) node_info_array_p; //CID 331
               cmi_dm_cleanup_array_of_node_info (arr_structs_p);
               *ppChildInfoArr = NULL;
               return OF_FAILURE;
            }
            if (ucmdm_CopyNodeInfointoArray (node_info_p,
                     node_info_array_p, uindex_i) != OF_SUCCESS)
            {
               arr_structs_p->struct_arr_p = (void *) node_info_array_p; //CID 331
               cmi_dm_cleanup_array_of_node_info (arr_structs_p);
               cmi_dm_free_node_info (node_info_p); //CID 330
               *ppChildInfoArr = NULL;
               return OF_FAILURE;
            }
            if (ordered_table_b == TRUE)
            {
               if (pChildNode->element_attrib.key_b == TRUE && bCpyKeyToRelNode == FALSE)
               {
                  uindex_i++;
                  if (of_strlen (node_info_p->name_p) < CM_DM_ELEMENT_RELATIVITY_KEY_LEN)
                  {
                     of_free (node_info_p->name_p);
                     node_info_p->name_p =
                        (char *) of_calloc (1, CM_DM_ELEMENT_RELATIVITY_KEY_LEN + 1);
                     of_strncpy (node_info_p->name_p, CM_DM_ELEMENT_RELATIVE_KEY_NAME,
                           CM_DM_ELEMENT_RELATIVITY_KEY_LEN);
                  }
                  node_info_p->element_attrib.key_b = FALSE;
                  node_info_p->element_attrib.mandatory_b = FALSE;
                  if (ucmdm_CopyNodeInfointoArray (node_info_p,
                           node_info_array_p,
                           uindex_i) != OF_SUCCESS)
                  {
                     arr_structs_p->struct_arr_p = (void *) node_info_array_p; //CID 331
                     cmi_dm_cleanup_array_of_node_info (arr_structs_p);
                     *ppChildInfoArr = NULL;
                     cmi_dm_free_node_info (node_info_p); //CID 330
                     return OF_FAILURE;
                  }
                  bCpyKeyToRelNode = TRUE;
               }
            }
            cmi_dm_free_node_info (node_info_p);
            uindex_i++;
         }
         if (ordered_table_b == TRUE)
         {
            node_info_p = cmi_dm_create_node_info_struct (&RelativityNode, NULL);
            if (ucmdm_CopyNodeInfointoArray (node_info_p,
                     node_info_array_p, uindex_i) != OF_SUCCESS)
            {
               arr_structs_p->struct_arr_p = (void *) node_info_array_p; //CID 331
               cmi_dm_cleanup_array_of_node_info (arr_structs_p);
               *ppChildInfoArr = NULL;
               cmi_dm_free_node_info (node_info_p); //CID 330
               return OF_FAILURE;
            }
            cmi_dm_free_node_info (node_info_p); //CID 330 
         }
         arr_structs_p->struct_arr_p = (void *) node_info_array_p;
      }
   }
   while (0);

   if ((return_value != OF_SUCCESS) && (arr_structs_p != NULL))
   {
      cmi_dm_cleanup_array_of_node_info (arr_structs_p);
      *ppChildInfoArr = NULL;
   }
   else
   {
      *ppChildInfoArr = arr_structs_p;
   }
   return return_value;
}

int32_t cmi_dm_create_key_child_info_array (struct cm_dm_data_element * parent_node_p,
      struct cm_array_of_structs ** ppChildInfoArr)
{
   struct cm_array_of_structs *arr_structs_p = NULL;
   UCMDllQ_t *pDll = NULL;
   UCMDllQNode_t *pNode = NULL;
   struct cm_dm_data_element *pChildNode = NULL;
   struct cm_dm_node_info *node_info_p = NULL;
   struct cm_dm_node_info *node_info_array_p = NULL;
   uint32_t uindex_i = 0;
   int32_t return_value = OF_SUCCESS;
   uint32_t  uiKeyCnt=0;


   if ((!parent_node_p) || (!ppChildInfoArr))
   {
      return OF_FAILURE;
   }

   do
   {
      arr_structs_p =
         (struct cm_array_of_structs *) of_calloc (1, sizeof (struct cm_array_of_structs));
      if (unlikely (arr_structs_p == NULL))
      {
         return_value = CM_DM_ERR_MEM_RESOURCES;
         break;
      }

      pDll = &(parent_node_p->child_list);
      CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
      {
         pChildNode =
            CM_DLLQ_LIST_MEMBER (pNode, struct cm_dm_data_element *, list_node);
         if(pChildNode->element_attrib.key_b == TRUE)
         {
            uiKeyCnt++;
         }
      }
      arr_structs_p->count_ui = uiKeyCnt;
      if (arr_structs_p->count_ui == 0)
      {
         break;
      }

      node_info_array_p = (struct cm_dm_node_info *) of_calloc (arr_structs_p->count_ui,
            sizeof (struct cm_dm_node_info));
      if (unlikely (node_info_array_p == NULL))
      {
         return_value = CM_DM_ERR_MEM_RESOURCES;
         break;
      }

      uindex_i = 0;
      pDll = &(parent_node_p->child_list);
      CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
      {
         pChildNode =
            CM_DLLQ_LIST_MEMBER (pNode, struct cm_dm_data_element *, list_node);
         if(pChildNode->element_attrib.key_b == TRUE)
         {
            node_info_p = cmi_dm_create_node_info_struct (pChildNode, NULL);
            if (node_info_p == NULL)
            {
               cmi_dm_cleanup_array_of_node_info (arr_structs_p);
               *ppChildInfoArr = NULL;                                                         of_free(node_info_array_p);//CID 333
               return OF_FAILURE;
            }
            if (ucmdm_CopyNodeInfointoArray (node_info_p,
                     node_info_array_p, uindex_i) != OF_SUCCESS)
            {
               cmi_dm_cleanup_array_of_node_info (arr_structs_p);
               *ppChildInfoArr = NULL;
               of_free(node_info_array_p);//CID 333
               cmi_dm_free_node_info (node_info_p); //CID 332
               return OF_FAILURE;
            }
            cmi_dm_free_node_info (node_info_p);
            uindex_i++;
         }
      }
      arr_structs_p->struct_arr_p = (void *) node_info_array_p;
   }
   while (0);

   if ((return_value != OF_SUCCESS) && (arr_structs_p != NULL))
   {
      cmi_dm_cleanup_array_of_node_info (arr_structs_p);
      *ppChildInfoArr = NULL;
   }
   else
   {
      *ppChildInfoArr = arr_structs_p;
   }
   return return_value;
}
/**************************************************************************
  Function Name : cmi_dm_cleanup_array_of_strings 
  Input Args    : arr_str_p - Pointer to Array of Strings
  Output Args   : None
Description   : This API Frees Array of Strings.
Return Values : None
 *************************************************************************/
void cmi_dm_cleanup_array_of_strings (struct cm_array_of_strings * arr_str_p)
{
   int32_t i;

   if (arr_str_p)
   {
      if (arr_str_p->string_arr_p)
      {
         for (i = 0; i < arr_str_p->count_ui; i++)
         {
            if (arr_str_p->string_arr_p[i])
               of_free (arr_str_p->string_arr_p[i]);
         }

         of_free (arr_str_p->string_arr_p);
      }
      of_free (arr_str_p);
   }
}

/**************************************************************************
  Function Name : cm_dm_initDataElementInternals 
  Input Args    : pNode - Pointer to Node Element structure
  : app_cbks_p - Pointer to Application Callbacks structure
  Output Args   : None
Description   : This API Initialises the given Data Element Node and registers
: application callbacks
Return Values : None
 *************************************************************************/
int32_t cm_dm_initDataElementInternals (struct cm_dm_data_element * pNode,
      struct cm_dm_app_callbacks * app_cbks_p)
{
   if (pNode)
   {
      CM_DLLQ_INIT_LIST (&(pNode->child_list));
      CM_DLLQ_INIT_NODE (&(pNode->list_node));
      if (app_cbks_p)
      {
         pNode->app_cbks_p =
            (struct cm_dm_app_callbacks *) of_calloc (1, sizeof (struct cm_dm_app_callbacks));
         if (pNode->app_cbks_p)
         {
            //of_memcpy (pNode->app_cbks_p, app_cbks_p,
            //     sizeof (struct cm_dm_app_callbacks));
            pNode->app_cbks_p->cm_start_xtn =
               app_cbks_p->cm_start_xtn;
            pNode->app_cbks_p->cm_add_rec =
               app_cbks_p->cm_add_rec;
            pNode->app_cbks_p->cm_modify_rec =
               app_cbks_p->cm_modify_rec;
            pNode->app_cbks_p->cm_delete_rec =
               app_cbks_p->cm_delete_rec;
            pNode->app_cbks_p->cm_end_xtn =
               app_cbks_p->cm_end_xtn;
            pNode->app_cbks_p->cm_getfirst_nrecs =
               app_cbks_p->cm_getfirst_nrecs;
            pNode->app_cbks_p->cm_getnext_nrecs =
               app_cbks_p->cm_getnext_nrecs;
            pNode->app_cbks_p->cm_getexact_rec =
               app_cbks_p->cm_getexact_rec;
            pNode->app_cbks_p->cm_test_config =
               app_cbks_p->cm_test_config;
         }
         else
         {
            return CM_DM_ERR_MEM_RESOURCES;
         }
      }
   }

   return OF_SUCCESS;
}

/**************************************************************************
  Function Name : cmi_dm_create_node_info_struct 
  Input Args    : pNode - pointer to DataElement node
  : dm_path_p - Pointer to Data Model path 
  Output Args   : NONE
Description   : This API will take DataElement node pointer and DMPath as input
: parameters.Creates the DMNodeInfo structure, copies all the
: required information into  DMNodeinfo structure from DataElement node.
Return Values : Pointer to DMNodeInfo structure
 *************************************************************************/
struct cm_dm_node_info *cmi_dm_create_node_info_struct (struct cm_dm_data_element * pNode,
      char * dm_path_p)
{
   struct cm_dm_node_info *node_info_p = NULL;
   int32_t iCnt =0;
   if (!pNode)
   {
      CM_DM_DEBUG_PRINT ("Invalid input");
      return NULL;
   }

   node_info_p = (struct cm_dm_node_info *) of_calloc (1, sizeof (struct cm_dm_node_info));
   if (unlikely (node_info_p == NULL))
   {
      return NULL;
   }

   node_info_p->name_p = (char *) of_calloc (1, of_strlen (pNode->name_p) + 1);
   if (unlikely (node_info_p->name_p == NULL))
   {
      of_free (node_info_p);
      return NULL;
   }

   if(pNode->friendly_name_p != NULL)
   {
      node_info_p->friendly_name_p = (char *) of_calloc (1, of_strlen(pNode->friendly_name_p) + 1);
      if (unlikely (node_info_p->friendly_name_p == NULL))
      {
         of_free (node_info_p->name_p);
         of_free (node_info_p);
         return NULL;
      }
      of_strcpy (node_info_p->friendly_name_p, pNode->friendly_name_p);
   }
   else
      node_info_p->friendly_name_p = NULL;

   node_info_p->description_p =
      (char *) of_calloc (1, of_strlen (pNode->description_p) + 1);
   if (unlikely (node_info_p->description_p == NULL))
   {
      if(node_info_p->friendly_name_p!=NULL)
         of_free (node_info_p->friendly_name_p);
      of_free (node_info_p->name_p);
      of_free (node_info_p);
      return NULL;
   }

   if (!dm_path_p)
   {
      dm_path_p = cmi_dm_create_name_path_from_node (pNode);
      if (unlikely (!dm_path_p))
      {
         of_free (node_info_p->description_p);
         if(node_info_p->friendly_name_p!=NULL)
            of_free (node_info_p->friendly_name_p);
         of_free (node_info_p->name_p);
         of_free (node_info_p);
         return NULL;
      }
      node_info_p->dm_path_p = dm_path_p;
   }
   else
   {
      node_info_p->dm_path_p = (char *) of_calloc (1, of_strlen (dm_path_p) + 1);
      if (!node_info_p->dm_path_p)
      {
         of_free (node_info_p->description_p);
         if(node_info_p->friendly_name_p!=NULL)
            of_free (node_info_p->friendly_name_p);
         of_free (node_info_p->name_p);
         of_free (node_info_p);
         return NULL;
      }
      of_strcpy (node_info_p->dm_path_p, dm_path_p);
   }

   of_strcpy (node_info_p->name_p, pNode->name_p);

   of_strcpy (node_info_p->description_p, pNode->description_p);
   node_info_p->id_ui = pNode->id_ui;
   if(pNode->save_file_name_p)
   {
      of_strcpy (node_info_p->file_name, pNode->save_file_name_p);
   }
   of_memcpy (&(node_info_p->element_attrib), &(pNode->element_attrib),
         sizeof (node_info_p->element_attrib));

   if(pNode->element_attrib.command_name_p)
   {
      node_info_p->element_attrib.command_name_p =
         (char *) of_calloc (1, of_strlen (pNode->element_attrib.command_name_p) + 1);
      if (unlikely (node_info_p->element_attrib.command_name_p == NULL))
      {
         of_free (node_info_p->name_p);
         if(node_info_p->friendly_name_p!=NULL)
            of_free (node_info_p->friendly_name_p);
         of_free (node_info_p->description_p);
         of_free (node_info_p);
         return NULL;
      }
      of_strcpy (node_info_p->element_attrib.command_name_p, pNode->element_attrib.command_name_p);
   }

   if(pNode->element_attrib.cli_identifier_p)
   {
      node_info_p->element_attrib.cli_identifier_p =
         (char *) of_calloc (1, of_strlen (pNode->element_attrib.cli_identifier_p) + 1);
      if (unlikely (node_info_p->element_attrib.cli_identifier_p == NULL))
      {
         of_free (node_info_p->element_attrib.command_name_p);
         of_free (node_info_p->name_p);
         if(node_info_p->friendly_name_p!=NULL)
            of_free (node_info_p->friendly_name_p);
         of_free (node_info_p->description_p);
         of_free (node_info_p);
         return NULL;
      }
      of_strcpy (node_info_p->element_attrib.cli_identifier_p, pNode->element_attrib.cli_identifier_p);
   }

   if(pNode->element_attrib.cli_idenetifier_help_p)
   {
      node_info_p->element_attrib.cli_idenetifier_help_p =
         (char *) of_calloc (1, of_strlen (pNode->element_attrib.cli_idenetifier_help_p) + 1);
      if (unlikely (node_info_p->element_attrib.cli_idenetifier_help_p == NULL))
      {
         of_free (node_info_p->element_attrib.command_name_p);
         of_free(node_info_p->element_attrib.cli_identifier_p);
         of_free (node_info_p->name_p);
         if(node_info_p->friendly_name_p!=NULL)
            of_free (node_info_p->friendly_name_p);
         of_free (node_info_p->description_p);
         of_free (node_info_p);
         return NULL;
      }
      of_strcpy (node_info_p->element_attrib.cli_idenetifier_help_p, pNode->element_attrib.cli_idenetifier_help_p);
   }
   of_memcpy (&(node_info_p->data_attrib), &(pNode->data_attrib),
         sizeof (node_info_p->data_attrib));

   if((pNode->data_attrib.data_type == CM_DATA_TYPE_STRING || 
            pNode->data_attrib.data_type == CM_DATA_TYPE_STRING_SPECIAL_CHARS) && 
         pNode->data_attrib.attrib_type == CM_DATA_ATTRIB_STR_ENUM)
   {
      node_info_p->data_attrib.attr.string_enum.count_ui= pNode->data_attrib.attr.string_enum.count_ui;
      for( iCnt =0 ;iCnt<pNode->data_attrib.attr.string_enum.count_ui;iCnt++)
      {
         if(strlen(pNode->data_attrib.attr.string_enum.array[iCnt]) > 0)
         {
            node_info_p->data_attrib.attr.string_enum.array[iCnt] =
               (char *) of_calloc (1, of_strlen ((char*)pNode->data_attrib.attr.string_enum.array[iCnt]) + 1);
            if (unlikely (node_info_p->data_attrib.attr.string_enum.array[iCnt] == NULL))
            {
               of_free (node_info_p->element_attrib.cli_idenetifier_help_p);
               of_free (node_info_p->element_attrib.command_name_p);
               of_free(node_info_p->element_attrib.cli_identifier_p);
               of_free (node_info_p->name_p);
               if(node_info_p->friendly_name_p!=NULL)
                  of_free (node_info_p->friendly_name_p);
               of_free (node_info_p->description_p);
               of_free (node_info_p);
               return NULL;
            }
            of_strcpy (node_info_p->data_attrib.attr.string_enum.array[iCnt], pNode->data_attrib.attr.string_enum.array[iCnt]);
         }
         else
            node_info_p->data_attrib.attr.string_enum.array[iCnt] = "";

      }
   }

   if((pNode->data_attrib.data_type == CM_DATA_TYPE_STRING || 
            pNode->data_attrib.data_type == CM_DATA_TYPE_STRING_SPECIAL_CHARS)
         && pNode->data_attrib.attrib_type == CM_DATA_ATTRIB_STR_ENUM)
   {
      node_info_p->data_attrib.attr.string_enum.count_ui= pNode->data_attrib.attr.string_enum.count_ui;
      for( iCnt =0 ;iCnt<pNode->data_attrib.attr.string_enum.count_ui;iCnt++)
      {
         if(strlen(pNode->data_attrib.attr.string_enum.aFrdArr[iCnt]) > 0)
         {
            node_info_p->data_attrib.attr.string_enum.aFrdArr[iCnt] =
               (char *) of_calloc (1, of_strlen ((char*)pNode->data_attrib.attr.string_enum.aFrdArr[iCnt]) + 1);
            if (unlikely (node_info_p->data_attrib.attr.string_enum.aFrdArr[iCnt] == NULL))
            {
               of_free(node_info_p->data_attrib.attr.string_enum.array[iCnt]);
               of_free (node_info_p->element_attrib.cli_idenetifier_help_p);
               of_free (node_info_p->element_attrib.command_name_p);
               of_free(node_info_p->element_attrib.cli_identifier_p);
               of_free (node_info_p->name_p);
               if(node_info_p->friendly_name_p!=NULL)
                  of_free (node_info_p->friendly_name_p);
               of_free (node_info_p->description_p);
               of_free (node_info_p);
               return NULL;
            }
            of_strcpy (node_info_p->data_attrib.attr.string_enum.aFrdArr[iCnt], pNode->data_attrib.attr.string_enum.aFrdArr[iCnt]);
         }
         else
            node_info_p->data_attrib.attr.string_enum.aFrdArr[iCnt] = "";

      }
   }
   return node_info_p;
}

/**************************************************************************
  Function Name : cmi_dm_get_id_path_from_name_path 
  Input Args    : name_path_p - Pointer to data_model Name path
  Output Args   : pIDArray - Pointer to Array of integers. Used to hold the 
  : DM node IDs.
Description   : This API will extract all the DM node IDs that are encountered
: across the given name path.
Return Values : OF_SUCCESS or OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_get_id_path_from_name_path (char * name_path_p,
      struct cm_array_of_uints * pIDArray)
{
   uint32_t i;
   char *pToken = NULL;
   uint32_t uiMaxTokenLen;
   uint32_t uindex_i = 0;
   uint32_t uiPathCount = 0;
   uint32_t uiNamePathLen;
   struct cm_dm_data_element *pNode = NULL;
   uint32_t return_value = UCMDM_PATH_PARSING_REMAINING;

   if ((!name_path_p) || (of_strlen (name_path_p) == 0) || (!pIDArray))
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

   uiNamePathLen = uiMaxTokenLen - 1;
   uiPathCount = 0;

   for (i = 0; i < uiNamePathLen; i++)
   {
      if (name_path_p[i] == '.')
      {
         uiPathCount++;
      }
   }

   pIDArray->uint_arr_p =
      (uint32_t *) of_calloc (1, sizeof (uint32_t) * uiPathCount);
   if (pIDArray->uint_arr_p == NULL)
   {
      of_free (pToken);
      return OF_FAILURE;
   }

   /* Tokenize the input string */
   pNode = pDMTree;
   while (return_value == UCMDM_PATH_PARSING_REMAINING)
   {
      return_value = cmi_dm_get_path_token (name_path_p, pToken, uiMaxTokenLen, &uindex_i);

      /* Find the matching child */
      pNode = cmi_dm_get_child_by_name (pNode, pToken);
      if (!pNode)
      {
         break;
      }

      /* Store the ID in IDPath */
      pIDArray->uint_arr_p[uiPathCount++] = pNode->id_ui;

      if (return_value == UCMDM_PATH_PARSING_COMPLETED)
      {
         break;
      }
   }

   of_free (pToken);
   pIDArray->count_ui = uiPathCount;
   return OF_SUCCESS;
}

/**************************************************************************
  Function Name : cmi_dm_create_name_path_from_node 
  Input Args    : pNode - Pointer to DM DataElement Node
  Output Args   : NONE
Description   : Given a DM Node, this API will frame the DM path(from root node 
: to given node). 
Return Values : DM Path / NULL
 *************************************************************************/
char *cmi_dm_create_name_path_from_node (struct cm_dm_data_element * pNode)
{
   uint32_t uiPathLen = 0;
   uint32_t name_length;
   char *dm_path_p = NULL;
   struct cm_dm_data_element *pTmpNode = pNode;

   while (pTmpNode)
   {
      uiPathLen += of_strlen (pTmpNode->name_p) + 1;        /* 1 for dot */
      pTmpNode = pTmpNode->parent_p;
   }

   dm_path_p = (char *) of_calloc (1, uiPathLen + 1);    /* 1 for \0 at end */
   if (dm_path_p)
   {
      pTmpNode = pNode;
      while (pTmpNode)
      {
         name_length = of_strlen (pTmpNode->name_p) + 1;       /* 1 for dot */
         of_strcpy (&(dm_path_p[uiPathLen - name_length]), pTmpNode->name_p);
         if (pTmpNode != pNode)
         {
            dm_path_p[uiPathLen - 1] = '.';
         }
         uiPathLen -= name_length;
         pTmpNode = pTmpNode->parent_p;
      }
   }

   return dm_path_p;
}

/**************************************************************************
  Function Name : cmi_dm_create_id_path_from_node 
  Input Args    : pNode - Pointer to DM DataElement node
  Output Args   : pIDArray - Pointer to Array of Integers. Stores the extracted
  : nodes IDs
Description   : Given DM DataElement node pointer, this API will extract and
: frame the Node ID path(IDs of Root node to the given node).
Return Values : OF_SUCCESS / OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_create_id_path_from_node (struct cm_dm_data_element * pNode,
      struct cm_array_of_uints * pIDArray)
{
   uint32_t uiPathCount;
   uint32_t tmp_count_ui;
   struct cm_dm_data_element *pTmpNode = NULL;

   if ((!pNode) || (!pIDArray))
   {
      return OF_FAILURE;
   }

   /* Allocate memory for output */
   pTmpNode = pNode;
   uiPathCount = 0;
   while (pTmpNode)
   {
      uiPathCount++;
      pTmpNode = pTmpNode->parent_p;
   }

   pIDArray->uint_arr_p =
      (uint32_t *) of_calloc (1, sizeof (uint32_t) * uiPathCount);
   if (pIDArray->uint_arr_p == NULL)
   {
      return OF_FAILURE;
   }

   /* Just traverse back on ancestors */
   pTmpNode = pNode;
   tmp_count_ui = uiPathCount;
   while (pTmpNode)
   {
      pIDArray->uint_arr_p[--tmp_count_ui] = pTmpNode->id_ui;
      pTmpNode = pTmpNode->parent_p;
   }

   pIDArray->count_ui = uiPathCount;

#if 0
   /* Incase, if we choose to traverse only once - bottom to top,
      we can reverse the ID path 
      */
   cmi_dm_reverse_uint_array (pIDArray);
#endif

   return OF_SUCCESS;
}

/**************************************************************************
  Function Name : cmi_dm_reverse_uint_array 
  Input/Output  : pIDArray - Pointer to array of integers
Description   : This API will take ID Array as input and reverses the contents 
: of the array.
Return Values : NONE
 *************************************************************************/
void cmi_dm_reverse_uint_array (struct cm_array_of_uints * pIDArray)
{
   uint32_t i;
   uint32_t uiTemp;
   uint32_t count_ui = pIDArray->count_ui;
   uint32_t loop_count_ui = count_ui / 2;
   for (i = 0; i < loop_count_ui; i++)
   {
      uiTemp = pIDArray->uint_arr_p[i];
      pIDArray->uint_arr_p[i] = pIDArray->uint_arr_p[count_ui - i - 1];
      pIDArray->uint_arr_p[count_ui - i - 1] = uiTemp;
   }
}

/**************************************************************************
  Function Name : cmi_dm_reverse_uint_array 
  Input/Output  : pIDArray - Pointer to array of integers
Description   : This API will take ID Array as input and reverses the contents 
: of the array.
Return Values : NONE
 *************************************************************************/
int32_t cmi_dm_get_key_array_from_namepath (char * name_path_p,
      struct cm_array_of_nv_pair ** keys_array_pp)
{
   int32_t iTokenRet = UCMDM_PATH_PARSING_REMAINING, ii, return_value;
   char *tm_name_path_p, *pToken, *key_p = NULL;
   uint32_t count_ui, uiNvPairIndex;
   struct cm_array_of_nv_pair *pnv_pair_array;
   struct cm_dm_data_element *pNode;
   unsigned char bPrevTokenIsTable = FALSE;
   uint32_t uiTokenLen, uindex_i = 0;

   if (!name_path_p)
   {
      CM_DM_DEBUG_PRINT ("Invalid input");
      return OF_FAILURE;
   }

   count_ui = 0;
   ii = 0;

   tm_name_path_p = name_path_p;
#define CM_DM_DMPATH_INSTANCE_BEGIN_DELIMITER '{'
#define CM_DM_DMPATH_INSTANCE_END_DELIMITER '}'
#define CM_DM_DMPATH_INSTANCE_SEPERATOR ','


   /* Count the number of Keys */
   while (tm_name_path_p[ii] != '\0')
   {
      if (tm_name_path_p[ii++] == '{')
      {
         count_ui++;
         while (tm_name_path_p[ii++] != '}')
         {
            if(tm_name_path_p[ii] == ',')
            {
               count_ui++;
            }
         }
      }
   }

   /* Point to the DMTree root */
   pNode = pDMTree;
   if (!pNode)
   {
      CM_DM_DEBUG_PRINT ("DMTree doesn't exits");
      return OF_FAILURE;
   }

   /* Allocate memory for NvPair Array */
   pnv_pair_array = (struct cm_array_of_nv_pair *) of_calloc (1,
         sizeof
         (struct cm_array_of_nv_pair));
   if (pnv_pair_array == NULL)
   {
      CM_DM_DEBUG_PRINT ("Memory allocation failed");
      return OF_FAILURE;
   }

   pnv_pair_array->count_ui = count_ui;

   /* If no Keys found, return failure */
   if (count_ui == 0)
   {
      CM_DM_DEBUG_PRINT ("No keys found in given NamePath");
      *keys_array_pp = pnv_pair_array;
      return OF_SUCCESS;
   }

   /* Get DMPath length */
   uiTokenLen = of_strlen (tm_name_path_p);

   pToken = (char *) of_calloc (1, uiTokenLen + 1);
   if (!pToken)
   {
      CM_DM_DEBUG_PRINT ("Memory allocation failed");
      of_free (pnv_pair_array);
      return OF_FAILURE;
   }



   /* Allocate memory for 'count_ui' number of nv_pairs */
   pnv_pair_array->nv_pairs =
      (struct cm_nv_pair *) of_calloc (count_ui, sizeof (struct cm_nv_pair));
   if (!pnv_pair_array->nv_pairs)
   {
      CM_DM_DEBUG_PRINT ("Memory allocation failed");
      of_free (pToken);
      of_free (pnv_pair_array);
      return OF_FAILURE;
   }

   uiNvPairIndex = 0;

   /* Parse the DMPath string and get the Keys information */
   while (iTokenRet == UCMDM_PATH_PARSING_REMAINING)
   {
      /* Get token */
      iTokenRet = cmi_dm_get_path_token (tm_name_path_p, pToken, uiTokenLen, &uindex_i);

      if (pToken[0] == '{')
      {
         if (bPrevTokenIsTable == TRUE)
         { 
            if((return_value=cmi_dm_get_key_array_from_namepathToken(pToken, pNode, pnv_pair_array, &uiNvPairIndex)) !=
                  OF_SUCCESS)
            {
               CM_DM_DEBUG_PRINT("Token parsing failed");
               of_free (pToken);
               of_free (pnv_pair_array);
               return return_value;
            }
            bPrevTokenIsTable=FALSE;
         }
      }
      else
      {
         /* Find the matching child */

         /*first token is root node name */
         if (!of_strcmp (pNode->name_p, pToken))
         {
            if (pDMTree != pNode)
            {
               pNode = NULL;;
               break;
            }
         }
         else
         {
            pNode = cmi_dm_get_child_by_name (pNode, pToken);
            if (!pNode)
            {
               break;
            }

            if (pNode->element_attrib.element_type == CM_DMNODE_TYPE_TABLE)
            {
               bPrevTokenIsTable = TRUE;
            }
            else
            {
               bPrevTokenIsTable = FALSE;
            }
         }
      }
   }

   *keys_array_pp = pnv_pair_array;
   of_free (pToken);
   of_free (key_p);
   return OF_SUCCESS;
}

int32_t cmi_dm_get_instance_keys_from_name_path (char * name_path_p,
      struct cm_array_of_nv_pair ** keys_array_pp)
{
   int32_t return_value = UCMDM_PATH_PARSING_REMAINING, ii;
   char *tm_name_path_p,*pTmpToken, *pToken, *key_p = NULL;
   uint32_t count_ui, uiNvPairIndex;
   struct cm_array_of_nv_pair *pnv_pair_array;
   struct cm_dm_data_element *pNode, *pTmpNode=NULL;
   unsigned char bPrevTokenIsTable = FALSE;
   uint32_t uiTokenLen, uindex_i = 0;

   if (!name_path_p)
   {
      CM_DM_DEBUG_PRINT ("Invalid input");
      return OF_FAILURE;
   }

   tm_name_path_p = name_path_p;

   /* Point to the DMTree root */
   pNode = pDMTree;
   if (!pNode)
   {
      CM_DM_DEBUG_PRINT ("DMTree doesn't exits");
      return OF_FAILURE;
   }

   /* Allocate memory for NvPair Array */
   pnv_pair_array = (struct cm_array_of_nv_pair *) of_calloc (1,
         sizeof
         (struct cm_array_of_nv_pair));
   if (pnv_pair_array == NULL)
   {
      CM_DM_DEBUG_PRINT ("Memory allocation failed");
      return OF_FAILURE;
   }

   /* Get DMPath length */
   uiTokenLen = of_strlen (tm_name_path_p);

   pToken = (char *) of_calloc (1, uiTokenLen + 1);
   if (!pToken)
   {
      CM_DM_DEBUG_PRINT ("Memory allocation failed");
      of_free (pnv_pair_array);
      return OF_FAILURE;
   }

   pTmpToken = (char *) of_calloc (1, uiTokenLen + 1);
   if (!pTmpToken)
   {
      CM_DM_DEBUG_PRINT ("Memory allocation failed");
      of_free (pnv_pair_array);
      of_free(pToken);
      return OF_FAILURE;
   }

   /* Parse the DMPath string and get the Keys information */
   while (return_value == UCMDM_PATH_PARSING_REMAINING)
   {
      /* Get token */
      return_value = cmi_dm_get_path_token (tm_name_path_p, pToken, uiTokenLen, &uindex_i);

      if (pToken[0] == '{')
      {
         if (bPrevTokenIsTable == TRUE)
         { 
            pTmpNode=pNode;
            of_strncpy(pTmpToken, pToken, uiTokenLen);
            bPrevTokenIsTable=FALSE;
         }
      }
      else
      {
         /*first token is root node name */
         if (!of_strcmp (pNode->name_p, pToken))
         {
            if (pDMTree != pNode)
            {
               pNode = NULL;;
               break;
            }
         }
         else
         {
            pNode = cmi_dm_get_child_by_name (pNode, pToken);
            if (!pNode)
            {
               break;
            }

            if (pNode->element_attrib.element_type == CM_DMNODE_TYPE_TABLE)
            {
               bPrevTokenIsTable = TRUE;
            }
            else
            {
               bPrevTokenIsTable = FALSE;
            }
         }
      }
      if (return_value == UCMDM_PATH_PARSING_COMPLETED)
      {
         break;
      }
   }

   if((pTmpToken && pTmpNode) && (pNode == pTmpNode)) 
   {
      /* Count the number of Keys */
      ii=0;
      count_ui=0;
      while (pTmpToken[ii] != '\0')
      {
         if (pTmpToken[ii++] == '{')
         {
            count_ui++;
            while (pTmpToken[ii++] != '}')
            {
               if(pTmpToken[ii] == ',')
               {
                  count_ui++;
               }
            }
         }
      }

      pnv_pair_array->count_ui=count_ui;
      /* Allocate memory for 'count_ui' number of nv_pairs */
      pnv_pair_array->nv_pairs =
         (struct cm_nv_pair *) of_calloc (count_ui, sizeof (struct cm_nv_pair));
      if (!pnv_pair_array->nv_pairs)
      {
         CM_DM_DEBUG_PRINT ("Memory allocation failed");
         of_free (pToken);
         of_free (pnv_pair_array);
         of_free (pTmpToken);
         return OF_FAILURE;
      }
      uiNvPairIndex=0;
      if(return_value=cmi_dm_get_key_array_from_namepathToken(pTmpToken, pTmpNode, pnv_pair_array, &uiNvPairIndex) !=
            OF_SUCCESS)
      {
         CM_DM_DEBUG_PRINT("Token parsing failed");
         of_free (pToken);
         of_free (pTmpToken);
         of_free (pnv_pair_array);
         return OF_FAILURE;
      }
   }
   else
   {
      CM_DM_DEBUG_PRINT("Token parsing failed");
      of_free (pToken);
      of_free (pTmpToken);
      of_free (pnv_pair_array);
      return OF_FAILURE;
   }
   *keys_array_pp = pnv_pair_array;
   of_free (pToken);
   of_free (pTmpToken);
   of_free (key_p);
   return OF_SUCCESS;
}

int32_t cmi_dm_get_keys_iv_array_from_name_path (char * name_path_p,
      struct cm_array_of_iv_pairs ** keys_array_pp)
{
   int32_t return_value = UCMDM_PATH_PARSING_REMAINING, ii, jj;
   char *tm_name_path_p, *pToken, *key_p = NULL;
   uint16_t count_ui, uiIvPairIndex;
   struct cm_array_of_iv_pairs *pIvPairArray;
   struct cm_dm_data_element *pNode, *pChildNode;
   UCMDllQNode_t *pDllQNode;
   unsigned char bPrevTokenIsTable = FALSE;
   uint32_t uiTokenLen, uindex_i = 0;

   if (!name_path_p)
   {
      CM_DM_DEBUG_PRINT ("Invalid input");
      return OF_FAILURE;
   }

   count_ui = 0;
   ii = 0;

   tm_name_path_p = name_path_p;
   /*  tm_name_path_p = aPath;  */

   /* Count the number of Keys */
   while (tm_name_path_p[ii] != '\0')
   {
      if (tm_name_path_p[ii++] == '{')
         count_ui++;
   }

   /* Point to the DMTree root */
   pNode = pDMTree;
   if (!pNode)
   {
      CM_DM_DEBUG_PRINT ("DMTree doesn't exits");
      return OF_FAILURE;
   }

   /* Allocate memory for IvPair Array */
   pIvPairArray = (struct cm_array_of_iv_pairs *) of_calloc (1,
         sizeof
         (struct cm_array_of_iv_pairs));
   if (pIvPairArray == NULL)
   {
      CM_DM_DEBUG_PRINT ("Memory allocation failed");
      return OF_FAILURE;
   }

   pIvPairArray->count_ui = count_ui;

   /* If no Keys found, return failure */
   if (count_ui == 0)
   {
      CM_DM_DEBUG_PRINT ("No keys found in given NamePath");
      *keys_array_pp = pIvPairArray;
      return OF_SUCCESS;
   }

   /* Get DMPath length */
   uiTokenLen = of_strlen (tm_name_path_p);

   pToken = (char *) of_calloc (1, uiTokenLen + 1);
   if (!pToken)
   {
      CM_DM_DEBUG_PRINT ("Memory allocation failed");
      of_free (pIvPairArray);
      return OF_FAILURE;
   }

   key_p = (char *) of_calloc (1, uiTokenLen + 1);
   if (!key_p)
   {
      CM_DM_DEBUG_PRINT ("Memory allocation failed");
      of_free (pIvPairArray);
      of_free (pToken);
      return OF_FAILURE;
   }
   /* Allocate memory for 'count_ui' number of IvPairs */
   pIvPairArray->iv_pairs =
      (struct cm_iv_pair *) of_calloc (count_ui, sizeof (struct cm_iv_pair));
   if (!pIvPairArray->iv_pairs)
   {
      CM_DM_DEBUG_PRINT ("Memory allocation failed");
      of_free (pToken);
      of_free (pIvPairArray);
      of_free (key_p);
      return OF_FAILURE;
   }

   uiIvPairIndex = 0;

   /* Parse the DMPath string and get the Keys information */
   while (return_value == UCMDM_PATH_PARSING_REMAINING)
   {
      /* Get token */
      return_value = cmi_dm_get_path_token (tm_name_path_p, pToken, uiTokenLen, &uindex_i);

      if (pToken[0] == '{')
      {
         if (bPrevTokenIsTable == TRUE)
         {
            of_memset (key_p, 0, uiTokenLen);
            for (ii = 1, jj = 0; pToken[ii] != '}'; ii++, jj++)
            {
               key_p[jj] = pToken[ii];
            }
            key_p[jj] = '\0';

            if (pNode->element_attrib.element_type == CM_DMNODE_TYPE_TABLE)
            {
               /* Scan through the Child List to get the Key Element */
               CM_DLLQ_SCAN (&pNode->child_list, pDllQNode, UCMDllQNode_t *)
               {
                  pChildNode = CM_DLLQ_LIST_MEMBER (pDllQNode,
                        struct cm_dm_data_element *, list_node);
                  if (pChildNode->element_attrib.key_b == TRUE)
                  {
                     /* Fill the Name-Value pair */
                     pIvPairArray->iv_pairs[uiIvPairIndex].id_ui = pChildNode->id_ui;
                     pIvPairArray->iv_pairs[uiIvPairIndex].value_type =
                        pChildNode->data_attrib.data_type;
                     pIvPairArray->iv_pairs[uiIvPairIndex].value_length = of_strlen (key_p);
                     pIvPairArray->iv_pairs[uiIvPairIndex].value_p =
                        (char *) of_calloc (1,
                              pIvPairArray->iv_pairs[uiIvPairIndex].
                              value_length + 1);
                     if (pIvPairArray->iv_pairs[uiIvPairIndex].value_p)
                     {
                        of_strcpy (pIvPairArray->iv_pairs[uiIvPairIndex].value_p, key_p);
                     }

                     uiIvPairIndex++;
                     bPrevTokenIsTable = FALSE;
                     break;
                  }
               }
            }
         }
      }
      else
      {
         /* Find the matching child */
         /*first token is root node name */
         if (!of_strcmp (pNode->name_p, pToken))
         {
            if (pDMTree != pNode)
            {
               pNode = NULL;;
               break;
            }
         }
         else
         {
            pNode = cmi_dm_get_child_by_name (pNode, pToken);
            if (!pNode)
            {
               break;
            }

            if (pNode->element_attrib.element_type == CM_DMNODE_TYPE_TABLE)
            {
               bPrevTokenIsTable = TRUE;
            }
            else
            {
               bPrevTokenIsTable = FALSE;
            }
         }
      }
      if (return_value == UCMDM_PATH_PARSING_COMPLETED)
      {
         break;
      }
   }

   *keys_array_pp = pIvPairArray;
   of_free (pToken);
   of_free (key_p);

   return OF_SUCCESS;
}

/**************************************************************************
  Debug Routines
 *************************************************************************/
/**************************************************************************
  Function Name : cm_dm_print_tree 
  Input Args    : data_element_p - Pointer to DataElement structure
  Output Args   : NONE
Description   : This API starts traversing from the given DataElement node
: and prints the node names.
Return Values : OF_SUCCESS / OF_FAILURE
 *************************************************************************/
int32_t cm_dm_print_tree (struct cm_dm_data_element * data_element_p)
{
   UCMDllQNode_t *pNode;
   UCMDllQ_t *pDll;
   struct cm_dm_data_element *pChildNode = NULL;

   if (data_element_p)
   {
      pDll = &(data_element_p->child_list);

      if (of_strcmp (data_element_p->name_p, "/"))
      {
         CM_DM_DEBUG_PRINT (",%s", data_element_p->name_p);
      }
      else
      {
         CM_DM_DEBUG_PRINT ("%s", data_element_p->name_p);
      }

      CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
      {
         pChildNode =
            CM_DLLQ_LIST_MEMBER (pNode, struct cm_dm_data_element *, list_node);
         if (pChildNode)
         {
            cm_dm_print_tree (pChildNode);
         }
      }
   }

   return OF_SUCCESS;
}

/**************************************************************************
  Function Name : cmi_dm_print_node_info 
  Input Args    : node_info_p - Pointer to DMNodeInfo structure
  Output Args   : NONE
Description   : This API prints all the elements of the given DMNodeinfo structure.
Return Values : OF_SUCCESS / OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_print_node_info (struct cm_dm_node_info * node_info_p)
{
   int32_t i;

   if (unlikely (node_info_p == NULL))
   {
      CM_DM_DEBUG_PRINT ("PrintNodeInfo : NULL input");
      return OF_FAILURE;
   }

   CM_DM_DEBUG_PRINT ("Name : %s   <--- ID  : %d ", node_info_p->name_p, node_info_p->id_ui);
   switch (node_info_p->element_attrib.element_type)
   {
      case CM_DMNODE_TYPE_ANCHOR:
         CM_DM_DEBUG_PRINT ("ElementType: Anchor");
         break;
      case CM_DMNODE_TYPE_TABLE:
         CM_DM_DEBUG_PRINT ("ElementType: Table");
         break;
      case CM_DMNODE_TYPE_SCALAR_VAR:
         CM_DM_DEBUG_PRINT ("ElementType: Variable");
         break;
      case CM_DMNODE_TYPE_INVALID:
      default:
         CM_DM_DEBUG_PRINT ("ElementType: Invalid. %d",
               node_info_p->element_attrib.element_type);
         break;
   }

   CM_DM_DEBUG_PRINT ("element_attrib.visible_b       : %d",
         node_info_p->element_attrib.visible_b);
   CM_DM_DEBUG_PRINT ("element_attrib.key_b           : %d",
         node_info_p->element_attrib.key_b);
   CM_DM_DEBUG_PRINT ("element_attrib.mandatory_b     : %d",
         node_info_p->element_attrib.mandatory_b);
   CM_DM_DEBUG_PRINT ("element_attrib.notify_change_b  : %d",
         node_info_p->element_attrib.notify_change_b);
   CM_DM_DEBUG_PRINT ("element_attrib.sticky_to_parent_b: %d",
         node_info_p->element_attrib.sticky_to_parent_b);

   switch (node_info_p->data_attrib.data_type)
   {
      case CM_DATA_TYPE_UNKNOWN:
         CM_DM_DEBUG_PRINT ("ValType : NONE");
         break;
      case CM_DATA_TYPE_INT:
         CM_DM_DEBUG_PRINT ("ValType : Integer");
         switch (node_info_p->data_attrib.attrib_type)
         {
            case CM_DATA_ATTRIB_NONE:
               CM_DM_DEBUG_PRINT ("Data Attrib Type : None");
               break;
            case CM_DATA_ATTRIB_INT_RANGE:
               CM_DM_DEBUG_PRINT ("Data Attrib Type : IntegerRange");
               CM_DM_DEBUG_PRINT ("ValSemantics         : %d - %d",
                     node_info_p->data_attrib.attr.int_range.start_value,
                     node_info_p->data_attrib.attr.int_range.end_value);
               break;
            case CM_DATA_ATTRIB_INT_ENUM:
               CM_DM_DEBUG_PRINT ("Data Attrib Type : Enumarated Integers");
               CM_DM_DEBUG_PRINT ("ValSemantics         : ");
               for (i = 0; i < node_info_p->data_attrib.attr.int_enum.count_ui; i++)
                  CM_DM_DEBUG_PRINT ("%d,",
                        node_info_p->data_attrib.attr.int_enum.array[i]);
               break;
            default:
               CM_DM_DEBUG_PRINT ("Data Attrib Type: Invalid %d",
                     node_info_p->data_attrib.attrib_type);
               break;
         }
         break;
      case CM_DATA_TYPE_UINT:
         CM_DM_DEBUG_PRINT ("ValType : Unsigned Integer");
         switch (node_info_p->data_attrib.attrib_type)
         {
            case CM_DATA_ATTRIB_NONE:
               CM_DM_DEBUG_PRINT ("Data Attrib Type : None");
               break;
            case CM_DATA_ATTRIB_UINT_RANGE:
               CM_DM_DEBUG_PRINT ("Data Attrib Type : Unsigned Integer Range");
               CM_DM_DEBUG_PRINT ("ValSemantics         : %u - %u",
                     node_info_p->data_attrib.attr.uint_range.start_value,
                     node_info_p->data_attrib.attr.uint_range.end_value);
               break;
            case CM_DATA_ATTRIB_UINT_ENUM:
               CM_DM_DEBUG_PRINT ("Data Attrib Type : Enumarate Unsigned Integers");
               CM_DM_DEBUG_PRINT ("ValSemantics         : ");
               for (i = 0; i < node_info_p->data_attrib.attr.uint_enum.count_ui; i++)
                  CM_DM_DEBUG_PRINT ("%u,",
                        node_info_p->data_attrib.attr.uint_enum.
                        array[i]);
               break;
            default:
               CM_DM_DEBUG_PRINT ("Data Attrib Type Invalid %d",
                     node_info_p->data_attrib.attrib_type);
               break;
         }
         break;
      case CM_DATA_TYPE_INT64:
         CM_DM_DEBUG_PRINT ("ValType : Long Integer");
         switch (node_info_p->data_attrib.attrib_type)
         {
            case CM_DATA_ATTRIB_NONE:
               CM_DM_DEBUG_PRINT ("Data Attrib Type : None");
               break;
            case CM_DATA_ATTRIB_INT64_RANGE:
               CM_DM_DEBUG_PRINT ("Data Attrib Type : Long IntegerRange");
               CM_DM_DEBUG_PRINT ("ValSemantics         : %d - %d",
                     node_info_p->data_attrib.attr.longint_range.start_value,
                     node_info_p->data_attrib.attr.longint_range.end_value);
               break;
            case CM_DATA_ATTRIB_INT64_ENUM:
               CM_DM_DEBUG_PRINT ("Data Attrib Type : Enumarated Long Integers");
               CM_DM_DEBUG_PRINT ("ValSemantics         : ");
               for (i = 0; i < node_info_p->data_attrib.attr.longint_enum.count_ui; i++)
                  CM_DM_DEBUG_PRINT ("%ld,",
                        node_info_p->data_attrib.attr.longint_enum.array[i]);
               break;
            default:
               CM_DM_DEBUG_PRINT ("Data Attrib Type: Invalid %d",
                     node_info_p->data_attrib.attrib_type);
               break;
         }
         break;
      case CM_DATA_TYPE_UINT64:
         CM_DM_DEBUG_PRINT ("ValType : Unsigned Long Integer");
         switch (node_info_p->data_attrib.attrib_type)
         {
            case CM_DATA_ATTRIB_NONE:
               CM_DM_DEBUG_PRINT ("Data Attrib Type : None");
               break;
            case CM_DATA_ATTRIB_UINT64_RANGE:
               CM_DM_DEBUG_PRINT ("Data Attrib Type : Unsigned Long Integer Range");
               CM_DM_DEBUG_PRINT ("ValSemantics         : %u - %u",
                     node_info_p->data_attrib.attr.ulongint_range.start_value,
                     node_info_p->data_attrib.attr.ulongint_range.end_value);
               break;
            case CM_DATA_ATTRIB_UINT64_ENUM:
               CM_DM_DEBUG_PRINT ("Data Attrib Type : Enumarate Unsigned Integers");
               CM_DM_DEBUG_PRINT ("ValSemantics         : ");
               for (i = 0; i < node_info_p->data_attrib.attr.ulongint_enum.count_ui; i++)
                  CM_DM_DEBUG_PRINT ("%lu,",
                        node_info_p->data_attrib.attr.ulongint_enum.array[i]);
               break;
            default:
               CM_DM_DEBUG_PRINT ("Data Attrib Type Invalid %d",
                     node_info_p->data_attrib.attrib_type);
               break;
         }
         break;

      case CM_DATA_TYPE_STRING:
      case CM_DATA_TYPE_STRING_SPECIAL_CHARS:
         CM_DM_DEBUG_PRINT ("ValType : String");
         switch (node_info_p->data_attrib.attrib_type)
         {
            case CM_DATA_ATTRIB_NONE:
               CM_DM_DEBUG_PRINT ("Data Attrib Type : None");
               break;
            case CM_DATA_ATTRIB_STR_ENUM:
               CM_DM_DEBUG_PRINT ("Data Attrib Type : Enumarate Strings");
               CM_DM_DEBUG_PRINT ("ValSemantics         : ");
               for (i = 0; i < node_info_p->data_attrib.attr.string_enum.count_ui; i++)
                  CM_DM_DEBUG_PRINT ("string enum =%s",
                        node_info_p->data_attrib.attr.string_enum.
                        array[i]);
               break;
            default:
               CM_DM_DEBUG_PRINT ("Data Attrib Type Invalid %d",
                     node_info_p->data_attrib.attrib_type);
               break;
         }
         break;
      case CM_DATA_TYPE_BOOLEAN:
         CM_DM_DEBUG_PRINT ("ValType : Boolean");
         break;
      case CM_DATA_TYPE_DATETIME:
         CM_DM_DEBUG_PRINT ("ValType : Date&Time");
         break;
      case CM_DATA_TYPE_BASE64:
         CM_DM_DEBUG_PRINT ("ValType : Base64");
         break;
      default:
         CM_DM_DEBUG_PRINT ("ValType : Unknown ");
         break;
   }

   return OF_SUCCESS;
}

/**************************************************************************
  Function Name : cmi_dm_test_compare_node_info 
  Input Args    : node_info_p - Pointer to DMNodeInfo structure
  : pNode -pointer to DataElement structure
  Output Args   : NONE
Description   : This API compares the element values present in NodeInfo structure 
: with the element values present in DMNodeInfo structure.
Return Values : OF_SUCCESS / OF_FAILURE
 *************************************************************************/
int32_t cmi_dm_test_compare_node_info (struct cm_dm_node_info * node_info_p,
      struct cm_dm_data_element * pNode)
{
   return OF_SUCCESS;
}

unsigned char cmi_dm_is_child_element (char * child_path_p, char * parent_path_p)
{
   char *pChildToken = NULL;
   uint32_t uiChildTokenLen;
   uint32_t uiChildIndex = 0;
   uint32_t return_value1 = UCMDM_PATH_PARSING_REMAINING;
   char *tmp_child_path_p = NULL;

   char *parent_token_p = NULL;
   uint32_t uiParentTokenLen;
   uint32_t uiParentIndex = 0;
   uint32_t return_value2 = UCMDM_PATH_PARSING_REMAINING;
   char *pTmparent_path_p = NULL;

   if ((!parent_path_p) || (!child_path_p))
   {
      return FALSE;
   }

   uiChildTokenLen = of_strlen (child_path_p) + 1;
   if (uiChildTokenLen == 1)
   {
      return FALSE;
   }
   /* As we distroy path while tokenizing, let us operate on a duplicate
      path entry. So that input will not be modified, and caller can
      use it later.
      */
   tmp_child_path_p = (char *) of_calloc (1, uiChildTokenLen);
   if (!tmp_child_path_p)
   {
      return FALSE;
   }
   /* Allocate token buffer */
   pChildToken = (char *) of_calloc (1, uiChildTokenLen);
   if (!pChildToken)
   {
      of_free (tmp_child_path_p);
      return FALSE;
   }
   of_strncpy (tmp_child_path_p, child_path_p, (uiChildTokenLen - 1));

   uiParentTokenLen = of_strlen (parent_path_p) + 1;
   if (uiParentTokenLen == 1)
   {
      of_free (tmp_child_path_p);
      of_free (pChildToken);
      return FALSE;
   }
   /* As we distroy path while tokenizing, let us operate on a duplicate
      path entry. So that input will not be modified, and caller can
      use it later.
      */
   pTmparent_path_p = (char *) of_calloc (1, uiParentTokenLen);
   if (!pTmparent_path_p)
   {
      of_free (tmp_child_path_p);
      of_free (pChildToken);
      return FALSE;
   }
   /* Allocate token buffer */
   parent_token_p = (char *) of_calloc (1, uiParentTokenLen);
   if (!parent_token_p)
   {
      of_free (pTmparent_path_p);
      of_free (tmp_child_path_p);
      of_free (pChildToken);
      return FALSE;
   }
   of_strncpy (pTmparent_path_p, parent_path_p, (uiParentTokenLen - 1));

   while (return_value1 == UCMDM_PATH_PARSING_REMAINING)
   {
      return_value1 =
         cmi_dm_get_path_token (tmp_child_path_p, pChildToken, uiChildTokenLen,
               &uiChildIndex);
      return_value2 =
         cmi_dm_get_path_token (pTmparent_path_p, parent_token_p, uiParentTokenLen,
               &uiParentIndex);

      if (of_strcmp (pChildToken, parent_token_p))
      {
         of_free (parent_token_p);
         of_free (pTmparent_path_p);
         of_free (tmp_child_path_p);
         of_free (pChildToken);
         return FALSE;
      }
      if ((return_value1 == UCMDM_PATH_PARSING_COMPLETED)
            || (return_value2 == UCMDM_PATH_PARSING_COMPLETED))
      {
         of_free (parent_token_p);
         of_free (pTmparent_path_p);
         of_free (tmp_child_path_p);
         of_free (pChildToken);
         return TRUE;
      }
   }

   of_free (parent_token_p);
   of_free (pTmparent_path_p);
   of_free (tmp_child_path_p);
   of_free (pChildToken);
   return TRUE;
}

int32_t cmi_dm_get_file_name (char * name_path_p, char ** file_name_p)
{
   struct cm_dm_data_element *pDMNode, *pTmpDMNode;
   char *pFile;
   uint32_t ulFileNameLen;

   pDMNode = (struct cm_dm_data_element *) cmi_dm_get_dm_node_from_instance_path
      (name_path_p, of_strlen (name_path_p));
   pTmpDMNode = pDMNode;

   while (pTmpDMNode)
   {
      if (pTmpDMNode->save_file_name_p)
      {
         ulFileNameLen = of_strlen (pTmpDMNode->save_file_name_p);
         pFile = of_calloc (1, ulFileNameLen + 1);
         if (!pFile)
         {
            CM_DM_DEBUG_PRINT ("Memory Allocation Failed");
            return OF_FAILURE;
         }
         *file_name_p = pFile;
         return OF_SUCCESS;
      }
      else
         pTmpDMNode = pTmpDMNode->parent_p;
   }

   return OF_FAILURE;
}

int32_t cmi_dm_get_directory_name (char * name_path_p, char ** pDirName)
{
   struct cm_dm_data_element *pDMNode, *pTmpDMNode;
   char *pDir;
   uint32_t ulDirNameLen;

   pDMNode = (struct cm_dm_data_element *) cmi_dm_get_dm_node_from_instance_path
      (name_path_p, of_strlen (name_path_p));
   pTmpDMNode = pDMNode;

   while (pTmpDMNode)
   {
      if (pTmpDMNode->save_dir_name_p)
      {
         ulDirNameLen = of_strlen (pTmpDMNode->save_dir_name_p);
         pDir = of_calloc (1, ulDirNameLen + 1);
         if (!pDir)
         {
            CM_DM_DEBUG_PRINT ("Memory Allocation Failed");
            return OF_FAILURE;
         }
         *pDirName = pDir;
         return OF_SUCCESS;
      }
      else
         pTmpDMNode = pTmpDMNode->parent_p;
   }
   return OF_FAILURE;
}

 int32_t ucmdm_CopyNodeInfointoArray (struct cm_dm_node_info * node_info_p,
      struct cm_dm_node_info * node_info_array_p,
      uint32_t uindex_i)
{
   struct cm_dm_node_info *tmp_node_info_p;
   uint32_t iCnt =0;

   tmp_node_info_p = (node_info_array_p + uindex_i);
   of_memcpy (tmp_node_info_p, node_info_p, sizeof (struct cm_dm_node_info));
   /* name_p */
   tmp_node_info_p->name_p = (char *) of_calloc (1,
         of_strlen (node_info_p->name_p) + 1);
   if (tmp_node_info_p->name_p)
      of_strcpy (tmp_node_info_p->name_p, node_info_p->name_p);
   else
   {
      return OF_FAILURE;
   }

   /* friendly_name_p */
   if(node_info_p->friendly_name_p!=NULL)
   {
      tmp_node_info_p->friendly_name_p = (char *) of_calloc (1, of_strlen (node_info_p->friendly_name_p) + 1);
      if (tmp_node_info_p->friendly_name_p)
         of_strcpy (tmp_node_info_p->friendly_name_p, node_info_p->friendly_name_p);
      else
      {
         of_free(tmp_node_info_p->name_p);
         return OF_FAILURE;
      }
   }


   /* description_p */
   tmp_node_info_p->description_p = (char *) of_calloc (1,
         of_strlen
         (node_info_p->description_p)
         + 1);
   if (tmp_node_info_p->description_p)
      of_strcpy (tmp_node_info_p->description_p, node_info_p->description_p);
   else
   {
      if(tmp_node_info_p->friendly_name_p!=NULL)
         of_free(tmp_node_info_p->friendly_name_p);
      of_free(tmp_node_info_p->name_p);
      return OF_FAILURE;
   }

   /* dm_path_p */
   tmp_node_info_p->dm_path_p = (char *) of_calloc (1,
         of_strlen
         (node_info_p->dm_path_p) + 1);
   if (tmp_node_info_p->dm_path_p)
      of_strcpy (tmp_node_info_p->dm_path_p, node_info_p->dm_path_p);
   else
   {
      of_free(tmp_node_info_p->description_p);
      if(tmp_node_info_p->friendly_name_p!=NULL)
         of_free(tmp_node_info_p->friendly_name_p);
      of_free(tmp_node_info_p->name_p);
      return OF_FAILURE;
   }

   if((node_info_p->data_attrib.data_type == CM_DATA_TYPE_STRING || 
            node_info_p->data_attrib.data_type == CM_DATA_TYPE_STRING_SPECIAL_CHARS)
         && node_info_p->data_attrib.attrib_type == CM_DATA_ATTRIB_STR_ENUM)
   {
      tmp_node_info_p->data_attrib.attr.string_enum.count_ui= node_info_p->data_attrib.attr.string_enum.count_ui;
      for( iCnt =0 ;iCnt<node_info_p->data_attrib.attr.string_enum.count_ui;iCnt++)
      {
         if(of_strlen(node_info_p->data_attrib.attr.string_enum.array[iCnt]) > 0)
         {
            tmp_node_info_p->data_attrib.attr.string_enum.array[iCnt] =
               (char *) of_calloc (1, of_strlen ((char *)node_info_p->data_attrib.attr.string_enum.array[iCnt]) + 1);
            if (tmp_node_info_p->data_attrib.attr.string_enum.array[iCnt])
               of_strcpy (tmp_node_info_p->data_attrib.attr.string_enum.array[iCnt], node_info_p->data_attrib.attr.string_enum.array[iCnt]);
            else
            {
               of_free (tmp_node_info_p->name_p);
               if(tmp_node_info_p->friendly_name_p!=NULL)
                  of_free (tmp_node_info_p->friendly_name_p);
               of_free (tmp_node_info_p->description_p);
               of_free (tmp_node_info_p);
               return OF_FAILURE;
            }
         }
         else
            tmp_node_info_p->data_attrib.attr.string_enum.array[iCnt] = "";
      }

      for( iCnt =0 ;iCnt<node_info_p->data_attrib.attr.string_enum.count_ui;iCnt++)
      {
         if(of_strlen(node_info_p->data_attrib.attr.string_enum.aFrdArr[iCnt]) > 0)
         {
            tmp_node_info_p->data_attrib.attr.string_enum.aFrdArr[iCnt] =
               (char *) of_calloc (1, of_strlen ((char *)node_info_p->data_attrib.attr.string_enum.aFrdArr[iCnt]) + 1);
            if (tmp_node_info_p->data_attrib.attr.string_enum.aFrdArr[iCnt])
               of_strcpy (tmp_node_info_p->data_attrib.attr.string_enum.aFrdArr[iCnt], node_info_p->data_attrib.attr.string_enum.aFrdArr[iCnt]);
            else
            {
               of_free(tmp_node_info_p->data_attrib.attr.string_enum.array[iCnt]);
               of_free (tmp_node_info_p->name_p);
               if(tmp_node_info_p->friendly_name_p!=NULL)
                  of_free (tmp_node_info_p->friendly_name_p);
               of_free (tmp_node_info_p->description_p);
               of_free (tmp_node_info_p);
               return OF_FAILURE;
            }
         }
         else
            tmp_node_info_p->data_attrib.attr.string_enum.aFrdArr[iCnt] = "";

      }


   }


   return OF_SUCCESS;
}


int32_t cmi_dm_get_key_array_from_namepathToken (char * pToken,
      struct cm_dm_data_element *pNode,
      struct cm_array_of_nv_pair *pnv_pair_array,  uint32_t *puiNvIndex)
{
   struct cm_dm_data_element *pChildNode;
   UCMDllQNode_t *pDllQNode;
   uint32_t uiTokenLen, uiTokenIndex=0, uiKeyCnt=0, uindex_i=0, uiCnt=0, jj=0, uiNvPairIndex;
   uint32_t uiNodeKeyCnt=0;
   unsigned char bMultipleKeys=FALSE;
   unsigned char bChildinKeysFound=FALSE;
   char *key_p=NULL, *key_name_p=NULL, *key_value_p=NULL;
   char *key_name_value_p[CM_DM_MAX_KEY_INSTANCES];


   uiNvPairIndex=*puiNvIndex;


   if((!pToken) || (!pNode) || (!pnv_pair_array))
   {
      return OF_FAILURE;
   }

   uiTokenLen= of_strlen(pToken);

   key_p = (char *) of_calloc (1, uiTokenLen + 1);
   if (!key_p)
   {
      CM_DM_DEBUG_PRINT ("Memory allocation failed");
      return OF_FAILURE;
   }

   while ((pToken[uiCnt] != '}')&&(pToken[uiCnt] != '\0'))
   {
      if(pToken[uiCnt] == ',')
      {
         uiKeyCnt++;
      }
      uiCnt++;
   }

   if(uiCnt == 0 )
   {
      CM_DM_DEBUG_PRINT ("Key is empty");
      of_free(key_p); //CID 334
      return OF_FAILURE;
   }

   uiKeyCnt+=1;

   if (pNode->element_attrib.element_type == CM_DMNODE_TYPE_TABLE)
   {
      /* Scan through the Child List to get the Key Element */
      CM_DLLQ_SCAN (&pNode->child_list, pDllQNode, UCMDllQNode_t *)
      {
         pChildNode = CM_DLLQ_LIST_MEMBER (pDllQNode,
               struct cm_dm_data_element *, list_node);
         if (pChildNode->element_attrib.key_b == TRUE)
         {
            uiNodeKeyCnt++;
         }
      }
   }

   if(uiNodeKeyCnt != uiKeyCnt)
   {
      CM_DM_DEBUG_PRINT("No of keys do not match");
      return OF_FAILURE;
   }

   if(uiKeyCnt == 1 )
   {
      bMultipleKeys=FALSE;
      of_memset (key_p, 0, uiTokenLen);
      uiCnt=0;

      if(pToken[uiCnt] == '{')
      {
         uiCnt++;
      }
      for (jj = 0; ((pToken[uiCnt] != '}') && (pToken[uiCnt] != '\0')); uiCnt++, jj++)
      {
         key_p[jj] = pToken[uiCnt];
      }
      key_p[jj] = '\0';
      if ((key_p == NULL) || of_strlen(key_p)==0)
      {
         CM_DM_DEBUG_PRINT ("Key is empty");
         of_free(key_p); //CID 334
         return UCMJE_ERROR_ONE_OF_THE_PATH_KEYS_IS_NULL;
      }
   }
   else
   {
      bMultipleKeys=TRUE;
      for(uiCnt = 0; uiCnt< CM_DM_MAX_KEY_INSTANCES; uiCnt++)
      {
         key_name_value_p[uiCnt] = (char*)of_calloc(1, uiTokenLen);
         if(key_name_value_p[uiCnt]== NULL)
         {
            CM_DM_DEBUG_PRINT ("Memory allocation failed");
            for(jj = 0; jj<uiCnt; jj++)
            {
               of_free(key_name_value_p[jj]);
            }
            of_free (key_p);
            return OF_FAILURE;
         }
      }
      for (uindex_i=0, uiTokenIndex=0; uindex_i < uiKeyCnt; uindex_i++, uiTokenIndex+=uiCnt)
      {
         uiCnt=0;
         /*beginning of token*/
         if((pToken[uiTokenIndex+uiCnt] == '{') || (pToken[uiTokenIndex+uiCnt] == ','))
         {
            uiCnt=1;
         }
         for ( jj = 0;((pToken[uiTokenIndex+uiCnt] != '\0') && 
                  ((pToken[uiTokenIndex+uiCnt] != ',') && (pToken[uiTokenIndex+uiCnt] != '}')));
               uiCnt++, jj++)
         {
            key_name_value_p[uindex_i][jj] = pToken[uiTokenIndex+uiCnt];
         }
         key_name_value_p[uindex_i][jj] = '\0';
      }
   }

   if (pNode->element_attrib.element_type == CM_DMNODE_TYPE_TABLE)
   {
      /* Scan through the Child List to get the Key Element */
      CM_DLLQ_SCAN (&pNode->child_list, pDllQNode, UCMDllQNode_t *)
      {
         pChildNode = CM_DLLQ_LIST_MEMBER (pDllQNode,
               struct cm_dm_data_element *, list_node);
         if (pChildNode->element_attrib.key_b == TRUE)
         {
            /* Fill the Name-Value pair */
            bChildinKeysFound=FALSE;
            if( bMultipleKeys == TRUE)
            {
               for (uindex_i=0; uindex_i < uiKeyCnt; uindex_i++)
               {
                  key_name_p=of_strtok(key_name_value_p[uindex_i],"=");
                  key_value_p=of_strtok(NULL,",");//There can be "=" exists in the key value (ex:ocsp - subname record), hence changed "=" to "," as "," is used as token seperator and this cannot be present in the key value.
                  //key_value_p=of_strtok(NULL,"=");
                  if(!of_strcmp(pChildNode->name_p, key_name_p))
                  {
                     bChildinKeysFound=TRUE;
                     key_name_value_p[uindex_i][of_strlen(key_name_p)]='=';
                     break;
                  }
                  key_name_value_p[uindex_i][of_strlen(key_name_p)]='=';
               }
            }
            else
            {
               /*Single Key
                * Assumed that passed value is for key child*/
               bChildinKeysFound=TRUE;
            }

            if(bChildinKeysFound)
            {
               pnv_pair_array->nv_pairs[uiNvPairIndex].name_length =
                  of_strlen (pChildNode->name_p);
               pnv_pair_array->nv_pairs[uiNvPairIndex].name_p =
                  (char *) of_calloc (1,
                        pnv_pair_array->nv_pairs[uiNvPairIndex].
                        name_length + 1);
               if (pnv_pair_array->nv_pairs[uiNvPairIndex].name_p)
               {
                  of_strcpy (pnv_pair_array->nv_pairs[uiNvPairIndex].name_p,
                        pChildNode->name_p);
               }
               pnv_pair_array->nv_pairs[uiNvPairIndex].value_type =
                  pChildNode->data_attrib.data_type;
               if(bMultipleKeys == FALSE)
               {
                  pnv_pair_array->nv_pairs[uiNvPairIndex].value_length = of_strlen (key_p);
                  pnv_pair_array->nv_pairs[uiNvPairIndex].value_p =
                     (char *) of_calloc (1,
                           pnv_pair_array->nv_pairs[uiNvPairIndex].
                           value_length + 1);
                  if (pnv_pair_array->nv_pairs[uiNvPairIndex].value_p)
                  {
                     of_strcpy (pnv_pair_array->nv_pairs[uiNvPairIndex].value_p, key_p);
                  }
               }
               else
               {
                  pnv_pair_array->nv_pairs[uiNvPairIndex].value_length = of_strlen (key_value_p);
                  pnv_pair_array->nv_pairs[uiNvPairIndex].value_p =
                     (char *) of_calloc (1,
                           pnv_pair_array->nv_pairs[uiNvPairIndex].
                           value_length + 1);
                  if (pnv_pair_array->nv_pairs[uiNvPairIndex].value_p)
                  {
                     of_strcpy (pnv_pair_array->nv_pairs[uiNvPairIndex].value_p, key_value_p);
                  }
               }
               if(pChildNode->element_attrib.cli_identifier_p)
               {
                  if(of_strlen (pChildNode->element_attrib.cli_identifier_p) > 0)
                  {
                     pnv_pair_array->nv_pairs[uiNvPairIndex].identifier_len_ui = of_strlen (pChildNode->element_attrib.cli_identifier_p);
                     pnv_pair_array->nv_pairs[uiNvPairIndex].cli_identifier_p = (char *) of_calloc (1,pnv_pair_array->nv_pairs[uiNvPairIndex].identifier_len_ui + 1);
                     if (pnv_pair_array->nv_pairs[uiNvPairIndex].cli_identifier_p)
                     {
                        of_strcpy (pnv_pair_array->nv_pairs[uiNvPairIndex].cli_identifier_p, pChildNode->element_attrib.cli_identifier_p);
                     }
                  }
                  else
                  {
                     pnv_pair_array->nv_pairs[uiNvPairIndex].identifier_len_ui = 0;
                     pnv_pair_array->nv_pairs[uiNvPairIndex].cli_identifier_p = NULL;
                  } 
               }
               uiNvPairIndex++;
            }
            else
            {
               CM_DM_DEBUG_PRINT("%s is not a key child for Node %s", key_name_p, pNode->name_p);
               of_free (key_p);
               if(bMultipleKeys)
               {
                  for(uiCnt = 0; uiCnt< CM_DM_MAX_KEY_INSTANCES; uiCnt++)
                     of_free(key_name_value_p[uiCnt]);
               }
               return OF_FAILURE;
            }
         }
      }
   }
   *puiNvIndex= uiNvPairIndex;
   of_free(key_p); //CID 334
   return OF_SUCCESS;
}

int32_t cmi_verify_global_trans(struct cm_dm_data_element *pDMNode)
{
   UCMDllQ_t *pDll = NULL;
   UCMDllQNode_t *pNode=NULL;
   struct cm_dm_data_element *pChildNode = NULL; 

   if(pDMNode->element_attrib.sticky_to_parent_b != 1)
   {
      pDll = &(pDMNode->child_list); 
      CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
      {
         pChildNode =
            CM_DLLQ_LIST_MEMBER (pNode, struct cm_dm_data_element *, list_node);
         if(pChildNode->element_attrib.sticky_to_parent_b == TRUE)
         {
            return OF_SUCCESS;
         }
      }
      return OF_FAILURE;
   }
   return OF_FAILURE;
}

#endif /* CM_DM_SUPPORT */
#endif /* CM_SUPPORT */
