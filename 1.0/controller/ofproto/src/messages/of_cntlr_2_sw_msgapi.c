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

/*!\file of_cntlr_2_sw_msgapi.c
 * Author     : Rajesh Madabushi <rajesh.madabushi@freescale.com>
 * Date       : November, 2012  
 * Description: 
 * This file contains defintions different controller to switch messaging APIs provided to 
 * North bound interface. 
 */
#include "controller.h"
#include "of_utilities.h"
#include "of_msgapi.h"
#include "of_multipart.h"
#include "cntrl_queue.h"
#include "of_msgpool.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "cntlr_event.h"
#include "dprmldef.h"
#include "multi_part.h"
#include "fsl_ext.h"

extern inline int32_t
cntlr_send_msg_to_dp(struct of_msg               *msg,
      struct dprm_datapath_entry* datapath,
      cntlr_conn_table_t         *conn_table,
      cntlr_conn_node_saferef_t  *conn_safe_ref,
      void                       *callback_fn,
      void                       *clbk_arg1,
      void                       *clbk_arg2);

#if 0
   int32_t
of_switch_feature_request msg(struct of_msg *msg,
      uint64_t  datapath_handle,
      of_switch_feature_reply_cbk_fn sw_feature_reply_cbk,
      void *clbk_arg1,
      void *clbk_arg2,
      uint32_t *xid)
{
   struct ofp_header *handshake_msg;

   handshake_msg = (struct ofp_header *)(msg->start_of_data);
   handshake_msg->version = OFP_VERSION;
   handshake_msg->type    = OFPT_FEATURES_REQUEST;
   handshake_msg->xid     = 0;
   msg->data_len = sizeof(struct ofp_header);
   handshake_msg->length = htons(pMsg->DataLen);

   return OF_SUCCESS;
}
#endif

/* API to set switch configuration*/
   int32_t
of_set_switch_config_msg(struct    of_msg *msg,
      uint64_t  datapath_handle,
      void     *clbk_command_arg1,
      void     *clbk_command_arg2,
      void **conn_info_pp)
{
   struct ofp_switch_config     *switch_config_msg;
   struct dprm_datapath_entry   *datapath;
   cntlr_conn_node_saferef_t conn_safe_ref;
   cntlr_conn_table_t        *conn_table=NULL;
   struct of_cntlr_conn_info *conn_info=NULL;
   uint8_t                       send_status = TRUE;
   int32_t                       retval = OF_SUCCESS;
   int32_t                       status = OF_SUCCESS;

   cntlr_assert(msg != NULL);

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"entered");
   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      if(msg->desc.buffer_len  <  OFU_SWITCH_CONFIG_SET_REQ_MESSAGE_LEN)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Allocated message buffer length is not suffecient ");
         status = OF_MSG_BUF_SIZE_IS_IN_SUFFECIENT;
      }

      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Error in getting datapath,Err=%d",retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      switch_config_msg = (struct ofp_switch_config *)(msg->desc.start_of_data);
      switch_config_msg->header.version = OFP_VERSION;
      switch_config_msg->header.type    = OFPT_SET_CONFIG;
      switch_config_msg->header.xid     = 0;
      msg->desc.data_len += sizeof(struct ofp_switch_config);
      switch_config_msg->header.length  = htons(msg->desc.data_len);

      if(datapath->is_main_conn_present)
      {
         CNTLR_GET_MAIN_CHANNEL_INFO(datapath,conn_info);
         
         if ( conn_info == NULL )
         {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"conn info is null");
            status=OF_FAILURE;
            break;
         }
		*conn_info_pp=conn_info;

      }
      else
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Error in getting  channel");
         status=OF_FAILURE;
         break;
      }
#if 0
      CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);
      CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,datapath,conn_table,conn_safe_ref,
            NULL,clbk_command_arg1,clbk_command_arg2,send_status);
      if(send_status  == FALSE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error in sending message to datapath ");
         status = CNTLR_SEND_MSG_TO_DP_ERROR;
         break;
      }
#endif
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   return status;
}

   int32_t
of_send_barrier_request_msg( uint64_t  datapath_handle,
      of_barrier_reply_notification_fn barrier_reply_cbk_fn,
      void *cbk_arg1,
      void *cbk_arg2)
{
   struct of_msg     *msg;
   struct ofp_header *header;
   struct dprm_datapath_entry   *datapath;
   cntlr_conn_node_saferef_t conn_safe_ref;
   cntlr_conn_table_t        *conn_table;
   uint8_t  send_status = TRUE;
   int32_t  status = OF_SUCCESS;
   int32_t  retval = OF_SUCCESS;

   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"entered");

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      msg = ofu_alloc_message(OFPT_BARRIER_REQUEST, OFU_BARRIER_REQUEST_MESSAGE_LEN);
      if(msg == NULL)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OF message alloc error");
         break;
      }

      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Error in getting datapath,Err=%d",
               retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }


      if(!datapath->is_main_conn_present)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error in sending message to datapath,channel closed ");
//         msg->desc.free_cbk(msg);
         status = CNTLR_CHN_CLOSED;
         break;
      }

      CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);
      if ( conn_table == NULL )
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"conn table NULL");
         status = OF_FAILURE;
         break;
      }
      header = (struct ofp_header *)(msg->desc.start_of_data);
      header->version = OFP_VERSION;
      header->type    = OFPT_BARRIER_REQUEST;
      header->xid     = 0;
      msg->desc.data_len += (OFU_BARRIER_REQUEST_MESSAGE_LEN);
      header->length  = htons(msg->desc.data_len);

      CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,datapath,conn_table,conn_safe_ref,
            (void*)barrier_reply_cbk_fn,cbk_arg1,cbk_arg2,send_status);
      if(send_status  == FALSE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error in sending message to datapath");
         status = CNTLR_SEND_MSG_TO_DP_ERROR;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return status;
}


   int32_t
of_get_switch_config_msg(struct of_msg *msg,
      uint64_t  datapath_handle,
      of_switch_config_response_cbk_fn sw_config_cbk,
      void *clbk_arg1,
      void *clbk_arg2)

{
   struct ofp_switch_config     *switch_config_msg;
   struct dprm_datapath_entry   *datapath;
   cntlr_conn_node_saferef_t conn_safe_ref;
   cntlr_conn_table_t        *conn_table;
   int32_t                       retval = OF_SUCCESS;
   int32_t                       status = OF_SUCCESS;
   uint8_t                       send_status = TRUE;

   cntlr_assert(msg != NULL);

   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"entered");
   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      if(msg->desc.buffer_len  <  OFU_SWITCH_CONFIG_GET_REQ_MESSAGE_LEN)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"---");
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," Allocated message buffer length is not suffecient ");
         status= OF_MSG_BUF_SIZE_IS_IN_SUFFECIENT;
      }

      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in getting datapath,Err=%d " ,      retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      switch_config_msg = (struct ofp_switch_config *)(msg->desc.start_of_data);
      switch_config_msg->header.version = OFP_VERSION;
      switch_config_msg->header.type    = OFPT_GET_CONFIG_REQUEST;
      switch_config_msg->header.xid     = 0;
      msg->desc.data_len += OFU_SWITCH_CONFIG_GET_REQ_MESSAGE_LEN;
      switch_config_msg->header.length  = htons(msg->desc.data_len);

      CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);
      if ( conn_table == NULL )
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"conn table NULL");
         status = OF_FAILURE;
         break;
      }
      CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,datapath,conn_table,conn_safe_ref,
            (void*)sw_config_cbk,clbk_arg1,clbk_arg2,send_status);

      if(send_status  == FALSE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"---");
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," Error in sending message to datapath ");
         status = CNTLR_SEND_MSG_TO_DP_ERROR;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "status:%d",status);
   return status;

}

   int32_t
of_get_switch_info_through_channel(struct of_msg       *msg,
      uint64_t               datapath_handle,
      of_switch_info_cbk_fn  switch_info_cbk,
      void                  *cbk_arg1,
      void                  *cbk_arg2,
      uint32_t              *xid)
{
   struct ofp_multipart_request   *switch_info_req_multipart_msg;
   struct dprm_datapath_entry   *datapath;
   cntlr_conn_node_saferef_t conn_safe_ref;
   cntlr_conn_table_t        *conn_table;
   uint8_t  send_status = TRUE;
   int32_t  status = OF_SUCCESS;
   int32_t  retval = OF_SUCCESS;


   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO," entered ");

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      if(msg->desc.buffer_len != OFU_SWINFO_REQ_MESSAGE_LEN)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Allocated message buffer length is not suffecient");
         status = OF_MSG_BUF_SIZE_IS_IN_SUFFECIENT;
      }

      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Error in getting datapath,Err=%d ",retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      if(!datapath->is_main_conn_present)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," Error in sending message to datapath,channel closed "
               );
//         msg->desc.free_cbk(msg);
         status = CNTLR_CHN_CLOSED;
         break;
      }
      CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);
      if ( conn_table == NULL )
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," conn table NULL " );
         status = OF_FAILURE;
         break;
      }
      switch_info_req_multipart_msg = (struct ofp_multipart_request *)(msg->desc.start_of_data);
      switch_info_req_multipart_msg->header.version = OFP_VERSION;
      switch_info_req_multipart_msg->header.type    = OFPT_MULTIPART_REQUEST;
      switch_info_req_multipart_msg->header.xid     = 0;
      switch_info_req_multipart_msg->type = htons(OFPMP_DESC);

      msg->desc.data_len += (OFU_SWINFO_REQ_MESSAGE_LEN);
      switch_info_req_multipart_msg->header.length = htons(msg->desc.data_len);

      CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,datapath,conn_table,conn_safe_ref,
            (void*)switch_info_cbk,cbk_arg1,cbk_arg2,send_status);
      if(send_status  == FALSE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error in sending message to datapath");
         status = CNTLR_SEND_MSG_TO_DP_ERROR;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return status;
}



   int32_t
of_get_groups_description(struct of_msg       *msg,
      uint64_t              datapath_handle,
      of_group_desc_cbk_fn  group_desc_cbk,
      void                  *cbk_arg1,
      void                  *cbk_arg2,
      uint32_t              *xid)
{
   struct ofp_multipart_request   *group_desc_req_multipart_msg;
   struct dprm_datapath_entry   *datapath;
   cntlr_conn_node_saferef_t conn_safe_ref;
   cntlr_conn_table_t        *conn_table;
   uint8_t  send_status = TRUE;
   int32_t  status = OF_SUCCESS;
   int32_t  retval = OF_SUCCESS;


   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO," entered ");

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      if(msg->desc.buffer_len != OFU_GROUP_DESC_REQ_MESSAGE_LEN)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Allocated message buffer length is not suffecient");
         status = OF_MSG_BUF_SIZE_IS_IN_SUFFECIENT;
      }

      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error in getting datapath,Err=%d ",retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      if(!datapath->is_main_conn_present)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in sending message to datapath,channel closed "
               );
//         msg->desc.free_cbk(msg);
         status = CNTLR_CHN_CLOSED;
         break;
      }
      CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);
      if ( conn_table == NULL )
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," conn table NULL " );
         status = OF_FAILURE;
         break;
      }
      group_desc_req_multipart_msg = (struct ofp_multipart_request *)(msg->desc.start_of_data);
      group_desc_req_multipart_msg->header.version = OFP_VERSION;
      group_desc_req_multipart_msg->header.type    = OFPT_MULTIPART_REQUEST;
      group_desc_req_multipart_msg->header.xid     = 0;
      group_desc_req_multipart_msg->type = htons(OFPMP_GROUP_DESC);

      msg->desc.data_len += (OFU_GROUP_DESC_REQ_MESSAGE_LEN);
      group_desc_req_multipart_msg->header.length = htons(msg->desc.data_len);

      CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,datapath,conn_table,conn_safe_ref,
            (void*)group_desc_cbk,cbk_arg1,cbk_arg2,send_status);
      if(send_status  == FALSE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error in sending message to datapath");
         status = CNTLR_SEND_MSG_TO_DP_ERROR;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return status;
}

   int32_t
