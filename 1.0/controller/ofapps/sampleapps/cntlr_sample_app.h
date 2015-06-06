#ifndef _CNTLR_SM_APP_H
#define _CNTLR_SM_APP_H


enum	{
	SM_APP_ETHTYPE_TABLE_ID = 0,
	SM_APP_MCAST_CLASSIFIER_TABLE_ID = 1,
	SM_APP_MCAST_ROUTING_TABLE_ID = 2,
	SM_APP_SESSION_TABLE_ID = 3,
	SM_APP_L3ROUTING_TABLE_ID = 4,
	SM_APP_ARP_TABLE_ID = 5,
	SM_APP_MAX_TABLE_ID = 255
};

/* Macros */
#define SM_APP_PKT_IN_PRIORITY     2

#define OF_CNTLR_APP_IFACE_NAME_LEN   12
#define OF_CNTLR_APP_MAC_ADDR_LEN     6
#define OF_CNTLR_APP_MAX_ROUTE_ENTRIES 20
#define OF_CNTLR_APP_MAX_ARP_ENTRIES   20
#define OF_CNTLR_APP_MAX_FW_ACL_RULES  20
#define OF_CNTLR_APP_MAX_MCAST_OUTPORTS 10
#define OF_CNTLR_APP_MAX_MCAST_ROUTE_ENTRIES 20




#define OF_CNTLR_IPV4_ADDRESS_LEN   4

#define OF_CNTLR_IP_PROTO_UDP      0x11
#define OF_CNTLR_IP_PROTO_TCP      0x06
#define OF_CNTLR_IP_PROTO_ICMP     0x01
#define OF_CNTLR_IP_PROTO_IGMP     0x02
#define OF_CNTLR_IP_PROTO_ESP      0x32
#define OF_CNTLR_IP_PROTO_AH       0x33
#define OF_CNTLR_IP_PROTO_GRE      0x2F
#define OF_CNTLR_IP_PROTO_SCTP     0x84
#define OF_CNTLR_IP_PROTO_IPIP     0x5E


#define OF_CNTLR_MAC_HDR_LEN        14
#define OF_CNTLR_HW_ADDR_LEN        6
#define OF_CNTLR_IPV4_HDR_OFFSET    OF_CNTLR_MAC_HDR_LEN
#define OF_CNTLR_IPV4_TTL_OFFSET         8


#define OF_CNTLR_IPV4_PROTO_OFFSET          9
#define OF_CNTLR_IPV4_FRAG_OFFSET          6
#define OF_CNTLR_IPV4_RES_FRAG 0x80
#define OF_CNTLR_IPV4_DONT_FRAG 0x40
#define OF_CNTLR_IPV4_MORE_FRAG 0x20


#define OF_CNTLR_IPV4_CHKSUM_OFFSET        10 

#define OF_CNTLR_IPV4_SRC_IP_OFFSET        12 

#define OF_CNTLR_IPV4_DST_IP_OFFSET        16

#define OF_CNTLR_IPV4_HDR_LEN(x)         (((*(x + OF_CNTLR_IPV4_HDR_OFFSET)) & 0x0F) << 2)



/* macros for icmp */
#define OF_CNTLR_ICMPV4_ID_OFFSET            4
#define OF_CNTLR_ICMPV4_CODE_OFFSET          1
/* icmp types */
#define OF_CNTLR_ICMPV4_ECHO_REQUEST         8
#define OF_CNTLR_ICMPV4_ECHO_REPLY           0
#define OF_CNTLR_ICMPV4_TIMESTAMP            13
#define OF_CNTLR_ICMPV4_TIME_REPLY           14
#define OF_CNTLR_ICMPV4_DEST_UNREACH         3
#define OF_CNTLR_ICMPV4_QUENCH               4
#define OF_CNTLR_ICMPV4_TIME_EXCEEDED        11
/* icmp error codes */
#define OF_CNTLR_ICMPV4_DEST_NW_UNREACHABLE  0
#define OF_CNTLR_ICMPV4_TTL_EXPIRED          0

/* igmp macros */
/* offsets from the start of IGMP header */
#define OF_CNTLR_IGMP_HEADER_OFFSET(x)          (OF_CNTLR_IPV4_HDR_OFFSET + OF_CNTLR_IPV4_HDR_LEN(x))
#define OF_CNTLR_IGMPV3_GROUP_ADDR_OFFSET       12
#define OF_CNTLR_IGMPV2_GROUP_ADDR_OFFSET       4
#define OF_CNTLR_IGMP_TYPE_OFFSET               0
#define OF_CNTLR_IGMP_CHECKSUM_OFFSET           2

