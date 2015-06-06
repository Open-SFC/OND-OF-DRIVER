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
 * File name: portcbk.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: 
 * 
 */

#ifdef OF_CM_SUPPORT


#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"


int32_t dprm_switch_attribute_appl_ucmcbk_init (void);

int32_t dprm_switch_attribute_addrec (void * config_trans_p,
			struct cm_array_of_iv_pairs * pMandParams,
			struct cm_array_of_iv_pairs * pOptParams,
			struct cm_app_result ** result_p);

int32_t dprm_switch_attribute_modrec (void * config_trans_p,
			struct cm_array_of_iv_pairs * pMandParams,
			struct cm_array_of_iv_pairs * pOptParams,
			struct cm_app_result ** result_p);

int32_t dprm_switch_attribute_delrec (void * config_transaction_p,
			struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_app_result ** result_p);

int32_t dprm_switch_attribute_verifycfg (struct cm_array_of_iv_pairs * keys_arr_p,
			uint32_t command_id, struct cm_app_result ** result_p);

int32_t dprm_switch_attribute_getfirstnrecs(struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);

int32_t dprm_switch_attribute_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p, uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p);

int32_t dprm_get_switch_attribute_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_array_of_iv_pairs ** pIvPairArr);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * dpattribute  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */

int32_t dprm_switch_attribute_ucm_validatemandparams (struct cm_array_of_iv_pairs *pMandParams,
			struct cm_app_result **presult_p);



int32_t dprm_switch_attribute_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
			struct dprm_switch_info     *datapath_info,
			struct dprm_attribute_name_value_pair *dpattribute_info,
			struct cm_app_result                  **presult_p);

int32_t dprm_switch_attribute_ucm_setoptparams (struct cm_array_of_iv_pairs *pOptParams,
			struct dprm_attribute_name_value_pair                    *dpattribute_info,
			struct cm_app_result                                     **presult_p);





struct cm_dm_app_callbacks dprm_switch_attribute_ucm_callbacks = {
	 NULL,
	 dprm_switch_attribute_addrec,
	 dprm_switch_attribute_modrec,
	 dprm_switch_attribute_delrec,
	 NULL,
	 dprm_switch_attribute_getfirstnrecs,
	 dprm_switch_attribute_getnextnrecs,
	 dprm_get_switch_attribute_getexactrec,
	 dprm_switch_attribute_verifycfg,
	 NULL
};

int32_t dprm_switch_attribute_appl_ucmcbk_init (void)
{
         CM_CBK_DEBUG_PRINT ("Entry");
         cm_register_app_callbacks (CM_ON_DIRECTOR_ONSWITCH_SWITCHATTRIBUTE_APPL_ID, &dprm_switch_attribute_ucm_callbacks);
         return OF_SUCCESS;
}

int32_t dprm_switch_attribute_addrec (void * config_trans_p,
			struct cm_array_of_iv_pairs * pMandParams,
			struct cm_array_of_iv_pairs * pOptParams,
			struct cm_app_result ** result_p)
{
	 struct   cm_app_result *dpattribute_result = NULL;
         struct   dprm_attribute_name_value_pair dprm_attribute_name_value_info={};
	 struct   dprm_switch_info switch_info={};

	 uint64_t switch_handle;
	 int32_t  return_value = OF_FAILURE;

    	 CM_CBK_DEBUG_PRINT ("Entered");

	 of_memset (&dprm_attribute_name_value_info, 0, sizeof (struct dprm_attribute_name_value_pair));


	 if ((dprm_switch_attribute_ucm_setmandparams (pMandParams, 
	      &switch_info, &dprm_attribute_name_value_info, 
	      &dpattribute_result)) != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			fill_app_result_struct (&dpattribute_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
			*result_p=dpattribute_result;
			return OF_FAILURE;
	 }


	 return_value = dprm_switch_attribute_ucm_setoptparams (pOptParams, 
		&dprm_attribute_name_value_info, 
		&dpattribute_result);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
			fill_app_result_struct (&dpattribute_result, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
			*result_p = dpattribute_result;
			return OF_FAILURE;
	 }

	 return_value = dprm_get_switch_handle(switch_info.name, &switch_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Switch does not exist with id %llx",switch_handle);
			fill_app_result_struct (&dpattribute_result, NULL, CM_GLU_SWITCH_DOESNOT_EXIST);
			*result_p = dpattribute_result;
			return OF_FAILURE;
	 }

	 return_value = dprm_add_attribute_to_switch(switch_handle, &dprm_attribute_name_value_info);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Switch Attribute Addition Failed (ret:%d)",return_value);
			*result_p = dpattribute_result;
			return OF_FAILURE;
	 }

