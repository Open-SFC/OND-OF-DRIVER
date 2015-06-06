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

/*File: of_l3ext_utils.c
 *Author: Kumara Swamy Ganji <kumar.sg@freescale.com>
 * Description:
 * This file contails NB APIs of experimenter L3 extension instructions 
 */

//#ifdef FSLX_COMMON_AND_L3_EXT_SUPPORT
#include "controller.h"
#include "of_utilities.h"
#include "cntlr_tls.h"
#include "cntrl_queue.h"
#include "cntlr_transprt.h"
#include "cntlr_event.h"
#include "dprmldef.h"
#include "of_utilities.h"
//#include "of_l3ext_utils.h"
#include "fsl_ext.h"
#include "oflog.h"


unsigned char is_fsl_extensions_loaded_g=FALSE;
void cntlr_shared_lib_init(void)
{

	OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_DEBUG,"loading fsl extentions library");
	is_fsl_extensions_loaded_g=TRUE;
	return;
}

/* Experimenter Actions */
	int32_t
fslx_action_write_metadata_from_pkt(struct of_msg *msg,
		uint32_t match_field_id, uint64_t mask)
{
	struct of_msg_descriptor *msg_desc;
	struct fslx_action_wr_metadata_from_pkt *wr_metadata;

	msg_desc = &msg->desc;

	if ((msg_desc->ptr3+msg_desc->length3 + 
				(FSLX_ACTION_WRITE_METADATA_FROM_PKT_LEN)) >
			(msg->desc.start_of_data + msg_desc->buffer_len))
	{
		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN, "Length of buffer is not suffeceient to add data");
		return OFU_NO_ROOM_IN_BUFFER;
	}

	OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_DEBUG,"mesg length%d",msg_desc->length3);

	//msg_desc->ptr3 = msg_desc->ptr3+msg_desc->length3;
	//msg_desc->length3=0;
	//exp_inst = (struct ofp_instruction_experimenter *)(msg_desc->ptr2+msg_desc->length2);
	//wr_metadata = (struct fslx_action_wr_metadata_from_pkt *)
	//              (msg_desc->ptr2+msg_desc->length2+OFU_EXPERIMENTER_INSTRUCTION_LEN);
	wr_metadata = 
		(struct fslx_action_wr_metadata_from_pkt *)(msg_desc->ptr3+msg_desc->length3);

	wr_metadata->type = htons(OFPAT_EXPERIMENTER);
	wr_metadata->len = htons(FSLX_ACTION_WRITE_METADATA_FROM_PKT_LEN);
	wr_metadata->vendor= htonl(FSLX_VENDOR_ID);
	wr_metadata->subtype = htons(FSLXAT_WRITE_METADATA_FROM_PKT);
	wr_metadata->field_id = htonl(match_field_id);
	wr_metadata->maskval = htonll(mask);

	msg_desc->length3 += (FSLX_ACTION_WRITE_METADATA_FROM_PKT_LEN);
	return OFU_ACTION_PUSH_SUCCESS;
}

	int32_t
fslx_action_write_metadata(struct of_msg *msg, uint64_t data)
{
	struct of_msg_descriptor *msg_desc;
	struct fslx_action_write_metadata *wr_metadata;

	msg_desc = &msg->desc;

	if ((msg_desc->ptr3+msg_desc->length3 +
				(FSLX_ACTION_WRITE_METADATA_LEN)) >
			(msg->desc.start_of_data + msg_desc->buffer_len))
	{
		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN, "Length of buffer is not suffeceient to add data");
		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN, "Length required %d length passed %d", (msg->desc.start_of_data + msg_desc->buffer_len), (msg_desc->ptr3+msg_desc->length3 +
					(FSLX_ACTION_WRITE_METADATA_LEN)));

		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN, "buffer len %d Length3 %d metadata %d ", msg_desc->buffer_len, msg_desc->length3 ,FSLX_ACTION_WRITE_METADATA_LEN);

		return OFU_NO_ROOM_IN_BUFFER;
	}

	OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_DEBUG,"mesg length%d",msg_desc->length3);

	wr_metadata =
		(struct fslx_action_write_metadata *)(msg_desc->ptr3+msg_desc->length3);

	wr_metadata->type = htons(OFPAT_EXPERIMENTER);
	wr_metadata->len = htons(FSLX_ACTION_WRITE_METADATA_LEN);
	wr_metadata->vendor= htonl(FSLX_VENDOR_ID);
	wr_metadata->subtype = htons(FSLXAT_WRITE_META_DATA);
	wr_metadata->meta_data = htonll(data);

	msg_desc->length3 += (FSLX_ACTION_WRITE_METADATA_LEN);
	return OFU_ACTION_PUSH_SUCCESS;
}


	int32_t
fslx_action_ipfragment(struct of_msg *msg, uint32_t mtu_reg,uint8_t offset, uint8_t n_bits, uint32_t pmtu_val)
{
	struct of_msg_descriptor *msg_desc;
	//struct ofp_action_experimenter_header *exp_action;
	struct fslx_action_ip_fragment *ip_frag_action;

	msg_desc = &msg->desc;
	if ((msg_desc->ptr3+msg_desc->length3 + (FSLX_ACTION_IP_FRAGMENT_LEN)) >
			(msg->desc.start_of_data + msg_desc->buffer_len))
	{
		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN, "Length of buffer is not suffeceient to add data");
		return OFU_NO_ROOM_IN_BUFFER;
	}

	//exp_action = (struct ofp_action_experimenter_header *)(msg_desc->ptr3+msg_desc->length3);
	ip_frag_action = (struct fslx_action_ip_fragment *)(msg_desc->ptr3+msg_desc->length3);

	ip_frag_action->type = htons(OFPAT_EXPERIMENTER);
	ip_frag_action->len = htons(FSLX_ACTION_IP_FRAGMENT_LEN);
	ip_frag_action->vendor = htonl(FSLX_VENDOR_ID);
	ip_frag_action->subtype = htons(FSLXAT_IP_FRAGMENT);
	if (pmtu_val != 0 )
	{
		ip_frag_action->mtu = htonl(pmtu_val);
	}
	else
	{
		ip_frag_action->mtu = 0;
		ip_frag_action->mtu_reg = htonl(mtu_reg);
		ip_frag_action->offset=offset;
		ip_frag_action->nbits=n_bits;

	}
	msg_desc->length3 += (FSLX_ACTION_IP_FRAGMENT_LEN);
	return OFU_ACTION_PUSH_SUCCESS;
}


