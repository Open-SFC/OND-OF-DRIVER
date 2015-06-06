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

/*File: cntlr_sample_app.c
 *Author: Kumara Swamy Ganji <kumar.sg@freescale.com>
 * Description:
 * This file contains sample application registering with dp and pkt in event and
 * flow mod APIs to add flows in session, route and arp tables by using NB APIs
 */



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
#include "fsl_ext.h"
#include "oflog.h"
#include "pktmbuf.h"

/*************** Global Variables declaration ***********************/
uint16_t       sess_entry_priority;
uint16_t       route_entry_priority;
uint16_t       arp_entry_priority;
uint32_t       no_of_data_paths_attached_g;
struct dprm_distributed_forwarding_domain_info  l3_fwd_domain_info;
cntlr_sm_sw_conf_t            dp_conf[OF_CNTLR_MAX_DATA_PATHS];

#ifdef FSLX_COMMON_AND_L3_EXT_SUPPORT
extern uint8_t is_fsl_extensions_loaded_g; 
#endif

/* Extern Functions  */
extern struct of_msg *ofu_alloc_message(uint8_t  type, uint16_t size);
extern int32_t
ofu_push_set_metadata_from_match_field_instruction(struct of_msg *msg,
                                                   uint8_t match_field_id);
extern int32_t sm_arp_app_add_proactive_flow(uint64_t datapath_handle, uint32_t dp_index, uint16_t eth_type);

extern int32_t 
sm_app_arp_request_n_response_handler(int64_t controller_handle,
                                      uint64_t domain_handle,
                                      uint64_t datapath_handle,
                                      struct of_msg *msg,
                                      struct ofl_packet_in  *pkt_in,
                                      void *cbk_arg1,
                                      void *cbk_arg2);
extern void getall_datapaths(void);
extern void getall_ports(uint64_t dp_handle);

extern int32_t sm_app_arp_dp_ready_handler(uint64_t dp_handle, uint32_t dp_index);
extern int32_t sm_app_igmp_dp_ready_handler(uint64_t dp_handle, uint32_t dp_index);
/* Static Function proto types */
extern int32_t sm_app_igmp_init(void);
extern int32_t
sm_app_igmp_pkt_in_rcvd(int64_t controller_handle,
                     uint64_t domain_handle,
                     uint64_t datapath_handle,
                     struct of_msg     *msg,
                     struct ofl_packet_in  *pkt_in,
                     void   *cbk_arg1,
                     void   *cbk_arg2);

extern int32_t sm_app_add_mcast_proactive_entry(uint64_t datapath_handle,
						uint32_t *mcast_ip, uint32_t *mask, uint8_t *protocol,
						uint8_t tableid, uint16_t priority,
						uint64_t  cookie, uint64_t  cookie_mask,
						uint32_t  buffer_id, uint16_t  flags);

/* Static Function proto types */
static int32_t sm_app_read_of_sw_interfaces(uint32_t dp_index);
static int32_t sm_app_read_fw_acl_rules(uint32_t dp_index);
static int32_t sm_app_read_route_entries(uint32_t dp_index);
static int32_t
sm_app_add_sessoin_table_miss_entry(uint64_t datapath_handle, uint32_t dp_index);


static int32_t
sm_app_add_route_table_miss_entry(uint64_t datapath_handle, uint32_t dp_index);

int32_t
sm_app_add_missentry_with_output_action(uint64_t datapath_handle, uint8_t table_id,
					uint32_t outport, uint16_t max_len);

int32_t sm_app_add_missentry_with_goto_table_instruction(uint64_t datapath_handle, uint8_t table_id, uint8_t jump_tableid);

void sm_app_domain_event_cbk(uint32_t notification_type,
                             uint64_t domain_handle,
                             struct   dprm_domain_notification_data notification_data,
                             void     *callback_arg1,
                             void     *callback_arg2);


void sm_app_print_iface_table(uint32_t dp_index);

void sm_barrier_reply_notification_fn(uint64_t  controller_handle,
                                                 uint64_t  domain_handle,
                                                 uint64_t  datapath_handle,
                                                 struct of_msg *msg,
                                                 void     *cbk_arg1,
                                                 void     *cbk_arg2,
                                                 uint8_t   status);

int32_t add_domain_oftable_to_idbased_list(uint64_t dm_handle,
                                           uint8_t table_id,
                                           struct dprm_oftable_entry *table_info_p);

int32_t sm_app_sess_table_pkt_handler(struct of_app_msg *app_msg);

int32_t sm_app_arp_table_pkt_process(struct of_app_msg *app_msg);

int32_t sm_app_frame_and_send_pktout_to_dp(uint64_t datapath_handle, struct ofl_packet_in *pkt_in);

int32_t sm_app_attach_dpid_to_conf_files(uint32_t dpinfo_index,
                                         uint64_t dpid,
                                         uint64_t datapath_handle);
void cntlr_shared_lib_init(void);

void cntlr_send_barrier_request(uint64_t datapath_handle);

char *itoa(int i) 
{
  int n = snprintf(NULL, 0, "%d", i) + 1;
  char *s = malloc(n);

  if (s != NULL)
    snprintf(s, n, "%d", i);
  return s;
}

char * int64toa(uint64_t i) 
{
  int n = snprintf(NULL, 0, "%llx", i) + 1;
  char *res = malloc(n);
  sprintf(res, "%llx", i);
  return res;
}

void i64toa(uint64_t value, char *sp, int radix)
{
  char tmp[16];// be careful with the length of the buffer
  char *tp = tmp;
  int i;
  unsigned v;
  int sign;

  sign = (radix == 10 && value < 0);
  if (sign)   v = -value;
  else    v = (unsigned)value;

  while (v || tp == tmp)
  {
    i = v % radix;
    v /= radix; // v/=radix uses less CPU clocks than v=v/radix does
    if (i < 10)
      *tp++ = i+'0';
    else
      *tp++ = i + 'a' - 10;
  }

  if (sign)
    *sp++ = '-';
  while (tp > tmp)
    *sp++ = *--tp;
}

static inline  uint16_t sm_app_get_sess_entry_priority(void)
{
	return ((sess_entry_priority == 0xFFFE)?10:sess_entry_priority++);
}

static inline  uint16_t sm_app_get_rt_entry_priority(void)
{
	return ((route_entry_priority == 0xFFFE)?10:route_entry_priority++);
}

static inline  uint16_t sm_app_get_arp_entry_priority(void)
{
	return ((arp_entry_priority == 0xFFFE)?10:arp_entry_priority++);
}


int32_t sm_app_config_resources()
{
  //struct dprm_switch_info                         switch_1;
  //struct dprm_datapath_general_info               datapath_info_1 = {};
  //struct dprm_datapath_general_info               datapath_info_2 = {};
  //struct dprm_datapath_general_info               datapath_info_3 = {};
  //uint64_t                                        switch_handle_1 = 0;
  //uint64_t                                        domain_handle_1     = 0;
  //uint64_t                                        dp_handle_1         = 0;
  //uint64_t                                        dp_handle_2         = 0;
  //uint64_t                                        dp_handle_3         = 0;
  //uint32_t                                        dp_index = 0;
  //uint32_t                                        retval = OF_SUCCESS;
  uint32_t                                        status = OF_SUCCESS;

  do
  {
#if 0
    /*Add switch to the system*/
    strcpy(switch_1.name, "SWITCH-1");
    switch_1.switch_type        = PHYSICAL_SWITCH;
    switch_1.ipv4addr.baddr_set = FALSE;
    strcpy(switch_1.switch_fqdn,"http:/xyz.com.");
    retval = dprm_register_switch(&switch_1,&switch_handle_1);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"Add Switch to the system fail,retval %d",retval);
      status = OF_FAILURE;
      break;
    }


    retval = dprm_create_distributed_forwarding_domain(&l3_fwd_domain_info,&domain_handle_1);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"Add Domain to the system fail retval=%d",retval);
      status = OF_FAILURE;
      break;
    }


    //struct dprm_table_info table_info;
    struct dprm_table_info oftable_info = {};
    struct dprm_oftable_entry *oftable_entry = NULL;
    uint64_t  table_handle;

    /* This table is to forward ARP request packets to ARP to application with PKT-IN msg.*/
    memset(&oftable_info, 0, sizeof(struct dprm_table_info));
    strcpy(oftable_info.table_name,"ARP-APP-TABLE");
    oftable_info.table_id           = SM_APP_ETHTYPE_TABLE_ID;/* Table 0*/
    oftable_info.no_of_match_fields = 1;
    retval = dprm_add_oftable_to_domain(domain_handle_1,&oftable_info,&table_handle);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"Add table to domain fail, retval=%d",retval);
      status = OF_FAILURE;
      break;
    }

    retval = get_domain_table_byname(domain_handle_1,"ARP-APP-TABLE",&oftable_entry);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, "Failed to get the table structure");
      status = OF_FAILURE;
      break;
    }

    retval = add_domain_oftable_to_idbased_list(domain_handle_1,
        SM_APP_ETHTYPE_TABLE_ID,oftable_entry);
    if(retval != DPRM_SUCCESS)
    {
      status = OF_FAILURE;
      break;
    }

    /* Add Session Table to the domain */ 
    oftable_entry = NULL;
    strcpy(oftable_info.table_name,"SESSION-TABLE");
    oftable_info.table_id           = SM_APP_SESSION_TABLE_ID;
    oftable_info.no_of_match_fields = 6;
    retval = dprm_add_oftable_to_domain(domain_handle_1,&oftable_info,&table_handle);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"Add table to domain fail, retval=%d",retval);
      status = OF_FAILURE;
      break;
    }

    retval = get_domain_table_byname(domain_handle_1,"SESSION-TABLE",&oftable_entry);
    if(retval != DPRM_SUCCESS)
    {
      status = OF_FAILURE;
      break;
    }

    retval = add_domain_oftable_to_idbased_list(domain_handle_1,
        SM_APP_SESSION_TABLE_ID,oftable_entry);
    if(retval != DPRM_SUCCESS)
    {
      status = OF_FAILURE;
      break;
    }

    /* Add Routing table to the domain */
    oftable_entry = NULL;
    memset(&oftable_info, 0, sizeof(struct dprm_table_info));
    strcpy(oftable_info.table_name,"ROUTE-TABLE");
    oftable_info.table_id           = SM_APP_L3ROUTING_TABLE_ID;
    oftable_info.no_of_match_fields = 2;
    retval = dprm_add_oftable_to_domain(domain_handle_1,&oftable_info,&table_handle);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"Add table to domain fail, retval=%d",retval);
      status = OF_FAILURE;
      break;
    }

    retval = get_domain_table_byname(domain_handle_1,"ROUTE-TABLE",&oftable_entry);
    if(retval != DPRM_SUCCESS)
    {
      status = OF_FAILURE;
      break;
    }

    retval = add_domain_oftable_to_idbased_list(domain_handle_1,
        SM_APP_L3ROUTING_TABLE_ID,oftable_entry);
    if(retval != DPRM_SUCCESS)
    {
      status = OF_FAILURE;
      break;
    }

    oftable_entry = NULL;
    memset(&oftable_info, 0, sizeof(struct dprm_table_info));
    strcpy(oftable_info.table_name,"ARP-TABLE");
    oftable_info.table_id           = SM_APP_ARP_TABLE_ID;/* Table 3*/
    oftable_info.no_of_match_fields = 1;
    retval = dprm_add_oftable_to_domain(domain_handle_1,&oftable_info,&table_handle);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"Add table to domain fail, retval=%d",retval);
      status = OF_FAILURE;
      break;
    }

    retval = get_domain_table_byname(domain_handle_1,"ARP-TABLE",&oftable_entry);
    if(retval != DPRM_SUCCESS)
    {
      status = OF_FAILURE;
      break;
    }

    retval = add_domain_oftable_to_idbased_list(domain_handle_1,
        SM_APP_ARP_TABLE_ID,oftable_entry);
    if(retval != DPRM_SUCCESS)
    {
      status = OF_FAILURE;
      break;
    }

    /* Add Data path to the system*/
#if 0
    datapath_info_1.dpid = 0x020001020304;
    strcpy(datapath_info_1.expected_subject_name_for_authentication,"dp_1@xyz.com");
    retval = dprm_register_datapath(&datapath_info_1,switch_handle_1,domain_handle_1,&dp_handle_1);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"Add Data path to the system fail retval=%d",retval);
      status = OF_FAILURE;
      break;
    }
    sm_app_attach_dpid_to_conf_files(dp_index, datapath_info_1.dpid, dp_handle_1);

    dp_index++;
    datapath_info_2.dpid = 0x030001020304;
    strcpy(datapath_info_2.expected_subject_name_for_authentication,"dp_1@xyz.com");
    retval = dprm_register_datapath(&datapath_info_2,switch_handle_1,domain_handle_1,&dp_handle_2);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"Add Data path to the system fail retval=%d",retval);
      status = OF_FAILURE;
      break;
    }
    sm_app_attach_dpid_to_conf_files(dp_index, datapath_info_2.dpid, dp_handle_2);

    dp_index++;
    datapath_info_3.dpid = 0x040001020304;
    strcpy(datapath_info_3.expected_subject_name_for_authentication,"dp_1@xyz.com");
    retval = dprm_register_datapath(&datapath_info_3,switch_handle_1,domain_handle_1,&dp_handle_3);
    if(retval != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"Add Data path to the system fail retval=%d",retval);
      status = OF_FAILURE;
      break;
    }
    sm_app_attach_dpid_to_conf_files(dp_index, datapath_info_3.dpid, dp_handle_3);
    no_of_data_paths_attached_g = dp_index + 1;
#endif
#endif
  }
  while(0);

  if(status == OF_FAILURE)
  {
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"APPLICATION INITIALISATION FAILED");
    /*TBD release of all resources*/
    return OF_FAILURE;
  }
  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"APPLICATION INITIALISED");
  return OF_SUCCESS;
}


static int32_t 
sm_app_sess_table_miss_entry_pkt_rcvd(int64_t controller_handle,
                                      uint64_t domain_handle,
                                      uint64_t datapath_handle,
                                      struct of_msg *msg,
                                      struct ofl_packet_in  *pkt_in,
                                      void *cbk_arg1,
                                      void *cbk_arg2)
{
  uint8_t  *pkt_data = pkt_in->packet;
  uint64_t dpid;
  struct of_app_msg *app_msg=NULL; 
  struct dprm_datapath_entry *pdatapath_info;
  int32_t iretval = OF_SUCCESS;

  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"PKT-IN RECEIVED FOR SESSION ENTRY MISS DPID");
  if (get_datapath_byhandle(datapath_handle, &pdatapath_info) != DPRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR, "No DP Entry with given datapath_handle=%llx",datapath_handle);
    if (msg != NULL)
    {
     of_get_pkt_mbuf_by_of_msg_and_free(msg);
//      msg->desc.free_cbk(msg);
    }
    return OF_ASYNC_MSG_CONSUMED;
  }
  dpid = pdatapath_info->dpid;
	  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG," Received pkt of length %d",pkt_in->packet_length);

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
  iretval = sm_app_sess_table_pkt_handler(app_msg);
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
      //msg->desc.free_cbk(msg);
    }
    return OF_ASYNC_MSG_CONSUMED;
  }
}


int32_t
sm_app_sess_table_pkt_handler(struct of_app_msg *app_msg)
{
  int64_t controller_handle;
  uint64_t domain_handle;
  uint64_t datapath_handle;
  struct ofl_packet_in  *pkt_in;
  void *cbk_arg1;
  void *cbk_arg2;
  uint16_t frag_off=0;
  uint8_t frag_bit;

  uint32_t source_ip, dest_ip;
  uint32_t snat_ip = 0, dnat_ip = 0;
  uint8_t  nat_type = OF_CNTLR_NO_NAT;
  uint16_t src_port, dst_port, priority;
  uint8_t  protocol;
  uint32_t in_port = 0, out_port = 0;
  uint8_t  *pkt_data;
  uint8_t  icmp_type, icmp_code;
  uint16_t ipv4_hdr_len;
  uint32_t ii;
  int32_t  status = OF_SUCCESS;
  uint64_t dpid;
  uint32_t dp_index;
  char out_iface[16]={};


  datapath_handle=  app_msg->datapath_handle;
  controller_handle=app_msg->controller_handle;
  domain_handle=app_msg->domain_handle;
  dpid=app_msg->dpid;
  pkt_in=app_msg->pkt_in;
  pkt_data = app_msg->pkt_data;
  cbk_arg1=app_msg->cbk_arg1;
  cbk_arg2=app_msg->cbk_arg2;

  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO, "entered");
  do
  {
#if 1
    for (dp_index = 0; dp_index < no_of_data_paths_attached_g; dp_index++)
    {
      if (dp_conf[dp_index].dp_handle == datapath_handle)
        break;
    }

    if (dp_index == no_of_data_paths_attached_g)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR, "No datapath found with given dp handle=%llx",datapath_handle);
      status = OF_FAILURE;
      break;
    }
