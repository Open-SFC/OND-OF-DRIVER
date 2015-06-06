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
 * File name: mempoolstats.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: Port Statistics.
 * 
 */

#ifdef OF_CM_SUPPORT


#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "of_utilities.h"
#include "of_multipart.h"

int32_t of_mempoolstats_appl_ucmcbk_init (void);


int32_t of_mempoolstats_getfirstnrecs (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    uint32_t * count_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp);

int32_t of_mempoolstats_getnextnrecs (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs * pPrevRecordKey, uint32_t * count_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp);


int32_t of_mempoolstats_getexactrec (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp);
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */



int32_t of_mempoolstats_ucm_set_mand_params (struct cm_array_of_iv_pairs *
    mand_iv_pairs_p,
    struct dprm_datapath_general_info *datapath_info,
    struct cm_app_result ** result_pp);

int32_t of_mempoolstats_ucm_set_opt_params (struct cm_array_of_iv_pairs *
    opt_iv_pairs_p,
    struct ofi_port_stats_info * port_info,
    struct cm_app_result ** result_pp);


int32_t of_mempoolstats_ucm_getparams (struct mempool_stats *mempool_stats_info_p,
    struct cm_array_of_iv_pairs * result_iv_pairs_p);



struct cm_dm_app_callbacks of_mempoolstats_ucm_callbacks = {
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  of_mempoolstats_getfirstnrecs,
  of_mempoolstats_getnextnrecs,
  of_mempoolstats_getexactrec,
  NULL,
  NULL
};
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * Switch  UCM Init * * * * * * *  * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t of_mempoolstats_appl_ucmcbk_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  cm_register_app_callbacks (CM_ON_DIRECTOR_MEMPOOLSTATS_APPL_ID,&of_mempoolstats_ucm_callbacks);
  return OF_SUCCESS;
}



int32_t of_mempoolstats_getfirstnrecs (struct cm_array_of_iv_pairs * key_iv_pairs_p,
				    uint32_t * count_p,
				    struct cm_array_of_iv_pairs **array_of_iv_pair_arr_p)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount=0;
  struct cm_app_result *result = NULL;
  uint64_t datapath_handle;
  struct mempool_stats  mempool_stats={};
  struct dprm_datapath_general_info datapath_info={};


  CM_CBK_DEBUG_PRINT ("Entered");
#if 0
  if ((of_mempoolstats_ucm_set_mand_params (key_iv_pairs_p,&datapath_info, &result)) !=
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
#endif
  return_value=mempool_get_first_stats(&mempool_stats); 
  if ( return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("mempool stats failed");
    return OF_FAILURE;
  } 
  if(return_value == OF_SUCCESS)
  {
	  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
	  if (result_iv_pairs_p == NULL)
	  {
		  CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
		  return OF_FAILURE;
	  }
	  of_mempoolstats_ucm_getparams(&mempool_stats, &result_iv_pairs_p[uiRecCount]);
	  uiRecCount++;
	  *array_of_iv_pair_arr_p = result_iv_pairs_p;
	  *count_p = uiRecCount;
  }
  return OF_SUCCESS;

}


int32_t of_mempoolstats_getnextnrecs (struct cm_array_of_iv_pairs * key_iv_pairs_p,
		struct cm_array_of_iv_pairs *pPrevRecordKey, uint32_t * count_p,
		struct cm_array_of_iv_pairs ** next_n_record_mempool_data_p)
{
	struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	int32_t return_value = OF_FAILURE;
  	uint32_t uiRecCount=0;
	struct cm_app_result *result = NULL;
	uint64_t datapath_handle;
	struct mempool_stats  mempool_stats={};
	struct dprm_datapath_general_info datapath_info={};

  CM_CBK_DEBUG_PRINT ("Entered");

#if 0

