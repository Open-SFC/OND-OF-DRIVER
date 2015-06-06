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

/*File: cntlr_sm_app_igmp.c
 *Author: Kumara Swamy Ganji <kumar.sg@freescale.com>
 * Description:
 * This file contains sample igmp application registering with controller transport for pkt in event and
 * push flow-mods accordingly for multicast data traffic to pass through openflow datapath */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include "controller.h"
#include "of_utilities.h"
#include "of_multipart.h"
#include "of_msgapi.h"
#include "cntrl_queue.h"
#include "cntlr_event.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "dprmldef.h"
#include "dprm.h"
#include "cntlr_sample_app.h"
#include "cntlr_sm_app_igmp.h"
#include "oflog.h"
#include "of_group.h"
#include "pktmbuf.h"

extern cntlr_sm_sw_conf_t	dp_conf[OF_CNTLR_MAX_DATA_PATHS];
extern uint32_t       no_of_data_paths_attached_g;
extern int32_t
sm_app_add_missentry_with_goto_table_instruction(uint64_t datapath_handle, 
						 uint8_t table_id, /* id of the table to which this entry will be added */
						 uint8_t jump_tableid);
extern int32_t of_frame_pktout_msg(struct of_msg *msg, uint64_t datapath_handle,
				uint8_t reply_pkt, uint32_t buffer_id,
				uint32_t in_port, uint8_t channel_no, void **conn_info_pp);

void sm_app_igmp_port_ready_cbk(uint32_t notification_type,
                                     uint64_t datapath_handle,
                                     struct   dprm_datapath_notification_data notification_data,
                                     void     *callback_arg1,
                                     void     *callback_arg2);


/**** Global Variables ******/
uint16_t entry_index;
uint16_t mcast_rtentry_priority;
uint32_t mcast_group_id;

int32_t	sm_app_igmp_init()
{
	uint32_t dp_index, ii;

	entry_index = 0;
	mcast_rtentry_priority = 10;
	mcast_group_id = 1;

	for (dp_index = 0; dp_index < OF_CNTLR_MAX_DATA_PATHS; dp_index++) {
		for (ii = 0; ii < OF_CNTLR_APP_MAX_MCAST_ROUTE_ENTRIES; ii++) {
			dp_conf[dp_index].mcast_rt_table[ii].no_of_outports = 0;
			dp_conf[dp_index].mcast_rt_table[ii].binuse = FALSE;
		}
	}
	return OF_SUCCESS;
}

/* Sends general query message from all ports */
int32_t sm_app_igmp_send_gen_query(uint64_t dp_handle, uint32_t dp_index,
				uint32_t dp_port, int8_t *port_name)
{
	uint32_t grp_ipaddr = IGMP_GRPADDR_ALL_HOSTS, src_ipaddr = 0;
	uint8_t  *ip_hdr, *igmp_hdr, grp_macaddr[OFP_ETH_ALEN] = {};
	uint16_t checksum = 0;
	uint8_t hw_addr[OFP_ETH_ALEN] = {00,01,0x27,0x2f,0x3b,0xb5};
	//uint32_t dp_port, no_of_ports = 0;
	//struct	dprm_port_info config_port_info = {};
	//struct	dprm_port_runtime_info config_runtime_port_info = {};
	struct	of_cntlr_conn_info *conn_info;
	//uint32_t	dp_index;
	uint8_t	gen_query[50];
	uint32_t 	igmp_pkt_len = 0;
	uint32_t 	ii;
	//uint64_t	port_handle;
	uint8_t	*ucPtr;
	uint8_t	*action_start_loc=NULL;
	uint16_t	action_len;
	uint16_t	msg_len;
	struct	of_msg *pktout_msg;
	int32_t	status = OF_SUCCESS;
	int32_t	retval = OF_SUCCESS;
	 int32_t flags=0;
  struct pkt_mbuf *mbuf = NULL;


	OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, "SENDING IGMP GEN QUERY...");
	do
	{
		#if 0
		for (dp_index = 0; dp_index < OF_CNTLR_MAX_DATA_PATHS; dp_index++)	{
			if (dp_conf[dp_index].dp_handle == dp_handle)	{
				break;
			}
		}

		if (dp_index == OF_CNTLR_MAX_DATA_PATHS)	{
			OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,
				"%s:no datapath found with handle=%llx",__FUNCTION__,dp_handle);
			status = OF_FAILURE;
			break;
		}

		if (dprm_get_first_datapath_port(dp_handle,&config_port_info,
				&config_runtime_port_info,&port_handle) != DPRM_SUCCESS)
		{
			OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:dprm_get_first_datapath_port failed.",
							__FUNCTION__);
			return OF_FAILURE;
		}
		
		
		while (retval == DPRM_SUCCESS)	{
			dp_port = config_port_info.port_id;
        #endif
			
			for (ii = 0; ii < dp_conf[dp_index].no_of_iface_entries; ii++)	{
				if (!strcmp(dp_conf[dp_index].iface_info[ii].port_name, port_name))	{
					src_ipaddr = dp_conf[dp_index].iface_info[ii].ip_address;
					break;
				}
			}

			if (ii == dp_conf[dp_index].no_of_iface_entries)
			{
			    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,
 				"%s:Unable to locate port with portname=%s",
					__FUNCTION__,port_name);
			    return OF_FAILURE;
			}
	
			/* Frame IGMP general query packet */ 
			OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"%s:framing igmp gen query...",__FUNCTION__);
			igmp_pkt_len = 0;
			ucPtr = gen_query;
			/* set dest mac derived from group ip address */
			GET_MCAST_MAC_BY_IP(grp_ipaddr, grp_macaddr);

			/** FRAME MAC HEADER **/
			memcpy(ucPtr, grp_macaddr, OFP_ETH_ALEN);
			ucPtr += OFP_ETH_ALEN;
			igmp_pkt_len += OFP_ETH_ALEN;
			/* set port mac address as src mac address */
			memcpy(ucPtr, hw_addr, OFP_ETH_ALEN);
			ucPtr += OFP_ETH_ALEN;
			igmp_pkt_len += OFP_ETH_ALEN;
			/* set eth-type as IP */
			(*(uint16_t *)ucPtr) = 0x0800;
			ucPtr += 2;
			igmp_pkt_len += 2;

			/**  FRAME IP HEADER **/
			/* start of IP header  is version which is 4 and header min len i.e 5*4=20 bytes*/
			ip_hdr = ucPtr;
			(*(uint8_t *)ucPtr) = 0x45;
			ucPtr += 1;
			igmp_pkt_len += 1;
			/* TOS */
			(*(uint8_t *)ucPtr) = 0;
			ucPtr += 1;
			igmp_pkt_len += 1;
			/* Total length IPHDR+DATA 20+8(IGMP)*/
			(*(uint16_t *)ucPtr) = 0x1C;
			ucPtr += 2;
			igmp_pkt_len += 2;
			/* identification */
			(*(uint16_t *)ucPtr) = 0x4321;/* some random number */
			ucPtr += 2;
			igmp_pkt_len += 2;
			/* flags + fragmentation offset: 3bits+13 bits*/
			(*(uint16_t *)ucPtr) = 0;
			ucPtr += 2;
			igmp_pkt_len += 2;
			/* TTL */
			(*(uint8_t *)ucPtr) = 1; /* TTL value must be one */
			ucPtr += 1;
			igmp_pkt_len += 1;
			/* Protocol */
			(*(uint8_t *)ucPtr) = OF_CNTLR_IP_PROTO_IGMP;
			ucPtr += 1;
			igmp_pkt_len += 1;
			/* header checksum */
			OF_CNTLR_CALCULATE_CHECKSUM(ip_hdr, 20, checksum);
			(*(uint16_t *)ucPtr) = checksum;
			ucPtr += 2;
			igmp_pkt_len += 2;
			/* src -ip */
			(*(uint32_t *)ucPtr) = src_ipaddr;
			ucPtr += 4;
			igmp_pkt_len += 4;
			/* dst -ip */
			(*(uint32_t *)ucPtr) = grp_ipaddr;
			ucPtr += 4;
			igmp_pkt_len += 4;

			/* FRAME IGMP HEADER */
			/* Start of IGMP packet message type */
			igmp_hdr = ucPtr;
			(*(uint8_t *)ucPtr) = IGMP_MEMBERSHIP_QUERY;
			ucPtr += 1;
			igmp_pkt_len += 1;
			/* max response time */
			(*(uint8_t *)ucPtr) = IGMP_MAX_RESPONSE_TIME;
			ucPtr += 1;
			igmp_pkt_len += 1;

			/* checksum */
			OF_CNTLR_CALCULATE_CHECKSUM(igmp_hdr, 8, checksum);
			(*(uint16_t *)ucPtr) = checksum;
			ucPtr += 2;
			igmp_pkt_len += 2;

			/* For general query group address will be zero */
			(*(uint32_t *)ucPtr) = 0;
			ucPtr += 4;
			igmp_pkt_len += 4;

			msg_len = (OFU_PACKET_OUT_MESSAGE_LEN+16+OFU_OUTPUT_ACTION_LEN+igmp_pkt_len);
			OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG," IGMP pkt out msg len %d",msg_len);
			of_alloc_pkt_mbuf_and_set_of_msg(mbuf, pktout_msg, OFPT_PACKET_OUT, msg_len, flags, status);
			if(status == FALSE)
			{
				OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR, "pkt_mbuf_alloc() failed");
				status = OF_FAILURE;
				return status;
			}
	
