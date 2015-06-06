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
 * File name: jeutil.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/05/2013
 * Description: 
*/

#ifndef UCMJE_UTIL_H
#define UCMJE_UTIL_H

#define   CM_JE_IS_DIGIT(Char)  ((Char>='0') && (Char<='9'))
int32_t je_copy_cfgsession (struct je_config_session * pJECfgSession,
                                       struct cm_je_config_session * pUCMCfgSession);

int32_t je_copy_cfgsession_to_buffer (struct cm_je_config_session * pUCMCfgSession,
                                        char * pUCMRespBuf,
                                        uint32_t * puiCfgSessionLen);

int32_t cm_je_copy_nvpair_from_buffer (char * pRequestData,
                                   struct cm_nv_pair * pNvPair, uint32_t * puiLen);
int32_t cm_je_copy_nvpair_to_buffer (struct cm_nv_pair * pNvPair, char * pResponseBuf,
                                 uint32_t * puiLen);
inline int32_t je_copy_nvpair_to_nvpair (struct cm_nv_pair * pInNvPair,  struct cm_nv_pair * pOutNvPair);

inline int32_t je_copy_ivpair_to_ivpair (struct cm_iv_pair * pInIvPair, struct cm_iv_pair * pOutIvPair);

int32_t je_copy_ivpair_to_nvpair (char * dm_path_p,
                                 struct cm_iv_pair * pInIvPair,
                                 struct cm_nv_pair * pOutNvPair);
int32_t je_copy_ivpairarr_to_nvpairarr (char * dm_path_p,
                                       struct cm_array_of_iv_pairs * pInIvPairArr,
                                       struct cm_array_of_nv_pair * pOutNvPairArr);

int32_t je_copy_arr_ivpairarr_to_arr_nvpairarr (char * dm_path_p,
                                             struct cm_array_of_iv_pairs *
                                             pInArrIvPairArr, uint32_t uiArrCnt,
                                             struct cm_array_of_nv_pair *
                                             pOutArrNvPairArr);

int32_t je_copy_nvpair_to_ivpair (char * dm_path_p,
                                 struct cm_nv_pair * pInNvPair,
                                 struct cm_iv_pair * pOutIvPair);

int32_t je_copy_nvpairarr_to_ivpairarr (char * dm_path_p,
                                       struct cm_array_of_nv_pair * pInNvPairArr,
                                       struct cm_array_of_iv_pairs * pOutIvPairArr,
                                       uint32_t mgmt_engine_id);
int32_t je_split_ivpair_arr (char * dm_path_p,
                             struct cm_array_of_iv_pairs * pIvPairArr,
                             struct cm_array_of_iv_pairs * pMandIvPairArr,
                             struct cm_array_of_iv_pairs * pOptIvPairArr,
                             struct cm_array_of_iv_pairs * key_iv_pair_arr_p,
                             struct cm_result * result_p,
                             uint32_t mgmt_engine_id);

int32_t je_combine_nvpairs (struct cm_nv_pair * pNvPair1,
                             struct cm_array_of_nv_pair * pInArrNvPairArr1,
                             struct cm_array_of_nv_pair * pInArrNvPairArr2,
                             struct cm_array_of_nv_pair ** ppOutArrNvPairArr);

int32_t ucmJECopyApresult_pToUCMResult (char * dm_path_p,
                                       struct cm_app_result * app_result_p,
                                       struct cm_result * result_p);

int32_t je_make_ivpairs (char * dm_path_p,
														 struct cm_nv_pair * pNvPair1,
                             struct cm_array_of_nv_pair * pInArrNvPairArr1,
                             struct cm_array_of_nv_pair * pInArrNvPairArr2,
                             struct cm_array_of_iv_pairs * pMandIvPairArr,
                             struct cm_array_of_iv_pairs * pOptIvPairArr,
                             struct cm_array_of_iv_pairs * key_iv_pair_arr_p,
                             struct cm_result * result_p,
                             uint32_t mgmt_engine_id);

int32_t je_get_key_nvpair (char * dm_path_p,
                             struct cm_array_of_nv_pair * nv_pair_arr_p,
                             struct cm_nv_pair * key_nv_pair_p);

