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


extern void cntrl_ucm_send_multipart_response(uint32_t multipart_type,void *data_p, uint8_t last);
int32_t of_update_switch_info(uint64_t datapath_handle,  struct ofi_switch_info *switch_info);
int32_t cntlr_send_group_desc_request(uint64_t datapath_handle);

int32_t cntlr_send_group_statistics_request(uint64_t datapath_handle,uint32_t  group_id);

int32_t cntlr_send_group_features_request(uint64_t datapath_handle);

int32_t cntl_send_ipopts_cfg_get_request(uint64_t datapath_handle);

void of_group_desc_reply_cbk_fn (struct of_msg *msg,
    uint64_t  controller_handle,
    uint64_t  domain_handle,
    uint64_t  datapath_handle,
    void     *cbk_arg1,
    void     *cbk_arg2,
    uint8_t   status,
    struct ofi_group_desc_info *group_desc,
    uint32_t more_groups);

void of_group_stats_reply_cbk_fn( struct of_msg *msg,
    uint64_t  controller_handle,
    uint64_t  domain_handle,
    uint64_t  datapath_handle,
    uint32_t  group_id,
    void     *cbk_arg1,
    void     *cbk_arg2,
    uint8_t   status,
    struct ofi_group_stat *group_stats,
    uint8_t   more_groups);

void of_group_features_reply_cbk_fn( struct of_msg *msg,
    uint64_t  controller_handle,
    uint64_t  domain_handle,
    uint64_t  datapath_handle,
    void     *cbk_arg1,
    void     *cbk_arg2,
    uint8_t   status,
    struct ofi_group_features_info *group_features);

#if 0
void of_switch_info_reply_cbk_fn(struct of_msg *msg, uint64_t domain_handle,
    struct ofi_switch_info *switch_info_p);
#endif

void of_switch_info_reply_cbk_fn_after_feature_reply(struct of_msg *msg, uint64_t domain_handle,
    uint16_t status, struct ofi_switch_info *switch_info_p);

void of_ipopts_config_response_cbk_fn(uint64_t  controller_handle,
    uint64_t  domain_handle,
    uint64_t  datapath_handle,
    struct of_msg *msg,
    void     *cbk_arg1,
    void     *cbk_arg2,
    uint8_t  status,
    struct ofl_switch_config *switch_config);

int32_t
of_get_meter_config(struct of_msg       *msg,
    uint64_t              datapath_handle,
    of_meter_config_cbk_fn      meter_config_cbk,
    void                  *cbk_arg1,
    void                  *cbk_arg2,
    uint32_t              *xid);
int32_t
of_get_meter_features( struct of_msg             *msg,
    uint64_t                  datapath_handle,
    of_meter_features_cbk_fn  meter_features_cbk,
    void                      *cbk_arg1,
    void                      *cbk_arg2,
    uint32_t                  *xid);
int32_t
of_get_meter_statistics(struct of_msg         *msg,
    uint64_t              datapath_handle,
    uint32_t              meter_id,
    of_meter_stats_cbk_fn meter_stats_cbk,
    void                  *cbk_arg1,
    void                  *cbk_arg2);


void of_meter_config_reply_cbk_fn (struct of_msg *msg,
    uint64_t  controller_handle,
    uint64_t  domain_handle,
    uint64_t  datapath_handle,
    void     *cbk_arg1,
    void     *cbk_arg2,
    uint8_t   status,
    struct ofi_meter_rec_info *meter_record_p,
    uint32_t more_meters);

void of_meter_features_reply_cbk_fn( struct of_msg *msg, 
    uint64_t  controller_handle,
    uint64_t  domain_handle,
    uint64_t  datapath_handle,
    void     *cbk_arg1,
    void     *cbk_arg2,
    uint8_t   status,
    struct ofi_meter_features_info *meter_features_p);

void of_meter_stats_reply_cbk_fn( struct of_msg *msg,
    uint64_t  controller_handle,
    uint64_t  domain_handle,
    uint64_t  datapath_handle,
    uint32_t  meter_id,
    void     *cbk_arg1,
    void     *cbk_arg2,
    uint8_t   status,
    struct ofi_meter_stats_info *meter_stats,
    uint8_t   more_meters);


