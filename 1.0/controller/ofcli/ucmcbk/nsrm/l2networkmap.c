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
 * File name: l2networkmap.c
 * Author:
 * Date: 
 * Description: 
*/
#include "cmincld.h"
#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "nsrm.h"

int32_t nsrm_l2networkmap_init (void);

int32_t nsrm_l2networkmap_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);

int32_t nsrm_l2networkmap_modrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);


int32_t nsrm_l2networkmap_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p);


int32_t nsrm_l2networkmap_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);



int32_t nsrm_l2networkmap_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p,  uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p);



int32_t nsrm_l2networkmap_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs_p,
                                        struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t nsrm_l2networkmap_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
                        uint32_t command_id, struct cm_app_result ** result_p);


int32_t nsrm_l2networkmap_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                        struct nsrm_l2nw_service_map_key* appl_category_info,
                        struct nsrm_l2nw_service_map_config_info* nsrm_l2networkmap_config_info);


int32_t nsrm_l2networkmap_ucm_setoptparams (struct cm_array_of_iv_pairs *pOptParams,
                                  struct nsrm_l2nw_service_map_key* appl_l2networkmap_info,
                        struct nsrm_l2nw_service_map_config_info* nsrm_l2networkmap_config_info);

int32_t nsrm_l2networkmap_ucm_getparams (struct nsrm_l2nw_service_map_record *appl_l2networkmap_info,
                                   struct cm_array_of_iv_pairs * result_iv_pairs_p);

int32_t nsrm_l2networkmap_ucm_validatemandparams(struct cm_array_of_iv_pairs *mand_iv_pairs_p,
    struct cm_app_result **  presult_p);


struct cm_dm_app_callbacks nsrm_l2networkmap_callbacks =
{
  NULL,
  nsrm_l2networkmap_addrec,
  nsrm_l2networkmap_modrec,
  nsrm_l2networkmap_delrec,
  NULL,
  nsrm_l2networkmap_getfirstnrecs,
  nsrm_l2networkmap_getnextnrecs,
  nsrm_l2networkmap_getexactrec,
  nsrm_l2networkmap_verifycfg,
  NULL
};

int32_t nsrm_l2networkmap_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  if (cm_register_app_callbacks (CM_ON_DIRECTOR_NSRM_NETWORKMAP_APPL_ID, &nsrm_l2networkmap_callbacks) != OF_SUCCESS)
    return OF_FAILURE;

  return OF_SUCCESS;
}

