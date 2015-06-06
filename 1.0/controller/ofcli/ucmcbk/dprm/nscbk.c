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
 * File name: switchcbk.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: 
 * 
 */

#ifdef OF_CM_SUPPORT


#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"

int32_t of_namespace_appl_ucmcbk_init (void);
int32_t of_namespace_addrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * mand_iv_pairs_p,
    struct cm_array_of_iv_pairs * opt_iv_pairs_p,
    struct cm_app_result ** result_p);

int32_t of_namespace_setrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * mand_iv_pairs_p,
    struct cm_array_of_iv_pairs * opt_iv_pairs_p,
    struct cm_app_result ** result_p);

int32_t of_namespace_delrec (void * config_transaction_p,
    struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_app_result ** result_p);

int32_t of_namespace_getfirstnrecs (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    uint32_t * count_p,
    struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);

int32_t of_namespace_getnextnrecs (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs * prev_record_key_p, uint32_t * count_p,
    struct cm_array_of_iv_pairs ** next_n_record_data_p);

int32_t of_namespace_getexactrec (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t of_namespace_verifycfg (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    uint32_t command_id, struct cm_app_result ** result_p);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */
int32_t of_namespace_ucm_validatemandparams (struct cm_array_of_iv_pairs *
    mand_iv_pairs_p,
    struct cm_app_result **
    presult_p);

int32_t of_namespace_ucm_validateoptparams (struct cm_array_of_iv_pairs *
    opt_iv_pairs_p,
    struct cm_app_result ** presult_p);


int32_t of_namespace_ucm_setmandparams (struct cm_array_of_iv_pairs *
    mand_iv_pairs_p,
    struct dprm_namespace_info * of_namespace,
    struct cm_app_result ** presult_p);

int32_t of_namespace_ucm_setoptparams (struct cm_array_of_iv_pairs *
    opt_iv_pairs_p,
    struct dprm_namespace_info * of_namespace,
    struct cm_app_result ** presult_p);


int32_t of_namespace_ucm_getparams (struct dprm_namespace_info *of_namespace,
    struct cm_array_of_iv_pairs * result_iv_pairs_p);



struct cm_dm_app_callbacks of_namespace_ucm_callbacks = {
  NULL,
  of_namespace_addrec,
  of_namespace_setrec,
  of_namespace_delrec,
  NULL,
  of_namespace_getfirstnrecs,
  of_namespace_getnextnrecs,
  of_namespace_getexactrec,
  of_namespace_verifycfg,
  NULL
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * Namespace  UCM Init * * * * * * *  * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t of_namespace_appl_ucmcbk_init (void)
{
  CM_CBK_DEBUG_PRINT ("namespace -Entry");
  cm_register_app_callbacks (CM_ON_DIRECTOR_NAMESPACE_APPL_ID, &of_namespace_ucm_callbacks);
  return OF_SUCCESS;
}

int32_t of_namespace_addrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * mand_iv_pairs_p,
    struct cm_array_of_iv_pairs * opt_iv_pairs_p,
    struct cm_app_result ** result_p)
{
  struct cm_app_result *of_namespace_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct dprm_namespace_info of_namespace = { };
  uint64_t namespace_handle;

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&of_namespace, 0, sizeof (struct dprm_namespace_info));

  if ((of_namespace_ucm_setmandparams (mand_iv_pairs_p, &of_namespace, &of_namespace_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    *result_p=of_namespace_result;
    return OF_FAILURE;
  }

  return_value = of_namespace_ucm_setoptparams (opt_iv_pairs_p, &of_namespace, &of_namespace_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
    *result_p = of_namespace_result;
    return OF_FAILURE;
  }
 return_value = dprm_validate_namespace(of_namespace.name);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Namespace validate Failed");
    *result_p = of_namespace_result;
    return OF_FAILURE;
  }

  return_value = dprm_register_namespace(&of_namespace,&namespace_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Namespace Addition Failed");
    fill_app_result_struct (&of_namespace_result, NULL, CM_GLU_NAMESPACE_ADD_FAILED);
    *result_p = of_namespace_result;
    return OF_FAILURE;
  }

  return OF_SUCCESS;
}

int32_t of_namespace_setrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * mand_iv_pairs_p,
    struct cm_array_of_iv_pairs * opt_iv_pairs_p,
    struct cm_app_result ** result_p)
{
  struct cm_app_result *of_namespace_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct dprm_namespace_info of_namespace = { };
  uint64_t namespace_handle;


  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&of_namespace, 0, sizeof (struct dprm_namespace_info));

  if ((of_namespace_ucm_setmandparams (mand_iv_pairs_p, &of_namespace, &of_namespace_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    *result_p=of_namespace_result;
    return OF_FAILURE;
  }


  return_value=dprm_get_namespace_handle(of_namespace.name, &namespace_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Namespace does not exist with the name %s",of_namespace.name);
    fill_app_result_struct (&of_namespace_result, NULL, CM_GLU_NAMESPACE_DOESNOT_EXIST);
    *result_p = of_namespace_result;
    return OF_FAILURE;
  }

  of_memset (&of_namespace, 0, sizeof (struct dprm_namespace_info));
  return_value=dprm_get_exact_namespace(namespace_handle, &of_namespace);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Unable to find the matching record");

    fill_app_result_struct (&of_namespace_result, NULL, CM_GLU_NO_MATCHING_RECORD);
    *result_p = of_namespace_result;
    return OF_FAILURE;
  }

  return_value = of_namespace_ucm_setoptparams (opt_iv_pairs_p, &of_namespace, &of_namespace_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
    *result_p = of_namespace_result;
    return OF_FAILURE;
  }

  return_value = dprm_update_namespace(namespace_handle,&of_namespace);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("namespace updation Failed");
    fill_app_result_struct (&of_namespace_result, NULL, CM_GLU_NAMESPACE_UPDATE_FAILED);
    *result_p = of_namespace_result;
    return OF_FAILURE;
  }

  return OF_SUCCESS;
}

