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
 * File name: chainattrcbk.c
 * Author:
 * Date: 
 * Description: 
*/
#include "cmincld.h"
#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "nsrm.h"


int32_t nsrm_nschainset_attribute_addrec (void * config_trans_p,
                                     struct cm_array_of_iv_pairs * pMandParams,
                                     struct cm_array_of_iv_pairs * pOptParams,
                                     struct cm_app_result ** result_p);
int32_t nsrm_nschainset_attribute_setrec (void * config_trans_p,
                                     struct cm_array_of_iv_pairs * pMandParams,
                                     struct cm_array_of_iv_pairs * pOptParams,
                                     struct cm_app_result ** result_p);

int32_t nsrm_nschainset_attribute_delrec (void * config_transaction_p,
                                     struct cm_array_of_iv_pairs * keys_arr_p,
                                     struct cm_app_result ** result_p);


int32_t nsrm_nschainset_attribute_getfirstnrecs(struct cm_array_of_iv_pairs * keys_arr_p,
                                               uint32_t * count_p,
                                               struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);



int32_t nsrm_nschainset_attribute_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                                               struct cm_array_of_iv_pairs * prev_record_key_p,
                                               uint32_t * count_p,
                                               struct cm_array_of_iv_pairs ** next_n_record_data_p);


int32_t nsrm_nschainset_attribute_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
                                             struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t nsrm_nschainset_attribute_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
                        uint32_t command_id, struct cm_app_result ** result_p);

int32_t nsrm_nschainset_attribute_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                        char*  nschainset_object_name_p,
                        struct nsrm_attribute_name_value_pair *attribute_info,
                        struct cm_app_result              **presult_p);


int32_t nsrm_nschainset_attribute_getparams (struct nsrm_attribute_name_value_output_info *attribute_info,
                                            struct cm_array_of_iv_pairs * result_iv_pairs_p);


struct cm_dm_app_callbacks  nsrm_nschainset_attribute_ucm_callbacks =
{
  NULL,
  nsrm_nschainset_attribute_addrec,
  nsrm_nschainset_attribute_setrec,
  nsrm_nschainset_attribute_delrec,
  NULL,
  nsrm_nschainset_attribute_getfirstnrecs,
  nsrm_nschainset_attribute_getnextnrecs,
  nsrm_nschainset_attribute_getexactrec,
  nsrm_nschainset_attribute_verifycfg, 
 NULL
};

/***************************************************************************************************/
int32_t nsrm_nschainset_attribute_appl_ucmcbk_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  if(cm_register_app_callbacks (CM_ON_DIRECTOR_NSRM_CHAINSET_CHAINSETATTRIBUTE_APPL_ID, &nsrm_nschainset_attribute_ucm_callbacks) != OF_SUCCESS)
    return OF_FAILURE;

  return OF_SUCCESS;
}

/***************************************************************************************************/
int32_t nsrm_nschainset_attribute_addrec (void * config_trans_p,
                                     struct cm_array_of_iv_pairs * pMandParams,
                                     struct cm_array_of_iv_pairs * pOptParams,
                                     struct cm_app_result ** result_p)
{
  struct   cm_app_result                    *attribute_result = NULL;
  struct   nsrm_attribute_name_value_pair    nsrm_attribute_name_value_info={};
  char*    nschainset_object_name_p;
  int32_t  ret_value;

