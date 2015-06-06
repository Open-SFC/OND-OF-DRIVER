/**
 * Copyright (c) 2012, 2013  Freescale.
 *  
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/

#ifndef _FSL_EXT_H
#define _FSL_EXT_H 

//! FSL Message Extensions
/*! This module lists all the vendor extensions proposed by Freescale, not yet
 *  standardized
 *  \addtogroup FSL_Message_Extensions
 */

//! FSL Common Open Flow Extensions
/*! This module lists all the "Common Open Flow" extensions proposed by Freescale.
 *  \addtogroup FSL_Common_Extensions
 */

//! FSL L3 Forwarding Open Flow Extensions
/*! This module lists all the "L3 Forwarding Open Flow" extensions proposed by Freescale.
 *  \addtogroup FSL_L3_Forwarding_Extensions
 */

//! FSL IPsec Open Flow Extensions
/*! This module lists all the "IPsec Open Flow" extensions proposed by Freescale.
 *  \addtogroup FSL_IPsec_Extensions
 */

//! FSL Firewall Open Flow Extensions
/*! This module lists all the "Firewall Open Flow" extensions proposed by Freescale.
 *  \addtogroup FSL_Firewall_Extensions
 */

//! Extensions Error Management
/*! This module lists all the error values returned by various FSL Extensions API
* \addtogroup Error_Management
  */

//! Vendor ID
/*! This module defines the Vendor ID
* \addtogroup Vendor_ID
  */

//! FSL Actions 
/*! This module defines the FSL Actions
* \addtogroup FSL_Actions
  */

//! FSL Instructions 
/*! This module defines the FSL Instructions
* \addtogroup FSL_Instructions
  */

//! FSL Messages 
/*! This module defines the FSL Messages
* \addtogroup FSL_Messages
  */

//! Vendor ID
/*! This module defines the Vendor ID
* \addtogroup Vendor_ID
  */

/** \ingroup Vendor_ID
  * FSLX_VENDOR_ID 
  * - Freescale Vendor ID
  */
#define FSLX_VENDOR_ID 0x0000049F


/** \ingroup Vendor_ID
  * ONF_VENDOR_ID 
  * - Open Network Foundation Vendor ID
  */
#define ONF_VENDOR_ID 0x00A42305 


/** \ingroup FSL_Message_Extensions
  * FSLX_VERSION 
  * - Freescale extensions version. Versions released: 0x81 = 1.0
  */
#define FSLX_VERSION   0x81

/* Components of a FSLXM TLV header. */
#define FSLXM_HEADER__(CLASS, FIELD, HASMASK, LENGTH) \
    (((CLASS) << 16) | ((FIELD) << 9) | ((HASMASK) << 8) | (LENGTH))
#define FSLXM_HEADER(CLASS, FIELD, LENGTH) \
    FSLXM_HEADER__(CLASS, FIELD, 0, LENGTH)
#define FSLXM_HEADER_W(CLASS, FIELD, LENGTH) \
    FSLXM_HEADER__(CLASS, FIELD, 1, (LENGTH) * 2)
#define FSLXM_CLASS(HEADER) ((HEADER) >> 16)
#define FSLXM_FIELD(HEADER) (((HEADER) >> 9) & 0x7f)
#define FSLXM_TYPE(HEADER) (((HEADER) >> 9) & 0x7fffff)
#define FSLXM_HASMASK(HEADER) (((HEADER) >> 8) & 1)
#define FSLXM_LENGTH(HEADER) ((HEADER) & 0xff)

#define FSLXM_MAKE_WILD_HEADER(HEADER) \
        FSLXM_HEADER_W(FSLXM_CLASS(HEADER), FSLXM_FIELD(HEADER), FSLXM_LENGTH(HEADER))

/* FSLXM Flow match field types for OpenFlow basic class. */
enum fslxm_ofb_match_fields {
    /** IPSec SA spi */
    FSLXM_OFB_SPI_ID        = 120  
};
 
/* Masking: Arbitrary masks. */
#define FSLXM_OF_SPI    FSLXM_HEADER  (0xFFFF, FSLXM_OFB_SPI_ID, 4) 

/** \ingroup FSL_Message_Extensions 
 *  \struct fslx_header
 *  \brief Header for Freescale vendor requests and replies.\n
 *  <b>Description:</b>\n
 *  Header for Freescale vendor requests and replies. 
 */
struct fslx_header {
   /** Message type OFPT_EXPERIMENTER*/
    struct ofp_header header;           
   /** FSLX_VENDOR_ID */
    uint32_t          vendor;        
   /** Freescale Extension message sub-type */
    uint32_t          subtype;           
};
OFP_ASSERT(sizeof(struct fslx_header) == 16);

/** \ingroup FSL_Messages 
 *  \enum fslx_subtypes
 *  <b>Description:</b>\n
 *  Freescale extension message sub type definitions.
 */
enum fslx_subtypes {
    /** struct fslx_table_config*/ 
    FSLX_TABLE_CONFIG             = 1,
    /** struct fslx_bind_mod */
    FSLX_BIND_MOD                 = 2, 
    /** struct fslx_revalidation_obj*/
    FSLX_RE_VALIDATION_OBJ        = 3, 
    /** struct fslx_remove_bind_obj */
    FSLX_REMOVE_BIND_OBJ          = 4, 
    /** struct fslx_app_state_info */
    FSLX_APP_STATE_INFO           = 5, 
    /** struct fslx_pkt_out_completion */
    FSLX_PKT_OUT_COMPLETION       = 6  
};

/** \ingroup FSL_Message_Extensions 
 *  \struct onfx_multipart_request
 *  \brief Open Network Foundation Header Multipart Request.\n
 *  <b>Description:</b>\n
 * Open Network Foundation Header Multipart Request.
 */
struct onfx_multipart_request {
   /** OFPT_MULTIPART_REQUEST */
    struct ofp_header header;   
  /** OFPMP_EXPERIMENTER */
    uint16_t          type;     
  /** OFPMPF_REQ_* flags. */
    uint16_t          flags;  
  /** Allign 64 bits */
    uint8_t           pad[4];   
  /** ONF_VENDOR_ID */
    uint32_t          vendor;   
  /** ONF Extension multipart message sub-type */
    uint32_t          subtype;  
  /** Body of the multipart request. */
    uint8_t           body[0];  
};
OFP_ASSERT(sizeof(struct onfx_multipart_request) == 24);

/** \ingroup FSL_Message_Extensions 
 *  \struct onfx_multipart_reply
 *  \brief Open Network Foundation Header Multipart Reply.\n
 *  <b>Description:</b>\n
 *  Open Network Foundation vendor Header Multipart Reply.
 */
struct onfx_multipart_reply {
    /** OFPT_MULTIPART_REPLY */
    struct ofp_header header;   
   /** OFPMP_EXPERIMENTER */
    uint16_t          type;     
   /** OFPMPF_REPLY_* flags. */
    uint16_t          flags;    
   /** Allign 64 bits */
    uint8_t           pad[4];   
  /** ONF_VENDOR_ID */
    uint32_t          vendor; 
  /** ONF Extension multipart message sub-type */
    uint32_t          subtype;
 /** Body of the multipart reply. */
    uint8_t           body[0]; 
};
OFP_ASSERT(sizeof(struct onfx_multipart_reply) == 24);

/* Open Network Foundation extension message sub type definitions */
enum onfx_multipart_subtypes {
    /** Request body is empty, Reply is array of struct onfx_extension_version */
   ONFX_VENDOR_VER  = 1  
};

/** \ingroup FSL_Message_Extensions 
 *  \struct fslx_multipart_request
 *  \brief Freescale extension Header for Multipart Request.\n
 *  <b>Description:</b>\n
 * Freescale extension Header for Multipart Request.
 */
struct fslx_multipart_request {
    /** OFPT_MULTIPART_REQUEST */
    struct ofp_header header;   
   /** ONF_VENDOR_ID */
    uint32_t          vendor;   
  /** Freescale Extension multipart message sub-type */
    uint32_t          subtype; 
  /** OFPMP_EXPERIMENTER */
    uint16_t          type; 
 /** OFPMPF_REQ_* flags. */
    uint16_t          flags;   
 /** Allign 64 bits */
    uint8_t           pad[4];   
 /** Body of the multipart request. */
    uint8_t           body[0];  
};
OFP_ASSERT(sizeof(struct fslx_multipart_request) == 24);

/** \ingroup FSL_Message_Extensions 
 *  \struct fslx_multipart_reply
 *  \brief Freescale extension Header for Multipart Reply.\n
 *  <b>Description:</b>\n
 * Freescale extension Header for Multipart Reply.
 */
struct fslx_multipart_reply {
    /** OFPT_MULTIPART_REPLY */
    struct ofp_header header;   
    /** ONF_VENDOR_ID */
    uint32_t          vendor;   
    /** Freescale Extension multipart message sub-type */
    uint32_t          subtype;  
    /* OFPMP_EXPERIMENTER */
    uint16_t          type;     
    /* OFPMPF_REPLY_* flags. */
    uint16_t          flags;    
    /** Allign 64 bits */
    uint8_t           pad[4];   
    /** Body of the multipart reply. */
    uint8_t           body[0];  
};
OFP_ASSERT(sizeof(struct fslx_multipart_reply) == 24);

/* Freescale extension message sub type definitions */
enum fslx_multipart_subtypes {
    /** Request body is struct fslx_bind_obj_info_request, Reply is struct fslx_bind_obj_info_reply */
    FSLX_MP_BO_INFO  = 1 
   /** Bind object details such as number of flows attached and statistics values */  
};

/** \ingroup FSL_Actions 
 *  \enum fslx_action_subtype
 *  <b>Description:</b>\n
 * Frescale vendor actions, Following four different types of classes of actions defined  
 * FSLX_PER_PKT_ACTIONS              - Actions applied on per packet basis, these are usually applied on the packets
 *                                     that hit the flow. This is something similar to current actions defined in Open Flow 1.3.
 * FSLX_FLOW_CREATION_ACTIONS        - Actions applied during flow creation time. These are usually executed only once
 *                                     during flow creation time to trigger some task so that some logic is executed during 
 *                                     the life time of the flow. 
 * FSLX_FLOW_TERMINATION_ACTIONS     - Actions that applied during removal of the flow entry.  
 * FSLX_BIND_OBJ_CREATION_ACTIONS    - Actions that applied during creation of flow bind object.
 * FSXL_BIND_OBJ_TERMINATION_ACTIONS - Actions that applied during termination of flow bind object.
 */
enum fslx_action_subtype {
    /** struct fslx_action_attach_bind_obj*/
    FSLXAT_ATTACH_BIND_OBJ              = 1,
    /** struct fslx_action_detach_bind_obj*/
    FSLXAT_DETACH_BIND_OBJ              = 2,
    /** struct fslx_action_attach_revalidation_obj*/
    FSLXAT_ATTACH_REVALIDATION_OBJ      = 3, 
    /** struct fslx_action_detach_revalidation_obj*/
    FSLXAT_DETACH_REVALIDATION_OBJ      = 4, 
    /** struct fslx_action_set_flow_direction*/
    FSLXAT_SET_FLOW_DIRECTION           = 5, 
    /** struct fslx_action_set_app_state_info*/
    FSLXAT_SET_APP_STATE_INFO           = 6, 
    /** struct fslx_action_set_close_wait*/
    FSLXAT_SET_CLOSE_WAIT_TIME          = 7, 
    /** struct fslx_action_send_app_state_info */
    FSLXAT_SEND_APP_STATE_INFO          = 8, 
    /** struct fslx_action_execute_bind_obj_apply_actns*/
    FSLXAT_BIND_OBJECT_APPLY_ACTIONS    = 9,
    /** struct fslx_action_execute_bind_obj_write_actns*/
    FSLXAT_BIND_OBJECT_WRITE_ACTIONS    = 10,
    /** struct fslx_action_set_phy_port_cntxt */
    FSLXAT_SET_PHY_PORT_FIELD_CNTXT     = 11,
   /** struct fslx_action_meta_data_cntxt */
    FSLXAT_SET_META_DATA_FIELD_CNTXT    = 12,
   /** struct fslx_action_tunnel_id_cntxt */
    FSLXAT_SET_TUNNEL_ID_FIELD_CNTXT    = 13,
    /** struct fslx_action_start_tcp_terminate_sm*/
    FSLXAT_ENABLE_TCP_TERMINATE_SM      = 14,
    /** struct fslx_action_arp_response */
    FSLXAT_ARP_RESPONSE                 = 15,
    /** struct fslx_action_wr_metadata_from_pkt*/
    FSLXAT_WRITE_METADATA_FROM_PKT      = 16,
    /** struct fslx_action_seq_no_addition*/
    FSLXAT_SEQ_NO_DELTA_ADDITION        = 17,
    /** struct fslx_action_seq_no_validation*/
    FSLXAT_SEQ_NO_VALIDATION            = 18, 
    /** struct fslx_action_start_table */
    FSLXAT_START_TABLE                  = 19,
    /** struct fslx_action_ip_reasm */
    FSLXAT_IP_REASM                     =  20,
    /** struct fslx_action_ip_fragment */
    FSLXAT_IP_FRAGMENT                  = 21,
    /** struct fslx_action_send_icmpv4_time_exceed*/
    FSLXAT_SEND_ICMPV4_TIME_EXCEED      = 22,
    /** struct fslx_action_send_icmpv4_dest_unreachable*/
    FSLXAT_SEND_ICMPV4_DEST_UNREACHABLE = 23,
    /** struct fslx_action_drop_packet*/
    FSLXAT_DROP_PACKET                  = 24,
    /** struct fslx_create_ipsec_sa*/
    FSLXAT_CREATE_SA_CNTXT              = 25,
    /** struct fslx_delete_ipsec_sa*/
    FSLXAT_DELETE_SA_CNTXT              = 26,
    /** struct fslx_ipsec_encap*/
    FSLXAT_IPSEC_ENCAP                  = 27,
    /** struct fslx_ipsec_decap*/
    FSLXAT_IPSEC_DECAP                  = 28,
    /** struct fslx_action_header*/
    FSLXAT_POP_NATT_UDP_HEADER          = 29,
    /** struct fslx_action_header*/
    FSLXAT_WRITE_META_DATA          = 30     
};

/* Experimenter messages */

/* Macros */

/** \ingroup FSL_Message_Extensions
  * FSLX_HEADER_LEN 
  * - Freescale extension header length.
  */
#define FSLX_HEADER_LEN  sizeof(struct fslx_header)

/** \ingroup FSL_Message_Extensions
  * FSLX_PKT_OUTS_COMPLETION_MSG_LEN 
  * - Freescale extension packet out completion message length.
  */
#define FSLX_PKT_OUTS_COMPLETION_MSG_LEN sizeof(struct fslx_pkt_out_completion)

/** \ingroup FSL_Message_Extensions
  * FSLX_TABLE_CONFIG_MSG_LEN 
  * - Freescale extension table configuration message length.
  */
#define FSLX_TABLE_CONFIG_MSG_LEN  sizeof(struct fslx_table_config)

/** \ingroup FSL_Message_Extensions
  * ONFX_MULTIPART_REQUEST_LEN 
  * - Open Network Foundaton Multipart request length.
  */
#define ONFX_MULTIPART_REQUEST_LEN     sizeof(struct onfx_vendor_version)


enum fslx_exper_instruction_type {
   FSLXIT_END_INSTRUCTION_TYPE = 1,
};

