/*
 * Copyright (c) 2012, 2013  Freescale.
 *   
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 ***/

/*!\file of_utilities.h
 * \author Rajesh Madabushi   
 * \author Srini Addepalli   
 * \date Jan, 2013  
 * <b>Description:</b>\n 
 * This file contains different utility functions that help in building and 
 * parsing Open Flow messages.
 *
 * Applications can call these APIs before actual API to send Open Flow
 * message.  
 *
 * The utility APIs are typically build, parse match fields, pack actions
 * and instructions. And more utility APIs used in group, metering, queue's etc.
 *
 **/

#ifndef _OF_UTILITY_H
#define _OF_UTILITY_H

//!Memory management utilities to manage Open Flow messages.
/*!This module provides different memory management utilities to store and manage the messages.\n
 * The message consists of message descriptor and actual message buffer which\n
 * holds Open Flow message. 
 *
 * The message descriptor maintains different information pointers of fields in message buffer.\n
 * Utility functions uses these pointers to build and parse Open Flow message fields.
 * 
 * Following is the format of the message, used mainly on sending side of message,\n 
 *   Message Descriptor +  Message buffer holding Open Flow Message   
 *
 * Along with Open Flow message  applications might be using its own local native buffer descriptors like 'skb'\n
 * for its packet processing.  For this  one need to allocate message buffer required to store and manage\n
 * application specific  message buffer descriptor along with message buffer created for Open Flow message.\n
 * Following is the format of the message typically used, in case of head room requirement, mostly on receiving\n
 * side of message from data_path\n
 *     Message Descriptor + Head room for application specific buffer descriptor + Open Flow Message   
 *
 * On sending side
 *   - Application allocates the message by using the message allocation API before calling the APIs\n
 *     to build and send actual Open Flow  message to data_path.
 *   - After sending the messages, Controller frees the memory by using free callback function available\n
 *     in the message descriptor.
 *
 * On receiving side 
 *   - Controller allocates required size memory by using Application registered callback primitives.\n
 *     In case no callback function registered, Controller uses its own API to allocate message.
 *   - The freeing of the memory is done by Application by using free callback function available \n
 *     with in the message descriptor.
 
 *\addtogroup msg_mem_mgmt
**/

//!Utilities used in managing Match Fields of table
/*! This module contains utility functions for match fields of table,\n
 * Provides APIs for\n
 *  -setting fields and their values\n
 *  -parsing fields sent by data_path\n
 *  -finding length of buffer required to build match fields\n
 *\addtogroup Match_Fields 
**/

//!Utility function used in managing instructions
/*! This module contains utility functions for different instructions \n
 * Provides APIs for\n
 *  -pack instructions and their values\n
 *  -finding length of buffers required to build instructions\n
 *\addtogroup Instructions 
**/

//!Utilities used in managing actions
/*! This module contains utility functions for different actions \n
 * Provides APIs for\n
 *  -pack actions and their values\n
 *  -finding length of buffers required to build actions\n
 *\addtogroup Action_Utilities 
**/

//!Utility function used in managing groups
/*! This module contains utility functions for different instructions \n
 * Provides APIs for\n
 *  -finding length of buffers required to build groups and group buckets\n
 *  -appending action buckets of different group types to groups.
 * \sa Action_Utilities 
 *\addtogroup Group_Utilities 
**/

//!Utility function used in managing meters
/*! This module contains utility functions for different instructions \n
 * Provides APIs for\n
 *  -finding length of buffers required to build meters and meter bands\n
 *  -building configuration flags for meters.
 *  -appending different types of bands to meters.
 *\addtogroup Metering_Utilities 
**/

//!Port Description Operations 
/*! This module provides APIs for modify port description \n
 * APIs to set
 *   - port configuration values.  
 *   - port speed
 *   - port medium
 * Also provide APIs to build and send port modification message.
 *\addtogroup Port_Description 
**/
typedef struct of_msg of_msg;

/** \ingroup msg_mem_mgmt 
 * \typedef free_func
 * \brief Free function pointer to release messages. 
 * \param [in] msg - Message contains message_descriptor with buffer to free. 
 */
typedef void (*free_func)(of_msg *msg);

/** \ingroup msg_mem_mgmt 
 * \struct of_msg_descriptor
 * \brief Message Descriptor to manage message buffer fields\n
 * <b> Description </b>\n
 * It maintains information that points to different message buffer data fields. Open Flow message\n
 * usually consists of Open  Flow header and message. The message may carries different details \n
 * like match fields, instructions,actions etc.
 *
 * That is message descriptor basically points to Open Flow protocol headers, match fields, instructions\n
 * actions,etc.
 *
*/
 struct of_msg_descriptor{

   /** Open flow message type, the type of message to whom this message buffer is allocated*/
   enum ofp_type type;

   /** Length of the message buffer, this is equal to length value passed to API to allocate 
    *  message buffer*/
   uint16_t  buffer_len;
   /** This points to start of message buffer, that is it points to start of message after message descriptor.
   * If head room is not required in the message,start_of_buffer points to start of Open Flow message,
   * that is start_of_buffer and start_of_data points the same*/ 
   uint8_t *start_of_buffer; 

   /** This points to 'ofp_header' portion of the Open Flow message*/
   uint8_t *start_of_data;

   /** Length of data written to buffer */
   uint16_t  data_len;

   //uint8_t  *curr_ptr; /*pointing to starting point of variable size information*/
    /**pointing to starting point of variable size information*/
   uint8_t  *utils_ptr; 
   /** length of msg descriptor*/
   uint16_t utils_length; 

   /** Pointer to build variable sized information such as instructions, actions, match fields etc.. */
   uint8_t  *ptr1; /* match, groups */

   /** Current length of the data pointed by ptr1. The ptr1 may points to instructions or match fields\n 
   * or any other that required for that particular Open Flow message.*/
   uint16_t length1;

   /** Same as pointer 1,but at next level data manipulation, it points to different type of data\n
    * that are nested within the the data pointed by 'ptr1'. One example is actions within in the \n
    * 'Write_Action_Instruction' or 'Apply_Action_Instruction'. Like its parent data, typically this data\n 
    * is also of variable size of the data so required to have ptr and length pair to manipulate data.*/
   uint8_t *ptr2;   /* instructions, bucket */
   /** Current length of the data pointed by 'ptr2'. */
   uint16_t length2;

   uint8_t *ptr3;  /* actions */
   /** Current length of the data pointed by 'ptr2'. */
   uint16_t length3;


   /** Call back function used to free message descriptor*/
   free_func free_cbk; 

 };

/** \ingroup msg_mem_mgmt 
 * \struct of_msg
 * \sa struct of_msg_descriptor
 * \brief Message Information \n
 * <b> Description </b>\n
 * The message consists of message descriptor and actual message buffer to hold\n
 * the Open Flow message. 
 */
struct of_msg{
/** offset */
    uint32_t  pkt_mbuf_offset;
/** The message descriptor fields are used to manage and fill information\n
 *  in the message buffer with message header,field,instructions, action,etc.\n 
 *  Different utility functions are provided for applications  to perform these operations. */
      struct of_msg_descriptor  desc; 
/**Start of message buffer */
      uint8_t data[0];
};

/** \ingroup msg_mem_mgmt 
 * \brief Message memory allocation\n
 * <b>Description </b>\n      
 *  API for Applications to allocate the message buffer. 
 *
 *  Applications are expected to calculate size of message buffer by using  utility \n
 *  functions provided. Application may need to calculate the size of each message\n
 *  buffer only once during init and pass the same in subsequent calls of \n
 *  allocate message buffer at run time. 
 *
 *  For example once table is initialized, applications knows the requirements of\n
 *  flow entry of the table. Applications then computes memory sizes for each \n
 *  requirement by using utilities provided, thereby find the message buffer size.\n
 *  And then use the compute size value to subsequent calls of message allocation API.
 *  
 *  As part of API to send message, provided some details like what parameters are need to\n
 *  include for the calculation of message buffer size. 
 *
 *  \param [in] type - Open Flow message type to which this message is allocated.
 *  \param [in] size - Size of the buffer to allocate message.
 *  \return  Pointer to message with type struct of_msg that contains message\n
 *           descriptor with message buffer.
 *  \return  NULL in case of error.
 */
struct of_msg* 
ofu_alloc_message( uint8_t  type,
                   uint16_t size);

/** \ingroup msg_mem_mgmt 
 * \typedef alloc_func
 * \brief Message memory allocation function pointer.\n
 * <b>Description</b>\n      
 *  Memory allocation function pointer for application to register allocation functions\n
 *  as part of memory primitive APIs ofu_reg_dflt_alloc_recv_msg_mem_mgmt_primitive()\n
 *  and ofu_reg_alloc_msg_type_mem_mgmt_primitive(). 
 *  \param [in] msg_type    - Type of the message to which the allocation happen.
 *  \param [in] msg_buf_len - Length of message for the allocation. 
 *  \return  Pointer to message with type struct of_msg that contains message\n
 *           descriptor with message buffer.
 *  \return  NULL in case of error.
 */
typedef struct of_msg* (*alloc_func)(uint8_t msg_type,uint16_t msg_buf_length);                   

/** \ingroup msg_mem_mgmt 
 * \brief Register Default Memory Allocation Primitives\n
 * <b>Description </b>\n      
 * By using this API, application can register memory allocation primitives\n
 * that are used to allocate message of required size,  the allocated message\n
 * buffer are used to read and process asynchronous message received from data_path. 
 *
 * Sometimes multi_part responses also need to register memory primitives that\n
 * used to allocate message buffer to receive messages from  data_path for multi_part\n
 * requests that ON Director sends.
 *
 * The callback function that registered is expected to pass free function pointer as
 * part of struct of_msg returning.
 *
 * The default_alloc_cbk function registered is used as default memory allocation function\n
 * for the ON Director. Along with this default allocation function pointer, application can\n
 * register a memory allocation function for each asynchronous message type separately\n
 * by using ofu_register_alloc_async_memory_mgmt_primitive() API.
 *
 * As a part of reading asynchronous message form data_path, ON Director calls this default 
 * memory alloc function if no call back is registered for the give message type. 
 *
 * \sa ofu_reg_alloc_msg_type_mem_mgmt_primitive() 
 *
 * Even in case no registered default memory allocation pointer exists, ON Director calls
 * ofu_alloc_message() API.
 * \param [in] domain_handle  - Handle of the domain to which application is registering default\n
 *                              memory management primitives.\n
 * \param [in] head_room_size - Size of the head room  that applications wants to reserve for its\n
 *                              local processing.\n 
 * \param [in] default_alloc_cbk -  Default alloc callback function pointer of type alloc_func.
 * \return OFU_INVALID_PARAMS_PASSED
 * \return OFU_MEMORY_ALLOC_SUCCESS
 * \return OFU_MEMORY_ALLOC_FAILED
 */
int32_t
ofu_reg_dflt_alloc_recv_msg_mem_mgmt_primitive(uint64_t   domain_handle,
                                               uint32_t   head_room_size,
                                               alloc_func default_alloc_cbk);

/** \ingroup msg_mem_mgmt 
 * \brief Register Default Memory Allocation Primitives\n
 * <b>Description </b>\n      
 * By using this API, application can register memory allocation primitives\n
 * for given Open Flow message type, this can be typically asynchronous messages or\n
 * multi_part response messages. 
 *
 * The allocate message buffer are used to read and process Open Flow message of given type\n
 * that received from data_path. 
 *
 * The callback function that registered is expected to pass free function pointer as
 * part of struct of_msg returning.
 *
 * In case no registered  callback function exists for the given type, ON Director calls 
 * default memory allocation function pointer by using ofu_reg_dflt_alloc_recv_msg_mem_mgmt_primitive().
 *
 * \sa ofu_reg_dflt_alloc_recv_msg_mem_mgmt_primitive()
 *
 * \param [in] domain_handle  - Handle of the domain to which application registering\n
 *                              memory management primitives.\n
 * \param [in] msg_type       - Type of the message to which application registering memory
 *                              primitives.
 * \param [in] head_room_size - Size of the head room  that applications wants to reserve for its\n
 *                              local processing.\n 
 * \param [in] alloc_cbk      - Alloc callback function pointer of type alloc_func.
 * \return OFU_INVALID_PARAMS_PASSED
 * \return OFU_MEMORY_ALLOC_SUCCESS
 * \return OFU_MEMORY_ALLOC_FAILED
 */
int32_t
ofu_reg_alloc_msg_type_mem_mgmt_primitive(uint64_t domain_handle,
                                          uint16_t  msg_type,
                                          uint32_t head_room_size,
                                          alloc_func alloc_cbk);

/** \ingroup Match_Fields 
 *   OFU_IN_PORT_FIELD_LEN
 *   - Returns length of buffer required for 'In Port' field
 */
#define  OFU_IN_PORT_FIELD_LEN              8 

/** \ingroup Match_Fields 
 *   OFU_IN_PHY_PORT_FIELD_LEN
 *   -Returns length of buffer required for 'In Port Physical port' field
 */
#define  OFU_IN_PHY_PORT_FIELD_LEN          8 

/** \ingroup Match_Fields 
 *   OFU_META_DATA_FIELD_LEN
 *   - Returns length of buffer required for 'Meta Data' field
 */
#define  OFU_META_DATA_FIELD_LEN            12

/** \ingroup Match_Fields 
 *   OFU_META_DATA_MASK_LEN
 *   - Returns length of buffer required for 'Meta Data mask' field
 */
#define  OFU_META_DATA_MASK_LEN             20

/** \ingroup Match_Fields 
 *   OFU_ETH_DST_MAC_ADDR_FIELD_LEN
 *   - Returns length of buffer required for 'Dest MAC address' field
 */
#define  OFU_ETH_DST_MAC_ADDR_FIELD_LEN     10

/** \ingroup Match_Fields 
 *   OFU_ETH_DST_MAC_ADDR_MASK_LEN
 *   - Returns length of buffer required for 'Dest MAC address' mask
 */
#define  OFU_ETH_DST_MAC_ADDR_MASK_LEN      16

/** \ingroup Match_Fields 
 *   OFU_ETH_SRC_MAC_ADDR_FIELD_LEN
 *   - Returns length of buffer required for 'Source MAC address' field
 */
#define  OFU_ETH_SRC_MAC_ADDR_FIELD_LEN     10

/** \ingroup Match_Fields 
 *   OFU_ETH_SRC_MAC_ADDR_MASK_LEN
 *   - Returns length of buffer required for 'Source MAC address' mask
 */
#define  OFU_ETH_SRC_MAC_ADDR_MASK_LEN      16

/** \ingroup Match_Fields 
 *   OFU_ETH_TYPE_FIELD_LEN
 *   - Returns length of buffer required for 'Ethernet Type' field
 */
#define  OFU_ETH_TYPE_FIELD_LEN             6

/** \ingroup Match_Fields 
 *   OFU_VLAN_VID_FIELD_LEN
 *   - Returns length of buffer required for 'VLAN Id' field
 */
#define  OFU_VLAN_VID_FIELD_LEN             6

/** \ingroup Match_Fields 
 *   OFU_VLAN_VID_MASK_LEN
 *   - Returns length of buffer required for 'VLAN Id' mask
 */
#define  OFU_VLAN_VID_MASK_LEN              6

/** \ingroup Match_Fields 
 *   OFU_VLAN_PCP_FIELD_LEN
 *   - Returns length of buffer required for 'VLAN Priority' field
 */
#define  OFU_VLAN_PCP_FIELD_LEN             5

/** \ingroup Match_Fields 
 *   OFU_IP_DSCP_FIELD_LEN
 *   -Returns length of buffer required for 'IP DSCP bits' field
 */
#define  OFU_IP_DSCP_FIELD_LEN              5

