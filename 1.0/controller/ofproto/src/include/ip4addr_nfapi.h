
/**************************************************************************//**
@File          ip4addr_api.h

@Description   This file contains the IPv4 Address  NF APIs, related MACROS & data structures
*//***************************************************************************/
#ifndef __FSL_IP4ADDR_H
#define __FSL_IP4ADDR_H

/**************************************************************************//**
@Description Structure for configuring ipv4 address information
*//***************************************************************************/
struct ip4fwd_ipaddr{
  if_id     ifid; /* interface ID */
  uint32_t  ipaddr; /* IPv4 Address */
  uint32_t  dest_ip; /* Remote IPv4 Address */
  uint8_t   prefixlen; /*prefix length for netmask */
  uint32_t  bcast_addr; /* broadcast address */
  uint8_t   scope; /* scope of IPv4 address*/
  uint32_t  ipflags; /* secondary, optimistic, tentative, permanent*/
};


#define IP4FWD_BIT(x)      (1<<((x))) 

/**************************************************************************//**
@Description enum  for  ipv4 address flags 
*//***************************************************************************/
enum ip4fwd_ipflags{
IP4FWD_IF_ADDR_SECONDARY = IP4FWD_BIT(0), 
IP4FWD_IF_ADDR_OPTIMISTIC= IP4FWD_BIT(1),
IP4FWD_IF_ADDR_TENTATIVE = IP4FWD_BIT(2),
IP4FWD_IF_ADDR_PERMANENT = IP4FWD_BIT(3)
};

/**************************************************************************//**
@Description  enum  for  ipv4 address  scope flags 
*//***************************************************************************/
enum  ip4fwd_scope{
        IP4FWD_SCOPE_UNIVERSE=0,
        IP4FWD_SCOPE_SITE,
        IP4FWD_SCOPE_LINK,
        IP4FWD_SCOPE_HOST,
        IP4FWD_SCOPE_NOWHERE
};
/**************************************************************************//**
@Description  enum  for  ipv4  config operations 
*//***************************************************************************/
enum  ip4fwd_ipaddr_cfg_op{
        IP4FWD_IPADDR_ADD=0,
        IP4FWD_IPADDR_DELETE
};

/**************************************************************************//**
@Description	Input parameters to IPv4 address configuration
*//***************************************************************************/
struct ip4fwd_ipaddr_inargs{
	enum  ip4fwd_ipaddr_cfg_op operation; /* Operation mentions add/del */
	struct ip4fwd_ipaddr  ip4addr_entry;
};


/**************************************************************************//**
@Description  output arguments
*//***************************************************************************/
 struct ip4fwd_outargs
{
   int32_t result;
};

/**************************************************************************//**
@Description  enum  for  ipv4  get operations 
*//***************************************************************************/
enum  ip4fwd_ipaddr_get_op{
        IP4FWD_IPADDR_GET_FIRST=0,
        IP4FWD_IPADDR_GET_NEXT,
        IP4FWD_IPADDR_GET_EXACT
};
/**************************************************************************//**
@Description	Input parameters for fetching IPv4 address details 
*//***************************************************************************/
struct  ip4fwd_ipaddr_get_inargs{
	enum ip4fwd_ipaddr_get_op operation; /* Operation mentions get_first/
		get_next/get_exact */
	/* Following two fields are not valid for get_first */
        if_id    ifid; /* interface ID */
};

/**************************************************************************//**
@Description	out parameters for fetch to fetching IPv4 address details 
*//***************************************************************************/
struct  ip4fwd_ipaddr_get_outargs{
    ns_id ns_id; /* NamesSpace ID */
    struct ip4fwd_ipaddr  ip4addr_entry;
    int32_t result;
};

/*************************************************************************//**
@Function	ip4fwd_ipaddr_config

@Description    This API is used for ipv4 address configuration for 
		add or delete operations. This function first validates the
		incoming parameters and if all validations succeed, adds the ipv4 address entry  into the 
		database.

@Param[in]	ns_id-   network namespace id. 	
@Param[in] ip4fwd_inargs - input arguments that will be passed to this api. 
@Param[in]	operation-    configuration operations add/delete. 	
@Param[in]	ifid-  interface id of the device. 	
@Param[in]	ipaddr - the address of the interface 	
@Param[in]	dest_ip- if tunnel is present then it holds the address of the peer.
@Param[in]	prefixlen- the prefix length of the interface for calculating netmask. 
@Param[in]	bcast_addr -  the broadcast address on the interface	
@Param[in]	scope- validity of the interface address. 
@Param[in]	ipflags- ip  address flags - secondary, optimistic, tentative, permanent

@Param[in]	flags - API behavioural flags.

@Param[in] ip4fwd_respargs - Response arguments that will be passed to the call back when the call is asynchronous. 
                  Following fields are to be filled by the caller:
                  cbfn - Call back function pointer to be invoked
                  cbarg  - Call back function argument
                  cbarg_len - Call back function argument	length
                  
@Param[out] ip4fwd_outargs - Structure that will be filled with output values of this API.
		This structure is filled when the call is synchronous or asynchronous. If asynchronous, this will
		be the last argument to the call back function ip4fwd_respargs described above.
                  Following fields are filled in the structure:
                   result : Result of this API. Success or failure code in case of failure. Refer Return values of this API.

@Return		Success or Failure
		Success if a ipv4 address added/deleted successfully else one of the following 
		error code is returned.
		- Invalid parameter value
		- Internal Memory allocation failure 		
		- TBD

@Cautions	None.
*//***************************************************************************/
#if 0 
/*Commented by deepthi for now to integrate bnsp 2.0 version to get definitions 
* for api_control_flags etc */
int32_t ip4fwd_ipaddr_config(ns_id nsid, struct ip4fwd_ipaddr_inargs *ip4addr_inargs,
				api_control_flags flags,
				struct ip4fwd_outargs *ip4fwd_outargs,
				struct api_resp_args  *ip4fwd_respargs);
#endif 

/* Deepthi -defined it inline with bnsp 1.0 ver */
int32_t ip4fwd_ipaddr_config(ns_id nsid, struct ip4fwd_ipaddr_inargs *ip4addr_inargs,
                        ZZZ_flags_t flags, struct ip4fwd_outargs *outargs, struct api_resp_args *resp);

/**************************************************************************//**
@Function       ip4fwd_ipaddr_get	

@Description	This API is used to fetch ipv4 address information. This database 
		is maintained per Name Space. 
		This function first validates the incoming parameters 
		and if all validations succeed, the following is performed depending on
		the type of operation:
		if operation is get_first, fetches first entry information from ipv4 address database.
		if operation is get_next, finds the entry in the  database 
		with given information and returns the next entry.
		if operation is get_exact, finds the entry and returns it.

@Param[in]	flags - API behavioural flags.
@Param[in]	ns_id -  network namespace id.
@Param[in]	in - Pointer to input param structure. 
		which contains  ipv4 address  information.
@Param[in]	resp - Response arguments for asynchronous call.

@Param[out]	out - Pointer to output param structure
		that will be filled with output values of this API.

@Return		Success or Failure (more errors TBD)

@Cautions	None.
*//***************************************************************************/
int32_t  ip4fwd_ipaddr_get(ns_id nsid,
//	api_control_flags flags, /* Deepthi - uncomment after bnsp 2.0 integration */
        ZZZ_flags_t flags, /* Deepthi - added this for bnsp 1.0 compatibility */
	struct ip4fwd_ipaddr_get_inargs *in,
	struct ip4fwd_ipaddr_get_outargs *out,
	struct api_resp_args *resp);




#endif  /*__NF_IP4ADDR_H */

