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

/* File  :      udp_init.c
 * Author:      Atmaram G <atmaram.g@freescale.com>
 * Date:   08/23/2013
 * Description: UDP Init.
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

extern cntlr_info_t  cntlr_info;

struct of_transprt_proto_callbacks  udp_proto_callbacks=
{
   udp_transport_init,
   udp_transport_open,
   udp_transport_write_data,
   udp_transport_read_data,
   udp_transport_close,
   udp_transport_deinit,
   udp_transport_get_hash_value,
   udp_transport_compare_conn_spec_info,
   udp_transport_compare_proto_spec_info,
   udp_transport_alloc_and_set_proto_spec_info,
   udp_transport_get_proto_spec_info

};

/******************************************************************************
 * * * * * * * * * * * * Function Definitions * * * * * * * * * * * * * * * * * *
 *******************************************************************************/


int32_t of_transport_proto_init()
{

   int32_t ret_val = OF_SUCCESS;
   int32_t status = OF_SUCCESS;
   of_trnsprt_proto_info_t  transprt_proto_info={};

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"UDP Protocol Library Init");

   do
   {
      ret_val=cntlr_register_udp_proto();
      if(ret_val != OF_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"register udp proto   - Failure");
         status = OF_FAILURE;
         break;
      }
      ret_val=udp_db_init();
      if(ret_val != OF_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"register udp database   - Failure");
         status = OF_FAILURE;
         break;
      }
   }while(0);

   return;
}


int32_t cntlr_register_udp_proto()
{
   int32_t ret_val = OF_SUCCESS;
   int32_t status = OF_SUCCESS;
   of_trnsprt_proto_info_t  transprt_proto_info={};

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"entered");
   do
   {
      transprt_proto_info.proto = OF_IPPROTO_UDP;
      transprt_proto_info.transport_type = CNTLR_TRANSPORT_TYPE_UDP;
      if ( cntlr_info.cntlr_portno != 0 )
         transprt_proto_info.port_number            = cntlr_info.cntlr_portno;
      else
         transprt_proto_info.port_number            =  OF_CONN_DFLT_PORT;
//      transprt_proto_info.fd_type =   OF_TRANSPORT_PROTO_FD_TYPE_SHARED;

      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"UDP portno %d", transprt_proto_info.port_number);
      ret_val = cntlr_register_transport_proto(&transprt_proto_info, &udp_proto_callbacks);
      if(ret_val != OF_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"add transport proto  - Failure");
         status = OF_FAILURE;
         break;
      }       

   }while(0);

   return status;
}

int32_t cntlr_unregister_udp_proto()
{
   int32_t ret_val = OF_SUCCESS;
   int32_t status = OF_SUCCESS;
   of_trnsprt_proto_info_t  transprt_proto_info={};

   OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"entered");
   do
   {
      /* un register here*/
      ret_val=  cntlr_delete_transport_by_proto(CNTLR_TRANSPORT_TYPE_UDP);
      if(ret_val != OF_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,"delete transport proto  - UDP -Failed");
         status = OF_FAILURE;
         break;
      }       
   }while(0);

   return status;
}