#if 0
	int32_t
fslx_action_set_in_port(struct of_msg *msg, uint32_t port)
{
	struct of_msg_descriptor *msg_desc;
	struct fslx_action_set_in_port_cntxt *in_port;

	msg_desc = &msg->desc;
	if((msg_desc->ptr3+msg_desc->length3 + (FSLX_ACTION_SET_IN_PORT_LEN)) >
			(msg->desc.start_of_data + msg_desc->buffer_len))
	{
		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN, "Length of buffer is not suffeceient to add data");
		return OFU_NO_ROOM_IN_BUFFER;
	}

	//exp_action = (struct ofp_action_experimenter_header *)(msg_desc->ptr3+msg_desc->length3);
	in_port = 
		(struct fslx_action_set_in_port_cntxt *)(msg_desc->ptr3+msg_desc->length3);

	in_port->type = htons(OFPAT_EXPERIMENTER);
	in_port->len = htons(FSLX_ACTION_SET_IN_PORT_LEN);
	in_port->vendor = htonl(FSLX_VENDOR_ID);
	in_port->subtype = htons(FSLXAT_SET_IN_PORT_FIELD_CNTXT);
	in_port->in_port = htonl(port);

	msg_desc->length3 += (FSLX_ACTION_SET_IN_PORT_LEN);
	return OFU_ACTION_PUSH_SUCCESS;
}
#endif


	int32_t
fslx_action_set_in_phy_port(struct of_msg *msg, uint32_t in_phy_port)
{
	struct of_msg_descriptor *msg_desc;
	struct fslx_action_set_in_phy_port_cntxt *phy_port;

	msg_desc = &msg->desc;
	if ((msg_desc->ptr3+msg_desc->length3 + (FSLX_ACTION_SET_IN_PHY_PORT_LEN)) >
			(msg->desc.start_of_data + msg_desc->buffer_len))
	{
		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN, "Length of buffer is not suffeceient to add data");
		return OFU_NO_ROOM_IN_BUFFER;
	}

	//exp_action = (struct ofp_action_experimenter_header *)(msg_desc->ptr3+msg_desc->length3);
	phy_port = 
		(struct fslx_action_set_in_phy_port_cntxt *)(msg_desc->ptr3+msg_desc->length3);

	phy_port->type = htons(OFPAT_EXPERIMENTER);
	phy_port->len = htons(FSLX_ACTION_SET_IN_PHY_PORT_LEN);
	phy_port->vendor = htonl(FSLX_VENDOR_ID);
	phy_port->subtype = htons(FSLXAT_SET_PHY_PORT_FIELD_CNTXT);
	phy_port->in_phy_port = htonl(in_phy_port);

	msg_desc->length3 += (FSLX_ACTION_SET_IN_PHY_PORT_LEN);
	return OFU_ACTION_PUSH_SUCCESS;
}


	int32_t
fslx_action_set_metadata(struct of_msg *msg, uint64_t meta_data)
{
	struct of_msg_descriptor *msg_desc;
	struct fslx_action_meta_data_cntxt *metadata_cntxt;

	msg_desc = &msg->desc;
	if ((msg_desc->ptr3+msg_desc->length3 + (FSLX_ACTION_SET_METADATA_LEN)) >
			(msg->desc.start_of_data + msg_desc->buffer_len))
	{
		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN, "Length of buffer is not suffeceient to add data");
		return OFU_NO_ROOM_IN_BUFFER;
	}

	//exp_action = (struct ofp_action_experimenter_header *)(msg_desc->ptr3+msg_desc->length3);
	metadata_cntxt = 
		(struct fslx_action_meta_data_cntxt *)(msg_desc->ptr3+msg_desc->length3);

	metadata_cntxt->type = htons(OFPAT_EXPERIMENTER);
	metadata_cntxt->len = htons(FSLX_ACTION_SET_METADATA_LEN);
	metadata_cntxt->vendor = htonl(FSLX_VENDOR_ID);
	metadata_cntxt->subtype = htons(FSLXAT_SET_META_DATA_FIELD_CNTXT);
	metadata_cntxt->meta_data = htonll(meta_data);

	msg_desc->length3 += (FSLX_ACTION_SET_METADATA_LEN);
	return OFU_ACTION_PUSH_SUCCESS;
}

	int32_t
fslx_action_set_tunnel_id(struct of_msg *msg, uint64_t tunnel_id)
{
	struct of_msg_descriptor *msg_desc;
	struct fslx_action_tunnel_id_cntxt *tunnel_id_cntxt;

	msg_desc = &msg->desc;
	if((msg_desc->ptr3+msg_desc->length3 + (FSLX_ACTION_SET_TUNNEL_ID_LEN)) >
			(msg->desc.start_of_data + msg_desc->buffer_len)) {
		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN, "Length of buffer is not suffeceient to add data");
		return OFU_NO_ROOM_IN_BUFFER;
	}

	tunnel_id_cntxt = 
		(struct fslx_action_tunnel_id_cntxt *)(msg_desc->ptr3+msg_desc->length3);

	tunnel_id_cntxt->type = htons(OFPAT_EXPERIMENTER);
	tunnel_id_cntxt->len = htons(FSLX_ACTION_SET_TUNNEL_ID_LEN);
	tunnel_id_cntxt->vendor = htonl(FSLX_VENDOR_ID);
	tunnel_id_cntxt->subtype = htons(FSLXAT_SET_TUNNEL_ID_FIELD_CNTXT);
	tunnel_id_cntxt->tunnel_id = htonll(tunnel_id);

	msg_desc->length3 += (FSLX_ACTION_SET_TUNNEL_ID_LEN);
	return OFU_ACTION_PUSH_SUCCESS;
}


	int32_t
fslx_action_start_from_table(struct of_msg *msg, uint8_t table_id)
{
	struct of_msg_descriptor *msg_desc;
	struct fslx_action_start_table *table_id_cntxt;

	msg_desc = &msg->desc;
	if((msg_desc->ptr3+msg_desc->length3 + (FSLX_ACTION_START_TABLE_LEN)) >
			(msg->desc.start_of_data + msg_desc->buffer_len)) {
		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN, "Length of buffer is not suffeceient to add data");
		return OFU_NO_ROOM_IN_BUFFER;
	}

	table_id_cntxt = 
		(struct fslx_action_start_table *)(msg_desc->ptr3+msg_desc->length3);

	table_id_cntxt->type = htons(OFPAT_EXPERIMENTER);
	table_id_cntxt->len = htons(FSLX_ACTION_START_TABLE_LEN);
	table_id_cntxt->vendor = htonl(FSLX_VENDOR_ID);
	table_id_cntxt->subtype = htons(FSLXAT_START_TABLE);
	table_id_cntxt->table_id = table_id;

	msg_desc->length3 += (FSLX_ACTION_START_TABLE_LEN);
	return OFU_ACTION_PUSH_SUCCESS;
}


	int32_t