of_get_meter_config(struct of_msg       *msg,
      uint64_t              datapath_handle,
      of_meter_config_cbk_fn      meter_config_cbk,
      void                  *cbk_arg1,
      void                  *cbk_arg2,
      uint32_t              *xid)
{
   struct ofp_multipart_request   *meter_config_req_multipart_msg;
   struct dprm_datapath_entry   *datapath;
   cntlr_conn_node_saferef_t conn_safe_ref;
   cntlr_conn_table_t        *conn_table;
   uint8_t  send_status = TRUE;
   int32_t  status = OF_SUCCESS;
   int32_t  retval = OF_SUCCESS;


   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO," entered ");

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      if(msg->desc.buffer_len != OFU_METER_CONFIG_REQ_MESSAGE_LEN)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Allocated message buffer length is not suffecient");
         status = OF_MSG_BUF_SIZE_IS_IN_SUFFECIENT;
      }

      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error in getting datapath,Err=%d ",retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      if(!datapath->is_main_conn_present)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in sending message to datapath,channel closed "
               );
         //msg->desc.free_cbk(msg);
         status = CNTLR_CHN_CLOSED;
         break;
      }
      CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);
      if ( conn_table == NULL )
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," conn table NULL " );
         status = OF_FAILURE;
         break;
      }
      meter_config_req_multipart_msg = (struct ofp_multipart_request *)(msg->desc.start_of_data);
      meter_config_req_multipart_msg->header.version = OFP_VERSION;
      meter_config_req_multipart_msg->header.type    = OFPT_MULTIPART_REQUEST;
      meter_config_req_multipart_msg->header.xid     = 0;
      meter_config_req_multipart_msg->type = htons(OFPMP_METER_CONFIG);

      msg->desc.data_len += (OFU_METER_CONFIG_REQ_MESSAGE_LEN);
      meter_config_req_multipart_msg->header.length = htons(msg->desc.data_len);

      CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,datapath,conn_table,conn_safe_ref,
            (void*)meter_config_cbk,cbk_arg1,cbk_arg2,send_status);
      if(send_status  == FALSE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error in sending message to datapath");
         status = CNTLR_SEND_MSG_TO_DP_ERROR;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return status;
}

   int32_t
of_get_meter_features( struct of_msg             *msg,
      uint64_t                  datapath_handle,
      of_meter_features_cbk_fn  meter_features_cbk,
      void                      *cbk_arg1,
      void                      *cbk_arg2,
      uint32_t                  *xid)

{
   struct ofp_multipart_request   *meter_feature_multipart_req;
   struct dprm_datapath_entry   *datapath;
   cntlr_conn_node_saferef_t conn_safe_ref;
   cntlr_conn_table_t        *conn_table;
   uint8_t  send_status = TRUE;
   int32_t  status = OF_SUCCESS;
   int32_t  retval = OF_SUCCESS;


   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"entered ");

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      if(msg->desc.buffer_len != OFU_METER_FEATURE_REQ_MESSAGE_LEN)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"Allocated message buffer length is not suffecient");
         status = OF_MSG_BUF_SIZE_IS_IN_SUFFECIENT;
      }

      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in getting datapath,Err=%d",retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      if(!datapath->is_main_conn_present)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in sending message to datapath,channel closed "
               );
//         msg->desc.free_cbk(msg);
         status = CNTLR_CHN_CLOSED;
         break;
      }
      CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);
      if ( conn_table == NULL )
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"conn table NULL");
         status = OF_FAILURE;
         break;
      }

      meter_feature_multipart_req = (struct ofp_multipart_request *)(msg->desc.start_of_data);
      meter_feature_multipart_req->header.version = OFP_VERSION;
      meter_feature_multipart_req->header.type    = OFPT_MULTIPART_REQUEST;
      meter_feature_multipart_req->header.xid     = 0;
      meter_feature_multipart_req->type = htons(OFPMP_METER_FEATURES);

      msg->desc.data_len += (OFU_METER_FEATURE_REQ_MESSAGE_LEN);
      meter_feature_multipart_req->header.length = htons(msg->desc.data_len);

      CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,datapath,conn_table,conn_safe_ref,
            (void*)meter_features_cbk,cbk_arg1,cbk_arg2,send_status);
      if(send_status  == FALSE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"Error in sending message to datapath");
         status = CNTLR_SEND_MSG_TO_DP_ERROR;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return status;
}

   int32_t
of_get_meter_statistics(struct of_msg         *msg,
      uint64_t              datapath_handle,
      uint32_t              meter_id,
      of_meter_stats_cbk_fn meter_stats_cbk,
      void                  *cbk_arg1,
      void                  *cbk_arg2)
{
   struct ofp_multipart_request   *meter_stat_multipart_req;
   struct ofp_meter_multipart_request  *meter_stat_req;
   struct dprm_datapath_entry   *datapath;
   cntlr_conn_node_saferef_t conn_safe_ref;
   cntlr_conn_table_t        *conn_table;
   uint8_t  send_status = TRUE;
   int32_t  status = OF_SUCCESS;
   int32_t  retval = OF_SUCCESS;


   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO," entered ");

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      if(msg->desc.buffer_len != OFU_METER_STATS_REQ_MESSAGE_LEN)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Allocated message buffer length is not suffecient "
               );
         status = OF_MSG_BUF_SIZE_IS_IN_SUFFECIENT;
      }

      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in getting datapath,Err=%d ",
               retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      if(!datapath->is_main_conn_present)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in sending message to datapath,channel closed "
               );
//         msg->desc.free_cbk(msg);
         status = CNTLR_CHN_CLOSED;
         break;
      }
      CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);
      if ( conn_table == NULL )
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," conn table NULL ");
         status = OF_FAILURE;
         break;
      }

      meter_stat_multipart_req = (struct ofp_multipart_request *)(msg->desc.start_of_data);
      meter_stat_multipart_req->header.version = OFP_VERSION;
      meter_stat_multipart_req->header.type    = OFPT_MULTIPART_REQUEST;
      meter_stat_multipart_req->header.xid     = 0;
      meter_stat_multipart_req->type = htons(OFPMP_METER);

      meter_stat_req = (struct ofp_meter_multipart_request*)(meter_stat_multipart_req->body);

      meter_stat_req->meter_id  = htonl(meter_id);

      msg->desc.data_len += (OFU_METER_STATS_REQ_MESSAGE_LEN);
      meter_stat_multipart_req->header.length = htons(msg->desc.data_len);


      CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,datapath,conn_table,conn_safe_ref,
            (void*)meter_stats_cbk,cbk_arg1,cbk_arg2,send_status);
      if(send_status  == FALSE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in sending message to datapath ");
         status = CNTLR_SEND_MSG_TO_DP_ERROR;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return status;
}

   int32_t
of_get_group_statistics( struct of_msg         *msg,
      uint64_t              datapath_handle,
      uint32_t              group_id,
      of_group_stats_cbk_fn group_stats_cbk,
      void                  *cbk_arg1,
      void                  *cbk_arg2)
{
   struct ofp_multipart_request   *group_stat_multipart_req;
   struct ofp_group_stats_request  *group_stat_req;
   struct dprm_datapath_entry   *datapath;
   cntlr_conn_node_saferef_t conn_safe_ref;
   cntlr_conn_table_t        *conn_table;
   uint8_t  send_status = TRUE;
   int32_t  status = OF_SUCCESS;
   int32_t  retval = OF_SUCCESS;


   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO," entered ");

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      if(msg->desc.buffer_len != OFU_GROUP_STATS_REQ_MESSAGE_LEN)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Allocated message buffer length is not suffecient "
               );
         status = OF_MSG_BUF_SIZE_IS_IN_SUFFECIENT;
      }

      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in getting datapath,Err=%d "
               ,retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      if(!datapath->is_main_conn_present)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in sending message to datapath,channel closed "
               );
//         msg->desc.free_cbk(msg);
         status = CNTLR_CHN_CLOSED;
         break;
      }
      CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);
      if ( conn_table == NULL )
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," conn table NULL ");
         status = OF_FAILURE;
         break;
      }

      group_stat_multipart_req = (struct ofp_multipart_request *)(msg->desc.start_of_data);
      group_stat_multipart_req->header.version = OFP_VERSION;
      group_stat_multipart_req->header.type    = OFPT_MULTIPART_REQUEST;
      group_stat_multipart_req->header.xid     = 0;
      group_stat_multipart_req->type = htons(OFPMP_GROUP);

      group_stat_req =
         (struct ofp_group_stats_request*)(group_stat_multipart_req->body);

      group_stat_req->group_id  = htonl(group_id);

      msg->desc.data_len += (OFU_GROUP_STATS_REQ_MESSAGE_LEN);
      group_stat_multipart_req->header.length = htons(msg->desc.data_len);


      CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,datapath,conn_table,conn_safe_ref,
            (void*)group_stats_cbk,cbk_arg1,cbk_arg2,send_status);
      if(send_status  == FALSE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in sending message to datapath ");
         status = CNTLR_SEND_MSG_TO_DP_ERROR;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return status;
}

   int32_t
of_get_groups_features( struct of_msg             *msg,
      uint64_t                  datapath_handle,
      of_group_features_cbk_fn  group_features_cbk,
      void                      *cbk_arg1,
      void                      *cbk_arg2,
      uint32_t                  *xid)

{
   struct ofp_multipart_request   *group_feature_multipart_req;
   struct dprm_datapath_entry   *datapath;
   cntlr_conn_node_saferef_t conn_safe_ref;
   cntlr_conn_table_t        *conn_table;
   uint8_t  send_status = TRUE;
   int32_t  status = OF_SUCCESS;
   int32_t  retval = OF_SUCCESS;


   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"entered ");

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      if(msg->desc.buffer_len != OFU_GROUP_FEATURES_REQ_MESSAGE_LEN)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"Allocated message buffer length is not suffecient");
         status = OF_MSG_BUF_SIZE_IS_IN_SUFFECIENT;
      }

      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in getting datapath,Err=%d",retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      if(!datapath->is_main_conn_present)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in sending message to datapath,channel closed "
               );
//         msg->desc.free_cbk(msg);
         status = CNTLR_CHN_CLOSED;
         break;
      }
      CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);
      if ( conn_table == NULL )
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"conn table NULL");
         status = OF_FAILURE;
         break;
      }

      group_feature_multipart_req = (struct ofp_multipart_request *)(msg->desc.start_of_data);
      group_feature_multipart_req->header.version = OFP_VERSION;
      group_feature_multipart_req->header.type    = OFPT_MULTIPART_REQUEST;
      group_feature_multipart_req->header.xid     = 0;
      group_feature_multipart_req->type = htons(OFPMP_GROUP_FEATURES);


      msg->desc.data_len += (OFU_GROUP_FEATURES_REQ_MESSAGE_LEN);
      group_feature_multipart_req->header.length = htons(msg->desc.data_len);

      CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,datapath,conn_table,conn_safe_ref,
            (void*)group_features_cbk,cbk_arg1,cbk_arg2,send_status);
      if(send_status  == FALSE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"Error in sending message to datapath");
         status = CNTLR_SEND_MSG_TO_DP_ERROR;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return status;
}




   int32_t
of_send_role_change_msg(struct of_msg *msg,
      uint64_t   datapath_handle,
      uint32_t   new_role,
      uint64_t   generation_id,
      of_controller_role_change_notification_fn role_change_cbk_fn,
      void      *clbk_arg1,
      void      *clbk_arg2)

