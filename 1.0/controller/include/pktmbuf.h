
/** \addtogroup PBUFSTRUCTS PSP Buffer Module Data Structures
*/
/** \addtogroup PBUFMACROS PSP Buffer Access Macros
*/

/** \addtogroup PBUFFLAGS PSP Buffer Flag Macros
*/

/** \addtogroup PBUFFLAGS PSP Buffer Flag Macros
*/

/**************************************************************************
 * Copyright 2011-2012, Freescale Semiconductor, Inc. All rights reserved.
 ***************************************************************************/
/*
 * File:	pspbuf.h
 *
 * Description: PSP buffer module for PSP.
 *
 * Authors:	Venu Maya <venu_maya@freescale.com>
 * Modifier:
 *
 */
/*
 * History
 * 20 Sep 2011 - Venu Maya - Initial Implementation.
 *
 */
/******************************************************************************/
#ifndef __PSPBUF_H
#define __PSPBUF_H

/*********************** Structures ******************/
// #include <psp/ip6.h>

#ifdef CONFIG_PSP_DEBUG_FEATURES
extern uint32_t pkt_mbuf_dbg;
#define pkt_mbuf_debug(fmt,arg...) do { if(pkt_mbuf_dbg) PSPPrint("<T%d> %s:%d - %s - " fmt, PSP_SELF_THREAD_INDEX(), __FILE__,__LINE__,__FUNCTION__,##arg); } while(0)
#else
#define pkt_mbuf_debug(fmt,arg...) do { } while(0)
#endif

/* Removing IPv6 supoort for now. - Mallik. */
// #define PSP_IPV6_SUPPORT

#ifndef PSP_VLAN_ID_MASK
#define PSP_VLAN_ID_MASK  0xfff
#endif

#define PSP_BIT(x)      (1<<((x)))

#define PSP_BIT0        PSP_BIT(0)
#define PSP_BIT1        PSP_BIT(1)
#define PSP_BIT2        PSP_BIT(2)
#define PSP_BIT3        PSP_BIT(3)
#define PSP_BIT4        PSP_BIT(4)
#define PSP_BIT5        PSP_BIT(5)
#define PSP_BIT6        PSP_BIT(6)
#define PSP_BIT7        PSP_BIT(7)
#define PSP_BIT8        PSP_BIT(8)
#define PSP_BIT9        PSP_BIT(9)
#define PSP_BIT10       PSP_BIT(10)
#define PSP_BIT11       PSP_BIT(11)
#define PSP_BIT12       PSP_BIT(12)
#define PSP_BIT13       PSP_BIT(13)
#define PSP_BIT14       PSP_BIT(14)
#define PSP_BIT15       PSP_BIT(15)
#define PSP_BIT16       PSP_BIT(16)
#define PSP_BIT17       PSP_BIT(17)
#define PSP_BIT18       PSP_BIT(18)
#define PSP_BIT19       PSP_BIT(19)
#define PSP_BIT20       PSP_BIT(20)
#define PSP_BIT21       PSP_BIT(21)
#define PSP_BIT22       PSP_BIT(22)
#define PSP_BIT23       PSP_BIT(23)
#define PSP_BIT24       PSP_BIT(24)
#define PSP_BIT25       PSP_BIT(25)
#define PSP_BIT26       PSP_BIT(26)
#define PSP_BIT27       PSP_BIT(27)
#define PSP_BIT28       PSP_BIT(28)
#define PSP_BIT29       PSP_BIT(29)
#define PSP_BIT30       PSP_BIT(30)
#define PSP_BIT31       PSP_BIT(31)

#define PSP_BIT_SET(var,bit) do { var |= (bit); } while(0)
#define PSP_BIT_RST(var,bit) do { var &= ~(bit); } while(0)

#define PSP_CACHE_LINE_SIZE     32

#define CACHE_ALIGN __attribute__((aligned(PSP_CACHE_LINE_SIZE)))
#define PAD_CACHE_LINE uchar8_t cachelinepad[0] CACHE_ALIGN;

enum {
  PSP_L2PROTO_LOOP =         0x0060,
  PSP_L2PROTO_ETHER =        0x0001,
  PSP_L2PROTO_IP4 =          0x0800,
  PSP_L2PROTO_IP6 =          0x86DD,
  PSP_L2PROTO_ARP =          0x0806,
  PSP_L2PROTO_VLAN =         0x8100, /* 8021Q VLAN */
  PSP_L2PROTO_IPX =          0x8137,
  PSP_L2PROTO_PPP_DISCOVERY= 0x8863,          /* PPPoE discovery data */
  PSP_L2PROTO_PPP_SESSION =  0x8864,          /* PPPoE session data */
  PSP_L2PROTO_MPLS_UNICAST = 0x8847,          /* MPLS Unicast data */
  PSP_L2PROTO_MPLS_MULTICAST=0x8848,          /* MPLS Multicast data */

  /* special codes */
  PSP_L2PROTO_OTHER =        0x0002,
  PSP_L2PROTO_ALL  =         0xFFFE
};

#define PSP_L2PROTO_IP  PSP_L2PROTO_IP4
#define PSP_L2PROTO_IP6 PSP_L2PROTO_IP6


enum {
 PSP_IPPROTO_HOP = 0,
 PSP_IPPROTO_ICMP  =  1,
 PSP_IPPROTO_IGMP  =  2,
 PSP_IPPROTO_IPIP =  4,
 PSP_IPPROTO_TCP  =  6,
 PSP_IPPROTO_EGP =  8,
 PSP_IPPROTO_UDP  =  17,
 PSP_IPPROTO_IPV6 = 41,
 PSP_IPPROTO_ROUTING = 43,
 PSP_IPPROTO_FRAGMENT = 44,
 PSP_IPPROTO_RSVP =  46,
 PSP_IPPROTO_GRE =  47,
 PSP_IPPROTO_ESP =  50,
 PSP_IPPROTO_AH =  51,
 PSP_IPPROTO_ICMP6 = 58,
 PSP_IPPROTO_NO_EXTHDR = 59,
 PSP_IPPROTO_DESTOPTS = 60,
 PSP_IPPROTO_SCTP =  132,
 PSP_IPPROTO_MOBILE_IP = 135,
 PSP_IPPROTO_UDPLITE =  136,
 PSP_IPPROTO_IPCOMP = 108,
 /* special codes */
 PSP_IPPROTO_OTHER  =  254,
 PSP_IPPROTO_ALL  =  255,
};

enum of_appl_type {
 OF_APP_TYPE_NONE = 0,
 OF_APP_TYPE_ARP,
 OF_APP_TYPE_IPFWD,
 OF_APP_TYPE_IPSEC,
 OF_APP_TYPE_MAX
};

/* Application subtype for of_appl_type OF_APP_TYPE_IPSEC */
#define IPSEC_ENCRYPT_AND_SEND  1
#define IPSEC_DECRYPT_AND_SEND  2

/** \ingroup PBUFSTRUCTS
This structure is used by applications to refer PSPBuf data.
All user references to PSPBuf data should be through 'struct pkt_mbuf_data_ref'.

\struct struct pkt_mbuf_data_ref

\verbatim
struct pkt_mbuf_data_ref
{
        uchar8_t         *ptr;
        struct pkt_mbuf  *buf;
};
\endverbatim

\n<b> Fields: </b>\n
<i> ptr  - </i>pointer to the data.
\n<i> buf  - </i> pointer to the PSPBuf which contains the data.
*/

struct pkt_mbuf_data_ref
{
        uchar8_t         *ptr;
        struct pkt_mbuf  *buf;
};

/** \ingroup PBUFSTRUCTS
This structure represents the packet data.
It contains meta-data associated with packet and the actual packet data.

\struct struct pkt_mbuf

\verbatim
struct pkt_mbuf
{
        struct pkt_mbuf                 *pPrev;
        struct pkt_mbuf                 *pNext;

        uchar8_t                        *start_of_data;
        struct pkt_mbuf_data_ref        data;
        struct pkt_mbuf_data_ref        end;
        uint32_t                        count;

        uint32_t                        data_ref_cnt;
        PSPLock_t                       data_ref_lock;

        void                          (*freertn)(void*);
        void                          *arg;

        uint64_t                        time_stamp;

        struct safe_ref                 orig_rx_iface_id;
        struct safe_ref                 rx_iface_id;
        uint32_t                        ns_id;
        uint16_t                        rx_vlan_info;

        uint16_t                        l2_proto;
        uint16_t                        ex_l2_proto;
        uchar8_t                        l3_proto;
        struct pkt_mbuf_data_ref        mac_hdr;
        struct pkt_mbuf_data_ref        network_hdr;
        struct pkt_mbuf_data_ref        transport_hdr;
        union
        {
                struct
                {
                        uint32_t        src;
                        uint32_t        dst;
                        uint32_t        total_len;
                        uchar8_t        tos;
                        struct
                        {
                                uint32_t        next_hop;
                                uchar8_t        opt_len;
                                uchar8_t        srr;
                                uchar8_t        rr;
                                uchar8_t        ts;
                                uchar8_t        is_strictroute:1,
                                                srr_is_hit:1,
                                                is_changed:1,
                                                rr_needaddr:1,
                                                ts_needtime:1,
                                                ts_needaddr:1;
                        }ip_opts;
                }ip4;
        #ifdef PSP_IPV6_SUPPORT
                struct
                {
                        PSPIPv6Addr_t   src;
                        PSPIPv6Addr_t   dst;
                        uint32_t        payload_len;
                        uint32_t        flow_info;
                        uchar8_t        transport_proto;
                        uchar8_t        traffic_class;
                        struct
                        {
                                uint32_t        hop_by_hop;
                                uint32_t        route;
                                uint32_t        dest0;
                                uint32_t        dest1;
                                uint32_t        fragment;
                                uint32_t        transport;
                                uint32_t        num_ext_hdr;
                                uint32_t        last_opt;
                                uint32_t        nh_offset;
                                uint32_t        ext_hdrs_len;
                                uint32_t        ra_offset;
                                uint32_t        unfrag_len;
                                uint32_t        next_proto;
                                uint32_t        flags;
                        #define PKT_MBUF_IPV6_EXTHDR_PROCESSED          PSP_BIT0
                        #define PKT_MBUF_IPV6_EXTHDR_HOP_BY_HOP         PSP_BIT1
                        #define PKT_MBUF_IPV6_EXTHDR_DEST0              PSP_BIT2
                        #define PKT_MBUF_IPV6_EXTHDR_DEST1              PSP_BIT3
                        #define PKT_MBUF_IPV6_EXTHDR_ROUTING            PSP_BIT4
                        #define PKT_MBUF_IPV6_EXTHDR_HAO_PRESENT        PSP_BIT5
                        #define PKT_MBUF_IPV6_EXTHDR_FRAG               PSP_BIT6
                        #define PKT_MBUF_IPV6_JUMBOPAYLOAD              PSP_BIT7
                        #define PKT_MBUF_IPV6_PROC_BEFORE_DEFRAG        PSP_BIT8
                        }params;
                }ip6;
        #endif
        }nw_tuple;

        union
        {
                uint32_t        val;
                uint32_t        spi;
                uint32_t        id;
                struct
                {
                        uint32_t        icmp_type:8;
                        uint32_t        icmp_code:8;
                };
                struct
                {
                        uint32_t        mh_type:8;
                        uint32_t        mhn_ext_proto:8;
                };
                struct
                {
                        uint32_t        src_port:16;
                        uint32_t        dst_port:16;
                };
        }xport_tuple;

        uint64_t        of_metadata;
        uint32_t        of_table_miss;
        uint32_t        of_table_id;
        uint32_t        of_dpath_id;
        union
        {
                void            *of_flow;
                uint32_t        app_flags;
        };
        struct
        {
                struct pkt_mbuf *next;
                struct pkt_mbuf *prev;
                struct pkt_mbuf *head;
                uint32_t        total_len;
        }frag;

        uint32_t                flags;
        uchar8_t                pkt_type;
        uint32_t                gateway_ip;
        uint32_t                path_mtu;
        struct safe_ref         tx_iface_id;
        uint16_t                tx_vlan_info;
        uint32_t                qos_prio;
        uint32_t                max_data_size;
        struct pkt_mbuf         *parent;
        bool8_t                 cloned;
        void                  *buf_pool;
        t_phyaddr               buf_phy_addr;
        uint32_t                port_num;
        t_phyaddr               sod_phy_addr;

        union
        {
                struct safe_ref route_cache_ref;
                struct safe_ref route6_cache_ref;
        };
        uint32_t        route_cache_magic;
        struct
        {
                struct pkt_mbuf         *prev;
                struct pkt_mbuf         *next;
                struct pkt_mbuf         *head;
                uint32_t                rem_nodes;
                uint32_t                rem_len;
        }sg;

        #ifdef CONFIG_PKT_MBUF_DEBUG_PKTPATH
        char         pkt_path[128];
        char         *pkt_path_p;
        #endif
******   Skip area   ******
******   Packet data area   ******
******   Third party Application buffer area   ******
******   Application Scratch area   ******
        PAD_CACHE_LINE;
}CACHE_ALIGN ;

\endverbatim

\n<b> Fields: </b>\n

<i>   Neighbour - </i> pointer to the next PSPBuf in a Queue
\n
<i> sg - </i> Scatter Gather Nodes Information
\n
<i>  sg.pPrev - </i>     Previous SG node in SG chain
\n
<i>  sg.pNext - </i>     Next SG node in SG chain
\n
<i>  sg.pHead - </i>     Pointer to the  PSPBuf representing the packet.Valid only in SG PSPBufs
\n
<i>  sg.remNodes - </i>  Number of SG Nodes in the SG chain excluding the current node
\n
<i>  sg.remLen - </i>    Length of data in SG nodes other than current PSPBuf
\n


<i> frag  </i> IP Fragments Information
\n
<i>  frag.next - </i> Next Fragment
\n
<i>  frag.prev - </i> Previous Fragment
\n
<i>  frag.head - </i> Head of Fragment Chain
\n
<i>  frag.total_len - </i> Total length of the packet(  including all fragments)
\n

<i> pSoD - </i> Start Of packet Data area
\n
<i> Data - </i> Current reference to packet data
\n
<i> count - </i> Number of valid bytes of packet data from current data reference
\n
<i> orig_rx_iface_id - </i> Original interface on which packet is received
\n
<i> rxIfaceId - </i> Interface on which packet is received
\n
<i> txIfaceId - </i> interface on which packet is to be transmitted
\n
<i> uiGatewayIP - </i> Gateway IP
\n
<i> uiNSId - </i> Namespace ID
<i> uiSrcZone - </i> Source Network zone
<i> uiDstZone - </i> Destination  Network zone
<i> time_stamp -  </i> 64 bit Receive Timestamp of packet
<i> uiFlags - </i> Flags
<i> usL2Proto - </i> L2 Protocol
<i> ucL3Proto - </i> L3 Protocol
<i> ucPktType - </i> Packet type: unicast/multicast/broadcast
<i> rx_vlan_info - </i> Received VLAN ID and Qos bit as seen in VLAN header.
<i> usTxVlanInfo - </i> Transmit VLAN ID and Qos bits
<i> MacHdr - </i>     Data reference to ethernet MAC header
<i> NetworkHdr - </i> Data reference to Network header
<i> transport_hdr - </i> Data reference to Transport header

<i> nw_tuple - </i> Network layer protocol tuples
<i> ip4.src - </i> IPv4 source address
<i> ip4.dst - </i> IPv4 destination address
<i> ip4.ulTotLen </i> IPv4 packet total length
<i> ip4.ucToS </i> IPv4 packet ToS value

<i> xport_tuple - </i> Transport layer protocol tuples
<i> val - </i>
<i> spi - </i> IPSEC SPI value
<i> code - </i> ICMP code
<i> id - </i> IP packet Identification
<i> srcPort - </i>
<i> dstPort - </i>
<i> uiQosPrio - </i> Qos Priority of this packet

<i> uiMaxDataSize - </i> Maximum size of packet data in this PSPBuf
<i> uiThreadId - </i> Index of worker thread  processing the packet
<i> pParent - </i> pointer to Parent buffer (valid if this buffer is a clone)
<i> uiDataRefCnt - </i> Reference count to PSPBuf data
<i> DataRefLock - </i> Mutex to protect Data reference count
<i> bCloned - </i> Indicates if this buffer is cloned
<i> pBufPool - </i> PSP Buffer pool
<i> uiBufPhyAddr - </i> Physical address of PSPBuf
<i> uiSoDPhyAddr - </i>  Physical address of packet data area

<i> uiSessionHash64 - </i> 64 bit session hash value
<i> uiSessionHashHi - </i> higher 32 bites of session hash value
<i> uiSessionHash - </i> lower 32 bits of session hash value

<i> PSPRtCacheRef </i> Reference to Route Cache
<i> uiAppFlags - </i> Flags for applications use

*/

struct pkt_mbuf
{
        struct pkt_mbuf                 *pPrev;
        struct pkt_mbuf                 *pNext;

        uchar8_t                        *start_of_data;
        struct pkt_mbuf_data_ref        data;
        struct pkt_mbuf_data_ref        end;
        uint32_t                        count;

        uint8_t  of_app_type; /* enum of_appl_type */
        uint8_t  of_app_subtype; /* flags is to used by the application */
        int32_t  os_if_index; 

        uint32_t                        data_ref_cnt;
        PSPLock_t                       data_ref_lock;

        void                          (*freertn)(void*);
        void                          *arg;

        uint64_t                        time_stamp;

        struct safe_ref                 orig_rx_iface_id;
        struct safe_ref                 rx_iface_id;
        uint32_t                        ns_id;
        uint16_t                        rx_vlan_info;

