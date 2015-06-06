/** ARP Application Header File */

#define ARP_TABLE_SIZE 32 /* number of buckets in hash table */ 
#define MAX_ARP_ENTRIES 128
#define MAX_ARP_STATIC_ENTRIES 15

#define ETHER_TYPE_FOR_ARP 0x0806
#define HW_TYPE_FOR_ETHER 0x0001
#define OP_CODE_FOR_ARP_REQ 0x0001
#define HW_LEN_FOR_ETHER 0x06
#define HW_LEN_FOR_IP 0x04
#define PROTO_TYPE_FOR_IP 0x0800
#define ARP_ENTRY_IDLE_TIMEOUT 60 /*seconds(/proc/sys/net/ipv4/neigh/eth1/gc_stale_time)*/

uint32_t ulNeighHashInitValue_g; /*random value, 
                                      required for HASH calculation*/

#define ARP_HASH(ipaddr,Index,nsid,hash)   \
{\
  OF_DOBBS_WORDS2 (ipaddr, nsid,   \
                    ulNeighHashInitValue_g, \
		                  ARP_TABLE_SIZE, hash);\
 OF_LOG_MSG(OF_RT_LOG, OF_LOG_DEBUG,"hash key is:%d", hash); \
}


#define ARP_MAPNODE_HASH_TBLLNK_OFFSET offsetof(struct arp_rec_map_entry, hash_tbl_link);
#define ARP_TYPE_STATIC 0
#define ARP_TYPE_DYNAMIC 1
#define IFACE_HWADDR_LEN 6
#if 0
/*Arp state enum struct for cache*/
enum arp_state_e
{
  ARP_STATE_FREE, /*free - when no of retries becomes 0, can replace the record with the new entry*/
  ARP_STATE_PENDING,/*Pending - when waiting for the response. Send the request again periodically for every 5 secs and decrement ttl */
  ARP_STATE_RESOLVED /*Resolved - when arp reply is received and arp timer of lifetime will be decremented */
};
#endif
struct arp_rec
{
  uint8_t  dst_hwaddr[IFACE_HWADDR_LEN]; /* MAC address */
  uint32_t dest_ipaddr;/* IP Address */
  uint32_t ifnum; /* Router can be connected to different networks each with a different interface number*/
  uint32_t index;
  uint32_t magic;
  uint32_t flags; /*static, dynamic */
  uint16_t state; /* FREE, PENDING, RESOLVED */
  uint32_t nsid;

  uint16_t usHwAddrtype; /*Hardware type - 1 for Ethernet */
  uint16_t usProtoType; /*Protocol type - 0x0800 for ip, 0x0806 for arp */
  uint8_t  ucHwaddrLen; /*Hardware address length - 6 for ethernet */
  uint8_t  ucProtoLen; /*length of address used by upper layer protocol, 4 for ipv4*/
  uint32_t uiQueueNum; /*Queue number - ARP uses numbered queues to enqueue the packets for address resolution. 
			 Packets with same destination are enqueued in the same queue
			 ARP maintains one queue for each destination*/
  uint32_t uiNoOfRetries; /* Number of retries*/
  uint64_t ullArpLifeTime; /* ARP life time */
};

/*
   This structure will be used as member for ns_nsid_and_socket_mapping_repository hash table.
 */

struct arp_rec_map_entry
{
   struct mchash_table_link hash_tbl_link;   /* It is used by mchash table infrastructure to attach a this node to a ns_nsid_maping hash table  repository*/
   uint32_t index;   /* index of the safe reference to this node entry in mchash table */
   uint32_t magic;   /* index of the safe reference to this node entry in mchash table */
   struct arp_rec arp_rec_info;   /* pointer to the arp record struct which holds the mapping of dest IP, MAC and State*/
   of_list_t msg_queue;
   uint8_t heap_b;
};

/*
  This structure is used to trigger ARP request in kernel
*/
struct arp_req
{
  uint32_t dest_ip;
  uint32_t nsid;
  char peth_name[IFACE_NAME_LEN];  /*Interface ID*/
};

void arp_get_map_entry_mempoolentries(uint32_t* db_entries_max,uint32_t* db_static_entries);
int32_t arp_create_table_mempool();
int32_t trigger_arp_request_in_kernel_ioctl(struct arp_req *arpreq_p);
int32_t send_arp_pkt_to_nf_backend(struct arp_rec_map_entry *arp_entry_p, uint32_t api_type);