/** 
 *  Applications on top of controller sends this message to a data_path for a particular flow to handle out of order packet.\n
 *  data_path. Please see details of WAIT_TO_COMPLETE_PACKET_OUTS instruction to know more about out of order
 *  handling. Since the match fields are not of fixed number, caller of this function has to build message with flow match fields
 *  similar to flow-mod message. 
 *  
 *
 * \param [in] msg  - Message descriptor with buffer allocated earlier to send switch feature request. 
 * \param [in] datapath_handle      - Handle of the data_path to which we are sending the switch feature request. 
 * \param [in] table_id  -   Identifier of the table to which this message applicable.
 * \param [in] cookie  -     cookie of a flow to which this message is applicable.
 * \param [in] cookie_mask  - mask of the above cookie.
 * \param [in] verion  - OpenFlow Protocol version to form message header accordingly.
 * \return OFU_INVALID_PARAM_PASSED
 * \return OF_SUCCESS
 */
int32_t
fslx_frame_pkt_out_completion_msg(
                      struct of_msg *msg,
                      uint64_t  datapath_handle,
                      uint8_t   table_id,
                      uint64_t  cookie,
                      uint64_t  cookie_mask,
                      uint8_t   version,
                      struct of_cntlr_conn_info **conn_info_pp
                      );

/** \ingroup FSL_Message_Extensions
  * \sa fslx_frame_table_config_msg
  * \brief Send this message to switch along with set of instructions and table id to inform it 
  * that whenever a packet hits the given table then switch has to execute the set of given instructions.\n
  *  <b>Description </b>\n
  *  Applications on top of controller sends this message to a data_path for a particular table to handle set of instructions.\n
  *  which are required to be executed before start looking up for the matching flow entry. Since the no of instructions are not fixed,\n
  *  caller of this function has to build message with required instructions similar to flow-mod message. 
  *  
  *
  * \param [in] msg - Message descriptor with buffer allocated earlier to send switch feature request. 
  * \param [in] datapath_handle	  - Handle of the data_path to which we are sending the switch feature request. 
  * \param [in] table_id  -   Identifier of the table to which this message is applicable.
  * \param [in] version	- OpenFlow Protocol version to form message header accordingly.
  * \return OFU_INVALID_PARAM_PASSED
  * \return OF_SUCCESS
  */
int32_t
fslx_frame_table_config_msg(struct of_msg *msg,
                            uint64_t  datapath_handle,
                            uint8_t   table_id,
                            uint8_t   version,
                            struct of_cntlr_conn_info **conn_info_pp
                           );

/** \ingroup FSL_Actions 
 *  \struct fslx_action_header
 *  \brief Header for Freescale defined actions.\n
 *  <b>Description:</b>\n
 *  Header for Freescale defined actions.
 */
struct fslx_action_header {
    /** OFPAT_EXPERIMENTER. */
    uint16_t type;            
    /** Length is 16. */
    uint16_t len;             
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;          
    /** FSLXAT_* */
    uint16_t subtype;         
    /** Padded to 64 bits */
    uint8_t  pad[6];
};
OFP_ASSERT(sizeof(struct fslx_action_header) == 16);

/* Header for Freescale-defined instructions. */
struct fslx_instruction_header {
    /** OFPIT_EXPERIMENTER. */
    uint16_t type;            
    /** Length is 16. */
    uint16_t len;             
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;          
   /** FSLXIT_* */
    uint16_t subtype;         
   /** Padded to 64 bits */
    uint8_t  pad[6];
};
OFP_ASSERT(sizeof(struct fslx_instruction_header) == 16);

/** \ingroup FSL_Instructions 
 *  \enum fslx_instrctn_subtype
 *  <b>Description:</b>\n
 *  Freescale vendor Instructions subtypes */
enum fslx_instrctn_subtype {
   /**struct fslx_instrctn_operation_actns */
   FSLXIT_APPLY_ACTIONS_ON_CREATE       = 1, 
   /**struct fslx_instrctn_operation_actns*/
   FSLXIT_APPLY_ACTIONS_ON_DELETE       = 2, 
   /**struct fslxit_wait_to_cmplete_cntlr_pkts*/
   FSLXIT_WAIT_TO_COMPLETE_PKT_OUTS   = 3, 
   /**struct fslxit_write_prio_register */
   FSLXIT_WRITE_PRIORITY_REGISTER	  = 5,
   
   FSLXIT_WRITE_PRIORITY_REG_FROM_CURRENT_FLOW = 6,
   /** struct fslxit_re_lookup */
   FSLXIT_RE_LOOKUP = 7,
   /**struct fslxit_re_inject_to */
   FSLXIT_RE_INJECT_TO = 8,

   FSLXIT_APPLY_GROUP = 9,
   /** struct fslx_instruction_group */
   FSLXIT_WRITE_GROUP = 10,
   /** struct fslxit_write_prio_reg_from_flow */
   FSLXIT_WRITE_PRIORITY_REG_FROM_NEXT_FLOW=11
};

/******************* Freescale Generic Open Flow Extensions - Begin ********************************/

/** \ingroup FSL_Message_Extensions 
 *  \struct fslx_table_config
 *  \brief Modification of table configuration parameters.\n
 *  <b>Description:</b>\n
 *  Freescale Table configuration message is used to define table level configuration values.
 *  Presently, it is facilitating set of instructions that will be executed when the packet 
 *  hit the table.
 */
struct fslx_table_config {
    /** ID of the table, OFPTT_ALL indicates all tables */
    uint8_t table_id; 
    /** Pad to 64 bits */
    uint8_t pad[7];   
    /** Instruction set. These instructions are executed 
     * on each packet that hit the table. Before actually doing the
     * table lookup these instructions are executed. That is these
     * are list of instructions to execute at table level on each 
     * packet that hit the table */
    struct ofp_instruction instructions[0];
};
OFP_ASSERT(sizeof(struct fslx_table_config) == 8);

/** 
 *  This message is sent from the controller after completely sending the current packets 
 *  waiting for the given flow. This message and instruction FSLXIT_WAIT_TO_COMPLETE_PKT_OUTS is used to  handle out-of-order
 *  packets. Please find details explained in FSLXIT_WAIT_TO_COMPLETE_PKT_OUTS to understand more of this message purpose. 
 */
struct fslx_pkt_out_completion
{
    /** ID of table and flow to which flow is added, to which pkt_out 
        completion request instruction added */
    uint8_t                     table_id;       
    /** Align to 64 bits. */
    uint8_t                     pad[7];          
    /** Require matching entries to contain this cookie value */
    uint64_t                    cookie;         
    /** Mask used to restrict the cookie bits that must match. 
        A value of 0 indicates no restriction. */
    uint64_t                    cookie_mask;    
    /** Flow Fields to match. Variable size. */
    struct ofp_match            match;          
};
OFP_ASSERT(sizeof(struct fslx_pkt_out_completion) == 32); 

/** \ingroup FSL_Message_Extensions 
 *  \struct onfx_vendor_version
 *  \brief Version number support to Open Flow Vendor extensions.\n
 *  <b>Description:</b>\n
 * In Open Flow 1.3, the most significant bit being set in the version field indicates an
 * experimental Open Flow version.  And 0x81 -- 0x99 range is used to define experimental
 * vendor versions. But it does not have way to define versions numbers to given 
 * vendor extension. Also vendor may support multiple vendor extension versions. Need a 
 * procedure to define different vendor extension version numbers.  
 *
 * Basically, need a way for switch to inform the controller about vendor IDs and version
 * numbers the switch is supporting.
 *
 * ONFX_EXTENSION_VER multi-part message subtype is used to define vendor extension. 
 * versions numbers. The multi-part request will not have any body. And the reply 
 * body is array of struct onfx_vendor_version.   
 *
 * One may support more than one version. If vendor supports more than one version, they 
 * must use latest version and had backward compatablity to previous verions. Length is
 * padded to 64 bits. 
 */  
struct onfx_vendor_version {
     /** Extensions supported Experimenter ID,
         Experimenter ID:
         * - MSB 0: low-order bytes are IEEE OUI.
         * - MSB != 0: defined by ONF. */
    uint32_t vendor_id;
    /** Version number of the vendor, any value between 0x81 and 0x99 */
    uint8_t version_number; 
    /** Pad to 64 bits */
    uint8_t pad[3];        
    
};
OFP_ASSERT(sizeof(struct onfx_vendor_version) == 8);

/** \ingroup FSL_Message_Extensions
 * \typedef onfx_vendor_version_cbk_fn 
 * \brief Callback function to receive vendor extension version information.\n
 * <b>Description </b>\n
 * This callback function will be passed to onfx_get_vendor_versions() API \n
 * to receive vendor experimentor version information values in the data_path. \n
 * The vendor version information are returned asynchronously at later pointer by using this callback.
 *
 * The callback function might be called multiple times with vendor information details of one\n
 * at a time depending on the amount of information available in the data_path.
 * \param [in] controller_handle  - Handle of the controller
 * \param [in] domain_handle	   - Handle of the domain
 * \param [in] datapath_handle    - Handle of the data_path.
 * \param [in] msg 		   - Message descriptor with buffer contains Open Flow message holding vendor information
 *                                            received from data_path. Controller allocates the memory required to read 
 * 		                              vendor version information received from data_path. Once application done with version information 
 *                                            received, the free function pointer available in the message descriptor will be called
 *                                            to free the  memory. 
 * \param [in] cbk_arg1   - First Application specific private info that passed in the API, if required.
 * \param [in] cbk_arg2   - Second Application specific private info that passed in the API, if required.
 * \param [in] status	    - Flag indicated the status	of response to earlier multi-part request.
 * \param [in] version_info   - Pointer to vendor version info. All the data is converted into host byte order.
 * \param [in] more_version_info	 - Boolean value, TRUE indicates, this is not final response and more details
 * 						 yet to come. FALSE means this is the last port response
 */
typedef void (*onfx_vendor_version_cbk_fn)(struct of_msg *msg,
                                          uint64_t controller_handle,
                                          uint64_t domain_handle,
                                          uint64_t datapath_handle,
                                          void	*cbk_arg1,
                                          void	*cbk_arg2,
                                          uint8_t status,
                                          struct onfx_vendor_version *vendor_version,
                                          uint8_t more_version_info);

 /** \ingroup FSL_Message_Extensions 
  *  \sa inonfx_get_vendor_versions() 
  *  \brief Send Vendor Version Information request message.\n
  *  <b>Description </b>\n
  *  API to get details of vendor experimenter extension version information present in the data_path. 
  * 
  *  The return value of ONFX_MULTIPART_REQUEST_LEN is used to allocate \n
  *  length of the message required to send Vendor Version Information request message.
  *  \param [in] msg	- Message descriptor with buffer allocated earlier to send request to get port
  *							  description.	
  *  \param [in] datapath_handle- Handle of the data_path to which we are sending the get request. 
  *  \param [in] vendor_version_info_cbk - Callback function pointer which will called once reply comes to this request.
  *  \param [in] cbk_arg1- Application private information, arg1, that will be passed to error callback in case\n
  * 								  of any errors. Also same passed to callback functions in case of success. \n
  *  \param [in] cbk_arg2 - Application private information, arg2, that will be passed to error callback in case\n
  * 								  of any errors. Also same passed to callback functions in case of success. \n
  *  \return OFU_INVALID_PARAMETER_PASSED
  *  \return OF_SUCCESS 
  */
int32_t
onfx_get_vendor_versions(struct of_msg *msg,
                         uint64_t datapath_handle,
                         onfx_vendor_version_cbk_fn vendor_version_info_cbk, 
                         void *cbk_arg1,
                         void *cbk_arg2);

/** \ingroup FSL_Message_Extensions 
 *  \enum fslx_table_feature_prop_subtype
 *  <b>Description:</b>\n
 *  Freescale extensions for table features.
 */
enum fslx_table_feature_prop_subtype {
    /** Range property. */
    FSLXTFPT_RANGE                 = 0,  
    /** lpm property. */
    FSLXTFPT_LPM                   = 1,  
    /** Flow setup rate property */
    FSLXTFPT_FLOW_SETUP_FREQUENCY  = 2   
};

/** \ingroup FSL_Message_Extensions 
 *  \struct fslx_table_feature_prop_range
 *  \brief Table feature range property.\n
 *  <b>Description:</b>\n
 *  Whether table fields supports had range values?
 */
struct fslx_table_feature_prop_range {
    /** OFPTFPT_EXPERIMENTER*/
    uint16_t   type;       
    /** Length in bytes of this property (16). */
    uint16_t   length;     
    /** FSLXTFPT_RANGE*/
    uint32_t   vendor;     
    /** Pad to 64 bits */
    uint8_t    pad[4];     
    /** FSLXTFPT_LPM */
    uint32_t   subtype;    
};
OFP_ASSERT(sizeof(struct fslx_table_feature_prop_range ) == 16);

/** \ingroup FSL_Message_Extensions 
 *  \struct fslx_table_feature_prop_lpm
 *  \brief Table feature LPM property.\n
 *  <b>Description:</b>\n
 *  Tabe type is lpm or not.
 */
struct fslx_table_feature_prop_lpm {
    /** OFPTFPT_EXPERIMENTER*/
    uint16_t   type;       
    /** Length in bytes of this property (16). */
    uint16_t   length;     
    /** FSLX_VENDOR_ID */
    uint32_t   vendor;     
    /** FSLXTFPT_LPM */
    uint32_t   subtype;    
    /** Field used for lpm */
    uint32_t   oxm_field;  
};
OFP_ASSERT(sizeof(struct fslx_table_feature_prop_lpm ) == 16);

/** \ingroup FSL_Message_Extensions 
 *  \struct fslx_table_feature_prop_flow_setup_rate
 *  \brief Table feature flow setup rate frequency property.\n
 *  <b>Description:</b>\n
 * Addition and deletion of flow entries to some
   tables happens very frequently and for some other tables addition of and deletion of flow entries 
   happens very rarely. And some packet classification algorithms like RFC will have good 
   performance compared to DCFL with respect to table lookup. But addition and deletion operation
   is very costly in RFC algorithm, DCFL will be very good algorithm with respect to addition and
   deletion operation. So it is necessary for the switch to select right algorithm and if necessary
   to know flow set up rate. This table feature indicates whether flow setup rate is high or low.
 */
struct fslx_table_feature_prop_flow_setup_rate {
    /** OFPTFPT_EXPERIMENTER */
    uint16_t   type;       
    /** Length in bytes of this property (16). */
    uint16_t   length;     
    /** FSLX_VENDOR_ID. */
    uint32_t   vendor;     
    /** FSLXTFPT_FLOW_SETUP_FREQUENCY */
    uint32_t   subtype;    
    /** One of value of FSLXTFPT_FLOW_RATE */
    uint32_t   rate;       
};
OFP_ASSERT(sizeof(struct fslx_table_feature_prop_flow_setup_rate ) == 16);

enum fsxl_flow_setup_rate {
     /** Flow setup frequency is low */  
     FSLX_FLOW_SETUP_RATE_LOW  = 0,
     /** Flow setup frequency is high */  
     FSLX_FLOW_SETUP_RATE_HIGH = 1 
};

/** \ingroup FSL_Actions 
 *  \struct fslx_action_experimenter_header
 *  \brief Action experimenter header.\n
 *  <b>Description:</b>\n
 *  Action experimenter header. 
 */
struct fslx_action_experimenter_header {
    /** OFPAT_EXPERIMENTER. */
    uint16_t type;                  
    /** Length is 16 */
    uint16_t len;                   
    /** FSLX_VENDOR_ID */
    uint32_t vendor;                
    /** FSLXAT_WRITE_METADATA_FROM_PKT */
    uint16_t subtype;     
    /** Pad to 64 bits*/           
    uint8_t  pad[6];
};
OFP_ASSERT(sizeof(struct fslx_action_experimenter_header) == 16);

/** \ingroup FSL_L3_Forwarding_Extensions 
 *  \struct fslx_action_wr_metadata_from_pkt
 *  \brief Action to write meta data from packet.\n
 *  <b>Description:</b>\n
 * This action facilitates for application to write meta_data value with specified match field value from the packet. 
 *
 * This is required in case, if application needs to extract specific field from the packet and write it
 * to meta_data and use it to match rules for the next table.
 *
 * One use case is selecting local routes, here one may needs to use destination IP of the packet 
 * to route the packet. Here destination IP with mask value will be written to meta_data and 
 * use it as a match_field in the ARP resolver table to transmit the packet on the line. 
 */