        uint16_t                        l2_proto;
        uint16_t                        ex_l2_proto;
        uchar8_t                        l3_proto;
        struct pkt_mbuf_data_ref        mac_hdr;
        struct pkt_mbuf_data_ref        network_hdr;
        struct pkt_mbuf_data_ref        transport_hdr;
        union
        {
                struct
                {
                        uint32_t        src;
                        uint32_t        dst;
                        uint32_t        total_len;
                        uchar8_t        tos;
                        struct
                        {
                                uint32_t        next_hop;
                                uchar8_t        opt_len;
                                uchar8_t        srr;
                                uchar8_t        rr;
                                uchar8_t        ts;
                                uchar8_t        is_strictroute:1,
                                                srr_is_hit:1,
                                                is_changed:1,
                                                rr_needaddr:1,
                                                ts_needtime:1,
                                                ts_needaddr:1;
                        }ip_opts;
                }ip4;
        #ifdef PSP_IPV6_SUPPORT
                struct
                {
                        PSPIPv6Addr_t   src;
                        PSPIPv6Addr_t   dst;
                        uint32_t        payload_len; /*to hold jumbo frame length*/
                        uint32_t        flow_info;
                        uchar8_t        transport_proto;
                        uchar8_t        traffic_class;
                        struct
                        {
                                uint32_t        hop_by_hop;
                                uint32_t        route;
                                uint32_t        dest0;
                                uint32_t        dest1;
                                uint32_t        fragment;
                                uint32_t        transport;
                                uint32_t        num_ext_hdr;
                                uint32_t        last_opt;
                                uint32_t        nh_offset;
                                uint32_t        ext_hdrs_len;
                                uint32_t        ra_offset;
                                uint32_t        unfrag_len;
                                uint32_t        next_proto;
                                uint32_t        flags;
                        #define PKT_MBUF_IPV6_EXTHDR_PROCESSED          PSP_BIT0
                        #define PKT_MBUF_IPV6_EXTHDR_HOP_BY_HOP         PSP_BIT1
                        #define PKT_MBUF_IPV6_EXTHDR_DEST0              PSP_BIT2
                        #define PKT_MBUF_IPV6_EXTHDR_DEST1              PSP_BIT3
                        #define PKT_MBUF_IPV6_EXTHDR_ROUTING            PSP_BIT4
                        #define PKT_MBUF_IPV6_EXTHDR_HAO_PRESENT        PSP_BIT5
                        #define PKT_MBUF_IPV6_EXTHDR_FRAG               PSP_BIT6
                        #define PKT_MBUF_IPV6_JUMBOPAYLOAD              PSP_BIT7
                        #define PKT_MBUF_IPV6_PROC_BEFORE_DEFRAG        PSP_BIT8
                        }params;
                }ip6;
        #endif /*PSP_IPV6_SUPPORT*/
        }nw_tuple;

        union
        {
                uint32_t        val;
                uint32_t        spi;
                uint32_t        id;
                struct
                {
                        uint32_t        icmp_type:8;
                        uint32_t        icmp_code:8;
                };
                struct
                {
                        uint32_t        mh_type:8;
                        uint32_t        mhn_ext_proto:8;
                };
                struct
                {
                        uint32_t        src_port:16;
                        uint32_t        dst_port:16;
                };
        }xport_tuple;

        struct
        {
                struct pkt_mbuf *next;
                struct pkt_mbuf *prev;
                struct pkt_mbuf *head;
                uint32_t        total_len;
        }frag;

        uint32_t                flags;
        uchar8_t                pkt_type;
        uint32_t                gateway_ip;
        uint32_t                path_mtu;
        struct safe_ref         tx_iface_id;
        uint16_t                tx_vlan_info;
        uint32_t                qos_prio;
        uint32_t                max_data_size;
        struct pkt_mbuf         *parent;
        bool8_t                 cloned;
        void                  *buf_pool;
        t_phyaddr               buf_phy_addr;
        uint32_t                port_num;
        t_phyaddr               sod_phy_addr;

        union
        {
                struct safe_ref route_cache_ref;
                struct safe_ref route6_cache_ref;
        };
        uint32_t        route_cache_magic;
        struct
        {
                struct pkt_mbuf         *prev;
                struct pkt_mbuf         *next;
                struct pkt_mbuf         *head;
                uint32_t                rem_nodes;
                uint32_t                rem_len;
        }sg;

        #ifdef CONFIG_PKT_MBUF_DEBUG_PKTPATH
        char         pkt_path[128];
        char         *pkt_path_p;
        #endif
/******   Skip area   ******/
/******   Packet data area   ******/
/******   Third party Application buffer area   ******/
/******   Application Scratch area   ******/
        PAD_CACHE_LINE;
}CACHE_ALIGN;

struct pkt_mbuf_conf
{
        uint32_t        hw_area_size;
        uint32_t        pkt_prepend_size;
        uint32_t        pkt_append_size;
        uint32_t        appl_scratch_area_size;
        uint32_t        third_party_app_buf_area_size;
        bool8_t         pool_created;
};

struct pkt_mbuf_pool_conf
{    
        uint32_t        data_size; /* Maximum size of packet data area */
        uint32_t        pool_size; /* Total Number of buffers in the pool */
};

struct pkt_mbuf_pool
{
        uint32_t        data_size; /* Maximum size of packet data area */
        uint32_t        buf_size;  /* Size of PSPBuf.
                                    * Includes PSPBuf header, packet data,
                                    *  skip area, application scratch area and
                                    * Third Party App area
                                    */
        uint32_t        num_bufs; /* Total Number of buffers in the pool */
        void          *mem_pool; /* pointer to the Memory pool of PSPBufs*/
        struct pkt_mbuf_pool *next; /* Pointer to the next PSPBuf pool */
};

#ifndef __KERNEL__
extern struct pkt_mbuf_conf  pkt_mbuf_conf_g;
#endif


/*************** Macros ***************/


/** \ingroup PBUFMACROS
*/

/* This macro reads 16 bit value using the data pointer.*/
#define pkt_mbuf_get_16bits(cp)                \
  (((uint16_t)(*(char *)(cp)) << 8 & 0xFF00) | (*((char *)(cp) + 1) & 0xFF))


/* This macro reads 32 bit value using the data pointer.*/
#define pkt_mbuf_get_32bits(cp)                                    \
  ((((uint32_t)(*((char *) (cp))    ) << 24 ) & 0xFF000000 ) |       \
   (((uint32_t)(*((char *) (cp) + 1)) << 16 ) & 0x00FF0000 ) |       \
   (((uint32_t)(*((char *) (cp) + 2)) << 8  ) & 0x0000FF00 ) |       \
   ((uint32_t)(*((char *) (cp) + 3))         & 0x000000FF ))

/* This macro reads 64 bit value using the data pointer.*/
#define pkt_mbuf_get_64bits(cp)                                    \
  ((((uint64_t)(*((char *) (cp))    ) << 56 ) & 0xFF00000000000000) |      \
   (((uint64_t)(*((char *) (cp) + 1)) << 48 ) & 0x00FF000000000000) |      \
   (((uint64_t)(*((char *) (cp) + 2)) << 40 ) & 0x0000FF0000000000) |      \
   (((uint64_t)(*((char *) (cp) + 3)) << 32 ) & 0x000000FF00000000) |      \
   (((uint64_t)(*((char *) (cp) + 4)) << 24 ) & 0x00000000FF000000) |      \
   (((uint64_t)(*((char *) (cp) + 5)) << 16 ) & 0x0000000000FF0000) |      \
   (((uint64_t)(*((char *) (cp) + 6)) << 8  ) & 0x000000000000FF00) |      \
   ((uint64_t)(*((char *)  (cp) + 7))         & 0x00000000000000FF))

/* This macro writes 16 bit value using the data pointer. */
#define pkt_mbuf_put_16bits(cp, val)                          \
{                                                  \
  *((char *) (cp)    ) = (char) ((val) >> 8) & 0xFF; \
  *((char *) (cp) + 1) = (char) ((val) & 0xFF);        \
}

/* This macro writes 32 bit value using the data pointer.*/
#define pkt_mbuf_put_32bits(cp, val)                               \
{                                                       \
  *((char *) (cp))     = (char) ((val) >> 24 ) & 0xFF;  \
  *((char *) (cp) + 1) = (char) ((val) >> 16 ) & 0xFF;  \
  *((char *) (cp) + 2) = (char) ((val) >> 8  ) & 0xFF;  \
  *((char *) (cp) + 3) = (char)  (val)         & 0xFF;  \
}

/* This macro writes 64 bit value using the data pointer.*/
#define pkt_mbuf_put_64bits(cp, val)                               \
{                                                       \
  *((char *) (cp))     = (char) ((val) >> 56 ) & 0xFF;  \
  *((char *) (cp) + 1) = (char) ((val) >> 48 ) & 0xFF;  \
  *((char *) (cp) + 2) = (char) ((val) >> 40 ) & 0xFF;  \
  *((char *) (cp) + 3) = (char) ((val) >> 32 ) & 0xFF;  \
  *((char *) (cp) + 4) = (char) ((val) >> 24 ) & 0xFF;  \
  *((char *) (cp) + 5) = (char) ((val) >> 16 ) & 0xFF;  \
  *((char *) (cp) + 6) = (char) ((val) >> 8  ) & 0xFF;  \
  *((char *) (cp) + 7) = (char)  (val)         & 0xFF;  \
}


#ifndef __KERNEL__
/** \ingroup PBUFMACROS
#define pkt_mbuf_hw_area_size()           (pkt_mbuf_conf_g.hw_area_size)

\n<b>Description:</b>\n
 Size of Hardware area in PSPBuf
*/
#define pkt_mbuf_hw_area_size()           (pkt_mbuf_conf_g.hw_area_size)

/** \ingroup PBUFMACROS
#define pkt_mbuf_prepend_size()       (pkt_mbuf_conf_g.pkt_prepend_size)
 \n<b>Description:</b>\n
Size of Packet Prepend area in PSPBuf
*/
#define pkt_mbuf_prepend_size()       (pkt_mbuf_conf_g.pkt_prepend_size)


/** \ingroup PBUFMACROS
#define pkt_mbuf_append_size()        (pkt_mbuf_conf_g.pkt_append_size)
 \n<b>Description:</b>\n
Size of Packet Append area in PSPBuf
*/
#define pkt_mbuf_append_size()        (pkt_mbuf_conf_g.pkt_append_size)

/** \ingroup PBUFMACROS
#define pkt_mbuf_appl_scratch_area_size() (pkt_mbuf_conf_g.appl_scratch_area_size)
 \n<b>Description:</b>\n
Size of application scratch area in PSPBuf
*/
#define pkt_mbuf_appl_scratch_area_size() (pkt_mbuf_conf_g.appl_scratch_area_size)


/** \ingroup PBUFMACROS
#define pkt_mbuf_third_party_appl_area_size()      (pkt_mbuf_conf_g.third_party_app_buf_area_size)
  \n<b>Description:</b>\n
 Size of third party application area
*/
#define pkt_mbuf_third_party_appl_area_size()      (pkt_mbuf_conf_g.third_party_app_buf_area_size)


/** \ingroup PBUFMACROS
#define pkt_mbuf_hw_area_ptr(mbuf) ((uchar8_t*) ((uint32_t)(mbuf) - pkt_mbuf_hw_area_size()))
 \n<b>Description:</b>\n
 Pointer to Hardware area of PSPBuf
*/
#define pkt_mbuf_hw_area_ptr(mbuf) ((uchar8_t*) ((uint32_t)(mbuf) - pkt_mbuf_hw_area_size()))

#else

#define pkt_mbuf_hw_area_size()           (0)

#endif /* __KERNEL__ */

/** \ingroup PBUFMACROS
#define pkt_mbuf_hw_area_phy_addr(mbuf) ((uint32_t)mbuf->buf_phy_addr)
 \n<b>Description:</b>\n
 Physical address of Hardware area
*/
#define pkt_mbuf_hw_area_phy_addr(mbuf) ((uint32_t)mbuf->buf_phy_addr)


/** \ingroup PBUFMACROS
#define pkt_mbuf_phy_addr(mbuf) (((uint32_t)(mbuf->buf_phy_addr) + pkt_mbuf_hw_area_size()))
 \n<b>Description:</b>\n
 Physical address of PSP Buffer
*/
#define pkt_mbuf_phy_addr(mbuf) (((uint32_t)(mbuf->buf_phy_addr) + pkt_mbuf_hw_area_size()))


/** \ingroup PBUFMACROS
#define pkt_mbuf_set_sod_phy_addr(mbuf)  mbuf->sod_phy_addr = ( (uint32_t)mbuf->buf_phy_addr +  pkt_mbuf_hw_area_size() + sizeof(struct pkt_mbuf) );
 \n<b>Description:</b>\n
 Set Physical address of SoD (start of packet data area)
*/
#define pkt_mbuf_set_sod_phy_addr(mbuf)  mbuf->sod_phy_addr = ( (uint32_t)mbuf->buf_phy_addr +  pkt_mbuf_hw_area_size() + sizeof(struct pkt_mbuf) );


/** \ingroup PBUFMACROS
#define pkt_mbuf_get_pkt_phy_addr(mbuf)  mbuf->sod_phy_addr
 \n<b>Description:</b>\n
 Physical address of SoD (start of packet data area)
*/
#define pkt_mbuf_get_pkt_phy_addr(mbuf)  (mbuf->sod_phy_addr)


/* \ingroup PBUFMACROS
#define pkt_mbuf_skip_area_ptr(mbuf)  ( (uchar8_t*) ((uint32_t)(mbuf) + sizeof(struct pkt_mbuf)) )
 \n<b>Description:</b>\n
*/
#define pkt_mbuf_skip_area_ptr(mbuf)  ( (uchar8_t*) ((uint32_t)(mbuf) + sizeof(struct pkt_mbuf)) )

/** \ingroup PBUFMACROS
#define pkt_mbuf_appl_scratch_area_ptr(mbuf)  \
                (pkt_mbuf_sod_ptr(mbuf) + (mbuf->max_data_size) + \
                 pkt_mbuf_prepend_size() + \
                 pkt_mbuf_append_size())
 \n<b>Description:</b>\n
 Pointer to the Applicaton scratch area
*/
#define pkt_mbuf_appl_scratch_area_ptr(mbuf)  \
                (pkt_mbuf_sod_ptr(mbuf) + (mbuf->max_data_size) + \
                 pkt_mbuf_prepend_size() + \
                 pkt_mbuf_append_size())

/** \ingroup PBUFMACROS
#define pkt_mbuf_third_party_appl_area_ptr(mbuf) \
              (pkt_mbuf_appl_scratch_area_ptr(mbuf) +\
               pkt_mbuf_appl_scratch_area_size())
 \n<b>Description:</b>\n
 Pointer to the Third party applicaton area
*/
#define pkt_mbuf_third_party_appl_area_ptr(mbuf) \
              (pkt_mbuf_appl_scratch_area_ptr(mbuf) +\
               pkt_mbuf_appl_scratch_area_size())

/** \ingroup PBUFMACROS
#define pkt_mbuf_sg_rem_len(mbuf)     ((mbuf)->sg.rem_len)
 \n<b>Description:</b>\n
 Remaining length in Scatter Gather nodes
*/
#define pkt_mbuf_sg_rem_len(mbuf)     ((mbuf)->sg.rem_len)


/** \ingroup PBUFMACROS
#define pkt_mbuf_sg_rem_nodes(mbuf)   ((mbuf)->sg.rem_nodes)
 \n<b>Description:</b>\n
 Remaining Scatter Gather nodes
*/
#define pkt_mbuf_sg_rem_nodes(mbuf)   ((mbuf)->sg.rem_nodes)


/** \ingroup PBUFMACROS
#define pkt_mbuf_sg_prev(mbuf)       ((mbuf)->sg.prev)
 \n<b>Description:</b>\n
 Previous node in Scatter Gather chain
*/
#define pkt_mbuf_sg_prev(mbuf)       ((mbuf)->sg.prev)


/** \ingroup PBUFMACROS
#define pkt_mbuf_sg_next(mbuf)       ((mbuf)->sg.next)
 \n<b>Description:</b>\n
 Next node in Scatter Gather chain
*/
#define pkt_mbuf_sg_next(mbuf)       ((mbuf)->sg.next)


/** \ingroup PBUFMACROS
#define pkt_mbuf_sg_head(mbuf)       ((mbuf)->sg.head)
 \n<b>Description:</b>\n
 Head node in Scatter Gather chain
*/
#define pkt_mbuf_sg_head(mbuf)       ((mbuf)->sg.head)


/** \ingroup PBUFMACROS
#define pkt_mbuf_sg_is_head(mbuf) \
    (mbuf->sg.head == mbuf)
 \n<b>Description:</b>\n
 Checks if this buffer is a Head buffer in Scatter Gather chain
*/
#define pkt_mbuf_sg_is_head(mbuf) \
    (mbuf->sg.head == mbuf)

/** \ingroup PBUFMACROS
#define pkt_mbuf_frag_prev(mbuf)      ((mbuf)->frag.prev)
 \n<b>Description:</b>\n
 Previous buffer in IP Fragment chain
*/
#define pkt_mbuf_frag_prev(mbuf)      ((mbuf)->frag.prev)


/** \ingroup PBUFMACROS
#define pkt_mbuf_frag_next(mbuf)      ((mbuf)->frag.next)
 \n<b>Description:</b>\n
 Next buffer in IP Fragment chain
*/
#define pkt_mbuf_frag_next(mbuf)      ((mbuf)->frag.next)


/** \ingroup PBUFMACROS
#define pkt_mbuf_frag_head(mbuf)      ((mbuf)->frag.head)
 \n<b>Description:</b>\n
 Head buffer in IP Fragment chain
*/
#define pkt_mbuf_frag_head(mbuf)      ((mbuf)->frag.head)


/** \ingroup PBUFMACROS
#define pkt_mbuf_frag_total_pkt_len(mbuf) ((mbuf)->frag.total_len)
 \n<b>Description:</b>\n
 Total packet length of fragmented IP packet
*/
#define pkt_mbuf_frag_total_pkt_len(mbuf) ((mbuf)->frag.total_len)


