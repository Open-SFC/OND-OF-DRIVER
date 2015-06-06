
/**************************************************************************//**
@File          ip4_mcfwd_api.h

@Description   This file contains the IPv4  Multicast Forward NF APIs, related
                MACROS & data structures
*//***************************************************************************/
#ifndef __FSL_IP4_MCAST_FWD_API_H
#define __FSL_IP4_MCAST_FWD_API_H
#include "nf_infra_api.h"
//#include "nfinfra_nfapi.h"



/* Following macro defines max number of multicast virtual interfaces 
that can be supported in the system. It is a tunable parameter*/

#define IP4_MCFWD_MAX_VIFS    32 

/* Structure for configuring multicast forward group entries */
struct ip4_mcfwd_group{
 uint32_t   groupaddr;/* multicast group address */
 if_id  ifid;  /* interface id */
};


/* structure for configuring multicast virtual interface entries */
struct ip4_mcfwd_vif_entry {
 uint8_t   threshold; /*  TTL threshold */
 uint16_t  vif_type; /*  vif type , ex IP4_MCFWD_VIF_TYPE_TUNNEL for tunnels  */
 uint32_t  vif_id; /* vif_id, index for the vif table */
 uint32_t  local;/* local address for tunnel*/
 uint32_t  remote; /*remote address for tunnel*/
 if_id  link_id; /*interface index */
};

/* structure for configuring multicast forward flow  entries */
struct ip4_mcfwd_mfentry{
  uint32_t mcastgrp; /* multicast group address */
  uint32_t src_ip; /* source address for the packet */
  uint32_t vif_id; /* index of the virtual network device in the vif table over which packets this MF entry should arrive */
  uint32_t minvif; /*minimum vif value, indexes to elements in the ttls list */
  uint32_t maxvif; /* maximum vif value, indexes to elements in the ttls list */
  uint8_t  ttls[IP4_MCFWD_MAX_VIFS]; /* ttls is an array with max vifs entries*/
};

/* IP Multicast Forward stats structure */
struct ip4_mcfwd_stats{
  uint32_t vif_id; /* vif_id, index for the vif table */
  uint32_t ip4_mcast_tot_in_pkts; /*In rcvd pkts */
  uint32_t ip4_mcast_tot_in_bytes; /*in rcvd bytes */
  uint32_t ip4_mcast_tot_out_pkts; /*Out sent pkts */
  uint32_t ip4_mcast_tot_out_bytes; /*Out sent bytes*/
};

/* vif type for multicast virtual interface entries */
enum ip4_mcfwd_vif_type 
{
  IP4_MCFWD_VIF_TYPE_TUNNEL = 1, /* vif tunnel */
  IP4_MCFWD_VIF_TYPE_REGISTER,  /* vif PIM Register interface */
  IP4_MCFWD_VIF_TYPE_STATIC  /* vif static interface */
};


/**************************************************************************//**
@Description  enum  for  ipv4 multicast forward   get operations 
*//***************************************************************************/
enum ip4_mcfwd_get_op {
        IP4_MCFWD_GET_FIRST=0,
        IP4_MCFWD_GET_NEXT,
        IP4_MCFWD_GET_EXACT
};
/**************************************************************************//**
@Description  enum  for  ipv4 multicast forward config operations 
*//***************************************************************************/
enum ip4_mcfwd_cfg_op{
        IP4_MCFWD_ADD=0,
        IP4_MCFWD_DELETE,
        IP4_MCFWD_MOD
};

/**************************************************************************//**
@Description	Input parameters to IPv4 multicast interface specific group configuration
*//***************************************************************************/
struct ip4_mcfwd_group_inargs{
	enum  ip4_mcfwd_cfg_op operation; /* Operation mentions add/del */
	struct ip4_mcfwd_group  ip4addr_entry;
};

/**************************************************************************//**
@Description  multicast interface specific group output arguments
*//***************************************************************************/
 struct ip4_mcfwd_group_outargs
{
   int32_t result;
};
/**************************************************************************//**
@Description multicast vif output arguments
*//***************************************************************************/
 struct ip4_mcfwd_vif_outargs
{
   int32_t result;
};
/**************************************************************************//**
@Description  multicast forward entry output arguments
*//***************************************************************************/
 struct ip4_mcfwd_mfe_outargs
{
   int32_t result;
};

