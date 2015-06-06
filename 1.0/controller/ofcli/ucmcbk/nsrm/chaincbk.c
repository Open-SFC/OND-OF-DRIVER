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
 * File name: chaincbk.c
 * Author:
 * Date: 
 * Description: 
*/


#include "cmincld.h"
#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "nsrm.h"

int32_t nsrm_chain_init (void);

int32_t nsrm_chain_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);

int32_t nsrm_chain_modrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);

int32_t nsrm_chain_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p);

int32_t nsrm_chain_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);

int32_t nsrm_chain_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p,  uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p);

int32_t nsrm_chain_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs_p,
                                        struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t nsrm_chain_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
                        uint32_t command_id, struct cm_app_result ** result_p);

int32_t nsrm_chain_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                        struct nsrm_nschain_object_key* appl_category_info,
                        struct nsrm_nschain_object_config_info* nsrm_chain_config_info);

int32_t nsrm_chain_ucm_setoptparams (struct cm_array_of_iv_pairs *pOptParams,
                        struct nsrm_nschain_object_config_info* nsrm_chain_config_info);

int32_t nsrm_chain_ucm_getparams (struct nsrm_nschain_object_record *appl_chain_info,
                                   struct cm_array_of_iv_pairs * result_iv_pairs_p);


int32_t nsrm_chain_ucm_validateoptsparams (struct cm_array_of_iv_pairs *pOptParams,
                        struct cm_app_result ** presult_p);

int32_t nsrm_chain_ucm_validatemandparams(struct cm_array_of_iv_pairs *mand_iv_pairs_p,
    struct cm_app_result **  presult_p);

struct cm_dm_app_callbacks nsrm_chain_callbacks =
{
  NULL,
  nsrm_chain_addrec,
  nsrm_chain_modrec,
  nsrm_chain_delrec,
  NULL,
  nsrm_chain_getfirstnrecs,
  nsrm_chain_getnextnrecs,
  nsrm_chain_getexactrec,
  nsrm_chain_verifycfg,
  NULL
};

int32_t nsrm_chain_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  if (cm_register_app_callbacks (CM_ON_DIRECTOR_NSRM_CHAIN_APPL_ID, &nsrm_chain_callbacks) != OF_SUCCESS)
    return OF_FAILURE;

  return OF_SUCCESS;
}