{

   struct ofp_role_request    *role_req_msg=NULL;
   struct dprm_datapath_entry *datapath=NULL;
   cntlr_conn_node_saferef_t   conn_safe_ref={};
   cntlr_conn_table_t        *conn_table=NULL;
   uint8_t  send_status = TRUE;
   int32_t  status = OF_SUCCESS;
   int32_t  retval = OF_SUCCESS;

   cntlr_assert(msg != NULL);

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      if(msg->desc.buffer_len != OFU_ROLE_REQUEST_MESSAGE_LEN)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Allocated message buffer length is not suffecient "
               );
         status = OF_MSG_BUF_SIZE_IS_IN_SUFFECIENT;
      }

      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in getting datapath,Err=%d "
               ,retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      role_req_msg = (struct ofp_role_request *)(msg->desc.start_of_data);
      role_req_msg->header.version = OFP_VERSION;
      role_req_msg->header.type    = OFPT_ROLE_REQUEST;
      role_req_msg->header.xid     = 0;
      role_req_msg->role = htonl(new_role);
      role_req_msg->generation_id = htonll(generation_id);
      msg->desc.data_len = sizeof(struct ofp_role_request);
      role_req_msg->header.length  = htons(msg->desc.data_len);

      if(!datapath->is_main_conn_present)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in sending message to datapath,channel closed "
               );
         //msg->desc.free_cbk(msg);
         status = CNTLR_CHN_CLOSED;
         break;
      }
      CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,
            conn_safe_ref,
            conn_table
            );
      if ( conn_table == NULL )
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"conn table NULL");
         status = OF_FAILURE;
         break;
      }

      CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,
            datapath,
            conn_table,
            conn_safe_ref,
            (void*)role_change_cbk_fn,
            clbk_arg1,clbk_arg2,
            send_status
            );
      if(send_status  == FALSE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in sending message to datapath ");
         status = CNTLR_SEND_MSG_TO_DP_ERROR;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return status;
}


   int32_t
of_get_port_description(struct of_msg *msg,
      uint64_t datapath_handle,
      of_port_desc_cbk_fn port_desc_cbk,
      void *cbk_arg1,
      void *cbk_arg2)
{
   struct ofp_multipart_request *desc_req_msg;
   struct dprm_datapath_entry   *datapath;
   cntlr_conn_node_saferef_t conn_safe_ref;
   cntlr_conn_table_t        *conn_table;
   uint8_t  send_status = TRUE;
   int32_t  status = OF_SUCCESS;
   int32_t  retval = OF_SUCCESS;

   cntlr_assert(msg != NULL);
   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," entered ")

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      if(msg->desc.buffer_len != OFU_PORT_DESCRIPTION_REQ_MESSAGE_LEN)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Allocated message buffer length is not suffecient "
               );
         status = OF_MSG_BUF_SIZE_IS_IN_SUFFECIENT;
      }

      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in getting datapath,Err=%d "
               ,retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      desc_req_msg = (struct ofp_multipart_request *)(msg->desc.start_of_data);
      desc_req_msg->header.version = OFP_VERSION;
      desc_req_msg->header.type    = OFPT_MULTIPART_REQUEST;
      desc_req_msg->header.xid     = 0;
      desc_req_msg->type = htons(OFPMP_PORT_DESC);
      msg->desc.data_len = sizeof(struct ofp_multipart_request);
      desc_req_msg->header.length  = htons(msg->desc.data_len);

      if(!datapath->is_main_conn_present)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in sending message to datapath,channel closed "
               );
//         msg->desc.free_cbk(msg);
         status = CNTLR_CHN_CLOSED;
         break;
      }
      CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);
      if ( conn_table == NULL )
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"conn table NULL");
         status = OF_FAILURE;
         break;
      }
      CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,datapath,conn_table,conn_safe_ref,
            (void*)port_desc_cbk,cbk_arg1,cbk_arg2,send_status);
      if(send_status  == FALSE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in sending message to datapath ");
         status = CNTLR_SEND_MSG_TO_DP_ERROR;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   return status;
}

   int32_t
of_get_table_statistics(struct of_msg *msg,
      uint64_t datapath_handle,
      of_table_stats_cbk_fn table_stats_cbk,
      void *cbk_arg1,
      void *cbk_arg2
      )
{
   struct ofp_multipart_request *table_stats_multipart_req;
   struct dprm_datapath_entry   *datapath;
   cntlr_conn_node_saferef_t conn_safe_ref;
   cntlr_conn_table_t        *conn_table;
   uint8_t  send_status = TRUE;
   int32_t  status = OF_SUCCESS;
   int32_t  retval = OF_SUCCESS;

   cntlr_assert(msg != NULL);

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      if(msg->desc.buffer_len != OFU_TABLE_STATISTICS_REQ_MESSAGE_LEN)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Allocated message buffer length is not suffecient "
               );
         status = OF_MSG_BUF_SIZE_IS_IN_SUFFECIENT;
      }

      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in getting datapath,Err=%d "
               ,retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      if(!datapath->is_main_conn_present)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in sending message to datapath,channel closed "
               );
//         msg->desc.free_cbk(msg);
         status = CNTLR_CHN_CLOSED;
         break;
      }
      CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);
      if ( conn_table == NULL )
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"conn table NULL ");
         status = OF_FAILURE;
         break;
      }
      table_stats_multipart_req = (struct ofp_multipart_request *)(msg->desc.start_of_data);
      table_stats_multipart_req->header.version = OFP_VERSION;
      table_stats_multipart_req->header.type    = OFPT_MULTIPART_REQUEST;
      table_stats_multipart_req->header.xid     = 0;
      table_stats_multipart_req->type = htons(OFPMP_TABLE);

      msg->desc.data_len += (OFU_INDIVIUAL_TABLE_STATS_REQ_MESSAGE_LEN );
      table_stats_multipart_req->header.length = htons(msg->desc.data_len);


      CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,datapath,conn_table,conn_safe_ref,
            (void*)table_stats_cbk,cbk_arg1,cbk_arg2,send_status);
      if(send_status  == FALSE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in sending message to datapath ");
         status = CNTLR_SEND_MSG_TO_DP_ERROR;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   return status;
}



   int32_t
of_get_port_statistics(struct of_msg *msg,
      uint64_t datapath_handle,
      uint32_t port_no,
      of_port_stats_cbk_fn port_stats_cbk,
      void *cbk_arg1,
      void *cbk_arg2
      )
{
   struct ofp_multipart_request *port_stats_req_multipart_msg;
   struct ofp_port_stats_request  *port_stat_req;
   struct dprm_datapath_entry   *datapath;
   cntlr_conn_node_saferef_t conn_safe_ref;
   cntlr_conn_table_t        *conn_table;
   uint8_t  send_status = TRUE;
   int32_t  status = OF_SUCCESS;
   int32_t  retval = OF_SUCCESS;

   cntlr_assert(msg != NULL);

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      if(msg->desc.buffer_len != OFU_PORT_STATISTICS_REQ_MESSAGE_LEN)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Allocated message buffer length is not suffecient "
               );
         status = OF_MSG_BUF_SIZE_IS_IN_SUFFECIENT;
         break;
      }

      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in getting datapath,Err=%d "
               ,retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      if(!datapath->is_main_conn_present)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in sending message to datapath,channel closed "
               );
//         msg->desc.free_cbk(msg);
         status = CNTLR_CHN_CLOSED;
         break;
      }
      CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);
      if ( conn_table == NULL )
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"conn table NULL ");
         status = OF_FAILURE;
         break;
      }
      port_stats_req_multipart_msg = (struct ofp_multipart_request *)(msg->desc.start_of_data);
      port_stats_req_multipart_msg->header.version = OFP_VERSION;
      port_stats_req_multipart_msg->header.type    = OFPT_MULTIPART_REQUEST;
      port_stats_req_multipart_msg->header.xid     = 0;
      port_stats_req_multipart_msg->type = htons(OFPMP_PORT_STATS);
      port_stat_req =
         (struct ofp_port_stats_request*)(port_stats_req_multipart_msg->body);

      port_stat_req->port_no = port_no;

      msg->desc.data_len += (OFU_INDIVIUAL_PORT_STATS_REQ_MESSAGE_LEN );
      port_stats_req_multipart_msg->header.length = htons(msg->desc.data_len);


      CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,datapath,conn_table,conn_safe_ref,
            (void*)port_stats_cbk,cbk_arg1,cbk_arg2,send_status);
      if(send_status  == FALSE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in sending message to datapath ");
         status = CNTLR_SEND_MSG_TO_DP_ERROR;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   return status;
}
//Table_feature request for auto learning of tables in the switch
   int32_t
of_get_table_features( struct of_msg *msg,
      uint64_t datapath_handle,
      of_table_features_cbk_fn table_features_cbk, 
      void *cbk_arg1,
      void *cbk_arg2)
{
   struct ofp_multipart_request   *table_feature_req_multipart_msg;
   struct dprm_datapath_entry     *datapath;
   cntlr_conn_node_saferef_t       conn_safe_ref;
   cntlr_conn_table_t             *conn_table;
   int32_t                         retval = OF_SUCCESS;
   int32_t                         status = OF_SUCCESS;
   uint8_t  send_status = TRUE;

   //return OF_FAILURE;
   cntlr_assert(msg != NULL);

   CNTLR_RCU_READ_LOCK_TAKE();

   do
   {
      if(msg->desc.buffer_len != OFU_TABLE_FEATURES_REQ_MESSAGE_LEN)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Allocated message buffer length is not suffecient "
               );
         status = OF_MSG_BUF_SIZE_IS_IN_SUFFECIENT;
         break;
      }
      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in getting datapath,Err=%d "
               ,retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }
      if(!datapath->is_main_conn_present)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in sending message to datapath,channel closed "
               );
//         msg->desc.free_cbk(msg);
         status = CNTLR_CHN_CLOSED;
         break;
      }
      CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);
      if ( conn_table == NULL )
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"conn table NULL ");
         status = OF_FAILURE;
         break;
      }

      /*Build message content*/
      table_feature_req_multipart_msg = (struct ofp_multipart_request*)(msg->desc.start_of_data);
      table_feature_req_multipart_msg->header.version  = OFP_VERSION;
      table_feature_req_multipart_msg->header.type     = OFPT_MULTIPART_REQUEST; 
      table_feature_req_multipart_msg->header.xid      = 0;
      table_feature_req_multipart_msg->type     = htons(OFPMP_TABLE_FEATURES);
      msg->desc.data_len  += OFU_TABLE_FEATURES_REQ_MESSAGE_LEN;
      table_feature_req_multipart_msg->header.length   = htons(msg->desc.data_len);

      CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,datapath,conn_table,conn_safe_ref,
            (void*)table_features_cbk,NULL, NULL,send_status);
      if(send_status == FALSE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in sending message to datapath ");
         status = CNTLR_SEND_MSG_TO_DP_ERROR;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   return status;
}


   int32_t
of_get_flow_entries(struct of_msg *msg,
      uint64_t datapath_handle,
      uint8_t  table_id,
      uint32_t out_port,
      uint32_t out_group,
      uint64_t cookie,
      uint64_t cookie_mask,
      of_flow_entry_stats_cbk_fn flow_entry_cbk_fn,
      void *cbk_arg1,
      void *cbk_arg2)
{
   struct ofp_multipart_request   *flow_entry_req_multipart_msg;
   struct ofp_flow_stats_request  *flow_entry_stat_req;
   struct dprm_datapath_entry     *datapath;
   cntlr_conn_node_saferef_t       conn_safe_ref;
   cntlr_conn_table_t             *conn_table;
   uint8_t                         send_status = TRUE;
   int32_t                         retval = OF_SUCCESS;
   int32_t                         status = OF_SUCCESS;

   cntlr_assert(msg != NULL);

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      if(msg->desc.buffer_len !=  OFU_INDIVIUAL_FLOW_ENTRY_STATS_REQ_MESSAGE_LEN)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Allocated message buffer length is not suffecient "
               );
         status = OF_MSG_BUF_SIZE_IS_IN_SUFFECIENT;
      }
      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in getting datapath,Err=%d "
               ,retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      if(!datapath->is_main_conn_present)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in sending message to datapath,channel closed "
               );
