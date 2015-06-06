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
 * File name:   ldsvdmn.c
 * Author:      Freescale Semiconductor    
 * Date:        06/12/2013
 * Description: LDSV Deamon API
*/

#ifdef CM_LDSV_SUPPORT

#include "ldsvinc.h"
#include "cmsock.h"
#include "ldsvldef.h"
#include "ldsvlif.h"
#include "errno.h"
#include "netinet/in.h"

extern cm_sock_handle ulLDSVListenSock_g;
cm_sock_handle ulSocketFd_g = 0;
uint64_t ulVersionNo_g;

int32_t cm_ldsv_accept_connection (cm_sock_handle fd_ui);
int32_t cm_ldsv_read_and_process_config_request (cm_sock_handle fd_ui);
int32_t cm_ldsv_handler (cm_ldsv_config_request_t * pUCMCfgReq, uint64_t iVersionNo);
void cm_ldsv_clean_config_request(cm_ldsv_config_request_t *pUCMCfgRequest);

/**************************************************************************
 * Function Name :cm_ldsv_daemon
 * Input Args    : none
 * Output Args   :none
 * Description   : LDSV Daemon is a user space daemon that waits on a loopback
 *        TCP port for “SAVE” and “LOAD” requests from UCM Job execution 
 *        engine. Load requests can be given by any security application 
 *        using the API provided by LDSV Daemon.
 * Return Values : Successful Daemon start returns OF_SUCCESS, if any failure 
 *         occured, returns OF_FAILURE
 ***************************************************************************/

int32_t cm_ldsv_daemon (void)
{

  uint32_t ulFdPollRetVal;
  uint32_t ulMaxFds = 0;
  uint32_t ulFd = 0;
  struct cm_poll_info cmLdsvFDList[CM_LDSV_FDPOLL_MAXFDS];

  CM_LDSV_DEBUG_PRINT ("Entered into LDSV Daemon \n");
  while (TRUE)
  {
    memset (cmLdsvFDList, 0,
            (sizeof (struct cm_poll_info) * CM_LDSV_FDPOLL_MAXFDS));
    ulMaxFds = cm_ldsv_set_poll_fd_list (cmLdsvFDList);
    if (ulMaxFds == 0)
    {
      CM_LDSV_DEBUG_PRINT ("MaxFds returned zero");
    }
    ulFdPollRetVal =
      cm_fd_poll (ulMaxFds, cmLdsvFDList, CM_LDSV_FDPOLL_LEAST_POLL_TIME);
    for (ulFd = 0; (ulFdPollRetVal > 0) && (ulFd < ulMaxFds); ulFd++)
    {
      if ((cmLdsvFDList[ulFd].returned_events & CM_FDPOLL_READ)
          && (cmLdsvFDList[ulFd].fd_i == ulLDSVListenSock_g))
      {
        cm_ldsv_accept_connection (cmLdsvFDList[ulFd].fd_i);
      }
      else if ((cmLdsvFDList[ulFd].returned_events & CM_FDPOLL_READ)
               && (cmLdsvFDList[ulFd].fd_i == ulSocketFd_g))
      {
        cm_ldsv_read_and_process_config_request (cmLdsvFDList[ulFd].fd_i);
      }
    }                           /* End for */
  }                             /* End While */
}

/**************************************************************************
 * Function Name : cm_ldsv_create_listen_socket
 * Input Args    : none
 * Output Args   : none
 * Description   : This method is used to create ldsv socket for listening 
 *        and processing the commands
 * Return Values : Successful socket creation returns OF_SUCCESS, if any failure
 *                  occured, returns OF_FAILURE
 ****************************************************************************/

