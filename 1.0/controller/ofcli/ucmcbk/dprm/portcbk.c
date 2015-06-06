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
 * File name: portcbk.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: 
 * 
 */

#ifdef OF_CM_SUPPORT


#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"


int32_t dprm_datapath_port_appl_ucmcbk_init (void);

int32_t dprm_datapath_port_addrec (void * config_trans_p,
			struct cm_array_of_iv_pairs * pMandParams,
			struct cm_array_of_iv_pairs * pOptParams,
			struct cm_app_result ** result_p);

int32_t dprm_datapath_port_delrec (void * config_transaction_p,
			struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_app_result ** result_p);

int32_t dprm_datapath_port_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
			uint32_t * count_p,
			struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);

int32_t dprm_datapath_port_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_array_of_iv_pairs * prev_record_key_p, uint32_t * count_p,
			struct cm_array_of_iv_pairs ** next_n_record_data_p);

int32_t dprm_datapath_port_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t dprm_datapath_port_verifycfg (struct cm_array_of_iv_pairs * keys_arr_p,
			uint32_t command_id, struct cm_app_result ** result_p);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */
 int32_t dprm_datapath_port_ucm_validatemandparams (struct cm_array_of_iv_pairs *
			pMandParams,
			struct cm_app_result **presult_p);

 int32_t dprm_datapath_port_ucm_validateoptparams (struct cm_array_of_iv_pairs *
			pOptParams,
			struct cm_app_result ** presult_p);


 int32_t dprm_datapath_port_ucm_setmandparams (struct cm_array_of_iv_pairs *
			pMandParams,
			struct dprm_datapath_general_info *datapath_info,
			struct dprm_port_info* port_info,
			struct cm_app_result ** presult_p);

 int32_t dprm_datapath_port_ucm_setoptparams (struct cm_array_of_iv_pairs *
			pOptParams,
                         struct ofp_port_mod * port_info,
			struct cm_app_result ** presult_p);

 int32_t dprm_datapath_port_ucm_getparams (struct dprm_port_info *port_info,
			struct cm_array_of_iv_pairs * result_iv_pairs_p);

int32_t port_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
    struct dprm_datapath_general_info *datapath_info,
    struct ofp_port_mod* port_info, struct cm_app_result ** presult_p);

struct cm_dm_app_callbacks dprm_datapath_port_ucm_callbacks = {
	 NULL,
	 NULL,//dprm_datapath_port_addrec,
	 dprm_datapath_port_addrec,
	 NULL,//dprm_datapath_port_delrec,
	 NULL,
	 dprm_datapath_port_getfirstnrecs,
	 dprm_datapath_port_getnextnrecs,
	 dprm_datapath_port_getexactrec,
	 dprm_datapath_port_verifycfg,
	 NULL
};

char *ofp_port_config[]={"Port is administratively down.",
			 "none",
			 "Drop all packets received by port.",
			 "none",
			 "none",
			 "Drop packets forwarded to port.",
			 "Do not send packet-in msgs for port."		
			};

char *ofp_port_state[]=	{"No physical link present.",
			 "Port is blocked.",
			 "Live for Fast Failover Group."			
			};

char *fp_port_features[] ={"10 Mb half-duplex rate support.",
			   "10 Mb full-duplex rate support.",
		           "100 Mb half-duplex rate support.",
		           "100 Mb full-duplex rate support.",
			   "1 Gb half-duplex rate support.",
			   "1 Gb full-duplex rate support.",
			   "10 Gb full-duplex rate support.",
			   "40 Gb full-duplex rate support.",
		           "100 Gb full-duplex rate support.",
		           "1 Tb full-duplex rate support.",
	                   "Other rate, not in the list."
		           "Copper medium.",
		           "Fiber medium.",
		           "Auto-negotiation." ,
		           "Pause.",
		           "Asymmetric pause."
		          };


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * Switch  UCM Init * * * * * * *  * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t dprm_datapath_port_appl_ucmcbk_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  cm_register_app_callbacks (CM_ON_DIRECTOR_DATAPATH_PORT_APPL_ID,&dprm_datapath_port_ucm_callbacks);
  return OF_SUCCESS;
}

