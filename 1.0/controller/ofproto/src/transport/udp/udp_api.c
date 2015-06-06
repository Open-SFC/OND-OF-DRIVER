/**
 * Copyright (c) 2012, 2013  Freescale.
 *  
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/

/* File  :      udp_api.c
 * Author:      Atmaram G <atmaram.g@freescale.com>
 * Date:   08/23/2013
 * Description: UDP plugin APIs.
 */

/******************************************************************************
 * * * * * * * * * * * * Include Files * * * * * * * * * * * * * * * * * *
 *******************************************************************************/


#include "controller.h"
#include "cntlr_epoll.h"
#include "cntrl_queue.h"
#include "of_utilities.h"
#include "of_msgapi.h"
#include "of_multipart.h"
#include "dprm.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "cntlr_event.h"
#include "dprmldef.h"
#include "cntlr_xtn.h"
#include "timerapi.h"
#include "of_proto.h"
#include "of_udp_proto.h"


/******************************************************************************
 * * * * * * * * * * * * Global Variables * * * * * * * * * * * * * * * * * *
 *******************************************************************************/


extern uint32_t cntlr_conn_hash_table_init_value_g;


/******************************************************************************
 * * * * * * * * * * * * Function Definitions * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
int32_t udp_transport_init (of_transport_proto_properties_t *transport_proto)
{

   int32_t               optval;
   int32_t flags;
   int32_t               status = OF_SUCCESS;
   int32_t               ret_val = OF_SUCCESS;
   udp_proto_details_t *udp_proto_details_p = NULL;
   struct sockaddr_in local_addr;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"entered");

   do
   {
      /*Create TCP stream socket */
      transport_proto->fd = of_socket_create(transport_proto->proto);
      if(transport_proto->fd == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Socket Create failure");
         status = OF_FAILURE;
         break;
      }
      udp_proto_details_p = (udp_proto_details_t *)transport_proto->proto_spec;

      /*Make socket to non-blocking mode*/
      if((flags = fcntl(transport_proto->fd, F_GETFL, 0)) < 0)
      {
         perror("get socket flags");
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"Socket set non-block failure");
         status = OF_FAILURE;
         break;
      }

      if( fcntl( transport_proto->fd, F_SETFL, flags | O_NONBLOCK ) < 0 )
      {
         perror("set socket flags to NON BLOCK");
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Socket set non-block failure");
         status = OF_FAILURE;
         break;
      }

      /* Set resuse option to socket*/
      setsockopt(transport_proto->fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));


      memset((char*)&local_addr,0,sizeof(local_addr));
      local_addr.sin_family      = AF_INET;
      local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
      local_addr.sin_port        = htons(udp_proto_details_p->port_number);
      ret_val = bind(transport_proto->fd, (struct sockaddr *)&local_addr, sizeof(struct sockaddr_in));
      if(ret_val == OF_FAILURE)
      {
         perror("bind");
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Socket Bind failure");
         status = OF_FAILURE;
         break;
      }
#if 0
      /* Set listen queue length to socket*/
      if(listen(transport_proto->fd, CNTLR_TCP_ACCEPT_Q_LEN) < 0)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Socket listen failure, error 0x%x",errno);
         perror("listen");
         //     status = OF_FAILURE;
         break;
      }
#endif

   }while(0);

   return status;
}

int32_t udp_transport_open ( int32_t fd, of_conn_properties_t *conn_details_p)
{
   int32_t               status = OF_SUCCESS;
   int32_t               ret_val = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"entered");

   do
   {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"nothing to do here");

   }while(0);

   return status;
}

int32_t udp_transport_read_data (int32_t fd,
      void *conn_spec_info_p,
      void                *msg,
      uint16_t               length,
      int32_t flags,
      uint16_t               *len_read,
      uint8_t              *peer_closed_conn,
      void **opaque_pp)

