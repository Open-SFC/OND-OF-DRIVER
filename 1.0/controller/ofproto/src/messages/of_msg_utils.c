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

/*!\file of_utilities.c
* Author     : Rajesh Madabushi <rajesh.madabushi@freescale.com>
* Date       : 
* Description: 
* This file contains defintions different utility functions 
* that used to set match fileds, actions, instructions ,etc.
 */
#include "controller.h"
#include "of_utilities.h"

#define OF_OXM_HDR_LEN   (4)

#define OF_SET_FIELD_ACTION_PADDING_LEN(oxm_len) (((oxm_len) + 7)/8*8 - (oxm_len))

/*########################### Match field routines #################################*/
  void
ofu_start_setting_match_field_values(struct of_msg *msg)
{
  struct ofp_match *match = NULL;
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;
  match = msg_desc->utils_ptr;
  match->type       = htons(OFPMT_OXM);
  match->length     = 4; 
  msg->desc.ptr1    = (uint8_t*)&match->oxm_fields;
  msg->desc.length1 = 0; 
  msg_desc->utils_length = match->length;
}

  inline int32_t
of_set_in_port(struct    of_msg *msg,
    uint32_t *in_port_number)
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if ((msg_desc->ptr1+msg_desc->length1 + OFU_IN_PORT_FIELD_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficiant...\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)                 = htonl(OXM_OF_IN_PORT);
  *(uint32_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htonl(*in_port_number); 

  msg_desc->length1 += OFU_IN_PORT_FIELD_LEN;
  return OFU_SET_FIELD_SUCCESS;
}

  inline int32_t
of_set_in_physical_port(struct of_msg *msg,
    uint32_t      *in_phy_port_no)
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if ((msg_desc->ptr1+msg_desc->length1 + OFU_IN_PHY_PORT_FIELD_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficiant...\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)                 = htonl(OXM_OF_IN_PHY_PORT);
  *(uint32_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htonl(*in_phy_port_no); 

  msg_desc->length1 += OFU_IN_PHY_PORT_FIELD_LEN;
  return OFU_SET_FIELD_SUCCESS;
}

  inline int32_t
of_set_protocol(struct of_msg *msg,
    uint8_t *ip_protocol)
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_IP_PROTO_FIELD_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)                 = htonl(OXM_OF_IP_PROTO);
  *(uint8_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = *ip_protocol; 

  msg_desc->length1 += OFU_IP_PROTO_FIELD_LEN;
  return OFU_SET_FIELD_SUCCESS;
}

int32_t of_set_ip_dscp_bits(struct of_msg *msg,  uint8_t *ip_dscp)
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_IP_DSCP_FIELD_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)                 = htonl(OXM_OF_IP_DSCP);
  *(uint8_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = *ip_dscp; 

  msg_desc->length1 += OFU_IP_DSCP_FIELD_LEN;
  return OFU_SET_FIELD_SUCCESS;
}

  int32_t
of_set_ip_ecn_bits(struct of_msg *msg,
    uint8_t *ip_ecn)
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_IP_ECN_FIELD_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)                 = htonl(OXM_OF_IP_ECN);
  *(uint8_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = *ip_ecn; 

  msg_desc->length1 += OFU_IP_ECN_FIELD_LEN;
  return OFU_SET_FIELD_SUCCESS;
}

  int32_t
of_set_ether_type(struct of_msg *msg,
    uint16_t      *ether_type)
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_ETH_TYPE_FIELD_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  *(uint32_t*)(msg_desc->ptr1+msg_desc->length1) = htonl(OXM_OF_ETH_TYPE);
  *(uint16_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htons(*ether_type); 

  msg_desc->length1 += OFU_ETH_TYPE_FIELD_LEN;
  return OFU_SET_FIELD_SUCCESS;
}

  int32_t
of_set_arp_op_code(struct of_msg *msg,
    uint16_t  *arp_op)
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_ARP_OP_FIELD_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  *(uint32_t*)(msg_desc->ptr1+msg_desc->length1) = htonl(OXM_OF_ARP_OP);
  *(uint16_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htons(*arp_op); 

  msg_desc->length1 += OFU_ARP_OP_FIELD_LEN;

  return OFU_SET_FIELD_SUCCESS;
}

  int32_t
of_set_arp_source_ip_addr(struct of_msg *msg,
    ipv4_addr_t   *ipv4_addr,
    uint8_t        is_mask_set,
    ipv4_addr_t   *ipv4_addr_mask)
{
  struct of_msg_descriptor *msg_desc = &msg->desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if(is_mask_set)
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_ARP_SPA_MASK_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)     = htonl(OXM_OF_ARP_SPA_W);
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htonl(*ipv4_addr); 
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1 + 8) = htonl(*ipv4_addr_mask); 
    msg_desc->length1 += OFU_ARP_SPA_MASK_LEN;
  }
  else
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_ARP_SPA_FIELD_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)  = htonl(OXM_OF_ARP_SPA);
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htonl(*ipv4_addr); 
    msg_desc->length1 += OFU_ARP_SPA_FIELD_LEN;
  }
  return OFU_SET_FIELD_SUCCESS;
}

  int32_t
of_set_arp_target_ip_addr(struct of_msg      *msg,
    ipv4_addr_t   *ipv4_addr,
    uint8_t        is_mask_set,
    ipv4_addr_t   *ipv4_addr_mask)
{
  struct of_msg_descriptor *msg_desc = &msg->desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if(is_mask_set)
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_ARP_TPA_MASK_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)     = htonl(OXM_OF_ARP_TPA_W);
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htonl(*ipv4_addr); 
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1 + 8) = htonl(*ipv4_addr_mask); 
    msg_desc->length1 += OFU_ARP_TPA_MASK_LEN;
  }
  else
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_ARP_TPA_FIELD_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)  = htonl(OXM_OF_ARP_TPA);
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htonl(*ipv4_addr); 
    msg_desc->length1 += OFU_ARP_TPA_FIELD_LEN;
  }
  return OFU_SET_FIELD_SUCCESS;
}
  int32_t
of_set_arp_source_hw_addr(struct of_msg *msg,
    uint8_t       hw_addr[6],
    uint8_t       is_mask_set,
    uint8_t       hw_addr_mask[6])
{
  struct of_msg_descriptor *msg_desc = &msg->desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;
  if(is_mask_set)
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_ARP_SHA_MASK_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len) ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)     = htonl(OXM_OF_ARP_SHA_W);
    memcpy ((msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN), hw_addr, 6);
    memcpy ((msg_desc->ptr1+msg_desc->length1 + 10), hw_addr_mask, 6); 
    msg_desc->length1 += OFU_ARP_SHA_MASK_LEN;
  }
  else
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_ARP_SHA_FIELD_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len) ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)  = htonl(OXM_OF_ARP_SHA);
    memcpy ((msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN), hw_addr, 6);
    msg_desc->length1 += OFU_ARP_SHA_FIELD_LEN;
  }

  return OFU_SET_FIELD_SUCCESS;
}

  int32_t
of_set_arp_target_hw_addr(struct of_msg *msg,
    uint8_t        hw_addr[6],
    uint8_t        is_mask_set,
    uint8_t        hw_addr_mask[6])
{
  struct of_msg_descriptor *msg_desc = &msg->desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if(is_mask_set)
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_ARP_DHA_MASK_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len) ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)     = htonl(OXM_OF_ARP_THA_W);
    memcpy ((msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN), hw_addr, 6);
    memcpy ((msg_desc->ptr1+msg_desc->length1 + 10), hw_addr_mask, 6); 
    msg_desc->length1 += OFU_ARP_DHA_MASK_LEN;
  }
  else
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_ARP_DHA_FIELD_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len) ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)  = htonl(OXM_OF_ARP_THA);
    memcpy ((msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN), hw_addr, 6);
    msg_desc->length1 += OFU_ARP_DHA_FIELD_LEN;
  }

  return OFU_SET_FIELD_SUCCESS;

}

int32_t of_set_mpls_label(struct of_msg *msg, uint32_t *mpls_label_val)
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_MPLS_LABEL_FIELD_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)                 = htonl(OXM_OF_MPLS_LABEL);
  *(uint32_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htonl(*mpls_label_val); 

  msg_desc->length1 += OFU_MPLS_LABEL_FIELD_LEN;

  return OFU_SET_FIELD_SUCCESS;
}

int32_t of_set_mpls_tc(struct of_msg *msg, uint8_t *mpls_tc_val)
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_MPLS_TC_FIELD_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)                 = htonl(OXM_OF_MPLS_TC);
  *(uint8_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = *mpls_tc_val; 

  msg_desc->length1 += OFU_MPLS_TC_FIELD_LEN;

  return OFU_SET_FIELD_SUCCESS;
}

int32_t of_set_mpls_bos(struct of_msg *msg, uint8_t *mpls_bos_val)
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_MPLS_BOS_FIELD_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)                 = htonl(OXM_OF_MPLS_BOS);
  *(uint8_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = *mpls_bos_val; 

  msg_desc->length1 += OFU_MPLS_BOS_FIELD_LEN;

  return OFU_SET_FIELD_SUCCESS;
}
int32_t of_set_pbb_i_sid(struct of_msg *msg,
    uint8_t      *pbb_i_sid,
    uint8_t       is_mask_set,
    uint8_t      *pbb_i_sid_mask)
{
  struct of_msg_descriptor *msg_desc = &msg->desc;
  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if(is_mask_set)
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_PBB_ISID_MASK_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)     = htonl(OXM_OF_PBB_ISID_W);
    *(uint8_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = *pbb_i_sid; 
    *(uint8_t*)(msg_desc->ptr1+msg_desc->length1 + 7) = *pbb_i_sid_mask; 
    msg_desc->length1 += OFU_PBB_ISID_MASK_LEN;
  }
  else
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_PBB_ISID_FIELD_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)  = htonl(OXM_OF_PBB_ISID);
    *(uint8_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = *pbb_i_sid; 
    msg_desc->length1 += OFU_PBB_ISID_FIELD_LEN;
  }
  return OFU_SET_FIELD_SUCCESS;
}
int32_t of_set_logical_port_meta_data(struct of_msg *msg,
    uint64_t      *tunnel_id,
    uint8_t       is_mask_set,
    uint64_t      *tunnel_id_mask)
{
  struct of_msg_descriptor *msg_desc = &msg->desc;
  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if(is_mask_set)
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_TUNNEL_ID_MASK_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)     = htonl(OXM_OF_TUNNEL_ID_W);
    *(uint64_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htonll(*tunnel_id);
    *(uint64_t*)(msg_desc->ptr1+msg_desc->length1 + 12) = htonll(*tunnel_id_mask); 
    msg_desc->length1 += OFU_TUNNEL_ID_MASK_LEN;
  }
  else
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_TUNNEL_ID_FIELD_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)  = htonl(OXM_OF_TUNNEL_ID);
    *(uint64_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htonll(*tunnel_id); 
    msg_desc->length1 += OFU_TUNNEL_ID_FIELD_LEN;
  }
  return OFU_SET_FIELD_SUCCESS;
}

  int32_t
of_set_vlan_id(struct of_msg *msg,
    uint16_t   *vlan_id,
    uint8_t    is_mask_set,
    uint16_t   *vlan_id_mask)
{
  struct of_msg_descriptor *msg_desc = &msg->desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if(is_mask_set)
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_VLAN_VID_MASK_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)     = htonl(OXM_OF_VLAN_VID_W);
    *(uint16_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htons(*vlan_id); 
    *(uint16_t*)(msg_desc->ptr1+msg_desc->length1 + 6) = htons(*vlan_id_mask); 
    msg_desc->length1 += OFU_VLAN_VID_MASK_LEN;
  }
  else
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_VLAN_VID_FIELD_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)  = htonl(OXM_OF_VLAN_VID);
    *(uint16_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htons(*vlan_id); 
    msg_desc->length1 += OFU_VLAN_VID_FIELD_LEN;
  }
  return OFU_SET_FIELD_SUCCESS;
}

  int32_t
of_set_vlan_priority_bits(struct of_msg *msg,
    uint8_t *vlan_pcp)
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_VLAN_PCP_FIELD_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)                 = htonl(OXM_OF_VLAN_PCP);
  *(uint8_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = *vlan_pcp; 

  msg_desc->length1 += OFU_VLAN_PCP_FIELD_LEN;
  return OFU_SET_FIELD_SUCCESS;
}

  int32_t