  CM_CBK_DEBUG_PRINT("Entered"); 
  nschainset_object_name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH);
  if((nsrm_nschainset_attribute_ucm_setmandparams (
                                               pMandParams,
                                               nschainset_object_name_p, &nsrm_attribute_name_value_info,
                                               &attribute_result)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&attribute_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p = attribute_result;
    of_free(nschainset_object_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_value_p);
    return OF_FAILURE;
  }

  ret_value = nsrm_add_attribute_to_nschainset_object(nschainset_object_name_p,
                                                     &nsrm_attribute_name_value_info);
  if (ret_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Service Attribute Addition Failed");
    fill_app_result_struct (&attribute_result, NULL, CM_GLU_ATTRIBUTE_ADD_FAILED);
    *result_p = attribute_result;
    of_free(nschainset_object_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_value_p);
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("Attribute added succesfully");
  of_free(nschainset_object_name_p);
  of_free(nsrm_attribute_name_value_info.attribute_name_p);
  of_free(nsrm_attribute_name_value_info.attribute_value_p);
  return OF_SUCCESS;

}

int32_t nsrm_nschainset_attribute_setrec (void * config_trans_p,
                                     struct cm_array_of_iv_pairs * pMandParams,
                                     struct cm_array_of_iv_pairs * pOptParams,
                                     struct cm_app_result ** result_p)
{
  struct   cm_app_result                    *attribute_result = NULL;
  struct   nsrm_attribute_name_value_pair    nsrm_attribute_name_value_info={};
  char*    nschainset_object_name_p;
  int32_t  ret_value;

  CM_CBK_DEBUG_PRINT("Entered"); 
  nschainset_object_name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH);
  if((nsrm_nschainset_attribute_ucm_setmandparams (
                                               pMandParams,
                                               nschainset_object_name_p, &nsrm_attribute_name_value_info,
                                               &attribute_result)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&attribute_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p = attribute_result;
    of_free(nschainset_object_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_value_p);
    return OF_FAILURE;
  }

  ret_value = nsrm_add_attribute_to_nschainset_object(nschainset_object_name_p,
                                                     &nsrm_attribute_name_value_info);
  if (ret_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Service Attribute Addition Failed");
    fill_app_result_struct (&attribute_result, NULL, CM_GLU_ATTRIBUTE_MOD_FAILED);
    *result_p = attribute_result;
    of_free(nschainset_object_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_value_p);
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("Attribute added succesfully");
  of_free(nschainset_object_name_p);
  of_free(nsrm_attribute_name_value_info.attribute_name_p);
  of_free(nsrm_attribute_name_value_info.attribute_value_p);
  return OF_SUCCESS;

}


int32_t nsrm_nschainset_attribute_delrec (void * config_transaction_p,
                                     struct cm_array_of_iv_pairs * keys_arr_p,
                                     struct cm_app_result ** result_p)
{
  struct   cm_app_result                    *attribute_result = NULL;
  struct   nsrm_attribute_name_value_pair    nsrm_attribute_name_value_info={};
  char*    nschainset_object_name_p;
  int32_t  ret_value;
  
  nschainset_object_name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH);
  if((nsrm_nschainset_attribute_ucm_setmandparams (
                                               keys_arr_p,
                                               nschainset_object_name_p, &nsrm_attribute_name_value_info,
                                               &attribute_result)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&attribute_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p = attribute_result;
    of_free(nschainset_object_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_value_p);
    return OF_FAILURE;
  }
  
  ret_value = nsrm_del_attribute_from_nschainset_object(nschainset_object_name_p,
                                                       nsrm_attribute_name_value_info.attribute_name_p);
  if (ret_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Delete attribute record failed");
    fill_app_result_struct (&attribute_result, NULL, CM_GLU_ATTRIBUTE_DEL_FAILED);
    *result_p = attribute_result;
    of_free(nschainset_object_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_value_p);
    return OF_FAILURE;
  }
    
  CM_CBK_DEBUG_PRINT ("service attribute deleted succesfully");
  of_free(nschainset_object_name_p);
  of_free(nsrm_attribute_name_value_info.attribute_name_p);
  of_free(nsrm_attribute_name_value_info.attribute_value_p);
  return OF_SUCCESS;
}




int32_t nsrm_nschainset_attribute_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                                               struct cm_array_of_iv_pairs * prev_record_key_p, 
                                               uint32_t * count_p,
                                               struct cm_array_of_iv_pairs ** next_n_record_data_p)
{
  struct   cm_array_of_iv_pairs                   *result_iv_pairs_p = NULL;
  struct   nsrm_attribute_name_value_pair          nsrm_attribute_name_value_info={};
  struct   nsrm_attribute_name_value_output_info   nsrm_attribute_output={};
  char*    nschainset_object_name_p ;
  int32_t  return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;

  nschainset_object_name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH);
  if((nsrm_nschainset_attribute_ucm_setmandparams (keys_arr_p,nschainset_object_name_p,
                                                  &nsrm_attribute_name_value_info,NULL)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }
  if((nsrm_nschainset_attribute_ucm_setmandparams (prev_record_key_p,nschainset_object_name_p,
                                                  &nsrm_attribute_name_value_info,NULL)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    of_free(nschainset_object_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_value_p);
    return OF_FAILURE;
  }

  nsrm_attribute_output.name_p       = of_calloc(1,NSRM_MAX_NAME_LENGTH);
  nsrm_attribute_output.value_p      = of_calloc(1,NSRM_MAX_NAME_LENGTH); 
  nsrm_attribute_output.attr_length  = NSRM_MAX_NAME_LENGTH -1;
  nsrm_attribute_output.val_length   = NSRM_MAX_NAME_LENGTH -1;
  return_value = nsrm_get_next_nschainset_object_attribute(
                                                       nschainset_object_name_p,
                                                       nsrm_attribute_name_value_info.attribute_name_p, 
                                                       &nsrm_attribute_output);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Unable to get next n attribute records!");
    of_free(nschainset_object_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_value_p);
    of_free(nsrm_attribute_output.name_p );
    of_free(nsrm_attribute_output.value_p);
    return OF_FAILURE;
  }
  result_iv_pairs_p = (struct cm_array_of_iv_pairs *)of_calloc(1,sizeof(struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    of_free(nschainset_object_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_value_p);
    of_free(nsrm_attribute_output.name_p );
    of_free(nsrm_attribute_output.value_p);
    return OF_FAILURE;
  }
  
  nsrm_nschainset_attribute_getparams(&nsrm_attribute_output, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *next_n_record_data_p = result_iv_pairs_p;
  *count_p = uiRecCount;
  of_free(nschainset_object_name_p);
  of_free(nsrm_attribute_name_value_info.attribute_name_p);
  of_free(nsrm_attribute_name_value_info.attribute_value_p);
  of_free(nsrm_attribute_output.name_p );
  of_free(nsrm_attribute_output.value_p);
  return NSRM_SUCCESS;
}

int32_t nsrm_nschainset_attribute_getfirstnrecs(struct cm_array_of_iv_pairs * keys_arr_p,
                                               uint32_t * count_p,
                                               struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{
  struct   cm_array_of_iv_pairs                   *result_iv_pairs_p = NULL;
  struct   nsrm_attribute_name_value_pair          nsrm_attribute_name_value_info={};
  struct   nsrm_attribute_name_value_output_info   nsrm_attribute_output={};
  char*    nschainset_object_name_p ;
  int32_t  return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;

  nschainset_object_name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH);
  if((nsrm_nschainset_attribute_ucm_setmandparams (keys_arr_p,nschainset_object_name_p, 
                                                   &nsrm_attribute_name_value_info,NULL)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    of_free(nschainset_object_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_value_p);
    return OF_FAILURE;
  }


  nsrm_attribute_output.name_p       = of_calloc(1,NSRM_MAX_NAME_LENGTH); 
  nsrm_attribute_output.value_p      = of_calloc(1,NSRM_MAX_NAME_LENGTH); 
  nsrm_attribute_output.attr_length  = NSRM_MAX_NAME_LENGTH -1;
  nsrm_attribute_output.val_length   = NSRM_MAX_NAME_LENGTH -1;

  return_value = nsrm_get_first_nschainset_object_attribute(
                                                        nschainset_object_name_p,
                                                        &nsrm_attribute_output);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Unable to get first n attribute records!");
    of_free(nschainset_object_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_value_p);
    of_free(nsrm_attribute_output.name_p );
    of_free(nsrm_attribute_output.value_p);
    return OF_FAILURE;
  }

  result_iv_pairs_p = (struct cm_array_of_iv_pairs *)of_calloc(1,sizeof(struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    of_free(nschainset_object_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_value_p);
    of_free(nsrm_attribute_output.name_p );
    of_free(nsrm_attribute_output.value_p);
    return OF_FAILURE;
  }
  
  nsrm_nschainset_attribute_getparams(&nsrm_attribute_output, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *array_of_iv_pair_arr_p = result_iv_pairs_p;
  *count_p                = uiRecCount;

  return NSRM_SUCCESS;
}


int32_t nsrm_nschainset_attribute_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
                                             struct cm_array_of_iv_pairs ** pIvPairArr)
{
  struct   cm_array_of_iv_pairs                    *result_iv_pairs_p = NULL;
  struct   nsrm_attribute_name_value_pair           nsrm_attribute_name_value_info={};
  struct   nsrm_attribute_name_value_output_info    nsrm_attribute_output={};
  char*    nschainset_object_name_p ;
  int32_t  return_value = OF_FAILURE;

  CM_CBK_DEBUG_PRINT ("Entered");
  result_iv_pairs_p =
                    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Failed to allocate memory for result_iv_pairs_p");
    return OF_FAILURE;
  }

  nschainset_object_name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH);
  if ((nsrm_nschainset_attribute_ucm_setmandparams (keys_arr_p,nschainset_object_name_p, 
                                                   &nsrm_attribute_name_value_info,NULL)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    of_free(nschainset_object_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_name_p);
    of_free(nsrm_attribute_name_value_info.attribute_value_p);
    return OF_FAILURE;
  }

  nsrm_attribute_output.name_p       = of_calloc(1,NSRM_MAX_NAME_LENGTH);
  nsrm_attribute_output.value_p      = of_calloc(1,NSRM_MAX_NAME_LENGTH);
  nsrm_attribute_output.attr_length  = NSRM_MAX_NAME_LENGTH -1;
  nsrm_attribute_output.val_length   = NSRM_MAX_NAME_LENGTH -1;

  return_value = nsrm_get_exact_nschainset_object_attribute(
                                                        nschainset_object_name_p, 
                                                        nsrm_attribute_name_value_info.attribute_name_p,
                                                        &nsrm_attribute_output
                                                        );
   if (return_value != OF_SUCCESS)
   {
     CM_CBK_DEBUG_PRINT ("Unable to find  exact attribute record!");
     of_free(nschainset_object_name_p);
     of_free(nsrm_attribute_name_value_info.attribute_name_p);
     of_free(nsrm_attribute_name_value_info.attribute_value_p);
     of_free(nsrm_attribute_output.name_p );
     of_free(nsrm_attribute_output.value_p);
     return OF_FAILURE;
   }

   nsrm_nschainset_attribute_getparams(&nsrm_attribute_output, result_iv_pairs_p);
   *pIvPairArr = result_iv_pairs_p;
   of_free(nschainset_object_name_p);
   of_free(nsrm_attribute_name_value_info.attribute_name_p);
   of_free(nsrm_attribute_name_value_info.attribute_value_p);
   of_free(nsrm_attribute_output.name_p );
   of_free(nsrm_attribute_output.value_p);
   return return_value;
}

int32_t nsrm_nschainset_attribute_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                        char*  nschainset_object_name_p,
                        struct nsrm_attribute_name_value_pair *attribute_info,
                        struct cm_app_result              **presult_p)
{
   int32_t uiMandParamCnt;

   CM_CBK_DEBUG_PRINT ("Entered");
   for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;
                                 uiMandParamCnt++)
   {  
     CM_CBK_DEBUG_PRINT("hit for");
     switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
     {
       case CM_DM_CHAINSET_NAME_ID:
             of_strncpy (nschainset_object_name_p,(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                         pMandParams->iv_pairs[uiMandParamCnt].value_length);
             CM_CBK_DEBUG_PRINT ("nschainset_name %s",nschainset_object_name_p);
             break;

      case CM_DM_CHAINSETATTRIBUTE_ATTRIBUTENAME_ID:
            attribute_info->attribute_name_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH);
            of_strncpy (attribute_info->attribute_name_p,
            (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
            pMandParams->iv_pairs[uiMandParamCnt].value_length);
            CM_CBK_DEBUG_PRINT ("Attribute name %s",attribute_info->attribute_name_p);
            break;

     case CM_DM_CHAINSETATTRIBUTE_ATTRIBUTEVALUE_ID:
           attribute_info->attribute_value_p = (char*)of_calloc(1,NSRM_MAX_NAME_LENGTH);
           of_strncpy (attribute_info->attribute_value_p,
           (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
           pMandParams->iv_pairs[uiMandParamCnt].value_length);
           CM_CBK_DEBUG_PRINT ("Attribute value %s",attribute_info->attribute_value_p);
           break;
     }
   }
         CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
         return OF_SUCCESS;
}


int32_t nsrm_nschainset_attribute_getparams (struct nsrm_attribute_name_value_output_info *attribute_info,
                                            struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
  uint32_t uindex_i = 0;

  CM_CBK_DEBUG_PRINT ("Entered");
  result_iv_pairs_p->iv_pairs = (struct cm_iv_pair *) of_calloc (2, sizeof (struct cm_iv_pair));
  if (result_iv_pairs_p->iv_pairs == NULL)
  {
    CM_CBK_DEBUG_PRINT("Memory allocation failed for result_iv_pairs_p->iv_pairs");
    return OF_FAILURE;
  }

  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_CHAINSETATTRIBUTE_ATTRIBUTENAME_ID,
                   CM_DATA_TYPE_STRING, attribute_info->name_p);
  uindex_i++;

  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_CHAINSETATTRIBUTE_ATTRIBUTEVALUE_ID,
                   CM_DATA_TYPE_STRING, attribute_info->value_p);
  uindex_i++;

  result_iv_pairs_p->count_ui = uindex_i;
  return OF_SUCCESS;

}
int32_t nsrm_nschainset_attribute_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
                        uint32_t command_id, struct cm_app_result ** result_p)
{
  return OF_SUCCESS;
}
