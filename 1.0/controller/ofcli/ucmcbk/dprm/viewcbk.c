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
 * File name: viewcbk.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   08/08/2013
 * Description: 
 * 
 */

#ifdef OF_CM_SUPPORT


#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "of_view.h"

int32_t of_view_appl_ucmcbk_init (void);
int32_t of_view_addrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * pMandParams,
    struct cm_array_of_iv_pairs * pOptParams,
    struct cm_app_result ** result_p);

int32_t of_view_setrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * pMandParams,
    struct cm_array_of_iv_pairs * pOptParams,
    struct cm_app_result ** result_p);

int32_t of_view_delrec (void * config_transaction_p,
    struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_app_result ** result_p);

int32_t of_view_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
    uint32_t * count_p,
    struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);

int32_t of_view_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_array_of_iv_pairs * prev_record_key_p, uint32_t * count_p,
    struct cm_array_of_iv_pairs ** next_n_record_data_p);

int32_t of_view_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t of_view_verifycfg (struct cm_array_of_iv_pairs * keys_arr_p,
    uint32_t command_id, struct cm_app_result ** result_p);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */
int32_t of_view_ucm_validatemandparams (struct cm_array_of_iv_pairs *
    pMandParams,
    struct cm_app_result **
    presult_p);

int32_t of_view_ucm_validateoptparams (struct cm_array_of_iv_pairs *
    pOptParams,
    struct cm_app_result ** presult_p);


int32_t of_view_ucm_setmandparams (struct cm_array_of_iv_pairs *
    pMandParams,
    char *view_type,
    struct database_view_node_info * of_view_node,
    struct cm_app_result ** presult_p);

int32_t of_view_ucm_setoptparams (struct cm_array_of_iv_pairs *
    pOptParams,
    struct database_view_node_info * of_view_node,
    struct cm_app_result ** presult_p);


int32_t of_view_ucm_getparams (char *view_type,struct database_view_node_info *of_view_node,
    struct cm_array_of_iv_pairs * result_iv_pairs_p);



struct cm_dm_app_callbacks of_view_ucm_callbacks = {
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  of_view_getfirstnrecs,
  of_view_getnextnrecs,
  of_view_getexactrec,
  of_view_verifycfg,
  NULL
};


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * View  UCM Init * * * * * * *  * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t of_view_appl_ucmcbk_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  cm_register_app_callbacks (CM_ON_DIRECTOR_VIEW_APPL_ID, &of_view_ucm_callbacks);
  return OF_SUCCESS;
}
#if 0
int32_t of_view_addrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * pMandParams,
    struct cm_array_of_iv_pairs * pOptParams,
    struct cm_app_result ** result_p)
{
  struct cm_app_result *of_view_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct database_view_node_info of_view_node = { };
  uint64_t view_handle;

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&of_view_node, 0, sizeof (struct database_view_node_info));

  if ((of_view_ucm_setmandparams (pMandParams, &of_view_node, &of_view_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    *result_p=of_view_result;
    return OF_FAILURE;
  }

  return_value = of_view_ucm_setoptparams (pOptParams, &of_view_node, &of_view_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
    *result_p = of_view_result;
    return OF_FAILURE;
  }

  return_value = dprm_register_switch(&of_view_node,&view_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("View Addition Failed");
    fill_app_result_struct (&of_view_result, NULL, CM_GLU_SWITCH_ADD_FAILED);
    *result_p = of_view_result;
    return OF_FAILURE;
  }

  return OF_SUCCESS;
}

int32_t of_view_setrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * pMandParams,
    struct cm_array_of_iv_pairs * pOptParams,
    struct cm_app_result ** result_p)
{
  struct cm_app_result *of_view_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct database_view_node_info of_view_node = { };
  struct dprm_switch_runtime_info runtime_info;
  uint64_t view_handle;


  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&of_view_node, 0, sizeof (struct database_view_node_info));

  if ((of_view_ucm_setmandparams (pMandParams, &of_view_node, &of_view_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    *result_p=of_view_result;
    return OF_FAILURE;
  }


  return_value=of_view_get_namespace_dpid_view_handle_by_name(of_view_node.node_name, &view_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("View does not exist with the name %s",of_view_node.node_name);
    fill_app_result_struct (&of_view_result, NULL, CM_GLU_SWITCH_DOESNOT_EXIST);
    *result_p = of_view_result;
    return OF_FAILURE;
  }

  of_memset (&of_view_node, 0, sizeof (struct database_view_node_info));
  return_value=dprm_get_exact_switch(view_handle, &of_view_node, &runtime_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Unable to find the matching record");

    fill_app_result_struct (&of_view_result, NULL, CM_GLU_NO_MATCHING_RECORD);
    *result_p = of_view_result;
    return OF_FAILURE;
  }

  return_value = of_view_ucm_setoptparams (pOptParams, &of_view_node, &of_view_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
    *result_p = of_view_result;
    return OF_FAILURE;
  }

  return_value = dprm_update_switch(view_handle,&of_view_node);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("View updation Failed");
    fill_app_result_struct (&of_view_result, NULL, CM_GLU_SWITCH_UPDATE_FAILED);
    *result_p = of_view_result;
    return OF_FAILURE;
  }

  return OF_SUCCESS;
}