#else
    pdp_conf =  (cntlr_sm_sw_conf_t *)cbk_arg1;
    if (pdp_conf == NULL)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, "miss entry callcbk arg1 is NULL..");
      status = OF_FAILURE;
      break;
    }
    else if (pdp_conf->dp_handle == datapath_handle)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, "invalid dp handle received..dp_handle=%llx",datapath_handle);
      status = OF_FAILURE;
      break;
    }
    dp_index = pdp_conf->dp_index;
#endif
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"PKT-IN RECEIVED FOR SESSION ENTRY MISS DPID=%llx",dpid);

    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"Received pkt of length %d",pkt_in->packet_length);
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"msg %p pkt_in %p pkt_data %p",app_msg,pkt_in,pkt_data);

    source_ip = ntohl(*(uint32_t*)(pkt_data + OF_CNTLR_IPV4_HDR_OFFSET +
          OF_CNTLR_IPV4_SRC_IP_OFFSET));

    dest_ip = ntohl(*(uint32_t*)(pkt_data + OF_CNTLR_IPV4_HDR_OFFSET+
          OF_CNTLR_IPV4_DST_IP_OFFSET));

    protocol = *(pkt_data + OF_CNTLR_IPV4_HDR_OFFSET + OF_CNTLR_IPV4_PROTO_OFFSET);
    frag_off =ntohs(*(uint16_t *)(pkt_data + OF_CNTLR_IPV4_HDR_OFFSET + OF_CNTLR_IPV4_FRAG_OFFSET));
    //    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"frag_off value %d", frag_off );
    frag_bit= (frag_off >> 12) & 0xe;
    frag_off = (frag_off & 0x1FFF) << 3;
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,"frag_off is %d and frag_bit %d", frag_off , frag_bit);

    if  ((frag_off ) != 0) 
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"offset is not zero.. controller droping it ");
      status = OF_FAILURE;
      break;
    }
    if ( frag_bit  ==   OF_CNTLR_IPV4_MORE_FRAG )
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"more fragments bit set .. controller dropping it");
      status = OF_FAILURE;
      break;
    }
#if 0
    if (( (frag_off & 0x000fffffffffffff) == 0)  || ( frag_bit & 0x0 ==   OF_CNTLR_IPV4_MORE_FRAG ))
    {
      if ( frag_bit & 0x0 ==   OF_CNTLR_IPV4_MORE_FRAG ) 
      {
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"fragmented packet ");
        //        status = OF_FAILURE;
        //        break;
      }
    }
#endif

    ipv4_hdr_len = OF_CNTLR_IPV4_HDR_LEN(pkt_data);

    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"IPV4_HDR_LEN=%d",ipv4_hdr_len);
    if (protocol == OF_CNTLR_IP_PROTO_ICMP)
    {
      src_port = dst_port = 
        ntohs(*(uint16_t*)(pkt_data + OF_CNTLR_IPV4_HDR_OFFSET +
              ipv4_hdr_len + OF_CNTLR_ICMPV4_ID_OFFSET));

      icmp_type = *(pkt_data + OF_CNTLR_IPV4_HDR_OFFSET + ipv4_hdr_len);
      icmp_code = *(pkt_data + OF_CNTLR_IPV4_HDR_OFFSET + ipv4_hdr_len +
          OF_CNTLR_ICMPV4_CODE_OFFSET);

      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, "srcip=%x dstip=%x icmptype=%d icmpcode=%d proto=%d",
          source_ip,dest_ip,icmp_type, icmp_code, protocol);

#if 0
      if ((icmp_type != OF_CNTLR_ICMPV4_ECHO_REQUEST) &&
          (icmp_type != OF_CNTLR_ICMPV4_TIMESTAMP))
      {
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"non icmp echo/timestamp request pkt received...");
        status = OF_FAILURE;
        break;
      }
#endif
    }
    else
    {
      src_port = ntohs(*(uint16_t*)(pkt_data + OF_CNTLR_IPV4_HDR_OFFSET+ipv4_hdr_len));
      dst_port = ntohs(*(uint16_t*)(pkt_data + OF_CNTLR_IPV4_HDR_OFFSET+ipv4_hdr_len+2));
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"\r\n srcip=%x dstip=%x srcprt=%d dstprt=%d proto=%d",source_ip,dest_ip,
          src_port,dst_port,protocol);
    }

    for (ii=0; ii< dp_conf[dp_index].no_of_fw_rules; ii++)
    {
      if ((!(dp_conf[dp_index].fw_rule_list[ii].src_ip) || (dp_conf[dp_index].fw_rule_list[ii].src_ip == source_ip)) && 
          (!(dp_conf[dp_index].fw_rule_list[ii].dst_ip) || (dp_conf[dp_index].fw_rule_list[ii].dst_ip == dest_ip)) &&
          (!(dp_conf[dp_index].fw_rule_list[ii].src_port) || (dp_conf[dp_index].fw_rule_list[ii].src_port == src_port)) &&
          (!(dp_conf[dp_index].fw_rule_list[ii].dst_port) || (dp_conf[dp_index].fw_rule_list[ii].dst_port == dst_port)) &&
          (!(dp_conf[dp_index].fw_rule_list[ii].protocol) || (dp_conf[dp_index].fw_rule_list[ii].protocol == protocol)) && 
          (dp_conf[dp_index].fw_rule_list[ii].action == OF_CNTLR_FW_RULE_ALLOW))
      {
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,"packet matched firewall rule... ****");
        break;
      }
    }

    if (ii == dp_conf[dp_index].no_of_fw_rules)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_WARN,"packet did not match firewall rule...");
      status = OF_FAILURE;
      break;
    }

#if 0
    dp_conf[dp_index].fw_rule_list[ii].in_iface, strlen(dp_conf[dp_index].fw_rule_list[ii].in_iface));

    //getall_datapaths(); 
    //getall_ports(datapath_handle);
    retval = dprm_get_port_handle(datapath_handle, dp_conf[dp_index].fw_rule_list[ii].in_iface, &port_handle);
    if (retval == DPRM_SUCCESS)
    {
      retval = get_datapath_port_byhandle(datapath_handle, port_handle, &dp_port_entry);
      if (retval != DPRM_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"Error in getting port  %s err=%d \r\n",dp_conf[dp_index].fw_rule_list[ii].in_iface, retval);
        status = OF_FAILURE;
        break;
      }
      else
      {
        in_port = dp_port_entry->port_id;
      }

    }
    else
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"dprm_get_port_handle by name failed..");
      status = OF_FAILURE;
      break;
    }
#endif
    if (ofu_get_in_port_field(NULL, pkt_in->match_fields,
                   pkt_in->match_fields_length, &in_port) != OFU_IN_PORT_FIELD_PRESENT)
    {
       OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:failed get inport ....inport=%d",__FUNCTION__,in_port);
       status = OF_FAILURE;
       break;
    }
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, " PKT-IN IN PORT =%x",in_port);

    /* Adding session flow entry */
#if 0
    if ((in_port == OFPP_TAP_PORT1) ||  (in_port == OFPP_TAP_PORT2))	{
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG," ##### IN PORT TAP INTERFACE SNAT WILL BE DONE #####...");
	if (sm_app_rt_get_out_iface_by_dstip(dp_index, dest_ip, out_iface) != OF_SUCCESS)
	{
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," rt get iface by destip failed...");
	    status = OF_FAILURE;
	    break;
	}
	if (sm_app_get_ip_by_iface_name(dp_index, out_iface, &snat_ip) != OF_SUCCESS)
	{
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"get ip by iface name failed ...");
	    status = OF_FAILURE;
	    break;
	}
	    /* in reverse direction destination ip must be changed to original source ip */
	nat_type = OF_CNTLR_SRC_NAT;
	dnat_ip = source_ip;
    }
    else
#endif 
    {
	    if (dp_conf[dp_index].fw_rule_list[ii].snat_ip) /*SNAT is enabled */
	    { 
            OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_MOD," SNAT is enabled");
		    nat_type = OF_CNTLR_SRC_NAT;
		    snat_ip = dp_conf[dp_index].fw_rule_list[ii].snat_ip;
		    /*in SNAT in reverse direction destination ip must be changed to original source ip */
		    dnat_ip = source_ip;
	    }
	    else if (dp_conf[dp_index].fw_rule_list[ii].dnat_ip) /* DNAT is enabled */
	    {
            OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_MOD," DNAT is enabled");
		    nat_type = OF_CNTLR_DST_NAT;
		    dnat_ip = dp_conf[dp_index].fw_rule_list[ii].dnat_ip;
		    /*in DNAT in reverse direction source ip must be changed to original destination ip */
		    snat_ip = dest_ip; 
	    }
    }

    /* Adding session flow entry */
	priority = sm_app_get_sess_entry_priority();
    if (sm_app_add_session_entry(datapath_handle, dp_index, priority,
          source_ip, dest_ip,
          src_port, dst_port,
          protocol, in_port,
          icmp_type, icmp_code,
          (nat_type == OF_CNTLR_SRC_NAT)?snat_ip:0,
          (nat_type == OF_CNTLR_DST_NAT)?dnat_ip:0) != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"adding session flow entry failed");
      status = OF_FAILURE;
      break;
    }

#if 0
    /* Adding session flow reverse entry */
    //if (sm_app_rt_get_out_iface_by_dstip(dp_index, dest_ip, out_iface) != OF_SUCCESS)
    if (sm_app_rt_get_out_iface(dp_index, source_ip, dest_ip, out_iface) != OF_SUCCESS)

    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"getting output interface failed....");
      status = OF_FAILURE;
      break;
    }

    dp_port_entry = NULL;
    port_handle = 0;
    retval = dprm_get_port_handle(datapath_handle, out_iface, &port_handle);
    if (retval == DPRM_SUCCESS)
    {
      retval = get_datapath_port_byhandle(datapath_handle, port_handle, &dp_port_entry);
      if (retval != DPRM_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"Error in getting port  %s err=%d ", out_iface, retval);
        status = OF_FAILURE;
        break;
      }
      out_port = dp_port_entry->port_id;

    }
    else
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"dprm_get_port_handle by name failed..");
      status = OF_FAILURE;
      break;
    }
#endif

    if (icmp_type == OF_CNTLR_ICMPV4_ECHO_REQUEST)
    {
      icmp_type = OF_CNTLR_ICMPV4_ECHO_REPLY;
      icmp_code = 0;
    }

	/* Reverse Flow */
	priority = sm_app_get_sess_entry_priority();
    if (sm_app_add_session_entry(datapath_handle, dp_index, priority,
          (nat_type == OF_CNTLR_DST_NAT)?dnat_ip:dest_ip,
          (nat_type == OF_CNTLR_SRC_NAT)?snat_ip:source_ip,
          dst_port, src_port,
          protocol, out_port,
          icmp_type, icmp_code,
	      (nat_type == OF_CNTLR_SRC_NAT)?0:snat_ip,
	      (nat_type == OF_CNTLR_DST_NAT)?0:dnat_ip) != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"adding session flow entry failed");
      status = OF_FAILURE;
      break;
    }
    cntlr_send_barrier_request(datapath_handle);

    if (sm_app_frame_and_send_pktout_to_dp(datapath_handle,pkt_in) != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"sending packet out failed");
      status = OF_FAILURE;
      break;
    }
  }
  while(0);

  return status;
}


int32_t
sm_app_arp_table_miss_entry_pkt_rcvd(int64_t controller_handle,
                     uint64_t domain_handle,
                     uint64_t datapath_handle,
                     struct of_msg     *msg,
                     struct ofl_packet_in  *pkt_in,
                     void   *cbk_arg1,
                     void   *cbk_arg2)
{
  struct dprm_datapath_entry *pdatapath_info;
  uint64_t    dpid;
  struct of_app_msg *app_msg;
  int32_t iretval = OF_SUCCESS;

  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG," PKT-IN RECEIVED FOR ARP MISS ENTRY");


  if (get_datapath_byhandle(datapath_handle, &pdatapath_info) != DPRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"No DP Entry with given datapath_handle=%llu",datapath_handle);
    if (msg != NULL)
    {
      of_get_pkt_mbuf_by_of_msg_and_free(msg);
     // msg->desc.free_cbk(msg);
    }

    return OF_ASYNC_MSG_CONSUMED;
  }
  dpid = pdatapath_info->dpid;

  app_msg=(struct of_app_msg *)calloc(1,sizeof(struct of_app_msg));
  app_msg->table_id=pkt_in->table_id;
  app_msg->datapath_handle=datapath_handle;
  app_msg->controller_handle=controller_handle;
  app_msg->domain_handle=domain_handle;
  app_msg->dpid=dpid;
  app_msg->pkt_in=pkt_in;
  app_msg->cbk_arg1=cbk_arg1;
  app_msg->cbk_arg2=cbk_arg2;

#ifdef CNTRL_APP_THREAD_SUPPORT
  cntlr_enqueue_msg_2_appxprt_conn(app_msg);
#else
  iretval = sm_app_arp_table_pkt_process(app_msg);
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
      //msg->desc.free_cbk(msg);
    }
    return OF_ASYNC_MSG_CONSUMED;
  }
}
void sm_arp_app_arp_req_timer_cbk(void *cbk_arg1,  //arp_req
                             void *cbk_arg2)  //datapath_handle
{

  uint32_t  dp_index, ii;
  for (dp_index = 0; dp_index < no_of_data_paths_attached_g; dp_index++)
  {
    if (dp_conf[dp_index].dp_handle == *((uint64_t*)cbk_arg2))
      break;
  }
  for (ii=0; ii< dp_conf[dp_index].no_of_arp_entries; ii++)
  {
    if( (dp_conf[dp_index].arp_table[ii].count < 3) && (dp_conf[dp_index].arp_table[ii].state != OF_CNTLR_ARP_STATE_LEARNED))
    {
      sm_arp_app_send_arp_req((cntlr_app_arp_req_t*) cbk_arg1, *((uint64_t*)cbk_arg2));
      dp_conf[dp_index].arp_table[ii].count++;
    }
    else if(dp_conf[dp_index].arp_table[ii].count >= 3)
    {
      dp_conf[dp_index].arp_table[ii].state = OF_CNTLR_ARP_STATE_TIMEOUT;       
      timer_stop_tmr(NULL,&dp_conf[dp_index].arp_table[ii].arpreq_timer);
      if(cbk_arg1 != NULL)
      {
        free(cbk_arg1);
        cbk_arg1 = NULL;
      }
      if(cbk_arg2 != NULL)
      {
        free(cbk_arg2);
        cbk_arg2 = NULL;
      }
    }
  }
}

int32_t sm_arp_app_send_arp_req(cntlr_app_arp_req_t *arp_req, uint64_t datapath_handle)
{
  uint8_t  arp_request[50];
  uint32_t arp_pkt_len = 0;
  //struct dprm_port_entry *dp_port_entry;
  //uint64_t    port_handle;
  uint8_t  *ucPtr;
  uint8_t       *action_start_loc;
  uint16_t      action_len;
  uint16_t       msg_len;
  struct of_msg *req_msg;
  int32_t  status = OF_SUCCESS;
  int32_t  retval = OF_SUCCESS;
  void *conn_info_p;
  int32_t flags=0;
  struct pkt_mbuf *mbuf = NULL;

  //struct of_msg *msg;

  /* Frame ARP request packet */
  arp_pkt_len = 0;
  ucPtr = arp_request;
  /* dest. mac address */ 
  memcpy(ucPtr, arp_req->target_hw_addr, OFP_ETH_ALEN);
  ucPtr += OFP_ETH_ALEN;
  arp_pkt_len += OFP_ETH_ALEN;
  /* src mac address */
  memcpy(ucPtr, arp_req->sender_hw_addr, OFP_ETH_ALEN);
  ucPtr += OFP_ETH_ALEN;
  arp_pkt_len += OFP_ETH_ALEN;
  /* eth type */
  (*(uint16_t *)ucPtr) = htons(OF_CNTLR_ETH_TYPE_ARP);
  ucPtr += 2;
  arp_pkt_len += 2;  
  /* hw type */
  (*(uint16_t *)ucPtr) = htons(arp_req->hw_type);
  ucPtr += 2;
  arp_pkt_len += 2;
  /* protocol type */
  (*(uint16_t *)ucPtr) = htons(arp_req->proto_type);
  ucPtr += 2;
  arp_pkt_len += 2;
  /* hw length */
  (*(uint8_t *)ucPtr) = arp_req->hw_len;
  ucPtr += 1;
  arp_pkt_len += 1;
  /* protocol length */
  (*(uint8_t *)ucPtr) = arp_req->proto_len;
  ucPtr += 1;
  arp_pkt_len += 1;
  /* op-code:ARP reply */
  (*(uint16_t *)ucPtr) = htons(OF_CNTLR_ARP_REQUEST);
  ucPtr += 2;
  arp_pkt_len += 2;
  /* sender mac address or resolved mac address */
  memcpy(ucPtr, arp_req->sender_hw_addr, OFP_ETH_ALEN);
  ucPtr += OFP_ETH_ALEN;
  arp_pkt_len += OFP_ETH_ALEN;
  /* sender ip */
  (*(uint32_t *)ucPtr) = htonl(arp_req->sender_ip);
  ucPtr += OF_CNTLR_IPV4_ADDRESS_LEN;
  arp_pkt_len += OF_CNTLR_IPV4_ADDRESS_LEN;
  /* target mac address */
  memcpy(ucPtr, arp_req->target_hw_addr, OFP_ETH_ALEN);
  ucPtr += OFP_ETH_ALEN;
  arp_pkt_len += OFP_ETH_ALEN;
  /* target ip */
  (*(uint32_t *)ucPtr) = htonl(arp_req->target_ip);
  ucPtr += OF_CNTLR_IPV4_ADDRESS_LEN;
  arp_pkt_len += OF_CNTLR_IPV4_ADDRESS_LEN;


  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,"\n%s %d arp request len: %d, source Ip:%x",__FUNCTION__,__LINE__, arp_pkt_len,arp_req->sender_ip);
  msg_len = (OFU_PACKET_OUT_MESSAGE_LEN+16+OFU_OUTPUT_ACTION_LEN+arp_pkt_len);


  of_alloc_pkt_mbuf_and_set_of_msg(mbuf, req_msg, OFPT_PACKET_OUT, msg_len, flags, status);
  if(status == FALSE)
  {
	  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR, "pkt_mbuf_alloc() failed");
	  status = OF_FAILURE;
	  return status;
  }

