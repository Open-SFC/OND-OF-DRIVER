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
 * File name:   ldsvwrap.c
 * Author:      Freescale Semiconductor    
 * Date:        06/12/2013
 * Description: 
*/

#ifdef CM_LDSV_SUPPORT

#include "ldsvinc.h"

extern uint32_t uiOSIntSize_g;

int32_t cm_extract_ldsv_response_from_buffer (uint32_t command_id,
                                                   char * pRespBuff,
                                                   uint32_t * puiReqBufLen,
                                                   struct cm_result * pUCMRespMsg);

/**
 \ingroup LDSVAPI
  This API is used to save the Input Data. Save operation is initiated by UCM JE engine. This   process is in 2 ways,
   i)Explicit Save operation
  Administrator issues a “save configuration” command via any management engine.
   ii)Implicit Save operation
  UCM JE detected that there are N pending configuration versions from previous save operation

  LDSV engine acts same for both the above operation.When Save command is received by LDSV 
  engine,  it traverses through the data model elements using DM Path,and for each element it 
  invokes JE APIs to extract the configuration.The retrieved configuration data is then saved
  in to LDSV save configuration file.
 
  Saved configuration file is devided into 3 sections,
  1. Header information - Save file related information like file name, description, dmpath, instance path and version
  2. Dictionary section - Data Attributes for a table, comes from Data Model Attributes.
  3. Data section - Actual admin configuration data.
  Sample saved configuration file as shown below,

  <configuration>
	<header>
		<filename>vsg.xml</filename>
		<description>VSG Table</description>
		<version>1</version>
		<dmpath>igd.vsg</dmpath>
		<instancepath>igd.vsg</instancepath>
	</header>
  <body>
	<record name='vsg' type='1' relative_dmpath='vsg'/>
	<dictionary>
		<a0>vsgid  type=2</a0>
		<a1>nsid  type=2</a1>
		<a2>vsgname  type=1</a2>
	</dictionary>
		<row>
			<a0>0</a0>
			<a1>0</a1>
			<a2>general</a2>
		</row>
  </body>
  </configuration>

  <b>Input paramameters: </b>\n
  <b><i> pTransportChannel: </i></b> Pointer to Transport Channel.
  <b><i> pCommand: </i></b> Pointer to Command.
  <b><i> uiMgmtEngineID: </i></b> Management engine id.
  <b><i> pRoleInfo: </i></b> Pointer to user roles.
  <b>Output Parameters: </b>\n
  <b><i> ppResult: </i></b> Pointer of pointer to the result message.
  <b>Return Value:</b> OF_SUCCESS in Success
         OF_FAILURE in Failure case.\n
 */

/**************************************************************************
 Function Name :cm_ldsv_save_config
 Input Args    :
                pTransportChannel: Pointer to Transport Channel.
                pCommand: Pointer to Command.
                uiMgmtEngineID: Management engine id.
                pRoleInfo:  Pointer to user roles.
 Output Args   :
                ppResult:  Pointer of pointer to the result message.
 Description   :This method is used to save configuration data into persistent store.
 Return Values :OF_SUCCESS/OF_FAILURE
 *************************************************************************/
