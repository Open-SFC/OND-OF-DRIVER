
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

/* File  :      tls_api.c
 * Author:      Atmaram G <atmaram.g@freescale.com>
 * Date:   08/23/2013
 * Description: TLS plugin APIs.
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
#include "of_tls_proto.h"



/******************************************************************************
 * * * * * * * * * * * * Global Variables * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
extern SSL_CTX *ctx;
extern uint32_t cntlr_conn_hash_table_init_value_g;


/******************************************************************************
 * * * * * * * * * * * * Function Definitions * * * * * * * * * * * * * * * * * *
 *******************************************************************************/



int32_t tls_transport_init (of_transport_proto_properties_t *transport_proto)
{

   int32_t               optval;
   int32_t flags;
   int32_t               status = OF_SUCCESS;
   int32_t               ret_val = OF_SUCCESS;
   tls_proto_details_t *tls_proto_details_p = NULL;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"entered");

   do
   {
      if ( transport_proto == NULL )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"invalid input");
         status = OF_FAILURE;
         break;
      }
      tls_proto_details_p = (tls_proto_details_t *)transport_proto->proto_spec;

      /*Create TCP stream socket */
      transport_proto->fd = of_socket_create(transport_proto->proto);
      if(transport_proto->fd == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Socket Create failure");
         status = OF_FAILURE;
         break;
      }

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

      /* Bind socket to the specified port*/
      {
         struct sockaddr_in local_addr;

         memset((char*)&local_addr,0,sizeof(local_addr));
         local_addr.sin_family      = AF_INET;
         local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
         local_addr.sin_port        = htons(tls_proto_details_p->port_number);
         ret_val = bind(transport_proto->fd, (struct sockaddr *)&local_addr, sizeof(struct sockaddr_in));
         if(ret_val == OF_FAILURE)
         {
            perror("bind");
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Socket Bind failure");
            status = OF_FAILURE;
            break;
         }
      }

      /* Set listen queue length to socket*/
      if(listen(transport_proto->fd, CNTLR_TCP_ACCEPT_Q_LEN) < 0)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Socket listen failure");
         perror("listen");
         status = OF_FAILURE;
         break;
      }

   }while(0);

   return status;

}

int32_t tls_transport_open ( int32_t fd, of_conn_properties_t *conn_details_p)
{
   int32_t               optval;
   int32_t flags;
   int32_t               status = OF_SUCCESS;
   int32_t               ret_val = OF_SUCCESS;
   int32_t               conn_sock_fd  = OF_FAILURE;
   struct sockaddr_in    rmt_addr;
   tls_conn_details_t *tls_conn_details_p;
   int32_t               rmt_addr_len;
   uint64_t                     ssl_match_dp_id;
   cntlr_ssl_info_t            *pssl_info = NULL;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"entered");

   do
   {
      if ( conn_details_p == NULL )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Invalid input");
         status = OF_FAILURE;
         break;
      }

      tls_conn_details_p = (tls_conn_details_t*)calloc(1,sizeof(tls_conn_details_t));

      pssl_info = &tls_conn_details_p->pssl_info;
      /* validating ssl info */
      pssl_info = (cntlr_ssl_info_t *)malloc(sizeof (cntlr_ssl_info_t));
      pssl_info->ctx = ctx;
      pssl_info->ssl = NULL;
      ret_val = of_ssl_get_sslobject_for_client(fd,
            pssl_info->ctx,&(pssl_info->ssl));
      if(ret_val == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"Failed to get ssl object");
         free(pssl_info);
         pssl_info = NULL;
         status = OF_FAILURE;
         break;
      }
      ret_val = SSL_accept(pssl_info->ssl);
      status = SSL_get_error(pssl_info->ssl,ret_val);
      if(status != SSL_ERROR_NONE)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,
               "SSL handshake has failed");
         of_ssl_shutdown_connection(pssl_info->ssl, NULL /* ctx */);
         free(pssl_info);
         pssl_info = NULL;
         status = OF_FAILURE;
         break;
      }
      ret_val = of_ssl_verify_peer_subjectname(pssl_info->ssl, &(ssl_match_dp_id));
      if(ret_val == OF_FAILURE)
      {   
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,
               "SSL: Subject Name found in peer certificate is invalid,SSL handshake has failed");
         of_ssl_shutdown_connection(pssl_info->ssl,NULL /* ctx */);
         free(pssl_info);
         pssl_info = NULL;
         status = OF_FAILURE;
         break;
      }
      conn_details_p->conn_spec_info =tls_conn_details_p;

   }while(0);

   return status;
}

