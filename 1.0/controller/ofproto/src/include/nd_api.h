/**************************************************************************//**
@File          nd_api.h

@Description   This file contains the ARP & IPv6 Neighbor Discovery related 
               macros, data structures
*//***************************************************************************/

#ifndef __ARP_ND_H
#define __ARP_ND_H
#include "nf_infra_api.h"


#define MAX_ADDR_SIZE 16
#define IFACE_HWADDR_LEN 6
#define ARP_MAX_RETRIES 10

#define ARP_TYPE_STATIC 0
#define ARP_TYPE_DYNAMIC 1

#define ND_MAX_IFACE_NAME_LEN 16
/**************************************************************************//**
@Description	Arp state enum struct for cache
*//***************************************************************************/
enum arp_state_e
{
  ARP_STATE_FREE, /*free - when number of retries becomes 0 or when lifetime of a resolved entry expires, can replace the record with the new entry*/
  ARP_STATE_INCOMPLETE,/*Pending - when waiting for the response. Send the request again periodically for every 5 secs and decrement number of retries */
  ARP_STATE_PROBE,
  ARP_STATE_STALE,
  ARP_STATE_REACHABLE /*Resolved - when arp reply is received and arp timer lifetime will be decremented */
};

/**************************************************************************//**
@Description    Input parameters used for add/delete static arp record
*//***************************************************************************/
struct arp_inargs{
	ifid_t ifid;  /*Interface ID*/
	ip4addr_t ip_address; /*Destination IP Address */
	char mac_addr[IFACE_HWADDR_LEN];/*Destination MAC Address */
        uint32_t uiNsId; /*Namespace ID */
        uint16_t usHwAddrtype; /*Hardware type - 1 for Ethernet */
  	uint16_t usProtoType; /*Protocol type - 0x0800 for ip, 0x0806 for arp */
  	uint8_t  ucHwaddrLen; /*Hardware address length - 6 for ethernet */
  	uint8_t  ucProtoLen; /*length of address used by upper layer protocol, 4 for ipv4*/
	uint32_t uiQueueNum; /*Queue number - ARP uses numbered queues to enqueue the packets for address resolution. 
			 Packets with same destination are enqueued in the same queue
			 ARP maintains one queue for each destination*/
	uint32_t uiNoOfRetries; /* Number of retries*/
	uint64_t ullArpLifeTime; /* ARP life time */
	uint16_t uiState; /* FREE, PENDING, RESOLVED */
	uint32_t uiFlags; /*static, dynamic */
};

/**************************************************************************//**
@Description    Output parameters used for add/delete static arp record
*//***************************************************************************/
struct arp_outargs{
                int32_t result;
};

/**************************************************************************//**
@Description    Input parameters used for add/delete static neighbour record
*//***************************************************************************/
struct nd_inargs 
{
	ifid_t ifid; 
//	struct ip6_addr neigh6addr;  //Deepthi
	char mac_addr[MAX_ADDR_SIZE];
};

/**************************************************************************//**
@Description    Output parameters used for add/delete static neighbour record
*//***************************************************************************/
struct nd_outargs{
                int32_t result;
};

/**************************************************************************//**
@Description   List of capabilities 
*//***************************************************************************/
struct nd_capabilities {
#define ARP_CAP_AUTO_RESOLVE BIT(0)
#define ND6_CAP_AUTO_RESOLVE BIT(1)
/* TBD – more capabilities to be listed */
	uint64_t capabilities; 
};

/**************************************************************************//**
@Description   ARP packet structure used to send ARP Request to DP
*//***************************************************************************/
struct arp_pkt
{
  uint32_t uiNsid; /*Namespace ID */
  uint16_t usHwAddrtype; /*Hardware type - 1 for Ethernet */
  uint16_t usProtoType; /*Protocol type - 0x0800 for ip, 0x0806 for arp */
  uint8_t ucHwaddrLen; /*Hardware address length - 6 for ethernet */
  uint8_t ucProtoLen; /*length of address used by upper layer protocol, 4 for ipv4*/
  uint16_t ucOpcode; /* 1 for ARP request and 2 for ARP reply */ 
  uint8_t SenderHwAddr[IFACE_HWADDR_LEN];
  uint32_t uiSenderAddr;    
  uint8_t TargetHwAddr[IFACE_HWADDR_LEN];
  uint32_t uiTargetAddr; 
  char cIfaceName[IFACE_NAME_LEN]; /*Interface Name */
  uint32_t uiPort; /*port on which the response/request should be sent. will be outport for arp reply and inport for arp request*/
};