int32_t cm_ldsv_save_config (void * pTransportChannel,
                            struct cm_command * pCommand,
                            struct cm_result ** ppResult,
                            uint32_t uiMgmtEngineID, struct cm_role_info * pRoleInfo)
{
  unsigned char *pSocketBuf = NULL;
  struct cm_msg_generic_header UCMGenericHeader = { };
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pSaveResult = NULL;
  uint32_t uiMsgLen = 0;

  /* Setting Generic Header Values */
  cm_fill_generic_header (uiMgmtEngineID, CM_COMMAND_LDSV_REQUEST,
                        pRoleInfo, &UCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_LDSVWRAP_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    return OF_FAILURE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg, pCommand->command_id,
                         pCommand->dm_path_p, 0, NULL, NULL, &uiMsgLen);

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiMsgLen);
  if (!pSocketBuf)
  {
    CM_LDSVWRAP_DEBUG_PRINT (" Memory allocation failed");
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (pTransportChannel, &UCMGenericHeader,
                                   pSocketBuf, uiMsgLen)) != OF_SUCCESS)
  {
    CM_LDSVWRAP_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  /*Sending Request Message is done. So freeing it */
  memset (&UCMGenericHeader, 0, sizeof (struct cm_msg_generic_header));

  /*Recive response through Transport Channel */
  uiMsgLen = 0;
  of_free (pSocketBuf);
  if ((cm_tnsprt_recv_message (pTransportChannel,
                                   &UCMGenericHeader, &pSocketBuf,
                                   &uiMsgLen)) == OF_FAILURE)
  {
    CM_LDSVWRAP_DEBUG_PRINT ("cm_tnsprt_recv_message failed");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  /*Memory Allocation for Result Structure */
  (pSaveResult) = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));

  if (pSaveResult == NULL)
  {
    CM_LDSVWRAP_DEBUG_PRINT (" Memory Allocation failed for Result Structure");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);

    return OF_FAILURE;
  }

  /*Fill Response Msg from Recieve Buffer */
  if ((cm_extract_ldsv_response_from_buffer (CM_CMD_SAVE_CONFIG,
                                       pSocketBuf, &uiMsgLen,
                                       pSaveResult)) != OF_SUCCESS)
  {
    CM_LDSVWRAP_DEBUG_PRINT ("Fill Response Msg struct Failed");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  if (pSaveResult->result_code == CM_JE_FAILURE)
  {
    *ppResult = pSaveResult;
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  *ppResult = pSaveResult;
  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);

  return OF_SUCCESS;
}

/**
   \ingroup LDSVAPI
    This API is be used by UCM JE process to send a configuration load request to LDSV daemon. 
    As data model path is supplied as one of the input parameter, it helps LDSV daemon to read the 
    configuration elements and locate their data in Saved configuration file.

    The result of the Load command will be sent on same TCP socket. So to receive the result of the
    Load command, caller has to poll for response on the same transport channel that is used to send 
    the load request.
    Sample saved configuration file as shown below to load,

    <configuration>
  	<header>
		<filename>vsg.xml</filename>
		<description>VSG Table</description>
		<version>1</version>
		<dmpath>igd.vsg</dmpath>
		<instancepath>igd.vsg</instancepath>
	</header>
    <body>
	<record name='vsg' type='1' relative_dmpath='vsg'/>
	<dictionary>
		<a0>vsgid  type=2</a0>
		<a1>nsid  type=2</a1>
		<a2>vsgname  type=1</a2>
	</dictionary>
		<row>
			<a0>0</a0>
			<a1>0</a1>
			<a2>general</a2>
		</row>
    </body>
    </configuration>

   <b>Input paramameters: </b>\n
         <b><i> dm_path_p: </i></b> Pointer to Configuration Data Model Path to Load Configuration
      The complete path of the head of the data model node for which Load Configuration need to be 
      executed. All the child node configurations also gets Loaded.
   <b>Output Parameters: </b>
    <b>Return Value:</b> OF_SUCCESS in Success OF_FAILURE in Failure case.\n
*/
/**************************************************************************
 Function Name :cm_ldsv_load_config
 Input Args    :
                pTransportChannel: Pointer to Transport Channel.
                pCommand: Pointer to Command.
                uiMgmtEngineID: Management engine id.
                pRoleInfo:  Pointer to user roles.
 Output Args   :
                ppResult:  Pointer of pointer to the result message.
 Description   :This method is used to load the configuration data.
 Return Values :OF_SUCCESS/OF_FAILURE
*************************************************************************/
int32_t cm_ldsv_load_config (void * pTransportChannel,
                            struct cm_command * pCommand,
                            struct cm_result ** ppResult,
                            uint32_t uiMgmtEngineID, struct cm_role_info * pRoleInfo)
{
  unsigned char *pSocketBuf = NULL;
  struct cm_msg_generic_header UCMGenericHeader = { };
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pLoadResult = NULL;
  uint32_t uiMsgLen = 0;

  /* Setting Generic Header Values */
  cm_fill_generic_header (uiMgmtEngineID, CM_COMMAND_LDSV_REQUEST,
                        pRoleInfo, &UCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_LDSVWRAP_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    return OF_FAILURE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg, pCommand->command_id,
                         pCommand->dm_path_p, 0, NULL, NULL, &uiMsgLen);

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiMsgLen);
  if (!pSocketBuf)
  {
    CM_LDSVWRAP_DEBUG_PRINT (" Memory allocation failed");
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (pTransportChannel, &UCMGenericHeader,
                                   pSocketBuf, uiMsgLen)) != OF_SUCCESS)
  {
    CM_LDSVWRAP_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  /*Sending Request Message is done. So freeing it */
  memset (&UCMGenericHeader, 0, sizeof (struct cm_msg_generic_header));

  /*Recive response through Transport Channel */
  uiMsgLen = 0;
  of_free (pSocketBuf);
  if ((cm_tnsprt_recv_message (pTransportChannel,
                                   &UCMGenericHeader, &pSocketBuf,
                                   &uiMsgLen)) == OF_FAILURE)
  {
    CM_LDSVWRAP_DEBUG_PRINT ("cm_tnsprt_recv_message failed");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  /*Memory Allocation for Result Structure */
  (pLoadResult) = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));

  if (pLoadResult == NULL)
  {
    CM_LDSVWRAP_DEBUG_PRINT (" Memory Allocation failed for Result Structure");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);

    return OF_FAILURE;
  }

  /*Fill Response Msg from Recieve Buffer */
  if ((cm_extract_ldsv_response_from_buffer (CM_CMD_LOAD_CONFIG,
                                       pSocketBuf, &uiMsgLen,
                                       pLoadResult)) != OF_SUCCESS)
  {
    CM_LDSVWRAP_DEBUG_PRINT ("Fill Response Msg struct Failed");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  if (pLoadResult->result_code == CM_JE_FAILURE)
  {
    *ppResult = pLoadResult;
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  *ppResult = pLoadResult;
  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);

  return OF_SUCCESS;
}

