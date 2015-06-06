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
* Unless required by nwsrvicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
**/

/*
 *
 * File name: bypass_services.c
 * Author:
 * Date: 
 * Description: 
*/
#include "cmincld.h"
#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "nsrm.h"

int32_t nsrm_attach_nwservice_to_bypass_rule_init (void);

int32_t nsrm_attach_nwservice_to_bypass_rule_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);

int32_t nsrm_attach_nwservice_to_bypass_rule_modrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);


int32_t nsrm_attach_nwservice_to_bypass_rule_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p);


int32_t nsrm_attach_nwservice_to_bypass_rule_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);



int32_t nsrm_attach_nwservice_to_bypass_rule_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p,  uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p);



int32_t nsrm_attach_nwservice_to_bypass_rule_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs_p,
                                        struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t nsrm_attach_nwservice_to_bypass_rule_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
                        uint32_t command_id, struct cm_app_result ** result_p);

int32_t nsrm_attach_nwservice_to_bypass_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                        struct nsrm_nschain_object_key*          chain_obj_info_p,
                        struct nsrm_nwservice_bypass_rule_key*   bypass_info_p,
                        struct nsrm_bypass_rule_nwservice_key*   key_info_p
                        );

int32_t nsrm_attach_nwservice_to_bypasss_getparams (struct  nsrm_bypass_rule_nwservice_key* key_info_p
                                   ,struct cm_array_of_iv_pairs * result_iv_pairs_p);

struct cm_dm_app_callbacks nsrm_attach_nwservice_to_bypass_rule_callbacks =
{
  NULL,
  nsrm_attach_nwservice_to_bypass_rule_addrec,
  NULL,
  nsrm_attach_nwservice_to_bypass_rule_delrec,
  NULL,
  nsrm_attach_nwservice_to_bypass_rule_getfirstnrecs,
  nsrm_attach_nwservice_to_bypass_rule_getnextnrecs,
  nsrm_attach_nwservice_to_bypass_rule_getexactrec,
  nsrm_attach_nwservice_to_bypass_rule_verifycfg,
  NULL
};

int32_t nsrm_attach_nwservice_to_bypass_rule_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  if (cm_register_app_callbacks (CM_DM_BYPASSRULE_BYPASS_SERVICES_ID,
              &nsrm_attach_nwservice_to_bypass_rule_callbacks) != OF_SUCCESS)
    return OF_FAILURE;

  return OF_SUCCESS;
}

