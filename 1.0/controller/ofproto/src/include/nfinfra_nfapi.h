/**************************************************************************//**
@File          fsl_nfapi_nfinfra.h

@Description   This file contains the NF-Infra Network Function API
*//***************************************************************************/

#ifndef __FSL_NFAPI_NFINFRA_H
#define __FSL_NFAPI_NFINFRA_H

typedef uint16_t ns_id;
typedef uint16_t zone;
typedef uint32_t if_id;

#define MAX_ADDR_SIZE 6

/**************************************************************************//**
@Description	API behavioural flags.
*//***************************************************************************/
enum api_control_flags {
	API_CTRL_FLAG_ASYNC = BIT(0), /**< If set, API call should be
			asynchronous. Otherwise API call will be synchronous.*/
	API_CTRL_FLAG_NO_RESP_EXPECTED = BIT(1), /**< If set, no response is
			expected for this API call */
};
typedef uint32_t api_control_flags;


/**************************************************************************//**
@Description	Structure that defines the arguments for the call back API that registered by 
the application to receive packets from DP/AIOP to CP/GPP 
*//***************************************************************************/
struct nfpkt_buf
{
     ns_id nsid; /* Name space id*/
     void *pPkt;  /*Buffer from DP  - TBD */
};

/**************************************************************************//**
@Description	Definition of response callback function for asynchronous API
		call.
*//***************************************************************************/
typedef void (*api_resp_cbfn)(void *cbarg, int32_t cbarg_len, void *outargs);


/**************************************************************************//**
@Description	API response argumentes structure.
*//***************************************************************************/
struct api_resp_args {
	api_resp_cbfn cbfn; /**< Response callback function pointer */
	void *cbarg;	/**< Pointer to response callback argument data */
	int32_t cbarg_len;/**< Length of callback argument data */
};


/**************************************************************************//**
@Description	NFInfra notification hooks structure.

		This contains hooks for receiving unsolicited notifications.
*//***************************************************************************/
struct nfinfra_notification_hooks {
	void (*receive_packet)(
		/* TODO: describe packet structure based on NADK definitions */
			); /**<
			Handle a packet in control plane. This hook typically
			gets called for unhandled packets in data-path due to
			unknown l2 protocol or due to a request from other
			data paths to push this packet to control path. */
	void (*transmit_packet)(
		/* TODO: describe packet structure based on NADK definitions */
			); /**<
			Handle packet transmission on an interface for which
			driver is not present in data path. Typically this
			happens when data path frind packet for transmission
			on a logical interface (e.g: gre, ipip) */
};

/**************************************************************************//**
@Description	Network namespace statistics structure.
*//***************************************************************************/
struct nfinfra_netns_stats {
	uint64_t in_pkts;	/**< Ingress packets */
	uint64_t in_bytes;	/**< Egress packets */
	uint64_t out_pkts;	/**< Ingress bytes */
	uint64_t out_bytes;	/**< Egress bytes */
};

/**************************************************************************//**
@Description	Network infra structure DP capabilities structure.
*//***************************************************************************/
struct nfinfra_capabilities { 
	uint64_t capabilities; /**< List of capabilities */
};

/**************************************************************************//**
@Description	Generic output parameters structure for NFInfra NF API.
*//***************************************************************************/
struct  nfinfra_outargs{
	int32_t result;	/**< Result code of the requested operation.
			Success or error code indicating failure */
};


/**************************************************************************//**
@Description	Output arguments structure for namespace statistics retrieval
		API.
*//***************************************************************************/
struct nfinfra_netns_get_stats_outargs {
	int32_t result;	/**< Result code of the requested operation.
			Success or error code indicating failure */
	struct nfinfra_netns_stats stats; /**< namespace statistics */
};


/**************************************************************************//**
@Description Structure for configuring ethernet filtering information
*//***************************************************************************/
struct ip4fwd_link_addr{
  if_id     ifid; /* interface ID */
  char      mac_addr[MAX_ADDR_SIZE];  /* MAC Address */
};

enum  ip4fwd_link_cfg_op {
        IP4FWD_LINK_ADD=0,
        IP4FWD_LINK_DELETE
};
/**************************************************************************//**
@Description	Input parameters to  MAC address configuration
*//***************************************************************************/
struct ip4fwd_link_inargs{
	enum  ip4fwd_link_cfg_op operation; /* Operation mentions add/del */
	struct ip4fwd_link_addr ip_link_entry;
};
int32_t ip4fwd_link_config(ns_id nsid, struct ip4fwd_link_inargs *ip4fwd_link_inargs,
				api_control_flags flags,
				struct ip4fwd_outargs *ip4fwd_outargs,
				struct api_resp_args  *ip4fwd_respargs);
/**************************************************************************//**
@Function	nfinfra_api_get_version

@Description	Get the version string of NFinfra NF API.

@Param[out]	version - Pointer to string in which API version string is
		copied.

@Return		0 on Success or negative value on failure

@Cautions	None.
*//***************************************************************************/
int32_t nfinfra_api_get_version(char *version);



/**************************************************************************//**
@Function	nfinfra_api_get_capabilities

@Description	Get the capabilities of NFInfra DP.

@Param[out]	capabilities - Pointer to nfinfra_capabilities structure.

@Return		0 on Success or negative value on failure

@Cautions	None.
*//***************************************************************************/
int32_t nfinfra_api_get_capabilities(struct nfinfra_capabilities *capabilities);


