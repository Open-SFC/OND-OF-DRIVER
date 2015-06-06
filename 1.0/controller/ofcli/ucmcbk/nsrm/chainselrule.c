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
 * File name: chainselrule.c
 * Author:
 * Date: 
 * Description: 
*/
#include "cmincld.h"
#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "nsrm.h"

int32_t nsrm_chainselrule_init (void);

int32_t nsrm_chainselrule_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);

int32_t nsrm_chainselrule_modrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);


int32_t nsrm_chainselrule_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p);


int32_t nsrm_chainselrule_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);



int32_t nsrm_chainselrule_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p,  uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p);



int32_t nsrm_chainselrule_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs_p,
                                        struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t nsrm_chainselrule_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
                        uint32_t command_id, struct cm_app_result ** result_p);


int32_t nsrm_chainselrule_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                        struct nsrm_nschainset_object_key *nshcainset_object_key_p,
                        struct nsrm_nschain_selection_rule_key* appl_category_info,
                        struct nsrm_nschain_selection_rule_config_info* nsrm_chainselrule_config_info);


int32_t nsrm_chainselrule_ucm_setoptparams (struct cm_array_of_iv_pairs *pOptParams,
                                  struct nsrm_nschain_selection_rule_key* appl_chainselrule_info,
                        struct nsrm_nschain_selection_rule_config_info* nsrm_chainselrule_config_info);

int32_t nsrm_chainselrule_ucm_getparams (struct nsrm_nschain_selection_rule_record *appl_chainselrule_info,
                                   struct cm_array_of_iv_pairs * result_iv_pairs_p);

int32_t nsrm_chainselrule_ucm_validatemandparams(struct cm_array_of_iv_pairs *mand_iv_pairs_p,
                                                 struct cm_app_result **  presult_p);


struct cm_dm_app_callbacks nsrm_chainselrule_callbacks =
{
  NULL,
  nsrm_chainselrule_addrec,
  nsrm_chainselrule_modrec,
  nsrm_chainselrule_delrec,
  NULL,
  nsrm_chainselrule_getfirstnrecs,
  nsrm_chainselrule_getnextnrecs,
  nsrm_chainselrule_getexactrec,
  nsrm_chainselrule_verifycfg,
  NULL
};

int32_t nsrm_chainselrule_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  if (cm_register_app_callbacks (CM_ON_DIRECTOR_NSRM_CHAINSET_CHAINSELRULE_APPL_ID, 
               &nsrm_chainselrule_callbacks) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Failed to initialize callbacks");
    return OF_FAILURE;
  }

  return OF_SUCCESS;
}

