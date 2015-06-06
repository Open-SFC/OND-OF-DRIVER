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
 * File name: jegwrp.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/05/2013
 * Description: 
*/

#ifdef CM_SUPPORT
//#ifdef CM_JE_SUPPORT

#include "jeincld.h"
#include "jewrap.h"

 int32_t UCMExtractApplGetResponseFromBuff (uint32_t command_id,
                                                      char * pRespBuff,
                                                      uint32_t * puiReqBufLen,
                                                      struct cm_result *
                                                      cm_resp_msg_p);
extern uint32_t uiInt32Size_g;
extern uint32_t uiOSIntSize_g;
/* *****************************************************************************
 * * * * * * * * * * * * * FUNCTION DEFINITIONS * * * * * * * * * * * * * * * *
 **************************************************************************** */
/**
  \addtogroup JEGWRPAPI JEG Wrapper APIs
   This API is used to Get First 'N' Records from Security Application. API Frames Configuration Request Message and converts it into Socket Buffer. It sends buffer  on Transport Channel and recieves JE response buffer. It converts response buffer into Configuration Response structure and reads the Records in Nv Pair structre.
   <b>Input Parameters: </b>\n
   <i>tnsprt_channel_p: </i> Pointer to the Transport channel node.
   <i>mgmt_engine_id: </i> Management Engine ID
   <i>admin_role_p: </i> Role of Administrator
   <i>dm_path_p: </i> Pointer to Data Model Path
   <i>keys_array_p: </i> Pointer to Input NV Pair Array
   <b>In/Out Params</b>\n
   <i>pcount_ui: </i> Pointer to Number of Records to fetch and Number of records extracted.
   <b>OutputParams</b>\n
   <i>out_array_of_nv_pair_arr_p:</i> Pointer to Array of Name Value Pairs extracted from Security Application
   <b>Return Value:</b>
   <i>OF_SUCCESS:</i>In Success case
   <i>OF_FAILURE:</i>In Failure case
 **/
int32_t cm_get_first_n_records (void * tnsprt_channel_p,
                             uint32_t mgmt_engine_id,
                             char * admin_role_p,
                             char * dm_path_p,
                             struct cm_array_of_nv_pair * keys_array_p,
                             uint32_t * pcount_ui,
                             struct cm_array_of_nv_pair ** out_array_of_nv_pair_arr_p)
{
  unsigned char *pSocketBuf = NULL;
  struct cm_msg_generic_header UCMGenericHeader = { };
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pUCMResult = NULL;
  struct cm_role_info role_info = { "", "" };
  uint32_t uiMsgLen = 0;

  CM_JETRANS_DEBUG_PRINT ("Entered");

  /* Transport Channel Information */
  if (tnsprt_channel_p == NULL)
  {
    CM_JETRANS_DEBUG_PRINT ("Tranposrt Channel Info is NULL");
    return OF_FAILURE;
  }

  /* Admin Role Information */
  if (admin_role_p == NULL)
  {
    CM_JETRANS_DEBUG_PRINT ("Administrator Role is NULL");
    return OF_FAILURE;
  }
  of_strcpy (role_info.admin_role, admin_role_p);

