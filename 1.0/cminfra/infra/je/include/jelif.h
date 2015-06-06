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
 * File name: jelif.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/05/2013
 * Description: 
*/

#ifndef UCMJE_LIF_H
#define UCMJE_LIF_H

/*****************************************************************************
 * * * * * * * * * * * I N I T   F U N C T I O N S * * * * * * * * * * * * * *
 *****************************************************************************/
int32_t cm_je_daemon (void);
int32_t cm_je_create_listen_socket (void);
void cm_je_ldsv_init (void);

/*****************************************************************************
 * * * * * * R E Q U E S T  P R O C E S S I N G  F U N C T I O N S * * * * * *
 *****************************************************************************/
int32_t cm_je_read_and_process_config_request (cm_sock_handle ulFd);

/*****************************************************************************
 * * * * * * * *  J E   H A N D L E R   F U N C T I O N S * * * * * * * * * * 
 *****************************************************************************/
int32_t cm_je_config_session_handler (void * pInCfgSession,
      struct je_config_request * config_request_p,
      struct cm_result ** presult_p);

int32_t cm_je_appl_show_request_handler (struct je_config_request * config_request_p,
      struct cm_result ** presult_p);

int32_t cm_je_dm_get_request_handler (struct je_config_request * config_request_p,
      struct cm_result ** presult_p);

int32_t cm_je_ldsv_handler (struct je_config_request * config_request_p,
      struct cm_result ** presult_p);

int32_t cm_je_version_history_t_handler (struct je_config_request * config_request_p,
      struct cm_result ** presult_p);

int32_t cm_je_rebootdevice_handler (struct je_config_request * config_request_p,
      struct cm_result ** presult_p);

int32_t cm_je_notification_handler (struct je_config_request * config_request_p,
      struct cm_result ** presult_p);

int32_t cm_je_read_notifications(uint32_t mgmt_engine_id, uint32_t uiNotification, void *pData, struct cm_result *result_p);

int32_t cm_tnsprt_channel_properties_handler (struct je_config_request * config_request_p,
      struct cmje_tnsprtchannel_info * tnsprt_channel_p,
      struct cm_result ** presult_p);

int32_t cm_je_authentication_request_handler(struct je_config_request * config_request_p,struct cm_result ** presult_p);

#ifdef CM_STATS_COLLECTOR_SUPPORT
int32_t cm_je_stats_request_handler (struct cmje_tnsprtchannel_info *tnsprt_channel_p,
                                  struct je_config_request * config_request_p,
                                            struct cm_result ** presult_p);
int32_t cm_je_fill_error(char *dm_path_p, struct cm_result *presult_p, int32_t error_code);
int32_t cm_je_send_request_to_stats (struct je_config_request * config_request_p,
                             struct cmje_tnsprtchannel_info *pCurrentChannel,
                                           struct cm_result ** presult_p);
#endif


/*****************************************************************************
 * * * * * * T R A N S P O R T  C H A N N E L   F U N C T I O N S * * * * * *
 *****************************************************************************/
int32_t cmje_add_tnsprtchannel_to_list (struct cmje_tnsprtchannel_info * tnsprt_channel_p);

int32_t cmje_delete_tnsprtchannel_from_list (struct cmje_tnsprtchannel_info * tnsprt_channel_p);

struct cmje_tnsprtchannel_info *cmje_get_tnsprtchannel_from_list (cm_sock_handle ulFd);

struct cmje_tnsprtchannel_info *cmje_get_parent_tnsprtchannel_from_list (cm_sock_handle fd_ui);

int32_t cmje_create_recv_tnsprtchannel (cm_sock_handle ulFd);

int32_t cmje_tnsprtchannel_close_session (struct je_config_session * config_session_p);

int32_t cmje_close_recv_tnsprtchannel (cm_sock_handle ulFd);

void cmje_tnsprtchannel_timeout (void * pArg);


/*****************************************************************************
 * * * * * * R O L E  & P E R M I S S I O N   F U N C T I O N S * * * * * * *
 *****************************************************************************/
int32_t cm_verify_role_permissions (char * dm_path_p,
      char * role_name_p, uint32_t command_id);

/*****************************************************************************
 * * * * * * C O N F I G  S E S S I O N   F U N C T I O N S * * * * * *
 *****************************************************************************/
struct je_config_session *cmje_config_session_start (struct je_config_request *
      config_request_p);

int32_t cmje_generate_session_id (uint32_t * pNewId);

int32_t cmje_cfgsess_clean_cache (struct je_config_session_cache * pCfgCache);

int32_t cmje_delete_subtree (char * dm_path_p,
      struct cm_array_of_nv_pair * pnv_pair_array,
      struct cm_result * result_p,
      unsigned char bDelInst,
      int32_t state,
      uint32_t mgmt_engine_id);

int32_t cmje_execute_delete_command (struct cm_dm_data_element * pDMNode,
      struct cm_array_of_iv_pairs * pNewKeyIVPairArray,
      struct cm_result * result_p, int32_t state);

/*****************************************************************************
 * * * J E  I N S T A N C E I D  G E N E R A T O R  F U N C T I O N S * * * * *
 *****************************************************************************/
/* JE Instance IDs Generator */
int32_t je_instanceid_generator_init (void);

void je_instanceid_generator_deinit (void);

int16_t je_get_new_instance_id (uint32_t * pInstanceId_p);

int16_t je_set_bit_for_instance_id (uint32_t ulInstanceId_p, unsigned char bSetFlag_p);

void je_set_instance_id_value (uint32_t ulInstanceVal_p);

/*****************************************************************************
 * * * * *  * * * * * * L D  S V   F U N C T I O N S * * * * * * * * * * * * *
 *****************************************************************************/
int32_t cmje_process_pending_request (void );

void *cmje_establish_tnsprtchannel_with_ldsv (void);

void cmje_terminate_tnsprtchannel_with_ldsv (struct cm_tnsprt_channel *
      tnsprt_channel_p);

void cm_je_trace (int32_t iModuleId, unsigned char ucLevel, char *pFunc, int32_t ulLine,
      char * pFormatString,
      ...);
#endif
