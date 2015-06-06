//#define IPV4_FWD_DOMAIN "IPV4_FWD_DOMAIN"

struct ipfwd_db{
  of_list_t route_list;
  of_list_t group_list;
};

struct route_entry{
  of_list_node_t     next_route;
  uint32_t ns_id;
  uint32_t ip_addr;
  uint32_t net_mask;
  uint32_t metric;
  uint32_t flow_priority;
  uint32_t rt_tbl_id;
  uint32_t group_id;
  of_list_t gw_list;
};

struct route_gw{
	of_list_node_t next_gw;
	uint32_t gw;
	uint32_t port_id;
	uint32_t write_group_id;
	uint32_t path_mtu;
//	uint32_t flow_priority;
	uint16_t dp_ns_id;
	uint32_t rt_tbl_id;
	uint32_t tos;
};
#define OF_ROUTE_DB_LISTNODE_OFFSET offsetof(struct route_entry, next_route)
#define OF_ROUTE_GW_DB_LISTNODE_OFFSET offsetof(struct route_gw, next_gw)
#define OF_ROUTE_GET_ROUTE_GW(route_entry_p) (struct route_gw *)((unsigned char *)OF_LIST_FIRST(route_entry_p->gw_list)-OF_ROUTE_GW_DB_LISTNODE_OFFSET);


#define OF_ROUTE_NEW_GROUP_VALUE 0xffffffff
#define OF_ROUTE_MAX_WEIGHT 0xff
#define OF_ROUTE_NEW_GROUP_FOUND 2
#define OF_ROUTE_GROUP_EXISTS 3
#define OF_ROUTE_NO_GROUP_EXISTS 4


#define OF_GROUP_WRITE_GROUP 1
#define OF_GROUP_APPLY_GROUP 2

#if 0
#define OF_GROUP_MAX_GROUP_ID 4095
#define OF_GROUP_MIN_GROUP_ID 1000
#define OF_GROUP_GENERATE_GROUP_ID(id) \
{\
  group_id_g++; \
  OF_LOG_MSG(OF_RT_LOG, OF_LOG_DEBUG,"new group Id %d",group_id_g); \
  if ( group_id_g == OF_GROUP_MAX_GROUP_ID ) \
  group_id_g=OF_GROUP_MIN_GROUP_ID; \
  id=group_id_g; \
}     
#endif

#define CNTLR_PRINT_PKT_DATA(pkt, length) \
{\
        int32_t i, j;\
        char data[1024];\
	    OF_LOG_MSG(OF_RT_LOG, OF_LOG_DEBUG,"pkt length %d", length);\
        for (i =0, j=0; ((i < length) && (j < 1024-6)); i++, j=j+3)\
        {\
                if (!(i%16))\
                {\
                        sprintf(&data[j],"\n");\
                        j++;\
                }\
                if (!(i%8))\
                {\
                        sprintf(&data[j],"  ");\
                        j += 2;\
                }\
                sprintf(&data[j],"%02x ",*(pkt + i ));\
        }\
	OF_LOG_MSG(OF_RT_LOG, OF_LOG_DEBUG, "pkt data %s", data);\
}

enum ROUTE_ACTION_FLAGS{
 OF_ROUTE_DB_DEC_NEW_TTL=1 << 1,
 OF_ROUTE_DB_ICMPV4_TIME_EXCEED=1 << 2,
 OF_ROUTE_DB_IP_FRAGMENT_ACTION=1 <<3,
 OF_ROUTE_DB_WRITE_METADATA_ACTION =1 << 4,
 OF_ROUTE_DB_NXAST_WRITE_REG=1 << 5,
 OF_ROUTE_DB_NXAST_WRITE_REG2=1 << 6,
 OF_ROUTE_DB_OUTPUT_TO_PORT =1 << 7,
};

#if 0
#define OF_META_DATA_GW_MASK 0xffffffff00000000
#define OF_META_DATA_DP_NS_ID_MASK 0x00000000000000ffff

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
#endif
/******************************************************************************
 * * * * * * * * * * * * * * Function Prototypes* * * * * * * * * * * * * * * * 
 *******************************************************************************/

void ipfwd_db_init();
int32_t ipfwd_db_add_route_entry(uint32_t ns_id,uint32_t ip_addr, uint32_t net_mask,  uint32_t metric, uint32_t rt_tbl_id, struct route_entry **route_entry_pp);
int32_t ipfwd_db_get_route_entry(uint32_t ns_id, uint32_t ip_addr, uint32_t net_mask,  int32_t metric, uint32_t rt_tbl_id, struct route_entry **route_entry_pp);
int32_t ipfwd_db_delete_route_entry( struct route_entry *route_entry_p);
int32_t ipfwd_db_delete_route_gw_entry( struct route_entry *route_entry_p,int32_t gw);

int32_t ipfwd_db_create_group(uint64_t datapath_handle, struct route_entry *route_entry_p, int32_t flags, uint32_t *group_id_p);
int32_t ipfwd_db_delete_group(uint64_t datapath_handle, uint32_t group_id);
int32_t ipfwd_db_update_group(uint64_t datapath_handle, struct route_entry *route_entry_p, int32_t flags);
