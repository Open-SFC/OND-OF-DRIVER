/**
 *  Copyright (c) 2012, 2013  Freescale.
 *   
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 **/
/*
 * File:        virtio_api.c
 * 
 * Description: 
 * This file contains the APIs required for Virtio serial transport plugin. These APIs are
 * Expected by Transport base module of OF controller.
 *
 *
 * Authors:     Laxman B <laxman.b@freescale.com>
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
#include "of_virtio_proto.h"



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
int32_t virtio_transport_init (of_transport_proto_properties_t *transport_proto)
{

	int32_t               optval;
	int32_t flags;
	int32_t               status = OF_SUCCESS;
	int32_t               ret_val = OF_SUCCESS;
	virtio_proto_details_t *virtio_proto_details_p = NULL;


	OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"entered");

	do
	{
		if ( transport_proto == NULL )
		{
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"invalid input");
			status = OF_FAILURE;
			break;
		}
		virtio_proto_details_p = (virtio_proto_details_t *)transport_proto->proto_spec;

		/*Create TCP stream socket */
		transport_proto->fd = open(virtio_proto_details_p->dev_name, O_RDWR);
		if(transport_proto->fd == OF_FAILURE)
		{
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Socket Create failure");
			status = OF_FAILURE;
			break;
		}
	}while(0);

	return status;
}

int32_t virtio_transport_open ( int32_t fd, of_conn_properties_t *conn_details_p)
{
	int32_t               optval;
	int32_t flags;
	int32_t               status = OF_SUCCESS;
	int32_t               ret_val = OF_SUCCESS;
	int32_t               conn_sock_fd  = OF_FAILURE;
	struct sockaddr_in    rmt_addr;
	virtio_conn_details_t *virtio_conn_deatils_p = NULL;
	int32_t               rmt_addr_len;

	OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"entered");

	do
	{
		OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"nothing to do here");

	}while(0);

	return status;
}

int32_t virtio_transport_read_data (int32_t fd,
		void *conn_spec_info,
		void                *buffer,
		uint16_t               length,
		int32_t flags,
		uint16_t               *len_read,
		uint8_t              *peer_closed_conn,
		void **opaque_pp)

{
	int32_t              len_recv = 0;
	int32_t              status     = CNTLR_CONN_READ_SUCCESS;
	char dev_name[32]={};
	char temp[32]={};
	virtio_conn_details_t *virtio_conn_details_p = NULL;

	OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"entered");

	do
	{
		//LAX-TBR
#if 0
		if( length >=1056) {
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR, "length >= 1056");
			status = CNTLR_CONN_NO_DATA;
			break;
		}
#endif
		if(length <= 0)
		{
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR, "invalid length, not reading");
			break;
		}

		len_recv = read(fd, buffer, length);
		OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR, " Read Length/len_recv =%d/%d", length, len_recv);
		// CNTLR_XPRT_PRINT_PKT_DATA((char *)buffer, len_recv);
		if(len_recv == 0)
		{
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR, " Read Error, error =0x%x", errno);
			if(errno == EAGAIN)
			{
				status = CNTLR_CONN_NO_DATA;
				break;
			}
			// *peer_closed_conn = TRUE; /* setting this flag to TRUE...  channel will be closed by base module */
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
			{ //*peer_closed_conn = TRUE;
			}
			status = CNTLR_CONN_READ_ERROR;
		} else { //LAX-VIO
			*len_read = len_recv;
			if(conn_spec_info == NULL)
			{
				virtio_conn_details_p = (virtio_conn_details_t*)calloc(1,sizeof(virtio_conn_details_t));
				sprintf(temp,"/proc/self/fd/%d",fd);
				readlink(temp,dev_name,MAX_DEV_NAME_SIZE);
				strcpy(virtio_conn_details_p->dev_name,dev_name);
				*opaque_pp = virtio_conn_details_p;
			}

			break;    
		}

	}while(0);

	return status;
}

int32_t virtio_transport_write_data ( int32_t fd,
		void *conn_spec_info,
		void            *msg,
		uint16_t                length,
		int32_t flags,
		uint16_t                *length_sent,
		uint8_t              *peer_closed_conn )

{
	int32_t                     ret_val = OF_SUCCESS;
	int32_t                     status = OF_SUCCESS;
	virtio_conn_details_t *virtio_conn_deatils_p = NULL;


	OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"entered");

	do
	{
		if(fd == -1)
		{
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"fd already closed");
			status=OF_FAILURE;
			break;
		}
		ret_val = write(fd,  msg, length);
		if(ret_val == -1)
		{
			perror("send");
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,
					"Send Error = 0x%lx",errno);
			if(errno == ECONNRESET)
			{
				OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN," conn reset by peer");
				// *peer_closed_conn = TRUE;
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

