/**************************************************************************//**
@File          ip4_fwd_nf_api.h

@Description   This file contains the IPv4 unicast forward NF API & related 
                      macros, data structures
*//***************************************************************************/

#ifndef __NF_IPFWD_H
#define __NF_IPFWD_H

#include "nf_infra_api.h"

/* Flags for a PBR entry*/
#define IP4_PBR_IN_IFACE_VALID 0x01
#define IP4_PBR_OUT_IFACE_VALID 0x02
#define IP4_PBR_PERMANENT_RULE 0x04
#define IP4_MAX_IFACE_NAME_LEN 16  // - added this

/* Actions for a PBR rule */
enum ipfwd_rule_action_e{
	IP4_PBR_ACT_GOTO_TABLE, /* Go to a table */
	IP4_PBR_ACT_GOTO_RULE, /* Go to another rule */
	IP4_PBR_ACT_NO_OPERATION, /* Skip the rule */
	IP4_PBR_ACT_BLACKHOLE, /* Black hole, silent drop */
	IP4_PBR_ACT_UNREACHABLE, /* Drop, send ICMP net unreachable err msg */
	IP4_PBR_ACT_PROHIBIT, /* Drop, send ICMP Pkt filtered err msg */
	IP4_PBR_ACT_MAX_VAL
};


/* Structue for configuring PBR Rule */
struct ip4_fwd_pbr_rule {
     uint16_t ns_id; /* Name Space instance ID*/
     uint16_t priority; /* Priority of the rule */
     uint32_t saddr; /*Source IP address*/
     uint32_t smask; /*Source IP mask value */
     uint32_t daddr; /*Destination IP address*/
     uint32_t dmask; /*Destination IP mask value */
     uint8_t in_ifid; /* Source iface id */ 
     uint8_t out_ifid; /* Destination iface id */ 
     uint16_t uiflags; /*valid in ifid, valid out ifid, permanent rule*/
     uint8_t tos; /* type of service value */
     //ipfwd_rule_action_e action; /* Different actions : prohibit/reject/unreachable/goto etc */  // - commented
     uint8_t action; /* Different actions : prohibit/reject/unreachable/goto etc */ 
     uint32_t fwmark; /* fw mark value */
     uint32_t fwmark_mask; /* fw mark mask */
     uint32_t target_priority; /* Target priority, if action is goto rule */
     uint32_t rt_table_no; /* Route table number, if action is goto table */
};

/* Scope of the route, covers from universe to site to link to host and to nowhere (from linux )*/
enum ip4_fwd_route_scope_e{
   IP4_RT_SCOPE_UNIVERSE=0, /* Global/Universal scope */
   /* User defined values goes here  */
   IP4_RT_SCOPE_SITE=200, /* Site scope */
   IP4_RT_SCOPE_LINK=253, /*Link level scope*/
   IP4_RT_SCOPE_HOST=254, /* Scope is host*/
   IP4_RT_SCOPE_NOWHERE=255 /* Scope no where. Drop packet*/
};

/* Different route types that can be configured for a route */
enum ip4_fwd_route_type_e{
  IP4_RT_TYPE_UNICAST = 1, /* Unicast*/
  IP4_RT_TYPE_LOCAL, /* Local */
  IP4_RT_TYPE_UNREACHABLE, /* Drop, send ICMP net unreachable err msg */
  IP4_RT_TYPE_BLACKHOLE, /* Black hole, silent drop */
  IP4_RT_TYPE_PROHIBIT, /* Drop, send ICMP Pkt filtered err msg */
  IP4_RT_TYPE_BROADCAST, /* Broadcast */
  IP4_RT_TYPE_THROW /* A special route used in conjunction with PBR. If this route matches, 
                                         lookup in this table is terminated pretending that no route was found. 
                                         The packets are dropped and the ICMP message net unreachable is generated.  
                                         Without PBR it is equivalent to the absence of the route in the routing table. */ 
} ;

/*Flag for next hop */
#define RT_NH_DEAD 1

/*Next hop structure */
struct ip4_fwd_nh{
  uint16_t	 flags;  /* RT_NH_DEAD */
  uint8_t   scope; /* Scope of the gateway */ 
  uint32_t	 out_ifid; /* Out interface Id*/
  uint32_t	 gw_ipaddr; /* Gateway IP address*/
  int32_t	 weight; /*weight  - for ECMP */
  int32_t	 power;/*Power  - for ECMP */
};

