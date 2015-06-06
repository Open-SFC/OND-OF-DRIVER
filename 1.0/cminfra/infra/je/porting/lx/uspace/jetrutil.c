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
 * File name: jetrutil.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/05/2013
 * Description: 
*/

#ifdef CM_TRANSPORT_CHANNEL_SUPPORT
//#ifdef CM_JE_SUPPORT
#include "jeincld.h"
#include "jewrap.h"

extern uint32_t uiInt32Size_g;
extern uint32_t uiInt64Size_g;
extern uint32_t uiOSIntSize_g;
extern uint32_t uiOSLongIntSize_g;
/**
 \addtogroup TRCHLJEAPI Transaction Channel APIs
    This API is used to fill the Generic header by taking the required informtion as input. This will take Management EngineID, CommandId and Admin Role Information and fills the Generic header structure.
    <b>Input Parameters: </b> \n
    <i>uiEngineId:</i> Management Engine ID.
    <i>ulCmdId:</i> Command Identifier.
    <i>role_info_p:</i> Pointer to the Admin role information structure.
    <b>Output Parameters:</b> NONE \n
    <i>gen_hdr_p</i> Pointer to the Generic header structure.
    <b>Return Value:</b> NONE.\n
 **/
void cm_fill_generic_header (uint32_t uiEngineId, uint32_t uiCmdId,
                             struct cm_role_info * role_info_p,
                             struct cm_msg_generic_header * gen_hdr_p)
{

  if (!(role_info_p) || !(gen_hdr_p))
  {
    CM_JETRANS_DEBUG_PRINT (" Invalid input");
    return;
  }

  /* Fill CommandId */
  //gen_hdr_p->command_id = htonl(uiCmdId);
  gen_hdr_p->command_id = uiCmdId;

  /* Fill EngineID */
  //gen_hdr_p->mgmt_engine_id = htonl(uiEngineId);
  gen_hdr_p->mgmt_engine_id = uiEngineId;

  gen_hdr_p->flags = 0;
  gen_hdr_p->sequence_id = 0;        /* reserved: future purpose */

  /* Admin Name  */
  if (role_info_p->admin_name[0] != '\0')
  {
    of_strcpy (gen_hdr_p->admin_name, role_info_p->admin_name);
    CM_JETRANS_DEBUG_PRINT ("AdminName=%s",gen_hdr_p->admin_name);
  }
  /* Admin Role  */
  if (role_info_p->admin_role[0] != '\0')
  {
    of_strcpy (gen_hdr_p->admin_role, role_info_p->admin_role);
    CM_JETRANS_DEBUG_PRINT ("ROLE=%s",gen_hdr_p->admin_role);
  }


}

/**
 \ingroup TRCHLJEAPI
  This API is used to fill the receive message structure. This will take the Sub-Command ID, DMpath information and Array of Name-Value pairs and fill the Request message structure.
  <b>Input Parameters: </b> \n
  <i>sub_command_id:</i> Sub-Comman Identifier.
  <i>dm_path_p:</i> Pointer to data_model path.
  <i>pNvPairAry:</i> Pointer to the Array of Name-Value pairs.
  <b>Output Parameters:</b> \n
  <i>pRequestMsg:</i> Pointer to the request message structure.
  <b>Return Value:</b>\n
  <i>OF_SUCCESS:</i> On Sending the Message buffer successfully .
  <i>OF_FAILURE:</i> On any failure.
 **/
int32_t cm_fill_request_message (struct cm_request_msg * pRequestMsg,
                               uint32_t sub_command_id,
                               unsigned char * dm_path_p,
                               uint32_t count_ui,
                               struct cm_array_of_nv_pair * pNvPairAry,
                               struct cm_array_of_nv_pair * pPrevRecNvPair, uint32_t * puiLen)
{
  uint32_t uiLen = 0;
  int32_t i, uiDMPathLen = 0;
  struct cm_nv_pair *pTmpNvPair;

  /* Input validations */
  if (!pRequestMsg)
  {
    CM_JETRANS_DEBUG_PRINT (" Invalid input");
    *puiLen = uiLen;
    return OF_FAILURE;
  }

  if ((sub_command_id == CM_COMMAND_CONFIG_SESSION_UPDATE) && !(pNvPairAry))
  {
    CM_JETRANS_DEBUG_PRINT (" Invalid input");
    *puiLen = uiLen;
    return OF_FAILURE;
  }

  /* Fill Sub-Command ID */
  pRequestMsg->sub_command_id = sub_command_id;
//  uiLen += sizeof (sub_command_id);
  uiLen += uiOSIntSize_g;
  /* DM Path Length */
//  uiLen += uiInt32Size_g;
  uiLen += uiOSIntSize_g;
  if (dm_path_p)
  {
    uiDMPathLen = of_strlen (dm_path_p);
    uiLen += uiDMPathLen;
  }
  pRequestMsg->dm_path_p = dm_path_p;

  switch (sub_command_id)
  {
    case CM_CMD_GET_NEXT_N_LOGS:
    case CM_CMD_GET_NEXT_N_RECS:
      if (pPrevRecNvPair)
      {
#if 0
        pRequestMsg->prev_nv_pair.name_length = pPrevRecNvPair->name_length;
        pRequestMsg->prev_nv_pair.name_p = pPrevRecNvPair->name_p;
        pRequestMsg->prev_nv_pair.value_type = pPrevRecNvPair->value_type;
        pRequestMsg->prev_nv_pair.value_length = pPrevRecNvPair->value_length;
        pRequestMsg->prev_nv_pair.value_p = pPrevRecNvPair->value_p;

        uiLen += uiOSIntSize_g;
        uiLen += pPrevRecNvPair->name_length;
        uiLen += uiOSIntSize_g;       /* Value Type */
        uiLen += uiOSIntSize_g;       /* Value Len */
        uiLen += pPrevRecNvPair->value_length;
#endif
        pRequestMsg->prev_nv_pair.count_ui = pPrevRecNvPair->count_ui;
        pRequestMsg->prev_nv_pair.nv_pairs = pPrevRecNvPair->nv_pairs;
//        uiLen += uiInt32Size_g;   /*  sizeof (pNvPairAry->count_ui) */
        uiLen += uiOSIntSize_g;
        pTmpNvPair = pRequestMsg->prev_nv_pair.nv_pairs;
        /* Length calculation */
        for (i = 0; i <  pPrevRecNvPair->count_ui; i++)
        {
//           uiLen += uiInt32Size_g;
           uiLen += uiOSIntSize_g;
           uiLen += pTmpNvPair[i].name_length;
//           uiLen += uiInt32Size_g; /* Value Type */
//           uiLen += uiInt32Size_g; /* Value Len */
           uiLen += uiOSIntSize_g; /* Value Type */
           uiLen += uiOSIntSize_g; /* Value Len */
           uiLen += pTmpNvPair[i].value_length;
        }
      }
    case CM_CMD_GET_FIRST_N_RECS:
    case CM_CMD_GET_FIRST_N_LOGS:
      pRequestMsg->count_ui = count_ui;
//      uiLen += uiInt32Size_g;
      uiLen += uiOSIntSize_g;
      break;
    case CM_CMD_GET_EXACT_REC:
#ifdef CM_STATS_COLLECTOR_SUPPORT
    case CM_CMD_GET_AGGREGATE_STATS:
    case CM_CMD_GET_AVERAGE_STATS:
    case CM_CMD_GET_PER_DEVICE_STATS:
#endif
      pRequestMsg->count_ui = 1;
//      uiLen += uiInt32Size_g;
      uiLen += uiOSIntSize_g;
      break;

  }

  /* Copy Name-Value pairs */
  if (pNvPairAry)
  {
    pRequestMsg->nv_pair_array.count_ui = pNvPairAry->count_ui;
    pRequestMsg->nv_pair_array.nv_pairs = pNvPairAry->nv_pairs;
//    uiLen += uiInt32Size_g;   /*  sizeof (pNvPairAry->count_ui) */
    uiLen += uiOSIntSize_g;   /*  sizeof (pNvPairAry->count_ui) */
    pTmpNvPair = pRequestMsg->nv_pair_array.nv_pairs;
    /* Length calculation */
    for (i = 0; i < pNvPairAry->count_ui; i++)
    {
//      uiLen += uiInt32Size_g;
      uiLen += uiOSIntSize_g;
      uiLen += pTmpNvPair[i].name_length;
      uiLen += uiOSIntSize_g; /* Value Type */
      uiLen += uiOSIntSize_g; /* Value Len */
      uiLen += pTmpNvPair[i].value_length;
    }
  }

  *puiLen += uiLen;
  CM_JETRANS_DEBUG_PRINT ("Input Message Len %d", uiLen);
  return OF_SUCCESS;
}

/**
 \ingroup TRCHLJEAPI
  This API is used to flaten a given structure into a character buffer. This will take the request message structure as input, that contains SubCommandID, DMPath and Array of Name-Value pairs and flatens them into to a character buffer to send the buffer across Transport channel.
  <b>Input Parameters: </b> \n
  <i>sock_fd_i:</i> Holds the Socket handle.
  <i>pBuff:</i> Pointer to the Message buffer.
  <i>uiLen:</i> Message Buffer length.
  <b>Output Parameters:</b> NONE \n
  <b>Return Value:</b>\n
  <i>OF_SUCCESS:</i> On Sending the Message buffer successfully .
  <i>OF_FAILURE:</i> On any failure.
 **/
