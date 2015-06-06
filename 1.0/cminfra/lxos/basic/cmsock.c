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
 * File name: ucmsock.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/23/2013
 * Description: This file contains socket specific calls. All functions
 *              in this file are wrapper function that in turn calls   
 *              underlying TCP/IP stack specific socket calls. All     
 *              IP address and port number that are specified are in   
 *              host byte order. If the target TCP/IP socket           
 *              implementation deals in network order then respective  
 *              functions in this file has to be changed to convert    
 *              from network byte order to host byte order and vice    
 *              versa. Target TCP/IP stack should include socket header
 *              files specific to their stack in this file. Also if   
 *              socket descriptor is not integer, then the respective
 *              value needs to be changed in cmsock.h file.
*/

#include<stdio.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <fcntl.h>
#include "cmincld.h"
#include "cmsock.h"

/***************************************************************/
/* This function creates a new socket based on the supplied    */
/* protocol_ui parameter. The protocol_ui values can either be   */
/* CM_IPPROTO_TCP or CM_IPPROTO_UDP. This function returns   */
/* handle to the new socket or returns OF_FAILURE on failure.   */
/***************************************************************/
cm_sock_handle cm_socket_create (unsigned char protocol_ui)
{
  cm_sock_handle sock_fd_i;

  switch (protocol_ui)
  {
    case CM_IPPROTO_TCP:
      {
        sock_fd_i = socket (AF_INET, SOCK_STREAM, protocol_ui);
        break;
      }
    case CM_IPPROTO_UDP:
      {
        sock_fd_i = socket (AF_INET, SOCK_DGRAM, protocol_ui);
        break;
      }
    case CM_IPPROTO_ICMP:
      {
        sock_fd_i = socket (AF_INET, SOCK_RAW, protocol_ui);
        break;
      }
    case CM_IPPROTO_IGMP:
      {
        sock_fd_i = socket (AF_INET, SOCK_RAW, protocol_ui);
        break;
      }
    default:
      return OF_FAILURE;
  }

  if (sock_fd_i < 0)
  {
    return OF_FAILURE;
  }
  else
  {
    return sock_fd_i;
  }
}

/***************************************************************/
/* This function binds the given socket descriptor to the IP   */
/* address and port supplied as input to this function. Both IP*/
/* address and port number has to be host byte order. If target*/
/* TCP/IP socket implementation expects IP and port numbers in */
/* network byte order, this function should convert to network */
/* address before calling the underlying socket bind function. */
/* This function returns OF_SUCCESS on success and OF_FAILURE on */
/* failure.                                                    */
/***************************************************************/
int32_t cm_socket_bind (cm_sock_handle sock_fd_i,
                       uint32_t local_ip, uint16_t local_port_ui)
{
  struct sockaddr_in LocalAddress;

  LocalAddress.sin_family = AF_INET;

  LocalAddress.sin_addr.s_addr = htonl (local_ip);
  LocalAddress.sin_port = htons (local_port_ui);

  if (bind (sock_fd_i, (struct sockaddr *) &LocalAddress,
            sizeof (struct sockaddr_in)) != OF_SUCCESS)
  {
    return OF_FAILURE;
  }

  return OF_SUCCESS;
}

/***************************************************************/
/* This function does TCP socket connect with a remote peer.   */
/* The function accepts the socket descriptor on which the     */
/* connections needs to be established along with the remote IP*/
/* address and the remote port. This function in-turn calls the*/
/* underlying socket connection function. This function returns*/
/* OF_SUCCESS on successful connection and OF_FAILURE on failed  */
/* connection attempt. Both peer_ip and peer_port_ui has to be in */
/* host byte order. If the socket implementation expects IP in */
/* network byte order, this function has to convert it before  */
/* calling the underlying socket connect.                      */
/***************************************************************/
int32_t cm_socket_connect (cm_sock_handle sock_fd_i,
                          uint32_t peer_ip, uint16_t peer_port_ui)
{
  struct sockaddr_in PeerAddress;

  PeerAddress.sin_family = AF_INET;

  PeerAddress.sin_addr.s_addr = htonl (peer_ip);
  PeerAddress.sin_port = htons (peer_port_ui);

  if (connect (sock_fd_i, (struct sockaddr *) (&PeerAddress),
               sizeof (struct sockaddr_in)) < 0)
  {
    return OF_FAILURE;
  }

  return OF_SUCCESS;
}

