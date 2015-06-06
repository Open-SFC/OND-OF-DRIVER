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
 * File name: switchcbk.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: 
 * 
 */

#ifdef OF_CM_SUPPORT


#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "cmbufdef.h"

int32_t of_switch_appl_ucmcbk_init (void);
int32_t of_switch_addrec (void * config_trans_p,
			struct cm_array_of_iv_pairs * pMandParams,
			struct cm_array_of_iv_pairs * pOptParams,
			struct cm_app_result ** result_p);

int32_t of_switch_setrec (void * config_trans_p,
			struct cm_array_of_iv_pairs * pMandParams,
			struct cm_array_of_iv_pairs * pOptParams,
			struct cm_app_result ** result_p);

int32_t of_switch_delrec (void * config_transaction_p,
			struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_app_result ** result_p);

int32_t of_switch_getfirstnrecs (struct cm_array_of_iv_pairs * pkeysArr,
			uint32_t * count_p,
			struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);

int32_t of_switch_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_array_of_iv_pairs * prev_record_key_p, uint32_t * count_p,
			struct cm_array_of_iv_pairs ** next_n_record_data_p);

int32_t of_switch_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t of_switch_verifycfg (struct cm_array_of_iv_pairs * keys_arr_p,
			uint32_t command_id, struct cm_app_result ** result_p);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */
 int32_t of_switch_ucm_validatemandparams (struct cm_array_of_iv_pairs *
			pMandParams,
			struct cm_app_result **
			presult_p);

 int32_t of_switch_ucm_validateoptparams (struct cm_array_of_iv_pairs *
			pOptParams,
			struct cm_app_result ** presult_p);


 int32_t of_switch_ucm_setmandparams (struct cm_array_of_iv_pairs *
			pMandParams,
			struct dprm_switch_info * of_switch,
			struct cm_app_result ** presult_p);

 int32_t of_switch_ucm_setoptparams (struct cm_array_of_iv_pairs *
			pOptParams,
			struct dprm_switch_info * of_switch,
			struct cm_app_result ** presult_p);


 int32_t of_switch_ucm_getparams (struct dprm_switch_info *of_switch,
			struct cm_array_of_iv_pairs * result_iv_pairs_p);



struct cm_dm_app_callbacks of_switch_ucm_callbacks = {
	 NULL,
	 of_switch_addrec,
	 of_switch_setrec,
	 of_switch_delrec,
	 NULL,
	 of_switch_getfirstnrecs,
	 of_switch_getnextnrecs,
	 of_switch_getexactrec,
	 of_switch_verifycfg,
	 NULL
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * Switch  UCM Init * * * * * * *  * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t of_switch_appl_ucmcbk_init (void)
{
	 CM_CBK_DEBUG_PRINT ("Entry");
	 cm_register_app_callbacks (CM_ON_DIRECTOR_ONSWITCH_APPL_ID, &of_switch_ucm_callbacks);
	 return OF_SUCCESS;
}

int32_t of_switch_addrec (void * config_trans_p,
			struct cm_array_of_iv_pairs * pMandParams,
			struct cm_array_of_iv_pairs * pOptParams,
			struct cm_app_result ** result_p)
{
	 struct cm_app_result *of_switch_result = NULL;
	 int32_t return_value = OF_FAILURE;
	 struct dprm_switch_info of_switch = { };
	 uint64_t switch_handle;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 of_memset (&of_switch, 0, sizeof (struct dprm_switch_info));

	 if ((of_switch_ucm_setmandparams (pMandParams, &of_switch, &of_switch_result)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			*result_p=of_switch_result;
			return OF_FAILURE;
	 }

	 return_value = of_switch_ucm_setoptparams (pOptParams, &of_switch, &of_switch_result);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
			*result_p = of_switch_result;
			return OF_FAILURE;
	 }

	 return_value = dprm_register_switch(&of_switch,&switch_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Switch Addition Failed");
			fill_app_result_struct (&of_switch_result, NULL, CM_GLU_SWITCH_ADD_FAILED);
			*result_p = of_switch_result;
			return OF_FAILURE;
	 }