//  req_msg = ofu_alloc_message(OFPT_PACKET_OUT, msg_len);
  if (req_msg == NULL)
  {
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d OF message alloc error \r\n",__FUNCTION__,__LINE__);
    status = OF_FAILURE;
    return status;
  }
  retval=of_frame_pktout_msg(
      req_msg,
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
    of_get_pkt_mbuf_by_of_msg_and_free(req_msg);
    //req_msg->desc.free_cbk(req_msg);
    status = OF_FAILURE;
    return status;
  }
  ofu_start_pushing_actions(req_msg);
  retval = ofu_push_output_action(req_msg,arp_req->in_port,OFPCML_NO_BUFFER);
  if (retval != OFU_ACTION_PUSH_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d  Error in adding output action  err = %d\r\n",
        __FUNCTION__,__LINE__,retval);
    of_get_pkt_mbuf_by_of_msg_and_free(req_msg);
    //if (req_msg)
    {
      //  of_pkt_mbuf_free(mbuf);
    }
//    req_msg->desc.free_cbk(req_msg);
    status = OF_FAILURE;
    return status;
  }
  ofu_end_pushing_actions(req_msg,action_start_loc,&action_len);
  ((struct ofp_packet_out*)(req_msg->desc.start_of_data))->actions_len=htons(action_len);
  retval = of_add_data_to_pkt_and_send_to_dp(req_msg,
      datapath_handle,
      conn_info_p,
      arp_pkt_len,
      arp_request,
      NULL,NULL);
  if (retval != OF_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d  Error in sending packet err = %d\r\n", __FUNCTION__,__LINE__,retval);
 //   req_msg->desc.free_cbk(req_msg);
    of_get_pkt_mbuf_by_of_msg_and_free(req_msg);
    status = OF_FAILURE;
//	if (req_msg)
  //      of_pkt_mbuf_free(mbuf);
  }
  return OF_SUCCESS;
} 

int32_t
sm_app_arp_table_pkt_process(struct of_app_msg *app_msg)
{
  uint32_t ii, arp_index, jj ;
  uint32_t sender_ip, dest_ip, target_ip;
  uint32_t gateway_ip = 0,masked_dest_ip,meta_data_ip;
  uint64_t meta_data, meta_data_mask;
  char arp_mac[OF_CNTLR_APP_MAC_ADDR_LEN];
  char   iface_name[OF_CNTLR_APP_IFACE_NAME_LEN] = {};
  uint16_t out_port, priority;
  int32_t       retval;
  int32_t       status = OF_SUCCESS;
  struct dprm_port_entry *dp_port_entry;
  int32_t dpid; 
  uint8_t     *pkt_data; 
  uint64_t    port_handle;
  uint32_t    dp_index;
  uint8_t  sender_hw_addr[OFP_ETH_ALEN];
  uint8_t  target_hw_addr[OFP_ETH_ALEN];
  cntlr_app_arp_req_t arp_req;


  int64_t controller_handle;
  uint64_t domain_handle;
  uint64_t datapath_handle;
  //struct of_msg *msg;
  struct ofl_packet_in  *pkt_in;
  void *cbk_arg1;
  void *cbk_arg2;


  datapath_handle=  app_msg->datapath_handle;
  controller_handle=app_msg->controller_handle;
  domain_handle=app_msg->domain_handle;
  dpid=app_msg->dpid;
  pkt_in=app_msg->pkt_in;
  pkt_data = pkt_in->packet;
  cbk_arg1=app_msg->cbk_arg1;
  cbk_arg2=app_msg->cbk_arg2;


  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO, "%s:%d, ARP PKT HANDLER entered", __FUNCTION__, __LINE__);
  /* parse the pkt-in message and look up local arp table to match IP address, once the
     an entry matched with the given IP address then take corresponding MAC address
     and then add flow mod entry with IP and MAC address pair in ARP flow table by using
     NB API
   * Send the pkt out message 
   */
  //memcpy(sender_hw_addr, ntohl(*(uint32_t*)(pkt_data + OF_CNTLR_IPV4_HDR_OFFSET +6)),  OFP_ETH_ALEN);
  sender_ip = ntohl(*(uint32_t*)(pkt_data + OF_CNTLR_IPV4_HDR_OFFSET +
        OF_CNTLR_IPV4_SRC_IP_OFFSET)); 

  //  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"%s:sender_HW %x:%x:%x:%x:%x:%x,sender ip: %x",__FUNCTION__,sender_hw_addr[0],sender_hw_addr[1],sender_hw_addr[2],sender_hw_addr[3],
  //  sender_hw_addr[4], sender_hw_addr[5], sender_ip);
  //OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"%s:data %x",__FUNCTION__,pkt_data);

  target_ip = dest_ip = ntohl(*(uint32_t*)(pkt_data + OF_CNTLR_IPV4_HDR_OFFSET+
        OF_CNTLR_IPV4_DST_IP_OFFSET));  


  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,"\n%s %d srcip:%x, detsip:%x",__FUNCTION__,__LINE__, sender_ip, dest_ip);
  do
  {  

#if 1
    for (dp_index = 0; dp_index < no_of_data_paths_attached_g; dp_index++)
    {
      if (dp_conf[dp_index].dp_handle == datapath_handle)
      {
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO, "DP handle found");
        break;
      }
    }

    if (dp_index == no_of_data_paths_attached_g)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR, "No datapath found with given dp handle=%llx",datapath_handle);
      status = OF_FAILURE;
      break;
    }
#else
    pdp_conf =  (cntlr_sm_sw_conf_t *)cbk_arg1;
    if (pdp_conf == NULL)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, "miss entry callcbk arg1 is NULL..");
      status = OF_FAILURE;
      break;
    }
    else if (pdp_conf->dp_handle == datapath_handle)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, "invalid dp handle received..dp_handle=%llx",datapath_handle);
      status = OF_FAILURE;
      break;
    }
    dp_index = pdp_conf->dp_index;
#endif

    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,"\r\n%s:%d,PKT-IN RECEIVED FOR ARP MISS ENTRY ###########DP=%d",__FUNCTION__,__LINE__,dpid);
    for (ii=0; ii< dp_conf[dp_index].no_of_route_entries; ii++)
    {
      masked_dest_ip = (dest_ip & dp_conf[dp_index].rt_table[ii].rt_mask);
      if (dp_conf[dp_index].rt_table[ii].rt_dest == masked_dest_ip)
      {
        gateway_ip = dp_conf[dp_index].rt_table[ii].rt_gateway;
        strcpy(iface_name, dp_conf[dp_index].rt_table[ii].iface_name);
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,"\n%s %d iface name is %s",__FUNCTION__,__LINE__, iface_name);
        break;
      }
    }

    if (ii == dp_conf[dp_index].no_of_route_entries)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"\n%s %d no route entry matched..",__FUNCTION__,__LINE__);
      status = OF_FAILURE;
      break;
    }


    if (gateway_ip)
    {
      meta_data = (uint64_t)gateway_ip;
      meta_data_ip = target_ip = gateway_ip;
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,
          "\n metadata ip is gateway IP= %x, target ip is gateway ip:%x", meta_data_ip,target_ip);
    }
    else /* for derectly reachable route(local route) */
    {
      meta_data = (uint64_t)dest_ip;
      meta_data_ip = target_ip = dest_ip;
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,
          "\n metadata ip is dest IP= %x, target ip is dest ip: %x", meta_data_ip, target_ip);
    }
    meta_data_mask = (uint64_t)0xffffffff;

    for (ii=0; ii < dp_conf[dp_index].no_of_arp_entries; ii++)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,
          "\n metadata ip = %x, arp ip : %x",
          meta_data_ip,dp_conf[dp_index].arp_table[ii].arp_ip );

      if((meta_data_ip == dp_conf[dp_index].arp_table[ii].arp_ip) &&
          ((dp_conf[dp_index].arp_table[ii].state == OF_CNTLR_ARP_STATE_STATIC) ||
           (dp_conf[dp_index].arp_table[ii].state == OF_CNTLR_ARP_STATE_LEARNED))
        ) 
      {
        memcpy(arp_mac, dp_conf[dp_index].arp_table[ii].arp_mac, OF_CNTLR_APP_MAC_ADDR_LEN);
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,
            "\n MAC Address found at index:%d, mac address is: 0x%x:%x:%x:%x:%x:%x",
            ii, arp_mac[0],arp_mac[1],arp_mac[2],
            arp_mac[3], arp_mac[4], arp_mac[5]); 
        break;
      }
    }

    retval = dprm_get_port_handle(datapath_handle, iface_name, &port_handle);
    if (retval == DPRM_SUCCESS)
    {
      retval = get_datapath_port_byhandle(datapath_handle, port_handle, &dp_port_entry);
      if (retval != DPRM_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"Error in getting port  %s err=%d ",iface_name, retval);
        status = OF_FAILURE;
        break;
      }
      out_port = (uint16_t)(dp_port_entry->port_id);
    }
    else
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,
          "dprm get port handle by name=%s err=%d failed\r\n",iface_name, retval);
      status = OF_FAILURE;
      break;
    }

    if (ii == dp_conf[dp_index].no_of_arp_entries)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,
          "\n%s %d no arp entry matched.., sending arp request",__FUNCTION__,__LINE__);

      //FRAME ARP REQUEST AND SEND IT
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,
          "%s:%d:number of iface arp entries: %d",
          __FUNCTION__,__LINE__,dp_conf[dp_index].no_of_arp_entries);
      for (ii=0; ii < dp_conf[dp_index].no_of_arp_entries; ii++)
      {
        //Verifying if the record state for the target IP packet received.
        if (target_ip == dp_conf[dp_index].arp_table[ii].arp_ip)
        {
          switch(dp_conf[dp_index].arp_table[ii].state)
          {
            //For Learned and wait state the packet is processed/already being processed.
            //Hence return with out sending ARP Request for this packet again.
            case OF_CNTLR_ARP_STATE_SENT :  
              OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,
                  "%s:Arp request for this packet is being processed, waiting for ARP response",__FUNCTION__);
#if 0 // This code should be uncommented once timers are implemented for this local table
              //Packet is already sent and timedout. verify whether if the packet is already sent 3 times and drop the pkt.
              if(dp_conf[dp_index].arp_table[ii].count >= 3)
              {             
                dp_conf[dp_index].arp_table[ii].state = OF_CNTLR_ARP_STATE_TIMEOUT;
                OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_WARN,
                    "%s:Arp request for this packet is already sent 3 times, Hence dropping the packet",
                    __FUNCTION__,port_handle);
                status = OF_FAILURE;
                return status; 
              }
#endif
              dp_conf[dp_index].arp_table[ii].count++;
              break;
            case OF_CNTLR_ARP_STATE_LEARNED: 
              OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,
                  "%s:Arp request is already in learned state...",__FUNCTION__);
              status = OF_SUCCESS;
              return status; 
              //If the state is timeout, then the packet is previously sent but there is no reply is timed-out,
              //Hence send the ARP request depending on number of retries. 
            case OF_CNTLR_ARP_STATE_TIMEOUT: 
              OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_WARN,"%s:Arp request timedout",__FUNCTION__);
              status = OF_FAILURE;
              return status; 
          }
          /*Packet is already sent and timedout. verify whether if the packet is already sent 3 times
            and drop the pkt.*/
          OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,
              "%s:%d, Number of times the request is sent: %d",
              __FUNCTION__, __LINE__,dp_conf[dp_index].arp_table[ii].count)
            break;
        }
      }

      for(jj =0; jj < dp_conf[dp_index].no_of_iface_entries; jj++)
      {
        if(strncmp((char*)dp_conf[dp_index].iface_info[jj].port_name, iface_name,OF_CNTLR_APP_IFACE_NAME_LEN) == 0)
        {
          sender_ip = dp_conf[dp_index].iface_info[jj].ip_address;
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,
           "\n%s %d iface entry matched and source IP copied..:sender_ip:%x, iface_name: %s",
           __FUNCTION__,__LINE__, sender_ip, iface_name);
          break;
        }
      }
      if(jj == dp_conf[dp_index].no_of_iface_entries)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"\n%s %d no iface entry matched..",__FUNCTION__,__LINE__);
        status = OF_FAILURE;
        break;
      }


      /*2. Get Source MAC address for the interface using DPRM API */
      //sender mac address  got from dprm 
      memcpy(arp_req.sender_hw_addr,  dp_port_entry->port_info.hw_addr, OFP_ETH_ALEN);
      memcpy(sender_hw_addr,  dp_port_entry->port_info.hw_addr, OFP_ETH_ALEN);

      /*3. Frame ARP Request with the above information  and send to controller*/
      memset(arp_req.target_hw_addr, 0xff, OFP_ETH_ALEN);
      memset(target_hw_addr, 0, OFP_ETH_ALEN);
      arp_req.sender_ip = sender_ip;
      arp_req.target_ip = meta_data_ip;
      // arp_req.eth_type = eth_type;
      arp_req.hw_type = 1; //Corresponds to ethernet type
      arp_req.proto_type = 0x0800;
      arp_req.hw_len = 6; //ethernet address size or mac address
      arp_req.proto_len = 4; //ipv4 address size
      strcpy(arp_req.iface_name,iface_name); 
      arp_req.in_port = out_port;

      if (sm_arp_app_send_arp_req(&arp_req, datapath_handle) != OF_SUCCESS)
      {
        status = OF_FAILURE;
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"Failed to send arp request");
        break;
      }

      /*3. Add Table Entry - use existing table SM_APP_ARP_TABLE_ID */
      arp_index = dp_conf[dp_index].no_of_arp_entries;
      //If the ARP table reached maximum entries, then replace the entry that is already in 
      // 1. learned state (or)
      // 2. number of retries reached 3  --- Deepthi - Discuss. If the timed-out entry is removed then there are chances of resending the same pkt again
      if(arp_index == OF_CNTLR_APP_MAX_ARP_ENTRIES)
      {
        for (ii=0; ii< dp_conf[dp_index].no_of_arp_entries; ii++)
        {
          if((dp_conf[dp_index].arp_table[ii].state == OF_CNTLR_ARP_STATE_LEARNED) ||
              (dp_conf[dp_index].arp_table[ii].state == OF_CNTLR_ARP_STATE_TIMEOUT && 
               dp_conf[dp_index].arp_table[ii].count == 3))
          {
            arp_index = ii;
            dp_conf[dp_index].no_of_arp_entries--;  
            break;
          }
        }
      }
      //if didnot find any record that is learned or which is timedout after 3 retries, update 0th entry
      if(arp_index == OF_CNTLR_APP_MAX_ARP_ENTRIES) 
      {
        arp_index = 0;
        dp_conf[dp_index].no_of_arp_entries--;  
      }
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"\n%s %d arp index is: %d",__FUNCTION__,__LINE__, arp_index);
      // updating Table to wait state.
      for (ii=0; ii< dp_conf[dp_index].no_of_arp_entries; ii++)
      {
        if(dp_conf[dp_index].arp_table[ii].arp_ip == target_ip)
        {
          memcpy(target_hw_addr, dp_conf[dp_index].arp_table[ii].arp_mac,
              OF_CNTLR_APP_MAC_ADDR_LEN);
          break;
        }
      }

      if (ii == dp_conf[dp_index].no_of_arp_entries)
      {
        memcpy(dp_conf[dp_index].arp_table[arp_index].arp_mac,
            target_hw_addr, OF_CNTLR_APP_MAC_ADDR_LEN);
        dp_conf[dp_index].arp_table[arp_index].arp_ip = target_ip; 
        dp_conf[dp_index].arp_table[arp_index].count++;
        dp_conf[dp_index].arp_table[arp_index].state = OF_CNTLR_ARP_STATE_SENT;
        dp_conf[dp_index].no_of_arp_entries++;  

        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,
            "%s:%d:number of arp entries: %d",__FUNCTION__,__LINE__,
            dp_conf[dp_index].no_of_arp_entries);
        /*4a. Start timer for this request - ideally 14sec */
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,
            "%s:%d:updating table: arp_ip: %x",__FUNCTION__,__LINE__,
            dp_conf[dp_index].arp_table[arp_index].arp_ip );
        /*4b. If response is not received before the timeout, retry for 3 times*/
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,
            "%s:updating table  with mac: %x:%x:%x:%x:%x:%x",
            __FUNCTION__,dp_conf[dp_index].arp_table[arp_index].arp_mac[0],
            dp_conf[dp_index].arp_table[arp_index].arp_mac[1],
            dp_conf[dp_index].arp_table[arp_index].arp_mac[2],
            dp_conf[dp_index].arp_table[arp_index].arp_mac[3],
            dp_conf[dp_index].arp_table[arp_index].arp_mac[4],
            dp_conf[dp_index].arp_table[arp_index].arp_mac[5]);

        /*4a. Start timer for this request - ideally 14sec */
        /*4b. If response is not received before the timeout, retry for 3 times*/
        dp_conf[dp_index].arp_table[arp_index].arp_req_tmr = 
          (cntlr_app_arp_req_t *) calloc(1, sizeof(cntlr_app_arp_req_t ));

        dp_conf[dp_index].arp_table[arp_index].datapath_handle_tmr = 
          (uint64_t *)calloc(1, sizeof(uint64_t));

        if(dp_conf[dp_index].arp_table[arp_index].arp_req_tmr != NULL && 
            dp_conf[dp_index].arp_table[arp_index].datapath_handle_tmr != NULL)
        {
          memcpy(dp_conf[dp_index].arp_table[arp_index].arp_req_tmr,
              &arp_req, sizeof(cntlr_app_arp_req_t)); 
          memcpy(dp_conf[dp_index].arp_table[arp_index].datapath_handle_tmr,
              &datapath_handle, sizeof(uint64_t));
         /*
          retval = timer_create_and_start(NULL,
              &dp_conf[dp_index].arp_table[arp_index].arpreq_timer,
              CNTLR_PEIRODIC_TIMER,
              OF_CNTLR_ARP_REQ_TIMER,
              sm_arp_app_arp_req_timer_cbk,
              dp_conf[dp_index].arp_table[arp_index].arp_req_tmr, 
              dp_conf[dp_index].arp_table[arp_index].datapath_handle_tmr);
       */
        }
      }
      return status;

    }
    else
    {
      priority = sm_app_get_arp_entry_priority();
      if (sm_app_add_arp_entry(datapath_handle, dp_index, priority, meta_data, meta_data_mask,
            arp_mac, dp_port_entry->port_info.hw_addr, out_port) != OF_SUCCESS)
      {
        status = OF_FAILURE;
        break;
      }
      cntlr_send_barrier_request(datapath_handle);
      if (sm_app_frame_and_send_pktout_to_dp(datapath_handle,pkt_in) != OF_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"sending packet out failed");
        status = OF_FAILURE;
        break;
      }
    }
  }
  while(0);

  return status;
}


