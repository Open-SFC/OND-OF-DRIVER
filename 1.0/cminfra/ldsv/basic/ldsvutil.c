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
 * File name:   ldsvutil.c
 * Author:      Freescale Semiconductor    
 * Date:        06/12/2013
 * Description: This file contains source code implementing 
 *              LDSV Util API
*/

#ifdef CM_LDSV_SUPPORT

#include "ldsvinc.h"
#include "dirent.h"
#include "hash.h"
#include "sys/types.h"
#include "sys/param.h"
#include "sys/stat.h"
#include "unistd.h"
#include "fcntl.h"
#include "errno.h"
#include "basic_ldsvinit.h"

/* Global Variables */
extern void *pTransportChannelJE;     /* Transport connection with JE */
extern char aCurrLoadingVersionNo_g[80];

void cm_ldsv_free_node_info_array (struct cm_array_of_structs * pArrayOfNodeInfo);

int32_t load_table_in_table_data(struct cm_array_of_nv_pair *array_of_key_nv_pairs_p,
                              void* pTransportChannel,
                              char * pInstDMPath,
                              char * pInstPath, struct cm_je_config_session *pConfigSess);

int32_t cm_ldsv_remove_keys_from_path (char * pInPath, char * pOutPath,
                                  uint32_t uiPathLen);

int32_t frame_and_send_to_je(struct cm_je_config_session *pConfigSess, char * pInsDMPath,
                       struct cm_array_of_nv_pair* pnv_pair_array, unsigned char bGroupedScalar);


 int32_t cm_ldsv_write_version_number_to_file(char* pFullPath, uint64_t iVersionNo);

int32_t cm_ldsv_get_version_number_from_file (FILE *pFile, char* pVersionNo, uint64_t *pullVersion);

int32_t cm_ldsv_prepare_load_instance_dm_path (struct cm_dm_node_info * pDMNodeAttrib,
                                       char* pdmPath,
                                       struct cm_array_of_nv_pair *dm_path_pArrayOfKeys,
                                       UCMDllQ_t *pLdsvCfgStackDllQ,
                                       char *pRefPath);

int32_t cm_ldsv_delete_load_stack_node_using_dm_path (UCMDllQ_t * pTableStackDll, char* dm_path_p);

int32_t cm_ldsv_delete_one_stack_node_using_dm_path (UCMDllQ_t * pTableStackDll, char* dm_path_p, 
                      struct cm_dm_node_info * pDMNodeAttrib, void *pTransportChannel, 
                      unsigned char *bMultiNode);

struct cm_array_of_nv_pair *cm_ldsv_collect_keys_from_stack_by_dm_path(char* dm_path_p, 
                                                  UCMDllQ_t* pTableStackDll);

int32_t cm_ldsv_load_table_in_table_file (FILE *pFile,
                          struct cm_dm_node_info* DMNodeAttrib,
                          void* pOpaqueInfo,
                          uint32_t uiOpaqueInfoLen,
                          void* pTransportChannel,
                          UCMDllQ_t *LoadTableStackNode, unsigned char bGroupedScalar, struct cm_je_config_session *pConfigSess);
/* Internal Functions */
/**************************************************************************
 Function Name : cm_ldsv_get_load_stack_node
 Input Args    : none
 Output Args   : 
                pTableStackNode : Table stack node.
 Description   : This method is used to get table stack node.
 Return Values : pTableStackNode of type cm_ldsv_table_stack_node_t structure.
 *************************************************************************/
cm_ldsv_table_stack_node_t *cm_ldsv_get_load_stack_node (UCMDllQ_t *pLdsvCfgStackDllQ)
{
  UCMDllQNode_t *pdllq;
  cm_ldsv_table_stack_node_t *pTableStackNode;

  /* Now collect the keys of parent table nodes */
  CM_DLLQ_SCAN (pLdsvCfgStackDllQ, pdllq, UCMDllQNode_t *)
  {
    pTableStackNode =
      CM_DLLQ_LIST_MEMBER (pdllq, cm_ldsv_table_stack_node_t *, ListNode);

    if (pTableStackNode)
    {
      return (pTableStackNode);
    }
  }
  return NULL;
}

/**************************************************************************
 Function Name : cm_ldsv_delete_load_stack_node
 Input Args    : 
                pTableStackDll: Stack node of type cm_ldsv_table_stack_node_t.
 Output Args   : none
 Description   : This method is used to delete one instance from the stack, 
                 After completed traversing all the directories under on instance. 
 Return Values : Successfull delete returns OF_SUCCESS,if any failure returns OF_FAILURE
 *************************************************************************/
int32_t cm_ldsv_delete_load_stack_node (UCMDllQ_t * pTableStackDll)
{
  cm_ldsv_table_stack_node_t *pTmpTableStackNode;

  UCMDllQNode_t *pNode;

  if (pTableStackDll != NULL)
  {
    pNode = CM_DLLQ_FIRST (pTableStackDll);
    pTmpTableStackNode =
      CM_DLLQ_LIST_MEMBER (pNode, cm_ldsv_table_stack_node_t*, ListNode);
    if (pTmpTableStackNode)
    {
      CM_LDSV_DEBUG_PRINT ("Deleting instance key:%s",
                            (char*)pTmpTableStackNode->ActiveInstKeys.nv_pairs->value_p);
      CM_DLLQ_DELETE_NODE (pTableStackDll, &(pTmpTableStackNode->ListNode));
    }
  }
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cm_ldsv_tokenize_data_and_send_to_je
 Input Args    : 
                pFile: File pointer
                pHashTable: Hash table contains keys information
                dm_path_p: Data Model Path from LDSV Save File
                pInstPath : Instance tree path
                pDMNodeAttrib : DM Node Attributes
                pKeyNvPair: Key Value Paits
                pOpaqueInfo: Opaque inforation
                uiOpaqueInfoLen: Opaque length
 Output Args   : none
 Description   : This method is used to Tokenize the data between <row> and </row> 
                from XML file,frame name value pairs and send it to JE
 Return Values : OF_SUCCESS/OF_FAILURE 
 *************************************************************************/
int32_t cm_ldsv_tokenize_data_and_send_to_je (FILE * pFile, 
                              char * dm_path_p, char * pInstPath,
                              struct cm_dm_node_info * pDMNodeAttrib,
                              struct cm_array_of_nv_pair* pKeyNvPair, void * pOpaqueInfo,
                              uint32_t uiOpaqueInfoLen,
                              struct cm_hash_table *pHashTable,
                              UCMDllQ_t *pLdsvCfgStackDllQ)
{
  char aReadLine[LDSV_XML_READ_LINE];
  char *pString = NULL, *pTempString = NULL;
  char *pTempToken1 = NULL;
  char *pTempToken2 = NULL;
  char *pTempToken3 = NULL;
  char aDictionary[100];
  char *pDictionary;
  void *pTransportChannel = NULL;
  int32_t i = 0, iRet;
  int32_t iHashSize;
  unsigned char scalar_group_b=FALSE, global_trans_b = FALSE;
  struct cm_array_of_nv_pair *array_of_key_nv_pairs_p = NULL;
  int32_t ii = 0;
  char *pResult = NULL;
  char *pResultArry[100];
  cm_ldsv_table_stack_node_t *pStackNode;
  /*unsigned char bTreeTraversalFirst = FALSE;
  uint32_t iStackRetVal =0;*/
  if (pHashTable == NULL)
  {
    CM_LDSV_DEBUG_PRINT ("Hash table is null.");
    return OF_FAILURE;
  }
  if ((dm_path_p == NULL) || (pInstPath == NULL))
  {
    CM_LDSV_DEBUG_PRINT ("DM Path is null");
    return OF_FAILURE;
  }
  CM_LDSV_DEBUG_PRINT ("dm_path_p=%s,Inst=%s:",dm_path_p,pInstPath);
  pTransportChannel = cm_ldsv_get_transport_channel();
  /* Validate transport channel */
  if (unlikely (pTransportChannel == NULL))
  {
    /* Create new transport channel */
    pTransportChannel = cm_tnsprt_create_channel (CM_IPPROTO_TCP, 0, 0,
                                                   UCMJE_CONFIGREQUEST_PORT);
  }
  memset (aReadLine, 0, sizeof(aReadLine));
  pString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);
  pTempString = cm_ldsv_trim_string (pString);
  if (pTempString == NULL)
  {
    CM_LDSV_DEBUG_PRINT ("Nothing to load, may be empty data file");
    return OF_FAILURE;
  }
  do
  {
    if (strcmp (pTempString, "<row>") == 0)
    {
      i = 0;
      iHashSize = cm_hash_size (pHashTable);
      /* allocate memory */
      array_of_key_nv_pairs_p =
        (struct cm_array_of_nv_pair *) of_calloc (1, sizeof (struct cm_array_of_nv_pair));
      if (unlikely (!array_of_key_nv_pairs_p))
      {
        return OF_FAILURE;
      }
      array_of_key_nv_pairs_p->count_ui = iHashSize;
      /* Allocate memory */
      array_of_key_nv_pairs_p->nv_pairs =
         (struct cm_nv_pair *) of_calloc (array_of_key_nv_pairs_p->count_ui,
                                    sizeof (struct cm_nv_pair));
      if (array_of_key_nv_pairs_p->nv_pairs == NULL)
      {
        of_free (array_of_key_nv_pairs_p);
        return OF_FAILURE;
      }

      pTempString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);
      pTempString = cm_ldsv_trim_string (pTempString);
      // tokenize for one complete record and fill nv pairs
      while (!(strcmp (pTempString, "</row>") == 0))
      {
        ii = 0;
        pTempToken1 = strtok (pTempString, "<");
        pTempToken2 = strchr (pTempToken1, '>');

        pTempToken2[0] = '\0';
        pTempToken3 = pTempToken2 + 1;

        pDictionary = (char *) cm_hash_lookup (pHashTable, pTempToken1);
        memset(aDictionary, 0, sizeof(aDictionary));
        if(pDictionary) //CID 395
          strcpy(aDictionary, pDictionary);

        CM_LDSV_DEBUG_PRINT ("%s::%s::%s=%s",pTempToken1,pTempToken2,pTempToken3,pDictionary);
        if (pDictionary == NULL)
        {
          pTempString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);
          pTempString = cm_ldsv_trim_string (pTempString);
          continue;
        }
        pResult = cm_ldsv_string_tokenize (aDictionary, " =");//String tokenize using 'space and equal to'
        while (pResult != NULL)
        {
          pResultArry[ii] = pResult;
          ii++;
          pResult = cm_ldsv_string_tokenize (NULL, " =");
        }
        iRet = cm_ldsv_fill_nv_pair_info (&(array_of_key_nv_pairs_p->nv_pairs[i]), pResultArry[0], //Name
                                       atoi (pResultArry[2]), //ValueType,
                                       strlen (pTempToken3),  //ValueLen
                                       (void *) pTempToken3); //Value
        if (unlikely (iRet == OF_FAILURE))
        {
          CM_LDSV_DEBUG_PRINT ("Failed to fill name value pairs.");
          cm_ldsv_clean_nv_pair_array (array_of_key_nv_pairs_p, TRUE);
          return OF_FAILURE;
        }
        i = i + 1;
        memset (aReadLine, 0, sizeof(aReadLine));
        pTempString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);
        pTempString = cm_ldsv_trim_string (pTempString);
      }
      array_of_key_nv_pairs_p->count_ui = i;
      scalar_group_b = FALSE;
      global_trans_b = FALSE;
      if (pDMNodeAttrib->element_attrib.scalar_group_b == TRUE)
         scalar_group_b = TRUE;
      if (pDMNodeAttrib->element_attrib.global_trans_b == TRUE)
         global_trans_b = TRUE;
   
      if (cm_ldsv_frame_and_send_add_params_to_je (pInstPath, array_of_key_nv_pairs_p,
                           pTransportChannel, dm_path_p, global_trans_b, scalar_group_b) == OF_FAILURE)
      {
        cm_ldsv_clean_nv_pair_array (array_of_key_nv_pairs_p, TRUE);
        iRet = OF_FAILURE;
      }
      if (unlikely (iRet != OF_FAILURE) && scalar_group_b != TRUE)
      {
        pKeyNvPair = cm_ldsv_find_key_nv_pair (pDMNodeAttrib, array_of_key_nv_pairs_p,
                                            pTransportChannel);
        if (pKeyNvPair != NULL)
        {
            CM_LDSV_DEBUG_PRINT ("dm_path_p=%s,Name=%s:",dm_path_p,pDMNodeAttrib->name_p);
          //if(bTreeTraversalFirst == FALSE)
          {
            pStackNode = cm_ldsv_create_table_stack_node (pDMNodeAttrib, dm_path_p,
                                           pDMNodeAttrib->name_p, pOpaqueInfo,
                                           uiOpaqueInfoLen, pKeyNvPair);
            if (pStackNode == NULL)
            {
              CM_LDSV_DEBUG_PRINT ("Failed to create stack");
              cm_ldsv_clean_nv_pair_array (array_of_key_nv_pairs_p, TRUE);
              return OF_FAILURE;
            }
            CM_DLLQ_APPEND_NODE (pLdsvCfgStackDllQ, &(pStackNode->ListNode));
            //bTreeTraversalFirst = TRUE;
          }
          /*else
          {
             if((  iStackRetVal = cm_ldsv_update_table_stack_node_key_nv_pair (pStackNode,pKeyNvPair)) != OF_SUCCESS)
             {
              CM_LDSV_DEBUG_PRINT ("Failed to update stack");
              cm_ldsv_clean_nv_pair_array (array_of_key_nv_pairs_p, TRUE);
              return OF_FAILURE;
             }
          }*/
        }
        cm_ldsv_clean_nv_pair_array (array_of_key_nv_pairs_p, TRUE);
      }
    }
    pString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);    /* get one line from the file */
    pTempString = cm_ldsv_trim_string (pString);
  } while (pString != NULL);
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name :load_table_in_table_data 
 Input Args    : array_of_key_nv_pairs_p - Array of name value pairs 
                 pTransportChannel -  transport channel
                 dm_path_p - Data Model path
                 pInstPath - Data Model instance path
                 pConfigSess - configuration session
 Output Args   : none 
 Description   : This method is used to load table inside table records
 Return Values : OF_FAILURE/OF_SUCCESS
 *************************************************************************/