struct arpapp_arp_pkt
{
  uint32_t nsid; /*Namespace ID */
  uint32_t ifid;  /*Interface ID*/
  //uint32_t ip_addr;
  //char mac_addr[IFACE_HWADDR_LEN];/*Destination MAC Address */
  char peth_name[ND_MAX_IFACE_NAME_LEN];  /*Interface ID*/
  struct pkt_mbuf *pPkt; /*PSP buf that contains entire IP Packet received from DP*/
};
/**************************************************************************//**
@Description   The IP Pkt whose mac address is not resolved in the DP will send the IP pkt.
	       This structure is for IP packet received from DP for MAC Address resolution.
*//***************************************************************************/
struct arpapp_ip_pkt
{
  uint32_t nsid; /*Namespace ID */
  uint32_t gwipaddr; /*gateway IP Address  -- deepthi modified fron destipaddr*/
  uint32_t ifid;  /*Interface ID*/ 
  char peth_name[ND_MAX_IFACE_NAME_LEN];  /*Interface ID*/
  struct pkt_mbuf *pkt_mbuf_p; /*PSP buf that contains entire IP Packet received from DP*/
  void *opaque_p;
  uint64_t dp_id;
  of_list_node_t           node_entry;
};
struct arpapp_pkt_in_data{
  uint64_t dp_id;
  uint32_t port_id;
};
#define OF_ARP_PKT_IN_LISTNODE_OFFSET offsetof(struct arpapp_ip_pkt, node_entry)
/**************************************************************************//**
@Function       arp_add_record

@Description    This function is called by ARP application to add an ARP entry 
                to the ARP database being maintained in the Data Path.
		Destination IP and Destination HW addresses are added to cache
		for further use. Next time when a packet is received Destination IP address
		is validated in the ARP cache and the IP packet is sent with MAC address
		in the cache.

@Param[in]      flags - Flags indicate API response (Sync/Async/etc..).
@Param[in]      inargs - Input Parameters.
@Param[in]      resp - Response Information

@Param[out]     outargs - Output Parameters

@Return         Success or Failure (more errors TBD).

@Cautions       None.
*//***************************************************************************/
int32_t arp_add_record(struct arp_inargs *inargs, ZZZ_flags_t flags, struct arp_outargs *outargs, struct api_resp_args *resp);

/**************************************************************************//**
@Function       arp_del_record

@Description    This API deletes the ARP entry from Arp cache. This is done 
		based on the destination IP address provided.

@Param[in]      flags - Flags indicate API response (Sync/Async/etc..).
@Param[in]      inargs - Input Parameters.
@Param[in]      resp - Response Information

@Param[out]     outargs - Output Parameters

@Return         Success or Failure (more errors TBD).

@Cautions       None.
*//***************************************************************************/
int32_t arp_del_record(struct arp_inargs *inargs, ZZZ_flags_t flags, struct arp_outargs *outargs, struct api_resp_args *resp);

/**************************************************************************//**
@Function       arp_api_flush_entries

@Description    Flush all  ARP entries. 

@Param[in]      flags - Flags indicate API response (Sync/Async/etc..).
@Param[in]      in - Input parameters to flush static arp entries.(None).  
@Param[in]      resp - Response Information

@Param[out]     outargs - Output Parameters

@Return         Success or Failure (more errors TBD).

@Cautions       None.
*//**************************************************************************/
int32_t arp_api_flush_entries(struct arp_inargs *in, ZZZ_flags_t flags, struct arp_outargs *outargs, struct api_resp_args *resp);

/**************************************************************************//**
@Function       nd_api_get_version

@Description    Get neighborhood API version

@Param[in]      None 

@Param[out]     version - arp version

@Return         Success or Failure (more errors TBD).

@Cautions       None.
*//***************************************************************************/
int32_t nd_api_get_version(char *version);

/**************************************************************************//**
@Function       nd_api_get_capabilities

@Description    Get neighborhood API Capabilities

@Param[in]      None 

@Param[out]     nd_capabilities - List of capabilities.

@Return         Success or Failure (more errors TBD).

@Cautions       None.
*//***************************************************************************/
int32_t nd_api_get_capabilities(struct nd_capabilities *capabilities);

/**************************************************************************//**
@Function       nd6_update_record

@Description    This api is used to add/delete neighbor record from neighbour 
		table.
@Param[in]      flags - Flags indicate API response (Sync/Async/etc..).
@Param[in]      in - Input Parameters.
@Param[in]      resp - Response Information

@Param[out]     out - Output Parameters

@Return         Success or Failure (more errors TBD).

@Cautions       None.
*//**************************************************************************/
int32_t nd6_update_record(struct nd_inargs *inargs, ZZZ_flags_t flags, struct arp_outargs *outargs, struct api_resp_args *resp);

/* Callback function that is registered from ARP Application to receive arp request from DP */
typedef int32_t (*arpappln_rcvarppkt_fromdp_cbkfn) (struct arpapp_arp_pkt *pArpPkt);


/* Callback function that is registered by ARP Application to receive ip packet from DP for MAC Address resolution*/
typedef int32_t  (*arpappln_rcvippkt_fromdp_cbkfn) (struct arpapp_ip_pkt *pIPPkt);

/*ARP application register structure for callbacks */
struct arpapln_register_callbacks
{
  arpappln_rcvarppkt_fromdp_cbkfn arpappln_process_arppkt;
  arpappln_rcvippkt_fromdp_cbkfn arpappln_process_ippkt;
};

/*************************************************************************//**
@Function	arpappln_register_cbkfns

@Description    On ARP application init this function should be called to register
		callback function which should be called on receiving arp request/IP
	        Packet (in case of MAC address not found for destination IP) from DP.
		This function will register callback function pointer received from application
		and will call the respective callback function on receiving ARP Request/IP packet
		from DP.

@Param[in]	pArpApplnCbks - callback functions registered by the ARP application.

@Return		Success or Failure
		Success if the callback funcation is registerd successfully;
	       	else failure is	returned.

@Cautions	None.
*//***************************************************************************/
uint32_t arpappln_register_cbkfns(struct arpapln_register_callbacks *pArpApplnCbks);

#endif /* __ARP_ND_H */