int32_t of_view_delrec (void * config_transaction_p,
    struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_app_result ** result_p)
{
  struct cm_app_result *of_view_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct database_view_node_info of_view_node = { };
  uint64_t view_handle;

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&of_view_node, 0, sizeof (struct database_view_node_info));

  if ((of_view_ucm_setmandparams (keys_arr_p, &of_view_node, &of_view_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&of_view_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p = of_view_result;
    return OF_FAILURE;
  }

  return_value=of_view_get_namespace_dpid_view_handle_by_name(of_view_node.node_name, &view_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: View doesn't exists with name %s",of_view_node.node_name);
    fill_app_result_struct (&of_view_result, NULL, CM_GLU_SWITCH_DOESNOT_EXIST);
    *result_p = of_view_result;
    return OF_FAILURE;
  }

  return_value = dprm_unregister_switch(view_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Delete View Record failed");
    fill_app_result_struct (&of_view_result, NULL, CM_GLU_SWITCH_DELETE_FAILED);
    *result_p = of_view_result;
    return OF_FAILURE;
  }
  return OF_SUCCESS;
}
#endif

int32_t of_view_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
    uint32_t * count_p,
    struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct database_view_node_info of_view_node;
  uint64_t view_handle;
  int32_t ii,iRes = OF_FAILURE;
  uint32_t uiRecCount = 0;
  char view_type[64];
  struct cm_app_result *of_view_result = NULL;

  CM_CBK_DEBUG_PRINT ("entry");
  of_memset (&of_view_node, 0, sizeof (struct database_view_node_info));
  of_memset (view_type, 0, 64);
#if 0
  if ((of_view_ucm_setmandparams (keys_arr_p, view_type,&of_view_node, &of_view_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&of_view_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    return OF_FAILURE;
  }

#endif
  for(ii=0; ii < DPRM_MAX_VIEW_DATABASE; ii++)
  {
    if (  of_view_db_valid(ii) == TRUE)
    {
      CM_CBK_DEBUG_PRINT ("request for view entry ");
      iRes=of_view_get_first_view_entry(ii,&of_view_node, &view_handle);
      if ( iRes != OF_SUCCESS)
      {
        continue;             
      }
      of_get_view_db_name(ii, view_type);
      break;
    }
  }
  if ( iRes != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("no records found");
    return OF_FAILURE;
  }
  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    return OF_FAILURE;
  }

  of_view_ucm_getparams (view_type,  &of_view_node, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *array_of_iv_pair_arr_p = result_iv_pairs_p;
  *count_p = uiRecCount;
  return OF_SUCCESS;
}


int32_t of_view_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_array_of_iv_pairs *prev_record_key_p, uint32_t * count_p,
    struct cm_array_of_iv_pairs ** next_n_record_data_p)
{

  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct cm_app_result *app_result_p = NULL;
  struct database_view_node_info of_view_node;
  uint64_t view_handle;
  struct cm_app_result *of_view_result = NULL;
  int32_t ii,iRes = OF_FAILURE;
  uint32_t uiRecCount = 0;
  char view_type[64];
  int32_t view_index=-1;

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&of_view_node, 0, sizeof (struct database_view_node_info));
  of_memset (view_type, 0, 64);

  if ((of_view_ucm_setmandparams (prev_record_key_p, view_type,&of_view_node, &app_result_p)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }


  CM_CBK_DEBUG_PRINT ("view_type %s",view_type);
  view_index=of_get_view_index(view_type);
  CM_CBK_DEBUG_PRINT ("view_index %d",view_index);
  iRes= of_view_get_view_handle_by_name(view_index,&of_view_node, &view_handle);
  if (iRes != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: view node does not exist with name %s",of_view_node.node_name);
    return OF_FAILURE;
  }

  iRes=of_view_get_next_view_entry(view_index,&of_view_node, &view_handle);
  if ( iRes != OF_SUCCESS)
  {
    for(ii=view_index+1; ii < DPRM_MAX_VIEW_DATABASE; ii++)
    {
      if ( of_view_db_valid(ii) == TRUE)
      {
        CM_CBK_DEBUG_PRINT ("request for view dpid_namespace ");
        iRes=of_view_get_first_view_entry(ii,&of_view_node, &view_handle);
        if ( iRes == OF_SUCCESS)
        {
          of_get_view_db_name(ii, view_type);
          break;
        }
        continue;
      }
    }
  }
  if (iRes != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("failed");
    *count_p = 0;
    return OF_FAILURE;
  }
  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));

  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("Next view record is : %s ",
      of_view_node.view_value);
  of_view_ucm_getparams (view_type,&of_view_node, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  CM_CBK_DEBUG_PRINT ("Number of records requested were %ld ", *count_p);
  CM_CBK_DEBUG_PRINT ("Number of records found are %ld", uiRecCount+1);
  *next_n_record_data_p = result_iv_pairs_p;
  *count_p = uiRecCount;
  return OF_SUCCESS;
}