int32_t nsrm_l2networkmap_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p)
{
   struct cm_app_result *nsrm_l2networkmap_result = NULL;
   int32_t return_value = OF_FAILURE;
   struct nsrm_l2nw_service_map_key *nsrm_l2networkmap_key_info;
   struct nsrm_l2nw_service_map_config_info *nsrm_l2networkmap_config_info;

   CM_CBK_DEBUG_PRINT ("Entered(nsrm_l2networkmap_addrec)");
  nsrm_l2networkmap_key_info = (struct nsrm_l2nw_service_map_key*) of_calloc(1, sizeof(struct nsrm_l2nw_service_map_key));
  nsrm_l2networkmap_config_info = (struct nsrm_l2nw_service_map_config_info *) 
                          of_calloc(NSRM_MAX_L2NW_CONFIG_PARAMETERS, sizeof(struct nsrm_l2nw_service_map_config_info));
  if ((nsrm_l2networkmap_ucm_setmandparams (pMandParams, nsrm_l2networkmap_key_info, 
                  nsrm_l2networkmap_config_info)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&nsrm_l2networkmap_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=nsrm_l2networkmap_result;
    of_free(nsrm_l2networkmap_key_info->map_name_p);
    of_free(nsrm_l2networkmap_key_info->tenant_name_p);
    of_free(nsrm_l2networkmap_key_info->vn_name_p);
    of_free(nsrm_l2networkmap_key_info);
    return OF_FAILURE;
  }

  CM_CBK_DEBUG_PRINT ("chainset:%s",(char *) nsrm_l2networkmap_config_info->value.nschainset_object_name_p);
  if ((nsrm_l2networkmap_ucm_setoptparams (pOptParams, nsrm_l2networkmap_key_info,
                  nsrm_l2networkmap_config_info)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
    fill_app_result_struct (&nsrm_l2networkmap_result, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
    *result_p=nsrm_l2networkmap_result;
    of_free(nsrm_l2networkmap_key_info->map_name_p);
    of_free(nsrm_l2networkmap_key_info->tenant_name_p);
    of_free(nsrm_l2networkmap_key_info->vn_name_p);
    of_free(nsrm_l2networkmap_key_info);
    return OF_FAILURE;
  }

  CM_CBK_DEBUG_PRINT ("chainset:%s",(char *) nsrm_l2networkmap_config_info->value.nschainset_object_name_p);
  CM_CBK_DEBUG_PRINT ("vnname:%s",(char *) nsrm_l2networkmap_key_info->vn_name_p);
  return_value = nsrm_add_l2nw_service_map_record(2, nsrm_l2networkmap_key_info,
                                         nsrm_l2networkmap_config_info);
  if(return_value == NSRM_ERROR_DUPLICATE_RESOURCE)
  {
    CM_CBK_DEBUG_PRINT ("This l2map maps to some other VN");
    CM_CBK_DEBUG_PRINT ("nsrm l2networkmap add  Failed");
    fill_app_result_struct (&nsrm_l2networkmap_result, NULL, CM_GLU_L2NW_MAPS_TO_OTHER_VN);
    *result_p = nsrm_l2networkmap_result;
    of_free(nsrm_l2networkmap_key_info->map_name_p);
    of_free(nsrm_l2networkmap_key_info->tenant_name_p);
    of_free(nsrm_l2networkmap_key_info->vn_name_p);
    of_free(nsrm_l2networkmap_key_info);
    return OF_FAILURE;
  }

  else if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("nsrm l2networkmap add  Failed");
    fill_app_result_struct (&nsrm_l2networkmap_result, NULL, CM_GLU_L2NW_ADD_FAILED);
    *result_p = nsrm_l2networkmap_result;
    of_free(nsrm_l2networkmap_key_info->map_name_p);
    of_free(nsrm_l2networkmap_key_info->tenant_name_p);
    of_free(nsrm_l2networkmap_key_info->vn_name_p);
    of_free(nsrm_l2networkmap_key_info);
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("NSRM l2networkmap added successfully");
    of_free(nsrm_l2networkmap_key_info->map_name_p);
    of_free(nsrm_l2networkmap_key_info->tenant_name_p);
    of_free(nsrm_l2networkmap_key_info->vn_name_p);
    of_free(nsrm_l2networkmap_key_info);
  return OF_SUCCESS;
}


int32_t nsrm_l2networkmap_modrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p)
{
   struct cm_app_result *nsrm_l2networkmap_result = NULL;
   int32_t return_value = OF_FAILURE;
   struct nsrm_l2nw_service_map_key *nsrm_l2networkmap_key_info;
   struct nsrm_l2nw_service_map_config_info *nsrm_l2networkmap_config_info;

   CM_CBK_DEBUG_PRINT ("Entered(nsrm_category_addrec)");

   nsrm_l2networkmap_key_info = (struct nsrm_l2nw_service_map_key*) of_calloc(1, sizeof(struct nsrm_l2nw_service_map_key));
   nsrm_l2networkmap_config_info = (struct nsrm_l2nw_service_map_config_info *) 
                          of_calloc(NSRM_MAX_L2NW_CONFIG_PARAMETERS, sizeof(struct nsrm_l2nw_service_map_config_info));
   if ((nsrm_l2networkmap_ucm_setmandparams (pMandParams, nsrm_l2networkmap_key_info,
                       nsrm_l2networkmap_config_info)) != OF_SUCCESS)
   {
     CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
     fill_app_result_struct (&nsrm_l2networkmap_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
     *result_p=nsrm_l2networkmap_result;
     of_free(nsrm_l2networkmap_key_info->map_name_p);
     of_free(nsrm_l2networkmap_key_info->tenant_name_p);
     of_free(nsrm_l2networkmap_key_info->vn_name_p);
     of_free(nsrm_l2networkmap_key_info);
     return OF_FAILURE;
   }
  if ((nsrm_l2networkmap_ucm_setoptparams (pOptParams, nsrm_l2networkmap_key_info,
                  nsrm_l2networkmap_config_info)) != OF_SUCCESS)
  {
     CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
     fill_app_result_struct (&nsrm_l2networkmap_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
     *result_p=nsrm_l2networkmap_result;
     of_free(nsrm_l2networkmap_key_info->map_name_p);
     of_free(nsrm_l2networkmap_key_info->tenant_name_p);
     of_free(nsrm_l2networkmap_key_info->vn_name_p);
     of_free(nsrm_l2networkmap_key_info);
     return OF_FAILURE;
   }
 
   return_value = nsrm_modify_l2nw_service_map_record(nsrm_l2networkmap_key_info, 2, 
                nsrm_l2networkmap_config_info);
   if (return_value != OF_SUCCESS)
   {
     CM_CBK_DEBUG_PRINT ("nsrm l2networkmap modify  Failed");
     fill_app_result_struct (&nsrm_l2networkmap_result, NULL, CM_GLU_L2NW_MOD_FAILED);
     *result_p = nsrm_l2networkmap_result;
     of_free(nsrm_l2networkmap_key_info->map_name_p);
     of_free(nsrm_l2networkmap_key_info->tenant_name_p);
     of_free(nsrm_l2networkmap_key_info->vn_name_p);
     of_free(nsrm_l2networkmap_key_info);
     return OF_FAILURE;
   }
   CM_CBK_DEBUG_PRINT ("NSRM l2networkmap modified successfully");
   of_free(nsrm_l2networkmap_key_info->map_name_p);
   of_free(nsrm_l2networkmap_key_info->tenant_name_p);
   of_free(nsrm_l2networkmap_key_info->vn_name_p);
   of_free(nsrm_l2networkmap_key_info);
   return OF_SUCCESS;
}
 
int32_t nsrm_l2networkmap_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p)
{

   struct cm_app_result *nsrm_l2networkmap_result = NULL;
  int32_t return_value = OF_FAILURE;
   struct nsrm_l2nw_service_map_key *nsrm_l2networkmap_key_info;
   struct nsrm_l2nw_service_map_config_info *nsrm_l2networkmap_config_info;

  CM_CBK_DEBUG_PRINT ("Entered");
  nsrm_l2networkmap_key_info = (struct nsrm_l2nw_service_map_key*) of_calloc(1, sizeof(struct nsrm_l2nw_service_map_key));
  nsrm_l2networkmap_config_info = (struct nsrm_l2nw_service_map_config_info *) 
                          of_calloc(NSRM_MAX_SELECTION_RULE_CONFIG_PARAMETERS, sizeof(struct nsrm_l2nw_service_map_config_info));
  if ((nsrm_l2networkmap_ucm_setmandparams (keys_arr_p, nsrm_l2networkmap_key_info,
                     nsrm_l2networkmap_config_info)) !=OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&nsrm_l2networkmap_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p = nsrm_l2networkmap_result;
    of_free(nsrm_l2networkmap_key_info->map_name_p);
    of_free(nsrm_l2networkmap_key_info->tenant_name_p);
    of_free(nsrm_l2networkmap_key_info->vn_name_p);
    of_free(nsrm_l2networkmap_key_info);
    return OF_FAILURE;
  }
  return_value = nsrm_del_l2nw_service_map_record(nsrm_l2networkmap_key_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("nsrm l2networkmap add  Failed");
    fill_app_result_struct (&nsrm_l2networkmap_result, NULL, CM_GLU_L2NW_DEL_FAILED);
    *result_p = nsrm_l2networkmap_result;
    of_free(nsrm_l2networkmap_key_info->map_name_p);
    of_free(nsrm_l2networkmap_key_info->tenant_name_p);
    of_free(nsrm_l2networkmap_key_info->vn_name_p);
    of_free(nsrm_l2networkmap_key_info);
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("NSRM Appliance delete successfully");
  of_free(nsrm_l2networkmap_key_info->map_name_p);
  of_free(nsrm_l2networkmap_key_info->tenant_name_p);
  of_free(nsrm_l2networkmap_key_info->vn_name_p);
  of_free(nsrm_l2networkmap_key_info);
  return OF_SUCCESS;

}

int32_t nsrm_l2networkmap_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  int32_t uiRequestedCount = 1, uiReturnedCnt=0;
  struct nsrm_l2nw_service_map_record *nsrm_l2networkmap_config_info;

  CM_CBK_DEBUG_PRINT ("Entered");
  nsrm_l2networkmap_config_info = (struct nsrm_l2nw_service_map_record *)of_calloc(1,sizeof(struct nsrm_l2nw_service_map_record));
  nsrm_l2networkmap_config_info->info = (struct nsrm_l2nw_service_map_config_info *)of_calloc(NSRM_MAX_L2NW_CONFIG_PARAMETERS,sizeof(struct nsrm_l2nw_service_map_config_info));
  nsrm_l2networkmap_config_info->key.map_name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH);
  nsrm_l2networkmap_config_info->key.tenant_name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH);
  nsrm_l2networkmap_config_info->key.vn_name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH);
  nsrm_l2networkmap_config_info->info[0].value.nschainset_object_name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH);
 return_value = nsrm_get_first_l2nw_service_maps(uiRequestedCount, &uiReturnedCnt, nsrm_l2networkmap_config_info);

 if (return_value != OF_SUCCESS)
 {
   CM_CBK_DEBUG_PRINT ("Get first record failed for NSRM l2networkmap  Table");
   return OF_FAILURE;
 }

  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
 if (result_iv_pairs_p == NULL)
 {
   CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    return OF_FAILURE;
 }
  nsrm_l2networkmap_ucm_getparams (nsrm_l2networkmap_config_info, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
 *array_of_iv_pair_arr_p = result_iv_pairs_p;
  return OF_SUCCESS;
}


