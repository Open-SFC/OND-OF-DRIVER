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
 * File name: attachsrvchain.c
 * Author:
 * Date: 
 * Description: 
*/
#include "cmincld.h"
#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "nsrm.h"

int32_t nsrm_attach_nwservice_to_nschain_init (void);

int32_t nsrm_attach_nwservice_to_nschain_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);

int32_t nsrm_attach_nwservice_to_nschain_modrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);


int32_t nsrm_attach_nwservice_to_nschain_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p);


int32_t nsrm_attach_nwservice_to_nschain_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);



int32_t nsrm_attach_nwservice_to_nschain_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p,  uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p);



int32_t nsrm_attach_nwservice_to_nschain_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs_p,
                                        struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t nsrm_attach_nwservice_to_nschain_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
                        uint32_t command_id, struct cm_app_result ** result_p);


int32_t nsrm_nwsrv_obj_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                        struct nsrm_nwservice_object_key *nwsrv_config_info,
                        struct nsrm_nwservice_attach_key* nwsrv_category_info,
                        struct nsrm_nschain_object_key*   chain_obj_info_p
                        );

int32_t nsrm_nwsrv_obj_getparams (struct nsrm_nwservice_attach_key *nwsrv_nwsrv_obj_info,
                                   struct cm_array_of_iv_pairs * result_iv_pairs_p);

int32_t nsrm_nwsrv_obj_validatemandparams(struct cm_array_of_iv_pairs *mand_iv_pairs_p,
                                          struct cm_app_result **presult_p);

struct cm_dm_app_callbacks nsrm_attach_nwservice_to_nschain_callbacks =
{
  NULL,
  nsrm_attach_nwservice_to_nschain_addrec,
  NULL,
  nsrm_attach_nwservice_to_nschain_delrec,
  NULL,
  nsrm_attach_nwservice_to_nschain_getfirstnrecs,
  nsrm_attach_nwservice_to_nschain_getnextnrecs,
  nsrm_attach_nwservice_to_nschain_getexactrec,
  nsrm_attach_nwservice_to_nschain_verifycfg,
  NULL
};

int32_t nsrm_attach_nwservice_to_nschain_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  if (cm_register_app_callbacks (CM_ON_DIRECTOR_NSRM_CHAIN_SERVICES_APPL_ID, 
              &nsrm_attach_nwservice_to_nschain_callbacks) != OF_SUCCESS)
    return OF_FAILURE;

  return OF_SUCCESS;
}

