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
 * File name: bypassrule.c
 * Author:
 * Date: 
 * Description: 
*/
#include "cmincld.h"
#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "nsrm.h"

int32_t nsrm_bypassrule_init (void);

int32_t nsrm_bypassrule_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);

int32_t nsrm_bypassrule_modrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);


int32_t nsrm_bypassrule_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p);


int32_t nsrm_bypassrule_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);



int32_t nsrm_bypassrule_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p,  uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p);



int32_t nsrm_bypassrule_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs_p,
                                        struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t nsrm_bypassrule_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
                        uint32_t command_id, struct cm_app_result ** result_p);


int32_t nsrm_bypassrule_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                        struct nsrm_nwservice_bypass_rule_key* appl_category_info,
                                  struct nsrm_nschain_object_key* nsrm_nschain_objkey,
                        struct nsrm_nwservice_bypass_rule_config_info* nsrm_bypassrule_config_info);


int32_t nsrm_bypassrule_ucm_setoptparams (struct cm_array_of_iv_pairs *pOptParams,
                                  struct nsrm_nwservice_bypass_rule_key* appl_bypassrule_info,
                        struct nsrm_nwservice_bypass_rule_config_info* nsrm_bypassrule_config_info);

int32_t nsrm_bypassrule_ucm_getparams (struct nsrm_nwservice_bypass_rule_record *appl_bypassrule_info,
                                   struct cm_array_of_iv_pairs * result_iv_pairs_p);

int32_t nsrm_bypassrule_ucm_validatemandparams(struct cm_array_of_iv_pairs *mand_iv_pairs_p,
                                               struct cm_app_result **  presult_p);


struct cm_dm_app_callbacks nsrm_bypassrule_callbacks =
{
  NULL,
  nsrm_bypassrule_addrec,
  nsrm_bypassrule_modrec,
  nsrm_bypassrule_delrec,
  NULL,
  nsrm_bypassrule_getfirstnrecs,
  nsrm_bypassrule_getnextnrecs,
  nsrm_bypassrule_getexactrec,
  nsrm_bypassrule_verifycfg,
  NULL
};

int32_t nsrm_bypassrule_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  if (cm_register_app_callbacks (CM_ON_DIRECTOR_NSRM_CHAIN_BYPASSRULE_APPL_ID, &nsrm_bypassrule_callbacks) != OF_SUCCESS)
    return OF_FAILURE;

  return OF_SUCCESS;
}