struct fslx_action_wr_metadata_from_pkt {
    /** OFPAT_EXPERIMENTER. */
    uint16_t type;                  
    /** Length is 16 */
    uint16_t len;                   
    /** FSLX_VENDOR_ID */
    uint32_t vendor;                
    /** FSLXAT_WRITE_METADATA_FROM_PKT */
    uint16_t subtype;               
   /** Pad to 64 bits */
    uint8_t  pad[2];                
   /** OXM Flow match field ID of field that need to copy from the  packet */
    uint32_t field_id;              
   /** The bitmask value, it is applicable only if bit mask is set for the field
       If bit-mask value is not set, exact match field value will be used */
    uint64_t maskval;               
};
OFP_ASSERT(sizeof(struct fslx_action_wr_metadata_from_pkt) == 24);

/** \ingroup FSL_L3_Forwarding_Extensions 
 *  \struct fslx_action_write_metadata
 *  \brief Action to write meta data.\n
 *  <b>Description:</b>\n
 * This action is similar to write metadata instruction. This action is defined to use the same
 * functionality in groups.
 */ 
struct fslx_action_write_metadata {
    /** OFPAT_EXPERIMENTER */
    uint16_t type;                  
    /** Length is 16 */
    uint16_t len;                   
    /** FSLX_VENDOR_ID */
    uint32_t vendor;                
    /** FSLXAT_WRITE_METADATA_FROM_PKT */
    uint16_t subtype;                 
    /** Pad to 64 bits */           
    uint8_t  pad[6]; 
   /** OXM Flow match field ID of field that need to copy from the  packet */
    uint64_t meta_data;              
};
OFP_ASSERT(sizeof(struct fslx_action_write_metadata) == 24);

/** \ingroup FSL_L3_Forwarding_Extensions 
 *  \struct fslx_action_start_table
 *  \brief By using this action controller can send packet_out message to data_path containing 
 *  packet to start processing from specific table.\n
 *  <b>Description:</b>\n
 * This is useful for the packet out message sending as reply to packet_in. Currently the packet in the 
 * packet_out can be send to data_path pipeline with OFPAT_OUTPUT action with port as reserved port
 * OFPP_TABLE, but here packet processing starts with table-0 and currently it is not possible to send it
 * to specific table. But it is always good to start processing packet_out from the table from which 
 * actual exception raised. This action provides facility to start processing of packet_out message
 * from specific table. 
 *
 * One use case is the application can select table ID based on incoming port, rather than always
 * starting from table-0.
 *
 * This action is applicable only to OFPT_PACKET_OUT message. 
 */
struct fslx_action_start_table {
    /** OFPAT_EXPERIMENTER. */
    uint16_t type;                  
    /** Length is 16. */
    uint16_t len;                   
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;                
    /** FSLXAT_START_TABLE. */
    uint16_t subtype;               
    /** Packet start processing from Table ID. */
    uint8_t  table_id;              
   /** Pad to 64 bits. */
    uint8_t  pad[5];                
};
OFP_ASSERT(sizeof(struct fslx_action_start_table) == 16);

/** \ingroup FSL_L3_Forwarding_Extensions 
 *  \struct fslx_action_meta_data_cntxt
 *  \brief Action to set meta-data context field.\n
 *  <b>Description:</b>\n
 * As part of packet_in message, controller receives match-field context value 'meta data'.
 * Controller or application based on the controller may likes to send 'physical port' as part of
 * packet_out message to switch. This is done by sending FSLXAT_SET_META_DATA_FIELD_CNTXT action 
 * in the packet_out message with 'meta data' extracted from packet_in messages as a parameter.  
 * 
 * For this controller extracts meta data value received in Packet_in and copy meta data value
 * as a paramter to action FSLXAT_SET_META_DATA_FIELD_CNTXT in OFPT_PACKET_OUT message.
 */
struct fslx_action_meta_data_cntxt {
    /** OFPAT_EXPERIMENTER. */
    uint16_t type;      
    /** Length is 24. */
    uint16_t len;              
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;   
    /** FSLXAT_SET_META_DATA_FIELD_CNTXT */
    uint16_t subtype;            
    /** Pad to 64 bits. */
    uint8_t  pad[6];           
    /** Meta data value extracted from packet_in message*/           
    uint64_t meta_data;       
};
OFP_ASSERT(sizeof(struct fslx_action_meta_data_cntxt) == 24);

/** \ingroup FSL_L3_Forwarding_Extensions 
 *  \struct fslx_action_tunnel_id_cntxt
 *  \brief Action to set tunnel-id context field.\n
 *  <b>Description:</b>\n
 * As part of packet_in message, controller receives match-field context value 'tunnel id'.
 * Controller or application based on the controller may likes to send 'tunnel id' as part of
 * packet_out message to switch. This is done by sending FSLXAT_SET_TUNNEL_ID_FIELD_CNTXT action 
 * in the packet_out message with 'tunnel id' that extracted from packet_in messages as a parameter.  
 * 
 * For this controller extracts tunnel id value received in Packet_in and copy tunnel id value
 * as a paramter to action FSLXAT_SET_TUNNEL_ID_FIELD_CNTXT in OFPT_PACKET_OUT message.
 */
struct fslx_action_tunnel_id_cntxt {
    /** OFPAT_EXPERIMENTER. */
    uint16_t type;              
    /** Length is 24. */
    uint16_t len;               
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;            
    /** FSLXAT_SET_TUNNEL_ID_FIELD_CNTXT */
    uint16_t subtype;           
    /** Pad to 64 bits. */
    uint8_t  pad[6];            
    /** Tunnel Id value extracted from packet_in message*/           
    uint64_t tunnel_id;         
};
OFP_ASSERT(sizeof(struct fslx_action_tunnel_id_cntxt) == 24);

/** \ingroup FSL_L3_Forwarding_Extensions 
 *  \struct fslx_action_arp_response
 *  \brief Action to offload ARP Response.\n
 *  <b>Description:</b>\n
 * Controller can offload ARP reponse for switch port by using this action.
 * As part of this action, following actions are executed on the ARP packet
 * received
   - Swap Source and Destination IP addresses
   - Set ARP packet type to response
   - Send packet to port on which ARP request is received. */
struct fslx_action_arp_response {
    /** OFPAT_EXPERIMENTER. */
    uint16_t type;                  
    /** Length is 16. */
    uint16_t len;                   
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;                
    /** FSLXAT_ARP_RESPONSE*/ 
    uint16_t subtype;               
    /** MAC address of the target */
    uint8_t  target_mac[6];         
    /** port used */
    uint32_t port;
    /** Pad to 64 bits. */
    uint8_t  pad[4];                
};
OFP_ASSERT(sizeof(struct fslx_action_arp_response) == 24);

/** \ingroup FSL_Common_Extensions 
 *  \struct fslx_action_drop_packet
 *  \brief Action to drop packet.\n
 *  <b>Description:</b>\n
 * Sometimes situation arises to drop packets due
 * to firewall filters or some other reason. This action is used to drop the packet. At times
 * depending on case basis one needs to send an error along with droping packet. This action
 * is not talking about sending of error messages, it just used to drop packet. 
 */
struct fslx_action_drop_packet {
    /** OFPAT_EXPERIMENTER. */
    uint16_t type;              
    /** Length is 16. */
    uint16_t len;               
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;           
    /** FSLXAT_DROP_PACKET*/
    uint16_t subtype;           
    /** Pad to 64 bits. */
    uint8_t  pad[6];            
};
 
/** \ingroup FSL_Common_Extensions 
 *  \struct fslx_instrctn_operation_actns
 *  \brief Instruction operation actions.\n
 *  <b>Description:</b>\n
 * FSLX_APPLY_ACTIONS_ON_CREATE - Apply all actions immediately during addition/modification
 *                                operation.
 * FSLX_APPLY_ACTIONS_ON_DELETE - Apply all actions immediately during deletion operation.
 *   
 * Controller usually changes different switch states at different times. For example controller might 
 * add or delete flow entry to flow table. This instruction is used to define set of actions that executed
 * during addition/modification and delete command operation that used to change state. 
 *
 * For example, if controller is sending OFPT_FLOW_MOD message with ADD command and contains one of 
 * instruction as FSLX_APPLY_ACTIONS_ON_CREATE, all the actions listed as part of this 
 * instruction will be executed during addition of flow entry into flow table in the switch. 
 *
 * In the Open Flow 1.3 spec all the instructions defined with the flows are applied on the each packet
 * only when packet hit flow. And don't have facility to execute set of actions during flow addition or
 * modification or deletion. 
 *
 * With FSLX_APPLY_ACTIONS_ON_CREATE instruction, controller can define set of actions that are applied during addition
 * or modification of flow entry into flow table. Note that the actions in this instructions are executed only 
 * during flow creation time. And these are not per packet actions, means actions listed here are not applied
 * on the each packets that hit the flow. It is new class of action list.    
 *
 * These actions usually change state of object entity to which it is attached. Some other purpose of 
 * these actions is to trigger some task or logic to run during object entity life time.
 *
 * The FSLX_APPLY_ACTIONS_ON_CREATE is not only attached to OFPT_FLOW_MOD but it can also be
 * attached to other objects like bind objects as part of FSLX_BIND_MOD message. 
 *
 * The instruction FSLX_APPLY_ACTIONS_ON_DELETE is, counter part instruction of * FSLX_APPLY_ACTIONS_ON_CREATE, used to define 
 * list of actions to execute during DELETION command operation. For example, these actions usually execute once 
 * during flow removal time. In general actions described here stops the task or logic that is triggered 
 * due to execution of action list in FSLX_APPLY_ACTIONS_ON_CREATE during flow creation time.
 */
struct fslx_instrctn_operation_actns {
    /** OFPAT_EXPERIMENTER. */
    uint16_t type;                  
    /** Length is 16. */
    uint16_t len;                   
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;                
    /** FSLX_APPLY_ACTIONS_ON_CREATE or FSLX_APPLY_ACTIONS_ON_DELETE */
    uint16_t subtype;           
    /** Pad to 64 bits. */
    uint8_t  pad[6];            
    /** List of actions, variable size */
    struct ofp_action_header  actions[0]; 
};
OFP_ASSERT(sizeof(struct fslx_instrctn_operation_actns) == 16);
/** 
 * Instruction asking switch to wait for the completion of all current
 * packet_outs queued for the flow in the controller before sending out current and new packets that hit 
 * the flow. 
 *
 * Controller sometimes may not push OFPT_FLOW_MOD message immediately after receiving the 
 * packet_in message. In meantime there is a possibility of controller to receive many packet_in messages
 * due to continous switch miss-exceptions and it is expected for the controller to queue recieved packet_in
 * locally. 
 *
 * After adding flow entry into the switch, controller usually sends all the earlier received packet in messages
 * as packet_out messages to switch, and switch sends those pacekts to output port. But in meantime there 
 * is possibility of receiving new packet on the switch for same flow after flow entry addition from controller,  
 * now these packets hits the flow entry and sent to output port. Because of this there is a possibility of sending
 * out of order packets on the output port.
 *
 * Both instruction FSLXIT_WAIT_TO_COMPLETE_PKT_OUTS and FSLX_PKT_OUT_COMPLETION_MESSAGE are used to solve the packets
 * out of order issue.
 *
 * Instruction FSLXIT_WAIT_TO_COMPLETE_PKT_OUTS switch to wait for the completion of all current packet outs queued for 
 * the flow in the controller. Controller as part of adding flow entry, the instruction FSLXIT_WAIT_TO_COMPLETE_PKT_OUTS 
 * will be added to flow with ‘state’ as FSLXPS_WAIT. 
 *
 * This instruction must always push as first instruction of the flow.
 *
 * As part of the switch, if any traffic hits flow which has FSLXIT_WAIT_TO_COMPLETE_PKT_OUTS instruction, and state as
 * FSLXPS_WAIT, subsequent instructions attached to the flow will not be executed. Instead it waits for the completion of 
 * current packet outs waiting at controller.
 *
 * The decision of whether dropping or queuing of current packet that hit the instruction is outside the
 * scope. It depends purely on switch implementation.
 *
 * After switch receiving the message FSLX_PKT_OUT_COMPLETION_MESSAGE, the flow instruction FSLXIT_WAIT_TO_COMPLETE_PKT_OUTS 
 * the status will be changed to FSLXPS_READY.
 * Then all the subsequent packets, for the same flow send to next instruction for execution.
 */
struct fslxit_wait_to_cmplete_cntlr_pkts {
    /** OFPIT_EXPERIMENTER */
    uint16_t type;              
    /** Length of this struct in bytes */
    uint16_t len;               
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;            
    /** FSLXIT_WAIT_TO_COMPLETE_PKT_OUTS */
    uint16_t subtype;           
    /** Current Packet out complete status, one of FSLXPS_* value */
    uint16_t state;             
    /** Align to 64-bits */
    uint8_t  pad[4];            
};
OFP_ASSERT(sizeof(struct fslxit_wait_to_cmplete_cntlr_pkts) == 16);

/** 
 * Freescale packet out completion state values*/
enum fslx_pkt_out_cmplete_status {
    /** Waiting for controller to send all packet outs */ 
    FSLXPS_WAIT  = 1, 
   /** Received all the packet outs from controller */
    FSLXPS_READY = 2 
};

/** \ingroup FSL_L3_Forwarding_Extensions 
 *  \struct fslxit_write_prio_register
 *  \brief Instruction used to write immediate priority value into priority register of the packet.\n
 *  <b>Description:</b>\n
 * Instruction used to write immediate priority value into priority register of the packet.
 * At present, there is only one priority register and it can be enhanced to multiple registers by defining priority register array.
 */
struct fslxit_write_prio_register	{
    /** OFPIT_EXPERIMENTER */
    uint16_t type;              
    /** Length of this struct in bytes */
    uint16_t len;               
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;            
    /** FSLXIT_WRITE_PRIORITY_REGISTER */
    uint16_t subtype;        
    /** Priority*/   
    uint16_t priority_value;
    /** Align to 64-bits */
    uint8_t  pad[4];            
};
OFP_ASSERT(sizeof(struct fslxit_write_prio_register) == 16);

/** \ingroup FSL_L3_Forwarding_Extensions 
 *  \enum fslx_prio_reg_type
 *  <b>Description:</b>\n
 * Instruction used to write priority value of the next flow entry of the matched
 * flow entry into priority register.
 */
enum fslx_prio_reg_type {
                /** lookup starts from first flow of the table */
		FSLX_PRIO_REG_NONE = 0, 
                /** lookup starts from flow of priority given in the priority register */
		FSLX_PRIO_REG_SAME = 1, 
                /**  lookup starts from next flow of the flow priority given in the priority register */
		FSLX_PRIO_REG_NEXT = 2 
};

/** \ingroup FSL_L3_Forwarding_Extensions 
 *  \struct fslxit_write_prio_reg_from_flow
 *  \brief Instruction used to write immediate priority value.\n
 *  <b>Description:</b>\n
 *
*/
struct fslxit_write_prio_reg_from_flow	{
    /** OFPIT_EXPERIMENTER */
    uint16_t type;              
    /** Length of this struct in bytes */
    uint16_t len;               
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;            
    /** FSLXIT_WRITE_PRIORITY_REG_FROM_NEXT_FLOW */
    uint16_t subtype;           
    /** Align to 64-bits */
    uint8_t  pad[6];            
};
OFP_ASSERT(sizeof(struct fslxit_write_prio_reg_from_flow) == 16);


/** \ingroup FSL_Common_Extensions 
 *  \struct fslxit_re_lookup
 *  \brief Instruction used to submit the packet to the same table.\n
 *  <b>Description:</b>\n
 * If priority register is set then packet will be submitted to the given
 * priority flow entry level and start lookup from that entry, else lookup start from first entry.
 */