int32_t cm_frame_tnsprt_request (struct cm_request_msg * pReqMsg,
                                     unsigned char * pSendMsg)
{
  char *pTmp;
  int32_t i; 
  uint32_t uiDMPathLen = 0;

  if (!pReqMsg || !(pSendMsg))
  {
    CM_JETRANS_DEBUG_PRINT (" Invalid Input");
    return OF_FAILURE;
  }

  pTmp = pSendMsg;

  /* Start framing the message buffer */

  /* Copy Sub-Command Id */
  pTmp = (char *)of_mbuf_put_32 (pTmp, pReqMsg->sub_command_id);

  /* Copy DMPathLength */
  if (pReqMsg->dm_path_p)
  {
    uiDMPathLen = of_strlen (pReqMsg->dm_path_p);
    pTmp = (char *)of_mbuf_put_32 (pTmp, uiDMPathLen);
    of_memcpy (pTmp, pReqMsg->dm_path_p, uiDMPathLen);
    pTmp += uiDMPathLen;
  }
  else
    pTmp = (char *)of_mbuf_put_32 (pTmp, 0);

  switch (pReqMsg->sub_command_id)
  {
    case CM_CMD_ADD_PARAMS:
    case CM_CMD_SET_PARAMS:
    case CM_CMD_DEL_PARAMS:
    case CM_CMD_COMPARE_PARAM:
    case CM_CMD_CANCEL_PREV_CMD:
    case CM_CMD_IS_DM_INSTANCE_USING_NAMEPATH_AND_KEYARRAY:
    case CM_SUBCOMMAND_AUTHENTICATE_USER:
#ifdef CM_STATS_COLLECTOR_SUPPORT
    case CM_CMD_GET_AGGREGATE_STATS:
    case CM_CMD_GET_AVERAGE_STATS:
    case CM_CMD_GET_PER_DEVICE_STATS:
#endif 
      /* Copy NvPair Array Count */
      pTmp = (char *)of_mbuf_put_32 (pTmp, pReqMsg->nv_pair_array.count_ui);
      if (pReqMsg->nv_pair_array.count_ui)
      {
        /* Copy Name-Value pair(s) */
        for (i = 0; i < pReqMsg->nv_pair_array.count_ui; i++)
        {
          cm_je_copy_nvpair_to_buffer (&pTmp, &pReqMsg->nv_pair_array.nv_pairs[i]);
        }
      }
      break;
    case CM_CMD_GET_FIRST_N_LOGS:
    case CM_CMD_GET_NEXT_N_LOGS:
    case CM_CMD_GET_FIRST_N_RECS:
    case CM_CMD_GET_NEXT_N_RECS:
    case CM_CMD_GET_EXACT_REC:
      /* Copy Record Count */
      pTmp = (char *)of_mbuf_put_32 (pTmp, pReqMsg->count_ui);
      pTmp = (char *)of_mbuf_put_32 (pTmp, pReqMsg->nv_pair_array.count_ui);

      /* Copy NvPair Array Count */
      if (pReqMsg->nv_pair_array.count_ui)
      {
        /* Copy Name-Value pair(s) */
        for (i = 0; i < pReqMsg->nv_pair_array.count_ui; i++)
        {
          /* Copy Name Length */
          cm_je_copy_nvpair_to_buffer (&pTmp, &pReqMsg->nv_pair_array.nv_pairs[i]);

        }
      }

      if (pReqMsg->sub_command_id == CM_CMD_GET_NEXT_N_RECS)
      {
         //        cm_je_copy_nvpair_to_buffer (&pTmp, &pReqMsg->prev_nv_pair);
         pTmp = (char *)of_mbuf_put_32 (pTmp, pReqMsg->prev_nv_pair.count_ui);
         if (pReqMsg->prev_nv_pair.count_ui)
         {
            /* Copy Name-Value pair(s) */
            for (i = 0; i < pReqMsg->prev_nv_pair.count_ui; i++)
            {
               /* Copy Name Length */
               cm_je_copy_nvpair_to_buffer (&pTmp, &pReqMsg->prev_nv_pair.nv_pairs[i]);

            }
         }
      }
      break;

    case CM_CMD_LOAD_CONFIG:
      break;
    case CM_CMD_SAVE_CONFIG:
      break;
    case CM_CMD_RESET_TO_FACTORY_DEFAULTS:
    case CM_CMD_FLUSH_CONFIG:
      break;
    case CM_CMD_GET_DM_FIRST_NODE_INFO:
    case CM_CMD_GET_DM_NEXT_NODE_INFO:
    case CM_CMD_GET_DM_NODE_INFO:
    case CM_CMD_GET_DM_NODE_AND_CHILD_INFO:
    case CM_CMD_GET_DM_CHILD_NODE_COUNT:
    case CM_CMD_SET_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
    case CM_CMD_GET_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
    case CM_CMD_DEL_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
    case CM_CMD_GET_NEW_DM_INSTANCE_ID:
    case CM_CMD_ADD_DM_INSTANCE_MAP:
    case CM_CMD_GET_DM_INSTANCE_MAPLIST_BY_NAMEPATH:
    case CM_CMD_DEL_DM_INSTANCE_MAP:
    case CM_CMD_GET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
    case CM_CMD_SET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
    case CM_CMD_DEL_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
      // copy into
      // pConfigRequest->data.data_modelGet.count_ui:
      break;
  }

  return OF_SUCCESS;
}

/**
 \ingroup TRCHLJEAPI
  This API is used to extract the Result information from the received message buffer. This will take the Message buffer received from Transport channel as input and tries to extract the expected information from the buffer and fills the struct cm_result structure with apropriate values.
  <b>Input Parameters: </b> \n
  <i>pRespBuff:</i> Pointer to the Message buffer received from Transport channel.
  <b>Output Parameters:</b> \n
  <i>puiReqBufLen:</i> Request buffer length.
  <i>resp_msg_p:</i> Pointer to the struct cm_result structure used to hold the extracted information.
  <b>Return Value:</b>\n
  <i>OF_SUCCESS:</i> On successful extraction of Result structure information.
  <i>OF_FAILURE:</i> On any failure.
 **/
int32_t UCMExtractResponseFromBuff (uint32_t command_id,
                                    char * pRespBuff,
                                    uint32_t * puiReqBufLen,
                                    struct cm_result * cm_resp_msg_p)
{
  char *pUCMTmpRespBuf = pRespBuff;
  uint32_t uiNvCnt = 0;
  uint32_t uiRecCnt = 0, uiLen = 0, uiDMPathLen = 0;
  int32_t ii;
  uint32_t no_of_commands;
  struct cm_nv_pair *nv_pairs_p;
  struct cm_dm_instance_map *pTempInst;
  struct cm_array_of_commands *pCmdArray = NULL;
  struct cm_command *pTmpCmd;

  if (pUCMTmpRespBuf == NULL)
  {
    CM_JETRANS_DEBUG_PRINT (" Invalid input Buffer");
    return OF_FAILURE;
  }

  if (puiReqBufLen <= 0)
  {
    CM_JETRANS_DEBUG_PRINT (" Invalid input Buffer Length");
    return OF_FAILURE;
  }

  if (cm_resp_msg_p == NULL)
  {
    CM_JETRANS_DEBUG_PRINT ("Invalid Output Response Message");
    return OF_FAILURE;
  }
  /*presuming Generic Header is already extracted. */

  /* Copy Sub Command */
  cm_resp_msg_p->sub_command_id = of_mbuf_get_32 (pUCMTmpRespBuf);
  pUCMTmpRespBuf += uiOSIntSize_g;

  /* Copy Result code */
  cm_resp_msg_p->result_code = of_mbuf_get_32 (pUCMTmpRespBuf);
  pUCMTmpRespBuf += uiOSIntSize_g;

  /* Copy Data */
  if (command_id == CM_COMMAND_CONFIG_SESSION_START)
  {
    switch (cm_resp_msg_p->result_code)
    {
      case CM_JE_SUCCESS:
        CM_JETRANS_DEBUG_PRINT (" Configuration Session recieved");
        cm_resp_msg_p->data.je_data_p =
          (struct cm_je_config_session *) of_calloc (1, sizeof (struct cm_je_config_session));
        if (!cm_resp_msg_p->data.je_data_p)
        {
          CM_JETRANS_DEBUG_PRINT ("Memory Allocatin Failed");
          return OF_FAILURE;
        }
        of_memcpy (cm_resp_msg_p->data.je_data_p, pUCMTmpRespBuf,
                  sizeof (struct cm_je_config_session));
        pUCMTmpRespBuf += sizeof (struct cm_je_config_session);
        break;
      case CM_JE_FAILURE:
        break;
    }
  }