int32_t cm_ldsv_create_listen_socket (void)
{
  int32_t iListenFd = 0;

  if ((iListenFd = cm_socket_create (CM_IPPROTO_TCP)) == OF_FAILURE)
  {
    CM_LDSV_DEBUG_PRINT ("Error :Unable to Create Tcp socket");
    return OF_FAILURE;
  }

  if ((cm_socket_bind (iListenFd, INADDR_ANY,
                      CMLDSV_CONFIGREQUEST_PORT)) == OF_FAILURE)
  {
    CM_LDSV_DEBUG_PRINT ("Error in Binding");
    cm_socket_close (iListenFd);
    return OF_FAILURE;
  }

  if (cm_socket_listen (iListenFd, 2) != OF_SUCCESS)
  {
    CM_LDSV_DEBUG_PRINT ("TCP SERVER SocketListen failure");
    cm_socket_close (iListenFd);
    return OF_FAILURE;
  }

  CM_LDSV_DEBUG_PRINT ("Listen socket succesfully created %d", iListenFd);
  return iListenFd;
}

/**************************************************************************
 * Function Name : cm_ldsv_set_poll_fd_list
 * Input Args    : 
 *      pSigDwnDfds : poll fd list
 * Output Args   : none
 * Description   : This method is used to set poll fd list
 * Return Values : returns poll fd if found in the global list, otherwise null.
 ******************************************************************************/

int32_t cm_ldsv_set_poll_fd_list (struct cm_poll_info * pSigDwnDFds)
{
  uint32_t ulFd = 0;

  if (ulLDSVListenSock_g == 0)
  {
    return (ulFd);
  }
  CM_FDPOLL_ZERO (pSigDwnDFds, CM_LDSV_FDPOLL_MAXFDS);
  pSigDwnDFds[ulFd].fd_i = ulLDSVListenSock_g;
  pSigDwnDFds[ulFd].interested_events = CM_FDPOLL_READ;
  pSigDwnDFds[ulFd].returned_events = 0;
  ulFd++;

  if (ulSocketFd_g != 0)
  {
    pSigDwnDFds[ulFd].fd_i = ulSocketFd_g;
    pSigDwnDFds[ulFd].interested_events = CM_FDPOLL_READ;
    pSigDwnDFds[ulFd].returned_events = 0;
    ulFd++;
  }
  return (ulFd);
}

/**************************************************************************
 * Function Name :cm_ldsv_accept_connection 
 * Input Args    : 
 *      fd_ui : fd list to handle socket calls
 * Output Args   : none
 * Description   : This method is used to accept socket calls
 * Return Values : returns success/failure
 ******************************************************************************/
int32_t cm_ldsv_accept_connection (cm_sock_handle fd_ui)
{
  cm_sock_handle uiNewFd = 0;
  uint32_t PeerIp = 0;
  uint16_t uiPeerPort = 0;
  char aAddr[16] = { };
  
  if (ulSocketFd_g != 0)
  {
    CM_LDSV_DEBUG_PRINT("Last Request is in process. ");
    ulSocketFd_g = 0;
    return OF_FAILURE;
  }
  /* Accept Fd */
  uiNewFd = cm_socket_accept (fd_ui, &PeerIp, &uiPeerPort);
  if (uiNewFd < 0)
  {
    CM_LDSV_DEBUG_PRINT ("Socket Accept Failed");
    ulSocketFd_g = 0;
    return OF_FAILURE;
  }

  cm_inet_ntoal (PeerIp, aAddr);
  CM_LDSV_DEBUG_PRINT ("socket recived from Ip Address= %s Port= %d ", aAddr,
                        uiPeerPort);
  ulSocketFd_g = uiNewFd;
  return OF_SUCCESS;
}

/**************************************************************************
 * Function Name : cm_ldsv_read_and_process_config_request
 * Input Args    : 
 *      fd_ui : fd list to handle socket calls
 * Output Args   : none
 * Description   : This method is used to read and process configuration requests from je
 * Return Values : returns success/failure
 ******************************************************************************/