/** \ingroup Match_Fields 
 *   OFU_IP_ECN_FIELD_LEN
 *   -Returns length of buffer required for 'IP ECN bits' field
 */
#define  OFU_IP_ECN_FIELD_LEN               5

/** \ingroup Match_Fields 
 *   OFU_IP_PROTO_FIELD_LEN
 *   -Returns length of buffer required for 'IP Protocol' field
 */
#define  OFU_IP_PROTO_FIELD_LEN             5

/** \ingroup Match_Fields 
 *   OFU_IPV4_SRC_FIELD_LEN
 *   -Returns length of buffer required for 'IPv4 Source' field
 */
#define  OFU_IPV4_SRC_FIELD_LEN             8

/** \ingroup Match_Fields 
 *   OFU_IPV4_SRC_MASK_LEN
 *   -Returns length of buffer required for 'IPv4 Source' mask
 */
#define  OFU_IPV4_SRC_MASK_LEN              12

/** \ingroup Match_Fields 
 *   OFU_IPV4_DST_FIELD_LEN
 *   -Returns length of buffer required for 'IPv4 Destination' field
 */
#define  OFU_IPV4_DST_FIELD_LEN             8

/** \ingroup Match_Fields 
 *   OFU_IPV4_DST_MASK_LEN
 *   -Returns length of buffer required for 'IPv4 Destination' mask
 */
#define  OFU_IPV4_DST_MASK_LEN              12

/** \ingroup Match_Fields 
 *   OFU_TCP_SRC_FIELD_LEN
 *   -Returns length of buffer required for 'TCP Source Port' field
 */
#define  OFU_TCP_SRC_FIELD_LEN              6

/** \ingroup Match_Fields 
 *   OFU_TCP_DST_FIELD_LEN
 *   -Returns length of buffer required for 'TCP Destination port' field
 */
#define  OFU_TCP_DST_FIELD_LEN              6

/** \ingroup Match_Fields 
 *   OFU_UDP_SRC_FIELD_LEN
 *   -Returns length of buffer required for 'UDP Source port' field
 */
#define  OFU_UDP_SRC_FIELD_LEN              6

/** \ingroup Match_Fields 
 *   OFU_UDP_DST_FIELD_LEN
 *   -Returns length of buffer required for 'UDP Destination port' field
 */
#define  OFU_UDP_DST_FIELD_LEN              6

/** \ingroup Match_Fields 
 *   OFU_SCTP_SRC_FIELD_LEN
 *   -Returns length of buffer required for 'SCTP Source Port' field
 */
#define  OFU_SCTP_SRC_FIELD_LEN             6

/** \ingroup Match_Fields 
 *   OFU_SCTP_DST_FIELD_LEN
 *   -Returns length of buffer required for 'SCTP Destination Port' field
 */
#define  OFU_SCTP_DST_FIELD_LEN             6

/** \ingroup Match_Fields 
 *   OFU_ICPMPV4_TYPE_FIELD_LEN
 *   -Returns length of buffer required for 'ICMPv4 type' field
 */
#define  OFU_ICMPV4_TYPE_FIELD_LEN         5

/** \ingroup Match_Fields 
 *   OFU_ICPMPV4_CODE_FIELD_LEN
 *   -Returns length of buffer required for 'ICMPv4 code' field
 */
#define  OFU_ICMPV4_CODE_FIELD_LEN         5

/** \ingroup Match_Fields 
 *   OFU_IPSEC_SPI_FIELD_LEN
 *   -Returns length of buffer required for 'ESP or AH Header SPI' field
 */
#define  OFU_IPSEC_SPI_FIELD_LEN             8

/** \ingroup Match_Fields 
 *   OFU_ARP_OP_FIELD_LEN
 *   -Returns length of buffer required for 'ARP OP code' field
 */
#define  OFU_ARP_OP_FIELD_LEN               6

/** \ingroup Match_Fields 
 *   OFU_ARP_SPA_FIELD_LEN
 *   -Returns length of buffer required for 'ARP Source IPv4 Addr' field
 */
#define  OFU_ARP_SPA_FIELD_LEN              8

/** \ingroup Match_Fields 
 *   OFU_ARP_SPA_MASK_LEN
 *   -Returns length of buffer required for 'ARP Source IPv4 Addr' mask
 */
#define  OFU_ARP_SPA_MASK_LEN               12

/** \ingroup Match_Fields 
 *   OFU_ARP_TPA_FIELD_LEN
 *   -Returns length of buffer required for 'ARP Dest IPv4 Addr' field
 */
#define  OFU_ARP_TPA_FIELD_LEN              8

/** \ingroup Match_Fields 
 *   OFU_ARP_TPA_MASK_LEN
 *   -Returns length of buffer required for 'ARP Dest IPv4 Addr' mask
 */
#define  OFU_ARP_TPA_MASK_LEN               12

/** \ingroup Match_Fields 
 *   OFU_ARP_SHA_FIELD_LEN
 *   -Returns length of buffer required for 'ARP Source HW Addr' field
 */
#define  OFU_ARP_SHA_FIELD_LEN              10

/** \ingroup Match_Fields 
 *   OFU_ARP_SHA_MASK_LEN
 *   -Returns length of buffer required for 'ARP Source HW Addr' mask
 */
#define  OFU_ARP_SHA_MASK_LEN               16

/** \ingroup Match_Fields 
 *   OFU_ARP_DHA_FIELD_LEN
 *   -Returns length of buffer required for 'ARP Dest HW Addr' field
 */
#define  OFU_ARP_DHA_FIELD_LEN              10

/** \ingroup Match_Fields 
 *   OFU_ARP_DHA_MASK_LEN
 *   -Returns length of buffer required for 'ARP Dest HW Addr' mask
 */
#define  OFU_ARP_DHA_MASK_LEN               16

/** \ingroup Match_Fields 
 *   OFU_IPV6_SRC_FIELD_LEN
 *   -Returns length of buffer required for 'IPv6 Source Addr' field
 */
#define  OFU_IPV6_SRC_FIELD_LEN             20 

/** \ingroup Match_Fields 
 *   OFU_IPV6_SRC_MASK_LEN
 *   -Returns length of buffer required for 'IPv6 Source Addr' mask
 */
#define  OFU_IPV6_SRC_MASK_LEN              36

/** \ingroup Match_Fields 
 *   OFU_IPV6_DST_FIELD_LEN
 *   -Returns length of buffer required for 'IPv6 Dest Addr' field
 */
#define  OFU_IPV6_DST_FIELD_LEN             20 

/** \ingroup Match_Fields 
 *   OFU_IPV6_DST_MASK_LEN
 *   -Returns length of buffer required for 'IPv6 Dest Addr' mask
 */
#define  OFU_IPV6_DST_MASK_LEN              36

/** \ingroup Match_Fields 
 *   OFU_IPV6_FLABEL_FIELD_LEN
 *   -Returns length of buffer required for 'IPV6 Flow Label' field
 */
#define  OFU_IPV6_FLABEL_FIELD_LEN          8 

/** \ingroup Match_Fields 
 *   OFU_IPV6_FLABEL_MASK_LEN
 *   -Returns length of buffer required for 'IPv6 Flow Label' mask
 */
#define  OFU_IPV6_FLABEL_MASK_LEN           12

/** \ingroup Match_Fields 
 *   OFU_ICMPV6_TYPE_FIELD_LEN
 *   -Returns length of buffer required for 'ICMPv6 Type' field
 */
#define  OFU_ICMPV6_TYPE_FIELD_LEN          5 

/** \ingroup Match_Fields 
 *   OFU_ICMPV6_CODE_FIELD_LEN
 *   -Returns length of buffer required for 'ICMPv6 code' field
 */
#define  OFU_ICMPV6_CODE_FIELD_LEN          5 

/** \ingroup Match_Fields 
 *   OFU_IPV6_ND_TARGET_FIELD_LEN
 *   -Returns length of buffer required for 'ND target address' field
 */
#define  OFU_IPV6_ND_TARGET_FIELD_LEN       20 

/** \ingroup Match_Fields 
 *   OFU_IPV6_ND_SLL_FIELD_LEN
 *   -Returns length of buffer required for 'ND target source link layer' field
 */
#define  OFU_IPV6_ND_SLL_FIELD_LEN          10 

/** \ingroup Match_Fields 
 *   OFU_IPV6_ND_DLL_FIELD_LEN
 *   -Returns length of buffer required for 'ND target target link layer' field
 */
#define  OFU_IPV6_ND_DLL_FIELD_LEN          10 

/** \ingroup Match_Fields 
 *   OFU_MPLS_LABEL_FIELD_LEN
 *   -Returns length of buffer required for 'MPLS Label' field
 */
#define  OFU_MPLS_LABEL_FIELD_LEN           8 

/** \ingroup Match_Fields 
 *   OFU_MPLS_TC_FIELD_LEN
 *   - Returns length of buffer required for 'MPLS TC' field
 */
#define  OFU_MPLS_TC_FIELD_LEN              5 

/** \ingroup Match_Fields 
 *   OFU_MPLS_BOS_FIELD_LEN
 *   - Returns length of buffer required for 'MPLS BoS' field
 */
#define  OFU_MPLS_BOS_FIELD_LEN             5 

/** \ingroup Match_Fields 
 *   OFU_PBB_ISID_FIELD_LEN
 *   - Returns length of buffer required for 'PBB I-SID' field
 */
#define  OFU_PBB_ISID_FIELD_LEN             7 

/** \ingroup Match_Fields 
 *   OFU_PBB_ISID_MASK_LEN
 *   - Returns length of buffer required for 'PBB I-SID' mask
 */
#define  OFU_PBB_ISID_MASK_LEN              7 

/** \ingroup Match_Fields 
 *   OFU_TUNNEL_ID_FIELD_LEN
 *   - Returns length of buffer required for 'Tunnel ID' field
 */
#define  OFU_TUNNEL_ID_FIELD_LEN            12 

/** \ingroup Match_Fields 
 *   OFU_TUNNEL_ID_MASK_LEN
 *   - Returns length of buffer required for 'Tunnel ID' mask
 */
#define  OFU_TUNNEL_ID_MASK_LEN             12 

/** \ingroup Match_Fields 
 *   OFU_IPV6_EXTHDR_FIELD_LEN
 *   - Returns length of buffer required for 'Extension Header pseudo-field'
 */
#define  OFU_IPV6_EXTHDR_FIELD_LEN          6 

/** \ingroup Match_Fields 
 *   OFU_IPV6_EXTHDR_MASK_LEN
 *   - Returns length of the buffer required for 'IPv6 Extension Header pseudo-field' mask
 */
#define  OFU_IPV6_EXTHDR_MASK_LEN           8

/** \ingroup Instructions 
 *   OFU_GOTO_TABLE_INSTRUCTION_LEN
 *   - Returns length of buffer required for 'GoTo Table Instruction'
 */
#define OFU_GOTO_TABLE_INSTRUCTION_LEN       sizeof(struct ofp_instruction_goto_table)

/** \ingroup Instructions 
 *   OFU_WRITE_META_DATA_INSTRUCTION_LEN
 *   - Returns length of buffer required for 'Write Meta Data Instruction'
 */
#define OFU_WRITE_META_DATA_INSTRUCTION_LEN  sizeof(struct ofp_instruction_write_metadata)

/** \ingroup Instructions 
 *   OFU_CLEAR_ACTION_INSTRUCTION_LEN
 *   - Returns length of buffer required for 'Clear Action Instruction'
 */
#define OFU_CLEAR_ACTION_INSTRUCTION_LEN     sizeof(struct ofp_instruction_actions)

/** \ingroup Instructions 
 * OFU_APPLY_ACTION_INSTRUCTION_LEN
 * - Returns length of buffer required for apply_action_instruction. 
 *   Along with including following instruction length, applications needs to add 
 *   lengths of actions that are pushing to apply_action_instruction.
 */
#define OFU_APPLY_ACTION_INSTRUCTION_LEN sizeof(struct ofp_instruction_actions)

/** \ingroup Instructions 
 * OFU_WRITE_ACTION_INSTRUCTION_LEN
 * - Returns length of buffer required for write_action_instruction. 
 *   Along with including following instruction length, applications needs to add 
 *   lengths of actions that are pushing to write_action_instruction.
 */
#define OFU_WRITE_ACTION_INSTRUCTION_LEN sizeof(struct ofp_instruction_actions)

/** \ingroup Instructions 
 *   OFU_METER_INSTRUCTION_LEN
 *   - Returns length of buffer required for 'Meter Instruction'
 */
#define OFU_METER_INSTRUCTION_LEN        sizeof(struct ofp_instruction_meter)

/** \ingroup Instructions 
 *   OFU_EXPERIMENTER_INSTRUCTION_LEN
 *   - Returns length of buffer required for 'Experimenter Instruction'
 */
#define OFU_EXPERIMENTER_INSTRUCTION_LEN sizeof(struct ofp_instruction_experimenter)

/** \ingroup Action_Utilities
 *   OFU_OUTPUT_ACTION_LEN
 *   - Returns length of buffer required for 'Output Action'
 */
#define OFU_OUTPUT_ACTION_LEN            sizeof(struct ofp_action_output)

/** \ingroup Action_Utilities
 *   OFU_GROUP_ACTION_LEN
 *   - Returns length of buffer required for 'Group Action'
 */
#define OFU_GROUP_ACTION_LEN             sizeof(struct ofp_action_group)

/** \ingroup Action_Utilities
 *   OFU_SET_QUEUE_ACTION_LEN
 *   - Returns length of buffer required for 'Set Queue Action'
 */
#define OFU_SET_QUEUE_ACTION_LEN         sizeof(struct ofp_action_set_queue)

/** \ingroup Action_Utilities
 *   OFU_SET_MPLS_TTL_ACTION_LEN
 *   - Returns length of buffer required for 'Set MPLS TTL field Action'
 */
#define OFU_SET_MPLS_TTL_ACTION_LEN      sizeof(struct ofp_action_mpls_ttl)

/** \ingroup Action_Utilities
 *   OFU_DEC_MPLS_TTL_ACTION_LEN
 *   - Returns length of buffer required for 'Decrement MPLS TTL value action'
 */
#define OFU_DEC_MPLS_TTL_ACTION_LEN      sizeof(struct ofp_action_header)

/** \ingroup Action_Utilities
 *   OFU_SET_IP_TTL_ACTION_LEN
 *   - Returns length of buffer required for ' Set IP TTL Field Action'
 */
#define OFU_SET_IP_TTL_ACTION_LEN        sizeof(struct ofp_action_nw_ttl)

/** \ingroup Action_Utilities
 *   OFU_DEC_IP_TTL_ACTION_LEN
 *   - Returns length of buffer required for 'Decrement IP TTL field Action'
 */
#define OFU_DEC_IP_TTL_ACTION_LEN        sizeof(struct ofp_action_nw_ttl)

/** \ingroup Action_Utilities
 *   OFU_COPY_TTL_OUTWARD_ACTION_LEN
 *   - Returns length of buffer required for 'Copy TTL from the next-to-outermost header Action'
 */
#define OFU_COPY_TTL_OUTWARD_ACTION_LEN  sizeof(struct ofp_action_header)

/** \ingroup Action_Utilities
 *   OFU_COPY_TTL_INWARD_ACTION_LEN
 *   - Returns length of buffer required for 'Copy TTL from the outermost header with TTL to the 
 *     next-to-outermost header Action'
 */
#define OFU_COPY_TTL_INWARD_ACTION_LEN   sizeof(struct ofp_action_header)

/** \ingroup Action_Utilities
 *   OFU_PUSH_VLAN_HEADER_ACTION_LEN
 *   - Returns length of buffer required for 'Push VLAN Header Action'
 */
