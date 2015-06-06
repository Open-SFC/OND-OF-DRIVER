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

/*File      :of_symmetric_msgapi.c
 *Author     : Rajesh Madabushi <rajesh.madabushi@freescale.com>
 *Date       : March, 2013  
 *Description: 
 * This file contains defintions symmetric messaging APIs provided to 
 * North bound interface. 
 **/
#include "controller.h"
#include "of_utilities.h"
#include "of_multipart.h"
#include "of_msgapi.h"
#include "cntrl_queue.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "cntlr_event.h"
#include "dprmldef.h"

//extern uint32_t  uiKeepAliveCount_g; 
extern inline int32_t cntlr_send_msg_to_dp(struct of_msg               *msg,
                     struct dprm_datapath_entry* datapath,
                     cntlr_conn_table_t         *conn_table,
                     cntlr_conn_node_saferef_t  *conn_safe_ref,
                     void                       *callback_fn,
                     void                       *clbk_arg1,
                     void                       *clbk_arg2);
int32_t
of_register_symmetric_message_hooks(uint64_t  domain_handle,
                                    uint8_t   event_type,
                                    void     *call_back,
                                    void     *cbk_arg1,
                                    void     *cbk_arg2)
{
   struct dprm_domain_entry *domain;
   int32_t                   status = OF_SUCCESS;
   int32_t                   retval = OF_SUCCESS;

   if(call_back == NULL)
   {
       OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s:%d  Invaid paramter passed \r\n",__FUNCTION__,__LINE__);
       return OF_REG_SYMMETRIC_HOOK_NULL_CALLBACK;
   }
   if( (event_type < OF_FIRST_SYMMMETRIC_MSG_EVENT) ||
       (event_type > OF_LAST_SYMMMETRIC_MSG_EVENT)    )
   {
       OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s:%d Invalid event(%d) type passed \r\n",__FUNCTION__,__LINE__,event_type);
       return OF_REG_SYMMETRIC_HOOK_INVALID_EVENT_TYPE;

   }
 
   CNTLR_RCU_READ_LOCK_TAKE();

   do
   {
     retval = get_domain_byhandle(domain_handle, &domain);
     if(retval != DPRM_SUCCESS)
     {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s:%d Error in getting domain details retval=%d \r\n",__FUNCTION__,__LINE__,retval);
        status = OF_REG_SYMMETRIC_HOOK_INVALID_DOMAIN;
        break;
     }
     CNTLR_LOCK_TAKE(domain->lock);
     domain->symmetric_msg_handle[event_type].type     = event_type;
     domain->symmetric_msg_handle[event_type].callback = call_back;
     domain->symmetric_msg_handle[event_type].cbk_arg1 = cbk_arg1;
     domain->symmetric_msg_handle[event_type].cbk_arg2 = cbk_arg2;
     CNTLR_LOCK_RELEASE(domain->lock);
   }
   while(0);

   CNTLR_RCU_READ_LOCK_RELEASE();

   return status;
}


int32_t
ofp_prepare_and_send_echo_request ( uint64_t datapath_handle,
                                    struct ofl_echo_req_send_msg *echo_msg,
                                    echo_reply_clbk_fn echo_reply_clbk_fn,
                                    void * cbk_arg1,
                                    void * cbk_arg2
                                  )
{
     struct of_msg      *msg;
     struct ofp_header *msg_hdr;
     struct dprm_datapath_entry   *datapath;
     cntlr_conn_node_saferef_t conn_safe_ref;
     cntlr_conn_table_t        *conn_table;
     int32_t                       retval = OF_SUCCESS;
     int32_t                       status = OF_SUCCESS;
      uint8_t               hash_tbl_access_status = FALSE;
      cntlr_channel_info_t *channel;


     /*TBD Currently it is handling only main connection,
       but need to send echo per connectio basis, use cb1_arg1 to
       indicutate auxiliary ID*/
    
     /*TBD of sending message along with echo request*/
     if(echo_msg != NULL)
     {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s:%d Currently no support of echo message\r\n",__FUNCTION__,__LINE__);
         return OF_FAILURE;
     }

     do
     {
        msg = ofu_alloc_message(OFPT_ECHO_REQUEST,
                                sizeof(struct ofp_header));
        if(msg == NULL)
        {
           OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d OF message alloc error \r\n",__FUNCTION__,__LINE__);
           status = OF_FAILURE;
           break;
        }

        msg_hdr = (struct ofp_header*)(msg->desc.start_of_data);
        msg_hdr->version  = OFP_VERSION;
        msg_hdr->type     = OFPT_ECHO_REQUEST;
        msg_hdr->xid      = 0; 
        msg->desc.data_len = sizeof(struct ofp_header);
        msg_hdr->length    = htons(msg->desc.data_len);

        retval = get_datapath_byhandle(datapath_handle, &datapath);
        if(retval  != DPRM_SUCCESS)
        {
           OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error in getting datapath info, Err=%d \r\n",
                                                __FUNCTION__,__LINE__,retval);
           status = DPRM_INVALID_DATAPATH_HANDLE;
           break;
        }

	CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);
	if ( conn_table == NULL )
	{
		OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"conn table NULL");
		status = OF_FAILURE;
		break;
	}
        retval = cntlr_send_msg_to_dp(msg,datapath,conn_table,&conn_safe_ref,
                                          echo_reply_clbk_fn,cbk_arg1,cbk_arg2);
        if(retval  == OF_FAILURE)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error in sending message to datapath \r\n",__FUNCTION__,__LINE__);
          status = CNTLR_SEND_MSG_TO_DP_ERROR;
          break;
        }
	MCHASH_ACCESS_NODE(conn_table,conn_safe_ref.index,conn_safe_ref.magic,channel,hash_tbl_access_status);
	if(hash_tbl_access_status == FALSE   )
	{
		OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,"Channel safe reference is not valid ");
		status = OF_FAILURE;
		break;
	}
      channel->uiKeepAliveCount++; 
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Incremented the uiKeepAliveCount_g to %d \r\n",channel->uiKeepAliveCount);


      } while(0);

      if(status == OF_FAILURE)
      {
         if(msg != NULL)
          msg->desc.free_cbk(msg);
      }

      return status;
}

