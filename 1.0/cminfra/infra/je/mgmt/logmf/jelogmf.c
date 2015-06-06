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
 * File name: jelogmf.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/30/2013
 * Description: Log Controller and Syslog Formatting
*/

#ifdef CM_SUPPORT
#ifndef VORTIQA_SYSLOG_SUPPORT
#ifdef CM_JE_SUPPORT
#ifdef CM_AUDIT_SUPPORT

#include "jeincld.h"
#include "jeldef.h"
#include "jelif.h"
#include "ucmmesg.h"
#include "logcltrgdef.h"
#include "jesyslog.h"
#include "jeigwlog.h"
#include "jemsgid.h"


int32_t
UCMJELogCltrFormat (UCMLogMessage_t * pLogMessageInfo,
                      void * pTempLogField, uint32_t * pTotalFields);
int32_t
UCMJEMCFormatFn (UCMLogMessage_t * pLogMessageInfo,
                   unsigned char * pLogBuf, uint32_t * pulLen_p);

/******************************************************************************
 * * * * * * * * * * * * * * * FUNCTION PROTOTYPES * * * * * * * * * * * * * * 
 *****************************************************************************/

void UCMMsgClntReg_Init(void);

 int32_t
JEMsgsGetMsgInfo (uint32_t ulMsgId, UCMMSGSThrtMsgIdInfo_t * pJEMsgIdInfo);

void UCMJERegisterSubfamilyInfo (void);

/******************************************************************************
 * * *  * * * * * * JE MSGTHROTNG GLOBAL STRUCTURES * * * * * * * * * * * * *
 ******************************************************************************/

UCMMSGSThrtParams_t JEMsgThrtParams =
  { CM_JE_MSGTHRT_TIME, CM_JE_MSGTHRT_COUNT,
  CM_JE_MSGTHRT_ISSUE_IPMASK
};

void UCMMsgClntReg_Init(void)
{
	 CM_DEBUG_PRINT ("JE Log Message Family Init");
	 UCMJEMCRegFormaters();
}

/****************************************************************************
 ** FunctionName  : UCMJEMCRegFormaters
 ** Description   : This function is used to register the JE Logcontroller
 **                 and Syslog formatting functions to LogMessage formater
 ** Input         : NONE
 ** Output        : NONE
 ** Return value  : OF_SUCCESS/OF_FAILURE
 ****************************************************************************/
void UCMJEMCRegFormaters (void)
{
  UCMMSGSFormatterRegInfo_t JEMsgFmtrRegInfo;
  UCMMSGSFamilyRegInfo_t JEMsgFamilyRegInfo;

  CM_DEBUG_PRINT ("Registering Formaters");
  of_memset (&JEMsgFamilyRegInfo, 0, sizeof (JEMsgFamilyRegInfo));
  of_memset (&JEMsgFmtrRegInfo, 0, sizeof (JEMsgFmtrRegInfo));

  JEMsgFamilyRegInfo.ulMessageFamilyId = CM_LOG_FAMILY_JE;
  of_strcpy (JEMsgFamilyRegInfo.ucMsgFamilyName, CM_JE_MSG_FAMILY_NAME);
  JEMsgFamilyRegInfo.ulMaxSubFamilies = 2;

  if (UCMMSGSRegisterFamily (&JEMsgFamilyRegInfo) == OF_FAILURE)
  {
    CM_DEBUG_PRINT ("UCMMSGSRegisterFamily failed");
    return;
  }

  JEMsgFmtrRegInfo.ulMessageFamily = CM_LOG_FAMILY_JE;
  JEMsgFmtrRegInfo.pLogCltrFormatter = UCMJELogCltrFormat;
  JEMsgFmtrRegInfo.pMCFormatter = UCMJEMCFormatFn;

  if (UCMMSGSRegisterFormatters (&JEMsgFmtrRegInfo) != OF_SUCCESS)
  {
    CM_DEBUG_PRINT ("UCMRegisterMessageFamliy failed");
  }
  UCMJERegisterSubfamilyInfo ();
  UCMJEMsgsRegInit ();
  CM_DEBUG_PRINT ("Exit");
  return;
}

/****************************************************************************
 ** FunctionName  : UCMJERegisterSubfamilyInfo
 ** Description   : This function is used to register the JE Log Sub Family
 **                 and its Status
 ** Input         : NONE
 ** Output        : NONE
 ** Return value  : NONE
 ****************************************************************************/