  /* Setting Generic Header Values */
  cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_APPL_REQUEST,
                        &role_info, &UCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_JETRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    return OF_FAILURE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg, CM_CMD_GET_FIRST_N_RECS,
                         dm_path_p, *pcount_ui, keys_array_p, NULL, &uiMsgLen);

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiMsgLen);
  if (!pSocketBuf)
  {
    CM_JETRANS_DEBUG_PRINT ("Memory allocation failed");
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (tnsprt_channel_p, &UCMGenericHeader,
                                   pSocketBuf, uiMsgLen)) != OF_SUCCESS)

  {
    CM_JETRANS_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pSocketBuf);
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /*Sending Request Message is done. So freeing it */
  of_memset (&UCMGenericHeader, 0, sizeof (struct cm_msg_generic_header));

  /*Recive response through Transport Channel */
  uiMsgLen = 0;
  of_free (pSocketBuf);
  if ((cm_tnsprt_recv_message (tnsprt_channel_p,
                                   &UCMGenericHeader, &pSocketBuf,
                                   &uiMsgLen)) == OF_FAILURE)
  {
    CM_JETRANS_DEBUG_PRINT ("cm_tnsprt_recv_message failed");
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /*Memory Allocation for Response Msg structure */
  pUCMResult = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
  if (!pUCMResult)
  {
    CM_JETRANS_DEBUG_PRINT
      ("Memory allocation failed for UCM Response Message");
    of_free (pSocketBuf);
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /*Fill Response Msg from Recieve Buffer */
  if ((UCMExtractApplGetResponseFromBuff (CM_COMMAND_APPL_REQUEST,
                                          pSocketBuf, &uiMsgLen,
                                          pUCMResult)) != OF_SUCCESS)
  {
    CM_JETRANS_DEBUG_PRINT ("TransportChannelRecv Message Failed");
    UCMFreeUCMResult (pUCMResult);
    of_free (pSocketBuf);
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  if (pUCMResult->result_code == CM_JE_FAILURE)
  {
    /*Deallocating memory */
    UCMFreeUCMResult (pUCMResult);
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  /* Copy Extracted Record Count */
  *pcount_ui = pUCMResult->data.sec_appl.count_ui;
  /* Copy Output Nv Pair Array */
  *out_array_of_nv_pair_arr_p = pUCMResult->data.sec_appl.nv_pair_array;

  /*Deallocating memory */
  UCMFreeUCMResult (pUCMResult);
  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);

  return OF_SUCCESS;
}

/**
  \ingroup JEGWRPAPI
  This API is used to Get Next 'N' Records from Security Application. API Frames Configuration Request Message and converts it into Socket Buffer. It sends buffer  on Transport Channel and recieves JE response buffer. It converts response buffer into Configuration Response structure and reads the Records in Nv Pair structure.
  <b>Input Parameters: </b>\n
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>admin_role_p:</i> Role of Administrator
  <i>dm_path_p:</i> Pointer to Data Model Path
  <i>prev_rec_key_p   - Pointer to Previous Record Key NV Pairs
  <i>keys_array_p:</i> Pointer to Input NV Pair Array
  <b>In/Out Params</b>\n
  <i>pcount_ui:</i> Pointer to Number of Records to fetch and Number of records extracted.
  <b>OutputParams</b>\n
  <i>out_array_of_nv_pair_arr_p:</i> Pointer to Array of Name Value Pairs extracted from Security Application
  <b>Return Value:</b>
  <i>OF_SUCCESS:</i>In Success case
  <i>OF_FAILURE:</i>In Failure case
 **/
int32_t cm_get_next_n_records (void * tnsprt_channel_p,
                            uint32_t mgmt_engine_id,
                            char * admin_role_p,
                            char * dm_path_p,
                            //struct cm_nv_pair * prev_rec_key_p,
                            struct cm_array_of_nv_pair * prev_rec_key_p,
                            struct cm_array_of_nv_pair * keys_array_p,
                            uint32_t * pcount_ui,
                            struct cm_array_of_nv_pair ** out_array_of_nv_pair_arr_p)
{
  unsigned char *pSocketBuf = NULL;
  struct cm_msg_generic_header UCMGenericHeader = { };
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pUCMResult = NULL;
  struct cm_role_info role_info = { };
  uint32_t uiMsgLen = 0;

  CM_JETRANS_DEBUG_PRINT ("Entered");

  /* Transport Channel Information */
  if (tnsprt_channel_p == NULL)
  {
    CM_JETRANS_DEBUG_PRINT ("Tranposrt Channel Info is NULL");
    return OF_FAILURE;
  }

  /* Admin Role Information */
  if (admin_role_p == NULL)
  {
    CM_JETRANS_DEBUG_PRINT ("Administrator Role is NULL");
    return OF_FAILURE;
  }
  of_strcpy (role_info.admin_role, admin_role_p);

  /* Setting Generic Header Values */
  cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_APPL_REQUEST,
                        &role_info, &UCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_JETRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    *pcount_ui = 0;
    return OF_FAILURE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg, CM_CMD_GET_NEXT_N_RECS,
                         dm_path_p, *pcount_ui, keys_array_p, prev_rec_key_p,
                         &uiMsgLen);

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiMsgLen);
  if (!pSocketBuf)
  {
    CM_JETRANS_DEBUG_PRINT ("Memory allocation failed");
    *pcount_ui = 0;
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (tnsprt_channel_p, &UCMGenericHeader,
                                   pSocketBuf, uiMsgLen)) != OF_SUCCESS)

  {
    CM_JETRANS_DEBUG_PRINT ("Sending through transport channel failed");
    *pcount_ui = 0;
    of_free (pSocketBuf);
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /*Sending Request Message is done. So freeing it */
  of_memset (&UCMGenericHeader, 0, sizeof (struct cm_msg_generic_header));

  /*Recive response through Transport Channel */
  uiMsgLen = 0;
  of_free (pSocketBuf);
  if ((cm_tnsprt_recv_message (tnsprt_channel_p,
                                   &UCMGenericHeader, &pSocketBuf,
                                   &uiMsgLen)) == OF_FAILURE)
  {
    CM_JETRANS_DEBUG_PRINT ("cm_tnsprt_recv_message failed");
    *pcount_ui = 0;
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /*Memory Allocation for Response Msg structure */
  pUCMResult = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
  if (!pUCMResult)
  {
    CM_JETRANS_DEBUG_PRINT
      ("Memory allocation failed for UCM Response Message");
    of_free (pSocketBuf);
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /*Fill Response Msg from Recieve Buffer */
  if ((UCMExtractApplGetResponseFromBuff (CM_COMMAND_APPL_REQUEST,
                                          pSocketBuf, &uiMsgLen,
                                          pUCMResult)) != OF_SUCCESS)
  {
    CM_JETRANS_DEBUG_PRINT ("TransportChannelRecv Message Failed");
    UCMFreeUCMResult (pUCMResult);
    of_free (pSocketBuf);
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }
  if (pUCMResult->result_code == CM_JE_FAILURE)
  {
    /*Deallocating memory */
    UCMFreeUCMResult (pUCMResult);
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  /* Copy Output Nv Pair Array */
  *out_array_of_nv_pair_arr_p = pUCMResult->data.sec_appl.nv_pair_array;
  /* Copy Extracted Record Count */
  *pcount_ui = pUCMResult->data.sec_appl.count_ui;

  /*Deallocating memory */
  UCMFreeUCMResult (pUCMResult);
  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);

  return OF_SUCCESS;
}

/**
  \ingroup JEGWRPAPI
  This API is used to Get Specific Record from Security Application. API Frames Configuration Request Message and converts it into Socket Buffer. It sends buffer  on Transport Channel and recieves JE response buffer. It converts response buffer into Configuration Response structure and reads the Records in Nv Pair structre.
   <b>Input Parameters: </b>\n
   <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
   <i>mgmt_engine_id:</i> Management Engine ID
   <i>admin_role_p:</i> Role of Administrator
   <i>dm_path_p:</i> Pointer to Data Model Path
   <i>keys_array_p:</i> Pointer to Input NV Pair Array
   <b>In/Out Params</b>\n
   <i>pcount_ui:</i> Pointer to Number of Records to fetch and Number of records extracted.
   <b>OutputParams</b>\n
   <i>out_array_of_nv_pair_arr_p:</i> Pointer to Array of Name Value Pairs extracted from Security Application.
   <b>Return Value:</b>
   <i>OF_SUCCESS:</i>In Success case
   <i>OF_FAILURE:</i>In Failure case
 **/
int32_t cm_get_exact_record (void * tnsprt_channel_p,
                           uint32_t mgmt_engine_id,
                           char * admin_role_p,
                           char * dm_path_p,
                           struct cm_array_of_nv_pair * keys_array_p,
                           struct cm_array_of_nv_pair ** out_array_of_nv_pair_arr_p)
{
  unsigned char *pSocketBuf = NULL;
  struct cm_msg_generic_header UCMGenericHeader = { };
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pUCMResult = NULL;
  struct cm_role_info role_info = { };
  uint32_t uiMsgLen = 0;

  CM_JETRANS_DEBUG_PRINT ("Entered");

  /* Transport Channel Information */
  if (tnsprt_channel_p == NULL)
  {
    CM_JETRANS_DEBUG_PRINT ("Tranposrt Channel Info is NULL");
    return OF_FAILURE;
  }

  /* Admin Role Information */
  if (admin_role_p == NULL)
  {
    CM_JETRANS_DEBUG_PRINT ("Administrator Role is NULL");
    return OF_FAILURE;
  }
  of_strcpy (role_info.admin_role, admin_role_p);

  /* Setting Generic Header Values */
  cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_APPL_REQUEST,
                        &role_info, &UCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_JETRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    return OF_FAILURE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg, CM_CMD_GET_EXACT_REC,
                         dm_path_p, 1, keys_array_p, NULL, &uiMsgLen);

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiMsgLen);
  if (!pSocketBuf)
  {
    CM_JETRANS_DEBUG_PRINT ("Memory allocation failed");
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (tnsprt_channel_p, &UCMGenericHeader,
                                   pSocketBuf, uiMsgLen)) != OF_SUCCESS)

  {
    CM_JETRANS_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pSocketBuf);
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /*Sending Request Message is done. So freeing it */
  of_memset (&UCMGenericHeader, 0, sizeof (struct cm_msg_generic_header));
  /*Recive response through Transport Channel */
  uiMsgLen = 0;
  of_free (pSocketBuf);

  if ((cm_tnsprt_recv_message (tnsprt_channel_p,
                                   &UCMGenericHeader, &pSocketBuf,
                                   &uiMsgLen)) == OF_FAILURE)
  {
    CM_JETRANS_DEBUG_PRINT ("cm_tnsprt_recv_message failed");
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /*Memory Allocation for Response Msg structure */
  pUCMResult = of_calloc (1, sizeof (struct cm_result));
  if (!pUCMResult)
  {
    CM_JETRANS_DEBUG_PRINT
      ("Memory allocation failed for UCM Response Message");
    of_free (pSocketBuf);
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /*Fill Response Msg from Recieve Buffer */
  if ((UCMExtractApplGetResponseFromBuff (CM_COMMAND_APPL_REQUEST,
                                          pSocketBuf, &uiMsgLen,
                                          pUCMResult)) != OF_SUCCESS)
  {
    CM_JETRANS_DEBUG_PRINT ("TransportChannelRecv Message Failed");
    UCMFreeUCMResult (pUCMResult);
    of_free (pSocketBuf);
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  if (pUCMResult->result_code == CM_JE_FAILURE)
  {
    /*Deallocating memory */
    UCMFreeUCMResult (pUCMResult);
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);

    return OF_FAILURE;
  }

  /* Copy Output Nv Pair Array */
  *out_array_of_nv_pair_arr_p = pUCMResult->data.sec_appl.nv_pair_array;
  /*Deallocating memory */
  UCMFreeUCMResult (pUCMResult);
  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);

  return OF_SUCCESS;
}

