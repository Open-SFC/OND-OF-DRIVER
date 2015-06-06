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

Modified
Date: Nov,2013
Author : Vivek Sen Reddy K.
Description : Added  parameter checks for the Group Types as specified
		in the OpenFlow spec, Enabled setfield option for action.

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
#include "oflog.h"
#include "of_group.h"
#include "fsl_ext.h"
#include "nicira_ext.h"
#define OF_OXM_HDR_LEN   (4)

void of_group_free_bucket_entry(struct rcu_head *node);

void of_group_free_action_entry(struct rcu_head *node);

int32_t of_register_group(uint64_t datapath_handle, struct ofi_group_desc_info *group_entry_p)
{
  struct   dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;
  uchar8_t offset;
  offset = OF_GROUP_DESC_LISTNODE_OFFSET;

  if(group_entry_p == NULL)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"group_entry_p is NULL");
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
    OF_LIST_INIT(group_entry_p->bucket_list, of_group_free_bucket_entry);
    status=OF_SUCCESS;
    break;
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  if ( status == OF_SUCCESS)
  {	
    OF_APPEND_NODE_TO_LIST(datapath_entry_p->group_table,group_entry_p,offset);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"succesfully added group %d", group_entry_p->group_id); 	
  }	

  return status;
}

int32_t of_update_group(uint64_t datapath_handle, struct ofi_group_desc_info *group_info_p)
{
  struct ofi_group_desc_info *group_entry_p;
  struct   dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;
  uchar8_t offset;
  offset = OF_GROUP_DESC_LISTNODE_OFFSET;

  if (group_info_p == NULL)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"bucket_entry_p is NULL");
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
    OF_LIST_SCAN(datapath_entry_p->group_table, group_entry_p, struct ofi_group_desc_info *,offset)
    {
      if( group_entry_p->group_id == group_info_p->group_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"group found updateing group values");
        group_entry_p->group_type=group_info_p->group_type;
        status=OF_SUCCESS;
        break;

      }
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  return status;
}

int32_t of_get_group(uint64_t datapath_handle,  uint32_t group_id,  struct ofi_group_desc_info **group_entry_pp)
{
  struct ofi_group_desc_info *group_entry_p;
  struct   dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;
  uchar8_t offset;
  offset = OF_GROUP_DESC_LISTNODE_OFFSET;


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
    OF_LIST_SCAN(datapath_entry_p->group_table, group_entry_p, struct ofi_group_desc_info *,offset)
    {
      if( group_entry_p->group_id == group_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"group found ");
        status=OF_SUCCESS;
        break;

      }
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  if (status == OF_SUCCESS )
  {
    *group_entry_pp=group_entry_p;
  }
  return status;
}

int32_t of_register_bucket_to_group(uint64_t datapath_handle, struct ofi_bucket *bucket_entry_p, uint32_t group_id)
{
  struct ofi_group_desc_info *group_entry_p;
  struct   dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;
  uchar8_t offset;
  offset = OF_GROUP_DESC_LISTNODE_OFFSET;

  if (bucket_entry_p == NULL)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"bucket_entry_p is NULL");
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
    OF_LIST_SCAN(datapath_entry_p->group_table, group_entry_p, struct ofi_group_desc_info *,offset)
    {
      if( group_entry_p->group_id == group_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"group found adding bucket to it");
        status=OF_SUCCESS;
        break;

      }
    }
  }while(0);
  if (status == OF_SUCCESS )
  {
    OF_LIST_INIT(bucket_entry_p->action_list, of_group_free_action_entry);
    OF_APPEND_NODE_TO_LIST(group_entry_p->bucket_list, bucket_entry_p,OF_BUCKET_LISTNODE_OFFSET);
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return status;
}

int32_t of_update_bucket_in_group(uint64_t datapath_handle, struct ofi_bucket *bucket_info_p, uint32_t group_id)
{
  struct ofi_bucket *bucket_entry_p;
  struct ofi_group_desc_info *group_entry_p=NULL;
  struct dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;
  uchar8_t offset;
  offset = OF_GROUP_DESC_LISTNODE_OFFSET;


  if(bucket_info_p == NULL)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"bucket_entry_p is NULL");
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

    OF_LIST_SCAN(datapath_entry_p->group_table,group_entry_p, struct ofi_group_desc_info *,offset)
    {
      if( group_entry_p->group_id == group_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"group found");

        OF_LIST_SCAN(group_entry_p->bucket_list, bucket_entry_p, struct ofi_bucket *,OF_BUCKET_LISTNODE_OFFSET)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"bucket  found");
          if( bucket_entry_p->bucket_id == bucket_info_p->bucket_id)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"updating bucket");
            bucket_entry_p->weight=bucket_info_p->weight;
            bucket_entry_p->watch_port=bucket_info_p->watch_port;
            bucket_entry_p->watch_group=bucket_info_p->watch_group;
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