int32_t nsrm_attach_nwservice_to_bypass_rule_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p)
{
   struct  cm_app_result                   *nsrm_bypass_services_result = NULL;
   struct  nsrm_nschain_object_key         *nschain_object_key = NULL;
   struct  nsrm_nwservice_bypass_rule_key  *bypass_rule_key = NULL;
   struct  nsrm_bypass_rule_nwservice_key  *nwservice_bypass_key = NULL;  

   int32_t return_value = OF_FAILURE;

   CM_CBK_DEBUG_PRINT ("Entered");
   nschain_object_key = (struct nsrm_nschain_object_key*)of_calloc(1,sizeof(struct nsrm_nschain_object_key));
   bypass_rule_key    = (struct  nsrm_nwservice_bypass_rule_key*)of_calloc(1,sizeof(struct  nsrm_nwservice_bypass_rule_key));
   nwservice_bypass_key = (struct  nsrm_bypass_rule_nwservice_key*)of_calloc(1,sizeof(struct  nsrm_bypass_rule_nwservice_key)); 
 
   if(nsrm_attach_nwservice_to_bypass_setmandparams(pMandParams ,
                                                    nschain_object_key,
                                                    bypass_rule_key,
                                                    nwservice_bypass_key) != OF_SUCCESS)
   {
      CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
      fill_app_result_struct(&nsrm_bypass_services_result , NULL ,CM_GLU_SET_MAND_PARAM_FAILED);
      *result_p = nsrm_bypass_services_result;
      of_free(nschain_object_key);
      of_free(nschain_object_key->name_p);
      of_free(bypass_rule_key);
      of_free(bypass_rule_key->name_p);
      of_free(nwservice_bypass_key->nschain_object_name_p);
      of_free(nwservice_bypass_key->nwservice_object_name_p);
      of_free(nwservice_bypass_key->bypass_rule_name_p);
      return OF_FAILURE;
  }      
  
  return_value = nsrm_attach_nwservice_to_bypass_rule(nwservice_bypass_key);
  if(return_value != NSRM_SUCCESS)
  {
     fill_app_result_struct(&nsrm_bypass_services_result , NULL ,CM_GLU_NS_CHAIN_ADD_FAILED);
      *result_p = nsrm_bypass_services_result;
      of_free(nschain_object_key);
      of_free(bypass_rule_key);
      of_free(nschain_object_key->name_p);
      of_free(bypass_rule_key->name_p);
      of_free(nwservice_bypass_key->nschain_object_name_p);
      of_free(nwservice_bypass_key->nwservice_object_name_p);
      of_free(nwservice_bypass_key->bypass_rule_name_p);
      return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT("Added succesfully");
  return NSRM_SUCCESS;
} 

int32_t nsrm_attach_nwservice_to_bypass_rule_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p)
{
   struct  cm_app_result                   *nsrm_bypass_services_result = NULL;
   struct  nsrm_nschain_object_key         *nschain_object_key = NULL;
   struct  nsrm_nwservice_bypass_rule_key  *bypass_rule_key = NULL;
   struct  nsrm_bypass_rule_nwservice_key  *nwservice_bypass_key = NULL;

   int32_t return_value = OF_FAILURE;

   CM_CBK_DEBUG_PRINT ("Entered");
   nschain_object_key = (struct nsrm_nschain_object_key*)of_calloc(1,sizeof(struct nsrm_nschain_object_key));
   bypass_rule_key    = (struct  nsrm_nwservice_bypass_rule_key*)of_calloc(1,sizeof(struct  nsrm_nwservice_bypass_rule_key));
   nwservice_bypass_key = (struct  nsrm_bypass_rule_nwservice_key*)of_calloc(1,sizeof(struct  nsrm_bypass_rule_nwservice_key));

   if(nsrm_attach_nwservice_to_bypass_setmandparams(keys_arr_p ,
                                                    nschain_object_key,
                                                    bypass_rule_key,
                                                    nwservice_bypass_key) != OF_SUCCESS)
   {
      CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
      fill_app_result_struct(&nsrm_bypass_services_result , NULL ,CM_GLU_SET_MAND_PARAM_FAILED);
      *result_p = nsrm_bypass_services_result;
      of_free(nschain_object_key->name_p);
      of_free(nschain_object_key);
      of_free(bypass_rule_key);
      of_free(bypass_rule_key->name_p);
      of_free(nwservice_bypass_key->nschain_object_name_p);
      of_free(nwservice_bypass_key->nwservice_object_name_p);
      of_free(nwservice_bypass_key->bypass_rule_name_p);
      return OF_FAILURE;
  }

  return_value = nsrm_detach_nwservice_from_bypass_rule(nwservice_bypass_key);
  if(return_value != NSRM_SUCCESS)
  {
     fill_app_result_struct(&nsrm_bypass_services_result , NULL ,CM_GLU_SET_MAND_PARAM_FAILED);
      *result_p = nsrm_bypass_services_result;
      of_free(nschain_object_key->name_p);
      of_free(bypass_rule_key->name_p);
      of_free(nwservice_bypass_key->nschain_object_name_p);
      of_free(nwservice_bypass_key->nwservice_object_name_p);
      of_free(nwservice_bypass_key->bypass_rule_name_p);
      return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT("Deleted succesfully");
  return NSRM_SUCCESS;
}

int32_t nsrm_attach_nwservice_to_bypass_rule_getfirstnrecs (struct cm_array_of_iv_pairs* keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{
   struct cm_array_of_iv_pairs             *result_iv_pairs_p = NULL;
   struct cm_app_result                    *nsrm_attach_result = NULL;
   struct  nsrm_nschain_object_key         *nschain_object_key = NULL;
   struct  nsrm_nwservice_bypass_rule_key  *bypass_rule_key = NULL;
   struct  nsrm_bypass_rule_nwservice_key  *nwservice_bypass_key = NULL;
   struct  nsrm_bypass_rule_nwservice_key*  nwservice_bypass_key_record = NULL;

   int32_t  uiRecquestedCount = 1, uiReturnedCnt=0;
   int32_t return_value = OF_FAILURE;

   CM_CBK_DEBUG_PRINT ("Entered");
   nschain_object_key = (struct nsrm_nschain_object_key*)of_calloc(1,sizeof(struct nsrm_nschain_object_key));
   bypass_rule_key    = (struct  nsrm_nwservice_bypass_rule_key*)of_calloc(1,sizeof(struct  nsrm_nwservice_bypass_rule_key));
   nwservice_bypass_key = (struct  nsrm_bypass_rule_nwservice_key*)of_calloc(1,sizeof(struct  nsrm_bypass_rule_nwservice_key));
  nwservice_bypass_key_record = (struct  nsrm_bypass_rule_nwservice_key*)of_calloc(1,sizeof(struct  nsrm_bypass_rule_nwservice_key));
  nwservice_bypass_key_record->nwservice_object_name_p = (char *) of_calloc(1,NSRM_MAX_NAME_LENGTH);

   if(nsrm_attach_nwservice_to_bypass_setmandparams(keys_arr_p ,
                                                    nschain_object_key,
                                                    bypass_rule_key,
                                                    nwservice_bypass_key) != OF_SUCCESS)
   {
      CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
      fill_app_result_struct(&nsrm_attach_result , NULL ,CM_GLU_SET_MAND_PARAM_FAILED);
      *array_of_iv_pair_arr_p = result_iv_pairs_p;
      of_free(nschain_object_key->name_p);
      of_free(bypass_rule_key->name_p);
      of_free(nwservice_bypass_key->nschain_object_name_p);
      of_free(nwservice_bypass_key->nwservice_object_name_p);
      of_free(nwservice_bypass_key->bypass_rule_name_p);
      of_free(nwservice_bypass_key_record);
      of_free(nwservice_bypass_key_record->nwservice_object_name_p);
      return OF_FAILURE;
  }

  CM_CBK_DEBUG_PRINT("name : %s",nwservice_bypass_key->nwservice_object_name_p);
  CM_CBK_DEBUG_PRINT("name : %s",nwservice_bypass_key->nwservice_object_name_p);
  return_value =nsrm_get_first_nwservice_objects_from_bypass_rule(bypass_rule_key,
                                                                  uiRecquestedCount, &uiReturnedCnt,
                                                                  nwservice_bypass_key_record);
  if(return_value != NSRM_SUCCESS)
  {
      fill_app_result_struct(&nsrm_attach_result , NULL ,CM_GLU_SET_MAND_PARAM_FAILED);
      *array_of_iv_pair_arr_p = result_iv_pairs_p;
      of_free(nschain_object_key->name_p);
      of_free(bypass_rule_key->name_p);
      of_free(nwservice_bypass_key->nschain_object_name_p);
      of_free(nwservice_bypass_key->nwservice_object_name_p);
      of_free(nwservice_bypass_key->bypass_rule_name_p);
      of_free(nwservice_bypass_key_record);
      of_free(nwservice_bypass_key_record->nwservice_object_name_p);
      return OF_FAILURE;
  }
  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));

  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Failed to allocate memory for result_iv_pairs_p");
    of_free(nschain_object_key->name_p);
    of_free(bypass_rule_key->name_p);
    of_free(nwservice_bypass_key->nschain_object_name_p);
    of_free(nwservice_bypass_key->nwservice_object_name_p);
    of_free(nwservice_bypass_key->bypass_rule_name_p);
    of_free(nwservice_bypass_key_record);
    of_free(nwservice_bypass_key_record->nwservice_object_name_p);
    return OF_FAILURE;
  }

  nsrm_attach_nwservice_to_bypasss_getparams(nwservice_bypass_key_record ,result_iv_pairs_p);
  *array_of_iv_pair_arr_p = result_iv_pairs_p;
  
  of_free(nschain_object_key->name_p);
  of_free(bypass_rule_key->name_p);
  of_free(nwservice_bypass_key->nschain_object_name_p);
  of_free(nwservice_bypass_key->nwservice_object_name_p);
  of_free(nwservice_bypass_key->bypass_rule_name_p);
  of_free(nwservice_bypass_key_record);
  of_free(nwservice_bypass_key_record->nwservice_object_name_p);
  return OF_SUCCESS;
}