/**
   \ingroup LDSVAPI
   This API is used to set configuration into factory defaults. Factory defaults operation is
  initiated by UCM JE engine.
   
  This API is executed by LDSV daemon when it receives Factory Reset on the LDSV main
  socket. As data model path is supplied as one of the input parameter, it helps LDSV daemon 
  to read the configuration elements and their configuration. 
    The result of the factory reset operation is encoded into generic Transport channel message format and sends on the given TCP socket Fd.

   <b>Input paramameters: </b>\n
         <b><i> dm_path_p: </i></b> Pointer to Configuration Data Model Path to reset factory defaults
      The complete path of the head of the data model node for which factory reset operation need to be executed. All the child node configurations also get configured to factory defaults.
   <b>Output Parameters: </b>
    <b>Return Value:</b> OF_SUCCESS in Success OF_FAILURE in Failure case.\n
*/

/**************************************************************************
 Function Name :cm_ldsv_factory_reset
 Input Args    :
                pTransportChannel: Pointer to Transport Channel.
                pCommand: Pointer to Command.
                uiMgmtEngineID: Management engine id.
                pRoleInfo:  Pointer to user roles.
 Output Args   :
                ppResult:  Pointer of pointer to the result message.
 Description   :This method is used to reset configuration data into default 
                Cofiguration.
 Return Values :OF_SUCCESS/OF_FAILURE
**************************************************************************/
int32_t cm_ldsv_factory_reset (void * pTransportChannel,
                              struct cm_command * pCommand,
                              struct cm_result ** ppResult,
                              uint32_t uiMgmtEngineID,
                              struct cm_role_info * pRoleInfo)
{
  unsigned char *pSocketBuf = NULL;
  struct cm_msg_generic_header UCMGenericHeader = { };
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pFactResult = NULL;
  uint32_t uiMsgLen = 0;