/***************************************************************/
/* This function sends data on the specified socket (sock_fd_i). */
/* The buffer that needs to be sent along with the length of   */
/* buffer and the flags is taken as input to this function. The*/
/* function invokes the underlying socket send function and the*/
/* return value of underlying socket send function will be the */
/* return value of this function as well.                      */
/***************************************************************/
int32_t cm_socket_send (cm_sock_handle sock_fd_i, char * buffer_p,
                       int32_t buf_len_i, int16_t flags_i)
{
  return send (sock_fd_i, buffer_p, buf_len_i, flags_i);
}

/***************************************************************/
/* This function sends data on the specified socket (sock_fd_i). */
/* The buffer that needs to be sent along with the length of   */
/* buffer and the flags is taken as input to this function. The*/
/* IP address of the remote machine and the remote port is     */
/* accepted as the final 2 parameters. Based on supplied values*/
/* function invokes the underlying socket sendto function and  */
/* return value of underlying socket sendto function will be   */
/* return value of this function as well.                      */
/* Both peer_ip and peer_port_ui values must be in host byte order*/
/* and if the target socket implementation expects IP and port */
/* in network byte order, this function has to convert them    */
/* before calling sendto.                                      */
/***************************************************************/
int32_t cm_socket_send_to (cm_sock_handle sock_fd_i, char * buffer_p,
                         int32_t buf_len_i, int16_t flags_i,
                         uint32_t peer_ip, uint16_t peer_port_ui)
{
  struct sockaddr_in PeerAddress;

  PeerAddress.sin_family = AF_INET;
  PeerAddress.sin_addr.s_addr = htonl (peer_ip);
  PeerAddress.sin_port = htons (peer_port_ui);

  return sendto (sock_fd_i, buffer_p, buf_len_i, flags_i,
                 (struct sockaddr *) &PeerAddress, sizeof (PeerAddress));
}

/***************************************************************/
/* This function receives data from specified socket (sock_fd_i).*/
/* The buffer in which the data needs to be read along with its*/
/* size is sent in as parameter. Flags is the final parameter. */
/* function invokes the underlying socket recv function and the*/
/* return value of underlying socket send function will be the */
/* return value of this function as well.                      */
/***************************************************************/
int32_t cm_socket_recv (cm_sock_handle sock_fd_i, char * buffer_p,
                       int32_t buf_len_i, int16_t flags_i)
{
  return recv (sock_fd_i, buffer_p, buf_len_i, flags_i);
}

/***************************************************************/
/* This function receives data from specified socket (sock_fd_i).*/
/* The buffer in which the data needs to be read is sent in as */
/* parameter along with the size of the buffer and flags. The  */
/* last 2 parameters are optional fields and if the calling    */
/* function needs to know the remote IP address and the remote */
/* port, then memory for storing IP address and port number    */
/* should be allocated and the address of which needs to be    */
/* sent in for the last 2 parameters. This function in turn    */
/* calls the underlying socket recvfrom function and the return*/
/* value for this function will be the return value of recvfrom*/
/* function. If the remote_ip_p and remote_port_ui_p is supplied    */
/* this function fills the values in both the fields.          */
/* If the underlying socket recvfrom function fills IP and port*/
/* number in network byte order, this function has to convert  */
/* it to host byte order before filling in remote_ip_p and       */
/* remote_port_ui_p.                                              */
/***************************************************************/
int32_t cm_socket_recv_from (cm_sock_handle sock_fd_i, char * buffer_p,
                           int32_t buf_len_i, int16_t flags_i,
                           uint32_t * remote_ip_p, uint16_t * remote_port_ui_p)
{
  struct sockaddr_in RemoteAddress;
  int32_t return_value, iAddrLength = sizeof RemoteAddress;
  unsigned char bStoreAddress;

  if (!remote_ip_p || !remote_port_ui_p)
    bStoreAddress = FALSE;
  else
    bStoreAddress = TRUE;

  if (bStoreAddress)
  {
    return_value = recvfrom (sock_fd_i, buffer_p, buf_len_i, flags_i,
                        (struct sockaddr *) &RemoteAddress, &iAddrLength);
  }
  else
    return_value = recvfrom (sock_fd_i, buffer_p, buf_len_i, flags_i, NULL, 0);

  /*
   *  Still This is not a ideal wrapper, because linux returns different error values
   *  on different occasions. like  EINTR , EWOULDBLOCK etc
   *  We should have PERFECT wrapper for them too.
   *              - Muralidhar
   */

/*--------------------------------------------------------------------------------**
**     if Success case only we have to assign the remote IP and Port not in       **
**     Failure Case.                                                              **
**     Why should we Change return_value ?                                             **
**     In Failure case Linux will return -1.  This is Equall to our OF_FAILURE     **
**     Return it as it is.                                                        **
**     -Bhagavan 17/10/2001                                                       **
**--------------------------------------------------------------------------------*/

  if (return_value >= 0 && bStoreAddress)
  {
    if (remote_ip_p)
    {
      *remote_ip_p = ntohl (RemoteAddress.sin_addr.s_addr);
    }
    if (remote_port_ui_p)
    {
      *remote_port_ui_p = ntohs (RemoteAddress.sin_port);
    }
  }

  return return_value;
}

