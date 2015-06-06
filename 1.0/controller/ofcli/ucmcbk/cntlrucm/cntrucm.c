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
 * File name: cntrucm.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: Controller to UCM socket communication.
 * 
 */

#include"cbcmninc.h"
#include "cmgutil.h"
#include"igwincl.h"
#include "cntrucminc.h"
#define CM_CONTROLLER_PORT 63002

void cntlr_ucm_handle_cfg_msgs(int32_t  fd,
			epoll_user_data_t *info,	uint32_t  received_events);

int32_t ucm_sock_fd;
extern cntlr_thread_info_t master_thread_info;
extern __thread cntlr_thread_info_t *self_thread_info;

void cntrl_ucm_init( )
{
	 ucm_cntrl_app_init();
	 
	 ucm_cntrl_create_conn();

}

/******************************************************************************
 ** Function Name : ucm_cntrl_create_conn
 ** Description   : This API Creates a TCP socket, bind on known port for ucm requests
 **                  Listening
 ** Input params  : None
 ** Output params : None
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t ucm_cntrl_create_conn()
{
	 int32_t sock_fd;
	 int32_t retval;
	 int32_t status=OF_SUCCESS;
	 int32_t intrested_events;
	 struct sockaddr_in local_addr;
	 epoll_user_data_t          user_data;

				 cntrlrucm_debugmsg("entered");
	 do
	 {
			sock_fd= of_socket_create(OF_IPPROTO_UDP);
			if( sock_fd == OF_FAILURE )
			{
				 perror("socket creation failed");
				 status = OF_FAILURE;
				 break;
			}

			memset((char*)&local_addr,0,sizeof(local_addr));
			local_addr.sin_family      = AF_INET;
			local_addr.sin_addr.s_addr = htonl(OF_LOOPBACK_SOCKET_IPADDR);
			local_addr.sin_port        = htons(CM_CONTROLLER_PORT);


			retval = bind(sock_fd, (struct sockaddr *)&local_addr, sizeof(struct sockaddr_in));
			if(retval == OF_FAILURE)
			{
				 cntrlrucm_debugmsg("bind failed");
				 perror("socket bind failed");
				 status = OF_FAILURE;
				 break;
			}

				 cntrlrucm_debugmsg("entered");
			ucm_sock_fd=sock_fd;
			intrested_events  = ePOLL_POLLIN;
			user_data.arg1 = NULL;
			user_data.arg2 = NULL;
     cntlr_epoll_add_fd(
//		 			 cntlr_epoll_add_fd_to_thread(&master_thread_info,
						ucm_sock_fd,
						intrested_events,
						&user_data,
						cntlr_ucm_handle_cfg_msgs);
			if(retval == OF_FAILURE)
			{
				 cntrlrucm_debugmsg("%s:%d Failed to Add FD to epoll context of master thread\r\n", __FUNCTION__,__LINE__);
				 perror("epollAdd");
				 status = OF_FAILURE;
				 break;
			}
	 }while (0);


	 if(status == OF_FAILURE)
	 {
			cntrlrucm_debugmsg("%s:%d Failed", __FUNCTION__,__LINE__);
			if(ucm_sock_fd > 0)
				 close(ucm_sock_fd);
			return status;
	 }

	 cntrlrucm_debugmsg("success");
	 return OF_SUCCESS;
}


void cntlr_ucm_handle_cfg_msgs(int32_t  fd,
			epoll_user_data_t *info,
			uint32_t           received_events)

{
	 int32_t sock_fd = 0;
	 int32_t retval;
	 uint32_t addr_length;
	 ucmcntrlmsg_t *ucmcntrlmsg;
	 struct sockaddr_in           remote_addr;


	 cntrlrucm_debugmsg("%s:%d entered\r\n", __FUNCTION__,__LINE__);

	 ucmcntrlmsg=(ucmcntrlmsg_t *)calloc(1,sizeof(ucmcntrlmsg_t));
	 if(ucmcntrlmsg == NULL)
	 {
			cntrlrucm_debugmsg("%s:%d memory allocation failed\n", __FUNCTION__,__LINE__);
			return;
	 }
	 addr_length = sizeof(remote_addr);
	 retval = recvfrom(fd,
				 (char*)ucmcntrlmsg, sizeof(ucmcntrlmsg_t),
				 0,
				 (struct sockaddr *)&remote_addr,
				 (socklen_t *)&addr_length);
	 if(retval < 0)
	 {
			cntrlrucm_debugmsg("%s:%d error in reading the data on controller msg FD\n", __FUNCTION__,__LINE__);
			perror("read");
			free(ucmcntrlmsg);
			return;
	 }

	 if(retval  > 0)
	 {
			retval= cntrl_ucm_process_request (&remote_addr,  ucmcntrlmsg);
	 }

	 free(ucmcntrlmsg);
	 return;
}

int32_t cntrl_ucm_sendresponse(struct sockaddr_in *peer_address, char *msg, uint32_t msg_len)
{
	 int32_t retval;
	 
	 cntrlrucm_debugmsg("%s:%d  sending msg to ucm\r\n",__FUNCTION__,__LINE__);
	 retval = sendto(ucm_sock_fd,
				 (char *)msg,
				 msg_len,
				 0,
				 (struct sockaddr *)peer_address,
				 sizeof(struct sockaddr_in));
	 if( retval <= 0 )
	 {
			cntrlrucm_debugmsg("%s:%d  Event send to Transport layer thread failed !!!\r\n",__FUNCTION__,__LINE__);
			close(ucm_sock_fd);
			return  OF_FAILURE;
	 }
	 return  OF_SUCCESS;
}