//         msg->desc.free_cbk(msg);
         status = CNTLR_CHN_CLOSED;
         break;
      }
      CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);
      if ( conn_table == NULL )
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"conn table NULL ");
         status = OF_FAILURE;
         break;
      }
      flow_entry_req_multipart_msg = (struct ofp_multipart_request *)(msg->desc.start_of_data);
      flow_entry_req_multipart_msg->header.version = OFP_VERSION;
      flow_entry_req_multipart_msg->header.type    = OFPT_MULTIPART_REQUEST;
      flow_entry_req_multipart_msg->header.xid     = 0;
      flow_entry_req_multipart_msg->type = htons(OFPMP_FLOW);

      flow_entry_stat_req = 
         (struct ofp_flow_stats_request*)(flow_entry_req_multipart_msg->body);

      flow_entry_stat_req->table_id    = table_id; 
      flow_entry_stat_req->out_port    =htonl( out_port); 
      flow_entry_stat_req->out_group   =htonl(out_group); 
      flow_entry_stat_req->cookie      = htonll(cookie); 
      flow_entry_stat_req->cookie_mask = htonll(cookie_mask); 
      flow_entry_stat_req->match.type = htons(OFPMT_OXM);
      flow_entry_stat_req->match.length = htons(sizeof(struct ofp_match) -4);

      msg->desc.data_len += (OFU_INDIVIUAL_FLOW_ENTRY_STATS_REQ_MESSAGE_LEN  );
      flow_entry_req_multipart_msg->header.length = htons(msg->desc.data_len); 

      if(!datapath->is_main_conn_present)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in sending message to datapath,channel closed "
               );
//         msg->desc.free_cbk(msg);
         status = CNTLR_CHN_CLOSED;
         break;
      }
      CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);
      if ( conn_table == NULL )
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"conn table NULL");
         status = OF_FAILURE;
         break;
      }
      CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,datapath,conn_table,conn_safe_ref,
            (void*) flow_entry_cbk_fn,cbk_arg1,cbk_arg2,send_status);
      if(send_status == FALSE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in sending message to datapath ");
         status = CNTLR_SEND_MSG_TO_DP_ERROR;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   return status;
}


/*Vinod */
   int32_t
of_get_bind_entries(struct of_msg *msg,
      uint64_t datapath_handle,
      uint8_t  bind_obj_id,
      of_bind_stats_cbk_fn bind_entry_cbk_fn,
      void *cbk_arg1,
      void *cbk_arg2)
{
   struct fslx_multipart_request      *bind_entry_req_multipart_msg;
   struct fslx_bind_obj_info_request  *bind_entry_stat_req;
   struct dprm_datapath_entry         *datapath;
   cntlr_conn_node_saferef_t           conn_safe_ref;
   cntlr_conn_table_t                 *conn_table;
   uint8_t                             send_status = TRUE;
   int32_t                             retval = OF_SUCCESS;
   int32_t                             status = OF_SUCCESS;

   cntlr_assert(msg != NULL);

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      if(msg->desc.buffer_len !=  OFU_INDIVIUAL_BIND_ENTRY_STATS_REQ_MESSAGE_LEN)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Allocated message buffer length is not suffecient "
               );
         status = OF_MSG_BUF_SIZE_IS_IN_SUFFECIENT;
      }
      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in getting datapath,Err=%d "
               ,retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }
      if(!datapath->is_main_conn_present)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in sending message to datapath,channel closed "
               );
//         msg->desc.free_cbk(msg);
         status = CNTLR_CHN_CLOSED;
         break;
      }
      CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);
      if ( conn_table == NULL )
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"conn table NULL ");
         status = OF_FAILURE;
         break;
      }
      bind_entry_req_multipart_msg = (struct fslx_multipart_request *)(msg->desc.start_of_data);
      bind_entry_req_multipart_msg->header.version = OFP_VERSION;
      bind_entry_req_multipart_msg->header.type    = OFPT_MULTIPART_REQUEST;
      bind_entry_req_multipart_msg->header.xid     = 0;
      bind_entry_req_multipart_msg->type           = htons(OFPMP_EXPERIMENTER);
      bind_entry_req_multipart_msg->subtype        = htons(FSLX_MP_BO_INFO);

      bind_entry_stat_req =
         (struct fslx_bind_obj_info_request*)(bind_entry_req_multipart_msg->body);

      bind_entry_stat_req->bind_obj_id = htons(bind_obj_id);

      msg->desc.data_len += (OFU_INDIVIUAL_BIND_ENTRY_STATS_REQ_MESSAGE_LEN  );
      bind_entry_req_multipart_msg->header.length = htons(msg->desc.data_len);

      if(!datapath->is_main_conn_present)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in sending message to datapath,channel closed "
               );
//         msg->desc.free_cbk(msg);
         status = CNTLR_CHN_CLOSED;
         break;
      }
      CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);
      if ( conn_table == NULL )
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"conn table NULL");
         status = OF_FAILURE;
         break;
      }
      CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,datapath,conn_table,conn_safe_ref,
            (void*) bind_entry_cbk_fn,cbk_arg1,cbk_arg2,send_status);
      if(send_status == FALSE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in sending message to datapath ");
         status = CNTLR_SEND_MSG_TO_DP_ERROR;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   return status;
}

     


   int32_t
of_get_aggregate_flow_entries(struct of_msg *msg, 
      uint64_t datapath_handle,
      uint8_t  table_id,
      uint32_t out_port,
      uint32_t out_group,
      uint64_t cookie,
      uint64_t cookie_mask,
      of_aggregate_stats_cbk_fn aggregat_stats_cbk_fn, 
      void *cbk_arg1,
      void *cbk_arg2)
{
   struct ofp_multipart_request   *aggr_stats_req_multipart_msg;
   struct ofp_aggregate_stats_request *aggr_stat_req;
   struct dprm_datapath_entry     *datapath;
   cntlr_conn_node_saferef_t       conn_safe_ref;
   cntlr_conn_table_t             *conn_table;
   uint8_t  send_status = TRUE;
   int32_t                         retval = OF_SUCCESS;
   int32_t                         status = OF_SUCCESS;


   cntlr_assert(msg != NULL);

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      if(msg->desc.buffer_len != OFU_AGGREGATE_FLOW_ENTRY_STATS_REQ_MESSAGE_LEN)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Allocated message buffer length is not suffecient "
               );
         status = OF_MSG_BUF_SIZE_IS_IN_SUFFECIENT;
      }
      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in getting datapath,Err=%d "
               ,retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      if(!datapath->is_main_conn_present)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in sending message to datapath,channel closed "
               );
//         msg->desc.free_cbk(msg);
         status = CNTLR_CHN_CLOSED;
         break;
      }
      CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);
      if ( conn_table == NULL )
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"conn table NULL ");
         status = OF_FAILURE;
         break;
      }
      aggr_stats_req_multipart_msg = (struct ofp_multipart_request *)(msg->desc.start_of_data);
      aggr_stats_req_multipart_msg->header.version = OFP_VERSION;
      aggr_stats_req_multipart_msg->header.type    = OFPT_MULTIPART_REQUEST;
      aggr_stats_req_multipart_msg->header.xid     = 0;
      aggr_stats_req_multipart_msg->type = htons(OFPMP_AGGREGATE);

      aggr_stat_req= 
         (struct ofp_aggregate_stats_request*)(aggr_stats_req_multipart_msg->body);

      aggr_stat_req->table_id    = table_id; 
      aggr_stat_req->out_port    = htonl(out_port); 
      aggr_stat_req->out_group   = htonl(out_group); 
      aggr_stat_req->cookie      = htonll(cookie); 
      aggr_stat_req->cookie_mask = htonll(cookie_mask); 
      aggr_stat_req->match.type = htons(OFPMT_OXM);
      aggr_stat_req->match.length =  htons(sizeof(struct ofp_match) -4); 
      msg->desc.data_len += (OFU_AGGREGATE_FLOW_ENTRY_STATS_REQ_MESSAGE_LEN);
      aggr_stats_req_multipart_msg->header.length = htons(msg->desc.data_len); 

      if(!datapath->is_main_conn_present)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in sending message to datapath,channel closed "
               );
//         msg->desc.free_cbk(msg);
         status = CNTLR_CHN_CLOSED;
         break;
      }
      CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);
      if ( conn_table == NULL )
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"conn table NULL");
         status = OF_FAILURE;
         break;
      }
      CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,datapath,conn_table,conn_safe_ref,
            (void*) aggregat_stats_cbk_fn,cbk_arg1,cbk_arg2,send_status);
      if(send_status == FALSE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in sending message to datapath ");
         status = CNTLR_SEND_MSG_TO_DP_ERROR;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return status;
}

   int32_t
of_add_group(struct of_msg *msg,
      uint64_t  datapath_handle,
      uint32_t  group_id,
      uint8_t   group_type,
      void **conn_info_pp)
{
   struct ofp_group_mod *group_mod;
   struct dprm_datapath_entry  *datapath;
   int32_t  status = OFU_ADD_GROUP_ENTRY_SUCCESS;
   int32_t  retval = OF_SUCCESS;
   struct of_cntlr_conn_info *conn_info;

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"Error in getting datapath,Err=%d ",
               retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      group_mod = (struct ofp_group_mod*)(msg->desc.start_of_data);


      group_mod->header.version = OFP_VERSION;
      group_mod->header.type    = OFPT_GROUP_MOD;
      group_mod->header.xid     = 0;

      group_mod->command  = htons(OFPGC_ADD);
      group_mod->type = group_type;
      group_mod->group_id = htonl(group_id);

      msg->desc.data_len += (sizeof(struct ofp_group_mod));
      msg->desc.utils_ptr   = msg->desc.start_of_data + msg->desc.data_len;
      msg->desc.ptr1=  msg->desc.ptr2=  msg->desc.ptr3 =msg->desc.utils_ptr;
      msg->desc.length1=  msg->desc.length2 =  msg->desc.length3=0;

      if(!datapath->is_main_conn_present)
      {
	      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"main connection not present");
	      status=OF_FAILURE;
	      break;
      }
      CNTLR_GET_MAIN_CHANNEL_INFO(datapath,conn_info);
      *conn_info_pp=conn_info;
      if ( conn_info == NULL )
      {
	      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"conn info is null");
	      status=OF_FAILURE;
	      break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return status;
}

int32_t
of_cntrlr_send_request_message(struct of_msg *msg,
      uint64_t  datapath_handle,
      void *conn_info,
      void *clbk_command_arg1,
      void *clbk_command_arg2)
{
   struct ofp_header *header;
   struct dprm_datapath_entry  *datapath;
   int32_t  status = OF_SUCCESS;
   int32_t  retval = OF_SUCCESS;
   uint8_t  send_status = TRUE;
   cntlr_conn_node_saferef_t    conn_safe_ref;
   cntlr_conn_table_t          *conn_table= ((struct of_cntlr_conn_info*)conn_info)->conn_table;


   memcpy(&conn_safe_ref, &((struct of_cntlr_conn_info*)conn_info)->conn_safe_ref, sizeof(cntlr_conn_node_saferef_t));
   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      header = (struct ofp_header*)(msg->desc.start_of_data);
      header->length = htons(msg->desc.data_len);
      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error in getting datapath,Err=%d ",
               retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,datapath,
            //      ((struct of_cntlr_conn_info *)conn_info)->conn_table,
            //      ((struct of_cntlr_conn_info*)conn_info)->conn_safe_ref,
            conn_table, conn_safe_ref,
            NULL,clbk_command_arg1,clbk_command_arg2,send_status);
      if(send_status == FALSE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error in sending message to datapath");
         status = CNTLR_SEND_MSG_TO_DP_ERROR;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   free(conn_info);
   return status;
}

   int32_t
