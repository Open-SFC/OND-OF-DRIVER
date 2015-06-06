/*
** Netwrok Service resource manager tenant source file 
* Copyright (c) 2012, 2013  Freescale.
*  
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at:
* 
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
**/

/*
 *
 * File name: chainsetcbk.c
 * Author:
 * Date: 
 * Description: 
*/
#include "cmincld.h"
#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "nsrm.h"

int32_t nsrm_chainset_init (void);

int32_t nsrm_chainset_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);

int32_t nsrm_chainset_modrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);


int32_t nsrm_chainset_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p);


int32_t nsrm_chainset_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);



int32_t nsrm_chainset_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p,  uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p);



int32_t nsrm_chainset_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs_p,
                                        struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t nsrm_chainset_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
                        uint32_t command_id, struct cm_app_result ** result_p);


int32_t nsrm_chainset_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                        struct nsrm_nschainset_object_key* appl_category_info,
                        struct nsrm_nschainset_object_config_info* nsrm_chainset_config_info);


int32_t nsrm_chainset_ucm_setoptparams (struct cm_array_of_iv_pairs *pOptParams,
                        struct nsrm_nschainset_object_config_info* nsrm_chainset_config_info);

int32_t nsrm_chainset_ucm_getparams (struct nsrm_nschainset_object_record *appl_chainset_info,
                                   struct cm_array_of_iv_pairs * result_iv_pairs_p);

int32_t nsrm_chainset_ucm_validateoptsparams (struct cm_array_of_iv_pairs *pOptParams,
                                              struct cm_app_result ** presult_p);

int32_t nsrm_chainset_ucm_validatemandparams(struct cm_array_of_iv_pairs *mand_iv_pairs_p,
                                             struct cm_app_result **  presult_p);


struct cm_dm_app_callbacks nsrm_chainset_callbacks =
{
  NULL,
  nsrm_chainset_addrec,
  nsrm_chainset_modrec,
  nsrm_chainset_delrec,
  NULL,
  nsrm_chainset_getfirstnrecs,
  nsrm_chainset_getnextnrecs,
  nsrm_chainset_getexactrec,
  nsrm_chainset_verifycfg,
  NULL
};

int32_t nsrm_chainset_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  if (cm_register_app_callbacks (CM_ON_DIRECTOR_NSRM_CHAINSET_APPL_ID, &nsrm_chainset_callbacks) != OF_SUCCESS)
    return OF_FAILURE;

  return OF_SUCCESS;
}

