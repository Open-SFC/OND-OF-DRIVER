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

int32_t of_band_appl_ucmcbk_init (void);


void* of_band_starttrans(struct cm_array_of_iv_pairs   * key_iv_pairs,
    uint32_t command_id,
    struct cm_app_result ** result_pp);

int32_t  of_band_endtrans(void *config_trans_p,
    uint32_t command_id,
    struct cm_app_result ** result_pp);

int32_t of_band_addrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * mand_iv_pairs,
    struct cm_array_of_iv_pairs * opt_iv_pairs,
    struct cm_app_result ** result_pp);

int32_t of_band_setrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * mand_iv_pairs,
    struct cm_array_of_iv_pairs * opt_iv_pairs,
    struct cm_app_result ** result_pp);

int32_t of_band_delrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * key_iv_pairs,
    struct cm_app_result ** result_pp);

int32_t of_band_getfirstnrecs (struct cm_array_of_iv_pairs * key_iv_pairs,
    uint32_t * pCount,
    struct cm_array_of_iv_pairs ** pArrayofIvPairArr);

int32_t of_band_getnextnrecs (struct cm_array_of_iv_pairs * key_iv_pairs,
    struct cm_array_of_iv_pairs * pPrevRecordKey, uint32_t * pCount,
    struct cm_array_of_iv_pairs ** pNextNRecordData_p);

