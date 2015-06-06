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
#include "oflog.h"
#include "of_meter.h"


int32_t of_register_meter(uint64_t datapath_handle, struct ofi_meter_rec_info *meter_entry_p)
{
  struct   dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;
  uchar8_t offset;
  offset = OF_METER_REC_LISTNODE_OFFSET;

  if(meter_entry_p == NULL)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"meter_entry_p is NULL");
    return OF_FAILURE;
  }

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle( datapath_handle,&datapath_entry_p);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"datapath entry not found");
      status=OF_FAILURE;
      break;
    }
    OF_LIST_INIT(meter_entry_p->meter_band_list, NULL);
    status=OF_SUCCESS;
    break;
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  if ( status == OF_SUCCESS)
  {	
    OF_APPEND_NODE_TO_LIST(datapath_entry_p->meter_table,meter_entry_p,offset);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"succesfully added meter %d", meter_entry_p->meter_id); 	
  }	

  return status;
}

int32_t of_update_meter(uint64_t datapath_handle, struct ofi_meter_rec_info *meter_info_p)
{
  struct ofi_meter_rec_info *meter_entry_p;
  struct   dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;
  uchar8_t offset;
  offset = OF_METER_REC_LISTNODE_OFFSET;

  if (meter_info_p == NULL)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"band_entry_p is NULL");
    return OF_FAILURE;
  }


  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle( datapath_handle,&datapath_entry_p);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"datapath entry not found");
      status=OF_FAILURE;
      break;
    }
    OF_LIST_SCAN(datapath_entry_p->meter_table, meter_entry_p, struct ofi_meter_rec_info *,offset)
    {
      if( meter_entry_p->meter_id == meter_info_p->meter_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"meter found updateing meter values");
        meter_entry_p->flags=meter_info_p->flags;
        status=OF_SUCCESS;
        break;

      }
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  return status;
}

int32_t of_get_meter(uint64_t datapath_handle,  uint32_t meter_id,  struct ofi_meter_rec_info **meter_entry_pp)
{
  struct ofi_meter_rec_info *meter_entry_p;
  struct   dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;
  uchar8_t offset;
  offset = OF_METER_REC_LISTNODE_OFFSET;


  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"entered");

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle( datapath_handle,&datapath_entry_p);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"datapath entry not found");
      status=OF_FAILURE;
      break;
    }
    OF_LIST_SCAN(datapath_entry_p->meter_table, meter_entry_p, struct ofi_meter_rec_info *,offset)
    {
      if( meter_entry_p->meter_id == meter_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"meter found ");
        status=OF_SUCCESS;
        break;

      }
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  if (status == OF_SUCCESS )
  {
    *meter_entry_pp=meter_entry_p;
  }
  return status;
}

int32_t of_register_band_to_meter(uint64_t datapath_handle, struct ofi_meter_band *band_entry_p, uint32_t meter_id)
{
  struct ofi_meter_rec_info *meter_entry_p;
  struct   dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;
  uchar8_t offset;
  offset = OF_METER_REC_LISTNODE_OFFSET;

  if (band_entry_p == NULL)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"band_entry_p is NULL");
    return OF_FAILURE;
  }


  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle( datapath_handle,&datapath_entry_p);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"datapath entry not found");
      status=OF_FAILURE;
      break;
    }
    OF_LIST_SCAN(datapath_entry_p->meter_table, meter_entry_p, struct ofi_meter_rec_info *,offset)
    {
      if( meter_entry_p->meter_id == meter_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"meter found adding band to it");
        status=OF_SUCCESS;
        break;
      }
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  if (status == OF_SUCCESS )
  {
    OF_APPEND_NODE_TO_LIST(meter_entry_p->meter_band_list, band_entry_p,OF_METER_BAND_LISTNODE_OFFSET);
  }
  return status;
}