  switch (cm_resp_msg_p->sub_command_id)
  {
    case CM_CMD_ADD_PARAMS:
    case CM_CMD_SET_PARAMS:
    case CM_CMD_DEL_PARAMS:
    case CM_CMD_CONFIG_SESSION_COMMIT:
    case CM_CMD_CONFIG_SESSION_REVOKE:
      CM_JETRANS_DEBUG_PRINT (" Response for Post Configuration Request");
      switch (cm_resp_msg_p->result_code)
      {
        case CM_JE_SUCCESS:
          CM_JETRANS_DEBUG_PRINT (" JE returned Success");
          break;
        case CM_JE_FAILURE:
        {
          CM_JETRANS_DEBUG_PRINT (" JE returned Failured");
          cm_resp_msg_p->result.error.error_code = of_mbuf_get_32 (pUCMTmpRespBuf);
          pUCMTmpRespBuf += uiOSIntSize_g;

          UCMCopyResultMessageFromBuffer (pUCMTmpRespBuf, cm_resp_msg_p, &uiLen);
          pUCMTmpRespBuf += uiLen;

          uiLen = 0;
          if (UCMCopyDMPathFromBuffer
              (pUCMTmpRespBuf, cm_resp_msg_p, &uiLen) == OF_FAILURE)
          {
            CM_JETRANS_DEBUG_PRINT ("Copy DM Path Failed");
            if (cm_resp_msg_p->result.error.result_string_p)
              of_free (cm_resp_msg_p->result.error.result_string_p);
            return OF_FAILURE;
          }
          pUCMTmpRespBuf += uiLen;

          /* Copy Error Name Value Pair */
          cm_resp_msg_p->result.error.nv_pair_b = of_mbuf_get_32 (pUCMTmpRespBuf);
          pUCMTmpRespBuf += uiOSIntSize_g;
          if (cm_resp_msg_p->result.error.nv_pair_b)
          {
            uiLen = 0;
            if ((UCMCopyNvPairFromBuffer
                 (pUCMTmpRespBuf, &(cm_resp_msg_p->result.error.nv_pair),
                  &uiLen)) != OF_SUCCESS)
            {
              CM_JETRANS_DEBUG_PRINT ("Copy Name Value Pairs failed ");
              if (cm_resp_msg_p->result.error.result_string_p)
                of_free (cm_resp_msg_p->result.error.result_string_p);
              if (cm_resp_msg_p->result.error.dm_path_p)
                of_free (cm_resp_msg_p->result.error.dm_path_p);
              return OF_FAILURE;
            }
            pUCMTmpRespBuf += uiLen;
          }
        }
          break;
      }
      break;
    case CM_CMD_CANCEL_PREV_CMD:
    case CM_CMD_SET_DEFAULTS:
    case CM_CMD_SET_PARAM_TO_DEFAULT:
      switch (cm_resp_msg_p->result_code)
      {
        case CM_JE_SUCCESS:
          CM_JETRANS_DEBUG_PRINT ("JE returned Success");
          break;
        case CM_JE_FAILURE:
          CM_JETRANS_DEBUG_PRINT ("JE returned Failured");
      }
      break;
    case CM_CMD_COMPARE_PARAM:
      switch (cm_resp_msg_p->result_code)
      {
        case CM_JE_SUCCESS:
        {
          struct cm_array_of_nv_pair *pnv_pair_array;
          pnv_pair_array =
            (struct cm_array_of_nv_pair *) of_calloc (1, sizeof (struct cm_array_of_nv_pair));
          if (pnv_pair_array == NULL)
          {
            CM_JETRANS_DEBUG_PRINT ("Memory Allocation Failed");
            return OF_FAILURE;
          }
          /* Get Commands Count */
          pnv_pair_array->count_ui = of_mbuf_get_32 (pUCMTmpRespBuf);
          pUCMTmpRespBuf += uiOSIntSize_g;

          if (pnv_pair_array->count_ui)
          {
            pnv_pair_array->nv_pairs =
              (struct cm_nv_pair *) of_calloc (pnv_pair_array->count_ui,
                                        sizeof (struct cm_nv_pair));
            if (!pnv_pair_array->nv_pairs)
            {
              of_free (pnv_pair_array);
              CM_JETRANS_DEBUG_PRINT ("Memory Allocation Failed");
              return OF_FAILURE;
            }
            for (uiNvCnt = 0, uiLen = 0;
                 uiNvCnt < pnv_pair_array->count_ui;
                 uiNvCnt++, pUCMTmpRespBuf += uiLen, uiLen = 0)
            {
              if ((UCMCopyNvPairFromBuffer
                   (pUCMTmpRespBuf, &pnv_pair_array->nv_pairs[uiNvCnt],
                    &uiLen)) != OF_SUCCESS)
              {
    if(pnv_pair_array)
                  CM_FREE_PTR_NVPAIR_ARRAY (pnv_pair_array, pnv_pair_array->count_ui);
                CM_JETRANS_DEBUG_PRINT ("Memory Allocation Failed");
                return OF_FAILURE;
              }
            }
          }
          cm_resp_msg_p->data.je_data_p = (void *) pnv_pair_array;
          break;
        }
        case CM_JE_FAILURE:
          CM_JETRANS_DEBUG_PRINT (" JE returned Failure");
          break;
      }
      break;
    case CM_CMD_GET_CONFIG_SESSION:
      switch (cm_resp_msg_p->result_code)
      {
        case CM_JE_SUCCESS:
        {
          pCmdArray =
            (struct cm_array_of_commands *) of_calloc (1,
                                               sizeof (struct cm_array_of_commands));
          if (pCmdArray == NULL)
          {
            CM_JETRANS_DEBUG_PRINT ("Memory Allocation Failed");
            return OF_FAILURE;
          }
          /* Get Commands Count */
          pCmdArray->count_ui = of_mbuf_get_32 (pUCMTmpRespBuf);
          pUCMTmpRespBuf += uiOSIntSize_g;

          pCmdArray->Commands = (struct cm_command *) of_calloc (pCmdArray->count_ui,
                                                           sizeof
                                                           (struct cm_command));
          if (pCmdArray->Commands == NULL)
          {
            of_free (pCmdArray);
            CM_JETRANS_DEBUG_PRINT ("Memory Allocation Failed");
            return OF_FAILURE;
          }
          for (no_of_commands = 0; no_of_commands < pCmdArray->count_ui; no_of_commands++)
          {
            pTmpCmd = &pCmdArray->Commands[no_of_commands];

            /* Get CommandID */
            pTmpCmd->command_id = of_mbuf_get_32 (pUCMTmpRespBuf);
            pUCMTmpRespBuf += uiOSIntSize_g;

            uiDMPathLen = of_mbuf_get_32 (pUCMTmpRespBuf);
            pUCMTmpRespBuf += uiOSIntSize_g;

            if (uiDMPathLen)
            {
              pTmpCmd->dm_path_p = (char *) of_calloc (1, uiDMPathLen + 1);
              if (!pTmpCmd->dm_path_p)
              {
                je_free_commandArray (pCmdArray, no_of_commands);
                CM_JETRANS_DEBUG_PRINT ("Memory Allocation Failed");
                return OF_FAILURE;
              }
              of_memcpy (pTmpCmd->dm_path_p, pUCMTmpRespBuf, uiDMPathLen);
              pUCMTmpRespBuf += uiDMPathLen;
            }
            /* copy NvPair count */
            /*struct cm_array_of_nv_pair nv_pair_array; */
            pTmpCmd->nv_pair_array.count_ui = of_mbuf_get_32 (pUCMTmpRespBuf);
            pUCMTmpRespBuf += uiOSIntSize_g;

            pTmpCmd->nv_pair_array.nv_pairs =
              (struct cm_nv_pair *) of_calloc (pTmpCmd->nv_pair_array.count_ui,
                                        sizeof (struct cm_nv_pair));
            if (!pTmpCmd->nv_pair_array.nv_pairs)
            {
              je_free_commandArray (pCmdArray, no_of_commands);
              CM_JETRANS_DEBUG_PRINT ("Memory Allocation Failed");
              return OF_FAILURE;
            }

            for (uiNvCnt = 0, uiLen = 0;
                 uiNvCnt < pTmpCmd->nv_pair_array.count_ui;
                 uiNvCnt++, pUCMTmpRespBuf += uiLen, uiLen = 0)
            {
              if ((UCMCopyNvPairFromBuffer
                   (pUCMTmpRespBuf, &pTmpCmd->nv_pair_array.nv_pairs[uiNvCnt],
                    &uiLen)) != OF_SUCCESS)
              {
                je_free_commandArray (pCmdArray, no_of_commands);
                CM_JETRANS_DEBUG_PRINT ("Memory Allocation Failed");
                return OF_FAILURE;
              }
            }
          }
          cm_resp_msg_p->data.je_data_p = (void *) pCmdArray;
          break;
        }
        case CM_JE_FAILURE:
          break;
      }
      break;
    case CM_CMD_LOAD_CONFIG:
    case CM_CMD_SAVE_CONFIG:
    case CM_CMD_RESET_TO_FACTORY_DEFAULTS:
    case CM_CMD_FLUSH_CONFIG:
      break;
    case CM_CMD_GET_FIRST_N_LOGS:
    case CM_CMD_GET_NEXT_N_LOGS:
    case CM_CMD_GET_FIRST_N_RECS:
    case CM_CMD_GET_NEXT_N_RECS:
    case CM_CMD_GET_EXACT_REC:
      CM_JETRANS_DEBUG_PRINT (" Copy Response from security applications");
      switch (cm_resp_msg_p->result_code)
      {
        case CM_JE_SUCCESS:
        case CM_JE_NO_MORE_RECS:
        case CM_JE_MORE_RECS:
          CM_JETRANS_DEBUG_PRINT (" JE returned Success");

          /* Number of Records */
          cm_resp_msg_p->data.sec_appl.count_ui = of_mbuf_get_32 (pUCMTmpRespBuf);
          pUCMTmpRespBuf += uiOSIntSize_g;

          cm_resp_msg_p->data.sec_appl.nv_pair_array =
            (struct cm_array_of_nv_pair *) of_calloc (cm_resp_msg_p->data.sec_appl.count_ui,
                                              sizeof (struct cm_array_of_nv_pair));
          if (!(cm_resp_msg_p->data.sec_appl.nv_pair_array))
          {
            CM_JETRANS_DEBUG_PRINT ("Memory Allocation Failed");
            return OF_FAILURE;
          }

          for (uiRecCnt = 0; uiRecCnt < cm_resp_msg_p->data.sec_appl.count_ui;
               uiRecCnt++)
          {
            /* Number of Records  */
            cm_resp_msg_p->data.sec_appl.nv_pair_array[uiRecCnt].count_ui =
              of_mbuf_get_32 (pUCMTmpRespBuf);
            pUCMTmpRespBuf += uiOSIntSize_g;

            /* Allocate Memory nv_pairs */
            cm_resp_msg_p->data.sec_appl.nv_pair_array[uiRecCnt].nv_pairs =
              (struct cm_nv_pair *) of_calloc (cm_resp_msg_p->data.sec_appl.
                                        nv_pair_array[uiRecCnt].count_ui,
                                        sizeof (struct cm_nv_pair));
            if (!(cm_resp_msg_p->data.sec_appl.nv_pair_array[uiRecCnt].nv_pairs))
            {
              CM_JETRANS_DEBUG_PRINT ("Memory Allocation Failed");
            }

            /* Copy  Name Value Pair */
            for (uiNvCnt = 0, uiLen = 0;
                 uiNvCnt <
                 cm_resp_msg_p->data.sec_appl.nv_pair_array[uiRecCnt].count_ui;
                 pUCMTmpRespBuf += uiLen, uiNvCnt++, uiLen = 0)
            {
              if (UCMCopyNvPairFromBuffer
                  (pUCMTmpRespBuf,
                   &(cm_resp_msg_p->data.sec_appl.
                     nv_pair_array[uiRecCnt].nv_pairs[uiNvCnt]),
                   &uiLen) != OF_SUCCESS)
              {
                CM_JETRANS_DEBUG_PRINT ("Copy Name Value Pairs failed ");
                return OF_FAILURE;
              }
            }
          }
          break;
        case CM_JE_FAILURE:
        {
          CM_JETRANS_DEBUG_PRINT (" JE returned Failured");
          cm_resp_msg_p->result.error.error_code = of_mbuf_get_32 (pUCMTmpRespBuf);
          pUCMTmpRespBuf += uiOSIntSize_g;

          uiLen = 0;
          UCMCopyResultMessageFromBuffer (pUCMTmpRespBuf, cm_resp_msg_p, &uiLen);
          pUCMTmpRespBuf += uiLen;

          uiLen = 0;
          if (UCMCopyDMPathFromBuffer
              (pUCMTmpRespBuf, cm_resp_msg_p, &uiLen) == OF_FAILURE)
          {
            CM_JETRANS_DEBUG_PRINT ("Copy DM Path Failed");
            if (cm_resp_msg_p->result.error.result_string_p)
              of_free (cm_resp_msg_p->result.error.result_string_p);
            return OF_FAILURE;
          }
          pUCMTmpRespBuf += uiLen;
        }
          break;
      }
      break;

    case CM_CMD_GET_DM_NODE_INFO:
    case CM_CMD_GET_DM_FIRST_NODE_INFO:
    case CM_CMD_GET_DM_KEY_CHILD_INFO:
      CM_JETRANS_DEBUG_PRINT (" Copy Response for DM NodeInfo");
      switch (cm_resp_msg_p->result_code)
      {
        case CM_JE_SUCCESS:
          CM_JETRANS_DEBUG_PRINT (" JE returned Success");
          cm_resp_msg_p->data.dm_info.count_ui = of_mbuf_get_32 (pUCMTmpRespBuf);
          pUCMTmpRespBuf += uiOSIntSize_g;

          if (cm_resp_msg_p->data.dm_info.count_ui != 1)
          {
            CM_JETRANS_DEBUG_PRINT (" Invalid Node Count received");
            return OF_FAILURE;
          }

          UCMCopyNodeInfoFromRespBuff (&pUCMTmpRespBuf,
                                       cm_resp_msg_p->data.dm_info.count_ui,
                                       &cm_resp_msg_p->data.dm_info.node_info_p);

          /* Get Opaque Length */
          if (cm_resp_msg_p->sub_command_id == CM_CMD_GET_DM_FIRST_NODE_INFO)
          {
            cm_resp_msg_p->data.dm_info.opaque_info_length = of_mbuf_get_32 (pUCMTmpRespBuf);
            pUCMTmpRespBuf += uiOSIntSize_g;
            if (cm_resp_msg_p->data.dm_info.count_ui == 0)
            {
              CM_JETRANS_DEBUG_PRINT ("Invalid Opaque length received");
              /* Free Node Attribs */
              // of_free(cm_resp_msg_p->data.dm_info.node_info_p);
              return OF_FAILURE;
            }

            UCMCopyOpaqueInfoFromRespBuff (&pUCMTmpRespBuf,
                                           cm_resp_msg_p->data.dm_info.opaque_info_length,
                                           &cm_resp_msg_p->data.
                                           dm_info.opaque_info_p);
          }
          break;
        case CM_JE_FAILURE:
        {
          CM_JETRANS_DEBUG_PRINT ("DM returned Failure");
          cm_resp_msg_p->result.error.error_code = of_mbuf_get_32 (pUCMTmpRespBuf);
          pUCMTmpRespBuf += uiOSIntSize_g;
        }
          break;
      }
      break;
    case CM_CMD_GET_DM_NODE_AND_CHILD_INFO:
    {
      CM_JETRANS_DEBUG_PRINT (" Copy Response for DM NodeInfo");
      switch (cm_resp_msg_p->result_code)
      {
        case CM_JE_SUCCESS:
          cm_resp_msg_p->data.dm_info.count_ui = of_mbuf_get_32 (pUCMTmpRespBuf);
          pUCMTmpRespBuf += uiOSIntSize_g;

          UCMCopyNodeInfoFromRespBuff (&pUCMTmpRespBuf,
                                       cm_resp_msg_p->data.dm_info.count_ui,
                                       &cm_resp_msg_p->data.dm_info.node_info_p);

          cm_resp_msg_p->data.dm_info.node_info_array_p =
            (struct cm_array_of_structs *) of_calloc (1, sizeof (struct cm_array_of_structs));
          if (cm_resp_msg_p->data.dm_info.node_info_array_p == NULL)
          {
            CM_JETRANS_DEBUG_PRINT (" Memory allocation failed");
            /* TODO  Freeing of allocated buffers */
            return OF_FAILURE;
          }

          cm_resp_msg_p->data.dm_info.node_info_array_p->count_ui =
            of_mbuf_get_32 (pUCMTmpRespBuf);
          pUCMTmpRespBuf += uiOSIntSize_g;

          UCMCopyNodeInfoFromRespBuff (&pUCMTmpRespBuf,
                                       cm_resp_msg_p->data.dm_info.
                                       node_info_array_p->count_ui,
                                       (struct cm_dm_node_info **) &
                                       cm_resp_msg_p->data.dm_info.node_info_array_p->
                                       struct_arr_p);
          break;
        case CM_JE_FAILURE:
        {
          CM_JETRANS_DEBUG_PRINT ("DM returned Failure");
          cm_resp_msg_p->result.error.error_code = of_mbuf_get_32 (pUCMTmpRespBuf);
          pUCMTmpRespBuf += uiOSIntSize_g;
        }
      }
      break;
    }
    case CM_CMD_GET_DM_KEYS_ARRAY:
      switch (cm_resp_msg_p->result_code)
      {
        case CM_JE_SUCCESS:
          CM_JETRANS_DEBUG_PRINT (" JE returned Success");
          uiNvCnt = of_mbuf_get_32 (pUCMTmpRespBuf);

          cm_resp_msg_p->data.dm_info.dm_node_nv_pairs_p =
            (struct cm_array_of_nv_pair *) of_calloc (1, sizeof (struct cm_array_of_nv_pair));
          if (cm_resp_msg_p->data.dm_info.dm_node_nv_pairs_p != NULL)
          {
            cm_resp_msg_p->data.dm_info.dm_node_nv_pairs_p->count_ui = uiNvCnt;
            pUCMTmpRespBuf += uiOSIntSize_g;

            cm_resp_msg_p->data.dm_info.dm_node_nv_pairs_p->nv_pairs =
              (struct cm_nv_pair *) of_calloc (uiNvCnt, sizeof (struct cm_nv_pair));
            if (cm_resp_msg_p->data.dm_info.dm_node_nv_pairs_p->nv_pairs == NULL)
            {
              CM_JETRANS_DEBUG_PRINT ("Memory Allocation Failed");
              of_free (cm_resp_msg_p->data.dm_info.dm_node_nv_pairs_p);
              return OF_FAILURE;
            }
            nv_pairs_p = cm_resp_msg_p->data.dm_info.dm_node_nv_pairs_p->nv_pairs;
            /* Copy  Name Value Pair */
            for (ii = 0, uiLen = 0; ii < uiNvCnt;
                 ii++, pUCMTmpRespBuf += uiLen, uiLen = 0)
            {
              if (UCMCopyNvPairFromBuffer
                  (pUCMTmpRespBuf, &nv_pairs_p[ii], &uiLen) == OF_FAILURE)
              {
                CM_JETRANS_DEBUG_PRINT ("UCMCopyNvPairFromBuffer failed");
                CM_FREE_PTR_NVPAIR_ARRAY
                  (cm_resp_msg_p->data.dm_info.dm_node_nv_pairs_p,
                   cm_resp_msg_p->data.dm_info.dm_node_nv_pairs_p->count_ui);
                return OF_FAILURE;
              }
            }
          }
          break;
        case CM_JE_FAILURE:
        {
          CM_JETRANS_DEBUG_PRINT ("DM returned Failure");
          cm_resp_msg_p->result.error.error_code = of_mbuf_get_32 (pUCMTmpRespBuf);
          pUCMTmpRespBuf += uiOSIntSize_g;
        }
      }
      break;

    case CM_CMD_GET_DM_INSTANCE_MAPLIST_BY_NAMEPATH:
      CM_JETRANS_DEBUG_PRINT (" Copy DM InstanceMap");
      switch (cm_resp_msg_p->result_code)
      {
        case CM_JE_SUCCESS:
          cm_resp_msg_p->data.dm_info.count_ui = of_mbuf_get_32 (pUCMTmpRespBuf);
          pUCMTmpRespBuf += uiOSIntSize_g;
          cm_resp_msg_p->data.dm_info.instance_map_list_p =
            (struct cm_dm_instance_map *) of_calloc (cm_resp_msg_p->data.dm_info.count_ui,
                                              sizeof (struct cm_dm_instance_map));
          if (cm_resp_msg_p->data.dm_info.instance_map_list_p == NULL)
          {
            CM_JETRANS_DEBUG_PRINT
              (" Memory allocation failed for InstanceMap");
            return OF_FAILURE;
          }

          for (ii = 0; ii < cm_resp_msg_p->data.dm_info.count_ui; ii++)
          {
            pTempInst = &cm_resp_msg_p->data.dm_info.instance_map_list_p[ii];

            /* Get Instance ID */
            pTempInst->instance_id = of_mbuf_get_32 (pUCMTmpRespBuf);
            pUCMTmpRespBuf = pUCMTmpRespBuf + uiOSIntSize_g;

            /* Get Instance Type */
            pTempInst->key_type = *pUCMTmpRespBuf;
            pUCMTmpRespBuf++;

            /* Get Instance Key Length */
            pTempInst->key_length = of_mbuf_get_32 (pUCMTmpRespBuf);
            pUCMTmpRespBuf = pUCMTmpRespBuf + uiOSIntSize_g;

            /* Get Key value */
            pTempInst->key_p = (char *) of_calloc (1, pTempInst->key_length + 1);
            if (!pTempInst->key_p)
            {
              int32_t j;
              for (j = 0; j < ii; j++)
              {
                of_free (cm_resp_msg_p->data.dm_info.instance_map_list_p[j].key_p);
              }

              of_free (cm_resp_msg_p->data.dm_info.instance_map_list_p);
              return OF_FAILURE;
            }
            of_memcpy (pTempInst->key_p, pUCMTmpRespBuf, pTempInst->key_length);
            pUCMTmpRespBuf = pUCMTmpRespBuf + pTempInst->key_length;
          }
          break;
        case CM_JE_FAILURE:
        {
          CM_JETRANS_DEBUG_PRINT ("DM returned Failure");
          cm_resp_msg_p->result.error.error_code = of_mbuf_get_32 (pUCMTmpRespBuf);
          pUCMTmpRespBuf += uiOSIntSize_g;
        }
      }
      break;

    case CM_CMD_GET_DM_NEXT_NODE_INFO:
      CM_JETRANS_DEBUG_PRINT (" Copy Response for DM GetNextNodeInfo");
      switch (cm_resp_msg_p->result_code)
      {
        case CM_JE_SUCCESS:
          CM_JETRANS_DEBUG_PRINT ("JE returned Success");
          cm_resp_msg_p->data.dm_info.count_ui = of_mbuf_get_32 (pUCMTmpRespBuf);
          pUCMTmpRespBuf += uiOSIntSize_g;
          if (cm_resp_msg_p->data.dm_info.count_ui != 1)
          {
            CM_JETRANS_DEBUG_PRINT (" Invalid Node Count received");
            return OF_FAILURE;
          }

          UCMCopyNodeInfoFromRespBuff (&pUCMTmpRespBuf,
                                       cm_resp_msg_p->data.dm_info.count_ui,
                                       &cm_resp_msg_p->data.dm_info.node_info_p);

          /* Get Opaque Length */
          cm_resp_msg_p->data.dm_info.opaque_info_length = of_mbuf_get_32 (pUCMTmpRespBuf);
          pUCMTmpRespBuf += uiOSIntSize_g;
          if (cm_resp_msg_p->data.dm_info.count_ui == 0)
          {
            CM_JETRANS_DEBUG_PRINT (" Invalid Opaque length received");
            return OF_FAILURE;
          }

          UCMCopyOpaqueInfoFromRespBuff (&pUCMTmpRespBuf,
                                         cm_resp_msg_p->data.dm_info.opaque_info_length,
                                         &cm_resp_msg_p->data.dm_info.opaque_info_p);
          break;
        case CM_JE_FAILURE:
        {
          CM_JETRANS_DEBUG_PRINT ("DM returned Failure");
          cm_resp_msg_p->result.error.error_code = of_mbuf_get_32 (pUCMTmpRespBuf);
          pUCMTmpRespBuf += uiOSIntSize_g;
        }
      }
      break;

    case CM_CMD_IS_DM_INSTANCE_USING_NAMEPATH_AND_KEY:
    case CM_CMD_IS_DM_CHILD_NODE:
      cm_resp_msg_p->result.success.reserved = of_mbuf_get_32 (pUCMTmpRespBuf);
      pUCMTmpRespBuf += uiOSIntSize_g;
      break;

    case CM_CMD_GET_DM_CHILD_NODE_COUNT:
    case CM_CMD_SET_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
    case CM_CMD_GET_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
    case CM_CMD_DEL_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
    case CM_CMD_GET_NEW_DM_INSTANCE_ID:
    case CM_CMD_ADD_DM_INSTANCE_MAP:
    case CM_CMD_DEL_DM_INSTANCE_MAP:
    case CM_CMD_GET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
    case CM_CMD_SET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
    case CM_CMD_DEL_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
      // copy into   cm_resp_msg_p->data.data_modelGet.count_ui:
      break;
  }
  return OF_SUCCESS;
}