int32_t of_register_action_to_bucket(uint64_t datapath_handle,struct ofi_action *action_p, uint32_t group_id, uint32_t bucket_id)
{
  struct ofi_bucket *bucket_entry_p;
  struct ofi_group_desc_info *group_entry_p=NULL;
  struct dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;
  uchar8_t offset;
  offset = OF_GROUP_DESC_LISTNODE_OFFSET;


  if(action_p == NULL)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"bucket_entry_p is NULL");
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

    OF_LIST_SCAN(datapath_entry_p->group_table,group_entry_p, struct ofi_group_desc_info *,offset)
    {
      if( group_entry_p->group_id == group_id)
      {
//        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"group found");

        OF_LIST_SCAN(group_entry_p->bucket_list, bucket_entry_p, struct ofi_bucket *, OF_BUCKET_LISTNODE_OFFSET)
        {
          if( bucket_entry_p->bucket_id == bucket_id)
          {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"adding action to it bucket");
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

  if (status == OF_SUCCESS)
  {
    OF_APPEND_NODE_TO_LIST(bucket_entry_p->action_list,action_p,OF_ACTION_LISTNODE_OFFSET);

  }

  return status;
}

int32_t of_group_set_action_type(struct ofi_action *action_info,
		char *action_type, uint32_t action_len){

	if (!strncmp(action_type, "output", action_len))
	{
		action_info->type = OFPAT_OUTPUT;
	}
	else if (!strncmp(action_type, "copyTTLout", action_len))
	{
		action_info->type= OFPAT_COPY_TTL_OUT;
	}
	else if (!strncmp(action_type, "copyTTLin", action_len))
	{
		action_info->type= OFPAT_COPY_TTL_IN;
	}
	else if (!strncmp(action_type, "setMPLSTTL", action_len))
	{
		action_info->type= OFPAT_SET_MPLS_TTL;
	}
	else if (!strncmp(action_type, "decMPLSTTL", action_len))
	{
		action_info->type= OFPAT_DEC_MPLS_TTL;
	}
	else if (!strncmp(action_type, "pushVLAN", action_len))
	{
		action_info->type= OFPAT_PUSH_VLAN;
	}
	else if (!strncmp(action_type, "popVLAN", action_len))
	{
		action_info->type= OFPAT_POP_VLAN;
	}
	else if (!strncmp(action_type, "pushMPLS", action_len))
	{
		action_info->type= OFPAT_PUSH_MPLS;
	}
	else if (!strncmp(action_type, "popMPLS", action_len))
	{
		action_info->type= OFPAT_POP_MPLS;
	}
	else if (!strncmp(action_type, "applyGroup", action_len))
	{
		action_info->type= OFPAT_GROUP;
	}  
	else if (!strncmp(action_type, "setTTL", action_len))
	{
		action_info->type= OFPAT_SET_NW_TTL;
	}
	else if (!strncmp(action_type, "decTTL", action_len))
	{
		action_info->type= OFPAT_DEC_NW_TTL;
	}
	else if (!strncmp(action_type, "setQueue", action_len))
	{
		action_info->type= OFPAT_SET_QUEUE;
	}
	else if (!strncmp(action_type, "setField", action_len))
	{
		OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL,"actin type:setfield");
		action_info->type = OFPAT_SET_FIELD;
		action_info->setfield_type = -1;
	}
	else if (!strncmp(action_type, "pushPBB", action_len))
	{
		action_info->type= OFPAT_PUSH_PBB;
	}
	else if (!strncmp(action_type, "popPBB", action_len))
	{
		action_info->type= OFPAT_POP_PBB;
	}
	else
	{
		OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Unknown action type ....");
		return OF_FAILURE;
	}

	return OF_SUCCESS;



}

int32_t of_group_set_action_setfield_type(struct ofi_action *action_info,
		    char *setfield_type, uint32_t setfield_len) 
{                                       
	   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL,"of_group_set_action_setfield_type Entered & set_field_type:%s ",setfield_type);                                     

	   if (!strncmp(setfield_type, "srcIpV4Addr", setfield_len))
	   {                             
		   action_info->setfield_type = OFPXMT_OFB_IPV4_SRC;
	   }                             
	   else if (!strncmp(setfield_type, "dstIpV4Addr", setfield_len))
	   {                                     
		   action_info->setfield_type = OFPXMT_OFB_IPV4_DST;
	   }                                             
	   else if (!strncmp(setfield_type, "udpSrcPort", setfield_len))
	   {                                     
		   action_info->setfield_type = OFPXMT_OFB_UDP_SRC;
	   }                                       
	   else if (!strncmp(setfield_type, "udpDstPort", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_UDP_DST;
	   }
	   else if (!strncmp(setfield_type, "tcpSrcPort", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_TCP_SRC;
	   }
	   else if (!strncmp(setfield_type, "tcpDstPort", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_TCP_DST;
	   }
	   else if (!strncmp(setfield_type, "sctpSrcPort", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_SCTP_SRC;
	   }
	   else if (!strncmp(setfield_type, "sctpDstPort", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_SCTP_DST;
	   }
	   else if (!strncmp(setfield_type, "protocol", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_IP_PROTO;
	   }
	   else if (!strncmp(setfield_type, "ethType", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_ETH_TYPE;
	   }
	   else if (!strncmp(setfield_type, "srcMacAddr", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_ETH_SRC;
	   }
	   else if (!strncmp(setfield_type, "dstMacAddr", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_ETH_DST;
	   }
	   else if (!strncmp(setfield_type, "icmpType", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_ICMPV4_TYPE;
	   }
	   else if (!strncmp(setfield_type, "icmpCode", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_ICMPV4_CODE;
	   }
	   else if (!strncmp(setfield_type, "inPort", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_IN_PORT;
	   }
	   else if (!strncmp(setfield_type, "inPhyPort", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_IN_PHY_PORT;
	   }
	   else if (!strncmp(setfield_type, "tableMetaData", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_METADATA;
	   }
	   else if (!strncmp(setfield_type, "mplsLabel", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_MPLS_LABEL;
	   }
	   else if (!strncmp(setfield_type, "mplsTC", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_MPLS_TC;
	   }
	   else if (!strncmp(setfield_type, "mplsBos", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_MPLS_BOS;
	   }
	   else if (!strncmp(setfield_type, "pbbIsid", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_PBB_ISID;
	   }
	   else if (!strncmp(setfield_type, "tunnelId", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_TUNNEL_ID;
	   }
	   else if (!strncmp(setfield_type, "VlanId", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_VLAN_VID;
	   }
	   else if (!strncmp(setfield_type, "VlanPriority", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_VLAN_PCP;
	   }
	   else if (!strncmp(setfield_type, "arpSrcIpv4Addr", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_ARP_SPA;
	   }
	   else if (!strncmp(setfield_type, "arpDstIpv4Addr", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_ARP_TPA;
	   }
	   else if (!strncmp(setfield_type, "arpSrcMacAddr", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_ARP_SHA;
	   }
	   else if (!strncmp(setfield_type, "arpDstMacAddr", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_ARP_THA;
	   }
	   else if (!strncmp(setfield_type, "arpOpcode", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_ARP_OP;
	   }
	   else if (!strncmp(setfield_type, "IpDiffServCodePointBits", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_IP_DSCP;
	   }
	   else if (!strncmp(setfield_type, "IpECNBits", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_IP_ECN;
	   }
	   else if (!strncmp(setfield_type, "srcIpv6Addr", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_IPV6_SRC;
	   }
	   else if (!strncmp(setfield_type, "dstIpv6Addr", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_IPV6_DST;
	   }
	   else if (!strncmp(setfield_type, "icmpv6Type", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_ICMPV6_TYPE;
	   }
	   else if (!strncmp(setfield_type, "icmpv6Code", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_ICMPV6_CODE;
	   }
	   else if (!strncmp(setfield_type, "ipv6NDTarget", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_IPV6_ND_TARGET;
	   }
	   else if (!strncmp(setfield_type, "ipv6NDSrcLinkLayerAddr", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_IPV6_ND_SLL;
	   }
	   else if (!strncmp(setfield_type, "ipv6NDTargetLinkLayerAddr", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_IPV6_ND_TLL;
	   }
	   else if (!strncmp(setfield_type, "Ipv6flowlabel", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_IPV6_FLABEL;
	   }
	   else if (!strncmp(setfield_type, "ipv6ExtnHdrPseudofield", setfield_len))
	   {
		   action_info->setfield_type = OFPXMT_OFB_IPV6_EXTHDR;
	   }
	   else
	   {
		   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Invalide set field type ....");
		   return OF_FAILURE;
	   }
	   return OF_SUCCESS;
}

int32_t of_group_set_action_setfieldtype_value(struct ofi_action *action_info,
		char *data, uint32_t data_len)
{
	int32_t retval = OF_SUCCESS;
	uint32_t field_type = action_info->setfield_type;
	OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL," of_flow_set_action_setfieldtype_value Entered & setfieldtype_value:%s",data);

	do
	{
		switch (field_type)
		{
			case OFPXMT_OFB_IN_PORT: /* Switch input port. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," OFPXMT_OFB_IN_PORT:");
					action_info->ui32_data = (uint32_t)atoi(data);
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "INPORT=%d",action_info->ui32_data);
					break;
				}
			case OFPXMT_OFB_IN_PHY_PORT: /* Switch physical input port. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IN_PHY_PORT:");
					action_info->ui32_data = (uint32_t)atoi(data);
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "INPhy-port=%d",action_info->ui32_data);

					break;
				}
			case OFPXMT_OFB_METADATA: /* Metadata passed between tables. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_METADATA:");
					if ((*data == '0') && ((*(data+1) == 'x') || (*(data+1) == 'X')))
					{
						action_info->ui64_data = atox_64(data+2);
					}
					else
					{
						action_info->ui64_data = (uint64_t)atoll(data);
					}
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "metadata=%llx",action_info->ui64_data);
					break;
				}
			case OFPXMT_OFB_ETH_DST: /* Ethernet destination address. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ETH_DST:");
					if ((*data == '0') && ((*(data+1) == 'x') || (*(data+1) == 'X')))
					{
						of_flow_match_atox_mac_addr(data+2, action_info->ui8_data_array);
					}
					else
					{
						of_flow_match_atox_mac_addr(data, action_info->ui8_data_array);
					}
					break;
				}
			case OFPXMT_OFB_ETH_SRC: /* Ethernet source address. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ETH_SRC: ");
					if ((*data == '0') &&
							((*(data+1) == 'x') || (*(data+1) == 'X')))
					{
						of_flow_match_atox_mac_addr(data+2, action_info->ui8_data_array);
					}
					else
					{
						of_flow_match_atox_mac_addr(data, action_info->ui8_data_array);
					}
					break;
				}
			case OFPXMT_OFB_ETH_TYPE: /* Ethernet frame type. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ETH_TYPE: ");
					if ((*data == '0') &&
							((*(data+1) == 'x') || (*(data+1) == 'X')))
					{
						action_info->ui16_data = (uint16_t)atox_32(data+2);
					}
					else
					{
						action_info->ui16_data = (uint16_t)atoi(data);
					}
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"ethtype=%x",action_info->ui16_data);
					break;
				}
			case OFPXMT_OFB_VLAN_VID: /* VLAN id. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_VLAN_VID: ");
					action_info->ui16_data = (uint16_t)atoi(data);
					break;
				}
			case OFPXMT_OFB_IP_PROTO: /* IP protocol. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IP_PROTO: ");
					action_info->ui8_data = (uint8_t)atoi(data);
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "protocol=%d",action_info->ui8_data);
					break;
				}
			case OFPXMT_OFB_IPV4_SRC: /* IPv4 source address. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV4_SRC: ");
					action_info->ui32_data = (uint32_t)inet_network(data);
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"src ip=%x",action_info->ui32_data);
					break;
				}
			case OFPXMT_OFB_IPV4_DST: /* IPv4 destination address. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV4_DST: ");
					action_info->ui32_data = (uint32_t)inet_network(data);
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "dest-ip=%x",action_info->ui32_data);
					break;
				}
			case OFPXMT_OFB_TCP_SRC: /* TCP source port. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_TCP_SRC: ");
					action_info->ui16_data = (uint16_t)atoi(data);
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "tcp srcport=%d",action_info->ui16_data);
					break;
				}
			case OFPXMT_OFB_TCP_DST: /* TCP destination port. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_TCP_DST: ");
					action_info->ui16_data = (uint16_t)atoi(data);
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "tcp destport=%d",action_info->ui16_data);
					break;
				}
			case OFPXMT_OFB_UDP_SRC: /* UDP source port. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_UDP_SRC: ");
					action_info->ui16_data = (uint16_t)atoi(data);

					break;
				}
			case OFPXMT_OFB_UDP_DST: /* UDP destination port. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_UDP_DST: ");
					action_info->ui16_data = (uint16_t)atoi(data);

					break;
				}
			case OFPXMT_OFB_SCTP_SRC: /* SCTP source port. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_SCTP_SRC: ");
					action_info->ui16_data = (uint16_t)atoi(data);

					break;
				}
			case OFPXMT_OFB_SCTP_DST: /* SCTP destination port. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_SCTP_DST: ");
					action_info->ui16_data = (uint16_t)atoi(data);

					break;
				}
			case OFPXMT_OFB_ICMPV4_TYPE: /* ICMP type. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV4_TYPE:");
					action_info->ui8_data = (uint8_t)atoi(data);

					break;
				}
			case OFPXMT_OFB_ICMPV4_CODE: /* ICMP code. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV4_CODE: ");
					action_info->ui8_data = (uint8_t)atoi(data);
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "icmp-code=%d",action_info->ui8_data);
					break;
				}
			case OFPXMT_OFB_ARP_OP: /* ARP opcode. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_OP: ");
					action_info->ui16_data = (uint16_t)atoi(data);
					break;
				}
			case OFPXMT_OFB_MPLS_LABEL: /* MPLS label. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_MPLS_LABEL:");
					action_info->ui32_data = (uint32_t)atoi(data);
					break;
				}
			case OFPXMT_OFB_MPLS_TC: /* MPLS TC. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_MPLS_TC:");
					action_info->ui8_data = (uint8_t)atoi(data);
					break;
				}
			case OFPXMT_OFB_MPLS_BOS: /* MPLS BoS bit. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_MPLS_BOS:");
					action_info->ui8_data = (uint8_t)atoi(data);
					break;
				}
			case OFPXMT_OFB_PBB_ISID: /* PBB I-SID. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_PBB_ISID:");
					action_info->ui8_data = (uint8_t)atoi(data);
					break;
				}
			case OFPXMT_OFB_TUNNEL_ID: /* Logical Port Metadata. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_TUNNEL_ID:");
					action_info->ui64_data = (uint64_t)atoll(data);
					break;
				}
			case OFPXMT_OFB_VLAN_PCP:
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_VLAN_PCP");
					action_info->ui8_data = (uint8_t)atoi(data);
					break;
				}
			case OFPXMT_OFB_ARP_SPA: /* IPv4 source address. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_SPA: ");
					action_info->ui32_data = (uint32_t)inet_network(data);
					break;
				}
			case OFPXMT_OFB_ARP_TPA: /* IPv4 source address. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_TPA: ");
					action_info->ui32_data = (uint32_t)inet_network(data);
					break;
				}
			case OFPXMT_OFB_ARP_SHA:
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_ARP_SHA: ");
					if ((*data == '0') &&
							((*(data+1) == 'x') || (*(data+1) == 'X')))
					{
						of_flow_match_atox_mac_addr(data+2, action_info->ui8_data_array);
					}
					else
					{
						of_flow_match_atox_mac_addr(data, action_info->ui8_data_array);
					}
					break;
				}
			case OFPXMT_OFB_ARP_THA:
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_ARP_THA: ");
					if ((*data == '0') &&
							((*(data+1) == 'x') || (*(data+1) == 'X')))
					{
						of_flow_match_atox_mac_addr(data+2, action_info->ui8_data_array);
					}
					else
					{
						of_flow_match_atox_mac_addr(data, action_info->ui8_data_array);
					}
					break;
				}
			case OFPXMT_OFB_IP_DSCP:
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IP_DSCP: ");
					action_info->ui8_data = (uint8_t)atoi(data);
					break;
				}
			case OFPXMT_OFB_IP_ECN:
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IP_ECN: ");
					action_info->ui8_data = (uint8_t)atoi(data);
					break;
				}
			case OFPXMT_OFB_IPV6_SRC:
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_SRC");
					if(inet_pton(AF_INET6, data, &(action_info->ipv6_addr.ipv6Addr32)) != 1)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Invalid Ipv6 src address!. ");
						return OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_IPV6_DST:
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_DST");
					if(inet_pton(AF_INET6, data, &(action_info->ipv6_addr.ipv6Addr32)) != 1)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Invalid Ipv6 dst address!. ");
						return OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_IPV6_FLABEL:
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_FLABEL ");
					if ((*data == '0') &&
							((*(data+1) == 'x') || (*(data+1) == 'X')))
					{
						action_info->ui32_data = (uint32_t)atox_32(data+2);
					}
					else
					{
						action_info->ui32_data = (uint32_t)atoi(data);
					}
					break;
				}
			case OFPXMT_OFB_ICMPV6_TYPE: /* ICMP type. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV6_TYPE:");
					if ((*data == '0') &&
							((*(data+1) == 'x') || (*(data+1) == 'X')))
					{
						action_info->ui8_data = (uint8_t)atox_32(data+2);
					}
					else
					{
						action_info->ui8_data = (uint8_t)atoi(data);
					}
					break;
				}
			case OFPXMT_OFB_ICMPV6_CODE: /* ICMP code. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV6_CODE: ");
					if ((*data == '0') &&
							((*(data+1) == 'x') || (*(data+1) == 'X')))
					{
						action_info->ui8_data = (uint8_t)atox_32(data+2);
					}
					else
					{
						action_info->ui8_data = (uint8_t)atoi(data);
					}
					action_info->ui8_data = (uint8_t)atoi(data);
					break;
				}
			case OFPXMT_OFB_IPV6_ND_TARGET:
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_ND_TARGET: ");

					if(inet_pton(AF_INET6, data, &(action_info->ipv6_addr.ipv6Addr32)) != 1)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Invalid Ipv6  address!. ");
					}
					break;
				}
			case OFPXMT_OFB_IPV6_ND_SLL: /* The source link-layer address option in an IPv6Neighbor Discovery message.*/                    
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_ND_SLL:");
					if ((*data == '0') && ((*(data+1) == 'x') || (*(data+1) == 'X')))
					{
						of_flow_match_atox_mac_addr(data+2, action_info->ui8_data_array);
					}
					else
					{
						of_flow_match_atox_mac_addr(data, action_info->ui8_data_array);
					}
					break;
				}
			case OFPXMT_OFB_IPV6_ND_TLL: /* The target link-layer address option in an IPv6Neighbor Discovery message.*/
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," OFPXMT_OFB_IPV6_ND_TLL: ");
					if ((*data == '0') &&
							((*(data+1) == 'x') || (*(data+1) == 'X')))
					{
						of_flow_match_atox_mac_addr(data+2, action_info->ui8_data_array);
					}
					else
					{
						of_flow_match_atox_mac_addr(data, action_info->ui8_data_array);
					}
					break;
				}
			case OFPXMT_OFB_IPV6_EXTHDR:
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_EXTHDR: ");
					if ((*data == '0') &&
							((*(data+1) == 'x') || (*(data+1) == 'X')))
					{
						action_info->ui16_data = (uint16_t)atox_32(data+2);
					}
					else
					{
						action_info->ui16_data = (uint16_t)atoi(data);
					}
					break;
				}
		}
	}
	while(0);

	return retval;
}


int32_t of_unregister_group(uint64_t datapath_handle, uint32_t group_id)
{
  struct ofi_group_desc_info *group_entry_p, *prev_group_entry_p=NULL;
  struct ofi_action *action_p;
  struct ofi_bucket *bucket_entry_p;
  struct dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;
  uchar8_t offset;
  offset = OF_GROUP_DESC_LISTNODE_OFFSET;

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle( datapath_handle,&datapath_entry_p);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"datapath entry not found");
      break;
    }

    OF_LIST_SCAN(datapath_entry_p->group_table,group_entry_p, struct ofi_group_desc_info *,offset)
    {
      if( group_entry_p->group_id == group_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"group found");
        OF_LIST_REMOVE_HEAD_AND_SCAN(group_entry_p->bucket_list, bucket_entry_p,struct ofi_bucket *,OF_BUCKET_LISTNODE_OFFSET)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"deleting bucket");
          OF_LIST_REMOVE_HEAD_AND_SCAN(bucket_entry_p->action_list,action_p,struct ofi_action *,OF_ACTION_LISTNODE_OFFSET);
        }
        status=OF_SUCCESS;
        break;
      }
      prev_group_entry_p=group_entry_p;
    }

  }while(0);

  if ( status == OF_SUCCESS)
  {
    OF_REMOVE_NODE_FROM_LIST(datapath_entry_p->group_table,group_entry_p, prev_group_entry_p,offset);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"succesfully deleted group");

  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return status;
}

int32_t of_group_unregister_bucket(uint64_t datapath_handle, uint32_t group_id, uint32_t bucket_id)
{

  struct ofi_group_desc_info *group_entry_p=NULL;
  struct ofi_action *action_p;
  struct ofi_bucket *bucket_entry_p, *prev_bucket_entry_p=NULL;
  struct dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;
  uchar8_t offset;
  offset = OF_GROUP_DESC_LISTNODE_OFFSET;

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle( datapath_handle,&datapath_entry_p);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"datapath entry not found");
      break;
    }

    OF_LIST_SCAN(datapath_entry_p->group_table,group_entry_p, struct ofi_group_desc_info *,offset)
    {
      if( group_entry_p->group_id == group_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"group found");
        OF_LIST_SCAN(group_entry_p->bucket_list, bucket_entry_p,struct ofi_bucket *,OF_BUCKET_LISTNODE_OFFSET)
        {
          if (bucket_entry_p->bucket_id == bucket_id)
          {
            OF_LIST_REMOVE_HEAD_AND_SCAN(bucket_entry_p->action_list,action_p,struct ofi_action *,OF_ACTION_LISTNODE_OFFSET);
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"deleting bucket %d", bucket_id);
            status=OF_SUCCESS;
            break;
          }
          prev_bucket_entry_p=bucket_entry_p;
        }
      }
      if ( status == OF_SUCCESS)
        break;
    }
  }while(0);

  if ( status == OF_SUCCESS)
  {
    OF_REMOVE_NODE_FROM_LIST(group_entry_p->bucket_list,bucket_entry_p, prev_bucket_entry_p,OF_BUCKET_LISTNODE_OFFSET);
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return status;
}

int32_t of_group_unregister_buckets(uint64_t datapath_handle, uint32_t group_id)
{

  struct ofi_group_desc_info *group_entry_p =NULL;
  struct ofi_action *action_p;
  struct ofi_bucket *bucket_entry_p;
  struct dprm_datapath_entry *datapath_entry_p=NULL;
  int32_t status=OF_FAILURE;
  int32_t retval;
  uchar8_t offset;
  offset = OF_GROUP_DESC_LISTNODE_OFFSET;

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    retval= get_datapath_byhandle( datapath_handle,&datapath_entry_p);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"datapath entry not found");
      break;
    }

    OF_LIST_SCAN(datapath_entry_p->group_table,group_entry_p, struct ofi_group_desc_info *,offset)
    {
      if( group_entry_p->group_id == group_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"group found");
        OF_LIST_REMOVE_HEAD_AND_SCAN(group_entry_p->bucket_list, bucket_entry_p,struct ofi_bucket *,OF_BUCKET_LISTNODE_OFFSET)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"deleting bucket");
          OF_LIST_REMOVE_HEAD_AND_SCAN(bucket_entry_p->action_list,action_p,struct ofi_action *, OF_ACTION_LISTNODE_OFFSET);
        }
	group_entry_p->max_bucket_id=0;
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"succesfully deleted all buckets");
        status=OF_SUCCESS;
        CNTLR_RCU_READ_LOCK_RELEASE();
        return status;
      }
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  return status;

}

int32_t of_group_frame_action_request(struct of_msg *msg, struct ofi_action  *action_p)
{

  int32_t retval;
  int32_t status=OF_SUCCESS;

  switch(action_p->type)
  {
    case 	OFPAT_OUTPUT:  /* Output to switch port. */
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_OUTPUT");
      retval=ofu_push_output_action(msg,action_p->port_no, action_p->max_len);
      if (retval != OFU_ACTION_PUSH_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"ofu_push_output_action failed");
        status=OF_FAILURE;
        break;
      }
      break;
    case   OFPAT_COPY_TTL_OUT: /* Copy TTL "outwards" -- from next-to-outermost    to outermost */
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_COPY_TTL_OUT");
      retval=
        ofu_push_copy_ttl_outward_action(msg);
      if (retval != OFU_ACTION_PUSH_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"ofu_push_copy_ttl_outward_action failed");
        status=OF_FAILURE;
        break;
      }
      break;
    case   OFPAT_COPY_TTL_IN  : /* Copy TTL "inwards" -- from outermost to    next-to-outermost */
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_COPY_TTL_IN:");
      retval=ofu_push_copy_ttl_inward_action(msg);
      if (retval != OFU_ACTION_PUSH_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"fu_push_inward_action failed");
        status=OF_FAILURE;
        break;
      }
      break;
    case   OFPAT_SET_MPLS_TTL : /* MPLS TTL */
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_SET_MPLS_TTL");
      retval=
        ofu_push_set_mpls_ttl_action(msg, action_p->ttl);
      if (retval != OFU_ACTION_PUSH_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"ofu_push_set_mpls_ttl_action failed");
        status=OF_FAILURE;
        break;
      }
      break;
    case   OFPAT_DEC_MPLS_TTL : /* Decrement MPLS TTL */
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_DEC_MPLS_TTL");
      retval=
        ofu_push_dec_mpls_ttl_action(msg);
      if (retval != OFU_ACTION_PUSH_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"ofu_push_dec_mpls_ttl_action failed");
        status=OF_FAILURE;
        break;
      }
      break;
    case   OFPAT_PUSH_VLAN  : /* Push a new VLAN tag */
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_PUSH_VLAN");

      retval=ofu_push_push_vlan_header_action(msg,action_p->ether_type);
      if (retval != OFU_ACTION_PUSH_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"ofu_push_push_vlan_header_action failed");
        status=OF_FAILURE;
        break;
      }
      break;
    case   OFPAT_POP_VLAN : /* Pop the outer VLAN tag */
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_POP_MPLS:");

      retval=ofu_push_pop_vlan_header_action(msg);
      if (retval != OFU_ACTION_PUSH_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"ofu_push_pop_mpls_header_action failed");
        status=OF_FAILURE;
        break;
      }
      break;
    case   OFPAT_PUSH_MPLS: /* Push a new MPLS tag */
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_PUSH_MPLS:");

      retval=
        ofu_push_push_mpls_header_action(msg, action_p->ether_type);
      if (retval != OFU_ACTION_PUSH_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"ofu_push_push_mpls_header_action failed");
        status=OF_FAILURE;
        break;
      }
      break;
    case   OFPAT_POP_MPLS : /* Pop the outer MPLS tag */
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_POP_MPLS:");
      retval=
        ofu_push_pop_mpls_header_action(msg, action_p->ether_type);
      if (retval != OFU_ACTION_PUSH_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"ofu_push_pop_mpls_header_action failed");
        status=OF_FAILURE;
        break;
      }
      break;
    case   OFPAT_SET_QUEUE: /* Set queue id when outputting to a port */
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_SET_QUE:");
      retval=ofu_push_set_queue_action(msg,action_p->queue_id);
      if (retval != OFU_ACTION_PUSH_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"fu_push_queue_action failed");
        status=OF_FAILURE;
        break;
      }
      break;
    case   OFPAT_GROUP : /* Apply group. */
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_GROUP:");
      retval=ofu_push_group_action(msg, action_p->group_id);
      if (retval != OFU_ACTION_PUSH_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"fu_push_group_action failed");
        status=OF_FAILURE;
        break;
      }
      break;
    case   OFPAT_SET_NW_TTL : /* IP TTL. */
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," OFPAT_SET_NW_TTL:");
      retval=
        ofu_push_set_ipv4_ttl_action(msg,action_p->ttl);
      if (retval != OFU_ACTION_PUSH_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"ofu_push_set_ipv4_ttl_action failed");
        status=OF_FAILURE;
        break;
      }

      break;
    case   OFPAT_DEC_NW_TTL: /* Decrement IP TTL. */
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," OFPAT_DEC_NW_TTL:");
      retval=ofu_push_dec_ipv4_ttl_action(msg);
      if (retval != OFU_ACTION_PUSH_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"ofu_push_dec_ipv4_ttl_action failed");
        status=OF_FAILURE;
        break;
      }
      break;

    case   OFPAT_SET_FIELD: /* Set a header field using OXM TLV format. */
      //Added support for setfield
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," OFPAT_SET_FIELD:");
      retval = of_flow_set_action_setfield(msg,action_p);
      if (retval != OFU_ACTION_PUSH_SUCCESS)
      {
	      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," of_flow_set_action_setfield failed!");
	      status=OF_FAILURE;
	      break;
      }
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s(%d) msg->desc.length3:%d",__FUNCTION__,__LINE__,msg->desc.length3);
      break;

    case   OFPAT_PUSH_PBB: /* Push a new PBB service tag (I-TAG) */
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_PUSH_PBB:");
      retval=
        ofu_push_push_pbb_header_action(msg,action_p->ether_type);

      if (retval != OFU_ACTION_PUSH_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"ofu_push_push_pbb_header_action failed");
        status=OF_FAILURE;
        break;
      }
      break;
    case   OFPAT_POP_PBB: /* Pop the outer PBB service tag (I-TAG) */
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_POP_PBB:");
      retval=
        ofu_push_pop_pbb_header_action(msg);

      if (retval != OFU_ACTION_PUSH_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"ofu_push_pop_pbb_header_action failed");
        status=OF_FAILURE;
        break;
      }
      break;
    case OFPAT_EXPERIMENTER:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"experiment action message");
        switch(action_p->sub_type)
        {
#ifdef FSLX_COMMON_AND_L3_EXT_SUPPORT
          case FSLXAT_WRITE_META_DATA:
            {
              retval=fslx_action_write_metadata(msg,action_p->ui64_data);

              if (retval != OFU_ACTION_PUSH_SUCCESS)
              {
                OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"fslx_action_write_metadata failed");
                status=OF_FAILURE;
                break;
              }
            }
            break;
          case FSLXAT_IP_FRAGMENT:
            {
              retval=fslx_action_ipfragment(msg,action_p->ui32_data);
              if (retval != OFU_ACTION_PUSH_SUCCESS)
              {
                OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"fslx_action_ipfragment failed");
                status=OF_FAILURE;
                break;
              }
            }
	    break;
	  case FSLXAT_SEND_ICMPV4_TIME_EXCEED:
	    {
		    retval = fslx_action_send_icmpv4_time_exceed_err_msg(msg, action_p->ui8_data);
		    if (retval != OFU_ACTION_PUSH_SUCCESS)
		    {
			    OF_LOG_MSG(OF_LOG_FWD_NF, OF_LOG_ERROR,"Error in sending icmp time exceed action  err = %d",retval);
			    status = OF_FAILURE;
			    break;
		    }

	    }
	    break;
	  case NXAST_REG_LOAD:
	    {
		    retval = nicira_action_write_register(msg, action_p->ui32_data, (uint64_t) action_p->port_no, action_p->ui8_data,(uint8_t)action_p->ui16_data );
		    if(retval != OFU_SET_FIELD_SUCCESS)
		    {
			    OF_LOG_MSG(OF_LOG_FWD_NF, OF_LOG_ERROR,"action nicira register 0 write data failed,err = %d",retval);
			    status = OF_FAILURE;
			    break;
		    }

	    }
	    break;
	  case FSLXAT_DROP_PACKET:
	    {
		    retval = fslx_action_drop_packet(msg);
		    if(retval != OFU_ACTION_PUSH_SUCCESS)
		    {
			    OF_LOG_MSG(OF_LOG_FWD_NF, OF_LOG_ERROR,"action drop packet failed,err = %d",retval);
			    status = OF_FAILURE;
			    break;
		    }
	    }
	    break;