/* macros for ARP */
#define OF_CNTLR_ARP_REQUEST                 1
#define OF_CNTLR_ARP_REPLY                   2

/* eth type macros */
#define OF_CNTLR_ETH_TYPE_IPV4               0x0800
#define OF_CNTLR_ETH_TYPE_ARP                0x0806
#define OF_CNTLR_ETH_TYPE_LLDP               0x88cc

#define OF_CNTLR_MAX_DATA_PATHS              50

#define OF_CNTLR_DP_SW_CONF_PATH             "/usr/local/ond_of_driver/app-conf/1.0/"

#define OF_CNTLR_ARP_STATE_SENT		     1

#define OF_CNTLR_ARP_STATE_TIMEOUT	     2

#define OF_CNTLR_ARP_STATE_LEARNED	     3

#define OF_CNTLR_ARP_STATE_STATIC        4

#define OF_CNTLR_ARP_REQ_TIMER           14000000

#define OF_CNTLR_NO_NAT				0
#define OF_CNTLR_SRC_NAT			1
#define OF_CNTLR_DST_NAT			2

#define OF_CNTLR_CALCULATE_CHECKSUM(cptr, size, cksum)\
{\
	uint32_t ii = 0;\
	cksum = 0;\
	while (size >= ii)\
	{\
		cksum |= *(uint16_t *)(cptr + ii);\
		ii += 2;\
	}\
	cksum = ~cksum;\
}

typedef struct cntlr_app_arp_req_s
{
  uint8_t  sender_hw_addr[OFP_ETH_ALEN];
  uint8_t  target_hw_addr[OFP_ETH_ALEN];
  uint16_t eth_type;
  uint16_t hw_type;
  uint16_t  proto_type; 
  uint8_t  hw_len;
  uint8_t  proto_len; 
  uint32_t sender_ip;
  uint32_t target_ip; 
  uint64_t datapath_handle;
  char     iface_name[OF_CNTLR_APP_IFACE_NAME_LEN];
  uint32_t in_port;
}cntlr_app_arp_req_t;

typedef struct cntlr_app_mcast_portinfo_s
{
	uint32_t port_id;
	uint32_t bucket_id;
	uint8_t port_name[OF_CNTLR_APP_IFACE_NAME_LEN];
	uint8_t binuse;
}mcast_portinfo_t;

typedef struct cntlr_app_mcast_rtentry_s
{
  uint32_t	grp_addr;
  mcast_portinfo_t port_info[OF_CNTLR_APP_MAX_MCAST_OUTPORTS];
  uint32_t	grp_id;
  uint32_t	flow_id;
  uint32_t	no_of_outports;
  uint8_t	binuse; /* TRUE if entry is being used and valid else FALSE */
}cntlr_app_mcast_rtentry_t;

typedef struct cntlr_app_l3rt_rtentry_s
{
  uint32_t rt_src;
  uint32_t rt_dest;
  uint32_t rt_mask;
  uint32_t rt_gateway;
  char     iface_name[OF_CNTLR_APP_IFACE_NAME_LEN];
  uint32_t rt_pmtu;
}cntlr_sm_app_l3rt_rtentry_t;

typedef struct cntlr_app_l3rt_arp_entry_s
{
  uint32_t arp_ip;
  uint8_t arp_mac[OF_CNTLR_APP_MAC_ADDR_LEN];
  uint8_t state;
  uint8_t count;
  
  struct tmr_saferef           arpreq_timer; /*Timer used to send keep alive messages to dp*/
  cntlr_app_arp_req_t *arp_req_tmr;
  uint64_t *datapath_handle_tmr;
}cntlr_sm_app_l3rt_arp_entry_t;

typedef struct cntlr_sm_app_fw_rule_list_s
{
  uint32_t src_ip;
  uint32_t dst_ip;
  uint16_t src_port;
  uint16_t dst_port;
  uint8_t  protocol;
  uint32_t snat_ip;
  uint32_t dnat_ip;

#define OF_CNTLR_FW_RULE_ALLOW 1
#define OF_CNTLR_FW_RULE_DENY  2
  uint8_t  action;
  char   in_iface[20];
}cntlr_sm_app_fw_rule_list_t;