#define OFU_PUSH_VLAN_HEADER_ACTION_LEN  sizeof(struct ofp_action_push)

/** \ingroup Action_Utilities
 *   OFU_POP_VLAN_HEADER_ACTION_LEN
 *   - Returns length of buffer required for 'Pop VLAN Header  Action'
 */
#define OFU_POP_VLAN_HEADER_ACTION_LEN   sizeof(struct ofp_action_header)

/** \ingroup Action_Utilities
 *   OFU_PUSH_MPLS_HEADER_ACTION_LEN
 *   - Returns length of buffer required for 'Push MPLS header Action'
 */
#define OFU_PUSH_MPLS_HEADER_ACTION_LEN  sizeof(struct ofp_action_push)

/** \ingroup Action_Utilities
 *   OFU_POP_MPLS_HEADER_ACTION_LEN
 *   - Returns length of buffer required for 'Pop MPLS header Action'
 */
#define OFU_POP_MPLS_HEADER_ACTION_LEN   sizeof(struct ofp_action_pop_mpls)

/** \ingroup Action_Utilities
 *   OFU_PUSH_PBB_HEADER_ACTION_LEN
 *   - Returns length of buffer required for 'Push PBB Header Action'
 */
#define OFU_PUSH_PBB_HEADER_ACTION_LEN   sizeof(struct ofp_action_push)

/** \ingroup Action_Utilities
 *   OFU_POP_PBB_HEADER_ACTION_LEN
 *   - Returns length of buffer required for 'Pop PBB Header Action'
 */
#define OFU_POP_PBB_HEADER_ACTION_LEN    sizeof(struct ofp_action_header)


#define OFU_SET_FIELD_WITH_IN_PORT_LABEL_FIELD_ACTION_LEN  (sizeof(struct ofp_action_set_field) + 4)
#define OFU_SET_FIELD_WITH_PHY_PORT_LABEL_FIELD_ACTION_LEN (sizeof(struct ofp_action_set_field) + 4)
#define OFU_SET_FIELD_WITH_TUNNEL_ID_FIELD_ACTION_LEN (sizeof(struct ofp_action_set_field) + 8)

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_ETH_SRC_MAC_ACTION_LEN
 *   - Returns length of buffer required for 'Source MAC address set field action"
 */
#define  OFU_SET_FIELD_WITH_ETH_SRC_MAC_ACTION_LEN          (sizeof(struct ofp_action_set_field) + 6) 

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_ETH_DST_MAC_ACTION_LEN
 *   - Returns length of buffer required for 'Dest MAC address set field action'
 */
#define  OFU_SET_FIELD_WITH_ETH_DST_MAC_ACTION_LEN          (sizeof(struct ofp_action_set_field) + 6) 

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_ETH_TYPE_ACTION_LEN
 *   - Returns length of buffer required for 'Ethernet Type set field action'
 */
#define  OFU_SET_FIELD_WITH_ETH_TYPE_ACTION_LEN             (sizeof(struct ofp_action_set_field) + 2)     

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_VLAN_VID_ACTION_LEN
 *   - Returns length of buffer required for 'VLAN Id set field action'
 */
#define  OFU_SET_FIELD_WITH_VLAN_VID_ACTION_LEN             (sizeof(struct ofp_action_set_field) + 2)   

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_VLAN_PCP_ACTION_LEN
 *   - Returns length of buffer required for 'VLAN Priority set field action'
 */
#define  OFU_SET_FIELD_WITH_VLAN_PCP_ACTION_LEN             (sizeof(struct ofp_action_set_field) + 1)

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_IP_DSCP_FIELD_ACTION_LEN
 *   - Returns length of buffer required for 'IP DSCP set field action'
 */
#define  OFU_SET_FIELD_WITH_IP_DSCP_FIELD_ACTION_LEN (sizeof(struct ofp_action_set_field) + 1) 

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_IP_ECN_FIELD_ACTION_LEN
 *   - Returns length of buffer required for 'IP ECN set field action'
 */
#define  OFU_SET_FIELD_WITH_IP_ECN_FIELD_ACTION_LEN         (sizeof(struct ofp_action_set_field) + 1)

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_IP_PROTO_FIELD_ACTION_LEN
 *   - Returns length of buffer required for 'IP Protocol set field action'
 */
#define  OFU_SET_FIELD_WITH_IP_PROTO_FIELD_ACTION_LEN       (sizeof(struct ofp_action_set_field) + 1)

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_IPV4_SRC_FIELD_ACTION_LEN
 *   - Returns length of buffer required for 'IPv4 Source Addr set field action'
 */
#define  OFU_SET_FIELD_WITH_IPV4_SRC_FIELD_ACTION_LEN       (sizeof(struct ofp_action_set_field) + 4)

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_IPV4_DST_FIELD_ACTION_LEN
 *   - Returns length of buffer required for 'IPv4 Destination Addr set field action'
 */
#define  OFU_SET_FIELD_WITH_IPV4_DST_FIELD_ACTION_LEN       (sizeof(struct ofp_action_set_field) + 4)

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_TCP_SRC_PORT_FIELD_ACTION_LEN
 *   - Returns length of buffer required for 'TCP Source Port set field action'
 */
#define  OFU_SET_FIELD_WITH_TCP_SRC_PORT_FIELD_ACTION_LEN   (sizeof(struct ofp_action_set_field) + 2)

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_TCP_DST_PORT_FIELD_ACTION_LEN
 *   - Returns length of buffer required for ' TCP Dest Port set field action'
 */
#define  OFU_SET_FIELD_WITH_TCP_DST_PORT_FIELD_ACTION_LEN   (sizeof(struct ofp_action_set_field) + 2)

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_UDP_SRC_PORT_FIELD_ACTION_LEN
 *   - Returns length of buffer required for 'UDP Source port set field action'
 */
#define  OFU_SET_FIELD_WITH_UDP_SRC_PORT_FIELD_ACTION_LEN   (sizeof(struct ofp_action_set_field) + 2)

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_UDP_DST_PORT_FIELD_ACTION_LEN
 *   - Returns length of buffer required for 'UDP Dest port set field action'
 */
#define  OFU_SET_FIELD_WITH_UDP_DST_PORT_FIELD_ACTION_LEN   (sizeof(struct ofp_action_set_field) + 2)

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_SCTP_SRC_FIELD_ACTION_LEN
 *   - Returns length of buffer required for 'SCTP Source port set field action'
 */
#define  OFU_SET_FIELD_WITH_SCTP_SRC_FIELD_ACTION_LEN       (sizeof(struct ofp_action_set_field) + 2)

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_SCTP_DST_FIELD_ACTION_LEN
 *   - Returns length of buffer required for 'SCTP Dest port set field action'
 */
#define  OFU_SET_FIELD_WITH_SCTP_DST_FIELD_ACTION_LEN       (sizeof(struct ofp_action_set_field) + 2)

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_ICPMPV4_TYPE_FIELD_ACTION_LEN
 *   - Returns length of buffer required for 'ICMPv4 type set field action'
 */
#define  OFU_SET_FIELD_WITH_ICPMPV4_TYPE_FIELD_ACTION_LEN   (sizeof(struct ofp_action_set_field) + 1)

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_ICPMPV4_CODE_FIELD_ACTION_LEN
 *   - Returns length of buffer required for 'ICMPv4 code set field action'
 */
#define  OFU_SET_FIELD_WITH_ICPMPV4_CODE_FIELD_ACTION_LEN   (sizeof(struct ofp_action_set_field) + 1)

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_ARP_OP_FIELD_ACTION_LEN
 *   - Returns length of buffer required for 'ARP OP code set field action'
 */
#define  OFU_SET_FIELD_WITH_ARP_OP_FIELD_ACTION_LEN         (sizeof(struct ofp_action_set_field) + 2)

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_ARP_SPA_FIELD_ACTION_LEN
 *   -Returns length of buffer required for 'ARP Source IPv4 Addr set field action'
 */
#define  OFU_SET_FIELD_WITH_ARP_SPA_FIELD_ACTION_LEN        (sizeof(struct ofp_action_set_field) + 4)

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_ARP_TPA_FIELD_ACTION_LEN
 *   -Returns length of buffer required for 'ARP Target IPv4 set field action'
 */
#define  OFU_SET_FIELD_WITH_ARP_TPA_FIELD_ACTION_LEN        (sizeof(struct ofp_action_set_field) + 4)

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_ARP_SHA_FIELD_ACTION_LEN
 *   -Returns length of buffer required for 'ARP Source hw addr set field action'
 */
#define  OFU_SET_FIELD_WITH_ARP_SHA_FIELD_ACTION_LEN        (sizeof(struct ofp_action_set_field) + 6)

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_ARP_DHA_FIELD_ACTION_LEN
 *   - Returns length of buffer required for 'ARP Target hw addr set field action'
 */
#define  OFU_SET_FIELD_WITH_ARP_DHA_FIELD_ACTION_LEN        (sizeof(struct ofp_action_set_field) + 6)

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_IPV6_SRC_FIELD_ACTION_LEN
 *   - Returns length of buffer required for 'IPv6 Source Addr set field action'
 */
#define  OFU_SET_FIELD_WITH_IPV6_SRC_FIELD_ACTION_LEN       (sizeof(struct ofp_action_set_field) + 16) 

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_IPV6_DST_FIELD_ACTION_LEN
 *   - Returns length of buffer required for 'IPv6 Dest Addr set field action'
 */
#define  OFU_SET_FIELD_WITH_IPV6_DST_FIELD_ACTION_LEN       (sizeof(struct ofp_action_set_field) + 16) 

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_IPV6_FLABEL_FIELD_ACTION_LEN
 *   - Returns length of buffer required for 'IPv6 Label set field action'
 */
#define  OFU_SET_FIELD_WITH_IPV6_FLABEL_FIELD_ACTION_LEN    (sizeof(struct ofp_action_set_field) + 4)  

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_ICMPV6_TYPE_FIELD_ACTION_LEN
 *   - Returns length of buffer required for 'ICMPV6 Type set field action'
 */
#define  OFU_SET_FIELD_WITH_ICMPV6_TYPE_FIELD_ACTION_LEN    (sizeof(struct ofp_action_set_field) + 1)

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_ICMPV6_CODE_FIELD_ACTION_LEN
 *   -Returns length of buffer required for 'ICMPv6 code set field action'
 */
#define  OFU_SET_FIELD_WITH_ICMPV6_CODE_FIELD_ACTION_LEN    (sizeof(struct ofp_action_set_field) + 1) 

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_IPV6_ND_TARGET_FIELD_ACTION_LEN
 *   -Returns length of buffer required for 'ND Target Addr set field action'
 */
#define  OFU_SET_FIELD_WITH_IPV6_ND_TARGET_FIELD_ACTION_LEN (sizeof(struct ofp_action_set_field) + 16) 

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_IPV6_ND_SLL_FIELD_ACTION_LEN
 *   -Returns length of buffer required for 'ND Source Link Layer addr set field action'
 */
#define  OFU_SET_FIELD_WITH_IPV6_ND_SLL_FIELD_ACTION_LEN    (sizeof(struct ofp_action_set_field) + 6)

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_IPV6_ND_DLL_FIELD_ACTION_LEN
 *   -Returns length of buffer required for 'ND Destination Link Layer addr set field action'
 */
#define  OFU_SET_FIELD_WITH_IPV6_ND_DLL_FIELD_ACTION_LEN    (sizeof(struct ofp_action_set_field) + 6) 

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_MPLS_LABEL_FIELD_ACTION_LEN
 *   -Returns length of buffer required for 'MPLS Label set field action'
 */
#define  OFU_SET_FIELD_WITH_MPLS_LABEL_FIELD_ACTION_LEN     (sizeof(struct ofp_action_set_field) + 4) 

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_MPLS_TC_FIELD_ACTION_LEN
 *   -Returns length of buffer required for 'MPLS TC set field action'
 */
#define  OFU_SET_FIELD_WITH_MPLS_TC_FIELD_ACTION_LEN        (sizeof(struct ofp_action_set_field) + 1) 

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_MPLS_BOS_FIELD_ACTION_LEN
 *   -Returns length of buffer required for 'MPLS BoS set field action'
 */
#define  OFU_SET_FIELD_WITH_MPLS_BOS_FIELD_ACTION_LEN       (sizeof(struct ofp_action_set_field) + 1) 

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_PBB_ISID_FIELD_ACTION_LEN
 *   -Returns length of buffer required for 'PBB I-sid set field action'
 */
#define  OFU_SET_FIELD_WITH_PBB_ISID_FIELD_ACTION_LEN       (sizeof(struct ofp_action_set_field) + 3) 

/** \ingroup Action_Utilities 
 *   OFU_SET_FIELD_WITH_IPV6_EXTHDR_FIELD_ACTION_LEN
 *   -Returns length of buffer required for 'IPv6 extension header set pseudo-field action'
 */
#define  OFU_SET_FIELD_WITH_IPV6_EXTHDR_FIELD_ACTION_LEN    (sizeof(struct ofp_action_set_field) + 2) 

/** \ingroup Action_Utilities 
 *   OFU_EXPERIMENTER_ACTION_LEN
 *   - Returns length of buffer required for 'Experimenter Action'
 */
#define OFU_EXPERIMENTER_ACTION_LEN        sizeof(struct ofp_action_experimenter_header)

/* Utility functions to add Instructions. All the necessary instructions that are required  for
 * a flow entry are  pushed by using  following utility function. 
 * All the necessary instruction should be in correct sequence of order in which it is
 * expected to execute when a flow matches the entry.
 *
 * All the instruction should be executed between ofu_start_pushing_instructions() and
 * ofu_end_pushing_instructions()
 */


/** \ingroup Instructions 
 *  \brief Start Pushing Instructions\n 
 *  <b> Description</b>\n                                                                
 *  Initialize message descriptor values. Applications must issue this call before pushing\n
 *  the actual set of instructions.   
 *  \param [in] msg - Message descriptor with buffer to store message.\n 
 *  \param [out] length_of_match_fields - Total length of match fields in the buffer
 */ 
void
ofu_start_pushing_instructions(struct of_msg *msg, uint16_t length_of_match_fields);

/** \ingroup Instructions 
 * \brief Meta data Instruction\n
 * <b>Description </b>\n                                                               
 *  API to push meta data instruction to the message buffer.
 *  \param [in] msg - Message descriptor with buffer to store instructions.
 *  \param [in] meta_data      - 64 bit meta data value to write
 *  \param [in] meta_data_mask - 64 bit Bit-Mask to use during meta data write, 
 *  \return OFU_NO_ROOM_IN_BUFFER 
 *  \return OFU_INSTRUCTION_PUSH_SUCCESS
 */
int32_t
ofu_push_write_meta_data_instruction(struct of_msg *msg,
                                    uint64_t       meta_data,
                                    uint64_t       meta_data_mask);
/** \ingroup Instructions 
 * \brief Go To Table Instruction\n
 * <b>Description </b>\n                                                               
 *  Push "Go to table instruction" to Instruction set. This instruction informs Id of the next table in the 
 *  pipeline processing. The flow entries pushed to last table of the pipeline can not include this instruction.
 *  \param [in] msg      - Message descriptor with buffer to store instructions.
 *  \param [in] table_id -  Table ID of the next table to which packet of the flow need to pass 
 *  \return OFU_NO_ROOM_IN_BUFFER 
 *  \return OFU_INSTRUCTION_PUSH_SUCCESS
 */
int32_t
ofu_push_go_to_table_instruction(struct  of_msg *msg,
                                 uint8_t table_id);