int32_t of_band_getexactrec (struct cm_array_of_iv_pairs * key_iv_pairs,
    struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t of_band_verifycfg (struct cm_array_of_iv_pairs * key_iv_pairs,
    uint32_t command_id, struct cm_app_result ** result_pp);

int32_t of_band_ucm_getparams (struct ofi_meter_band *meter_rec_info,
    struct cm_array_of_iv_pairs * result_iv_pairs_p);

int32_t of_band_ucm_setmandparams (struct cm_array_of_iv_pairs *
    mand_iv_pairs,
    struct  ofi_meter_rec_info *meter_rec_info,
    struct ofi_meter_band *band_info,
    struct cm_app_result ** app_result_pp);

int32_t of_band_ucm_setoptparams (struct cm_array_of_iv_pairs *
    opt_iv_pairs,
    struct ofi_meter_band * band_info,
    struct cm_app_result ** app_result_pp);

struct cm_dm_app_callbacks of_band_ucm_callbacks = 
{
  of_band_starttrans,
  of_band_addrec,
  of_band_setrec,
  of_band_delrec,
  of_band_endtrans,
  of_band_getfirstnrecs,
  of_band_getnextnrecs,
  of_band_getexactrec,
  of_band_verifycfg,
  NULL
};

extern of_list_t meter_trans_list_g;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Function Name:
* Description:
* Input:
* Output:
* Result:
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_meter_band_appl_ucmcbk_init (void)
{
  int32_t return_value;

  return_value=cm_register_app_callbacks (CM_ON_DIRECTOR_DATAPATH_METERS_METER_BANDS_APPL_ID,
      &of_band_ucm_callbacks);
  if(return_value != OF_SUCCESS)
  {
    cntrlrucm_debugmsg("band application CBK registration failed");
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
int32_t of_band_verifycfg (struct cm_array_of_iv_pairs * key_iv_pairs,
    uint32_t command_id, struct cm_app_result ** result_pp)
{
  struct cm_app_result *of_band_result = NULL;
  int32_t return_value = OF_FAILURE;

  CM_CBK_DEBUG_PRINT ("Entered");

  return_value = of_band_ucm_validatemandparams (key_iv_pairs, &of_band_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    return OF_FAILURE;
  }
  *result_pp = of_band_result;
  return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Function Name:
* Description:
* Input:
* Output:
* Result:
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void* of_band_starttrans(struct cm_array_of_iv_pairs   * key_iv_pairs,
    uint32_t command_id,
    struct cm_app_result ** result_pp)
{
  struct meter_trans *trans_rec;
  struct ofi_meter_rec_info meter_rec_info={};
  struct ofi_meter_band *band_info=NULL;
  struct cm_app_result *of_band_result = NULL;
  int32_t return_value;

  uchar8_t offset;
  offset = OF_METER_TRANS_LISTNODE_OFFSET;

  CM_CBK_DEBUG_PRINT ("Entered");

  band_info=(struct ofi_meter_band *) calloc (1, sizeof(struct ofi_meter_band));
  if( band_info == NULL )
  {
    CM_CBK_DEBUG_PRINT ("memory allocation failed");
    return NULL;
  }

  if ((of_band_ucm_setmandparams (key_iv_pairs,&meter_rec_info, band_info, &of_band_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return NULL;
  }

  OF_LIST_SCAN(meter_trans_list_g, trans_rec, struct meter_trans *,offset )
  {
    if(trans_rec->meter_id == meter_rec_info.meter_id)
    {
      CM_CBK_DEBUG_PRINT ("transaction found");
      break;
    }

  }

#if 1
  return_value =of_register_band_to_meter(trans_rec->datapath_handle,band_info,meter_rec_info.meter_id);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Band Addition to meter %d Failed", meter_rec_info.meter_id);
    fill_app_result_struct (&of_band_result, NULL, CM_GLU_BAND_ADD_FAILED);
    *result_pp =of_band_result;
    return NULL;
  }
#endif
  CM_CBK_DEBUG_PRINT ("transaction pointer %p",trans_rec);
  return (void *)trans_rec;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Function Name:
* Description:
* Input:
* Output:
* Result:
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t of_band_addrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * mand_iv_pairs,
    struct cm_array_of_iv_pairs * opt_iv_pairs,
    struct cm_app_result ** result_pp)
{
  struct cm_app_result *of_band_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct ofi_meter_band band_info = { };
  struct ofi_meter_rec_info meter_rec_info = { };
  struct meter_trans *trans_rec=(struct meter_trans *)config_trans_p;

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&band_info, 0, sizeof (struct ofi_meter_band));

  CM_CBK_DEBUG_PRINT ("trans id %d command id %d sub cmd id %d meter id %d",trans_rec->trans_id,trans_rec->command_id,
      trans_rec->sub_command_id,trans_rec->meter_id);

  if ((of_band_ucm_setmandparams (mand_iv_pairs,&meter_rec_info, &band_info, &of_band_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    *result_pp=of_band_result;
    return OF_FAILURE;
  }

  return_value =of_band_ucm_setoptparams (opt_iv_pairs, &band_info, &of_band_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
    *result_pp =of_band_result;
    return OF_FAILURE;
  }
#if 1
  return_value =of_update_band_in_meter(trans_rec->datapath_handle, &band_info,meter_rec_info.meter_id);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("BAND updation at meter %d failed", meter_rec_info.meter_id);
    fill_app_result_struct (&of_band_result, NULL, CM_GLU_BAND_UPDATE_FAILED);
    *result_pp =of_band_result;
    return OF_FAILURE;
  }
#endif
  return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Function Name:
* Description:
* Input:
* Output:
* Result:
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_band_setrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * mand_iv_pairs,
    struct cm_array_of_iv_pairs * opt_iv_pairs,
    struct cm_app_result ** result_pp)
{
  struct cm_app_result *of_band_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct ofi_meter_band band_info = { };
  struct dprm_switch_runtime_info runtime_info;
  struct meter_trans *trans_rec=config_trans_p;


  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&band_info, 0, sizeof (struct ofi_meter_band));
#if 0
  if ((of_band_ucm_setmandparams (mand_iv_pairs,&meter_rec_info, &band_info, &of_band_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    *result_pp=of_band_result;
    return OF_FAILURE;
  }


  return_value=of_band_unregister_bands(band_info.meter_id);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: meter doesn't exists with name %d",band_info.meter_id);
    fill_app_result_struct (&of_band_result, NULL, CM_GLU_VLAN_FAILED);
    *result_pp =of_band_result;
    return OF_FAILURE;
  }

#endif 
  return OF_FAILURE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Function Name:
* Description:
* Input:
* Output:
* Result:
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_band_delrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * key_iv_pairs,
    struct cm_app_result ** result_pp)
{
  struct cm_app_result *of_band_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct ofi_meter_band band_info = { };
  struct ofi_meter_rec_info meter_rec_info = { };
  struct meter_trans *trans_rec=config_trans_p;

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&band_info, 0, sizeof (struct ofi_meter_band));

  if ( trans_rec->command_id != CM_CMD_ADD_PARAMS)
  {
    CM_CBK_DEBUG_PRINT ("Delete not supported");
    return OF_FAILURE;
  }

  if ((of_band_ucm_setmandparams (key_iv_pairs,&meter_rec_info, &band_info, &of_band_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&of_band_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_pp =of_band_result;
    return OF_FAILURE;
  }

  return_value=of_meter_unregister_band(trans_rec->datapath_handle,meter_rec_info.meter_id, band_info.band_id);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("BAND does not exist with id %d",band_info.band_id);

    fill_app_result_struct (&of_band_result, NULL, CM_GLU_BAND_DOESNOT_EXIST);
    *result_pp =of_band_result;
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
int32_t  of_band_endtrans(void *config_trans_p,
    uint32_t command_id,
    struct cm_app_result ** result_pp)
{
  struct meter_trans *trans_rec=config_trans_p;
  struct ofi_meter_band meter_rec_info={};
  struct cm_app_result *of_band_result = NULL;

  CM_CBK_DEBUG_PRINT ("Entered");

  if ( trans_rec )
  {
    CM_CBK_DEBUG_PRINT ("trans id %d command id %d sub cmd id %d meter id %d",trans_rec->trans_id,trans_rec->command_id,
        trans_rec->sub_command_id,trans_rec->meter_id);
  }
  else
  {
    CM_CBK_DEBUG_PRINT ("trans is null");
  }

  switch(command_id)
  {

    case CM_CMD_CONFIG_SESSION_REVOKE:
      CM_CBK_DEBUG_PRINT ("Revoke command");
      break;
    case CM_CMD_CONFIG_SESSION_COMMIT:
      CM_CBK_DEBUG_PRINT ("Commit command");
      break;
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
int32_t of_band_getfirstnrecs(struct cm_array_of_iv_pairs * key_iv_pairs,
    uint32_t * pCount,
    struct cm_array_of_iv_pairs ** arr_ivpair_arr_pp)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  struct cm_app_result *of_band_result = NULL;
  struct ofi_meter_rec_info meter_rec_info={};
  struct ofi_meter_band band_info={};


  CM_CBK_DEBUG_PRINT ("not supporting");
  return OF_FAILURE;

  of_memset (&meter_rec_info, 0, sizeof (struct ofi_meter_rec_info));
  of_memset (&band_info, 0, sizeof (struct ofi_meter_band));
  if ((of_band_ucm_setmandparams (key_iv_pairs,&meter_rec_info, &band_info, &of_band_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }


  //	return_value=cntlr_send_table_stats_request(1);
  if ( return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Table stats failed");
    return OF_FAILURE;
  } 
  CM_CBK_DEBUG_PRINT ("Table stats will be sent as a multipart response");
  return CM_CNTRL_MULTIPART_RESPONSE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Function Name:
* Description:
* Input:
* Output:
* Result:
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_band_getnextnrecs (struct cm_array_of_iv_pairs * key_iv_pairs,
    struct cm_array_of_iv_pairs *pPrevRecordKey, uint32_t * pCount,
    struct cm_array_of_iv_pairs ** pNextNRecordData_p)
{
  CM_CBK_DEBUG_PRINT ("no next records");
  *pCount=0;
  return OF_FAILURE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Function Name:
* Description:
* Input:
* Output:
* Result:
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_band_getexactrec (struct cm_array_of_iv_pairs * key_iv_pairs,
    struct cm_array_of_iv_pairs ** pIvPairArr)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct cm_app_result *of_band_result = NULL;
  uint32_t ii = 0;
  int32_t return_value = OF_FAILURE;

  CM_CBK_DEBUG_PRINT ("no exact record");
  return OF_FAILURE;

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Function Name:
* Description:
* Input:
* Output:
* Result:
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_band_ucm_setmandparams (struct cm_array_of_iv_pairs *
    mand_iv_pairs,
    struct ofi_meter_rec_info *meter_rec_info,
    struct ofi_meter_band *band_info,
    struct cm_app_result ** app_result_pp)
{
  uint32_t mand_param_cnt;
  uint32_t meter_id;
  uint32_t band_id;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (mand_param_cnt = 0; mand_param_cnt < mand_iv_pairs->count_ui;mand_param_cnt++)
  {
    switch (mand_iv_pairs->iv_pairs[mand_param_cnt].id_ui)
    {
      case CM_DM_METERREC_METERID_ID:
      meter_id=of_atoi((char *) mand_iv_pairs->iv_pairs[mand_param_cnt].value_p);
      meter_rec_info->meter_id=meter_id;
      CM_CBK_DEBUG_PRINT ("meter id is %d", meter_id);
      break;
      
      case  CM_DM_BANDS_BANDID_ID:
      band_id=of_atoi((char *) mand_iv_pairs->iv_pairs[mand_param_cnt].value_p);
      band_info->band_id=band_id;
      CM_CBK_DEBUG_PRINT ("band id %d", band_id);
      break;
      
      case CM_DM_BANDS_BANDTYPE_ID:
      if (!strncmp (mand_iv_pairs->iv_pairs[mand_param_cnt].value_p,"OFPMBT_DROP", 
              mand_iv_pairs->iv_pairs[mand_param_cnt].value_length))
      {
          band_info->type = OFPMBT_DROP;
      }
      else if (!strncmp (mand_iv_pairs->iv_pairs[mand_param_cnt].value_p, "OFPMBT_DSCP_REMARK",
              mand_iv_pairs->iv_pairs[mand_param_cnt].value_length))
      {
          band_info->type = OFPMBT_DSCP_REMARK;
      }
      else if (!strncmp (mand_iv_pairs->iv_pairs[mand_param_cnt].value_p, "OFPMBT_EXPERIMENTER",
               mand_iv_pairs->iv_pairs[mand_param_cnt].value_length))
      {
          band_info->type = OFPMBT_EXPERIMENTER;
      }
      else
        CM_CBK_DEBUG_PRINT("Invalid band Type");

      CM_CBK_DEBUG_PRINT ("type  %d", band_info->type);
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
int32_t of_band_ucm_validatemandparams (struct cm_array_of_iv_pairs *
    mand_iv_pairs,
    struct cm_app_result ** app_result_pp)
{
  uint32_t opt_param_cnt;
  uint32_t meter_id;
  uint8_t type;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (opt_param_cnt = 0; opt_param_cnt < mand_iv_pairs->count_ui;
      opt_param_cnt++)
  {
    switch (mand_iv_pairs->iv_pairs[opt_param_cnt].id_ui)
    {
      case CM_DM_METERREC_METERID_ID:
        meter_id=of_atoi((char *) mand_iv_pairs->iv_pairs[opt_param_cnt].value_p);
        CM_CBK_DEBUG_PRINT ("meter id is %d", meter_id);
        break;
      case CM_DM_BANDS_BANDTYPE_ID:
        type=ucm_uint8_from_str_ptr((char *) mand_iv_pairs->iv_pairs[opt_param_cnt].value_p);
        CM_CBK_DEBUG_PRINT ("type is %d", type);
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
int32_t of_band_ucm_setoptparams (struct cm_array_of_iv_pairs *
    opt_iv_pairs,
    struct ofi_meter_band * band_info,
    struct cm_app_result ** app_result_pp)
{
  uint32_t opt_param_cnt;
  uint16_t len; 
  uint32_t rate;
  uint32_t burst_size; 
  uint8_t prec_level; 

  CM_CBK_DEBUG_PRINT ("Entered");

  for (opt_param_cnt = 0; opt_param_cnt < opt_iv_pairs->count_ui; opt_param_cnt++)
  {
    switch (opt_iv_pairs->iv_pairs[opt_param_cnt].id_ui)
    {
      case CM_DM_BANDS_RATE_ID:
        rate=of_atoi((char *) opt_iv_pairs->iv_pairs[opt_param_cnt].value_p);
        band_info->rate=rate;
        CM_CBK_DEBUG_PRINT ("rate  %d", rate);
        break;
      case CM_DM_BANDS_BURST_SIZE_ID:
        burst_size=of_atoi((char *) opt_iv_pairs->iv_pairs[opt_param_cnt].value_p);
        band_info->burst_size=burst_size;
        CM_CBK_DEBUG_PRINT ("burst_size  %d", burst_size);
        break;
      case CM_DM_BANDS_PREC_LEVEL_ID:
        prec_level=ucm_uint8_from_str_ptr((char *)opt_iv_pairs->iv_pairs[opt_param_cnt].value_p);
        band_info->prec_level=prec_level;
        CM_CBK_DEBUG_PRINT ("prec_level  %c", prec_level);
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
int32_t of_band_ucm_getparams (struct ofi_meter_band *band_info,
    struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
  char string[64];
  int32_t index = 0, jj, count_ui, iIndex;
#define CM_BAND_CHILD_COUNT 2
  count_ui = CM_BAND_CHILD_COUNT;
#if 0
  //CM_CBK_DEBUG_PRINT ("Entered");
  result_iv_pairs_p->iv_pairs = (struct cm_iv_pai*)of_calloc(count_ui, sizeof(struct cm_iv_pair));
  if(!result_iv_pairs_p->iv_pairs)
  {
    CM_CBK_DEBUG_PRINT (" Memory allocation failed ");
    return OF_FAILURE;
  }

  of_memset(string, 0, sizeof(string));
  of_itoa (band_info->meter_id, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_GROUPDESC_meter_id_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_sprintf(string,"%d",band_info->group_type);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_TABLESTATS_ACTIVECOUNT_ID,CM_DATA_TYPE_STRING, string);
  index++;


  result_iv_pairs_p->count_ui = index;

  //CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);
#endif
  return OF_SUCCESS;
}
#endif /*OF_CM_SUPPORT */
