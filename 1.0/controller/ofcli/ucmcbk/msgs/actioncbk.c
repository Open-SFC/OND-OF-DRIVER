
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
 * File name: actioncbk.c
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

int32_t of_action_appl_ucmcbk_init (void);


void* of_action_starttrans(struct cm_array_of_iv_pairs   * key_iv_pairs,
		uint32_t command_id,
		struct cm_app_result ** result_pp);

int32_t  of_action_endtrans(void *config_trans_p,
		uint32_t command_id,
		struct cm_app_result ** result_pp);

int32_t of_action_addrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * mand_iv_pairs,
		struct cm_array_of_iv_pairs * opt_iv_pairs,
		struct cm_app_result ** result_pp);

int32_t of_action_setrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * mand_iv_pairs,
		struct cm_array_of_iv_pairs * opt_iv_pairs,
		struct cm_app_result ** result_pp);

int32_t of_action_delrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * key_iv_pairs,
		struct cm_app_result ** result_pp);

int32_t of_action_getfirstnrecs (struct cm_array_of_iv_pairs * key_iv_pairs,
		uint32_t * pCount,
		struct cm_array_of_iv_pairs ** pArrayofIvPairArr);

int32_t of_action_getnextnrecs (struct cm_array_of_iv_pairs * key_iv_pairs,
		struct cm_array_of_iv_pairs * pPrevRecordKey, uint32_t * pCount,
		struct cm_array_of_iv_pairs ** pNextNRecordData_p);