/***************************************************************/
/* This function closes the open socket. This function in turn */
/* calls the underlying socket close function.                 */
/***************************************************************/
int32_t cm_socket_close (cm_sock_handle sock_fd_i)
{
  return close (sock_fd_i);
}

/*************************************************************************/
/* This function shut downs all or part of a full duplex connection.     */
/* This function in turn calls the underlying socket shutdown function.  */
/*************************************************************************/
int32_t cm_socket_shutdown (cm_sock_handle sock_fd_i, int32_t how_i)
{
  return shutdown (sock_fd_i, how_i);
}

/***************************************************************/
/* This function polls for registered events on the supplied   */
/* FDs and sets the events that have occured. This function    */
/* internally uses the target TCP/IP stack specific polling    */
/* mechanism and in-turn registers with that and invokes the   */
/* appropriate polling function. Once the under lying polling  */
/* function completes the operation, this function checks for  */
/* the events that have occured and then sets occured events   */
/* in the fds_to_be_polled_p record.                               */
/* If the target TCP/IP stack uses different polling mechanism */
/* this function has to be modified to register events and get */
/* get values accordingly.                                     */
/* This function returns the number of events triggered.       */
/***************************************************************/
int32_t cm_fd_poll (uint16_t num_desc_ui, struct cm_poll_info * fds_to_be_polled_p,
                   uint32_t time_out_ui)
{
  int32_t timeout, usPosition, retval;
  struct pollfd LocalPollFd[num_desc_ui];

  if (!num_desc_ui || num_desc_ui > CM_FDPOLL_MAXFDS || !fds_to_be_polled_p)
    return 0;

  CM_FDPOLL_ZERO (LocalPollFd, num_desc_ui);

  for (usPosition = 0; usPosition < num_desc_ui; usPosition++)
  {
    fds_to_be_polled_p[usPosition].returned_events = 0;
    LocalPollFd[usPosition].fd = fds_to_be_polled_p[usPosition].fd_i;

    if (fds_to_be_polled_p[usPosition].interested_events & CM_FDPOLL_READ)
    {
      LocalPollFd[usPosition].events |= POLLIN;
    }
    if (fds_to_be_polled_p[usPosition].interested_events & CM_FDPOLL_WRITE)
    {
      LocalPollFd[usPosition].events |= POLLOUT;
    }
    if (fds_to_be_polled_p[usPosition].interested_events & CM_FDPOLL_EXCEPTION)
    {
      LocalPollFd[usPosition].events |= POLLERR;
    }
    if (fds_to_be_polled_p[usPosition].interested_events & CM_FDPOLL_URGENT)
    {
      LocalPollFd[usPosition].events |= POLLPRI;
    }
  }

/*
 * More than one interested event may be recorded, and hence the evnts 
 * should be cumulatively added (ORed).
 */

/*------------------------------------------------------------------------------**
**                                                                              **
**   Note 1: In Linux if the time_out_ui is negative value means infinite timeout **
**   Note 2: IN Linux the actual poll system call proto type is                 **
**           int poll(struct pollfd *ufds, unsigned int nfds, int timeout);     **
**           But we are passing third argument as unsigned int.                 **
**           Internally poll call will treate that as int only                  **
**                                                                              **
** -Bhagavan 13/09/2001                                                         **
**------------------------------------------------------------------------------*/
#ifdef CM_SOCK_DEBUG
  printf ("in cm_fd_poll time_out_ui :%ld\r\n", time_out_ui);
#endif

  if (time_out_ui == 0)
    timeout = -1;
  else
    timeout = time_out_ui;

#ifdef CM_SOCK_DEBUG
  printf ("struct pollfd              :%d\r\n", sizeof (struct pollfd));
  printf ("struct pollfd LocalPollFd  :%d\r\n", sizeof (*(LocalPollFd)));
  printf ("struct cm_poll_info              :%d\r\n", sizeof (struct cm_poll_info));
#endif

  retval = poll (LocalPollFd, num_desc_ui, timeout);

  for (usPosition = 0; usPosition < num_desc_ui; usPosition++)
  {
    fds_to_be_polled_p[usPosition].returned_events = 0;
    if (LocalPollFd[usPosition].revents & POLLIN)
    {
      fds_to_be_polled_p[usPosition].returned_events |= CM_FDPOLL_READ;
    }
    if (LocalPollFd[usPosition].revents & POLLOUT)
    {
      fds_to_be_polled_p[usPosition].returned_events |= CM_FDPOLL_WRITE;
    }
    if ((LocalPollFd[usPosition].revents & POLLERR) ||
        (LocalPollFd[usPosition].revents & POLLHUP) ||
        (LocalPollFd[usPosition].revents & POLLNVAL))
    {
      fds_to_be_polled_p[usPosition].returned_events |= CM_FDPOLL_EXCEPTION;
    }
    if (LocalPollFd[usPosition].revents & POLLPRI)
    {
      fds_to_be_polled_p[usPosition].returned_events |= CM_FDPOLL_URGENT;
    }
  }

  return retval;
}