#endif
	 default:
	    {
		    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"unknown freescale extention");
		    status=OF_FAILURE;
		    break;
	    }
	}
      }
      break;
  }
  return status;
}

int32_t of_group_set_action_setfield(struct of_msg *msg,
		    struct ofi_action *action)
{
	int32_t retval ;
	OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ALL," of_flow_set_action_setfield Entered .setfield_type:%d",action->setfield_type);
	do
	{
		switch (action->setfield_type)
		{
			case OFPXMT_OFB_IN_PORT:  /* Switch input port. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," OFPXMT_OFB_IN_PORT:");
					retval = ofu_push_set_port_in_set_field_action(msg, &action->ui32_data);
					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action field failed...");
						retval = OF_FAILURE;
					}
					break;
				}

			case OFPXMT_OFB_IN_PHY_PORT: /* Switch physical input port. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IN_PHY_PORT:");
					retval = ofu_push_set_in_physical_port_in_set_field_action(msg, &action->ui32_data);            
					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_ETH_DST: /* Ethernet destination address. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ETH_DST:");
					retval = ofu_push_set_destination_mac_in_set_field_action(msg, action->ui8_data_array);

					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting dst mac match field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_ETH_SRC: /* Ethernet source address. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ETH_SRC: ");
					retval = ofu_push_set_source_mac_in_set_field_action(msg, action->ui8_data_array);

					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting src mac action field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_ETH_TYPE: /* Ethernet frame type. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ETH_TYPE: ");
					retval = ofu_push_set_eth_type_in_set_field_action(msg, &action->ui16_data);

					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting ethtype action field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			 case OFPXMT_OFB_VLAN_VID: /* VLAN id. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_VLAN_VID: ");
					retval = ofu_push_set_vlan_id_in_set_field_action(msg, &action->ui16_data);

					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Setting  action failed...");
						retval = OF_FAILURE;
					}
					break;
				}

			 case OFPXMT_OFB_IP_PROTO: /* IP protocol. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IP_PROTO: ");
					retval = ofu_push_set_ip_protocol_in_set_field_action(msg, &action->ui8_data);

					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			 case OFPXMT_OFB_IPV4_SRC: /* IPv4 source address. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV4_SRC: ");
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s(%d) msg->desc.length3:%d",__FUNCTION__,__LINE__,msg->desc.length3);
					retval = ofu_push_set_ipv4_src_addr_in_set_field_action(msg, &action->ui32_data);
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s(%d) msg->desc.length3:%d",__FUNCTION__,__LINE__,msg->desc.length3);

					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Setting action set  field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			 case OFPXMT_OFB_IPV4_DST: /* IPv4 destination address. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV4_DST: ");
					retval = ofu_push_set_ipv4_dst_addr_in_set_field_action(msg, &action->ui32_data);

					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Setting action set  field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_TCP_SRC: /* TCP source port. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_TCP_SRC: ");
					retval = ofu_push_set_tcp_src_port_in_set_field_action(msg, &action->ui16_data);

					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action set  field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_TCP_DST: /* TCP destination port. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_TCP_DST: ");
					retval = ofu_push_set_tcp_dst_port_in_set_field_action(msg, &action->ui16_data);

					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action set  field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_UDP_SRC: /* UDP source port. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_UDP_SRC: ");
					retval = ofu_push_set_udp_src_port_in_set_field_action(msg, &action->ui16_data);

					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action set field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_UDP_DST: /* UDP destination port. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_UDP_DST: ");
					retval = ofu_push_set_udp_dst_port_in_set_field_action(msg, &action->ui16_data);

					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action set field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_SCTP_SRC: /* SCTP source port. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_SCTP_SRC: ");
					retval = ofu_push_set_sctp_src_port_in_set_field_action(msg,&action->ui16_data);

					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action set  field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_SCTP_DST: /* SCTP destination port. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_SCTP_DST: ");
					retval = ofu_push_set_sctp_dst_port_in_set_field_action(msg,&action->ui16_data);

					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action set  field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_ICMPV4_TYPE: /* ICMP type. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV4_TYPE:");
					retval = ofu_push_set_icmpv4_type_in_set_field_action(msg, &action->ui8_data);

					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action set  field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_ICMPV4_CODE: /* ICMP code. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV4_CODE: ");
					retval = ofu_push_set_icmpv4_code_in_set_field_action(msg, &action->ui8_data);

					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Setting action set  field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_ARP_OP: /* ARP opcode. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_OP: ");
					retval = ofu_push_set_arp_opcode_in_set_field_action(msg, &action->ui16_data);

					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Setting action set  field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_MPLS_LABEL: /* MPLS label. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_MPLS_LABEL: ");
					retval = ofu_push_set_mpls_label_in_set_field_action(msg, &action->ui32_data);
					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action set  field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_MPLS_TC: /* MPLS TC. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_MPLS_TC: ");
					retval = ofu_push_set_mpls_tc_in_set_field_action(msg, &action->ui8_data);
					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action set  field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_MPLS_BOS: /* MPLS BoS bit. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_MPLS_BOS: ");
					retval = ofu_push_set_mpls_bos_bits_in_set_field_action(msg, &action->ui8_data);
					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action set  field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_PBB_ISID: /* PBB I-SID. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_PBB_ISID:");
					retval = ofu_push_set_pbb_isid_in_set_field_action(msg, &action->ui8_data);
					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting action set  field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_TUNNEL_ID: /* Logical Port Metadata. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_TUNNEL_ID:");
					retval = ofu_push_set_logical_port_meta_data_in_set_field_action(msg, &action->ui64_data);
					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, " Setting action set  field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case  OFPXMT_OFB_VLAN_PCP:
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_VLAN_PCP:");
					retval = ofu_push_set_vlan_priority_bits_in_set_field_action(msg, &action->ui64_data);
					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, " Setting action set  field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			 case OFPXMT_OFB_ARP_SPA: /* ARP source IPv4 address. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_SPA");
					retval = ofu_push_set_arp_source_ipv4_addr_in_set_field_action(msg, &action->ui32_data);
					if (retval != OFU_SET_FIELD_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting arp source ipv4 action field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			 case OFPXMT_OFB_ARP_TPA: /* ARP target IPv4 address. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_TPA:");
					retval = ofu_push_set_arp_target_ipv4_addr_in_set_field_action(msg, &action->ui32_data);
					if (retval != OFU_SET_FIELD_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting arp dst ipv4 action field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			 case OFPXMT_OFB_ARP_SHA: /* ARP source hardware address. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_SHA:");
					retval = ofu_push_set_arp_source_hw_addr_in_set_field_action(msg, action->ui8_data_array);
					if (retval != OFU_SET_FIELD_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting arp src mac action field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_ARP_THA: /* ARP target hardware address. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_THA:");
					retval = ofu_push_set_arp_target_hw_addr_in_set_field_action(msg, action->ui8_data_array);
					if (retval != OFU_SET_FIELD_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting arp dst mac action field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_IP_DSCP:  /* IP DSCP (6 bits in ToS field). */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IP_DSCP:");
					retval = ofu_push_set_ip_dscp_bits_in_set_field_action(msg, &action->ui8_data);
					if (retval != OFU_SET_FIELD_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting ip DSCP action fieild failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_IP_ECN:  /* IP ECN (2 bits in ToS field). */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IP_ECN:");
					retval = ofu_push_set_ip_ecn_bits_in_set_field_action(msg, &action->ui8_data);
					if (retval != OFU_SET_FIELD_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting ip ECN action fieild failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			 case OFPXMT_OFB_IPV6_SRC:
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IP_SRC:");
					retval = ofu_push_set_source_ipv6_addr_in_set_field_action(msg, &(action->ipv6_addr.ipv6Addr32));
					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting ipv6 src addr action fieild failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			 case OFPXMT_OFB_IPV6_DST:
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IP_DST:");
					retval = ofu_push_set_destination_ipv6_addr_in_set_field_action(msg, &(action->ipv6_addr.ipv6Addr32));
					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting ipv6 dest addr  action fieild failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			 case OFPXMT_OFB_IPV6_FLABEL: /* IPv6 Flow Label */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_FLABEL:%x",action->ui32_data);
					retval = ofu_push_set_ipv6_flow_label_in_set_field_action(msg, &action->ui32_data);
					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting ipv6 flow label  action field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_ICMPV6_TYPE: /* ICMPv6 type. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_ICMPV6_TYPE:");
					retval = ofu_push_set_icmpv6_type_in_set_field_action(msg, &action->ui8_data);
					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting icmpv6 type action field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_ICMPV6_CODE: /* ICMPv6 code. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_ICMPV6_CODE:");
					retval = ofu_push_set_icmpv6_code_in_set_field_action(msg, &action->ui8_data);
					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Setting  icmpv6 code action field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_IPV6_ND_TARGET: /* Target address for ND. */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV6_ND_TARGET ");
					retval = ofu_push_set_nd_target_ipv6_addr_in_set_field_action(msg, &action->ipv6_addr);
					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting  IPv6 target addr in ND action field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case OFPXMT_OFB_IPV6_ND_SLL:
				/*The source link-layer address option in an IPv6Neighbor Discovery message */                     	{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_ND_SLL:");
					retval = ofu_push_set_nd_source_link_layer_addr_in_set_field_action(msg, action->ui8_data_array);

					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting source link layer action field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			case  OFPXMT_OFB_IPV6_ND_TLL:
				/* The target link-layer address option in an IPv6Neighbor Discovery message */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV6_ND_TLL: ");
					retval = ofu_push_set_nd_target_link_layer_addr_in_set_field_action(msg, action->ui8_data_array);

					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting target link layer action field failed...");
						retval = OF_FAILURE;
					}
					break;
				}
			 case OFPXMT_OFB_IPV6_EXTHDR: /* IPv6 Extension Header pseudo-field */
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV6_EXTHDR: ");
					retval = ofu_push_set_ipv6_exthdr_field_in_set_field_action(msg, &action->ui16_data);

					if (retval != OFU_ACTION_PUSH_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Setting ipv6 extension header action field failed...");
						retval = OF_FAILURE;
					}
					break;
				}



			 default:
				{
					OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Invalid action set  field Id");
					retval = OF_FAILURE;
				}
		}
	}while(0);

	return retval;
}


