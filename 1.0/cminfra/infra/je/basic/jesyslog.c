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
 * File name: jesyslog.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/25/2013
 * Description: This file has the code which does frame the and sends the
 *              syslog messages
*/

#ifdef CM_SUPPORT
#ifdef CM_JE_SUPPORT
#ifdef CM_AUDIT_SUPPORT

#include "jeincld.h"
#include "jeldef.h"
#include "jever.h"
#include "jeldef.h"
#include "jelif.h"
#include "evntgdef.h"
#include "evntgif.h"
#include "dmldef.h"
#include "dmlif.h"
#ifndef VORTIQA_SYSLOG_SUPPORT
#include "ucmmesg.h"
#endif
#include "jesyslog.h"
#include "jeigwlog.h"
#include "jemsgid.h"

#define CM_DM_MAX_DEFAULT_ELEMENT_VALLEN 50
#define CM_JE_MAX_REFFERENCE_LEN 100
#define CM_JE_MAX_DMPATH_LEN 100
#define CM_JE_DISPLAY_FRIENDLY_NAMES_IN_LOGMESGS
unsigned char bIpFound=FALSE;
char aIpAddress_g[128]={};

/******************************************************************************
 * * * * * * * * * * * * * * * FUNCTION PROTOTYPES * * * * * * * * * * * * * * 
 *****************************************************************************/

 int32_t
JEMsgsGetMsgInfo (uint32_t ulMsgId, UCMMSGSThrtMsgIdInfo_t * pJEMsgIdInfo);

 int32_t JEPrepareLogMessageCfgData(char *dm_path_p, 
      struct cm_array_of_nv_pair *arr_nv_pair_p, UCMJELogInfo_t *pJESpecificInfo);

/****************************************************************************
 ** FunctionName  : UCMJEFrameAndSendLogMsg
 ** Description   : This Function frames  JE Log message and send it.
 ** Input         : ulMsgId:
 ** Output        : NONE
 ** Return value  : OF_SUCCESS/OF_FAILURE
 *****************************************************************************/