#ifdef CM_STATS_COLLECTOR_SUPPORT
int32_t cm_get_stats_records (void * tnsprt_channel_p,
                           uint32_t mgmt_engine_id,
                           char * admin_role_p,
                           char * dm_path_p,
                           char stats_sub_cmd_ui,
                           struct cm_array_of_nv_pair * keys_array_p,
                           uint32_t *rec_count_p,
                           struct cm_array_of_nv_pair ** out_array_of_nv_pair_arr_p)
{
  unsigned char *pSocketBuf = NULL;
  struct cm_msg_generic_header UCMGenericHeader = { };
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pUCMResult = NULL;
  struct cm_role_info role_info = { };
  uint32_t uiMsgLen = 0;

  CM_JETRANS_DEBUG_PRINT ("Entered");

  /* Transport Channel Information */
  if (tnsprt_channel_p == NULL)
  {
    CM_JETRANS_DEBUG_PRINT ("Tranposrt Channel Info is NULL");
    return OF_FAILURE;
  }

  /* Admin Role Information */
  if (admin_role_p == NULL)
  {
    CM_JETRANS_DEBUG_PRINT ("Administrator Role is NULL");
    return OF_FAILURE;
  }
  of_strcpy (role_info.admin_role, admin_role_p);

  /* Setting Generic Header Values */
  cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_STATS_REQUEST,
                          &role_info, &UCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_JETRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    return OF_FAILURE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg, stats_sub_cmd_ui, dm_path_p, 1,
                                keys_array_p, NULL, &uiMsgLen);

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiMsgLen);
  if (!pSocketBuf)
  {
    CM_JETRANS_DEBUG_PRINT ("Memory allocation failed");
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (tnsprt_channel_p, &UCMGenericHeader,
                                   pSocketBuf, uiMsgLen)) != OF_SUCCESS)

  {
    CM_JETRANS_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pSocketBuf);
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /*Sending Request Message is done. So freeing it */
  of_memset (&UCMGenericHeader, 0, sizeof (struct cm_msg_generic_header));
  /*Recive response through Transport Channel */
  uiMsgLen = 0;
  of_free (pSocketBuf);

  if ((cm_tnsprt_recv_message (tnsprt_channel_p,
                                   &UCMGenericHeader, &pSocketBuf,
                                   &uiMsgLen)) == OF_FAILURE)
  {
    CM_JETRANS_DEBUG_PRINT ("cm_tnsprt_recv_message failed");
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /*Memory Allocation for Response Msg structure */
  pUCMResult = of_calloc (1, sizeof (struct cm_result));
  if (!pUCMResult)
  {
    CM_JETRANS_DEBUG_PRINT
      ("Memory allocation failed for UCM Response Message");
    of_free (pSocketBuf);
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /*Fill Response Msg from Recieve Buffer */
  if ((UCMExtractApplGetResponseFromBuff (CM_COMMAND_APPL_REQUEST,
                                          pSocketBuf, &uiMsgLen,
                                          pUCMResult)) != OF_SUCCESS)
  {
    CM_JETRANS_DEBUG_PRINT ("TransportChannelRecv Message Failed");
    UCMFreeUCMResult (pUCMResult);
    of_free (pSocketBuf);
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  if (pUCMResult->result_code == CM_JE_FAILURE)
  {
    /*Deallocating memory */
    UCMFreeUCMResult (pUCMResult);
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);

    return OF_FAILURE;
  }

  /* Copy Output Nv Pair Array */
  *out_array_of_nv_pair_arr_p = pUCMResult->data.sec_appl.nv_pair_array;
  *rec_count_p = pUCMResult->data.sec_appl.count_ui;
  /*Deallocating memory */
  UCMFreeUCMResult (pUCMResult);
  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);

  return OF_SUCCESS;
}
#endif 