int32_t nsrm_chainselrule_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p)
{
   struct cm_app_result *nsrm_chainselrule_result = NULL;
   int32_t return_value = OF_FAILURE;
   struct nsrm_nschain_selection_rule_key *nsrm_chainselrule_key_info;
   struct nsrm_nschain_selection_rule_config_info *nsrm_chainselrule_config_info;
   struct   nsrm_nschainset_object_key*     nschainset_object_key_p;
   
   CM_CBK_DEBUG_PRINT ("Entered(nsrm_chainselrule_addrec)");
   nschainset_object_key_p = (struct nsrm_nschainset_object_key *)of_calloc(1,sizeof(struct nsrm_nschainset_object_key));
   nsrm_chainselrule_key_info = (struct nsrm_nschain_selection_rule_key*) 
                    of_calloc(1, sizeof(struct nsrm_nschain_selection_rule_key));
   nsrm_chainselrule_config_info = (struct nsrm_nschain_selection_rule_config_info *) 
                    of_calloc(NSRM_MAX_SELECTION_RULE_CONFIG_PARAMETERS, sizeof(struct nsrm_nschain_selection_rule_config_info));
   if ((nsrm_chainselrule_ucm_setmandparams (pMandParams,nschainset_object_key_p, nsrm_chainselrule_key_info, 
                  nsrm_chainselrule_config_info)) != OF_SUCCESS)
   {
     CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
     fill_app_result_struct (&nsrm_chainselrule_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
     *result_p=nsrm_chainselrule_result;
     of_free(nsrm_chainselrule_key_info->name_p);
     of_free(nsrm_chainselrule_key_info->tenant_name_p);
     of_free(nsrm_chainselrule_key_info->nschainset_object_name_p);
     of_free(nsrm_chainselrule_key_info);
     return OF_FAILURE;
   }

  CM_CBK_DEBUG_PRINT("after setmand");
  CM_CBK_DEBUG_PRINT("after setmand");
  if ((nsrm_chainselrule_ucm_setoptparams (pOptParams, nsrm_chainselrule_key_info,
                  nsrm_chainselrule_config_info)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
    fill_app_result_struct (&nsrm_chainselrule_result, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
    *result_p=nsrm_chainselrule_result;
    of_free(nsrm_chainselrule_key_info->name_p);
    of_free(nsrm_chainselrule_key_info->tenant_name_p);
    of_free(nsrm_chainselrule_key_info->nschainset_object_name_p);
    of_free(nsrm_chainselrule_key_info);
    return OF_FAILURE;
  }

  CM_CBK_DEBUG_PRINT("debug");
  CM_CBK_DEBUG_PRINT("debug");
  CM_CBK_DEBUG_PRINT("name : %s",nsrm_chainselrule_key_info->name_p);
  return_value = nsrm_add_nschain_selection_rule_to_nschainset (nsrm_chainselrule_key_info,
                                         2, nsrm_chainselrule_config_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("nsrm chainselrule add  Failed");
    fill_app_result_struct (&nsrm_chainselrule_result, NULL, CM_GLU_SELECTION_RULE_ADD_FAILED);
    *result_p = nsrm_chainselrule_result;
    of_free(nsrm_chainselrule_key_info->name_p);
    of_free(nsrm_chainselrule_key_info->tenant_name_p);
    of_free(nsrm_chainselrule_key_info->nschainset_object_name_p);
    of_free(nsrm_chainselrule_key_info);
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT("debug");
  CM_CBK_DEBUG_PRINT("debug");
  CM_CBK_DEBUG_PRINT ("NSRM chainselrule added successfully");
    of_free(nsrm_chainselrule_key_info->name_p);
    of_free(nsrm_chainselrule_key_info->tenant_name_p);
    of_free(nsrm_chainselrule_key_info->nschainset_object_name_p);
    of_free(nsrm_chainselrule_key_info);
  return OF_SUCCESS;
}

int32_t nsrm_chainselrule_modrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p)
{
   struct cm_app_result *nsrm_chainselrule_result = NULL;
   int32_t return_value = OF_FAILURE;
   struct nsrm_nschain_selection_rule_key *nsrm_chainselrule_key_info = NULL;
   struct nsrm_nschain_selection_rule_config_info *nsrm_chainselrule_config_info = NULL;
   struct   nsrm_nschainset_object_key*     nschainset_object_key_p = NULL;
   
   nschainset_object_key_p = (struct nsrm_nschainset_object_key *)of_calloc(1,sizeof(struct nsrm_nschainset_object_key));

   CM_CBK_DEBUG_PRINT ("Entered(nsrm_category_addrec)");

  nsrm_chainselrule_key_info = (struct nsrm_nschain_selection_rule_key*) 
                         of_calloc(1, sizeof(struct nsrm_nschain_selection_rule_key));
  nsrm_chainselrule_config_info = (struct nsrm_nschain_selection_rule_config_info *) 
                          of_calloc(NSRM_MAX_SELECTION_RULE_CONFIG_PARAMETERS, sizeof(struct nsrm_nschain_selection_rule_config_info));
  if ((nsrm_chainselrule_ucm_setmandparams (pMandParams,nschainset_object_key_p, nsrm_chainselrule_key_info,
                       nsrm_chainselrule_config_info)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&nsrm_chainselrule_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=nsrm_chainselrule_result;
    of_free(nsrm_chainselrule_key_info->name_p);
    of_free(nsrm_chainselrule_key_info->tenant_name_p);
    of_free(nsrm_chainselrule_key_info->nschainset_object_name_p);
    of_free(nsrm_chainselrule_key_info);
    return OF_FAILURE;
  }
 if ((nsrm_chainselrule_ucm_setoptparams (pOptParams, nsrm_chainselrule_key_info,
                  nsrm_chainselrule_config_info)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
    fill_app_result_struct (&nsrm_chainselrule_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=nsrm_chainselrule_result;
    of_free(nsrm_chainselrule_key_info->name_p);
    of_free(nsrm_chainselrule_key_info->tenant_name_p);
    of_free(nsrm_chainselrule_key_info->nschainset_object_name_p);
    of_free(nsrm_chainselrule_key_info);
    return OF_FAILURE;
  }

  return_value = nsrm_modify_nschain_selection_rule (nsrm_chainselrule_key_info, 2, 
               nsrm_chainselrule_config_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("nsrm chainselrule modify  Failed");
    fill_app_result_struct (&nsrm_chainselrule_result, NULL, CM_GLU_SELECTION_RULE_MOD_FAILED);
    *result_p = nsrm_chainselrule_result;
    of_free(nsrm_chainselrule_key_info->name_p);
    of_free(nsrm_chainselrule_key_info->tenant_name_p);
    of_free(nsrm_chainselrule_key_info->nschainset_object_name_p);
    of_free(nsrm_chainselrule_key_info);
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("NSRM chainselrule modified successfully");
    of_free(nsrm_chainselrule_key_info->name_p);
    of_free(nsrm_chainselrule_key_info->tenant_name_p);
    of_free(nsrm_chainselrule_key_info->nschainset_object_name_p);
    of_free(nsrm_chainselrule_key_info);
  return OF_SUCCESS;
}

int32_t nsrm_chainselrule_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p)
{

   struct cm_app_result *nsrm_chainselrule_result = NULL;
  int32_t return_value = OF_FAILURE;
   struct nsrm_nschain_selection_rule_key *nsrm_chainselrule_key_info;
   struct nsrm_nschain_selection_rule_config_info *nsrm_chainselrule_config_info;
   struct   nsrm_nschainset_object_key*     nschainset_object_key_p;

   nschainset_object_key_p = (struct nsrm_nschainset_object_key *)of_calloc(1,sizeof(struct nsrm_nschainset_object_key));


  CM_CBK_DEBUG_PRINT ("Entered");
  nsrm_chainselrule_key_info = (struct nsrm_nschain_selection_rule_key*) of_calloc(1, sizeof(struct nsrm_nschain_selection_rule_key));
  nsrm_chainselrule_config_info = (struct nsrm_nschain_selection_rule_config_info *) 
                          of_calloc(1, sizeof(struct nsrm_nschain_selection_rule_config_info));
  if ((nsrm_chainselrule_ucm_setmandparams (keys_arr_p,nschainset_object_key_p, nsrm_chainselrule_key_info,
                     nsrm_chainselrule_config_info)) !=OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&nsrm_chainselrule_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p = nsrm_chainselrule_result;
    of_free(nsrm_chainselrule_key_info->name_p);
    of_free(nsrm_chainselrule_key_info->tenant_name_p);
    of_free(nsrm_chainselrule_key_info->nschainset_object_name_p);
    of_free(nsrm_chainselrule_key_info);
    return OF_FAILURE;
  }
  return_value = nsrm_del_nschain_selection_rule_from_nschainset(nsrm_chainselrule_key_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("nsrm chainselrule add  Failed");
    fill_app_result_struct (&nsrm_chainselrule_result, NULL, CM_GLU_SELECTION_RULE_DEL_FAILED);
    *result_p = nsrm_chainselrule_result;
    of_free(nsrm_chainselrule_key_info->name_p);
    of_free(nsrm_chainselrule_key_info->tenant_name_p);
    of_free(nsrm_chainselrule_key_info->nschainset_object_name_p);
    of_free(nsrm_chainselrule_key_info);
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("NSRM Appliance delete successfully");
    of_free(nsrm_chainselrule_key_info->name_p);
    of_free(nsrm_chainselrule_key_info->tenant_name_p);
    of_free(nsrm_chainselrule_key_info->nschainset_object_name_p);
    of_free(nsrm_chainselrule_key_info);
  return OF_SUCCESS;

}

int32_t nsrm_chainselrule_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  int32_t uiRequestedCount = 1, uiReturnedCnt=0;
  struct nsrm_nschain_selection_rule_record *nsrm_chainselrule_config_info;
  struct nsrm_nschain_selection_rule_key *nsrm_chainselrule_key_info;
  struct   nsrm_nschain_selection_rule_config_info*   nsrm_srvmap_config_info;
  struct   nsrm_nschainset_object_key*     nschainset_object_key_p;

  CM_CBK_DEBUG_PRINT ("Entered");
  nschainset_object_key_p = (struct nsrm_nschainset_object_key *) 
          of_calloc(1, sizeof(struct nsrm_nschainset_object_key));
  nsrm_chainselrule_key_info = (struct nsrm_nschain_selection_rule_key*) of_calloc(1, 
                   sizeof(struct nsrm_nschain_selection_rule_key));
  nsrm_srvmap_config_info = (struct nsrm_nschain_selection_rule_config_info *) 
                          of_calloc(NSRM_MAX_SELECTION_RULE_CONFIG_PARAMETERS, sizeof(struct nsrm_nschain_selection_rule_config_info));

  if ((nsrm_chainselrule_ucm_setmandparams (keys_arr_p,nschainset_object_key_p,
                                            nsrm_chainselrule_key_info, 
                                            nsrm_srvmap_config_info)) != OF_SUCCESS)
  {
     CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
     of_free(nsrm_chainselrule_key_info->name_p);
     of_free(nsrm_chainselrule_key_info->tenant_name_p);
     of_free(nsrm_chainselrule_key_info->nschainset_object_name_p);
     of_free(nsrm_chainselrule_key_info);
     return OF_FAILURE;
  }

  CM_CBK_DEBUG_PRINT("debug");
  CM_CBK_DEBUG_PRINT("tenant name : %s",nsrm_chainselrule_key_info->tenant_name_p);
  CM_CBK_DEBUG_PRINT("chainset name : %s",nsrm_chainselrule_key_info->nschainset_object_name_p);
  CM_CBK_DEBUG_PRINT("debug");
  CM_CBK_DEBUG_PRINT("debug");
  if ((nsrm_chainselrule_key_info->nschainset_object_name_p == NULL) ||
        (nschainset_object_key_p->name_p == NULL))
  {
     CM_CBK_DEBUG_PRINT ("Parent table key is missing, not proceed further");
     of_free(nsrm_chainselrule_key_info->name_p);
     of_free(nsrm_chainselrule_key_info->tenant_name_p);
     of_free(nsrm_chainselrule_key_info->nschainset_object_name_p);
     of_free(nsrm_chainselrule_key_info);
     return OF_FAILURE;
  }
  nsrm_chainselrule_config_info = (struct nsrm_nschain_selection_rule_record *)of_calloc(1,sizeof(struct nsrm_nschain_selection_rule_record));

  nsrm_chainselrule_config_info->info = (struct nsrm_nschain_selection_rule_config_info *)
                          of_calloc(NSRM_MAX_SELECTION_RULE_CONFIG_PARAMETERS, sizeof(struct nsrm_nschain_selection_rule_config_info));
  nsrm_chainselrule_config_info->info[8].value.nschain_object_name_p = (char *) of_calloc(1,NSRM_MAX_NAME_LENGTH);
  nsrm_chainselrule_config_info->info[9].value.relative_name_p = (char *) of_calloc(1,NSRM_MAX_NAME_LENGTH);
  nsrm_chainselrule_config_info->key.name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH); 
  nsrm_chainselrule_config_info->key.tenant_name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH); 
  nsrm_chainselrule_config_info->key.nschainset_object_name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH); 
 
  return_value = nsrm_get_first_selection_rules_from_nschainset_object(nschainset_object_key_p,
                   uiRequestedCount, &uiReturnedCnt, nsrm_chainselrule_config_info);
  CM_CBK_DEBUG_PRINT("debug");
  CM_CBK_DEBUG_PRINT("debug");
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Get first record failed for NSRM chainselrule  Table");
    return OF_FAILURE;
  }

  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc(1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
     return OF_FAILURE;
  }
  nsrm_chainselrule_ucm_getparams (nsrm_chainselrule_config_info, &result_iv_pairs_p[uiRecCount]);
  CM_CBK_DEBUG_PRINT("debug");
  CM_CBK_DEBUG_PRINT("debug");
  uiRecCount++;
  *array_of_iv_pair_arr_p = result_iv_pairs_p;
  return OF_SUCCESS;
}


int32_t nsrm_chainselrule_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p,  uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  int32_t uiRequestedCount = 1, uiReturnedCnt=0;
  struct nsrm_nschain_selection_rule_key *nsrm_chainselrule_key_info = NULL;
  struct nsrm_nschain_selection_rule_record *nsrm_chainselrule_config_info = NULL;
  struct   nsrm_nschain_selection_rule_config_info*   nsrm_srvmap_config_info = NULL;
  struct   nsrm_nschainset_object_key*     nschainset_object_key_p;

  CM_CBK_DEBUG_PRINT ("Entered");
  nschainset_object_key_p = (struct nsrm_nschainset_object_key*) 
          of_calloc(1, sizeof(struct nsrm_nschainset_object_key));
  nsrm_chainselrule_key_info = (struct nsrm_nschain_selection_rule_key*) of_calloc(1, 
                   sizeof(struct nsrm_nschain_selection_rule_key));
  nsrm_srvmap_config_info = (struct nsrm_nschain_selection_rule_config_info *) 
                          of_calloc(1, sizeof(struct nsrm_nschain_selection_rule_config_info));
  nsrm_chainselrule_ucm_setmandparams (keys_arr_p, nschainset_object_key_p,nsrm_chainselrule_key_info, 
                                         nsrm_srvmap_config_info);
  if ((nsrm_chainselrule_ucm_setmandparams (prev_record_key_p,nschainset_object_key_p, nsrm_chainselrule_key_info, 
                                       nsrm_srvmap_config_info)) != OF_SUCCESS)
  {
     CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
     of_free(nsrm_chainselrule_key_info->name_p);
     of_free(nsrm_chainselrule_key_info->tenant_name_p);
     of_free(nsrm_chainselrule_key_info->nschainset_object_name_p);
     of_free(nsrm_chainselrule_key_info);
     return OF_FAILURE;
  }

  nsrm_chainselrule_config_info = (struct nsrm_nschain_selection_rule_record *)of_calloc(1,sizeof(struct nsrm_nschain_selection_rule_record));

  nsrm_chainselrule_config_info->info = (struct nsrm_nschain_selection_rule_config_info *)
                          of_calloc(NSRM_MAX_SELECTION_RULE_CONFIG_PARAMETERS, sizeof(struct nsrm_nschain_selection_rule_config_info));
  nsrm_chainselrule_config_info->info[8].value.nschain_object_name_p = (char *) of_calloc(1,NSRM_MAX_NAME_LENGTH);
  nsrm_chainselrule_config_info->info[9].value.relative_name_p = (char *) of_calloc(1,NSRM_MAX_NAME_LENGTH);
  nsrm_chainselrule_config_info->key.name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH);
  nsrm_chainselrule_config_info->key.tenant_name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH);
  nsrm_chainselrule_config_info->key.nschainset_object_name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH);

  return_value = nsrm_get_next_selection_rules_from_nschainset_object (nschainset_object_key_p,
                        nsrm_chainselrule_key_info, uiRequestedCount, &uiReturnedCnt, 
                        nsrm_chainselrule_config_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Get next record failed for NSRM appl  Table");
    of_free(nsrm_chainselrule_key_info->name_p);
    of_free(nsrm_chainselrule_key_info->tenant_name_p);
    of_free(nsrm_chainselrule_key_info->nschainset_object_name_p);
    of_free(nsrm_chainselrule_key_info);
    return OF_FAILURE;
  }

  result_iv_pairs_p =(struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    of_free(nsrm_chainselrule_key_info->name_p);
    of_free(nsrm_chainselrule_key_info->tenant_name_p);
    of_free(nsrm_chainselrule_key_info->nschainset_object_name_p);
    of_free(nsrm_chainselrule_key_info);
    return OF_FAILURE;
  }
  nsrm_chainselrule_ucm_getparams (nsrm_chainselrule_config_info, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *next_n_record_data_p = result_iv_pairs_p;
  *count_p = uiRecCount;
  of_free(nsrm_chainselrule_key_info->name_p);
  of_free(nsrm_chainselrule_key_info->tenant_name_p);
  of_free(nsrm_chainselrule_key_info->nschainset_object_name_p);
  of_free(nsrm_chainselrule_key_info);
  return OF_SUCCESS;
}

