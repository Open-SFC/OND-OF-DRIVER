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
 * File name: jewrpapi.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/05/2013
 * Description: 
*/

#ifdef CM_SUPPORT
//#ifdef CM_JE_SUPPORT

#include "jeincld.h"
#include "jewrap.h"

extern uint32_t uiInt32Size_g;
extern uint32_t uiInt64Size_g;
extern uint32_t uiOSIntSize_g;
/*
 *****************************************************************************
 * * * * * * * * * * * * * * * FUNCTION PROTOTYPES * * * * * * * * * * * * * *
 *****************************************************************************
 */
 int32_t UCMExtractSessionResponseFromBuff (uint32_t command_id,
      char * pRespBuff,
      uint32_t * puiReqBufLen,
      struct cm_result *
      cm_resp_msg_p);
/*
 *****************************************************************************
 * * * * * * * * * * * * * FUNCTION DEFINITIONS * * * * * * * * * * * * * * * *
 *****************************************************************************
 */

/*
   AT THE FIRST INSTANCE OF A GROUP NAME, ALWAYS USE /addtogroup <group_name>
   IN THE SUBSEQUENT INTANCES, USE /ingroup <group_name>
   */

/*
   AVOID NEW LINE CHARACTER IN THE DESCRIPTION.
   */

/**
  \addtogroup JEWRPAPI JE Wrapper APIs
  This API is used to Start a Configuration Session for update Security Application Database. API Frames Configuration Request Message and converts it into Socket Buffer to sent on Transport Channel. This API recieves JE response buffer,converts into Configuration Response structure and reads Configuration Session.
  <b>Input Parameters: </b>\n
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>role_info_p:</i> Pointer to  Role of Administrator
  <b>Return Value:</b>
  <i></i>Pointer to Configuration Session in SUCCESS case or NULL pointer in FAILURE case.\n
 **/
void *cm_config_session_start (uint32_t mgmt_engine_id,
      struct cm_role_info * role_info_p,
      void * tnsprt_channel_p)
{
   struct cm_je_config_session *pJEConfigSession = NULL;
   unsigned char *pSocketBuf = NULL;
   struct cm_msg_generic_header UCMGenericHeader = { };
   struct cm_request_msg *pUCMRequestMsg = NULL;
   struct cm_result *pUCMResponseMsg = NULL;
   uint32_t uiMsgLen = 0;

   CM_JETRANS_DEBUG_PRINT ("Entered");

   /* Administror Role Information */
   if (role_info_p == NULL)
   {
      CM_JETRANS_DEBUG_PRINT ("ROLE Info is NULL");
      return NULL;
   }
   /* Setting Generic Header Values */
   cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_CONFIG_SESSION_START,
         role_info_p, &UCMGenericHeader);

   /* Memory Allocation for Request Message */
   pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
   if (pUCMRequestMsg == NULL)
   {
      CM_JETRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");

      return NULL;
   }

   /*Fill Request Message Structure */
   cm_fill_request_message (pUCMRequestMsg, 0, NULL, 0, NULL, NULL, &uiMsgLen);

   /* Allocate Memory for Send Buffer */
   pSocketBuf = (unsigned char *) of_calloc (1, uiMsgLen);
   if (!pSocketBuf)
   {
      CM_JETRANS_DEBUG_PRINT ("Memory allocation failed");
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      return NULL;
   }

   /* Convert Request Message Structure into flat Buffer */
   cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

   /*Send Message through Transport Channel */
   if ((cm_tnsprt_send_message (tnsprt_channel_p, &UCMGenericHeader,
               pSocketBuf, uiMsgLen)) != OF_SUCCESS)

   {
      CM_JETRANS_DEBUG_PRINT ("Sending through transport channel failed");
      of_free (pSocketBuf);

      UCMJEFreeRequestMsg (pUCMRequestMsg);
      return NULL;
   }
   of_free (pSocketBuf);
   uiMsgLen = 0;
   pSocketBuf = NULL;
   of_memset (&UCMGenericHeader, 0, sizeof (struct cm_msg_generic_header));
   /*Recive response through Transport Channel */
   if ((cm_tnsprt_recv_message (tnsprt_channel_p,
               &UCMGenericHeader, &pSocketBuf,
               &uiMsgLen)) == OF_FAILURE)
   {
      CM_JETRANS_DEBUG_PRINT ("cm_tnsprt_recv_message failed");
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      return NULL;
   }

   /*Memory Allocation for Response Msg structure */
   pUCMResponseMsg = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
   if (!pUCMResponseMsg)
   {
      CM_JETRANS_DEBUG_PRINT ("Memory allocation failed");
      of_free (pSocketBuf);
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      return NULL;
   }

   /*Fill Response Msg from Recieve Buffer */
   if ((UCMExtractSessionResponseFromBuff (CM_COMMAND_CONFIG_SESSION_START,
               pSocketBuf, &uiMsgLen,
               pUCMResponseMsg)) != OF_SUCCESS)
   {
      CM_JETRANS_DEBUG_PRINT ("TransportChannelRecv Message Failed");
      of_free (pSocketBuf);
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      UCMFreeUCMResult (pUCMResponseMsg);
      return NULL;
   }

   if (pUCMResponseMsg)
   {
      /*Fill Configuration Session Values */
      if (pUCMResponseMsg->result_code == CM_JE_FAILURE)
      {
         CM_JETRANS_DEBUG_PRINT ("JE returned Failure");
         UCMJEFreeRequestMsg (pUCMRequestMsg);
         UCMFreeUCMResult (pUCMResponseMsg);
         /* free pUCMResponseMsg */
         of_free (pSocketBuf);
         return NULL;
      }
      pJEConfigSession = (struct cm_je_config_session *) pUCMResponseMsg->data.je_data_p;
      /*Store Transport Chanel information in ConfigSession
       * for future use*/
      pJEConfigSession->tnsprt_channel_p = tnsprt_channel_p;
      CM_JETRANS_DEBUG_PRINT
         ("session id=%d MgmtId=%d Role=%s Name=%s ",
          pJEConfigSession->session_id, pJEConfigSession->mgmt_engine_id,
          pJEConfigSession->admin_role, pJEConfigSession->admin_name);
   }

   UCMJEFreeRequestMsg (pUCMRequestMsg);
   /* free pUCMResponseMsg */
   of_free (pSocketBuf);

   UCMFreeUCMResult (pUCMResponseMsg);
   return (void *) pJEConfigSession;
}

/*
   AVOID NEW LINE CHARACTER IN THE DESCRIPTION.
   */

/**
  \ingroup JEWRPAPI
  This API Sends a set of commands information through JE to  update Security Application Database. API Frames Configuration Request Message using config_session_p and command_p structures and converts it into Socket Buffer to sent on Transport Channel. This API recieves JE response buffer,converts into Configuration Response structure and reads Success or Failure Information.
  <b>Input Parameters: </b>\n
  <i>config_session_p:</i> Pointer to JE Confiration Session Structure.
  <i>command_p:</i> Pointer to Command structure which contains Command ID, Data Model Path and Name Value Pairs.
  <i>count_i</i> Number of commands
  <b>Output Parameters: </b>\n
  <i>presult_p:</i> Doutble  Pointer to  struct cm_result which contains Success or Failure information.
  <b>Return Value:</b>
  <i>OF_SUCCESS</i> On success.
  <i>OF_FAILURE</i> On failure.\n
 **/
int32_t cm_config_session_update_cmds (void * config_session_p,
      struct cm_command *command_p,
      int32_t count_i,
      struct cm_result ** presult_p)
{
   int32_t ii;