void UCMJEFrameAndSendLogMsg (uint32_t ulMsgId,char *admin_name_p,char * admin_role_p,
      uint32_t command_ui, char * dm_path_p,  char * pInstance,struct cm_array_of_nv_pair *arr_nv_pair_p)
{
   UCMLogMessage_t *pJELogMessage = NULL;
   UCMJELogInfo_t *pJESpecificInfo = NULL;
   UCMMSGSThrtMsgIdInfo_t *pJEMsgIdInfo = NULL;
   struct cm_dm_data_element *pDMNode = NULL;
   FILE *fp;
   char aSysCmd[128];
   int32_t iMsgRetVal;
   uint32_t uiNvCnt, uiLen=0;
   char *name_p = NULL;

   pDMNode = (struct cm_dm_data_element *)
      cmi_dm_get_dm_node_from_instance_path (dm_path_p, of_strlen (dm_path_p));
   if (!pDMNode)
   {
      CM_JE_DEBUG_PRINT ("pDMNode is NULL");
      return ;
   }


   pJEMsgIdInfo =
      (UCMMSGSThrtMsgIdInfo_t *) of_calloc (1, sizeof (UCMMSGSThrtMsgIdInfo_t));

   if (!pJEMsgIdInfo)
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation Failed for pJEMsgIdInfo");
      return;
   }

   iMsgRetVal = JEMsgsGetMsgInfo (ulMsgId, pJEMsgIdInfo);

   if (iMsgRetVal != OF_SUCCESS)  /* if MsgId not get the message Info */
   {
      CM_JE_DEBUG_PRINT ("Message Id Not found in defined messages");
      of_free (pJEMsgIdInfo);
      return;
   }
   if (pJEMsgIdInfo->usSeverity > JE_LOG_LEVEL)
   {
      CM_JE_DEBUG_PRINT ("Message loglevel >");
      of_free (pJEMsgIdInfo);
      return;
   }
   switch (pJEMsgIdInfo->ulMsgFamilyId)
   {
      case CM_LOG_FAMILY_JE:
         {
            pJELogMessage = (UCMLogMessage_t *)UCMAllocLogMessageBuf (sizeof (UCMJELogInfo_t));
            if (NULL == pJELogMessage)
            {
               of_free (pJEMsgIdInfo);
               return;
            }
            pJESpecificInfo =
               (UCMJELogInfo_t *) pJELogMessage->MessageFamilySpecificInfo;
            if (admin_name_p)
            {
               of_strncpy (pJESpecificInfo->admin_name, admin_name_p,
                     CM_JE_LOGDB_FIELD_UCMADMIN_LEN);
            }

            if (admin_role_p)
            {
               of_strncpy (pJESpecificInfo->admin_role, admin_role_p,
                     CM_JE_LOGDB_FIELD_UCMROLE_LEN);
            }
            if (dm_path_p)
            {
               of_strncpy (pJESpecificInfo->dmpath_a, dm_path_p,
                     CM_JE_LOGDB_FIELD_DMPATH_LEN);
            }

            if (pInstance)
            {
               of_strncpy (pJESpecificInfo->aInstance, pInstance,
                     CM_JE_LOGDB_FIELD_DMINSTANCE_LEN);
            }
            if(arr_nv_pair_p)
            {
#ifdef CM_JE_DISPLAY_FRIENDLY_NAMES_IN_LOGMESGS
               if(dm_path_p)
               {
                  if( JEPrepareLogMessageCfgData(dm_path_p, arr_nv_pair_p, pJESpecificInfo) != OF_SUCCESS)
                     CM_JE_DEBUG_PRINT(" JEPrepareLogMessageCfgData failed");
               } 
               else
#endif
               {
                  for(uiNvCnt=0; uiNvCnt < arr_nv_pair_p->count_ui; uiNvCnt++)
                  {
                     uiLen += arr_nv_pair_p->nv_pairs[uiNvCnt].name_length + 1;
                     uiLen += arr_nv_pair_p->nv_pairs[uiNvCnt].value_length + 1;

                     if(uiLen >= CM_JE_LOGDB_FIELD_NVPAIRS_LEN)
                     {
                        CM_JE_DEBUG_PRINT("Exceeded Max Log Message Length");
                        break; 
                     }
                     if(arr_nv_pair_p->nv_pairs[uiNvCnt].name_length > 0 )  
                     {
                        of_strncat(pJESpecificInfo->aCfgData,name_p,of_strlen((char *)name_p));
                        of_strncat(pJESpecificInfo->aCfgData,"=",1);
                     }
                     if(arr_nv_pair_p->nv_pairs[uiNvCnt].value_length > 0 )  
                     {
                        of_strncat(pJESpecificInfo->aCfgData,arr_nv_pair_p->nv_pairs[uiNvCnt].value_p,
                              of_strlen((char *)arr_nv_pair_p->nv_pairs[uiNvCnt].value_p));
                     }

                     of_strncat(pJESpecificInfo->aCfgData,";",1);
                  }
               }
            }
            switch (command_ui)
            {
               case CM_CMD_ADD_PARAMS:
                  of_strncpy (pJESpecificInfo->aCommand, "Add",
                        CM_JE_LOGDB_FIELD_COMMAND_LEN);
                  break;
               case CM_CMD_SET_PARAMS:
                  of_strncpy (pJESpecificInfo->aCommand, "Modify",
                        CM_JE_LOGDB_FIELD_COMMAND_LEN);
                  break;
               case CM_CMD_DEL_PARAMS:
                  of_strncpy (pJESpecificInfo->aCommand, "Delete",
                        CM_JE_LOGDB_FIELD_COMMAND_LEN);
                  break;
            }
            if (bIpFound == FALSE)
            {
               sprintf(aSysCmd,"cat /var/state/dhcp/dhclient.leases | grep fixed-address | cut -d \" \" -f 4 | cut -d \";\" -f 1 >%s", CM_VORTIQA_DEVIP_FILE);
               system(aSysCmd);
               fp = fopen (CM_VORTIQA_DEVIP_FILE, "r");
               if (!fp)
               {
                  perror ("fopen");
                  CM_JE_DEBUG_PRINT("device ip file open  failed");
               }
               else
               {
                  fgets(aIpAddress_g,128,fp);
                  if (aIpAddress_g[0] != '\0')
                  {
                     CM_JE_DEBUG_PRINT("Ip address %s found", aIpAddress_g);
                     bIpFound=TRUE;
                  }
                  fclose(fp);
               }
            }
            if (bIpFound== TRUE)
            {
               of_strncpy (pJESpecificInfo->aIpAddress, aIpAddress_g,
                     CM_JE_LOGDB_FIELD_COMMAND_LEN);
            }
            CM_JE_DEBUG_PRINT("Ip=%s Admin=%s Role=%s Cmd=%s DMPath=%s Instance=%s ",
                  pJESpecificInfo->aIpAddress,pJESpecificInfo->admin_name,
                  pJESpecificInfo->admin_role, pJESpecificInfo->aCommand,
                  pJESpecificInfo->dmpath_a, pJESpecificInfo->aInstance);
            if(pDMNode->friendly_name_p != NULL)
               of_strncpy(pJESpecificInfo->aTableName,pDMNode->friendly_name_p,of_strlen(pDMNode->friendly_name_p));
            else
               of_strncpy(pJESpecificInfo->aTableName,pDMNode->name_p,of_strlen(pDMNode->name_p));
         }

         break;
   }

   if (pJELogMessage)
   {
      // pJELogMessage->usSeverity = pJEMsgIdInfo->usSeverity;
      pJELogMessage->usCategory = pJEMsgIdInfo->usCategory;
      pJELogMessage->ulMsgId = pJEMsgIdInfo->ulMsgId;
      pJELogMessage->ulMsgFamily = pJEMsgIdInfo->ulMsgFamilyId;
      pJELogMessage->ulMsgSubFamily = pJEMsgIdInfo->ulSubFamilyId;
      if (of_strlen (pJEMsgIdInfo->ucMsgDesc) > 0)
      {
         of_strncpy (pJELogMessage->pMesgDesc, pJEMsgIdInfo->ucMsgDesc,
               CM_JE_LOGDB_FIELD_MSGDESC_LEN);
      }
      CM_JE_DEBUG_PRINT("Id=%d Msg=%s MsgSize=%d",pJELogMessage->ulMsgId, pJEMsgIdInfo->ucMsgDesc,
            pJELogMessage->ulFamilySize);
      UCMLogMessage (pJELogMessage);
   }

   of_free (pJEMsgIdInfo);
   return;
}

