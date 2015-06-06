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
 * File name: dmlif.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/17/2013
 * Description: This file contains function declarations of config 
 *              middle-ware data model internal routines. These 
 *              routines are useful for traversing and accessing
 *              data model tree structures directly. Very much
 *              useful for Job Execution engine.
*/

#ifndef DM_LIF_H
#define DM_LIF_H

/*****************************************************************************
 * * * * * * * * M A C R O   D E F I N I T I O N S  * * * * * * * * * * * * *
 *****************************************************************************/
/* Node accessing related macros */
#define UCMDM_GET_NODE_NAME(nodeptr)  ((nodeptr)->name_p)
#define UCMDM_GET_NODE_ATTRIBS(nodeptr)  (&((nodeptr)->Attribs))
#define UCMDM_GET_NODE_PARENT(nodeptr)  ((nodeptr)->parent_p)
#define UCMDM_GET_NEXT_PEER_NODE(nodeptr)  CM_DLLQ_NEXT((UCMDllQNode_t*)(nodeptr))
#define UCMDM_GET_FIRST_CHILD_NODE(nodeptr)  (((nodeptr)->pChildrenList) ? CM_DLLQ_FIRST(((struct cm_dm_data_element*)(nodeptr))->pChildrenList) : 0)

int32_t cm_dm_init (void);
int32_t cmi_dm_get_node_info (char * node_path_p,
                           struct cm_dm_node_info ** node_info_pp);
int32_t cm_dm_get_node_children (char * node_path_p,
                               struct cm_array_of_strings ** pChildrenNames);
int32_t cmi_dm_get_node_and_child_info (char * node_path_p,
                                   struct cm_dm_node_info ** node_info_pp,
                                   struct cm_array_of_structs ** pChildNodeInfo_p);
int32_t cmi_dm_get_key_child_nodes (char * node_path_p,
                                   struct cm_array_of_structs ** pChildInfoArr);

int32_t cmi_dm_get_key_array_from_namepath (char * name_path_p,
                                        struct cm_array_of_nv_pair ** keys_array_pp);

/* Node traversal callback routine prototype */
typedef int32_t (*ucmdm_TraverseCallBack_t) (struct cm_dm_data_element * pNode);
#define UCMDM_TRAVERSE_PROCEED 0
#define UCMDM_TRAVERSE_STOP    1

typedef int32_t (*UCMDM_TRAVERSE_CALLBK_FN) (struct cm_dm_node_info * node_info_p);

/* Node accessing related routines */
struct cm_dm_data_element *cmi_dm_get_node_by_namepath (char * name_path_p);
struct cm_dm_data_element *cmi_dm_get_node_by_idpath (struct cm_array_of_uints * pIDArray);

struct cm_dm_data_element *cmi_dm_get_child_by_id (struct cm_dm_data_element * pNode,
                                         uint32_t uiChildID);
struct cm_dm_data_element *cmi_dm_get_child_by_name (struct cm_dm_data_element * pNode,
                                           char * pChildName);
uint32_t cmi_dm_get_key_child_datatype (struct cm_dm_data_element * parent_node_p);
int32_t cmi_dm_get_key_child_info (char * node_path_p,
                               struct cm_dm_node_info ** key_child_info_p);

char *cmi_dm_create_name_path_from_node (struct cm_dm_data_element * pNode);

int32_t cmi_dm_create_id_path_from_node (struct cm_dm_data_element * pNode,
                                    struct cm_array_of_uints * pIDArray);
/* utility routines */
int32_t cmi_dm_attach_child (struct cm_dm_data_element * parent_node_p,
                           struct cm_dm_data_element * pChildNode);
int32_t cmi_dm_attach_role_perm_entry (struct cm_dm_data_element * pDMNode,
                                   ucmdm_RolePermission_t * role_perm_p);
int32_t cmi_dm_dettach_role_perm_entry (struct cm_dm_data_element * pDMNode,
                                    ucmdm_RolePermission_t * role_perm_p);