int32_t nsrm_chain_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p)
{

   struct cm_app_result *nsrm_chain_result = NULL;
   int32_t return_value = OF_FAILURE;
   struct nsrm_nschain_object_key *nsrm_chain_key_info;
   struct nsrm_nschain_object_config_info *nsrm_chain_config_info = NULL;

   CM_CBK_DEBUG_PRINT ("Entered(nsrm_chain_addrec)");

  nsrm_chain_key_info = (struct nsrm_nschain_object_key*) of_calloc(1, sizeof(struct nsrm_nschain_object_key));

  nsrm_chain_config_info = (struct nsrm_nschain_object_config_info *)of_calloc(NSRM_MAX_NSCHAIN_OBJECT_CONFIG_PARAMETERS,sizeof(struct nsrm_nschain_object_config_info));
  if ((nsrm_chain_ucm_setmandparams (pMandParams, nsrm_chain_key_info, nsrm_chain_config_info)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&nsrm_chain_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=nsrm_chain_result;
    of_free(nsrm_chain_key_info->name_p);
    of_free(nsrm_chain_key_info->tenant_name_p);
    of_free(nsrm_chain_key_info);
    return OF_FAILURE;
  }

   
  CM_CBK_DEBUG_PRINT ("type : %d",nsrm_chain_config_info[0].value.nschain_type);

    if ((nsrm_chain_ucm_setoptparams (pOptParams, nsrm_chain_config_info)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
    fill_app_result_struct (&nsrm_chain_result, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
    *result_p=nsrm_chain_result;
    of_free(nsrm_chain_key_info->name_p);
    of_free(nsrm_chain_key_info->tenant_name_p);
    of_free(nsrm_chain_key_info);
    return OF_FAILURE;
  }


  CM_CBK_DEBUG_PRINT ("high : %d",nsrm_chain_config_info[3].value.high_threshold);
  CM_CBK_DEBUG_PRINT ("low : %d",nsrm_chain_config_info[4].value.low_threshold);
  return_value = nsrm_add_nschain_object(2, nsrm_chain_key_info,
                                         nsrm_chain_config_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("nsrm chain add  Failed");
    fill_app_result_struct (&nsrm_chain_result, NULL, CM_GLU_NS_CHAIN_ADD_FAILED);
    *result_p = nsrm_chain_result;
    of_free(nsrm_chain_key_info->name_p);
    of_free(nsrm_chain_key_info->tenant_name_p);
    of_free(nsrm_chain_key_info);
    return OF_FAILURE;
  }

  
  CM_CBK_DEBUG_PRINT ("NSRM chain added successfully");
  of_free(nsrm_chain_key_info->name_p);
  of_free(nsrm_chain_key_info->tenant_name_p);
  of_free(nsrm_chain_key_info);
  return OF_SUCCESS;
}


int32_t nsrm_chain_modrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p)
{
   struct cm_app_result *nsrm_chain_result = NULL;
   int32_t return_value = OF_FAILURE;
   struct nsrm_nschain_object_key *nsrm_chain_key_info =NULL ;
   struct nsrm_nschain_object_config_info *nsrm_chain_config_info = NULL;

   CM_CBK_DEBUG_PRINT ("Entered(nsrm_category_addrec)");

  nsrm_chain_key_info = (struct nsrm_nschain_object_key*) of_calloc(1, sizeof(struct nsrm_nschain_object_key));
  nsrm_chain_config_info = (struct nsrm_nschain_object_config_info *)of_calloc(NSRM_MAX_NSCHAIN_OBJECT_CONFIG_PARAMETERS,sizeof(struct nsrm_nschain_object_config_info));
  if ((nsrm_chain_ucm_setmandparams (pMandParams, nsrm_chain_key_info, nsrm_chain_config_info)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&nsrm_chain_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=nsrm_chain_result;
    of_free(nsrm_chain_key_info->name_p);
    of_free(nsrm_chain_key_info->tenant_name_p);
    of_free(nsrm_chain_key_info);
    return OF_FAILURE;
  }
 if ((nsrm_chain_ucm_setoptparams (pOptParams, nsrm_chain_config_info)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
    fill_app_result_struct (&nsrm_chain_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=nsrm_chain_result;
    of_free(nsrm_chain_key_info->name_p);
    of_free(nsrm_chain_key_info->tenant_name_p);
    of_free(nsrm_chain_key_info);
    return OF_FAILURE;
  }

  return_value = nsrm_modify_nschain_object(nsrm_chain_key_info, 2, nsrm_chain_config_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("nsrm chain modify  Failed");
    fill_app_result_struct (&nsrm_chain_result, NULL, CM_GLU_NS_CHAIN_MOD_FAILED);
    *result_p = nsrm_chain_result;
    of_free(nsrm_chain_key_info->name_p);
    of_free(nsrm_chain_key_info->tenant_name_p);
    of_free(nsrm_chain_key_info);
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("NSRM chain modified successfully");
    of_free(nsrm_chain_key_info->name_p);
    of_free(nsrm_chain_key_info->tenant_name_p);
    of_free(nsrm_chain_key_info);
  return OF_SUCCESS;
}

int32_t nsrm_chain_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p)
{

   struct cm_app_result *nsrm_chain_result = NULL;
  int32_t return_value = OF_FAILURE;

   struct nsrm_nschain_object_key *nsrm_chain_key_info;
   struct nsrm_nschain_object_config_info *nsrm_chain_config_info = NULL;

  CM_CBK_DEBUG_PRINT ("Entered");
  nsrm_chain_key_info = (struct nsrm_nschain_object_key*) of_calloc(1, sizeof(struct nsrm_nschain_object_key));
  nsrm_chain_config_info = (struct nsrm_nschain_object_config_info *)of_calloc(NSRM_MAX_NSCHAIN_OBJECT_CONFIG_PARAMETERS,sizeof(struct nsrm_nschain_object_config_info ));

  if ((nsrm_chain_ucm_setmandparams (keys_arr_p, nsrm_chain_key_info, nsrm_chain_config_info)) !=OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&nsrm_chain_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p = nsrm_chain_result;
    of_free(nsrm_chain_key_info->name_p);
    of_free(nsrm_chain_key_info->tenant_name_p);
    of_free(nsrm_chain_key_info);
    return OF_FAILURE;
  }
  return_value = nsrm_del_nschain_object(nsrm_chain_key_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("nsrm chain add  Failed");
    fill_app_result_struct (&nsrm_chain_result, NULL, CM_GLU_NS_CHAIN_DEL_FAILED);
    *result_p = nsrm_chain_result;
    of_free(nsrm_chain_key_info->name_p);
    of_free(nsrm_chain_key_info->tenant_name_p);
    of_free(nsrm_chain_key_info);
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("NSRM Appliance delete successfully");
    of_free(nsrm_chain_key_info->name_p);
    of_free(nsrm_chain_key_info->tenant_name_p);
    of_free(nsrm_chain_key_info);
  return OF_SUCCESS;

}

int32_t nsrm_chain_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  int32_t uiRequestedCount = 1, uiReturnedCnt=0;
  struct nsrm_nschain_object_record *nsrm_chain_config_info;


  CM_CBK_DEBUG_PRINT ("Entered");
 
  nsrm_chain_config_info = (struct nsrm_nschain_object_record*)of_calloc(1,sizeof(struct nsrm_nschain_object_record));
  nsrm_chain_config_info->info = (struct nsrm_nschain_object_config_info *)of_calloc(NSRM_MAX_NSCHAIN_OBJECT_CONFIG_PARAMETERS,sizeof(struct nsrm_nschain_object_config_info));

  nsrm_chain_config_info->key.name_p = (char*)of_calloc(1,128);
  nsrm_chain_config_info->key.tenant_name_p = (char*)of_calloc(1,128);  

  return_value = nsrm_get_first_nschain_objects(uiRequestedCount, &uiReturnedCnt, nsrm_chain_config_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Get first record failed for NSRM chain  Table");
    return OF_FAILURE;
  }

  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    of_free(nsrm_chain_config_info->key.name_p);
    of_free(nsrm_chain_config_info->key.tenant_name_p);
    of_free(nsrm_chain_config_info);
    return OF_FAILURE;
  }
  nsrm_chain_ucm_getparams (nsrm_chain_config_info, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
 *array_of_iv_pair_arr_p = result_iv_pairs_p;
    of_free(nsrm_chain_config_info->key.name_p);
    of_free(nsrm_chain_config_info->key.tenant_name_p);
    of_free(nsrm_chain_config_info);
  return OF_SUCCESS;
}


int32_t nsrm_chain_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p,  uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  int32_t uiRequestedCount = 1, uiReturnedCnt=0;
  struct nsrm_nschain_object_key *nsrm_chain_key_info = NULL;
  struct nsrm_nschain_object_record *nsrm_chain_config_info = NULL;
  struct nsrm_nschain_object_config_info *chain_config_info = NULL;

  CM_CBK_DEBUG_PRINT ("Entered");
  nsrm_chain_key_info = (struct nsrm_nschain_object_key*) of_calloc(1, sizeof(struct nsrm_nschain_object_key));

  chain_config_info = (struct nsrm_nschain_object_config_info *)of_calloc(NSRM_MAX_NSCHAIN_OBJECT_CONFIG_PARAMETERS,sizeof(struct nsrm_nschain_object_config_info));
  nsrm_chain_config_info = (struct nsrm_nschain_object_record*)of_calloc(1,sizeof(struct nsrm_nschain_object_record));
  nsrm_chain_config_info->info = (struct nsrm_nschain_object_config_info *)of_calloc(NSRM_MAX_NSCHAIN_OBJECT_CONFIG_PARAMETERS,sizeof(struct nsrm_nschain_object_config_info));

  nsrm_chain_config_info->key.name_p = (char*)of_calloc(1,128);
  nsrm_chain_config_info->key.tenant_name_p = (char*)of_calloc(1,128);

  if ((nsrm_chain_ucm_setmandparams (prev_record_key_p, nsrm_chain_key_info, 
                                       chain_config_info)) != OF_SUCCESS)
  {
     CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    of_free(nsrm_chain_key_info->name_p);
    of_free(nsrm_chain_key_info->tenant_name_p);
    of_free(nsrm_chain_key_info);
     return OF_FAILURE;
  }

 return_value = nsrm_get_next_nschain_objects(nsrm_chain_key_info, uiRequestedCount, 
                    &uiReturnedCnt, nsrm_chain_config_info);
 if (return_value != OF_SUCCESS)
 {
   CM_CBK_DEBUG_PRINT ("Get next record failed for NSRM appl  Table");
    of_free(nsrm_chain_key_info->name_p);
    of_free(nsrm_chain_key_info->tenant_name_p);
    of_free(nsrm_chain_key_info);
   return OF_FAILURE;
 }

  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
 if (result_iv_pairs_p == NULL)
 {
   CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    of_free(nsrm_chain_config_info->key.name_p);
    of_free(nsrm_chain_config_info->key.tenant_name_p);
    of_free(nsrm_chain_config_info);
    return OF_FAILURE;
 }
  nsrm_chain_ucm_getparams (nsrm_chain_config_info, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *next_n_record_data_p = result_iv_pairs_p;
  *count_p = uiRecCount;
  of_free(nsrm_chain_key_info->name_p);
  of_free(nsrm_chain_key_info->tenant_name_p);
  of_free(nsrm_chain_key_info);

    of_free(nsrm_chain_config_info->key.name_p);
    of_free(nsrm_chain_config_info->key.tenant_name_p);
    of_free(nsrm_chain_config_info);
  return OF_SUCCESS;
}

int32_t nsrm_chain_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs ** pIvPairArr)

{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct nsrm_nschain_object_key *nsrm_chain_key_info;
  struct nsrm_nschain_object_config_info *nsrm_chain_config_info = NULL;
  int32_t iRes = OF_FAILURE;
  struct nsrm_nschain_object_record *chain_object_config_info ;

  CM_CBK_DEBUG_PRINT ("Entered");
  nsrm_chain_key_info = (struct nsrm_nschain_object_key*) of_calloc(1, sizeof(struct nsrm_nschain_object_key));
  nsrm_chain_config_info = (struct nsrm_nschain_object_config_info *)of_calloc(NSRM_MAX_NSCHAIN_OBJECT_CONFIG_PARAMETERS,sizeof(struct nsrm_nschain_object_config_info));
  if((nsrm_chain_ucm_setmandparams(key_iv_pairs_p, nsrm_chain_key_info, nsrm_chain_config_info)) !=OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    of_free(nsrm_chain_key_info->name_p);
    of_free(nsrm_chain_key_info->tenant_name_p);
    of_free(nsrm_chain_key_info);
    return OF_FAILURE;
  }

 
  chain_object_config_info = (struct nsrm_nschain_object_record *)of_calloc(1,sizeof(struct nsrm_nschain_object_record));
  chain_object_config_info->info = (struct nsrm_nschain_object_config_info*)of_calloc(NSRM_MAX_NSCHAIN_OBJECT_CONFIG_PARAMETERS,sizeof(struct nsrm_nschain_object_config_info));

  chain_object_config_info->key.name_p = (char *)of_calloc(1,128);
  chain_object_config_info->key.tenant_name_p = (char*)of_calloc(1,128);  

  iRes = nsrm_get_exact_nschain_object(nsrm_chain_key_info, chain_object_config_info);
  if (iRes != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: Appliacne doesn't exists with name %s",nsrm_chain_key_info->name_p);
    of_free(nsrm_chain_key_info->name_p);
    of_free(nsrm_chain_key_info->tenant_name_p);
    of_free(nsrm_chain_key_info);
    return OF_FAILURE;
  }

  CM_CBK_DEBUG_PRINT ("Exact matching record found");
  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));

  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Failed to allocate memory for result_iv_pairs_p");
    of_free(nsrm_chain_key_info->name_p);
    of_free(nsrm_chain_key_info->tenant_name_p);
    of_free(nsrm_chain_key_info);
    return OF_FAILURE;
  }
  nsrm_chain_ucm_getparams(chain_object_config_info, result_iv_pairs_p);
  *pIvPairArr = result_iv_pairs_p;
  of_free(nsrm_chain_key_info->name_p);
  of_free(nsrm_chain_key_info->tenant_name_p);
  of_free(nsrm_chain_key_info);
  return OF_SUCCESS;
}

int32_t nsrm_chain_ucm_getparams (struct nsrm_nschain_object_record *appl_chain_info,
                                   struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
   uint32_t uindex_i = 0;
   char buf[128] = "";
   CM_CBK_DEBUG_PRINT ("Entered");
#define CM_APPLIANCE_CHILD_COUNT 3

   result_iv_pairs_p->iv_pairs =(struct cm_iv_pair *) of_calloc (CM_APPLIANCE_CHILD_COUNT, sizeof (struct cm_iv_pair));
   if (result_iv_pairs_p->iv_pairs == NULL)
   {
     CM_CBK_DEBUG_PRINT("Memory allocation failed for result_iv_pairs_p->iv_pairs");
     return OF_FAILURE;
   }

   CM_CBK_DEBUG_PRINT("name : %s",appl_chain_info->key.name_p);
   CM_CBK_DEBUG_PRINT("tenant name : %s",appl_chain_info->key.tenant_name_p);

   sprintf(buf,"%s",appl_chain_info->key.name_p);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_CHAIN_NAME_ID,
		   CM_DATA_TYPE_STRING, buf);
   
   uindex_i++;

   
   sprintf(buf,"%s",appl_chain_info->key.tenant_name_p);

   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_CHAIN_TENANT_ID,
		   CM_DATA_TYPE_STRING, buf);
   uindex_i++;

  #if 0 
   CM_CBK_DEBUG_PRINT("type : %d",appl_chain_info->info[0].value.nschain_type);
   CM_CBK_DEBUG_PRINT("type : %d",appl_chain_info->info[0].value.nschain_type);

   of_memset(buf, 0, sizeof(buf));
   if(appl_chain_info->info[0].value.nschain_type == L2)
   {
      sprintf(buf,"%s","L2");
   }
   else if(appl_chain_info->info[0].value.nschain_type == L3)
   {
      sprintf(buf,"%s","L3");
   }
   
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_CHAIN_TYPE_ID,
		   CM_DATA_TYPE_STRING, buf);
   uindex_i++;