int32_t
sm_app_dp_ready_tap_handler(uint64_t  controller_handle,
                            uint64_t   domain_handle,
                            uint64_t   datapath_handle,
                            void       *cbk_arg1,
                            void       *cbk_arg2)
{

    sm_app_tap_dp_ready_handler(datapath_handle);
    return OF_SUCCESS;
}

int32_t
sm_app_dp_ready_event(uint64_t  controller_handle,
                      uint64_t   domain_handle,
                      uint64_t   datapath_handle,
                      void       *cbk_arg1,
                      void       *cbk_arg2)
{
  uint32_t ii;
  uint16_t eth_type;
  uint64_t dpid;
  struct dprm_datapath_entry *pdatapath_info;
  int32_t status = OF_SUCCESS;
  uint32_t dp_index;

  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"DATA PATH READY EVENT RECEIVED");
  if (get_datapath_byhandle(datapath_handle, &pdatapath_info) != DPRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_WARN, "No DP Entry with given datapath_handle=%llu",datapath_handle);
    return OF_FAILURE;
  }
  dpid = pdatapath_info->dpid;

  for (dp_index = 0; dp_index < no_of_data_paths_attached_g; dp_index++)
  {
    if (dp_conf[dp_index].dp_handle == datapath_handle)
    {
      break;
    }
  }

  if (dp_index == no_of_data_paths_attached_g)
  {
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_WARN, "No Datapath found with handle = %llx", datapath_handle);
    return OF_FAILURE;
  }

  do
  {
    /* Read route and arp entries from text/xml file */
    if (sm_app_read_route_entries(dp_index) != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"Reading route entries from text cofig file failed");
      status = OF_FAILURE;
      break;
    }

    if (sm_app_read_fw_acl_rules(dp_index) != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"Reading fw acl entries from text cofig file failed");         
      status = OF_FAILURE;
      break;
    }

    if (sm_app_read_of_sw_interfaces(dp_index) != OF_SUCCESS)
    {
	OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_WARN," Failed to read entries from interface info file",__FUNCTION__);
	return OF_FAILURE;
    }

    dp_conf[dp_index].ethtype_table_id = get_dptableid_by_domaintableid(datapath_handle, SM_APP_ETHTYPE_TABLE_ID);
    dp_conf[dp_index].mcast_class_table_id = get_dptableid_by_domaintableid(datapath_handle, SM_APP_MCAST_CLASSIFIER_TABLE_ID);
    dp_conf[dp_index].mcast_routing_table_id = get_dptableid_by_domaintableid(datapath_handle, SM_APP_MCAST_ROUTING_TABLE_ID);
    dp_conf[dp_index].session_table_id = get_dptableid_by_domaintableid(datapath_handle, SM_APP_SESSION_TABLE_ID);
    dp_conf[dp_index].l3_routing_table_id = get_dptableid_by_domaintableid(datapath_handle, SM_APP_L3ROUTING_TABLE_ID);
    dp_conf[dp_index].arp_table_id = get_dptableid_by_domaintableid(datapath_handle, SM_APP_ARP_TABLE_ID);

    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"table ids %d %d %d %d %d %d ",dp_conf[dp_index].ethtype_table_id,dp_conf[dp_index].mcast_class_table_id,dp_conf[dp_index].mcast_routing_table_id,dp_conf[dp_index].session_table_id,dp_conf[dp_index].l3_routing_table_id,dp_conf[dp_index].arp_table_id);


    /* Add proactively add all static routes red from test file into Table1 i.e switch routing table */  
    for (ii=0; ii < dp_conf[dp_index].no_of_route_entries; ii++)
    {
      if (sm_app_add_route_entry(datapath_handle, dp_index, route_entry_priority++,
            dp_conf[dp_index].rt_table[ii].rt_src,
            dp_conf[dp_index].rt_table[ii].rt_dest,
            dp_conf[dp_index].rt_table[ii].rt_mask,
            dp_conf[dp_index].rt_table[ii].rt_gateway,
            dp_conf[dp_index].rt_table[ii].rt_pmtu,
            dp_conf[dp_index].rt_table[ii].iface_name
            ) != OF_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"adding route entry=%d failed",ii);
        status = OF_FAILURE;
        break;
      }
    }
#if 0
    if(sm_app_add_route_table_miss_entry(datapath_handle,dp_index)!=OF_SUCCESS)
    {
      status = OF_FAILURE;
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"adding miss entries in route  failed ");
      break;
    }
#endif
    /* Add miss entries in session table */
    if (sm_app_add_missentry_with_output_action(datapath_handle, dp_conf[dp_index].session_table_id,
					OFPP_CONTROLLER, OFPCML_NO_BUFFER) != OF_SUCCESS)
    {
      status = OF_FAILURE;
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"adding miss entries in route and arp table failed ");
      break;
    }

    /* Add miss entries in eth type table */
    eth_type = 0x0806;/* ARP */
    if (sm_arp_app_add_proactive_flow(datapath_handle, dp_index, eth_type) != OF_SUCCESS)
    {
      status = OF_FAILURE;
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," failed adding flow entry with ethtype=%x ",eth_type);
      break;
    }

    eth_type = 0x0800;/* IPV4 */
    if (sm_arp_app_add_proactive_flow(datapath_handle, dp_index, eth_type) != OF_SUCCESS)
    {
      status = OF_FAILURE;
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," failed adding flow entry with ethtype=%x ",eth_type);
      break;
    }

    eth_type = 0x88cc;/* LLDP */
    if (sm_arp_app_add_proactive_flow(datapath_handle, dp_index,  eth_type) != OF_SUCCESS)
    {
      status = OF_FAILURE;
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," failed adding flow entry with ethtype=%x ",eth_type);
      break;
    }


    if (sm_app_arp_dp_ready_handler(datapath_handle, dp_index) != OF_SUCCESS)
    {
      	status = OF_FAILURE;
	OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"dp_ready_arp_handler failed.... ");
      break;
    }
    
    if (sm_app_igmp_dp_ready_handler(datapath_handle, dp_index) != OF_SUCCESS)
    {
      	status = OF_FAILURE;
	OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"dp_ready_igmp_handler failed...",eth_type);
      	break;
    }
  }
  while(0);

  return status;
}


//void cntlr_shared_lib_init(void)
void sample_app_init(void)
{
  uint32_t dp_index;

	strcpy(l3_fwd_domain_info.name,"L3_FWD_DOMAIN");
	strcpy(l3_fwd_domain_info.expected_subject_name_for_authentication,"fwd_domain@abc.com");

	if (dprm_register_forwarding_domain_notifications(DPRM_DOMAIN_ALL_NOTIFICATIONS,
	        sm_app_domain_event_cbk,
	        &l3_fwd_domain_info, NULL) != OF_SUCCESS)
	{
	    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"Registering callback to forward domain failed....");
	    return;
	}

	for (dp_index = 0;dp_index < OF_CNTLR_MAX_DATA_PATHS; dp_index++)	{
		dp_conf[dp_index].no_of_route_entries = 0;
		dp_conf[dp_index].no_of_arp_entries = 0;
		dp_conf[dp_index].no_of_fw_rules = 0;
		dp_conf[dp_index].no_of_iface_entries = 0;
		dp_conf[dp_index].no_of_mcast_route_entries = 0;
	}
	sess_entry_priority = 10;
	route_entry_priority = 10;
	arp_entry_priority = 10;
	no_of_data_paths_attached_g = 0;

	sm_app_config_resources();
	return;
}


void sm_app_domain_event_cbk(uint32_t notification_type,
                             uint64_t domain_handle,
                             struct   dprm_domain_notification_data notification_data,
                             void     *callback_arg1,
                             void     *callback_arg2)
{ 
  uint64_t dpid;
  struct dprm_datapath_entry *pdatapath_info;
  uint64_t datapath_handle = notification_data.datapath_handle;
  struct dprm_distributed_forwarding_domain_info  *domain_info=(struct dprm_distributed_forwarding_domain_info *)callback_arg1 ;

  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"domain name = %s  domain name of our intrest %s",
				notification_data.domain_name, domain_info->name);

  /* TAP_DOMAIN or L3_FWD_DOMAIN belongs to "SAMPLE_L3_FWD_TTP"*/
  if(strcmp(notification_data.ttp_name, "SAMPLE_L3_FWD_TTP"))
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"notification is  not for l3_fwd_ttp.");
    return;
  }


  switch(notification_type)
  {
    case DPRM_DOMAIN_ADDED:
      {
         if (strcmp(notification_data.domain_name,"TAP_DOMAIN") == 0)
         {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"DPRM_DOMAIN_ADDED for TAP_DOMAIN");
            if (cntlr_register_asynchronous_event_hook(domain_handle,
                                                   DP_READY_EVENT,
                                                   1, 
                                                   sm_app_dp_ready_tap_handler,
                                                   NULL, NULL) == OF_FAILURE)
            {
               OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Register DP Ready event cbk failed for TAP_DOMAIN");
            }
             
         }
         else if (strcmp(notification_data.domain_name, domain_info->name) == 0)
         {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"DPRM_DOMAIN_ADDED for L3_FWD_DOMAIN");
            if (cntlr_register_asynchronous_event_hook(domain_handle,
                                                   DP_READY_EVENT,
                                                   1, 
                                                   sm_app_dp_ready_event,
                                                   NULL, NULL) == OF_FAILURE)
            {
               OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Register DP Ready event cbk failed for L3_FWD_DOMAIN");
            }
           
            if (of_register_asynchronous_message_hook(domain_handle,
                  SM_APP_ETHTYPE_TABLE_ID,/*Table 0*/
                  OF_ASYNC_MSG_PACKET_IN_EVENT,
                  SM_APP_PKT_IN_PRIORITY,
                  sm_app_arp_request_n_response_handler,NULL,NULL) != OF_SUCCESS)
            {  
              OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,"registering ARP Reply cbk");
            }

 	    if (of_register_asynchronous_message_hook(domain_handle,
				SM_APP_MCAST_CLASSIFIER_TABLE_ID, /* Table 1*/
				OF_ASYNC_MSG_PACKET_IN_EVENT,
				SM_APP_PKT_IN_PRIORITY,
	 			sm_app_igmp_pkt_in_rcvd,NULL,NULL) != OF_SUCCESS)
    	    {
			OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,
				"registering sample apps to Asyc message of mcast classifier");
	    }

            if (of_register_asynchronous_message_hook(domain_handle,
                  SM_APP_SESSION_TABLE_ID, /* Table 1*/
                  OF_ASYNC_MSG_PACKET_IN_EVENT,
                  SM_APP_PKT_IN_PRIORITY,
                  sm_app_sess_table_miss_entry_pkt_rcvd,NULL,NULL) != OF_SUCCESS)
            {
              OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,"registering sample apps to Asyc message of session table");
            }

           if (of_register_asynchronous_message_hook(domain_handle,
                  SM_APP_ARP_TABLE_ID, /* Table 3*/
                  OF_ASYNC_MSG_PACKET_IN_EVENT,
                  SM_APP_PKT_IN_PRIORITY,
                  sm_app_arp_table_miss_entry_pkt_rcvd,NULL,NULL) != OF_SUCCESS)
            {
              OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO,"registering sample apps to Asyc message of arp table");
            }
         }
         else
         {
            OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," Not intrested for this domain=%s",notification_data.domain_name);
            return;
         }
         break;
      }
    case DPRM_DOMAIN_DELETE:
      {
        if(strcmp(notification_data.domain_name,domain_info->name) != 0)
           return;

        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"DPRM_DOMAIN_DELETED");
        break;
      }

   case DPRM_DOMAIN_DATAPATH_ATTACHED:
      {
        if(strcmp(notification_data.domain_name,domain_info->name) != 0)
           return;

        uint32_t dpinfo_index;
        //uint32_t len, totlen = 0;
        if (get_datapath_byhandle(datapath_handle, &pdatapath_info) != DPRM_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR, "No DP Entry with given datapath_handle=%llu",datapath_handle);
	  break;
        }
        dpid = pdatapath_info->dpid;
        dpinfo_index = no_of_data_paths_attached_g;
        if (sm_app_attach_dpid_to_conf_files(dpinfo_index, dpid, datapath_handle) != OF_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_WARN, "Error in creating and adding dpid to conf files");
        }

        no_of_data_paths_attached_g++;
        break;
      }
    case DPRM_DOMAIN_DATAPATH_DETACH:
      {
        if(strcmp(notification_data.domain_name,domain_info->name) != 0)
           return;

        if (get_datapath_byhandle(datapath_handle, &pdatapath_info) != DPRM_SUCCESS)
        {
          OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_WARN, "No DP Entry with given datapath_handle=%llu",datapath_handle);
	  break;
        }
        dpid = pdatapath_info->dpid;
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG," DPRM_DOMAIN_DATAPATH_DETACHED datapath handle=%llu dpid=%llx noOfDps=%d",
            datapath_handle,dpid,no_of_data_paths_attached_g);
        no_of_data_paths_attached_g--;
        break;
      }
    default:
      {
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_WARN, "Invalid domain event=%d",notification_type);
      }
  }

  return;
}



void sm_app_init(uint64_t  domain_handle)
{  
  int32_t status = OF_SUCCESS; 

  cntlr_shared_lib_init();

	if (cntlr_register_asynchronous_event_hook(domain_handle, DP_READY_EVENT, 1, 
									sm_app_dp_ready_event, NULL, NULL) == OF_FAILURE)
  {
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_WARN,"Register DP Ready event cbk failed");
    return;
  } 

	if (status == OF_SUCCESS)	{
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, "APPLICATION INITIALISED");
  }
	else	{
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, "APPLICATION INITIALISATION FAILED");
  }
	return;
}

/* string of two chars as input and unsigned char returns in output*/
unsigned char atox(char *cPtr)
{
  unsigned char ucbyte = 0;
  char str[] = "0123456789ABCDEF";
  unsigned int digit;
  char cCmp;

  if (strlen(cPtr) > 2)
  {
    *(cPtr + 2) = '\0';
  }

  while (*(cPtr))
  {
    if (isalpha(*cPtr) && islower(*cPtr))
    {
      cCmp = toupper(*cPtr);
    }
    else
    {
      cCmp = *(cPtr);
    }

    for (digit = 0; digit < (strlen(str)); digit++)
    {
      if (str[digit] == cCmp)
      {
        if (!(*(cPtr+1)))
          ucbyte += digit;
        else
          ucbyte += 16*digit;

        break;
      }
    }
    cPtr++;
  }

  return ucbyte;
}