ucmdm_RolePermission_t *cmi_dm_get_role_perms_entry_by_role (struct cm_dm_data_element *
                                                       pDMNode,
                                                       char * pRole);
int32_t cmi_dm_copy_role_perms (ucmdm_RolePermission_t * pInternalEntry,
                             struct cm_dm_role_permission * pExternalEntry,
                             uint32_t uiCopyDir);
int32_t cmi_dm_verify_role_permissions (struct cm_dm_role_permission * role_perm_p);

int32_t cmi_dm_get_path_token (char * path_string_p,
                            char * pToken,
                            uint32_t uiMaxTokenLen, uint32_t * pIndex);
struct cm_dm_node_info *cmi_dm_create_node_info_struct (struct cm_dm_data_element * pNode,
                                              char * dm_path_p);

int32_t cmi_dm_create_child_info_array (struct cm_dm_data_element * parent_node_p,
                                    struct cm_array_of_structs ** pChildInfoArr);
int32_t cmi_dm_create_child_name_array (struct cm_dm_data_element * parent_node_p,
                                    struct cm_array_of_strings ** arr_str_pp);
int32_t cmi_dm_create_child_node_info_array (struct cm_dm_data_element * parent_node_p,
                                        struct cm_array_of_structs **
                                        pChildNodeInfoArr);
void cmi_dm_cleanup_array_of_strings (struct cm_array_of_strings * arr_str_p);
int32_t cmi_dm_test_compare_node_info (struct cm_dm_node_info * node_info_p,
                                   struct cm_dm_data_element * pNode);
int32_t cmi_dm_get_id_path_from_name_path (char * name_path_p,
                                     struct cm_array_of_uints * pIDArray);
int32_t cmi_dm_get_id_path_from_node (struct cm_dm_data_element * pNode,
                                 struct cm_array_of_uints * pIDArray);
void cmi_dm_reverse_uint_array (struct cm_array_of_uints * pIDArray);
int32_t cm_dm_initDataElementInternals (struct cm_dm_data_element * pNode,
                                        struct cm_dm_app_callbacks * app_cbks_p);
/* synchronous traversal routine */
int32_t cmi_dm_traverse_dm_tree (char * head_node_path_p,
                              uint32_t traversal_type_ui,
                              UCMDM_TRAVERSE_CALLBK_FN pFn);

/* Test routines */
int32_t cmi_dm_register_root_children (void);
int32_t cmi_dm_test_init (void);
int32_t cmi_dm_execute_tests (void);
int32_t cmi_dm_print_node_info (struct cm_dm_node_info * node_info_p);

/* Instance Tree related internal routines */
int32_t cm_dm_initInstanceTree (void);
int32_t cmi_dm_create_root_instance (void);
UCMDM_Inode_t *cmi_dm_get_inode_by_name_path (char * name_path_p);
UCMDM_Inode_t *cmi_dm_get_inode_by_id_path (struct cm_dm_id_path * id_path_p);
struct cm_dm_data_element *cmi_dm_get_dm_node_from_instance_path (char * name_path_p,
                                                      uint32_t uiValidPathLen);
UCMDM_Instance_t *cmi_dm_get_instance_using_key (UCMDM_Inode_t * pInode,
                                             void * key_p, uint32_t key_type);
UCMDM_Instance_t *cmi_dm_get_instance_using_id (UCMDM_Inode_t * pInode,
                                            uint32_t instance_id);
UCMDM_Inode_t *cmi_dm_get_child_inode_by_name (UCMDllQ_t * pChildInodeDll,
                                          char * pChildName);
UCMDM_Inode_t *cmi_dm_get_child_inode_by_mid (UCMDllQ_t * pChildInodeDll,
                                          uint32_t uiDMID);
int32_t cmi_dm_remove_keys_from_path (char * pInPath, char * pOutPath,
                                  uint32_t uiPathLen);
int32_t cmi_dm_get_new_instance_id_from_inode (UCMDM_Inode_t * pInode,
                                         uint32_t * pInstID);