int32_t nsrm_attach_nwservice_to_nschain_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p)
{

   struct  cm_app_result *nsrm_nwsrv_obj_result = NULL;
   int32_t return_value = OF_FAILURE;
   struct nsrm_nwservice_object_key *nwsrv_config_info = NULL;
   struct  nsrm_nwservice_attach_key *nsrm_nwsrv_obj_key_info;
   struct  nsrm_nschain_object_key*   chain_obj_info_p;

   CM_CBK_DEBUG_PRINT ("Entered(nsrm_nwsrv_obj_addrec)");
 
  nwsrv_config_info = (struct nsrm_nwservice_object_key*)of_calloc(1,sizeof(struct nsrm_nwservice_object_key));
  nsrm_nwsrv_obj_key_info = (struct nsrm_nwservice_attach_key *) 
                            of_calloc(1, sizeof(struct nsrm_nwservice_attach_key ));
  chain_obj_info_p = (struct nsrm_nschain_object_key*) 
             of_calloc(1, sizeof(struct nsrm_nschain_object_key));
  if ((nsrm_nwsrv_obj_setmandparams (pMandParams,nwsrv_config_info, nsrm_nwsrv_obj_key_info, 
                  chain_obj_info_p)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&nsrm_nwsrv_obj_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=nsrm_nwsrv_obj_result;
    of_free(nsrm_nwsrv_obj_key_info->name_p);
    of_free(nsrm_nwsrv_obj_key_info);
    return OF_FAILURE;
  }
 
  return_value = nsrm_attach_nwservice_object_to_nschain_object(nsrm_nwsrv_obj_key_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("nsrm nwsrv_obj add  Failed");
    fill_app_result_struct (&nsrm_nwsrv_obj_result, NULL, CM_GLU_APPLIANCE_ADD_FAILED);
    *result_p = nsrm_nwsrv_obj_result;
    of_free(nsrm_nwsrv_obj_key_info->name_p);
    of_free(nsrm_nwsrv_obj_key_info);
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("NSRM nwsrv_obj added successfully");
  of_free(nsrm_nwsrv_obj_key_info->name_p);
  of_free(nsrm_nwsrv_obj_key_info);
  return OF_SUCCESS;
}


int32_t nsrm_attach_nwservice_to_nschain_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p)
{

   struct cm_app_result *nsrm_nwsrv_obj_result = NULL;
   int32_t return_value = OF_FAILURE;

   struct nsrm_nwservice_object_key *nwsrv_config_info = NULL;
   struct nsrm_nwservice_attach_key *nwsrv_obj_info = NULL;
   struct nsrm_nschain_object_key*   chain_obj_info_p;

  CM_CBK_DEBUG_PRINT ("Entered");

  nwsrv_obj_info = (struct nsrm_nwservice_attach_key*)of_calloc(1,sizeof(struct nsrm_nwservice_attach_key));
  nwsrv_config_info = (struct nsrm_nwservice_object_key *) 
                 of_calloc(1, sizeof(struct nsrm_nwservice_object_key));
  chain_obj_info_p = (struct nsrm_nschain_object_key*) 
             of_calloc(1, sizeof(struct nsrm_nschain_object_key));
  if ((nsrm_nwsrv_obj_setmandparams (keys_arr_p,nwsrv_config_info, nwsrv_obj_info, 
                    chain_obj_info_p)) !=OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&nsrm_nwsrv_obj_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p = nsrm_nwsrv_obj_result;
    of_free(nwsrv_obj_info->name_p);
    of_free(nwsrv_obj_info);
    return OF_FAILURE;
  }
  return_value = nsrm_detach_nwservice_object_from_nschain_object(nwsrv_obj_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("nsrm nwsrv_obj del  Failed");
    fill_app_result_struct (&nsrm_nwsrv_obj_result, NULL, CM_GLU_APPLIANCE_DEL_FAILED);
    *result_p = nsrm_nwsrv_obj_result;
    of_free(nwsrv_obj_info->name_p);
    of_free(nwsrv_obj_info);
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("NSRM Appliance delete successfully");
  of_free(nwsrv_obj_info->name_p);
  of_free(nwsrv_obj_info);
  return OF_SUCCESS;

}

int32_t nsrm_attach_nwservice_to_nschain_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  int32_t uiRecquestedCount = 1, uiReturnedCnt=0;
  struct nsrm_nwservice_object_key *nwsrv_config_info = NULL;
  struct nsrm_nwservice_attach_key *nsrm_nwsrv_obj_config_info=NULL;
  struct nsrm_nschain_object_key *chain_obj_info_p;
  struct nsrm_nwservice_attach_key *nwsrv_obj_info = NULL;


  nwsrv_config_info = (struct nsrm_nwservice_object_key *)
             of_calloc(1, sizeof(struct nsrm_nwservice_object_key));
  CM_CBK_DEBUG_PRINT ("Entered");

  nwsrv_obj_info = (struct nsrm_nwservice_attach_key *) 
                 of_calloc(1, sizeof(struct nsrm_nwservice_attach_key));
  chain_obj_info_p = (struct nsrm_nschain_object_key*) 
             of_calloc(1, sizeof(struct nsrm_nschain_object_key));
    if ((nsrm_nwsrv_obj_setmandparams (keys_arr_p, nwsrv_config_info,nwsrv_obj_info, 
                    chain_obj_info_p)) !=OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    of_free(nwsrv_obj_info->name_p);
    of_free(nwsrv_obj_info);
    return OF_FAILURE;
 
  }
  CM_CBK_DEBUG_PRINT("chain name : %s",chain_obj_info_p->name_p);
  nsrm_nwsrv_obj_config_info = (struct  nsrm_nwservice_attach_key*)
                 of_calloc(1,sizeof(struct  nsrm_nwservice_attach_key));
  nsrm_nwsrv_obj_config_info->name_p = (char*)of_calloc(1,128);  
  
 return_value = nsrm_get_first_nwservice_objects_in_nschain_object(chain_obj_info_p,
                 uiRecquestedCount, &uiReturnedCnt, nsrm_nwsrv_obj_config_info);
 if (return_value != OF_SUCCESS)
 {
   CM_CBK_DEBUG_PRINT ("Get first record failed for NSRM nwsrv_obj  Table");
   return OF_FAILURE;
 }

  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) 
               of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
 if (result_iv_pairs_p == NULL)
 {
   CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
   of_free(nsrm_nwsrv_obj_config_info->name_p);
   of_free(nsrm_nwsrv_obj_config_info);
   return OF_FAILURE;
 }
  nsrm_nwsrv_obj_getparams (nsrm_nwsrv_obj_config_info, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
 *array_of_iv_pair_arr_p = result_iv_pairs_p;
  of_free(nsrm_nwsrv_obj_config_info->name_p);
  of_free(nsrm_nwsrv_obj_config_info);
  return OF_SUCCESS;
}


int32_t nsrm_attach_nwservice_to_nschain_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p,  uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  int32_t uiRecquestedCount = 1, uiReturnedCnt=0;
  struct nsrm_nwservice_object_key *nwsrv_config_info = NULL;
  struct nsrm_nwservice_attach_key *nwsrv_obj_info;
  struct nsrm_nwservice_attach_key *nsrm_nwsrv_obj_config_info = NULL;
  struct nsrm_nschain_object_key*   chain_obj_info_p;
 
  nwsrv_config_info = (struct nsrm_nwservice_object_key *)
                      of_calloc(1, sizeof(struct nsrm_nwservice_object_key));
  nsrm_nwsrv_obj_config_info = (struct  nsrm_nwservice_attach_key*)
                 of_calloc(1,sizeof(struct  nsrm_nwservice_attach_key));
  nsrm_nwsrv_obj_config_info->name_p = (char*)of_calloc(1,128);

  CM_CBK_DEBUG_PRINT ("Entered");

  nwsrv_obj_info = (struct nsrm_nwservice_attach_key *) 
               of_calloc(1, sizeof(struct nsrm_nwservice_attach_key));
  chain_obj_info_p = (struct nsrm_nschain_object_key*) 
             of_calloc(1, sizeof(struct nsrm_nschain_object_key));
  nsrm_nwsrv_obj_setmandparams(keys_arr_p, nwsrv_config_info,nwsrv_obj_info,
                                chain_obj_info_p);
  if ((nsrm_nwsrv_obj_setmandparams(prev_record_key_p, nwsrv_config_info,nwsrv_obj_info,
                                chain_obj_info_p)) != OF_SUCCESS)
  {
     CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
     return OF_FAILURE;
  }
 CM_CBK_DEBUG_PRINT("after setmand in get next");
 CM_CBK_DEBUG_PRINT("chain name : %s",chain_obj_info_p->name_p);
 return_value = nsrm_get_next_nwservice_object_in_nschain_object(chain_obj_info_p,nwsrv_config_info, 
                       uiRecquestedCount, &uiReturnedCnt, nsrm_nwsrv_obj_config_info);
 if (return_value != OF_SUCCESS)
 {
   CM_CBK_DEBUG_PRINT ("Get next record failed for NSRM nwsrv  Table");
   of_free(nwsrv_obj_info->name_p);
   of_free(nwsrv_obj_info);
   return OF_FAILURE;
 }

  CM_CBK_DEBUG_PRINT ("name:%s",nsrm_nwsrv_obj_config_info->name_p);
  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) 
              of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    of_free(nsrm_nwsrv_obj_config_info->name_p);
    of_free(nsrm_nwsrv_obj_config_info);
    of_free(nwsrv_obj_info->name_p);
    of_free(nwsrv_obj_info);
    return OF_FAILURE;
  }
  nsrm_nwsrv_obj_getparams (nsrm_nwsrv_obj_config_info, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *next_n_record_data_p = result_iv_pairs_p;
  *count_p = uiRecCount;
  of_free(nsrm_nwsrv_obj_config_info->name_p);
  of_free(nsrm_nwsrv_obj_config_info);
  of_free(nwsrv_obj_info->name_p);
  of_free(nwsrv_obj_info);
  return OF_SUCCESS;
}

