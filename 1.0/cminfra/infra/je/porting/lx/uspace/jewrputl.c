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
 * File name: jewrputl.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/05/2013
 * Description: 
*/

#ifdef CM_SUPPORT
//#ifdef CM_JE_SUPPORT

#include "jeincld.h"
#include "jewrap.h"

/* *****************************************************************************
 * * * * * * * * * * * * * FUNCTION DEFINITIONS * * * * * * * * * * * * * * * *
 **************************************************************************** */

/**
  \addtogroup JEWRPUTL JE Utilities APIs
  This API is called to make a copy of multiple Arrays of Name Value Pairs. This calls UCMCopyNvPairArrToNvPairArr repeatedly for each Array of Name Value pairs to accomplish the task.
  <b>Input Parameters: </b>\n
  <i>arr_in_nv_pair_arr_p:</i> Pointer to the first Array of Name Value pairs structure from where to copy the data.
  <i>uiArrayCount:</i> Total number of Such Arrays.
  <b>Output Parameters: </b>\n
  <i>arr_out_nv_pair_arr_p:</i> Pointer to the first Array of Name Value pairs structure to where to copy the data.
  <b>Return Value:</b>
  <i>OF_SUCCESS:</i>In Success case
  <i>OF_FAILURE:</i>In Failure case
 **/
int32_t UCMCopyArrNvPairArrToArrNvPairArr (struct cm_array_of_nv_pair *
                                           arr_in_nv_pair_arr_p,
                                           uint32_t uiArrayCount,
                                           struct cm_array_of_nv_pair *
                                           arr_out_nv_pair_arr_p)
{
  uint32_t uiCnt;

  if ((!arr_in_nv_pair_arr_p) || (!arr_out_nv_pair_arr_p))
  {
    CM_JETRANS_DEBUG_PRINT ("Invalild Input");
    return OF_FAILURE;
  }

  for (uiCnt = 0; uiCnt < uiArrayCount; uiCnt++)
  {
    if (UCMCopyNvPairArrToNvPairArr (&(arr_in_nv_pair_arr_p[uiCnt]),
                                     &(arr_out_nv_pair_arr_p[uiCnt])) != OF_SUCCESS)
    {
      CM_JETRANS_DEBUG_PRINT ("Copy To nv_pair_array Failed");
      return OF_FAILURE;
    }
  }
  return OF_SUCCESS;
}

/**
  \ingroup JEWRPUTL
   This API is called to make a copy of the Array of Name Value Pairs. This calls UCMCopyNvPairToNvPair repeatedly for each Name Value pair to accomplish the task.
   <b>Input Parameters: </b>\n
   <i>pInNvPairArr:</i>  Array Name Value pairs structure from where to copy the data.
   <b>Output Parameters: </b>\n
   <i>pOutNvPairArr:</i> Array of Name Value pairs structure to where to copy the data.
  <b>Return Value:</b>
  <i>OF_SUCCESS:</i>In Success case
  <i>OF_FAILURE:</i>In Failure case
 **/

int32_t UCMCopyNvPairArrToNvPairArr (struct cm_array_of_nv_pair * pInNvPairArr,
                                     struct cm_array_of_nv_pair * pOutNvPairArr)
{
  uint32_t uiCnt;
  uint32_t uiNvCnt;

  if ((!pInNvPairArr) || (!pOutNvPairArr))
  {
    CM_JETRANS_DEBUG_PRINT ("Invalid Input");
    return OF_FAILURE;
  }

  pOutNvPairArr->count_ui = pInNvPairArr->count_ui;
  pOutNvPairArr->nv_pairs = (struct cm_nv_pair *) of_calloc (pOutNvPairArr->count_ui,
                                                     sizeof (struct cm_nv_pair));
  if (!(pOutNvPairArr->nv_pairs))
  {
    CM_JETRANS_DEBUG_PRINT ("Memory Allocation Failed for Name Field");
    return OF_FAILURE;
  }