   for(ii = 0; ii < count_i; ii++)
   {
      if(cm_config_session_update_cmd (config_session_p,&command_p[ii],presult_p)!= OF_SUCCESS)
      {
         return OF_FAILURE;
      }
   }
   return OF_SUCCESS;
}

/*
   AVOID NEW LINE CHARACTER IN THE DESCRIPTION.
   */

/**
  \ingroup JEWRPAPI
  This API is Sends Command Information through JE to  update Security Application Database. API Frames Configuration Request Message using config_session_p and command_p structures and converts it into Socket Buffer to sent on Transport Channel. This API recieves JE response buffer,converts into Configuration Response structure and reads Success or Failure Information.
  <b>Input Parameters: </b>\n
  <i>config_session_p:</i> Pointer to JE Confiration Session Structure.
  <i>command_p:</i> Pointer to Command structure which contains Command ID, Data Model Path and Name Value Pairs.
  <b>Output Parameters: </b>\n
  <i>presult_p:</i> Doutble  Pointer to  struct cm_result which contains Success or Failure information.
  <b>Return Value:</b>
  <i>OF_SUCCESS</i> On success.
  <i>OF_FAILURE</i> On failure.\n
 **/
int32_t cm_config_session_update_cmd (void * config_session_p,
      struct cm_command * command_p,
      struct cm_result ** presult_p)
{
   struct cm_je_config_session *pJEConfigSession = (struct cm_je_config_session *) config_session_p;
   unsigned char *pSocketBuf = NULL;
   struct cm_msg_generic_header UCMGenericHeader = { };
   struct cm_request_msg *pUCMRequestMsg = NULL;
   struct cm_role_info role_info = { };
   struct cm_result *pUpdateResult = NULL;
   uint32_t uiMsgLen = 0;

   CM_JETRANS_DEBUG_PRINT ("Entered");

   /* Configuration Session Information */
   if (pJEConfigSession == NULL)
   {
      CM_JETRANS_DEBUG_PRINT ("Invalid ConfigSession");
      return OF_FAILURE;
   }

   /* Administror Role Information */
   CM_JETRANS_DEBUG_PRINT ("Role =%s",pJEConfigSession->admin_role);
   of_strncpy (role_info.admin_role, pJEConfigSession->admin_role,
         of_strlen (pJEConfigSession->admin_role));
   of_strncpy (role_info.admin_name, pJEConfigSession->admin_name,
         of_strlen (pJEConfigSession->admin_name));

   CM_JETRANS_DEBUG_PRINT ("session id=%u MgmtId=%u Role=%s Name=%s ",
         pJEConfigSession->session_id,
         pJEConfigSession->mgmt_engine_id,
         pJEConfigSession->admin_role,
         pJEConfigSession->admin_name);

   /* Setting Generic Header Values */
   cm_fill_generic_header (pJEConfigSession->mgmt_engine_id,
         CM_COMMAND_CONFIG_SESSION_UPDATE,
         &role_info, &UCMGenericHeader);

   /* Memory Allocation for Request Message */
   pUCMRequestMsg = of_calloc (1, sizeof (struct cm_request_msg));
   if (pUCMRequestMsg == NULL)
   {
      CM_JETRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");

      return OF_FAILURE;
   }

   /*Fill Request Message Structure */
   cm_fill_request_message (pUCMRequestMsg, command_p->command_id,
         command_p->dm_path_p, 0, &(command_p->nv_pair_array), NULL,
         &uiMsgLen);

   /* Allocate Memory for Send Buffer */
   pSocketBuf = (unsigned char *) of_calloc (1, uiMsgLen);
   if (!pSocketBuf)
   {
      CM_JETRANS_DEBUG_PRINT (" Memory allocation failed");

      UCMJEFreeRequestMsg (pUCMRequestMsg);
      return OF_FAILURE;
   }

   /* Convert Request Message Structure into flat Buffer */
   cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

   /*Send Message through Transport Channel */
   if ((cm_tnsprt_send_message (pJEConfigSession->tnsprt_channel_p,
               &UCMGenericHeader, pSocketBuf,
               uiMsgLen)) != OF_SUCCESS)

   {
      CM_JETRANS_DEBUG_PRINT ("Sending through transport channel failed");

      UCMJEFreeRequestMsg (pUCMRequestMsg);
      of_free (pSocketBuf);
      return OF_FAILURE;
   }

   /*Sending Request Message is done. So freeing it */
   of_memset (&UCMGenericHeader, 0, sizeof (struct cm_msg_generic_header));

   /*Recive response through Transport Channel */
   of_free (pSocketBuf);
   uiMsgLen = 0;
   if ((cm_tnsprt_recv_message
            ((pJEConfigSession->tnsprt_channel_p),
             &UCMGenericHeader, &pSocketBuf, &uiMsgLen)) == OF_FAILURE)
   {
      CM_JETRANS_DEBUG_PRINT ("cm_tnsprt_recv_message failed");
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      return OF_FAILURE;
   }

   /*Memory Allocation for Result Structure */
   pUpdateResult = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
   if (pUpdateResult == NULL)
   {
      CM_JETRANS_DEBUG_PRINT (" Memory Allocation failed for Result Structure");
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      of_free (pSocketBuf);
      return OF_FAILURE;
   }

   /*Fill Response Msg from Recieve Buffer */
   if ((UCMExtractSessionResponseFromBuff
            (CM_COMMAND_CONFIG_SESSION_UPDATE, pSocketBuf, &uiMsgLen,
             pUpdateResult)) != OF_SUCCESS)
   {
      CM_JETRANS_DEBUG_PRINT ("Fill Response Msg struct Failed");
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      UCMFreeUCMResult (pUpdateResult);
      of_free (pSocketBuf);
      return OF_FAILURE;
   }

   if (pUpdateResult->result_code == CM_JE_FAILURE)
   {
      CM_JETRANS_DEBUG_PRINT ("JE returned failure for Config Update");
      of_free (pSocketBuf);
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      *presult_p = pUpdateResult;
      return OF_FAILURE;
   }

   UCMJEFreeRequestMsg (pUCMRequestMsg);
   of_free (pSocketBuf);
   *presult_p = pUpdateResult;
   return OF_SUCCESS;
}

/**
  \ingroup JEWRPAPI
  This API calls state machine handler and fetchs the Pointer to Configuration Session
  <b>Input Parameters: </b>\n
  <i>config_session_p:</i> Pointer to JE Confiration Session Structure.
  <i>command_p:</i> Pointer to Command structure which contains Command ID, Data Model Path and Name Value Pairs.
  <b>Output Parameters: </b>\n
  <i>presult_p:</i> Doutble  Pointer to  struct cm_result which contains Success or Failure information.
  <b>Return Value:</b>
  <i>OF_SUCCESS</i> On success.
  <i>OF_FAILURE</i> On failure.\n
 **/
int32_t cm_config_session_get_cmd (void * config_session_p,
      struct cm_command * command_p,
      struct cm_result ** presult_p)
{
   struct cm_je_config_session *pJEConfigSession = (struct cm_je_config_session *) config_session_p;
   unsigned char *pSocketBuf = NULL;
   struct cm_msg_generic_header UCMGenericHeader = { };
   struct cm_request_msg *pUCMRequestMsg = NULL;
   struct cm_role_info role_info = { };
   uint32_t uiMsgLen = 0;
   struct cm_result *pGetResult = NULL;

   CM_JETRANS_DEBUG_PRINT ("Entered");

   /* Configuration Session Information */
   if (pJEConfigSession == NULL)
   {
      CM_JETRANS_DEBUG_PRINT ("Invalid ConfigSession");
      return OF_FAILURE;
   }