//			pktout_msg = ofu_alloc_message(OFPT_PACKET_OUT, msg_len);
			if (pktout_msg == NULL)
			{
				OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,
					"%s:%d OF message alloc error \r\n",__FUNCTION__,__LINE__);
				return OF_FAILURE;
			}
	
			retval = of_frame_pktout_msg(pktout_msg, dp_handle,
							FALSE, OFP_NO_BUFFER,
							OFPP_CONTROLLER, 0,
							&conn_info);
			if (retval != OF_SUCCESS)
			{
				OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in framing pkt out");
//				pktout_msg->desc.free_cbk(pktout_msg);
                                of_get_pkt_mbuf_by_of_msg_and_free(pktout_msg);
 				//of_pkt_mbuf_free(pktout_msg);
				status = OF_FAILURE;
				break;
			}
			ofu_start_pushing_actions(pktout_msg);
			retval = ofu_push_output_action(pktout_msg, dp_port, OFPCML_NO_BUFFER);
			if (retval != OFU_ACTION_PUSH_SUCCESS)
			{
				OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,
					"%s:%d  Error in adding output action  err = %d\r\n",
								__FUNCTION__,__LINE__,retval);
				//pktout_msg->desc.free_cbk(pktout_msg);
                                of_get_pkt_mbuf_by_of_msg_and_free(pktout_msg);
 				//of_pkt_mbuf_free(pktout_msg);
				status = OF_FAILURE;
				break;
			}
			ofu_end_pushing_actions(pktout_msg, action_start_loc, &action_len);
			((struct ofp_packet_out*)(pktout_msg->desc.start_of_data))->actions_len = htons(action_len);

			retval = of_add_data_to_pkt_and_send_to_dp(pktout_msg, dp_handle, conn_info,
								igmp_pkt_len, gen_query, NULL, NULL);
			if (retval != OF_SUCCESS)	{
				OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,
					"%s:%d  Error in sending packet err = %d\r\n", __FUNCTION__,__LINE__,retval);
				//pktout_msg->desc.free_cbk(pktout_msg);
                                of_get_pkt_mbuf_by_of_msg_and_free(pktout_msg);
 				//of_pkt_mbuf_free(pktout_msg);
				return retval;
			}
#if 0
			memset(&config_port_info, 0, sizeof(struct	  dprm_port_info));
			retval = dprm_get_next_datapath_port(dp_handle, &config_port_info,
						&config_runtime_port_info, &port_handle);
		}
#endif
	  }
	  while(0);

	  if (status == OF_FAILURE) {
	//	pktout_msg->desc.free_cbk(pktout_msg);
                                of_get_pkt_mbuf_by_of_msg_and_free(pktout_msg);
 				//of_pkt_mbuf_free(pktout_msg);
	  }

	  return status;
}



static int32_t sm_app_mcast_get_free_entry(uint32_t dp_index)
{
	int32_t	ii;

	for (ii=0; ii< OF_CNTLR_APP_MAX_MCAST_ROUTE_ENTRIES; ii++) {
		if (dp_conf[dp_index].mcast_rt_table[ii].binuse == FALSE)
			return ii;
	}
	OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,"IGMP:%s no free mcast entry found ",__FUNCTION__);
	return OF_FAILURE;
}