static int32_t sm_app_read_route_entries(uint32_t dp_index)
{ 
  int32_t n = 0, ii = 0, len;
  char line[150];
  char src_ipaddr[18] = {};
  char dst_ipaddr[18] = {};
  char net_mask[18] = {};
  char gateway[18] = {};
  char pmtu[18] = {};
  char iface[OF_CNTLR_APP_IFACE_NAME_LEN] = {};
  char *token = NULL;
  FILE *fr;        /* declare the file pointer */


  fr = fopen(dp_conf[dp_index].rt_arp_conf_file, "rt");/* open the file for reading */
  if (fr == NULL)	{
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG," No more files:%s to read errno=%d",dp_conf[dp_index].rt_arp_conf_file,errno);
    perror(NULL);
    return OF_FAILURE;
  }

  while(fgets(line, 150, fr) != NULL)
  {
    if ((line[0] == '#') || ((line[0] == '/') && (line[1] == '*'))) {
      continue;
    }

    token = (char *)strtok(line,"-");
    do
    {
      if (!strncmp(token,"srcip",strlen("srcip")))	{
        strcpy(src_ipaddr, (token+strlen("srcip ")));
        dp_conf[dp_index].rt_table[n].rt_src = inet_network(src_ipaddr);
        if (token[strlen(token)-1] == '\n')
          n++; 
      }
      else if (!strncmp(token,"destip",strlen("destip")))	{
        strcpy(dst_ipaddr, (token+strlen("destip ")));
        dp_conf[dp_index].rt_table[n].rt_dest = inet_network(dst_ipaddr);
        if (token[strlen(token)-1] == '\n')
          n++; 
      }
      else if (!strncmp(token,"mask",strlen("mask")))	{
        strcpy(net_mask, (token+strlen("mask ")));
        dp_conf[dp_index].rt_table[n].rt_mask = inet_network(net_mask);
        if (token[strlen(token)-1] == '\n')
          n++; 
      }
      else if (!strncmp(token,"gw",strlen("gw")))	{
        strcpy(gateway, (token+strlen("gw ")));
        dp_conf[dp_index].rt_table[n].rt_gateway = inet_network(gateway);
        if (token[strlen(token)-1] == '\n')
          n++; 
      }
      else if (!strncmp(token,"iface",strlen("iface")))	{   
        strncpy(iface, (token+strlen("iface ")), OF_CNTLR_APP_IFACE_NAME_LEN);
        ii = 0;
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, "token = %s iface=%s",token, iface);
        while (iface[ii] != '\0')
        {
          if ((iface[ii] == ' ') ||  (iface[ii] == '-') || (iface[ii] == '\n'))
          {
            iface[ii] = '\0';
            break;
          }
          ii++;
        }
        strcpy(dp_conf[dp_index].rt_table[n].iface_name, iface);
        if (token[strlen(token)-1] == '\n')
          n++; 
      }
      else if (!strncmp(token,"gb",strlen("gb")) || !strncmp(token,"10g",strlen("10g")))	{
        strcat(iface, "-"); 
        len = strlen(iface);
        strcpy(iface+len, token); 
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, "token = %s iface=%s",token, iface);
        ii=len;
        while (iface[ii] != '\0')
        { 
          if ((iface[ii] == ' ') ||  (iface[ii] == '-') || (iface[ii] == '\n'))
          {
            iface[ii] = '\0';
            break;
          }
          ii++;
        }
        strcpy(dp_conf[dp_index].rt_table[n].iface_name, iface);
        if (token[strlen(token)-1] == '\n')
          n++; 
      }
      else if(!strncmp(token,"pmtu",strlen("pmtu")))	{
        strcpy(pmtu, (token+strlen("pmtu ")));
        dp_conf[dp_index].rt_table[n].rt_pmtu = (uint32_t)atoi(pmtu);
        if (token[strlen(token)-1] == '\n')
          n++;
      }
    }while((token = (char *)strtok(NULL,"-")) != NULL);
  }
  dp_conf[dp_index].no_of_route_entries = n;
  sm_app_print_l3routing_table(dp_index);

  fclose(fr);
  return OF_SUCCESS; 
}

void sm_app_print_l3routing_table(uint32_t dp_index)
{
  uint32_t ii;

  printf("\nDP%llx_ROUTING Table:",dp_conf[dp_index].dp_id);
  printf("\r\nTotal No Of Entries:%d",dp_conf[dp_index].no_of_route_entries);
  puts("\n _____________________________________________________________");
  puts("\n SRC-IP   DST-IP   MASK     GATEWAY       OUT-IFACE  PATH-MTU ");
  puts("\n -------------------------------------------------------------");
  for (ii = 0; ii < dp_conf[dp_index].no_of_route_entries; ii++)	{
    printf("\n %x   %x    %x    %x     %s  %d",dp_conf[dp_index].rt_table[ii].rt_src,
                                                    dp_conf[dp_index].rt_table[ii].rt_dest,
                                                    dp_conf[dp_index].rt_table[ii].rt_mask,
                                                    dp_conf[dp_index].rt_table[ii].rt_gateway,
                                                    dp_conf[dp_index].rt_table[ii].iface_name,
                                                    dp_conf[dp_index].rt_table[ii].rt_pmtu);
  }
  puts("\n -------------------------------------------------------------");
  return;
}

 
int32_t sm_app_add_route_entry(uint64_t dp_handle, uint32_t dp_index, uint16_t priority,
                               uint32_t rt_src, uint32_t rt_dest,
                               uint32_t rt_mask, uint32_t rt_gateway,
                               uint32_t rt_pmtu, char *iface_name)
{
  struct      of_msg *msg=NULL;
  uint16_t    msg_len;
  uint16_t    instruction_len;
  uint8_t     *inst_start_loc = NULL;
  uint16_t    action_len;
  uint8_t     *action_start_loc = NULL;
  uint16_t    match_fd_len=0;
  uint8_t     *match_start_loc = NULL;
  uint16_t    flags;
  uint64_t    meta_data, meta_data_mask;
  int32_t     retval = OF_SUCCESS;
  int32_t     status = OF_SUCCESS;
  void *conn_info_p;

  if (rt_gateway)	{ 
    msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+16+
        OFU_ETH_TYPE_FIELD_LEN+
        OFU_IPV4_SRC_FIELD_LEN+OFU_IPV4_DST_FIELD_LEN+
        OFU_DEC_IP_TTL_ACTION_LEN+
        OFU_WRITE_META_DATA_INSTRUCTION_LEN+
		OFU_GOTO_TABLE_INSTRUCTION_LEN);

#ifdef FSLX_COMMON_AND_L3_EXT_SUPPORT
    if (is_fsl_extensions_loaded_g)	{
      msg_len += (FSLX_ACTION_IP_FRAGMENT_LEN + FSLX_ACTION_ICMPV4_TIME_EXCEED_LEN+FSLX_ACTION_WRITE_METADATA_FROM_PKT_LEN);
    }
#endif   
  }
  else	{
    msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+16+
        OFU_ETH_TYPE_FIELD_LEN+OFU_IPV4_SRC_FIELD_LEN+OFU_IPV4_DST_FIELD_LEN+
        OFU_DEC_IP_TTL_ACTION_LEN+OFU_GOTO_TABLE_INSTRUCTION_LEN);
#ifdef FSLX_COMMON_AND_L3_EXT_SUPPORT
    if (is_fsl_extensions_loaded_g)	{
      msg_len += (FSLX_ACTION_ICMPV4_TIME_EXCEED_LEN+FSLX_ACTION_IP_FRAGMENT_LEN+FSLX_ACTION_WRITE_METADATA_FROM_PKT_LEN);
    }
#endif
  }

  do
  {
    msg = ofu_alloc_message(OFPT_FLOW_MOD, msg_len);
    if (msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF message alloc error ");
      status = OF_FAILURE;
      return status;
    }

    flags = 0;
    flags |= OFPFF_RESET_COUNTS;

    /* zero idle and hard timeout makes entry permanet */
    retval = of_create_add_flow_entry_msg(msg,
        dp_handle,
        dp_conf[dp_index].l3_routing_table_id, /*Table Id 4*/
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
    }

    ofu_start_setting_match_field_values(msg);
    uint16_t eth_type = 0x0800; /*Eth type is IP*/
    retval = of_set_ether_type(msg,&eth_type);
    if (retval != OFU_SET_FIELD_SUCCESS)	{
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF Set IPv4 src field failed,err = %d",retval);
      status = OF_FAILURE;
      break;
    }

    retval = of_set_ipv4_source(msg, &rt_src,FALSE,NULL);
    if (retval != OFU_SET_FIELD_SUCCESS)	{
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF Set IPv4 src field failed,err = %d",retval);
      status = OF_FAILURE;
      break;
    }

    retval = of_set_ipv4_destination(msg, &rt_dest,TRUE,&rt_mask);
    if (retval != OFU_SET_FIELD_SUCCESS)	{
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF Set IPv4 dest field failed,err = %d",retval);
      status = OF_FAILURE;
      break;
    }
    ofu_end_setting_match_field_values(msg,match_start_loc,&match_fd_len);
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, "Match field len %d",match_fd_len);
    ofu_start_pushing_instructions(msg, match_fd_len);
    retval = ofu_start_pushing_apply_action_instruction(msg);
    if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)	{
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in adding apply action instruction err = %d",retval);
      status = OF_FAILURE;
      break;
    }
    ofu_start_pushing_actions(msg);
    retval = ofu_push_dec_ipv4_ttl_action(msg);
    if (retval != OFU_ACTION_PUSH_SUCCESS)	{
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in dec ttl value  err = %d",retval);
      status = OF_FAILURE;
      break;
    }

#ifdef FSLX_COMMON_AND_L3_EXT_SUPPORT
#if 0
    if (unlikely(is_fsl_extensions_loaded_g))	{
      retval = fslx_action_send_icmpv4_time_exceed_err_msg(msg,OF_CNTLR_ICMPV4_TTL_EXPIRED);
      if (retval != OFU_ACTION_PUSH_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"Error in pushing ipv4 ttl zero action  err = %d",retval);
        status = OF_FAILURE;
        break;
      }
    }
#endif
#endif

#ifdef FSLX_COMMON_AND_L3_EXT_SUPPORT
    if (unlikely(is_fsl_extensions_loaded_g))	{
       retval = fslx_action_ipfragment(msg, rt_pmtu);
       if (retval != OFU_ACTION_PUSH_SUCCESS)
       {
          OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"Error in pushing ip frag action  err = %d",retval);
          status = OF_FAILURE;
          break;
       }
    }
#endif
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"total actions len = %d for this instruction", action_len);

    if (rt_gateway)	{
      ofu_end_pushing_actions(msg,action_start_loc,&action_len);
    ((struct ofp_instruction_actions*)(msg->desc.ptr2))->len = htons(action_len);
      meta_data = (uint64_t)rt_gateway;
      meta_data_mask = (uint64_t)0xffffffff;
      retval = ofu_push_write_meta_data_instruction(msg, meta_data, meta_data_mask);
		
		if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)	{
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF write meta data instruction failed,err = %d",retval);
        status = OF_FAILURE;
        break;
      }
    }
#ifdef FSLX_COMMON_AND_L3_EXT_SUPPORT
    else        {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,
                 "gateway is zero call set metadata from pkt field...");
      if (unlikely(is_fsl_extensions_loaded_g))	{
          retval = fslx_action_write_metadata_from_pkt(msg, (uint8_t)OFPXMT_OFB_IPV4_DST, (uint64_t)0xffffffff);
          if (retval != OFU_ACTION_PUSH_SUCCESS)	{
              OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,
              "OF set meta data from packet field failed..,err = %d",retval);
              status = OF_FAILURE;
              break;
           }
      }
    ofu_end_pushing_actions(msg,action_start_loc,&action_len);
    ((struct ofp_instruction_actions*)(msg->desc.ptr2))->len = htons(action_len);
    }
#endif
    retval = ofu_push_go_to_table_instruction(msg, dp_conf[dp_index].arp_table_id);
    if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)	{
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"go to table instruction failed err = %d",retval);
      status = OF_FAILURE;
      break;
    }
    ofu_end_pushing_instructions(msg,inst_start_loc,&instruction_len);
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"instruction len = %d", instruction_len);

    retval = of_cntrlr_send_request_message(msg, dp_handle, conn_info_p, NULL, NULL);
    if (retval != OF_SUCCESS)	{
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error adding flow entry in route table = %d",retval);
      status = OF_FAILURE;
      return status;
    }
    meta_data = 0;
  }
  while(0);

  if (status == OF_FAILURE)	{
            msg->desc.free_cbk(msg);
  }

  return status;
}

int32_t
sm_app_add_session_entry(uint64_t datapath_handle, uint32_t dp_index, uint16_t priority,
                         uint32_t source_ip, uint32_t dest_ip,
                         uint16_t src_port, uint16_t dst_port,
                         uint8_t protocol, uint32_t in_port, 
                         uint8_t icmp_type, uint8_t icmp_code,
                         uint32_t snat_ip, uint32_t dnat_ip)
{
  struct of_msg *msg;
  uint16_t      msg_len;
  uint16_t      instruction_len;
  uint8_t     *inst_start_loc = NULL;
  uint16_t      actions_len;
  uint8_t     *action_start_loc = NULL;
  uint16_t      match_fd_len=0;
  uint8_t     *match_start_loc = NULL;
  uint16_t      flags;
  int32_t       retval = OF_SUCCESS;
  int32_t       status = OF_SUCCESS;
  void *conn_info_p;

  switch (protocol)
  { 
    case OF_CNTLR_IP_PROTO_TCP:
      {
        msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+16+
            OFU_ETH_TYPE_FIELD_LEN+
            OFU_IPV4_SRC_FIELD_LEN+OFU_IPV4_DST_FIELD_LEN+
            OFU_TCP_SRC_FIELD_LEN+OFU_TCP_DST_FIELD_LEN+
            OFU_IP_PROTO_FIELD_LEN+
            OFU_GOTO_TABLE_INSTRUCTION_LEN);
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, "non icmp Msg Len %d",msg_len);
        break;
      }
    case OF_CNTLR_IP_PROTO_UDP:
      {
        msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+16+
            OFU_ETH_TYPE_FIELD_LEN+
            OFU_IPV4_SRC_FIELD_LEN+OFU_IPV4_DST_FIELD_LEN+
            OFU_UDP_SRC_FIELD_LEN+OFU_UDP_DST_FIELD_LEN+
            OFU_IP_PROTO_FIELD_LEN+
            OFU_GOTO_TABLE_INSTRUCTION_LEN);
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, "non icmp Msg Len %d",msg_len);
        break;
      }
    case OF_CNTLR_IP_PROTO_ICMP:
      {
        msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+16+
            OFU_ETH_TYPE_FIELD_LEN+
            OFU_IPV4_SRC_FIELD_LEN+OFU_IPV4_DST_FIELD_LEN+
            OFU_ICMPV4_TYPE_FIELD_LEN+OFU_ICMPV4_CODE_FIELD_LEN+
            OFU_IP_PROTO_FIELD_LEN+
            OFU_GOTO_TABLE_INSTRUCTION_LEN);
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, "icmp Msg Len %d",msg_len);
        break;
      }
    default:
      {
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_WARN,"Invalid protocol=%d ",protocol);
        return OF_FAILURE;
      }
  }

  /* For both nat_ip length will be same */
  if ((snat_ip) || (dnat_ip))	{
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_MOD," SNAT/DNAT is enabled update msg_len");
      msg_len += OFU_SET_FIELD_WITH_IPV4_SRC_FIELD_ACTION_LEN; 
  }

  do
  {
    msg = ofu_alloc_message(OFPT_FLOW_MOD, msg_len);
    if(msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF message alloc error ");
      status = OF_FAILURE;
      return status;
    }

    flags = 0;
    flags |= OFPFF_RESET_COUNTS;

    retval = of_create_add_flow_entry_msg(msg,
        datapath_handle,
        dp_conf[dp_index].session_table_id, /*Table Id 3*/
        sess_entry_priority, /*priority*/
        OFPFC_ADD,
        0, /*coockie*/
        0,/*Coockie amsk*/
        OFP_NO_BUFFER,
        flags,
        0, /*No Hard timeout*/
        15, /*Idle timeout*/
        &conn_info_p
        );

    if (retval != OFU_ADD_FLOW_ENTRY_TO_TABLE_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in adding miss entry  = %d",retval);
      status = OF_FAILURE;
    }
    ofu_start_setting_match_field_values(msg);
#if 0
    retval = of_set_in_port(msg, &in_port);
    if(retval != OFU_SET_FIELD_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"OF Input Port set failed ,err = %d",retval);
      status = OF_FAILURE;
      break;
    }