int32_t nsrm_chainselrule_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs ** pIvPairArr)

{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct nsrm_nschain_selection_rule_key *nsrm_chainselrule_key_info;
  uint32_t uiRecCount = 0;
  int32_t iRes = OF_FAILURE;
  struct nsrm_nschain_selection_rule_record *chainselrule_object_config_info;
  struct nsrm_nschain_selection_rule_config_info*   nsrm_srvmap_config_info;
  struct   nsrm_nschainset_object_key*     nschainset_object_key_p;

  CM_CBK_DEBUG_PRINT ("Entered");
  nschainset_object_key_p = (struct nsrm_nschainset_object_key*) 
          of_calloc(1, sizeof(struct nsrm_nschainset_object_key));
  nsrm_chainselrule_key_info = (struct nsrm_nschain_selection_rule_key*) 
          of_calloc(1, sizeof(struct nsrm_nschain_selection_rule_key));
  nsrm_srvmap_config_info = (struct nsrm_nschain_selection_rule_config_info *) 
	  of_calloc(NSRM_MAX_SELECTION_RULE_CONFIG_PARAMETERS, sizeof(struct nsrm_nschain_selection_rule_config_info));
  if((nsrm_chainselrule_ucm_setmandparams(key_iv_pairs_p,nschainset_object_key_p, nsrm_chainselrule_key_info, 
                   nsrm_srvmap_config_info)) !=OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    of_free(nsrm_chainselrule_key_info->name_p);
    of_free(nsrm_chainselrule_key_info->tenant_name_p);
    of_free(nsrm_chainselrule_key_info->nschainset_object_name_p);
    of_free(nsrm_chainselrule_key_info);
    return OF_FAILURE;
  }
 
 
  CM_CBK_DEBUG_PRINT("debug");
  CM_CBK_DEBUG_PRINT("debug");

   chainselrule_object_config_info = (struct nsrm_nschain_selection_rule_record *)of_calloc(1,sizeof(struct nsrm_nschain_selection_rule_record));
    chainselrule_object_config_info->key.name_p = (char *) of_calloc(1,NSRM_MAX_NAME_LENGTH);
    chainselrule_object_config_info->key.tenant_name_p = (char *) of_calloc(1,NSRM_MAX_NAME_LENGTH);
    chainselrule_object_config_info->key.nschainset_object_name_p = (char *) of_calloc(1,NSRM_MAX_NAME_LENGTH);
    chainselrule_object_config_info->info = (struct nsrm_nschain_selection_rule_config_info*) of_calloc(NSRM_MAX_SELECTION_RULE_CONFIG_PARAMETERS,sizeof(struct nsrm_nschain_selection_rule_config_info));
    chainselrule_object_config_info->info[8].value.nschain_object_name_p = (char *) of_calloc(1,NSRM_MAX_NAME_LENGTH);
    chainselrule_object_config_info->info[9].value.relative_name_p = (char *) of_calloc(1,NSRM_MAX_NAME_LENGTH);
  

  CM_CBK_DEBUG_PRINT("debug");
  CM_CBK_DEBUG_PRINT("debug");
  iRes = nsrm_get_exact_selection_rule_from_nschainset_object (nschainset_object_key_p,
                       nsrm_chainselrule_key_info, chainselrule_object_config_info);
  CM_CBK_DEBUG_PRINT("debug");
  CM_CBK_DEBUG_PRINT("debug");
  if (iRes != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: Appliacne doesn't exists with name %s",nsrm_chainselrule_key_info->name_p);
    of_free(nsrm_chainselrule_key_info->name_p);
    of_free(nsrm_chainselrule_key_info->tenant_name_p);
    of_free(nsrm_chainselrule_key_info->nschainset_object_name_p);
    of_free(nsrm_chainselrule_key_info);
    return OF_FAILURE;
  }

  CM_CBK_DEBUG_PRINT ("Exact matching record found");
  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));

  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Failed to allocate memory for result_iv_pairs_p");
    of_free(nsrm_chainselrule_key_info->name_p);
    of_free(nsrm_chainselrule_key_info->tenant_name_p);
    of_free(nsrm_chainselrule_key_info->nschainset_object_name_p);
    of_free(nsrm_chainselrule_key_info);
    return OF_FAILURE;
  }
  nsrm_chainselrule_ucm_getparams(chainselrule_object_config_info, &result_iv_pairs_p[uiRecCount]);
  *pIvPairArr = result_iv_pairs_p;
                                                                                       
    of_free(nsrm_chainselrule_key_info->name_p);
    of_free(nsrm_chainselrule_key_info->tenant_name_p);
    of_free(nsrm_chainselrule_key_info->nschainset_object_name_p);
    of_free(nsrm_chainselrule_key_info);
  return OF_SUCCESS;
}