   /* Administror Role Information */
   of_strncpy (role_info.admin_role, pJEConfigSession->admin_role,
         of_strlen (pJEConfigSession->admin_role));
   of_strncpy (role_info.admin_name, pJEConfigSession->admin_name,
         of_strlen (pJEConfigSession->admin_name));

   CM_JETRANS_DEBUG_PRINT ("session id=%u MgmtId=%u Role=%s Name=%s ",
         pJEConfigSession->session_id,
         pJEConfigSession->mgmt_engine_id,
         pJEConfigSession->admin_role,
         pJEConfigSession->admin_name);

   /* Setting Generic Header Values */
   cm_fill_generic_header (pJEConfigSession->mgmt_engine_id,
         CM_COMMAND_CONFIG_SESSION_GET,
         &role_info, &UCMGenericHeader);

   /* Memory Allocation for Request Message */
   pUCMRequestMsg = of_calloc (1, sizeof (struct cm_request_msg));
   if (pUCMRequestMsg == NULL)
   {
      CM_JETRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");

      return OF_FAILURE;
   }

   /*Fill Request Message Structure */
   cm_fill_request_message (pUCMRequestMsg, command_p->command_id,
         command_p->dm_path_p, 0, &(command_p->nv_pair_array),
         NULL, &uiMsgLen);

   /* Allocate Memory for Send Buffer */
   pSocketBuf = (unsigned char *) of_calloc (1, uiMsgLen);
   if (!pSocketBuf)
   {
      CM_JETRANS_DEBUG_PRINT (" Memory allocation failed");

      UCMJEFreeRequestMsg (pUCMRequestMsg);
      return OF_FAILURE;
   }

   /* Convert Request Message Structure into flat Buffer */
   cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

   /*Send Message through Transport Channel */
   if ((cm_tnsprt_send_message (pJEConfigSession->tnsprt_channel_p,
               &UCMGenericHeader, pSocketBuf,
               uiMsgLen)) != OF_SUCCESS)

   {
      CM_JETRANS_DEBUG_PRINT ("Sending through transport channel failed");

      UCMJEFreeRequestMsg (pUCMRequestMsg);
      of_free (pSocketBuf);
      return OF_FAILURE;
   }

   /*Sending Request Message is done. So freeing it */
   of_memset (&UCMGenericHeader, 0, sizeof (struct cm_msg_generic_header));

   /*Recive response through Transport Channel */
   of_free (pSocketBuf);
   uiMsgLen = 0;
   if ((cm_tnsprt_recv_message
            (pJEConfigSession->tnsprt_channel_p,
             &UCMGenericHeader, &pSocketBuf, &uiMsgLen)) == OF_FAILURE)
   {
      CM_JETRANS_DEBUG_PRINT ("cm_tnsprt_recv_message failed");
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      return OF_FAILURE;
   }

   /*Memory Allocation for Result Structure */
   pGetResult = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
   if (pGetResult == NULL)
   {

      CM_JETRANS_DEBUG_PRINT (" Memory Allocation failed for Result Structure");

      UCMJEFreeRequestMsg (pUCMRequestMsg);
      of_free (pSocketBuf);
      return OF_FAILURE;
   }

   /*Fill Response Msg from Recieve Buffer */
   if ((UCMExtractSessionResponseFromBuff
            (CM_COMMAND_CONFIG_SESSION_GET, pSocketBuf, &uiMsgLen,
             pGetResult)) != OF_SUCCESS)
   {
      CM_JETRANS_DEBUG_PRINT ("Fill Response Msg struct Failed");
      of_free (pGetResult);
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      of_free (pSocketBuf);
      return OF_FAILURE;
   }

   if (pGetResult->result_code == CM_JE_FAILURE)
   {
      /*Deallocating memory */
      of_free (pGetResult);
      of_free (pUCMRequestMsg);
      of_free (pSocketBuf);

      return OF_FAILURE;
   }

   UCMJEFreeRequestMsg (pUCMRequestMsg);
   of_free (pSocketBuf);

   /* Copy the output */
   *presult_p = pGetResult;
   return OF_SUCCESS;
}

/*
   AVOID NEW LINE CHARACTER IN THE DESCRIPTION.
   */

/**
  \ingroup JEWRPAPI
  This API makes changes done by configuration session permanet to security applications or undo all changes and closes configuration session. API Frames Configuration Request Message using config_session_p and REVOKE/COMMIT method and converts it into Socket Buffer to sent on Transport Channel. This API recieves JE response buffer,converts into Configuration Response structure and reads Success or Failure Information.
  <b>Input Parameters: </b>\n
  <i>config_session_p:</i> Pointer to JE Confiration Session Structure.
  <i>operation_ui:</i> COMMIT or REVOKE  method
  <b>Output Parameters: </b>\n
  <i>presult_p:</i> Doutble  Pointer to  struct cm_result which contains Success or Failure information.
  <b>Return Value:</b>
  <i>OF_SUCCESS</i> On success.
  <i>OF_FAILURE</i> On failure.\n
 **/
int32_t cm_config_session_end (void * config_session_p, uint32_t operation_ui,
      struct cm_result ** presult_p)
{
   struct cm_je_config_session *pJEConfigSession =
      (struct cm_je_config_session *) config_session_p;
   unsigned char *pSocketBuf = NULL;
   struct cm_msg_generic_header UCMGenericHeader = { };
   struct cm_role_info role_info = { };
   struct cm_request_msg *pUCMRequestMsg = NULL;
   struct cm_result *pSessionEndResult = NULL;
   uint32_t uiMsgLen = 0;

   CM_JETRANS_DEBUG_PRINT ("Entered");

   /* Configuration Session Information */
   if (pJEConfigSession == NULL)
   {
      CM_JETRANS_DEBUG_PRINT ("ROLE Info is OF_FAILURE");
      return OF_FAILURE;
   }

   /* Administror Role Information */
   of_strncpy (role_info.admin_role, pJEConfigSession->admin_role,
         of_strlen (pJEConfigSession->admin_role));
   of_strncpy (role_info.admin_name, pJEConfigSession->admin_name,
         of_strlen (pJEConfigSession->admin_name));

   CM_JETRANS_DEBUG_PRINT ("session id=%d MgmtId=%d Role=%s Name=%s ",
         pJEConfigSession->session_id,
         pJEConfigSession->mgmt_engine_id,
         pJEConfigSession->admin_role,
         pJEConfigSession->admin_name);

   /* Setting Generic Header Values */
   cm_fill_generic_header (pJEConfigSession->mgmt_engine_id,
         CM_COMMAND_CONFIG_SESSION_END,
         &role_info, &UCMGenericHeader);

   /* Memory Allocation for Request Message */
   pUCMRequestMsg = of_calloc (1, sizeof (struct cm_request_msg));
   if (pUCMRequestMsg == NULL)
   {
      CM_JETRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");

      return OF_FAILURE;
   }
   /*Fill Request Message Structure */
   cm_fill_request_message (pUCMRequestMsg, operation_ui, NULL, 0, NULL, NULL,
         &uiMsgLen);

   /* Allocate Memory for Send Buffer */
   pSocketBuf = (unsigned char *) of_calloc (1, uiMsgLen);
   if (!pSocketBuf)
   {
      CM_JETRANS_DEBUG_PRINT ("Memory allocation failed");

      UCMJEFreeRequestMsg (pUCMRequestMsg);
      return OF_FAILURE;
   }

   /* Convert Request Message Structure into flat Buffer */
   cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