/****************************************************************************
 ** FunctionName  : JEMsgsGetMsgInfo 
 ** Description   : This Function Gets Message Information for given Message Id.
 ** Input         : ulMsgId - Message Id
 ** Output        : pJEMsgIdInfo - Pointer to JE Message Info
 ** Return value  : OF_SUCCESS/OF_FAILURE
 *****************************************************************************/
    int32_t
JEMsgsGetMsgInfo (uint32_t ulMsgId, UCMMSGSThrtMsgIdInfo_t * pJEMsgIdInfo)
{
   uint32_t ulIndex = 0;
   uint32_t ulMaxMsgId = sizeof (JEMsgIdInfo) / sizeof (UCMMSGSThrtMsgIdInfo_t);

   for (ulIndex = 0; ulIndex < ulMaxMsgId; ulIndex++)
   {
      if (ulMsgId == JEMsgIdInfo[ulIndex].ulMsgId)
      {
         of_memcpy (pJEMsgIdInfo, &(JEMsgIdInfo[ulIndex]),
               sizeof (UCMMSGSThrtMsgIdInfo_t));
         return OF_SUCCESS;
      }
   }
   return OF_FAILURE;
}

#ifdef CM_JE_DISPLAY_FRIENDLY_NAMES_IN_LOGMESGS
 int32_t 
