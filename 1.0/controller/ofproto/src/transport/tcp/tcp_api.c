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

/* File  :      tcp_api.c
 * Author:      Atmaram G <atmaram.g@freescale.com>
 * Date:   08/23/2013
 * Description: TCP plugin APIs.
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
#include "of_tcp_proto.h"



/******************************************************************************
 * * * * * * * * * * * * Function Prototypes * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
/******************************************************************************
 * * * * * * * * * * * * Global Variables * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
extern uint32_t cntlr_conn_hash_table_init_value_g;

/******************************************************************************
 * * * * * * * * * * * * Function Definitions * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
int32_t tcp_transport_init (of_transport_proto_properties_t *transport_proto)
{

	int32_t               optval;
	int32_t flags;
	int32_t               status = OF_SUCCESS;
	int32_t               ret_val = OF_SUCCESS;
	tcp_proto_details_t *tcp_proto_details_p = NULL;
	struct sockaddr_in local_addr;


	OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"entered");

	do
	{
		if ( transport_proto == NULL )
		{
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"invalid input");
			status = OF_FAILURE;
			break;
		}
		tcp_proto_details_p = (tcp_proto_details_t *)transport_proto->proto_spec;

		/*Create TCP stream socket */
		transport_proto->fd = of_socket_create(transport_proto->proto);
		if(transport_proto->fd == OF_FAILURE)
		{
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Socket Create failure");
			status = OF_FAILURE;
			break;
		}

		OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"fd created %d", transport_proto->fd);
		/*Make socket to non-blocking mode*/
		if((flags = fcntl(transport_proto->fd, F_GETFL, 0)) < 0)
		{
			perror("get socket flags");
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"Socket set non-block failure %d", errno);
			status = OF_FAILURE;
			break;
		}

		if( fcntl( transport_proto->fd, F_SETFL, flags | O_NONBLOCK ) < 0 )
		{
			perror("set socket flags to NON BLOCK");
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Socket set non-block failure %d", errno);
			status = OF_FAILURE;
			break;
		}

		/* Set resuse option to socket*/
		setsockopt(transport_proto->fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

		/* Bind socket to the specified port*/
		memset((char*)&local_addr,0,sizeof(local_addr));
		local_addr.sin_family      = AF_INET;
		local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		local_addr.sin_port        = htons(tcp_proto_details_p->port_number);
		ret_val = bind(transport_proto->fd, (struct sockaddr *)&local_addr, sizeof(struct sockaddr_in));
		if(ret_val == OF_FAILURE)
		{
			perror("bind");
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Socket Bind failure errno=%d", errno);
			status = OF_FAILURE;
			break;
		}

		/* Set listen queue length to socket*/
		if(listen(transport_proto->fd, CNTLR_TCP_ACCEPT_Q_LEN) < 0)
		{
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Socket listen failure errno=%d", errno);
			perror("listen");
			status = OF_FAILURE;
			break;
		}


	}while(0);

	return status;
}

int32_t tcp_transport_open ( int32_t fd, of_conn_properties_t *conn_details_p)
{
	int32_t               optval;
	int32_t flags;
	int32_t               status = OF_SUCCESS;
	int32_t               ret_val = OF_SUCCESS;
	int32_t               conn_sock_fd  = OF_FAILURE;
	tcp_conn_details_t *tcp_conn_details_p = NULL;
	struct sockaddr_in    rmt_addr;
	int32_t               rmt_addr_len;

	OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"entered");

	tcp_conn_details_p = (tcp_conn_details_t *)calloc(1,sizeof(tcp_conn_details_t));

	do
	{
		if ( conn_details_p == NULL )
		{
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Invalid input");
			status = OF_FAILURE;
			break;
		}
		rmt_addr_len = sizeof(rmt_addr);
		if( (conn_sock_fd = accept4( fd, (struct sockaddr *)&rmt_addr, (socklen_t *)&rmt_addr_len, SOCK_NONBLOCK)) < 0 )
		{
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Error in accepting new connection error 0x%0", errno);
			status = OF_FAILURE;
			break;
		}

		tcp_conn_details_p->remote_ip.type    = CNTLR_IPV4_ADDR;
		tcp_conn_details_p->remote_ip.v4_val  = ntohl(rmt_addr.sin_addr.s_addr);
		tcp_conn_details_p->remote_port       = ntohs(rmt_addr.sin_port);
		conn_details_p->trnsprt_fd        = conn_sock_fd;
		conn_details_p->conn_spec_info    = tcp_conn_details_p;

	}while(0);

	return status;
}

int32_t tcp_transport_read_data (int32_t fd,
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

	OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"entered");

    flags = 0;
	do
	{
		if(length <= 0)
		{
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR, "invalid length, not reading");
			break;
		}

		len_recv = recv( fd,  msg, length, flags);
		if(len_recv == 0)
		{
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR, " Read Error, error =0x%x", errno);
			if(errno == EAGAIN)
			{
				status = CNTLR_CONN_NO_DATA;
				return status;
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
				return status;
			}
			/*TBD, Assumed some error cases will not occur, if occur, it is bug need to fix*/
			if( (errno == ENOTCONN) || (errno == ETIMEDOUT) || (errno == ECONNRESET) )
				*peer_closed_conn = TRUE;
			status = CNTLR_CONN_READ_ERROR;
		}
		else
		{ 
			*len_read=len_recv;
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"length to be read %d, lengh read %d", length, len_recv);
		}
	}while(0);

	return status;
}

