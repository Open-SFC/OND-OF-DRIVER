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
 * File name: bucketcbk.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: 
 * 
 */


#ifdef OF_CM_SUPPORT

#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "of_utilities.h"
#include "of_multipart.h"
#include "of_group.h"

int32_t of_bucket_appl_ucmcbk_init (void);


void* of_bucket_starttrans(struct cm_array_of_iv_pairs   * key_iv_pairs,
		uint32_t command_id,
		struct cm_app_result ** result_pp);

int32_t  of_bucket_endtrans(void *config_trans_p,
		uint32_t command_id,
		struct cm_app_result ** result_pp);

int32_t of_bucket_addrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * mand_iv_pairs,
		struct cm_array_of_iv_pairs * opt_iv_pairs,
		struct cm_app_result ** result_pp);

int32_t of_bucket_setrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * mand_iv_pairs,
		struct cm_array_of_iv_pairs * opt_iv_pairs,
		struct cm_app_result ** result_pp);

int32_t of_bucket_delrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * key_iv_pairs,
		struct cm_app_result ** result_pp);

int32_t of_bucket_getfirstnrecs (struct cm_array_of_iv_pairs * key_iv_pairs,
		uint32_t * pCount,
		struct cm_array_of_iv_pairs ** pArrayofIvPairArr);

int32_t of_bucket_getnextnrecs (struct cm_array_of_iv_pairs * key_iv_pairs,
		struct cm_array_of_iv_pairs * pPrevRecordKey, uint32_t * pCount,
		struct cm_array_of_iv_pairs ** pNextNRecordData_p);