int32_t tls_transport_read_data (int32_t fd,
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
   int32_t              bytes_recv = 0, err;
   int32_t len=length;
   void *msg_tmp_p;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"entered");

   tls_conn_details_t *tls_conn_details_p = (tls_conn_details_t *)(conn_spec_info_p);

   cntlr_ssl_info_t            *pssl_info = NULL;


   pssl_info = &tls_conn_details_p>pssl_info;


   do
   {
      msg_tmp_p = msg;
      do
      {
         if(length <= 0)
         {
            break;
         }
         bytes_recv = SSL_read(pssl_info->ssl, msg_tmp_p, len);
         if(bytes_recv >  0)
         {
            len_recv += bytes_recv;
            msg_tmp_p += bytes_recv;
            len   -= bytes_recv;
         }
         else if(bytes_recv <=  0)
         {
            if(bytes_recv ==  0)
            {
               *peer_closed_conn = TRUE;
               status = CNTLR_CONN_READ_ERROR;
               break;
            }
            err = SSL_get_error(pssl_info->ssl, bytes_recv);
            OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR, "SSL Read Error, error =0x", err);
            *peer_closed_conn = TRUE;
            status = CNTLR_CONN_READ_ERROR;
            break;
         }
      }while(SSL_pending(pssl_info->ssl));

      *len_read=bytes_recv;

   }while(0);

   return status;
}

int32_t tls_transport_write_data ( int32_t fd,
      void *conn_spec_info_p,
      void            *msg,
      uint16_t                length,
      int32_t flags,
      uint16_t                *length_sent,
      uint8_t              *peer_closed_conn )
{
   int32_t                     ret_val = OF_SUCCESS;
   int32_t                     status = OF_SUCCESS;


   tls_conn_details_t *tls_conn_details_p = (tls_conn_details_t *)conn_spec_info_p;
   cntlr_ssl_info_t            *pssl_info = NULL;


   pssl_info = &tls_conn_details_p->pssl_info;


   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"entered");

   do
   {
      if(pssl_info == NULL )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,  "connection specific info is NULL");
         status = OF_FAILURE;
         *length_sent = 0;
         break;
      }

      ret_val = SSL_write(pssl_info->ssl,msg, length);
      if(ret_val <= 0)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,  "SSL Write Error");
         perror("send");
         /* TBD: Need to verify */
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

int32_t tls_transport_close (int32_t fd, void *conn_spec_info_p)
{
   int32_t                     ret_val = OF_SUCCESS;
   int32_t                     status = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"entered");

   tls_conn_details_t *tls_conn_details_p = (tls_conn_details_t *)conn_spec_info_p;
   cntlr_ssl_info_t            *pssl_info = NULL;


   pssl_info = &tls_conn_details_p->pssl_info;


   do
   {
      if(pssl_info == NULL )
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,  "connection specific info is NULL");
         status = OF_FAILURE;
         break;
      }
      if(pssl_info)
      {
         if(pssl_info->ssl)
            of_ssl_shutdown_connection(pssl_info->ssl, NULL /* ctx */);
         free(pssl_info);
         break;
      }
      shutdown(fd,SHUT_RDWR);
      close(fd);
   }while(0);
   return status;
}

int32_t tls_transport_deinit (int32_t fd)
{
   int32_t                     ret_val = OF_SUCCESS;
   int32_t                     status = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"de-initializing");
   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"closing listener fd %d", fd);
   close(fd);

}


