
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
 * File name: cntlrrolecbk.c
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

int32_t of_cntlrrole_appl_ucmcbk_init (void);
int32_t of_cntlrrole_modrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * mand_iv_pairs_p,
		struct cm_array_of_iv_pairs * opt_iv_pairs_p,
		struct cm_app_result ** result_p);

int32_t of_cntlrrole_getexactrec (struct cm_array_of_iv_pairs * pKeysArr,
		struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t of_cntlrrole_verifycfg (struct cm_array_of_iv_pairs * pKeysArr,
		uint32_t command_id, struct cm_app_result ** result_p);




int32_t of_cntlrrole_getparams (uint8_t   controller_role,
		struct cm_array_of_iv_pairs * result_iv_pairs_p);

int8_t cntlr_role_config_b=FALSE;
struct cm_dm_app_callbacks of_cntlrrole_ucm_callbacks = {
	NULL,
	NULL,
	of_cntlrrole_modrec,
	NULL,
	NULL,
	NULL,
	NULL,
	of_cntlrrole_getexactrec,
	of_cntlrrole_verifycfg,
	NULL
};


int32_t of_cntlrrole_appl_ucmcbk_init (void)
{
	CM_CBK_DEBUG_PRINT ("Entry");
	cm_register_app_callbacks(CM_ON_DIRECTOR_DATAPATH_ONDIRECTORROLE_APPL_ID, &of_cntlrrole_ucm_callbacks);
	return OF_SUCCESS;
}


int32_t of_cntlrrole_modrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * mand_iv_pairs_p,
		struct cm_array_of_iv_pairs * opt_iv_pairs_p,
		struct cm_app_result ** result_p)
{
	uint64_t datapath_handle;
	int32_t  retval= CNTLR_SUCCESS;
	int32_t  status= OF_SUCCESS;
	uint32_t iRetVal=OF_FAILURE;
	char     cntlr_role_config[30]={};
	uint8_t   controller_role;
	uint64_t generation_id=1;

	struct   cm_app_result *of_cntlrrole_result = NULL;
	struct   dprm_datapath_general_info datapath_info={};
	struct   of_msg  *msg=NULL;


	CM_CBK_DEBUG_PRINT ("Entry");

	if ((of_cntlrrole_ucm_setmandparams (mand_iv_pairs_p, &datapath_info,  &of_cntlrrole_result)) !=
			OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		*result_p=of_cntlrrole_result;
		return OF_FAILURE;
	}

	iRetVal=dprm_get_datapath_handle(datapath_info.dpid, &datapath_handle);
	if (iRetVal != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		*result_p=of_cntlrrole_result;
		return OF_FAILURE;
	}


	
	if ((of_cntlrrole_ucm_setoptparams (opt_iv_pairs_p,   cntlr_role_config, &of_cntlrrole_result)) !=
			OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
		*result_p=of_cntlrrole_result;
		return OF_FAILURE;
	}



	


			CM_CBK_DEBUG_PRINT("cntlr_role_config:%s",cntlr_role_config);
	if ( cntlr_role_config_b == TRUE )
	{


		if(!strcmp(cntlr_role_config, "OFPCR_ROLE_NOCHANGE"))
		{
			CM_CBK_DEBUG_PRINT("OFPCR_ROLE_NOCHANGE");
			controller_role = OFPCR_ROLE_NOCHANGE;
		}
		else if(!strcmp(cntlr_role_config, "OFPCR_ROLE_EQUAL"))
		{
			CM_CBK_DEBUG_PRINT("OFPCR_ROLE_EQUAL");
			controller_role = OFPCR_ROLE_EQUAL;
		}
		else if(!strcmp(cntlr_role_config,"OFPCR_ROLE_MASTER"))
		{
			CM_CBK_DEBUG_PRINT("OFPCR_ROLE_MASTER");
			controller_role = OFPCR_ROLE_MASTER;
		}
		else if (!strcmp(cntlr_role_config,"OFPCR_ROLE_SLAVE"))
		{
			CM_CBK_DEBUG_PRINT("OFPCR_ROLE_SLAVE");
			controller_role = OFPCR_ROLE_SLAVE;
		}
		else
		{
			CM_CBK_DEBUG_PRINT("not valid");
			return OF_FAILURE;
		}
		cntlr_role_config_b=FALSE;
		cntlr_send_role_req_request(datapath_handle, controller_role);

		if(retval != CNTLR_SUCCESS)
		{
			OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Send Role Request  message fail, error=%d \r\n",
					__FUNCTION__,__LINE__,retval);
			return OF_FAILURE;
		}


	}
	return OF_SUCCESS;


}
int32_t of_cntlrrole_ucm_setmandparams (struct cm_array_of_iv_pairs *mand_iv_pairs_p,
		struct dprm_datapath_general_info *datapath_info, struct cm_app_result ** result_pp)

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
				CM_CBK_DEBUG_PRINT("dpid is: %llx", idpId);
				datapath_info->dpid=idpId;
				break;

		}
	}
	CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs_p);
	return OF_SUCCESS;
}