of_set_ipv4_source(struct of_msg *msg,
    ipv4_addr_t   *ipv4_addr,
    uint8_t        is_mask_set,
    ipv4_addr_t   *ipv4_addr_mask)
{
  struct of_msg_descriptor *msg_desc = &msg->desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if(is_mask_set)
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_IPV4_SRC_MASK_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)     = htonl(OXM_OF_IPV4_SRC_W);
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htonl(*ipv4_addr); 
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1 + 8) = htonl(*ipv4_addr_mask); 
    msg_desc->length1 += OFU_IPV4_SRC_MASK_LEN;
  }
  else
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_IPV4_SRC_FIELD_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)  = htonl(OXM_OF_IPV4_SRC);
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htonl(*ipv4_addr); 
    msg_desc->length1 += OFU_IPV4_SRC_FIELD_LEN;
  }
  return OFU_SET_FIELD_SUCCESS;
}

int32_t of_set_ipv6_source(struct of_msg *msg,
                           ipv6_addr_t   *ipv6_addr,
                           uint8_t        is_mask_set,
                           ipv6_addr_t   *ipv6_addr_mask)
{
  struct of_msg_descriptor *msg_desc = &msg->desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if(is_mask_set)
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_IPV6_SRC_MASK_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)     = htonl(OXM_OF_IPV6_SRC_W);
    memcpy((ipv6_addr_t *)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) , &(ipv6_addr->ipv6Addr32), sizeof(ipv6_addr->ipv6Addr32)); 
    memcpy((ipv6_addr_t *)(msg_desc->ptr1+msg_desc->length1 + OFU_IPV6_SRC_FIELD_LEN) ,&(ipv6_addr_mask->ipv6Addr32), sizeof(ipv6_addr_mask->ipv6Addr32)); 
    msg_desc->length1 += OFU_IPV6_SRC_MASK_LEN;
  }
  else
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_IPV6_SRC_FIELD_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)  = htonl(OXM_OF_IPV6_SRC);
    memcpy((ipv6_addr_t *)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) , &(ipv6_addr->ipv6Addr32), sizeof(ipv6_addr->ipv6Addr32)); 
    msg_desc->length1 += OFU_IPV6_SRC_FIELD_LEN;
  }
  return OFU_SET_FIELD_SUCCESS;
}



  int32_t
of_set_ipv4_destination(struct of_msg *msg,
    ipv4_addr_t   *ipv4_addr,
    uint8_t        is_mask_set,
    ipv4_addr_t   *ipv4_addr_mask)
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if(is_mask_set)
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_IPV4_DST_MASK_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)     = htonl(OXM_OF_IPV4_DST_W);
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htonl(*ipv4_addr); 
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1 + 8) = htonl(*ipv4_addr_mask); 
    msg_desc->length1 += OFU_IPV4_DST_MASK_LEN;
  }
  else
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_IPV4_DST_FIELD_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)  = htonl(OXM_OF_IPV4_DST);
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htonl(*ipv4_addr); 
    msg_desc->length1 += OFU_IPV4_DST_FIELD_LEN;
  }
  return OFU_SET_FIELD_SUCCESS;
}
  int32_t
of_set_ipv6_destination(struct of_msg *msg,
    ipv6_addr_t   *ipv6_addr,
    uint8_t        is_mask_set,
    ipv6_addr_t   *ipv6_addr_mask)
{
  struct of_msg_descriptor *msg_desc = &msg->desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if(is_mask_set)
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_IPV6_DST_MASK_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)     = htonl(OXM_OF_IPV6_DST_W);
    memcpy((ipv6_addr_t *)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) , &(ipv6_addr->ipv6Addr32), sizeof(ipv6_addr->ipv6Addr32)); 
    memcpy((ipv6_addr_t *)(msg_desc->ptr1+msg_desc->length1 + OFU_IPV6_DST_FIELD_LEN) ,&(ipv6_addr_mask->ipv6Addr32), sizeof(ipv6_addr_mask->ipv6Addr32)); 
    msg_desc->length1 += OFU_IPV6_DST_MASK_LEN;
  }
  else
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_IPV6_DST_FIELD_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)  = htonl(OXM_OF_IPV6_DST);
    memcpy((ipv6_addr_t *)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) , &(ipv6_addr->ipv6Addr32), sizeof(ipv6_addr->ipv6Addr32)); 
    msg_desc->length1 += OFU_IPV6_DST_FIELD_LEN;
  }
  return OFU_SET_FIELD_SUCCESS;
}
  int32_t
of_set_ipv6_flow_label(struct of_msg *msg,
    uint32_t      *ipv6_flow_label,
    uint8_t       is_mask_set,
    uint32_t      *v6_flow_label_mask)
{
  struct of_msg_descriptor *msg_desc = &msg->desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if(is_mask_set)
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_IPV6_FLABEL_MASK_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)     = htonl(OXM_OF_IPV6_FLABEL_W);
    *(uint32_t *)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) =htonl(*ipv6_flow_label); 
    *(uint32_t *)(msg_desc->ptr1+msg_desc->length1 + OFU_IPV6_FLABEL_FIELD_LEN) =htonl(*v6_flow_label_mask); 
    msg_desc->length1 += OFU_IPV6_FLABEL_MASK_LEN;
  }
  else
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_IPV6_FLABEL_FIELD_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)  = htonl(OXM_OF_IPV6_FLABEL);
    *(uint32_t *)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) =htonl(*ipv6_flow_label); 
    msg_desc->length1 += OFU_IPV6_FLABEL_FIELD_LEN;
  }
  return OFU_SET_FIELD_SUCCESS;

}
  int32_t
of_set_icmpv6_type(struct of_msg *msg,
    uint8_t *icmpv6_type )
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_ICMPV6_TYPE_FIELD_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  *(uint32_t*)(msg_desc->ptr1+msg_desc->length1) = htonl(OXM_OF_ICMPV6_TYPE);
  *(uint8_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = *icmpv6_type;

  msg_desc->length1 += OFU_ICMPV6_TYPE_FIELD_LEN;

  return OFU_SET_FIELD_SUCCESS;

}

  int32_t
of_set_icmpv6_code(struct of_msg *msg,
    uint8_t *icmpv6_code )
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_ICMPV6_CODE_FIELD_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  *(uint32_t*)(msg_desc->ptr1+msg_desc->length1) = htonl(OXM_OF_ICMPV6_CODE);
  *(uint8_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = *icmpv6_code;

  msg_desc->length1 += OFU_ICMPV6_CODE_FIELD_LEN;

  return OFU_SET_FIELD_SUCCESS;

}
  int32_t
of_set_ipv6_nd_target(struct of_msg *msg,
    ipv6_addr_t   *nd_ipv6_target_addr )
{
  struct of_msg_descriptor *msg_desc = &msg->desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_IPV6_ND_TARGET_FIELD_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }
  *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)  = htonl(OXM_OF_IPV6_ND_TARGET);
  memcpy((ipv6_addr_t *)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) , &(nd_ipv6_target_addr->ipv6Addr32), sizeof(nd_ipv6_target_addr->ipv6Addr32)); 
  msg_desc->length1 += OFU_IPV6_ND_TARGET_FIELD_LEN;

  return OFU_SET_FIELD_SUCCESS;

}
  int32_t
of_set_ipv6_nd_source_hw_addr(struct of_msg *msg,
    uint8_t *hw_addr)
{
  struct of_msg_descriptor *msg_desc = &msg->desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_IPV6_ND_SLL_FIELD_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len) )
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)  = htonl(OXM_OF_IPV6_ND_SLL);
    memcpy ((msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN), hw_addr, 6);
    msg_desc->length1 += OFU_IPV6_ND_SLL_FIELD_LEN;
  }
  return OFU_SET_FIELD_SUCCESS;

}

  int32_t
of_set_ipv6_nd_target_hw_addr(struct of_msg *msg,
    uint8_t *hw_addr)
{
  struct of_msg_descriptor *msg_desc = &msg->desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_IPV6_ND_DLL_FIELD_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len) )
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)  = htonl(OXM_OF_IPV6_ND_TLL);
    memcpy ((msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN), hw_addr, 6);
    msg_desc->length1 += OFU_IPV6_ND_DLL_FIELD_LEN;
  }
  return OFU_SET_FIELD_SUCCESS;


}

  int32_t
of_set_source_mac(struct of_msg *msg,
    uint8_t  *src_mac_addr,
    uint8_t   is_mask_set,
    uint8_t  *src_mac_mask)
{
  struct of_msg_descriptor *msg_desc = &msg->desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if(is_mask_set)
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_ETH_SRC_MAC_ADDR_MASK_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len) ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)     = htonl(OXM_OF_ETH_SRC_W);
    memcpy ((msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN), src_mac_addr, 6);
    memcpy ((msg_desc->ptr1+msg_desc->length1 + 10), src_mac_mask, 6); 
    msg_desc->length1 += OFU_ETH_SRC_MAC_ADDR_MASK_LEN;
  }
  else
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_ETH_SRC_MAC_ADDR_FIELD_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len) ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)  = htonl(OXM_OF_ETH_SRC);
    memcpy ((msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN), src_mac_addr, 6);
    msg_desc->length1 += OFU_ETH_SRC_MAC_ADDR_FIELD_LEN;
  }
  return OFU_SET_FIELD_SUCCESS;
}

  int32_t
of_set_destination_mac(struct of_msg *msg,
    uint8_t  *dst_mac_addr,
    uint8_t  is_mask_set,
    uint8_t  *dst_mac_mask)
{
  struct of_msg_descriptor *msg_desc = &msg->desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if(is_mask_set)
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_ETH_DST_MAC_ADDR_MASK_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len) ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)     = htonl(OXM_OF_ETH_DST_W);
    memcpy ((msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN), dst_mac_addr, 6);
    memcpy ((msg_desc->ptr1+msg_desc->length1 + 10), dst_mac_mask, 6); 
    msg_desc->length1 += OFU_ETH_DST_MAC_ADDR_MASK_LEN;
  }
  else
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_ETH_DST_MAC_ADDR_FIELD_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len) ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }
    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)  = htonl(OXM_OF_ETH_DST);
    memcpy ((msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN), dst_mac_addr, 6);
    msg_desc->length1 += OFU_ETH_DST_MAC_ADDR_FIELD_LEN;
  }
  return OFU_SET_FIELD_SUCCESS;
}

  int32_t
of_set_tcp_source_port(struct of_msg *msg,
    uint16_t *port_number )
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_TCP_SRC_FIELD_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)                 = htonl(OXM_OF_TCP_SRC);
  *(uint16_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htons(*port_number); 

  msg_desc->length1 += OFU_TCP_SRC_FIELD_LEN;

  return OFU_SET_FIELD_SUCCESS;
}

  int32_t
of_set_tcp_destination_port(struct of_msg *msg,
    uint16_t *port_number )
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_TCP_DST_FIELD_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)                 = htonl(OXM_OF_TCP_DST);
  *(uint16_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htons(*port_number); 

  msg_desc->length1 += OFU_TCP_DST_FIELD_LEN;

  return OFU_SET_FIELD_SUCCESS;
}

  int32_t
of_set_sctp_source_port(struct of_msg *msg,
    uint16_t *port_number )
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_SCTP_SRC_FIELD_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)                 = htonl(OXM_OF_SCTP_SRC);
  *(uint16_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htons(*port_number); 

  msg_desc->length1 += OFU_SCTP_SRC_FIELD_LEN;

  return OFU_SET_FIELD_SUCCESS;
}

  int32_t
of_set_sctp_destination_port(struct of_msg *msg,
    uint16_t *port_number )
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_SCTP_DST_FIELD_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)                 = htonl(OXM_OF_SCTP_DST);
  *(uint16_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htons(*port_number); 

  msg_desc->length1 += OFU_SCTP_DST_FIELD_LEN;

  return OFU_SET_FIELD_SUCCESS;
}

  int32_t
of_set_udp_source_port(struct of_msg *msg,
    uint16_t *port_number )
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_UDP_SRC_FIELD_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)                  = htonl(OXM_OF_UDP_SRC);
  *(uint16_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htons(*port_number); 

  msg_desc->length1 += OFU_UDP_SRC_FIELD_LEN;

  return OFU_SET_FIELD_SUCCESS;
}

  int32_t
of_set_udp_destination_port(struct of_msg *msg,
    uint16_t *port_number )
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_UDP_DST_FIELD_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)                  = htonl(OXM_OF_UDP_DST);
  *(uint16_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htons(*port_number); 

  msg_desc->length1 += OFU_UDP_DST_FIELD_LEN;

  return OFU_SET_FIELD_SUCCESS;
}


  int32_t
of_set_icmpv4_type(struct of_msg *msg,
    uint8_t *icmpv4_type )
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_ICMPV4_TYPE_FIELD_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  *(uint32_t*)(msg_desc->ptr1+msg_desc->length1) = htonl(OXM_OF_ICMPV4_TYPE);
  *(uint8_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = *icmpv4_type; 

  msg_desc->length1 += OFU_ICMPV4_TYPE_FIELD_LEN;

  return OFU_SET_FIELD_SUCCESS;
}


  int32_t