void UCMJERegisterSubfamilyInfo (void)
{
  UCMMSGSSubFamilyRegInfo_t MessageSubFamilyRegInfo;
  UCMMSGSSetSubFamilyStatus_t MessageSubFamilyStatus;

	CM_DEBUG_PRINT("Registering SubFamily");

	of_memset (&MessageSubFamilyRegInfo, 0, sizeof (UCMMSGSSubFamilyRegInfo_t));
  MessageSubFamilyRegInfo.ulMsgFamilyId = CM_LOG_FAMILY_JE;
  MessageSubFamilyRegInfo.ulSubFamilyId = CM_LOG_JE_SUBFAMILY_CONFIG_UPDATE;
  of_strcpy (MessageSubFamilyRegInfo.ucSubFamilyName,
            CM_JE_CONFIG_UPDATE_SUBFAMILY_NAME);
  if (UCMMSGSRegisterSubFamily (&MessageSubFamilyRegInfo) == OF_FAILURE)
  {
    CM_DEBUG_PRINT ("UCMMSGSRegisterSubFamily-ConfigUpdate failed");
    return;
  }

  of_memset (&MessageSubFamilyStatus, 0, sizeof (UCMMSGSSetSubFamilyStatus_t));
  MessageSubFamilyStatus.ulMsgFamilyId = CM_LOG_FAMILY_JE;
  MessageSubFamilyStatus.ulSubFamilyId = CM_LOG_JE_SUBFAMILY_CONFIG_UPDATE;
  MessageSubFamilyStatus.status_b = TRUE;
  if (UCMMSGSSetSubFamilyStatus (&MessageSubFamilyStatus) == OF_FAILURE)
  {
    CM_DEBUG_PRINT ("UCMMSGSSetSubFamilyStatus-ConfigUpdate failed");
  }

	of_memset (&MessageSubFamilyRegInfo, 0, sizeof (UCMMSGSSubFamilyRegInfo_t));
  MessageSubFamilyRegInfo.ulMsgFamilyId = CM_LOG_FAMILY_JE;
  MessageSubFamilyRegInfo.ulSubFamilyId = CM_LOG_SUBFAMILY_LDSV;
  of_strcpy (MessageSubFamilyRegInfo.ucSubFamilyName,
             CM_LOG_LDSV_SUBFAMILY_NAME);
  if (UCMMSGSRegisterSubFamily (&MessageSubFamilyRegInfo) == OF_FAILURE)
  {
    CM_DEBUG_PRINT ("UCMMSGSRegisterSubFamily-LDSV failed");
    return;
  }

  of_memset (&MessageSubFamilyStatus, 0, sizeof (UCMMSGSSetSubFamilyStatus_t));
  MessageSubFamilyStatus.ulMsgFamilyId = CM_LOG_FAMILY_JE;
  MessageSubFamilyStatus.ulSubFamilyId = CM_LOG_SUBFAMILY_LDSV;
  MessageSubFamilyStatus.status_b = TRUE;
  if (UCMMSGSSetSubFamilyStatus (&MessageSubFamilyStatus) == OF_FAILURE)
  {
    CM_DEBUG_PRINT ("UCMMSGSSetSubFamilyStatus-LDSV failed");
  }

  return;
}


/****************************************************************************
 ** FunctionName  : UCMJEMsgsRegInit
 ** Description   : This Function frames  JE Log message and send it.
 ** Input         : NONE
 ** Output        : NONE
 ** Return value  : OF_SUCCESS/OF_FAILURE
 *****************************************************************************/
void UCMJEMsgsRegInit (void)
{
  UCMMSGSThrtMsgIdRegInfo_t UCMJEMsgsThrtRegInfo = { };

	CM_DEBUG_PRINT("Message Registration");
  UCMJEMsgsThrtRegInfo.ulNumOfMsgIds = CM_JE_MAX_MSGIDS;
//    sizeof (JEMsgIdInfo) / sizeof (UCMMSGSThrtMsgIdInfo_t);
  UCMJEMsgsThrtRegInfo.msg_id_info_p = JEMsgIdInfo;

  if (UCMMSGSRegisterMsgIds (&UCMJEMsgsThrtRegInfo) != OF_SUCCESS)
  {
    CM_DEBUG_PRINT ("Registration MsgIds Failed");
  }

  return;
}

