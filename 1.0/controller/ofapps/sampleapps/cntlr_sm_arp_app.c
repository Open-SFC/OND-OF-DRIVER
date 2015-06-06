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

/*File: cntlr_sm_arp_app.c
 *Author: Kumara Swamy Ganji <kumar.sg@freescale.com>
 * Description:
 * This file contains sample ARP application registering with dp async pkt in event.
 * The code to add miss entry.
 * The code related to ARP response for ARP request received in pkt in.
 */



#include <errno.h>
#include <stdint.h>
#include <stdio.h>
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
#include "fsl_ext.h"
#include "pktmbuf.h"


/*************** Global Variables declaration ***********************/

#if 0
uint32_t       no_of_iface_entries[OF_CNTLR_MAX_DATA_PATHS];
#endif

extern cntlr_sm_sw_conf_t dp_conf[OF_CNTLR_MAX_DATA_PATHS];
extern uint32_t       no_of_data_paths_attached_g;

uint32_t       arp_app_entry_priority = 10;

#if 0
cntlr_sm_arp_app_iface_info_t iface_info[OF_CNTLR_MAX_DATA_PATHS][OF_CNTLR_SW_MAX_IFACES];
#endif

/* Extern Functions  */
extern struct of_msg *ofu_alloc_message(uint8_t  type, uint16_t size);
extern int32_t sm_app_add_missentry_with_output_action(uint64_t datapath_handle, uint8_t table_id,
														uint32_t outport, uint16_t max_len);

extern char *itoa(int n);
extern unsigned char atox(char *cPtr);


/* Static Function proto types */
void sm_app_print_pkt(uint8_t *pkt, uint32_t length);
int32_t sm_app_arp_request_n_response_pkt_process(struct of_app_msg *app_msg);

  int32_t 
sm_app_arp_request_n_response_handler(int64_t controller_handle,
    uint64_t domain_handle,
    uint64_t datapath_handle,
    struct of_msg *msg,
    struct ofl_packet_in  *pkt_in,
    void *cbk_arg1,
    void *cbk_arg2)
{
  uint8_t  *pkt_data = pkt_in->packet;
  struct dprm_datapath_entry *pdatapath_info;
  uint64_t  dpid;
  struct of_app_msg *app_msg; 
  int32_t iretval = OF_SUCCESS;


  if (get_datapath_byhandle(datapath_handle, &pdatapath_info) != DPRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_WARN, "No DP Entry with given datapath_handle=%llu",datapath_handle);
  }
  dpid = pdatapath_info->dpid;

  app_msg=(struct of_app_msg *)calloc(1,sizeof(struct of_app_msg));
  app_msg->table_id=pkt_in->table_id;
  app_msg->datapath_handle=datapath_handle;
  app_msg->controller_handle=controller_handle;
  app_msg->domain_handle=domain_handle;
  app_msg->dpid=dpid;
  app_msg->pkt_in=pkt_in;
  app_msg->pkt_data=pkt_data;
  app_msg->cbk_arg1=cbk_arg1;
  app_msg->cbk_arg2=cbk_arg2;
#ifdef CNTRL_APP_THREAD_SUPPORT  
  cntlr_enqueue_msg_2_appxprt_conn(app_msg);
#else
  iretval = sm_app_arp_request_n_response_pkt_process(app_msg);
#endif
  free(app_msg);
  if (iretval != OF_SUCCESS)
  {
    return OF_ASYNC_MSG_DROP;
  }
  else
  {
    if (msg != NULL)
    {
     of_get_pkt_mbuf_by_of_msg_and_free(msg);
     // msg->desc.free_cbk(msg);
    }
    return OF_ASYNC_MSG_CONSUMED;
  }
}

  int32_t
