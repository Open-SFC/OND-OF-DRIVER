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
 * File name: jegif.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/26/2013
 * Description: This file contains defines and structure
 *              definitions provided by UCM Job Execution engine.
 * 
 */

#ifndef UCMJE_GIF_H
#define UCMJE_GIF_H

/*****************************************************************
  UCM JE Interface with Management Engines
  These following API are available to Management engines in the
  form of library.
 *****************************************************************/
/* First Management engine has to create a transport channel  */
int32_t cm_set_transport_channel_props (void * tnsprt_channel_p,
                                     uint32_t property_id_ui,
                                     void * property_data_p);
//int32_t ucmJECloseTransportChannel(void* tnsprt_channel_p);

/* Config session related API 
                      Start : once
                      UpdateCmd : multiple times
                      End   : once
 */

void *cm_config_session_start (uint32_t mgmt_engine_id,
                               struct cm_role_info * admin_role_info,
                               void * tnsprt_channel_p);

int32_t cm_config_session_update_cmd (void * config_session_p,
                                   struct cm_command * command_p,
                                   struct cm_result ** result_p);
int32_t cm_config_session_update_cmds (void * config_session_p,
                                  struct cm_command *command_p,
                                          int32_t count_i,
                                 struct cm_result ** presult_p);

int32_t cm_config_session_end (void * config_session_p,
                             uint32_t operation_ui, struct cm_result ** result_pp);

int32_t cm_config_session_get_cmd (void * config_session_p,
                                struct cm_command * command_p,
                                struct cm_result ** presult_p);
int32_t cm_validate_login_auth_req(uint32_t mgmt_engine_id,
                                unsigned char *user_name_p,
                                unsigned char *password_p,
                                struct cm_role_info * role_info_p,
                                void * tnsprt_channel_p);

/* Information retrieval API : No need of config session */

int32_t cm_get_first_n_records (void * tnsprt_channel_p,
                             uint32_t mgmt_engine_id,
                             char * admin_role_p,
                             char * dm_path_p,
                             struct cm_array_of_nv_pair * keys_array_p,
                             uint32_t * pcount_ui,
                             struct cm_array_of_nv_pair ** out_array_of_nv_pair_arr_p);

int32_t cm_get_next_n_records (void * tnsprt_channel_p,
                            uint32_t mgmt_engine_id,
                            char * admin_role_p,
                            char * dm_path_p,
                            struct cm_array_of_nv_pair * prev_rec_key_p,
                            struct cm_array_of_nv_pair * keys_array_p,
                            uint32_t * pcount_ui,
                            struct cm_array_of_nv_pair ** out_array_of_nv_pair_arr_p);

int32_t cm_get_exact_record (void * tnsprt_channel_p,
                           uint32_t mgmt_engine_id,
                           char * admin_role_p,
                           char * dm_path_p,
                           struct cm_array_of_nv_pair * keys_array_p,
                           struct cm_array_of_nv_pair ** out_array_of_nv_pair_arr_p);

#ifdef CM_STATS_COLLECTOR_SUPPORT
int32_t cm_get_stats_records (void * tnsprt_channel_p,
                           uint32_t mgmt_engine_id,
                           char * admin_role_p,
                           char * dm_path_p,
                           char stats_sub_cmd_ui,
                           struct cm_array_of_nv_pair * keys_array_p,
                           uint32_t *rec_count_p,
                           struct cm_array_of_nv_pair ** out_array_of_nv_pair_arr_p);
#endif 

int32_t cm_je_get_err_msg (int32_t err_index_i, char * err_arr_p);
int32_t cm_je_get_success_msg (int32_t success_code_i, char * success_msg_p);

typedef int32_t (*cm_auth_user) (unsigned char *user_name_p, unsigned char * password_p,
                                struct cm_role_info *role_info_p);

int32_t cm_get_delta_version_history_t ( uint32_t mgmt_engine_id,
		struct cm_role_info * role_info_p, void * tnsprt_channel_p,
		uint64_t from_version_ui, uint64_t end_version, uint32_t *ver_cnt_ui_p,  struct cm_version **version_list_pp);

int32_t cm_send_notification_to_je ( uint32_t mgmt_engine_id,
		struct cm_role_info * role_info_p, void * tnsprt_channel_p,
		uint32_t notification_ui, void *notify_data_p);
/*****************************************************************
  UCM JE Interface with Security Application's callback glue layer
 *****************************************************************/
//int32_t cm_init (void);
#endif /* UCMJE_GIF_H */