struct fslxit_re_lookup	{
    /** OFPIT_EXPERIMENTER */
    uint16_t type;              
    /** Length of this struct in bytes */
    uint16_t len;               
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;            
    /** FSLXIT_RE_LOOKUP */
    uint16_t subtype;           
    //uint8_t	 is_set_prio_reg;   /* TRUE, if the priority value is set in priority register,  else FALSE */
    /** TRUE, if the priority value is set in priority register,  else FALSE */
    uint8_t  prio_reg_type;
    /** Align to 64-bits */
    uint8_t  pad[5];            
};
OFP_ASSERT(sizeof(struct fslxit_re_lookup) == 16);

/** \ingroup FSL_Common_Extensions 
 *  \struct fslxit_re_inject_to
 *  \brief Instruction used to submit the packet to one of the previous tables.\n
 *  <b>Description:</b>\n
 * If priority register is set then packet will be submitted to the given
 * priority flow entry level and start lookup from that entry, else lookup start from first entry.
 */
struct fslxit_re_inject_to	{
    /** OFPIT_EXPERIMENTER */
    uint16_t type;              
    /** Length of this struct in bytes */
    uint16_t len;               
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;            
    /** FSLXIT_RE_INJECT_TO */
    uint16_t subtype;           
    /** Packet start processing from Table ID. */
    uint8_t  table_id;
	//uint8_t	 is_set_prio_reg;   /* TRUE, if the priority value is set in priority register else FALSE */
    /** TRUE, if the priority value is set in priority register else FALSE */
    uint8_t  prio_reg_type;
    /** Align to 64-bits */
    uint8_t  pad[4];            
};
OFP_ASSERT(sizeof(struct fslxit_re_inject_to) == 16);



/** \ingroup FSL_Common_Extensions 
 *  \struct fslx_instruction_group
 *  \brief This instruction is similar to apply group action.\n
 *  <b>Description:</b>\n
 *  Packet submitted to apply group action
 *  will be consumed by that group itself. Packet submitted to write group instruction will return
 *  and can continue from next instruction onwards.
 */
struct fslx_instruction_group {
       /** OFPIT_EXPERIMENTER */
       uint16_t type;              
       /** Length of this struct in bytes */
       uint16_t len;               
       /** FSLX_VENDOR_ID. */
       uint32_t vendor;            
       /** FSLXIT_WRITE_GROUP */
       uint16_t subtype;           
       /** Align to 64-bits */
       uint8_t  pad[2];            
       /** Group identifier. */
       uint32_t group_id;          
};
OFP_ASSERT(sizeof(struct fslx_instruction_group) ==16);

/** \inExperimenter Instructions */

/*
 * FSLX_APPLY_ACTION_DURING_ADD_MOD_DEL_LEN
 * - Returns length of buffer required for apply_action_during add_mod_del_instruction. 
 *   Along with including following instruction length, applications needs to add 
 *   lengths of actions that are pushing to apply_action_during_add_mod_del_instruction.
 */
#define FSLXIT_WRITE_GROUP_ACTIONS_LEN sizeof(struct fslx_instruction_group)
#define FSLXIT_APPLY_GROUP_ACTIONS_LEN sizeof(struct fslx_instruction_group)

#define FSLXIT_APPLY_ACTION_ON_CREATE_OR_DELETE_INSTRUCTION_LEN sizeof(struct fslx_instrctn_operation_actns)

#define FSLXIT_WAIT_TO_COMPLETE_PKT_OUTS_LEN  sizeof(struct fslxit_wait_to_cmplete_cntlr_pkts)

#define FSLXIT_WRITE_PRIORITY_REG_LEN			sizeof(struct fslxit_write_prio_register)

#define FSLXIT_WRITE_PRIORITY_REG_FROM_FLOW_LEN sizeof(struct fslxit_write_prio_reg_from_flow)

#define FSLXIT_RE_LOOKUP_LEN			sizeof(struct fslxit_re_lookup)

#define FSLXIT_RE_INJECT_TO_LEN			sizeof(struct fslxit_re_inject_to)

/********************** Freescale Generic Open Flow Extensions - End      ****************************/

/********************** Freescale Common Open Flow Extensions - Begin     ***************************/

/** \ingroup FSL_Common_Extensions 
 *  \struct fslx_bind_mod
 *  \brief Modification of Bind Object Table from the controller. This is used to
 *  add/modify/delete flow bind objects.\n
 *  <b>Description:</b>\n
 * In the Open Flow protocol environment each flow works independently from other flows.
 * By defining bind-object, one can bind multiple set of flows to the object and perform necessary 
 * functionality commonly among all the flows. 
 *
 * One use case of bind-object is binding two flows in Firewall application.  
 * Controller usually adds two flows separately in its table in the data-path for Firewall sessions,
 * one in the forward direction from client to server and another in the reverse direction 
 * from server to client. It is required to combine both forward and reverse flows 
 * and need to perform some functionality commonly to both the flows. 
 *
 * One more use case is combining different flows, based on selector belongs to same IPSec Secure
 * Association context.
 *
 * Controller attaches flows to bind-object during flow creation time by using FSLXAT_ATTACH_BIND_OBJ 
 * action as part of FSLX_APPLY_ACTIONS_ON_CREATE instruction of OFPT_FLOW_MOD message.
 */
struct fslx_bind_mod {
   /** ID of flow bind object. The bind_obj_id is generated by 
       controller and it should be unique within data-path. */
   uint32_t   bind_obj_id;            
   /** Application type, any one of FSLX_BO_APP_* */ 
   uint8_t    type;                        
   /** Command used to Add or Delete bind object to the table */
   uint8_t    command;                     
   /** Any one of FSLX_BO_DF_* value */
   uint8_t    remove_flow_config;          
   /** Any one of FSLX_BO_DB_* value */
   uint8_t    del_bind_obj_config;         
   /** Opaque controller-issued identifier. */
   uint64_t   cookie;                      
   /** Life time of flows in seconds. It is life time of flows attached to bind-object 
                                              defined as per time elapsed. If this value is other than zero, removes all the flows 
                                              attached to bind_object after elapsing lifetime_as_timeout value. The lifetime_as_timeout
                                              is fixed time elapsed after bind_object creation. The timeout happens irrespective of 
                                              whether any packet hits any one of the flow. After removing all flows, the treatment on
                                              bind-object is as per the configuration value defined in del_bind_obj_config.*/
   uint64_t   life_time_as_timeout;
   /** Life time of flows attached to bind-object defined as per bytes of data processed. 
                                              If this value is other than zero, removes all the flows attached to the bind_object
                                              after processing of lifetime_as_no_of_bytes elapsed. The lifetime_as_no_of_bytes is sum 
                                              of bytes of data that hit all the flows of bind_object. After removing all flows, the 
                                              treatment on bind-object is as per the configuration value defined in del_bind_obj_config.*/
   uint64_t   life_time_as_no_of_bytes;
   /** Life time of flows in packets. It defines life time of the flows attached to bind-object 
                                              as per the number of packets processed. If this value is other than zero, removes all the
                                              flows attached to bind_object after processing of lifetime_as_no_of_packets. The 
                                              lifetime_as_no_of_packets is sum of packets that hit all the flows of bind_object. After 
                                              removing all flows, the treatment on bind-object is as per the configuration value defined
                                              in del_bind_obj_config.*/
   uint64_t   life_time_as_no_of_packets;
   /** Inactivity timeout in seconds. When traffic doesn’t hit any one of the 
                                              flow during idle_timeout period, it removes all flows attached in the bind-object. 
                                              The treatment on bind-object after removing all the flows is as per the configuration
                                              value defined in del_bind_obj_config. When traffic hits in any one of the flow the 
                                              idle_timeout will be reset. The idle_timeout will always be started only after 
                                              attaching atleast any one of single flow to the bind-object. */
   uint64_t   idle_timeout;
   /** Instruction applies commonly to all flows attached to the bind-object,
                                              it contains actions during bind object creation and deletion, also 
                                              contains actions applied at run time packets that hit the flow */
   
   struct ofp_instruction instructions[0]; 
};
OFP_ASSERT(sizeof(struct fslx_bind_mod) == 48);

/** \ingroup FSL_Common_Extensions 
 *  \enum fslx_obj_config_flags
 *  <b>Description:</b>\n
 * Configuration flags used bind_object.
*/ 
enum fslx_obj_config_flags {
   /** Delete command should remove all the attached flows implicitly
       without throwing any error, this is default operation */
   FSLX_OC_DEL_CMD_REMOVE_FLOWS_IMPLICIT= 1 << 0, 
   /** Delete command should check, whether flow attached or not, 
       if attached, delete command should fail and throw error.*/
   FSLX_OC_DEL_CMD_REMOVE_FLOWS         = 1 << 1 
}; 

/** \ingroup FSL_Common_Extensions 
 *  \enum fslx_bo_application_types
 *  <b>Description:</b>\n
 * Following Application Types are currently defined for bind-object. Application specific action FSLXAT_SET_STATE_BLOB
 * will usually be attached as part FSLX_APPLY_ACTIONS_ON_CREATE instruction as part of bind-object, in case 
 * application specific data is required. 
*/
enum fslx_bo_application_types {
  /** Firewall application */
  FSLX_BO_APP_FIREWALL = 1,
  /**IPSEC application*/
  FSLX_BO_APP_IPSEC,
  /** Any generic application, The ID is useful in the place where some applications want to 
      use bind-object but doesn’t have any application specific state variables and type */ 
  FSLX_BO_GENERAL_APP  = 0xFF 
};

/** \ingroup FSL_Common_Extensions 
 *  \enum fslx_bind_obj_mod_command
 *  <b>Description:</b>\n
 *  Flow Bind object Commands.
*/
enum fslx_bind_obj_mod_command {
   /** New bind object */
    FSLX_BC_ADD   =1, 
    /** Delete bind object, it deletes current bind object of given bind_obj_id. The treatment of flows when
                         delete bind-object command received is as per configuration value defined in remove_flow_config. 
                         Switch will send FSLX_REMOVE_BIND_OBJ event with reason of deletion after deleting the bind object. */
    FSLX_BC_DELETE=2  
};

/** \ingroup FSL_Common_Extensions 
 *  \enum fslx_delete_flows_with_bind_obj_del
 *  <b>Description:</b>\n
 *  Configuration value for the treatment of flows attached in the bind-object when delete command is received.
 */
enum fslx_delete_flows_with_bind_obj_del {    
  /** Delete command of bind-object should not remove all the attached flows,
      it simply detaches flows from the bind-object */
  FSLX_BO_DF_DONT_REMOVE_FLOWS       = 0, 
  /** Delete command of bind-object should remove all the attached flows implicitly 
      without sending any error to the controller */    
  FSLX_BO_DF_REMOVE_FLOWS_IMPLICIT   = 1, 
  /** Delete command of bind-object send an error to the controller indicating that 
                                            currently flows are attached and should not remove flows */
  FSLX_BO_DF_SEND_ERROR_TO_CNTLR      = 2  
};

/** \ingroup FSL_Common_Extensions 
 *  \enum fslx_remove_bind_obj_after_flow_removes
 *  <b>Description:</b>\n
 *  Configuration value used for the treatment of bind-object after removing all flows in the bind_object.
 */
enum fslx_remove_bind_obj_after_flow_removes {
    /** Once removed all flows in the bind object delete bind_object itself. */
    FSLX_BO_DB_DEL_BIND_OBJ      = 0, 
    /** Don’t delete bind-object after removing all flows */
    FSLX_BO_DB_DONT_DEL_BIND_OBJ = 1 
};

/** \ingroup FSL_Common_Extensions 
 *  \struct fslx_revalidation_obj
 *  \brief Add and Delete Revalidation Object.\n
 *  <b>Description:</b>\n
 * For a given ACL rule maintained in the controller,
 * every packet that hit the rule, one or more flows might be created in the switch. Due to the modification/deletion 
 * of ACL rule or change in the priority of the rule, the flows that already created by the rule in the  switch might not be 
 * valid and one need to revalidate and delete the flows.
 * To achieve this functionality, it needs to group all the flows created by same ACL rule, for this it need to
 * create object called 'revalidation-object' and attach the corresponding flows to 'revalidation-object'.
 *
 * FSLXAT_ATTACH_REVALIDATION_OBJ action as part of FSLX_APPLY_ACTIONS_ON_CREATE instruction is used to
 * attach flow to Revalidation-Object. 
 */
struct fslx_revalidation_obj {
   /** ID of revalidation object */
   uint32_t               revaidation_obj_id; 
   /** Command to Add or Delete revalidation object to the table */
   uint16_t               command;  
   /** Align to 64 bytes */
   uint8_t                pad[2]; 
   /** Instruction to execute during addition/deletion of Revalidation-Object */
   struct ofp_instruction instructions[0]; 
};
OFP_ASSERT(sizeof(struct fslx_revalidation_obj) == 8);

/** \ingroup FSL_Common_Extensions 
 *  \enum fslx_revalidation_obj_mod_command
 *  <b>Description:</b>\n
 *  Revalidation object Commands */
enum fslx_revalidation_obj_mod_command {
    /** New revalidation object */
    FSLX_RC_ADD   =1, 
    /** Delete revalidation object, it deletes all the flows attached to the revalidation-object
                         and deletes revalidation-object itself */
    FSLX_RC_DELETE=2  
};

/** \ingroup FSL_Common_Extensions 
 *  \struct fslx_remove_bind_obj
 *  \brief Delete bind object.\n
 *  <b>Description:</b>\n
 *  Asynchronous message notification to send delete bind object event to the controller.
 */
struct fslx_remove_bind_obj {
    /** Opaque controller-issued identifier. */
    uint64_t    cookie;  
    /** ID of bind object to deleted */
    uint32_t    bind_obj_id;  
    /** delete reason, any one FSLX_BO_RR_* */
    uint8_t     reason;  
    /** Application type, any one of FSLX_BO_APP_* */
    uint8_t     type;    
    /** Length of state variables blob data, this value will be zero, if no application state 
                            variables attached by FSLXAT_SET_STATE_BLOB action. The length need not be same
                            as length passed as part of the action. */
    uint16_t    length;  
    /** Application specific state variables data, if FSLXAT_SET_STATE_BLOB action is pushed */
    uint8_t     data[0]; 
};
OFP_ASSERT(sizeof(struct fslx_remove_bind_obj) == 16);
 
/** \ingroup FSL_Common_Extensions 
 *  \enum fslx_bind_obj_remove_reason
 *  <b>Description:</b>\n
 *  Reason flag sent as part of asynchronous message to send deletion of bind-object.
 */
enum fslx_bind_obj_remove_reason {
  /** Delete bind-object issued by controller */
  FSLX_BO_RR_DEL_CMD           = 0, 
  /** All attached flows to bind-object are evicted because of time-out or issue of 
                                     flow delete command or any other reason. And del_bind_obj_config value of
                                     bind-object set as FSLX_BO_DB_DEL_BIND_OBJ */
  FSLX_BO_RR_ALL_FLOWS_EVICTED = 1,  
  /** idle time exceeded idle_timeout. */
  FSLX_BO_RR_HARD_TIMEOUT = 3,     
  /** Time exceeded hard_timeout. */
  FSLX_BO_RR_IDLE_TIMEOUT = 2,     
  /** Unable to Delete bind-object, flows are attached */
  FSLX_BO_RR_FLOW_ATTACHED_TO_BIND_OBJ = 4 
};

