
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
 * File name: flowmatchcbk.c
 * Author: Kumara Swamy G. <B38525@freescale.com>
 * Date:   05/13/2013
 * Description: 
 * 
 */

#ifdef OF_CM_SUPPORT

#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "of_utilities.h"
#include "of_multipart.h"
#include "of_flowinfo.h"
#include "of_flowmod.h"


int32_t of_flow_match_appl_ucmcbk_init (void);


void* of_flow_match_starttrans(struct cm_array_of_iv_pairs   * key_iv_pairs,
		uint32_t command_id,
		struct cm_app_result ** result_pp);

int32_t  of_flow_match_endtrans(void *config_trans_p,
		uint32_t command_id,
		struct cm_app_result ** result_pp);

int32_t of_flow_match_addrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * mand_iv_pairs,
		struct cm_array_of_iv_pairs * opt_iv_pairs,
		struct cm_app_result ** result_pp);

int32_t of_flow_match_setrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * mand_iv_pairs,
		struct cm_array_of_iv_pairs * opt_iv_pairs,
		struct cm_app_result ** result_pp);

int32_t of_flow_match_delrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * key_iv_pairs,
		struct cm_app_result ** result_pp);

int32_t of_flow_match_verifycfg (struct cm_array_of_iv_pairs * key_iv_pairs,
		uint32_t command_id, struct cm_app_result ** result_pp);


int32_t of_flow_match_verifycfg (struct cm_array_of_iv_pairs * key_iv_pairs,
		uint32_t command_id, struct cm_app_result ** result_pp);

static int32_t of_flow_match_ucm_getparams(uint32_t flow_id, 
                       struct ofi_flow_match_fields *match_info,
                       struct cm_array_of_iv_pairs * result_iv_pairs_p);

int32_t of_flow_match_ucm_setmandparams (struct cm_array_of_iv_pairs *mand_iv_pairs,
                                 struct dprm_datapath_general_info *data_path_p,
		                         struct  ofi_flow_mod_info *flow_info,
		                         struct ofi_flow_match_fields *flow_match_info,
		                         struct cm_app_result **result_pp);

int32_t of_flow_match_ucm_setoptparams (struct cm_array_of_iv_pairs *opt_iv_pairs,
		                        struct ofi_flow_match_fields *info,
		                        struct cm_app_result **result_pp);

int32_t of_flow_match_get_exactrec(struct cm_array_of_iv_pairs * key_iv_pairs,
                                   struct cm_array_of_iv_pairs ** result_iv_pairs_pp);

struct cm_dm_app_callbacks of_flow_match_ucm_callbacks = 
{
	of_flow_match_starttrans,
	of_flow_match_addrec,
	NULL,
	of_flow_match_delrec,
	of_flow_match_endtrans,
	NULL,
	NULL,
	of_flow_match_get_exactrec,
	of_flow_match_verifycfg,
	NULL
};