sm_app_arp_request_n_response_pkt_process(struct of_app_msg *app_msg)
{
  uint32_t sender_ip, target_ip;
  uint32_t in_port;
  uint8_t  *pkt_data;
  uint8_t  arp_reply[50];
  uint32_t arp_pkt_len = 0;
  uint32_t ii;
  struct dprm_port_entry *dp_port_entry;
  uint64_t    port_handle;
  uint8_t  sender_hw_addr[OFP_ETH_ALEN];
  uint8_t  target_hw_addr[OFP_ETH_ALEN];
  uint8_t  *ucPtr, hw_len, proto_len;
  uint16_t eth_type, proto_type, hw_type, operation;
  uint8_t       *action_start_loc=NULL;
  uint16_t      action_len;
  uint16_t       msg_len;
  struct of_msg *reply_msg;
  int32_t  status = OF_SUCCESS;
  int32_t  retval = OF_SUCCESS;
  uint64_t  dpid, meta_data, meta_data_mask;
  uint32_t  dp_index;


  int64_t controller_handle;
  uint64_t domain_handle;
  uint64_t datapath_handle;
  struct of_msg *msg=NULL;
  struct ofl_packet_in  *pkt_in;
  void *cbk_arg1;
  void *cbk_arg2;
  void *conn_info_p;
  int32_t flags=0;
  struct pkt_mbuf *mbuf = NULL;



  datapath_handle=  app_msg->datapath_handle;
  controller_handle=app_msg->controller_handle;
  domain_handle=app_msg->domain_handle;
  dpid=app_msg->dpid;
  pkt_in=app_msg->pkt_in;
  pkt_data = app_msg->pkt_data;
  cbk_arg1=app_msg->cbk_arg1;
  cbk_arg2=app_msg->cbk_arg2;

#if 1
  for (dp_index = 0; dp_index < no_of_data_paths_attached_g; dp_index++)
  {
    if (dp_conf[dp_index].dp_handle == datapath_handle)
      break;
  }

  if (dp_index == no_of_data_paths_attached_g)
  {
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"\n%s: No datapath found with given dp handle=%llx",__FUNCTION__,datapath_handle);
    return OF_FAILURE;
  }
#else
  pdp_conf =  (cntlr_sm_sw_conf_t *)cbk_arg1;
  if (pdp_conf == NULL)
  {
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"\n%s:miss entry callcbk arg1 is NULL..",__FUNCTION__);
    if (msg != NULL)
    {
      msg->desc.free_cbk(msg);
    }
    return OF_ASYNC_MSG_CONSUMED;
  }
  else if (pdp_conf->dp_handle == datapath_handle)
  {
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"\n%s:invalid dp handle received..dp_handle=%llx",__FUNCTION__,datapath_handle);
    if (msg != NULL)
    {
      msg->desc.free_cbk(msg);
    }
    return OF_ASYNC_MSG_CONSUMED;
  }
  dp_index = pdp_conf->dp_index;