/**
  \ingroup TRCHLJEAPI
  This API extracts informations of all the nodes from the response buffer.
  <b>Input Parameters: </b>\n
  <i>pUCMRespBuff:</i> Pointer to the Message buffer received from Transport channel.
  <i>uiNodeCnt:</i> Number of Nodes.
  <b>Output Parameters: </b>\n
  <i>node_info_pp:</i>  Double pointer to the NodeInfo structure.
  <b>Return Value:</b> NONE \n
 **/
void UCMCopyNodeInfoFromRespBuff (char ** pUCMRespBuff,
                                    uint32_t uiNodeCnt,
                                    struct cm_dm_node_info ** node_info_pp)
{
  uint32_t name_length, uiDescLen, uiDMPathLen, uiFrdNameLen=0;
  struct cm_dm_node_info *node_info_p = NULL;
  char *pTmpBuff = *pUCMRespBuff;
  int32_t ii;
  uint32_t uiStrArrLen = 0;
  uint32_t uiStrEnumLen = 0,uiFrdEnumLen = 0,uicFileLen = 0,uiCmdLen = 0,jj = 0,uiIpLen = 0;

  CM_JETRANS_DEBUG_PRINT (" JE returned Success");

  node_info_p =
    (struct cm_dm_node_info *) of_calloc (uiNodeCnt, sizeof (struct cm_dm_node_info));
  if (node_info_p == NULL)
  {
    CM_JETRANS_DEBUG_PRINT (" Memory allocation failed");
    return;
  }