/** \ingroup FSL_Common_Extensions 
 *  \struct fslx_action_set_app_state_info
 *  \brief Action to set state variable.\n
 *  <b>Description:</b>\n
 * This action in general will be part of FSLX_APPLY_ACTIONS_ON_CREATE either
 * in OFP_FLOW_MOD or FSLX_BIND_MOD messages, but currently it is used only as part of FSLX_BIND_MOD. By using this action 
 * application can attach any application specific private data to flow or bind object. 
 *
 * This is purely application specific action and there by corresponding application specific action will be executed by
 * using application type field. As part of this action, it will initialize and attach application specific private data variables. 
 *
 * Application specific actions that are executed might use or update these variable is outside the scope of Open Flow. These state 
 * variables are opaque to Open Flow and used purely within application running as part of data-path. 
 *
 * This data will be sent to controller as part of FSLX_REMOVE_BIND_OBJ and FSLX_APP_STATE messages.
 */ 
struct fslx_action_set_app_state_info {
   /** OFPAT_EXPERIMENTER. */
   uint16_t type;               
   /** Length is 16. */
   uint16_t len;                
   /** FSLX_VENDOR_ID. */
   uint32_t vendor;         
   /** FSLXAT_SEND_APP_STATE_INFO */
   uint16_t subtype;            
   /** Pad to 64 bits */
   uint8_t  pad[2];             
   /** Length of state variables data */
   uint32_t length;             
   /** application specific state variables data */
   uint8_t  data[0] ;           
};
OFP_ASSERT(sizeof(struct fslx_action_set_app_state_info) == 16);
 
/** \ingroup FSL_Common_Extensions 
 *  \struct fslx_app_state_info
 *  \brief Asynchronous message notification to send application state.\n
 *  <b>Description:</b>\n
 * This event will be sent to controller due to initialization of FSLXAT_SEND_ONCE_APP_STATE or
 * FSLXAT_SEND_PERIODIC_APP_STATES action. Some applications like
 * IPSec running in the controller may register for the events to receive application state of data in datapath.
 */
struct fslx_app_state_info {
    /** Bind object ID created by controller application*/
    uint32_t bind_obj_id; 
    /** One of FSLX_AS_EM_* value */
    uint8_t  event_method;     
    /** One of FSLX_AS_ET_* value */
    uint8_t  event_type;       
    /** Application type, any one of FSLX_BO_APP_* */ 
    uint8_t  type;             
    /** Pad to 64 bits */
    uint8_t  pad[1];           
    /** Opaque controller-issued identifier. */
    uint64_t cookie;          
    /** Count of the given event_type occured, 
                                  for example if event_type is FSLX_PE_BYTES, 
                                  this field contains number of bytes count of
                                  data processed 
                                */
    uint64_t event_type_count;  
    /** Pad to 64 bits */
    uint8_t  pad2[6];     
    /** Length of state variables blob, this value will be zero
                             if no application state variables attached by
                             FSLXAT_SET_APP_DATA action. */
    uint16_t length;      
    /** Application specific state variables data, 
                             if available */
    uint8_t  data[0] ;    
};
OFP_ASSERT(sizeof(struct fslx_app_state_info) == 32);

/** \ingroup FSL_Common_Extensions 
 *  \struct fslx_bind_obj_info_request
 *  \brief Get Bind Object Information.\n
 *  <b>Description:</b>\n
 * Controller can query any bind-object details at any point of time by using
 * multipart message FSLX_MP_BO_INFO.
 * Following data-structure is used as bind-object information multipart request
 */

/* Bind Object numbering. bind object id can use any number up to FSLX_BO_MAX. */
enum fslx_bind_object_number {
    /** Last usable Bind Object number. */
    FSLX_BO_MAX        = 0xfffffffe,

    /* Fake Bind Object Id. */
    /** Wildcard Bind Object used for 
    bind obj stats and bind obj deletes. */
    FSLX_BO_ALL        = 0xffffffff

 };

struct fslx_bind_obj_info_request {
   /** Bind object ID to which get details  
       { enum fslx_bind_object_number } */
   uint32_t  bind_obj_id; 
   /** Pad to 64 bits */
   uint8_t   pad[4];      
};
OFP_ASSERT (sizeof(struct fslx_bind_obj_info_request) == 8);

/** \ingroup FSL_Common_Extensions 
 *  \struct fslx_bind_obj_info_reply
 *  \brief Bind object information multipart reply.\n
 *  <b>Description:</b>\n
 *  Following data structure is used for bind object information multipart reply.
 */
struct fslx_bind_obj_info_reply {
   /** Bind object ID to which returning details */
   uint32_t  bind_obj_id;       
   /** Current numbers of flows attached to bind-object */
   uint32_t  number_of_flows;   
   /** Number of packet processed by the bind-object */
   uint64_t  packet_count;       
   /** Number of bytes  processed by the bind-object */
   uint64_t  byte_count;        
   /** Number of errors occurred */
   uint64_t  error_count;       
   /** Opaque controller-issued identifier. */
   uint64_t  cookie;          
   /** Application type, any one of FSLX_BO_APP_* */ 
   uint8_t   type;             
   /** Pad to 64 bits */
   uint8_t   pad[5];            
   /** Length of application data, it is zero incase no
                                   Application data is available */
   uint16_t  length;            
   /** Variable size application data */
   uint8_t   data[0];           

};
OFP_ASSERT (sizeof(struct fslx_bind_obj_info_reply) == 48);

 
/** \ingroup FSL_Common_Extensions 
 *  \struct fslx_action_attach_bind_obj
 *  \brief This action attaches flow to bind object.\n
 *  <b>Description:</b>\n
 * This action usually will be part of
 * FSLX_APPLY_ACTIONS_ON_CREATE instruction of OFP_FLOW_MOD message. If a flow is attached to any bind-object and it is evicted
 * for any reason it must be detached from the bind object. A flow can be detached from bind-object and continue to exist
 * independently.
 *
 * One flow can be attached to more than one bind-object. If one flow is attached to more than one bind-object and if one
 * of bind-object is deleted, the treatment of flow is based on one of configuration value FSLX_BO_DB-* defined in the 
 * corresponding bind-object. In case the flow is deleted because of FSLX_BO_DB_DEL_BIND_OBJ configuration is set, it must
 * be detached from another bind-object.
 */
struct fslx_action_attach_bind_obj {
    /** OFPAT_EXPERIMENTER. */
    uint16_t type;              
    /** Length is 16. */
    uint16_t len;               
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;            
    /** FSLXAT_ATTACH_BIND_OBJ */
    uint16_t subtype;           
    /** One of FSLX_AF_*/
    uint8_t  flag;              
    /** Pad to 64 bits. */
    uint8_t  pad;               
    /** ID of bind-object to which flow need to add */
    uint32_t bind_obj_id;       
};
OFP_ASSERT(sizeof(struct fslx_action_attach_bind_obj) == 16);
 
/** \ingroup FSL_Common_Extensions 
 *  \enum fslx_action_attach_bindobj_flag
 *  <b>Description:</b>\n
 *  Configuration flags of attach bind object action.
 */
enum fslx_action_attach_bindobj_flag {
   FSLX_AF_MORE_FLOWS  = 1 << 0 /* More flows to follow. */
};
 
/** \ingroup FSL_Common_Extensions 
 *  \struct fslx_action_detach_bind_obj
 *  \brief Action to detach flow from bind-object.\n
 *  <b>Description:</b>\n
 * This is usually an action as part of 
   FSLX_APPLY_ACTIONS_ON_DELETE instruction that will be applied during flow deletion time. It is not mandatory requirement. If flow is deleted, it will be detached from bind-object automatically. 
*/ 
struct fslx_action_detach_bind_obj {
    /** OFPAT_EXPERIMENTER. */
    uint16_t type;               
    /** Length is 16. */
    uint16_t len;                
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;             
    /** FLSXAT_DETACH_BIND_OBJ */
    uint16_t subtype;            
    /** Pad to 64 bits. */
    uint8_t  pad[2];             
    /** ID of bind-object from which flow need to detach */
    uint32_t bind_obj_id;        
};
OFP_ASSERT(sizeof(struct fslx_action_detach_bind_obj) == 16);

/** \ingroup FSL_Common_Extensions 
 *  \struct fslx_action_attach_revalidation_obj
 *  \brief This action attaches flow to revalidation object.\n
 *  <b>Description:</b>\n
 * This action usually part of FSLX_APPLY_ACTIONS_ON_CREATE instruction OFPT_FLOW_MOD.
 * That is flow attachment to revalidation object happens during flow creation time.
 */
struct fslx_action_attach_revalidation_obj{
    /** OFPAT_EXPERIMENTER. */
    uint16_t type;            
    /** Length is 16. */
    uint16_t len;             
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;          
    /** FSLXAT_ATTACH_REVALIDATION_OBJ */
    uint16_t subtype;         
    /** Pad to 64 bits. */
    uint8_t  pad[2];          
    /** ID of revalidtion for which to attach flows */
    uint32_t revaidation_obj_id; 
};
OFP_ASSERT(sizeof(struct fslx_action_attach_revalidation_obj) == 16);

/** \ingroup FSL_Common_Extensions 
 *  \struct fslx_action_detach_revalidation_obj
 *  \brief This action detaches flow from revalidation object.\n
 *  <b>Description:</b>\n
 * This action detaches revalidation object.
 */
struct fslx_action_detach_revalidation_obj{
    /** OFPAT_EXPERIMENTER. */
    uint16_t type;            
    /** Length is 16. */
    uint16_t len;             
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;          
    /** FSLXAT_DETACH_REVALIDATION_OBJ */
    uint16_t subtype;         
    /** Pad to 64 bits. */
    uint8_t  pad[2];          
    /** ID of revalidation object to detach flow */
    uint32_t revaidation_obj_id; 
};
OFP_ASSERT(sizeof(struct fslx_action_detach_revalidation_obj) == 16);

/** \ingroup FSL_Common_Extensions 
 *  \struct fslx_action_set_flow_direction
 *  \brief Set Flow type as FSLXFT_CLNT_TO_SRVR or FSLXFT_SRVR_TO_CLNT.\n
 *  <b>Description:</b>\n
 *  This action usually part of FSLX_APPLY_ACTIONS_ON_CREATE instruction as part of OFPT_FLOW_MOD message. 
 * 
 * Applications usually creates two flows. Either server to client or client to server direction
 * and attaches to bind-object. By using this action controller can set direction of the flow entry created.
 */
struct fslx_action_set_flow_direction {
    /** OFPAT_EXPERIMENTER. */
    uint16_t type;            
    /** Length is 16. */
    uint16_t len;             
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;          
    /** FSLXAT_SET_FLOW_TYPE */
    uint16_t subtype;         
    /** One of FSLXFD_* */
    uint16_t flow_direction;  
    /** Pad to 64 bits. */
    uint8_t  pad[4];          
};
OFP_ASSERT(sizeof(struct fslx_action_set_flow_direction) == 16);

enum fslx_flow_type {
    /** Flow direction is client to server */
    FSLXFD_CLNT_TO_SRVR = 1, 
    /** Flow direction is server to client */
    FSLXFD_SRVR_TO_CLNT = 2  
};

/** \ingroup FSL_Common_Extensions 
 *  \struct fslx_action_send_app_state_info
 *  \brief Action to Send Application Status Values.\n
 *  <b>Description:</b>\n
 * This action triggers data-path to send FSLX_APP_STATUS events
 * for a given application to controller.
 * This event is meant to send application specific data with state to controller.
 * This action is usually part of FSLX_APPLY_ACTIONS_ON_CREATE instruction 
 * in FSLX_BIND_MOD message.
 */ 
struct fslx_action_send_app_state_info {
    /** OFPAT_EXPERIMENTER */
    uint16_t type;             
    /** Length is 16. */
    uint16_t len;              
    /** FSLX_VENDOR_ID */
    uint32_t vendor;           
    /** FSLXAT_SEND_APP_STATE_INFO */
    uint16_t subtype;          
    /** One of FSLX_AS_ET_* value */
    uint8_t  event_type;       
    /** One of FSLX_AS_EM_* value */
    uint8_t  event_method;     
    /** Pad to 64 bits. */
    uint8_t  pad[4];           
    /** Count value at which to send event */
    uint64_t event_count;      
};
OFP_ASSERT(sizeof(struct fslx_action_send_app_state_info) == 24);

/** \ingroup FSL_Common_Extensions 
 *  \enum fslx_app_state_event_type
 *  <b>Description:</b>\n
 * Following are event type values currently defined. 
 * To send more than one type of event controller need to push one action 
 * FSLXAT_SEND_APP_STATE separately.
 */ 
enum fslx_app_state_event_type {

/** Type to indicate send event to controller after processing certain number
 * of bytes of data 
 */
  FSLX_AS_ET_BYTES      = 1,

/** Type to indicate send event to controller after processing certain number of
 * packets. 
 */
  FSLX_AS_ET_PKTS = 2,

/** Type to indicate send event to controller after elapse of certain amount of time. */
  FSLX_AS_ET_TIME = 3
};

/** \ingroup FSL_Common_Extensions 
 *  \enum fslx_app_state_event_method
 *  <b>Description:</b>\n
 * The event_count indicates count value of given type. For example 
 * if event_type is FSLX_AS_ET_BYTES, event_count indicate byte count at which 
 * event need to send it to controller. That is after processing of event_count
 * number of bytes data, the data_path will send FSLX_APP_STATUS event to controller.
 * Following are the event_method values supported.
 */
enum fslx_app_state_event_method {
/** Send event only one time */
FSLX_AS_EM_ONCE   = 0, 
/** Send continuous periodic events */
FSLX_AS_EM_PERIODIC = 1 
};

/** \ingroup FSL_Common_Extensions 
 *  \struct fslx_action_start_tcp_terminate_sm
 *  \brief Action to start TCP Termination state machine.\n
 *  <b>Description:</b>\n
 * This action usually will be part of FSLX_APPLY_ACTIONS_ON_CREATE either in OFP_FLOW_MOD or FSLX_BIND_MOD messages.
 *
 * This action triggers to run TCP termination state machine. It tracks the state of TCP FIN, FIN+ACK, 
 * RST packets to handle TCP termination process.  
 *
 * This action usually will be attached as one of the action list of the bind-object so that termination
 * process for all the flows attached to bind-object are monitored. 
 *
 * If connection is closed for one flow that is completed TCP 4-Way for that flow, then along with removing that flow,
 * it also removes all other other flows attached to the bind-object. 
 *
 * Note that termination will take care of using close_wait_time, if action FSLXAT_SET_CLOSE_WAIT_TIME is also attached
 */
struct fslx_action_start_tcp_terminate_sm {
     /** OFPAT_EXPERIMENTER. */
    uint16_t type;                  
    /** Length is 16. */
    uint16_t len;                
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;            
    /** FSLXAT_START_TCP_TERMINATE_SEQ*/
    uint16_t subtype;            
    /** Pad to 64 bits. */
    uint8_t  pad[6];         
};
OFP_ASSERT(sizeof(struct fslx_action_start_tcp_terminate_sm) == 16);

/** \ingroup FSL_Common_Extensions 
 *  \struct fslx_action_set_close_wait
 *  \brief Action to set TCP close wait time.\n
 *  <b>Description:</b>\n
 * Close wait time (Milli Seconds) is amount of time to wait
 * after receiving TCP FIN, FIN+ACK or RST packets. This is to ensure that all the packets belong to previous session are 
 * received completely, otherwise there is possibility of the previous packets can be considered for the new session. That
 * is because of session timeout before removing flow entry from the table, the close_wait_time will also need to considered.
 * The FSLXAT_SET_CLOSE_WAIT_TIME is an optional action and by default close wait timeout value will be set as zero.
 * This action usually will be part of FSLX_APPLY_ACTIONS_ON_CREATE either in OFP_FLOW_MOD or FSLX_BIND_MOD messages.
 */
struct fslx_action_set_close_wait {
    /** OFPAT_EXPERIMENTER. */
    uint16_t type;               
    /** Length is 16. */
    uint16_t len;                
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;             
    /** FLSXAT_DETACH_BIND_OBJ */
    uint16_t subtype;            
    /** TCP close wait timeout value*/
    uint32_t close_wait_time;    
    /** Pad to 64 bits. */
    uint8_t  pad[2];             
};
OFP_ASSERT(sizeof(struct fslx_action_set_close_wait) == 20);

