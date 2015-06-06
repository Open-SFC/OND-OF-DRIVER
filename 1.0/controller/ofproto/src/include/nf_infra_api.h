
/**************************************************************************//**
@File          nf_infra_api.h

@Description   This file contains the NF-Infra NF API & related MACROS, data structures
*//***************************************************************************/

#ifndef __NFINFRA_API_H
#define __NFINFRA_API_H

typedef uint16_t nsid_t;
typedef uint16_t zone_t;
typedef uint32_t ifid_t;
typedef uint32_t ip4addr_t;

/* Need to remove after integrating nfinfra_nfapi.h file of bnsp 2.0 */
typedef uint16_t ns_id;
typedef uint16_t zone;
typedef uint32_t if_id;

/**************************************************************************//**
@Description	API behavioural flags.
*//***************************************************************************/
enum api_control_flags {
	API_CTRL_FLAG_ASYNC = 0,/**< If set, API call should be
			asynchronous. Otherwise API call will be synchronous.*/
	API_CTRL_FLAG_NO_RESP_EXPECTED = 1/**< If set, no response is
			expected for this API call */
};
typedef uint32_t api_control_flags;
/* Remove till here */

/*enum ZZZ_flags_e {
	ZZZ_FLAG_ASYNC = BIT(0),
	ZZZ_FLAG_NO_RESP_EXPECTED = BIT(1),
};*/  //Deepthi

typedef uint32_t ZZZ_flags_t;

typedef void (*api_resp_cbfn)(void *cbarg, int cbarg_len, void *outargs);

struct api_resp_args
{
	api_resp_cbfn cbfn;
	void *cbarg;
	int  cbarg_len;
};



/* structure for network namespace statistics*/
struct nfinfra_netns_stats {
  uint64_t in_pkts; /* In rcvd pkts */
  uint64_t in_bytes; /* In rcvd bytes */
  uint64_t out_pkts; /* Out sent pkts */
  uint64_t out_bytes; /* Out sent bytes*/
};

struct nfinfra_capabilities { 
   uint64_t capabilities; /* list of capabilities */
};

int32_t  nfinfra_api_get_version(char *version); /* This API would internally copy the nf infra version string to the supplied parameter */

int32_t  nfinfra_api_get_capabilities(struct nfinfra_capabilities *capabilities); /* Provides the list of nf infra DP capabilities */

/* Structure used for output arguments for NF Infra related NF API*/
struct  nfinfra_outargs{
   int32_t result;
};

/* Structure used for output arguments for NF Infra statistics related NF API */
struct  nfinfra_stats_outargs{
   int32_t result;
};
/*************************************************************************//**
@Function	nfinfra_netns_add

@Description    This API adds a network namespace. This function first validates the
		incoming parameters and if all validations succeed, adds the entry  in the 
		database.

@Param[in]	ns_id - Name Space instance ID. 	

@Param[in]	flags - Flags for the NF API. It can take the following values
			 - 0 -Synchronous call of this API. Caller will be blocked till this API 
			 	returns.
                      - ZZZ_FLAG_ASYNC : Asynchronous call of this API
                      - ZZZ_FLAG_NO_RESP_EXPECTED:  Caller of this API is not expecting response

@Param[in] nfinfra_respargs - Response arguments that will be passed to the call back when the call is asynchronous. 
                  Following fields are to by filled by the caller:
                  cbfn - Call back function pointer to be invoked
                  cbarg  - Call back function argument
                  cbarg_len - Call back function argument	length
                  
@Param[out] nfinfra_outargs - Structure that will be filled with output values of this API.
		This structure is filled when the call is synchronous or asynchronous. If asynchronous, this will
		be the last argument to the call back function nfinfra_respargs described above.
                  Following fields are filled in the structure:
                   result : Result of this API. Success or failure code in case of failure. Refer Return values of this API.

@Return		Success or Failure

		Success if a rule is added successfully else one of the following 
		error code is returned.
		- Invalid parameter value
		- Internal Memory allocation failure 		
		- TBD

@Cautions	None.
*//***************************************************************************/

int32_t nfinfra_netns_add(nsid_t nsid,
				ZZZ_flags_t flags,
				struct nfinfra_outargs *nfinfra_outargs,
				struct api_resp_args  *nfinfra_respargs);

/*************************************************************************//**
@Function	nfinfra_netns_delete

@Description    This API deletes  a network namespace. This function first validates the
		incoming parameters and if all validations succeed, the entry  will be deleted from  
		database.

@Param[in]	ns_id - Name Space instance ID. 	

@Param[in]	flags - Flags for the NF API. It can take the following values
			 - 0 -Synchronous call of this API. Caller will be blocked till this API 
			 	returns.
                      - ZZZ_FLAG_ASYNC : Asynchronous call of this API
                      - ZZZ_FLAG_NO_RESP_EXPECTED:  Caller of this API is not expecting response

@Param[in] nfinfra_respargs - Response arguments that will be passed to the call back when the call is asynchronous. 
                  Following fields are to by filled by the caller:
                  cbfn - Call back function pointer to be invoked
                  cbarg  - Call back function argument
                  cbarg_len - Call back function argument	length
                  
@Param[out] nfinfra_outargs - Structure that will be filled with output values of this API.
		This structure is filled when the call is synchronous or asynchronous. If asynchronous, this will
		be the last argument to the call back function nfinfra_respargs described above.
                  Following fields are filled in the structure:
                   result : Result of this API. Success or failure code in case of failure. Refer Return values of this API.

@Return		Success or Failure
		Success if a rule is added successfully else one of the following 
		error code is returned.
		- Invalid parameter value
		- Internal Memory allocation failure 		
		- TBD

@Cautions	None.
*//***************************************************************************/