/**************************************************************************//**
@Function	nfinfra_netns_add

@Description	This API adds a network namespace. This function first
		validates the incoming parameters and if all validations
		succeed, adds the entry in the database.

@Param[in]	nsid - Network namespace ID.

@Param[in]	flags - API behavioural flags.

@Param[out]	out - output arguments structure.

@Param[in]	resp - Response arguments for asynchronous call.

@Return		0 on Success or negative value on failure

@Cautions	None.
*//***************************************************************************/
int32_t nfinfra_netns_add(
			ns_id nsid,
			api_control_flags flags,
			struct nfinfra_outargs *out,
			struct api_resp_args *resp);


/**************************************************************************//**
@Function	nfinfra_netns_delete

@Description	This API deletes a network namespace in NFInfra DP.

@Param[in]	nsid - Network namespace ID.

@Param[in]	flags - API behavioural flags.

@Param[out]	out - output arguments structure.

@Param[in]	resp - Response arguments for asynchronous call.

@Return		0 on Success or negative value on failure

@Cautions	None.
*//***************************************************************************/
int32_t nfinfra_netns_delete(
			ns_id nsid,
			api_control_flags flags,
			struct nfinfra_outargs *out,
			struct api_resp_args *resp);


/**************************************************************************//**
@Function	nfinfra_bind_iface

@Description	This API binds an interface to a network namespace.

@Param[in]	nsid - Network namespace ID.

@Param[in]	ifid - interface identifier.

@Param[in]	flags - API behavioural flags.

@Param[out]	out - output arguments structure.

@Param[in]	resp - Response arguments for asynchronous call.

@Return		0 on Success or negative value on failure

@Cautions	None.
*//***************************************************************************/
int32_t nfinfra_bind_iface(
			ns_id nsid,
			if_id ifid,
			api_control_flags flags,
			struct nfinfra_outargs *out,
			struct api_resp_args *resp);



/**************************************************************************//**
@Function	nfinfra_unbind_iface

@Description	This API unbinds an interface from a network namespace.

@Param[in]	nsid - Network namespace ID.

@Param[in]	ifid - interface identifier.

@Param[in]	flags - API behavioural flags.

@Param[out]	out - output arguments structure.

@Param[in]	resp - Response arguments for asynchronous call.

@Return		0 on Success or negative value on failure

@Cautions	None.
*//***************************************************************************/
int32_t nfinfra_unbind_iface(
			ns_id nsid,
			if_id ifid,
			api_control_flags flags,
			struct nfinfra_outargs *out,
			struct api_resp_args *resp);


/**************************************************************************//**
@Function	nfinfra_bind_zone

@Description	This API binds a zone value to an interface. By default each
		interface contains zero as zone value. This API can be used to
		set a different value. As such NFInfra DP treats zone value as
		opaque.

@Param[in]	nsid - Network namespace ID.

@Param[in]	ifid - interface identifier.

@Param[in]	zone - zone value.

@Param[in]	flags - API behavioural flags.

@Param[out]	out - output arguments structure.

@Param[in]	resp - Response arguments for asynchronous call.

@Return		0 on Success or negative value on failure

@Cautions	None.
*//***************************************************************************/
int32_t nfinfra_bind_zone(
			ns_id nsid,
			if_id ifid,
			zone zone,
			api_control_flags flags,
			struct nfinfra_outargs *out,
			struct api_resp_args *resp);


/**************************************************************************//**
@Function	nfinfra_unbind_zone

@Description	This API unbinds zone value from an interface. This effectively
		set zero as zone value for the given interface.

@Param[in]	nsid - Network namespace ID.

@Param[in]	ifid - interface identifier.

@Param[in]	flags - API behavioural flags.

@Param[out]	out - output arguments structure.

@Param[in]	resp - Response arguments for asynchronous call.

@Return		0 on Success or negative value on failure

@Cautions	None.
*//***************************************************************************/
static inline int32_t nfinfra_unbind_zone(
			ns_id nsid,
			if_id ifid,
			api_control_flags flags,
			struct nfinfra_outargs *out,
			struct api_resp_args *resp)
{
	return nfinfra_bind_zone(nsid,ifid,0,flags,out,resp);
}


/**************************************************************************//**
@Function	nfinfra_netns_stats_get

@Description	This API retrieves statistics of a given network namespace.

@Param[in]	nsid - Network namespace ID.

@Param[in]	flags - API behavioural flags.

@Param[out]	out - pointer to nfinfra_netns_get_stats_outargs structure.

@Param[in]	resp - Response arguments for asynchronous call.

@Return		0 on Success or negative value on failure

@Cautions	None.
*//***************************************************************************/
int32_t nfinfra_netns_stats_get(
			ns_id nsid,
			api_control_flags flags,
			struct nfinfra_netns_get_stats_outargs *out,
			struct api_resp_args *resp);


/**************************************************************************//**
@Function	nfinfra_notification_hooks_register

@Description	Register callback hooks for receiving unsolicited notifications
                sent by NFInfra NF-DP.

@Param[out]	hooks - Pointer to nfinfra_notification_hooks structure
		containing callback function pointers.

@Return		0 on Success or negative value on failure

@Cautions	None.
*//***************************************************************************/
int32_t nfinfra_notification_hooks_register(
			struct nfinfra_notification_hooks *hooks);


/**************************************************************************//**
@Function	nfinfra_notification_hooks_deregister

@Description	Deregister NFInfra DP notification callback hooks.

@Return		0 on Success or negative value on failure

@Cautions	None.
*//***************************************************************************/
int32_t nfinfra_notification_hooks_deregister(void);

#endif /* __FSL_NFAPI_NFINFRA_H */
