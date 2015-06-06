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

/* File:        cntlr_app.c
 * Author:      Rajesh Madabushi <rajesh.madabushi@freescale.com>
 * Description: This file contains definitions of controller working as application.
 *              Here controller is also working as one application.
 *              It uses North Bound APIs interface to communicate with the switch.
 */

#include "controller.h"
#include "of_utilities.h"
#include "of_multipart.h"
#include "of_msgapi.h"
#include "cntrl_queue.h"
#include "of_msgpool.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "cntlr_epoll.h"
#include "cntlr_event.h"
#include "dprmldef.h"
#include "of_msgapi.h"
#include "multi_part.h"
#include "fsl_ext.h"

#define OPENFLOW_IPSEC_SUPPORT
//#define of_debug printf

#define OF_DEFAULT_SW_MISS_SEND_LEN  128
//Defining of_error_printable_string_data 

of_error_strings_t of_error_printable_string_data[] = {

   {OFPET_HELLO_FAILED,OFPHFC_INCOMPATIBLE,
      "Hello protocol failed : No compatibile version."},   
   {OFPET_HELLO_FAILED,OFPHFC_EPERM,
      "Hello protocol failed : Permissions error."},   


   {OFPET_BAD_REQUEST,OFPBRC_BAD_VERSION,
      "Request was not understood:ofp_header version not supported."},
   {OFPET_BAD_REQUEST,OFPBRC_BAD_TYPE,
      "Request was not understood :  ofp_header.type not supported."},
   {OFPET_BAD_REQUEST,OFPBRC_BAD_MULTIPART,
      "Request was not understood : ofp_multipart_request.type not supported."},
   {OFPET_BAD_REQUEST,OFPBRC_BAD_EXPERIMENTER,
      "Request was not understood : Experimenter id not supported  (in ofp_experimenter_header or ofp_multipart_request or * ofp_multipart_reply)."},
   {OFPET_BAD_REQUEST,OFPBRC_BAD_EXP_TYPE,
      "Request was not understood : Experimenter type not supported."},
   {OFPET_BAD_REQUEST,OFPBRC_EPERM,
      "Request was not understood : Permissions error."},
   {OFPET_BAD_REQUEST,OFPBRC_BAD_LEN,
      "Request was not understood : Wrong request length for type."},
   {OFPET_BAD_REQUEST,OFPBRC_BUFFER_EMPTY,
      "Request was not understood : Specified buffer has already been used."},
   {OFPET_BAD_REQUEST,OFPBRC_BUFFER_UNKNOWN,
      "Request was not understood : Specified buffer does not exist."},
   {OFPET_BAD_REQUEST,OFPBRC_BAD_TABLE_ID,
      "Request was not understood : Specified table-id invalid or does not  exist."},
   {OFPET_BAD_REQUEST,OFPBRC_IS_SLAVE,
      "Request was not understood : Denied because controller is slave."},
   {OFPET_BAD_REQUEST,OFPBRC_BAD_PORT,
      "Request was not understood : Invalid port."},
   {OFPET_BAD_REQUEST,OFPBRC_BAD_PACKET,
      "Request was not understood : Invalid packet in packet-out."},
   {OFPET_BAD_REQUEST,OFPBRC_MULTIPART_BUFFER_OVERFLOW,
      "Request was not understood : ofp_multipart_request overflowed the assigned buffer."},


   {OFPET_BAD_ACTION,OFPBAC_BAD_TYPE,
      " Error in action description : Unknown action type."},
   {OFPET_BAD_ACTION,OFPBAC_BAD_LEN,
      " Error in action description : Length problem in actions."},
   {OFPET_BAD_ACTION,OFPBAC_BAD_EXPERIMENTER,
      " Error in action description : Unknown experimenter id specified."},
   {OFPET_BAD_ACTION,OFPBAC_BAD_EXP_TYPE,
      " Error in action description : Unknown action for experimenter id."},
   {OFPET_BAD_ACTION,OFPBAC_BAD_OUT_PORT,
      " Error in action description : Problem validating output port."},
   {OFPET_BAD_ACTION,OFPBAC_BAD_ARGUMENT,
      " Error in action description : Bad action argument."},
   {OFPET_BAD_ACTION,OFPBAC_EPERM,
      " Error in action description :  Permissions error. "},
   {OFPET_BAD_ACTION,OFPBAC_TOO_MANY,
      " Error in action description : Can't handle this many actions."},
   {OFPET_BAD_ACTION,OFPBAC_BAD_QUEUE,
      " Error in action description : Problem validating output queue."},
   {OFPET_BAD_ACTION,OFPBAC_BAD_OUT_GROUP,
      " Error in action description : Invalid group id in forward action."},
   {OFPET_BAD_ACTION,OFPBAC_MATCH_INCONSISTENT,
      " Error in action description :  Action can't apply for this match, or Set-Field missing prerequisite."},
   {OFPET_BAD_ACTION,OFPBAC_UNSUPPORTED_ORDER,
      " Error in action description : Action order is unsupported for the action list in an Apply-Actions instruction"},
   {OFPET_BAD_ACTION,OFPBAC_BAD_TAG,
      " Error in action description : Actions uses an unsupported tag/encap."},
   {OFPET_BAD_ACTION,OFPBAC_BAD_SET_TYPE,
      " Error in action description : Unsupported type in SET_FIELD action."},
   {OFPET_BAD_ACTION,OFPBAC_BAD_SET_LEN,
      " Error in action description : Length problem in SET_FIELD action."},
   {OFPET_BAD_ACTION,OFPBAC_BAD_SET_ARGUMENT,
      " Error in action description : Bad argument in SET_FIELD action. "},


   {OFPET_BAD_INSTRUCTION,OFPBIC_UNKNOWN_INST,
      "Error in instruction list : Unknown instruction."},
   {OFPET_BAD_INSTRUCTION,OFPBIC_UNSUP_INST,
      "Error in instruction list : Switch or table does not support the instruction"},
   {OFPET_BAD_INSTRUCTION,OFPBIC_BAD_TABLE_ID,
      "Error in instruction list :  Invalid Table-ID specified."},
   {OFPET_BAD_INSTRUCTION,OFPBIC_UNSUP_METADATA,
      "Error in instruction list : Metadata value unsupported by datapath."},
   {OFPET_BAD_INSTRUCTION,OFPBIC_UNSUP_METADATA_MASK,
      "Error in instruction list : Metadata mask value unsupported by datapath."},
   {OFPET_BAD_INSTRUCTION,OFPBIC_BAD_EXPERIMENTER,
      "Error in instruction list : Unknown experimenter id specified."},
   {OFPET_BAD_INSTRUCTION,OFPBIC_BAD_EXP_TYPE,
      "Error in instruction list : Unknown instruction for experimenter id."},
   {OFPET_BAD_INSTRUCTION,OFPBIC_BAD_LEN,
      "Error in instruction list : Length problem in instructions."},
   {OFPET_BAD_INSTRUCTION,OFPBIC_EPERM,
      "Error in instruction list : Permissions error."},


   {OFPET_BAD_MATCH,OFPBMC_BAD_TYPE,
      "Error in Match : Unsupported match type specified by the match "},
   {OFPET_BAD_MATCH,OFPBMC_BAD_LEN,
      "Error in Match : Length problem in match."},
   {OFPET_BAD_MATCH,OFPBMC_BAD_TAG,
      "Error in Match : Match uses an unsupported tag/encap."},
   {OFPET_BAD_MATCH,OFPBMC_BAD_DL_ADDR_MASK,
      "Error in Match : Unsupported datalink addr mask - switch does not support arbitrary datalink address mask."},
   {OFPET_BAD_MATCH,OFPBMC_BAD_NW_ADDR_MASK,
      "Error in Match : Unsupported network addr mask - switch does not support arbitrary network address mask."},
   {OFPET_BAD_MATCH,OFPBMC_BAD_WILDCARDS,
      "Error in Match : Unsupported combination of fields masked or omitted in the match."},
   {OFPET_BAD_MATCH,OFPBMC_BAD_FIELD,
      "Error in Match : Unsupported field type in the match."},
   {OFPET_BAD_MATCH,OFPBMC_BAD_VALUE,
      "Error in Match : Unsupported value in a match field."},
   {OFPET_BAD_MATCH,OFPBMC_BAD_MASK,
      "Error in Match : Unsupported mask specified in the match, field is not dl-address or nw-address."},
   {OFPET_BAD_MATCH,OFPBMC_BAD_PREREQ,
      "Error in Match : A prerequisite was not met."},
   {OFPET_BAD_MATCH,OFPBMC_DUP_FIELD,
      "Error in Match : A field type was duplicated."},
   {OFPET_BAD_MATCH,OFPBMC_EPERM,
      "Error in Match : Permissions error."},


   {OFPET_FLOW_MOD_FAILED,OFPFMFC_UNKNOWN,
      "Problem Modifying the Flow : Unspecified error."},
   {OFPET_FLOW_MOD_FAILED,OFPFMFC_TABLE_FULL,
      "Problem Modifying the Flow :  Flow not added because table was full."},
   {OFPET_FLOW_MOD_FAILED,OFPFMFC_BAD_TABLE_ID,
      "Problem Modifying the Flow : Table does not exist"},
   {OFPET_FLOW_MOD_FAILED,OFPFMFC_OVERLAP,
      "Problem Modifying the Flow : Attempted to add overlapping flow with CHECK_OVERLAP flag set."},
   {OFPET_FLOW_MOD_FAILED,OFPFMFC_EPERM,
      "Problem Modifying the Flow : Permissions error."},
   {OFPET_FLOW_MOD_FAILED,OFPFMFC_BAD_TIMEOUT,
      "Problem Modifying the Flow : Flow not added because of unsupported idle/hard timeout."},
   {OFPET_FLOW_MOD_FAILED,OFPFMFC_BAD_COMMAND,
      "Problem Modifying the Flow : Unsupported or unknown command."},
   {OFPET_FLOW_MOD_FAILED,OFPFMFC_BAD_FLAGS,
      "Problem Modifying the Flow : Unsupported or unknown flags"},


   {OFPET_GROUP_MOD_FAILED,OFPGMFC_GROUP_EXISTS,
      "Probelm Modifying Group Entry : Group not added because a group ADD attempted to replace an already-present group."},
   {OFPET_GROUP_MOD_FAILED,OFPGMFC_INVALID_GROUP,
      "Probelm Modifying Group Entry : Group not added because Group specified is invalid."},
   {OFPET_GROUP_MOD_FAILED,OFPGMFC_WEIGHT_UNSUPPORTED,
      "Probelm Modifying Group Entry : Switch does not support unequal load sharing with select groups. "},
   {OFPET_GROUP_MOD_FAILED,OFPGMFC_OUT_OF_GROUPS,
      "Probelm Modifying Group Entry : The group table is full."},
   {OFPET_GROUP_MOD_FAILED,OFPGMFC_OUT_OF_BUCKETS,
      "Probelm Modifying Group Entry : The maximum number of action buckets for a group has been exceeded."},
   {OFPET_GROUP_MOD_FAILED,OFPGMFC_CHAINING_UNSUPPORTED,
      "Probelm Modifying Group Entry :  Switch does not support groups that forward to groups."},
   {OFPET_GROUP_MOD_FAILED,OFPGMFC_WATCH_UNSUPPORTED,
      "Probelm Modifying Group entry : This group cannot watch the watch_port or watch_group specified."},
   {OFPET_GROUP_MOD_FAILED,OFPGMFC_LOOP,
      "Probelm Modifying Group Entry : Group entry would cause a loop."},
   {OFPET_GROUP_MOD_FAILED,OFPGMFC_UNKNOWN_GROUP,
      "Probelm Modifying Group Entry : Group not modified because a group MODIFY attempted to modify a   non-existent group."},
   {OFPET_GROUP_MOD_FAILED,OFPGMFC_CHAINED_GROUP,
      "Probelm Modifying Group Entry : Group not deleted because another group is forwarding to it."},
   {OFPET_GROUP_MOD_FAILED,OFPGMFC_BAD_TYPE,
      "Probelm Modifying Group Entry : Unsupported or unknown group type."},
   {OFPET_GROUP_MOD_FAILED,OFPGMFC_BAD_COMMAND,
      "Probelm Modifying Group Entry : Unsupported or unknown command."},
   {OFPET_GROUP_MOD_FAILED,OFPGMFC_BAD_BUCKET,
      "Probelm Modifying Group Entry : Error in bucket."},
   {OFPET_GROUP_MOD_FAILED,OFPGMFC_BAD_WATCH,
      "Probelm Modifying Group Entry : Error in watch port/group."},
   {OFPET_GROUP_MOD_FAILED,OFPGMFC_EPERM,
      "Probelm Modifying Group Entry : Permissions error."},


   {OFPET_PORT_MOD_FAILED,OFPPMFC_BAD_PORT,
      "Port mod Request failed : Specified port number does not exist."},
   {OFPET_PORT_MOD_FAILED,OFPPMFC_BAD_HW_ADDR,
      "Port mod Request failed : Specified hardware address does not match the port number."},
   {OFPET_PORT_MOD_FAILED,OFPPMFC_BAD_CONFIG,
      "Port mod Request failed : Specified config is invalid."},
   {OFPET_PORT_MOD_FAILED,OFPPMFC_BAD_ADVERTISE,
      "Port mod Request failed : Specified advertise is invalid"},
   {OFPET_PORT_MOD_FAILED,OFPPMFC_EPERM,
      "Port mod Request failed : Permissions error."},


   {OFPET_TABLE_MOD_FAILED,OFPTMFC_BAD_TABLE,
      "Table mod request failed : Specified table does not exist."},
   {OFPET_TABLE_MOD_FAILED,OFPTMFC_BAD_CONFIG,
      "Table mod request failed : Specified config is invalid."},
   {OFPET_TABLE_MOD_FAILED,OFPTMFC_EPERM,
      "Table mod request failed : Permissions error."},

   {OFPET_QUEUE_OP_FAILED,OFPQOFC_BAD_PORT,
      "Quere operation failed : Invalid port (or port does not exist)."},
   {OFPET_QUEUE_OP_FAILED,OFPQOFC_BAD_QUEUE,
      "Quere operation failed : Queue does not exist."},
   {OFPET_QUEUE_OP_FAILED,OFPQOFC_EPERM,
      "Quere operation failed : Permissions error. "},


   {OFPET_SWITCH_CONFIG_FAILED,OFPSCFC_BAD_FLAGS,
      "Switch config request failed : Specified flags is invalid."},
   {OFPET_SWITCH_CONFIG_FAILED,OFPSCFC_BAD_LEN,
      "Switch config request failed : Specified len is invalid."},
   {OFPET_SWITCH_CONFIG_FAILED,OFPSCFC_EPERM,
      "Switch config request failed : Permissions error."},


   {OFPET_ROLE_REQUEST_FAILED,OFPRRFC_STALE,
      "Controller role request failed : Stale Message: old generation_id."},
   {OFPET_ROLE_REQUEST_FAILED,OFPRRFC_UNSUP,
      "Controller role request failed : Controller role change unsupported."},
   {OFPET_ROLE_REQUEST_FAILED,OFPRRFC_BAD_ROLE,
      "Controller role request failed : Invalid role."},



   {OFPET_METER_MOD_FAILED,OFPMMFC_UNKNOWN,
      "Error in meter : Unspecified error."},
   {OFPET_METER_MOD_FAILED,OFPMMFC_METER_EXISTS,
      "Error in meter : Meter not added because a Meter ADD attempted to replace an existing Meter."},
   {OFPET_METER_MOD_FAILED,OFPMMFC_INVALID_METER,
      "Error in meter : Meter not added because Meter specified  is invalid."},
   {OFPET_METER_MOD_FAILED,OFPMMFC_UNKNOWN_METER,
      "Error in meter : Meter not modified because a Meter MODIFY attempted to modify a non-existent Meter."},
   {OFPET_METER_MOD_FAILED,OFPMMFC_BAD_COMMAND,
      "Error in meter : Unsupported or unknown command."},
   {OFPET_METER_MOD_FAILED,OFPMMFC_BAD_FLAGS,
      "Error in meter :  Flag configuration unsupported."},
   {OFPET_METER_MOD_FAILED,OFPMMFC_BAD_RATE,
      "Error in meter : Rate unsupported."},
   {OFPET_METER_MOD_FAILED,OFPMMFC_BAD_BURST,
      "Error in meter : Burst size unsupported."},
   {OFPET_METER_MOD_FAILED,OFPMMFC_BAD_BAND,
      "Error in meter : Band unsupported."},
   {OFPET_METER_MOD_FAILED,OFPMMFC_BAD_BAND_VALUE,
      "Error in meter : Band value unsupported."},
   {OFPET_METER_MOD_FAILED,OFPMMFC_OUT_OF_METERS,
      "Error in meter : No more meters available."},
   {OFPET_METER_MOD_FAILED,OFPMMFC_OUT_OF_BANDS,
      "Error in meter : The maximum number of properties for a meter has been exceeded."},


   {OFPET_TABLE_FEATURES_FAILED,OFPTFFC_BAD_TABLE,
      "Setting table features failed : Specified table does not exist. "},
   {OFPET_TABLE_FEATURES_FAILED,OFPTFFC_BAD_METADATA,
      "Setting table features failed : Invalid metadata mask."},
   {OFPET_TABLE_FEATURES_FAILED,OFPTFFC_BAD_TYPE,
      "Setting table features failed : Unknown property type."},
   {OFPET_TABLE_FEATURES_FAILED,OFPTFFC_BAD_LEN,
      "Setting table features failed : Length problem in properties."},
   {OFPET_TABLE_FEATURES_FAILED,OFPTFFC_BAD_ARGUMENT,
      "Setting table features failed : Unsupported property value."},
   {OFPET_EXPERIMENTER,OFPTFFC_EPERM,
      "Experimenter error message : Permissions error."}

};