#endif

  if (ntohs(*(uint16_t *)(pkt_data+12)) != OF_CNTLR_ETH_TYPE_ARP)	{
	  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,
				"%s:%d NON ARP pkt received dropping",__FUNCTION__,__LINE__);
	  return OF_FAILURE;
  }

  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"PKT-IN RECEIVED FOR ARP APP MISS ENTRY DPID=%llx dpindex=%d",dpid,dp_index);
  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, "Received pkt of length %d",pkt_in->packet_length);

  /* Parse ARP request packet */
  ucPtr = pkt_data;
  /* skip dest MAC address */
  ucPtr += OFP_ETH_ALEN;
  /* skip src MAC address */
  ucPtr = ucPtr + OFP_ETH_ALEN;
  /* get eth type */
  eth_type = ntohs(*(uint16_t *)ucPtr);
  ucPtr += 2;
  /* parse hw type */
  hw_type = ntohs(*(uint16_t *)ucPtr);
  ucPtr += 2;
  /* parse protocol type */
  proto_type = ntohs(*(uint16_t *)ucPtr);
  ucPtr += 2;
  /* parse hw address length */
  hw_len = *ucPtr;
  ucPtr += 1;
  /* protocol address length */
  proto_len = *ucPtr;
  ucPtr += 1;
  /* parse opcode: ARP REQUEST */
  operation = ntohs(*(uint16_t *)ucPtr);
  ucPtr += 2;
  /* parse sender mac address */
  memcpy(sender_hw_addr, ucPtr, OF_CNTLR_HW_ADDR_LEN);
  ucPtr += OFP_ETH_ALEN;
  /* parse sender's ip address */
  sender_ip = ntohl(*(uint32_t *)ucPtr);
  ucPtr += OF_CNTLR_IPV4_ADDRESS_LEN;
  /* Skip target MAC address which is all zeros anyway */
  memcpy(target_hw_addr, ucPtr, OF_CNTLR_HW_ADDR_LEN);
  ucPtr = ucPtr + OFP_ETH_ALEN;
  /* parse target ip address */
  target_ip = ntohl(*(uint32_t *)ucPtr);

  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"ARP REQUEST PKT");
  //sm_app_print_pkt(pkt_data, pkt_in->packet_length);
  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"PARSED PKT DETAILS");
  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, 
      "\neth-type=%x hw_type=%x pro-type=%x hwlen=%d protolen=%d sendip=%x targetip=%x opcode=%d",
      eth_type,hw_type, proto_type, hw_len, proto_len,sender_ip,target_ip,operation);

  do
  {
    if (operation != OF_CNTLR_ARP_REQUEST && operation != OF_CNTLR_ARP_REPLY)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d Invalid arp request pkt received with operation=%x",__FUNCTION__,__LINE__,operation);
      status = OF_FAILURE;
      return status;
    }

    if (proto_type != 0x0800)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:Invalid arp request pkt with invalid protocol type=%x",__FUNCTION__,proto_type);
      status = OF_FAILURE;
      break;
    }

    /* in_port = get using north bound API */

    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"%s:number of iface interfaces: %d, operation: %d",__FUNCTION__,dp_conf[dp_index].no_of_iface_entries, operation);
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"%s:target_ip: %x, sender_ip: %x",__FUNCTION__,target_ip, sender_ip);
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"%s:target_HW: %x:%x:%x:%x:%x:%x",__FUNCTION__,target_hw_addr[0],target_hw_addr[1],target_hw_addr[2],target_hw_addr[3],
        target_hw_addr[4], target_hw_addr[5]);
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"%s:sender_HW %x:%x:%x:%x:%x:%x",__FUNCTION__,sender_hw_addr[0],sender_hw_addr[1],sender_hw_addr[2],sender_hw_addr[3],
        sender_hw_addr[4], sender_hw_addr[5]);
    // (char*)sender_hw_addr);
    for (ii=0; ii< dp_conf[dp_index].no_of_iface_entries; ii++)
    {
      if (dp_conf[dp_index].iface_info[ii].ip_address == target_ip)
      {
        break;
      }
    }

    if (ii == dp_conf[dp_index].no_of_iface_entries)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:arp request not belong to this controller target_ip=%x",__FUNCTION__,target_ip);
      status = OF_FAILURE;
      break;
    }

    /* get port mac address with port id */ 
    retval = ofu_get_in_port_field(msg, pkt_in->match_fields,
        pkt_in->match_fields_length, &in_port);
    if (retval != OFU_IN_PORT_FIELD_PRESENT)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:failed get inport ....inport=%d",__FUNCTION__,in_port);
      status = OF_FAILURE;
      break;
    }
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, " PKT-IN IN PORT =%d",in_port);
#if 1
    retval = get_port_handle_by_port_id(datapath_handle, in_port, &port_handle);
#else
    retval = dprm_get_port_handle(datapath_handle, dp_conf[dp_index].iface_info[ii].port_name, &port_handle);