/** \ingroup FSL_Common_Extensions 
 *  \struct fslx_action_execute_bind_obj_apply_actns
 *  \brief Action to execute bind object apply actions.\n
 *  <b>Description:</b>\n
 * This action is part of OFPIT_APPLY_ACTIONS instruction in OFPT_FLOW_MOD. If this action hits 
 * as part of OPFPIT_APPLY_ACTIONS list of a flow, it first executes all the actions available in
 * OFPIT_APPLY_ACTIONS of bind-object and return back and continue to next action available in
 * OFPIT_APPLY_ACTIONS of the flow.
 */
struct fslx_action_execute_bind_obj_apply_actns {
      /** OFPAT_EXPERIMENTER. */
      uint16_t type;            
      /** Length is 16. */
      uint16_t len;             
      /** FSLX_VENDOR_ID. */
      uint32_t vendor;          
      /** FSLXAT_BIND_OBJECT_APPLY_ACTIONS */
      uint16_t subtype;         
      /** Pad to 64 bits. */
      uint8_t  pad[2];          
      /** ID of bind-object from which the per packet actions are executed */
      uint32_t bind_obj_id;     
};
OFP_ASSERT(sizeof(struct fslx_action_execute_bind_obj_apply_actns) == 16);

/** \ingroup FSL_Common_Extensions 
 *  \struct fslx_action_execute_bind_obj_write_actns
 *  \brief Action to execute bind object actions.\n
 *  <b>Description:</b>\n
 * This action is part of OFPIT_APPLY_ACTIONS instruction in OFPT_FLOW_MOD. 
 * If this action hits as part of OPFPIT_APPLY_ACTIONS
 * list of a flow, it will first execute all the actions available in OFPIT_WRITE_ACTIONS of bind-object
 * and return back and continue to next action available in OFPIT_APPLY_ACTIONS of the flow.
 */
struct fslx_action_execute_bind_obj_write_actns {
      /** OFPAT_EXPERIMENTER. */
      uint16_t type;            
      /** Length is 16. */
      uint16_t len;             
      /** FSLX_VENDOR_ID. */
      uint32_t vendor;          
      /** FSLXAT_BIND_OBJECT_WRITE_ACTIONS */
      uint16_t subtype;         
      /** Pad to 64 bits. */
      uint8_t  pad[2];          
      /** ID of bind-object from which the per packet actions are executed */
      uint32_t bind_obj_id;      
};
OFP_ASSERT(sizeof(struct fslx_action_execute_bind_obj_write_actns) == 16);

/************** Freescale Common Open Flow Extensions - End *****************************/


/************** Freescale L3 Forwarding Open Flow Extensions - Begin ********************/

/** \ingroup FSL_L3_Forwarding_Extensions 
 *  \struct fslx_action_ip_reasm
 *  \brief Re-assembly of fragmented IP packets.\n
 *  <b>Description:</b>\n
 * Do re-assembly of fragmented IP packets. This is table level action applied
 * on per packet basis. 
 *
 * Currently, Open Flow 1.3 have switch level configuration of whether to do IP
 * re-assembly or not by using OFPT_SET_CONFIG message. With this configuration, IP reassembly applies to 
 * all the packet irrespective of whether applications needs IP reassembly or not. But for some applications
 * it may not require full packet for its processing. However some applications like firewall requires
 * full packet. So selection of doing IP reassembly is based on table. 
 *
 * This action will be defined as part of 'per_pkt_actions' list in FSLX_TABLE_MOD message.
 * If this action is defined, switch will do reassembly of IP fragment packets received at table.
 * Before actually doing table lookup it performs the IP reassembly
 */ 
struct fslx_action_ip_reasm {
    /** OFPAT_EXPERIMENTER. */
    uint16_t type;                  
    /** Length is 16. */
    uint16_t len;                   
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;                
    /** FSLXAT_IP_FRAG_REASM. */
    uint16_t subtype;               
    /** Pad to 64 bits. */
    uint8_t  pad[6];                
}; 
OFP_ASSERT(sizeof(struct fslx_action_ip_reasm) == 16);

/** 
 * This action informs switch to perform fragmentation of the packets, if length of
 * the packet is more than MTU. The minimum of MTU value defined in the action and MTU value of interface
 * is taken as MTU value to do fragments.
 */  
struct fslx_action_ip_fragment {
    /** OFPAT_EXPERIMENTER. */
    uint16_t type;                  
    /** Length is 16. */
    uint16_t len;                   
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;                
    /** FSLXAT_FRAG_PKT*/
    uint16_t subtype;               
    /**Offset of the ipfragment  */
    uint8_t offset;
    /** n bits*/
    uint8_t nbits;
    /** MTU size */
    uint32_t mtu_reg;                   
    /** MTU size */
    uint32_t mtu;                   
    /** Pad to 64 bits. */
    uint8_t  pad[4];                
};
OFP_ASSERT(sizeof(struct fslx_action_ip_fragment) == 24);

/** \ingroup FSL_L3_Forwarding_Extensions 
 *  \struct fslx_action_set_in_phy_port_cntxt
 *  \brief Action to set physical port context field.\n
 *  <b>Description:</b>\n
 * As part of packet_in message, controller receives match-field context value 'physical port'.
 * Controller or application based on the controller may likes to send 'physical port' as part of
 * packet_out message to switch. This is done by sending FSLXAT_SET_IN_PHY_PORT_FIELD_CNTXT action 
 * in the packet_out message with 'physical port' that isextracted from packet_in messages as a parameter.  
 * 
 * For this controller extracts physical port value received in Packet_in and copy physical port value
 * as a paramter to action FSLXAT_SET_IN_PHY_PORT_FIELD_CNTXT in Packet_out message.
 */
struct fslx_action_set_in_phy_port_cntxt {
    /** OFPAT_EXPERIMENTER. */
    uint16_t type;                  
    /** Length is 16. */
    uint16_t len;                   
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;                
    /** FSLXAT_SET_IN_PHY_PORT_FIELD_CNTXT */
    uint16_t subtype;               
    /** Pad to 64 bits. */
    uint8_t  pad[2];                
    /** Physical port value extracted from Packet_in message */           
    uint32_t in_phy_port;           
    
};
OFP_ASSERT(sizeof(struct fslx_action_set_in_phy_port_cntxt) == 16);

/* Rev provision set mtu in meta-data and read later stage 
  basically need mtu read from metadata */

/** \ingroup FSL_L3_Forwarding_Extensions 
 *  \struct fslx_action_icmpv4_time_exceed
 *  \brief Action to send ICMPV4 time exceeded error message.\n
 *  <b>Description:</b>\n
 * Action to send ICMPV4 error message to indicate the source about discarded datagram due to time exceeded.
 */
struct fslx_action_icmpv4_time_exceed {
    /** OFPAT_EXPERIMENTER. */
    uint16_t type;                  
    /** Length is 16. */
    uint16_t len;                   
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;                
    /** FSLXAT_SEND_ICMPV4_TIME_EXCEED*/
    uint16_t subtype;               
    /** One of FSLX_ICMPV4_TE_EC error code*/
    uint8_t  code;                  
    /** Pad to 64 bits. */
    uint8_t  pad[5];                
};

OFP_ASSERT(sizeof(struct fslx_action_icmpv4_time_exceed) == 16);
enum fslx_icmpv4_time_exceed_codes {
    /** Error code, indicates IP TTL value reached zero, that is 
       Time-to-live exceeded in transit */
    FSLX_ICMPV4_TE_EC_TTL_VAL_ZERO = 0, 
    /** Error code, indicates Fragment reassembly time exceeded 
        That is it fails to reassemble a fragmented datagram 
        within its time limit.*/
    FSLX_ICMPV4_TE_EC_RE_ASSMBLY_EXCEED_TM_LIMIT = 1
};

/** \ingroup FSL_L3_Forwarding_Extensions 
 *  \struct fslx_action_icmpv4_dest_unreachable
 *  \brief Action to send ICMPV4 destination unreachable error message.\n
 *  <b>Description:</b>\n
 * Action to send ICMPV4 error message to inform the client that the destination is unreachable for some reason.
 */
struct fslx_action_icmpv4_dest_unreachable {
    /** OFPAT_EXPERIMENTER. */
    uint16_t type;                  
    /** Length is 16. */
    uint16_t len;                   
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;                
    /** FSLXAT_SEND_ICMPV4_DEST_UNREACHABLE */
    uint16_t subtype;               
    /** One of FSLX_ICMPV4_DR_EC error code */
    uint8_t  code;                  
    /** Next hop MTU value applicable for code, applicable for the error code
        FSLX_ICMPV4_DR_EC_DFBIT_SET*/ 
    uint16_t next_hop_mtu;          
    /** Pad to 64 bits. */
    uint8_t  pad[3];                
};
OFP_ASSERT(sizeof(struct fslx_action_icmpv4_dest_unreachable) == 20);

enum fslx_icmpv4_dest_unreachable_codes {
    /** Error code, indicates destination network is unreachable*/ 
    FSLX_ICMPV4_DR_EC_NW_UNREACHABLE = 0,  
    /** Error code, indicates destination host is unreachable*/ 
    FSLX_ICMPV4_DR_EC_HOST_UNREACHABLE = 1,
    /** Error code, indicates destination port is unreachable*/ 
    FSLX_ICMPV4_DR_EC_PORT_UNREACHABLE = 3,
    /** Error code, indicates datagram is too big. Packet 
        fragmentation is required but 'don't fragment'
        (DF) flag is on */
    FSLX_ICMPV4_DR_EC_DFBIT_SET        = 4,
    /** Error code, indicates destination network is 
                                              administratively prohibited */
    FSLX_ICMPV4_DR_EC_NW_FILTER_SET = 9,
    /** Error code, indicates the destination host is 
                                             administratively prohibited */
    FSLX_ICMPV4_DR_EC_HOST_FILTER_SET = 10 
};


/** \Experimenter Instruction
 * FSLX_ACTION_WRITE_METADATA_FROM_PKT_LEN
 *   - Returns length of buffer required for 'Set Meta Data from match field instruction'
 */
#define FSLX_ACTION_WRITE_METADATA_FROM_PKT_LEN  sizeof(struct fslx_action_wr_metadata_from_pkt)
#define FSLX_ACTION_WRITE_METADATA_LEN  sizeof(struct fslx_action_write_metadata)

/** \Experimenter Action 
 *  FSLX_ACTION_IP_REASM_LEN
 *   - Returns length of buffer required for 'ip reassembly action'
 */
#define FSLX_ACTION_IP_REASM_LEN                sizeof(struct fslx_action_ip_reasm)

/** \Experimenter Action 
 *  FSLX_ACTION_IP_FRAG_LEN
 *   - Returns length of buffer required for 'ip fragmentation action'
 */
#define FSLX_BIND_MOD_MESSAGE_LEN               sizeof(struct fslx_bind_mod)

#define FSLX_ACTION_IP_FRAGMENT_LEN             sizeof(struct fslx_action_ip_fragment)

#define FSLX_ACTION_START_TABLE_LEN             sizeof(struct fslx_action_start_table)

#define FSLX_ACTION_SET_IN_PHY_PORT_LEN         sizeof(struct fslx_action_set_in_phy_port_cntxt)

#define FSLX_ACTION_SET_METADATA_LEN            sizeof(struct fslx_action_meta_data_cntxt)

#define FSLX_ACTION_SET_TUNNEL_ID_LEN           sizeof(struct fslx_action_tunnel_id_cntxt)

#define FSLX_ACTION_DROP_PACKET_LEN             sizeof(struct fslx_action_drop_packet)

#define FSLX_ACTION_ARP_RESPONSE_LEN            sizeof(struct fslx_action_arp_response)

#define FSLX_ACTION_ICMPV4_TIME_EXCEED_LEN      sizeof(struct fslx_action_icmpv4_time_exceed)

#define FSLX_ACTION_ICMPV4_DEST_UNREACH_LEN     sizeof(struct fslx_action_icmpv4_dest_unreachable)

#define FSLX_ACTION_ATTACH_BIND_OBJ_ID_LEN      sizeof(struct fslx_action_attach_bind_obj)

#define FSLX_ACTION_DETACH_BIND_OBJ_ID_LEN      sizeof(struct fslx_action_detach_bind_obj)

#define FSLX_ACTION_SET_APP_STATE_INFO_LEN      sizeof(struct fslx_action_set_app_state_info)

#define FSLX_ACTION_SEND_APP_STATE_INFO_LEN     sizeof(struct fslx_action_send_app_state_info)

#define FSLX_ACTION_IPSEC_ENACAP_LEN            sizeof(struct fslx_ipsec_encap)

#define FSLX_ACTION_IPSEC_DECAP_LEN             sizeof(struct fslx_ipsec_decap)

#define FSLX_ACTION_IPSEC_SA_DELETE_LEN         sizeof(struct fslx_delete_ipsec_sa)

#define FSLX_ACTION_IPSEC_SA_CREATE_LEN         sizeof(struct fslx_create_ipsec_sa)

#define FSLX_ACTION_EXECUTE_BIND_OBJ_APPLY_ACTNS_LEN sizeof(struct fslx_action_execute_bind_obj_apply_actns)

#define FSLX_ACTION_POP_NATT_UDP_HEADER_LEN  sizeof(struct fslx_action_header)

#define FSLX_APP_STATE_INFO_LEN              sizeof(struct fslx_app_state_info)

#define FSLX_MULTIPART_REQ_LEN          sizeof(struct fslx_multipart_request)

#define FSLX_MULTIPART_RESP_LEN         sizeof(struct fslx_multipart_reply)

#define FSLX_MP_BIND_OBJ_REQ_LEN         sizeof(struct fslx_bind_obj_info_request)

#define FSLX_MP_BIND_OBJ_REPLY_LEN         sizeof(struct fslx_bind_obj_info_reply)

/****************** Freescale L3 Forwarding Open Flow Extensions - End  *************************/


/****************** Freescale IPsec Open Flow Extensions - Begin *******************************/


/** \ingroup FSL_IPsec_Extensions 
 *  \struct fslx_sa_cntxt_prop_header
 *  \brief IPSec Security Association context header.\n
 *  <b>Description:</b>\n
 * IPSec Security Association context header.
 */
struct fslx_sa_cntxt_prop_header {
    /** One of FSLX_IPSEC_SA_*. */
    uint16_t         type;    
    /** Length in bytes of this property. */
    uint16_t         length;  
    /** Pad to 64 bits. */
    uint8_t          pad[4];  
};
OFP_ASSERT(sizeof(struct fslx_sa_cntxt_prop_header) == 8);

/** \ingroup FSL_IPsec_Extensions 
 *  \struct fslx_create_ipsec_sa
 *  \brief Action to create IPSec Security Association context.\n
 *  <b>Description:</b>\n
 * This action usually will be part of FSLX_APPLY_ACTIONS_ON_CREATE in FSLX_BIND_MOD messages.
 * 
 * The storage location of SA context handle that is generated by running FSLX_CREATE_SA_CNTXT
 * action is upto the application. But it is recommending to maintain the handle as part of 
 * application state blob. The application state varilables were created by using the action
 * FSLXAT_SET_STATE_BLOB.
 * 
 * Also it is recommending to create one bind-object for each IPSec SA context and run this action as 
 * part of FSLX_APPLY_ACTIONS_ON_CREATE in FSLX_BIND_MOD message so that IPSec SA context is created 
 * during bind-object creation and is used commonly across all the flows attached to the bind-object.
 */