int32_t of_update_band_in_meter(uint64_t datapath_handle, struct ofi_meter_band *band_info_p, uint32_t meter_id)
{
  struct ofi_meter_band *band_entry_p;
  struct ofi_meter_rec_info *meter_entry_p=NULL;
  struct dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;
  uchar8_t offset;
  offset = OF_METER_REC_LISTNODE_OFFSET;


  if(band_info_p == NULL)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"band_entry_p is NULL");
    return OF_FAILURE;
  }

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle(datapath_handle,&datapath_entry_p);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"datapath entry not found");
      break;
    }

    OF_LIST_SCAN(datapath_entry_p->meter_table,meter_entry_p, struct ofi_meter_rec_info *,offset)
    {
      if( meter_entry_p->meter_id == meter_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"meter found");

        OF_LIST_SCAN(meter_entry_p->meter_band_list, band_entry_p, struct ofi_meter_band *,OF_METER_BAND_LISTNODE_OFFSET)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"band  found");
          if( band_entry_p->band_id == band_info_p->band_id)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"updating band");
            band_entry_p->type=band_info_p->type;
            band_entry_p->rate=band_info_p->rate;
            band_entry_p->burst_size=band_info_p->burst_size;
            band_entry_p->prec_level=band_info_p->prec_level; 
            status=OF_SUCCESS;
            break;
          }
        }

      }

      if ( status == OF_SUCCESS)
      {
        break;
      }
    }

  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();


  return status;
}

int32_t of_unregister_meter(uint64_t datapath_handle, uint32_t meter_id)
{
  struct ofi_meter_rec_info *meter_entry_p, *prev_meter_entry_p=NULL;
  struct ofi_meter_band *band_entry_p;
  struct dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;
  uchar8_t offset;
  offset = OF_METER_REC_LISTNODE_OFFSET;

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle( datapath_handle,&datapath_entry_p);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"datapath entry not found");
      break;
    }

    OF_LIST_SCAN(datapath_entry_p->meter_table,meter_entry_p, struct ofi_meter_rec_info *,offset)
    {
      if( meter_entry_p->meter_id == meter_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"meter found");
        OF_LIST_REMOVE_HEAD_AND_SCAN(meter_entry_p->meter_band_list, band_entry_p,struct ofi_meter_band *,OF_METER_BAND_LISTNODE_OFFSET)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"deleting band");
        }
        status=OF_SUCCESS;
        break;
      }
      prev_meter_entry_p=meter_entry_p;
    }

  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  if ( status == OF_SUCCESS)
  {
    OF_REMOVE_NODE_FROM_LIST(datapath_entry_p->meter_table,meter_entry_p, prev_meter_entry_p,offset);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"succesfully deleted meter");

  }
  return status;
}

int32_t of_meter_unregister_band(uint64_t datapath_handle, uint32_t meter_id, uint32_t band_id)
{

  struct ofi_meter_rec_info *meter_entry_p=NULL;
  struct ofi_meter_band *band_entry_p, *prev_band_entry_p=NULL;
  struct dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;
  uchar8_t offset;
  offset = OF_METER_REC_LISTNODE_OFFSET;

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle( datapath_handle,&datapath_entry_p);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"datapath entry not found");
      break;
    }

    OF_LIST_SCAN(datapath_entry_p->meter_table,meter_entry_p, struct ofi_meter_rec_info *,offset)
    {
      if( meter_entry_p->meter_id == meter_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"meter found");
        OF_LIST_SCAN(meter_entry_p->meter_band_list, band_entry_p,struct ofi_meter_band *,OF_METER_BAND_LISTNODE_OFFSET)
        {
          if (band_entry_p->band_id == band_id)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"deleting band %d", band_id);
            status=OF_SUCCESS;
            break;
          }
          prev_band_entry_p=band_entry_p;
        }
      }
      if ( status == OF_SUCCESS)
        break;
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  if ( status == OF_SUCCESS)
  {
    OF_REMOVE_NODE_FROM_LIST(meter_entry_p->meter_band_list,band_entry_p, prev_band_entry_p, OF_METER_BAND_LISTNODE_OFFSET);
  }
  return status;
}

int32_t of_meter_unregister_bands(uint64_t datapath_handle, uint32_t meter_id)
{

  struct ofi_meter_rec_info *meter_entry_p =NULL;
  struct ofi_meter_band *band_entry_p;
  struct dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;
  uchar8_t offset;
  offset = OF_METER_REC_LISTNODE_OFFSET;

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle( datapath_handle,&datapath_entry_p);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"datapath entry not found");
      break;
    }

    OF_LIST_SCAN(datapath_entry_p->meter_table,meter_entry_p, struct ofi_meter_rec_info *,offset)
    {
      if( meter_entry_p->meter_id == meter_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"meter found");
        OF_LIST_REMOVE_HEAD_AND_SCAN(meter_entry_p->meter_band_list, band_entry_p,struct ofi_meter_band *, OF_METER_BAND_LISTNODE_OFFSET)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"deleting band");
        }
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"succesfully deleted all bands");
        status=OF_SUCCESS;
        CNTLR_RCU_READ_LOCK_RELEASE();
        return status;
      }
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  return status;

}

