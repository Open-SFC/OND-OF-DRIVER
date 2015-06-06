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
 * File name: jetrans.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/25/2013
 * Description: 
*/

#ifdef CM_SUPPORT
#ifdef CM_JE_SUPPORT

/********************************************************
 * * * * * *  I n c l u d e   S e c t i o n   * * * * * *
 ********************************************************/
#include "jeincld.h"
#include "jeldef.h"
#include "jelif.h"
#include "cmtransgdf.h"
#include "errno.h"
#include "netinet/in.h"
/******************************************************************************
 * * * * * * * * * * * * * * * FUNCTION PROTOTYPES * * * * * * * * * * * * * * 
 *****************************************************************************/

 int32_t cm_je_prepare_config_request_from_message (char * pRequestBuf,
      struct je_config_request *
      config_request_p);

 int32_t cm_je_prepare_response_message (struct cm_msg_generic_header *
      pUCMGenHeader,
      struct cm_result * pResponseMsg,
      char * pRespBuf,
      uint32_t uiBuffLen);

 int32_t cm_je_get_response_message_length (struct je_config_request * config_request_p,
      struct cm_result * result_p,
      uint32_t * puiBuffLen);

 int32_t cm_je_free_config_request (struct je_config_request * config_request_p);

 int32_t cm_je_free_ucmresult (uint32_t command_id, uint32_t sub_command_id,
      struct cm_result * pUCMResult);

 int32_t cm_je_get_nodeinfo_length (struct cm_dm_node_info *
      result_pNodeInfo,
      uint32_t uiNodeCnt,
      uint32_t * puiResponseLen);
 void cm_je_copy_dminfo_to_buff (struct cm_dm_node_info * node_info_p,
      uint32_t uiNodeCnt,
      char ** buffer_pp);
 void cm_je_free_nodeinfo_struct (uint32_t uiNodeCnt,
      struct cm_dm_node_info * node_info_p);
 void cm_je_copy_opaqueinfo_to_message (int32_t * opaque_info_p,
      uint32_t opaque_info_length,
      char ** buffer_pp);

 void *cm_je_create_send_tnsprtchannel (uint32_t trans_protocol_ui,
      uint16_t src_port_ui,
      uint16_t dest_port_ui);

 void cm_je_close_send_tnsprtchannel (struct cm_tnsprt_channel *
      tnsprt_channel_p);

 int32_t cm_je_get_errorinfo_length (struct cm_result * cm_resp_msg_p,
      uint32_t * puiLen);

 int32_t cm_je_copy_errorinfo_to_buffer (char * pUCMTempRespBuf,
      struct cm_result * cm_resp_msg_p,
      uint32_t * puiLen);

void cm_je_get_dataattrib_length(struct cm_dm_node_info * node_info_p,uint32_t *uiDataAttrLen);
void cm_je_get_elementattrib_length(struct cm_dm_node_info * node_info_p , uint32_t *uiELeAttrLen);
/********************************************************
 * * * * * *  G l o b a l   S e c t i o n   * * * * * *
 ********************************************************/
struct cm_poll_info cm_je_fd_list_g[UCMJE_FDPOLL_MAXFDS];
extern UCMDllQ_t cm_je_tnsprtchannel_list_g;
extern UCMDllQ_t cm_je_request_queue_g;
uint32_t uiGenHdrLen_g = sizeof (struct cm_msg_generic_header);
#ifdef CM_JE_STATS_SUPPORT
extern int32_t cm_je_global_stats_g[JE_STATS_MAX];
extern int32_t cm_je_session_stats_g[JE_SESSION_STATS_MAX];
extern int32_t cm_je_appl_stats_g[JE_SECAPPL_STATS_MAX];
#endif
uint32_t uiInt32Size_g=CM_UINT32_SIZE;
uint32_t uiInt64Size_g=CM_UINT64_SIZE;
uint32_t uiOSIntSize_g=CM_UINT_OS_SIZE;

/********************************************************
 * F u n c t i o n   D e f i n i t i o n   S e c t i o n*
 ********************************************************/

/**
  \ingroup JETRAPI
  This API Adds given Transport Channel to Global Transport Channel List.

  <b>Input paramameters: </b>\n
  <b><i> tnsprt_channel_p: </i></b> Pointer to Transport Channel structure
  <b>Output Parameters: </b> NONE \n
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : cmje_add_tnsprtchannel_to_list 
 ** Description   : This API adds Transport Channel structure to Global Transport
 **                  Channel List.
 ** Input params  : tnsprt_channel_p - pointer to struct cmje_tnsprtchannel_info
 ** Output params : NONE
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t cmje_add_tnsprtchannel_to_list (struct cmje_tnsprtchannel_info * tnsprt_channel_p)
{

   if (tnsprt_channel_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("Transport Channel is NULL");
      return OF_FAILURE;
   }
   CM_DLLQ_APPEND_NODE (&cm_je_tnsprtchannel_list_g, &tnsprt_channel_p->list_node);
   CM_JE_DEBUG_PRINT ("Successfully added to Transport Channel List");
   return OF_SUCCESS;
}

/**
  \ingroup JETRAPI
  This API Deletes given Transport Channel from Global Transport Channel List.

  <b>Input paramameters: </b>\n
  <b><i> tnsprt_channel_p: </i></b> Pointer to Transport Channel structure
  <b>Output Parameters: </b> NONE \n
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : cmje_delete_tnsprtchannel_from_list 
 ** Description   : This API Deletes Transport Channel structure from Global Transport
 **                  Channel List.
 ** Input params  : tnsprt_channel_p - pointer to struct cmje_tnsprtchannel_info
 ** Output params : NONE
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t cmje_delete_tnsprtchannel_from_list (struct cmje_tnsprtchannel_info * tnsprt_channel_p)
{
   UCMDllQNode_t *pDllQNode = NULL, *pTmpNode = NULL;
   struct cmje_tnsprtchannel_info *pJETrDllNode = NULL;

   if (tnsprt_channel_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("Transport Channel is NULL");
      return OF_FAILURE;
   }

   CM_DLLQ_DYN_SCAN (&cm_je_tnsprtchannel_list_g, pDllQNode, pTmpNode,
         UCMDllQNode_t *)
   {
      pJETrDllNode =
         CM_DLLQ_LIST_MEMBER (pDllQNode, struct cmje_tnsprtchannel_info *, list_node);
      if (pJETrDllNode == tnsprt_channel_p)
      {
         CM_DLLQ_DELETE_NODE (&cm_je_tnsprtchannel_list_g, pDllQNode);
         return OF_SUCCESS;
      }
   }

   return OF_FAILURE;
}

/**
  \ingroup JETRAPI
  This API Finds Transport Channel structure for given socket descriptor from Global
  Transport Channel List.  

  <b>Input paramameters: </b>\n
  <b><i> fd_ui: </i></b> Socket Descriptor
  <b>Output Parameters: </b> NONE \n
  <b>Return Value:</b>  tnsprt_channel_p - pointer to struct cmje_tnsprtchannel_info in success
  NULL in failure case. \n
  */
/******************************************************************************
 ** Function Name : cmje_get_tnsprtchannel_from_list
 ** Description   : This API Finds Transport Channel structure for given socket
 **                  descriptor from Global Transport Channel List.
 ** Input params  : fd_ui - Socket Descriptor
 ** Output params : NONE
 ** Return Value  : tnsprt_channel_p - pointer to struct cmje_tnsprtchannel_info in success
 **                  NULL in failure case
 *******************************************************************************/
struct cmje_tnsprtchannel_info *cmje_get_tnsprtchannel_from_list (cm_sock_handle fd_ui)
{
   struct cmje_tnsprtchannel_info *tnsprt_channel_p = NULL;
   UCMDllQNode_t *pDllQNode = NULL;

   CM_DLLQ_SCAN (&cm_je_tnsprtchannel_list_g, pDllQNode, UCMDllQNode_t *)
   {
      tnsprt_channel_p =
         CM_DLLQ_LIST_MEMBER (pDllQNode, struct cmje_tnsprtchannel_info *, list_node);
      if (tnsprt_channel_p->sock_fd_ui == fd_ui)
      {
         return tnsprt_channel_p;
      }
   }
   CM_JE_DEBUG_PRINT ("Transport Channel is NULL");
   return NULL;
}

struct cmje_tnsprtchannel_info *cmje_get_parent_tnsprtchannel_from_list (cm_sock_handle fd_ui)
{
   struct cmje_tnsprtchannel_info *tnsprt_channel_p = NULL;
   UCMDllQNode_t *pDllQNode = NULL;

   CM_DLLQ_SCAN (&cm_je_tnsprtchannel_list_g, pDllQNode, UCMDllQNode_t *)
   {
      tnsprt_channel_p =
         CM_DLLQ_LIST_MEMBER (pDllQNode, struct cmje_tnsprtchannel_info *, list_node);
      if (tnsprt_channel_p->child_sock_fd_ui == fd_ui)
      {
         return tnsprt_channel_p;
      }
   }
   CM_JE_DEBUG_PRINT ("Transport Channel is NULL");
   return NULL;
}


/**
  \ingroup JETRAPI
  This API creates Transport Channel by accepting TCP connection for Management
  Engines. This API also allocates memory for transport channel data structure
  for new fd and initializes values. Starts a Timer for the transport channel.


  <b>Input paramameters: </b>\n
  <b><i> sock_fd_ui: </i></b> Socket Descriptor from Management Engines to be accepted
  <b>Output Parameters: </b> NONE \n
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : cmje_create_recv_tnsprtchannel 
 ** Description   : This API accepts TCP Connection from Management Engine and
 **                  initializes new TransChannel structure for it.
 ** Input params  : fd_ui - Socket Descripter
 ** Output params : NONE
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t cmje_create_recv_tnsprtchannel (cm_sock_handle fd_ui)
{
   cm_sock_handle uiNewFd = 0;
   uint32_t peer_ip = 0;
   uint16_t uiPeerPort = 0;
#ifdef CM_JE_DEBUG
   char aAddr[16];
#endif
   struct cmje_tnsprtchannel_info *tnsprt_channel_p = NULL;

   /* Accept Fd */
   uiNewFd = cm_socket_accept (fd_ui, &peer_ip, &uiPeerPort);
   if (uiNewFd < 0)
   {
      CM_JE_DEBUG_PRINT ("Socket Accept Failed");
      return OF_FAILURE;
   }

#ifdef CM_JE_DEBUG
   cm_inet_ntoal (peer_ip, aAddr);
#endif
   CM_JE_DEBUG_PRINT ("socket recived from Ip Address= %s Port= %d ", aAddr,
         uiPeerPort);

   CM_JE_DEBUG_PRINT ("Created New  FD = %d", uiNewFd);

   /* Create Transport Channel Structure for Fd */
   tnsprt_channel_p = (struct cmje_tnsprtchannel_info *) cm_je_tnsprtchannel_alloc ();
   if (!tnsprt_channel_p)
   {
      CM_JE_DEBUG_PRINT ("Transport Channel Memory Allocation Failed");
      cm_socket_close(uiNewFd);
      return OF_FAILURE;
   }

   /* Set Transport Channel  Values */
   tnsprt_channel_p->sock_fd_ui = uiNewFd;
   cm_timer_create (&(tnsprt_channel_p->session_timer_p));

   /* Start Idle Timer for Transport Channel */
   cm_timer_start (tnsprt_channel_p->session_timer_p,
         (void *) cmje_tnsprtchannel_timeout, (void *) tnsprt_channel_p,
         SECS_TMR, UCMJE_INAC_TIMEOUT, 0);

   /* Add Transport Channel to Global List */
   cmje_add_tnsprtchannel_to_list (tnsprt_channel_p);
   CM_INC_STATS_JE (JE_TRANSCHANNEL_STARTED);
   return OF_SUCCESS;
}

/**
  \ingroup JETRAPI
  This API buffers Data recieved from Management Engine and frames into
  Configuration Request. It calls SessionHandler/SecurityApplicationGetHandlers/
  data_modelHandlers based on Command Id.It converts Result Structure recieved
  from Handler into socket buffer and sends back to Management engine through
  tranport channel.


  <b>Input paramameters: </b>\n
  <b><i> sock_fd_ui: </i></b> Socket Descriptor from Management Engines 
  <b>Output Parameters: </b> NONE \n
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : cm_je_read_and_process_config_request 
 ** Description   : This API buffers Data recieved from Management Engine and
 **                  frames into Configuration Request. It calls SessionHandler/Security
 **                  ApplicationGetHandlers/data_modelHandlers based on Command.
 **                  Convert Result Structure recieved from Handler into socket buffer and
 **                  sends back to Management engine through tranport channel.
 ** Input params  : fd_ui - Socket Descripter
 ** Output params : NONE
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t cm_je_read_and_process_config_request (cm_sock_handle fd_ui)
{
   struct cmje_tnsprtchannel_info *tnsprt_channel_p = NULL;
   struct je_config_request *config_request_p = NULL;
   struct cm_msg_generic_header GenericHdr;
   char *pResponseBuf = NULL;
   int32_t iBytesRecd = 0;
   struct cm_result *pUCMResult = NULL;
   uint32_t return_value = 0;
   struct cm_je_config_session *config_session_p = NULL;
   uint32_t uiBuffLen = 0;
   uint32_t uiRecvBufLen=0;
   char aRecvBufLen[uiOSIntSize_g];
#ifdef CM_STATS_COLLECTOR_SUPPORT
   struct cmje_tnsprtchannel_info *pJEParTrChannel = NULL;
#endif 
   cm_sock_handle sock_fd_ui;

   /* Extract Transport Channel from Global List */
   tnsprt_channel_p = cmje_get_tnsprtchannel_from_list (fd_ui);
   if (!tnsprt_channel_p)
   {
      CM_JE_DEBUG_PRINT ("Invalid Transport Channel");
      return OF_FAILURE;
   }

   /* Read the Message Length */
   if ((iBytesRecd = cm_socket_recv (tnsprt_channel_p->sock_fd_ui,
               aRecvBufLen,
               uiOSIntSize_g, MSG_PEEK)) <= 0)
   {
      perror ("JE-SocketRecvFrom:");
      CM_JE_DEBUG_PRINT ("failed and return value = %d", errno);
      cmje_close_recv_tnsprtchannel (tnsprt_channel_p->sock_fd_ui);
      return OF_FAILURE;
   }

   uiRecvBufLen = of_mbuf_get_32(aRecvBufLen);
   if( uiRecvBufLen > CM_MAX_REQUEST_LEN)
   {
      CM_JE_DEBUG_PRINT ("Request Length is greater than allowed = %d",uiRecvBufLen);
      cmje_close_recv_tnsprtchannel (tnsprt_channel_p->sock_fd_ui);
      return OF_FAILURE;
   }

   tnsprt_channel_p->recv_buf_p = (char *) of_calloc (1, uiRecvBufLen+1);
   if (!tnsprt_channel_p->recv_buf_p)
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation Failed");
      cmje_close_recv_tnsprtchannel (tnsprt_channel_p->sock_fd_ui);
      return OF_FAILURE;
   }

   /* Read Buffer from Socket */
   if ((iBytesRecd = cm_socket_recv (tnsprt_channel_p->sock_fd_ui,
               tnsprt_channel_p->recv_buf_p,
               uiRecvBufLen,
               0)) <= 0)
   {
      perror ("JE-SocketRecvFrom:");
      CM_JE_DEBUG_PRINT ("failed and return value = %d", errno);
      of_free (tnsprt_channel_p->recv_buf_p);
      cmje_close_recv_tnsprtchannel (tnsprt_channel_p->sock_fd_ui);
      return OF_FAILURE;
   }

   CM_JE_DEBUG_PRINT ("Message Length=%d and Recieved %d",
         uiRecvBufLen, iBytesRecd);


   CM_INC_STATS_JE (JE_CFGREQ_RECEIVES);
   /* Restart Idle Timer for Transport Channel */
   cm_timer_restart (tnsprt_channel_p->session_timer_p);

   /* Allocate Memory for Configuration Request Structure */
   config_request_p = (struct je_config_request *) cm_je_cfgrequest_alloc ();
   if (!config_request_p)
   {
      CM_JE_DEBUG_PRINT ("Transport Channel Memory Allocation Failed");
      of_free (tnsprt_channel_p->recv_buf_p);
      cmje_close_recv_tnsprtchannel (tnsprt_channel_p->sock_fd_ui);
      return OF_FAILURE;
   }

   /*Fill Socket Buffer into  Memory for Configuration Request Structure */
   if (cm_je_prepare_config_request_from_message
         (tnsprt_channel_p->recv_buf_p, config_request_p) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Prepare Config Request Failed");
      /* Freeing of structure members taken care by called function */
      of_free (config_request_p);
      of_free (tnsprt_channel_p->recv_buf_p);
      cmje_close_recv_tnsprtchannel (tnsprt_channel_p->sock_fd_ui);
      return OF_FAILURE;
   }


   switch (config_request_p->command_id)
   {
      case CM_COMMAND_SET_TRANSPORT_ATTRIBS:
         CM_JE_DEBUG_PRINT ("Command is Set Transport Channel Attribs");
         cm_tnsprt_channel_properties_handler (config_request_p, tnsprt_channel_p,
               &pUCMResult);
         break;
      case CM_COMMAND_RESPONSE:
         CM_JE_DEBUG_PRINT ("Command is Response");
         //ucmJEResponseHandler(config_request_p);
         break;
      case CM_COMMAND_LDSV_REQUEST:
         CM_JE_DEBUG_PRINT ("Command is Ldsv Request");
         cm_je_ldsv_handler (config_request_p, &pUCMResult);
         break;
      case CM_COMMAND_DM_REQUEST:
         CM_JE_DEBUG_PRINT ("Command is DM Generic");
         cm_je_dm_get_request_handler (config_request_p, &pUCMResult);
         break;
      case CM_COMMAND_APPL_REQUEST:
         CM_JE_DEBUG_PRINT ("Command is Appl Generic");
         cm_je_appl_show_request_handler (config_request_p, &pUCMResult);
         break;
      case CM_COMMAND_CONFIG_SESSION_START:
      case CM_COMMAND_CONFIG_SESSION_UPDATE:
      case CM_COMMAND_CONFIG_SESSION_END:
      case CM_COMMAND_CONFIG_SESSION_GET:
         CM_JE_DEBUG_PRINT ("Command is Session Start/Update/End/Get");
         cm_je_config_session_handler (tnsprt_channel_p->config_session_p, config_request_p,
               &pUCMResult);
         break;
#ifdef CM_STATS_COLLECTOR_SUPPORT
      case CM_COMMAND_STATS_REQUEST:
      case CM_COMMAND_STATS_RESPONSE: 
         CM_JE_DEBUG_PRINT("Command is stats request");
         cm_je_stats_request_handler(tnsprt_channel_p,config_request_p, &pUCMResult);
         break;
#endif
      case  CM_COMMAND_AUTHENTICATE_USER:
         CM_JE_DEBUG_PRINT ("Command is Authenticate user");
         cm_je_authentication_request_handler(config_request_p,&pUCMResult);
         break;
      case CM_COMMAND_VERSION_HISTORY:
         CM_JE_DEBUG_PRINT ("Command is version history_t");
         cm_je_version_history_t_handler(config_request_p,&pUCMResult);
         break;
      case  CM_COMMAND_NOTIFICATION:
         CM_JE_DEBUG_PRINT ("Command is Notification");
         cm_je_notification_handler(config_request_p,&pUCMResult);
         break;
      case CM_COMMAND_REBOOT_DEVICE:
	CM_JE_DEBUG_PRINT("Command is Reboot Device");
        cm_je_rebootdevice_handler(config_request_p,&pUCMResult);	
		break;
      default:
         CM_JE_DEBUG_PRINT ("unhandled Command");
         break;
   }

   if (pUCMResult != NULL)
   {
      switch (config_request_p->command_id)
      {
         case CM_COMMAND_CONFIG_SESSION_START:
            if (pUCMResult->result_code == CM_JE_SUCCESS)
            {
               tnsprt_channel_p->config_session_p =
                  (struct je_config_session *) pUCMResult->data.je_data_p;

               config_session_p =
                  (struct cm_je_config_session *) of_calloc (1,
                        sizeof (struct cm_je_config_session));
               if (config_session_p == NULL)
               {
                  CM_JE_DEBUG_PRINT ("Memory allocation failed");
                  of_free (tnsprt_channel_p->recv_buf_p);
                  cmje_close_recv_tnsprtchannel (tnsprt_channel_p->sock_fd_ui);
                  cm_je_free_ucmresult (config_request_p->command_id, config_request_p->sub_command_id,
                        pUCMResult);
                  cm_je_free_config_request (config_request_p);
                  return OF_FAILURE;
               }
               if (je_copy_cfgsession
                     ((struct je_config_session *) pUCMResult->data.je_data_p,
                      config_session_p) == OF_FAILURE)
               {
                  CM_JE_DEBUG_PRINT ("Copy Config Session Failed");
                  of_free (config_session_p);
                  of_free (tnsprt_channel_p->recv_buf_p);
                  cmje_close_recv_tnsprtchannel (tnsprt_channel_p->sock_fd_ui);
                  cm_je_free_ucmresult (config_request_p->command_id, config_request_p->sub_command_id,
                        pUCMResult);
                  cm_je_free_config_request (config_request_p);
                  return OF_FAILURE;
               }
               pUCMResult->data.je_data_p = config_session_p;
            }
            else
            {
               CM_JE_DEBUG_PRINT ("Session Start Failed");
            }
            break;
         case CM_COMMAND_CONFIG_SESSION_END:
            if (pUCMResult->result_code == CM_JE_SUCCESS)
            {
               tnsprt_channel_p->config_session_p = NULL;
            }
            break;
         case CM_COMMAND_LDSV_REQUEST:
            if (pUCMResult->result_code == CM_JE_SUCCESS)
            {
               if(config_request_p->sub_command_id == CM_CMD_SAVE_CONFIG)
               {
                  if((config_request_p->mgmt_engine_id == CM_CLI_MGMT_ENGINE_ID) ||
                        (config_request_p->mgmt_engine_id ==  CM_HTTP_MGMT_ENGINE_ID))
                  {
                     CM_JE_DEBUG_PRINT("Save Command started");
                     cm_je_free_ucmresult (config_request_p->command_id, config_request_p->sub_command_id,
                           pUCMResult);

                     /*Resetting Transfer Channel Buffer */
                     of_free (tnsprt_channel_p->recv_buf_p);
                     tnsprt_channel_p->recv_buf_p = NULL;

                     struct je_request_node *pJEPendingRequest;
                     pJEPendingRequest=(struct je_request_node *)of_calloc(1,sizeof(struct je_request_node));
                     pJEPendingRequest->fd_ui=tnsprt_channel_p->sock_fd_ui;
                     pJEPendingRequest->config_request_p=config_request_p;
                     CM_DLLQ_APPEND_NODE(&cm_je_request_queue_g, &pJEPendingRequest->list_node);
                     return OF_SUCCESS;
                  }
               }
            }
         default:
            break;
      }

      /* Preperation of RESPOSNSE */
      //  of_memcpy (&GenericHdr, config_request_p, uiGenHdrLen_g);
      of_memset(&GenericHdr,0,sizeof(struct cm_msg_generic_header));
      GenericHdr.command_id = config_request_p->command_id;
      GenericHdr.flags = config_request_p->flags;
      GenericHdr.sequence_id = config_request_p->sequence_id;
      GenericHdr.mgmt_engine_id = config_request_p->mgmt_engine_id;
      of_strncpy(GenericHdr.admin_role,config_request_p->admin_role,of_strlen(config_request_p->admin_role));
      of_strncpy(GenericHdr.admin_name,config_request_p->admin_name,of_strlen(config_request_p->admin_name));

      uiBuffLen += (2 * uiOSIntSize_g) + (6 * uiOSIntSize_g) + of_strlen(config_request_p->admin_role) + of_strlen(config_request_p->admin_name);

      CM_JE_DEBUG_PRINT("GenHeaderLen=%d",uiBuffLen);
      /*Fill Response Structure */
      cm_je_get_response_message_length (config_request_p, pUCMResult, &uiBuffLen);
      CM_JE_DEBUG_PRINT("buffLen=%d",uiBuffLen);

      pResponseBuf = (char *) of_calloc (1, uiBuffLen+1);
      if (!pResponseBuf)
      {
         CM_JE_DEBUG_PRINT ("Response Buffer Memory Allocation Failed");
         of_free (tnsprt_channel_p->recv_buf_p);
         cm_je_free_ucmresult (config_request_p->command_id, config_request_p->sub_command_id,
               pUCMResult);
         cm_je_free_config_request (config_request_p);
         return OF_FAILURE;
      }

      /*Fill Socket Response Buffer */
      pUCMResult->sub_command_id = config_request_p->sub_command_id;
      cm_je_prepare_response_message (&GenericHdr, pUCMResult, pResponseBuf,
            uiBuffLen);
#ifdef CM_STATS_COLLECTOR_SUPPORT
      if(config_request_p->command_id == CM_COMMAND_STATS_RESPONSE)
      {
         pJEParTrChannel = cmje_get_parent_tnsprtchannel_from_list(tnsprt_channel_p->sock_fd_ui);
         sock_fd_ui = pJEParTrChannel->sock_fd_ui;
      }
      else
#endif
      {
         sock_fd_ui = tnsprt_channel_p->sock_fd_ui;
      }
      if ((return_value =
               cm_socket_send (sock_fd_ui, pResponseBuf, uiBuffLen,
                  0)) <= 0)
      {
         perror ("JE-SocketSend:");
         CM_JE_DEBUG_PRINT ("SocketSend Failed with errno = %d RetVal=%d", errno,
               return_value);
         of_free (pResponseBuf);
         of_free (tnsprt_channel_p->recv_buf_p);
         cm_je_free_ucmresult (config_request_p->command_id, config_request_p->sub_command_id,
               pUCMResult);
         cm_je_free_config_request (config_request_p);

#ifdef CM_STATS_COLLECTOR_SUPPORT 
         if(config_request_p->command_id == CM_COMMAND_STATS_RESPONSE)
         { 
            of_free (tnsprt_channel_p->recv_buf_p);
            cmje_close_recv_tnsprtchannel (tnsprt_channel_p->sock_fd_ui);
         }  
#endif 

         return OF_FAILURE;
      }
      CM_INC_STATS_JE (JE_RESPONSE_SENDS);
      CM_JE_DEBUG_PRINT ("Response Message of %d bytes succesfuly sent",
            uiBuffLen);
      of_free (pResponseBuf);
      cm_je_free_ucmresult (config_request_p->command_id, config_request_p->sub_command_id,
            pUCMResult);

#ifdef CM_STATS_COLLECTOR_SUPPORT
      if(config_request_p->command_id == CM_COMMAND_STATS_RESPONSE)
      {
         of_free (tnsprt_channel_p->recv_buf_p);
         cmje_close_recv_tnsprtchannel (tnsprt_channel_p->sock_fd_ui);
      }
#endif
   }

   /*Resetting Transfer Channel Buffer */
   of_free (tnsprt_channel_p->recv_buf_p);
   tnsprt_channel_p->recv_buf_p = NULL;
   /* De Allocating Memory Config Request*/
   cm_je_free_config_request (config_request_p);
   je_print_statistics();
   return OF_SUCCESS;
}