struct fslx_create_ipsec_sa {
    /** OFPAT_EXPERIMENTER. */
    uint16_t type;                                  
    /** Length of this action */
    uint16_t len;                                   
    /** FSLX_VENDOR_ID */
    uint32_t vendor;                                
    /** FSLX_CREATE_SA_CNTXT */
    uint16_t subtype;                          
    /** It is either 
        FSLX_IPSEC_SA_FLAGS_NONE value (or) 
        One or more values FLSX_IPSEC_SA_ flags set */
    uint16_t flags;                                 
    /** Security Parameter Index */ 
    uint32_t spi;                                   
    /** One of FSLX_SA_SECURITY_PROTOCOL_* */
    uint8_t  protocol;                              
    /** One of FSLX_SA_DF_BIT_* */
    uint8_t  handle_df_bit;                         
    /** DSCP value copy in outer IP header */
    uint8_t  copy_dscp_value;                       
    /** IPSec encapsulation mode either 
        FSLX_IPSEC_TUNNEL_MODE  or
        FSLX_IPSEC_TRANSPORT_MODE */
    uint8_t  encapsulation_mode;                    
    /** Size of window used in the replay check */
    uint32_t replay_window_size;                    
   /** Variable size property values */
   struct fslx_sa_cntxt_prop_header properties[0];  
};
OFP_ASSERT(sizeof(struct fslx_create_ipsec_sa) == 24);

/** \ingroup FSL_IPsec_Extensions 
 *  \enum fslx_sa_cntxt_flags
 *  <b>Description:</b>\n
 * Flags to enable some configuration values for SA context.
 */
enum  fslx_sa_cntxt_flags {
    /** None of the configuration values defined here is enabled */
    FSLX_IPSEC_SA_FLAGS_NONE        = 0,      
    /** Set to enable Redside fragementation configuration */
    FSLX_IPSEC_SA_ENABLE_REDSIDE_FRAG      = 1 << 0, 
    /** Set to enable Peer gateway IP address adaptaion */
    FSLX_IPSEC_SA_ENABLE_PEER_GW_ADAPTION  = 1 << 1, 
    /** Set to enable Extended Sequence Number */
    FSLX_IPSEC_SA_ENABLE_ESN               = 1 << 2, 
    /** Set to enable Propogate ECN bits */
    FSLX_IPSEC_SA_ENABLE_PROPOGATE_ECN     = 1 << 3, 
    /** Set to enable Copy DSCP bits */
    FSLX_IPSEC_SA_ENABLE_COPY_DSCP_BITS    = 1 << 4, 
    /** Set, if SA direction is Inbound. Else Outbound SA */
    FSLX_IPSEC_SA_DIRECTION_INBOUND        = 1 << 5  
};

/** \ingroup FSL_IPsec_Extensions 
 *  \enum fslx_ipsec_sa_ipv4_handle_df_bit
 *  <b>Description:</b>\n
 * DF Bit handling in Outer IP Header.
 */
enum fslx_ipsec_sa_ipv4_handle_df_bit {
   /** sET df bit copy*/  
   FSLX_SA_DF_BIT_COPY  = 0, 
   /**  Set DF Bit in Outer IPv4 Header */
   FSLX_SA_DF_BIT_SET   = 1, 
   /**  Clear DF Bit in Outer IPv4 Header */
   FSLX_SA_DF_BIT_CLEAR = 2  
};

/** \ingroup FSL_IPsec_Extensions 
 *  \enum fslx_ipsec_sa_security_protocol
 *  <b>Description:</b>\n
 * Security Protocol configuration.
 */
enum fslx_ipsec_sa_security_protocol {
   /** Protocol is Encapsulating Security Payloads (ESP) */
   FSLX_SA_SECURITY_PROTOCOL_ESP = 50, 
   /** Protocol is Authentication Headers (AH) */
   FSLX_SA_SECURITY_PROTOCOL_AH  = 51  
};

/** \ingroup FSL_IPsec_Extensions 
 *  \enum fslx_ipsec_sa_encapsulation_mode
 *  <b>Description:</b>\n
 * Encapsulation Mode configuration.
 */
enum fslx_ipsec_sa_encapsulation_mode {
   /** Encapsulation mode is tunnel mode */
   FSLX_SA_TUNNEL_MODE    = 0, 
   /** Encapsulation mode is transport mode */
   FSLX_SA_TRANSPORT_MODE = 1
};

/** \ingroup FSL_IPsec_Extensions 
 *  \enum fslx_ipsec_sa_properity_type
 *  <b>Description:</b>\n
 * IPSec SA Context creation Property types, all these property values are optional.
 */
enum fslx_ipsec_sa_properity_type {
   /** struct fslx_ipsec_sa_auth_algo_info */
   FSLX_IPSEC_SA_AUTH_ALGO                = 0,  
   /** struct fslx_ipsec_sa_enc_algo_info */ 
   FSLX_IPSEC_SA_ENCRYPT_ALGO             = 1,  
   /** struct fslx_ipsec_sa_enc_auth_algo_info */
   FSLX_IPSEC_SA_CMB_MODE_ALGO            = 2,  
   /** struct fslx_ipsec_sa_aesctr_block_or_salt _info */
   FSLX_IPSEC_SA_AESCTR_OR_CMBN_MODE_SALT = 3,  
   /** struct fslx_ipsec_sa_enc_algo_iv_info */
   FSLX_IPSEC_SA_ENCRYPT_ALGO_IV          = 4,  
   /** struct fslx_ipsec_sa_ipv4_udp_encap_info */
   FSLX_IPSEC_SA_IPV4_UDP_ENCAP           = 5,  
   /** struct fslx_ipsec_sa_ipv6_udp_encap_info */
   FSLX_IPSEC_SA_IPV6_UDP_ENCAP           = 6,  
   /** struct fslx_ipsec_sa_ipv4_tunnel_end_addrs_info */
   FSLX_IPSEC_SA_IPV4_TUNNEL_END_ADDRS    = 7,  
   /** struct fslx_ipsec_sa_ipv6_tunnel_end_addrs_info */
   FSLX_IPSEC_SA_IPV6_TUNNEL_END_ADDRS    = 8,  
   /** struct fslx_ipsec_sa_ipv4_selectors_info */
   FSLX_IPSEC_SA_IPV4_SELECTOR            = 9,  
   /** struct fslx_ipsec_sa_ipv6_selectors_info */
   FSLX_IPSEC_SA_IPV6_SELECTOR            = 10, 
   /** struct fslx_ipsec_sa_dscp_range */
   FSLX_IPSEC_SA_DSCP_RANGE               = 11, 
   /** struct fslx_ipsec_sa_ipv4_udp_encap_info */
   FSLX_IPSEC_SA_IPV4_UDP_ENCAP_INFO      = 12, 
   /** struct fslx_ipsec_sa_ipv6_udp_encap_info */
   FSLX_IPSEC_SA_IPV6_UDP_ENCAP_INFO      = 13  
};

/** \ingroup FSL_IPsec_Extensions 
 *  \struct fslx_ipsec_sa_auth_algo_info
 *  \brief Security Association Authentication Algorithm Information.\n
 *  <b>Description:</b>\n
 * Security Association Authentication Algorithm property, used to send authentication algorithm key information.
 */
struct fslx_ipsec_sa_auth_algo_info {
   /** FSLX_IPSEC_SA_AUTH_ALGO*/
   uint16_t type;               
   /** Length of this property*/
   uint16_t length;             
   /** One of FSLX_AUTH_ALG_* */
   uint16_t auth_algo;          
   /** ICV size in bits */
   uint16_t icv_size_in_bits;   
   /** Authentication algorithm key length in bits */
   uint16_t authkey_len_in_bits;
   /** Align to 64 bits */
   uint8_t  pad[6];             
  /** Authentication algorithm key value */
   uint8_t  authkey_val[0];     
};
OFP_ASSERT(sizeof(struct fslx_ipsec_sa_auth_algo_info) == 16);

/** \ingroup FSL_IPsec_Extensions 
 *  \enum fslx_authuentication_algo
 *  <b>Description:</b>\n
 *  Authentication algorithms.
 */
enum fslx_authuentication_algo {
   /** MD5 algorithm */
   FSLX_AUTH_ALG_HMAC_MD5        = 0, 
   /** SHA_1 algorithm */
   FSLX_AUTH_ALG_HMAC_SHA1       = 1, 
   /** SHA_2 256 algorithm */
   FSLX_AUTH_ALG_HMAC_SHA2_256   = 2, 
   /** SHA_2 384 algorithm */
   FSLX_AUTH_ALG_HMAC_SHA2_384   = 3,
   /** SHA_2 512 algorithm */
   FSLX_AUTH_ALG_HMAC_SHA2_512   = 4,
   /** RIPEMD_160 algorithm */
   FSLX_AUTH_ALG_HMAC_RIPEMD_160 = 5, 
   /** AES_XCBC_MAC algorithm */
   FSLX_AUTH_ALG_AES_XCBC_MAC    = 6 
};

/** \ingroup FSL_IPsec_Extensions 
 *  \struct fslx_ipsec_sa_enc_algo_info
 *  \brief Encryption algorithm key information.\n
 *  <b>Description:</b>\n
 *  Security encryption algorithm property, used to send encryption algorithm key information.
 */
struct fslx_ipsec_sa_enc_algo_info {
   /** FSLX_IPSEC_SA_ENCRYPT_ALGO */
   uint16_t  type;        
   /** Length of this property */
   uint16_t  length;      
   /** One of FSLX_ ENCR_ALG_* */
   uint16_t  enc_algo;    
   /** encryption algorithm key length in bits */
   uint16_t  enc_key_len_in_bits; 
   /** Encryption algorithm key value */
   uint8_t   enc_key[0];  
};
OFP_ASSERT(sizeof(struct fslx_ipsec_sa_enc_algo_info) == 8);

/** \ingroup FSL_IPsec_Extensions 
 *  \enum fslx_encryption_algo
 *  <b>Description:</b>\n
 *  Encryption algorithms.
 */
enum fslx_encryption_algo {
  /** CBC_DES algorithm */
  FSLX_ENCR_ALG_CBC_DES      = 0, 
  /** CBC_3DES algorithm */
  FSLX_ENCR_ALG_CBC_3DES     = 1, 
  /** CBC_CAST algorithm */
  FSLX_ENCR_ALG_CBC_CAST     = 2,     
  /** CBC_BLOWFISH algorithm */
  FSLX_ENCR_ALG_CBC_BLOWFISH = 3,   
  /** NULL */
  FSLX_ENCR_ALG_NULL         = 4, 
  /** AES_CBC algorithm */
  FSLX_ENCR_ALG_AES_CBC      = 5, 
  /** AES_CTS algorithm */
  FSLX_ENCR_ALG_AES_CTR      = 6, 
  /** AES_CAMELLIACBC algorithm */
  FSLX_ENCR_ALG_CAMELLIACBC  = 7  
};

/** \ingroup FSL_IPsec_Extensions 
 *  \struct fslx_ipsec_sa_enc_auth_algo_info
 *  \brief Combine mode algorithm key information.\n
 *  <b>Description:</b>\n
 *  Security combine mode algorithm property, used for sending combine mode algorithm key information.
 */
struct fslx_ipsec_sa_enc_auth_algo_info {
  /** FSLX_IPSEC_SA_CMB_MODE_ALGO */
  uint16_t type;             
  /** Length of this property */
  uint16_t length;           
  /** Any one of FSLX_CM_ALG_* */
  uint16_t enc_auth_algo;    
  /** ICV size in bits */
  uint16_t icv_size_in_bits; 
  /** combine mode algorithm key length in bits */
  uint16_t enc_auth_key_len_in_bits; 
  /** Align to 64 bits */
  uint8_t  pad[6];           
  /** combine mode algorithm key value */
  uint8_t  enc_auth_key[0];  
};
OFP_ASSERT(sizeof(struct fslx_ipsec_sa_enc_auth_algo_info) == 16);

/** \ingroup FSL_IPsec_Extensions 
 *  \enum fslx_combine_mode_algo
 *  <b>Description:</b>\n
 *  Combine Mode algorithms.
 */
enum fslx_combine_mode_algo {
  /** AES_CCM algorithm */
  FSLX_CM_ALG_AES_CCM  = 0,
  /** AES_GCM algorithm */
  FSLX_CM_ALG_AES_GCM  = 1,
  /** AES_GMAC algorithm */
  FSLX_CM_ALG_AES_GMAC = 2
};

/** \ingroup FSL_IPsec_Extensions 
 *  \struct fslx_ipsec_sa_aesctr_block_or_iv_info
 *  \brief Property is used for sending salt information.\n
 *  <b>Description:</b>\n
 *  Property is used for sending salt information.
 */
struct fslx_ipsec_sa_aesctr_block_or_iv_info {
  /** FSLX_IPSEC_SA_AESCTR_OR_CMBN_MODE_SALT */
  uint16_t type;             
  /** Length of this property */
  uint16_t length;           
  /** aes-ctr nonce block/combine mode algorithm salt/IV length in bits */
  uint16_t aesctr_block_or_iv_len_bits;  
  /** Align to 64 bits */
  uint8_t  pad[2];           
  /** aes-ctr nonce block or combine mode algorithm salt/IV value */
  uint8_t aesctr_block_or_iv[0]; 
};
OFP_ASSERT(sizeof(struct fslx_ipsec_sa_aesctr_block_or_iv_info) == 8);

/** \ingroup FSL_IPsec_Extensions 
 *  \struct fslx_ipsec_sa_ipv4_udp_encap_info
 *  \brief IPv4 UDP NAT traversal information.\n
 *  <b>Description:</b>\n
 *  Property is used for sending IPv4 UDP NAT traversal information.
 */
struct fslx_ipsec_sa_ipv4_udp_encap_info {
    /** FSLX_IPSEC_SA_IPV4_UDP_ENCAP */
    uint16_t type;       
    /** Length of this properity */
    uint16_t length;     
    /** FSLX_UDP_ENCAP_TYPE_* */
    uint8_t  nat_type;   
    /** Align to 64 bits */
    uint8_t  pad[3];     
    /** src port */   
    uint16_t src_port;
    /** dst_port */
    uint16_t dst_port;
    /** UDP encapsulation original address */
    uint32_t ip4_oa_addr;
};
OFP_ASSERT(sizeof(struct fslx_ipsec_sa_ipv4_udp_encap_info) == 16);

enum fslx_ipsec_sa_udp_encap_type {
    /** FSLX_UDP_ENCAP_TYPE_ESP_IN_UDP */
    FSLX_UDP_ENCAP_TYPE_ESP_IN_UDP = 1,
    /** FSLX_UDP_ENCAP_TYPE_ESP_IN_UDP_NON_IKE */
    FSLX_UDP_ENCAP_TYPE_ESP_IN_UDP_NON_IKE = 2
};

/** \ingroup FSL_IPsec_Extensions 
 *  \struct fslx_ipsec_sa_ipv6_udp_encap_info
 *  \brief IPv6 UDP NAT traversal information.\n
 *  <b>Description:</b>\n
 *  Property is used for sending IPv6 UDP NAT traversal information.
 */
struct fslx_ipsec_sa_ipv6_udp_encap_info {
    /** FSLX_IPSEC_SA_IPV6_UDP_ENCAP */
    uint16_t type;       
    /** Length of this property */
    uint16_t length;     
    /** FSLX_UDP_ENCAP_TYPE_* */
    uint8_t  nat_type;   
    /** Align to 64 bits */
    uint8_t  pad[7];     
    /** Source port of tunnel */  
    uint16_t src_port;   
    /** Destination port of tunnel */
    uint16_t dst_port;   
    /** udp encapsulation original address */
    uint8_t  ip6_oa_addr[16]; 
};
OFP_ASSERT(sizeof(struct fslx_ipsec_sa_ipv6_udp_encap_info) == 32);

/** \ingroup FSL_IPsec_Extensions 
 *  \struct fslx_ipsec_sa_ipv4_tunnel_end_addrs_info
 *  \brief IPv4 Tunnel End point address information.\n
 *  <b>Description:</b>\n
 *  Property is used for sending IPv4 Tunnel End point address information.
 */
