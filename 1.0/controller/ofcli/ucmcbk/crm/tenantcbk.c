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
 * File name: tenantcbk.c
 * Author: Varun Kumar Reddy <b36461@freescale.com>
 * Date:   15/10/2013
 * Description: 
 * 
 */

#ifdef OF_CM_SUPPORT


#include "cmincld.h"
#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "crm_tenant_api.h"

int32_t crm_tenant_appl_ucmcbk_init (void);

int32_t crm_tenant_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);


int32_t crm_tenant_modrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);

int32_t crm_tenant_setrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);

int32_t crm_tenant_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p);

int32_t crm_tenant_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);

int32_t crm_tenant_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs * prev_record_key_p, uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p);

int32_t crm_tenant_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t crm_tenant_verifycfg (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t command_id, struct cm_app_result ** result_p);







/************************ *********************************************/
int32_t crm_tenant_ucm_validatemandparams (struct cm_array_of_iv_pairs *
                        pMandParams,
                        struct cm_app_result **
                        presult_p);

int32_t crm_tenant_ucm_validateoptparams (struct cm_array_of_iv_pairs *
                        pOptParams,
                        struct cm_app_result ** presult_p);


int32_t crm_tenant_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                        struct crm_tenant_config_info *crm_tenant_config_info_p,
                        struct cm_app_result ** presult_p);

int32_t crm_tenant_ucm_setoptparams (struct cm_array_of_iv_pairs *pOptParams,
			struct crm_tenant_config_info *crm_tenant_config_info_p,
                        struct cm_app_result ** presult_p);


int32_t crm_tenant_ucm_getparams (struct crm_tenant_config_info *crm_tenant_config_info_p,
                        struct cm_array_of_iv_pairs * result_iv_pairs_p);


struct cm_dm_app_callbacks crm_tenant_ucm_callbacks = 
{
  NULL,
  crm_tenant_addrec,
  NULL,
  crm_tenant_delrec,
  NULL,
  crm_tenant_getfirstnrecs,
  crm_tenant_getnextnrecs,
  crm_tenant_getexactrec,
  crm_tenant_verifycfg,
  NULL
};


int32_t crm_tenant_appl_ucmcbk_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  int32_t ret_val;
  ret_val = cm_register_app_callbacks (CM_ON_DIRECTOR_CRM_TENANT_APPL_ID, &crm_tenant_ucm_callbacks);
  if(ret_val != CRM_SUCCESS)
     return OF_FAILURE;
  return OF_SUCCESS;
}



int32_t crm_tenant_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p)
{
   struct cm_app_result *crm_tenant_result = NULL;
   int32_t return_value = OF_FAILURE;
   struct crm_tenant_config_info crm_tenatn_config_info={};
   uint64_t crm_tenant_handle;

   CM_CBK_DEBUG_PRINT ("Entered(crm_tenant_addrec)");

  if ((crm_tenant_ucm_setmandparams (pMandParams, &crm_tenatn_config_info, &crm_tenant_result)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&crm_tenant_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=crm_tenant_result;
    return OF_FAILURE;
  }

  return_value =crm_add_tenant(&crm_tenatn_config_info,  &crm_tenant_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("crm tenant add  Failed");
    fill_app_result_struct (&crm_tenant_result, NULL, CM_GLU_TENANT_ADD_FAILED);
    *result_p = crm_tenant_result;
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("CRM Tenant added successfully");
  return OF_SUCCESS;  
}


int32_t crm_tenant_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p)
{
  struct cm_app_result *crm_tenant_result = NULL;
  int32_t return_value = OF_FAILURE;

  struct crm_tenant_config_info  tenant_info={};

  CM_CBK_DEBUG_PRINT ("Entered");
  if ((crm_tenant_ucm_setmandparams (keys_arr_p, &tenant_info, &crm_tenant_result)) !=
           OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&crm_tenant_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p = crm_tenant_result;
    return OF_FAILURE;
  }
  return_value =crm_del_tenant(tenant_info.tenant_name);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("crm tenant add  Failed");
    fill_app_result_struct (&crm_tenant_result, NULL, CM_GLU_TENANT_DEL_FAILED);
    *result_p = crm_tenant_result;
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("CRM Tenant delete successfully");
  return OF_SUCCESS;  

}
int32_t crm_tenant_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;

  struct  crm_tenant_config_info crm_tenant_config_info={};
  struct  tenant_runtime_info  runtime_info={};
  uint64_t tenant_handle;

  CM_CBK_DEBUG_PRINT ("Entered");
 return_value = crm_get_first_tenant(&crm_tenant_config_info, &runtime_info, &tenant_handle);
 if (return_value != OF_SUCCESS)
 {
   CM_CBK_DEBUG_PRINT ("Get first record failed for CRM Tenant Table");
   return OF_FAILURE;
 }

CM_CBK_DEBUG_PRINT ("name:%s",crm_tenant_config_info.tenant_name);
 result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
 if (result_iv_pairs_p == NULL)
 {
   CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    return OF_FAILURE;
 }
 crm_tenant_ucm_getparams (&crm_tenant_config_info, &result_iv_pairs_p[uiRecCount]);
 uiRecCount++;
 *array_of_iv_pair_arr_p = result_iv_pairs_p;
 *count_p = uiRecCount;
 return OF_SUCCESS;
}