#endif

    uint16_t eth_type = 0x0800; /*Eth type is IP*/
    retval = of_set_ether_type(msg,&eth_type);/*setting ethtype is mandatory before setting ips*/
    if(retval != OFU_SET_FIELD_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF Set IPv4 src field failed,err = %d",retval);
      status = OF_FAILURE;
      break;
    }

    retval = of_set_ipv4_source(msg,&source_ip,FALSE,NULL);
    if(retval != OFU_SET_FIELD_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF Set IPv4 src field failed,err = %d",retval);
      status = OF_FAILURE;
      break;
    } 

    retval = of_set_ipv4_destination(msg, &dest_ip, FALSE, NULL);
    if (retval != OFU_SET_FIELD_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF Set IPv4 dest field failed,err = %d",retval);
      status = OF_FAILURE;
      break;
    }

    retval = of_set_protocol(msg, &protocol);/*it is mandatory to set before setting ports*/
    if (retval != OFU_SET_FIELD_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF Set ip protocol field failed,err = %d",retval);
      status = OF_FAILURE;
      break;
    }
    switch (protocol)
    {
      case OF_CNTLR_IP_PROTO_ICMP:
        {
          retval = of_set_icmpv4_type(msg, &icmp_type);
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF Set Src Port field failed,err = %d",retval);
            return OF_FAILURE;
          }

          retval = of_set_icmpv4_code(msg, &icmp_code);
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF Set Src Port field failed,err = %d",retval);
            return OF_FAILURE;
          }
          break;
        }
      case OF_CNTLR_IP_PROTO_UDP:
        {
          retval = of_set_udp_source_port(msg, &src_port);
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF Set udp Src Port field failed,err = %d",retval);
            return OF_FAILURE;
          }

          retval = of_set_udp_destination_port(msg, &dst_port);
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF Set udp Dst Port field failed,err = %d",retval);
            return OF_FAILURE;
          }
          break;
        }
      case OF_CNTLR_IP_PROTO_TCP:
        {
          retval = of_set_tcp_source_port(msg, &src_port);
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF Set Src Port field failed,err = %d",retval);
            return OF_FAILURE;
          }

          retval = of_set_tcp_destination_port(msg, &dst_port);
          if (retval != OFU_SET_FIELD_SUCCESS)
          {
            OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF Set Src Port field failed,err = %d",retval);
            return OF_FAILURE;
          }
          break;
        }
    }
    ofu_end_setting_match_field_values(msg, match_start_loc, &match_fd_len);
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"Match field len %d",match_fd_len);

//    msg->desc.ptr2=  msg->desc.ptr1 + match_fd_len;
    ofu_start_pushing_instructions(msg,match_fd_len);
    if ((snat_ip) || (dnat_ip))	{
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_MOD," SNAT/DNAT is enabled");
		ofu_start_pushing_apply_action_instruction(msg);
		ofu_start_pushing_actions(msg);
		if (snat_ip)	{
		      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_MOD," SNAT is enabled");
		  ofu_push_set_ipv4_src_addr_in_set_field_action(msg, &snat_ip);
		}

		if (dnat_ip)	{
		      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_MOD," DNAT is enabled");
		  ofu_push_set_ipv4_dst_addr_in_set_field_action(msg, &dnat_ip);
		}
		ofu_end_pushing_actions(msg,action_start_loc, &actions_len);
		((struct ofp_instruction_actions*)(msg->desc.ptr2))->len = htons(actions_len); 
    }
    retval = ofu_push_go_to_table_instruction(msg, dp_conf[dp_index].l3_routing_table_id);
    if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"go to table instruction failed err = %d",retval);
      status = OF_FAILURE;
      break;
    }
    ofu_end_pushing_instructions(msg,inst_start_loc,&instruction_len);
    ((struct ofp_instruction_actions*)(msg->desc.ptr2))->len = htons(instruction_len);

    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"instruction len = %d", instruction_len);

    retval=of_cntrlr_send_request_message(
        msg,
        datapath_handle,
        conn_info_p,
        NULL,
        NULL);
    if (retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in adding miss entry  = %d",retval);
      status = OF_FAILURE;
      return status;
    }
  }
  while(0);

  if (status == OF_FAILURE)	{
            msg->desc.free_cbk(msg);
  }

  return status;
}
/* Adds miss entry in the route table */
static int32_t
sm_app_add_route_table_miss_entry(uint64_t datapath_handle, uint32_t dp_index)
{
  struct      of_msg *msg;
  uint16_t    msg_len;
  uint16_t    instruction_len;
  uint8_t     *inst_start_loc = NULL;
  uint16_t    action_len;
  uint8_t     *action_start_loc = NULL;
  uint16_t    match_fd_len;
  uint8_t     *match_start_loc = NULL;
  uint16_t    flags;
  int32_t     retval = OF_SUCCESS;
  int32_t     status = OF_SUCCESS;
  void *conn_info_p;

  msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+16+
      OFU_APPLY_ACTION_INSTRUCTION_LEN+FSLX_ACTION_ICMPV4_DEST_UNREACH_LEN);

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Msg Len %d\r\n",msg_len);
  do
  {
    msg = ofu_alloc_message(OFPT_FLOW_MOD, msg_len);
    if (msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"OF message alloc error ");
      status = OF_FAILURE;
      return status;
    }

    flags = 0;
    flags |= OFPFF_RESET_COUNTS;
    retval = of_create_add_flow_entry_msg(msg,
        datapath_handle,
        dp_conf[dp_index].l3_routing_table_id, /*Table Id 4*/
        0, /* Low priority*/
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
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in adding miss entry route table = %d",retval);
      status = OF_FAILURE;
    } 
    /*TBD, if we are not pusshing any match fields,this might be not rquired*/ 
    ofu_start_setting_match_field_values(msg);
    ofu_end_setting_match_field_values(msg,match_start_loc,&match_fd_len);

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Match field len %d",match_fd_len);

//    msg->desc.ptr2=  msg->desc.ptr1 + match_fd_len;

    ofu_start_pushing_instructions(msg, match_fd_len);
    retval = ofu_start_pushing_apply_action_instruction(msg);
    if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in adding apply action instruction err = %d",retval);
      status = OF_FAILURE;
      break;
    }
    ofu_start_pushing_actions(msg);
    retval = fslx_action_send_icmpv4_dest_unreach_err_msg(msg, OF_CNTLR_ICMPV4_DEST_NW_UNREACHABLE, 0);
    if (retval != OFU_ACTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in adding  action  err = %d",retval);
      status = OF_FAILURE;
      break;
    }
    ofu_end_pushing_actions(msg,action_start_loc,&action_len);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"action len = %d", action_len);
    ((struct ofp_instruction_actions*)(msg->desc.ptr2))->len = htons(action_len);
    ofu_end_pushing_instructions(msg,inst_start_loc,&instruction_len);

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"instruction len = %d", instruction_len);

    retval = of_cntrlr_send_request_message(
        msg,
        datapath_handle,
        conn_info_p,
        NULL,
        NULL);
    if (retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR," Error in adding miss entry route table = %d",retval);
      status = OF_FAILURE;
      return status;
    } 
  }
  while(0);

  if (status == OF_FAILURE)	{
            msg->desc.free_cbk(msg);
  }

  return status;
}



/* This function adds miss entries in routing and arp tables */
static int32_t
sm_app_add_sessoin_table_miss_entry(uint64_t datapath_handle, uint32_t dp_index)
{
  struct      of_msg *msg;
  uint16_t    msg_len;
  uint16_t    instruction_len;
  uint8_t     *inst_start_loc = NULL;
  uint16_t    action_len;
  uint8_t     *action_start_loc = NULL;
  uint16_t    match_fd_len;
  uint8_t     *match_start_loc = NULL;
  uint16_t    flags;
  int32_t     retval = OF_SUCCESS;
  int32_t     status = OF_SUCCESS;
  void *conn_info_p;

  msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+16+
      OFU_APPLY_ACTION_INSTRUCTION_LEN+OFU_OUTPUT_ACTION_LEN);

  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, "Msg Len %d\r\n",msg_len);
  do
  {
    msg = ofu_alloc_message(OFPT_FLOW_MOD, msg_len);
    if (msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF message alloc error ");
      status = OF_FAILURE;
      return status;
    }

    flags = 0;
    flags |= OFPFF_RESET_COUNTS;
    retval = of_create_add_flow_entry_msg(msg,
        datapath_handle,
        dp_conf[dp_index].session_table_id, /*Table Id 3*/
        0, /* Low priority*/
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
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in adding miss entry session table = %d",retval);
      status = OF_FAILURE;
    } 
    /*TBD, if we are not pusshing any match fields,this might be not rquired*/ 
    ofu_start_setting_match_field_values(msg);
    ofu_end_setting_match_field_values(msg,match_start_loc,&match_fd_len);

    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"Match field len %d",match_fd_len);

//    msg->desc.ptr2=  msg->desc.ptr1 + match_fd_len;

    ofu_start_pushing_instructions(msg, match_fd_len);
    retval = ofu_start_pushing_apply_action_instruction(msg);
    if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in adding apply action instruction err = %d",retval);
      status = OF_FAILURE;
      break;
    }

    ofu_start_pushing_actions(msg);
    retval = ofu_push_output_action(msg,OFPP_CONTROLLER, OFPCML_NO_BUFFER);
    if (retval != OFU_ACTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in adding output action  err = %d",retval);
      status = OF_FAILURE;
      break;
    }
    ofu_end_pushing_actions(msg,action_start_loc,&action_len);
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"action len = %d", action_len);
    ((struct ofp_instruction_actions*)(msg->desc.ptr2))->len = htons(action_len);
    ofu_end_pushing_instructions(msg,inst_start_loc,&instruction_len);

    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"instruction len = %d", instruction_len);

    retval = of_cntrlr_send_request_message(
        msg,
        datapath_handle,
        conn_info_p,
        NULL,
        NULL);
    if (retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in adding miss entry route table = %d",retval);
      status = OF_FAILURE;
      return status;
    } 
  }
  while(0);

  if (status == OF_FAILURE)	{
            msg->desc.free_cbk(msg);
  }

  return status;
}

int32_t
sm_app_add_missentry_with_output_action(uint64_t datapath_handle, uint8_t table_id,
						uint32_t outport, uint16_t max_len)
{
  struct      of_msg *msg;
  uint16_t    msg_len;
  uint16_t    instruction_len;
  uint8_t     *inst_start_loc = NULL;
  uint16_t    action_len;
  uint8_t     *action_start_loc = NULL;
  uint16_t    match_fd_len=0;
  uint8_t     *match_start_loc = NULL;
  uint16_t    flags;
  int32_t     retval = OF_SUCCESS;
  int32_t     status = OF_SUCCESS;
  void *conn_info_p;

  msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+16+
      OFU_APPLY_ACTION_INSTRUCTION_LEN+OFU_OUTPUT_ACTION_LEN);

  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"msg len %d", msg_len);
  do
  {
    /* Add Miss Entry to the ARP Table...*/
    msg = ofu_alloc_message(OFPT_FLOW_MOD, msg_len);
    if (msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF message alloc error ");
      status = OF_FAILURE;
      return status;
    }

    flags = 0;
    flags |= OFPFF_RESET_COUNTS;
    retval = of_create_add_flow_entry_msg(msg,
        datapath_handle,
        table_id,
        0, /* Low priority*/
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
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in adding miss entry arp table = %d",retval);
      status = OF_FAILURE;
    }
    /*TBD, if we are not pusshing any match fields,this might be not rquired*/ 
    ofu_start_setting_match_field_values(msg);
    ofu_end_setting_match_field_values(msg,match_start_loc,&match_fd_len);

    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"Match field len %d",match_fd_len);

    ofu_start_pushing_instructions(msg, match_fd_len);
    retval = ofu_start_pushing_apply_action_instruction(msg);
    if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in adding apply action instruction err = %d",retval);
      status = OF_FAILURE;
      break;
    }

    retval = ofu_push_output_action(msg, outport, max_len);
    if (retval != OFU_ACTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in adding output action  err = %d",retval);
      status = OF_FAILURE;
      break;
    }
    ofu_end_pushing_actions(msg,action_start_loc,&action_len);
    ((struct ofp_instruction_actions*)(msg->desc.ptr2))->len = htons(action_len);
    ofu_end_pushing_instructions(msg,inst_start_loc,&instruction_len);


    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"instruction len = %d total msg len %d", instruction_len, msg->desc.data_len);

    retval=of_cntrlr_send_request_message(
        msg,
        datapath_handle,
        conn_info_p,
        NULL,
        NULL);
    if (retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in adding miss entry arp table = %d",retval);
      status = OF_FAILURE;
      return status;
    }
  }
  while(0);

  if (status == OF_FAILURE)	
  {
            msg->desc.free_cbk(msg);
  }

  return status;
}

int32_t
sm_app_add_missentry_with_goto_table_instruction(uint64_t datapath_handle, 
						 uint8_t table_id, /* id of the table to which this entry will be added */
						 uint8_t jump_tableid)
{
	struct      of_msg *msg;
	uint16_t    msg_len;
	uint16_t    instruction_len;
	uint8_t     *inst_start_loc = NULL;
	uint16_t    match_fd_len=0;
	uint8_t     *match_start_loc = NULL;
	uint16_t    flags;
	int32_t     retval = OF_SUCCESS;
	int32_t     status = OF_SUCCESS;
	void *conn_info_p;

	msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+16+
				OFU_GOTO_TABLE_INSTRUCTION_LEN);

	OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"msg len %d", msg_len);
	do
	{
	    /* Add Miss Entry to the ARP Table...*/
	    msg = ofu_alloc_message(OFPT_FLOW_MOD, msg_len);
	    if (msg == NULL)	{
	      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF message alloc error ");
	      status = OF_FAILURE;
              return status;
	    }

	    flags = 0;
	    flags |= OFPFF_RESET_COUNTS;
	    retval = of_create_add_flow_entry_msg(msg, datapath_handle,
						table_id, /*Table Id 3*/
						0, /* Low priority*/
						OFPFC_ADD,
						0, /*coockie*/
						0,/*Coockie amsk*/
						OFP_NO_BUFFER,
						flags,
						0, /*No Hard timeout*/
						0, /* No Idle timeout*/
						&conn_info_p
						);

		if (retval != OFU_ADD_FLOW_ENTRY_TO_TABLE_SUCCESS)	{
			OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in adding miss entry arp table = %d",retval);
			status = OF_FAILURE;
			break;
		}
	    	/*TBD, if we are not pusshing any match fields,this might be not rquired*/ 
	    	ofu_start_setting_match_field_values(msg);
	    	ofu_end_setting_match_field_values(msg,match_start_loc,&match_fd_len);

	    	OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"Match field len %d",match_fd_len);

	    	ofu_start_pushing_instructions(msg, match_fd_len);
		if (ofu_push_go_to_table_instruction(msg, jump_tableid) != OFU_INSTRUCTION_PUSH_SUCCESS)
		{
			status = OF_FAILURE;
			break;
		}
	    	ofu_end_pushing_instructions(msg,inst_start_loc,&instruction_len);


		OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"instruction len = %d total msg len %d", instruction_len, msg->desc.data_len);

		retval = of_cntrlr_send_request_message(msg, datapath_handle,
		conn_info_p, NULL, NULL);
		if (retval != OF_SUCCESS)	{
			OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in adding miss entry arp table = %d",retval);
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

int32_t  sm_app_add_arp_entry(uint64_t datapath_handle, uint32_t dp_index, uint16_t priority,
                              uint64_t meta_data, uint64_t meta_data_mask,
                              char *dest_mac, char *src_mac, uint16_t out_port)
{
  struct      of_msg *msg;
  uint16_t    msg_len;
  uint16_t    instruction_len;
  uint8_t     *inst_start_loc = NULL;
  uint16_t    action_len;
  uint8_t     *action_start_loc = NULL;
  uint16_t    match_fd_len=0;
  uint8_t     *match_start_loc = NULL;
  uint16_t    flags;
  int32_t     retval = OF_SUCCESS;
  int32_t     status = OF_SUCCESS;
  void *conn_info_p;

  msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+16+
      OFU_META_DATA_MASK_LEN+
      OFU_APPLY_ACTION_INSTRUCTION_LEN+
      OFU_SET_FIELD_WITH_ETH_DST_MAC_ACTION_LEN+
      OFU_OUTPUT_ACTION_LEN);

  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG," Msg Len %d",msg_len);
  do
  {
    msg = ofu_alloc_message(OFPT_FLOW_MOD, msg_len);
    if (msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF message alloc error ");
      status = OF_FAILURE;
      return status;
    }

    flags = 0;
    flags |= OFPFF_RESET_COUNTS;

    retval = of_create_add_flow_entry_msg(msg,
        datapath_handle,
        dp_conf[dp_index].arp_table_id, /*Table Id 5*/
        priority, /*priority*/
        OFPFC_ADD,
        0, /*coockie*/
        0,/*Coockie amsk*/
        OFP_NO_BUFFER,
        flags,
        0, /*No Hard timeout*/
        60,/*Idle timeout*/
        &conn_info_p
        );

    if (retval != OFU_ADD_FLOW_ENTRY_TO_TABLE_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in adding arp flow entry  = %d",retval);
      status = OF_FAILURE;
    }
    ofu_start_setting_match_field_values(msg);
#if 0 /* Not required */
    uint16_t eth_type = 0x800; /*Eth type is IP*/
    retval = of_set_ether_type(msg,&eth_type);
    if(retval != OFU_SET_FIELD_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"OF Set IPv4 src field failed,err = %d",retval);
      status = OF_FAILURE;
      break;
    }
