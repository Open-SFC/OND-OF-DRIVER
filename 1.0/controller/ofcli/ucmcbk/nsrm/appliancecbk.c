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
 * File name: appliancecbk.c
 * Author:
 * Date: 
 * Description: 
*/
#include "cmincld.h"
#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "nsrm.h"

int32_t nsrm_appliance_init (void);

int32_t nsrm_appliance_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);

int32_t nsrm_appliance_modrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);


int32_t nsrm_appliance_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p);


int32_t nsrm_appliance_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);



int32_t nsrm_appliance_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p,  uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p);



int32_t nsrm_appliance_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs_p,
                                        struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t nsrm_appliance_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
                        uint32_t command_id, struct cm_app_result ** result_p);


int32_t nsrm_appliance_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                        struct nsrm_nwservice_object_key* appl_category_info,
                        struct nsrm_nwservice_object_config_info *nsrm_appliance_config_info);


int32_t nsrm_appliance_ucm_setoptparams (struct cm_array_of_iv_pairs *pOptParams,
                        struct nsrm_nwservice_object_config_info* nsrm_appliance_config_info);

int32_t nsrm_appliance_ucm_getparams (struct nsrm_nwservice_object_record *appl_appliance_info,
                                   struct cm_array_of_iv_pairs * result_iv_pairs_p);

int32_t nsrm_appliance_ucm_validatemandparams(struct cm_array_of_iv_pairs *mand_iv_pairs_p,
                                              struct cm_app_result **  presult_p);

int32_t nsrm_appliance_ucm_validateoptsparams (struct cm_array_of_iv_pairs *pOptParams,
                        struct cm_app_result ** presult_p);


struct cm_dm_app_callbacks nsrm_appliance_callbacks =
{
  NULL,
  nsrm_appliance_addrec,
  nsrm_appliance_modrec,
  nsrm_appliance_delrec,
  NULL,
  nsrm_appliance_getfirstnrecs,
  nsrm_appliance_getnextnrecs,
  nsrm_appliance_getexactrec,
  nsrm_appliance_verifycfg,
  NULL
};

int32_t nsrm_appliance_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  if (cm_register_app_callbacks (CM_ON_DIRECTOR_NSRM_SERVICE_APPL_ID, &nsrm_appliance_callbacks) != OF_SUCCESS)
    return OF_FAILURE;

  return OF_SUCCESS;
}