  /* Setting Generic Header Values */
  cm_fill_generic_header (uiMgmtEngineID, CM_COMMAND_LDSV_REQUEST,
                        pRoleInfo, &UCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_LDSVWRAP_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    return OF_FAILURE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg, pCommand->command_id,
                         pCommand->dm_path_p, 0, NULL, NULL, &uiMsgLen);

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiMsgLen);
  if (!pSocketBuf)
  {
    CM_LDSVWRAP_DEBUG_PRINT (" Memory allocation failed");
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (pTransportChannel, &UCMGenericHeader,
                                   pSocketBuf, uiMsgLen)) != OF_SUCCESS)
  {
    CM_LDSVWRAP_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  /*Sending Request Message is done. So freeing it */
  memset (&UCMGenericHeader, 0, sizeof (struct cm_msg_generic_header));

  /*Recive response through Transport Channel */
  uiMsgLen = 0;
  of_free (pSocketBuf);
  if ((cm_tnsprt_recv_message (pTransportChannel,
                                   &UCMGenericHeader, &pSocketBuf,
                                   &uiMsgLen)) == OF_FAILURE)
  {
    CM_LDSVWRAP_DEBUG_PRINT ("cm_tnsprt_recv_message failed");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  /*Memory Allocation for Result Structure */
  (pFactResult) = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));

  if (pFactResult == NULL)
  {
    CM_LDSVWRAP_DEBUG_PRINT (" Memory Allocation failed for Result Structure");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);

    return OF_FAILURE;
  }

  /*Fill Response Msg from Recieve Buffer */
  if ((cm_extract_ldsv_response_from_buffer (CM_CMD_RESET_TO_FACTORY_DEFAULTS,
                                       pSocketBuf, &uiMsgLen,
                                       pFactResult)) != OF_SUCCESS)
  {
    CM_LDSVWRAP_DEBUG_PRINT ("Fill Response Msg struct Failed");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  if (pFactResult->result_code == CM_JE_FAILURE)
  {
    *ppResult = pFactResult;
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  *ppResult = pFactResult;
  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);

  return OF_SUCCESS;
}

/**************************************************************************
 Function Name :cm_ldsv_flush
 Input Args    :
                pTransportChannel: Pointer to Transport Channel.
                pCommand: Pointer to Command.
                uiMgmtEngineID: Management engine id.
                pRoleInfo:  Pointer to user roles.
 Output Args   :
                ppResult:  Pointer of pointer to the result message.
 Description   :This method is used to flush configuration data 
 Return Values :OF_SUCCESS/OF_FAILURE
**************************************************************************/
int32_t cm_ldsv_flush (void * pTransportChannel,
                              struct cm_command * pCommand,
                              struct cm_result ** ppResult,
                              uint32_t uiMgmtEngineID,
                              struct cm_role_info * pRoleInfo)
{
  unsigned char *pSocketBuf = NULL;
  struct cm_msg_generic_header UCMGenericHeader = { };
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pFactResult = NULL;
  uint32_t uiMsgLen = 0;


  /* Setting Generic Header Values */
  cm_fill_generic_header (uiMgmtEngineID, CM_COMMAND_LDSV_REQUEST,
                        pRoleInfo, &UCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_LDSVWRAP_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    return OF_FAILURE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg, pCommand->command_id,
                         pCommand->dm_path_p, 0, NULL, NULL, &uiMsgLen);

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiMsgLen);
  if (!pSocketBuf)
  {
    CM_LDSVWRAP_DEBUG_PRINT (" Memory allocation failed");
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (pTransportChannel, &UCMGenericHeader,
                                   pSocketBuf, uiMsgLen)) != OF_SUCCESS)
  {
    CM_LDSVWRAP_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  /*Sending Request Message is done. So freeing it */
  memset (&UCMGenericHeader, 0, sizeof (struct cm_msg_generic_header));

  /*Recive response through Transport Channel */
  uiMsgLen = 0;
  of_free (pSocketBuf);
  if ((cm_tnsprt_recv_message (pTransportChannel,
                                   &UCMGenericHeader, &pSocketBuf,
                                   &uiMsgLen)) == OF_FAILURE)
  {
    CM_LDSVWRAP_DEBUG_PRINT ("cm_tnsprt_recv_message failed");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  /*Memory Allocation for Result Structure */
  (pFactResult) = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));

  if (pFactResult == NULL)
  {
    CM_LDSVWRAP_DEBUG_PRINT (" Memory Allocation failed for Result Structure");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);

    return OF_FAILURE;
  }

  if ((cm_extract_ldsv_response_from_buffer (CM_CMD_FLUSH_CONFIG,
                                       pSocketBuf, &uiMsgLen,
                                       pFactResult)) != OF_SUCCESS)
  {
    CM_LDSVWRAP_DEBUG_PRINT ("Fill Response Msg struct Failed");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  if (pFactResult->result_code == CM_JE_FAILURE)
  {
    *ppResult = pFactResult;
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  *ppResult = pFactResult;
  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);

  return OF_SUCCESS;
}