static int32_t sm_app_mcast_get_free_portentry(uint32_t dp_index, uint32_t entry_index)
{
	int32_t	ii;

	for (ii=0; ii < OF_CNTLR_APP_MAX_MCAST_OUTPORTS; ii++)	{
		if (dp_conf[dp_index].mcast_rt_table[entry_index].port_info[ii].binuse == FALSE)
			return ii;
	}
	OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,"IGMP:%s no free port entry found ",__FUNCTION__);
	return OF_FAILURE;
}

static int32_t sm_app_mcast_get_portentry_by_id(uint32_t dp_index,
						uint32_t entry_index, uint32_t port_id)
{
	int32_t	ii;

	for (ii=0; ii < OF_CNTLR_APP_MAX_MCAST_OUTPORTS; ii++)	{
		if ((dp_conf[dp_index].mcast_rt_table[entry_index].port_info[ii].port_id == port_id) &&
			(dp_conf[dp_index].mcast_rt_table[entry_index].port_info[ii].binuse == TRUE))
		{
			return ii;
		}
	}
	OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,"IGMP:%s no entry found with port-id=%d",__FUNCTION__,port_id);
	return OF_FAILURE;
}

static int32_t sm_app_mcast_get_entry_by_grp_address(uint32_t dp_index, uint32_t grp_address)
{
	int32_t	ii;

	for (ii=0; ii< OF_CNTLR_APP_MAX_MCAST_ROUTE_ENTRIES; ii++)	{
		if ((dp_conf[dp_index].mcast_rt_table[ii].grp_addr == grp_address) &&
			(dp_conf[dp_index].mcast_rt_table[ii].binuse == TRUE))
		{
			return ii;
		}
	}
	OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,"IGMP:%s no entry found with grp-ip=%x",__FUNCTION__,grp_address);
	return OF_FAILURE;
}

int32_t sm_app_igmp_add_group_to_dprm(uint64_t dp_handle, uint32_t group_type, uint32_t group_id)
{
	struct ofi_group_desc_info *group_info;

	group_info = (struct ofi_group_desc_info * )calloc(1, sizeof(struct ofi_group_desc_info));
	if (group_info == NULL)	{
		OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,
		"%s:%d	allocating group node failed..\r\n", __FUNCTION__,__LINE__);
    	return OF_FAILURE;
	}
	
	group_info->group_id = group_id;
	group_info->group_type = group_type;
	if (of_register_group(dp_handle, group_info) != OF_SUCCESS)	{
		OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,
		"%s:%d group registration to dprm datapath failed...\r\n", __FUNCTION__,__LINE__);
		return OF_FAILURE;
	}
	return OF_SUCCESS;
}

int32_t sm_app_igmp_del_group_from_dprm(uint64_t dp_handle, uint32_t group_id)
{
	return of_unregister_group(dp_handle, group_id);
}

int32_t sm_app_igmp_add_bucket_to_group(uint64_t dp_handle, uint32_t group_id, uint32_t bucket_id)
{
	struct ofi_bucket *bucket_entry;

	bucket_entry = (struct ofi_bucket *)calloc(1, sizeof(struct ofi_bucket));
	if (bucket_entry == NULL)	{
		OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,
		"%s:%d	allocating bucket to group node failed..\r\n", __FUNCTION__,__LINE__);
    	return OF_FAILURE;
	}
	
	bucket_entry->bucket_id = bucket_id;
	if (of_register_bucket_to_group(dp_handle, bucket_entry, group_id) != OF_SUCCESS)	{
		OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,
			"%s:%d bucket registration to dprm datapath's group failed...\r\n", __FUNCTION__,__LINE__);
		return OF_FAILURE;
	}
	return OF_SUCCESS;
}

int32_t sm_app_igmp_del_bucket_from_group(uint64_t dp_handle, uint32_t group_id, uint32_t bucket_id)
{
	return of_group_unregister_bucket(dp_handle, group_id, bucket_id);
}

int32_t sm_app_igmp_add_action_to_bucket(uint64_t dp_handle,
				 	uint32_t group_id, uint32_t bucket_id,
					uint16_t action_type, uint32_t action_val)
{
	struct ofi_action *action;

	action = (struct ofi_action *)calloc(1, sizeof(struct ofi_action));
	if (action == NULL)	{
		OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,
			"%s:%d	allocating bucket action failed..\r\n", __FUNCTION__,__LINE__);
		return OF_FAILURE;
	}

	action->type = action_type;
	action->max_len = OFPCML_NO_BUFFER;
	switch(action_type)	{
		case OFPAT_OUTPUT:
			action->port_no = action_val;
			break;
		case OFPAT_SET_MPLS_TTL:
			action->ttl = (uint8_t)action_val;
			break;			
		case OFPAT_SET_QUEUE:
			action->queue_id = action_val;
			break;
		case OFPAT_GROUP:
			action->group_id = action_val;
			break;
		case OFPAT_SET_NW_TTL:
			action->ttl = (uint8_t)action_val;
			break;
		case OFPAT_PUSH_VLAN:
		case OFPAT_POP_VLAN:
		case OFPAT_PUSH_MPLS:
		case OFPAT_POP_MPLS:
		case OFPAT_COPY_TTL_IN:
		case OFPAT_COPY_TTL_OUT:
		case OFPAT_DEC_NW_TTL:
		case OFPAT_SET_FIELD:
		case OFPAT_PUSH_PBB:
		case OFPAT_POP_PBB:
			break;
		default:
			OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,
			"%s:%d action is not supported..\r\n", __FUNCTION__,__LINE__);
			return OF_FAILURE;
	}
	return of_register_action_to_bucket(dp_handle, action, group_id, bucket_id);
}

static inline uint32_t sm_app_mcast_get_group_id()
{
	return ((mcast_group_id == 4095)?1:mcast_group_id++);
}

static inline  uint16_t sm_app_mcast_get_entry_priority()
{
	return ((mcast_rtentry_priority == 0xFFFE)?10:mcast_rtentry_priority++);
}