/**************************************************************************//**
@Description	Input parameters for fetching interface specific multicast group 
*//***************************************************************************/
struct  ip4_mcfwd_group_get_inargs{
	enum ip4_mcfwd_get_op operation; /* Operation mentions get_first/
		get_next/get_exact */
	/* Following two fields are not valid for get_first */
         uint32_t   groupaddr;/* multicast group address */
         if_id  ifid;  /* interface id */
};

/**************************************************************************//**
@Description	out parameters for fetch to fetching multicast virtual interface 
*//***************************************************************************/
struct  ip4_mcfwd_group_get_outargs{
    uint32_t ns_id; /* NamesSpace ID */
    struct ip4_mcfwd_group  ip4addr_entry;
    int32_t result;
};

/**************************************************************************//**
@Description	Input parameters to IPv4 multicast virtual interface 
*//***************************************************************************/
struct ip4_mcfwd_vif_inargs{
	enum  ip4_mcfwd_cfg_op operation; /* Operation mentions add/del */
	struct ip4_mcfwd_vif_entry  ip4_mcfwd_entry;
};

/**************************************************************************//**
@Description	Input parameters for fetching multicast virtual interface  
*//***************************************************************************/
struct  ip4_mcfwd_vif_get_inargs{
	enum ip4_mcfwd_get_op operation; /* Operation mentions get_first/
		get_next/get_exact */
	/* Following two fields are not valid for get_first */
        uint32_t  vif_id; /* vif_id, index for the vif table */
};

/**************************************************************************//**
@Description	out parameters for fetch to fetching IPv4 address details 
*//***************************************************************************/
struct  ip4_mcfwd_vif_get_outargs{
    uint32_t ns_id; /* NamesSpace ID */
    struct ip4_mcfwd_vif_entry  ip4_mcfwd_entry;
    struct ip4_mcfwd_stats  stats;
    int32_t result;
};

/**************************************************************************//**
@Description	Input parameters to IPv4 multicast forward  entry configuration
*//***************************************************************************/
struct ip4_mcfwd_mfe_inargs{
	enum  ip4_mcfwd_cfg_op operation; /* Operation mentions add/del */
	struct ip4_mcfwd_mfentry  ip4_mcfwd_entry;
};

/**************************************************************************//**
@Description	Input parameters for fetching multicast forward entry details 
*//***************************************************************************/
struct  ip4_mcfwd_mfe_get_inargs{
	enum ip4_mcfwd_get_op operation; /* Operation mentions get_first/
		get_next/get_exact */
	/* Following two fields are not valid for get_first */
	struct ip4_mcfwd_mfentry  ip4_mcfwd_entry;
};

/**************************************************************************//**
@Description	out parameters for fetch to fetching multicast forward entry details 
*//***************************************************************************/
struct  ip4_mcfwd_mfe_get_outargs{
    uint32_t ns_id; /* NamesSpace ID */
    struct ip4_mcfwd_mfentry  ip4_mcfwd_entry;
    int32_t result;
};
/**************************************************************************//**
@Description	Callback function for processing packet received from DP/AIOP
*//***************************************************************************/

typedef int32_t (*ip4mcfwdappl_rcvselfpkt_fromdp_cbkfn)(struct nfpkt_buf *pPkt);

/**************************************************************************//**
@Description	Ip fwd application registered structure 
*//***************************************************************************/
struct ip4_mcfwd_apln_register_cbkfns
{
  ip4mcfwdappl_rcvselfpkt_fromdp_cbkfn ipmcfwdappln_selfpkt_recvfn;
};


/**************************************************************************//**
@Description     IP forward Namespace specific PBR status enable/disable
*//***************************************************************************/
enum ip4_mcfwd_status_flag {
                IP4_MCFWD_STATUS_ENABLE = 0,
                IP4_MCFWD_STATUS_DISABLE
};