#endif
    if (retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:failed dprm_get_port_handle port-id=%d",__FUNCTION__,in_port);
      status = OF_FAILURE;
      break;
    }

    retval = get_datapath_port_byhandle(datapath_handle, port_handle, &dp_port_entry);
    if (retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:failed get_datapath_port_byhandle =%ld",__FUNCTION__,port_handle);
      status = OF_FAILURE;
      break;
    }
    //if(target_hw_addr == 0)  //Deepthi
    memcpy(target_hw_addr, dp_port_entry->port_info.hw_addr, OFP_ETH_ALEN);
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"%s:2-target_HW: %x:%x:%x:%x:%x:%x",__FUNCTION__,target_hw_addr[0],target_hw_addr[1],target_hw_addr[2],target_hw_addr[3],
        target_hw_addr[4], target_hw_addr[5]);
    //sm_app_print_pkt(target_hw_addr, 6);
    if (operation == OF_CNTLR_ARP_REPLY)
    {
      //On receiving the response, verify the sender IP and set its state to learned and stop the timer.
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"%s:ARP REPLY RECVD, no of arp entries: %d",__FUNCTION__, dp_conf[dp_index].no_of_arp_entries);
      for (ii=0; ii< dp_conf[dp_index].no_of_arp_entries; ii++)
      {
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"%s:arp_ip: %x, sender_ip: %x, index:%d",__FUNCTION__,dp_conf[dp_index].arp_table[ii].arp_ip, sender_ip, ii);
        if(dp_conf[dp_index].arp_table[ii].arp_ip == sender_ip)
        {
          if(dp_conf[dp_index].arp_table[ii].state != OF_CNTLR_ARP_STATE_LEARNED)
          {
            OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,"%s:ARP REPLY RECVD, Moving the state to LEARNED",__FUNCTION__);
            dp_conf[dp_index].arp_table[ii].state = OF_CNTLR_ARP_STATE_LEARNED;
            timer_stop_tmr(NULL,&dp_conf[dp_index].arp_table[ii].arpreq_timer);
            if(dp_conf[dp_index].arp_table[ii].arp_req_tmr != NULL)
            {
              free(dp_conf[dp_index].arp_table[ii].arp_req_tmr);
              dp_conf[dp_index].arp_table[ii].arp_req_tmr = NULL;
            }
            if(dp_conf[dp_index].arp_table[ii].datapath_handle_tmr != NULL)
            {
              free(dp_conf[dp_index].arp_table[ii].datapath_handle_tmr);
              dp_conf[dp_index].arp_table[ii].datapath_handle_tmr = NULL;
            }
          }
          //arp reply - mac address of the target is received and updated
          memcpy(dp_conf[dp_index].arp_table[ii].arp_mac, sender_hw_addr, OF_CNTLR_APP_MAC_ADDR_LEN);
          OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"%s:sender_HW %x:%x:%x:%x:%x:%x",__FUNCTION__,dp_conf[dp_index].arp_table[ii].arp_mac[0],dp_conf[dp_index].arp_table[ii].arp_mac[1],dp_conf[dp_index].arp_table[ii].arp_mac[2],dp_conf[dp_index].arp_table[ii].arp_mac[3],dp_conf[dp_index].arp_table[ii].arp_mac[4], dp_conf[dp_index].arp_table[ii].arp_mac[5]);
          meta_data = (uint64_t)sender_ip;
          meta_data_mask = (uint64_t)0xffffffff;
      
          break;
        }
      }
      break;
    }
    /* Frame ARP response packet */
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"%s:framing arp response",__FUNCTION__);
    arp_pkt_len = 0;
    ucPtr = arp_reply;
    /* dest. mac address */
    memcpy(ucPtr, sender_hw_addr, OFP_ETH_ALEN);
    ucPtr += OFP_ETH_ALEN;
    arp_pkt_len += OFP_ETH_ALEN;
    /* src mac address */
    memcpy(ucPtr, target_hw_addr, OFP_ETH_ALEN);
    ucPtr += OFP_ETH_ALEN;
    arp_pkt_len += OFP_ETH_ALEN;
    /* eth type */
    (*(uint16_t *)ucPtr) = htons(eth_type);
    ucPtr += 2;
    arp_pkt_len += 2;
    /* hw type */
    (*(uint16_t *)ucPtr) = htons(hw_type);
    ucPtr += 2;
    arp_pkt_len += 2;
    /* protocol type */
    (*(uint16_t *)ucPtr) = htons(proto_type);
    ucPtr += 2;
    arp_pkt_len += 2;
    /* hw length */
    (*(uint8_t *)ucPtr) = hw_len;
    ucPtr += 1;
    arp_pkt_len += 1;
    /* protocol length */
    (*(uint8_t *)ucPtr) = proto_len;
    ucPtr += 1;
    arp_pkt_len += 1;
    /* op-code:ARP reply */
    (*(uint16_t *)ucPtr) = htons(OF_CNTLR_ARP_REPLY);
    ucPtr += 2;
    arp_pkt_len += 2;
    /* sender mac address or resolved mac address */
    memcpy(ucPtr, target_hw_addr, OFP_ETH_ALEN);
    ucPtr += OFP_ETH_ALEN;
    arp_pkt_len += OFP_ETH_ALEN;
    /* sender ip */
    (*(uint32_t *)ucPtr) = htonl(target_ip);
    ucPtr += OF_CNTLR_IPV4_ADDRESS_LEN;
    arp_pkt_len += OF_CNTLR_IPV4_ADDRESS_LEN;
    /* target mac address */
    memcpy(ucPtr, sender_hw_addr, OFP_ETH_ALEN);
    ucPtr += OFP_ETH_ALEN;
    arp_pkt_len += OFP_ETH_ALEN;
    /* target ip */
    (*(uint32_t *)ucPtr) = htonl(sender_ip);
    ucPtr += OF_CNTLR_IPV4_ADDRESS_LEN;
    arp_pkt_len += OF_CNTLR_IPV4_ADDRESS_LEN;
    //sm_app_print_pkt(arp_reply, arp_pkt_len);

    msg_len = (OFU_PACKET_OUT_MESSAGE_LEN+16+OFU_OUTPUT_ACTION_LEN+arp_pkt_len);
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"Msg len %d",msg_len);
    /* get port mac address with port id */ 
    retval = ofu_get_in_port_field(msg, pkt_in->match_fields,
        pkt_in->match_fields_length, &in_port);
    if (retval != OFU_IN_PORT_FIELD_PRESENT)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:failed get inport ....inport=%d",__FUNCTION__,in_port);
      status = OF_FAILURE;
      break;
    }
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG," PKT-IN IN PORT =%d",in_port);
#if 1
    retval = get_port_handle_by_port_id(datapath_handle, in_port, &port_handle);