  for (ii = 0; ii < uiNodeCnt; ii++)
  {
    uiStrEnumLen = 0;
    uiFrdEnumLen = 0;
    name_length =0;
    uiFrdNameLen = 0;
    uiDescLen = 0;
    uiDMPathLen = 0;

    name_length = of_mbuf_get_32 (pTmpBuff);
    //pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;
    node_info_p[ii].name_p = (char *) of_calloc (1, name_length + 1);
    if (node_info_p[ii].name_p)
    {
      of_memcpy (node_info_p[ii].name_p, pTmpBuff, name_length);
    }
    pTmpBuff += name_length;

    uiFrdNameLen = of_mbuf_get_32 (pTmpBuff);
    //pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;

    if(uiFrdNameLen > 0)
    {
      node_info_p[ii].friendly_name_p = (char *) of_calloc (1, uiFrdNameLen + 1);
      if (node_info_p[ii].friendly_name_p)
      {
        of_memcpy (node_info_p[ii].friendly_name_p, pTmpBuff, uiFrdNameLen);
      }
    }
    else
    {
      node_info_p[ii].friendly_name_p = NULL;
    }

    pTmpBuff +=uiFrdNameLen;

    uiDescLen = of_mbuf_get_32 (pTmpBuff);
    //pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;
    node_info_p[ii].description_p = (char *) of_calloc (1, uiDescLen + 1);
    if (node_info_p[ii].description_p)
    {
      of_memcpy (node_info_p[ii].description_p,pTmpBuff, uiDescLen);
    }
    pTmpBuff += uiDescLen;
  
    uiDMPathLen = of_mbuf_get_32 (pTmpBuff);
    //pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;

    node_info_p[ii].dm_path_p = (char *) of_calloc (1, uiDMPathLen + 1);
    if (node_info_p[ii].dm_path_p)
    {
      of_memcpy (node_info_p[ii].dm_path_p,pTmpBuff, uiDMPathLen);
    }
    pTmpBuff += uiDMPathLen;

   node_info_p[ii].id_ui = of_mbuf_get_32 (pTmpBuff);
   //pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;

   uicFileLen = of_mbuf_get_32 (pTmpBuff);
   //pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;
  
    {
      of_memcpy (node_info_p[ii].file_name,pTmpBuff,uicFileLen);
    }
    pTmpBuff += uicFileLen;
 
   node_info_p[ii].save_b = *pTmpBuff;
    //CM_JETRANS_DEBUG_PRINT ("save_b :%d",node_info_p[ii].save_b);
   pTmpBuff += sizeof(unsigned char);
/*
** Element Attrib Values
*/
   node_info_p[ii].element_attrib.element_type = ((*pTmpBuff) & 0x7);
   node_info_p[ii].element_attrib.visible_b = (((*pTmpBuff) & 0x8) >> 3);
   node_info_p[ii].element_attrib.key_b = (((*pTmpBuff) & 0x10) >> 4);
   node_info_p[ii].element_attrib.mandatory_b = (((*pTmpBuff) & 0x20) >> 5);
   node_info_p[ii].element_attrib.ordered_table_b = (((*pTmpBuff) & 0x40) >> 6);
   node_info_p[ii].element_attrib.table_priority_b = (((*pTmpBuff) & 0x80) >> 7);
      pTmpBuff += sizeof(unsigned char);
  
   node_info_p[ii].element_attrib.notify_change_b = ((*pTmpBuff) & 0x1);
   node_info_p[ii].element_attrib.notify_attrib = (((*pTmpBuff) & 0xE) >> 1); 
   node_info_p[ii].element_attrib.reboot_on_change_b = (((*pTmpBuff) & 0x10) >> 4); 
   node_info_p[ii].element_attrib.sticky_to_parent_b = (((*pTmpBuff) & 0x20) >> 5); 
   node_info_p[ii].element_attrib.access_controle_b = (((*pTmpBuff) & 0xC0) >> 6); 
      pTmpBuff += sizeof(unsigned char);

   node_info_p[ii].element_attrib.scalar_group_b = ((*pTmpBuff) & 0x1); 
//    CM_JETRANS_DEBUG_PRINT ("scalar Val :%d",node_info_p[ii].element_attrib.scalar_group_b);
   node_info_p[ii].element_attrib.global_trans_b = (((*pTmpBuff) & 0x2) >> 1); 
  //  CM_JETRANS_DEBUG_PRINT ("global trans Val :%d",node_info_p[ii].element_attrib.global_trans_b);
   node_info_p[ii].element_attrib.parent_trans_b = (((*pTmpBuff) & 0x4) >> 2);
    //CM_JETRANS_DEBUG_PRINT ("ParentTrans Val :%d",node_info_p[ii].element_attrib.parent_trans_b);
   node_info_p[ii].element_attrib.system_command_b = (((*pTmpBuff) & 0x8) >> 3); 
   // CM_JETRANS_DEBUG_PRINT ("system_command_b Val :%d",node_info_p[ii].element_attrib.system_command_b);
   node_info_p[ii].element_attrib.stats_command_b = (((*pTmpBuff) & 0x10) >> 4); 
   node_info_p[ii].element_attrib.device_specific_b = (((*pTmpBuff) & 0x20) >> 5); 
   pTmpBuff += sizeof(unsigned char);

   node_info_p[ii].element_attrib.save_in_parent_instance_dir_b = *pTmpBuff;
   pTmpBuff += sizeof(unsigned char);

   node_info_p[ii].element_attrib.non_config_leaf_node_b = *pTmpBuff;
   pTmpBuff += sizeof(unsigned char);
  
   uiCmdLen = of_mbuf_get_32 (pTmpBuff);
   //pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;

   if(uiCmdLen)
   {
     node_info_p[ii].element_attrib.command_name_p = (char *) of_calloc (1, uiCmdLen + 1);
     of_memcpy (node_info_p[ii].element_attrib.command_name_p, pTmpBuff, uiCmdLen);
     pTmpBuff += uiCmdLen;
   }
     
   uiCmdLen = 0;
   uiCmdLen = of_mbuf_get_32 (pTmpBuff);
   //pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;

   if(uiCmdLen)
   {
     node_info_p[ii].element_attrib.cli_identifier_p = (char *) of_calloc (1, uiCmdLen + 1);
     of_memcpy (node_info_p[ii].element_attrib.cli_identifier_p, pTmpBuff, uiCmdLen);
     pTmpBuff += uiCmdLen;
   }
   
   uiCmdLen = 0;
   uiCmdLen = of_mbuf_get_32 (pTmpBuff);
   //pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;
 
   if(uiCmdLen)
   {
     node_info_p[ii].element_attrib.cli_idenetifier_help_p = (char *) of_calloc (1, uiCmdLen + 1);
     of_memcpy (node_info_p[ii].element_attrib.cli_idenetifier_help_p, pTmpBuff, uiCmdLen);
     pTmpBuff += uiCmdLen;
   }

/*
** Data Attrib values 
*/
  
   //node_info_p[ii].data_attrib.data_type = ((*pTmpBuff) & 0xF);
   node_info_p[ii].data_attrib.data_type = *pTmpBuff;
   pTmpBuff += sizeof(unsigned char);
   // CM_JETRANS_DEBUG_PRINT ("data_type :%d",node_info_p[ii].data_attrib.data_type);
   node_info_p[ii].data_attrib.attrib_type = *pTmpBuff;
   // CM_JETRANS_DEBUG_PRINT ("Attrib Type :%d",node_info_p[ii].data_attrib.attrib_type);
   pTmpBuff += sizeof(unsigned char);

  switch(node_info_p[ii].data_attrib.data_type)
  { 
    case CM_DATA_TYPE_INT:
    {
	if(node_info_p[ii].data_attrib.attrib_type == CM_DATA_ATTRIB_INT_RANGE)
	{ 
	  node_info_p[ii].data_attrib.attr.int_range.start_value = of_mbuf_get_32 (pTmpBuff); 
    	  //pTmpBuff += uiInt32Size_g;
    	  pTmpBuff += uiOSIntSize_g;

	  node_info_p[ii].data_attrib.attr.int_range.end_value = of_mbuf_get_32 (pTmpBuff); 
    	  //pTmpBuff += uiInt32Size_g;
    	  pTmpBuff += uiOSIntSize_g;
	}
	else if(node_info_p[ii].data_attrib.attrib_type == CM_DATA_ATTRIB_INT_ENUM)
	{
	  node_info_p[ii].data_attrib.attr.int_enum.count_ui = of_mbuf_get_32 (pTmpBuff);
    	  //pTmpBuff += uiInt32Size_g;
    	  pTmpBuff += uiOSIntSize_g;
	  for(jj = 0 ;jj<node_info_p[ii].data_attrib.attr.int_enum.count_ui;jj++)
	  {
	    node_info_p[ii].data_attrib.attr.int_enum.array[jj] = of_mbuf_get_32 (pTmpBuff);
    	    //pTmpBuff += uiInt32Size_g;
    	    pTmpBuff += uiOSIntSize_g;
	  }
	}
    }
    break;
   case CM_DATA_TYPE_UINT:
   {
	if(node_info_p[ii].data_attrib.attrib_type == CM_DATA_ATTRIB_UINT_RANGE)
	{
          node_info_p[ii].data_attrib.attr.uint_range.start_value = of_mbuf_get_32 (pTmpBuff);
	  //pTmpBuff += uiInt32Size_g;
    	  pTmpBuff += uiOSIntSize_g;
          
          node_info_p[ii].data_attrib.attr.uint_range.end_value = of_mbuf_get_32 (pTmpBuff);
	  //pTmpBuff += uiInt32Size_g;
    	  pTmpBuff += uiOSIntSize_g;
	}
	else if(node_info_p[ii].data_attrib.attrib_type ==CM_DATA_ATTRIB_UINT_ENUM)
	{
	 node_info_p[ii].data_attrib.attr.uint_enum.count_ui = of_mbuf_get_32 (pTmpBuff);
 //	 pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;

         for(jj=0;jj<node_info_p[ii].data_attrib.attr.uint_enum.count_ui;jj++)
         {
	   node_info_p[ii].data_attrib.attr.uint_enum.array[jj] = of_mbuf_get_32 (pTmpBuff);
 //          pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;
	 }

	}
   }
   break;

   case CM_DATA_TYPE_INT64:
    {
	if(node_info_p[ii].data_attrib.attrib_type == CM_DATA_ATTRIB_INT64_RANGE)
	{ 
	  node_info_p[ii].data_attrib.attr.longint_range.start_value = of_mbuf_get_64 (pTmpBuff); 
    	  pTmpBuff += uiOSLongIntSize_g;

	  node_info_p[ii].data_attrib.attr.longint_range.end_value = of_mbuf_get_64 (pTmpBuff); 
    	  pTmpBuff += uiOSLongIntSize_g;
	}
	else if(node_info_p[ii].data_attrib.attrib_type == CM_DATA_ATTRIB_INT64_ENUM)
	{
	  node_info_p[ii].data_attrib.attr.longint_enum.count_ui = of_mbuf_get_64 (pTmpBuff);
    	  pTmpBuff += uiOSLongIntSize_g;
	  for(jj = 0 ;jj<node_info_p[ii].data_attrib.attr.longint_enum.count_ui;jj++)
	  {
	    node_info_p[ii].data_attrib.attr.longint_enum.array[jj] = of_mbuf_get_64 (pTmpBuff);
    	    pTmpBuff += uiOSLongIntSize_g;
	  }
	}
    }
    break;

   case CM_DATA_TYPE_UINT64:
   {
	if(node_info_p[ii].data_attrib.attrib_type == CM_DATA_ATTRIB_UINT64_RANGE)
	{
          node_info_p[ii].data_attrib.attr.ulongint_range.start_value = of_mbuf_get_64 (pTmpBuff);
    	  pTmpBuff += uiOSLongIntSize_g;
          
          node_info_p[ii].data_attrib.attr.ulongint_range.end_value = of_mbuf_get_64 (pTmpBuff);
    	  pTmpBuff += uiOSLongIntSize_g;
	}
	else if(node_info_p[ii].data_attrib.attrib_type ==CM_DATA_ATTRIB_UINT64_ENUM)
	{
	 node_info_p[ii].data_attrib.attr.ulongint_enum.count_ui = of_mbuf_get_64 (pTmpBuff);
         pTmpBuff += uiOSLongIntSize_g;

         for(jj=0;jj<node_info_p[ii].data_attrib.attr.ulongint_enum.count_ui;jj++)
         {
	   node_info_p[ii].data_attrib.attr.ulongint_enum.array[jj] = of_mbuf_get_64 (pTmpBuff);
           pTmpBuff += uiOSLongIntSize_g;
	 }

	}
   }
   break;

   case CM_DATA_TYPE_STRING:
   case CM_DATA_TYPE_STRING_SPECIAL_CHARS:
   {
	if(node_info_p[ii].data_attrib.attrib_type == CM_DATA_ATTRIB_STR_RANGE)
	{
          node_info_p[ii].data_attrib.attr.string_range.min_length = of_mbuf_get_32 (pTmpBuff);
//	  pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;

          node_info_p[ii].data_attrib.attr.string_range.max_length = of_mbuf_get_32 (pTmpBuff);
    //      pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;
	}
	else if(node_info_p[ii].data_attrib.attrib_type == CM_DATA_ATTRIB_STR_ENUM)
	{
	  node_info_p[ii].data_attrib.attr.string_enum.count_ui = of_mbuf_get_32 (pTmpBuff);
   //       pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;
	 for(jj = 0;jj < node_info_p[ii].data_attrib.attr.string_enum.count_ui;jj++)
         {
            uiStrArrLen = 0;
	    uiStrArrLen = of_mbuf_get_32 (pTmpBuff);
  //          pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;
	    node_info_p[ii].data_attrib.attr.string_enum.array[jj] = (char*)calloc(1,uiStrArrLen +1);
            of_memcpy(node_info_p[ii].data_attrib.attr.string_enum.array[jj],pTmpBuff,uiStrArrLen);
            pTmpBuff += uiStrArrLen; 
            uiStrArrLen = 0;
          
            uiStrArrLen = of_mbuf_get_32 (pTmpBuff);
 //           pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;
	    node_info_p[ii].data_attrib.attr.string_enum.aFrdArr[jj] = (char*)calloc(1,uiStrArrLen +1);
            of_memcpy(node_info_p[ii].data_attrib.attr.string_enum.aFrdArr[jj],pTmpBuff,uiStrArrLen);
	     pTmpBuff += uiStrArrLen;
         } 

	}
   }
   break;

   case CM_DATA_TYPE_IPADDR:
   {
     if(node_info_p[ii].data_attrib.attrib_type == CM_DATA_ATTRIB_IPADDR_RANGE)
     {
        uiIpLen = of_mbuf_get_32(pTmpBuff);
//	pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;
        of_memcpy(node_info_p[ii].data_attrib.attr.ip_range.start_ip,pTmpBuff,uiIpLen);
	pTmpBuff += uiIpLen;

	uiIpLen = 0;
        uiIpLen = of_mbuf_get_32(pTmpBuff);
//	pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;
        of_memcpy(node_info_p[ii].data_attrib.attr.ip_range.end_ip,pTmpBuff,uiIpLen);
	pTmpBuff += uiIpLen;
     }	
     else if(node_info_p[ii].data_attrib.attrib_type ==CM_DATA_ATTRIB_IPADDR_ENUM)
     {
	node_info_p[ii].data_attrib.attr.ip_enum.count_ui = of_mbuf_get_32(pTmpBuff);
//	pTmpBuff += uiInt32Size_g;	
    pTmpBuff += uiOSIntSize_g;
	for(jj = 0;jj < node_info_p[ii].data_attrib.attr.ip_enum.count_ui;jj++)
	{
  	  uiIpLen = 0;
	  uiIpLen = of_mbuf_get_32(pTmpBuff);
//	  pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;
	  of_memcpy( node_info_p[ii].data_attrib.attr.ip_enum.ip_arr_a[jj],pTmpBuff,uiIpLen);
	  pTmpBuff += uiIpLen;
	}	

     }
   }
   break;

  case CM_DATA_TYPE_IPV6ADDR:
  {
	if(node_info_p[ii].data_attrib.attrib_type ==CM_DATA_ATTRIB_IPV6ADDR_RANGE)
	{
	  uiIpLen = 0;
	  uiIpLen = of_mbuf_get_32(pTmpBuff); 
  //        pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;
          of_memcpy(node_info_p[ii].data_attrib.attr.ipv6_range.start_ip,pTmpBuff,uiIpLen);
	  pTmpBuff += uiIpLen;

	  uiIpLen = 0;
	  uiIpLen = of_mbuf_get_32(pTmpBuff); 
 //         pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;
          of_memcpy(node_info_p[ii].data_attrib.attr.ipv6_range.end_ip,pTmpBuff,uiIpLen);
	  pTmpBuff += uiIpLen;
	}
	else if(node_info_p[ii].data_attrib.attrib_type == CM_DATA_ATTRIB_IPV6ADDR_ENUM)
	{
	 node_info_p[ii].data_attrib.attr.ipv6_enum.count_ui = of_mbuf_get_32(pTmpBuff);
//	 pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;

         for(jj = 0;jj < node_info_p[ii].data_attrib.attr.ipv6_enum.count_ui;jj++)
         {
  	  uiIpLen = 0;
	  uiIpLen = of_mbuf_get_32(pTmpBuff);
//	  pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;
          of_memcpy(node_info_p[ii].data_attrib.attr.ipv6_enum.ip_v6_arr_a[jj] ,pTmpBuff,uiIpLen);
	  pTmpBuff += uiIpLen;
         }
       }
 }
 break;

  }

  switch(node_info_p[ii].data_attrib.data_type)
  {
   case CM_DATA_TYPE_UNKNOWN:
    {
     node_info_p[ii].data_attrib.attr.max_no_of_tables = of_mbuf_get_32(pTmpBuff);
 //    pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;
    }
    break;
   
   case CM_DATA_TYPE_INT:
   {
        node_info_p[ii].data_attrib.default_value.idefault_value = of_mbuf_get_32(pTmpBuff);
//        pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;
   }   
   break;
   case CM_DATA_TYPE_UINT:
   {
      node_info_p[ii].data_attrib.default_value.uidefault_value = of_mbuf_get_32(pTmpBuff);
//      pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;
   }
   break;
   case CM_DATA_TYPE_INT64:
   {
      node_info_p[ii].data_attrib.default_value.ldefault_value = of_mbuf_get_64(pTmpBuff);
      pTmpBuff += uiOSLongIntSize_g;
   }   
   break;
  
   case CM_DATA_TYPE_UINT64:
   {
      node_info_p[ii].data_attrib.default_value.uldefault_value = of_mbuf_get_64(pTmpBuff);
      pTmpBuff += uiOSLongIntSize_g;
   }
   break;


   case CM_DATA_TYPE_STRING:
   case CM_DATA_TYPE_STRING_SPECIAL_CHARS:
   {
     jj = 0;
     jj = of_mbuf_get_32(pTmpBuff);
//     pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;

     of_memcpy (node_info_p[ii].data_attrib.default_value.default_string,pTmpBuff,jj);
     pTmpBuff += jj;
   }
   break;
  }


  /* if(node_info_p[ii].data_attrib.attrib_type ==CM_DATA_TYPE_INT)
   {
	   node_info_p[ii].data_attrib.default_value.idefault_value = of_mbuf_get_32(pTmpBuff);
	   pTmpBuff += uiOSIntSize_g;
   } 
   else if (node_info_p[ii].data_attrib.attrib_type ==CM_DATA_TYPE_UINT)
   {
      node_info_p[ii].data_attrib.default_value.uidefault_value = of_mbuf_get_32(pTmpBuff);
      pTmpBuff += uiOSIntSize_g;
   }
   else if (node_info_p[ii].data_attrib.attrib_type ==CM_DATA_TYPE_STRING)
   {
    jj = 0;
    jj = of_mbuf_get_32(pTmpBuff);
    pTmpBuff += uiOSIntSize_g;

     of_memcpy (node_info_p[ii].data_attrib.default_value.default_string,pTmpBuff,jj);
     pTmpBuff += jj;
   }*/

  }
 