  for (uiNvCnt = 0; uiNvCnt < pOutNvPairArr->count_ui; uiNvCnt++)
  {
    if (UCMCopyNvPairToNvPair (&(pInNvPairArr->nv_pairs[uiNvCnt]),
                               &(pOutNvPairArr->nv_pairs[uiNvCnt])) != OF_SUCCESS)
    {
      CM_JETRANS_DEBUG_PRINT ("CopyNvPair To NvPair Failed");
      for (uiCnt = 0; uiCnt < (uiNvCnt - 1); uiCnt++)
      {
        CM_FREE_NVPAIR (pInNvPairArr->nv_pairs[uiCnt]);
      }
      of_free (pInNvPairArr->nv_pairs);
      return OF_FAILURE;
    }

  }
  return OF_SUCCESS;
}

/**
  \ingroup JEWRPUTL
  This APi is called to make a copy of the Name Value Pair. It copies Name Length, Allocate Memory for Name, copies Name, copies Value type, copies Value Length, Allocate Memory for Value and copies Value.
  <b>Input Parameters: </b>\n
  <i>pInNvPair:</i> Name Value pair structure from where to copy the data.
  <b>Output Parameters: </b>\n
  <i>pOutNvPair:</i> Name Value pair structure to where to copy the data.
  <b>Return Value:</b>
  <i>OF_SUCCESS:</i>In Success case
  <i>OF_FAILURE:</i>In Failure case
 **/
int32_t UCMCopyNvPairToNvPair (struct cm_nv_pair * pInNvPair,
                               struct cm_nv_pair * pOutNvPair)
{

  if ((!pInNvPair) || (!pOutNvPair))
  {
    CM_JETRANS_DEBUG_PRINT ("Invalid Input");
    return OF_FAILURE;
  }
  /* Copy Name Length */
  pOutNvPair->name_length = pInNvPair->name_length;

  /* Allocate Memory for Name */
  pOutNvPair->name_p = (char *) of_calloc (1, pOutNvPair->name_length + 1);
  if (!(pOutNvPair->name_p))
  {
    CM_JETRANS_DEBUG_PRINT ("Memory Allocation Failed for Name Field");
    return OF_FAILURE;
  }

  /* Copy Name */
  of_strncpy (pOutNvPair->name_p, pInNvPair->name_p, pOutNvPair->name_length);

  /* Copy Value type */
  pOutNvPair->value_type = pInNvPair->value_type;

  /* Copy Value Length */
  pOutNvPair->value_length = pInNvPair->value_length;

  /* Allocate Memory for Value */
  pOutNvPair->value_p = (char *) of_calloc (1, pOutNvPair->value_length + 1);
  if (!(pOutNvPair->value_p))
  {
    CM_JETRANS_DEBUG_PRINT ("Memory Allocation Failed for Value Field");
    of_free (pOutNvPair->name_p);
    return OF_FAILURE;
  }

  /* Copy Value */
  of_strncpy (pOutNvPair->value_p, pInNvPair->value_p, pOutNvPair->value_length);

  CM_JETRANS_DEBUG_PRINT ("NameLen= %dName=%s  ValueType=%d Len=%d Value=%s",
                           pOutNvPair->name_length,
                           pOutNvPair->name_p,
                           pOutNvPair->value_type,
                           pOutNvPair->value_length,(char *) pOutNvPair->value_p);

  return OF_SUCCESS;

}

/**
  \ingroup JEWRPUTL
  This API is used to free up memory used in the UCM result structure, the data path and the Name Value pair in case of error result and then the pUCMResult structure itself.
  <b>Input Parameters: </b>\n
  <i>pUCMResult:</i> UCM Result Structure.
  <b>Output Parameters: </b> NONE \n
  <b>Return Value:</b>
  <i>OF_SUCCESS:</i>In Success case
  <i>OF_FAILURE:</i>In Failure case
 **/
