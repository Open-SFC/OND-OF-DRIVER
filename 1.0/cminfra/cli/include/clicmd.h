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
 * File name:  clicmd.h
 * Author: Freescale Semiconductor    
 * Date:   01/20/2013
 * Description: 
*/

#ifndef _CLICMD_H_
#define _CLICMD_H_

#define CM_CLI_MAX_DATA_TAG_SIZE 15
#define CM_CLI_CMD_NAME_LEN 32
#define CM_CLI_MAX_ADDCMD_PARAMS 2

/* Enum for CLI Data Types */
enum cm_cli_data_type
{
  CM_CLI_NOMORE,
  CM_CLI_UNKNOWN,
  CM_CLI_NONE,
  CM_CLI_CHAR,
  CM_CLI_INT,
  CM_CLI_UINT,
  CM_CLI_ARRAY,
  CM_CLI_IPADDR,
  CM_CLI_MACADDR,
};

/* Enum for CLI Parameter Type */
enum cm_cli_param_type
{
  CM_CLI_OPTIONAL = 0,
  CM_CLI_MANDATORY
};

#define CM_CLI_MAX_CMDPARAMS  200

/* CLI Parameter structure  */
struct cm_cli_params
{
  enum cm_cli_data_type param_data_type;      /* Parameter data type */
  ulong param_value;          /* Parameter type Optional / Mandatory */
};

/* CLI Parameter definition structure */
struct cm_cli_params_def
{
  enum cm_cli_data_type param_data_type;      /* Parameter data type */
  uint16_t param_size;          /* Parameter size */
  enum cm_cli_param_type param_type;  /* Parameter type Optional / Mandatory */
};

/*
 * Enum for CLI default commands. 
 */
enum cm_cli_default_cmd
{
  CM_CLI_CMD_LS,
  CM_CLI_CMD_HELP,
  CM_CLI_CMD_QHELP,
  CM_CLI_CMD_CD,
  CM_CLI_CMD_CD_PARENTDIR,
  CM_CLI_CMD_SHOW,
  #ifdef CM_STATS_COLLECTOR_SUPPORT
  CM_CLI_CMD_SHOW_STATS,
  #endif
  CM_CLI_CMD_SHOWALL,
  CM_CLI_CMD_DEL,
  CM_CLI_CMD_FACTRESET,
  CM_CLI_CMD_FLUSH,
  CM_CLI_CMD_SAVE,
  CM_CLI_CMD_SHOW_SESSION,
  CM_CLI_CMD_SET_DEFAULT,
  CM_CLI_CMD_COMMIT,
  CM_CLI_CMD_REVOKE,
  CM_CLI_CMD_NO,
  CM_CLI_CMD_COMPARE,
  CM_CLI_CMD_SET,
  CM_CLI_CMD_CANCEL,
  CM_CLI_CMD_DONE,
  CM_CLI_CMD_GET_VERSIONS,
  CM_CLI_CMD_REBOOT_DEVICE,
#ifdef   CM_ROLES_PERM_SUPPORT
  CM_CLI_CMD_SET_ROLE_N_PERM,
  CM_CLI_CMD_GET_ROLE_N_PERM,
  CM_CLI_CMD_DEL_ROLE_N_PERM,
#endif /*   CM_ROLES_PERM_SUPPORT*/
  CM_CLI_CMD_SETCERT,
  CM_CLI_MAX_CMD,
};

struct cm_cli_cmd
{
  char command_name_c[CM_CLI_CMD_NAME_LEN + 1];
  char *description_p;
  unsigned char session_b;
  uint16_t param_count_us;
  struct cm_cli_params_def *param_list_p;
    int32_t (*call_back_function_p) (struct cm_cli_session * cli_session_p,
                                         struct cm_cli_params * params_p,
                                         uint32_t param_cnt_ui);
};

typedef int32_t (*UCMCLIFUNPTR) (struct cm_cli_session * cli_session_p,
                                 struct cm_cli_params * params_p, uint32_t param_cnt_ui);