/** \ingroup PBUFMACROS
#define pkt_mbuf_is_fragmented_pkt(mbuf) (pkt_mbuf_frag_next(mbuf) != NULL)
 \n<b>Description:</b>\n
 Checks if the given packet is an IP fragmented packet.
 This macro should only be used with the first fragment in the reassembled buffer
*/
#define pkt_mbuf_is_fragmented_pkt(mbuf) (pkt_mbuf_frag_next(mbuf) != NULL)


/** \ingroup PBUFMACROS
#define pkt_mbuf_count_pkt_bytes(mbuf)           ((mbuf)->count)
\n<b>Description:</b>\n
 Count of valid packet bytes in current buffer from current data pointer
*/
#define pkt_mbuf_count_pkt_bytes(mbuf)           ((mbuf)->count)


/** \ingroup PBUFMACROS
#define pkt_mbuf_pkt_len(mbuf)\
  ((pkt_mbuf_frag_total_pkt_len(mbuf))?(pkt_mbuf_frag_total_pkt_len(mbuf)):(pkt_mbuf_data_len(mbuf)))
Count of valid packet bytes in scatter gather buffer from current data pointer
*/
#define pkt_mbuf_pkt_len(mbuf)\
  ((pkt_mbuf_frag_total_pkt_len(mbuf))?(pkt_mbuf_frag_total_pkt_len(mbuf)):(pkt_mbuf_data_len(mbuf)))

/** \ingroup PBUFMACROS
#define pkt_mbuf_data_len(mbuf)        ( pkt_mbuf_count_pkt_bytes(mbuf)  + pkt_mbuf_sg_rem_len(mbuf) )
Count of valid packet bytes in the current fragment
*/
#define pkt_mbuf_data_len(mbuf)        ( pkt_mbuf_count_pkt_bytes(mbuf)  + pkt_mbuf_sg_rem_len(mbuf) )

/** \ingroup PBUFMACROS
#define pkt_mbuf_max_data_size(mbuf)   ((mbuf)->max_data_size)
\n<b>Description:</b>\n
 Maximum data that can be stored in this buffer
*/
#define pkt_mbuf_max_data_size(mbuf)   ((mbuf)->max_data_size)


/** \ingroup PBUFMACROS
#define pkt_mbuf_sod_ptr(mbuf) ( pkt_mbuf_skip_area_ptr(mbuf) )
\n <b>Description:</b>\n
 pointer to the Start of packet data
*/
#define pkt_mbuf_sod_ptr(mbuf) ( pkt_mbuf_skip_area_ptr(mbuf) )

#ifdef NOT_NEEDED
/** \ingroup PBUFMACROS
#define PSPBUF_PKT_PREPEND_LEN(mbuf)       (mbuf->data.ptr - mbuf->start_of_data)
*/
#define PSPBUF_PKT_PREPEND_LEN(mbuf)       (mbuf->data.ptr - mbuf->start_of_data)

/** \ingroup PBUFMACROS
#define PSPBUF_PKT_APPEND_LEN(mbuf)    ( (mbuf->start_of_data+mbuf->uiDataSize ) -  (mbuf->data.ptr + mbuf->count) )
*/
#define PSPBUF_PKT_APPEND_LEN(mbuf)    ( (mbuf->start_of_data+mbuf->uiDataSize ) -  (mbuf->data.ptr + mbuf->count) )
#endif

/** \ingroup PBUFMACROS
#define pkt_mbuf_rxiface_id(mbuf)     ((mbuf)->rx_iface_id)
\n <b>Description:</b>\n
 Receive interface ID
*/
#define pkt_mbuf_rxiface_id(mbuf)     ((mbuf)->rx_iface_id)

/** \ingroup PBUFMACROS
#define pkt_mbuf_txiface_id(mbuf)     ((mbuf)->tx_iface_id)
\n <b>Description:</b>\n
 Transmit interface ID
*/
#define pkt_mbuf_txiface_id(mbuf)     ((mbuf)->tx_iface_id)

/** \ingroup PBUFMACROS
#define pkt_mbuf_ns_id(mbuf)          ((mbuf)->ns_id)
\n <b>Description:</b>\n
 Network Namespace ID
*/
#define pkt_mbuf_ns_id(mbuf)          ((mbuf)->ns_id)

/** \ingroup PBUFMACROS
#define pkt_mbuf_rx_vlanid(mbuf)      ((mbuf)->rx_vlan_info&PSP_VLAN_ID_MASK)
\n<b>Description:</b>\n
 Receive VLAN ID
*/
#define pkt_mbuf_rx_vlanid(mbuf)      ((mbuf)->rx_vlan_info&PSP_VLAN_ID_MASK)

/** \ingroup PBUFMACROS
#define pkt_mbuf_tx_vlanid(mbuf)      ((mbuf)->tx_vlan_info&PSP_VLAN_ID_MASK)
 \n<b>Description:</b>\n
 Transmit VLAN ID
*/
#define pkt_mbuf_tx_vlanid(mbuf)      ((mbuf)->tx_vlan_info&PSP_VLAN_ID_MASK)

/** \ingroup PBUFMACROS
#define PSPBUF_SET_RXVLANID(mbuf,id) do { (mbuf)->usRxVlanID = id; } while(0)
\n<b>Description:</b>\n
 Set Receive VLAN ID
*/
#define pkt_mbuf_set_rx_vlan_info(mbuf,info) do { (mbuf)->rx_vlan_info = info; } while(0)

/** \ingroup PBUFMACROS
#define pkt_mbuf_set_tx_vlanid(mbuf,id) do { (mbuf)->tx_vlan_info |= (id&PSP_VLAN_ID_MASK); } while(0)
\n<b>Description:</b>\n
Set Transmit VLAN ID. Valid only if outgoing interface supports native VLAN tagging. Thsi macro retains Qos bits.
*/
#define pkt_mbuf_set_tx_vlanid(mbuf,id) do { (mbuf)->tx_vlan_info |= (id&PSP_VLAN_ID_MASK); } while(0)

/** \ingroup PBUFMACROS
#define pkt_mbuf_set_tx_vlan_info(mbuf,info) do { (mbuf)->tx_vlan_info = info; } while(0)
\n<b>Description:</b>\n
Set Transmit VLAN ID and Qos bits. Valid only if outgoing interface supports native VLAN tagging.
*/
#define pkt_mbuf_set_tx_vlan_info(mbuf,info) do { (mbuf)->tx_vlan_info = info; } while(0)

/** \ingroup PBUFMACROS
#define pkt_mbuf_set_rxiface_id(mbuf,id) do { (mbuf)->rx_iface_id = id; } while(0)
\n<b>Description:</b>\n
 Set Receive interface ID
*/
#define pkt_mbuf_set_rxiface_id(mbuf,id) do { (mbuf)->rx_iface_id = id; } while(0)

/** \ingroup PBUFMACROS
#define pkt_mbuf_set_txiface_id(mbuf,id) do { (mbuf)->tx_iface_id = id; } while(0)
\n<b>Description:</b>\n
 Set Transmit interface ID
*/
#define pkt_mbuf_set_txiface_id(mbuf,id) do { (mbuf)->tx_iface_id = id; } while(0)

/** \ingroup PBUFMACROS
#define pkt_mbuf_set_rx_iface(mbuf,iface) do { (mbuf)->rx_iface_id = PSPIfaceId(iface); } while(0)
Set Receive interface ID
*/
#define pkt_mbuf_set_rx_iface(mbuf,iface) do { (mbuf)->rx_iface_id = PSPIfaceId(iface); } while(0)

/** \ingroup PBUFMACROS
#define pkt_mbuf_set_tx_iface(mbuf,iface) do { (mbuf)->tx_iface_id = PSPIfaceId(iface); } while(0)
Set Receive interface ID
*/
#define pkt_mbuf_set_tx_iface(mbuf,iface) do { (mbuf)->tx_iface_id = PSPIfaceId(iface); } while(0)

/** \ingroup PBUFMACROS
#define pkt_mbuf_mac_hdr(mbuf)     ((mbuf)->mac_hdr)
\n<b>Description:</b>\n
 Data reference to L2 header in packet
*/
#define pkt_mbuf_mac_hdr(mbuf)     ((mbuf)->mac_hdr)

/** \ingroup PBUFMACROS
#define pkt_mbuf_nw_hdr(mbuf)      ((mbuf)->network_hdr)
\n<b>Description:</b>\n
 Data reference to L3 header in packet
*/
#define pkt_mbuf_nw_hdr(mbuf)      ((mbuf)->network_hdr)

/** \ingroup PBUFMACROS
#define pkt_mbuf_xport_hdr(mbuf)   ( (mbuf)->transport_hdr)
\n<b>Description:</b>\n
 Data reference to transport header in packet
*/
#define pkt_mbuf_xport_hdr(mbuf)   ( (mbuf)->transport_hdr)

/** \ingroup PBUFMACROS
#define pkt_mbuf_mac_hdr_ptr(mbuf)     ((mbuf)->mac_hdr.ptr)
\n<b>Description:</b>\n
 Pointer to L2 header in packet
*/
#define pkt_mbuf_mac_hdr_ptr(mbuf)     ((mbuf)->mac_hdr.ptr)

/** \ingroup PBUFMACROS
#define pkt_mbuf_nw_hdr_ptr(mbuf)      ((mbuf)->network_hdr.ptr)
\n<b>Description:</b>\n
 Pointer to L3 header in packet
*/
#define pkt_mbuf_nw_hdr_ptr(mbuf)      ((mbuf)->network_hdr.ptr)

/** \ingroup PBUFMACROS
#define pkt_mbuf_xport_hdr_ptr(mbuf)   ( (mbuf)->transport_hdr.ptr)
\n<b>Description:</b>\n
 Pointer to Transport header in packet
*/
#define pkt_mbuf_xport_hdr_ptr(mbuf)   ( (mbuf)->transport_hdr.ptr)




/* #define PSPBUF_DST_NSID(mbuf)      ((mbuf)->uiDstNsid) */

/** \ingroup PBUFMACROS
#define pkt_mbuf_src_ip(mbuf)           ((mbuf)->nw_tuple.ip4.src)
\n<b>Description:</b>\n
 Source Ip address
*/
#define pkt_mbuf_src_ip(mbuf)           ((mbuf)->nw_tuple.ip4.src)


/** \ingroup PBUFMACROS
#define pkt_mbuf_dst_ip(mbuf)           ((mbuf)->nw_tuple.ip4.dst)
\n<b>Description:</b>\n
 Destination  Ip address
*/
#define pkt_mbuf_dst_ip(mbuf)           ((mbuf)->nw_tuple.ip4.dst)

/** \ingroup PBUFMACROS
#define pkt_mbuf_total_len(mbuf)           ((mbuf)->nw_tuple.ip4.total_len)
\n<b>Description:</b>\n
 Total packet length of ip packet
*/
#define pkt_mbuf_total_len(mbuf)           ((mbuf)->nw_tuple.ip4.total_len)


/** \ingroup PBUFMACROS
#define pkt_mbuf_src_port(mbuf)       ((mbuf)->xport_tuple.src_port)
\n<b>Description:</b>\n
 Source port
*/
#define pkt_mbuf_src_port(mbuf)       ((mbuf)->xport_tuple.src_port)

/** \ingroup PBUFMACROS
#define pkt_mbuf_dst_port(mbuf)       ((mbuf)->xport_tuple.dst_port)
\n<b>Description:</b>\n
 Destination port
*/
#define pkt_mbuf_dst_port(mbuf)       ((mbuf)->xport_tuple.dst_port)

/** \ingroup PBUFMACROS
#define pkt_mbuf_spi(mbuf)           ((mbuf)->xport_tuple.spi)
\n<b>Description:</b>\n
 IPSEC SPI value
*/
#define pkt_mbuf_spi(mbuf)           ((mbuf)->xport_tuple.spi)

/** \ingroup PBUFMACROS
#define pkt_mbuf_icmp_type(mbuf)      ((mbuf)->xport_tuple.icmptype)
\n<b>Description:</b>\n
 ICMP Type
*/
#define pkt_mbuf_icmp_type(mbuf)      ((mbuf)->xport_tuple.icmptype)

/** \ingroup PBUFMACROS
#define pkt_mbuf_icmp_code(mbuf)      ((mbuf)->-xport_tuple.icmp_code)
\n<b>Description:</b>\n
 ICMP code
*/
#define pkt_mbuf_icmp_code(mbuf)      ((mbuf)->xport_tuple.icmp_code)

/** \ingroup PBUFMACROS
#define pkt_mbuf_icmp_id(mbuf)          ((mbuf)->xport_tuple.id)
\n<b>Description:</b>\n
 ICMP ID
*/
#define pkt_mbuf_icmp_id(mbuf)          ((mbuf)->xport_tuple.id)

/** \ingroup PBUFMACROS
#define pkt_mbuf_is_sg_valid(mbuf)       ((mbuf)->sg.rem_nodes != 0)
\n<b>Description:</b>\n
 Is Scatter Gather valid for this buffer ?
*/
#define pkt_mbuf_is_sg_valid(mbuf)       ((mbuf)->sg.rem_nodes != 0)



/** \ingroup PBUFMACROS
#define pkt_mbuf_ipv4_opt_strict_route(mbuf)  ((mbuf)->nw_tuple.ip4.ip_opts.is_strictroute)
\n<b>Description:</b>\n
 Checks if strict route option is present in the ipv4 options
*/
#define pkt_mbuf_ipv4_opt_strict_route(mbuf)  ((mbuf)->nw_tuple.ip4.ip_opts.is_strictroute)

/** \ingroup PBUFMACROS
*/
// Venu separate the flags into those:
// a) stored in pspbuf and
// b) passed during create/alloc/free of pspbuf
/* Following macros define PSPBuf flags stored in 'ulFlags' member */
/** \ingroup PBUFFLAGS
#define PKT_MBUF_NW_INFO_VALID             PSP_BIT0
\n<b>Description:</b>\n
 Is Network tuple info in PSP buffer valid
*/
#define PKT_MBUF_NW_INFO_VALID             PSP_BIT0

/** \ingroup PBUFFLAGS
#define PKT_MBUF_XPORT_INFO_VALID          PSP_BIT1
\n<b>Description:</b>\n
 Transport tuple info in PSP buffer is valid
*/
#define PKT_MBUF_XPORT_INFO_VALID          PSP_BIT1

/** \ingroup PBUFFLAGS
#define PKT_MBUF_SNET_INFO_VALID           PSP_BIT2
\n<b>Description:</b>\n
 SNET info in PSP buffer is valid
*/
#define PKT_MBUF_SNET_INFO_VALID           PSP_BIT2

/** \ingroup PBUFFLAGS
#define PKT_MBUF_VLAN_INFO_VALID           PSP_BIT3
\n<b>Description:</b>\n
 VLAN info in PSP buffer is valid
*/
#define PKT_MBUF_VLAN_INFO_VALID           PSP_BIT3

/** \ingroup PBUFFLAGS
#define PKT_MBUF_IP_CHKSUM_VERIFIED      PSP_BIT4
\n<b>Description:</b>\n
 IP checksum is already verified
*/
#define PKT_MBUF_IP_CHKSUM_VERIFIED      PSP_BIT4

/** \ingroup PBUFFLAGS
#define PKT_MBUF_IP_CHKSUM_NEEDED        PSP_BIT5
\n<b>Description:</b>\n
 IP checksum needs to be calculated
*/
#define PKT_MBUF_IP_CHKSUM_NEEDED        PSP_BIT5

/** \ingroup PBUFFLAGS
#define PKT_MBUF_IP_CHKSUM_INVALID       PSP_BIT6
\n<b>Description:</b>\n
 IP checksum is invalid
*/
#define PKT_MBUF_IP_CHKSUM_INVALID       PSP_BIT6

/** \ingroup PBUFFLAGS
#define PKT_MBUF_XPORT_CHKSUM_VERIFIED   PSP_BIT7
\n<b>Description:</b>\n
 Transport checksum is already verified
*/
#define PKT_MBUF_XPORT_CHKSUM_VERIFIED   PSP_BIT7

/** \ingroup PBUFFLAGS
#define PSPBUF_XPORT_CHECKSUM_NEEDED     PSP_BIT8
\n<b>Description:</b>\n
 Transport checksum needs to be calculated
*/
#define PSPBUF_XPORT_CHECKSUM_NEEDED     PSP_BIT8

/** \ingroup PBUFFLAGS
#define PKT_MBUF_XPORT_CHKSUM_INVALID    PSP_BIT9
\n<b>Description:</b>\n
 Transport checksum is invalid
*/
#define PKT_MBUF_XPORT_CHKSUM_INVALID    PSP_BIT9

/** \ingroup PBUFFLAGS
#define PKT_MBUF_SESSION_HASH_VALID        PSP_BIT10
\n<b>Description:</b>\n
 Session hash is valid
*/
#define PKT_MBUF_SESSION_HASH_VALID        PSP_BIT10

/** \ingroup PBUFFLAGS
#define PKT_MBUF_RETRANS_TCP_PKT           PSP_BIT11
\n<b>Description:</b>\n
 Retransmitted TCP packet
*/
#define PKT_MBUF_RETRANS_TCP_PKT           PSP_BIT11

/** \ingroup PBUFFLAGS
#define PKT_MBUF_SRC_IP_MODIFIED           PSP_BIT12
\n<b>Description:</b>\n
 Source Ip is modified
*/
#define PKT_MBUF_SRC_IP_MODIFIED           PSP_BIT12

#define PKT_MBUF_SEC_PKT                   PSP_BIT14
#define PKT_MBUF_IPOPTS_PRESENT            PSP_BIT15

/** \ingroup PBUFFLAGS
 #define PKT_MBUF_ASF_DUMMY_PKT          PSP_BIT19
 \n<b>Description:</b>\n
  Use to mark the packet as dummy
 */
#define PKT_MBUF_ASF_DUMMY_PKT             PSP_BIT19

/** \ingroup PBUFMACROS
#define PKT_MBUF_SELFPKT_DO_NOT_FORWORD     PSP_BIT20
\n<b>Description:</b>\n
 Do not forward the packet
*/
#define PKT_MBUF_SELFPKT_DO_NOT_FORWORD     PSP_BIT20

