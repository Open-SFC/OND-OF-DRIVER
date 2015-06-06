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
 * File name: aggregatestats.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: Aggregate Statistics.
 * 
 */

#ifdef OF_CM_SUPPORT


#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "of_utilities.h"
#include "of_multipart.h"

int32_t of_aggrstats_appl_ucmcbk_init (void);

int32_t of_aggrstats_addrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * mand_iv_pairs_p,
    struct cm_array_of_iv_pairs * opt_iv_pairs_p,
    struct cm_app_result ** result_p);

int32_t of_aggrstats_setrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * mand_iv_pairs_p,
    struct cm_array_of_iv_pairs * opt_iv_pairs_p,
    struct cm_app_result ** result_p);

int32_t of_aggrstats_delrec (void * pConfigTransction,
    struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_app_result ** result_p);

int32_t of_aggrstats_getfirstnrecs (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    uint32_t * count_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp);

int32_t of_aggrstats_getnextnrecs (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs * pPrevRecordKey, uint32_t * count_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp);

int32_t of_aggrstats_getexactrec (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp);

int32_t of_aggregatestats_verifycfg (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    uint32_t command_id, struct cm_app_result ** result_p);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */
int32_t of_aggrstats_ucm_validate_mand_params (struct cm_array_of_iv_pairs *
    mand_iv_pairs_p,
    struct cm_app_result **presult_p);

int32_t of_aggrstats_ucm_validate_opt_params (struct cm_array_of_iv_pairs *
    opt_iv_pairs_p,
    struct cm_app_result ** presult_p);


int32_t of_aggrstats_ucm_set_mand_params (struct cm_array_of_iv_pairs *
    mand_iv_pairs_p,
    struct dprm_datapath_general_info *datapath_info,
    struct cm_app_result ** presult_p);

int32_t of_aggrstats_ucm_set_opt_params (struct cm_array_of_iv_pairs *
    opt_iv_pairs_p,
    struct ofi_aggregate_flow_stats * stats_info,
    struct cm_app_result ** presult_p);


int32_t of_aggrstats_ucm_getparams (struct ofi_aggregate_flow_stats *stats_info,
    struct cm_array_of_iv_pairs * result_iv_pairs_p);



struct cm_dm_app_callbacks of_aggrstats_ucm_callbacks = {
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  of_aggrstats_getfirstnrecs,
  of_aggrstats_getnextnrecs,
  of_aggrstats_getexactrec,
  NULL,
  NULL
};
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * Switch  UCM Init * * * * * * *  * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t of_aggrstats_appl_ucmcbk_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  cm_register_app_callbacks (CM_ON_DIRECTOR_DATAPATH_STATS_AGGREGATESTATS_APPL_ID,&of_aggrstats_ucm_callbacks);
  return OF_SUCCESS;
}