int32_t of_group_frame_message_request(struct of_msg *msg, struct ofi_group_desc_info *group_entry_p)
{
  struct ofi_action *action_p;
  struct ofi_bucket *bucket_entry_p;
  int32_t retval;
  int32_t status;
  uint8_t starting_location;
  uint16_t length_of_instructions;
  uint16_t length_of_bucket;
  uint32_t bucket_count;
  uchar8_t offset;
  offset = OF_GROUP_DESC_LISTNODE_OFFSET;


  if ( msg == NULL )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"invalid message");
    return OF_FAILURE;

  }
  if ( group_entry_p == NULL )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"group entry is invalid");
    return OF_FAILURE;

  }

  do
  { 
    ofu_start_appending_buckets_to_group(msg);
    bucket_count = OF_LIST_COUNT(group_entry_p->bucket_list);
    OF_LIST_SCAN(group_entry_p->bucket_list, bucket_entry_p, struct ofi_bucket *,OF_BUCKET_LISTNODE_OFFSET)
    {
      switch(group_entry_p->group_type)
      {
        case  OFPGT_ALL: /* All (multicast/broadcast) group.  */
          status=ofu_append_bucket_to_group_type_all(msg);
          break;

        case OFPGT_SELECT: /* Select group. */
	  if(bucket_entry_p->weight == 0){
		  status = OF_GROUP_WEIGHT_PARAM_REQUIRED;
		  break;
 	  }
          status=ofu_append_bucket_to_group_type_select(msg,
              bucket_entry_p->weight);
	  break;

        case OFPGT_INDIRECT: /* Indirect group. */
	  if(bucket_count > 1){
		  status = OF_GROUP_BUCKET_LIMIT_EXCEEDED;
		  break;
	  }
          status=ofu_append_bucket_to_group_type_indirect(msg);
         // OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"not supported");
          break;

        case OFPGT_FF: /* fast failover group */ 
	  if((bucket_entry_p->watch_port == 0) && (bucket_entry_p->watch_group == 0))
	  {
		  status = OF_GROUP_WATCH_PORT_OR_GROUP_REQUIRED;
		  break;
	  }
          status=ofu_append_bucket_to_group_type_fast_failover(msg,
              bucket_entry_p->watch_port,
              bucket_entry_p->watch_group);
          break; 

      }

     if(status == OFU_APPEND_BUCKET_SUCCESS){
      ofu_start_pushing_actions(msg);
      OF_LIST_SCAN(bucket_entry_p->action_list, action_p, struct ofi_action *, OF_ACTION_LISTNODE_OFFSET)
      {
        retval=	of_group_frame_action_request(msg, action_p);
        if (retval != OF_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"of_group_frame_action_request failed");
          return OF_FAILURE;
        }
      }

      ofu_end_pushing_actions(msg,
          &starting_location,
          &length_of_instructions);
      ((struct ofp_bucket *) msg->desc.ptr2)->len = htons(length_of_instructions);

    }
    }
    ofu_end_appending_buckets_to_group(msg,
        &starting_location,
        &length_of_bucket);
  }while(0);
  return status;
}