int32_t tcp_transport_write_data ( int32_t fd,
		void *conn_spec_info_p,
		void            *msg,
		uint16_t                length,
		int32_t flags,
		uint16_t                *length_sent,
		uint8_t              *peer_closed_conn )
{
	int32_t                     ret_val = OF_SUCCESS;
	int32_t                     status = OF_SUCCESS;

	OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"entered");

	do
	{
		ret_val = send(fd,
				msg, length, flags);
		if(ret_val == -1)
		{
			perror("send");
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"Send Error = 0x%lx",errno);
			if(errno == ECONNRESET)
			{
				OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"conn reset by peer");
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

int32_t tcp_transport_close (int32_t fd, void *conn_spec_info_p)
{
	int32_t                     ret_val = OF_SUCCESS;
	int32_t                     status = OF_SUCCESS;

	shutdown(fd,SHUT_RDWR);
	OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"closing accepted fd %d", fd);
	close(fd);

	return status;
}

int32_t tcp_transport_deinit (int32_t fd)
{
	int32_t                     ret_val = OF_SUCCESS;
	int32_t                     status = OF_SUCCESS;
	OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"de-initializing");
	OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"closing listener fd %d", fd);
	close(fd);
	return status;
}

int32_t tcp_transport_get_hash_value (void *conn_spec_info_p, int32_t *hash_p)
{
	int32_t                     ret_val = OF_SUCCESS;
	int32_t                     status = OF_SUCCESS;
	int32_t hash;
	tcp_conn_details_t *tcp_conn_details_p = NULL;

	do
	{
		OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"finding hashvalue");
		tcp_conn_details_p = (tcp_conn_details_t *)conn_spec_info_p;
		if ( tcp_conn_details_p == NULL)
		{
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"invaid input");
			status=OF_FAILURE;
			break;
		}
		OF_CONN_TABLE_HASH(tcp_conn_details_p->remote_ip.v4_val,
				tcp_conn_details_p->remote_port,
				0,hash);
		*hash_p=hash;
	}while(0);

	return status;
}


int32_t tcp_transport_compare_conn_spec_info (void *conn_spec_info_one_p,void *conn_spec_info_two_p, int8_t *hit_p)
{
	int32_t                     ret_val = OF_SUCCESS;
	int32_t                     status = OF_SUCCESS;
	int32_t hit;
	tcp_conn_details_t *tcp_conn_details_one_p = (tcp_conn_details_t *)conn_spec_info_one_p;
	tcp_conn_details_t *tcp_conn_details_two_p = (tcp_conn_details_t *)conn_spec_info_two_p;

	do
	{
		OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"finding hashvalue");
		if ( tcp_conn_details_one_p == NULL || tcp_conn_details_two_p == NULL)
		{
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"invaid input");
			status=OF_FAILURE;
			break;
		}
		if ((tcp_conn_details_one_p->remote_ip.v4_val == tcp_conn_details_two_p->remote_ip.v4_val )
				&& (tcp_conn_details_one_p->remote_port == tcp_conn_details_two_p->remote_port ))
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

int32_t tcp_transport_compare_proto_spec_info (of_trnsprt_proto_info_t *trnsprt_proto_info_p,void *proto_spec_p , int8_t *hit_p)
{
	int32_t                     ret_val = OF_SUCCESS;
	int32_t                     status = OF_SUCCESS;
	int8_t hit;
	tcp_proto_details_t *tcp_proto_details_p =NULL;
	do
	{
		OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"COmpare proto spec");
		tcp_proto_details_p = (tcp_proto_details_t *)proto_spec_p;
		if ( tcp_proto_details_p == NULL)
		{
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"invaid inpute");
			status=OF_FAILURE;
			break;
		}
		 OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"trnsport port=%d tcp port=%d", 
                         trnsprt_proto_info_p->port_number,tcp_proto_details_p->port_number);
		if (tcp_proto_details_p->port_number == trnsprt_proto_info_p->port_number)
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

int32_t tcp_transport_alloc_and_set_proto_spec_info (of_trnsprt_proto_info_t *trnsprt_proto_info_p,void **proto_spec_pp)
{
	int32_t                     ret_val = OF_SUCCESS;
	int32_t                     status = OF_SUCCESS;
	int32_t hit;
	tcp_proto_details_t *tcp_proto_details_p = NULL;
	do
	{
		OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Alloc proto spec info");
		tcp_proto_details_p = (tcp_proto_details_t *)calloc(1,sizeof(tcp_proto_details_t));
		if ( tcp_proto_details_p == NULL)
		{
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Memory alloc faied");
			status=OF_FAILURE;
			break;
		}
		tcp_proto_details_p->port_number = trnsprt_proto_info_p->port_number;
		OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"port is %d",tcp_proto_details_p->port_number);
		*proto_spec_pp = tcp_proto_details_p;
	}while(0);

	return status;
}

int32_t tcp_transport_get_proto_spec_info (of_trnsprt_proto_info_t *trnsprt_proto_info_p,void *proto_spec_p)
{
	int32_t                     ret_val = OF_SUCCESS;
	int32_t                     status = OF_SUCCESS;
	int32_t hit;
	tcp_proto_details_t *tcp_proto_details_p = NULL;
	do
	{
		OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"get proto spec info");
		tcp_proto_details_p = (tcp_proto_details_t *)(proto_spec_p);
		if ( tcp_proto_details_p == NULL)
		{
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"invaid input");
			status=OF_FAILURE;
			break;
		}
		trnsprt_proto_info_p->port_number = tcp_proto_details_p->port_number;
	}while(0);

	return status;
}
