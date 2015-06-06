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
 * File name: jelog.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/26/2013
 * Description: 
 * 
 */

typedef enum
{
  CM_JE_TABLE_DB_FID_START_ID = 0,
  CM_JE_TABLE_DB_LOG_ID = CM_JE_TABLE_DB_FID_START_ID,
  CM_JE_TABLE_DB_LOG_TIME,
  CM_JE_TABLE_DB_MSG_ID,
//  CM_JE_TABLE_DB_SEVERITY,
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
  CM_JE_TABLE_DB_TOTAL_FIELDS = (CM_JE_TABLE_DB_END_ID - CM_JE_TABLE_DB_FID_START_ID) + 1
} cm_je_log_cltr_db_log_fields_e;

 