/** \ingroup PBUFFLAGS
#define PKT_MBUF_MEMFLAG_HEAP              PSP_BIT21
\n<b>Description:</b>\n
 Memeory allocated in heap
*/
#define PKT_MBUF_MEMFLAG_HEAP              PSP_BIT21

/** \ingroup PBUFFLAGS
#define PKT_MBUF_HW_AREA_REQD       PSP_BIT22
\n<b>Description:</b>\n
 Hardware area is required.
*/
#define PKT_MBUF_HW_AREA_REQD       PSP_BIT22

/** \ingroup PBUFFLAGS
#define PKT_MBUF_SKIP_AREA_REQD     PSP_BIT23
\n<b>Description:</b>\n
Skip area is required
*/
#define PKT_MBUF_SKIP_AREA_REQD     PSP_BIT23

/** \ingroup PBUFFLAGS
#define PKT_MBUF_SG_ACCEPTED               PSP_BIT24
\n<b>Description:</b>\n
Scatter Gather Buffer is accepted for allocation
*/
#define PKT_MBUF_SG_ACCEPTED               PSP_BIT24

/** \ingroup PBUFFLAGS
#define PKT_MBUF_CLONE                     PSP_BIT25
\n<b>Description:</b>\n
Clone buffer.Data area points to data area of parent buffer.
*/
#define PKT_MBUF_CLONE                     PSP_BIT25

/* macros to set/reset flag */
/** \ingroup PBUFFLAGS
#define PKT_MBUF_RT_LOCAL_DELIVER          PSP_BIT26
\n<b>Description:</b>\n
Deliver to stack. Used when rt cache is absent
*/
#define PKT_MBUF_RT_LOCAL_DELIVER           PSP_BIT26

/* macros to set/reset flag */
/** \ingroup PBUFFLAGS
#define PKT_MBUF_SCRATCH_AREA_REQD  PSP_BIT27
\n<b>Description:</b>\n
 Scratch area is required.
*/
#define PKT_MBUF_SCRATCH_AREA_REQD  PSP_BIT27

/* macros to set/reset flag */
/** \ingroup PBUFFLAGS
#define PKT_MBUF_V6REASM_MODULE_PROCESSED   PSP_BIT28
\n<b>Description:</b>\n
 packet processed by V6 reassembly module.
*/
#define PKT_MBUF_V6REASM_MODULE_PROCESSED  PSP_BIT28

/* macros to set/reset flag */
/** \ingroup PBUFFLAGS
#define PKT_MBUF_IPSEC_INPROCESS_DONE          PSP_BIT29
\n<b>Description:</b>\n
 Used for IPSEC Inbound processing is done in ASF on this packet.
*/
#define PKT_MBUF_IPSEC_INPROCESS_DONE          PSP_BIT29

/* used to indicate RX packet if pkt from kernel, DPAA case only */
#define PKT_MBUF_KERN_RX_PKT		  PKT_MBUF_RT_LOCAL_DELIVER

/** \ingroup PBUFMACROS
#define pkt_mbuf_flag_set(buf,bit) PSP_BIT_SET((buf)->flags,bit)
\n<b>Description:</b>\n
Set a bit in PSPBuf Flags
*/
#define pkt_mbuf_flag_set(buf,bit) PSP_BIT_SET((buf)->flags,bit)

/** \ingroup PBUFFLAGS
#define pkt_mbuf_flag_reset(buf,bit) PSP_BIT_RST((buf)->flags,bit)
\n<b>Description:</b>\n
Reset a bit in PSPBuf Flags
*/
#define pkt_mbuf_flag_reset(buf,bit) PSP_BIT_RST((buf)->flags,bit)

/** \ingroup PBUFFLAGS
#define pkt_mbuf_data_ptr(x)  ((x)->data.ptr)
\n<b>Description:</b>\n
Access PSP Buffers data pointer
*/
#define pkt_mbuf_data_ptr(x)  ((x)->data.ptr)

/** \ingroup PBUFMACROS
#define pkt_mbuf_sg_valid(mbuf) \
           ( mbuf->sg.next != NULL)
\n<b>Description:</b>\n
Is this a Scatter Gather buffer
*/
#define pkt_mbuf_sg_valid(mbuf) \
           ( mbuf->sg.next != NULL)

#define pkt_mbuf_nwhdr_ptr_valid(mbuf) \
           ( ( mbuf->network_hdr.buf != NULL) &&  (mbuf->network_hdr.ptr != NULL))

#define pkt_mbuf_xporthdr_ptr_valid(mbuf) \
           ( ( mbuf->transport_hdr.buf != NULL)  && (mbuf->transport_hdr.ptr != NULL))




/** \ingroup PBUFMACROS
#define PKT_MBUF_ICMP_HDR_LEN               8
*/
#define PKT_MBUF_ICMP_HDR_LEN               8

/** \ingroup PBUFMACROS
#define PKT_MBUF_IPV4_HDR_MIN_LEN          20
*/
#define PKT_MBUF_IPV4_HDR_MIN_LEN          20


/** \ingroup PBUFMACROS
#define PKT_MBUF_IPV4_HDR_MAX_LEN          60
*/
#define PKT_MBUF_IPV4_HDR_MAX_LEN          60

/** \ingroup PBUFMACROS
#define PKT_MBUF_IPV4_OPT_MAX_LEN          40
*/
#define PKT_MBUF_IPV4_OPT_MAX_LEN          40

/** \ingroup PBUFMACROS
#define PKT_MBUF_IPV4_VERSION               4
*/
#define PKT_MBUF_IPV4_VERSION               4

/** \ingroup PBUFMACROS
#define PKT_MBUF_IPV4_TOS_OFFSET            1
*/
#define PKT_MBUF_IPV4_TOS_OFFSET            1

/** \ingroup PBUFMACROS
#define PKT_MBUF_IPV4_TOTLEN_OFFSET         2
*/
#define PKT_MBUF_IPV4_TOTLEN_OFFSET         2

/** \ingroup PBUFMACROS
#define PKT_MBUF_IPV4_ID_OFFSET             4
*/
#define PKT_MBUF_IPV4_ID_OFFSET             4

/** \ingroup PBUFMACROS
#define PKT_MBUF_IPV4_FRAGOFF_OFFSET        6
*/
#define PKT_MBUF_IPV4_FRAGOFF_OFFSET        6

/** \ingroup PBUFMACROS
#define PKT_MBUF_IPV4_TTL_OFFSET            8
*/
#define PKT_MBUF_IPV4_TTL_OFFSET            8

/** \ingroup PBUFMACROS
#define PKT_MBUF_IPV4_PROTO_OFFSET          9
*/
#define PKT_MBUF_IPV4_PROTO_OFFSET          9

/** \ingroup PBUFMACROS
*/
#define PKT_MBUF_IPV4_CKSUM_OFFSET        10

/** \ingroup PBUFMACROS
#define PKT_MBUF_IPV4_SRC_IP_OFFSET        12
*/
#define PKT_MBUF_IPV4_SRC_IP_OFFSET        12

/** \ingroup PBUFMACROS
#define PKT_MBUF_IPV4_DST_IP_OFFSET      16
*/
#define PKT_MBUF_IPV4_DST_IP_OFFSET      16





/* The following macros can be used to access different fields of IP header in the packet
 * WARNING !!! We need to make sure that we can read sufficient number of bytes starting from IP header.
 */
#define pkt_mbuf_ipv4_hdr_ptr(x)   pkt_mbuf_nw_hdr_ptr(x)


#define pkt_mbuf_ipv4_tos_ptr(x)      ( (pkt_mbuf_ipv4_hdr_ptr(x) + PKT_MBUF_IPV4_TOS_OFFSET))

#define PSPBUF_IPV4_TOTLEN_PTR(x)    \
             (pkt_mbuf_ipv4_hdr_ptr(x) + PKT_MBUF_IPV4_TOTLEN_OFFSET)

#define pkt_mbuf_ipv4_id_ptr(x)        \
             (pkt_mbuf_ipv4_hdr_ptr(x) + PKT_MBUF_IPV4_ID_OFFSET)

#define pkt_mbuf_ipv4_fragoff_ptr(x)   \
             (pkt_mbuf_ipv4_hdr_ptr(x) + PKT_MBUF_IPV4_FRAGOFF_OFFSET)

#define pkt_mbuf_ipv4_ttl_ptr(x)       \
             (pkt_mbuf_ipv4_hdr_ptr(x) +PKT_MBUF_IPV4_TTL_OFFSET)

#define pkt_mbuf_ipv4_proto_ptr(x)     \
             (pkt_mbuf_ipv4_hdr_ptr(x) + PKT_MBUF_IPV4_PROTO_OFFSET)

#define pkt_mbuf_ipv4_cksum_ptr(x)    \
             (pkt_mbuf_ipv4_hdr_ptr(x) + PKT_MBUF_IPV4_CKSUM_OFFSET)

#define pkt_mbuf_ipv4_src_ip_ptr(x)    \
             (pkt_mbuf_ipv4_hdr_ptr(x) + PKT_MBUF_IPV4_SRC_IP_OFFSET)

#define pkt_mbuf_ipv4_dst_ip_ptr(x)    \
             (pkt_mbuf_ipv4_hdr_ptr(x) + PKT_MBUF_IPV4_DST_IP_OFFSET)


#define pkt_mbuf_ipv4_ver(x)             ((*pkt_mbuf_ipv4_hdr_ptr(x)) >> 4)
#define pkt_mbuf_ipv4_hdr_len(x)         (((*pkt_mbuf_ipv4_hdr_ptr(x)) & 0x0F) << 2)
#define pkt_mbuf_ipv4_tos(x)             (*pkt_mbuf_ipv4_tos_ptr(x))
#define pkt_mbuf_ipv4_total_len(x)       pkt_mbuf_get_16bits(PSPBUF_IPV4_TOTLEN_PTR(x))
#define pkt_mbuf_ipv4_id(x)              pkt_mbuf_get_16bits(pkt_mbuf_ipv4_id_ptr(x))


#define pkt_mbuf_ipv4_flags(x)           ((*pkt_mbuf_ipv4_fragoff_ptr(x)) >> 5)
#define pkt_mbuf_ipv4_df_flag(x)        ((*pkt_mbuf_ipv4_fragoff_ptr(x)) >> 6 & 0x01)
#define pkt_mbuf_ipv4_mf_flag(x)        ((*pkt_mbuf_ipv4_fragoff_ptr(x)) >> 5 & 0x01)
#define PSPBUF_IPV4_FRAG_OFFSET(x)     \
             (pkt_mbuf_get_16bits(pkt_mbuf_ipv4_fragoff_ptr(x)) & 0x1FFF)
#define pkt_mbuf_ipv4_ttl(x)             ((*pkt_mbuf_ipv4_ttl_ptr(x)))

#define pkt_mbuf_ipv4_proto(x)           (*pkt_mbuf_ipv4_proto_ptr(x))
#define pkt_mbuf_ipv4_cksum(x)          pkt_mbuf_get_16bits(pkt_mbuf_ipv4_cksum_ptr(x))
#define pkt_mbuf_ipv4_src_ip(x)          pkt_mbuf_get_32bits(pkt_mbuf_ipv4_src_ip_ptr(x))
#define pkt_mbuf_ipv4_dst_ip(x)          pkt_mbuf_get_32bits(pkt_mbuf_ipv4_dst_ip_ptr(x))
#define pkt_mbuf_ipv4_payload_len(x)  \
             (pkt_mbuf_ipv4_total_len(x) - pkt_mbuf_ipv4_hdr_len(x))
#define pkt_mbuf_ipv4_options_len(x)      \
             (pkt_mbuf_ipv4_hdr_len(x) - PKT_MBUF_IPV4_HDR_MIN_LEN)

#define pkt_mbuf_ipv4_fragoffset_val(x)  pkt_mbuf_get_16bits(pkt_mbuf_ipv4_fragoff_ptr(x))
#define pkt_mbuf_ipv4_is_frag(x)        \
             (pkt_mbuf_get_16bits(pkt_mbuf_ipv4_fragoff_ptr(x)) & 0x3FFF)

/*  TCP related macros */
/** \ingroup PBUFMACROS
#define PKT_MBUF_TCP_HDR_MIN_LEN       20  // bytes
*/
#define PKT_MBUF_TCP_HDR_MIN_LEN       20  // bytes

/** \ingroup PBUFMACROS
#define PKT_MBUF_TCP_OPT_MAX_LEN       40
*/
#define PKT_MBUF_TCP_OPT_MAX_LEN       40

/** \ingroup PBUFMACROS
#define PKT_MBUF_TCP_OPT_MSS_LEN        4
*/
#define PKT_MBUF_TCP_OPT_MSS_LEN        4

/** \ingroup PBUFMACROS
#define PKT_MBUF_TCP_SRC_PORT_OFFSET    0
*/
#define PKT_MBUF_TCP_SRC_PORT_OFFSET    0

/** \ingroup PBUFMACROS
#define PKT_MBUF_TCP_DST_PORT_OFFSET    2
*/
#define PKT_MBUF_TCP_DST_PORT_OFFSET    2

/** \ingroup PBUFMACROS
#define PKT_MBUF_TCP_SEQ_NUM_OFFSET     4
*/
#define PKT_MBUF_TCP_SEQ_NUM_OFFSET     4

/** \ingroup PBUFMACROS
#define PKT_MBUF_TCP_ACK_NUM_OFFSET     8
*/
#define PKT_MBUF_TCP_ACK_NUM_OFFSET     8

/** \ingroup PBUFMACROS
#define PKT_MBUF_TCP_HDR_LEN_OFFSET    12
*/
#define PKT_MBUF_TCP_HDR_LEN_OFFSET    12

/** \ingroup PBUFMACROS
#define PKT_MBUF_TCP_FLAGS_OFFSET      13
*/
#define PKT_MBUF_TCP_FLAGS_OFFSET      13

/** \ingroup PBUFMACROS
#define PKT_MBUF_TCP_WIN_OFFSET        14
*/
#define PKT_MBUF_TCP_WIN_OFFSET        14

/** \ingroup PBUFMACROS
#define PKT_MBUF_TCP_CKSUM_OFFSET     16
*/
#define PKT_MBUF_TCP_CKSUM_OFFSET     16

/** \ingroup PBUFMACROS
#define PKT_MBUF_TCP_URG_OFFSET        18
*/
#define PKT_MBUF_TCP_URG_OFFSET        18

/** \ingroup PBUFMACROS
*/
/** \ingroup PBUFMACROS
#define PKT_MBUF_ICMP_TYPE_OFFSET    0
*/
#define PKT_MBUF_ICMP_TYPE_OFFSET    0


/** \ingroup PBUFMACROS
*/
#define pkt_mbuf_icmp_type_ptr(x)   \
           (pkt_mbuf_nw_hdr_ptr(x) + pkt_mbuf_ipv4_hdr_len(x) + PKT_MBUF_ICMP_TYPE_OFFSET)

#define pkt_mbuf_tcp_src_port_ptr(x)   \
           (pkt_mbuf_xport_hdr_ptr(x) + PKT_MBUF_TCP_SRC_PORT_OFFSET)
#define pkt_mbuf_tcp_dst_port_ptr(x)   \
           (pkt_mbuf_xport_hdr_ptr(x) + PKT_MBUF_TCP_DST_PORT_OFFSET)
#define pkt_mbuf_tcp_seq_num_ptr(x)    \
           (pkt_mbuf_xport_hdr_ptr(x) + PKT_MBUF_TCP_SEQ_NUM_OFFSET)
#define pkt_mbuf_tcp_ack_num_ptr(x)    \
           (pkt_mbuf_xport_hdr_ptr(x) + PKT_MBUF_TCP_ACK_NUM_OFFSET)
#define pkt_mbuf_tcp_hdr_len_ptr(x)    \
           (pkt_mbuf_xport_hdr_ptr(x) + PKT_MBUF_TCP_HDR_LEN_OFFSET)
#define pkt_mbuf_tcp_flags_ptr(x)      \
           (pkt_mbuf_xport_hdr_ptr(x) + PKT_MBUF_TCP_FLAGS_OFFSET)
#define pkt_mbuf_tcp_win_ptr(x)        \
           (pkt_mbuf_xport_hdr_ptr(x) + PKT_MBUF_TCP_WIN_OFFSET)
#define pkt_mbuf_tcp_cksum_ptr(x)     \
           (pkt_mbuf_xport_hdr_ptr(x) + PKT_MBUF_TCP_CKSUM_OFFSET)
#define pkt_mbuf_tcp_urg_ptr(x)        \
           (pkt_mbuf_xport_hdr_ptr(x) + PKT_MBUF_TCP_URG_OFFSET)

#define pkt_mbuf_tcp_src_port(x)  pkt_mbuf_get_16bits(pkt_mbuf_tcp_src_port_ptr(x))
#define pkt_mbuf_tcp_dst_port(x)  pkt_mbuf_get_16bits(pkt_mbuf_tcp_dst_port_ptr(x))
#define pkt_mbuf_tcp_seq_num(x)   pkt_mbuf_get_32bits(pkt_mbuf_tcp_seq_num_ptr(x))
#define pkt_mbuf_tcp_ack_num(x)   pkt_mbuf_get_32bits(pkt_mbuf_tcp_ack_num_ptr(x))
#define pkt_mbuf_tcp_hdr_len(x)   (((*pkt_mbuf_tcp_hdr_len_ptr(x)) & 0xF0) >> 2)
#define pkt_mbuf_tcp_flags(x)     (*pkt_mbuf_tcp_flags_ptr(x))
#define pkt_mbuf_tcp_win(x)       pkt_mbuf_get_16bits(pkt_mbuf_tcp_win_ptr(x))
#define pkt_mbuf_tcp_cksum(x)    pkt_mbuf_get_16bits(pkt_mbuf_tcp_cksum_ptr(x))
#define pkt_mbuf_tcp_urg(x)       pkt_mbuf_get_16bits(pkt_mbuf_tcp_urg_ptr(x))