int32_t of_group_frame_and_send_message_request(uint64_t datapath_handle, uint32_t group_id, uint8_t command)
{
  struct of_msg *msg;
  uint16_t  msg_len=800;
  struct ofi_group_desc_info *group_entry_p;
  int32_t retval;
  int32_t status=OF_SUCCESS;
  struct dprm_datapath_entry *datapath_entry_p=NULL;
  void *conn_info_p;
  uchar8_t offset;
  offset = OF_GROUP_DESC_LISTNODE_OFFSET;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"entered");
  msg = ofu_alloc_message(OFPT_GROUP_MOD, msg_len);
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


    OF_LIST_SCAN(datapath_entry_p->group_table,group_entry_p, struct ofi_group_desc_info *,offset)
    {
      if( group_entry_p->group_id == group_id)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"group found");
        break;
      }
    }

    if (group_entry_p == NULL )
    {

      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"group not found");
      status=OF_FAILURE;
      break;
    }

  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  switch(command)
  {
    case ADD_GROUP:
      retval=of_add_group(msg, datapath_handle,group_entry_p->group_id,group_entry_p->group_type, &conn_info_p);
      if ( retval != OFU_ADD_GROUP_ENTRY_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"add_group failed");
        status=OF_FAILURE;
      }
      break;
    case MOD_GROUP:
      retval=of_modify_group(msg, datapath_handle,group_entry_p->group_id,group_entry_p->group_type,&conn_info_p);
      if ( retval != OFU_MODIFY_GROUP_ENTRY_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"mod_group failed");
        status=OF_FAILURE;
      }
      break;
    case DEL_GROUP:
      retval=of_delete_group(msg, datapath_handle,group_entry_p->group_id,&conn_info_p);
      if ( retval != OFU_DELETE_GROUP_ENTRY_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Delete group failed");
        status=OF_FAILURE;
      }
      break;
    default:
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"invalid command");
      status=OF_FAILURE;
      break;
  }
  if ( status == OF_FAILURE )
  {
	  if (msg)
		  msg->desc.free_cbk(msg);
      status=OF_FAILURE;
      return status;
  }

  CNTLR_RCU_READ_LOCK_TAKE(); 
  do
  {
    switch(command)
    {
      case ADD_GROUP:
        retval= of_group_frame_message_request(msg, group_entry_p);
        if ( retval != OFU_APPEND_BUCKET_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"framing message failed");
          status=retval;
          break;
        }
        break;
      case MOD_GROUP:
        retval= of_group_frame_message_request(msg, group_entry_p);
        if ( retval != OFU_APPEND_BUCKET_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"framing message failed");
          status=retval;
          break;
        }
        break;
      case DEL_GROUP:
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Delete group ");
        break;
      default:
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"invalid command");
        status=OF_FAILURE;
        break;
    }
  }while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  if ( status != OF_SUCCESS )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"status failed");
    return status;

  }

  switch(command)
  {
    case ADD_GROUP:
    case MOD_GROUP:
    case DEL_GROUP:
      retval=of_cntrlr_send_request_message(msg, datapath_handle, conn_info_p,NULL, NULL);
      if ( retval != OF_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"add_group failed");
        status=OF_FAILURE;
      }
      break;