/**
  \ingroup JETRAPI
  This API Deletes Transport Channel for given Socket Desciptors from Global Transport
  Channel List. Closes all configuration Session belongs to the Transport Channel.
  Stops the timer and de-allocates memory for transport channel.


  <b>Input paramameters: </b>\n
  <b><i> sock_fd_ui: </i></b> Socket Descriptor from Management Engines to be closed
  <b>Output Parameters: </b> NONE \n
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : cmje_close_recv_tnsprtchannel 
 ** Description   : This API closes TCP Connection from Management Engine.
 ** Input params  : fd_ui - Socket Descripter
 ** Output params : NONE
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t cmje_close_recv_tnsprtchannel (cm_sock_handle fd_ui)
{
   struct cmje_tnsprtchannel_info *tnsprt_channel_p = NULL;

   CM_JE_DEBUG_PRINT ("Entered");
   tnsprt_channel_p = cmje_get_tnsprtchannel_from_list (fd_ui);
   if (!tnsprt_channel_p)
   {
      CM_JE_DEBUG_PRINT ("Get Transport Channel Failed");
      return OF_FAILURE;
   }
   cmje_tnsprtchannel_close_session (tnsprt_channel_p->config_session_p);

   tnsprt_channel_p->config_session_p = NULL;

   /* Stop Timer for Transport Channel */
   if ((cm_timer_stop (tnsprt_channel_p->session_timer_p)) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Timer Stop Handler failed");
      return OF_FAILURE;
   }
   /*Delete Transport Channel from Global list */
   if ((cmje_delete_tnsprtchannel_from_list (tnsprt_channel_p)) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Delete Transport Channel from global list failed");
      return OF_FAILURE;
   }

   if(tnsprt_channel_p->child_sock_fd_ui != 0)
   {
      if(cmje_close_recv_tnsprtchannel(tnsprt_channel_p->child_sock_fd_ui)!= OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT("Closing transport channel of child failed ");  
      }
   }
   /*Free Memory for  Transport Channel */
   cm_je_tnsprtchannel_free (tnsprt_channel_p);
   /*Close Socket */
   cm_socket_close (fd_ui);
   CM_INC_STATS_JE (JE_TRANSCHANNEL_CLOSED);
   return OF_SUCCESS;
}

/**
  \ingroup JETRAPI
  This API Closes Configuration Session.

  <b>Input paramameters: </b>\n
  <b><i> config_session_p: </i></b> Pointer to Configuration Session.
  <b>Output Parameters: </b> NONE \n
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : cmje_tnsprtchannel_close_session 
 ** Description   : This API closes  configuration session 
 ** Input params  : config_session_p - pointer to Configuration Session
 ** Output params : NONE
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t cmje_tnsprtchannel_close_session (struct je_config_session * config_session_p)
{
   struct cm_result *result_p = NULL;
   struct je_config_request UCMCfgReq = { };

   CM_JE_DEBUG_PRINT ("Entered");

   if (config_session_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("Configuration Session is NULL");
      return OF_FAILURE;
   }

   UCMCfgReq.command_id = CM_COMMAND_CONFIG_SESSION_END;
   UCMCfgReq.sub_command_id = CM_CMD_CONFIG_SESSION_REVOKE;

   /* Revoke configuration session */
   if ((cm_je_config_session_handler (config_session_p, &UCMCfgReq, &result_p)) !=
         OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Session Handler failed");
      return OF_FAILURE;
   }

   /* Revoke Handler will take care of cleaning configuration session */

   /* Free UCM Result allocated by Session Handler */
   cm_je_free_ucmresult (UCMCfgReq.command_id, UCMCfgReq.sub_command_id, result_p);
   return OF_SUCCESS;
}

/**
  \ingroup JETRAPI
  This API does a cleanup activities for a Transport Channel because timeout happened
  from Management Engines.

  <b>Input paramameters: </b>\n
  <b><i> pArg: </i></b> Pointer to Transport Channel
  <b>Output Parameters: </b> NONE \n
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : cmje_tnsprtchannel_timeout 
 ** Description   : This API does clean up activity for a Transport Channel.
 ** Input params  : pArg - pointer to struct cmje_tnsprtchannel_info
 ** Output params : NONE
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
void cmje_tnsprtchannel_timeout (void * pArg)
{
   struct cmje_tnsprtchannel_info *tnsprt_channel_p = (struct cmje_tnsprtchannel_info *) pArg;
   struct je_config_session *config_session_p;
   struct je_config_request UCMCfgReq;
   struct cm_result *result_p = NULL;

   CM_JE_DEBUG_PRINT ("TimeOut occured");

   /* Extract configuration session for Transport Channel */
   config_session_p = tnsprt_channel_p->config_session_p;
   if (config_session_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("Configuration Session is NULL");
      return;
   }

   UCMCfgReq.command_id = CM_COMMAND_CONFIG_SESSION_TIMEOUT;

   /* Revoke configuration session */
   if ((cm_je_config_session_handler (config_session_p, &UCMCfgReq, &result_p)) !=
         OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Handling TiemOut failed");
      return;
   }

   /* Freeing Configuration Session and cache done by time out handler  */

   /* Free UCM Result allocated by Session Handler */
   cm_je_free_ucmresult (UCMCfgReq.command_id, UCMCfgReq.sub_command_id, result_p);

   /* Stop Timer for Transport Channel */
   if ((cm_timer_stop (tnsprt_channel_p->session_timer_p)) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Timer Stop Handler failed");
      return;
   }

   /*Delete Transport Channel from Global list */
   if ((cmje_delete_tnsprtchannel_from_list (tnsprt_channel_p)) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Delete Transport Channel from global list failed");
      return;
   }

   /* Closing Socket */
   cm_socket_close (tnsprt_channel_p->sock_fd_ui);

   /*Free Memory for  Transport Channel */
   cm_je_tnsprtchannel_free (tnsprt_channel_p);

   return;
}

/**
  \ingroup JETRAPI
  This API frames Configuration Request from Data recieved from management Engines.

  <b>Input paramameters: </b>\n
  <b><i> pRequestBuf: </i></b> Pointer to character buffer.
  <b>Output Parameters: </b>\n
  <b><i> config_request_p: </i></b> Pointer to UCM confguration request structure.
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : cm_je_prepare_config_request_from_message 
 ** Description   : This API Frames Configuration Request from Data recieved from
 **                  from Management Engines.
 ** Input params  : pRequestBuf - Character Pointer to input buffer
 ** Output params : config_request_p - Pointer to UCM Configuration Request
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
 int32_t cm_je_prepare_config_request_from_message (char * pRequestBuf,
      struct je_config_request *
      config_request_p)
{
   unsigned char *pTmpReqBuf = pRequestBuf;
   struct je_config_request *pConfigRequest = config_request_p;
   unsigned char *pRequestHeader = NULL;
   unsigned char *pRequestData = NULL;
   uint32_t uiDMPathLen = 0;
   uint32_t uiNvCnt = 0, type;
   uint32_t uiLen = 0;
   uint32_t uiAdminNameLen = 0,uiAdminRoleLen=0;
   struct cm_dm_role_permission * role_perm_p=NULL;
   uint64_t ulData=0;
   uint64_t *pData;

   if (pTmpReqBuf == NULL)
   {
      CM_JE_DEBUG_PRINT ("Input Request is NULL");
      return OF_FAILURE;
   }

   /* Copy Message length from buffer */
   pConfigRequest->length = of_mbuf_get_32 (pTmpReqBuf);
   pTmpReqBuf += uiOSIntSize_g;

   /* Copy command_id from buffer */
   pConfigRequest->command_id = of_mbuf_get_32 (pTmpReqBuf);
   pTmpReqBuf += uiOSIntSize_g;

   /* Copy flags from buffer */
   pConfigRequest->flags = of_mbuf_get_32 (pTmpReqBuf);
   pTmpReqBuf += uiOSIntSize_g;

   /* Copy sequence_id from buffer */
   pConfigRequest->sequence_id = of_mbuf_get_32 (pTmpReqBuf);
   pTmpReqBuf += uiOSIntSize_g;

   /* Copy mgmt_engine_id from buffer */
   pConfigRequest->mgmt_engine_id = of_mbuf_get_32 (pTmpReqBuf);
   pTmpReqBuf += uiOSIntSize_g;

   uiAdminRoleLen = of_mbuf_get_32(pTmpReqBuf); 
   pTmpReqBuf += uiOSIntSize_g;

   if ( uiAdminRoleLen > 0 )
   {
      of_memcpy (pConfigRequest->admin_role, pTmpReqBuf, uiAdminRoleLen);
      pTmpReqBuf += uiAdminRoleLen;
   }

   uiAdminNameLen = of_mbuf_get_32(pTmpReqBuf);
   pTmpReqBuf += uiOSIntSize_g;

   if(uiAdminNameLen > 0 )
   {
      of_memcpy (pConfigRequest->admin_name, pTmpReqBuf, uiAdminNameLen);
      pTmpReqBuf += uiAdminNameLen;
   }

#if 0
   /* Copy Generic header */
   of_memcpy (pConfigRequest, pTmpReqBuf, uiGenHdrLen_g);
   pTmpReqBuf += uiGenHdrLen_g;