int32_t cntl_send_ipopts_cfg_get_request(uint64_t datapath_handle)
{
  struct of_msg  *msg;
  int32_t         retval= CNTLR_SUCCESS;
  int32_t         status= OF_SUCCESS;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"entered");
  CNTLR_RCU_READ_LOCK_TAKE();
  do      
  {       
    msg = ofu_alloc_message(OFPT_GET_CONFIG_REQUEST, OFU_SWITCH_CONFIG_GET_REQ_MESSAGE_LEN);
    if(msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"OF message alloc error ");
      status = OF_FAILURE;
      break;  
    }               

    retval =  of_get_switch_config_msg(msg,
        datapath_handle,
        of_ipopts_config_response_cbk_fn,
        NULL,
        NULL);

    if(retval != CNTLR_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Send Get Port Description message fail, error=%d",retval);
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

void of_ipopts_config_response_cbk_fn(uint64_t  controller_handle,
    uint64_t  domain_handle,
    uint64_t  datapath_handle,
    struct of_msg *msg,
    void     *cbk_arg1,
    void     *cbk_arg2,
    uint8_t  status,
    struct ofl_switch_config *switch_config)
{

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Entered ...calling ucm handlers");
#ifdef CNTRL_OFCLI_SUPPORT
  cntrl_ucm_send_multipart_response(OFPMP_IPOPTS_DESC, switch_config, TRUE);
#endif
  if(msg != NULL)
    msg->desc.free_cbk(msg);
}

int32_t cntlr_send_group_desc_request(uint64_t datapath_handle)
{
  struct of_msg  *msg;
  int32_t         retval= CNTLR_SUCCESS;
  int32_t         status= OF_SUCCESS;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"entered");
  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    msg = ofu_alloc_message(OFPT_MULTIPART_REQUEST,
        OFU_GROUP_DESC_REQ_MESSAGE_LEN);
    if(msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"OF message alloc error ");
      status = OF_FAILURE;
      break;
    }

    retval =  of_get_groups_description(msg,
        datapath_handle,
        of_group_desc_reply_cbk_fn,
        NULL,
        NULL,
        NULL);
    if(retval != CNTLR_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Send Get Port Description message fail, error=%d",retval);
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

void of_group_desc_reply_cbk_fn (struct of_msg *msg,
    uint64_t  controller_handle,
    uint64_t  domain_handle,
    uint64_t  datapath_handle,
    void     *cbk_arg1,
    void     *cbk_arg2,
    uint8_t   status,
    struct ofi_group_desc_info *group_desc_p,
    uint32_t more_groups)
{
  struct ofi_action *action_p;
  struct ofi_bucket *bucket_entry_p;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Entered ...calling ucm handlers");
#ifdef CNTRL_OFCLI_SUPPORT
  cntrl_ucm_send_multipart_response(OFPMP_GROUP_DESC, group_desc_p, more_groups);
#endif
  if(msg != NULL)
    msg->desc.free_cbk(msg);

  if (group_desc_p)
  {
    OF_LIST_REMOVE_HEAD_AND_SCAN(group_desc_p->bucket_list, bucket_entry_p,struct ofi_bucket *,OF_BUCKET_LISTNODE_OFFSET)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"deleting bucket");
      OF_LIST_REMOVE_HEAD_AND_SCAN(bucket_entry_p->action_list,action_p,struct ofi_action *,OF_ACTION_LISTNODE_OFFSET);
    }
  }
}