   /*Send Message through Transport Channel */
   if ((cm_tnsprt_send_message
            (pJEConfigSession->tnsprt_channel_p, &UCMGenericHeader, pSocketBuf,
             uiMsgLen)) != OF_SUCCESS)
   {
      CM_JETRANS_DEBUG_PRINT ("Sending through transport channel failed");

      UCMJEFreeRequestMsg (pUCMRequestMsg);
      of_free (pSocketBuf);
      return OF_FAILURE;
   }

   /*Sending Request Message is done. So freeing it */
   of_memset (&UCMGenericHeader, 0, sizeof (struct cm_msg_generic_header));

   /*Recive response through Transport Channel */
   uiMsgLen = 0;
   of_free (pSocketBuf);
   if ((cm_tnsprt_recv_message
            (pJEConfigSession->tnsprt_channel_p,
             &UCMGenericHeader, &pSocketBuf, &uiMsgLen)) == OF_FAILURE)
   {
      CM_JETRANS_DEBUG_PRINT ("cm_tnsprt_recv_message failed");
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      return OF_FAILURE;
   }

   /*Memory Allocation for Result Structure */
   pSessionEndResult = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
   if (pSessionEndResult == NULL)
   {
      CM_JETRANS_DEBUG_PRINT (" Memory Allocation failed for Result Structure");

      UCMJEFreeRequestMsg (pUCMRequestMsg);
      of_free (pSocketBuf);
      return OF_FAILURE;
   }

   /*Fill Response Msg from Recieve Buffer */
   if ((UCMExtractSessionResponseFromBuff (CM_COMMAND_CONFIG_SESSION_END,
               pSocketBuf, &uiMsgLen,
               pSessionEndResult)) != OF_SUCCESS)
   {
      CM_JETRANS_DEBUG_PRINT ("Fill Response Msg struct Failed");

      UCMJEFreeRequestMsg (pUCMRequestMsg);
      UCMFreeUCMResult (pSessionEndResult);
      of_free (pSocketBuf);
      return OF_FAILURE;
   }

   /*Fill Result structure for Management Engines */
   if (pSessionEndResult->result_code == CM_JE_FAILURE)
   {
      CM_JETRANS_DEBUG_PRINT ("JE returned failure for Session End");

      UCMJEFreeRequestMsg (pUCMRequestMsg);
      of_free (pSocketBuf);
      *presult_p = pSessionEndResult;
      return OF_FAILURE;
   }

   UCMJEFreeRequestMsg (pUCMRequestMsg);
   of_free (pSocketBuf);
   *presult_p = pSessionEndResult;
   of_free(pJEConfigSession);
   return OF_SUCCESS;
}

/*
   AT THE FIRST INSTANCE OF A GROUP NAME, ALWAYS USE /addtogroup <group_name>
   IN THE SUBSEQUENT INTANCES, USE /ingroup <group_name>
   */

/*
   AVOID NEW LINE CHARACTER IN THE DESCRIPTION.
   */

/**
  \ingroup JEWRPAPI
  This API is used to extract the Result information from the received message buffer. This will take the Message buffer received from Transport channel as input and tries to extract the expected information from the buffer and fills the struct cm_result structure with apropriate values.
  <b>Input Parameters: </b> \n
  <i>pRespBuff:</i> Pointer to the Message buffer received from Transport channel.\n
  <b>Output Parameters:</b> \n
  <i>puiReqBufLen:</i> Request buffer length.
  <i>resp_msg_p:</i> Pointer to the struct cm_result structure used to hold the extracted information.\n
  <b>Return Value:</b>\n
  <i>OF_SUCCESS:</i> On successful extraction of Result structure information.\n
  <i>OF_FAILURE:</i> On any failure.
  */
 int32_t UCMExtractSessionResponseFromBuff (uint32_t command_id,
      char * pRespBuff,
      uint32_t * puiReqBufLen,
      struct cm_result * cm_resp_msg_p)
{
   char *pUCMTmpRespBuf = pRespBuff;
   uint32_t uiNvCnt = 0;
   uint32_t uiLen = 0, uiDMPathLen = 0;
   uint32_t no_of_commands;
   uint32_t uiTmpLen = 0;
   struct cm_array_of_commands *pCmdArray = NULL;
   struct cm_command *pTmpCmd;
   struct cm_je_config_session *pTmconfig_session_p = NULL;

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
//   pUCMTmpRespBuf += uiInt32Size_g;
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
//            cm_resp_msg_p->data.je_data_p =
//       (struct cm_je_config_session *) of_calloc (1, sizeof (struct cm_je_config_session));
       pTmconfig_session_p = (struct cm_je_config_session *) of_calloc (1, sizeof (struct cm_je_config_session));
            //if (!cm_resp_msg_p->data.je_data_p)
            if (!pTmconfig_session_p)
            {
               CM_JETRANS_DEBUG_PRINT ("Memory Allocatin Failed");
               return OF_FAILURE;
            }
/*            of_memcpy (cm_resp_msg_p->data.je_data_p, pUCMTmpRespBuf,
                  sizeof (struct cm_je_config_session));
      pUCMTmpRespBuf += sizeof(struct cm_je_config_session);
*/
            pTmconfig_session_p->session_id = of_mbuf_get_32 (pUCMTmpRespBuf);
            pUCMTmpRespBuf += uiOSIntSize_g;

            pTmconfig_session_p->mgmt_engine_id = of_mbuf_get_32 (pUCMTmpRespBuf);
            pUCMTmpRespBuf += uiOSIntSize_g;

            pTmconfig_session_p->flags = of_mbuf_get_32 (pUCMTmpRespBuf);
            pUCMTmpRespBuf += uiOSIntSize_g;

            uiTmpLen = of_mbuf_get_32 (pUCMTmpRespBuf);
            pUCMTmpRespBuf += uiOSIntSize_g;

            of_memcpy(pTmconfig_session_p->admin_role,pUCMTmpRespBuf,uiTmpLen);
            pUCMTmpRespBuf += uiTmpLen;
     
            uiTmpLen = 0; 
            uiTmpLen = of_mbuf_get_32 (pUCMTmpRespBuf);
            pUCMTmpRespBuf += uiOSIntSize_g;
     
            of_memcpy(pTmconfig_session_p->admin_name,pUCMTmpRespBuf,uiTmpLen);
            pUCMTmpRespBuf += uiTmpLen;
     