int32_t cmi_dm_add_instance_map_to_inode (UCMDM_Inode_t * pInode,
                                     struct cm_dm_instance_map * pMap);

char *cmi_dm_make_instance_path (char * pSrcInstPath, char * pTplPath);

int32_t cmi_dm_get_instance_map_entry_list_using_name_path (char * node_path_p,
                                                     uint32_t * pcount_ui,
                                                     struct cm_dm_instance_map
                                                     ** map_pp);

int32_t cmi_dm_get_node_info_by_path_ids (struct cm_array_of_uints * path_ids_p,
                                    struct cm_dm_node_info ** node_info_pp);
int32_t cm_dm_get_node_childrenByPathIDs (struct cm_array_of_uints * path_ids_p,
                                        struct cm_array_of_strings ** pChildrenNames);
int32_t cmi_dm_get_node_and_child_info_by_path_ids (struct cm_array_of_uints * path_ids_p,
                                            struct cm_dm_node_info ** node_info_pp,
                                            struct cm_array_of_structs **
                                            pChildNodeInfo_p);


/* DM Tree traversal related API 
*/
int32_t cmi_dm_get_first_node_info (char * node_path_p,
                                uint32_t traversal_type_ui,
                                struct cm_dm_node_info ** node_info_pp,
                                void ** opaque_info_pp, uint32_t * opaque_len_p);

int32_t cmi_dm_get_next_node_info (char * head_node_path_p,
                               uint32_t traversal_type_ui,
                               struct cm_dm_node_info ** node_info_pp,
                               void ** opaque_info_pp, uint32_t * opaque_len_p);

int32_t cmi_dm_get_next_table_n_anchor_node_info (char * head_node_path_p,
                               uint32_t traversal_type_ui,
                               struct cm_dm_node_info **node_info_pp,
                               void **opaque_info_pp, uint32_t *opaque_len_p);
/* Cleanup utilities */
void cmi_dm_cleanup_array_of_node_info (struct cm_array_of_structs * arr_node_info_p);
void cmi_dm_free_node_info (struct cm_dm_node_info * node_info_p);

/**********************************************************************
               Instance ID-KEY Map related API 
 ***********************************************************************/
/* Get New Instance ID */
int32_t cmi_dm_get_new_instance_id_using_name_path (char * node_path_p,
                                              uint32_t * pInstID);
int32_t cmi_dm_get_new_instance_id_using_id_path (struct cm_dm_id_path * id_path_p,
                                            uint32_t * pInstanceID);

/* Add New Instance ID-Key Map Entry */
int32_t cmi_dm_add_instance_map_entry_using_name_path (char * node_path_p,
                                                 struct cm_dm_instance_map * pMap);
int32_t cmi_dm_add_instance_map_entry_using_id_path (struct cm_dm_id_path * id_path_p,
                                               struct cm_dm_instance_map * pMap);

/* Read Existing Instance ID-Key Map Entry */
int32_t cmi_dm_get_instance_map_entry_using_name_path_and_key (char * node_path_p,
                                                       struct cm_dm_instance_map *
                                                       pMap);
int32_t cmi_dm_get_instance_map_entry_using_name_path_and_inst_id (char * node_path_p,
                                                          struct cm_dm_instance_map *
                                                          pMap);
int32_t cmi_dm_get_instance_map_entry_using_id_path_and_key (struct cm_dm_id_path * id_path_p,
                                                     struct cm_dm_instance_map *
                                                     pMap);
int32_t cmi_dm_get_instance_map_entry_using_id_path_and_inst_id (struct cm_dm_id_path *
                                                        id_path_p,
                                                        struct cm_dm_instance_map *
                                                        pMap);

/* Set Permissions, Attributes to an existing Instance ID-Key Map Entry */
int32_t cmi_dm_set_instance_map_attribs_using_name_path_and_inst_id (char * node_path_p,
                                                            struct cm_dm_instance_map
                                                            * pMap);
