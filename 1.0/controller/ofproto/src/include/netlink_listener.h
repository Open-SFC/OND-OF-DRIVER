
#define T_SUCCESS 0
#define T_FAILURE -1

#define RTL_NS_NAME_LEN  NEM_MAX_NS_NAME_LEN
#define RTL_NS_SIZE  OF_NEM_MAX_NAMESPACE_ENTRIES

#define IFACE_NAME_LEN (32)
#define HWADDR_LEN     (32)
#define MAX_IFACE_ENTRIES 256 
#define MAX_IFACE_STATIC_ENTRIES 64 
#define IFACE_MAX_RECS MAX_IFACE_ENTRIES 
#define IP4_RT_TABLE_LOCAL 255
/* Thread specific ID of Controlle process thread*/
//__thread uint32_t rtl_thread_index;

/** Name space and corresponding socket information data structure */
struct rtl_ns_data
{
   /** Name of the namespace */
   char namespace_name[RTL_NS_NAME_LEN + 1];
  
   /** Namespace ID */
   uint16_t ns_id;
   uint64_t  dp_id;       
   uint16_t dp_ns_id;       
   uint32_t port_id;
   char port_name[NEM_MAX_IFACE_NAME_LEN];
   char peth_name[NEM_MAX_IFACE_NAME_LEN];

   /** Netlink Socket FD for routes */
   int32_t rt_sock_fd;
};
struct iface_info 
{
  uint16_t        ns_id;
  uint32_t        flags;
  int32_t         mtu;
  uint16_t        type;

  struct {
    void  *ip4;
    void  *ip6;
    void  *pOpaque1;
    void  *pOpaque2;
  } l3;

  unsigned char     hw_addr[HWADDR_LEN];
  uint16_t     hw_addr_len;

  int32_t      os_ifindex;
  char      iface_name[IFACE_NAME_LEN];
  uint32_t  ip_addr;
  uint32_t dst_ipaddr;
}; 
uint32_t iface_buckets_g; 
struct   mchash_table* iface_table_g; 

typedef uint32_t ipaddr;
struct listener_register_app *app_tail_node_p;
struct listener_register_app *app_head_node_p;

struct rtl_pkt_msg *pkt_head_msg_p;
struct rtl_pkt_msg *pkt_tail_msg_p;

#define FOR_EACH_NETLINK_NODE(node) \
        for ((node) = app_head_node_p; (node) != NULL; (node) = (node)->next)
//typedef int32_t (*rtl_cbfunc)(uint32_t ns_id, struct sockaddr_nl *snl, struct nlmsghdr *h, void *cbarg);
typedef int32_t (*rtl_cbfunc)(char* namespace_name, struct sockaddr_nl *snl, struct nlmsghdr *h, void *cbarg);


struct listener_register_app
{
  struct listener_register_app *next;

  uint32_t                    groups;
  rtl_cbfunc             cbfunc;
  void                        *cbarg_p;
  uint32_t                    priority;
};


struct rtl_pkt_msg
{
  struct rtl_pkt_msg *next;

  uint8_t table_id;
  uint32_t nsid;
  struct ofl_packet_in  *pkt_in;
  struct pkt_mbuf *pkt;
};

enum ipfwd_app_api_type {
  IPFWD_RT_ADD_REC,
  IPFWD_RT_DEL_REC,
  IPFWD_PBR_ADD_REC,
  IPFWD_PBR_DEL_REC,
  IPFWD_APP_SEND_PKT_OUT_DP,
  IPFWD_IFACE_ADD_REC,
  IPFWD_IFACE_DEL_REC
};

struct ipfwd_app_api_cb_info {
  enum ipfwd_app_api_type api_type;
  uint64_t seq_no;
  uint32_t ns_id;
};

struct route_app_peth_info {
  uint32_t ns_id;
  struct pkt_mbuf *pkt;
  int32_t if_index;
};

struct route_app_trnsprt_thread_info {
  uint32_t notification_type;
  struct of_nem_notification_data nem_data_s;
};


int32_t rtl_create_nlsock_for_route();
int32_t rtl_nlsock_bind(struct rtl_ns_data *nsdata_p);
int32_t close_nl_socket(int32_t netlink_fd);
//void handle_netlink_rtmsgs(int32_t sockfd, struct epoll_user_data_t *info_p, uint32_t  received_events);
//void netlink_read_event_rtmsgs(struct rtl_ns_and_sock_map_entry *map_entry_p);
//void read_rt_netlink_events(struct rtl_ns_and_sock_map_entry *map_entry_p, uint8_t bdone);
//void netlink_read_complete_rtmsg(struct rtl_ns_and_sock_map_entry *map_entry_p);
int32_t netlink_msgtype_to_grp(uint32_t msgtype);
int32_t listener_register_cbfn(uint32_t groups, int32_t priority, rtl_cbfunc cbfunc, void *cbarg_p);
int32_t rt_get_ns_info_by_name(char *namespace_name, struct rtl_ns_data *rtl_ns_data_p);


int32_t rtl_app_thread_init();
void* rtl_app_thrd_entry(void *pArg);
int32_t rtl_enqueue_msg_2_appxprt_conn(struct rtl_pkt_msg *msg);


#ifdef CONFIG_VQA_PSP_KERNEL_PATCH
enum {

   RTM_NEWVIF = 90,
#define RTM_NEWVIF RTM_NEWVIF
   RTM_DELVIF,
#define RTM_DELVIF RTM_DELVIF
   RTM_GETVIF,
#define RTM_GETVIF RTM_GETVIF

   RTM_NEWMCADDR = 94,
#define RTM_NEWMCADDR RTM_NEWMCADDR
   RTM_DELMCADDR,
#define RTM_DELMCADDR RTM_DELMCADDR
   RTM_GETMCADDR,
#define RTM_GETMCADDR RTM_GETMCADDR

   RTM_NEWMIF = 98,
#define RTM_NEWMIF RTM_NEWMIF
   RTM_DELMIF,
#define RTM_DELMIF RTM_DELMIF
   RTM_GETMIF,
#define RTM_GETMIF RTM_GETMIF

   RTM_NEWMC6ADDR = 102,
#define RTM_NEWMC6ADDR RTM_NEWMC6ADDR
   RTM_DELMC6ADDR,
#define RTM_DELMC6ADDR RTM_DELMC6ADDR
   RTM_GETMC6ADDR,
#define RTM_GETMC6ADDR RTM_GETMC6ADDR

   RTM_BRIDGEINFO = 106,
#define RTM_BRIDGEINFO RTM_BRIDGEINFO 
};
#endif