int32_t nsrm_appliance_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p)
{

   struct cm_app_result *nsrm_appliance_result = NULL;
   int32_t return_value = OF_FAILURE;
   struct nsrm_nwservice_object_key *nsrm_appliance_key_info;
   struct nsrm_nwservice_object_config_info *nsrm_appliance_config_info = NULL;

   CM_CBK_DEBUG_PRINT ("Entered(nsrm_appliance_addrec)");

  nsrm_appliance_key_info = (struct nsrm_nwservice_object_key *) of_calloc(1, sizeof(struct nsrm_nwservice_object_key ));
  nsrm_appliance_config_info = (struct nsrm_nwservice_object_config_info*)of_calloc(NSRM_MAX_NWSERVICE_OBJECT_CONFIG_PARAMETERS,sizeof(struct nsrm_nwservice_object_config_info));
  if ((nsrm_appliance_ucm_setmandparams (pMandParams, nsrm_appliance_key_info, nsrm_appliance_config_info)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&nsrm_appliance_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=nsrm_appliance_result;
    of_free(nsrm_appliance_key_info->name_p);
    of_free(nsrm_appliance_key_info->tenant_name_p);
    of_free(nsrm_appliance_key_info);
    return OF_FAILURE;
  }
  

  if ((nsrm_appliance_ucm_setoptparams (pOptParams, nsrm_appliance_config_info)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
    fill_app_result_struct (&nsrm_appliance_result, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
    *result_p=nsrm_appliance_result;
    of_free(nsrm_appliance_key_info->name_p);
    of_free(nsrm_appliance_key_info->tenant_name_p);
    of_free(nsrm_appliance_key_info);
    return OF_FAILURE;
  }

 CM_CBK_DEBUG_PRINT("nsrm_appliance_config_info :%d",nsrm_appliance_config_info[0].value.nwservice_object_form_factor_type_e);
 CM_CBK_DEBUG_PRINT("nsrm_appliance_config_info :%d",nsrm_appliance_config_info[1].value.admin_status_e);

  return_value =nsrm_add_nwservice_object(2, nsrm_appliance_key_info,
                                         nsrm_appliance_config_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("nsrm appliance add  Failed");
    fill_app_result_struct (&nsrm_appliance_result, NULL, CM_GLU_APPLIANCE_ADD_FAILED);
    *result_p = nsrm_appliance_result;
    of_free(nsrm_appliance_key_info->name_p);
    of_free(nsrm_appliance_key_info->tenant_name_p);
    of_free(nsrm_appliance_key_info);
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("NSRM appliance added successfully");
  of_free(nsrm_appliance_key_info->name_p);
  of_free(nsrm_appliance_key_info->tenant_name_p);
  of_free(nsrm_appliance_key_info);
  return OF_SUCCESS;
}


int32_t nsrm_appliance_modrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p)
{
   struct cm_app_result *nsrm_appliance_result = NULL;
   int32_t return_value = OF_FAILURE;
   struct nsrm_nwservice_object_key *nsrm_appliance_key_info = NULL;
   struct nsrm_nwservice_object_config_info *nsrm_appliance_config_info = NULL;

   CM_CBK_DEBUG_PRINT ("Entered(nsrm_category_addrec)");

  nsrm_appliance_key_info = (struct nsrm_nwservice_object_key *) of_calloc(1, sizeof(struct nsrm_nwservice_object_key ));
   nsrm_appliance_config_info = (struct nsrm_nwservice_object_config_info*)of_calloc(NSRM_MAX_NWSERVICE_OBJECT_CONFIG_PARAMETERS,sizeof(struct nsrm_nwservice_object_config_info));
  if ((nsrm_appliance_ucm_setmandparams (pMandParams, nsrm_appliance_key_info, nsrm_appliance_config_info)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&nsrm_appliance_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=nsrm_appliance_result;
    of_free(nsrm_appliance_key_info->name_p);
    of_free(nsrm_appliance_key_info->tenant_name_p);
    of_free(nsrm_appliance_key_info);
    return OF_FAILURE;
  }

 if ((nsrm_appliance_ucm_setoptparams (pOptParams, nsrm_appliance_config_info)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
    fill_app_result_struct (&nsrm_appliance_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=nsrm_appliance_result;
    of_free(nsrm_appliance_key_info->name_p);
    of_free(nsrm_appliance_key_info->tenant_name_p);
    of_free(nsrm_appliance_key_info);
    return OF_FAILURE;
  }

  return_value = nsrm_modify_nwservice_object(nsrm_appliance_key_info, 2, nsrm_appliance_config_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("nsrm appliance modify  Failed");
    fill_app_result_struct (&nsrm_appliance_result, NULL, CM_GLU_APPLIANCE_MOD_FAILED);
    *result_p = nsrm_appliance_result;
    of_free(nsrm_appliance_key_info->name_p);
    of_free(nsrm_appliance_key_info->tenant_name_p);
    of_free(nsrm_appliance_key_info);
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("NSRM appliance modified successfully");
  of_free(nsrm_appliance_key_info->name_p);
  of_free(nsrm_appliance_key_info->tenant_name_p);
  of_free(nsrm_appliance_key_info);
  return OF_SUCCESS;
}



int32_t nsrm_appliance_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p)
{

   struct cm_app_result *nsrm_appliance_result = NULL;
   int32_t return_value = OF_FAILURE;

   struct nsrm_nwservice_object_key *appliance_info = NULL;
   struct nsrm_nwservice_object_config_info *nsrm_appliance_config_info = NULL;

  CM_CBK_DEBUG_PRINT ("Entered");
  appliance_info = (struct nsrm_nwservice_object_key *) of_calloc(1, sizeof(struct nsrm_nwservice_object_key));
  if ((nsrm_appliance_ucm_setmandparams (keys_arr_p, appliance_info, nsrm_appliance_config_info)) !=OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&nsrm_appliance_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p = nsrm_appliance_result;
    of_free(appliance_info->name_p);
    of_free(appliance_info->tenant_name_p);
    of_free(appliance_info);
    return OF_FAILURE;
  }
  return_value = nsrm_del_nwservice_object(appliance_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("nsrm appliance del  Failed");
    fill_app_result_struct (&nsrm_appliance_result, NULL, CM_GLU_APPLIANCE_DEL_FAILED);
    *result_p = nsrm_appliance_result;
    of_free(appliance_info->name_p);
    of_free(appliance_info->tenant_name_p);
    of_free(appliance_info);
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("NSRM Appliance delete successfully");
  of_free(appliance_info->name_p);
  of_free(appliance_info->tenant_name_p);
  of_free(appliance_info);
  return OF_SUCCESS;

}

int32_t nsrm_appliance_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  int32_t uiRecquestedCount = 1, uiReturnedCnt=0;
  struct nsrm_nwservice_object_record *nsrm_appliance_config_info=NULL;

  CM_CBK_DEBUG_PRINT ("Entered");

  nsrm_appliance_config_info = (struct  nsrm_nwservice_object_record*)of_calloc(1,sizeof(struct  nsrm_nwservice_object_record));
  nsrm_appliance_config_info->info = (struct nsrm_nwservice_object_config_info*)of_calloc(NSRM_MAX_NWSERVICE_OBJECT_CONFIG_PARAMETERS,sizeof(struct nsrm_nwservice_object_config_info));

  nsrm_appliance_config_info->key.name_p = (char*)of_calloc(1,128);
  nsrm_appliance_config_info->key.tenant_name_p = (char*)of_calloc(1,128);
  
 return_value = nsrm_get_first_nwservice_objects(uiRecquestedCount, 
                 &uiReturnedCnt, nsrm_appliance_config_info);
 if (return_value != OF_SUCCESS)
 {
   CM_CBK_DEBUG_PRINT ("Get first record failed for NSRM appliance  Table");
   return OF_FAILURE;
 }

  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
 if (result_iv_pairs_p == NULL)
 {
   CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
   of_free(nsrm_appliance_config_info->key.name_p);
   of_free(nsrm_appliance_config_info->key.tenant_name_p);
   of_free(nsrm_appliance_config_info);
   of_free(nsrm_appliance_config_info->info);
   return OF_FAILURE;
 }
  nsrm_appliance_ucm_getparams (nsrm_appliance_config_info, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
 *array_of_iv_pair_arr_p = result_iv_pairs_p;
  of_free(nsrm_appliance_config_info->key.name_p);
  of_free(nsrm_appliance_config_info->key.tenant_name_p);
  of_free(nsrm_appliance_config_info);
  of_free(nsrm_appliance_config_info->info);
  return OF_SUCCESS;
}


int32_t nsrm_appliance_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p,  uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  int32_t uiRecquestedCount = 1, uiReturnedCnt=0;
  struct nsrm_nwservice_object_key *appliance_info;
  struct nsrm_nwservice_object_record *nsrm_appliance_config_info = NULL;

  nsrm_appliance_config_info = (struct  nsrm_nwservice_object_record*)of_calloc(1,sizeof(struct  nsrm_nwservice_object_record));
  nsrm_appliance_config_info->info = (struct nsrm_nwservice_object_config_info*)of_calloc(NSRM_MAX_NWSERVICE_OBJECT_CONFIG_PARAMETERS,sizeof(struct nsrm_nwservice_object_config_info));

  nsrm_appliance_config_info->key.name_p = (char*)of_calloc(1,128);
  nsrm_appliance_config_info->key.tenant_name_p = (char*)of_calloc(1,128);
  
  CM_CBK_DEBUG_PRINT ("Entered");

  appliance_info = (struct nsrm_nwservice_object_key *) of_calloc(1, sizeof(struct nsrm_nwservice_object_key));
  if ((nsrm_appliance_ucm_setmandparams (prev_record_key_p, appliance_info, nsrm_appliance_config_info)) != OF_SUCCESS)
  {
     CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
     return OF_FAILURE;
  }

 return_value = nsrm_get_next_nwservice_object(appliance_info, uiRecquestedCount, 
                       &uiReturnedCnt, nsrm_appliance_config_info);
 if (return_value != OF_SUCCESS)
 {
   CM_CBK_DEBUG_PRINT ("Get next record failed for NSRM appl  Table");
   of_free(appliance_info->name_p);
   of_free(appliance_info->tenant_name_p);
   of_free(appliance_info);
   return OF_FAILURE;
 }

  CM_CBK_DEBUG_PRINT ("name:%s",nsrm_appliance_config_info->key.name_p);
  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
 if (result_iv_pairs_p == NULL)
 {
   CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    of_free(nsrm_appliance_config_info->key.name_p);
    of_free(nsrm_appliance_config_info->key.tenant_name_p);
    of_free(nsrm_appliance_config_info);
    of_free(nsrm_appliance_config_info->info);
    of_free(appliance_info->name_p);
    of_free(appliance_info->tenant_name_p);
    of_free(appliance_info);
    return OF_FAILURE;
 }
  nsrm_appliance_ucm_getparams (nsrm_appliance_config_info, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
 *next_n_record_data_p = result_iv_pairs_p;
 *count_p = uiRecCount;
  of_free(nsrm_appliance_config_info->key.name_p);
  of_free(nsrm_appliance_config_info->key.tenant_name_p);
  of_free(nsrm_appliance_config_info);
  of_free(nsrm_appliance_config_info->info);
  of_free(appliance_info->name_p);
  of_free(appliance_info->tenant_name_p);
  of_free(appliance_info);
  return OF_SUCCESS;
}

int32_t nsrm_appliance_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs ** pIvPairArr)

{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct nsrm_nwservice_object_key *appl_config_info;
  int32_t iRes = OF_FAILURE;
  struct nsrm_nwservice_object_record *appl_object_config_info = NULL;
   struct nsrm_nwservice_object_config_info *nsrm_appliance_config_info = NULL;

  CM_CBK_DEBUG_PRINT ("Entered");
  appl_config_info = (struct nsrm_nwservice_object_key *) of_calloc(1, sizeof(struct nsrm_nwservice_object_key));
   appl_object_config_info = (struct  nsrm_nwservice_object_record*)of_calloc(1,sizeof(struct  nsrm_nwservice_object_record));
  appl_object_config_info->info = (struct nsrm_nwservice_object_config_info*)of_calloc(NSRM_MAX_NWSERVICE_OBJECT_CONFIG_PARAMETERS,sizeof(struct nsrm_nwservice_object_config_info));
  if((nsrm_appliance_ucm_setmandparams(key_iv_pairs_p, appl_config_info, nsrm_appliance_config_info)) !=OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }

   CM_CBK_DEBUG_PRINT("name : %s",appl_config_info->name_p);
   CM_CBK_DEBUG_PRINT("teant name : %s",appl_config_info->tenant_name_p);


  appl_object_config_info->key.name_p = (char*)of_calloc(1,128);
  appl_object_config_info->key.tenant_name_p = (char*)of_calloc(1,128);
  
 iRes = nsrm_get_exact_nwservice_object(appl_config_info, appl_object_config_info);

  if (iRes != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: Appliacne doesn't exists with name %s",appl_config_info->name_p);
    of_free(appl_config_info->name_p);
    of_free(appl_config_info->tenant_name_p);
    of_free(appl_config_info);
    return OF_FAILURE;
  }

  CM_CBK_DEBUG_PRINT ("Exact matching record found");
  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));

  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Failed to allocate memory for result_iv_pairs_p");
    of_free(appl_config_info->name_p);
    of_free(appl_config_info->tenant_name_p);
    of_free(appl_config_info);
    /*of_free(&appl_object_config_info->key.name_p);
    of_free(appl_object_config_info->key.tenant_name_p);
    of_free(&appl_object_config_info);*/
    return OF_FAILURE;
  }
  nsrm_appliance_ucm_getparams(appl_object_config_info, result_iv_pairs_p);
  *pIvPairArr = result_iv_pairs_p;
                                                                                       
  of_free(appl_config_info->name_p);
  of_free(appl_config_info->tenant_name_p);
  of_free(appl_config_info);
/*/
  of_free(appl_object_config_info->key.name_p);
  of_free(appl_object_config_info->key.tenant_name_p);
  of_free(appl_object_config_info);
*/
  return OF_SUCCESS;
}

int32_t nsrm_appliance_ucm_getparams (struct nsrm_nwservice_object_record *appl_appliance_info,
                                   struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
   uint32_t uindex_i = 0;
   char buf[128] = "";
   CM_CBK_DEBUG_PRINT ("Entered");
#define CM_APPLIANCE_CHILD_COUNT 8

   result_iv_pairs_p->iv_pairs =(struct cm_iv_pair *) of_calloc (CM_APPLIANCE_CHILD_COUNT, sizeof (struct cm_iv_pair));
   if (result_iv_pairs_p->iv_pairs == NULL)
   {
     CM_CBK_DEBUG_PRINT("Memory allocation failed for result_iv_pairs_p->iv_pairs");
     return OF_FAILURE;
   } 

   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_SERVICE_NAME_ID,
		   CM_DATA_TYPE_STRING, appl_appliance_info->key.name_p);
   CM_CBK_DEBUG_PRINT("name : %s",result_iv_pairs_p->iv_pairs[uindex_i].value_p);
   uindex_i++;

   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_SERVICE_TENANT_ID,
		   CM_DATA_TYPE_STRING, appl_appliance_info->key.tenant_name_p);
   CM_CBK_DEBUG_PRINT("tenant name : %s",result_iv_pairs_p->iv_pairs[uindex_i].value_p);
   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   CM_CBK_DEBUG_PRINT("form : %d",appl_appliance_info->info[0].value.nwservice_object_form_factor_type_e);
   if(appl_appliance_info->info[0].value.nwservice_object_form_factor_type_e == PHYSICAL)
   { 
      sprintf(buf,"%s","Physical");
   }
   else if(appl_appliance_info->info[0].value.nwservice_object_form_factor_type_e == VIRTUAL)
   {
      sprintf(buf,"%s","Virtual");
   }
   CM_CBK_DEBUG_PRINT("form : %d",appl_appliance_info->info[0].value.nwservice_object_form_factor_type_e);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_SERVICE_FORMFACTORTYPE_ID,
		   CM_DATA_TYPE_STRING, buf);
   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   sprintf(buf,"%d",appl_appliance_info->info[1].value.admin_status_e);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_SERVICE_ENABLED_ID,
		   CM_DATA_TYPE_BOOLEAN, buf);
   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   if(appl_appliance_info->info[1].value.load_sharing_algorithm_e == HASH_BASED_ALGORITHM)
   {
      sprintf(buf,"%s","Hash_Based");
   }
   else if(appl_appliance_info->info[1].value.load_sharing_algorithm_e == ROUND_ROBIN)
   {
      sprintf(buf,"%s","Round_Robin");
   }
   else if(appl_appliance_info->info[1].value.load_sharing_algorithm_e == LEAST_CONNECTIONS)
   {
      sprintf(buf,"%s","Least_Connections");
   }


   CM_CBK_DEBUG_PRINT("algo :%d",appl_appliance_info->info[1].value.load_sharing_algorithm_e);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_SERVICE_LOADSHAREALG_ID,
                   CM_DATA_TYPE_STRING, buf);
   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   if(appl_appliance_info->info[2].value.load_indication_type_e == CONNECTION_BASED)
   {
      sprintf(buf,"%s","Connection_Based");
   }
   else if(appl_appliance_info->info[2].value.load_indication_type_e == TRAFFIC_BASED)
   {
      sprintf(buf,"%s","Traffic_Based");
   }
   
   CM_CBK_DEBUG_PRINT("algo :%d",appl_appliance_info->info[2].value.load_indication_type_e);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_SERVICE_LOADINDICATION_ID,
                   CM_DATA_TYPE_STRING, buf);
   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   of_ltoa (appl_appliance_info->info[3].value.high_threshold, buf);
   CM_CBK_DEBUG_PRINT("type : %d",appl_appliance_info->info[0].value.nwservice_type_e);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_SERVICE_HIGHTHRESHOLD_ID,
		    CM_DATA_TYPE_STRING, buf);
   uindex_i++;

   of_memset(buf, 0, sizeof(buf));
   of_ltoa (appl_appliance_info->info[4].value.low_threshold, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_SERVICE_LOWTHRESHOLD_ID,
		    CM_DATA_TYPE_STRING, buf);
   uindex_i++;

   result_iv_pairs_p->count_ui = uindex_i;
   CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);

   return OF_SUCCESS;

}

