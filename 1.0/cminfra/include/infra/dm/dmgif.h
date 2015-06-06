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
 * File name: dmgif.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/26/2013
 * Description: This file contains API declarations of config 
 *              middle-ware data model. These routines are useful
 *              to create and manage various data nodes.
 * 
 */

#ifndef UCMDM_GIF_H
#define UCMDM_GIF_H

/*
    Data model interface overview : 

           +--------+   +--------+  +--------+  +-------+
           |  CLI   |   |  HTTP  |  | LDSV   |  | TR069 |
           +--------+   +--------+  +--------+  +-------+
               |             |          |          |
               V             V          V          V
           +----------------CMJE API--------------------+    +------------+
           |    Unified Data Configuration Interface    |<-->| iGateway   | 
           |    (Config Job Execution Daemon : CMJE)    |    | Data Model |
           +--------------------------------------------+    | (UCMDM)     |
                                |                            +------------+
                                V
           +-------------- Toolkit API -----------------+ 
           |    Application specific Glue Layer         |
           +--------------------------------------------+   
                                |
                                V
                    Application specific 'C' API

*/

/**********************************************************************
   API provided by Data model to create, delete Data model nodes,
   To modify, read node attributes, node children, etc.
 ***********************************************************************/

/* Node, and member Registration API */
int32_t cm_dm_register_data_elements (uint32_t appl_id_ui,uint32_t channel_id_ui,char * path_p,
                                    uint32_t element_count_ui,
                                    struct cm_dm_data_element * element_array_p);
int32_t cm_dm_register_app_callback_fns (char * path_p,
                                      struct cm_dm_app_callbacks * app_cbks_p);

int32_t cm_dm_register_scalar_appl_id(uint32_t appl_id_ui,uint32_t chann_id_ui,char * path_p,
                                    struct cm_dm_data_element * scalar_element_p);

/* Node information retrieval API */
int32_t cm_get_dm_node_info (void * tnsprt_channel_p,
                          struct cm_command * command_p,
                          uint32_t mgmt_engine_id,
                          struct cm_role_info * role_info_p,
                          struct cm_dm_node_info ** node_info_pp);

int32_t cm_dm_get_node_info (void * tnsprt_channel_p,
                           char * dm_path_p,
                           uint32_t mgmt_engine_id,
                           struct cm_role_info * role_info_p,
                           struct cm_dm_node_info ** node_info_pp);

int32_t cm_dm_get_node_and_child_info (void * tnsprt_channel_p,
                                   char * dm_path_p,
                                   uint32_t mgmt_engine_id,
                                   struct cm_role_info * role_info_p,
                                   struct cm_dm_node_info ** node_info_pp,
                                   struct cm_array_of_structs ** child_info_array_p);

int32_t cm_dm_get_key_childs (void * tnsprt_channel_p,
                                   char * dm_path_p,
                                   uint32_t mgmt_engine_id,
                                   struct cm_role_info * role_info_p,
                                   struct cm_array_of_structs ** child_info_array_pp);

int32_t cm_dm_get_key_child_info (void * tnsprt_channel_p,
                               char * dm_path_p,
                               uint32_t mgmt_engine_id,
                               struct cm_role_info * role_info_p,
                               struct cm_dm_node_info ** node_info_pp);

int32_t cm_dm_get_first_node_info (char * head_node_path_p,
                                uint32_t traversal_type_ui,
                                void * tnsprt_channel_p,
                                uint32_t mgmt_engine_id,
                                struct cm_role_info * role_info_p,
                                struct cm_dm_node_info ** node_info_pp,
                                void ** opaque_info_pp, uint32_t * opaque_len_p);

int32_t cm_dm_get_next_node_info (char * head_node_path_p,
                               uint32_t traversal_type_ui,
                               void * tnsprt_channel_p,
                               uint32_t mgmt_engine_id,
                               struct cm_role_info * role_info_p,
                               struct cm_dm_node_info ** node_info_pp,
                               void ** opaque_info_pp, uint32_t * opaque_len_p);

int32_t cm_dm_get_next_table_n_anchor_node_info (char * head_node_path_p,
                               uint32_t traversal_type_ui,
                               void * tnsprt_channel_p,
                               uint32_t mgmt_engine_id,
                               struct cm_role_info * role_info_p,
                               struct cm_dm_node_info ** node_info_pp,
                               void ** opaque_info_pp, uint32_t * opaque_len_p);

unsigned char cm_dm_is_child_element (char * child_path_p,
                              char * parent_path_p,
                              void * tnsprt_channel_p,
                              uint32_t mgmt_engine_id,
                              struct cm_role_info * role_info_p);

int32_t cm_dm_get_keys_array_from_name_path (void * tnsprt_channel_p,
                                        char * dm_path_p,
                                        uint32_t mgmt_engine_id,
                                        struct cm_role_info * role_info_p,
                                        struct cm_array_of_nv_pair ** keys_array_pp);