#define pkt_mbuf_icmptype(x)     (*(pkt_mbuf_icmp_type_ptr(x)))
/* UDP related macros */
/** \ingroup PBUFMACROS
#define PKT_MBUF_UDP_HDR_MIN_LEN        8
*/
#define PKT_MBUF_UDP_HDR_MIN_LEN        8

/** \ingroup PBUFMACROS
#define PKT_MBUF_UDP_SRC_PORT_OFFSET    0
*/
#define PKT_MBUF_UDP_SRC_PORT_OFFSET    0

/** \ingroup PBUFMACROS
#define PKT_MBUF_UDP_DST_PORT_OFFSET    2
*/
#define PKT_MBUF_UDP_DST_PORT_OFFSET    2

/** \ingroup PBUFMACROS
#define PKT_MBUF_UDP_PKT_LEN_OFFSET     4
*/
#define PKT_MBUF_UDP_PKT_LEN_OFFSET     4

/** \ingroup PBUFMACROS
#define PKT_MBUF_UDP_CKSUM_OFFSET      6
*/
#define PKT_MBUF_UDP_CKSUM_OFFSET      6

#define pkt_mbuf_udp_src_port_ptr(x)   \
           (pkt_mbuf_xport_hdr_ptr(x) + PKT_MBUF_UDP_SRC_PORT_OFFSET)
#define pkt_mbuf_udp_dst_port_ptr(x)   \
           (pkt_mbuf_xport_hdr_ptr(x) + PKT_MBUF_UDP_DST_PORT_OFFSET)
#define pkt_mbuf_udp_pkt_len_ptr(x)    \
           (pkt_mbuf_xport_hdr_ptr(x) + PKT_MBUF_UDP_PKT_LEN_OFFSET)
#define pkt_mbuf_udp_cksum_ptr(x)    \
           (pkt_mbuf_xport_hdr_ptr(x) + PKT_MBUF_UDP_CKSUM_OFFSET)

#define pkt_mbuf_udp_src_port(x)  pkt_mbuf_get_16bits(pkt_mbuf_udp_src_port_ptr(x))
#define pkt_mbuf_udp_dst_port(x)  pkt_mbuf_get_16bits(pkt_mbuf_udp_dst_port_ptr(x))
#define pkt_mbuf_udp_pkt_len(x)   pkt_mbuf_get_16bits(pkt_mbuf_udp_pkt_len_ptr(x))
#define pkt_mbuf_udp_cksum(x)    pkt_mbuf_get_16bits(pkt_mbuf_udp_cksum_ptr(x))

// ipv6macros
#ifdef PSP_IPV6_SUPPORT
#define pkt_mbuf_ipv6_src_addr(x)  (&(x)->nw_tuple.ip6.src)
#define pkt_mbuf_ipv6_dest_addr(x)  (&(x)->nw_tuple.ip6.dst)
#define pkt_mbuf_ipv6_src_ip(x)      ((x)->nw_tuple.ip6.src)
#define pkt_mbuf_ipv6_dest_ip(x)      ((x)->nw_tuple.ip6.dst)
#define pkt_mbuf_ipv6_hdr_len(x)  ((x)->nw_tuple.ip6.payload_len)
#define pkt_mbuf_ipv6_payload_len(x)  ((x)->nw_tuple.ip6.payload_len)
#define pkt_mbuf_ipv6_ext_hdr_offset_hopbyhop(x) ((x)->nw_tuple.ip6.params.hop_by_hop)
#define pkt_mbuf_ipv6_ext_hdr_offset_route(x) ((x)->nw_tuple.ip6.params.route)
#define pkt_mbuf_ipv6_ext_hdr_offset_dest0(x) ((x)->nw_tuple.ip6.params.dest0)
#define pkt_mbuf_ipv6_ext_hdr_offset_dest1(x) ((x)->nw_tuple.ip6.params.dest1)
#define pkt_mbuf_ipv6_ext_hdr_offset_fragment(x) ((x)->nw_tuple.ip6.params.fragment)
#define pkt_mbuf_ipv6_ext_hdr_offset_noextheader(x) ((x)->nw_tuple.ip6.params.num_ext_hdr)
#define pkt_mbuf_ipv6_ext_hdr_offset_xport(x) ((x)->nw_tuple.ip6.params.transport)
#define pkt_mbuf_ipv6_ext_hdr_offset_radv(x) ((x)->nw_tuple.ip6.params.ra_offset)

#define pkt_mbuf_ipv6_ext_hdrs_len(x) ((x)->nw_tuple.ip6.params.ext_hdrs_len)

#define pkt_mbuf_ipv6_xport_proto(x) ((x)->l3_proto)

#define pkt_mbuf_ipv6_is_fragment(x)  ( ((x)->nw_tuple.ip6.params.fragment) ? TRUE : FALSE)


#define PKT_MBUF_IPV6_ADDR_LEN                  16

/** \ingroup PBUFMACROS
#define PKT_MBUF_IPV6_PAYLOAD_LENGTH_OFFSET         4
*/
#define PKT_MBUF_IPV6_PAYLOAD_LENGTH_OFFSET         4

/** \ingroup PBUFMACROS
#define PKT_MBUF_IPV6_NEXT_HDR_OFFSET         6
*/
#define PKT_MBUF_IPV6_NEXT_HDR_OFFSET         6

/** \ingroup PBUFMACROS
#define PKT_MBUF_IPV6_HOP_LIMIT_OFFSET         7
*/
#define PKT_MBUF_IPV6_HOP_LIMIT_OFFSET         7

/** \ingroup PBUFMACROS
#define PKT_MBUF_IPV6_SRC_OFFSET         8
*/
#define PKT_MBUF_IPV6_SRC_OFFSET         8


/** \ingroup PBUFMACROS
#define PKT_MBUF_IPV6_DST_OFFSET         24
*/
#define PKT_MBUF_IPV6_DST_OFFSET         24


/** \ingroup PBUFMACROS
#define PKT_MBUF_IPV6_FLOW_INFO_OFFSET   0
*/
#define PKT_MBUF_IPV6_FLOW_INFO_OFFSET   0

#define pkt_mbuf_ipv6_hdr_ptr(x)   pkt_mbuf_nw_hdr_ptr(x)

#define PKT_MBUF_IPV6_TRAFFIC_CLASS_OFFSET          0

//Traffic class
#define pkt_mbuf_ipv6_traffic_class_ptr(x) \
             (pkt_mbuf_ipv6_hdr_ptr(x) + PKT_MBUF_IPV6_TRAFFIC_CLASS_OFFSET)
#define pkt_mbuf_ipv6_traffic_class(x)  (*pkt_mbuf_ipv6_traffic_class_ptr(x))

#define pkt_mbuf_ipv6_unfragmented_length(x)  (x->nw_tuple.ip6.params.unfrag_len)

#define pkt_mbuf_ipv6_hdr_ptr(x)   pkt_mbuf_nw_hdr_ptr(x)

#define pkt_mbuf_ipv6_next_hdr_ptr(X)         ( (pkt_mbuf_ipv6_hdr_ptr(X) + PKT_MBUF_IPV6_NEXT_HDR_OFFSET))
#define pkt_mbuf_ipv6_hop_limit_ptr(X)        ( (pkt_mbuf_ipv6_hdr_ptr(X) + PKT_MBUF_IPV6_HOP_LIMIT_OFFSET))
#define pkt_mbuf_ipv6_src_addr_ptr(X)         ( (pkt_mbuf_ipv6_hdr_ptr(X) + PKT_MBUF_IPV6_SRC_OFFSET))
#define pkt_mbuf_ipv6_dst_addr_ptr(X)         ( (pkt_mbuf_ipv6_hdr_ptr(X) + PKT_MBUF_IPV6_DST_OFFSET))
#define pkt_mbuf_ipv6_flow_info_ptr(X)        ( (pkt_mbuf_ipv6_hdr_ptr(X) + PKT_MBUF_IPV6_FLOW_INFO_OFFSET))

#define pkt_mbuf_ipv6_proto(X)                (*pkt_mbuf_ipv6_next_hdr_ptr(X))
#define pkt_mbuf_ipv6_hop_limit(X)            (*pkt_mbuf_ipv6_hop_limit_ptr(X))
#define pkt_mbuf_ipv6_flow_label(X)           ( (pkt_mbuf_get_32bits(pkt_mbuf_ipv6_flow_info_ptr(X))) & PSP_IPV6_FLOWLABEL_MASK )
#define pkt_mbuf_ipv6_flow_info(X)            ( (pkt_mbuf_get_32bits(pkt_mbuf_ipv6_flow_info_ptr(X))) & PSP_IPV6_FLOWINFO_MASK )
#define pkt_mbuf_ipv6_next_hdr_at_offset(X,Y)     (*(pkt_mbuf_ipv6_hdr_ptr(x) + y))

#define pkt_mbuf_ipv6_payload_length_ptr(x) \
             (pkt_mbuf_ipv6_hdr_ptr(x) + PKT_MBUF_IPV6_PAYLOAD_LENGTH_OFFSET)
/*
Payload length in the ipv6 header -- in jumbo packet case, this will not be correct
#define pkt_mbuf_ipv6_payload_len(X)       (pkt_mbuf_get_16bits(pkt_mbuf_ipv6_payload_length_ptr(X)))
*/

/** \ingroup PBUFMACROS
#define pkt_mbuf_ipv6_total_len(mbuf)  ((mbuf)->nw_tuple.ip6.payload_len+PSP_IPV6_HDR_LEN)
\n<b>Description:</b>\n
 Total packet length of ipv6 packet
*/
#define pkt_mbuf_ipv6_total_len(mbuf)  ((mbuf)->nw_tuple.ip6.payload_len+PSP_IPV6_HDR_LEN)

static inline void pkt_mbuf_parse_ipv6_tuple(struct pkt_mbuf *pkt)
{
  // Transport protocol must already be extraced while parsing extension headers
  //pkt->l3_proto = pkt_mbuf_ipv6_xport_proto(pkt);
  if(!(pkt->nw_tuple.ip6.params.flags & PKT_MBUF_IPV6_EXTHDR_PROCESSED))
    pkt->l3_proto = pkt_mbuf_ipv6_proto(pkt);
  memcpy(&pkt->nw_tuple.ip6.dst,pkt_mbuf_ipv6_dst_addr_ptr(pkt),PKT_MBUF_IPV6_ADDR_LEN);
  memcpy(&pkt->nw_tuple.ip6.src,pkt_mbuf_ipv6_src_addr_ptr(pkt),PKT_MBUF_IPV6_ADDR_LEN);
  pkt->nw_tuple.ip6.flow_info = pkt_mbuf_ipv6_flow_info(pkt); /*Not require if rtcache is in radix tree*/
  pkt->flags |= PKT_MBUF_NW_INFO_VALID;
  // In case of jumbo pkts, payload length is already parsed and it should not be overwritten
  //if(pkt->nw_tuple.ip6.payload_len == 0)
  if( !(pkt->nw_tuple.ip6.params.flags & PKT_MBUF_IPV6_JUMBOPAYLOAD))
    pkt->nw_tuple.ip6.payload_len  =   pkt_mbuf_get_16bits(pkt_mbuf_ipv6_payload_length_ptr(pkt)) ;
}

static inline void pkt_mbuf_parse_ipv6_tuple_ex(struct pkt_mbuf *buf)
{
  if(!(buf->flags & PKT_MBUF_NW_INFO_VALID)) pkt_mbuf_parse_ipv6_tuple(buf);
}


#endif /*PSP_IPV6_SUPPORT*/

/* ESP Header related macros */
/** \ingroup PBUFMACROS
#define PKT_MBUF_ESP_SPI_OFFSET        0
*/
#define PKT_MBUF_ESP_SPI_OFFSET        0

/** \ingroup PBUFMACROS
#define PKT_MBUF_ESP_SEQ_NUM_OFFSET    4
*/
#define PKT_MBUF_ESP_SEQ_NUM_OFFSET    4

#define PKT_MBUF_ESP_SPI_PTR(x)   \
           (pkt_mbuf_xport_hdr_ptr(x) + PKT_MBUF_ESP_SPI_OFFSET)
#define PKT_MBUF_ESP_SEQ_NUM_PTR(x)   \
           (pkt_mbuf_xport_hdr_ptr(x) + PKT_MBUF_ESP_SEQ_NUM_OFFSET)

#define PKT_MBUF_ESP_SPI(x)      pkt_mbuf_get_32bits(PKT_MBUF_ESP_SPI_PTR(x))
#define PKT_MBUF_ESP_SEQ_NUM(x)  pkt_mbuf_get_32bits(PKT_MBUF_ESP_SEQ_NUM_PTR(x))

/* AH Header related macros */
/** \ingroup PBUFMACROS
#define PKT_MBUF_AH_SPI_OFFSET        4
*/
#define PKT_MBUF_AH_SPI_OFFSET        4

/** \ingroup PBUFMACROS
#define PKT_MBUF_SEQ_NUM_OFFSET    8
*/
#define PKT_MBUF_SEQ_NUM_OFFSET    8

#define PKT_MBUF_AH_SPI_PTR(x)   \
           (pkt_mbuf_xport_hdr_ptr(x) + PKT_MBUF_AH_SPI_OFFSET)
#define PKT_MBUF_AH_SEQ_NUM_PTR(x)   \
           (pkt_mbuf_xport_hdr_ptr(x) + PKT_MBUF_SEQ_NUM_OFFSET)

#define PKT_MBUF_AH_SPI(x)      pkt_mbuf_get_32bits(PKT_MBUF_AH_SPI_PTR(x))
#define PKT_MBUF_AH_SEQ_NUM(x)  pkt_mbuf_get_32bits(PKT_MBUF_AH_SEQ_NUM_PTR(x))


#define PKT_MBUF_ICMP_ERR_INNER_IP_HRDR_PTR(x)   \
           (pkt_mbuf_nw_hdr_ptr(x) + pkt_mbuf_ipv4_hdr_len(x) + PKT_MBUF_ICMP_HDR_LEN)
/* #define PSPBUF_IPV4_PAYLOAD_PTR(x)   \
            (pkt_mbuf_ipv4_hdr_ptr(x) + PSP_IPV4_HDR_LEN(x))

#define PSPBUF_IPV4_OPT_PTR(x)       \
             (pkt_mbuf_ipv4_hdr_ptr(x) + PKT_MBUF_IPV4_HDR_MIN_LEN)
*/

/** \ingroup PBUFMACROS
#define __pkt_mbuf_get_phy_addr_at_data_offset(mbuf,uiOffset)

\n<b>Description:</b>\n
 This macro returns Physical address at given offset in PSPBuf data
  <b>Input Parameters:</b>\n
  <i> mbuf :</i> pointer to PSPBuf structure.
  <i> uiOffset:</i> offset from the beginning of data.

  <b>Output Parameters</b>\n
  <i> None </i>

  <b>Returns</b>\n
  <i> Physical address of data at given offset </i>
*/
#define __pkt_mbuf_get_phy_addr_at_data_offset(mbuf,uiOffset)\
   (pkt_mbuf_get_pkt_phy_addr(mbuf) + (mbuf->data.ptr + uiOffset) - mbuf->start_of_data)

/** \ingroup PBUFMACROS
#define __pkt_mbuf_get_phy_addr_at_current_data_ptr(mbuf)

\n<b>Description:</b>\n
 This macro returns Physical address of data pointed by current data in PSPBuf
  <b>Input Parameters:</b>\n
  <i> mbuf :</i> pointer to PSPBuf structure.

  <b>Output Parameters</b>\n
  <i> None </i>

  <b>Returns</b>\n
  <i> Physical address of data pointed by current data pointer </i>
*/
#define __pkt_mbuf_get_phy_addr_at_current_data_ptr(mbuf)\
    (pkt_mbuf_get_pkt_phy_addr(mbuf) + (mbuf->data.ptr - mbuf->start_of_data))

/** \ingroup PBUFMACROS
#define __pkt_mbuf_get_phy_addr_of_data_ref(mbuf,data_ref,phy_addr)

\n<b>Description:</b>\n
 This macro returns Physical address of data pointed by given user data reference
  <b>Input Parameters:</b>\n
  <i> mbuf :</i> pointer to PSPBuf structure.
  <i>  :</i> .

  <b>Output Parameters</b>\n
  <i> None </i>

  <b>Returns</b>\n
  <i> Physical address of data pointed by givent data reference </i>
  <i> None </i>
*/
#define __pkt_mbuf_get_phy_addr_of_data_ref(mbuf,data_ref,phy_addr)\
  phy_addr = (pkt_mbuf_get_pkt_phy_addr(data_ref.buf) + ( data_ref.ptr - data_ref.buf->start_of_data));