  *pUCMRespBuff = pTmpBuff;
  *node_info_pp = node_info_p;
}

/**
  \ingroup TRCHLJEAPI
  This API extracts Opaque information from the response buffer.
  <b>Input Parameters: </b>\n
  <i>pUCMRespBuff:</i> Pointer to the Message buffer received from Transport channel.
  <i>opaque_info_length:</i> Length of the Opaque Information.
  <b>Output Parameters: </b>\n
  <i>opaque_info_pp:</i>  Double pointer to hold the Opaque information.
  <b>Return Value:</b> NONE \n
 **/
void UCMCopyOpaqueInfoFromRespBuff (char ** pUCMRespBuff,
                                      uint32_t opaque_info_length,
                                      void ** opaque_info_pp)
{
  int32_t *pIDArray;
  char *pTmpBuff;
  int32_t ii;

  pTmpBuff = *pUCMRespBuff;

  pIDArray = (int32_t *) of_calloc (opaque_info_length, sizeof (int32_t));
  if (pIDArray == NULL)
  {
    CM_JETRANS_DEBUG_PRINT ("Memory allocation failed");
    return;
  }

  for (ii = 0; ii < opaque_info_length; ii++)
  {
    pIDArray[ii] = of_mbuf_get_32 (pTmpBuff);
    //pTmpBuff += uiInt32Size_g;
    pTmpBuff += uiOSIntSize_g;
  }

  *opaque_info_pp = (void *) pIDArray;
  *pUCMRespBuff = pTmpBuff;
}

