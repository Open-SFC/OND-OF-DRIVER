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
* File name: auxchannelscbk.c
* Author: D Praveen <B38862@freescale.com>
* Date:   21/05/2013
* Description: 
* 
 */

#ifdef OF_CM_SUPPORT


#include "cbcmninc.h"
#include "cmgutil.h"
#include "cmnet.h"
#include "cntrucminc.h"
#include "cntrl_queue.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"

int32_t dprm_datapath_auxchannels_appl_cmcbk_init (void);

int32_t dprm_datapath_auxchannels_getfirstnrecs (struct cm_array_of_iv_pairs * pKeysArr,
    uint32_t * pCount,
    struct cm_array_of_iv_pairs ** pArrayofIvPairArr);

int32_t dprm_datapath_auxchannels_getnextnrecs (struct cm_array_of_iv_pairs * pKeysArr,
    struct cm_array_of_iv_pairs * pPrevRecordKey, uint32_t * pCount,
    struct cm_array_of_iv_pairs ** pNextNRecordData_p);

int32_t dprm_datapath_auxchannels_verifycfg (struct cm_array_of_iv_pairs * pKeysArr,
    uint32_t command_id, struct cm_app_result ** pResult);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
* * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */
int32_t dprm_datapath_auxchannels_cm_setmandparams (struct cm_array_of_iv_pairs *
    pMandParams,
    struct dprm_datapath_general_info *datapath_info,
    struct cntlr_channel_info_s *channel_info,
    struct cm_app_result ** ppResult);

int32_t dprm_datapath_auxchannels_cm_setoptparams (struct cm_array_of_iv_pairs *pOptParams,
    struct cntlr_channel_info_s *channel_info,
    struct cm_app_result ** ppResult);

int32_t dprm_datapath_auxchannels_cm_getparams (struct cntlr_channel_info_s *channel_info,
    struct cm_array_of_iv_pairs * result_iv_pairs_p);

struct cm_dm_app_callbacks dprm_datapath_auxchannels_cm_callbacks = {
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  dprm_datapath_auxchannels_getfirstnrecs,
  dprm_datapath_auxchannels_getnextnrecs,
  NULL,
  dprm_datapath_auxchannels_verifycfg,
  NULL
};

int32_t dprm_datapath_auxchannels_appl_cmcbk_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  cm_register_app_callbacks (CM_ON_DIRECTOR_DATAPATH_AUXCHANNELS_APPL_ID,&dprm_datapath_auxchannels_cm_callbacks);
  return OF_SUCCESS;
}

int32_t dprm_datapath_auxchannels_getfirstnrecs (struct cm_array_of_iv_pairs * pKeysArr,
    uint32_t * pCount,
    struct cm_array_of_iv_pairs ** pArrayofIvPairArr)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  struct cm_app_result *channel_result = NULL;
  struct dprm_datapath_general_info datapath_info={};
  struct cntlr_channel_info_s channel_info={};
  uint64_t datapath_handle;
  struct dprm_datapath_entry     *datapath = NULL;

  of_memset (&channel_info, 0, sizeof (struct cntlr_channel_info_s));

  if ((dprm_datapath_auxchannels_cm_setmandparams (pKeysArr,&datapath_info, &channel_info, &channel_result)) !=
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

  return_value=get_datapath_byhandle(datapath_handle, &datapath);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Get Datapath failed");
    return OF_FAILURE;
  }

  return_value=dprm_get_first_auxchannel_info(datapath, &channel_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Get first channel record failed for DPRM Table");
    return OF_FAILURE;
  }

  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    return OF_FAILURE;
  }

  dprm_datapath_auxchannels_cm_getparams (&channel_info, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *pArrayofIvPairArr = result_iv_pairs_p;
  //*pCount = uiRecCount;
  return OF_SUCCESS;
}


