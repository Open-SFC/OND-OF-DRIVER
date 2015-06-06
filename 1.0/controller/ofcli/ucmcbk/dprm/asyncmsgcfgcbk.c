
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
 * File name: asyncmsgcfgcbk.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: 
 * 
 */

#ifdef OF_CM_SUPPORT


#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"


int32_t dprm_datapath_asyncmsgcfg_appl_ucmcbk_init (void);
int32_t dprm_datapath_asyncmsgcfg_modrec (void * pConfigTransaction,
		struct cm_array_of_iv_pairs * pMandParams,
		struct cm_array_of_iv_pairs * pOptParams,
		struct cm_app_result ** pResult);

int32_t dprm_datapath_asyncmsgcfg_verifycfg (struct cm_array_of_iv_pairs * pKeysArr,
                        uint32_t uiCommand, struct cm_app_result ** pResult);


int32_t dprm_datapath_asyncmsgcfg_ucm_getparms (struct ofp_async_config   *of_async_config_msg,
    struct cm_array_of_iv_pairs * result_iv_pairs_p);

#if 0

int32_t dprm_datapath_get_first_asyncmsgcfg(struct cm_array_of_iv_pairs * key_iv_pairs_p,
    uint32_t * count_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp);

int32_t dprm_datapath_get_next_asyncmsgcfg (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs *pPrevRecordKey, uint32_t * count_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp);
#endif

int32_t dprm_datapath_get_exact_asyncmsgcfg (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp);

struct cm_dm_app_callbacks dprm_datapath_asyncmsgcfg_ucm_callbacks = {
	 NULL,
	 NULL,
	 dprm_datapath_asyncmsgcfg_modrec,
	 NULL,
	 NULL,
	 NULL,
	 NULL,
	 dprm_datapath_get_exact_asyncmsgcfg,
	 dprm_datapath_asyncmsgcfg_verifycfg,
	 NULL
};
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * Switch  UCM Init * * * * * * *  * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t of_asyncmsgcfg_appl_ucmcbk_init (void)
{
	 CM_CBK_DEBUG_PRINT ("Entry");
	 cm_register_app_callbacks(CM_ON_DIRECTOR_DATAPATH_ASYNCMSGCFG_APPL_ID, &dprm_datapath_asyncmsgcfg_ucm_callbacks);
	 return OF_SUCCESS;
}

int32_t dprm_datapath_asyncmsgcfg_modrec (void * pConfigTransaction,
		struct cm_array_of_iv_pairs * pMandParams,
		struct cm_array_of_iv_pairs * pOptParams,
		struct cm_app_result ** result_p)
{
	uint64_t datapath_handle;
        uint32_t iRetVal=OF_FAILURE;
        uint8_t controller_role;
	struct   cm_app_result *async_msg_cfg_result = NULL;

	struct dprm_datapath_general_info datapath_info={};
         struct ofp_async_config     of_async_config_msg={}; 
	 struct of_msg *msg=NULL;


