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
 * File name:  clilif.h
 * Author: Freescale Semiconductor    
 * Date:   01/20/2013
 * Description: 
*/

#ifndef CLI_LIF_H
#define CLI_LIF_H

void cm_cli_set_prompt (struct cm_cli_session * cli_session_p);
int32_t cm_cli_init_session_pool (void);
struct cm_cli_session *ucm_cli_alloc_session (void);
int32_t cm_cli_free_session (struct cm_cli_session * cli_session_p);
void cm_cli_clean_sessions (void);
char *cm_cli_get_context_prompt (edit_line * edit_line_ctx_p);

void cm_cli_handle_signal (int16_t sig_num_i);

unsigned char cm_cli_auto_complete (edit_line * el, int ch);

void cm_cli_display_result (struct cm_result * result_p);

void cm_cli_update_prompt (char * param_value_p);
void cm_cli_get_dmpath_from_prompt (char * prompt_p, char * dm_path_p);
enum tok_stat cm_cli_get_token (char * path_string, char * token,
                          uint8_t * index, int32_t param_cnt_i);

void cm_cli_convert_dir_to_dmpath (char * prompt_p, char * dm_path_p);

void cm_cli_free_child_nodes (struct cm_cli_node * current_node_p);
void cm_cli_append_child_nodes (struct cm_cli_node * current_node_p,
                               struct cm_array_of_strings * child_names_p);

int32_t cm_cli_interpret_command (/* volatile */struct cm_cli_session * cli_session_p, /*const*/volatile char * line_p,volatile unsigned char is_tab_pressed_b);

int16_t cm_cli_puts (struct cm_cli_session * cli_session_p, char * buff_p);

int16_t cm_cli_gets (struct cm_cli_session * cli_session_p, char * buff);

int32_t cm_cli_find_directory (char * param_p, char * dir_path_p, unsigned char find_b, int32_t param_cnt_i);

void cm_cli_copy_data_element_info (struct cm_cli_data_ele_node * node_p,
                              struct cm_dm_node_info * node_info_p);
void cm_cli_fill_command_info (int32_t cmd_i, char * dm_path_p,
                              int32_t count_i, struct cm_nv_pair * nv_pair_p,
                              struct cm_command * cmd_p);

void cm_cli_create_child_node_mempool (void);
void *cm_cli_child_node_alloc (void);
void cm_cli_child_node_free (struct cm_cli_node * free_p);
void cm_cli_create_data_element_node_mempool (void);
void *cm_cli_data_element_node_alloc (void);
void cm_cli_data_element_node_free (struct cm_cli_data_ele_node * free_p);
int32_t cm_cli_frame_and_send_add_params_to_je (struct cm_cli_params *params_p,
                                         int32_t param_cnt_i);
int32_t cm_cli_update_child_list_and_prompt (char * dm_path_p, char * prompt_p,
                                        unsigned char add_b, unsigned char *is_table_b);

int32_t cm_cli_display_instances_and_key_params (char * table_name_p,unsigned char system_command_b);
void cm_cli_restore_prompt (void);

void cm_cli_terminate_transport_channel (struct cm_tnsprt_channel * tnsprt_channel_p);

void *cm_cli_establish_transport_channel (void);
int32_t cm_cli_set_transport_channel_attribs (void * tnsprt_channel_p,
                                          uint16_t attrib_id_ui,
                                          void * attrib_data_p,
                                          struct cm_role_info * role_info_p);
void cm_cli_free_array_of_strings (void);
void cm_cli_mem_init (void);

void cm_cli_free_instance_map_list (struct cm_dm_instance_map * map_p, int32_t
count_ui);
void cm_cli_display_nv_pair(uint32_t count_ui, struct cm_array_of_nv_pair
*nv_pair_array_p,char* dm_path_p);
int32_t cm_cli_validate_path (char * path_p);
int32_t cm_cli_validate_and_change_dir(char *path_p);
void cm_cli_display_config_session (struct cm_array_of_commands * cmd_array_p);
int32_t cm_cli_display_attrib_info (struct cm_cli_data_ele_node * data_ele_node_p,char (*tmp_sort_buff)[512],int32_t tmp_attrib_cnt_i);
int32_t cm_cli_validate_attribute (struct cm_nv_pair * nv_pair_p , char *dm_path_p);
int16_t cm_cli_get_char (struct cm_cli_session * cli_session_p, char * char_p);

int32_t cm_cli_get_user_name(unsigned char* user_name_p);
int32_t cm_cli_get_password(unsigned char* password_p);

int32_t cm_cli_user_login(unsigned char *user_name_p, unsigned char *password_p);
int32_t cm_cli_get_server_ip(uint32_t *server_ip_p);

int32_t cm_cli_prepare_keys_array_from_token(char *dm_path_p, char *key_p, struct cm_array_of_nv_pair **out_nv_array_p);
void cm_cli_cleanup_array_of_node_info (struct cm_array_of_structs * arr_node_info_structs_p);
void cm_cli_free_node_info (struct cm_dm_node_info * node_info_p);
int32_t cm_cli_return_enum_string(struct cm_array_of_nv_pair * in_nv_pair_array_p,char *dm_path_p, uint32_t array_cnt_ui);
void cm_cli_cleanup(void);
#endif
