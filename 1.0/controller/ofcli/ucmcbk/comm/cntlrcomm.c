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
 * File name: cntlrcomm.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: UCM to controller socket class.
 * 
 */

#include "cbcmninc.h"
#include "openflow.h"
#include "cmcntrlr.h"
#include "cmcomm.h"

#define CM_CONTROLLER_PORT 63002


int32_t UCMSendCmdToController( ucmcommmsg_t *ucmcommmsg, int32_t *pOutLen);
/******************************************************************************
 ** Function Name : UCMSendCmdToController
 ** Description   : This API Creates a TCP socket, bind on known port and starts
 **                  Listening
 ** Input params  : None
 ** Output params : None
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t  iCntrSockfd_g=0;

int32_t UCMCreateCommChannelwithController()
{
	 
  int32_t sock_fd_i = 0;
 
 	CM_COMM_DEBUG_PRINT ("Entered");
  /*Creating socket to controller */
  if ((sock_fd_i = cm_socket_create (CM_IPPROTO_UDP)) == OF_FAILURE)
  {
    CM_COMM_DEBUG_PRINT ("Error :Unable to Create SyncSign Tcp socket");
    return OF_FAILURE;
  }

  /* Binding to controller socket */
#if 0
  if (cm_socket_bind (sock_fd_i, htonl(CM_LOOPBACK_ADDR),
              htons(CM_CONTROLLER_PORT)) == OF_FAILURE)
  {
     CM_COMM_DEBUG_PRINT ("Unable to connect to loopback port %d", htons(CM_CONTROLLER_PORT));
     cm_socket_close (sock_fd_i);
     return OF_FAILURE;
  }
#endif
	iCntrSockfd_g=sock_fd_i;
}