	 CM_CBK_DEBUG_PRINT ("Switch Attribute added succesfully");
	 return OF_SUCCESS;

}

int32_t dprm_switch_attribute_modrec (void * config_trans_p,
			struct cm_array_of_iv_pairs * pMandParams,
			struct cm_array_of_iv_pairs * pOptParams,
			struct cm_app_result ** result_p)
{
	 struct   cm_app_result *dpattribute_result = NULL;
         struct   dprm_attribute_name_value_pair dprm_attribute_name_value_info={};
	 struct   dprm_switch_info switch_info={};

	 uint64_t switch_handle;
	 int32_t  return_value = OF_FAILURE;

    	 CM_CBK_DEBUG_PRINT ("Entered");

	 of_memset (&dprm_attribute_name_value_info, 0, sizeof (struct dprm_attribute_name_value_pair));


	 if ((dprm_switch_attribute_ucm_setmandparams (pMandParams, 
	      &switch_info, &dprm_attribute_name_value_info, 
	      &dpattribute_result)) != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			fill_app_result_struct (&dpattribute_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
			*result_p=dpattribute_result;
			return OF_FAILURE;
	 }


	 return_value = dprm_switch_attribute_ucm_setoptparams (pOptParams, 
		&dprm_attribute_name_value_info, 
		&dpattribute_result);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
			fill_app_result_struct (&dpattribute_result, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
			*result_p = dpattribute_result;
			return OF_FAILURE;
	 }


	 return_value = dprm_get_switch_handle(switch_info.name, &switch_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Switch does not exist with id %llx",switch_handle);
			fill_app_result_struct (&dpattribute_result, NULL, CM_GLU_GROUP_DATAPATH_DOESNOT_EXIST);
			*result_p = dpattribute_result;
			return OF_FAILURE;
	 }

	 return_value = dprm_add_attribute_to_switch(switch_handle, &dprm_attribute_name_value_info);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Switch Attribute Addition Failed (ret:%d",return_value);
			fill_app_result_struct (&dpattribute_result, NULL, CM_GLU_PORT_ADD_FAILED);
			*result_p = dpattribute_result;
			return OF_FAILURE;
	 }

	 CM_CBK_DEBUG_PRINT ("Switch Attribute added succesfully");
	 return OF_SUCCESS;

}
int32_t dprm_switch_attribute_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p, uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p)
{
	struct   cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	struct   dprm_attribute_name_value_pair        dprm_attribute_name_value_info={};
	struct   dprm_attribute_name_value_output_info dprm_attribute_output={};
	struct   dprm_switch_info            switch_info={};
	char     current_name_string[DPRM_MAX_ATTRIBUTE_NAME_LEN];
	char     current_value_string[DPRM_MAX_ATTRIBUTE_VALUE_LEN];

	CM_CBK_DEBUG_PRINT ("Entered");

	uint64_t switch_handle;
	int32_t return_value = OF_FAILURE;
	uint32_t uiRecCount = 0;
	uint32_t uindex_i = 0;

	of_memset (&dprm_attribute_name_value_info, 0, sizeof (struct dprm_attribute_name_value_pair));
	if ((dprm_switch_attribute_ucm_setmandparams (keys_arr_p, 
					&switch_info, &dprm_attribute_name_value_info, 
					NULL)) != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		return OF_FAILURE;
	}

	return_value = dprm_get_switch_handle(switch_info.name, &switch_handle);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Datapath does not exist with id %llx",switch_info.name);
		return OF_FAILURE;
	}
	strcpy(current_name_string,  prev_record_key_p->iv_pairs[uindex_i].value_p);
	strcpy(current_value_string, prev_record_key_p->iv_pairs[uindex_i+1].value_p);
	strcpy(dprm_attribute_output.name_string, current_name_string);
	dprm_attribute_output.name_length = DPRM_MAX_ATTRIBUTE_NAME_LEN -1;
	dprm_attribute_output.value_length = DPRM_MAX_ATTRIBUTE_VALUE_LEN -1;

	return_value = dprm_get_switch_attribute_next_value(switch_handle, current_name_string, current_value_string, &dprm_attribute_output);
	if (return_value != OF_SUCCESS)
	{
		do
		{
			dprm_attribute_output.name_length = DPRM_MAX_ATTRIBUTE_NAME_LEN -1;
			dprm_attribute_output.value_length = DPRM_MAX_ATTRIBUTE_VALUE_LEN -1;

			return_value = dprm_get_switch_next_attribute_name(switch_handle, dprm_attribute_output.name_string,&dprm_attribute_output);
			if (return_value != OF_SUCCESS)
			{
				CM_CBK_DEBUG_PRINT ("dprm_get_first_datapath_attribute_name failed!");
				return OF_FAILURE;
			}

			return_value = dprm_get_switch_attribute_first_value(switch_handle, &dprm_attribute_output);
		}
		while(return_value!=OF_SUCCESS);

	}

	if (return_value != OF_SUCCESS)
	{
		return OF_FAILURE;
	}

	result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
	if (result_iv_pairs_p == NULL)
	{
		CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
		return OF_FAILURE;
	}
	dprm_switch_attribute_getparams(&dprm_attribute_output, &result_iv_pairs_p[uiRecCount]);
	uiRecCount++;
	*next_n_record_data_p = result_iv_pairs_p;
	*count_p = uiRecCount;	
	return DPRM_SUCCESS;  
}