#endif


   CM_JE_DEBUG_PRINT
      ("MgmtEngine =%d Command=%d Lenght=%d AdminName=%s AdminRole=%s",
       pConfigRequest->mgmt_engine_id, pConfigRequest->command_id,
       pConfigRequest->length, pConfigRequest->admin_name,
       pConfigRequest->admin_role);

   /* Get the Request Header poinder */
   pRequestHeader = pTmpReqBuf;

   /* Copy Sub Command */
   pConfigRequest->sub_command_id = of_mbuf_get_32 (pRequestHeader);
   //  pRequestHeader += uiInt32Size_g;
   pRequestHeader += uiOSIntSize_g;

   /* Allocate Memory for DM Path  */
   uiDMPathLen = of_mbuf_get_32 (pRequestHeader);
   pRequestHeader += uiOSIntSize_g;
   if (uiDMPathLen > 0)
   {
      if (uiDMPathLen > CM_MAX_REQUEST_LEN)
      {
         CM_JE_DEBUG_PRINT ("Dm Path Length is greater than Allowed request length");
         return OF_FAILURE;
      }

      pConfigRequest->dm_path_p = (char *) of_calloc (1, uiDMPathLen + 1);
      if (!pConfigRequest->dm_path_p)
      {
         CM_JE_DEBUG_PRINT ("Memory Allocation Failed for DM Path");
         return OF_FAILURE;
      }
      /* copy DM Path */
      of_memcpy (pConfigRequest->dm_path_p, pRequestHeader, uiDMPathLen);
      pRequestHeader += uiDMPathLen;
      CM_JE_DEBUG_PRINT ("DM Path=%s", pConfigRequest->dm_path_p);
   }

   //Read Data 
   pRequestData = pRequestHeader;

   switch (pConfigRequest->sub_command_id)
   {
      case CM_CMD_ADD_PARAMS:
      case CM_CMD_SET_PARAMS:
      case CM_CMD_DEL_PARAMS:
      case CM_CMD_CANCEL_PREV_CMD:
      case CM_CMD_COMPARE_PARAM:
      case CM_CMD_IS_DM_INSTANCE_USING_NAMEPATH_AND_KEYARRAY:
      case CM_SUBCOMMAND_AUTHENTICATE_USER:

#ifdef CM_STATS_COLLECTOR_SUPPORT
      case CM_CMD_GET_AGGREGATE_STATS:
      case CM_CMD_GET_AVERAGE_STATS:
      case CM_CMD_GET_PER_DEVICE_STATS: 
#endif 
         /* Copy NV Pair Count */
         pConfigRequest->data.nv_pair_array.count_ui = of_mbuf_get_32 (pRequestData);
         pRequestData += uiOSIntSize_g;

         if(pConfigRequest->data.nv_pair_array.count_ui > CM_MAX_NVPAIR_COUNT)
         {
            CM_JE_DEBUG_PRINT ("Exceeding Allowed Nv Pair Count");
            of_free (pConfigRequest->dm_path_p);
            return OF_FAILURE;
         }

         /* Allocate Memory for NV Pairs */
         pConfigRequest->data.nv_pair_array.nv_pairs = (struct cm_nv_pair *)
            of_calloc (pConfigRequest->data.nv_pair_array.count_ui,
                  sizeof (struct cm_nv_pair));
         if (!(pConfigRequest->data.nv_pair_array.nv_pairs))
         {
            CM_JE_DEBUG_PRINT ("Memory Allocation Failed for Nv Pairs");
            of_free (pConfigRequest->dm_path_p);
            return OF_FAILURE;
         }

         /* Copy Each Nv Pair */
         for (uiNvCnt = 0, uiLen = 0;
               uiNvCnt < pConfigRequest->data.nv_pair_array.count_ui;
               uiNvCnt++, pRequestData += uiLen, uiLen = 0)
         {
            if ((cm_je_copy_nvpair_from_buffer (pRequestData,
                        &(pConfigRequest->data.
                           nv_pair_array.nv_pairs[uiNvCnt]),
                        &uiLen)) != OF_SUCCESS)
            {
               CM_JE_DEBUG_PRINT ("Copy Name Value Pairs failed ");
               of_free (pConfigRequest->dm_path_p);
               return OF_FAILURE;
            }
         }
         break;

      case CM_CMD_GET_FIRST_N_LOGS:
      case CM_CMD_GET_NEXT_N_LOGS:
      case CM_CMD_GET_FIRST_N_RECS:
      case CM_CMD_GET_NEXT_N_RECS:
      case CM_CMD_GET_EXACT_REC:

         /* Copy Record Count */
         pConfigRequest->data.sec_appl.count_ui = of_mbuf_get_32 (pRequestData);
         pRequestData += uiOSIntSize_g;

         if(pConfigRequest->data.sec_appl.count_ui >  CM_MAX_RECORD_COUNT)
         {
            CM_JE_DEBUG_PRINT ("Exceeding Allowed Record Count");
            of_free (pConfigRequest->dm_path_p);
            return OF_FAILURE;
         }

         /* Copy NV Key Pair Array Count */
         pConfigRequest->data.sec_appl.key_nv_pairs.count_ui =
            of_mbuf_get_32 (pRequestData);
         pRequestData += uiOSIntSize_g;

         if(pConfigRequest->data.sec_appl.key_nv_pairs.count_ui > CM_MAX_NVPAIR_COUNT)
         {
            CM_JE_DEBUG_PRINT ("Exceeding Allowed Nv Pair Count");
            of_free (pConfigRequest->dm_path_p);
            return OF_FAILURE;
         }
         if (pConfigRequest->data.sec_appl.key_nv_pairs.count_ui)
         {
            /* Memory Allocation for NV Key Pairs */
            pConfigRequest->data.sec_appl.key_nv_pairs.nv_pairs =
               of_calloc (pConfigRequest->data.sec_appl.key_nv_pairs.count_ui,
                     sizeof (struct cm_nv_pair));
            if (!(pConfigRequest->data.sec_appl.key_nv_pairs.nv_pairs))
            {
               CM_JE_DEBUG_PRINT
                  ("Memory Allocation Failed for Security Application  Nv Pairs");
               of_free (pConfigRequest->dm_path_p);
               return OF_FAILURE;
            }
            /* Copy Each Nv Pair */
            for (uiNvCnt = 0, uiLen = 0;
                  uiNvCnt < pConfigRequest->data.sec_appl.key_nv_pairs.count_ui;
                  uiNvCnt++, pRequestData += uiLen, uiLen = 0)
            {
               if ((cm_je_copy_nvpair_from_buffer (pRequestData,
                           &(pConfigRequest->data.sec_appl.
                              key_nv_pairs.nv_pairs[uiNvCnt]),
                           &uiLen)) != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("Copy Name Value Pairs failed ");
                  of_free (pConfigRequest->dm_path_p);
                  return OF_FAILURE;
               }
            }
         }

         if (pConfigRequest->sub_command_id == CM_CMD_GET_NEXT_N_RECS)
         {
#if 0
            if ((cm_je_copy_nvpair_from_buffer (pRequestData,
                        &(pConfigRequest->data.sec_appl.
                           prev_rec_key_nv_pairs),
                        &uiLen)) != OF_SUCCESS)
            {
               CM_JE_DEBUG_PRINT ("Copy Name Value Pairs failed ");
               of_free (pConfigRequest->dm_path_p);
               return OF_FAILURE;
            }
#endif

            /* Copy NV Key Pair Array Count */
            pConfigRequest->data.sec_appl.prev_rec_key_nv_pairs.count_ui =
               of_mbuf_get_32 (pRequestData);
            pRequestData += uiOSIntSize_g;

            if(pConfigRequest->data.sec_appl.prev_rec_key_nv_pairs.count_ui > CM_MAX_NVPAIR_COUNT)
            {
               CM_JE_DEBUG_PRINT ("Exceeding Allowed Nv Pair Count");
               of_free (pConfigRequest->dm_path_p);
               return OF_FAILURE;
            }
            if (pConfigRequest->data.sec_appl.prev_rec_key_nv_pairs.count_ui)
            {
               /* Memory Allocation for NV Key Pairs */
               pConfigRequest->data.sec_appl.prev_rec_key_nv_pairs.nv_pairs =
                  of_calloc (pConfigRequest->data.sec_appl.prev_rec_key_nv_pairs.count_ui,
                        sizeof (struct cm_nv_pair));
               if (!(pConfigRequest->data.sec_appl.prev_rec_key_nv_pairs.nv_pairs))
               {
                  CM_JE_DEBUG_PRINT
                     ("Memory Allocation Failed for Security Application  Nv Pairs");
                  of_free (pConfigRequest->dm_path_p);
                  return OF_FAILURE;
               }
               /* Copy Each Nv Pair */
               for (uiNvCnt = 0, uiLen = 0;
                     uiNvCnt < pConfigRequest->data.sec_appl.prev_rec_key_nv_pairs.count_ui;
                     uiNvCnt++, pRequestData += uiLen, uiLen = 0)
               {
                  if ((cm_je_copy_nvpair_from_buffer (pRequestData,
                              &(pConfigRequest->data.sec_appl.
                                 prev_rec_key_nv_pairs.nv_pairs[uiNvCnt]),
                              &uiLen)) != OF_SUCCESS)
                  {
                     CM_JE_DEBUG_PRINT ("Copy Name Value Pairs failed ");
                     of_free (pConfigRequest->dm_path_p);
                     return OF_FAILURE;
                  }
               }
            }
         }
         break;
      case CM_CMD_LOAD_CONFIG:
         break;

      case CM_CMD_SAVE_CONFIG:
         break;

      case CM_CMD_SET_DEFAULTS:
         break;

      case CM_CMD_GET_DM_FIRST_NODE_INFO:
         type = of_mbuf_get_32 (pRequestData);
         pConfigRequest->data.data_model.type = type;
         CM_JE_DEBUG_PRINT ("type = %d", type);
         break;

      case CM_CMD_GET_DM_NEXT_NODE_INFO:
      case CM_CMD_GET_DM_NEXT_TABLE_ANCHOR_NODE_INFO:
         /* Get Traversal type */
         pConfigRequest->data.data_model.type = of_mbuf_get_32 (pRequestData);
         pRequestData = pRequestData + uiOSIntSize_g;

         /* Get Opaque Info length */
         pConfigRequest->data.data_model.opaque_info_length = of_mbuf_get_32 (pRequestData);
         pRequestData = pRequestData + uiOSIntSize_g;

         /* Get Opaque Info */
         pConfigRequest->data.data_model.opaque_info_p =
            (uint32_t *) of_calloc (pConfigRequest->data.data_model.opaque_info_length,
                  uiOSIntSize_g);
         if (!pConfigRequest->data.data_model.opaque_info_p)
         {
            CM_JE_DEBUG_PRINT ("Memory allocation failed");
            /* TODO :: Free any allocated memory */
            if(pConfigRequest->dm_path_p)
               of_free (pConfigRequest->dm_path_p);
            return OF_FAILURE;
         }
         of_memcpy (pConfigRequest->data.data_model.opaque_info_p, pRequestData,
               (pConfigRequest->data.data_model.opaque_info_length* uiOSIntSize_g));
         pRequestData += (pConfigRequest->data.data_model.opaque_info_length*uiOSIntSize_g);

         break;
      case CM_CMD_IS_DM_INSTANCE_USING_NAMEPATH_AND_KEY:
         /* Key Value Type */
         pConfigRequest->data.data_model.type = of_mbuf_get_32 (pRequestData);
         pRequestData = pRequestData + uiOSIntSize_g;
         /*Here Opaque Len is Instance Key Length */
         pConfigRequest->data.data_model.opaque_info_length = of_mbuf_get_32 (pRequestData);
         pRequestData = pRequestData + uiOSIntSize_g;
         /*Here Opaque Info is Instance Key */
         pConfigRequest->data.data_model.opaque_info_p = of_calloc (1,
               pConfigRequest->
               data.data_model.
               opaque_info_length + 1);
         if (!pConfigRequest->data.data_model.opaque_info_p)
         {
            CM_JE_DEBUG_PRINT ("Memory allocation failed");
            /* TODO :: Free any allocated memory */
            if(pConfigRequest->dm_path_p)
               of_free (pConfigRequest->dm_path_p);
            return OF_FAILURE;
         }
         //of_memcpy (pConfigRequest->data.data_model.opaque_info_p, pRequestData,
         //        pConfigRequest->data.data_model.opaque_info_length);
         of_strncpy (pConfigRequest->data.data_model.opaque_info_p, pRequestData,
               pConfigRequest->data.data_model.opaque_info_length);
         pRequestData += pConfigRequest->data.data_model.opaque_info_length;
         break;

      case CM_CMD_IS_DM_CHILD_NODE:
         /*Here Opaque Len  Parent DM Path Length */
         pConfigRequest->data.data_model.opaque_info_length = of_mbuf_get_32 (pRequestData);
         pRequestData = pRequestData + uiOSIntSize_g;
         /*Here Opaque Info is  Parent DM Path  */
         pConfigRequest->data.data_model.opaque_info_p = of_calloc (1,
               pConfigRequest->
               data.data_model.
               opaque_info_length + 1);
         if (!pConfigRequest->data.data_model.opaque_info_p)
         {
            CM_JE_DEBUG_PRINT ("Memory allocation failed");
            /* TODO :: Free any allocated memory */
            if(pConfigRequest->dm_path_p)
               of_free (pConfigRequest->dm_path_p);
            return OF_FAILURE;
         }
         of_memcpy (pConfigRequest->data.data_model.opaque_info_p, pRequestData,
               pConfigRequest->data.data_model.opaque_info_length);
         pRequestData += pConfigRequest->data.data_model.opaque_info_length;
         break;

      case CM_CMD_GET_DM_NODE_INFO:
      case CM_CMD_GET_DM_NODE_AND_CHILD_INFO:
      case CM_CMD_GET_DM_KEY_CHILDS:
      case CM_CMD_GET_DM_CHILD_NODE_COUNT:
         break;
      case CM_CMD_SET_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_GET_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_DEL_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_GET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_SET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_DEL_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
         role_perm_p=(struct cm_dm_role_permission *) of_calloc
            (1,sizeof(struct cm_dm_role_permission));
         if(!role_perm_p)
         {
            CM_JE_DEBUG_PRINT ("Memory allocation failed");
            /* TODO :: Free any allocated memory */
            if(pConfigRequest->dm_path_p)
               of_free (pConfigRequest->dm_path_p);
            return OF_FAILURE;
         }
         of_memcpy(role_perm_p,pRequestData,sizeof(struct cm_dm_role_permission));
         pRequestData += sizeof(struct cm_dm_role_permission);
         pConfigRequest->data.data_model.opaque_info_p = (void *)role_perm_p;
         break;
      case CM_CMD_GET_DM_NODE_ROLE_ATTRIBS:
      case CM_CMD_GET_DM_INSTANCE_ROLE_ATTRIBS:
         /* No specific input is needed*/
         break;

#if UCMDM_INSTANCE_ROLE_AND_PERMISSIONS_USING_KEY
      case CM_CMD_GET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_SET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_DEL_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
         role_perm_p=(struct cm_dm_role_permission *) of_calloc
            (1,sizeof(struct cm_dm_role_permission));
         if(!role_perm_p)
         {
            CM_JE_DEBUG_PRINT ("Memory allocation failed");
            if(pConfigRequest->dm_path_p)
               of_free (pConfigRequest->dm_path_p);
            return OF_FAILURE;
         }
         of_memcpy(role_perm_p,pRequestData,sizeof(struct cm_dm_role_permission));
         pRequestData += sizeof(struct cm_dm_role_permission);
         pConfigRequest->data.data_model.opaque_info_p = (void *)role_perm_p;
         pConfigRequest->data.data_model.type=of_mbuf_get_32(pRequestData);
         pRequestData += uiOSIntSize_g;
         pConfigRequest->data.data_model.opaque_info_length=of_mbuf_get_32(pRequestData);
         pRequestData += uiOSIntSize_g;
         pConfigRequest->data.data_model.key_p=of_calloc(1,
               pConfigRequest->data.data_model.opaque_info_length+1);
         if(!pConfigRequest->data.data_model.key_p)
         {
            CM_JE_DEBUG_PRINT ("Memory allocation failed");
            if(pConfigRequest->dm_path_p)
               of_free (pConfigRequest->dm_path_p);
            of_free(pConfigRequest->data.data_model.opaque_info_p);
            return OF_FAILURE;
         }
         of_memcpy(pConfigRequest->data.data_model.key_p,pRequestData,
               pConfigRequest->data.data_model.opaque_info_length);
         break;
#endif
      case CM_CMD_GET_NEW_DM_INSTANCE_ID:
      case CM_CMD_ADD_DM_INSTANCE_MAP:
      case CM_CMD_DEL_DM_INSTANCE_MAP:
         break;
         /* version history_t commands*/
      case CM_CMD_GET_DELTA_VERSION_HISTORY:
         pConfigRequest->data.version.version = of_mbuf_get_64 (pRequestData);
         pRequestData += uiInt64Size_g;
         pConfigRequest->data.version.end_version = of_mbuf_get_64 (pRequestData);
         pRequestData += uiInt64Size_g;
         break;
         /* Notification commands */
      case CM_CMD_NOTIFY_JE:
         CM_JE_DEBUG_PRINT("CM_CMD_NOTIFY_JE Command");
         pConfigRequest->data.Notify.event = of_mbuf_get_32 (pRequestData);
         pRequestData += uiOSIntSize_g;
         pData=of_calloc(1, uiInt64Size_g);
         ulData=of_mbuf_get_64(pRequestData);
         *pData=ulData;
         pConfigRequest->data.Notify.pData=(void *)pData;
         pRequestData += uiInt64Size_g;
         CM_JE_DEBUG_PRINT("version num %llx", ulData);
         break;
#ifdef CM_STATS_COLLECTOR_SUPPORT
      case CM_CMD_COALESCAR_RESP:
         pConfigRequest->data.stats_data.result_code = of_mbuf_get_32 (pRequestData);
         pRequestData += uiOSIntSize_g;
         pConfigRequest->data.stats_data.nv_pair_flat_buf_len_ui= of_mbuf_get_32 (pRequestData);
         pRequestData += uiOSIntSize_g;
         pConfigRequest->data.stats_data.nv_pair_flat_buf_p = (char*)of_calloc(1,
               pConfigRequest->data.stats_data.nv_pair_flat_buf_len_ui);
         of_memcpy(pConfigRequest->data.stats_data.nv_pair_flat_buf_p,
               pRequestData,
               pConfigRequest->data.stats_data.nv_pair_flat_buf_len_ui);

         break;
#endif

   }
   //CM_JE_DEBUG_PRINT ("Success");
   return OF_SUCCESS;
}

/**
  \ingroup JETRAPI
  This API Prepare character Buffer from Response Message structure.

  <b>Input paramameters: </b>\n
  <b><i> pUCMGenHeader: </i></b> Pointer to UCM Generic Header.
  <b><i> pResponseMsg: </i></b> Pointer to UCM Response Message.
  <b>Output Parameters: </b>\n
  <b><i> pRespBuf: </i></b> Pointer to UCM Response Character Buffer.
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : cm_je_prepare_response_message 
 ** Description   : This API Prepare character Buffer from Response Message structure
 ** Input params  : pUCMGenHeader - Pointer to Generic Header
 **               : pResponseMsg - Pointer to UCM Response Message structure
 ** Output params : pRespBuf - Pointer to UCM response character buffer
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
 int32_t cm_je_prepare_response_message (struct cm_msg_generic_header *
      pUCMGenHeader,
      struct cm_result * pResponseMsg,
      char * pRespBuf,
      uint32_t uiBuffLen)
{
   char *pUCMTempRespBuf = pRespBuf;
   struct cm_msg_generic_header *pGenericHeader = pUCMGenHeader;
   struct cm_result *cm_resp_msg_p = pResponseMsg;
   struct cm_dm_instance_map *pInstanceMap;
   struct cm_dm_array_of_role_permissions *role_perm_ary_p;
   uint32_t uiNvCnt = 0;
   uint32_t uiCnt = 0;
   uint32_t uiLen = 0;
   uint32_t uiSessionLen = 0;
   uint32_t uiDMPathLen = 0;
   uint32_t uiGenHdrLen = 0;
   int32_t ii;

   CM_JE_DEBUG_PRINT ("entered");
   if (pGenericHeader == NULL)
   {
      CM_JE_DEBUG_PRINT ("Generic Header is NULL");
      return OF_FAILURE;
   }
   if (cm_resp_msg_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("Response Message is NULL");
      return OF_FAILURE;
   }

   /* Copy Total length from buffer */
   pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf, uiBuffLen);

   /* Copy header CommandId */
   //of_memcpy (pUCMTempRespBuf, pGenericHeader, uiGenHdrLen_g);
   //pUCMTempRespBuf += uiGenHdrLen_g;
   uiGenHdrLen = (6 * uiOSIntSize_g) + of_strlen(pGenericHeader->admin_role) + of_strlen(pGenericHeader->admin_name);

   pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf, uiGenHdrLen);
   pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf, pGenericHeader->command_id);
   pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf, pGenericHeader->flags);
   pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf, pGenericHeader->sequence_id);
   pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf, pGenericHeader->mgmt_engine_id);

   pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf, of_strlen(pGenericHeader->admin_role));
   of_memcpy(pUCMTempRespBuf,pGenericHeader->admin_role,of_strlen(pGenericHeader->admin_role));
   pUCMTempRespBuf += of_strlen(pGenericHeader->admin_role);


   pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf, of_strlen(pGenericHeader->admin_name));
   of_memcpy(pUCMTempRespBuf,pGenericHeader->admin_name,of_strlen(pGenericHeader->admin_name));
   pUCMTempRespBuf += of_strlen(pGenericHeader->admin_name);


   //RESPONSE HEADER
   /* Copy Sub Command */
   pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf, cm_resp_msg_p->sub_command_id);

   /* Copy Result Code */
   pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf, cm_resp_msg_p->result_code);

   /* Copy Configuration Session for start command */
   if (pGenericHeader->command_id == CM_COMMAND_CONFIG_SESSION_START)
   {
      switch (cm_resp_msg_p->result_code)
      {
         case CM_JE_SUCCESS:
            {
               uiSessionLen=0;
               if (je_copy_cfgsession_to_buffer
                     ((struct cm_je_config_session *) cm_resp_msg_p->data.je_data_p,
                      pUCMTempRespBuf, &uiSessionLen) == OF_FAILURE)
               {
                  CM_JE_DEBUG_PRINT ("Copy Config Session Failed");
                  return OF_FAILURE;
               }
               pUCMTempRespBuf += uiSessionLen;
               break;
            }
         case CM_JE_FAILURE:
            {
               CM_JE_DEBUG_PRINT ("Failure.. Session not copied");
               break;
            }
      }
   }

   //RESPONSE MESSAGE
   /* Copy Data */
   switch (cm_resp_msg_p->sub_command_id)
   {
      case CM_CMD_CONFIG_SESSION_COMMIT:
         switch (cm_resp_msg_p->result_code)
				 {
						case CM_JE_SUCCESS:
							 /* Copy Success Code */
							 pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf,cm_resp_msg_p->result.success.reserved);
							 break;
            case CM_JE_FAILURE:
               uiLen = 0;
               cm_je_copy_errorinfo_to_buffer (pUCMTempRespBuf, cm_resp_msg_p, &uiLen);
               pUCMTempRespBuf += uiLen;
               break;
        }
			 break;
      case CM_CMD_CONFIG_SESSION_REVOKE:
      case CM_CMD_ADD_PARAMS:
      case CM_CMD_SET_PARAMS:
      case CM_CMD_DEL_PARAMS:
      case CM_CMD_CANCEL_TRANS_CMD:
      case CM_CMD_EXEC_TRANS_CMD:
         switch (cm_resp_msg_p->result_code)
         {
            case CM_JE_SUCCESS:
               break;
            case CM_JE_FAILURE:
               uiLen = 0;
               cm_je_copy_errorinfo_to_buffer (pUCMTempRespBuf, cm_resp_msg_p, &uiLen);
               pUCMTempRespBuf += uiLen;
               break;
         }
         break;
      case CM_CMD_LOAD_CONFIG:
      case CM_CMD_SAVE_CONFIG:
      case CM_CMD_RESET_TO_FACTORY_DEFAULTS:
      case CM_CMD_NOTIFY_JE:
      case CM_CMD_FLUSH_CONFIG:
      case CM_CMD_REBOOT_DEVICE:
         switch (cm_resp_msg_p->result_code)
         {
            case CM_JE_SUCCESS:
               break;
            case CM_JE_FAILURE:
               uiLen = 0;
               cm_je_copy_errorinfo_to_buffer (pUCMTempRespBuf, cm_resp_msg_p, &uiLen);
               pUCMTempRespBuf += uiLen;
               break;
         }
         break;
      case CM_CMD_COMPARE_PARAM:
      case CM_SUBCOMMAND_AUTHENTICATE_USER:
      case CM_CMD_GET_VERSIONS:
         switch (cm_resp_msg_p->result_code)
         {
            case CM_JE_SUCCESS:
               {
                  struct cm_array_of_nv_pair *pNvPairAry =
                     (struct cm_array_of_nv_pair *) cm_resp_msg_p->data.je_data_p;

                  /* Copy NvPair-Count */
                  pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf, pNvPairAry->count_ui);

                  /* Copy  Name Value Pair */
                  for (uiNvCnt = 0, uiLen = 0;
                        uiNvCnt < pNvPairAry->count_ui;
                        uiNvCnt++, pUCMTempRespBuf += uiLen, uiLen = 0)
                  {
                     if (cm_je_copy_nvpair_to_buffer
                           (&pNvPairAry->nv_pairs[uiNvCnt], pUCMTempRespBuf,
                            &uiLen) != OF_SUCCESS)
                     {
                        CM_JE_DEBUG_PRINT ("Copy Name Value Pairs into Buffer Failed");
                        return OF_FAILURE;
                     }
                  }
                  break;
               }
            case CM_JE_FAILURE:
               {
                  uiLen = 0;
                  cm_je_copy_errorinfo_to_buffer (pUCMTempRespBuf, cm_resp_msg_p, &uiLen);
                  pUCMTempRespBuf += uiLen;
               }
         }
         break;
      case CM_CMD_CANCEL_PREV_CMD:
         switch (cm_resp_msg_p->result_code)
         {
            case CM_JE_SUCCESS:
               break;
            case CM_JE_FAILURE:
               {
                  uiLen = 0;
                  cm_je_copy_errorinfo_to_buffer (pUCMTempRespBuf, cm_resp_msg_p, &uiLen);
                  pUCMTempRespBuf += uiLen;
               }
         }
         break;
      case CM_CMD_GET_CONFIG_SESSION:
         switch (cm_resp_msg_p->result_code)
         {
            case CM_JE_SUCCESS:
               {
                  uint32_t no_of_commands;
                  struct cm_array_of_commands *pAryOfCmds = cm_resp_msg_p->data.je_data_p;

                  /* Command Count */
                  pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf, pAryOfCmds->count_ui);

                  for (no_of_commands = 0; no_of_commands < pAryOfCmds->count_ui; no_of_commands++)
                  {
                     /* Command ID */
                     pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf,
                           pAryOfCmds->
                           Commands[no_of_commands].command_id);

                     /* DM Path Length Field */
                     uiDMPathLen = of_strlen (pAryOfCmds->Commands[no_of_commands].dm_path_p);
                     pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf, uiDMPathLen);
                     if (uiDMPathLen > 0)
                     {
                        of_memcpy (pUCMTempRespBuf,
                              pAryOfCmds->Commands[no_of_commands].dm_path_p, uiDMPathLen);
                        pUCMTempRespBuf += uiDMPathLen;
                     }

                     /* Copy NvPair-Count */
                     pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf,
                           pAryOfCmds->
                           Commands[no_of_commands].nv_pair_array.
                           count_ui);

                     /* Copy  Name Value Pair */
                     for (uiNvCnt = 0, uiLen = 0;
                           uiNvCnt <
                           pAryOfCmds->Commands[no_of_commands].nv_pair_array.count_ui;
                           uiNvCnt++, pUCMTempRespBuf += uiLen, uiLen = 0)
                     {
                        if (cm_je_copy_nvpair_to_buffer
                              (&
                               (pAryOfCmds->Commands[no_of_commands].nv_pair_array.
                                nv_pairs[uiNvCnt]), pUCMTempRespBuf, &uiLen) != OF_SUCCESS)
                        {
                           CM_JE_DEBUG_PRINT ("Copy Name Value Pairs into Buffer Failed");
                           return OF_FAILURE;
                        }
                     }
                  }
                  break;
               }
            case CM_JE_FAILURE:
               {
                  uiLen = 0;
                  cm_je_copy_errorinfo_to_buffer (pUCMTempRespBuf, cm_resp_msg_p, &uiLen);
                  pUCMTempRespBuf += uiLen;
               }
         }
         break;
      case  CM_CMD_GET_DELTA_VERSION_HISTORY:
         switch (cm_resp_msg_p->result_code)
         {
            case CM_JE_SUCCESS:
               {
                  struct cm_version *version_list_p= pResponseMsg->data.version.version_list_p;
                  uint32_t uiversionCnt = 0;
                  uint32_t no_of_commands;

                  /* Number of versions */
                  pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf,pResponseMsg->data.version.count_ui);

                  /* for each version */
                  CM_JE_DEBUG_PRINT ("version count %d", pResponseMsg->data.version.count_ui);
                  for (uiversionCnt = 0; uiversionCnt <  pResponseMsg->data.version.count_ui; uiversionCnt++)
                  {
                     struct cm_array_of_commands *pAryOfCmds= version_list_p[uiversionCnt].command_list_p;

                     /* version Number */
                     CM_JE_DEBUG_PRINT ("version %llx", version_list_p[uiversionCnt].version);
                     pUCMTempRespBuf = (char *)of_mbuf_put_64 (pUCMTempRespBuf, version_list_p[uiversionCnt].version);

                     /* Command Count */
                     pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf, pAryOfCmds->count_ui);

                     for (no_of_commands = 0; no_of_commands < pAryOfCmds->count_ui; no_of_commands++)
                     {
                        /* Command ID */
                        pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf,
                              pAryOfCmds->Commands[no_of_commands].command_id);

                        /* DM Path Length Field */
                        if(pAryOfCmds->Commands[no_of_commands].dm_path_p)
                        {
                           uiDMPathLen = of_strlen (pAryOfCmds->Commands[no_of_commands].dm_path_p);
                        }
                        else
                        {
                           uiDMPathLen=0;
                        }

                        pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf, uiDMPathLen);
                        if (uiDMPathLen > 0)
                        {
                           CM_JE_DEBUG_PRINT ("dmpath %s", pAryOfCmds->Commands[no_of_commands].dm_path_p);
                           of_memcpy (pUCMTempRespBuf,
                                 pAryOfCmds->Commands[no_of_commands].dm_path_p, uiDMPathLen);
                           pUCMTempRespBuf += uiDMPathLen;
                        }

                        /* Copy NvPair-Count */
                        pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf,
                              pAryOfCmds->Commands[no_of_commands].nv_pair_array.count_ui);

                        /* Copy  Name Value Pair */
                        for (uiNvCnt = 0, uiLen = 0;
                              uiNvCnt <
                              pAryOfCmds->Commands[no_of_commands].nv_pair_array.count_ui;
                              uiNvCnt++, pUCMTempRespBuf += uiLen, uiLen = 0)
                        {
                           if (cm_je_copy_nvpair_to_buffer
                                 (&
                                  (pAryOfCmds->Commands[no_of_commands].nv_pair_array.
                                   nv_pairs[uiNvCnt]), pUCMTempRespBuf, &uiLen) != OF_SUCCESS)
                           {
                              CM_JE_DEBUG_PRINT ("Copy Name Value Pairs into Buffer Failed");
                              return OF_FAILURE;
                           }
                        }
                     }
                  }
               }
               break;
            case CM_JE_FAILURE:
               {
                  uiLen = 0;
                  cm_je_copy_errorinfo_to_buffer (pUCMTempRespBuf, cm_resp_msg_p, &uiLen);
                  pUCMTempRespBuf += uiLen;
               }
               break;
         }
         break;
      case CM_CMD_GET_FIRST_N_LOGS:
      case CM_CMD_GET_NEXT_N_LOGS:
      case CM_CMD_GET_FIRST_N_RECS:
      case CM_CMD_GET_NEXT_N_RECS:
      case CM_CMD_GET_EXACT_REC:

#ifdef CM_STATS_COLLECTOR_SUPPORT
      case CM_CMD_GET_AGGREGATE_STATS:
      case CM_CMD_GET_AVERAGE_STATS:
      case CM_CMD_GET_PER_DEVICE_STATS:
#endif
         switch (cm_resp_msg_p->result_code)
         {
            case CM_JE_SUCCESS:
            case CM_JE_NO_MORE_RECS:
            case CM_JE_MORE_RECS:
               /* Record Count */
               pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf,
                     cm_resp_msg_p->data.sec_appl.count_ui);

               for (uiCnt = 0; uiCnt < cm_resp_msg_p->data.sec_appl.count_ui; uiCnt++)
               {
                  /* Pair Count */
                  pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf,
                        cm_resp_msg_p->data.sec_appl.
                        nv_pair_array[uiCnt].count_ui);

                  /* Copy  Name Value Pair */
                  for (uiNvCnt = 0, uiLen = 0;
                        uiNvCnt <
                        cm_resp_msg_p->data.sec_appl.nv_pair_array[uiCnt].count_ui;
                        uiNvCnt++, pUCMTempRespBuf += uiLen, uiLen = 0)
                  {
                     if (cm_je_copy_nvpair_to_buffer
                           (&
                            (cm_resp_msg_p->data.sec_appl.
                             nv_pair_array[uiCnt].nv_pairs[uiNvCnt]), pUCMTempRespBuf,
                            &uiLen) != OF_SUCCESS)
                     {
                        CM_JE_DEBUG_PRINT ("Copy Name Value Pairs into Buffer Failed");
                        return OF_FAILURE;
                     }
                  }
               }
               break;
            case CM_JE_FAILURE:
               CM_JE_DEBUG_PRINT ("JE Failure");
               uiLen = 0;
               cm_je_copy_errorinfo_to_buffer (pUCMTempRespBuf, cm_resp_msg_p, &uiLen);
               pUCMTempRespBuf += uiLen;
               break;
         }
         break;

      case CM_CMD_GET_DM_NODE_INFO:
      case CM_CMD_GET_DM_KEY_CHILD_INFO:
      case CM_CMD_GET_DM_FIRST_NODE_INFO:
      case CM_CMD_GET_DM_NEXT_NODE_INFO:
      case CM_CMD_GET_DM_NEXT_TABLE_ANCHOR_NODE_INFO:
         switch (cm_resp_msg_p->result_code)
         {
            case CM_JE_SUCCESS:
               cm_je_copy_dminfo_to_buff (cm_resp_msg_p->data.dm_info.node_info_p,
                     cm_resp_msg_p->data.dm_info.count_ui,
                     &pUCMTempRespBuf);

               /* Copy Opaque Info to Buffer for GetFirst & GetNext NodeInfo */
               if (cm_resp_msg_p->data.dm_info.opaque_info_length)
               {
                  cm_je_copy_opaqueinfo_to_message (cm_resp_msg_p->data.dm_info.opaque_info_p,
                        cm_resp_msg_p->data.dm_info.opaque_info_length,
                        &pUCMTempRespBuf);
               }
               break;
            case CM_JE_FAILURE:
               uiLen = 0;
               cm_je_copy_errorinfo_to_buffer (pUCMTempRespBuf, cm_resp_msg_p, &uiLen);
               pUCMTempRespBuf += uiLen;

               break;
         }
         break;

      case CM_CMD_GET_DM_NODE_AND_CHILD_INFO:
      case CM_CMD_GET_DM_KEY_CHILDS:
         switch (cm_resp_msg_p->result_code)
         {
            case CM_JE_SUCCESS:
               /* Copy Parent-NodeInfo */
               cm_je_copy_dminfo_to_buff (cm_resp_msg_p->data.dm_info.node_info_p,
                     cm_resp_msg_p->data.dm_info.count_ui,
                     &pUCMTempRespBuf);

               /* Copy Child-NodeInfo */
               cm_je_copy_dminfo_to_buff (cm_resp_msg_p->data.dm_info.node_info_array_p->
                     struct_arr_p,
                     cm_resp_msg_p->data.dm_info.node_info_array_p->
                     count_ui, &pUCMTempRespBuf);
               break;

            case CM_JE_FAILURE:
               uiLen = 0;
               cm_je_copy_errorinfo_to_buffer (pUCMTempRespBuf, cm_resp_msg_p, &uiLen);
               pUCMTempRespBuf += uiLen;
               break;
         }
         break;

      case CM_CMD_GET_DM_KEYS_ARRAY:
         switch (cm_resp_msg_p->result_code)
         {
            case CM_JE_SUCCESS:
               /* Record Count */
               pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf,
                     cm_resp_msg_p->data.
                     dm_info.dm_node_nv_pairs_p->count_ui);

               for (uiCnt = 0, uiLen = 0; uiCnt <
                     cm_resp_msg_p->data.dm_info.dm_node_nv_pairs_p->count_ui;
                     uiCnt++, pUCMTempRespBuf += uiLen, uiLen = 0)
               {
                  if (cm_je_copy_nvpair_to_buffer
                        (&
                         (cm_resp_msg_p->data.dm_info.dm_node_nv_pairs_p->nv_pairs[uiCnt]),
                         pUCMTempRespBuf, &uiLen) != OF_SUCCESS)
                  {
                     CM_JE_DEBUG_PRINT ("Copy Name Value Pairs into Buffer Failed");
                     return OF_FAILURE;
                  }
               }
               break;

            case CM_JE_FAILURE:
               uiLen = 0;
               cm_je_copy_errorinfo_to_buffer (pUCMTempRespBuf, cm_resp_msg_p, &uiLen);
               pUCMTempRespBuf += uiLen;

               break;
         }
         break;

      case CM_CMD_GET_DM_INSTANCE_MAPLIST_BY_NAMEPATH:
         switch (cm_resp_msg_p->result_code)
         {
            case CM_JE_SUCCESS:
               /* Record Count */
               pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf,
                     cm_resp_msg_p->data.dm_info.count_ui);

               for (ii = 0; ii < cm_resp_msg_p->data.dm_info.count_ui; ii++)
               {
                  /* Copy instance map entries */
                  pInstanceMap = &cm_resp_msg_p->data.dm_info.instance_map_list_p[ii];
                  pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf,
                        pInstanceMap->instance_id);
                  *pUCMTempRespBuf = pInstanceMap->key_type;
                  pUCMTempRespBuf++;
                  pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf,
                        pInstanceMap->key_length);
                  of_memcpy (pUCMTempRespBuf, pInstanceMap->key_p,
                        pInstanceMap->key_length);
                  pUCMTempRespBuf = pUCMTempRespBuf + pInstanceMap->key_length;

               }
               break;

            case CM_JE_FAILURE:
               uiLen = 0;
               cm_je_copy_errorinfo_to_buffer (pUCMTempRespBuf, cm_resp_msg_p, &uiLen);
               pUCMTempRespBuf += uiLen;
         }
         break;
      case CM_CMD_IS_DM_INSTANCE_USING_NAMEPATH_AND_KEY:
      case CM_CMD_IS_DM_CHILD_NODE:
         /* Record Count */
         of_mbuf_put_32 (pUCMTempRespBuf,
               pResponseMsg->result.success.reserved);
         switch (cm_resp_msg_p->result_code == CM_JE_FAILURE)
         {
            uiLen = 0;
            cm_je_copy_errorinfo_to_buffer (pUCMTempRespBuf, cm_resp_msg_p, &uiLen);
            pUCMTempRespBuf += uiLen;
         }
         break;
      case CM_CMD_GET_SAVE_FILE_NAME:
      case CM_CMD_GET_SAVE_DIRECTORY_NAME:
         switch (cm_resp_msg_p->result_code)
         {
            case CM_JE_SUCCESS:
               pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf,
                     cm_resp_msg_p->data.dm_info.opaque_info_length);
               of_memcpy(pUCMTempRespBuf,
                     cm_resp_msg_p->data.dm_info.opaque_info_p,
                     cm_resp_msg_p->data.dm_info.opaque_info_length);
               pUCMTempRespBuf += cm_resp_msg_p->data.dm_info.opaque_info_length;
               break;
            case CM_JE_FAILURE:
               uiLen = 0;
               cm_je_copy_errorinfo_to_buffer (pUCMTempRespBuf, cm_resp_msg_p, &uiLen);
               pUCMTempRespBuf += uiLen;
               break;
         }
         break;
      case CM_CMD_GET_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_GET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
         switch (cm_resp_msg_p->result_code )
         {
            case CM_JE_SUCCESS:
               /* Send Role and Permission structure*/
               of_memcpy(pUCMTempRespBuf, cm_resp_msg_p->data.dm_info.opaque_info_p,
                     sizeof(struct cm_dm_role_permission));
               pUCMTempRespBuf += sizeof(struct cm_dm_role_permission);
               break;
            case CM_JE_FAILURE:
               uiLen = 0;
               cm_je_copy_errorinfo_to_buffer (pUCMTempRespBuf, cm_resp_msg_p, &uiLen);
               pUCMTempRespBuf += uiLen;
               break;
         }
         break;
      case CM_CMD_GET_DM_NODE_ROLE_ATTRIBS:
      case CM_CMD_GET_DM_INSTANCE_ROLE_ATTRIBS:
         switch (cm_resp_msg_p->result_code )
         {
            case CM_JE_SUCCESS:
               /* Send multiple Role and Permission structure*/
               role_perm_ary_p=( struct cm_dm_array_of_role_permissions *)cm_resp_msg_p->data.dm_info.opaque_info_p;
               pUCMTempRespBuf=(char *)of_mbuf_put_32(pUCMTempRespBuf, role_perm_ary_p->count_ui);
               if(role_perm_ary_p->count_ui)
               {
                  for(ii=0; ii<role_perm_ary_p->count_ui;ii++)
                  {
                     of_memcpy(pUCMTempRespBuf,role_perm_ary_p->role_permissions[ii].role,UCMDM_ADMIN_ROLE_MAX_LEN + 1);
                     pUCMTempRespBuf += UCMDM_ADMIN_ROLE_MAX_LEN + 1;
                     pUCMTempRespBuf = of_mbuf_put_32(pUCMTempRespBuf, role_perm_ary_p->role_permissions[ii].permissions);
                  }
               }
               break;
            case CM_JE_FAILURE:
               uiLen = 0;
               cm_je_copy_errorinfo_to_buffer (pUCMTempRespBuf, cm_resp_msg_p, &uiLen);
               pUCMTempRespBuf += uiLen;
               break;
         }
         break;
      case CM_CMD_SET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_DEL_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_SET_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_DEL_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
         switch (cm_resp_msg_p->result_code )
         {
            case CM_JE_FAILURE:
               uiLen = 0;
               cm_je_copy_errorinfo_to_buffer (pUCMTempRespBuf, cm_resp_msg_p, &uiLen);
               pUCMTempRespBuf += uiLen;
               break;
         }
         break;
      case CM_CMD_GET_NEW_DM_INSTANCE_ID:
      case CM_CMD_ADD_DM_INSTANCE_MAP:
      case CM_CMD_DEL_DM_INSTANCE_MAP:
      case CM_CMD_GET_DM_CHILD_NODE_COUNT:
         // copy into    cm_resp_msg_p->data.data_modelGet.count_ui:
         break;
#ifdef CM_STATS_COLLECTOR_SUPPORT
      case CM_CMD_COALESCAR_RESP:
         of_memcpy(pUCMTempRespBuf,cm_resp_msg_p->data.stats_data.nv_pair_flat_buf_p,
               cm_resp_msg_p->data.stats_data.nv_pair_flat_buf_len_ui);
#endif
   }
   /* Copy Response Lengh in Response Header */

   CM_JE_DEBUG_PRINT ("Response Preperation Succesfull");
   return OF_SUCCESS;
}

void cm_je_get_elementattrib_length(struct cm_dm_node_info * node_info_p,uint32_t *uiEleAttrLen)
{
   uint32_t ulEleAttribLen = 0;

   ulEleAttribLen += 4 * sizeof(char); /* size of variables from ucELementType to pad = 32bits */
   ulEleAttribLen += 4 * sizeof(char); /* size of variables from save_in_parent_instance_dir_b to pad = 4 bytes */

   //  ulEleAttribLen += 3 * sizeof(int32_t); /* length of commandname,identifier,help */

   ulEleAttribLen += sizeof(int32_t);
   if(node_info_p->element_attrib.command_name_p)
   {
      ulEleAttribLen += of_strlen (node_info_p->element_attrib.command_name_p);
   }
   ulEleAttribLen += sizeof(int32_t);
   if(node_info_p->element_attrib.cli_identifier_p)
   {
      ulEleAttribLen += of_strlen (node_info_p->element_attrib.cli_identifier_p);
   }
   ulEleAttribLen += sizeof(int32_t);
   if(node_info_p->element_attrib.cli_idenetifier_help_p)
   {
      ulEleAttribLen += of_strlen (node_info_p->element_attrib.cli_idenetifier_help_p);
   }
   //CM_JE_DEBUG_PRINT("Element Attrib Lenth =%d",ulEleAttribLen);
   *uiEleAttrLen =  ulEleAttribLen;

}

void cm_je_get_dataattrib_length(struct cm_dm_node_info * node_info_p,uint32_t *uiDataAttrLen)
{
   uint32_t ulDataAttrbLen = 0;
   uint32_t uiStrEnumCnt = 0;

   ulDataAttrbLen += (2*sizeof(unsigned char));  /*Data type + attrib type */

   switch(node_info_p->data_attrib.data_type)
   {
      case CM_DATA_TYPE_INT:
         {
            if(node_info_p->data_attrib.attrib_type == CM_DATA_ATTRIB_INT_RANGE)
               ulDataAttrbLen += sizeof(node_info_p->data_attrib.attr.int_range);
            else if(node_info_p->data_attrib.attrib_type == CM_DATA_ATTRIB_INT_ENUM)
               ulDataAttrbLen += sizeof(node_info_p->data_attrib.attr.int_enum);
         }
         break;

      case CM_DATA_TYPE_UINT:
         {
            if(node_info_p->data_attrib.attrib_type == CM_DATA_ATTRIB_UINT_RANGE)
               ulDataAttrbLen += sizeof(node_info_p->data_attrib.attr.uint_range);
            else if(node_info_p->data_attrib.attrib_type == CM_DATA_ATTRIB_UINT_ENUM)
               ulDataAttrbLen += sizeof(node_info_p->data_attrib.attr.uint_enum);
         }
         break;

    case CM_DATA_TYPE_INT64:
      {
        if(node_info_p->data_attrib.attrib_type == CM_DATA_ATTRIB_INT64_RANGE)
          ulDataAttrbLen += sizeof(node_info_p->data_attrib.attr.longint_range);
        else if(node_info_p->data_attrib.attrib_type == CM_DATA_ATTRIB_INT64_ENUM)
          ulDataAttrbLen += sizeof(node_info_p->data_attrib.attr.longint_enum);
      }
      break;

    case CM_DATA_TYPE_UINT64:
      {
        if(node_info_p->data_attrib.attrib_type == CM_DATA_ATTRIB_UINT64_RANGE)
          ulDataAttrbLen += sizeof(node_info_p->data_attrib.attr.ulongint_range);
        else if(node_info_p->data_attrib.attrib_type == CM_DATA_ATTRIB_UINT64_ENUM)
          ulDataAttrbLen += sizeof(node_info_p->data_attrib.attr.ulongint_enum);
      }
      break;

      case CM_DATA_TYPE_STRING:
      case CM_DATA_TYPE_STRING_SPECIAL_CHARS:
         {
            if(node_info_p->data_attrib.attrib_type == CM_DATA_ATTRIB_STR_RANGE)
               ulDataAttrbLen += sizeof(node_info_p->data_attrib.attr.string_range);
            else if(node_info_p->data_attrib.attrib_type == CM_DATA_ATTRIB_STR_ENUM)
            {
               ulDataAttrbLen += sizeof(node_info_p->data_attrib.attr.string_enum.count_ui);
               for(uiStrEnumCnt = 0; uiStrEnumCnt < node_info_p->data_attrib.attr.string_enum.count_ui;uiStrEnumCnt++)
               {
                  if(node_info_p->data_attrib.attr.string_enum.array[uiStrEnumCnt])
                  {
                     ulDataAttrbLen += sizeof(uint32_t);
                     ulDataAttrbLen += of_strlen (node_info_p->data_attrib.attr.string_enum.array[uiStrEnumCnt]);
                  }
                  if(node_info_p->data_attrib.attr.string_enum.aFrdArr[uiStrEnumCnt])
                  {
                     ulDataAttrbLen += sizeof(uint32_t);
                     ulDataAttrbLen += of_strlen (node_info_p->data_attrib.attr.string_enum.aFrdArr[uiStrEnumCnt]);
                  }
               }
            }
         }
         break;

      case CM_DATA_TYPE_IPADDR:
         {
            if(node_info_p->data_attrib.attrib_type == CM_DATA_ATTRIB_IPADDR_RANGE)
               ulDataAttrbLen += sizeof(node_info_p->data_attrib.attr.ip_range);
            else if(node_info_p->data_attrib.attrib_type == CM_DATA_ATTRIB_IPADDR_ENUM)
            {
               ulDataAttrbLen += sizeof(node_info_p->data_attrib.attr.ip_enum.count_ui);
               for(uiStrEnumCnt = 0; uiStrEnumCnt < node_info_p->data_attrib.attr.ip_enum.count_ui;uiStrEnumCnt++)
               {
                  ulDataAttrbLen += sizeof(uint32_t);
                  ulDataAttrbLen += of_strlen (node_info_p->data_attrib.attr.ip_enum.ip_arr_a[uiStrEnumCnt]);
               } 
            }
         }
         break;

      case CM_DATA_TYPE_IPV6ADDR:
         {
            if(node_info_p->data_attrib.attrib_type == CM_DATA_ATTRIB_IPV6ADDR_RANGE)
               ulDataAttrbLen += sizeof(node_info_p->data_attrib.attr.ipv6_range);
            else if(node_info_p->data_attrib.attrib_type == CM_DATA_ATTRIB_IPV6ADDR_ENUM)
            {
               ulDataAttrbLen += sizeof(node_info_p->data_attrib.attr.ipv6_enum.count_ui);
               for(uiStrEnumCnt = 0; uiStrEnumCnt < node_info_p->data_attrib.attr.ipv6_enum.count_ui;uiStrEnumCnt++)
               {
                  ulDataAttrbLen += sizeof(uint32_t);
                  ulDataAttrbLen += of_strlen (node_info_p->data_attrib.attr.ipv6_enum.ip_v6_arr_a[uiStrEnumCnt]);
               }
            }
         }
         break;

      default:
         break;
   }  

   switch(node_info_p->data_attrib.data_type)
   {
      case CM_DATA_TYPE_UNKNOWN:
         ulDataAttrbLen += sizeof(node_info_p->data_attrib.attr.max_no_of_tables);
         break;
      case CM_DATA_TYPE_INT:
         ulDataAttrbLen += sizeof(node_info_p->data_attrib.default_value.idefault_value);
         break;
      case CM_DATA_TYPE_UINT:
         ulDataAttrbLen += sizeof(node_info_p->data_attrib.default_value.uidefault_value);
      break;
    case CM_DATA_TYPE_INT64:
      ulDataAttrbLen += sizeof(node_info_p->data_attrib.default_value.ldefault_value);
      break;
    case CM_DATA_TYPE_UINT64:
      ulDataAttrbLen += sizeof(node_info_p->data_attrib.default_value.uldefault_value);
         break;
      case CM_DATA_TYPE_STRING:
      case CM_DATA_TYPE_STRING_SPECIAL_CHARS:
         {
            ulDataAttrbLen += sizeof(uint32_t);
            ulDataAttrbLen += of_strlen(node_info_p->data_attrib.default_value.default_string);
         }
         break;
   }
   //CM_JE_DEBUG_PRINT("Data Attrib Lenth =%d",ulDataAttrbLen);
   *uiDataAttrLen = ulDataAttrbLen; 

}