fslx_action_drop_packet(struct of_msg *msg)
{
	struct of_msg_descriptor *msg_desc;
	struct fslx_action_drop_packet *ip_rsm;

	msg_desc = &msg->desc;
	if((msg_desc->ptr3+msg_desc->length3 + (FSLX_ACTION_DROP_PACKET_LEN)) >
			(msg->desc.start_of_data + msg_desc->buffer_len)) {
		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN, "Length of buffer is not suffeceient to add data");
		return OFU_NO_ROOM_IN_BUFFER;
	}

	ip_rsm = 
		(struct fslx_action_drop_packet *)(msg_desc->ptr3+msg_desc->length3);

	ip_rsm->type = htons(OFPAT_EXPERIMENTER);
	ip_rsm->len = htons(FSLX_ACTION_DROP_PACKET_LEN);
	ip_rsm->vendor = htonl(FSLX_VENDOR_ID);
	ip_rsm->subtype = htons(FSLXAT_DROP_PACKET);

	msg_desc->length3 += (FSLX_ACTION_DROP_PACKET_LEN);
	return OFU_ACTION_PUSH_SUCCESS;
}

	int32_t
fslx_action_send_arp_response(struct of_msg *msg,  uint8_t *mac)
{
	struct of_msg_descriptor *msg_desc;
	struct fslx_action_arp_response *arp_response;

	msg_desc = &msg->desc;
	if((msg_desc->ptr3+msg_desc->length3 + (FSLX_ACTION_ARP_RESPONSE_LEN)) >
			(msg->desc.start_of_data + msg_desc->buffer_len)) {
		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN, "Length of buffer is not suffeceient to add data");
		return OFU_NO_ROOM_IN_BUFFER;
	}

	arp_response = 
		(struct fslx_action_arp_response *)(msg_desc->ptr3+msg_desc->length3);

	arp_response->type = htons(OFPAT_EXPERIMENTER);
	arp_response->len = htons(FSLX_ACTION_ARP_RESPONSE_LEN);
	arp_response->vendor = htonl(FSLX_VENDOR_ID);
	arp_response->subtype = htons(FSLXAT_ARP_RESPONSE);
	memcpy(arp_response->target_mac, mac, OFP_ETH_ALEN);

	msg_desc->length3 += (FSLX_ACTION_ARP_RESPONSE_LEN);
	return OFU_ACTION_PUSH_SUCCESS;
}


/* Ations to send ICMPV4 error messages */

	int32_t
fslx_action_send_icmpv4_time_exceed_err_msg(struct of_msg *msg, uint8_t error_code)
{
	struct of_msg_descriptor *msg_desc;
	struct fslx_action_icmpv4_time_exceed *time_exceed;

	msg_desc = &msg->desc;
	if((msg_desc->ptr3+msg_desc->length3 + (FSLX_ACTION_ICMPV4_TIME_EXCEED_LEN)) >
			(msg->desc.start_of_data + msg_desc->buffer_len)) {
		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN, "Length of buffer is not suffeceient to add data");
		return OFU_NO_ROOM_IN_BUFFER;
	}

	time_exceed = 
		(struct fslx_action_icmpv4_time_exceed *)(msg_desc->ptr3+msg_desc->length3);

	time_exceed->type = htons(OFPAT_EXPERIMENTER);
	time_exceed->len = htons(FSLX_ACTION_ICMPV4_TIME_EXCEED_LEN);
	time_exceed->vendor = htonl(FSLX_VENDOR_ID);
	time_exceed->subtype = htons(FSLXAT_SEND_ICMPV4_TIME_EXCEED);
	time_exceed->code = error_code;

	msg_desc->length3 += (FSLX_ACTION_ICMPV4_TIME_EXCEED_LEN);
	return OFU_ACTION_PUSH_SUCCESS;
}

	int32_t
fslx_action_send_icmpv4_dest_unreach_err_msg(struct of_msg *msg, uint8_t error_code, uint16_t next_hop_mtu)
{
	struct of_msg_descriptor *msg_desc;
	struct fslx_action_icmpv4_dest_unreachable *dest_unreach;

	msg_desc = &msg->desc;
	if((msg_desc->ptr3+msg_desc->length3 + (FSLX_ACTION_ICMPV4_DEST_UNREACH_LEN)) >
			(msg->desc.start_of_data + msg_desc->buffer_len)) {
		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN, "Length of buffer is not suffeceient to add data");
		return OFU_NO_ROOM_IN_BUFFER;
	}

	dest_unreach = 
		(struct fslx_action_icmpv4_dest_unreachable *)(msg_desc->ptr3+msg_desc->length3);

	dest_unreach->type = htons(OFPAT_EXPERIMENTER);
	dest_unreach->len = htons(FSLX_ACTION_ICMPV4_DEST_UNREACH_LEN);
	dest_unreach->vendor = htonl(FSLX_VENDOR_ID);
	dest_unreach->subtype = htons(FSLXAT_SEND_ICMPV4_DEST_UNREACHABLE);
	dest_unreach->code = error_code;

    if(error_code == FSLX_ICMPV4_DR_EC_DFBIT_SET)
      dest_unreach->next_hop_mtu = next_hop_mtu; 

	msg_desc->length3 += (FSLX_ACTION_ICMPV4_DEST_UNREACH_LEN);
	return OFU_ACTION_PUSH_SUCCESS;
}


/* Table specific actions  */
	int32_t