/**
  \ingroup TRCHLJEAPI
  This API copies a message into the Response buffer's result structure. This typically would be the error message in cases of failure.
  <b>Input Parameters: </b>\n
  <i>pUCMTmpRespBuf:</i>  The message to be copied to the resoponse buffer.
  <b>Output Parameters: </b>\n
  <i>puiLen:</i> This field is incremented by the length of the Message plus size of an integer.
  <i>cm_resp_msg_p:</i>  Response buffer updated with the message.
  <b>Return Value:</b>
  <i>OF_SUCCESS:</i>In Success case
  <i>OF_FAILURE:</i>In Failure case
 **/
int32_t UCMCopyResultMessageFromBuffer (char * pUCMTmpRespBuf,
                                        struct cm_result *
                                        cm_resp_msg_p, uint32_t * puiLen)
{
  uint32_t uiResultMsgLen;

  /* Copy Result Message Lengh */
  uiResultMsgLen = of_mbuf_get_32 (pUCMTmpRespBuf);
//  pUCMTmpRespBuf += uiInt32Size_g;
//  *puiLen += uiInt32Size_g;

  pUCMTmpRespBuf += uiOSIntSize_g;
  *puiLen += uiOSIntSize_g;

  if (uiResultMsgLen > 0)
  {
    cm_resp_msg_p->result.error.result_string_p = (char *)
      of_calloc (1, uiResultMsgLen + 1);
    if (!cm_resp_msg_p->result.error.result_string_p)
    {
      CM_JETRANS_DEBUG_PRINT ("Memory Allocation Failed");
      return OF_FAILURE;
    }
    of_memcpy (cm_resp_msg_p->result.error.result_string_p, pUCMTmpRespBuf,
              uiResultMsgLen);
    *puiLen += uiResultMsgLen;
    CM_JETRANS_DEBUG_PRINT ("Result Message= %s",
                             cm_resp_msg_p->result.error.result_string_p);
  }
  return OF_SUCCESS;
}

/**
  \ingroup TRCHLJEAPI
  This API copies the Data Model Path into the Response buffer's result structure.
  <b>Input Parameters: </b>\n
  <i>pUCMTmpRespBuf:</i>  Buffer with the DM Path to be copied to the resoponse buffer.
  <b>Output Parameters: </b>\n
  <i>puiLen:</i> This field is incremented by the length of the Dm Path plus size of an integer.
  <i>cm_resp_msg_p:</i>  Response buffer updated with the DM Path.
  <b>Return Value:</b>
  <i>OF_SUCCESS:</i>In Success case
  <i>OF_FAILURE:</i>In Failure case
 **/
int32_t UCMCopyDMPathFromBuffer (char * pUCMTmpRespBuf,
                                 struct cm_result * cm_resp_msg_p, uint32_t * puiLen)
{
  uint32_t uiDMPathLen;