#if 0
    case MOD_GROUP:
      retval=of_cntrlr_send_request_message(msg, datapath_handle,group_entry_p->group_id,group_entry_p->group_type,NULL, NULL);
      if ( retval != OFU_MODIFY_GROUP_ENTRY_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"mod_group failed");
        status=OF_FAILURE;
      }
      break;
    case DEL_GROUP:
      retval=of_cntrlr_send_request_message(msg, datapath_handle,group_entry_p->group_id,NULL, NULL);
      if ( retval != OFU_DELETE_GROUP_ENTRY_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Delete group failed");
        status=OF_FAILURE;
      }
      break;
#endif
    default:
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"invalid command");
      status=OF_FAILURE;
      break;
  }

  if (status == OF_FAILURE)	{
            msg->desc.free_cbk(msg);
  }

  return status;
}

int32_t of_group_frame_action_response(char *msg, struct ofi_action **action_entry_pp, uint32_t *length)
{
  struct ofi_action *action_entry_p;
  struct ofp_action_header *action_read_p;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Entered");

  action_read_p = (struct ofp_action_header *)msg;
  action_entry_p=(struct ofi_action *)calloc(1,sizeof(struct ofi_action ));
  action_entry_p->type=ntohs(action_read_p->type);

  switch(action_entry_p->type)
  {

    case OFPAT_OUTPUT:  /* Output to switch port. */
      {
        struct ofp_action_output *output_action;
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_OUTPUT");
        output_action = (struct ofp_action_output *)msg;
        //	retval=ofu_push_output_action(msg,port_no, max_len);
        action_entry_p->port_no= ntohl(output_action->port);
        action_entry_p->max_len= ntohs(output_action->max_len);
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"output_action->port %d output_action->port%d\r\n",output_action->port,ntohl(output_action->port));
        *length +=  sizeof(struct ofp_action_output);
      }
      break;
    case   OFPAT_COPY_TTL_OUT: /* Copy TTL "outwards" -- from next-to-outermost    to outermost */
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_COPY_TTL_OUT");
      //retval=ofu_push_copy_ttl_outward_action(msg);
      *length +=  sizeof(struct ofp_action_header);
      break;
    case   OFPAT_COPY_TTL_IN  : /* Copy TTL "inwards" -- from outermost to    next-to-outermost */
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_COPY_TTL_IN:");
      //retval=ofu_push_copy_ttl_inward_action(msg);
      *length +=  sizeof(struct ofp_action_header);
      break;
    case   OFPAT_SET_MPLS_TTL : /* MPLS TTL */
      {
        struct ofp_action_mpls_ttl *mpls_ttl_action;
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_SET_MPLS_TTL");
        //retval=ofu_push_set_mpls_ttl_action(msg, mpls_ttl);
        mpls_ttl_action = (struct ofp_action_mpls_ttl *)msg;
        action_entry_p->ttl = mpls_ttl_action->mpls_ttl;
        *length +=  sizeof(struct ofp_action_mpls_ttl);
      }
      break;
    case   OFPAT_DEC_MPLS_TTL : /* Decrement MPLS TTL */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_DEC_MPLS_TTL");
        //retval=ofu_push_dec_mpls_ttl_action(msg);
        *length +=  sizeof(struct ofp_action_header);
      }
      break;
    case   OFPAT_PUSH_VLAN  : /* Push a new VLAN tag */
      {
        struct ofp_action_push *push_action;
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_PUSH_VLAN");
        //retval=ofu_push_push_vlan_header_action(msg,ether_type);
        push_action = (struct ofp_action_push *)msg;
        action_entry_p->ether_type= ntohs(push_action->ethertype);
        *length +=  sizeof(struct ofp_action_push);
      }
      break;
    case   OFPAT_POP_VLAN : /* Pop the outer VLAN tag */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_POP_MPLS:");
        //					retval=ofu_push_pop_vlan_header_action(msg);
        *length +=  sizeof(struct ofp_action_header);
      }
      break;
    case   OFPAT_PUSH_MPLS: /* Push a new MPLS tag */
      {
        struct ofp_action_push *push_action;
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_PUSH_MPLS:");
        //retval=	ofu_push_push_mpls_header_action(msg, ether_type);
        push_action = (struct ofp_action_push *)msg;
        action_entry_p->ether_type= ntohs(push_action->ethertype);
        *length +=  sizeof(struct ofp_action_push);
      }
      break;
    case   OFPAT_POP_MPLS : /* Pop the outer MPLS tag */
      {
        struct ofp_action_pop_mpls *pop_mpls_action;
        //retval=	ofu_push_pop_mpls_header_action(msg, ether_type);
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_POP_MPLS:");
        pop_mpls_action = (struct ofp_action_pop_mpls *)msg;
        action_entry_p->ether_type= ntohs(pop_mpls_action->ethertype);
        *length +=  sizeof(struct ofp_action_pop_mpls);
      }
      break;
    case   OFPAT_SET_QUEUE: /* Set queue id when outputting to a port */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_SET_QUE:");
        //retval=ofu_push_set_queue_action(msg,queue_id);
        *length +=  sizeof(struct ofp_action_header);
      }
      break;
    case   OFPAT_GROUP : /* Apply group. */
      {
        struct ofp_action_group *group_action;
        //retval=ofu_push_group_action(msg, group_id);
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_GROUP:");
        group_action = (struct ofp_action_group *)msg;
        action_entry_p->group_id= ntohl(group_action->group_id);
        *length +=  sizeof(struct ofp_action_group);
      }
      break;
    case   OFPAT_SET_NW_TTL : /* IP TTL. */
      {
        //						retval=	ofu_push_set_ipv4_ttl_action(msg,ipv4_ttl);
        struct ofp_action_nw_ttl *nw_ttl_action;
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," OFPAT_SET_NW_TTL:");
        nw_ttl_action = (struct ofp_action_nw_ttl *)msg;
        action_entry_p->ttl= nw_ttl_action->nw_ttl;
        *length +=  sizeof(struct ofp_action_nw_ttl);
      }
      break;
    case   OFPAT_DEC_NW_TTL: /* Decrement IP TTL. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," OFPAT_DEC_NW_TTL:");
        //retval=ofu_push_dec_ipv4_ttl_action(msg);
        *length +=  sizeof(struct ofp_action_header);
      }
      break;
    case   OFPAT_SET_FIELD: /* Set a header field using OXM TLV format. */
      {
        struct ofp_action_set_field *action_set_field = (struct ofp_action_set_field *)msg;
        msg += 4; /* Moving till OXM value */

        of_group_frame_action_setField_response(msg,action_entry_p,length);
        
        *length += ntohs(action_set_field->len);
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," OFPAT_SET_FIELD: len=%d\r\n", action_set_field->len);
      }
      break;
    case   OFPAT_PUSH_PBB: /* Push a new PBB service tag (I-TAG) */
      {
        struct ofp_action_push *push_action;
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_PUSH_PBB:");
        //		retvail=ofu_push_push_pbb_header_action(msg,ether_type);
        push_action = (struct ofp_action_push *)msg;
        action_entry_p->ether_type= ntohs(push_action->ethertype);
        *length +=  sizeof(struct ofp_action_push);
      }
      break;
    case   OFPAT_POP_PBB: /* Pop the outer PBB service tag (I-TAG) */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPAT_POP_PBB:");
        //retval=	ofu_push_pop_pbb_header_action(msg);
        *length +=  sizeof(struct ofp_action_header);
      }
      break;
    case OFPAT_EXPERIMENTER:
      {
	      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPAT_EXPERIMENTER:");
	      struct fslx_action_header *fslx_action = (struct fslx_action_header *)msg;
                
              action_entry_p->vendorid = ntohs(fslx_action->vendor);
              action_entry_p->sub_type = ntohs(fslx_action->subtype);
              OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "fslx_action->vendorid %x n2hl%x fslx_action->subtype %d ntohs(%d) ",fslx_action->vendor,ntohl(fslx_action->vendor),fslx_action->subtype,ntohs(fslx_action->subtype));
	      switch(fslx_action->subtype)
	      {
		      case FSLXAT_ARP_RESPONSE:
			      {
				      struct fslx_action_arp_response *fslx_action = msg;
				      memcpy(&(action_entry_p->ui64_data),fslx_action->target_mac,6);
				      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," FSLXAT_ARP_RESPONSE action_entry_p->ui64_data %llX\n",action_entry_p->ui64_data);
			      }
			      break;
		      case FSLXAT_IP_FRAGMENT:
			      {
				      struct fslx_action_ip_fragment *fslx_action = msg;
				      action_entry_p->ui32_data = fslx_action->mtu;
				      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," FSLXAT_IP_FRAGMENT fslx_action->mtu %d\n",fslx_action->mtu);
			      }
			      break;
		      case FSLXAT_IP_REASM:
			      {
				      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," FSLXAT_IP_REASM \n");
			      }
			      break;
		      case FSLXAT_DROP_PACKET:
			      {
				      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," FSLXAT_DROP_PACKET \n");
			      }
			      break;
		      case FSLXAT_SEND_ICMPV4_DEST_UNREACHABLE:
			      {
				      struct fslx_action_icmpv4_dest_unreachable *fslx_action = msg;
				      action_entry_p->ui8_data = fslx_action->code;
				      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," FSLXAT_SEND_ICMPV4_DEST_UNREACHABLE code %d\n",fslx_action->code);
				      if(fslx_action->code == FSLX_ICMPV4_DR_EC_DFBIT_SET)
					      action_entry_p->ui16_data = fslx_action->next_hop_mtu;
			      }
			      break;
		      case FSLXAT_SEND_ICMPV4_TIME_EXCEED:
			      {
				      struct fslx_action_icmpv4_time_exceed *fslx_action = msg;
				      action_entry_p->ui8_data = fslx_action->code;
				      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," FSLXAT_SEND_ICMPV4_TIME_EXCEED fslx_action->code %d\n",fslx_action->code);
			      }
			      break;
		      case FSLXAT_SET_PHY_PORT_FIELD_CNTXT:
			      {
				      struct fslx_action_set_in_phy_port_cntxt *fslx_action = msg;
				      action_entry_p->ui32_data = fslx_action->in_phy_port;
				      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," FSLXAT_SET_PHY_PORT_FIELD_CNTXT fslx_action->in_phy_port %d\n",fslx_action->in_phy_port);
			      }
			      break;
		      case FSLXAT_SET_META_DATA_FIELD_CNTXT:
			      {
				      struct fslx_action_meta_data_cntxt *fslx_action = msg;
				      action_entry_p->ui64_data = fslx_action->meta_data;
				      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," FSLXAT_SET_META_DATA_FIELD_CNTXT fslx_action->meta_data %llX\n",fslx_action->meta_data);
			      }
			      break;
		      case FSLXAT_SET_TUNNEL_ID_FIELD_CNTXT:
			      {
				      struct fslx_action_tunnel_id_cntxt *fslx_action = msg;
				      action_entry_p->ui64_data = fslx_action->tunnel_id;
				      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," FSLXAT_SET_TUNNEL_ID_FIELD_CNTXT fslx_action->tunnel_id %llX\n",fslx_action->tunnel_id);
			      }
			      break;
		      case FSLXAT_START_TABLE:
			      {
				      struct fslx_action_start_table *fslx_action = msg;
				      action_entry_p->ui8_data = fslx_action->table_id;
				      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," FSLXAT_START_TABLE fslx_action->table_id %d\n",fslx_action->table_id);
			      }
			      break;
		      case FSLXAT_WRITE_METADATA_FROM_PKT:
			      {
				      struct fslx_action_wr_metadata_from_pkt *fslx_action = msg;
				      action_entry_p->ui32_data = fslx_action->field_id;
				      action_entry_p->ui64_data = fslx_action->maskval;
				      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," FSLXAT_WRITE_METADATA_FROM_PKT field_ID %X mask %llX\n",fslx_action->field_id,fslx_action->maskval);
			      }
			      break;
		      case FSLXAT_WRITE_META_DATA:
			      {
				      struct fslx_action_write_metadata *fslx_action = msg;
				      action_entry_p->ui64_data = fslx_action->meta_data;
				      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"FSLXAT_WRITE_METADATA metadata %llX",fslx_action->meta_data);
			      }
			      break;
		      case FSLXAT_DETACH_BIND_OBJ:
			      {
				      struct fslx_action_detach_bind_obj *fslx_action = msg;
				      action_entry_p->ui32_data = fslx_action->bind_obj_id;
				      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"FSLXAT_DETACH_BIND_OBJ fslx_action->bind_obj_id %X",fslx_action->bind_obj_id);
				      break;
			      }
		      case FSLXAT_SEND_APP_STATE_INFO:
			      {
				      struct fslx_action_send_app_state_info *fslx_action = msg;
				      action_entry_p->ui8_data=fslx_action->event_type;
				      action_entry_p->ui16_data = fslx_action->event_method;
				      action_entry_p->ui64_data = fslx_action->event_count;
				      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"FSLXAT_SEND_APP_STATE_INFO type %d method %X count %llX",fslx_action->event_type,fslx_action->event_method,fslx_action->event_count);
				      break;
			      }

		      case FSLXAT_BIND_OBJECT_APPLY_ACTIONS:
			      {
				      struct fslx_action_execute_bind_obj_apply_actns *fslx_action = msg;
				      action_entry_p->ui32_data = fslx_action->bind_obj_id;
				      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"FSLXAT_BIND_OBJECT_APPLY_ACTIONS fslx_action->bind_obj_id %X",fslx_action->bind_obj_id);
				      break;
			      }

		      case FSLXAT_SET_APP_STATE_INFO:
			      {
				      /* struct fslx_action_set_app_state_info */
				      break;
			      }
		      case FSLXAT_CREATE_SA_CNTXT:
			      {
				      /* struct fslx_create_ipsec_sa */
				      break;
			      }

		      case FSLXAT_DELETE_SA_CNTXT:
			      {
				      /* struct fslx_delete_ipsec_sa */
				      break;
			      }

		      case FSLXAT_IPSEC_ENCAP:
			      {
				      /* struct fslx_ipsec_encap */
				      break;
			      }

		      case FSLXAT_IPSEC_DECAP:
			      {
				      /* struct fslx_ipsec_decap */
				      break;
			      }

		      case FSLXAT_POP_NATT_UDP_HEADER:
			      {
				      break;
			      }

		      case NXAST_REG_LOAD:
			      {
				      struct nx_action_reg_load *nx_action = (struct nx_action_reg_load *)((uint8_t *)msg);
				      action_entry_p->ui8_data  = (nx_action->ofs_nbits & 0x3f) + 1; //nbits
				      action_entry_p->ui16_data  = (nx_action->ofs_nbits >> 6); //offset
				      action_entry_p->ui32_data  = NXM_FIELD(nx_action->dst);
				      action_entry_p->ui64_data  = (nx_action->value);
				      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "fslx_action->vendorid %x fslx_action->subtype %d action_entry_p->ui16_data %d nx_action->value %lld",fslx_action->vendor,fslx_action->subtype,action_entry_p->ui16_data,nx_action->value);
			      }
			      break;
		      case NXAST_COPY_FIELD:
			      {
				      struct eric_action_copy_field *eric_action = msg;
				      action_entry_p->ui16_data = eric_action->num_bits;
				      action_entry_p->src_offset = eric_action->src_offset;
				      action_entry_p->dst_offset = eric_action->dst_offset;
				      action_entry_p->oxm_src_dst[0]= eric_action->oxm_src_dst[0];
				      action_entry_p->oxm_src_dst[1]= eric_action->oxm_src_dst[1];
				      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"NXAST_COPY_FIELD num_bits %x src_offset %x dst_offset %x [0] %x [1]%x ",eric_action->num_bits,eric_action->src_offset,eric_action->dst_offset,eric_action->oxm_src_dst[0],eric_action->oxm_src_dst[1]);
			      }

		      default:
			      break;
	      }
 		*length += ntohs(action_read_p->len); 
      }
      break; 

    default:
      {
	      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"unknown action %d", action_entry_p->type);
	      *length += ntohs(action_read_p->len); 
      }
	break;
  }

  *action_entry_pp=action_entry_p;
  return OF_SUCCESS;

}