fslx_action_ip_reassembly(struct of_msg *msg)
{
	struct of_msg_descriptor *msg_desc;
	struct fslx_action_ip_reasm *ip_rsm;

	msg_desc = &msg->desc;
	if((msg_desc->ptr3+msg_desc->length3 + (FSLX_ACTION_IP_REASM_LEN)) >
			(msg->desc.start_of_data + msg_desc->buffer_len)) {
		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN, "Length of buffer is not suffeceient to add data");
		return OFU_NO_ROOM_IN_BUFFER;
	}

	ip_rsm = 
		(struct fslx_action_ip_reasm *)(msg_desc->ptr3+msg_desc->length3);

	ip_rsm->type = htons(OFPAT_EXPERIMENTER);
	ip_rsm->len = htons(FSLX_ACTION_IP_REASM_LEN);
	ip_rsm->vendor = htonl(FSLX_VENDOR_ID);
	ip_rsm->subtype = htons(FSLXAT_IP_REASM);

	msg_desc->length3 += (FSLX_ACTION_IP_REASM_LEN);
	return OFU_ACTION_PUSH_SUCCESS;
}



/* Experimenter instructions */
	int32_t
fslx_instruction_start_pushing_apply_actions_during_add_mod(struct of_msg *msg)
{
	struct of_msg_descriptor *msg_desc;
	struct fslx_instrctn_operation_actns *operation;

	cntlr_assert(msg != NULL);

	msg_desc = &msg->desc;

	if ((msg_desc->ptr2+msg_desc->length2 + FSLXIT_APPLY_ACTION_ON_CREATE_OR_DELETE_INSTRUCTION_LEN) >
			(msg->desc.start_of_data + msg_desc->buffer_len)) 
	{
		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN,
				"Length of buffer is not sufficient to add data");
		return OFU_NO_ROOM_IN_BUFFER;
	}

	operation = (struct fslx_instrctn_operation_actns *)(msg_desc->ptr2+msg_desc->length2);
  	msg_desc->ptr2= msg_desc->ptr2+msg_desc->length2;//vikas
	msg_desc->length2 = 0;
	operation->type = htons(OFPIT_EXPERIMENTER);
	operation->len  = FSLXIT_APPLY_ACTION_ON_CREATE_OR_DELETE_INSTRUCTION_LEN;
	operation->vendor = htonl(FSLX_VENDOR_ID);
	operation->subtype = htons(FSLXIT_APPLY_ACTIONS_ON_CREATE);

	msg_desc->length2 += FSLXIT_APPLY_ACTION_ON_CREATE_OR_DELETE_INSTRUCTION_LEN;
	msg_desc->length3  = 0;

	/* Rembering apply action instruction starting point, so that later one can update total action length
	 * Currently insturctions are avilable for flow-mode, but if it is added to multiple messages, it may
	 * not be useful */
	msg_desc->ptr3 = (uint8_t*)operation->actions;
   //      msg->desc.data_len += msg->desc.length2;//vikas	  

	return OFU_INSTRUCTION_PUSH_SUCCESS;
}

	int32_t
fslx_instruction_end_pushing_apply_actions_during_add_mod(struct of_msg *msg)
{
	return OFU_INSTRUCTION_PUSH_SUCCESS;
}

	int32_t
fslx_instruction_start_pushing_apply_actions_during_del(struct of_msg *msg)
{
	struct of_msg_descriptor *msg_desc;
	struct fslx_instrctn_operation_actns *operation;

	cntlr_assert(msg != NULL);

	msg_desc = &msg->desc;

	if ((msg_desc->ptr2+msg_desc->length2 + FSLXIT_APPLY_ACTION_ON_CREATE_OR_DELETE_INSTRUCTION_LEN) >
			(msg->desc.start_of_data + msg_desc->buffer_len)) 
	{
		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN,"Length of buffer is not sufficient to add data");
		return OFU_NO_ROOM_IN_BUFFER;
	}

	operation = (struct fslx_instrctn_operation_actns *)(msg_desc->ptr2+msg_desc->length2);
  	msg_desc->ptr2= msg_desc->ptr2+msg_desc->length2;//vikas
	msg_desc->length2 = 0;
	operation->type = htons(OFPIT_EXPERIMENTER);
	operation->len  = htons(FSLXIT_APPLY_ACTION_ON_CREATE_OR_DELETE_INSTRUCTION_LEN);
	operation->vendor = htonl(FSLX_VENDOR_ID);
	operation->subtype = htons(FSLXIT_APPLY_ACTIONS_ON_DELETE);

	msg_desc->length2 += FSLXIT_APPLY_ACTION_ON_CREATE_OR_DELETE_INSTRUCTION_LEN;
	msg_desc->length3 = 0;

	/* Rembering apply action instruction starting point, so that later one can update total action length
	 * Currently insturctions are avilable for flow-mode, but if it is added to multiple messages, it may
	 * not be useful */
	msg_desc->ptr3 = (uint8_t *)operation->actions;
   //   msg->desc.data_len += msg->desc.length2;//vikas	  

	return OFU_INSTRUCTION_PUSH_SUCCESS;
}

	int32_t
fslx_instruction_end_pushing_apply_actions_during_del(struct of_msg *msg)
{
	return OFU_INSTRUCTION_PUSH_SUCCESS;
}

#if 0
	int32_t
fslx_instruction_jump_to_flow_entry(struct of_msg *msg, uint16_t priority)
{
	struct of_msg_descriptor *msg_desc;
	struct fslx_instructn_jump *jump;

	cntlr_assert(msg != NULL);

	msg_desc = &msg->desc;

	if((msg_desc->ptr2+msg_desc->length2 + FSLXIT_JUMP_INSTRUCTION_LEN) >
			(msg->desc.start_of_data + msg_desc->buffer_len)) {
		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN,"Length of buffer is not sufficient to add data");
		return OFU_NO_ROOM_IN_BUFFER;
	}

	jump = (struct fslx_instructn_jump *)(msg_desc->ptr2+msg_desc->length2);
	jump->type = htons(OFPIT_EXPERIMENTER);
	jump->len = htons(FSLXIT_JUMP_INSTRUCTION_LEN);
	jump->vendor = htonl(FSLX_VENDOR_ID);
	jump->subtype = htons(FSLXIT_JUMP);
	jump->priority = htons(priority);

	msg_desc->length2 += FSLXIT_JUMP_INSTRUCTION_LEN;

	return OFU_INSTRUCTION_PUSH_SUCCESS;
}
#endif

	int32_t