int32_t cm_dm_get_instance_map_entry_list_using_name_path (void * tnsprt_channel_p,
                                                     char * dm_path_p,
                                                     uint32_t mgmt_engine_id,
                                                     struct cm_role_info * role_info_p,
                                                     uint32_t * pcount_ui,
                                                     struct cm_dm_instance_map **
                                                     map_pp);

unsigned char cm_dm_is_instance_map_entry_using_key_and_name_path (char * node_path_p,
                                                      void * tnsprt_channel_p,
                                                      uint32_t mgmt_engine_id,
                                                      struct cm_role_info * role_info_p,
                                                      void * key_p,
                                                      uint32_t key_type);
struct cm_dm_data_element *cm_dm_get_root_node (void);
int32_t cm_dm_print_tree (struct cm_dm_data_element * data_element_p);


int32_t cm_dm_get_file_name(void * tnsprt_channel_p,
			uint32_t mgmt_engine_id,
			char * dm_path_p,
			char *role_name_p,
			char **file_name_p
			);

int32_t cm_dm_get_directory_name(void * tnsprt_channel_p,
			uint32_t mgmt_engine_id,
			char * dm_path_p,
			char *role_name_p,
			char **directory_name_p
			);


int32_t cm_dm_get_role_permissions (void * tnsprt_channel_p,
			uint32_t mgmt_engine_id,	char * dm_path_p,
			char *role_name_p,	struct cm_dm_array_of_role_permissions  **role_perm_ary_pp);

int32_t cm_dm_get_role_permissions_by_role (void * tnsprt_channel_p,
			uint32_t mgmt_engine_id,	char * dm_path_p,
			char *role_name_p,	struct cm_dm_role_permission * role_perm_p);

int32_t cm_dm_set_role_permissions_by_role (void * tnsprt_channel_p,
			uint32_t mgmt_engine_id,	char * dm_path_p,
			char *role_name_p,	struct cm_dm_role_permission * role_perm_p, struct cm_result **result_p);

int32_t cm_dm_delete_role_permissions_by_role (void * tnsprt_channel_p,
			uint32_t mgmt_engine_id,	char * dm_path_p,
			char *role_name_p,	struct cm_dm_role_permission * role_perm_p);

int32_t cm_dm_get_instance_role_permissions(void * tnsprt_channel_p,
			uint32_t mgmt_engine_id,	char * dm_path_p,
			char *role_name_p,
			struct cm_dm_array_of_role_permissions **role_perm_ary_pp);

int32_t cm_dm_set_instance_role_permissions_by_role(void * tnsprt_channel_p,
			uint32_t mgmt_engine_id,	char * dm_path_p,
			char *role_name_p,
			struct cm_dm_role_permission * role_perm_p);

int32_t cm_dm_get_instance_role_permissions_by_role(void * tnsprt_channel_p,
			uint32_t mgmt_engine_id,	char * dm_path_p,
			char *role_name_p,
			struct cm_dm_role_permission * role_perm_p);

int32_t cm_dm_delete_instance_role_permissions_by_role(void * tnsprt_channel_p,
			uint32_t mgmt_engine_id,	char * dm_path_p,
			char *role_name_p,
			struct cm_dm_role_permission * role_perm_p);

#ifdef UCMDM_INSTANCE_ROLE_AND_PERMISSIONS_USING_KEY
int32_t cm_dm_set_instance_role_permissions(void * tnsprt_channel_p,
			uint32_t mgmt_engine_id,	char * dm_path_p,
			char *role_name_p,void *key_p, uint32_t  key_type,
			struct cm_dm_role_permission * role_perm_p);

int32_t cm_dm_get_instance_role_permissions(void * tnsprt_channel_p,
			uint32_t mgmt_engine_id,	char * dm_path_p,
			char *role_name_p,void *key_p, uint32_t  key_type,
			struct cm_dm_role_permission * role_perm_p);

int32_t cm_dm_delete_instance_role_permissions(void * tnsprt_channel_p,
			uint32_t mgmt_engine_id,	char * dm_path_p,
			char *role_name_p,void *key_p, uint32_t  key_type,
			struct cm_dm_role_permission * role_perm_p);
#endif

unsigned char cm_dm_is_instance_map_entry_using_key_array_and_name_path (char * node_path_p,
                                                      void * tnsprt_channel_p,
                                                      uint32_t mgmt_engine_id,
                                                      struct cm_role_info * role_info_p,
                                                      struct cm_array_of_nv_pair *nv_pair_arr_p);

int32_t cm_dm_register_role_permissions(char * path_p,
                                      struct cm_dm_role_permission  * role_perm_p);

int32_t cm_register_channel(uint32_t channel_id,struct cm_dm_channel_callbacks *chan_callbacks_p);
//int32_t cm_register_app_callbacks (void);
int32_t cm_init (void);
#endif /* DM_GIF_H */