extern void cntrl_ucm_send_multipart_response(uint32_t app_id,void *data_p, uint8_t last);
extern void cntrl_ucm_async_msg_cfg_response(struct ofp_async_config   *of_async_config_msg);
extern inline int32_t
cntlr_transprt_send_to_dp_locally(struct of_msg              *msg,
      cntlr_conn_table_t         *conn_table,
      cntlr_conn_node_saferef_t  *conn_safe_ref);
int32_t config_resources();

extern int32_t                     
of_add_port_to_dp(uint64_t                     datapath_handle,
      struct ofl_port_desc_info   *port_desc);
void
of_handle_echo_request_msg(uint64_t controller_handle,
      uint64_t domain_handle,
      uint64_t datapath_handle,
      struct of_msg  *msg,
      struct ofl_echo_req_recv_msg *echo_req,
      void *cbk_arg1,
      void *cbk_arg2);
int32_t 
dp_port_status_change_event(uint64_t  controller_handle,
      uint64_t   domain_handle,
      uint64_t   datapath_handle,
      void      *event_info1,
      void      *event_info2);

void
dp_error_message_event(uint64_t               controller_handle,
      uint64_t               domain_handle,
      uint64_t               datapath_handle,
      void      *event_info1,
      void      *event_info2);

void dp_conn_opened_event(uint64_t  controller_handle,
      uint64_t  domain_handle,
      uint64_t  datapath_handle,
      void      *event_info1,
      void      *event_info2);