/** \ingroup Instructions 
 * \brief Clear Action Instruction\n
 * <b>Description </b>\n                                                               
 * Push "Clear Action Instruction" to Instruction set, it clears all the currently available actions 
 * in the action set immediately.
 * \param [in] msg  - Message descriptor with buffer to store instructions.
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_INSTRUCTION_PUSH_SUCCESS
 */
int32_t
ofu_push_clear_action_instruction(struct of_msg *msg);

/** \ingroup Instructions 
 * \brief Meter Instruction\n
 * <b>Description </b>\n                                                               
 *  Push Meter instruction to instruction set. It indicates the meter Id to apply on the flow.
 * \param [in] msg      - Message descriptor with buffer to store instructions.
 * \param [in] meter_id - Meter Id to apply on flow. 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_INSTRUCTION_PUSH_SUCCESS
 */
int32_t
ofu_push_meter_instruction(struct of_msg *msg,
                           uint32_t  meter_id);
/** \ingroup Instructions 
 * \brief Write action Instruction\n
 * <b>Description </b>\n                                                               
 *  Push Write action instruction to instruction set of the message buffer. 
 *  Applications are expected to push one or more actions to write action instruction set.
 *
 *  It initializes buffer descriptor to start actions. No explicit call 'ofu_start_pushing_actions()'
 *  is required to  push one or more actions to apply action instruction set.
 *
 *  Applications must call this API before actually pushing the actions to instruction set.
 * \param [in] msg      - Message descriptor with buffer to store instructions.
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_INSTRUCTION_PUSH_SUCCESS
 */
int32_t
ofu_start_pushing_write_action_instruction(struct of_msg *msg);

/** \ingroup Action_Utilities 
 * \brief End Apply Action Instruction\n
 * <b>Description </b>\n                                                               
 * This is to indicate the completion of pushing all actions to write action instruction set.
 * Applications  MUST issue this call after pushing all actions to write action instruction set. 
 * The output parameters may be used by applications for debugging purpose. 
 * \param [in] msg                - Message descriptor with buffer that contains added actions 
 * \param [out] starting_location - pointer to starting location of the action pushed in the  message\n
 *                                  buffer
 * \param [out] length_of_actions - Total length of actions pushed in the buffer
 */
void 
ofu_end_pushing_write_action_instruction(struct of_msg *msg,
                                         uint8_t *starting_location, 
                                         uint16_t *length_of_actions);

/** \ingroup Instructions 
 * \brief Apply action Instruction\n
 * <b>Description </b>\n                                                               
 *  Push apply action instruction to instruction set of the message buffer. 
 *  Applications are expected to push one or more actions to apply action instruction set.
 *
 *  It initializes buffer descriptor to start actions. No explicit call 'ofu_start_pushing_actions()'
 *  is required to  push one or more actions to apply action instruction set.
 *
 *  Applications must call this API before actually pushing the actions instruction set.
 *  \param [in] msg      - Message descriptor with buffer to store instructions.
 *  \return OFU_NO_ROOM_IN_BUFFER 
 *  \return OFU_INSTRUCTION_PUSH_SUCCESS
 */
int32_t
ofu_start_pushing_apply_action_instruction(struct of_msg *msg);


/** \ingroup Action_Utilities 
 * \brief End Apply Action Instruction\n
 * <b>Description </b>\n                                                               
 * This is to indicate the completion of pushing all actions to apply action instruction set.
 * Applications  MUST issue this call after pushing all actions to apply action instruction set. 
 * The output parameters may be used by applications for debugging purpose. 
 * \param [in] msg                - Message descriptor with buffer that contains added actions 
 * \param [out] starting_location - pointer to starting location of the action pushed in the  message\n
 *                                  buffer
 * \param [out] length_of_actions - Total length of actions pushed in the buffer
 */
void 
ofu_end_pushing_apply_action_instruction(struct of_msg *msg,
 //                                        uint8_t *starting_location, 
                                         uint16_t length_of_actions);



/** \ingroup Instructions 
 * \brief End Instructions\n
 * <b>Description </b>\n                                                               
 * This is to indicate the completion of pushing the instructions. Applications must issue this call 
 * after pushing all the instructions. The output parameters may be used by applications for debugging purpose.    
 * \param [in] msg - Message descriptor with buffer that contains added instructions. 
 * \param [out] starting_location      - pointer to starting location of the instructions pushed in the buffer.
 * \param [out] length_of_instructions - The total length of instructions pushed in the buffer.
 */
void 
ofu_end_pushing_instructions(struct of_msg *msg,
                            uint8_t *starting_location, 
                            uint16_t *length_of_instructions);


/* Utility functions to add actions the message buffer, it is expected from applications
 * to push in the sequence of order in which it needs to execute.
 *
 * Actions are required to either to add either for Apply Action Instructions or  Write Action Instructions or
 * Group buckets or Packet out message.
 */

/** \ingroup Action_Utilities 
 * \brief Start Actions\n
 * <b>Description </b>\n                                                               
 * Initialize message descriptor values for the actions. Applications must issue this call 
 * before pushing the actual action set.    
 * \param [in] msg - Message descriptor with buffer to store action in buffer 
 */
void 
ofu_start_pushing_actions(struct of_msg *msg);

/** \ingroup Action_Utilities 
 * \brief Output Port Action\n
 * <b>Description </b>\n                                                               
 * This pushes output port action to messages buffer. This action is used to send packet
 * specified output port.
 * \param [in] msg     - Message descriptor with buffer to add actions. 
 * \param [in] port_no - Port through which the packet should be sent.
 * \param [in] max_len - The maximum amount of data from a packet that should be sent when the port is OFPP_CONTROLLER.\n
 *                       One can specify OFPCML_NO_BUFFER  that indicates no buffering should be applied and the whole packet\n
 *                       is to be sent to the ON Director.
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_output_action(struct of_msg *msg,
                      uint32_t  port_no,
                      uint16_t  max_len);
/** \ingroup Action_Utilities 
 * \brief Group Action\n
 * <b>Description </b>\n                                                               
 * Pushes group action to message buffer. This actions applies group actions specified to the packet.
 * \param [in] msg      - Message descriptor with buffer to add actions. 
 * \param [in] group_id - Id of the group 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_group_action(struct of_msg *msg,
                     uint32_t  group_id);

/** \ingroup Action_Utilities 
 * \brief Set Queue Action\n
 * <b>Description </b>\n                                                               
 *  Pushes set queue action to message buffer. This action map the flow entry to
 *  queue of flow entry. This will set irrespective of packet ToS and VLAN PCP bits. 
 * \param [in] msg      - Message descriptor with buffer to add actions. 
 * \param [in] queue_id - Id of the queue to use for the packet. 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_queue_action(struct of_msg *msg,
                          uint32_t  queue_id);

/** \ingroup Action_Utilities 
 * \brief Set MPLS TTL Action\n
 * <b>Description </b>\n                                                               
 *  Pushes set_mpls_ttl_action to message buffer. This action sets the mpls ttl value.
 * \param [in] msg      - Message descriptor with buffer to add actions. 
 * \param [in] mpls_ttl - MPLS TTL value to set. 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_mpls_ttl_action(struct of_msg *msg,
                             uint8_t  mpls_ttl);

/** \ingroup Action_Utilities 
 * \brief Decrement MPLS TTL Action\n
 * <b>Description </b>\n                                                               
 *   Pushes decrement mpls ttl action to message buffer. This action will decrement
 *   mpls ttl value.
 * \param [in] msg      - Message descriptor with buffer to add actions. 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_dec_mpls_ttl_action(struct of_msg *msg);

/** \ingroup Action_Utilities 
 * \brief Set IPv4 TTL Action\n
 * <b>Description </b>\n                                                               
 *  Pushes Set IPv4 TTL Action to message buffer. This action sets the ttl value
 *  in the IPV4 header.
 * \param [in] msg      - Message descriptor with buffer to add actions. 
 * \param [in] ipv4_ttl - IPV4 TTL value to set. 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_ipv4_ttl_action(struct of_msg *msg,
                             uint8_t  ipv4_ttl);
/** \ingroup Action_Utilities 
 * \brief Decrement IPv4 TTL Action\n
 * <b>Description </b>\n                                                               
 *  Pushes decrement ipv4 ttl action to message buffer. This action will decrement
 *  ttl value in IPV4, if present.
 * \param [in] msg - Message descriptor with buffer to add actions. 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_dec_ipv4_ttl_action(struct of_msg *msg);

/** \ingroup Action_Utilities 
 * \brief Copy TTL Outward Action\n
 * <b>Description </b>\n      
 *   Pushes copy ttl outward action to message buffer.The action copies the TTL 
 *   from the next-to-outermost header with TTL to the outermost header with TTL.
 * \param [in] msg - Message descriptor with buffer to add actions. 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_copy_ttl_outward_action(struct of_msg *msg);

/** \ingroup Action_Utilities 
 * \brief Copy TTL inward Action\n
 * <b>Description </b>\n      
 *   Pushes copy ttl inward action to message buffer. The action copies the TTL from
 *   the outermost header with TTL to the next-to-outermost header with TTL.
 * \param [in] msg - Message descriptor with buffer to add actions. 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_copy_ttl_inward_action(struct of_msg *msg);

/** \ingroup Action_Utilities 
 * \brief Push VLAN Header Action\n
 * <b>Description </b>\n      
 *   Pushes 'push VLAN header' action to message buffer. It is used to push new VLAN
 *   tag.
 * \param [in] msg        - Message descriptor with buffer to add actions. 
 * \param [in] ether_type - Ether type of the new tag.
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_push_vlan_header_action(struct of_msg *msg,
                                uint16_t      ether_type);

/** \ingroup Action_Utilities 
 * \brief Pop VLAN Header Action\n
 * <b>Description </b>\n      
 *   Pushes 'pop vlan header' action to message buffer. 
 * \param [in] msg        - Message descriptor with buffer to add actions. 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_pop_vlan_header_action(struct of_msg *msg);

/** \ingroup Action_Utilities 
 * \brief Push pbb Header Action\n
 * <b>Description </b>\n      
 *   Pushes 'push pbb header' action to message buffer. It is used to push new pbb
 *   service tag.
 * \param [in] msg        - Message descriptor with buffer to add actions. 
 * \param [in] ether_type - Ether type of the new tag.
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_push_pbb_header_action(struct of_msg *msg,
                                uint16_t      ether_type);

/** \ingroup Action_Utilities 
 * \brief Pop pbb Header Action\n
 * <b>Description </b>\n      
 *   Pushes 'pop pbb header' action to message buffer.
 * \param [in] msg        - Message descriptor with buffer to add actions. 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_pop_pbb_header_action(struct of_msg *msg);

/** \ingroup Action_Utilities 
 * \brief Push MPLS Header Action\n
 * <b>Description </b>\n      
 *   Pushes 'push MPLS header' action to message buffer. It is used to push new MPLS
 *   header.
 * \param [in] msg        - Message descriptor with buffer to add actions. 
 * \param [in] ether_type - Ether type of the new tag.
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_push_mpls_header_action(struct of_msg *msg,
                                uint16_t      ether_type);

/** \ingroup Action_Utilities 
 * \brief Pop MPLS Header Action\n
 * <b>Description </b>\n      
 *   Pushes 'pop MPLS header' action to message buffer. 
 * \param [in] msg        - Message descriptor with buffer to add actions. 
 * \param [in] ether_type - Ether type of the new tag. 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_pop_mpls_header_action(struct of_msg *msg,
                                uint16_t      ether_type);

/* Following APIs pushes Set Field actions for different fields
 * Parameters:
 *   Input 
 *    -Message descriptor with buffer to add actions.
 *    -Field value that need to in set field action
 */ 