int32_t dprm_datapath_auxchannels_getnextnrecs (struct cm_array_of_iv_pairs * pKeysArr,
    struct cm_array_of_iv_pairs *pPrevRecordKey, uint32_t * pCount,
    struct cm_array_of_iv_pairs ** pNextNRecordData_p)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  struct cm_app_result *channel_result = NULL;
  struct dprm_datapath_general_info datapath_info={};
  struct cntlr_channel_info_s channel_info={};
  uint64_t datapath_handle;
  struct dprm_datapath_entry     *datapath = NULL;

  of_memset (&channel_info, 0, sizeof (struct cntlr_channel_info_s));

  if ((dprm_datapath_auxchannels_cm_setmandparams (pKeysArr,&datapath_info, &channel_info, &channel_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }
  
  if ((dprm_datapath_auxchannels_cm_setmandparams (pPrevRecordKey,&datapath_info, &channel_info, &channel_result)) !=
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

  return_value=get_datapath_byhandle(datapath_handle, &datapath);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Get Datapath failed");
    return OF_FAILURE;
  }

  return_value= dprm_get_next_auxchannel_info(datapath, channel_info.auxiliary_id, &channel_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Get Next channel record failed for DPRM Table");
    return OF_FAILURE;
  }

  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    return OF_FAILURE;
  }

  dprm_datapath_auxchannels_cm_getparams (&channel_info, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *pNextNRecordData_p = result_iv_pairs_p;
  *pCount = uiRecCount;
  return OF_SUCCESS;
}

int32_t dprm_datapath_auxchannels_verifycfg (struct cm_array_of_iv_pairs * pKeysArr,
    uint32_t command_id, struct cm_app_result ** pResult)
{
  return OF_SUCCESS;
}

int32_t dprm_datapath_auxchannels_cm_setmandparams (struct cm_array_of_iv_pairs *    pMandParams,
    struct dprm_datapath_general_info *datapath_info,
    struct cntlr_channel_info_s *channel_info,
    struct cm_app_result ** ppResult)
{
  uint32_t uiMandParamCnt;
  uint32_t auxilary_id;
  uint64_t idpId;

  CM_CBK_DEBUG_PRINT ("Entered \r\n");
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

      case CM_DM_AUXCHANNELS_AUXLIARYID_ID:
        auxilary_id = of_atoi((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p); 
        CM_CBK_DEBUG_PRINT("auxilary id is %d", auxilary_id);
        channel_info->auxiliary_id=auxilary_id;
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
  return OF_SUCCESS;
}

int32_t dprm_datapath_auxchannels_cm_setoptparams (struct cm_array_of_iv_pairs *pOptParams,
    struct cntlr_channel_info_s *channel_info,
    struct cm_app_result ** ppResult)
{
  uint32_t uiOptParamCnt;

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


int32_t dprm_datapath_auxchannels_cm_getparams (struct cntlr_channel_info_s *channel_info,
    struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
  uint32_t uiIndex = 0;
  char buf[64] = "";
#define CM_AUXCHANNELS_CHILD_COUNT 7

  result_iv_pairs_p->iv_pairs =
    (struct cm_iv_pair *) of_calloc (CM_AUXCHANNELS_CHILD_COUNT, sizeof (struct cm_iv_pair));
  if (result_iv_pairs_p->iv_pairs == NULL)
  {
    CM_CBK_DEBUG_PRINT
      ("Memory allocation failed for result_iv_pairs_p->iv_pairs");
    return OF_FAILURE;
  }

  of_memset(buf, 0, sizeof(buf));
  of_itoa (channel_info->auxiliary_id, buf);
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uiIndex], CM_DM_AUXCHANNELS_AUXLIARYID_ID,
      CM_DATA_TYPE_STRING, buf);
  uiIndex++;

  of_memset(buf, 0, sizeof(buf));
  //cm_inet_ntoal(channel_info->dp_ip.v4_val, buf);
  strcpy(buf, "127.0.0.1"); /*temporarily copying loop back address*/
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uiIndex], CM_DM_AUXCHANNELS_LOCALIP_ID,
      CM_DATA_TYPE_STRING, buf);
  uiIndex++;
/*Atmaram -TBD: need to get protocol specific info and displayed*/
 #if 0 
  of_memset(buf, 0, sizeof(buf));
  of_itoa (channel_info->local_port, buf);
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uiIndex], CM_DM_AUXCHANNELS_LOCALPORT_ID,
      CM_DATA_TYPE_STRING, buf);
  uiIndex++;
  
  of_memset(buf, 0, sizeof(buf));
  cm_inet_ntoal(channel_info->dp_ip.v4_val, buf);
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uiIndex], CM_DM_AUXCHANNELS_PEERIP_ID,
      CM_DATA_TYPE_STRING, buf);
  uiIndex++;

  of_memset(buf, 0, sizeof(buf));
  of_itoa (channel_info->dp_port, buf);
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uiIndex],CM_DM_AUXCHANNELS_PEERPORT_ID,
      CM_DATA_TYPE_STRING, buf);
  uiIndex++;
 #endif
  of_memset(buf, 0, sizeof(buf));
  of_ltoa (channel_info->recv_bytes, buf);
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uiIndex],CM_DM_AUXCHANNELS_RXBYTES_ID,
      CM_DATA_TYPE_STRING, buf);
  uiIndex++;

  of_memset(buf, 0, sizeof(buf));
  of_ltoa (channel_info->tx_bytes, buf);
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uiIndex],CM_DM_AUXCHANNELS_TXBYTES_ID,
      CM_DATA_TYPE_STRING, buf);
  uiIndex++;

  result_iv_pairs_p->count_ui = uiIndex;
  return OF_SUCCESS;
}

#endif /* OF_CM_SUPPORT */