of_modify_group(struct of_msg *msg,
      uint64_t  datapath_handle,
      uint32_t  group_id,
      uint8_t   group_type,
      void **conn_info_pp)
{
   struct ofp_group_mod *group_mod;
   struct dprm_datapath_entry  *datapath;
   //cntlr_conn_node_saferef_t    conn_safe_ref;
   //cntlr_conn_table_t          *conn_table;
   //uint8_t  send_status = TRUE;
   int32_t  status = OFU_MODIFY_GROUP_ENTRY_SUCCESS;
   int32_t  retval = OF_SUCCESS;
   struct of_cntlr_conn_info *conn_info;


   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in getting datapath,Err=%d",
               retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      group_mod = (struct ofp_group_mod*)(msg->desc.start_of_data);


      group_mod->header.version = OFP_VERSION;
      group_mod->header.type    = OFPT_GROUP_MOD;
      group_mod->header.xid     = 0;

      group_mod->command  = htons(OFPGC_MODIFY);
      group_mod->type = group_type;
      group_mod->group_id = htonl(group_id);
      //      group_mod->buckets = ; /* To BE DONE */

      msg->desc.data_len += (sizeof(struct ofp_group_mod));
      //      group_mod->header.length = htons(msg->desc.data_len); 

      msg->desc.utils_ptr   = msg->desc.start_of_data + msg->desc.data_len;
      msg->desc.ptr1=  msg->desc.ptr2=  msg->desc.ptr3 =msg->desc.utils_ptr;
      msg->desc.length1=  msg->desc.length2 =  msg->desc.length3=0;
      if(!datapath->is_main_conn_present)
      {
	      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"main connection not present");
	      status=OF_FAILURE;
	      break;
      }
      CNTLR_GET_MAIN_CHANNEL_INFO(datapath,conn_info);
      *conn_info_pp=conn_info;
      if ( conn_info == NULL )
      {
	      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"conn info is null");
	      status=OF_FAILURE;
	      break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   return status;
}


   int32_t
of_delete_group(struct of_msg *msg,
      uint64_t  datapath_handle,
      uint32_t  group_id,
      void **conn_info_pp)
{
   struct ofp_group_mod *group_mod;
   struct dprm_datapath_entry  *datapath;
   int32_t  status = OFU_DELETE_GROUP_ENTRY_SUCCESS;
   int32_t  retval = OF_SUCCESS;
   struct of_cntlr_conn_info *conn_info;

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,
               "Error in getting datapath,Err=%d ",retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      group_mod = (struct ofp_group_mod*)(msg->desc.start_of_data);


      group_mod->header.version = OFP_VERSION;
      group_mod->header.type    = OFPT_GROUP_MOD;
      group_mod->header.xid     = 0;

      group_mod->command  = htons(OFPGC_DELETE);
      group_mod->group_id = htonl(group_id);
      //      group_mod->buckets = ; /* To BE DONE */

      msg->desc.data_len += (sizeof(struct ofp_group_mod));
      //group_mod->header.length = htons(msg->desc.data_len); 

      msg->desc.utils_ptr   = msg->desc.start_of_data + msg->desc.data_len;
      msg->desc.ptr1=  msg->desc.ptr2=  msg->desc.ptr3 =msg->desc.utils_ptr;
      msg->desc.length1=  msg->desc.length2 =  msg->desc.length3=0;
      if(!datapath->is_main_conn_present)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"main connection not present");
         status=OF_FAILURE;
         break;
      }
      CNTLR_GET_MAIN_CHANNEL_INFO(datapath,conn_info);
      *conn_info_pp=conn_info;
      if ( conn_info == NULL )
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"conn info is null");
         status=OF_FAILURE;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   return status;
}

int32_t of_add_meter(struct of_msg *msg,
      uint64_t  datapath_handle,
      uint32_t  meter_id,
      uint16_t   meter_type,
      void **conn_info_pp)
{
   struct ofp_meter_mod *meter_mod;
   struct dprm_datapath_entry  *datapath;
   int32_t  status = OFU_ADD_METER_ENTRY_SUCCESS;
   int32_t  retval = OF_SUCCESS;
   struct of_cntlr_conn_info *conn_info;
   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"Error in getting datapath,Err=%d ",
               retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      meter_mod = (struct ofp_meter_mod*)(msg->desc.start_of_data);


      meter_mod->header.version = OFP_VERSION;
      meter_mod->header.type    = OFPT_METER_MOD;
      meter_mod->header.xid     = 0;

      meter_mod->command  = htons(OFPMC_ADD);
      meter_mod->flags = htons(meter_type);
      meter_mod->meter_id = htonl(meter_id);

      msg->desc.data_len += (sizeof(struct ofp_meter_mod));
      msg->desc.utils_ptr   = msg->desc.start_of_data + msg->desc.data_len;
      msg->desc.ptr1=  msg->desc.ptr2=  msg->desc.ptr3 =msg->desc.utils_ptr;
      msg->desc.length1=  msg->desc.length2 =  msg->desc.length3=0;


      if(datapath->is_main_conn_present)
      {
         CNTLR_GET_MAIN_CHANNEL_INFO(datapath,conn_info);
         *conn_info_pp=conn_info;
         if ( conn_info == NULL )
         {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"conn info is null");
            status=OF_FAILURE;
            break;
         }
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();
   return status;
}


int32_t of_modify_meter(struct of_msg *msg,
      uint64_t  datapath_handle,
      uint32_t  meter_id,
      uint16_t   meter_type,
      void **conn_info_pp)
{
   struct ofp_meter_mod *meter_mod;
   struct dprm_datapath_entry  *datapath;
   int32_t  status = OFU_MODIFY_METER_ENTRY_SUCCESS;
   int32_t  retval = OF_SUCCESS;
   struct of_cntlr_conn_info *conn_info;


   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN," Error in getting datapath,Err=%d",
               retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      meter_mod = (struct ofp_meter_mod*)(msg->desc.start_of_data);


      meter_mod->header.version = OFP_VERSION;
      meter_mod->header.type    = OFPT_METER_MOD;
      meter_mod->header.xid     = 0;

      meter_mod->command  = htons(OFPMC_MODIFY);
      meter_mod->flags = htons(meter_type);
      meter_mod->meter_id = htonl(meter_id);

      msg->desc.data_len += (sizeof(struct ofp_meter_mod));

      msg->desc.utils_ptr   = msg->desc.start_of_data + msg->desc.data_len;
      msg->desc.ptr1=  msg->desc.ptr2=  msg->desc.ptr3 =msg->desc.utils_ptr;
      msg->desc.length1=  msg->desc.length2 =  msg->desc.length3=0;
      if(datapath->is_main_conn_present)
      {
         CNTLR_GET_MAIN_CHANNEL_INFO(datapath,conn_info);
         *conn_info_pp=conn_info;
         if ( conn_info == NULL )
         {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"conn info is null");
            status=OF_FAILURE;
            break;
         }
      }
      else
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"main connection not present");
         msg->desc.free_cbk(msg);
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   return status;
}


int32_t of_delete_meter(struct of_msg *msg,
      uint64_t  datapath_handle,
      uint32_t  meter_id,
      void **conn_info_pp)
{
   struct ofp_meter_mod *meter_mod;
   struct dprm_datapath_entry  *datapath;
   int32_t  status = OFU_DELETE_METER_ENTRY_SUCCESS;
   int32_t  retval = OF_SUCCESS;
   struct of_cntlr_conn_info *conn_info;

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,
               "Error in getting datapath,Err=%d ",retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      meter_mod = (struct ofp_meter_mod*)(msg->desc.start_of_data);

      meter_mod->header.version = OFP_VERSION;
      meter_mod->header.type    = OFPT_METER_MOD;
      meter_mod->header.xid     = 0;

      meter_mod->command  = htons(OFPMC_DELETE);
      meter_mod->meter_id = htonl(meter_id);

      msg->desc.data_len += (sizeof(struct ofp_meter_mod));

      msg->desc.utils_ptr   = msg->desc.start_of_data + msg->desc.data_len;
      msg->desc.ptr1=  msg->desc.ptr2=  msg->desc.ptr3 =msg->desc.utils_ptr;
      msg->desc.length1=  msg->desc.length2 =  msg->desc.length3=0;
      if (datapath->is_main_conn_present)
      {
         CNTLR_GET_MAIN_CHANNEL_INFO(datapath,conn_info);
         *conn_info_pp=conn_info;
         if ( conn_info == NULL )
         {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"conn info is null");
            status=OF_FAILURE;
            break;
         }
      }
      else
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"main connection not present");
         msg->desc.free_cbk(msg);
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   return status;
}

   int32_t
of_create_add_flow_entry_msg(
      struct of_msg *msg,
      uint64_t  datapath_handle,
      uint8_t   table_id,
      uint16_t  priority,
      uint8_t   command,
      uint64_t  cookie,
      uint64_t  cookie_mask,
      uint32_t  buffer_id,
      uint16_t  flags,
      uint16_t  hard_timeout,
      uint16_t  idle_timeout,
      void **conn_info_pp
      )
{
   struct ofp_flow_mod         *flow_mod;
   struct dprm_datapath_entry  *datapath;
   int32_t  status = OFU_ADD_FLOW_ENTRY_TO_TABLE_SUCCESS;
   int32_t  retval = OF_SUCCESS;
   struct ofp_match *match = NULL;
   struct of_cntlr_conn_info *conn_info;
   cntlr_assert(msg != NULL);

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Error in getting datapath,Err=%d ",
               retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"table %d Command id is = %d mask %llX",table_id, command,htonll(cookie_mask));

      flow_mod = (struct ofp_flow_mod*)(msg->desc.start_of_data);

      flow_mod->header.version = OFP_VERSION;
      flow_mod->header.type    = OFPT_FLOW_MOD;
      flow_mod->header.xid     = 0;

      flow_mod->cookie = htonll(cookie);
      flow_mod->cookie_mask = htonll(cookie_mask);
      flow_mod->table_id = table_id;
      flow_mod->priority = htons(priority);
      flow_mod->command  = command;
      flow_mod->buffer_id = htonl(buffer_id);
      flow_mod->hard_timeout = htons(hard_timeout);
      flow_mod->idle_timeout = htons(idle_timeout);
      flow_mod->out_port  = OFPP_ANY;
      flow_mod->out_group = OFPG_ANY;
      flow_mod->flags = htons(flags);

      msg->desc.data_len += (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN - 4);

      match = &((struct ofp_flow_mod*)(msg->desc.start_of_data))->match;
      msg->desc.utils_ptr   = match;


      if(datapath->is_main_conn_present)
      {
         CNTLR_GET_MAIN_CHANNEL_INFO(datapath,conn_info);
         *conn_info_pp=conn_info;
         if ( conn_info == NULL )
         {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"conn info is null");
            status=OF_FAILURE;
            break;
         }
      }
      else
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Error in getting  channel");
         status=OF_FAILURE;
         break;
      }


   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   return status;
}

   int32_t
of_modify_flow_entries_of_table(struct of_msg *msg,
      uint64_t  datapath_handle,
      uint8_t   table_id,
      uint16_t  flags,
      uint64_t  cookie,
      uint64_t  cookie_mask,
      void **conn_info_pp)
{
   struct ofp_flow_mod         *flow_mod;
   struct dprm_datapath_entry  *datapath;
   cntlr_conn_node_saferef_t    conn_safe_ref;
   cntlr_conn_table_t          *conn_table;
   int32_t  status = OFU_MODIFY_FLOW_ENTRY_TO_TABLE_SUCCESS;
   int32_t  retval = OF_SUCCESS;
   struct of_cntlr_conn_info *conn_info;
   struct ofp_match *match = NULL;

   cntlr_assert(msg != NULL);

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," Error in getting datapath,Err=%d "
               ,retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      flow_mod = (struct ofp_flow_mod *)(msg->desc.start_of_data);
      flow_mod->header.version = OFP_VERSION;
      flow_mod->header.type    = OFPT_FLOW_MOD;
      flow_mod->header.xid     = 0;

      flow_mod->cookie = htonll(cookie);
      flow_mod->cookie_mask = htonll(cookie_mask);
      flow_mod->table_id = table_id;
      flow_mod->flags = htons(flags);
      flow_mod->command  = OFPFC_MODIFY;
	flow_mod->out_port  = OFPP_ANY;
      flow_mod->out_group = OFPG_ANY;
      msg->desc.data_len += (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN - 4);
      
      match = &((struct ofp_flow_mod*)(msg->desc.start_of_data))->match;
      msg->desc.utils_ptr   = match;

      if(datapath->is_main_conn_present)
      {
         CNTLR_GET_MAIN_CHANNEL_INFO(datapath,conn_info);
         *conn_info_pp=conn_info;
         if ( conn_info == NULL )
         {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"conn info is null");
            status=OF_FAILURE;
            break;
         }
      }
      else
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Error in getting  channel");
         status=OF_FAILURE;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   return status;
}


   int32_t