extern int32_t
fwd_app_pkt_rcvd(uint64_t              controller_handle,
      uint64_t               domain_handle,
      uint64_t               datapath_handle,
      struct of_msg         *msg,
      struct ofl_packet_in  *pkt_in,
      void                  *cbk_arg1,
      void                  *cbk_arg2);

   int32_t
fwd_arp_app_pkt_rcvd(uint64_t              controller_handle,
      uint64_t               domain_handle,
      uint64_t               datapath_handle,
      struct of_msg         *msg,
      struct ofl_packet_in  *pkt_in,
      void                  *cbk_arg1,
      void                  *cbk_arg2)
{
   return OF_SUCCESS;
}


   int32_t
cntlr_app_init(uint64_t  domain_handle)
{
   int32_t retval = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"entered.....");
#if 0
   retval = dprm_register_forwarding_domain_notifications(DPRM_DOMAIN_OFTABLE_ADDED,
         dm_notifications_cbk,
         (void *)&callback_arg1,
         (void *)&callback_arg2);

   if(retval != DRPM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Regtering with DPRM domain event failed, err = %d\r\n",
            __FUNCTION__,__LINE__,retval);
      return OF_FAILURE;
   }
#endif

#if 0
   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,
         "%s:%d registering for async conn open from dp event\r\n",__FUNCTION__,__LINE__);
   retval = cntlr_register_asynchronous_event_hook(domain_handle,
         OPEN_CONN_FROM_DP_EVENT,
         1, 
         (void*)dp_conn_opened_event,
         NULL,
         NULL);
   if(retval == OF_FAILURE)
   {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,
            "%s:%d Register Port add event cbk failed\r\n",__FUNCTION__,__LINE__);
      return OF_FAILURE;
   }