int32_t nsrm_bypassrule_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p)
{
   struct cm_app_result *nsrm_bypassrule_result = NULL;
   int32_t return_value = OF_FAILURE;
   struct nsrm_nwservice_bypass_rule_key *nsrm_bypassrule_key_info = NULL;
   struct nsrm_nwservice_bypass_rule_config_info *nsrm_bypassrule_config_info = NULL;
   struct nsrm_nschain_object_key*     nschain_object_key_p = NULL;

   CM_CBK_DEBUG_PRINT ("Entered(nsrm_bypassrule_addrec)");
  nschain_object_key_p = (struct nsrm_nschain_object_key *) 
	  of_calloc(1, sizeof(struct nsrm_nschain_object_key));
  nsrm_bypassrule_key_info = (struct nsrm_nwservice_bypass_rule_key*) 
            of_calloc(1, sizeof(struct nsrm_nwservice_bypass_rule_key));
  nsrm_bypassrule_config_info = (struct nsrm_nwservice_bypass_rule_config_info *) 
                  of_calloc(NSRM_MAX_BYPASS_RULE_CONFIG_PARAMETERS, sizeof(struct nsrm_nwservice_bypass_rule_config_info));
  if ((nsrm_bypassrule_ucm_setmandparams (pMandParams, nsrm_bypassrule_key_info, 
                  nschain_object_key_p, nsrm_bypassrule_config_info)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&nsrm_bypassrule_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=nsrm_bypassrule_result;
    of_free(nsrm_bypassrule_key_info->name_p);
    of_free(nsrm_bypassrule_key_info->tenant_name_p);
    of_free(nsrm_bypassrule_key_info->nschain_object_name_p);
    of_free(nsrm_bypassrule_key_info);
    return OF_FAILURE;
  }

  CM_CBK_DEBUG_PRINT("debug");
  CM_CBK_DEBUG_PRINT("debug");
  if ((nsrm_bypassrule_ucm_setoptparams (pOptParams, nsrm_bypassrule_key_info,
                  nsrm_bypassrule_config_info)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
    fill_app_result_struct (&nsrm_bypassrule_result, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
    *result_p=nsrm_bypassrule_result;
    of_free(nsrm_bypassrule_key_info->name_p);
    of_free(nsrm_bypassrule_key_info->tenant_name_p);
    of_free(nsrm_bypassrule_key_info->nschain_object_name_p);
    of_free(nsrm_bypassrule_key_info);
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT("chain name : %s",nsrm_bypassrule_key_info->nschain_object_name_p);
  return_value = nsrm_add_nwservice_bypass_rule_to_nschain_object (nsrm_bypassrule_key_info,
                                         2, nsrm_bypassrule_config_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("nsrm bypassrule add  Failed");
    fill_app_result_struct (&nsrm_bypassrule_result, NULL, CM_GLU_BYPASS_RULE_ADD_FAILED);
    *result_p = nsrm_bypassrule_result;
    of_free(nsrm_bypassrule_key_info->name_p);
    of_free(nsrm_bypassrule_key_info->tenant_name_p);
    of_free(nsrm_bypassrule_key_info->nschain_object_name_p);
    of_free(nsrm_bypassrule_key_info);
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("NSRM bypassrule added successfully");
    of_free(nsrm_bypassrule_key_info->name_p);
    of_free(nsrm_bypassrule_key_info->tenant_name_p);
    of_free(nsrm_bypassrule_key_info->nschain_object_name_p);
    of_free(nsrm_bypassrule_key_info);
  return OF_SUCCESS;
}


int32_t nsrm_bypassrule_modrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p)
{
   struct cm_app_result *nsrm_bypassrule_result = NULL;
   int32_t return_value = OF_FAILURE;
   struct nsrm_nwservice_bypass_rule_key *nsrm_bypassrule_key_info = NULL;
   struct nsrm_nwservice_bypass_rule_config_info *nsrm_bypassrule_config_info = NULL;
   struct nsrm_nschain_object_key*     nschain_object_key_p = NULL;

   CM_CBK_DEBUG_PRINT ("Entered(nsrm_category_addrec)");

  nschain_object_key_p = (struct nsrm_nschain_object_key *) 
	  of_calloc(1, sizeof(struct nsrm_nschain_object_key));
  nsrm_bypassrule_key_info = (struct nsrm_nwservice_bypass_rule_key*) 
                  of_calloc(1, sizeof(struct nsrm_nwservice_bypass_rule_key));
  nsrm_bypassrule_config_info = (struct nsrm_nwservice_bypass_rule_config_info *) 
                          of_calloc(NSRM_MAX_BYPASS_RULE_CONFIG_PARAMETERS, sizeof(struct nsrm_nwservice_bypass_rule_config_info));
  if ((nsrm_bypassrule_ucm_setmandparams (pMandParams, nsrm_bypassrule_key_info,
                       nschain_object_key_p, nsrm_bypassrule_config_info)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&nsrm_bypassrule_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=nsrm_bypassrule_result;
    of_free(nsrm_bypassrule_key_info->name_p);
    of_free(nsrm_bypassrule_key_info->tenant_name_p);
    of_free(nsrm_bypassrule_key_info->nschain_object_name_p);
    of_free(nsrm_bypassrule_key_info);
    return OF_FAILURE;
  }
 if ((nsrm_bypassrule_ucm_setoptparams (pOptParams, nsrm_bypassrule_key_info,
                  nsrm_bypassrule_config_info)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
    fill_app_result_struct (&nsrm_bypassrule_result, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
    *result_p=nsrm_bypassrule_result;
    of_free(nsrm_bypassrule_key_info->name_p);
    of_free(nsrm_bypassrule_key_info->tenant_name_p);
    of_free(nsrm_bypassrule_key_info->nschain_object_name_p);
    of_free(nsrm_bypassrule_key_info);
    return OF_FAILURE;
  }

  return_value = nsrm_modify_nwservice_bypass_rule (nsrm_bypassrule_key_info, 2, 
               nsrm_bypassrule_config_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("nsrm bypassrule modify  Failed");
    fill_app_result_struct (&nsrm_bypassrule_result, NULL, CM_GLU_BYPASS_RULE_MOD_FAILED);
    *result_p = nsrm_bypassrule_result;
    of_free(nsrm_bypassrule_key_info->name_p);
    of_free(nsrm_bypassrule_key_info->tenant_name_p);
    of_free(nsrm_bypassrule_key_info->nschain_object_name_p);
    of_free(nsrm_bypassrule_key_info);
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("NSRM bypassrule modified successfully");
    of_free(nsrm_bypassrule_key_info->name_p);
    of_free(nsrm_bypassrule_key_info->tenant_name_p);
    of_free(nsrm_bypassrule_key_info->nschain_object_name_p);
    of_free(nsrm_bypassrule_key_info);
  return OF_SUCCESS;
}

int32_t nsrm_bypassrule_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p)
{

   struct cm_app_result *nsrm_bypassrule_result = NULL;
  int32_t return_value = OF_FAILURE;
   struct nsrm_nwservice_bypass_rule_key *nsrm_bypassrule_key_info;
   struct nsrm_nwservice_bypass_rule_config_info *nsrm_bypassrule_config_info;
  struct nsrm_nschain_object_key*     nschain_object_key_p;

  CM_CBK_DEBUG_PRINT ("Entered");
  nschain_object_key_p = (struct nsrm_nschain_object_key *) 
	  of_calloc(1, sizeof(struct nsrm_nschain_object_key));
  nsrm_bypassrule_key_info = (struct nsrm_nwservice_bypass_rule_key*) 
                        of_calloc(1, sizeof(struct nsrm_nwservice_bypass_rule_key));
  nsrm_bypassrule_config_info = (struct nsrm_nwservice_bypass_rule_config_info *) 
                          of_calloc(1, sizeof(struct nsrm_nwservice_bypass_rule_config_info));
  if ((nsrm_bypassrule_ucm_setmandparams (keys_arr_p, nsrm_bypassrule_key_info,
                     nschain_object_key_p, nsrm_bypassrule_config_info)) !=OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&nsrm_bypassrule_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p = nsrm_bypassrule_result;
    of_free(nsrm_bypassrule_key_info->name_p);
    of_free(nsrm_bypassrule_key_info->tenant_name_p);
    of_free(nsrm_bypassrule_key_info->nschain_object_name_p);
    of_free(nsrm_bypassrule_key_info);
    return OF_FAILURE;
  }
  return_value = nsrm_del_nwservice_bypass_rule_from_nschain_object (nsrm_bypassrule_key_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("nsrm bypassrule add  Failed");
    fill_app_result_struct (&nsrm_bypassrule_result, NULL, CM_GLU_BYPASS_RULE_DEL_FAILED);
    *result_p = nsrm_bypassrule_result;
    of_free(nsrm_bypassrule_key_info->name_p);
    of_free(nsrm_bypassrule_key_info->tenant_name_p);
    of_free(nsrm_bypassrule_key_info->nschain_object_name_p);
    of_free(nsrm_bypassrule_key_info);
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("NSRM Appliance delete successfully");
    of_free(nsrm_bypassrule_key_info->name_p);
    of_free(nsrm_bypassrule_key_info->tenant_name_p);
    of_free(nsrm_bypassrule_key_info->nschain_object_name_p);
    of_free(nsrm_bypassrule_key_info);
  return OF_SUCCESS;

}

int32_t nsrm_bypassrule_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  int32_t uiRequestedCount = 1, uiReturnedCnt=0;
  struct nsrm_nwservice_bypass_rule_key *nsrm_bypassrule_key_info = NULL;
  struct nsrm_nwservice_bypass_rule_record *nsrm_bypassrule_config_info = NULL;
  struct   nsrm_nwservice_bypass_rule_config_info*   nsrm_bypass_config_info = NULL;
  struct nsrm_nschain_object_key*     nschain_object_key_p = NULL;

  CM_CBK_DEBUG_PRINT ("Entered");
  nschain_object_key_p = (struct nsrm_nschain_object_key *) 
	  of_calloc(1, sizeof(struct nsrm_nschain_object_key));
  nsrm_bypassrule_key_info = (struct nsrm_nwservice_bypass_rule_key*) of_calloc(1, 
                   sizeof(struct nsrm_nwservice_bypass_rule_key));
  nsrm_bypass_config_info = (struct nsrm_nwservice_bypass_rule_config_info *) 
                          of_calloc(1, sizeof(struct nsrm_nwservice_bypass_rule_config_info));

  nsrm_bypassrule_config_info = (struct nsrm_nwservice_bypass_rule_record *) of_calloc(NSRM_MAX_BYPASS_RULE_CONFIG_PARAMETERS, sizeof(struct nsrm_nwservice_bypass_rule_record));

  nsrm_bypassrule_config_info->key.name_p = (char *) of_calloc(1,NSRM_MAX_NAME_LENGTH);
  nsrm_bypassrule_config_info->key.tenant_name_p = (char *) of_calloc(1,NSRM_MAX_NAME_LENGTH);
  nsrm_bypassrule_config_info->key.nschain_object_name_p = (char *) of_calloc(1,NSRM_MAX_NAME_LENGTH);

  nsrm_bypassrule_config_info->info = (struct nsrm_nwservice_bypass_rule_config_info *)of_calloc(NSRM_MAX_BYPASS_RULE_CONFIG_PARAMETERS,sizeof(struct nsrm_nwservice_bypass_rule_config_info));
  nsrm_bypassrule_config_info->info[8].value.relative_name_p =(char *) of_calloc(1,NSRM_MAX_NAME_LENGTH);


  if ((nsrm_bypassrule_ucm_setmandparams (keys_arr_p, nsrm_bypassrule_key_info, 
                                    nschain_object_key_p, nsrm_bypass_config_info)) != OF_SUCCESS)
  { 
     CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
     of_free(nsrm_bypassrule_key_info->name_p);
     of_free(nsrm_bypassrule_key_info->tenant_name_p);
     of_free(nsrm_bypassrule_key_info->nschain_object_name_p);
     of_free(nsrm_bypassrule_key_info);
     return OF_FAILURE;
  }
  
  CM_CBK_DEBUG_PRINT ("debug");
  CM_CBK_DEBUG_PRINT ("debug");
 
 return_value = nsrm_get_first_bypass_rules_from_nschain_object (nschain_object_key_p, uiRequestedCount, 
                 &uiReturnedCnt, nsrm_bypassrule_config_info);
 if (return_value != OF_SUCCESS)
 {
   CM_CBK_DEBUG_PRINT ("Get first record failed for NSRM bypassrule  Table");
   return OF_FAILURE;
 }

  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
 if (result_iv_pairs_p == NULL)
 {
   CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    return OF_FAILURE;
 }
  nsrm_bypassrule_ucm_getparams (nsrm_bypassrule_config_info, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
 *array_of_iv_pair_arr_p = result_iv_pairs_p;
  return OF_SUCCESS;
}


int32_t nsrm_bypassrule_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p,  uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  int32_t uiRequestedCount = 1, uiReturnedCnt=0;
  struct nsrm_nwservice_bypass_rule_key *nsrm_bypassrule_key_info;
  struct nsrm_nwservice_bypass_rule_record *nsrm_bypassrule_config_info;
  struct   nsrm_nwservice_bypass_rule_config_info*   nsrm_bypass_config_info;
  struct nsrm_nschain_object_key*     nschain_object_key_p;

  CM_CBK_DEBUG_PRINT ("Entered");
  nschain_object_key_p = (struct nsrm_nschain_object_key *) 
	  of_calloc(1, sizeof(struct nsrm_nschain_object_key));
  nsrm_bypassrule_key_info = (struct nsrm_nwservice_bypass_rule_key*) of_calloc(1, 
                   sizeof(struct nsrm_nwservice_bypass_rule_key));
  nsrm_bypass_config_info = (struct nsrm_nwservice_bypass_rule_config_info *) 
                          of_calloc(NSRM_MAX_BYPASS_RULE_CONFIG_PARAMETERS, sizeof(struct nsrm_nwservice_bypass_rule_config_info));

  nsrm_bypassrule_config_info = (struct nsrm_nwservice_bypass_rule_record *) of_calloc(1, sizeof(struct nsrm_nwservice_bypass_rule_record));
   nsrm_bypassrule_config_info->info = (struct nsrm_nwservice_bypass_rule_config_info *)of_calloc(NSRM_MAX_BYPASS_RULE_CONFIG_PARAMETERS,sizeof(struct nsrm_nwservice_bypass_rule_config_info));
  nsrm_bypassrule_config_info->key.name_p = (char *) of_calloc(1,NSRM_MAX_NAME_LENGTH);
  nsrm_bypassrule_config_info->key.tenant_name_p = (char *) of_calloc(1,NSRM_MAX_NAME_LENGTH);
  nsrm_bypassrule_config_info->key.nschain_object_name_p = (char *) of_calloc(1,NSRM_MAX_NAME_LENGTH);
 nsrm_bypassrule_config_info->info[8].value.relative_name_p = (char *) of_calloc(1,NSRM_MAX_NAME_LENGTH);

  nsrm_bypassrule_ucm_setmandparams (keys_arr_p, nsrm_bypassrule_key_info,
                                    nschain_object_key_p, nsrm_bypass_config_info);

  if ((nsrm_bypassrule_ucm_setmandparams (prev_record_key_p, nsrm_bypassrule_key_info, 
                                    nschain_object_key_p, nsrm_bypass_config_info)) != OF_SUCCESS)
  {
     CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
     of_free(nsrm_bypassrule_key_info->name_p);
     of_free(nsrm_bypassrule_key_info->tenant_name_p);
     of_free(nsrm_bypassrule_key_info->nschain_object_name_p);
     of_free(nsrm_bypassrule_key_info);
     return OF_FAILURE;
  }


  return_value = nsrm_get_next_bypass_rules_from_nschain_object (nsrm_bypassrule_key_info, 
                                                                 nschain_object_key_p,
                                                                 uiRequestedCount, &uiReturnedCnt, 
                                                                 nsrm_bypassrule_config_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Get next record failed for NSRM appl  Table");
    of_free(nsrm_bypassrule_key_info->name_p);
    of_free(nsrm_bypassrule_key_info->tenant_name_p);
    of_free(nsrm_bypassrule_key_info->nschain_object_name_p);
    of_free(nsrm_bypassrule_key_info);
    return OF_FAILURE;
  }

  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    of_free(nsrm_bypassrule_key_info->name_p);
    of_free(nsrm_bypassrule_key_info->tenant_name_p);
    of_free(nsrm_bypassrule_key_info->nschain_object_name_p);
    of_free(nsrm_bypassrule_key_info);
    return OF_FAILURE;
  }
  nsrm_bypassrule_ucm_getparams (nsrm_bypassrule_config_info, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *next_n_record_data_p = result_iv_pairs_p;
  *count_p = uiRecCount;
  of_free(nsrm_bypassrule_key_info->name_p);
  of_free(nsrm_bypassrule_key_info->tenant_name_p);
  of_free(nsrm_bypassrule_key_info->nschain_object_name_p);
  of_free(nsrm_bypassrule_key_info);
  return OF_SUCCESS;
}

int32_t nsrm_bypassrule_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs ** pIvPairArr)

{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct nsrm_nwservice_bypass_rule_key *nsrm_bypassrule_key_info;
  uint32_t uiRecCount = 0;
  int32_t iRes = OF_FAILURE;
  struct nsrm_nwservice_bypass_rule_record *bypassrule_object_config_info;
  struct nsrm_nwservice_bypass_rule_config_info*   nsrm_bypass_config_info;
  struct nsrm_nschain_object_key*     nschain_object_key_p;

  CM_CBK_DEBUG_PRINT ("Entered");
  nsrm_bypassrule_key_info = (struct nsrm_nwservice_bypass_rule_key*) 
          of_calloc(1, sizeof(struct nsrm_nwservice_bypass_rule_key));
  nsrm_bypass_config_info = (struct nsrm_nwservice_bypass_rule_config_info *) 
	  of_calloc(NSRM_MAX_BYPASS_RULE_CONFIG_PARAMETERS, sizeof(struct nsrm_nwservice_bypass_rule_config_info));
  nschain_object_key_p = (struct nsrm_nschain_object_key *) 
	  of_calloc(1, sizeof(struct nsrm_nschain_object_key));

  bypassrule_object_config_info = (struct nsrm_nwservice_bypass_rule_record *) of_calloc(1, sizeof(struct nsrm_nwservice_bypass_rule_record));
  bypassrule_object_config_info->info = (struct nsrm_nwservice_bypass_rule_config_info*)of_calloc(NSRM_MAX_BYPASS_RULE_CONFIG_PARAMETERS,sizeof(struct nsrm_nwservice_bypass_rule_config_info));
  bypassrule_object_config_info->key.name_p = (char *) of_calloc(1,NSRM_MAX_NAME_LENGTH);
  bypassrule_object_config_info->key.tenant_name_p = (char *) of_calloc(1,NSRM_MAX_NAME_LENGTH);
  bypassrule_object_config_info->key.nschain_object_name_p = (char *) of_calloc(1,NSRM_MAX_NAME_LENGTH);
  bypassrule_object_config_info->info[8].value.relative_name_p = (char *) of_calloc(1,NSRM_MAX_NAME_LENGTH);
  if((nsrm_bypassrule_ucm_setmandparams(key_iv_pairs_p, nsrm_bypassrule_key_info, 
                   nschain_object_key_p, nsrm_bypass_config_info)) !=OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    of_free(nsrm_bypassrule_key_info->name_p);
    of_free(nsrm_bypassrule_key_info->tenant_name_p);
    of_free(nsrm_bypassrule_key_info->nschain_object_name_p);
    of_free(nsrm_bypassrule_key_info);
    return OF_FAILURE;
  }

  iRes = nsrm_get_exact_bypass_rule_from_nschain_object (nschain_object_key_p, nsrm_bypassrule_key_info, 
                                                         bypassrule_object_config_info);
  if (iRes != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: Appliacne doesn't exists with name %s",nsrm_bypassrule_key_info->name_p);
    of_free(nsrm_bypassrule_key_info->name_p);
    of_free(nsrm_bypassrule_key_info->tenant_name_p);
    of_free(nsrm_bypassrule_key_info->nschain_object_name_p);
    of_free(nsrm_bypassrule_key_info);
    return OF_FAILURE;
  }

  CM_CBK_DEBUG_PRINT ("Exact matching record found");
  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));

  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Failed to allocate memory for result_iv_pairs_p");
    of_free(nsrm_bypassrule_key_info->name_p);
    of_free(nsrm_bypassrule_key_info->tenant_name_p);
    of_free(nsrm_bypassrule_key_info->nschain_object_name_p);
    of_free(nsrm_bypassrule_key_info);
    return OF_FAILURE;
  }
  nsrm_bypassrule_ucm_getparams(bypassrule_object_config_info, &result_iv_pairs_p[uiRecCount]);
  *pIvPairArr = result_iv_pairs_p;
                                                                                       
    of_free(nsrm_bypassrule_key_info->name_p);
    of_free(nsrm_bypassrule_key_info->tenant_name_p);
    of_free(nsrm_bypassrule_key_info->nschain_object_name_p);
    of_free(nsrm_bypassrule_key_info);
  return OF_SUCCESS;
}

int32_t nsrm_bypassrule_ucm_getparams (struct nsrm_nwservice_bypass_rule_record *appl_bypassrule_info,
                                   struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
   uint32_t uindex_i = 0;
   char buf[128] = "";
   CM_CBK_DEBUG_PRINT ("Entered");
#define CM_BYPASS_CHILD_COUNT 40

   result_iv_pairs_p->iv_pairs =(struct cm_iv_pair *) of_calloc (CM_BYPASS_CHILD_COUNT, sizeof (struct cm_iv_pair));
   if (result_iv_pairs_p->iv_pairs == NULL)
   {
     CM_CBK_DEBUG_PRINT("Memory allocation failed for result_iv_pairs_p->iv_pairs");
     return OF_FAILURE;
   }

   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_BYPASSRULE_BYPASSNAME_ID,
		   CM_DATA_TYPE_STRING, appl_bypassrule_info->key.name_p);
   CM_CBK_DEBUG_PRINT("name : %s",result_iv_pairs_p->iv_pairs[uindex_i].value_p);
   uindex_i++;

  
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_BYPASSRULE_RELATIVE_NAME_ID,
		   CM_DATA_TYPE_STRING, appl_bypassrule_info->info[8].value.relative_name_p);
   CM_CBK_DEBUG_PRINT("relative_name_p : %s",result_iv_pairs_p->iv_pairs[uindex_i].value_p);
   uindex_i++;


   of_memset(buf, 0, sizeof(buf));
   if (appl_bypassrule_info->key.location_e == FIRST )
   {
      sprintf(buf,"%s","FIRST");
   }
   else if(appl_bypassrule_info->key.location_e == LAST)
   {
      sprintf(buf,"%s","LAST");
   }
   else if (appl_bypassrule_info->key.location_e == BEFORE )
   {
      sprintf(buf,"%s","BEFORE");
   }
   else if(appl_bypassrule_info->key.location_e == AFTER)
   {
      sprintf(buf,"%s","AFTER");
   }
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_BYPASSRULE_LOCATION_ID,
                    CM_DATA_TYPE_STRING,buf);

   uindex_i++;

   if (appl_bypassrule_info->info[0].value.src_mac.mac_address_type_e == MAC_ANY)
   {
      sprintf(buf,"%s","Any");
   }
   else if (appl_bypassrule_info->info[0].value.src_mac.mac_address_type_e == MAC_SINGLE)
   {
      sprintf(buf,"%s","Single");
   }
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_BYPASSRULE_SRCMACTYPE_ID,CM_DATA_TYPE_STRING,buf);
   uindex_i++;


   if (appl_bypassrule_info->info[0].value.dst_mac.mac_address_type_e == MAC_ANY)
   {
      sprintf(buf,"%s","Any");
   }
   else if (appl_bypassrule_info->info[0].value.dst_mac.mac_address_type_e == MAC_SINGLE)
   {
      sprintf(buf,"%s","Single");
   }
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_BYPASSRULE_DSTMACTYPE_ID,CM_DATA_TYPE_STRING,buf);
   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   if (appl_bypassrule_info->info[2].value.eth_type.ethtype_type_e == ETHTYPE_ANY)
   {
      sprintf(buf,"%s","Any");
   }
   else if (appl_bypassrule_info->info[2].value.eth_type.ethtype_type_e == ETHTYPE_SINGLE)
   {
      sprintf(buf,"%s","Single");
   }

   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_BYPASSRULE_ETHTYPE_ID,
                                   CM_DATA_TYPE_STRING,buf);

   uindex_i++;
   of_memset(buf, 0, sizeof(buf));
   if (appl_bypassrule_info->info[5].value.sp.port_object_type_e == PORT_ANY )
   {
      sprintf(buf,"%s","Any");
   }
   else if (appl_bypassrule_info->info[5].value.sp.port_object_type_e == PORT_SINGLE )
   {
      sprintf(buf,"%s","Single");
   }
   else if (appl_bypassrule_info->info[5].value.sp.port_object_type_e == PORT_RANGE )
   {
      sprintf(buf,"%s","Range");
   }
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_BYPASSRULE_SPTYPE_ID,CM_DATA_TYPE_STRING,buf);

   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   if (appl_bypassrule_info->info[6].value.dp.port_object_type_e == PORT_ANY )
   {
      sprintf(buf,"%s","Any");
   }
   else if (appl_bypassrule_info->info[6].value.dp.port_object_type_e == PORT_SINGLE )
   {
      sprintf(buf,"%s","Single");
   }
   else if (appl_bypassrule_info->info[6].value.dp.port_object_type_e == PORT_RANGE )
   {
      sprintf(buf,"%s","Range");
   }
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_BYPASSRULE_DPTYPE_ID,CM_DATA_TYPE_STRING,buf);

   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   if (appl_bypassrule_info->info[3].value.sip.ip_object_type_e == IP_ANY )
   {
     sprintf(buf,"%s","Any");
   }
   else if (appl_bypassrule_info->info[3].value.sip.ip_object_type_e == IP_SINGLE )
   {
     sprintf(buf,"%s","Single");
   }
   else if (appl_bypassrule_info->info[3].value.sip.ip_object_type_e == IP_RANGE )
   {
     sprintf(buf,"%s","Range");
   }
   else if (appl_bypassrule_info->info[3].value.sip.ip_object_type_e == IP_SUBNET )
   {
     sprintf(buf,"%s","Subnet");
   }

   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_BYPASSRULE_SIPTYPE_ID,CM_DATA_TYPE_STRING,buf);

   uindex_i++;


   of_memset(buf, 0, sizeof(buf));
   if (appl_bypassrule_info->info[4].value.dip.ip_object_type_e == IP_ANY )
   {
     sprintf(buf,"%s","Any");
   }
   else if (appl_bypassrule_info->info[4].value.dip.ip_object_type_e == IP_SINGLE )
   {
     sprintf(buf,"%s","Single");
   }
   else if (appl_bypassrule_info->info[4].value.dip.ip_object_type_e == IP_RANGE )
   {
     sprintf(buf,"%s","Range");
   }
   else if (appl_bypassrule_info->info[4].value.dip.ip_object_type_e == IP_SUBNET )
   {
     sprintf(buf,"%s","Subnet");
   }

   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_BYPASSRULE_SIPTYPE_ID,CM_DATA_TYPE_STRING,buf);

   uindex_i++;




   of_memset(buf, 0, sizeof(buf));
   of_itoa (appl_bypassrule_info->info[7].value.ip_protocol, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_BYPASSRULE_PROTOCOL_ID,
                    CM_DATA_TYPE_STRING,buf);
   uindex_i++;


   of_memset(buf, 0, sizeof(buf));
   of_itoa (appl_bypassrule_info->info[0].value.src_mac.mac, buf);
   sprintf(buf,"%02x:%02x:%02x:%02x:%02x:%02x",appl_bypassrule_info->info[0].value.src_mac.mac[0],appl_bypassrule_info->info[0].value.src_mac.mac[1],appl_bypassrule_info->info[0].value.src_mac.mac[2],appl_bypassrule_info->info[0].value.src_mac.mac[3],appl_bypassrule_info->info[0].value.src_mac.mac[4],appl_bypassrule_info->info[0].value.src_mac.mac[5]);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_BYPASSRULE_SRCMAC_ID,
                    CM_DATA_TYPE_STRING,buf);
   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   of_itoa (appl_bypassrule_info->info[1].value.dst_mac.mac, buf);
   sprintf(buf,"%02x:%02x:%02x:%02x:%02x:%02x",appl_bypassrule_info->info[1].value.dst_mac.mac[0],appl_bypassrule_info->info[1].value.dst_mac.mac[1],appl_bypassrule_info->info[1].value.dst_mac.mac[2],appl_bypassrule_info->info[1].value.dst_mac.mac[3],appl_bypassrule_info->info[1].value.dst_mac.mac[4],appl_bypassrule_info->info[1].value.dst_mac.mac[5]);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_BYPASSRULE_DSTMAC_ID,
                    CM_DATA_TYPE_STRING,buf);
   uindex_i++;


   
   of_memset(buf, 0, sizeof(buf));
   of_itoa (appl_bypassrule_info->info[2].value.eth_type.ethtype, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_BYPASSRULE_ETHTYPEVALUE_ID,
                    CM_DATA_TYPE_STRING,buf);
   uindex_i++;


   of_memset(buf, 0, sizeof(buf));
   of_itoa (appl_bypassrule_info->info[5].value.sp.port_start, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_BYPASSRULE_SPSTART_ID,
                    CM_DATA_TYPE_STRING,buf);
   uindex_i++;
   
    of_memset(buf, 0, sizeof(buf));
   of_itoa (appl_bypassrule_info->info[5].value.sp.port_end, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_BYPASSRULE_SPEND_ID,
                    CM_DATA_TYPE_STRING,buf);
   uindex_i++;


   of_memset(buf, 0, sizeof(buf));
   of_itoa (appl_bypassrule_info->info[6].value.dp.port_end, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_BYPASSRULE_DPEND_ID,
                    CM_DATA_TYPE_STRING,buf);
   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   of_itoa (appl_bypassrule_info->info[6].value.dp.port_start, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_BYPASSRULE_DPSTART_ID,
                    CM_DATA_TYPE_STRING,buf);
   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   cm_inet_ntoal (appl_bypassrule_info->info[3].value.sip.ipaddr_start, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_BYPASSRULE_SIPSTART_ID,
                    CM_DATA_TYPE_STRING,buf);
   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   cm_inet_ntoal(appl_bypassrule_info->info[3].value.sip.ipaddr_end, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_BYPASSRULE_SIPEND_ID,
                    CM_DATA_TYPE_STRING,buf);
   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   cm_inet_ntoal(appl_bypassrule_info->info[4].value.dip.ipaddr_start, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_BYPASSRULE_DIPSTART_ID,
                    CM_DATA_TYPE_STRING,buf);
   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   cm_inet_ntoal(appl_bypassrule_info->info[4].value.dip.ipaddr_end, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_BYPASSRULE_DIPEND_ID,
                    CM_DATA_TYPE_STRING,buf);
   uindex_i++;
 

   of_memset(buf, 0, sizeof(buf));
   of_itoa (appl_bypassrule_info->info[9].value.admin_status_e, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_BYPASSRULE_ENABLED_ID,
		   CM_DATA_TYPE_STRING, buf);
   uindex_i++;

   result_iv_pairs_p->count_ui = uindex_i;
   CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);

   return OF_SUCCESS;

}