	 return OF_SUCCESS;
}

int32_t of_switch_setrec (void * config_trans_p,
			struct cm_array_of_iv_pairs * pMandParams,
			struct cm_array_of_iv_pairs * pOptParams,
			struct cm_app_result ** result_p)
{
	 struct cm_app_result *of_switch_result = NULL;
	 int32_t return_value = OF_FAILURE;
	 struct dprm_switch_info of_switch = { };
	 struct dprm_switch_runtime_info runtime_info;
	 uint64_t switch_handle;


	 CM_CBK_DEBUG_PRINT ("Entered");
	 of_memset (&of_switch, 0, sizeof (struct dprm_switch_info));

	 if ((of_switch_ucm_setmandparams (pMandParams, &of_switch, &of_switch_result)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			*result_p=of_switch_result;
			return OF_FAILURE;
	 }


	 return_value=dprm_get_switch_handle(of_switch.name, &switch_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Switch does not exist with the name %s",of_switch.name);
			fill_app_result_struct (&of_switch_result, NULL, CM_GLU_SWITCH_DOESNOT_EXIST);
			*result_p = of_switch_result;
			return OF_FAILURE;
	 }

	 of_memset (&of_switch, 0, sizeof (struct dprm_switch_info));
	 return_value=dprm_get_exact_switch(switch_handle, &of_switch, &runtime_info);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Unable to find the matching record");

			fill_app_result_struct (&of_switch_result, NULL, CM_GLU_NO_MATCHING_RECORD);
			*result_p = of_switch_result;
			return OF_FAILURE;
	 }
	 
         
	 return_value = of_switch_ucm_setoptparams (pOptParams, &of_switch, &of_switch_result);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
			*result_p = of_switch_result;
			return OF_FAILURE;
	 }
	 
	 return_value = dprm_update_switch(switch_handle,&of_switch);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("switch updation Failed");
			fill_app_result_struct (&of_switch_result, NULL, CM_GLU_SWITCH_UPDATE_FAILED);
			*result_p = of_switch_result;
			return OF_FAILURE;
	 }

	 return OF_SUCCESS;
}