	if ((of_mempoolstats_ucm_set_mand_params (key_iv_pairs_p,&datapath_info, &result)) !=
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
#endif

	return_value=mempool_get_next_stats(pPrevRecordKey->iv_pairs[0].value_p, &mempool_stats); 
	if ( return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("mempool next stats failed");
		return OF_FAILURE;
	} 
	if(return_value == OF_SUCCESS)
	{
		result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
		if (result_iv_pairs_p == NULL)
		{
			CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
			return OF_FAILURE;
		}
		of_mempoolstats_ucm_getparams(&mempool_stats, &result_iv_pairs_p[uiRecCount]);
		uiRecCount++;
		*next_n_record_mempool_data_p = result_iv_pairs_p;
		*count_p = uiRecCount;
	}
	return OF_SUCCESS;


}

int32_t of_mempoolstats_getexactrec (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp)
{
  int32_t return_value = OF_FAILURE;

  CM_CBK_DEBUG_PRINT ("Entered");
  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */




int32_t of_mempoolstats_ucm_set_mand_params (struct cm_array_of_iv_pairs *mand_iv_pairs_p,
    struct dprm_datapath_general_info *datapath_info,
    struct cm_app_result ** result_pp)
{
  uint32_t param_count;
  uint64_t idpId;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (param_count = 0; param_count < mand_iv_pairs_p->count_ui;
      param_count++)
  {
    switch (mand_iv_pairs_p->iv_pairs[param_count].id_ui)
    {
      case CM_DM_DATAPATH_DATAPATHID_ID:
        idpId=charTo64bitNum((char *) mand_iv_pairs_p->iv_pairs[param_count].value_p);
        CM_CBK_DEBUG_PRINT("dpid is: %llx\r\n", idpId);
        datapath_info->dpid=idpId;
        break;
    }
  }
  //CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs_p);
  return OF_SUCCESS;
}



int32_t of_mempoolstats_ucm_getparams (struct mempool_stats *mempool_stats_info_p,
    struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
  char string[64];
  int32_t index = 0,  count_ui ;

  CM_CBK_DEBUG_PRINT ("Entered");
#define CM_MEMPOOLSTATS_CHILD_COUNT 18
  count_ui = CM_MEMPOOLSTATS_CHILD_COUNT;

  if(mempool_stats_info_p == NULL)
  {
    return OF_FAILURE;
  }

  result_iv_pairs_p->iv_pairs = (struct cm_iv_pair*)of_calloc(count_ui, sizeof(struct cm_iv_pair));
  if(!result_iv_pairs_p->iv_pairs)
  {
    CM_CBK_DEBUG_PRINT (" Memory allocation failed ");
    return OF_FAILURE;
  }

  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_MEMPOOLSTATS_MEMPOOLNAME_ID, CM_DATA_TYPE_STRING, mempool_stats_info_p->mempool_name);
  index++;


  of_memset(string, 0, sizeof(string));
  of_itoa (mempool_stats_info_p->free_blocks_count, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_MEMPOOLSTATS_FREEBLOCKSCOUNT_ID, CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_itoa (mempool_stats_info_p->allocated_blocks_count, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_MEMPOOLSTATS_ALLOCATEDBLOCKSCOUNT_ID,CM_DATA_TYPE_STRING, string);
  index++;


  of_memset(string, 0, sizeof(string));
  of_itoa (mempool_stats_info_p->released_blocks_count, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_MEMPOOLSTATS_RELEASEDBLOCKSCOUNT_ID,CM_DATA_TYPE_STRING, string);
  index++;


  of_memset(string, 0, sizeof(string));
  of_itoa (mempool_stats_info_p->allocation_fail_blocks_count, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_MEMPOOLSTATS_ALLOCATIONFAILBLOCKS_COUNT_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_itoa (mempool_stats_info_p->released_fail_blocks_count, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_MEMPOOLSTATS_RELEASEDFAILBLOCKSCOUNT_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_itoa (mempool_stats_info_p->heap_free_blocks_count, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_MEMPOOLSTATS_HEAPFREEBLOCKSCOUNT_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_itoa (mempool_stats_info_p->heap_allocated_blocks_count, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_MEMPOOLSTATS_HEAPALLOCATEDBLOCKSCOUNT_ID,CM_DATA_TYPE_STRING, string);
  index++;


  of_memset(string, 0, sizeof(string));
  of_itoa (mempool_stats_info_p->heap_released_blocks_count, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_MEMPOOLSTATS_HEAPRELEASEDBLOCKSCOUNT_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_itoa (mempool_stats_info_p->heap_allocation_fail_blocks_count, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_MEMPOOLSTATS_HEAPALLOCATIONFAILBLOCKSCOUNT_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_itoa (mempool_stats_info_p->heap_allowed_blocks_count, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_MEMPOOLSTATS_HEAPALLOWEDBLOCKSCOUNT_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_itoa (mempool_stats_info_p->total_blocks_count, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_MEMPOOLSTATS_TOTALBLOCKSCOUNT_ID,CM_DATA_TYPE_STRING, string);
  index++;


  of_memset(string, 0, sizeof(string));
  of_itoa (mempool_stats_info_p->block_size, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_MEMPOOLSTATS_BLOCKSIZE_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_itoa (mempool_stats_info_p->threshold_min, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_MEMPOOLSTATS_THRESHOLDMIN_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_itoa (mempool_stats_info_p->threshold_max, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_MEMPOOLSTATS_THRESHOLD_MAX_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_itoa (mempool_stats_info_p->replenish_count, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_MEMPOOLSTATS_REPLENISHCOUNT_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_itoa (mempool_stats_info_p->static_allocated_blocks_count, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_MEMPOOLSTATS_STATICALLOCATEDBLOCKSCOUNT_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_itoa (mempool_stats_info_p->static_blocks, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_MEMPOOLSTATS_STATICBLOCKS_ID, CM_DATA_TYPE_STRING, string);
  index++;


  result_iv_pairs_p->count_ui = index;

  //CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);
  return OF_SUCCESS;
}

#endif /* OF_CM_SUPPORT */