/** \ingroup Action_Utilities 
 * \brief Set Field 'Source MAC' Action\n
 * \param [in] msg             - Message descriptor with buffer to add actions. 
 * \param [in] source_mac_addr - Source MAC Address value to set. 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_source_mac_in_set_field_action(struct of_msg *msg,
                                            uint8_t *source_mac_addr); 

/** \ingroup Action_Utilities 
 * \brief Set Field 'In Physical Port' Action\n
 * \param [in] msg             - Message descriptor with buffer to add actions. 
 * \param [in] in_phy_port_no  - In Physical Port value to set. 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_in_physical_port_in_set_field_action(struct    of_msg *msg,
               uint32_t *in_phy_port_no);

/** \ingroup Action_Utilities 
 * \brief Set Field 'In Port' Action\n
 * \param [in] msg             - Message descriptor with buffer to add actions. 
 * \param [in] in_port_number  - In Port value to set. 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_port_in_set_field_action(struct    of_msg *msg,
               uint32_t *in_port_number);

/** \ingroup Action_Utilities 
 * \brief Set Field 'Logical Port Metadata' Action\n
 * \param [in] msg             - Message descriptor with buffer to add actions. 
 * \param [in] tunnel_id       - Tunnel id value to set. 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t 
ofu_push_set_logical_port_meta_data_in_set_field_action(struct of_msg *msg,
                              uint64_t      *tunnel_id);

/** \ingroup Action_Utilities 
 * \brief Set Field 'Destination MAC Address' Action\n
 * \param [in] msg           - Message descriptor with buffer to add actions. 
 * \param [in] dest_mac_addr - Destination MAC Address value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_destination_mac_in_set_field_action(struct of_msg *msg,
                                            uchar8_t *dest_mac_addr); 

/** \ingroup Action_Utilities 
 * \brief Set Field 'Ethernet Frame types' Action\n
 * \param [in] msg            - Message descriptor with buffer to add actions. 
 * \param [in] eth_frame_type - Ethernet Frame type value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_eth_type_in_set_field_action(struct of_msg *msg,
                                          uint16_t      *eth_frame_type);

/** \ingroup Action_Utilities 
 * \brief Set Field 'VLAN Id' Action\n
 * \param [in] msg     - Message descriptor with buffer to add actions. 
 * \param [in] vlan_id - VLAN Id value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_vlan_id_in_set_field_action(struct of_msg *msg,
                                         uint16_t      *vlan_id);

/** \ingroup Action_Utilities 
 * \brief Set Field 'VLAN Priority' Action\n
 * \param [in] msg                - Message descriptor with buffer to add actions. 
 * \param [in] vlan_priority_bits - VLAN Priority values to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_vlan_priority_bits_in_set_field_action(struct of_msg *msg,
                                                    uint8_t       *vlan_priority_bits);

/** \ingroup Action_Utilities 
 * \brief Set Field 'IP DSCP bits' Action\n
 * \param [in] msg          - Message descriptor with buffer to add actions. 
 * \param [in] ip_dscp_bits - IP DSCP Bits to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_ip_dscp_bits_in_set_field_action(struct of_msg *msg,
                                              uint8_t      *ip_dscp_bits);

/** \ingroup Action_Utilities 
 * \brief Set Field 'IP ECN' Action\n
 * \param [in] msg         - Message descriptor with buffer to add actions. 
 * \param [in] ip_ecn_bits - IP ECN Bits to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_ip_ecn_bits_in_set_field_action(struct of_msg *msg,
                                              uint8_t      *ip_ecn_bits);

/** \ingroup Action_Utilities 
 * \brief Set Field 'IP Protocol' Action\n
 * \param [in] msg         - Message descriptor with buffer to add actions. 
 * \param [in] ip_protocol - IP Protocol value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_ip_protocol_in_set_field_action(struct of_msg *msg,
                                             uint8_t      *ip_protocol);

/** \ingroup Action_Utilities 
 * \brief Set Field 'IPv4 Source Address' Action\n
 * \param [in] msg       - Message descriptor with buffer to add actions. 
 * \param [in] ipv4_addr - IPv4 Source Address value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_ipv4_src_addr_in_set_field_action(struct of_msg *msg,
                                               ipv4_addr_t   *ipv4_addr);

/** \ingroup Action_Utilities 
 * \brief Set Field 'IPv4 Destination Address' Action\n
 * \param [in] msg       - Message descriptor with buffer to add actions. 
 * \param [in] ipv4_addr - IPv4 Destination Address to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_ipv4_dst_addr_in_set_field_action(struct of_msg *msg,
                                               ipv4_addr_t   *ipv4_addr);

/** \ingroup Action_Utilities 
 * \brief Set Field 'Source TCP Port' Action\n
 * \param [in] msg         - Message descriptor with buffer to add actions. 
 * \param [in] port_number - Source TCP Port value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_tcp_src_port_in_set_field_action(struct of_msg *msg,
                                              uint16_t  *port_number);

/** \ingroup Action_Utilities 
 * \brief Set Field 'Destination TCP Port' Action\n
 * \param [in] msg         - Message descriptor with buffer to add actions. 
 * \param [in] port_number - Destination TCP Port value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_tcp_dst_port_in_set_field_action(struct of_msg *msg,
                                              uint16_t  *port_number);

/** \ingroup Action_Utilities 
 * \brief Set Field 'Source UDP Port' Action\n
 * \param [in] msg         - Message descriptor with buffer to add actions. 
 * \param [in] port_number - Source UDP Port value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_udp_src_port_in_set_field_action(struct of_msg *msg,
                                              uint16_t  *port_number);

/** \ingroup Action_Utilities 
 * \brief Set Field 'Destination UDP Port' Action\n
 * \param [in] msg         - Message descriptor with buffer to add actions. 
 * \param [in] port_number - Destination UDP Port value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_udp_dst_port_in_set_field_action(struct of_msg *msg,
                                              uint16_t  *port_number);

/** \ingroup Action_Utilities 
 * \brief Set Field 'SCTP Source Port' Action\n
 * \param [in] msg         - Message descriptor with buffer to add actions. 
 * \param [in] port_number - SCTP Source Port value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_sctp_src_port_in_set_field_action(struct of_msg *msg,
                                              uint16_t  *port_number);

/** \ingroup Action_Utilities 
 * \brief Set Field 'Destination SCTP Port' Action\n
 * \param [in] msg         - Message descriptor with buffer to add actions. 
 * \param [in] port_number - Destination SCTP Port value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_sctp_dst_port_in_set_field_action(struct of_msg *msg,
                                              uint16_t  *port_number);

/** \ingroup Action_Utilities 
 * \brief Set Field 'ICMPv4 Type' Action\n
 * \param [in] msg         - Message descriptor with buffer to add actions. 
 * \param [in] icmpv4_type - ICMPv4 Type value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_icmpv4_type_in_set_field_action(struct of_msg *msg,
                                              uint8_t  *icmpv4_type);

/** \ingroup Action_Utilities 
 * \brief Set Field 'ICMPv4 code' Action\n
 * \param [in] msg         - Message descriptor with buffer to add actions. 
 * \param [in] icmpv4_code - ICMPv4 code value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_icmpv4_code_in_set_field_action(struct of_msg *msg,
                                              uint8_t  *icmpv4_code);

/** \ingroup Action_Utilities 
 * \brief Set Field 'ARP OP Code' Action\n
 * \param [in] msg        - Message descriptor with buffer to add actions. 
 * \param [in] arp_opcode - ARP OP Code value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_arp_opcode_in_set_field_action(struct of_msg *msg,
                                            uint16_t  *arp_opcode);

/** \ingroup Action_Utilities 
 * \brief Set Field 'ARP Source IPv4 Address' Action\n
 * \param [in] msg       - Message descriptor with buffer to add actions. 
 * \param [in] ipv4_addr - ARP Source IPv4 Address value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_arp_source_ipv4_addr_in_set_field_action(struct of_msg *msg,
                                                      ipv4_addr_t  *ipv4_addr);

/** \ingroup Action_Utilities 
 * \brief Set Field 'ARP Target IPv4 Address' Action\n
 * \param [in] msg       - Message descriptor with buffer to add actions. 
 * \param [in] ipv4_addr - ARP Target IPv4 Address value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_arp_target_ipv4_addr_in_set_field_action(struct of_msg *msg,
                                            ipv4_addr_t  *ipv4_addr);

/** \ingroup Action_Utilities 
 * \brief Set Field 'ARP Source HW Address' Action\n
 * \param [in] msg     - Message descriptor with buffer to add actions. 
 * \param [in] hw_addr - ARP Source Hardware Address value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_arp_source_hw_addr_in_set_field_action(struct of_msg *msg,
                                                    uint8_t  *hw_addr); /* Caller need to pass 6 bytes of mac addr*/

/** \ingroup Action_Utilities 
 * \brief Set Field 'ARP Destination Hardware Address' Action\n
 * \param [in] msg      - Message descriptor with buffer to add actions. 
 * \param [in] hw_addr  - ARP Destination Hardware Address value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_arp_target_hw_addr_in_set_field_action(struct of_msg *msg,
                                                    uint8_t  *hw_addr); /* Caller need to pass 6 bytes of mac addr*/
/** \ingroup Action_Utilities 
 * \brief Set Field 'Source IPv6 Address' Action\n
 * \param [in] msg       - Message descriptor with buffer to add actions. 
 * \param [in] ipv6_addr - IPv6 Source Address value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_source_ipv6_addr_in_set_field_action(struct of_msg *msg,
                                                  ipv6_addr_t   *ipv6_addr); 

/** \ingroup Action_Utilities 
 * \brief Set Field 'Destination IPv6 Address' Action\n
 * \param [in] msg       - Message descriptor with buffer to add actions. 
 * \param [in] ipv6_addr - IPv6 Destination Address value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_destination_ipv6_addr_in_set_field_action(struct of_msg *msg,
                                                       ipv6_addr_t   *ipv6_addr); 

/** \ingroup Action_Utilities 
 * \brief Set Field 'IPv6 Flow Label' Action\n
 * \param [in] msg        - Message descriptor with buffer to add actions. 
 * \param [in] flow_label - IPv6 Flow Label value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_ipv6_flow_label_in_set_field_action(struct of_msg *msg,
                                                 uint32_t  *flow_label); 

/** \ingroup Action_Utilities 
 * \brief Set Field 'ICMPv6 type' Action\n
 * \param [in] msg         - Message descriptor with buffer to add actions. 
 * \param [in] icmpv6_type - ICMPv6 Type value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_icmpv6_type_in_set_field_action(struct of_msg *msg,
                                              uint8_t  *icmpv6_type);
/** \ingroup Action_Utilities 
 * \brief Set Field 'ICMPv6 code' Action\n
 * \param [in] msg         - Message descriptor with buffer to add actions. 
 * \param [in] icmpv6_code - ICMPv6  Code value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_icmpv6_code_in_set_field_action(struct of_msg *msg,
                                              uint8_t  *icmpv6_code);

/** \ingroup Action_Utilities 
 * \brief Set Field 'ND Target IPv6 Address' Action\n
 * \param [in] msg       - Message descriptor with buffer to add actions. 
 * \param [in] ipv6_addr - ND Target IPv6 Address value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_nd_target_ipv6_addr_in_set_field_action(struct of_msg *msg,
                                                       ipv6_addr_t *ipv6_addr); 

/** \ingroup Action_Utilities 
 * \brief Set Field 'ND Target Link Layer Address' Action\n
 * \param [in] msg     - Message descriptor with buffer to add actions. 
 * \param [in] hw_addr - ND Target Link layer Address value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_nd_target_link_layer_addr_in_set_field_action(struct of_msg *msg,
                                                           uint8_t  *hw_addr); /* Caller need to pass 6 bytes of mac addr*/

/** \ingroup Action_Utilities 
 * \brief Set Field 'ND Source Link Layer addr' Action\n
 * \param [in] msg     - Message descriptor with buffer to add actions. 
 * \param [in] hw_addr - ND source Link layer Address value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_nd_source_link_layer_addr_in_set_field_action(struct of_msg *msg,
                                                           uint8_t  *hw_addr); /* Caller need to pass 6 bytes of mac addr*/

/** \ingroup Action_Utilities 
 * \brief Set Field 'MPLS Label' Action\n
 * \param [in] msg        - Message descriptor with buffer to add actions. 
 * \param [in] mpls_label - MPLS label value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_mpls_label_in_set_field_action(struct of_msg *msg,
                                            uint32_t  *mpls_label);

/** \ingroup Action_Utilities 
 * \brief Set Field 'MPLS TC' Action\n
 * \param [in] msg    - Message descriptor with buffer to add actions. 
 * \param [in] mpls_tc - MPLS TC value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_mpls_tc_in_set_field_action(struct of_msg *msg,
                                         uint8_t  *mpls_tc);

/** \ingroup Action_Utilities 
 * \brief Set Field 'MPLS BoS' Action\n
 * \param [in] msg           - Message descriptor with buffer to add actions. 
 * \param [in] mpls_bos_bits - MPLS BoS value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_mpls_bos_bits_in_set_field_action(struct of_msg *msg,
                                         uint8_t  *mpls_bos_bits);

/** \ingroup Action_Utilities 
 * \brief Set Field 'PBB I-SID' Action\n
 * \param [in] msg      - Message descriptor with buffer to add actions. 
 * \param [in] pbb_isid - PBB I-SID value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_pbb_isid_in_set_field_action(struct of_msg *msg,
                                         uint8_t  *pbb_isid); /* 3 bytes I-SID of first PBB service flag*/

/** \ingroup Action_Utilities 
 * \brief Set Field 'IPv6 Extended header Pseudo' Action\n
 * \param [in] msg                       - Message descriptor with buffer to add actions. 
 * \param [in] ipv6_exthdr_pseudo_fields - IPv6 Extended header Pseudo value to set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_ACTION_PUSH_SUCCESS
 */
int32_t
ofu_push_set_ipv6_exthdr_field_in_set_field_action(struct of_msg *msg,
                                         uint16_t  *ipv6_exthdr_pseudo_fields);

/** \ingroup Action_Utilities 
 * \brief End Actions\n
 * <b>Description </b>\n                                                               
 * This is to indicate the completion of pushing the actions. Applications must issue this call 
 * after pushing all actions. The output parameters may be used by applications for debugging purpose.    
 * \param [in] msg                - Message descriptor with buffer that contains added actions 
 * \param [out] starting_location - Pointer to starting location of the action pushed in the message\n
 *                                  buffer
 * \param [out] length_of_actions - Total length of actions pushed in the buffer
 */
void 
ofu_end_pushing_actions(struct of_msg *msg,
                        uint8_t *starting_location, 
                        uint16_t *length_of_actions);

/** \ingroup Match_Fields 
 * \brief Start Match field setting\n 
 * <b>Description</b>\n
 * Initialize message descriptor values to set fields and masks if required. The 
 * applications  MUST issue this call before start setting the different matches
 * fields.    
 * Different Utility APIs defined here are used to set match fields and mask values 
 * if supported, Applications pass mask values, if the boolean value 'is_mask_set' 
 * that indicates whether mask value passed or not.
 * \param [in] msg - Message descriptor with buffer to set fields in the message buffer 
 */
void 
ofu_start_setting_match_field_values(struct of_msg *msg);