            cm_resp_msg_p->data.je_data_p = pTmconfig_session_p;
            break;
         case CM_JE_FAILURE:
            break;
      }
   }

   switch (cm_resp_msg_p->sub_command_id)
   {
      case CM_CMD_CONFIG_SESSION_COMMIT:
         CM_JETRANS_DEBUG_PRINT (" Response for Commit Command");
         switch (cm_resp_msg_p->result_code)
         {
            case CM_JE_SUCCESS:
               cm_resp_msg_p->result.success.reserved= of_mbuf_get_32 (pUCMTmpRespBuf);
               pUCMTmpRespBuf += uiOSIntSize_g;
               CM_JETRANS_DEBUG_PRINT ("JE returned Success Code %d",  cm_resp_msg_p->result.success.reserved);
               break;
            case CM_JE_FAILURE:
               {
                  uiLen = 0;
                  if (ucmJECopyErrorInfoFromoBuffer
                        (pUCMTmpRespBuf, cm_resp_msg_p, &uiLen) != OF_SUCCESS)
                  {
                     CM_JETRANS_DEBUG_PRINT
                        ("Copying Error Info into UCMResult Failed");
                     return OF_FAILURE;
                  }
                  pUCMTmpRespBuf += uiLen;
                  break;
               }
         }
         break;
      case CM_CMD_CONFIG_SESSION_REVOKE:
      case CM_CMD_ADD_PARAMS:
      case CM_CMD_SET_PARAMS:
      case CM_CMD_DEL_PARAMS:
      case CM_CMD_EXEC_TRANS_CMD:
      case CM_CMD_CANCEL_TRANS_CMD:
         CM_JETRANS_DEBUG_PRINT (" Response for Post Configuration Request");
         switch (cm_resp_msg_p->result_code)
         {
            case CM_JE_SUCCESS:
               CM_JETRANS_DEBUG_PRINT (" JE returned Success");
               break;
            case CM_JE_FAILURE:
               {
                  uiLen = 0;
                  if (ucmJECopyErrorInfoFromoBuffer
                        (pUCMTmpRespBuf, cm_resp_msg_p, &uiLen) != OF_SUCCESS)
                  {
                     CM_JETRANS_DEBUG_PRINT
                        ("Copying Error Info into UCMResult Failed");
                     return OF_FAILURE;
                  }
                  pUCMTmpRespBuf += uiLen;
                  break;
               }
         }
         break;
      case CM_CMD_CANCEL_PREV_CMD:
      case CM_CMD_SET_DEFAULTS:
      case CM_CMD_SET_PARAM_TO_DEFAULT:
      case CM_CMD_NOTIFY_JE:
         switch (cm_resp_msg_p->result_code)
         {
            case CM_JE_SUCCESS:
               CM_JETRANS_DEBUG_PRINT ("JE returned Success");
               break;
            case CM_JE_FAILURE:
               {
                  uiLen = 0;
                  if (ucmJECopyErrorInfoFromoBuffer
                        (pUCMTmpRespBuf, cm_resp_msg_p, &uiLen) != OF_SUCCESS)
                  {
                     CM_JETRANS_DEBUG_PRINT
                        ("Copying Error Info into UCMResult Failed");
                     return OF_FAILURE;
                  }
                  pUCMTmpRespBuf += uiLen;
                  break;
               }
         }
         break;
      case CM_CMD_COMPARE_PARAM:
      case CM_SUBCOMMAND_AUTHENTICATE_USER:
      case CM_CMD_GET_VERSIONS:
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
               {
                  uiLen = 0;
                  if (ucmJECopyErrorInfoFromoBuffer
                        (pUCMTmpRespBuf, cm_resp_msg_p, &uiLen) != OF_SUCCESS)
                  {
                     CM_JETRANS_DEBUG_PRINT
                        ("Copying Error Info into UCMResult Failed");
                     return OF_FAILURE;
                  }
                  pUCMTmpRespBuf += uiLen;
                  break;
               }
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
      case CM_CMD_GET_DELTA_VERSION_HISTORY:
         switch (cm_resp_msg_p->result_code)
         {
            case CM_JE_SUCCESS:
               {
                  struct cm_version *version_list_p;
                  uint32_t uiversionCnt = 0, uindex_i=0;
                  uint32_t no_of_commands = 0;

                  uiversionCnt=  of_mbuf_get_32 (pUCMTmpRespBuf);
                  pUCMTmpRespBuf += uiOSIntSize_g;

                  version_list_p= (struct cm_version *) of_calloc(uiversionCnt, sizeof(struct cm_version));
                  if (version_list_p == NULL)
                  {
                     CM_JETRANS_DEBUG_PRINT ("Memory Allocation Failed");
                     return OF_FAILURE;
                  }
                  for(uindex_i=0; uindex_i < uiversionCnt; uindex_i++)
                  {
                     struct cm_array_of_commands *pCmdArray;

                     version_list_p[uindex_i].version= of_mbuf_get_64 (pUCMTmpRespBuf);
                     pUCMTmpRespBuf += uiInt64Size_g;

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
                           sizeof(struct cm_command));
                     if (pCmdArray->Commands == NULL)
                     {
                        of_free (version_list_p);
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

                        if (uiDMPathLen > 0)
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
                     version_list_p[uindex_i].command_list_p=pCmdArray;
                  }
                  cm_resp_msg_p->data.version.version_list_p = version_list_p;
                  cm_resp_msg_p->data.version.count_ui = uiversionCnt;
                  break;
               }  
            case CM_JE_FAILURE:
               {
                  uiLen = 0;
                  if (ucmJECopyErrorInfoFromoBuffer
                        (pUCMTmpRespBuf, cm_resp_msg_p, &uiLen) != OF_SUCCESS)
                  {
                     CM_JETRANS_DEBUG_PRINT
                        ("Copying Error Info into UCMResult Failed");
                     return OF_FAILURE;
                  }
                  pUCMTmpRespBuf += uiLen;
                  break;
               }
         }
      default:
         CM_JETRANS_DEBUG_PRINT ("un handled command ");

   }
   return OF_SUCCESS;
}