/** \ingroup PBUFMACROS
#define __pkt_mbuf_get_data_ref_at_offset( mbuf, offset, data_ref, bytes_valid)

\n<b>Description:</b>\n
 This macro returns reference to the data at given offset into the packet data.
 Macro also gives number of bytes that can be read using the pointer in the returned Data Reference.
 Caller need to check the parameter 'bytes_valid' to see how many bytes can be read using the data pointer.
 If 'bytes_valid' is zero, we should treat returned Data reference as invalid.
  <b>Input Parameters:</b>\n
  <i> mbuf :</i> pointer to PSPBuf structure.
  <i> offset:</i> offset from the currrent data pointer.

  <b>Output Parameters</b>\n
  <i> data_ref: </i> Reference to the data at given offset into the packet data
  <i> bytes_valid: </i> Valid contiguous bytes available starting from the given offset

  <b>Returns</b>\n
  <i> none </i>
*/
#define __pkt_mbuf_get_data_ref_at_offset( mbuf, offset, data_ref, bytes_valid) \
{\
  uint32_t addr;\
  if((pkt_mbuf_is_sg_valid(mbuf) != TRUE))\
  {\
   uint32_t addr, data_start,data_end ;\
   addr = (uint32_t)mbuf->data.ptr + offset ;\
   data_start = (uint32_t)(mbuf->data.ptr);\
   data_end   = (uint32_t)(mbuf->data.ptr) + (mbuf->count);\
   if((addr>=data_start) && (addr<=data_end))    \
   {\
        data_ref.ptr = (uchar8_t *)addr;		\
	data_ref.buf  = mbuf; \
        bytes_valid = mbuf->count - offset; \
   }\
   else\
   {\
     printf("\r\n%s:%d Panic!!! Offset is beyond valid data!!!   \r\n",__FUNCTION__,__LINE__);\
     pkt_mbuf_debug(" Addr = 0x%x, Data Begin = 0x%x, Data End = 0x%x    \r\n",addr,data_start,data_end);\
   }\
  }\
  else \
  {\
   uint32_t num_bytes;\
   struct pkt_mbuf *buf = mbuf;\
   num_bytes = offset; \
   do \
   {\
     addr = (uint32_t)buf->data.ptr;\
     if(num_bytes < buf->count) \
     {\
       data_ref.ptr = (uchar8_t *)(addr + num_bytes);	\
       data_ref.buf =  buf;\
       bytes_valid = buf->count - num_bytes; \
       break; \
     }\
     else \
     {\
       num_bytes -= buf->count;\
       if(buf->sg.next == buf->sg.head/*NULL*/)\
       { \
         printf("%s:%d Panic!!! Offset is beyond valid data!!!   \r\n",__FUNCTION__,__LINE__);\
  	 bytes_valid = 0; \
	 break;\
       } \
     }\
     buf = buf->sg.next;\
   }while(num_bytes);\
 }\
}





/** \ingroup PBUFMACROS
#define __pkt_mbuf_get_data_offset(mbuf,data_ref,offset)

\n<b>Description:</b>\n
  This macro gives offset from current data to the data pointed by given data reference.
  <b>Input Parameters:</b>\n
  <i> mbuf :</i> pointer to PSPBuf structure.
  <i> data_ref :</i> reference to data for which offset is needed.

  <b>Output Parameters</b>\n
  <i> offset:</i> offset from the currrent data pointer to the given data reference.

  <b>Returns</b>\n
  <i> None </i>
*/
#define __pkt_mbuf_get_data_offset(mbuf,data_ref,offset) \
{\
  struct pkt_mbuf *buf;\
  offset = 0;\
  uint32_t addr,start;\
  addr  = (uint32_t)data_ref.ptr;\
  \
  if(!(pkt_mbuf_is_sg_valid(mbuf)))\
  {\
   uint32_t data_start,data_end ;\
   data_start = (uint32_t)(mbuf->data.ptr);\
   data_end   = (uint32_t)(mbuf->data.ptr) + (mbuf->count);\
    if((addr < data_start) ||   (addr > data_end ))\
    {\
      offset = PSP_FAILURE;\
      pkt_mbuf_debug("%s:%d Panic!!! Offset is going beyond valid data!!!   \r\n",__FUNCTION__,__LINE__);\
      pkt_mbuf_debug(" Addr = 0x%x, Data Begin = 0x%x, Data End = 0x%x    \r\n",addr,data_start,data_end);\
    }\
    else \
    {\
     offset = (addr - data_start);\
    }\
  }\
  \
  /* Scatter Gather list */\
  else\
  {\
    buf = mbuf;\
    while(1)\
    {\
      start = (uint32_t)buf->data.ptr;\
      if( (data_ref.buf != buf) ||  (addr < start) ||\
	 (addr >  start + buf->count))\
      {\
        offset += buf->count;\
        if(buf->sg.next == buf->sg.head)\
        {\
          offset = PSP_FAILURE;\
          pkt_mbuf_debug(" buf = %p,  Addr = 0x%x, Data Begin = 0x%x, count = %u    \r\n",buf,addr,start,buf->count);\
          pkt_mbuf_debug("%s:%d Panic!!! Data reference going beyond valid data!!!   \r\n",__FUNCTION__,__LINE__);\
          break;\
        }\
        buf = buf->sg.next;\
      }\
      else\
      {\
       offset +=  (addr - ((uint32_t)buf->data.ptr));\
       break;\
      }\
    }\
  }\
}

/** \ingroup PBUFMACROS
#define __pkt_mbuf_get_sod_offset(mbuf,data_ref,offset)

\n\n<b>Description:</b>\n
  This macro gives offset from SoD to the data pointed by given data reference.
  <b>Input Parameters:</b>\n
  <i> mbuf :</i> pointer to PSPBuf structure.
  <i> data_ref :</i> reference to data for which offset is needed.

  <b>Output Parameters</b>\n
  <i> offset:</i> offset from SoD to the given data reference.

  <b>Returns</b>\n
  <i> None </i>
*/
#define __pkt_mbuf_get_sod_offset(mbuf,data_ref,offset) \
{\
  struct pkt_mbuf *buf;\
  offset = 0;\
  uint32_t addr,start;\
  addr  = (uint32_t)data_ref.ptr;\
  \
  if(!(pkt_mbuf_is_sg_valid(mbuf)))\
  {\
   uint32_t data_start,data_end ;\
   data_start = (uint32_t)(mbuf->start_of_data);\
   data_end   = (uint32_t)(mbuf->start_of_data) + pkt_mbuf_max_data_size(mbuf);\
    if((addr < data_start) ||   (addr > data_end ))\
    {\
      offset = PSP_FAILURE;\
      pkt_mbuf_debug("%s:%d Panic!!! Offset is going beyond valid data!!!   \r\n",__FUNCTION__,__LINE__);\
      pkt_mbuf_debug(" Addr = 0x%x, Data Begin = 0x%x, Data End = 0x%x    \r\n",addr,data_start,data_end);\
    }\
    else \
    {\
     offset = (addr - data_start);\
    }\
  }\
  \
  /* Scatter Gather list */\
  else\
  {\
    buf = mbuf;\
    start = (uint32_t)buf->start_of_data;\
    while(1)\
    {\
      if((addr < start) ||\
	 (addr >  start + pkt_mbuf_max_data_size(buf)))	\
      {\
        offset += pkt_mbuf_max_data_size(buf);	\
        if(buf->sg.next == buf->sg.head)\
        {\
          offset = PSP_FAILURE;\
          pkt_mbuf_debug("%s:%d Panic!!! Data reference going beyond valid data!!!   \r\n",__FUNCTION__,__LINE__);\
          break;\
        }\
        buf = buf->sg.next;\
        start = (uint32_t)buf->start_of_data;\
      }\
      else\
      {\
       offset +=  (addr - ((uint32_t)buf->start_of_data));\
       break;\
      }\
    }\
  }\
}

//INLINE void  __pkt_mbuf_pull_data_ref(struct mbuf *mbuf, struct pkt_mbuf_data_ref data_ref,uint32_t num_bytes, uint32_t bytes_pulled)
/* __pkt_mbuf_pull_data_ref(struct pkt_mbuf *mbuf, struct pkt_mbuf_data_ref data_ref,uint32_t num_bytes, OUT uint32_t bytes_pulled) */
/** \ingroup PBUFMACROS
#define __pkt_mbuf_pull_data_ref(mbuf,data_ref,num_bytes,bytes_pulled)

\n<b>Description:</b>\n
  This macro advances the user Data Reference by given bytes.
  Current data reference in PSPBuf is NOT modified.
  Before calling the macro, user needs to make sure that PSPBuf has enough data to pull.
  After the macro, user may check if all the bytes are pulled ('bytes_pulled' is equal to 'num_bytes')
  If 'bytes_pulled' is zero, data_ref returned is NOT valid.
  <b>Input Parameters:</b>\n
  <i> mbuf : </i> pointer to PSPBuf structure.
  <i> data_ref :</i> data reference that needs to be pulled.
  <i> num_bytes : </i> Number of Bytes to be pulled

  <b>Output Parameters</b>\n
  <i> bytes_pulled: </i> Number of Bytes actually pulled

  <b>Returns</b>\n
  <i> None </i>
*/
#define __pkt_mbuf_pull_data_ref(mbuf,data_ref,num_of_bytes,bytes_pulled) \
{\
  uint32_t num_bytes = num_of_bytes;\
  uint32_t max_data;\
  if((pkt_mbuf_is_sg_valid(mbuf) != TRUE))	\
  {\
    uint32_t addr,data_start,data_end ;\
    addr = (uint32_t)data_ref.ptr + num_bytes ;\
    data_start = (uint32_t)(mbuf->start_of_data);\
    data_end   = (uint32_t)mbuf->start_of_data + pkt_mbuf_max_data_size(mbuf) - 1;\
    if((addr >= data_start) && (addr <= data_end))    \
    {\
      data_ref.ptr += num_bytes;\
      bytes_pulled = num_bytes; \
    }\
    else\
    {\
      printf("%s:%d Panic!!! Data reference going beyond valid data!!!   \r\n",__FUNCTION__,__LINE__);\
      printf(" Addr = 0x%x, Data Begin = 0x%x, Data End = 0x%x    \r\n",addr,data_start,data_end);\
    }\
  }\
  else\
  {\
    uint32_t valid_bytes;\
    bytes_pulled = 0;\
    while(num_bytes)\
    {\
      max_data = pkt_mbuf_max_data_size(data_ref.buf);\
      /*valid_bytes = data_ref.buf->count - (data_ref.ptr - data_ref.buf->data.ptr);*/\
      valid_bytes = max_data - (data_ref.ptr - data_ref.buf->start_of_data);\
      if(num_bytes <= valid_bytes) \
      {\
        data_ref.ptr += num_bytes;\
        bytes_pulled += num_bytes;\
        if(num_bytes == valid_bytes)\
        {\
          data_ref.buf = data_ref.buf->sg.next;\
          data_ref.ptr = data_ref.buf->data.ptr;\
        }\
        break;\
      }\
      else \
      {\
        num_bytes -= valid_bytes;\
        bytes_pulled += valid_bytes;\
        if(data_ref.buf->sg.next == data_ref.buf->sg.head/*NULL*/) \
        {\
          printf("%s:%d Panic!!! Data reference going beyond valid data!!!   \r\n",__FUNCTION__,__LINE__);\
          bytes_pulled = 0; \
          break; \
        } \
        data_ref.buf = data_ref.buf->sg.next;\
        data_ref.ptr = data_ref.buf->data.ptr;\
      } \
   }\
 }\
}

/* __pkt_mbuf_pull_data(struct pkt_mbuf *mbuf, uint32_t num_bytes,OUT struct pkt_mbuf_data_ref Data, OUT uint32_t bytes_pulled) */
/** \ingroup PBUFMACROS
#define __pkt_mbuf_pull_data(mbuf,num_bytes,bytes_pulled)

\n<b>Description:</b>\n
  This macro advances the current Data Reference in PSPBuf by given bytes.
  Before calling the macro, user needs to make sure that PSPBuf has enough data to pull.
  It decrements the data count by number of bytes pulled
  After the macro, user may check if all the bytes are pulled ('bytes_pulled' is equal to 'num_bytes')
  If 'bytes_pulled' is zero, data_ref returned is NOT valid.
  <b>Input Parameters:</b>\n
  <i> mbuf :</i> pointer to PSPBuf structure.
  <i> num_bytes : </i> Number of Bytes to be pulled

  <b>Output Parameters</b>\n
  <i> bytes_pulled : </i> Number of Bytes actually pulled

  <b>Returns</b>\n
  <i> None </i>
*/
#define __pkt_mbuf_pull_data(mbuf,num_bytes,bytes_pulled)\
{\
  struct pkt_mbuf_data_ref temp_data = mbuf->data;\
  __pkt_mbuf_pull_data_ref(mbuf,temp_data,num_bytes,bytes_pulled);\
  mbuf->data = temp_data;\
  mbuf->count -= bytes_pulled;\
}



/** \ingroup PBUFMACROS
#define __pkt_mbuf_read_data_ext(mbuf,data_ref,num_bytes,user_data,bytes_read)

\n<b>Description:</b>\n
  This macro reads specified number of bytes from the given data reference and advances the reference.
  Current Data reference in PSPBuf is NOT modified.
  Before calling the macro, user needs to make sure that PSPBuf has enough data to read.
  After the macro, user may check if all the bytes are read ('bytes_read' is equal to 'num_bytes')
  <b>Input Parameters:</b>\n
  <i> mbuf :</i> pointer to PSPBuf structure.
  <i> data_ref :</i> data reference from where data needs to be read.
  <i> num_bytes :</i> Number of bytes to read.
  <i> user_data : </i> Destination data pointer where data is copied

  <b>Output Parameters</b>\n
  <i> bytes_read: </i> Number of bytes actually read

  <b>Returns</b>\n
  <i> None </i>
*/
#define __pkt_mbuf_read_data_ext(mbuf,data_ref,num_of_bytes,user_data,bytes_read) \
{\
  uint32_t num_bytes = num_of_bytes;\
  bytes_read = 0;\
  if((pkt_mbuf_is_sg_valid(mbuf) != TRUE))\
  { \
   uint32_t addr,data_start,data_end ;\
   addr      = (uint32_t)data_ref.ptr + num_bytes ; \
   data_start = (uint32_t)(mbuf->data.ptr);\
   data_end   = (uint32_t)(mbuf->data.ptr) + pkt_mbuf_max_data_size(mbuf) - 1 ;\
   \
    if((addr >= data_start) && (addr <=  data_end))    \
    {\
      memcpy(user_data,data_ref.ptr,num_bytes);\
      data_ref.ptr += num_bytes;\
      bytes_read += num_bytes;\
    } \
    else\
    {\
      printf("%s:%d Panic!!! Data reference going beyond valid data!!!   \r\n",__FUNCTION__,__LINE__);\
      printf("\r\n Addr = 0x%x, Data Begin = 0x%x, Data End = 0x%x    \r\n",addr,data_start,data_end);\
    }\
  }\
  else \
  {\
    uint32_t valid_bytes;\
    while(num_bytes) \
    {\
      valid_bytes = data_ref.buf->count - (data_ref.ptr - data_ref.buf->data.ptr);\
      if(num_bytes < valid_bytes) \
      {\
	memcpy(user_data,data_ref.ptr,num_bytes);\
	data_ref.ptr += num_bytes;\
    bytes_read += num_bytes;\
	break;\
      }\
      else \
      {\
        memcpy(user_data,data_ref.ptr,valid_bytes);\
	data_ref.ptr += valid_bytes;\
        bytes_read += valid_bytes;\
        num_bytes -= valid_bytes;\
        if(data_ref.buf->sg.next == data_ref.buf->sg.head /*NULL*/)\
        {\
          printf("%s:%d Panic!!! Data reference going beyond valid data!!!   \r\n",__FUNCTION__,__LINE__);\
          break;\
        }\
        data_ref.buf = data_ref.buf->sg.next;\
        data_ref.ptr = data_ref.buf->data.ptr;\
      }\
    }\
  }\
}

/** \ingroup PBUFMACROS
#define __pkt_mbuf_read_data(mbuf,num_bytes,user_data,bytes_read)

\n<b>Description:</b>\n
  This macro reads specified number of bytes from the current data reference in PSPBuf.
  Current Data Reference in PSPBuf is advanced by number of bytes read.
  Before calling the macro, user needs to make sure that PSPBuf has enough data to read.
  After the macro, user may check if all the bytes are read ('bytes_read' is equal to 'num_bytes')
  <b>Input Parameters:</b>\n
  <i> mbuf : </i> pointer to PSPBuf structure.
  <i> num_bytes: </i> number of bytes to read.
  <i> user_data : </i> Destination data pointer where data is copied

  <b>Output Parameters</b>\n
  <i> bytes_read: </i> Number of bytes actually read

  <b>Returns</b>\n
  <i> None </i>
*/
#define __pkt_mbuf_read_data(mbuf,num_bytes,user_data,bytes_read) \
{\
  struct pkt_mbuf_data_ref temp_data;\
  temp_data = mbuf->data;\
  __pkt_mbuf_read_data_ext(mbuf, temp_data,num_bytes,user_data,bytes_read); \
  mbuf->data = temp_data;\
}



/** \ingroup PBUFMACROS
#define __pkt_mbuf_get_data_ext(mbuf,data_ref,num_bytes,user_data,bytes_read)

\n<b>Description:</b>\n
  This macro copies specified number of bytes from the given data reference without updating the data reference.
  Data pointer in PSPBuf is also NOT changed.
  Before calling the macro, user needs to make sure that PSPBuf has enough data to read.
  After the macro, user may check if all the bytes are read ('bytes_read' is equal to 'num_bytes')
  <b>Input Parameters:</b>\n
  <i> mbuf :</i> pointer to PSPBuf structure.
  <i> data_ref :</i> data reference from where data needs to be read.
  <i> num_bytes: </i> number of bytes to read.
  <i> user_data : </i> Destination data pointer where data is copied

  <b>Output Parameters</b>\n
  <i> bytes_read: </i> Number of bytes actually read

  <b>Returns</b>\n
  <i> None </i>
*/
#define __pkt_mbuf_get_data_ext(mbuf,data_ref,num_bytes,user_data,bytes_read) \
{\
  struct pkt_mbuf_data_ref orig_user_data;\
  orig_user_data = data_ref;\
  __pkt_mbuf_read_data_ext(mbuf,data_ref,num_bytes,user_data,bytes_read); \
  data_ref = orig_user_data;\
}

/** \ingroup PBUFMACROS
#define __pkt_mbuf_get_data(mbuf,num_bytes,user_data,bytes_read)

\n<b>Description:</b>\n
  This macro copies specified number of bytes from the current data pointer in PSPBuf.
  Data pointer in PSPBuf is NOT changed.
  Before calling the macro, user needs to make sure that PSPBuf has enough data to read.
  After the macro, user may check if all the bytes are read ('bytes_read' is equal to 'num_bytes')
  <b>Input Parameters:</b>\n
  <i> mbuf :</i> pointer to PSPBuf structure.
  <i> num_bytes: </i> number of bytes to read.
  <i> user_data : </i> Destination data pointer where data is copied

  <b>Output Parameters</b>\n
  <i> bytes_read: </i> Number of bytes actually read

  <b>Returns</b>\n
  <i> None </i>
*/
#define __pkt_mbuf_get_data(mbuf,num_bytes,user_data,bytes_read) \
{\
  struct pkt_mbuf_data_ref temp_data;\
  temp_data = mbuf->data;\
  __pkt_mbuf_read_data_ext(mbuf, temp_data,num_bytes,user_data,bytes_read); \
}