of_set_icmpv4_code(struct of_msg *msg,
    uint8_t *icmpv4_code )
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_ICMPV4_CODE_FIELD_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  *(uint32_t*)(msg_desc->ptr1+msg_desc->length1) = htonl(OXM_OF_ICMPV4_CODE);
  *(uint8_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = *icmpv4_code; 

  msg_desc->length1 += OFU_ICMPV4_CODE_FIELD_LEN;

  return OFU_SET_FIELD_SUCCESS;
}


  int32_t
of_set_meta_data(struct of_msg *msg,
    uint64_t      *meta_data,
    uint8_t        is_mask_set,
    uint64_t      *meta_data_mask)
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if(is_mask_set)
  {

    if( (msg_desc->ptr1+msg_desc->length1+OFU_META_DATA_MASK_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }

    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)                  = htonl(OXM_OF_METADATA_W);
    *(uint64_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htonll(*meta_data); 
    *(uint64_t*)(msg_desc->ptr1+msg_desc->length1 + 12)             = htonll(*meta_data_mask); 

    msg_desc->length1 += OFU_META_DATA_MASK_LEN;
  }
  else
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_META_DATA_FIELD_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }

    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)                  = htonl(OXM_OF_METADATA);
    *(uint64_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htonll(*meta_data); 

    msg_desc->length1 += OFU_META_DATA_FIELD_LEN;
  }

  return OFU_SET_FIELD_SUCCESS;
}
  int32_t
of_set_ipv6_extension_hdr(struct of_msg *msg,
    uint16_t      *ipv6_ext_hdr_pseudo_fd,
    uint8_t       is_mask_set,
    uint16_t      *ipv6_ext_hdr_pseudo_fd_mask)
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if(is_mask_set)
  {

    if( (msg_desc->ptr1+msg_desc->length1+OFU_IPV6_EXTHDR_MASK_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }

    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)                  = htonl(OXM_OF_IPV6_EXTHDR_W);
    *(uint16_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htons(*ipv6_ext_hdr_pseudo_fd); 
    *(uint16_t*)(msg_desc->ptr1+msg_desc->length1 + OFU_IPV6_EXTHDR_FIELD_LEN)   = htons(*ipv6_ext_hdr_pseudo_fd_mask); 

    msg_desc->length1 += OFU_IPV6_EXTHDR_MASK_LEN;
  }
  else
  {
    if( (msg_desc->ptr1+msg_desc->length1+OFU_IPV6_EXTHDR_FIELD_LEN) >
        (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
      return OFU_NO_ROOM_IN_BUFFER;
    }

    *(uint32_t*)(msg_desc->ptr1+msg_desc->length1)                  = htonl(OXM_OF_IPV6_EXTHDR);
    *(uint16_t*)(msg_desc->ptr1+msg_desc->length1 + OF_OXM_HDR_LEN) = htons(*ipv6_ext_hdr_pseudo_fd); 

    msg_desc->length1 += OFU_IPV6_EXTHDR_FIELD_LEN;
  }

  return OFU_SET_FIELD_SUCCESS;

}

  void
ofu_end_setting_match_field_values(struct of_msg *msg,
    uint8_t  *starting_location,
    uint16_t *length_of_match_fields)
{
  struct ofp_match *match = NULL;
  struct of_msg_descriptor *msg_desc;
  uint16_t match_len;
  uint16_t roundup_bytes;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;
  match = msg_desc->utils_ptr;
  msg_desc->utils_length += ( msg->desc.length1 + 4 );
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg_desc->utils_length %d",msg_desc->utils_length);

  match_len = msg->desc.length1 + match->length;
  match->length = htons(match_len);

  roundup_bytes = ROUND_UP(match_len);
  memset(((uint8_t*)match + 4 + msg->desc.length1),0,roundup_bytes);
  msg->desc.length1 += roundup_bytes;

  msg->desc.data_len += msg->desc.length1;

  *length_of_match_fields = msg->desc.length1;
  starting_location = (uint8_t*)&match;
}
/*########################### Instruction routines #################################*/

  void
ofu_start_pushing_instructions(struct of_msg *msg, uint16_t length_of_match_fields)
{ 
  cntlr_assert(msg != NULL);

  msg->desc.ptr2=  msg->desc.ptr1 + length_of_match_fields; /* atmaram - updated */
  msg->desc.length2 = 0;
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg->desc.ptr1 %p length1 %d msg->desc.ptr2 %p msg->desc.length2 %d msgdec.datalen %d ",msg->desc.ptr1, msg->desc.length1, msg->desc.ptr2, msg->desc.length2, msg->desc.data_len);
}

  int32_t
ofu_start_pushing_apply_action_instruction(struct of_msg *msg)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_instruction_actions *action_instruction;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr2+msg_desc->length2 + OFU_APPLY_ACTION_INSTRUCTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }


  msg_desc->ptr2 = (msg_desc->ptr2 + msg_desc->length2);
  msg_desc->length2=0;
  action_instruction = (struct ofp_instruction_actions*)(msg_desc->ptr2+msg_desc->length2);
  action_instruction->type = htons(OFPIT_APPLY_ACTIONS);
  action_instruction->len  = 0;
  msg_desc->length2 += OFU_APPLY_ACTION_INSTRUCTION_LEN;
  msg_desc->length3  = 0;

  /* Rembering apply action instruction starting point, so that later one can update total action length
  * Currently insturctions are avilable for flow-mode, but if it is added to multiple messages, it may
  * not be useful*/
  msg_desc->ptr3     =  (uint8_t*)action_instruction->actions;

  return OFU_INSTRUCTION_PUSH_SUCCESS;
}

int32_t ofu_start_pushing_write_action_instruction(struct of_msg *msg)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_instruction_actions *action_instruction;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr2+msg_desc->length2 + OFU_WRITE_ACTION_INSTRUCTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  msg_desc->ptr2 = (msg_desc->ptr2 + msg_desc->length2);
  msg_desc->length2=0;
  action_instruction = (struct ofp_instruction_actions*)(msg_desc->ptr2+msg_desc->length2);
  action_instruction->type = htons(OFPIT_WRITE_ACTIONS);
  action_instruction->len  = 0;
  msg_desc->length2 += OFU_WRITE_ACTION_INSTRUCTION_LEN;
  msg_desc->length3  = 0;
  msg_desc->ptr3     =  (uint8_t*)action_instruction->actions;

  return OFU_INSTRUCTION_PUSH_SUCCESS;
}

  int32_t
ofu_push_write_meta_data_instruction(struct of_msg *msg,
    uint64_t meta_data,
    uint64_t meta_data_mask)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_instruction_write_metadata  *metadata_instrctn;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr2+msg_desc->length2 + OFU_WRITE_META_DATA_INSTRUCTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }


  msg_desc->ptr2 = (msg_desc->ptr2 + msg_desc->length2);
  msg_desc->length2=0;
  metadata_instrctn = (struct ofp_instruction_write_metadata*)(msg_desc->ptr2);
  metadata_instrctn->type           = htons(OFPIT_WRITE_METADATA); 
  metadata_instrctn->len            = htons(OFU_WRITE_META_DATA_INSTRUCTION_LEN); 
  metadata_instrctn->metadata       = htonll(meta_data); 
  metadata_instrctn->metadata_mask  = htonll(meta_data_mask); 

  msg_desc->length2 += OFU_WRITE_META_DATA_INSTRUCTION_LEN;
  msg->desc.data_len += msg->desc.length2;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg->desc.ptr1 %p length1 %d msg->desc.ptr2 %p msg->desc.length2 %d msg->desc.ptr3 %p msg->desc.length3 %d msgdec.datalen %d ",msg->desc.ptr1, msg->desc.length1, msg->desc.ptr2, msg->desc.length2,msg->desc.ptr3, msg->desc.length3, msg->desc.data_len);
  return OFU_INSTRUCTION_PUSH_SUCCESS;
}

  int32_t
ofu_push_go_to_table_instruction(struct  of_msg *msg,
    uint8_t table_id)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_instruction_goto_table *gototable_instrctn;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr2+msg_desc->length2 + OFU_GOTO_TABLE_INSTRUCTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }
  msg_desc->ptr2= msg_desc->ptr2+msg_desc->length2;
  msg_desc->length2=0;

  gototable_instrctn = (struct ofp_instruction_goto_table*)(msg_desc->ptr2+msg_desc->length2);

  gototable_instrctn->type      = htons(OFPIT_GOTO_TABLE);
  gototable_instrctn->len       = htons(OFU_GOTO_TABLE_INSTRUCTION_LEN);
  gototable_instrctn->table_id  = table_id;

  msg_desc->length2 += OFU_GOTO_TABLE_INSTRUCTION_LEN;
  msg->desc.data_len += msg->desc.length2;

  return OFU_INSTRUCTION_PUSH_SUCCESS;
}

  int32_t
ofu_push_clear_action_instruction(struct of_msg *msg)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_instruction_actions *clear_action_instrctn;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr2+msg_desc->length2 + OFU_CLEAR_ACTION_INSTRUCTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  msg_desc->ptr2= msg_desc->ptr2+msg_desc->length2;
  msg_desc->length2=0;

  clear_action_instrctn = (struct ofp_instruction_actions*)(msg_desc->ptr2+msg_desc->length2);
  clear_action_instrctn->type      = htons(OFPIT_CLEAR_ACTIONS);
  clear_action_instrctn->len       = htons(8);

  msg_desc->length2 += OFU_CLEAR_ACTION_INSTRUCTION_LEN;
  msg->desc.data_len += msg->desc.length2;

  return OFU_INSTRUCTION_PUSH_SUCCESS;
}

  int32_t
ofu_push_meter_instruction(struct of_msg *msg,
    uint32_t  meter_id)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_instruction_meter *meter_instrctn;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr2+msg_desc->length2 + OFU_METER_INSTRUCTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  msg_desc->ptr2= msg_desc->ptr2+msg_desc->length2;
  msg_desc->length2=0;

  meter_instrctn = (struct ofp_instruction_meter*)(msg_desc->ptr2+msg_desc->length2);
  meter_instrctn->type      = htons(OFPIT_METER);
  meter_instrctn->len       = htons(8);
  meter_instrctn->meter_id  = htonl(meter_id);

  msg_desc->length2 += OFU_METER_INSTRUCTION_LEN;
  msg->desc.data_len += msg->desc.length2;

  return OFU_INSTRUCTION_PUSH_SUCCESS;
}

  void
ofu_end_pushing_apply_action_instruction(struct of_msg *msg,
    uint16_t length_of_actions)
{
  cntlr_assert(msg != NULL);
  ((struct ofp_instruction_actions*)(msg->desc.ptr2))->len = htons(length_of_actions);	
   return;
}
  void
ofu_end_pushing_instructions(struct of_msg *msg,
    uint8_t *starting_location,
    uint16_t *length_of_instructions)
{
  cntlr_assert(msg != NULL);
  starting_location = msg->desc.ptr2;
  *length_of_instructions = msg->desc.length2;
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg->desc.ptr1 %p length1 %d msg->desc.ptr2 %p msg->desc.length2 %d msgdec.datalen %d ",msg->desc.ptr1, msg->desc.length1, msg->desc.ptr2, msg->desc.length2, msg->desc.data_len);
}

/*########################### Action routines #################################*/
  void 
ofu_start_pushing_actions(struct of_msg *msg)
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  msg_desc->ptr3 =  msg_desc->ptr2 + msg_desc->length2;
  msg_desc->length3 = 0; 

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg->desc.ptr1 %p length1 %d msg->desc.ptr2 %p msg->desc.length2 %d msgdec.datalen %d ",msg->desc.ptr1, msg->desc.length1, msg->desc.ptr2, msg->desc.length2, msg->desc.data_len);
}

  int32_t
ofu_push_output_action(struct of_msg *msg,
    uint32_t  port_no,
    uint16_t  max_len)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_output *output_action;

  if(max_len == 0){
	  max_len = 128;
  }

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, " buf loc %p %p",(msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN),(msg->desc.start_of_data + msg_desc->buffer_len) );


  if( (msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }
  output_action = (struct ofp_action_output *)(msg_desc->ptr3+msg_desc->length3);
  output_action->type    = htons(OFPAT_OUTPUT);
  output_action->len     = htons(sizeof(struct ofp_action_output));
  output_action->port    = htonl(port_no);
  output_action->max_len = htons(max_len);
  msg_desc->length3 += OFU_OUTPUT_ACTION_LEN; 
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg->desc.ptr1 %p length1 %d msg->desc.ptr2 %p msg->desc.length2 %d msg->desc.ptr3 %p msg->desc.length3 %d msgdec.datalen %d ",msg->desc.ptr1, msg->desc.length1, msg->desc.ptr2, msg->desc.length2,msg->desc.ptr3, msg->desc.length3, msg->desc.data_len);
  return OFU_ACTION_PUSH_SUCCESS;
}

  int32_t