#endif

    retval = of_set_meta_data(msg, &meta_data, TRUE, &meta_data_mask);
    if (retval != OFU_SET_FIELD_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"OF Set meta data failed,err = %d",retval);
      status = OF_FAILURE;
      break;
    }
    ofu_end_setting_match_field_values(msg,match_start_loc,&match_fd_len);

    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"Match field len %d",match_fd_len); 
//    msg->desc.ptr2=  msg->desc.ptr1 + match_fd_len;
    ofu_start_pushing_instructions(msg, match_fd_len);
    retval = ofu_start_pushing_apply_action_instruction(msg);
    if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in adding apply action instruction err = %d",retval);
      status = OF_FAILURE;
      break;
    }
    ofu_start_pushing_actions(msg);
    retval = ofu_push_set_source_mac_in_set_field_action(msg, src_mac);
    if (retval != OFU_ACTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in set mac hdr action  err = %d",retval);
      status = OF_FAILURE;
      break;
    }

    retval = ofu_push_set_destination_mac_in_set_field_action(msg, dest_mac);
    if (retval != OFU_ACTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in set mac hdr action  err = %d",retval);
      status = OF_FAILURE;
      break;
    }

    retval = ofu_push_output_action(msg, out_port, OFPCML_NO_BUFFER);
    if (retval != OFU_ACTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in adding output action  err = %d",retval);
      status = OF_FAILURE;
      break;
    }
    ofu_end_pushing_actions(msg,action_start_loc,&action_len);
    ofu_end_pushing_instructions(msg,inst_start_loc,&instruction_len);
    ((struct ofp_instruction_actions*)(msg->desc.ptr2))->len = htons(instruction_len);
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"instruction len = %d", instruction_len);


    retval=of_cntrlr_send_request_message(
        msg,
        datapath_handle,
        conn_info_p,
        NULL,
        NULL);
    if (retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in adding arp flow entry  = %d",retval);
      status = OF_FAILURE;
      return status;
    }
  }
  while(0);

  if (status == OF_FAILURE)	{
            msg->desc.free_cbk(msg);
  }

  return status;
}

static int32_t sm_app_read_of_sw_interfaces(uint32_t dp_index)
{ 
  int32_t n = 0, ii = 0, len;
  char line[150];
  char if_ipaddr[18] = {};
  char bcast_addr[18] = {};
  char net_mask[18] = {};
  char iface[OF_CNTLR_APP_IFACE_NAME_LEN] = {};
  char *token = NULL;
  FILE *fr;    /* declare the file pointer */


  fr = fopen(dp_conf[dp_index].sw_iface_info_file, "rt");/* open the file for reading */
  if (fr == NULL)
  {
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"Failed to Open file in read mode errno=%d",errno);
    perror(NULL);
    return OF_SUCCESS;
  }

  while (fgets(line, 150, fr) != NULL)
  {
    if ((line[0] == '#') || ((line[0] == '/') && (line[1] == '*')))
    {
      continue;
    }

    token = (char *)strtok(line,"-");
    do
    {
      if (!strncmp(token,"iface",strlen("iface")))
      {
        strncpy(iface, (token+strlen("iface ")), OF_CNTLR_APP_IFACE_NAME_LEN);
        ii = 0;
        while (iface[ii] != '\0')
        {
          if ((iface[ii] == ' ') || (iface[ii] == '-') || (iface[ii] == '\n'))
          {
            iface[ii] = '\0';
            break;
          }
          ii++;
        }
        strcpy(dp_conf[dp_index].iface_info[n].port_name, iface);
      }
      else if (!strncmp(token,"gb",strlen("gb")) || !strncmp(token,"10g",strlen("10g")))
      {
        strcat(iface, "-"); 
        len = strlen(iface);
        strcpy(iface+len, token); 
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, "token = %s iface=%s",token, iface);
        ii=len;
        while (iface[ ii] != '\0')
        {
          if ((iface[ii] == ' ') || 
              (!isalpha(iface[ii]) && 
               !isdigit(iface[ii]) &&
               (iface[ii] != '-')
              ))
          {
            iface[ii] = '\0';
            break;
          }
          ii++;
        }
        strcpy(dp_conf[dp_index].iface_info[n].port_name, iface);
      }
      else if (!strncmp(token,"ip",strlen("ip")))
      {
        strcpy(if_ipaddr, (token+strlen("ip ")));
        dp_conf[dp_index].iface_info[n].ip_address = inet_network(if_ipaddr);
      }
      else if (!strncmp(token,"mask",strlen("mask")))
      {
        strcpy(net_mask, (token+strlen("mask ")));
        dp_conf[dp_index].iface_info[n].net_mask = inet_network(net_mask);
      }
      else if (!strncmp(token,"bcast",strlen("bcast")))
      {
        strcpy(bcast_addr, (token+strlen("bcast ")));
        dp_conf[dp_index].iface_info[n].bcast_addr = inet_network(bcast_addr);
        n++;
      }
    }while((token = (char *)strtok(NULL,"-")) != NULL);
  }
  dp_conf[dp_index].no_of_iface_entries = n;
  sm_app_print_iface_table(dp_index);

  fclose(fr);
  return OF_SUCCESS; 
}

static int32_t sm_app_read_fw_acl_rules(uint32_t dp_index)
{
  int32_t n = 0, ii = 0, len;
  char line[128];
  char src_ipaddr[18] = {};
  char dst_ipaddr[18] = {};
  char snat_ipaddr[18] = {};
  char dnat_ipaddr[18] = {};
  char src_port[8] = {};
  char dst_port[8] = {};
  char protocol[6] = {};
  char action[8] = {};
  char in_iface[20] = {};
  char *token = NULL;
  FILE *fr;        /* declare the file pointer */


  fr = fopen(dp_conf[dp_index].fw_rule_conf_file, "rt");/* open the file for reading */
  if (fr == NULL)
  {
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG," Failed to Open file:%s in read mode errno=%d",
        dp_conf[dp_index].fw_rule_conf_file,errno);
    perror(NULL);
    return OF_SUCCESS;
  }

  while(fgets(line, 128, fr) != NULL)
  {
    if ((line[0] == '#') || ((line[0] == '/') && (line[1] == '*')))
    {
      continue;
    }

    token = (char *)strtok(line,"-");
    do
    {
      if (!strncmp(token,"sip",strlen("sip")))
      {
        strcpy(src_ipaddr, (token+strlen("sip ")));
        dp_conf[dp_index].fw_rule_list[n].src_ip = inet_network(src_ipaddr);
      }
      else if (!strncmp(token,"dip",strlen("dip")))
      {
        strcpy(dst_ipaddr, (token+strlen("dip ")));
        dp_conf[dp_index].fw_rule_list[n].dst_ip = inet_network(dst_ipaddr);
      }
      else if (!strncmp(token,"sp",strlen("sp")))
      {
        strcpy(src_port, (token+strlen("sp ")));
        dp_conf[dp_index].fw_rule_list[n].src_port = (uint16_t)atoi(src_port);
      }
      else if (!strncmp(token,"dp",strlen("dp")))
      {
        strcpy(dst_port, (token+strlen("dp ")));
        dp_conf[dp_index].fw_rule_list[n].dst_port = (uint16_t)atoi(dst_port);
      }
      else if (!strncmp(token,"proto",strlen("proto")))
      {   
        strncpy(protocol, (token+strlen("proto ")), 6);
        ii = 0;
        while(protocol[ii] != '\0')
        {
          if (!isalpha(protocol[ii]))
          {
            protocol[ii] = '\0';
            break;
          }
          ii++;
        }

        if (!strncasecmp(protocol, "TCP", strlen("TCP")))
        {
          dp_conf[dp_index].fw_rule_list[n].protocol = OF_CNTLR_IP_PROTO_TCP;
        }
        else if (!strncasecmp(protocol, "UDP", strlen("UDP")))
        {
          dp_conf[dp_index].fw_rule_list[n].protocol = OF_CNTLR_IP_PROTO_UDP;
        }
        else if (!strncasecmp(protocol, "ICMP", strlen("ICMP")))
        {
          dp_conf[dp_index].fw_rule_list[n].protocol = OF_CNTLR_IP_PROTO_ICMP;
        }
        else if (!strncasecmp(protocol, "IGMP", strlen("IGMP")))
        {
          dp_conf[dp_index].fw_rule_list[n].protocol = OF_CNTLR_IP_PROTO_IGMP;
        }
        else if (!strncasecmp(protocol, "IPIP", strlen("IPIP")))
        {
          dp_conf[dp_index].fw_rule_list[n].protocol = OF_CNTLR_IP_PROTO_IPIP;
        }
        else if (!strncasecmp(protocol, "ESP", strlen("ESP")))
        {
          dp_conf[dp_index].fw_rule_list[n].protocol = OF_CNTLR_IP_PROTO_ESP;
        }
        else if (!strncasecmp(protocol, "GRE", strlen("GRE")))
        {
          dp_conf[dp_index].fw_rule_list[n].protocol = OF_CNTLR_IP_PROTO_GRE;
        }
        else if (!strncasecmp(protocol, "SCTP", strlen("SCTP")))
        {
          dp_conf[dp_index].fw_rule_list[n].protocol = OF_CNTLR_IP_PROTO_SCTP;
        }
        else
        {
          OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR, "Invalid Protocol Congured in FW rule...");
          return OF_FAILURE;
        }
      }
      else if (!strncmp(token,"iniface",strlen("iniface")))
      {
        strncpy(in_iface,(token+strlen("iniface ")), OF_CNTLR_APP_IFACE_NAME_LEN);
        ii = 0;
        while(in_iface[ii] != '\0')
        {
          if ((in_iface[ii] == ' ') || (in_iface[ii] == '-'))
          {
            in_iface[ii] = '\0';
            break;
          }
          ii++;
        }
        strcpy(dp_conf[dp_index].fw_rule_list[n].in_iface, in_iface);
      }
      else if (!strncmp(token,"gb",strlen("gb")) || !strncmp(token,"10g",strlen("10g")))
      {
        strcat(in_iface, "-"); 
        len = strlen(in_iface);
        strcpy(in_iface+len, token); 
        OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, "token = %s iface=%s",token, in_iface);
        ii=len;
        while (in_iface[ii] != '\0')
        {
          if ((in_iface[ii] == ' ') || (in_iface[ii] == '-'))
          {
            in_iface[ii] = '\0';
            break;
          }
          ii++;
        }
        strcpy(dp_conf[dp_index].fw_rule_list[n].in_iface, in_iface);
      }
      else if (!strncmp(token,"snat",strlen("snat")))
      {
        strcpy(snat_ipaddr, (token+strlen("snat ")));
        dp_conf[dp_index].fw_rule_list[n].snat_ip = inet_network(snat_ipaddr);
      }
      else if (!strncmp(token,"dnat",strlen("dnat")))
      {
        strcpy(dnat_ipaddr, (token+strlen("dnat ")));
        dp_conf[dp_index].fw_rule_list[n].dnat_ip = inet_network(dnat_ipaddr);
      }
      else if (!strncmp(token,"action",strlen("action")))
      {
        strncpy(action, (token+strlen("action ")), 8);
        ii = 0;
        while(action[ii] != '\0')
        {
          if (!isalpha(action[ii]))
          {
            action[ii] = '\0';
            break;
          }
          ii++;
        }

        if (!strncmp(action,"allow",strlen("allow")))
        {
          dp_conf[dp_index].fw_rule_list[n].action = OF_CNTLR_FW_RULE_ALLOW;
        }
        else if (!strncmp(action,"deny",strlen("deny")))
        {
          dp_conf[dp_index].fw_rule_list[n].action = OF_CNTLR_FW_RULE_DENY;
        }
        else
        {
          OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_WARN, "Invalid FW rule..action=%s....",action);
        }
      }
      if (token[strlen(token)-1] == '\n')
        n++;
    }while((token = (char *)strtok(NULL,"-")) != NULL);
  }/* end of while loop */

  dp_conf[dp_index].no_of_fw_rules = n;
  sm_app_print_fw_acl_rule_table(dp_index);


  fclose(fr);
  return OF_SUCCESS;
}
  
void sm_app_print_iface_table(uint32_t dp_index)
{
  int32_t ii;
  //uint32_t cbyte;

  printf("\nDP%llx_Interface Table:",dp_conf[dp_index].dp_id);
  printf("\r\nTotal No Of Entries:%d",dp_conf[dp_index].no_of_iface_entries);
  puts("\n _____________________________________________________________");
  puts("\n PORT-NAME  IP-ADDR   NET-MASK     BCAST-ADDR   ");
  puts("\n -------------------------------------------------------------");
  for (ii = 0; ii < dp_conf[dp_index].no_of_iface_entries; ii++)	{
    printf("\n%s   %x    %x    %x   ",dp_conf[dp_index].iface_info[ii].port_name,
        dp_conf[dp_index].iface_info[ii].ip_address,
        dp_conf[dp_index].iface_info[ii].net_mask,
        dp_conf[dp_index].iface_info[ii].bcast_addr);
  }
  puts("\n -------------------------------------------------------------");
}


void sm_app_print_fw_acl_rule_table(uint32_t dp_index)
{
  uint32_t ii;

  printf("\nDP%llx_FW ACL Rules:",dp_conf[dp_index].dp_id);
  printf("\r\nTotal No Of Rules:%d",dp_conf[dp_index].no_of_fw_rules);
  puts("\n _____________________________________________________________________________________");
  puts("\n SRC-IP   DST-IP   SRC-PORT   DST-PORT   PROTOCOL  SNAT-IP/DNAT-IP IN-IFACE   ACTION  ");
  puts("\n -------------------------------------------------------------------------------------");
  for (ii = 0; ii < dp_conf[dp_index].no_of_fw_rules; ii++)
  {
    if (dp_conf[dp_index].fw_rule_list[ii].src_ip)
      printf("\n %x",dp_conf[dp_index].fw_rule_list[ii].src_ip);
    else
      printf("\n any");

    if (dp_conf[dp_index].fw_rule_list[ii].dst_ip)
      printf("    %x",dp_conf[dp_index].fw_rule_list[ii].dst_ip);
    else
      printf("    any");

    if (dp_conf[dp_index].fw_rule_list[ii].src_port)
      printf("     %d",dp_conf[dp_index].fw_rule_list[ii].src_port);
    else
      printf("     any");

    if (dp_conf[dp_index].fw_rule_list[ii].dst_port)
      printf("     %d",dp_conf[dp_index].fw_rule_list[ii].dst_port);
    else
      printf("     any");

    switch (dp_conf[dp_index].fw_rule_list[ii].protocol)
    {
      case OF_CNTLR_IP_PROTO_TCP:
        printf("     tcp");
        break;
      case OF_CNTLR_IP_PROTO_UDP:
        printf("     udp");
        break;
      case OF_CNTLR_IP_PROTO_ICMP:
        printf("     icmp");
        break;
      case OF_CNTLR_IP_PROTO_IGMP:
        printf("     igmp");
        break;
      case 0:
        printf("     any");
        break;
      default:
        printf("     other");
        break;
    }

    if (dp_conf[dp_index].fw_rule_list[ii].snat_ip)
        printf("\n snat %x",dp_conf[dp_index].fw_rule_list[ii].snat_ip);
    else if (dp_conf[dp_index].fw_rule_list[ii].dnat_ip)
        printf("\n dnat %x",dp_conf[dp_index].fw_rule_list[ii].dnat_ip);

    if (strlen(dp_conf[dp_index].fw_rule_list[ii].in_iface))
      printf("     %s",dp_conf[dp_index].fw_rule_list[ii].in_iface);
    else
      printf("   any");

    if (dp_conf[dp_index].fw_rule_list[ii].action == OF_CNTLR_FW_RULE_ALLOW)
      puts("   allow");
    else
      puts("   deny");
  }
  puts("\n ------------------------------------------------------------------------------");
}

int32_t sm_app_get_ip_by_iface_name(uint32_t dp_index, char *out_iface, uint32_t *ip)
{
  uint32_t ii;

  for (ii = 0; ii < dp_conf[dp_index].no_of_iface_entries; ii++)	
  {
    if (!strcmp(dp_conf[dp_index].iface_info[ii].port_name, out_iface))	
    {
      *ip = dp_conf[dp_index].iface_info[ii].ip_address;
      return OF_SUCCESS;
    }
  }
  return OF_FAILURE;
}

int32_t sm_app_rt_get_out_iface_by_dstip(uint32_t dp_index, uint32_t dest_ip, char *out_iface)
{
  uint32_t ii, masked_dest_ip;

	for (ii = 0; ii < dp_conf[dp_index].no_of_route_entries; ii++)	{
    masked_dest_ip = (dest_ip & dp_conf[dp_index].rt_table[ii].rt_mask);
		if (dp_conf[dp_index].rt_table[ii].rt_dest == masked_dest_ip)	{
			strcpy(out_iface, dp_conf[dp_index].rt_table[ii].iface_name);
			return OF_SUCCESS;
    }
  }
  return OF_FAILURE;
}