int32_t sm_app_igmp_del_mcast_entry(uint64_t dp_handle, uint16_t priority,
				uint32_t src_ip, uint32_t group_ip,
				uint32_t group_id, uint8_t table_id, uint16_t flags)
{
	struct of_msg *msg = NULL;
	uint16_t msg_len;
	uint16_t match_fd_len=0;
	uint8_t *match_start_loc = NULL;
	uint16_t	instruction_len = 0;
	uint8_t 	*inst_start_loc = NULL;
	uint16_t eth_type;
	int32_t retval = OF_SUCCESS;
	int32_t status = OF_SUCCESS;
	void *conn_info_p;
	
	msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+16+
			  OFU_ETH_TYPE_FIELD_LEN+OFU_IPV4_DST_FIELD_LEN+16);

	OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,"IGMP: del mcast entry..");
	do
	{
		msg = ofu_alloc_message(OFPT_FLOW_MOD, msg_len);
		if(!msg)	{
		  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF message alloc for flow-mod del failed.. ");
		  return OF_FAILURE;
		}

		#if 0
		ofu_start_setting_match_field_values(msg);
		eth_type = 0x0800; /*Eth type is IP*/
		retval = of_set_ether_type(msg, &eth_type);
		if (retval != OFU_SET_FIELD_SUCCESS)	{
			OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF Set IPv4 src field failed,err = %d",retval);
			status = OF_FAILURE;
			break;
		}

		if (src_ip)	{
			retval = of_set_ipv4_source(msg, &src_ip, FALSE, NULL);
			if (retval != OFU_SET_FIELD_SUCCESS)	{
			  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF Set IPv4 dest field failed,err = %d",retval);
			  status = OF_FAILURE;
			  break;
			}
		}
		retval = of_set_ipv4_destination(msg, &group_ip, FALSE, NULL);
		if (retval != OFU_SET_FIELD_SUCCESS)	{
		  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF Set IPv4 dest field failed,err = %d",retval);
		  status = OF_FAILURE;
		  break;
		}
		ofu_end_setting_match_field_values(msg,match_start_loc,&match_fd_len);
		OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, "Match field len %d",match_fd_len);
		#endif
		flags |= OFPFF_SEND_FLOW_REM;
        #if 0
		if (priority)
		{
			retval = of_create_delete_flow_entries_msg_of_table_with_strict_match(msg,
										dp_handle,
										table_id,
										priority,
										flags,
										0, /*cookie*/
										0, /*cookie_mask*/
										OFPP_NONE,
										group_id,
										&conn_info_p);

			if (retval != OFU_DELETE_FLOW_ENTRY_TO_TABLE_SUCCESS)
			{
				OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR, "deletion of flow_mod for tableid=%d failed",table_id);
				status = OF_FAILURE;
			}
		}
		else
		#endif
		{
			retval = of_create_delete_flow_entries_msg_of_table(msg,
									dp_handle,
									table_id,
									flags,
									0, /*cookie */
									0, /*cookie_mask */
									OFPP_NONE,
									0,
									&conn_info_p);
			if (retval != OFU_DELETE_FLOW_ENTRY_TO_TABLE_SUCCESS)
			{
				OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR, "deletion of flow_mod for tableid=%d failed",table_id);
				status = OF_FAILURE;
			}
		}

		ofu_start_setting_match_field_values(msg);
		eth_type = 0x0800; /*Eth type is IP*/
		retval = of_set_ether_type(msg, &eth_type);
		if (retval != OFU_SET_FIELD_SUCCESS)	{
			OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF Set IPv4 src field failed,err = %d",retval);
			status = OF_FAILURE;
			break;
		}

		if (src_ip)	{
			retval = of_set_ipv4_source(msg, &src_ip, FALSE, NULL);
			if (retval != OFU_SET_FIELD_SUCCESS)	{
			  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF Set IPv4 dest field failed,err = %d",retval);
			  status = OF_FAILURE;
			  break;
			}
		}
		retval = of_set_ipv4_destination(msg, &group_ip, FALSE, NULL);
		if (retval != OFU_SET_FIELD_SUCCESS)	{
		  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF Set IPv4 dest field failed,err = %d",retval);
		  status = OF_FAILURE;
		  break;
		}
		ofu_end_setting_match_field_values(msg,match_start_loc,&match_fd_len);
		OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, "Match field len %d",match_fd_len);

		ofu_start_pushing_instructions(msg, match_fd_len);
		ofu_end_pushing_instructions(msg, inst_start_loc, &instruction_len);

		retval=of_cntrlr_send_request_message(
		    msg,
		    dp_handle,
		    conn_info_p,
		    NULL,
		    NULL);
		if (retval != OF_SUCCESS)
		{
		  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error deleting flow entry in mcast route table");
		  status = OF_FAILURE;
          return status;
		}
	}while(0);

	if (status == OF_FAILURE) {
		msg->desc.free_cbk(msg);
	}

	return status;
}