/** \ingroup PBUFMACROS
#define __pkt_mbuf_read_data_at_offset(mbuf, offset, num_bytes,user_data,bytes_read)

\n<b>Description:</b>\n
  This macro copies specified number of bytes starting from the given offset in PSPBuf.
  Current Data Reference in PSPBuf is advanced by number of bytes read.
  Before calling the macro, user needs to make sure that PSPBuf has enough data to read.
  After the macro, user may check if all the bytes are read ('bytes_read' is equal to 'num_bytes')
  <b>Input Parameters:</b>\n
  <i> mbuf :</i> pointer to PSPBuf structure.
  <i> offset :</i> offset from current data pointer.
  <i> user_data : </i> Destination data pointer where data is copied

  <b>Output Parameters</b>\n
  <i> bytes_read: </i> Number of bytes actually read

  <b>Returns</b>\n
  <i> None </i>
*/
#define __pkt_mbuf_read_data_at_offset(mbuf, offset, num_bytes,user_data,bytes_read) \
{\
  struct pkt_mbuf_data_ref temp_data;\
  uint32_t bytes_valid;\
  __pkt_mbuf_get_data_ref_at_offset( mbuf, offset, temp_data, bytes_valid); \
  __pkt_mbuf_read_data_ext(mbuf, temp_data,num_bytes,user_data,bytes_read); \
  mbuf->data = temp_data;\
}


/** \ingroup PBUFMACROS
#define __pkt_mbuf_get_data_at_offset(mbuf, offset, num_bytes,user_data,bytes_read)

\n<b>Description:</b>\n
  This macro copies specified number of bytes starting from the given offset in PSPBuf.
  Current Data Reference in PSPBuf is NOT modified.
  Before calling the macro, user needs to make sure that PSPBuf has enough data to read.
  After the macro, user may check if all the bytes are read ('bytes_read' is equal to 'num_bytes')
  <b>Input Parameters:</b>\n
  <i> mbuf :</i> pointer to PSPBuf structure.
  <i> offset :</i> offset from current data pointer.
  <i> num_bytes: </i> number of bytes to read.
  <i> user_data : </i> Destination data pointer where data is copied

  <b>Output Parameters</b>\n
  <i> bytes_read: </i> Number of bytes actually read

  <b>Returns</b>\n
  <i> None </i>
*/
#define __pkt_mbuf_get_data_at_offset(mbuf, offset, num_bytes,user_data,bytes_read) \
{\
  struct pkt_mbuf_data_ref temp_data;\
  uint32_t bytes_valid;\
  __pkt_mbuf_get_data_ref_at_offset( mbuf, offset, temp_data, bytes_valid); \
  __pkt_mbuf_read_data_ext(mbuf, temp_data,num_bytes,user_data,bytes_read); \
}


/** \ingroup PBUFMACROS
#define __pkt_mbuf_write_data_ext(mbuf,data_ref,user_data,num_bytes)

\n<b>Description:</b>\n
  This macro writes user data to PSPBuf at location pointed by the given user data pointer.
  User Data pointer is advanced by number of bytes written.
  Current  data pointer in PSPBuf is not modified .
  Before calling the macro, user needs to make sure that PSPBuf has enough space to write.
  <b>Input Parameters:</b>\n
  <i> mbuf :</i> pointer to PSPBuf structure.
  <i> data_ref :</i> desination data reference where data is written.
  <i> user_data : </i> Destination data pointer where data is copied
  <i> num_bytes: </i> number of bytes to write.

  <b>Output Parameters</b>\n
  <i> None </i>

  <b>Returns</b>\n
  <i> None </i>
*/
#define __pkt_mbuf_write_data_ext(mbuf,data_ref,user_data,num_of_bytes) \
{\
  uint32_t num_bytes = num_of_bytes;\
  uchar8_t *write_data = user_data;\
  if((pkt_mbuf_is_sg_valid(mbuf) != TRUE))\
  { \
   uint32_t addr,data_start,data_end ;\
   addr      = (uint32_t)data_ref.ptr + num_bytes -1 ; \
   data_start = (uint32_t)(mbuf->start_of_data);\
   data_end   = (uint32_t)(mbuf->data.ptr) + pkt_mbuf_max_data_size(mbuf) - 1;\
    if((addr >= data_start) && (addr <=  data_end))    \
    {\
      memcpy(data_ref.ptr,write_data,num_bytes);\
      data_ref.ptr += num_bytes;\
    } \
    else\
    {\
      printf("%s:%d Panic!!! Data reference going beyond valid data!!!   \r\n",__FUNCTION__,__LINE__);\
      printf(" Addr = 0x%x, Data Begin = 0x%x, Data End = 0x%x    \r\n",addr,data_start,data_end);\
    }\
  }\
  else \
  {\
    uint32_t valid_bytes;\
    while(num_bytes) \
    {\
      valid_bytes = data_ref.buf->count - (data_ref.ptr - data_ref.buf->data.ptr);\
      printf("%s:%d SG case : valid bytes to write = %u   \r\n",__FUNCTION__,__LINE__,valid_bytes);\
      if(num_bytes < valid_bytes) \
      {\
	memcpy(data_ref.ptr,write_data,num_bytes);\
        printf("%s:%d LAST NODE, Written Data:  \r\n",__FUNCTION__,__LINE__);\
        __printchar(data_ref.ptr,num_bytes);\
        data_ref.ptr += num_bytes;\
        if(num_bytes == valid_bytes)\
        {\
          printf("%s:%d moving to Next SG NODE \r\n",__FUNCTION__,__LINE__);\
          data_ref.buf = data_ref.buf->sg.next;\
          data_ref.ptr = data_ref.buf->start_of_data;\
        }\
	break;\
      }\
      else \
      {\
        memcpy(data_ref.ptr,write_data,valid_bytes);\
        printf("%s:%d SG NODE Written Data  \r\n",__FUNCTION__,__LINE__);\
        __printchar(data_ref.ptr,valid_bytes);\
        write_data     += valid_bytes;\
        num_bytes -= valid_bytes;\
        if(data_ref.buf->sg.next == data_ref.buf->sg.head/*NULL*/)\
        {\
          printf("%s:%d Panic!!! PSPBuf has no space to write!!!   \r\n",__FUNCTION__,__LINE__);\
          break;\
        }\
        data_ref.buf = data_ref.buf->sg.next;\
        data_ref.ptr = data_ref.buf->start_of_data;\
      }\
    }\
  }\
}


/** \ingroup PBUFMACROS
#define __pkt_mbuf_write_data(mbuf,write_data,num_bytes)

\n<b>Description:</b>\n
 This macro writes user data to PSPBuf at current data pointer.
 Current  data pointer in PSPBuf is advanced .
 Before calling the macro, user needs to make sure that PSPBuf has enough space to write.
  <b>Input Parameters:</b>\n
  <i> mbuf :</i> pointer to PSPBuf structure.
  <i> write_data : </i> Source data pointer from where data is copied
  <i> num_bytes: </i> number of bytes to read.

  <b>Output Parameters</b>\n
  <i> None </i>

  <b>Returns</b>\n
  <i> None </i>
*/
#define __pkt_mbuf_write_data(mbuf,write_data,num_bytes) \
{\
  struct pkt_mbuf_data_ref data = mbuf->data;\
  __pkt_mbuf_write_data_ext(mbuf,data,write_data,num_bytes)\
 mbuf->data = data;\
}


/** \ingroup PBUFMACROS
#define __pkt_mbuf_put_data(mbuf,data_ref,write_data,num_bytes)

\n<b>Description:</b>\n
  This macro writes specified number of bytes starting from the given data pointer.
  Data pointer is NOT changed.
  Before calling the macro, user needs to make sure that PSPBuf has enough space to write.
  <b>Input Parameters:</b>\n
  <i> mbuf :</i> pointer to PSPBuf structure.
  <i> data_ref :</i> desination data reference where data is written.
  <i> write_data : </i> Source data pointer from where data is copied
  <i> num_bytes: </i> number of bytes to read.

  <b>Output Parameters</b>\n
  <i> None </i>

  <b>Returns</b>\n
  <i> None </i>
*/
#define __pkt_mbuf_put_data(mbuf,data_ref,write_data,num_bytes) \
{\
  struct pkt_mbuf_data_ref orig_user_data,orig_buf_data;\
  orig_buf_data = mbuf->data;\
  orig_user_data = data_ref;\
  __pkt_mbuf_write_data_ext(mbuf, data_ref,write_data,num_bytes); \
  data_ref = orig_user_data;\
  mbuf->data = orig_buf_data;\
 }



/** \ingroup PBUFMACROS
#define __pkt_mbuf_get_data_count(data_ref,num_bytes)

\n<b>Description:</b>\n
  This macro returns number of contiguous bytes available from given data reference
  <b>Input Parameters:</b>\n
  <i> data_ref :</i> data reference from where valid data is counted .

  <b>Output Parameters</b>\n
  <i> None </i>

  <b>Returns</b>\n
  <i> None </i>
*/
#define __pkt_mbuf_get_data_count(data_ref,num_bytes) \
{\
  num_bytes = data_ref.buf->count - (data_ref.ptr - data_ref.buf->data.ptr);\
}
/** \ingroup PBUFMACROS

  \n<b>Description:</b>\n
  This macro returns number of bytes available in the packet from given data pointer
  <b>Input Parameters:</b>\n
  <i> mbuf :</i> pointer to PSPBuf structure.
  <i> data_ref :</i> data reference from where valid data is counted .

  <b>Output Parameters</b>\n
  <i> num_bytes: </i> number of valid bytes available in the packet from given data reference.

  <b>Returns</b>\n
  <i> None </i>
*/
#define __pkt_mbuf_get_pkt_data_count(mbuf, data_ref,num_bytes) \
{\
  struct mbuf *buf;\
  num_bytes = data_ref.buf->count - (data_ref.ptr - data_ref.buf->data.ptr);\
  buf = data_ref.buf->sg.next;\
  while(buf != mbuf->sg.head)\
  {\
    num_bytes += buf->count;\
    buf = buf->sg.next;\
  }\
}


/** \ingroup PBUFMACROS
#define __pkt_mbuf_push_data_ref(mbuf,data_ref,num_bytes,bytes_pushed) \
\n<b>Description:</b>\n
  This macro pushes given data reference by given bytes and returns reference .
  Current data reference in PSPBuf is NOT moved.
  Data count is also NOT modified
  Before calling the macro, user needs to make sure that PSPBuf has enough data to push.
  After the macro, user may check if all the bytes are pushed ('bytes_pushed' is equal to 'num_bytes')
  If 'bytes_pushed' is zero, data_ref returned is NOT valid.
  <b>Input Parameters:</b>\n
  <i> mbuf :</i> pointer to PSPBuf structure.
  <i> data_ref :</i> data reference from where data is pushed.
  <i> num_bytes :</i> Number of bytes to be pushed.

  <b>Output Parameters</b>\n
  <i> bytes_pushed:  </i> Number of bytes actually pushed

  <b>Returns</b>\n
  <i> None </i>
*/
#define __pkt_mbuf_push_data_ref(mbuf,data_ref,num_of_bytes,bytes_pushed) \
 {\
  uint32_t addr,data_start,data_end ;\
  uint32_t num_bytes = num_of_bytes;\
  if((pkt_mbuf_is_sg_valid(mbuf) != TRUE))\
  {\
   addr      = (uint32_t)data_ref.ptr - num_bytes ;\
   data_start = (uint32_t)(mbuf->start_of_data);\
   data_end   = (uint32_t)(mbuf->start_of_data) + pkt_mbuf_max_data_size(mbuf) - 1;\
   if((addr >= data_start) && (addr <=  data_end))    \
   {\
     data_ref.ptr -= num_bytes;\
     bytes_pushed = num_bytes; \
     pkt_mbuf_debug("%s:%d   bytes_pushed  = %u \r\n",__FUNCTION__,__LINE__,bytes_pushed);\
   }\
   else\
   {\
     printf("%s:%d NON SG Panic!!! Data reference going beyond valid data!!!   \r\n",__FUNCTION__,__LINE__);\
     printf(" Addr = 0x%x, Data Begin = 0x%x, Data End = 0x%x    \r\n",addr,data_start,data_end);\
   }\
 }\
 else\
 {\
   uint32_t valid_bytes;\
   bytes_pushed = 0;\
   while(num_bytes)\
   {\
     valid_bytes =  (data_ref.ptr - data_ref.buf->start_of_data);\
     pkt_mbuf_debug("%s:%d SG case DataStart = %p, Data ptr = %p \r\n",__FUNCTION__,__LINE__,data_ref.buf->start_of_data,data_ref.ptr);\
     pkt_mbuf_debug("%s:%d SG case  validBytes = %u \r\n",__FUNCTION__,__LINE__,valid_bytes);\
     pkt_mbuf_debug("%s:%d   numBytes = %u \r\n",__FUNCTION__,__LINE__,num_bytes);\
     if(num_bytes <= valid_bytes) \
     {\
       data_ref.ptr -= num_bytes;\
       bytes_pushed += num_bytes;\
       break;\
     }\
     else \
     {\
       num_bytes -= valid_bytes;\
       bytes_pushed += valid_bytes;\
       if(data_ref.buf->sg.prev == data_ref.buf/*NULL*/) \
       {\
         printf("%s:%d Panic!!! Data reference going beyond valid data!!!   \r\n",__FUNCTION__,__LINE__);\
         break; \
       } \
       data_ref.buf = data_ref.buf->sg.prev;\
       pkt_mbuf_debug("%s:%d   Moving to Prev SG  = %p \r\n",__FUNCTION__,__LINE__,data_ref.buf);\
       data_ref.ptr = data_ref.buf->end.ptr;\
     }\
   }\
 }\
}


/** \ingroup PBUFMACROS
#define __pkt_mbuf_push_data(mbuf, num_bytes, data_ref, bytes_pushed)

\n<b>Description:</b>\n
 This macro pushes the current data Reference in PSPBuf by given bytes and returns the updated reference.
 Before calling the macro, user needs to make sure that PSPBuf has enough data to push.
 It increments the data count by number of bytes pushed
 After the macro, user may check if all the bytes are pushed
 If 'bytes_pushed' is zero, data_ref returned is NOT valid.
  <b>Input Parameters:</b>\n
  <i> mbuf :</i> pointer to PSPBuf structure.
  <i> num_bytes :</i> Number of bytes to be pushed.

  <b>Output Parameters</b>\n
  <i> data_ref :</i> data reference after the data is pushed.

  <b>Returns</b>\n
  <i> None </i>
*/
#define __pkt_mbuf_push_data(mbuf, num_bytes, data_ref, bytes_pushed) \
{\
 struct pkt_mbuf_data_ref temp_data;\
 temp_data = mbuf->data;\
 __pkt_mbuf_push_data_ref(mbuf, temp_data,num_bytes,bytes_pushed); \
 data_ref = mbuf->data = temp_data;\
 mbuf->count += bytes_pushed; \
}

#define __printchar(cp,cnt)\
{\
  int ii;\
  /*printf(" DATAPOINTER = %p, Data = \r\n",cp );*/\
  for(ii = 0;ii<cnt;ii++)\
    printf(" %c",cp[ii]  );\
  /*printf(" \r\n");*/\
}

extern struct pkt_mbuf_pool *pPSPBufPoolList_g;
/* Find a pool which can hold data size of at least 'ulDataSize' */
PSP_PRIVATE inline struct pkt_mbuf_pool *pkt_mbuf_get_bufpool(uint32_t data_size)
{
  struct pkt_mbuf_pool *buf_pool;
  struct pkt_mbuf_pool *match_pool;

  buf_pool = pPSPBufPoolList_g;
  match_pool = NULL;
  while(buf_pool)
  {
    if( data_size > buf_pool->data_size)
    {
      match_pool = buf_pool;
      buf_pool = buf_pool->next;
    }
    else
    {
      return(buf_pool);
    }
  }
  return match_pool;
}