int32_t cm_ldsv_read_and_process_config_request (cm_sock_handle fd_ui)
{
  cm_ldsv_config_request_t *pUCMCfgRequest = NULL;
  int32_t iBytesRecd = 0;
  uint32_t ulLen;
  char aRecvBuf[4] = { };
  char *pTmpBuff;
  int32_t iBytesToRead;
  char *pRecvBuf;
  uint64_t iVersionNo;

  if ((iBytesRecd = cm_socket_recv (fd_ui, aRecvBuf, CM_UINT_OS_SIZE, MSG_PEEK)) <= 0)
  {
    ulSocketFd_g = 0;
    return OF_FAILURE;
  }

  if (iBytesRecd < CM_UINT_OS_SIZE)
  {
    ulSocketFd_g = 0;
    return OF_FAILURE;
  }

  iBytesToRead = of_mbuf_get_32 (aRecvBuf);
  pRecvBuf = (char *) of_calloc (1, iBytesToRead);
  if (!pRecvBuf)
  {
    CM_LDSV_DEBUG_PRINT ("Memory Allocation Failed");
    ulSocketFd_g = 0;
    return OF_FAILURE;
  }
  memset(pRecvBuf, 0, iBytesToRead);
  if ((iBytesRecd = cm_socket_recv (fd_ui, pRecvBuf, iBytesToRead, 0)) <= 0)
  {
    perror ("LDSV-SocketRecvFrom:");
    CM_LDSV_DEBUG_PRINT ("failed and return value = %d", errno);
    of_free(pRecvBuf);
    ulSocketFd_g = 0;
    return OF_FAILURE;
  }


  /* Allocate Memory for Configuration Request Structure */
  pUCMCfgRequest = (cm_ldsv_config_request_t *) of_calloc
    (1, sizeof (cm_ldsv_config_request_t));
  if (!pUCMCfgRequest)
  {
    CM_LDSV_DEBUG_PRINT ("Memory Allocation Failed");
    of_free(pRecvBuf);
    ulSocketFd_g = 0;
    return OF_FAILURE;
  }

  pTmpBuff = pRecvBuf;
  /* Length Field */
  ulLen = of_mbuf_get_32 (pTmpBuff);
  pTmpBuff += CM_UINT_OS_SIZE;

  CM_LDSV_DEBUG_PRINT ("Bytes recieved is %d socet Fd %d", iBytesRecd, fd_ui);

  pUCMCfgRequest->command_id = CM_COMMAND_LDSV_REQUEST;

  /* load/save/factorydefault */
  pUCMCfgRequest->sub_command_id = of_mbuf_get_32 (pTmpBuff);
  pTmpBuff += CM_UINT_OS_SIZE;

  /* DM Path Length */
  pUCMCfgRequest->iDMPathLength = of_mbuf_get_32 (pTmpBuff);
  pTmpBuff += CM_UINT_OS_SIZE;

  if (pUCMCfgRequest->iDMPathLength > 0)
  {
    pUCMCfgRequest->dm_path_p =
      (char *) of_calloc (1, pUCMCfgRequest->iDMPathLength + 1);
    if (pUCMCfgRequest->dm_path_p == NULL)
    {
      of_free(pUCMCfgRequest);
      of_free(pRecvBuf);
      ulSocketFd_g = 0;
      return OF_FAILURE;
    }
    memcpy (pUCMCfgRequest->dm_path_p, pTmpBuff, pUCMCfgRequest->iDMPathLength);
    pTmpBuff += pUCMCfgRequest->iDMPathLength; 
  }

  /* Version no */
  iVersionNo =  of_mbuf_get_64 (pTmpBuff);
  pTmpBuff += CM_UINT64_SIZE;

  switch (pUCMCfgRequest->command_id)
  {
    case CM_COMMAND_LDSV_REQUEST:
      if ((cm_ldsv_handler (pUCMCfgRequest, iVersionNo)) == OF_FAILURE)
      {
        cm_ldsv_clean_config_request(pUCMCfgRequest);
        cm_socket_close (fd_ui);
        ulSocketFd_g = 0;
        of_free(pRecvBuf);
        return OF_FAILURE;
      }
      break;
  }
  cm_socket_close (fd_ui);
  ulSocketFd_g = 0;
  cm_ldsv_clean_config_request(pUCMCfgRequest);
  of_free(pRecvBuf);
  return OF_SUCCESS;
}