int32_t nsrm_attach_nwservice_to_nschain_getexactrec(struct cm_array_of_iv_pairs 
                                 *key_iv_pairs_p,
                                 struct cm_array_of_iv_pairs ** pIvPairArr)

{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct nsrm_nwservice_object_key *nwsrv_config_info = NULL;
  struct nsrm_nwservice_attach_key *nwsrv_attach_info = NULL;
  int32_t iRes = OF_FAILURE;
  struct nsrm_nwservice_attach_key *nwsrv_object_config_info = NULL;
  struct nsrm_nschain_object_key*   chain_obj_info_p;

  CM_CBK_DEBUG_PRINT ("Entered");
  nwsrv_config_info = (struct nsrm_nwservice_object_key *) 
             of_calloc(1, sizeof(struct nsrm_nwservice_object_key));
  chain_obj_info_p = (struct nsrm_nschain_object_key*) 
             of_calloc(1, sizeof(struct nsrm_nschain_object_key));
  nwsrv_attach_info = (struct nsrm_nwservice_attach_key*)of_calloc(1,sizeof(struct nsrm_nwservice_attach_key));
  if((nsrm_nwsrv_obj_setmandparams(key_iv_pairs_p, nwsrv_config_info,nwsrv_attach_info,
                            chain_obj_info_p)) !=OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("After set mand in get exact");
  nwsrv_object_config_info = (struct  nsrm_nwservice_attach_key*)
                        of_calloc(1,sizeof(struct  nsrm_nwservice_attach_key));

  nwsrv_object_config_info->name_p = (char*)of_calloc(1,128);
  CM_CBK_DEBUG_PRINT("chain name : %s",chain_obj_info_p->name_p);
  iRes = nsrm_get_exact_nwservice_object_in_nschain_object(chain_obj_info_p, nwsrv_config_info, 
                    nwsrv_object_config_info);

  if (iRes != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: Appliacne doesn't exists with name %s",nwsrv_config_info->name_p);
    of_free(nwsrv_config_info->name_p);
    of_free(nwsrv_config_info->tenant_name_p);
    of_free(nwsrv_config_info);
    return OF_FAILURE;
  }

  CM_CBK_DEBUG_PRINT ("Exact matching record found");
  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));

  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Failed to allocate memory for result_iv_pairs_p");
    of_free(nwsrv_config_info->name_p);
    of_free(nwsrv_config_info->tenant_name_p);
    of_free(nwsrv_config_info);
    return OF_FAILURE;
  }
  nsrm_nwsrv_obj_getparams(nwsrv_object_config_info, result_iv_pairs_p);
  *pIvPairArr = result_iv_pairs_p;
                                                                                       
  of_free(nwsrv_config_info->name_p);
  of_free(nwsrv_config_info->tenant_name_p);
  of_free(nwsrv_config_info);
  return OF_SUCCESS;
}

