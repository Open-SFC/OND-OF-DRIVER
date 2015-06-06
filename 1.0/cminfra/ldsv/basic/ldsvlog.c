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
 * File name:   ldsvlog.c
 * Author:      Freescale Semiconductor    
 * Date:        06/12/2013
 * Description: Frames and sends the syslog messages.
*/

#ifdef CM_LDSV_SUPPORT
#ifdef CM_AUDIT_SUPPORT

#include "ldsvinc.h"
#ifndef VORTIQA_SYSLOG_SUPPORT
#include "ucmmesg.h"
#endif
#include "jesyslog.h"
#include "jeigwlog.h"
#include "jemsgid.h"

#define CM_DM_MAX_DEFAULT_ELEMENT_VALLEN 50
#define CM_JE_MAX_REFFERENCE_LEN 100
#define CM_JE_MAX_DMPATH_LEN 100
/******************************************************************************
 * * * * * * * * * * * * * * * FUNCTION PROTOTYPES * * * * * * * * * * * * * * 
 *****************************************************************************/

 int32_t
ldsv_get_message_info (uint32_t ulMsgId, UCMMSGSThrtMsgIdInfo_t * pLDSVMsgIdInfo);

/****************************************************************************
 ** FunctionName  : cm_ldsv_frame_and_send_log_message
 ** Description   : This Function frames  JE Log message and send it.
 ** Input         : ulMsgId:
 ** Output        : NONE
 ** Return value  : OF_SUCCESS/OF_FAILURE
 *****************************************************************************/