#endif

   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,
         "%s:%d registering for dp port status change event\r\n",__FUNCTION__,__LINE__);
   retval = cntlr_register_asynchronous_event_hook(domain_handle,
         DP_PORT_STATUS_CHANGE,
         1, 
         (void*)dp_port_status_change_event,
         NULL,
         NULL);
   if(retval == OF_FAILURE)
   {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,
            "%s:%d Register Port add event cbk failed\r\n",__FUNCTION__,__LINE__);
      return OF_FAILURE;
   }
   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,
         "%s:%d registering for async error msg event\r\n",__FUNCTION__,__LINE__);

   retval = cntlr_register_asynchronous_event_hook(domain_handle,
         DP_ERROR_MESSAGE,
         1, 
         (void*)dp_error_message_event,
         NULL,
         NULL);
   if(retval == OF_FAILURE)
   {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,
            "%s:%d Register error message recieve failed\r\n",__FUNCTION__,__LINE__);
      return OF_FAILURE;
   }

   retval = of_register_symmetric_message_hooks(domain_handle,
         OF_SYMMETRIC_MSG_ECHO_REQ,
         (void*)of_handle_echo_request_msg,
         NULL,
         NULL);
   if(retval != OF_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,
            "%s:%d Register to receive echo request message failed\r\n",__FUNCTION__,__LINE__);
      return OF_FAILURE;
   }
   return OF_SUCCESS;
}

   void
dp_error_message_event(uint64_t  controller_handle,
      uint64_t  domain_handle,
      uint64_t  datapath_handle,
      void      *event_info1,
      void      *event_info2)
{
   //uint8_t buffer[CNTLR_MAX_MSG_BUF_SIZE + 1];
   uint8_t buffer[512];
   char error_str[300];
   int32_t retval;
   //  struct of_msg  *msg= (struct of_msg *)event_info1;
   struct ofl_error_msg  *error_msg =(struct ofl_error_msg *)event_info2; 
   //CNTLR_BUILD_MSG_BUF_2_PRINT(error_msg->data,error_msg->length,buffer,CNTLR_MAX_MSG_BUF_SIZE);
   CNTLR_BUILD_MSG_BUF_2_PRINT(error_msg->data,error_msg->length,buffer,511);

   OF_GET_ERROR_STRING(error_msg->type, error_msg->code_or_exp_type,error_str);

   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,
                 "ERROR MSG :: type : %d ,code :%d::%s",
                 error_msg->type, error_msg->code_or_exp_type, error_str);
   /*
      if(msg  != NULL)
      msg->desc.free_cbk(msg);
    */
   return;
}
   void
of_handle_echo_request_msg(uint64_t controller_handle,
      uint64_t domain_handle,
      uint64_t datapath_handle,
      struct of_msg  *msg,
      struct ofl_echo_req_recv_msg *echo_req,
      void *cbk_arg1,
      void *cbk_arg2)
{
   struct ofp_header *msg_hdr;
   struct dprm_datapath_entry *datapath;
   cntlr_conn_node_saferef_t conn_safe_ref;
   cntlr_conn_table_t        *conn_table;
   int32_t                    retval = OF_SUCCESS;
   int32_t                    status = OF_SUCCESS;

   /*Sending back same openflow message, not allocating new message buffer*/
   msg_hdr = (struct ofp_header*)(msg->desc.start_of_data);
   msg_hdr->type = OFPT_ECHO_REPLY;
   //    msg_hdr->length =  ntohs(msg_hdr->length);
   //    msg_hdr->xid = ntohl(msg_hdr->xid);
   do
   {
      CNTLR_RCU_READ_LOCK_TAKE();
      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error in getting datapath info, Err = %d\n",
               __FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;   

      }

      CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);
      //retval = cntlr_send_msg_to_dp(msg,datapath_handle,conn_table,&conn_safe_ref,NULL,NULL,NULL);
      retval = cntlr_transprt_send_to_dp_locally(msg,conn_table,&conn_safe_ref);
      if(retval  == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error in sending the message\n",__FUNCTION__,__LINE__);
         status = OF_FAILURE;  
         break;
      }
   }while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return status;
}

   int32_t
