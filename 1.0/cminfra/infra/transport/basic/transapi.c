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
 * File name: transapi.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/05/2013
 * Description: UCM Transport Channel Support 
*/

#ifdef CM_TRANSPORT_CHANNEL_SUPPORT
#include <netinet/in.h>
#include "trincld.h"
#include "trchllif.h"

extern uint32_t uiOSIntSize_g;
/****************************************************************************
 * Function definition section:
 *****************************************************************************/

/**
  \ingroup TRCHNLCAPI
  This API is used to create a Transport Channel. The transport channel is a secure
  TCP socket connection. This API will create a Socket and connects to the specified
  Server Port.

  <b>Input Parameters: </b>\n
  <b><i>trans_protocol_ui:</i></b> Specifies the Transport Protocol using which the 
  Socket will be created. By default it's TCP. 

  <b><i>addr_ui:</i></b> Specifies the IP Address. \n

  <b><i>src_port_ui:</i></b> Specifies the Source Port. \n

  <b><i>dest_port_ui:</i></b> Specifies the Destination Port. Using this port the Socket 
  will get connected to the other end.\n

  <b>Output Parameters:</b> NONE \n

  <b>Return Value:</b> Pointer to the Transport channel structure on successfull 
  creation of socket and connecting to the Server.\n

  Null pointer on failure.\n
  */

/******************************************************************************
 * Function Name : cm_tnsprt_create_channel
 * Description   : This API is used to create a Transport Channel
 * Input params  : trans_protocol_ui - Transport Protocol
 *                 src_port_ui - Source Port
 *                 addr_ui - IP Address
 *                 dest_port_ui - Destination Port
 * Output params : NONE
 * Return value  : Return pointer to Transport channel on successful creation of
 *                 socket and connecting to the Server. 
 *                 NULL Pointer on failure.
 *****************************************************************************/
