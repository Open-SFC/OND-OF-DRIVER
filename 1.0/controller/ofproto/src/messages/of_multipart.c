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

/* File       : of_multipart.c
 * Author     : Rajesh Madabushi <rajesh.madabushi@freescale.com>
 * Description: 
 * This file contains defintions multipart msg handling routines. 
 */

/* INCLUDE_START ************************************************************/
#include "controller.h"
#include "of_utilities.h"
#include "of_multipart.h"
#include "of_msgapi.h"
#include "cntrl_queue.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "cntlr_xtn.h"
#if 0
#include "cntlr_event.h"
#include "dprmldef.h"
#endif
/* INCLUDE_END **************************************************************/

void 
of_handle_port_status_response(cntlr_channel_info_t       *p_channel,
                               struct dprm_datapath_entry *p_datapath,
                               struct ofp_header          *p_msg_hdr,
                               cntlr_transactn_rec_t      *p_xtn_rec,
                               uint8_t                     more_data)
{
   uint32_t                   port_no;
   uint32_t                   no_of_ports;
   uint8_t                    is_last_port;
   struct of_msg             *msg;
   struct ofp_port           *port_desc_read;
   struct ofi_port_desc_info  port_desc;
   uint16_t                   len_rcvd;
   int32_t                    retval = OF_SUCCESS;
   int32_t                    status = OF_SUCCESS;


   no_of_ports = OF_MULTIPART_RPLY_MSG_NO_OF_DESC(p_msg_hdr,struct ofp_port);
   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "No of ports %d",no_of_ports);
   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Header length %d multipart struct len %d  ofp port  len %d\r\n",
                                  ntohs(p_msg_hdr->length),sizeof(struct ofp_multipart_reply), sizeof(struct ofp_port));

   for(port_no = 1; port_no <= no_of_ports;port_no++)
   {
       /* Allocate requried message buffer required to read one port description */
       msg = ofu_alloc_message(p_msg_hdr->type,sizeof(struct ofp_port));
       if(msg == NULL)
       {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d message allocation failed - port description multipart reply,portno %d\r\n",
                                                                        __FUNCTION__,__LINE__,port_no);
         status = OF_FAILURE;
         break;
       }

       port_desc_read = (struct ofp_port *)(msg->desc.start_of_data);    
    
       retval = cntlr_read_msg_recvd_from_dp(p_channel,(uint8_t*)port_desc_read,sizeof(struct ofp_port),&len_rcvd);
       if(retval != CNTLR_CONN_READ_SUCCESS)
       {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error in reading port descption info for port %d \r\n",
                                                         __FUNCTION__,__LINE__,port_no);
          status = OF_FAILURE;
          msg->desc.free_cbk(msg); 
          break;
       }

       strncpy(port_desc.name,port_desc_read->name,OFP_MAX_PORT_NAME_LEN);
       memcpy(port_desc.hw_addr,port_desc_read->hw_addr,OFP_ETH_ALEN);
       port_desc.port_no    = ntohl(port_desc_read->port_no);
       port_desc.config     = ntohl(port_desc_read->config);
       port_desc.state      = ntohl(port_desc_read->state);
       port_desc.curr       = ntohl(port_desc_read->curr);
       port_desc.advertised = ntohl(port_desc_read->advertised);
       port_desc.supported  = ntohl(port_desc_read->supported);
       port_desc.peer       = ntohl(port_desc_read->peer);
       port_desc.curr_speed = ntohl(port_desc_read->curr_speed);
       port_desc.max_speed  = ntohl(port_desc_read->max_speed);

       if(port_no == no_of_ports)
          is_last_port = TRUE;
       else
          is_last_port = FALSE;

       ((of_port_desc_cbk_fn)(p_xtn_rec->callback_fn))(msg,
                                                     controller_handle,
                                                     p_datapath->datapath_handle,
                                                     p_datapath->domain->domain_handle,
                                                     port_desc.port_no,
                                                     p_xtn_rec->app_pvt_arg1,
                                                     p_xtn_rec->app_pvt_arg2,
                                                     OF_RESPONSE_STATUS_SUCCESS,
                                                     &port_desc,
                                                     is_last_port);
   }

   if(status == OF_FAILURE)
   {
       ((of_port_desc_cbk_fn)(p_xtn_rec->callback_fn))(NULL,
                                                     controller_handle,
                                                     p_datapath->datapath_handle,
                                                     p_datapath->domain->domain_handle,
                                                     0, 
                                                     NULL,
                                                     NULL,
                                                     OF_RESPONSE_STATUS_ERROR,
                                                     NULL,
                                                     TRUE);
   }
}

void 
of_handle_flow_entry_stats_response(cntlr_channel_info_t       *p_channel,
                                    struct dprm_datapath_entry *p_datapath,
                                    struct ofp_header          *p_msg_hdr,
                                    cntlr_transactn_rec_t      *p_xtn_rec)
{
   uint32_t                   port_no;
   uint32_t                   no_of_ports;
   uint8_t                    is_last_port;
   struct of_msg             *msg;
   struct ofp_flow_stats      
   struct ofp_port           *port_desc_read;
   struct ofi_port_desc_info  port_desc;
   uint16_t                   len_rcvd;
   int32_t                    retval = OF_SUCCESS;
   int32_t                    status = OF_SUCCESS;


   no_of_ports = OF_MULTIPART_RPLY_MSG_NO_OF_DESC(p_msg_hdr,struct ofp_port);
   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"No of ports %d\r\n",no_of_ports);
   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Header length %d multipart struct len %d  ofp port  len %d\r\n",
                                  ntohs(p_msg_hdr->length),sizeof(struct ofp_multipart_reply), sizeof(struct ofp_port));

   for(port_no = 1; port_no <= no_of_ports;port_no++)
   {
       /* Allocate requried message buffer required to read one port description */
       msg = ofu_alloc_message(p_msg_hdr->type,sizeof(struct ofp_port));
       if(msg == NULL)
       {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d message allocation failed - port description multipart reply,portno %d\r\n",
                                                                        __FUNCTION__,__LINE__,port_no);
         status = OF_FAILURE;
         break;
       }

       port_desc_read = (struct ofp_port *)(msg->desc.start_of_data);    
    
       retval = cntlr_read_msg_recvd_from_dp(p_channel,(uint8_t*)port_desc_read,sizeof(struct ofp_port),&len_rcvd);
       if(retval != CNTLR_CONN_READ_SUCCESS)
       {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error in reading port descption info for port %d \r\n",
                                                         __FUNCTION__,__LINE__,port_no);
          status = OF_FAILURE;
          msg->desc.free_cbk(msg); 
          break;
       }