/*Maximum multiple gateways that can be configured for a single route */
#define IP4_FWD_MAX_ECMP_GWS 8

/* NOTE: For LS AIOP only hash based algorithm is valid*/
#define IP4_ECMP_ALGO_HASH_BASED  1
#define IP4_ECMP_ALGO_ROUND_ROBIN  2
#define IP4_ECMP_ALGO_WEIGHTED_RANDOM    3 


/* Metrics for a route -- Linux requirement */
/* Do we need to support all? TBD */
enum ip4_fwd_route_metrics_e{
//  IP4_RT_METRIC_UNSPEC=0, /* Not a metric */
//  IP4_RT_METRIC_LOCK, /* Not a metric */
  IP4_RT_METRIC_MTU=2, /* Path MTU */
  IP4_RT_METRIC_WINDOW, /* Maximum advertised window */
  IP4_RT_METRIC_RTT, /* Round trip time*/ 
  IP4_RT_METRIC_RTTVAR, /* RTT variance */ 
  IP4_RT_METRIC_SSTHRESH, /* Slow start threshold */
  IP4_RT_METRIC_CWND, /* Congestion window */
  IP4_RT_METRIC_ADVMSS, /* Maximum Segment Size */ 
  IP4_RT_METRIC_REORDERING, /* Maximum Reordering */ 
  IP4_RT_METRIC_HOPLIMIT, /* Default Time To Live */
  IP4_RT_METRIC_INITCWND, /* Initial Congestion window */
  IP4_RT_METRIC_FEATURES, /*Not a metric */
  IP4_RT_METRIC_RTO_MIN, /* Min retransmission timeout val */
  IP4_RT_METRIC_INITRWND, /* Initial receive window size*/
  IP4_RT_METRICS_MAX
};

/*Macros that define which metric is set */
//#define   IP4_RT_METRIC_UNSPEC_SET (1<<0)
//#define   IP4_RT_METRIC_LOCK_SET (1<<1)
#define   IP4_RT_METRIC_MTU_SET (1<<2)
#define   IP4_RT_METRIC_WINDOW_SET (1<<3)
#define   IP4_RT_METRIC_RTT_SET (1<<4)
#define   IP4_RT_METRIC_RTTVAR_SET (1<<5)
#define   IP4_RT_METRIC_SSTHRESH_SET (1<<6)
#define   IP4_RT_METRIC_CWND_SET (1<<7)
#define   IP4_RT_METRIC_ADVMSS_SET (1<<8)
#define   IP4_RT_METRIC_REORDERING_SET (1<<9)
#define   IP4_RT_METRIC_HOPLIMIT_SET (1<<10)
#define   IP4_RT_METRIC_INITCWND_SET (1<<11)
//#define   IP4_RT_METRIC_FEATURES_SET (1<<12)
#define   IP4_RT_METRIC_RTO_MIN_SET (1<<13)
#define   IP4_RT_METRIC_INITRWND_SET (1<<14)

/* Structure to add a route entry */
struct ip4_fwd_route_entry {
     uint16_t ns_id; /* Name space id*/
     uint32_t dst_addr; /* Destination IP address */
     uint32_t dst_mask; /* Destination IP mask */
     uint32_t prefsrc; /* Preferred source for the route */
     uint32_t path_mtu; /* MTU defined for the path */
     uint32_t rt_table_id; /* Route table ID, to which this route is to be added */
     uint8_t tos; /* type of service */
  //   uint8_t scope; /* scope */ /* - Discuss */
     uint8_t type; /* Route type*/
     uint8_t num_gw; /* Number of gateways present in this route */
     uint8_t metric; /* metric value of this route */
     uint8_t proto; /* Specifies how this route is installed. Could be through 
     			ICMP redirect message/native OS/During boot/by admin/by a routing protocol */ 
     uint32_t  priority; /* Priority of the route. Smaller the value, higher the priority. */
     uint32_t   route_metrics[IP4_RT_METRICS_MAX]; /*Metrics for the route. Linux values- TCP 
                                            window size, RTT, advmss etc : */
     uint16_t route_metrics_flags; /*Route metrics flag - bit wise value of the route metrics structure for the values set represents ip4_fwd_route_metrics_e*/
#define IP4_FWD_ROUTE_ACCESSED 0x01										
     uint8_t state; /* State of route accessibility.  If this flag is set, this it is assumed that internal implementation has 
                              to invalidate the cache (if implemented). TBD */
     struct ip4_fwd_nh gw_info[IP4_FWD_MAX_ECMP_GWS]; /*Single/Multiple gateway(s). */
};