fslx_instruction_wait_to_complete_pkt_outs(struct of_msg *msg, uint16_t state)
{
	struct of_msg_descriptor *msg_desc;
	struct fslxit_wait_to_cmplete_cntlr_pkts *wait_to_complete;

	cntlr_assert(msg != NULL);

	msg_desc = &msg->desc;

	if((msg_desc->ptr2+msg_desc->length2 + FSLXIT_WAIT_TO_COMPLETE_PKT_OUTS_LEN) >
			(msg->desc.start_of_data + msg_desc->buffer_len)) {
		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN,"Length of buffer is not sufficient to add data");
		return OFU_NO_ROOM_IN_BUFFER;
	}

	msg_desc->ptr2= msg_desc->ptr2+msg_desc->length2;
	msg_desc->length2=0;
	wait_to_complete = (struct fslxit_wait_to_cmplete_cntlr_pkts *)(msg_desc->ptr2+msg_desc->length2);
	wait_to_complete->type = htons(OFPIT_EXPERIMENTER);
	wait_to_complete->len = htons(FSLXIT_WAIT_TO_COMPLETE_PKT_OUTS_LEN);
	wait_to_complete->vendor = htonl(FSLX_VENDOR_ID);
	wait_to_complete->subtype = htons(FSLXIT_WAIT_TO_COMPLETE_PKT_OUTS);
	wait_to_complete->state = htons(state);

	msg_desc->length2 += FSLXIT_WAIT_TO_COMPLETE_PKT_OUTS_LEN;
        msg->desc.data_len += msg->desc.length2;

	return OFU_INSTRUCTION_PUSH_SUCCESS;
}

	int32_t
fslx_instruction_write_priority_register(struct of_msg *msg, uint16_t reg_value)
{
	struct of_msg_descriptor *msg_desc;
	struct fslxit_write_prio_register *write_prio_reg;

	cntlr_assert(msg != NULL);

	msg_desc = &msg->desc;

	if((msg_desc->ptr2+msg_desc->length2 + FSLXIT_WRITE_PRIORITY_REG_LEN) >
			(msg->desc.start_of_data + msg_desc->buffer_len)) {
		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN,"Length of buffer is not sufficient to add data");
		return OFU_NO_ROOM_IN_BUFFER;
	}

	msg_desc->ptr2= msg_desc->ptr2+msg_desc->length2;
	msg_desc->length2=0;
	write_prio_reg = (struct fslxit_write_prio_register *)(msg_desc->ptr2+msg_desc->length2);
	write_prio_reg->type = htons(OFPIT_EXPERIMENTER);
	write_prio_reg->len = htons(FSLXIT_WRITE_PRIORITY_REG_LEN);
	write_prio_reg->vendor = htonl(FSLX_VENDOR_ID);
	write_prio_reg->subtype = htons(FSLXIT_WRITE_PRIORITY_REGISTER);
	write_prio_reg->priority_value = htons(reg_value);

	msg_desc->length2 += FSLXIT_WRITE_PRIORITY_REG_LEN;
        msg->desc.data_len += msg->desc.length2;

	return OFU_INSTRUCTION_PUSH_SUCCESS;
}
#if 1
int32_t
fslx_instruction_write_priority_reg_from_next_flow(struct of_msg *msg)
{
	struct of_msg_descriptor *msg_desc;
	struct fslxit_write_prio_reg_from_flow *write_prio_reg;

	cntlr_assert(msg != NULL);

	msg_desc = &msg->desc;

	if((msg_desc->ptr2+msg_desc->length2 + FSLXIT_WRITE_PRIORITY_REG_FROM_FLOW_LEN) >
			(msg->desc.start_of_data + msg_desc->buffer_len)) {
		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN,"Length of buffer is not sufficient to add data");
		return OFU_NO_ROOM_IN_BUFFER;
	}

	msg_desc->ptr2= msg_desc->ptr2+msg_desc->length2;
	msg_desc->length2=0;

	write_prio_reg = (struct fslxit_write_prio_reg_from_flow *)(msg_desc->ptr2+msg_desc->length2);
	write_prio_reg->type = htons(OFPIT_EXPERIMENTER);
	write_prio_reg->len = htons(FSLXIT_WRITE_PRIORITY_REG_FROM_FLOW_LEN);
	write_prio_reg->vendor = htonl(FSLX_VENDOR_ID);
	write_prio_reg->subtype = htons(FSLXIT_WRITE_PRIORITY_REG_FROM_NEXT_FLOW);

	msg_desc->length2 += FSLXIT_WRITE_PRIORITY_REG_FROM_FLOW_LEN;
        msg->desc.data_len += msg->desc.length2;
 
	return OFU_INSTRUCTION_PUSH_SUCCESS;
}
#endif
	int32_t
fslx_instruction_write_priority_reg_from_current_flow(struct of_msg *msg)
{
	struct of_msg_descriptor *msg_desc;
	struct fslxit_write_prio_reg_from_flow *write_prio_reg;

	cntlr_assert(msg != NULL);

	msg_desc = &msg->desc;

	if((msg_desc->ptr2+msg_desc->length2 + FSLXIT_WRITE_PRIORITY_REG_FROM_FLOW_LEN) >
			(msg->desc.start_of_data + msg_desc->buffer_len)) {
		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN,"Length of buffer is not sufficient to add data");
		return OFU_NO_ROOM_IN_BUFFER;
	}

	msg_desc->ptr2= msg_desc->ptr2+msg_desc->length2;
	msg_desc->length2=0;
	write_prio_reg = (struct fslxit_write_prio_reg_from_flow *)(msg_desc->ptr2+msg_desc->length2);
	write_prio_reg->type = htons(OFPIT_EXPERIMENTER);
	write_prio_reg->len = htons(FSLXIT_WRITE_PRIORITY_REG_FROM_FLOW_LEN);
	write_prio_reg->vendor = htonl(FSLX_VENDOR_ID);
	write_prio_reg->subtype = htons(FSLXIT_WRITE_PRIORITY_REG_FROM_CURRENT_FLOW);

	msg_desc->length2 += FSLXIT_WRITE_PRIORITY_REG_FROM_FLOW_LEN;
        msg->desc.data_len += msg->desc.length2;

	return OFU_INSTRUCTION_PUSH_SUCCESS;
}


	int32_t
