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
 * File name: swinfocbk.c
 * Author: D Praveen <B38862@freescale.com>
 * Date:   05/27/2013
 * Description: 
 * 
 */

#ifdef OF_CM_SUPPORT

#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "of_utilities.h"
#include "of_multipart.h"

int32_t of_swinfo_appl_ucmcbk_init (void);

int32_t of_swinfo_getexactrec (struct cm_array_of_iv_pairs * pKeysArr,
                               struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t of_swinfo_ucm_getparams (struct ofi_switch_info *swinfo_info,
    struct cm_array_of_iv_pairs * result_iv_pairs_p);

int32_t of_swinfo_ucm_setmandparams (struct cm_array_of_iv_pairs *
    mand_iv_pairs,
    struct dprm_datapath_general_info *datapath_info,
    struct ofi_switch_info *swinfo_info,
    struct cm_app_result ** app_result_pp);

struct cm_dm_app_callbacks of_swinfo_ucm_callbacks = 
{
  NULL, 
  NULL,
  NULL,
  NULL, 
  NULL,  
  NULL,
  NULL,
  of_swinfo_getexactrec,
  NULL,
  NULL
};


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_swinfo_appl_cmcbk_init (void)
{
  int32_t return_value;

  CM_CBK_DEBUG_PRINT ("Entered");
  return_value=cm_register_app_callbacks (CM_ON_DIRECTOR_DATAPATH_SWITCHINFO_APPL_ID,
      &of_swinfo_ucm_callbacks);
  if(return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT("sw info application registration failed");
    return OF_FAILURE;
  }
  return OF_SUCCESS;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_swinfo_verifycfg (struct cm_array_of_iv_pairs * key_iv_pairs,
    uint32_t command_id, struct cm_app_result ** result_pp)
{
  return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_swinfo_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs,
                               struct cm_array_of_iv_pairs ** pIvPairArr)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  struct cm_app_result *swinfo_result = NULL;
  struct ofi_switch_info swinfo_info={};
  struct dprm_datapath_general_info datapath_info={};
  uint64_t datapath_handle;

  CM_CBK_DEBUG_PRINT ("Entered");

  of_memset (&swinfo_info, 0, sizeof (struct ofi_switch_info));
  if ((of_swinfo_ucm_setmandparams (key_iv_pairs,&datapath_info, &swinfo_info, &swinfo_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  return_value=dprm_get_datapath_handle(datapath_info.dpid, &datapath_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: datapath does not exist with id %llx",datapath_info.dpid);
    return OF_FAILURE;
  }

  memset(&swinfo_info, 0, sizeof(struct ofi_switch_info));
  return_value=of_get_switch_info(datapath_handle, &swinfo_info);
  if ( return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("switch info listing failed");
    return OF_FAILURE;
  } 

  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    return OF_FAILURE;
  }
  of_swinfo_ucm_getparams (&swinfo_info, result_iv_pairs_p);
  CM_CBK_DEBUG_PRINT ("getting switch info successful");

  *pIvPairArr = result_iv_pairs_p;
  return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_swinfo_ucm_setmandparams (struct cm_array_of_iv_pairs *
    mand_iv_pairs,
    struct dprm_datapath_general_info *datapath_info,
    struct ofi_switch_info *swinfo_info,
    struct cm_app_result ** app_result_pp)
{
  uint32_t mand_param_cnt;
  uint32_t swinfo_id;
  uint64_t idpId;
  uint8_t type;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (mand_param_cnt = 0; mand_param_cnt < mand_iv_pairs->count_ui;
      mand_param_cnt++)
  {
    switch (mand_iv_pairs->iv_pairs[mand_param_cnt].id_ui)
    {

      case CM_DM_DATAPATH_DATAPATHID_ID:
        idpId=charTo64bitNum((char *) mand_iv_pairs->iv_pairs[mand_param_cnt].value_p);
        CM_CBK_DEBUG_PRINT("dpid is: %llx",idpId);
        datapath_info->dpid=idpId;
        break;
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs);
  return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_swinfo_ucm_getparams (struct ofi_switch_info *swinfo_entry_p,
    struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
  int32_t index = 0, count_ui;

#define CM_SWINFO_CHILD_COUNT 5 
  count_ui = CM_SWINFO_CHILD_COUNT;

  CM_CBK_DEBUG_PRINT ("Entered");
  result_iv_pairs_p->iv_pairs = (struct cm_iv_pair*)of_calloc(count_ui, sizeof(struct cm_iv_pair));
  if(!result_iv_pairs_p->iv_pairs)
  {
    CM_CBK_DEBUG_PRINT (" Memory allocation failed ");
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("Entered");

  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_SWINFO_MANFACTUREDESC_ID,
                   CM_DATA_TYPE_STRING, swinfo_entry_p->mfr_desc);
  index++;

  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_SWINFO_HARDWAREDESC_ID,
                   CM_DATA_TYPE_STRING, swinfo_entry_p->hw_desc);
  index++;
  
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_SWINFO_SOFTWAREDESC_ID,
      CM_DATA_TYPE_STRING, swinfo_entry_p->sw_desc);
  index++;
  
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_SWINFO_SERIALNUMBER_ID,
      CM_DATA_TYPE_STRING, swinfo_entry_p->serial_num);
  index++;
  
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_SWINFO_DATAPATHDESC_ID,
      CM_DATA_TYPE_STRING, swinfo_entry_p->dp_desc);
  index++;

  CM_CBK_DEBUG_PRINT ("Entered");
  result_iv_pairs_p->count_ui = index;
  return OF_SUCCESS;
}
#endif /*OF_CM_SUPPORT */