/** \ingroup Match_Fields 
 * \brief Set Field 'In Port'
 * \param [in] msg            - Message descriptor with buffer to set match fields. 
 * \param [in] in_port_number - Port number on packet receive to which In Port field set 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_in_port(struct    of_msg *msg,
               uint32_t *in_port_number );

/** \ingroup Match_Fields 
 * \brief Set Field 'In Phy Port'
 * \param [in] msg            - Message descriptor with buffer to set match fields. 
 * \param [in] in_phy_port_no - Physical Port number on packet receive to which In Physical Port field set\n
 *                              This is required only if ' In Port' is logical port. 
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_in_physical_port(struct of_msg *msg,
                        uint32_t      *in_phy_port_no );

/** \ingroup Match_Fields 
 * \brief Set Field 'meta data'. Set meta data values passed between tables. 
 * \param [in] msg            - Message descriptor with buffer to set match fields. 
 * \param [in] meta_data      - Meta data value to set 
 * \param [in] is_mask_set    - Boolean flag, TRUE indicates mask is set and valid 'meta_data_mask' is passed  
 * \param [in] meta_data_mask - Meta Data mask value, this is valid if 'is_mask_set' value is TRUE.  
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_meta_data(struct of_msg *msg,
                 uint64_t      *meta_data,
                 uint8_t        is_mask_set,
                 uint64_t      *meta_data_mask);

/** \ingroup Match_Fields 
 * \brief Set Field 'Source MAC Addr'
 * \param [in] msg            - Message descriptor with buffer to set match fields. 
 * \param [in] mac_addr       - Source MAC Address value to set.
 * \param [in] is_mask_set    - Boolean flag, TRUE indicates mask is set and valid 'mac_mask_value' is passed  
 * \param [in] mac_mask_value - Source MAC addr mask value, this is valid if 'is_mask_set' value is TRUE.  
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_source_mac(struct of_msg *msg,
                  uint8_t *mac_addr,
                  uint8_t  is_mask_set,
                  uint8_t  *mac_mask_value);

/** \ingroup Match_Fields 
 * \brief Set Field 'Destination MAC Addr'
 * \param [in] msg            - Message descriptor with buffer to set match fields. 
 * \param [in] mac_addr       - Destination MAC Address value to set.
 * \param [in] is_mask_set    - Boolean flag, TRUE indicates mask is set and valid 'mac_mask_value' is passed  
 * \param [in] mac_mask_value - Destination MAC addr mask value, this is valid if 'is_mask_set' value is TRUE.  
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_destination_mac(struct of_msg *msg,
                      uint8_t *mac_addr,
                      uint8_t  is_mask_set,
                      uint8_t  *mac_mask_value);

/** \ingroup Match_Fields 
 * \brief Set Field 'Ether type'
 * \param [in] msg        - Message descriptor with buffer to set match fields. 
 * \param [in] ether_type - Ethernet type field to set
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_ether_type(struct of_msg *msg,
                  uint16_t        *ether_type);

/** \ingroup Match_Fields 
 * \brief Set Field 'VLAN ID'
 * \param [in] msg     - Message descriptor with buffer to set match fields. 
 * \param [in] vlan_id - VLAN Id value to set
 * \param [in] is_mask_set    - Boolean flag, TRUE indicates mask is set and valid 'vlan_id_mask' is passed  
 * \param [in] vlan_id_mask   - VLAN ID mask value, this is valid if 'is_mask_set' value is TRUE.  
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_vlan_id(struct of_msg *msg,
               uint16_t      *vlan_id,
               uint8_t        is_mask_set,
               uint16_t      *vlan_id_mask);

/** \ingroup Match_Fields 
 * \brief Set VLAN Priority field values.
 * \param [in] msg           - Message descriptor with buffer to set match fields. 
 * \param [in] vlan_pcp_bits - VLAN Priority values to set
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_vlan_priority_bits(struct of_msg *msg,
                          uint8_t       *vlan_pcp_bits );

/** \ingroup Match_Fields 
 * \brief Set IP DSCP bits field values.
 * \param [in] msg       - Message descriptor with buffer to set match fields. 
 * \param [in] dscp_bits - IP DSCP Bit values to set
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_ip_dscp_bits(struct of_msg *msg,
                    uint8_t       *dscp_bits );

/** \ingroup Match_Fields 
 * \brief Set IP ecn bits  values.
 * \param [in] msg         - Message descriptor with buffer to set match fields. 
 * \param [in] ip_ecn_bits - IP ecn bits to set
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_ip_ecn_bits(struct of_msg *msg,
                   uint8_t       *ip_ecn_bits);

/** \ingroup Match_Fields 
 * \brief Set  IP Protocol value.
 * \param [in] msg         - Message descriptor with buffer to set match fields. 
 * \param [in] ip_protocol - IP Protocol value to set
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_protocol(struct of_msg *msg,
                uint8_t *ip_protocol );

/** \ingroup Match_Fields 
 * \brief Set IPv4 source address field. 
 * \param [in] msg              - Message descriptor with buffer to set match fields. 
 * \param [in] ipv4_addr        - IPv4 address value to set as source IP
 * \param [in] is_mask_set      - Boolean flag, TRUE indicates mask is set and valid 'ipv4_addr_mask' is passed  
 * \param [in] ipv4_addr_mask   - Source IPv4 address mask value, this is valid if 'is_mask_set' value is TRUE.  
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_ipv4_source(struct of_msg *msg,
                   ipv4_addr_t   *ipv4_addr,
                   uint8_t        is_mask_set,
                   ipv4_addr_t   *ipv4_addr_mask);

/** \ingroup Match_Fields 
 * \brief Set IPv4 Destination address field. 
 * \param [in] msg              - Message descriptor with buffer to set match fields. 
 * \param [in] ipv4_addr        - IPv4 address value to set as destination IP\n
 * \param [in] is_mask_set      - Boolean flag, TRUE indicates mask is set and valid 'ipv4_addr_mask' is passed  
 * \param [in] ipv4_addr_mask   - Destination IPv4 address mask value, this is valid if 'is_mask_set' value is TRUE.  
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_ipv4_destination(struct of_msg *msg,
                        ipv4_addr_t   *ipv4_addr,
                        uint8_t        is_mask_set,
                        ipv4_addr_t   *ipv4_addr_mask);

/** \ingroup Match_Fields 
 * \brief Set UDP Source Port field value. 
 * \param [in] msg         - Message descriptor with buffer to set match fields. 
 * \param [in] port_number - Source UDP Port value to set
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_udp_source_port(struct of_msg *msg,
                       uint16_t *port_number );

/** \ingroup Match_Fields 
 * \brief Set UDP Destination Port field value. 
 * \param [in] msg         - Message descriptor with buffer to set match fields. 
 * \param [in] port_number - Destination UDP Port value to set
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_udp_destination_port(struct of_msg *msg,
                            uint16_t *port_number );

/** \ingroup Match_Fields 
 * \brief Set TCP Source Port field value. 
 * \param [in] msg         - Message descriptor with buffer to set match fields. 
 * \param [in] port_number - Source TCP Port value to set
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_tcp_source_port(struct of_msg *msg,
                       uint16_t *port_number );

/** \ingroup Match_Fields 
 * \brief Set TCP Destination Port field value. 
 * \param [in] msg         - Message descriptor with buffer to set match fields. 
 * \param [in] port_number - Destination TCP Port value to set
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_tcp_destination_port(struct of_msg *msg,
                            uint16_t *port_number );

/** \ingroup Match_Fields 
 * \brief Set SCTP Source Port field value. 
 * \param [in] msg         - Message descriptor with buffer to set match fields. 
 * \param [in] port_number - Source SCTP Port value to set
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_sctp_source_port(struct of_msg *msg,
                       uint16_t *port_number );

/** \ingroup Match_Fields 
 * \brief Set SCTP Destination Port field value. 
 * \param [in] msg         - Message descriptor with buffer to set match fields. 
 * \param [in] port_number - Destination SCTP Port value to set
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_sctp_destination_port(struct of_msg *msg,
                   uint16_t *port_number );

/** \ingroup Match_Fields 
 * \brief Set ICMPv4 type field value. 
 * \param [in] msg         - Message descriptor with buffer to set match fields. 
 * \param [in] icmpv4_type - ICMPv4 type field value to set
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_icmpv4_type(struct of_msg *msg,
                   uint8_t *icmpv4_type );

/** \ingroup Match_Fields 
 * \brief Set ICMPv4 code field value. 
 * \param [in] msg         - Message descriptor with buffer to set match fields. 
 * \param [in] icmpv4_code - ICMPv4 code value to set
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_icmpv4_code(struct of_msg *msg,
                   uint8_t *icmpv4_code );

/** \ingroup Match_Fields 
 * \brief Set ARP OP Code field value. 
 * \param [in] msg         - Message descriptor with buffer to set match fields. 
 * \param [in] arp_opcode  - ARP OP Code value to set.
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_arp_op_code(struct    of_msg *msg,
                   uint16_t *arp_opcode );

/** \ingroup Match_Fields 
 * \brief Set ARP IPv4 Source address field. 
 * \param [in] msg              - Message descriptor with buffer to set match fields. 
 * \param [in] ipv4_addr        - IPv4 address value to set as ARP source IP
 * \param [in] is_mask_set      - Boolean flag, TRUE indicates mask is set and valid 'ipv4_addr_mask' is passed  
 * \param [in] ipv4_addr_mask   - ARP Source IPv4 address mask value, this is valid if 'is_mask_set' value is TRUE.  
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_arp_source_ip_addr(struct of_msg *msg,
                          ipv4_addr_t   *ipv4_addr,
                          uint8_t        is_mask_set,
                          ipv4_addr_t   *ipv4_addr_mask);

/** \ingroup Match_Fields 
 * \brief Set ARP IPv4 target address field. 
 * \param [in] msg              - Message descriptor with buffer to set match fields. 
 * \param [in] ipv4_addr        - IPv4 address value to set as ARP target IP
 * \param [in] is_mask_set      - Boolean flag, TRUE indicates mask is set and valid 'ipv4_addr_mask' is passed  
 * \param [in] ipv4_addr_mask   - ARP target IPv4 address mask value, this is valid if 'is_mask_set' value is TRUE.  
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_arp_target_ip_addr(struct of_msg      *msg,
                               ipv4_addr_t   *ipv4_addr,
                               uint8_t        is_mask_set,
                               ipv4_addr_t   *ipv4_addr_mask);

/** \ingroup Match_Fields 
 * \brief Set ARP Source hardware address field. 
 * \param [in] msg              - Message descriptor with buffer to set match fields. 
 * \param [in] hw_addr          - ARP Source mac address value to set
 * \param [in] is_mask_set      - Boolean flag, TRUE indicates mask is set and valid 'hw_addr_mask' is passed  
 * \param [in] hw_addr_mask     - ARP source Mac address mask value, this is valid if 'is_mask_set' value is TRUE.  
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_arp_source_hw_addr(struct of_msg *msg,
                          uint8_t       hw_addr[6], /* Caller is expected to pass 6 bytes of mac*/
                          uint8_t       is_mask_set,
                          uint8_t       hw_addr_mask[6]);

/** \ingroup Match_Fields 
 * \brief Set ARP Target hardware address field. 
 * \param [in] msg              - Message descriptor with buffer to set match fields. 
 * \param [in] hw_addr          - ARP Target mac address value to set
 * \param [in] is_mask_set      - Boolean flag, TRUE indicates mask is set and valid 'hw_addr_mask' is passed  
 * \param [in] hw_addr_mask     - ARP Target Mac address mask value, this is valid if 'is_mask_set' value is TRUE.  
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_arp_target_hw_addr(struct of_msg *msg,
                          uint8_t        hw_addr[6], /* Caller is expected to pass 6 bytes of mac*/
                          uint8_t        is_mask_set,
                          uint8_t        hw_addr_mask[6]);

/** \ingroup Match_Fields 
 * \brief Set IPv6 Source Address field. 
 * \param [in] msg              - Message descriptor with buffer to set match fields. 
 * \param [in] ipv6_addr        - IPv6 Source address value to set
 * \param [in] is_mask_set      - Boolean flag, TRUE indicates mask is set and valid 'ipv6_addr_mask' is passed  
 * \param [in] ipv6_addr_mask   - IPv6 source address mask value, this is valid if 'is_mask_set' value is TRUE.  
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_ipv6_source(struct of_msg *msg,
                   ipv6_addr_t   *ipv6_addr,
                   uint8_t        is_mask_set,
                   ipv6_addr_t   *ipv6_addr_mask);

/** \ingroup Match_Fields 
 * \brief Set IPv6 Destination Address field. 
 * \param [in] msg              - Message descriptor with buffer to set match fields. 
 * \param [in] ipv6_addr        - IPv6 Destination address value to set
 * \param [in] is_mask_set      - Boolean flag, TRUE indicates mask is set and valid 'ipv6_addr_mask' is passed  
 * \param [in] ipv6_addr_mask   - IPv6 destination address mask value, this is valid if 'is_mask_set' value is TRUE.  
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_ipv6_destination(struct of_msg *msg,
                        ipv6_addr_t   *ipv6_addr,
                        uint8_t        is_mask_set,
                        ipv6_addr_t   *ipv6_addr_mask);

/** \ingroup Match_Fields 
 * \brief Set IPv6 Flow Label field. 
 * \param [in] msg                  - Message descriptor with buffer to set match fields. 
 * \param [in] ipv6_flow_label      - IPv6 flow label field value to set\n
 * \param [in] is_mask_set          - Boolean flag, TRUE indicates mask is set and valid 'v6_flow_label_mask' is passed  
 * \param [in] v6_flow_label_mask   - flow label field IPv6 address mask value, this is valid if 'is_mask_set' value is TRUE.  
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_ipv6_flow_label(struct of_msg *msg,
                       uint32_t      *ipv6_flow_label,
                       uint8_t       is_mask_set,
                       uint32_t      *v6_flow_label_mask);

/** \ingroup Match_Fields 
 * \brief Set ICMPv6 type field value. 
 * \param [in] msg         - Message descriptor with buffer to set match fields. 
 * \param [in] icmpv6_type - ICMPv6 type field value to set
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_icmpv6_type(struct of_msg *msg,
                   uint8_t *icmpv6_type );

/** \ingroup Match_Fields 
 * \brief Set ICMPv6 code field value. 
 * \param [in] msg         - Message descriptor with buffer to set match fields. 
 * \param [in] icmpv6_code - ICMPv6 code field value to set
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_icmpv6_code(struct of_msg *msg,
                   uint8_t *icmpv6_code );

/** \ingroup Match_Fields 
 * \brief Set ND Target address field value. 
 * \param [in] msg                 - Message descriptor with buffer to set match fields. 
 * \param [in] nd_ipv6_target_addr - ND IPv6 target address field value to set
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_ipv6_nd_target(struct of_msg *msg,
                      ipv6_addr_t   *nd_ipv6_target_addr );

/** \ingroup Match_Fields 
 * \brief Set ND source hardware address field value. 
 * \param [in] msg    - Message descriptor with buffer to set match fields. 
 * \param [in] hw_addr - ND source hardware address field value to set
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_ipv6_nd_source_hw_addr(struct of_msg *msg,
                              uint8_t *hw_addr); /*Caller is expected to pass 6 bytes*/

/** \ingroup Match_Fields 
 * \brief Set ND target hardware address field value. 
 * \param [in] msg    - Message descriptor with buffer to set match fields. 
 * \param [in] hw_addr - ND target hardware address field value to set
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_ipv6_nd_target_hw_addr(struct of_msg *msg,
                             uint8_t *hw_addr); /*Caller is expected to pass 6 bytes*/

/** \ingroup Match_Fields 
 * \brief Set MPLS Label field value. 
 * \param [in] msg    - Message descriptor with buffer to set match fields. 
 * \param [in] mpls_label_val - MPLS Label value value to set
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_mpls_label(struct of_msg *msg,
                  uint32_t *mpls_label_val);

/** \ingroup Match_Fields 
 * \brief Set MPLS TC field value. 
 * \param [in] msg         - Message descriptor with buffer to set match fields. 
 * \param [in] mpls_tc_val - MPLS TC value to set
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_mpls_tc(struct of_msg *msg,
               uint8_t *mpls_tc_val);

/** \ingroup Match_Fields 
 * \brief Set MPLS BoS bits. 
 * \param [in] msg          - Message descriptor with buffer to set match fields. 
 * \param [in] mpls_bos_val - MPLS BoS bits to set
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_mpls_bos(struct of_msg *msg,
               uint8_t *mpls_bos_val);

/** \ingroup Match_Fields 
 * \brief Set IPv6 Flow Label field. 
 * \param [in] msg              - Message descriptor with buffer to set match fields. 
 * \param [in] pbb_i_sid        - PBB I-SID field value to set\n
 * \param [in] is_mask_set      - Boolean flag, TRUE indicates mask is set and valid 'pbb_i_sid_mask' is passed  
 * \param [in] pbb_i_sid_mask   - PBB I-SID mask value, this is valid if 'is_mask_set' value is TRUE.  
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_pbb_i_sid(struct of_msg *msg,
                 uint8_t      *pbb_i_sid, /* 3 bytes I-ISID in first PBB service flag*/
                 uint8_t       is_mask_set,
                 uint8_t      *pbb_i_sid_mask);

/** \ingroup Match_Fields 
 * \brief Set logical ports meta data value. 
 * \param [in] msg              - Message descriptor with buffer to set match fields. 
 * \param [in] tunnel_id        - tunnel ID to set logical port meta data. 
 * \param [in] is_mask_set      - Boolean flag, TRUE indicates mask is set and valid 'tunnel_id_mask' is passed  
 * \param [in] tunnel_id_mask   - PBB I-SID mask value, this is valid if 'is_mask_set' value is TRUE.  
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_logical_port_meta_data(struct of_msg *msg,
                              uint64_t      *tunnel_id,
                              uint8_t       is_mask_set,
                              uint64_t      *tunnel_id_mask);

/** \ingroup Match_Fields 
 * \brief Set IPv6 Extension header pseudo field 
 * \param [in] msg          - Message descriptor with buffer to set match fields. 
 * \param [in] ipv6_ext_hdr_pseudo_fd - IPv6 extension header pseudo field
 * \param [in] is_mask_set   - Boolean flag, TRUE indicates mask is set and valid\n
 *                            'ipv6_ext_hdr_pseudo_fd_mask' is passed  
 * \param [in] ipv6_ext_hdr_pseudo_fd_mask - Extension header pseudo field value, this is valid\n
 *                                            if 'is_mask_set' value is TRUE.  
 * \return OFU_NO_ROOM_IN_BUFFER 
 * \return OFU_SET_FIELD_SUCCESS
 */
int32_t
of_set_ipv6_extension_hdr(struct of_msg *msg,
                          uint16_t      *ipv6_ext_hdr_pseudo_fd,
                          uint8_t       is_mask_set,
                          uint16_t      *ipv6_ext_hdr_pseudo_fd_mask);

/** \ingroup Match_Fields 
 * \brief End Setting of Match Fields \n 
 * <b>Description</b>\n                                                                
 * This is to indicate the completion of setting of all the fields. The applications\n
 * must issue this call after setting all the fields and before actual send message API.\n
 * The output parameters may be used by applications for debugging purpose.    
 * \param [in] msg - Message descriptor with buffer that contains match field values 
 * \param [out] starting_location - pointer to starting location of the match fields in the message
 * \param [out] length_of_match_fields - Total length of match fields in the buffer
 */
void 
ofu_end_setting_match_field_values(struct of_msg *msg,
                                    uint8_t *starting_location, 
                                    uint16_t *length_of_match_fields);

/*As part of packet_in message, along with receiving packet from data_path,
 * ON Director also receives match field context contains one or more fields
 * whose values cannot be determined from the packet data. The same
 * match field context will be passed to applications that registered for
 * packet_in message. 
 *
 * Following utility functions are provided to get field details for the applications
 */