int32_t sm_app_igmp_add_mcast_entry(uint64_t dp_handle, uint16_t priority,
				uint32_t src_ip, uint32_t group_ip,
				uint32_t group_id, uint8_t table_id, uint16_t flags)
{
	struct of_msg *msg = NULL;
	uint16_t msg_len;
	uint16_t instruction_len;
	uint8_t *inst_start_loc = NULL;
	uint16_t action_len;
	uint8_t *action_start_loc = NULL;
	uint16_t match_fd_len=0;
	uint8_t *match_start_loc = NULL;
	uint16_t eth_type;
	int32_t retval = OF_SUCCESS;
	int32_t status = OF_SUCCESS;
	void *conn_info_p;
	
	msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+16+
				OFU_ETH_TYPE_FIELD_LEN+OFU_IPV4_DST_FIELD_LEN+
				OFU_APPLY_ACTION_INSTRUCTION_LEN+OFU_GROUP_ACTION_LEN);
		
	do
	{
		OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,"IGMP: add mcast entry..");
		msg = ofu_alloc_message(OFPT_FLOW_MOD, msg_len);
		if (msg == NULL)	{
			OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF message alloc error ");
			status = OF_FAILURE;
			return status;
		}

		flags |= OFPFF_RESET_COUNTS;
	
		/* zero idle and hard timeout makes entry permanet */
		retval = of_create_add_flow_entry_msg(msg,
						dp_handle,
						table_id,
						priority, /*priority*/
                                                OFPFC_ADD,
						0, /*coockie*/
						0,/*Coockie amsk*/
						OFP_NO_BUFFER,
						flags,
						0, /*No Hard timeout*/
						0, /*No Idle timeout*/
						&conn_info_p
						);
	
		if (retval != OFU_ADD_FLOW_ENTRY_TO_TABLE_SUCCESS)	{
			OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error adding flow entry in route table = %d",retval);
			status = OF_FAILURE;
			break;
		}
	
		ofu_start_setting_match_field_values(msg);
		eth_type = 0x0800; /*Eth type is IP*/
		retval = of_set_ether_type(msg, &eth_type);
		if (retval != OFU_SET_FIELD_SUCCESS)	{
			OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF Set IPv4 src field failed,err = %d",retval);
			status = OF_FAILURE;
			break;
		}

		if (src_ip) {
			retval = of_set_ipv4_source(msg, &src_ip, FALSE, NULL);
			if (retval != OFU_SET_FIELD_SUCCESS)	{
			  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF Set IPv4 dest field failed,err = %d",retval);
			  status = OF_FAILURE;
			  break;
			}
		}

		retval = of_set_ipv4_destination(msg, &group_ip, FALSE, NULL);
		if (retval != OFU_SET_FIELD_SUCCESS)	{
		  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF Set IPv4 dest field failed,err = %d",retval);
		  status = OF_FAILURE;
		  break;
		}
		ofu_end_setting_match_field_values(msg,match_start_loc,&match_fd_len);
		OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, "Match field len %d",match_fd_len);
	
		ofu_start_pushing_instructions(msg, match_fd_len);
		retval = ofu_start_pushing_apply_action_instruction(msg);
		if (retval != OFU_INSTRUCTION_PUSH_SUCCESS) {
		  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in adding apply action instruction err = %d",retval);
		  status = OF_FAILURE;
		  break;
		}
	
		retval = ofu_push_group_action(msg, group_id);
		if (retval != OFU_ACTION_PUSH_SUCCESS)	{
			OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in dec ttl value	err = %d",retval);
			status = OF_FAILURE;
			break;
		}
		ofu_end_pushing_actions(msg,action_start_loc,&action_len);
		((struct ofp_instruction_actions*)(msg->desc.ptr2))->len = htons(action_len);
		OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"total actions len = %d for this instruction", action_len);
	
		ofu_end_pushing_instructions(msg,inst_start_loc,&instruction_len);
		OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"instruction len = %d", instruction_len);
	
		retval = of_cntrlr_send_request_message(msg, dp_handle, conn_info_p, NULL, NULL);
		if (retval != OF_SUCCESS)	{
			OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error adding flow entry in route table = %d",retval);
			status = OF_FAILURE;
            return status;
		}
	  }
	  while(0);

	  if (status == OF_FAILURE) {
		msg->desc.free_cbk(msg);
	  }

	  return status;
}


int32_t sm_app_igmp_process_membership_report(uint64_t dp_handle, uint32_t dp_index,
						uint32_t group_addr, uint32_t in_port)
{
	uint32_t	ii, port_index;
	uint32_t	group_id, bucket_id;
	uint16_t	flow_id;
	int32_t		status = OF_SUCCESS;


	do
	{
		OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,"IGMP: membership report..");
		if ((ii = sm_app_mcast_get_entry_by_grp_address(dp_index, group_addr)) == OF_FAILURE)
		{
			ii = sm_app_mcast_get_free_entry(dp_index);
			if (ii == OF_FAILURE)	{
		           OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"IGMP: Free entry not found..");
				ii = entry_index;/* TABLE is full overwrite least recently used */
				entry_index++;
				if (entry_index == OF_CNTLR_APP_MAX_MCAST_ROUTE_ENTRIES)
					entry_index = 0;
			}
			else
				dp_conf[dp_index].no_of_mcast_route_entries++;
		
		        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"IGMP:");
			dp_conf[dp_index].mcast_rt_table[ii].grp_addr = group_addr;
			dp_conf[dp_index].mcast_rt_table[ii].port_info[0].port_id = in_port;
			dp_conf[dp_index].mcast_rt_table[ii].port_info[0].binuse = TRUE;
			dp_conf[dp_index].mcast_rt_table[ii].binuse = TRUE;
			dp_conf[dp_index].mcast_rt_table[ii].no_of_outports = 1;

			group_id = sm_app_mcast_get_group_id();
			dp_conf[dp_index].mcast_rt_table[ii].grp_id = group_id;
			flow_id = sm_app_mcast_get_entry_priority();
			dp_conf[dp_index].mcast_rt_table[ii].flow_id = flow_id;

			if (sm_app_igmp_add_group_to_dprm(dp_handle,
							OFPGT_ALL, group_id) != OF_SUCCESS)
			{
				status = OF_FAILURE;
				break;
			}

			bucket_id = dp_conf[dp_index].mcast_rt_table[ii].no_of_outports;
 			if (sm_app_igmp_add_bucket_to_group(dp_handle,
							group_id, bucket_id) != OF_SUCCESS)
			{
				status = OF_FAILURE;
				break;
			}
			dp_conf[dp_index].mcast_rt_table[ii].port_info[0].bucket_id = bucket_id;
			
			if (sm_app_igmp_add_action_to_bucket(dp_handle,
							group_id, bucket_id,
							OFPAT_OUTPUT, in_port) != OF_SUCCESS)
			{
				status = OF_FAILURE;
				break;
			}
		
			if (sm_app_igmp_add_mcast_entry(dp_handle, flow_id, 0,
						 group_addr, group_id, dp_conf[dp_index].mcast_routing_table_id, 0) != OF_SUCCESS)
			{
				OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"IGMP:adding flow entry using flowmod msg failed");
				status = OF_FAILURE;
				break;
			}

			if (of_group_frame_and_send_message_request(dp_handle,
								group_id, ADD_GROUP) != OF_SUCCESS)
			{
				OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"IGMP:sending groupmod add msg failed");
				status = OF_FAILURE;
				break;
			}
		}
		else	{
				OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"IGMP:");
			port_index = sm_app_mcast_get_portentry_by_id(dp_index, ii, in_port);
			if (port_index == OF_FAILURE)	{
				port_index = sm_app_mcast_get_free_portentry(dp_index, ii);
		
				dp_conf[dp_index].mcast_rt_table[ii].port_info[port_index].port_id = in_port;
				dp_conf[dp_index].mcast_rt_table[ii].port_info[port_index].binuse = TRUE;
				dp_conf[dp_index].mcast_rt_table[ii].no_of_outports++;
				group_id = dp_conf[dp_index].mcast_rt_table[ii].grp_id;
				bucket_id = dp_conf[dp_index].mcast_rt_table[ii].no_of_outports;

				if (sm_app_igmp_add_bucket_to_group(dp_handle,
								group_id, bucket_id) != OF_SUCCESS)
				{
					status = OF_FAILURE;
					break;
				}

				if (sm_app_igmp_add_action_to_bucket(dp_handle,
								group_id, bucket_id,
								OFPAT_OUTPUT, in_port) != OF_SUCCESS)
				{
					status = OF_FAILURE;
					break;
				}
				dp_conf[dp_index].mcast_rt_table[ii].port_info[port_index].bucket_id = bucket_id;

				if (of_group_frame_and_send_message_request(dp_handle,
								group_id, MOD_GROUP) != OF_SUCCESS)
				{
					OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"IGMP:sending groupmod modify msg failed");
					status = OF_FAILURE;
					break;
				}
			}
                        else
                        {
			   OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"IGMP: Memeber is already joined");

                        }
		}
	}while(0);

	return status;
}