int32_t of_view_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_array_of_iv_pairs ** pIvPairArr)
{

  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct cm_app_result *app_result_p = NULL;
  struct database_view_node_info of_view_node;
  uint64_t view_handle;
  uint32_t view_index = 0;
  int32_t iRes = OF_FAILURE;
  char view_type[24];

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&of_view_node, 0, sizeof (struct database_view_node_info));
  of_memset (view_type, 0, 64);

  if ((of_view_ucm_setmandparams (keys_arr_p, view_type,&of_view_node, &app_result_p)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  view_index=of_get_view_index(view_type);
  if (view_index == OF_FAILURE)
  {
    CM_CBK_DEBUG_PRINT ("Error: View does not exist with name %s",of_view_node.node_name);
    return OF_FAILURE;
  }
  of_memset (&of_view_node, 0, sizeof (struct database_view_node_info));
  iRes=of_view_get_first_view_entry(view_index,&of_view_node, &view_handle);
  if ( iRes != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("of_view_get_first_view_entry");
    return OF_FAILURE;
  }
  if (iRes != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: View does not exist with name %s",of_view_node.node_name);
    return OF_FAILURE;
  }


  CM_CBK_DEBUG_PRINT ("Exact matching record found");
  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));

  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Failed to allocate memory for result_iv_pairs_p");
    return OF_FAILURE;
  }
  of_view_ucm_getparams (view_type, &of_view_node, result_iv_pairs_p);
  *pIvPairArr = result_iv_pairs_p;
  return OF_SUCCESS;
}

int32_t of_view_verifycfg (struct cm_array_of_iv_pairs * keys_arr_p,
    uint32_t command_id, struct cm_app_result ** result_p)
{
  struct cm_app_result *of_view_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct database_view_node_info of_view_node = { };

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&of_view_node, 0, sizeof (struct database_view_node_info));

  return_value = of_view_ucm_validatemandparams (keys_arr_p, &of_view_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    return OF_FAILURE;
  }
#if 0
  if (of_view_ucm_validateoptparams (keys_arr_p, &of_view_result)
      != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    return OF_FAILURE;
  }
#endif
  *result_p = of_view_result;
  return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */

int32_t of_view_ucm_validatemandparams (struct cm_array_of_iv_pairs *
    pMandParams,
    struct cm_app_result **  presult_p)
{
  uint32_t uiMandParamCnt;
  struct cm_app_result *of_view_result = NULL;