int32_t load_table_in_table_data(struct cm_array_of_nv_pair* array_of_key_nv_pairs_p, 
                              void* pTransportChannel,
                              char *dm_path_p,
                              char *pInstPath, struct cm_je_config_session *pConfigSess)
{
  void * pOpaqueInfo;
  uint32_t uiOpaqueInfoLen;
  int32_t iRet;
  struct cm_dm_node_info* DMNodeAttrib, *pPrevDMNdAttrib=NULL, *pTmpNodeInfo;
  struct cm_role_info RoleInfo = { "VortiQa", CM_VORTIQA_SUPERUSER_ROLE };
  char * pFileName=NULL;
  char aDmPath[LDSV_DMPATH_SIZE]="", aInstancePath[LDSV_DMPATH_SIZE]="";
  char aRefDmPath[LDSV_DMPATH_SIZE]="";
  char aFullPath[LDSV_DMPATH_SIZE]="";
  char *pDirPath=NULL, *pTableDMPath = NULL;
  char *pInstPathFromFile=NULL;
  char *pdmPathFromFile=NULL;
  /*struct cm_hash_table *pTablInTabHashTable;*/
  FILE * pFile;
  struct cm_array_of_nv_pair *array_of_nv_pairs_p, *pArrayOfKeys;
  //struct cm_command CommandInfo;
  unsigned char bDataFound=FALSE, bIsChildNode=FALSE, bMultiInstance=FALSE;
  /*struct cm_array_of_nv_pair *pKeyNvPair;*/
  UCMDllQ_t TransStackNode;
  UCMDllQ_t *pTransStackDllQ = &TransStackNode;
  cm_ldsv_table_stack_node_t *pStackNode = NULL;
  UCMDllQNode_t *pDllqNode;
  int32_t ii, ij;
  struct cm_array_of_structs *pChildInfoArr = NULL, *pInTableChildArr = NULL;

  iRet = cm_dm_get_first_node_info (dm_path_p,
                                UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
                                pTransportChannel,
                                CM_LDSV_MGMT_ENGINE_ID,
                                &RoleInfo,
                                &DMNodeAttrib, &pOpaqueInfo, &uiOpaqueInfoLen);
  if ((DMNodeAttrib == NULL) || (iRet == OF_FAILURE))
  {
    CM_LDSV_DEBUG_PRINT ("GetFirstNodeInfo failed.");
    return OF_FAILURE;
  }
  pDirPath = (char *) of_calloc (1, LDSV_DMPATH_SIZE);
  if (pDirPath == NULL)
  {
    CM_LDSV_DEBUG_PRINT ("Failed to create memory.");
    return OF_FAILURE;
  }
  pFileName = (char *) of_calloc (1, CM_SAVE_FILE_NAME_MAX_LEN);
  if (pFileName == NULL)
  {
    CM_LDSV_DEBUG_PRINT ("Failed to create memory for File Name.");
    of_free(pDirPath);
    return OF_FAILURE;
  }

  CM_LDSV_DEBUG_PRINT ("dm_path_p=%s,DMNodeAttrib->Name=%s",dm_path_p,DMNodeAttrib->name_p);
  //memset(&CommandInfo,0,sizeof(CommandInfo));
  pInstPathFromFile = (char *) of_calloc (LDSV_DMPATH_SIZE, sizeof (char));
  pdmPathFromFile = (char *) of_calloc (LDSV_DMPATH_SIZE, sizeof (char));
  if ((pInstPathFromFile == NULL) || (pdmPathFromFile == NULL))
  {
    CM_LDSV_DEBUG_PRINT (" Unable to create memory");
    if(pInstPathFromFile)         /*CID 368*/
      of_free(pInstPathFromFile);
    if(pdmPathFromFile)
      of_free(pdmPathFromFile); /*CID 365*/
    of_free (pFileName);
    of_free (pDirPath); /*CID 364*/
    //cm_hash_free (pTablInTabHashTable, NULL);/*CID 363*/
    return OF_FAILURE;
  }
    /*initializing double linked list */
   CM_DLLQ_INIT_LIST (&TransStackNode);

  cm_dm_get_key_childs (pTransportChannel, DMNodeAttrib->dm_path_p,
                       CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, &pChildInfoArr);

        memset(aDmPath, 0, sizeof(aDmPath));
        strcpy (aDmPath, pInstPath);
        CM_LDSV_DEBUG_PRINT("aDmPath::%s", aDmPath);
        strncat(aDmPath,"{",1);
        for (ij = 0; ij < pChildInfoArr->count_ui; ij++)
        {
          pTmpNodeInfo = (struct cm_dm_node_info *) (pChildInfoArr->struct_arr_p) + ij;
          if (pTmpNodeInfo->element_attrib.element_type == CM_DMNODE_TYPE_SCALAR_VAR)
          {
            for (ii=0; ii< array_of_key_nv_pairs_p->count_ui; ii++)
            {
              if(strcmp(pTmpNodeInfo->name_p, array_of_key_nv_pairs_p->nv_pairs[ii].name_p) == 0)
              {
                if(pChildInfoArr->count_ui > 1)
                {
                  strncat(aDmPath, pTmpNodeInfo->name_p, strlen(pTmpNodeInfo->name_p));
                  strncat(aDmPath,"=",1);
                }
                strncat(aDmPath, array_of_key_nv_pairs_p->nv_pairs[ii].value_p, array_of_key_nv_pairs_p->nv_pairs[ii].value_length);
                if(ij != (pChildInfoArr->count_ui-1))
                {
                  strncat(aDmPath,",",1);
                }
              }
            }
          }
        }
        strncat(aDmPath,"}",1);
        strcpy(aRefDmPath,aDmPath);

  while (iRet == OF_SUCCESS)
  {
    if (DMNodeAttrib == NULL)
    {
      cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo, 
                                        uiOpaqueInfoLen);
      of_free(pFileName);
      of_free(pDirPath);
      of_free(pInstPathFromFile);
      of_free(pdmPathFromFile);
      //cm_hash_free (pTablInTabHashTable, NULL);
      return OF_FAILURE;
    }
    if (DMNodeAttrib->element_attrib.parent_trans_b == TRUE)
    {
      if (pTableDMPath != NULL)
      {
        bIsChildNode = cm_dm_is_child_element (DMNodeAttrib->dm_path_p,
                                   pTableDMPath,
                                   pTransportChannel,
                                   CM_LDSV_MGMT_ENGINE_ID,
                                   &RoleInfo);
      }
      else
      {
        bIsChildNode = FALSE;
      }

    if (bIsChildNode == FALSE)
    {
    switch (DMNodeAttrib->element_attrib.element_type)
    {
      case CM_DMNODE_TYPE_TABLE:
        memset (pDirPath, 0, sizeof (pDirPath));
        memset(pFileName, 0, sizeof(pFileName));
#if 0
        memset(aDmPath, 0, sizeof(aDmPath));
        strcpy (aDmPath, pInstPath);
        CM_LDSV_DEBUG_PRINT("aDmPath::%s", aDmPath);
        strncat(aDmPath,"{",1);
        for (ij = 0; ij < pChildInfoArr->count_ui; ij++)
        {
          pTmpNodeInfo = (struct cm_dm_node_info *) (pChildInfoArr->struct_arr_p) + ij;
          if (pTmpNodeInfo->element_attrib.element_type == CM_DMNODE_TYPE_SCALAR_VAR)
          {
            for (ii=0; ii< array_of_key_nv_pairs_p->count_ui; ii++)
            {
              if(strcmp(pTmpNodeInfo->name_p, array_of_key_nv_pairs_p->nv_pairs[ii].name_p) == 0)
              {
                if(pChildInfoArr->count_ui > 1)
                {
                  strncat(aDmPath, pTmpNodeInfo->name_p, strlen(pTmpNodeInfo->name_p));
                  strncat(aDmPath,"=",1);
                }
                strncat(aDmPath, array_of_key_nv_pairs_p->nv_pairs[ii].value_p, array_of_key_nv_pairs_p->nv_pairs[ii].value_length);
                if(ij != (pChildInfoArr->count_ui-1))
                {
                  strncat(aDmPath,",",1);
                }
              }
            }
          }
        }
        strncat(aDmPath,"}",1);

#endif
        memset(aDmPath,0,sizeof(aDmPath));
        cm_ldsv_prepare_load_instance_dm_path(DMNodeAttrib,aDmPath,NULL,pTransStackDllQ,aRefDmPath);
        CM_LDSV_DEBUG_PRINT("aDmPath::%s::DMNodeAttrib->pDMpath:%s", aDmPath, DMNodeAttrib->dm_path_p);
        if(strlen(DMNodeAttrib->file_name) == 0) //CID 193
        {
          CM_LDSV_DEBUG_PRINT ("aFullPath=%s,NO FILE,pDirPath=%s,KeyVal=%s: aDmPath=%s,pInstPath=%s:",aFullPath,pDirPath, array_of_key_nv_pairs_p->nv_pairs[0].value_p,aDmPath,pInstPath);
          break;
        }
        strncpy(pFileName, DMNodeAttrib->file_name, strlen(DMNodeAttrib->file_name)+1);
        strcat(pFileName, ".xml");
#if 0
        if (CM_DLLQ_COUNT(pTransStackDllQ)>0)
        {
          pDllqNode = CM_DLLQ_LAST (pTransStackDllQ);
          pStackNode =
             CM_DLLQ_LIST_MEMBER (pDllqNode, cm_ldsv_table_stack_node_t *, ListNode);
          if (pStackNode != NULL)
          {
            CM_LDSV_DEBUG_PRINT("StackNode DMpath:%s: Node Name:%s:", pStackNode->dm_path_p, pStackNode->pElementName);
            cm_dm_get_key_childs (pTransportChannel, pStackNode->dm_path_p,
                                 CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, &pInTableChildArr);
            pArrayOfKeys = cm_ldsv_collect_keys_from_stack_by_dm_path(pStackNode->dm_path_p, pTransStackDllQ);
            if ((pArrayOfKeys != NULL) && (pArrayOfKeys->count_ui)>0)
            {
              CM_LDSV_DEBUG_PRINT("aDmPath::%s", aDmPath);
              strncat(aDmPath,".",1);
              strncat(aDmPath, pStackNode->pElementName, strlen(pStackNode->pElementName));
              strncat(aDmPath,"{",1);
              for (ij = 0; ij < pInTableChildArr->count_ui; ij++)
              {
                pTmpNodeInfo = (struct cm_dm_node_info *) (pInTableChildArr->struct_arr_p) + ij;
                if (pTmpNodeInfo->element_attrib.element_type == CM_DMNODE_TYPE_SCALAR_VAR)
                {
                  for (ii=0; (ii < pArrayOfKeys->count_ui && (ii<pInTableChildArr->count_ui) ); ii++)
                  {
                    if(strcmp(pTmpNodeInfo->name_p, pArrayOfKeys->nv_pairs[ii].name_p) == 0)
                    {
                      if(pInTableChildArr->count_ui > 1)
                      {
                        strncat(aDmPath, pTmpNodeInfo->name_p, strlen(pTmpNodeInfo->name_p));
                        strncat(aDmPath,"=",1);
                      }
                      strncat(aDmPath, pArrayOfKeys->nv_pairs[ii].value_p, pArrayOfKeys->nv_pairs[ii].value_length);
                      if(ij != (pInTableChildArr->count_ui-1))
                      {
                        strncat(aDmPath,",",1);
                      }
                    }
                  }
                }
              }
              strncat(aDmPath,"}",1);
            }
          }
        }
#endif
        CM_LDSV_DEBUG_PRINT("aDmPath::%s:", aDmPath);
        cm_ldsv_create_directory_path (aDmPath, pDirPath);
        /*Get the current loading version from the previously preserved global variable */
        CM_LDSV_CREATE_COMPLETE_FILE_PATH (aFullPath, pDirPath, pFileName, aCurrLoadingVersionNo_g);
        CM_LDSV_DEBUG_PRINT ("aFullPath=%s,pFileName=%s,pDirPath=%s,KeyVal=%s: aDmPath=%s,pInstPath=%s:",aFullPath,pFileName,pDirPath, array_of_key_nv_pairs_p->nv_pairs[0].value_p,aDmPath,pInstPath);
        pPrevDMNdAttrib = DMNodeAttrib;
        if ((pFile = fopen (aFullPath, "r")) != NULL)
        {
          if (cm_ldsv_load_table_in_table_file(pFile, DMNodeAttrib, pOpaqueInfo, uiOpaqueInfoLen, 
                                      pTransportChannel, pTransStackDllQ, FALSE, pConfigSess) != OF_SUCCESS)
          {
            cm_ldsv_clean_dm_node_attrib_info (DMNodeAttrib);
            cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo, 
                                        uiOpaqueInfoLen);
            of_free(pFileName);
            of_free(pDirPath);
            of_free(pdmPathFromFile); /*CID 365*/
            of_free(pInstPathFromFile);
            //cm_hash_free (pTablInTabHashTable, NULL);
            fclose(pFile); /*CID 367*/
            cm_ldsv_clean_nv_pair_array (array_of_nv_pairs_p, FALSE); /*CID 362*/
            return OF_FAILURE;
          }
          fclose(pFile);
        }
        break;

      case CM_DMNODE_TYPE_ANCHOR:
        if(DMNodeAttrib->element_attrib.scalar_group_b == TRUE)
        {
          memset (pDirPath, 0, sizeof (pDirPath));
          memset(aDmPath, 0, sizeof(aDmPath));
          memset(pFileName, 0, sizeof(pFileName));
#if 0
          strcpy (aDmPath, pInstPath);
          strncat (aDmPath, "{", 1);
        for (ij = 0; ij < pChildInfoArr->count_ui; ij++)
        {
          pTmpNodeInfo = (struct cm_dm_node_info *) (pChildInfoArr->struct_arr_p) + ij;
          if (pTmpNodeInfo->element_attrib.element_type == CM_DMNODE_TYPE_SCALAR_VAR)
          {
            for (ii=0; ii< array_of_key_nv_pairs_p->count_ui; ii++)
            {
              if(strcmp(pTmpNodeInfo->name_p, array_of_key_nv_pairs_p->nv_pairs[ii].name_p) == 0)
              {
                if(pChildInfoArr->count_ui > 1)
                {
                  strncat(aDmPath, pTmpNodeInfo->name_p, strlen(pTmpNodeInfo->name_p));
                  strncat(aDmPath,"=",1);
                }
                strncat(aDmPath, array_of_key_nv_pairs_p->nv_pairs[ii].value_p, array_of_key_nv_pairs_p->nv_pairs[ii].value_length);
                if(ij != (pChildInfoArr->count_ui-1))
                {
                  strncat(aDmPath,",",1);
                }
              }
            }
          }
        }
        strncat(aDmPath,"}",1);
#endif

          if(strlen(DMNodeAttrib->file_name) == 0) //CID 193
          {
            CM_LDSV_DEBUG_PRINT ("aFullPath=%s,NO FILE,pDirPath=%s,KeyVal=%s: aDmPath=%s,pInstPath=%s:",aFullPath,pDirPath, array_of_key_nv_pairs_p->nv_pairs[0].value_p,aDmPath,pInstPath);
            break;
          }
          strncpy(pFileName, DMNodeAttrib->file_name, strlen(DMNodeAttrib->file_name)+1);
          strcat(pFileName, ".xml");
          memset(aDmPath,0,sizeof(aDmPath));
          cm_ldsv_prepare_load_instance_dm_path(DMNodeAttrib,aDmPath,NULL,pTransStackDllQ,aRefDmPath);

          cm_ldsv_create_directory_path (aDmPath, pDirPath);
          /*Get the current loading version from the previously preserved global variable */
          CM_LDSV_CREATE_COMPLETE_FILE_PATH (aFullPath, pDirPath, pFileName, aCurrLoadingVersionNo_g);
          pPrevDMNdAttrib = DMNodeAttrib;
          CM_LDSV_DEBUG_PRINT ("aFullPath=%s,pFileName=%s,pDirPath=%s,KeyVal=%s: aDmPath=%s,pInstPath=%s:",aFullPath,pFileName,pDirPath, array_of_key_nv_pairs_p->nv_pairs[0].value_p,aDmPath,pInstPath);
          if ((pFile = fopen (aFullPath, "r")) != NULL)
          {
            cm_ldsv_load_table_in_table_file (pFile, DMNodeAttrib, pOpaqueInfo, uiOpaqueInfoLen, 
                                         pTransportChannel, pTransStackDllQ, TRUE, pConfigSess);
            fclose(pFile);
          }
        }
        break;

    } // switch
    } // ischild node
    } // if parent transaction
    else
    {
      if (strcmp(DMNodeAttrib->dm_path_p, dm_path_p)!=0)
        pTableDMPath = DMNodeAttrib->dm_path_p; 
    }
    /* Now get the next node from DM template tree */
    iRet = cm_dm_get_next_table_n_anchor_node_info (dm_path_p,
                                  UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
                                  pTransportChannel,
                                  CM_LDSV_MGMT_ENGINE_ID,
                                  &RoleInfo,
                                  &DMNodeAttrib, &pOpaqueInfo, &uiOpaqueInfoLen);

    if (iRet == OF_FAILURE)
    {
      CM_LDSV_DEBUG_PRINT ("Get Next Node info failed.");
      if (CM_DLLQ_COUNT (pTransStackDllQ) != 0)
      {
        bMultiInstance = FALSE;
        pDllqNode = CM_DLLQ_LAST (pTransStackDllQ);
        pStackNode =
             CM_DLLQ_LIST_MEMBER (pDllqNode, cm_ldsv_table_stack_node_t *, ListNode);
        if (pStackNode != NULL)
        {
          cm_ldsv_delete_one_stack_node_using_dm_path(pTransStackDllQ, pStackNode->dm_path_p, 
                                     DMNodeAttrib, pTransportChannel, &bMultiInstance);
          if ((bMultiInstance == TRUE) && (CM_DLLQ_COUNT (pTransStackDllQ) != 0))
          {
            // Last node is nothing but, recently added record.
            pDllqNode = CM_DLLQ_LAST (pTransStackDllQ);
            pStackNode =
              CM_DLLQ_LIST_MEMBER (pDllqNode, cm_ldsv_table_stack_node_t *, ListNode);
            if (pStackNode != NULL)
            {
              memcpy (pOpaqueInfo, pStackNode->pOpaqueInfo,
                        (sizeof (uint32_t) * pStackNode->uiOpaqueInfoLen));
              uiOpaqueInfoLen = pStackNode->uiOpaqueInfoLen;
              CM_LDSV_DEBUG_PRINT("GetNextNodeInfo of StackNode DMPath:%s", pStackNode->dm_path_p);
              iRet = cm_dm_get_next_node_info (pStackNode->dm_path_p,
                                         UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
                                         pTransportChannel,
                                         CM_LDSV_MGMT_ENGINE_ID,
                                         &RoleInfo,
                                         &DMNodeAttrib, &pOpaqueInfo,
                                         &uiOpaqueInfoLen);
             }
             else
             {
               cm_ldsv_clean_dm_node_attrib_info (DMNodeAttrib);
               cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo, 
                                                   uiOpaqueInfoLen);
               of_free(pFileName);
               of_free(pInstPathFromFile);
               of_free(pdmPathFromFile);
               of_free(pDirPath);
               //cm_hash_free (pTablInTabHashTable, NULL);
               if (bDataFound == TRUE)
                 cm_ldsv_clean_nv_pair_array (array_of_nv_pairs_p, TRUE); /*CID 362*/
               while(CM_DLLQ_COUNT(pTransStackDllQ)>0)
               {
                 cm_ldsv_delete_load_stack_node (pTransStackDllQ);
               }
               return OF_FAILURE;
             }
          }
        }
      }
      else
      {
        cm_ldsv_clean_dm_node_attrib_info (DMNodeAttrib);
        cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo, 
                                            uiOpaqueInfoLen);
        of_free(pFileName);
        of_free(pInstPathFromFile);
        of_free(pdmPathFromFile);
        of_free(pDirPath);
        /*cm_hash_free (pTablInTabHashTable, NULL);*/
        if (bDataFound == TRUE)
          cm_ldsv_clean_nv_pair_array (array_of_nv_pairs_p, TRUE); /*CID 362*/
        while(CM_DLLQ_COUNT(pTransStackDllQ)>0)
        {
          cm_ldsv_delete_load_stack_node (pTransStackDllQ);
        }
        return OF_SUCCESS;
      }
    }
    CM_LDSV_DEBUG_PRINT ("dm_path_p=%s,DMNodeAttrib->Name=%s",dm_path_p,DMNodeAttrib->name_p);
    if (CM_DLLQ_COUNT (pTransStackDllQ) != 0)
    {
       if (pPrevDMNdAttrib != NULL)
       {
       bIsChildNode = cm_dm_is_child_element (DMNodeAttrib->dm_path_p,
                                   pPrevDMNdAttrib->dm_path_p,
                                   pTransportChannel,
                                   CM_LDSV_MGMT_ENGINE_ID,
                                   &RoleInfo);
      if (bIsChildNode == FALSE)
      {
         // Get last added instances from stack using DMPath and Delete one by one
         if(cm_ldsv_delete_load_stack_node_using_dm_path(pTransStackDllQ, 
                    pPrevDMNdAttrib->dm_path_p) != OF_SUCCESS)
         {
           // Free all allocated memory and return from here
            cm_ldsv_clean_dm_node_attrib_info (DMNodeAttrib);
            cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo,
                                          uiOpaqueInfoLen);
            of_free(pFileName);
            of_free(pInstPathFromFile);
            of_free(pdmPathFromFile);
            of_free(pDirPath);
            //cm_hash_free (pTablInTabHashTable, NULL);
            if (bDataFound == TRUE)
              cm_ldsv_clean_nv_pair_array (array_of_nv_pairs_p, TRUE); /*CID 362*/
            while(CM_DLLQ_COUNT(pTransStackDllQ)>0)
            {
              cm_ldsv_delete_load_stack_node (pTransStackDllQ);
            }
           return OF_FAILURE;
         }
        if (CM_DLLQ_COUNT(pTransStackDllQ)>0)
        {
        pDllqNode = CM_DLLQ_LAST (pTransStackDllQ);
        pStackNode =
           CM_DLLQ_LIST_MEMBER (pDllqNode, cm_ldsv_table_stack_node_t *, ListNode);
        if (pStackNode != NULL)
        {
        bIsChildNode = cm_dm_is_child_element (DMNodeAttrib->dm_path_p,
                                   pStackNode->dm_path_p,
                                   pTransportChannel,
                                   CM_LDSV_MGMT_ENGINE_ID,
                                   &RoleInfo);
        if (bIsChildNode == FALSE)
        {
          CM_LDSV_DEBUG_PRINT("pDMNdAttrib->dm_path_p:%s  ::  pStackNode->dm_path_p:%s", DMNodeAttrib->dm_path_p, pStackNode->dm_path_p);
          bMultiInstance = FALSE;
          cm_ldsv_delete_one_stack_node_using_dm_path(pTransStackDllQ, pStackNode->dm_path_p, 
                                     DMNodeAttrib, pTransportChannel, &bMultiInstance);
          if ((bMultiInstance == TRUE) && (CM_DLLQ_COUNT (pTransStackDllQ) != 0))
          {
            // Last node is nothing but, recently added record.
            pDllqNode = CM_DLLQ_LAST (pTransStackDllQ);
            pStackNode =
              CM_DLLQ_LIST_MEMBER (pDllqNode, cm_ldsv_table_stack_node_t *, ListNode);
            if (pStackNode != NULL)
            {
              memcpy (pOpaqueInfo, pStackNode->pOpaqueInfo,
                        (sizeof (uint32_t) * pStackNode->uiOpaqueInfoLen));
              uiOpaqueInfoLen = pStackNode->uiOpaqueInfoLen;
              CM_LDSV_DEBUG_PRINT("GetNextNodeInfo pStackNode->dm_path_p:%s", pStackNode->dm_path_p);
              iRet = cm_dm_get_next_node_info (pStackNode->dm_path_p,
                                         UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
                                         pTransportChannel,
                                         CM_LDSV_MGMT_ENGINE_ID,
                                         &RoleInfo,
                                         &DMNodeAttrib, &pOpaqueInfo,
                                         &uiOpaqueInfoLen);
            }
          }
         }
        }
       }
      }
      }
    }
  }
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : frame_and_send_to_je
 Input Args    : pInsDMPath: Data Model Instance Path
                 pnv_pair_array : Name Value pairs
                 pConfigSess: configuration Session 
                 bGroupedScalar - boolean variable to find grouped scalar or not
 Output Args   : none
 Description   : This method is used to Frame name value pairs and send into 
                 to JE in commands format.
 Return Values : OF_FAILURE/OF_SUCCESS
*************************************************************************/
int32_t frame_and_send_to_je(struct cm_je_config_session *pConfigSess, char * pInsDMPath,
                       struct cm_array_of_nv_pair* pnv_pair_array, unsigned char bGroupedScalar)
{
  struct cm_result *pResult;
  struct cm_command CommandInfo;

  /* Fill Command Info structure */
  memset (&CommandInfo, 0, sizeof (CommandInfo));

  if (!pConfigSess)
  {
    CM_LDSV_DEBUG_PRINT ("UCMConfigSession is null");
    return OF_FAILURE;
  }
  if(bGroupedScalar == FALSE)
  {
    cm_ldsv_fill_command_info (CM_CMD_ADD_PARAMS, pInsDMPath,
                          pnv_pair_array->count_ui, pnv_pair_array->nv_pairs,
                          &CommandInfo);
  }
  else
  {
    cm_ldsv_fill_command_info (CM_CMD_SET_PARAMS, pInsDMPath,
                          pnv_pair_array->count_ui, pnv_pair_array->nv_pairs,
                          &CommandInfo);
  }
  /* Send to JE using ConfigSessionUpdate */
  if (cm_config_session_update_cmd (pConfigSess,
                                 &CommandInfo, &pResult) != OF_SUCCESS)
  {
    CM_LDSV_DEBUG_PRINT (" cm_config_session_update_cmd failed");
    if (cm_config_session_end (pConfigSess, CM_CMD_CANCEL_TRANS_CMD, &pResult) == OF_FAILURE)
    {
      CM_LDSV_DEBUG_PRINT("Cancel global transaction failed.");
      return OF_FAILURE;
    }
    if (pResult)
    {
      UCMFreeUCMResult (pResult);
    }
    return OF_FAILURE;
  }
  
  if (pResult)
  {
    UCMFreeUCMResult (pResult);
  }
  return OF_SUCCESS;
}


/**************************************************************************
 Function Name : cm_ldsv_tokenize_dictionary
 Input Args    : 
	              pFile: File Pointer
                pHashTable: Hash Table Contains Dictionary part name and Values
                from Save file
 Output Args   : none
 Description   : This method is used to Tokenize Dictionary part from LDSV save file
                 and adds name & Values into hash table for further use. 
 Return Values : OF_FAILURE/OF_SUCCESS
 *************************************************************************/
int32_t cm_ldsv_tokenize_dictionary (FILE * pFile, struct cm_hash_table * pHashTable)
{
  char aReadLine[LDSV_XML_READ_LINE];
  char *pString = NULL, *pTempString = NULL;
  char *pTempToken1 = NULL;
  char *pTempToken2 = NULL;
  char *pTempToken3 = NULL;
  uint32_t iRet;

  memset (aReadLine, 0, sizeof(aReadLine));
  pString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);      /* get one line from the file */
  pTempString = cm_ldsv_trim_string (pString);

  while (strcmp (pTempString, "<dictionary>") != 0)
  {
    memset (aReadLine, 0, sizeof(aReadLine));
    pString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);    /* get one line from the file */
    if (pString == NULL)
    {
      // end of file
      return OF_FAILURE;
    }
    pTempString = cm_ldsv_trim_string (pString);
  }
  // Now we are at dictionary
  memset (aReadLine, 0, sizeof(aReadLine));
  pString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);
  pTempString = cm_ldsv_trim_string (pString);
  CM_LDSV_DEBUG_PRINT ("Dictionary");
  while (!(strcmp (pTempString, "</dictionary>") == 0))
  {
    pTempToken1 = strtok (pTempString, "<");
    pTempToken2 = strchr (pTempToken1, '>');
    if (pTempToken2)
    {
      pTempToken2[0] = '\0';
      pTempToken3 = pTempToken2 + 1;

     iRet = cm_hash_add_entry (pHashTable, pTempToken1, (void *) strdup (pTempToken3));
     if (iRet == -1)
     {
       /* Name already there in the hash so, update with new value.*/
       cm_hash_update_entry(pHashTable, pTempToken1, (void *) strdup (pTempToken3), NULL);
     }
    }
    CM_LDSV_DEBUG_PRINT ("%s:%s",pTempToken1,pTempToken3);
    memset (aReadLine, 0, sizeof(aReadLine));
    pTempString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);
    pTempString = cm_ldsv_trim_string (pTempString);
  }                             // while
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cm_ldsv_trim_string
 Input Args    : 
                pString: String to remove leading and trailing spaces.
 Output Args   : none
 Description   : This method is used to remove triling and leading spaces.
 Return Values : New String after removing spaces.
 *************************************************************************/
char *cm_ldsv_trim_string (char * pString)
{
  char *pInBuffer = pString, *pOutBuff = pString;
  int32_t i = 0, iCount = 0;

  if (pString)
  {
    for (pInBuffer = pString; *pInBuffer && isspace (*pInBuffer); ++pInBuffer)
      ;
    if (pString != pInBuffer)
      memmove (pString, pInBuffer, pInBuffer - pString);

    while (*pInBuffer)
    {
      if (isspace (*pInBuffer) && iCount)
        pInBuffer++;
      else
      {
        if (!isspace (*pInBuffer))
          iCount = 0;
        else
        {
          *pInBuffer = ' ';
          iCount = 1;
        }
        pOutBuff[i++] = *pInBuffer++;
      }
    }
    pOutBuff[i] = '\0';

    while (--i >= 0)
    {
      if (!isspace (pOutBuff[i]))
        break;
    }
    pOutBuff[++i] = '\0';
  }
  return pString;
}

/**************************************************************************
 Function Name : cm_ldsv_get_dm_path_and_instance_path_from_save_file
 Input Args    : 
                pFile: File Pointer
 Output Args   : 
                dm_path_p: Data Model Template tree Path
                pInstPath: Data Model Instance tree Path
 Description   : This method is used to get Data Model template tree path and
                Data Model Instance Path from LDSV save file.
 Return Values : OF_SUCCESS/OF_FAILURE
*************************************************************************/
int32_t cm_ldsv_get_dm_path_and_instance_path_from_save_file (FILE * pFile,
                                               char * dm_path_p,
                                               char * pInstPath)
{

  char aReadLine[LDSV_XML_READ_LINE];
  char *pString = NULL, *pTempString = NULL;
  char *pTempToken1 = NULL;
  char *pTempToken2 = NULL;
  char *pDmPathToken = "dmpath";
  char *pInstPathToken = "instancepath";

  if ((dm_path_p == NULL) || (pInstPath == NULL))
    return OF_FAILURE;

  memset (aReadLine, 0, sizeof(aReadLine));
  pString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);      /* get one line from the file */
  pTempString = cm_ldsv_trim_string (pString);

  while (strcmp (pTempString, "<header>") != 0)
  {
    memset (aReadLine, 0, sizeof(aReadLine));
    pString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);    /* get one line from the file */
    if (pString == NULL)
    {
      // end of file 
      return OF_FAILURE;
    }
    pTempString = cm_ldsv_trim_string (pString);
  }
  CM_LDSV_DEBUG_PRINT ("Received String from file =%s:",pTempString);

  // Now we are at Header
  memset (aReadLine, 0, sizeof(aReadLine));
  pString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);
  pTempString = cm_ldsv_trim_string (pString);
  if (pTempString == NULL)
    return OF_FAILURE;
  if (pTempString != NULL)
  {
    while (!(strcmp (pTempString, "</header>") == 0))
    {
      pTempString = strtok (pTempString, "<");
      pTempToken1 = strchr (pTempString, '>');
      if(pTempToken1)
      {
        pTempToken1[0] = '\0';
        pTempToken2 = pTempToken1 + 1;
      }
      pTempString = cm_ldsv_trim_string (pTempString);
      if (strcmp (pTempString, pDmPathToken) == 0)
      {
        strcpy (dm_path_p, pTempToken2);
      }
      if (strcmp (pTempString, pInstPathToken) == 0)
      {
        strcpy (pInstPath, pTempToken2);
      }
      memset (aReadLine, 0, sizeof(aReadLine));
      pString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);
      pTempString = cm_ldsv_trim_string (pString);
      while((pTempString == NULL) || strlen(pTempString)==0)
      {
        memset (aReadLine, 0, sizeof(aReadLine));
        pString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);
        pTempString = cm_ldsv_trim_string (pString);
      }
    }
  }
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cm_ldsv_frame_and_send_add_params_to_je
 Input Args    : dm_path_p: Data Model template tree Path
                 pnv_pair_array : Name Value pairs
                 pTransportChannel: Transport Channel
 Output Args   : none
 Description   : This method is used to Frame name value pairs and send into 
                 to JE in commands format.
 Return Values : OF_FAILURE/OF_SUCCESS