/* IP Forward stats structure */
struct ip4_fwd_stats
{
  uint16_t ns_id; /* Name space id*/
  uint64_t ip4_tot_in_pkts; /*In rcvd pkts */
  uint64_t ip4_tot_in_bytes; /*In rcvd bytes*/
  uint64_t ip4_in_hdr_errs; /*In pkt hdr err */
  uint64_t ip4_in_no_route; /*In no route */
  uint64_t ip4_local_in_pkts; /*In local deliver */
  uint64_t ip4_fwd_pkts; /*Out fwd pkts */
  uint64_t ip4_tot_out_pkts; /*Out pkts */
  uint64_t ip4_tot_out_bytes; /*Out bytes*/
  uint64_t ip4_out_hdr_errs; /*Out pkt hdr err */
  uint64_t ip4_out_no_route; /*Out no route */
  uint64_t ip4_spoof_pkt; /*Spoof pkts */
};


struct ipforward_capabilities { 
#define IPFWD_V4_PBR_RULES_SUPPORTED  0x01  /*PBR rules are supported for IPv4 forward*/
#define IPFWD_V6_PBR_RULES_SUPPORTED  0x02  /*PBR rules are supported for IPv6 forward*/
   uint64_t capabilities; /* list of capabilities */
};

/*structure that receives packet from DP*/
struct nfpkt_buf
{
     uint32_t ns_id; /* Name space id*/
     uint32_t ifid; /* pETH interface id*/
     char peth_name[IP4_MAX_IFACE_NAME_LEN];  /*PETH interface name*/ /* */
     struct pkt_mbuf *pkt;  /*Packet Buffer from DP */
};

int32_t  ipforward_api_get_version(char *version); /* This API would internally copy the ip forward version string to the supplied parameter */

int32_t  ipforward_api_get_capabilities(struct ipforward_capabilities *capabilities); /* Provides the list of ip forward DP capabilities */


/* Structure used for output arguments for PBR rules related NF API*/
struct ip4_fwd_rule_outargs 
{
   int32_t result;
};

/* Structure used for output arguments for route related NF API */
struct ip4_fwd_route_outargs 
{
   int32_t result;
};

/* Structure used for output arguments for ip4 fwd stats related NF API */
struct ip4_fwd_stats_outargs 
{
   int32_t result;
};

/* callback function for processing packet received from DP*/	
typedef int32_t (*ipfwdappl_rcvselfpkt_fromdp_cbkfn)(struct nfpkt_buf *pkt);

/*Ip fwd application registered structure */
struct ipfwdapln_register_cbkfns
{
  ipfwdappl_rcvselfpkt_fromdp_cbkfn ipfwdappln_selfpkt_recvfn;
};