int32_t nsrm_nwsrv_obj_getparams (struct nsrm_nwservice_attach_key 
                                   *nwsrv_nwsrv_obj_info,
                                   struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
   uint32_t uindex_i = 0;
   char buf[128] = "";
   CM_CBK_DEBUG_PRINT ("Entered");
#define CM_APPLIANCE_CHILD_COUNT 2

   result_iv_pairs_p->iv_pairs =(struct cm_iv_pair *) 
            of_calloc (CM_APPLIANCE_CHILD_COUNT, sizeof (struct cm_iv_pair));
   if (result_iv_pairs_p->iv_pairs == NULL)
   {
     CM_CBK_DEBUG_PRINT("Memory allocation failed for result_iv_pairs_p->iv_pairs");
     return OF_FAILURE;
   } 

   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_SERVICES_SRVNAME_ID,
		   CM_DATA_TYPE_STRING, nwsrv_nwsrv_obj_info->name_p);
   CM_CBK_DEBUG_PRINT("name : %s",result_iv_pairs_p->iv_pairs[uindex_i].value_p);
   uindex_i++;

 
   of_memset(buf,0,sizeof(buf));
   of_itoa(nwsrv_nwsrv_obj_info->sequence_number,buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_SERVICES_SEQUENCENUMBER_ID,
                   CM_DATA_TYPE_STRING, buf);
   CM_CBK_DEBUG_PRINT("seqno : %s",result_iv_pairs_p->iv_pairs[uindex_i].value_p);
   uindex_i++;

   result_iv_pairs_p->count_ui = uindex_i;
   CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);



   return OF_SUCCESS;

}