#endif

   of_memset(buf, 0, sizeof(buf));
   of_itoa (appl_chain_info->info[5].value.admin_status_e, buf);
   CM_CBK_DEBUG_PRINT("debug1");
   CM_CBK_DEBUG_PRINT("debug2");
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_CHAIN_ENABLED_ID,
		    CM_DATA_TYPE_STRING, buf);
   uindex_i++;


#if 0 

   of_memset(buf, 0, sizeof(buf));
   if(appl_chain_info->info[1].value.load_sharing_algorithm_e == HASH_BASED_ALGORITHM)
   {
      sprintf(buf,"%s","Hash_Based");
   }
   else if(appl_chain_info->info[1].value.load_sharing_algorithm_e == ROUND_ROBIN)
   {
      sprintf(buf,"%s","Round_Robin");
   }
   else if(appl_chain_info->info[1].value.load_sharing_algorithm_e == LEAST_CONNECTIONS)
   {
      sprintf(buf,"%s","Least_Connections");
   }


   CM_CBK_DEBUG_PRINT("algo :%d",appl_chain_info->info[1].value.load_sharing_algorithm_e);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_CHAIN_LOADSHAREALG_ID,
                   CM_DATA_TYPE_STRING, buf);
   uindex_i++;

 
  
   of_memset(buf, 0, sizeof(buf));
   if(appl_chain_info->info[2].value.load_indication_type_e == CONNECTION_BASED)
   {
      sprintf(buf,"%s","Connection_Based");
   }
   else if(appl_chain_info->info[2].value.load_indication_type_e == TRAFFIC_BASED)
   {
      sprintf(buf,"%s","Traffic_Based");
   }
   
   CM_CBK_DEBUG_PRINT("algo :%d",appl_chain_info->info[2].value.load_indication_type_e);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_CHAIN_LOADINDICATION_ID,
                   CM_DATA_TYPE_STRING, buf);
   uindex_i++;

 
   CM_CBK_DEBUG_PRINT("debug3");
   CM_CBK_DEBUG_PRINT("debug4");
  /*
   of_memset(buf, 0, sizeof(buf));
  // of_itoa (appl_chain_info->info[]value.packet_fields_to_hash, buf);
   CM_CBK_DEBUG_PRINT("debug5");
   CM_CBK_DEBUG_PRINT("debug6");
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_CHAIN_PKTFIELDSTOHASH_ID,
		   CM_DATA_TYPE_STRING, buf);
   uindex_i++;
  */
   of_memset(buf, 0, sizeof(buf));
   of_ltoa (appl_chain_info->info[3].value.high_threshold, buf);
   CM_CBK_DEBUG_PRINT("type : %d",appl_chain_info->info[0].value.nschain_type);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_CHAIN_HIGHTHRESHOLD_ID,
		    CM_DATA_TYPE_STRING, buf);
   CM_CBK_DEBUG_PRINT("debug7");
   uindex_i++;

   CM_CBK_DEBUG_PRINT("debug8");

   of_memset(buf, 0, sizeof(buf));
   of_ltoa (appl_chain_info->info[4].value.low_threshold, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_CHAIN_LOWTHRESHOLD_ID,
		    CM_DATA_TYPE_STRING, buf);
   uindex_i++;