int32_t nfinfra_netns_delete(nsid_t nsid,
				ZZZ_flags_t flags,
				struct nfinfra_outargs *nfinfra_outargs,
				struct api_resp_args  *nfinfra_respargs);

/*************************************************************************//**
@Function	nfinfra_bind_iface

@Description    This API binds Interface with a network namespace. This function first validates the
		incoming parameters and if all validations succeed, binds interface with network namespace.

@Param[in]	ns_id - Name Space instance ID. 	
@Param[in]	if_id - Interface ID. 	

@Param[in]	flags - Flags for the NF API. It can take the following values
			 - 0 -Synchronous call of this API. Caller will be blocked till this API 
			 	returns.
                      - ZZZ_FLAG_ASYNC : Asynchronous call of this API
                      - ZZZ_FLAG_NO_RESP_EXPECTED:  Caller of this API is not expecting response

@Param[in] nfinfra_respargs - Response arguments that will be passed to the call back when the call is asynchronous. 
                  Following fields are to by filled by the caller:
                  cbfn - Call back function pointer to be invoked
                  cbarg  - Call back function argument
                  cbarg_len - Call back function argument	length
                  
@Param[out] nfinfra_outargs - Structure that will be filled with output values of this API.
		This structure is filled when the call is synchronous or asynchronous. If asynchronous, this will
		be the last argument to the call back function nfinfra_respargs described above.
                  Following fields are filled in the structure:
                   result : Result of this API. Success or failure code in case of failure. Refer Return values of this API.

@Return		Success or Failure
		Success if a rule is added successfully else one of the following 
		error code is returned.
		- Invalid parameter value
		- Internal Memory allocation failure 		
		- TBD

@Cautions	None.
*//***************************************************************************/

int32_t nfinfra_bind_iface(nsid_t nsid, ifid_t ifid,
				ZZZ_flags_t flags,
				struct nfinfra_outargs *nfinfra_outargs,
				struct api_resp_args  *nfinfra_respargs);

/*************************************************************************//**
@Function	nfinfra_unbind_iface

@Description    This API unbinds Interface from a network namespace. This function first validates the
		incoming parameters and if all validations succeed, unbinds the interface from network namespace.

@Param[in]	ns_id - Name Space instance ID. 	

@Param[in]	if_id - Interface ID. 	

@Param[in]	flags - Flags for the NF API. It can take the following values
			 - 0 -Synchronous call of this API. Caller will be blocked till this API 
			 	returns.
                      - ZZZ_FLAG_ASYNC : Asynchronous call of this API
                      - ZZZ_FLAG_NO_RESP_EXPECTED:  Caller of this API is not expecting response

@Param[in] nfinfra_respargs - Response arguments that will be passed to the call back when the call is asynchronous. 
                  Following fields are to by filled by the caller:
                  cbfn - Call back function pointer to be invoked
                  cbarg  - Call back function argument
                  cbarg_len - Call back function argument	length
                  
@Param[out] nfinfra_outargs - Structure that will be filled with output values of this API.
		This structure is filled when the call is synchronous or asynchronous. If asynchronous, this will
		be the last argument to the call back function nfinfra_respargs described above.
                  Following fields are filled in the structure:
                   result : Result of this API. Success or failure code in case of failure. Refer Return values of this API.

@Return		Success or Failure
		Success if a rule is added successfully else one of the following 
		error code is returned.
		- Invalid parameter value
		- Internal Memory allocation failure 		
		- TBD

@Cautions	None.
*//***************************************************************************/

int32_t nfinfra_unbind_iface(nsid_t nsid, ifid_t ifid,
				ZZZ_flags_t flags,
				struct nfinfra_outargs *nfinfra_outargs,
				struct api_resp_args  *nfinfra_respargs);