ofu_push_group_action(struct of_msg *msg,
    uint32_t  group_id)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_group *group_action;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," buf loc %p %p",(msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN),
      (msg->desc.start_of_data + msg_desc->buffer_len) );

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }
  group_action = (struct ofp_action_group *)(msg_desc->ptr3+msg_desc->length3);
  group_action->type    = htons(OFPAT_GROUP);
  group_action->len     = htons(sizeof(struct ofp_action_group));
  group_action->group_id    = htonl(group_id);
  msg_desc->length3 += OFU_GROUP_ACTION_LEN;
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg->desc.ptr1 %p length1 %d msg->desc.ptr2 %p msg->desc.length2 %d msg->desc.ptr3 %p msg->desc.length3 %d msgdec.datalen %d ",msg->desc.ptr1, msg->desc.length1, msg->desc.ptr2, msg->desc.length2,msg->desc.ptr3, msg->desc.length3, msg->desc.data_len);
  return OFU_ACTION_PUSH_SUCCESS;
}

  int32_t
ofu_push_set_queue_action(struct of_msg *msg,
    uint32_t  queue_id)
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," buf loc %p %p",(msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN),
      (msg->desc.start_of_data + msg_desc->buffer_len) );

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  if (msg_desc->type != OFPT_GROUP_MOD)
  {
    /* To be done */
  }
  else
  {
    /* group messages */

  }

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg->desc.ptr1 %p length1 %d msg->desc.ptr2 %p msg->desc.length2 %d msg->desc.ptr3 %p msg->desc.length3 %d msgdec.datalen %d ",msg->desc.ptr1, msg->desc.length1, msg->desc.ptr2, msg->desc.length2,msg->desc.ptr3, msg->desc.length3, msg->desc.data_len);
  return OFU_ACTION_PUSH_SUCCESS;
}

  int32_t
ofu_push_set_mpls_ttl_action(struct of_msg *msg,
    uint8_t  mpls_ttl)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_mpls_ttl *mpls_ttl_action;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"buf loc %p %p",(msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN),
      (msg->desc.start_of_data + msg_desc->buffer_len) );

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }
  mpls_ttl_action = (struct ofp_action_mpls_ttl *)(msg_desc->ptr3+msg_desc->length3);
  mpls_ttl_action->type    = htons(OFPAT_SET_MPLS_TTL);
  mpls_ttl_action->len     = htons(sizeof(struct ofp_action_mpls_ttl));
  mpls_ttl_action->mpls_ttl = mpls_ttl;
  msg_desc->length3 += OFU_SET_MPLS_TTL_ACTION_LEN;
  return OFU_ACTION_PUSH_SUCCESS;
}


  int32_t
ofu_push_dec_mpls_ttl_action(struct of_msg *msg)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_mpls_ttl *mpls_ttl_action;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"buf loc %p %p",(msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN),
      (msg->desc.start_of_data + msg_desc->buffer_len) );

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }
  mpls_ttl_action = (struct ofp_action_mpls_ttl *)(msg_desc->ptr3+msg_desc->length3);
  mpls_ttl_action->type    = htons(OFPAT_DEC_MPLS_TTL);
  mpls_ttl_action->len     = htons(sizeof(struct ofp_action_mpls_ttl));
  msg_desc->length1 += OFU_DEC_MPLS_TTL_ACTION_LEN;
  return OFU_ACTION_PUSH_SUCCESS;
}

  int32_t
ofu_push_set_ipv4_ttl_action(struct of_msg *msg,
    uint8_t  ipv4_ttl)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_nw_ttl *nw_ttl_action;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"buf loc %p %p",(msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN),
      (msg->desc.start_of_data + msg_desc->buffer_len) );

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }
  nw_ttl_action = (struct ofp_action_nw_ttl *)(msg_desc->ptr3+msg_desc->length3);
  nw_ttl_action->type    = htons(OFPAT_SET_NW_TTL);
  nw_ttl_action->len     = htons(sizeof(struct ofp_action_nw_ttl));
  nw_ttl_action->nw_ttl = ipv4_ttl;
  msg_desc->length3 += OFU_SET_IP_TTL_ACTION_LEN;
  return OFU_ACTION_PUSH_SUCCESS;
}

  int32_t
ofu_push_dec_ipv4_ttl_action(struct of_msg *msg)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_nw_ttl *nw_ttl_action;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, " buf loc %p %p",(msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN),
      (msg->desc.start_of_data + msg_desc->buffer_len) );

  if((msg_desc->ptr1+msg_desc->length1 + OFU_DEC_IP_TTL_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  nw_ttl_action = (struct ofp_action_nw_ttl *)(msg_desc->ptr3 + msg_desc->length3);
  nw_ttl_action->type    = htons(OFPAT_DEC_NW_TTL);
  nw_ttl_action->len     = htons(OFU_DEC_IP_TTL_ACTION_LEN);
  msg_desc->length3 += OFU_DEC_IP_TTL_ACTION_LEN; 
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg->desc.ptr1 %p length1 %d msg->desc.ptr2 %p msg->desc.length2 %d msgdec.datalen %d ",msg->desc.ptr1, msg->desc.length1, msg->desc.ptr2, msg->desc.length2, msg->desc.data_len);
  return OFU_ACTION_PUSH_SUCCESS;
}


  int32_t
ofu_push_push_pbb_header_action(struct of_msg *msg,
    uint16_t      ether_type)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_push *push_action;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," buf loc %p %p",(msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN),
      (msg->desc.start_of_data + msg_desc->buffer_len) );

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  push_action = (struct ofp_action_push *)(msg_desc->ptr3+msg_desc->length3);
  push_action->type    = htons(OFPAT_PUSH_PBB);
  push_action->len     = htons(sizeof(struct ofp_action_push));
  push_action->ethertype = htons(ether_type);
  msg_desc->length3 += OFU_PUSH_PBB_HEADER_ACTION_LEN;
  return OFU_ACTION_PUSH_SUCCESS;
}

  int32_t
ofu_push_pop_pbb_header_action(struct of_msg *msg)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_header *pop_action;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," buf loc %p %p",(msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN),
      (msg->desc.start_of_data + msg_desc->buffer_len) );

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  pop_action = (struct ofp_action_header *)(msg_desc->ptr3+msg_desc->length3);
  pop_action->type    = htons(OFPAT_POP_PBB);
  pop_action->len     = htons(sizeof(struct ofp_action_header));
  msg_desc->length3 += OFU_POP_PBB_HEADER_ACTION_LEN;
  return OFU_ACTION_PUSH_SUCCESS;
}

  int32_t
ofu_push_push_mpls_header_action(struct of_msg *msg,
    uint16_t      ether_type)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_push *push_action;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," buf loc %p %p",(msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN),
      (msg->desc.start_of_data + msg_desc->buffer_len) );

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  push_action = (struct ofp_action_push *)(msg_desc->ptr3+msg_desc->length3);
  push_action->type    = htons(OFPAT_PUSH_MPLS);
  push_action->len     = htons(sizeof(struct ofp_action_push));
  push_action->ethertype = htons(ether_type);
  msg_desc->length3 += OFU_PUSH_MPLS_HEADER_ACTION_LEN;
  return OFU_ACTION_PUSH_SUCCESS;
}

  int32_t
ofu_push_pop_mpls_header_action(struct of_msg *msg,
    uint16_t      ether_type)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_pop_mpls *pop_mpls_action;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," buf loc %p %p",(msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN),
      (msg->desc.start_of_data + msg_desc->buffer_len) );

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  pop_mpls_action = (struct ofp_action_pop_mpls *)(msg_desc->ptr3+msg_desc->length3);
  pop_mpls_action->type    = htons(OFPAT_POP_MPLS);
  pop_mpls_action->len     = htons(sizeof(struct ofp_action_pop_mpls));
  pop_mpls_action->ethertype = htons(ether_type);
  msg_desc->length3 += OFU_POP_MPLS_HEADER_ACTION_LEN;
  return OFU_ACTION_PUSH_SUCCESS;
}


  int32_t
ofu_push_push_vlan_header_action(struct of_msg *msg,
    uint16_t      ether_type)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_push *push_action;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, " buf loc %p %p",(msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN),
      (msg->desc.start_of_data + msg_desc->buffer_len) );

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  push_action = (struct ofp_action_push *)(msg_desc->ptr3+msg_desc->length3);
  push_action->type    = htons(OFPAT_PUSH_VLAN);
  push_action->len     = htons(sizeof(struct ofp_action_push));
  push_action->ethertype = htons(ether_type);
  msg_desc->length3 += OFU_PUSH_VLAN_HEADER_ACTION_LEN;
  return OFU_ACTION_PUSH_SUCCESS;
}
  int32_t
ofu_push_pop_vlan_header_action(struct of_msg *msg)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_header *pop_action;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "buf loc %p %p",(msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN),
      (msg->desc.start_of_data + msg_desc->buffer_len) );

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  pop_action = (struct ofp_action_header *)(msg_desc->ptr3+msg_desc->length3);
  pop_action->type    = htons(OFPAT_POP_VLAN);
  pop_action->len     = htons(sizeof(struct ofp_action_header));
  msg_desc->length3 += OFU_POP_VLAN_HEADER_ACTION_LEN;
  return OFU_ACTION_PUSH_SUCCESS;
}

  int32_t
ofu_push_copy_ttl_inward_action(struct of_msg *msg)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_header *ttl_inward_action;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "buf loc %p %p",(msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN),
      (msg->desc.start_of_data + msg_desc->buffer_len) );

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  ttl_inward_action = (struct ofp_action_header *)(msg_desc->ptr3+msg_desc->length3);
  ttl_inward_action->type    = htons(OFPAT_COPY_TTL_IN);
  ttl_inward_action->len     = htons(sizeof(struct ofp_action_header));
  msg_desc->length3= OFU_COPY_TTL_OUTWARD_ACTION_LEN;
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg->desc.ptr1 %p length1 %d msg->desc.ptr2 %p msg->desc.length2 %d msgdec.datalen %d ",msg->desc.ptr1, msg->desc.length1, msg->desc.ptr2, msg->desc.length2, msg->desc.data_len);
  return OFU_ACTION_PUSH_SUCCESS;
}


  int32_t
ofu_push_copy_ttl_outward_action(struct of_msg *msg)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_header *ttl_outward_action;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "buf loc %p %p",(msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN),
      (msg->desc.start_of_data + msg_desc->buffer_len) );

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_OUTPUT_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }
  ttl_outward_action = (struct ofp_action_header *)(msg_desc->ptr3+msg_desc->length3);
  ttl_outward_action->type    = htons(OFPAT_COPY_TTL_OUT);
  ttl_outward_action->len     = htons(sizeof(struct ofp_action_header));
  msg_desc->length3 += OFU_COPY_TTL_OUTWARD_ACTION_LEN; 
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg->desc.ptr1 %p length1 %d msg->desc.ptr2 %p msg->desc.length2 %d msgdec.datalen %d ",msg->desc.ptr1, msg->desc.length1, msg->desc.ptr2, msg->desc.length2, msg->desc.data_len);
  return OFU_ACTION_PUSH_SUCCESS;
}
  int32_t
ofu_push_set_port_in_set_field_action(struct    of_msg *msg,
    uint32_t *in_port_number)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(4);
  OF_LOG_MSG(OF_LOG_MOD,OF_LOG_DEBUG,"ofu_push_set_port_in_set_field_action Entered");

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_SET_FIELD_WITH_IN_PORT_LABEL_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);

  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_IN_PORT_LABEL_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field = htonl(OXM_OF_IN_PORT);
  *(uint32_t*)(set_field->field + OF_OXM_HDR_LEN) = htonl(*in_port_number);
  memset( (set_field->field + OFU_SET_FIELD_WITH_IN_PORT_LABEL_FIELD_ACTION_LEN), 0, padding_bytes);   
  msg_desc->length3 += OFU_SET_FIELD_WITH_IN_PORT_LABEL_FIELD_ACTION_LEN + padding_bytes;

  return OFU_ACTION_PUSH_SUCCESS;
}

  int32_t
ofu_push_set_in_physical_port_in_set_field_action(struct    of_msg *msg,
    uint32_t *in_phy_port_no)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(4);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_SET_FIELD_WITH_PHY_PORT_LABEL_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);

  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_PHY_PORT_LABEL_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field = htonl(OXM_OF_IN_PHY_PORT);
  *(uint32_t*)(set_field->field + OF_OXM_HDR_LEN) = htonl(*in_phy_port_no);
  memset( (set_field->field + OFU_SET_FIELD_WITH_PHY_PORT_LABEL_FIELD_ACTION_LEN), 0, padding_bytes);   
  msg_desc->length3 += OFU_SET_FIELD_WITH_PHY_PORT_LABEL_FIELD_ACTION_LEN + padding_bytes;

  return OFU_ACTION_PUSH_SUCCESS;

}



  int32_t