/******************************************************************************
 * Function Name :
 * Description   : 
 * Input params  :
 * Output params : 
 * Return value  : 
 *******************************************************************************/
 void cm_je_copy_dminfo_to_buff (struct cm_dm_node_info * node_info_p,
      uint32_t uiNodeCnt,
      char ** buffer_pp)
{
   char  *pUCMTempRespBuf;
   uint32_t name_length, uiDescLen, uiDMPathLen, ii,jj, uiFrdNameLen = 0;

   uint32_t uiCmdNameLen = 0, uiStrEnumCnt = 0,
            uiStrEnumLen = 0,uiFrdEnumLen = 0;

   uint32_t uiStrArrLen[CM_MAX_STRENUM_COUNT];
   uint32_t uiFrdArrLen[CM_MAX_STRENUM_COUNT];
   uint32_t uiIPArrayLen[5];

   uint32_t uidata_attribLen=0;
   uint32_t uielement_attribLen=0;
   uint8_t TmpElementType = 0;

   pUCMTempRespBuf = *buffer_pp;


   /* Copy Node-count */
   pUCMTempRespBuf =(char *)of_mbuf_put_32 (pUCMTempRespBuf, uiNodeCnt);

   /* Copy NodeInfo Structure(s) */
   for (ii = 0; ii < uiNodeCnt; ii++)
   {
      uiStrEnumLen = 0;
      uiFrdEnumLen = 0;
      name_length = 0;
      uiFrdNameLen = 0;
      uiDescLen = 0;
      uiDMPathLen = 0;

      cm_je_get_elementattrib_length(&node_info_p[ii],&uielement_attribLen);
      cm_je_get_dataattrib_length(&node_info_p[ii],&uidata_attribLen);

      if(node_info_p[ii].name_p!=NULL)
         name_length = of_strlen (node_info_p[ii].name_p);
      else
         name_length = 0;
      if(node_info_p[ii].friendly_name_p!=NULL)
         uiFrdNameLen = of_strlen(node_info_p[ii].friendly_name_p);
      else
         uiFrdNameLen = 0;
      if(node_info_p[ii].description_p != NULL)
         uiDescLen = of_strlen (node_info_p[ii].description_p);
      else
         uiDescLen = 0;
      if(node_info_p[ii].dm_path_p != NULL)
         uiDMPathLen = of_strlen (node_info_p[ii].dm_path_p);
      else
         uiDMPathLen = 0;

      if( (node_info_p[ii].data_attrib.data_type == CM_DATA_TYPE_STRING || node_info_p[ii].data_attrib.data_type == CM_DATA_TYPE_STRING_SPECIAL_CHARS )
            && node_info_p[ii].data_attrib.attrib_type == CM_DATA_ATTRIB_STR_ENUM)
      {
         for(uiStrEnumCnt = 0;uiStrEnumCnt < node_info_p[ii].data_attrib.attr.string_enum.count_ui;uiStrEnumCnt++)
         {
            uiStrArrLen[uiStrEnumCnt] = of_strlen(node_info_p[ii].data_attrib.attr.string_enum.array[uiStrEnumCnt]);
            uiFrdArrLen[uiStrEnumCnt] = of_strlen(node_info_p[ii].data_attrib.attr.string_enum.aFrdArr[uiStrEnumCnt]);
         }
      }

      if(node_info_p[ii].data_attrib.data_type == CM_DATA_TYPE_IPADDR && node_info_p[ii].data_attrib.attrib_type == CM_DATA_ATTRIB_IPADDR_ENUM)
      {
         for(uiStrEnumCnt = 0;uiStrEnumCnt < node_info_p[ii].data_attrib.attr.ip_enum.count_ui;uiStrEnumCnt++)
         {
            uiIPArrayLen[uiStrEnumCnt] = of_strlen(node_info_p[ii].data_attrib.attr.ip_enum.ip_arr_a[uiStrEnumCnt]);
         }
      }

      /*
       ** Copy values of NodeInfo structure..
       */

      pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,name_length); 
      if(node_info_p[ii].name_p!=NULL)
      {
         of_memcpy (pUCMTempRespBuf, node_info_p[ii].name_p, name_length);
         pUCMTempRespBuf += name_length;
      }

      pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,uiFrdNameLen); 
      if(node_info_p[ii].friendly_name_p!=NULL)
      {
         of_memcpy (pUCMTempRespBuf,node_info_p[ii].friendly_name_p,uiFrdNameLen);
         pUCMTempRespBuf += uiFrdNameLen;
      }
      pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,uiDescLen); 
      if(node_info_p[ii].description_p !=NULL)
      {
         of_memcpy (pUCMTempRespBuf, node_info_p[ii].description_p, uiDescLen);
         pUCMTempRespBuf += uiDescLen;
      }
      pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,uiDMPathLen); 
      if(node_info_p[ii].dm_path_p!=NULL)
      {

         of_memcpy (pUCMTempRespBuf,node_info_p[ii].dm_path_p, uiDMPathLen);
         pUCMTempRespBuf += uiDMPathLen;
      }
      pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,node_info_p[ii].id_ui); 

      pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,of_strlen(node_info_p[ii].file_name));
      of_memcpy (pUCMTempRespBuf,node_info_p[ii].file_name,of_strlen(node_info_p[ii].file_name));
      pUCMTempRespBuf += of_strlen(node_info_p[ii].file_name);

      *pUCMTempRespBuf = node_info_p[ii].save_b;
      pUCMTempRespBuf += sizeof(unsigned char);
      /*
       ** Start copying values of Element Attrib Structure..
       */

      TmpElementType = (node_info_p[ii].element_attrib.element_type & 0x7) |
         ((node_info_p[ii].element_attrib.visible_b & 0x1)<<3) |
         ((node_info_p[ii].element_attrib.key_b & 0x1)<<4)    |
         (( node_info_p[ii].element_attrib.mandatory_b & 0x1) <<5) |
         ((node_info_p[ii].element_attrib.ordered_table_b & 0x1) <<6) |
         ((node_info_p[ii].element_attrib.table_priority_b & 0x1) <<7);

      *pUCMTempRespBuf =  TmpElementType; 
      pUCMTempRespBuf ++;


      TmpElementType = 0;
      TmpElementType  = (node_info_p[ii].element_attrib.notify_change_b & 0x1)  |
         ((node_info_p[ii].element_attrib.notify_attrib & 0x7) << 1) |
         ((node_info_p[ii].element_attrib.reboot_on_change_b & 0x1) <<4) | 
         ((node_info_p[ii].element_attrib.sticky_to_parent_b & 0x1) << 5) |
         ((node_info_p[ii].element_attrib.access_controle_b & 0x3) << 6);

      *pUCMTempRespBuf = TmpElementType;
      pUCMTempRespBuf ++;

      TmpElementType = 0;
      TmpElementType  = (node_info_p[ii].element_attrib.scalar_group_b & 0x1) |
         ((node_info_p[ii].element_attrib.global_trans_b & 0x1) << 1) | 
         ((node_info_p[ii].element_attrib.parent_trans_b & 0x1) << 2)|
         ((node_info_p[ii].element_attrib.system_command_b & 0x1) << 3)| 
         ((node_info_p[ii].element_attrib.stats_command_b & 0x1) << 4)|
         ((node_info_p[ii].element_attrib.device_specific_b & 0x1) << 5); 

      //CM_JE_DEBUG_PRINT("Element Type :%d",TmpElementType);
      *pUCMTempRespBuf =TmpElementType;
      pUCMTempRespBuf ++;

      *pUCMTempRespBuf = node_info_p[ii].element_attrib.save_in_parent_instance_dir_b;
      pUCMTempRespBuf ++;

      *pUCMTempRespBuf = node_info_p[ii].element_attrib.non_config_leaf_node_b;
      pUCMTempRespBuf ++;

      if(node_info_p[ii].element_attrib.command_name_p !=NULL)
         uiCmdNameLen = of_strlen(node_info_p[ii].element_attrib.command_name_p); 
      else 
         uiCmdNameLen = 0;
      pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,uiCmdNameLen); 
      if(node_info_p[ii].element_attrib.command_name_p!=NULL)
      {
         of_memcpy (pUCMTempRespBuf, node_info_p[ii].element_attrib.command_name_p, uiCmdNameLen);
         pUCMTempRespBuf += uiCmdNameLen;
      }

      uiCmdNameLen = 0; 
      if(node_info_p[ii].element_attrib.cli_identifier_p !=NULL)
         uiCmdNameLen = of_strlen(node_info_p[ii].element_attrib.cli_identifier_p); 
      else
         uiCmdNameLen = 0; 

      pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,uiCmdNameLen); 
      if(node_info_p[ii].element_attrib.cli_identifier_p!=NULL)
      {
         of_memcpy (pUCMTempRespBuf, node_info_p[ii].element_attrib.cli_identifier_p, uiCmdNameLen);
         pUCMTempRespBuf += uiCmdNameLen;
      }

      uiCmdNameLen = 0; 
      if(node_info_p[ii].element_attrib.cli_idenetifier_help_p !=NULL)
         uiCmdNameLen = of_strlen(node_info_p[ii].element_attrib.cli_idenetifier_help_p); 
      else
         uiCmdNameLen = 0; 

      pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,uiCmdNameLen); 
      if(node_info_p[ii].element_attrib.cli_idenetifier_help_p!=NULL)
      {
         of_memcpy (pUCMTempRespBuf, node_info_p[ii].element_attrib.cli_idenetifier_help_p, uiCmdNameLen);
         pUCMTempRespBuf += uiCmdNameLen;
      }
      /*
       ** Start copying values of Data Attrib Structure..
       */

      TmpElementType = 0;
      TmpElementType = node_info_p[ii].data_attrib.data_type;
      *pUCMTempRespBuf = TmpElementType;
      pUCMTempRespBuf ++;
     
      TmpElementType = 0;
      TmpElementType = node_info_p[ii].data_attrib.attrib_type;

      //CM_JE_DEBUG_PRINT("Attrib Type :%d",TmpElementType);
      *pUCMTempRespBuf = TmpElementType;
      pUCMTempRespBuf ++;

      switch(node_info_p[ii].data_attrib.data_type) 
      {
         case CM_DATA_TYPE_INT:
            {
               if(node_info_p[ii].data_attrib.attrib_type == CM_DATA_ATTRIB_INT_RANGE)
               { 
                  pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.int_range.start_value);
                  pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.int_range.end_value);
               }
               else if(node_info_p[ii].data_attrib.attrib_type == CM_DATA_ATTRIB_INT_ENUM)
               {
                  pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.int_enum.count_ui);
                  for(jj = 0;jj<node_info_p[ii].data_attrib.attr.int_enum.count_ui;jj++)
                  {
                     pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.int_enum.array[jj]);
                    // pUCMTempRespBuf += uiOSIntSize_g;
                  }
               }
            }
            break;

         case CM_DATA_TYPE_UINT:
            {
               if(node_info_p[ii].data_attrib.attrib_type == CM_DATA_ATTRIB_UINT_RANGE)
               {
                  pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.uint_range.start_value);
                  pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.uint_range.end_value);
               }
               else if(node_info_p[ii].data_attrib.attrib_type ==CM_DATA_ATTRIB_UINT_ENUM)
               {
                  pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.uint_enum.count_ui);
                  for(jj=0;jj<node_info_p[ii].data_attrib.attr.uint_enum.count_ui;jj++)
                  {
                     pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.uint_enum.array[jj]);
                   //  pUCMTempRespBuf += uiOSIntSize_g;
                  }

               }
            }
            break;

      case CM_DATA_TYPE_INT64:
        {
          if(node_info_p[ii].data_attrib.attrib_type == CM_DATA_ATTRIB_INT64_RANGE)
          { 
            pUCMTempRespBuf = (char*)of_mbuf_put_64(pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.longint_range.start_value);
            pUCMTempRespBuf = (char*)of_mbuf_put_64(pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.longint_range.end_value);
          }
          else if(node_info_p[ii].data_attrib.attrib_type == CM_DATA_ATTRIB_INT64_ENUM)
          {
            pUCMTempRespBuf = (char*)of_mbuf_put_64(pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.longint_enum.count_ui);
            for(jj = 0;jj<node_info_p[ii].data_attrib.attr.longint_enum.count_ui;jj++)
            {
              pUCMTempRespBuf = (char*)of_mbuf_put_64(pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.longint_enum.array[jj]);
              // pUCMTempRespBuf += uiOSIntSize_g;
            }
          }
        }
        break;

      case CM_DATA_TYPE_UINT64:
        {
          if(node_info_p[ii].data_attrib.attrib_type == CM_DATA_ATTRIB_UINT64_RANGE)
          {
            pUCMTempRespBuf = (char*)of_mbuf_put_64(pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.ulongint_range.start_value);
            pUCMTempRespBuf = (char*)of_mbuf_put_64(pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.ulongint_range.end_value);
          }
          else if(node_info_p[ii].data_attrib.attrib_type ==CM_DATA_ATTRIB_UINT64_ENUM)
          {
            pUCMTempRespBuf = (char*)of_mbuf_put_64(pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.ulongint_enum.count_ui);
            for(jj=0;jj<node_info_p[ii].data_attrib.attr.ulongint_enum.count_ui;jj++)
            {
              pUCMTempRespBuf = (char*)of_mbuf_put_64(pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.ulongint_enum.array[jj]);
              //  pUCMTempRespBuf += uiOSIntSize_g;
            }

          }
        }
        break;

         case CM_DATA_TYPE_STRING:
         case CM_DATA_TYPE_STRING_SPECIAL_CHARS:
            {
               if(node_info_p[ii].data_attrib.attrib_type == CM_DATA_ATTRIB_STR_RANGE)
               {
                  pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.string_range.min_length);
                  pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.string_range.max_length);
               }
               else if(node_info_p[ii].data_attrib.attrib_type == CM_DATA_ATTRIB_STR_ENUM)
               {
                  pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.string_enum.count_ui);
                  /* Place length,values in buffer */
                  for(uiStrEnumCnt = 0;uiStrEnumCnt < node_info_p[ii].data_attrib.attr.string_enum.count_ui;uiStrEnumCnt++)
                  {
                     pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,uiStrArrLen[uiStrEnumCnt]);
                     of_memcpy(pUCMTempRespBuf , node_info_p[ii].data_attrib.attr.string_enum.array[uiStrEnumCnt] ,uiStrArrLen[uiStrEnumCnt]);
                     pUCMTempRespBuf += uiStrArrLen[uiStrEnumCnt];

                     pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,uiFrdArrLen[uiStrEnumCnt]);
                     of_memcpy(pUCMTempRespBuf , node_info_p[ii].data_attrib.attr.string_enum.aFrdArr[uiStrEnumCnt] ,uiFrdArrLen[uiStrEnumCnt]);
                     pUCMTempRespBuf += uiFrdArrLen[uiStrEnumCnt];
                  } 

               }
            }
            break;

         case CM_DATA_TYPE_IPADDR:
            {
               if(node_info_p[ii].data_attrib.attrib_type == CM_DATA_ATTRIB_IPADDR_RANGE)
               {
                  pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,of_strlen(node_info_p[ii].data_attrib.attr.ip_range.start_ip));
                  of_memcpy (pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.ip_range.start_ip,of_strlen(node_info_p[ii].data_attrib.attr.ip_range.start_ip));
                  pUCMTempRespBuf += of_strlen(node_info_p[ii].data_attrib.attr.ip_range.start_ip);

                  pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,of_strlen(node_info_p[ii].data_attrib.attr.ip_range.end_ip));
                  of_memcpy (pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.ip_range.end_ip,of_strlen(node_info_p[ii].data_attrib.attr.ip_range.end_ip));
                  pUCMTempRespBuf += of_strlen(node_info_p[ii].data_attrib.attr.ip_range.end_ip);
               }  
               else if(node_info_p[ii].data_attrib.attrib_type ==CM_DATA_ATTRIB_IPADDR_ENUM)
               {
                  pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.ip_enum.count_ui);  

                  for(uiStrEnumCnt = 0;uiStrEnumCnt < node_info_p[ii].data_attrib.attr.ip_enum.count_ui;uiStrEnumCnt++)
                  {
                     pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,uiIPArrayLen[uiStrEnumCnt]);
                     of_memcpy(pUCMTempRespBuf , node_info_p[ii].data_attrib.attr.ip_enum.ip_arr_a[uiStrEnumCnt] ,uiIPArrayLen[uiStrEnumCnt]);
                     pUCMTempRespBuf += uiIPArrayLen[uiStrEnumCnt];
                  }  

               }
            }
            break;

         case CM_DATA_TYPE_IPV6ADDR:
            {
               if(node_info_p[ii].data_attrib.attrib_type ==CM_DATA_ATTRIB_IPV6ADDR_RANGE)
               {
                  pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,of_strlen(node_info_p[ii].data_attrib.attr.ipv6_range.start_ip));
                  of_memcpy (pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.ipv6_range.start_ip,of_strlen(node_info_p[ii].data_attrib.attr.ipv6_range.start_ip));
                  pUCMTempRespBuf += of_strlen(node_info_p[ii].data_attrib.attr.ipv6_range.start_ip);

                  pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,of_strlen(node_info_p[ii].data_attrib.attr.ipv6_range.end_ip));
                  of_memcpy (pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.ipv6_range.end_ip,of_strlen(node_info_p[ii].data_attrib.attr.ipv6_range.end_ip));
                  pUCMTempRespBuf += of_strlen(node_info_p[ii].data_attrib.attr.ipv6_range.end_ip);
               }
               else if(node_info_p[ii].data_attrib.attrib_type == CM_DATA_ATTRIB_IPV6ADDR_ENUM)
               {
                  of_memset(uiIPArrayLen,0,sizeof(uiIPArrayLen));
                  pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.ipv6_enum.count_ui); 
                  for(uiStrEnumCnt = 0;uiStrEnumCnt < node_info_p[ii].data_attrib.attr.ipv6_enum.count_ui;uiStrEnumCnt++)
                  {
                     pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,uiIPArrayLen[uiStrEnumCnt]);
                     of_memcpy(pUCMTempRespBuf , node_info_p[ii].data_attrib.attr.ipv6_enum.ip_v6_arr_a[uiStrEnumCnt] ,uiIPArrayLen[uiStrEnumCnt]);
                     pUCMTempRespBuf += uiIPArrayLen[uiStrEnumCnt];
                  }
               }
            }
            break;

      }


      switch(node_info_p[ii].data_attrib.data_type)
      {      
         case CM_DATA_TYPE_UNKNOWN:
            pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,node_info_p[ii].data_attrib.attr.max_no_of_tables);
            break;
         case CM_DATA_TYPE_INT:
            pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,node_info_p[ii].data_attrib.default_value.idefault_value); 
            break;
         case CM_DATA_TYPE_UINT:
            pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,node_info_p[ii].data_attrib.default_value.uidefault_value);
        break;
      case CM_DATA_TYPE_INT64:
        pUCMTempRespBuf = (char*)of_mbuf_put_64(pUCMTempRespBuf,node_info_p[ii].data_attrib.default_value.ldefault_value); 
        break;
      case CM_DATA_TYPE_UINT64:
        pUCMTempRespBuf = (char*)of_mbuf_put_64(pUCMTempRespBuf,node_info_p[ii].data_attrib.default_value.uldefault_value);
            break;
         case CM_DATA_TYPE_STRING:
         case CM_DATA_TYPE_STRING_SPECIAL_CHARS:
            {
               pUCMTempRespBuf = (char*)of_mbuf_put_32(pUCMTempRespBuf,of_strlen(node_info_p[ii].data_attrib.default_value.default_string));
               of_memcpy (pUCMTempRespBuf,node_info_p[ii].data_attrib.default_value.default_string,of_strlen(node_info_p[ii].data_attrib.default_value.default_string));
               pUCMTempRespBuf += of_strlen(node_info_p[ii].data_attrib.default_value.default_string);

            }
            break;
      }
   }

   *buffer_pp = pUCMTempRespBuf;
}
/******************************************************************************
 * Function Name :
 * Description   : 
 * Input params  :
 * Output params : 
 * Return value  : 
 *******************************************************************************/
 void cm_je_copy_opaqueinfo_to_message (int32_t * opaque_info_p,
      uint32_t opaque_info_length,
      char ** buffer_pp)
{
   char *pTmpBuff;
   int32_t ii;

   pTmpBuff = *buffer_pp;

   pTmpBuff = (char *)of_mbuf_put_32 (pTmpBuff, opaque_info_length);

   for (ii = 0; ii < opaque_info_length; ii++)
   {
      pTmpBuff = (char *)of_mbuf_put_32 (pTmpBuff, *(opaque_info_p + ii));
   }

   *buffer_pp = pTmpBuff;
}