/**
  \ingroup JEGWRPAPI
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
 int32_t UCMExtractApplGetResponseFromBuff (uint32_t command_id,
                                                      char * pRespBuff,
                                                      uint32_t * puiReqBufLen,
                                                      struct cm_result * cm_resp_msg_p)
{
  char *pUCMTmpRespBuf = pRespBuff;
  uint32_t uiNvCnt = 0;
  uint32_t uiRecCnt = 0, uiLen = 0;

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
//  pUCMTmpRespBuf += uiInt32Size_g;
  pUCMTmpRespBuf += uiOSIntSize_g;

  /* Copy Result code */
  cm_resp_msg_p->result_code = of_mbuf_get_32 (pUCMTmpRespBuf);
//  pUCMTmpRespBuf += uiInt32Size_g;
  pUCMTmpRespBuf += uiOSIntSize_g;

  switch (cm_resp_msg_p->sub_command_id)
  {
    case CM_CMD_GET_FIRST_N_LOGS:
    case CM_CMD_GET_NEXT_N_LOGS:
    case CM_CMD_GET_FIRST_N_RECS:
    case CM_CMD_GET_NEXT_N_RECS:
    case CM_CMD_GET_EXACT_REC:
#ifdef CM_STATS_COLLECTOR_SUPPORT
    case CM_CMD_GET_AGGREGATE_STATS:
    case CM_CMD_GET_AVERAGE_STATS:
    case CM_CMD_GET_PER_DEVICE_STATS:
    case CM_CMD_COALESCAR_RESP:
#endif
      CM_JETRANS_DEBUG_PRINT (" Copy Response from security applications");
      switch (cm_resp_msg_p->result_code)
      {
        case CM_JE_SUCCESS:
        case CM_JE_NO_MORE_RECS:
        case CM_JE_MORE_RECS:
          CM_JETRANS_DEBUG_PRINT (" JE returned Success");

          /* Number of Records */
          cm_resp_msg_p->data.sec_appl.count_ui = of_mbuf_get_32 (pUCMTmpRespBuf);
//          pUCMTmpRespBuf += uiInt32Size_g;
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
//            pUCMTmpRespBuf += uiInt32Size_g;
            pUCMTmpRespBuf += uiOSIntSize_g;

            /* Allocate Memory nv_pairs */
            cm_resp_msg_p->data.sec_appl.nv_pair_array[uiRecCnt].nv_pairs =
              (struct cm_nv_pair *) of_calloc (cm_resp_msg_p->data.
                                        sec_appl.nv_pair_array[uiRecCnt].count_ui,
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
                   &(cm_resp_msg_p->data.sec_appl.nv_pair_array[uiRecCnt].
                     nv_pairs[uiNvCnt]), &uiLen) != OF_SUCCESS)
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
//          pUCMTmpRespBuf += uiInt32Size_g;
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
    default:
      CM_JETRANS_DEBUG_PRINT ("unhandled command");
      break;

  }
  return OF_SUCCESS;
}


