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
 * File name: domaincbk.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: 
 * 
 */

#ifdef OF_CM_SUPPORT


#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"



int32_t of_domain_appl_ucmcbk_init (void);

int32_t of_domain_addrec (void * config_trans_p,
			struct cm_array_of_iv_pairs * pMandParams,
			struct cm_array_of_iv_pairs * pOptParams,
			struct cm_app_result ** result_p);

int32_t of_domain_setrec (void * config_trans_p,
			struct cm_array_of_iv_pairs * pMandParams,
			struct cm_array_of_iv_pairs * pOptParams,
			struct cm_app_result ** result_p);

int32_t of_domain_delrec (void * config_transaction_p,
			struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_app_result ** result_p);

int32_t of_domain_getfirstnrecs (struct cm_array_of_iv_pairs * pkeysArr,
			uint32_t * count_p,
			struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);

int32_t of_domain_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_array_of_iv_pairs * prev_record_key_p, uint32_t * count_p,
			struct cm_array_of_iv_pairs ** next_n_record_data_p);

int32_t of_domain_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t of_domain_verifycfg (struct cm_array_of_iv_pairs * keys_arr_p,
			uint32_t command_id, struct cm_app_result ** result_p);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */
 int32_t of_domain_ucm_validatemandparams (struct cm_array_of_iv_pairs *
			pMandParams,
			struct cm_app_result **
			presult_p);

 int32_t of_domain_ucm_validateoptparams (struct cm_array_of_iv_pairs *
			pOptParams,
			struct cm_app_result ** presult_p);


 int32_t of_domain_ucm_setmandparams (struct cm_array_of_iv_pairs *
			pMandParams,
			struct dprm_distributed_forwarding_domain_info* of_domain,
			struct cm_app_result ** presult_p);

 int32_t of_domain_ucm_setoptparams (struct cm_array_of_iv_pairs *
			pOptParams,
			struct dprm_distributed_forwarding_domain_info * of_domain,
			struct cm_app_result ** presult_p);


 int32_t of_domain_ucm_getparams (struct dprm_distributed_forwarding_domain_info *of_domain,
			struct cm_array_of_iv_pairs * result_iv_pairs_p);



struct cm_dm_app_callbacks of_domain_ucm_callbacks = {
	 NULL,
	 of_domain_addrec,
	 NULL,
	 of_domain_delrec,
	 NULL,
	 of_domain_getfirstnrecs,
	 of_domain_getnextnrecs,
	 of_domain_getexactrec,
	 of_domain_verifycfg,
	 NULL
};
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * Switch  UCM Init * * * * * * *  * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t of_domain_appl_ucmcbk_init (void)
{
	 CM_CBK_DEBUG_PRINT ("Entry");
	 
	 cm_register_app_callbacks (CM_ON_DIRECTOR_DOMAIN_APPL_ID,&of_domain_ucm_callbacks);
	 return OF_SUCCESS;
}

int32_t of_domain_addrec (void * config_trans_p,
			struct cm_array_of_iv_pairs * pMandParams,
			struct cm_array_of_iv_pairs * pOptParams,
			struct cm_app_result ** result_p)
{
	 struct cm_app_result *of_domain_result = NULL;
	 int32_t return_value = OF_FAILURE;
	 struct dprm_distributed_forwarding_domain_info of_domain = { };
	 uint64_t domain_handle;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 of_memset (&of_domain, 0, sizeof (struct dprm_distributed_forwarding_domain_info ));