int32_t crm_tenant_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p, uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p)
{

  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct cm_app_result *tenant_result = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;

  struct  crm_tenant_config_info tenant_config_info={};
  struct  tenant_runtime_info  runtime_info={};
  uint64_t tenant_handle;

  CM_CBK_DEBUG_PRINT ("Entered");

 if ((crm_tenant_ucm_setmandparams (prev_record_key_p, &tenant_config_info, &tenant_result)) !=
                                OF_SUCCESS)
  {
     CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
      return OF_FAILURE;
  }

  return_value = crm_get_tenant_handle(tenant_config_info.tenant_name, &tenant_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: tenant doesn't exists with name %s",tenant_config_info.tenant_name);
    return OF_FAILURE;
  }

  of_memset (&tenant_config_info, 0, sizeof(tenant_config_info));


  return_value = crm_get_next_tenant(&tenant_config_info, &runtime_info, &tenant_handle);

  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("failed");
    *count_p = 0;
    return OF_FAILURE;
  }
CM_CBK_DEBUG_PRINT ("name:%s",tenant_config_info.tenant_name);
  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
     return OF_FAILURE;
  }
  crm_tenant_ucm_getparams (&tenant_config_info, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *next_n_record_data_p = result_iv_pairs_p;
  *count_p = uiRecCount;
  return OF_SUCCESS;
}
 



int32_t crm_tenant_ucm_getparams (struct crm_tenant_config_info *tenant_info,
                        struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
         uint32_t uindex_i = 0;
         char buf[128] = "";
         CM_CBK_DEBUG_PRINT ("Entered");
#define CM_TENANT_CHILD_COUNT 1

         result_iv_pairs_p->iv_pairs =
                        (struct cm_iv_pair *) of_calloc (CM_TENANT_CHILD_COUNT, sizeof (struct cm_iv_pair));
         if (result_iv_pairs_p->iv_pairs == NULL)
         {
                        CM_CBK_DEBUG_PRINT
                                 ("Memory allocation failed for result_iv_pairs_p->iv_pairs");
                        return OF_FAILURE;
         }

         sprintf(buf,"%s",tenant_info->tenant_name);
         FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_TENANT_NAME_ID,
                                 CM_DATA_TYPE_STRING, buf);
         uindex_i++;

        result_iv_pairs_p->count_ui = uindex_i;
         return OF_SUCCESS;


}




int32_t crm_tenant_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                        struct crm_tenant_config_info* tenant_info,               struct cm_app_result ** presult_p)
{
         uint32_t uiMandParamCnt;

         CM_CBK_DEBUG_PRINT ("Entered");
         for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;
                                 uiMandParamCnt++)
         {
                        switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
                        {
                                 case CM_DM_TENANT_NAME_ID:
         					CM_CBK_DEBUG_PRINT ("tenant_name:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
                                                of_strncpy (tenant_info->tenant_name,
                                                                        (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                                                                        pMandParams->iv_pairs[uiMandParamCnt].value_length);
                                                break;
                        }
          }
	  return OF_SUCCESS;
}

int32_t crm_tenant_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
                        uint32_t command_id, struct cm_app_result ** result_p)
{
  struct cm_app_result *of_tenant_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct crm_tenant_config_info   tenant_config_info={};

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&tenant_config_info, 0, sizeof (tenant_config_info));

  return_value = crm_tenant_ucm_validatemandparams(key_iv_pairs_p, &of_tenant_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    return OF_FAILURE;
  }
  *result_p = of_tenant_result;
  return OF_SUCCESS;

}


int32_t crm_tenant_ucm_validatemandparams(struct cm_array_of_iv_pairs *
    mand_iv_pairs_p,
    struct cm_app_result **  presult_p)
{
  uint32_t count;
  struct cm_app_result *of_tenant_result = NULL;


  CM_CBK_DEBUG_PRINT ("Entered");
  for (count = 0; count < mand_iv_pairs_p->count_ui;
      count++)
  {
    switch (mand_iv_pairs_p->iv_pairs[count].id_ui)
    {

      case CM_DM_NAMESPACE_NAME_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Tenant Name is NULL");
          fill_app_result_struct (&of_tenant_result, NULL, CM_GLU_TENANT_NAME_NULL);
          *presult_p = of_tenant_result;
          return OF_FAILURE;
        }
        break;

    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs_p);
  return OF_SUCCESS;
}



int32_t crm_tenant_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs ** pIvPairArr)
{

  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct cm_app_result *crm_tenant_result = NULL;
  struct crm_tenant_config_info   tenant_config_info={};
  struct  tenant_runtime_info   runtime_info={};
  int32_t iRes = OF_FAILURE;

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&tenant_config_info, 0, sizeof(tenant_config_info));

  if ((crm_tenant_ucm_setmandparams(key_iv_pairs_p, &tenant_config_info, &crm_tenant_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    //fill_app_result_struct (&crm_tenant_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    //*result_p = crm_tenant_result;
    return OF_FAILURE;
  }

  iRes = crm_get_tenant_by_name(tenant_config_info.tenant_name, &tenant_config_info,&runtime_info);
  if (iRes != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: Tenant doesn't exists with name %s",tenant_config_info.tenant_name);
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
  crm_tenant_ucm_getparams(&tenant_config_info, result_iv_pairs_p);
  *pIvPairArr = result_iv_pairs_p;
  return OF_SUCCESS;
}
#endif