cntlr_configure_switch(uint64_t datapath_handle)
{
   struct of_msg  *msg;
   int32_t         retval= CNTLR_SUCCESS;
   int32_t         status= OF_SUCCESS;
   void *conn_info_p;

   CNTLR_RCU_READ_LOCK_TAKE();

   do
   {
      msg = ofu_alloc_message(OFPT_SET_CONFIG,
            OFU_SWITCH_CONFIG_SET_REQ_MESSAGE_LEN);
      if(msg == NULL)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d OF message alloc error \r\n",__FUNCTION__,__LINE__);
         status = OF_FAILURE;
         break;
      }

      retval = of_set_switch_config_msg(msg,
            datapath_handle,
            NULL,
            NULL, &conn_info_p);
      if(retval != CNTLR_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Set Switch config fail, error=%d \r\n",
               __FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      }
      /*Configure switch to handle frags normally, no-reassembly*/
      /*TBD configuration support is required to take input*/
//      ofu_set_switch_to_frag_normal(msg);
	ofu_set_switch_to_re_assmble_frags(msg);
      /*Configure switch to miss send length as default, i.e., 126 bytes*/
      /*TBD configuration support is required to take input*/
      ofu_set_miss_send_len_in_switch(msg, OF_DEFAULT_SW_MISS_SEND_LEN);

      retval=of_cntrlr_send_request_message(
            msg,
            datapath_handle,
            conn_info_p,
            NULL,
            NULL);
      if (retval != OF_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error adding flow entry in route table = %d",retval);
         status = OF_FAILURE;
      }

   }
   while(0);

   CNTLR_RCU_READ_LOCK_RELEASE();

   if(status != OF_SUCCESS)
   {
      if(msg != NULL)
         msg->desc.free_cbk(msg);
   }     
   return status;
}

/******************************************************************************/
void of_controller_role_change_reply_fn(uint64_t      controller_handle,
      uint64_t      domain_handle,
      uint64_t      datapath_handle,
      struct of_msg *msg,
      void          *cbk_arg1,
      void          *cbk_arg2,
      uint8_t       status,
      uint32_t      new_role)
{

   if( status == OF_RESPONSE_STATUS_SUCCESS)
   {
      if(of_add_new_role_to_dp(datapath_handle, new_role) == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Add new role to DP failure \r\n",__FUNCTION__,__LINE__);
      }

   }
   if(msg != NULL)
      msg->desc.free_cbk(msg);

}
/*****************************************************************************/
void of_async_config_reply_cbk_fn(uint64_t  controller_handle,
      uint64_t  domain_handle,
      uint64_t  datapath_handle,
      struct of_msg *msg,
      void     *cbk_arg1,
      void     *cbk_arg2,
      uint8_t   status)
{
   uint32_t rec_index=0;
   struct ofp_async_config   *of_async_config_msg = NULL;   
   if( status == OF_RESPONSE_STATUS_SUCCESS)
   {
      of_async_config_msg = (struct ofp_async_config *)(msg->desc.start_of_data);
      cntrl_ucm_async_msg_cfg_response(of_async_config_msg);
   }
   if(msg != NULL)
      msg->desc.free_cbk(msg);

#if 0

   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "##### Async Configuration######## ");
   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "packet_in_mask[0]:%x,packet_in_mask[1]:%x ", of_async_config_msg->packet_in_mask[0], of_async_config_msg->packet_in_mask[1]);
   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "port_status_mask[0]:%x,port_status_mask[1]:%x ",of_async_config_msg->port_status_mask[0],of_async_config_msg->port_status_mask[1]);
   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "flow_removed_mask[0]:%x,flow_removed_mask[1]:%x ",of_async_config_msg->flow_removed_mask[0], of_async_config_msg->flow_removed_mask[1]);

#endif
}
/******************************************************************************/
   void
cntlr_handle_port_desc_msg( struct of_msg *msg,
      uint64_t controller_handle,
      uint64_t domain_handle,
      uint64_t datapath_handle,
      uint8_t  port_no,
      void  *cbk_arg1,
      void  *cbk_arg2,
      uint8_t status,
      struct ofl_port_desc_info *port_description,
      uint8_t more_ports)
{
   if( status == OF_RESPONSE_STATUS_SUCCESS)
   {
      if(of_add_port_to_dp(datapath_handle,port_description) == OF_FAILURE)
      { 
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Add port to DP failure \r\n",__FUNCTION__,__LINE__);
      }
   }
   if(msg != NULL)
      msg->desc.free_cbk(msg);
}

void
cntlr_handle_flow_stats_msg( 
      uint64_t controller_handle,
      uint64_t domain_handle,
      uint64_t datapath_handle,
      uint8_t  port_no,
      struct of_msg *msg,
      void  *cbk_arg1,
      void  *cbk_arg2,
      uint8_t status,
      struct ofi_flow_entry_info *flow_stats,
      uint8_t more_ports)
{
   // OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Entered ..calling ucm handler",__FUNCTION__,__LINE__);
#ifdef CNTRL_OFCLI_SUPPORT
   cntrl_ucm_send_multipart_response(OFPMP_FLOW,flow_stats,more_ports);
#endif
   if(msg != NULL)
      msg->desc.free_cbk(msg);
}

/*Vinod*/
void
cntlr_handle_bind_stats_msg( 
      uint64_t controller_handle,
      uint64_t domain_handle,
      uint64_t datapath_handle,
      uint8_t  port_no,
      struct of_msg *msg,
      void  *cbk_arg1,
      void  *cbk_arg2,
      uint8_t status,
      struct ofi_bind_entry_info *bind_stats,
      uint8_t more_ports)
{
  struct dprm_datapath_entry *datapath = NULL;
  int32_t retval;

  retval = get_datapath_byhandle(datapath_handle, &datapath);
  if(retval  != DPRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,
               " Error in getting datapath,Err=%d ",retval);
    return;
  }

   // OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Entered ..calling ucm handler",__FUNCTION__,__LINE__);
   switch(bind_stats->type)
   {
#ifdef OPENFLOW_IPSEC_SUPPORT
      case FSLX_BO_APP_IPSEC:
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"handling bind stats msg");
        inform_event_to_app(datapath,
                            DP_BIND_MOD_MULTIPART_RESP_EVENT,
                            (void*)msg,
                            (void*)bind_stats);
         break;
#endif /* OPENFLOW_IPSEC_SUPPORT */
   }

#ifdef CNTRL_OFCLI_SUPPORT
   cntrl_ucm_send_multipart_response(OFPMP_EXPERIMENTER,bind_stats,more_ports);
#endif
   if(msg != NULL)
      msg->desc.free_cbk(msg);
}