int32_t of_switch_delrec (void * config_transaction_p,
			struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_app_result ** result_p)
{
	 struct cm_app_result *of_switch_result = NULL;
	 int32_t return_value = OF_FAILURE;
	 struct dprm_switch_info of_switch = { };
	 uint64_t switch_handle;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 of_memset (&of_switch, 0, sizeof (struct dprm_switch_info));

	 if ((of_switch_ucm_setmandparams (keys_arr_p, &of_switch, &of_switch_result)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			fill_app_result_struct (&of_switch_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
			*result_p = of_switch_result;
			return OF_FAILURE;
	 }

	 return_value=dprm_get_switch_handle(of_switch.name, &switch_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: switch does not exist with name %s",of_switch.name);
			fill_app_result_struct (&of_switch_result, NULL, CM_GLU_SWITCH_DOESNOT_EXIST);
			*result_p = of_switch_result;
			return OF_FAILURE;
	 }

	 return_value = dprm_unregister_switch(switch_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Delete Switch Record failed");
			fill_app_result_struct (&of_switch_result, NULL, CM_GLU_SWITCH_DELETE_FAILED);
			*result_p = of_switch_result;
			return OF_FAILURE;
	 }
	 return OF_SUCCESS;
}

int32_t of_switch_getfirstnrecs (struct cm_array_of_iv_pairs * pkeysArr,
			uint32_t * count_p,
			struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{

	 struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	 struct dprm_switch_info of_switch;
	 struct    dprm_switch_runtime_info runtime_info;
   uint64_t switch_handle;
	 int32_t iRes = OF_FAILURE;
	 uint32_t uiRecCount = 0;

	 CM_CBK_DEBUG_PRINT ("entry");

	 of_memset (&of_switch, 0, sizeof (struct dprm_switch_info));
	 iRes=dprm_get_first_switch(&of_switch, &runtime_info, &switch_handle);
	 if (iRes != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Get first record failed for Switch");
    return OF_FAILURE;
  }
  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
			return OF_FAILURE;
	 }

	 of_switch_ucm_getparams (&of_switch, &result_iv_pairs_p[uiRecCount]);
	 uiRecCount++;
	 *array_of_iv_pair_arr_p = result_iv_pairs_p;
	 *count_p = uiRecCount;
	 return OF_SUCCESS;
}


int32_t of_switch_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_array_of_iv_pairs *prev_record_key_p, uint32_t * count_p,
			struct cm_array_of_iv_pairs ** next_n_record_data_p)
{

	 struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	 struct cm_app_result *app_result_p = NULL;
	 struct dprm_switch_info of_switch;
	 struct    dprm_switch_runtime_info runtime_info;
   uint64_t switch_handle;
	 struct cm_app_result *of_switch_result = NULL;
	 int32_t iRes = OF_FAILURE;
	 uint32_t uiRecCount = 0;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 of_memset (&of_switch, 0, sizeof (struct dprm_switch_info));

	 if ((of_switch_ucm_setmandparams (prev_record_key_p, &of_switch, &app_result_p)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			return OF_FAILURE;
	 }

	 iRes=dprm_get_switch_handle(of_switch.name, &switch_handle);
	 if (iRes != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: switch does not exist with name %s",of_switch.name);
			return OF_FAILURE;
	 }

	 of_memset (&of_switch, 0, sizeof (struct dprm_switch_info));
	 iRes=dprm_get_next_switch(&of_switch, &runtime_info, &switch_handle);
  if (iRes != OF_SUCCESS)
	 {
    CM_CBK_DEBUG_PRINT ("failed");
    *count_p = 0;
    return OF_FAILURE;
  }
    result_iv_pairs_p =
      (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));

    if (result_iv_pairs_p == NULL)
    {
      CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
      return OF_FAILURE;
    }
			CM_CBK_DEBUG_PRINT ("Next Switch record is : %s ",
						of_switch.switch_fqdn);
			of_switch_ucm_getparams (&of_switch, &result_iv_pairs_p[uiRecCount]);
			uiRecCount++;
	 CM_CBK_DEBUG_PRINT ("Number of records requested were %ld ", *count_p);
	 CM_CBK_DEBUG_PRINT ("Number of records found are %ld", uiRecCount+1);
	 *next_n_record_data_p = result_iv_pairs_p;
	 *count_p = uiRecCount;
	 return OF_SUCCESS;
}

int32_t of_switch_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_array_of_iv_pairs ** pIvPairArr)
{

	 struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	 struct dprm_switch_info of_switch;
	 struct dprm_switch_runtime_info runtime_info;
   uint64_t switch_handle;
	 uint32_t ii = 0;
	 int32_t iRes = OF_FAILURE;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 of_memset (&of_switch, 0, sizeof (struct dprm_switch_info));

	 for (ii = 0; ii < keys_arr_p->count_ui; ii++)
	 {
			switch (keys_arr_p->iv_pairs[ii].id_ui)
			{
				 case CM_DM_ONSWITCH_TYPE_ID:
        CM_CBK_DEBUG_PRINT ("Switch Type id : %s ",
									(char *) keys_arr_p->iv_pairs[ii].value_p);
        of_switch.switch_type = ucm_uint8_from_str_ptr (keys_arr_p->iv_pairs[ii].value_p);
						break;
				 case CM_DM_ONSWITCH_FQDN_ID:
						CM_CBK_DEBUG_PRINT ("Switch FQDN Name : %s ",
									(char *) keys_arr_p->iv_pairs[ii].value_p);
						of_strncpy (of_switch.switch_fqdn,
									(char *) keys_arr_p->iv_pairs[ii].value_p,
									keys_arr_p->iv_pairs[ii].value_length);
						break;
				 case CM_DM_ONSWITCH_NAME_ID:
						CM_CBK_DEBUG_PRINT ("Switch Name is %s ",
									(char *) keys_arr_p->iv_pairs[ii].value_p);
						of_strncpy (of_switch.name,
									(char *) keys_arr_p->iv_pairs[ii].value_p,
									keys_arr_p->iv_pairs[ii].value_length);
				 default:
						break;
			}
	 }

	 iRes=dprm_get_switch_handle(of_switch.name, &switch_handle);
	 if (iRes != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: switch does not exist with name %s",of_switch.name);
			return OF_FAILURE;
	 }

	 of_memset (&of_switch, 0, sizeof (struct dprm_switch_info));
	 iRes=dprm_get_exact_switch(switch_handle, &of_switch, &runtime_info);
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
    CM_CBK_DEBUG_PRINT ("Failed to allocate memory for result_iv_pairs_p");
    return OF_FAILURE;
  }
			of_switch_ucm_getparams (&of_switch, result_iv_pairs_p);
			*pIvPairArr = result_iv_pairs_p;
  return OF_SUCCESS;
}