extern of_list_t flow_trans_list_g;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_flow_match_appl_ucmcbk_init (void)
{
	int32_t return_value;

	return_value= cm_register_app_callbacks (CM_ON_DIRECTOR_DATAPATH_FLOWMOD_MATCHFIELD_APPL_ID,
			&of_flow_match_ucm_callbacks);
	if(return_value != OF_SUCCESS)
	{
		cntrlrucm_debugmsg("flow_match application CBK registration failed");
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
int32_t of_flow_match_verifycfg (struct cm_array_of_iv_pairs * key_iv_pairs,
		uint32_t command_id, struct cm_app_result ** result_pp)
{
	struct cm_app_result *of_flow_match_result = NULL;
	int32_t return_value = OF_FAILURE;

	CM_CBK_DEBUG_PRINT ("Entered");

	return_value = of_flow_match_ucm_validatemandparams (key_iv_pairs, &of_flow_match_result);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
		return OF_FAILURE;
	}
	*result_pp = of_flow_match_result;
	return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void* of_flow_match_starttrans(struct cm_array_of_iv_pairs   * key_iv_pairs,
		uint32_t command_id,
		struct cm_app_result ** result_pp)
{
  struct flow_trans *trans_rec=NULL;
  struct ofi_flow_mod_info flow_info={};
  struct ofi_flow_match_fields flow_match_info={};
  struct cm_app_result *of_flow_match_result = NULL;
  uchar8_t offset;
  offset = OF_FLOW_TRANS_LISTNODE_OFFSET;


  CM_CBK_DEBUG_PRINT ("Entered");


  if ((of_flow_match_ucm_setmandparams(key_iv_pairs, NULL, &flow_info,
                                      &flow_match_info,
                                      &of_flow_match_result)) != OF_SUCCESS)
  {
      CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
      return NULL;
  }

  OF_LIST_SCAN(flow_trans_list_g, trans_rec, struct flow_trans *,offset)
  {
	  if(trans_rec->flow_id == flow_info.flow_id)
	  {
		  CM_CBK_DEBUG_PRINT ("transflow_match found");
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

int32_t of_flow_match_addrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * mand_iv_pairs,
		struct cm_array_of_iv_pairs * opt_iv_pairs,
		struct cm_app_result ** result_pp)
{
 struct cm_app_result *of_flow_match_result = NULL;
 int32_t return_value = OF_FAILURE;
 struct ofi_flow_match_fields *flow_match_info = NULL;
 struct ofi_flow_mod_info flow_info = { };
 struct flow_trans *trans_rec=(struct flow_trans *)config_trans_p;

 CM_CBK_DEBUG_PRINT ("Entered");

 flow_match_info =(struct ofi_flow_match_fields *) calloc ( 1, sizeof(struct ofi_flow_match_fields));
 if (flow_match_info == NULL )
 {
	 CM_CBK_DEBUG_PRINT ("memory allocation Failed");
	 fill_app_result_struct (&of_flow_match_result, NULL, CM_GLU_VLAN_FAILED);
	 *result_pp =of_flow_match_result;
	 return OF_FAILURE;
 }

 if ((of_flow_match_ucm_setmandparams(mand_iv_pairs, NULL,
				 &flow_info, flow_match_info,
				 &of_flow_match_result)) != OF_SUCCESS)
 {
	 CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
	 fill_app_result_struct (&of_flow_match_result, NULL, CM_GLU_VLAN_FAILED);
	 *result_pp=of_flow_match_result;
	 return OF_FAILURE;
 }

 return_value = of_flow_match_ucm_setoptparams (opt_iv_pairs, flow_match_info, 
		 &of_flow_match_result);
 if (return_value != OF_SUCCESS)
 {
	 CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");

	 fill_app_result_struct (&of_flow_match_result, NULL, CM_GLU_VLAN_FAILED);
	 *result_pp =of_flow_match_result;
	 return OF_FAILURE;
 }

 return_value = of_flow_add_match_field_to_list(trans_rec->datapath_handle,
		 flow_match_info, flow_info.flow_id);
 if (return_value != OF_SUCCESS)
 {
	 CM_CBK_DEBUG_PRINT ("flow_match Addition to flow %d Failed", flow_match_info->field_type);
	 fill_app_result_struct (&of_flow_match_result, NULL, CM_GLU_VLAN_FAILED);
	 *result_pp =of_flow_match_result;
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
int32_t of_flow_match_setrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * mand_iv_pairs,
		struct cm_array_of_iv_pairs * opt_iv_pairs,
		struct cm_app_result ** result_pp)
{
	struct cm_app_result *of_flow_match_result = NULL;
	int32_t return_value = OF_FAILURE;
	struct dprm_switch_runtime_info runtime_info;
	struct flow_trans *trans_rec=config_trans_p;


	CM_CBK_DEBUG_PRINT ("Entered");

#if 0
	if ((of_flow_match_ucm_setmandparams (mand_iv_pairs,&flow_info, &bucket_info, &of_flow_match_result)) !=
			OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		*result_pp=of_flow_match_result;
		return OF_FAILURE;
	}


	return_value=of_flow_match_unregister_buckets(bucket_info.flow_id);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Error: flow doesn't exists with name %d",bucket_info.flow_id);
		fill_app_result_struct (&of_flow_match_result, NULL, CM_GLU_VLAN_FAILED);
		*result_pp =of_flow_match_result;
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
int32_t of_flow_match_delrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * key_iv_pairs,
		struct cm_app_result ** result_pp)
{
	struct cm_app_result *of_flow_match_result = NULL;
	int32_t return_value = OF_FAILURE;
	struct ofi_flow_mod_info flow_info = { };
	struct ofi_flow_match_fields flow_match_info={};
	struct flow_trans *trans_rec=config_trans_p;

	CM_CBK_DEBUG_PRINT ("Entered");
	return OF_FAILURE;

	of_memset (&flow_match_info, 0, sizeof (struct ofi_flow_match_fields));
	if ( trans_rec->command_id != CM_CMD_DEL_PARAMS)
	{
		CM_CBK_DEBUG_PRINT ("Delete not supported");
		return OF_FAILURE;
	}

	if ((of_flow_match_ucm_setmandparams (key_iv_pairs, NULL,
                                     &flow_info, &flow_match_info,
                                     &of_flow_match_result)) != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		fill_app_result_struct (&of_flow_match_result, NULL, CM_GLU_VLAN_FAILED);
		*result_pp =of_flow_match_result;
		return OF_FAILURE;
	}

	return_value = of_flow_remove_match_field_from_list(trans_rec->datapath_handle,
                                        flow_info.flow_id,
                                        flow_match_info.field_id);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Error: flow_match does not exist with id %d",flow_match_info.field_type);
		fill_app_result_struct (&of_flow_match_result, NULL, CM_GLU_VLAN_FAILED);
		*result_pp =of_flow_match_result;
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
int32_t  of_flow_match_endtrans(void *config_trans_p,
		uint32_t command_id,
		struct cm_app_result ** result_pp)
{
	struct flow_trans *trans_rec=config_trans_p;
	struct ofi_bucket flow_info={};
	struct cm_app_result *of_flow_match_result = NULL;

	CM_CBK_DEBUG_PRINT ("Entered");

	if (trans_rec)
	{
       CM_CBK_DEBUG_PRINT ("trans id %d command id %d sub cmd id %d flow id %d",trans_rec->trans_id,trans_rec->command_id,
       trans_rec->sub_command_id,trans_rec->flow_id);
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
int32_t of_flow_match_ucm_setmandparams(struct cm_array_of_iv_pairs *mand_iv_pairs,
                                        struct dprm_datapath_general_info *data_path,
                                        struct  ofi_flow_mod_info *flow_info,
                                        struct ofi_flow_match_fields *flow_match_info,
                                        struct cm_app_result ** result_pp)
{
	uint32_t opt_param_cnt;
	uint32_t flow_id;
	uint32_t uidp_id;
	uint32_t flow_match_type;

	CM_CBK_DEBUG_PRINT ("Entered");
	for (opt_param_cnt = 0; opt_param_cnt < mand_iv_pairs->count_ui;
			opt_param_cnt++)
	{
		switch (mand_iv_pairs->iv_pairs[opt_param_cnt].id_ui)
		{
      case CM_DM_DATAPATH_DATAPATHID_ID:
           uidp_id=charTo64bitNum((char *) mand_iv_pairs->iv_pairs[opt_param_cnt].value_p);
           CM_CBK_DEBUG_PRINT("dpid is: %llx",uidp_id);
           if(data_path)
           {
             data_path->dpid = uidp_id;
           }
           break;
			case CM_DM_FLOWMOD_FLOWID_ID:
           flow_id = of_atoi((char *) mand_iv_pairs->iv_pairs[opt_param_cnt].value_p);
           flow_info->flow_id = flow_id;
           CM_CBK_DEBUG_PRINT ("flow id is %d", flow_id);
           break;
      case CM_DM_MATCHFIELD_FIELDID_ID:
           flow_match_info->field_id = 
             of_atoi((char *) mand_iv_pairs->iv_pairs[opt_param_cnt].value_p);
           break;
			case  CM_DM_MATCHFIELD_MATCHFIELDTYPE_ID:	
           of_flow_set_match_field_type(flow_match_info,
               (char *)mand_iv_pairs->iv_pairs[opt_param_cnt].value_p,
               mand_iv_pairs->iv_pairs[opt_param_cnt].value_length);
           CM_CBK_DEBUG_PRINT ("flow_match type %d %d ",
               flow_match_info->field_type,flow_match_type);
           break;
      case  CM_DM_MATCHFIELD_MATCHFIELDDATA_ID:

           of_flow_set_match_field_data(flow_match_info,
               (char *)mand_iv_pairs->iv_pairs[opt_param_cnt].value_p,
               mand_iv_pairs->iv_pairs[opt_param_cnt].value_length);
           CM_CBK_DEBUG_PRINT ("flow_match type %d %d ",
               flow_match_info->field_type,flow_match_type);
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
 int32_t of_flow_match_ucm_validatemandparams (struct cm_array_of_iv_pairs *
		mand_iv_pairs,
		struct cm_app_result ** result_pp)
{
	uint32_t opt_param_cnt;
	uint32_t flow_id;
	uint8_t type;
	uint32_t flow_match_type;
	struct cm_app_result *of_flow_match_result=NULL;
	CM_CBK_DEBUG_PRINT ("Entered");
	for (opt_param_cnt = 0; opt_param_cnt < mand_iv_pairs->count_ui;
			opt_param_cnt++)
	{
		switch (mand_iv_pairs->iv_pairs[opt_param_cnt].id_ui)
		{
			case CM_DM_DATAPATH_FLOWMOD_ID:
				flow_id=of_atoi((char *) mand_iv_pairs->iv_pairs[opt_param_cnt].value_p);
				CM_CBK_DEBUG_PRINT ("flow id is %d", flow_id);
				break;

			case CM_DM_MATCHFIELD_FIELDID_ID:
				flow_match_type=of_atoi((char *) mand_iv_pairs->iv_pairs[opt_param_cnt].value_p);
				CM_CBK_DEBUG_PRINT ("flow_match_type is %d", flow_match_type);
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
int32_t of_flow_match_ucm_setoptparams (struct cm_array_of_iv_pairs *opt_iv_pairs,
                                           struct ofi_flow_match_fields *flow_match_info,
                                           struct cm_app_result ** result_pp)
{
	uint32_t opt_param_cnt;
	uint16_t weight;
	uint32_t watch_port;
	uint32_t watch_flow;

	CM_CBK_DEBUG_PRINT ("Entered");

	for (opt_param_cnt = 0; opt_param_cnt < opt_iv_pairs->count_ui; opt_param_cnt++)
	{
		switch (opt_iv_pairs->iv_pairs[opt_param_cnt].id_ui)
		{
                  case CM_DM_MATCHFIELD_ISMASKSET_ID:
                    flow_match_info->is_mask_set =
                      (uint8_t)of_atoi((char *)opt_iv_pairs->iv_pairs[opt_param_cnt].value_p);
                    CM_CBK_DEBUG_PRINT ("match field is mask set %d",
                        flow_match_info->is_mask_set);
                    break;

                  case CM_DM_MATCHFIELD_MASK_ID:
                    CM_CBK_DEBUG_PRINT ("match field mask  ");
                    of_flow_set_match_field_mask(flow_match_info,
                        opt_iv_pairs->iv_pairs[opt_param_cnt].value_p,
                        opt_iv_pairs->iv_pairs[opt_param_cnt].value_length);   				
                    break;
                  default:
                    break;
                }
	}

	CM_CBK_PRINT_IVPAIR_ARRAY (opt_iv_pairs);
	return OF_SUCCESS;
}

static int32_t of_flow_match_ucm_getparams(uint32_t flow_id, 
                       struct ofi_flow_match_fields *match_info,
                       struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
  char string[64];
  int32_t index = 0, jj, uiCount;

  uiCount = 2;//CM_FLOWMOD_CHILD_COUNT;

  CM_CBK_DEBUG_PRINT ("Entered");
  result_iv_pairs_p->iv_pairs = (struct cm_iv_pair*)calloc(uiCount, sizeof(struct cm_iv_pair));
  if (!result_iv_pairs_p->iv_pairs)
  {
      CM_CBK_DEBUG_PRINT (" Memory allocation failed ");
      return OF_FAILURE;
  }

  memset(string, 0, sizeof(string));
  of_itoa(flow_id, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_FLOWMOD_FLOWID_ID,
                  CM_DATA_TYPE_STRING, string);
  index++;

  memset(string, 0, sizeof(string));
  of_itoa(match_info->field_id, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_MATCHFIELD_FIELDID_ID,
                  CM_DATA_TYPE_STRING, string);
  index++;

  result_iv_pairs_p->count_ui = index;

  CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);
  return OF_SUCCESS;
}

int32_t of_flow_match_get_exactrec(struct cm_array_of_iv_pairs * key_iv_pairs,
                                   struct cm_array_of_iv_pairs ** result_iv_pairs_pp)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct cm_app_result *flow_result = NULL;
  uint32_t ii = 0;
  int32_t return_value = OF_FAILURE;
  struct ofi_flow_match_fields *match_entry_p;
  struct ofi_flow_mod_info flow_info={};
  struct ofi_flow_match_fields match_info={};
  struct dprm_datapath_general_info datapath_info={};
  uint64_t datapath_handle;

  CM_CBK_DEBUG_PRINT ("Entered");
  if (of_flow_match_ucm_setmandparams(key_iv_pairs, &datapath_info, &flow_info,
                                      &match_info, &flow_result) != OF_SUCCESS)
  {
     CM_CBK_DEBUG_PRINT("Set Mandatory Parameters Failed");
     return OF_FAILURE;
  }

  return_value = dprm_get_datapath_handle(datapath_info.dpid, &datapath_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: datapath does not exist with id %llx",datapath_info.dpid);
    return OF_FAILURE;
  }

  return_value = of_get_flow_match_field(datapath_handle, flow_info.flow_id,
                                         match_info.field_id, &match_entry_p);
  if (return_value != OF_SUCCESS)
  {
     CM_CBK_DEBUG_PRINT ("of_get_flow_match_field failed");
     return OF_FAILURE;
  }

  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for pResultiv_pairs");
    return OF_FAILURE;
  }

  return_value=of_flow_match_ucm_getparams(flow_info.flow_id,
                                           match_entry_p, &result_iv_pairs_p[0]);
  if(return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("of_group_ucm_getparams failed");
    return OF_FAILURE;
  } 

  *result_iv_pairs_pp = result_iv_pairs_p;

  return OF_SUCCESS;
}

#endif /*OF_CM_SUPPORT */