of_create_modify_flow_entries_msg_of_table_with_strict_match(struct of_msg *msg,
      uint64_t  datapath_handle,
      uint8_t   table_id,
      uint16_t  priority,
      uint16_t  flags,
      uint64_t  cookie,
      uint64_t  cookie_mask,
      void **conn_info_pp)
{
   struct ofp_flow_mod         *flow_mod;
   struct dprm_datapath_entry  *datapath;
   cntlr_conn_node_saferef_t   conn_safe_ref;
   cntlr_conn_table_t         *conn_table;
   struct ofp_match *match = NULL;
   int32_t   status = OFU_MODIFY_FLOW_ENTRY_TO_TABLE_SUCCESS;
   int32_t   retval = OF_SUCCESS;
   struct of_cntlr_conn_info *conn_info;

   cntlr_assert(msg != NULL);

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in getting datapath,Err=%d "
               ,retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      flow_mod = (struct ofp_flow_mod *)(msg->desc.start_of_data);
      flow_mod->header.version = OFP_VERSION;
      flow_mod->header.type    = OFPT_FLOW_MOD;
      flow_mod->header.xid      = 0;

      flow_mod->cookie      = htonll(cookie);   
      flow_mod->cookie_mask = htonll(cookie_mask);
      flow_mod->table_id = table_id;
      flow_mod->priority = htons(priority);
      flow_mod->flags = htons(flags);
      flow_mod->command  = OFPFC_MODIFY_STRICT;
      flow_mod->out_port  = 0;
      flow_mod->out_group = 0;
      msg->desc.data_len += (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN - 4);
      
      match = &((struct ofp_flow_mod*)(msg->desc.start_of_data))->match;
      msg->desc.utils_ptr   = match;

      if(datapath->is_main_conn_present)
      {
         CNTLR_GET_MAIN_CHANNEL_INFO(datapath,conn_info);
         *conn_info_pp=conn_info;
         if ( conn_info == NULL )
         {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"conn info is null");
            status=OF_FAILURE;
            break;
         }
      }
      else
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Error in getting  channel");
         status=OF_FAILURE;
         break;
      }

   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   return status;
}

   int32_t
of_create_delete_flow_entries_msg_of_table( struct of_msg *msg,
      uint64_t  datapath_handle,
      uint8_t   table_id,
      uint16_t  flags,
      uint64_t  cookie,
      uint64_t  cookie_mask,
      uint32_t  out_port,
      uint32_t  out_group,
      void **conn_info_pp)
{
   struct ofp_flow_mod         *flow_mod;
   struct dprm_datapath_entry  *datapath;
   cntlr_conn_node_saferef_t   conn_safe_ref;
   cntlr_conn_table_t         *conn_table;
   int32_t   status = OFU_DELETE_FLOW_ENTRY_TO_TABLE_SUCCESS;
   int32_t   retval = OF_SUCCESS;
   struct of_cntlr_conn_info *conn_info;
   struct ofp_match *match = NULL;

   cntlr_assert(msg != NULL);

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in getting datapath,Err=%d "
               ,retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      flow_mod = (struct ofp_flow_mod *)(msg->desc.start_of_data);
      flow_mod->header.version = OFP_VERSION;
      flow_mod->header.type    = OFPT_FLOW_MOD;
      flow_mod->header.xid      = 0;

      flow_mod->cookie = htonll(cookie);
      flow_mod->cookie_mask = htonll(cookie_mask);
      flow_mod->table_id = table_id;
      flow_mod->flags = htons(flags);
      //    flow_mod->priority = htons(priority);
      flow_mod->command  = OFPFC_DELETE;
      flow_mod->out_port  = htonl(out_port);
      flow_mod->out_group = htonl(out_group);


      msg->desc.data_len += (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN - 4);
      match = &((struct ofp_flow_mod*)(msg->desc.start_of_data))->match;
      msg->desc.utils_ptr   = match;

      if(datapath->is_main_conn_present)
      {
         CNTLR_GET_MAIN_CHANNEL_INFO(datapath,conn_info);
         *conn_info_pp=conn_info;
         if ( conn_info == NULL )
         {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"conn info is null");
            status=OF_FAILURE;
            break;
         }
      }
      else
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Error in getting  channel");
         status=OF_FAILURE;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   return status;
}

   int32_t
of_create_delete_flow_entries_msg_of_table_with_strict_match( struct of_msg *msg,
      uint64_t  datapath_handle,
      uint8_t    table_id,
      uint16_t  priority,
      uint16_t  flags,
      uint64_t  cookie,
      uint64_t  cookie_mask,
      uint32_t  out_port,
      uint32_t  out_group,
      void **conn_info_pp)
{
   struct ofp_flow_mod         *flow_mod;
   struct dprm_datapath_entry  *datapath;
   cntlr_conn_node_saferef_t   conn_safe_ref;
   cntlr_conn_table_t         *conn_table;
   int32_t   status = OFU_DELETE_FLOW_ENTRY_TO_TABLE_SUCCESS;
   int32_t   retval = OF_SUCCESS;
   struct of_cntlr_conn_info *conn_info;
   struct ofp_match *match = NULL;

   cntlr_assert(msg != NULL);

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if (retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in getting datapath,Err=%d "
               ,retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      flow_mod = (struct ofp_flow_mod *)(msg->desc.start_of_data);
      flow_mod->header.version = OFP_VERSION;
      flow_mod->header.type    = OFPT_FLOW_MOD;
      flow_mod->header.xid      = 0;

      flow_mod->cookie = htonll(cookie);
      flow_mod->cookie_mask = htonll(cookie_mask);
      flow_mod->table_id = table_id;
      flow_mod->priority = htons(priority);
      flow_mod->flags = htons(flags);
      flow_mod->command  = OFPFC_DELETE_STRICT;
      flow_mod->out_port  = htonl(out_port);
      flow_mod->out_group = htonl(out_group);


      msg->desc.data_len += (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN - 4);
      match = &((struct ofp_flow_mod*)(msg->desc.start_of_data))->match;
      msg->desc.utils_ptr   = match;

      if(datapath->is_main_conn_present)
      {
         CNTLR_GET_MAIN_CHANNEL_INFO(datapath,conn_info);
         *conn_info_pp=conn_info;
         if ( conn_info == NULL )
         {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"conn info is null");
            status=OF_FAILURE;
            break;
         }
      }
      else
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Error in getting  channel");
         status=OF_FAILURE;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   return status;
}

   int32_t
of_frame_pktout_msg(struct of_msg *msg,
      uint64_t       datapath_handle,
      uint8_t        reply_pkt,
      uint32_t       buffer_id,
      uint32_t       in_port,
      uint8_t        channel_no,
      void **conn_info_pp
      )
{   
   struct dprm_datapath_entry  *datapath;
   struct ofp_packet_out       *pkt_out_msg;
   struct of_msg_descriptor    *msg_desc;
   int32_t                      retval = OF_SUCCESS;
   int32_t                      status = OF_SUCCESS;
   struct of_cntlr_conn_info *conn_info;

   cntlr_assert(msg != NULL);
   msg_desc = &msg->desc;

   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"entered");
   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error in getting datapath,Err=%d"
               ,retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      pkt_out_msg = (struct ofp_packet_out*)(msg_desc->start_of_data);
      pkt_out_msg->header.version = OFP_VERSION;
      pkt_out_msg->header.type    = OFPT_PACKET_OUT;
      pkt_out_msg->header.xid     = 0;
      pkt_out_msg->in_port   = htonl(in_port);

      msg->desc.data_len += (OFU_PACKET_OUT_MESSAGE_LEN);
      msg_desc->utils_ptr  = msg_desc->start_of_data + sizeof(struct ofp_packet_out);
      msg_desc->ptr1= msg_desc->ptr2= msg_desc->ptr3=msg->desc.utils_ptr;
      msg_desc->length1 = msg_desc->length2=msg_desc->length3=0;

      if(reply_pkt)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"REPLY");
         pkt_out_msg->buffer_id = buffer_id;
         CNTLR_GET_GIVEN_CHANNEL_INFO(datapath,channel_no,conn_info);
         if ( conn_info == NULL )
         {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"getting channel info failed for channel no");
            printf("Connection to the Datapath=%llx closed, Sending Packet out failed \r\n",datapath->dpid);
            status = CNTLR_CHN_CLOSED;
            break;

         }
         *conn_info_pp=conn_info;
      }
      else
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"non reply");
         pkt_out_msg->buffer_id = buffer_id;
         if(!datapath->is_main_conn_present)
         {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in sending message to datapath,channel closed " );
            printf("Connection to the Datapath=%llx closed, Sending Packet out failed \r\n",datapath->dpid);
            status = CNTLR_CHN_CLOSED;
            break;
         }
         //CNTLR_GET_MAIN_CHANNEL_INFO(datapath,conn_info);
         CNTLR_GET_AUX_CHANNEL_IF_EXISTS_ELSE_GET_MAIN_CHANNEL(datapath, conn_info);
         if ( conn_info == NULL )
         {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"conn info is null");
            printf("Connection to the Datapath=%llx closed, Sending Packet out failed \r\n",datapath->dpid);
            status=OF_FAILURE;
            break;
         }
         *conn_info_pp=conn_info;
      }

   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   return status;
}

   int32_t
of_add_data_to_pkt_and_send_to_dp(struct of_msg *msg,
      uint64_t       datapath_handle,
      void *conn_info_p,
      uint16_t       data_len,
      uint8_t       *data,
      void          *clbk_command_arg1,
      void          *clbk_command_arg2)
{   
   uint8_t                     *pkt_addr;
   int32_t                      retval = OF_SUCCESS;
   int32_t                      status = OF_SUCCESS;

   if ( data_len > 0 )
   {
      pkt_addr = (msg->desc.start_of_data + msg->desc.data_len );
      memcpy( pkt_addr, data, data_len); 
      msg->desc.data_len +=  data_len;
   }

   retval=of_cntrlr_send_request_message(msg, datapath_handle, conn_info_p,clbk_command_arg1, clbk_command_arg2);
   if ( retval != OF_SUCCESS)
   {         
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"of_cntrlr_send_request_message failed");
      status=OF_FAILURE;
   }

   return status;
}

   int32_t
cntlr_send_portmod_request(uint64_t datapath_handle, struct ofp_port_mod *port_info)
{
   int32_t         retval= CNTLR_SUCCESS;
   int32_t         status= OF_SUCCESS;
   struct          ofp_port_mod *port_mod;
   struct          of_msg *msg;

   CNTLR_RCU_READ_LOCK_TAKE();

   do
   {
      msg = ofu_alloc_message(OFPT_PORT_MOD,
            OFU_PORT_STATUS_MODIFACTION_MESSAGE_LEN);
      if(msg == NULL)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," OF message alloc error ");
         status = OF_FAILURE;
         break;
      }
      retval = of_set_portmod_config(msg,
            datapath_handle,
            NULL,
            NULL, port_info);
      if(retval != CNTLR_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Set config fail, error=%d "
               ,retval);
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
      return OF_FAILURE;
   }
   return OF_SUCCESS;
}

   int32_t