/*************************************************************************//**
@Function	ip4_fwd_policy_rule_add

@Description    This API adds a rule to the Policy Based Routing rule database. This database 
		is maintained per Name Space instance. This function first validates the
		incoming parameters and if all validations succeed, adds the rule in the 
		PBR rule database.
Following is  the description fields of struct ip4_fwd_pbr_rule fields
@Param[in]	ns_id - Name Space instance ID. 	
@Param[in]	priority - Priority (Policy Id) of the given rule.
@Param[in]	saddr - Source IP Address to match for against the source ip 
		field in the incoming path 
@Param[in]	smask -  Source IP Mask value to be used against the source ip
		address in the packet.
@Param[in]	daddr - Destination IP Address to match for against the 
		destination ip field in the incoming path.
@Param[in]	dmask - Destination IP Mask value to be used against the 
		destination ip address in the packet.
@Param[in]	in_ifid - In interface that should be matched with the 
		incoming interface of the packet.
@Param[in]	out_ifid - Out interface that should be matched with the 
		outgoing interface  of the packet. This field is valid for the 
		packets going from the device.
@Param[in]	uiflags - valid in ifid, valid out ifid, permanent rule.
@Param[in]	tos - Type of service.
@Param[in]	action - Prohibit / Reject / Unreachable / Table / 
		Goto [Policy rule No] / No operation.
@Param[in]	fwmark - FWMark ID of iptables/ipchains rule.
@Param[in]	fwmark_mask - Mask value for FWMark.
@Param[in]	target_policy_id - Target policy ID, if action is goto rule.
@Param[in]	rt_table_no - Route table number, if action is goto table. 


@Param[in]	flags - Flags for the NF API. It can take the following values
			 - 0 -Synchronous call of this API. Caller will be blocked till this API 
			 	returns.
                      - ZZZ_FLAG_ASYNC : Asynchronous call of this API
                      - ZZZ_FLAG_NO_RESP_EXPECTED:  Caller of this API is not expecting response

@Param[in] ip4_fwd_pbr_respargs - Response arguments that will be passed to the call back when the call is asynchronous. 
                  Following fields are to be filled by the caller:
                  cbfn - Call back function pointer to be invoked
                  cbarg  - Call back function argument
                  cbarg_len - Call back function argument	length
                  
@Param[out] ip4_out_args - Structure that will be filled with output values of this API.
		This structure is filled when the call is synchronous or asynchronous. If asynchronous, this will
		be the last argument to the call back function ip4_fwd_pbr_respargs described above.
                  Following fields are filled in the structure:
                   result : Result of this API. Success or failure code in case of failure. Refer Return values of this API.


@Return		Success or Failure
		Success if a rule is added successfully else one of the following 
		error code is returned in .
		- Invalid parameter value (ip, action, …)
		- Memory allocation failure to create a rule		
		- more values are TBD

@Cautions	None.
*//***************************************************************************/
int32_t ip4_fwd_policy_rule_add(uint16_t ns_id, struct ip4_fwd_pbr_rule *new_pbr_rule, 
									ZZZ_flags_t flags,
									struct ip4_fwd_rule_outargs *ip4_out_args,
									struct api_resp_args  *ip4_fwd_pbr_respargs);





/*************************************************************************//**
@Function	ip4_fwd_policy_rule_delete

@Description    This API deletes a rule from a Policy Based Routing rule 
		database. If the rule to be deleted is a permanent rule, an error will be thrown.
		Any non-zero value for the in parameters is considered as a valid field value.
		The order of validating fields will be in the following order.
		
@Param[in]	ns_id - Name Space instance ID. 	
@Param[in]	priority - Priority of the rule.
@Param[in]	action - Prohibit / Reject / Unreachable / Table / 
		Goto [Policy rule No] / No operation.
@Param[in]	rt_table_no - Route table number. 
@Param[in]	in_ifid - In interface id
@Param[in]	out_ifid - Out interface id
@Param[in]	saddr - Source IP Address 
@Param[in]	smask -  Source IP Mask value 
@Param[in]	daddr - Destination IP Address.
@Param[in]	dmask - Destination IP Mask value.
@Param[in]	tos - Type of service.
@Param[in]	fwmark - FWMark ID of iptables/ipchains rule.
@Param[in]	fwmark_mask - Mask value for FWMark.
@Param[in]	target_policy_id - Target policy ID.

@Param[in]	flags - Flags for the NF API. It can take the following values
			 - 0 -Synchronous call of this API. Caller will be blocked till this API 
			 	returns.
                      - ZZZ_FLAG_ASYNC : Asynchronous call of this API
                      - ZZZ_FLAG_NO_RESP_EXPECTED:  Caller of this API is not expecting response

@Param[in] ip4_fwd_pbr_respargs - Response arguments that will be passed to the call back when the call is asynchronous. 
                  Following fields are to be filled by the caller:
                  cbfn - Call back function pointer to be invoked
                  cbarg  - Call back function argument
                  cbarg_len - Call back function argument	length
                  
@Param[out] ip4_out_args - Structure that will be filled with output values of this API.
		This structure is filled when the call is synchronous or asynchronous. If asynchronous, this will
		be the last argument to the call back function ip4_fwd_pbr_respargs described above.
                  Following fields are filled in the structure:
                   result : Result of this API. Success or failure code in case of failure. Refer Return values of this API.

@Return		Success or Failure
		Success if a rule has been deleted successfully with the given 
		values else one of the following error code is returned.
		- Invalid parameter value (ip, action, …)
		- Rule is a permanent rule
		- Rule not found with the given params
		- TBD

@Cautions	None.
*//***************************************************************************/
int32_t ip4_fwd_policy_rule_delete(uint16_t ns_id, struct ip4_fwd_pbr_rule *pbr_rule,
                                                               ZZZ_flags_t flags,
									struct ip4_fwd_rule_outargs *ip4_out_args,
									struct api_resp_args  *ip4_fwd_pbr_respargs);