int32_t nsrm_chainset_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p)
{

   struct cm_app_result *nsrm_chainset_result = NULL;
   int32_t return_value = OF_FAILURE;
   struct nsrm_nschainset_object_key *nsrm_chainset_key_info;
   struct nsrm_nschainset_object_config_info *nsrm_chainset_config_info = NULL;

   CM_CBK_DEBUG_PRINT ("Entered(nsrm_chainset_addrec)");
   nsrm_chainset_key_info = (struct nsrm_nschainset_object_key*) of_calloc(1, sizeof(struct nsrm_nschainset_object_key));
  nsrm_chainset_config_info = (struct nsrm_nschainset_object_config_info *)of_calloc(NSRM_MAX_NSCHAINSET_CONFIG_PARAMETERS,sizeof(struct nsrm_nschainset_object_config_info));

  if ((nsrm_chainset_ucm_setmandparams (pMandParams, nsrm_chainset_key_info, 
                          nsrm_chainset_config_info)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&nsrm_chainset_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=nsrm_chainset_result;
    of_free(nsrm_chainset_key_info->name_p);
    of_free(nsrm_chainset_key_info->tenant_name_p);
    of_free(nsrm_chainset_key_info);
    return OF_FAILURE;
  }

  if ((nsrm_chainset_ucm_setoptparams (pOptParams, nsrm_chainset_config_info)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
    fill_app_result_struct (&nsrm_chainset_result, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
    *result_p=nsrm_chainset_result;
    of_free(nsrm_chainset_key_info->name_p);
    of_free(nsrm_chainset_key_info->tenant_name_p);
    of_free(nsrm_chainset_key_info);
    return OF_FAILURE;
  }

  return_value = nsrm_add_nschainset_object(2, nsrm_chainset_key_info,
                                         nsrm_chainset_config_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("nsrm chainset add  Failed");
    fill_app_result_struct (&nsrm_chainset_result, NULL, CM_GLU_NSCHAIN_SET_ADD_FAILED);
    *result_p = nsrm_chainset_result;
    of_free(nsrm_chainset_key_info->name_p);
    of_free(nsrm_chainset_key_info->tenant_name_p);
    of_free(nsrm_chainset_key_info);
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("NSRM chainset added successfully");
    of_free(nsrm_chainset_key_info->name_p);
    of_free(nsrm_chainset_key_info->tenant_name_p);
    of_free(nsrm_chainset_key_info);
  return OF_SUCCESS;
}

int32_t nsrm_chainset_modrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p)
{
   struct cm_app_result *nsrm_chainset_result = NULL;
   int32_t return_value = OF_FAILURE;
   struct nsrm_nschainset_object_key *nsrm_chainset_key_info;
   struct nsrm_nschainset_object_config_info *nsrm_chainset_config_info = NULL;

   CM_CBK_DEBUG_PRINT ("Entered(nsrm_category_addrec)");
  nsrm_chainset_key_info = (struct nsrm_nschainset_object_key*) of_calloc(1, sizeof(struct nsrm_nschainset_object_key));
  nsrm_chainset_config_info = (struct nsrm_nschainset_object_config_info *)of_calloc(NSRM_MAX_NSCHAINSET_CONFIG_PARAMETERS,sizeof(struct nsrm_nschainset_object_config_info));


  if ((nsrm_chainset_ucm_setmandparams (pMandParams, nsrm_chainset_key_info, nsrm_chainset_config_info)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&nsrm_chainset_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=nsrm_chainset_result;
    of_free(nsrm_chainset_key_info->name_p);
    of_free(nsrm_chainset_key_info->tenant_name_p);
    of_free(nsrm_chainset_key_info);
    return OF_FAILURE;
  }
 if ((nsrm_chainset_ucm_setoptparams (pOptParams, nsrm_chainset_config_info)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
    fill_app_result_struct (&nsrm_chainset_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=nsrm_chainset_result;
    of_free(nsrm_chainset_key_info->name_p);
    of_free(nsrm_chainset_key_info->tenant_name_p);
    of_free(nsrm_chainset_key_info);
    return OF_FAILURE;
  }

  return_value = nsrm_modify_nschainset_object(nsrm_chainset_key_info, 2, nsrm_chainset_config_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("nsrm chainset modify  Failed");
    fill_app_result_struct (&nsrm_chainset_result, NULL, CM_GLU_NSCHAIN_SET_MOD_FAILED);
    *result_p = nsrm_chainset_result;
    of_free(nsrm_chainset_key_info->name_p);
    of_free(nsrm_chainset_key_info->tenant_name_p);
    of_free(nsrm_chainset_key_info);
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("NSRM chainset modified successfully");
    of_free(nsrm_chainset_key_info->name_p);
    of_free(nsrm_chainset_key_info->tenant_name_p);
    of_free(nsrm_chainset_key_info);
  return OF_SUCCESS;
}

int32_t nsrm_chainset_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p)
{

   struct cm_app_result *nsrm_chainset_result = NULL;
  int32_t return_value = OF_FAILURE;

   struct nsrm_nschainset_object_key *nsrm_chainset_key_info;
   struct nsrm_nschainset_object_config_info *nsrm_chainset_config_info = NULL;

  CM_CBK_DEBUG_PRINT ("Entered");
  nsrm_chainset_key_info = (struct nsrm_nschainset_object_key*) of_calloc(1, sizeof(struct nsrm_nschainset_object_key));
  nsrm_chainset_config_info = (struct nsrm_nschainset_object_config_info*)of_calloc(NSRM_MAX_NSCHAINSET_CONFIG_PARAMETERS,sizeof(struct nsrm_nschainset_object_config_info));
  if ((nsrm_chainset_ucm_setmandparams (keys_arr_p, nsrm_chainset_key_info, 
                nsrm_chainset_config_info)) !=OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&nsrm_chainset_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p = nsrm_chainset_result;
    of_free(nsrm_chainset_key_info->name_p);
    of_free(nsrm_chainset_key_info->tenant_name_p);
    of_free(nsrm_chainset_key_info);
    return OF_FAILURE;
  }
  return_value = nsrm_del_nschainset_object(nsrm_chainset_key_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("nsrm chainset add  Failed");
    fill_app_result_struct (&nsrm_chainset_result, NULL, CM_GLU_NSCHAIN_SET_DEL_FAILED);
    *result_p = nsrm_chainset_result;
    of_free(nsrm_chainset_key_info->name_p);
    of_free(nsrm_chainset_key_info->tenant_name_p);
    of_free(nsrm_chainset_key_info);
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("NSRM Appliance delete successfully");
    of_free(nsrm_chainset_key_info->name_p);
    of_free(nsrm_chainset_key_info->tenant_name_p);
    of_free(nsrm_chainset_key_info);
  return OF_SUCCESS;

}

int32_t nsrm_chainset_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  int32_t uiRequestedCount = 1, uiReturnedCnt=0;
  struct nsrm_nschainset_object_record *nsrm_chainset_config_info;

  CM_CBK_DEBUG_PRINT ("Entered");
  nsrm_chainset_config_info = (struct nsrm_nschainset_object_record*)of_calloc(1,sizeof(struct nsrm_nschainset_object_record));
  nsrm_chainset_config_info->info = (struct nsrm_nschainset_object_config_info *)of_calloc(NSRM_MAX_NSCHAINSET_CONFIG_PARAMETERS,sizeof(struct nsrm_nschainset_object_config_info));

  nsrm_chainset_config_info->key.name_p = (char*)of_calloc(1,128);
  nsrm_chainset_config_info->key.tenant_name_p = (char*)of_calloc(1,128);  
 return_value = nsrm_get_first_nschainset_objects(uiRequestedCount, &uiReturnedCnt, 
              nsrm_chainset_config_info);
 if (return_value != OF_SUCCESS)
 {
   CM_CBK_DEBUG_PRINT ("Get first record failed for NSRM chainset  Table");
   return OF_FAILURE;
 }

  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
 if (result_iv_pairs_p == NULL)
 {
   CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    return OF_FAILURE;
 }
  nsrm_chainset_ucm_getparams (nsrm_chainset_config_info, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
 *array_of_iv_pair_arr_p = result_iv_pairs_p;
 *count_p = uiRecCount;
  return OF_SUCCESS;
}

int32_t nsrm_chainset_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p,  uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  int32_t uiRequestedCount = 1, uiReturnedCnt=0;
  struct nsrm_nschainset_object_key *nsrm_chainset_key_info;
  struct nsrm_nschainset_object_record *nsrm_chainset_config_info = NULL;
   struct nsrm_nschainset_object_config_info *chainset_config_info = NULL;

  CM_CBK_DEBUG_PRINT ("Entered");
  nsrm_chainset_key_info = (struct nsrm_nschainset_object_key*) of_calloc(1, sizeof(struct nsrm_nschainset_object_key));
  nsrm_chainset_config_info = (struct nsrm_nschainset_object_record*) of_calloc(NSRM_MAX_NSCHAINSET_CONFIG_PARAMETERS, sizeof(struct nsrm_nschainset_object_record));
   nsrm_chainset_config_info->info = (struct nsrm_nschainset_object_config_info *)of_calloc(NSRM_MAX_NSCHAINSET_CONFIG_PARAMETERS,sizeof(struct nsrm_nschainset_object_config_info));
   chainset_config_info = (struct nsrm_nschainset_object_config_info*) of_calloc(NSRM_MAX_NSCHAINSET_CONFIG_PARAMETERS, sizeof(struct nsrm_nschainset_object_config_info));
   


   nsrm_chainset_config_info->key.name_p = (char*)of_calloc(1,128);
   nsrm_chainset_config_info->key.tenant_name_p = (char*)of_calloc(1,128);  
 
  if ((nsrm_chainset_ucm_setmandparams (prev_record_key_p, nsrm_chainset_key_info, 
                                       chainset_config_info)) != OF_SUCCESS)
  {
     CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    of_free(nsrm_chainset_key_info->name_p);
    of_free(nsrm_chainset_key_info->tenant_name_p);
    of_free(nsrm_chainset_key_info);
     return OF_FAILURE;
  }

  return_value = nsrm_get_next_nschainset_objects(nsrm_chainset_key_info, 
                      uiRequestedCount, &uiReturnedCnt, nsrm_chainset_config_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Get next record failed for NSRM appl  Table");
    of_free(nsrm_chainset_key_info->name_p);
    of_free(nsrm_chainset_key_info->tenant_name_p);
    of_free(nsrm_chainset_key_info);
    return OF_FAILURE;
  }

  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
 if (result_iv_pairs_p == NULL)
 {
   CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    of_free(nsrm_chainset_key_info->name_p);
    of_free(nsrm_chainset_key_info->tenant_name_p);
    of_free(nsrm_chainset_key_info);
    return OF_FAILURE;
 }
  nsrm_chainset_ucm_getparams (nsrm_chainset_config_info, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
 *next_n_record_data_p = result_iv_pairs_p;
 *count_p = uiRecCount;
    of_free(nsrm_chainset_key_info->name_p);
    of_free(nsrm_chainset_key_info->tenant_name_p);
    of_free(nsrm_chainset_key_info);
  return OF_SUCCESS;
}

int32_t nsrm_chainset_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs ** pIvPairArr)

{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct nsrm_nschainset_object_key *nsrm_chainset_key_info = NULL;
  struct nsrm_nschainset_object_config_info *nsrm_chainset_config_info = NULL;
  int32_t iRes = OF_FAILURE;
  struct nsrm_nschainset_object_record *chainset_object_config_info = NULL;

  CM_CBK_DEBUG_PRINT ("Entered");
  nsrm_chainset_key_info = (struct nsrm_nschainset_object_key*) of_calloc(1, sizeof(struct nsrm_nschainset_object_key));

  nsrm_chainset_config_info = (struct nsrm_nschainset_object_config_info *)of_calloc(NSRM_MAX_NSCHAINSET_CONFIG_PARAMETERS,sizeof(struct nsrm_nschainset_object_config_info));

  chainset_object_config_info = (struct nsrm_nschainset_object_record *)of_calloc(NSRM_MAX_NSCHAINSET_CONFIG_PARAMETERS,sizeof(struct nsrm_nschainset_object_record));
  chainset_object_config_info->info = (struct nsrm_nschainset_object_config_info *)of_calloc(NSRM_MAX_NSCHAINSET_CONFIG_PARAMETERS,sizeof(struct nsrm_nschainset_object_config_info));
  chainset_object_config_info->key.name_p = (char*)of_calloc(1,128);
  chainset_object_config_info->key.tenant_name_p = (char*)of_calloc(1,128);  
  

  CM_CBK_DEBUG_PRINT ("debug1");
  CM_CBK_DEBUG_PRINT ("debug2");
  if((nsrm_chainset_ucm_setmandparams(key_iv_pairs_p, nsrm_chainset_key_info, nsrm_chainset_config_info)) !=OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    of_free(nsrm_chainset_key_info->name_p);
    of_free(nsrm_chainset_key_info->tenant_name_p);
    of_free(nsrm_chainset_key_info);
    return OF_FAILURE;
  }

  CM_CBK_DEBUG_PRINT ("debug3");
  CM_CBK_DEBUG_PRINT ("debug4");
 iRes = nsrm_get_exact_nschainset_object(nsrm_chainset_key_info, chainset_object_config_info);
  if (iRes != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: Chainset exists with name %s",nsrm_chainset_key_info->name_p);
    of_free(nsrm_chainset_key_info->name_p);
    of_free(nsrm_chainset_key_info->tenant_name_p);
    of_free(nsrm_chainset_key_info);
    return OF_FAILURE;
  }

  CM_CBK_DEBUG_PRINT ("Exact matching record found");
  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));

  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Failed to allocate memory for result_iv_pairs_p");
    of_free(nsrm_chainset_key_info->name_p);
    of_free(nsrm_chainset_key_info->tenant_name_p);
    of_free(nsrm_chainset_key_info);
    return OF_FAILURE;
  }
  nsrm_chainset_ucm_getparams(chainset_object_config_info, result_iv_pairs_p);
  *pIvPairArr = result_iv_pairs_p;
                                                                                       
    of_free(nsrm_chainset_key_info->name_p);
    of_free(nsrm_chainset_key_info->tenant_name_p);
    of_free(nsrm_chainset_key_info);
  return OF_SUCCESS;
}

int32_t nsrm_chainset_ucm_getparams (struct nsrm_nschainset_object_record *appl_chainset_info,
                                   struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
   uint32_t uindex_i = 0;
   char buf[128] = "";
   CM_CBK_DEBUG_PRINT ("Entered");
#define CM_APPLIANCE_CHILD_COUNT 4
   if (appl_chainset_info == NULL)
     return OF_FAILURE;
   result_iv_pairs_p->iv_pairs =(struct cm_iv_pair *) of_calloc (CM_APPLIANCE_CHILD_COUNT, sizeof (struct cm_iv_pair));
   if (result_iv_pairs_p->iv_pairs == NULL)
   {
     CM_CBK_DEBUG_PRINT("Memory allocation failed for result_iv_pairs_p->iv_pairs");
     return OF_FAILURE;
   }

   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_CHAINSET_NAME_ID,
		   CM_DATA_TYPE_STRING, appl_chainset_info->key.name_p);
   CM_CBK_DEBUG_PRINT("name : %s",result_iv_pairs_p->iv_pairs[uindex_i].value_p);
   uindex_i++;

   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_CHAINSET_TENANT_ID,
		   CM_DATA_TYPE_STRING, appl_chainset_info->key.tenant_name_p);
   CM_CBK_DEBUG_PRINT("name : %s",result_iv_pairs_p->iv_pairs[uindex_i].value_p);
   uindex_i++;

   
   CM_CBK_DEBUG_PRINT ("debug5");
   of_memset(buf, 0, sizeof(buf));
   if(appl_chainset_info->info[0].value.nschainset_type == L2)
   {
      sprintf(buf,"%s","L2");
   }
   else if(appl_chainset_info->info[0].value.nschainset_type == L3)
   {
      sprintf(buf,"%s","L3");
   }

   #if 0

   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_CHAINSET_TYPE_ID,
		   CM_DATA_TYPE_STRING, buf);
   uindex_i++;
   #endif


   CM_CBK_DEBUG_PRINT ("debug6");
   CM_CBK_DEBUG_PRINT ("debug7");
   of_memset(buf, 0, sizeof(buf));
   of_itoa (appl_chainset_info->info[1].value.admin_status_e, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_CHAINSET_ENABLED_ID,
		   CM_DATA_TYPE_STRING, buf);
   uindex_i++;

   result_iv_pairs_p->count_ui = uindex_i;
   CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);

   return OF_SUCCESS;
}