int32_t cmi_dm_set_instance_map_attribs_using_name_path_and_inst_key (char *
                                                             node_path_p,
                                                             struct cm_dm_instance_map
                                                             * pMap);
int32_t cmi_dm_set_instance_map_attribs_using_id_path_and_inst_id (struct cm_dm_id_path *
                                                          id_path_p,
                                                          struct cm_dm_instance_map *
                                                          pMap);
int32_t cmi_dm_set_instance_map_attribs_using_id_path_and_inst_key (struct cm_dm_id_path *
                                                           id_path_p,
                                                           struct cm_dm_instance_map *
                                                           pMap);

int32_t cm_dm_get_child_id_by_name (char * path_p, char * pNodeName);
int32_t cmi_dm_get_child_id_by_name_from_dm_path (char * path_p, char * pNodeName);
struct cm_dm_data_element *cmi_dm_get_child_node_by_name_from_dm_path (char * path_p, char * pNodeName);
struct cm_dm_data_element *cmi_dm_get_child_node_by_id_from_dm_path (char * path_p,  uint32_t id_ui);

struct cm_dm_data_element *cmi_dm_get_child_info_by_name (char * path_p,
                                               char * pNodeName);
struct cm_dm_data_element *cmi_dm_get_child_info_by_id (char * path_p, uint32_t id_ui);

char *cmi_dm_get_child_name_by_id (char * path_p, uint32_t id_ui);

unsigned char cmi_dm_is_child_element (char * child_path_p, char * parent_path_p);

unsigned char cmi_dm_is_instance_map_entry_using_key_and_name_path (char * node_path_p,
                                                      void * key_p,
                                                      uint32_t key_type);


int32_t cmi_dm_get_first_inode (char * head_node_path_p,
                             uint32_t traversal_type_ui,
                             void ** opaque_info_pp, uint32_t * opaque_len_p);

int32_t cmi_dm_get_first_inode_using_instance_path (char * head_node_path_p,
                                          uint32_t traversal_type_ui,
                                          UCMDM_Inode_t ** ppDMInode,
                                          void ** opaque_info_pp);

int32_t cmi_dm_create_keys_array_from_inode (UCMDM_Inode_t *pHeadNode,
                                   			UCMDM_Inode_t * pINode,
                                        struct cm_array_of_structs * keys_array_p);

int32_t cmi_dm_create_keys_array_from_instance_node (UCMDM_Instance_t * pHeadNode,
                                               UCMDM_Inode_t *pINode,
                                               struct cm_array_of_structs * keys_array_p);
int32_t cmi_dm_create_nv_pair_from_imap_entry_and_dm_node (struct cm_dm_instance_map * pMap,
                                                  struct cm_dm_data_element * pDMNode,
                                                  struct cm_nv_pair * pNvPair);

int32_t cmi_dm_delete_instance_using_key (UCMDM_Inode_t * pInode,
                                      void * key_p, uint32_t key_type);

int32_t cmi_dm_delete_instance (UCMDM_Instance_t * pInstanceNode);

struct cm_dm_data_element *cmi_dm_get_key_child_element (struct cm_dm_data_element *
                                               parent_node_p);

int32_t cmi_dm_get_key_child_info_UsingNode (struct cm_dm_data_element * pNode,
                                         struct cm_dm_node_info ** key_child_info_p);

int32_t cmi_dm_get_keys_iv_array_from_name_path (char * name_path_p,
                                          struct cm_array_of_iv_pairs ** keys_array_pp);

int32_t cmi_dm_get_inode_or_instance_node_by_name_path (char * name_path_p,
                                                int32_t * pnode_type_ui,
                                                void ** pOutNode);

UCMDM_Inode_t *cmi_dm_get_root_inode (void);

int32_t cmi_dm_delete_inode (UCMDM_Inode_t * pInode);

void cmi_dm_free_instance_opaque_info(void *opaque_info_p);


/*****************************************************************************
 * * * * * * * * R O L E S  N  P E R M I S S I O N S * * * * * * * * * * * *
 *****************************************************************************/