/* Default Command callback prototypes */
int32_t cm_cli_display_commands (struct cm_cli_session *, struct cm_cli_params *, uint32_t);
int32_t cm_cli_change_directory (struct cm_cli_session *, struct cm_cli_params *, uint32_t);
int32_t cm_cli_move_to_prev_directory (struct cm_cli_session *, struct cm_cli_params *,
                                   uint32_t);
int32_t cm_cli_commit_config_session (struct cm_cli_session *, struct cm_cli_params *,
                                   uint32_t);
int32_t cm_cli_cancel_command(struct cm_cli_session *, struct cm_cli_params *,
                                   uint32_t);
int32_t cm_cli_done_command(struct cm_cli_session *, struct cm_cli_params *,
                                   uint32_t);

int32_t cm_cli_get_versions(struct cm_cli_session * ,
                                   struct cm_cli_params * ,
                                   uint32_t );
void cm_cli_reboot_device_in_config_session(struct cm_cli_session *cli_session_p);
int32_t cm_cli_reboot_device(struct cm_cli_session * ,
                                   struct cm_cli_params * ,
                                   uint32_t );
int32_t cm_cli_revoke_config_session (struct cm_cli_session *, struct cm_cli_params *,
                                   uint32_t);
int32_t cm_cli_cancel_prev_command (struct cm_cli_session *, struct cm_cli_params *, uint32_t);
int32_t cm_cli_compare (struct cm_cli_session *, struct cm_cli_params *, uint32_t);
int32_t cm_cli_display_records (struct cm_cli_session *, struct cm_cli_params *, uint32_t);
#ifdef CM_STATS_COLLECTOR_SUPPORT
int32_t cm_cli_display_statistics (struct cm_cli_session *, struct cm_cli_params *, uint32_t);
#endif
int32_t cm_cli_display_all_records (struct cm_cli_session *, struct cm_cli_params *, uint32_t);
int32_t cm_cli_delete_record (struct cm_cli_session *, struct cm_cli_params *, uint32_t);
int32_t cm_cli_display_session (struct cm_cli_session *, struct cm_cli_params *, uint32_t);
int32_t cm_cli_set_factory_defaults (struct cm_cli_session *, struct cm_cli_params *,
                                  uint32_t);
int32_t cm_cli_flush_command(struct cm_cli_session * cli_session_p,struct cm_cli_params * params_p, uint32_t param_cnt_ui);

int32_t cm_cli_set_default_parameters (struct cm_cli_session *, struct cm_cli_params *, uint32_t);
int32_t cm_cli_display_record (struct cm_cli_session *, struct cm_cli_params *, uint32_t);
int32_t cm_cli_display_all_tables (struct cm_cli_session *, struct cm_cli_params *, uint32_t);
int32_t cm_cli_display_child_node_names (struct cm_cli_session *, struct cm_cli_params *,
                                     uint32_t);

int32_t cm_cli_frame_and_send_set_params_to_je (struct cm_cli_session *, struct cm_cli_params *,
                                         uint32_t);
int32_t cm_cli_logout (struct cm_cli_session *, struct cm_cli_params *, uint32_t);
int32_t cm_cli_save_config (struct cm_cli_session *, struct cm_cli_params *, uint32_t);
int32_t cm_cli_set_role_and_permissions(struct cm_cli_session * cli_session_p,
                                         struct cm_cli_params * params_p,
                                         uint32_t param_cnt_ui);

int32_t cm_cli_get_role_and_permissions(struct cm_cli_session * cli_session_p,
                                         struct cm_cli_params * params_p,
																				 uint32_t param_cnt_ui);

int32_t cm_cli_delete_role_and_permissions(struct cm_cli_session * cli_session_p,
                                         struct cm_cli_params * params_p,uint32_t param_cnt_ui);

int32_t cm_cli_set_certificate_command(struct cm_cli_session *, struct cm_cli_params *,
                                   uint32_t);

#endif /* _CLICMD_H_ */