/*************************************************************************//**
@Function	ip4_fwd_route_add

@Description    This API adds a route to routing database. This database is 
		maintained per name space instance. This function first validates the
		incoming parameters and if all validation succeeds, adds a route in the 
		routing table. This functin checks if a table has been created with the
		given table ID. If not creates a table with the given ID.

@Param[in]	ns_id - Name Space instance ID. 	
@Param[in]	dst_addr - Destination address
@Param[in]	dst_mask - Netmask used for matching Destination IP
@Param[in]	prefsrc - Source IP address to prefer when sending the packet 
		out to a destination covered by this route.
@Param[in]	path_mtu - Mtu for the path
@Param[in]	rt_table_id - Table id to which this route is to be added.
@Param[in]	tos - Type of service. For same destination the ToS will be 
		arranged in the decreasing value.
@Param[in]	type - Route type
		Different route types that can be configured for a route are,
		Unicast / unreachable /blackhole / prohibit / local/ broadcast/
		throw. Default value is unicast.
@Param[in]	metric - metric value of this route 
@Param[in]	scope - Scope for the route.
		UNIVERSE/ SITE/ LINK/ HOST/ NOWHERE
@Param[in]	num_gw - Number of gateways present in this route. Gateway(s) information to be 
			stored in the array gw_info.
@Param[in]	gw_info - Gateway address; It can be Single/Multiple gateway(s).
@Param[in]	proto -  Specifies how this route is installed. Could be through ICMP redirect message/native OS/During boot/by admin
				or by a routing protocol 
@Param[in]	priority - Priority of the route. Smaller the value, higher the priority. 
@Param[in]    route_metrics - Array of Routing metrics 
@Param[in]    route_metrics_flags -  Flags to denote which routing metrics are set


@Param[in]	flags - Flags for the NF API. It can take the following values
			 - 0 -Synchronous call of this API. Caller will be blocked till this API 
			 	returns.
                      - ZZZ_FLAG_ASYNC : Asynchronous call of this API
                      - ZZZ_FLAG_NO_RESP_EXPECTED:  Caller of this API is not expecting response

@Param[in] ip4_fwd_route_respargs - Response arguments that will be passed to the call back when the call is asynchronous. 
                  Following fields are to be filled by the caller:
                  cbfn - Call back function pointer to be invoked
                  cbarg  - Call back function argument
                  cbarg_len - Call back function argument	length
                  
@Param[out] ip4_out_args - Structure that will be filled with output values of this API.
		This structure is filled when the call is synchronous or asynchronous. If asynchronous, this will
		be the last argument to the call back function ip4_fwd_route_respargs described above.
                  Following fields are filled in the structure:
                   result : Result of this API. Success or failure code in case of failure. Refer Return values of this API.


@Return		Success or Failure
		Success if a route is added successfully else one of the following 
		error code is returned.
		- Invalid parameter value (ip, action, …)
		- Memory allocation failure to create a route
		- A route already exists with the given parameters
		- [TBD]
		
@Cautions	None.
*//***************************************************************************/
int32_t ip4_fwd_route_add(uint16_t ns_id, struct ip4_fwd_route_entry *new_rt_entry_data,
                                                 ZZZ_flags_t flags,
							struct ip4_fwd_route_outargs *ip4_out_args,
							struct api_resp_args  *ip4_fwd_route_respargs);


