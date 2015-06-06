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
 * File name:   ldsvlif.h
 * Author:      Freescale Semiconductor    
 * Date:        06/12/2013
 * Description: This header file contains LDSV engine specific 
 *              local function declarations.
*/

#ifndef CM_LDSV_LIF_H
#define CM_LDSV_LIF_H

cm_ldsv_table_stack_node_t *cm_ldsv_create_table_stack_node (struct cm_dm_node_info *
                                                        pDMNodeAttrib,
                                                        char * dm_path_p,
                                                        char * name_p,
                                                        void * pOpaqueInfo,
                                                        uint32_t
                                                        uiOpaqueInfoLen,
                                                        struct cm_array_of_nv_pair * pArrayOfKey);

int32_t cm_ldsv_fill_nv_pair_info (struct cm_nv_pair * pNvPair,
                                char * pElementName,
                                uint32_t uiKeyType,
                                uint32_t uiKeyLen, void * pKey);

struct cm_array_of_nv_pair *cm_ldsv_find_key_nv_pair (struct cm_dm_node_info * DMNodeAttrib,
                                    struct cm_array_of_nv_pair * pArrayOfKeys,
                                    void * pTransportChannel);

/* Debug routines */

int32_t cm_ldsv_write_to_file (struct cm_array_of_nv_pair * pMembers,
                             struct cm_hash_table * pHashTable,
                             char* pFullPath, struct cm_dm_node_info *pDMNodeAttrib,
                             void * pTransportChannel);

/* Utility routines. Should be in common place */
void cm_ldsv_clean_nv_pair_array (struct cm_array_of_nv_pair * pArrayOfNvPairs,
                                 unsigned char bDeleteContainer);

void cm_ldsv_cleanup_table_stack_node (cm_ldsv_table_stack_node_t *
                                      pTableStackNode);

void cm_ldsv_delete_table_stack (UCMDllQ_t * pTableStackNode);

struct cm_array_of_nv_pair *cm_ldsv_collect_keys_from_table_stack (UCMDllQ_t *pTableStackDll,
                                              struct cm_array_of_nv_pair *dm_path_pArrayOfKeys);

int32_t cm_ldsv_update_table_stack_node_key_nv_pair (cm_ldsv_table_stack_node_t * pNode,
                                               struct cm_array_of_nv_pair* pKeyNvPair);

struct cm_array_of_nv_pair *cm_ldsv_copy_array_of_nv_pairs (struct cm_array_of_nv_pair *
                                                 pArrayOfNvPairs);

void cm_ldsv_clean_tree_traversal_opaque_info (void * pOpaqueInfo,
                                             uint32_t uiOpaqueInfoLen);

void cm_ldsv_clean_dm_node_attrib_info (struct cm_dm_node_info * DMNodeAttrib);

int32_t cm_ldsv_set_poll_fd_list (struct cm_poll_info * pSigDwnDFds);

int32_t search_file (char * name_p);

int32_t ldsv_copy_file (unsigned char * pSrcFile, unsigned char * pDestFile,
                       char * pDirPath);

int32_t cm_ldsv_validate_file_name (unsigned char * pFileName);

int32_t cm_save_config (char * dm_path_p, uint64_t iVersionNo);

int32_t cm_ldsv_create_backup (char * pFileName, char * pDirPath);

void *cm_ldsv_get_transport_channel(void);

int32_t cm_ldsv_init (void);

int32_t cm_ldsv_create_listen_socket (void);

int32_t cm_ldsv_daemon (void);

int32_t cm_factory_reset (char * dm_path_p);

int32_t cm_load_config (char * dm_path_p, unsigned char bFactResetFlag);

int32_t cm_ldsv_search_file (char * name_p, char * pDirPath);

int32_t cmldsv_copy_file (unsigned char * pSrcFile, unsigned char * pDestFile,
                          char * pDirPath);

void cm_ldsv_clean_array_of_nv_pair_array (struct cm_array_of_nv_pair * pArrayOfNvPairs,
                                      uint32_t uiRecordCount);

int32_t cm_ldsv_write_header (char *aFileName, char* aFullPath,
                             char* pTempDirPath, char* aDMInstancePath,
                             struct cm_dm_node_info * DMNodeAttrib);

int32_t cm_ldsv_write_array_of_nv_pairs (struct cm_array_of_nv_pair * pKeys,
                                     struct cm_hash_table * pHashTable,
                                     char* pFullPath, struct cm_dm_node_info *pDMNodeAttrib,
                                      void * pTransportChannel);

void cm_ldsv_write_dictionary (struct cm_dm_node_info * DMNodeAttrib,
                                void * pTransportChannel, 
                                struct cm_hash_table *pDictonaryTable,
                                char* aFullPath);