of_set_portmod_config(struct    of_msg *msg,
      uint64_t  datapath_handle,
      void     *clbk_command_arg1,
      void     *clbk_command_arg2, 
      struct ofp_port_mod *port_info)
{
   struct ofp_port_mod     *port_mod;
   struct dprm_datapath_entry   *datapath;
   cntlr_conn_node_saferef_t conn_safe_ref;
   cntlr_conn_table_t        *conn_table;
   uint8_t                       send_status = TRUE;
   int32_t                       retval = OF_SUCCESS;
   int32_t                       status = OF_SUCCESS;

   cntlr_assert(msg != NULL);

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      if(msg->desc.buffer_len  <  OFU_PORT_STATUS_MODIFACTION_MESSAGE_LEN)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Allocated message buffer length is not suffecient ");
         status = OF_MSG_BUF_SIZE_IS_IN_SUFFECIENT;
      }

      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Error in getting datapath,Err=%d",retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      port_mod = (struct ofp_port_mod *)(msg->desc.start_of_data);

      memcpy(port_mod->hw_addr,port_info->hw_addr,OFP_ETH_ALEN);
      port_mod->port_no        = htonl(port_info->port_no);
      port_mod->config         = htonl(port_info->config);
      port_mod->mask           = htonl(port_info->mask);
      port_mod->advertise      = htonl(port_info->advertise);
      port_mod->header.version = OFP_VERSION;
      port_mod->header.type    = OFPT_PORT_MOD;
      port_mod->header.xid     = 0;

      msg->desc.data_len += (OFU_PORT_STATUS_MODIFACTION_MESSAGE_LEN);
      //         msg->desc.buffer_len = htons(msg->desc.data_len);
      port_mod->header.length  = htons(msg->desc.data_len);

      if(!datapath->is_main_conn_present)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in sending message to datapath,channel closed "
               );
//         msg->desc.free_cbk(msg);
         status = CNTLR_CHN_CLOSED;
         break;
      }
      CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);
      if ( conn_table == NULL )
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"conn table NULL");
         status = OF_FAILURE;
         break;
      }
      CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,datapath,conn_table,conn_safe_ref,
            NULL,clbk_command_arg1,clbk_command_arg2,send_status);
      if(send_status  == FALSE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error in sending message to datapath ");
         status = CNTLR_SEND_MSG_TO_DP_ERROR;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   return status;
}

/** \ingroup Async_Config
 * \brief Set Asynchronous configuration message in the data_path
 * <b>Description </b>\n
 * API to modify asynchronous message reception configuration values.\n
 * All the necessary utility functions were expected call before actully calling this API.\n
 *
 * The Macro OFU_SET_ASYNC_CONFIG_MESSAGE_LEN returns length of message to set asynchronous\n
 * configuraion message.\n
 * 
 * \param [in] datapath_handle   - Handle of the data_path to send asynchronous configuration message. 
 * \param [in] msg               - Message descriptor with buffer allocated earlier for modification
 *                                 of async message configuration values.
 * \param [in] clbk_command_arg1 - Application private information, arg1.In case of any errors,same will be passed to the\n
 *                                 error callback function that registered by the applications to receive errors.
 * \param [in] clbk_command_arg2 - Application private information, arg2.In case of any errors,same will be passed to the\n
 *                                 error callback function that registered by the applications to receive errors.
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_SEND_ASYNC_SET_MSG_SUCCESS
 */
   int32_t
of_set_async_configuration( uint64_t  datapath_handle,
      struct of_msg *msg,
      void *clbk_command_arg1,
      void *clbk_command_arg2)
{

   struct ofp_async_config      *of_async_config_msg=NULL;
   struct dprm_datapath_entry   *datapath=NULL;
   cntlr_conn_node_saferef_t    conn_safe_ref;
   cntlr_conn_table_t           *conn_table=NULL;
   uint8_t                      send_status = TRUE;
   int32_t                      retval = OF_SUCCESS;
   int32_t                      status = OF_SUCCESS;

   cntlr_assert(msg != NULL);

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      if(msg->desc.buffer_len  <  OFU_SET_ASYNC_CONFIG_MESSAGE_LEN)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Allocated message buffer length is not suffecient ");
         status = OF_MSG_BUF_SIZE_IS_IN_SUFFECIENT;
      }

      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Error in getting datapath,Err=%d", retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      of_async_config_msg = (struct ofp_async_config *)(msg->desc.start_of_data);
      of_async_config_msg->header.version = OFP_VERSION;
      of_async_config_msg->header.type    = OFPT_SET_ASYNC;
      of_async_config_msg->header.xid     = 0;
      msg->desc.data_len += sizeof(struct ofp_async_config);
      of_async_config_msg->header.length  = htons(msg->desc.data_len);

      of_async_config_msg->packet_in_mask[0] = htonl(of_async_config_msg->packet_in_mask[0]);
      of_async_config_msg->packet_in_mask[1] = htonl(of_async_config_msg->packet_in_mask[1]);

      of_async_config_msg->port_status_mask[0] = htonl(of_async_config_msg->port_status_mask[0]);
      of_async_config_msg->port_status_mask[1] = htonl(of_async_config_msg->port_status_mask[1]);

      of_async_config_msg->flow_removed_mask[0] = htonl(of_async_config_msg->flow_removed_mask[0]);
      of_async_config_msg->flow_removed_mask[1] = htonl(of_async_config_msg->flow_removed_mask[1]);





      if(!datapath->is_main_conn_present)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in sending message to datapath,channel closed "
               );
//         msg->desc.free_cbk(msg);
         status = CNTLR_CHN_CLOSED;
         break;
      }
      CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,
            conn_safe_ref,
            conn_table);
      if ( conn_table == NULL )
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"conn table NULL");
         status = OF_FAILURE;
         break;
      }

      CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,
            datapath,
            conn_table,
            conn_safe_ref,
            NULL,
            clbk_command_arg1,
            clbk_command_arg2,
            send_status);
      if(send_status  == FALSE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error in sending message to datapath ");
         status = CNTLR_SEND_MSG_TO_DP_ERROR;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   return status;

}
/** \ingroup Async_Config
 * \brief Set flag to indicate reason for receiving packet_in msg with role as 'master' or 'equal' 
 * <b>Description </b>\n
 * Setting controller specified interested reason flags to  generates packet in message.\n
 * Here Application can tell on what reasons controller wants to receive packet_in message \n
 * when controller role as 'master' or 'equal'. \n
 *
 * Setting FALSE to all the reason flags indicates controller don't wants to receive packet_in message\n
 * when role is 'master' or 'equal'.\n
 *
 * Application should call this utility function with necessary reason flags before actually calling\n
 * to send set asynchronous configuration message API of_set_async_configuration().
 *
 * \param [in] msg             -  Message descriptor with buffer allocated earlier for async message\n
 *                                configuration values.
 * \param [in] no_match_flow   -  Boolean flag, setting this value as TRUE indicates controller with \n
 *                                'master' or 'equal' role wants to receives packet_in messages due to\n
 *                                flow entry miss reason.  
 * \param [in] action          -  Boolean flag, setting this value TRUE indicates controller with \n
 *                                'master' or 'equal' role  wants to receives packet_in messages due\n
 *                                to action that indicated to send it to controller.  
 * \param [in] invalid_ttl     -  Boolean flag, setting this value TRUE indicates controller with\n
 *                                'master' or 'equal' role wants to receives packet_in messages due to\n
 *                                invalid_ttl reason.
 *
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_SET_PACKET_IN_GEN_REASON_FLAG_SUCCESS
 */
   int32_t
ofu_set_packetin_async_msg_flags_in_master_or_equal_role(struct of_msg *msg,
      uint8_t no_match_flow,
      uint8_t action,
      uint8_t invalid_ttl)
{

   if(msg == NULL)return OF_FAILURE;

   struct ofp_async_config      *of_async_config_msg 
      = ((struct ofp_async_config *)msg->desc.start_of_data);


   of_async_config_msg->packet_in_mask[0] = 0;

   if(no_match_flow == TRUE)
   {
      of_async_config_msg->packet_in_mask[0] = 1<<OFPR_NO_MATCH;
   }

   if(action == TRUE)
   {
      of_async_config_msg->packet_in_mask[0] |= 1<<OFPR_ACTION;
   }
   if(invalid_ttl == TRUE)
   {
      of_async_config_msg->packet_in_mask[0] |= 1<<OFPR_INVALID_TTL;
   }
   return OF_SUCCESS;

}

/** \ingroup Async_Config
 * \brief Set flag to indicate reason for receiving packet_in msg with role as 'slave'
 * <b>Description </b>\n
 * Setting controller specified interested reason flags to  generates packet in message.\n
 * Here Application can tell on what reasons controller wants to receive packet_in message \n
 * when controller role as 'slave'
 *
 * Setting FALSE to all the reason flags indicates controller don't wants to receive packet_in message\n
 * when role is 'slave'\n
 *
 * Application should call this utility function with necessary reason flags before actually calling\n
 * to send set asynchronous configuration message API of_set_async_configuration().
 *
 * \param [in] msg             -  Message descriptor with buffer allocated earlier for async message\n
 *                                configuration values.
 * \param [in] no_match_flow   -  Boolean flag, setting this value as TRUE indicates controller with \n
 *                                'slave' role wants to receives packet_in messages due to\n
 *                                flow entry miss reason.  
 * \param [in] action          -  Boolean flag, setting this value TRUE indicates controller with \n
 *                                'slave' role  wants to receives packet_in messages due\n
 *                                to action that indicated to send it to controller.  
 * \param [in] invalid_ttl     -  Boolean flag, setting this value TRUE indicates controller with\n
 *                                'slave' role wants receives packet_in messages due to invalid_ttl reason.
 *
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_SET_PACKET_IN_GEN_REASON_FLAG_SUCCESS
 */
   int32_t
ofu_set_packetin_async_msg_flags_in_slave_role(struct of_msg *msg,
      uint8_t no_match_flow,
      uint8_t action,
      uint8_t invalid_ttl)
{
   struct ofp_async_config      *of_async_config_msg 
      = ((struct ofp_async_config *)msg->desc.start_of_data);

   if(no_match_flow == TRUE)
   {
      of_async_config_msg->packet_in_mask[1] = 1<<OFPR_NO_MATCH;
   }

   if(action == TRUE)
   {
      of_async_config_msg->packet_in_mask[1] |= 1<<OFPR_ACTION;
   }
   if(invalid_ttl == TRUE)
   {
      of_async_config_msg->packet_in_mask[1] |= 1<<OFPR_INVALID_TTL;
   }
   return OF_SUCCESS;

}

/** \ingroup Async_Config
 * \brief Set flag to indicate reason for receiving port status with role as 'master' or 'equal'
 * <b>Description </b>\n
 * Set controller interested reason flags that  generates  port status change message.
 * Application can tell on what reasons controller wants to receive port status change message\n
 * when its role is 'master' or 'equal'. 
 * Setting FALSE to all the flags indicates controller don't wants to receive port status \n
 * change message when role is 'master' or 'equal'. 
 *
 * Port status message is way to indicate change in status of data_path ports. The reason\n
 * for generation port status change message is due to 'add' or 'modify' or 'deletion of port.   
 *
 * Application should call this utility function with necessary configuration flags before actually\n
 * calling to send set asynchronous configuration message API of_set_async_configuration().\n
 *
 * \param [in] msg          -  Message descriptor with buffer allocated earlier for async message\n
 *                             configuration values.
 * \param [in] add_port     -  Boolean flag, setting this value TRUE indicates controller with\n
 *                             'master' or 'equal' role wants to receives port  change message \n
 *                             due to addition of new port  
 * \param [in] modify_port  -  Boolean flag, setting this value TRUE indicates controller with\n
 *                             'master' or 'equal' role wants to receives port  change message\n
 *                             due to modification of port  
 * \param [in] delete_port  -  Boolean flag, setting this value TRUE indicates controller with\n
 *                             'master' or 'equal' role wants to receives port  change message \n
 *                             due to deletion of port  
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_SET_PORT_STATUS_GEN_REASON_FLAG_SUCCESS
 */
   int32_t