/** \ingroup Match_Fields 
 * \brief In_Port field set or not\n 
 * <b>Description</b>\n
 * Utility function to know  whether 'in_port' field set in the match field context\n
 * or not, it returns boolean value, TRUE indicates 'in_port' field available in\n
 * the match field context. If the field is set, application may call\n
 * ofu_get_in_port_field().  
 * \param [in] msg              - Message descriptor with buffer contain Open Flow message\n
 *                                received from data_path. Controller allocates memory for this.\n
 *                                Applications, after its usage, need to call free API available\n
 *                                within message descriptor. \n
 * \param [in] match_field_cntxt- Pointer to match field context contains details of fields set. This\n
 *                                is same as 'match_fields'  value passed to application's callback \n
 *                                during handing over of the  packet as part of packet_in message.\n
 * \return TRUE  - If In_port field set in the context.
 * \return FALSE - If In_port field not set in the context
 */
uint8_t
ofu_is_in_port_field_set(struct of_msg *msg,
                         void          *match_field_cntxt);

/** \ingroup Match_Fields 
 * \brief Get "In Port" field value\n 
 * <b>Description</b>\n
 * \param [in] msg               - Message descriptor with buffer containing Open Flow message\n
 *                                 received from data_path. Controller allocates memory for this.\n
 *                                 Applications, after its usage, need to call free API available\n
 *                                 within message descriptor.\n
 * \param [in] match_fields      - Pointer to 'match_fields' value passed to application's callback 
 *                                 during handing over of the packet as part of packet_in message.
 * \param [out] match_fields_len - Length of total match fields.
 * \param [out] in_phy_port      - In Port field value available in the fields context.
 * \return 
 */
int32_t
ofu_get_in_port_field(struct of_msg *msg,
                          void *match_fields,
                          uint16_t match_fields_len,
                          uint32_t *in_phy_port);
int32_t
ofu_get_metadata_field(struct of_msg *msg,
                    void *match_fields,
                    uint16_t match_fields_len,
                    uint64_t *metadata);

/** \ingroup Match_Fields 
 * \brief In_Phy_Port field set or not\n 
 * <b>Description</b>\n
 * Utility function to know  whether 'in_phy_port' field set in the match field context\n
 * or not, it returns boolean value, TRUE indicates 'in_phy_port' field available in\n
 * the match field context. If the field is set, application may call\n
 * ofu_get_in_phy_port_field().  
 * \param [in] msg              - Message descriptor with buffer contain Open Flow message\n
 *                                received from data_path. Controller allocates memory for this.\n
 *                                Applications, after its usage, need to call free API available\n
 *                                within message descriptor. \n
 * \param [in] match_field_cntx - Pointer to match field context contains details of fields set. 
 * \return TRUE  - If in_phy_port field set in the context.
 * \return FALSE - If in_phy_port field not set in the context
 */
uint8_t
ofu_is_in_phy_port_field_set(struct of_msg *msg,
                             void          *match_field_cntx);

/** \ingroup Match_Fields 
 * \brief Get in_phy_port field value \n 
 * <b>Description</b>\n
 * \param [in] msg              - Message descriptor with buffer contain Open Flow message\n
 *                                received from data_path. Controller allocates memory for this.\n
 *                                Applications, after its usage, need to call free API available\n
 *                                within message descriptor.\n
 * \param [in] match_field_cntx - Pointer to match field context contains details of fields set. 
 * \param [out] in_phy_port     - In Phy Port field value available in the fields context.
 * \return OFU_UNSUPPORTED_MATCH_FIELDS_TYPE_PASSED - Match field type passed is not supported by
 *                                                    ON Director.
 * \return OFU_IN_PORT_FIELD_PRESENT - Indicates In-Port field is present and the In_Port field 
 *                                     value be passed as output parameter of 'in_phy_port'
 * \return OFU_IN_PORT_FIELD_NOT_PRESENT -Indicates In-Port field is not present and ignored
 *                                        'in_phy_port' output parameter value.
 */
int32_t
ofu_get_in_phy_port_field(struct of_msg *msg,
                          void *match_field_cntx,
                          uint32_t *in_phy_port);

/** \ingroup Match_Fields 
 * \brief Meta Data field set or not\n 
 * <b>Description</b>\n
 * Utility function to know  whether 'meta data' field set in the match field context\n
 * or not, it returns boolean value, TRUE indicates 'meta data' field available in\n
 * the match field context. If the field is set, application may call\n
 * ofu_get_meta_data_field().  
 * \param [in] msg              - Message descriptor with buffer contain Open Flow message\n
 *                                received from data_path. Controller allocates memory for this.\n
 *                                Applications, after its usage, need to call free API available\n
 *                                within message descriptor. \n
 * \param [in] match_field_cntx - Pointer to match field context contains details of fields set. 
 * \return TRUE  - If meta data field set in the context.
 * \return FALSE - If meta data field not set in the context
 */
uint8_t
ofu_is_meta_data_field_set(struct of_msg *msg,
                           void *match_field_cntx);

/** \ingroup Match_Fields 
 * \brief Get meta data field value \n 
 * <b>Description</b>\n
 * \param [in] msg              - Message descriptor with buffer contain Open Flow message\n
 *                                received from data_path. Controller allocates memory for this.\n
 *                                Applications, after its usage, need to call free API available\n
 *                                within message descriptor.\n
 * \param [in] match_field_cntx - Pointer to match field context contains details of fields set. 
 * \param [out] meta_data       - meta data field value available in the fields context.
 */
void
ofu_get_meta_data_field(struct of_msg *msg,
                        void* match_field_cntx,
                        uint64_t* meta_data);

/** \ingroup Match_Fields 
 * \brief Logical port Tunnel ID field set or not\n 
 * <b>Description</b>\n
 * Utility function to know  whether 'Tunnel ID' field set in the match field context\n
 * or not, it returns boolean value, TRUE indicates 'Tunnel ID' field available in\n
 * the match field context. If the field is set, application may call\n
 * ofu_get_tunnel_id_field().  
 * \param [in] msg              - Message descriptor with buffer contain Open Flow message\n
 *                                received from data_path. Controller allocates memory for this.\n
 *                                Applications, after its usage, need to call free API available\n
 *                                within message descriptor. \n
 * \param [in] match_field_cntx - Pointer to match field context contains details of fields set. 
 * \return TRUE  - If Tunnel ID field set in the context.
 * \return FALSE - If Tunnel ID field not set in the context
 */
uint8_t
ofu_is_tunnel_id_field_set(struct of_msg *msg,
                           void *match_field_cntx);


/** \ingroup Match_Fields 
 * \brief Get Logical Port Tunnel Id field value \n 
 * <b>Description</b>\n
 * \param [in] msg              - Message descriptor with buffer contain Open Flow message\n
 *                                received from data_path. Controller allocates memory for this.\n
 *                                Applications, after its usage, need to call free API available\n
 *                                within message descriptor.\n
 * \param [in] match_field_cntx - Pointer to match field context contains details of fields set. 
 * \param [out] tunnel_id       - Tunnel Id data field value available in the fields context.
 */
void
ofu_get_tunnel_id_field(struct of_msg *msg,
                        void *match_field_cntx,
                        uint64_t *tunnel_id);

/** \ingroup Group_Utilities 
 *  OFU_GROUP_ACTION_BUCKET_LEN
 *  - Returns length of buffer required for action bucket 
 */
#define OFU_GROUP_ACTION_BUCKET_LEN     sizeof(struct ofp_bucket)

/** \ingroup Group_Utilities 
 *  OFU_ADD_OR_MODIFY_OR_DELETE_GROUP_MESSAGE
 *  - Returns length of buffer required to add/modify/delete group message
 */
#define OFU_ADD_OR_MODIFY_OR_DELETE_GROUP_MESSAGE sizeof(struct ofp_group_mod)

/** \ingroup Group_Utilities 
 * \brief Start Appending Buckets\n
 * <b>Description </b>\n      
 * Utility function to initialize message descriptor values. Applications \n
 * must issue this call before appending multiple action buckets to group. 
 * One must append buckets of same group type. That is applications should not mix groups
 * of one type with another.
 *
 * \note After each call to append bucket, applications must call series of action utility APIs\n
 * to push necessary actions to that bucket before calling APIs to append next bucket.
 *
 * \param [in]  msg - Message descriptor with buffer to append buckets in the message buffer 
 */
void 
ofu_start_appending_buckets_to_group(struct of_msg *msg);

/** \ingroup Group_Utilities 
 * \brief Append Bucket to group type All\n
 * <b>Description </b>\n      
 * Append bucket to group type all. All the buckets in the group must be of
 * same group type All.\n
 *
 * After this call application should push necessary actions by using utility API\n
 * before calling API to append next bucket of same type.
 * Before pushing action, caller should not call start pushing action API \n
 * ofu_start_pushing_actions().
 * \param [in] msg - Message descriptor with buffer used to append bucket with actions 
 * \return OFU_INVALID_GROUP_TYPE
 * \return OFU_APPEND_BUCKET_SUCCESS
 */
int32_t
ofu_append_bucket_to_group_type_all(struct of_msg *msg);

/** \ingroup Group_Utilities 
 * \brief Append Bucket to group type Select\n
 * <b>Description </b>\n      
 * Append bucket to group type Select. All the buckets in the group must be of
 * same group type Select.\n
 *
 * After this call application should push necessary actions by using utility API\n
 * before calling API to append next bucket of same type.
 * Before pushing action, caller should not call start pushing action API \n
 * ofu_start_pushing_actions().
 * \param [in] msg    - Message descriptor with buffer used to append buckets with actions 
 * \param [in] weight - Weight of the bucket
 * \return OFU_INVALID_GROUP_TYPE
 * \return OFU_APPEND_BUCKET_SUCCESS
 */
int32_t
ofu_append_bucket_to_group_type_select(struct of_msg *msg,
                                       uint16_t weight);

/** \ingroup Group_Utilities 
 * \brief Append Bucket to group type "Fast fail over"\n
 * <b>Description </b>\n      
 * Append bucket to group type Fast Fail over. All the buckets in the group must be of
 * same group type Fast Fail over.\n
 *
 * After this call application should push necessary actions by using utility API\n
 * before calling API to append next bucket of same type.
 * Before pushing action, caller should not call start pushing action API \n
 * ofu_start_pushing_actions().
 * \param [in] msg         - Message descriptor with buffer used to append buckets with actions\n 
 * \param [in] watch_port  - If specified, system performs the liveness check of the port to know whether\n
 *                           this bucket can be candidate for forwarding packet.
 * \param [in] watch_group - If specified, system performs the liveness check of the group to know whether\n
 *                           this bucket can be candidate for forwarding packet.
 * \return OFU_INVALID_GROUP_TYPE
 * \return OFU_APPEND_BUCKET_SUCCESS
 */
int32_t
ofu_append_bucket_to_group_type_fast_failover(struct of_msg *msg,
                                              uint32_t watch_port,
                                              uint32_t watch_group);

/** \ingroup Group_Utilities 
 * \brief End of appending all buckets\n
 * <b>Description </b>\n      
 * This is to indicate the completion of appending all buckets. Applications must issue this call 
 * after appending necessary buckets. The output parameters may be used by applications for debugging purpose.    
 *
 * \param [in]  msg               - Message with descriptor and buffer that contains required buckets with actions. 
 * \param [out] starting_location - Starting location of bucket which contains bucket with actions.
 * \param [out] length_of_bucket  - Total length of buckets.
 */
void
ofu_end_appending_buckets_to_group(struct of_msg *msg,
                                   uint8_t *starting_location,
                                   uint16_t *length_of_bucket);

/** \ingroup Metering_Utilities 
 *  OFU_METER_BAND_LEN
 *  - Returns length of buffer required for Meter Bands 
 */
#define OFU_METER_BAND_LEN     sizeof(struct ofp_meter_band_dscp_remark)

/** \ingroup Metering_Utilities 
 *  OFU_ADD_OR_MODIFY_OR_DELETE_METER_MESSAGE
 *  - Returns length of buffer required for the add/modify/delete meter message 
 */
#define OFU_ADD_OR_MODIFY_OR_DELETE_METER_MESSAGE sizeof(struct ofp_meter_mod)


/** \ingroup Metering_Utilities 
 * \brief Start Appending Bands to Meter\n
 * \param [in] msg           - Message with descriptor and buffer allocated for\n
 *                             addition or modification of meter configuration values.
 */
void ofu_start_appending_bands_to_meter(struct of_msg *msg);

/** \ingroup Metering_Utilities 
 * \brief End Appending Bands to Meter\n
 * \param [in] msg                - Message with descriptor and buffer allocated for\n
 *                                  addition or modification of meter configuration values.
 * \param [out] starting_location - Pointer to the starting location of the band.
 * \param [out] length_of_band    - Total length of the band.
 */
void ofu_end_appending_bands_to_meter(struct of_msg *msg,
      uint8_t *starting_location,
      uint16_t *length_of_band);

/** \ingroup Metering_Utilities 
 * \brief Set Meter configuration flags\n
 * <b>Description </b>\n      
 * API to set meter configuration flag values. Before add or modify the meter\n 
 * to the datapath, this utility function is used to set configuration\n
 * flag for meter that need to add or a modify meter
 * \param [in] msg           - Message with descriptor and buffer allocated earlier for\n
 *                             addition or modification meter configuration values.
 * \param [in] rate_type     - Type of rate to measure in terms of packets per second or\n
 *                             kilo_bits per second.
 *                             Either one of following value need to be passed\n
 *                             OFPMF_KBPS  - If rate of meter is calculated w.r.t. bytes\n
 *                             OFPMF_PKTPS - If rate of meter is calculated w.r.t. packets\n
 * \param [in] include_burst - Boolean value, TRUE indicates include of bursts in rate limit\n
 *                             calculation.
 * \return OFU_INVALID_METER_RATE_TYPE
 * \return OFU_INVALID_METER_PARAM_VALUE
 * \return OFU_METER_SET_CONFIG_SUCCESS
 */
int32_t
ofu_set_meter_config_flag(struct of_msg *msg,
                          uint8_t        rate_type,
                          uint8_t        include_burst);

/** \ingroup Metering_Utilities 
 * \brief Add band of type OFMPMT_DROP\n
 * <b>Description </b>\n      
 * API to add bands of type OFPMBT_DROP to meter. In this type packets whose rate
 * exceed the band_rate are dropped. \n
 * 
 * Before add or modify meter operation in the datapath, this utility function is\n
 * used to add one or more meter bands of type OFMPMT_DROP in the message \n
 * \param [in] msg         - Message descriptor with buffer allocated earlier for addition or
 *                           modification meter configuration values.
 * \param [in] band_rate   - Rate of the band. It is in terms kilo_bits per second, if meter\n
 *                           configuration rate_type is set as OFPMF_KBPS. Otherwise it will be
 *                           in terms of packets per sec, if meter rate_type set as OFPMF_PKTPS.
 * \param [in] burst_value - It will be applicable, if include_burst of meter configuration set to TRUE.\n
 *                           It indicates the length of the packet or byte burst to consider for applying\n
 *                           the meter packet, if rate_type is OFPMF_PKTPS. Otherwise it will be kilobits, if\n
 *                           rate_type is OFPMF_KBPS.
 * \return OFU_INVALID_METER_PARAM_VALUE
 * \return OFU_ADD_METER_BAND_SUCCESS
 */
int32_t ofu_append_band_to_meter_type_drop(struct of_msg *msg,
                                uint32_t       band_rate,
                                uint32_t       burst_value);