int32_t nsrm_l2networkmap_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p,  uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  int32_t uiRequestedCount = 1, uiReturnedCnt=0;
  struct nsrm_l2nw_service_map_key *nsrm_l2networkmap_key_info;
  struct nsrm_l2nw_service_map_record *nsrm_l2networkmap_config_info;
  struct   nsrm_l2nw_service_map_config_info*   nsrm_srvmap_config_info;


  CM_CBK_DEBUG_PRINT ("Entered");


  nsrm_l2networkmap_config_info = (struct nsrm_l2nw_service_map_record *)of_calloc(1,sizeof(struct nsrm_l2nw_service_map_record));
  nsrm_l2networkmap_key_info = (struct nsrm_l2nw_service_map_key*) of_calloc(1, sizeof(struct nsrm_l2nw_service_map_key));
  nsrm_srvmap_config_info = (struct nsrm_l2nw_service_map_config_info *) 
                          of_calloc(NSRM_MAX_L2NW_CONFIG_PARAMETERS, sizeof(struct nsrm_l2nw_service_map_config_info));
  nsrm_l2networkmap_config_info->info = (struct nsrm_l2nw_service_map_config_info *)of_calloc(NSRM_MAX_L2NW_CONFIG_PARAMETERS,sizeof(struct nsrm_l2nw_service_map_config_info));
  nsrm_l2networkmap_config_info->key.map_name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH);
  nsrm_l2networkmap_config_info->key.tenant_name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH);
  nsrm_l2networkmap_config_info->key.vn_name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH);
  nsrm_l2networkmap_config_info->info[0].value.nschainset_object_name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH);


  
  if ((nsrm_l2networkmap_ucm_setmandparams (prev_record_key_p, nsrm_l2networkmap_key_info, 
                                       nsrm_srvmap_config_info)) != OF_SUCCESS)
  {
     CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    of_free(nsrm_l2networkmap_key_info->map_name_p);
    of_free(nsrm_l2networkmap_key_info->tenant_name_p);
    of_free(nsrm_l2networkmap_key_info->vn_name_p);
    of_free(nsrm_l2networkmap_key_info);
     return OF_FAILURE;
  }

  return_value = nsrm_get_next_l2nw_service_maps(nsrm_l2networkmap_key_info, uiRequestedCount, 
                             &uiReturnedCnt, nsrm_l2networkmap_config_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Get next record failed for NSRM appl  Table");
    of_free(nsrm_l2networkmap_key_info->map_name_p);
    of_free(nsrm_l2networkmap_key_info->tenant_name_p);
    of_free(nsrm_l2networkmap_key_info->vn_name_p);
    of_free(nsrm_l2networkmap_key_info);
    return OF_FAILURE;
  }

  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    of_free(nsrm_l2networkmap_key_info->map_name_p);
    of_free(nsrm_l2networkmap_key_info->tenant_name_p);
    of_free(nsrm_l2networkmap_key_info->vn_name_p);
    of_free(nsrm_l2networkmap_key_info);
    return OF_FAILURE;
  }
  nsrm_l2networkmap_ucm_getparams (nsrm_l2networkmap_config_info, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
 *next_n_record_data_p = result_iv_pairs_p;
 *count_p = uiRecCount;
    of_free(nsrm_l2networkmap_key_info->map_name_p);
    of_free(nsrm_l2networkmap_key_info->tenant_name_p);
    of_free(nsrm_l2networkmap_key_info->vn_name_p);
    of_free(nsrm_l2networkmap_key_info);
  return OF_SUCCESS;
}