int32_t nsrm_nwsrv_obj_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                           struct nsrm_nwservice_object_key *nwsrv_config_info,
                           struct nsrm_nwservice_attach_key* nwsrv_nwsrv_obj_info,
                           struct nsrm_nschain_object_key*   chain_obj_info_p)
{
  uint32_t uiMandParamCnt;

  CM_CBK_DEBUG_PRINT ("Entered");
 
  for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;uiMandParamCnt++)
  {
    switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
    {
      case CM_DM_SERVICES_SRVNAME_ID:
      nwsrv_nwsrv_obj_info->name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
      nwsrv_config_info->name_p  = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
      CM_CBK_DEBUG_PRINT ("name_p:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
      of_strncpy (nwsrv_nwsrv_obj_info->name_p, 
                (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                 pMandParams->iv_pairs[uiMandParamCnt].value_length);
      of_strncpy (nwsrv_config_info->name_p,
                (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                 pMandParams->iv_pairs[uiMandParamCnt].value_length);
      break;

      case CM_DM_CHAIN_TENANT_ID:
      CM_CBK_DEBUG_PRINT ("name_p:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
      chain_obj_info_p->tenant_name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
      of_strncpy (chain_obj_info_p->tenant_name_p, 
                (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                 pMandParams->iv_pairs[uiMandParamCnt].value_length);
      break;

      case CM_DM_CHAIN_NAME_ID:
      nwsrv_nwsrv_obj_info->nschain_object_name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
      chain_obj_info_p->name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
      of_strncpy (chain_obj_info_p->name_p, 
                (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                 pMandParams->iv_pairs[uiMandParamCnt].value_length);
      of_strncpy (nwsrv_nwsrv_obj_info->nschain_object_name_p,
                (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                 pMandParams->iv_pairs[uiMandParamCnt].value_length);
      CM_CBK_DEBUG_PRINT ("name_p:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
      CM_CBK_DEBUG_PRINT ("chain name_p:%s",(char *) chain_obj_info_p->name_p);
      CM_CBK_DEBUG_PRINT ("chain name_p:%s",(char *) nwsrv_nwsrv_obj_info->nschain_object_name_p);
      break;

      case CM_DM_SERVICES_SEQUENCENUMBER_ID:
         nwsrv_nwsrv_obj_info->sequence_number = of_atoi((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
         CM_CBK_DEBUG_PRINT("seq  :%d ",nwsrv_nwsrv_obj_info->sequence_number); 
      break;
     }
  }
   return OF_SUCCESS;
}


int32_t nsrm_attach_nwservice_to_nschain_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
                        uint32_t command_id, struct cm_app_result ** result_p)
{
  struct cm_app_result *of_nwsrv_obj_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct nsrm_nwservice_object_key   nwsrv_config_info={};

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&nwsrv_config_info, 0, sizeof (nwsrv_config_info));

  return_value = nsrm_nwsrv_obj_validatemandparams(key_iv_pairs_p, &of_nwsrv_obj_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  *result_p = of_nwsrv_obj_result;
  return OF_SUCCESS;

}

int32_t nsrm_nwsrv_obj_validatemandparams(struct cm_array_of_iv_pairs *mand_iv_pairs_p,
    struct cm_app_result **presult_p)
{
  uint32_t count;
  struct cm_app_result *of_nwsrv_obj_result = NULL;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (count = 0; count < mand_iv_pairs_p->count_ui;count++)
  {
    switch (mand_iv_pairs_p->iv_pairs[count].id_ui)
    {
      case CM_DM_CHAIN_NAME_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Appl category name is NULL");
          fill_app_result_struct (&of_nwsrv_obj_result, NULL, CM_GLU_APPLIANCE_NAME_NULL);
          *presult_p = of_nwsrv_obj_result;
          return OF_FAILURE;
        }
       case CM_DM_CHAIN_TENANT_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Appl tenant name is NULL");
          fill_app_result_struct (&of_nwsrv_obj_result, NULL, CM_GLU_APPLIANCE_NAME_NULL);
          *presult_p = of_nwsrv_obj_result;
          return OF_FAILURE;
        }
        break;
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs_p);
  return OF_SUCCESS;
}

