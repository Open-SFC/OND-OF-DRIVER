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
 * File name: jesyslog.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/05/2013
 * Description: 
*/

#ifndef _JESYSLOG_H_
#define _JESYSLOG_H_

/******************************************************************************
 * * * * * * * * * * * JE METHOD DEFINITIONS * * * * * * * * * * * * * * * * * 
 ******************************************************************************/

#define JE_TRACELOG_LOCATION __FILE__,__FUNCTION__,__LINE__
#define JE_LOG_PARAMS_NONE  0,0,0,0,0,0,0,0
#define JE_TRACE_ENABLED FALSE
#define JE_SYSLOG_ENABLED FALSE
#define JE_SEND_SYSLOG  UCMJEFrameAndSendLogMsg

/******************************************************************************
 * * * * * * * * * * * JE LOG MACROS * * * * * * * * * * * * * * * * * * * *
 ******************************************************************************/

//#define CM_LOG_FAMILY_JE 1
#define CM_JE_MSG_FAMILY_NAME "CM_Messages"
#define CM_JE_CONFIG_UPDATE_SUBFAMILY_NAME "CM_ConfigUpdates"
#define  CM_LOG_LDSV_SUBFAMILY_NAME "CM_Ldsv"
#define  CM_LOG_CONFIGSYNC_SUBFAMILY_NAME "CM_ConfigSync"

#define JE_START_MSGID_BASE 50000
#define CM_JE_LOBDB_TABLE_FIELD_LEN 32
#define CM_JE_LOGDB_FIELD_COMMAND_LEN CM_JE_LOBDB_TABLE_FIELD_LEN
#define CM_JE_LOGDB_FIELD_DMPATH_LEN (2*CM_JE_LOBDB_TABLE_FIELD_LEN)
#define CM_JE_LOGDB_FIELD_DMINSTANCE_LEN CM_JE_LOBDB_TABLE_FIELD_LEN
#define CM_JE_LOGDB_FIELD_UCMADMIN_LEN CM_JE_LOBDB_TABLE_FIELD_LEN
#define CM_JE_LOGDB_FIELD_UCMROLE_LEN CM_JE_LOBDB_TABLE_FIELD_LEN
#define CM_JE_LOGDB_FIELD_NVPAIRS_LEN (64* CM_JE_LOBDB_TABLE_FIELD_LEN)
#define CM_JE_LOGDB_FIELD_REASON_LEN 200
#define CM_JE_LOGDB_FIELD_ACTION_LEN 100
#define CM_JE_LOGDB_FIELD_MSGDESC_LEN 200 
#define CM_JE_LOGDB_FIELD_IPADDRESS_LEN CM_JE_LOBDB_TABLE_FIELD_LEN 
#define CM_JE_MSGTHRT_TIME 1
#define CM_JE_MSGTHRT_COUNT 5
#define CM_JE_MSGTHRT_ISSUE_IPMASK 0
#define CM_JE_MSGTHRT_FLAGS 0

#define JE_TRACE_STRING_MAX_LEN 256

#define CM_VORTIQA_DEVIP_FILE "/igateway/uint32_t"
/******************************************************************************
 * * * * * * * * * * * * JE LOG ENUMERATORS* * * * * * * * * * * * * * * * * *
 ******************************************************************************/

typedef enum
{
  CM_LOG_JE_SUBFAMILY_CONFIG_UPDATE = 0,
  CM_LOG_SUBFAMILY_LDSV,
  CM_LOG_SUBFAMILY_CONFIGSYNC,
  CM_LOG_JE_SUBFAMILY_ROLES,
	CM_LOG_JE_MAX_SUBFAMILY
} UCMJESubFamilyFields_e;


typedef enum
{
  JE_TRACE_LEVEL_ONE = 1,
  JE_TRACE_LEVEL_TWO,
  JE_TRACE_LEVEL_THREE,
  JE_TRACE_LEVEL_FOUR,
  JE_TRACE_LEVEL_NONE
} DPTraceLevels_e;

typedef enum
{
  CM_LOG_SEVERITY_EMERGENCY = 0,
  CM_LOG_SEVERITY_ALERT,
  CM_LOG_SEVERITY_CRITICAL,
  CM_LOG_SEVERITY_ERROR,
  CM_LOG_SEVERITY_WARNING,
  CM_LOG_SEVERITY_NOTICE,
  CM_LOG_SEVERITY_INFO,
  CM_LOG_SEVERITY_DEBUG
}UCMMSGSSeverity_e;

typedef enum
{
  CM_LOG_CATEGORY_GENERAL = 0,
  CM_LOG_CATEGORY_ALERT
}UCMMSGSCategory_e;


typedef enum
{
  JE_LOG_SEVERITY_EMERGENCY = CM_LOG_SEVERITY_EMERGENCY,
  JE_LOG_SEVERITY_ALERT = 1,
  JE_LOG_SEVERITY_CRITICAL,
  JE_LOG_SEVERITY_ERROR,
  JE_LOG_SEVERITY_WARNING,
  JE_LOG_SEVERITY_NOTICE,
  JE_LOG_SEVERITY_INFO,
  JE_LOG_SEVERITY_DEBUG,
  JE_LOG_SEVERITY_NONE
} JEMSGSSeverity_e;