  CM_CBK_DEBUG_PRINT ("Entered");
  for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;
      uiMandParamCnt++)
  {
    switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
    {

      case CM_DM_VIEW_NODE_NAME_ID:
        if (pMandParams->iv_pairs[uiMandParamCnt].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Node Name is NULL");
          fill_app_result_struct (&of_view_result, NULL, CM_GLU_SWITCH_NAME_NULL);
          *presult_p = of_view_result;
          return OF_FAILURE;
        }
        break;

    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
  return OF_SUCCESS;
}

int32_t of_view_ucm_validateoptparams (struct cm_array_of_iv_pairs *
    pOptParams,
    struct cm_app_result ** presult_p)
{
  uint32_t uiOptParamCnt;
  struct cm_app_result *of_view_result = NULL;
  uint8_t view_type;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (uiOptParamCnt = 0; uiOptParamCnt < pOptParams->count_ui; uiOptParamCnt++)
  {
    switch (pOptParams->iv_pairs[uiOptParamCnt].id_ui)
    {
      case CM_DM_VIEW_VIEW_VALUE_ID:
        if (pOptParams->iv_pairs[uiOptParamCnt].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Fully Qualified Domain Name (FQDN) is NULL");
          fill_app_result_struct (&of_view_result, NULL, CM_GLU_SWITCH_FQDN_NULL);
          *presult_p = of_view_result;
          return OF_FAILURE;
        }
        break;
      case CM_DM_VIEW_TYPE_ID:
        if (pOptParams->iv_pairs[uiOptParamCnt].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Node Name is NULL");
          fill_app_result_struct (&of_view_result, NULL, CM_GLU_SWITCH_NAME_NULL);
          *presult_p = of_view_result;
          return OF_FAILURE;
        }
        break;
      default:
        break;
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
  return OF_SUCCESS;
}

int32_t of_view_ucm_setmandparams (struct cm_array_of_iv_pairs *
    pMandParams,
    char *view_type,
    struct database_view_node_info * of_view_node,
    struct cm_app_result ** presult_p)
{
  uint32_t uiMandParamCnt;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;
      uiMandParamCnt++)
  {
    switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
    {

      case CM_DM_VIEW_NODE_NAME_ID:
        of_strncpy (of_view_node->node_name,
            (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
            pMandParams->iv_pairs[uiMandParamCnt].value_length);
        break;
      case CM_DM_VIEW_VIEW_VALUE_ID:
        of_strncpy (of_view_node->view_value,
            (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
            pMandParams->iv_pairs[uiMandParamCnt].value_length);
        break;
      case CM_DM_VIEW_TYPE_ID:
        of_strncpy (view_type,
            (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
            pMandParams->iv_pairs[uiMandParamCnt].value_length);
        break;

    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
  return OF_SUCCESS;
}

int32_t of_view_ucm_setoptparams (struct cm_array_of_iv_pairs *
    pOptParams,
    struct database_view_node_info * of_view_node,
    struct cm_app_result ** presult_p)
{
  uint32_t uiOptParamCnt;

  CM_CBK_DEBUG_PRINT ("Entered");

  for (uiOptParamCnt = 0; uiOptParamCnt < pOptParams->count_ui; uiOptParamCnt++)
  {
    switch (pOptParams->iv_pairs[uiOptParamCnt].id_ui)
    {
      default:
        break;
    }
  }

  CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
  return OF_SUCCESS;
}



int32_t of_view_ucm_getparams (char *view_type, struct database_view_node_info *of_view_node,
    struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
  uint32_t uindex_i = 0;
  char buf[10] = "";
#define CM_VIEW_CHILD_COUNT 3

  result_iv_pairs_p->iv_pairs =
    (struct cm_iv_pair *) of_calloc (CM_VIEW_CHILD_COUNT, sizeof (struct cm_iv_pair));
  if (result_iv_pairs_p->iv_pairs == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p->iv_pairs");
    return OF_FAILURE;
  }
  /*view type */
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_VIEW_TYPE_ID,
      CM_DATA_TYPE_STRING, view_type);
  uindex_i++;

  /*view value*/
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_VIEW_VIEW_VALUE_ID,
      CM_DATA_TYPE_STRING, of_view_node->view_value);
  uindex_i++;

  /* node name*/
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_VIEW_NODE_NAME_ID,
      CM_DATA_TYPE_STRING, of_view_node->node_name);
  uindex_i++;



  result_iv_pairs_p->count_ui = uindex_i;
  CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);
  return OF_SUCCESS;
}


#endif /* OF_CM_SUPPORT */