{
   int32_t              len_recv = 0;
   int32_t              status     = CNTLR_CONN_READ_SUCCESS;
   udp_conn_details_t *udp_conn_details_p = NULL;
   struct sockaddr_in remote_address;
   int32_t  addr_len = sizeof(remote_address);
   int32_t retval;
   struct udp_db_entry *db_entry_p=NULL;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"entered");

   do
   {
      if(length <= 0)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR, "invalid length, not reading");
         break;
      }

      if (conn_spec_info_p)
      {
         if ( flags != 0 )
         {
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR, "not handling flags");
            status=OF_FAILURE;
            break;
         }
         udp_conn_details_p = (udp_conn_details_t *)conn_spec_info_p;
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_INFO, "Remote IP 0x%x port %d length to be read %d",  udp_conn_details_p->remote_ip.v4_val, udp_conn_details_p->remote_port, length);

         retval=udp_hash_db_get_db_entry(udp_conn_details_p->remote_ip, udp_conn_details_p->remote_port, &db_entry_p);
         if ( retval != OF_SUCCESS)
         {
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_INFO, "no db entry found");
            retval=udp_hash_db_add_db_entry(udp_conn_details_p->remote_ip, udp_conn_details_p->remote_port, &db_entry_p);
            if ( retval != OF_SUCCESS)
            {
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR, "DB entry creation failed");
               status=OF_FAILURE;
               break;
            }
            len_recv = recvfrom(fd, db_entry_p->read_buffer,UDP_MAX_BUFFER_LENGTH, flags,(struct sockaddr *)&remote_address,
                  (socklen_t *)&addr_len);
            if(len_recv == 0)
            {
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR, "Read Error, error =0x%x", errno);
               if(errno == EAGAIN)
               {
                  status = CNTLR_CONN_NO_DATA;
                  break;
               }
               *peer_closed_conn = TRUE;
               status = CNTLR_CONN_READ_ERROR;
               break;
            }
            else if(len_recv ==  -1)
            {
               perror("read");
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Read Error, error =0x%x", errno);
               if(errno == EAGAIN)
               {
                  status = CNTLR_CONN_NO_DATA;
                  break;
               }
               /*TBD, Assumed some error cases will not occur, if occur, it is bug need to fix*/
               if( (errno == ENOTCONN) || (errno == ETIMEDOUT) || (errno == ECONNRESET) )
                  *peer_closed_conn = TRUE;
               status = CNTLR_CONN_READ_ERROR;
               break;
            }
            db_entry_p->msg_len = len_recv;
         }
         else
         {
            if ( db_entry_p->processed_b == TRUE)
            {
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "already processed ...some problem here");
               status = CNTLR_CONN_READ_ERROR;
               break;
            }

         }
         if (db_entry_p->msg_len > (db_entry_p->read_len + length))
         {
            memcpy(msg, db_entry_p->read_buffer + db_entry_p->read_len, length);
            db_entry_p->read_len += length;
            *len_read=length;
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "read lengh %d total read length %d msg len %d", length, db_entry_p->read_len, db_entry_p->msg_len);
         }
         else
         {
            if (db_entry_p->msg_len == (db_entry_p->read_len + length))
               *len_read=length;
            else
               *len_read=  (db_entry_p->msg_len  - db_entry_p->read_len);
            memcpy(msg, db_entry_p->read_buffer + db_entry_p->read_len, *len_read);
            db_entry_p->read_len += *len_read;
            db_entry_p->processed_b = TRUE;
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "read complete mesg... lengh %d total read length %d msg len %d", length, db_entry_p->read_len, db_entry_p->msg_len);
            retval=udp_hash_db_del_db_entry(udp_conn_details_p->remote_ip, udp_conn_details_p->remote_port);
         }

      }
      else
      {
         len_recv = recvfrom(fd, msg, length, flags,(struct sockaddr *)&remote_address,
               (socklen_t *)&addr_len);
         if(len_recv == 0)
         {
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR, "Read Error, error =0x%x", errno);
            if(errno == EAGAIN)
            {
               status = CNTLR_CONN_NO_DATA;
               break;
            }
            *peer_closed_conn = TRUE;
            status = CNTLR_CONN_READ_ERROR;
         }
         else if(len_recv ==  -1)
         {
            perror("read");
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Read Error, error =0x%x", errno);
            if(errno == EAGAIN)
            {
               status = CNTLR_CONN_NO_DATA;
               break;
            }
            /*TBD, Assumed some error cases will not occur, if occur, it is bug need to fix*/
            if( (errno == ENOTCONN) || (errno == ETIMEDOUT) || (errno == ECONNRESET) )
               *peer_closed_conn = TRUE;
            status = CNTLR_CONN_READ_ERROR;
         }
         else
         {
            if(conn_spec_info_p == NULL)
            {
               udp_conn_details_p = (udp_conn_details_t *)calloc(1,sizeof(udp_conn_details_t));
               udp_conn_details_p->remote_ip.type    = CNTLR_IPV4_ADDR;
               udp_conn_details_p->remote_ip.v4_val  = ntohl(remote_address.sin_addr.s_addr);
               udp_conn_details_p->remote_port = ntohs(remote_address.sin_port);
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR, "Remote IP 0x%x port %d",  udp_conn_details_p->remote_ip.v4_val, udp_conn_details_p->remote_port);
               *opaque_pp = udp_conn_details_p;
            }
            else
            {
               *len_read=len_recv;
               OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"length to be read %d, lengh read %d", length, len_recv);
            }
         }
      }
   }while(0);

   return status;
}

int32_t udp_transport_write_data ( int32_t fd,
      void *conn_spec_info_p,
      void            *msg,
      uint16_t                length,
      int32_t flags,
      uint16_t                *length_sent,
      uint8_t              *peer_closed_conn )

{
   int32_t                     ret_val = OF_SUCCESS;
   int32_t                     status = OF_SUCCESS;

   struct sockaddr_in remote_address;
   udp_conn_details_t *udp_conn_details_p = NULL;


   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"entered");
   udp_conn_details_p = (udp_conn_details_t *)conn_spec_info_p;

   remote_address.sin_family = AF_INET;

   remote_address.sin_addr.s_addr = htonl(udp_conn_details_p->remote_ip.v4_val);
   remote_address.sin_port = htons(udp_conn_details_p->remote_port);


   do
   {
      if(fd == -1)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"fd already closed");
         status=OF_FAILURE;
         break;
      }
      ret_val = sendto(fd,
            msg, length, flags,
            (struct sockaddr *)&remote_address, sizeof(remote_address));
      if(ret_val == -1)
      {
         perror("send");
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,
               "Send Error = 0x%lx",errno);
         if(errno == ECONNRESET)
         {
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN," conn reset by peer");
            *peer_closed_conn = TRUE;
         }
         status = OF_FAILURE;
         *length_sent = 0;
      }
      else
      {
         *length_sent = ret_val;
      }

   }while(0);

   return status;
}