int32_t nsrm_bypassrule_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                                  struct nsrm_nwservice_bypass_rule_key* appl_bypassrule_info,
                                  struct nsrm_nschain_object_key* nsrm_nschain_objkey,
                                  struct nsrm_nwservice_bypass_rule_config_info* nsrm_bypassrule_config_info)
{
  uint32_t uiMandParamCnt;   

  CM_CBK_DEBUG_PRINT ("Entered");

  for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;uiMandParamCnt++)
  {
    switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
    {
      case CM_DM_BYPASSRULE_BYPASSNAME_ID:
         appl_bypassrule_info->name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
          CM_CBK_DEBUG_PRINT ("name_p:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
          CM_CBK_DEBUG_PRINT ("debug");
          of_strncpy (appl_bypassrule_info->name_p,(char *)pMandParams->iv_pairs[uiMandParamCnt].value_p,
                 pMandParams->iv_pairs[uiMandParamCnt].value_length);
      break;

     // case CM_DM_BYPASSRULE_BYPASSTENANT_ID:
       case CM_DM_CHAIN_TENANT_ID:
          appl_bypassrule_info->tenant_name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
          of_strncpy (appl_bypassrule_info->tenant_name_p,(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                 pMandParams->iv_pairs[uiMandParamCnt].value_length);
	  CM_CBK_DEBUG_PRINT ("tenant name_p:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
          CM_CBK_DEBUG_PRINT ("debug");
      break;

  //     case CM_DM_BYPASSRULE_BYPASS_CHAIN_ID:
       case CM_DM_CHAIN_NAME_ID :
          appl_bypassrule_info->nschain_object_name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
          nsrm_nschain_objkey->name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
          CM_CBK_DEBUG_PRINT ("chainname_p:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
          of_strncpy (appl_bypassrule_info->nschain_object_name_p,
                 (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                 pMandParams->iv_pairs[uiMandParamCnt].value_length);
          of_strncpy (nsrm_nschain_objkey->name_p,
                 (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                 pMandParams->iv_pairs[uiMandParamCnt].value_length);
          CM_CBK_DEBUG_PRINT ("chain name : %s",nsrm_nschain_objkey->name_p);
          CM_CBK_DEBUG_PRINT ("chain name : %s",appl_bypassrule_info->nschain_object_name_p);
          CM_CBK_DEBUG_PRINT ("debug");
      break;

      case CM_DM_BYPASSRULE_LOCATION_ID:
        if (strcmp(pMandParams->iv_pairs[uiMandParamCnt].value_p, "FIRST") ==0 )
        {
          appl_bypassrule_info->location_e = FIRST;
        }
        else if (strcmp(pMandParams->iv_pairs[uiMandParamCnt].value_p, "LAST") ==0 )
        {
          appl_bypassrule_info->location_e = LAST;
        }
        else if(strcmp(pMandParams->iv_pairs[uiMandParamCnt].value_p, "BEFORE") ==0 )
        {
          appl_bypassrule_info->location_e = BEFORE;
        }
        else if(strcmp(pMandParams->iv_pairs[uiMandParamCnt].value_p, "AFTER") ==0 )
        {
          appl_bypassrule_info->location_e = AFTER;
        }

      break;
     }
  }
  return OF_SUCCESS;
}

int32_t nsrm_bypassrule_ucm_setoptparams (struct cm_array_of_iv_pairs *pOptParams,
                                  struct nsrm_nwservice_bypass_rule_key* appl_bypassrule_info,
                                  struct nsrm_nwservice_bypass_rule_config_info* nsrm_bypassrule_config_info)
{
  
  uint32_t uiOptParamCnt;
  char    *src_mac;
  char    *dst_mac;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (uiOptParamCnt = 0; uiOptParamCnt < pOptParams->count_ui; uiOptParamCnt++)
  {
    switch (pOptParams->iv_pairs[uiOptParamCnt].id_ui)
    {
      case CM_DM_BYPASSRULE_RELATIVE_NAME_ID:
              nsrm_bypassrule_config_info[8].value.relative_name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
              CM_CBK_DEBUG_PRINT("rel name : %s",pOptParams->iv_pairs[uiOptParamCnt].value_p);
              of_strncpy (nsrm_bypassrule_config_info[8].value.relative_name_p,
                         (char *) pOptParams->iv_pairs[uiOptParamCnt].value_p,
                         pOptParams->iv_pairs[uiOptParamCnt].value_length);
              CM_CBK_DEBUG_PRINT("rel name : %s",nsrm_bypassrule_config_info[8].value.relative_name_p);
          break;

      case CM_DM_BYPASSRULE_ENABLED_ID:
        nsrm_bypassrule_config_info[9].value.admin_status_e = of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p);
      break;

      case CM_DM_BYPASSRULE_PROTOCOL_ID:
        nsrm_bypassrule_config_info[7].value.ip_protocol = of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p);
      break;

      case CM_DM_BYPASSRULE_SRCMACTYPE_ID:
        if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Any") ==0 )
        {
          nsrm_bypassrule_config_info[0].value.src_mac.mac_address_type_e = MAC_ANY;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Single") ==0 )
        {
          nsrm_bypassrule_config_info[0].value.src_mac.mac_address_type_e = MAC_SINGLE;
        }
      break;

      case CM_DM_BYPASSRULE_ETHTYPE_ID:
        if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Any") ==0 )
        {
          nsrm_bypassrule_config_info[2].value.eth_type.ethtype_type_e = ETHTYPE_ANY;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Single") ==0 )
        {
          nsrm_bypassrule_config_info[2].value.eth_type.ethtype_type_e = ETHTYPE_SINGLE;
        }
      break;

      case CM_DM_BYPASSRULE_DSTMACTYPE_ID:
        if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Any") ==0 )
        {
          nsrm_bypassrule_config_info[1].value.dst_mac.mac_address_type_e = MAC_ANY;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Single") ==0 )
        {
          nsrm_bypassrule_config_info[1].value.dst_mac.mac_address_type_e = MAC_SINGLE;
        }
      break;

      case CM_DM_BYPASSRULE_SRCMAC_ID:
           CM_CBK_DEBUG_PRINT ("src_mac_address:%s",(char *) pOptParams->iv_pairs[uiOptParamCnt].value_p);
           src_mac = (char *) pOptParams->iv_pairs[uiOptParamCnt].value_p;
           if ((*src_mac == '0') &&
            ((*(src_mac+1) == 'x') || (*(src_mac+1) == 'X')))
           {
              of_flow_match_atox_mac_addr(src_mac+2, nsrm_bypassrule_config_info[0].value.src_mac.mac);
           }
           else
           {
              of_flow_match_atox_mac_addr(src_mac, nsrm_bypassrule_config_info[0].value.src_mac.mac);
           }

           CM_CBK_DEBUG_PRINT ("src_mac_address:%s",nsrm_bypassrule_config_info[0].value.src_mac.mac);

      break;

      case CM_DM_BYPASSRULE_DSTMAC_ID:
          CM_CBK_DEBUG_PRINT ("dst_mac_address:%s",(char *) pOptParams->iv_pairs[uiOptParamCnt].value_p);
           dst_mac = (char *) pOptParams->iv_pairs[uiOptParamCnt].value_p;
           if ((*dst_mac == '0') &&
            ((*(dst_mac+1) == 'x') || (*(dst_mac+1) == 'X')))
           {
              of_flow_match_atox_mac_addr(dst_mac+2, nsrm_bypassrule_config_info[1].value.dst_mac.mac);
           }
           else
           {
              of_flow_match_atox_mac_addr(dst_mac, nsrm_bypassrule_config_info[1].value.dst_mac.mac);
           }

           CM_CBK_DEBUG_PRINT ("dst_mac_address:%s",nsrm_bypassrule_config_info[1].value.dst_mac.mac);

      break;

      case CM_DM_BYPASSRULE_ETHTYPEVALUE_ID:
          nsrm_bypassrule_config_info[2].value.eth_type.ethtype = of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p);
      break;

      case CM_DM_BYPASSRULE_SPTYPE_ID:
        if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Any") ==0 )
        {
          nsrm_bypassrule_config_info[5].value.sp.port_object_type_e = PORT_ANY;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Single") ==0 )
        {
          nsrm_bypassrule_config_info[5].value.sp.port_object_type_e = PORT_SINGLE;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Range") ==0 )
        {
          nsrm_bypassrule_config_info[5].value.sp.port_object_type_e = PORT_RANGE;
        }
      break;

      case CM_DM_BYPASSRULE_DPTYPE_ID:
        if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Any") ==0 )
        {
          nsrm_bypassrule_config_info[6].value.dp.port_object_type_e = PORT_ANY;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Single") ==0 )
        {
          nsrm_bypassrule_config_info[6].value.dp.port_object_type_e = PORT_SINGLE;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Range") ==0 )
        {
          nsrm_bypassrule_config_info[6].value.dp.port_object_type_e = PORT_RANGE;
        }
      break;

      case CM_DM_BYPASSRULE_SPSTART_ID:
          nsrm_bypassrule_config_info[5].value.sp.port_start = of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p);
      break;

      case CM_DM_BYPASSRULE_SPEND_ID:
          nsrm_bypassrule_config_info[5].value.sp.port_end = of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p);
      break;

      case CM_DM_BYPASSRULE_DPEND_ID:
          nsrm_bypassrule_config_info[6].value.dp.port_end = of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p);
      break;

      case CM_DM_BYPASSRULE_DPSTART_ID:
          nsrm_bypassrule_config_info[6].value.dp.port_start = of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p);
      break;

      case CM_DM_BYPASSRULE_SIPTYPE_ID:
        if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Any") ==0 )
        {
          nsrm_bypassrule_config_info[3].value.sip.ip_object_type_e = IP_ANY;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Single") ==0 )
        {
          nsrm_bypassrule_config_info[3].value.sip.ip_object_type_e = IP_SINGLE;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Range") ==0 )
        {
          nsrm_bypassrule_config_info[3].value.sip.ip_object_type_e = IP_RANGE;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Subnet") ==0 )
        {
          nsrm_bypassrule_config_info[3].value.sip.ip_object_type_e = IP_SUBNET;
        }
      break;

      case CM_DM_BYPASSRULE_DIPTYPE_ID:
        if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Any") ==0 )
        {
          nsrm_bypassrule_config_info[4].value.dip.ip_object_type_e = IP_ANY;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Single") ==0 )
        {
          nsrm_bypassrule_config_info[4].value.dip.ip_object_type_e = IP_SINGLE;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Range") ==0 )
        {
          nsrm_bypassrule_config_info[4].value.dip.ip_object_type_e = IP_RANGE;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Subnet") ==0 )
        {
          nsrm_bypassrule_config_info[4].value.dip.ip_object_type_e = IP_SUBNET;
        }
      break;

      case CM_DM_BYPASSRULE_SIPSTART_ID:
        if(cm_val_and_conv_aton((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p, &nsrm_bypassrule_config_info[3].value.sip.ipaddr_start) != OF_SUCCESS)
            return OF_FAILURE;
      break;

      case CM_DM_BYPASSRULE_SIPEND_ID:
        if(cm_val_and_conv_aton((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p, &nsrm_bypassrule_config_info[3].value.sip.ipaddr_end) != OF_SUCCESS)
            return OF_FAILURE;

      break;

      case CM_DM_BYPASSRULE_DIPSTART_ID:
        if(cm_val_and_conv_aton((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p, &nsrm_bypassrule_config_info[4].value.dip.ipaddr_start) != OF_SUCCESS)
            return OF_FAILURE;

      break;

      case CM_DM_BYPASSRULE_DIPEND_ID:
      if(cm_val_and_conv_aton((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p, &nsrm_bypassrule_config_info[4].value.dip.ipaddr_end) != OF_SUCCESS)
            return OF_FAILURE;
      break;

    }
   }
     CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
     return OF_SUCCESS;

}


int32_t nsrm_bypassrule_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
                        uint32_t command_id, struct cm_app_result ** result_p)
{
  struct cm_app_result *of_bypassrule_result = NULL;
  int32_t return_value = OF_FAILURE;

  CM_CBK_DEBUG_PRINT ("Entered");
  return_value = nsrm_bypassrule_ucm_validatemandparams(key_iv_pairs_p, &of_bypassrule_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    return OF_FAILURE;
  }
  *result_p = of_bypassrule_result;
  return OF_SUCCESS;

}


int32_t nsrm_bypassrule_ucm_validatemandparams(struct cm_array_of_iv_pairs *mand_iv_pairs_p,
    struct cm_app_result **  presult_p)
{
  uint32_t count;
  struct cm_app_result *of_bypassrule_result = NULL;


  CM_CBK_DEBUG_PRINT ("Entered");
  for (count = 0; count < mand_iv_pairs_p->count_ui;count++)
  {
    switch (mand_iv_pairs_p->iv_pairs[count].id_ui)
    {
      case CM_DM_BYPASSRULE_BYPASSNAME_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Appl category name is NULL");
          fill_app_result_struct (&of_bypassrule_result, NULL, CM_GLU_BYPASS_RULE_NAME_NULL);
          *presult_p = of_bypassrule_result;
          return OF_FAILURE;
        }
        break;
     }
   }
  CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs_p);
  return OF_SUCCESS;
}