int32_t cm_je_copy_ivpair_arr_to_buffer(struct cm_array_of_iv_pairs *pNvPairAry,
														char *pUCMTempRespBuf, uint32_t *puiLen);

int32_t cm_je_ivpair_to_buffer (struct cm_iv_pair * pInNvPair,
                                 char * pResponseBuf, uint32_t * puiLen);

int32_t je_get_last_token (char * path_p, char **pLastToken);

unsigned char  ucmJEIsDigits(char *string_p,uint32_t uiLen);
/*****************************************************************************
 * * * * *  * * * * * * I N L I N E   F U N C T I O N S * * * * * * * * * * * * 
 *****************************************************************************/
inline void  je_print_statistics(void);

inline void je_free_command(struct cm_command *command_p);

inline void je_free_command_list(struct je_command_list *pCmdList);

inline void je_free_array_commands(struct cm_array_of_commands *array_of_commands_p,
			uint32_t  count_ui);

inline void je_free_nvpair(struct cm_nv_pair *pNvPair);

inline void cm_je_free_ptr_nvpair(struct cm_nv_pair *pNvPair);

inline void cm_je_free_ptr_nvpair_member_of_array(struct cm_nv_pair *pNvPair);

inline void je_free_nvpair_array(struct cm_array_of_nv_pair *pnv_pair_array);

inline void cm_je_free_ptr_nvpairArray(struct cm_array_of_nv_pair *pnv_pair_array);

inline void cm_je_free_ivpair(struct cm_iv_pair *pIvPair);

inline void cm_je_free_ptr_ivpair(struct cm_iv_pair *pIvPair);

inline void cm_je_free_ptr_ivpar_member_of_array(struct cm_iv_pair *pIvPair);

inline void cm_je_free_ivpair_array(struct cm_array_of_iv_pairs *IvPairArray);

inline void cm_je_free_ptr_ivpairArray(struct cm_array_of_iv_pairs *pIvPairArray);

int32_t cm_je_copy_ivpair_arr_from_buffer (char *pRequestData,struct cm_array_of_iv_pairs *nv_pair_array, uint32_t * puiLen);

int32_t cm_je_copy_ivpair_from_buffer (char * pRequestData,
                                   struct cm_iv_pair * pNvPair, uint32_t * puiLen);




int32_t je_create_and_push_stack_node(
			struct cm_dm_data_element *pDMNodeInfo,	void *opaque_info_p,
			uint32_t		opaque_info_length,struct cm_array_of_iv_pairs *dm_path_keys_arr_p,	struct cm_array_of_iv_pairs *pIvPairArr);

int32_t je_update_stack_node( struct je_stack_node *pStackNode,
			struct cm_array_of_iv_pairs *dm_path_keys_iv_pair_arr_p,
			struct cm_array_of_iv_pairs *pIvPairArr);

void je_delete_table_stack (UCMDllQ_t* pJEDllQ);

void je_cleanup_stack_node (struct je_stack_node *	pJEStackNode);

struct cm_array_of_iv_pairs *je_collect_keys_from_stack(UCMDllQ_t *pJEDllQ,  struct cm_array_of_iv_pairs *dm_path_array_of_key_p);

struct cm_array_of_iv_pairs *je_find_key_ivpair_array(struct cm_dm_data_element *pElement, struct cm_array_of_iv_pairs *pIvArr);


struct cm_array_of_iv_pairs *je_combine_nvpair_arr(struct cm_array_of_iv_pairs *arr1_p, 		struct cm_array_of_iv_pairs *arr2_p);

int32_t je_get_key_nvpair_arr(char * dm_path_p,
                             struct cm_array_of_nv_pair * nv_pair_arr_p,
                             struct cm_array_of_nv_pair * key_child_nv_array_p);
struct cm_array_of_nv_pair *ucmJEFindkey_nv_pairs(struct cm_dm_data_element *pElement, struct cm_array_of_nv_pair *pNvArr);

int32_t je_fill_command_info(int32_t iCmd, char * dm_path_p,
                              int32_t count_i, struct cm_nv_pair * pNvPair,
                              struct cm_command * pCmd);
#endif