int32_t of_cntlrrole_ucm_setoptparams (struct cm_array_of_iv_pairs *opt_iv_pairs_p,
    char *cntlr_role_config,  struct cm_app_result ** result_pp)
{
  uint32_t param_count;
  CM_CBK_DEBUG_PRINT ("Entered");


  for (param_count = 0; param_count < opt_iv_pairs_p->count_ui; param_count++)
  {
  CM_CBK_DEBUG_PRINT("opt_iv_pairs_p->count_ui:%d,opt_iv_pairs_p->iv_pairs[param_count].id_ui:%d\n",opt_iv_pairs_p->count_ui,opt_iv_pairs_p->iv_pairs[param_count].id_ui);
    switch (opt_iv_pairs_p->iv_pairs[param_count].id_ui)
    {
	    case CM_DM_ONDIRECTORROLE_ONDIRECTORROLE_ID:       
		    cntlr_role_config_b = TRUE;
		    of_strncpy(cntlr_role_config, (char *) opt_iv_pairs_p->iv_pairs[param_count].value_p,
				    opt_iv_pairs_p->iv_pairs[param_count].value_length);
		    CM_CBK_DEBUG_PRINT("ondirector_role_config:%s\n",cntlr_role_config);
		    break;


	    default:
		    break;
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (opt_iv_pairs_p);
  return OF_SUCCESS;

}


int32_t of_cntlrrole_verifycfg (struct cm_array_of_iv_pairs * mand_iv_pairs_p,
		uint32_t command_id, struct cm_app_result ** result_p)
{
	CM_CBK_DEBUG_PRINT ("Entered");
	return OF_SUCCESS;
}

int32_t of_cntlrrole_getexactrec (struct cm_array_of_iv_pairs * pKeysArr,
		struct cm_array_of_iv_pairs ** pIvPairArr)
{
	struct   dprm_datapath_general_info datapath_info={};
	uint64_t datapath_handle;
	int32_t  iRetVal;  
	struct   cm_array_of_iv_pairs *result_iv_pairs_p=NULL;
	uint32_t rec_index=0;
        uint8_t  controller_role;
        struct dprm_datapath_entry   *datapath=NULL;




	CM_CBK_DEBUG_PRINT ("Entered");

	if ((of_cntlrrole_ucm_setmandparams (pKeysArr, &datapath_info, NULL)) !=
			OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		return OF_FAILURE;
	}


	iRetVal = dprm_get_datapath_handle(datapath_info.dpid, &datapath_handle);
	if (iRetVal != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("dprm_get_datapath_handle");
		return OF_FAILURE;
	}
	iRetVal = dprm_get_datapath_controller_role(datapath_handle, &controller_role);
        if(iRetVal  != DPRM_SUCCESS)
        {
           CM_CBK_DEBUG_PRINT("Error in getting datapath  ONDirector role");
           return OF_FAILURE;
        }

	CM_CBK_DEBUG_PRINT("ONDirector_role:%d\n",controller_role);
	result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
	if (result_iv_pairs_p == NULL)
	{
		cntrlrucm_debugmsg("memory allocation failed");
		return OF_FAILURE;
	}

	of_cntlrrole_getparams(controller_role, result_iv_pairs_p);
        *pIvPairArr = result_iv_pairs_p;
	return OF_SUCCESS;
}


int32_t of_cntlrrole_getparams (uint8_t   controller_role,
		struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
	uint32_t uiIndex = 0;
	char buf[64] = "";



	CM_CBK_DEBUG_PRINT ("Entered");
#define CM_CNTLR_ROLE_CHILD_COUNT 1



	result_iv_pairs_p->iv_pairs = (struct cm_iv_pair *) of_calloc (CM_CNTLR_ROLE_CHILD_COUNT, sizeof (struct cm_iv_pair));
	if (result_iv_pairs_p->iv_pairs == NULL)
	{
		CM_CBK_DEBUG_PRINT("Memory allocation failed for result_iv_pairs_p->iv_pairs");
		return OF_FAILURE;
	}

	switch(controller_role)
	{
		case  OFPCR_ROLE_NOCHANGE:
			{
				strcpy(buf, "OFPCR_ROLE_NOCHANGE");
			}
			break;
		case OFPCR_ROLE_EQUAL:
			{
				strcpy(buf, "OFPCR_ROLE_EQUAL");
			}
			break;
		case  OFPCR_ROLE_MASTER:
			{
				strcpy(buf, "OFPCR_ROLE_MASTER");
			}
			break;
		case OFPCR_ROLE_SLAVE:
			{
				strcpy(buf, "OFPCR_ROLE_SLAVE");
			}
			break;
		default:
			{
				strcpy(buf, "None");

			}
	}
	CM_CBK_DEBUG_PRINT ("buf:%s",buf);
	FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uiIndex], CM_DM_ONDIRECTORROLE_ONDIRECTORROLE_ID,
			CM_DATA_TYPE_STRING, buf);
	uiIndex++;



	result_iv_pairs_p->count_ui = uiIndex;
	CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);
	return OF_SUCCESS;
}


#endif