/*
 *
 *  Timeout will be Always in Seconds
 *
 */
int32_t cm_fd_select (uint16_t max_dpl_ui, cm_fd_set_t * read_fds_p,
                     cm_fd_set_t * write_fds_p, cm_fd_set_t * except_fds_p,
                     struct cm_time_out * time_out_p)
{
  if (time_out_p)
  {
#ifdef LINUX_SUPPORT
    return select (max_dpl_ui, read_fds_p, write_fds_p, except_fds_p, &(time_out_p->time_out));
#endif
  }
  else
  {
    return select (max_dpl_ui, read_fds_p, write_fds_p, except_fds_p, NULL);
  }
}

/***************************************************************/
/* Function Name: cm_socket_listen                              */
/*                                                             */
/* Prototype:                                                  */
/* int32_t cm_socket_listen(                                    */
/*                       cm_sock_handle sock_fd_i                 */
/*                       int32_t  backlog_i )                   */
/*                                                             */
/* Prototype:                                                  */
/* sock_fd_i - This field identifies the socket for which the    */
/* listen operation needs to be performed. This socket handle  */
/* is the return value of the ucm_socket_createFunction.         */
/* backlog_i - Maximumlength of queue of pending connections    */
/*                                                             */
/* Description:                                                */
/*                                                             */
/* Return Value:                                               */
/* This function returns OF_SUCCESS on success and OF_FAILURE on */
/* failure.                                                    */
/***************************************************************/
int32_t cm_socket_listen (cm_sock_handle sock_fd_i, int32_t backlog_i)
{
  if (listen (sock_fd_i, backlog_i) < 0)
  {
    return OF_FAILURE;
  }

  return OF_SUCCESS;
}

