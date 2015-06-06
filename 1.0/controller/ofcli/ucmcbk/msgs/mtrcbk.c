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

int32_t of_meter_appl_ucmcbk_init (void);


void* of_meter_starttrans(struct cm_array_of_iv_pairs   * key_iv_pairs,
    uint32_t command_id,
    struct cm_app_result ** result_pp);

int32_t  of_meter_endtrans(void *config_trans_p,
    uint32_t command_id,
    struct cm_app_result ** result_pp);

int32_t of_meter_addrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * mand_iv_pairs,
    struct cm_array_of_iv_pairs * opt_iv_pairs,
    struct cm_app_result ** result_pp);

int32_t of_meter_setrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * mand_iv_pairs,
    struct cm_array_of_iv_pairs * opt_iv_pairs,
    struct cm_app_result ** result_pp);

int32_t of_meter_delrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * key_iv_pairs,
    struct cm_app_result ** result_pp);

int32_t of_meter_getfirstnrecs (struct cm_array_of_iv_pairs * key_iv_pairs,
    uint32_t * count_p,
    struct cm_array_of_iv_pairs ** pArrayofIvPairArr);

int32_t of_meter_getnextnrecs (struct cm_array_of_iv_pairs * key_iv_pairs,
    struct cm_array_of_iv_pairs * prev_rec_iv_pairs, uint32_t * count_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp);

int32_t of_meter_getexactrec (struct cm_array_of_iv_pairs * key_iv_pairs,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp);

int32_t of_meter_ucm_getparams (struct ofi_meter_rec_info *meter_info,
    struct cm_array_of_iv_pairs * result_iv_pairs_p);

int32_t of_meter_verifycfg (struct cm_array_of_iv_pairs * key_iv_pairs,
    uint32_t command_id, struct cm_app_result ** result_pp);

int32_t of_meter_ucm_setmandparams (struct cm_array_of_iv_pairs *mand_iv_pairs,
    struct dprm_datapath_general_info *datapath_info,
    struct ofi_meter_rec_info *meter_rec_info,
    struct cm_app_result ** app_result_pp);

struct cm_dm_app_callbacks of_meter_ucm_callbacks = 
{
  of_meter_starttrans,
  of_meter_addrec,
  of_meter_setrec,
  of_meter_delrec,
  of_meter_endtrans,
  of_meter_getfirstnrecs,
  of_meter_getnextnrecs,
  of_meter_getexactrec,
  of_meter_verifycfg,
  NULL
};