#else
    retval = dprm_get_port_handle(datapath_handle, dp_conf[dp_index].iface_info[ii].port_name, &port_handle);
#endif
    if (retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:failed dprm_get_port_handle port-id=%d",__FUNCTION__,in_port);
      status = OF_FAILURE;
      break;
    }

    retval = get_datapath_port_byhandle(datapath_handle, port_handle, &dp_port_entry);
    if (retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:failed get_datapath_port_byhandle =%ld",__FUNCTION__,port_handle);
      status = OF_FAILURE;
      break;
    }
    memcpy(target_hw_addr, dp_port_entry->port_info.hw_addr, OFP_ETH_ALEN);


    
  of_alloc_pkt_mbuf_and_set_of_msg(mbuf, reply_msg, OFPT_PACKET_OUT, msg_len, flags, status);
  if(status == FALSE)
  {
          OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR, "pkt_mbuf_alloc() failed");
          status = OF_FAILURE;
          return status;
  }

//    reply_msg = ofu_alloc_message(OFPT_PACKET_OUT, msg_len);
    if (reply_msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d OF message alloc error \r\n",__FUNCTION__,__LINE__);
      status = OF_FAILURE;
      return status;
    }

    retval=of_frame_pktout_msg(  
        reply_msg,
        datapath_handle,
        FALSE,
        OFP_NO_BUFFER,
        OFPP_CONTROLLER,
        0,
        &conn_info_p

        );
    if (retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in framing pkt out");
      of_get_pkt_mbuf_by_of_msg_and_free(reply_msg);
      //reply_msg->desc.free_cbk(reply_msg);
      status = OF_FAILURE;
      break;
    }
    ofu_start_pushing_actions(reply_msg);
    retval = ofu_push_output_action(reply_msg,in_port,OFPCML_NO_BUFFER);
    if (retval != OFU_ACTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d  Error in adding output action  err = %d\r\n",
          __FUNCTION__,__LINE__,retval);
      of_get_pkt_mbuf_by_of_msg_and_free(reply_msg);
      //  of_pkt_mbuf_free(reply_msg);
        status = OF_FAILURE;
        break;
    }
    ofu_end_pushing_actions(reply_msg,action_start_loc,&action_len);
    ((struct ofp_packet_out*)(reply_msg->desc.start_of_data))->actions_len=htons(action_len);
    retval = of_add_data_to_pkt_and_send_to_dp(
        reply_msg,
        datapath_handle,
        conn_info_p,
        arp_pkt_len,
        arp_reply,
        NULL,NULL);
    if (retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d  Error in sending packet err = %d\r\n", __FUNCTION__,__LINE__,retval);
      of_get_pkt_mbuf_by_of_msg_and_free(reply_msg);
      //of_pkt_mbuf_free(reply_msg);
      status = OF_FAILURE;
      break;
    }
 
    meta_data = (uint64_t)sender_ip;
    meta_data_mask = (uint64_t)0xffffffff;
    sm_app_add_arp_entry(datapath_handle,dp_index, arp_app_entry_priority++,
                         meta_data, meta_data_mask
                         ,sender_hw_addr,target_hw_addr, in_port);
  }
  while(0);

  return status;
}


int32_t sm_arp_app_init()
{
  return OF_SUCCESS;
}