void * cm_tnsprt_create_channel (uint8_t trans_protocol_ui, uint32_t addr_ui,
      uint16_t src_port_ui, uint16_t dest_port_ui)
{
   struct cm_tnsprt_channel *tnsprt_channel_p;
   if(addr_ui == 0)
   {
      /* 
       * if the user does not specify the IP Address,
       * consider the loopback address.
       */
      addr_ui = CM_LOOPBACK_ADDR;
   }

   /* Allocate transport channel node */
   tnsprt_channel_p =
      (struct cm_tnsprt_channel *) of_calloc (1, sizeof (struct cm_tnsprt_channel));
   if (unlikely (tnsprt_channel_p == NULL))
   {
      CM_TRANS_DEBUG_PRINT ("Unable to allocate memory");
      return NULL;
   }

   /* Create Socket */
   tnsprt_channel_p->sock_fd_i = cm_socket_create (trans_protocol_ui);
   if (unlikely (tnsprt_channel_p->sock_fd_i == OF_FAILURE))
   {
      CM_TRANS_DEBUG_PRINT ("Failed to create socket");
      of_free (tnsprt_channel_p);
      tnsprt_channel_p = NULL;
      return NULL;
   }

   /* Bind to local port, if requested */
   if (src_port_ui)
   {
      if (cm_socket_bind (tnsprt_channel_p->sock_fd_i, addr_ui, src_port_ui)
            == OF_FAILURE)
      {
         CM_TRANS_DEBUG_PRINT ("Unable to bind to port %d",  src_port_ui);
         cm_socket_close (tnsprt_channel_p->sock_fd_i);
         of_free (tnsprt_channel_p);
         tnsprt_channel_p = NULL;
         return NULL;
      }
   }

   /* Now connect to UCM infrastructure */
   if (trans_protocol_ui == CM_IPPROTO_TCP)
   {
      if (cm_socket_connect (tnsprt_channel_p->sock_fd_i, addr_ui,
               dest_port_ui) == OF_FAILURE)
      {
         CM_TRANS_DEBUG_PRINT ("Unable to connect to loopback port %d",    dest_port_ui);
         cm_socket_close (tnsprt_channel_p->sock_fd_i);
         of_free (tnsprt_channel_p);
         tnsprt_channel_p = NULL;
         return NULL;
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
 * Function Name : cm_tnsprt_close_channel
 * Description   : This API is used to close the Transport connection with the
 *                 Server and frees the Transport channel node.
 * Input params  : tnsprt_channel_p - pointer to the Transport channel structure.
 * Output params : NONE
 * Return value  : NONE
 *****************************************************************************/
void cm_tnsprt_close_channel (struct cm_tnsprt_channel * tnsprt_channel_p)
{
   if (unlikely (tnsprt_channel_p == NULL))
   {
      CM_TRANS_DEBUG_PRINT ("Invalid Transport channel");
      return;
   }

   cm_socket_close (tnsprt_channel_p->sock_fd_i);
   of_free (tnsprt_channel_p);
   tnsprt_channel_p = NULL;
}

/**
  \ingroup TRCHNLCAPI
  This API is used to set the attributes to the Transport Channel. This will take
  the attribute ID and attribute Data as input parameters and will set the attributes
  to the Transport channel. One of the attribute is Inactivity Timeout.

  <b>Input Parameters: </b> \n
  <b><i>tnsprt_channel_p:</i></b> Pointer to the Transport channel node.\n
  <b><i>attrib_p:</i></b> Pointer to the struct cm_tnsprt_channel_attribs structure.
  Holds the Attribute identifier and attribute data.\n
  <b><i>role_info_p:</i></b> Pointer to the struct cm_role_info structure. Holds the
  Admin role and name information.

  <b>Output Parameters:</b> NONE \n

  <b>Return Value:</b>\n
  <b><i>OF_SUCCESS:</i></b> On Successfull setting of Attribute.
  <b><i>OF_FAILURE:</i></b> On any failure.

*/

/******************************************************************************
 * Function Name : cm_tnsprt_set_channel_attribs
 * Description   : This API is used to set the attributes to the Transport channel.
 *                 The attribute can be Inactivity timeout.
 * Input params  : tnsprt_channel_p - pointer to the Transport Channel.
 *                 uiAttribId - Attribute Identifier.
 *                 attrib_data_p - Pointer to the Attribute Data.
 * Output params : NONE
 * Return value  : OF_SUCCESS - On success.
 *                 OF_FAILURE - on Failure.
 *****************************************************************************/
int32_t cm_tnsprt_set_channel_attribs (void * tnsprt_channel_p,
      struct cm_tnsprt_channel_attribs * attrib_p)
{
   int32_t return_value = OF_SUCCESS;
   unsigned char *msg_p = NULL;
   uint32_t uiMsgLen = 0;
   struct cm_msg_generic_header GenHdr;

   if (unlikely ((tnsprt_channel_p == NULL) || (attrib_p == NULL)))
   {
      CM_TRANS_DEBUG_PRINT ("Invalid Input");
      return OF_FAILURE;
   }

   of_memset (&GenHdr, 0, sizeof (struct cm_msg_generic_header));
   GenHdr.command_id = CM_COMMAND_SET_TRANSPORT_ATTRIBS;

   /* Send the struct cm_tnsprt_channel_attribs as is. */
   return_value = cm_tnsprt_send_message (tnsprt_channel_p,
         &GenHdr,
         (char *) attrib_p,
         sizeof (struct cm_tnsprt_channel_attribs));
   if (unlikely (return_value == OF_FAILURE))
   {
      CM_TRANS_DEBUG_PRINT ("Unable to send msg over Transport Channel");
      return OF_FAILURE;
   }

   /* Now receive the result */
   return_value = cm_tnsprt_recv_message (tnsprt_channel_p, &GenHdr, &msg_p, &uiMsgLen);
   if (unlikely ((return_value == OF_FAILURE) || (uiMsgLen != sizeof (struct cm_result))))
   {
      CM_TRANS_DEBUG_PRINT ("Unable to recv msg from Transport Channel");
      cm_tnsprt_free_message (msg_p); /*CID 357*/
      return OF_FAILURE;
   }

   return_value = ((struct cm_result *) msg_p)->result_code;
   cm_tnsprt_free_message (msg_p);

   return return_value;
}

/**
  \ingroup TRCHNLCAPI
  This API is used to send Data to through Transport channel.

  <b>Input Parameters: </b> \n
  <b><i>tnsprt_channel_p:</i></b> Pointer to the Transport channel structure..\n
  <b><i>gen_hdr_p:</i></b> Pointer to the Generic header structure.\n
  <b><i>input_buff_p:</i></b> Pointer to the Message Buffer. \n
  <b><i>input_buff_len_ui:</i></b>Message Buffer Length.

  <b>Output Parameters:</b> NONE \n

  <b>Return Value:</b>\n
  <b><i>OF_SUCCESS:</i></b> On Sending the Message buffer successfully .
  <b><i>OF_FAILURE:</i></b> On any failure.

*/

/******************************************************************************
 * Function Name : cm_tnsprt_send_message
 * Description   : This API is used to send the Message buffer across Transport 
 *                 Channel.
 * Input params  : sock_fd_i - Holds the Socket Handle.
 *                 pBuff - Pointer to the message buffer.
 *                 uiLen - Message buffer length.
 * Output params : NONE.
 * Return value  : OF_SUCCESS - On Sending the Message buffer successfully.
 *                 OF_FAILURE - On any failure.
 *****************************************************************************/
int32_t cm_tnsprt_send_message (void * tnsprt_channel_p,
      struct cm_msg_generic_header * gen_hdr_p,
      char * msg_p, uint32_t uiMsgLen)
{
   int32_t return_value;
   char *pBuf = NULL;
   char *pTmp = NULL;
   uint32_t uiTotalLen = 0;
   uint32_t uiAdminRoleLen, uiAdminNameLen;

   if(uiMsgLen == 0)
   {
      CM_TRANS_DEBUG_PRINT ("No message to send");
      return OF_FAILURE;
   }
   if (unlikely ((tnsprt_channel_p == NULL) || (gen_hdr_p == NULL) || (msg_p == NULL)))
   {
      CM_TRANS_DEBUG_PRINT ("Invalid Input");
      return OF_FAILURE;
   }

   //  uiTotalLen = sizeof (uint32_t) + uiMsgLen + sizeof (struct cm_msg_generic_header);
   uiTotalLen = uiOSIntSize_g + uiMsgLen + (6 * uiOSIntSize_g) + of_strlen(gen_hdr_p->admin_role) + of_strlen(gen_hdr_p->admin_name) ;


   /*
Fixme : If msg_p contains enough pre-overhead, we can just use it for 
Generic header. But how do we know whether there is a preoverhead
or not. Workout on that. Until then, we always allocate fresh 
buffer, and copy both generic header and the actual message into
it.
*/
   pBuf = (char *) cm_tnsprt_alloc_message (uiTotalLen);
   if (unlikely (pBuf == NULL))
   {
      CM_TRANS_DEBUG_PRINT ("Unable to allocate memory");
      return OF_FAILURE;
   }

   pTmp = pBuf;
   // pTmp =  of_mbuf_put_32 (pTmp, uiTotalLen);
   pBuf = (char*)of_mbuf_put_32 (pBuf, uiTotalLen);
   //  of_memcpy (pBuf + sizeof(uiTotalLen) , gen_hdr_p,
   //            sizeof (struct cm_msg_generic_header));

   //  of_memcpy (pBuf + 8 , gen_hdr_p,
   //            sizeof (struct cm_msg_generic_header));
   /* For MP to itcm communication, this may be requied in future*/

   /* Copy  command_id */
   pBuf = (char *)of_mbuf_put_32 (pBuf, gen_hdr_p->command_id);

   /* Copy  flags */
   pBuf = (char *)of_mbuf_put_32 (pBuf, gen_hdr_p->flags);

   /* Copy  sequence_id */
   pBuf = (char *)of_mbuf_put_32 (pBuf, gen_hdr_p->sequence_id);

   /* Copy  mgmt_engine_id */
   pBuf = (char *)of_mbuf_put_32 (pBuf, gen_hdr_p->mgmt_engine_id);

   uiAdminRoleLen=of_strlen(gen_hdr_p->admin_role);
   pBuf = (char *)of_mbuf_put_32 (pBuf, uiAdminRoleLen);
   /* Copy  admin_role */

   if ( uiAdminRoleLen > 0 )
   { 
      of_memcpy(pBuf, gen_hdr_p->admin_role, uiAdminRoleLen);
      pBuf += uiAdminRoleLen;
   }

   uiAdminNameLen=of_strlen(gen_hdr_p->admin_name);
   pBuf = (char *)of_mbuf_put_32 (pBuf,uiAdminNameLen);

   /* Copy  admin_name */
   if ( uiAdminNameLen > 0 )
   {
      of_memcpy(pBuf, gen_hdr_p->admin_name,  uiAdminNameLen);
      pBuf += uiAdminNameLen;
   }
   //pBuf += uiAdminNameLen;

   of_memcpy (pBuf , msg_p, uiMsgLen);
   //  of_memcpy (pBuf + 8 + sizeof (struct cm_msg_generic_header), msg_p,
   //            uiMsgLen);

   CM_TRANS_DEBUG_PRINT ("sending %d bytes of message",  uiTotalLen);

   /*verifying whether socket is valid */
   if(((struct cm_tnsprt_channel *) tnsprt_channel_p)->sock_fd_i == 0)
   { 
      CM_TRANS_DEBUG_PRINT ("Invalid Socket FD");
      cm_tnsprt_free_message (pTmp);
      return OF_FAILURE;
   }

   return_value =
      cm_socket_send (((struct cm_tnsprt_channel *) tnsprt_channel_p)->sock_fd_i, pTmp,
            uiTotalLen, MSG_NOSIGNAL);
   //  if (return_value <= 0)
   if(return_value == 0)
   {
      CM_TRANS_DEBUG_PRINT ("Transport Channel socket closed connection");
      cm_tnsprt_free_message (pTmp);
      return OF_FAILURE;
   }
   if(return_value < 0)
   {
      CM_TRANS_DEBUG_PRINT ("Unable to send message on Transport Channel");
      cm_tnsprt_free_message (pTmp);
      return OF_FAILURE;
   }

   cm_tnsprt_free_message (pTmp);
   return OF_SUCCESS;
}

/**
  \ingroup TRCHNLCAPI
  This API is used to receive message buffer through Transport channel.

  <b>Input Parameters: </b> \n
  <b><i>sock_fd_i:</i></b> Holds the Socket handle.\n
  <b><i>msg_len_ui_p:</i></b> Holds the allocated message buffer length.\n

  <b>Output Parameters:</b> NONE \n
  <b><i>gen_hdr_p:</i></b> Pointer to the Generic header structure.\n
  <b><i>msg_p:</i></b> pointer to message buffer.\n
  <b><i>msg_len_ui_p:</i></b> Holds the newly allocated message buffer length.\n

  <b>Return Value:</b>\n
  <b><i>OF_SUCCESS:</i></b> On receiving the message buffer successfully .
  <b><i>OF_FAILURE:</i></b> On any failure.

*/
/******************************************************************************
 * Function Name : cm_tnsprt_recv_message
 * Description   : This function is used to receive the data from the socket.
 *                 Checks whether the input buffer is having enough space to hold
 *                 the received message otherwise reallocates sufficient memory 
 *                 to hold the message.
 * Input params  : sock_fd_i - Socket handle
 *                 msg_len_ui_p - holds the message buffer length
 * Output params : gen_hdr_p - Generic header pointer. Used to hold the extracted
 *                           generic header data.
 *                 msg_p - Pointer to buffer, used to hold the received message.
 *                 msg_len_ui_p - used to hold the received message length.
 * Return value  : OF_SUCCESS - If message received successfully
 *                 OF_FAILURE - In any failure.
 *****************************************************************************/
int32_t cm_tnsprt_recv_message (void * tnsprt_channel_p,
      struct cm_msg_generic_header * gen_hdr_p,
      unsigned char ** p_msg_p, uint32_t * msg_len_ui_p)
{
   uint32_t uiTotalMsgLen = 0, uiMsgBufLen = 0;
   int32_t  iRcvdBytes = 0;
   char  *pBuf = NULL;
   char  aBuff[sizeof (uint32_t)] = { 0 };
   uint32_t tmp_msg_buf_len_ui=0;
   uint32_t lRetVal=0;
   uint32_t uiGenHdrLen =0;
   int32_t  iTotRecdBytes;
   struct cm_poll_info cm_je_fd_list_g[1]={};

   if (unlikely ((p_msg_p == NULL) || (msg_len_ui_p == NULL)))
   {
      CM_TRANS_DEBUG_PRINT ("Invalid Input");
      return OF_FAILURE;
   }

   if(((struct cm_tnsprt_channel *) tnsprt_channel_p)->sock_fd_i == 0)
   { 
      CM_TRANS_DEBUG_PRINT ("Invalid Socket FD");
      return OF_FAILURE;
   }

   CM_FDPOLL_ZERO (cm_je_fd_list_g, 1);
   cm_je_fd_list_g[0].fd_i = ((struct cm_tnsprt_channel *) tnsprt_channel_p)->sock_fd_i;
   cm_je_fd_list_g[0].interested_events = CM_FDPOLL_READ;
   cm_je_fd_list_g[0].returned_events = 0;
   if( ( (gen_hdr_p->mgmt_engine_id == CM_CLI_MGMT_ENGINE_ID) ||
            (gen_hdr_p->mgmt_engine_id == CM_HTTP_MGMT_ENGINE_ID))
         &&
         (gen_hdr_p->command_id == CM_COMMAND_LDSV_REQUEST)
     )
   {
      lRetVal = cm_fd_poll(1,cm_je_fd_list_g,120 * 1000); 
   }
   else
   {
      lRetVal = cm_fd_poll(1,cm_je_fd_list_g,5 * 1000); 
   }
   if(lRetVal < 0)
   { 
      CM_TRANS_DEBUG_PRINT ("FdPoll Returned Error :%d",lRetVal);
      return OF_FAILURE;
   }

   /* Just peek into buffer, and read the message length */
   /*  iRcvdBytes = cm_socket_recv (((struct cm_tnsprt_channel *) tnsprt_channel_p)->sock_fd_i,
       (char *) aBuff, CM_UINT32_SIZE, MSG_PEEK);
       if (iRcvdBytes != CM_UINT32_SIZE)
       {
       CM_TRANS_DEBUG_PRINT ("Unable to read the length field of msg");
       return OF_FAILURE;
       }
       */

   iRcvdBytes = cm_socket_recv (((struct cm_tnsprt_channel *) tnsprt_channel_p)->sock_fd_i,
         (char *) aBuff, CM_UINT_OS_SIZE, MSG_PEEK);
   if (iRcvdBytes != CM_UINT_OS_SIZE)
   {
      CM_TRANS_DEBUG_PRINT ("Unable to read the length field of msg");
      return OF_FAILURE;
   }

   uiTotalMsgLen = of_mbuf_get_32 (aBuff);
   CM_TRANS_DEBUG_PRINT ("Total Message Len %d",  uiTotalMsgLen);
#if 0
   if (uiTotalMsgLen < (sizeof (struct cm_msg_generic_header) + CM_UINT_OS_SIZE))
   {
      CM_TRANS_DEBUG_PRINT ("Partial message on socket");
      return OF_FAILURE;
   }

   /* Allocate the required buffer */
   //  uiMsgBufLen = uiTotalMsgLen - (sizeof (uint32_t) + sizeof (struct cm_msg_generic_header));
   uiMsgBufLen = uiTotalMsgLen - (CM_UINT_OS_SIZE + sizeof (struct cm_msg_generic_header));
   pBuf = (char *) cm_tnsprt_alloc_message (uiMsgBufLen);
   if (unlikely (pBuf == NULL))
   {
      CM_TRANS_DEBUG_PRINT ("Unable to allocate memory");
      return OF_FAILURE;
   }
#endif
   /* Now read the length field */
   //  iRcvdBytes = cm_socket_recv (((struct cm_tnsprt_channel *) tnsprt_channel_p)->sock_fd_i,
   //                              (char *) & uiTotalMsgLen, CM_UINT32_SIZE, 0);
   iRcvdBytes = cm_socket_recv (((struct cm_tnsprt_channel *) tnsprt_channel_p)->sock_fd_i,
         (char *) & uiTotalMsgLen, CM_UINT_OS_SIZE, 0);
   // if (iRcvdBytes != CM_UINT32_SIZE)
   if (iRcvdBytes != CM_UINT_OS_SIZE)
   {
      CM_TRANS_DEBUG_PRINT ("Unable to read msg length field");
      return OF_FAILURE;
   }
   uiTotalMsgLen = of_mbuf_get_32 ((char *) & uiTotalMsgLen);


   iRcvdBytes = cm_socket_recv (((struct cm_tnsprt_channel *) tnsprt_channel_p)->sock_fd_i,
         (char *) & uiGenHdrLen,  CM_UINT_OS_SIZE,0);
   if (iRcvdBytes != CM_UINT_OS_SIZE)
   {
      CM_TRANS_DEBUG_PRINT ("Unable to read the length field of msg");
      return OF_FAILURE;
   }

   uiGenHdrLen = of_mbuf_get_32((char *) & uiGenHdrLen);
   CM_TRANS_DEBUG_PRINT ("Total Generic Header Len %d",  uiGenHdrLen);
   /* Read the generic header message */
   if (uiTotalMsgLen < uiGenHdrLen + CM_UINT_OS_SIZE)
   {
      CM_TRANS_DEBUG_PRINT ("Partial message on socket");
      return OF_FAILURE;
   }

   /* Allocate the required buffer */
   //  uiMsgBufLen = uiTotalMsgLen - (sizeof (uint32_t) + sizeof (struct cm_msg_generic_header));
   uiMsgBufLen = uiTotalMsgLen - (2 * CM_UINT_OS_SIZE + uiGenHdrLen);
   pBuf = (char *) cm_tnsprt_alloc_message (uiMsgBufLen);
   if (unlikely (pBuf == NULL))
   {
      CM_TRANS_DEBUG_PRINT ("Unable to allocate memory");
      return OF_FAILURE;
   }
   iRcvdBytes = cm_socket_recv (((struct cm_tnsprt_channel *) tnsprt_channel_p)->sock_fd_i,
         (char *) gen_hdr_p,uiGenHdrLen,0);
   //sizeof (struct cm_msg_generic_header), 0);
   if (iRcvdBytes != uiGenHdrLen)
   {
      CM_TRANS_DEBUG_PRINT ("Unable to read generic header");
      CM_TRANS_DEBUG_PRINT ("RecdBytes is %d",  iRcvdBytes);
      cm_tnsprt_free_message (pBuf);
      return OF_FAILURE;
   }
   /* Read the actual message */

   tmp_msg_buf_len_ui=uiMsgBufLen;
   iTotRecdBytes=0;
   iRcvdBytes = cm_socket_recv (((struct cm_tnsprt_channel *) tnsprt_channel_p)->sock_fd_i,
         pBuf, tmp_msg_buf_len_ui, 0);
   if (iRcvdBytes < 0 )
   {
      CM_TRANS_DEBUG_PRINT ("Unable to read msg ");
      cm_tnsprt_free_message (pBuf);
      return OF_FAILURE;
   }
   /* Partial Message recieved */
   while (iRcvdBytes != tmp_msg_buf_len_ui)
   {
      CM_TRANS_DEBUG_PRINT ("Unable to read complete msg to be read %d.. actual RecdBytes is %d",
            tmp_msg_buf_len_ui, iRcvdBytes);
      iTotRecdBytes+= iRcvdBytes;
      tmp_msg_buf_len_ui = tmp_msg_buf_len_ui - iRcvdBytes;
      iRcvdBytes = cm_socket_recv (((struct cm_tnsprt_channel *) tnsprt_channel_p)->sock_fd_i,
            &pBuf[iTotRecdBytes], tmp_msg_buf_len_ui, 0);
      if (iRcvdBytes < 0 )
      {
         CM_TRANS_DEBUG_PRINT ("Unable to read msg ");
         cm_tnsprt_free_message (pBuf);
         return OF_FAILURE;
      }
   }

   *p_msg_p = pBuf;
   *msg_len_ui_p = uiMsgBufLen;
   return OF_SUCCESS;
}

/**
  \ingroup TRCHNLCAPI
  This API is used to allocate memory needed by Transport channel.

  <b>Input Parameters: </b> \n
  <b><i>size_ui:</i></b> Size of the memory that need to be allcated.\n

  <b>Output Parameters:</b> NONE \n

  <b>Return Value:</b>\n
  <b><i>Pointer to allocated memory:</i></b> In case of success.
  <b><i>NULL:</i></b> In case of failure.
  */

/******************************************************************************
 * Function Name : cm_tnsprt_alloc_message
 * Description   : This function is used to allocate memory, needed by
 *                 transport channel layer, and its users.
 * Input params  : size_ui - Size of the memory that need to be allocated.
 * Output params : None
 * Return value  : Pointer to the allocated memory in case of success.
 NULL, incase of failure.
 *****************************************************************************/
void *cm_tnsprt_alloc_message (uint32_t size_ui)
{
   /* In future, we can convert this into mempools */
   return of_calloc (1, size_ui);
}

/**
  \ingroup TRCHNLCAPI
  This API is used to free the memory that was previously allocated by Transport channel.

  <b>Input Parameters: </b> \n
  <b><i>msg_p:</i></b> Pointer to the memory that need to be freed.\n
  <b>Output Parameters:</b> NONE \n
  <b>Return Value:</b> NONE \n
  */

/******************************************************************************
 * Function Name : cm_tnsprt_free_message
 * Description   : This function is used to free the memory, which is
 *                 previously allocated by transport channel.
 * Input params  : msg_p - Pointer to the buffer that need to be freed.
 * Output params : None
 * Return value  : None
 *****************************************************************************/
void cm_tnsprt_free_message (void * msg_p)
{
   /* In future, we can convert this into mempools */
   if(msg_p != NULL)
   {
      of_free (msg_p);
      msg_p = NULL;
   }
}

#endif /*CM_TRANSPORT_CHANNEL_SUPPORT */
