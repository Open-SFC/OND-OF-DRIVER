



#if 0
/* igmp macros */
/* offsets from the start of IGMP header */
#define OF_CNTLR_IGMP_HEADER_OFFSET			(OF_CNTLR_IPV4_HDR_OFFSET + 20)			
#define OF_CNTLR_IGMPV3_GROUP_ADDR_OFFSET       12
#define OF_CNTLR_IGMPV2_GROUP_ADDR_OFFSET	4
#define OF_CNTLR_IGMP_TYPE_OFFSET		0
#define OF_CNTLR_IGMP_CHECKSUM_OFFSET		2
#endif



/* igmp message types */
#define IGMPV1_MEMBERSHIP_REPORT			0x12
#define IGMPV2_MEMBERSHIP_REPORT			0x16
#define IGMPV3_MEMBERSHIP_REPORT			0x22
#define IGMP_MEMBERSHIP_QUERY				0x11
#define IGMPV2_LEAVE_GROUP				    0x17
#define IGMP_MAX_RESPONSE_TIME				4	/* in units of  1/10th of sec i.e 400 msec*/
#define IGMP_GRPADDR_ALL_HOSTS              0xE0000001


#define GET_MCAST_MAC_BY_IP(ip, mac)\
{\
        unsigned int prefix = 0x5e0001;\
        memcpy(mac, &prefix, 3);\
        ip &= 0x007FFFFF;\
        mac[3] = (ip & 0x00FF0000) >> 16;\
        mac[4] = (ip & 0x0000FF00) >> 8;\
        mac[5] = (ip & 0x000000FF) >> 0;\
}