int32_t cm_validate_login_auth_req(uint32_t mgmt_engine_id,
      unsigned char *user_name_p,
      unsigned char *password_p,
      struct cm_role_info * role_info_p,
      void * tnsprt_channel_p)
{
   unsigned char *pSocketBuf = NULL;
   struct cm_array_of_nv_pair *pNvPairAry;
   struct cm_array_of_nv_pair *pOutNvPairAry = NULL;
   struct cm_msg_generic_header UCMGenericHeader = { };
   struct cm_request_msg *pUCMRequestMsg = NULL;
   struct cm_result *pUCMResponseMsg = NULL;
   uint32_t ii=0, uiMsgLen = 0, uiNVPairCount=0;

   CM_JETRANS_DEBUG_PRINT ("Entered");

   /* Administror Role Information */
   if (role_info_p == NULL)
   {
      CM_JETRANS_DEBUG_PRINT ("ROLE Info is NULL");
      return OF_FAILURE;
   }
   /* Setting Generic Header Values */
   cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_AUTHENTICATE_USER,
         role_info_p, &UCMGenericHeader);

   /* Memory Allocation for Request Message */
   pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
   if (pUCMRequestMsg == NULL)
   {
      CM_JETRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");

      return OF_FAILURE;
   }

   /*Memory Allocation for NVPair Array*/
   pNvPairAry = (struct cm_array_of_nv_pair *)of_calloc(1,sizeof(struct cm_array_of_nv_pair));
   if(pNvPairAry == NULL)
   {
      CM_JETRANS_DEBUG_PRINT ("Memory Allocation for nv_pairs failed");
      return OF_FAILURE;
   }

   /*Memory Allocation for username and password*/
   pNvPairAry->nv_pairs = (struct cm_nv_pair *) of_calloc (2, sizeof (struct cm_nv_pair));
   if(pNvPairAry->nv_pairs == NULL)
   {
      CM_JETRANS_DEBUG_PRINT ("Memory Allocation for NVPair Arrays failed");
      of_free(pNvPairAry);
      return OF_FAILURE;
   }
   pNvPairAry->count_ui = 2;

   /*Filling User name into NVPair*/
   pNvPairAry->nv_pairs[0].name_length = of_strlen("username");
   if(pNvPairAry->nv_pairs[0].name_length)
   {    
      pNvPairAry->nv_pairs[0].name_p = of_calloc (1, pNvPairAry->nv_pairs[0].name_length + 1);  
      if (pNvPairAry->nv_pairs[0].name_p == NULL)
      {
         CM_JETRANS_DEBUG_PRINT ("Memory Allocation Failed for user name Name");
         return OF_FAILURE;
      }
      of_strncpy (pNvPairAry->nv_pairs[0].name_p, "username", pNvPairAry->nv_pairs[0].name_length);
   }  

   pNvPairAry->nv_pairs[0].value_type = CM_DATA_TYPE_STRING;
   pNvPairAry->nv_pairs[0].value_length = of_strlen(user_name_p);
   if (pNvPairAry->nv_pairs[0].value_length > 0)
   {
      pNvPairAry->nv_pairs[0].value_p = of_calloc (1, pNvPairAry->nv_pairs[0].value_length+1);
      if (pNvPairAry->nv_pairs[0].value_p == NULL)
      {
         CM_JETRANS_DEBUG_PRINT ("Memory Allocation Failed for user name Value");
         return OF_FAILURE;
      }
      of_strncpy (pNvPairAry->nv_pairs[0].value_p, user_name_p, pNvPairAry->nv_pairs[0].value_length);
   }

   /*Filling Password into NVPair*/
   pNvPairAry->nv_pairs[1].name_length = of_strlen("password");
   if(pNvPairAry->nv_pairs[1].name_length)
   {    
      pNvPairAry->nv_pairs[1].name_p = of_calloc (1, pNvPairAry->nv_pairs[1].name_length + 1);  
      if (pNvPairAry->nv_pairs[1].name_p == NULL)
      {
         CM_JETRANS_DEBUG_PRINT ("Memory Allocation Failed for password Name");
         return OF_FAILURE;
      }
      of_strncpy (pNvPairAry->nv_pairs[1].name_p, "password", pNvPairAry->nv_pairs[1].name_length);
   }

   pNvPairAry->nv_pairs[1].value_type = CM_DATA_TYPE_STRING;
   pNvPairAry->nv_pairs[1].value_length = of_strlen(password_p);
   if (pNvPairAry->nv_pairs[1].value_length > 0)
   {
      pNvPairAry->nv_pairs[1].value_p = of_calloc (1, pNvPairAry->nv_pairs[1].value_length+1);
      if (pNvPairAry->nv_pairs[1].value_p == NULL)
      {
         CM_JETRANS_DEBUG_PRINT ("Memory Allocation Failed for password Value");
         return OF_FAILURE;
      }
      of_strncpy (pNvPairAry->nv_pairs[1].value_p, password_p, pNvPairAry->nv_pairs[1].value_length);
   }

   /*Fill Request Message Structure */
   cm_fill_request_message (pUCMRequestMsg, CM_SUBCOMMAND_AUTHENTICATE_USER , NULL, 0, pNvPairAry, NULL, &uiMsgLen);

   /* Allocate Memory for Send Buffer */
   pSocketBuf = (unsigned char *) of_calloc (1, uiMsgLen);
   if (!pSocketBuf)
   {
      CM_JETRANS_DEBUG_PRINT ("Memory allocation failed");
      UCMJEFreeRequestMsg (pUCMRequestMsg);
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

      UCMJEFreeRequestMsg (pUCMRequestMsg);
      return OF_FAILURE;
   }
   of_free (pSocketBuf);
   uiMsgLen = 0;
   pSocketBuf = NULL;
   of_memset (&UCMGenericHeader, 0, sizeof (struct cm_msg_generic_header));
   /*Recive response through Transport Channel */
   if ((cm_tnsprt_recv_message (tnsprt_channel_p,
               &UCMGenericHeader, &pSocketBuf,
               &uiMsgLen)) == OF_FAILURE)
   {
      CM_JETRANS_DEBUG_PRINT ("cm_tnsprt_recv_message failed");
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      return OF_FAILURE;
   }

   /*Memory Allocation for Response Msg structure */
   pUCMResponseMsg = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
   if (!pUCMResponseMsg)
   {
      CM_JETRANS_DEBUG_PRINT ("Memory allocation failed");
      of_free (pSocketBuf);
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      return OF_FAILURE;
   }

   /*Fill Response Msg from Recieve Buffer */
   if ((UCMExtractSessionResponseFromBuff (CM_COMMAND_AUTHENTICATE_USER,
               pSocketBuf, &uiMsgLen,
               pUCMResponseMsg)) != OF_SUCCESS)
   {
      CM_JETRANS_DEBUG_PRINT ("TransportChannelRecv Message Failed");
      of_free (pSocketBuf);
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      UCMFreeUCMResult (pUCMResponseMsg);
      return OF_FAILURE;
   }

   if (pUCMResponseMsg)
   {
      /*Fill Configuration Session Values */
      if (pUCMResponseMsg->result_code == CM_JE_FAILURE)
      {
         CM_JETRANS_DEBUG_PRINT ("JE returned Failure");
         UCMJEFreeRequestMsg (pUCMRequestMsg);
         UCMFreeUCMResult (pUCMResponseMsg);
         /* free pUCMResponseMsg */
         of_free (pSocketBuf);
         return UCMJE_ERROR_USER_AUTHENTICATION_FAILED;
      }
      pOutNvPairAry = (struct cm_array_of_nv_pair *) pUCMResponseMsg->data.je_data_p;
      uiNVPairCount = pOutNvPairAry->count_ui;

      for (ii=0; ii<uiNVPairCount; ii++)
      {
         if (of_strcmp(pOutNvPairAry->nv_pairs[ii].name_p,"adminname") == 0)
         {
            of_strncpy(role_info_p->admin_name,pOutNvPairAry->nv_pairs[ii].value_p,pOutNvPairAry->nv_pairs[ii].value_length);
         }
         if (of_strcmp(pOutNvPairAry->nv_pairs[ii].name_p,"adminrole") == 0)
         {
            of_strncpy(role_info_p->admin_role,pOutNvPairAry->nv_pairs[ii].value_p,pOutNvPairAry->nv_pairs[ii].value_length);
         }
      }
   }

   UCMJEFreeRequestMsg (pUCMRequestMsg);
   /* free pUCMResponseMsg */
   of_free (pSocketBuf);

   UCMFreeUCMResult (pUCMResponseMsg);
   return OF_SUCCESS;
}