/***************************************************************/
/* Function Name: cm_socket_accept                              */
/*                                                             */
/* Prototype:                                                  */
/* int32_t cm_socket_accept(                                    */
/*                       cm_sock_handle  sock_fd_i,               */
/*                       uint32_t  *remote_ip ,                   */
/*                       uint16_t  *remote_port_ui )             */
/*                                                             */
/* Prototype:                                                  */
/* sock_fd_i - This field identifies the socket for which the    */
/* listen operation needs to be performed. This socket handle  */
/* is the return value of the ucm_socket_createFunction.         */
/*                                                             */
/* Description:                                                */
/*                                                             */
/* Return Value:                                               */
/* This function returns OF_SUCCESS on success and OF_FAILURE on */
/* failure.                                                    */
/***************************************************************/
cm_sock_handle cm_socket_accept (cm_sock_handle sock_fd_i,
                               uint32_t * remote_ip, uint16_t * remote_port_ui)
{
  cm_sock_handle iNewFd;
  struct sockaddr_in rmtAddr;
  int32_t rmtAddrLen;

  rmtAddrLen = sizeof (rmtAddr);
  if ((iNewFd =
       accept (sock_fd_i, (struct sockaddr *) &rmtAddr, &rmtAddrLen)) < 0)
  {
    return OF_FAILURE;
  }
  else
  {
    *remote_ip = ntohl (rmtAddr.sin_addr.s_addr);
    *remote_port_ui = ntohs (rmtAddr.sin_port);
    return iNewFd;
  }

  return OF_FAILURE;             /* Never comes here */
}

/*
 *   This is moved from HTTP to here.
 *   FIXME-This has to be modified to get RemoteHostName - not a nice function
 *   ----- Caller Should Free  remote_host_c and remote_ip_a
 */
int32_t cm_socket_get_peer_info (int32_t stream_sock_i,
                              char ** remote_host_c, char ** remote_ip_a)
{
  struct sockaddr addr;
  struct in_addr *iaddr;
  int32_t len = sizeof (addr);
  char *remoteHost = NULL, *remoteIp = NULL;

  *remote_host_c = NULL;
  *remote_ip_a = NULL;

  /* Get the peer's info */
  if ((getpeername (stream_sock_i, &addr, &len)) < 0)
  {
    return OF_FAILURE;
  }

  iaddr = &(((struct sockaddr_in *) (&addr))->sin_addr);
  remoteIp = (char *) inet_ntoa (*iaddr);
  *remote_host_c = remoteHost;
  if (remoteIp)
  {
    *remote_ip_a = (char *) of_strdup (remoteIp);
  }

  return OF_SUCCESS;
}

int32_t cm_socket_make_non_blocking (cm_sock_handle sock_fd_i)
{
  int32_t lOn, flags;
  lOn = TRUE;

  /******* 
      T_ NT32 iPrevFlags;
       iPrevFlags = fcntl(sock_fd_i,F_GETFL,0);  // Get the Previous Flags 
       return fcntl(sock_fd_i,F_SETFL, iPrevFlags | O_NONBLOCK); // Set Non-Blocking Flag 

       ioctl(sock_fd_i, FIONBIO, &lOn);
   ******/
  /*
   * Get the flags of the socket ---RKP
   */
  if ((flags = fcntl (sock_fd_i, F_GETFL, 0)) < 0)
  {
    perror ("CreateSocket():The getting flags is failed");
    return (OF_FAILURE);
  }

  /* 
   * Set the flag to Non-block mode
   */
  if (fcntl (sock_fd_i, F_SETFL, flags | O_NONBLOCK) < 0)
  {
    perror ("CreateSocket():theSetting Flags Failed");
    return (OF_FAILURE);
  }
  /*
   * If the process done correct then send return value zero
   */
  return OF_SUCCESS;
}

#if 0
int32_t UCMGetSockOption (cm_sock_handle sockid, int32_t * opt_value_p)
{
  int32_t errorlen = sizeof (int);

  return (getsockopt (sockid, SOL_SOCKET, SO_ERROR, opt_value_p, &errorlen));
}
#endif
int32_t cm_get_socket_option (cm_sock_handle iSockid, int32_t level_i, int32_t opt_name_i,
                                                              int32_t *opt_value_p)
{     
         int32_t iOptlen = sizeof(int);
  
         return(getsockopt(iSockid,level_i,opt_name_i,(void*)opt_value_p,&iOptlen));
}/* end IGWGetSocketOption */