/** \ingroup Metering_Utilities 
 * \brief Add meter band of type OFPMBT_DSCP_REMARK\n
 * <b>Description </b>\n      
 * API to add bands of type OFPMBT_DSCP_REMARK to meter. It remarks the drop precedence of the\n
 * DSCP field in the IP header of the packets that exceed the band rate value. It basically\n
 * decrease the drop precedence of the DSCP field in the IP header of the packet. It can be used\n
 * to define a simple DiffServ policy.
 * Before add or modify the meter operation in the datapath, this utility function is used to add one\n
 * or more meter bands of type OFPMBT_DSCP_REMARK.
 * \param [in] msg         - Message descriptor with buffer allocated earlier for addition or
 *                           modification meter configuration values.
 * \param [in] band_rate   - Rate of the band. It is in terms of kilo_bits per second, if meter\n
 *                           configuration rate_type is set as OFPMF_KBPS. Otherwise it will be
 *                           in terms of packets per sec, if meter rate_type set as OFPMF_PKTPS.
 * \param [in] burst_value - It will be applicable, if include_burst of meter configuration is set to TRUE.\n
 *                           It indicates the length of the packet or byte burst to consider for applying\n
 *                           the meter packet, if rate_type is OFPMF_PKTPS. Otherwise it will be kilobits, if\n
 *                           rate_type is OFPMF_KBPS.
 * \param [in] prec_level  - Number of precedence level to subtract.
 * \return OFU_INVALID_METER_PARAM_VALUE
 * \return OFU_ADD_METER_BAND_SUCCESS
 */
int32_t ofu_append_band_to_meter_type_dscp_remark(struct of_msg *msg,
                                        uint32_t       band_rate,
                                        uint8_t        prec_level,
                                        uint32_t       burst_value);

/** \ingroup Port_Description 
 * \brief Set Port Admin Config Flag values\n
 * <b>Description </b>\n      
 * Set flag values that indicates administrative status of the status. Application should\n
 * call this utility function before actually calling port modification API, in case of any \n
 * port admin status indicated here need to change.
 *
 * \param [in] msg        -  Message descriptor with buffer allocated earlier for modification
 *                           of port configuration values.
 * \param [in] admin_down -  Boolean flag, TRUE means brings port administratively down
 * \param [in] recv_drop  -  Boolean flag, TRUE means drop all packets received by the port
 * \param [in] no_fwd     -  Boolean flag, TRUE means drop sending packets to the port 
 * \param [in] no_dp_pkts -  Boolean flag,TRUE means not to send exception packets(Packet_In messages)
 *                           from this port to ON Director.
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_PORT_ADMIN_CONFIG_CHANGE_SUCCESS
 */
int32_t
ofu_set_port_admin_config_flag(struct of_msg *msg,
                               uint8_t admin_down,
                               uint8_t recv_drop,
                               uint8_t no_fwd,
                               uint8_t no_dp_pkts);

/* Set port features, fallowing set of utility function are used
 * to configure port features
 * One ore more fallowing required utility functions are called before 
 * calling API to modify port configuration. 
 */

/** \ingroup Port_Description 
 * \brief Set Port speed as 10MB Half Duplex\n
 * \param [in] msg    Message descriptor with buffer allocated earlier for modification of port speed.
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_PORT_SPEED_CHANGE_SUCCESS
 */
int32_t
ofu_set_port_speed_as_10MB_half_duplex(struct of_msg *msg);

/** \ingroup Port_Description 
 * \brief Set Port speed as 10MB Full Duplex\n
 * \param [in] msg    Message descriptor with buffer allocated earlier for modification of port speed.
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_PORT_SPEED_CHANGE_SUCCESS
 */
int32_t
ofu_set_port_speed_as_10MB_full_duplex(struct of_msg *msg);

/** \ingroup Port_Description 
 * \brief Set Port speed as 100MB Half Duplex\n
 * \param [in] msg    Message descriptor with buffer allocated earlier for modification of port speed.
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_PORT_SPEED_CHANGE_SUCCESS
 */
int32_t
ofu_set_port_speed_as_100MB_half_duplex(struct of_msg *msg);

/** \ingroup Port_Description 
 * \brief Set Port speed as 100MB Full Duplex\n
 * \param [in] msg    Message descriptor with buffer allocated earlier for modification of port speed.
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_PORT_SPEED_CHANGE_SUCCESS
 */
int32_t
ofu_set_port_speed_as_100MB_full_duplex(struct of_msg *msg);

/** \ingroup Port_Description 
 * \brief Set Port speed as 1GB Half Duplex\n
 * \param [in] msg    Message descriptor with buffer allocated earlier for modification of port speed.
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_PORT_SPEED_CHANGE_SUCCESS
 */
int32_t
ofu_set_port_speed_as_1GB_half_duplex(struct of_msg *msg);

/** \ingroup Port_Description 
 * \brief Set Port speed as 1GB Full Duplex\n
 * \param [in] msg    Message descriptor with buffer allocated earlier for modification of port speed.
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_PORT_SPEED_CHANGE_SUCCESS
 */
int32_t
ofu_set_port_speed_as_1GB_full_duplex(struct of_msg *msg);

/** \ingroup Port_Description 
 * \brief Set Port speed as 10GB Full Duplex\n
 * \param [in] msg    Message descriptor with buffer allocated earlier for modification of port speed.
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_PORT_SPEED_CHANGE_SUCCESS
 */
int32_t
ofu_set_port_speed_as_10GB_full_duplex(struct of_msg *msg);

/** \ingroup Port_Description 
 * \brief Set Port speed as 40GB Full Duplex\n
 * \param [in] msg    Message descriptor with buffer allocated earlier for modification of port speed.
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_PORT_SPEED_CHANGE_SUCCESS
 */
int32_t
ofu_set_port_speed_as_40GB_full_duplex(struct of_msg *msg);

/** \ingroup Port_Description 
 * \brief Set Port speed as 100GB Full Duplex\n
 * \param [in] msg    Message descriptor with buffer allocated earlier for modification of port speed.
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_PORT_SPEED_CHANGE_SUCCESS
 */
int32_t
ofu_set_port_speed_as_100GB_full_duplex(struct of_msg *msg);

/** \ingroup Port_Description 
 * \brief Set Port speed as 1TB Full Duplex\n
 * \param [in] msg    Message descriptor with buffer allocated earlier for modification of port speed.
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_PORT_SPEED_CHANGE_SUCCESS
 */
int32_t
ofu_set_port_speed_as_1TB_full_duplex(struct of_msg *msg);

/** \ingroup Port_Description 
 * \brief Set Port medium as copper\n
 * \param [in] msg    Message descriptor with buffer allocated earlier for modification of port medium.
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_PORT_SPEED_CHANGE_SUCCESS
 */
int32_t
ofu_set_port_medium_as_copper(struct of_msg *msg);

/** \ingroup Port_Description 
 * \brief Set Port medium as fiber\n
 * \param [in] msg    Message descriptor with buffer allocated earlier for modification of port medium.
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_PORT_SPEED_CHANGE_SUCCESS
 */
int32_t
ofu_set_port_medium_as_fiber(struct of_msg *msg);

/** \ingroup Port_Description 
 * \brief Enable Auto negotiation feature\n
 * \param [in] msg    Message descriptor with buffer allocated earlier for port configuration. 
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_PORT_SPEED_CHANGE_SUCCESS
 */
int32_t
ofu_set_enable_auto_negotiation_to_port(struct of_msg *msg);

/** \ingroup Port_Description 
 * \brief Set link feature as pause\n
 * \param [in] msg    Message descriptor with buffer allocated earlier for port configuration
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_PORT_SPEED_CHANGE_SUCCESS
 */
int32_t
ofu_set_pause_to_port(struct of_msg *msg);

/** \ingroup Port_Description 
 * \brief Set link feature as Asymmetric pause\n
 * \param [in] msg    Message descriptor with buffer allocated earlier port configuration. 
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_PORT_SPEED_CHANGE_SUCCESS
 */
int32_t
ofu_set_asymmetric_pause_to_port(struct of_msg *msg);


struct ofl_oxm_match_fields {
   //    OFPXMT_OFB_IN_PORT        = 0;  /* Switch input port. */
   uint32_t    in_port;  /* sWITCH INPUT PORT. */
   //  OFPXMT_OFB_IN_PHY_PORT    = 1;  /* Switch physical input port. */
   uint32_t    in_phy_port;  /* sWITCH PHYSICAL INPUT PORT. */
   // OFPXMT_OFB_METADATA       = 2;  /* Metadata passed between tables. */
   uint64_t    metadata;  /* mETADATA PASSED BETWEEN TABLES. */
   //    OFPXMT_OFB_ETH_DST        = 3;  /* Ethernet destination address. */
   uint64_t    eth_dst;  /* eTHERNET DESTINATION ADDRESS. */
   //  OFPXMT_OFB_ETH_SRC        = 4;  /* Ethernet source address. */
   uint64_t    eth_src;  /* eTHERNET SOURCE ADDRESS. */
   //   OFPXMT_OFB_ETH_TYPE       = 5;  /* Ethernet frame type. */
   uint16_t    eth_type;  /* eTHERNET FRAME TYPE. */
   //   OFPXMT_OFB_VLAN_VID       = 6;  /* VLAN id. */
   uint16_t    vlan_vid;  /* vlan ID. */
   //   OFPXMT_OFB_VLAN_PCP       = 7;  /* VLAN priority. */
   uint8_t    vlan_pcp;  /* vlan PRIORITY. */
   //   OFPXMT_OFB_IP_DSCP        = 8;  /* IP DSCP (6 bits in ToS field). */
   uint8_t    ip_dscp;  /* ip dscp (6 BITS IN tOs FIELD). */
   //    OFPXMT_OFB_IP_ECN         = 9;  /* IP ECN (2 bits in ToS field). */ 
   uint8_t    ip_ecn ;  /* ip ecn (2 BITS IN tOs FIELD). */ 
   //   OFPXMT_OFB_IP_PROTO       = 10; /* IP protocol. */
   uint8_t    ip_proto; /* ip PROTOCOL. */
   //  OFPXMT_OFB_IPV4_SRC       = 11; /* IPv4 source address. */
   uint32_t    ipv4_src; /* ipV4 SOURCE ADDRESS. */
   //  OFPXMT_OFB_IPV4_DST       = 12; /* IPv4 destination address. */
   uint32_t    ipv4_dst; /* ipV4 DESTINATION ADDRESS. */
   //  OFPXMT_OFB_TCP_SRC        = 13; /* TCP source port. */
   uint32_t    tcp_src; /* tcp SOURCE PORT. */
   //  OFPXMT_OFB_TCP_DST        = 14; /* TCP destination port. */
   uint32_t    tcp_dst; /* tcp DESTINATION PORT. */
   //   OFPXMT_OFB_UDP_SRC        = 15; /* UDP source port. */
   uint32_t    udp_src; /* udp SOURCE PORT. */
   //   OFPXMT_OFB_UDP_DST        = 16; /* UDP destination port. */
   uint32_t    udp_dst; /* udp DESTINATION PORT. */
   //   OFPXMT_OFB_SCTP_SRC       = 17; /* SCTP source port. */
   uint32_t    sctp_src; /* sctp SOURCE PORT. */
   // OFPXMT_OFB_SCTP_DST       = 18; /* SCTP destination port. */
   uint32_t    sctp_dst; /* sctp DESTINATION PORT. */
   //   OFPXMT_OFB_ICMPV4_TYPE    = 19; /* ICMP type. */
   uint32_t    icmpv4_type; /* icmp TYPE. */
   // OFPXMT_OFB_ICMPV4_CODE    = 20; /* ICMP code. */
   uint32_t    icmpv4_code; /* icmp CODE. */
   //   OFPXMT_OFB_ARP_OP         = 21; /* ARP opcode. */
   uint16_t    arp_op ; /* arp OPCODE. */
   // OFPXMT_OFB_ARP_SPA        = 22; /* ARP source IPv4 address. */
   uint32_t    arp_spa; /* arp SOURCE ipV4 ADDRESS. */
   // OFPXMT_OFB_ARP_TPA        = 23; /* ARP target IPv4 address. */
   uint32_t    arp_tpa; /* arp TARGET ipV4 ADDRESS. */
   // OFPXMT_OFB_ARP_SHA        = 24; /* ARP source hardware address. */
   uint32_t    arp_sha; /* arp SOURCE HARDWARE ADDRESS. */
   // OFPXMT_OFB_ARP_THA        = 25; /* ARP target hardware address. */
   uint32_t    arp_tha; /* arp TARGET HARDWARE ADDRESS. */
   //  OFPXMT_OFB_IPV6_SRC       = 26; /* IPv6 source address. */
   uint32_t    ipv6_src; /* ipV6 SOURCE ADDRESS. */
   //  OFPXMT_OFB_IPV6_DST       = 27; /* IPv6 destination address. */
   uint32_t    ipv6_dst; /* ipV6 DESTINATION ADDRESS. */
   // OFPXMT_OFB_IPV6_FLABEL    = 28; /* IPv6 Flow Label */
   uint32_t    ipv6_flabel; /* ipV6 fLOW lABEL */
   //  OFPXMT_OFB_ICMPV6_TYPE    = 29; /* ICMPv6 type. */
   uint32_t    icmpv6_type; /* icmpV6 TYPE. */
   //  OFPXMT_OFB_ICMPV6_CODE    = 30; /* ICMPv6 code. */
   uint32_t    icmpv6_code; /* icmpV6 CODE. */
   // OFPXMT_OFB_IPV6_ND_TARGET = 31; /* Target address for ND. */
   uint32_t    ipv6_nd_target ; /* tARGET ADDRESS FOR nd. */
   // OFPXMT_OFB_IPV6_ND_SLL    = 32; /* Source link-layer for ND. */
   uint32_t    ipv6_nd_sll; /* sOURCE LINK-LAYER FOR nd. */
   // OFPXMT_OFB_IPV6_ND_TLL    = 33; /* Target link-layer for ND. */
   uint32_t    ipv6_nd_tll; /* tARGET LINK-LAYER FOR nd. */
   // OFPXMT_OFB_MPLS_LABEL     = 34; /* MPLS label. */
   uint32_t    mpls_label; /* mpls LABEL. */
   // OFPXMT_OFB_MPLS_TC        = 35; /* MPLS TC. */
   uint32_t    mpls_tc; /* mpls tc. */
   // OFPXMT_OFP_MPLS_BOS       = 36; /* MPLS BoS bit. */
   uint32_t    ofpxmt_ofp_mpls_bos; /* mpls bOs BIT. */
   //    OFPXMT_OFB_PBB_ISID       = 37; /* PBB I-SID. */
   uint32_t    pbb_isid; /* pbb i-sid. */
   //    OFPXMT_OFB_TUNNEL_ID      = 38; /* Logical Port Metadata. */
   uint32_t    tunnel_id ; /* lOGICAL pORT mETADATA. */
   //  OFPXMT_OFB_IPV6_EXTHDR    = 39; /* IPv6 Extension Header pseudo-field */
   uint32_t    ipv6_exthdr; /* ipV6 eXTENSION hEADER PSEUDO-FIELD */
   //  OFPXMT_OFB_MAX            = 40
};


#define OXM_GET_DATA_UINT8(a)  (*(uint8_t *)(a))      
#define OXM_GET_DATA_UINT16(a) (*(uint16_t *)(a))
#define OXM_GET_DATA_UINT32(a) (*(uint32_t *)(a))
#define OXM_GET_DATA_UINT64(a) (*(uint64_t *)(a))
#define OXM_GET_DATA_BYTE(a)   (*(uint8_t *)(a))      
#define OXM_GET_DATA_BYTES1(a)  OXM_GET_DATA_BYTE(a)
#define OXM_GET_DATA_BYTES2(a)  OXM_GET_DATA_UINT16(a)
#define OXM_GET_DATA_BYTES6(a)  ((OXM_GET_DATA_UINT32(a)<< 4 )| OXM_GET_DATA_UINT16(a+4))
#define OXM_GET_DATA_UINT6(a)  ((OXM_GET_DATA_UINT32(a)<< 4 )| OXM_GET_DATA_UINT16(a+4))

#endif /*_OF_UTILITY_H*/