int32_t sm_app_igmp_process_leave(uint64_t dp_handle, uint32_t dp_index,
				uint32_t group_addr, uint32_t in_port)
{
	int32_t	ii, port_index;
	uint32_t	group_id, bucket_id;
	uint16_t	flow_id;
	int32_t		status = OF_SUCCESS;

	do
	{
		OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,"IGMP: processing leave");
		if ((ii = sm_app_mcast_get_entry_by_grp_address(dp_index, group_addr)) != OF_FAILURE)
		{
			port_index = sm_app_mcast_get_portentry_by_id(dp_index, ii, in_port);
			if (port_index != OF_FAILURE)	{
				//dp_conf[dp_index].mcast_rt_table[ii].port_info[port_index].binuse = FALSE;
				//dp_conf[dp_index].mcast_rt_table[ii].no_of_outports--;
				group_id = dp_conf[dp_index].mcast_rt_table[ii].grp_id;
				bucket_id = dp_conf[dp_index].mcast_rt_table[ii].port_info[port_index].bucket_id;

				if (sm_app_igmp_del_bucket_from_group(dp_handle,
								 group_id, bucket_id) != OF_SUCCESS)
				{
					status = OF_FAILURE;
					break;
				}

		
				if ((dp_conf[dp_index].mcast_rt_table[ii].no_of_outports - 1) == 0) {
					dp_conf[dp_index].mcast_rt_table[ii].binuse = FALSE;
					dp_conf[dp_index].no_of_mcast_route_entries--;
				
					if (of_group_frame_and_send_message_request(dp_handle,
								group_id, DEL_GROUP) != OF_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,
							"IGMP:sending groupmod del msg failed for leave msg");
						status = OF_FAILURE;
						break;
					}
					sm_app_igmp_del_group_from_dprm(dp_handle, group_id);

					flow_id = dp_conf[dp_index].mcast_rt_table[ii].flow_id;	
					if (sm_app_igmp_del_mcast_entry(dp_handle, flow_id, 0,
                                              group_addr, group_id, dp_conf[dp_index].mcast_routing_table_id, 0) != OF_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"IGMP:sending flowmod del msg failed");
						status = OF_FAILURE;
						break;
					}	
				}
				else	{
					if (of_group_frame_and_send_message_request(dp_handle,
								group_id, MOD_GROUP) != OF_SUCCESS)
					{
						OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,
							"IGMP:sending groupmod modify msg failed for igmp leave");
						status = OF_FAILURE;
						break;
					}
				}
				dp_conf[dp_index].mcast_rt_table[ii].port_info[port_index].binuse = FALSE;
				dp_conf[dp_index].mcast_rt_table[ii].no_of_outports--;
			}
		}
		else {
			OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,
				"IGMPLEAVE: no entry found with grp-add=%x",group_addr);
		}

	}while(0);
	return status;
}

int32_t
sm_app_igmp_pkt_in_rcvd(int64_t controller_handle,
                     uint64_t domain_handle,
                     uint64_t datapath_handle,
                     struct of_msg     *msg,
                     struct ofl_packet_in  *pkt_in,
                     void   *cbk_arg1,
                     void   *cbk_arg2)
{
	struct dprm_datapath_entry *pdatapath_info;
	uint32_t group_addr, dp_index, in_port;
	uint16_t igmp_type;
	uint8_t	*pkt = pkt_in->packet;
	uint8_t	protocol;
	int32_t retval = OF_SUCCESS;
	int32_t status = OF_SUCCESS;

	OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG," PKT-IN RECEIVED FOR IGMP PKT..");
	if (get_datapath_byhandle(datapath_handle, &pdatapath_info) != DPRM_SUCCESS)	{
		OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"No DP Entry with given datapath_handle=%llx",datapath_handle);
		if (msg != NULL)	{
     of_get_pkt_mbuf_by_of_msg_and_free(msg);
//			msg->desc.free_cbk(msg);
		}
		return OF_ASYNC_MSG_CONSUMED;
	}
 
	do
	{
		for (dp_index = 0; dp_index < no_of_data_paths_attached_g; dp_index++)	{
			if (dp_conf[dp_index].dp_handle == datapath_handle)
				break;
		}
  
		if (dp_index == no_of_data_paths_attached_g)	{
			OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,
				"No datapath found with given dp handle=%llx",datapath_handle);
			status = OF_FAILURE;
			break;
		}
		OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"PKT-IN RECEIVED FOR IGMP PACKET...");
		OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"Received pkt of length %d",pkt_in->packet_length);
  
		protocol = *((uint8_t *)(pkt + OF_CNTLR_IPV4_HDR_OFFSET + OF_CNTLR_IPV4_PROTO_OFFSET));
		if (unlikely(protocol != OF_CNTLR_IP_PROTO_IGMP))	{
			OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"Non IGMP packet received...",datapath_handle);
			status = OF_FAILURE;
			break;
		} 
		igmp_type = *((uint8_t*)(pkt + OF_CNTLR_IGMP_HEADER_OFFSET(pkt)));

		retval = ofu_get_in_port_field(msg, pkt_in->match_fields,
					pkt_in->match_fields_length, &in_port);
		if (retval != OFU_IN_PORT_FIELD_PRESENT)	{
			OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,
				"%s:failed to get inport ....inport=%d",__FUNCTION__,in_port);
			status = OF_FAILURE;
			break;
		}

		switch (igmp_type)	{
			case IGMPV1_MEMBERSHIP_REPORT:
			case IGMPV2_MEMBERSHIP_REPORT:
				group_addr = ntohl(*(uint32_t*)(pkt + 
				   OF_CNTLR_IGMP_HEADER_OFFSET(pkt) + 
				   OF_CNTLR_IGMPV2_GROUP_ADDR_OFFSET));
				goto process_join;
			case IGMPV3_MEMBERSHIP_REPORT:
				group_addr = ntohl(*(uint32_t*)(pkt + 
					OF_CNTLR_IGMP_HEADER_OFFSET(pkt) + 
					OF_CNTLR_IGMPV3_GROUP_ADDR_OFFSET));
process_join:
				if (sm_app_igmp_process_membership_report(datapath_handle, dp_index,
									  group_addr, in_port) != OF_SUCCESS)
				{
					status = OF_FAILURE;
					OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,
						"processing of IGMP join failed.grp-ip=%x.inport=%d.",group_addr,in_port);
				}
				break;
			case IGMPV2_LEAVE_GROUP:
				group_addr = ntohl(*(uint32_t*)(pkt + OF_CNTLR_IGMP_HEADER_OFFSET(pkt) + 
									OF_CNTLR_IGMPV2_GROUP_ADDR_OFFSET));
				if (sm_app_igmp_process_leave(datapath_handle, dp_index, group_addr, in_port) != OF_SUCCESS)
				{
					OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,
						"processiong of IGMP leave failed.grp-ip=%x inport=%d",group_addr,in_port);
					status = OF_FAILURE;
				}
				break;
			default:
				OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"Invalid IGMP packet received.type=%x.",igmp_type);
		}
	
	}
	while(0);

	if (msg != NULL)	{
     of_get_pkt_mbuf_by_of_msg_and_free(msg);
//		msg->desc.free_cbk(msg);
	}
	return OF_ASYNC_MSG_CONSUMED;
}