/* Role-Permission related API */
int32_t cmi_dm_set_role_permissions (char * node_path_p,
																	char *role_name_p,
                                  struct cm_dm_role_permission * role_perm_p);
int32_t cmi_dm_get_permissions_by_role (char * node_path_p,
																	char *role_name_p,
                                    struct cm_dm_role_permission * role_perm_p);

int32_t cmi_dm_get_role_permissions (char * node_path_p,
																	char *role_name_p,
                                    struct cm_dm_array_of_role_permissions  **role_perm_ary_pp);

int32_t cmi_dm_del_permissions_by_role (char * node_path_p,
																		char *role_name_p,
                                    struct cm_dm_role_permission * role_perm_p);

int32_t cmi_dm_verify_role_set_privileges (struct cm_dm_data_element *pDMNode,
			char *admin_role_p);

int32_t cmi_dm_verify_role_get_privileges (struct cm_dm_data_element *pDMNode,
			char *admin_role_p);

/* * * * * * I N S T A N C E S * * * * * * * */


int32_t cmi_dm_set_instance_role_permissions (char * node_path_p,
			char *role_name_p,
			struct cm_dm_role_permission * role_perm_p);

int32_t cmi_dm_get_instance_role_permissions_by_role(char * node_path_p,char *role_name_p,
			struct cm_dm_role_permission * role_perm_p);

int32_t cmi_dm_get_instance_role_permissions(char * node_path_p,char *role_name_p,
			struct cm_dm_array_of_role_permissions **role_perm_ary_pp);

int32_t cmi_dm_del_instance_permissions_by_role (char * node_path_p,
			char *role_name_p,
			struct cm_dm_role_permission * role_perm_p);

#if UCMDM_INSTANCE_ROLE_AND_PERMISSIONS_USING_KEY

int32_t cmi_dm_set_instance_role_permissions (char * node_path_p,
			char *role_name_p,
			void *key_p, uint32_t  key_type,
			struct cm_dm_role_permission * role_perm_p);

int32_t cmi_dm_get_instance_role_permissions_by_role(char * node_path_p,char *role_name_p,
			void *key_p, uint32_t  key_type,
			struct cm_dm_role_permission * role_perm_p);

int32_t cmi_dm_del_instance_permissions_by_role (char * node_path_p,
			char *role_name_p,	char *key_p, uint32_t key_type,
			struct cm_dm_role_permission * role_perm_p);
#endif
int32_t cmi_dm_verify_instance_role_set_privileges (UCMDM_Instance_t *pDMInstanceNode,
			char *admin_role_p);

int32_t cmi_dm_verify_instance_role_get_privileges (UCMDM_Instance_t *pDMInstanceNode,
			char *admin_role_p);

int32_t cmi_dm_attach_instance_role_perm_entry (UCMDM_Instance_t * pInstanceNode,
                                   ucmdm_RolePermission_t * role_perm_p);

int32_t cmi_dm_dettach_instance_role_perm_entry (UCMDM_Instance_t * pInstanceNode,
                                    ucmdm_RolePermission_t * role_perm_p);


int32_t cmi_dm_delete_perm_list_from_instance(UCMDM_Instance_t *pInstanceNode);

ucmdm_RolePermission_t *cmi_dm_get_instance_role_perms_entry_by_role(UCMDM_Instance_t
			*pInstanceNode,		char * pRole);

ucmdm_RolePermission_t* cmi_dm_get_instance_tree_role_perm_by_role(UCMDM_Instance_t *pDMInstanceNode, char * admin_role_p);

int32_t cmi_dm_get_permissions (char * node_path_p,
                                    struct cm_dm_role_permission * role_perm_p);
int32_t cmi_dm_role_perm_init(void);

int32_t cmi_dm_get_file_name (char * name_path_p,
			char	 **file_name_p);

int32_t cmi_dm_get_directory_name (char * name_path_p,
			char	 **pDirName);