int32_t of_aggrstats_getfirstnrecs (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    uint32_t * count_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t no_of_records = 0;
  struct dprm_datapath_general_info datapath_info={};
  uint64_t datapath_handle;
  struct cm_app_result *stats_result_p;


  if ((of_aggrstats_ucm_set_mand_params (key_iv_pairs_p, &datapath_info,  &stats_result_p)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  return_value=dprm_get_datapath_handle(datapath_info.dpid, &datapath_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Get DPRM Handle failed");
    return OF_FAILURE;
  }

  return_value=cntlr_send_aggregate_stats_request(datapath_handle); 
  if ( return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("aggregate failed");
    return OF_FAILURE;
  } 

  CM_CBK_DEBUG_PRINT ("aggregate stats will be sent as multipart response");
  *count_p = no_of_records;
  return CM_CNTRL_MULTIPART_RESPONSE;

}


int32_t of_aggrstats_getnextnrecs (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs *pPrevRecordKey, uint32_t * count_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp)
{

  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct cm_app_result *stats_result_p = NULL;
  int32_t return_value = OF_FAILURE;

  CM_CBK_DEBUG_PRINT ("Entered");
  *count_p=0;
  return return_value;

}

int32_t of_aggrstats_getexactrec (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;

  CM_CBK_DEBUG_PRINT ("Entered");
  return return_value;
}

int32_t of_aggregatestats_verifycfg (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    uint32_t command_id, struct cm_app_result ** result_p)
{
  struct cm_app_result *stats_result_p = NULL;
  int32_t return_value = OF_FAILURE;
  struct ofi_aggregate_flow_stats stats_info = { };

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&stats_info, 0, sizeof (struct ofi_aggregate_flow_stats));

  return_value = of_aggrstats_ucm_validate_mand_params (key_iv_pairs_p, &stats_result_p);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  if (of_aggrstats_ucm_validate_opt_params (key_iv_pairs_p, &stats_result_p)
      != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  *result_p = stats_result_p;
  return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */

int32_t of_aggrstats_ucm_validate_mand_params (struct cm_array_of_iv_pairs *
    mand_iv_pairs_p,  struct cm_app_result **presult_p)
{
  uint32_t param_count;
  struct cm_app_result *stats_result_p = NULL;
  uint32_t uiPortId;


  CM_CBK_DEBUG_PRINT ("Entered");
  for (param_count = 0; param_count < mand_iv_pairs_p->count_ui;
      param_count++)
  {
    switch (mand_iv_pairs_p->iv_pairs[param_count].id_ui)
    {
      default:
        break;

    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs_p);
  return OF_SUCCESS;
}

int32_t of_aggrstats_ucm_validate_opt_params (struct cm_array_of_iv_pairs *
    opt_iv_pairs_p,
    struct cm_app_result ** presult_p)
{
  uint32_t param_count, uiTableCnt;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (param_count = 0; param_count < opt_iv_pairs_p->count_ui; param_count++)
  {
    switch (opt_iv_pairs_p->iv_pairs[param_count].id_ui)
    {
      default:
        break;
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (opt_iv_pairs_p);
  return OF_SUCCESS;
}

int32_t of_aggrstats_ucm_set_mand_params (struct cm_array_of_iv_pairs *
    mand_iv_pairs_p,
    struct dprm_datapath_general_info *datapath_info,
       struct cm_app_result ** presult_p)
{
  uint32_t param_count;
  uint64_t idpId ;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (param_count = 0; param_count < mand_iv_pairs_p->count_ui;
      param_count++)
  {
    switch (mand_iv_pairs_p->iv_pairs[param_count].id_ui)
    {
      case CM_DM_DATAPATH_DATAPATHID_ID:
        idpId=charTo64bitNum((char *) mand_iv_pairs_p->iv_pairs[param_count].value_p);
        datapath_info->dpid=idpId;
        CM_CBK_DEBUG_PRINT("dpid is: %llx\r\n", idpId);
        break;

    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs_p);
  return OF_SUCCESS;
}

int32_t of_aggrstats_ucm_set_opt_params (struct cm_array_of_iv_pairs *
    opt_iv_pairs_p,
    struct ofi_aggregate_flow_stats * stats_info,
    struct cm_app_result ** presult_p)
{
  uint32_t param_count;

  CM_CBK_DEBUG_PRINT ("Entered");

  for (param_count = 0; param_count < opt_iv_pairs_p->count_ui; param_count++)
  {
    switch (opt_iv_pairs_p->iv_pairs[param_count].id_ui)
    {
      default:
        break;
    }
  }

  CM_CBK_PRINT_IVPAIR_ARRAY (opt_iv_pairs_p);
  return OF_SUCCESS;
}



int32_t of_aggrstats_ucm_getparams (struct ofi_aggregate_flow_stats *stats_info_p,
    struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
  char string[64];
  int32_t index = 0, count_ui;

#define CM_AGGREGATESTATS_CHILD_COUNT 3
  count_ui = CM_AGGREGATESTATS_CHILD_COUNT;

  result_iv_pairs_p->iv_pairs = (struct cm_iv_pair*)of_calloc(count_ui, sizeof(struct cm_iv_pair));
  if(!result_iv_pairs_p->iv_pairs)
  {
    CM_CBK_DEBUG_PRINT (" Memory allocation failed ");
    return OF_FAILURE;
  }

  of_memset(string, 0, sizeof(string));
  of_ltoa (stats_info_p->packet_count, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_AGGREGATESTATS_PACKETCOUNT_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_ltoa (stats_info_p->byte_count, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_AGGREGATESTATS_BYTECOUNT_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_itoa (stats_info_p->flow_count, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_AGGREGATESTATS_FLOWCOUNT_ID,CM_DATA_TYPE_STRING, string);
  index++;

  result_iv_pairs_p->count_ui = index;
  return OF_SUCCESS;
}

#endif /* OF_CM_SUPPORT */