#endif

   CM_CBK_DEBUG_PRINT("debug9"); 
   result_iv_pairs_p->count_ui = uindex_i;
   CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);

   return OF_SUCCESS;

}

int32_t nsrm_chain_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                                         struct nsrm_nschain_object_key* appl_chain_info,
                                         struct nsrm_nschain_object_config_info* nsrm_chain_config_info)
{
  uint32_t uiMandParamCnt;

  CM_CBK_DEBUG_PRINT ("Entered");
  appl_chain_info->name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
  appl_chain_info->tenant_name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
   for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;uiMandParamCnt++)
  {
    switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
    {
      case CM_DM_CHAIN_NAME_ID:
      CM_CBK_DEBUG_PRINT ("name_p:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
      of_strncpy (appl_chain_info->name_p,(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                 pMandParams->iv_pairs[uiMandParamCnt].value_length);
      break;

      case CM_DM_CHAIN_TENANT_ID:
      CM_CBK_DEBUG_PRINT ("tenant_name_p:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
      of_strncpy (appl_chain_info->tenant_name_p,(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                 pMandParams->iv_pairs[uiMandParamCnt].value_length);
      break;
#if 0
      case CM_DM_CHAIN_TYPE_ID:
        if (strcmp((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p, "L2")==0)
        {
          nsrm_chain_config_info[0].value.nschain_type = L2;
        }
        else if(strcmp((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p, "L3")==0)
        {
          nsrm_chain_config_info[0].value.nschain_type = L3;
        }

        CM_CBK_DEBUG_PRINT ("chain type : %d",nsrm_chain_config_info[0].value.nschain_type);
      break;

      case CM_DM_CHAIN_LOADSHAREALG_ID:
        if (strcmp((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p, "Hash_Based")==0)
        {
          nsrm_chain_config_info[1].value.load_sharing_algorithm_e = HASH_BASED_ALGORITHM;
        }
        else if(strcmp((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p, "Round_Robin")==0)
        {
          nsrm_chain_config_info[1].value.load_sharing_algorithm_e = ROUND_ROBIN;
        }
        else if(strcmp((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p, "Least_Connections")==0)
        {
          nsrm_chain_config_info[1].value.load_sharing_algorithm_e = LEAST_CONNECTIONS;
        }
        else
        {
          CM_CBK_DEBUG_PRINT ("Invalid option");
          return OF_FAILURE;
        }
      break;

      case CM_DM_CHAIN_LOADINDICATION_ID:
        if (strcmp((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p, "Connection_Based")==0)
        {
          nsrm_chain_config_info[2].value.load_indication_type_e = CONNECTION_BASED;
        }
        else if(strcmp((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p, "Traffic_Based")==0)
        {
          nsrm_chain_config_info[2].value.load_indication_type_e = TRAFFIC_BASED;
        }
        else
        {
          CM_CBK_DEBUG_PRINT ("Invalid option");
          return OF_FAILURE;
        }
      break;

#endif
     }
  }
        return OF_SUCCESS;
}

int32_t nsrm_chain_ucm_setoptparams (struct cm_array_of_iv_pairs *pOptParams,
                        struct nsrm_nschain_object_config_info* nsrm_chain_config_info)
{
  
  uint32_t uiOptParamCnt;
  CM_CBK_DEBUG_PRINT ("Entered");

   for (uiOptParamCnt = 0; uiOptParamCnt < pOptParams->count_ui; uiOptParamCnt++)
  {
    switch (pOptParams->iv_pairs[uiOptParamCnt].id_ui)
    {
#if 0
      case CM_DM_CHAIN_HIGHTHRESHOLD_ID:
        nsrm_chain_config_info[3].value.high_threshold = of_atoi((char*)pOptParams->iv_pairs[uiOptParamCnt].value_p);
        
        CM_CBK_DEBUG_PRINT ("high threshold:%d",nsrm_chain_config_info[3].value.high_threshold);
      break;

      case CM_DM_CHAIN_LOWTHRESHOLD_ID:
        nsrm_chain_config_info[4].value.low_threshold = of_atoi((char*)pOptParams->iv_pairs[uiOptParamCnt].value_p);
        
        CM_CBK_DEBUG_PRINT ("low threshold:%d",nsrm_chain_config_info[4].value.low_threshold);
      break;
#endif 
       case CM_DM_CHAIN_ENABLED_ID:
         nsrm_chain_config_info[5].value.admin_status_e = of_atoi((char *)pOptParams->iv_pairs[uiOptParamCnt].value_p);
      break;

    }
   }
     CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
     return OF_SUCCESS;

}

int32_t nsrm_chain_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
                        uint32_t command_id, struct cm_app_result ** result_p)
{
  struct cm_app_result *of_chain_result = NULL;
  int32_t return_value = OF_FAILURE;

  CM_CBK_DEBUG_PRINT ("Entered");

  return_value = nsrm_chain_ucm_validatemandparams(key_iv_pairs_p, &of_chain_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  return_value = nsrm_chain_ucm_validateoptsparams(key_iv_pairs_p, &of_chain_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate optional  Parameters Failed");
    return OF_FAILURE;
  }
  *result_p = of_chain_result;
  return OF_SUCCESS;

}


int32_t nsrm_chain_ucm_validatemandparams(struct cm_array_of_iv_pairs *mand_iv_pairs_p,
    struct cm_app_result **  presult_p)
{
  uint32_t count;
  struct cm_app_result *of_chain_result = NULL;


  CM_CBK_DEBUG_PRINT ("Entered");
  for (count = 0; count < mand_iv_pairs_p->count_ui;count++)
  {
    switch (mand_iv_pairs_p->iv_pairs[count].id_ui)
    {
      case CM_DM_CHAIN_NAME_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Appl category name is NULL");
          fill_app_result_struct (&of_chain_result, NULL, CM_GLU_NS_CHAIN_NAME_NULL);
          *presult_p = of_chain_result;
          return OF_FAILURE;
        }
        break;

       case CM_DM_CHAIN_TENANT_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Appl category name is NULL");
          fill_app_result_struct (&of_chain_result, NULL, CM_GLU_NS_CHAIN_TENANT_INVALID);
          *presult_p = of_chain_result;
          return OF_FAILURE;
        }
    break;
   }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs_p);
  return OF_SUCCESS;
}


int32_t nsrm_chain_ucm_validateoptsparams (struct cm_array_of_iv_pairs *pOptParams,
                        struct cm_app_result ** presult_p)
{
  CM_CBK_DEBUG_PRINT ("Entered");
  CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
  return OF_SUCCESS;
}



