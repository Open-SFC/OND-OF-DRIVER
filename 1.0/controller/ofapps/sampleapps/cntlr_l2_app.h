#ifndef _CNTLR_L2_APP_H
#define _CNTLR_L2_APP_H

/* Macros */
#define L2_APP_ETHADDR_TYPE_TABLE_ID        0
#define L2_APP_MAC_LEARN_TABLE_ID           1
#define L2_APP_FIB_TABLE_ID                 2
#define L2_APP_BCAST_TABLE_ID               3
#define L2_APP_PKT_IN_PRIORITY     2

#define OF_CNTLR_APP_MAX_FIB_ENTRIES 64

typedef struct cntlr_l2app_mac_learn_req_s
{
  uint8_t  sender_hw_addr[OFP_ETH_ALEN];
  char     iface_name[OF_CNTLR_APP_IFACE_NAME_LEN];
  uint32_t in_port;
}cntlr_l2app_mac_learn_req_t;

typedef struct cntlr_l2app_fib_req_s
{
  uint8_t  dest_hw_addr[OFP_ETH_ALEN];
  char     iface_name[OF_CNTLR_APP_IFACE_NAME_LEN];
  uint32_t in_port;
}cntlr_l2app_fib_req_t;


typedef struct cntlr_sml2_sw_conf_s
{
  uint64_t dp_id;
  uint32_t dp_index;
  uint64_t dp_handle;

  char   eth_addr_type_file[OF_CNTLR_MAX_FILE_NAME_LEN]; 
  char   mac_learn_conf_file[OF_CNTLR_MAX_FILE_NAME_LEN]; 
  char   fib_conf_file[OF_CNTLR_MAX_FILE_NAME_LEN]; 
  char   bacst_conf_file[OF_CNTLR_MAX_FILE_NAME_LEN]; 

  cntlr_l2app_mac_learn_req_t            eth_addr_type_table[OF_CNTLR_APP_MAX_FIB_ENTRIES];
  cntlr_l2app_mac_learn_req_t   mac_learn_table[OF_CNTLR_APP_MAX_FIB_ENTRIES];
  cntlr_l2app_fib_req_t         fib_table[OF_CNTLR_APP_MAX_FIB_ENTRIES];
  cntlr_l2app_mac_learn_req_t            bacst_table[OF_CNTLR_APP_MAX_FIB_ENTRIES];

  uint32_t       no_of_ethaddr_type_entries;
  uint32_t       no_of_mac_learn_entries;
  uint32_t       no_of_fib_entries;
  uint32_t       no_of_bcast_entries;
}cntlr_sml2_sw_conf_t;

#endif /* _CNTLR_L2_APP_H */