/**
  \ingroup JEGWRPAPI
		This API is used to the First *pcount_ui logs available. This field is updated with the actual number of logs that were fetched.
  <b>Input Parameters: </b>\n
  <i>tnsprt_channel_p: </i> Pointer to the Transport channel node.
  <i>mgmt_engine_id: </i> Management Engine ID
  <i>admin_role_p: </i> Role of Administrator
  <i>dm_path_p: </i> Pointer to Data Model Path
  <i>keys_array_p: </i> Pointer to Input NV Pair Array
  <b>In/Out Params</b>\n
  <i>pcount_ui: </i> Pointer to Number of Records to fetch and Number of records extracted.
  <b>Output Parameters: </b>\n
  <i>out_array_of_nv_pair_arr_p:</i> Pointer to Array of Name Value Pairs extracted from Security Application
		<b>Return Value:</b>\n
		<i>OF_SUCCESS:</i> On successful extraction of Result structure information.
		<i>OF_FAILURE:</i> On any failure.
 **/
int32_t UCMGetFirstNLogs (void * tnsprt_channel_p,
                             uint32_t mgmt_engine_id,
                             char * admin_role_p,
                             char * dm_path_p,
                             struct cm_array_of_nv_pair * keys_array_p,
                             uint32_t * pcount_ui,
                             struct cm_array_of_nv_pair ** out_array_of_nv_pair_arr_p)
{
  unsigned char *pSocketBuf = NULL;
  struct cm_msg_generic_header UCMGenericHeader = { };
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pUCMResult = NULL;
  struct cm_role_info role_info = { "", "" };
  uint32_t uiMsgLen = 0;

