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
 * File name: jeigwlog.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/05/2013
 * Description: 
*/

#ifdef VORTIQA_SYSLOG_SUPPORT
#define OF_LOG_CLTR_MAX_FIELD_NAME 63
#define OF_LOG_CLTR_MAX_FIELD_DISPALY_NAME 127
#define CM_MSGID_DESCRIPTION_LEN 200
#define OF_MAX_MSGFAMILY_NAME_LEN 47
#define OF_MAX_SUBFAMILY_NAME_LEN 47


typedef struct IGWSllQNode_s
{
  struct IGWSllQNode_s *next_p;
} IGWSllQNode_t;

typedef struct IGWSllQ_s
{
  IGWSllQNode_t      Head;
  IGWSllQNode_t     *pTail;
  uint32_t          ulCount;
} IGWSllQ_t;


typedef struct IGWLogMessage_s
{
       IGWSllQNode_t SllNode;
       uint32_t   LogId;
       uint32_t   LogTime;
       uint16_t   usSeverity;
       uint16_t   usCategory;   /*type of message-ALERT/GENERAL*/
       unsigned char   pMesgDesc[CM_MSGID_DESCRIPTION_LEN+1];
       uint32_t   ulFamilySize;
       uint32_t   ulTotalSize; /* equals family size + sizeof(IGWLogMessage_t) */
       uint32_t   ulThrottledCount;
      
       /* members to be filled by modules. */
       uint32_t   ulMsgId; 
       uint32_t   ulMsgFamily;
       uint32_t   ulMsgSubFamily;
       uint32_t   ulVSGId;
       uint32_t   ulSrcNetType;
       uint32_t   ulDestNetType;
       uint32_t     SrcIpAddr;
       uint32_t     DstIpAddr;
       #ifdef OF_IPV6_SUPPORT
       unsigned char    ip_v6_addr_b;
       IGWIPv6Addr_t src_ip_v6_addr;
       IGWIPv6Addr_t dst_ip_v6_addr;
       #endif
       uint16_t   SrcPort;
       uint16_t   DstPort;
       uint16_t   usProto;
       uint32_t   ulUserId;
       unsigned char  ucIcmpType;
       unsigned char  ucIcmpCode;
       /* dummy member, used for the purpose of holding variable size family info 
        * This member should be the last member in the structure, addition
        * of new members should be before this member */
       unsigned char   MessageFamilySpecificInfo[0]; 
} IGWLogMessage_t;

typedef int32_t (*IGWLogCltrFormatFnPtr_t)(IGWLogMessage_t *msg_info_p,void *pLogDBField,uint32_t *pulTotalFields_p);

typedef int32_t (*IGWMCFormatFnPtr_t)(IGWLogMessage_t *msg_info_p,unsigned char *,uint32_t *);

typedef struct IGWMSGSFormatterRegInfo_s
{
  uint32_t ulMessageFamily;
  IGWLogCltrFormatFnPtr_t pLogCltrFormatter;
  IGWMCFormatFnPtr_t pMCFormatter;
}IGWMSGSFormatterRegInfo_t;

typedef struct IGWMSGSThrtParams_s
{
  uint32_t ulThrtCnt;
  uint32_t ulThrtTime;
  uint16_t usIssueMask;
#define OF_MSGS_MASK_SRCIP 0x0001
#define OF_MSGS_MASK_DESTIP 0x0002
#define OF_MSGS_MASK_SRCPORT 0x0004
#define OF_MSGS_MASK_DESTPORT 0x0008
#define OF_MSGS_MASK_PROTO 0x0010
#define OF_MSGS_MASK_VSG 0x0020
}IGWMSGSThrtParams_t;

typedef struct IGWMSGSFamilyRegInfo_s
{
  uint32_t ulMessageFamilyId;
  unsigned char ucMsgFamilyName[OF_MAX_MSGFAMILY_NAME_LEN+1];
  uint32_t ulMaxSubFamilies;
}IGWMSGSFamilyRegInfo_t;