fslx_instruction_re_lookup(struct of_msg *msg, uint8_t prio_reg_type)
{
	struct of_msg_descriptor *msg_desc;
	struct fslxit_re_lookup *re_lookup;

	cntlr_assert(msg != NULL);

	msg_desc = &msg->desc;

	if((msg_desc->ptr2+msg_desc->length2 + FSLXIT_RE_LOOKUP_LEN) >
			(msg->desc.start_of_data + msg_desc->buffer_len)) {
		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN,"Length of buffer is not sufficient to add data");
		return OFU_NO_ROOM_IN_BUFFER;
	}

	msg_desc->ptr2= msg_desc->ptr2+msg_desc->length2;
	msg_desc->length2=0;
	re_lookup = (struct fslxit_re_lookup *)(msg_desc->ptr2+msg_desc->length2);
	re_lookup->type = htons(OFPIT_EXPERIMENTER);
	re_lookup->len = htons(FSLXIT_RE_LOOKUP_LEN);
	re_lookup->vendor = htonl(FSLX_VENDOR_ID);
	re_lookup->subtype = htons(FSLXIT_RE_LOOKUP);
	re_lookup->prio_reg_type = prio_reg_type;

	msg_desc->length2 += FSLXIT_RE_LOOKUP_LEN;

        msg->desc.data_len += msg->desc.length2;
	return OFU_INSTRUCTION_PUSH_SUCCESS;
}


	int32_t
fslx_instruction_re_inject_to(struct of_msg *msg, uint8_t table_id, uint8_t prio_reg_type)
{
	struct of_msg_descriptor *msg_desc;
	struct fslxit_re_inject_to *re_inject;

	cntlr_assert(msg != NULL);

	msg_desc = &msg->desc;

	if((msg_desc->ptr2+msg_desc->length2 + FSLXIT_RE_INJECT_TO_LEN) >
			(msg->desc.start_of_data + msg_desc->buffer_len)) {
		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN,"Length of buffer is not sufficient to add data");
		return OFU_NO_ROOM_IN_BUFFER;
	}

	msg_desc->ptr2= msg_desc->ptr2+msg_desc->length2;
	msg_desc->length2=0;
	re_inject = (struct fslxit_re_inject_to *)(msg_desc->ptr2+msg_desc->length2);
	re_inject->type = htons(OFPIT_EXPERIMENTER);
	re_inject->len = htons(FSLXIT_RE_INJECT_TO_LEN);
	re_inject->vendor = htonl(FSLX_VENDOR_ID);
	re_inject->subtype = htons(FSLXIT_RE_INJECT_TO);
	re_inject->table_id = table_id;
	re_inject->prio_reg_type = prio_reg_type;

	msg_desc->length2 += FSLXIT_RE_INJECT_TO_LEN;
        msg->desc.data_len += msg->desc.length2;

	return OFU_INSTRUCTION_PUSH_SUCCESS;
}

	int32_t
fslx_instruction_write_group_actions(struct of_msg *msg, uint32_t group_id)
{
	struct of_msg_descriptor *msg_desc;
	struct fslx_instruction_group *group;

	cntlr_assert(msg != NULL);

	msg_desc = &msg->desc;

	if ((msg_desc->ptr2+msg_desc->length2 + FSLXIT_WRITE_GROUP_ACTIONS_LEN) >
			(msg->desc.start_of_data + msg_desc->buffer_len)) 
	{
		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN,
				"Length of buffer is not sufficient to add data");
		return OFU_NO_ROOM_IN_BUFFER;
	}

	msg_desc->ptr2= msg_desc->ptr2+msg_desc->length2;
	msg_desc->length2=0;
	group = (struct fslx_instruction_group *)(msg_desc->ptr2+msg_desc->length2);
	group->type = htons(OFPIT_EXPERIMENTER);
	group->len  = FSLXIT_WRITE_GROUP_ACTIONS_LEN;
	group->vendor = htonl(FSLX_VENDOR_ID);
	group->subtype = htons(FSLXIT_WRITE_GROUP);
	group->group_id = htonl(group_id);

	msg_desc->length2 += FSLXIT_WRITE_GROUP_ACTIONS_LEN;
	msg_desc->length3  = 0;
        msg->desc.data_len += msg->desc.length2;

	return OFU_INSTRUCTION_PUSH_SUCCESS;
}


	int32_t
fslx_instruction_apply_group_actions(struct of_msg *msg, uint32_t group_id)
{
	struct of_msg_descriptor *msg_desc;
	struct fslx_instruction_group *group;

	cntlr_assert(msg != NULL);

	msg_desc = &msg->desc;

	if ((msg_desc->ptr2+msg_desc->length2 + FSLXIT_WRITE_GROUP_ACTIONS_LEN) >
			(msg->desc.start_of_data + msg_desc->buffer_len)) 
	{
		OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_WARN,
				"Length of buffer is not sufficient to add data");
		return OFU_NO_ROOM_IN_BUFFER;
	}

	msg_desc->ptr2= msg_desc->ptr2+msg_desc->length2;
	msg_desc->length2=0;
	group = (struct fslx_instruction_group *)(msg_desc->ptr2);
	group->type = htons(OFPIT_EXPERIMENTER);
	group->len  = htons(FSLXIT_WRITE_GROUP_ACTIONS_LEN);
	group->vendor = htonl(FSLX_VENDOR_ID);
	group->subtype = htons(FSLXIT_APPLY_GROUP);
	group->group_id = htonl(group_id);

	msg_desc->length2 += FSLXIT_APPLY_GROUP_ACTIONS_LEN;
	msg_desc->length3  = 0;
	msg->desc.data_len += msg->desc.length2;

	return OFU_INSTRUCTION_PUSH_SUCCESS;
}
#if 0
	int32_t