typedef enum
{
  JE_LOG_CATEGORY_GENERAL = CM_LOG_CATEGORY_GENERAL,
  JE_LOG_CATEGORY_ALERT = CM_LOG_CATEGORY_ALERT,
} JEMSGSCategory_e;

typedef enum
{
  JE_MSGID_BASE = JE_START_MSGID_BASE,
  JE_MSG_CONFIG_ADD,
  JE_MSG_CONFIG_ADD_FAIL,
  JE_MSG_CONFIG_SET,
  JE_MSG_CONFIG_SET_FAIL,
  JE_MSG_CONFIG_DEL,
  JE_MSG_CONFIG_DEL_FAIL,
  JE_MSG_CONFIG_PERMISSION_DENIED,
	JE_MSG_CONFIG_LOAD_SUCCESS,
	JE_MSG_CONFIG_LOAD_FAIL,
	JE_MSG_CONFIG_LOADDEFAULOF_SUCCESS,
	JE_MSG_CONFIG_LOADDEFAULT_FAIL,
	JE_MSG_CONFIG_SAVE_SUCCESS,
	JE_MSG_CONFIG_SAVE_FAIL,
  CM_MSG_CONFIG_RUNTIME_VERSIONS_REACHED_MAX,
  CM_MSG_CSD_FILE_SYNC_SUCCESS,
  CM_MSG_CSD_FILE_SYNC_FAIL,
  CM_MSG_CSD_DELTA_SYNC_SUCCESS,
  CM_MSG_CSD_DELTA_SYNC_FAIL,
  CM_MSG_CSD_IMPLICIT_SAVE_ISSUE,
  CM_MSG_CSD_STATE_MASTER,
  CM_MSG_CSD_STATE_PARTICIPANT,
  CM_MSG_CSD_STATE_HALT,
  CM_MSG_CSD_STATE_AUTOSYNC_DISABLED,
  JE_END_MSGID_BASE = CM_MSG_CSD_STATE_AUTOSYNC_DISABLED
} JEMsgIds_e;
#define CM_JE_MAX_MSGIDS (JE_END_MSGID_BASE - JE_START_MSGID_BASE)
typedef enum
{
  CM_JE_TABLE_DB_FID_START_ID = 0,
  CM_JE_TABLE_DB_LOG_ID = CM_JE_TABLE_DB_FID_START_ID,
  CM_JE_TABLE_DB_LOG_TIME,
  CM_JE_TABLE_DB_MSG_ID,
 // CM_JE_TABLE_DB_SEVERITY,
  CM_JE_TABLE_DB_MSGDESC,
  CM_JE_TABLE_DB_UCMADMIN,
  CM_JE_TABLE_DB_UCMROLE,
  CM_JE_TABLE_DB_DMPATH,
  CM_JE_TABLE_DB_COMMAND,
  CM_JE_TABLE_DB_DMINSTANCE,
  CM_JE_TABLE_DB_CONFIGDATA,
  CM_JE_TABLE_DB_TABLENAME,
  CM_JE_TABLE_DB_IPADDRESS,
  CM_JE_TABLE_DB_END_ID = CM_JE_TABLE_DB_IPADDRESS,
  CM_JE_TABLE_DB_TOTAL_FIELDS = (CM_JE_TABLE_DB_END_ID -
                                    CM_JE_TABLE_DB_FID_START_ID) + 1
} cm_je_log_cltr_db_log_fields_e;

/******************************************************************************
 * * * * * * * * JE ENUMERATOR DEPENDENT MACRO DEFINITIONS * * * * * * * * * *
 *******************************************************************************/
#define JE_LOG_LEVEL   JE_LOG_SEVERITY_DEBUG
#define JE_TRACE_LEVEL JE_TRACE_LEVEL_FOUR

/*****************************************************************************
 * * * * * * * * * * JE LOG STRUCTURE DEFINITIONS * * * * * * * * * * * * * *
 *****************************************************************************/
typedef struct UCMJELogInfo_s
{
  char aCommand[CM_JE_LOGDB_FIELD_COMMAND_LEN];
  char dmpath_a[CM_JE_LOGDB_FIELD_DMPATH_LEN];
  char aInstance[CM_JE_LOGDB_FIELD_DMINSTANCE_LEN];
  char admin_name[CM_JE_LOGDB_FIELD_UCMADMIN_LEN];
  char admin_role[CM_JE_LOGDB_FIELD_UCMROLE_LEN];
  char aCfgData[CM_JE_LOGDB_FIELD_NVPAIRS_LEN];
  char aReason[CM_JE_LOGDB_FIELD_REASON_LEN];
  char aAction[CM_JE_LOGDB_FIELD_ACTION_LEN];
  char aTableName[CM_JE_LOGDB_FIELD_MSGDESC_LEN];
  char aIpAddress[CM_JE_LOGDB_FIELD_IPADDRESS_LEN];
} UCMJELogInfo_t;

/******************************************************************************
 * * * * * * * * * JE LOG FUNCTION PROTOTYPES * * * * * * * * * * * * * * * *
 ******************************************************************************/
void UCMJEMsgsRegInit (void);

void UCMJERegisterToLogCltrDB (void);

void UCMJEMCRegFormaters (void);

#endif