int32_t dprm_switch_attribute_getfirstnrecs(struct cm_array_of_iv_pairs * keys_arr_p,
		uint32_t * count_p,
		struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{
	struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	struct   dprm_attribute_name_value_pair dprm_attribute_name_value_info={};
	struct   dprm_attribute_name_value_output_info dprm_attribute_output={};
	struct   dprm_switch_info switch_info={};

	uint64_t switch_handle;
	int32_t return_value = OF_FAILURE;
	uint32_t uiRecCount = 0;

	CM_CBK_DEBUG_PRINT ("Entered");

	of_memset (&dprm_attribute_name_value_info, 0, sizeof (struct dprm_attribute_name_value_pair));
	if ((dprm_switch_attribute_ucm_setmandparams (keys_arr_p, 
					&switch_info, &dprm_attribute_name_value_info, 
					NULL)) != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		return OF_FAILURE;
	}
	return_value = dprm_get_switch_handle(switch_info.name, &switch_handle);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Datapath does not exist with id %s",switch_info.name);
		return OF_FAILURE;
	}

	dprm_attribute_output.name_length = DPRM_MAX_ATTRIBUTE_NAME_LEN -1;
	dprm_attribute_output.value_length = DPRM_MAX_ATTRIBUTE_VALUE_LEN -1;

	return_value = dprm_get_switch_first_attribute_name(switch_handle, &dprm_attribute_output);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("dprm_get_first_switch_attribute_name failed!");
		return OF_FAILURE;
	}
	return_value = dprm_get_switch_attribute_first_value(switch_handle, &dprm_attribute_output);
	if(return_value == OF_SUCCESS)
	{ 
		result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
		if (result_iv_pairs_p == NULL)
		{
			CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
			return OF_FAILURE;
		}
		dprm_switch_attribute_getparams(&dprm_attribute_output, &result_iv_pairs_p[uiRecCount]);
		uiRecCount++;
		*array_of_iv_pair_arr_p = result_iv_pairs_p;
		*count_p = uiRecCount;  
	}
	return DPRM_SUCCESS;  
}

int32_t dprm_get_switch_attribute_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_array_of_iv_pairs ** pIvPairArr)