int32_t of_meter_frame_message_request(struct of_msg *msg, struct ofi_meter_rec_info *meter_entry_p)
{
  struct ofi_meter_band *band_entry_p;
  int32_t retval;
  uint8_t starting_location;
  uint16_t length_of_band;
  int32_t status;


  if ( msg == NULL )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"invalid message");
    return OF_FAILURE;

  }
  if ( meter_entry_p == NULL )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"meter entry is invalid");
    return OF_FAILURE;

  }

  do
  { 
    ofu_start_appending_bands_to_meter(msg);
    OF_LIST_SCAN(meter_entry_p->meter_band_list, band_entry_p, struct ofi_meter_band *,OF_METER_BAND_LISTNODE_OFFSET)
    {
      switch(band_entry_p->type)
      {
        case OFPMBT_DROP: /* Drop packet. */ 
	  if(band_entry_p->rate == 0){
		  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"meter is drop type");
		  status = OF_METER_RATE_REQUIRED;
		  break;
	  }
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Band Type OFPMBT_DROP:");
          retval=ofu_append_band_to_meter_type_drop(msg, band_entry_p->rate,band_entry_p->burst_size);
          break;

        case OFPMBT_DSCP_REMARK: /* Remark DSCP in the IP header. */
	  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"meter is dsc_remark type");
	  if(band_entry_p->rate == 0 || band_entry_p->prec_level == 0){
		  status = OF_METER_PREC_LEVEL_REQUIRED;
		  break;
	    }
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Band Type OFPMBT_DSCP_REMARK:");
          retval=ofu_append_band_to_meter_type_dscp_remark(msg, band_entry_p->rate, band_entry_p->prec_level, band_entry_p->burst_size);
          break;

        case OFPMBT_EXPERIMENTER: /* Experimenter meter band. */
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"not supported");
          break;
        default:
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Invalid bandType");
          break;
      }
    }

    ofu_end_appending_bands_to_meter(msg,
        &starting_location,
        &length_of_band);
  }while(0);
  return status;
}

int32_t of_meter_frame_and_send_message_request(uint64_t datapath_handle, uint32_t meter_id, uint8_t command)
{
  struct of_msg *msg;
  uint16_t  msg_len=300;
  struct ofi_meter_rec_info *meter_entry_p = NULL;
  int32_t retval;
  int32_t status=OF_SUCCESS;
  struct dprm_datapath_entry *datapath_entry_p=NULL;
  void *conn_info_p;
  uchar8_t offset;
  offset = OF_METER_REC_LISTNODE_OFFSET;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"entered");
  msg = ofu_alloc_message(OFPT_METER_MOD, msg_len);
  if (msg == NULL)   
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"OF message alloc error");
    status = OF_FAILURE;
    return status;
  }

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle( datapath_handle,&datapath_entry_p);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"datapath entry not found");
      status=OF_FAILURE;
      break;
    }

    OF_LIST_SCAN(datapath_entry_p->meter_table,meter_entry_p, struct ofi_meter_rec_info *,offset)
    {
      if( meter_entry_p->meter_id == meter_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"meter found");
        break;
      }
    }

    if (meter_entry_p == NULL )
    {

      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"meter found");
      status=OF_FAILURE;
      break;
    }

  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  switch(command)
  {
    case ADD_METER:
      retval=of_add_meter(msg, datapath_handle,meter_entry_p->meter_id,meter_entry_p->flags, &conn_info_p);
      if ( retval != OFU_ADD_METER_ENTRY_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"add_meter failed");
        status=OF_FAILURE;
      }
      break;
    case MOD_METER:
      retval=of_modify_meter(msg, datapath_handle,meter_entry_p->meter_id,meter_entry_p->flags,&conn_info_p);
      if ( retval != OFU_MODIFY_METER_ENTRY_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"mod_meter failed");
        status=OF_FAILURE;
      }
      break;
    case DEL_METER:
      retval=of_delete_meter(msg, datapath_handle,meter_entry_p->meter_id,&conn_info_p);
      if ( retval != OFU_DELETE_METER_ENTRY_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Delete meter failed");
        status=OF_FAILURE;
      }
      break;
    default:
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"invalid command");
      status=OF_FAILURE;
      break;
  }

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    switch(command)
    {
      case ADD_METER:
        retval= of_meter_frame_message_request(msg, meter_entry_p);
        if ( retval != OFU_APPEND_BAND_SUCCESS)
 	{
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"framing message failed");
          status=retval;
          break;
        }
        break;
      case MOD_METER:
        retval= of_meter_frame_message_request(msg, meter_entry_p);
        if ( retval != OFU_APPEND_BAND_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"framing message failed");
          status=retval;
          break;
        }
        break;
      case DEL_METER:
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Delete meter ");
        break;
      default:
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"invalid command");
        status=OF_FAILURE;
        break;
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  if ( status != OF_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"status failed");
    return status;

  }

  switch(command)
  {
    case ADD_METER:
    case MOD_METER:
    case DEL_METER:
    {
      retval=of_cntrlr_send_request_message(msg, datapath_handle, conn_info_p,NULL, NULL);
      if ( retval != OF_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"add_meter failed");
        status=OF_FAILURE;
      }
      break;
    }