int32_t cm_set_socket_option (cm_sock_handle iSockid, int32_t level_i, int32_t opt_name_i,
                                                              int32_t opt_value_i)
{
         int32_t iOptlen = sizeof(int);
         return(setsockopt(iSockid,level_i,opt_name_i,(void *)&opt_value_i,iOptlen));
}/*end  IGWGetSocketOption */


/***************************************************************************
 * Function Name : 
 * Description   :
 * Input         : 
 * Output        :
 * Return value  :
 ***************************************************************************/
int32_t cm_socket_get_sock_name (cm_sock_handle fd_i, uint32_t * ip_p, uint16_t * port_p)
{
  struct sockaddr_in MyAddr;
  int32_t MyAddrLen;

  MyAddrLen = sizeof (MyAddr);

  if (getsockname (fd_i, (struct sockaddr *) &MyAddr, &MyAddrLen) < 0)
  {
    return OF_FAILURE;
  }

  if (ip_p)
  {
    *ip_p = ntohl (MyAddr.sin_addr.s_addr);
  }

  if (port_p)
  {
    *port_p = ntohs (MyAddr.sin_port);
  }

  return OF_SUCCESS;
}

/***************************************************************************
 * Function Name : 
 * Description   :
 * Input         : 
 * Output        :
 * Return value  :
 ***************************************************************************/
int32_t cm_socket_get_peer_name (cm_sock_handle fd_i, uint32_t * ip_p, uint16_t * port_p)
{
  struct sockaddr_in MyAddr;
  int32_t MyAddrLen;

  MyAddrLen = sizeof (MyAddr);

  if (getpeername (fd_i, (struct sockaddr *) &MyAddr, &MyAddrLen) < 0)
  {
    return OF_FAILURE;
  }

  if (ip_p)
  {
    *ip_p = ntohl (MyAddr.sin_addr.s_addr);
  }

  if (port_p)
  {
    *port_p = ntohs (MyAddr.sin_port);
  }

  return OF_SUCCESS;
}

/***************************************************************************
 * Function Name : 
 * Description   :
 * Input         : 
 * Output        :
 * Return value  :
 ***************************************************************************/
int32_t cm_socket_set_opt (int32_t sock_fd_i, uint32_t command_ui, void * pArg)
{
#ifdef LINUX_SUPPORT
  if (command_ui == SET_RESET_BLOCK_MODE)
  {
    return (cm_socket_make_non_blocking (sock_fd_i));
  }
#endif
  printf ("------------- cm_socket_set_opt : Not Implemented ------------\n");
  return OF_SUCCESS;
}

/***************************************************************************
 * Function Name : 
 * Description   :
 * Input         : 
 * Output        :
 * Return value  :
 ***************************************************************************/
int32_t cm_socket_get_opt (int32_t sock_fd_i, uint32_t command_ui, void * pArg)
{
#ifdef LINUX_SUPPORT
  uint32_t ulFlags = 0;
	int32_t iTmpLen = 0;
	unsigned char bConn;
#endif

  switch (command_ui)
  {
#ifdef LINUX_SUPPORT
    case CM_GET_CONNECTION_STATUS:
      ulFlags = 0;
      iTmpLen = sizeof (ulFlags);
      bConn = FALSE;
      if ((getsockopt (sock_fd_i, SOL_SOCKET, SO_ERROR, &ulFlags, &iTmpLen) == 0)
          && (ulFlags == 0))
      {
        bConn = TRUE;
      }
      *(uint32_t *) pArg = bConn ? 1 : 0;
      break;
#endif
    default:
      printf ("------------- cm_socket_get_opt : Not Implemented ------------\n");
      break;
  }

  return OF_SUCCESS;
}

/***************************************************************************
 * Function Name : cm_set_sock_reuse_opt
 * Description   : This function sets SO_REUSEADDR option for the socket 
 *                 so that a socket can bind to a port if noother is using
 *                 that port
 * Input         : Socket Handle 
 * Output        : None
 * Return value  : None
 ***************************************************************************/