int32_t virtio_transport_close (int32_t fd, 
		void *conn_spec_info_p)
{
	int32_t                     ret_val = OF_SUCCESS;
	int32_t                     status = OF_SUCCESS;

	OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"closing fd %d", fd);
	close(fd);
	return status;
}

int32_t virtio_transport_deinit (int32_t fd)
{
	int32_t                     ret_val = OF_SUCCESS;
	int32_t                     status = OF_SUCCESS;

	OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"de-initializing... nothing to do here");
}


int32_t virtio_transport_get_hash_value (void *conn_spec_info, int32_t *hash_p)
{
	int32_t                     ret_val = OF_SUCCESS;
	int32_t                     status = OF_SUCCESS;
	int32_t hash;
	char name_hash[12];
	virtio_conn_details_t *virtio_conn_details_p = (virtio_conn_details_t *)conn_spec_info;

	do
	{
		memset(name_hash,0,12);
		OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"finding hashvalue");
		if ( virtio_conn_details_p == NULL)
		{
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"invaid inpute");
			status=OF_FAILURE;
			break;
		}
                strncpy(name_hash,virtio_conn_details_p->dev_name,12);
                OF_CONN_TABLE_HASH(*(uint32_t *)(name_hash +0),
                                *(uint32_t *)(name_hash +4),
                                *(uint32_t *)(name_hash +8),hash);

		*hash_p = 0;
		status = OF_FAILURE;
	}while(0);

	return status;
}


int32_t virtio_transport_compare_conn_spec_info (void *conn_spec_info_one,void *conn_spec_info_two, int8_t *hit_p)
{
	int32_t                     ret_val = OF_SUCCESS;
	int32_t                     status = OF_SUCCESS;
	int32_t hit = COMP_NO_MATCH;
	virtio_conn_details_t *virtio_conn_details_one_p = (virtio_conn_details_t *)conn_spec_info_one;
	virtio_conn_details_t *virtio_conn_details_two_p = (virtio_conn_details_t *)conn_spec_info_two;

	do
	{
		OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"finding hashvalue");
		if ( virtio_conn_details_one_p == NULL || virtio_conn_details_two_p == NULL)
		{
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"invaid inpute");
			status=OF_FAILURE;
			break;
		}
		if (strcmp(virtio_conn_details_one_p->dev_name,virtio_conn_details_two_p->dev_name) == 0){
			hit = COMP_MATCH;
		}
		else {
			hit  = COMP_NO_MATCH;
		}
		*hit_p=hit;

	}while(0);

	return status;
}


int32_t virtio_transport_compare_proto_spec_info (of_trnsprt_proto_info_t *trnsprt_proto_info_p,void *proto_spec , int8_t *hit_p)
{
	int32_t                     ret_val = OF_SUCCESS;
	int32_t                     status = OF_SUCCESS;
	int8_t hit = COMP_NO_MATCH; 
	do
	{
		virtio_proto_details_t *virtio_proto_details = (virtio_proto_details_t *)proto_spec;
		OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"COmpare proto spec");
		if ( virtio_proto_details == NULL)
		{
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"invaid inpute");
			status=OF_FAILURE;
			break;
		}
		if (!strcmp(virtio_proto_details->dev_name,trnsprt_proto_info_p->dev_name))
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


int32_t virtio_transport_alloc_and_set_proto_spec_info (of_trnsprt_proto_info_t *trnsprt_proto_info_p,void **proto_spec_p)
{
	int32_t                     ret_val = OF_SUCCESS;
	int32_t                     status = OF_SUCCESS;
	int32_t hit;
	virtio_proto_details_t *virtio_proto_details_p = NULL;
	do
	{	
		OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Alloc proto spec info");
		virtio_proto_details_p = (virtio_proto_details_t *)calloc(1,sizeof(virtio_proto_details_t));
		if ( virtio_proto_details_p == NULL)
		{
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"Memory alloc faied");
			status=OF_FAILURE;
			break;
		}
		strcpy(virtio_proto_details_p->dev_name,trnsprt_proto_info_p->dev_name);
		OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"virtio dev name %s",virtio_proto_details_p->dev_name);
		*proto_spec_p = virtio_proto_details_p;
	}while(0);

	return status;
}


int32_t virtio_transport_get_proto_spec_info (of_trnsprt_proto_info_t *trnsprt_proto_info_p,void *proto_spec_p)
{
	int32_t                     ret_val = OF_SUCCESS;
	int32_t                     status = OF_SUCCESS;
	int32_t hit;
	do
	{
		virtio_proto_details_t *virtio_proto_details_p = (virtio_proto_details_t *)(proto_spec_p);
		OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"get proto spec info");
		if ( virtio_proto_details_p == NULL)
		{
			OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"invaid inpute");
			status=OF_FAILURE;
			break;
		};
		 strcpy(trnsprt_proto_info_p->dev_name,virtio_proto_details_p->dev_name);
	}while(0);

	return status;
}