int32_t nsrm_l2networkmap_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs ** pIvPairArr)

{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct nsrm_l2nw_service_map_key *nsrm_l2networkmap_key_info;
  uint32_t uiRecCount = 0;
  int32_t iRes = OF_FAILURE;
  struct nsrm_l2nw_service_map_record *l2networkmap_object_config_info;
  struct nsrm_l2nw_service_map_config_info*   nsrm_srvmap_config_info;

  CM_CBK_DEBUG_PRINT ("Entered");
  l2networkmap_object_config_info = (struct nsrm_l2nw_service_map_record *)of_calloc(1,sizeof(struct nsrm_l2nw_service_map_record));
  nsrm_l2networkmap_key_info = (struct nsrm_l2nw_service_map_key*) of_calloc(1, sizeof(struct nsrm_l2nw_service_map_key));
  nsrm_srvmap_config_info = (struct nsrm_l2nw_service_map_config_info *) 
                          of_calloc(NSRM_MAX_L2NW_CONFIG_PARAMETERS, sizeof(struct nsrm_l2nw_service_map_config_info));
  l2networkmap_object_config_info->info = (struct nsrm_l2nw_service_map_config_info *)of_calloc(NSRM_MAX_L2NW_CONFIG_PARAMETERS,sizeof(struct nsrm_l2nw_service_map_config_info));
  l2networkmap_object_config_info->key.map_name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH);
  l2networkmap_object_config_info->key.tenant_name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH);
  l2networkmap_object_config_info->key.vn_name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH);
  l2networkmap_object_config_info->info[0].value.nschainset_object_name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH);

  if((nsrm_l2networkmap_ucm_setmandparams(key_iv_pairs_p, nsrm_l2networkmap_key_info, 
                   nsrm_srvmap_config_info)) !=OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    of_free(nsrm_l2networkmap_key_info->map_name_p);
    of_free(nsrm_l2networkmap_key_info->tenant_name_p);
    of_free(nsrm_l2networkmap_key_info->vn_name_p);
    of_free(nsrm_l2networkmap_key_info);
    return OF_FAILURE;
  }

 iRes = nsrm_get_exact_l2nw_service_map(nsrm_l2networkmap_key_info, l2networkmap_object_config_info);
  if (iRes != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: Appliacne doesn't exists with name %s",nsrm_l2networkmap_key_info->map_name_p);
    of_free(nsrm_l2networkmap_key_info->map_name_p);
    of_free(nsrm_l2networkmap_key_info->tenant_name_p);
    of_free(nsrm_l2networkmap_key_info->vn_name_p);
    of_free(nsrm_l2networkmap_key_info);
    return OF_FAILURE;
  }

  CM_CBK_DEBUG_PRINT ("Exact matching record found");
  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));

  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Failed to allocate memory for result_iv_pairs_p");
    of_free(nsrm_l2networkmap_key_info->map_name_p);
    of_free(nsrm_l2networkmap_key_info->tenant_name_p);
    of_free(nsrm_l2networkmap_key_info->vn_name_p);
    of_free(nsrm_l2networkmap_key_info);
    return OF_FAILURE;
  }
  nsrm_l2networkmap_ucm_getparams(l2networkmap_object_config_info, &result_iv_pairs_p[uiRecCount]);
  *pIvPairArr = result_iv_pairs_p;
                                                                                       
    of_free(nsrm_l2networkmap_key_info->map_name_p);
    of_free(nsrm_l2networkmap_key_info->tenant_name_p);
    of_free(nsrm_l2networkmap_key_info->vn_name_p);
    of_free(nsrm_l2networkmap_key_info);
  return OF_SUCCESS;
}