int32_t cmi_add_dm_path_appl_channid(char *dm_path_p,uint32_t appl_id_ui,uint32_t channel_id_ui);
int32_t cm_get_channel_id_and_appl_id(char *dm_path_p,uint32_t *appl_id_ui,uint32_t *channel_id_ui);


int32_t cmi_dm_get_first_element_info (char * head_node_path_p,
                                uint32_t traversal_type_ui,
                                struct cm_dm_data_element ** data_element_p,
                                void ** opaque_info_pp, uint32_t * opaque_len_p);

int32_t cmi_dm_get_first_last_child_element_info (char * head_node_path_p,
                                uint32_t traversal_type_ui,
                                struct cm_dm_data_element ** data_element_p,
                                void ** opaque_info_pp, uint32_t * opaque_len_p);
int32_t cmi_dm_get_next_element_info (char * head_node_path_p,
                               uint32_t traversal_type_ui,
                               struct cm_dm_data_element ** data_element_p,
                               void ** opaque_info_pp, uint32_t * opaque_len_p);

int32_t cmi_dm_get_next_last_child_element_info (char * head_node_path_p,
                               uint32_t traversal_type_ui,
                               struct cm_dm_data_element ** data_element_p,
                               void ** opaque_info_pp, uint32_t * opaque_len_p);
char *cmi_dm_prepare_dm_instance_path(char *path_p, struct cm_array_of_iv_pairs *keys_array_p);

int32_t cmi_dm_create_key_child_info_array (struct cm_dm_data_element * parent_node_p,
                                    struct cm_array_of_structs ** ppChildInfoArr);

void cmi_dm_free_node_infoArray (struct cm_array_of_structs *	array_of_node_info_p);

int32_t cmi_dm_get_key_array_from_namepathToken (char * pToken,
			struct cm_dm_data_element *pNode,
			struct cm_array_of_nv_pair *pnv_pair_array,	 uint32_t *puiNvIndex);
UCMDM_Instance_t *cmi_dm_get_instance_using_keyArray (UCMDM_Inode_t * pInode,
                                             struct cm_array_of_nv_pair *key_nv_pairs_p);
int32_t cmi_dm_delete_instance_using_keyArray (UCMDM_Inode_t * pInode,
                                      struct cm_array_of_nv_pair *key_nv_pairs_p);
int32_t cmi_dm_get_instance_keys_from_name_path (char * name_path_p,
                                        struct cm_array_of_nv_pair ** keys_array_pp);
unsigned char cmi_dm_is_instance_map_entry_using_key_array_and_name_path (char * node_path_p,
                                              struct cm_array_of_nv_pair *pnv_pair_array);

UCMDM_Instance_t *cmi_dm_get_first_instance_using_name_path(char * node_path_p);
UCMDM_Instance_t *cmi_dm_get_next_instance_using_name_path(char * node_path_p, struct cm_array_of_nv_pair *nv_pair_arr_p);
UCMDM_Instance_t *cmi_dm_get_first_instance (UCMDM_Inode_t * pInode);
UCMDM_Instance_t *cmi_dm_get_next_instance (UCMDM_Inode_t * pInode,
                                             struct cm_array_of_nv_pair *key_nv_pairs_p);
int32_t cmi_verify_global_trans(struct cm_dm_data_element *pDMNode);
int32_t cmi_dm_get_next_table_n_anchor_element_info (char * head_node_path_p,
                               uint32_t traversal_type_ui,
                               struct cm_dm_data_element **data_element_p,
                               void **opaque_info_pp, uint32_t *opaque_len_p);

int32_t cmi_dm_get_next_last_child_table_n_anchor_element_info (char * head_node_path_p,
                               uint32_t traversal_type_ui,
                               struct cm_dm_data_element **data_element_p,
                               void **opaque_info_pp, uint32_t *opaque_len_p);

void cmi_dm_trace (int32_t iModuleId, unsigned char ucLevel, char * pFunc,
      int32_t ulLine, char * pFormatString, ...);
#endif /* DM_LIF_H */