int32_t of_namespace_delrec (void * config_transaction_p,
    struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_app_result ** result_p)
{
  struct cm_app_result *of_namespace_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct dprm_namespace_info of_namespace = { };
  uint64_t namespace_handle;

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&of_namespace, 0, sizeof (struct dprm_namespace_info));

  if ((of_namespace_ucm_setmandparams (key_iv_pairs_p, &of_namespace, &of_namespace_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&of_namespace_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p = of_namespace_result;
    return OF_FAILURE;
  }

  return_value=dprm_get_namespace_handle(of_namespace.name, &namespace_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: namespace does not exist with name %s",of_namespace.name);
    fill_app_result_struct (&of_namespace_result, NULL, CM_GLU_NAMESPACE_DOESNOT_EXIST);
    *result_p = of_namespace_result;
    return OF_FAILURE;
  }

  return_value = dprm_unregister_namespace(namespace_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Delete Namespace Record failed");
    fill_app_result_struct (&of_namespace_result, NULL, CM_GLU_NAMESPACE_DELETE_FAILED);
    *result_p = of_namespace_result;
    return OF_FAILURE;
  }
  return OF_SUCCESS;
}

int32_t of_namespace_getfirstnrecs (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    uint32_t * count_p,
    struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{

  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct dprm_namespace_info of_namespace;
  uint64_t namespace_handle;
  int32_t iRes = OF_FAILURE;
  uint32_t uiRecCount = 0;

  CM_CBK_DEBUG_PRINT ("entry");

  of_memset (&of_namespace, 0, sizeof (struct dprm_namespace_info));
  iRes=dprm_get_first_namespace(&of_namespace,  &namespace_handle);
  if (iRes != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Get first record failed for Namespace");
    return OF_FAILURE;
  }
  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    return OF_FAILURE;
  }

  of_namespace_ucm_getparams (&of_namespace, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *array_of_iv_pair_arr_p = result_iv_pairs_p;
  *count_p = uiRecCount;
  return OF_SUCCESS;
}


int32_t of_namespace_getnextnrecs (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs *prev_record_key_p, uint32_t * count_p,
    struct cm_array_of_iv_pairs ** next_n_record_data_p)
{

  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct cm_app_result *app_result_p = NULL;
  struct dprm_namespace_info of_namespace;
  uint64_t namespace_handle;
  struct cm_app_result *of_namespace_result = NULL;
  int32_t iRes = OF_FAILURE;
  uint32_t uiRecCount = 0;

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&of_namespace, 0, sizeof (struct dprm_namespace_info));

  if ((of_namespace_ucm_setmandparams (prev_record_key_p, &of_namespace, &app_result_p)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  iRes=dprm_get_namespace_handle(of_namespace.name, &namespace_handle);
  if (iRes != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: namespace does not exist with name %s",of_namespace.name);
    return OF_FAILURE;
  }

  of_memset (&of_namespace, 0, sizeof (struct dprm_namespace_info));
  iRes=dprm_get_next_namespace(&of_namespace, &namespace_handle);
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
  CM_CBK_DEBUG_PRINT ("Next Namespace record is : %s ",
      of_namespace.name);
  of_namespace_ucm_getparams (&of_namespace, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  CM_CBK_DEBUG_PRINT ("Number of records requested were %ld ", *count_p);
  CM_CBK_DEBUG_PRINT ("Number of records found are %ld", uiRecCount+1);
  *next_n_record_data_p = result_iv_pairs_p;
  *count_p = uiRecCount;
  return OF_SUCCESS;
}

int32_t of_namespace_getexactrec (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs ** pIvPairArr)
{

  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct cm_app_result *of_namespace_result = NULL;
  struct dprm_namespace_info of_namespace;
  uint64_t namespace_handle;
  uint32_t ii = 0;
  int32_t iRes = OF_FAILURE;

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&of_namespace, 0, sizeof (struct dprm_namespace_info));

  if ((of_namespace_ucm_setmandparams (key_iv_pairs_p, &of_namespace, &of_namespace_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    //fill_app_result_struct (&of_namespace_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    //*result_p = of_namespace_result;
    return OF_FAILURE;
  }

  iRes=dprm_get_namespace_handle(of_namespace.name, &namespace_handle);
  if (iRes != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: namespace does not exist with name %s",of_namespace.name);
    return OF_FAILURE;
  }

  of_memset (&of_namespace, 0, sizeof (struct dprm_namespace_info));
  iRes=dprm_get_exact_namespace(namespace_handle, &of_namespace);
  if (iRes != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Unable to find the matching record ");
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
  of_namespace_ucm_getparams (&of_namespace, result_iv_pairs_p);
  *pIvPairArr = result_iv_pairs_p;
  return OF_SUCCESS;
}

int32_t of_namespace_verifycfg (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    uint32_t command_id, struct cm_app_result ** result_p)
{
  struct cm_app_result *of_namespace_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct dprm_namespace_info of_namespace = { };

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&of_namespace, 0, sizeof (struct dprm_namespace_info));

  return_value = of_namespace_ucm_validatemandparams (key_iv_pairs_p, &of_namespace_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    return OF_FAILURE;
  }
#if 0
  if (of_namespace_ucm_validateoptparams (key_iv_pairs_p, &of_namespace_result)
      != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    return OF_FAILURE;
  }
#endif
  *result_p = of_namespace_result;
  return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */

int32_t of_namespace_ucm_validatemandparams (struct cm_array_of_iv_pairs *
    mand_iv_pairs_p,
    struct cm_app_result **  presult_p)
{
  uint32_t count;
  struct cm_app_result *of_namespace_result = NULL;


  CM_CBK_DEBUG_PRINT ("Entered");
  for (count = 0; count < mand_iv_pairs_p->count_ui;
      count++)
  {
    switch (mand_iv_pairs_p->iv_pairs[count].id_ui)
    {

      case CM_DM_NAMESPACE_NAME_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Namespace Name is NULL");
          fill_app_result_struct (&of_namespace_result, NULL, CM_GLU_NAMESPACE_NAME_NULL);
          *presult_p = of_namespace_result;
          return OF_FAILURE;
        }
        break;

    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs_p);
  return OF_SUCCESS;
}

int32_t of_namespace_ucm_validateoptparams (struct cm_array_of_iv_pairs *
    opt_iv_pairs_p,
    struct cm_app_result ** presult_p)
{
  uint32_t count;
  struct cm_app_result *of_namespace_result = NULL;
  uint8_t switch_type;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (count = 0; count < opt_iv_pairs_p->count_ui; count++)
  {
    switch (opt_iv_pairs_p->iv_pairs[count].id_ui)
    {
      case CM_DM_NAMESPACE_NAME_ID:
        if (opt_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Fully Qualified Domain Name (FQDN) is NULL");
          fill_app_result_struct (&of_namespace_result, NULL, CM_GLU_NAMESPACE_NAME_NULL);
          *presult_p = of_namespace_result;
          return OF_FAILURE;
        }
        break;
      default:
        break;
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (opt_iv_pairs_p);
  return OF_SUCCESS;
}

int32_t of_namespace_ucm_setmandparams (struct cm_array_of_iv_pairs *
    mand_iv_pairs_p,
    struct dprm_namespace_info * of_namespace,
    struct cm_app_result ** presult_p)
{
  uint32_t count;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (count = 0; count < mand_iv_pairs_p->count_ui;
      count++)
  {
    switch (mand_iv_pairs_p->iv_pairs[count].id_ui)
    {

      case CM_DM_NAMESPACE_NAME_ID:
        of_strncpy (of_namespace->name,
            (char *) mand_iv_pairs_p->iv_pairs[count].value_p,
            mand_iv_pairs_p->iv_pairs[count].value_length);
        break;

    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs_p);
  return OF_SUCCESS;
}

int32_t of_namespace_ucm_setoptparams (struct cm_array_of_iv_pairs *
    opt_iv_pairs_p,
    struct dprm_namespace_info * of_namespace,
    struct cm_app_result ** presult_p)
{
  uint32_t count;

  CM_CBK_DEBUG_PRINT ("Entered");

  for (count = 0; count < opt_iv_pairs_p->count_ui; count++)
  {
    switch (opt_iv_pairs_p->iv_pairs[count].id_ui)
    {
      default:
        break;
    }
  }

  CM_CBK_PRINT_IVPAIR_ARRAY (opt_iv_pairs_p);
  return OF_SUCCESS;
}



int32_t of_namespace_ucm_getparams (struct dprm_namespace_info *of_namespace,
    struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
  uint32_t uindex_i = 0;
  char buf[10] = "";
#define CM_NAMESPACE_CHILD_COUNT 1

  result_iv_pairs_p->iv_pairs =
    (struct cm_iv_pair *) of_calloc (CM_NAMESPACE_CHILD_COUNT, sizeof (struct cm_iv_pair));
  if (result_iv_pairs_p->iv_pairs == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p->iv_pairs");
    return OF_FAILURE;
  }

  /* Namespace Interface*/
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_NAMESPACE_NAME_ID,
      CM_DATA_TYPE_STRING, of_namespace->name);
  uindex_i++;


  result_iv_pairs_p->count_ui = uindex_i;
  CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);
  return OF_SUCCESS;
}


#endif /* OF_CM_SUPPORT */