/**
 \ingroup LDSVAPI
This API is used to extract the Result information from the received message
buffer. This will take the Message buffer received from Transport channel as
input and tries to extract the expected information from the buffer and fills
the struct cm_result structure with apropriate values.
 
<b>Input Parameters: </b> \n
<b><i>pRespBuff:</i></b> Pointer to the Message buffer received from Transport
                         channel.\n

<b>Output Parameters:</b> \n
<b><i>puiReqBufLen:</i></b> Request buffer length.
<b><i>pRespMsg:</i></b> Pointer to the struct cm_result structure used to hold the
                        extracted information.\n
<b>Return Value:</b>\n
<b><i>OF_SUCCESS:</i></b> On successful extraction of Result structure
information.\n
<b><i>OF_FAILURE:</i></b> On any failure.
*/

/******************************************************************************
 * Function Name : cm_extract_ldsv_response_from_buffer
 * Description   : This API is used to extract the Result structure
 *                 information from the buffer received form Transport
 *                 channel.
 * Input params  : pRespBuff - This will point to the message buffer.
 * Output params : puiReqBufLen - pointer used to hold the request buffer
 *                                length.
 *                 pRespMsg - Pointer to the struct cm_result structure, used to
 *                 hold the extracted information from the message buffer.
 * Return value  : OF_SUCCESS - On successful extraction of Result structure
 *                             information.
 *                 OF_FAILURE - In any failure
 *****************************************************************************/
 int32_t cm_extract_ldsv_response_from_buffer (uint32_t command_id,
                                                   char * pRespBuff,
                                                   uint32_t * puiReqBufLen,
                                                   struct cm_result * pUCMRespMsg)
{
  char *pUCMTmpRespBuf = pRespBuff;
  uint32_t uiLen=0;

  if (pUCMTmpRespBuf == NULL)
  {
    CM_LDSVWRAP_DEBUG_PRINT ("Invalid input Buffer");
    return OF_FAILURE;
  }

  if (puiReqBufLen <= 0)
  {
    CM_LDSVWRAP_DEBUG_PRINT ("Invalid input Buffer Length");
    return OF_FAILURE;
  }

  if (pUCMRespMsg == NULL)
  {
    CM_LDSVWRAP_DEBUG_PRINT ("Invalid Output Response Message");
    return OF_FAILURE;
  }
  /*presuming Generic Header is already extracted. */

  /* Copy Sub Command */
  pUCMRespMsg->sub_command_id = of_mbuf_get_32 (pUCMTmpRespBuf);
  //pUCMTmpRespBuf += CM_UINT32_SIZE;
  pUCMTmpRespBuf += uiOSIntSize_g;

  /* Copy Result code */
  pUCMRespMsg->result_code = of_mbuf_get_32 (pUCMTmpRespBuf);
  //pUCMTmpRespBuf += CM_UINT32_SIZE;
  pUCMTmpRespBuf += uiOSIntSize_g;

  switch (pUCMRespMsg->sub_command_id)
  {
    case CM_CMD_LOAD_CONFIG:
		case CM_CMD_SAVE_CONFIG:
		case CM_CMD_RESET_TO_FACTORY_DEFAULTS:
		case CM_CMD_FLUSH_CONFIG:
			 if (pUCMRespMsg->result_code == CM_JE_FAILURE)
			 {
					if (ucmJECopyErrorInfoFromoBuffer
								(pUCMTmpRespBuf, pUCMRespMsg, &uiLen) != OF_SUCCESS)
					{
						 CM_LDSVWRAP_DEBUG_PRINT
								("Copying Error Info into UCMResult Failed");
						 return OF_FAILURE;
					}
					pUCMTmpRespBuf += uiLen;
			 }
      break;
  }
  return OF_SUCCESS;
}
#endif /*ifdef CM_LDSV_SUPPORT */