  /* Get  DM Path  Length */
  uiDMPathLen = of_mbuf_get_32 (pUCMTmpRespBuf);
  //pUCMTmpRespBuf += uiInt32Size_g;
  //*puiLen += uiInt32Size_g;

  pUCMTmpRespBuf += uiOSIntSize_g;
  *puiLen += uiOSIntSize_g;
  if (uiDMPathLen > 0)
  {
    /* Allocate Memory for DMPath */
    cm_resp_msg_p->result.error.dm_path_p =
      (char *) of_calloc (1, uiDMPathLen + 1);
    if (!cm_resp_msg_p->result.error.dm_path_p)
    {
      CM_JETRANS_DEBUG_PRINT ("Memory Allocatin Failed");
      return OF_FAILURE;
    }
    of_memcpy (cm_resp_msg_p->result.error.dm_path_p, pUCMTmpRespBuf, uiDMPathLen);
    *puiLen += uiDMPathLen;
  }
  return OF_SUCCESS;
}

/**
  \ingroup TRCHLJEAPI
  This API reads the linear request buffer and copies the element attributes and value into a Name Value structure.
  <b>Input Parameters: </b>\n
  <i>pRequestData:</i>  Request Data buffer to be read.
  <b>Output Parameters: </b>\n
  <i>pNvPair:</i> Name Value Pair structure.
  <i>puiLen:</i>  Total length of data read from input buffer.
  <b>Return Value:</b>
  <i>OF_SUCCESS:</i>In Success case
  <i>OF_FAILURE:</i>In Failure case
 **/
int32_t UCMCopyNvPairFromBuffer (char * pRequestData,
                                 struct cm_nv_pair * pNvPair, uint32_t * puiLen)
{

  /* Input Requet Buffer */
  if (pRequestData == NULL)
  {
    CM_JETRANS_DEBUG_PRINT ("Invalid Input Buff");
    return OF_FAILURE;
  }

  /* Output Name Value Pair Structure */
  if (pNvPair == NULL)
  {
    CM_JETRANS_DEBUG_PRINT ("Invalid NV Pair Output ");
    return OF_FAILURE;
  }

  /* Copy Length of Element Name */
  pNvPair->name_length = of_mbuf_get_32 (pRequestData);
//  pRequestData += uiInt32Size_g;
//  *puiLen += uiInt32Size_g;

  pRequestData += uiOSIntSize_g;
  *puiLen += uiOSIntSize_g;
  /* Allocate Memory for Element Name */
  pNvPair->name_p = of_calloc (1, pNvPair->name_length + 1);
  if (!(pNvPair->name_p))
  {
    CM_JETRANS_DEBUG_PRINT ("Memory Allocation Failed for Name Field");
    return OF_FAILURE;
  }
  /* Copy Element Name */
  of_strncpy (pNvPair->name_p, pRequestData, pNvPair->name_length);
  pRequestData += pNvPair->name_length;
  *puiLen += pNvPair->name_length;

  /* Copy Value Type */
  pNvPair->value_type = of_mbuf_get_32 (pRequestData);
  //pRequestData += uiInt32Size_g;
  //*puiLen += uiInt32Size_g;
  pRequestData += uiOSIntSize_g;
  *puiLen += uiOSIntSize_g;


  /* Copy Length of Element Value */
  pNvPair->value_length = of_mbuf_get_32 (pRequestData);
  pRequestData += uiOSIntSize_g;
  *puiLen += uiOSIntSize_g;

  /* Allocate Memory for Element Value */
  pNvPair->value_p = of_calloc (1, pNvPair->value_length + 1);
  if (!(pNvPair->value_p))
  {
    CM_JETRANS_DEBUG_PRINT ("Memory Allocation Failed for Value Field");
    return OF_FAILURE;
  }

  /* Copy Element Value */
  of_strncpy (pNvPair->value_p, pRequestData, pNvPair->value_length);
  pRequestData += pNvPair->value_length;
  *puiLen += pNvPair->value_length;

  CM_PRINT_PTR_NVPAIR (pNvPair);
  return OF_SUCCESS;
}

/**
  \ingroup TRCHLJEAPI
  This API frees the memory associated with an array of Commands.
  <b>Input Parameters: </b>\n
  <i>pCmdArray:</i>  Structure containing the commands.
  <i>count_ui:</i>  Number of commands
  <b>Output Parameters: </b> NONE \n
  <b>Return Value:</b> NONE \n
 **/
void je_free_commandArray (struct cm_array_of_commands * pCmdArray,
                              uint32_t count_ui)
{
  int32_t ii, jj;
  struct cm_command *pTmpCmd;
  struct cm_nv_pair *pTmpNvPair;

  for (ii = 0; ii < count_ui; ii++)
  {
    pTmpCmd = &pCmdArray->Commands[ii];
    if (pTmpCmd->dm_path_p)
    {
      of_free (pTmpCmd->dm_path_p);
    }

    for (jj = 0; jj < pTmpCmd->nv_pair_array.count_ui; jj++)
    {
      pTmpNvPair = &pTmpCmd->nv_pair_array.nv_pairs[ii];
      if (pTmpNvPair->name_p)
        of_free (pTmpNvPair->name_p);
      if (pTmpNvPair->value_p)
        of_free (pTmpNvPair->value_p);
    }
    of_free (pTmpCmd->nv_pair_array.nv_pairs);
  }

  of_free (pCmdArray);

}

/**
  \ingroup TRCHLJEAPI
  This API copies the attributes and value of an element into a linear buffer.
   <b>Input Parameters: </b>\n
   <i>pNvPair:</i>  The Name Value structure.
   <b>Output Parameters: </b>\n
   <i>tmp_buffer_p:</i>  The linear buffer. The caller needs to allocate necessary memory.
   <b>Return Value:</b> None \n
 **/
void cm_je_copy_nvpair_to_buffer (char ** tmp_buffer_p, struct cm_nv_pair * pNvPair)
{
  char *pTmp;
  pTmp = *tmp_buffer_p;
  pTmp = (char *)of_mbuf_put_32 (pTmp, pNvPair->name_length);

  /* Copy Name */
  of_memcpy (pTmp, pNvPair->name_p, pNvPair->name_length);
  pTmp += pNvPair->name_length;

  /* Copy Value type */
  pTmp = (char *)of_mbuf_put_32 (pTmp, pNvPair->value_type);

  /* Copy Value Length */
  pTmp = (char *)of_mbuf_put_32 (pTmp, pNvPair->value_length);

  /* Copy Value */
  if (pNvPair->value_length)
  {
    of_memcpy (pTmp, pNvPair->value_p, pNvPair->value_length);
    pTmp += pNvPair->value_length;

    CM_JETRANS_DEBUG_PRINT
      ("NameLen=%d Name=%s ValueType=%d Len=%d Value=%s",
       pNvPair->name_length,
       pNvPair->name_p,
       pNvPair->value_type, pNvPair->value_length, (char *) pNvPair->value_p);
  }

  *tmp_buffer_p = pTmp;
}

/**
  \ingroup TRCHLJEAPI
  This API copies the complete error information present in the linear input buffer into the UCm Result structure.
  <b>Input Parameters: </b>\n
  <i>pUCMTmpRespBuf:</i>  The linear buffer.
  <b>Output Parameters: </b>\n
  <i>pUCMResult:</i>  UCM Result structure
  <i>puiLen:</i> Number ofbytes read from the input buffer.
  <b>Return Value:</b>
  <i>OF_SUCCESS:</i>In Success case
  <i>OF_FAILURE:</i>In Failure case
 **/
int32_t ucmJECopyErrorInfoFromoBuffer (char * pUCMTmpRespBuf,
                                       struct cm_result * pUCMResult,
                                       uint32_t * puiLen)
{

  uint32_t uiLen = 0;
  uint32_t uiTotLen = 0;

  CM_JETRANS_DEBUG_PRINT ("Copying Error Info");

  pUCMResult->result.error.error_code = of_mbuf_get_32 (pUCMTmpRespBuf);
  pUCMTmpRespBuf += uiOSIntSize_g;
  uiTotLen += uiOSIntSize_g;

  UCMCopyResultMessageFromBuffer (pUCMTmpRespBuf, pUCMResult, &uiLen);
  pUCMTmpRespBuf += uiLen;
  uiTotLen += uiLen;

  uiLen = 0;
  if (UCMCopyDMPathFromBuffer (pUCMTmpRespBuf, pUCMResult, &uiLen) == OF_FAILURE)
  {
    CM_JETRANS_DEBUG_PRINT ("Copy DM Path Failed");
    if (pUCMResult->result.error.result_string_p)
      of_free (pUCMResult->result.error.result_string_p);
    return OF_FAILURE;
  }
  pUCMTmpRespBuf += uiLen;
  uiTotLen += uiLen;

  /* Copy Error Name Value Pair */
  pUCMResult->result.error.nv_pair_b = of_mbuf_get_32 (pUCMTmpRespBuf);
  pUCMTmpRespBuf += uiOSIntSize_g;
  uiTotLen += uiOSIntSize_g;
  if (pUCMResult->result.error.nv_pair_b)
  {
    uiLen = 0;
    if ((UCMCopyNvPairFromBuffer
         (pUCMTmpRespBuf, &(pUCMResult->result.error.nv_pair),
          &uiLen)) != OF_SUCCESS)
    {
      CM_JETRANS_DEBUG_PRINT ("Copy Name Value Pairs failed ");
      if (pUCMResult->result.error.result_string_p)
        of_free (pUCMResult->result.error.result_string_p);
      if (pUCMResult->result.error.dm_path_p)
        of_free (pUCMResult->result.error.dm_path_p);
      return OF_FAILURE;
    }
    pUCMTmpRespBuf += uiLen;
    uiTotLen += uiLen;
  }

  *puiLen = uiTotLen;
  return OF_SUCCESS;
}
#endif /*(CM_TRANSPORT_CHANNEL_SUPPORT && CM_JE_SUPPORT) */
