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
 * File name: ttpcbk.c
 * Date:  
 * Description: 
 * 
 */

#ifdef OF_CM_SUPPORT


#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "ttpldef.h"

int32_t of_ttp_appl_init (void);

int32_t of_ttp_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
    uint32_t * count_p,
    struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);

int32_t of_ttp_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_array_of_iv_pairs * prev_record_key_p, uint32_t * count_p,
    struct cm_array_of_iv_pairs ** next_n_record_data_p);

int32_t of_ttp_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_array_of_iv_pairs ** pIvPairArr);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */
int32_t of_ttp_validatemandparams (struct cm_array_of_iv_pairs *
    pMandParams,
    struct cm_app_result **
    presult_p);

int32_t of_ttp_validateoptparams (struct cm_array_of_iv_pairs *
    pOptParams,
    struct cm_app_result ** presult_p);


int32_t of_ttp_setmandparams (struct cm_array_of_iv_pairs *
    pMandParams, struct ttp_config_info * of_ttp_node,
    struct cm_app_result ** presult_p);

int32_t of_ttp_getparams (struct ttp_config_info *of_ttp_node,
    struct cm_array_of_iv_pairs * result_iv_pairs_p);

struct cm_dm_app_callbacks of_ttp_callbacks = {
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  of_ttp_getfirstnrecs,
  of_ttp_getnextnrecs,
  of_ttp_getexactrec,
  NULL,
  NULL
};


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * View  Init * * * * * * *  * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t of_ttp_appl_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  cm_register_app_callbacks (CM_ON_DIRECTOR_TTP_APPL_ID, &of_ttp_callbacks);
  return OF_SUCCESS;
}

int32_t of_ttp_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
    uint32_t * count_p,
    struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct ttp_config_info  of_ttp_node;
  int32_t iRes = OF_FAILURE;
  uint32_t uiRecCount = 0;
  char ttp_type[64];

  CM_CBK_DEBUG_PRINT ("entry");
  of_memset (&of_ttp_node, 0, sizeof (struct ttp_config_info));
  of_memset (ttp_type, 0, 64);

  iRes = get_first_ttp(&of_ttp_node);
  if ( iRes != CNTLR_TTP_SUCCESS)
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

  of_ttp_getparams (&of_ttp_node, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *array_of_iv_pair_arr_p = result_iv_pairs_p;
  *count_p = uiRecCount;
  return OF_SUCCESS;
}

int32_t of_ttp_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_array_of_iv_pairs *prev_record_key_p, uint32_t * count_p,
    struct cm_array_of_iv_pairs ** next_n_record_data_p)
{

  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct cm_app_result *app_result_p = NULL;
  struct  ttp_config_info of_ttp_node;
  int32_t iRes = OF_FAILURE;
  uint32_t uiRecCount = 0;

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&of_ttp_node, 0, sizeof (struct ttp_config_info));

  if ((of_ttp_setmandparams (keys_arr_p, &of_ttp_node, &app_result_p)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  if ((of_ttp_setmandparams (prev_record_key_p, &of_ttp_node, &app_result_p)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  iRes = get_next_ttp(of_ttp_node.name, &of_ttp_node);

  if (iRes != CNTLR_TTP_SUCCESS)
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
  of_ttp_getparams (&of_ttp_node, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  CM_CBK_DEBUG_PRINT ("Number of records requested were %ld ", *count_p);
  CM_CBK_DEBUG_PRINT ("Number of records found are %ld", uiRecCount+1);
  *next_n_record_data_p = result_iv_pairs_p;
  *count_p = uiRecCount;
  return OF_SUCCESS;
}

int32_t of_ttp_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_array_of_iv_pairs ** pIvPairArr)
{

  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct cm_app_result *app_result_p = NULL;
  struct  ttp_config_info of_ttp_node;
  int32_t iRes = OF_FAILURE;

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&of_ttp_node, 0, sizeof (struct ttp_config_info));

  if ((of_ttp_setmandparams (keys_arr_p, &of_ttp_node, &app_result_p)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  iRes = get_exact_ttp(of_ttp_node.name, &of_ttp_node);
  if (iRes != CNTLR_TTP_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("of_ttp_get_first_ttp_entry");
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
  of_ttp_getparams (&of_ttp_node, result_iv_pairs_p);
  *pIvPairArr = result_iv_pairs_p;
  return OF_SUCCESS;
}

int32_t of_ttp_setmandparams (struct cm_array_of_iv_pairs *
    pMandParams, struct ttp_config_info * of_ttp_node,
    struct cm_app_result ** presult_p)
{
  uint32_t uiMandParamCnt;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;
      uiMandParamCnt++)
  {
    switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
    {

      case CM_DM_TTP_TTPNAME_ID:
        of_strncpy (of_ttp_node->name,
            (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
            pMandParams->iv_pairs[uiMandParamCnt].value_length);
        break;
      case CM_DM_TTP_NUMOFTABLES_ID:
         of_ttp_node->no_of_tbls = 
                    of_atoi((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        break;
      case CM_DM_TTP_NUMOFDOMAINS_ID:
         of_ttp_node->no_of_domains = 
                    of_atoi((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        break;

    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
  return OF_SUCCESS;
}

int32_t of_ttp_getparams (struct ttp_config_info *of_ttp_node,
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

  /* name*/
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_TTP_TTPNAME_ID,
      CM_DATA_TYPE_STRING, of_ttp_node->name);
  uindex_i++;

  /*ttp no of domains */
  of_memset(buf,0,sizeof(buf));
  of_itoa(of_ttp_node->no_of_domains, buf);
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_TTP_NUMOFDOMAINS_ID,
      CM_DATA_TYPE_STRING, buf);
  uindex_i++;

  /*ttp no of tables*/
  of_memset(buf,0,sizeof(buf));
  of_itoa(of_ttp_node->no_of_tbls, buf);
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_TTP_NUMOFTABLES_ID,
      CM_DATA_TYPE_STRING, buf);
  uindex_i++;

  result_iv_pairs_p->count_ui = uindex_i;
  CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);
  return OF_SUCCESS;
}


#endif /* OF_CM_SUPPORT */