void cm_ldsv_frame_and_send_log_message (uint32_t ulMsgId,char *pAdminName,char * pAdminRole,
      uint32_t ulCommand, char * dm_path_p,  char * pInstance,struct cm_array_of_nv_pair *pArrNvPair, char *pReason, uint64_t ulVersion)
{
   UCMLogMessage_t *pLDSVLogMessage = NULL;
   UCMJELogInfo_t *pUCMSpecificInfo = NULL;
   UCMMSGSThrtMsgIdInfo_t *pLDSVMsgIdInfo = NULL;
   int32_t iMsgRetVal;
   uint32_t uiNvCnt;
   uint32_t uiLen = 0;
   char *name_p = NULL;
   char aVersion[32];

   pLDSVMsgIdInfo =
      (UCMMSGSThrtMsgIdInfo_t *) of_calloc (1, sizeof (UCMMSGSThrtMsgIdInfo_t));
   if (!pLDSVMsgIdInfo)
   {
      CM_LDSV_DEBUG_PRINT ("Memory Allocation Failed for pLDSVMsgIdInfo");
      return;
   }

   iMsgRetVal = ldsv_get_message_info (ulMsgId, pLDSVMsgIdInfo);
   if (iMsgRetVal != OF_SUCCESS)  /* if MsgId not get the message Info */
   {
      CM_LDSV_DEBUG_PRINT ("Message Id Not found in defined messages");
      of_free (pLDSVMsgIdInfo);
      return;
   }

   if (pLDSVMsgIdInfo->usSeverity > JE_LOG_LEVEL)
   {
      CM_LDSV_DEBUG_PRINT ("Message loglevel >");
      of_free (pLDSVMsgIdInfo);
      return;
   }
   switch (pLDSVMsgIdInfo->ulMsgFamilyId)
   {
      case CM_LOG_FAMILY_JE:
         {
            pLDSVLogMessage = (UCMLogMessage_t *)UCMAllocLogMessageBuf (sizeof (UCMJELogInfo_t));
            if (NULL == pLDSVLogMessage)
            {
               of_free (pLDSVMsgIdInfo);
               return;
            }
            pUCMSpecificInfo =
               (UCMJELogInfo_t *) pLDSVLogMessage->MessageFamilySpecificInfo;
            if (pAdminName)
            {
               strncpy (pUCMSpecificInfo->aAdminName, pAdminName,
                     CM_JE_LOGDB_FIELD_UCMADMIN_LEN);
            }

            if (pAdminRole)
            {
               strncpy (pUCMSpecificInfo->admin_role, pAdminRole,
                     CM_JE_LOGDB_FIELD_UCMROLE_LEN);
            }
            if (dm_path_p)
            {
               strncpy (pUCMSpecificInfo->aDMPath, dm_path_p,
                     CM_JE_LOGDB_FIELD_DMPATH_LEN);
            }

            if (pInstance)
            {
               strncpy (pUCMSpecificInfo->aInstance, pInstance,
                     CM_JE_LOGDB_FIELD_DMINSTANCE_LEN);
            }
            if(pArrNvPair)
            {
               for(uiNvCnt=0; uiNvCnt < pArrNvPair->count_ui; uiNvCnt++)
               {
                  if(pArrNvPair->NvPairs[uiNvCnt].name_length > 0 )  
                  {  
                     if(pArrNvPair->NvPairs[uiNvCnt].friendly_name_p)
                     {
                        name_p = pArrNvPair->NvPairs[uiNvCnt].friendly_name_p;
                     }
                     else
                     {
                        name_p = pArrNvPair->NvPairs[uiNvCnt].name_p;
                     }
                  }

                  if(pArrNvPair->NvPairs[uiNvCnt].uiEnumValLen > 0)
                  {
                     uiLen += pArrNvPair->NvPairs[uiNvCnt].uiEnumValLen+2;
                  }

                  uiLen += strlen((char *)name_p) + 1;
                  uiLen += pArrNvPair->NvPairs[uiNvCnt].value_length + 1;

                  if(uiLen >= CM_JE_LOGDB_FIELD_NVPAIRS_LEN)
                  {
                     CM_LDSV_DEBUG_PRINT("Exceeded Max Log Message Length");
                     break; 
                  }

                  strncat(pUCMSpecificInfo->aCfgData,name_p,strlen((char *)name_p));
                  strncat(pUCMSpecificInfo->aCfgData,"=",1);

                  if(pArrNvPair->NvPairs[uiNvCnt].uiEnumValLen > 0)
                  {
                     strncat(pUCMSpecificInfo->aCfgData,pArrNvPair->NvPairs[uiNvCnt].pEnumValue,
                           strlen((char *)pArrNvPair->NvPairs[uiNvCnt].pEnumValue));
                     strncat(pUCMSpecificInfo->aCfgData,"(",1);
                  }   

                  if(pArrNvPair->NvPairs[uiNvCnt].value_length > 0 )  
                  {
                     strncat(pUCMSpecificInfo->aCfgData,pArrNvPair->NvPairs[uiNvCnt].value_p,
                           strlen((char *)pArrNvPair->NvPairs[uiNvCnt].value_p));
                  }

                  if(pArrNvPair->NvPairs[uiNvCnt].uiEnumValLen > 0)
                  {
                     strncat(pUCMSpecificInfo->aCfgData,")",1);
                  }

                  strncat(pUCMSpecificInfo->aCfgData,";",1);
               }
            }
            else
            {
               CM_READ_VERSION(ulVersion,aVersion);
               sprintf(pUCMSpecificInfo->aCfgData,"%s",aVersion);
            }
            if (pReason)
            {
               strncpy(pUCMSpecificInfo->aReason, pReason, CM_JE_LOGDB_FIELD_REASON_LEN);
            }
            switch (ulCommand)
            {
               case CM_CMD_LOAD_CONFIG:
                  strncpy (pUCMSpecificInfo->aCommand, "Load",
                        CM_JE_LOGDB_FIELD_COMMAND_LEN);
                  break;
							 case CM_CMD_SAVE_CONFIG:
                  strncpy (pUCMSpecificInfo->aCommand, "Save",
                        CM_JE_LOGDB_FIELD_COMMAND_LEN);
                  break;
							 case CM_CMD_RESET_TO_FACTORY_DEFAULTS:
                  strncpy (pUCMSpecificInfo->aCommand, "Factory-Reset",
                        CM_JE_LOGDB_FIELD_COMMAND_LEN);
                  break;
            }
            CM_LDSV_DEBUG_PRINT("Admin=%s Role=%s Cmd=%s DMPath=%s Instance=%s ",
                  pUCMSpecificInfo->aAdminName,pUCMSpecificInfo->admin_role,
                  pUCMSpecificInfo->aCommand,  pUCMSpecificInfo->aDMPath,
                  pUCMSpecificInfo->aInstance);
         }

         break;
   }

   if (pLDSVLogMessage)
   {
      // pLDSVLogMessage->usSeverity = pLDSVMsgIdInfo->usSeverity;
      pLDSVLogMessage->usCategory = pLDSVMsgIdInfo->usCategory;
      pLDSVLogMessage->ulMsgId = pLDSVMsgIdInfo->ulMsgId;
      pLDSVLogMessage->ulMsgFamily = pLDSVMsgIdInfo->ulMsgFamilyId;
      pLDSVLogMessage->ulMsgSubFamily = pLDSVMsgIdInfo->ulSubFamilyId;
      if (strlen (pLDSVMsgIdInfo->ucMsgDesc) > 0)
      {
         strncpy (pLDSVLogMessage->pMesgDesc, pLDSVMsgIdInfo->ucMsgDesc,
               CM_JE_LOGDB_FIELD_MSGDESC_LEN);
      }
      CM_LDSV_DEBUG_PRINT("Id=%ld Msg=%s MsgSize=%ld",pLDSVLogMessage->ulMsgId, pLDSVMsgIdInfo->ucMsgDesc,
            pLDSVLogMessage->ulFamilySize);
      UCMLogMessage (pLDSVLogMessage);
   }

   of_free (pLDSVMsgIdInfo);
   return;
}

/****************************************************************************
 ** FunctionName  : ldsv_get_message_info 
 ** Description   : This Function Gets Message Information for given Message Id.
 ** Input         : ulMsgId - Message Id
 ** Output        : pLDSVMsgIdInfo - Pointer to JE Message Info
 ** Return value  : OF_SUCCESS/OF_FAILURE
 *****************************************************************************/
    int32_t
ldsv_get_message_info (uint32_t ulMsgId, UCMMSGSThrtMsgIdInfo_t * pLDSVMsgIdInfo)
{
   uint32_t ulIndex = 0;
   uint32_t ulMaxMsgId = sizeof (JEMsgIdInfo) / sizeof (UCMMSGSThrtMsgIdInfo_t);

   for (ulIndex = 0; ulIndex < ulMaxMsgId; ulIndex++)
   {
      if (ulMsgId == JEMsgIdInfo[ulIndex].ulMsgId)
      {
         memcpy (pLDSVMsgIdInfo, &(JEMsgIdInfo[ulIndex]),
               sizeof (UCMMSGSThrtMsgIdInfo_t));
         return OF_SUCCESS;
      }
   }
   return OF_FAILURE;
}
#endif
#endif