fslx_frame_pkt_out_completion_msg(
		struct of_msg *msg,
		uint64_t  datapath_handle,
		uint8_t   table_id,
		uint64_t  cookie,
		uint64_t  cookie_mask,
		uint8_t   version,
		struct of_cntlr_conn_info **conn_info_pp
		)
{
	struct fslx_pkt_out_completion_msg *pktout_cmplt_msg;
	struct fslx_header *fslx_hdr;
	struct dprm_datapath_entry  *datapath;
	int32_t  status = OF_SUCCESS;
	int32_t  retval = OF_SUCCESS;
	struct ofp_match *match = NULL;
	struct of_cntlr_conn_info *conn_info;

	cntlr_assert(msg != NULL);

	CNTLR_RCU_READ_LOCK_TAKE();
	do
	{
		retval = get_datapath_byhandle(datapath_handle, &datapath);
		if(retval  != DPRM_SUCCESS) {
			OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_DEBUG,"Error in getting datapath,Err=%d ",
					retval);
			status = DPRM_INVALID_DATAPATH_HANDLE;
			break;
		}

		fslx_hdr = (struct fslx_header *)(msg->desc.start_of_data);


		fslx_hdr->header.version = version;
		fslx_hdr->header.type    = OFPT_EXPERIMENTER;
		fslx_hdr->header.xid     = 0;
		fslx_hdr->vendor        = htonl(FSLX_VENDOR_ID);
		fslx_hdr->subtype       = htonl(FSLX_PKT_OUT_COMPLETION);

		pktout_cmplt_msg = 
			(struct fslx_pkt_out_completion_msg *)(msg->desc.start_of_data + FSLX_HEADER_LEN);

		pktout_cmplt_msg->cookie = htonll(cookie);
		pktout_cmplt_msg->cookie_mask = htonll(cookie_mask);
		pktout_cmplt_msg->table_id = table_id;
		/* match fields can vary in every flow hence match fields can be set NBAPIs by caller after 
		 * returning from this function similar to flow mod message. */
		match = &pktout_cmplt_msg->match;
		msg->desc.utils_ptr   = (uint8_t *)match;

		msg->desc.data_len += (FSLX_PKT_OUTS_COMPLETION_MSG_LEN + FSLX_HEADER_LEN - 4);

		if(datapath->is_main_conn_present) {
			CNTLR_GET_MAIN_CHANNEL_INFO(datapath,conn_info);
			*conn_info_pp=conn_info;
		}
		else {
			OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_DEBUG,"Error in getting  channel");
			status=OF_FAILURE;
			break;
		}
	}
	while(0);
	CNTLR_RCU_READ_LOCK_RELEASE();

	return status;
}
#endif


	int32_t
fslx_frame_table_config_msg(
		struct of_msg *msg,
		uint64_t  datapath_handle,
		uint8_t   table_id,
		uint8_t   version,
		struct of_cntlr_conn_info **conn_info_pp
		)
{
	struct fslx_table_config *table_config_msg;
	struct fslx_header *fslx_hdr; 
	struct dprm_datapath_entry  *datapath;
	int32_t  status = OF_SUCCESS;
	int32_t  retval = OF_SUCCESS;
	struct ofp_instruction *instructions = NULL;
	struct of_cntlr_conn_info *conn_info;

	cntlr_assert(msg != NULL);

	CNTLR_RCU_READ_LOCK_TAKE();
	do
	{
		retval = get_datapath_byhandle(datapath_handle, &datapath);
		if(retval  != DPRM_SUCCESS) {
			OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_DEBUG,"Error in getting datapath,Err=%d ",
					retval);
			status = DPRM_INVALID_DATAPATH_HANDLE;
			break;
		}

		fslx_hdr = (struct fslx_header *)(msg->desc.start_of_data);


		fslx_hdr->header.version = version;
		fslx_hdr->header.type    = OFPT_EXPERIMENTER;
		fslx_hdr->header.xid     = 0;
		fslx_hdr->vendor        = htonl(FSLX_VENDOR_ID);
		fslx_hdr->subtype       = htonl(FSLX_TABLE_CONFIG);

		table_config_msg = 
			(struct fslx_table_config *)(msg->desc.start_of_data + FSLX_HEADER_LEN);
		table_config_msg->table_id = table_id;

		/* no. of instructions can vary hence the caller set required instructions after returning from this function
		 * similar to flow mod message.
		 */
		instructions = table_config_msg->instructions;
		msg->desc.utils_ptr   = (uint8_t *)instructions;

		msg->desc.data_len += (FSLX_TABLE_CONFIG_MSG_LEN + FSLX_HEADER_LEN - 4);
		if(datapath->is_main_conn_present) {
			CNTLR_GET_MAIN_CHANNEL_INFO(datapath,conn_info);
			*conn_info_pp=conn_info;
		}
		else {
			OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_DEBUG,"Error in getting  channel");
			status=OF_FAILURE;
			break;
		}


	}
	while(0);
	CNTLR_RCU_READ_LOCK_RELEASE();

	return status;
}


	int32_t
onfx_get_vendor_versions(
		struct of_msg         *msg,
		uint64_t              datapath_handle,
		onfx_vendor_version_cbk_fn vendor_version_cbk,
		void                  *cbk_arg1,
		void                  *cbk_arg2)
{
	struct onfx_multipart_request  *verdor_version_req;
	struct dprm_datapath_entry   *datapath;
	cntlr_conn_node_saferef_t conn_safe_ref;
	cntlr_conn_table_t        *conn_table;
	uint8_t  send_status = TRUE;
	int32_t  status = OF_SUCCESS;
	int32_t  retval = OF_SUCCESS;


	OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_INFO," entered ");

	CNTLR_RCU_READ_LOCK_TAKE();
	do
	{
		if(msg->desc.buffer_len != ONFX_MULTIPART_REQUEST_LEN) {
			OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_ERROR,
					" Allocated message buffer length is not suffecient ");
			status = OF_MSG_BUF_SIZE_IS_IN_SUFFECIENT;
		}

		retval = get_datapath_byhandle(datapath_handle, &datapath);
		if(retval  != DPRM_SUCCESS) {
			OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_ERROR," Error in getting datapath,Err=%d ",
					retval);
			status = DPRM_INVALID_DATAPATH_HANDLE;
			break;
		}

		if(!datapath->is_main_conn_present) {
			OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_ERROR,
					" Error in sending message to datapath,channel closed ");
			msg->desc.free_cbk(msg);
			status = CNTLR_CHN_CLOSED;
			break;
		}
		CNTLR_GET_MAIN_CHANNEL_SAFE_REF(datapath,conn_safe_ref,conn_table);

		if( conn_table == NULL ) {
			OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_ERROR," conn table NULL ");
			status = OF_FAILURE;
			break;
		}

		verdor_version_req = (struct onfx_multipart_request *)(msg->desc.start_of_data);
		verdor_version_req->header.version = OFP_VERSION;
		verdor_version_req->header.type = OFPT_MULTIPART_REQUEST;
		verdor_version_req->header.xid = 0;

		verdor_version_req->type = htons(OFPMP_EXPERIMENTER);
		verdor_version_req->flags = 0;
		verdor_version_req->vendor = htonl(ONF_VENDOR_ID);
		verdor_version_req->subtype = htonl(ONFX_VENDOR_VER);


		msg->desc.data_len += (ONFX_MULTIPART_REQUEST_LEN);
		verdor_version_req->header.length = htons(msg->desc.data_len);


		CNTLR_Q_2_BARRIERQ_IF_REQUIRED_OTHERWISE_SEND_2_DP(msg,datapath,conn_table,conn_safe_ref,
				(void*)vendor_version_cbk,cbk_arg1,cbk_arg2,send_status);

		if(send_status  == FALSE) {
			OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_ERROR,
					" Error in sending message to datapath ");
			status = CNTLR_SEND_MSG_TO_DP_ERROR;
			break;
		}
	}
	while(0);
	CNTLR_RCU_READ_LOCK_RELEASE();
	return status;
}


	int32_t