int32_t dprm_datapath_port_addrec (void * config_trans_p,
			struct cm_array_of_iv_pairs * pMandParams,
			struct cm_array_of_iv_pairs * pOptParams,
			struct cm_app_result ** result_p)
{
  struct cm_app_result *port_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct ofp_port_mod port_info = { };
  struct dprm_datapath_general_info datapath_info={};
  uint64_t datapath_handle;
  uint64_t port_handle;

  CM_CBK_DEBUG_PRINT ("Entered");

  of_memset (&port_info, 0, sizeof (struct dprm_port_info));

  if ((port_ucm_setmandparams (pMandParams, &datapath_info, &port_info, &port_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&port_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=port_result;
    return OF_FAILURE;
  }

  return_value = dprm_datapath_port_ucm_setoptparams (pOptParams, &port_info, &port_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
    fill_app_result_struct (&port_result, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
    *result_p = port_result;
    return OF_FAILURE;
  }

  return_value=dprm_get_datapath_handle(datapath_info.dpid, &datapath_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Datapath does not exist with id %llx",datapath_info.dpid);
    fill_app_result_struct (&port_result, NULL, CM_GLU_GROUP_DATAPATH_DOESNOT_EXIST);
    *result_p = port_result;
    return OF_FAILURE;
  }

  return_value = cntlr_send_portmod_request(datapath_handle, &port_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Datapath port Addition Failed");
    fill_app_result_struct (&port_result, NULL, CM_GLU_PORT_ADD_FAILED);
    *result_p = port_result;
    return OF_FAILURE;
  }

  CM_CBK_DEBUG_PRINT ("Datapath port added succesfully");
  return OF_SUCCESS;
}

int32_t dprm_datapath_port_delrec (void * config_transaction_p,
    struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_app_result ** result_p)
{
  struct cm_app_result *port_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct dprm_port_info port_info = { };
  struct dprm_datapath_general_info datapath_info={};
  uint64_t datapath_handle;
  uint64_t port_handle;

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&port_info, 0, sizeof (struct dprm_port_info));

  if ((dprm_datapath_port_ucm_setmandparams (keys_arr_p,&datapath_info, &port_info, &port_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&port_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p = port_result;
    return OF_FAILURE;
  }

  return_value=dprm_get_datapath_handle(datapath_info.dpid, &datapath_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Get DPRM Handle failed");
    fill_app_result_struct (&port_result, NULL, CM_GLU_NO_MATCHING_PORT_RECORD);
    *result_p = port_result;
    return OF_FAILURE;
  }

  return_value=dprm_get_port_handle(datapath_handle,port_info.port_name, &port_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: get dprm port handle failed for %s", port_info.port_name);
    fill_app_result_struct (&port_result, NULL, CM_GLU_PORT_DELETE_FAILED);
    *result_p = port_result;
    return OF_FAILURE;
  }

  return_value=dprm_delete_port_from_datapath(datapath_handle, port_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Delete port record failed");
    fill_app_result_struct (&port_result, NULL, CM_GLU_PORT_DELETE_FAILED);
    *result_p = port_result;
    return OF_FAILURE;
  }

  CM_CBK_DEBUG_PRINT ("Datapath port deleted succesfully");
  return OF_SUCCESS;
}

int32_t dprm_datapath_port_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
    uint32_t * count_p,
    struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  struct cm_app_result *port_result = NULL;
  struct dprm_datapath_general_info datapath_info={};
  struct dprm_port_info port_info;
  struct dprm_port_runtime_info runtime_info;
  uint64_t datapath_handle;
  uint64_t port_handle;

  of_memset (&port_info, 0, sizeof (struct dprm_port_info));

  if ((dprm_datapath_port_ucm_setmandparams (keys_arr_p,&datapath_info, &port_info, &port_result)) !=
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

  return_value=dprm_get_first_datapath_port(datapath_handle, &port_info, &runtime_info, &port_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Get first record failed for DPRM Table");
    return OF_FAILURE;
  }

  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    return OF_FAILURE;
  }

  dprm_datapath_port_ucm_getparams (&port_info, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *array_of_iv_pair_arr_p = result_iv_pairs_p;
  *count_p = uiRecCount;
  return OF_SUCCESS;
}


int32_t dprm_datapath_port_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_array_of_iv_pairs *prev_record_key_p, uint32_t * count_p,
    struct cm_array_of_iv_pairs ** next_n_record_data_p)
{

  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct cm_app_result *port_result = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  struct dprm_datapath_general_info datapath_info={};
  struct dprm_port_info port_info={};
  struct dprm_port_runtime_info runtime_info;
  uint64_t datapath_handle;
  uint64_t port_handle;

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&port_info, 0, sizeof (struct dprm_port_info));

  if ((dprm_datapath_port_ucm_setmandparams (keys_arr_p,&datapath_info, &port_info, &port_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  if ((dprm_datapath_port_ucm_setmandparams (prev_record_key_p,&datapath_info, &port_info, &port_result)) !=
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

  return_value=dprm_get_port_handle(datapath_handle,port_info.port_name, &port_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: get dprm port handle failed for %s", port_info.port_name);
    return OF_FAILURE;
  }

  return_value=dprm_get_next_datapath_port( datapath_handle, &port_info, &runtime_info, &port_handle);
  if (return_value == OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Next port rec is: %s ",	port_info.port_name);
    result_iv_pairs_p =
      (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
    if (result_iv_pairs_p == NULL)
    {
      CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
      return OF_FAILURE;
    }
    dprm_datapath_port_ucm_getparams (&port_info, &result_iv_pairs_p[uiRecCount]);
    uiRecCount++;
  }
  else
    return OF_FAILURE;

  CM_CBK_DEBUG_PRINT ("Number of records requested were %ld ", *count_p);
  CM_CBK_DEBUG_PRINT ("Number of records found are %ld", uiRecCount+1);
  *next_n_record_data_p = result_iv_pairs_p;
  *count_p = uiRecCount;
  return OF_SUCCESS;
}

int32_t dprm_datapath_port_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_array_of_iv_pairs ** pIvPairArr)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct cm_app_result *port_result = NULL;
  uint32_t ii = 0;
  int32_t return_value = OF_FAILURE;
  struct dprm_datapath_general_info datapath_info={};
  struct dprm_port_info port_info={};
  struct dprm_port_runtime_info runtime_info;
  uint64_t datapath_handle;
  uint64_t port_handle;

  CM_CBK_DEBUG_PRINT ("Entered");

  if ((dprm_datapath_port_ucm_setmandparams (keys_arr_p, &datapath_info, &port_info, &port_result)) !=
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

  return_value=dprm_get_port_handle(datapath_handle, port_info.port_name, &port_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: get dprm port handle failed for %s", port_info.port_name);
    return OF_FAILURE;
  }

  of_memset (&port_info, 0, sizeof (struct dprm_port_info));
  return_value=dprm_get_exact_datapath_port(datapath_handle, &port_info,&runtime_info, port_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: get dprm table handle failed");
    return OF_FAILURE;
  }

  CM_CBK_DEBUG_PRINT ("Exact matching record found");
  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Failed to allocate memory for result_iv_pairs_p");
    return OF_FAILURE;
  }
  dprm_datapath_port_ucm_getparams (&port_info, result_iv_pairs_p);
  *pIvPairArr = result_iv_pairs_p;
  return return_value;
}

int32_t dprm_datapath_port_verifycfg (struct cm_array_of_iv_pairs * keys_arr_p,
    uint32_t command_id, struct cm_app_result ** result_p)
{
  struct cm_app_result *port_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct dprm_port_info port_info = { };

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&port_info, 0, sizeof (struct dprm_port_info));

  return_value = dprm_datapath_port_ucm_validatemandparams (keys_arr_p, &port_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  if (dprm_datapath_port_ucm_validateoptparams (keys_arr_p, &port_result)
      != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Optional Parameters Failed");
    return OF_FAILURE;
  }

  *result_p = port_result;
  return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */

int32_t dprm_datapath_port_ucm_validatemandparams (struct cm_array_of_iv_pairs *
    pMandParams,	struct cm_app_result **presult_p)
{
  uint32_t uiMandParamCnt;
  struct cm_app_result *port_result = NULL;
  uint32_t uiPortId;


  CM_CBK_DEBUG_PRINT ("Entered");
  for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;
      uiMandParamCnt++)
  {
    switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
    {
      case CM_DM_PORT_PORTNAME_ID:
	if (pMandParams->iv_pairs[uiMandParamCnt].value_p == NULL)
	{
	  CM_CBK_DEBUG_PRINT ("Port Name is NULL");
	  fill_app_result_struct (&port_result, NULL, CM_GLU_PORT_NAME_NULL);
	  *presult_p = port_result;
	  return OF_FAILURE;
	}
	break;

      case CM_DM_PORT_PORTID_ID: 
	if (pMandParams->iv_pairs[uiMandParamCnt].value_p == NULL)
	{
	  CM_CBK_DEBUG_PRINT ("Port ID is NULL");
	  fill_app_result_struct (&port_result, NULL, CM_GLU_PORT_ID_NULL);
	  *presult_p = port_result;
	  return OF_FAILURE;
	}
#if 0
	uiPortId =
	  of_atoi ((char *) (pMandParams->iv_pairs[uiMandParamCnt].value_p));
	if (uiPortId < 0 ||  uiPortId > 24)
	{
	  CM_CBK_DEBUG_PRINT ("Invalid Port ID entered");
	  fill_app_result_struct (&port_result, NULL, CM_GLU_PORT_ID_INVALID);
	  *presult_p = port_result;
	  return OF_FAILURE;
	}
#endif
	break;
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
  return OF_SUCCESS;
}

int32_t dprm_datapath_port_ucm_validateoptparams (struct cm_array_of_iv_pairs *
    pOptParams,
    struct cm_app_result ** presult_p)
{
  uint32_t uiOptParamCnt, uiTableCnt;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (uiOptParamCnt = 0; uiOptParamCnt < pOptParams->count_ui; uiOptParamCnt++)
  {
    switch (pOptParams->iv_pairs[uiOptParamCnt].id_ui)
    {
      default:
	break;
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
  return OF_SUCCESS;
}

int32_t port_ucm_setmandparams (struct cm_array_of_iv_pairs *
    pMandParams,
    struct dprm_datapath_general_info *datapath_info,
    struct ofp_port_mod* port_info,		struct cm_app_result ** presult_p)
{
  uint32_t uiMandParamCnt;
  uint64_t idpId;
  int32_t iportId;

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

      /*case CM_DM_PORT_PORTNAME_ID:
	of_strncpy (port_info->port_name,
	    (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
	    pMandParams->iv_pairs[uiMandParamCnt].value_length);
	CM_CBK_DEBUG_PRINT ("port name %s",port_info->port_name);
	break;*/

      case CM_DM_PORT_PORTID_ID:
	iportId=of_atoi((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
	port_info->port_no=iportId;
	break;
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
  return OF_SUCCESS;
}

int32_t dprm_datapath_port_ucm_setmandparams (struct cm_array_of_iv_pairs *
    pMandParams,
    struct dprm_datapath_general_info *datapath_info,
    struct dprm_port_info* port_info,		struct cm_app_result ** presult_p)
{
  uint32_t uiMandParamCnt;
  uint64_t idpId;
  int32_t iportId;

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

      case CM_DM_PORT_PORTNAME_ID:
	of_strncpy (port_info->port_name,
	    (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
	    pMandParams->iv_pairs[uiMandParamCnt].value_length);
	CM_CBK_DEBUG_PRINT ("port name %s",port_info->port_name);
	break;

      case CM_DM_PORT_PORTID_ID:
	iportId=of_atoi((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
	port_info->port_id=iportId;
	break;
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
  return OF_SUCCESS;
}

int32_t dprm_datapath_port_ucm_setoptparams (struct cm_array_of_iv_pairs *
    pOptParams,
    struct ofp_port_mod * port_info,
    struct cm_app_result ** presult_p)
{
  uint32_t uiOptParamCnt,ii=0;
  int32_t iportId;
  char *token,curr_feature[200];

  CM_CBK_DEBUG_PRINT ("Entered");

  for (uiOptParamCnt = 0; uiOptParamCnt < pOptParams->count_ui; uiOptParamCnt++)
  {
    switch (pOptParams->iv_pairs[uiOptParamCnt].id_ui)
    {
      /*case CM_DM_PORT_CONFIG_ID:
	iportId=0;
	if (strcmp((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p,"down") == 0)
	{
	  iportId=OFPPC_PORT_DOWN;
	} 
	else if (strcmp((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p,"no_recv") == 0)
	{
	  iportId=OFPPC_NO_RECV;
	}
	else if (strcmp((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p,"no_fwd") == 0)
	{
	  iportId=OFPPC_NO_FWD;
	}
	else if (strcmp((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p,"no_packet_in") == 0)
	{
	  iportId=OFPPC_NO_PACKET_IN;
	}
	port_info->port_desc.config=iportId;
	break;*/

      case CM_DM_PORT_PORT_DOWN_ID:
        if (of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p) == TRUE)
          port_info->config |= OFPPC_PORT_DOWN;
        else if (of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p) == FALSE)
          port_info->config &= ~OFPPC_PORT_DOWN;

          port_info->mask |= OFPPC_PORT_DOWN;
	break;

      case CM_DM_PORT_PORT_NO_RECV_ID:
        if (of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p) == TRUE)
          port_info->config |= OFPPC_NO_RECV;
        else if (of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p) == FALSE)
          port_info->config &= ~OFPPC_NO_RECV;

          port_info->mask |= OFPPC_NO_RECV;
	break;

      case CM_DM_PORT_PORT_NO_FWD_ID:
        if (of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p) == TRUE)
          port_info->config |= OFPPC_NO_FWD;
        else if (of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p) == FALSE)
          port_info->config &= ~OFPPC_NO_FWD;

          port_info->mask |= OFPPC_NO_FWD;
	break;

      case CM_DM_PORT_PORT_NO_PACKET_IN_ID:
        if (of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p) == TRUE)
          port_info->config |= OFPPC_NO_PACKET_IN;
        else if (of_atoi(pOptParams->iv_pairs[uiOptParamCnt].value_p) == FALSE)
          port_info->config &= ~OFPPC_NO_PACKET_IN;

          port_info->mask |= OFPPC_NO_PACKET_IN;
	break;

      case CM_DM_PORT_ADVERTISED_ID:
	iportId=0;
	iportId=of_atoi((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p);
	port_info->advertise=iportId;
	break;
#if 0
      case CM_DM_PORT_STATE_ID:
	iportId=0;
	if (strcmp((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p,"down") == 0)
	{
	  iportId=OFPPS_LINK_DOWN;
	} 
	else if (strcmp((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p,"blocked") == 0)
	{
	  iportId=OFPPS_BLOCKED;
	}
	else if (strcmp((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p,"live") == 0)
	{
	  iportId=OFPPS_LIVE;
	}
	port_info->port_desc.state=iportId;
	break;
      case CM_DM_PORT_CURR_ID:

	CM_CBK_DEBUG_PRINT ("CM_DM_PORT_CURR_ID(%s)",(char *) pOptParams->iv_pairs[uiOptParamCnt].value_p);
	strcpy(curr_feature, (char *) pOptParams->iv_pairs[uiOptParamCnt].value_p);
	if(!strcmp(curr_feature, "none"))
	{
	  port_info->port_desc.curr=0;
	}
	else
	{
	  token = strtok(curr_feature, ".");
	  while(token!=NULL){
	    CM_CBK_DEBUG_PRINT("token:%s",token);
	    for(ii=0; ii<16;ii++)
	    {
	      if(!strncmp(fp_port_features[ii], token,strlen(fp_port_features[ii])-1))
	      {
		port_info->port_desc.curr  |= (1<<ii);			
		CM_CBK_DEBUG_PRINT ("port_info->port_desc.curr:%x",port_info->port_desc.curr);
		break;

	      }

	    }
	    token = strtok(NULL, ".");
	  }
	}
	break;
      case CM_DM_PORT_SUPPORTED_ID:
	iportId=0;
	iportId=of_atoi((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p);
	port_info->port_desc.supported=iportId;
	break;
      case CM_DM_PORT_PEER_ID:
	iportId=0;
	iportId=of_atoi((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p);
	port_info->port_desc.peer=iportId;
	break;
      case CM_DM_PORT_CURRSPEED_ID:
	iportId=0;
	iportId=of_atoi((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p);
	port_info->port_desc.curr_speed=iportId;
	break;
      case CM_DM_PORT_MAXSPEED_ID:
	iportId=0;
	iportId=of_atoi((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p);
	port_info->port_desc.max_speed=iportId;
	break;
#endif
      default:
	break;
    }
  }

  CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
  return OF_SUCCESS;
}



int32_t dprm_datapath_port_ucm_getparams (struct dprm_port_info *port_info,
    struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
  uint32_t index_ui = 0,ii;
  char buf[64] = "";
#define CM_PORT_CHILD_COUNT 11

  result_iv_pairs_p->iv_pairs =
    (struct cm_iv_pair *) of_calloc (CM_PORT_CHILD_COUNT, sizeof (struct cm_iv_pair));
  if (result_iv_pairs_p->iv_pairs == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p->iv_pairs");
    return OF_FAILURE;
  }

  /* subject name*/
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[index_ui], CM_DM_PORT_PORTNAME_ID,
      CM_DATA_TYPE_STRING, port_info->port_name);
  index_ui++;

  /*Id authentication*/
  of_itoa (port_info->port_id, buf);
  // sprintf(buf,"%lld",port_info->port_id);
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[index_ui],CM_DM_PORT_PORTID_ID,
      CM_DATA_TYPE_STRING, buf);
  index_ui++;

  sprintf(buf,"%02x:%02x:%02x:%02x:%02x:%02x",port_info->port_desc.hw_addr[0],port_info->port_desc.hw_addr[1],port_info->port_desc.hw_addr[2],port_info->port_desc.hw_addr[3],port_info->port_desc.hw_addr[4],port_info->port_desc.hw_addr[5]);


  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index_ui], CM_DM_PORT_HWADDR_ID, CM_DATA_TYPE_STRING, buf);
  index_ui++;



  memset(buf, 0, sizeof(buf));
  if(port_info->port_desc.config == 0)
  {
    strcpy(buf,"none");
  }
  else
  {
    for(ii=0;ii<7;ii++)
    { 
      if(port_info->port_desc.config &(1<<ii))
      {
	strcpy(buf, ofp_port_config[ii]);
	break;
      }	
    }
  }
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index_ui], CM_DM_PORT_CONFIG_ID, CM_DATA_TYPE_STRING, buf);
  index_ui++;




  memset(buf, 0, sizeof(buf));
  if(port_info->port_desc.state ==0 )
  {
    strcpy(buf,"none");
  }
  else
  {
    for(ii=0;ii<3;ii++)
    {
      if(port_info->port_desc.state &(1<<ii))
      {
	strcpy(buf, ofp_port_state[ii]);
	break;
      }			
    }
  }
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index_ui], CM_DM_PORT_STATE_ID, CM_DATA_TYPE_STRING, buf);
  index_ui++;



  memset(buf, 0, sizeof(buf));
  if(port_info->port_desc.curr == 0)
  {
    strcpy(buf,"none");
  }
  else
  {
    for (ii=0; ii < 16; ii++ )
    {
      if ( port_info->port_desc.curr & (1 << ii) )
      {
	strcat(buf, fp_port_features[ii]);	
      }
    }
  }
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index_ui], CM_DM_PORT_CURR_ID, CM_DATA_TYPE_STRING, buf);
  index_ui++;


  memset(buf, 0, sizeof(buf));
  of_itoa (port_info->port_desc.advertised, buf);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index_ui], CM_DM_PORT_ADVERTISED_ID, CM_DATA_TYPE_STRING, buf);
  index_ui++;

  memset(buf, 0, sizeof(buf));
  of_itoa (port_info->port_desc.supported, buf);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index_ui], CM_DM_PORT_SUPPORTED_ID, CM_DATA_TYPE_STRING, buf);
  index_ui++;


  memset(buf, 0, sizeof(buf));
  of_itoa (port_info->port_desc.peer, buf);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index_ui], CM_DM_PORT_PEER_ID, CM_DATA_TYPE_STRING, buf);
  index_ui++;

  memset(buf, 0, sizeof(buf));
  of_itoa (port_info->port_desc.curr_speed, buf);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index_ui], CM_DM_PORT_CURRSPEED_ID, CM_DATA_TYPE_STRING, buf);
  index_ui++;

  memset(buf, 0, sizeof(buf));
  of_itoa (port_info->port_desc.max_speed, buf);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index_ui], CM_DM_PORT_MAXSPEED_ID, CM_DATA_TYPE_STRING, buf);
  index_ui++;

  result_iv_pairs_p->count_ui = index_ui;
  return OF_SUCCESS;
}

#endif /* OF_CM_SUPPORT */