ofu_set_port_mod_async_msg_flags_in_master_or_equal_role(struct of_msg *msg,
      uint8_t add_port,
      uint8_t modify_port,
      uint8_t delete_port)
{
   struct ofp_async_config      *of_async_config_msg 
      = ((struct ofp_async_config *)msg->desc.start_of_data);

   if(add_port == TRUE)
   {
      of_async_config_msg->port_status_mask[0] = 1<<OFPPR_ADD;
   }

   if(modify_port == TRUE)
   {
      of_async_config_msg->port_status_mask[0] |= 1<<OFPPR_MODIFY;
   }
   if(delete_port == TRUE)
   {
      of_async_config_msg->port_status_mask[0] |= 1<<OFPPR_DELETE;
   }
   return OF_SUCCESS;

}

/** \ingroup Async_Config
 * \brief Set flag to indicate reason for receiving port status with role as 'slave'
 * <b>Description </b>\n
 * Set controller interested reason flags that  generates  port status change message.
 * Application can tell on what reasons controller wants to receive port status change message\n
 * when its role is 'slave'. 
 * Setting FALSE to all the flags indicates controller don't wants to receive port status \n
 * change message when role is 'slave'. 
 *
 * Port status message is way to indicate change in status of data_path ports. The reason\n
 * for generation port status change message is due to 'add' or 'modify' or 'deletion of port.   
 *
 * Application should call this utility function with necessary configuration flags before actually\n
 * calling send set asynchronous configuration message API of_set_async_configuration().\n
 *
 * \param [in] msg          -  Message descriptor with buffer allocated earlier for async message\n
 *                             configuration values.
 * \param [in] add_port     -  Boolean flag, setting this value TRUE indicates controller with\n
 *                             'slave' role wants to receives port  change message \n
 *                             due to addition of new port  
 * \param [in] modify_port  -  Boolean flag, setting this value TRUE indicates controller with\n
 *                             'slave' role wants to receives port  change message\n
 *                             due to modification of port  
 * \param [in] delete_port  -  Boolean flag, setting this value TRUE indicates controller with\n
 *                             'slave' role wants to receives port  change message \n
 *                             due to deletion of port  
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_SET_PORT_STATUS_GEN_REASON_FLAG_SUCCESS
 */
   int32_t
ofu_set_port_mod_async_msg_flags_in_slave_role(struct of_msg *msg,
      uint8_t add_port,
      uint8_t modify_port,
      uint8_t delete_port)
{

   struct ofp_async_config      *of_async_config_msg 
      = ((struct ofp_async_config *)msg->desc.start_of_data);

   if(add_port == TRUE)
   {
      of_async_config_msg->port_status_mask[1] = 1<<OFPPR_ADD;
   }

   if(modify_port == TRUE)
   {
      of_async_config_msg->port_status_mask[1] |= 1<<OFPPR_MODIFY;
   }
   if(delete_port == TRUE)
   {
      of_async_config_msg->port_status_mask[1] |= 1<<OFPPR_DELETE;
   }
   return OF_SUCCESS;

}

/** \ingroup Async_Config
 * \brief Set flag to indicate reason for receiving flow removed event with role as 'master' or 'equal'
 * <b>Description </b>\n
 *  Set controller interested reason flags that  generates flow remove event. 
 *  Application can tell on what reasons controller wants to receive flow removed event when the role\n
 *  as 'master' or 'equal'. 
 *
 *  Setting FALSE to all the flags indicates controller don't wants to receive flow removed event when \n
 *  role is 'master' or 'equal'. 
 *
 *  Application should call this utility function with necessary configuration flags before actually\n
 *  calling to send set asynchronous configuration message API of_set_async_configuration().\n
 *  \param [in] msg            -  Message descriptor with buffer allocated earlier for async message\n
 *                                configuration values.
 *  \param [in] idle_timeout   -  Boolean flag, TRUE indicates controller with 'master' or 'equal' role\n
 *                                receives flow removed event due to timeout of flow idle time.  \n
 *  \param [in] hard_timeout   -  Boolean flag, TRUE indicates controller with 'master' or 'equal'role\n
 *                                receives flow removed event due to exceed of flow hard time.  \n
 *  \param [in] delete_command -  Boolean flag, TRUE indicates controller with 'master' or 'equal'role\n
 *                                receives flow removed event due to command received to delete\n
 *                                flow entry.  \n
 *  \param [in] group_delete   - Boolean flag, TRUE indicates controller with 'master' or 'equal' role\n
 *                               receives flow removed event due to deletion of group that\n
 *                               attached in the group actions of flow. \n
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_SET_FLOW_REMOVE_GEN_REASON_FLAG_SUCCESS
 */
   int32_t
ofu_set_flow_remove_async_msg_flags_in_master_or_equal_role(struct of_msg *msg,
      uint8_t idle_timeout,
      uint8_t hard_timeout,
      uint8_t delete_command,
      uint8_t group_delete)
{
   struct ofp_async_config      *of_async_config_msg 
      = ((struct ofp_async_config *)msg->desc.start_of_data);

   if(idle_timeout == TRUE)
   {
      of_async_config_msg->flow_removed_mask[0] = 1<<OFPRR_IDLE_TIMEOUT;
   }

   if(hard_timeout == TRUE)
   {
      of_async_config_msg->flow_removed_mask[0] |= 1<<OFPRR_HARD_TIMEOUT;
   }
   if(delete_command == TRUE)
   {
      of_async_config_msg->flow_removed_mask[0] |= 1<<OFPRR_DELETE;
   }
   if(group_delete == TRUE)
   {
      of_async_config_msg->flow_removed_mask[0] |= 1<<OFPRR_GROUP_DELETE;
   }
   return OF_SUCCESS;
}
/** \ingroup Async_Config
 * \brief Set flag to indicate reason for receiving flow removed event with role as 'slave'
 * <b>Description </b>\n
 *  Set controller interested reason flags that  generates flow remove event. \n
 *  Application can tell on what reasons controller wants to receive flow removed event when the role\n
 *  as 'slave'. 
 *
 *  Setting FALSE to all the flags indicates controller don't wants to receive flow removed event when \n
 *  role is 'slave'. 
 *
 *  Application should call this utility function with necessary configuration flags before actually\n
 *  calling send set asynchronous configuration message API of_set_async_configuration().\n
 *  \param [in] msg            -  Message descriptor with buffer allocated earlier for async message\n
 *                                configuration values.
 *  \param [in] idle_timeout   -  Boolean flag, TRUE indicates controller with 'slave' role\n
 *                                receives flow removed event due to timeout of flow idle time.  \n
 *  \param [in] hard_timeout   -  Boolean flag, TRUE indicates controller with 'slave' role\n
 *                                receives flow removed event due to exceed of flow hard time.  \n
 *  \param [in] delete_command -  Boolean flag, TRUE indicates controller with 'slave' role\n
 *                                receives flow removed event due to command received to delete\n
 *                                flow entry.  \n
 *  \param [in] group_delete   - Boolean flag, TRUE indicates controller with 'slave' role\n
 *                               receives flow removed event due to deletion of group that\n
 *                               attached in the group actions of flow. \n
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_SET_FLOW_REMOVE_GEN_REASON_FLAG_SUCCESS
 */
   int32_t
ofu_set_flow_remove_async_msg_flags_in_slave_role(struct of_msg *msg,
      uint8_t idle_timeout,
      uint8_t hard_timeout,
      uint8_t delete_command,
      uint8_t group_delete)
{
   struct ofp_async_config      *of_async_config_msg 
      = ((struct ofp_async_config *)msg->desc.start_of_data);

   if(idle_timeout == TRUE)
   {
      of_async_config_msg->flow_removed_mask[1] = 1<<OFPRR_IDLE_TIMEOUT;
   }

   if(hard_timeout == TRUE)
   {
      of_async_config_msg->flow_removed_mask[1] |= 1<<OFPRR_HARD_TIMEOUT;
   }
   if(delete_command == TRUE)
   {
      of_async_config_msg->flow_removed_mask[1] |= 1<<OFPRR_DELETE;
   }
   if(group_delete == TRUE)
   {
      of_async_config_msg->flow_removed_mask[1] |= 1<<OFPRR_GROUP_DELETE;
   }
   return OF_SUCCESS;

}
/** \ingroup Async_Config
 * \brief Get current Asynchronous configuration values that set at data_path.
 * <b>Description </b>\n
 * API to get current asynchronous configuration values. Callback function is passed to\n
 * get current configuration values. The asynchronous configuration values that \n
 * available as part of message descriptor need to pass different utility functions to\n
 * know whether specific configuration values set or not. \n
 *
 * The Macro OFU_GET_ASYNC_CONFIG_MESSAGE_LEN returns length of the message required to get\n
 * asynchronous configuration message.
 * \param [in]  msg                - Message descriptor with buffer allocated earlier to send get current \n
 *                                   async configuration request message. Application need to allocate\n
 *                                   message descriptor with Length to get asynchronous configuration message\n
 *                                   defined above.\n
 * \param [in] datapath_handle     - Handle of the data_path to which we are sending request. \n
 * \param [in] async_config_info_cbk_fn - Callback function to  pass asynchronous configuration values.
 * \param [in] clbk_arg1           - Application private information, arg1.In case of any errors,same will be passed to the\n
 *                                   error callback function that registered by the applications to receive errors.\n
 * \param [in] clbk_arg2           - Application private information, arg2.In case of any errors,same will be passed to the\n
 *                                   error callback function that registered by the applications to receive errors.\n
 */
   int32_t
of_get_async_config_info(struct of_msg *msg,
      uint64_t   datapath_handle,
      of_async_config_cbk_fn async_config_info_cbk_fn,
      void *clbk_arg1,
      void *clbk_arg2)
{
   struct ofp_header      *of_async_config_msg=NULL;
   struct dprm_datapath_entry   *datapath=NULL;
   cntlr_conn_node_saferef_t    conn_safe_ref;
   cntlr_conn_table_t           *conn_table=NULL;
   uint8_t                      send_status = TRUE;
   int32_t                      retval = OF_SUCCESS;
   int32_t                      status = OF_SUCCESS;

   cntlr_assert(msg != NULL);

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      if(msg->desc.buffer_len  <  OFU_GET_ASYNC_CONFIG_MESSAGE_LEN)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Allocated message buffer length is not suffecient ");
         status = OF_MSG_BUF_SIZE_IS_IN_SUFFECIENT;
      }

      retval = get_datapath_byhandle(datapath_handle, &datapath);
      if(retval  != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Error in getting datapath,Err=%d", retval);
         status = DPRM_INVALID_DATAPATH_HANDLE;
         break;
      }

      of_async_config_msg = (struct ofp_async_config *)(msg->desc.start_of_data);
      of_async_config_msg->version = OFP_VERSION;
      of_async_config_msg->type    = OFPT_GET_ASYNC_REQUEST;
      of_async_config_msg->xid     = 0;
      msg->desc.data_len += sizeof(struct ofp_header);
      of_async_config_msg->length  = htons(msg->desc.data_len);

      if(!datapath->is_main_conn_present)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in sending message to datapath,channel closed ");
//         msg->desc.free_cbk(msg);
         status = CNTLR_CHN_CLOSED;
         break;
      }
      CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);
      if ( conn_table == NULL )
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"conn table NULL");
         status = OF_FAILURE;
         break;
      }
      CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,datapath,conn_table,conn_safe_ref,
            (void *)async_config_info_cbk_fn,clbk_arg1,clbk_arg2,send_status);
      if(send_status  == FALSE)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error in sending message to datapath ");
         status = CNTLR_SEND_MSG_TO_DP_ERROR;
         break;
      }
   }
   while(0);
   CNTLR_RCU_READ_LOCK_RELEASE();

   return status;


}