/**
  \ingroup JETRAPI
  This API Frames UCM Response Message from Configuration Request and UCM Result

  <b>Input paramameters: </b>\n
  <b><i> config_request_p: </i></b> Pointer to UCM Configuration Request.
  <b><i> pUCMResult: </i></b> Pointer to UCM Result
  <b>Output Parameters: </b>\n
  <b><i> cm_resp_msg_p: </i></b> Pointer to UCM Response Message.
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : cm_je_get_response_message_length 
 ** Description   : This API Frames UCM Response Message from Configuration Request
 **                    and UCM Result.
 ** Input params  : config_request_p - Pointer to UCM Configuration Request
 **                  result_p - Pointer to UCM Result
 ** Output params : cm_resp_msg_p - Pointer to Response Message
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
 int32_t cm_je_get_response_message_length (struct je_config_request * config_request_p,
      struct cm_result * result_p,
      uint32_t * puiBuffLen)
{
   uint32_t uiResponseLen = 0;
   uint32_t uiNvCnt = 0, uiLen = 0;
   uint32_t uiArrayCnt = 0;
   int32_t ii;
   struct cm_dm_instance_map *result_pInstMap;
   struct cm_dm_array_of_role_permissions *role_perm_ary_p;

   if (config_request_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("Configuratin Request is NULL");
      return OF_FAILURE;
   }

   //RESPONSE HEADER
   /* Copy Sub Command */
   uiResponseLen += uiOSIntSize_g;

   /* Copy Result Code */
   uiResponseLen += uiOSIntSize_g;

   //RESPONSE MESSAGE
   /* Copy Data */
   if (config_request_p->command_id == CM_COMMAND_CONFIG_SESSION_START)
   {
      switch (result_p->result_code)
      {
         case CM_JE_SUCCESS:
            {
               uiResponseLen += sizeof (struct cm_je_config_session);
               break;
            }
         case CM_JE_FAILURE:
            {
               CM_JE_DEBUG_PRINT ("Failure");
               break;
            }
      }
   }
   switch (config_request_p->sub_command_id)
   {
      case CM_CMD_CONFIG_SESSION_COMMIT:
         switch (result_p->result_code)
         {
            case CM_JE_SUCCESS:
							 {
									/* Copy Success Code */
									uiResponseLen += uiOSIntSize_g;
									CM_JE_DEBUG_PRINT
										 ("Post Configuration Succesfull for add/del/set command");
									break;
							 }
            case CM_JE_FAILURE:
							 {
									CM_JE_DEBUG_PRINT ("Post Configuration Failed");
									if (cm_je_get_errorinfo_length (result_p, &uiResponseLen)
												!= OF_SUCCESS)
									{
										 CM_JE_DEBUG_PRINT ("Get Error Info Length failed");
										 return OF_FAILURE;
									}
							 }
         }
				 break;
      case CM_CMD_CONFIG_SESSION_REVOKE:
      case CM_CMD_ADD_PARAMS:
      case CM_CMD_CANCEL_TRANS_CMD:
      case CM_CMD_EXEC_TRANS_CMD:
      case CM_CMD_SET_PARAMS:
      case CM_CMD_DEL_PARAMS:
         switch (result_p->result_code)
				 {
						case CM_JE_SUCCESS:
							 {
									CM_JE_DEBUG_PRINT
										 ("Revoke Succesfull for add/del/set command");
									break;
							 }
						case CM_JE_FAILURE:
							 {
									CM_JE_DEBUG_PRINT ("Post Configuration Failed");
									if (cm_je_get_errorinfo_length (result_p, &uiResponseLen)
												!= OF_SUCCESS)
									{
										 CM_JE_DEBUG_PRINT ("Get Error Info Length failed");
										 return OF_FAILURE;
									}
							 }
				 }
         break;
      case CM_CMD_LOAD_CONFIG:
      case CM_CMD_SAVE_CONFIG:
      case CM_CMD_RESET_TO_FACTORY_DEFAULTS:
      case CM_CMD_FLUSH_CONFIG:
      case CM_CMD_REBOOT_DEVICE:
      case CM_CMD_NOTIFY_JE:
         switch (result_p->result_code)
         {
            case CM_JE_SUCCESS:
               break;
            case CM_JE_FAILURE:
               {
                  if (cm_je_get_errorinfo_length (result_p, &uiResponseLen)
                        != OF_SUCCESS)
                  {
                     CM_JE_DEBUG_PRINT ("Get Error Info Length failed");
                     return OF_FAILURE;
                  }
               }
         }
         break;
      case CM_CMD_COMPARE_PARAM:
      case CM_SUBCOMMAND_AUTHENTICATE_USER:
      case CM_CMD_GET_VERSIONS:
         switch (result_p->result_code)
         {
            case CM_JE_SUCCESS:
               {
                  struct cm_array_of_nv_pair *pNvPairAry =
                     (struct cm_array_of_nv_pair *) result_p->data.je_data_p;
                  /* Name Value Pair */
                  if (pNvPairAry)
                  {
                     /* NvPair count */
                     uiResponseLen += uiOSIntSize_g;

                     /* Copy Each Nv Pair */
                     for (uiNvCnt = 0, uiLen = 0; uiNvCnt < pNvPairAry->count_ui;
                           uiNvCnt++, uiLen = 0)
                     {
                        CM_JE_GET_NVPAIR_LEN (pNvPairAry->nv_pairs[uiNvCnt], uiLen);
                        uiResponseLen += uiLen;
                     }
                  }
                  break;
               }
            case CM_JE_FAILURE:
               {
                  CM_JE_DEBUG_PRINT ("getversion or compare param Failed");
                  if (cm_je_get_errorinfo_length (result_p, &uiResponseLen)
                        != OF_SUCCESS)
                  {
                     CM_JE_DEBUG_PRINT ("Get Error Info Length failed");
                     return OF_FAILURE;
                  }
               }
         }
         break;

      case CM_CMD_CANCEL_PREV_CMD:
         switch (result_p->result_code)
         {
            case CM_JE_SUCCESS:
               break;
            case CM_JE_FAILURE:
               {
                  CM_JE_DEBUG_PRINT ("cancel prev command  Failed");
                  if (cm_je_get_errorinfo_length (result_p, &uiResponseLen)
                        != OF_SUCCESS)
                  {
                     CM_JE_DEBUG_PRINT ("Get Error Info Length failed");
                     return OF_FAILURE;
                  }
               }
         }
         break;
      case CM_CMD_GET_CONFIG_SESSION:
         switch (result_p->result_code)
         {
            case CM_JE_SUCCESS:
               {
                  uint32_t no_of_commands = 0;
                  struct cm_array_of_commands *pAryOfCmds = result_p->data.je_data_p;

                  /* Number of Commands */
                  uiResponseLen += uiOSIntSize_g;

                  /* for each Command */
                  for (no_of_commands = 0; no_of_commands < pAryOfCmds->count_ui; no_of_commands++)
                  {
                     /* Command Id */
                     uiResponseLen += uiOSIntSize_g;

                     /* DM Path Length Field */
                     uiResponseLen += uiOSIntSize_g;

                     if (pAryOfCmds->Commands[no_of_commands].dm_path_p)
                     {
                        uiResponseLen +=
                           of_strlen (pAryOfCmds->Commands[no_of_commands].dm_path_p);
                     }

                     /* Name Value Pair */
                     if (pAryOfCmds->Commands[no_of_commands].nv_pair_array.nv_pairs)
                     {
                        /* NvPair count */
                        uiResponseLen += uiOSIntSize_g;

                        /* Copy Each Nv Pair */
                        for (uiNvCnt = 0, uiLen = 0;
                              uiNvCnt <
                              pAryOfCmds->Commands[no_of_commands].nv_pair_array.count_ui;
                              uiNvCnt++, uiLen = 0)
                        {
                           CM_JE_GET_NVPAIR_LEN (pAryOfCmds->Commands[no_of_commands].
                                 nv_pair_array.nv_pairs[uiNvCnt], uiLen);
                           uiResponseLen += uiLen;
                        }
                     }
                  }
                  break;
               }
            case CM_JE_FAILURE:
               {
                  CM_JE_DEBUG_PRINT ("Get Config Session Failed");
                  if (cm_je_get_errorinfo_length (result_p, &uiResponseLen)
                        != OF_SUCCESS)
                  {
                     CM_JE_DEBUG_PRINT ("Get Error Info Length failed");
                     return OF_FAILURE;
                  }
               }
         }
         break;
      case  CM_CMD_GET_DELTA_VERSION_HISTORY:
         switch (result_p->result_code)
         {
            case CM_JE_SUCCESS:
               {
                  struct cm_version *version_list_p= result_p->data.version.version_list_p;
                  uint32_t uiversionCnt = 0;
                  uint32_t no_of_commands = 0;

                  /* Number of versions */
                  uiResponseLen += uiOSIntSize_g;

                  /* for each version */
                  for (uiversionCnt = 0; uiversionCnt <  result_p->data.version.count_ui ; uiversionCnt++)
                  {
                     struct cm_array_of_commands *pAryOfCmds=  version_list_p[uiversionCnt].command_list_p;

                     /* version Number */
                     uiResponseLen += uiInt64Size_g;

                     /* Number of Commands */
                     uiResponseLen += uiOSIntSize_g;

                     /* for each Command */
                     for (no_of_commands = 0; no_of_commands < pAryOfCmds->count_ui; no_of_commands++)
                     {
                        /* Command Id */
                        uiResponseLen += uiOSIntSize_g;

                        /* DM Path Length Field */
                        uiResponseLen += uiOSIntSize_g;

                        if (pAryOfCmds->Commands[no_of_commands].dm_path_p)
                        {
                           uiResponseLen +=
                              of_strlen (pAryOfCmds->Commands[no_of_commands].dm_path_p);
                        }

                        /* Name Value Pair */
                        if (pAryOfCmds->Commands[no_of_commands].nv_pair_array.nv_pairs)
                        {
                           /* NvPair count */
                           uiResponseLen += uiOSIntSize_g;

                           /* Copy Each Nv Pair */
                           for (uiNvCnt = 0, uiLen = 0;
                                 uiNvCnt <
                                 pAryOfCmds->Commands[no_of_commands].nv_pair_array.count_ui;
                                 uiNvCnt++, uiLen = 0)
                           {
                              CM_JE_GET_NVPAIR_LEN (pAryOfCmds->Commands[no_of_commands].
                                    nv_pair_array.nv_pairs[uiNvCnt], uiLen);
                              uiResponseLen += uiLen;
                           }
                        }

                     }
                  }
               }
               break;
            case CM_JE_FAILURE:
               {
                  CM_JE_DEBUG_PRINT ("Get Config Session Failed");
                  if (cm_je_get_errorinfo_length (result_p, &uiResponseLen)
                        != OF_SUCCESS)
                  {
                     CM_JE_DEBUG_PRINT ("Get Error Info Length failed");
                     return OF_FAILURE;
                  }
               }
               break;
         }
         break;
      case CM_CMD_GET_FIRST_N_LOGS:
      case CM_CMD_GET_NEXT_N_LOGS:
      case CM_CMD_GET_FIRST_N_RECS:
      case CM_CMD_GET_NEXT_N_RECS:
      case CM_CMD_GET_EXACT_REC:
#ifdef CM_STATS_COLLECTOR_SUPPORT
      case CM_CMD_GET_AGGREGATE_STATS:
      case CM_CMD_GET_PER_DEVICE_STATS:
      case CM_CMD_GET_AVERAGE_STATS: 
#endif
         switch (result_p->result_code)
         {
            case CM_JE_SUCCESS:
            case CM_JE_NO_MORE_RECS:
            case CM_JE_MORE_RECS:

               /*Copy Record Count */
               uiResponseLen += uiOSIntSize_g;

               /*  Array of Nv Pair */
               if (result_p->data.sec_appl.nv_pair_array)
               {
                  for (uiArrayCnt = 0;
                        uiArrayCnt < result_p->data.sec_appl.count_ui; uiArrayCnt++)
                  {
                     /*NV Pair Count */
                     uiResponseLen += uiOSIntSize_g;

                     /* Name Value Pair */
                     if (result_p->data.sec_appl.nv_pair_array[uiArrayCnt].nv_pairs)
                     {
                        /* Copy Each Nv Pair */
                        for (uiNvCnt = 0, uiLen = 0;
                              uiNvCnt <
                              result_p->data.sec_appl.nv_pair_array[uiArrayCnt].count_ui;
                              uiNvCnt++, uiLen = 0)
                        {
                           CM_JE_GET_NVPAIR_LEN ((result_p->data.sec_appl.
                                    nv_pair_array[uiArrayCnt].nv_pairs
                                    [uiNvCnt]), uiLen);
                           uiResponseLen += uiLen;
                        }
                     }
                  }
               }
               break;

            case CM_JE_FAILURE:
               {
                  CM_JE_DEBUG_PRINT ("Get Configuration Failed");
                  if (cm_je_get_errorinfo_length (result_p, &uiResponseLen)
                        != OF_SUCCESS)
                  {
                     CM_JE_DEBUG_PRINT ("Get Error Info Length failed");
                     return OF_FAILURE;
                  }
               }
               break;
         }
         break;

      case CM_CMD_GET_DM_NODE_INFO:
      case CM_CMD_GET_DM_KEY_CHILD_INFO:
      case CM_CMD_GET_DM_FIRST_NODE_INFO:
      case CM_CMD_GET_DM_NEXT_NODE_INFO:
      case CM_CMD_GET_DM_NEXT_TABLE_ANCHOR_NODE_INFO:
         switch (result_p->result_code)
         {
            case CM_JE_SUCCESS:
               {
                  /*For Node Count */
                  uiResponseLen += uiOSIntSize_g;
                  CM_JE_DEBUG_PRINT("Case CM_CMD_GET_DM_NODE_INFO");
                  /* Get Node Info len */
                  cm_je_get_nodeinfo_length (result_p->data.dm_info.node_info_p,
                        result_p->data.dm_info.count_ui, &uiResponseLen);
                  /* For OpaqueInfo */
                  if ((config_request_p->sub_command_id == CM_CMD_GET_DM_FIRST_NODE_INFO)
                        || (config_request_p->sub_command_id == CM_CMD_GET_DM_NEXT_NODE_INFO)
                        || (config_request_p->sub_command_id == CM_CMD_GET_DM_NEXT_TABLE_ANCHOR_NODE_INFO))
                  {
                     uiResponseLen +=
                        (result_p->data.dm_info.opaque_info_length * uiOSIntSize_g) +
                        uiOSIntSize_g;
                  }
                  break;
               }
            case CM_JE_FAILURE:
               {
                  if (cm_je_get_errorinfo_length (result_p, &uiResponseLen)
                        != OF_SUCCESS)
                  {
                     CM_JE_DEBUG_PRINT ("Get Error Info Length failed");
                     return OF_FAILURE;
                  }
               }
         }
         break;

      case CM_CMD_GET_DM_NODE_AND_CHILD_INFO:
      case CM_CMD_GET_DM_KEY_CHILDS:
         switch (result_p->result_code)
         {
            case CM_JE_SUCCESS:
               {
                  /* Copy Node Count */
                  uiResponseLen += uiOSIntSize_g;

                  CM_JE_DEBUG_PRINT("success CM_CMD_GET_DM_NODE_AND_CHILD_INFO:");
                  /* For NodeInfo */
                  cm_je_get_nodeinfo_length (result_p->data.dm_info.node_info_p,
                        result_p->data.dm_info.count_ui, &uiResponseLen);


                  /* Child Node Count */
                  uiResponseLen += uiOSIntSize_g;
                  cm_je_get_nodeinfo_length (result_p->data.dm_info.node_info_array_p->struct_arr_p,
                        result_p->data.dm_info.node_info_array_p->count_ui,
                        &uiResponseLen);
                  break;
               }
            case CM_JE_FAILURE:
               {
                  if (cm_je_get_errorinfo_length (result_p, &uiResponseLen)
                        != OF_SUCCESS)
                  {
                     CM_JE_DEBUG_PRINT ("Get Error Info Length failed");
                     return OF_FAILURE;
                  }
               }
         }
         break;

      case CM_CMD_GET_DM_KEYS_ARRAY:
         switch (result_p->result_code)
         {
            case CM_JE_SUCCESS:
               {
                  /* For NvPair count */
                  uiResponseLen += uiOSIntSize_g;

                  if (result_p->data.dm_info.dm_node_nv_pairs_p->nv_pairs)
                  {
                     /* Copy Each Nv Pair */
                     for (ii = 0; ii < result_p->data.dm_info.dm_node_nv_pairs_p->count_ui;
                           ii++)
                     {
                        CM_JE_GET_NVPAIR_LEN (result_p->data.dm_info.dm_node_nv_pairs_p->
                              nv_pairs[ii], uiResponseLen);
                     }
                  }
                  break;
               }
            case CM_JE_FAILURE:
               {
                  if (cm_je_get_errorinfo_length (result_p, &uiResponseLen)
                        != OF_SUCCESS)
                  {
                     CM_JE_DEBUG_PRINT ("Get Error Info Length failed");
                     return OF_FAILURE;
                  }

               }
         }
         break;

      case CM_CMD_GET_DM_INSTANCE_MAPLIST_BY_NAMEPATH:
         switch (result_p->result_code)
         {
            case CM_JE_SUCCESS:
               {
                  /* Copy Node Count */
                  uiResponseLen += uiOSIntSize_g;

                  /* Copy Instance Map Info from Result to Resp */
                  for (ii = 0; ii < result_p->data.dm_info.count_ui; ii++)
                  {
                     result_pInstMap = &result_p->data.dm_info.instance_map_list_p[ii];
                     uiResponseLen += uiOSIntSize_g;
                     uiResponseLen += CM_UINT8_SIZE;
                     uiResponseLen += uiOSIntSize_g;
                     if (result_pInstMap->key_p)
                     {
                        uiResponseLen += result_pInstMap->key_length;
                     }
                  }
                  break;
               }
            case CM_JE_FAILURE:
               {
                  if (cm_je_get_errorinfo_length (result_p, &uiResponseLen)
                        != OF_SUCCESS)
                  {
                     CM_JE_DEBUG_PRINT ("Get Error Info Length failed");
                     return OF_FAILURE;
                  }
               }
         }
         break;
      case CM_CMD_GET_SAVE_FILE_NAME :
      case CM_CMD_GET_SAVE_DIRECTORY_NAME:
         switch (result_p->result_code)
         {
            case CM_JE_SUCCESS:
               {
                  /* Copy Opaque Len */
                  uiResponseLen += uiOSIntSize_g;
                  uiResponseLen += result_p->data.dm_info.opaque_info_length;
                  break;
               }
            case CM_JE_FAILURE:
               {
                  if (cm_je_get_errorinfo_length (result_p, &uiResponseLen)
                        != OF_SUCCESS)
                  {
                     CM_JE_DEBUG_PRINT ("Get Error Info Length failed");
                     return OF_FAILURE;
                  }
                  break;
               }
            default:
               break;
         }
         break;
      case CM_CMD_IS_DM_CHILD_NODE:
      case CM_CMD_IS_DM_INSTANCE_USING_NAMEPATH_AND_KEY:
         {
            /*              pResponseMsg->result.success.reserved); */
            uiResponseLen += uiOSIntSize_g;
            if (result_p->result_code ==  CM_JE_FAILURE)
            {
               if (cm_je_get_errorinfo_length (result_p, &uiResponseLen)
                     != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("Get Error Info Length failed");
                  return OF_FAILURE;
               }
            }
         }
         break;
      case CM_CMD_GET_DM_CHILD_NODE_COUNT:
         break;

      case CM_CMD_GET_DM_INSTANCE_ROLE_ATTRIBS:
      case CM_CMD_GET_DM_NODE_ROLE_ATTRIBS:
         if (result_p->result_code == CM_JE_SUCCESS)
         {
            role_perm_ary_p=( struct cm_dm_array_of_role_permissions *)result_p->data.dm_info.opaque_info_p;
            uiResponseLen += uiOSIntSize_g;
            uiResponseLen +=(role_perm_ary_p->count_ui * sizeof(struct cm_dm_role_permission));
         }
         else if (result_p->result_code ==  CM_JE_FAILURE)
         {
            if (cm_je_get_errorinfo_length (result_p, &uiResponseLen)
                  != OF_SUCCESS)
            {
               CM_JE_DEBUG_PRINT ("Get Error Info Length failed");
               return OF_FAILURE;
            }
         }
         break;
      case CM_CMD_GET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_GET_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
         if (result_p->result_code == CM_JE_SUCCESS)
         {
            uiResponseLen += sizeof(struct cm_dm_role_permission);
         }
         else if (result_p->result_code ==  CM_JE_FAILURE)
         {
            if (cm_je_get_errorinfo_length (result_p, &uiResponseLen)
                  != OF_SUCCESS)
            {
               CM_JE_DEBUG_PRINT ("Get Error Info Length failed");
               return OF_FAILURE;
            }
         }
         break;
      case CM_CMD_SET_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_DEL_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_SET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_DEL_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
         if (result_p->result_code ==  CM_JE_FAILURE)
         {
            if (cm_je_get_errorinfo_length (result_p, &uiResponseLen)
                  != OF_SUCCESS)
            {
               CM_JE_DEBUG_PRINT ("Get Error Info Length failed");
               return OF_FAILURE;
            }
         }
         break;
      case CM_CMD_GET_NEW_DM_INSTANCE_ID:
      case CM_CMD_ADD_DM_INSTANCE_MAP:
      case CM_CMD_DEL_DM_INSTANCE_MAP:
         break;
         // copy into    config_request_p->data.data_modelGet.count_ui:
         break;
#ifdef CM_STATS_COLLECTOR_SUPPORT
      case CM_CMD_COALESCAR_RESP:
         uiResponseLen += config_request_p->data.stats_data.nv_pair_flat_buf_len_ui;
         break;
#endif 
      default:
         CM_JE_DEBUG_PRINT ("Un handled sub command");
         break;
   }
   /* Increment Len field */
   uiResponseLen += uiOSIntSize_g;
   *puiBuffLen += uiResponseLen;
   CM_JE_DEBUG_PRINT ("Result Message Len=%d GenHeader Len=%d", uiResponseLen,
         *puiBuffLen);
   return OF_SUCCESS;
}