int32_t sm_app_add_mcast_proactive_entry(uint64_t datapath_handle,
					uint32_t *mcast_ip, uint32_t *mask, uint8_t *protocol,
					uint32_t dp_index, uint16_t priority,
					uint64_t  cookie, uint64_t  cookie_mask,
					uint32_t  buffer_id, uint16_t  flags)

{
	  struct	  of_msg *msg;
	  uint16_t	  msg_len;
	  uint16_t	  instruction_len;
	  uint8_t	  *inst_start_loc = NULL;
	  uint16_t	  action_len, eth_type = 0x0800;
	  uint8_t	  *action_start_loc = NULL;
	  uint16_t	  match_fd_len=0;
	  uint8_t	  *match_start_loc = NULL;
	  int32_t	  retval = OF_SUCCESS;
	  int32_t	  status = OF_SUCCESS;
	  void *conn_info_p;
          uint8_t mcst_mac[6]={0x01,0,0x5e,0,0,0};
          uint8_t mcst_mask[6]={0xFF,0xFF,0xFF,0,0,0};
 

	  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"entered");
	  if (*protocol == OF_IPPROTO_IGMP)
	  { 
		msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+16+
			OFU_ETH_TYPE_FIELD_LEN+OFU_IPV4_DST_MASK_LEN+OFU_IP_PROTO_FIELD_LEN+
			OFU_APPLY_ACTION_INSTRUCTION_LEN+OFU_OUTPUT_ACTION_LEN);
	  }
	  else
	  {
		msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+16+
			OFU_ETH_TYPE_FIELD_LEN+OFU_IPV4_DST_MASK_LEN+OFU_IP_PROTO_FIELD_LEN+
			OFU_GOTO_TABLE_INSTRUCTION_LEN);
	  }
	
	  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"Msg Len %d\r\n",msg_len);
	  do
	  {
		msg = ofu_alloc_message(OFPT_FLOW_MOD, msg_len);
		if (msg == NULL)
		{
		  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d OF message alloc error \r\n",__FUNCTION__,__LINE__);
		  status = OF_FAILURE;
		  return status;
		}
	
		flags |= OFPFF_RESET_COUNTS;
		retval = of_create_add_flow_entry_msg(msg,
			datapath_handle,
			dp_conf[dp_index].mcast_class_table_id, 
			priority, 
                        OFPFC_ADD,
			cookie,
			cookie_mask,
			OFP_NO_BUFFER,
			flags,
			0, /*No Hard timeout*/
			0, /* No Idle timeout*/
			&conn_info_p
			);
	
		if (retval != OFU_ADD_FLOW_ENTRY_TO_TABLE_SUCCESS)
		{
		  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"Error in adding arp flow entry  = %d",
			  retval);
		  status = OF_FAILURE;
		  break;
		}
	
		ofu_start_setting_match_field_values(msg);
                retval = of_set_ether_type(msg, &eth_type);
		if (retval != OFU_SET_FIELD_SUCCESS)
		{
		  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,
		        "%s:%d OF Set IPv4 src field failed,err = %d\r\n",__FUNCTION__,__LINE__,retval);
        	    status = OF_FAILURE;
		    break;
	        }

		retval =  of_set_destination_mac(msg,
				mcst_mac,
				TRUE,
				mcst_mask);
		if( retval != OFU_SET_FIELD_SUCCESS)
		{
			OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,
					"OF Set MAc field failed,err = %d",retval);
			status = OF_FAILURE;
			break;
		}

		if (*mcast_ip){
		  retval = of_set_ipv4_destination(msg, mcast_ip, ((*mask)?TRUE:FALSE), mask);
		  if (retval != OFU_SET_FIELD_SUCCESS)
		  {
		    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,
			"%s:OF Set DEST MCAST field failed,mcastip = %x\r\n",__FUNCTION__,mcast_ip);
		    status = OF_FAILURE;
		    break;
		  }
		}
		
		retval = of_set_protocol(msg, protocol);
		if (retval != OFU_SET_FIELD_SUCCESS)
		{
		  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:OF Set IPPROTO field failed,mcastip = %d\r\n",__FUNCTION__,protocol);
		  status = OF_FAILURE;
		  break;
		}
		ofu_end_setting_match_field_values(msg,match_start_loc,&match_fd_len);

		if (*protocol == OF_IPPROTO_IGMP) /* if packet is IGMP send it to Controller */
		{ 
		  ofu_start_pushing_instructions(msg, match_fd_len);
		  retval = ofu_start_pushing_apply_action_instruction(msg);
		  if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)
		  {
			OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d Error in adding apply action instruction err = %d\r\n",
				__FUNCTION__,__LINE__,retval);
			status = OF_FAILURE;
			break;
		  }
	
		  retval = ofu_push_output_action(msg, OFPP_CONTROLLER, OFPCML_NO_BUFFER);
		  if (retval != OFU_ACTION_PUSH_SUCCESS)
		  {
			OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d Error in adding output action\r\n",
				__FUNCTION__,__LINE__);
			status = OF_FAILURE;
			break;
		  }
		  ofu_end_pushing_actions(msg,action_start_loc,&action_len);
		  ((struct ofp_instruction_actions*)(msg->desc.ptr2))->len = htons(action_len);
		  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"total actions len = %d for this instruction", action_len);
	
		  ofu_end_pushing_instructions(msg,inst_start_loc,&instruction_len);
		}
		else if (*protocol == OF_IPPROTO_UDP) /* if it is a mcast data packet  goto mcast route table */
		{
		  ofu_start_pushing_instructions(msg, match_fd_len);
		  retval = ofu_push_go_to_table_instruction(msg, dp_conf[dp_index].mcast_routing_table_id);
		  if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)
		  {
			OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d go to table instruction failed \r\n",
				__FUNCTION__,__LINE__);
			status = OF_FAILURE;
			break;
		  }
		  ofu_end_pushing_instructions(msg, inst_start_loc,&instruction_len);
		} 
	
		retval=of_cntrlr_send_request_message(
			msg,
			datapath_handle,
			conn_info_p,
			NULL,
			NULL);
		if (retval != OF_SUCCESS)
		{
		  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d  Error in adding arp flow entry \r\n",
			  __FUNCTION__,__LINE__);
		  status = OF_FAILURE;
                  return status;
		}
	  }
	  while(0);
	
	  return status;
}