JEPrepareLogMessageCfgData(char *dm_path_p, struct cm_array_of_nv_pair *arr_nv_pair_p, UCMJELogInfo_t *pJESpecificInfo)
{
   unsigned char bEnumFound;
   unsigned char bNodeFound;
   struct cm_dm_data_element *pDMNode = NULL;
   struct cm_dm_node_info *tmp_node_info_p = NULL;
   struct cm_array_of_structs *pChildInfoArr = NULL;
   uint32_t uindex_i, uiNvCnt, uiLen=0, uiEnumCnt;
   int32_t return_value;
   char *pTemvalue_p=NULL, *name_p=NULL;

   //   CM_JE_DEBUG_PRINT("Entered");
   if ((!arr_nv_pair_p)||(!dm_path_p))
   {
      CM_JE_DEBUG_PRINT ("invalid input");
      return OF_FAILURE;
   }

   pDMNode = (struct cm_dm_data_element *)
      cmi_dm_get_dm_node_from_instance_path (dm_path_p, of_strlen (dm_path_p));
   if (!pDMNode)
   {
      CM_JE_DEBUG_PRINT ("pDMNode is NULL");
      return OF_FAILURE;
   }

   return_value = cmi_dm_create_child_info_array (pDMNode, &pChildInfoArr);
   if (unlikely (return_value != OF_SUCCESS))
   {
      CM_JE_DEBUG_PRINT ("Child array is NULL");
      return OF_FAILURE;
   }

   for(uiNvCnt=0; uiNvCnt < arr_nv_pair_p->count_ui; uiNvCnt++)
   {
      bEnumFound=FALSE;
      bNodeFound=FALSE;
      for (uindex_i = 0; uindex_i < pChildInfoArr->count_ui; uindex_i++)
      {
         tmp_node_info_p = (struct cm_dm_node_info *) (pChildInfoArr->struct_arr_p) + uindex_i;
         if(of_strcmp(arr_nv_pair_p->nv_pairs[uiNvCnt].name_p,tmp_node_info_p->name_p)==0)
         {
            bNodeFound=TRUE;
            break;
         }
      }
      uiLen += (arr_nv_pair_p->nv_pairs[uiNvCnt].value_length + 1);
      if(bNodeFound == TRUE)
      {
         if(tmp_node_info_p->friendly_name_p != NULL)
         {  
            name_p =tmp_node_info_p->friendly_name_p;
         }
         else
         {
            name_p = arr_nv_pair_p->nv_pairs[uiNvCnt].name_p;
         }
      }
      else
      {
         CM_JE_DEBUG_PRINT ("Node not  found");
         name_p = arr_nv_pair_p->nv_pairs[uiNvCnt].name_p;
      }

      uiLen += (of_strlen((char *)name_p)+1);
      uiLen += (arr_nv_pair_p->nv_pairs[uiNvCnt].value_length +1);
      if(uiLen >= CM_JE_LOGDB_FIELD_NVPAIRS_LEN)
      {
         CM_JE_DEBUG_PRINT("Exceeded Max Log Message Length");
         break; 
      }
      of_strncat(pJESpecificInfo->aCfgData,name_p,of_strlen((char *)name_p)+1);
      of_strncat(pJESpecificInfo->aCfgData,"=",1);

      if((bNodeFound == TRUE) && (tmp_node_info_p->data_attrib.attrib_type == CM_DATA_ATTRIB_STR_ENUM))
      {
         uiEnumCnt=0;
         for (uiEnumCnt=0;
               (uiEnumCnt < tmp_node_info_p->data_attrib.attr.string_enum.count_ui); uiEnumCnt++)
         {
            if (of_strcmp(arr_nv_pair_p->nv_pairs[uiNvCnt].value_p,
                     tmp_node_info_p->data_attrib.attr.string_enum.array[uiEnumCnt])==0)
            {
               if(tmp_node_info_p->data_attrib.attr.string_enum.aFrdArr[uiEnumCnt] != NULL)
               {
                  if(of_strlen(tmp_node_info_p->data_attrib.attr.string_enum.aFrdArr[uiEnumCnt])!=0)
                  {
                     CM_JE_DEBUG_PRINT ("enum found");
                     pTemvalue_p = tmp_node_info_p->data_attrib.attr.string_enum.aFrdArr[uiEnumCnt];
                     uiLen += (of_strlen(pTemvalue_p) +2) ;
                     if(uiLen >= CM_JE_LOGDB_FIELD_NVPAIRS_LEN)
                     {
                        CM_JE_DEBUG_PRINT("Exceeded Max Log Message Length");
                        break; 
                     }
                     of_strncat(pJESpecificInfo->aCfgData,pTemvalue_p, of_strlen(pTemvalue_p)+1);
                     of_strncat(pJESpecificInfo->aCfgData,"(",1);
                     bEnumFound=TRUE;
                  }
               }
               break;
            }
         }
      }

      if(arr_nv_pair_p->nv_pairs[uiNvCnt].value_length > 0 )  
      {
         of_strncat(pJESpecificInfo->aCfgData,arr_nv_pair_p->nv_pairs[uiNvCnt].value_p,
               arr_nv_pair_p->nv_pairs[uiNvCnt].value_length+1);
      }

      if(bEnumFound == TRUE)
      {
         of_strncat(pJESpecificInfo->aCfgData,")",1);
      }

      of_strncat(pJESpecificInfo->aCfgData,";",1);
   }
   CM_JE_DEBUG_PRINT("Success CfgData=%s",pJESpecificInfo->aCfgData);
   return OF_SUCCESS;
}
#endif
#endif
#endif
#endif