int32_t of_group_frame_action_setField_response(char *msg,struct ofi_action *action_entry_p,uint32_t length){

  struct oxm_match_field *action_oxm_p;
  uint8_t field_type;

  action_oxm_p =(struct oxm_match_field *)msg;
  field_type = action_oxm_p->field_type >> 1;
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," Action ->Field type is : %d ",action_oxm_p->field_type);
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," Field type is : %d ",field_type);

  action_entry_p->setfield_type = field_type;

  switch(field_type){
  
  case OFPXMT_OFB_IN_PORT: /* Switch input port. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," OFPXMT_OFB_IN_PORT:");
        action_entry_p->ui32_data = ntohl(*(uint32_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "INPORT=%d", action_entry_p->ui32_data);
        break;
      }
    case OFPXMT_OFB_IN_PHY_PORT: /* Switch physical input port. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IN_PHY_PORT:");
        action_entry_p->ui32_data = ntohl(*(uint32_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "In-PhyPort=%d", action_entry_p->ui32_data);
        break;
      }
    case OFPXMT_OFB_METADATA: /* Metadata passed between tables. */			  
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_METADATA:");
        action_entry_p->ui64_data = ntohll(*(uint64_t *)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Metadata=%llx",action_entry_p->ui64_data);
        break;
      }
    case OFPXMT_OFB_ETH_DST: /* Ethernet destination address. */ 		   
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ETH_DST:");
        memcpy(action_entry_p->ui8_data_array, (msg + OF_OXM_HDR_LEN), 6);
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Eth_Dest =%lx", action_entry_p->ui8_data_array);
        break;
      }
    case OFPXMT_OFB_ETH_SRC: /* Ethernet source address. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ETH_SRC: ");
        memcpy(action_entry_p->ui8_data_array, (msg + OF_OXM_HDR_LEN), 6);
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Eth_Src =%lx", action_entry_p->ui8_data_array);
        break;
      }
    case OFPXMT_OFB_ETH_TYPE: /* Ethernet frame type. */ 		   
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ETH_TYPE: ");
        action_entry_p->ui16_data = ntohs(*(uint16_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Eth type=%x", action_entry_p->ui16_data);
        break;
      }
    case OFPXMT_OFB_VLAN_VID: /* VLAN id. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_VLAN_VID: ");
        action_entry_p->ui16_data = ntohs(*(uint16_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "valn id=%d", action_entry_p->ui16_data);
        break;
      }
    case OFPXMT_OFB_IP_PROTO: /* IP protocol. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IP_PROTO: ");
        action_entry_p->ui8_data = (*(uint8_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "IP Protocol=%d",action_entry_p->ui8_data);
        break;
      }
    case OFPXMT_OFB_IPV4_SRC: /* IPv4 source address. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV4_SRC: ");
        action_entry_p->ui32_data = ntohl(*(uint32_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "IPv4 src IP=%x",action_entry_p->ui32_data);
        break;
      }
    case OFPXMT_OFB_IPV4_DST: /* IPv4 destination address. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_IPV4_DST: ");
        action_entry_p->ui32_data = ntohl(*(uint32_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "IPv4 Dest IP=%x",action_entry_p->ui32_data);
        break;
      }
    case OFPXMT_OFB_TCP_SRC: /* TCP source port. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_TCP_SRC: ");
        action_entry_p->ui16_data = ntohs(*(uint16_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "tcp src port=%d", action_entry_p->ui16_data);
        break;
      }
    case OFPXMT_OFB_TCP_DST: /* TCP destination port. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_TCP_DST: ");
        action_entry_p->ui16_data = ntohs(*(uint16_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "tcp dest port=%d", action_entry_p->ui16_data);
        break;
      }
    case OFPXMT_OFB_UDP_SRC: /* UDP source port. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_UDP_SRC: ");
        action_entry_p->ui16_data = ntohs(*(uint16_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "udp src port=%d", action_entry_p->ui16_data);
        break;
      }
    case OFPXMT_OFB_UDP_DST: /* UDP destination port. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_UDP_DST: ");
        action_entry_p->ui16_data = ntohs(*(uint16_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "udp dest port=%d", action_entry_p->ui16_data);
        break;
      }
    case OFPXMT_OFB_SCTP_SRC: /* SCTP source port. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_SCTP_SRC: ");
        action_entry_p->ui16_data = ntohs(*(uint16_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "sctp src port=%d", action_entry_p->ui16_data);
        break;
      }
    case OFPXMT_OFB_SCTP_DST: /* SCTP destination port. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_SCTP_DST: ");
        action_entry_p->ui16_data = ntohs(*(uint16_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "sctp dest port=%d", action_entry_p->ui16_data);
        break;
      }
    case OFPXMT_OFB_ICMPV4_TYPE: /* ICMP type. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV4_TYPE:");
        action_entry_p->ui8_data = (*(uint8_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "ICMP type=%d",action_entry_p->ui8_data);
        break;
      }
    case OFPXMT_OFB_ICMPV4_CODE: /* ICMP code. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV4_CODE: ");
        action_entry_p->ui8_data = (*(uint8_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "ICMP code=%d",action_entry_p->ui8_data);
        break;
      }
    case OFPXMT_OFB_ARP_OP: /* ARP opcode. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_OP: ");
        action_entry_p->ui16_data = ntohs(*(uint16_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "arp op code =%d", action_entry_p->ui16_data);
        break;
      }
    case OFPXMT_OFB_MPLS_LABEL: /* MPLS label. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_MPLS_LABEL:");
        action_entry_p->ui32_data = ntohl(*(uint32_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "MPLS Label=%d",action_entry_p->ui32_data);
        break;
      }
    case OFPXMT_OFB_MPLS_TC: /* MPLS TC. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_MPLS_TC:");
        action_entry_p->ui8_data = (*(uint8_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "MPLS TC=%d",action_entry_p->ui8_data);
        break;
      }
    case OFPXMT_OFB_MPLS_BOS: /* MPLS BoS bit. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_MPLS_BOS:");
        action_entry_p->ui8_data = (*(uint8_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "MPLS BOS=%d",action_entry_p->ui8_data);
        break;
      }
    case OFPXMT_OFB_PBB_ISID: /* PBB I-SID. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_PBB_ISID:");
        action_entry_p->ui8_data = (*(uint8_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "PBB ISID=%d",action_entry_p->ui8_data);
        break;
      }
    case OFPXMT_OFB_TUNNEL_ID: /* Logical Port Metadata. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_TUNNEL_ID:");
        action_entry_p->ui64_data = ntohll(*(uint64_t *)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Tunnel ID=%llx",action_entry_p->ui64_data);
        break;
      }
    case OFPXMT_OFB_VLAN_PCP:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_VLAN_PCP");
        action_entry_p->ui8_data = (*(uint8_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "VLAN PCP=%d",action_entry_p->ui8_data);
        break;
      }
    case OFPXMT_OFB_ARP_SPA: /* IPv4 source address. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_SPA: ");
        action_entry_p->ui32_data = ntohl(*(uint32_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "ARP SIP=%x",action_entry_p->ui32_data);
        break;
      }
    case OFPXMT_OFB_ARP_TPA: /* IPv4 source address. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ARP_TPA: ");
        action_entry_p->ui32_data = ntohl(*(uint32_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "ARP DIP=%x",action_entry_p->ui32_data);
        break;
      }
    case OFPXMT_OFB_ARP_SHA:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_ARP_SHA: ");
        memcpy(action_entry_p->ui8_data_array, (msg + OF_OXM_HDR_LEN), 6);
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "ARP SRC MAC =%lx", action_entry_p->ui8_data_array);
        break;
      }	
    case OFPXMT_OFB_ARP_THA:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_ARP_THA: ");
        memcpy(action_entry_p->ui8_data_array, (msg + OF_OXM_HDR_LEN), 6);
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "ARP DST MAC =%lx", action_entry_p->ui8_data_array);
        break;
      }
    case OFPXMT_OFB_IP_DSCP:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IP_DSCP: ");
        action_entry_p->ui8_data = (*(uint8_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "IP DSCP=%d",action_entry_p->ui8_data);
        break;
      }
    case OFPXMT_OFB_IP_ECN:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IP_ECN: ");
        action_entry_p->ui8_data = (*(uint8_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "IP ECN=%d",action_entry_p->ui8_data);
        break;
      }
    case OFPXMT_OFB_IPV6_SRC:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_SRC: ");
        break;
      }
    case OFPXMT_OFB_IPV6_DST:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_DST: ");
        break;
      }
    case OFPXMT_OFB_ICMPV6_TYPE: /* ICMP type. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV6_TYPE:");
        action_entry_p->ui8_data = (*(uint8_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "ICMPV6 Type=%d",action_entry_p->ui8_data);
        break;
      }
    case OFPXMT_OFB_ICMPV6_CODE: /* ICMP code. */
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"OFPXMT_OFB_ICMPV6_CODE: ");
        action_entry_p->ui8_data = (*(uint8_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "ICMPV6 Code=%d",action_entry_p->ui8_data);
        break;
      }
    case OFPXMT_OFB_IPV6_FLABEL:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_FLABEL");
        action_entry_p->ui32_data = ntohl(*(uint32_t*)(msg + OF_OXM_HDR_LEN));
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "IPV6 FLABEL=%x",action_entry_p->ui32_data);
        break;
      }
    case OFPXMT_OFB_IPV6_ND_TARGET:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_ND_TARGET: ");
        break;
      }
    case OFPXMT_OFB_IPV6_ND_SLL: /* The source link-layer address option in an IPv6Neighbor Discovery message.*/
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_ND_SLL:");
         break;
      }
    case OFPXMT_OFB_IPV6_ND_TLL: /* The target link-layer address option in an IPv6Neighbor Discovery message.*/
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," OFPXMT_OFB_IPV6_ND_TLL: ");
        break;
      }
    case OFPXMT_OFB_IPV6_EXTHDR:
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "OFPXMT_OFB_IPV6_EXTHDR: ");
        break;
      }
    default :
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Invalid Match Field Type");
        break;
       }
  }
  return OF_SUCCESS;


}

