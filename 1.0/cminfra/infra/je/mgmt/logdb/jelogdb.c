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
 * File name: evmgrreg.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/29/2013
 * Description: Log Controller Database Registration
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

void UCMLogCltrReg_Init(void);
UCMLogCltrDbTableRegInfo_t JELogTblRegInfo =
  { CM_JE_MSG_FAMILY_NAME, CM_JE_TABLE_DB_FID_START_ID,
  CM_JE_TABLE_DB_END_ID
};

UCMLogCltrDbFieldTable_t
  pJELogCtlrFieldTable[CM_JE_TABLE_DB_TOTAL_FIELDS] =
{
  { "logid", "Log Identifier", CM_LOG_CLTR_DB_FID_TYPE_LONGLONG, sizeof ("11112222333344445555")} ,
  { "logtmstmp", "Log Time", CM_LOG_CLTR_DB_FID_TYPE_TIMESTAMP, sizeof ("mm/dd/yyyy hh:mm:ss +xx:yy")} ,
  {  "MsgId", "Message Identifier", CM_LOG_CLTR_DB_FID_TYPE_INTEGER,sizeof (uint32_t)},
#if 0
  {
  "Severity", "Message Severity", CM_LOG_CLTR_DB_FID_TYPE_SHORT_INTEGER,
      sizeof (uint16_t)}
  ,
#endif
  { "MsgDesc", "Message Description", CM_LOG_CLTR_DB_FID_TYPE_STRING,CM_JE_LOGDB_FIELD_MSGDESC_LEN} ,
  { "Admin", "UCM Admin", CM_LOG_CLTR_DB_FID_TYPE_STRING,CM_JE_LOGDB_FIELD_UCMADMIN_LEN},
  { "Role", "UCM Role", CM_LOG_CLTR_DB_FID_TYPE_STRING,CM_JE_LOGDB_FIELD_UCMROLE_LEN},
  { "DMPath", "Data Model Path", CM_LOG_CLTR_DB_FID_TYPE_STRING,CM_JE_LOGDB_FIELD_DMPATH_LEN},
  { "Command", "Operation", CM_LOG_CLTR_DB_FID_TYPE_STRING,CM_JE_LOGDB_FIELD_COMMAND_LEN},
  { "Instance", "Instance", CM_LOG_CLTR_DB_FID_TYPE_STRING,CM_JE_LOGDB_FIELD_DMINSTANCE_LEN},
  { "configdata", "Configuration Data", CM_LOG_CLTR_DB_FID_TYPE_STRING, CM_JE_LOGDB_FIELD_NVPAIRS_LEN},
  { "TableName", "Table Name", CM_LOG_CLTR_DB_FID_TYPE_STRING,CM_JE_LOGDB_FIELD_DMPATH_LEN}

};

/******************************************************************************
 * * * * * * * D P L B   L O G   C T L   D B   I N I T * * * * * * * * * * * * 
 *****************************************************************************/
void UCMLogCltrReg_Init()
{
	 CM_DEBUG_PRINT ("JE Log Cltr DB Init");
   UCMJERegisterToLogCltrDB();
	 return;
}

void UCMJERegisterToLogCltrDB (void)
{
  UCMLogCltrTableRegInfo_t JELogCltrRegInfo = { };

  CM_DEBUG_PRINT ("JE registration to LogCltrDB");
  JELogCltrRegInfo.ulTableIndex = CM_LOG_FAMILY_JE;
  JELogCltrRegInfo.pTableRegInfo = &(JELogTblRegInfo);
  JELogCltrRegInfo.ulTotalFields = CM_JE_TABLE_DB_TOTAL_FIELDS;
  JELogCltrRegInfo.pLogCltrFieldTable = pJELogCtlrFieldTable;
  if (UCMLogCtlrRegisterMessage (&JELogCltrRegInfo) != OF_SUCCESS)
  {
    CM_DEBUG_PRINT ("UCMLogCtlrRegisterMessage failed");
  }
  CM_DEBUG_PRINT ("Exit");
  return;
}
#endif
#endif
#endif
#endif