typedef struct IGWMSGSSubFamilyRegInfo_s
{
  uint32_t ulMsgFamilyId;
  uint32_t ulSubFamilyId;
  unsigned char ucSubFamilyName[OF_MAX_SUBFAMILY_NAME_LEN+1];
}IGWMSGSSubFamilyRegInfo_t;

typedef struct IGWMSGSSetSubFamilyStatus_s
{
  uint32_t ulMsgFamilyId;
  uint32_t ulSubFamilyId;
  unsigned char  status_b;
#define OF_MSGS_LOG_ENABLE   TRUE
#define OF_MSGS_LOG_DISABLE  FALSE
}IGWMSGSSetSubFamilyStatus_t;


typedef struct IGWMSGSThrtMsgIdInfo_s
{
  uint32_t ulMsgId;
  uint16_t usSeverity;
  uint16_t usCategory;
  uint32_t ulMsgFamilyId;
  uint32_t ulSubFamilyId;
  unsigned char ucMsgDesc[CM_MSGID_DESCRIPTION_LEN+1];
  unsigned char bFlag;
#define OF_MSGS_FALLBACK_SUBFAMILY FALSE
#define OF_MSGS_USE_MSGID          TRUE
  IGWMSGSThrtParams_t ThrtParams;
}IGWMSGSThrtMsgIdInfo_t;

typedef struct IGWMSGSThrtMsgIdRegInfo_s
{
  uint32_t ulNumOfMsgIds;
  IGWMSGSThrtMsgIdInfo_t *msg_id_info_p;
}IGWMSGSThrtMsgIdRegInfo_t;


typedef struct IGWLogCltrDbTableRegInfo_s
{
  unsigned char *pTableName;
  uint32_t ulFieldIdStart;
  uint32_t ulFieldIdEnd;
} IGWLogCltrDbTableRegInfo_t;
   
typedef  struct IGWLogCltrDbFieldTable_s
{ 
  unsigned char  ucFieldName[OF_LOG_CLTR_MAX_FIELD_NAME + 1];
  unsigned char  ucFieldDisplayName[OF_LOG_CLTR_MAX_FIELD_DISPALY_NAME + 1];
  uint32_t ulFieldType;
  uint32_t ulFieldValMaxLen;
}IGWLogCltrDbFieldTable_t;
   
typedef struct IGWLogCltrTableRegInfo_s
{
  uint32_t ulTableIndex;
  uint32_t ulTotalFields;
  IGWLogCltrDbTableRegInfo_t *pTableRegInfo;
  IGWLogCltrDbFieldTable_t  *pLogCltrFieldTable;
}IGWLogCltrTableRegInfo_t;

typedef enum
{
  OF_LOG_SEVERITY_EMERGENCY = 0,
  OF_LOG_SEVERITY_ALERT,
  OF_LOG_SEVERITY_CRITICAL,
  OF_LOG_SEVERITY_ERROR,
  OF_LOG_SEVERITY_WARNING,
  OF_LOG_SEVERITY_NOTICE,
  OF_LOG_SEVERITY_INFO,
  OF_LOG_SEVERITY_DEBUG
}IGWMSGSSeverity_e;

typedef enum
{ 
  OF_LOG_CATEGORY_GENERAL = 0,
  OF_LOG_CATEGORY_ALERT
}IGWMSGSCategory_e;

/*
 * note: iGateway modules, passing integer fields values in binary, should
 * make sure that the values are in host byte format
 */