int32_t tls_transport_get_hash_value (void *conn_spec_info, int32_t *hash_p)
{
   int32_t                     ret_val = OF_SUCCESS;
   int32_t                     status = OF_SUCCESS;
   int32_t hash;
   tls_conn_details_t *tls_conn_details_p = (tls_conn_details_t *)conn_spec_info;

   do
   {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"finding hashvalue");
      if ( tls_conn_details_p == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"invaid inpute");
         status=OF_FAILURE;
         break;
      }
      OF_CONN_TABLE_HASH(tls_conn_details_p->remote_ip.v4_val,
            tls_conn_details_p->remote_port,
            0,hash);
      *hash_p=hash;
   }while(0);

   return status;
}
int32_t tls_transport_compare_conn_spec_info (void *conn_spec_info_one,void *conn_spec_info_two, int8_t *hit_p)
{
   int32_t                     ret_val = OF_SUCCESS;
   int32_t                     status = OF_SUCCESS;
   int32_t hit;
   tls_conn_details_t *tls_conn_details_one_p = (tls_conn_details_t *)conn_spec_info_one;
   tls_conn_details_t *tls_conn_details_two_p = (tls_conn_details_t *)conn_spec_info_two;

   do
   {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"finding hashvalue");
      if ( tls_conn_details_one_p == NULL || tls_conn_details_two_p == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"invaid inpute");
         status=OF_FAILURE;
         break;
      }
      if ((tls_conn_details_one_p->remote_ip.v4_val == tls_conn_details_two_p->remote_ip.v4_val )
            && (tls_conn_details_one_p->remote_port == tls_conn_details_two_p->remote_port )){
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

int32_t tls_transport_compare_proto_spec_info (of_trnsprt_proto_info_t *trnsprt_proto_info_p,void *proto_spec , int8_t *hit_p)
{
   int32_t                     ret_val = OF_SUCCESS;
   int32_t                     status = OF_SUCCESS;
   int32_t hit;
   do
   {
      tls_proto_details_t *tls_proto_details = (tls_proto_details_t *)proto_spec;
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"COmpare proto spec");
      if ( tls_proto_details == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"invaid inpute");
         status=OF_FAILURE;
         break;
      }
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"port_number are %d , %d ",tls_proto_details->port_number,trnsprt_proto_info_p->port_number);
      if (tls_proto_details->port_number == trnsprt_proto_info_p->port_number)
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

int32_t tls_transport_alloc_and_set_proto_spec_info (of_trnsprt_proto_info_t *trnsprt_proto_info_p,void **proto_spec_p)
{
   int32_t                     ret_val = OF_SUCCESS;
   int32_t                     status = OF_SUCCESS;
   int32_t hit;
   tls_proto_details_t *tls_proto_details_p = NULL;
   do
   {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Alloc proto spec info");
      tls_proto_details_p = (tls_proto_details_t *)calloc(1,sizeof(tls_proto_details_t));
      if ( tls_proto_details_p == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Memory alloc faied");
         status=OF_FAILURE;
         break;
      }
      tls_proto_details_p->port_number = trnsprt_proto_info_p->port_number;
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"port is %d",tls_proto_details_p->port_number);
      *proto_spec_p = tls_proto_details_p;
   }while(0);

   return status;
}

int32_t tls_transport_get_proto_spec_info (of_trnsprt_proto_info_t *trnsprt_proto_info_p,void *proto_spec_p)
{
   int32_t                     ret_val = OF_SUCCESS;
   int32_t                     status = OF_SUCCESS;
   int32_t hit;
   do
   {
      tls_proto_details_t *tls_proto_details_p = (tls_proto_details_t *)(proto_spec_p);
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"get proto spec info");
      if ( tls_proto_details_p == NULL)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"invaid inpute");
         status=OF_FAILURE;
         break;
      }
      trnsprt_proto_info_p->port_number = tls_proto_details_p->port_number;
   }while(0);

   return status;
}

