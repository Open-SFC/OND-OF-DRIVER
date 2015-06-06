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
* File name: ipsec_pol_stats.c
* Author: G Atmaram <B38856@freescale.com>
* Date:   03/13/2013
* Description: Flow Statistics.
* 
 */

#ifdef OPENFLOW_IPSEC_SUPPORT


#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "of_utilities.h"
#include "of_multipart.h"
#include "fsl_nfapi_ipsec.h"

int32_t ipsec_policy_stats_appl_ucmcbk_init (void);

int32_t ipsec_policy_stats_addrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * pMandParams,
    struct cm_array_of_iv_pairs * pOptParams,
    struct cm_app_result ** pResult);

int32_t ipsec_policy_stats_setrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * pMandParams,
    struct cm_array_of_iv_pairs * pOptParams,
    struct cm_app_result ** pResult);

int32_t ipsec_policy_stats_delrec (void * pConfigTransction,
    struct cm_array_of_iv_pairs * pKeysArr,
    struct cm_app_result ** pResult);

int32_t ipsec_policy_stats_getfirstnrecs (struct cm_array_of_iv_pairs * pKeysArr,
    uint32_t * pCount,
    struct cm_array_of_iv_pairs ** pArrayofIvPairArr);

int32_t ipsec_policy_stats_getnextnrecs (struct cm_array_of_iv_pairs * pKeysArr,
    struct cm_array_of_iv_pairs * pPrevRecordKey, uint32_t * pCount,
    struct cm_array_of_iv_pairs ** pNextNRecordData_p);

int32_t ipsec_policy_stats_getexactrec (struct cm_array_of_iv_pairs * pKeysArr,
    struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t ipsec_policy_stats_verifycfg (struct cm_array_of_iv_pairs * pKeysArr,
    uint32_t command_id, struct cm_app_result ** pResult);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
* * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */
int32_t ipsec_policy_stats_ucm_validatemandparams (struct cm_array_of_iv_pairs *
    pMandParams,
    struct cm_app_result **ppResult);

int32_t ipsec_policy_stats_ucm_validateoptparams (struct cm_array_of_iv_pairs *
    pOptParams,
    struct cm_app_result ** ppResult);

int32_t ipsec_policy_stats_ucm_setmandparams (struct cm_array_of_iv_pairs *
    pMandParams,struct dprm_datapath_general_info *datapath_info,
    struct cm_app_result ** ppResult);

int32_t ipsec_policy_stats_ucm_setoptparams (struct cm_array_of_iv_pairs *
    pOptParams,
    struct ipsec_spd_get_inargs *in_info,
    struct ipsec_spd_get_outargs  *out_info,
    struct cm_app_result ** ppResult);


int32_t ipsec_policy_stats_ucm_getparams (struct ipsec_spd_get_inargs *in_info,
                                          struct ipsec_spd_get_outargs *out_info,
                                          struct cm_array_of_iv_pairs  *result_iv_pairs_p);

struct cm_dm_app_callbacks ipsec_policy_stats_ucm_callbacks = {
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  ipsec_policy_stats_getfirstnrecs,
  ipsec_policy_stats_getnextnrecs,
  ipsec_policy_stats_getexactrec,
  NULL,
  NULL
};
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* * * * * * * * * * * * Switch  UCM Init * * * * * * *  * * * * * * * * * * *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t ipsec_policy_stats_appl_ucmcbk_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  cm_register_app_callbacks (CM_DM_STATS_SPDSTATS_ID,&ipsec_policy_stats_ucm_callbacks);
  return OF_SUCCESS;
}



int32_t ipsec_policy_stats_getfirstnrecs (struct cm_array_of_iv_pairs * pKeysArr,
    uint32_t * pCount,
    struct cm_array_of_iv_pairs ** pArrayofIvPairArr)
{
  struct   cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t  return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  struct   dprm_datapath_general_info datapath_info = {};
  struct   cm_app_result *stats_result = NULL;
  uint64_t datapath_handle;
  struct ipsec_spd_get_inargs  in;
  struct ipsec_spd_get_outargs out;
  uint32_t nsid;

  of_memset (&in, 0, sizeof (struct ipsec_spd_get_inargs));
  of_memset (&out, 0, sizeof (struct ipsec_spd_get_outargs));

  out.spd_params = (struct ipsec_policy *) calloc (1, sizeof(struct ipsec_policy));
  if(!out.spd_params)
    return OF_FAILURE;