/*************************************************************************//**
@Function	nfinfra_bind_zone

@Description    This API binds Interface with a zone. This function first validates the
		incoming parameters and if all validations succeed, binds the interface with zone. 

@Param[in]	zone_id - Name Space instance ID. 	
@Param[in]	if_id - Interface ID. 	

@Param[in]	flags - Flags for the NF API. It can take the following values
			 - 0 -Synchronous call of this API. Caller will be blocked till this API 
			 	returns.
                      - ZZZ_FLAG_ASYNC : Asynchronous call of this API
                      - ZZZ_FLAG_NO_RESP_EXPECTED:  Caller of this API is not expecting response

@ParGam[in] nfinfra_respargs - Response arguments that will be passed to the call back when the call is asynchronous. 
                  Following fields are to by filled by the caller:
                  cbfn - Call back function pointer to be invoked
                  cbarg  - Call back function argument
                  cbarg_len - Call back function argument	length
                  
@Param[out] nfinfra_outargs - Structure that will be filled with output values of this API.
		This structure is filled when the call is synchronous or asynchronous. If asynchronous, this will
		be the last argument to the call back function nfinfra_respargs described above.
                  Following fields are filled in the structure:
                   result : Result of this API. Success or failure code in case of failure. Refer Return values of this API.

@Return		Success or Failure
		Success if a rule is added successfully else one of the following 
		error code is returned.
		- Invalid parameter value
		- Internal Memory allocation failure 		
		- TBD

@Cautions	None.
*//***************************************************************************/

int32_t nfinfra_bind_zone(ifid_t ifid, zone_t zone,
				ZZZ_flags_t flags,
				struct nfinfra_outargs *nfinfra_outargs,
				struct api_resp_args  *nfinfra_respargs);

/*************************************************************************//**
@Function	nfinfra_unbind_zone 

@Description    This API unbinds Interface from a zone. This function first validates the
		incoming parameters and if all validations succeed, unbinds interface from zone.

@Param[in]	zone_id - Name Space instance ID. 	

@Param[in]	if_id - Interface ID. 	

@Param[in]	flags - Flags for the NF API. It can take the following values
			 - 0 -Synchronous call of this API. Caller will be blocked till this API 
			 	returns.
                      - ZZZ_FLAG_ASYNC : Asynchronous call of this API
                      - ZZZ_FLAG_NO_RESP_EXPECTED:  Caller of this API is not expecting response

@Param[in] nfinfra_respargs - Response arguments that will be passed to the call back when the call is asynchronous. 
                  Following fields are to by filled by the caller:
                  cbfn - Call back function pointer to be invoked
                  cbarg  - Call back function argument
                  cbarg_len - Call back function argument	length
                  
@Param[out] nfinfra_outargs - Structure that will be filled with output values of this API.
		This structure is filled when the call is synchronous or asynchronous. If asynchronous, this will
		be the last argument to the call back function nfinfra_respargs described above.
                  Following fields are filled in the structure:
                   result : Result of this API. Success or failure code in case of failure. Refer Return values of this API.

@Return		Success or Failure
		Success if a rule is added successfully else one of the following 
		error code is returned.
		- Invalid parameter value
		- Internal Memory allocation failure 		
		- TBD

@Cautions	None.
*//***************************************************************************/

int32_t nfinfra_unbind_zone(ifid_t ifid, zone_t zone,
				ZZZ_flags_t flags,
				struct nfinfra_outargs *nfinfra_outargs,
				struct api_resp_args  *nfinfra_respargs);

/*************************************************************************//**
@Function 	nfinfra_netns_stats_get

@Description  Fetches  statistics for Network namespace.

@Param[in]  ns_id - Name Space ID for which the statistics could be retrieved.

@Param[out] nfinfra_netns_stats - Structure where the statistics are stored. Following fields are updated.

  in_pkts - In rcvd pkts counter
  in_bytes - In rcvd bytes counter
  out_pkts - Out rcvd pkts counter
  out_bytes - Out rcvd bytes counter

@Param[in]	flags - Flags for the NF API. It can take the following values
			 - 0 -Synchronous call of this API. Caller will be blocked till this API 
			 	returns.
                      - ZZZ_FLAG_ASYNC : Asynchronous call of this API
                      - ZZZ_FLAG_NO_RESP_EXPECTED:  Caller of this API is not expecting response

@Param[in] nfinfra_respargs - Response arguments that will be passed to the call back when the call is asynchronous. 
                  Following fields are to by filled by the caller:
                  cbfn - Call back function pointer to be invoked
                  cbarg  - Call back function argument
                  cbarg_len - Call back function argument	length
                  
@Param[out] nfinfra_outargs - Structure that will be filled with output values of this API.
		This structure is filled when the call is synchronous or asynchronous. If asynchronous, this will
		be the last argument to the call back function nfinfra_respargs described above.
                  Following fields are filled in the structure:
                   result : Result of this API. Success or failure code in case of failure. Refer Return values of this API.

@Return   Success or Failure (more errors TBD).

@Cautions 
@Cautions	None.
*//***************************************************************************/

int32_t nfinfra_netns_stats_get(nsid_t nsid,
				ZZZ_flags_t flags,
				struct nfinfra_stats_outargs *nfinfra_outargs,
				struct api_resp_args  *nfinfra_respargs);

/*************************************************************************//**
@Function 	nfinfra_packet_hook

@Description   This API would be called from AIOP Layer If there is no registration for the packet at L2Hooks.

@Param[in]  TBD

@Param[out] TBD


@Param[out]  TBD

@Return   Success or Failure (more errors TBD).

@Cautions 
@Cautions	None.
*//***************************************************************************/

int32_t nfinfra_packet_hook(void); /* TBD */

#endif /*__NFINFRA_API_H */