/*************************************************************************//**
@Function	ip4_mcfwd_group_config

@Description    This API is used for multicast group address  configuration for 
		add or delete operations. This database 
		is maintained per Name Space instance. This function first validates the
		incoming parameters and if all validations succeed, adds the entry  in the 
		database.

@Param[in]	nsid - Name Space instance ID. 	
@Param[in]      ip4_mcfwd_inargs - input arguments that will be passed to this api. 
@Param[in]	operation-    configuration operations add/delete. 	

@Param[in]	groupaddr- Interested multicast group address.

@Param[in]	if_id -  interface id.


@Param[in]	flags - API behavioural flags.

@Param[in] ip4_mcfwd_respargs - Response arguments that will be passed to the call back when the call is asynchronous. 
                  Following fields are to be filled by the caller:
                  cbfn - Call back function pointer to be invoked
                  cbarg  - Call back function argument
                  cbarg_len - Call back function argument	length
                  
@Param[out] ip4_mcfwd_outargs - Structure that will be filled with output values of this API.
		This structure is filled when the call is synchronous or asynchronous. If asynchronous, this will
		be the last argument to the call back function ip4_mcfwd_respargs described above.
                  Following fields are filled in the structure:
                   result : Result of this API. Success or failure code in case of failure. Refer Return values of this API.

@Return		Success or Failure
		Success if the record is added/deleted successfully else one of the following 
		error code is returned in case of failure.
		- Invalid parameter value
		- Internal Memory allocation failure 		
		- TBD

@Cautions	None.
*//***************************************************************************/

int32_t ip4_mcfwd_group_config(uint32_t nsid, struct ip4_mcfwd_group_inargs *ip4_mcfwd_inargs,
				api_control_flags flags,
				struct ip4_mcfwd_group_outargs *ip4_mcfwd_outargs,
				struct api_resp_args  *ip4_mcfwd_respargs);


/**************************************************************************//**
@Function       ip4_mcfwd_group_get	

@Description	This API is used to fetch  ipv4 interface specific multicast group address information. This database 
		is maintained per Name Space. 
		This function first validates the incoming parameters 
		and if all validations succeed, the following is performed depending on
		the type of operation:
		if operation is get_first, fetches first entry information from ipv4 address database.
		if operation is get_next, finds the entry in the  database 
		with given information and returns the next entry.
		if operation is get_exact, finds the entry and returns it.

@Param[in]	flags - API behavioural flags.
@Param[in]	nsid -  network namespace id.
@Param[in]	in - Pointer to input param structure. 
		which contains  ipv4 address  information.
@Param[in]	resp - Response arguments for asynchronous call.

@Param[out]	out - Pointer to output param structure
		that will be filled with output values of this API.

@Return		Success or Failure (more errors TBD)

@Cautions	None.
*//***************************************************************************/
int32_t  ip4_mcfwd_group_get(ns_id nsid,
	api_control_flags flags,
	struct ip4_mcfwd_group_get_inargs *in,
	struct ip4_mcfwd_group_get_outargs *out,
	struct api_resp_args *resp);



/*************************************************************************//**
@Function	ip4_mcfwd_vif_config

@Description    This API is used for ipv4 multicast virtual interface configuration for 
		add or delete operations. Multicast packets can be received and sent in two different ways  
                      - either directly over the network adapter in a LAN (or)
                      - encapsulated in unicast  IP packet and transmitted through a tunnel. 
                      To differentiate between normal and tunnel cases multicast 
                      virtual interface was introduced. Each multicast virtual  
                      interface maps to either physical interface or tunnel interface.
                      This database is maintained per Name Space instance. This 
                      function first validates the incoming parameters and if 
                      all validations succeed, adds the vif entry  in the vif database.

@Param[in]	nsid - Name Space instance ID. 	

@Param[in]      ip4_mcfwd_inargs - input arguments that will be passed to this api. 

@Param[in]	threshold -  ttl threshold value for packets that should be sent over this 
                virtual network device. This value will be copied into  multicast forward entry ttls array, which will be
                used subsequently in  packet processing.

@Param[in]	vif_type - vif_type ex: whether the VIF represents a tunnel, if it is tunnel interface this flag should be set with
                IP4_MCFWD_VIF_TYPE_TUNNEL 

@Param[in]	vif_id - multicast virtual interface id value, each virtual interface 
                     will be identified by unique virtual interface id.

@Param[in]	local - ip address of the device or ip address of the tunnel starting point 

@Param[in]	remote - ip address of the tunnel end point. 

@Param[in]	link_id - index of the network device, each multicast virtual interface is 
                associated with a network device index, that will be used for subsequent packet processing. 

@Param[in]	flags - API behavioural flags.

@Param[in] ip4_mcfwd_respargs - Response arguments that will be passed to the call back when the call is asynchronous. 
                  Following fields are to be filled by the caller:
                  cbfn - Call back function pointer to be invoked
                  cbarg  - Call back function argument
                  cbarg_len - Call back function argument	length
                  
@Param[out] ip4_mcfwd_outargs - Structure that will be filled with output values of this API.
		This structure is filled when the call is synchronous or asynchronous. If asynchronous, this will
		be the last argument to the call back function ip4_mcfwd_respargs described above.
                  Following fields are filled in the structure:
                   result : Result of this API. Success or failure code in case of failure. Refer Return values of this API.


@Return		Success or Failure
		Success if the addition/deletion is success else one of the following 
		error code is returned in case of failure.
		- Invalid parameter value 
		- Internal memory allocation failure
		- TBD

@Cautions	None.
*//***************************************************************************/