int32_t cm_get_delta_version_history_t ( uint32_t mgmt_engine_id,
      struct cm_role_info * role_info_p, void * tnsprt_channel_p, uint64_t from_version_ui,
      uint64_t end_version, uint32_t *ver_cnt_ui_p, struct cm_version **version_list_pp)
{
   unsigned char *pSocketBuf = NULL, *pTmp;
   struct cm_msg_generic_header UCMGenericHeader = { };
   struct cm_request_msg *pUCMRequestMsg = NULL;
   uint32_t uiMsgLen = 0, uiReqMsgLen;
   struct cm_result *pversionResult = NULL;
   int32_t return_value;

   CM_JETRANS_DEBUG_PRINT ("Entered");

   /* Setting Generic Header Values */
   cm_fill_generic_header (mgmt_engine_id,
         CM_COMMAND_VERSION_HISTORY,
         role_info_p, &UCMGenericHeader);

   /* Memory Allocation for Request Message */
   pUCMRequestMsg = of_calloc (1, sizeof (struct cm_request_msg));
   if (pUCMRequestMsg == NULL)
   {
      CM_JETRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
      return OF_FAILURE;
   }

   /*Fill Request Message Structure */
   cm_fill_request_message (pUCMRequestMsg, CM_CMD_GET_DELTA_VERSION_HISTORY,
         NULL, 0, NULL, NULL, &uiMsgLen); 
   uiReqMsgLen=uiMsgLen;
   uiMsgLen = uiReqMsgLen + uiInt64Size_g +  uiInt64Size_g;

   /* Allocate Memory for Send Buffer */
   pSocketBuf = (unsigned char *) of_calloc (1, uiMsgLen);
   if (!pSocketBuf)
   {
      CM_JETRANS_DEBUG_PRINT (" Memory allocation failed");
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      return OF_FAILURE;
   }

   /* Convert Request Message Structure into flat Buffer */
   cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

   /* Append from version  SocketBuffer */
   pTmp = pSocketBuf;
   pTmp = of_mbuf_put_64 (pTmp + uiReqMsgLen, from_version_ui);

   /* Append from version  SocketBuffer */
   pTmp = of_mbuf_put_64 (pTmp, end_version);


   /*Send Message through Transport Channel */
   if ((cm_tnsprt_send_message (tnsprt_channel_p,
               &UCMGenericHeader, pSocketBuf,
               uiMsgLen)) != OF_SUCCESS)
   {
      CM_JETRANS_DEBUG_PRINT ("Sending through transport channel failed");
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      of_free (pSocketBuf);
      return OF_FAILURE;
   }

   /*Sending Request Message is done. So freeing it */
   of_memset (&UCMGenericHeader, 0, sizeof (struct cm_msg_generic_header));

   /*Recive response through Transport Channel */
   of_free (pSocketBuf);
   uiMsgLen = 0;
   if ((cm_tnsprt_recv_message(tnsprt_channel_p,
               &UCMGenericHeader, &pSocketBuf, &uiMsgLen)) == OF_FAILURE)
   {
      CM_JETRANS_DEBUG_PRINT ("cm_tnsprt_recv_message failed");
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      return OF_FAILURE;
   }

   /*Memory Allocation for Result Structure */
   pversionResult = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
   if (pversionResult == NULL)
   {

      CM_JETRANS_DEBUG_PRINT (" Memory Allocation failed for Result Structure");
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      of_free (pSocketBuf);
      return OF_FAILURE;
   }

   /*Fill Response Msg from Recieve Buffer */
   if ((UCMExtractSessionResponseFromBuff
            (CM_COMMAND_VERSION_HISTORY, pSocketBuf, &uiMsgLen,
             pversionResult)) != OF_SUCCESS)
   {
      CM_JETRANS_DEBUG_PRINT ("Fill Response Msg struct Failed");
      of_free (pversionResult);
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      of_free (pSocketBuf);
      return OF_FAILURE;
   }

   if (pversionResult->result_code == CM_JE_FAILURE)
   {
      /*Deallocating memory */
      of_free (pversionResult);
      of_free (pUCMRequestMsg);
      of_free (pSocketBuf);
      return OF_FAILURE;
   }

   return_value=pversionResult->result_code;
   *version_list_pp = (struct cm_version *)pversionResult->data.version.version_list_p;
   *ver_cnt_ui_p=pversionResult->data.version.count_ui;
   UCMFreeUCMResult (pversionResult);
   of_free (pUCMRequestMsg);
   of_free (pSocketBuf);
   return return_value;

}

int32_t UCMRebootDevice ( uint32_t mgmt_engine_id,
      struct cm_role_info * role_info_p, void * tnsprt_channel_p,struct cm_array_of_nv_pair **ppnv_pair_array)
{
   unsigned char *pSocketBuf = NULL;
   struct cm_msg_generic_header UCMGenericHeader = { };
   struct cm_request_msg *pUCMRequestMsg = NULL;
   uint32_t uiMsgLen = 0, uiReqMsgLen;
   int32_t return_value = OF_SUCCESS;

   CM_JETRANS_DEBUG_PRINT ("Entered");

   /* Setting Generic Header Values */
   cm_fill_generic_header (mgmt_engine_id,
         CM_COMMAND_REBOOT_DEVICE,
         role_info_p, &UCMGenericHeader);

   /* Memory Allocation for Request Message */
   pUCMRequestMsg = of_calloc (1, sizeof (struct cm_request_msg));
   if (pUCMRequestMsg == NULL)
   {
      CM_JETRANS_DEBUG_PRINT ("Memory Allocation for Reboot Request Message failed");
      return OF_FAILURE;
   }

   /*Fill Request Message Structure */
   cm_fill_request_message (pUCMRequestMsg, CM_CMD_REBOOT_DEVICE,
         NULL, 0, NULL, NULL, &uiMsgLen); 
   uiReqMsgLen=uiMsgLen;

   /* Allocate Memory for Send Buffer */
   pSocketBuf = (unsigned char *) of_calloc (1, uiMsgLen);
   if (!pSocketBuf)
   {
      CM_JETRANS_DEBUG_PRINT (" Memory allocation failed");
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      return OF_FAILURE;
   }

   /* Convert Request Message Structure into flat Buffer */
   cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);


   /*Send Message through Transport Channel */
   if ((cm_tnsprt_send_message (tnsprt_channel_p,
               &UCMGenericHeader, pSocketBuf,
               uiMsgLen)) != OF_SUCCESS)
   {
      CM_JETRANS_DEBUG_PRINT ("Sending Reboot Request through transport channel failed");
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      of_free (pSocketBuf);
      return OF_FAILURE;
   }
#if 0 
   /*Sending Request Message is done. So freeing it */
   of_memset (&UCMGenericHeader, 0, sizeof (struct cm_msg_generic_header));

   /*Recive response through Transport Channel */
   of_free (pSocketBuf);
   uiMsgLen = 0;
   if ((cm_tnsprt_recv_message(tnsprt_channel_p,
               &UCMGenericHeader, &pSocketBuf, &uiMsgLen)) == OF_FAILURE)
   {
      CM_JETRANS_DEBUG_PRINT ("cm_tnsprt_recv_message failed");
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      return OF_FAILURE;
   }

   /*Memory Allocation for Result Structure */
   pversionResult = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
   if (pversionResult == NULL)
   {

      CM_JETRANS_DEBUG_PRINT (" Memory Allocation failed for Result Structure");
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      of_free (pSocketBuf);
      return OF_FAILURE;
   }

   /*Fill Response Msg from Recieve Buffer */
   if ((UCMExtractSessionResponseFromBuff
            (CM_COMMAND_REBOOT_DEVICE, pSocketBuf, &uiMsgLen,
             pversionResult)) != OF_SUCCESS)
   {
      CM_JETRANS_DEBUG_PRINT ("Fill Response Msg struct Failed");
      of_free (pversionResult);
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      of_free (pSocketBuf);
      return OF_FAILURE;
   }

   if (pversionResult->result_code == CM_JE_FAILURE)
   {
      /*Deallocating memory */
      of_free (pversionResult);
      of_free (pUCMRequestMsg);
      of_free (pSocketBuf);
      return OF_FAILURE;
   }
   return_value=pversionResult->result_code;
   *ppnv_pair_array = (struct cm_array_of_nv_pair *)pversionResult->data.je_data_p;
   UCMFreeUCMResult (pversionResult);
#endif
   of_free (pUCMRequestMsg);
   of_free (pSocketBuf);
   return return_value;
}

int32_t UCMGetversions ( uint32_t mgmt_engine_id,
      struct cm_role_info * role_info_p, void * tnsprt_channel_p,struct cm_array_of_nv_pair **ppnv_pair_array)
{
   unsigned char *pSocketBuf = NULL;
   struct cm_msg_generic_header UCMGenericHeader = { };
   struct cm_request_msg *pUCMRequestMsg = NULL;
   uint32_t uiMsgLen = 0, uiReqMsgLen;
   struct cm_result *pversionResult = NULL;
   int32_t return_value;

   CM_JETRANS_DEBUG_PRINT ("Entered");

   /* Setting Generic Header Values */
   cm_fill_generic_header (mgmt_engine_id,
         CM_COMMAND_VERSION_HISTORY,
         role_info_p, &UCMGenericHeader);

