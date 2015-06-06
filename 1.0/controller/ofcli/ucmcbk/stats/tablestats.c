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
 * File name: tablestats.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: Table Statistics.
 * 
 */

#ifdef OF_CM_SUPPORT

#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "of_utilities.h"
#include "of_multipart.h"

int32_t of_tablestats_appl_ucmcbk_init (void);

int32_t of_tablestats_addrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * mand_iv_pairs_p,
    struct cm_array_of_iv_pairs * opt_iv_pairs_p,
    struct cm_app_result ** pResult);

int32_t of_tablestats_setrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * mand_iv_pairs_p,
    struct cm_array_of_iv_pairs * opt_iv_pairs_p,
    struct cm_app_result ** pResult);

int32_t of_tablestats_delrec (void * pConfigTransction,
    struct cm_array_of_iv_pairs * pKeysArr,
    struct cm_app_result ** pResult);

int32_t of_tablestats_getfirstnrecs (struct cm_array_of_iv_pairs * pKeysArr,
    uint32_t * count_p,
    struct cm_array_of_iv_pairs ** pArrayofIvPairArr);

int32_t of_tablestats_getnextnrecs (struct cm_array_of_iv_pairs * pKeysArr,
    struct cm_array_of_iv_pairs * pPrevRecordKey, uint32_t * count_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp);

int32_t of_tablestats_getexactrec (struct cm_array_of_iv_pairs * pKeysArr,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp);

int32_t of_tablestats_verifycfg (struct cm_array_of_iv_pairs * pKeysArr,
    uint32_t command_id, struct cm_app_result ** pResult);

int32_t of_tablestats_ucm_getparams (struct ofi_table_stats_info *table_info,
    struct cm_array_of_iv_pairs * result_iv_pairs_p);

struct cm_dm_app_callbacks of_tablestats_ucm_callbacks = 
{
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  of_tablestats_getfirstnrecs,
  of_tablestats_getnextnrecs,
  of_tablestats_getexactrec,
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
int32_t of_tablestats_appl_ucmcbk_init (void)
{
  int32_t return_value;

  return_value= cm_register_app_callbacks (CM_ON_DIRECTOR_DATAPATH_STATS_TABLESTATS_APPL_ID,
      &of_tablestats_ucm_callbacks);
  if(return_value != OF_SUCCESS)
  {
    cntrlrucm_debugmsg("Table stats Objects CBK registration failed");
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
int32_t of_tablestats_getfirstnrecs(struct cm_array_of_iv_pairs * pKeysArr,
    uint32_t * count_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp)
{
  int32_t return_value = OF_FAILURE;
  struct cm_app_result *table_stats_result = NULL;
  struct dprm_datapath_general_info datapath_info={};
  uint64_t datapath_handle;


  if ((of_tablestats_ucm_setmandparams (pKeysArr,&datapath_info, &table_stats_result)) !=
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

  return_value=cntlr_send_table_stats_request(datapath_handle);
  if ( return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Table stats failed");
    return OF_FAILURE;
  } 
  CM_CBK_DEBUG_PRINT ("Table stats will be sent as a multipart response");
  return CM_CNTRL_MULTIPART_RESPONSE;
}

int32_t of_tablestats_getnextnrecs (struct cm_array_of_iv_pairs * pKeysArr,
    struct cm_array_of_iv_pairs *pPrevRecordKey, uint32_t * count_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp)
{
  CM_CBK_DEBUG_PRINT ("no next records");
  *count_p=0;
  return OF_FAILURE;
}

int32_t of_tablestats_getexactrec (struct cm_array_of_iv_pairs * pKeysArr,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp)
{

  CM_CBK_DEBUG_PRINT ("no exact record");
  return OF_FAILURE;

}

int32_t of_tablestats_ucm_setmandparams (struct cm_array_of_iv_pairs *
    mand_iv_pairs_p,
    struct dprm_datapath_general_info *datapath_info,
    struct cm_app_result ** result_pp)
{
  uint8_t param_count;
  uint64_t idpId;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (param_count = 0; param_count < mand_iv_pairs_p->count_ui;
      param_count++)
  {
    switch (mand_iv_pairs_p->iv_pairs[param_count].id_ui)
    {
      case CM_DM_DATAPATH_DATAPATHID_ID:
        idpId=charTo64bitNum((char *) mand_iv_pairs_p->iv_pairs[param_count].value_p);
        datapath_info->dpid=idpId;
        break;
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs_p);
  return OF_SUCCESS;
}

int32_t of_tablestats_ucm_getparams (struct ofi_table_stats_info *table_info_p,
    struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
  char string[64];
  int32_t index = 0,  count_ui;

#define CM_TABLESTATS_CHILD_COUNT 4
  count_ui = CM_TABLESTATS_CHILD_COUNT;

  //CM_CBK_DEBUG_PRINT ("Entered");
  result_iv_pairs_p->iv_pairs = (struct cm_iv_pair*)of_calloc(count_ui, sizeof(struct cm_iv_pair));
  if(!result_iv_pairs_p->iv_pairs)
  {
    CM_CBK_DEBUG_PRINT (" Memory allocation failed");
    return OF_FAILURE;
  }

  of_memset(string, 0, sizeof(string));
  of_itoa (table_info_p->table_id, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_TABLESTATS_TABLEID_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_itoa (table_info_p->active_count, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_TABLESTATS_ACTIVECOUNT_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_ltoa (table_info_p->lookup_count, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_TABLESTATS_LOOKUPCOUNT_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_ltoa (table_info_p->matched_count, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_TABLESTATS_MATCHEDCOUNT_ID,CM_DATA_TYPE_STRING, string);
  index++;

  result_iv_pairs_p->count_ui = index;

  //CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);
  return OF_SUCCESS;
}
#endif /*OF_CM_SUPPORT */