int32_t UCMFreeUCMResult (struct cm_result * pUCMResult)
{

  CM_JETRANS_DEBUG_PRINT ("Entered");

  if (pUCMResult == NULL)
  {
    CM_JETRANS_DEBUG_PRINT ("Input Request is NULL");
    return OF_FAILURE;
  }

  if (pUCMResult->result_code == CM_JE_FAILURE)
  {
    CM_JETRANS_DEBUG_PRINT ("Freeing UCM result in error case");

    if (pUCMResult->result.error.dm_path_p)
      of_free (pUCMResult->result.error.dm_path_p);
    if (pUCMResult->result.error.result_string_p)
      of_free (pUCMResult->result.error.result_string_p);
    if (pUCMResult->result.error.nv_pair_b)
    {
      if (pUCMResult->result.error.nv_pair.name_p)
        of_free (pUCMResult->result.error.nv_pair.name_p);
      if (pUCMResult->result.error.nv_pair.value_p)
        of_free (pUCMResult->result.error.nv_pair.value_p);
    }
  }
  of_free (pUCMResult);
  return OF_SUCCESS;
}

/**
  \ingroup JEWRPUTL
  This API frees the UCmRequest Message structure and its any other memory as dictated by the command is had serviced.
  <b>Input Parameters: </b>\n
  <i>pUCMRequestMsg:</i> The UCM Request Structure
  <b>Output Parameters: </b> NONE \n
  <b>Return Value:</b>
  <i>OF_SUCCESS:</i>In Success case
  <i>OF_FAILURE:</i>In Failure case
 **/
int32_t UCMJEFreeRequestMsg (struct cm_request_msg * pUCMRequestMsg)
{

  CM_JETRANS_DEBUG_PRINT ("Entered");

  if (pUCMRequestMsg == NULL)
  {
    CM_JETRANS_DEBUG_PRINT ("Input Request is NULL");
    return OF_FAILURE;
  }

  switch (pUCMRequestMsg->sub_command_id)
  {
    case CM_CMD_ADD_PARAMS:
    case CM_CMD_SET_PARAMS:
    case CM_CMD_DEL_PARAMS:
      //  UCMJE_FREE_NVPAIR_ARRAY (pUCMRequestMsg->nv_pair_array,
      //                         pUCMRequestMsg->nv_pair_array.count_ui);
      break;

    case CM_CMD_LOAD_CONFIG:
    case CM_CMD_SAVE_CONFIG:
    case CM_CMD_SET_DEFAULTS:
      break;

		case CM_CMD_GET_NEXT_N_LOGS:
		case CM_CMD_GET_FIRST_N_LOGS:
    case CM_CMD_GET_NEXT_N_RECS:
      //CM_FREE_NVPAIR (pUCMRequestMsg->prev_nv_pair);
#if 0
        UCMJE_FREE_NVPAIR_ARRAY (pUCMRequestMsg->prev_nv_pair,
                               pUCMRequestMsg->prev_nv_pair.count_ui);
#endif
    case CM_CMD_GET_FIRST_N_RECS:
    case CM_CMD_GET_EXACT_REC:
      //  UCMJE_FREE_NVPAIR_ARRAY (pUCMRequestMsg->nv_pair_array,
      //                         pUCMRequestMsg->nv_pair_array.count_ui);
      break;
    case CM_CMD_GET_DM_NODE_INFO:
    case CM_CMD_GET_DM_NODE_AND_CHILD_INFO:
    case CM_CMD_GET_DM_CHILD_NODE_COUNT:
    case CM_CMD_SET_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
    case CM_CMD_GET_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
    case CM_CMD_DEL_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
    case CM_CMD_GET_DM_FIRST_NODE_INFO:
    case CM_CMD_GET_DM_NEXT_NODE_INFO:
    case CM_CMD_GET_NEW_DM_INSTANCE_ID:
    case CM_CMD_ADD_DM_INSTANCE_MAP:
    case CM_CMD_GET_DM_INSTANCE_MAPLIST_BY_NAMEPATH:
    case CM_CMD_DEL_DM_INSTANCE_MAP:
    case CM_CMD_GET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
    case CM_CMD_SET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
    case CM_CMD_DEL_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
      // Free    pUCMRequestMsg->data.data_modelGet.count_ui:
      break;
  }

  if (pUCMRequestMsg->dm_path_p)
  {
    // of_free (pUCMRequestMsg->dm_path_p);
  }
  of_free (pUCMRequestMsg);
  return OF_SUCCESS;
}

//#endif
#endif