int32_t of_group_frame_bucket_response(char *msg, uint8_t   group_type,  struct ofi_bucket **bucket_entry_pp, uint32_t *msg_length)
{
  struct ofi_bucket *bucket_entry_p;
  struct ofp_bucket *bucket_read_p;
  struct ofi_action *action_entry_p=NULL;
  int32_t total_actions_len=0;
  uint32_t length=0;
  int32_t return_value;
  int32_t status=OF_SUCCESS;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Entered");

  bucket_read_p=( struct ofp_bucket *)msg;
  bucket_entry_p=(struct ofi_bucket *)calloc(1,sizeof(struct ofi_bucket ));
  if ( bucket_entry_p == NULL )
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Entered");
    status=OF_FAILURE;
    return status;
  }
  OF_LIST_INIT(bucket_entry_p->action_list, of_group_free_action_entry);
  switch(group_type)
  {
    case  OFPGT_ALL: /* All (multicast/broadcast) group.  */
      break;

    case OFPGT_SELECT: /* Select group. */
      bucket_entry_p->weight=ntohs(bucket_read_p->weight);
      break;

    case OFPGT_INDIRECT: /* Indirect group. */
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"not supported");
      break;

    case OFPGT_FF: /* fast failover group */ 
      bucket_entry_p->watch_port=ntohl(bucket_read_p->watch_port);
      bucket_entry_p->watch_group=ntohl(bucket_read_p->watch_group);
      break; 

  }

  total_actions_len= ntohs(bucket_read_p->len) - sizeof(struct ofp_bucket);
  msg += sizeof(struct ofp_bucket);
  *msg_length += sizeof(struct ofp_bucket);   

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"bucket len %d bucketsize %d actionsen %d", ntohs(bucket_read_p->len), sizeof(struct ofp_bucket), total_actions_len);
  while (total_actions_len > 0 )
  {
    length=0;
    return_value=of_group_frame_action_response(msg, &action_entry_p, &length);
    if ( return_value != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"frame action response failed");
    	//call of_unregister_actions  -- need to write new api - Atmaram 
         //free(bucket_entry_p); -- then free bucket entry
      return OF_FAILURE;
    }
    OF_APPEND_NODE_TO_LIST(bucket_entry_p->action_list, action_entry_p, OF_ACTION_LISTNODE_OFFSET);
    total_actions_len -= length;
    msg += length;
    *msg_length += length;
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"action_length %d", length);
  }

  *bucket_entry_pp=bucket_entry_p;
  return OF_SUCCESS;
}

int32_t of_group_frame_response(char *msg, struct ofi_group_desc_info *group_entry_p, uint32_t msg_length)
{

  struct ofi_bucket *bucket_entry_p;
  uint32_t bucket_length;
  int32_t return_value;
  uchar8_t offset;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"frame response msg_length %d", msg_length);

  OF_LIST_INIT(group_entry_p->bucket_list, of_group_free_bucket_entry);

  while (msg_length > 0 )
  {
    bucket_length=0;
    return_value= of_group_frame_bucket_response(msg, group_entry_p->group_type, &bucket_entry_p, &bucket_length);
    if ( return_value != OF_SUCCESS )
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"of_group_frame_bucket_response failed");
      return OF_FAILURE;
    }
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"bucket_length %d", bucket_length);
    OF_APPEND_NODE_TO_LIST(group_entry_p->bucket_list, bucket_entry_p,OF_BUCKET_LISTNODE_OFFSET);
    msg += bucket_length;
    msg_length -= bucket_length;

  }
  return OF_SUCCESS;
}

void of_group_free_bucket_entry(struct rcu_head *node)
{
  struct ofi_bucket  *bucket;

  if(node)
  {
    bucket = (struct ofi_bucket *)(((char *)node) - OF_BUCKET_LISTNODE_OFFSET);
    if(bucket)
    {
      free(bucket);
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,
                 "Deleted succesfully group bucket node");
    }
    else
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,
                 "Failed to free group bucket node");
    }
  }
  return;
}

void of_group_free_action_entry(struct rcu_head *node)
{
  struct ofi_action  *action;

  if(node)
  {
    action = (struct ofi_action *)(((char *)node) - OF_ACTION_LISTNODE_OFFSET);
    if(action)
    {
      free(action);
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,
                 "Deleted succesfully group action node");
    }
    else
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,
                 "Failed to free group action node");
    }
  }
  return;
}