#if 0
    case MOD_GROUP:
      retval=of_cntrlr_send_request_message(msg, datapath_handle,meter_entry_p->meter_id,meter_entry_p->flags,NULL, NULL);
      if ( retval != OFU_MODIFY_GROUP_ENTRY_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"mod_meter failed");
        status=OF_FAILURE;
      }
      break;
    case DEL_GROUP:
      retval=of_cntrlr_send_request_message(msg, datapath_handle,meter_entry_p->meter_id,NULL, NULL);
      if ( retval != OFU_DELETE_GROUP_ENTRY_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Delete meter failed");
        status=OF_FAILURE;
      }
      break;
#endif
    default:
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"invalid command");
      status=OF_FAILURE;
      break;
  }
  return status;
}

int32_t of_meter_frame_band_response(char *msg, struct ofi_meter_band **band_entry_pp, uint32_t *msg_length)
{
  struct ofi_meter_band *band_entry_p;
  struct ofp_meter_band_header *band_read_p;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Entered");

  band_read_p=(struct ofp_meter_band_header *)msg;
  band_entry_p=(struct ofi_meter_band *)calloc(1,sizeof(struct ofi_meter_band ));
  if ( band_entry_p == NULL )
  {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"MEMORY allocation failed");
      return OF_FAILURE;
  }

  band_entry_p->type = ntohs(band_read_p->type);
  switch(band_entry_p->type)
  {
    case  OFPMBT_DROP:
    {
      struct ofp_meter_band_drop *band_type_drop;
      band_type_drop = (struct ofp_meter_band_drop *)msg;
      band_entry_p->len = ntohs(band_type_drop->len);
      band_entry_p->rate = ntohl(band_type_drop->rate);
      band_entry_p->burst_size = ntohl(band_type_drop->burst_size);
      break;
    }

    case OFPMBT_DSCP_REMARK:
    {
      struct ofp_meter_band_dscp_remark *band_type_dscp;
      band_type_dscp = (struct ofp_meter_band_dscp_remark *)msg;
      band_entry_p->len = ntohs(band_type_dscp->len);
      band_entry_p->rate = ntohl(band_type_dscp->rate);
      band_entry_p->burst_size = ntohl(band_type_dscp->burst_size);
      band_entry_p->prec_level = band_type_dscp->prec_level;
      break;
    }

    case OFPMBT_EXPERIMENTER:
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Not supported");
      break;
    }

    default:
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Unknownband Type");
      break;
    }
  }

  msg += sizeof(struct ofp_meter_band_drop);
  *msg_length += sizeof(struct ofp_meter_band_drop);   

  *band_entry_pp=band_entry_p;
  return OF_SUCCESS;
}

int32_t of_meter_frame_response(char *msg, struct ofi_meter_rec_info *meter_entry_p, uint32_t msg_length)
{
  struct ofi_meter_band *band_entry_p;
  uint32_t band_length;
  int32_t return_value;
  uchar8_t offset;
  offset = OF_METER_BAND_LISTNODE_OFFSET;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"frame response msg_length %d", msg_length);

  OF_LIST_INIT(meter_entry_p->meter_band_list, NULL);

  while (msg_length > 0 )
  {
    band_length=0;
    return_value= of_meter_frame_band_response(msg, &band_entry_p, &band_length);
    if ( return_value != OF_SUCCESS )
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"of_meter_frame_band_response failed");
      return OF_FAILURE;
    }
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"band_length %d", band_length);
    OF_APPEND_NODE_TO_LIST(meter_entry_p->meter_band_list, band_entry_p,offset);
    msg += band_length;
    msg_length -= band_length;
  }
  return OF_SUCCESS;
}