int32_t nsrm_attach_nwservice_to_bypass_rule_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p,  uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p)
{
   struct cm_array_of_iv_pairs             *result_iv_pairs_p = NULL;
   struct  nsrm_nschain_object_key         *nschain_object_key = NULL;
   struct  nsrm_nwservice_bypass_rule_key  *bypass_rule_key = NULL;
   struct  nsrm_bypass_rule_nwservice_key  *nwservice_bypass_key = NULL;
   struct  nsrm_bypass_rule_nwservice_key*  nwservice_bypass_key_record = NULL;

   uint32_t uiRecCount = 0;
   int32_t  uiRecquestedCount = 1, uiReturnedCnt=0;
   int32_t return_value = OF_FAILURE;

   CM_CBK_DEBUG_PRINT ("Entered");
   nschain_object_key = (struct nsrm_nschain_object_key*)of_calloc(1,sizeof(struct nsrm_nschain_object_key));
   bypass_rule_key    = (struct  nsrm_nwservice_bypass_rule_key*)of_calloc(1,sizeof(struct  nsrm_nwservice_bypass_rule_key));
   nwservice_bypass_key = (struct  nsrm_bypass_rule_nwservice_key*)of_calloc(1,sizeof(struct  nsrm_bypass_rule_nwservice_key));
  nwservice_bypass_key_record = (struct  nsrm_bypass_rule_nwservice_key*)of_calloc(1,sizeof(struct  nsrm_bypass_rule_nwservice_key));
  nwservice_bypass_key_record->nwservice_object_name_p = (char *) of_calloc(1,NSRM_MAX_NAME_LENGTH);


  nsrm_attach_nwservice_to_bypass_setmandparams(keys_arr_p ,
                                                    nschain_object_key,
                                                    bypass_rule_key,
                                                    nwservice_bypass_key);


   if(nsrm_attach_nwservice_to_bypass_setmandparams(prev_record_key_p ,
                                                    nschain_object_key,
                                                    bypass_rule_key,
                                                    nwservice_bypass_key) != OF_SUCCESS)
   {
      CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
      of_free(nschain_object_key->name_p);
      of_free(bypass_rule_key->name_p);
      of_free(nwservice_bypass_key->nschain_object_name_p);
      of_free(nwservice_bypass_key->nwservice_object_name_p);
      of_free(nwservice_bypass_key->bypass_rule_name_p);
      of_free(nwservice_bypass_key_record);
      of_free(nwservice_bypass_key_record->nwservice_object_name_p);
      return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT("name : %s",nwservice_bypass_key->nwservice_object_name_p);
  CM_CBK_DEBUG_PRINT("name : %s",nwservice_bypass_key->nwservice_object_name_p);
  return_value =nsrm_get_next_nwservice_objects_from_bypass_rule(bypass_rule_key,nwservice_bypass_key,
                                                                  uiRecquestedCount, &uiReturnedCnt,
                                                                  nwservice_bypass_key_record);
  if(return_value != NSRM_SUCCESS)
  {
      of_free(nschain_object_key->name_p);
      of_free(bypass_rule_key->name_p);
      of_free(nwservice_bypass_key->nschain_object_name_p);
      of_free(nwservice_bypass_key->nwservice_object_name_p);
      of_free(nwservice_bypass_key->bypass_rule_name_p);
      of_free(nwservice_bypass_key_record);
      of_free(nwservice_bypass_key_record->nwservice_object_name_p);
      return OF_FAILURE;
  }
  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));

  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Failed to allocate memory for result_iv_pairs_p");
    of_free(nschain_object_key->name_p);
    of_free(bypass_rule_key->name_p);
    of_free(nwservice_bypass_key->nschain_object_name_p);
    of_free(nwservice_bypass_key->nwservice_object_name_p);
    of_free(nwservice_bypass_key->bypass_rule_name_p);
    of_free(nwservice_bypass_key_record);
    of_free(nwservice_bypass_key_record->nwservice_object_name_p);
    return OF_FAILURE;
  }

  nsrm_attach_nwservice_to_bypasss_getparams(nwservice_bypass_key_record,&result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *next_n_record_data_p = result_iv_pairs_p;
  *count_p = uiRecCount;
 
  of_free(nschain_object_key->name_p);
  of_free(bypass_rule_key->name_p);
  of_free(nwservice_bypass_key->nschain_object_name_p);
  of_free(nwservice_bypass_key->nwservice_object_name_p);
  of_free(nwservice_bypass_key->bypass_rule_name_p);
  of_free(nwservice_bypass_key_record);
  of_free(nwservice_bypass_key_record->nwservice_object_name_p);
  return OF_SUCCESS;
}