struct  fslx_ipsec_sa_ipv4_tunnel_end_addrs_info {
    /** FSLX_IPSEC_SA_IPV4_TUNNEL_END_ADDRS */
    uint16_t type;          
    /** Length of this property */
    uint16_t length;        
    /** Source IPV4 Address of tunnel end point */
    uint32_t src_addr; 
    /** Destination IPV4 address of tunnel end point */
    uint32_t dest_addr; 
    /** Align to 64 bits */
    uint8_t  pad[4];        
};
OFP_ASSERT(sizeof(struct fslx_ipsec_sa_ipv4_tunnel_end_addrs_info) == 16);

/** \ingroup FSL_IPsec_Extensions 
 *  \struct fslx_ipsec_sa_ipv6_tunnel_end_addrs_info
 *  \brief IPv6 Tunnel End point address information.\n
 *  <b>Description:</b>\n
 *  Property used for sending IPv6 Tunnel End point address information.
 */
struct  fslx_ipsec_sa_ipv6_tunnel_end_addrs_info {
    /** FSLX_IPSEC_SA_IPV6_TUNNEL_END_ADDRS */
    uint16_t type;              
    /** Length of this property */
    uint16_t length;            
    /** Align to 64 bits */
    uint8_t  pad[4];         
    /** Source IPV6 Address of tunnel end point */
    uint8_t  src_addr[16]; 
    /** Destination IPV6 address of tunnel end point */
    uint8_t  dest_addr[16]; 
};
OFP_ASSERT(sizeof(struct fslx_ipsec_sa_ipv6_tunnel_end_addrs_info) == 40);

/** \ingroup FSL_IPsec_Extensions 
 *  \struct fslx_ipsec_sa_ipv4_selectors_info
 *  \brief IPv4 SA selector information.\n
 *  <b>Description:</b>\n
 *  Property is used for sending IPv4 SA selector information.
 */
struct  fslx_ipsec_sa_ipv4_selectors_info {
    /** FSLX_IPSEC_SA_IPV4_SELECTOR */
    uint16_t type;              
    /** Length of this property */
    uint16_t length;            
    /** source start ipaddress */
    uint32_t src_start_addr;
    /** source mask or end  ipaddress */
    uint32_t src_mask_or_end_addr;
    /** destination start ip address */
    uint32_t dst_start_addr;
    /** destination mask or end ipaddress */
    uint32_t dst_mask_or_end_addr;
    /** source port start */
    uint16_t src_start_port;
    /** src end port*/
    uint16_t src_end_port;
   /** destination port start*/
    uint16_t dst_start_port;
   /** destination end port*/
    uint16_t dst_end_port;
   /** protocol used*/
    uint8_t  protocol;
   /** Align to 64 bits */
    uint8_t  pad[3];        
};
OFP_ASSERT(sizeof(struct fslx_ipsec_sa_ipv4_selectors_info) == 32);

/** \ingroup FSL_IPsec_Extensions 
 *  \struct fslx_ipsec_sa_ipv6_selectors_info
 *  \brief IPv6 SA selector information.\n
 *  <b>Description:</b>\n
 *  Property is used for sending IPv6 SA selector information.
 */
struct  fslx_ipsec_sa_ipv6_selectors_info {
    /** FSLX_IPSEC_SA_IPV6_SELECTOR */
    uint16_t type;              
    /** Length of this property */
    uint16_t length;            
    /** source start ipaddress */
    uint8_t src_start_addr[16];
    /** source mask or end  ipaddress */ 
    uint8_t src_mask_or_end_addr[16];
    /** destination start ip address */
    uint8_t dst_start_addr[16];
    /** destination mask or end ipaddress */
    uint8_t dst_mask_or_end_addr[16];
    /** source port start */
    uint16_t src_start_port;
    /** src end port*/
    uint16_t src_end_port;
    /** destination port start*/
    uint16_t dst_start_port;
    /** destination end port*/
    uint16_t dst_end_port;
   /** protocol used */
    uint8_t protocol;
};
OFP_ASSERT(sizeof(struct fslx_ipsec_sa_ipv6_selectors_info) == 78);

/** \ingroup FSL_IPsec_Extensions 
 *  \struct fslx_ipsec_sa_dscp_range
 *  \brief DSCP range information.\n
 *  <b>Description:</b>\n
 *  Property is used for sending DSCP range information.
 */
struct fslx_ipsec_sa_dscp_range {
    /** FSLX_IPSEC_SA_DSCP_RANGE */
    uint16_t  type;              
    /** Length of this property */
    uint16_t  length;            
    /** Start of DSCP Range */
    uint8_t   start;             
    /** End of DSCP Range */
    uint8_t   end;               
    /** Align to 64 bits */
    uint8_t  pad[2];             
};
OFP_ASSERT(sizeof(struct fslx_ipsec_sa_dscp_range) == 8);

/** \ingroup FSL_IPsec_Extensions 
 *  \struct fslx_delete_ipsec_sa
 *  \brief Action to delete IPSec Security Association context.\n
 *  <b>Description:</b>\n
 * This action usually will be part of FSLX_APPLY_ACTIONS_ON_DELETE in FSLX_BIND_MOD messages.
 *
 * Assuming FSLX_CREATE_SA_CONTEXT was pushed as bind-object and in the same bind-object 
 * this action was executed and uses context handle stored as part of state variable blob. 
 */
struct fslx_delete_ipsec_sa {
   /** OFPAT_EXPERIMENTER. */
   uint16_t type;              
   /** Length of this action */
   uint16_t len;               
   /** FSLX_VENDOR_ID */
   uint32_t vendor;            
   /** FSLX_DELETE_SA_CONTEXT */
   uint16_t subtype;           
   /** Align to 64 bits */
   uint8_t  pad[6];            
};
OFP_ASSERT(sizeof(struct fslx_delete_ipsec_sa) == 16);

/** \ingroup FSL_IPsec_Extensions 
 *  \struct fslx_ipsec_encap
 *  \brief Action to do IPSec Encapsulation.\n
 *  <b>Description:</b>\n
 * This action usually will be part of OFPIT_APPLY_ACTIONS in FSLX_BIND_MOD messages.
 * 
 * The security version and hardware or software support depends on switch capabilities
 * and is opaque to this action. 
 */
struct fslx_ipsec_encap { 
   /** OFPAT_EXPERIMENTER. */
   uint16_t type;              
   /** Length of this action */
   uint16_t len;               
   /** FSLX_VENDOR_ID */
   uint32_t vendor;            
   /** FSLXAT_IPSEC_ENCAP */
   uint16_t subtype;           
   /** Align to 64 bits */
   uint8_t  pad[6];            
};
OFP_ASSERT(sizeof(struct fslx_ipsec_encap) == 16);

/** \ingroup FSL_IPsec_Extensions 
 *  \struct fslx_ipsec_decap
 *  \brief Action to do IPSec decapsulation.\n
 *  <b>Description:</b>\n
 * This action usually will be part of OFPIT_APPLY_ACTIONS in FSLX_BIND_MOD messages.
 * 
 * The security version and hardware or software support depends on switch capabilities
 * and is opaque to this action. 
 */
struct fslx_ipsec_decap { 
   /** OFPAT_EXPERIMENTER. */
   uint16_t type;              
   /** Length of this action */
   uint16_t len;               
   /** FSLX_VENDOR_ID */
   uint32_t vendor;            
   /** FSLXAT_IPSEC_DECAP */
   uint16_t subtype;           
  /** Align to 64 bits */
   uint8_t  pad[6];            
};
OFP_ASSERT(sizeof(struct fslx_ipsec_decap) == 16);
 
#define IPSEC_SA_AUTH_ALGO_INFO_LEN      sizeof(struct fslx_ipsec_sa_auth_algo_info)
#define IPSEC_SA_ENC_ALGO_INFO_LEN       sizeof(struct fslx_ipsec_sa_enc_algo_info)
#define IPSEC_SA_ENC_AUTH_ALGO_INFO_LEN  sizeof(struct fslx_ipsec_sa_enc_auth_algo_info)
#define IPSEC_SA_BLOCK_OR_IV_INFO_LEN    sizeof(struct fslx_ipsec_sa_aesctr_block_or_iv_info)
#define IPSEC_SA_IPV4_UDP_ENCAP_INFO_LEN sizeof(struct fslx_ipsec_sa_ipv4_udp_encap_info)
#define IPSEC_SA_IPV6_UDP_ENCAP_INFO_LEN sizeof(struct fslx_ipsec_sa_ipv6_udp_encap_info)
#define IPSEC_SA_IPV4_SELECTOR_INFO_LEN  sizeof(struct fslx_ipsec_sa_ipv4_selectors_info)
#define IPSEC_SA_IPV6_SELECTOR_INFO_LEN  sizeof(struct fslx_ipsec_sa_ipv6_selectors_info)
#define IPSEC_SA_IPV4_TUNNEL_ENDS_INFO_LEN sizeof(struct fslx_ipsec_sa_ipv4_tunnel_end_addrs_info)
#define IPSEC_SA_IPV6_TUNNEL_ENDS_INFO_LEN sizeof(struct fslx_ipsec_sa_ipv6_tunnel_end_addrs_info)
#define IPSEC_SA_DSCP_RANGE_INFO_LEN       sizeof(struct fslx_ipsec_sa_dscp_range)

/* IPSec SA STATE INFORMATION SEND TO DATAPATH
 * FSLXAT_SET_APP_STATE_INFO - Action to set state variable This action in 
 * general will be part of FSLX_APPLY_ACTIONS_ON_CREATE either in OFP_FLOW_MOD
 * or FSLX_BIND_MOD messages, but currently it is used only as part of 
 * FSLX_BIND_MOD. By using this action application can attach any application 
 * specific private data to flow or bind object. 
 *
 * This is purely application specific action and there by corresponding 
 * application specific action will be executed by using application type field.
 * As part  of this action, it initialize and attach application specific 
 * private data variables. 
 *
 * Application specific actions that executed might use or update these variable
 * is outside the scope of Open Flow. These state variables are opaque to Open 
 * Flow and used purely within application running as part of data-path. 
 *
 * This data will be send to controller as part of FSLX_REMOVE_BIND_OBJ and 
 * FSLX_APP_STATE messages.
 */ 

enum ipsec_sa_state_resp_type {
 /** IPSEC SA soft time out event*/
 IPSEC_SA_SOFT_LIFE_TIME_OUT_EVENT = 1,  
 /** IPSEC SA hard time out event*/
 IPSEC_SA_HARD_LIFE_TIME_OUT_EVENT = 2   
};

struct ipsec_sa_state_info_req {
  /** ipsec_sa_state_resp_type */
  uint8_t  resp_type; 
  /** Pad to 64 bits */
  uint8_t  pad[7];
  /** sequence number */
  uint32_t seq_num;
  /** high sequence number */
  uint32_t hi_seq_num;
  /** time elapsed in seconds */
  uint64_t elapsed_time_sec;
  /** bytes processed */
  uint64_t bytes_processed;
  /** packets processed */
  uint64_t pkts_processed;
};
OFP_ASSERT(sizeof(struct ipsec_sa_state_info_req) == 40);

#define IPSEC_SA_STATE_INFO_REQ_LEN sizeof(struct ipsec_sa_state_info_req)

/***************** Freescale IPsec Open Flow Extensions - End ****************************/


/***************** Freescale Firewall Open Flow Extensions - Begin ***********************/

/** \ingroup FSL_Firewall_Extensions 
 *  \struct fslx_action_seq_no_addition
 *  \brief Addition of sequence number.\n
 *  <b>Description:</b>\n
 * As part of intermediate devices, some applications as part of detecting some attacks or any other 
 * purpose uses different TCP sequence numbers as part of the data. And it is required to find the
 * the difference (delta) and add the same to real data transfer between client and server.
 *
 * Also there is a posiblity of using more one sequence number and it is required to
 * maintain multiple (delta values).
 *
 * Note this delta sequence number can be negative or positive value. 
 * Rev Add More details with examples
 */
struct fslx_action_seq_no_addition {
    /** OFPAT_EXPERIMENTER. */
    uint16_t type;                  
    /** Length of this action */
    uint16_t len;                   
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;                
    /** FSLXAT_SEQ_NO_DELTA_ADDITION */
    uint16_t subtype;               
    /** Pad to 64 bits. */
    uint8_t  pad[6];                
    /** Array of delta values, multiple delta value, 
        each value can be negative or positive */
    int16_t  delta[0];              
};
OFP_ASSERT(sizeof(struct fslx_action_seq_no_addition) == 16);

/** \ingroup FSL_Firewall_Extensions 
 *  \struct fslx_action_seq_no_validation
 *  \brief Action to validate sequence number.\n
 *  <b>Description:</b>\n
 * Action to validate sequence number of both client_to_server
 * and server_to_client traffics. For some applications like BGP, it is very easy for attacker
 * to predict most of 5-tuple values of given traffic. One way to detect such traffic is to identify 
 * whether they use high sequence number or not.
 *
 * This is one of the action of FSLX_APPLY_ACTIONS_ON_CREATE at bind-object. This is used to
 * start some state machine which check given traffic's sequence number is falling in given range or
 * not. This action is applied commonly at bind-object between two flows, that belong to same session. 
 *
 * From the acknowledgement number of one direction traffic one can predict nearest sequence number 
 * of traffic from other direction that belongs to same session. Basically one needs to check 
 * traffic's sequence number is within some range (delta) of acknowledgement number received 
 * from other direction. 
 */
struct fslx_action_seq_no_validation {
    /** OFPAT_EXPERIMENTER. */
    uint16_t type;                  
    /** Length is 16. */
    uint16_t len;                   
    /** FSLX_VENDOR_ID. */
    uint32_t vendor;                
    /** FSLXAT_SEQ_NO_VALIDATION */
    uint16_t subtype;               
    /** Delta or range value */
    uint16_t delta;                 
    /** Pad to 64 bits. */
    uint8_t  pad[4];                
};
OFP_ASSERT(sizeof(struct fslx_action_seq_no_validation) == 16);

/**************** Freescale Firewall Open Flow Extensions - End *****************************/

/***************** Freescale slb Open Flow Extensions - Begin ***********************/
/***************** Freescale slb  Open Flow Extnesions - End ************************/


/** \ingroup Error_Management 
 *  \enum fslx_error_types
 *  <b>Description:</b>\n
 *  Freescale extension error Codes.
 */
#define FSLX_DEFAULT_SW_ERROR_MSG_SEND_LEN  64

enum fslx_error_codes {
    FSLXET_TABLE_MOD_FAILED_CODE = 0, /** Table mod request failed */
    FSLXBMFC_BIND_OBJ_EXISTS,  /** Already present bind object. */
    FSLXBMFC_INVALID_BIND_OBJ, /** Bind object not added because bind object specified is invalid. */
    FSLXBMFC_UNKNOWN_BIND_OBJ, /** Bind object not modified because a Bind object MODIFY attempted to modify a
                                  non-existent bind object. */
    FSLXBMFC_BAD_COMMAND,     /** Bind object bad command */
    FSLXBMFC_UNKNOWN_ERROR,   /** Bind object unknown error */
    FSLXBMFC_FLOWS_ATTACHED_TO_BIND_OBJ, /** Flows attached to bind object */
    FSLXBMFC_TIMEOUT_OBJ_EXISTS,  /** Timeout object not added because a bind object ADD
                                        attempted to replace an already-present bind object. */
    FSLXBMFC_INVALID_TIMEOUT_OBJ, /** Timeout object not added because bind object specified is invalid. */
    FSLXBMFC_UNKNOWN_TIMEOUT_OBJ /** Timeout object not modified because a Timeout object MODIFY attempted to modify a non-existent bind object. */
};

/* Rev Review ICMP, single or multiple */
/* Rev action to drop */
/* Target policy ID  routing table handle this acion is required */
/* Rev Fw mark and fwmark_mask */

#endif /* FSL_EXT_H */