int32_t nsrm_chainset_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                                         struct nsrm_nschainset_object_key* appl_chainset_info,
                                         struct nsrm_nschainset_object_config_info* nsrm_chainset_config_info)
{
  uint32_t uiMandParamCnt;

  CM_CBK_DEBUG_PRINT ("Entered");
  appl_chainset_info->name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
  appl_chainset_info->tenant_name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
   for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;uiMandParamCnt++)
  {
    switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
    {
      case CM_DM_CHAINSET_NAME_ID:
      CM_CBK_DEBUG_PRINT ("name_p:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
      of_strncpy (appl_chainset_info->name_p,(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                 pMandParams->iv_pairs[uiMandParamCnt].value_length);
      break;

      case CM_DM_CHAINSET_TENANT_ID:
      CM_CBK_DEBUG_PRINT ("tenant_p:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
      of_strncpy (appl_chainset_info->tenant_name_p,(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,pMandParams->iv_pairs[uiMandParamCnt].value_length);
      break;

#if 0 
      case CM_DM_CHAINSET_TYPE_ID:
      if (strcmp((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p, "L2")==0)
        {
          nsrm_chainset_config_info[0].value.nschainset_type = L2;
        }
        else if(strcmp((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p, "L3")==0)
        {
          nsrm_chainset_config_info[0].value.nschainset_type = L3;
        }

      CM_CBK_DEBUG_PRINT ("type : %d",nsrm_chainset_config_info[0].value.nschainset_type);
      break;
#endif       
      
     }
  }
    return OF_SUCCESS;
}

int32_t nsrm_chainset_ucm_setoptparams (struct cm_array_of_iv_pairs *pOptParams,
                        struct nsrm_nschainset_object_config_info* nsrm_chainset_config_info)
{
  
  uint32_t uiOptParamCnt;
  CM_CBK_DEBUG_PRINT ("Entered");
  for (uiOptParamCnt = 0; uiOptParamCnt < pOptParams->count_ui; uiOptParamCnt++)
  {
    switch (pOptParams->iv_pairs[uiOptParamCnt].id_ui)
    {
      case CM_DM_CHAINSET_ENABLED_ID:
        nsrm_chainset_config_info[1].value.admin_status_e = 
                   of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p);
      break;
    }
   }
     CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
     return OF_SUCCESS;

}

int32_t nsrm_chainset_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
                        uint32_t command_id, struct cm_app_result ** result_p)
{
  struct cm_app_result *of_chainset_result = NULL;
  int32_t return_value = OF_FAILURE;

  CM_CBK_DEBUG_PRINT ("Entered");
  return_value = nsrm_chainset_ucm_validatemandparams(key_iv_pairs_p, &of_chainset_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  return_value = nsrm_chainset_ucm_validateoptsparams(key_iv_pairs_p, &of_chainset_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate optional  Parameters Failed");
    return OF_FAILURE;
  }
  *result_p = of_chainset_result;
  return OF_SUCCESS;

}


int32_t nsrm_chainset_ucm_validatemandparams(struct cm_array_of_iv_pairs *mand_iv_pairs_p,
    struct cm_app_result **  presult_p)
{
  uint32_t count;
  struct cm_app_result *of_chainset_result = NULL;


  CM_CBK_DEBUG_PRINT ("Entered");
  for (count = 0; count < mand_iv_pairs_p->count_ui;count++)
  {
    switch (mand_iv_pairs_p->iv_pairs[count].id_ui)
    {
      case CM_DM_CHAINSET_NAME_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Appl category name is NULL");
          fill_app_result_struct (&of_chainset_result, NULL, CM_GLU_NSCHAIN_SET_NAME_NULL);

          *presult_p = of_chainset_result;
          return OF_FAILURE;
        }
        /*if (of_strlen(mand_iv_pairs_p->iv_pairs[count].value_p) < 0 ||
            of_strlen(mand_iv_pairs_p->iv_pairs[count].value_p) > NSRM_MAX_APPLIANCE_NAME_LEN)
        {
          CM_CBK_DEBUG_PRINT ("Invalid appl category Name");
          fill_app_result_struct (&of_chainset_result, NULL, CM_GLU_APPLIANCE_NAME_INVALID);
          *presult_p = of_chainset_result;
          return OF_FAILURE;
        }*/
        break;
   }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs_p);
  return OF_SUCCESS;
}

int32_t nsrm_chainset_ucm_validateoptsparams (struct cm_array_of_iv_pairs *pOptParams,
                        struct cm_app_result ** presult_p)
{

  CM_CBK_DEBUG_PRINT ("Entered");
  CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
  return OF_SUCCESS;
}