int32_t UCMSendCmdToController( ucmcommmsg_t *ucm_comm_msg_p, int32_t *pOutLen)
{
	 struct sockaddr_in local_addr;
	 struct cm_poll_info    PollFds[1] = {};
	 uint32_t           peer_ip;
	 uint16_t         PeerPort = 0;
	 ucmcntrlmsg_t CntrlResp;
	 int32_t return_value,i;
	 char *c;
	 int32_t ulMultiPartBufLen=0;
	 char *pMultiPartBuf=NULL;


	 CM_COMM_DEBUG_PRINT ("Sending Command to ONDirector");

	 PollFds[0].fd_i = iCntrSockfd_g;
	 PollFds[0].interested_events = CM_FDPOLL_READ;
	 PollFds[0].returned_events = 0;

	 /* Send UCM Data to Controller along with Application ID and Module ID */
	 if (ucm_comm_msg_p == NULL) 
	 {
			CM_COMM_DEBUG_PRINT ("Data is NULL");
			return OF_FAILURE;
	 }
#if 0
	 c=(char*)pInData;
	 CM_CNTRL_PRINT_PACKET(c, 100);
#endif
	 return_value = cm_socket_send_to(iCntrSockfd_g,(char*)&ucm_comm_msg_p->cntlreqmsg, sizeof(ucmcntrlmsg_t), 
				 0, CM_LOOPBACK_ADDR, CM_CONTROLLER_PORT);
	 if(return_value < 0)
	 {
			CM_COMM_DEBUG_PRINT ("Send To Failed");
			return OF_FAILURE;
	 }

	 CM_COMM_DEBUG_PRINT ("Succesfully sent");

	 /* Poll to receive the output */
	 return_value=cm_fd_poll(1, PollFds, 10 *1000);  
	 if(return_value < 0)
	 {
			CM_COMM_DEBUG_PRINT ("Poll Failed");
			return OF_FAILURE;
	 }

	 do
	 {
			if(PollFds[0].returned_events & CM_FDPOLL_READ)
			{
				 CM_COMM_DEBUG_PRINT ("poll read event recieving  message");

				 /* Receive data from socket */
                 of_memset(&CntrlResp, 0 , sizeof(ucmcntrlmsg_t));
				 return_value = cm_socket_recv_from(iCntrSockfd_g, (unsigned char*)&CntrlResp, sizeof(ucmcntrlmsg_t),
							 0, &peer_ip, &PeerPort);
				 if(return_value < 0)
				 {
						CM_COMM_DEBUG_PRINT ("Recv from failed");
						return OF_FAILURE;
				 }
				 CM_COMM_DEBUG_PRINT ("recved message");

				 if(CntrlResp.msg_type == CM_CNTRL_MULTIPART_RESPONSE)
				 {
						CM_COMM_DEBUG_PRINT ("Multipart reply message");
						if(!pMultiPartBuf)
							 pMultiPartBuf=of_calloc(1,CM_CNTRL_MAX_MULTIPART_RESP_LEN);
						if(!pMultiPartBuf)
						{
							 CM_COMM_DEBUG_PRINT ("memory alloc for multipart failed");
							 return OF_FAILURE;

						}
						if(CntrlResp.result_code == OF_FAILURE)
						{
							 of_memcpy(&ucm_comm_msg_p->cntlrespmsg, &CntrlResp,sizeof(ucmcntrlmsg_t));
							 ucm_comm_msg_p->msglen =CntrlResp.length;
							 *pOutLen=return_value;
							 if(pMultiPartBuf)
							 of_free(pMultiPartBuf);
							 break;
						}
						if( CntrlResp.multi_part ==  CM_CNTRL_MULTIPART_REPLY_MORE)
						{
							 CM_COMM_DEBUG_PRINT ("More replies to be followed for this  message");
							 of_memcpy(pMultiPartBuf+ulMultiPartBufLen, &CntrlResp.msg,CntrlResp.length);
							 ulMultiPartBufLen+=CntrlResp.length;
							 *pOutLen+=return_value;
						}
						else
						{
							 of_memcpy(pMultiPartBuf+ulMultiPartBufLen, &CntrlResp.msg,CntrlResp.length);
							 ulMultiPartBufLen+=CntrlResp.length;
							 of_memcpy(&ucm_comm_msg_p->cntlrespmsg, &CntrlResp,32);
							 ucm_comm_msg_p->msglen = ulMultiPartBufLen;
							 ucm_comm_msg_p->pmultipart=pMultiPartBuf;
							 *pOutLen=return_value;
							 CM_COMM_DEBUG_PRINT ("This is last message of multipart... total len %d ",ulMultiPartBufLen);
							 break;
						}
				 }
				 else
				 {
						of_memcpy(&ucm_comm_msg_p->cntlrespmsg, &CntrlResp,sizeof(ucmcntrlmsg_t));
						ucm_comm_msg_p->msglen =CntrlResp.length;
						*pOutLen=return_value;
						break;
				 }
			}
	 }while(1);

	 return OF_SUCCESS;
}