int32_t nsrm_attach_nwservice_to_bypass_rule_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs_p,
                                        struct cm_array_of_iv_pairs ** pIvPairArr)
{
   struct cm_array_of_iv_pairs             *result_iv_pairs_p = NULL;
   struct cm_app_result                    *nsrm_attach_result = NULL;
   struct  nsrm_nschain_object_key         *nschain_object_key = NULL;
   struct  nsrm_nwservice_bypass_rule_key  *bypass_rule_key = NULL;
   struct  nsrm_bypass_rule_nwservice_key  *nwservice_bypass_key = NULL;
   struct  nsrm_bypass_rule_nwservice_key*  nwservice_bypass_key_record = NULL;

   int32_t return_value = OF_FAILURE;

   CM_CBK_DEBUG_PRINT ("Entered");
   nschain_object_key = (struct nsrm_nschain_object_key*)of_calloc(1,sizeof(struct nsrm_nschain_object_key));
   bypass_rule_key    = (struct  nsrm_nwservice_bypass_rule_key*)of_calloc(1,sizeof(struct  nsrm_nwservice_bypass_rule_key));
   nwservice_bypass_key = (struct  nsrm_bypass_rule_nwservice_key*)of_calloc(1,sizeof(struct  nsrm_bypass_rule_nwservice_key));
   nwservice_bypass_key_record = (struct  nsrm_bypass_rule_nwservice_key*)of_calloc(1,sizeof(struct  nsrm_bypass_rule_nwservice_key));
   nwservice_bypass_key_record->nwservice_object_name_p = (char *) of_calloc(1,NSRM_MAX_NAME_LENGTH);