void cm_ldsv_close_xml_tag (struct cm_array_of_nv_pair *dm_path_pArrayOfKeys,
                            UCMDllQ_t LoadTableStackNode);

char *cm_ldsv_read_line (FILE * pFile);

char *cm_ldsv_trim_string (char * pString);

int32_t cm_ldsv_tokenize_data_and_send_to_je (FILE * input, 
                              char * dm_path_p,
                              char * pInstPath,
                              struct cm_dm_node_info * pDMNodeAttrib,
                              struct cm_array_of_nv_pair * pKeyNvPair, void * pOpaqueInfo,
                              uint32_t uiOpaqueInfoLen,
                              struct cm_hash_table *pHashTable, 
                              UCMDllQ_t *TableStackDll);

int32_t cm_ldsv_tokenize_dictionary (FILE * pFile, struct cm_hash_table * pHashTable);

int32_t cm_ldsv_frame_and_send_add_params_to_je (char aDMPath[],
                                          struct cm_array_of_nv_pair * pnv_pair_array,
                                          void * pTransportChannel, char *dm_path_p,
                                          unsigned char scalar_group_b, unsigned char global_trans_b);

int32_t cm_ldsv_frame_and_send_delete_params_to_je (char * dm_path_p,
                                             struct cm_array_of_nv_pair * pnv_pair_array,
                                             void * pTransportChannel);

void cm_ldsv_fill_command_info (int32_t iCmd, char * dm_path_p,
                               int32_t iCount, struct cm_nv_pair * pNvPair,
                               struct cm_command * pCmd);

int32_t cm_ldsv_get_dm_path_and_instance_path_from_save_file(FILE * pFile, char * dm_path_p,
                                               char * pInstPath);

int32_t cm_ldsv_prepare_instance_dm_path (struct cm_dm_node_info * DMNodeAttrib,
                                       char* pdmPath,
                                       struct cm_array_of_nv_pair *dm_path_pArrayOfKeys,
                                       UCMDllQ_t *TableStackDll);

int32_t cm_ldsv_get_dm_path_token (char * pPathString,
                                char * pToken,
                                uint32_t uiMaxTokenLen, uint32_t * pIndex);

int32_t cm_ldsv_find_dictionary (char * file_name);

int32_t cm_ldsv_create_backup_files (void);

int32_t cm_ldsv_create_directory (char * pDirPath, char * pDirName);

char *cm_ldsv_string_tokenize (char * s, char * ct);

char *cm_ldsv_string_break_p (char * cs, char * ct);

int32_t cm_ldsv_close_open_xml_tags (char * pDirName);

int32_t cm_ldsv_process_file (char * pFileName);

int32_t cm_ldsv_change_directory (struct cm_dm_node_info * DMNodeAttrib,
                                 void * pTransportChannel,
                                 char * pDirPath);

int32_t cm_ldsv_create_directory_path (char * pDMInstancePath, char * pDir);

cm_ldsv_table_stack_node_t *cm_ldsv_get_load_stack_node (UCMDllQ_t *LoadTableStackNode);

struct cm_array_of_nv_pair *cm_ldsv_collect_keys_from_load_table_stack (UCMDllQ_t *pTableStackDll,
                                 struct cm_array_of_nv_pair *dm_path_pArrayOfKeys);

int32_t cm_ldsv_load_file (FILE *pFile,
                          struct cm_dm_node_info * pDMNodeAttrib,
                          struct cm_array_of_nv_pair* pKeyNvPair, void * pOpaqueInfo,
                          uint32_t uiOpaqueInfoLen, UCMDllQ_t *TableStackDll);

int32_t cm_ldsv_delete_load_stack_node (UCMDllQ_t * pTableStackDll);

int32_t cm_ldsv_find_data_in_file (char * pFileName);

struct cm_array_of_nv_pair *cm_ldsv_prepare_array_of_keys (cm_ldsv_table_stack_node_t *pTableStackNode);

 int32_t cm_write_role_permissions_info(struct cm_dm_array_of_role_permissions *pRolePermAry,
                                             char* pFullPath, char * dm_path_p);
 void cm_get_permission_string(uint32_t uiPerm, char* pPerm);
 int32_t cm_get_permission_value(char* pPerm);
int32_t cm_ldsv_load_roles_and_permission (FILE *pFile, void* pTransChannel,
                                       unsigned char bRoleAtTemplate);

void cm_ldsv_frame_and_send_log_message (uint32_t ulMsgId,char *pAdminName,char * pAdminRole,
			uint32_t ulCommand, char * dm_path_p,  char * pInstance,struct cm_array_of_nv_pair *pArrNvPair,
			char *pReason, uint64_t ulVersion);
#endif /* CM_LDSV_LIF_H */
