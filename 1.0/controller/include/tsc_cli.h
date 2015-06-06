#define TSC_SUCCESS  0
#define TSC_FAILURE  1


#define  vn_nsc_info_offset_g  sizeof(struct crm_virtual_network)

#define ANY 0

struct nsc_repository_key
{
  char*    vn_name;
  int32_t  table_id;
};


struct nsc_repository_entry
{
   char      local_switch_name_p[64];
   uint64_t  dp_handle;
   uint32_t  nid;
   uint16_t  serviceport;
   uint32_t  remote_switch_ip;
   uint32_t  in_port_id;
   uint8_t   ns_chain_b;
   uint8_t   more_nf_b;

   uint16_t  vlan_id_pkt;
   uint16_t  match_vlan_id;
   uint16_t  next_vlan_id;
   uint8_t   nw_port_b;
   uint32_t  out_port_no;
   uint8_t   next_table_id;

   uint8_t   local_in_mac[6];
   uint8_t   local_out_mac[6];

   /*from selector inside nschain_repository_entry structure */
   uint32_t  src_mac_high; /* Total only 6 bytes */
   uint32_t  src_mac_low;
   uint32_t  dst_mac_high; /* Total only 6 bytes */
   uint32_t  dst_mac_low;

   uint8_t   src_mac[6];
   uint8_t   dst_mac[6];

   uint16_t  ethernet_type;

   uint8_t   protocol;
   uint16_t  src_port;
   uint16_t  dst_port;

   uint32_t  src_ip;
   uint32_t  dst_ip;

   uint64_t safe_reference;
};

struct ucastpkt_repository_entry
{
  char      local_switch_name_p[64];
  char      remote_switch_name_p[64];
  uint64_t  dp_handle;
  uint32_t  nid;
  uint64_t  tun_id;
  uint16_t  serviceport;
  uint8_t   dst_mac[6];
  uint8_t   nw_port_b;
  uint32_t  tun_dest_ip;
  uint8_t   port_type;
  uint32_t  out_port_no;
  uint64_t  safe_reference;
};



struct nsc_selector_node_key
{
  char*     l2map_record_name_p;
  char*     vn_name;
  uint8_t   nw_type; /*L2 or L3*/

  uint32_t  src_ip;  /*Can be a valid IP address or can be given as ANY*/
  uint32_t  dst_ip;  /*Can be a valid IP address or can be given as ANY*/
  
  uint16_t  dst_port;/*Can be a valid port number or can be given as ANY*/
  uint16_t  protocol;
};

struct tsc_cnbind_entry_info
{
  uint32_t  src_ip;  
  uint32_t  dst_ip;  

  uint16_t  dst_port;
  uint16_t  protocol;

  char*       nschainset_name_p;
  of_list_t   nwservices_info; /*Maintaining a list for UCM purpose */
  uint32_t    no_of_nwservice_instances;
};

struct tsc_nwservices
{
  of_list_node_t  next_node;
  struct          rcu_head rcu_head;
  uint8_t         heap_b;
  char            *nschain_name_p;
  char            *nwservice_object_name_p;
  char            *nwservice_instance_name_p;
};

#define CNBIND_NWSERVICE_LIST_NODE_OFFSET    offsetof(struct tsc_nwservices , next_node)


int32_t tsc_get_first_matching_cnbind_entry(struct nsc_selector_node_key   *selector_key,
                                            uint64_t                       *matched_node_saferef,
                                            struct tsc_cnbind_entry_info   *cnbind_entry_info_p);

int32_t tsc_get_next_matching_cnbind_entry(struct nsc_selector_node_key   *selector_key,
                                           uint64_t                       relative_node_saferef,
                                           uint64_t                       *matched_node_saferef,
                                           struct tsc_cnbind_entry_info   *cnbind_entry_info_p);


#if 0
/*****************************************************************************************************/
int32_t tsc_get_first_matching_cnbind_entry(struct nsc_selector_node_key   selector_key,
                                            uint64_t                       *matched_node_saferef,
                                            struct tsc_cnbind_entry_info   *cnbind_entry_info)
{
  /*
     1)Scan the selector table and get the first matched selector node.
     2)Copy the saferef of the matched selector node into matched_node_saferef pointer .This
       pointer is passed to the get_next API to get the next selector.
     3)From the obtained selector node , get the cnbind node info using the pointer 
       in the selector node.
     4)The obtained cnbind node holds a structure named cnbind_nwservice_info. This structure contains
       the information about the chain,chainset,service instances list .
     5)These values are filled into the tsc_cnbind_entry_info structure and returned to the caller.
  */
};
 
/*****************************************************************************************************/
int32_t tsc_get_next_matching_cnbind_entry(struct nsc_selector_node_key   selector_key,
                                           uint64_t                       relative_node_saferef,
                                           uint64_t                       *matched_node_saferef,
                                           struct tsc_cnbind_entry_info   *cnbind_entry_info) 
{
  /*
     1)Scan the selector hash table and search for the relative_node-saferef. Once it is found, skip 
       that selector node and proceed to the next matching node.
     2)Repeat the same steps as in get_first.
  */

};
/*****************************************************************************************************/
#endif
