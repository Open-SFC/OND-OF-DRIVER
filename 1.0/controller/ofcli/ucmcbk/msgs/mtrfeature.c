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

#ifdef OF_CM_SUPPORT

#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "of_utilities.h"
#include "of_multipart.h"
#include "of_meter.h"

int32_t of_meter_features_appl_ucmcbk_init (void);


void* of_meter_features_starttrans(struct cm_array_of_iv_pairs   * key_iv_pairs,
    uint32_t command_id,
    struct cm_app_result ** result_pp);

int32_t  of_meter_features_endtrans(void *config_trans_p,
    uint32_t command_id,
    struct cm_app_result ** result_pp);

int32_t of_meter_features_addrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * mand_iv_pairs,
    struct cm_array_of_iv_pairs * opt_iv_pairs,
    struct cm_app_result ** result_pp);

int32_t of_meter_features_setrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * mand_iv_pairs,
    struct cm_array_of_iv_pairs * opt_iv_pairs,
    struct cm_app_result ** result_pp);

int32_t of_meter_features_delrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * key_iv_pairs,
    struct cm_app_result ** result_pp);

int32_t of_meter_features_getexactrec (struct cm_array_of_iv_pairs * key_iv_pairs,
    struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t of_meter_features_verifycfg (struct cm_array_of_iv_pairs * key_iv_pairs,
    uint32_t command_id, struct cm_app_result ** result_pp);


int32_t of_meter_features_ucm_getparams (struct ofi_meter_features_info *meter_features_p,
    struct cm_array_of_iv_pairs * result_iv_pairs_p);

int32_t of_meter_features_verifycfg (struct cm_array_of_iv_pairs * key_iv_pairs,
    uint32_t command_id, struct cm_app_result ** result_pp);

int32_t of_meter_features_ucm_setmandparams (struct cm_array_of_iv_pairs *
    mand_iv_pairs,
    struct dprm_datapath_general_info *datapath_info,
    struct cm_app_result ** app_result_pp);

struct cm_dm_app_callbacks of_meter_features_ucm_callbacks = 
{
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  of_meter_features_getexactrec,
  of_meter_features_verifycfg,
  NULL
};


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Function Name:
* Description:
* Input:
* Output:
* Result:
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_meter_features_appl_ucmcbk_init (void)
{
  int32_t return_value;

  CM_CBK_DEBUG_PRINT ("Entered");
  return_value=cm_register_app_callbacks (CM_ON_DIRECTOR_DATAPATH_METERS_FEATURES_APPL_ID,
      &of_meter_features_ucm_callbacks);
  if(return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT("meter features application registration failed");
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
int32_t of_meter_features_verifycfg (struct cm_array_of_iv_pairs * key_iv_pairs,
    uint32_t command_id, struct cm_app_result ** result_pp)
{
  struct cm_app_result *meter_result = NULL;
  int32_t return_value = OF_FAILURE;

  CM_CBK_DEBUG_PRINT ("Entered");

  return_value = of_meter_features_ucm_validatemandparams (key_iv_pairs, &meter_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    return OF_FAILURE;
  }
  *result_pp = meter_result;
  return OF_SUCCESS;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Function Name:
* Description:
* Input:
* Output:
* Result:
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_meter_features_getexactrec (struct cm_array_of_iv_pairs * key_iv_pairs,
    struct cm_array_of_iv_pairs ** pIvPairArr)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  struct cm_app_result *meter_result = NULL;
  struct dprm_datapath_general_info datapath_info={};
  uint64_t datapath_handle;

  CM_CBK_DEBUG_PRINT ("Entered");

  if ((of_meter_features_ucm_setmandparams (key_iv_pairs,&datapath_info,  &meter_result)) !=
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

  return_value=cntlr_send_meter_features_request(datapath_handle);
  if ( return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("cntlr_send_meter_features_request failed");
    return OF_FAILURE;
  } 
  CM_CBK_DEBUG_PRINT ("Meter features will be sent as a multipart response");
  return CM_CNTRL_MULTIPART_RESPONSE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Function Name:
* Description:
* Input:
* Output:
* Result:
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_meter_features_ucm_setmandparams (struct cm_array_of_iv_pairs *
    mand_iv_pairs,
    struct dprm_datapath_general_info *datapath_info,
    struct cm_app_result ** app_result_pp)
{
  uint32_t mand_param_cnt;
  uint32_t meter_id;
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
int32_t of_meter_features_ucm_validatemandparams (struct cm_array_of_iv_pairs *
    mand_iv_pairs,
    struct cm_app_result ** app_result_pp)
{
  uint32_t mand_param_cnt;
  uint32_t meter_id;
  uint8_t type;
  struct cm_app_result *meter_result;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (mand_param_cnt = 0; mand_param_cnt < mand_iv_pairs->count_ui;
      mand_param_cnt++)
  {
    switch (mand_iv_pairs->iv_pairs[mand_param_cnt].id_ui)
    {
      default:
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
int32_t of_meter_features_ucm_setoptparams (struct cm_array_of_iv_pairs *
    opt_iv_pairs,
    struct ofi_meter_rec_info * meter_info,
    struct cm_app_result ** app_result_pp)
{
  uint32_t uiOptParamCnt;
  uint8_t switch_type;

  CM_CBK_DEBUG_PRINT ("Entered");

  for (uiOptParamCnt = 0; uiOptParamCnt < opt_iv_pairs->count_ui; uiOptParamCnt++)
  {
    switch (opt_iv_pairs->iv_pairs[uiOptParamCnt].id_ui)
    {
      default:
        break;
    }
  }

  CM_CBK_PRINT_IVPAIR_ARRAY (opt_iv_pairs);
  return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Function Name:
* Description:
* Input:
* Output:
* Result:
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_meter_features_ucm_getparams (struct ofi_meter_features_info *meter_features_p,
    struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
  char string[1024];
  char tmp[256];
  int32_t i,j, index = 0, jj, count_ui, iIndex;

  char *band_types[]={
   "OFPMBT_DROP",
   "OFPMBT_DSCP_REMARK",
   "OFPMBT_EXPERIMENTER"
  };

  char *meter_capapbilities[]={
    "OFPMF_KBPS",
    "OFPMF_PKTPS",
    "OFPMF_BURST",
    "OFPMF_STATS"
  };

#define CM_METERFEATURES_CHILD_COUNT 5
  count_ui = CM_METERFEATURES_CHILD_COUNT;

  //CM_CBK_DEBUG_PRINT ("Entered");
  result_iv_pairs_p->iv_pairs = (struct cm_iv_pair*)of_calloc(count_ui, sizeof(struct cm_iv_pair));
  if(!result_iv_pairs_p->iv_pairs)
  {
    CM_CBK_DEBUG_PRINT (" Memory allocation failed ");
    return OF_FAILURE;
  }

  of_memset(string, 0, sizeof(string));
  of_itoa (meter_features_p->max_meter,string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_METERFEATURES_MAX_METER_ID, CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  for (i =0; i < 4; i ++ )
  {
    if ((meter_features_p->band_types) & (1 << i))
    {
      strcat(string,band_types[i]);
      strcat(string," ");
    }
  }
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_METERFEATURES_BAND_TYPES_ID, CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  for (i =0; i < 4; i ++ )
  {
    if ( (meter_features_p->capabilities) & (1 << i )) 
    {
      strcat(string, meter_capapbilities[i]);
    }
    strcat(string, " ");
  }
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_METERFEATURES_CAPABILITIES_ID, CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_itoa (meter_features_p->max_bands,string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_METERFEATURES_MAX_BANDS_ID,CM_DATA_TYPE_STRING, string);
  index++;
  
  of_memset(string, 0, sizeof(string));
  of_itoa (meter_features_p->max_color,string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_METERFEATURES_MAX_COLOR_ID,CM_DATA_TYPE_STRING, string);
  index++;

  result_iv_pairs_p->count_ui = index;

  CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);
  CM_CBK_DEBUG_PRINT (" success");
  return OF_SUCCESS;
}
#endif /*OF_CM_SUPPORT */