/****************************************************************************
 ** FunctionName  : UCMJELogCltrFormat
 ** Description   : This is the JE Log Controller formatting funtion
 ** Input         : pLogMessageInfo
 		  : pTempLogField
 		  : pTotalFields
 ** Output        : NONE
 ** Return value  : OF_SUCCESS/OF_FAILURE
 ****************************************************************************/
int32_t
UCMJELogCltrFormat (UCMLogMessage_t * pLogMessageInfo,
                    void * pTempLogField, uint32_t * pTotalFields)
{
  UCMLogMessage_t *pJELogMsgInfo = pLogMessageInfo;
  UCMJELogInfo_t *pJESpcificLogMsg = NULL;
  UCMLogCltrDbLogField_t *pJELogDbField = NULL;

	CM_DEBUG_PRINT("Formating Log Message for Data Base");
  if ((!pLogMessageInfo) || (!pTempLogField))
  {
    CM_DEBUG_PRINT ("Invalid Input");
    return OF_FAILURE;
  }

  pJELogDbField = (UCMLogCltrDbLogField_t *) pTempLogField;

  /* should be the message family id */
  pJELogDbField->ulTableIndex = CM_LOG_FAMILY_JE;
  pJELogDbField->ulFieldId = CM_JE_TABLE_DB_MSG_ID;
  pJELogDbField->ulFieldType = CM_LOG_CLTR_DB_FID_TYPE_INTEGER;
  pJELogDbField->pucFieldVal = (unsigned char *) (pJELogMsgInfo->ulMsgId);
  pJELogDbField->ulFieldLen = sizeof (uint32_t);
  pJELogDbField++;
  (*pTotalFields)++;            /* incrmenting for each field */
#if 0
  pJELogDbField->ulTableIndex = CM_LOG_FAMILY_JE;
  pJELogDbField->ulFieldId = CM_JE_TABLE_DB_SEVERITY;
  pJELogDbField->ulFieldType = CM_LOG_CLTR_DB_FID_TYPE_SHORT_INTEGER;
  pJELogDbField->pucFieldVal =
    (unsigned char *) ((uint32_t) pJELogMsgInfo->usSeverity);
  pJELogDbField->ulFieldLen = sizeof (uint16_t);
  pJELogDbField++;
  (*pTotalFields)++;
#endif
  pJELogDbField->ulTableIndex = CM_LOG_FAMILY_JE;
  pJELogDbField->ulFieldId = CM_JE_TABLE_DB_MSGDESC;
  pJELogDbField->ulFieldType = CM_LOG_CLTR_DB_FID_TYPE_STRING;
  pJELogDbField->pucFieldVal = (pJELogMsgInfo->pMesgDesc);
  pJELogDbField->ulFieldLen = of_strlen (pJELogMsgInfo->pMesgDesc);
  pJELogDbField++;
  (*pTotalFields)++;

  /* Message Family specific information */
  pJESpcificLogMsg =
    (UCMJELogInfo_t *) pJELogMsgInfo->MessageFamilySpecificInfo;

  pJELogDbField->ulTableIndex = CM_LOG_FAMILY_JE;
  pJELogDbField->ulFieldId = CM_JE_TABLE_DB_UCMADMIN;
  pJELogDbField->ulFieldType = CM_LOG_CLTR_DB_FID_TYPE_STRING;
  pJELogDbField->pucFieldVal = (pJESpcificLogMsg->admin_name);
  pJELogDbField->ulFieldLen = of_strlen (pJESpcificLogMsg->admin_name);
  pJELogDbField++;
  (*pTotalFields)++;

  pJELogDbField->ulTableIndex = CM_LOG_FAMILY_JE;
  pJELogDbField->ulFieldId = CM_JE_TABLE_DB_UCMROLE;
  pJELogDbField->ulFieldType = CM_LOG_CLTR_DB_FID_TYPE_STRING;
  pJELogDbField->pucFieldVal = (pJESpcificLogMsg->admin_role);
  pJELogDbField->ulFieldLen = of_strlen (pJESpcificLogMsg->admin_role);
  pJELogDbField++;
  (*pTotalFields)++;

  pJELogDbField->ulTableIndex = CM_LOG_FAMILY_JE;
  pJELogDbField->ulFieldId = CM_JE_TABLE_DB_DMPATH;
  pJELogDbField->ulFieldType = CM_LOG_CLTR_DB_FID_TYPE_STRING;
  pJELogDbField->pucFieldVal = (pJESpcificLogMsg->dmpath_a);
  pJELogDbField->ulFieldLen = of_strlen (pJESpcificLogMsg->dmpath_a);
  pJELogDbField++;
  (*pTotalFields)++;

  pJELogDbField->ulTableIndex = CM_LOG_FAMILY_JE;
  pJELogDbField->ulFieldId = CM_JE_TABLE_DB_COMMAND;
  pJELogDbField->ulFieldType = CM_LOG_CLTR_DB_FID_TYPE_STRING;
  pJELogDbField->pucFieldVal = (pJESpcificLogMsg->aCommand);
  pJELogDbField->ulFieldLen = of_strlen (pJESpcificLogMsg->aCommand);
  pJELogDbField++;
  (*pTotalFields)++;

  pJELogDbField->ulTableIndex = CM_LOG_FAMILY_JE;
  pJELogDbField->ulFieldId = CM_JE_TABLE_DB_DMINSTANCE;
  pJELogDbField->ulFieldType = CM_LOG_CLTR_DB_FID_TYPE_STRING;
  pJELogDbField->pucFieldVal = (pJESpcificLogMsg->aInstance);
  pJELogDbField->ulFieldLen = of_strlen (pJESpcificLogMsg->aInstance);
  pJELogDbField++;
  (*pTotalFields)++;

  pJELogDbField->ulTableIndex = CM_LOG_FAMILY_JE;
  pJELogDbField->ulFieldId = CM_JE_TABLE_DB_CONFIGDATA;
  pJELogDbField->ulFieldType = CM_LOG_CLTR_DB_FID_TYPE_STRING;
  pJELogDbField->pucFieldVal = (pJESpcificLogMsg->aCfgData);
  pJELogDbField->ulFieldLen = of_strlen (pJESpcificLogMsg->aCfgData);
  pJELogDbField++;
  (*pTotalFields)++;

  pJELogDbField->ulTableIndex = CM_LOG_FAMILY_JE;
  pJELogDbField->ulFieldId = CM_JE_TABLE_DB_TABLENAME;
  pJELogDbField->ulFieldType = CM_LOG_CLTR_DB_FID_TYPE_STRING;
  pJELogDbField->pucFieldVal = (pJESpcificLogMsg->aTableName);
  pJELogDbField->ulFieldLen = of_strlen (pJESpcificLogMsg->aTableName);
  pJELogDbField++;
  (*pTotalFields)++;
  return OF_SUCCESS;

}