int32_t nsrm_chainselrule_ucm_getparams (struct nsrm_nschain_selection_rule_record *appl_chainselrule_info,
                                   struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
   uint32_t uindex_i = 0;
   char buf[128] = "";
   CM_CBK_DEBUG_PRINT ("Entered");
#define CM_APPLIANCE_CHILD_COUNT 25

   result_iv_pairs_p->iv_pairs =(struct cm_iv_pair *) of_calloc (CM_APPLIANCE_CHILD_COUNT, sizeof (struct cm_iv_pair));
   if (result_iv_pairs_p->iv_pairs == NULL)
   {
     CM_CBK_DEBUG_PRINT("Memory allocation failed for result_iv_pairs_p->iv_pairs");
     return OF_FAILURE;
   }

   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_CHAINSELRULE_SELNAME_ID,
		   CM_DATA_TYPE_STRING, appl_chainselrule_info->key.name_p);
   CM_CBK_DEBUG_PRINT("name : %s",result_iv_pairs_p->iv_pairs[uindex_i].value_p);
   uindex_i++;

  /*
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_CHAINSELRULE_SELTENANT_ID,
		   CM_DATA_TYPE_STRING, appl_chainselrule_info->key.tenant_name_p);
   CM_CBK_DEBUG_PRINT("tenant_name_p : %s",result_iv_pairs_p->iv_pairs[uindex_i].value_p);
   uindex_i++;
*/
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_CHAINSELRULE_CHAIN_ID,
		   CM_DATA_TYPE_STRING, appl_chainselrule_info->info[8].value.nschain_object_name_p);
   CM_CBK_DEBUG_PRINT("nschain_object_name_p: %s",result_iv_pairs_p->iv_pairs[uindex_i].value_p);
   uindex_i++;

   of_memset(buf, 0, sizeof(buf)); 
   if (appl_chainselrule_info->key.location_e == FIRST )
   {
      sprintf(buf,"%s","FIRST");
   }
   else if(appl_chainselrule_info->key.location_e == LAST)
   {
      sprintf(buf,"%s","LAST");
   }
   else if (appl_chainselrule_info->key.location_e == BEFORE )
   {
      sprintf(buf,"%s","BEFORE");
   }
   else if(appl_chainselrule_info->key.location_e == AFTER)
   {
      sprintf(buf,"%s","AFTER");
   }
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_CHAINSELRULE_LOCATION_ID,                                  CM_DATA_TYPE_STRING,buf);

   uindex_i++;

  CM_CBK_DEBUG_PRINT("debug");
   
     FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_CHAINSELRULE_RELATIVE_NAME_ID,
                   CM_DATA_TYPE_STRING, appl_chainselrule_info->info[9].value.relative_name_p);
     CM_CBK_DEBUG_PRINT("relative_name_p: %s",result_iv_pairs_p->iv_pairs[uindex_i].value_p);
     uindex_i++;
   

  /*
   else 
   {
     FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_CHAINSELRULE_RELATIVE_NAME_ID,
		   CM_DATA_TYPE_STRING, appl_chainselrule_info->info[9].value.relative_name_p);
     CM_CBK_DEBUG_PRINT("relative_name_p: %s",result_iv_pairs_p->iv_pairs[uindex_i].value_p);
     uindex_i++;
   }
*/
  

   /*FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_CHAINSELRULE_CHAINSET_NAME_ID,
		   CM_DATA_TYPE_STRING, appl_chainselrule_info->key.nschainset_object_name_p);
   CM_CBK_DEBUG_PRINT("nschainset_name_p: %s",result_iv_pairs_p->iv_pairs[uindex_i].value_p);
   uindex_i++;*/

   

   of_memset(buf, 0, sizeof(buf));
   of_itoa (appl_chainselrule_info->info[7].value.ip_protocol, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_CHAINSELRULE_PROTOCOL_ID,
                    CM_DATA_TYPE_STRING,buf);
   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   of_itoa (appl_chainselrule_info->info[0].value.src_mac.mac, buf);
   sprintf(buf,"%02x:%02x:%02x:%02x:%02x:%02x",appl_chainselrule_info->info[0].value.src_mac.mac[0],appl_chainselrule_info->info[0].value.src_mac.mac[1],appl_chainselrule_info->info[0].value.src_mac.mac[2],appl_chainselrule_info->info[0].value.src_mac.mac[3],appl_chainselrule_info->info[0].value.src_mac.mac[4],appl_chainselrule_info->info[0].value.src_mac.mac[5]);

   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_CHAINSELRULE_SRCMAC_ID,
                    CM_DATA_TYPE_STRING,buf);
   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   of_itoa (appl_chainselrule_info->info[1].value.dst_mac.mac, buf);
   sprintf(buf,"%02x:%02x:%02x:%02x:%02x:%02x",appl_chainselrule_info->info[1].value.dst_mac.mac[0],appl_chainselrule_info->info[1].value.dst_mac.mac[1],appl_chainselrule_info->info[1].value.dst_mac.mac[2],appl_chainselrule_info->info[1].value.dst_mac.mac[3],appl_chainselrule_info->info[1].value.dst_mac.mac[4],appl_chainselrule_info->info[1].value.dst_mac.mac[5]);

   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_CHAINSELRULE_DSTMAC_ID,
                    CM_DATA_TYPE_STRING,buf);
   uindex_i++;


   

   of_memset(buf, 0, sizeof(buf));
   of_itoa (appl_chainselrule_info->info[2].value.eth_type.ethtype, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_CHAINSELRULE_ETHTYPEVALUE_ID,
                    CM_DATA_TYPE_STRING,buf);
   uindex_i++;

   
   of_memset(buf, 0, sizeof(buf));
   of_itoa (appl_chainselrule_info->info[5].value.sp.port_start, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_CHAINSELRULE_SPSTART_ID,
                    CM_DATA_TYPE_STRING,buf);
   uindex_i++;


   of_memset(buf, 0, sizeof(buf));
   of_itoa (appl_chainselrule_info->info[5].value.sp.port_end, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_CHAINSELRULE_SPEND_ID,
                    CM_DATA_TYPE_STRING,buf);
   uindex_i++;


   of_memset(buf, 0, sizeof(buf));
   of_itoa (appl_chainselrule_info->info[6].value.dp.port_end, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_CHAINSELRULE_DPEND_ID,
                    CM_DATA_TYPE_STRING,buf);
   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   of_itoa (appl_chainselrule_info->info[6].value.dp.port_start, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_CHAINSELRULE_DPSTART_ID,
                    CM_DATA_TYPE_STRING,buf);
   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   cm_inet_ntoal(appl_chainselrule_info->info[3].value.sip.ipaddr_start, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_CHAINSELRULE_SIPSTART_ID,
                    CM_DATA_TYPE_STRING,buf);
   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   cm_inet_ntoal(appl_chainselrule_info->info[3].value.sip.ipaddr_end, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_CHAINSELRULE_SIPEND_ID,
                    CM_DATA_TYPE_STRING,buf);
   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   cm_inet_ntoal(appl_chainselrule_info->info[4].value.dip.ipaddr_start, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_CHAINSELRULE_DIPSTART_ID,
                    CM_DATA_TYPE_STRING,buf);
   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   cm_inet_ntoal(appl_chainselrule_info->info[4].value.dip.ipaddr_end, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_CHAINSELRULE_DIPEND_ID,
                    CM_DATA_TYPE_STRING,buf);
   uindex_i++;

   if (appl_chainselrule_info->info[0].value.src_mac.mac_address_type_e == MAC_ANY)
   {
      sprintf(buf,"%s","Any");
   }
   else if (appl_chainselrule_info->info[0].value.src_mac.mac_address_type_e == MAC_SINGLE)
   {
      sprintf(buf,"%s","Single");
   }
 
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_CHAINSELRULE_DIPTYPE_ID,                                     CM_DATA_TYPE_STRING,buf);
   
   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   if (appl_chainselrule_info->info[2].value.eth_type.ethtype_type_e == ETHTYPE_ANY)
   {
      sprintf(buf,"%s","Any");
   }
   else if (appl_chainselrule_info->info[2].value.eth_type.ethtype_type_e == ETHTYPE_SINGLE)
   {
      sprintf(buf,"%s","Single");
   }

   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_CHAINSELRULE_ETHTYPE_ID,
                                   CM_DATA_TYPE_STRING,buf);
 
   uindex_i++;
   of_memset(buf, 0, sizeof(buf)); 
   if (appl_chainselrule_info->info[5].value.sp.port_object_type_e == PORT_ANY )
   {
      sprintf(buf,"%s","Any");
   }
   else if (appl_chainselrule_info->info[5].value.sp.port_object_type_e == PORT_SINGLE )
   {
      sprintf(buf,"%s","Single");
   }
   else if (appl_chainselrule_info->info[5].value.sp.port_object_type_e == PORT_RANGE )
   {
      sprintf(buf,"%s","Range");
   }
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_CHAINSELRULE_SPTYPE_ID,                                  CM_DATA_TYPE_STRING,buf);

   uindex_i++;

   of_memset(buf, 0, sizeof(buf)); 
   if (appl_chainselrule_info->info[6].value.dp.port_object_type_e == PORT_ANY )
   {
      sprintf(buf,"%s","Any");
   }
   else if (appl_chainselrule_info->info[6].value.dp.port_object_type_e == PORT_SINGLE )
   {
      sprintf(buf,"%s","Single");
   }
   else if (appl_chainselrule_info->info[6].value.dp.port_object_type_e == PORT_RANGE )
   {
      sprintf(buf,"%s","Range");
   }
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_CHAINSELRULE_DPTYPE_ID,                                  CM_DATA_TYPE_STRING,buf);

   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   if (appl_chainselrule_info->info[3].value.sip.ip_object_type_e == IP_ANY )
   {
     sprintf(buf,"%s","Any");
   }
   else if (appl_chainselrule_info->info[3].value.sip.ip_object_type_e == IP_SINGLE )
   {
     sprintf(buf,"%s","Single");
   }
   else if (appl_chainselrule_info->info[3].value.sip.ip_object_type_e == IP_RANGE )
   {
     sprintf(buf,"%s","Range");
   }
   else if (appl_chainselrule_info->info[3].value.sip.ip_object_type_e == IP_SUBNET )
   {
     sprintf(buf,"%s","Subnet");
   }

   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_CHAINSELRULE_SIPTYPE_ID,                                  CM_DATA_TYPE_STRING,buf);

   uindex_i++;


   of_memset(buf, 0, sizeof(buf));
   if (appl_chainselrule_info->info[4].value.dip.ip_object_type_e == IP_ANY )
   {
     sprintf(buf,"%s","Any");
   }
   else if (appl_chainselrule_info->info[4].value.dip.ip_object_type_e == IP_SINGLE )
   {
     sprintf(buf,"%s","Single");
   }
   else if (appl_chainselrule_info->info[4].value.dip.ip_object_type_e == IP_RANGE )
   {
     sprintf(buf,"%s","Range");
   }
   else if (appl_chainselrule_info->info[4].value.dip.ip_object_type_e == IP_SUBNET )
   {
     sprintf(buf,"%s","Subnet");
   }

   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_CHAINSELRULE_DIPTYPE_ID,                                  CM_DATA_TYPE_STRING,buf);

   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   of_itoa (appl_chainselrule_info->info[10].value.admin_status_e, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_CHAINSELRULE_ENABLED_ID,
		   CM_DATA_TYPE_STRING, buf);
   uindex_i++;

  CM_CBK_DEBUG_PRINT("debug");
   result_iv_pairs_p->count_ui = uindex_i;
   CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);

   return OF_SUCCESS;

}