int32_t cntlr_send_group_statistics_request(uint64_t datapath_handle,uint32_t  group_id)
{
  struct of_msg  *msg;
  int32_t         retval= CNTLR_SUCCESS;
  int32_t         status= OF_SUCCESS;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"entered ");
  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    msg = ofu_alloc_message(OFPT_MULTIPART_REQUEST,
        OFU_GROUP_STATS_REQ_MESSAGE_LEN);
    if(msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"OF message alloc error ");
      status = OF_FAILURE;
      break;
    }

    retval =  of_get_group_statistics(msg,
        datapath_handle,
        group_id,
        of_group_stats_reply_cbk_fn,
        NULL,
        NULL);
    if(retval != CNTLR_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Send Get Port Description message fail, error=%d",retval);
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

void of_group_stats_reply_cbk_fn( struct of_msg *msg,
    uint64_t  controller_handle,
    uint64_t  domain_handle,
    uint64_t  datapath_handle,
    uint32_t  group_id,
    void     *cbk_arg1,
    void     *cbk_arg2,
    uint8_t   status,
    struct ofi_group_stat *group_stats,
    uint8_t   more_groups)
{
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Entered ...calling ucm handlers");
#ifdef CNTRL_OFCLI_SUPPORT
  cntrl_ucm_send_multipart_response(OFPMP_GROUP, group_stats, more_groups);
#endif
  if(msg != NULL)
    msg->desc.free_cbk(msg);
}

int32_t cntlr_send_group_features_request(uint64_t datapath_handle)
{
  struct of_msg  *msg;
  int32_t         retval= CNTLR_SUCCESS;
  int32_t         status= OF_SUCCESS;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"entered ");
  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    msg = ofu_alloc_message(OFPT_MULTIPART_REQUEST,
        OFU_GROUP_FEATURES_REQ_MESSAGE_LEN);
    if(msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"OF message alloc error ");
      status = OF_FAILURE;
      break;
    }

    retval =  of_get_groups_features(msg,
        datapath_handle,
        of_group_features_reply_cbk_fn,
        NULL,
        NULL,
        NULL);
    if(retval != CNTLR_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Send Get Port Description message fail, error=%d",retval);
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

void of_group_features_reply_cbk_fn( struct of_msg *msg, 
    uint64_t  controller_handle,
    uint64_t  domain_handle,
    uint64_t  datapath_handle,
    void     *cbk_arg1,
    void     *cbk_arg2,
    uint8_t   status,
    struct ofi_group_features_info *group_features_p)
{
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Entered ...calling ucm handlers");
#ifdef CNTRL_OFCLI_SUPPORT
  cntrl_ucm_send_multipart_response(OFPMP_GROUP_FEATURES, group_features_p, TRUE);
#endif
  if(msg != NULL)
    msg->desc.free_cbk(msg);
}

#if 0
int32_t cntlr_send_swinfo_desc_request(uint64_t datapath_handle)
{
  struct of_msg  *msg;
  int32_t         retval= CNTLR_SUCCESS;
  int32_t         status= OF_SUCCESS;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"entered");
  CNTLR_RCU_READ_LOCK_TAKE();
  do  
  {
    msg = ofu_alloc_message(OFPT_MULTIPART_REQUEST, OFU_SWINFO_REQ_MESSAGE_LEN);
    if(msg == NULL)
    {   
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"OF message alloc error ");
      status = OF_FAILURE;
      break;
    }   

    retval =  of_get_switch_info_through_channel(msg,
        datapath_handle,
        of_switch_info_reply_cbk_fn,
        NULL,
        NULL,
        NULL);
    if(retval != CNTLR_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Send Get switch info message fail, error=%d",retval);
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
void of_switch_info_reply_cbk_fn(struct of_msg *msg,
                                 uint64_t  domain_handle,
                                 struct ofi_switch_info *switch_info_p)
{
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Entered ...calling ucm handlers");
#ifdef CNTRL_OFCLI_SUPPORT
  cntrl_ucm_send_multipart_response(OFPMP_DESC, switch_info_p, TRUE);
#endif
  if(msg != NULL)
    msg->desc.free_cbk(msg);

}
#endif


int32_t cntlr_send_swinfo_desc_request_after_feature_reply(uint64_t datapath_handle)
{
  struct of_msg  *msg;
  int32_t         retval= CNTLR_SUCCESS;
  int32_t         status= OF_SUCCESS;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"entered");
  CNTLR_RCU_READ_LOCK_TAKE();
  do  
  {
    msg = ofu_alloc_message(OFPT_MULTIPART_REQUEST, OFU_SWINFO_REQ_MESSAGE_LEN);
    if(msg == NULL)
    {   
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"OF message alloc error ");
      status = OF_FAILURE;
      break;
    }   

    retval =  of_get_switch_info_through_channel(msg,
        datapath_handle,
        of_switch_info_reply_cbk_fn_after_feature_reply,
        NULL,
        NULL,
        NULL);
    if(retval != CNTLR_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Send Get switch info message fail, error=%d",retval);
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

void of_switch_info_reply_cbk_fn_after_feature_reply(struct of_msg *msg,
                                                     uint64_t  datapath_handle,
                                                     uint16_t status,
                                                     struct ofi_switch_info *switch_info)
{
  if(status == OF_RESPONSE_STATUS_SUCCESS)
  {
    if(of_update_switch_info(datapath_handle, switch_info) != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Update switch info to DP failed \r\n");
    }
  }
  if(msg != NULL)
    msg->desc.free_cbk(msg);

  return;
}

int32_t cntlr_send_meter_config_request(uint64_t datapath_handle)
{
  struct of_msg  *msg;
  int32_t         retval= CNTLR_SUCCESS;
  int32_t         status= OF_SUCCESS;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"entered");
  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    msg = ofu_alloc_message(OFPT_MULTIPART_REQUEST,
                  OFU_METER_CONFIG_REQ_MESSAGE_LEN);
    if(msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"OF message alloc error ");
      status = OF_FAILURE;
      break;
    }

    retval =  of_get_meter_config(msg,
        datapath_handle,
        of_meter_config_reply_cbk_fn,
        NULL,
        NULL,
        NULL);
    if(retval != CNTLR_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Send Get Meter Config message fail, error=%d",retval);
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

void of_meter_config_reply_cbk_fn (struct of_msg *msg,
    uint64_t  controller_handle,
    uint64_t  domain_handle,
    uint64_t  datapath_handle,
    void     *cbk_arg1,
    void     *cbk_arg2,
    uint8_t   status,
    struct ofi_meter_rec_info *meter_record_p,
    uint32_t more_meters)
{
  struct ofi_meter_band *band_entry_p;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Entered ...calling ucm handlers");
#ifdef CNTRL_OFCLI_SUPPORT
  cntrl_ucm_send_multipart_response(OFPMP_METER_CONFIG, meter_record_p, more_meters);
#endif
  if(msg != NULL)
    msg->desc.free_cbk(msg);

  if (meter_record_p)
  {
    OF_LIST_REMOVE_HEAD_AND_SCAN(meter_record_p->meter_band_list, band_entry_p,struct ofi_meter_band *,OF_METER_BAND_LISTNODE_OFFSET)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"deleting band");
    }
  }
}

int32_t cntlr_send_meter_features_request(uint64_t datapath_handle)
{
  struct of_msg  *msg;
  int32_t         retval= CNTLR_SUCCESS;
  int32_t         status= OF_SUCCESS;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"entered ");
  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    msg = ofu_alloc_message(OFPT_MULTIPART_REQUEST,
        OFU_METER_FEATURE_REQ_MESSAGE_LEN);
    if(msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"OF message alloc error ");
      status = OF_FAILURE;
      break;
    }

    retval =  of_get_meter_features(msg,
        datapath_handle,
        of_meter_features_reply_cbk_fn,
        NULL,
        NULL,
        NULL);
    if(retval != CNTLR_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Send Get Meter Features message fail, error=%d",retval);
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

void of_meter_features_reply_cbk_fn( struct of_msg *msg, 
    uint64_t  controller_handle,
    uint64_t  domain_handle,
    uint64_t  datapath_handle,
    void     *cbk_arg1,
    void     *cbk_arg2,
    uint8_t   status,
    struct ofi_meter_features_info *meter_features_p)
{
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Entered ...calling ucm handlers");
#ifdef CNTRL_OFCLI_SUPPORT
  cntrl_ucm_send_multipart_response(OFPMP_METER_FEATURES, meter_features_p, TRUE);
#endif
  if(msg != NULL)
    msg->desc.free_cbk(msg);
}

int32_t cntlr_send_meter_statistics_request(uint64_t datapath_handle,uint32_t  meter_id)
{
  struct of_msg  *msg;
  int32_t         retval= CNTLR_SUCCESS;
  int32_t         status= OF_SUCCESS;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"entered ");
  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    msg = ofu_alloc_message(OFPT_MULTIPART_REQUEST,
        OFU_METER_STATS_REQ_MESSAGE_LEN);
    if(msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"OF message alloc error ");
      status = OF_FAILURE;
      break;
    }

    retval =  of_get_meter_statistics(msg,
        datapath_handle,
        meter_id,
        of_meter_stats_reply_cbk_fn,
        NULL,
        NULL);
    if(retval != CNTLR_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Send Get meter stats message fail, error=%d",retval);
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

void of_meter_stats_reply_cbk_fn( struct of_msg *msg,
    uint64_t  controller_handle,
    uint64_t  domain_handle,
    uint64_t  datapath_handle,
    uint32_t  meter_id,
    void     *cbk_arg1,
    void     *cbk_arg2,
    uint8_t   status,
    struct ofi_meter_stats_info *meter_stats,
    uint8_t   more_meters)
{
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Entered ...calling ucm handlers");
#ifdef CNTRL_OFCLI_SUPPORT
  cntrl_ucm_send_multipart_response(OFPMP_METER, meter_stats, more_meters);
#endif
  if(msg != NULL)
    msg->desc.free_cbk(msg);
}