/** \ingroup PBUFMACROS

\n<b>Description:</b>\n
#define pkt_mbuf_get_data_offset(mbuf,data_ref,offset) \
This macro gives offset from SoD to the given data reference.
  <b>Input Parameters:</b>\n
  <i> mbuf :</i> pointer to PSPBuf structure.
  <i> data_ref :</i> data reference for which Offset is needed.

  <b>Output Parameters</b>\n
  <i> offset: </i> offset from SoD to the given data reference

  <b>Returns</b>\n
  <i> None </i>
*/
#define pkt_mbuf_get_data_offset(mbuf,data_ref,offset) \
{\
  struct pkt_mbuf *buf;\
  offset = 0;\
  uint32_t addr,start,max_data;\
  addr  = (uint32_t)data_ref.ptr;\
  \
  if(!(pkt_mbuf_is_sg_valid(mbuf)))\
  {\
   uint32_t data_start,data_end ;\
   data_start = (uint32_t)(mbuf->start_of_data);\
   data_end   = data_start + pkt_mbuf_max_data_size(mbuf) - 1;\
    if((addr < data_start) ||   (addr > data_end ))\
    {\
      offset = PSP_FAILURE;\
      printf("%s:%d Panic!!! Offset is going beyond valid data!!!   \r\n",__FUNCTION__,__LINE__);\
      printf(" Addr = 0x%x, Data Begin = 0x%x, Data End = 0x%x    \r\n",addr,data_start,data_end);\
    }\
    else \
    {\
     offset = (addr - data_start);\
    }\
  }\
  \
  /* Scatter Gather list */\
  else\
  {\
    buf = mbuf;\
    start = (uint32_t)buf->start_of_data;\
    while(1)\
    {\
      max_data = pkt_mbuf_max_data_size(buf);\
      printf(" Addr = 0x%x, Data Begin = 0x%x, MaxData = 0x%x    \r\n",addr,start,max_data);\
      if((addr < start) ||\
	 (addr >=  (start + max_data) ))\
      {\
        offset +=  max_data ;\
        if(buf->sg.next == buf->sg.head)\
        {\
          offset = PSP_FAILURE;\
          printf("%s:%d Panic!!! Data reference going beyond valid data!!!   \r\n",__FUNCTION__,__LINE__);\
          break;\
        }\
        buf = buf->sg.next;\
        start = (uint32_t)buf->start_of_data;\
      }\
      else\
      {\
       offset +=  (addr - ((uint32_t)buf->data.ptr));\
       /*offset +=  ( max_data - (  addr - ((uint32_t)buf->data.ptr)));*/\
       break;\
      }\
    }\
  }\
}
/** \ingroup PBUFMACROS
#define pkt_mbuf_get_data_data_ref_ad_sod_offset( mbuf, offset, data_ref, bytes_valid) \

\n<b>Description:</b>\n
 This Macro gives data_ref at given offset from SoD
  <b>Input Parameters:</b>\n
  <i> mbuf :</i> pointer to PSPBuf structure.
  <i> offset: </i> offset from SoD to the given data reference

  <b>Output Parameters</b>\n
  <i> data_ref:  </i> reference to the data at given offset

  <b>Returns</b>\n
  <i> None </i>
*/
#define pkt_mbuf_get_data_data_ref_ad_sod_offset( mbuf, offset, data_ref, bytes_valid) \
{\
  uint32_t addr;\
  if((pkt_mbuf_is_sg_valid(mbuf) != TRUE))\
  {\
   uint32_t addr,data_start,data_end ;\
   addr = (uint32_t)mbuf->start_of_data + offset ;\
   data_start = (uint32_t)(mbuf->start_of_data);\
   data_end   = (uint32_t)(mbuf->start_of_data) + pkt_mbuf_max_data_size(mbuf);\
   if((addr>=data_start) && (addr<data_end))    \
   {\
        data_ref.ptr = (uchar8_t *)addr;		\
	       data_ref.buf  = mbuf; \
        bytes_valid = pkt_mbuf_max_data_size(mbuf) - offset; \
   }\
   else\
   {\
     pkt_mbuf_debug("\r\n%s:%d Panic!!! Offset is beyond valid data!!!   \r\n",__FUNCTION__,__LINE__);\
     pkt_mbuf_debug(" Addr = 0x%x, Data Begin = 0x%x, Data End = 0x%x    \r\n",addr,data_start,data_end);\
   }\
  }\
  else \
  {\
   uint32_t num_bytes;\
   struct pkt_mbuf *buf = mbuf;\
   num_bytes = offset; \
   addr = (uint32_t)buf->start_of_data;\
   do \
   {\
     if(num_bytes < pkt_mbuf_max_data_size(buf))	\
     {\
       data_ref.ptr = (uchar8_t *)(addr + num_bytes);	\
       data_ref.buf =  buf;\
       bytes_valid = pkt_mbuf_max_data_size(buf) - num_bytes; \
       break; \
     }\
     else \
     {\
       num_bytes -= pkt_mbuf_max_data_size(buf);\
       if(data_ref.buf->sg.next == data_ref.buf->sg.head/*NULL*/)\
       { \
         pkt_mbuf_debug("%s:%d Panic!!! Offset is beyond valid data!!!   \r\n",__FUNCTION__,__LINE__);\
  	 bytes_valid = 0; \
	 break;\
       } \
     }\
     buf = buf->sg.next;\
     addr = (uint32_t)buf->start_of_data;		\
   }while(num_bytes);\
 }\
}

/**************** Enumerations ***************/


#ifdef CONFIG_PKT_MBUF_DEBUG_PKTALLOC
#define pkt_mbuf_alloc(a,b) ({\
      struct pkt_mbuf *p; \
     /* p = __pkt_mbuf_alloc(a,b);*/\
	p=cntlr_instnace.pktbuf_cbks->pktbuf_alloc(a,b);\
      if(p)\
        printf("%s:%d - %s - BUF ALLOC %p (data %p) dsize %u count %u\n", __FILE__,__LINE__,__FUNCTION__,p, p->data.ptr, a, p->count);\
      else\
        printf("%s:%d - %s - BUF ALLOC FAILED dsize %u\n", __FILE__,__LINE__,__FUNCTION__, a);\
      p;\
  })


#define pkt_mbuf_free(p) ({\
      if(p)\
        printf("%s:%d - %s - BUF FREE %p (data %p count %u)\n", __FILE__,__LINE__,__FUNCTION__,p, (p)->data.ptr, (p)->count); \
      else\
        printf("%s:%d - %s - BUF FREE (NULL pkt)\n", __FILE__,__LINE__,__FUNCTION__);\
     /* __pkt_mbuf_free(p);*/\
	cntlr_instnace.pktbuf_cbks->pktbuf_free(mbuf);\
  })
#else
//#define pkt_mbuf_alloc __pkt_mbuf_alloc
#define pkt_mbuf_alloc   pktbuf_cbks->pktbuf_alloc
#define pkt_mbuf_free   pktbuf_cbks->pktbuf_free
#define pkt_mbuf_free_chain __pkt_mbuf_free_chain
#endif




#ifdef CONFIG_PKT_MBUF_DEBUG_PKTPATH
#define pkt_mbuf_mark_path(pkt,str) do { \
  int len; \
  if(!pkt->pPktPath) pkt->pPktPath = pkt->cPktPath; \
  len = strlen(str); \
  if((pkt->pPktPath+len+1) < (pkt->cPktPath+sizeof(pkt->cPktPath))) \
  { \
    sprintf(pkt->pPktPath,"-%s",str); \
    pkt->pPktPath += (len+1); \
  } \
  else \
  { \
    pkt_mbuf_debug("No room to mark path '%s'\n", str); \
  } \
} while(0)
#else
#define pkt_mbuf_mark_path(pkt,str) do {} while(0)
#endif

static inline void pkt_mbuf_reset_parse_state(struct pkt_mbuf *buf)
{
  pkt_mbuf_flag_reset(buf,PKT_MBUF_NW_INFO_VALID);
  pkt_mbuf_flag_reset(buf,PKT_MBUF_XPORT_INFO_VALID);
  pkt_mbuf_flag_reset(buf,PKT_MBUF_SESSION_HASH_VALID);
}

static inline void pkt_mbuf_parse_ipv4_tuple(struct pkt_mbuf *pkt)
{
  pkt->l3_proto = pkt->data.ptr[9]; /* protocol */
  pkt->nw_tuple.ip4.dst = pkt_mbuf_ipv4_dst_ip(pkt);
  pkt->nw_tuple.ip4.src = pkt_mbuf_ipv4_src_ip(pkt);
  pkt->nw_tuple.ip4.tos = pkt_mbuf_ipv4_tos(pkt);
  pkt->nw_tuple.ip4.total_len = pkt_mbuf_ipv4_total_len(pkt);
  pkt->flags |= PKT_MBUF_NW_INFO_VALID;
}

static inline void pkt_mbuf_parse_xport_tuple(struct pkt_mbuf *pkt)
{
  switch (pkt->l3_proto)
  {
    case PSP_IPPROTO_UDP:
    case PSP_IPPROTO_TCP:
    case PSP_IPPROTO_SCTP:
      pkt->xport_tuple.src_port = pkt_mbuf_get_16bits(pkt_mbuf_xport_hdr_ptr(pkt));
      pkt->xport_tuple.dst_port = pkt_mbuf_get_16bits(pkt_mbuf_xport_hdr_ptr(pkt) + 2);
      break;
    case PSP_IPPROTO_ESP:
      pkt->xport_tuple.spi = PKT_MBUF_ESP_SPI(pkt);
      break;
    case PSP_IPPROTO_ICMP:
      pkt->xport_tuple.icmp_type = *(pkt_mbuf_xport_hdr_ptr(pkt));
      pkt->xport_tuple.icmp_code = *(pkt_mbuf_xport_hdr_ptr(pkt) + 1);
      break;
    case PSP_IPPROTO_AH:
      pkt->xport_tuple.spi = PKT_MBUF_AH_SPI(pkt);
      break;
    case PSP_IPPROTO_MOBILE_IP:
      pkt->xport_tuple.mhn_ext_proto = *(pkt_mbuf_xport_hdr_ptr(pkt));
      pkt->xport_tuple.mh_type = *(pkt_mbuf_xport_hdr_ptr(pkt) + 2);
      break;
    default:
      pkt->xport_tuple.val = 0;
  }
  pkt->flags |= PKT_MBUF_XPORT_INFO_VALID;
}

static inline void pkt_mbuf_parse_ipv4_tuple_ex(struct pkt_mbuf *buf)
{
  if(!(buf->flags & PKT_MBUF_NW_INFO_VALID)) pkt_mbuf_parse_ipv4_tuple(buf);
}

static inline void pkt_mbuf_parse_xport_tuple_ex(struct pkt_mbuf *buf)
{
  if(!(buf->flags & PKT_MBUF_XPORT_INFO_VALID)) pkt_mbuf_parse_xport_tuple(buf);
}



#ifdef NOT_USED

#define PSPBUF_IPV4_PAYLOAD_PTR(x)   \
            (pkt_mbuf_ipv4_hdr_ptr(x) + PSP_IPV4_HDR_LEN(x))

#define PSPBUF_IPV4_OPT_PTR(x)       \
             (pkt_mbuf_ipv4_hdr_ptr(x) + PKT_MBUF_IPV4_HDR_MIN_LEN)

#define __pkt_mbuf_get_phy_addr_at_data_offset(mbuf,uiOffset)\
    (pkt_mbuf_get_pkt_phy_addr(mbuf) + (mbuf->data.ptr + uiOffset) - mbuf->start_of_data)

#define __pkt_mbuf_get_phy_addr_at_current_data_ptr(buf)\
    (buf->buf_phy_addr+pkt_mbuf_hw_area_size()+(buf->data.ptr-((uchar8_t*)buf)))

/* This macro advances the given PSPBuf Data Reference by given bytes and returns the updated reference.
 * Current data reference in PSPBuf is also advanced.
 * Before calling the macro, user needs to make sure that PSPBuf has enough data to pull.
 * After the macro, user may check if all the bytes are pulled ('bytes_pulled' is equal to 'num_bytes')
 * If 'bytes_pulled' is zero, data_ref returned is NOT valid.
 */

//INLINE void  __PSPBufPullData_ext(struct pkt_mbuf *pkt_mbuf, struct pkt_mbuf_data_ref data_ref,uint32_t num_bytes, uint32_t bytes_pulled)
/* __PSPBufPullData_ext(struct pkt_mbuf *mbuf, struct pkt_mbuf_data_ref data_ref,uint32_t num_bytes, OUT uint32_t bytes_pulled) */
#define __PSPBufPullData_ext(mbuf, data_ref,num_bytes,bytes_pulled) \
{\
  __pkt_mbuf_pull_data_ref(mbuf, data_ref,num_bytes,bytes_pulled); \
    mbuf->data = data_ref;\
}
#endif




/************************* API Prototypes ************************/
void *pkt_mbuf_pool_create(  uint32_t buf_size, uint32_t num_bufs);

void pkt_mbuf_fill_hdrs_info_in_pkt_mbuf(struct pkt_mbuf *mbuf);

int32_t  pkt_mbuf_reserve_app_scratch_area( uint32_t scratch_area_size);

void  pkt_mbuf_reserve_third_party_area( uint32_t third_party_app_area_size );

struct pkt_mbuf *__pkt_mbuf_alloc( uint32_t data_size,uint32_t flags);

int32_t  __pkt_mbuf_free( struct pkt_mbuf *buf);
int32_t  __pkt_mbuf_free_chain( struct pkt_mbuf *buf);
/*
struct pkt_mbuf*  PSPBufESBAlloc(
                         uchar8_t *pData,
                         uint32_t   uiLength,
                         uint32_t   uiDataBufPhyAddr,
                         void       (*pFreeFn)(void *pFnArg),
                         void       *pFnArg);

int32_t PSPBufGetRdDataPtr(struct pkt_mbuf   *pkt_mbuf,
                             uint32_t   offset,
                             uchar8_t **ppData,
                             uint32_t   *pulSize);
*/

int32_t _pkt_mbuf_free( struct pkt_mbuf *buf);
int32_t pkt_mbuf_get_offset_to_ptr ( struct pkt_mbuf  *mbuf, void    *ptr);

struct pkt_mbuf *pkt_mbuf_flatten(struct pkt_mbuf *mbuf,uint32_t extra_buf_size,uint32_t flags);

struct pkt_mbuf *pkt_mbuf_clone(struct pkt_mbuf *mbuf, uint32_t flags);

int32_t  pkt_mbuf_copy_data(struct pkt_mbuf *dest_buf,struct pkt_mbuf *src_buf, uint32_t bytes_to_copy);


void pkt_mbuf_queue_init(struct dll_queue *buf_queue);
void pkt_mbuf_queue_append(struct dll_queue *buf_queue, struct pkt_mbuf *buf );
uint32_t pkt_mbuf_dup_p(struct pkt_mbuf **buf_p_p,
                    struct pkt_mbuf *buf,
                    uint32_t offset,
                    uint32_t count,
                    uint32_t extra_hdr_size);

void pkt_mbuf_trim(struct dll_queue *buf_queue,
                struct pkt_mbuf **buf,
                uint32_t length);

uint32_t pkt_mbuf_pullup( struct dll_queue *buf_queue,
                      char  *buf,
                      uint32_t cnt);

void pkt_mbuf_free_p( struct dll_queue *buf_queue, struct pkt_mbuf *buf);


//#define OF_PKT_MBUF_DEBUG_STATS 

#ifdef OF_PKT_MBUF_DEBUG_STATS
extern int32_t pkt_mbuf_alloc_stats_g;
extern int32_t pkt_mbuf_free_stats_g;
#endif

#ifdef OF_PKT_MBUF_DEBUG_STATS
#define OF_PKT_MBUF_INC_ALLOC_CNT    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "pkt_mbuf_alloc count %d", ++pkt_mbuf_alloc_stats_g);
#else
#define OF_PKT_MBUF_INC_ALLOC_CNT
#endif 



#ifdef OF_PKT_MBUF_DEBUG_STATS
#define OF_PKT_MBUF_INC_FREE_CNT    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "pkt_mbuf_free count %d", ++pkt_mbuf_free_stats_g);
#else
#define OF_PKT_MBUF_INC_FREE_CNT
#endif 

#define pkt_get_mbuf_ptr_by_skip_area_ptr(msg)  ((uchar8_t*) ((uint32_t)(msg) - (sizeof(struct pkt_mbuf))) - msg->pkt_mbuf_offset )

#define of_pkt_mbuf_free(mbuf) \
{\
        OF_PKT_MBUF_INC_FREE_CNT;\
	/*pkt_mbuf_free(mbuf);*/\
	pktbuf_cbks->pktbuf_free(mbuf);\
}

#define of_get_pkt_mbuf_by_of_msg_and_free(msg)  \
{\
   struct pkt_mbuf *mbuf = NULL;\
	if(msg)\
	{\
		if ( pktbuf_cbks )\
		{\
			mbuf=pkt_get_mbuf_ptr_by_skip_area_ptr(msg); \
			     if(mbuf)\
			     {\
				     OF_PKT_MBUF_INC_FREE_CNT;\
					     pkt_mbuf_free(mbuf);\
			     }\
		}\
		else\
		{\
			msg->desc.free_cbk(msg);\ 
		}\
	}\
}

#define of_check_pkt_mbuf_and_free(msg)\
{\
   struct ofp_header *pheader=NULL;\
   if(msg)\
   {\
      pheader=(struct ofp_header *)(msg->desc.start_of_data);\
         if ((pheader->type == OFPT_PACKET_IN) || (pheader->type == OFPT_PACKET_OUT))\
	   {\
		   if (!pktbuf_cbks)\
		   {\
			   msg->desc.free_cbk(msg);\
		   }\
		   else\
		  {\
				of_get_pkt_mbuf_by_of_msg_and_free(msg);\
		   }\
	   }\
	 else\
	 {\
		 msg->desc.free_cbk(msg);\
	 }\
   }\
}

#define of_alloc_pkt_mbuf_and_set_of_msg(mbuf, msg, msg_type, size, flags, status)\
{\
	status=TRUE;\
	if ( pktbuf_cbks )\
	{\
		mbuf = pktbuf_cbks->pktbuf_alloc( sizeof(struct of_msg) + size,flags);\
		/*mbuf = pkt_mbuf_alloc( sizeof(struct of_msg) + size,flags); */\
		if(!mbuf)\
		{\
			OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "pkt_mbuf_alloc() failed");\
			status = FALSE;\
		}\
		else\
		{\
			OF_PKT_MBUF_INC_ALLOC_CNT;\
			msg=pkt_mbuf_skip_area_ptr(mbuf);\
			msg->pkt_mbuf_offset=0;\
			msg->desc.type            = msg_type;\
			msg->desc.buffer_len      = size + ROUND_UP(size);\
			msg->desc.data_len        = 0;\
			msg->desc.start_of_data   =\
			msg->desc.start_of_buffer = msg->data;\
			msg->desc.free_cbk        = NULL;\
		}\
	}\
	else \
	{\
		msg=ofu_alloc_message(msg_type, size);\
	}\
}

#define of_alloc_set_msg_params( msg, msg_type, size)\
 {\
	   msg->desc.type            = msg_type;\
	   msg->desc.buffer_len      = size + ROUND_UP(size);\
	   msg->desc.data_len        = 0;\
	   msg->desc.start_of_data   =\
		   msg->desc.start_of_buffer = msg->data;\
	   msg->desc.free_cbk        = NULL;\
   }

extern struct pktbuf_callbacks *pktbuf_cbks;
#endif