int32_t of_bucket_getexactrec (struct cm_array_of_iv_pairs * key_iv_pairs,
		struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t of_bucket_verifycfg (struct cm_array_of_iv_pairs * key_iv_pairs,
		uint32_t command_id, struct cm_app_result ** result_pp);


int32_t of_bucket_verifycfg (struct cm_array_of_iv_pairs * key_iv_pairs,
		uint32_t command_id, struct cm_app_result ** result_pp);

 int32_t of_bucket_ucm_getparams (struct ofi_bucket *group_info,
		struct cm_array_of_iv_pairs * result_iv_pairs_p);

 int32_t of_bucket_ucm_setmandparams (struct cm_array_of_iv_pairs *
		mand_iv_pairs,
		struct  ofi_group_desc_info *group_info,
		struct ofi_bucket *bucket_info,
		struct cm_app_result ** app_result_pp);

 int32_t of_bucket_ucm_setoptparams (struct cm_array_of_iv_pairs *
		opt_iv_pairs,
		struct ofi_bucket * bucket_info,
		struct cm_app_result ** app_result_pp);

struct cm_dm_app_callbacks of_bucket_ucm_callbacks = 
{
	of_bucket_starttrans,
	of_bucket_addrec,
	of_bucket_setrec,
	of_bucket_delrec,
	of_bucket_endtrans,
	of_bucket_getfirstnrecs,
	of_bucket_getnextnrecs,
	of_bucket_getexactrec,
	of_bucket_verifycfg,
	NULL
};

extern of_list_t group_trans_list_g;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_bucket_appl_ucmcbk_init (void)
{
	int32_t return_value;

	return_value=cm_register_app_callbacks (CM_ON_DIRECTOR_DATAPATH_GROUPS_GROUP_BUCKET_APPL_ID,
			&of_bucket_ucm_callbacks);
	if(return_value != OF_SUCCESS)
	{
		cntrlrucm_debugmsg("bucket application CBK registration failed");
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
int32_t of_bucket_verifycfg (struct cm_array_of_iv_pairs * key_iv_pairs,
		uint32_t command_id, struct cm_app_result ** result_pp)
{
	struct cm_app_result *of_bucket_result = NULL;
	int32_t return_value = OF_FAILURE;

	CM_CBK_DEBUG_PRINT ("Entered");

	return_value = of_bucket_ucm_validatemandparams (key_iv_pairs, &of_bucket_result);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
		return OF_FAILURE;
	}
	*result_pp = of_bucket_result;
	return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void* of_bucket_starttrans(struct cm_array_of_iv_pairs   * key_iv_pairs,
		uint32_t command_id,
		struct cm_app_result ** result_pp)
{
	struct group_trans *trans_rec;
	struct ofi_group_desc_info group_info={};
	struct ofi_bucket *bucket_info=NULL;
	struct cm_app_result *of_bucket_result = NULL;
	int32_t return_value;

        uchar8_t offset;
        offset = OF_GROUP_TRANS_LISTNODE_OFFSET;

	CM_CBK_DEBUG_PRINT ("Entered");

	bucket_info=(struct ofi_bucket *) calloc (1, sizeof(struct ofi_bucket));
	if( bucket_info == NULL )
	{
		CM_CBK_DEBUG_PRINT ("memory allocation failed");
		return NULL;
	}

	if ((of_bucket_ucm_setmandparams (key_iv_pairs,&group_info, bucket_info, &of_bucket_result)) !=
			OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		return NULL;
	}

	OF_LIST_SCAN(group_trans_list_g, trans_rec, struct group_trans *,offset )
	{
		if(trans_rec->group_id == group_info.group_id)
		{
			CM_CBK_DEBUG_PRINT ("transaction found");
			break;
		}

	}

#if 1
	return_value =of_register_bucket_to_group(trans_rec->datapath_handle,bucket_info,group_info.group_id);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Bucket Addition to group %d Failed", group_info.group_id);
		fill_app_result_struct (&of_bucket_result, NULL, CM_GLU_BUCKET_ADD_FAILED);
		*result_pp =of_bucket_result;
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

int32_t of_bucket_addrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * mand_iv_pairs,
		struct cm_array_of_iv_pairs * opt_iv_pairs,
		struct cm_app_result ** result_pp)
{
	struct cm_app_result *of_bucket_result = NULL;
	int32_t return_value = OF_FAILURE;
	struct ofi_bucket bucket_info = { };
	struct ofi_group_desc_info group_info = { };
	struct group_trans *trans_rec=(struct group_trans *)config_trans_p;

	CM_CBK_DEBUG_PRINT ("Entered");
	of_memset (&bucket_info, 0, sizeof (struct ofi_bucket));

	CM_CBK_DEBUG_PRINT ("trans id %d command id %d sub cmd id %d group id %d",trans_rec->trans_id,trans_rec->command_id,
			trans_rec->sub_command_id,trans_rec->group_id);

	if ((of_bucket_ucm_setmandparams (mand_iv_pairs,&group_info, &bucket_info, &of_bucket_result)) !=
			OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		*result_pp=of_bucket_result;
		return OF_FAILURE;
	}

	return_value =of_bucket_ucm_setoptparams (opt_iv_pairs, &bucket_info, &of_bucket_result);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
		*result_pp =of_bucket_result;
		return OF_FAILURE;
	}
#if 1
	return_value =of_update_bucket_in_group(trans_rec->datapath_handle, &bucket_info,group_info.group_id);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Bucket updation at group %d failed", group_info.group_id);
		fill_app_result_struct (&of_bucket_result, NULL, CM_GLU_BUCKET_UPDATE_FAILED);
		*result_pp =of_bucket_result;
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
int32_t of_bucket_setrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * mand_iv_pairs,
		struct cm_array_of_iv_pairs * opt_iv_pairs,
		struct cm_app_result ** result_pp)
{
	struct cm_app_result *of_bucket_result = NULL;
	int32_t return_value = OF_FAILURE;
	struct ofi_bucket bucket_info = { };
	struct dprm_switch_runtime_info runtime_info;
	struct group_trans *trans_rec=config_trans_p;


	CM_CBK_DEBUG_PRINT ("Entered");
	of_memset (&bucket_info, 0, sizeof (struct ofi_bucket));
#if 0
	if ((of_bucket_ucm_setmandparams (mand_iv_pairs,&group_info, &bucket_info, &of_bucket_result)) !=
			OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		*result_pp=of_bucket_result;
		return OF_FAILURE;
	}


	return_value=of_bucket_unregister_buckets(bucket_info.group_id);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Error: group doesn't exists with name %d",bucket_info.group_id);
		fill_app_result_struct (&of_bucket_result, NULL, CM_GLU_VLAN_FAILED);
		*result_pp =of_bucket_result;
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
int32_t of_bucket_delrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * key_iv_pairs,
		struct cm_app_result ** result_pp)
{
	struct cm_app_result *of_bucket_result = NULL;
	int32_t return_value = OF_FAILURE;
	struct ofi_bucket bucket_info = { };
	struct ofi_group_desc_info group_info = { };
	struct group_trans *trans_rec=config_trans_p;

	CM_CBK_DEBUG_PRINT ("Entered");
	of_memset (&bucket_info, 0, sizeof (struct ofi_bucket));

	if ( trans_rec->command_id != CM_CMD_ADD_PARAMS)
	{
		CM_CBK_DEBUG_PRINT ("Delete not supported");
		return OF_FAILURE;
	}

	if ((of_bucket_ucm_setmandparams (key_iv_pairs,&group_info, &bucket_info, &of_bucket_result)) !=
			OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		fill_app_result_struct (&of_bucket_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
		*result_pp =of_bucket_result;
		return OF_FAILURE;
	}

	return_value=of_group_unregister_bucket(trans_rec->datapath_handle,group_info.group_id, bucket_info.bucket_id);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Bucket does not exist with id %d",bucket_info.bucket_id);

		fill_app_result_struct (&of_bucket_result, NULL, CM_GLU_BUCKET_DOESNOT_EXIST);
		*result_pp =of_bucket_result;
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
int32_t  of_bucket_endtrans(void *config_trans_p,
		uint32_t command_id,
		struct cm_app_result ** result_pp)
{
	struct group_trans *trans_rec=config_trans_p;
	struct ofi_bucket group_info={};
	struct cm_app_result *of_bucket_result = NULL;

	CM_CBK_DEBUG_PRINT ("Entered");

	if ( trans_rec )
	{
		CM_CBK_DEBUG_PRINT ("trans id %d command id %d sub cmd id %d group id %d",trans_rec->trans_id,trans_rec->command_id,
				trans_rec->sub_command_id,trans_rec->group_id);
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
int32_t of_bucket_getfirstnrecs(struct cm_array_of_iv_pairs * key_iv_pairs,
		uint32_t * pCount,
		struct cm_array_of_iv_pairs ** arr_ivpair_arr_pp)
{
	struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	int32_t return_value = OF_FAILURE;
	uint32_t uiRecCount = 0;
	struct cm_app_result *of_bucket_result = NULL;
	struct ofi_group_desc_info group_info={};
	struct ofi_bucket bucket_info={};


	CM_CBK_DEBUG_PRINT ("not supporting");
	return OF_FAILURE;

	of_memset (&group_info, 0, sizeof (struct ofi_group_desc_info));
	of_memset (&bucket_info, 0, sizeof (struct ofi_bucket));
	if ((of_bucket_ucm_setmandparams (key_iv_pairs,&group_info, &bucket_info, &of_bucket_result)) !=
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
int32_t of_bucket_getnextnrecs (struct cm_array_of_iv_pairs * key_iv_pairs,
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
int32_t of_bucket_getexactrec (struct cm_array_of_iv_pairs * key_iv_pairs,
		struct cm_array_of_iv_pairs ** pIvPairArr)
{
	struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	struct cm_app_result *of_bucket_result = NULL;
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
 int32_t of_bucket_ucm_setmandparams (struct cm_array_of_iv_pairs *
		mand_iv_pairs,
		struct  ofi_group_desc_info *group_info,
		struct ofi_bucket *bucket_info,
		struct cm_app_result ** app_result_pp)
{
	uint32_t opt_param_cnt;
	uint32_t group_id;
	uint32_t bucket_id;

	CM_CBK_DEBUG_PRINT ("Entered");
	for (opt_param_cnt = 0; opt_param_cnt < mand_iv_pairs->count_ui;
			opt_param_cnt++)
	{
		switch (mand_iv_pairs->iv_pairs[opt_param_cnt].id_ui)
		{
			case CM_DM_GROUPDESC_GROUP_ID_ID:
				group_id=of_atoi((char *) mand_iv_pairs->iv_pairs[opt_param_cnt].value_p);
				group_info->group_id=group_id;
				CM_CBK_DEBUG_PRINT ("group id is %d", group_id);
				break;
			case  CM_DM_BUCKET_BUCKETID_ID:
				bucket_id=of_atoi((char *) mand_iv_pairs->iv_pairs[opt_param_cnt].value_p);
				bucket_info->bucket_id=bucket_id;
				CM_CBK_DEBUG_PRINT ("bucket id %d", bucket_id);
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
 int32_t of_bucket_ucm_validatemandparams (struct cm_array_of_iv_pairs *
		mand_iv_pairs,
		struct cm_app_result ** app_result_pp)
{
	uint32_t opt_param_cnt;
	uint32_t group_id;
	uint8_t type;

	CM_CBK_DEBUG_PRINT ("Entered");
	for (opt_param_cnt = 0; opt_param_cnt < mand_iv_pairs->count_ui;
			opt_param_cnt++)
	{
		switch (mand_iv_pairs->iv_pairs[opt_param_cnt].id_ui)
		{
			case CM_DM_GROUPDESC_GROUP_ID_ID:
				group_id=of_atoi((char *) mand_iv_pairs->iv_pairs[opt_param_cnt].value_p);
				CM_CBK_DEBUG_PRINT ("group id is %d", group_id);
				break;
			case CM_DM_GROUPDESC_TYPE_ID:
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
 int32_t of_bucket_ucm_setoptparams (struct cm_array_of_iv_pairs *
		opt_iv_pairs,
		struct ofi_bucket * bucket_info,
		struct cm_app_result ** app_result_pp)
{
	uint32_t opt_param_cnt;
	uint16_t weight;
	uint32_t watch_port;
	uint32_t watch_group;

	CM_CBK_DEBUG_PRINT ("Entered");

	for (opt_param_cnt = 0; opt_param_cnt < opt_iv_pairs->count_ui; opt_param_cnt++)
	{
		switch (opt_iv_pairs->iv_pairs[opt_param_cnt].id_ui)
		{
			case CM_DM_BUCKET_WEIGHT_ID:
				weight=(uint16_t)of_atoi((char *) opt_iv_pairs->iv_pairs[opt_param_cnt].value_p);
				bucket_info->weight=weight;
				CM_CBK_DEBUG_PRINT ("weight  %d", weight);
				break;
			case CM_DM_BUCKET_WATCH_PORT_ID:
				watch_port=of_atoi((char *) opt_iv_pairs->iv_pairs[opt_param_cnt].value_p);
				bucket_info->watch_port=watch_port;
				CM_CBK_DEBUG_PRINT ("watch_port  %d", watch_port);
				break;
			case CM_DM_BUCKET_WATCH_GROUP_ID:
				watch_group=of_atoi((char *) opt_iv_pairs->iv_pairs[opt_param_cnt].value_p);
				bucket_info->watch_group=watch_group;
				CM_CBK_DEBUG_PRINT ("watch_group  %d", watch_group);
				break;
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
 int32_t of_bucket_ucm_getparams (struct ofi_bucket *bucket_info,
		struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
	char string[64];
	int32_t index = 0, jj, count_ui, iIndex;
#define CM_BUCKET_CHILD_COUNT 2
	count_ui = CM_BUCKET_CHILD_COUNT;
#if 0
	//CM_CBK_DEBUG_PRINT ("Entered");
	result_iv_pairs_p->iv_pairs = (struct cm_iv_pai*)of_calloc(count_ui, sizeof(struct cm_iv_pair));
	if(!result_iv_pairs_p->iv_pairs)
	{
		CM_CBK_DEBUG_PRINT (" Memory allocation failed ");
		return OF_FAILURE;
	}

	of_memset(string, 0, sizeof(string));
	of_itoa (bucket_info->group_id, string);
	FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_GROUPDESC_GROUP_ID_ID,CM_DATA_TYPE_STRING, string);
	index++;

	of_memset(string, 0, sizeof(string));
	of_sprintf(string,"%d",bucket_info->group_type);
	FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_TABLESTATS_ACTIVECOUNT_ID,CM_DATA_TYPE_STRING, string);
	index++;


	result_iv_pairs_p->count_ui = index;

	//CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);
#endif
	return OF_SUCCESS;
}
#endif /*OF_CM_SUPPORT */