fslx_create_bind_mod_add_entry_msg(
      struct of_msg *msg,
      uint64_t  datapath_handle,
      uint32_t  bind_obj_id,
      uint8_t   type,
      uint8_t   remove_flow_config,
      uint8_t   del_bind_obj_config,
      uint64_t  hard_timeout_sec,
      uint64_t  hard_timeout_bytes,
      uint64_t  hard_timeout_packets,
      uint32_t  idle_timeout,
      uint64_t  cookie,
      void      **conn_info_pp)
{
  struct fslx_header          *fslx_hdr;
  struct fslx_bind_mod        *bind_mod;
  struct dprm_datapath_entry  *datapath = NULL;
  struct of_cntlr_conn_info   *conn_info = NULL;
  int32_t                     retval, status = OF_SUCCESS;

  OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_INFO, "entered");
  cntlr_assert(msg != NULL);

  OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_DEBUG,
         "dp_handle = 0x%llx, bind_obj_id = 0x%lx",
         datapath_handle, bind_obj_id);
  CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    retval = get_datapath_byhandle(datapath_handle, &datapath);
    if(retval  != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_DEBUG,
                 "Error in getting datapath,Err=%d ", retval);
      status = DPRM_INVALID_DATAPATH_HANDLE;
      break;
    }
    fslx_hdr = (struct fslx_header *)(msg->desc.start_of_data);
    fslx_hdr->header.version = FSLX_VERSION;
    fslx_hdr->header.type    = OFPT_EXPERIMENTER;
    fslx_hdr->header.xid     = 0;
    fslx_hdr->vendor        = htonl(FSLX_VENDOR_ID);
    fslx_hdr->subtype       = htonl(FSLX_BIND_MOD);

    bind_mod = (struct fslx_bind_mod*)(msg->desc.start_of_data + FSLX_HEADER_LEN);
    bind_mod->bind_obj_id  = htonl(bind_obj_id);
    bind_mod->type    = type;
    bind_mod->command = FSLX_BC_ADD;
    bind_mod->remove_flow_config = remove_flow_config;
    bind_mod->del_bind_obj_config = del_bind_obj_config;
    bind_mod->idle_timeout = htonl(idle_timeout);
    bind_mod->life_time_as_timeout = htonll(hard_timeout_sec);
    bind_mod->life_time_as_no_of_bytes  = htonll(hard_timeout_bytes);
    bind_mod->life_time_as_no_of_packets = htonll(hard_timeout_packets);
    bind_mod->cookie = htonll(cookie);

    msg->desc.data_len += (FSLX_HEADER_LEN + FSLX_BIND_MOD_MESSAGE_LEN);

    if(datapath->is_main_conn_present)
    {
      CNTLR_GET_MAIN_CHANNEL_INFO(datapath,conn_info);
      *conn_info_pp = conn_info;
      if ( conn_info == NULL )
      {
        OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_ERROR,"conn info is null");
        status = OF_FAILURE;
        break;
      }
    }
    else
    {
      OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_ERROR,"Error in getting  channel");
      status = OF_FAILURE;
      break;
    }
  }
  while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();
  OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_INFO, "exit, status = %d",status);
  return status;
}

int32_t fslx_create_bind_mod_del_entry_msg(struct of_msg *msg,
					   uint64_t  datapath_handle,
					   uint32_t  bind_obj_id,
					   uint8_t   type,
					   uint8_t   remove_flow_config,
					   uint8_t   del_bind_obj_config,
					   void **conn_info_pp)
{
	struct fslx_header          *fslx_hdr;
	struct fslx_bind_mod        *bind_mod;
	struct dprm_datapath_entry  *datapath;
	struct of_cntlr_conn_info   *conn_info;
	int32_t                     retval, status = OF_SUCCESS;

	OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_INFO, "entered");
	cntlr_assert(msg != NULL);

	CNTLR_RCU_READ_LOCK_TAKE();
	do
	{
		/** Get Datapath information by giving datapath handle */
		retval = get_datapath_byhandle(datapath_handle, &datapath);
		if(retval  != DPRM_SUCCESS)
		{
			OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_ERROR,
					"Error in getting datapath,Err=%d ", retval);
			status = DPRM_INVALID_DATAPATH_HANDLE;
			break;
		}
		fslx_hdr = (struct fslx_header *)(msg->desc.start_of_data);
		fslx_hdr->header.version = FSLX_VERSION;
		fslx_hdr->header.type    = OFPT_EXPERIMENTER;
		fslx_hdr->header.xid     = 0;
		fslx_hdr->vendor        = htonl(FSLX_VENDOR_ID);
		fslx_hdr->subtype       = htonl(FSLX_BIND_MOD);

		bind_mod = (struct fslx_bind_mod*)(msg->desc.start_of_data + FSLX_HEADER_LEN);
        bind_mod->bind_obj_id         = htonl(bind_obj_id);
		bind_mod->type                = type;
		bind_mod->command             = FSLX_BC_DELETE;
		bind_mod->remove_flow_config  = remove_flow_config;
		bind_mod->del_bind_obj_config = del_bind_obj_config;
		bind_mod->idle_timeout        = 0;
		bind_mod->life_time_as_timeout = 0;
		bind_mod->life_time_as_no_of_bytes  = 0;
		bind_mod->life_time_as_no_of_packets = 0;

		msg->desc.data_len += (FSLX_HEADER_LEN + FSLX_BIND_MOD_MESSAGE_LEN);

		if(datapath->is_main_conn_present)
		{
			CNTLR_GET_MAIN_CHANNEL_INFO(datapath,conn_info);
			*conn_info_pp = conn_info;
			if ( conn_info == NULL )
			{
				OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_ERROR,"conn info is null");
				status=OF_FAILURE;
				break;
			}
		}
		else
		{
			OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_ERROR,"Error in getting  channel");
			status = OF_FAILURE;
			break;
		}
	}
	while(0);
	CNTLR_RCU_READ_LOCK_RELEASE();
	OF_LOG_MSG(OF_LOG_UTIL_L3EXT, OF_LOG_INFO, "exit, status = %d",status);
	return status;
}

//#endif