/****************************************************************************
 ** FunctionName  : UCMJEMCFormatFn
 ** Description   : This is the JE Syslog message formatting function
 ** Input         : pJELogMessage
 		  : pLogBuf
 		  : pulLen_p
 ** Output        : NONE
 ** Return value  : OF_SUCCESS/OF_FAILURE
 *****************************************************************************/
int32_t
UCMJEMCFormatFn (UCMLogMessage_t * pJELogMessage,
                 unsigned char * pLogBuf, uint32_t * pulLen_p)
{

  uint32_t ulTempLen;
  uint32_t ulLen;
  uint32_t ulTotLen = 0;
  UCMJELogInfo_t *pJESpecificInfo = NULL;
#ifdef CM_COMMON_DEBUG
  unsigned char *pTmp=pLogBuf;
#endif

	CM_DEBUG_PRINT("Formating Log Message for Sys Log");
  if ((!pJELogMessage) || (!pLogBuf) || (!pulLen_p))
  {
    CM_DEBUG_PRINT ("Invalid Input");
    return OF_FAILURE;
  }

  ulTempLen = *pulLen_p;
  ulLen = snprintf (pLogBuf, CM_MSGID_DESCRIPTION_LEN + 1, "message=\"%s\"",
                      pJELogMessage->pMesgDesc);
  if (ulLen >= ulTempLen)
  {
    *pulLen_p = ulTotLen;
    return OF_SUCCESS;
  }
  ulTempLen = ulTempLen - ulLen;
  pLogBuf += ulLen;
  ulTotLen += ulLen;

  pJESpecificInfo =
    (UCMJELogInfo_t *) (pJELogMessage->MessageFamilySpecificInfo);

  if (of_strlen (pJESpecificInfo->admin_name) > 0)
  {
    ulLen =
      snprintf (pLogBuf, ulTempLen, "AdminName=\"%s\"",
                  pJESpecificInfo->admin_name);
    if (ulLen >= ulTempLen)
    {
      *pulLen_p = ulTotLen;
      return OF_SUCCESS;
    }
    ulTempLen = ulTempLen - ulLen;
    pLogBuf += ulLen;
    ulTotLen += ulLen;
  }

  if (of_strlen (pJESpecificInfo->admin_role) > 0)
  {
    ulLen =
      snprintf (pLogBuf, ulTempLen, "AdminRole=\"%s\"",
                  pJESpecificInfo->admin_role);
    if (ulLen >= ulTempLen)
    {
      *pulLen_p = ulTotLen;
      return OF_SUCCESS;
    }
    ulTempLen = ulTempLen - ulLen;
    pLogBuf += ulLen;
    ulTotLen += ulLen;
  }

  if (of_strlen (pJESpecificInfo->dmpath_a) > 0)
  {
    ulLen =
      snprintf (pLogBuf, ulTempLen, "DMPath=\"%s\"",
                  pJESpecificInfo->dmpath_a);
    if (ulLen >= ulTempLen)
    {
      *pulLen_p = ulTotLen;
      return OF_SUCCESS;
    }
    ulTempLen = ulTempLen - ulLen;
    pLogBuf += ulLen;
    ulTotLen += ulLen;
  }

  if (of_strlen (pJESpecificInfo->aCommand) > 0)
  {
    ulLen =
      snprintf (pLogBuf, ulTempLen, "Command=\"%s\"",
                  pJESpecificInfo->aCommand);
    if (ulLen >= ulTempLen)
    {
      *pulLen_p = ulTotLen;
      return OF_SUCCESS;
    }
    ulTempLen = ulTempLen - ulLen;
    pLogBuf += ulLen;
    ulTotLen += ulLen;
  }

  if (of_strlen (pJESpecificInfo->aInstance) > 0)
  {
    ulLen =
      snprintf (pLogBuf, ulTempLen, "Instance=\"%s\"",
                  pJESpecificInfo->aInstance);
    if (ulLen >= ulTempLen)
    {
      *pulLen_p = ulTotLen;
      return OF_SUCCESS;
    }
    ulTempLen = ulTempLen - ulLen;
    pLogBuf += ulLen;
    ulTotLen += ulLen;
  }

  if (of_strlen (pJESpecificInfo->aCfgData) > 0)
  {
    ulLen =
      snprintf (pLogBuf, ulTempLen, "ConfigData=\"%s\"",
                  pJESpecificInfo->aCfgData);
    if (ulLen >= ulTempLen)
    {
      *pulLen_p = ulTotLen;
      return OF_SUCCESS;
    }
    ulTempLen = ulTempLen - ulLen;
    pLogBuf += ulLen;
    ulTotLen += ulLen;
  }
#if 0
  if (pJESpecificInfo->pReason)
  {
    ulLen =
      snprintf (pLogBuf, ulTempLen, "reason=\"%s\"",
                  pJESpecificInfo->pReason);
    if (ulLen >= ulTempLen)
    {
      *pulLen_p = ulTotLen;
      return OF_SUCCESS;
    }
    ulTempLen = ulTempLen - ulLen;
    pLogBuf += ulLen;
    ulTotLen += ulLen;
  }

  if (pJESpecificInfo->pAction)
  {
    ulLen =
      snprintf (pLogBuf, ulTempLen, "action=\"%s\"",
                  pJESpecificInfo->pReason);
    if (ulLen >= ulTempLen)
    {
      *pulLen_p = ulTotLen;
      return OF_SUCCESS;
    }
    ulTempLen = ulTempLen - ulLen;
    pLogBuf += ulLen;
    ulTotLen += ulLen;
  }
#endif

  *pulLen_p = ulTotLen;
	CM_DEBUG_PRINT("LogMessage=%s",pTmp);
  return OF_SUCCESS;

}
#endif
#endif
#endif
#endif