{
         struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	 int32_t  return_value = OF_FAILURE;
         struct   dprm_attribute_name_value_pair dprm_attribute_name_value_info={};
   	 struct   dprm_attribute_name_value_output_info dprm_attribute_output={};
	 struct   dprm_switch_info switch_info={};
	 uint64_t switch_handle;

    	 CM_CBK_DEBUG_PRINT ("Entered");

         result_iv_pairs_p =
                        (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
         if (result_iv_pairs_p == NULL)
         {
                        CM_CBK_DEBUG_PRINT ("Failed to allocate memory for result_iv_pairs_p");
                        return OF_FAILURE;
         }


	 of_memset (&dprm_attribute_name_value_info, 0, sizeof (struct dprm_attribute_name_value_pair));

	 if ((dprm_switch_attribute_ucm_setmandparams (keys_arr_p, 
	      &switch_info, &dprm_attribute_name_value_info, 
	      NULL)) != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			return OF_FAILURE;
	 }
	 return_value = dprm_get_switch_handle(switch_info.name, &switch_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("switch does not exist with name %s",switch_info.name);
			return OF_FAILURE;
	 }
         strcpy(dprm_attribute_output.name_string, dprm_attribute_name_value_info.name_string);
         dprm_attribute_output.name_length = strlen(dprm_attribute_output.name_string);

	 return_value = dprm_get_switch_exact_attribute_name(switch_handle, dprm_attribute_output.name_string);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Unable to find get exact attribute record!");
			return OF_FAILURE;
	 }

         dprm_switch_attribute_getparams (&dprm_attribute_name_value_info, result_iv_pairs_p);
         *pIvPairArr = result_iv_pairs_p;
         return return_value;
}

int32_t dprm_switch_attribute_getparams (struct dprm_attribute_name_value_output_info *dpattribute_info,
		struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
	uint32_t uindex_i = 0;
	char buf[64] = "";

	CM_CBK_DEBUG_PRINT ("Entered");
	result_iv_pairs_p->iv_pairs = (struct cm_iv_pair *) of_calloc (2, sizeof (struct cm_iv_pair));
	if (result_iv_pairs_p->iv_pairs == NULL)
	{
		CM_CBK_DEBUG_PRINT("Memory allocation failed for result_iv_pairs_p->iv_pairs");
		return OF_FAILURE;
	}

	FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_SWITCHATTRIBUTE_ATTRIBUTENAME_ID,
			CM_DATA_TYPE_STRING, dpattribute_info->name_string);
	uindex_i++;

	FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_SWITCHATTRIBUTE_ATTRIBUTEVAL_ID,
			CM_DATA_TYPE_STRING, dpattribute_info->value_string);
	uindex_i++;

	result_iv_pairs_p->count_ui = uindex_i;
	return OF_SUCCESS;   

}