/******************************************************************************
 * Function Name :
 * Description   : 
 * Input params  :
 * Output params : 
 * Return value  : 
 *******************************************************************************/
 int32_t cm_je_get_nodeinfo_length (struct cm_dm_node_info * node_info_p,
      uint32_t uiNodeCnt,
      uint32_t * puiResponseLen)
{
   int32_t ii;
   uint32_t uiResponseLen = 0;
   uint32_t uidata_attribLen = 0;
   uint32_t uielement_attribLen = 0;


   for (ii = 0; ii < uiNodeCnt; ii++)
   {
      uielement_attribLen =0;
      uidata_attribLen=0;
      cm_je_get_elementattrib_length(&node_info_p[ii],&uielement_attribLen);
      cm_je_get_dataattrib_length(&node_info_p[ii],&uidata_attribLen);


      uiResponseLen += sizeof (int32_t);
      if(node_info_p[ii].name_p !=NULL)
      {
         uiResponseLen += of_strlen (node_info_p[ii].name_p);
         //CM_JE_DEBUG_PRINT("node %s",node_info_p[ii].name_p);
      }

      uiResponseLen += sizeof (int32_t);
      if(node_info_p[ii].friendly_name_p != NULL)
      { 
         uiResponseLen += of_strlen (node_info_p[ii].friendly_name_p);
      }
      uiResponseLen += sizeof (int32_t);
      if(node_info_p[ii].description_p !=NULL)
         uiResponseLen += of_strlen (node_info_p[ii].description_p);

      uiResponseLen += sizeof (int32_t);
      if(node_info_p[ii].dm_path_p !=NULL)
         uiResponseLen += of_strlen (node_info_p[ii].dm_path_p);
      /*      if(node_info_p[ii].element_attrib.command_name_p)
              uiResponseLen += of_strlen (node_info_p[ii].element_attrib.command_name_p);
              if(node_info_p[ii].element_attrib.cli_identifier_p)
              uiResponseLen += of_strlen (node_info_p[ii].element_attrib.cli_identifier_p);
              if(node_info_p[ii].element_attrib.cli_idenetifier_help_p)
              uiResponseLen += of_strlen (node_info_p[ii].element_attrib.cli_idenetifier_help_p);
              */
      uiResponseLen += sizeof(uint32_t); /* for Uid */
      uiResponseLen += sizeof(uint32_t); /* for file_name */
      uiResponseLen += of_strlen(node_info_p[ii].file_name); /* for file_name */
      uiResponseLen += sizeof(unsigned char); /* for save_b*/

      uiResponseLen += uielement_attribLen;
      uiResponseLen += uidata_attribLen;



   }

   //CM_JE_DEBUG_PRINT("response len %d",uiResponseLen);

   *puiResponseLen += uiResponseLen;
   return OF_SUCCESS;
}

/**
  \ingroup JETRAPI
  This API Frees UCM Configuration Request

  <b>Input paramameters: </b>\n
  <b><i> config_request_p: </i></b> Pointer to UCM Configuration Request.
  <b>Output Parameters: </b> None\n
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/*******************************************************************************
 ** Function Name : cm_je_free_config_request
 ** Description   : This API Frees UCM Configuration Request
 ** Input params  : config_request_p - Pointer to UCM Configuration Request
 ** Output params : None
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
 int32_t cm_je_free_config_request (struct je_config_request * config_request_p)
{

   if (config_request_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("Input Request is NULL");
      return OF_FAILURE;
   }

   switch (config_request_p->sub_command_id)
   {
      case CM_CMD_ADD_PARAMS:
      case CM_CMD_SET_PARAMS:
      case CM_CMD_DEL_PARAMS:
      case CM_CMD_IS_DM_INSTANCE_USING_NAMEPATH_AND_KEYARRAY:
#ifdef CM_STATS_COLLECTOR_SUPPORT
      case CM_CMD_GET_AGGREGATE_STATS:
      case CM_CMD_GET_PER_DEVICE_STATS:
      case CM_CMD_GET_AVERAGE_STATS: 
#endif
         //CM_JE_DEBUG_PRINT ("Free Input NV Pairs for Post Request");
         je_free_nvpair_array (&config_request_p->data.nv_pair_array);
         break;

      case CM_CMD_LOAD_CONFIG:
      case CM_CMD_SAVE_CONFIG:
      case CM_CMD_SET_DEFAULTS:
         break;

      case CM_CMD_GET_FIRST_N_LOGS:
      case CM_CMD_GET_NEXT_N_LOGS:
      case CM_CMD_GET_FIRST_N_RECS:
      case CM_CMD_GET_NEXT_N_RECS:
      case CM_CMD_GET_EXACT_REC:
         CM_JE_DEBUG_PRINT ("Free Input NV Pairs for Get Request");
         je_free_nvpair_array (&config_request_p->data.sec_appl.key_nv_pairs);
         if (config_request_p->sub_command_id == CM_CMD_GET_NEXT_N_RECS)
         {
            //  je_free_nvpair (config_request_p->data.sec_appl.prev_rec_key_nv_pairs);
            je_free_nvpair_array (&config_request_p->data.sec_appl.prev_rec_key_nv_pairs);
         }
         break;
      case CM_CMD_IS_DM_INSTANCE_USING_NAMEPATH_AND_KEY:
      case CM_CMD_IS_DM_CHILD_NODE:
         of_free (config_request_p->data.data_model.opaque_info_p);
         break;
      case CM_CMD_GET_DM_FIRST_NODE_INFO:
         break;
      case CM_CMD_GET_DM_NEXT_NODE_INFO:
      case CM_CMD_GET_DM_NEXT_TABLE_ANCHOR_NODE_INFO:
         /* In failure cases it may not be freed by DM api */
#if 0
         if (config_request_p->data.data_model.opaque_info_p)
         {
            of_free(config_request_p->data.data_model.opaque_info_p);
         }
#endif
         break;
      case CM_CMD_GET_DM_NODE_INFO:
      case CM_CMD_GET_DM_NODE_AND_CHILD_INFO:
      case CM_CMD_GET_DM_CHILD_NODE_COUNT:
      case CM_CMD_SET_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
         break;
      case CM_CMD_GET_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
         /* It contains request and responce RolePerm structure*/
         of_free (config_request_p->data.data_model.opaque_info_p);
         break;
      case CM_CMD_GET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
         /* It contains request and responce RolePerm structure*/
         of_free (config_request_p->data.data_model.opaque_info_p);
#if UCMDM_INSTANCE_ROLE_AND_PERMISSIONS_USING_KEY
         of_free (config_request_p->data.data_model.key_p);
#endif
         break;
      case CM_CMD_SET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_DEL_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
#if UCMDM_INSTANCE_ROLE_AND_PERMISSIONS_USING_KEY
         of_free (config_request_p->data.data_model.key_p);
#endif
         break;
      case CM_CMD_DEL_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_GET_NEW_DM_INSTANCE_ID:
      case CM_CMD_ADD_DM_INSTANCE_MAP:
      case CM_CMD_DEL_DM_INSTANCE_MAP:
         break;
      case CM_CMD_NOTIFY_JE:
         of_free (config_request_p->data.Notify.pData);
         break;
   }
   if (config_request_p->dm_path_p)
   {
      of_free (config_request_p->dm_path_p);
   }
   cm_je_cfgrequest_free (config_request_p);
   return OF_SUCCESS;
}

 void cm_je_free_nodeinfo_struct (uint32_t uiNodeCnt,
      struct cm_dm_node_info * node_info_p)
{
	 uint32_t ii=0, iCnt=0;

	 for (ii = 0; ii < uiNodeCnt; ii++)
	 {
			if (node_info_p[ii].name_p)
			{
				 of_free (node_info_p[ii].name_p);
				 node_info_p[ii].name_p = NULL;
			}

			if (node_info_p[ii].friendly_name_p != NULL)
			{
				 of_free (node_info_p[ii].friendly_name_p);
				 node_info_p[ii].friendly_name_p = NULL;
			} 
			if (node_info_p[ii].description_p)
			{
				 of_free (node_info_p[ii].description_p);
				 node_info_p[ii].description_p = NULL;
			}

			if (node_info_p[ii].dm_path_p)
			{
				 of_free (node_info_p[ii].dm_path_p);
				 node_info_p[ii].dm_path_p = NULL;
			}
			/*Atmaram - Memory Leak Here - not freeing CommandName, cliidentifier,
			 * cliidentifier help ... etc */
			if (node_info_p[ii].element_attrib.command_name_p)
			{ 
				 of_free(node_info_p[ii].element_attrib.command_name_p);
				 node_info_p[ii].element_attrib.command_name_p=NULL;
			}
			if (node_info_p[ii].element_attrib.cli_identifier_p)
			{ 
				 of_free(node_info_p[ii].element_attrib.cli_identifier_p);
				 node_info_p[ii].element_attrib.cli_identifier_p=NULL;
			}
			if (node_info_p[ii].element_attrib.cli_idenetifier_help_p)
			{ 
				 of_free(node_info_p[ii].element_attrib.cli_idenetifier_help_p);
				 node_info_p[ii].element_attrib.cli_idenetifier_help_p=NULL;
			}
			if((node_info_p[ii].data_attrib.data_type == CM_DATA_TYPE_STRING || node_info_p[ii].data_attrib.data_type == CM_DATA_TYPE_STRING_SPECIAL_CHARS) && node_info_p[ii].data_attrib.attrib_type == CM_DATA_ATTRIB_STR_ENUM)
			{
				 for( iCnt =0 ;iCnt<node_info_p[ii].data_attrib.attr.string_enum.count_ui;iCnt++)
				 {
						if (of_strlen(node_info_p[ii].data_attrib.attr.string_enum.array[iCnt])> 0 )
						of_free(node_info_p[ii].data_attrib.attr.string_enum.array[iCnt]);
				 }
			}
			if((node_info_p[ii].data_attrib.data_type == CM_DATA_TYPE_STRING || node_info_p[ii].data_attrib.data_type == CM_DATA_TYPE_STRING_SPECIAL_CHARS)&& node_info_p[ii].data_attrib.attrib_type == CM_DATA_ATTRIB_STR_ENUM)
			{
				 for( iCnt =0 ;iCnt < node_info_p[ii].data_attrib.attr.string_enum.count_ui;iCnt++)
				 {
						if (of_strlen(node_info_p[ii].data_attrib.attr.string_enum.aFrdArr[iCnt])> 0 )
						of_free(node_info_p[ii].data_attrib.attr.string_enum.aFrdArr[iCnt]);
				 }
			}
	 }
	 of_free (node_info_p);
}

/**
  \ingroup JETRAPI
  This API Frees UCM Result structure. It uses Command ID and Sub Command ID to access
  its union members.

  <b>Input paramameters: </b>\n
  <b><i> command_id: </i></b> command_id - Command ID from any of the following
  CM_COMMAND_CONFIG_SESSION_START
  CM_COMMAND_CONFIG_SESSION_UPDATE
  CM_COMMAND_CONFIG_SESSION_END
  CM_COMMAND_DM_REQUEST
  CM_COMMAND_APPL_REQUEST
  CM_COMMAND_RESPONSE
  <b><i> sub_command_id: </i></b> sub_command_id - Sub Command ID from any of the following
  CM_CMD_ADD_PARAMS
  CM_CMD_SET_PARAMS
  CM_CMD_DEL_PARAMS
  CM_CMD_LOAD_CONFIG
  CM_CMD_SAVE_CONFIG
  CM_CMD_SET_DEFAULTS
  CM_CMD_GET_FIRST_N_RECS
  CM_CMD_GET_NEXT_N_RECS
  CM_CMD_GET_EXACT_REC
  CM_CMD_GET_DM_NODE_INFO
  CM_CMD_GET_DM_NODE_AND_CHILD_INFO
  CM_CMD_GET_DM_CHILD_NODE_COUNT
  CM_CMD_SET_DM_NODE_ROLE_ATTRIBS_BY_ROLE
  CM_CMD_GET_DM_NODE_ROLE_ATTRIBS_BY_ROLE
  CM_CMD_DEL_DM_NODE_ROLE_ATTRIBS_BY_ROLE
  CM_CMD_GET_DM_FIRST_NODE_INFO
  CM_CMD_GET_DM_NEXT_NODE_INFO
  CM_CMD_GET_NEW_DM_INSTANCE_ID
  CM_CMD_GET_ADD_DM_INSTANCE_MAP
  CM_CMD_GET_DM_INSTANCE_MAPLIST_BY_NAMEPATH
  CM_CMD_GET_DEL_DM_INSTANCE_MAP
  CM_CMD_GET_GET_DM_INSTANCE_ROLE_ATTRIBS
  CM_CMD_GET_SET_DM_INSTANCE_ROLE_ATTRIBS
  CM_CMD_GET_DEL_DM_INSTANCE_ROLE_ATTRIBS
  Pointer to UCM Response Message structure.
  <b><i> cm_resp_msg_p: </i></b> Pointer to UCM Response Message structure.
  <b>Output Parameters: </b> None\n
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/*******************************************************************************
 ** Function Name : ucmJEFreeResult
 ** Description   : This API Frees UCM Result
 ** Input params  : command_id - Command ID from any of the following
 **                      CM_COMMAND_CONFIG_SESSION_START
 **                      CM_COMMAND_CONFIG_SESSION_UPDATE
 **                      CM_COMMAND_CONFIG_SESSION_END
 **                      CM_COMMAND_DM_REQUEST
 **                      CM_COMMAND_APPL_REQUEST
 **                      CM_COMMAND_RESPONSE
 **                : sub_command_id - Sub Command  from any of the following
 **                      CM_CMD_ADD_PARAMS
 **                      CM_CMD_SET_PARAMS
 **                      CM_CMD_DEL_PARAMS
 **                      CM_CMD_LOAD_CONFIG
 **                      CM_CMD_SAVE_CONFIG
 **                      CM_CMD_SET_DEFAULTS
 **                      CM_CMD_GET_FIRST_N_RECS
 **                      CM_CMD_GET_NEXT_N_RECS
 **                      CM_CMD_GET_EXACT_REC
 **                      CM_CMD_GET_DM_NODE_INFO
 **                      CM_CMD_GET_DM_NODE_AND_CHILD_INFO
 **                      CM_CMD_GET_DM_CHILD_NODE_COUNT
 **                      CM_CMD_SET_DM_NODE_ROLE_ATTRIBS_BY_ROLE
 **                      CM_CMD_GET_DM_NODE_ROLE_ATTRIBS_BY_ROLE
 **                      CM_CMD_DEL_DM_NODE_ROLE_ATTRIBS_BY_ROLE
 **                      CM_CMD_GET_DM_FIRST_NODE_INFO
 **                      CM_CMD_GET_DM_NEXT_NODE_INFO
 **                      CM_CMD_GET_NEW_DM_INSTANCE_ID
 **                      CM_CMD_GET_ADD_DM_INSTANCE_MAP
 **                      CM_CMD_GET_DM_INSTANCE_MAPLIST_BY_NAMEPATH
 **                      CM_CMD_GET_DEL_DM_INSTANCE_MAP
 **                      CM_CMD_GET_GET_DM_INSTANCE_ROLE_ATTRIBS
 **                      CM_CMD_GET_SET_DM_INSTANCE_ROLE_ATTRIBS
 **                      CM_CMD_GET_DEL_DM_INSTANCE_ROLE_ATTRIBS
 **                 : pUCMResult - Pointer to UCM result structure to be freed
 ** Output params : None
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
 int32_t cm_je_free_ucmresult (uint32_t command_id, uint32_t sub_command_id,
      struct cm_result * pUCMResult)
{
   int32_t ii;
   uint32_t uiArrayCnt = 0, uindex_i;
   struct cm_dm_array_of_role_permissions *role_perm_ary_p;

   if (pUCMResult == NULL)
   {
      CM_JE_DEBUG_PRINT ("Input Request is NULL");
      return OF_FAILURE;
   }

   if (command_id == CM_COMMAND_CONFIG_SESSION_START)
   {
      if (pUCMResult->result_code == CM_JE_SUCCESS)
      {
         /* It is  a struct cm_je_config_session type
          * and not allocated from Memory Pool
          */
         of_free (pUCMResult->data.je_data_p);
      }
   }

   switch (sub_command_id)
   {
      case CM_CMD_ADD_PARAMS:
      case CM_CMD_SET_PARAMS:
      case CM_CMD_DEL_PARAMS:
         break;

      case CM_CMD_LOAD_CONFIG:
      case CM_CMD_SAVE_CONFIG:
      case CM_CMD_SET_DEFAULTS:
         break;

      case CM_CMD_GET_FIRST_N_LOGS:
      case CM_CMD_GET_NEXT_N_LOGS:
      case CM_CMD_GET_FIRST_N_RECS:
      case CM_CMD_GET_NEXT_N_RECS:
      case CM_CMD_GET_EXACT_REC:
         if (pUCMResult->result_code == CM_JE_SUCCESS)
         {
            for (uiArrayCnt = 0; uiArrayCnt < pUCMResult->data.sec_appl.count_ui;
                  uiArrayCnt++)
            {
               je_free_nvpair_array (&pUCMResult->data.
                     sec_appl.nv_pair_array[uiArrayCnt]);
            }
            of_free (pUCMResult->data.sec_appl.nv_pair_array);
         }
         break;
      case CM_SUBCOMMAND_AUTHENTICATE_USER:
      case CM_CMD_GET_VERSIONS:
         if (pUCMResult->result_code == CM_JE_SUCCESS)
         {
            cm_je_free_ptr_nvpairArray((struct cm_array_of_nv_pair *)pUCMResult->data.je_data_p);
         }
         break;

      case CM_CMD_GET_CONFIG_SESSION:
         if (pUCMResult->result_code == CM_JE_SUCCESS)
         {
            struct cm_array_of_commands *pCmdAry=NULL;
            pCmdAry=(struct cm_array_of_commands *)pUCMResult->data.je_data_p;
            je_free_array_commands(pCmdAry, pCmdAry->count_ui);
         }
         break;
      case CM_CMD_GET_DELTA_VERSION_HISTORY:
         if (pUCMResult->result_code == CM_JE_SUCCESS)
         {
            struct cm_version *version_list_p=pUCMResult->data.version.version_list_p;
            for(uindex_i=0; uindex_i <  pUCMResult->data.version.count_ui; uindex_i++)
            {
               struct cm_array_of_commands *pCmdAry;
               CM_JE_DEBUG_PRINT("Free version %llx",version_list_p[uindex_i].version);
               pCmdAry=version_list_p[uindex_i].command_list_p;
               // je_free_array_commands(pCmdAry, pCmdAry->count_ui) ; 
               /* Above dmpath and nvpairs are deleted, while consolidating
                * config version history_t */
               of_free(pCmdAry);
            }
            of_free(version_list_p);
         }
         break;
      case CM_CMD_GET_DM_NODE_INFO:
      case CM_CMD_GET_DM_KEY_CHILD_INFO:
      case CM_CMD_GET_DM_FIRST_NODE_INFO:
      case CM_CMD_GET_DM_NEXT_NODE_INFO:
      case CM_CMD_GET_DM_NEXT_TABLE_ANCHOR_NODE_INFO:
         if (pUCMResult->result_code == CM_JE_SUCCESS)
         {
            if (pUCMResult->data.dm_info.node_info_p->name_p)
            {
               of_free (pUCMResult->data.dm_info.node_info_p->name_p);
               pUCMResult->data.dm_info.node_info_p->name_p = NULL;
            }

            if (pUCMResult->data.dm_info.node_info_p->friendly_name_p)
            {
               of_free (pUCMResult->data.dm_info.node_info_p->friendly_name_p);
               pUCMResult->data.dm_info.node_info_p->friendly_name_p = NULL;
            }


            if (pUCMResult->data.dm_info.node_info_p->description_p)
            {
               of_free (pUCMResult->data.dm_info.node_info_p->description_p);
               pUCMResult->data.dm_info.node_info_p->description_p = NULL;
            }

            if (pUCMResult->data.dm_info.node_info_p->dm_path_p)
            {
               of_free (pUCMResult->data.dm_info.node_info_p->dm_path_p);
               pUCMResult->data.dm_info.node_info_p->dm_path_p = NULL;
            }
            if ((sub_command_id == CM_CMD_GET_DM_FIRST_NODE_INFO) ||
                  (sub_command_id == CM_CMD_GET_DM_NEXT_NODE_INFO) ||
                  (sub_command_id == CM_CMD_GET_DM_NEXT_TABLE_ANCHOR_NODE_INFO))

            {
               if (pUCMResult->data.dm_info.opaque_info_p)
               {
                  of_free (pUCMResult->data.dm_info.opaque_info_p);
                  pUCMResult->data.dm_info.opaque_info_p = NULL;
               }
            }
            if(pUCMResult->data.dm_info.node_info_p)
            {
               of_free (pUCMResult->data.dm_info.node_info_p);
               pUCMResult->data.dm_info.node_info_p = NULL;
            }
         }
         break;

      case CM_CMD_GET_DM_NODE_AND_CHILD_INFO:
         /* Free Node And Child Info */
         if (pUCMResult->result_code == CM_JE_SUCCESS)
         {
            cm_je_free_nodeinfo_struct (pUCMResult->data.dm_info.count_ui,
                  pUCMResult->data.dm_info.node_info_p);

            if (pUCMResult->data.dm_info.node_info_array_p)
            {
               cm_je_free_nodeinfo_struct (pUCMResult->data.dm_info.
                     node_info_array_p->count_ui,
                     pUCMResult->data.dm_info.
                     node_info_array_p->struct_arr_p);
               of_free (pUCMResult->data.dm_info.node_info_array_p);
            }

         }
         break;
      case CM_CMD_GET_DM_KEY_CHILDS:
         if (pUCMResult->result_code == CM_JE_SUCCESS)
         {
            if (pUCMResult->data.dm_info.node_info_array_p)
            {
               cm_je_free_nodeinfo_struct (pUCMResult->data.dm_info.
                     node_info_array_p->count_ui,
                     pUCMResult->data.dm_info.
                     node_info_array_p->struct_arr_p);
               of_free (pUCMResult->data.dm_info.node_info_array_p);
            }
         }
         break;
      case CM_CMD_GET_DM_KEYS_ARRAY:
         if (pUCMResult->result_code == CM_JE_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Freeing Get Response NV Pairs");
            if (pUCMResult->data.dm_info.dm_node_nv_pairs_p)
            {
               cm_je_free_ptr_nvpairArray (pUCMResult->data.dm_info.dm_node_nv_pairs_p);
            }
         }
         break;
      case CM_CMD_GET_DM_INSTANCE_MAPLIST_BY_NAMEPATH:
         if (pUCMResult->result_code == CM_JE_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Freeing Instance Map Keys ");
            if (pUCMResult->data.dm_info.instance_map_list_p)
            {
               for (ii = 0; ii < pUCMResult->data.dm_info.count_ui; ii++)
               {
                  of_free (pUCMResult->data.dm_info.instance_map_list_p[ii].key_p);
               }
               of_free (pUCMResult->data.dm_info.instance_map_list_p);
               pUCMResult->data.dm_info.instance_map_list_p = NULL;
            }
         }
         break;

      case CM_CMD_GET_DM_CHILD_NODE_COUNT:
      case CM_CMD_GET_NEW_DM_INSTANCE_ID:
      case CM_CMD_ADD_DM_INSTANCE_MAP:
      case CM_CMD_DEL_DM_INSTANCE_MAP:
         break;
      case CM_CMD_GET_DM_NODE_ROLE_ATTRIBS:
      case CM_CMD_GET_DM_INSTANCE_ROLE_ATTRIBS:
         if (pUCMResult->result_code == CM_JE_SUCCESS)
         {
            role_perm_ary_p= (struct cm_dm_array_of_role_permissions *)pUCMResult->data.dm_info.opaque_info_p;
            of_free(role_perm_ary_p->role_permissions);
            of_free(role_perm_ary_p);
         }
         break;
      case CM_CMD_SET_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_GET_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_DEL_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_GET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_SET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
      case CM_CMD_DEL_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
         // Free    pUCMResult->data.data_modelGet.count_ui:
         break;
   }
   if (pUCMResult->result_code == CM_JE_FAILURE)
   {
      if(pUCMResult->result.error.dm_path_p)
         of_free (pUCMResult->result.error.dm_path_p);
   }
   of_free (pUCMResult);
   return OF_SUCCESS;
}