/*************************************************************************//**
@Function	ip4_fwd_route_modify

@Description    This API modifies an existing route in the routing database. This function 
                first validates the incoming parameters and if all validations succeed, this 
                functin updates the given route record. 

@Param[in]	ns_id - Name Space instance ID. 	
@Param[in]	dst_addr - Destination address
@Param[in]	dst_mask - Netmask used for matching Destination IP
@Param[in]	rt_table_id - Table id in which this route has to be modified.
@Param[in]	tos - Type of service. 
@Param[in]	type - Route type
                     Different route types that can be configured for a route are,
                     Unicast / unreachable /blackhole / prohibit / local/ broadcast/
                      throw. Default value is unicast.
@Param[in]	scope - Scope for the route.
                      UNIVERSE/ SITE/ LINK/ HOST/ NOWHERE
@Param[in]	priority - Priority of the route. 
[Mandatory till here]
@Param[in]	state - State of route accessibility.  If this flag is set, this it
                     is assumed that internal implementation has to invalidate 
                     the cache (if implemented). 
@Param[in]	prefsrc - Source IP address to prefer when sending the packet 
                     out to a destination covered by a route.
@Param[in]	path_mtu - Mtu for the path
@Param[in]	metric - metric value of this route 
@Param[in]	num_gw - Number of gateways present in this route. Gateway(s) information to be 
			stored in the array gw_info.
@Param[in]	gw_info - Gateway address; It can be Single/Multiple gateway(s).
@Param[in]	proto -  Specifies how this route is installed. Could be through ICMP redirect message/native OS/During boot/by admin
				or by a routing protocol
@Param[in]    route_metrics - Array of Routing metrics 
@Param[in]    route_metrics_flags -  Flags to denote which routing metrics are set

@Param[in]	flags - Flags for the NF API. It can take the following values
			 - 0 -Synchronous call of this API. Caller will be blocked till this API 
			 	returns.
                      - ZZZ_FLAG_ASYNC : Asynchronous call of this API
                      - ZZZ_FLAG_NO_RESP_EXPECTED:  Caller of this API is not expecting response

@Param[in] ip4_fwd_route_respargs - Response arguments that will be passed to the call back when the call is asynchronous. 
                  Following fields are to be filled by the caller:
                  cbfn - Call back function pointer to be invoked
                  cbarg  - Call back function argument
                  cbarg_len - Call back function argument	length
                  
@Param[out] ip4_out_args - Structure that will be filled with output values of this API.
		This structure is filled when the call is synchronous or asynchronous. If asynchronous, this will
		be the last argument to the call back function ip4_fwd_route_respargs described above.
                  Following fields are filled in the structure:
                   result : Result of this API. Success or failure code in case of failure. Refer Return values of this API.
				
@Return		Success or Failure
		Success if a route is modified successfully else one of the following 
		error code is returned.
		- Invalid parameter value (ip, action, …)
		- A route doesn't exists with the given parameters
		- [TBD]
		
@Cautions	None.
*//***************************************************************************/
int32_t ip4_fwd_route_modify(uint16_t ns_id, struct ip4_fwd_route_entry *rt_entry_data,
                                                 ZZZ_flags_t flags,
							struct ip4_fwd_route_outargs *ip4_out_args,
							struct api_resp_args  *ip4_fwd_route_respargs);

/*************************************************************************//**
@Function	ip4_fwd_route_delete

@Description    This API deletes a route from a routing table. Any non-zero value for the in 
                      parameters is considered as a valid field value.
                      The order of validating fields will be in the following order.

@Param[in]	ns_id - Name Space instance ID. 	
@Param[in]	dst_addr - Destination address
@Param[in]	dst_mask - Netmask used for matching Destination IP
@Param[in]	rt_table_id - Table id from which this route is to be deleted.

@Param[in]	tos - Type of service. 
@Param[in]	scope - Scope for the route.
		UNIVERSE/ SITE/ LINK/ HOST/ NOWHERE
@Param[in]	type - Route type
		Different route types that can be configured for a route are,
		Unicast / unreachable /blackhole / prohibit / local/ broadcast/
		throw. Default value is unicast.
@Param[in]	priority - Priority of the route. 
@Param[in]	proto -  Route installed through ICMP redirect message/native OS/During boot/by admin
				or by a routing protocol 
@Param[in]	num_gw - Number of gateways present in this route. Gateway(s) information to be 
			stored in the array gw_info.
@Param[in]	gw_info - Gateway address; It can be Single/Multiple gateway(s). 
 

@Param[in]	flags - Flags for the NF API. It can take the following values
			 - 0 -Synchronous call of this API. Caller will be blocked till this API 
			 	returns.
                      - ZZZ_FLAG_ASYNC : Asynchronous call of this API
                      - ZZZ_FLAG_NO_RESP_EXPECTED:  Caller of this API is not expecting response

@Param[in] ip4_fwd_route_respargs - Response arguments that will be passed to the call back when the call is asynchronous. 
                  Following fields are to be filled by the caller:
                  cbfn - Call back function pointer to be invoked
                  cbarg  - Call back function argument
                  cbarg_len - Call back function argument	length
                  
@Param[out] ip4_out_args - Structure that will be filled with output values of this API.
		This structure is filled when the call is synchronous or asynchronous. If asynchronous, this will
		be the last argument to the call back function ip4_fwd_route_respargs described above.
                  Following fields are filled in the structure:
                   result : Result of this API. Success or failure code in case of failure. Refer Return values of this API.

@Return		Success or Failure
		Success if a route is deleted successfully else one of the following 
		error code is returned.
		- Invalid parameter value (ip, action, …)
		- A route doesn't exists with the given parameters
		- [TBD]
		
@Cautions	None.
*//***************************************************************************/
int32_t ip4_fwd_route_delete(uint16_t ns_id, struct ip4_fwd_route_entry *rt_entry_data,
                                                     ZZZ_flags_t flags,
							struct ip4_fwd_route_outargs *ip4_out_args,
							struct api_resp_args  *ip4_fwd_route_respargs);