#define OF_CNTLR_SW_MAX_IFACES         10
typedef struct cntlr_sm_arp_app_iface_info_s
{
  char   port_name[20];
  uint32_t ip_address;
  uint32_t net_mask;
  uint32_t bcast_addr;
  //uint8_t  hw_address[OF_CNTLR_APP_MAC_ADDR_LEN];
}cntlr_sm_arp_app_iface_info_t;

#define OF_CNTLR_MAX_FILE_NAME_LEN     80
typedef struct cntlr_sm_sw_conf_s
{
  uint64_t dp_id;
  uint32_t dp_index;
  uint64_t dp_handle;
  char   rt_arp_conf_file[OF_CNTLR_MAX_FILE_NAME_LEN]; 
  char   fw_rule_conf_file[OF_CNTLR_MAX_FILE_NAME_LEN]; 
  char   sw_iface_info_file[OF_CNTLR_MAX_FILE_NAME_LEN]; 

  cntlr_sm_app_l3rt_rtentry_t   rt_table[OF_CNTLR_APP_MAX_ROUTE_ENTRIES];
  cntlr_app_mcast_rtentry_t	  mcast_rt_table[OF_CNTLR_APP_MAX_MCAST_ROUTE_ENTRIES];

  cntlr_sm_app_l3rt_arp_entry_t arp_table[OF_CNTLR_APP_MAX_ARP_ENTRIES];

  cntlr_sm_app_fw_rule_list_t   fw_rule_list[OF_CNTLR_APP_MAX_FW_ACL_RULES];

  cntlr_sm_arp_app_iface_info_t iface_info[OF_CNTLR_SW_MAX_IFACES];

  uint32_t       no_of_route_entries;
  uint32_t       no_of_mcast_route_entries;
  uint32_t       no_of_arp_entries;
  uint32_t       no_of_fw_rules;
  uint32_t       no_of_iface_entries;
  uint8_t        ethtype_table_id;
  uint8_t        mcast_class_table_id;
  uint8_t        mcast_routing_table_id;
  uint8_t        session_table_id;
  uint8_t        l3_routing_table_id;
  uint8_t        arp_table_id;

}cntlr_sm_sw_conf_t;

int32_t sm_app_rt_get_out_iface_by_dstip(uint32_t dp_index, uint32_t dst_ip, char *out_iface);

int32_t sm_app_add_route_entry(uint64_t dp_handle, uint32_t dp_index, uint16_t priority,
                               uint32_t rt_src, uint32_t rt_dest,
                               uint32_t rt_mask, uint32_t rt_gateway,
                               uint32_t rt_pmtu, char *iface_name);

int32_t  sm_app_add_arp_entry(uint64_t datapath_handle, uint32_t dp_index, uint16_t priority,
                              uint64_t meta_data, uint64_t meta_data_mask,
                              char *dest_mac, char *src_mac, uint16_t out_port);

int32_t sm_app_add_session_entry(uint64_t datapath_handle, uint32_t dp_index, uint16_t priority,
                         uint32_t source_ip, uint32_t dest_ip,
                         uint16_t src_port, uint16_t dst_port,
                         uint8_t protocol, uint32_t in_port,
                         uint8_t icmp_type, uint8_t icmp_code,
                         uint32_t snat_ip, uint32_t dnat_ip);

void sm_app_print_fw_acl_rule_table(uint32_t dp_index);
void sm_app_print_arp_table(uint32_t dp_index);
void sm_app_print_l3routing_table(uint32_t dp_index);

void sm_app_print_iface_table(uint32_t dp_index);

int32_t sm_app_arp_request_n_response_handler(int64_t controller_handle,
                                      uint64_t domain_handle,
                                      uint64_t datapath_handle,
                                      struct of_msg *msg,
                                      struct ofl_packet_in  *pkt_in,
                                      void *cbk_arg1,
                                      void *cbk_arg2);

int32_t  get_port_handle_by_port_id(uint64_t dp_handle, uint16_t port_id, uint64_t *port_handle);

int32_t sm_app_sess_table_pkt_handler(struct of_app_msg *app_msg);

int32_t sm_app_arp_request_n_response_pkt_process(struct of_app_msg *app_msg);

int32_t sm_app_arp_table_pkt_process(struct of_app_msg *app_msg);

#endif /* _CNTLR_SM_APP_H */