uint32_t metertransId_g=0;
of_list_t meter_trans_list_g={};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_meter_appl_ucmcbk_init (void)
{
  int32_t return_value;

  CM_CBK_DEBUG_PRINT ("Entered");
  return_value=cm_register_app_callbacks (CM_ON_DIRECTOR_DATAPATH_METERS_METER_APPL_ID,
      &of_meter_ucm_callbacks);
  if(return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT("meter application registration failed");
    return OF_FAILURE;
  }
  OF_LIST_INIT(meter_trans_list_g,NULL);
  return OF_SUCCESS;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_meter_verifycfg (struct cm_array_of_iv_pairs * key_iv_pairs,
    uint32_t command_id, struct cm_app_result ** result_pp)
{
  struct cm_app_result *meter_result = NULL;
  int32_t return_value = OF_FAILURE;

  CM_CBK_DEBUG_PRINT ("Entered");

  return_value = of_meter_ucm_validatemandparams (key_iv_pairs, &meter_result);
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
void* of_meter_starttrans(struct cm_array_of_iv_pairs   * key_iv_pairs,
    uint32_t command_id,
    struct cm_app_result ** result_pp)
{
  struct meter_trans *trans_rec=NULL;
  struct ofi_meter_rec_info *meter_info_p;
  struct dprm_datapath_general_info datapath_info={};
  struct cm_app_result *meter_result = NULL;
  uint64_t datapath_handle;
  int32_t return_value;
  uchar8_t offset;

  CM_CBK_DEBUG_PRINT ("Entered");
  
  offset = OF_METER_TRANS_LISTNODE_OFFSET;

  meter_info_p=(struct ofi_meter_rec_info * ) calloc(1,sizeof(struct ofi_meter_rec_info ));
  if ( meter_info_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("memory allocation failed");
    return NULL;
  }

  if ((of_meter_ucm_setmandparams (key_iv_pairs,&datapath_info, meter_info_p, &meter_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return NULL;
  }

  return_value=dprm_get_datapath_handle(datapath_info.dpid, &datapath_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: datapath does not exist with id %llx",datapath_info.dpid);
    fill_app_result_struct (&meter_result, NULL, CM_GLU_METER_DATAPATH_DOESNOT_EXIST);
    *result_pp = meter_result;
    return NULL;
  }

  OF_METER_GENERATE_TRANS_ID(metertransId_g);
  trans_rec=(struct meter_trans *) calloc(1,sizeof(struct meter_trans));  
  trans_rec->datapath_handle=datapath_handle;
  trans_rec->trans_id=metertransId_g;
  trans_rec->command_id=command_id;
  trans_rec->meter_id=meter_info_p->meter_id;
  switch (command_id)
  {
    case CM_CMD_ADD_PARAMS:
      trans_rec->sub_command_id=ADD_METER;
#if 1
      return_value =of_register_meter(datapath_handle, meter_info_p);
      if (return_value != OF_SUCCESS)
      {
        CM_CBK_DEBUG_PRINT ("Meter Addition Failed");
        fill_app_result_struct (&meter_result, NULL, CM_GLU_METER_ADD_FAILED);
        *result_pp =meter_result;
        return NULL;
      }
#endif
      break;

    case CM_CMD_SET_PARAMS:
      trans_rec->sub_command_id=MOD_METER;
#if 1
      return_value=of_meter_unregister_bands(datapath_handle, meter_info_p->meter_id);
      if (return_value != OF_SUCCESS)
      {
        CM_CBK_DEBUG_PRINT ("Meter Modification Failed");
        fill_app_result_struct (&meter_result, NULL, CM_GLU_METER_EDIT_FAILED);
        *result_pp =meter_result;
        return NULL;
      }
#endif
      break;

    case CM_CMD_DEL_PARAMS:
      trans_rec->sub_command_id=DEL_METER;
      break;
  }
  OF_APPEND_NODE_TO_LIST(meter_trans_list_g,trans_rec,offset);
  CM_CBK_DEBUG_PRINT ("meter node appended to list");
  return (void *)trans_rec;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t of_meter_addrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * mand_iv_pairs,
    struct cm_array_of_iv_pairs * opt_iv_pairs,
    struct cm_app_result ** result_pp)
{
  struct cm_app_result *meter_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct ofi_meter_rec_info meter_info = { };
  struct dprm_datapath_general_info datapath_info={};
  uint64_t datapath_handle;

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&meter_info, 0, sizeof (struct ofi_meter_rec_info));

  if ((of_meter_ucm_setmandparams (mand_iv_pairs,&datapath_info, &meter_info, &meter_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    *result_pp=meter_result;
    return OF_FAILURE;
  }

  return_value =of_meter_ucm_setoptparams (opt_iv_pairs, &meter_info, &meter_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
    *result_pp =meter_result;
    return OF_FAILURE;
  }

  return_value=dprm_get_datapath_handle(datapath_info.dpid, &datapath_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: datapath does not exist with id %llx",datapath_info.dpid);
    fill_app_result_struct (&meter_result, NULL, CM_GLU_METER_DATAPATH_DOESNOT_EXIST);
    *result_pp = meter_result;
    return OF_FAILURE;
  }

  return_value =of_update_meter(datapath_handle, &meter_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("meter Addition Failed");
    fill_app_result_struct (&meter_result, NULL, CM_GLU_METER_ADD_FAILED);
    *result_pp =meter_result;
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
int32_t of_meter_setrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * mand_iv_pairs,
    struct cm_array_of_iv_pairs * opt_iv_pairs,
    struct cm_app_result ** result_pp)
{
  struct cm_app_result *meter_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct ofi_meter_rec_info meter_info = { };
  struct dprm_datapath_general_info datapath_info={};
  uint64_t datapath_handle;

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&meter_info, 0, sizeof (struct ofi_meter_rec_info));

  if ((of_meter_ucm_setmandparams (mand_iv_pairs,&datapath_info, &meter_info, &meter_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    *result_pp=meter_result;
    return OF_FAILURE;
  }

  return_value=dprm_get_datapath_handle(datapath_info.dpid, &datapath_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: datapath does not exist with id %llx",datapath_info.dpid);
    fill_app_result_struct (&meter_result, NULL, CM_GLU_METER_DATAPATH_DOESNOT_EXIST);
    *result_pp = meter_result;
    return OF_FAILURE;
  }

  return_value=dprm_get_datapath_handle(datapath_info.dpid, &datapath_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: datapath does not exist with id %llx",datapath_info.dpid);
    fill_app_result_struct (&meter_result, NULL, CM_GLU_METER_DATAPATH_DOESNOT_EXIST);
    *result_pp = meter_result;
    return OF_FAILURE;
  }
#if 0
  return_value=of_meter_unregister_buckets(datapath_handle, meter_info.meter_id);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: meter does not exist with name %d",meter_info.meter_id);
    fill_app_result_struct (&meter_result, NULL, CM_GLU_METER_DOESNOT_EXIST);
    *result_pp =meter_result;
    return OF_FAILURE;
  }
#endif

  return_value =of_update_meter(datapath_handle, &meter_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("meter Addition Failed");
    fill_app_result_struct (&meter_result, NULL, CM_GLU_METER_ADD_FAILED);
    *result_pp =meter_result;
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
int32_t of_meter_delrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * key_iv_pairs,
    struct cm_app_result ** result_pp)
{
  struct cm_app_result *meter_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct ofi_meter_rec_info meter_info = { };
  struct dprm_datapath_general_info datapath_info={};
  uint64_t datapath_handle;

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&meter_info, 0, sizeof (struct ofi_meter_rec_info));

  if ((of_meter_ucm_setmandparams (key_iv_pairs,&datapath_info, &meter_info, &meter_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&meter_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_pp =meter_result;
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
int32_t  of_meter_endtrans(void *config_trans_p,
    uint32_t command_id,
    struct cm_app_result ** result_pp)
{
  struct meter_trans *trans_rec=config_trans_p;
  struct meter_trans *trans_rec_entry_p=NULL, *prev_trans_rec_p=NULL;
  struct ofi_meter_rec_info meter_info={};
  struct cm_app_result *meter_result = NULL;
  int32_t return_value = OF_FAILURE;
  uchar8_t offset;

  CM_CBK_DEBUG_PRINT ("Entered");

  offset = OF_METER_TRANS_LISTNODE_OFFSET;

  CM_CBK_DEBUG_PRINT ("trans id %d command id %d sub cmd id %d meter id %d",trans_rec->trans_id,trans_rec->command_id,
      trans_rec->sub_command_id,trans_rec->meter_id);

  switch(command_id)
  {

    case CM_CMD_CONFIG_SESSION_REVOKE:
      CM_CBK_DEBUG_PRINT ("Revoke command");

      if (trans_rec->sub_command_id==ADD_METER)
      {
        return_value=of_unregister_meter(trans_rec->datapath_handle, trans_rec->meter_id);
        if (return_value != OF_SUCCESS)
        {
          CM_CBK_DEBUG_PRINT ("Error: meter does not exist with id %d",trans_rec->meter_id);
          fill_app_result_struct (&meter_result, NULL, CM_GLU_METER_DOESNOT_EXIST);
          *result_pp =meter_result;
          return OF_FAILURE;
        }
      }
      OF_LIST_SCAN(meter_trans_list_g, trans_rec_entry_p, struct meter_trans *,offset)
      {
        if (trans_rec_entry_p->trans_id == trans_rec->trans_id )
        {
          CM_CBK_DEBUG_PRINT ("trans rec removed from list and freed");
          OF_REMOVE_NODE_FROM_LIST(  meter_trans_list_g, trans_rec_entry_p,  prev_trans_rec_p, offset);
          free(trans_rec_entry_p);
          break;
        }
        prev_trans_rec_p=trans_rec_entry_p;
      }

      break;

    case CM_CMD_CONFIG_SESSION_COMMIT:
      CM_CBK_DEBUG_PRINT ("Commit command");
      /* need to send meter add message to switch */
      return_value=of_meter_frame_and_send_message_request(trans_rec->datapath_handle, trans_rec->meter_id, trans_rec->sub_command_id);
      if (return_value != OF_SUCCESS)
      {
        CM_CBK_DEBUG_PRINT ("Error: of_meter_frame_and_send_message_request failed");
	if(return_value == OF_METER_RATE_REQUIRED){
        fill_app_result_struct (&meter_result, NULL, CM_GLU_METER_RATE_REQUIRED);
        *result_pp =meter_result;
        return OF_FAILURE;
	}
	else if(return_value == OF_METER_PREC_LEVEL_REQUIRED){
	if(return_value == OF_METER_PREC_LEVEL_REQUIRED){
        fill_app_result_struct (&meter_result, NULL, CM_GLU_METER_PREC_LEVEL_REQUIRED);
        *result_pp =meter_result;
        return OF_FAILURE;
	}
	}
	else {
        fill_app_result_struct (&meter_result, NULL, CM_GLU_METER_ADD_TO_SWITCH_FAILED);
        *result_pp =meter_result;
        return OF_FAILURE;
	}
      }


      if (trans_rec->sub_command_id==DEL_METER)
      {
        return_value=of_unregister_meter(trans_rec->datapath_handle, trans_rec->meter_id);
        if (return_value != OF_SUCCESS)
        {
          CM_CBK_DEBUG_PRINT ("Error: meter does not exist with id %d",trans_rec->meter_id);
          fill_app_result_struct (&meter_result, NULL, CM_GLU_METER_DOESNOT_EXIST);
          *result_pp =meter_result;
          return OF_FAILURE;
        }

      }

      OF_LIST_SCAN(meter_trans_list_g, trans_rec_entry_p, struct meter_trans *, offset) 
      {
        if (trans_rec_entry_p->trans_id == trans_rec->trans_id )
        {
        OF_REMOVE_NODE_FROM_LIST(  meter_trans_list_g, trans_rec_entry_p,  prev_trans_rec_p, offset );
          CM_CBK_DEBUG_PRINT ("trans rec removed from list and freed");
          free(trans_rec_entry_p);
          break;
        }
        prev_trans_rec_p=trans_rec_entry_p;
      }
      break;

    default:
      CM_CBK_DEBUG_PRINT ("unknown command");

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
int32_t of_meter_getfirstnrecs(struct cm_array_of_iv_pairs * key_iv_pairs,
    uint32_t * count_p,
    struct cm_array_of_iv_pairs ** arr_ivpair_arr_pp)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  struct cm_app_result *meter_result = NULL;
  struct ofi_meter_rec_info meter_info={};
  struct dprm_datapath_general_info datapath_info={};
  uint64_t datapath_handle;

  CM_CBK_DEBUG_PRINT ("Entered");

  of_memset (&meter_info, 0, sizeof (struct ofi_meter_rec_info));
  if ((of_meter_ucm_setmandparams (key_iv_pairs,&datapath_info, &meter_info, &meter_result)) !=
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

  return_value=cntlr_send_meter_config_request(datapath_handle);
  if ( return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Meter Record get failed");
    return OF_FAILURE;
  } 
  CM_CBK_DEBUG_PRINT ("Meter Record will be sent as a multipart response");
  return CM_CNTRL_MULTIPART_RESPONSE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_meter_getnextnrecs (struct cm_array_of_iv_pairs * key_iv_pairs,
    struct cm_array_of_iv_pairs *prev_rec_iv_pairs, uint32_t * count_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp)
{
  CM_CBK_DEBUG_PRINT ("no next records");
  *count_p=0;
  return OF_FAILURE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_meter_getexactrec (struct cm_array_of_iv_pairs * key_iv_pairs,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct cm_app_result *meter_result = NULL;
  uint32_t ii = 0;
  int32_t return_value = OF_FAILURE;
  struct ofi_meter_rec_info *meter_entry_p;
  struct ofi_meter_rec_info meter_info={};
  struct dprm_datapath_general_info datapath_info={};
  uint64_t datapath_handle;

  CM_CBK_DEBUG_PRINT ("Entered");

  of_memset (&meter_info, 0, sizeof (struct ofi_meter_rec_info));
  if ((of_meter_ucm_setmandparams (key_iv_pairs,&datapath_info, &meter_info, &meter_result)) !=
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

  return_value=of_get_meter(datapath_handle, meter_info.meter_id, &meter_entry_p);
  if ( return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("of_get_meter failed");
    return OF_FAILURE;
  }

  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    return OF_FAILURE;
  }

  return_value=of_meter_ucm_getparams (meter_entry_p, &result_iv_pairs_p[0]);
  if ( return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("of_meter_ucm_getparams failed");
    return OF_FAILURE;
  } 

  *result_iv_pairs_pp = result_iv_pairs_p;

  return OF_SUCCESS;

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_meter_ucm_setmandparams (struct cm_array_of_iv_pairs *
    mand_iv_pairs,
    struct dprm_datapath_general_info *datapath_info,
    struct ofi_meter_rec_info *meter_info,
    struct cm_app_result ** app_result_pp)
{
  uint32_t mand_param_cnt;
  uint32_t meter_id;
  uint64_t idpId;

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

      case CM_DM_METERREC_METERID_ID:
        meter_id=of_atoi((char *) mand_iv_pairs->iv_pairs[mand_param_cnt].value_p);
        meter_info->meter_id=meter_id;
        CM_CBK_DEBUG_PRINT ("meter id is %d", meter_id);
        break;

      case CM_DM_METERREC_METERTYPE_ID:
        if (!strncmp (mand_iv_pairs->iv_pairs[mand_param_cnt].value_p,"OFPMF_KBPS", 
              mand_iv_pairs->iv_pairs[mand_param_cnt].value_length))
        {
          meter_info->flags = OFPMF_KBPS;
        }
        else if (!strncmp (mand_iv_pairs->iv_pairs[mand_param_cnt].value_p, "OFPMF_PKTPS",
              mand_iv_pairs->iv_pairs[mand_param_cnt].value_length))
        {
          meter_info->flags = OFPMF_PKTPS;
        }
        else if (!strncmp (mand_iv_pairs->iv_pairs[mand_param_cnt].value_p, "OFPMF_BURST",
              mand_iv_pairs->iv_pairs[mand_param_cnt].value_length))
        {
          meter_info->flags = OFPMF_BURST;
        }
        else if (!strncmp (mand_iv_pairs->iv_pairs[mand_param_cnt].value_p, "OFPMF_STATS",
              mand_iv_pairs->iv_pairs[mand_param_cnt].value_length))
        {
          meter_info->flags = OFPMF_STATS;
        }
        else
          CM_CBK_DEBUG_PRINT ("Invalid Type entered");

        CM_CBK_DEBUG_PRINT ("type is %d", meter_info->flags);
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
int32_t of_meter_ucm_validatemandparams (struct cm_array_of_iv_pairs *
    mand_iv_pairs,
    struct cm_app_result ** app_result_pp)
{
  uint32_t mand_param_cnt;
  uint32_t meter_id;
  uint16_t type;
  struct cm_app_result *meter_result;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (mand_param_cnt = 0; mand_param_cnt < mand_iv_pairs->count_ui;
      mand_param_cnt++)
  {
    switch (mand_iv_pairs->iv_pairs[mand_param_cnt].id_ui)
    {
      case CM_DM_METERREC_METERID_ID:
        meter_id=of_atoi((char *) mand_iv_pairs->iv_pairs[mand_param_cnt].value_p);
        CM_CBK_DEBUG_PRINT ("meter id is %d", meter_id);
        break;
      case CM_DM_METERREC_METERTYPE_ID:
        type = (uint16_t)of_atoi((char *) mand_iv_pairs->iv_pairs[mand_param_cnt].value_p);
        CM_CBK_DEBUG_PRINT ("type is %d", type);
        if ( type > OFPMF_STATS)
        {
          CM_CBK_DEBUG_PRINT ("meter type is not valid");
          fill_app_result_struct (&meter_result, NULL, CM_GLU_METER_TYPE_INVALID);
          *app_result_pp = meter_result;
          return OF_FAILURE;
        }
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
int32_t of_meter_ucm_setoptparams (struct cm_array_of_iv_pairs *
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
int32_t of_meter_ucm_getparams (struct ofi_meter_rec_info *meter_entry_p,
    struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
  char string[1024];
  char tmp_string[1024];
  uint16_t len; 
  uint32_t rate; 
  uint32_t burst_size; 
  uint8_t prec_level; 
  int32_t index = 0, jj, count_ui, iIndex;

  struct ofi_meter_band *band_entry_p;
  int32_t i;
  
  char *meter_types[]={
    "OFPMF_KBPS",
    "OFPMF_PKTPS",
    "OFPMF_BURST",
    "OFPMF_STATS"
  };
  
  char *band_types[]={
    "OFPMBT_DROP",
    "OFPMBT_DSCP_REMARK",
    "OFPMBT_EXPERIMENTER"
  };


#define CM_METERREC_CHILD_COUNT 20
  count_ui = CM_METERREC_CHILD_COUNT;

  result_iv_pairs_p->iv_pairs = (struct cm_iv_pair*)of_calloc(count_ui, sizeof(struct cm_iv_pair));
  if(!result_iv_pairs_p->iv_pairs)
  {
    CM_CBK_DEBUG_PRINT (" Memory allocation failed ");
    return OF_FAILURE;
  }

  of_memset(string, 0, sizeof(string));
  of_itoa (meter_entry_p->meter_id, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_METERREC_METERID_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_sprintf(string,"%s",meter_types[meter_entry_p->flags]);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_METERREC_METERTYPE_ID,CM_DATA_TYPE_STRING, string);
  index++;

  OF_LIST_SCAN(meter_entry_p->meter_band_list, band_entry_p, struct ofi_meter_band *, OF_METER_BAND_LISTNODE_OFFSET)
  {
    of_memset(string, 0, sizeof(string));
    of_sprintf(string,"%s",meter_types[band_entry_p->type]);
    FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_METERREC_BANDTYPE_ID,CM_DATA_TYPE_STRING, string);
    index++;

    of_memset(string, 0, sizeof(string));
    switch(band_entry_p->type)
    {
      case  OFPMBT_DROP:
        len = band_entry_p->len;
        rate = band_entry_p->rate;
        burst_size = band_entry_p->burst_size;
        of_sprintf(string,"Length=%d  Rate=%d burst_size=%d",len,rate,burst_size);
        break;

      case  OFPMBT_DSCP_REMARK:
        len = band_entry_p->len;
        rate = band_entry_p->rate;
        burst_size = band_entry_p->burst_size;
        prec_level = band_entry_p->prec_level;
        of_sprintf(string,"Length=%d  Rate=%d burst_size=%d prec_level=%c",len,rate,burst_size,prec_level);
        break;
      case OFPMBT_EXPERIMENTER:
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"not supported");
        of_strcpy(string,"-");
        break;
    }
    FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_METERREC_BANDINFO_ID,CM_DATA_TYPE_STRING, string);
    index++;

  }
  result_iv_pairs_p->count_ui = index;
  CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);
  return OF_SUCCESS;
}
#endif /*OF_CM_SUPPORT */