int32_t sm_arp_app_add_proactive_flow(uint64_t datapath_handle, uint32_t dp_index, uint16_t eth_type)
{
  struct      of_msg *msg;
  uint16_t    msg_len;
  uint16_t    instruction_len;
  uint8_t     *inst_start_loc = NULL;
  uint16_t    action_len;
  uint8_t     *action_start_loc = NULL;
  uint16_t    match_fd_len=0;
  uint8_t     *match_start_loc = NULL;
  uint16_t    flags=0;
  int32_t     retval = OF_SUCCESS;
  int32_t     status = OF_SUCCESS;
  void *conn_info_p;

  if ( (eth_type == OF_CNTLR_ETH_TYPE_ARP) || 
       (eth_type == OF_CNTLR_ETH_TYPE_LLDP) )
  { 
	msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+16+
			OFU_ETH_TYPE_FIELD_LEN+
			OFU_APPLY_ACTION_INSTRUCTION_LEN+
			OFU_OUTPUT_ACTION_LEN);
  }
  else
  {
    msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+16+
        OFU_ETH_TYPE_FIELD_LEN+
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
        dp_conf[dp_index].ethtype_table_id,/*Table Id 0*/
        arp_app_entry_priority++, /*priority*/
        OFPFC_ADD,
        0, /*coockie*/
        0,/*Coockie amsk*/
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
    }

    ofu_start_setting_match_field_values(msg);
    retval = of_set_ether_type(msg,&eth_type);
    if (retval != OFU_SET_FIELD_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d OF Set IPv4 src field failed,err = %d\r\n",__FUNCTION__,__LINE__,retval);
      status = OF_FAILURE;
      break;
    }
    ofu_end_setting_match_field_values(msg,match_start_loc,&match_fd_len);
    if ((eth_type == OF_CNTLR_ETH_TYPE_ARP) ||
        (eth_type == OF_CNTLR_ETH_TYPE_LLDP))
    { 
      ofu_start_pushing_instructions(msg, match_fd_len);
      retval = ofu_start_pushing_apply_action_instruction(msg);
      if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d  Error in adding apply action instruction err = %d\r\n",
            __FUNCTION__,__LINE__,retval);
        status = OF_FAILURE;
        break;
      }

      //ofu_start_pushing_actions(msg); 
      retval = ofu_push_output_action(msg, OFPP_CONTROLLER, OFPCML_NO_BUFFER);
      if (retval != OFU_ACTION_PUSH_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d  Error in adding output action  err = %d\r\n",
            __FUNCTION__,__LINE__,retval);
        status = OF_FAILURE;
        break;
      }
      ofu_end_pushing_actions(msg,action_start_loc,&action_len);
      ((struct ofp_instruction_actions*)(msg->desc.ptr2))->len = htons(action_len);
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"total actions len = %d for this instruction", action_len);

      ofu_end_pushing_instructions(msg,inst_start_loc,&instruction_len);
    }
    else if (eth_type == OF_CNTLR_ETH_TYPE_IPV4)
    {
      ofu_start_pushing_instructions(msg, match_fd_len);
      retval = ofu_push_go_to_table_instruction(msg, dp_conf[dp_index].mcast_class_table_id);
      if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d go to table instruction failed err = %d\r\n",
            __FUNCTION__,__LINE__,retval);
        status = OF_FAILURE;
        break;
      }
      ofu_end_pushing_instructions(msg,inst_start_loc,&instruction_len);
    }    

    retval=of_cntrlr_send_request_message(
        msg,
        datapath_handle,
        conn_info_p,
        NULL,
        NULL);
    if (retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d  Error in adding arp flow entry  = %d\r\n",
          __FUNCTION__,__LINE__,retval);
      status = OF_FAILURE;
    }
  }
  while(0);

  return status;
}


int32_t  get_port_handle_by_port_id(uint64_t dp_handle, uint16_t port_id, uint64_t *port_handle_ptr)
{
  struct    dprm_port_info config_info_port = {};
  struct    dprm_port_runtime_info config_runtime_info_port = {};
  int32_t   retval; 
  uint64_t  port_handle = 0;

  retval =  dprm_get_first_datapath_port(dp_handle,&config_info_port,&config_runtime_info_port,&port_handle);
  while (retval == DPRM_SUCCESS)
  {
    if (config_info_port.port_id == port_id)
    {
      *port_handle_ptr = port_handle;
      return OF_SUCCESS;
    }
    //port_handle = 0;
    memset(&config_info_port, 0, sizeof(struct    dprm_port_info));
    retval = dprm_get_next_datapath_port(dp_handle,&config_info_port,&config_runtime_info_port,&port_handle);
  }
  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:No entry found with given port_id  = %d retval=%d\r\n",__FUNCTION__,port_id,retval);
  return OF_FAILURE;
}