/**************************************************************************
 * Function Name : cm_ldsv_clean_config_request 
 * Input Args    : 
 *          pUCMCfgRequest: pointer to configuration requsts
 * Output Args   : none
 * Description   : This method is used to free configuration requests
 * Return Values : none
 ******************************************************************************/
void cm_ldsv_clean_config_request(cm_ldsv_config_request_t *pUCMCfgRequest)
{
  if (pUCMCfgRequest == NULL)
  {
    return;
  }
  if (pUCMCfgRequest->dm_path_p)
    of_free(pUCMCfgRequest->dm_path_p);
  of_free(pUCMCfgRequest);
}

/**************************************************************************
 * Function Name : cm_ldsv_handler 
 * Input Args    : 
 *          pUCMCfgReq: pointer to configuration requsts
            iVersionNo : configuration version no
 * Output Args   : none
 * Description   : This method is used to handle configuration requests
 * Return Values : returns failure/success
 ******************************************************************************/

int32_t cm_ldsv_handler (cm_ldsv_config_request_t * pUCMCfgReq, uint64_t iVersionNo)
{
  uint32_t ulReqLength = 0;
  char *pTmdm_path_p;

  if (pUCMCfgReq->command_id != CM_COMMAND_LDSV_REQUEST)
  {
    CM_LDSV_DEBUG_PRINT ("Not a Ldsv  Request");
    return OF_FAILURE;
  }

  pTmdm_path_p = pUCMCfgReq->dm_path_p;
  if (pTmdm_path_p)
  {
    CM_LDSV_DEBUG_PRINT ("dm_path_p is %s", pUCMCfgReq->dm_path_p);
    ulReqLength += strlen (pUCMCfgReq->dm_path_p);
  }

  switch (pUCMCfgReq->sub_command_id)
  {
    case CM_CMD_LOAD_CONFIG:
      CM_LDSV_DEBUG_PRINT ("Load Config");
      if ((cm_load_config (pTmdm_path_p, FALSE)) == OF_FAILURE)
      {
        CM_LDSV_DEBUG_PRINT("Failed to load configuration.");
        return OF_FAILURE;
      }
      ulSocketFd_g = 0;
      break;
    case CM_CMD_SAVE_CONFIG:
      CM_LDSV_DEBUG_PRINT ("Save Config");
      if ((cm_save_config (pTmdm_path_p, iVersionNo)) == OF_FAILURE)
      {
        CM_LDSV_DEBUG_PRINT("Failed to Save configuration.");
        return OF_FAILURE;
      }
      ulSocketFd_g = 0;
      break;
    case CM_CMD_RESET_TO_FACTORY_DEFAULTS:
			ulVersionNo_g=iVersionNo;
      CM_LDSV_DEBUG_PRINT ("Factory defaults");
      if ((cm_factory_reset (pTmdm_path_p)) == OF_FAILURE)
      {
        CM_LDSV_DEBUG_PRINT("Failed to do Factory Reset.");
        return OF_FAILURE;
      }
      if ((cm_save_config (pTmdm_path_p, iVersionNo)) == OF_FAILURE)
      {
        CM_LDSV_DEBUG_PRINT("Failed to Save configuration.");
        return OF_FAILURE;
      }
      ulSocketFd_g = 0;
      break;
    case CM_CMD_FLUSH_CONFIG:
	break;
    default:
      CM_LDSV_DEBUG_PRINT ("unknown  Command");
      ulSocketFd_g = 0;
      return OF_FAILURE;
  }
  return OF_SUCCESS;
}

#endif /*CM_LDSV_SUPPORT */