   if(nsrm_attach_nwservice_to_bypass_setmandparams(key_iv_pairs_p ,
                                                    nschain_object_key,
                                                    bypass_rule_key,
                                                    nwservice_bypass_key) != OF_SUCCESS)
   {
      CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
      fill_app_result_struct(&nsrm_attach_result , NULL ,CM_GLU_SET_MAND_PARAM_FAILED);
      *pIvPairArr = result_iv_pairs_p;
      of_free(nschain_object_key->name_p);
      of_free(bypass_rule_key->name_p);
      of_free(nwservice_bypass_key->nschain_object_name_p);
      of_free(nwservice_bypass_key->nwservice_object_name_p);
      of_free(nwservice_bypass_key->bypass_rule_name_p);
      of_free(nwservice_bypass_key_record);
      of_free(nwservice_bypass_key_record->nwservice_object_name_p);
      return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("debug");
  CM_CBK_DEBUG_PRINT ("debug");

  return_value =nsrm_get_exact_nwservice_objects_from_bypass_rule(bypass_rule_key,nwservice_bypass_key,
                                                                  nwservice_bypass_key_record);
  if(return_value != NSRM_SUCCESS)
  {
    fill_app_result_struct(&nsrm_attach_result , NULL ,CM_GLU_SET_MAND_PARAM_FAILED);
    *pIvPairArr = result_iv_pairs_p;
    of_free(nschain_object_key->name_p);
    of_free(bypass_rule_key->name_p);
    of_free(nwservice_bypass_key->nschain_object_name_p);
    of_free(nwservice_bypass_key->nwservice_object_name_p);
    of_free(nwservice_bypass_key->bypass_rule_name_p);
    of_free(nwservice_bypass_key_record);
    of_free(nwservice_bypass_key_record->nwservice_object_name_p);
    return OF_FAILURE;
  }
  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));

  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Failed to allocate memory for result_iv_pairs_p");
    of_free(nschain_object_key->name_p);
    of_free(bypass_rule_key->name_p);
    of_free(nwservice_bypass_key->nschain_object_name_p);
    of_free(nwservice_bypass_key->nwservice_object_name_p);
    of_free(nwservice_bypass_key->bypass_rule_name_p);
    of_free(nwservice_bypass_key_record);
    of_free(nwservice_bypass_key_record->nwservice_object_name_p);
    return OF_FAILURE;
  }

  nsrm_attach_nwservice_to_bypasss_getparams(nwservice_bypass_key_record ,result_iv_pairs_p);
  *pIvPairArr = result_iv_pairs_p;
  
  of_free(nschain_object_key->name_p);
  of_free(bypass_rule_key->name_p);
  of_free(nwservice_bypass_key->nschain_object_name_p);
  of_free(nwservice_bypass_key->nwservice_object_name_p);
  of_free(nwservice_bypass_key->bypass_rule_name_p);
  of_free(nwservice_bypass_key_record);
  of_free(nwservice_bypass_key_record->nwservice_object_name_p);
  CM_CBK_DEBUG_PRINT ("before return in get exact");
  return OF_SUCCESS;
}