int32_t of_action_getexactrec (struct cm_array_of_iv_pairs * key_iv_pairs,
		struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t of_action_verifycfg (struct cm_array_of_iv_pairs * key_iv_pairs,
		uint32_t command_id, struct cm_app_result ** result_pp);

int32_t of_action_ucm_getparams (struct ofi_bucket *group_info,
		struct cm_array_of_iv_pairs * result_iv_pairs_p);

int32_t of_action_ucm_set_mand_params (struct cm_array_of_iv_pairs *
		mand_iv_pairs,
		struct  ofi_group_desc_info *group_info,
		struct ofi_bucket *bucket_info,
		struct ofi_action *action_info,
		struct cm_app_result ** result_pp);

int32_t of_action_ucm_set_opt_params (struct cm_array_of_iv_pairs *
		opt_iv_pairs,
		struct ofi_action * bucket_info,
		struct cm_app_result ** result_pp);

int32_t of_action_ucm_validate_mand_params (struct cm_array_of_iv_pairs *
		mand_iv_pairs,
		struct cm_app_result ** result_pp);

int32_t of_action_ucm_validate_opt_params (struct cm_array_of_iv_pairs *
		opt_iv_pairs,
		struct ofi_action * action_info,
		struct cm_app_result ** result_pp);

struct cm_dm_app_callbacks of_action_ucm_callbacks = 
{
	of_action_starttrans,
	of_action_addrec,
	of_action_setrec,
	of_action_delrec,
	of_action_endtrans,
	of_action_getfirstnrecs,
	of_action_getnextnrecs,
	of_action_getexactrec,
	of_action_verifycfg,
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
int32_t of_action_appl_ucmcbk_init (void)
{
	int32_t return_value;

	return_value=cm_register_app_callbacks ( CM_ON_DIRECTOR_DATAPATH_GROUPS_GROUP_BUCKET_ACTION_APPL_ID,
			&of_action_ucm_callbacks);
	if(return_value != OF_SUCCESS)
	{
		cntrlrucm_debugmsg("action application CBK registration failed");
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
int32_t of_action_verifycfg (struct cm_array_of_iv_pairs * key_iv_pairs,
		uint32_t command_id, struct cm_app_result ** result_pp)
{
	struct cm_app_result *of_action_result = NULL;
	int32_t return_value = OF_FAILURE;

	CM_CBK_DEBUG_PRINT ("Entered");

	return_value = of_action_ucm_validate_mand_params (key_iv_pairs, &of_action_result);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
	        *result_pp = of_action_result;
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
void* of_action_starttrans(struct cm_array_of_iv_pairs   * key_iv_pairs,
		uint32_t command_id,
		struct cm_app_result ** result_pp)
{
	struct group_trans *trans_rec=NULL;
	struct ofi_group_desc_info group_info={};
	struct ofi_bucket bucket_info={};
	struct ofi_action action_info={};
	struct cm_app_result *of_action_result = NULL;
	uchar8_t offset;
	offset = OF_GROUP_TRANS_LISTNODE_OFFSET;
	
	CM_CBK_DEBUG_PRINT ("Entered");


	if ((of_action_ucm_set_mand_params (key_iv_pairs,&group_info, &bucket_info, &action_info, &of_action_result)) !=
			OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		return NULL;
	}

	OF_LIST_SCAN(group_trans_list_g, trans_rec, struct group_trans *,offset)
	{
		if(trans_rec->group_id == group_info.group_id)
		{
			CM_CBK_DEBUG_PRINT ("transaction found");
			break;
		}

	}
	return trans_rec;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t of_action_addrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * mand_iv_pairs,
		struct cm_array_of_iv_pairs * opt_iv_pairs,
		struct cm_app_result ** result_pp)
{
	struct cm_app_result *of_action_result = NULL;
	int32_t return_value = OF_FAILURE;
	struct ofi_action *action_info = NULL;
	struct ofi_bucket bucket_info = { };
	struct ofi_group_desc_info group_info = { };
	struct group_trans *trans_rec=(struct group_trans *)config_trans_p;

	CM_CBK_DEBUG_PRINT ("Entered");
	of_memset (&bucket_info, 0, sizeof (struct ofi_bucket));

	action_info =(struct ofi_action *) calloc ( 1, sizeof(struct ofi_action));
	if (action_info == NULL )
	{
		CM_CBK_DEBUG_PRINT ("memory allocation Failed");
		fill_app_result_struct (&of_action_result, NULL, CM_GLU_MEMORY_ALLOCATION_FAILED);
		*result_pp =of_action_result;
		return OF_FAILURE;
	}

	if ((of_action_ucm_set_mand_params (mand_iv_pairs,&group_info, &bucket_info, action_info, &of_action_result)) !=
			OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		fill_app_result_struct (&of_action_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
		*result_pp=of_action_result;
		return OF_FAILURE;
	}

	return_value =of_action_ucm_validate_opt_params(opt_iv_pairs, action_info, &of_action_result);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Validate Optional Parameters Failed");
		//fill_app_result_struct (&of_action_result, NULL, CM_GLU_VALIDATE_OPT_PARAM_FAILED);
		*result_pp =of_action_result;
		return OF_FAILURE;
	}

	return_value =of_action_ucm_set_opt_params (opt_iv_pairs, action_info, &of_action_result);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");

		fill_app_result_struct (&of_action_result, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
		*result_pp =of_action_result;
		return OF_FAILURE;
	}

	return_value =of_register_action_to_bucket(trans_rec->datapath_handle,action_info,group_info.group_id, bucket_info.bucket_id);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Action addition to group %d failed", action_info->type);
		fill_app_result_struct (&of_action_result, NULL, CM_GLU_ACTION_ADD_FAILED);
		*result_pp =of_action_result;
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
int32_t of_action_setrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * mand_iv_pairs,
		struct cm_array_of_iv_pairs * opt_iv_pairs,
		struct cm_app_result ** result_pp)
{
	struct cm_app_result *of_action_result = NULL;
	int32_t return_value = OF_FAILURE;
	struct ofi_bucket bucket_info = { };
	struct dprm_switch_runtime_info runtime_info;
	struct group_trans *trans_rec=config_trans_p;


	CM_CBK_DEBUG_PRINT ("Entered");
	of_memset (&bucket_info, 0, sizeof (struct ofi_bucket));
#if 0
	if ((of_action_ucm_set_mand_params (mand_iv_pairs,&group_info, &bucket_info, &of_action_result)) !=
			OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		*result_pp=of_action_result;
		return OF_FAILURE;
	}


	return_value=of_action_unregister_buckets(bucket_info.group_id);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Error: group doesn't exists with name %d",bucket_info.group_id);
		fill_app_result_struct (&of_action_result, NULL, CM_GLU_VLAN_FAILED);
		*result_pp =of_action_result;
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
int32_t of_action_delrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * key_iv_pairs,
		struct cm_app_result ** result_pp)
{
	struct cm_app_result *of_action_result = NULL;
	int32_t return_value = OF_FAILURE;
	struct ofi_bucket bucket_info = { };
	struct ofi_group_desc_info group_info = { };
	struct ofi_action action_info={};
	struct group_trans *trans_rec=config_trans_p;

	CM_CBK_DEBUG_PRINT ("Entered");
	return OF_FAILURE;

	of_memset (&action_info, 0, sizeof (struct ofi_action));
	if ( trans_rec->command_id != CM_CMD_ADD_PARAMS)
	{
		CM_CBK_DEBUG_PRINT ("Delete not supported");
		return OF_FAILURE;
	}

	if ((of_action_ucm_set_mand_params (key_iv_pairs,&group_info, &bucket_info, &action_info, &of_action_result)) !=
			OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		fill_app_result_struct (&of_action_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
		*result_pp =of_action_result;
		return OF_FAILURE;
	}

	return_value=of_group_unregister_bucket(trans_rec->datapath_handle,group_info.group_id, bucket_info.bucket_id);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Action does not exist with id %d",action_info.type);
		fill_app_result_struct (&of_action_result, NULL, CM_GLU_ACTION_DOESNOT_EXIST);
		*result_pp =of_action_result;
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
int32_t  of_action_endtrans(void *config_trans_p,
		uint32_t command_id,
		struct cm_app_result ** result_pp)
{
	struct group_trans *trans_rec=config_trans_p;
	struct ofi_bucket group_info={};
	struct cm_app_result *of_action_result = NULL;

	CM_CBK_DEBUG_PRINT ("Entered");

	if ( trans_rec)
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
int32_t of_action_getfirstnrecs(struct cm_array_of_iv_pairs * key_iv_pairs,
		uint32_t * pCount,
		struct cm_array_of_iv_pairs ** arr_ivpair_arr_pp)
{
	struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	int32_t return_value = OF_FAILURE;
	uint32_t uiRecCount = 0;
	struct cm_app_result *of_action_result = NULL;
	struct ofi_group_desc_info group_info={};
	struct ofi_bucket bucket_info={};
	struct ofi_action action_info={};


	CM_CBK_DEBUG_PRINT ("not supporting");
	return OF_FAILURE;

	of_memset (&group_info, 0, sizeof (struct ofi_group_desc_info));
	of_memset (&bucket_info, 0, sizeof (struct ofi_bucket));
	if ((of_action_ucm_set_mand_params (key_iv_pairs,&group_info, &bucket_info, &action_info, &of_action_result)) !=
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
int32_t of_action_getnextnrecs (struct cm_array_of_iv_pairs * key_iv_pairs,
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
int32_t of_action_getexactrec (struct cm_array_of_iv_pairs * key_iv_pairs,
		struct cm_array_of_iv_pairs ** pIvPairArr)
{
	struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	struct cm_app_result *of_action_result = NULL;
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
int32_t of_action_ucm_set_mand_params (struct cm_array_of_iv_pairs *
		mand_iv_pairs,
		struct  ofi_group_desc_info *group_info,
		struct ofi_bucket *bucket_info,
		struct ofi_action *action_info,
		struct cm_app_result ** result_pp)
{
	uint32_t param_count;
	uint32_t group_id;
	uint32_t bucket_id;
	char *action_type;
	uint32_t action_type_len;

	CM_CBK_DEBUG_PRINT ("Entered");
	for (param_count = 0; param_count < mand_iv_pairs->count_ui;
			param_count++)
	{
		switch (mand_iv_pairs->iv_pairs[param_count].id_ui)
		{
			case CM_DM_GROUPDESC_GROUP_ID_ID:
				group_id=of_atoi((char *) mand_iv_pairs->iv_pairs[param_count].value_p);
				group_info->group_id=group_id;
				CM_CBK_DEBUG_PRINT ("group id is %d", group_id);
				break;
			case  CM_DM_BUCKET_BUCKETID_ID:
				bucket_id=of_atoi((char *) mand_iv_pairs->iv_pairs[param_count].value_p);
				bucket_info->bucket_id=bucket_id;
				CM_CBK_DEBUG_PRINT ("bucket id %d", bucket_id);
				break;
			case  CM_DM_ACTION_TYPE_ID:
				action_type=((char *) mand_iv_pairs->iv_pairs[param_count].value_p);
				action_type_len = (mand_iv_pairs->iv_pairs[param_count].value_length);
				of_group_set_action_type(action_info,action_type,action_type_len);

				//action_info->type=(uint16_t )action_type;
				CM_CBK_DEBUG_PRINT ("action type %d %s ",action_info->type,action_type);
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
int32_t of_action_ucm_validate_mand_params (struct cm_array_of_iv_pairs *
		mand_iv_pairs,
		struct cm_app_result ** result_pp)
{
	uint32_t param_count;
	uint32_t group_id;
	uint8_t type;
	uint32_t action_type;
	struct cm_app_result *of_action_result=NULL;
	CM_CBK_DEBUG_PRINT ("Entered");
	for (param_count = 0; param_count < mand_iv_pairs->count_ui;
			param_count++)
	{
		switch (mand_iv_pairs->iv_pairs[param_count].id_ui)
		{
			case CM_DM_GROUPDESC_GROUP_ID_ID:
				group_id=of_atoi((char *) mand_iv_pairs->iv_pairs[param_count].value_p);
				CM_CBK_DEBUG_PRINT ("group id is %d", group_id);
				break;

			case CM_DM_ACTION_TYPE_ID:
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

int32_t of_action_ucm_validate_opt_params (struct cm_array_of_iv_pairs *
		opt_iv_pairs,
		struct ofi_action * action_info,
		struct cm_app_result ** result_pp)
{
	struct cm_app_result *of_action_result = NULL;
	uint32_t param_count;
	char *port_no;
	uint16_t  max_len;
	uint16_t  ttl;
	uint16_t ether_type;
	uint32_t  queue_id;
	uint32_t  group_id;

	uint8_t port_no_b=FALSE;
	uint8_t max_len_b=FALSE;
	uint8_t ttl_b=FALSE;
	uint8_t queue_id_b=FALSE;
	uint8_t group_id_b=FALSE;
	uint8_t ether_type_b=FALSE;

	CM_CBK_DEBUG_PRINT ("Entered");

	for (param_count = 0; param_count < opt_iv_pairs->count_ui; param_count++)
	{
		switch (opt_iv_pairs->iv_pairs[param_count].id_ui)
		{
			case CM_DM_ACTION_PORT_NO_ID:
				port_no=((char *) opt_iv_pairs->iv_pairs[param_count].value_p);
				CM_CBK_DEBUG_PRINT ("port_no  %s", port_no);
				port_no_b=TRUE;
				break;
			case CM_DM_ACTION_MAX_LEN_ID:
				max_len=of_atoi((char *) opt_iv_pairs->iv_pairs[param_count].value_p);
				CM_CBK_DEBUG_PRINT ("max_len  %d", max_len);
				max_len_b=TRUE;
				break;
			case CM_DM_ACTION_TTL_ID:
				ttl=of_atoi((char *) opt_iv_pairs->iv_pairs[param_count].value_p);
				CM_CBK_DEBUG_PRINT ("ttl  %d", ttl);
				ttl_b=TRUE;
				break;
			case CM_DM_ACTION_ETHER_TYPE_ID:
				ether_type=of_atoi((char *) opt_iv_pairs->iv_pairs[param_count].value_p);
				CM_CBK_DEBUG_PRINT ("ether_type  %d", ether_type);
				ether_type_b=TRUE;
				break;
			case CM_DM_ACTION_GROUPID_ID:
				group_id=of_atoi((char *) opt_iv_pairs->iv_pairs[param_count].value_p);
				CM_CBK_DEBUG_PRINT ("group_id  %d", group_id);
				group_id_b=TRUE;
				break;
			case CM_DM_ACTION_QUEUEID_ID:
				queue_id=of_atoi((char *) opt_iv_pairs->iv_pairs[param_count].value_p);
				CM_CBK_DEBUG_PRINT ("queue_id  %d", queue_id);
				queue_id_b=TRUE;
				break;

			default:
				break;
		}
	}

	switch (action_info->type)
	{
		case  OFPAT_OUTPUT :
			if ( port_no_b == FALSE )			
			{
				CM_CBK_DEBUG_PRINT ("port_no or max_len not configured");
				fill_app_result_struct (&of_action_result, NULL, CM_GLU_PORT_OR_MAXLEN_NOT_CONFIGURED);
				*result_pp =of_action_result;
				return OF_FAILURE;
			}
			else{
				if((!strcmp(port_no,"0xfffffffd")) && (max_len_b == FALSE)){
					CM_CBK_DEBUG_PRINT ("port_no or max_len not configured");
					fill_app_result_struct (&of_action_result, NULL, CM_GLU_PORT_OR_MAXLEN_NOT_CONFIGURED);
					*result_pp =of_action_result;
					return OF_FAILURE;
				}

			}

			break;
		case    OFPAT_SET_NW_TTL:
		case    OFPAT_SET_MPLS_TTL:
			if (( ttl_b == FALSE ) )
			{
				CM_CBK_DEBUG_PRINT ("ttl value not set");
				fill_app_result_struct (&of_action_result, NULL, CM_GLU_TTL_NOT_CONFIGURED);
				*result_pp =of_action_result;
				return OF_FAILURE;
			}
			break;
		case OFPAT_PUSH_VLAN :
		case OFPAT_PUSH_MPLS :
		case OFPAT_POP_MPLS:
		case OFPAT_PUSH_PBB :
			if (( ether_type_b == FALSE ) )
			{
				CM_CBK_DEBUG_PRINT ("ether type not set");
				fill_app_result_struct (&of_action_result, NULL, CM_GLU_ETHER_TYPE_NOT_CONFIGURED);
				*result_pp =of_action_result;
				return OF_FAILURE;
			}
			break;
		case    OFPAT_SET_QUEUE:
			if (( queue_id_b == FALSE ) )
			{
				CM_CBK_DEBUG_PRINT ("queue id not set");
				fill_app_result_struct (&of_action_result, NULL, CM_GLU_QUEUE_ID_NOT_CONFIGURED);
				*result_pp =of_action_result;
				return OF_FAILURE;
			}
			break;
		case    OFPAT_GROUP:
			if (( group_id_b == FALSE ) )
			{
				CM_CBK_DEBUG_PRINT ("group id not set");
				fill_app_result_struct (&of_action_result, NULL, CM_GLU_GROUP_ID_NOT_CONFIGURED);
				*result_pp =of_action_result;
				return OF_FAILURE;
			}
			break;
		case OFPAT_COPY_TTL_OUT:
		case OFPAT_COPY_TTL_IN:
		case OFPAT_DEC_MPLS_TTL:
		case OFPAT_POP_VLAN:
		case OFPAT_POP_PBB:      
		case OFPAT_DEC_NW_TTL:
		case OFPAT_SET_FIELD :
			break;
		default:
			CM_CBK_DEBUG_PRINT ("unknown action type %d", action_info->type);
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
int32_t of_action_ucm_set_opt_params (struct cm_array_of_iv_pairs *
		opt_iv_pairs,
		struct ofi_action * action_info,
		struct cm_app_result ** result_pp)
{
	uint32_t param_count;

	struct cm_app_result *of_action_result = NULL;
	uint32_t  port_no;
	uint32_t  max_len;
	uint32_t  ttl;
	uint32_t ether_type;
	uint32_t  queue_id;
	uint32_t  group_id;
	char *data;

	CM_CBK_DEBUG_PRINT ("Entered");

	for (param_count = 0; param_count < opt_iv_pairs->count_ui; param_count++)
	{
		switch (opt_iv_pairs->iv_pairs[param_count].id_ui)
		{
			{
				case CM_DM_ACTION_PORT_NO_ID:
					data = (char *) opt_iv_pairs->iv_pairs[param_count].value_p;
					if ((*data == '0') &&
							((*(data+1) == 'x') || (*(data+1) == 'X')))
					{
						action_info->port_no = (uint32_t)atox_32(data+2);
					}
					else
					{
						action_info->port_no = (uint32_t)atoi(data);
					}
					CM_CBK_DEBUG_PRINT ("port_no  %d action_info->port_no %d", port_no, action_info->port_no );
				break;
				case CM_DM_ACTION_MAX_LEN_ID:
					max_len=of_atoi((char *) opt_iv_pairs->iv_pairs[param_count].value_p);
					action_info->max_len=(uint16_t ) max_len;
					CM_CBK_DEBUG_PRINT ("max_len  %d action_info->max_len %d ", max_len,action_info->max_len);
				break;
				case CM_DM_ACTION_TTL_ID:
					ttl=ucm_uint8_from_str_ptr((char *) opt_iv_pairs->iv_pairs[param_count].value_p);
					action_info->ttl= ttl;
					CM_CBK_DEBUG_PRINT ("ttl  %d action_info->ttl %d ", ttl,action_info->ttl);
					break;
				case CM_DM_ACTION_ETHER_TYPE_ID:
					ether_type=of_atoi((char *) opt_iv_pairs->iv_pairs[param_count].value_p);
					action_info->ether_type=(uint16_t ) ether_type;
					CM_CBK_DEBUG_PRINT ("ether_type  %d action_info->ether_type %d ", ether_type,action_info->ether_type);
					break;
				case CM_DM_ACTION_GROUPID_ID:
					group_id=of_atoi((char *) opt_iv_pairs->iv_pairs[param_count].value_p);
					action_info->group_id= group_id;
					CM_CBK_DEBUG_PRINT ("group_id  %d action_info->group_id %d ", group_id,action_info->group_id);
					break;
				case CM_DM_ACTION_QUEUEID_ID:
					queue_id=of_atoi((char *) opt_iv_pairs->iv_pairs[param_count].value_p);
					action_info->queue_id= queue_id;
					CM_CBK_DEBUG_PRINT ("queue_id  %d action_info->queue_id %d ", queue_id,action_info->queue_id);
				break;

				case CM_DM_ACTION_SETFIELDTYPE_ID:
					if(action_info->type != OFPAT_SET_FIELD)
					{
						CM_CBK_DEBUG_PRINT("Action type is not set_field ");
						return  OF_FAILURE;
					}
					if(of_group_set_action_setfield_type(action_info,
						(int8_t *)opt_iv_pairs->iv_pairs[param_count].value_p,
					opt_iv_pairs->iv_pairs[param_count].value_length)!=OF_SUCCESS)
					{
						CM_CBK_DEBUG_PRINT("set action field type failed");
						return OF_FAILURE;
					}
				break;

				case CM_DM_ACTION_SETFIELDVAL_ID:
					if(action_info->type != OFPAT_SET_FIELD)
					{
						CM_CBK_DEBUG_PRINT("Action type is not set_field");
						return OF_FAILURE;
					}
					if(of_group_set_action_setfieldtype_value(action_info,
					   (int8_t *)opt_iv_pairs->iv_pairs[param_count].value_p,
					  opt_iv_pairs->iv_pairs[param_count].value_length)!=OF_SUCCESS)
					{
						CM_CBK_DEBUG_PRINT("set action field type value failed!.");
						return  OF_FAILURE;

					}
				break;

			default:
				break;
			}
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
 int32_t of_action_ucm_getparams (struct ofi_bucket *bucket_info,
		struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
	char string[64];
	int32_t index = 0, jj, count_ui, iIndex;
#if 0
#define CM_BUCKET_CHILD_COUNT 2
	count_ui = CM_BUCKET_CHILD_COUNT;
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
