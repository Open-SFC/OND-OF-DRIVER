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
 * File name: trchltst.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/05/2013
 * Description: UCM Transport Channel Support 
*/

#if (defined(CM_TRANSPORT_CHANNEL_SUPPORT) && defined (CM_TRCHNL_TEST_SUPPORT))
#include <netinet/in.h>
#include "trincld.h"

extern int32_t UCMFreeUCMResult (uint32_t command_id, uint32_t sub_command_id,
                                 struct cm_result * pUCMResult);
main (int argc, char *argv[])
{
  struct cm_tnsprt_channel *tnsprt_channel_p = NULL;
  struct cm_je_config_session *pConfigSess = NULL;
  struct cm_role_info role_info = { "Admin", "Admin" };
  struct cm_nv_pair nv_pairs[2] = {
    {7, "VsgName", CM_DATA_TYPE_STRING, 7, "general"},
    {11, "VsgId", CM_DATA_TYPE_UINT, 1, "0"},
  }
  ;
  struct cm_array_of_nv_pair nv_pair_array = { 2, nv_pairs };
  struct cm_command Command = { CM_CMD_ADD_PARAMS, "IGD.VSG",
    nv_pair_array
  };

  struct cm_result *result_p = NULL;
  int32_t return_value;

  struct cm_nv_pair nv_pairs2[4] = {
    {6, "IPName", CM_DATA_TYPE_STRING, 9, "MYIPDBREC"},
    {7, "VsgName", CM_DATA_TYPE_STRING, 7, "general"},
    {11, "RecordType", CM_DATA_TYPE_STRING, 6, "single"},
    {10, "IPAddr", CM_DATA_TYPE_IPADDR, 7, "1.2.3.4"}
  }
  ;
  struct cm_array_of_nv_pair nv_pair_array2 = { 4, nv_pairs2 };
  struct cm_command Command2 = { CM_CMD_ADD_PARAMS, "IGD.VSG{general}.NetObj.IPDB",
    nv_pair_array2
  };

  struct cm_array_of_nv_pair *out_array_of_nv_pair_arr_p = NULL;
  struct cm_nv_pair key_nv_pairs[2] = {
    {6, "IPName", CM_DATA_TYPE_STRING, 9, "MYIPDBREC"},
    {7, "VsgName", CM_DATA_TYPE_STRING, 7, "general"}
  }
  ;
  struct cm_array_of_nv_pair KeysArray = { 2, key_nv_pairs };

  uint32_t count_ui = 1, uiCnt = 0, uiNvCnt = 0, i;

  struct cm_dm_role_permission RolePerm = { "MyRole", CM_PERMISSION_READ_WRITE };
  struct cm_dm_role_permission RolePerm2 = { "MyInstanceRole", CM_PERMISSION_READ_ONLY };
  struct cm_dm_array_of_role_permissions *role_perm_ary_p;

  /* Start the Transport connection with JE */
  if ((tnsprt_channel_p = cm_tnsprt_create_channel (CM_IPPROTO_TCP, 0, 0,
                                                  UCMJE_CONFIGREQUEST_PORT)) ==
      NULL)
  {
    CM_TRANS_DEBUG_PRINT ("%s :: Cannot create socket\n\r", __FUNCTION__);
    exit (1);
  }
#if 0
  /* POST Request */
  pConfigSess =
    (struct cm_je_config_session *) cm_config_session_start (CM_CLI_MGMT_ENGINE_ID,
                                                    &role_info, tnsprt_channel_p);
  if (!pConfigSess)
  {
    CM_TRANS_DEBUG_PRINT ("%s :: cm_config_session_start failed\n\r",
                           __FUNCTION__);
    exit (1);
  }

  return_value = cm_config_session_update_cmd (pConfigSess, &Command, &result_p);

  if (return_value == OF_SUCCESS)
  {
    cm_config_session_end (pConfigSess, CM_CMD_CONFIG_SESSION_COMMIT, &result_p);
  }
  else
  {
    cm_config_session_end (pConfigSess, CM_CMD_CONFIG_SESSION_REVOKE, &result_p);
  }

  of_free (pConfigSess);
  pConfigSess = NULL;

  of_free (result_p);
  result_p = NULL;

  UCMFreeUCMResult (CM_COMMAND_CONFIG_SESSION_START, CM_CMD_ADD_PARAMS,
                    result_p);
  result_p = NULL;

  /* POST Request */
  pConfigSess =
    (struct cm_je_config_session *) cm_config_session_start (CM_CLI_MGMT_ENGINE_ID,
                                                    &role_info, tnsprt_channel_p);
  if (!pConfigSess)
  {
    CM_TRANS_DEBUG_PRINT ("%s :: cm_config_session_start failed\n\r",
                           __FUNCTION__);
    exit (1);
  }

  return_value = cm_config_session_update_cmd (pConfigSess, &Command2, &result_p);
  if (return_value == OF_SUCCESS)
  {
    cm_config_session_end (pConfigSess, CM_CMD_CONFIG_SESSION_COMMIT, &result_p);
  }
  else
  {
    cm_config_session_end (pConfigSess, CM_CMD_CONFIG_SESSION_REVOKE, &result_p);
  }

  of_free (pConfigSess);
  pConfigSess = NULL;

  UCMFreeUCMResult (CM_COMMAND_CONFIG_SESSION_START, CM_CMD_ADD_PARAMS,
                    result_p);
  result_p = NULL;

  /* GET  Request */
  cm_get_first_n_records (tnsprt_channel_p, CM_CLI_MGMT_ENGINE_ID,
                       "Admin", "IGD.VSG{general}.NetObj.IPDB",
                       &KeysArray, &count_ui, &out_array_of_nv_pair_arr_p);
  if (out_array_of_nv_pair_arr_p)
  {
    CM_TRANS_DEBUG_PRINT ("\r\n %s():: printint output ", __FUNCTION__);
    for (uiCnt = 0; uiCnt < count_ui; uiCnt++)
    {
      for (uiNvCnt = 0; uiNvCnt < out_array_of_nv_pair_arr_p[uiCnt].count_ui;
           uiNvCnt++)
      {
        CM_TRANS_DEBUG_PRINT ("\r\n %s():: Name = %s Value = %s ",
                                 __FUNCTION__,
                                 out_array_of_nv_pair_arr_p[uiCnt].nv_pairs[uiNvCnt].
                                 name_p,
                                 (char *) out_array_of_nv_pair_arr_p[uiCnt].
                                 nv_pairs[uiNvCnt].value_p);

      }

    }
  }
#endif
#ifdef CM_ROLES_PERM_SUPPORT
  if (cm_dm_set_role_permissions_by_role (tnsprt_channel_p, CM_CLI_MGMT_ENGINE_ID,
                                "IGD.VSG", CM_DM_DEFAULT_ROLE,
                                &RolePerm,NULL) != OF_SUCCESS)
  {
    CM_TRANS_DEBUG_PRINT ("UCMDM_Setrole_permissions Failed");
  }
  else
  {
    CM_TRANS_DEBUG_PRINT ("permissions succesfull");
  }
  RolePerm.permissions=0;
  if (cm_dm_get_role_permissions_by_role (tnsprt_channel_p, CM_CLI_MGMT_ENGINE_ID,
                                "IGD.VSG", CM_DM_DEFAULT_ROLE,
                                &RolePerm) != OF_SUCCESS)
  {
    CM_TRANS_DEBUG_PRINT ("GetPermissions ByRole Failed");
  }
	else
	{
    CM_TRANS_DEBUG_PRINT (" name = %s perms = %d", RolePerm.role, RolePerm.permissions);
	}

  if (cm_dm_get_role_permissions (tnsprt_channel_p, CM_CLI_MGMT_ENGINE_ID,
                                "IGD.VSG", CM_DM_DEFAULT_ROLE,
                                &role_perm_ary_p) != OF_SUCCESS)
  {
    CM_TRANS_DEBUG_PRINT ("  1GetPermissions Failed");
  }
  else
  {
    CM_TRANS_DEBUG_PRINT ("Number of Roles %u at IGD.VSG", role_perm_ary_p->count_ui);
    for (i = 0; i < role_perm_ary_p->count_ui; i++)
    {
      CM_TRANS_DEBUG_PRINT ("Role=%s Perm=%u",
                               role_perm_ary_p->role_permissions[i].role,
                               role_perm_ary_p->role_permissions[i].permissions);
		}
		of_free(role_perm_ary_p->role_permissions);
		of_free(role_perm_ary_p);
		role_perm_ary_p = NULL;
	}


  if (cm_dm_set_instance_role_permissions_by_role (tnsprt_channel_p, CM_CLI_MGMT_ENGINE_ID,
                                        "IGD.VSG{general}", CM_DM_DEFAULT_ROLE,
                                        &RolePerm2) != OF_SUCCESS)
  {
    CM_TRANS_DEBUG_PRINT ("UCMDM_Setrole_permissions Failed");
  }
  else
  {
    CM_TRANS_DEBUG_PRINT ("permissions succesfull");
  }


  RolePerm2.permissions=0;
  if (cm_dm_get_instance_role_permissions_by_role (tnsprt_channel_p, CM_CLI_MGMT_ENGINE_ID,
                                "IGD.VSG{general}", CM_DM_DEFAULT_ROLE,
                                &RolePerm2) != OF_SUCCESS)
  {
    CM_TRANS_DEBUG_PRINT ("GetPermissions ByRole Failed");
  }
	else
	{
    CM_TRANS_DEBUG_PRINT (" name = %s perms = %d", RolePerm2.role, RolePerm2.permissions);
	}



  if (cm_dm_get_instance_role_permissions (tnsprt_channel_p, CM_CLI_MGMT_ENGINE_ID,
                                        "IGD.VSG{general}", CM_DM_DEFAULT_ROLE,
                                        &role_perm_ary_p) != OF_SUCCESS)
  {
    CM_TRANS_DEBUG_PRINT ("GetPermissions Failed");
  }
  else
  {
    CM_TRANS_DEBUG_PRINT ("Number of Roles %u at IGD.VSG{general}", role_perm_ary_p->count_ui);
    for (i = 0; i < role_perm_ary_p->count_ui; i++)
    {
      CM_TRANS_DEBUG_PRINT ("Role=%s Perm=%u",
                               role_perm_ary_p->role_permissions[i].role,
                               role_perm_ary_p->role_permissions[i].permissions);
		}

		of_free(role_perm_ary_p->role_permissions);
		of_free(role_perm_ary_p);
		role_perm_ary_p = NULL;
	}

#endif

}

#endif /* CM_TRANSPORT_CHANNEL_SUPPORT */