int32_t sm_app_igmp_dp_ready_handler(uint64_t dp_handle, uint32_t dp_index)
{
	uint8_t	protocol;
	uint32_t mcast_ip, mcast_mask;
	uint16_t flow_priority = 1000; /*highest number is highest priority */
	int32_t status = OF_SUCCESS;

	do	{
		/*  Add one missentry and two flow entries in Mcast classifier TABLE(i.e TABLE-1) */
		protocol = OF_IPPROTO_IGMP;
		mcast_ip = 0;
		mcast_mask = 0;
		if (sm_app_add_mcast_proactive_entry(dp_handle,
						&mcast_ip, &mcast_mask, &protocol,
                                                dp_index,
						++flow_priority,
						0, 0, /*  cookie, cookie mask */
						0, 0 /* buffer-id, flags */
						) != OF_SUCCESS)
		{
		  status = OF_FAILURE;
		  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," ## failed adding proactive mcast entry with proto=%d ",protocol);
		  break;
		}

		#if 0
		mcast_ip = 0;
		mcast_mask = 0;
		if (sm_app_add_mcast_proactive_entry(dp_handle,
						&mcast_ip, &mcast_mask, &protocol,
						SM_APP_MCAST_CLASSIFIER_TABLE_ID,
						++flow_priority,
						0, 0, /*  cookie, cookie mask */
						0, 0 /* buffer-id, flags */
						) != OF_SUCCESS)
		{
		  status = OF_FAILURE;
		  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," ## failed adding proactive mcast entry with proto=%d ",protocol);
		  break;
		}

		#endif
		protocol = OF_IPPROTO_UDP;
		if (sm_app_add_mcast_proactive_entry(dp_handle,
						&mcast_ip, &mcast_mask, &protocol,
						dp_index,
						++flow_priority,
						0, 0, /*  cookie, cookie mask */
						0, 0 /* buffer-id, flags */
						) != OF_SUCCESS)
		{
			status = OF_FAILURE;
			OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," failed adding proact mcast entry with proto=%d ",protocol);
		}

		/* Add miss entries in mcast classifier table 
		 * so that if pkt is Mcast data pkt then send it to next table i.e mcast routing table*/
		if (sm_app_add_missentry_with_goto_table_instruction(dp_handle,
		      dp_conf[dp_index].mcast_class_table_id, /* add entry to this table */
		      dp_conf[dp_index].mcast_routing_table_id) != OF_SUCCESS)
		{
		  status = OF_FAILURE;
		  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"adding miss entries in mcast classify table failed### ");
		  break;
		}

		/* Add miss entries in mcast routing table 
		 * so that if pkt is neither IGMP nor Mcast data pkt then send it to next table i.e SESSION table*/
		if (sm_app_add_missentry_with_goto_table_instruction(dp_handle,
		      dp_conf[dp_index].mcast_routing_table_id, /* add entry to this table */
		      dp_conf[dp_index].session_table_id) != OF_SUCCESS)
		{
		  status = OF_FAILURE;
		  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"adding miss entries in mcast routing table failed ####");
		  break;
		}

		status = dprm_register_datapath_notifications(DPRM_DATAPATH_PORT_ADDED,
                                             sm_app_igmp_port_ready_cbk,
                                                   NULL,
                                                   NULL);
		if (status != DPRM_SUCCESS)
		{
		   status = OF_FAILURE;
		   OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"dprm_register_datapath_notifications to add event failed ####");
		   break;
		}

		//sm_app_igmp_send_gen_query(dp_handle, dp_index);
	}while(0);
	return status;
}


void sm_app_igmp_port_ready_cbk(uint32_t notification_type,
                                  uint64_t dp_handle,
                                  struct   dprm_datapath_notification_data notification_data,
                                  void     *callback_arg1,
                                  void     *callback_arg2)
{
	uint32_t dp_index;

	OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,
		"%s:port add event received from dprm with port-name=%s handle=%llx",
		__FUNCTION__,notification_data.port_name, dp_handle);
	if (notification_type != DPRM_DATAPATH_PORT_ADDED)
		return;

	for (dp_index = 0; dp_index < OF_CNTLR_MAX_DATA_PATHS; dp_index++)	{
		if (dp_conf[dp_index].dp_handle == dp_handle)	{
			break;
		}
	}
	
	if (dp_index == OF_CNTLR_MAX_DATA_PATHS)	{
		OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,
			"%s:no datapath found with handle=%llx",__FUNCTION__,dp_handle);
		return;
	}

	sm_app_igmp_send_gen_query(dp_handle, dp_index,
			notification_data.port_id, notification_data.port_name);
}


