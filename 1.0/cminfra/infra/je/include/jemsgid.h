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
 * File name: jemsgid.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/05/2013
 * Description: 
*/

#ifndef _JEMSGID_H_
#define _JEMSGID_H_

/*****************************************************************************
 * * * * * * * * * * JE GLOBAL STRUCTUREs  * * * * * * * * * * * * * *
 *****************************************************************************/
UCMMSGSThrtMsgIdInfo_t JEMsgIdInfo[CM_JE_MAX_MSGIDS] = {
   /*JE_LOG_MSGID_INIT_RANGE_START*/
   {JE_MSG_CONFIG_ADD, JE_LOG_SEVERITY_CRITICAL, JE_LOG_CATEGORY_ALERT,
      CM_LOG_FAMILY_JE, CM_LOG_JE_SUBFAMILY_CONFIG_UPDATE,
      "Configuration Added",
      TRUE, {0, 0,
         CM_JE_MSGTHRT_ISSUE_IPMASK}
   }
   ,
      {JE_MSG_CONFIG_ADD_FAIL, JE_LOG_SEVERITY_CRITICAL, JE_LOG_CATEGORY_ALERT,
         CM_LOG_FAMILY_JE, CM_LOG_JE_SUBFAMILY_CONFIG_UPDATE,
         "Configuration Add Failed",
         TRUE, {0, 0,
            CM_JE_MSGTHRT_ISSUE_IPMASK}
      }
   ,
      {JE_MSG_CONFIG_SET, JE_LOG_SEVERITY_CRITICAL, JE_LOG_CATEGORY_ALERT,
         CM_LOG_FAMILY_JE, CM_LOG_JE_SUBFAMILY_CONFIG_UPDATE,
         "Configuration Modified",TRUE, {0, 0,
            CM_JE_MSGTHRT_ISSUE_IPMASK}
      }
   ,
      {JE_MSG_CONFIG_SET_FAIL, JE_LOG_SEVERITY_CRITICAL, JE_LOG_CATEGORY_ALERT,
         CM_LOG_FAMILY_JE, CM_LOG_JE_SUBFAMILY_CONFIG_UPDATE,
         "Configuration Modification Failed",TRUE, {0, 0,
            CM_JE_MSGTHRT_ISSUE_IPMASK}
      }
   ,

      {JE_MSG_CONFIG_DEL, JE_LOG_SEVERITY_CRITICAL, JE_LOG_CATEGORY_ALERT,
         CM_LOG_FAMILY_JE, CM_LOG_JE_SUBFAMILY_CONFIG_UPDATE,
         "Configuration Deleted",
         TRUE, {0, 0,
            CM_JE_MSGTHRT_ISSUE_IPMASK}
      }
   ,
      {JE_MSG_CONFIG_DEL_FAIL, JE_LOG_SEVERITY_CRITICAL, JE_LOG_CATEGORY_ALERT,
         CM_LOG_FAMILY_JE, CM_LOG_JE_SUBFAMILY_CONFIG_UPDATE,
         "Configuration Deletion Failed",
         TRUE, {0, 0,
            CM_JE_MSGTHRT_ISSUE_IPMASK}
      }
   ,
      {JE_MSG_CONFIG_PERMISSION_DENIED, JE_LOG_SEVERITY_CRITICAL, JE_LOG_CATEGORY_ALERT,
         CM_LOG_FAMILY_JE, CM_LOG_JE_SUBFAMILY_CONFIG_UPDATE,
         "Permission Denied",
         TRUE, {0, 0,
            CM_JE_MSGTHRT_ISSUE_IPMASK}
      }
   ,
      { JE_MSG_CONFIG_LOAD_SUCCESS, JE_LOG_SEVERITY_CRITICAL, JE_LOG_CATEGORY_ALERT,
         CM_LOG_FAMILY_JE, CM_LOG_SUBFAMILY_LDSV,
         "Configuration succesfully Loaded",
         TRUE, {0, 0,
            CM_JE_MSGTHRT_ISSUE_IPMASK}
      }
   ,
      { JE_MSG_CONFIG_LOAD_FAIL, JE_LOG_SEVERITY_CRITICAL, JE_LOG_CATEGORY_ALERT,
         CM_LOG_FAMILY_JE, CM_LOG_SUBFAMILY_LDSV,
         "Configuration Load Failed",
         TRUE, {0, 0,
            CM_JE_MSGTHRT_ISSUE_IPMASK}
      }
   ,
      { JE_MSG_CONFIG_LOADDEFAULOF_SUCCESS, JE_LOG_SEVERITY_CRITICAL, JE_LOG_CATEGORY_ALERT,
         CM_LOG_FAMILY_JE, CM_LOG_SUBFAMILY_LDSV,
         "Default Configuration succesfully Loaded",
         TRUE, {0, 0,
            CM_JE_MSGTHRT_ISSUE_IPMASK}
      }
   ,
      { JE_MSG_CONFIG_LOADDEFAULT_FAIL, JE_LOG_SEVERITY_CRITICAL, JE_LOG_CATEGORY_ALERT,
         CM_LOG_FAMILY_JE, CM_LOG_SUBFAMILY_LDSV,
         "Default Configuration Load Failed",
         TRUE, {0, 0,
            CM_JE_MSGTHRT_ISSUE_IPMASK}
      }
   ,
      { JE_MSG_CONFIG_SAVE_SUCCESS, JE_LOG_SEVERITY_CRITICAL, JE_LOG_CATEGORY_ALERT,
         CM_LOG_FAMILY_JE, CM_LOG_SUBFAMILY_LDSV,
         "Configuration succesfully Saved",
         TRUE, {0, 0,
            CM_JE_MSGTHRT_ISSUE_IPMASK}
      }
   ,
      { JE_MSG_CONFIG_SAVE_FAIL, JE_LOG_SEVERITY_CRITICAL, JE_LOG_CATEGORY_ALERT,
         CM_LOG_FAMILY_JE, CM_LOG_SUBFAMILY_LDSV,
         "Configuration Save Failed",
         TRUE, {0, 0,
            CM_JE_MSGTHRT_ISSUE_IPMASK}
      }
	 ,
			{ CM_MSG_CONFIG_RUNTIME_VERSIONS_REACHED_MAX, JE_LOG_SEVERITY_CRITICAL, JE_LOG_CATEGORY_ALERT,
				 CM_LOG_FAMILY_JE, CM_LOG_JE_SUBFAMILY_CONFIG_UPDATE,
				 "version history_t will be lost, need to issue save command",
				 TRUE, {0, 0,
						CM_JE_MSGTHRT_ISSUE_IPMASK}
			}
	 ,
			{ CM_MSG_CSD_FILE_SYNC_SUCCESS, JE_LOG_SEVERITY_CRITICAL, JE_LOG_CATEGORY_ALERT,
         CM_LOG_FAMILY_JE, CM_LOG_SUBFAMILY_CONFIGSYNC,
         "Succesfully file Configuration Synchonized",
         TRUE, {0, 0,
            CM_JE_MSGTHRT_ISSUE_IPMASK}
      },

      { CM_MSG_CSD_FILE_SYNC_FAIL, JE_LOG_SEVERITY_CRITICAL, JE_LOG_CATEGORY_ALERT,
         CM_LOG_FAMILY_JE, CM_LOG_SUBFAMILY_CONFIGSYNC,
         "file Configuration Synchonization Failed",
         TRUE, {0, 0,
            CM_JE_MSGTHRT_ISSUE_IPMASK}
      },

      { CM_MSG_CSD_DELTA_SYNC_SUCCESS, JE_LOG_SEVERITY_CRITICAL, JE_LOG_CATEGORY_ALERT,
         CM_LOG_FAMILY_JE, CM_LOG_SUBFAMILY_CONFIGSYNC,
         "Succesfully Delta Configuration Synchonized",
         TRUE, {0, 0,
            CM_JE_MSGTHRT_ISSUE_IPMASK}
      },

      { CM_MSG_CSD_DELTA_SYNC_FAIL, JE_LOG_SEVERITY_CRITICAL, JE_LOG_CATEGORY_ALERT,
         CM_LOG_FAMILY_JE, CM_LOG_SUBFAMILY_CONFIGSYNC,
         "Delta Configuration Synchonization Failed",
         TRUE, {0, 0,
            CM_JE_MSGTHRT_ISSUE_IPMASK}
      },

      { CM_MSG_CSD_IMPLICIT_SAVE_ISSUE, JE_LOG_SEVERITY_CRITICAL, JE_LOG_CATEGORY_ALERT,
         CM_LOG_FAMILY_JE, CM_LOG_SUBFAMILY_CONFIGSYNC,
         "Issuing Implicit Save Command",
         TRUE, {0, 0,
            CM_JE_MSGTHRT_ISSUE_IPMASK}
      }
   ,
      { CM_MSG_CSD_IMPLICIT_SAVE_ISSUE, JE_LOG_SEVERITY_CRITICAL, JE_LOG_CATEGORY_ALERT,
         CM_LOG_FAMILY_JE, CM_LOG_SUBFAMILY_CONFIGSYNC,
         "Issuing Implicit Save Command",
         TRUE, {0, 0,
            CM_JE_MSGTHRT_ISSUE_IPMASK}
      }
   ,
      { CM_MSG_CSD_STATE_MASTER, JE_LOG_SEVERITY_CRITICAL, JE_LOG_CATEGORY_ALERT,
         CM_LOG_FAMILY_JE, CM_LOG_SUBFAMILY_CONFIGSYNC,
         "Config Sync Moved to Master State",
         TRUE, {0, 0,
            CM_JE_MSGTHRT_ISSUE_IPMASK}
      }
   ,
      { CM_MSG_CSD_STATE_PARTICIPANT, JE_LOG_SEVERITY_CRITICAL, JE_LOG_CATEGORY_ALERT,
         CM_LOG_FAMILY_JE, CM_LOG_SUBFAMILY_CONFIGSYNC,
         "Config Sync Moved to Participant State",
         TRUE, {0, 0,
            CM_JE_MSGTHRT_ISSUE_IPMASK}
      }
   ,
      { CM_MSG_CSD_STATE_HALT, JE_LOG_SEVERITY_CRITICAL, JE_LOG_CATEGORY_ALERT,
         CM_LOG_FAMILY_JE, CM_LOG_SUBFAMILY_CONFIGSYNC,
         "Config Sync Moved to Halt State",
         TRUE, {0, 0,
            CM_JE_MSGTHRT_ISSUE_IPMASK}
      }
   ,
      { CM_MSG_CSD_STATE_AUTOSYNC_DISABLED, JE_LOG_SEVERITY_CRITICAL, JE_LOG_CATEGORY_ALERT,
         CM_LOG_FAMILY_JE, CM_LOG_SUBFAMILY_CONFIGSYNC,
         "Config Sync Moved to AutoSync Disabled State",
         TRUE, {0, 0,
            CM_JE_MSGTHRT_ISSUE_IPMASK}
      }
};
#endif