*************************************************************************/
int32_t cm_ldsv_frame_and_send_add_params_to_je (char * pInsDMPath,
                                          struct cm_array_of_nv_pair * pnv_pair_array,
                                          void * pTransportChannel, char *dm_path_p,
                                          unsigned char global_trans_b, unsigned char scalar_group_b)
{
  struct cm_result *pResult;
  struct cm_command CommandInfo;
  struct cm_role_info RoleInfo = { "VortiQa", CM_VORTIQA_SUPERUSER_ROLE };
  struct cm_je_config_session *pConfigSess = NULL;

  /* Fill Command Info structure */
  memset (&CommandInfo, 0, sizeof (CommandInfo));

  /* Start config session  */
  pConfigSess =
    (struct cm_je_config_session *) cm_config_session_start (CM_LDSV_MGMT_ENGINE_ID,
                                                    &RoleInfo,
                                                    pTransportChannelJE);
  if (!pConfigSess)
  {
    CM_LDSV_DEBUG_PRINT ("cm_config_session_start failed");
    return OF_FAILURE;
  }
  CM_LDSV_DEBUG_PRINT ("pInsDMPath=%s:scalar_group_b=%d:Cnt=%ld:",pInsDMPath,scalar_group_b,pnv_pair_array->count_ui);
  if ((global_trans_b == TRUE) || (scalar_group_b == FALSE))
  {
    cm_ldsv_fill_command_info (CM_CMD_ADD_PARAMS, pInsDMPath,
                          pnv_pair_array->count_ui, pnv_pair_array->nv_pairs,
                          &CommandInfo);
  }
  else
  {
    cm_ldsv_fill_command_info (CM_CMD_SET_PARAMS, pInsDMPath,
                          pnv_pair_array->count_ui, pnv_pair_array->nv_pairs,
                          &CommandInfo);
    scalar_group_b = FALSE;
  }

  /* Send to JE using ConfigSessionUpdate */
  if (cm_config_session_update_cmd (pConfigSess,
                                 &CommandInfo, &pResult) != OF_SUCCESS)
  {
    CM_LDSV_DEBUG_PRINT (" cm_config_session_update_cmd failed");
    if (pResult)
    {
      UCMFreeUCMResult (pResult);
    }

    if (cm_config_session_end (pConfigSess, CM_CMD_CONFIG_SESSION_REVOKE, &pResult) == OF_FAILURE)
    {
      CM_LDSV_DEBUG_PRINT("Revoke configuration failed.");
      return OF_FAILURE;
    }
    return OF_FAILURE;
  }
  if (global_trans_b == TRUE)
  {
    // load inside table data.
    load_table_in_table_data(pnv_pair_array, pTransportChannel, 
                        dm_path_p, pInsDMPath, pConfigSess);
  }
  if (pResult)
  {
    UCMFreeUCMResult (pResult);
  }
  if ((cm_config_session_end (pConfigSess, CM_CMD_CONFIG_SESSION_COMMIT, 
                            &pResult)) == OF_FAILURE)
  {
    CM_LDSV_DEBUG_PRINT("Failed to close the configuration session.");
    if ((cm_config_session_end (pConfigSess, CM_CMD_CONFIG_SESSION_REVOKE, 
                              &pResult)) == OF_FAILURE)
    {
      CM_LDSV_DEBUG_PRINT("Revoke configuration failed.");
    }
    return OF_FAILURE;
  }
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cm_ldsv_fill_command_info
 Input Args    : 
                iCmd: Command Id(ADD/SET/DELETE...) 
                dm_path_p : Data Model Path
                iCount: Name value pair count
                pNvPair: Name value pairs
 Output Args   : 
                pCmd: New command params
 Description   : This method is used to Fill required information into commands
 Return Values : OF_SUCCESS/OF_FAILURE
 *************************************************************************/
void cm_ldsv_fill_command_info (int32_t iCmd, char * dm_path_p,
                               int32_t iCount, struct cm_nv_pair * pNvPair,
                               struct cm_command * pCmd)
{
  //memset(pCmd, 0, sizeof(struct cm_command));
  pCmd->command_id = iCmd;
  pCmd->dm_path_p = dm_path_p;
  pCmd->nv_pair_array.count_ui = iCount;
  pCmd->nv_pair_array.nv_pairs = pNvPair;
}

/**************************************************************************
 Function Name : cm_ldsv_get_transport_channel
 Input Args    : none
 Output Args   : none
 Description   : This method is used to validate transport channel, if not a 
     valid transport channel create new transport channel with JE
 Return Values : Transport channel
 *************************************************************************/
void *cm_ldsv_get_transport_channel(void)
{
  /* Validate trasport channel */
  if ((pTransportChannelJE != NULL))
  {
    /* return valid trasport channel */
    return pTransportChannelJE;
  }

  /* Create new transport channel */
  pTransportChannelJE =(void *) cm_tnsprt_create_channel (CM_IPPROTO_TCP, 0, 0,
                                                   UCMJE_CONFIGREQUEST_PORT);
  return pTransportChannelJE;
}

/**************************************************************************
 Function Name :cm_ldsv_create_table_stack_node
 Input Args    :dm_path_p - Data model Path
    name_p - element name
    pOpaqueInfo - opaque information for traversing
    uiOpaqueInfoLen - Length of the opaque information
    pKeyNvPair - Name value pair
 Output Args   : none
 Description   : This method is used to create stack and store elements information
     for forward traversing and backward traversing. This is useful to
     get next dm nodes and get next instance of current table node .
 Return Values : Stack node
 *************************************************************************/
cm_ldsv_table_stack_node_t *cm_ldsv_create_table_stack_node (struct cm_dm_node_info *
                                                        pDMNodeAttrib,
                                                        char * dm_path_p,
                                                        char * name_p,
                                                        void *
                                                        pOpaqueInfo,
                                                        uint32_t
                                                        uiOpaqueInfoLen,
                                                        struct cm_array_of_nv_pair *
                                                        pKeyNvPair)
{
  int32_t iRet = OF_SUCCESS;
  int32_t ii = 0;
  cm_ldsv_table_stack_node_t *pNode = NULL;

  if ((pKeyNvPair == NULL) || (pKeyNvPair->count_ui == 0))
    return NULL;

    /* First allocate needed memory */
    pNode =
      (cm_ldsv_table_stack_node_t *) of_calloc (1,
                                             sizeof
                                             (cm_ldsv_table_stack_node_t));
    if (!pNode)
    {
      /*iRet = OF_FAILURE; CID 129*/ 
      return NULL;
    }

    pNode->pDMNodeAttrib = pDMNodeAttrib;
    pNode->dm_path_p = (char *) of_calloc (1, strlen (dm_path_p) + 1);
    if (pNode->dm_path_p == NULL)
    {
      iRet = OF_FAILURE;
    }

    pNode->pElementName = (char *) of_calloc (1, strlen (name_p) + 1);
    if (pNode->pElementName == NULL)
    {
      iRet = OF_FAILURE;
    }

    pNode->pOpaqueInfo =
      (void *) of_calloc (uiOpaqueInfoLen, sizeof (uint32_t));
    if (pNode->pOpaqueInfo == NULL)
    {
      iRet = OF_FAILURE;
    }

    pNode->ActiveInstKeys.nv_pairs = (struct cm_nv_pair *) of_calloc (pKeyNvPair->count_ui, sizeof (struct cm_nv_pair));
    if(pNode->ActiveInstKeys.nv_pairs == NULL)
    {
      CM_LDSV_DEBUG_PRINT("Memory creation failed for nv_pairs.");
      iRet = OF_FAILURE;
    }

   while(ii < pKeyNvPair->count_ui)
   {
      CM_LDSV_DEBUG_PRINT("Name=%s,Val=%s:",pKeyNvPair->nv_pairs[ii].name_p,pKeyNvPair->nv_pairs[ii].value_p);
     iRet = cm_ldsv_fill_nv_pair_info (&(pNode->ActiveInstKeys.nv_pairs[ii]),
                            pKeyNvPair->nv_pairs[ii].name_p,
                            pKeyNvPair->nv_pairs[ii].value_type,
                            pKeyNvPair->nv_pairs[ii].value_length,
                            pKeyNvPair->nv_pairs[ii].value_p);
      ii++;
   }
    /* Now start copying all the data */
    strcpy (pNode->dm_path_p, dm_path_p);
    strcpy (pNode->pElementName, name_p);
    memcpy (pNode->pOpaqueInfo, pOpaqueInfo,
              (sizeof (uint32_t) * uiOpaqueInfoLen));
    pNode->uiOpaqueInfoLen = uiOpaqueInfoLen;
    pNode->ActiveInstKeys.count_ui = pKeyNvPair->count_ui;

      CM_LDSV_DEBUG_PRINT("DMPath=%s,Elm=%s,Cnt=%d:",dm_path_p,name_p,pKeyNvPair->count_ui);
  /* if any error occured, free all the allocated memory */
  if (iRet == OF_FAILURE)
  {
    if (pNode)
    {
      if (pNode->dm_path_p)
      {
        of_free (pNode->dm_path_p);
      }

      if (pNode->pElementName)
      {
        of_free (pNode->pElementName);
      }

      if (pNode->pOpaqueInfo)
      {
        of_free (pNode->pOpaqueInfo);
      }

      if (pNode->ActiveInstKeys.nv_pairs->name_p)
      {
        of_free (pNode->ActiveInstKeys.nv_pairs->name_p);
      }

      if (pNode->ActiveInstKeys.nv_pairs->value_p)
      {
        of_free (pNode->ActiveInstKeys.nv_pairs->value_p);
      }

      of_free (pNode);
    }
    return NULL;
  }
  CM_LDSV_DEBUG_PRINT ("Stack added");
  return pNode;
}

/**************************************************************************
 Function Name :cm_ldsv_update_table_stack_node_key_nv_pair
 Input Args    :pNode - table stack node
    pKeyNvPair - key name value pairs
 Output Args   : none
 Description   : This method is used to update table stack node key information
 Return Values : OF_SUCCESS/OF_FAILURE
 *************************************************************************/
int32_t cm_ldsv_update_table_stack_node_key_nv_pair (cm_ldsv_table_stack_node_t *
                                               pNode, struct cm_array_of_nv_pair *pKeyNvPair)
{
  void *value_p;
  int32_t ii;

  if (pNode && pKeyNvPair)
  {
    /* We need not touch Name */
    /*value_p = (void *) of_calloc (1, pKeyNvPair->nv_pairs->value_length);
    if (unlikely (value_p == NULL))
    {
      return OF_FAILURE;
    }

    if (pNode->ActiveInstKeys.nv_pairs->value_p)
      of_free (pNode->ActiveInstKeys.nv_pairs->value_p); */

    for (ii=0; ii< pKeyNvPair->count_ui; ii++ )
    {
      if (pNode->ActiveInstKeys.nv_pairs[ii].value_p)
        of_free (pNode->ActiveInstKeys.nv_pairs[ii].value_p);
      /*Copy active instace key data */
      value_p = (void *) of_calloc (1, pKeyNvPair->nv_pairs[ii].value_length);
      pNode->ActiveInstKeys.nv_pairs[ii].value_p = value_p;
      pNode->ActiveInstKeys.nv_pairs[ii].value_type = pKeyNvPair->nv_pairs[ii].value_type;
      pNode->ActiveInstKeys.nv_pairs[ii].value_length = pKeyNvPair->nv_pairs[ii].value_length;
      memcpy (pNode->ActiveInstKeys.nv_pairs[ii].value_p, pKeyNvPair->nv_pairs[ii].value_p,
                pKeyNvPair->nv_pairs[ii].value_length);
      CM_LDSV_DEBUG_PRINT("Name=%s,Val=%s:",pKeyNvPair->nv_pairs[ii].name_p,pKeyNvPair->nv_pairs[ii].value_p);
    }

    CM_LDSV_DEBUG_PRINT ("Stack updated");
    return OF_SUCCESS;
  }

  return OF_FAILURE;
}

/**************************************************************************
 Function Name :cm_ldsv_fill_nv_pair_info
 Input Args    :pNvPair - name value pair
    pElementName - Element name
    uiKeyType - instance key type
    uiKeyLen - Key length
    pKey - neme of the key
 Output Args   : none
 Description   : This method is used to fill name value pairs
 Return Values : OF_SUCCESS/OF_FAILURE
 *************************************************************************/
int32_t cm_ldsv_fill_nv_pair_info (struct cm_nv_pair * pNvPair,
                                char * pElementName,
                                uint32_t uiKeyType,
                                uint32_t uiKeyLen, void * pKey)
{

  if (unlikely (!pElementName))
  {
    return OF_FAILURE;
  }

  /* Allocate needed memory first */
  pNvPair->name_length = strlen (pElementName);
  pNvPair->name_p = (char *) of_calloc (1, pNvPair->name_length + 1);
  if (unlikely (pNvPair->name_p == NULL))
  {
    return OF_FAILURE;
  }
  pNvPair->value_p = (void *) of_calloc (1, uiKeyLen+1);
  if (unlikely (pNvPair->value_p == NULL))
  {
    of_free (pNvPair->name_p);
    return OF_FAILURE;
  }

  /* Now copy the input data */
  CM_LDSV_DEBUG_PRINT("Name=%s,Val=%s, ValLen: %d",pElementName, (char*)pKey, uiKeyLen);
  strcpy (pNvPair->name_p, pElementName);
  pNvPair->value_type = uiKeyType;
  pNvPair->value_length = uiKeyLen;
  memcpy (pNvPair->value_p, pKey, uiKeyLen);

  return OF_SUCCESS;
}

/**************************************************************************
 Function Name :cm_ldsv_clean_nv_pair_array
 Input Args    :array_of_nv_pairs_p - Array of name value pairs
    bDeleteContainer - boolean TRUE/FALSE
 Output Args   :none
 Description   :This method is used to clean the memory of run time data
 Return Values :T_SUCESS/OF_FAILURE
 *************************************************************************/
void cm_ldsv_clean_nv_pair_array (struct cm_array_of_nv_pair * array_of_nv_pairs_p,
                                 unsigned char bDeleteContainer)
{
  uint32_t i;

  /*
     Loop through every element of NvPair array, and free the dynamically
     allocated memories 
   */
  if (array_of_nv_pairs_p != NULL)
  {
    for (i = 0; i < array_of_nv_pairs_p->count_ui; i++)
    {
      if (array_of_nv_pairs_p->nv_pairs != NULL)
      {
      if (array_of_nv_pairs_p->nv_pairs[i].name_p)
      {
        of_free (array_of_nv_pairs_p->nv_pairs[i].name_p);
      }
      if (array_of_nv_pairs_p->nv_pairs[i].value_p)
      {
        of_free (array_of_nv_pairs_p->nv_pairs[i].value_p);
      }
      }
    }

    /* Now free the Array itself */
    if ((array_of_nv_pairs_p->count_ui > 0) || (array_of_nv_pairs_p->nv_pairs))
    {
      of_free (array_of_nv_pairs_p->nv_pairs);
      array_of_nv_pairs_p->nv_pairs = NULL;
      array_of_nv_pairs_p->count_ui = 0;
    }
    /* Now just reset the container structure. Do not free this */
    if (bDeleteContainer == TRUE)
    {
      of_free (array_of_nv_pairs_p);
      array_of_nv_pairs_p = NULL;
    }
  }
}

/**************************************************************************
 Function Name : cm_ldsv_clean_array_of_nv_pair_array
 Input Args    : array_of_nv_pairs_p: Array of Name Value pairs.
                 uiRecordCount : No of Records present in the array.
 Output Args   : none
 Description   : This method is used to clean name value pairs.
 Return Values : OF_FAILURE/OF_SUCCESS
*************************************************************************/
void cm_ldsv_clean_array_of_nv_pair_array (struct cm_array_of_nv_pair *
                                      array_of_nv_pairs_p, uint32_t uiRecordCount)
{
  uint32_t i;

  /*
     Loop through every element of NvPair array, and free the dynamically
     allocated memories 
   */
  if ((array_of_nv_pairs_p != NULL))
  {
    for (i = 0; i < uiRecordCount; i++)
    {
      cm_ldsv_clean_nv_pair_array (&(array_of_nv_pairs_p[i]), 0);
    }
  }
  of_free (array_of_nv_pairs_p);
  array_of_nv_pairs_p = NULL;
}

/**************************************************************************
 Function Name :cm_ldsv_find_key_nv_pair
 Input Args    :pDMNode - Data Model Attributes
                pArrayOfKeys - Array of keys
                pTransportChannel - Transport channel pointer
 Output Args   : Array of name value pairs.
 Description   :This method is used to fill array of name value pairs for the given node Attributes
 Return Values :struct cm_nv_pair - Corresponding name value pair
 *************************************************************************/
struct cm_array_of_nv_pair *cm_ldsv_find_key_nv_pair (struct cm_dm_node_info * pDMNode,
                                    struct cm_array_of_nv_pair * pArrayOfKeys,
                                    void * pTransportChannel)
{
  uint32_t i = 0;
  uint32_t iCount = 0;
  uint32_t ii = 0;
  int32_t iRet;
  struct cm_array_of_structs *pChildInfoArr = NULL;
  struct cm_array_of_nv_pair *pTmpArrayOfKeys = NULL;
  struct cm_role_info RoleInfo = { "VortiQa", CM_VORTIQA_SUPERUSER_ROLE };
  struct cm_dm_node_info *pKeyChild=NULL;

  if ((!pDMNode) || (!pArrayOfKeys))
  {
    CM_LDSV_DEBUG_PRINT ("Invalid input");
    return NULL;
  }

  cm_dm_get_key_childs (pTransportChannel, pDMNode->dm_path_p,
                       CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, &pChildInfoArr);
  if (!pChildInfoArr)
  {
    CM_LDSV_DEBUG_PRINT (" Get Key Child Info failed");
    return NULL;
  }
  pTmpArrayOfKeys =
    (struct cm_array_of_nv_pair *) of_calloc (1, sizeof (struct cm_array_of_nv_pair));
  if (!pTmpArrayOfKeys)
  {
    CM_LDSV_DEBUG_PRINT (" memory allocation failed"); //CID 135
    return NULL;
  }
  pTmpArrayOfKeys->count_ui = pChildInfoArr->count_ui;
  pTmpArrayOfKeys->nv_pairs =
      (struct cm_nv_pair *) of_calloc (pChildInfoArr->count_ui, sizeof (struct cm_nv_pair));
  if (pTmpArrayOfKeys->nv_pairs == NULL)
  {
    of_free (pTmpArrayOfKeys);
    return NULL;
  }
  for (ii=0; ii < pChildInfoArr->count_ui; ii++)
  {
    pKeyChild=(struct cm_dm_node_info *)(pChildInfoArr->struct_arr_p) + ii;
    for (i = 0; i < pArrayOfKeys->count_ui; i++)
    {
      if (!strcmp (pKeyChild->name_p, pArrayOfKeys->nv_pairs[i].name_p))
      {
        CM_LDSV_DEBUG_PRINT ("ELement name Matching found: %s :%s, Val=%s",
                              pKeyChild->name_p,
                              pArrayOfKeys->nv_pairs[i].name_p, pArrayOfKeys->nv_pairs[i].value_p);
        iRet = cm_ldsv_fill_nv_pair_info (&(pTmpArrayOfKeys->nv_pairs[iCount]),
                                     pArrayOfKeys->nv_pairs[i].name_p,
                                     pArrayOfKeys->nv_pairs[i].value_type,
                                     pArrayOfKeys->nv_pairs[i].value_length,
                                     pArrayOfKeys->nv_pairs[i].value_p);
        if (unlikely (iRet == OF_FAILURE))
        {
          cm_ldsv_clean_nv_pair_array (pTmpArrayOfKeys, FALSE);
          return NULL;
        }
        iCount++;
      }
    }
  }
  if (iCount == 0)
  {
    cm_ldsv_clean_nv_pair_array (pTmpArrayOfKeys, TRUE);
    return NULL;
  }
  return pTmpArrayOfKeys;
  //return NULL;
}

/**************************************************************************
 Function Name :cm_ldsv_collect_keys_from_table_stack
 Input Args    :pTableStackDll - double linked list of table stack elements
                dm_path_pArrayOfKeys- Array of Name value pairs
 Output Args   : Array of Name value pairs
 Description   : Collect array of key and values from table stack node.
 Return Values :struct cm_array_of_nv_pair - Array of key name value pairs
 *************************************************************************/
struct cm_array_of_nv_pair *cm_ldsv_collect_keys_from_table_stack (UCMDllQ_t* pTableStackDll,
                                           struct cm_array_of_nv_pair *dm_path_pArrayOfKeys)
{
  int32_t i = 0, j=0;
  int32_t iCount = 0;
  int32_t iRet;
  struct cm_array_of_nv_pair *array_of_key_nv_pairs_p = NULL;
  uint32_t uiDMPathCount = 0;
  uint32_t uiInstCount = 0;
  unsigned char bNodeAlreadyFound = FALSE;

  UCMDllQNode_t *pNode;
  cm_ldsv_table_stack_node_t *pTmpStackNode;
  cm_ldsv_table_stack_node_t *pTableStackNode;

  /* allocate memory */
  array_of_key_nv_pairs_p =
    (struct cm_array_of_nv_pair *) of_calloc (1, sizeof (struct cm_array_of_nv_pair));
  if (unlikely (!array_of_key_nv_pairs_p))
  {
    return NULL;
  }

  array_of_key_nv_pairs_p->count_ui = 0;
  array_of_key_nv_pairs_p->nv_pairs = NULL;

  if (CM_DLLQ_COUNT (pTableStackDll) != 0)
  {
    /* find no of nodes present in TableNodeStack */
    /* collect the keys of parent table nodes */
    CM_DLLQ_SCAN (pTableStackDll, pNode, UCMDllQNode_t *)
    {
      pTmpStackNode =
        CM_DLLQ_LIST_MEMBER (pNode, cm_ldsv_table_stack_node_t *, ListNode);
       // pTableStackNode->pDMNodeAttrib->dm_path_p
      if (pTmpStackNode == NULL)
        return array_of_key_nv_pairs_p;
      uiInstCount = uiInstCount + pTmpStackNode->ActiveInstKeys.count_ui;
    }
    array_of_key_nv_pairs_p->count_ui = uiInstCount;
    /* find no of nodes present in TableNodeStack */
    //array_of_key_nv_pairs_p->count_ui = CM_DLLQ_COUNT (pTableStackDll);

    if ((dm_path_pArrayOfKeys != NULL) && ((dm_path_pArrayOfKeys->count_ui) > 0))
    {
      /* These are the nv_pairs present in DM  Path */
      uiDMPathCount = dm_path_pArrayOfKeys->count_ui;
    }
    /* Allocate memory */
    array_of_key_nv_pairs_p->nv_pairs =
      (struct cm_nv_pair *) of_calloc (array_of_key_nv_pairs_p->count_ui +
                                uiDMPathCount, sizeof (struct cm_nv_pair));
    if (array_of_key_nv_pairs_p->nv_pairs == NULL)
    {
      /* We cant handle save operation any more. Quit from here */
      of_free (array_of_key_nv_pairs_p);
      return NULL;
    }

    for (i = 0; i < uiDMPathCount; i++)
    {
      CM_DLLQ_SCAN (pTableStackDll, pNode, UCMDllQNode_t *)
      {
       pTmpStackNode =
        CM_DLLQ_LIST_MEMBER (pNode, cm_ldsv_table_stack_node_t *, ListNode);
       // pTableStackNode->pDMNodeAttrib->dm_path_p
       if (pTmpStackNode != NULL)
       {
         for (j = 0; j < pTmpStackNode->ActiveInstKeys.count_ui; j++)
         {
           if (strcmp(dm_path_pArrayOfKeys->nv_pairs[i].name_p, pTmpStackNode->ActiveInstKeys.nv_pairs[j].name_p) == 0)
           {
             bNodeAlreadyFound = TRUE;
             break;
           }
         }
       }
        break;
      }
      if (bNodeAlreadyFound != TRUE)
      {
        CM_LDSV_DEBUG_PRINT ("Name=%s,Value=%s:",dm_path_pArrayOfKeys->nv_pairs[i].name_p,dm_path_pArrayOfKeys->nv_pairs[i].value_p);
        iRet = cm_ldsv_fill_nv_pair_info (&(array_of_key_nv_pairs_p->nv_pairs[iCount]),
                                     dm_path_pArrayOfKeys->nv_pairs[i].name_p,
                                     dm_path_pArrayOfKeys->nv_pairs[i].value_type,
                                     dm_path_pArrayOfKeys->nv_pairs[i].value_length,
                                     dm_path_pArrayOfKeys->nv_pairs[i].value_p);

        if (unlikely (iRet == OF_FAILURE))
        {
          cm_ldsv_clean_nv_pair_array (array_of_key_nv_pairs_p, FALSE);
          return NULL;
        }
        iCount = iCount+1;
      }
    }
    /* Now collect the keys of parent table nodes */
    CM_DLLQ_SCAN (pTableStackDll, pNode, UCMDllQNode_t *)
    {
      pTableStackNode =
        CM_DLLQ_LIST_MEMBER (pNode, cm_ldsv_table_stack_node_t *, ListNode);

      for (i = 0; i < pTableStackNode->ActiveInstKeys.count_ui; i++)
      {
        /* Prepare Keys Array entry */
        CM_LDSV_DEBUG_PRINT ("Name=%s,Value=%s:",pTableStackNode->ActiveInstKeys.nv_pairs[i].name_p,pTableStackNode->ActiveInstKeys.nv_pairs[i].value_p);
        iRet = cm_ldsv_fill_nv_pair_info (&(array_of_key_nv_pairs_p->nv_pairs[iCount]),
                                     pTableStackNode->ActiveInstKeys.nv_pairs[i].name_p,
                                     pTableStackNode->ActiveInstKeys.nv_pairs[i].value_type,
                                     pTableStackNode->ActiveInstKeys.nv_pairs[i].value_length,
                                     pTableStackNode->ActiveInstKeys.nv_pairs[i].value_p);
        if (unlikely (iRet == OF_FAILURE))
        {
          cm_ldsv_clean_nv_pair_array (array_of_key_nv_pairs_p, FALSE);
          return NULL;
        }
        iCount = iCount + 1;
      }
    }
    array_of_key_nv_pairs_p->count_ui = iCount;
  }
  else if ((dm_path_pArrayOfKeys != NULL) && 
               ((dm_path_pArrayOfKeys->count_ui) > 0))
  {
    /* These are the nv_pairs present in DM  Path */
    uiDMPathCount = dm_path_pArrayOfKeys->count_ui;
    /* Allocate memory */
    array_of_key_nv_pairs_p->nv_pairs =
      (struct cm_nv_pair *) of_calloc (array_of_key_nv_pairs_p->count_ui +
                                uiDMPathCount, sizeof (struct cm_nv_pair));
    if (array_of_key_nv_pairs_p->nv_pairs == NULL)
    {
      /* We cant handle save operation any more. Quit from here */
      of_free (array_of_key_nv_pairs_p);
      return NULL;
    }
    for (i = 0; i < uiDMPathCount; i++)
    {
      CM_LDSV_DEBUG_PRINT ("Name=%s,Value=%s:",dm_path_pArrayOfKeys->nv_pairs[i].name_p,dm_path_pArrayOfKeys->nv_pairs[i].value_p);
      iRet = cm_ldsv_fill_nv_pair_info (&(array_of_key_nv_pairs_p->nv_pairs[i]),
                                     dm_path_pArrayOfKeys->nv_pairs[i].name_p,
                                     dm_path_pArrayOfKeys->nv_pairs[i].value_type,
                                     dm_path_pArrayOfKeys->nv_pairs[i].value_length,
                                     dm_path_pArrayOfKeys->nv_pairs[i].value_p);
      if (unlikely (iRet == OF_FAILURE))
      {
        cm_ldsv_clean_nv_pair_array (array_of_key_nv_pairs_p, FALSE);
        return NULL;
      }
      array_of_key_nv_pairs_p->count_ui = i + 1;
    }
  }
  return array_of_key_nv_pairs_p;
}

/**************************************************************************
 Function Name :cm_ldsv_prepare_array_of_keys
 Input Args    :pTableStackDll - list of table stack elements
 Output Args   : Array of name value pairs
 Description   : this method is used to prepare array of name value pairs
 Return Values :struct cm_array_of_nv_pair - Array of key name value pairs
 *************************************************************************/
struct cm_array_of_nv_pair *cm_ldsv_prepare_array_of_keys (cm_ldsv_table_stack_node_t *pTableStackNode)
{
  int32_t iRet;
  uint32_t ii; 
  struct cm_array_of_nv_pair *array_of_key_nv_pairs_p = NULL;

  /* allocate memory */
  array_of_key_nv_pairs_p =
    (struct cm_array_of_nv_pair *) of_calloc (1, sizeof (struct cm_array_of_nv_pair));
  if (unlikely (!array_of_key_nv_pairs_p))
  {
    return NULL;
  }

  array_of_key_nv_pairs_p->count_ui =  pTableStackNode->ActiveInstKeys.count_ui;
  array_of_key_nv_pairs_p->nv_pairs = NULL;

  /* Allocate memory */
   array_of_key_nv_pairs_p->nv_pairs =
      (struct cm_nv_pair *) of_calloc (array_of_key_nv_pairs_p->count_ui,  sizeof (struct cm_nv_pair));
    if (array_of_key_nv_pairs_p->nv_pairs == NULL)
    {
      /* We cant handle operation any more. Quit from here */
      of_free (array_of_key_nv_pairs_p);
      return NULL;
    }

   /* Prepare Keys Array entry */
   CM_LDSV_DEBUG_PRINT("stack node count: %d", pTableStackNode->ActiveInstKeys.count_ui);
   for(ii=0; ii < pTableStackNode->ActiveInstKeys.count_ui; ii++)
   {
      CM_LDSV_DEBUG_PRINT("Name=%s,Val=%s:",pTableStackNode->ActiveInstKeys.nv_pairs[ii].name_p,pTableStackNode->ActiveInstKeys.nv_pairs[ii].value_p);
      iRet = cm_ldsv_fill_nv_pair_info (&(array_of_key_nv_pairs_p->nv_pairs[ii]),
                                  pTableStackNode->ActiveInstKeys.nv_pairs[ii].name_p,
                                  pTableStackNode->ActiveInstKeys.nv_pairs[ii].value_type,
                                  pTableStackNode->ActiveInstKeys.nv_pairs[ii].value_length,
                                  pTableStackNode->ActiveInstKeys.nv_pairs[ii].value_p);
      if (unlikely (iRet == OF_FAILURE))
      {
        cm_ldsv_clean_nv_pair_array (array_of_key_nv_pairs_p, TRUE);
        return NULL;
      }
   }
  return array_of_key_nv_pairs_p;
}

/**************************************************************************
 Function Name : cm_ldsv_copy_array_of_nv_pairs
 Input Args    :
     array_of_nv_pairs_p : Array of name value pairs
 Output Args   : Array of name value pairs
 Description   : This method is used to copy the array of name values pairs
                 from array_of_nv_pairs_p into pNewNvPairArr structure.
 Return Values : Array of name value pairs
 *************************************************************************/
struct cm_array_of_nv_pair *cm_ldsv_copy_array_of_nv_pairs (struct cm_array_of_nv_pair *
                                                 array_of_nv_pairs_p)
{
  int32_t i;
  struct cm_array_of_nv_pair *pNewNvPairArr;

  pNewNvPairArr =
    (struct cm_array_of_nv_pair *) of_calloc (1, sizeof (struct cm_array_of_nv_pair));
  if (pNewNvPairArr)
  {
    pNewNvPairArr->count_ui = array_of_nv_pairs_p->count_ui;
    pNewNvPairArr->nv_pairs =
      (struct cm_nv_pair *) of_calloc (pNewNvPairArr->count_ui, sizeof (struct cm_nv_pair));
    if (pNewNvPairArr->nv_pairs == NULL)
    {
      of_free (pNewNvPairArr);
      return NULL;
    }
    else
    {
      for (i = 0; i < pNewNvPairArr->count_ui; i++)
      {
        pNewNvPairArr->nv_pairs[i].name_p =
          (char *) of_calloc (1,
                                strlen (array_of_nv_pairs_p->nv_pairs[i].name_p) +
                                1);
        if (pNewNvPairArr->nv_pairs[i].name_p == NULL)
        {
          cm_ldsv_clean_nv_pair_array (pNewNvPairArr, TRUE);
          return NULL;
        }
        pNewNvPairArr->nv_pairs[i].value_p =
          (char *) of_calloc (1, array_of_nv_pairs_p->nv_pairs[i].value_length);
        if (pNewNvPairArr->nv_pairs[i].value_p == NULL)
        {
          cm_ldsv_clean_nv_pair_array (pNewNvPairArr, TRUE);
          return NULL;
        }

        strcpy (pNewNvPairArr->nv_pairs[i].name_p,
                  array_of_nv_pairs_p->nv_pairs[i].name_p);
        memcpy (pNewNvPairArr->nv_pairs[i].value_p,
                  array_of_nv_pairs_p->nv_pairs[i].value_p,
                  array_of_nv_pairs_p->nv_pairs[i].value_length);
        pNewNvPairArr->nv_pairs[i].value_length =
          array_of_nv_pairs_p->nv_pairs[i].value_length;
        pNewNvPairArr->nv_pairs[i].value_type =
          array_of_nv_pairs_p->nv_pairs[i].value_type;
      }
    }
  }
  return pNewNvPairArr;
}

/**************************************************************************
 Function Name : cm_ldsv_write_to_file
 Input Args    : pDMNodeAttrib - Data model Attributes
                 pMembers - Array of name value pairs
                 pTransportChannel - Transport Channel
 Output Args   : none 
 Description   : This method is used to write data into a persistent store
 Return Values : OF_FAILURE/OF_SUCCESS
 *************************************************************************/
int32_t cm_ldsv_write_to_file (struct cm_array_of_nv_pair * pMembers,
                             struct cm_hash_table *pDictionaryTable,
                             char* pFullPath, struct cm_dm_node_info *pDMNodeAttrib, 
                             void * pTransportChannel)
{
  if (!pMembers)
  {
    CM_LDSV_DEBUG_PRINT ("Invalid Inputs");
    return OF_FAILURE;
  }
  cm_ldsv_write_array_of_nv_pairs (pMembers, pDictionaryTable, pFullPath, pDMNodeAttrib, pTransportChannel);
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cm_ldsv_write_array_of_nv_pairs
 Input Args    : pKeys - Array of name value pairs
                 pDMNodeAttrib - Data Model Attributes
                 pTransportChannel - Transport Channel
 Output Args   : none 
 Description   : This method is used to store data in persistent store
 Return Values : OF_FAILURE/OF_SUCCESS
 *************************************************************************/
int32_t cm_ldsv_write_array_of_nv_pairs (struct cm_array_of_nv_pair * pKeys,
                                     struct cm_hash_table *pDictionaryTable,
                                     char* pFullPath, struct cm_dm_node_info *pDMNodeAttrib, 
                                     void * pTransportChannel)
{
  int32_t ii, ij;
  char* pHashValue;
  int32_t iDestFd = -1;
  int32_t iRet;
  char aBuffer[LDSV_XML_READ_LINE];
  char *pBuffer;
  struct cm_role_info RoleInfo = { "VortiQa", CM_VORTIQA_SUPERUSER_ROLE };
  struct cm_dm_node_info *pNodeInfo = NULL, *pTmpNodeInfo;
  struct cm_array_of_structs *pChildInfoArr = NULL;

  if ((pKeys == NULL) || (pKeys->count_ui == 0))
  {
    CM_LDSV_DEBUG_PRINT ("Invalid data.Nothing to write into file");
    return OF_FAILURE;
  }

  /* Open file if found, otherwise create a new file */
  iDestFd = cm_file_open (iDestFd, pFullPath, UCMFILE_MODE_WRITE_APPEND);
  if(iDestFd < 0)  //CID 164
  {
    if (errno == ENOENT)
    {
      iDestFd = cm_file_create (iDestFd, pFullPath);
      CM_LDSV_DEBUG_PRINT("File Fd:%d", iDestFd);
      if (iDestFd == -1)
      {
        CM_LDSV_DEBUG_PRINT ("Failed to create new file.");
        CM_LDSV_DEBUG_PRINT ("Error No is: %d", errno);
        return OF_FAILURE;
      }
    }
    else
    {
      CM_LDSV_DEBUG_PRINT ("Failed to open file.");
      CM_LDSV_DEBUG_PRINT ("Error No is: %d", errno);
      return OF_FAILURE;
    }
  }
  memset (aBuffer, 0, sizeof (aBuffer));
  sprintf (aBuffer, "\n\t\t<row>");
  cm_file_write (iDestFd, iDestFd, aBuffer, strlen (aBuffer));

  if (cm_dm_get_node_and_child_info (pTransportChannel,
                                pDMNodeAttrib->dm_path_p,
                                 CM_LDSV_MGMT_ENGINE_ID,
                                 &RoleInfo, &pNodeInfo,
                                 &pChildInfoArr) != OF_SUCCESS)
  {
    CM_LDSV_DEBUG_PRINT ("Failed to get child nodes.");
    return OF_FAILURE;
  }

  for (ii = 0; ii < pKeys->count_ui; ii++)
  {
    if ((pKeys->nv_pairs[ii].value_p != NULL) && (strlen(pKeys->nv_pairs[ii].value_p)>0))
    {
      for (ij = 0; ij < pChildInfoArr->count_ui; ij++)
      {
        pTmpNodeInfo = (struct cm_dm_node_info *) (pChildInfoArr->struct_arr_p) + ij;
        if (pTmpNodeInfo->element_attrib.non_config_leaf_node_b == FALSE)
        {
          if (strcmp(pKeys->nv_pairs[ii].name_p, pTmpNodeInfo->name_p)==0)
          {
            pHashValue =
              (char *) cm_hash_lookup (pDictionaryTable, pKeys->nv_pairs[ii].name_p);
            if (pHashValue != NULL)
            {
              pBuffer = (char*) of_calloc(1, (pKeys->nv_pairs[ii].value_length + strlen(pHashValue) + LDSV_XML_WRITE_BUFFER));
              sprintf (pBuffer, "\n\t\t\t<a%s>%s</a%s>", pHashValue,
                         (char*)pKeys->nv_pairs[ii].value_p, pHashValue);
              cm_file_write (iDestFd, iDestFd, pBuffer, strlen(pBuffer));
              of_free(pBuffer);
            }
          }
        }
      } //Inner For loop close
    }
  } //Outer for loop close

  memset (aBuffer, 0, sizeof (aBuffer));
  strcat (aBuffer, "\n\t\t</row>");
  cm_file_write (iDestFd, iDestFd, aBuffer, strlen (aBuffer));

  iRet = cm_file_close (iDestFd, iDestFd);
  if (iRet == -1)
  {
    CM_LDSV_DEBUG_PRINT (" Failed to close the file.");
    return OF_FAILURE;
  }
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cm_ldsv_search_file
 Input Args    : name_p: name of the file to search
                 pDirPath : Directory Path
 Output Args   : none
 Description   : This method is used to find the file in the given directory path.
 Return Values : OF_SUCCESS/OF_FAILURE
 **************************************************************************/

int32_t cm_ldsv_search_file (char * pFileName, char * pDirPath)
{
  struct dirent *pEntry;
  DIR *pDir;
  char aBuffer[LDSV_XML_READ_LINE];
  int32_t iRetVal = OF_SUCCESS;

  pDir = opendir (pDirPath);
  if (pDir == NULL)
  {
    CM_LDSV_DEBUG_PRINT ("Error loading: directory not found ");
    memset (aBuffer, 0, sizeof(aBuffer));
    sprintf (aBuffer, "mkdir -p %s", pDirPath);
    //CM_LDSV_DEBUG_PRINT ("Calling %s", aBuffer);
    iRetVal = system (aBuffer);
    if (iRetVal != OF_SUCCESS)
    {
      CM_LDSV_DEBUG_PRINT("Failed to create directory.");
    }
    return OF_FAILURE;
  }

  while ((pEntry = readdir (pDir)) != NULL)
  {
    // skip . and .. and all files starting with .
    if (pEntry->d_name[0] == '.')
      continue;
    if (pEntry->d_type != DT_DIR)
    {
      if (strcmp (pEntry->d_name, pFileName) == 0)
      {
        closedir (pDir);
        return OF_SUCCESS;
      }
    }
  }
  closedir (pDir);
  return OF_FAILURE;
}

/**************************************************************************
 Function Name : cm_ldsv_find_data_in_file
 Input Args    : pFileName : File Name
 Output Args   : none
 Description   : This method is used to Find the data in the given file.
 Return Values : OF_FAILURE/OF_SUCCESS
*************************************************************************/
int32_t cm_ldsv_find_data_in_file (char * pFileName)
{
  char *pString = NULL, *pTempString = NULL;
  char aReadLine[LDSV_XML_READ_LINE];
  FILE *pFile;

  if ((pFile = fopen (pFileName, "r")) == NULL)
  {
    CM_LDSV_DEBUG_PRINT ("File Not Found.");
    CM_LDSV_DEBUG_PRINT ("ERROR CODE :%d", errno);
    return OF_FAILURE;
  }
  memset (aReadLine, 0, sizeof(aReadLine));
  /* get one line from the file */
  pString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);
  pTempString = cm_ldsv_trim_string (pString);

  while (strcmp (pTempString, "<row>") != 0)
  {
    memset (aReadLine, 0, sizeof(aReadLine));
    /* get one line from the file */
    pString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);
    if (pString == NULL)
    {
      // end of file
      fclose (pFile);
      return OF_FAILURE;
    }
    pTempString = cm_ldsv_trim_string (pString);
  }
  fclose (pFile);
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cm_ldsv_create_backup
 Input Args    : pFileName: name of the file to create a back up
                 pDirPath : Directory Path
 Output Args   :
 Description   : This method is used to copy the contents from one file to another
     file for future use/reference.
 Return Values : OF_SUCCESS/OF_FAILURE
 **************************************************************************/

int32_t cm_ldsv_create_backup (char * pFileName, char * pDirPath)
{
  int32_t uiLength;
  int32_t iRet;
  unsigned char *pBkpFile;

  uiLength = strlen (pFileName);
  if (uiLength < 5)
  {
    return OF_FAILURE;           /* Minimum a.xml 5 chars */
  }
  pBkpFile = (char*) of_calloc(1, uiLength+1);
  if (pBkpFile == NULL)
  {
    CM_LDSV_DEBUG_PRINT("Memory allocation failed for pBkpFile");
    return OF_FAILURE;
  }
  if ((pFileName[uiLength - 4] != '.')
      || strcmp (pFileName + uiLength - 3, "xml"))
  {
    of_free(pBkpFile);
    return OF_FAILURE;
  }
  /* Copy Till The '.' */
  strncpy (pBkpFile, pFileName, uiLength - 3);

  /* Copy bkp as extention */
  strcpy (pBkpFile + uiLength - 3, "bkp");

  CM_LDSV_DEBUG_PRINT ("pFileName= %s",pFileName);
  iRet = cmldsv_copy_file (pFileName, pBkpFile, pDirPath);
  of_free(pBkpFile);
  return iRet;
}

/**************************************************************************
 Function Name : cmldsv_copy_file
 Input Args    : pSrcFile : source file
                 pDestFile : destination file is used to copy the contents 
                 from source file
                 pDirPath : Directory Path
 Output Args   :
 Description   : This method is used to copy the contents from one file to another
                 file for future use/reference.
 Return Values : OF_SUCCESS/OF_FAILURE
 **************************************************************************/

int32_t cmldsv_copy_file (unsigned char * pSrcFile, unsigned char * pDestFile,
                          char * pDirPath)
{
  int32_t iDestFsId, iSrcFsId, iSrcFd, iDestFd, iBytes, iRetVal = OF_SUCCESS;
  char *pBuffer;

  iSrcFsId = 0;
  iDestFsId = 0;
  iRetVal = chdir (pDirPath);
  if (iRetVal != OF_SUCCESS)
  {
    CM_LDSV_DEBUG_PRINT("Failed to change the directory.So, Unable to proceed further");
    return OF_FAILURE;
  }
  iSrcFd = cm_file_open (iSrcFsId, pSrcFile, UCMFILE_MODE_READ);
  if (iSrcFd < 0)
  {
    CM_LDSV_DEBUG_PRINT ("Failed to open source file.");
    return OF_FAILURE;
  }

  iDestFd = cm_file_open (iDestFsId, pDestFile, UCMFILE_MODE_WRITE);
  if (iDestFd < 0)
  {
    CM_LDSV_DEBUG_PRINT ("Destination file not found, creating new file");
    iDestFd = cm_file_create (iDestFsId, pDestFile);
    if (iDestFd < 0)
    {
      CM_LDSV_DEBUG_PRINT ("Failed to create new Destination file");
      cm_file_close (iSrcFsId, iSrcFd);
      return OF_FAILURE;
    }
  }
  pBuffer = (char*) of_calloc(1, LDSV_COPY_FILE_BUFFER_SIZE + 1);
  if (pBuffer == NULL)
  {
    CM_LDSV_DEBUG_PRINT("Memory allocation failed.");
    cm_file_close(iSrcFsId, iSrcFsId);
    return OF_FAILURE;
  }
  memset (pBuffer, 0, sizeof(pBuffer));
  while ((iBytes = cm_file_read (iSrcFsId, iSrcFd, pBuffer, LDSV_COPY_FILE_BUFFER_SIZE)) > 0)
  {
    if (cm_file_write (iDestFsId, iDestFd, pBuffer, iBytes) != iBytes)
    {
      CM_LDSV_DEBUG_PRINT ("Unable To Write To File!");
      iRetVal = OF_FAILURE;
      break;
    }
    memset (pBuffer, 0, sizeof(pBuffer));
  }
  CM_LDSV_DEBUG_PRINT (" Contents copied successfully");
  cm_file_close (iDestFsId, iDestFd);
  cm_file_close (iSrcFsId, iSrcFd);
  cm_file_remove (iSrcFd, pSrcFile);
  of_free(pBuffer);
  return iRetVal;
}

/**************************************************************************
 * Function Name :cm_ldsv_validate_file_name
 * Input Args    :
 *                pFileName: file name to validate
 * Output Args   : none
 * Description   : This method is used to validate file name
 * Return Values : If file name was valid returns OF_SUCCESS, otherwise 
 *        returns OF_FAILURE
 **************************************************************************/

int32_t cm_ldsv_validate_file_name (unsigned char * pFileName)
{
  uint32_t uiLength, count_ui;
  if (pFileName == NULL)
  {
    CM_LDSV_DEBUG_PRINT ("File Name should not be null.");
    return OF_FAILURE;
  }
  uiLength = strlen (pFileName);

  if (uiLength < 5)
  {
    CM_LDSV_DEBUG_PRINT ("Invalid file name ");
    return OF_FAILURE;           /* Minimum 5 chars like a.xml */
  }
  if (strcmp (pFileName + uiLength - 3, "xml"))
  {
    CM_LDSV_DEBUG_PRINT ("Invalid file name ");
    return OF_FAILURE;
  }
  /* Allow a-z A-Z 0-9 and Underscore( _ ) only */
  for (count_ui = 0; count_ui < uiLength - 4; count_ui++)
  {
    if (!isalnum (pFileName[count_ui]) && pFileName[count_ui] != '_')
    {
      CM_LDSV_DEBUG_PRINT ("Invalid file name ");
      return OF_FAILURE;
    }
  }
  CM_LDSV_DEBUG_PRINT (" File validation succedeed ");
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cm_ldsv_cleanup_table_stack_node
 Input Args    : pTableStackNode : Stack node
 Output Args   : none
 Description   : This method is used to free stack node
 Return Values : none 
 *************************************************************************/
void cm_ldsv_cleanup_table_stack_node (cm_ldsv_table_stack_node_t *
                                      pTableStackNode)
{
  
  uint32_t ii;

  if ((pTableStackNode != NULL) && (pTableStackNode->ActiveInstKeys.count_ui)>0)
  {
    for(ii=0; ii < pTableStackNode->ActiveInstKeys.count_ui; ii++)
    {
      CM_LDSV_DEBUG_PRINT("Name=%s,Val=%s:",pTableStackNode->ActiveInstKeys.nv_pairs[ii].name_p,pTableStackNode->ActiveInstKeys.nv_pairs[ii].value_p);
      if (pTableStackNode->ActiveInstKeys.nv_pairs[ii].name_p)
      {
        of_free (pTableStackNode->ActiveInstKeys.nv_pairs[ii].name_p);
      }
      if (pTableStackNode->ActiveInstKeys.nv_pairs[ii].value_p)
      {
        of_free (pTableStackNode->ActiveInstKeys.nv_pairs[ii].value_p);
      }
    }

    CM_LDSV_DEBUG_PRINT("DMPath=%s,Elm=%s:",pTableStackNode->dm_path_p,pTableStackNode->pElementName);
    if ((pTableStackNode->dm_path_p) && (strlen(pTableStackNode->dm_path_p)>0))
    {
      of_free (pTableStackNode->dm_path_p);
      pTableStackNode->dm_path_p = NULL;
    }
    if ((pTableStackNode->pElementName) && (strlen(pTableStackNode->pElementName)>0))
    {
      of_free (pTableStackNode->pElementName);
      pTableStackNode->pElementName = NULL;
    }
    if (pTableStackNode->pOpaqueInfo)
    {
      of_free (pTableStackNode->pOpaqueInfo);
      pTableStackNode->pOpaqueInfo = NULL;
    }
    cm_ldsv_clean_dm_node_attrib_info (pTableStackNode->pDMNodeAttrib);
    pTableStackNode->pDMNodeAttrib = NULL;
    /* Now free the Array itself */
    if ((pTableStackNode->ActiveInstKeys.count_ui > 0) || (pTableStackNode->ActiveInstKeys.nv_pairs))
    {
      of_free (pTableStackNode->ActiveInstKeys.nv_pairs);
      pTableStackNode->ActiveInstKeys.nv_pairs = NULL;
      pTableStackNode->ActiveInstKeys.count_ui=0;
    }

    of_free (pTableStackNode);
  }
}

/**************************************************************************
 * Function Name :cm_ldsv_write_header
 * Input Args    :pFullPath - absolute file path
                  pFileName : File Name
                  pDirPath - Directory path
 *                pMNodeAttrib: Data model Attributes
 * Output Args   : none
 * Description   : This method is used to Generate header part for storing
 *                 it in ldsv file.
 * Return Values : OF_FAILURE/OF_SUCCESS
**************************************************************************/
int32_t cm_ldsv_write_header (char* pFileName, char* pFullPath,
                             char* pDirPath, char* pDMInstancePath,
                             struct cm_dm_node_info *pDMNodeAttrib)

{
  int32_t iDestFd = -1;
  int32_t iVersion = 1;
  int32_t iRet;
  char aBuffer[LDSV_BUFFER_SIZE];
  
  if ((pFileName == NULL) || (pFullPath == NULL))
  {
    CM_LDSV_DEBUG_PRINT("File name is null");
    return OF_FAILURE;
  }
  if (cm_ldsv_search_file (pFileName, pDirPath) == OF_SUCCESS)
  {
    //file has header information
    if (cm_ldsv_find_data_in_file (pFullPath) == OF_SUCCESS)
    {
      cm_ldsv_create_backup (pFileName, pDirPath);
    }
    else
    {
      return OF_SUCCESS;
    }
  }

  iDestFd = cm_file_open (iDestFd, pFullPath, UCMFILE_MODE_WRITE);
  if (iDestFd <0)   //CID 163
  {
    //File not found, creating  new file
    if (errno == ENOENT)
    {
      iDestFd = cm_file_create (iDestFd, pFileName);
      if (iDestFd == -1)
      {
        CM_LDSV_DEBUG_PRINT ("Unable to create new file.");
        return OF_FAILURE;
      }
    }
    else
    {
      CM_LDSV_DEBUG_PRINT ("Unable to open file.");
      return OF_FAILURE;
    }
  }
  memset (aBuffer, 0, sizeof (aBuffer));
  sprintf (aBuffer,
             "<configuration>\n\t<header>\n\t\t<filename>%s</filename>\n\t\t<description>%s</description>\n\t\t<version>%d</version>\n\t\t<dmpath>%s</dmpath>\n\t\t<instancepath>%s</instancepath>\n\t</header>", pFileName,pDMNodeAttrib->description_p, iVersion,
            pDMNodeAttrib->dm_path_p, pDMInstancePath);

  iRet = cm_file_write (iDestFd, iDestFd, aBuffer, strlen (aBuffer));
  if (iRet != (strlen (aBuffer)))
  {
    CM_LDSV_DEBUG_PRINT ("Unable to write into file.");
  }

  iRet = cm_file_close (iDestFd, iDestFd);
  if (iRet == -1)
  {
    CM_LDSV_DEBUG_PRINT (" Failed to close the file.");
    CM_LDSV_DEBUG_PRINT ("ERROR NO: %d", errno);
    return OF_FAILURE;
  }

  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cm_ldsv_delete_table_stack
 Input Args    : pTableStackDll - Stack node
 Output Args   : none
 Description   : This method is used to Delete stack node
 Return Values : none
 *************************************************************************/
void cm_ldsv_delete_table_stack (UCMDllQ_t* pTableStackDll)
{
  cm_ldsv_table_stack_node_t *pTmpTableStackNode;

  UCMDllQNode_t *pNode, *pTmp;

  if (pTableStackDll != NULL)
  {
    CM_DLLQ_DYN_SCAN (pTableStackDll, pNode, pTmp, UCMDllQNode_t *)
    {
      pTmpTableStackNode =
        CM_DLLQ_LIST_MEMBER (pNode, cm_ldsv_table_stack_node_t *, ListNode);
      if (pTmpTableStackNode)
      {
        CM_DLLQ_DELETE_NODE (pTableStackDll, &(pTmpTableStackNode->ListNode));
        cm_ldsv_cleanup_table_stack_node (pTmpTableStackNode);
      }
    }
  }
}

/**************************************************************************
 Function Name : cm_ldsv_write_dictionary
 Input Args    : pDMNodeAttrib- Data model Attributes
                 pTransportChannel - transport Channel
                 pFullPath - absolute file path
 Output Args   : none
 Description   : This method is used to Write dictionary elements 
                 in persistant store.
 Return Values : none
*************************************************************************/
void cm_ldsv_write_dictionary (struct cm_dm_node_info *pDMNodeAttrib,
                                void * pTransportChannel, 
                                struct cm_hash_table *pDictionaryTable,
                                char* pFullPath)
{
  char* pId = NULL;
  char* pHashValue = NULL;
  int32_t iDestFd = -1;
  int32_t ii;
  int32_t iRet;
  struct cm_role_info RoleInfo = { "VortiQa", CM_VORTIQA_SUPERUSER_ROLE };
  struct cm_dm_node_info *pNodeInfo = NULL, *pTmpNodeInfo;
  struct cm_array_of_structs *pChildInfoArr = NULL;
  char aBuffer[LDSV_MAX_BUFFER_SIZE];
  char aTmpBuffer[LDSV_BUFFER_SIZE];
  char strVal[LDSV_XML_READ_LINE];

  if (pDMNodeAttrib == NULL)
  {
    CM_LDSV_DEBUG_PRINT (" Invalid parameters. Nothing to write into file");
    return;
  }

  if (cm_ldsv_find_dictionary (pFullPath) == OF_SUCCESS)
  {
    // dictionary is already added into the file. so no need to write
    // once again.return from here.
    return;
  }
  iDestFd = cm_file_open (iDestFd, pFullPath, UCMFILE_MODE_WRITE_APPEND);
  if (iDestFd <0)  //CID 162
  {
    if(errno == ENOENT)
    {
      CM_LDSV_DEBUG_PRINT ("File not found, creating  new file");
      iDestFd = cm_file_create (iDestFd, pFullPath);
      if (iDestFd == -1)
      {
        CM_LDSV_DEBUG_PRINT ("Failed to create new file.");
        CM_LDSV_DEBUG_PRINT ("Error No is: %d", errno);
        return;
      }
    }
    else
    {
      CM_LDSV_DEBUG_PRINT ("Failed to open file.");
      CM_LDSV_DEBUG_PRINT ("Error No is: %d", errno);
      return;
    }
  }
  memset (aBuffer, 0, sizeof (aBuffer));
  memset (aTmpBuffer, 0, sizeof (aTmpBuffer));
  sprintf (aBuffer,
             "\n<body>\n\t<record name='%s' type='%d' relative_dmpath='%s'/>\n\t<dictionary>",
            pDMNodeAttrib->name_p, pDMNodeAttrib->element_attrib.element_type,
            pDMNodeAttrib->name_p);

  if (cm_dm_get_node_and_child_info (pTransportChannel,
                                pDMNodeAttrib->dm_path_p,
                                 CM_LDSV_MGMT_ENGINE_ID,
                                 &RoleInfo, &pNodeInfo,
                                 &pChildInfoArr) == OF_SUCCESS)
  {
    if ((pNodeInfo->element_attrib.element_type == CM_DMNODE_TYPE_TABLE) ||
       (pNodeInfo->element_attrib.scalar_group_b == TRUE))
    {
      for (ii = 0; ii < pChildInfoArr->count_ui; ii++)
      {
        pTmpNodeInfo = (struct cm_dm_node_info *) (pChildInfoArr->struct_arr_p) + ii;
        if (pTmpNodeInfo->element_attrib.element_type ==
            CM_DMNODE_TYPE_SCALAR_VAR)
        {
          if (pTmpNodeInfo->name_p)
          {
            memset (strVal, 0, sizeof (strVal));
            sprintf (strVal, "%d", ii);
            pId = of_calloc (1, sizeof (strVal+1));
            strcpy (pId, strVal);
            iRet = cm_hash_add_entry (pDictionaryTable, pTmpNodeInfo->name_p,
                                    (void *) pId);
            if (iRet == -1)
            {
              /* Name already there in the hash so, update with new value.*/
              /*CM_LDSV_DEBUG_PRINT ("Failed to add to hash:%s",
                                    pTmpNodeInfo->name_p);*/
              cm_hash_remove_entry(pDictionaryTable, pTmpNodeInfo->name_p, NULL);
              iRet = cm_hash_add_entry (pDictionaryTable, pTmpNodeInfo->name_p,
                                    (void *) pId);
              /*cm_hash_update_entry(pDictionaryTable, pTmpNodeInfo->name_p, 
                                (void *) pId, NULL);*/
            }
            pHashValue =
              (char *) cm_hash_lookup (pDictionaryTable,
                                         pTmpNodeInfo->name_p);
            sprintf (aTmpBuffer, "\n\t\t<a%s>%s  type=%d</a%s>", pHashValue,
                       pTmpNodeInfo->name_p,
                       pTmpNodeInfo->data_attrib.data_type, pHashValue);
            strcat (aBuffer, aTmpBuffer);
            memset (aTmpBuffer, 0, sizeof (aTmpBuffer));
          }
        }
      }
      memset (strVal, 0, sizeof (strVal));
      sprintf (strVal, "%d", ii+1);
      pId = of_calloc (1, sizeof (strVal+1));
      strcpy (pId, strVal);
      iRet = cm_hash_add_entry (pDictionaryTable, "z", (void *) pId);
    }
  }
  else
  {
    CM_LDSV_DEBUG_PRINT ("GetNodeAndChildInfo failed.");
    return;
  }

  strcat (aBuffer, "\n\t</dictionary>");
  iRet = cm_file_write (iDestFd, iDestFd, aBuffer, strlen (aBuffer));
  if (iRet == -1)
  {
    CM_LDSV_DEBUG_PRINT ("ERROR writing data into a file.");
  }

  iRet = cm_file_close (iDestFd, iDestFd);
  if (iRet == -1)
  {
    CM_LDSV_DEBUG_PRINT ("Failed to close the file.");
    if (pId != NULL)
      of_free (pId);
    return;
  }
  if (pId != NULL)
    of_free (pId);
}

/**************************************************************************
 Function Name : cm_ldsv_find_dictionary
 Input Args    : pFileName - File name
 Output Args   : none
 Description   : This method is used to find dictionary elements in the given
                 file.
 Return Values : OF_FAILURE/OF_SUCCESS.
 *************************************************************************/
int32_t cm_ldsv_find_dictionary (char *pFileName)
{
  char *pString = NULL, *pTempString = NULL;
  char aReadLine[LDSV_XML_READ_LINE];
  FILE *pFile;
  
  if (pFileName == NULL)
  {
    CM_LDSV_DEBUG_PRINT("File Name is null");
    return OF_FAILURE;
  }
  if ((pFile = fopen (pFileName, "r")) == NULL)
  {
    if (errno == ENOENT)
    {
      CM_LDSV_DEBUG_PRINT ("File Not Found.");
    }
    return OF_FAILURE;
  }
  memset (aReadLine, 0, sizeof(aReadLine));
  /* get one line from the file */
  pString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);
  pTempString = cm_ldsv_trim_string (pString);

  if (pTempString == NULL)
  {
    /* end of file/no data exists in the file */
    fclose(pFile);
    return OF_FAILURE;
  }
  while (strcmp (pTempString, "<dictionary>") != 0)
  {
    memset (aReadLine, 0, sizeof(aReadLine));
    /* get one line from the file */
    pString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);
    if (pString == NULL)
    {
      /* end of file */
      fclose (pFile);
      return OF_FAILURE;
    }
    pTempString = cm_ldsv_trim_string (pString);
  }
  fclose (pFile);
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cm_ldsv_load_file
 Input Args    : pFileName - File Name
                 pDMNodeAttrib - Data Model Attributes
                 pKeyNvPair - Name value pairs
                 pOpaqueInfo - opaque info
                 uiOpaqueInfoLen - opaque info length
 Output Args   : none
 Description   : This method is used to Load the given xml file
 Return Values : OF_FAILURE/OF_SUCCESS
*************************************************************************/
int32_t cm_ldsv_load_file (FILE *pFile,
                          struct cm_dm_node_info* pDMNodeAttrib,
                          struct cm_array_of_nv_pair* pKeyNvPair, void* pOpaqueInfo,
                          uint32_t uiOpaqueInfoLen,
                          UCMDllQ_t *LoadTableStackNode)
{
  char *pdmPath;
  char *pInstPath;
  void *pTransportChannel;
  int32_t iRetVal;
  struct cm_hash_table *pHashTable = NULL;

  pdmPath = (char *) of_calloc (LDSV_DMPATH_SIZE, sizeof (char));
  pInstPath = (char *) of_calloc (LDSV_DMPATH_SIZE, sizeof (char));
  if ((pdmPath == NULL) || (pInstPath == NULL))
  {
    CM_LDSV_DEBUG_PRINT (" Unable to create memory");
    return OF_FAILURE;
  }
  iRetVal = cm_ldsv_get_dm_path_and_instance_path_from_save_file (pFile, pdmPath, pInstPath);
  if (iRetVal == OF_FAILURE)
  {
    CM_LDSV_DEBUG_PRINT ("Failed to get dm instance path from xml file");
    of_free (pdmPath);
    of_free (pInstPath);
    return OF_FAILURE;
  }
  CM_LDSV_DEBUG_PRINT ("DMPAth=%s, InstncePath=%s:",pdmPath, pInstPath);
  pHashTable = cm_hash_create (MAX_HASH_SIZE);
  if (pHashTable == NULL)
  {
    CM_LDSV_DEBUG_PRINT ("Dictonary Table is not created");
    of_free (pdmPath);
    of_free (pInstPath);
    return OF_FAILURE;
  }
  iRetVal = cm_ldsv_tokenize_dictionary (pFile, pHashTable);
  if (iRetVal == OF_FAILURE)
  {
    cm_hash_free (pHashTable, 0);
    of_free (pdmPath);
    of_free (pInstPath);
    return OF_FAILURE;
  }
   pTransportChannel = cm_ldsv_get_transport_channel();
  /* Validate transport channel */
  if (unlikely (pTransportChannel == NULL))
  {
    /* Create new transport channel */
    pTransportChannel =(void *) cm_tnsprt_create_channel (CM_IPPROTO_TCP, 0, 0,
                                                   UCMJE_CONFIGREQUEST_PORT);
  }

  if ((cm_ldsv_tokenize_data_and_send_to_je (pFile, pdmPath, pInstPath, 
                                   pDMNodeAttrib, pKeyNvPair,
                                   pOpaqueInfo, uiOpaqueInfoLen,
                                   pHashTable, LoadTableStackNode)) == OF_FAILURE)
  {
    of_free (pdmPath);
    of_free (pInstPath);
/*    if (cm_hash_size (pHashTable) > 0)
    { - CID 371*/
      CM_LDSV_DEBUG_PRINT ("cleaning hash entries.");
      cm_hash_free (pHashTable, NULL);
/*    }*/
    return OF_FAILURE;
  }
/*  if (cm_hash_size (pHashTable) > 0)
  { - CID 371*/
    CM_LDSV_DEBUG_PRINT ("cleaning hash entries.");
    cm_hash_free (pHashTable, NULL);
/*  }*/
  of_free (pdmPath);
  of_free (pInstPath);
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cm_ldsv_close_open_xml_tags
 Input Args    : pFileName - file name
 Output Args   : none
 Description   : This method is used to close xml tags in the given file
 Return Values : OF_SUCCESS/OF_FAILURE
 *************************************************************************/
int32_t cm_ldsv_close_open_xml_tags (char* pFileName)
{
  int32_t iDestFd = -1;
  int32_t iRet;
  char *pString = NULL, *pTempString = NULL;
  char aReadLine[LDSV_XML_READ_LINE];
  char aBuffer[LDSV_BUFFER_SIZE];
  FILE *pFile;

  if (pFileName == NULL)
  {
    CM_LDSV_DEBUG_PRINT("File Name is null.");
    return OF_FAILURE;
  }
  if ((pFile = fopen (pFileName, "r")) == NULL)
  {
    CM_LDSV_DEBUG_PRINT (" Error opening input file");
    CM_LDSV_DEBUG_PRINT ("ERROR CODE :%d", errno);
    return OF_FAILURE;
  }

  memset (aReadLine, 0, sizeof(aReadLine));
  pString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);      /* get one line from the file */
  pTempString = cm_ldsv_trim_string (pString);

  while (pTempString != NULL)
  {
    /* 
     * Trying to find out whether opened <configuration> tag
     * in the xml file has been closed or not.
     * <configuration> tag is the root tag for our ldsv xml files.
     * if already closed, no need to close once again.
     * if not close </body> and </configuration> tags.
     */ 
    if (strcmp (pTempString, "</configuration>") == 0)
    {
      fclose (pFile);
      return OF_SUCCESS;
    }
    memset (aReadLine, 0, sizeof(aReadLine));
    pString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);    /* get one line from the file */
    pTempString = cm_ldsv_trim_string (pString);
    if (pTempString == NULL)
    {
      /*end of file reached and no closing </configuration>
      * tag found. so we need to close opened <body> and <configuration>
      * tags. */
      break;
    }
  }
  fclose (pFile);

  iDestFd = cm_file_open (iDestFd, pFileName, UCMFILE_MODE_WRITE_APPEND);
  if (iDestFd == -1)
  {
    CM_LDSV_DEBUG_PRINT (" Unable to open file ");
    return OF_FAILURE;
  }
  memset (aBuffer, 0, sizeof (aBuffer));
  sprintf (aBuffer, "\n</body>\n</configuration>");

  iRet = cm_file_write (iDestFd, iDestFd, aBuffer, strlen (aBuffer));
  if (iRet == -1)
  {
    CM_LDSV_DEBUG_PRINT (" Unable to write into a file.");
    CM_LDSV_DEBUG_PRINT ("ERROR NO: %d", errno);
    return OF_FAILURE;
  }
  iRet = cm_file_close (iDestFd, iDestFd);
  if (iRet == -1)
  {
    CM_LDSV_DEBUG_PRINT (" Failed to close the file.");
    CM_LDSV_DEBUG_PRINT (" ERROR NO: %d", errno);
    return OF_FAILURE;
  }
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cm_ldsv_prepare_instance_dm_path
 Input Args    : pDMNodeAttrib - Data model Attributes
                 bLoadFlag - Boolean TRUE/FALSE
 Output Args   : pdmPath - newly prepared instance tree dm path.
 Description   : This method is used to Prepare instance tree path from 
                 the stack node. 
 Return Values : OF_SUCCESS/OF_FAILURE
*************************************************************************/
int32_t cm_ldsv_prepare_instance_dm_path (struct cm_dm_node_info * pDMNodeAttrib,
                                       char* pdmPath,
                                       struct cm_array_of_nv_pair *dm_path_pArrayOfKeys,
                                       UCMDllQ_t *pLdsvCfgStackDllQ)
{
  char *pToken = NULL;
  char aDmPath[LDSV_DMPATH_SIZE];
  char *pTmname_pPath = NULL;
  char *pDMTplPath;
  uint32_t uiIndex = 0;
  uint32_t uiMaxTokenLen, uiTokenLen;
  uint32_t uiOpaqueInfoLen = 0;
  int32_t iRet = CM_LDSV_DM_PATH_PARSING_REMAINING;
  int32_t iRetVal;
  int32_t iTreeRet;
  int32_t iKeyCnt = 0;
  void *pOpaqueInfo = NULL;
  void *pTransportChannel = NULL;
  struct cm_role_info RoleInfo = { "VortiQa", CM_VORTIQA_SUPERUSER_ROLE };
  struct cm_array_of_structs *pChildInfoArr = NULL;
  uint32_t   ii,jj;
  uint32_t uiKeyCnt=0;
  struct cm_dm_node_info *pKeyChild=NULL;

  struct cm_dm_node_info *pDmNodeAttrib = NULL;
  struct cm_array_of_nv_pair *pArrayOfKeys = NULL;

  pArrayOfKeys = cm_ldsv_collect_keys_from_table_stack (pLdsvCfgStackDllQ,
                                                      dm_path_pArrayOfKeys);
  if (pArrayOfKeys == NULL)
  {
    CM_LDSV_DEBUG_PRINT ("Keys not found.");
  }
  uiMaxTokenLen = strlen (pDMNodeAttrib->dm_path_p) + 1;
  if (uiMaxTokenLen == 1)
  {
    cm_ldsv_clean_nv_pair_array(pArrayOfKeys, TRUE);
    return OF_FAILURE;
  }
  pTmname_pPath = (char *) of_calloc (1, uiMaxTokenLen);
  if (!pTmname_pPath)
  {
    cm_ldsv_clean_nv_pair_array(pArrayOfKeys, TRUE);
    return OF_FAILURE;
  }
  /* Allocate token buffer */
  pToken = (char *) of_calloc (1, uiMaxTokenLen);
  if (!pToken)
  {
    cm_ldsv_clean_nv_pair_array(pArrayOfKeys, TRUE);
    of_free (pTmname_pPath);
    return OF_FAILURE;
  }
   pTransportChannel = cm_ldsv_get_transport_channel();
  /* Validate transport channel */
  if (unlikely (pTransportChannel == NULL))
  {
    /* Create new transport channel */
    pTransportChannel =(void *) cm_tnsprt_create_channel (CM_IPPROTO_TCP, 0, 0,
                                                   UCMJE_CONFIGREQUEST_PORT);
  }
  pDMTplPath = (char *) of_calloc (1, uiMaxTokenLen);
  if (!pDMTplPath)
  {
    cm_ldsv_clean_nv_pair_array(pArrayOfKeys, TRUE);
    of_free (pTmname_pPath);
    of_free (pToken);
    return OF_FAILURE;
  }

  memset (pToken, 0, sizeof (pToken));
  strncpy (pTmname_pPath, pDMNodeAttrib->dm_path_p, (uiMaxTokenLen - 1));

  memset (pDMTplPath, 0, sizeof (pDMTplPath));
  memset (aDmPath, 0, sizeof (aDmPath));
  while (iRet == CM_LDSV_DM_PATH_PARSING_REMAINING)
  {
    /* Tokenize the input string */
    iRet =
      cm_ldsv_get_dm_path_token (pTmname_pPath, pToken, uiMaxTokenLen, &uiIndex);
    if (pToken)
    {
      uiTokenLen = strlen (pToken);
      strncat (pDMTplPath, pToken, uiTokenLen);
      strncat (aDmPath, pToken, uiTokenLen);
    }
    CM_LDSV_DEBUG_PRINT ("dm_path_p=%s,pDMTplPath=%s,aDmPath=%s,pToken=%s: ",pDMNodeAttrib->dm_path_p,pDMTplPath,aDmPath,pToken);

    iTreeRet = cm_dm_get_first_node_info (pDMTplPath,
                                       UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
                                       pTransportChannel,
                                       CM_LDSV_MGMT_ENGINE_ID,
                                       &RoleInfo,
                                       &pDmNodeAttrib, &pOpaqueInfo,
                                       &uiOpaqueInfoLen);
    if ((iTreeRet == OF_FAILURE) || (pDmNodeAttrib == NULL))
    {
      CM_LDSV_DEBUG_PRINT("Get First Node Info failed");
      cm_ldsv_clean_nv_pair_array(pArrayOfKeys, TRUE);
      of_free (pTmname_pPath);
      of_free (pToken);
      of_free (pDMTplPath);
      return OF_FAILURE;
    }

    CM_LDSV_DEBUG_PRINT ("dm_path_p=%s,pDMName=%s: ",pDmNodeAttrib->dm_path_p,pDmNodeAttrib->name_p);
    if (pDmNodeAttrib->element_attrib.element_type == CM_DMNODE_TYPE_TABLE)
    {
      iRetVal = cm_dm_get_key_childs (pTransportChannel, pDmNodeAttrib->dm_path_p,
                                 CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, &pChildInfoArr);
      if ((pChildInfoArr == NULL) || (iRetVal != OF_SUCCESS))
      {
        CM_LDSV_DEBUG_PRINT("Failed to get child key info");
        cm_ldsv_clean_nv_pair_array(pArrayOfKeys, TRUE);
        of_free (pTmname_pPath);
        of_free (pToken);
        of_free (pDMTplPath);
        return OF_FAILURE;
      }
      if (iRet == CM_LDSV_DM_PATH_PARSING_REMAINING)
      {
        if ((pArrayOfKeys != NULL) && (pArrayOfKeys->count_ui) > iKeyCnt)
        {
          strncat (pDMTplPath, ".", 1);
	  if(pChildInfoArr->count_ui > 0 )
          {
	    strncat(aDmPath,"{", 1);
	    for (ii=0; ii < pChildInfoArr->count_ui; ii++)
	    {
	      pKeyChild=(struct cm_dm_node_info *)(pChildInfoArr->struct_arr_p) + ii;
	      for (jj=uiKeyCnt; jj< pArrayOfKeys->count_ui; jj++)
	      {
	      	if(strcmp(pKeyChild->name_p, pArrayOfKeys->nv_pairs[jj].name_p) == 0)
	      	{
	      	  if(pChildInfoArr->count_ui > 1)
	      	  {
	      	    strncat(aDmPath, pKeyChild->name_p, strlen(pKeyChild->name_p));
	            strncat(aDmPath,"=",1);
	          }
	          strncat(aDmPath, pArrayOfKeys->nv_pairs[jj].value_p, pArrayOfKeys->nv_pairs[jj].value_length);
	      	  if(ii != (pChildInfoArr->count_ui-1 ))
	          {
	      	    strncat(aDmPath,",",1);
	      	  }
	      	 uiKeyCnt++;
	       }
	     }
           }
           strncat(aDmPath,"}",1);
           strncat (aDmPath, ".", 1);
        }

#if 0
          strncat (pDMTplPath, ".", 1);
          strncat (aDmPath, "{", 1);
          strncat (aDmPath, pArrayOfKeys->nv_pairs[iKeyCnt].value_p,
                     pArrayOfKeys->nv_pairs[iKeyCnt].value_length);
          strncat (aDmPath, "}", 1);
          strncat (aDmPath, ".", 1);
          iKeyCnt = iKeyCnt+1;
#endif
        }
        else
        {
          strncat (pDMTplPath, ".", 1);
          strncat (aDmPath, ".", 1);
        }
      }
    }
    else
    {
      if (iRet == CM_LDSV_DM_PATH_PARSING_REMAINING)
      {
        strncat (aDmPath, ".", 1);
        strncat (pDMTplPath, ".", 1);
      }
    }
    cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo, uiOpaqueInfoLen);
    cm_ldsv_clean_dm_node_attrib_info (pDmNodeAttrib);
  }
  strcpy (pdmPath, aDmPath);
  CM_LDSV_DEBUG_PRINT ("dm_path_p=%s:",aDmPath);
  //*ppdmPath = aDmPath;
  cm_ldsv_clean_nv_pair_array(pArrayOfKeys, FALSE);
  of_free (pTmname_pPath);
  of_free (pToken);
  of_free (pDMTplPath);
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cm_ldsv_get_dm_path_token
 Input Args    : pPathString - String to tokenize
                 uiMaxTokenLen - Max Token Length
                 pIndex - Index
 Output Args   : pToken- Newly Tokenized string
 Description   : This method is used to Tokenize dm path into tokens
 Return Values : OF_SUCCESS/OF_FAILURE
*************************************************************************/
int32_t cm_ldsv_get_dm_path_token (char * pPathString,
                                char * pToken,
                                uint32_t uiMaxTokenLen, uint32_t * pIndex)
{
  uint32_t uiTmpIndex;
  uint32_t uiStrLen;
  uint32_t ii, jj = 0;
  unsigned char bCurlyBraceFound = FALSE;

  if ((pPathString == NULL) || (pIndex == NULL) || (pToken == NULL))
  {
    return CM_LDSV_DM_PATH_PARSING_COMPLETED;
  }
  uiMaxTokenLen = strlen (pPathString);
  uiTmpIndex = *pIndex;
  uiStrLen = strlen (pPathString);
  if (uiTmpIndex >= uiStrLen)
  {
    pToken[0] = '\0';
    return CM_LDSV_DM_PATH_PARSING_COMPLETED;
  }

  for (ii = uiTmpIndex; ii < uiStrLen; ii++)
  {
    if (jj >= uiMaxTokenLen)
    {
      pToken[0] = '\0';
      return CM_LDSV_DM_PATH_PARSING_COMPLETED;
    }
    if ((pPathString[ii] == '.'))
    {
      if (bCurlyBraceFound == TRUE)
      {
        pToken[jj++] = pPathString[ii];
        *pIndex = ii + 1;
        continue;
      }
      pToken[jj] = '\0';
      *pIndex = ii + 1;
      break;
    }
    /*  uiTmpIndex is begining of new token
     * tokenize at '{' if it is not the first position
     */
    if ((pPathString[ii] == '{') && (ii != uiTmpIndex))
    {
      pToken[jj] = '\0';
      *pIndex = ii;
      break;
    }
    if (pPathString[ii] == '{')
    {
      bCurlyBraceFound = TRUE;
    }
    else if(pPathString[ii] == '}')
    {
      bCurlyBraceFound = FALSE;
    }
    pToken[jj++] = pPathString[ii];
    *pIndex = ii + 1;
  }

  if (*pIndex >= uiStrLen)
  {
    pToken[jj] = '\0';
    return CM_LDSV_DM_PATH_PARSING_COMPLETED;
  }
  return CM_LDSV_DM_PATH_PARSING_REMAINING;
}

/**************************************************************************
 Function Name : cm_ldsv_clean_tree_traversal_opaque_info
 Input Args    : pOpaqueInfo - Opaque info
                 uiOpaqueInfoLen - Opaque info length
 Output Args   : none
 Description   : This method is used to Clean opaque info
 Return Values : none 
 *************************************************************************/
void cm_ldsv_clean_tree_traversal_opaque_info (void * pOpaqueInfo,
                                             uint32_t uiOpaqueInfoLen)
{
  of_free (pOpaqueInfo);
  pOpaqueInfo = NULL;
}

/**************************************************************************
 Function Name : cm_ldsv_clean_dm_node_attrib_info
 Input Args    : pDMNodeAttrib - Data model Attributes 
 Output Args   : none
 Description   : This method is used to Clean data model Attributes
 Return Values : none 
 *************************************************************************/
void cm_ldsv_clean_dm_node_attrib_info (struct cm_dm_node_info * pDMNdAttrib)
{
	 int32_t iCnt;

	 if (pDMNdAttrib != NULL)
	 {
			if (((pDMNdAttrib->name_p) != NULL) && (strlen(pDMNdAttrib->name_p) >0))
				 of_free (pDMNdAttrib->name_p);
	
			if (((pDMNdAttrib->friendly_name_p) != NULL) && (strlen(pDMNdAttrib->friendly_name_p) >0))
				 of_free (pDMNdAttrib->friendly_name_p);

			if (((pDMNdAttrib->description_p) != NULL)  && (strlen(pDMNdAttrib->description_p) >0))
				 of_free (pDMNdAttrib->description_p);

			if (((pDMNdAttrib->dm_path_p) != NULL)  && (strlen(pDMNdAttrib->dm_path_p) >0))
				 of_free (pDMNdAttrib->dm_path_p);

			if (pDMNdAttrib->element_attrib.command_name_p)
			{
				 of_free(pDMNdAttrib->element_attrib.command_name_p);
				 pDMNdAttrib->element_attrib.command_name_p=NULL;
			}
			if (pDMNdAttrib->element_attrib.cli_identifier_p)
			{
				 of_free(pDMNdAttrib->element_attrib.cli_identifier_p);
				 pDMNdAttrib->element_attrib.cli_identifier_p=NULL;
			}
			if (pDMNdAttrib->element_attrib.cli_idenetifier_help_p)
			{
				 of_free(pDMNdAttrib->element_attrib.cli_idenetifier_help_p);
				 pDMNdAttrib->element_attrib.cli_idenetifier_help_p=NULL;
			}

			if((pDMNdAttrib->data_attrib.data_type == CM_DATA_TYPE_STRING || pDMNdAttrib->data_attrib.data_type == CM_DATA_TYPE_STRING_SPECIAL_CHARS) && pDMNdAttrib->data_attrib.attrib_type == CM_DATA_ATTRIB_STR_ENUM)
			{
				 for( iCnt =0 ;iCnt<pDMNdAttrib->data_attrib.attr.string_enum.count_ui;iCnt++)
				 {
						if (pDMNdAttrib->data_attrib.attr.string_enum.array[iCnt])
						of_free(pDMNdAttrib->data_attrib.attr.string_enum.array[iCnt]);
				 }
			}

			if((pDMNdAttrib->data_attrib.data_type == CM_DATA_TYPE_STRING || pDMNdAttrib->data_attrib.data_type == CM_DATA_TYPE_STRING_SPECIAL_CHARS)&& pDMNdAttrib->data_attrib.attrib_type == CM_DATA_ATTRIB_STR_ENUM)
			{
				 for( iCnt =0 ;iCnt<  pDMNdAttrib->data_attrib.attr.string_enum.count_ui;iCnt++)
				 {
						if (pDMNdAttrib->data_attrib.attr.string_enum.aFrdArr[iCnt])
						of_free(pDMNdAttrib->data_attrib.attr.string_enum.aFrdArr[iCnt]);
				 }
			}

			of_free (pDMNdAttrib);
	 }
}

/**************************************************************************
 Function Name : cm_ldsv_string_tokenize
 Input Args    : s - string to tokenize
                 ct - character to tokenize the string(#,%s...)
 Output Args   : Newly tokenized string 
 Description   : This method is used to tokenize the given string using
                 tokenization parameter ct.
 Return Values : Newly tokenized string
 *************************************************************************/
char *cm_ldsv_string_tokenize (char * s, char * ct)
{
  char *pSbegin = NULL;
  char *pSend = NULL;
  static char *pSsave = NULL;

  pSbegin = s ? s : pSsave;
  if (!pSbegin)
  {
    return NULL;
  }
  if (*pSbegin == '\0')
  {
    pSsave = NULL;
    return NULL;
  }
  pSend = cm_ldsv_string_break_p (pSbegin, ct);
  if (pSend && *pSend != '\0')
  {
    *pSend++ = '\0';
  }
  pSsave = pSend;
  return pSbegin;
}

/**************************************************************************
 Function Name : cm_ldsv_string_break_p
 Input Args    : cs - string to tokenize
                 ct - character to tokenize the string(#,%s...)
 Output Args   : Newly tokenized string 
 Description   : This method is used to tokenize the given string using
                 tokenization parameter ct.
 Return Values : Newly tokenized string
*************************************************************************/
char *cm_ldsv_string_break_p (char * cs, char * ct)
{
  char *psc1 = NULL;
  char *psc2 = NULL;
  for (psc1 = cs; *psc1 != '\0'; ++psc1)
  {
    for (psc2 = ct; *psc2 != '\0'; ++psc2)
    {
      if (*psc1 == *psc2)
      {
        return (char *) psc1;
      }
    }
  }
  return NULL;
}

/**************************************************************************
 Function Name : cm_ldsv_create_directory_path
 Input Args    : pDMInstancePath - Data model instance path
 Output Args   : pDir - new directory name
 Description   : This method is used to Create directory path
 Return Values : OF_SUCCESS/OF_FAILURE
*************************************************************************/
int32_t cm_ldsv_create_directory_path (char *pDMInstancePath, 
                                     char * pDir)
{

  char *pToken = NULL;
  char *pKey = NULL;
  char *pEndOfKey = NULL;
  char *pPrevToken = NULL;
  uint32_t uiMaxTokenLen;
  uint32_t uiIndex = 0;
  char *pResult = NULL;
  char *pResultArry[100];
  int32_t ii=0, ix = 0;
  int32_t iRet = CM_LDSV_DM_PATH_PARSING_REMAINING;
  char aBuffer[100];
  char aTmpBuffer[100];

  if (!pDMInstancePath)
  {
    CM_LDSV_DEBUG_PRINT ("DM Path NULL");
    return OF_FAILURE;
  }
  if (!pDir)
  {
    CM_LDSV_DEBUG_PRINT ("Directory Path NULL");
    return OF_FAILURE;
  }

  uiMaxTokenLen = strlen (pDMInstancePath);
  pToken = (char *) of_calloc (1, uiMaxTokenLen + 1);
  pPrevToken = (char *) of_calloc (1, uiMaxTokenLen + 1);

  iRet =
    cm_ldsv_get_dm_path_token (pDMInstancePath, pToken, uiMaxTokenLen, &uiIndex);
  memcpy (pPrevToken, pToken, strlen (pToken));
  memset (aBuffer, 0, sizeof (aBuffer));
  memset (aTmpBuffer, 0, sizeof (aTmpBuffer));
  while (iRet == CM_LDSV_DM_PATH_PARSING_REMAINING)
  {
    iRet =
      cm_ldsv_get_dm_path_token (pDMInstancePath, pToken, uiMaxTokenLen, &uiIndex);
    if (*pToken == '{')
    {
      memset (aBuffer, 0, sizeof (aBuffer));
      ii = 0;
      ix = 0;
      pKey = pToken + 1;
      pEndOfKey = strchr (pKey, '}');
      if (pEndOfKey == NULL)
      {
        of_free(pToken);
        of_free(pPrevToken);
        return OF_FAILURE;
      }
      //*pEndOfKey = '\0';
      pResult = cm_ldsv_string_tokenize (pToken, "{,=}");
      while (pResult != NULL)
      {
         pResultArry[ix] = pResult;
         ix++;
         pResult = cm_ldsv_string_tokenize (NULL,"{,=}");
         ii++;
      }
      strncat (aBuffer, pPrevToken, strlen (pPrevToken));
      for (ix=0; ix<ii; ix++)
      {
        if ((pResultArry[ix] != NULL) && strlen(pResultArry[ix]) > 0)
        {
          sprintf (aTmpBuffer, "%s", pResultArry[ix]);
          strcat(aBuffer, aTmpBuffer);
          memset (aTmpBuffer, 0, sizeof (aTmpBuffer));
        }
        if (ii > 2)
          ix=ix+1;
      }
      strncat (pDir, aBuffer, strlen (aBuffer));
      strncat (pDir, "/", 1);
      memset (pPrevToken, 0, uiMaxTokenLen);
    }
    else
    {
      if (*pPrevToken != '\0')
      {
        strncat (pDir, pPrevToken, strlen (pPrevToken));
        strncat (pDir, "/", 1);
      }
      memset (pPrevToken, 0, uiMaxTokenLen);
      memcpy (pPrevToken, pToken, strlen (pToken));
    }
  }

  if (iRet == CM_LDSV_DM_PATH_PARSING_COMPLETED)
  {
    if (*pToken == '{')
    {
      pKey = pToken + 1;
      pEndOfKey = strchr (pKey, '}');
      if (pEndOfKey == NULL)
      {
        of_free(pToken);
        of_free(pPrevToken);
        return OF_FAILURE;
      }
      *pEndOfKey = '\0';
      pResult = cm_ldsv_string_tokenize (pToken, "{,=}");
      while (pResult != NULL)
      {
         pResultArry[ix] = pResult;
         ix++;
         pResult = cm_ldsv_string_tokenize (NULL,"{,=}");
         ii++;
      }
      for (ix=0; ix<ii; ix=ix+2)
      {
        if ((pResultArry[ix] != NULL) && strlen(pResultArry[ix]) > 0)
        {
          sprintf (aTmpBuffer, "%s", pResultArry[ix]);
          strcat(aBuffer, aTmpBuffer);
          memset (aTmpBuffer, 0, sizeof (aTmpBuffer));
        }
      }
      //CM_LDSV_DEBUG_PRINT("==> Result String:%s", aBuffer);
      strncat (pDir, aBuffer, strlen (aBuffer));
      //strncat (pDir, pKey, strlen (pKey));
/*      pPrevToken = NULL;  CID - 361 - check*/
    }
    else
    {
#if 0
      if (*pPrevToken != '\0')
      {
        strncat (pDir, pPrevToken, strlen (pPrevToken));
      }
#endif
    }
  }
  of_free (pToken);
  of_free(pPrevToken);
  return OF_SUCCESS;
}


void cm_ldsv_free_node_info_array (struct cm_array_of_structs *	pArrayOfNodeInfo)
{
  int32_t ii;
  struct cm_dm_node_info *pNodeInfo;

  if (pArrayOfNodeInfo == NULL)
  {
    //of_free (pArrayOfNodeInfo);
    return;
  }
  for (ii = 0; ii < pArrayOfNodeInfo->count_ui; ii++)
  {
    pNodeInfo=(struct cm_dm_node_info *) pArrayOfNodeInfo->struct_arr_p +ii;
    if (pNodeInfo)
    {
       if (pNodeInfo->name_p)
         of_free (pNodeInfo->name_p);

       if (pNodeInfo->friendly_name_p)
         of_free (pNodeInfo->friendly_name_p);

       if (pNodeInfo->description_p)
         of_free (pNodeInfo->description_p);

       if (pNodeInfo->dm_path_p)
         of_free (pNodeInfo->dm_path_p);
     }
   }
   of_free (pArrayOfNodeInfo->struct_arr_p);
   of_free (pArrayOfNodeInfo);
}

/**************************************************************************
 Function Name :cm_ldsv_write_version_number_to_file 
 Input Args    : pFullPath - abolute path
                 iVersionNo - version no
 Output Args   : none
 Description   : This method is used to write current saved version into a file
 Return Values : OF_FAILURE/OF_SUCCESS
 *************************************************************************/

 int32_t cm_ldsv_write_version_number_to_file(char* pFullPath, uint64_t iVersionNo)
{
  int32_t iDestFd = -1;
  int32_t iRet, iretval;
  char aBuffer[LDSV_MAX_BUFFER_SIZE];
  char aTmpBuffer[LDSV_MAX_BUFFER_SIZE];
  uint64_t ullTempEngVer=0, ullTempSaveVer=0;
  int32_t iRetVal=0;
  FILE *pFile;
  char aPrevSavedVersionNo[20+1]; /*UINT64 Verison consists: UINT32 ->eng ver and config ver. this should be convertd as 'x.y'*/
  char aRemPreSavedDir[LDSV_MAX_BUFFER_SIZE];
  char aCopySavedDirtoFlash[LDSV_MAX_BUFFER_SIZE];
  uint64_t ullPrevSavedConfigVer;


  /* Get previously saved version.
     As SAVE is succesful, remove the previously saved directory.
     If SAVE fails then leave it currently. We can use the current directory for debugging purpose.
     In future, we'll remove the failed directory.
    */ 
  memset(aPrevSavedVersionNo, 0, sizeof(aPrevSavedVersionNo));
  if ((pFile = fopen (pFullPath, "r")) != NULL)
  {
    iretval = cm_ldsv_get_version_number_from_file(pFile, aPrevSavedVersionNo, &ullPrevSavedConfigVer);
    CM_LDSV_DEBUG_PRINT ("Failed to retrive previous saved config version number- returned: %d", iretval);
    fclose(pFile);
  }

  ullTempEngVer= ((iVersionNo>>32)&0x00000000ffffffff);
  ullTempSaveVer = iVersionNo &0x00000000ffffffff;

  if (iVersionNo <= 0)
  {
    CM_LDSV_DEBUG_PRINT ("Invalid Version.Nothing to write into file");
    return OF_FAILURE;
  }

  /* Open file if found, otherwise create a new file */
  iDestFd = cm_file_open (iDestFd, pFullPath, UCMFILE_MODE_WRITE_APPEND);
  if (iDestFd < 0)  //CID 165
  {
    if(errno == ENOENT)
    {
      iDestFd = cm_file_create (iDestFd, pFullPath);
      CM_LDSV_DEBUG_PRINT("File Fd:%d", iDestFd);
      if (iDestFd == -1)
      {
        CM_LDSV_DEBUG_PRINT ("Failed to create new file.");
        CM_LDSV_DEBUG_PRINT ("Error No is: %d", errno);
        return OF_FAILURE;
      }
    }
    else
    {
      CM_LDSV_DEBUG_PRINT ("Failed to open file.");
      CM_LDSV_DEBUG_PRINT ("Error No is: %d", errno);
      return OF_FAILURE;
    }
  }
  else
  {
    cm_ldsv_create_backup (CM_CONFIGSYNC_CONFIG_VER_FILE, PERSISTENT_TRANSFERLIST_DIR);
    iDestFd = cm_file_create (iDestFd, pFullPath);
  }
  memset (aBuffer, 0, sizeof (aBuffer));
  memset (aTmpBuffer, 0, sizeof (aTmpBuffer));
  /*sprintf (aBuffer, "\n\t\t<row>");
  sprintf (aTmpBuffer, "\n\t\t\t<version>%d</version>", iVersionNo);
  strcat (aBuffer, aTmpBuffer);
  strcat (aBuffer, "\n\t\t</row>");*/
  sprintf(aBuffer,"%ld.%ld",(uint32_t)ullTempEngVer,(uint32_t)ullTempSaveVer);

  cm_file_write (iDestFd, iDestFd, aBuffer, strlen (aBuffer));

  iRet = cm_file_close (iDestFd, iDestFd);
    
  CM_LDSV_DEBUG_PRINT("Successful Saved Version :: %s",aBuffer);
  
  if (iRet == -1)
  {
    CM_LDSV_DEBUG_PRINT (" Failed to close the file.");
    return OF_FAILURE;
  }
  if(strcmp(aPrevSavedVersionNo,"") != 0)
  {
    memset (aRemPreSavedDir, 0, sizeof (aRemPreSavedDir));
    sprintf(aRemPreSavedDir,"rm -rf %s/%s",PERSISTENT_TRANSFERLIST_DIR,aPrevSavedVersionNo);
    CM_LDSV_DEBUG_PRINT (" PreSavedDir :%s.",aRemPreSavedDir);
  
    CM_LDSV_DEBUG_PRINT("********Removing :%s*********",aRemPreSavedDir);
    iRetVal = system (aRemPreSavedDir);
    if (iRetVal != OF_SUCCESS)
    {
      CM_LDSV_DEBUG_PRINT("Failed to delete previously saved version directory.");
    }
  }
 
#ifndef CM_MGMT_CARD_SUPPORT
  /*remove existing /mnt/vqfs/config directory, copy saved version to /mnt/vqfs/ dirrectory*/
#if 0 
  system("rm -rf /mnt/vqfs/config");
  memset (aCopySavedDirtoFlash, 0, sizeof (aCopySavedDirtoFlash));
  sprintf(aCopySavedDirtoFlash,"cp -rf %s %s",PERSISTENT_TRANSFERLIST_DIR, SAVE_CONFIG_TO_FLASH_DIR);
  
  CM_LDSV_DEBUG_PRINT("Command:%s",aCopySavedDirtoFlash);
  
  iRetVal = system (aCopySavedDirtoFlash);
  if (iRetVal != OF_SUCCESS)
  {
    CM_LDSV_DEBUG_PRINT("Failed to copy the saved configuration to flash directory");
  }
#endif  
  CM_LDSV_DEBUG_PRINT("Successfully copied saved configuration on hard disk");

  /*Execute sync command*/
/*Atmaram - sync is not required. since we are saving configuration on partition*/
  //system ("sync");
  
  CM_LDSV_DEBUG_PRINT("Sync command executed");
#endif
  
  return OF_SUCCESS;
}

#ifdef CM_ROLES_PERM_SUPPORT
/**************************************************************************
 Function Name : cm_write_role_permissions_info 
 Input Args    : pFullPath - abolute path
                 pRolePermAry - Name value pairs
                 dm_path_p - data model template tree path
 Output Args   : none
 Description   : This method is used to write roles and permissions of a given data model path
 Return Values : OF_FAILURE/OF_SUCCESS
 *************************************************************************/

 int32_t cm_write_role_permissions_info(struct cm_dm_array_of_role_permissions *pRolePermAry,
                                             char* pFullPath, char * dm_path_p)
{
  int32_t ii;
  char aPerm[CM_MAX_ADMINROLE_LEN]={};
  int32_t iDestFd = -1;
  int32_t iRet;
  char aBuffer[LDSV_MAX_BUFFER_SIZE];
  char aTmpBuffer[LDSV_MAX_BUFFER_SIZE];

  if (pRolePermAry == NULL)
  {
    CM_LDSV_DEBUG_PRINT ("Invalid data.Nothing to write into file");
    return OF_FAILURE;
  }

  /* Open file if found, otherwise create a new file */
  iDestFd = cm_file_open (iDestFd, pFullPath, UCMFILE_MODE_WRITE_APPEND);
  if (iDestFd < 0)  //CID 165
  {
    if(errno == ENOENT)
    {
      iDestFd = cm_file_create (iDestFd, pFullPath);
      CM_LDSV_DEBUG_PRINT("File Fd:%d", iDestFd);
      if (iDestFd == -1)
      {
        CM_LDSV_DEBUG_PRINT ("Failed to create new file.");
        CM_LDSV_DEBUG_PRINT ("Error No is: %d", errno);
        return OF_FAILURE;
      }
    }
    else
    {
      CM_LDSV_DEBUG_PRINT ("Failed to open file.");
      CM_LDSV_DEBUG_PRINT ("Error No is: %d", errno);
      return OF_FAILURE;
    }
  }
  memset (aBuffer, 0, sizeof (aBuffer));
  memset (aTmpBuffer, 0, sizeof (aTmpBuffer));
  sprintf (aBuffer, "\t\t<row>");
  sprintf (aTmpBuffer, "\n\t\t\t<dmpath>%s</dmpath>", dm_path_p);
  strcat (aBuffer, aTmpBuffer);
  memset (aTmpBuffer, 0, sizeof (aTmpBuffer));
  for (ii = 0; ii < pRolePermAry->count_ui; ii++)
  {
    memset(aPerm, 0, sizeof(aPerm));
    cm_get_permission_string(pRolePermAry->RolePermissions[ii].uiPermissions, aPerm);
    sprintf (aTmpBuffer, "\n\t\t\t<role>%s</role>", (char*)pRolePermAry->RolePermissions[ii].ucRole);
    strcat (aBuffer, aTmpBuffer);
    memset (aTmpBuffer, 0, sizeof (aTmpBuffer));
    sprintf (aTmpBuffer, "\n\t\t\t<permission>%s</permission>", aPerm);
    strcat (aBuffer, aTmpBuffer);
    memset (aTmpBuffer, 0, sizeof (aTmpBuffer));
  }
  strcat (aBuffer, "\n\t\t</row>\n");
  cm_file_write (iDestFd, iDestFd, aBuffer, strlen (aBuffer));

  iRet = cm_file_close (iDestFd, iDestFd);
  if (iRet == -1)
  {
    CM_LDSV_DEBUG_PRINT (" Failed to close the file.");
    return OF_FAILURE;
  }
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cm_get_permission_string 
 Input Args    : uiPerm - permission value
 Output Args   : pPerm - permission string
 Description   : This method is used to get permission string using a integer permission value
 Return Values : pPerm - permission string
 *************************************************************************/
 void cm_get_permission_string(uint32_t uiPerm, char* pPerm)
{
  switch(uiPerm)
  {
    case     CM_PERMISSION_NOACCESS:
      strcpy(pPerm, "CM_PERMISSION_NOACCESS");
      break;
    case     CM_PERMISSION_READ_ONLY:
      strcpy(pPerm, "CM_PERMISSION_READ_ONLY");
      break;
    case     CM_PERMISSION_READ_WRITE:
      strcpy(pPerm, "CM_PERMISSION_READ_WRITE");
      break;
    case     CM_PERMISSION_READ_ROLEPERM:
      strcpy(pPerm, "CM_PERMISSION_READ_ROLEPERM");
      break;
    case     CM_PERMISSION_READ_WRITE_ROLEPERM:
      strcpy(pPerm, "CM_PERMISSION_READ_WRITE_ROLEPERM");
      break;
  }
}

/**************************************************************************
 Function Name : cm_ldsv_load_roles_and_permission 
 Input Args    : pFile - file pointer
                 pTransChannel - transport channel
                 bRoleAtTemplate - boolean variable to figureout roles are at template level
                                   or instance level 
 Output Args   : none
 Description   : This method is used to load roles and permissions at instance level and 
                  template level. 
 Return Values : OF_FAILURE/OF_SUCCESS
 *************************************************************************/
int32_t cm_ldsv_load_roles_and_permission (FILE *pFile, void* pTransChannel,
                                       unsigned char bRoleAtTemplate)
{
  UCMDM_RolePermission_t RolePerm = {};
  uint32_t ii = 0, ix=0;
  char aReadLine[LDSV_XML_READ_LINE];
  char aDmPath[LDSV_DMPATH_SIZE];
  char *pString = NULL, *pTempString = NULL;
  char *pResult = NULL;
  char *pResultArry[1000];
  unsigned char bPermissionFilled = FALSE;
  struct cm_result *pUCMResult = NULL;

  memset (aReadLine, 0, sizeof(aReadLine));
  memset (pResultArry, 0, sizeof(pResultArry));
  pString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);
  pTempString = cm_ldsv_trim_string (pString);
  if ((pTempString == NULL) || strlen(pTempString) == 0)
  {
    CM_LDSV_DEBUG_PRINT ("Nothing to load, may be empty data file");
    return OF_FAILURE;
  }
  do
  {
    if (strcmp (pTempString, "<row>") == 0)
    {
      ix = 0;
      ii = 0;
      pTempString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);
      pTempString = cm_ldsv_trim_string (pTempString);
      while (!(strcmp (pTempString, "</row>") == 0))
      {
        pResult = cm_ldsv_string_tokenize (pTempString, "<>");
        while (pResult != NULL)
        {
          pResultArry[ix] = pResult;
          ix++;
          pResult = cm_ldsv_string_tokenize (NULL, "<>");
          ii++;
        }
 
        for (ix=0; ix<ii; ix++)
        {
          if ((pResultArry[ix] != NULL) && strlen(pResultArry[ix]) > 0)
          {
            if (strcmp(pResultArry[ix], "dmpath") == 0)
            {
              strcpy(aDmPath, pResultArry[ix+1]);
            }
            else if(strcmp(pResultArry[ix], "role") == 0)
            {
              strcpy(RolePerm.ucRole, (char*)pResultArry[ix+1]);              
            }
            else if(strcmp(pResultArry[ix], "permission") == 0)
            {
              RolePerm.uiPermissions = cm_get_permission_value(pResultArry[ix+1]); 
              bPermissionFilled = TRUE;
            }
            if (bPermissionFilled == TRUE)
            {
              if (bRoleAtTemplate == TRUE)
              {
                CM_LDSV_DEBUG_PRINT("aDmPath=%s:RolePerm.uiPermissions=%d",aDmPath,RolePerm.uiPermissions);
                if(UCMDM_SetRolePermissionsByRole (pTransChannel,
                                           CM_LDSV_MGMT_ENGINE_ID, aDmPath,
                                           CM_VORTIQA_SUPERUSER_ROLE, &RolePerm, &pUCMResult) != OF_SUCCESS)
                {
                   CM_LDSV_DEBUG_PRINT("UCMDM_SetRolePermissionsByRole failed\n\r");
                   //return OF_FAILURE;
                }
              }
              else if(bRoleAtTemplate == FALSE)
              {
                CM_LDSV_DEBUG_PRINT("aDmPath=%s:RolePerm.uiPermissions=%d",aDmPath,RolePerm.uiPermissions);
                if(UCMDM_SetInstanceRolePermissionsByRole (pTransChannel,
                                              CM_LDSV_MGMT_ENGINE_ID,
                                              aDmPath, CM_VORTIQA_SUPERUSER_ROLE,
                                              &RolePerm) != OF_SUCCESS)
                {
                  CM_LDSV_DEBUG_PRINT("UCMDM_SetInstanceRolePermissionsByRole failed\n\r");
                  //return OF_FAILURE;
                }
              } 
              bPermissionFilled = FALSE;
             }
          }
        }
        memset (aReadLine, 0, sizeof(aReadLine));
        pTempString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);
        pTempString = cm_ldsv_trim_string (pTempString);
        memset (pResultArry, 0, sizeof(pResultArry));
      }
    }
    pString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);    /* get one line from the file */
    pTempString = cm_ldsv_trim_string (pString);
  } while (pString != NULL);
 return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cm_get_permission_value 
 Input Args    : uiPerm - permission value
 Output Args   : permission value in the form of integer value
 Description   : This method is used to get integer permission value using permission string
 Return Values : permission value in the form of integer value 
 *************************************************************************/

 int32_t cm_get_permission_value(char* pPerm)
{
  if (strcmp("CM_PERMISSION_NOACCESS", pPerm) == 0)
     return CM_PERMISSION_NOACCESS;

  else if (strcmp("CM_PERMISSION_READ_ONLY", pPerm) == 0)
     return CM_PERMISSION_READ_ONLY;

  else if (strcmp("CM_PERMISSION_READ_WRITE", pPerm) == 0)
     return CM_PERMISSION_READ_WRITE;

  else if (strcmp("CM_PERMISSION_READ_ROLEPERM", pPerm) == 0)
     return CM_PERMISSION_READ_ROLEPERM;

  else if (strcmp("CM_PERMISSION_READ_WRITE_ROLEPERM", pPerm) == 0)
     return CM_PERMISSION_READ_WRITE_ROLEPERM;
  return CM_PERMISSION_NOACCESS; //CID 158 
}
#endif /*CM_ROLES_PERM_SUPPORT*/

/**************************************************************************
 Function Name : cm_ldsv_get_version_number_from_file
 Input Args    : pFile - file pointer
 Output Args   : pVersionNo - version no
                 pullVersion - combination of saved engine version and configuration version
 Description   : This method is used to get integer permission value using permission string
 Return Values : OF_FAILURE/OF_SUCCESS
 *************************************************************************/
int32_t cm_ldsv_get_version_number_from_file (FILE *pFile, char* pVersionNo, uint64_t *pullVersion)
{
  //uint32_t ii = 0, ix=0;
  char aReadLine[LDSV_XML_READ_LINE], aTempVersion[100];
  //char *pString = NULL, *pTempString = NULL;
  unsigned char *pResultArry, aResultArray[100];
  uint32_t uiSavedEngConfigVer=0,uiSavedConfigVer=0;

  *pullVersion = 0;
  memset (aReadLine, 0, sizeof(aReadLine));
  memset (aResultArray, 0, sizeof(aResultArray));
  pResultArry = aResultArray;
  pResultArry = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);
  if (pResultArry == NULL)
  {
    CM_LDSV_DEBUG_PRINT ("Nothing to load, may be empty data file");
    return OF_FAILURE;
  }

  strcpy(aTempVersion,pResultArry);
 
  strcpy(pVersionNo, cm_ldsv_trim_string(aTempVersion));
  if((sscanf(pResultArry, "%lu.%lu",&uiSavedEngConfigVer, &uiSavedConfigVer))< 2)
  {
    return OF_FAILURE;
  }
  *pullVersion=  uiSavedEngConfigVer;
  *pullVersion =  *pullVersion<<32 |uiSavedConfigVer;
#if 0 
  pString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);
  pTempString = cm_ldsv_trim_string (pString);
  if (pTempString == NULL)
  {
    CM_LDSV_DEBUG_PRINT ("Nothing to load, may be empty data file");
    return OF_FAILURE;
  }
  do
  {
    if (strcmp (pTempString, "<row>") == 0)
    {
      ix = 0;
      ii = 0;
      pTempString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);
      pTempString = cm_ldsv_trim_string (pTempString);
      while (!(strcmp (pTempString, "</row>") == 0))
      {
        pResult = cm_ldsv_string_tokenize (pTempString, "<>");
        while (pResult != NULL)
        {
          pResultArry[ix] = pResult;
          ix++;
          pResult = cm_ldsv_string_tokenize (NULL, "<>");
          ii++;
        }
 
        for (ix=0; ix<ii; ix++)
        {
          if ((pResultArry[ix] != NULL) && strlen(pResultArry[ix]) > 0)
          {
            if (strcmp(pResultArry[ix], "version") == 0)
            {
              strcpy(pVersionNo, pResultArry[ix+1]);
              return OF_SUCCESS;
            }
          }
        }
        memset (aReadLine, 0, sizeof(aReadLine));
        pTempString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);
        pTempString = cm_ldsv_trim_string (pTempString);
        memset (pResultArry, 0, sizeof(pResultArry));
      }
    }
    pString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);    /* get one line from the file */
    pTempString = cm_ldsv_trim_string (pString);
  } while (pString != NULL);
  return OF_FAILURE;
