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
 * File name: datapathcbk.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: 
 * 
 */

#ifdef OF_CM_SUPPORT


#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"



int32_t datapath_appl_ucmcbk_init (void);

int32_t datapath_addrec (void * config_trans_p,
			struct cm_array_of_iv_pairs * pMandParams,
			struct cm_array_of_iv_pairs * pOptParams,
			struct cm_app_result ** result_p);


int32_t datapath_modrec (void * config_trans_p,
			struct cm_array_of_iv_pairs * pMandParams,
			struct cm_array_of_iv_pairs * pOptParams,
			struct cm_app_result ** result_p);

int32_t datapath_setrec (void * config_trans_p,
			struct cm_array_of_iv_pairs * pMandParams,
			struct cm_array_of_iv_pairs * pOptParams,
			struct cm_app_result ** result_p);

int32_t datapath_delrec (void * config_transaction_p,
			struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_app_result ** result_p);

int32_t datapath_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
			uint32_t * count_p,
			struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);

int32_t datapath_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_array_of_iv_pairs * prev_record_key_p, uint32_t * count_p,
			struct cm_array_of_iv_pairs ** next_n_record_data_p);

int32_t datapath_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t datapath_verifycfg (struct cm_array_of_iv_pairs * keys_arr_p,
			uint32_t command_id, struct cm_app_result ** result_p);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */
 int32_t datapath_ucm_validatemandparams (struct cm_array_of_iv_pairs *
			pMandParams,
			struct cm_app_result **
			presult_p);

 int32_t datapath_ucm_validateoptparams (struct cm_array_of_iv_pairs *
			pOptParams,
			struct cm_app_result ** presult_p);


 int32_t datapath_ucm_setmandparams (struct cm_array_of_iv_pairs *
			pMandParams,
			struct dprm_distributed_forwarding_domain_info *of_domain,
			 struct dprm_switch_info * of_switch,
			struct dprm_datapath_general_info* datapath_info,
			struct cm_app_result ** presult_p);

 int32_t datapath_ucm_setoptparams (struct cm_array_of_iv_pairs *
			pOptParams,
			struct dprm_datapath_general_info * datapath_info,
			struct cm_app_result ** presult_p);


 int32_t datapath_ucm_getparams (struct dprm_datapath_general_info *datapath_info,
			struct cm_array_of_iv_pairs * result_iv_pairs_p);



struct cm_dm_app_callbacks datapath_ucm_callbacks = {
	 NULL,
	 datapath_addrec,
	 datapath_modrec,
	 datapath_delrec,
	 NULL,
	 datapath_getfirstnrecs,
	 datapath_getnextnrecs,
	 datapath_getexactrec,
	 datapath_verifycfg,
	 NULL
};
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * Switch  UCM Init * * * * * * *  * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t datapath_appl_ucmcbk_init (void)
{
	 CM_CBK_DEBUG_PRINT ("Entry");
	 cm_register_app_callbacks (CM_ON_DIRECTOR_DATAPATH_APPL_ID,&datapath_ucm_callbacks);
	 return OF_SUCCESS;
}

int32_t datapath_addrec (void * config_trans_p,
			struct cm_array_of_iv_pairs * pMandParams,
			struct cm_array_of_iv_pairs * pOptParams,
			struct cm_app_result ** result_p)
{
	 struct cm_app_result *datapath_result = NULL;
	 int32_t return_value = OF_FAILURE;
	 struct dprm_datapath_general_info datapath_info = { };
	 struct dprm_distributed_forwarding_domain_info of_domain={};
	 struct dprm_switch_info  of_switch={};
	 uint64_t domain_handle;
	 uint64_t switch_handle;
	 uint64_t datapath_handle;

	 CM_CBK_DEBUG_PRINT ("Entered");

	 of_memset (&datapath_info, 0, sizeof (struct dprm_datapath_general_info));
	 