   /* Memory Allocation for Request Message */
   pUCMRequestMsg = of_calloc (1, sizeof (struct cm_request_msg));
   if (pUCMRequestMsg == NULL)
   {
      CM_JETRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
      return OF_FAILURE;
   }

   /*Fill Request Message Structure */
   cm_fill_request_message (pUCMRequestMsg, CM_CMD_GET_VERSIONS,
         NULL, 0, NULL, NULL, &uiMsgLen); 
   uiReqMsgLen=uiMsgLen;

   /* Allocate Memory for Send Buffer */
   pSocketBuf = (unsigned char *) of_calloc (1, uiMsgLen);
   if (!pSocketBuf)
   {
      CM_JETRANS_DEBUG_PRINT (" Memory allocation failed");
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      return OF_FAILURE;
   }

   /* Convert Request Message Structure into flat Buffer */
   cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);


   /*Send Message through Transport Channel */
   if ((cm_tnsprt_send_message (tnsprt_channel_p,
               &UCMGenericHeader, pSocketBuf,
               uiMsgLen)) != OF_SUCCESS)
   {
      CM_JETRANS_DEBUG_PRINT ("Sending through transport channel failed");
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      of_free (pSocketBuf);
      return OF_FAILURE;
   }

   /*Sending Request Message is done. So freeing it */
   of_memset (&UCMGenericHeader, 0, sizeof (struct cm_msg_generic_header));

   /*Recive response through Transport Channel */
   of_free (pSocketBuf);
   uiMsgLen = 0;
   if ((cm_tnsprt_recv_message(tnsprt_channel_p,
               &UCMGenericHeader, &pSocketBuf, &uiMsgLen)) == OF_FAILURE)
   {
      CM_JETRANS_DEBUG_PRINT ("cm_tnsprt_recv_message failed");
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      return OF_FAILURE;
   }

   /*Memory Allocation for Result Structure */
   pversionResult = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
   if (pversionResult == NULL)
   {

      CM_JETRANS_DEBUG_PRINT (" Memory Allocation failed for Result Structure");
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      of_free (pSocketBuf);
      return OF_FAILURE;
   }

   /*Fill Response Msg from Recieve Buffer */
   if ((UCMExtractSessionResponseFromBuff
            (CM_COMMAND_VERSION_HISTORY, pSocketBuf, &uiMsgLen,
             pversionResult)) != OF_SUCCESS)
   {
      CM_JETRANS_DEBUG_PRINT ("Fill Response Msg struct Failed");
      of_free (pversionResult);
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      of_free (pSocketBuf);
      return OF_FAILURE;
   }

   if (pversionResult->result_code == CM_JE_FAILURE)
   {
      /*Deallocating memory */
      of_free (pversionResult);
      of_free (pUCMRequestMsg);
      of_free (pSocketBuf);
      return OF_FAILURE;
   }

   return_value=pversionResult->result_code;
   *ppnv_pair_array = (struct cm_array_of_nv_pair *)pversionResult->data.je_data_p;
   UCMFreeUCMResult (pversionResult);
   of_free (pUCMRequestMsg);
   of_free (pSocketBuf);
   return return_value;
}

int32_t cm_send_notification_to_je ( uint32_t mgmt_engine_id,
      struct cm_role_info * role_info_p, void * tnsprt_channel_p,
      uint32_t notification_ui, void *notify_data_p)
{
   unsigned char *pSocketBuf = NULL, *pTmp;
   struct cm_msg_generic_header UCMGenericHeader = { };
   struct cm_request_msg *pUCMRequestMsg = NULL;
   uint32_t uiMsgLen = 0, uiReqMsgLen;
   struct cm_result *pNotifyResult = NULL;
   uint64_t version=*(uint64_t *)notify_data_p;

   CM_JETRANS_DEBUG_PRINT ("Entered");

   /* Setting Generic Header Values */
   cm_fill_generic_header (mgmt_engine_id,
         CM_COMMAND_NOTIFICATION,
         role_info_p, &UCMGenericHeader);

   /* Memory Allocation for Request Message */
   pUCMRequestMsg = of_calloc (1, sizeof (struct cm_request_msg));
   if (pUCMRequestMsg == NULL)
   {
      CM_JETRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
      return OF_FAILURE;
   }

   /*Fill Request Message Structure */
   cm_fill_request_message (pUCMRequestMsg, CM_CMD_NOTIFY_JE,
         NULL, 0, NULL, NULL, &uiMsgLen); 
   uiReqMsgLen=uiMsgLen;
   uiMsgLen += uiOSIntSize_g +  uiInt64Size_g;

   /* Allocate Memory for Send Buffer */
   pSocketBuf = (unsigned char *) of_calloc (1, uiMsgLen);
   if (!pSocketBuf)
   {
      CM_JETRANS_DEBUG_PRINT (" Memory allocation failed");
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      return OF_FAILURE;
   }

   /* Append from version  SocketBuffer */
   pTmp = pSocketBuf;
   pTmp = of_mbuf_put_32 (pTmp + uiReqMsgLen, notification_ui);

   /* Append from version  SocketBuffer */
   pTmp = pSocketBuf;

   pTmp = of_mbuf_put_64 (pTmp + uiReqMsgLen+uiOSIntSize_g,version);


   /* Convert Request Message Structure into flat Buffer */
   cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

   /*Send Message through Transport Channel */
   if ((cm_tnsprt_send_message (tnsprt_channel_p,
               &UCMGenericHeader, pSocketBuf,
               uiMsgLen)) != OF_SUCCESS)
   {
      CM_JETRANS_DEBUG_PRINT ("Sending through transport channel failed");
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      of_free (pSocketBuf);
      return OF_FAILURE;
   }

   /*Sending Request Message is done. So freeing it */
   of_memset (&UCMGenericHeader, 0, sizeof (struct cm_msg_generic_header));

   /*Recive response through Transport Channel */
   of_free (pSocketBuf);
   uiMsgLen = 0;
   if ((cm_tnsprt_recv_message(tnsprt_channel_p,
               &UCMGenericHeader, &pSocketBuf, &uiMsgLen)) == OF_FAILURE)
   {
      CM_JETRANS_DEBUG_PRINT ("cm_tnsprt_recv_message failed");
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      return OF_FAILURE;
   }

   /*Memory Allocation for Result Structure */
   pNotifyResult = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
   if (pNotifyResult == NULL)
   {

      CM_JETRANS_DEBUG_PRINT (" Memory Allocation failed for Result Structure");
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      of_free (pSocketBuf);
      return OF_FAILURE;
   }

   /*Fill Response Msg from Recieve Buffer */
   if ((UCMExtractSessionResponseFromBuff
            (CM_COMMAND_NOTIFICATION, pSocketBuf, &uiMsgLen,
             pNotifyResult)) != OF_SUCCESS)
   {
      CM_JETRANS_DEBUG_PRINT ("Fill Response Msg struct Failed");
      of_free (pNotifyResult);
      UCMJEFreeRequestMsg (pUCMRequestMsg);
      of_free (pSocketBuf);
      return OF_FAILURE;
   }

   if (pNotifyResult->result_code == CM_JE_FAILURE)
   {
      /*Deallocating memory */
      of_free (pNotifyResult);
      of_free (pUCMRequestMsg);
      of_free (pSocketBuf);
      return OF_FAILURE;
   }

   UCMFreeUCMResult (pNotifyResult);
   of_free (pUCMRequestMsg);
   of_free (pSocketBuf);
   return OF_SUCCESS;
}
//#endif /* CM_JE_SUPPORT */
#endif /* CM_SUPPORT */