int32_t udp_transport_close (int32_t fd,
      void *conn_spec_info_p)
{
   int32_t                     status = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"not closing any fd");

   return status;
}

int32_t udp_transport_deinit (int32_t fd)
{

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"de-initializing");
   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"closing fd %d", fd);
   shutdown(fd,SHUT_RDWR);
   close(fd);
}

int32_t udp_transport_get_hash_value (void *conn_spec_info, int32_t *hash_p)
{
   int32_t                     ret_val = OF_SUCCESS;
   int32_t                     status = OF_SUCCESS;
   int32_t hash;
   udp_conn_details_t *udp_conn_details_p = (udp_conn_details_t *)conn_spec_info;

   do
   {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"finding hashvalue");
      if ( udp_conn_details_p == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"invaid inpute");
         status=OF_FAILURE;
         break;
      }
      OF_CONN_TABLE_HASH(udp_conn_details_p->remote_ip.v4_val,
            udp_conn_details_p->remote_port,
            0,hash);
      *hash_p=hash;
   }while(0);

   return status;
}


int32_t udp_transport_compare_conn_spec_info (void *conn_spec_info_one,void *conn_spec_info_two, int8_t *hit_p)
{
   int32_t                     ret_val = OF_SUCCESS;
   int32_t                     status = OF_SUCCESS;
   int32_t hit = COMP_NO_MATCH;
   udp_conn_details_t *udp_conn_details_one_p = (udp_conn_details_t *)conn_spec_info_one;
   udp_conn_details_t *udp_conn_details_two_p = (udp_conn_details_t *)conn_spec_info_two;

   do
   {
      if ( udp_conn_details_one_p == NULL || udp_conn_details_two_p == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"invaid input");
         status=OF_FAILURE;
         break;
      }

      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG, "1. Remote IP 0x%x port %d  2. Remote IP 0x%x port %d",  udp_conn_details_one_p->remote_ip.v4_val, udp_conn_details_one_p->remote_port, udp_conn_details_two_p->remote_ip.v4_val, udp_conn_details_two_p->remote_port);

      if ((udp_conn_details_one_p->remote_ip.v4_val == udp_conn_details_two_p->remote_ip.v4_val )
            && (udp_conn_details_one_p->remote_port == udp_conn_details_two_p->remote_port ))
      {
         hit = COMP_MATCH;
      }
      *hit_p=hit;
   }while(0);

   return status;
}

int32_t udp_transport_compare_proto_spec_info (of_trnsprt_proto_info_t *trnsprt_proto_info_p,void *proto_spec , int8_t *hit_p)
{
   int32_t                     ret_val = OF_SUCCESS;
   int32_t                     status = OF_SUCCESS;
   int8_t hit;
   do
   {
      udp_proto_details_t *udp_proto_details = (udp_proto_details_t *)proto_spec;
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"COmpare proto spec");
      if ( udp_proto_details == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"invaid inpute");
         status=OF_FAILURE;
         break;
      }
      if (udp_proto_details->port_number == trnsprt_proto_info_p->port_number)
      {
         hit = COMP_MATCH;
      }
      else
      {
         hit  = COMP_NO_MATCH;
      }
      *hit_p=hit;
   }while(0);

   return status;
}

int32_t udp_transport_alloc_and_set_proto_spec_info (of_trnsprt_proto_info_t *trnsprt_proto_info_p,void **proto_spec)
{
   int32_t                     ret_val = OF_SUCCESS;
   int32_t                     status = OF_SUCCESS;
   int32_t hit;
   udp_proto_details_t *udp_proto_details = NULL;
   do
   {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Alloc proto spec info");
      udp_proto_details = (udp_proto_details_t *)calloc(1,sizeof(udp_proto_details_t));
      if ( udp_proto_details == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Memory alloc faied");
         status=OF_FAILURE;
         break;
      }
      udp_proto_details->port_number = trnsprt_proto_info_p->port_number;
      *proto_spec = udp_proto_details;
   }while(0);

   return status;
}

int32_t udp_transport_get_proto_spec_info (of_trnsprt_proto_info_t *trnsprt_proto_info_p,void *proto_spec)
{
   int32_t                     ret_val = OF_SUCCESS;
   int32_t                     status = OF_SUCCESS;
   int32_t hit;
   do
   {
      udp_proto_details_t *udp_proto_details = (udp_proto_details_t *)(proto_spec);
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"get proto spec info");
      if ( udp_proto_details == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"invaid inpute");
         status=OF_FAILURE;
         break;
      }
      trnsprt_proto_info_p->port_number = udp_proto_details->port_number;
   }while(0);

   return status;
}