ofu_push_set_mpls_label_in_set_field_action(struct of_msg *msg,
    uint32_t  *mpls_label)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(4);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_SET_FIELD_WITH_MPLS_LABEL_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);

  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_MPLS_LABEL_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field = htonl(OXM_OF_MPLS_LABEL);
  *(uint32_t*)(set_field->field + OF_OXM_HDR_LEN) = htonl(*mpls_label);
  memset( (set_field->field + OFU_SET_FIELD_WITH_MPLS_LABEL_FIELD_ACTION_LEN),0,padding_bytes);   
  msg_desc->length3 += OFU_SET_FIELD_WITH_MPLS_LABEL_FIELD_ACTION_LEN+padding_bytes;

  return OFU_ACTION_PUSH_SUCCESS;
}

  int32_t
ofu_push_set_mpls_tc_in_set_field_action(struct of_msg *msg,
    uint8_t  *mpls_tc)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(1);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_SET_FIELD_WITH_MPLS_TC_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);

  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_MPLS_TC_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field = htonl(OXM_OF_MPLS_TC);
  *(uint8_t*)(set_field->field + OF_OXM_HDR_LEN) = *mpls_tc;
  memset( (set_field->field + OFU_SET_FIELD_WITH_MPLS_TC_FIELD_ACTION_LEN),0,padding_bytes);   
  msg_desc->length3 += OFU_SET_FIELD_WITH_MPLS_TC_FIELD_ACTION_LEN + padding_bytes;

  return OFU_ACTION_PUSH_SUCCESS;
}

  int32_t
ofu_push_set_mpls_bos_bits_in_set_field_action(struct of_msg *msg,
    uint8_t  *mpls_bos_bits)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(1);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_SET_FIELD_WITH_MPLS_BOS_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);

  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_MPLS_BOS_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field = htonl(OXM_OF_MPLS_BOS);
  *(uint8_t*)(set_field->field + OF_OXM_HDR_LEN) = *mpls_bos_bits;
  memset( (set_field->field + OFU_SET_FIELD_WITH_MPLS_BOS_FIELD_ACTION_LEN),0,padding_bytes);   
  msg_desc->length3 += OFU_SET_FIELD_WITH_MPLS_BOS_FIELD_ACTION_LEN + padding_bytes;

  return OFU_ACTION_PUSH_SUCCESS;
}



  int32_t
ofu_push_set_pbb_isid_in_set_field_action(struct of_msg *msg,
    uint8_t  *pbb_isid)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(1);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_SET_FIELD_WITH_PBB_ISID_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);

  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_PBB_ISID_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field = htonl(OXM_OF_PBB_ISID);
  *(uint8_t*)(set_field->field + OF_OXM_HDR_LEN) = *pbb_isid;
  memset( (set_field->field + OFU_SET_FIELD_WITH_PBB_ISID_FIELD_ACTION_LEN),0,padding_bytes);   
  msg_desc->length3 += OFU_SET_FIELD_WITH_PBB_ISID_FIELD_ACTION_LEN + padding_bytes;

  return OFU_ACTION_PUSH_SUCCESS;
}
  int32_t
ofu_push_set_logical_port_meta_data_in_set_field_action(struct of_msg *msg,
    uint64_t      *tunnel_id)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(8);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_SET_FIELD_WITH_TUNNEL_ID_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);

  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_TUNNEL_ID_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field = htonl(OXM_OF_TUNNEL_ID);
  *(uint64_t*)(set_field->field + OF_OXM_HDR_LEN) = htonll(*tunnel_id);
  memset((set_field->field + OFU_SET_FIELD_WITH_TUNNEL_ID_FIELD_ACTION_LEN), 0, padding_bytes);   
  msg_desc->length3 += OFU_SET_FIELD_WITH_TUNNEL_ID_FIELD_ACTION_LEN + padding_bytes;

  return OFU_ACTION_PUSH_SUCCESS;
}



  int32_t
ofu_push_set_destination_mac_in_set_field_action(struct of_msg *msg,
    uchar8_t *dest_mac_addr)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(6);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_SET_FIELD_WITH_ETH_DST_MAC_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);

  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_ETH_DST_MAC_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field = htonl(OXM_OF_ETH_DST);
  memcpy( (set_field->field + OF_OXM_HDR_LEN), dest_mac_addr,6);
  memset( (set_field->field + OFU_SET_FIELD_WITH_ETH_DST_MAC_ACTION_LEN),0,padding_bytes);

  msg_desc->length3 += OFU_SET_FIELD_WITH_ETH_DST_MAC_ACTION_LEN+padding_bytes;

  return OFU_ACTION_PUSH_SUCCESS;

}
  int32_t
ofu_push_set_source_mac_in_set_field_action(struct of_msg *msg,
    uint8_t *source_mac_addr)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(6);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_SET_FIELD_WITH_ETH_SRC_MAC_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);

  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_ETH_SRC_MAC_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field = htonl(OXM_OF_ETH_SRC);
  memcpy( (set_field->field + OF_OXM_HDR_LEN), source_mac_addr, 6);
  memset( (set_field->field + OFU_SET_FIELD_WITH_ETH_SRC_MAC_ACTION_LEN),0,padding_bytes);

  msg_desc->length3 += OFU_SET_FIELD_WITH_ETH_SRC_MAC_ACTION_LEN+padding_bytes; 

  return OFU_ACTION_PUSH_SUCCESS;
}
  int32_t
ofu_push_set_eth_type_in_set_field_action(struct of_msg *msg,
    uint16_t      *eth_frame_type)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(2);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_SET_FIELD_WITH_ETH_TYPE_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len))
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }
  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_ETH_TYPE_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field = htonl(OXM_OF_ETH_TYPE);

  *(uint16_t*)(set_field->field + OF_OXM_HDR_LEN) = htons(*eth_frame_type);
  memset( (set_field->field + OFU_SET_FIELD_WITH_ETH_TYPE_ACTION_LEN),0,padding_bytes);   

  msg_desc->length3 += OFU_SET_FIELD_WITH_ETH_TYPE_ACTION_LEN+padding_bytes;

  return OFU_ACTION_PUSH_SUCCESS;
}
  int32_t
ofu_push_set_vlan_id_in_set_field_action(struct of_msg *msg,
    uint16_t      *vlan_id)
{
  struct of_msg_descriptor *msg_desc = &msg->desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(2);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_SET_FIELD_WITH_VLAN_VID_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }
  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_VLAN_VID_ACTION_LEN + padding_bytes);

  *(uint32_t*)set_field->field   = htonl(OXM_OF_VLAN_VID);
  *(uint16_t*)(set_field->field + OF_OXM_HDR_LEN) = htons(*vlan_id);
  memset( (set_field->field + OFU_SET_FIELD_WITH_VLAN_VID_ACTION_LEN),0,padding_bytes);

  msg_desc->length3 += OFU_SET_FIELD_WITH_VLAN_VID_ACTION_LEN+padding_bytes;   
  return OFU_ACTION_PUSH_SUCCESS;

}

  int32_t
ofu_push_set_vlan_priority_bits_in_set_field_action(struct of_msg *msg,
    uint8_t       *vlan_priority_bits)
{
  struct of_msg_descriptor *msg_desc = &msg->desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(1);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_SET_FIELD_WITH_VLAN_PCP_ACTION_LEN ) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }
  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_VLAN_PCP_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field   = htonl(OXM_OF_VLAN_PCP);
  *(uint8_t*)(set_field->field + OF_OXM_HDR_LEN) = *vlan_priority_bits;
  memset( (set_field->field + OFU_SET_FIELD_WITH_VLAN_PCP_ACTION_LEN),0,padding_bytes);
  msg_desc->length3 += OFU_SET_FIELD_WITH_VLAN_PCP_ACTION_LEN + padding_bytes;
  return OFU_ACTION_PUSH_SUCCESS;
}
  int32_t
ofu_push_set_ip_dscp_bits_in_set_field_action(struct of_msg *msg,
    uint8_t      *ip_dscp_bits)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(1);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_SET_FIELD_WITH_IP_DSCP_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_IP_DSCP_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field   = htonl(OXM_OF_IP_DSCP);
  *(uint8_t*)(set_field->field + OF_OXM_HDR_LEN) = *ip_dscp_bits;
  memset( (set_field->field + OFU_SET_FIELD_WITH_IP_DSCP_FIELD_ACTION_LEN),0,padding_bytes);
  msg_desc->length3 += OFU_SET_FIELD_WITH_IP_DSCP_FIELD_ACTION_LEN + padding_bytes;
  return OFU_ACTION_PUSH_SUCCESS;

}


  int32_t
ofu_push_set_ip_ecn_bits_in_set_field_action(struct of_msg *msg,
    uint8_t      *ip_ecn_bits)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(1);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_SET_FIELD_WITH_IP_ECN_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_IP_ECN_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field   = htonl(OXM_OF_IP_ECN);
  *(uint8_t*)(set_field->field + OF_OXM_HDR_LEN) = *ip_ecn_bits;
  memset( (set_field->field + OFU_SET_FIELD_WITH_IP_ECN_FIELD_ACTION_LEN),0,padding_bytes);
  msg_desc->length3 += OFU_SET_FIELD_WITH_IP_ECN_FIELD_ACTION_LEN + padding_bytes;
  return OFU_ACTION_PUSH_SUCCESS;

}

  int32_t
ofu_push_set_ip_protocol_in_set_field_action(struct of_msg *msg,
    uint8_t      *ip_protocol)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(1);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_SET_FIELD_WITH_IP_PROTO_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_IP_PROTO_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field   = htonl(OXM_OF_IP_PROTO);
  *(uint8_t*)(set_field->field + OF_OXM_HDR_LEN) = *ip_protocol;
  memset( (set_field->field + OFU_SET_FIELD_WITH_IP_PROTO_FIELD_ACTION_LEN),0,padding_bytes);
  msg_desc->length3 += OFU_SET_FIELD_WITH_IP_PROTO_FIELD_ACTION_LEN + padding_bytes;
  return OFU_ACTION_PUSH_SUCCESS;
}
  int32_t
ofu_push_set_ipv4_src_addr_in_set_field_action(struct of_msg *msg,
    ipv4_addr_t   *ipv4_addr)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(4);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;


  if( (msg_desc->ptr1+msg_desc->length1+OFU_SET_FIELD_WITH_IPV4_SRC_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }
  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_IPV4_SRC_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field   = htonl(OXM_OF_IPV4_SRC);
  *(uint32_t*)(set_field->field + OF_OXM_HDR_LEN) = htonl(*ipv4_addr);
  memset( (set_field->field + OFU_SET_FIELD_WITH_IPV4_SRC_FIELD_ACTION_LEN),0,padding_bytes);
  msg_desc->length3 += OFU_SET_FIELD_WITH_IPV4_SRC_FIELD_ACTION_LEN + padding_bytes;
  return OFU_ACTION_PUSH_SUCCESS;

}
  int32_t
ofu_push_set_source_ipv6_addr_in_set_field_action(struct of_msg *msg,
    ipv6_addr_t   *ipv6_addr)