	if ((dprm_datapath_asyncmsgcfg_ucm_setmandparams (pMandParams, &datapath_info, NULL)) !=
			OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
	        fill_app_result_struct (&async_msg_cfg_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
                *result_p=async_msg_cfg_result;
		return OF_FAILURE;
	}



	iRetVal=dprm_get_datapath_handle(datapath_info.dpid, &datapath_handle);
	if (iRetVal != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("datapath does not exist with id %llx",datapath_info.dpid);
		fill_app_result_struct (&async_msg_cfg_result, NULL, CM_GLU_GROUP_DATAPATH_DOESNOT_EXIST);
                *result_p = async_msg_cfg_result;
		return OF_FAILURE;
	}


	iRetVal = dprm_get_datapath_controller_role(datapath_handle, &controller_role);
        if(iRetVal  != DPRM_SUCCESS)
        {
           CM_CBK_DEBUG_PRINT("Error in getting datapath ONDirector role");
	   fill_app_result_struct (&async_msg_cfg_result, NULL, CM_GLU_CONTROLLER_DOESNOT_EXIST);
           *result_p = async_msg_cfg_result;	
           return OF_FAILURE;
        }


	if((controller_role == OFPCR_ROLE_SLAVE))
	{
           CM_CBK_DEBUG_PRINT("Error: Slave cannot set the Async Message Configuration");
	   fill_app_result_struct (&async_msg_cfg_result, NULL, CM_GLU_SLAVE_CANNOT_SET);
           *result_p = async_msg_cfg_result;	
	   return OF_FAILURE;	
	}

	if((dprm_datapath_asyncmsgcfg_ucm_setoptparams(pOptParams,&of_async_config_msg, NULL))!=OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
	   	fill_app_result_struct (&async_msg_cfg_result, NULL, CM_GLU_SET_ASYNC_OPTS_PARMS_FAILED);
	       *result_p = async_msg_cfg_result;	
		return OF_FAILURE;
	}


	msg = ofu_alloc_message(OFPT_SET_ASYNC, OFU_SET_ASYNC_CONFIG_MESSAGE_LEN);
	if(msg == NULL)
	{
		CM_CBK_DEBUG_PRINT ("OF message alloc error");
	   	fill_app_result_struct (&async_msg_cfg_result, NULL, CM_GLU_MEM_ALLOC_FAIL);
	       *result_p = async_msg_cfg_result;	
		return OF_FAILURE;		
	}

	memcpy((( struct ofp_async_config *)msg->desc.start_of_data), &of_async_config_msg , sizeof(of_async_config_msg));
#if 0
	(( struct ofp_async_config *)msg->desc.start_of_data)->packet_in_mask[0]= of_async_config_msg.packet_in_mask[0];
	(( struct ofp_async_config *)msg->desc.start_of_data)->packet_in_mask[1]= of_async_config_msg.packet_in_mask[1];
	(( struct ofp_async_config *)msg->desc.start_of_data)->port_status_mask[0]= of_async_config_msg.port_status_mask[0];
	(( struct ofp_async_config *)msg->desc.start_of_data)->port_status_mask[1]= of_async_config_msg.port_status_mask[1];
	(( struct ofp_async_config *)msg->desc.start_of_data)->flow_removed_mask[0]= of_async_config_msg.flow_removed_mask[0];
	(( struct ofp_async_config *)msg->desc.start_of_data)->flow_removed_mask[1]= of_async_config_msg.flow_removed_mask[1];
#endif




	iRetVal = of_set_async_configuration(datapath_handle,
			msg,
			NULL,
			NULL);

	if(iRetVal != CNTLR_SUCCESS) 
	{
	        CM_CBK_DEBUG_PRINT ("of_set_async_configuration failed!");
	        msg->desc.free_cbk(msg);
	   	fill_app_result_struct (&async_msg_cfg_result, NULL, CM_GLU_SET_ASYNC_CFG_FAILED);
	       *result_p = async_msg_cfg_result;            
		return OF_FAILURE;
	}
	return OF_SUCCESS;
}
	



int32_t dprm_datapath_asyncmsgcfg_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,	
					  	     struct dprm_datapath_general_info *datapath_info,
					  	     struct cm_app_result ** ppResult)
{
  uint32_t uiMandParamCnt;
  uint64_t idpId;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;
		uiMandParamCnt++)
  {
    switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
    {
	case CM_DM_DATAPATH_DATAPATHID_ID:
		idpId=charTo64bitNum((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
		CM_CBK_DEBUG_PRINT("dpid is: %llx\r\n", idpId);
		datapath_info->dpid=idpId;
		break;
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
  return OF_SUCCESS;
}
int32_t dprm_datapath_asyncmsgcfg_ucm_setoptparams (struct cm_array_of_iv_pairs *pOptsParams,  
						    struct ofp_async_config *of_async_config_msg ,
						    struct cm_app_result ** ppResult)
{
  uint32_t uiOptsParamCnt;
  CM_CBK_DEBUG_PRINT ("Entered");

  CM_CBK_DEBUG_PRINT ("uiOptsParamCnt < pOptsParams->count_ui:%d\n",uiOptsParamCnt < pOptsParams->count_ui);

  for (uiOptsParamCnt = 0; uiOptsParamCnt < pOptsParams->count_ui;
		uiOptsParamCnt++)
  {
	
  	CM_CBK_DEBUG_PRINT ("id:%d\n",pOptsParams->iv_pairs[uiOptsParamCnt].id_ui);
	if(of_atoi((char *)(pOptsParams->iv_pairs[uiOptsParamCnt].value_p)) == TRUE)
	{
		switch (pOptsParams->iv_pairs[uiOptsParamCnt].id_ui)
		{


			/* Packet-In Async Config messages */
			case CM_DM_ASYNCMSGCFG_MASTER_PACKET_IN_NO_MATCH_ID:
				of_async_config_msg->packet_in_mask[0] |= 1<<OFPR_NO_MATCH;
				break;
			case CM_DM_ASYNCMSGCFG_MASTER_PACKET_IN_ACTION_ID:
				of_async_config_msg->packet_in_mask[0] |= 1<<OFPR_ACTION;
				break;
			case CM_DM_ASYNCMSGCFG_MASTER_PACKET_IN_INVALID_TTL_ID:
				of_async_config_msg->packet_in_mask[0] |= 1<<OFPR_INVALID_TTL;
				break;
			case CM_DM_ASYNCMSGCFG_SLAVE_PACKET_IN_NO_MATCH_ID:
				of_async_config_msg->packet_in_mask[1] |= 1<<OFPR_NO_MATCH;
				break;
			case CM_DM_ASYNCMSGCFG_SLAVE_PACKET_IN_ACTION_ID:
				of_async_config_msg->packet_in_mask[1] |= 1<<OFPR_ACTION;
				break;
			case CM_DM_ASYNCMSGCFG_SLAVE_PACKETIN_INVALID_TTL_ID:
				of_async_config_msg->packet_in_mask[1] |= 1<<OFPR_INVALID_TTL;
				break;

				/* Flow Removed Async Cfg Message */
			case CM_DM_ASYNCMSGCFG_MASTER_FLOW_REMOVED_IDLE_TIMEOUT_ID:
				of_async_config_msg->flow_removed_mask[0] |= 1<<OFPRR_IDLE_TIMEOUT;
				break;
			case CM_DM_ASYNCMSGCFG_MASTER_FLOW_REMOVED_HARD_TIMEOUT_ID:
				of_async_config_msg->flow_removed_mask[0] |= 1<<OFPRR_HARD_TIMEOUT;
				break;
			case CM_DM_ASYNCMSGCFG_MASTER_FLOW_REMOVED_DELETE_ID:
				of_async_config_msg->flow_removed_mask[0] |= 1<<OFPRR_DELETE;
				break;
			case CM_DM_ASYNCMSGCFG_MASTER_FLOW_REMOVED_GROUP_DELETE_ID:
				of_async_config_msg->flow_removed_mask[0] |= 1<<OFPRR_GROUP_DELETE;
				break;
			case CM_DM_ASYNCMSGCFG_SLAVE_FLOW_REMOVED_IDLE_TIMEOUT_ID:
				of_async_config_msg->flow_removed_mask[1] |= 1<<OFPRR_IDLE_TIMEOUT;
				break;
			case CM_DM_ASYNCMSGCFG_SLAVE_FLOW_REMOVED_HARD_TIMEOUT_ID:
				of_async_config_msg->flow_removed_mask[1] |= 1<<OFPRR_HARD_TIMEOUT;
				break;
			case CM_DM_ASYNCMSGCFG_SLAVE_FLOW_REMOVED_DELETE_ID:
				of_async_config_msg->flow_removed_mask[1] |= 1<<OFPRR_DELETE;
				break;
			case CM_DM_ASYNCMSGCFG_SLAVE_FLOW_REMOVED_GROUP_DELETE_ID:
				of_async_config_msg->flow_removed_mask[1] |= 1<<OFPRR_GROUP_DELETE;
				break;

				/* Port Async Config Message */
			case CM_DM_ASYNCMSGCFG_MASTER_PORT_ADD_ID:
				of_async_config_msg->port_status_mask[0] |= 1<<OFPPR_ADD;
				break;
			case CM_DM_ASYNCMSGCFG_MASTER_PORT_DEL_ID:
				of_async_config_msg->port_status_mask[0] |= 1<<OFPPR_DELETE;
				break;
			case CM_DM_ASYNCMSGCFG_MASTER_PORT_MOD_ID:
				of_async_config_msg->port_status_mask[0] |= 1<<OFPPR_MODIFY;
				break;
			case CM_DM_ASYNCMSGCFG_SLAVE_PORT_ADD_ID:
				of_async_config_msg->port_status_mask[1] |= 1<<OFPPR_ADD;
				break;
			case CM_DM_ASYNCMSGCFG_SLAVE_PORT_DEL_ID:
				of_async_config_msg->port_status_mask[1] |= 1<<OFPPR_DELETE;
				break;
			case CM_DM_ASYNCMSGCFG_SLAVE_PORT_MOD_ID:
				of_async_config_msg->port_status_mask[1] |= 1<<OFPPR_MODIFY;
				break;		

		}
	}
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (pOptsParams);
  return OF_SUCCESS;
}

int32_t dprm_datapath_asyncmsgcfg_verifycfg (struct cm_array_of_iv_pairs * pMandParams,
                        uint32_t uiCommand, struct cm_app_result ** pResult)
{
#if 0
         struct cm_app_result *dprm_table_result = NULL;
         int32_t iRetVal = OF_FAILURE;
         struct dprm_table_info dprm_table = { };


         CM_CBK_DEBUG_PRINT ("Entered");
         memset (&dprm_table, 0, sizeof (struct dprm_table_info));


         iRetVal = dprm_table_ucm_validatemandparams (pKeysArr, &dprm_table_result);
         if (iRetVal != OF_SUCCESS)
         {
                        CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
                        return OF_FAILURE;
         }

         if (dprm_table_ucm_validateoptparams (pKeysArr, &dprm_table_result)
                                 != OF_SUCCESS)
         {
                        CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
                        return OF_FAILURE;
         }

         *pResult = dprm_table_result;
#endif


         return OF_SUCCESS;
}
int32_t dprm_datapath_get_exact_asyncmsgcfg (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp)
{
	struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	int32_t return_value = OF_FAILURE;
	uint32_t no_of_records = 0;
	struct cm_app_result *async_msg_cfg_result = NULL;
	uint64_t datapath_handle;
	struct dprm_datapath_general_info datapath_info={};




	CM_CBK_DEBUG_PRINT ("Entered");
	if ((dprm_datapath_asyncmsgcfg_ucm_setmandparams (key_iv_pairs_p, &datapath_info, NULL)) !=
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



	return_value=cntlr_get_async_msg_cfg(datapath_handle);
	CM_CBK_DEBUG_PRINT ("retval %d", return_value);
	if ( return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Get Async configuration failed!");
		return OF_FAILURE;
	}

	return CM_CNTRL_ASYNC_RESPONSE;

}
int32_t dprm_datapath_asyncmsgcfg_ucm_getparms (struct ofp_async_config   *of_async_config_msg,
    struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
	char string[64];
	int32_t index = 0,  count_ui ;

#define CM_AYSNCMSGCFG_CHILD_COUNT 20
	count_ui = CM_AYSNCMSGCFG_CHILD_COUNT;

	if(of_async_config_msg == NULL)
	{
		return OF_FAILURE;
	}

	CM_CBK_DEBUG_PRINT ("Entered");
	result_iv_pairs_p->iv_pairs = (struct cm_iv_pair*)of_calloc(count_ui, sizeof(struct cm_iv_pair));
	if(!result_iv_pairs_p->iv_pairs)
	{
		CM_CBK_DEBUG_PRINT (" Memory allocation failed ");
		return OF_FAILURE;
	}


	of_memset(string, 0, sizeof(string));
        if((of_async_config_msg->packet_in_mask[0]) & (1<<OFPR_NO_MATCH))
	strcpy(string, "No matching flow (table-miss flow entry) for master enabled.");
	else
	strcpy(string, "No matching flow (table-miss flow entry) for master disabled.");	
	FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],
	 CM_DM_ASYNCMSGCFG_MASTER_PACKET_IN_NO_MATCH_ID,CM_DATA_TYPE_STRING, string);
	index++;
	of_memset(string, 0, sizeof(string));
        if((of_async_config_msg->packet_in_mask[1]) & (1<<OFPR_NO_MATCH))
	strcpy(string, "No matching flow (table-miss flow entry) for slave enabled.");
	else
	strcpy(string, "No matching flow (table-miss flow entry) for slave disabled.");	
	FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],
	 CM_DM_ASYNCMSGCFG_SLAVE_PACKET_IN_NO_MATCH_ID,CM_DATA_TYPE_STRING, string);
	index++;


	of_memset(string, 0, sizeof(string));
        if((of_async_config_msg->packet_in_mask[0]) & (1<<OFPR_ACTION))
	strcpy(string, "Action explicitly output to ONDirector for master enabled.");
	else
	strcpy(string, "Action explicitly output to ONDirector for master disabled.");	
	FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],
	 CM_DM_ASYNCMSGCFG_MASTER_PACKET_IN_ACTION_ID,CM_DATA_TYPE_STRING, string);
	index++;
	of_memset(string, 0, sizeof(string));
        if((of_async_config_msg->packet_in_mask[1]) & (1<<OFPR_ACTION))
	strcpy(string, "Action explicitly output to ONDirector for slave enabled.");
	else
	strcpy(string, "Action explicitly output to ONDirector for slave disabled.");	
	FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],
	 CM_DM_ASYNCMSGCFG_MASTER_PACKET_IN_ACTION_ID,CM_DATA_TYPE_STRING, string);
	index++;


	of_memset(string, 0, sizeof(string));
        if((of_async_config_msg->packet_in_mask[0]) & (1<<OFPR_INVALID_TTL))
	strcpy(string, "Packet has invalid TTL for master  enabled.");
	else
	strcpy(string, "Packet has invalid TTL for master  disabled.");	
	FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],
	 CM_DM_ASYNCMSGCFG_MASTER_PACKET_IN_INVALID_TTL_ID,CM_DATA_TYPE_STRING, string);
	index++;
	of_memset(string, 0, sizeof(string));
        if((of_async_config_msg->packet_in_mask[1]) & (1<<OFPR_INVALID_TTL))
	strcpy(string, "Packet has invalid TTL  for slave  enabled.");
	else
	strcpy(string, "Packet has invalid TTL for slave disabled.");	
	FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],
	 CM_DM_ASYNCMSGCFG_SLAVE_PACKETIN_INVALID_TTL_ID,CM_DATA_TYPE_STRING, string);
	index++;


	of_memset(string, 0, sizeof(string));
        if((of_async_config_msg->port_status_mask[0]) & (1<<OFPPR_ADD))
	strcpy(string, "The port was added async event enabled for master role.");
	else
	strcpy(string, "The port was added async event disabled for master role.");	
	FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],
	 CM_DM_ASYNCMSGCFG_MASTER_PORT_ADD_ID,CM_DATA_TYPE_STRING, string);
	index++;
	of_memset(string, 0, sizeof(string));
        if((of_async_config_msg->port_status_mask[1]) & (1<<OFPPR_ADD))
	strcpy(string, "The port was added async event enabled for slave role.");
	else
	strcpy(string, "The port was added async event disabled for slave role.");	
	FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],
	 CM_DM_ASYNCMSGCFG_SLAVE_PORT_ADD_ID,CM_DATA_TYPE_STRING, string);
	index++;

	of_memset(string, 0, sizeof(string));
        if((of_async_config_msg->port_status_mask[0]) & (1<<OFPPR_DELETE))
	strcpy(string, "The port was removed async event enabled for master role.");
	else
	strcpy(string, "The port was removed async event disabled for master role.");	
	FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],
	 CM_DM_ASYNCMSGCFG_MASTER_PORT_DEL_ID,CM_DATA_TYPE_STRING, string);
	index++;
	of_memset(string, 0, sizeof(string));
        if((of_async_config_msg->port_status_mask[1]) & (1<<OFPPR_DELETE))
	strcpy(string, "The port was removed async event enabled for slave role.");
	else
	strcpy(string, "The port was removed async event disabled for slave role.");	
	FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],
	 CM_DM_ASYNCMSGCFG_SLAVE_PORT_DEL_ID, CM_DATA_TYPE_STRING, string);
	index++;


	of_memset(string, 0, sizeof(string));
        if((of_async_config_msg->port_status_mask[0]) & (1<<OFPPR_MODIFY))
	strcpy(string, "The port was modify async event enabled for master role.");
	else
	strcpy(string, "The port was modify async event disabled for master role.");	
	FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],
	 CM_DM_ASYNCMSGCFG_MASTER_PORT_MOD_ID,CM_DATA_TYPE_STRING, string);
	index++;
	of_memset(string, 0, sizeof(string));
        if((of_async_config_msg->port_status_mask[1]) & (1<<OFPPR_MODIFY))
	strcpy(string, "The port was modify async event enabled for slave role.");
	else
	strcpy(string, "The port was modify async event disabled for slave role.");	
	FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],
	 CM_DM_ASYNCMSGCFG_SLAVE_PORT_MOD_ID, CM_DATA_TYPE_STRING, string);
	index++;


	of_memset(string, 0, sizeof(string));
        if((of_async_config_msg->flow_removed_mask[0]) & (1<<OFPRR_IDLE_TIMEOUT))
	strcpy(string, "Flow idle time exceeded idle_timeout async event enabled for master");
	else
	strcpy(string, "Flow idle time exceeded idle_timeout async event disabled for master.");	
	FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],
	 CM_DM_ASYNCMSGCFG_MASTER_FLOW_REMOVED_IDLE_TIMEOUT_ID,CM_DATA_TYPE_STRING, string);
	index++;
	of_memset(string, 0, sizeof(string));
        if((of_async_config_msg->flow_removed_mask[1]) & (1<<OFPRR_IDLE_TIMEOUT))
	strcpy(string, "Flow idle time exceeded idle_timeout async event enabled for slave.");
	else
	strcpy(string, "Flow idle time exceeded idle_timeout async event disabled for slave.");	
	FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],
	 CM_DM_ASYNCMSGCFG_SLAVE_FLOW_REMOVED_IDLE_TIMEOUT_ID, CM_DATA_TYPE_STRING, string);
	index++;

	of_memset(string, 0, sizeof(string));
        if((of_async_config_msg->flow_removed_mask[0]) & (1<<OFPRR_HARD_TIMEOUT))
	strcpy(string, "Time exceeded hard_timeout async event enabled for master");
	else
	strcpy(string, "Time exceeded hard_timeout async event disabled for master.");	
	FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],
	 CM_DM_ASYNCMSGCFG_MASTER_FLOW_REMOVED_HARD_TIMEOUT_ID,CM_DATA_TYPE_STRING, string);
	index++;
	of_memset(string, 0, sizeof(string));
        if((of_async_config_msg->flow_removed_mask[1]) & (1<<OFPRR_HARD_TIMEOUT))
	strcpy(string, "Time exceeded hard_timeout async event enabled for slave.");
	else
	strcpy(string, "Time exceeded hard_timeout async event disabled for slave.");	
	FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],
	 CM_DM_ASYNCMSGCFG_SLAVE_FLOW_REMOVED_HARD_TIMEOUT_ID, CM_DATA_TYPE_STRING, string);
	index++;

	of_memset(string, 0, sizeof(string));
        if((of_async_config_msg->flow_removed_mask[0]) & (1<<OFPRR_DELETE))
	strcpy(string, "DELETE flow mod async event enabled for master");
	else
	strcpy(string, "DELETE flow mod async event disabled for master.");	
	FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],
	 CM_DM_ASYNCMSGCFG_MASTER_FLOW_REMOVED_DELETE_ID,CM_DATA_TYPE_STRING, string);
	index++;
	of_memset(string, 0, sizeof(string));
        if((of_async_config_msg->flow_removed_mask[1]) & (1<<OFPRR_DELETE))
	strcpy(string, "DELETE flow mod async event enabled  for slave.");
	else
	strcpy(string, "DELETE flow mod async event disabled for slave.");	
	FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],
	 CM_DM_ASYNCMSGCFG_SLAVE_FLOW_REMOVED_DELETE_ID, CM_DATA_TYPE_STRING, string);
	index++;


	of_memset(string, 0, sizeof(string));
        if((of_async_config_msg->flow_removed_mask[0]) & (1<<OFPRR_GROUP_DELETE))
	strcpy(string, "Group was removed async event enabled for master");
	else
	strcpy(string, "Group was removed async event disabled for master.");	
	FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],
	 CM_DM_ASYNCMSGCFG_MASTER_FLOW_REMOVED_GROUP_DELETE_ID, CM_DATA_TYPE_STRING, string);
	index++;
	of_memset(string, 0, sizeof(string));
        if((of_async_config_msg->flow_removed_mask[1]) & (1<<OFPRR_DELETE))
	strcpy(string, "Group was removed async event enabled  for slave.");
	else
	strcpy(string, "Group was removed async event disabled for slave.");	
	FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],
	 CM_DM_ASYNCMSGCFG_MASTER_FLOW_REMOVED_GROUP_DELETE_ID, CM_DATA_TYPE_STRING, string);
	index++;

	result_iv_pairs_p->count_ui = index;

	CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);
	return OF_SUCCESS;
}

#if 0
int32_t dprm_datapath_get_next_asyncmsgcfg (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs *pPrevRecordKey, uint32_t * count_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp)
{
  int32_t return_value = OF_FAILURE;

  CM_CBK_DEBUG_PRINT ("Entered");
  *count_p=0;
  return return_value;
}

int32_t dprm_datapath_get_first_asyncmsgcfg (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp)
{
  int32_t return_value = OF_FAILURE;

  CM_CBK_DEBUG_PRINT ("Entered");
  return return_value;
}
#endif



#endif /* OF_CM_SUPPORT */