	 if ((of_domain_ucm_setmandparams (pMandParams, &of_domain, &of_domain_result)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			fill_app_result_struct (&of_domain_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
			*result_p=of_domain_result;
			return OF_FAILURE;
	 }

	 return_value = of_domain_ucm_setoptparams (pOptParams, &of_domain, &of_domain_result);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
			fill_app_result_struct (&of_domain_result, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
			*result_p = of_domain_result;
			return OF_FAILURE;
	 }
	 
	 return_value = dprm_create_distributed_forwarding_domain(&of_domain,&domain_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Domain Addition Failed");
			fill_app_result_struct (&of_domain_result, NULL, CM_GLU_DOMAIN_ADD_FAILED);
			*result_p = of_domain_result;
			return OF_FAILURE;
	 }

			CM_CBK_DEBUG_PRINT ("Successfully added domain");
	 return OF_SUCCESS;
}
#if 0
int32_t of_domain_setrec (void * config_trans_p,
			struct cm_array_of_iv_pairs * pMandParams,
			struct cm_array_of_iv_pairs * pOptParams,
			struct cm_app_result ** result_p)
{
	 struct cm_app_result *of_domain_result= NULL;
	 int32_t return_value = OF_FAILURE;
	 struct dprm_distributed_forwarding_domain_info of_domain = { };
	 uint64_t domain_handle;


	 CM_CBK_DEBUG_PRINT ("Entered");
	 of_memset (&of_domain, 0, sizeof (struct dprm_distributed_forwarding_domain_info));

	 if ((of_domain_ucm_setmandparams (pMandParams, &of_domain, &of_domain_result)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			*result_p=of_domain_result;
			return OF_FAILURE;
	 }

	 return_value = of_domain_ucm_setoptparams (pOptParams, &of_domain, &of_domain_result);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
			*result_p = of_domain_result;
			return OF_FAILURE;
	 }

	 return_value=dprm_get_switch_handle(of_domain.name, &domain_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: name framing failed");
			fill_app_result_struct (&of_domain_result, NULL, CM_GLU_VLAN_FAILED);
			*result_p = of_domain_result;
			return OF_FAILURE;
	 }

	 return_value = dprm_update_switch(domain_handle,&of_domain);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("switch updation Failed");
			fill_app_result_struct (&of_domain_result, NULL, CM_GLU_VLAN_FAILED);
			*result_p = of_domain_result;
			return OF_FAILURE;
	 }

	 return OF_SUCCESS;
}
#endif

int32_t of_domain_delrec (void * config_transaction_p,
			struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_app_result ** result_p)
{
	 struct cm_app_result *of_domain_result = NULL;
	 int32_t return_value = OF_FAILURE;
	 struct dprm_distributed_forwarding_domain_info of_domain = { };
	 uint64_t domain_handle;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 of_memset (&of_domain, 0, sizeof (struct dprm_distributed_forwarding_domain_info));

	 if ((of_domain_ucm_setmandparams (keys_arr_p, &of_domain, &of_domain_result)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			fill_app_result_struct (&of_domain_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
			*result_p = of_domain_result;
			return OF_FAILURE;
	 }

	 return_value=dprm_get_forwarding_domain_handle(of_domain.name, &domain_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: get forwarding handle failed");
			fill_app_result_struct (&of_domain_result, NULL, CM_GLU_DOMAIN_NOT_FOUND);
			*result_p = of_domain_result;
			return OF_FAILURE;
	 }

	 return_value = dprm_unregister_distributed_forwarding_domain(domain_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Delete Domain record failed");
			fill_app_result_struct (&of_domain_result, NULL, CM_GLU_DOMAIN_DELETE_FAILED);
			*result_p = of_domain_result;
			return OF_FAILURE;
	 }
	 return OF_SUCCESS;
}

int32_t of_domain_getfirstnrecs (struct cm_array_of_iv_pairs * pkeysArr,
			uint32_t * count_p,
			struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{

	 struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	 struct dprm_distributed_forwarding_domain_info of_domain;
	 struct dprm_distributed_forwarding_domain_runtime_info runtime_info;
	 uint64_t domain_handle;
	 int32_t iRes = OF_FAILURE;
	 uint32_t uiRecCount = 0;


	 of_memset (&of_domain, 0, sizeof (struct dprm_distributed_forwarding_domain_info));
	 iRes=dprm_get_first_forwarding_domain(&of_domain, &runtime_info, &domain_handle);
	 if (iRes != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Get first record failed for Vlan");
			return OF_FAILURE;
	 }

  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    return OF_FAILURE;
  }
	 of_domain_ucm_getparams (&of_domain, &result_iv_pairs_p[uiRecCount]);
	 uiRecCount++;
	 *array_of_iv_pair_arr_p = result_iv_pairs_p;
	 *count_p = uiRecCount;
	 return OF_SUCCESS;
}


int32_t of_domain_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_array_of_iv_pairs *prev_record_key_p, uint32_t * count_p,
			struct cm_array_of_iv_pairs ** next_n_record_data_p)
{

	 struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	 struct cm_app_result *app_result_p = NULL;
	 struct dprm_distributed_forwarding_domain_info of_domain;
	 struct    dprm_distributed_forwarding_domain_runtime_info runtime_info;
	 uint64_t domain_handle;
	 int32_t iRes = OF_FAILURE;
	 uint32_t uiRecCount = 0;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 of_memset (&of_domain, 0, sizeof (struct dprm_distributed_forwarding_domain_info));

	 if ((of_domain_ucm_setmandparams (prev_record_key_p, &of_domain, &app_result_p)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			return OF_FAILURE;
	 }

	 iRes= dprm_get_forwarding_domain_handle(of_domain.name, &domain_handle);
	 if (iRes != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: name framing failed");
			return OF_FAILURE;
	 }

	 of_memset (&of_domain, 0, sizeof (struct dprm_distributed_forwarding_domain_info));
	 iRes=dprm_get_next_forwarding_domain(&of_domain, &runtime_info, &domain_handle);
  if (iRes != OF_SUCCESS)
	 {
    return OF_FAILURE;
  }
  
			CM_CBK_DEBUG_PRINT ("Next domain record is: %s ",
					of_domain.name);
    result_iv_pairs_p =
      (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
    if (result_iv_pairs_p == NULL)
    {
      CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
      return OF_FAILURE;
    }
			of_domain_ucm_getparams (&of_domain, &result_iv_pairs_p[uiRecCount]);
			uiRecCount++;

	 CM_CBK_DEBUG_PRINT ("Number of records requested were %ld ", *count_p);
	 CM_CBK_DEBUG_PRINT ("Number of records found are %ld", uiRecCount+1);
	 *next_n_record_data_p = result_iv_pairs_p;
	 *count_p = uiRecCount;
	 return OF_SUCCESS;
}

int32_t of_domain_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_array_of_iv_pairs ** pIvPairArr)
{

	 struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	 struct cm_app_result *app_result_p = NULL;
	 uint32_t ii = 0;
	 int32_t iRes = OF_FAILURE;
	 struct dprm_distributed_forwarding_domain_info of_domain;
	 struct dprm_distributed_forwarding_domain_runtime_info runtime_info;
	 uint64_t domain_handle;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 of_memset (&of_domain, 0, sizeof (struct dprm_distributed_forwarding_domain_info));

	 if ((of_domain_ucm_setmandparams (keys_arr_p, &of_domain, &app_result_p)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			return OF_FAILURE;
	 }

	 iRes=dprm_get_forwarding_domain_handle(of_domain.name, &domain_handle);
	 if (iRes != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: name framing failed");
			return OF_FAILURE;
	 }

	 of_memset (&of_domain, 0, sizeof (struct dprm_distributed_forwarding_domain_info));
	 iRes=dprm_get_exact_forwarding_domain(domain_handle, &of_domain, &runtime_info );
	 if (iRes != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Unable to find the matching record ");
    return OF_FAILURE;
	 }
    CM_CBK_DEBUG_PRINT ("Exact matching record found");
    result_iv_pairs_p =
      (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
    if (result_iv_pairs_p == NULL)
	 {
      CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
      return OF_FAILURE;
    }
			of_domain_ucm_getparams (&of_domain, result_iv_pairs_p);
			*pIvPairArr = result_iv_pairs_p;
  return OF_SUCCESS;
}

int32_t of_domain_verifycfg (struct cm_array_of_iv_pairs * keys_arr_p,
			uint32_t command_id, struct cm_app_result ** result_p)
{
	 struct cm_app_result *of_domain_result = NULL;
	 int32_t return_value = OF_FAILURE;
	 struct dprm_distributed_forwarding_domain_info of_domain = { };

	 CM_CBK_DEBUG_PRINT ("Entered");
	 of_memset (&of_domain, 0, sizeof (struct dprm_distributed_forwarding_domain_info));

	 return_value = of_domain_ucm_validatemandparams (keys_arr_p, &of_domain_result);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
			return OF_FAILURE;
	 }

	 if (of_domain_ucm_validateoptparams (keys_arr_p, &of_domain_result)
				 != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Validate Optional Parameters Failed");
			return OF_FAILURE;
	 }

	 *result_p = of_domain_result;
	 return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */

 int32_t of_domain_ucm_validatemandparams (struct cm_array_of_iv_pairs *
			pMandParams,
			struct cm_app_result **presult_p)
{
	 uint32_t uiMandParamCnt;
	 struct cm_app_result *of_domain_result = NULL;
  uint8_t switch_type, uiTableCnt;


	 CM_CBK_DEBUG_PRINT ("Entered");
	 for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;
				 uiMandParamCnt++)
	 {
			switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
			{
				 case CM_DM_DOMAIN_NAME_ID:
						if (pMandParams->iv_pairs[uiMandParamCnt].value_p == NULL)
						{
							 CM_CBK_DEBUG_PRINT ("Domain Name is NULL");
							 fill_app_result_struct (&of_domain_result, NULL, CM_GLU_DOMAIN_NAME_NULL);
							 *presult_p = of_domain_result;
							 return OF_FAILURE;
						}
						break;

				 case CM_DM_DOMAIN_SUBJECTNAME_ID: 
						if (pMandParams->iv_pairs[uiMandParamCnt].value_p == NULL)
						{
							 CM_CBK_DEBUG_PRINT ("Subject Name is NULL");
							 fill_app_result_struct (&of_domain_result, NULL, CM_GLU_DOMAIN_SUBJECT_NAME_NULL);
							 *presult_p = of_domain_result;
							 return OF_FAILURE;
						}
						break;
#if 0
				 case CM_DM_DOMAIN_NUMOFTABLES_ID:
						if (pMandParams->iv_pairs[uiMandParamCnt].value_p == NULL)
						{
							 CM_CBK_DEBUG_PRINT ("Number of tables is NULL");
							 fill_app_result_struct (&of_domain_result, NULL, CM_GLU_DOMAIN_NUMOFTABLES_NULL);
							 *presult_p = of_domain_result;
							 return OF_FAILURE;
						}
						uiTableCnt =
           ucm_uint8_from_str_ptr (pMandParams->iv_pairs[uiMandParamCnt].value_p);
						if (uiTableCnt < 0 ||  uiTableCnt > 24)
						{
							 CM_CBK_DEBUG_PRINT ("Number of tables Exceeding its maximum Number");
							 fill_app_result_struct (&of_domain_result, NULL, CM_GLU_DOMAIN_NUMOFTABLES_EXCEEDED);
							 *presult_p = of_domain_result;
							 return OF_FAILURE;
						}
						break;
#endif
			}
	 }
	 CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
	 return OF_SUCCESS;
}

 int32_t of_domain_ucm_validateoptparams (struct cm_array_of_iv_pairs *
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

 int32_t of_domain_ucm_setmandparams (struct cm_array_of_iv_pairs *
			pMandParams,
			struct dprm_distributed_forwarding_domain_info* of_domain,
			struct cm_app_result ** presult_p)
{
	 uint32_t uiMandParamCnt;
	 uint32_t switch_type;
  uint8_t iTableCnt;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;
				 uiMandParamCnt++)
	 {
			switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
			{
				 case CM_DM_DOMAIN_NAME_ID:
						of_strncpy (of_domain->name,
									(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
									pMandParams->iv_pairs[uiMandParamCnt].value_length);
						break;

				 case CM_DM_DOMAIN_TTP_NAME_ID:
						of_strncpy (of_domain->ttp_name,
									(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
									pMandParams->iv_pairs[uiMandParamCnt].value_length);
						break;


				 case CM_DM_DOMAIN_SUBJECTNAME_ID:
						of_strncpy (of_domain->expected_subject_name_for_authentication,
									(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
									pMandParams->iv_pairs[uiMandParamCnt].value_length);
						break;
#if 0
				 case CM_DM_DOMAIN_NUMOFTABLES_ID:
        iTableCnt = ucm_uint8_from_str_ptr (pMandParams->iv_pairs[uiMandParamCnt].value_p);
						of_domain->number_of_tables = iTableCnt;
						break;
#endif
			}
	 }
	 CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
	 return OF_SUCCESS;
}

 int32_t of_domain_ucm_setoptparams (struct cm_array_of_iv_pairs *
			pOptParams,
			struct dprm_distributed_forwarding_domain_info * of_domain,
			struct cm_app_result ** presult_p)
{
	 uint32_t uiOptParamCnt;

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



 int32_t of_domain_ucm_getparams (struct dprm_distributed_forwarding_domain_info *of_domain,
			struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
	 uint32_t uindex_i = 0;
	 char buf[64] = "";
#define CM_DOMAIN_CHILD_COUNT 3

	 result_iv_pairs_p->iv_pairs =
			(struct cm_iv_pair *) of_calloc (CM_DOMAIN_CHILD_COUNT, sizeof (struct cm_iv_pair));
	 if (result_iv_pairs_p->iv_pairs == NULL)
	 {
			CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p->iv_pairs");
			return OF_FAILURE;
	 }

	 /* Domain Name - key param*/
	 FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_DOMAIN_NAME_ID,
				 CM_DATA_TYPE_STRING, of_domain->name);
	 uindex_i++;

	 /* TTP Name - key param*/
	 FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_DOMAIN_TTP_NAME_ID,
				 CM_DATA_TYPE_STRING, of_domain->ttp_name);
	 uindex_i++;

	

	 /*Domains Subject name for authentication*/
	 FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_DOMAIN_SUBJECTNAME_ID,
				 CM_DATA_TYPE_STRING, of_domain->expected_subject_name_for_authentication);
	 uindex_i++;
#if 0
	 /*Number of Domain tables*/
	 //of_itoa (of_domain->number_of_tables, buf);
	 sprintf(buf,"%d",of_domain->number_of_tables);
	 FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_DOMAIN_NUMOFTABLES_ID,
				 CM_DATA_TYPE_STRING, buf);
	 uindex_i++;
#endif

	 result_iv_pairs_p->count_ui = uindex_i;
	 return OF_SUCCESS;
}

#endif /* OF_CM_SUPPORT */