int32_t nsrm_attach_nwservice_to_bypasss_getparams (struct  nsrm_bypass_rule_nwservice_key* key_info_p
                                   ,struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
   uint32_t uindex_i = 0;
   CM_CBK_DEBUG_PRINT ("Entered");
   #define CM_BYPASS_NWSERVICES_COUNT 1
 
   result_iv_pairs_p->iv_pairs =(struct cm_iv_pair *)
                                of_calloc (CM_BYPASS_NWSERVICES_COUNT, sizeof (struct cm_iv_pair));
   if (result_iv_pairs_p->iv_pairs == NULL)
   {
     CM_CBK_DEBUG_PRINT("Memory allocation failed for result_iv_pairs_p->iv_pairs");
     return OF_FAILURE;
   }

   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_SERVICENAMES_SERVICENAME_ID,
                   CM_DATA_TYPE_STRING, key_info_p->nwservice_object_name_p);
   CM_CBK_DEBUG_PRINT("name : %s",result_iv_pairs_p->iv_pairs[uindex_i].value_p);
   uindex_i++;
  

   result_iv_pairs_p->count_ui = uindex_i;
   CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);

   return OF_SUCCESS;
}

int32_t nsrm_attach_nwservice_to_bypass_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                        struct nsrm_nschain_object_key*          chain_obj_info_p,
                        struct nsrm_nwservice_bypass_rule_key*   bypass_info_p,
                        struct nsrm_bypass_rule_nwservice_key*   key_info_p
                        )
{
  uint32_t uiMandParamCnt;
  CM_CBK_DEBUG_PRINT ("Entered");
  
  for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;uiMandParamCnt++)
  {
    switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
    {
      case CM_DM_SERVICENAMES_SERVICENAME_ID:
           key_info_p->nwservice_object_name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH);
           of_strncpy (key_info_p->nwservice_object_name_p,
                       (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                        pMandParams->iv_pairs[uiMandParamCnt].value_length);
           CM_CBK_DEBUG_PRINT("name : %s",key_info_p->nwservice_object_name_p);
            break;
      case CM_DM_BYPASSRULE_BYPASSNAME_ID:
           key_info_p->bypass_rule_name_p = (char *)of_calloc(1,NSRM_MAX_NAME_LENGTH);
           bypass_info_p->name_p = (char *)of_calloc(1,NSRM_MAX_NAME_LENGTH);
           of_strncpy (key_info_p->bypass_rule_name_p,
                       (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                        pMandParams->iv_pairs[uiMandParamCnt].value_length);
           of_strncpy (bypass_info_p->name_p,
                       (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                        pMandParams->iv_pairs[uiMandParamCnt].value_length);
 
           break;
      case CM_DM_CHAIN_NAME_ID:
           key_info_p->nschain_object_name_p = (char *)of_calloc(1,NSRM_MAX_NAME_LENGTH);
           bypass_info_p->nschain_object_name_p = (char *)of_calloc(1,NSRM_MAX_NAME_LENGTH);
           chain_obj_info_p->name_p  = (char *)of_calloc(1,NSRM_MAX_NAME_LENGTH);
           CM_CBK_DEBUG_PRINT("chain name :%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
           of_strncpy (bypass_info_p->nschain_object_name_p,
                       (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                        pMandParams->iv_pairs[uiMandParamCnt].value_length);
           of_strncpy (chain_obj_info_p->name_p,
                       (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                        pMandParams->iv_pairs[uiMandParamCnt].value_length);
           of_strncpy (key_info_p->nschain_object_name_p,
                       (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                        pMandParams->iv_pairs[uiMandParamCnt].value_length);

          break;
     }
   }
  return OF_SUCCESS;
} 

int32_t nsrm_attach_nwservice_to_bypass_rule_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
                        uint32_t command_id, struct cm_app_result ** result_p)
{
  uint32_t uiMandParamCnt;
  unsigned char key_found = FALSE;
  CM_CBK_DEBUG_PRINT ("Entered");
  
  for (uiMandParamCnt = 0; uiMandParamCnt < key_iv_pairs_p->count_ui;uiMandParamCnt++)
  {
    switch (key_iv_pairs_p->iv_pairs[uiMandParamCnt].id_ui)
    {
      case CM_DM_BYPASSRULE_BYPASSNAME_ID:
            key_found = TRUE;
           break;
     }
   }
  if (key_found == FALSE)
    return OF_FAILURE;

  return OF_SUCCESS;
}