  if ((ipsec_policy_stats_ucm_setmandparams (pKeysArr,&datapath_info, &stats_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }
  if ((ipsec_policy_stats_ucm_setoptparams (pKeysArr,&in,&out,&stats_result)) !=
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

  in.operation = IPSEC_SPD_GET_FIRST;
  if(in.dir == 0)
  {
    in.dir = IPSEC_INBOUND;
  }
  return_value = of_nem_get_nsid_from_dpid_and_dp_ns_id(datapath_info.dpid,0,&nsid);
  //get nsid from dpid before calling this API
  return_value = ipsec_spd_get(nsid,  API_CTRL_FLAG_NO_RESP_EXPECTED,
                               &in, &out, NULL);
  if ( return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("ipsec policy stats failed");
    return OF_FAILURE;
  } 
  CM_CBK_DEBUG_PRINT ("ipsec policy stats will be sent as multipart response");
  return CM_CNTRL_MULTIPART_RESPONSE;
  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));

 if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    return OF_FAILURE;
  }
  ipsec_policy_stats_ucm_getparams (&in,&out, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *next_n_record_data_p = result_iv_pairs_p;
  *pCount = uiRecCount;
  return OF_SUCCESS;
 
  CM_CBK_DEBUG_PRINT ("ipsec policy stats will be sent as multipart response");
  return CM_CNTRL_MULTIPART_RESPONSE;
}




int32_t ipsec_policy_stats_getnextnrecs (struct cm_array_of_iv_pairs * pKeysArr,
    struct cm_array_of_iv_pairs *pPrevRecordKey, uint32_t * pCount,
    struct cm_array_of_iv_pairs ** pNextNRecordData_p)
{

  struct   cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t  return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  struct   cm_app_result *stats_result = NULL;
  struct   dprm_datapath_general_info datapath_info={};
  struct   dprm_port_runtime_info runtime_info;
  uint64_t datapath_handle;
  uint64_t port_handle;
  struct ipsec_spd_get_inargs  in;
  struct ipsec_spd_get_outargs out;
  uint32_t nsid;
  CM_CBK_DEBUG_PRINT ("Entered");
  *pCount=0;
  

  of_memset (&in, 0, sizeof (struct ipsec_spd_get_inargs));
  of_memset (&out, 0, sizeof (struct ipsec_spd_get_outargs));

  out.spd_params = (struct ipsec_policy *) calloc (1, sizeof(struct ipsec_policy));
  if(!out.spd_params)
    return OF_FAILURE;

  if ((ipsec_policy_stats_ucm_setmandparams (pPrevRecordKey,&datapath_info, &stats_result)) !=
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

  in.operation = IPSEC_SPD_GET_NEXT;
  if(in.dir == 0)
  {
    in.dir = IPSEC_INBOUND;
  }
  return_value = of_nem_get_nsid_from_dpid_and_dp_ns_id(datapath_info.dpid,0,&nsid);
  //get nsid from dpid before calling this API
  return_value = ipsec_spd_get(nsid,  API_CTRL_FLAG_NO_RESP_EXPECTED,
                               &in, &out, NULL);
  if ( return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("ipsec policy stats failed");
    return OF_FAILURE;
  }
  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));

 if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    return OF_FAILURE;
  }
  ipsec_policy_stats_ucm_getparams (&in,&out, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *pNextNRecordData_p = result_iv_pairs_p;
  *pCount = uiRecCount;
  return OF_SUCCESS;
 
  CM_CBK_DEBUG_PRINT ("ipsec policy stats will be sent as multipart response");
  return CM_CNTRL_MULTIPART_RESPONSE;
}




int32_t ipsec_policy_stats_getexactrec (struct cm_array_of_iv_pairs * pKeysArr,
    struct cm_array_of_iv_pairs ** pIvPairArr)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct cm_app_result *stats_result = NULL;
  int32_t return_value = OF_FAILURE;

  CM_CBK_DEBUG_PRINT ("Entered");
  return return_value;

}