int32_t nsrm_appliance_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                                         struct nsrm_nwservice_object_key* appl_appliance_info, 
                                         struct nsrm_nwservice_object_config_info *nsrm_appliance_config_info)
{
  uint32_t uiMandParamCnt;

  CM_CBK_DEBUG_PRINT ("Entered");
 
  appl_appliance_info->name_p = (char *) of_calloc(1, 128);
  appl_appliance_info->tenant_name_p = (char *) of_calloc(1, 128);
  for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;uiMandParamCnt++)
  {
    switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
    {
      case CM_DM_SERVICE_NAME_ID:
      CM_CBK_DEBUG_PRINT ("name_p:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
      of_strncpy (appl_appliance_info->name_p, (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                 pMandParams->iv_pairs[uiMandParamCnt].value_length);
      break;

      case CM_DM_SERVICE_TENANT_ID:
      CM_CBK_DEBUG_PRINT ("name_p:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
      of_strncpy (appl_appliance_info->tenant_name_p, (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                 pMandParams->iv_pairs[uiMandParamCnt].value_length);
      break;

#if 1
      case CM_DM_SERVICE_TYPE_ID:
        if (strcmp((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p, "L2")==0)
        {
          nsrm_appliance_config_info[0].value.nwservice_type_e = L2;
        }
        else if(strcmp((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p, "L3")==0)
        {
          nsrm_appliance_config_info[0].value.nwservice_type_e = L3;
        }

        CM_CBK_DEBUG_PRINT ("chain type : %d",nsrm_appliance_config_info[0].value.nwservice_type_e);
      break;

      case CM_DM_SERVICE_LOADSHAREALG_ID:
        if (strcmp((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p, "Hash_Based")==0)
        {
          nsrm_appliance_config_info[1].value.load_sharing_algorithm_e = HASH_BASED_ALGORITHM;
        }
        else if(strcmp((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p, "Round_Robin")==0)
        {
          nsrm_appliance_config_info[1].value.load_sharing_algorithm_e = ROUND_ROBIN;
        }
        else if(strcmp((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p, "Least_Connections")==0)
        {
          nsrm_appliance_config_info[1].value.load_sharing_algorithm_e = LEAST_CONNECTIONS;
        }
        else
        {
          CM_CBK_DEBUG_PRINT ("Invalid option");
          return OF_FAILURE;
        }
      break;

      case CM_DM_SERVICE_LOADINDICATION_ID:
        if (strcmp((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p, "Connection_Based")==0)
        {
          nsrm_appliance_config_info[2].value.load_indication_type_e = CONNECTION_BASED;
        }
        else if(strcmp((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p, "Traffic_Based")==0)
        {
          nsrm_appliance_config_info[2].value.load_indication_type_e = TRAFFIC_BASED;
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

		
  

int32_t nsrm_appliance_ucm_setoptparams (struct cm_array_of_iv_pairs *pOptParams,
                        struct nsrm_nwservice_object_config_info* nsrm_appliance_config_info)
{
  
  uint32_t uiOptParamCnt;
  CM_CBK_DEBUG_PRINT ("Entered");
  for (uiOptParamCnt = 0; uiOptParamCnt < pOptParams->count_ui; uiOptParamCnt++)
  {
    switch (pOptParams->iv_pairs[uiOptParamCnt].id_ui)
    {
      case CM_DM_SERVICE_FORMFACTORTYPE_ID:
       // nsrm_appliance_config_info[0].value.nwservice_object_form_factor_type_e = 
         //             of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p);
         if(!strcmp((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p,"Physical"))
        {
          nsrm_appliance_config_info[0].value.nwservice_object_form_factor_type_e = PHYSICAL;
        }
        else if(!strcmp((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p,"Virtual"))
        {
          nsrm_appliance_config_info[0].value.nwservice_object_form_factor_type_e = VIRTUAL;
        }

      CM_CBK_DEBUG_PRINT("factor : %d",nsrm_appliance_config_info[0].value.nwservice_object_form_factor_type_e);
      
      break;

       case CM_DM_SERVICE_ENABLED_ID:
         nsrm_appliance_config_info[1].value.admin_status_e = 
                      of_atoi((char *)pOptParams->iv_pairs[uiOptParamCnt].value_p);
     CM_CBK_DEBUG_PRINT("enabled : %d",nsrm_appliance_config_info[1].value.admin_status_e);
      break;

#if 1
      case CM_DM_SERVICE_HIGHTHRESHOLD_ID:
        nsrm_appliance_config_info[3].value.high_threshold = of_atoi((char*)pOptParams->iv_pairs[uiOptParamCnt].value_p);
        
        CM_CBK_DEBUG_PRINT ("high threshold:%d",nsrm_appliance_config_info[3].value.high_threshold);
      break;

      case CM_DM_SERVICE_LOWTHRESHOLD_ID:
        nsrm_appliance_config_info[4].value.low_threshold = of_atoi((char*)pOptParams->iv_pairs[uiOptParamCnt].value_p);
        
        CM_CBK_DEBUG_PRINT ("low threshold:%d",nsrm_appliance_config_info[4].value.low_threshold);
      break;
#endif 
    }
   }
     CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
     return OF_SUCCESS;

}


int32_t nsrm_appliance_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
                        uint32_t command_id, struct cm_app_result ** result_p)
{
  struct cm_app_result *of_appliance_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct nsrm_nwservice_object_key   appl_config_info={};

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&appl_config_info, 0, sizeof (appl_config_info));

  return_value = nsrm_appliance_ucm_validatemandparams(key_iv_pairs_p, &of_appliance_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  return_value = nsrm_appliance_ucm_validateoptsparams(key_iv_pairs_p, &of_appliance_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate optional  Parameters Failed");
    return OF_FAILURE;
  }
  *result_p = of_appliance_result;
  return OF_SUCCESS;

}


int32_t nsrm_appliance_ucm_validatemandparams(struct cm_array_of_iv_pairs *mand_iv_pairs_p,
    struct cm_app_result **  presult_p)
{
  uint32_t count;
  struct cm_app_result *of_appliance_result = NULL;


  CM_CBK_DEBUG_PRINT ("Entered");
  for (count = 0; count < mand_iv_pairs_p->count_ui;count++)
  {
    switch (mand_iv_pairs_p->iv_pairs[count].id_ui)
    {
      case CM_DM_SERVICE_NAME_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Appl category name is NULL");
          fill_app_result_struct (&of_appliance_result, NULL, CM_GLU_APPLIANCE_NAME_NULL);
          *presult_p = of_appliance_result;
          return OF_FAILURE;
        }
       case CM_DM_SERVICE_TENANT_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Appl tenant name is NULL");
          fill_app_result_struct (&of_appliance_result, NULL, CM_GLU_APPLIANCE_NAME_NULL);
          *presult_p = of_appliance_result;
          return OF_FAILURE;
        }
        break;
       break;
   }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs_p);
  return OF_SUCCESS;
}


int32_t nsrm_appliance_ucm_validateoptsparams (struct cm_array_of_iv_pairs *pOptParams,
                        struct cm_app_result ** presult_p)
{
  CM_CBK_DEBUG_PRINT ("Entered");
  CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
  return OF_SUCCESS;
}



