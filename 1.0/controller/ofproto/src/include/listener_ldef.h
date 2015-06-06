
#define RTL_MAX_NAMESPACES OF_NEM_MAX_NAMESPACE_ENTRIES 
#define RTL_MAX_NAMESPACE_STATIC_ENTRIES OF_NEM_MAX_NAMESPACE_STATIC_ENTRIES
#define RTL_SUCCESS 0

/** \ingroup Error_Management
 * RTL_FAILURE 
 * - Route Listener API Functions return this value on general failure in execution
 */
#define RTL_FAILURE -1

/** \ingroup Error_Management
 * RTL_ERROR_NO_MEM 
 * - No memory available to create the record.
 */
#define RTL_ERROR_NO_MEM -2

/** \ingroup Error_Management
 * RTL_ERROR_NAME_LENGTH_NOT_ENOUGH 
 * - Name of the resource must be atleast 8 characters.
 */
#define RTL_ERROR_NAME_LENGTH_NOT_ENOUGH -3

/** \ingroup Error_Management
 *  RTL_ERROR_NO_MORE_ENTRIES 
 *  - No more resource entries in the database.
 */

#define RTL_ERROR_NO_MORE_ENTRIES -4


#define RTL_ERROR_INVALID_NAME -5
#define RTL_ERROR_INVALID_HANDLE -6
#define RTL_ERROR_DUPLICATE_RESOURCE -7
#define RTL_ERROR_INVALID_NOTIFICATION_TYPE -8
#define RTL_ERROR_NULL_CALLBACK_NOTIFICATION_HOOK -9
#define RTL_ERROR_INVALID_NSID -10

#define ARP_APP_PKT_IN_PRIORITY     1

/* Resolved arp packets are stored in this table */
#define ARP_RESOLUTION_TABLE_ID        80
/* ARP packet-in's stored in this table */
#define ARP_APP_IN_BOUND_TABLE_ID		10 


#define OF_CNTLR_APP_IFACE_NAME_LEN   12
/* macros for ARP */
#define OF_CNTLR_ARP_REQUEST                 1
#define OF_CNTLR_ARP_REPLY                   2

/* eth type macros */
#define OF_CNTLR_ETH_TYPE_IPV4               0x0800
#define OF_CNTLR_ETH_TYPE_ARP                0x0806

#define OF_CNTLR_IPV4_SRC_IP_OFFSET        12 
#define OF_CNTLR_IPV4_DST_IP_OFFSET        16
#define OF_CNTLR_MAC_HDR_LEN        14
#define OF_CNTLR_HW_ADDR_LEN        6
#define OF_CNTLR_IPV4_HDR_OFFSET    OF_CNTLR_MAC_HDR_LEN
#define OF_CNTLR_IPV4_HDR_LEN(x)   (((*(x + OF_CNTLR_IPV4_HDR_OFFSET)) & 0x0F) << 2)
#define OF_CNTLR_IPV4_TTL_OFFSET    8

#define OF_CNTLR_IP_PROTO_UDP      0x11
#define OF_CNTLR_IP_PROTO_TCP      0x06
#define OF_CNTLR_IP_PROTO_ICMP     0x01
#define OF_CNTLR_IP_PROTO_IGMP     0x02
#define OF_CNTLR_IP_PROTO_ESP      0x32
#define OF_CNTLR_IP_PROTO_GRE      0x2F
#define OF_CNTLR_IP_PROTO_SCTP     0x84
#define OF_CNTLR_IP_PROTO_IPIP     0x5E

#define OF_CNTLR_IPV4_PROTO_OFFSET          9
#define OF_CNTLR_IPV4_FRAG_OFFSET          6
#define OF_CNTLR_IPV4_RES_FRAG 0x80
#define OF_CNTLR_IPV4_DONT_FRAG 0x40
#define OF_CNTLR_IPV4_MORE_FRAG 0x20

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

#define RT_DOBBS_MIX(a,b,c) \
{ \
      a -= b; a -= c; a ^= (c>>13); \
      b -= c; b -= a; b ^= (a<<8); \
      c -= a; c -= b; c ^= (b>>13); \
      a -= b; a -= c; a ^= (c>>12);  \
      b -= c; b -= a; b ^= (a<<16); \
      c -= a; c -= b; c ^= (b>>5); \
      a -= b; a -= c; a ^= (c>>3);  \
      b -= c; b -= a; b ^= (a<<10); \
      c -= a; c -= b; c ^= (b>>15); \
}
#define RT_DOBBS_WORDS4(word_a, word_b, word_c, word_d, initval, \
                                 hashmask, hash) \
{ \
      register unsigned long temp_a, temp_b, temp_c; \
      temp_a = temp_b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */\
      temp_c = initval;         /* random value*/ \
    \
      temp_a += word_a; \
      temp_b += word_b; \
      temp_c += word_c; \
    \
      RT_DOBBS_MIX(temp_a, temp_b, temp_c); \
    \
      temp_a += word_d; \
      RT_DOBBS_MIX(temp_a, temp_b, temp_c); \
    \
      hash = temp_c & (hashmask-1); \
}

#define RT_COMPUTE_HASH(param1,param2,parm3,parm4,param5,hashkey) \
                          RT_DOBBS_WORDS4(param1,param2,param3,param4,0xa2956174,param5,hashkey) 
/*
   This structure will be used as member for ns_nsid_and_socket_mapping_repository hash table.
 */

struct rtl_ns_and_sock_map_entry
{
   struct mchash_table_link hash_tbl_link;   /* It is used by mchash table infrastructure to attach a this node to a ns_nsid_maping hash table  repository*/
   uint32_t index;   /* index of the safe reference to this node entry in mchash table */
   uint32_t magic;   /* index of the safe reference to this node entry in mchash table */
   struct rtl_ns_data rtl_ns_info;   /* pointer to the namespace entry struct which holds the mapping values of namespace name, id  and socket fd*/
   uint8_t heap_b;
};

#define RTL_NSID_MAPNODE_HASH_TBLLNK_OFFSET offsetof(struct rtl_ns_and_sock_map_entry, hash_tbl_link);
/******************************************************************************
 * * * * * * * * * * * * * * RTL INIT * * * * * * * * * * * * * * * * * * * *
 *******************************************************************************/
int32_t netlink_listener_init();

int32_t rtl_ns_mempool_init();

uint32_t rt_get_hashval_byname(char* name_p,uint32_t no_of_buckets);

struct iface_map_entry
{
   struct mchash_table_link hash_tbl_link;   /* It is used by mchash table infrastructure to attach a this node to a ns_nsid_maping hash table  repository*/
   uint32_t index;   /* index of the safe reference to this node entry in mchash table */
   uint32_t magic;   /* index of the safe reference to this node entry in mchash table */
   struct iface_info iface;   /* pointer to the namespace entry struct which holds interface and ipaddress and name space details */
   uint8_t heap_b;
};
#define IFACE_MAPNODE_HASH_TBLLNK_OFFSET offsetof(struct iface_map_entry, hash_tbl_link);

//int32_t iface_get_ifid_from_ifname(uint32_t ns_id, char *if_name, uint32_t *ifid);
int32_t iface_get_mtu_from_ifid(uint32_t ns_id, uint32_t ifid, int32_t *mtu);
//int32_t iface_get_ifname_from_ifid(uint32_t ns_id, int32_t ifid, char *if_name);
//int32_t listener_get_nsname_from_nsid(uint32_t ns_id, char *namespace_name);