int32_t ipsec_policy_stats_verifycfg (struct cm_array_of_iv_pairs * pKeysArr,
    uint32_t command_id, struct cm_app_result ** pResult)
{
  struct cm_app_result *stats_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct ipsec_spd_get_inargs in = {};
  struct ipsec_spd_get_outargs out = {};
  

  CM_CBK_DEBUG_PRINT ("Entered");

  return_value = ipsec_policy_stats_ucm_validatemandparams (pKeysArr, &stats_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  if (ipsec_policy_stats_ucm_validateoptparams (pKeysArr, &stats_result)
      != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Optional Parameters Failed");
    return OF_FAILURE;
  }

  *pResult = stats_result;
  return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
* * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */

int32_t ipsec_policy_stats_ucm_validatemandparams (struct cm_array_of_iv_pairs *
    pMandParams,	struct cm_app_result **ppResult)
{
  uint32_t uiMandParamCnt;
  struct cm_app_result *stats_result = NULL;
  uint32_t uiPortId;


  CM_CBK_DEBUG_PRINT ("Entered");
  for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;
      uiMandParamCnt++)
  {
    switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
    {
        default :     
        break;

    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
  return OF_SUCCESS;
}

int32_t ipsec_policy_stats_ucm_validateoptparams (struct cm_array_of_iv_pairs *
    pOptParams,
    struct cm_app_result ** ppResult)
{
  uint32_t uiOptParamCnt, uiTableCnt;
  struct cm_app_result *of_spd_result = NULL;  

  CM_CBK_DEBUG_PRINT ("Entered");
  for (uiOptParamCnt = 0; uiOptParamCnt < pOptParams->count_ui; uiOptParamCnt++)
  {
    switch (pOptParams->iv_pairs[uiOptParamCnt].id_ui)
    {
      case  CM_DM_SPDSTATS_DIRECTION_ID:
        if (pOptParams->iv_pairs[uiOptParamCnt].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT (" Direction is NULL");
          fill_app_result_struct (&of_spd_result, NULL, CM_GLU_IPSEC_DIRECTION_NULL);
          *presult_p = of_spd_result;
          return OF_FAILURE;
        }
        if(pOptParams->iv_pairs[uiOptParamCnt].value_p != IPSEC_INBOUND || 
           pOptParams->iv_pairs[uiOptParamCnt].value_p != IPSEC_OUTBOUND)
        {
           CM_CBK_DEBUG_PRINT (" Direction is NULL");
           return OF_FAILURE;
        }
        break;
     case CM_DM_SPDSTATS_POLICYID_ID:
       if (pOptParams->iv_pairs[uiOptParamCnt].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT (" Policy ID is NULL");
          fill_app_result_struct (&of_spd_result, NULL, CM_GLU_SPD_POLICY_ID_NULL);
          *presult_p = of_spd_result;
          return OF_FAILURE;
        }
        break;
   
      default:
        break;
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
  return OF_SUCCESS;
}

int32_t ipsec_policy_stats_ucm_setmandparams (struct cm_array_of_iv_pairs  *pMandParams,
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
        datapath_info->dpid=idpId;
        CM_CBK_DEBUG_PRINT("dpid is: %llx\r\n", idpId);
        break;
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
  return OF_SUCCESS;
}

int32_t ipsec_policy_stats_ucm_setoptparams (struct cm_array_of_iv_pairs *
    pOptParams,
    struct ipsec_spd_get_inargs *in_info,
    struct ipsec_spd_get_outargs *out_info,
    struct cm_app_result ** ppResult)
{
  uint32_t uiOptParamCnt;

  CM_CBK_DEBUG_PRINT ("Entered");

  for (uiOptParamCnt = 0; uiOptParamCnt < pOptParams->count_ui; uiOptParamCnt++)
  {
    switch (pOptParams->iv_pairs[uiOptParamCnt].id_ui)
    {
      case CM_DM_SPDSTATS_DIRECTION_ID:
        CM_CBK_DEBUG_PRINT ("direction:%s",(char *) pOptParams->iv_pairs[uiOptParamCnt].value_p);
        if(!strcmp((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p,"IPSEC_INBOUND"))
        {
          in_info->dir = IPSEC_INBOUND;
        }
        else if(!strcmp((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p,"IPSEC_OUTBOUND"))
        {
          in_info->dir = IPSEC_OUTBOUND;
        }
        break;

      case CM_DM_SPDSTATS_POLICYID_ID:
        CM_CBK_DEBUG_PRINT ("policy_id:%s",(char *) pOptParams->iv_pairs[uiOptParamCnt].value_p);
        in_info->policy_id = of_atoi((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p);
        break;
       
      default:
        break;
    }
  }

  CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
  return OF_SUCCESS;
}


int32_t ipsec_policy_stats_ucm_getparams (struct ipsec_spd_get_inargs *in_info,
                                          struct ipsec_spd_get_outargs *out_info,
                                          struct cm_array_of_iv_pairs  *result_iv_pairs_p)
{
  char *instruction_actions[]={
    "OFPAT_OUTPUT",
    "None",
    "None",
    "None",
    "None",
    "None",
    "None",
    "None",
    "None",
    "None",
    "None",
    "OFPAT_COPY_TTL_OUT",
    "OFPAT_COPY_TTL_IN",
    "None",
    "None",
    "OFPAT_SET_MPLS_TTL",
    "OFPAT_DEC_MPLS_TTL",
    "OFPAT_PUSH_VLAN",
    "OFPAT_POP_VLAN",
    "OFPAT_PUSH_MPLS",
    "OFPAT_POP_MPLS",
    "OFPAT_SET_QUEUE",
    "OFPAT_GROUP",
    "OFPAT_SET_NW_TTL",
    "OFPAT_DEC_NW_TTL",
    "OFPAT_SET_FIELD",
    "OFPAT_PUSH_PBB",
    "OFPAT_POP_PBB"
  };

  char *port_type[]={
    "OFPP_MAX",
    "OFPP_IN_PORT",
    "OFPP_TABLE",
    "OFPP_NORMAL",
    "OFPP_FLOOD",
    "OFPP_ALL",
    "OFPP_CONTROLLER",
    "OFPP_LOCAL",
    "OFPP_ANY"
  };

char *fslx_instrctn_subtype[]= 
{
  "None",
  "FSLX_APPLY_ACTIONS_ON_CREATE",
  "FSLX_APPLY_ACTIONS_ON_DELETE",
  "FSLXIT_WAIT_TO_COMPLETE_PKT_OUTS",
  "FSLXIT_JUMP",
  "FSLXIT_WRITE_PRIORITY_REGISTER",
  "FSLXIT_WRITE_PRIORITY_REG_FROM_CURRENT_FLOW",
  "FSLXIT_RE_LOOKUP",
  "FSLXIT_RE_INJECT_TO",
  "FSLXIT_APPLY_GROUP",
  "FSLXIT_WRITE_GROUP",
  "EXT1",
  "EXT2",
  "EXT3",
  "EXT4"
};

char *fslx_action_subtype[]=
{
    "None",
    "FSLXAT_ATTACH_BIND_OBJ",
    "FSLXAT_DETACH_BIND_OBJ",            
    "FSLXAT_ATTACH_REVALIDATION_OBJ",
    "FSLXAT_DETACH_REVALIDATION_OBJ",
    "FSLXAT_SET_FLOW_DIRECTION",          
    "FSLXAT_SET_APP_STATE_INFO",          
    "FSLXAT_SET_CLOSE_WAIT_TIME",         
    "FSLXAT_SEND_APP_STATE_INFO",         
    "FSLXAT_BIND_OBJECT_APPLY_ACTIONS",
    "FSLXAT_BIND_OBJECT_WRITE_ACTIONS",
    "FSLXAT_SET_PHY_PORT_FIELD_CNTXT",
    "FSLXAT_SET_META_DATA_FIELD_CNTXT",
    "FSLXAT_SET_TUNNEL_ID_FIELD_CNTXT",
    "FSLXAT_ENABLE_TCP_TERMINATE_SM",
    "FSLXAT_ARP_RESPONSE",
    "FSLXAT_WRITE_METADATA_FROM_PKT",
    "FSLXAT_SEQ_NO_DELTA_ADDITION",
    "FSLXAT_SEQ_NO_VALIDATION",
    "FSLXAT_START_TABLE",
    "FSLXAT_IP_REASM",
    "FSLXAT_IP_FRAGMENT",
    "FSLXAT_SEND_ICMPV4_TIME_EXCEED",
    "FSLXAT_SEND_ICMPV4_DEST_UNREACHABLE",
    "FSLXAT_DROP_PACKET",
    "FSLXAT_CREATE_SA_CNTXT", 
    "FSLXAT_DELETE_SA_CNTXT",
    "FSLXAT_IPSEC_ENCAP",   
    "FSLXAT_IPSEC_DECAP",
    "FSLXAT_POP_NATT_UDP_HEADER",
    "FSLXAT_WRITE_META_DATA",
    "EXT1",
    "EXT2",
    "EXT3",
    "EXT4"
};

  char string[1024];
  char tmp_string[1024];
  char buf_port[40];
  int32_t index = 0, count_ui, iIndex;
  uint8_t bind_obj_id;  

#define CM_SPDSTATS_CHILD_COUNT 5
  count_ui = CM_SPDSTATS_CHILD_COUNT;

  result_iv_pairs_p->iv_pairs = (struct cm_iv_pair*)of_calloc(count_ui, sizeof(struct cm_iv_pair));
  if(!result_iv_pairs_p->iv_pairs)
  {
    CM_CBK_DEBUG_PRINT (" Memory allocation failed ");
    return OF_FAILURE;
  }
  of_memset(string, 0, sizeof(string));
  sprintf(string,"%d",in_info->dir);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_SPDSTATS_DIRECTION_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  sprintf(string,"%d",in_info->policy_id);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_SPDSTATS_POLICYID_ID,CM_DATA_TYPE_STRING, string);
  index++;

  result_iv_pairs_p->count_ui = index;
  return OF_SUCCESS;
}

#endif /* OF_CM_SUPPORT */