int32_t UCMCntrlCommHandler (ucmcommmsg_t *pCommMsg, struct cm_app_result *app_result_p)
{
	 int32_t return_value;
	 int32_t ulResponseLen;  
	 ucmcntrlmsg_t *pUCMResponse;
	 char *pMultipartBuf;



	 return_value=UCMSendCmdToController(pCommMsg, &ulResponseLen);
	 if(return_value==OF_FAILURE)
	 {
			CM_COMM_DEBUG_PRINT("Return Value = %d",return_value);
			return return_value;
	 }

	 pUCMResponse=&pCommMsg->cntlrespmsg;
	 CM_COMM_DEBUG_PRINT("total resplength=%d msgleng=%d app_id =%d cmdid=%d subcmdid=%d translen=%d resultcode %d",ulResponseLen, pUCMResponse->length, pUCMResponse->app_id,pUCMResponse->cmd_id,pUCMResponse->sub_cmd_id, pUCMResponse->trans_len, pUCMResponse->result_code);


	 switch(pUCMResponse->sub_cmd_id)
	 {
			case CM_CPDP_APPL_TRANS_BEGIN_CMD:
				 if(pUCMResponse->result_code == OF_FAILURE)
				 {
						app_result_p->result_code = OF_FAILURE;
						if(pUCMResponse->length)
						{
							 app_result_p->result_string_p = (char *)of_calloc(1,pUCMResponse->length +1);
							 of_strncpy(app_result_p->result_string_p,pUCMResponse->msg,pUCMResponse->length);
							 //printf("updated result string. %s",app_result_p->result_string_p);
						}
						return_value=pUCMResponse->error_code;
						goto fail_case;
				 }
				 else
				 {       
						CM_COMM_DEBUG_PRINT("CM_CPDP_APPL_TRANS_BEGIN_CMD");
				 }
				 break;
			case CM_CPDP_APPL_ADD_CMD:
			case CM_CPDP_APPL_SET_CMD:
			case CM_CPDP_APPL_DEL_CMD:
			case CM_CPDP_APPL_TEST_CMD:

				 if(pUCMResponse->result_code == OF_FAILURE)
				 {
						CM_COMM_DEBUG_PRINT("failed");
						app_result_p->result_code = OF_FAILURE;
						if(pUCMResponse->length)
						{
							 app_result_p->result_string_p = (char *)of_calloc(1,pUCMResponse->length +1);
							 of_strncpy(app_result_p->result_string_p,pUCMResponse->msg,pUCMResponse->length);
							 //printf("updated result string. %s",app_result_p->result_string_p);
						}
						// return OF_FAILURE;
						return_value=pUCMResponse->error_code;
						goto fail_case;
				 }
				 break;

			case CM_CPDP_APPL_TRANS_END_CMD:
				 if(pUCMResponse->result_code == OF_FAILURE)
				 {
						CM_COMM_DEBUG_PRINT("CM_CPDP_APPL_TRANS_END_CMD Failure");
						app_result_p->result_code = OF_FAILURE;
						if(pUCMResponse->length)
						{
							 app_result_p->result_string_p = (char *)of_calloc(1,pUCMResponse->length +1);
							 of_strncpy(app_result_p->result_string_p,pUCMResponse->msg,pUCMResponse->length);
							 //printf("updated result string. %s",app_result_p->result_string_p);
						}
						return_value=pUCMResponse->error_code;
						goto fail_case;
						// return OF_FAILURE;
				 }
				 else
				 {
						CM_COMM_DEBUG_PRINT("CM_CPDP_APPL_TRANS_END_CMD success");
				 }
				 break;
			case CM_CPDP_APPL_GET_FIRSTN_CMD:
				 CM_COMM_DEBUG_PRINT("CM_CPDP_APPL_GET_FIRSTN_CMD ");
				 if(pUCMResponse->result_code == OF_FAILURE)
				 {
						CM_COMM_DEBUG_PRINT("failed");
						return_value=OF_FAILURE;
						goto fail_case;
				 }
				 break;
			case CM_CPDP_APPL_GET_NEXTN_CMD:
				 CM_COMM_DEBUG_PRINT("CM_CPDP_APPL_GET_NEXTN_CMD");
				 if(pUCMResponse->result_code == OF_FAILURE)
				 {
						CM_COMM_DEBUG_PRINT("failed");
						return_value=OF_FAILURE;
						goto fail_case;
				 }
				 break;
			case CM_CPDP_APPL_GET_EACT_CMD:
				 CM_COMM_DEBUG_PRINT("CM_CPDP_APPL_GET_EACT_CMD");
				 if(pUCMResponse->result_code == OF_FAILURE)
				 {
						CM_COMM_DEBUG_PRINT("failed");
						return_value=OF_FAILURE;
						goto fail_case;
				 }
				 break;
			default:
				 CM_COMM_DEBUG_PRINT("Invalid Result Command");
				 goto fail_case;
	 }
	 return OF_SUCCESS;
fail_case:
	 return 	return_value;
}