int32_t nsrm_l2networkmap_ucm_getparams (struct nsrm_l2nw_service_map_record *appl_l2networkmap_info,
                                   struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
   uint32_t uindex_i = 0;
   char buf[128] = "";
   CM_CBK_DEBUG_PRINT ("Entered");
#define CM_APPLIANCE_CHILD_COUNT 5

   result_iv_pairs_p->iv_pairs =(struct cm_iv_pair *) of_calloc (CM_APPLIANCE_CHILD_COUNT, sizeof (struct cm_iv_pair));
   if (result_iv_pairs_p->iv_pairs == NULL)
   {
     CM_CBK_DEBUG_PRINT("Memory allocation failed for result_iv_pairs_p->iv_pairs");
     return OF_FAILURE;
   }

   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_NETWORKMAP_NAME_ID,
		   CM_DATA_TYPE_STRING, appl_l2networkmap_info->key.map_name_p);
   CM_CBK_DEBUG_PRINT("name : %s",result_iv_pairs_p->iv_pairs[uindex_i].value_p);
   uindex_i++;

   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_NETWORKMAP_TENANT_ID,
		   CM_DATA_TYPE_STRING, appl_l2networkmap_info->key.tenant_name_p);
   CM_CBK_DEBUG_PRINT("tenant_name_p : %s",result_iv_pairs_p->iv_pairs[uindex_i].value_p);
   uindex_i++;

   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_NETWORKMAP_VNNAME_ID,
		   CM_DATA_TYPE_STRING, appl_l2networkmap_info->key.vn_name_p);
   CM_CBK_DEBUG_PRINT("vn_name_p: %s",result_iv_pairs_p->iv_pairs[uindex_i].value_p);
   uindex_i++;

   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_NETWORKMAP_CHAINSET_ID,
		   CM_DATA_TYPE_STRING, appl_l2networkmap_info->info[0].value.nschainset_object_name_p);
   CM_CBK_DEBUG_PRINT("nschainset_object_name_p: %s",result_iv_pairs_p->iv_pairs[uindex_i].value_p);
   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   of_itoa (appl_l2networkmap_info->info[1].value.admin_status_e, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_NETWORKMAP_ENABLED_ID,
		   CM_DATA_TYPE_STRING, buf);
   uindex_i++;

   result_iv_pairs_p->count_ui = uindex_i;
   CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);

   return OF_SUCCESS;

}