{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(16);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;


  if( (msg_desc->ptr1+msg_desc->length1+OFU_SET_FIELD_WITH_IPV6_SRC_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }
  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_IPV6_SRC_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field   = htonl(OXM_OF_IPV6_SRC);
  memcpy((ipv6_addr_t *)(set_field->field + OF_OXM_HDR_LEN) , &(ipv6_addr->ipv6Addr32), sizeof(ipv6_addr->ipv6Addr32)); 
  memset( (set_field->field + OFU_SET_FIELD_WITH_IPV6_SRC_FIELD_ACTION_LEN),0,padding_bytes);
  msg_desc->length3 += OFU_SET_FIELD_WITH_IPV6_SRC_FIELD_ACTION_LEN + padding_bytes;
  return OFU_ACTION_PUSH_SUCCESS;

}
  int32_t
ofu_push_set_destination_ipv6_addr_in_set_field_action(struct of_msg *msg,
    ipv6_addr_t   *ipv6_addr)

{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(16);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;


  if( (msg_desc->ptr1+msg_desc->length1+OFU_SET_FIELD_WITH_IPV6_DST_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }
  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_IPV6_DST_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field   = htonl(OXM_OF_IPV6_DST);
  memcpy((ipv6_addr_t *)(set_field->field + OF_OXM_HDR_LEN) , &(ipv6_addr->ipv6Addr32), sizeof(ipv6_addr->ipv6Addr32)); 
  memset( (set_field->field + OFU_SET_FIELD_WITH_IPV6_DST_FIELD_ACTION_LEN),0,padding_bytes);
  msg_desc->length3 += OFU_SET_FIELD_WITH_IPV6_DST_FIELD_ACTION_LEN + padding_bytes;
  return OFU_ACTION_PUSH_SUCCESS;

}

  int32_t
ofu_push_set_ipv6_flow_label_in_set_field_action(struct of_msg *msg,
    uint32_t  *flow_label)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(4);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;


  if( (msg_desc->ptr1+msg_desc->length1+OFU_SET_FIELD_WITH_IPV6_FLABEL_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }
  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_IPV6_FLABEL_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field   = htonl(OXM_OF_IPV6_FLABEL);
  *(uint32_t *)(set_field->field + OF_OXM_HDR_LEN) = htonl(*flow_label); 
  memset( (set_field->field + OFU_SET_FIELD_WITH_IPV6_FLABEL_FIELD_ACTION_LEN),0,padding_bytes);
  msg_desc->length3 += OFU_SET_FIELD_WITH_IPV6_FLABEL_FIELD_ACTION_LEN + padding_bytes;
  return OFU_ACTION_PUSH_SUCCESS;

}

  int32_t
ofu_push_set_icmpv6_type_in_set_field_action(struct of_msg *msg,
    uint8_t  *icmpv6_type)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(1);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;


  if( (msg_desc->ptr1+msg_desc->length1+OFU_SET_FIELD_WITH_ICMPV6_TYPE_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }
  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_ICMPV6_TYPE_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field   = htonl(OXM_OF_ICMPV6_TYPE);
  *(uint8_t *)(set_field->field + OF_OXM_HDR_LEN) = *icmpv6_type; 
  memset( (set_field->field + OFU_SET_FIELD_WITH_ICMPV6_TYPE_FIELD_ACTION_LEN),0,padding_bytes);
  msg_desc->length3 += OFU_SET_FIELD_WITH_ICMPV6_TYPE_FIELD_ACTION_LEN + padding_bytes;
  return OFU_ACTION_PUSH_SUCCESS;
}

  int32_t
ofu_push_set_icmpv6_code_in_set_field_action(struct of_msg *msg,
    uint8_t  *icmpv6_code)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(1);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;


  if( (msg_desc->ptr1+msg_desc->length1+OFU_SET_FIELD_WITH_ICMPV6_CODE_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }
  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD);
  set_field->len  = htons(OFU_SET_FIELD_WITH_ICMPV6_CODE_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field   = htonl(OXM_OF_ICMPV6_CODE);
  *(uint8_t *)(set_field->field + OF_OXM_HDR_LEN) = *icmpv6_code;
  memset( (set_field->field + OFU_SET_FIELD_WITH_ICMPV6_CODE_FIELD_ACTION_LEN),0,padding_bytes);
  msg_desc->length3 += OFU_SET_FIELD_WITH_ICMPV6_CODE_FIELD_ACTION_LEN + padding_bytes;
  return OFU_ACTION_PUSH_SUCCESS;
}

  int32_t
ofu_push_set_nd_target_ipv6_addr_in_set_field_action(struct of_msg *msg,
    ipv6_addr_t *ipv6_addr)
{  
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(16);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;


  if( (msg_desc->ptr1+msg_desc->length1+OFU_SET_FIELD_WITH_IPV6_ND_TARGET_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }
  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_IPV6_ND_TARGET_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field   = htonl(OXM_OF_IPV6_ND_TARGET);
  memcpy((ipv6_addr_t *)(set_field->field + OF_OXM_HDR_LEN) , &(ipv6_addr->ipv6Addr32), sizeof(ipv6_addr->ipv6Addr32)); 
  memset( (set_field->field + OFU_SET_FIELD_WITH_IPV6_ND_TARGET_FIELD_ACTION_LEN),0,padding_bytes);
  msg_desc->length3 += OFU_SET_FIELD_WITH_IPV6_ND_TARGET_FIELD_ACTION_LEN + padding_bytes;
  return OFU_ACTION_PUSH_SUCCESS;

}
  int32_t
ofu_push_set_nd_source_link_layer_addr_in_set_field_action(struct of_msg *msg,
    uint8_t  *hw_addr)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(6);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_SET_FIELD_WITH_IPV6_ND_SLL_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);

  set_field->type = htons(OFPAT_SET_FIELD);
  set_field->len  = htons(OFU_SET_FIELD_WITH_IPV6_ND_SLL_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field = htonl(OXM_OF_IPV6_ND_SLL);
  memcpy( (set_field->field + OF_OXM_HDR_LEN), hw_addr, 6);
  memset( (set_field->field + OFU_SET_FIELD_WITH_IPV6_ND_SLL_FIELD_ACTION_LEN),0,padding_bytes);

  msg_desc->length3 += OFU_SET_FIELD_WITH_IPV6_ND_SLL_FIELD_ACTION_LEN + padding_bytes;

  return OFU_ACTION_PUSH_SUCCESS;

}
  int32_t
ofu_push_set_nd_target_link_layer_addr_in_set_field_action(struct of_msg *msg,
    uint8_t  *hw_addr)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(6);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_SET_FIELD_WITH_IPV6_ND_DLL_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);

  set_field->type = htons(OFPAT_SET_FIELD);
  set_field->len  = htons(OFU_SET_FIELD_WITH_IPV6_ND_DLL_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field = htonl(OXM_OF_IPV6_ND_TLL);
  memcpy( (set_field->field + OF_OXM_HDR_LEN), hw_addr, 6);
  memset( (set_field->field + OFU_SET_FIELD_WITH_IPV6_ND_DLL_FIELD_ACTION_LEN),0,padding_bytes);

  msg_desc->length3 += OFU_SET_FIELD_WITH_IPV6_ND_DLL_FIELD_ACTION_LEN + padding_bytes;

  return OFU_ACTION_PUSH_SUCCESS;
}
  int32_t
ofu_push_set_ipv6_exthdr_field_in_set_field_action(struct of_msg *msg,
    uint16_t  *ipv6_exthdr_pseudo_fields)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(2);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1 + OFU_SET_FIELD_WITH_IPV6_EXTHDR_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL,"%s %d :Length of buffer is not sufficient to add data\r\n",
        __FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);

  set_field->type = htons(OFPAT_SET_FIELD);
  set_field->len  = htons(OFU_SET_FIELD_WITH_IPV6_EXTHDR_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t *)set_field->field = htonl(OXM_OF_IPV6_EXTHDR);
  *(uint16_t *)(set_field->field + OF_OXM_HDR_LEN) = htons(*ipv6_exthdr_pseudo_fields);
  memset( (set_field->field + OFU_SET_FIELD_WITH_IPV6_EXTHDR_FIELD_ACTION_LEN),0,padding_bytes);

  msg_desc->length3 += OFU_SET_FIELD_WITH_IPV6_EXTHDR_FIELD_ACTION_LEN + padding_bytes;

  return OFU_ACTION_PUSH_SUCCESS;
}


  int32_t
ofu_push_set_ipv4_dst_addr_in_set_field_action(struct of_msg *msg,
    ipv4_addr_t   *ipv4_addr)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(4);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;


  if( (msg_desc->ptr1+msg_desc->length1+OFU_SET_FIELD_WITH_IPV4_DST_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_IPV4_DST_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field   = htonl(OXM_OF_IPV4_DST);
  *(uint32_t*)(set_field->field + OF_OXM_HDR_LEN) = htonl(*ipv4_addr);
  memset( (set_field->field + OFU_SET_FIELD_WITH_IPV4_DST_FIELD_ACTION_LEN),0,padding_bytes);   
  msg_desc->length3 += OFU_SET_FIELD_WITH_IPV4_DST_FIELD_ACTION_LEN + padding_bytes;
  return OFU_ACTION_PUSH_SUCCESS;

}
  int32_t
ofu_push_set_tcp_src_port_in_set_field_action(struct of_msg *msg,
    uint16_t  *port_number)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(2);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_SET_FIELD_WITH_TCP_SRC_PORT_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_TCP_SRC_PORT_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field   = htonl(OXM_OF_TCP_SRC);
  *(uint16_t*)(set_field->field + OF_OXM_HDR_LEN) = htons(*port_number);
  memset( (set_field->field + OFU_SET_FIELD_WITH_TCP_SRC_PORT_FIELD_ACTION_LEN),0,padding_bytes);   
  msg_desc->length3 += OFU_SET_FIELD_WITH_TCP_SRC_PORT_FIELD_ACTION_LEN + padding_bytes;

  return OFU_ACTION_PUSH_SUCCESS;

}

  int32_t
ofu_push_set_tcp_dst_port_in_set_field_action(struct of_msg *msg,
    uint16_t  *port_number)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(2);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_SET_FIELD_WITH_TCP_DST_PORT_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }
  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_TCP_DST_PORT_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field   = htonl(OXM_OF_TCP_DST);
  *(uint16_t*)(set_field->field + OF_OXM_HDR_LEN) = htons(*port_number);
  memset( (set_field->field + OFU_SET_FIELD_WITH_TCP_DST_PORT_FIELD_ACTION_LEN),0,padding_bytes);   
  msg_desc->length3 += OFU_SET_FIELD_WITH_TCP_DST_PORT_FIELD_ACTION_LEN + padding_bytes;


  return OFU_ACTION_PUSH_SUCCESS;
}
  int32_t
ofu_push_set_udp_src_port_in_set_field_action(struct of_msg *msg,
    uint16_t  *port_number)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(2);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_SET_FIELD_WITH_UDP_SRC_PORT_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_UDP_SRC_PORT_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field   = htonl(OXM_OF_UDP_SRC);
  *(uint16_t*)(set_field->field + OF_OXM_HDR_LEN) = htons(*port_number);
  memset( (set_field->field + OFU_SET_FIELD_WITH_UDP_SRC_PORT_FIELD_ACTION_LEN),0,padding_bytes);   
  msg_desc->length3 += OFU_SET_FIELD_WITH_UDP_SRC_PORT_FIELD_ACTION_LEN + padding_bytes;

  return OFU_ACTION_PUSH_SUCCESS;
}

  int32_t
ofu_push_set_udp_dst_port_in_set_field_action(struct of_msg *msg,
    uint16_t  *port_number)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(2);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_SET_FIELD_WITH_UDP_DST_PORT_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"Length of buffer is not sufficient to add data\r\n");
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_UDP_DST_PORT_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field   = htonl(OXM_OF_UDP_DST);
  *(uint16_t*)(set_field->field + OF_OXM_HDR_LEN) = htons(*port_number);
  memset( (set_field->field + OFU_SET_FIELD_WITH_UDP_DST_PORT_FIELD_ACTION_LEN),0,padding_bytes);   
  msg_desc->length3 += OFU_SET_FIELD_WITH_UDP_DST_PORT_FIELD_ACTION_LEN + padding_bytes;

  return OFU_ACTION_PUSH_SUCCESS;
}
  int32_t
ofu_push_set_sctp_src_port_in_set_field_action(struct of_msg *msg,
    uint16_t  *port_number)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(2);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_SET_FIELD_WITH_SCTP_SRC_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"Length of buffer is not sufficient to add data\r\n");
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_SCTP_SRC_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field   = htonl(OXM_OF_SCTP_SRC);
  *(uint16_t*)(set_field->field + OF_OXM_HDR_LEN) = htons(*port_number);
  memset( (set_field->field + OFU_SET_FIELD_WITH_SCTP_SRC_FIELD_ACTION_LEN),0,padding_bytes);   
  msg_desc->length3 += OFU_SET_FIELD_WITH_SCTP_SRC_FIELD_ACTION_LEN + padding_bytes;

  return OFU_ACTION_PUSH_SUCCESS;
}
  int32_t
ofu_push_set_sctp_dst_port_in_set_field_action(struct of_msg *msg,
    uint16_t  *port_number)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(2);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_SET_FIELD_WITH_SCTP_DST_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"Length of buffer is not sufficient to add data\r\n");
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_SCTP_DST_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field   = htonl(OXM_OF_SCTP_DST);
  *(uint16_t*)(set_field->field + OF_OXM_HDR_LEN) = htons(*port_number);
  memset( (set_field->field + OFU_SET_FIELD_WITH_SCTP_DST_FIELD_ACTION_LEN),0,padding_bytes);   
  msg_desc->length3 += OFU_SET_FIELD_WITH_SCTP_DST_FIELD_ACTION_LEN + padding_bytes;

  return OFU_ACTION_PUSH_SUCCESS;
}
  int32_t
