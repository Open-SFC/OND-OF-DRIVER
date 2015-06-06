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
 * File name: jeigwlogdb.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/05/2013
 * Description: 
*/

#ifndef __JEIGWLOGDB_H
#define __JEIGWLOGDB_H
#ifdef VORTIQA_SYSLOG_SUPPORT

#define UCMLogCltrDbFieldName_t IGWLogCltrDbFieldName_t
#define UCMLogCltrDbCntxt_t IGWLogCltrDbCntxt_t
#define UCMLogCltrDbAddLog_t IGWLogCltrDbAddLog_t
#define UCMLogCltrDbSortParams_t IGWLogCltrDbSortParams_t
#define UCMLogCltrDbDelLog_t IGWLogCltrDbDelLog_t
#define UCMLogCltrDbOpenQry_t IGWLogCltrDbOpenQry_t
#define UCMLogCltrDbFieldInfo_t IGWLogCltrDbFieldInfo_t
#define UCMLogCltrDbFieldVal_t IGWLogCltrDbFieldVal_t
#define UCMLogCltrDbCloseQry_t IGWLogCltrDbCloseQry_t
#define UCMLogCltrDbQryResult_t IGWLogCltrDbQryResult_t
#define UCMLogCltrDbFltrParams_t IGWLogCltrDbFltrParams_t
#define UCMLogCltrDbAccessOpenCntxt IGWLogCltrDbAccessOpenCntxt
#define UCMLogCltrDbAccessCloseCntxt IGWLogCltrDbAccessCloseCntxt
#define UCMLogCltrDbAccessQryOpen IGWLogCltrDbAccessQryOpen
#define UCMLogCltrDbAccessGetQryResults IGWLogCltrDbAccessGetQryResults
#define UCMLogCltrDbAccessQryClose IGWLogCltrDbAccessQryClose

#define CM_LOGCLTR_AND_QUERY_FIELD 0x01
#define CM_LOGCLTR_OR_QUERY_FIELD 0x02

#define  CM_LOG_CLTR_MAX_FIELD_DISPALY_NAME  127

typedef  enum
{
  CM_LOG_CLTR_QUERY_FLTR_OPRTR_EQUAL_TO = 0,
  CM_LOG_CLTR_QUERY_FLTR_OPRTR_LESS_THAN,
  CM_LOG_CLTR_QUERY_FLTR_OPRTR_LESS_THAN_EQUAL_TO,
  CM_LOG_CLTR_QUERY_FLTR_OPRTR_GREATER_THAN,
  CM_LOG_CLTR_QUERY_FLTR_OPRTR_GREATER_THAN_EQUAL_TO,
  CM_LOG_CLTR_QUERY_FLTR_OPRTR_NOT_EQUAL_TO,
  CM_LOG_CLTR_QUERY_FLTR_OPRTR_IN_RANGE,
  CM_LOG_CLTR_QUERY_FLTR_OPRTR_OUT_OF_RANGE,
  CM_LOG_CLTR_QUERY_FLTR_OPRTR_EXACT_EQUAL_TO  
}UCMLogCltrDbQueryFltrOperators_e;

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

int32_t LogCltrDbGetTableIndexForTableName(unsigned char *pTableName);

#endif
#endif