  CM_JETRANS_DEBUG_PRINT ("Entered");

  /* Transport Channel Information */
  if (tnsprt_channel_p == NULL)
  {
    CM_JETRANS_DEBUG_PRINT ("Tranposrt Channel Info is NULL");
    return OF_FAILURE;
  }

  /* Admin Role Information */
  if (admin_role_p == NULL)
  {
    CM_JETRANS_DEBUG_PRINT ("Administrator Role is NULL");
    return OF_FAILURE;
  }
  of_strcpy (role_info.admin_role, admin_role_p);

  /* Setting Generic Header Values */
  cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_APPL_REQUEST,
                        &role_info, &UCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_JETRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    return OF_FAILURE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg, CM_CMD_GET_FIRST_N_LOGS,
                         dm_path_p, *pcount_ui, keys_array_p, NULL, &uiMsgLen);

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiMsgLen);
  if (!pSocketBuf)
  {
    CM_JETRANS_DEBUG_PRINT ("Memory allocation failed");
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (tnsprt_channel_p, &UCMGenericHeader,
                                   pSocketBuf, uiMsgLen)) != OF_SUCCESS)

  {
    CM_JETRANS_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pSocketBuf);
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /*Sending Request Message is done. So freeing it */
  of_memset (&UCMGenericHeader, 0, sizeof (struct cm_msg_generic_header));

  /*Recive response through Transport Channel */
  uiMsgLen = 0;
  of_free (pSocketBuf);
  if ((cm_tnsprt_recv_message (tnsprt_channel_p,
                                   &UCMGenericHeader, &pSocketBuf,
                                   &uiMsgLen)) == OF_FAILURE)
  {
    CM_JETRANS_DEBUG_PRINT ("cm_tnsprt_recv_message failed");
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /*Memory Allocation for Response Msg structure */
  pUCMResult = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
  if (!pUCMResult)
  {
    CM_JETRANS_DEBUG_PRINT
      ("Memory allocation failed for UCM Response Message");
    of_free (pSocketBuf);
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /*Fill Response Msg from Recieve Buffer */
  if ((UCMExtractApplGetResponseFromBuff (CM_COMMAND_APPL_REQUEST,
                                          pSocketBuf, &uiMsgLen,
                                          pUCMResult)) != OF_SUCCESS)
  {
    CM_JETRANS_DEBUG_PRINT ("TransportChannelRecv Message Failed");
    UCMFreeUCMResult (pUCMResult);
    of_free (pSocketBuf);
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  if (pUCMResult->result_code == CM_JE_FAILURE)
  {
    /*Deallocating memory */
    UCMFreeUCMResult (pUCMResult);
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  /* Copy Extracted Record Count */
  *pcount_ui = pUCMResult->data.sec_appl.count_ui;
  /* Copy Output Nv Pair Array */
  *out_array_of_nv_pair_arr_p = pUCMResult->data.sec_appl.nv_pair_array;

  /*Deallocating memory */
  UCMFreeUCMResult (pUCMResult);
  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);

  return OF_SUCCESS;
}

/**
  \ingroup JEGWRPAPI
		This API is used to the Next *pcount_ui logs available. This field is updated with the actual number of logs that were fetched.
  <b>Input Parameters: </b>\n
  <i>tnsprt_channel_p: </i> Pointer to the Transport channel node.
  <i>mgmt_engine_id: </i> Management Engine ID
  <i>admin_role_p: </i> Role of Administrator
  <i>dm_path_p: </i> Pointer to Data Model Path
  <i>prev_rec_key_p:</i> Pointer to Previous Rec key from where to fetch thhe next records.
  <i>keys_array_p: </i> Pointer to Input NV Pair Array
  <b>In/Out Params</b>\n
  <i>pcount_ui: </i> Pointer to Number of Records to fetch and Number of records extracted.
  <b>Output Parameters: </b>\n
  <i>out_array_of_nv_pair_arr_p:</i> Pointer to Array of Name Value Pairs extracted from Security Application
		<b>Return Value:</b>\n
		<i>OF_SUCCESS:</i> On successful extraction of Result structure information.
		<i>OF_FAILURE:</i> On any failure.
 **/
int32_t UCMGetNextNLogs (void * tnsprt_channel_p,
                            uint32_t mgmt_engine_id,
                            char * admin_role_p,
                            char * dm_path_p,
                            //struct cm_nv_pair * prev_rec_key_p,
                            struct cm_array_of_nv_pair * prev_rec_key_p,
                            struct cm_array_of_nv_pair * keys_array_p,
                            uint32_t * pcount_ui,
                            struct cm_array_of_nv_pair ** out_array_of_nv_pair_arr_p)
{
  unsigned char *pSocketBuf = NULL;
  struct cm_msg_generic_header UCMGenericHeader = { };
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pUCMResult = NULL;
  struct cm_role_info role_info = { };
  uint32_t uiMsgLen = 0;

  CM_JETRANS_DEBUG_PRINT ("Entered");

  /* Transport Channel Information */
  if (tnsprt_channel_p == NULL)
  {
    CM_JETRANS_DEBUG_PRINT ("Tranposrt Channel Info is NULL");
    return OF_FAILURE;
  }

  /* Admin Role Information */
  if (admin_role_p == NULL)
  {
    CM_JETRANS_DEBUG_PRINT ("Administrator Role is NULL");
    return OF_FAILURE;
  }
  of_strcpy (role_info.admin_role, admin_role_p);

  /* Setting Generic Header Values */
  cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_APPL_REQUEST,
                        &role_info, &UCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_JETRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    return OF_FAILURE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg, CM_CMD_GET_NEXT_N_LOGS,
                         dm_path_p, *pcount_ui, keys_array_p, prev_rec_key_p,
                         &uiMsgLen);

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiMsgLen);
  if (!pSocketBuf)
  {
    CM_JETRANS_DEBUG_PRINT ("Memory allocation failed");
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (tnsprt_channel_p, &UCMGenericHeader,
                                   pSocketBuf, uiMsgLen)) != OF_SUCCESS)

  {
    CM_JETRANS_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pSocketBuf);
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /*Sending Request Message is done. So freeing it */
  of_memset (&UCMGenericHeader, 0, sizeof (struct cm_msg_generic_header));

  /*Recive response through Transport Channel */
  uiMsgLen = 0;
  of_free (pSocketBuf);
  if ((cm_tnsprt_recv_message (tnsprt_channel_p,
                                   &UCMGenericHeader, &pSocketBuf,
                                   &uiMsgLen)) == OF_FAILURE)
  {
    CM_JETRANS_DEBUG_PRINT ("cm_tnsprt_recv_message failed");
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /*Memory Allocation for Response Msg structure */
  pUCMResult = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
  if (!pUCMResult)
  {
    CM_JETRANS_DEBUG_PRINT
      ("Memory allocation failed for UCM Response Message");
    of_free (pSocketBuf);
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /*Fill Response Msg from Recieve Buffer */
  if ((UCMExtractApplGetResponseFromBuff (CM_COMMAND_APPL_REQUEST,
                                          pSocketBuf, &uiMsgLen,
                                          pUCMResult)) != OF_SUCCESS)
  {
    CM_JETRANS_DEBUG_PRINT ("TransportChannelRecv Message Failed");
    UCMFreeUCMResult (pUCMResult);
    of_free (pSocketBuf);
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }
  if (pUCMResult->result_code == CM_JE_FAILURE)
  {
    /*Deallocating memory */
    UCMFreeUCMResult (pUCMResult);
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  /* Copy Output Nv Pair Array */
  *out_array_of_nv_pair_arr_p = pUCMResult->data.sec_appl.nv_pair_array;
  /* Copy Extracted Record Count */
  *pcount_ui = pUCMResult->data.sec_appl.count_ui;

  /*Deallocating memory */
  UCMFreeUCMResult (pUCMResult);
  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);

  return OF_SUCCESS;
}


//#endif /* CM_JE_SUPPORT */
#endif /* CM_SUPPORT */