ofu_push_set_icmpv4_type_in_set_field_action(struct of_msg *msg,
    uint8_t  *icmpv4_type)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(1);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_SET_FIELD_WITH_ICPMPV4_TYPE_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"Length of buffer is not sufficient to add data\r\n");
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_ICPMPV4_TYPE_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field   = htonl(OXM_OF_ICMPV4_TYPE);
  *(uint8_t*)(set_field->field + OF_OXM_HDR_LEN) = *icmpv4_type;
  memset( (set_field->field + OFU_SET_FIELD_WITH_ICPMPV4_TYPE_FIELD_ACTION_LEN),0,padding_bytes);   
  msg_desc->length3 += OFU_SET_FIELD_WITH_ICPMPV4_TYPE_FIELD_ACTION_LEN + padding_bytes;

  return OFU_ACTION_PUSH_SUCCESS;
}

  int32_t
ofu_push_set_icmpv4_code_in_set_field_action(struct of_msg *msg,
    uint8_t  *icmpv4_code)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(1);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_SET_FIELD_WITH_ICPMPV4_CODE_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"Length of buffer is not sufficient to add data\r\n");
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_ICPMPV4_CODE_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field   = htonl(OXM_OF_ICMPV4_CODE);
  *(uint8_t*)(set_field->field + OF_OXM_HDR_LEN) = *icmpv4_code;
  memset( (set_field->field + OFU_SET_FIELD_WITH_ICPMPV4_CODE_FIELD_ACTION_LEN),0,padding_bytes);   
  msg_desc->length3 += OFU_SET_FIELD_WITH_ICPMPV4_CODE_FIELD_ACTION_LEN + padding_bytes;

  return OFU_ACTION_PUSH_SUCCESS;
}
  int32_t
ofu_push_set_arp_opcode_in_set_field_action(struct of_msg *msg,
    uint16_t  *arp_opcode)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(2);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+OFU_SET_FIELD_WITH_ARP_OP_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"Length of buffer is not sufficient to add data\r\n");
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_ARP_OP_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field   = htonl(OXM_OF_ARP_OP);
  *(uint16_t*)(set_field->field + OF_OXM_HDR_LEN) = htons(*arp_opcode);
  memset( (set_field->field +  OFU_SET_FIELD_WITH_ARP_OP_FIELD_ACTION_LEN),0,padding_bytes);   
  msg_desc->length3 += OFU_SET_FIELD_WITH_ARP_OP_FIELD_ACTION_LEN + padding_bytes;

  return OFU_ACTION_PUSH_SUCCESS;
}
  int32_t
ofu_push_set_arp_source_ipv4_addr_in_set_field_action(struct of_msg *msg,
    ipv4_addr_t  *ipv4_addr)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(4);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+ OFU_SET_FIELD_WITH_ARP_SPA_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"Length of buffer is not sufficient to add data\r\n");
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons( OFU_SET_FIELD_WITH_ARP_SPA_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field   = htonl(OXM_OF_ARP_SPA);
  *(uint32_t*)(set_field->field +  OF_OXM_HDR_LEN) = htonl(*ipv4_addr);
  memset( (set_field->field + OFU_SET_FIELD_WITH_ARP_SPA_FIELD_ACTION_LEN),0,padding_bytes);   
  msg_desc->length3 +=  OFU_SET_FIELD_WITH_ARP_SPA_FIELD_ACTION_LEN + padding_bytes;

  return OFU_ACTION_PUSH_SUCCESS;
}
  int32_t
ofu_push_set_arp_target_ipv4_addr_in_set_field_action(struct of_msg *msg,
    ipv4_addr_t  *ipv4_addr)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(4);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+ OFU_SET_FIELD_WITH_ARP_TPA_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"Length of buffer is not sufficient to add data\r\n");
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons( OFU_SET_FIELD_WITH_ARP_TPA_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field   = htonl(OXM_OF_ARP_TPA);
  *(uint32_t*)(set_field->field + OF_OXM_HDR_LEN ) = htonl(*ipv4_addr);
  memset( (set_field->field + OFU_SET_FIELD_WITH_ARP_TPA_FIELD_ACTION_LEN),0,padding_bytes);   
  msg_desc->length3 += OFU_SET_FIELD_WITH_ARP_TPA_FIELD_ACTION_LEN + padding_bytes;

  return OFU_ACTION_PUSH_SUCCESS;
}
  int32_t
ofu_push_set_arp_source_hw_addr_in_set_field_action(struct of_msg *msg,
    uint8_t  *hw_addr)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(6);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+ OFU_SET_FIELD_WITH_ARP_SHA_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"Length of buffer is not sufficient to add data\r\n");
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_ARP_SHA_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field   = htonl(OXM_OF_ARP_SHA);
  memcpy( (set_field->field + OF_OXM_HDR_LEN), hw_addr, 6);
  memset( (set_field->field + OFU_SET_FIELD_WITH_ARP_SHA_FIELD_ACTION_LEN), 0, padding_bytes);
  msg_desc->length3 += OFU_SET_FIELD_WITH_ARP_SHA_FIELD_ACTION_LEN + padding_bytes;

  return OFU_ACTION_PUSH_SUCCESS;
}
  int32_t
ofu_push_set_arp_target_hw_addr_in_set_field_action(struct of_msg *msg,
    uint8_t  *hw_addr)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_action_set_field *set_field;
  uint16_t padding_bytes = OF_SET_FIELD_ACTION_PADDING_LEN(6);

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  if( (msg_desc->ptr1+msg_desc->length1+ OFU_SET_FIELD_WITH_ARP_DHA_FIELD_ACTION_LEN) >
      (msg->desc.start_of_data + msg_desc->buffer_len)   )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"Length of buffer is not sufficient to add data\r\n");
    return OFU_NO_ROOM_IN_BUFFER;
  }

  set_field = (struct ofp_action_set_field *)(msg_desc->ptr3+msg_desc->length3);
  set_field->type = htons(OFPAT_SET_FIELD); 
  set_field->len  = htons(OFU_SET_FIELD_WITH_ARP_DHA_FIELD_ACTION_LEN + padding_bytes);
  *(uint32_t*)set_field->field   = htonl(OXM_OF_ARP_THA);
  memcpy( (set_field->field + OF_OXM_HDR_LEN), hw_addr, 6);
  memset( (set_field->field + OFU_SET_FIELD_WITH_ARP_DHA_FIELD_ACTION_LEN), 0, padding_bytes);
  msg_desc->length3 += OFU_SET_FIELD_WITH_ARP_DHA_FIELD_ACTION_LEN + padding_bytes;

  return OFU_ACTION_PUSH_SUCCESS;
}


  void
ofu_end_pushing_actions(struct of_msg *msg,
    uint8_t *starting_location,
    uint16_t *length_of_actions)
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  starting_location = msg->desc.ptr3;
  msg->desc.length2 =   msg->desc.length2 + msg->desc.length3 ;
  *length_of_actions =  msg->desc.length2;
  msg->desc.data_len += msg->desc.length2 ;
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg->desc.ptr1 %p length1 %d msg->desc.ptr2 %p msg->desc.length2 %d msgdec.datalen %d ",msg->desc.ptr1, msg->desc.length1, msg->desc.ptr2, msg->desc.length2, msg->desc.data_len);
}

/*########################### Match field Parseing routines #################################*/
  int32_t
ofu_get_in_port_field(struct of_msg *msg,
    void *match_fields,
    uint16_t match_fields_len,
    uint32_t *in_phy_port)
{
  uint32_t          oxm_header;
  uint8_t          *ptr;
  uint16_t          match_field_len;
  uint16_t          parsed_data_len;

  cntlr_assert(match_fields != NULL);

  ptr =  (uint8_t*)match_fields;
  while(match_fields_len)
  {
    oxm_header = ntohl(*(uint32_t*)ptr);

    if(OXM_FIELD(oxm_header) == OFPXMT_OFB_IN_PORT)
    {
      *in_phy_port = ntohl(*(uint32_t*)(ptr+OF_OXM_HDR_LEN));
      return OFU_IN_PORT_FIELD_PRESENT;  
    }
    else
    {
      parsed_data_len = (OF_OXM_HDR_LEN + OXM_LENGTH(oxm_header)); 
      match_field_len -= parsed_data_len;
      ptr             += parsed_data_len;
    }
  }

  return OFU_IN_PORT_FIELD_NOT_PRESENT;
}

int32_t
ofu_get_metadata_field(struct of_msg *msg,
                    void *match_fields,
                    uint16_t match_fields_len,
                    uint64_t *metadata)
{
  uint32_t          oxm_header;
  uint8_t          *ptr;
//  uint16_t          match_field_len;
  uint16_t          parsed_data_len;

  cntlr_assert(match_fields != NULL);

  ptr =  (uint8_t*)match_fields;
  while(match_fields_len)
  {
     oxm_header = ntohl(*(uint32_t*)ptr);

     if(OXM_FIELD(oxm_header) == OFPXMT_OFB_METADATA)
     {
        *metadata = ntohll(*(uint64_t*)(ptr+OF_OXM_HDR_LEN));
        return OFU_IN_META_DATA_PRESENT;  
     }
     else
     {
       parsed_data_len = (OF_OXM_HDR_LEN + OXM_LENGTH(oxm_header)); 
       match_fields_len -= parsed_data_len;
       ptr             += parsed_data_len;
     }
  }

  return OFU_IN_META_DATA_NOT_PRESENT;
}

  void
ofu_start_appending_buckets_to_group(struct of_msg *msg)
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  msg->desc.ptr2    =msg_desc->ptr1 + msg_desc->length1;
  msg->desc.length2 = 0;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg->desc.ptr1 %p length1 %d msg->desc.ptr2 %p msg->desc.length2 %d msgdec.datalen %d ",msg->desc.ptr1, msg->desc.length1, msg->desc.ptr2, msg->desc.length2, msg->desc.data_len);
  return;
}

  int32_t
ofu_append_bucket_to_group_type_all(struct of_msg *msg)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_bucket *bucket;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  msg_desc->ptr2 =msg_desc->ptr2+msg_desc->length2;
  msg_desc->length2=0; /* ptr1 always points to new bucket */
  bucket=(struct ofp_bucket *) (msg_desc->ptr2);
  bucket->len=htons(OFU_GROUP_ACTION_BUCKET_LEN);
  msg->desc.length2+= OFU_GROUP_ACTION_BUCKET_LEN;
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg->desc.ptr1 %p length1 %d msg->desc.ptr2 %p msg->desc.length2 %d msgdec.datalen %d ",msg->desc.ptr1, msg->desc.length1, msg->desc.ptr2, msg->desc.length2, msg->desc.data_len);
  return OFU_APPEND_BUCKET_SUCCESS;
}

  int32_t
ofu_append_bucket_to_group_type_select(struct of_msg *msg,
    uint16_t weight)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_bucket *bucket;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  msg_desc->ptr2 =msg_desc->ptr2+msg_desc->length2;
  msg_desc->length2=0; /* ptr2 always points to new bucket */
  bucket=(struct ofp_bucket *) (msg_desc->ptr2);
  bucket->len=htons(OFU_GROUP_ACTION_BUCKET_LEN);
  bucket->weight=htons(weight);
  msg->desc.length2+=OFU_GROUP_ACTION_BUCKET_LEN;
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg->desc.ptr1 %p length1 %d msg->desc.ptr2 %p msg->desc.length2 %d msgdec.datalen %d ",msg->desc.ptr1, msg->desc.length1, msg->desc.ptr2, msg->desc.length2, msg->desc.data_len);
  return OFU_APPEND_BUCKET_SUCCESS;
}

  int32_t       
ofu_append_bucket_to_group_type_indirect(struct of_msg *msg)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_bucket *bucket;
  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;
  msg_desc->ptr2 =msg_desc->ptr2+msg_desc->length2;
  msg_desc->length2=0; /* ptr1 always points to new bucket */
  bucket=(struct ofp_bucket *) (msg_desc->ptr2);
  bucket->len=htons(OFU_GROUP_ACTION_BUCKET_LEN);
  msg->desc.length2+= OFU_GROUP_ACTION_BUCKET_LEN;
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg->desc.ptr1 %p length1 %d msg->desc.ptr2 %p msg->desc.length2 %d msgdec.datalen %d ",msg->desc.ptr1, msg->desc.length1, msg->desc.ptr2, msg->desc.length2, msg->desc.data_len);     
  return OFU_APPEND_BUCKET_SUCCESS;
}

  int32_t
ofu_append_bucket_to_group_type_fast_failover(struct of_msg *msg,
    uint32_t watch_port,
    uint32_t watch_group)