void cm_set_sock_reuse_opt (cm_sock_handle sock_fd_i)
{
  int32_t iOptVal = 1;

  if(setsockopt (sock_fd_i, SOL_SOCKET, SO_REUSEADDR, &iOptVal, sizeof (iOptVal))<0)
  {
    perror ("Set socket options for reusing socket failed ");
  }
}

/***************************************************************************
 * Function Name : cm_set_sock_iphdr_inc_opt
 * Description   : This function sets IP_HDRINCL option for the given raw 
 * socket. when this option is set, the application must build the entire
 * IP header except (a) the IPV4 identification field can be set to 0, which 
 * tells the kernel to set this value, and (b) the kernel always calculates 
 * and stores the IPV4 header checksum
 * Input         : Socket Handle
 * Output        : None
 * Return value  : OF_SUCCESS/OF_FAILURE
 ***************************************************************************/
int32_t cm_set_sock_iphdr_inc_opt (cm_sock_handle sock_fd_i)
{
  unsigned char bInclude = TRUE;

  if (setsockopt (sock_fd_i, IPPROTO_IP, IP_HDRINCL,
                  (char *) & bInclude, sizeof (bInclude)) < 0)
  {
    perror ("cm_set_sock_iphdr_inc_opt():IP_HDRINCL option setting failed");
    return OF_FAILURE;
  }
  return OF_SUCCESS;
}

/***************************************************************************
 * Function Name : 
 * Description   :
 * Input         : 
 * Output        :
 * Return value  :
 ***************************************************************************/
int32_t cm_set_tcp_keep_alive_options (cm_sock_handle stream_sock_i,
                                   struct cm_tcp_keep_alive_options_info * options_info_p)
{
  int32_t iOptVal;
  int32_t lRetVal;
  int32_t iError;

  do
  {
    iError = OF_SUCCESS;
    if (options_info_p == NULL)
    {
      iError = OF_FAILURE;
      break;
    }

    /*
     * First set the TCP Keep Alive option
     */
    if (options_info_p->enable_b == TRUE)
      iOptVal = 1;
    else
      iOptVal = 0;

    lRetVal = setsockopt (stream_sock_i,
                          SOL_SOCKET, SO_KEEPALIVE, &iOptVal, sizeof (iOptVal));
    if (lRetVal != 0)
    {
      iError = OF_FAILURE;
      break;
    }

    /*
     * If TCP Keep Alive option is enabled then set other options also
     */
    if (options_info_p->enable_b == TRUE)
    {
      /*
       * Idle Time
       */
      iOptVal = options_info_p->idle_time_ui;
      lRetVal = setsockopt (stream_sock_i,
                            SOL_TCP, TCP_KEEPIDLE, &iOptVal, sizeof (iOptVal));
      if (lRetVal != 0)
      {
        iError = OF_FAILURE;
        break;
      }

      /*
       * Probe Interval
       */
      iOptVal = options_info_p->probe_interval_ui;
      lRetVal = setsockopt (stream_sock_i,
                            SOL_TCP, TCP_KEEPINTVL, &iOptVal, sizeof (iOptVal));
      if (lRetVal != 0)
      {
        iError = OF_FAILURE;
        break;
      }

      /*
       * Probe Count
       */
      iOptVal = options_info_p->probe_count_ui;
      lRetVal = setsockopt (stream_sock_i,
                            SOL_TCP, TCP_KEEPCNT, &iOptVal, sizeof (iOptVal));
      if (lRetVal != 0)
      {
        iError = OF_FAILURE;
        break;
      }
    }
  }
  while (0);

  return iError;
}

/***************************************************************************
 * Function Name : 
 * Description   :
 * Input         : 
 * Output        :
 * Return value  :
 ***************************************************************************/
int32_t cm_get_tcp_keep_alive_options (cm_sock_handle stream_sock_i,
                                   struct cm_tcp_keep_alive_options_info * options_info_p)
{
  int32_t iOptVal;
  socklen_t iOptValLen;
  int32_t lRetVal;
  int32_t iError;