#endif /* 0 */
  return OF_SUCCESS;
}


/**************************************************************************
 Function Name : cm_ldsv_prepare_load_instance_dm_path 
 Input Args    : pDMNodeAttrib - data model node Attributes
                 dm_path_pArrayOfKeys - Array of name value pairs
                 pLdsvCfgStackDllQ - Table stack node
                 pRefPath - Reference data model path
 Output Args   : pdmPath - data model instacne path
 Description   : This method is used to prepare instance path using name value pairs and 
                 table stack node
 Return Values : OF_FAILURE/OF_SUCCESS
 *************************************************************************/
int32_t cm_ldsv_prepare_load_instance_dm_path (struct cm_dm_node_info * pDMNodeAttrib,
                                       char* pdmPath,
                                       struct cm_array_of_nv_pair *dm_path_pArrayOfKeys,
                                       UCMDllQ_t *pLdsvCfgStackDllQ,
                                       char *pRefPath)
{
  char *pToken = NULL;
  char aDmPath[LDSV_DMPATH_SIZE];
  char aTmpPath[LDSV_DMPATH_SIZE];
  char *pTmname_pPath = NULL;
  char *pDMTplPath;
  uint32_t uiIndex = 0;
  uint32_t uiMaxTokenLen, uiTokenLen;
  uint32_t uiOpaqueInfoLen = 0;
  int32_t iRet = CM_LDSV_DM_PATH_PARSING_REMAINING;
  int32_t iRetVal;
  int32_t iTreeRet;
  int32_t iKeyCnt = 0;
  void *pOpaqueInfo = NULL;
  void *pTransportChannel = NULL;
  struct cm_role_info RoleInfo = { "VortiQa", CM_VORTIQA_SUPERUSER_ROLE };
  struct cm_array_of_structs *pChildInfoArr = NULL;
  uint32_t   ii,jj;
  uint32_t uiKeyCnt=0;
  struct cm_dm_node_info *pKeyChild=NULL;

  struct cm_dm_node_info *pDmNodeAttrib = NULL;
  struct cm_array_of_nv_pair *pArrayOfKeys = NULL;

  /*pArrayOfKeys = cm_ldsv_collect_keys_from_table_stack (pLdsvCfgStackDllQ,
                                                      dm_path_pArrayOfKeys);
  if (pArrayOfKeys == NULL)
  {
    CM_LDSV_DEBUG_PRINT ("Keys not found.");
  } */
  uiMaxTokenLen = strlen (pDMNodeAttrib->dm_path_p) + 1;
  if (uiMaxTokenLen == 1)
  {
    //cm_ldsv_clean_nv_pair_array(pArrayOfKeys, TRUE);
    return OF_FAILURE;
  }
  pTmname_pPath = (char *) of_calloc (1, uiMaxTokenLen);
  if (!pTmname_pPath)
  {
    //cm_ldsv_clean_nv_pair_array(pArrayOfKeys, TRUE);
    return OF_FAILURE;
  }
  /* Allocate token buffer */
  pToken = (char *) of_calloc (1, uiMaxTokenLen);
  if (!pToken)
  {
    //cm_ldsv_clean_nv_pair_array(pArrayOfKeys, TRUE);
    of_free (pTmname_pPath);
    return OF_FAILURE;
  }
   pTransportChannel = cm_ldsv_get_transport_channel();
  /* Validate transport channel */
  if (unlikely (pTransportChannel == NULL))
  {
    /* Create new transport channel */
    pTransportChannel =(void *) cm_tnsprt_create_channel (CM_IPPROTO_TCP, 0, 0,
                                                   UCMJE_CONFIGREQUEST_PORT);
  }
  pDMTplPath = (char *) of_calloc (1, uiMaxTokenLen);
  if (!pDMTplPath)
  {
    //cm_ldsv_clean_nv_pair_array(pArrayOfKeys, TRUE);
    of_free (pTmname_pPath);
    of_free (pToken);
    return OF_FAILURE;
  }

  memset (pToken, 0, sizeof (pToken));
  strncpy (pTmname_pPath, pDMNodeAttrib->dm_path_p, (uiMaxTokenLen - 1));

  memset (pDMTplPath, 0, sizeof (pDMTplPath));
  memset (aDmPath, 0, sizeof (aDmPath));

  if(pRefPath)
  {
     strcpy(aDmPath, pRefPath);
     cm_ldsv_remove_keys_from_path(pRefPath,aTmpPath,strlen(pRefPath));
     if(strcmp(aTmpPath,pTmname_pPath) == 0)
     {
       strcpy (pdmPath, aDmPath);
       of_free (pTmname_pPath);
       of_free (pToken);
       of_free (pDMTplPath);
       return OF_SUCCESS;
     }
     strcat(aDmPath,".");
     uiIndex = strlen(aTmpPath)+1;
     strcpy(pDMTplPath,aTmpPath);
     strcat(pDMTplPath,".");
     
  }

  while (iRet == CM_LDSV_DM_PATH_PARSING_REMAINING)
  {
    /* Tokenize the input string */
    iRet =
      cm_ldsv_get_dm_path_token (pTmname_pPath, pToken, uiMaxTokenLen, &uiIndex);
    if (pToken)
    {
      uiTokenLen = strlen (pToken);
      strncat (pDMTplPath, pToken, uiTokenLen);
      strncat (aDmPath, pToken, uiTokenLen);
    }

    iTreeRet = cm_dm_get_first_node_info (pDMTplPath,
                                       UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
                                       pTransportChannel,
                                       CM_LDSV_MGMT_ENGINE_ID,
                                       &RoleInfo,
                                       &pDmNodeAttrib, &pOpaqueInfo,
                                       &uiOpaqueInfoLen);
    if ((iTreeRet == OF_FAILURE) || (pDmNodeAttrib == NULL))
    {
      CM_LDSV_DEBUG_PRINT("Get First Node Info failed");
      cm_ldsv_clean_nv_pair_array(pArrayOfKeys, TRUE);
      of_free (pTmname_pPath);
      of_free (pToken);
      of_free (pDMTplPath);
      return OF_FAILURE;
    }

    if (pDmNodeAttrib->element_attrib.element_type == CM_DMNODE_TYPE_TABLE)
    {
      uiKeyCnt = 0;
      pArrayOfKeys = cm_ldsv_collect_keys_from_stack_by_dm_path(pDmNodeAttrib->dm_path_p, pLdsvCfgStackDllQ);
      iRetVal = cm_dm_get_key_childs (pTransportChannel, pDmNodeAttrib->dm_path_p,
                                 CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, &pChildInfoArr);
      if ((pChildInfoArr == NULL) || (iRetVal != OF_SUCCESS))
      {
        CM_LDSV_DEBUG_PRINT("Failed to get child key info");
        cm_ldsv_clean_nv_pair_array(pArrayOfKeys, TRUE);
        of_free (pTmname_pPath);
        of_free (pToken);
        of_free (pDMTplPath);
        return OF_FAILURE;
      }
      if (iRet == CM_LDSV_DM_PATH_PARSING_REMAINING)
      {
        if ((pArrayOfKeys != NULL) && (pArrayOfKeys->count_ui) > iKeyCnt)
        {
          strncat (pDMTplPath, ".", 1);
	   if(pChildInfoArr->count_ui > 0 )
          {
	    strncat(aDmPath,"{", 1);
	    for (ii=0; ii < pChildInfoArr->count_ui; ii++)
	    {
	      pKeyChild=(struct cm_dm_node_info *)(pChildInfoArr->struct_arr_p) + ii;
	      for (jj=uiKeyCnt; ((jj< pArrayOfKeys->count_ui) && (uiKeyCnt<pChildInfoArr->count_ui)); jj++)
	      {
	      	if(strcmp(pKeyChild->name_p, pArrayOfKeys->nv_pairs[jj].name_p) == 0)
	      	{
	      	  if(pChildInfoArr->count_ui > 1)
	      	  {
	      	    strncat(aDmPath, pKeyChild->name_p, strlen(pKeyChild->name_p));
	            strncat(aDmPath,"=",1);
	          }
	          strncat(aDmPath, pArrayOfKeys->nv_pairs[jj].value_p, pArrayOfKeys->nv_pairs[jj].value_length);
	      	  if(ii != (pChildInfoArr->count_ui-1 ))
	          {
	      	    strncat(aDmPath,",",1);
	      	  }
	      	 uiKeyCnt++;
	       }
	     }
           }
           strncat(aDmPath,"}",1);
           strncat (aDmPath, ".", 1);
        }

#if 0
          strncat (pDMTplPath, ".", 1);
          strncat (aDmPath, "{", 1);
          strncat (aDmPath, pArrayOfKeys->nv_pairs[iKeyCnt].value_p,
                     pArrayOfKeys->nv_pairs[iKeyCnt].value_length);
          strncat (aDmPath, "}", 1);
          strncat (aDmPath, ".", 1);
          iKeyCnt = iKeyCnt+1;
#endif
        }
        else
        {
          strncat (pDMTplPath, ".", 1);
          strncat (aDmPath, ".", 1);
        }
      }
      // every time build fresh array of nv pairs
      cm_ldsv_clean_nv_pair_array(pArrayOfKeys, FALSE);
    }
    else
    {
      if (iRet == CM_LDSV_DM_PATH_PARSING_REMAINING)
      {
        strncat (aDmPath, ".", 1);
        strncat (pDMTplPath, ".", 1);
      }
    }
    cm_ldsv_clean_tree_traversal_opaque_info (pOpaqueInfo, uiOpaqueInfoLen);
    cm_ldsv_clean_dm_node_attrib_info (pDmNodeAttrib);
  }
  strcpy (pdmPath, aDmPath);
  of_free (pTmname_pPath);
  of_free (pToken);
  of_free (pDMTplPath);
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cm_ldsv_collect_keys_from_stack_by_dm_path 
 Input Args    : dm_path_p - data model template tree path
                 pTableStackDll- Table stack node
 Output Args   : Array of keys in the form of name value pairs
 Description   : This method is used to get keys from table stack using template tree path
 Return Values : OF_FAILURE/OF_SUCCESS
 *************************************************************************/
struct cm_array_of_nv_pair *cm_ldsv_collect_keys_from_stack_by_dm_path(char* dm_path_p, 
                                                  UCMDllQ_t* pTableStackDll)
                                           
{
  int32_t i = 0;//, j=0;
  int32_t iCount = 0;
  int32_t iRet;
  struct cm_array_of_nv_pair *array_of_key_nv_pairs_p = NULL;
  uint32_t uiInstCount = 0;

  UCMDllQNode_t *pNode;
  cm_ldsv_table_stack_node_t *pTmpStackNode;
  cm_ldsv_table_stack_node_t *pTableStackNode;

  /* allocate memory */
  array_of_key_nv_pairs_p =
    (struct cm_array_of_nv_pair *) of_calloc (1, sizeof (struct cm_array_of_nv_pair));
  if (unlikely (!array_of_key_nv_pairs_p))
  {
    return NULL;
  }

  array_of_key_nv_pairs_p->count_ui = 0;
  array_of_key_nv_pairs_p->nv_pairs = NULL;

  if (CM_DLLQ_COUNT (pTableStackDll) != 0)
  {
    /* find no of nodes present in TableNodeStack */
    /* collect the keys of parent table nodes */
    CM_DLLQ_SCAN (pTableStackDll, pNode, UCMDllQNode_t *)
    {
      pTmpStackNode =
        CM_DLLQ_LIST_MEMBER (pNode, cm_ldsv_table_stack_node_t *, ListNode);
       // pTableStackNode->pDMNodeAttrib->dm_path_p
      if (pTmpStackNode == NULL)
        return array_of_key_nv_pairs_p;
      if (strcmp(pTmpStackNode->dm_path_p, dm_path_p)==0)
      {
        uiInstCount = uiInstCount + pTmpStackNode->ActiveInstKeys.count_ui;
      }
    }
    array_of_key_nv_pairs_p->count_ui = uiInstCount;
    //array_of_key_nv_pairs_p->count_ui = 1; //uiInstCount;

    /* Allocate memory */
    array_of_key_nv_pairs_p->nv_pairs =
      (struct cm_nv_pair *) of_calloc (array_of_key_nv_pairs_p->count_ui, sizeof (struct cm_nv_pair));
    if (array_of_key_nv_pairs_p->nv_pairs == NULL)
    {
      /* We cant handle save operation any more. Quit from here */
      of_free (array_of_key_nv_pairs_p);
      return NULL;
    }

    /* Now collect the keys of parent table nodes */
    CM_DLLQ_SCAN (pTableStackDll, pNode, UCMDllQNode_t *)
    {
      pTableStackNode =
        CM_DLLQ_LIST_MEMBER (pNode, cm_ldsv_table_stack_node_t *, ListNode);

      if (strcmp(pTableStackNode->dm_path_p, dm_path_p)==0)
      {
        for (i = 0; i < pTableStackNode->ActiveInstKeys.count_ui; i++)
        {
          /* Prepare Keys Array entry */
          iRet = cm_ldsv_fill_nv_pair_info (&(array_of_key_nv_pairs_p->nv_pairs[iCount]),
                                       pTableStackNode->ActiveInstKeys.nv_pairs[i].name_p,
                                       pTableStackNode->ActiveInstKeys.nv_pairs[i].value_type,
                                       pTableStackNode->ActiveInstKeys.nv_pairs[i].value_length,
                                       pTableStackNode->ActiveInstKeys.nv_pairs[i].value_p);
          if (unlikely (iRet == OF_FAILURE))
          {
            cm_ldsv_clean_nv_pair_array (array_of_key_nv_pairs_p, FALSE);
            return NULL;
          }
          iCount = iCount + 1;
          //array_of_key_nv_pairs_p->count_ui = 1;
          //return array_of_key_nv_pairs_p;
        }
      }
    }
    array_of_key_nv_pairs_p->count_ui = iCount;
  }
  return array_of_key_nv_pairs_p;
}

/**************************************************************************
 Function Name : cm_ldsv_delete_load_stack_node_using_dm_path 
 Input Args    : dm_path_p - data model template tree path
                 pTableStackDll- Table stack node
 Output Args   : none
 Description   : This method is used to delete name value pairs in a stack node by datamodel path
 Return Values : OF_FAILURE/OF_SUCCESS
 *************************************************************************/
int32_t cm_ldsv_delete_load_stack_node_using_dm_path (UCMDllQ_t * pTableStackDll, char* dm_path_p)
{
  cm_ldsv_table_stack_node_t *pTmpTableStackNode;

  UCMDllQNode_t *pNode;
  uint32_t ulCount;
  int32_t ii;
  
  ulCount = CM_DLLQ_COUNT(pTableStackDll);  
  if ((pTableStackDll != NULL) && ulCount > 0)
  {
    for (ii=0; ii<ulCount; ii++)
    {
      pNode = CM_DLLQ_LAST (pTableStackDll);
      pTmpTableStackNode =
        CM_DLLQ_LIST_MEMBER (pNode, cm_ldsv_table_stack_node_t*, ListNode);
      if (strcmp(pTmpTableStackNode->dm_path_p, dm_path_p) == 0)
      {
        CM_LDSV_DEBUG_PRINT ("Deleting instance key:%s",
                              (char*)pTmpTableStackNode->ActiveInstKeys.nv_pairs->value_p);
        CM_DLLQ_DELETE_NODE (pTableStackDll, &(pTmpTableStackNode->ListNode));
      }
    }
  }
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cm_ldsv_delete_one_stack_node_using_dm_path 
 Input Args    : dm_path_p - data model template tree path
                 pTableStackDll- Table stack node
                 pDMNodeAttrib - Datamodel node Attributes
                 pTransportChannel - Transport channel
 Output Args   : pbMultiInstance - boolean variable to find whether the node has multiple instance or 
                 single instance at a particular data model path level
 Description   : This method is used to delete one stack node and name value pairs in a stack node by datamodel path
                 and find whether the node has multiple instance or single instance at a particular data model path level
 Return Values : OF_FAILURE/OF_SUCCESS
 *************************************************************************/

int32_t cm_ldsv_delete_one_stack_node_using_dm_path (UCMDllQ_t * pTableStackDll, char* dm_path_p, 
               struct cm_dm_node_info *pDMNodeAttrib, void *pTransportChannel, 
               unsigned char *pbMultiInstance)
{
  cm_ldsv_table_stack_node_t *pTmpTableStackNode;

  UCMDllQNode_t *pNode;
  uint32_t ulCount;
  int32_t iNodeCount=0;
  unsigned char bIsChildNode;
  struct cm_role_info RoleInfo = { "VortiQa", CM_VORTIQA_SUPERUSER_ROLE };
  
  ulCount = CM_DLLQ_COUNT(pTableStackDll);  
  if ((pTableStackDll != NULL) && ulCount > 0)
  {
     CM_DLLQ_SCAN (pTableStackDll, pNode, UCMDllQNode_t *)
    {
      pTmpTableStackNode =
        CM_DLLQ_LIST_MEMBER (pNode, cm_ldsv_table_stack_node_t*, ListNode);
      if (strcmp(pTmpTableStackNode->dm_path_p, dm_path_p) == 0)
      {
        iNodeCount++;
      }
    }
    if (iNodeCount > 1)
    {
      //Delete one instance
       CM_DLLQ_SCAN (pTableStackDll, pNode, UCMDllQNode_t *)
      {
        pTmpTableStackNode =
          CM_DLLQ_LIST_MEMBER (pNode, cm_ldsv_table_stack_node_t*, ListNode);
        if (strcmp(pTmpTableStackNode->dm_path_p, dm_path_p) == 0)
        {
          CM_LDSV_DEBUG_PRINT ("Deleting instance key:%s",
                              (char*)pTmpTableStackNode->ActiveInstKeys.nv_pairs->value_p);
          CM_DLLQ_DELETE_NODE (pTableStackDll, &(pTmpTableStackNode->ListNode));
          *pbMultiInstance = TRUE;
          break;
        }
      }
    }
    else if (iNodeCount == 1)
    {
      CM_DLLQ_SCAN (pTableStackDll, pNode, UCMDllQNode_t *)
      {
      pTmpTableStackNode =
          CM_DLLQ_LIST_MEMBER (pNode, cm_ldsv_table_stack_node_t*, ListNode);
      if (strcmp(pTmpTableStackNode->dm_path_p, dm_path_p) == 0)
      {
        CM_LDSV_DEBUG_PRINT ("Deleting instance key:%s",
                            (char*)pTmpTableStackNode->ActiveInstKeys.nv_pairs->value_p);
        CM_DLLQ_DELETE_NODE (pTableStackDll, &(pTmpTableStackNode->ListNode));
       if (CM_DLLQ_COUNT(pTableStackDll) > 0)
       { 
         pNode = CM_DLLQ_LAST (pTableStackDll);
         pTmpTableStackNode =
            CM_DLLQ_LIST_MEMBER (pNode, cm_ldsv_table_stack_node_t*, ListNode);
        if (pTmpTableStackNode != NULL)
        {
          if (pDMNodeAttrib != NULL)
          {
            bIsChildNode = cm_dm_is_child_element (pDMNodeAttrib->dm_path_p,
                                   pTmpTableStackNode->dm_path_p,
                                   pTransportChannel,
                                   CM_LDSV_MGMT_ENGINE_ID,
                                   &RoleInfo);
          }
          else
          {
            bIsChildNode = FALSE;
          }
          if (bIsChildNode == FALSE)
          {
            cm_ldsv_delete_one_stack_node_using_dm_path (pTableStackDll, pTmpTableStackNode->dm_path_p, 
                                       pDMNodeAttrib, pTransportChannel, pbMultiInstance);
          }
        }
       }
      }
      }
    }
  }
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cm_ldsv_load_table_in_table_file
 Input Args    : pFile - File pointer
                 DMNodeAttrib - Data Model Attributes
                 pTransportChannel - Transport channel
                 LoadTableStackNode - table stack node 
                 pOpaqueInfo - opaque info
                 uiOpaqueInfoLen - opaque info length
                 bGroupedScalar - boolean varible to find grouped scalar or not
                 pConfigSess - Configuration session
 Output Args   : none
 Description   : This method is used to Load the given xml file
 Return Values : OF_FAILURE/OF_SUCCESS
*************************************************************************/
int32_t cm_ldsv_load_table_in_table_file (FILE *pFile,
                          struct cm_dm_node_info* DMNodeAttrib,
                          void* pOpaqueInfo,
                          uint32_t uiOpaqueInfoLen,
                          void* pTransportChannel,
                          UCMDllQ_t *LoadTableStackNode, unsigned char bGroupedScalar,
                          struct cm_je_config_session *pConfigSess)
{

  int32_t i = 0, iRet;
  struct cm_dm_node_info *pTmpNodeInfo;
  struct cm_role_info RoleInfo = { "VortiQa", CM_VORTIQA_SUPERUSER_ROLE };
  char * pFileName=NULL;
  uint32_t ii = 0, ij = 0;
  char aReadLine[LDSV_XML_READ_LINE]="";
  char aDmPath[LDSV_DMPATH_SIZE]="";
  char *pResult = NULL;
  char *pResultArry[1000];
  char *pString = NULL, *pTempString = NULL;
  char *pTempToken1 = NULL;
  char *pTempToken2 = NULL;
  char *pTempToken3 = NULL;
  char aDictionary[100]="";
  char aTempInstPathFromFile[512]="";
  char *pDictionary=NULL;
  char *pInstPathFromFile=NULL;
  char *pdmPathFromFile=NULL;
  struct cm_hash_table *pTablInTabHashTable;
  struct cm_array_of_nv_pair* array_of_nv_pairs_p;
  unsigned char bDataFound=FALSE;
  struct cm_array_of_structs *pChildInfoArr = NULL;
  UCMDllQ_t TransStackNode;
  //UCMDllQ_t *pTransStackDllQ = &TransStackNode;
  cm_ldsv_table_stack_node_t *pStackNode;
  struct cm_array_of_nv_pair* pKeyNvPair;


  pInstPathFromFile = (char *) of_calloc (1, LDSV_DMPATH_SIZE);
  pdmPathFromFile = (char *) of_calloc (1,LDSV_DMPATH_SIZE);
  if ((pInstPathFromFile == NULL) || (pdmPathFromFile == NULL))
  {
    CM_LDSV_DEBUG_PRINT (" Unable to create memory");
    if(pInstPathFromFile)         /*CID 368*/
      of_free(pInstPathFromFile);
    if(pdmPathFromFile)
      of_free(pdmPathFromFile); /*CID 365*/
    return OF_FAILURE;
   }

  pTablInTabHashTable = cm_hash_create (MAX_HASH_SIZE);
  if (pTablInTabHashTable == NULL)
  {
    CM_LDSV_DEBUG_PRINT ("Dictonary Table is not created");
    of_free (pInstPathFromFile);
    of_free (pdmPathFromFile);
    return OF_FAILURE;
  }
          memset(pInstPathFromFile,0,strlen(pInstPathFromFile));
          if(cm_ldsv_get_dm_path_and_instance_path_from_save_file (pFile, pdmPathFromFile, pInstPathFromFile) != OF_SUCCESS)
          {
            CM_LDSV_DEBUG_PRINT ("Failed to get dm instance path from xml file");
            of_free(pdmPathFromFile); /*CID 365*/
            of_free(pInstPathFromFile);
            cm_hash_free (pTablInTabHashTable, NULL);
            return OF_FAILURE;
          }
          memset(aTempInstPathFromFile, 0, strlen(aTempInstPathFromFile));
          strncpy(aTempInstPathFromFile, pInstPathFromFile, strlen(pInstPathFromFile));
          CM_LDSV_DEBUG_PRINT ("pFileName=%s,aDmPath=%s,pInstPathFromFile=%s:",pFileName,aDmPath,pInstPathFromFile);
          cm_ldsv_tokenize_dictionary (pFile, pTablInTabHashTable);
          array_of_nv_pairs_p =
                  (struct cm_array_of_nv_pair *) of_calloc (1, sizeof (struct cm_array_of_nv_pair));
          array_of_nv_pairs_p->count_ui = cm_hash_size (pTablInTabHashTable);
          array_of_nv_pairs_p->nv_pairs =
                   (struct cm_nv_pair *) of_calloc (array_of_nv_pairs_p->count_ui,
                                              sizeof (struct cm_nv_pair));
           if (array_of_nv_pairs_p->nv_pairs == NULL)
           {
             of_free (array_of_nv_pairs_p);
             of_free(pInstPathFromFile);
             of_free(pdmPathFromFile); /*CID 365*/
             cm_hash_free (pTablInTabHashTable, NULL);
             return OF_FAILURE;
           }
        if (DMNodeAttrib->element_attrib.element_type == CM_DMNODE_TYPE_TABLE)
        {
          cm_dm_get_key_childs (pTransportChannel, DMNodeAttrib->dm_path_p,
                       CM_LDSV_MGMT_ENGINE_ID, &RoleInfo, &pChildInfoArr);
         }
          do
          {
            if (pTempString != NULL)
            {
              if (strcmp (pTempString, "<row>") == 0)
              {
 	            i = 0;
		        strncpy(pInstPathFromFile, aTempInstPathFromFile, strlen(aTempInstPathFromFile));
                pTempString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);
                pTempString = cm_ldsv_trim_string (pTempString);
                // tokenize for one complete record and fill nv pairs
                while (!(strcmp (pTempString, "</row>") == 0))
                {
                  ii = 0;
                  pTempToken1 = strtok (pTempString, "<");
                  pTempToken2 = strchr (pTempToken1, '>');
  
                  pTempToken2[0] = '\0';
                  pTempToken3 = pTempToken2 + 1;
  
                  pDictionary = (char *) cm_hash_lookup (pTablInTabHashTable, pTempToken1);
                  memset(aDictionary, 0, sizeof(aDictionary));
                  if(pDictionary)
                    strcpy(aDictionary, pDictionary); //CID 394
                  CM_LDSV_DEBUG_PRINT ("%s::%s::%s=%s",pTempToken1,pTempToken2,pTempToken3,pDictionary);
                  if (pDictionary == NULL)
                  {
                    pTempString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);
                    pTempString = cm_ldsv_trim_string (pTempString);
                    continue;
                  }
                  pResult = cm_ldsv_string_tokenize (aDictionary, " =");//String tokenize using 'space and equal to'
                  while (pResult != NULL)
                  {
                    pResultArry[ii] = pResult;
                    ii++;
                    pResult = cm_ldsv_string_tokenize (NULL, " =");
                  }
                  iRet = cm_ldsv_fill_nv_pair_info (&(array_of_nv_pairs_p->nv_pairs[i]), pResultArry[0], //Name
                                                 atoi (pResultArry[2]), //ValueType,
                                                 strlen (pTempToken3),  //ValueLen
                                                 (void *) pTempToken3); //Value
                  if (unlikely (iRet == OF_FAILURE))
                  {
                    CM_LDSV_DEBUG_PRINT ("Failed to fill name value pairs.");
                    cm_ldsv_clean_nv_pair_array (array_of_nv_pairs_p, TRUE);
                    of_free(pInstPathFromFile);
                    of_free(pdmPathFromFile); /*CID 365*/
                    cm_hash_free (pTablInTabHashTable, NULL);
                    return OF_FAILURE;
                  }
        
                  i = i + 1;
                  memset (aReadLine, 0, sizeof(aReadLine));
                  pTempString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);
                  pTempString = cm_ldsv_trim_string (pTempString);
                  bDataFound = TRUE;
                }
                array_of_nv_pairs_p->count_ui = i;
                memset(aDmPath, 0, sizeof(aDmPath));
                if (DMNodeAttrib->element_attrib.element_type == CM_DMNODE_TYPE_TABLE)
                {
                strncat(aDmPath,"{", 1);
                for (ij = 0; ij < pChildInfoArr->count_ui; ij++)
                {
                  pTmpNodeInfo = (struct cm_dm_node_info *) (pChildInfoArr->struct_arr_p) + ij;
                  if (pTmpNodeInfo->element_attrib.element_type ==
                      CM_DMNODE_TYPE_SCALAR_VAR)
                  {
                    for (ii=0; ii< array_of_nv_pairs_p->count_ui; ii++)
	            {
                      if(strcmp(pTmpNodeInfo->name_p, array_of_nv_pairs_p->nv_pairs[ii].name_p) == 0)
	      	      {
                        if(pChildInfoArr->count_ui > 1)
	      	        {
                          strncat(aDmPath, pTmpNodeInfo->name_p, strlen(pTmpNodeInfo->name_p));
                          strncat(aDmPath,"=",1);
	                }
	                strncat(aDmPath, array_of_nv_pairs_p->nv_pairs[ii].value_p, array_of_nv_pairs_p->nv_pairs[ii].value_length);
	      	        if(ij != (pChildInfoArr->count_ui-1))
	                {
	      	          strncat(aDmPath,",",1);
	      	        }
	              }
	            }
                  }
                 }
                  strncat(aDmPath,"}",1);
	              strncat (pInstPathFromFile, aDmPath, strlen(aDmPath));
                 }
                 CM_LDSV_DEBUG_PRINT ("pInstPathFromFile=%s:",pInstPathFromFile);
                 if(frame_and_send_to_je(pConfigSess, pInstPathFromFile, array_of_nv_pairs_p, bGroupedScalar) == OF_SUCCESS)
                 {
                   if (bGroupedScalar != TRUE)
                   {
                   pKeyNvPair = cm_ldsv_find_key_nv_pair (DMNodeAttrib, array_of_nv_pairs_p,
                                                       pTransportChannel);
                   if (pKeyNvPair != NULL)
                   {
                     pStackNode = cm_ldsv_create_table_stack_node (DMNodeAttrib, pdmPathFromFile,
                                                    DMNodeAttrib->name_p, pOpaqueInfo,
                                                    uiOpaqueInfoLen, pKeyNvPair);
                     if (pStackNode == NULL)
                     {
                       CM_LDSV_DEBUG_PRINT ("Failed to create stack");
                       cm_ldsv_clean_nv_pair_array (array_of_nv_pairs_p, TRUE);
                       return OF_FAILURE;
                     }
                     CM_DLLQ_APPEND_NODE (LoadTableStackNode, &(pStackNode->ListNode));
                   }
                 }
                 }
                 memset(pInstPathFromFile,0,strlen(pInstPathFromFile));
              }
            }
            pString = fgets (aReadLine, LDSV_XML_READ_LINE, pFile);    /* get one line from the file */
            pTempString = cm_ldsv_trim_string (pString);
          } while (pString != NULL);
     cm_ldsv_clean_nv_pair_array (array_of_nv_pairs_p, TRUE);
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name : cm_ldsv_remove_keys_from_path 
 Input Args    : pInPath - instance tree path
                 uiPathLen - instance tree path length
 Output Args   : pOutPath - instance tree path after removing instances
 Description   : This method is used to remove instances in instance tree path
 Return Values : OF_FAILURE/OF_SUCCESS
 *************************************************************************/
int32_t cm_ldsv_remove_keys_from_path (char * pInPath, char * pOutPath,
                                  uint32_t uiPathLen)
{
  uint32_t i;
  uint32_t j = 0;
  unsigned char bSkip = FALSE;

  if (unlikely ((pInPath == NULL) || (pOutPath == NULL)))
  {
    return OF_FAILURE;
  }

  // uiPathLen = strlen(pInPath);
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


#endif /* CM_LDSV_SUPPORT */
