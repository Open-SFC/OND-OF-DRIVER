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
 * * * * * * * * * * * * Global Variables * * * * * * * * * * * * * * * * * *
 *******************************************************************************/

extern cntlr_info_t  cntlr_info;


struct of_transprt_proto_callbacks  tcp_proto_callbacks=
{
   tcp_transport_init,
   tcp_transport_open,
   tcp_transport_write_data,
   tcp_transport_read_data,
   tcp_transport_close,
   tcp_transport_deinit,
   tcp_transport_get_hash_value,
   tcp_transport_compare_conn_spec_info,
   tcp_transport_compare_proto_spec_info,
   tcp_transport_alloc_and_set_proto_spec_info,
   tcp_transport_get_proto_spec_info
   		
};

/******************************************************************************
 * * * * * * * * * * * * Function Definitions * * * * * * * * * * * * * * * * * *
 *******************************************************************************/


int32_t of_transport_proto_init()
{

   int32_t ret_val = OF_SUCCESS;
   int32_t status = OF_SUCCESS;
   of_trnsprt_proto_info_t  transprt_proto_info={};

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"TCP Protocol Library Init");

   do
   {
      ret_val=cntlr_register_tcp_proto();
      if(ret_val != OF_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"register tcp proto   - Failure");
         status = OF_FAILURE;
         break;
      }
   }while(0);

   return;
}


int32_t cntlr_register_tcp_proto()
{
   int32_t ret_val = OF_SUCCESS;
   int32_t status = OF_SUCCESS;
   of_trnsprt_proto_info_t  transprt_proto_info={};

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"entered");
   do
   {
      transprt_proto_info.proto = OF_IPPROTO_TCP;
      transprt_proto_info.transport_type = CNTLR_TRANSPORT_TYPE_TCP;
      if ( cntlr_info.cntlr_portno != 0 )
         transprt_proto_info.port_number            = cntlr_info.cntlr_portno;
      else
         transprt_proto_info.port_number            =  OF_CONN_DFLT_PORT;
    //  transprt_proto_info.fd_type =   OF_TRANSPORT_PROTO_FD_TYPE_SHARED_AND_DEDICATED;

      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"TCP portno %d", transprt_proto_info.port_number);
      ret_val = cntlr_register_transport_proto(&transprt_proto_info, &tcp_proto_callbacks);
      if(ret_val != OF_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"add transport proto  - Failure");
         status = OF_FAILURE;
         break;
      }       

   }while(0);

   return status;
}

int32_t cntlr_unregister_tcp_proto()
{
   int32_t ret_val = OF_SUCCESS;
   int32_t status = OF_SUCCESS;
   of_trnsprt_proto_info_t  transprt_proto_info={};

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"entered");
   do
   {
      /* un register here*/
      ret_val=  cntlr_delete_transport_by_proto(CNTLR_TRANSPORT_TYPE_TCP);
      if(ret_val != OF_SUCCESS)         
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"delete transport proto  - UDP -Failed");
         status = OF_FAILURE;
         break;
      }       

   }while(0);

   return status;
}