int32_t of_switch_verifycfg (struct cm_array_of_iv_pairs * keys_arr_p,
			uint32_t command_id, struct cm_app_result ** result_p)
{
	 struct cm_app_result *of_switch_result = NULL;
	 int32_t return_value = OF_FAILURE;
	 struct dprm_switch_info of_switch = { };

	 CM_CBK_DEBUG_PRINT ("Entered");
	 of_memset (&of_switch, 0, sizeof (struct dprm_switch_info));

	 return_value = of_switch_ucm_validatemandparams (keys_arr_p, &of_switch_result);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
			return OF_FAILURE;
	 }
#if 0
	 if (of_switch_ucm_validateoptparams (keys_arr_p, &of_switch_result)
				 != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
			return OF_FAILURE;
	 }
#endif
	 *result_p = of_switch_result;
	 return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */

 int32_t of_switch_ucm_validatemandparams (struct cm_array_of_iv_pairs *
			pMandParams,
			struct cm_app_result **	presult_p)
{
	 uint32_t uiMandParamCnt;
	 struct cm_app_result *of_switch_result = NULL;


	 CM_CBK_DEBUG_PRINT ("Entered");
	 for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;
				 uiMandParamCnt++)
	 {
			switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
			{

				 case CM_DM_ONSWITCH_NAME_ID:
						if (pMandParams->iv_pairs[uiMandParamCnt].value_p == NULL)
						{
							 CM_CBK_DEBUG_PRINT ("Switch Name is NULL");
							 fill_app_result_struct (&of_switch_result, NULL, CM_GLU_SWITCH_NAME_NULL);
							 *presult_p = of_switch_result;
							 return OF_FAILURE;
						}
						break;

			}
	 }
	 CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
	 return OF_SUCCESS;
}

 int32_t of_switch_ucm_validateoptparams (struct cm_array_of_iv_pairs *
			pOptParams,
			struct cm_app_result ** presult_p)
{
	 uint32_t uiOptParamCnt;
	 struct cm_app_result *of_switch_result = NULL;
  uint8_t switch_type;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 for (uiOptParamCnt = 0; uiOptParamCnt < pOptParams->count_ui; uiOptParamCnt++)
	 {
			switch (pOptParams->iv_pairs[uiOptParamCnt].id_ui)
			{
				 case CM_DM_ONSWITCH_FQDN_ID:
						if (pOptParams->iv_pairs[uiOptParamCnt].value_p == NULL)
						{
							 CM_CBK_DEBUG_PRINT ("Fully Qualified Domain Name (FQDN) is NULL");
							 fill_app_result_struct (&of_switch_result, NULL, CM_GLU_SWITCH_FQDN_NULL);
							 *presult_p = of_switch_result;
							 return OF_FAILURE;
						}
						break;
				 case CM_DM_ONSWITCH_TYPE_ID:
						if (pOptParams->iv_pairs[uiOptParamCnt].value_p == NULL)
						{
							 CM_CBK_DEBUG_PRINT ("Switch Name is NULL");
							 fill_app_result_struct (&of_switch_result, NULL, CM_GLU_SWITCH_NAME_NULL);
							 *presult_p = of_switch_result;
							 return OF_FAILURE;
						}
						switch_type =
          ucm_uint8_from_str_ptr ( pOptParams->iv_pairs[uiOptParamCnt].value_p);
						if (switch_type < 0 ||  switch_type > 1)
						{
							 CM_CBK_DEBUG_PRINT ("Switch Id is exceeding the maximum number");
							 fill_app_result_struct (&of_switch_result, NULL, CM_GLU_SWITCH_NAME_EXCEEDED);
							 *presult_p = of_switch_result;
							 return OF_FAILURE;
						}
						break;
        case CM_DM_ONSWITCH_BADDRIP_ID:
        if (pOptParams->iv_pairs[uiOptParamCnt].value_p == NULL)
         {
          CM_CBK_DEBUG_PRINT (" Baddr IP is NULL");
          fill_app_result_struct (&of_switch_result, NULL, CM_GLU_SWITCH_BADDR_IP_NULL);
          *presult_p = of_switch_result;
          return OF_FAILURE;
        }
        break;

				 default:
						break;
			}
	 }
	 CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
	 return OF_SUCCESS;
}

 int32_t of_switch_ucm_setmandparams (struct cm_array_of_iv_pairs *
			pMandParams,
			struct dprm_switch_info * of_switch,
			struct cm_app_result ** presult_p)
{
	 uint32_t uiMandParamCnt;
	 uint32_t switch_type;

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
						break;

			}
	 }
	 CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
	 return OF_SUCCESS;
}

 int32_t of_switch_ucm_setoptparams (struct cm_array_of_iv_pairs *
			pOptParams,
			struct dprm_switch_info * of_switch,
			struct cm_app_result ** presult_p)
{
	 uint32_t uiOptParamCnt;
  uint8_t switch_type;