int32_t dprm_switch_attribute_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
			struct dprm_switch_info *of_switch,
			struct dprm_attribute_name_value_pair *dpattribute_info,
			struct cm_app_result              **presult_p)
{
	 uint32_t uiMandParamCnt;
	 uint64_t idpId;
         int32_t iportId;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;
				 uiMandParamCnt++)
	 {
			switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
			{
				 case CM_DM_ONSWITCH_NAME_ID:
                                                of_strncpy (of_switch->name,
                                                                        (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                                                                        pMandParams->iv_pairs[uiMandParamCnt].value_length);

						CM_CBK_DEBUG_PRINT ("switch name %s",of_switch->name);
                                                break;


				 case CM_DM_SWITCHATTRIBUTE_ATTRIBUTENAME_ID:
						of_strncpy (dpattribute_info->name_string,
									(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
									pMandParams->iv_pairs[uiMandParamCnt].value_length);
						CM_CBK_DEBUG_PRINT ("Attribute name %s",dpattribute_info->name_string);
						break;

				 case CM_DM_SWITCHATTRIBUTE_ATTRIBUTEVAL_ID:
						of_strncpy (dpattribute_info->value_string,
									(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
									pMandParams->iv_pairs[uiMandParamCnt].value_length);
						CM_CBK_DEBUG_PRINT ("Attribute value %s",dpattribute_info->value_string);
						break;
			}
	 }
	 CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
	 return OF_SUCCESS;
}

 int32_t dprm_switch_attribute_ucm_setoptparams (struct cm_array_of_iv_pairs *pOptParams,
			struct dprm_attribute_name_value_pair                    *dpattribute_info,
			struct cm_app_result                                     **presult_p)
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


int32_t dprm_switch_attribute_verifycfg (struct cm_array_of_iv_pairs * keys_arr_p,
			uint32_t command_id, struct cm_app_result ** result_p)
{
	 struct cm_app_result *dpattribute_result = NULL;
	 int32_t return_value = OF_FAILURE;

	 CM_CBK_DEBUG_PRINT ("Entered");

	 return_value = dprm_switch_attribute_ucm_validatemandparams (keys_arr_p, &dpattribute_result);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
			return OF_FAILURE;
	 }

	 if (datapath_ucm_validateoptparams (keys_arr_p, &dpattribute_result)
				 != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
			return OF_FAILURE;
	 }

	 *result_p = dpattribute_result;
	 return OF_SUCCESS;
}

int32_t dprm_switch_attribute_ucm_validatemandparams (struct cm_array_of_iv_pairs *
			pMandParams,	struct cm_app_result **presult_p)
{
	 uint32_t uiMandParamCnt;
	 struct cm_app_result *dpattribute_result = NULL;
	 uint32_t uiPortId;


	 CM_CBK_DEBUG_PRINT ("Entered");
	 for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;
				 uiMandParamCnt++)
	 {
			switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
			{
				 case CM_DM_DPATTRIBUTE_ATTRIBUTENAME_ID:
						if (pMandParams->iv_pairs[uiMandParamCnt].value_p == NULL)
						{
							 CM_CBK_DEBUG_PRINT ("Attribute Name is NULL");
							 fill_app_result_struct (&dpattribute_result, NULL, CM_GLU_PORT_NAME_NULL);
							 *presult_p = dpattribute_result;
							 return OF_FAILURE;
						}
						break;

				 case CM_DM_DPATTRIBUTE_ATTRIBUTEVAL_ID: 
						if (pMandParams->iv_pairs[uiMandParamCnt].value_p == NULL)
						{
							 CM_CBK_DEBUG_PRINT ("Attribute Value is NULL");
							 fill_app_result_struct (&dpattribute_result, NULL, CM_GLU_PORT_ID_NULL);
							 *presult_p = dpattribute_result;
							 return OF_FAILURE;
						}
						break;
			}
	 }
	 CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
	 return OF_SUCCESS;
}
int32_t dprm_switch_attribute_delrec (void * config_transaction_p,
			struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_app_result ** result_p)
{
	 struct cm_app_result *dpattribute_result = NULL;
	 int32_t return_value = OF_FAILURE;
	 struct dprm_attribute_name_value_pair dpattribute_info={};
	 struct dprm_switch_info switch_info={};
	 uint64_t switch_handle;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 of_memset (&dpattribute_info, 0, sizeof (struct dprm_attribute_name_value_pair));

	 if ((dprm_switch_attribute_ucm_setmandparams (keys_arr_p, &switch_info, &dpattribute_info, &dpattribute_result)) != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			fill_app_result_struct (&dpattribute_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
			*result_p = dpattribute_result;
			return OF_FAILURE;
	 }

	 return_value=dprm_get_switch_handle(switch_info.name, &switch_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Get datapath handle failed");
			fill_app_result_struct (&dpattribute_result, NULL, CM_GLU_NO_MATCHING_PORT_RECORD);
			*result_p = dpattribute_result;
			return OF_FAILURE;
	 }
	 return_value=dprm_delete_attribute_from_switch(switch_handle, &dpattribute_info);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Delete datapath attribute record failed");
			fill_app_result_struct (&dpattribute_result, NULL, CM_GLU_PORT_DELETE_FAILED);
			*result_p = dpattribute_result;
			return OF_FAILURE;
	 }

         CM_CBK_DEBUG_PRINT ("Datapath attribute deleted succesfully");
	 return OF_SUCCESS;
}
#endif