int32_t ip4_mcfwd_vif_config(uint32_t nsid, struct ip4_mcfwd_vif_inargs *ip4_mcfwd_inargs,
				api_control_flags flags,
				struct ip4_mcfwd_vif_outargs *ip4_mcfwd_outargs,
				struct api_resp_args  *ip4_mcfwd_respargs);



/**************************************************************************//**
@Function       ip4_mcfwd_vif_get	

@Description	This API is used to fetch  ipv4  multicast virtual interface information. This database 
		is maintained per Name Space. 
		This function first validates the incoming parameters 
		and if all validations succeed, the following is performed depending on
		the type of operation:
		if operation is get_first, fetches first entry information from ipv4 address database.
		if operation is get_next, finds the entry in the  database 
		with given information and returns the next entry.
		if operation is get_exact, finds the entry and returns it.

@Param[in]	flags - API behavioural flags.
@Param[in]	nsid -  network namespace id.
@Param[in]	in - Pointer to input param structure. 
		which contains  ipv4 address  information.
@Param[in]	resp - Response arguments for asynchronous call.

@Param[out]	out - Pointer to output param structure
		that will be filled with output values of this API.

@Return		Success or Failure (more errors TBD)

@Cautions	None.
*//***************************************************************************/
int32_t  ip4_mcfwd_vif_get(ns_id nsid,
	api_control_flags flags,
	struct ip4_mcfwd_vif_get_inargs *in,
	struct ip4_mcfwd_vif_get_outargs *out,
	struct api_resp_args *resp);


/*************************************************************************//**
@Function	ip4_mcfwd_mfentry_config

@Description    This API is used for ipv4 multicast forward entry configuration for 
		add or delete operations. The multicast forward entry 
                implements the multicast forward routing table. The multicast forward entry database built in the 
                form of a hash table. This database is maintained per Name Space instance. This function first validates 
                the incoming parameters  and if all validations succeed, adds the  entry  in the mf entry database.

@Param[in]	nsid - Name Space instance ID. 	

@Param[in]      ip4_mcfwd_inargs - input arguments that will be passed to this api. 

@Param[in]	mcastgrp-  Multicast group address. 

@Param[in]	src_ip-  Source address of the packet.  mcastgrp and src_ip together form the key 
                for an entry in mf entry database.

@Param[in]	vif_id - Index of the virtual network device in the vif table over which packets this MF entry should arrive 

@Param[in]	minvif - min vif  index to  the ttls list of the MF entry.

@Param[in]	maxvif - maxi vif  index  to the ttls list of the MF entry.

@Param[in]	ttl - is an array with max vifs entries, where each entry specifies whether a packet
                should be forwarded over the virtual network device in vif table list with corresponding
                index.

@Param[in]	flags - API behavioural flags.

@Param[in] ip4_mcfwd_respargs - Response arguments that will be passed to the call back when the call is asynchronous. 
                  Following fields are to be filled by the caller:
                  cbfn - Call back function pointer to be invoked
                  cbarg  - Call back function argument
                  cbarg_len - Call back function argument	length
                  
@Param[out] ip4_mcfwd_outargs - Structure that will be filled with output values of this API.
		This structure is filled when the call is synchronous or asynchronous. If asynchronous, this will
		be the last argument to the call back function ip4_mcfwd_respargs described above.
                  Following fields are filled in the structure:
                   result : Result of this API. Success or failure code in case of failure. Refer Return values of this API.


@Return		Success or Failure
		Success if a multicast forward entry added/deleted  successfully else one of the following 
		error code is returned.
		- Invalid parameter value 
		- Internal memory allocation failure 
		- TBD

@Cautions	None.
*//***************************************************************************/
int32_t ip4_mcfwd_mfe_config(uint32_t nsid, struct ip4_mcfwd_mfe_inargs *ip4_mcfwd_inargs,
				api_control_flags flags,
				struct ip4_mcfwd_mfe_outargs *ip4_mcfwd_outargs,
				struct api_resp_args  *ip4_mcfwd_respargs);