void
cntlr_handle_aggregate_stats_msg( 
      uint64_t controller_handle,
      uint64_t domain_handle,
      uint64_t datapath_handle,
      uint8_t  port_no,
      struct of_msg *msg,
      void  *cbk_arg1,
      void  *cbk_arg2,
      uint8_t status,
      struct ofi_aggregate_flow_stats *aggregate_stats,
      uint8_t more_ports)
{
   // OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Entered ..calling ucm handler",__FUNCTION__,__LINE__);
#ifdef CNTRL_OFCLI_SUPPORT
   cntrl_ucm_send_multipart_response(OFPMP_AGGREGATE,aggregate_stats,more_ports);
#endif
   if(msg != NULL)
      msg->desc.free_cbk(msg);
}

   void
cntlr_handle_table_stats_msg(
      uint64_t controller_handle,
      uint64_t domain_handle,
      uint64_t datapath_handle,
      uint8_t  port_no,
      struct of_msg *msg,
      void  *cbk_arg1,
      void  *cbk_arg2,
      uint8_t status,
      struct ofi_table_stats_info *table_stats,
      uint8_t last)
{
   // OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Entered ...calling ucm handlers \r\n",__FUNCTION__,__LINE__);
#ifdef CNTRL_OFCLI_SUPPORT
   cntrl_ucm_send_multipart_response(OFPMP_TABLE,table_stats,last);
#endif

   if(msg != NULL)
      msg->desc.free_cbk(msg);
}

void
cntlr_handle_port_stats_msg( 
      uint64_t controller_handle,
      uint64_t domain_handle,
      uint64_t datapath_handle,
      uint8_t  port_no,
      struct of_msg *msg,
      void  *cbk_arg1,
      void  *cbk_arg2,
      uint8_t status,
      struct ofi_port_stats_info *port_stats,
      uint8_t more_ports)
{
   //OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Entered ...calling ucm handlers\r\n",__FUNCTION__,__LINE__);
#ifdef CNTRL_OFCLI_SUPPORT
   cntrl_ucm_send_multipart_response(OFPMP_PORT_STATS,port_stats,more_ports);
#endif
   if(msg != NULL)
      msg->desc.free_cbk(msg);
}


void cntlr_set_async_msg_cfg(uint64_t datapath_handle)
{
   int32_t retval=CNTLR_SUCCESS;
   int32_t         status= OF_SUCCESS;
   struct of_msg  *msg=NULL;
   struct dprm_datapath_entry   *datapath=NULL;
   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "%s:%d \n",__FUNCTION__,__LINE__);


   retval = get_datapath_byhandle(datapath_handle, &datapath);
   if(retval  != DPRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Error in getting datapath,Err=%d");
      return;
   }


   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "%s:%d  controller role:%d\n",__FUNCTION__,__LINE__,datapath->controller_role);
   if(datapath->controller_role == OFPCR_ROLE_SLAVE)
   {
      return;   
   }
   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      msg = ofu_alloc_message(OFPT_SET_ASYNC, OFU_SET_ASYNC_CONFIG_MESSAGE_LEN);
      if(msg == NULL)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d OF message alloc error \r\n",__FUNCTION__,__LINE__);
         status = OF_FAILURE;
         break;
      }


      if(ofu_set_packetin_async_msg_flags_in_master_or_equal_role(msg, TRUE, TRUE, FALSE)
            != OF_SUCCESS)
      {

         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, " ofu_set_packetin_async_msg_flags_in_master_or_equal_role failed!.");      

      }


      retval = of_set_async_configuration(datapath_handle, 
            msg,
            NULL,
            NULL);

      if(retval != CNTLR_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Send Role Request  message fail, error=%d \r\n",
               __FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   if(status != OF_SUCCESS)
   {
      if(msg != NULL)
         msg->desc.free_cbk(msg);
   }     
}
int32_t cntlr_get_async_msg_cfg(uint64_t datapath_handle)
{
   int32_t retval=CNTLR_SUCCESS;
   int32_t  status= OF_SUCCESS;
   struct of_msg  *msg=NULL;
   struct dprm_datapath_entry   *datapath=NULL;
   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "%s:%d \n",__FUNCTION__,__LINE__);


   retval = get_datapath_byhandle(datapath_handle, &datapath);
   if(retval  != DPRM_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Error in getting datapath,Err=%d");
      return OF_FAILURE;
   }


   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      msg = ofu_alloc_message(OFPT_GET_ASYNC_REQUEST, OFU_GET_ASYNC_CONFIG_MESSAGE_LEN);
      if(msg == NULL)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d OF message alloc error \r\n",__FUNCTION__,__LINE__);
         status = OF_FAILURE;
         break;
      }


      retval = of_get_async_config_info(msg, datapath_handle,of_async_config_reply_cbk_fn,NULL, NULL);


      if(retval != CNTLR_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Send Role Request  message fail, error=%d \r\n",
               __FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   if(status != OF_SUCCESS)
   {
      if(msg != NULL)
         msg->desc.free_cbk(msg);
   }     

   return status;
}

/******************************************************************************/
void cntlr_send_role_req_request(uint64_t datapath_handle, uint8_t new_role)
{
   struct of_msg  *msg;
   int32_t         retval= CNTLR_SUCCESS;
   int32_t         status= OF_SUCCESS;
   static uint64_t        generation_id=1;


   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL, "%s:%d \n",__FUNCTION__,__LINE__);
   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      msg = ofu_alloc_message(OFPT_ROLE_REQUEST, OFU_ROLE_REQUEST_MESSAGE_LEN);
      if(msg == NULL)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d OF message alloc error \r\n",__FUNCTION__,__LINE__);
         status = OF_FAILURE;
         break;
      }


      retval= of_send_role_change_msg(msg,
            datapath_handle,
            new_role,
            generation_id,
            of_controller_role_change_reply_fn,
            NULL,
            NULL);
      generation_id++;


      if(retval != CNTLR_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Send Role Request  message fail, error=%d \r\n",
               __FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   if(status != OF_SUCCESS)
   {
      if(msg != NULL)
         msg->desc.free_cbk(msg);
   }     
}
/******************************************************************************/
   void
cntlr_send_port_desc_request(uint64_t datapath_handle)
{
   struct of_msg  *msg;
   int32_t         retval= CNTLR_SUCCESS;
   int32_t         status= OF_SUCCESS;

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      msg = ofu_alloc_message(OFPT_MULTIPART_REQUEST,
            OFU_PORT_DESCRIPTION_REQ_MESSAGE_LEN);
      if(msg == NULL)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d OF message alloc error \r\n",__FUNCTION__,__LINE__);
         status = OF_FAILURE;
         break;
      }

      retval = of_get_port_description(msg, 
            datapath_handle,
            cntlr_handle_port_desc_msg,
            NULL,NULL);
      if(retval != CNTLR_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Send Get Port Description message fail, error=%d \r\n",
               __FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   if(status != OF_SUCCESS)
   {
      if(msg != NULL)
         msg->desc.free_cbk(msg);
   }     
}

   int32_t 
