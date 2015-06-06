#ifndef __CNTRL_APP_CMN_H__
#define __CNTRL_APP_CMN_H__

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

#define L3ROUTING_LOCAL_TABLE_ID  3
#define L3ROUTING_PBR_TABLE_ID  50
#define L3ROUTING_ROUTE_TABLE_ID  51
#define SELF_PKT_TABLE_ID 52 

#define IPFWD_APP_SEFL_PKT_PRIORITY     1
#define MCAST_APP_SELF_PKT_PRIORITY     2
#define IPSEC_APP_SELF_PKT_PRIORITY     3
#define IPV4_FWD_DOMAIN "IPV4_FWD_DOMAIN"




#define OF_META_DATA_GW_MASK 0xffffffff00000000
#define OF_META_DATA_DP_NS_ID_MASK 0x000000000000ffff

#define OF_UPDATE_METADATA_WITH_DP_NS_ID(meta_data, meta_data_mask, dp_ns_id) \
{\
   meta_data = ((uint64_t)dp_ns_id);\
   meta_data_mask = OF_META_DATA_DP_NS_ID_MASK;\
} 

#define OF_UPDATE_METADATA_WITH_DP_NS_ID_AND_GW_IP(meta_data, meta_data_mask, dp_ns_id, gw_ipaddr) \
{\
   meta_data = (((uint64_t)dp_ns_id))|(((uint64_t)gw_ipaddr)<<32);\
   meta_data_mask = 0xffffffff0000ffff;\
}

#define OF_UPDATE_METADATA_WITH_GW_IP(meta_data, meta_data_mask,  gw_ipaddr) \
{\
   meta_data = ((meta_data)|(((uint64_t)gw_ipaddr)<<32));\
   meta_data_mask = OF_META_DATA_GW_MASK;\
}
#define OF_GET_DP_NS_ID_AND_GW_IP_FROM_METADATA(meta_data, dp_ns_id, gw_ipaddr) \
{\
   gw_ipaddr= ((meta_data & OF_META_DATA_GW_MASK) >> 32);\
   dp_ns_id=  (meta_data & OF_META_DATA_DP_NS_ID_MASK);\
}

#define OF_UPDATE_METADATA_WITH_DP_NS_ID_AND_IPSEC_ACTION(meta_data,\
                                     meta_data_mask, dp_ns_id, ipsec_action) \
{\
   meta_data = ((((uint64_t)dp_ns_id))|(ipsec_action<<16));\
   meta_data_mask = 0x0000000000ffffff;\
}

#define OF_UPDATE_METADATA_WITH_IPSEC_ACTION(meta_data,\
                                     meta_data_mask, ipsec_action) \
{\
   meta_data = (ipsec_action<<16);\
   meta_data_mask = 0x0000000000ff0000;\
}

#define OF_GROUP_MAX_GROUP_ID 4095
#define OF_GROUP_MIN_GROUP_ID 1000
   
#define OF_GROUP_GENERATE_GROUP_ID(id) \
{\
  group_id_g++; \
  OF_LOG_MSG(OF_RT_LOG, OF_LOG_DEBUG,"new group Id %d",group_id_g); \
  if( group_id_g == OF_GROUP_MAX_GROUP_ID) \
  group_id_g=OF_GROUP_MIN_GROUP_ID; \
  id=group_id_g; \
}     

#endif /* __CNTRL_APP_CMN_H__ */