int32_t sm_app_rt_get_out_iface(uint32_t dp_index, uint32_t src_ip, uint32_t dest_ip, char *out_iface)
{
  uint32_t ii, masked_dest_ip;


  	for (ii = 0; ii < dp_conf[dp_index].no_of_route_entries; ii++)	{
    masked_dest_ip = (dest_ip & dp_conf[dp_index].rt_table[ii].rt_mask);
    if ((dp_conf[dp_index].rt_table[ii].rt_dest == masked_dest_ip) &&
		(dp_conf[dp_index].rt_table[ii].rt_src == src_ip))	{
      strcpy(out_iface, dp_conf[dp_index].rt_table[ii].iface_name);
      return OF_SUCCESS;
    }
  }
  return OF_FAILURE;
}


int32_t sm_app_attach_dpid_to_conf_files(uint32_t dpinfo_index,
                                         uint64_t dpid,
                                         uint64_t datapath_handle)
{
  int32_t len, totlen = 0;
  char  *pdp_id = NULL;
  int32_t fd;
  char buff[200] = {};

  dp_conf[dpinfo_index].dp_id = dpid;
  dp_conf[dpinfo_index].dp_handle = datapath_handle;
  dp_conf[dpinfo_index].dp_index = dpinfo_index;
  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"DPRM_DOMAIN_DATAPATH_ATTACHED datapath handle=%llx dpid=%llx noOfDps=%d",
      datapath_handle,dpid,dpinfo_index);
  len = strlen(OF_CNTLR_DP_SW_CONF_PATH);
  strncpy(dp_conf[dpinfo_index].rt_arp_conf_file, OF_CNTLR_DP_SW_CONF_PATH, len);
  totlen += len;

  len = strlen("rt_and_arp_dp_");
  strncat(dp_conf[dpinfo_index].rt_arp_conf_file, "rt_and_arp_dp_", len);
  totlen += len;

  pdp_id  =  int64toa(dpid);
  len = strlen(pdp_id);
  strncat(dp_conf[dpinfo_index].rt_arp_conf_file, pdp_id, len);
  totlen += len;
  free(pdp_id);
  pdp_id = NULL;

  len = strlen(".conf");
  strncat(dp_conf[dpinfo_index].rt_arp_conf_file,".conf", len);
  totlen += len;
  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"TOTAL FILE LEN=%d ",totlen);
  dp_conf[dpinfo_index].rt_arp_conf_file[totlen] = '\0';

  fd = open(dp_conf[dpinfo_index].rt_arp_conf_file, O_CREAT|O_RDWR|O_EXCL, S_IRWXU);
  if (fd == -1)
  {
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_WARN, "File:%s creation failed with RW permissions",
        dp_conf[dpinfo_index].rt_arp_conf_file);
    //return OF_FAILURE;
  }
  else
    close(fd);

  totlen = 0;
  len = strlen(OF_CNTLR_DP_SW_CONF_PATH);
  strncpy(dp_conf[dpinfo_index].fw_rule_conf_file,
      OF_CNTLR_DP_SW_CONF_PATH, strlen(OF_CNTLR_DP_SW_CONF_PATH));
  totlen += len;

  len = strlen("fw_rules_dp_");
  strncat(dp_conf[dpinfo_index].fw_rule_conf_file,
      "fw_rules_dp_",strlen("fw_rules_dp_"));
  totlen += len;

  pdp_id  =  int64toa(dpid);
  len = strlen(pdp_id);
  strncat(dp_conf[dpinfo_index].fw_rule_conf_file, pdp_id, len);
  totlen += len;
  free(pdp_id);
  pdp_id = NULL;

  //strncat(dp_conf[dpinfo_index].fw_rule_conf_file, itoa(dpid), strlen(itoa(dpid)));
  len = strlen(".conf");
  strncat(dp_conf[dpinfo_index].fw_rule_conf_file,".conf",len);
  totlen += len;
  dp_conf[dpinfo_index].fw_rule_conf_file[totlen] = '\0';

  fd = open(dp_conf[dpinfo_index].fw_rule_conf_file, O_CREAT|O_RDWR|O_EXCL, S_IRWXU);
  if (fd == -1)
  {
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_WARN, "File:%s creation failed with RW permissions",
        dp_conf[dpinfo_index].fw_rule_conf_file);
    //return OF_FAILURE;
  }
  else
    close(fd);

  totlen = 0;
  len = strlen(OF_CNTLR_DP_SW_CONF_PATH);
  strncpy(dp_conf[dpinfo_index].sw_iface_info_file,
      OF_CNTLR_DP_SW_CONF_PATH, len);
  totlen += len;

  len = strlen("sw_iface_info_dp_");
  strncat(dp_conf[dpinfo_index].sw_iface_info_file,
      "sw_iface_info_dp_",len);
  totlen += len;

  pdp_id  =  int64toa(dpid);
  len = strlen(pdp_id);
  strncat(dp_conf[dpinfo_index].sw_iface_info_file, pdp_id, len);
  totlen += len;
  free(pdp_id);
  pdp_id = NULL;

  //strncat(dp_conf[dpinfo_index].sw_iface_info_file, itoa(dpid),strlen(itoa(dpid)));
  len = strlen(".conf");
  strncat(dp_conf[dpinfo_index].sw_iface_info_file,".conf",len);
  totlen += len;
  dp_conf[dpinfo_index].sw_iface_info_file[totlen] = '\0';

  fd = open(dp_conf[dpinfo_index].sw_iface_info_file, O_CREAT|O_RDWR|O_EXCL, S_IRWXU);
  if (fd == -1)
  {
    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_WARN, "File:%s creation failed with RW permissions",
        dp_conf[dpinfo_index].sw_iface_info_file);
    return OF_FAILURE;
  }
  else
    close(fd);

  /* Open files with write permission and write configuration help */
  fd = open(dp_conf[dpinfo_index].rt_arp_conf_file, O_WRONLY);
  if (fd == -1)
  {
    perror("File cannot be opened in write mode");
    return OF_FAILURE;
  }
  strcpy(buff, "#Configure static routes and arp entries as given in example, Please don't give extra spaces.\n");
  write(fd, buff, strlen(buff));
  memset(buff, 0, 150);
  strcpy(buff,"#Ex:ip route add -srcip 1.1.1.1 -destip 2.2.2.2 -mask 255.255.255.0 -gw 3.3.3.3 -iface eth0 -pmtu 1500\n");
  write(fd, buff, strlen(buff));
  memset(buff, 0, 150);
  strcpy(buff, "#Ex:arp add -ip 1.1.1.1 -mac 00:07:e9:ac:21:33\n");
  write(fd, buff, strlen(buff));
  close(fd);
  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, "created file:%s",dp_conf[dpinfo_index].rt_arp_conf_file);

  fd = open(dp_conf[dpinfo_index].fw_rule_conf_file, O_WRONLY);
  if (fd == -1)
  {
    perror("File cannot be opened in write mode");
    return OF_FAILURE;
  }
  memset(buff, 0, 150);
  strcpy(buff, "#Configure Firewall Policy rules as given in example, Please don't give extra spaces\n");
  write(fd, buff, strlen(buff));
  memset(buff, 0, 150);
  strcpy(buff,"#Ex:ip rule add -sip 2.2.2.2 -dip 1.1.1.1 -sp 1234 -dp 5678 -proto TCP -snat/-dnat <snat-ip/dnat-ip> -iniface eth0 -action allow\n");
  write(fd, buff, strlen(buff));
  close(fd);
  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, "created file:%s",dp_conf[dpinfo_index].fw_rule_conf_file);

  fd = open(dp_conf[dpinfo_index].sw_iface_info_file, O_WRONLY);
  if (fd == -1)
  {
    perror("File cannot be opened in write mode");
    return OF_FAILURE;
  }

  memset(buff, 0, 150);
  strcpy(buff, "#Configure datapath ip information as given example, Please don't give extra spaces.\n");
  write(fd, buff, strlen(buff));
  memset(buff, 0, 150);
  strcpy(buff,"#Ex:ip address -iface eth0 -ip 2.2.2.2 -mask 255.255.255.0 -bcast 2.2.2.255\n");
  write(fd, buff, strlen(buff));
  close(fd);
  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG, "created file:%s",dp_conf[dpinfo_index].sw_iface_info_file);
  return OF_SUCCESS;
}


void
cntlr_send_barrier_request(uint64_t datapath_handle)
{
    int32_t         retval= CNTLR_SUCCESS;
    int32_t         status= OF_SUCCESS;

    return;  /*Commenting temporarily*/

    OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_DEBUG,"entered ");
    do
    {
        retval = of_send_barrier_request_msg(datapath_handle,
                                         sm_barrier_reply_notification_fn,
                                         NULL,NULL);
        if(retval != CNTLR_SUCCESS)
        {
           OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"Send Get barrier request message fail, error=%d",retval);
           status = OF_FAILURE;
           break;
        }
    }
    while(0);

}


void sm_barrier_reply_notification_fn(uint64_t  controller_handle,
                                                 uint64_t  domain_handle,
                                                 uint64_t  datapath_handle,
                                                 struct of_msg *msg,
                                                 void     *cbk_arg1,
                                                 void     *cbk_arg2,
                                                 uint8_t   status)
{
  OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_INFO," my barrier notification func ");
  /* Need to handle barrier message */
  return; 
}

int32_t sm_app_frame_and_send_pktout_to_dp(uint64_t datapath_handle, struct ofl_packet_in *pkt_in)
{
  uint8_t      *action_start_loc=NULL;
  uint16_t      action_len;
  uint16_t      msg_len;
  struct of_msg *msg;
  uint32_t      in_port;
  uint8_t     *pkt_data = pkt_in->packet;
  int32_t       status = OF_SUCCESS;
  int32_t       retval;
  void *conn_info_p;
  uint64_t meta_data;
  int32_t flags=0;
  struct pkt_mbuf *mbuf = NULL;


  do
  {
    /*Kumar:TBD Barrier message should be sent before pkt out message */
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"incoming packet length =%d ",pkt_in->packet_length);
#ifdef FSLX_COMMON_AND_L3_EXT_SUPPORT
    msg_len = (OFU_PACKET_OUT_MESSAGE_LEN+16+
                FSLX_ACTION_START_TABLE_LEN+FSLX_ACTION_SET_METADATA_LEN+
                pkt_in->packet_length);
#else
    msg_len = (OFU_PACKET_OUT_MESSAGE_LEN+16+OFU_OUTPUT_ACTION_LEN+pkt_in->packet_length);
#endif
    of_alloc_pkt_mbuf_and_set_of_msg(mbuf, msg, OFPT_PACKET_OUT, msg_len, flags, status);
    if (status == FALSE)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d OF message alloc error \r\n",__FUNCTION__,__LINE__);
      status = OF_FAILURE;
      return status;
    }

    retval = ofu_get_in_port_field(msg, pkt_in->match_fields,
        pkt_in->match_fields_length, &in_port);
    if (retval != OFU_IN_PORT_FIELD_PRESENT)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:failed get inport ....inport=%d",__FUNCTION__,in_port);
      status = OF_FAILURE;
      break;
    }

    retval = ofu_get_metadata_field(msg, pkt_in->match_fields,
        pkt_in->match_fields_length, &meta_data);
    if (retval != OFU_IN_META_DATA_PRESENT)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"PKT_OUT:failed get metadata ...");
      status = OF_FAILURE;
      break;
    }
 

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"....inport=%d metadata=%llx",in_port,meta_data);
    retval=of_frame_pktout_msg(
        msg,
        datapath_handle,
        TRUE,
        OFP_NO_BUFFER,
        in_port,
        0,
        &conn_info_p
        );
    if (retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in framing pkt out");
      status = OF_FAILURE;
      break;
    }

    ofu_start_pushing_actions(msg);
#ifdef FSLX_COMMON_AND_L3_EXT_SUPPORT
    retval = fslx_action_set_metadata(msg, meta_data);
    if (retval != OFU_ACTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"PKT_OUT: Error in  set metadata  err = %d\r\n",
          retval);
      status = OF_FAILURE;
      break;
    }

    retval = fslx_action_start_from_table(msg,pkt_in->table_id);
    if (retval != OFU_ACTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"PKT_OUT Error in  start from the table  err = %d\r\n",
          retval);
      status = OF_FAILURE;
      break;
    }
#else
    retval = ofu_push_output_action(msg,OFPP_TABLE,OFPCML_NO_BUFFER);
    if (retval != OFU_ACTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR,"%s:%d  Error in adding output action  err = %d\r\n",
          __FUNCTION__,__LINE__,retval);
      status = OF_FAILURE;
      break;
    }
#endif
    ofu_end_pushing_actions(msg,action_start_loc,&action_len);
    ((struct ofp_packet_out*)(msg->desc.start_of_data))->actions_len=htons(action_len);

    retval = of_add_data_to_pkt_and_send_to_dp(msg,
        datapath_handle,
        conn_info_p,
        pkt_in->packet_length,
        pkt_data,
        NULL,
        NULL);
    if (retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_SAMPLE_APP, OF_LOG_ERROR," Error in sending packet err = %d",retval);
      status = OF_FAILURE;
      return status;
    }
  }
  while(0);

  if ((status == OF_FAILURE) && (msg != NULL))
  {
    of_get_pkt_mbuf_by_of_msg_and_free(msg);
//	of_pkt_mbuf_free(mbuf);
  }
  return status;
}

int32_t sm_app_add_tap_proactive_entry(uint64_t dp_handle, uint32_t inport, uint32_t outport)
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
  void        *conn_info_p;
  static uint32_t priority = 10;

  msg_len = (OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN+16+
      OFU_IN_PORT_FIELD_LEN+
      OFU_APPLY_ACTION_INSTRUCTION_LEN+
      OFU_OUTPUT_ACTION_LEN);

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Msg Len %d\r\n",msg_len);
  do
  {
    msg = ofu_alloc_message(OFPT_FLOW_MOD, msg_len);
    if (msg == NULL)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d OF message alloc error \r\n",__FUNCTION__,__LINE__);
      status = OF_FAILURE;
      return status;
    }

    flags |= OFPFF_RESET_COUNTS;
    retval = of_create_add_flow_entry_msg(msg,
	dp_handle,
	0, /*Table Id 0*/
	priority++, /*priority*/
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
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Error in adding arp flow entry  = %d",
          retval);
      status = OF_FAILURE;
    }

    ofu_start_setting_match_field_values(msg);
    retval = of_set_in_port(msg, &inport);
    if (retval != OFU_SET_FIELD_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d OF Set inport failed");
      status = OF_FAILURE;
      break;
    }
    ofu_end_setting_match_field_values(msg,match_start_loc,&match_fd_len);
    
    ofu_start_pushing_instructions(msg, match_fd_len);
    retval = ofu_start_pushing_apply_action_instruction(msg);
    if (retval != OFU_INSTRUCTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d  Error in adding apply action instruction err = %d\r\n",
	  __FUNCTION__,__LINE__,retval);
      status = OF_FAILURE;
      break;
    }

    //ofu_start_pushing_actions(msg); 
    retval = ofu_push_output_action(msg, outport, OFPCML_NO_BUFFER);
    if (retval != OFU_ACTION_PUSH_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d  Error in adding output action  err = %d\r\n",
	  __FUNCTION__,__LINE__,retval);
      status = OF_FAILURE;
      break;
    }
    ofu_end_pushing_actions(msg,action_start_loc,&action_len);
    ((struct ofp_instruction_actions*)(msg->desc.ptr2))->len = htons(action_len);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"total actions len = %d for this instruction", action_len);
    ofu_end_pushing_instructions(msg,inst_start_loc,&instruction_len);

    retval=of_cntrlr_send_request_message(
        msg,
        dp_handle,
        conn_info_p,
        NULL,
        NULL);
    if (retval != OF_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d  Error in adding arp flow entry  = %d\r\n",
          __FUNCTION__,__LINE__,retval);
      status = OF_FAILURE;
    }
  }
  while(0);

  return status;
}

int32_t sm_app_tap_dp_ready_handler(uint64_t datapath_handle)
{ 
   sm_app_add_tap_proactive_entry(datapath_handle, 1, 0x000000f0);
   sm_app_add_tap_proactive_entry(datapath_handle, 2, 0x000000f1);
   sm_app_add_tap_proactive_entry(datapath_handle, 3, 0x000000f2);
   sm_app_add_tap_proactive_entry(datapath_handle, 4, 0x000000f3);
   sm_app_add_tap_proactive_entry(datapath_handle, 5, 0x000000f4);
   sm_app_add_tap_proactive_entry(datapath_handle, 0x000000f0, 1);
   sm_app_add_tap_proactive_entry(datapath_handle, 0x000000f1, 2);
   sm_app_add_tap_proactive_entry(datapath_handle, 0x000000f2, 3);
   sm_app_add_tap_proactive_entry(datapath_handle, 0x000000f3, 4);
   sm_app_add_tap_proactive_entry(datapath_handle, 0x000000f4, 5);
}