{
  struct of_msg_descriptor *msg_desc;
  struct ofp_bucket *bucket;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  msg_desc->ptr2 =msg_desc->ptr2+msg_desc->length2;
  msg_desc->length2=0; /* ptr2 always points to new bucket */
  bucket=(struct ofp_bucket *) (msg_desc->ptr2);
  bucket->len=htons(OFU_GROUP_ACTION_BUCKET_LEN);
  bucket->watch_port=htonl(watch_port);
  bucket->watch_group=htonl(watch_group);
  msg->desc.length2+=OFU_GROUP_ACTION_BUCKET_LEN;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg->desc.ptr1 %p length1 %d msg->desc.ptr2 %p msg->desc.length2 %d msgdec.datalen %d ",msg->desc.ptr1, msg->desc.length1, msg->desc.ptr2, msg->desc.length2, msg->desc.data_len);
  return OFU_APPEND_BUCKET_SUCCESS;
}


  void
ofu_end_appending_buckets_to_group(struct of_msg *msg,
    uint8_t *starting_location,
    uint16_t *length_of_bucket)
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  starting_location=msg->desc.ptr2;
  *length_of_bucket = msg->desc.length1 + msg->desc.length2;
  msg->desc.data_len += msg->desc.length1;
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg->desc.ptr1 %p length1 %d msg->desc.ptr2 %p msg->desc.length2 %d msgdec.datalen %d ",msg->desc.ptr1, msg->desc.length1, msg->desc.ptr2, msg->desc.length2, msg->desc.data_len);
}

  void
ofu_start_appending_bands_to_meter(struct of_msg *msg)
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  msg->desc.ptr2    =msg_desc->ptr1 + msg_desc->length1;
  msg->desc.length2 = 0;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg->desc.ptr1 %p length1 %d msg->desc.ptr2 %p msg->desc.length2 %d msgdec.datalen %d ",msg->desc.ptr1, msg->desc.length1, msg->desc.ptr2, msg->desc.length2, msg->desc.data_len);
  return;
}

int32_t ofu_append_band_to_meter_type_drop(struct of_msg *msg,
    uint32_t       band_rate,
    uint32_t       burst_value)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_meter_band_drop *band;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  msg_desc->ptr2 =msg_desc->ptr2+msg_desc->length2;
  msg_desc->length2=0; /* ptr1 always points to new band */
  band=(struct ofp_meter_band_drop *) (msg_desc->ptr2);
  band->type = htons(OFPMBT_DROP); 
  band->rate = htonl(band_rate);
  band->burst_size = htonl(burst_value);
  band->len=htons(OFU_METER_BAND_LEN);
  msg->desc.length2+= OFU_METER_BAND_LEN;
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg->desc.ptr1 %p length1 %d msg->desc.ptr2 %p msg->desc.length2 %d msgdec.datalen %d ",msg->desc.ptr1, msg->desc.length1, msg->desc.ptr2, msg->desc.length2, msg->desc.data_len);
  return OFU_APPEND_BAND_SUCCESS;
}

int32_t ofu_append_band_to_meter_type_dscp_remark(struct of_msg *msg,
    uint32_t       band_rate,
    uint8_t        prec_level,
    uint32_t       burst_value)
{
  struct of_msg_descriptor *msg_desc;
  struct ofp_meter_band_dscp_remark *band;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  msg_desc->ptr2 =msg_desc->ptr2+msg_desc->length2;
  msg_desc->length2=0; /* ptr2 always points to new bucket */
  band=(struct ofp_meter_band_dscp_remark *) (msg_desc->ptr2);
  band->type = htons(OFPMBT_DSCP_REMARK); 
  band->len=htons(OFU_METER_BAND_LEN);
  band->prec_level= prec_level;
  band->rate = htonl(band_rate);
  band->burst_size = htonl(burst_value);
  msg->desc.length2+=OFU_METER_BAND_LEN;
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg->desc.ptr1 %p length1 %d msg->desc.ptr2 %p msg->desc.length2 %d msgdec.datalen %d ",msg->desc.ptr1, msg->desc.length1, msg->desc.ptr2, msg->desc.length2, msg->desc.data_len);
  return OFU_APPEND_BAND_SUCCESS;
}

  void
ofu_end_appending_bands_to_meter(struct of_msg *msg,
    uint8_t *starting_location,
    uint16_t *length_of_band)
{
  struct of_msg_descriptor *msg_desc;

  cntlr_assert(msg != NULL);
  msg_desc = &msg->desc;

  starting_location=msg->desc.ptr2;
  *length_of_band = msg->desc.length1 + msg->desc.length2;
  msg->desc.data_len += msg->desc.length2;
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"msg->desc.ptr1 %p length1 %d msg->desc.ptr2 %p msg->desc.length2 %d msgdec.datalen %d ",msg->desc.ptr1, msg->desc.length1, msg->desc.ptr2, msg->desc.length2, msg->desc.data_len);
}

int32_t of_get_flow_rem_msg_match_fields( struct ofl_flow_removed  *flow_rem_msg,   struct ofl_oxm_match_fields *oxm_match_fields_p, uint64_t *flags_p)
{

   int32_t status=OF_SUCCESS;
   of_match_tlv_t  *tlv;
   struct ofp_match *ofp_match;
   uint32_t length;
   uint32_t field;
   uint32_t oxm_class, tlv_len, match_field_len;
   uint8_t mask;

   OF_LOG_MSG(OF_LOG_MCAST_APP, OF_LOG_DEBUG,"Reading Match fields from Flow Removed Message");

   ofp_match = (struct ofp_match *)(flow_rem_msg->match_fields);

   length = ofp_match->length - offsetof(struct ofp_match, oxm_fields);      /* total length of all TLVs */
   tlv = (of_match_tlv_t *) (ofp_match->oxm_fields); /* first TLV */

   match_field_len=flow_rem_msg->match_field_len;


   while(length > 0)
   {
      field = tlv->oxm_field;           /* Look at field identifier */
      mask = tlv->has_mask;
      oxm_class = tlv->oxm_class;
      OF_LOG_MSG(OF_LOG_MCAST_APP, OF_LOG_DEBUG,"class %x field %d ", oxm_class, field);
      if (oxm_class == OFPXMC_OPENFLOW_BASIC)
      {
         switch (field)
         {
            case OFPXMT_OFB_IN_PORT:
               oxm_match_fields_p->in_port = OXM_GET_DATA_UINT32(tlv->oxm_value);
               *flags_p |= ((uint64_t)(pow(2, OFPXMT_OFB_IN_PORT)));
               break;
            case OFPXMT_OFB_IN_PHY_PORT:
               oxm_match_fields_p->in_phy_port = OXM_GET_DATA_UINT32(tlv->oxm_value);
               *flags_p |= ((uint64_t)(pow(2, OFPXMT_OFB_IN_PHY_PORT)));
               break;
            case OFPXMT_OFB_METADATA:
               oxm_match_fields_p->metadata = OXM_GET_DATA_UINT64(tlv->oxm_value);
               *flags_p |= ((uint64_t)(pow(2, OFPXMT_OFB_METADATA)));
               if (mask)
               {
               }
               break;
            case OFPXMT_OFB_ETH_DST:
               oxm_match_fields_p->eth_dst = OXM_GET_DATA_UINT6(tlv->oxm_value);
               *flags_p |= ((uint64_t)(pow(2, OFPXMT_OFB_ETH_DST)));
               if (mask)
               {
               }
               break;
            case OFPXMT_OFB_ETH_SRC:
               oxm_match_fields_p->eth_dst = OXM_GET_DATA_UINT6(tlv->oxm_value);
               *flags_p |= ((uint64_t)(pow(2, OFPXMT_OFB_ETH_SRC)));
               if (mask)
               {
               }
               else
               {
               }
               break;
            case OFPXMT_OFB_ETH_TYPE:
               oxm_match_fields_p->eth_type = OXM_GET_DATA_UINT16(tlv->oxm_value);
               *flags_p |= ((uint64_t)(pow(2, OFPXMT_OFB_ETH_TYPE)));
               break;
            case OFPXMT_OFB_VLAN_VID:
               oxm_match_fields_p->vlan_vid = OXM_GET_DATA_UINT16(tlv->oxm_value);
               *flags_p |= ((uint64_t)(pow(2, OFPXMT_OFB_VLAN_VID)));
               break;
            case OFPXMT_OFB_VLAN_PCP:
               oxm_match_fields_p->vlan_pcp = OXM_GET_DATA_BYTES1(tlv->oxm_value);
               *flags_p |= ((uint64_t)(pow(2, OFPXMT_OFB_VLAN_PCP)));
               break;
            case OFPXMT_OFB_IP_DSCP:
               oxm_match_fields_p->ip_dscp= OXM_GET_DATA_BYTES1(tlv->oxm_value);
               *flags_p |= ((uint64_t)(pow(2, OFPXMT_OFB_IP_DSCP)));
               break;
            case OFPXMT_OFB_IP_ECN:
               oxm_match_fields_p->ip_ecn = OXM_GET_DATA_BYTE(tlv->oxm_value);
               *flags_p |= ((uint64_t)(pow(2, OFPXMT_OFB_IP_ECN)));
               break;
            case OFPXMT_OFB_IP_PROTO:
               oxm_match_fields_p->ip_proto =OXM_GET_DATA_BYTE(tlv->oxm_value);
               *flags_p |= ((uint64_t)(pow(2, OFPXMT_OFB_IP_PROTO)));
               break;
            case OFPXMT_OFB_IPV4_SRC:
               oxm_match_fields_p->ipv4_src = OXM_GET_DATA_UINT32(tlv->oxm_value);
               *flags_p |= ((uint64_t)(pow(2, OFPXMT_OFB_IPV4_SRC)));
               if (mask)
               {
               }
               break;
            case OFPXMT_OFB_IPV4_DST:
               oxm_match_fields_p->ipv4_dst = OXM_GET_DATA_UINT32(tlv->oxm_value);
               *flags_p |= (((uint64_t)pow(2, OFPXMT_OFB_IPV4_DST)));
               if (mask)
               {
               }
               break;
            case OFPXMT_OFB_TCP_SRC:
               break;
            case OFPXMT_OFB_TCP_DST:
               break;
            case OFPXMT_OFB_UDP_SRC:
               break;
            case OFPXMT_OFB_UDP_DST:
               break;
            case OFPXMT_OFB_ICMPV4_TYPE:
               break;
            case OFPXMT_OFB_ICMPV4_CODE:
               break;
            case OFPXMT_OFB_SCTP_SRC:
               break;
            case OFPXMT_OFB_SCTP_DST:
               break;
            case OFPXMT_OFB_ARP_OP:
               oxm_match_fields_p->arp_op = OXM_GET_DATA_UINT16(tlv->oxm_value);
               *flags_p |= ((uint64_t)(pow(2, OFPXMT_OFB_ARP_OP)));
               break;
            case OFPXMT_OFB_ARP_SPA:
               oxm_match_fields_p->arp_spa = OXM_GET_DATA_UINT32(tlv->oxm_value);
               *flags_p |= ((uint64_t)(pow(2, OFPXMT_OFB_ARP_SPA)));
               break;
            case OFPXMT_OFB_ARP_TPA:
               oxm_match_fields_p->arp_tpa = OXM_GET_DATA_UINT32(tlv->oxm_value);
               *flags_p |= ((uint64_t)(pow(2, OFPXMT_OFB_ARP_TPA)));
               break;
            case OFPXMT_OFB_ARP_SHA:
               oxm_match_fields_p->arp_sha = OXM_GET_DATA_BYTES6(tlv->oxm_value);
               *flags_p |= ((uint64_t)(pow(2, OFPXMT_OFB_ARP_SHA)));
               break;
            case OFPXMT_OFB_ARP_THA:
               oxm_match_fields_p->arp_tha = OXM_GET_DATA_BYTES6(tlv->oxm_value);
               *flags_p |= (((uint64_t)pow(2, OFPXMT_OFB_ARP_THA)));
               break;
            case OFPXMT_OFB_IPV6_SRC:
               break;
            case OFPXMT_OFB_IPV6_DST:
               break;
            case OFPXMT_OFB_IPV6_FLABEL:
               break;
            case OFPXMT_OFB_ICMPV6_TYPE:
               break;
            case OFPXMT_OFB_ICMPV6_CODE:
               break;
            default:
               OF_LOG_MSG(OF_LOG_MCAST_APP, OF_LOG_DEBUG,"Unknown match field %d", field);
               break;
         }
      }
      else if (oxm_class == OFPXMC_EXPERIMENTER)
      {
      }
      else if ((oxm_class == OFPXMC_NXM_1))   
      {
      }
      else
      {
         OF_LOG_MSG(OF_LOG_MCAST_APP, OF_LOG_ERROR,"invalid OXM_CLASS=%x",oxm_class);
      }

      tlv_len = sizeof(*tlv) + tlv->oxm_length;  /* oxm_length is length of value only */
      tlv = (of_match_tlv_t *)  ((uint8_t *)tlv + tlv_len);
      length -= tlv_len;

   } /* while */

   return status;
}