  do
  {
    iError = OF_SUCCESS;
    if (options_info_p == NULL)
    {
      iError = OF_FAILURE;
      break;
    }

    iOptVal = -1;
    iOptValLen = sizeof (iOptVal);
    lRetVal = getsockopt (stream_sock_i,
                          SOL_SOCKET, SO_KEEPALIVE, &iOptVal, &iOptValLen);
    if (lRetVal != 0)
    {
      iError = OF_FAILURE;
      break;
    }

    /*
     * First set the TCP Keep Alive option
     */
    if (iOptVal == 1)
      options_info_p->enable_b = TRUE;
    else
      options_info_p->enable_b = FALSE;

    /*
     * If TCP Keep Alive option is enabled then set other options also
     */
    if (options_info_p->enable_b == TRUE)
    {
      /*
       * Idle Time
       */
      iOptVal = -1;
      iOptValLen = sizeof (iOptVal);
      lRetVal = getsockopt (stream_sock_i,
                            SOL_TCP, TCP_KEEPIDLE, &iOptVal, &iOptValLen);
      if (lRetVal != 0)
      {
        iError = OF_FAILURE;
        break;
      }
      options_info_p->idle_time_ui = iOptVal;

      /*
       * Probe Interval
       */
      iOptVal = -1;
      iOptValLen = sizeof (iOptVal);
      lRetVal = getsockopt (stream_sock_i,
                            SOL_TCP, TCP_KEEPINTVL, &iOptVal, &iOptValLen);
      if (lRetVal != 0)
      {
        iError = OF_FAILURE;
        break;
      }
      options_info_p->probe_interval_ui = iOptVal;

      /*
       * Probe Count
       */
      iOptVal = -1;
      iOptValLen = sizeof (iOptVal);
      lRetVal = getsockopt (stream_sock_i,
                            SOL_TCP, TCP_KEEPCNT, &iOptVal, &iOptValLen);
      if (lRetVal != 0)
      {
        iError = OF_FAILURE;
        break;
      }
      options_info_p->probe_count_ui = iOptVal;

    }
  }
  while (0);

  return iError;
}

/***************************************************************************
 * Function Name : 
 * Description   :
 * Input         : 
 * Output        :
 * Return value  :
 ***************************************************************************/
int32_t cm_set_sock_linger_option (cm_sock_handle stream_sock_i,
                                struct cm_socket_linger_option_info * options_info_p)
{
  int32_t lRetVal;
  int32_t iError;
  struct linger linger;

  do
  {
    iError = OF_SUCCESS;

    if (options_info_p == NULL)
    {
      iError = OF_FAILURE;
      break;
    }

    /*
     * set the Linger option
     */
    if (options_info_p->enable_b == TRUE)
    {
      linger.l_onoff = 1;
      linger.l_linger = options_info_p->linger_time_ui;
    }
    else
    {
      linger.l_onoff = 0;
      linger.l_linger = 0;
    }

    lRetVal = setsockopt (stream_sock_i,
                          SOL_SOCKET, SO_LINGER, &linger, sizeof (linger));
    if (lRetVal != 0)
    {
      iError = OF_FAILURE;
      break;
    }
  }
  while (0);

  return iError;
}

/***************************************************************************
 * Function Name : 
 * Description   :
 * Input         : 
 * Output        :
 * Return value  :
 ***************************************************************************/
int32_t cm_get_sock_linger_option (cm_sock_handle stream_sock_i,
                                struct cm_socket_linger_option_info * options_info_p)
{
  int32_t lRetVal;
  int32_t iError;
  socklen_t iOptValLen;
  struct linger linger;

  do
  {
    iError = OF_SUCCESS;

    if (options_info_p == NULL)
    {
      iError = OF_FAILURE;
      break;
    }

    /*
     * get the Linger option
     */
    iOptValLen = sizeof (linger);
    lRetVal = getsockopt (stream_sock_i,
                          SOL_SOCKET, SO_LINGER, &linger, &iOptValLen);
    if (lRetVal != 0)
    {
      iError = OF_FAILURE;
      break;
    }

    if (linger.l_onoff)
    {
      options_info_p->enable_b = TRUE;
      options_info_p->linger_time_ui = linger.l_linger;
    }
    else
    {
      options_info_p->enable_b = FALSE;
      options_info_p->linger_time_ui = 0;
    }
  }
  while (0);

  return iError;

}