	 CM_CBK_DEBUG_PRINT ("Entered");

	 for (uiOptParamCnt = 0; uiOptParamCnt < pOptParams->count_ui; uiOptParamCnt++)
	 {
		 switch (pOptParams->iv_pairs[uiOptParamCnt].id_ui)
		 {
			 case CM_DM_ONSWITCH_TYPE_ID:
				 switch_type = ucm_uint8_from_str_ptr(pOptParams->iv_pairs[uiOptParamCnt].value_p);
        of_switch->switch_type = switch_type;
				 break;

			 case CM_DM_ONSWITCH_BADDR_ID:
				 of_switch->ipv4addr.baddr_set = of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p);
				 break;



                         case CM_DM_ONSWITCH_BADDRIP_ID:
          CM_CBK_DEBUG_PRINT ("switch_baddr_ip:%s",(char *) pOptParams->iv_pairs[uiOptParamCnt].value_p);
            if(cm_val_and_conv_aton((char *)pOptParams->iv_pairs[uiOptParamCnt].value_p,&(of_switch->ipv4addr.addr))!=OF_SUCCESS)
            return OF_FAILURE;
            break;
			 case CM_DM_ONSWITCH_FQDN_ID:
                                of_memset(of_switch->switch_fqdn, 0, sizeof(of_switch->switch_fqdn));
				 of_strncpy (of_switch->switch_fqdn,
						 (char *) pOptParams->iv_pairs[uiOptParamCnt].value_p,
						 pOptParams->iv_pairs[uiOptParamCnt].value_length);
				 break;

			 default:
				 break;
		 }
	 }

	 CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
	 return OF_SUCCESS;
}



 int32_t of_switch_ucm_getparams (struct dprm_switch_info *of_switch,
			struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
	 uint32_t uindex_i = 0;
	 char buf[128] = "";
#define CM_SWITCH_CHILD_COUNT 5

	 result_iv_pairs_p->iv_pairs =
			(struct cm_iv_pair *) of_calloc (CM_SWITCH_CHILD_COUNT, sizeof (struct cm_iv_pair));
	 if (result_iv_pairs_p->iv_pairs == NULL)
	 {
			CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p->iv_pairs");
			return OF_FAILURE;
	 }

	 /* Switch Interface*/
	 FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_ONSWITCH_NAME_ID,
				 CM_DATA_TYPE_STRING, of_switch->name);
	 uindex_i++;

	 /*Switch name*/
	 FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_ONSWITCH_FQDN_ID,
				 CM_DATA_TYPE_STRING, of_switch->switch_fqdn);
	 uindex_i++;

	 /* Switch Id*/
  of_itoa (of_switch->switch_type, buf);
	 FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_ONSWITCH_TYPE_ID,
				 CM_DATA_TYPE_STRING, buf);
	 uindex_i++;

	 /*broadcast address flag*/
	 of_itoa (of_switch->ipv4addr.baddr_set, buf);
	 FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_ONSWITCH_BADDR_ID,
				 CM_DATA_TYPE_STRING, buf);
	 uindex_i++;
       
         cm_inet_ntoal(of_switch->ipv4addr.addr, buf);

   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_ONSWITCH_BADDRIP_ID,
                    CM_DATA_TYPE_IPADDR, buf);
   uindex_i++;


	 result_iv_pairs_p->count_ui = uindex_i;
	 CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);
	 return OF_SUCCESS;
}


#endif /* OF_CM_SUPPORT */