void sm_app_print_arp_table(uint32_t dp_index)
{
  uint32_t ii;
  uint8_t cbyte = 0;

  printf("\nDP%llx_ARP Table:",dp_conf[dp_index].dp_id);
  printf("\nTotal No Of Entries:%d",dp_conf[dp_index].no_of_arp_entries);
  puts("\n ____________________________________________________");
  puts("\n  ARP-IP                       MAC  ");
  puts("\n ----------------------------------------------------");
  for (ii = 0; ii < dp_conf[dp_index].no_of_arp_entries; ii++)	{
    printf("\n %x              ",dp_conf[dp_index].arp_table[ii].arp_ip);
    for (cbyte = 0; cbyte < 6; cbyte++)	{
      printf("%x ",*(dp_conf[dp_index].arp_table[ii].arp_mac+cbyte));
    }
  }
  puts("\n -----------------------------------------------------");
  return;
}

static int32_t sm_app_read_arp_entries(uint32_t dp_index)
{ 
  int32_t ii = 0, a = 0;
  char line[150];
  char arp_ipaddr[18] = {};
  char ucMacAddr[20];
  char *token = NULL;
  char *byte = NULL;
  uint8_t cbyte = 0;
  FILE *fr;        /* declare the file pointer */


  fr = fopen(dp_conf[dp_index].rt_arp_conf_file, "rt");/* open the file for reading */
  if (fr == NULL)	{
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG," No more files:%s to read errno=%d",dp_conf[dp_index].rt_arp_conf_file,errno);
    perror(NULL);
    return OF_FAILURE;
  }

  while(fgets(line, 150, fr) != NULL)
  {
    if ((line[0] == '#') || ((line[0] == '/') && (line[1] == '*')))	{
      continue;
    }

    token = (char *)strtok(line,"-");
    do
    {
      if (!strncmp(token,"ip",strlen("ip")))	{
        strcpy(arp_ipaddr, (token+strlen("ip ")));
        dp_conf[dp_index].arp_table[a].arp_ip = inet_network(arp_ipaddr);
        if (token[strlen(token)-1] == '\n')
          a++; 
      }
      else if (!strncmp(token,"mac",strlen("mac")))	{
        strcpy(ucMacAddr, (token+strlen("mac ")));
        byte = strtok(ucMacAddr,":");
        ii = 0;
        while (byte != NULL)
        {
          cbyte = atox(byte);
          dp_conf[dp_index].arp_table[a].arp_mac[ii] = cbyte;
          if ((++ii) == 6)
          {
            break;
          }
          byte = strtok(NULL,":");
        }
        dp_conf[dp_index].arp_table[a].state = OF_CNTLR_ARP_STATE_STATIC;
        if (token[strlen(token)-1] == '\n')
          a++;
      }
    }while((token = (char *)strtok(NULL,"-")) != NULL);
  }
  dp_conf[dp_index].no_of_arp_entries = a;
  sm_app_print_arp_table(dp_index);

  fclose(fr);
  return OF_SUCCESS; 
}


int32_t sm_app_arp_dp_ready_handler(uint64_t dp_handle, uint32_t dp_index)
{
	int32_t status = OF_SUCCESS;

	do	{
	  if (sm_app_read_arp_entries(dp_index) != OF_SUCCESS)
	  {
	    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"Reading route entries from text cofig file failed");
	    status = OF_FAILURE;
	    break;
	  }

	  /* Add miss entries in arp table */
	  if (sm_app_add_missentry_with_output_action(dp_handle, dp_conf[dp_index].arp_table_id,
		OFPP_CONTROLLER, OFPCML_NO_BUFFER) != OF_SUCCESS)
	  {
	    status = OF_FAILURE;
	    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"adding miss entries in arp table failed ");
	    break;
	  }
	}while(0);

	return status;
}

void sm_app_print_pkt(uint8_t *pkt, uint32_t length)
{
  uint8_t ii;

  puts("\n");
  for (ii=0; ii<length; ii++)
  {
    printf("%x ", *(pkt+ii));
  }
}

