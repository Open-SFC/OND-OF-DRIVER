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
 * File name: jedaemon.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/23/2013
 * Description: 
*/

#ifdef CM_SUPPORT
#ifdef CM_JE_SUPPORT

#include "jeincld.h"
#include "cmsock.h"
#include "jeldef.h"
#include "jelif.h"
#include "netinet/in.h"

/******************************************************************************
 * * * * * * * * * * * * * * * GLOBAL VARIABLES* * * * * * * * * * * * * * * * 
 *****************************************************************************/
extern cm_sock_handle je_listen_sock_g;
extern UCMDllQ_t cm_je_tnsprtchannel_list_g;

/******************************************************************************
 * * * * * * * * * * * * * * * FUNCTION PROTOTYPES * * * * * * * * * * * * * * 
 *****************************************************************************/
 int32_t cm_je_set_poll_fdlist (struct cm_poll_info * je_fds_p);

/******************************************************************************
 * * * * * * * * * * * * * FUNCTION DEFINITIONS * * * * * * * * * * * * * * * * 
 *****************************************************************************/
/**
 \ingroup JEAPI
  This API runs Daemon and and reads requests from Management Engines.
 
  <b>Input paramameters: </b> None\n
  <b>Output Parameters: </b> None\n
  <b>Return Value:</b> OF_SUCCESS in Success
    OF_FAILURE in Failure case.\n
 */
/******************************************************************************
 ** Function Name : cm_je_daemon
 ** Description   : This API runs Daemon and reads requests from Management Engines
 ** Input params  : None
 ** Output params : None
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t cm_je_daemon (void)
{
  struct cm_poll_info cm_je_fd_list_g[UCMJE_FDPOLL_MAXFDS];
  uint32_t ulFdPollRetVal;
  uint32_t ulMaxFds = 0;
  uint32_t ulFd = 0;
  time_t TimePollStart, TimePollEnd;


  CM_JE_DEBUG_PRINT ("Entered");

  TimePollStart = of_time (&TimePollStart);
  while (TRUE)
  {
    ulMaxFds = cm_je_set_poll_fdlist (cm_je_fd_list_g);
    if (ulMaxFds == 0)
    {
      CM_JE_DEBUG_PRINT ("MaxFds returned zero");
    }
    ulFdPollRetVal =
      cm_fd_poll (ulMaxFds, cm_je_fd_list_g, (UCMJE_FDPOLL_LEAST_POLL_TIME * 1000));
    for (ulFd = 0; (ulFd < ulMaxFds) && (ulFdPollRetVal > 0); ulFd++)
    {
      if ((cm_je_fd_list_g[ulFd].returned_events & CM_FDPOLL_READ)
          && (cm_je_fd_list_g[ulFd].fd_i == je_listen_sock_g))
      {
        cmje_create_recv_tnsprtchannel (cm_je_fd_list_g[ulFd].fd_i);
      }
      else if (cm_je_fd_list_g[ulFd].returned_events & CM_FDPOLL_READ)
      {
#ifdef CM_JE_DEBUG
        printf ("\r\n");
#endif
        CM_JE_DEBUG_PRINT ("*=*=*=*=*=* NEW REQUEST ON FD%d *=*=*=*=*=*",
                            cm_je_fd_list_g[ulFd].fd_i);
        cm_je_read_and_process_config_request (cm_je_fd_list_g[ulFd].fd_i);
      }
		}                           /* End for */
		TimePollEnd = of_time (&TimePollEnd);
		if(((TimePollEnd - TimePollStart)/60) > UCMJE_FDPOLL_LEAST_POLL_TIME )
		{
			 cm_timer_process(UCMJE_FDPOLL_LEAST_POLL_TIME);
		 TimePollStart=TimePollEnd;
		}
  }                             /* End While */
  return OF_FAILURE;
}

/**
 \ingroup JEAPI
  This API Creates a TCP socket, bind on known port and starts listening on the port.
 
  <b>Input paramameters: </b> None\n
  <b>Output Parameters: </b> None\n
  <b>Return Value:</b> OF_SUCCESS in Success
    OF_FAILURE in Failure case.\n
 */
/******************************************************************************
 ** Function Name : cm_je_create_listen_socket
 ** Description   : This API Creates a TCP socket, bind on known port and starts
 **                  Listening
 ** Input params  : None
 ** Output params : None
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t cm_je_create_listen_socket (void)
{
  int32_t iListenFd = 0;

  if ((iListenFd = cm_socket_create (CM_IPPROTO_TCP)) == OF_FAILURE)
  {
    CM_JE_DEBUG_PRINT ("Error :Unable to Create SyncSign Tcp socket");
    return OF_FAILURE;
  }

  cm_set_sock_reuse_opt(iListenFd); 

  if ((cm_socket_bind (iListenFd, INADDR_ANY,
                      UCMJE_CONFIGREQUEST_PORT)) == OF_FAILURE)
  {
    perror ("Error in Binding:");
    CM_JE_DEBUG_PRINT ("Error in Binding");
    cm_socket_close (iListenFd);
    return OF_FAILURE;
  }

  if (cm_socket_listen (iListenFd, 2) != OF_SUCCESS)
  {
    CM_JE_DEBUG_PRINT ("TCP SERVER SocketListen failure");
    cm_socket_close (iListenFd);
    return OF_FAILURE;
  }

  CM_JE_DEBUG_PRINT ("Listen socket succesfully created");
  return iListenFd;
}

/**
 \ingroup JEAPI
  This API Gets all accepted socket Fds  from management Engines by JE for polling.
  <b>Input paramameters: </b> None\n
  <b>Output Parameters: </b> \n
   <b><i>je_fds_p </i></b> je_fds_p - Pointer to Poll Info structure
  <b>Return Value:</b> OF_SUCCESS in Success
    OF_FAILURE in Failure case.\n
 */
/******************************************************************************
 ** Function Name : cm_je_create_listen_socket
 ** Description   : This API Gets all Accepted Socket Fds from Management Engines
 **                  for polling by JE Daemon.
 ** Input  params : None
 ** Output params : je_fds_p - Pointer to Poll Info structure
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
 int32_t cm_je_set_poll_fdlist (struct cm_poll_info * je_fds_p)
{
  uint32_t ulFd = 0;
  struct cmje_tnsprtchannel_info *pTrChannel = NULL;

  if (je_listen_sock_g == 0)
  {
    return (ulFd);
  }
  CM_FDPOLL_ZERO (je_fds_p, UCMJE_FDPOLL_MAXFDS);
  je_fds_p[ulFd].fd_i = je_listen_sock_g;
  je_fds_p[ulFd].interested_events = CM_FDPOLL_READ;
  je_fds_p[ulFd].returned_events = 0;
  ulFd++;

  if ((CM_DLLQ_COUNT (&cm_je_tnsprtchannel_list_g)) > 0)
  {
    CM_DLLQ_SCAN (&cm_je_tnsprtchannel_list_g, pTrChannel, struct cmje_tnsprtchannel_info *)
    {
      if (pTrChannel)
      {
        je_fds_p[ulFd].fd_i = pTrChannel->sock_fd_ui;
        je_fds_p[ulFd].interested_events = CM_FDPOLL_READ;
        je_fds_p[ulFd].returned_events = 0;
        ulFd++;
      }
    }
  }
  return (ulFd);
}

#endif /*CM_JE_SUPPORT */
#endif /*CM_SUPPORT */