cntlr_send_table_stats_request(uint64_t datapath_handle)
{
   struct of_msg  *msg;
   int32_t         retval= CNTLR_SUCCESS;
   int32_t         status= OF_SUCCESS;

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      msg = ofu_alloc_message(OFPT_MULTIPART_REQUEST,
            OFU_TABLE_STATISTICS_REQ_MESSAGE_LEN);
      if(msg == NULL)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d OF message alloc error \r\n",__FUNCTION__,__LINE__);
         status = OF_FAILURE;
         break;
      }

      retval = of_get_table_statistics(msg, 
            datapath_handle,
            cntlr_handle_table_stats_msg,
            NULL,
            NULL
            );
      if(retval != CNTLR_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Send Get Table Statistics message fail, error=%d \r\n",
               __FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   if(status != OF_SUCCESS)
   {
      if(msg != NULL)
         msg->desc.free_cbk(msg);
   }     
   return status;

}

   int32_t
cntlr_send_port_stats_request(uint64_t datapath_handle)
{
   struct of_msg  *msg;
   int32_t         retval= CNTLR_SUCCESS;
   int32_t         status= OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"%s:%d entered \r\n",__FUNCTION__,__LINE__);
   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      msg = ofu_alloc_message(OFPT_MULTIPART_REQUEST,
            OFU_PORT_STATISTICS_REQ_MESSAGE_LEN);
      if(msg == NULL)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d OF message alloc error \r\n",__FUNCTION__,__LINE__);
         status = OF_FAILURE;
         break;
      }

      retval = of_get_port_statistics(msg,
            datapath_handle,
            OFPP_ANY,
            cntlr_handle_port_stats_msg,
            NULL,
            NULL 
            );
      if(retval != CNTLR_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Send Get Port statistics message fail, error=%d \r\n",
               __FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   if(status != OF_SUCCESS)
   {

      if(msg != NULL)
         msg->desc.free_cbk(msg);
   }
   return status;
}


   int32_t
cntlr_send_flow_stats_request(uint64_t datapath_handle)
{
   struct of_msg  *msg;
   int32_t         retval= CNTLR_SUCCESS;
   int32_t         status= OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"%s:%d entered \r\n",__FUNCTION__,__LINE__);
   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      msg = ofu_alloc_message(OFPT_MULTIPART_REQUEST,
            OFU_INDIVIUAL_FLOW_ENTRY_STATS_REQ_MESSAGE_LEN);
      if(msg == NULL)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d OF message alloc error \r\n",__FUNCTION__,__LINE__);
         status = OF_FAILURE;
         break;
      }


      retval = of_get_flow_entries(msg,
            datapath_handle,
            OFPTT_ALL, //table_id,
            OFPP_ANY, //out_port,
            OFPG_ANY, //out_group,
            0, //cookie,
            0, //cookie_mask,
            cntlr_handle_flow_stats_msg,
            NULL,
            NULL);
      if(retval != CNTLR_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Send Get flow stats message fail, error=%d \r\n",
               __FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   if(status != OF_SUCCESS)
   {
      if(msg != NULL)
         msg->desc.free_cbk(msg);
   }
   return status;
}

/*Vinod*/

   int32_t
cntlr_send_bind_stats_request(uint64_t datapath_handle, uint32_t bind_obj_id)
{ 
   struct of_msg  *msg;
   int32_t         retval= CNTLR_SUCCESS;
   int32_t         status= OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"%s:%d entered \r\n",__FUNCTION__,__LINE__);
   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      msg = ofu_alloc_message(OFPT_MULTIPART_REQUEST,
            OFU_INDIVIUAL_BIND_ENTRY_STATS_REQ_MESSAGE_LEN);
      if(msg == NULL)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d OF message alloc error \r\n",__FUNCTION__,__LINE__);
         status = OF_FAILURE;
         break;
      }


      retval = of_get_bind_entries(msg,
            datapath_handle,
            bind_obj_id,
            cntlr_handle_bind_stats_msg,
            NULL,
            NULL);
      if(retval != CNTLR_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Send Get bind stats message fail, error=%d \r\n",
               __FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   if(status != OF_SUCCESS)
   {
      if(msg != NULL)
         msg->desc.free_cbk(msg);
   }
   return status;
}


   int32_t
cntlr_send_aggregate_stats_request(uint64_t datapath_handle)
{
   struct of_msg  *msg;
   int32_t         retval= CNTLR_SUCCESS;
   int32_t         status= OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"%s:%d entered \r\n",__FUNCTION__,__LINE__);
   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      msg = ofu_alloc_message(OFPT_MULTIPART_REQUEST,
            OFU_AGGREGATE_FLOW_ENTRY_STATS_REQ_MESSAGE_LEN );
      if(msg == NULL)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d OF message alloc error \r\n",__FUNCTION__,__LINE__);
         status = OF_FAILURE;
         break;
      }

      retval = of_get_aggregate_flow_entries(msg,
            datapath_handle,
            OFPTT_ALL,//table_id,
            OFPP_ANY,//out_port,
            OFPG_ANY,//out_group,
            0,//cookie,
            0,//cookie_mask,
            cntlr_handle_aggregate_stats_msg,
            NULL,
            NULL);
      if(retval != CNTLR_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Send Get aggregate stats message fail, error=%d \r\n",
               __FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   if(status != OF_SUCCESS)
   {
      if(msg != NULL)
         msg->desc.free_cbk(msg);
   }
   return status;
}


void
cntlr_handle_table_feature_msg( 
      uint64_t controller_handle,
      uint64_t domain_handle,
      uint64_t datapath_handle,
      uint8_t  table_id,
      struct of_msg *msg,
      void  *cbk_arg1,
      void  *cbk_arg2,
      uint8_t status,
      struct ofi_table_features_info *table_features,
      uint8_t more_tables)
{
   // OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"%s:%d Entered ..calling ucm handler",__FUNCTION__,__LINE__);
#ifdef CNTRL_OFCLI_SUPPORT
   cntrl_ucm_send_multipart_response(OFPMP_TABLE_FEATURES,table_features,more_tables);
#endif
   if(msg != NULL)
      msg->desc.free_cbk(msg);
}

   int32_t