/******************************************************************************
 * Function Name : ucmCliEstablishTransportChannel
 * Description   : This API is used to establish a transport channel with
 *                  the UCM JE.
 * Input params  : NONE
 * Output params : NONE
 * Return value  : Pointer to Transport channel on successful creation
 *    of Transport channel.  NULL in any failure.
 ******************************************************************************/
void *cmje_establish_tnsprtchannel_with_ldsv (void)
{
   struct cm_tnsprt_channel *tnsprt_channel_p;

   tnsprt_channel_p =
      (struct cm_tnsprt_channel *) cm_je_create_send_tnsprtchannel (CM_IPPROTO_TCP, 0,
            CMLDSV_CONFIGREQUEST_PORT);
   if (!tnsprt_channel_p)
   {
      CM_JE_DEBUG_PRINT ("cm_je_create_send_tnsprtchannel failed");
      return NULL;
   }
   return ((void *) tnsprt_channel_p);
}

/******************************************************************************
 * Function Name : ucmCliTarminateTransportChannel
 * Description   : This API is used to close a Transport channel.
 * Input params  : tnsprt_channel_p - pointer to transport channel node.
 * Output params : NONE
 * Return value  : NONE
 *****************************************************************************/
void cmje_terminate_tnsprtchannel_with_ldsv (struct cm_tnsprt_channel *
      tnsprt_channel_p)
{
   if (tnsprt_channel_p)
   {
      cm_je_close_send_tnsprtchannel (tnsprt_channel_p);
   }
}

/**
  \ingroup TRCHNLCAPI
  This API is used to create a Transport Channel. The transport channel is a secure
  TCP socket connection. This API will create a Socket and connects to the specified
  Server Port.

  <b>Input Parameters: </b>\n
  <b><i>trans_protocol_ui:</i></b> Specifies the Transport Protocol using which the 
  Socket will be created. By default it's TCP. 

  <b><i>src_port_ui:</i></b> Specifies the Source Port. \n

  <b><i>dest_port_ui:</i></b> Specifies the Destination Port. Using this port the Socket 
  will get connected to the other end.\n

  <b>Output Parameters:</b> NONE \n

  <b>Return Value:</b> Pointer to the Transport channel structure on successfull 
  creation of socket and connecting to the Server.\n

  Null pointer on failure.\n
  */
/******************************************************************************
 * Function Name : cm_je_create_send_tnsprtchannel
 * Description   : This API is used to create a Transport Channel
 * Input params  : trans_protocol_ui - Transport Protocol
 *                 src_port_ui - Source Port
 *                 dest_port_ui - Destination Port
 * Output params : NONE
 * Return value  : Return pointer to Transport channel on successful creation of
 *                 socket and connecting to the Server. 
 *                 NULL Pointer on failure.
 *****************************************************************************/
 void *cm_je_create_send_tnsprtchannel (uint32_t trans_protocol_ui,
      uint16_t src_port_ui,
      uint16_t dest_port_ui)
{
   struct cm_tnsprt_channel *tnsprt_channel_p = NULL;
   int32_t no_of_retries = 0;

   /* Allocate transport channel node */
   tnsprt_channel_p =
      (struct cm_tnsprt_channel *) of_calloc (1, sizeof (struct cm_tnsprt_channel));
   if (!tnsprt_channel_p)
   {
      CM_JE_DEBUG_PRINT ("UCMTransportChannelAlloc failed");
      return NULL;
   }

   /* Create Socket */
   tnsprt_channel_p->sock_fd_i = cm_socket_create (trans_protocol_ui);
   if (tnsprt_channel_p->sock_fd_i == OF_FAILURE)
   {
      perror ("Socket Create Failed");
      CM_JE_DEBUG_PRINT ("cm_socket_create failed");
      of_free (tnsprt_channel_p);
      return NULL;
   }
   CM_JE_DEBUG_PRINT ("cm_socket_created %d", tnsprt_channel_p->sock_fd_i);

   if (src_port_ui)
   {
      if ((cm_socket_bind (tnsprt_channel_p->sock_fd_i, CM_LOOPBACK_ADDR,
                  src_port_ui)) == OF_FAILURE)
      {
         perror ("Failed to bind to Port");
         CM_JE_DEBUG_PRINT ("Failed to bind to Port(%d)", src_port_ui);
         cm_socket_close (tnsprt_channel_p->sock_fd_i);
         of_free (tnsprt_channel_p);
         return NULL;
      }
   }

   /* Connect to server */
   if (cm_socket_connect (tnsprt_channel_p->sock_fd_i, CM_LOOPBACK_ADDR,
            dest_port_ui) == OF_FAILURE)
   {
      CM_JE_DEBUG_PRINT ("Failed to Connect to Loopback address on \
            Port(%d) ", dest_port_ui);

      while (no_of_retries <= CM_SOCK_CONNET_MAX_RETRIES)
      {
         of_sleep_ms (1);
         if (cm_socket_connect (tnsprt_channel_p->sock_fd_i, CM_LOOPBACK_ADDR,
                  dest_port_ui) == OF_FAILURE)
         {
            no_of_retries++;
            CM_JE_DEBUG_PRINT ("Failed to Connect to Loopback address on \
                  Port(%d) Retry Count=%d", dest_port_ui, no_of_retries);
            if (no_of_retries < CM_SOCK_CONNET_MAX_RETRIES)
            {
               continue;
            }
            cm_socket_close (tnsprt_channel_p->sock_fd_i);
            of_free (tnsprt_channel_p);
            return NULL;
         }
      }
   }
   return ((void *) tnsprt_channel_p);
}

/**
  \ingroup TRCHNLCAPI
  This API is used to close the Transport Channel. This will close the connection
  with the Server and closes the Socket. Frees up the memory allocated for the
  transport channel node.

  <b>Input Parameters: </b>\n
  <b><i>tnsprt_channel_p:</i></b> Pointer to the Transport channel node.

  <b>Output Parameters:</b> NONE \n

  <b>Return Value:</b> NONE\n

*/

/******************************************************************************
 * Function Name : cmje_close_recv_tnsprtchannel
 * Description   : This API is used to close the Transport connection with the
 *                 Server and frees the Transport channel node.
 * Input params  : tnsprt_channel_p - pointer to the Transport channel structure.
 * Output params : NONE
 * Return value  : NONE
 *****************************************************************************/
 void cm_je_close_send_tnsprtchannel (struct cm_tnsprt_channel * tnsprt_channel_p)
{
   if (!tnsprt_channel_p)
   {
      CM_JE_DEBUG_PRINT ("Invalid input");
      return;
   }

   cm_socket_close (tnsprt_channel_p->sock_fd_i);
   of_free (tnsprt_channel_p);
}

void cm_je_ldsv_init (void)
{
   struct je_config_request *config_request_p = NULL;
   struct cm_result *result_p = NULL;

   CM_JE_DEBUG_PRINT ("Entered");

#if 0
   pDMRootNode = (struct cm_dm_data_element *)cm_dm_get_root_node();
   /* Application Callbacks */
   if (!(pDMRootNode))
   {
      CM_JE_DEBUG_PRINT ("DM Node is NULL");
      error_code=UCMJE_ERROR_GETNODE_BYDMPATH;
      break;
   }
   if (!(pDMRootNode->app_cbks_p))
   {
      CM_JE_DEBUG_PRINT ("Application Callbacks are NULL");
      error_code=UCMJE_ERROR_APPCBK_NULL;
      break;
   }
   pGetLoadCfgStatusFn = pDMNode->app_cbks_p->UCMConfigGetLoadCfgStatus;
   if (!pGetLoadCfgStatusFn)
   {
      CM_JE_DEBUG_PRINT ("pGetLoadCfgStatusFn Callback is NULL");
      error_code= UCMJE_ERROR_GETFIRSTCBK_NULL;
      break;                                                 
   }

   error_code = pGetLoadCfgStatusFn (&status_b)
      if (error_code != OF_SUCCESS)
         error_code=UCMJE_ERROR_GETFIRSTCBK_FAILED;

   if(status_b)
   {
      /* Seems to be UCM died and came up again*/
      CM_JE_DEBUG_PRINT("Configuration already loaded at Security Applications");
      bLdsvInitialized_g=TRUE;
   }
   else
   {
      bLdsvInitialized_g=FALSE;
   }

   if(bLdsvInitialized_g)
   {
      /* Prepare Data Model Instance Tree by calling Get First, GetNext Records
       * from Security Applications */
      ucmJEPrepareInstanceTree(NULL);
   }
   else
   {
      /* Calls LDSV Handler to initialize factory defaults*/
   }
#endif
   /* Allocate Memory for Configuration Request Structure */
   config_request_p = (struct je_config_request *) cm_je_cfgrequest_alloc ();
   if (!config_request_p)
   {
      CM_JE_DEBUG_PRINT ("Transport Channel Memory Allocation Failed");
      return;
   }

   of_strncpy(config_request_p->admin_role, CM_VORTIQA_SUPERUSER_ROLE,UCMDM_ADMIN_ROLE_MAX_LEN);
   config_request_p->command_id = CM_COMMAND_LDSV_REQUEST;
   config_request_p->sub_command_id = CM_CMD_LOAD_CONFIG;
   config_request_p->dm_path_p = (char*) of_calloc(1,of_strlen(CM_DM_ROOT_NODE_PATH)+1);
   of_strncpy(config_request_p->dm_path_p,CM_DM_ROOT_NODE_PATH,of_strlen(CM_DM_ROOT_NODE_PATH));

   if (cm_je_ldsv_handler (config_request_p, &result_p) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("cm_je_ldsv_handler Failed");
   }

   if (result_p)
   {
      cm_je_free_ucmresult (config_request_p->command_id, config_request_p->sub_command_id,
            result_p);
   }
   if (config_request_p)
   {
      cm_je_free_config_request (config_request_p);
   }
}



int32_t cmje_process_pending_request (void )
{
   struct je_request_node *pJEPendingRequest;
   struct cmje_tnsprtchannel_info *tnsprt_channel_p = NULL;
   struct je_config_request *config_request_p = NULL;
   struct cm_msg_generic_header GenericHdr;
   char *pResponseBuf = NULL;
   struct cm_result *pUCMResult = NULL;
   UCMDllQNode_t *pDllQNode, *pTmpNode;
   uint32_t return_value = 0;
   uint32_t uiBuffLen = 0;

   CM_JE_DEBUG_PRINT("Entered");
   CM_DLLQ_DYN_SCAN (&cm_je_request_queue_g, pDllQNode,pTmpNode, UCMDllQNode_t *)
   {
      pJEPendingRequest =
         CM_DLLQ_LIST_MEMBER (pDllQNode, struct je_request_node *, list_node);
      if(pJEPendingRequest != NULL)
      {
         /* Extract Transport Channel from Global List */
         tnsprt_channel_p = cmje_get_tnsprtchannel_from_list (pJEPendingRequest->fd_ui);
         if (!tnsprt_channel_p)
         {
            CM_JE_DEBUG_PRINT ("Invalid Transport Channel");
            return OF_FAILURE;
         }
         config_request_p=pJEPendingRequest->config_request_p;
         pUCMResult=pJEPendingRequest->result_p;
         switch (config_request_p->command_id)
         {
            case CM_COMMAND_LDSV_REQUEST:
               if(config_request_p->sub_command_id == CM_CMD_SAVE_CONFIG)
               {
                  if((config_request_p->mgmt_engine_id == CM_CLI_MGMT_ENGINE_ID) ||
                        (config_request_p->mgmt_engine_id ==  CM_HTTP_MGMT_ENGINE_ID))
                  {
                     CM_JE_DEBUG_PRINT("Save Command completed or failed");
                     break;
                  }
               }
               /* save config is the only command handled in this function */
               return OF_FAILURE;
               break;
            default:
               return OF_FAILURE;
         }

         /* Preperation of RESPOSNSE */
         of_memcpy (&GenericHdr, config_request_p, uiGenHdrLen_g);
         uiBuffLen += uiGenHdrLen_g;

         /*Fill Response Structure */
         cm_je_get_response_message_length (config_request_p, pUCMResult, &uiBuffLen);
         CM_JE_DEBUG_PRINT("buffLen=%d",uiBuffLen);

         pResponseBuf = (char *) of_calloc (1, uiBuffLen+1);
         if (!pResponseBuf)
         {
            CM_JE_DEBUG_PRINT ("Response Buffer Memory Allocation Failed");
            of_free (tnsprt_channel_p->recv_buf_p);
            cm_je_free_ucmresult (config_request_p->command_id, config_request_p->sub_command_id,
                  pUCMResult);
            cm_je_free_config_request (config_request_p);
            return OF_FAILURE;
         }

         /*Fill Socket Response Buffer */
         pUCMResult->sub_command_id = config_request_p->sub_command_id;
         cm_je_prepare_response_message (&GenericHdr, pUCMResult, pResponseBuf,
               uiBuffLen);
         if ((return_value =
                  cm_socket_send (tnsprt_channel_p->sock_fd_ui, pResponseBuf, uiBuffLen,
                     0)) <= 0)
         {
            perror ("JE-SocketSend:");
            CM_JE_DEBUG_PRINT ("SocketSend Failed with errno = %d RetVal=%d", errno,
                  return_value);
            of_free (pResponseBuf);
            of_free (tnsprt_channel_p->recv_buf_p);
            cm_je_free_ucmresult (config_request_p->command_id, config_request_p->sub_command_id,
                  pUCMResult);
            cm_je_free_config_request (config_request_p);
            return OF_FAILURE;
         }
         CM_INC_STATS_JE (JE_RESPONSE_SENDS);
         CM_JE_DEBUG_PRINT ("Response Message of %d bytes succesfuly sent",
               uiBuffLen);
         of_free (pResponseBuf);
         cm_je_free_ucmresult (config_request_p->command_id, config_request_p->sub_command_id,
               pUCMResult);

         cm_je_free_config_request (config_request_p);
         CM_DLLQ_DELETE_NODE (&cm_je_request_queue_g, pDllQNode);
         of_free(pJEPendingRequest);
      }  
   }
   return OF_SUCCESS;
}

 int32_t cm_je_get_errorinfo_length (struct cm_result * result_p,
      uint32_t * puiLen)
{
   uint32_t uiErrorLen = 0;
   uint32_t uiNvPairLen = 0;

   /* for  cm_resp_msg_p->result.error.error_code */
   uiErrorLen += uiOSIntSize_g;

   /* for Result string length */
   uiErrorLen += uiOSIntSize_g;

   /* copy  Result String  */
   if (result_p->result.error.result_string_p)
   {
      uiErrorLen += of_strlen (result_p->result.error.result_string_p);
   }

   /* For Dm Path Length  */
   uiErrorLen += uiOSIntSize_g;
   /* copy  DM Path Length  */
   if (result_p->result.error.dm_path_p)
   {
      uiErrorLen += of_strlen (result_p->result.error.dm_path_p);
   }

   /* for nv_pair_b */
   uiErrorLen += uiOSIntSize_g;

   /* Copy Error Name Value Pair */
   if (result_p->result.error.nv_pair_b)
   {
      CM_JE_GET_NVPAIR_LEN (result_p->result.error.nv_pair, uiNvPairLen);
      uiErrorLen += uiNvPairLen;
   }
   CM_JE_DEBUG_PRINT ("Error Length = %d", uiErrorLen);
   *puiLen += uiErrorLen;

   return OF_SUCCESS;
}

 int32_t cm_je_copy_errorinfo_to_buffer (char * pUCMTempRespBuf,
      struct cm_result * cm_resp_msg_p,
      uint32_t * puiLen)
{
   uint32_t uiLen = 0, uiDMPathLen = 0, uiStrLen = 0;

   pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf,
         cm_resp_msg_p->result.error.error_code);
   //  uiLen += uiInt32Size_g;
   uiLen += uiOSIntSize_g;

   if (cm_resp_msg_p->result.error.result_string_p)
   {
      uiStrLen = of_strlen (cm_resp_msg_p->result.error.result_string_p);
      pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf, uiStrLen);
      //    uiLen += uiInt32Size_g;
      uiLen += uiOSIntSize_g;
      /* Copy Result Message */
      of_memcpy (pUCMTempRespBuf, cm_resp_msg_p->result.error.result_string_p,
            uiStrLen);
      pUCMTempRespBuf += uiStrLen;
      uiLen += uiStrLen;
   }
   else
   {
      pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf, 0);
      //uiLen += uiInt32Size_g;
      uiLen += uiOSIntSize_g;
   }
   if (cm_resp_msg_p->result.error.dm_path_p)
   {
      /* copy  DM Path Length  */
      uiDMPathLen = of_strlen (cm_resp_msg_p->result.error.dm_path_p);
      pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf, uiDMPathLen);
      //uiLen += uiInt32Size_g;
      uiLen += uiOSIntSize_g;

      /* copy DM Path */
      of_memcpy (pUCMTempRespBuf, cm_resp_msg_p->result.error.dm_path_p, uiDMPathLen);
      pUCMTempRespBuf += uiDMPathLen;
      uiLen += uiDMPathLen;
   }
   else
   {
      /* copy  DM Path Length  */
      pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf, 0);
      // uiLen += uiInt32Size_g;
      uiLen += uiOSIntSize_g;
   }

   /* Copy Error Name Value Pair */
   /* Copy nv_pair_b */
   pUCMTempRespBuf = (char *)of_mbuf_put_32 (pUCMTempRespBuf,
         cm_resp_msg_p->result.error.nv_pair_b);
   //uiLen += uiInt32Size_g;
   uiLen += uiOSIntSize_g;

   *puiLen = uiLen;

   uiLen = 0;
   if (cm_resp_msg_p->result.error.nv_pair_b)
   {
      if (cm_je_copy_nvpair_to_buffer
            (&(cm_resp_msg_p->result.error.nv_pair), pUCMTempRespBuf,
             &uiLen) != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("Copy Name Value Pairs into Buffer Failed");
         return OF_FAILURE;
      }
   }

   *puiLen += uiLen;
   return OF_SUCCESS;
}
#endif /* CM_JE_SUPPORT */
#endif /* CM_SUPPORT */
