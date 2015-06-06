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
 * File name: secappreg.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/17/2013
 * Description: 
*/

#include "dmincld.h"

void *IGWIpdb_BeginConfigTxn (uint32_t command_id);

int32_t IGWIpdb_AddRecord (void * config_trans_p,
                           struct cm_array_of_iv_pairs * pMandParams,
                           struct cm_array_of_iv_pairs * pOptParams,
                           struct cm_app_result ** result_p);

int32_t IGWIpdb_SetRecord (void * config_trans_p,
                           struct cm_array_of_iv_pairs * pMandParams,
                           struct cm_array_of_iv_pairs * pOptParams,
                           struct cm_app_result ** result_p);

int32_t IGWIpdb_DelRecord (void * config_transaction_p,
                           struct cm_array_of_iv_pairs * keys_arr_p,
                           struct cm_app_result ** result_p);

int32_t IGWIpdb_GetFirstNRecs (struct cm_array_of_iv_pairs * pkeysArr,
                               uint32_t * count_p,
                               struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);

int32_t IGWIpdb_GetNextNRecs (struct cm_array_of_iv_pairs * keys_arr_p,
                              struct cm_iv_pair * prev_record_key_p, uint32_t * count_p,
                              struct cm_array_of_iv_pairs ** next_n_record_data_p);

int32_t IGWIpdb_GetExactRec (struct cm_array_of_iv_pairs * keysArr,
                             struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t IGWIpdb_VerifyConfig (struct cm_array_of_iv_pairs * keys_arr_p,
                              uint32_t command_id);

int32_t IGWIpdb_EndConfigTxn (void * config_trans_p);
