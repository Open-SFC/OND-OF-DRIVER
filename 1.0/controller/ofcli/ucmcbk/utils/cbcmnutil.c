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
 * File name: cbcmnutil.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: This file contains utility functions for   
 *              security application callback functions.
 * 
 */

#ifdef OF_CM_SUPPORT 

/*#include "igwndinc.h"*/

#include"cbcmninc.h"
#include "cmgutil.h"

/***************** FUNCTION DEFINATIONS ****************************/

void fill_app_result_struct(struct cm_app_result** pOutput,struct cm_iv_pair* pIvPair,int32_t result_code)
{
     CM_CBK_DEBUG_PRINT("Building error Iv pair"); 
     struct cm_app_result* result_p = NULL;

     result_p = (struct cm_app_result*)of_calloc(1,sizeof(struct cm_app_result));
     if(result_p == NULL){
        CM_CBK_DEBUG_PRINT("Memory allocation failed for result_p");
        return;
     }

     result_p->result_string_p = (char*)of_calloc(1,CM_SECAPPGLU_ERROR_STRING_LENGTH + 1); 
     if(result_p->result_string_p == NULL){
       CM_CBK_DEBUG_PRINT("Memory allocation failed for pOutput->result_string_p");
       of_free(result_p);
       return ;
      }  

     result_p->result_code = result_code;
     *pOutput = result_p;

     UCMAppGluGetErrMsg (result_code,result_p->result_string_p);
     if(pIvPair != NULL)
       of_memcpy(&result_p->result.error.iv_pair,pIvPair ,sizeof(struct cm_iv_pair));    
     return ;
}

uint32_t fill_err_msg_to_cm_app_struct(struct cm_app_result** result_p,char* errBuff)
{
     struct cm_app_result* pOutput = NULL;

     pOutput = (struct cm_app_result*)of_calloc(1,sizeof(struct cm_app_result));
     if(pOutput == NULL){
        CM_CBK_DEBUG_PRINT("Memory allocation failed for pOutput");
        return OF_FAILURE;
     }

     pOutput->result_string_p = (char*)of_calloc(1,(of_strlen(errBuff) + 1));
     if(pOutput->result_string_p == NULL){
       CM_CBK_DEBUG_PRINT("Memory allocation failed for pOutput->result_string_p");
       of_free(pOutput);
       return OF_FAILURE ;
      }
  
     pOutput->result_code = OF_FAILURE;
     of_strcpy(pOutput->result_string_p,errBuff);
     *result_p = pOutput;
     return OF_SUCCESS;
}


int32_t igwUCMCopyIvPairToIvPair (struct cm_iv_pair * pInIvPair,
                                  struct cm_iv_pair * pOutIvPair)
{

  /* Input Name Value Pair Structure */
  if (pInIvPair == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Invalid NV Pair Output ");
    return OF_FAILURE;
  }

  /* Output Name Value Pair Structure */
  if (pOutIvPair == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Invalid Output Nv Pair");
    return OF_FAILURE;
  }

  /* Copy Name Length */
  pOutIvPair->id_ui = pInIvPair->id_ui;

  /* Copy Value type */
  pOutIvPair->value_type = pInIvPair->value_type;

  /* Copy Value Length */
  pOutIvPair->value_length = pInIvPair->value_length;

  /* Allocate Memory for Value */
  if (pOutIvPair->value_length > 0)
  {
    if (pInIvPair->value_length > CM_MAX_REQUEST_LEN)
    {
      CM_CBK_DEBUG_PRINT ("Value Length is more than allowed request length");
      return OF_FAILURE;
    }

    pOutIvPair->value_p = (char *) of_calloc (1, pOutIvPair->value_length + 1);
    if (!(pOutIvPair->value_p))
    {
      CM_CBK_DEBUG_PRINT ("Memory Allocation Failed for Value Field");
      if (pOutIvPair->value_p)
        of_free (pOutIvPair->value_p);
      return OF_FAILURE;
    }
    /* Copy Each Nv Pair */
    /* Copy Value */
    of_strncpy (pOutIvPair->value_p, pInIvPair->value_p, pOutIvPair->value_length);
  }

  return OF_SUCCESS;
}

int32_t igwUCMFILLIVPAIR (struct cm_iv_pair * pIvPair, uint32_t id_ui,
                          uint32_t uiValType, void * pData)
{
  uint32_t value_length;

  if (pIvPair == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Iv Pair is NULL");
    return OF_FAILURE;
  }
  pIvPair->id_ui = id_ui;
  value_length = of_strlen ((char *) pData);
  pIvPair->value_type = uiValType;
  pIvPair->value_length = value_length;
  pIvPair->value_p = (char *) of_calloc (1, value_length + 1);
  if (pIvPair->value_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory Allocation failed");
    return OF_FAILURE;
  }
  of_strncpy (pIvPair->value_p, (char *) pData, value_length);
  return OF_SUCCESS;
}

void igwUCMFreeIvPairArray (struct cm_array_of_iv_pairs * arr_iv_pair_p,
                              uint32_t count_ui)
{
  uint32_t uindex_i;

  if (arr_iv_pair_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Iv Pair is NULL");
    return;
  }
  if (count_ui > arr_iv_pair_p->count_ui)
  {
    count_ui = arr_iv_pair_p->count_ui;
  }
  for (uindex_i = 0; uindex_i < count_ui; uindex_i++)
  {
    igwUCMFreeIvPair (&arr_iv_pair_p->iv_pairs[uindex_i]);
  }
  of_free (arr_iv_pair_p->iv_pairs);
  of_free (arr_iv_pair_p);

  return;
}

void igwUCMFreeIvPair (struct cm_iv_pair * pIvPair)
{
  if (pIvPair == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Iv Pair is NULL");
    return;
  }
  if (pIvPair->value_p)
  {
    of_free (pIvPair->value_p);
  }
  return;
}


#endif /* OF_CM_SUPPORT */