	 if ((datapath_ucm_setmandparams (pMandParams, &of_domain, &of_switch, &datapath_info, &datapath_result)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			fill_app_result_struct (&datapath_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
			*result_p=datapath_result;
			return OF_FAILURE;
	 }

	 return_value = datapath_ucm_setoptparams (pOptParams, &datapath_info, &datapath_result);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
			fill_app_result_struct (&datapath_result, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
			*result_p = datapath_result;
			return OF_FAILURE;
	 }
	 
	 return_value=dprm_get_forwarding_domain_handle(of_domain.name, &domain_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: get domain handle failed");
			fill_app_result_struct (&datapath_result, NULL, CM_GLU_DOMAIN_NOT_FOUND);
			*result_p = datapath_result;
			return OF_FAILURE;
	 }

	 return_value=dprm_get_switch_handle(of_switch.name, &switch_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: switch does not exist with name %s",of_switch.name);
			fill_app_result_struct (&datapath_result, NULL, CM_GLU_SWITCH_DOESNOT_EXIST);
			*result_p = datapath_result;
			return OF_FAILURE;
	 }

	 return_value=dprm_register_datapath(&datapath_info, switch_handle, domain_handle, &datapath_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Domain Table Addition Failed");
			fill_app_result_struct (&datapath_result, NULL, CM_GLU_DOMAIN_ADD_FAILED);
			*result_p = datapath_result;
			return OF_FAILURE;
	 }

   CM_CBK_DEBUG_PRINT ("Datapath added successfully");
	 return OF_SUCCESS;
}

int32_t datapath_modrec (void * config_trans_p,
			struct cm_array_of_iv_pairs * pMandParams,
			struct cm_array_of_iv_pairs * pOptParams,
			struct cm_app_result ** result_p)
{
	 struct cm_app_result *datapath_result = NULL;
	 int32_t return_value = OF_FAILURE;
	 struct dprm_datapath_general_info datapath_info = { };
	 struct dprm_distributed_forwarding_domain_info of_domain={};
	 struct dprm_switch_info  of_switch={};
	 uint64_t domain_handle;
	 uint64_t switch_handle;
	 uint64_t datapath_handle;

	 CM_CBK_DEBUG_PRINT ("Entered");

	 of_memset (&datapath_info, 0, sizeof (struct dprm_datapath_general_info));
	 
	 if ((datapath_ucm_setmandparams (pMandParams, &of_domain, &of_switch, &datapath_info, &datapath_result)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			fill_app_result_struct (&datapath_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
			*result_p=datapath_result;
			return OF_FAILURE;
	 }

	
	 return_value = datapath_ucm_setoptparams (pOptParams, &datapath_info, &datapath_result);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
			fill_app_result_struct (&datapath_result, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
			*result_p = datapath_result;
			return OF_FAILURE;
	 }
	 
	
	 return_value=dprm_get_forwarding_domain_handle(of_domain.name, &domain_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: get domain handle failed");
			fill_app_result_struct (&datapath_result, NULL, CM_GLU_DOMAIN_NOT_FOUND);
			*result_p = datapath_result;
			return OF_FAILURE;
	 }

	
	 return_value=dprm_get_switch_handle(of_switch.name, &switch_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: switch does not exist with name %s",of_switch.name);
			fill_app_result_struct (&datapath_result, NULL, CM_GLU_SWITCH_DOESNOT_EXIST);
			*result_p = datapath_result;
			return OF_FAILURE;
	 }

	
	 return_value=dprm_update_datapath(&datapath_info, switch_handle, domain_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Domain Table Modification Failed");
			fill_app_result_struct (&datapath_result, NULL, CM_GLU_DOMAIN_ADD_FAILED);
			*result_p = datapath_result;
			return OF_FAILURE;
	 }

	
   CM_CBK_DEBUG_PRINT ("Datapath Modified succesfully");
	 return OF_SUCCESS;
}




int32_t datapath_delrec (void * config_transaction_p,
			struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_app_result ** result_p)
{
	 struct cm_app_result *datapath_result = NULL;
	 int32_t return_value = OF_FAILURE;
	 struct dprm_datapath_general_info datapath_info = { };
	 struct dprm_distributed_forwarding_domain_info of_domain={};
	 struct dprm_switch_info  of_switch={};
	 uint64_t domain_handle;
	 uint64_t datapath_handle;
	 uint64_t switch_handle;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 of_memset (&datapath_info, 0, sizeof (struct dprm_datapath_general_info));

	 if ((datapath_ucm_setmandparams (keys_arr_p,&of_domain,&of_switch, &datapath_info, &datapath_result)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			fill_app_result_struct (&datapath_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
			*result_p = datapath_result;
			return OF_FAILURE;
	 }
#if 0
	 return_value=dprm_get_forwarding_domain_handle(of_domain.name, &domain_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: get forwarding handle failed");
			fill_app_result_struct (&datapath_result, NULL, CM_GLU_VLAN_FAILED);
			*result_p = datapath_result;
			return OF_FAILURE;
	 }

	 return_value=dprm_get_switch_handle(of_switch.name, &switch_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: switch doesn't exists with name %s",of_switch.name);
			fill_app_result_struct (&datapath_result, NULL, CM_GLU_VLAN_FAILED);
			*result_p = datapath_result;
			return OF_FAILURE;
	 }
#endif
	 return_value=dprm_get_datapath_handle(datapath_info.dpid, &datapath_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Get DPRM Handle failed");
			fill_app_result_struct (&datapath_result, NULL, CM_GLU_NO_MATCHING_RECORD);
			*result_p = datapath_result;
			return OF_FAILURE;
	 }

	 return_value=dprm_unregister_datapath(datapath_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Delete Table record failed");
			fill_app_result_struct (&datapath_result, NULL, CM_GLU_DATAPATH_DELETE_FAILED);
			*result_p = datapath_result;
			return OF_FAILURE;
	 }
   CM_CBK_DEBUG_PRINT ("Datapath deleted succesfully");
	 return OF_SUCCESS;
}

int32_t datapath_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
			uint32_t * count_p,
			struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{
	 struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	 int32_t return_value = OF_FAILURE;
	 uint32_t uiRecCount = 0;
	 struct cm_app_result *datapath_result = NULL;
	 struct dprm_datapath_general_info datapath_info = { };
	 struct  dprm_distributed_forwarding_domain_info of_domain={};
	 struct dprm_switch_info  of_switch={};
	 struct dprm_datapath_runtime_info runtime_info= {};
	 uint64_t switch_handle;
	 uint64_t domain_handle;
	 uint64_t datapath_handle;

	 of_memset (&datapath_info, 0, sizeof (struct dprm_datapath_general_info));
	 if ((datapath_ucm_setmandparams (keys_arr_p,&of_domain, &of_switch, &datapath_info, &datapath_result)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			return OF_FAILURE;
	 }
#if 0
	 return_value=dprm_get_forwarding_domain_handle(of_domain.name, &domain_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: get forwarding handle failed");
			return OF_FAILURE;
	 }

	 return_value=dprm_get_switch_handle(of_switch.name, &switch_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: switch doesn't exists with name %s",of_switch.name);
			return OF_FAILURE;
	 }
#endif
	 return_value=dprm_get_first_datapath(&datapath_info, &runtime_info, &datapath_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Get first record failed for DPRM Table");
			return OF_FAILURE;
	 }
	 
	 result_iv_pairs_p =
			(struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
	 if (result_iv_pairs_p == NULL)
	 {
			CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
			return OF_FAILURE;
	 }
	 datapath_ucm_getparams (&datapath_info, &result_iv_pairs_p[uiRecCount]);
	 uiRecCount++;
	 *array_of_iv_pair_arr_p = result_iv_pairs_p;
	 *count_p = uiRecCount;
	 return OF_SUCCESS;
}


int32_t datapath_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_array_of_iv_pairs *prev_record_key_p, uint32_t * count_p,
			struct cm_array_of_iv_pairs ** next_n_record_data_p)
{

	 struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	 struct cm_app_result *datapath_result = NULL;
	 int32_t return_value = OF_FAILURE;
	 uint32_t uiRecCount = 0;
	 struct dprm_switch_info  of_switch={};
	 struct dprm_datapath_general_info datapath_info={};
	 struct dprm_datapath_runtime_info runtime_info={};
	 struct dprm_distributed_forwarding_domain_info of_domain={};
	 uint64_t domain_handle;
	 uint64_t datapath_handle;
	 uint64_t switch_handle;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 of_memset (&datapath_info, 0, sizeof (struct dprm_datapath_general_info));
	 result_iv_pairs_p =
			(struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
	 if (result_iv_pairs_p == NULL)
	 {
			CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
			return OF_FAILURE;
	 }

	 if ((datapath_ucm_setmandparams (keys_arr_p,&of_domain, &of_switch, &datapath_info, &datapath_result)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			return OF_FAILURE;
	 }

	 if ((datapath_ucm_setmandparams (prev_record_key_p,&of_domain, &of_switch, &datapath_info, &datapath_result)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			return OF_FAILURE;
	 }
#if 0
	 return_value=dprm_get_forwarding_domain_handle(of_domain.name, &domain_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: get forwarding handle failed");
			return OF_FAILURE;
	 }

	 return_value=dprm_get_switch_handle(of_switch.name, &switch_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: switch doesn't exists with name %s",of_switch.name);
			return OF_FAILURE;
	 }
#endif
	 return_value=dprm_get_datapath_handle( datapath_info.dpid,&datapath_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: get dprm table handle failed");
			return OF_FAILURE;
	 }
#if 0
	 return_value=dprm_get_exact_datapath(datapath_handle,&switch_handle,&domain_handle,&datapath_info,&runtime_info);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: get dprm table handle failed");
			return OF_FAILURE;
	 }
#endif
	 return_value=dprm_get_next_datapath( &datapath_info, &runtime_info, &datapath_handle);
	 if (return_value == OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Next domain record is: %ld ",
						datapath_info.dpid);
			datapath_ucm_getparams (&datapath_info, &result_iv_pairs_p[uiRecCount]);
			uiRecCount++;
	 }

	 CM_CBK_DEBUG_PRINT ("Number of records requested were %ld ", *count_p);
	 CM_CBK_DEBUG_PRINT ("Number of records found are %ld", uiRecCount+1);
	 *next_n_record_data_p = result_iv_pairs_p;
	 *count_p = uiRecCount;
	 return OF_SUCCESS;
}

int32_t datapath_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_array_of_iv_pairs ** pIvPairArr)
{
	 struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	 struct cm_app_result *datapath_result = NULL;
	 uint32_t ii = 0;
	 int32_t return_value = OF_FAILURE;
	 struct dprm_datapath_general_info datapath_info={};
	 struct dprm_switch_info of_switch={};
	 struct dprm_distributed_forwarding_domain_info of_domain={};
	 struct dprm_datapath_runtime_info runtime_info={};
	 uint64_t domain_handle;
	 uint64_t datapath_handle;
	 uint64_t switch_handle;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 result_iv_pairs_p =
			(struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
	 if (result_iv_pairs_p == NULL)
	 {
			CM_CBK_DEBUG_PRINT ("Failed to allocate memory for result_iv_pairs_p");
			return OF_FAILURE;
	 }
	 
	 if ((datapath_ucm_setmandparams (keys_arr_p,&of_domain, &of_switch, &datapath_info, &datapath_result)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			return OF_FAILURE;
	 }
#if 0
	 return_value=dprm_get_forwarding_domain_handle(of_domain.name, &domain_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: name framing failed");
			return OF_FAILURE;
	 }

	 return_value=dprm_get_switch_handle(of_switch.name, &switch_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: switch doesn't exists with name %s",of_switch.name);
			return OF_FAILURE;
	 }
#endif
	 return_value=dprm_get_datapath_handle(datapath_info.dpid, &datapath_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: get dprm table handle failed");
			return OF_FAILURE;
	 }
	 
	 of_memset (&datapath_info, 0, sizeof (struct dprm_datapath_general_info));
	 return_value=dprm_get_exact_datapath(datapath_handle,&switch_handle,&domain_handle,&datapath_info,&runtime_info);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: get dprm table handle failed");
			return OF_FAILURE;
	 }

	 CM_CBK_DEBUG_PRINT ("Exact matching record found");
	 datapath_ucm_getparams (&datapath_info, result_iv_pairs_p);
	 *pIvPairArr = result_iv_pairs_p;
	 return return_value;
}

int32_t datapath_verifycfg (struct cm_array_of_iv_pairs * keys_arr_p,
			uint32_t command_id, struct cm_app_result ** result_p)
{
	 struct cm_app_result *datapath_result = NULL;
	 int32_t return_value = OF_FAILURE;
	 struct dprm_datapath_general_info datapath_info = { };

	 CM_CBK_DEBUG_PRINT ("Entered");
	 of_memset (&datapath_info, 0, sizeof (struct dprm_datapath_general_info));

	 return_value = datapath_ucm_validatemandparams (keys_arr_p, &datapath_result);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
			return OF_FAILURE;
	 }

	 if (datapath_ucm_validateoptparams (keys_arr_p, &datapath_result)
				 != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Validate Optional Parameters Failed");
			return OF_FAILURE;
	 }

	 *result_p = datapath_result;
	 return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */

 int32_t datapath_ucm_validatemandparams (struct cm_array_of_iv_pairs *
			pMandParams,	struct cm_app_result **presult_p)
{
	 uint32_t uiMandParamCnt;
	 struct cm_app_result *datapath_result = NULL;
	 uint32_t switch_type, uiTableCnt;
         uint64_t uldpId;


	 CM_CBK_DEBUG_PRINT ("Entered");
	 for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;
				 uiMandParamCnt++)
	 {
			switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
			{
				 case CM_DM_DATAPATH_DOMAIN_ID:
						if (pMandParams->iv_pairs[uiMandParamCnt].value_p == NULL)
						{
							 CM_CBK_DEBUG_PRINT ("Domain Name is NULL");
							 fill_app_result_struct (&datapath_result, NULL, CM_GLU_DOMAIN_NAME_NULL);
							 *presult_p = datapath_result;
							 return OF_FAILURE;
						}
						break;

				 case CM_DM_DATAPATH_DATAPATHID_ID: 
						if (pMandParams->iv_pairs[uiMandParamCnt].value_p == NULL)
						{
							 CM_CBK_DEBUG_PRINT ("Datapath ID is NULL");
							 fill_app_result_struct (&datapath_result, NULL, CM_GLU_DATAPATH_ID_NULL);
							 *presult_p = datapath_result;
							 return OF_FAILURE;
						}
						if (of_strlen(pMandParams->iv_pairs[uiMandParamCnt].value_p) < 0 ||  
                                                    of_strlen(pMandParams->iv_pairs[uiMandParamCnt].value_p) > 16)
						{
							 CM_CBK_DEBUG_PRINT ("Invalid Datapath ID");
							 fill_app_result_struct (&datapath_result, NULL, CM_GLU_DATAPATH_ID_INVALID);
							 *presult_p = datapath_result;
							 return OF_FAILURE;
						}
						//uldpId =
						//	 of_atol ((char *) (pMandParams->iv_pairs[uiMandParamCnt].value_p));
#if 0
						if (uiTableId < 0 ||  uiTableId > 24)
						{
							 CM_CBK_DEBUG_PRINT ("Number of tables Exceeding its maximum Number");
							 fill_app_result_struct (&datapath_result, NULL, CM_GLU_VLAN_FAILED);
							 *presult_p = datapath_result;
							 return OF_FAILURE;
						}
#endif

						break;
#if 0
				 case CM_DM_DOMAINTABLE_NUMOFMATCHFIELDS_ID:
						if (pMandParams->iv_pairs[uiMandParamCnt].value_p == NULL)
						{
							 CM_CBK_DEBUG_PRINT ("Number of tables is NULL");
							 fill_app_result_struct (&datapath_result, NULL, CM_GLU_VLAN_FAILED);
							 *presult_p = datapath_result;
							 return OF_FAILURE;
						}
						uiTableCnt =
							 of_atoi ((char *) (pMandParams->iv_pairs[uiMandParamCnt].value_p));
						if (uiTableCnt < 0 ||  uiTableCnt > 24)
						{
							 CM_CBK_DEBUG_PRINT ("Number of Matching fields");
							 fill_app_result_struct (&datapath_result, NULL, CM_GLU_VLAN_FAILED);
							 *presult_p = datapath_result;
							 return OF_FAILURE;
						}
						break;
#endif
			}
	 }
	 CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
	 return OF_SUCCESS;
}

 int32_t datapath_ucm_validateoptparams (struct cm_array_of_iv_pairs *
			pOptParams,
			struct cm_app_result ** presult_p)
{
	 uint32_t uiOptParamCnt, uiTableCnt;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 for (uiOptParamCnt = 0; uiOptParamCnt < pOptParams->count_ui; uiOptParamCnt++)
	 {
			switch (pOptParams->iv_pairs[uiOptParamCnt].id_ui)
			{
				 default:
						break;
			}
	 }
	 CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
	 return OF_SUCCESS;
}
 int32_t datapath_ucm_setmandparams (struct cm_array_of_iv_pairs *
			pMandParams,
			struct dprm_distributed_forwarding_domain_info *of_domain,
			struct dprm_switch_info * of_switch,
			struct dprm_datapath_general_info* datapath_info,		struct cm_app_result ** presult_p)
{
	 uint32_t uiMandParamCnt;
	 uint32_t switch_type;
	 int32_t iTableCnt;
     int64_t idpId;
	char *pDPID;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;
				 uiMandParamCnt++)
	 {
			switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
			{
				 case CM_DM_DATAPATH_DOMAIN_ID:
						of_strncpy (of_domain->name,
									(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
									pMandParams->iv_pairs[uiMandParamCnt].value_length);
						break;


				case CM_DM_DATAPATH_DATAPATHNAME_ID:
						of_strncpy (datapath_info->datapath_name,
									(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
									pMandParams->iv_pairs[uiMandParamCnt].value_length);
						break;



				 case CM_DM_DATAPATH_ONSWITCH_ID:
						of_strncpy (of_switch->name,
									(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
									pMandParams->iv_pairs[uiMandParamCnt].value_length);
						break;

				 case CM_DM_DATAPATH_SUBJECTNAME_ID:
						of_strncpy (datapath_info->expected_subject_name_for_authentication,
									(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
									pMandParams->iv_pairs[uiMandParamCnt].value_length);
						break;

				 case CM_DM_DATAPATH_DATAPATHID_ID:
						pDPID = (char *) of_calloc(1,  pMandParams->iv_pairs[uiMandParamCnt].value_length+1);
						if(pDPID)
                 	    {
						  of_strncpy(pDPID, (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
							 pMandParams->iv_pairs[uiMandParamCnt].value_length);
						  datapath_info->dpid = charTo64bitNum(pDPID);
						  of_free(pDPID);
						}
						break;
			}
	 }
	 CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
	 return OF_SUCCESS;
}

 int32_t datapath_ucm_setoptparams (struct cm_array_of_iv_pairs *
			pOptParams,
			struct dprm_datapath_general_info * datapath_info,
			struct cm_app_result ** presult_p)
{
	 uint32_t uiOptParamCnt;

	 CM_CBK_DEBUG_PRINT ("Entered");

	 for (uiOptParamCnt = 0; uiOptParamCnt < pOptParams->count_ui; uiOptParamCnt++)
	 {
		 switch (pOptParams->iv_pairs[uiOptParamCnt].id_ui)
		 {
			 case CM_DM_DATAPATH_MULTIPLENAMESPACES_ID:
				 datapath_info->multi_namespaces = ucm_uint8_from_str_ptr(pOptParams->iv_pairs[uiOptParamCnt].value_p);	
				 datapath_info->is_multi_ns_config = 1;
	 			CM_CBK_DEBUG_PRINT ("datapath_info->multi_namespaces:%d",datapath_info->multi_namespaces);
				 break;
			 default:
					 break;
		 }
	 }

	 CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
	 return OF_SUCCESS;
}



 int32_t datapath_ucm_getparams (struct dprm_datapath_general_info *datapath_info,
			struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
	 uint32_t uindex_i = 0;
	 char buf[64] = "";
#define CM_DATAPATH_CHILD_COUNT 6

	 result_iv_pairs_p->iv_pairs =
			(struct cm_iv_pair *) of_calloc (CM_DATAPATH_CHILD_COUNT, sizeof (struct cm_iv_pair));
	 if (result_iv_pairs_p->iv_pairs == NULL)
	 {
			CM_CBK_DEBUG_PRINT
				 ("Memory allocation failed for result_iv_pairs_p->iv_pairs");
			return OF_FAILURE;
	 }

	 /*Id authentication*/
//	 of_itoa (datapath_info->dpid, buf);
	 sprintf(buf,"%llx",datapath_info->dpid);
	 FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_DATAPATH_DATAPATHID_ID,
				 CM_DATA_TYPE_STRING, buf);
	 uindex_i++;
#if 0
	 /*Number of Domain tables*/
	 FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_DATAPATH_DOMAIN_ID,
				 CM_DATA_TYPE_STRING, buf);
	 uindex_i++;
#endif
         FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_DATAPATH_ONSWITCH_ID,
				 CM_DATA_TYPE_STRING, datapath_info->switch_name);
	 uindex_i++;

         FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_DATAPATH_DOMAIN_ID,
				 CM_DATA_TYPE_STRING, datapath_info->domain_name);
	 uindex_i++;
	 
         /* subject name*/
	 FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_DATAPATH_SUBJECTNAME_ID,
				 CM_DATA_TYPE_STRING, datapath_info->expected_subject_name_for_authentication);	
	 uindex_i++;

        /* datapath name*/
	 FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_DATAPATH_DATAPATHNAME_ID,
				 CM_DATA_TYPE_STRING, datapath_info->datapath_name);	
	 uindex_i++;




       	  //config_info_p->multi_namespaces = datapath_info_p->multi_namespaces;


	sprintf(buf,"%d",datapath_info->multi_namespaces);
	 FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_DATAPATH_MULTIPLENAMESPACES_ID,
				 CM_DATA_TYPE_STRING, buf);	

	 uindex_i++;

	 result_iv_pairs_p->count_ui = uindex_i;
	 return OF_SUCCESS;
}

#endif /* OF_CM_SUPPORT */