typedef  enum
{
  CM_LOG_CLTR_DB_FID_TYPE_MIN = 0, /* always should be the first */

  CM_LOG_CLTR_DB_FID_TYPE_STRING,
  CM_LOG_CLTR_DB_FID_TYPE_BYTE,
  CM_LOG_CLTR_DB_FID_TYPE_CHAR,
  CM_LOG_CLTR_DB_FID_TYPE_SHORT_INTEGER, /* 2 bytes integer */
  CM_LOG_CLTR_DB_FID_TYPE_INTEGER,       /* 4 bytes integer */
  CM_LOG_CLTR_DB_FID_TYPE_LONGLONG, /* 8 byte integer*/
  CM_LOG_CLTR_DB_FID_TYPE_IPV4_ADDR,   /* binary address in network byte order*/
#ifdef OF_IPV6_SUPPORT
  CM_LOG_CLTR_DB_FID_TYPE_IPV6_ADDR,   /* binary address in network byte order*/
#endif
  CM_LOG_CLTR_DB_FID_TYPE_PORT,   /* port value in network byte order*/
  /* "mm/dd/yyyy" format,NULL terminated string*/
  CM_LOG_CLTR_DB_FID_TYPE_DATE,

/* "hh:min:secs +/-xx:yy" format,NULL terminated string,where xx:yy is offset from GMT */
  CM_LOG_CLTR_DB_FID_TYPE_TIME,

/* "mm/dd/yyyy hh:min:secs" format,NULL terminated string*/
  CM_LOG_CLTR_DB_FID_TYPE_TIMESTAMP,

/* "mm/dd/yyyy hh:min:secs +/-xx:yy" format,NULL terminated string*/
  CM_LOG_CLTR_DB_FID_TYPE_TIMESTAMPZ,

  CM_LOG_CLTR_DB_FID_TYPE_MAX      /* always should be the last */
}UCMLogCltrDbFieldTypes_e;

/*CM_MSG_FAMILIES_START value is derivied on how the OF_LOG_FAMILY_UCM is defined in ITCM.
 *This has to be synced manualy */
#define CM_MSG_FAMILIES_START 7
typedef enum
{
  CM_LOG_FAMILY_JE = CM_MSG_FAMILIES_START,
  CM_LOG_FAMILY_CONFIG,
  CM_LOG_FAMILY_SYSMSGS,
  CM_LOG_FAMILY_MAX
}UCMMSGSFamilies_e;


#define UCMLogCltrReg_Init IgwLogDBReg_Init
#define UCMLogCltrDbFieldTable_t IGWLogCltrDbFieldTable_t 
#define UCMLogCltrTableRegInfo_t IGWLogCltrTableRegInfo_t
#define UCMLogCtlrRegisterMessage IGWLogCtlrRegisterMessage
#define UCMLogCltrDbTableRegInfo_t IGWLogCltrDbTableRegInfo_t

IGWLogMessage_t *IGWAllocLogMessageBuf(uint32_t ulSize_p);
int32_t IGWLogMessage(IGWLogMessage_t* pLogMsg_p);

#define UCMAllocLogMessageBuf IGWAllocLogMessageBuf

#define UCMMSGSRegisterFamily IGWMSGSRegisterFamily
#define UCMMSGSRegisterMsgIds IGWMSGSRegisterMsgIds
#define UCMMSGSDeRegisterMsgIds IGWMSGSDeRegisterMsgIds
#define UCMMSGSRegisterSubFamily IGWMSGSRegisterSubFamily
#define UCMMsgClntReg_Init IgwSLogReg_Init
#define UCMMSGSFamilyRegInfo_t IGWMSGSFamilyRegInfo_t
#define UCMMSGSRegisterFamily IGWMSGSRegisterFamily
#define UCMMSGSRegisterFormatters IGWMSGSRegisterFormatters
#define UCMMSGSSetSubFamilyStatus IGWMSGSSetSubFamilyStatus
#define UCMMSGSThrtMsgIdRegInfo_t IGWMSGSThrtMsgIdRegInfo_t
#define UCMMSGSThrtMsgIdInfo_t IGWMSGSThrtMsgIdInfo_t
#define UCMMSGSSubFamilyRegInfo_t IGWMSGSSubFamilyRegInfo_t
#define UCMMSGSSetSubFamilyStatus_t IGWMSGSSetSubFamilyStatus_t 
#define UCMLogMessage_t IGWLogMessage_t
#define UCMLogMessage IGWLogMessage
#define UCMMSGSFormatterRegInfo_t IGWMSGSFormatterRegInfo_t

void UCMJEFrameAndSendLogMsg (uint32_t ulMsgId,  char *admin_name_p,  char * admin_role_p,
                                uint32_t command_ui, char * dm_path_p,															 	char * pInstance, struct cm_array_of_nv_pair *arr_nv_pair_p);

#endif