/**************************************************************************//**
@Function       ip4_mcfwd_mfe_get	

@Description	This API is used to fetch  multicast forward cache information. This database 
		is maintained per Name Space. 
		This function first validates the incoming parameters 
		and if all validations succeed, the following is performed depending on
		the type of operation:
		if operation is get_first, fetches first entry information from ipv4 address database.
		if operation is get_next, finds the entry in the  database 
		with given information and returns the next entry.
		if operation is get_exact, finds the entry and returns it.

@Param[in]	flags - API behavioural flags.
@Param[in]	nsid -  network namespace id.
@Param[in]	in - Pointer to input param structure. 
		which contains  ipv4 address  information.
@Param[in]	resp - Response arguments for asynchronous call.

@Param[out]	out - Pointer to output param structure
		that will be filled with output values of this API.

@Return		Success or Failure (more errors TBD)

@Cautions	None.
*//***************************************************************************/
int32_t  ip4_mcfwd_mfe_get(uint32_t nsid,
	api_control_flags flags,
	struct ip4_mcfwd_mfe_get_inargs *in,
	struct ip4_mcfwd_mfe_get_outargs *out,
	struct api_resp_args *resp);


/*************************************************************************//**
@Function  ip4_mcfwd_appln_register_cbkfn

@Description    DP/AIOP can send self-destined packet to application for the 
            packet to be given to local applications in the Control Plane/GPP.
            IP multicast Forward application at CP/GPP will register a function to 
            receive such packets from DP/AIOP and further process the packet.

@Param[in] pIpFwdApplnCbkFns - pointer to the structure containing the 
         callback functions being registered by the Ip Forward Application.

@Return         Success or Failure
      Success if the callback function is registered successfully else failure
      is returned.

@Cautions  None.
*//***************************************************************************/
int32_t ip4_mcfwd_appln_register_cbkfn(struct ip4_mcfwd_apln_register_cbkfns *pIpMcFwdApplnCbkFns);


/*************************************************************************//**
@Function 	ip4_mcfwd_appln_send_pkt_to_dp

@Description  Send packet from CP/GPP to DP/AIOP.

@Param[in]  	ns_id - Name Space ID for which the stats are to be retrieved. 
				
@Param[in] 	pPkt - Packet to send out

@Param[in] 	flags - API behavioural flags.

@Param[in]	ip4_mcfwd_send_pkt_respargs - Response arguments for asynchronous call.


@Return   Success or Failure (more errors TBD).

@Cautions	None.
*//***************************************************************************/

int32_t ip4_mcfwd_appln_send_pkt_to_dp(uint32_t nsid, 
                    void *pPkt  /*Buffer from CP/GPP  - TBD */,
                    api_control_flags flags,
                    struct api_resp_args  *ip4_mcfwd_send_pkt_respargs);




/**************************************************************************//**
@Function          ip4_mcfwd_set_status

@Description     This API is used to set IP multicast  forward  
                                status as enable/disable for a given name space. 

@Param[in]        ns_id - NamesSpace ID.                                
@Param[in]        status - Status indicating enable/disable.
@Param[in]        flags - API behavioural flags.
@Param[in]        resp - Response arguments for asynchronous call.

@Return   Success or Failure (more errors TBD).

@Cautions          None.
*//***************************************************************************/
int32_t ip4_mcfwd_set_status(
                uint32_t nsid,
                enum ip4_mcfwd_status_flag status,
                api_control_flags flags,
                struct api_resp_args *resp);

#endif /* __IP4_MCAST_FWD_API_H */