int32_t nsrm_l2networkmap_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                                  struct nsrm_l2nw_service_map_key* appl_l2networkmap_info,
                                  struct nsrm_l2nw_service_map_config_info* nsrm_l2networkmap_config_info)
{
  uint32_t uiMandParamCnt;

  CM_CBK_DEBUG_PRINT ("Entered");

  for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;uiMandParamCnt++)
  {
    switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
    {
      case CM_DM_NETWORKMAP_NAME_ID:
         appl_l2networkmap_info->map_name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
          CM_CBK_DEBUG_PRINT ("map_name_p:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
          of_strncpy (appl_l2networkmap_info->map_name_p,(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                 pMandParams->iv_pairs[uiMandParamCnt].value_length);
      break;

      case CM_DM_NETWORKMAP_TENANT_ID:
          appl_l2networkmap_info->tenant_name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
          of_strncpy (appl_l2networkmap_info->tenant_name_p,(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                 pMandParams->iv_pairs[uiMandParamCnt].value_length);
      break;

      
      case CM_DM_NETWORKMAP_VNNAME_ID:
         appl_l2networkmap_info->vn_name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
          CM_CBK_DEBUG_PRINT ("vnname:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
          of_strncpy (appl_l2networkmap_info->vn_name_p,(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                 pMandParams->iv_pairs[uiMandParamCnt].value_length);
      break;

     }
  }
        return OF_SUCCESS;
}

int32_t nsrm_l2networkmap_ucm_setoptparams (struct cm_array_of_iv_pairs *pOptParams,
                                  struct nsrm_l2nw_service_map_key* appl_l2networkmap_info,
                        struct nsrm_l2nw_service_map_config_info* nsrm_l2networkmap_config_info)
{
  
  uint32_t uiOptParamCnt;
  CM_CBK_DEBUG_PRINT ("Entered");
  for (uiOptParamCnt = 0; uiOptParamCnt < pOptParams->count_ui; uiOptParamCnt++)
  {
    switch (pOptParams->iv_pairs[uiOptParamCnt].id_ui)
    {
      case CM_DM_NETWORKMAP_ENABLED_ID:
        nsrm_l2networkmap_config_info[1].value.admin_status_e = of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p);
      break;

      case CM_DM_NETWORKMAP_CHAINSET_ID:
          nsrm_l2networkmap_config_info[0].value.nschainset_object_name_p = (char *) of_calloc(1,NSRM_MAX_NAME_LENGTH);
          CM_CBK_DEBUG_PRINT ("map_name_p:%s",(char *) pOptParams->iv_pairs[uiOptParamCnt].value_p);
          of_strncpy (nsrm_l2networkmap_config_info[0].value.nschainset_object_name_p,
                 (char *) pOptParams->iv_pairs[uiOptParamCnt].value_p,
                 pOptParams->iv_pairs[uiOptParamCnt].value_length);
      break;

     
    }
   }
     CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
     return OF_SUCCESS;

}


int32_t nsrm_l2networkmap_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
                        uint32_t command_id, struct cm_app_result ** result_p)
{
  struct cm_app_result *of_l2networkmap_result = NULL;
  int32_t return_value = OF_FAILURE;

  CM_CBK_DEBUG_PRINT ("Entered");
  return_value = nsrm_l2networkmap_ucm_validatemandparams(key_iv_pairs_p, &of_l2networkmap_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  *result_p = of_l2networkmap_result;
  return OF_SUCCESS;

}


int32_t nsrm_l2networkmap_ucm_validatemandparams(struct cm_array_of_iv_pairs *mand_iv_pairs_p,
    struct cm_app_result **  presult_p)
{
  uint32_t count;
  struct cm_app_result *of_l2networkmap_result = NULL;


  CM_CBK_DEBUG_PRINT ("Entered");
  for (count = 0; count < mand_iv_pairs_p->count_ui;count++)
  {
    switch (mand_iv_pairs_p->iv_pairs[count].id_ui)
    {
      case CM_DM_NETWORKMAP_NAME_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Appl category name is NULL");
          fill_app_result_struct (&of_l2networkmap_result, NULL, CM_GLU_L2NW_NAME_NULL);
          *presult_p = of_l2networkmap_result;
          return OF_FAILURE;
        }
        break;

      case CM_DM_NETWORKMAP_TENANT_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Appl category name is NULL");
          fill_app_result_struct (&of_l2networkmap_result, NULL, CM_GLU_TENANT_ID_INVALID);
          *presult_p = of_l2networkmap_result;
          return OF_FAILURE;
        }
        break;

      case CM_DM_NETWORKMAP_VNNAME_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Appl category name is NULL");
          fill_app_result_struct (&of_l2networkmap_result, NULL, CM_GLU_VN_NAME_NULL);
          *presult_p = of_l2networkmap_result;
          return OF_FAILURE;
        }
        break;

     }
   }
  CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs_p);
  return OF_SUCCESS;
}



