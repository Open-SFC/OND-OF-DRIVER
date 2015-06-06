
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
 * File name: dprmtblcbk.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: 
 * 
 */

#ifdef OF_CM_SUPPORT


#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"


int32_t dprm_datapath_event_appl_ucmcbk_init (void);
int32_t dprm_datapath_event_modrec (void * pConfigTransaction,
		struct cm_array_of_iv_pairs * pMandParams,
		struct cm_array_of_iv_pairs * pOptParams,
		struct cm_app_result ** pResult);

int32_t dprm_datapath_event_verifycfg (struct cm_array_of_iv_pairs * pKeysArr,
                        uint32_t uiCommand, struct cm_app_result ** pResult);



struct cm_dm_app_callbacks dprm_datapath_event_ucm_callbacks = {
	 NULL,
	 NULL,
	 dprm_datapath_event_modrec,
	 NULL,
	 NULL,
	 NULL,
	 NULL,
	 NULL,
	 dprm_datapath_event_verifycfg,
	 NULL
};
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * Switch  UCM Init * * * * * * *  * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t of_event_appl_ucmcbk_init (void)
{
	 CM_CBK_DEBUG_PRINT ("Entry");
	 cm_register_app_callbacks(CM_ON_DIRECTOR_DATAPATH_EVENTS_APPL_ID, &dprm_datapath_event_ucm_callbacks);
	 return OF_SUCCESS;
}

int32_t dprm_datapath_event_modrec (void * pConfigTransaction,
		struct cm_array_of_iv_pairs * pMandParams,
		struct cm_array_of_iv_pairs * pOptParams,
		struct cm_app_result ** pResult)
{
	uint64_t datapath_handle;
        struct cm_app_result *port_result = NULL;
        uint32_t iRetVal=OF_FAILURE;

	struct dprm_datapath_general_info datapath_info={};
	int32_t trigger_event;

	if ((dprm_datapath_event_ucm_setmandparams (pMandParams, &datapath_info, NULL)) !=
			OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		return OF_FAILURE;
	}

	if((dprm_datapath_event_ucm_setoptparams(pOptParams,&trigger_event, NULL))!=OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
		return OF_FAILURE;
	}


	iRetVal=dprm_get_datapath_handle(datapath_info.dpid, &datapath_handle);
	if (iRetVal != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Error: datapath does not exist with id %llx",datapath_info.dpid);
		return OF_FAILURE;
	}

	if (trigger_event == TRUE)
	{ 
		CM_CBK_DEBUG_PRINT ("sending event");
		cntlr_send_port_desc_request(datapath_handle);
		return OF_SUCCESS;
	}
	else if(trigger_event == FALSE)
	{
		
	}
	return OF_SUCCESS;
}
	



int32_t dprm_datapath_event_ucm_setmandparams (struct cm_array_of_iv_pairs *
		pMandParams,
		struct dprm_datapath_general_info *datapath_info,
		int32_t *trigger_event,    struct cm_app_result ** ppResult)
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
int32_t dprm_datapath_event_ucm_setoptparams (struct cm_array_of_iv_pairs *
		pOptsParams, int32_t *trigger_event,    struct cm_app_result ** ppResult)
{
  uint32_t uiOptsParamCnt;
  CM_CBK_DEBUG_PRINT ("Entered");

  for (uiOptsParamCnt = 0; uiOptsParamCnt < pOptsParams->count_ui;
		uiOptsParamCnt++)
  {
	switch (pOptsParams->iv_pairs[uiOptsParamCnt].id_ui)
        {
	    case CM_DM_EVENTS_TRIGGERPORT_ID:
		*trigger_event = of_atoi((char *) pOptsParams->iv_pairs[uiOptsParamCnt].value_p);
		break;
	}
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (pOptsParams);
  return OF_SUCCESS;
}

int32_t dprm_datapath_event_verifycfg (struct cm_array_of_iv_pairs * pMandParams,
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



#endif /* OF_CM_SUPPORT */