/*************************************************************************//**
@Function 	ip4_fwd_stats_get

@Description  Fetches ip4 forward module statistics.

@Param[in]  ns_id - Name Space ID for which the stats are to be retrieved. This field which is part of 
				ip4_fwd_stats argiment, has to be filled by the caller.
				
@Param[out] ip4_fwd_stats - Structure where the statistics are stored. Following fields are updated.

  ip4_tot_in_pkts - In rcvd pkts counter
  ip4_tot_in_bytes - In rcvd bytes counter
  ip4_in_hdr_errs - In pkt hdr err counter
  ip4_in_no_route - In no route counter
  ip4_local_in_pkts - In local deliver counter
  ip4_fwd_pkts - Out fwd pkts counter
  ip4_tot_out_pkts - Out rcvd pkts counter
  ip4_tot_out_bytes - Out rcvd bytes counter
  ip4_out_hdr_errs - Out pkt hdr err counter
  ip4_out_no_route - Out no route counter
  ip4_spoof_pkt - spoof pkts counter

@Param[in]	flags - Flags for the NF API. It can take the following values
			 - 0 -Synchronous call of this API. Caller will be blocked till this API 
			 	returns.
                      - ZZZ_FLAG_ASYNC : Asynchronous call of this API
                      - ZZZ_FLAG_NO_RESP_EXPECTED:  Caller of this API is not expecting response

@Param[in] ip4_fwd_stats_respargs - Response arguments that will be passed to the call back when the call is asynchronous. 
                  Following fields are to be filled by the caller:
                  cbfn - Call back function pointer to be invoked
                  cbarg  - Call back function argument
                  cbarg_len - Call back function argument	length
                  
@Param[out] ip4_out_args - Structure that will be filled with output values of this API.
		This structure is filled when the call is synchronous or asynchronous. If asynchronous, this will
		be the last argument to the call back function ip4_fwd_route_respargs described above.
                  Following fields are filled in the structure:
                   result : Result of this API. Success or failure code in case of failure. Refer Return values of this API.

@Return   Success or Failure (more errors TBD).

@Cautions 
@Cautions	None.
*//***************************************************************************/

int32_t ip4_fwd_stats_get(uint16_t ns_id, struct ip4_fwd_stats *ip4_fwd_stats,
                                               ZZZ_flags_t flags,
							struct ip4_fwd_stats_outargs *ip4_out_args,
							struct api_resp_args  *ip4_fwd_stats_respargs);


/*************************************************************************//**
@Function	ipfwdappln_register_cbkfns

@Description    DP can send self-destined packet to application for the
                packet to be given to local applications in the Control Plane.
		IP Fwd application will register a function to receive such
	       	packets from DP and further process the packet.

@Param[in]	ns_id - Name Space instance ID. 	
@Param[in]	pIpFwdApplnCbkFns - pointer to the structure containing the
                callback functions being registered by the Ip Fwd Application.

@Return		Success or Failure
		Success if the callback funcation is registerd successfully;
	       	else failure is returned.

@Cautions	None.
*//***************************************************************************/
//uint32_t ipfwdappln_register_cbkfns(uint16_t ns_id, struct ipfwdapln_register_cbkfns *pIpFwdApplnCbkFns);
uint32_t ipfwdappln_register_cbkfns(struct ipfwdapln_register_cbkfns *pIpFwdApplnCbkFns);  /* - modified Discuss */
#endif /* ifndef __NF_IPFWD_H */