int32_t nsrm_chainselrule_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                                  struct nsrm_nschainset_object_key* nschainset_object_key_p,
                                  struct nsrm_nschain_selection_rule_key* appl_chainselrule_info,
                                  struct nsrm_nschain_selection_rule_config_info* nsrm_chainselrule_config_info)
{
  uint32_t uiMandParamCnt;

  CM_CBK_DEBUG_PRINT ("Entered");

  for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;uiMandParamCnt++)
  {
    switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
    {
      case CM_DM_CHAINSELRULE_SELNAME_ID:
         appl_chainselrule_info->name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
          CM_CBK_DEBUG_PRINT ("name_p:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
          of_strncpy (appl_chainselrule_info->name_p,(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                 pMandParams->iv_pairs[uiMandParamCnt].value_length);
      break;

     // case CM_DM_CHAINSELRULE_SELTENANT_ID:
        case CM_DM_CHAINSET_TENANT_ID:
          appl_chainselrule_info->tenant_name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
          nschainset_object_key_p->tenant_name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
          of_strncpy (appl_chainselrule_info->tenant_name_p,(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,pMandParams->iv_pairs[uiMandParamCnt].value_length);
          CM_CBK_DEBUG_PRINT ("tenant_name_p:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
          of_strncpy (nschainset_object_key_p->tenant_name_p,(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,pMandParams->iv_pairs[uiMandParamCnt].value_length);
      break;

      //case CM_DM_CHAINSELRULE_CHAINSET_NAME_ID:
      case CM_DM_CHAINSET_NAME_ID:
          appl_chainselrule_info->nschainset_object_name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
          nschainset_object_key_p->name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
          CM_CBK_DEBUG_PRINT ("nschainset_object_name_p:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
          of_strncpy (appl_chainselrule_info->nschainset_object_name_p,
                 (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                 pMandParams->iv_pairs[uiMandParamCnt].value_length);
          
          of_strncpy (nschainset_object_key_p->name_p,
                 (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                 pMandParams->iv_pairs[uiMandParamCnt].value_length);

      break;
      
      case CM_DM_CHAINSELRULE_CHAIN_ID:
          nsrm_chainselrule_config_info[8].value.nschain_object_name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
          CM_CBK_DEBUG_PRINT ("nschain_object_name_p:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
          of_strncpy (nsrm_chainselrule_config_info[8].value.nschain_object_name_p,
                  (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                 pMandParams->iv_pairs[uiMandParamCnt].value_length);
      break;

      case CM_DM_CHAINSELRULE_LOCATION_ID:
        if (strcmp(pMandParams->iv_pairs[uiMandParamCnt].value_p, "FIRST") ==0 )
        {
          appl_chainselrule_info->location_e = FIRST;
        }
        else if (strcmp(pMandParams->iv_pairs[uiMandParamCnt].value_p, "LAST") ==0 )
        {
          appl_chainselrule_info->location_e = LAST;
        }
        else if(strcmp(pMandParams->iv_pairs[uiMandParamCnt].value_p, "BEFORE") ==0 )
        {
          appl_chainselrule_info->location_e = BEFORE;
        }
        else if(strcmp(pMandParams->iv_pairs[uiMandParamCnt].value_p, "AFTER") ==0 )
        {
          appl_chainselrule_info->location_e = AFTER;
        }

      break;
     }
  }
  return OF_SUCCESS;
}

int32_t nsrm_chainselrule_ucm_setoptparams (struct cm_array_of_iv_pairs *pOptParams,
                                  struct nsrm_nschain_selection_rule_key* appl_chainselrule_info,
                        struct nsrm_nschain_selection_rule_config_info* nsrm_chainselrule_config_info)
{
  
  uint32_t uiOptParamCnt;
  char     *src_mac , *dst_mac;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (uiOptParamCnt = 0; uiOptParamCnt < pOptParams->count_ui; uiOptParamCnt++)
  {
    switch (pOptParams->iv_pairs[uiOptParamCnt].id_ui)
    {

      
      case CM_DM_CHAINSELRULE_RELATIVE_NAME_ID:
           nsrm_chainselrule_config_info[9].value.relative_name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
           CM_CBK_DEBUG_PRINT("rel name : %s",pOptParams->iv_pairs[uiOptParamCnt].value_p);
           of_strncpy (nsrm_chainselrule_config_info[9].value.relative_name_p,
                  (char *) pOptParams->iv_pairs[uiOptParamCnt].value_p,
                 pOptParams->iv_pairs[uiOptParamCnt].value_length);
           CM_CBK_DEBUG_PRINT("rel name : %s",nsrm_chainselrule_config_info[9].value.relative_name_p);
           CM_CBK_DEBUG_PRINT("rel name : %s",nsrm_chainselrule_config_info[9].value.relative_name_p);

      case CM_DM_CHAINSELRULE_ENABLED_ID:
        nsrm_chainselrule_config_info[10].value.admin_status_e = of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p);
      break;

      case CM_DM_CHAINSELRULE_PROTOCOL_ID:
        nsrm_chainselrule_config_info[7].value.ip_protocol = of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p);
      break;

      case CM_DM_CHAINSELRULE_SRCMACTYPE_ID:
        if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Any") ==0 )
        {
          nsrm_chainselrule_config_info[0].value.src_mac.mac_address_type_e = MAC_ANY;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Single") ==0 )
        {
          nsrm_chainselrule_config_info[0].value.src_mac.mac_address_type_e = MAC_SINGLE;
        }
      break;

      case CM_DM_CHAINSELRULE_ETHTYPE_ID:
        if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Any") ==0 )
        {
          nsrm_chainselrule_config_info[2].value.eth_type.ethtype_type_e = ETHTYPE_ANY;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Single") ==0 )
        {
          nsrm_chainselrule_config_info[2].value.eth_type.ethtype_type_e = ETHTYPE_SINGLE;
        }
      break;

      case CM_DM_CHAINSELRULE_DSTMACTYPE_ID:
        if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Any") ==0 )
        {
          nsrm_chainselrule_config_info[1].value.dst_mac.mac_address_type_e = MAC_ANY;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Single") ==0 )
        {
          nsrm_chainselrule_config_info[1].value.dst_mac.mac_address_type_e = MAC_SINGLE;
        }
      break;

      case CM_DM_CHAINSELRULE_SRCMAC_ID:
           CM_CBK_DEBUG_PRINT ("src_mac_address:%s",(char *) pOptParams->iv_pairs[uiOptParamCnt].value_p);
           src_mac = (char *) pOptParams->iv_pairs[uiOptParamCnt].value_p;
           if ((*src_mac == '0') &&
            ((*(src_mac+1) == 'x') || (*(src_mac+1) == 'X')))
           {
              of_flow_match_atox_mac_addr(src_mac+2, nsrm_chainselrule_config_info[0].value.src_mac.mac);
           }
           else
           {
              of_flow_match_atox_mac_addr(src_mac, nsrm_chainselrule_config_info[0].value.src_mac.mac);
           }
   
           CM_CBK_DEBUG_PRINT ("src_mac_address:%s",nsrm_chainselrule_config_info[0].value.src_mac.mac);

      break;

      case CM_DM_CHAINSELRULE_DSTMAC_ID:
           CM_CBK_DEBUG_PRINT ("dst_mac_address:%s",(char *) pOptParams->iv_pairs[uiOptParamCnt].value_p);
           dst_mac = (char *) pOptParams->iv_pairs[uiOptParamCnt].value_p;
           if ((*dst_mac == '0') &&
            ((*(dst_mac+1) == 'x') || (*(dst_mac+1) == 'X')))
           {
              of_flow_match_atox_mac_addr(dst_mac+2, nsrm_chainselrule_config_info[1].value.dst_mac.mac);
           }
           else
           {
              of_flow_match_atox_mac_addr(dst_mac, nsrm_chainselrule_config_info[1].value.dst_mac.mac);
           }
   
           CM_CBK_DEBUG_PRINT ("dst_mac_address:%s",nsrm_chainselrule_config_info[1].value.dst_mac.mac);

 
      case CM_DM_CHAINSELRULE_ETHTYPEVALUE_ID:
          nsrm_chainselrule_config_info[2].value.eth_type.ethtype = of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p);
      break;

      case CM_DM_CHAINSELRULE_SPTYPE_ID:
        if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Any") ==0 )
        {
          nsrm_chainselrule_config_info[5].value.sp.port_object_type_e = PORT_ANY;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Single") ==0 )
        {
          nsrm_chainselrule_config_info[5].value.sp.port_object_type_e = PORT_SINGLE;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Range") ==0 )
        {
          nsrm_chainselrule_config_info[5].value.sp.port_object_type_e = PORT_RANGE;
        }
      break;

      case CM_DM_CHAINSELRULE_DPTYPE_ID:
        if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Any") ==0 )
        {
          nsrm_chainselrule_config_info[6].value.dp.port_object_type_e = PORT_ANY;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Single") ==0 )
        {
          nsrm_chainselrule_config_info[6].value.dp.port_object_type_e = PORT_SINGLE;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Range") ==0 )
        {
          nsrm_chainselrule_config_info[6].value.dp.port_object_type_e = PORT_RANGE;
        }
      break;

      case CM_DM_CHAINSELRULE_SPSTART_ID:
          nsrm_chainselrule_config_info[5].value.sp.port_start = of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p);
      break;

      case CM_DM_CHAINSELRULE_SPEND_ID:
          nsrm_chainselrule_config_info[5].value.sp.port_end = of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p);
      break;

      case CM_DM_CHAINSELRULE_DPEND_ID:
          nsrm_chainselrule_config_info[6].value.dp.port_end = of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p);
      break;

      case CM_DM_CHAINSELRULE_DPSTART_ID:
          nsrm_chainselrule_config_info[6].value.dp.port_start = of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p);
      break;

      case CM_DM_CHAINSELRULE_SIPTYPE_ID:
        if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Any") ==0 )
        {
          nsrm_chainselrule_config_info[3].value.sip.ip_object_type_e = IP_ANY;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Single") ==0 )
        {
          nsrm_chainselrule_config_info[3].value.sip.ip_object_type_e = IP_SINGLE;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Range") ==0 )
        {
          nsrm_chainselrule_config_info[3].value.sip.ip_object_type_e = IP_RANGE;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Subnet") ==0 )
        {
          nsrm_chainselrule_config_info[3].value.sip.ip_object_type_e = IP_SUBNET;
        }
      break;

      case CM_DM_CHAINSELRULE_DIPTYPE_ID:
        if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Any") ==0 )
        {
          nsrm_chainselrule_config_info[4].value.dip.ip_object_type_e = IP_ANY;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Single") ==0 )
        {
          nsrm_chainselrule_config_info[4].value.dip.ip_object_type_e = IP_SINGLE;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Range") ==0 )
        {
          nsrm_chainselrule_config_info[4].value.dip.ip_object_type_e = IP_RANGE;
        }
        else if (strcmp(pOptParams->iv_pairs[uiOptParamCnt].value_p, "Subnet") ==0 )
        {
          nsrm_chainselrule_config_info[4].value.dip.ip_object_type_e = IP_SUBNET;
        }
      break;

      case CM_DM_CHAINSELRULE_SIPSTART_ID:
         if(cm_val_and_conv_aton((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p, &nsrm_chainselrule_config_info[3].value.sip.ipaddr_start) != OF_SUCCESS)
            return OF_FAILURE;
        
      break;

      case CM_DM_CHAINSELRULE_SIPEND_ID:
         if(cm_val_and_conv_aton((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p, &nsrm_chainselrule_config_info[3].value.sip.ipaddr_end) != OF_SUCCESS)
            return OF_FAILURE;
      break;

      case CM_DM_CHAINSELRULE_DIPSTART_ID:
        if(cm_val_and_conv_aton((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p, &nsrm_chainselrule_config_info[4].value.dip.ipaddr_start) != OF_SUCCESS)
            return OF_FAILURE;
      break;

      case CM_DM_CHAINSELRULE_DIPEND_ID:
         if(cm_val_and_conv_aton((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p, &nsrm_chainselrule_config_info[4].value.dip.ipaddr_end) != OF_SUCCESS)
            return OF_FAILURE;
      break;

    }
   }
     CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
     return OF_SUCCESS;
}

int32_t nsrm_chainselrule_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
                        uint32_t command_id, struct cm_app_result ** result_p)
{
  struct cm_app_result *of_chainselrule_result = NULL;
  int32_t return_value = OF_FAILURE;

  return OF_SUCCESS;
  CM_CBK_DEBUG_PRINT ("Entered");
  return_value = nsrm_chainselrule_ucm_validatemandparams(key_iv_pairs_p, &of_chainselrule_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  *result_p = of_chainselrule_result;
  return OF_SUCCESS;

}

int32_t nsrm_chainselrule_ucm_validatemandparams(struct cm_array_of_iv_pairs *mand_iv_pairs_p,
    struct cm_app_result **  presult_p)
{
  uint32_t count;
  struct cm_app_result *of_chainselrule_result = NULL;


  CM_CBK_DEBUG_PRINT ("Entered");
  for (count = 0; count < mand_iv_pairs_p->count_ui;count++)
  {
    switch (mand_iv_pairs_p->iv_pairs[count].id_ui)
    {
      case CM_DM_CHAINSELRULE_SELNAME_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Appl category name is NULL");
          fill_app_result_struct (&of_chainselrule_result, NULL, CM_GLU_SELECTION_RULE_NAME_NULL);
          *presult_p = of_chainselrule_result;
          return OF_FAILURE;
        }
        break;

       case CM_DM_CHAINSET_TENANT_ID: 
       if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Appl category name is NULL");
          fill_app_result_struct (&of_chainselrule_result, NULL, CM_GLU_TENANT_ID_INVALID);
          *presult_p = of_chainselrule_result;
          return OF_FAILURE;
        }
        break;


     }
   }
  CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs_p);
  return OF_SUCCESS;
}