cntlr_send_tablefeature_request(uint64_t datapath_handle)
{
   struct of_msg  *msg;
   int32_t         retval= CNTLR_SUCCESS;
   int32_t         status= OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"%s:%d entered \r\n",__FUNCTION__,__LINE__);
   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      msg = ofu_alloc_message(OFPT_MULTIPART_REQUEST,
            OFU_TABLE_FEATURES_REQ_MESSAGE_LEN);
      if(msg == NULL)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d OF message alloc error \r\n",__FUNCTION__,__LINE__);
         status = OF_FAILURE;
         break;
      }

      retval = of_get_table_features(msg,
            datapath_handle,
            cntlr_handle_table_feature_msg,
            NULL,
            NULL);
      if(retval != CNTLR_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Send table feature message fail, error=%d \r\n",
               __FUNCTION__,__LINE__,retval);
         status = OF_FAILURE;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   if(status != OF_SUCCESS)
   {
      if(msg != NULL)
         msg->desc.free_cbk(msg);
   }
   return status;
}


   int32_t 
dp_port_status_change_event(uint64_t  controller_handle,
      uint64_t   domain_handle,
      uint64_t   datapath_handle,
      void      *event_info1,
      void      *event_info2)
{
   struct ofl_port_status  *port_status_msg;
   struct of_msg           *msg;
   uint64_t                 port_handle;
   int32_t                  retval = OF_SUCCESS;

   cntlr_assert(event_info1 != NULL)

      msg = (struct of_msg*)event_info1;
   port_status_msg = (struct ofl_port_status*)event_info2;

   if( (port_status_msg->reason == OFPPR_ADD) ||
         (port_status_msg->reason == OFPPR_MODIFY) ) 
   {
      if(of_add_port_to_dp(datapath_handle,&port_status_msg->port_info) == OF_FAILURE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Add port to DP failure \r\n",__FUNCTION__,__LINE__);
         return OF_FAILURE;
      }
   }
   else if (port_status_msg->reason == OFPPR_DELETE)
   { 

      retval = dprm_get_port_handle(datapath_handle,
            port_status_msg->port_info.name,
            &port_handle);
      if(retval != DPRM_SUCCESS)
      {  
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Get port info failure,err = %d \r\n",__FUNCTION__,__LINE__,retval);
         return OF_FAILURE;
      }

      retval = dprm_delete_port_from_datapath(datapath_handle,port_handle);
      if(retval != DPRM_SUCCESS)
      {  
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Delete port failure,err = %d \r\n",__FUNCTION__,__LINE__,retval);
         return OF_FAILURE;
      }
   }

   if(msg != NULL)
      msg->desc.free_cbk(msg);

   return OF_SUCCESS;
}

void cntlr_app_domain_event_cbk(uint32_t notification_type,
      uint64_t domain_handle,
      struct   dprm_domain_notification_data notification_data,
      void     *callback_arg1,
      void     *callback_arg2)
{

   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "domain name = %s",notification_data.domain_name);
   switch(notification_type)
   {
      case DPRM_DOMAIN_ADDED:
         {
            cntlr_app_init(domain_handle);
            break;
         }
   }
   return;
}

void cntlr_reg_domain_init(void)
{

   /* Registering to forward domain add/del/mod and other notifications */
   if (dprm_register_forwarding_domain_notifications(DPRM_DOMAIN_ALL_NOTIFICATIONS,
            cntlr_app_domain_event_cbk,
            NULL, NULL) != OF_SUCCESS)
   {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:Registering to forward domain failed....\r\n",__FUNCTION__);
      return;
   }
}

   void 
cntlr_echo_reply_clbk_fn( uint64_t controller_handle,
      uint64_t domain_handle,
      uint64_t datapath_handle,
      struct of_msg  *msg,
      struct ofl_echo_reply_msg *echo_reply,
      void *cbk_arg1,
      void *cbk_arg2)
{
   struct dprm_datapath_entry   *datapath;
   cntlr_conn_node_saferef_t    conn_safe_ref;
   cntlr_conn_table_t           *conn_table;
   uint8_t               hash_tbl_access_status = FALSE;
   cntlr_channel_info_t *channel;
   int32_t retval = OF_SUCCESS;
   int32_t status = OF_SUCCESS;

   /*TBD later user cbk_arg1 contains auxiliary ID, according set corresponding
     channel flag, currently */
   do
   {

	   retval = get_datapath_byhandle(datapath_handle, &datapath);
	   if(retval  != DPRM_SUCCESS)
	   {
		   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error in getting datapath info, Err=%d \r\n",
				   __FUNCTION__,__LINE__,retval);
		   status = DPRM_INVALID_DATAPATH_HANDLE;
		   break;
	   }

	   CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);

	   MCHASH_ACCESS_NODE(conn_table,conn_safe_ref.index,conn_safe_ref.magic,channel,hash_tbl_access_status);
	   if(hash_tbl_access_status == FALSE   )
	   {
		   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Channel safe reference is not valid  \r\n",__FUNCTION__,__LINE__);
		   status = OF_FAILURE;
		   break;
	   }

	   channel->keep_alive_state = channel->keep_alive_state|OF_TRNSPRT_CHN_CONN_KEEP_ALIVE_MSG_RESP_RCVD;
   }
   while(0);
}

   int32_t
of_send_echo_msg_on_channel(uint64_t datapath_handle,
      uint32_t auxilaryID)
{
   /*TBD, currently auxilaryID not handled, sending echo on main channel only*/
   return ofp_prepare_and_send_echo_request (datapath_handle,NULL,cntlr_echo_reply_clbk_fn,NULL,NULL);
}


void dp_conn_opened_event(uint64_t  controller_handle,
      uint64_t  domain_handle,
      uint64_t  datapath_handle,
      void      *event_info1,
      void      *event_info2)
{
   cntlr_channel_info_t  *channel = (cntlr_channel_info_t *)event_info1;
   int32_t retval;


#if 0
   retval = cntlr_trnsprt_send_hello_msg(channel);
   if(retval == OF_FAILURE)
   {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL,
            "%s:%d Error in sending hello message\r\n", __FUNCTION__,__LINE__);
   }
#endif
}


   void
cntlr_app_trnsprt_handle_handshake(int32_t  fd,
      epoll_user_data_t  *info,
      uint32_t  received_events)
{
   cntlr_trnsprt_handle_handshake(fd, info, received_events);
   return;
}

