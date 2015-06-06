 /**
 * Copyright (c) 2012, 2013  Freescale.
 *  
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/

/*!\file of_multipart.h 
 * \author Rajesh Madabushi   
 * \author Srini Addepalli   
 * \date Jan, 2013  
 * <b>Description:</b>\n 
 * This file provide APIs related to formatting and sending 
 * Multipart messages.
 *
 * It also has prototypes of various utility functions that help in formatting
 * and sending of multipart messages.
 *
 * It also has prototypes to utility functions that parse the messages received 
 * from data_path.
 */

#ifndef __OF_MULTIPART_H
#define OF_MULTIPART_H

/************** Switch description get API *********************/

//!Get Switch Description APIs 
/*! This module contain APIs for formatting and sending get switch description\n
 *
 * Callback function pointer will be passed to get switch description API to 
 * receive the switch description details.
 *
 * Different utility APIs are provided to get details switch description values.
 *
 *\addtogroup Switch_Description 
 **/

/** \ingroup Switch_Description
 * OFU_SWITCH_DESCRIPTION_REQ_MESSAGE_LEN
 * - Maximum length of the message required to send switch description request 
 *   message
 **/
#define OFU_SWITCH_DESCRIPTION_REQ_MESSAGE_LEN   sizeof(struct ofp_multipart_request)

/** \ingroup Switch_Description
 * \struct ofi_dp_desc_info
 * \brief Switch Description Reply message\n
 * <b> Description </b>\n
 *  This Data structure holds the switch description.  All the fields must be  
 *  converted into host_byte order before calling the callback 
 *  of_switch_description_cbk_fn() to  handover the switch description reply 
 *  message.
 *
 *  Separate Utility functions are provided to get switch description values.
 */
struct ofi_dp_desc_info{

   /** Switch manufacturer description length.*/
   uint16_t mfr_len;

   /** Switch manufacturer description. Maximum length of string is DESC_STR_LEN(256) bytes.*/
   char  *mfr_desc;

   /** Switch hardware description length.*/
   uint16_t hw_len;

   /** Switch hardware description.  Maximum length of string is DESC_STR_LEN(256) bytes.*/
   char  *hw_desc;

   /** Switch software description length.*/
   uint16_t sw_len;

   /** Switch software description.  Maximum length of string is DESC_STR_LEN(256) bytes.*/
   char  *sw_desc;

   /** Serial number string length*/
   uint16_t serial_len;

   /** Serial number of the switch.  Maximum length of string is SERIAL_NUM_LEN(32) bytes.*/
   char  *serial_num;

   /** Datapath description string length.*/
   uint16_t dp_len;

   /** Human readable description of data_path.  Maximum length of string is DESC_STR_LEN(256) bytes.*/
   char  *dp_desc;
};

/** \ingroup Switch_Description
 * \typedef of_switch_description_cbk_fn 
 * \brief Callback function to receive switch description reply.
 *  <b>Description </b>\n
 *  Callback function to pass data_path switch description details. 
 *  This function is a response to earlier call to get data_path switch 
 *  description get API of_get_switch_description().\n
 * \param [in] controller_handle  -Handle of the controller.
 * \param [in] domain_handle      -Handle of the domain.
 * \param [in] datapath_handle    -Handle of the data_path.
 * \param [in] buf                -Message descriptor with buffer containing 
 *                                 data_path_description reply that is
 *                                 received from data_path. Controller 
 *                                 allocates the memory required to read the
 *                                 messasge received from data_path. After the
 *                                 application has done processing the response
 *                                 received, the free function available in the 
 *                                 message descriptor will be called to free 
 *                                 the memory. 
 * \param [in] cbk_arg1           -First Application specific private info that 
 *                                 is passed to the API to get switch 
 *                                 description request.
 * \param [in] cbk_arg2           -Second Application specific private info 
 *                                 that is passed to the API to get switch 
 *                                 description request.
 * \param [in] status             -Flag to indicate the status of response to 
 *                                 the earlier switch configuration request.
 * \param [in] dp_desc_info       -Details of the switch description received 
 *                                 from data_path.
 */
typedef void (*of_switch_description_cbk_fn)( uint64_t  controller_handle,
                                        uint64_t  domain_handle,
                                        uint64_t  datapath_handle,
                                        struct of_msg *buf,
                                        void     *cbk_arg1,
                                        void     *cbk_arg2,
                                        uint8_t   status,
                                        struct ofi_dp_desc_info *dp_desc_info);




/** \ingroup Switch_Description 
 * \brief Get switch description details.\n
 *  <b>Description </b>\n
 *  API to get switch description details.  Callback function 
 *  of_switch_description_cbk_fn is passed to get switch description.
 *  The macro OFU_SWITCH_DESCRIPTION_REQ_MESSAGE_LEN is used to calculate 
 *  the length of the message buffer required for switch description request 
 *  message.
 *  \param [in] msg             -Message descriptor allocated earlier to send 
 *                               message to data_path description.
 *  \param [in] datapath_handle -Handle of the data_path to which we are 
 *                               sending the get request. 
 *  \param [in] description_cbk -Callback function to pass data_path switch 
 *                               description.
 *  \param [in] cbk_arg1        -Application private information, arg1, that 
 *                               will be passed to error callback in case
 *                               of any errors. Also same passed to callback 
 *                               functions in case of success.   
 *  \param [in] cbk_arg2        -Application private information, arg2, that 
 *                               will be passed to error callback in case
 *                               of any errors. Also same passed to callback 
 *                               functions in case of success.   
 *  \param [out] xid            -Id assigned for the transaction. The same 
 *                               transaction Id will be passed to callback 
 *                               function. 
 *  \return OFU_INVALID_PARAM_PASSED
 *  \return OFU_SEND_SW_DESC_REQ_MSG_SUCCESS
 */
int32_t
of_get_switch_description ( struct of_msg *msg,
                        uint64_t datapath_handle,
                        of_switch_description_cbk_fn description_cbk, 
                        void *cbk_arg1,
                        void *cbk_arg2,
                        uint32_t *xid);

/** \ingroup Switch_Description 
 * \brief Return NULL terminated switch manufacturer description\n
 *  <b>Description </b>\n
 *  API to get switch sofware manufacutrer description details. 
 * \param [in] msg     -Message received from data_path contains switch 
 *                      description.  It was passed to callback 
 *                      of_switch_description_cbk_fn().\n
 * \param [in] dp_desc -Pointer to switch description, it was passed to 
 *                      callback of_switch_description_cbk_fn()\n  
 */
char*
ofu_get_sw_manufacturer_desc(struct of_msg *msg,
                             struct ofi_dp_desc_info *dp_desc);

/** \ingroup Switch_Description 
 * \brief Return NULL terminated switch hardware description\n
 *  <b>Description </b>\n
 *  API to get switch software and hardware description details.
 * \param [in] msg     -Message received from data_path contains switch 
 *                      description.  It was passed to callback 
 *                      of_switch_description_cbk_fn().\n
 * \param [in] dp_desc -Pointer to switch description, it was passed to 
 *                      callback of_switch_description_cbk_fn()\n  
 */
char*
ofu_get_sw_hardware_desc(struct of_msg *msg,
                         struct ofi_dp_desc_info *dp_desc);

/** \ingroup Switch_Description 
 * \brief Return NULL terminated switch software description\n
 *  <b>Description </b>\n
 *  API to get switch software description details.
 * \param [in] msg     -Message received from data_path contains switch 
 *                      description.  It was passed to callback 
 *                      of_switch_description_cbk_fn().\n
 * \param [in] dp_desc -Pointer to switch description.  It was passed to 
 *                      callback of_switch_description_cbk_fn()\n  
 */
char*
ofu_get_sw_software_desc(struct of_msg *msg,
                         struct ofi_dp_desc_info *dp_desc);

 /** \ingroup Switch_Description 
 * \brief Return NULL terminated switch serial number description\n
 *  <b>Description </b>\n
 *  API to get switch serial number description details.
 * \param [in] msg     -Message received from data_path contains switch 
 *                      description.  It was passed to callback 
 *                      of_switch_description_cbk_fn().\n
 * \param [in] dp_desc -Pointer to switch description, it was passed to 
 *                      callback of_switch_description_cbk_fn()\n  
 */
char*
ofu_get_serial_no_desc(struct of_msg *msg,
                       struct ofi_dp_desc_info *dp_desc);

 /** \ingroup Switch_Description 
 * \brief Return NULL terminated switch data_path description\n
 *  <b>Description </b>\n
 *  API to get data_path description details.
 * \param [in] msg     -Message received from data_path contains switch 
 *                      description.  It was passed to callback 
 *                      of_switch_description_cbk_fn().\n
 * \param [in] dp_desc -Pointer to switch description, it was passed to 
 *                      callback of_switch_description_cbk_fn()\n  
 */
char*
ofu_get_dp_desc(struct of_msg *msg,
               struct ofi_dp_desc_info *dp_desc);
 
/************** Flow Entry get API *********************/

//!Get Individual Flow Entry statistics  APIs 
/*! This module contains  APIs for formatting and sending to get\n
 * statistics of each individual flow entries.
 *
 * Callback function pointer will be passed to receive individual flow entries\n
 * statistics values. 
 *
 *\addtogroup Individual_Flow_Entries 
 */

/** \ingroup Individual_Flow_Entries
 * \struct ofi_flow_entry_info
 * \brief Individual Flow entry statistics values\n
 * <b> Description </b>\n 
 * Data_structure to hold flow entry details, all fields are converted 
 * into host byte order before calling callback of_flow_entry_stats_cbk_fn().
 */
struct ofi_flow_entry_info
{
    /** table Id */
    uint8_t table_id; 
    /** Priority level of flow entry */
    uint16_t priority;

    /**  flow that has been alive in seconds*/
    uint32_t alive_sec; 

    /**  flow that has been alive in nano seconds*/
    uint32_t alive_nsec; 

    /** Opaque value that issued by controller */
    uint64_t cookie;

    /** Idle timeout value in seconds from original flow entry. */
    uint16_t Idle_timeout;

    /** Hard timeout value in seconds from original flow entry. */
    uint16_t hard_timeout; /* Hard timeout from original flow mod. */

    /** Number of packets that hit the flow. */
    uint64_t packet_count;

    /** Byte counts of packet that hit the flow */
    uint64_t byte_count;
   
    /* list of instructions of type struct ofi_instruction*/
    of_list_t  instruction_list;  
    
    /* list of instructions of type struct ofi_match_field*/
    of_list_t  match_field_list;  
};

#define OFU_FLOW_MATCH_FIELD_LEN  46

/** \ingroup Individual_Flow_Entries
 * \struct ofi_match_field
 * \brief List of match fields.\n
 * <b> Description </b>\n 
 * List of match fields.
 */
struct ofi_match_field
{
  of_list_node_t   next_match_field;
  uint8_t       field_type;
  uint8_t       field_len;

  uint8_t       ui8_data;
  uint16_t      ui16_data;
  uint32_t      ui32_data;
  uint64_t      ui64_data;
  uint8_t       ui8_data_array[6];
  ipv6_addr_t   ipv6_addr;
  uint8_t       is_mask_set;
  uint8_t       mask[OFU_FLOW_MATCH_FIELD_LEN];
  uint8_t       mask_len;
  uint16_t      match_class;
};
#define OF_MATCH_LISTNODE_OFFSET offsetof(struct ofi_match_field, next_match_field)

/** \ingroup Individual_Flow_Entries
 *  \struct oxm_match_field
 *  \brief oxm_match_field\n
 */
struct oxm_match_field
{
  /** class of the match field */
  uint16_t      match_class;
  /** field type */
  uint8_t       field_type;
  /** length of the match field */
  uint8_t       length;
};


/** \ingroup Individual_Flow_Entries
 * \struct ofi_instruction
 * \brief List of instructions.\n
 * <b> Description </b>\n 
 * List of instructions of type struct ofi_action.
 */
struct ofi_instruction
{
   of_list_node_t     next_instruction;
   
   /** Type of instruction */
   uint16_t type;
   /** length of the instruction */
   uint16_t len;        
   /** Set next table in the lookup pipeline */
   uint8_t table_id;             
   /** Metadata value to write */
   uint64_t metadata;            
   /** Metadata write bitmask */
   uint64_t metadata_mask;       
   /**experimenter vendor id*/
   uint32_t vendorid;            
   /**experimenter sub type*/
   uint16_t subtype;             
   /**Current Packet out complete status, one of FSLXPS_* value OR priority_value*/
   uint16_t value1;                
   /**Group Identifier*/
   uint32_t group_id;                
   /** list of instructions of type struct ofi_action*/
   of_list_t  action_list;
};
#define OF_INSTRUCTION_LISTNODE_OFFSET offsetof(struct ofi_instruction, next_instruction)

/** \ingroup Individual_Flow_Entries
 * OFU_INDIVIUAL_FLOW_ENTRY_STATS_REQ_MESSAGE_LEN
 * - Returns length of message required to send get request for individual flow entries 
 */
#define OFU_INDIVIUAL_FLOW_ENTRY_STATS_REQ_MESSAGE_LEN  (sizeof(struct ofp_multipart_request) + sizeof(struct ofp_flow_stats_request))


/** \ingroup Individual_Flow_Entries
 * \typedef of_flow_entry_stats_cbk_fn 
 * \brief Callback function to receive Invividual flow entry statistics values 
 *  <b>Description </b>\n
 *  Callback function to pass statistics details of flow entry. This is in response\n
 *  to earlier call to get API of_get_flow_entries().\n
 *
 *  For a given get request, there is a possibility of matching more than one flow\n
 *  entry. So this callback will be called multiple times each time with one flow entry\n
 *  details. That is this callback always calls with one flow entry.\n
 *
 * \param [in] controller_handle  -Handle of the controller
 * \param [in] domain_handle      -Handle of the domain
 * \param [in] datapath_handle    -Handle of the data_path.
 * \param [in] table_id           - Id of the table to which  sending  request to get flow entries.\n
 * \param [in] msg                -Message descriptor with buffer contains Open Flow message holding flow_entries
 *                                 received from data_path. Controller allocates the memory required to read 
 *                                 flow entries received from data_path. Once application done with flow entries
 *                                 received, the  free function available in the message descriptor will be 
 *                                 called to free the  memory. 
 * \param [in] cbk_arg1           -First Application specific private info that passed in the API to
 *                                 get flow entries request.
 * \param [in] cbk_arg2           -Second Application specific private info that passed in the API to
 *                                 get flow entries request.
 * \param [in] status             -Flag indicated the status  of response to earlier flow entry request.
 * \param [in] flow_entry         -Pointer to flow entries available. All the data Must be converted into
 *                                  host byte order.
 * \param [in] more_entries       - Boolean value , TRUE indicates, this is not final response and more flow
 *                                  entries yet to come. FALSE means this is the last flow entry.
 *
 */
typedef void (*of_flow_entry_stats_cbk_fn)( uint64_t controller_handle,
                                            uint64_t domain_handle,
                                            uint64_t datapath_handle,
                                            uint8_t  table_Id,
                                            struct of_msg *msg,
                                            void  *cbk_arg1,
                                            void  *cbk_arg2,
                                            uint8_t status,
                                            struct ofi_flow_entry_info *flow_entry,
                                            uint8_t more_entries);

/** \ingroup Individual_Flow_Entries 
 * \brief Send get Flow entries request message.\n
 *  <b>Description </b>\n
 *   API to get individual flow entry  details for a given choice. It asynchronously 
 *  getbacks one or more flow entry details of given request.  Callback function 
 *  is registered to pass flow entries that fetched from data_path.  
 *
 *  This callback called multiple times with one flow entry for every callback.
 *
 *  The selection of flow entries in the data_path depends on different filter\n
 *  values passed as input parameters. All the necessary utility functions are \n 
 *  required to call before calling this API.
 * 
 *  The total length of message required to send flow entry get request is\n
 *  return value of OFU_INDIVIUAL_FLOW_ENTRY_STATS_REQ_MESSAGE_LEN              +\n
 *  Total length of all the match field passed as filter to search flow entries. \n
 *  \param [in] msg             - Message descriptor with buffer allocated earlier to send flow entry get request.\n
 *                                Utility functions are used to fill the match fields. These match fields are used\n
 *                                to match the flow entries in the data_path.\n
 *  \param [in] datapath_handle - Handle of the data_path to which we are sending the get request. \n
 *  \param [in] table_id        - Id of the table to which  sending  request to get flow entries.\n
 *  \param [in] out_port        - Optionaly restrict the search of flow entries based on output port.\n
 *                                OFPP_ANY indicates no restriction.\n
 *  \param [in] out_group       - Optionaly restrict the search of flow entries based on output group.\n
 *                                OFPG_ANY indicates no restriction.\n
 *  \param [in] cookie          - Cookie value to match flow  entries.\n
 *  \param [in] cookie_mask,    - Mask used to restrict the cookie bits that must match. A value of 0 \n
 *                                indicates no restriction.\n
 *  \param [in] flow_entry_cbk_fn - Callback function pointer to pass flow entries received from data_path.\n
 *  \param [in] cbk_arg1        - Application private information, arg1, that will be passed to error callback in case\n
 *                                of any errors. Also same passed to callback functions in case of success.   \n
 *  \param [in] cbk_arg2        - Application private information, arg2, that will be passed to error callback in case\n
 *                                of any errors. Also same passed to callback functions in case of success.   \n
 *  \return OFU_INVALID_PARAM_PASSED
 *  \return OFU_SEND_FLOW_ENTRY_REQ_MSG_SUCCESS
 */
int32_t
of_get_flow_entries(struct of_msg *msg, 
                    uint64_t datapath_handle,
                    uint8_t  table_id,
                    uint32_t out_port,
                    uint32_t out_group,
                    uint64_t cookie,
                    uint64_t cookie_mask,
                    of_flow_entry_stats_cbk_fn flow_entry_cbk_fn, 
                    void *cbk_arg1,
                    void *cbk_arg2);

/************** Aggregate flow Entry get API *********************/

//!Get Aggregate Flow Entry statistics  APIs 
/*! This module contains  APIs for formatting and sending to get\n
 * statistics of each aggregate flow entries.
 *
 * Provides APIs to get the aggregated statistics values of all the \n
 * flow entries that requested.
 *
 * Callback function pointer will be passed to receive aggregate flow entries\n
 * statistics values. 
 *
 *\addtogroup Aggregate_Flow_Entries 
 */

/** \ingroup Aggregate_Flow_Entries
 * OFU_AGGREGATE_FLOW_ENTRY_STATS_REQ_MESSAGE_LEN
 * - Returns length of message required to send get request for individual flow entries 
 */
#define OFU_AGGREGATE_FLOW_ENTRY_STATS_REQ_MESSAGE_LEN \
          (sizeof(struct ofp_multipart_request) + sizeof(struct ofp_aggregate_stats_request))

/** \ingroup Aggregate_Flow_Entries
 * \struct ofi_aggregate_flow_stats
 * \brief Aggregate Flow entry statistics values\n
 *  <b>Description </b>\n
 *  Data structure holding aggregate flow entry statistics, all fields are converted to\n
 *  host byte order before calling callback of_aggregate_stats_cbk_fn()
 */ 
struct ofi_aggregate_flow_stats
{
   /** Total number of packets in flows*/
   uint64_t packet_count; 

   /** Total number of bytes in flows*/ 
   uint64_t byte_count; 

   /** Total Number of flows*/ 
   uint32_t flow_count;
};

/** \ingroup Aggregate_Flow_Entries
 * \typedef of_aggregate_stats_cbk_fn 
 * \brief Callback function to receive Aggregate flow entry statistics values 
 *  <b>Description </b>\n
 *  Callback function to pass aggregated statistics of flow entries from data_path.\n 
 *  This is in response to earlier call to get API of_get_aggregate_flow_entries().
 * \param [in] controller_handle  - Handle of the controller
 * \param [in] domain_handle      - Handle of the domain
 * \param [in] datapath_handle    - Handle of the data_path.
 * \param [in] msg                - Message descriptor with buffer contains Open Flow message holding aggregation of
 *                                  flow_entries received from data_path. Controller allocates the memory required to read 
 *                                  flow entries received from data_path. Once application done with flow entries
 *                                  received, the  free function available in the message descriptor will be 
 *                                  called to free the  memory. 
 * \param [in] table_id           - Id of the table to which aggregated statistics received.
 * \param [in] cbk_arg1           - First Application specific private info that passed in the API to
 *                                  get aggregated statistics request
 * \param [in] cbk_arg2           - Second Application specific private info that passed in the API to
 *                                  get aggregated statistics request
 * \param [in] status             - Flag indicated the status  of response to earlier get aggregated statistics request.
 * \param [in] aggregation_of_flow_stats - Aggregation of flow entry statistics details.
 * \param [in] more_entries       -Boolean value , TRUE indicates, this is not final response and more flow
 *                                 entries yet to come. FALSE means this is the last flow entry.
 */
typedef void (*of_aggregate_stats_cbk_fn)(
                                       uint64_t  controller_handle,
                                           uint64_t domain_handle,
                                           uint64_t datapath_handle,
                                           uint8_t  table_Id,
                                           struct of_msg *msg,
                                           void  *cbk_arg1,
                                           void  *cbk_arg2,
                                           uint8_t status,
                                          struct ofi_aggregate_flow_stats *aggregation_of_flow_stats,
                                          uint8_t more_entries);
    
/** \ingroup Aggregate_Flow_Entries 
 * \brief Send aggregate flow entries request message.\n
 *  <b>Description </b>\n
 *  API to get aggregate values  or sum of all multiple flow entries of given choice.
 *  It asynchronously getbacks aggregate status values of one or more flow entries
 *  of given request. 
 *
 *  Callback function is registered to pass aggregate values of flow entries that fetched from data_path.  
 *
 *  The selection of flow entries in the data_path is depends on different filter 
 *  values passed as input parameters. 
 * 
 *  The total length of message required to send flow entry get request is\n
 *  return value of OFU_AGGREGATE_FLOW_ENTRY_STATS_REQ_MESSAGE_LEN              +\n
 *  Total length of all the match field passed as filter to search flow entries. \n
 *  \param [in] msg             - Message descriptor with buffer allocated earlier to send aggregated\n
 *                                flow entry get request. Utility functions are used to fill the match\n
 *                                fields. These match fields are used to match the flow entries in the data_path.\n
 *  \param [in] datapath_handle - Handle of the data_path to which we are sending the get request. 
 *  \param [in] table_id        - Id of the table to which  sending  request to get aggregation of flow entries.
 *  \param [in] out_port        - Optionaly restrict the search of flow entries based on output port.
 *                                OFPP_ANY indicates no restriction.
 *  \param [in] out_group       - Optionaly restrict the search of flow entries based on output group.
 *                                OFPG_ANY indicates no restriction.
 *  \param [in] cookie          - Cookie value to match flow  entries.
 *  \param [in] cookie_mask     - Mask used to restrict the cookie bits that must match. A value of 0 
 *                                indicates no restriction.
 *  \param [in] aggregat_stats_cbk_fn - Callback function pointer to pass aggregate of flow entries
 *                                      received from data_path.
 *  \param [in] cbk_arg1        - Application private information, arg1, that will be passed to error callback in case\n
 *                                of any errors. Also same passed to callback functions in case of success.   \n
 *  \param [in] cbk_arg2        - Application private information, arg2, that will be passed to error callback in case\n
 *                                of any errors. Also same passed to callback functions in case of success.   \n
 *  \param [out] xid            - Id assigned for the transaction. The same transaction Id will be passed to
 *                                callback function to pass aggregated statistics values. 
 */ 
 int32_t
 of_get_aggregate_flow_entries(struct of_msg *msg, 
                              uint64_t datapath_handle,
                              uint8_t  table_id,
                              uint32_t out_port,
                              uint32_t out_group,
                              uint64_t cookie,
                              uint64_t cookie_mask,
                              of_aggregate_stats_cbk_fn aggregat_stats_cbk_fn, 
                              void *cbk_arg1,
                              void *cbk_arg2);

/***************** Table Features API *********************/

//!Table Feature  APIs 
/*! This module contains  APIs related to data_path's table features. 
 * for formatting and sending to get\n
 *
 * APIs are provided to get and set table features.\n
 *
 * Callback function will be passed to get table feature API to receive table features.
 *
 * \sa Table_Feature_Utilities
 * \sa msg_mem_mgmt
 *\addtogroup Table_Features 
 */

/** \ingroup Table_Features
 * OFU_TABLE_FEATURES_REQ_MESSAGE_LEN
 * - Returns length of message required to send 'get table features message'. 
 */
#define OFU_TABLE_FEATURES_REQ_MESSAGE_LEN   sizeof(struct ofp_multipart_request)

/** \ingroup Table_Features
 * \struct ofi_table_features_info
 * \brief Table feature details\n
 *  <b>Description </b>\n
 * Data structure holding table feature details, all the fields must be converted into \n
 * host byte order before calling callback functions of_table_features_cbk_fn().\n
 *
 * Different Utility functions are provided to parse and get the different table feature\n
 * details.
 */
struct ofi_table_features_info {

      /** Name of the table */
      char      name[OFP_MAX_TABLE_NAME_LEN]; 

      /* Identifier of table.  Lower numbered tables are consulted first. */
      uint8_t table_id;        

      /** Bits of metadata that table can match */
      uint64_t  metadata_match; 

      /** Bits of metadata that table can write using Write Meta Data Instruction. */
      uint64_t  metadata_write; 

      /** Max number of entries that table supported. */
      uint32_t  max_entries; 

      /** Number of Instructions that supported for flow entry*/
      uint8_t   no_of_flow_inst;  

      /** Pointing to Flow entry instructions details of table features received*/
      void     *flow_instructions; 

      /** Pointing current Flow entry instruction used in get next api*/ 
      void     *flow_instruction_curr;

      /** current flow entry instruction position, used in get next api*/
      uint8_t   flow_inst_curr_no; 

      /** Number of Instructions supported for miss entry*/
      uint8_t   no_of_miss_inst; 
      
      /** Pointing to instructions of table features that used for miss entry*/
      void     *miss_entry_instructions;

      /** Internal helping variable used in get next api*/
      uint8_t   no_of_missinst_curr;  

      /** Internal helping variable used in get next api*/
      void     *miss_entry_instructions_curr; 

      /** Number of next tables Ids configured that starting from current table*/
      uint8_t  no_of_next_tbl;

      /** Pointer to Next table Ids list*/ 
      void    *next_tbl; 

      /** Internal helping variable used in get next api*/
      uint8_t  no_of_next_tbl_curr; 

      /** Internal helping variable used in get next api*/
      void    *next_tbl_curr; 
 
      /** Number of next tables Ids configured that starting from current table for the miss entry*/
      uint8_t  no_of_next_tbls_4_miss; 

      /** Pointer to list of next table Ids configured for the miss entry*/
      void    *next_tbl_4_flow_miss;

      /** Internal helping variable used in get next api*/
      uint8_t  no_of_next_tbls_4_miss_curr;  

      /** Internal helping variable used in get next api*/
      void    *next_tbl_4_flow_miss_curr;  

      /** Number of Write actions supported for  flow entry*/
      uint8_t no_of_write_actns; 

      /** Pointer to write actions list of flow entry*/
      void   *flow_entry_write_actns; 

      /** Internal helping variable used in get next api*/
      uint8_t no_of_write_actns_curr;  

      /** Internal helping variable used in get next api*/
      void   *flow_entry_write_actns_curr;  

      /** Number of write actions supported for flow miss entry*/
      uint8_t no_of_write_actns_4_miss; 

      /** Pointer to write actions list that used for the miss entry*/
      void   *miss_entry_write_actns;

      /** Internal helping variable used in get next api*/
      uint8_t no_of_write_actns_4_miss_curr;

      /** Internal helping variable used in get next api*/
      void   *miss_entry_write_actns_curr;  

      /** Number of apply actions supported for flow entry*/
      uint8_t no_of_apply_actns; 

      /** Pointer to list of apply actions supported by flow entry*/
      void   *flow_entry_apply_actns; 

      /** Internal helping variable used in get next api*/
      uint8_t no_of_apply_actns_curr;  

      /** Internal helping variable used in get next api*/
      void   *flow_entry_apply_actns_curr; 

      /** Number of apply actions supported for miss entries of flow*/
      uint8_t  no_of_apply_actns_4_miss; 

      /** Pointer to list apply actions supported by mins entries of flow*/
      void    *miss_entry_apply_actns; 

      /** Internal helping variable used in get next api*/
      uint8_t  no_of_apply_actns_4_miss_curr; 

      /** Internal helping variable used in get next api*/
      void    *miss_entry_apply_actns_curr; 

      /** Number of match fields supported by table*/
      uint8_t no_of_match_fields; 

      /** Pointer to list of match fields supported */
      void    *match_field_info;

      /** Internal helping variable used in get next api*/
      uint8_t no_of_match_fields_curr;

      /** Internal helping variable used in get next api*/
      void    *match_field_info_curr;  

     /** Number of wildcard fields supported by table*/
      uint8_t no_of_wildcard_fields; 

      /** Pointer to list of wild card fields supported*/
      void    *wildcard_field_info; 

      /** Internal helping variable used in get next api*/
      uint8_t no_of_wildcard_fields_curr; 

      /** Internal helping variable used in get next api*/
      void    *wildcard_field_info_curr;  

      /** Number of set fields supported by table flow entry write action*/
      uint8_t no_of_wr_set_fields;

      /** Pointer to list of set fields of table flow entry write action*/
      void    *flow_wr_set_field_info; 

      /** Internal helping variable used in get next api*/
      uint8_t no_of_wr_set_fields_curr;  

      /** Internal helping variable used in get next api*/
      void    *flow_wr_set_field_info_curr;

      /** Number of set fields supported by table miss entry write action*/
      uint8_t no_of_wr_set_fields_miss;

      /** Pointer to set fields of table miss entry write action*/
      void    *miss_wr_entry_set_field_info; 

      /** Internal helping variable used in get next api*/
      uint8_t no_of_wr_set_fields_miss_curr;  

      /** Internal helping variable used in get next api*/
      void    *miss_wr_entry_set_field_info_curr; 

      /** Number of set fields supported by table flow entry apply action*/
      uint8_t no_of_ap_set_fields; 

      /** Pointer to list of set fields of table flow entry apply action*/
      void    *flow_ap_set_field_info;

      /** Internal helping variable used in get next api*/
      uint8_t no_of_ap_set_fields_curr;  /* used in get next api*/

      /** Internal helping variable used in get next api*/
      void    *flow_ap_set_field_info_curr;  /* used in get next api*/

      /** Number of set fields supported by table miss entry apply action*/
      uint8_t no_of_ap_set_fields_miss; 

      /** Pointer to set fields of table miss entry apply action*/
      void    *miss_ap_entry_set_field_info; 

      /** Internal helping variable used in get next api*/
      uint8_t no_of_ap_set_fields_miss_curr;

      /** Internal helping variable used in get next api*/
      void    *miss_ap_entry_set_field_info_curr;
      #ifdef FSLX_COMMON_AND_L3_EXT_SUPPORT
      uint32_t table_type;
      uint8_t  flow_setup_rate;
      #endif
};


/******************* Table feature utilities *******************************/

//!Table Features Utilities
/*! This module provides utilties to manage table features\n
 *\addtogroup Table_Feature_Utilities 
**/

/** \ingroup Table_Feature_Utilities 
 * \brief  Get Null terminated Table name
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return table name. 
 */
char*
ofu_get_table_name(struct of_msg *msg,
                   struct ofi_table_features_info *table_features);

/** \ingroup Table_Feature_Utilities 
 * \brief  Get number of next table Ids to which present table has reachablity
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return number of next table ids present in the data_path 
 */
uint8_t
ofu_get_number_of_next_table_ids_present(struct of_msg *msg,
                                         struct ofi_table_features_info *table_features);

/** \ingroup Table_Feature_Utilities 
 * \brief  Get First Table Id from next table feature list
 * <b>Description</b>\n 
 * The table list feature maintains list of tables that can be directly reachable from the present table\n
 * This API gets first table ID from the list of available next table IDs.
 *
 * In case of OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE return value, application may call\n
 * ofu_get_next_tbl_id_from_next_table_list_feature().
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [out] first_table_id - First Table ID of next table feature list.
 * \return OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE - More elements of the same table feature present in the list 
 * \return OF_LAST_DATA_ELEMENTS_IN_TABLE_FEATURE - Last elements of this table feature. 
 * \return OF_ERROR_IN_GETTING_THE_ELEMENT        - Error in getting the table feature element.
 */
int32_t
ofu_get_first_tbl_id_from_next_table_list_feature(struct of_msg *msg,
                                                  struct ofi_table_features_info *table_features,
                                                  uint8_t *first_table_id);
/** \ingroup Table_Feature_Utilities 
 * \brief  Get Next Table Id from next table feature list
 * <b>Description</b>\n 
 * The table list feature maintains list of tables that can be directly reachable from the present table\n
 * This API gets next table ID from the list of available next table IDs.\n
 * Multiples calls of the same API returns next to the current table ID.\n
 *
 * In case of OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE return value, application may call\n
 * ofu_get_next_tbl_id_from_next_table_list_feature() again.
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [out] next_table_id - Next Table ID of next table feature list.
 * \return OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE - More elements of the same table feature present in the list 
 * \return OF_LAST_DATA_ELEMENTS_IN_TABLE_FEATURE - Last elements of this table feature. 
 * \return OF_ERROR_IN_GETTING_THE_ELEMENT        - Error in getting the table feature element.
 */
int32_t
ofu_get_next_tbl_id_from_next_table_list_feature(struct of_msg *msg,
                                                 struct ofi_table_features_info *table_features,
                                                 uint8_t *next_table_id);

/** \ingroup Table_Feature_Utilities 
 * \brief  Get First Table Id from next table feature list for miss entry.
 * <b>Description</b>\n 
 * The table list feature maintains list of tables that can be directly reachable from the present table\n
 * This API gets first table ID from the list of available next table IDs for miss entry.\n
 *
 * In case of OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE return value, application may call\n
 * ofu_get_next_tbl_id_from_next_table_list_feature_for_miss_entry().
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [out] first_table_id - First Table ID of next table feature list for miss entry.
 * \return OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE - More elements of the same table feature present in the list 
 * \return OF_LAST_DATA_ELEMENTS_IN_TABLE_FEATURE - Last elements of this table feature. 
 * \return OF_ERROR_IN_GETTING_THE_ELEMENT        - Error in getting the table feature element.
 */
int32_t
ofu_get_first_tbl_id_from_next_table_list_feature_for_miss_entry(struct of_msg *msg,
                                                  struct ofi_table_features_info *table_features,
                                                  uint8_t *first_table_id);

/** \ingroup Table_Feature_Utilities 
 * \brief  Get Next Table Id from next table feature list for miss entry.
 * <b>Description</b>\n 
 * The table list feature maintains list of tables that can be directly reachable from the present table\n
 * This API gets next table ID from the list of available next table IDs for miss entry.\n
 * Multiples calls of the same API returns next to the current table ID.\n
 *
 * In case of OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE return value, application may call\n
 * ofu_get_next_tbl_id_from_next_table_list_feature_for_miss_entry() again.
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same paramter value that passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same paramter value that passed to of_table_features_cbk_fn().
 * \param [out] next_table_id - Next Table ID of next table feature list for miss entry.
 * \return OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE - More elements of the same table feature present in the list 
 * \return OF_LAST_DATA_ELEMENTS_IN_TABLE_FEATURE - Last elements of this table feature. 
 * \return OF_ERROR_IN_GETTING_THE_ELEMENT        - Error in getting the table feature element.
 */

int32_t
ofu_get_next_tbl_id_from_next_table_list_feature_for_miss_entry(struct of_msg *msg,
                                                 struct ofi_table_features_info *table_features,
                                                 uint8_t *next_table_id);

/** \ingroup Table_Feature_Utilities 
 * \brief  Get Number of Instructions that table supported 
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return Number of instructions that table supported. 
 */
uint8_t
ofu_get_no_of_instructions_table_support(struct of_msg *msg,
                                         struct ofi_table_features_info *table_features);

/** \ingroup Table_Feature_Utilities 
 * \brief  Whether goto table instruction supported or not 
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return TRUE  - If goto table instruction supported 
 * \return FALSE - If goto table instruction not supported 
 */
uint8_t
ofu_is_goto_table_instruction_support(struct of_msg *msg,
                                       struct ofi_table_features_info *table_features);
/** \ingroup Table_Feature_Utilities 
 * \brief  Whether write meta_data instruction supported or not 
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return TRUE  - If write meta_data instruction supported 
 * \return FALSE - If write meta_data instruction not supported 
 */
uint8_t
ofu_is_write_meta_data_instruction_support(struct of_msg *msg,
                                       struct ofi_table_features_info *table_features);
/** \ingroup Table_Feature_Utilities 
 * \brief  Whether write action instruction supported or not 
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return TRUE  - If write action instruction supported 
 * \return FALSE - If write action instruction not supported 
 */
uint8_t
ofu_is_write_action_instruction_support(struct of_msg *msg,
                                       struct ofi_table_features_info *table_features);

/** \ingroup Table_Feature_Utilities 
 * \brief  Whether apply action instruction supported or not 
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return TRUE  - If apply action instruction supported 
 * \return FALSE - If apply action instruction not supported 
 */
uint8_t
ofu_is_apply_action_instruction_support(struct of_msg *msg,
                                       struct ofi_table_features_info *table_features);

/** \ingroup Table_Feature_Utilities 
 * \brief  Whether clear action instruction supported or not 
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return TRUE  - If clear action instruction supported 
 * \return FALSE - If clear action instruction not supported 
 */
uint8_t
ofu_is_clear_action_instruction_support(struct of_msg *msg,
                                       struct ofi_table_features_info *table_features);

/** \ingroup Table_Feature_Utilities 
 * \brief  Whether apply meter instruction supported or not 
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return TRUE  - If apply meter instruction supported 
 * \return FALSE - If apply meter instruction not supported 
 */
uint8_t
ofu_is_apply_meter_instruction_supported(struct of_msg *msg,
                                       struct ofi_table_features_info *table_features);

/** \ingroup Table_Feature_Utilities 
 * \brief  Get Number of Instructions that table supported for miss entry 
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return Number of instructions that table supported for miss entry 
 */
uint8_t
ofu_get_no_of_instructions_table_support_for_miss_entry(struct of_msg *msg,
                                         struct ofi_table_features_info *table_features);
/** \ingroup Table_Feature_Utilities 
 * \brief  Whether goto table instruction supported or not for miss entry
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return TRUE  - If goto table instruction supported for miss entry
 * \return FALSE - If goto table instruction not supported for miss entry
 */
uint8_t
ofu_is_goto_table_instruction_support_for_miss_entry(struct of_msg *msg,
                                       struct ofi_table_features_info *table_features);

/** \ingroup Table_Feature_Utilities 
 * \brief  Whether write meta_data instruction supported or not for miss entry
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return TRUE  - If write meta_data instruction supported for miss entry
 * \return FALSE - If write meta_data instruction not supported for miss entry
 */
uint8_t
ofu_is_write_meta_data_instruction_support_for_miss_entry(struct of_msg *msg,
                                       struct ofi_table_features_info *table_features);

/** \ingroup Table_Feature_Utilities 
 * \brief  Whether write action instruction supported or not for miss entry
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return TRUE  - If write action instruction supported for miss entry
 * \return FALSE - If write action instruction not supported for miss entry
 */
uint8_t
ofu_is_write_action_instruction_support_for_miss_entry(struct of_msg *msg,
                                       struct ofi_table_features_info *table_features);

/** \ingroup Table_Feature_Utilities 
 * \brief  Whether apply action instruction supported or not for miss entry
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return TRUE  - If apply action instruction supported for miss entry
 * \return FALSE - If apply action instruction not supported for miss entry
 */
uint8_t
ofu_is_apply_action_instruction_support_for_miss_entry(struct of_msg *msg,
                                       struct ofi_table_features_info *table_features);

/** \ingroup Table_Feature_Utilities 
 * \brief  Whether clear action instruction supported or not for miss entry
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return TRUE  - If clear action instruction supported for miss entry
 * \return FALSE - If clear action instruction not supported for miss entry
 */
uint8_t
ofu_is_clear_action_instruction_support_for_miss_entry(struct of_msg *msg,
                                       struct ofi_table_features_info *table_features);
/** \ingroup Table_Feature_Utilities 
 * \brief  Whether meter instruction supported or not for miss entry
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return TRUE  - If meter instruction supported for miss entry
 * \return FALSE - If meter instruction not supported for miss entry
 */
uint8_t
ofu_is_apply_meter_instruction_support_for_miss_entry(struct of_msg *msg,
                                       struct ofi_table_features_info *table_features);

/** \ingroup Table_Feature_Utilities 
 * \brief  Get Number of Write actions that table supported 
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return number of write actions that table supported for 
 */
uint8_t
ofu_get_no_of_write_actions_supported(struct of_msg *msg,
                                      struct ofi_table_features_info *table_features);

/** \ingroup Table_Feature_Utilities 
 * \brief Get first action ID supported for write action instruction 
 * <b>Description</b>\n 
 * This API gets first action ID from the write actions instructions list.
 *
 * In case of OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE return value, application may call\n
 * ofu_get_next_action_id_of_write_actions().
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [out] first_action_id - First action ID from write action instruction of table 
 * \return OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE - More elements of the same table feature present in the list 
 * \return OF_LAST_DATA_ELEMENTS_IN_TABLE_FEATURE - Last elements of this table feature. 
 * \return OF_ERROR_IN_GETTING_THE_ELEMENT        - Error in getting the table feature element.
 */
int32_t
ofu_get_first_action_id_of_write_actions(struct of_msg *msg,
                                        struct ofi_table_features_info *table_features,
                                        uint8_t *first_action_id);                                        
/** \ingroup Table_Feature_Utilities 
 * \brief Get next action ID supported for write action instruction 
 * <b>Description</b>\n 
 * This API gets next action ID from the write actions instructions list.
 * Multiple calls of this API returns next action ID to previously returned 
 * action Id.
 *
 * In case of OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE return value, application may call\n
 * ofu_get_next_action_id_of_write_actions() again.
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [out] next_action_id - Next action ID from write action instruction of table 
 * \return OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE - More elements of the same table feature present in the list 
 * \return OF_LAST_DATA_ELEMENTS_IN_TABLE_FEATURE - Last elements of this table feature. 
 * \return OF_ERROR_IN_GETTING_THE_ELEMENT        - Error in getting the table feature element.
 */
int32_t
ofu_get_next_action_id_of_write_actions(struct of_msg *msg,
                                        struct ofi_table_features_info *table_features,
                                        uint8_t *next_action_id);

/** \ingroup Table_Feature_Utilities 
 * \brief  Get Number of Write actions that table supported for miss entry 
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return Number of write actions that table supported for miss entry 
 */

uint8_t
ofu_get_no_of_write_actions_supported_for_miss_entry(struct of_msg *msg,
                                         struct ofi_table_features_info *table_features);

/** \ingroup Table_Feature_Utilities 
 * \brief Get first action ID supported for write action instruction for miss entry
 * <b>Description</b>\n 
 * This API gets first action ID from the write actions instructions list for miss entry.
 *
 * In case of OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE return value, application may call\n
 * ofu_get_next_action_id_of_write_actions().
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [out] first_action_id - First action ID from write action instruction of table for miss entry
 * \return OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE - More elements of the same table feature present in the list 
 * \return OF_LAST_DATA_ELEMENTS_IN_TABLE_FEATURE - Last elements of this table feature. 
 * \return OF_ERROR_IN_GETTING_THE_ELEMENT        - Error in getting the table feature element.
 */
int32_t
ofu_get_first_action_id_of_write_actions_for_miss_entry(struct of_msg *msg,
                                        struct ofi_table_features_info *table_features,
                                        uint8_t *first_action_id);                                        

/** \ingroup Table_Feature_Utilities 
 * \brief Get next action ID supported for write action instruction for miss entry
 * <b>Description</b>\n 
 * This API gets next action ID from the write actions instructions list for miss entry.
 * Multiple calls of this API returns next action ID to previously returned 
 * action Id.
 *
 * In case of OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE return value, application may call\n
 * ofu_get_next_action_id_of_write_actions() again.
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [out] next_action_id - Next action ID from write action instruction of table for miss entry
 * \return OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE - More elements of the same table feature present in the list 
 * \return OF_LAST_DATA_ELEMENTS_IN_TABLE_FEATURE - Last elements of this table feature. 
 * \return OF_ERROR_IN_GETTING_THE_ELEMENT        - Error in getting the table feature element.
 */
int32_t
ofu_get_next_action_id_of_write_actions_for_miss_entry(struct of_msg *msg,
                                        struct ofi_table_features_info *table_features,
                                        uint8_t *next_action_id);

/** \ingroup Table_Feature_Utilities 
 * \brief  Get Number of Apply actions that table supported 
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return number of apply actions that table supported 
 */
uint8_t
ofu_get_no_of_apply_actions_supported(struct of_msg *msg,
                                         struct ofi_table_features_info *table_features);

/** \ingroup Table_Feature_Utilities 
 * \brief Get first action ID supported for apply action instruction 
 * <b>Description</b>\n 
 * This API gets first action ID from the apply actions instructions list.
 *
 * In case of OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE return value, application may call\n
 * ofu_get_next_action_id_of_apply_actions().
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [out] first_action_id - First action ID from apply action instruction of table 
 * \return OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE - More elements of the same table feature present in the list 
 * \return OF_LAST_DATA_ELEMENTS_IN_TABLE_FEATURE - Last elements of this table feature. 
 * \return OF_ERROR_IN_GETTING_THE_ELEMENT        - Error in getting the table feature element.
 */
int32_t
ofu_get_first_action_id_of_apply_actions(struct of_msg *msg,
                                        struct ofi_table_features_info *table_features,
                                        uint8_t *first_action_id);                                        

/** \ingroup Table_Feature_Utilities 
 * \brief Get next action ID supported for apply action instruction 
 * <b>Description</b>\n 
 * This API gets next action ID from the apply actions instructions list.
 * Multiple calls of this API returns next action ID to previously returned 
 * action Id.
 *
 * In case of OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE return value, application may call\n
 * ofu_get_next_action_id_of_apply_actions() again.
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [out] next_action_id - Next action ID from apply action instruction of table 
 * \return OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE - More elements of the same table feature present in the list 
 * \return OF_LAST_DATA_ELEMENTS_IN_TABLE_FEATURE - Last elements of this table feature. 
 * \return OF_ERROR_IN_GETTING_THE_ELEMENT        - Error in getting the table feature element.
 */
int32_t
ofu_get_next_action_id_of_apply_actions(struct of_msg *msg,
                                        struct ofi_table_features_info *table_features,
                                        uint8_t *next_action_id);

/** \ingroup Table_Feature_Utilities 
 * \brief  Get Number of Apply actions that table supported for miss entry
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return number of apply actions that table supported for miss entry
 */
uint8_t
ofu_get_no_of_apply_actions_supported_for_miss_entry(struct of_msg *msg,
                                         struct ofi_table_features_info *table_features);

/** \ingroup Table_Feature_Utilities 
 * \brief Get first action ID supported for apply action instruction for miss entry
 * <b>Description</b>\n 
 * This API gets first action ID from the apply actions instructions list for miss entry.
 *
 * In case of OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE return value, application may call\n
 * ofu_get_next_action_id_of_apply_actions().
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [out] first_action_id - First action ID from apply action instruction of table  for miss entry
 * \return OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE - More elements of the same table feature present in the list 
 * \return OF_LAST_DATA_ELEMENTS_IN_TABLE_FEATURE - Last elements of this table feature. 
 * \return OF_ERROR_IN_GETTING_THE_ELEMENT        - Error in getting the table feature element.
 */
int32_t
ofu_get_first_action_id_of_apply_actions_for_miss_entry(struct of_msg *msg,
                                        struct ofi_table_features_info *table_features,
                                        uint8_t *first_action_id);                                        

/** \ingroup Table_Feature_Utilities 
 * \brief Get next action ID supported for apply action instruction for miss entry
 * <b>Description</b>\n 
 * This API gets next action ID from the apply actions instructions list for miss entry.
 * Multiple calls of this API returns next action ID to previously returned 
 * action Id.
 *
 * In case of OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE return value, application may call\n
 * ofu_get_next_action_id_of_apply_actions() again.
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [out] next_action_id - Next action ID from apply action instruction of table for miss entry
 * \return OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE - More elements of the same table feature present in the list 
 * \return OF_LAST_DATA_ELEMENTS_IN_TABLE_FEATURE - Last elements of this table feature. 
 * \return OF_ERROR_IN_GETTING_THE_ELEMENT        - Error in getting the table feature element.
 */
int32_t
ofu_get_next_action_id_of_apply_actions_for_miss_entry(struct of_msg *msg,
                                                      struct ofi_table_features_info *table_features,
                                                      uint8_t *next_action_id);

/** \ingroup Table_Feature_Utilities 
 * \brief  Get Number of table match fields supported  
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return number of  table match fields supported
 */
uint8_t
ofu_get_no_of_match_fields_supported(struct of_msg *msg,
                                     struct ofi_table_features_info *table_features);

/** \ingroup Table_Feature_Utilities 
 * \brief Get first table match field 
 * <b>Description</b>\n 
 * This API gets details for first match field that table supported. 
 *
 * In case of OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE return value, application may call\n
 * ofu_get_next_match_field_of_table().
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [out] first_match_field_type - Type of the first match field supported by table
 * \param [out] first_match_field_mask - Boolean flag, TRUE indicate mask set for the first match field
 * \return OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE - More elements of the same table feature present in the list 
 * \return OF_LAST_DATA_ELEMENTS_IN_TABLE_FEATURE - Last elements of this table feature. 
 * \return OF_ERROR_IN_GETTING_THE_ELEMENT        - Error in getting the table feature element.
 */
int32_t
ofu_get_first_match_field_of_table(struct of_msg *msg,
                                   struct ofi_table_features_info *table_features,
                                   uint8_t *first_match_field_type,
                                   uint8_t *first_match_field_mask);                                        

/** \ingroup Table_Feature_Utilities 
 * \brief Get next match field of table 
 * <b>Description</b>\n 
 * This API gets details for next match field that table supported. 
 * Calling this API multiple times always returns next match field to\n
 * the match field returned previously.
 *
 * In case of OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE return value, application may call\n
 * ofu_get_next_match_field_of_table() again.
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same paramter value that passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same paramter value that passed to of_table_features_cbk_fn().
 * \param [out] next_match_field_type - Type of the next match field supported by table
 * \param [out] next_match_field_mask - Boolean flag, TRUE indicate mask set for the next match field
 * \return OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE - More elements of the same table feature present in the list 
 * \return OF_LAST_DATA_ELEMENTS_IN_TABLE_FEATURE - Last elements of this table feature. 
 * \return OF_ERROR_IN_GETTING_THE_ELEMENT        - Error in getting the table feature element.
 */
int32_t
ofu_get_next_match_field_of_table(struct of_msg *msg,
                                  struct ofi_table_features_info *table_features,
                                  uint8_t *next_match_field_type,
                                  uint8_t *next_match_field_mask);                                        

/** \ingroup Table_Feature_Utilities 
 * \brief  Get Number of table wildcard match fields supported  
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return number of  table wildcard  match fields supported
 */
uint8_t
ofu_get_no_of_wildcard_fields_supported(struct of_msg *msg,
                                       struct ofi_table_features_info *table_features);

/** \ingroup Table_Feature_Utilities 
 * \brief Get first table match field 
 * <b>Description</b>\n 
 * This API gets details for first wildcard field that table supported. 
 *
 * In case of OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE return value, application may call\n
 * ofu_get_next_wildcard_field_of_table().
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [out] first_wildcard_field_type - Type of the first wildcard field supported by table
 * \return OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE - More elements of the same table feature present in the list 
 * \return OF_LAST_DATA_ELEMENTS_IN_TABLE_FEATURE - Last elements of this table feature. 
 * \return OF_ERROR_IN_GETTING_THE_ELEMENT        - Error in getting the table feature element.
 */
int32_t
ofu_get_first_wildcard_field_of_table(struct of_msg *msg,
                                   struct ofi_table_features_info *table_features,
                                   uint8_t *first_wildcard_field_type);

/** \ingroup Table_Feature_Utilities 
 * \brief Get next wildcard field of table 
 * <b>Description</b>\n 
 * This API gets details for next wildcard field that table supported. 
 * Calling this API multiple times always returns next wildcard field to\n
 * the wildcard field returned previously.
 *
 * In case of OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE return value, application may call\n
 * ofu_get_next_wildcard_field_of_table() again.
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [out] next_wildcard_field_type - Type of the next wildcard field supported by table
 * \return OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE - More elements of the same table feature present in the list 
 * \return OF_LAST_DATA_ELEMENTS_IN_TABLE_FEATURE - Last elements of this table feature. 
 * \return OF_ERROR_IN_GETTING_THE_ELEMENT        - Error in getting the table feature element.
 */
int32_t
ofu_get_next_wildcard_field_of_table(struct of_msg *msg,
                                  struct ofi_table_features_info *table_features,
                                  uint8_t *next_wildcard_field_type);

/** \ingroup Table_Feature_Utilities 
 * \brief  Get Number of table set fields for write actions supported  
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return  number of match fields of table supported that used\n
 *          in set field action of write action instruction
 */
uint8_t
ofu_get_no_of_set_fields_supported_in_write_action(struct of_msg *msg,
                                                   struct ofi_table_features_info *table_features);


/** \ingroup Table_Feature_Utilities 
 * \brief Get first set field of write action 
 * <b>Description</b>\n 
 * This API gets first  set fields supported in write action instruction 
 *
 * In case of OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE return value, application may call\n
 * ofu_get_next_set_field_write_action().
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [out] first_set_field_type - Type of the first set field supported by table for write actions.
 * \param [out] first_set_field_mask - Boolean flag, TRUE indicate mask set for the first set field
 * \return OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE - More elements of the same table feature present in the list 
 * \return OF_LAST_DATA_ELEMENTS_IN_TABLE_FEATURE - Last elements of this table feature. 
 * \return OF_ERROR_IN_GETTING_THE_ELEMENT        - Error in getting the table feature element.
 */
int32_t
ofu_get_first_set_field_write_action(struct of_msg *msg,
                                   struct ofi_table_features_info *table_features,
                                   uint8_t *first_set_field_type,
                                   uint8_t *first_set_field_mask);                                        

/** \ingroup Table_Feature_Utilities 
 * \brief Get next set field of write action 
 * <b>Description</b>\n 
 * This API gets  next set fields supported in write action instruction.
 * Calling this API multiple times always returns next set field to\n
 * the set field returned previously.
 *
 * In case of OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE return value, application may call\n
 * ofu_get_next_set_field_write_action() again.
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [out] next_set_field_type - Type of the next set field supported by table write action
 * \param [out] next_set_field_mask - Boolean flag, TRUE indicate mask set for the next set field
 * \return OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE - More elements of the same table feature present in the list 
 * \return OF_LAST_DATA_ELEMENTS_IN_TABLE_FEATURE - Last elements of this table feature. 
 * \return OF_ERROR_IN_GETTING_THE_ELEMENT        - Error in getting the table feature element.
 */
int32_t
ofu_get_next_set_field_write_action(struct of_msg *msg,
                                  struct ofi_table_features_info *table_features,
                                  uint8_t *next_set_field_type,
                                  uint8_t *next_set_field_mask);                                        

/** \ingroup Table_Feature_Utilities 
 * \brief  Get Number of table set fields for write actions supported  in case of miss entry
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return  number of match fields of table supported that used\n
 *          in set field action of write action instruction in case of miss entry
 */
uint8_t
ofu_get_no_of_set_fields_supported_in_write_action_for_miss_entry(struct of_msg *msg,
                                         struct ofi_table_features_info *table_features);

/** \ingroup Table_Feature_Utilities 
 * \brief Get first set field of write action for miss entry
 * <b>Description</b>\n 
 * This API gets first  set fields supported in write action instruction for miss entry.
 *
 * In case of OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE return value, application may call\n
 * ofu_get_next_set_field_write_action_for_miss_entry().
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [out] first_set_field_type - Type of the first set field supported by table for write actions in case of miss entry.
 * \param [out] first_set_field_mask - Boolean flag, TRUE indicate mask set for the first set field
 * \return OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE - More elements of the same table feature present in the list 
 * \return OF_LAST_DATA_ELEMENTS_IN_TABLE_FEATURE - Last elements of this table feature. 
 * \return OF_ERROR_IN_GETTING_THE_ELEMENT        - Error in getting the table feature element.
 */
int32_t
ofu_get_first_set_field_write_action_for_miss_entry(struct of_msg *msg,
                                   struct ofi_table_features_info *table_features,
                                   uint8_t *first_set_field_type,
                                   uint8_t *first_set_field_mask);                                        

/** \ingroup Table_Feature_Utilities 
 * \brief Get next set field of write action in case of miss entry
 * <b>Description</b>\n 
 * This API gets  next set fields supported in write action instruction for miss entry.
 * Calling this API multiple times always returns next set field to\n
 * the set field returned previously.
 *
 * In case of OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE return value, application may call\n
 * ofu_get_next_set_field_write_action_for_miss_entry() again.
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [out] next_set_field_type - Type of the next set field supported by table write action in case of miss entry
 * \param [out] next_set_field_mask - Boolean flag, TRUE indicate mask set for the next set field
 * \return OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE - More elements of the same table feature present in the list 
 * \return OF_LAST_DATA_ELEMENTS_IN_TABLE_FEATURE - Last elements of this table feature. 
 * \return OF_ERROR_IN_GETTING_THE_ELEMENT        - Error in getting the table feature element.
 */
int32_t
ofu_get_next_set_field_write_action_for_miss_entry(struct of_msg *msg,
                                  struct ofi_table_features_info *table_features,
                                  uint8_t *next_set_field_type,
                                  uint8_t *next_set_field_mask);                                        

/** \ingroup Table_Feature_Utilities 
 * \brief  Get Number of table set fields for apply actions supported  
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return  number of match fields of table supported that used\n
 *          in set field action of apply action instruction
 */
uint8_t
ofu_get_no_of_set_fields_supported_in_apply_action(struct of_msg *msg,
                                         struct ofi_table_features_info *table_features);

/** \ingroup Table_Feature_Utilities 
 * \brief Get first set field of apply action 
 * <b>Description</b>\n 
 * This API gets first  set fields supported in apply action instruction 
 *
 * In case of OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE return value, application may call\n
 * ofu_get_next_set_field_apply_action().
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [out] first_set_field_type - Type of the first set field supported by table for apply actions.
 * \param [out] first_set_field_mask - Boolean flag, TRUE indicate mask set for the first set field
 * \return OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE - More elements of the same table feature present in the list 
 * \return OF_LAST_DATA_ELEMENTS_IN_TABLE_FEATURE - Last elements of this table feature. 
 * \return OF_ERROR_IN_GETTING_THE_ELEMENT        - Error in getting the table feature element.
 */
int32_t
ofu_get_first_set_field_apply_action(struct of_msg *msg,
                                   struct ofi_table_features_info *table_features,
                                   uint8_t *first_set_field_type,

                                   uint8_t *first_set_field_mask);                                        
/** \ingroup Table_Feature_Utilities 
 * \brief Get next set field of apply action 
 * <b>Description</b>\n 
 * This API gets  next set fields supported in apply action instruction.
 * Calling this API multiple times always returns next set field to\n
 * the set field returned previously.
 *
 * In case of OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE return value, application may call\n
 * ofu_get_next_set_field_apply_action() again.
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [out] next_set_field_type - Type of the next set field supported by table apply action
 * \param [out] next_set_field_mask - Boolean flag, TRUE indicate mask set for the next set field
 * \return OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE - More elements of the same table feature present in the list 
 * \return OF_LAST_DATA_ELEMENTS_IN_TABLE_FEATURE - Last elements of this table feature. 
 * \return OF_ERROR_IN_GETTING_THE_ELEMENT        - Error in getting the table feature element.
 */
int32_t
ofu_get_next_set_field_apply_action(struct of_msg *msg,
                                  struct ofi_table_features_info *table_features,
                                  uint8_t *next_set_field_type,
                                  uint8_t *next_set_field_mask);                                        

/** \ingroup Table_Feature_Utilities 
 * \brief  Get Number of table set fields for apply actions supported  in case of miss entry
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter was passed to of_table_features_cbk_fn().
 * \return  number of match fields of table supported that used\n
 *          in set field action of apply action instruction in case of miss entry
 */
uint8_t
ofu_get_no_of_set_fields_supported_in_apply_action_for_miss_entry(struct of_msg *msg,
                                         struct ofi_table_features_info *table_features);

/** \ingroup Table_Feature_Utilities 
 * \brief Get first set field of apply action for miss entry
 * <b>Description</b>\n 
 * This API gets first  set fields supported in apply action instruction for miss entry.
 *
 * In case of OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE return value, application may call\n
 * ofu_get_next_set_field_apply_action_for_miss_entry().
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [out] first_set_field_type - Type of the first set field supported by table for apply actions in case of miss entry.
 * \param [out] first_set_field_mask - Boolean flag, TRUE indicate mask set for the first set field
 * \return OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE - More elements of the same table feature present in the list 
 * \return OF_LAST_DATA_ELEMENTS_IN_TABLE_FEATURE - Last elements of this table feature. 
 * \return OF_ERROR_IN_GETTING_THE_ELEMENT        - Error in getting the table feature element.
 */
int32_t
ofu_get_first_set_field_apply_action_for_miss_entry(struct of_msg *msg,
                                                    struct ofi_table_features_info *table_features,
                                                    uint8_t *first_set_field_type,
                                                    uint8_t *first_set_field_mask);                                        

/** \ingroup Table_Feature_Utilities 
 * \brief Get next set field of apply action in case of miss entry
 * <b>Description</b>\n 
 * This API gets  next set fields supported in apply action instruction for miss entry.
 * Calling this API multiple times always returns next set field to\n
 * the set field returned previously.
 *
 * In case of OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE return value, application may call\n
 * ofu_get_next_set_field_apply_action_for_miss_entry() again.
 * \param [in] msg            - Message  buffer received from data_path contains table features. \n
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [in] table_features - Pointer to table features details. 
 *                              Same parameter value that passed to of_table_features_cbk_fn().
 * \param [out] next_set_field_type - Type of the next set field supported by table apply action in case of miss entry
 * \param [out] next_set_field_mask - Boolean flag, TRUE indicate mask set for the next set field
 * \return OF_MORE_DATA_ELEMENTS_IN_TABLE_FEATURE - More elements of the same table feature present in the list 
 * \return OF_LAST_DATA_ELEMENTS_IN_TABLE_FEATURE - Last elements of this table feature. 
 * \return OF_ERROR_IN_GETTING_THE_ELEMENT        - Error in getting the table feature element.
 */
int32_t
ofu_get_next_set_field_apply_action_for_miss_entry(struct of_msg *msg,
                                                   struct ofi_table_features_info *table_features,
                                                   uint8_t *next_set_field_type,
                                                   uint8_t *next_set_field_mask);                                        

/** \ingroup Table_Features
 * \typedef of_table_features_cbk_fn 
 * \brief Callback function to receive table features 
 * <b>Description </b>\n
 * This callback function will be passed to of_get_table_features() API to receive table features of\n
 * all the tables present in the data_path.
 *
 * The table features are returned asynchronously at later point of time by using this callback.
 *
 * This callback will be called multiple times with details of features for one table at a time.
 *  
 * \param [in] msg                - Message will messasge descriptor with buffer contains table features
 *                                  Controller allocates the memory required to read  table features.
 *                                  Once application done with response , the  free function available in 
 *                                  the message descriptor will be called to free the  memory. 
 *                                  This message need to pass into different utility function to parse the
 *                                  table features received from data_path.
 * \param [in] controller_handle  - Handle of the controller
 * \param [in] domain_handle      - Handle of the domain
 * \param [in] datapath_handle    - Handle of the data_path.
 * \param [in] table_id           - Id of the table for which passing table features. 
 * \param [in] cbk_arg1           - First Application specific private info that passed in the API to
 *                                  get table features request
 * \param [in] cbk_arg2           - Second Application specific private info that passed in the API to
 *                                  get table features request
 * \param [in] status             - Flag indicated the status  of response to earlier get table statistics request.
 * \param [in] table_features     - Pointer to table features details. One can call Utility functions to get more details.
 * \param [in] more_tables        - Boolean flag, TRUE indicates still more table features are pending. FALSE used while
 *                                  passing last table features of the data_path.
 */
typedef void (*of_table_features_cbk_fn)( 
                                       uint64_t  controller_handle,
                                       uint64_t  domain_handle,
                                       uint64_t  datapath_handle,
                                       uint8_t   table_id,
				       struct of_msg *msg,
                                       void     *cbk_arg1,
                                       void     *cbk_arg2,
                                       uint8_t   status,
                                       struct ofi_table_features_info *table_features,
                                       uint8_t   more_tables);

/** \ingroup Table_Features 
 * \brief Send Table feature request message.  \n
 *  <b>Description </b>\n
 *  API to get details of features  of all tables present in the data_path. 
 * 
 *  The return value of OFU_TABLE_FEATURES_REQ_MESSAGE_LEN used to allocate length of\n
 *  message required to send get table features details.
 *  \param [in] msg                - Message descriptor with buffer allocated earlier to send request to get table
 *                                   features.  
 *  \param [in] datapath_handle    - Handle of the data_path to which we are sending the get request. 
 *  \param [in] table_features_cbk - Callback function used to pass table features.
 *  \param [out] xid               - Id assigned for the transaction. The same transaction Id will be passed to
 *                                   callback function. 
 *  \param [in] cbk_arg1           - Application private information, arg1, that will be passed to error callback in case\n
 *                                   of any errors. Also same passed to callback functions in case of success.   \n
 *  \param [in] cbk_arg2           - Application private information, arg2, that will be passed to error callback in case\n
 *                                   of any errors. Also same passed to callback functions in case of success.   \n
 
 *  \return OFU_INVALID_PARAMETER_PASSED
 *  \return OFU_GET_TABLE_FEATURE_MSG_SUCCESS 
 */
int32_t
of_get_table_features( struct of_msg *msg,
                       uint64_t datapath_handle,
                       of_table_features_cbk_fn table_features_cbk, 
                       void *cbk_arg1,
                       void *cbk_arg2);

/************** Table Satatistics get API *********************/

//!Table Get Statistics  APIs 
/*! This module contains APIs related to getting table statistics values. 
 *
 * APIs are provided to get table statistics.\n
 *
 * Callback function will be passed to of_get_table_statistics() API to 
 * receive table statistics details. 
 *
 * \sa msg_mem_mgmt
 *\addtogroup Table_Statistics 
 */

/** \ingroup Table_Statistics
 * OFU_TABLE_STATISTICS_REQ_MESSAGE_LEN
 * - Returns length of message required to send 'get table statistics' message'. 
 */
#define OFU_INDIVIUAL_TABLE_STATS_REQ_MESSAGE_LEN  sizeof(struct ofp_multipart_request)

#define OFU_TABLE_STATISTICS_REQ_MESSAGE_LEN   sizeof(struct ofp_multipart_request)

/** \ingroup Table_Statistics
 * \struct ofi_table_stats_info
 * \brief Table statistics details\n
 *  <b>Description </b>\n
 * Data strcture holding table statistics details, all the fields must be converted into \n
 * host byte order before calling callback functions of_table_stats_cbk_fn().\n
 */
struct ofi_table_stats_info
{
    /* table id */	 
    uint8_t table_id;

    /** Number of active entries*/
    uint32_t  active_count; 

    /** Number of packets looked up in table*/
    uint64_t  lookup_count; 
    
    /** Number of packets that hit table entries*/
    uint64_t  matched_count;
};

/** \ingroup Table_Statistics
 * \typedef of_table_stats_cbk_fn 
 * \brief Callback function to receive table statistics 
 * <b>Description </b>\n
 * This callback function will be pased to of_get_table_statistics() API \n
 * to receive table statistics values for each table present in the system \n
 * The table statistics are returned asynchronously at later pointer by using this callback.
 *
 * The callback function will be called multiple times with statistics details of one\n
 * table at a time.
 * \param [in] controller_handle  - Handle of the controller
 * \param [in] domain_handle      - Handle of the domain
 * \param [in] datapath_handle    - Handle of the data_path.
 * \param [in] table_id           - Id of the table for which passing table statistics. 
 * \param [in] msg                - Message descriptor with buffer contains table statistics
 *                                  Controller allocates the memory required to read  table statistics. 
 *                                  Once application done with response , the  free function available in 
 *                                  the message descriptor will be called to free the  memory. 
 *                                  This message descriptor need to pass for debugging purpose. 
 * \param [in] cbk_arg1           - First Application specific private info that passed in the API to
 *                                  get table statistics request.
 * \param [in] cbk_arg2           - Second Application specific private info that passed in the API to
 *                                  get table statistics request.
 * \param [in] status             - Flag indicated the status  of response to earlier multipart request.
 * \param [in] table_stats        - Pointer to table statistics information
 * \param [in] more_tables        - Boolean flag, TRUE indicates still more table statistics are pending.\n
 *                                  FALSE used while passing last table of the data_path.
 */
typedef void (*of_table_stats_cbk_fn)( 
                                       uint64_t  controller_handle,
                                       uint64_t  domain_handle,
                                       uint64_t  datapath_handle,
                                       uint8_t   table_id,
				       struct of_msg *msg,
                                       void     *cbk_arg1,
                                       void     *cbk_arg2,
                                       uint8_t   status,
                                       struct ofi_table_stats_info *table_stats,
                                       uint8_t    more_tables);

/** \ingroup Table_Statistics 
 * \brief Send Table statistics request message.  \n
 *  <b>Description </b>\n
 *  API to get details of statistics  of all tables present in the data_path. 
 * 
 *  The return value of OFU_TABLE_STATISTICS_REQ_MESSAGE_LEN is used to allocate \n
 *  length of the message required to send get table statistics.
 *  \param [in] msg                - Message descriptor with buffer allocated earlier to send request to get table
 *                                   statistics.  
 *  \param [in] datapath_handle    - Handle of the data_path to which we are sending the get request. 
 *  \param [in] table_stats_cbk    - Callback function pass table statistics.
 *  \param [in] cbk_arg1           - First Application specific private info that passed in the API to
 *                                   get table statistics request.
 *  \param [in] cbk_arg2           - Second Application specific private info that passed in the API to
 *                                   get table statistics request.
 *  \param [out] xid               - Id assigned for the transaction. The same transaction Id will be passed to
 *                                   callback function. 
 *  \return OFU_INVALID_PARAMETER_PASSED
 *  \return OFU_GET_TABLE_STATS_MSG_SUCCESS 
 */
int32_t
of_get_table_statistics( struct of_msg *msg,
                         uint64_t datapath_handle,
                         of_table_stats_cbk_fn table_stats_cbk, 
                         void     *cbk_arg1,
                         void     *cbk_arg2
                         );

/************** Port(s) statistics get API *********************/

//!Port Get Statistics  APIs 
/*! This module contains APIs related to getting port statistics values. 
 *
 * APIs are provided to get port statistics.\n
 *
 * Callback function will be passed to of_get_port_statistics() API to 
 * receive port statistics details. 
 *
 * \sa msg_mem_mgmt
 *\addtogroup Port_Statistics 
 */

/** \ingroup Port_Statistics
 * OFU_PORT_STATISTICS_REQ_MESSAGE_LEN
 * - Returns length of message required to send 'get port statistics' message'. 
 */
#define OFU_PORT_STATISTICS_REQ_MESSAGE_LEN\
               ( sizeof(struct ofp_multipart_request) + sizeof(struct ofp_port_stats_request) )
#define OFU_INDIVIUAL_PORT_STATS_REQ_MESSAGE_LEN  (sizeof(struct ofp_multipart_request) + sizeof(struct ofp_port_stats_request))

/** \ingroup Port_Statistics
 * \struct ofi_port_stats_info
 * \brief port statistics details\n
 *  <b>Description </b>\n
 * Data strcture holding port statistics details, all the fields must be converted into \n
 * host byte order before calling callback functions of_port_stats_cbk_fn().\n
 */
struct ofi_port_stats_info {
/** Port number */ 
uint32_t port_no;
/** Number of packets received on the port. */
uint64_t rx_packets; 

/** Number of packets transmitted on the port */
uint64_t tx_packets; 

/** Number of bytes received on the port. */
uint64_t rx_bytes; /* Number of received bytes. */

/** NUmber of bytes transmitted on the port */
uint64_t tx_bytes; /* Number of transmitted bytes. */

/** Number of packets dropped on receive side. */
uint64_t rx_dropped; 

/** Number of packets dropped on transmit side. */
uint64_t tx_dropped; 

/** Number of receive errors. This is a super-set of more specific receive\n
 *  errors and should be greater than or equal to the sum of all rx_err values. 
 */ 
uint64_t rx_errors;

/** Number of transmit errors. This is a super-set of more specific transmit\n
 *  errors and should be greater than or equal to the sum of all tx_err values
 */
uint64_t tx_errors;

/** Number of frame alignment errors. */
uint64_t rx_frame_err; 

/** Number of packets with overrun on receive side. */
uint64_t rx_over_err; 

/** Number of CRC errors. */
uint64_t rx_crc_err; 

/** Number of collisions. */
uint64_t collisions; 

/** Time port has been alive in seconds. */
uint32_t duration_sec;

/** Time port has been alive in nanoseconds beyond duration_sec. */
uint32_t duration_nsec; 
};

/** \ingroup Port_Statistics
 * \typedef of_port_stats_cbk_fn 
 * \brief Callback function to receive port statistics 
 * <b>Description </b>\n
 * This callback function will be pased to of_get_port_statistics() API \n
 * to receive port statistics values for each port present in the data_path. \n
 * The port statistics are returned asynchronously at later pointer by using this callback.
 *
 * The callback function might be called multiple times with statistics details of one\n
 * port at a time.
 * \param [in] controller_handle  - Handle of the controller
 * \param [in] domain_handle      - Handle of the domain
 * \param [in] datapath_handle    - Handle of the data_path.
 * \param [in] port_no            - Port number to which passing statistics values. 
 * \param [in] msg                - Message descriptor with buffer contains Open Flow message holding port\n
 *                                  statistics received from data_path. Controller allocates the memory required\n
 *                                  to read  port statistics received from data_path. Once application done with\n
 *                                  port statistics received, the free function pointer available in the message\n
 *                                  descriptor will be called to free the  memory. 
 * \param [in] cbk_arg1           - First Application specific private info that passed in the API to
 *                                  get table statistics request.
 * \param [in] cbk_arg2           - Second Application specific private info that passed in the API to
 *                                  get table statistics request.
 * \param [in] status             - Flag indicated the status  of response to earlier multipart request.
 * \param [in] port_statistics    - Pointer to port statistics info. All the data is converted into host byte order.
 * \param [in] more_ports         - Boolean value , TRUE indicates, this is not final response and more ports details
 *                                  yet to come. FALSE means this is the last port response
 */
typedef void (*of_port_stats_cbk_fn)( 
                                      uint64_t controller_handle,
                                      uint64_t domain_handle,
                                      uint64_t datapath_handle,
                                      uint8_t  port_no,
                                      struct of_msg *msg,
                                      void  *cbk_arg1,
                                      void  *cbk_arg2,
                                      uint8_t status,
                                      struct ofi_port_stats_info *port_statistics,
                                      uint8_t more_ports);

/** \ingroup Port_Statistics 
 * \brief Send Port statistics request message.  \n
 *  <b>Description </b>\n
 *  API to get details of statistics of one or all ports present in the data_path. 
 * 
 *  The return value of OFU_PORT_STATISTICS_REQ_MESSAGE_LEN is used to allocate \n
 *  length of the message required to send get port statistics.
 *  \param [in] msg                - Message descriptor with buffer allocated earlier to send request to get port
 *                                   statistics.  
 *  \param [in] datapath_handle    - Handle of the data_path to which we are sending the get request. 
 *  \param [in] port_no            - Port number to get port statistics, OFPP_ANY indicates gets 
 *                                   statistics of all the ports of data_path.
 *  \param [in] port_stats_cbk     - Callback function pointer to which pass port statistics.
 *  \param [in] cbk_arg1           - Application private information, arg1, that will be passed to error callback in case\n
 *                                   of any errors. Also same passed to callback functions in case of success.   \n
 *  \param [in] cbk_arg2           - Application private information, arg2, that will be passed to error callback in case\n
 *                                   of any errors. Also same passed to callback functions in case of success.   \n
 *  \param [out] xid               - Id assigned for the transaction. The same transaction Id will be passed to
 *                                   callback function. 
 *  \return OFU_INVALID_PARAMETER_PASSED
 *  \return OFU_GET_PORT_STATS_MSG_SUCCESS 
 */
int32_t
of_get_port_statistics(struct of_msg *msg,
                       uint64_t datapath_handle,
                       uint32_t port_no,
                       of_port_stats_cbk_fn port_stats_cbk, 
                       void *cbk_arg1,
                       void *cbk_arg2);


#if 0
/************** Port(s) description get API *********************/

//!Get Port  Description  APIs 
/*! This module contains APIs related to getting port description values. 
 *
 * APIs are provided to get port description.\n
 *
 * Callback function will be passed to of_get_port_description() API to 
 * receive port description details. 
 *
 * \sa msg_mem_mgmt
 *\addtogroup Port_Descripton_Get 
 */

/** \ingroup Port_Descripton_Get
 * OFU_PORT_DESCRIPTION_REQ_MESSAGE_LEN
 * - Returns length of message required to send 'get port description' message'. 
 */
#define OFU_PORT_DESCRIPTION_REQ_MESSAGE_LEN sizeof(struct ofp_multipart_request)

/** \ingroup Port_Descripton_Get
 * \struct ofi_port_desc_info
 * \brief port description details\n
 *  <b>Description </b>\n
 * Data strcture holding port description details, all the fields must be converted into \n
 * host byte order before calling callback functions of_port_desc_cbk_fn().\n
 */
struct ofl_port_desc_info {

/** Port number */
uint32_t port_no;

/** MAC address of the port */
uint8_t hw_addr[OFP_ETH_ALEN];

/** Port name Null-terminated */
char name[OFP_MAX_PORT_NAME_LEN]; 

/** Port configurations flags, Bitmap of OFPPC_* flags. */
uint32_t config; 

/** State of the port, Bitmap of OFPPS_ flags.*/ 
uint32_t state; 

/** Current port features,  Bitmaps of OFPPF_* that describe features. 
 * All bits zeroed if unsupported or unavailable.
 */
uint32_t curr; 

/** Features being advertised by the port,  Bitmaps of OFPPF_* that describe features. 
 * All bits zeroed if unsupported or unavailable. 
 */
uint32_t advertised; 

/** Features supported by the port,  Bitmaps of OFPPF_* that describe features.
 * All bits zeroed, if unsupported or unavailable.
 */ 
uint32_t supported; 

/**Features advertised by peer,  Bitmaps of OFPPF_* that describe features. 
 * All bits zeroed if unsupported or unavailable. 
 */
uint32_t peer; 

/** Current port bitrate in kbps. */
uint32_t curr_speed; 

/** Max port bitrate in kbps */
uint32_t max_speed; 
};

/** \ingroup Port_Description_Get
 * \typedef of_port_desc_cbk_fn 
 * \brief Callback function to receive port description 
 * <b>Description </b>\n
 * This callback function will be pased to of_get_port_description() API \n
 * to receive port description values for each port present in the data_path. \n
 * The port description are returned asynchronously at later pointer by using this callback.
 *
 * The callback function might be called multiple times with port description details of one\n
 * port at a time depending on the number of  ports available in the data_path.
 * \param [in] controller_handle  - Handle of the controller
 * \param [in] domain_handle      - Handle of the domain
 * \param [in] datapath_handle    - Handle of the data_path.
 * \param [in] msg                - Message descriptor with buffer contains Open Flow message holding port description
 *                                  received from data_path. Controller allocates the memory required to read 
 *                                  port description received from data_path. Once application done with port description 
 *                                  received, the free function pointer available in the message descriptor will be called
 *                                  to free the  memory. 
 * \param [in] port_no            - Port number to which passing port description values. 
 * \param [in] cbk_arg1           - First Application specific private info that passed in the API to
 *                                  get table statistics request.
 * \param [in] cbk_arg2           - Second Application specific private info that passed in the API to
 *                                  get table statistics request.
 * \param [in] status             - Flag indicated the status  of response to earlier multipart request.
 * \param [in] port_description   - Pointer to port description info. All the data is converted into host byte order.
 * \param [in] more_ports         - Boolean value , TRUE indicates, this is not final response and more ports details
 *                                  yet to come. FALSE means this is the last port response
 */
typedef void (*of_port_desc_cbk_fn)( struct of_msg *msg,
                                     uint64_t controller_handle,
                                     uint64_t domain_handle,
                                     uint64_t datapath_handle,
                                     uint8_t  port_no,
                                     void  *cbk_arg1,
                                     void  *cbk_arg2,
                                     uint8_t status,
                                     struct ofl_port_desc_info *port_description,
                                     uint8_t more_ports);

/** \ingroup Port_Description_Get 
 * \brief Send Port description request message.  \n
 *  <b>Description </b>\n
 *  API to get details of port description of all ports present in the data_path. 
 * 
 *  The return value of OFU_PORT_DESCRIPTION_REQ_MESSAGE_LEN is used to allocate \n
 *  length of the message required to send get port description message.
 *  \param [in] msg                - Message descriptor with buffer allocated earlier to send request to get port
 *                                   description.  
 *  \param [in] datapath_handle    - Handle of the data_path to which we are sending the get request. 
 *  \param [in] port_desc_cbk      - Callback function pointer to which pass port description.
 *  \param [in] cbk_arg1           - Application private information, arg1, that will be passed to error callback in case\n
 *                                   of any errors. Also same passed to callback functions in case of success.   \n
 *  \param [in] cbk_arg2           - Application private information, arg2, that will be passed to error callback in case\n
 *                                   of any errors. Also same passed to callback functions in case of success.   \n
 *  \return OFU_INVALID_PARAMETER_PASSED
 *  \return OFU_GET_PORT_DESC_MSG_SUCCESS 
 */
int32_t
of_get_port_description(struct of_msg *msg,
                       uint64_t datapath_handle,
                       of_port_desc_cbk_fn port_desc_cbk, 
                       void *cbk_arg1,
                       void *cbk_arg2);
#endif

/************** Queue statistics get api  *********************/

//!Get  Queue Statistics APIs 
/*! This module contains APIs related to getting queue statistics values. 
 *
 * Callback function will be passed to of_get_queue_statistics() API to 
 * receive queue statistics.
 *
 * \sa msg_mem_mgmt
 *\addtogroup Queue_Statistics 
 */

/** \ingroup Queue_Statistics
 * OFU_QUEUE_STATS_REQ_MESSAGE_LEN
 * - Returns length of message required to send 'get queue statistics' message'. 
 */
#define OFU_QUEUE_STATS_REQ_MESSAGE_LEN\
        (sizeof(struct ofp_multipart_request) + sizeof(struct ofp_queue_stats_request) )

/** \ingroup Queue_Statistics
 * \struct ofi_queue_stats
 * \brief queue statistic details\n
 *  <b>Description </b>\n 
 *  Data structure hold Queue statistics of given queue of given port,\n
 * all the fields must converted into host byte order before calling \n
 * of_queue_stats_cbk_fn()
 */
struct ofi_queue_stats {

     /** Number of bytes transmitted on given queue of given port */
     uint64_t tx_bytes; 

     /** Number of packets  transmitted  on given queue of given port */
     uint64_t tx_packets; 

     /** Number of packets dropped due to overrun on given queue of given port */
     uint64_t tx_errors;

     /** Queue has been alive in seconds. */
     uint32_t duration_sec; 

     /** Queue has been alive in nanoseconds beyond duration_sec. */
     uint32_t duration_nsec; 
};

/** \ingroup Queue_Statistics
 * \typedef of_queue_stats_cbk_fn 
 * \brief Callback function to receive queue statistics 
 * <b>Description </b>\n
 * This callback function will be pased to of_get_queue_statistics() API \n
 * to receive queue statistics for each queue of given port  present in the data_path. \n
 *
 * The queue statistics are returned asynchronously at later pointer by using this callback.
 *
 * The callback function might be called multiple times with statistics details of one\n
 * queue of given port at a time.
 * \param [in] controller_handle  - Handle of the controller
 * \param [in] domain_handle      - Handle of the domain
 * \param [in] datapath_handle    - Handle of the data_path.
 * \param [in] msg                - Message descriptor with buffer contains Open Flow message holding queue stastics
 *                                  received from data_path. Controller allocates the memory required to read 
 *                                  queue statistics received from data_path. Once application done with queue statistics 
 *                                  received, the free function pointer available in the message descriptor will be called
 *                                  to free the  memory. 
 * \param [in] queue_id           - Priority Queue ID to which passing statistics values. 
 * \param [in] port_no            - Port number to get all queueu statistics details. 
 * \param [in] cbk_arg1           - First Application specific private info that passed in the API to
 *                                  get table statistics request.
 * \param [in] cbk_arg2           - Second Application specific private info that passed in the API to
 *                                  get table statistics request.
 * \param [in] status             - Flag indicated the status  of response to earlier multipart request.
 * \param [in] queue_stats        - pointer to queue statistics details. All the data is converted into\n
 *                                  host byte order.
 * \param [in] more_queues        - Boolean value , TRUE indicates, this is not final response and more queue details
 *                                  yet to come. FALSE means this is the last queue response
 */
typedef void (*of_queue_stats_cbk_fn)( struct of_msg *msg,
                                     uint64_t controller_handle,
                                     uint64_t domain_handle,
                                     uint64_t datapath_handle,
                                     uint8_t  table_Id,
                                     void  *cbk_arg1,
                                     void  *cbk_arg2,
                                     uint8_t status,
                                     uint32_t no_of_queues,
                                     struct ofi_queue_stats  *queue_stats,
                                     uint8_t more_queues);

/** \ingroup Queue_Statistics 
 * \brief Send queue statistics request message.\n
 *  <b>Description </b>\n
 *  The return value of OFU_QUEUE_STATS_REQ_MESSAGE_LEN is used to allocate \n
 *  length of the message required to send get queue statistics. 
 *  \param [in] msg             - Message descriptor that allocated  earlier to get queue statistics.
 *  \param [in] datapath_handle - Handle of the data_path to which we are sending the get request. 
 *  \param [in] port_no         - Port number to get queue statistics, OFPP_ANY indicates gets \n
 *                                statistics of all the ports of data_path.
 *  \param [in] queue_id        - Id of priority queue that attached to port, OFPQ_ALL indicates gets \n
 *                                statistics of all priority queues of port.
 *  \param [in] queue_stats_cbk - Callback function pointer to which queue statistics passed.
 *  \param [in] cbk_arg1        - Application private information, arg1, that will be passed to error callback in case\n
 *                                of any errors. Also same passed to callback functions in case of success.   \n
 *  \param [in] cbk_arg2        - Application private information, arg2, that will be passed to error callback in case\n
 *                                of any errors. Also same passed to callback functions in case of success.   \n
 *  \return OFU_INVALID_PARAMETER_PASSED
 *  \return OFU_GET_QUEUE_STATS_MSG_SUCCESS 
 */
int32_t
of_get_queue_statistics(struct of_msg *msg,
                        uint64_t datapath_handle,
                        uint32_t port_no,
                        uint32_t queue_id,
                        of_queue_stats_cbk_fn queue_stats_cbk, 
                        void *cbk_arg1,
                        void *cbk_arg2);

/************** GROUP features get api  *********************/

//!Group Feature APIs 
/*! This module contains APIs related to getting group features . 
 *
 * Callback function will be passed to of_get_groups_features() API to 
 * receive group features.
 *
 * Different utility functions are provided to parse and get details of\n
 * group features returned by data_path.
 *
 * \sa msg_mem_mgmt
 *\addtogroup Group_Features 
 */

/** \ingroup Group_Features
 * OFU_GROUP_FEATURES_REQ_MESSAGE_LEN
 * - Returns length of message required to send 'get group features' message'. 
 */
#define OFU_GROUP_FEATURES_REQ_MESSAGE_LEN sizeof(struct ofp_multipart_request)

/** \ingroup Group_Features
 * \struct ofi_group_features_info
 * \brief Group Features details\n
 *  <b>Description </b>\n 
 *  Data structure to hold group feataures supportes by switch, all fields are converted into \n
 *  host byte order before calling callback of_group_features_cbk_fn().
 *
 *  This structure need to pass it to different utility functions to parse and get\n
 *  group features details.
 */
struct ofi_group_features_info
{
   /** Group types All, Select, FF, and indirect supported by switch*/ 
   uint32_t types; 

   /** Group capabilities, Bitmap of OFPGFC_* capability supported 
    *  Utility functions are provided to parse and get details.
    */
   uint32_t capabilities; 

   /** Maximum number of groups supported for each type 
    *  Utility functions are provided to parse and get details.
    */
   uint32_t max_groups[4]; 

   /** Actions supported by each group type, Bitmaps of OFPAT_* that are supported.,
    *  Utility functions are provided to parse and get details.
    */
   uint32_t actions[4]; 
};

/** \ingroup Group_Features 
 * \typedef of_group_features_cbk_fn 
 * \brief Callback function to receive group features 
 * <b>Description </b>\n
 * This callback function will be pased to of_get_groups_features() API \n
 * to receive queue statistics for each queue of given port  present in the data_path. \n
 *
 * The group features are returned asynchronously at later pointer by using this callback.
 * \param [in] controller_handle  - Handle of the controller
 * \param [in] domain_handle      - Handle of the domain
 * \param [in] datapath_handle    - Handle of the data_path.
 * \param [in] msg                - Message descriptor with buffer contains Open Flow message holding group features
 *                                  received from data_path. Controller allocates the memory required to read 
 *                                  group features received from data_path. Once application done with group features 
 *                                  received, the free function pointer available in the message descriptor will be called
 *                                  to free the  memory. 
 * \param [in] xid                - Transaction Id of the get request issued earlier.
 * \param [in] cbk_arg1           - First Application specific private info that passed in the API to
 *                                  get table statistics request.
 * \param [in] cbk_arg2           - Second Application specific private info that passed in the API to
 *                                  get table statistics request.
 * \param [in] status             - Flag indicated the status  of response to earlier multipart request.
 * \param [in] group_features          - group features info.
 */
typedef void (*of_group_features_cbk_fn)(
                                         struct of_msg *msg,
				         uint64_t  controller_handle,
                                         uint64_t  domain_handle,
                                         uint64_t  datapath_handle,
                                         void     *cbk_arg1,
                                         void     *cbk_arg2,
                                         uint8_t   status,
                                         struct ofi_group_features_info *group_features);

/** \ingroup Group_Features 
 * \brief Get Group features request message.\n
 *  <b>Description </b>\n
 *  The return value of OFU_GROUP_FEATURES_REQ_MESSAGE_LEN is used to allocate \n
 *  length of the message required to send get group features. 
 *  \param [in] msg             - Message descriptor that allocated earlier to send get group
 *                                features request.
 *  \param [in] datapath_handle - Handle of the data_path to which we are sending the get request. 
 *  \param [in] group_features_cbk - Callback function pointer to pass group features.
 *  \param [in] cbk_arg1        - Application private information, arg1, that will be passed to error callback in case\n
 *                                of any errors. Also same passed to callback functions in case of success.   \n
 *  \param [in] cbk_arg2        - Application private information, arg2, that will be passed to error callback in case\n
 *                                of any errors. Also same passed to callback functions in case of success.   \n
 *  \param [out] xid            - Id assigned for the transaction. The same transaction Id will be passed to
 *                                callback function. 
 *  \return OFU_INVALID_PARAMETER_PASSED
 *  \return OFU_GET_GROUP_FEATURE_MSG_SUCCESS 
 */
int32_t
of_get_groups_features( struct of_msg             *msg,
                        uint64_t                  datapath_handle,
                        of_group_features_cbk_fn  group_features_cbk, 
                        void                      *cbk_arg1,
                        void                      *cbk_arg2,
                        uint32_t                  *xid);

/** \ingroup Group_Features 
 * \brief Utility function to know whether data_path supports group type 'All' or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - Group type 'All' supported 
 * \return FALSE - Group type "All' not supported 
 */
uint8_t
ofu_is_group_type_all_supported(struct of_msg *msg,
                                struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether data_path supports group type 'select' or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - Group type 'select' supported 
 * \return FALSE - Group type "select' not supported 
 */
uint8_t
ofu_is_group_type_select_supported(struct of_msg *msg,
                                   struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether data_path supports group type 'fast failover' or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - Group type 'fast failover' supported 
 * \return FALSE - Group type "fast failover' not supported 
 */
uint8_t
ofu_is_group_type_ff_supported(struct of_msg *msg,
                               struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether data_path supports group type 'indirect' or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - Group type 'indirect' supported 
 * \return FALSE - Group type "indirect' not supported 
 */
uint8_t
ofu_is_group_type_indirect_supported(struct of_msg *msg,
                                     struct ofi_group_features_info group_features);


/** \ingroup Group_Features 
 * \brief Utility function gets number of groups supported with type 'all'. 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return Number of groups supported for type 'all' 
 */
uint32_t
ofu_get_max_number_of_all_type_groups(struct of_msg *msg,
                                      struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function gets number of groups supported with type 'select'. 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return Number of groups supported for type 'select' 
 */
uint32_t
ofu_get_max_number_of_select_type_groups(struct of_msg *msg,
                                      struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function gets number of groups supported with type 'fast failover'. 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return Number of groups supported for type 'fast failover' 
 */
uint32_t
ofu_get_max_number_of_ff_type_groups(struct of_msg *msg,
                                  struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function gets number of groups supported with type 'select'. 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return Number of groups supported for type 'select' 
 */
uint32_t
ofu_get_max_number_of_indirect_type_groups(struct of_msg *msg,
                                        struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'weight' feature supported for select group or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - Weight feature is supported by select group 
 * \return FALSE - Weight feature is not supported by select group
 */
uint8_t
ofu_is_weight_support_for_select_group(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);
/** \ingroup Group_Features 
 * \brief Utility function to know whether 'liveness' feature supported for select group or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - liveness feature is supported by select group 
 * \return FALSE - liveness feature is not supported by select group
 */
uint8_t
ofu_is_liveness_support_for_select_group(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'output action' supported by  'select' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - Output actions is supported by select group 
 * \return FALSE - Output actions is not supported by select group
 */
uint8_t
ofu_is_select_group_support_output_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'group action' supported by  'select' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - group actions is supported by select group 
 * \return FALSE - group actions is not supported by select group
 */
uint8_t
ofu_is_select_group_support_group_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'set queue action' supported by  'select' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - set queue actions is supported by select group 
 * \return FALSE - set queue actions is not supported by select group
 */
uint8_t
ofu_is_select_group_support_set_queue_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'set mpls action' supported by  'select' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - set mpls actions is supported by select group 
 * \return FALSE - set mpls actions is not supported by select group
 */
uint8_t
ofu_is_select_group_support_set_mpls_ttl_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'decrement mpls action' supported by  'select' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - decrement mpls actions is supported by select group 
 * \return FALSE - decrement mpls actions is not supported by select group
 */
uint8_t
ofu_is_select_group_support_dec_mpls_ttl_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'set ipv4 action' supported by  'select' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - set ipv4 actions is supported by select group 
 * \return FALSE - set ipv4 actions is not supported by select group
 */
uint8_t
ofu_is_select_group_support_set_ipv4_ttl_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'decrement ipv4 action' supported by  'select' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - decrement ipv4 actions is supported by select group 
 * \return FALSE - decrement ipv4 actions is not supported by select group
 */
uint8_t
ofu_is_select_group_support_dec_ipv4_ttl_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'copy ttl outward action' supported by  'select' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - copy ttl action outward is supported by select group 
 * \return FALSE - copy ttl action outward is not supported by select group
 */
uint8_t
ofu_is_select_group_support_copy_ttl_outward_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'copy ttl inward action' supported by  'select' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - copy ttl action inward is supported by select group 
 * \return FALSE - copy ttl action inward is not supported by select group
 */
uint8_t
ofu_is_select_group_support_copy_ttl_inward_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'push vlan action' supported by  'select' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - push vlan action is supported by select group 
 * \return FALSE - push vlan action is not supported by select group
 */
uint8_t
ofu_is_select_group_support_push_vlan_header_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'pop vlan action' supported by  'select' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - pop vlan action is supported by select group 
 * \return FALSE - pop vlan action is not supported by select group
 */
uint8_t
ofu_is_select_group_support_pop_vlan_header_action(struct of_msg *msg,
                                                   struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'push pbb action' supported by  'select' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - push pbb action is supported by select group 
 * \return FALSE - push pbb action is not supported by select group
 */
uint8_t
ofu_is_select_group_support_push_pbb_header_action(struct of_msg *msg,
                                                   struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'pop pbb action' supported by  'select' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - pop pbb action is supported by select group 
 * \return FALSE - pop pbb action is not supported by select group
 */
uint8_t
ofu_is_select_group_support_pop_pbb_header_action(struct of_msg *msg,
                                                  struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'push mpls action' supported by  'select' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - push mpls action is supported by select group 
 * \return FALSE - push mpls action is not supported by select group
 */
uint8_t
ofu_is_select_group_support_push_mpls_header_action(struct of_msg *msg,
                                                    struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'pop mpls action' supported by  'select' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - pop mpls action is supported by select group 
 * \return FALSE - pop mpls action is not supported by select group
 */
uint8_t
ofu_is_select_group_support_pop_mpls_header_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'output action' supported by  'all' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - Output actions is supported by all group 
 * \return FALSE - Output actions is not supported by all group
 */
uint8_t
ofu_is_all_group_support_output_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'group action' supported by  'all' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - group actions is supported by all group 
 * \return FALSE - group actions is not supported by all group
 */
uint8_t
ofu_is_all_group_support_group_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'set queue action' supported by  'all' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - set queue actions is supported by all group 
 * \return FALSE - set queue actions is not supported by all group
 */
uint8_t
ofu_is_all_group_support_set_queue_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'set mpls action' supported by  'all' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - set mpls actions is supported by all group 
 * \return FALSE - set mpls actions is not supported by all group
 */
uint8_t
ofu_is_all_group_support_set_mpls_ttl_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'decrement mpls action' supported by  'all' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - decrement mpls actions is supported by all group 
 * \return FALSE - decrement mpls actions is not supported by all group
 */
uint8_t
ofu_is_all_group_support_dec_mpls_ttl_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'set ipv4 action' supported by  'all' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - set ipv4 actions is supported by all group 
 * \return FALSE - set ipv4 actions is not supported by all group
 */
uint8_t
ofu_is_all_group_support_set_ipv4_ttl_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'decrement ipv4 action' supported by  'all' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - decrement ipv4 actions is supported by all group 
 * \return FALSE - decrement ipv4 actions is not supported by all group
 */
uint8_t
ofu_is_all_group_support_dec_ipv4_ttl_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'copy ttl outward action' supported by  'all' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - copy ttl action outward is supported by all group 
 * \return FALSE - copy ttl action outward is not supported by all group
 */
uint8_t
ofu_is_all_group_support_copy_ttl_outward_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'copy ttl inward action' supported by  'all' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - copy ttl action inward is supported by all group 
 * \return FALSE - copy ttl action inward is not supported by all group
 */
uint8_t
ofu_is_all_group_support_copy_ttl_inward_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'push vlan action' supported by  'all' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - push vlan action is supported by all group 
 * \return FALSE - push vlan action inward is not supported by all group
 */
uint8_t
ofu_is_all_group_support_push_vlan_header_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'pop vlan action' supported by  'all' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - pop vlan action is supported by all group 
 * \return FALSE - pop vlan action inward is not supported by all group
 */
uint8_t
ofu_is_all_group_support_pop_vlan_header_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'push pbb action' supported by  'all' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - push pbb action is supported by all group 
 * \return FALSE - push pbb action inward is not supported by all group
 */
uint8_t
ofu_is_all_group_support_push_pbb_header_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'pop pbb action' supported by  'all' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - pop pbb action is supported by all group 
 * \return FALSE - pop pbb action inward is not supported by all group
 */
uint8_t
ofu_is_all_group_support_pop_pbb_header_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'push mpls action' supported by  'all' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - push mpls action is supported by all group 
 * \return FALSE - push mpls action inward is not supported by all group
 */
uint8_t
ofu_is_all_group_support_push_mpls_header_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'pop mpls action' supported by  'all' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - pop mpls action is supported by all group 
 * \return FALSE - pop mpls action inward is not supported by all group
 */
uint8_t
ofu_is_all_group_support_pop_mpls_header_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'output action' supported by  'fast failover' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - Output actions is supported by fast failover group 
 * \return FALSE - Output actions is not supported by fast failover group
 */
uint8_t
ofu_is_ff_group_support_output_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'group action' supported by  'fast failover' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - group actions is supported by fast failover group 
 * \return FALSE - group actions is not supported by fast failover group
 */

uint8_t
ofu_is_ff_group_support_group_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'set queue action' supported by  'fast failover' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - set queue actions is supported by fast failover group 
 * \return FALSE - set queue actions is not supported by fast failover group
 */
uint8_t
ofu_is_ff_group_support_set_queue_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'set mpls action' supported by  'fast failover' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - set mpls actions is supported by fast failover group 
 * \return FALSE - set mpls actions is not supported by fast failover group
 */
uint8_t
ofu_is_ff_group_support_set_mpls_ttl_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'decrement mpls action' supported by  'fast failover' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - decrement mpls actions is supported by fast failover group 
 * \return FALSE - decrement mpls actions is not supported by fast failover group
 */
uint8_t
ofu_is_ff_group_support_dec_mpls_ttl_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'set ipv4 action' supported by  'fast failover' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - set ipv4 actions is supported by fast failover group 
 * \return FALSE - set ipv4 actions is not supported by fast failover group
 */
uint8_t
ofu_is_ff_group_support_set_ipv4_ttl_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'decrement ipv4 action' supported by  'fast failover' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - decrement ipv4 actions is supported by fast failover group 
 * \return FALSE - decrement ipv4 actions is not supported by fast failover group
 */
uint8_t
ofu_is_ff_group_support_dec_ipv4_ttl_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'copy ttl outward action' supported by  'fast failover' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - copy ttl action outward is supported by fast failover group 
 * \return FALSE - copy ttl action outward is not supported by fast failover group
 */
uint8_t
ofu_is_ff_group_support_copy_ttl_outward_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'copy ttl inward action' supported by  'fast failover' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - copy ttl action inward is supported by fast failover group 
 * \return FALSE - copy ttl action inward is not supported by fast failover group
 */
uint8_t
ofu_is_ff_group_support_copy_ttl_inward_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'push vlan action' supported by  'fast failover' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - push vlan action is supported by fast failover group 
 * \return FALSE - push vlan action is not supported by fast failover group
 */
uint8_t
ofu_is_ff_group_support_push_vlan_header_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'pop vlan action' supported by  'fast failover' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - pop vlan action is supported by fast failover group 
 * \return FALSE - pop vlan action is not supported by fast failover group
 */
uint8_t
ofu_is_ff_group_support_pop_vlan_header_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'push pbb action' supported by  'fast failover' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - push pbb action is supported by fast failover group 
 * \return FALSE - push pbb action is not supported by fast failover group
 */
uint8_t
ofu_is_ff_group_support_push_pbb_header_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'pop pbb action' supported by  'fast failover' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - pop pbb action is supported by fast failover group 
 * \return FALSE - pop pbb action is not supported by fast failover group
 */
uint8_t
ofu_is_ff_group_support_pop_pbb_header_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'push mpls action' supported by  'fast failover' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - push mpls action is supported by fast failover group 
 * \return FALSE - push mpls action is not supported by fast failover group
 */
uint8_t
ofu_is_ff_group_support_push_mpls_header_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'pop mpls action' supported by  'fast failover' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - pop mpls action is supported by fast failover group 
 * \return FALSE - pop mpls action is not supported by fast failover group
 */
uint8_t
ofu_is_ff_group_support_pop_mpls_header_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'output action' supported by  'indirect' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - Output actions is supported by indirect group 
 * \return FALSE - Output actions is not supported by indirect group
 */
uint8_t
ofu_is_indirect_group_support_output_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'group action' supported by  'indirect' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - group actions is supported by indirect group 
 * \return FALSE - group actions is not supported by indirect group
 */
uint8_t
ofu_is_indirect_group_support_group_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'set queue action' supported by  'indirect' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - set queue actions is supported by indirect group 
 * \return FALSE - set queue actions is not supported by indirect group
 */
uint8_t
ofu_is_indirect_group_support_set_queue_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'set mpls action' supported by  'indirect' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - set mpls actions is supported by indirect group 
 * \return FALSE - set mpls actions is not supported by indirect group
 */
uint8_t
ofu_is_indirect_group_support_set_mpls_ttl_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'decrement mpls action' supported by  'indirect' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - decrement mpls actions is supported by indirect group 
 * \return FALSE - decrement mpls actions is not supported by indirect group
 */
uint8_t
ofu_is_indirect_group_support_dec_mpls_ttl_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'set ipv4 action' supported by  'indirect' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - set ipv4 actions is supported by indirect group 
 * \return FALSE - set ipv4 actions is not supported by indirect group
 */
uint8_t
ofu_is_indirect_group_support_set_ipv4_ttl_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'decrement ipv4 action' supported by  'indirect' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - decrement ipv4 actions is supported by indirect group 
 * \return FALSE - decrement ipv4 actions is not supported by indirect group
 */
uint8_t
ofu_is_indirect_group_support_dec_ipv4_ttl_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'copy ttl outward action' supported by  'indirect' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - copy ttl action outward is supported by indirect group 
 * \return FALSE - copy ttl action outward is not supported by indirect group
 */
uint8_t
ofu_is_indirect_group_support_copy_ttl_outward_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'copy ttl inward action' supported by  'indirect' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - copy ttl action inward is supported by indirect group 
 * \return FALSE - copy ttl action inward is not supported by indirect group
 */
uint8_t
ofu_is_indirect_group_support_copy_ttl_inward_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'push vlan action' supported by  'indirect' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - push vlan action is supported by indirect group 
 * \return FALSE - push vlan action is not supported by indirect group
 */
uint8_t
ofu_is_indirect_group_support_push_vlan_header_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'pop vlan action' supported by  'indirect' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - pop vlan action is supported by indirect group 
 * \return FALSE - pop vlan action is not supported by indirect group
 */
uint8_t
ofu_is_indirect_group_support_pop_vlan_header_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'push pbb action' supported by  'indirect' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - push pbb action is supported by indirect group 
 * \return FALSE - push pbb action is not supported by indirect group
 */
uint8_t
ofu_is_indirect_group_support_push_pbb_header_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'pop pbb action' supported by  'indirect' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - pop pbb action is supported by indirect group 
 * \return FALSE - pop pbb action is not supported by indirect group
 */
uint8_t
ofu_is_indirect_group_support_pop_pbb_header_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'push mpls action' supported by  'indirect' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - push mpls action is supported by indirect group 
 * \return FALSE - push mpls action is not supported by indirect group
 */
uint8_t
ofu_is_indirect_group_support_push_mpls_header_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/** \ingroup Group_Features 
 * \brief Utility function to know whether 'pop mpls action' supported by  'indirect' group bucket or not 
 * \param [in] msg - Message received from data_path contains group features\n
 *                   Same value was passed into callback of_group_features_cbk_fn. 
 * \param [in] group_features - Pointer to group features in the message.
 *                              Same value was passed into callback of_group_features_cbk_fn. 
 * \return TRUE  - pop mpls action is supported by indirect group 
 * \return FALSE - pop mpls action is not supported by indirect group
 */
uint8_t
ofu_is_indirect_group_support_pop_mpls_header_action(struct of_msg *msg,
                                       struct ofi_group_features_info group_features);

/************** GROUP statistics get api  *********************/

//!Group Statistics APIs 
/*! This module contains APIs related to getting group statistics. 
 *
 * Callback function will be passed to of_get_group_statistics() API to 
 * receive group statistics.
 *
 * Different utility functions are provided to parse and get details of\n
 * group statistics returned by data_path.
 *
 * \sa msg_mem_mgmt
 *\addtogroup Group_Statistics 
 */

/** \ingroup Group_Statistics
 * OFU_GROUP_STATS_REQ_MESSAGE_LEN
 * - Returns length of message required to send 'get group statistics' message'. 
 */
#define OFU_GROUP_STATS_REQ_MESSAGE_LEN\
       ( sizeof(struct ofp_multipart_request) + sizeof(struct ofp_group_stats_request))

/** \ingroup Group_Statistics
 *  \struct ofi_group_stat
 *  \brief Group Statistics details\n
 *  <b>Description </b>\n 
 *  Data structure to hold group statistics supportes by switch, all fields are converted into \n
 *  host byte order before calling callback of_group_stats_cbk_fn().
 *
 *  This structure need to pass it to different utility functions to parse and get\n
 *  group statistics details.
 */
struct ofi_group_stat
{
    uint32_t group_id;       /* Group identifier. */
   /** Number of flows or groups that directly forward to this group */
   uint32_t ref_count;

   /** Number of packets processed by group */
   uint64_t packet_count; 

   /** Number of bytes processed by group */
   uint64_t byte_count; 

   /** Time that group has been alive in seconds */
   uint32_t duration_sec; 

   /** Time that group has been alive in nanoseconds beyond duration_sec */
   uint32_t duration_nsec; 

   /** Number of action buckets available in group*/
   uint32_t no_of_action_buckets; 

   /** Bucket stats, one can use utility APIs to get action bucket statistics*/
   void *bucket_stats;  

   /** Internal variable used in get next API*/
   uint32_t action_bucket_curr; 

   /** Internal variable used in get next API*/
   void *bucket_stats_curr;  
};

/** \ingroup Group_Statistics 
 * \typedef of_group_stats_cbk_fn 
 * \brief Callback function to receive group statistics 
 * <b>Description </b>\n
 * Callback function to pass group statistics details. This is response
 * to earlier call of_get_group_statistics() to get group statistics of data_path. 
 *
 * This callback might be called  multiple times to pass all groups statistics.\n
 * However statistics details of only one group are passed for each call.\n
 * However it will be called only once with requested group Id details,if specific\n
 * gorup Id statistics details are requested.
 * \param [in] controller_handle  - Handle of the controller
 * \param [in] domain_handle      - Handle of the domain
 * \param [in] datapath_handle    - Handle of the data_path.
 * \param [in] group_id           - Group Id to which the statistics details are sent.
 * \param [in] msg                - Message descriptor with buffer contains Open Flow message holding group stastics
 *                                  received from data_path. Controller allocates the memory required to read 
 *                                  group statistics received from data_path. Once application done with group statistics 
 *                                  received, the free function pointer available in the message descriptor will be called
 *                                  to free the  memory. 
 * \param [in] cbk_arg1           - First Application specific private info that passed in the API to
 *                                  get table statistics request.
 * \param [in] cbk_arg2           - Second Application specific private info that passed in the API to
 *                                  get table statistics request.
 * \param [in] status             - Flag indicated the status  of response to earlier multipart request.
 * \param [in] group_stats        - Pointer to group statistics info. Different utility functions are provided\n 
 *                                  to parse and get details.
 * \param [in] more_groups        - Boolean flag, TRUE indicates still more group statistics are pending. FALSE used while\n
 *                                   passing last group.
 */
typedef void (*of_group_stats_cbk_fn)( struct of_msg *msg,
                                       uint64_t  controller_handle,
                                       uint64_t  domain_handle,
                                       uint64_t  datapath_handle,
                                       uint32_t  group_id,
                                       void     *cbk_arg1,
                                       void     *cbk_arg2,
                                       uint8_t   status,
                                       struct ofi_group_stat *group_stats,
                                       uint8_t   more_groups);

/** \ingroup Group_Statistics 
 * \brief Get Group statistics request message.\n
 *  <b>Description </b>\n
 *  The return value of OFU_GROUP_STATS_REQ_MESSAGE_LEN is used to allocate \n
 *  length of the message required to send get group statistics. 
 *  \param [in] msg             - Message descriptor that allocated earlier to send get group
 *                                statistics request.
 *  \param [in] datapath_handle - Handle of the data_path to which we are sending the get request. 
 *  \param [in] group_id        - Id of group to get statistics. OFPG_ALL to get statistics of groups\n
 *                                available in the data_path.
 *  \param [in] group_stats_cbk - Callback function pointer to pass group statistics.
 *  \param [in] cbk_arg1        - Application private information, arg1, that will be passed to error callback in case\n
 *                                of any errors. Also same passed to callback functions in case of success.   \n
 *  \param [in] cbk_arg2        - Application private information, arg2, that will be passed to error callback in case\n
 *                                of any errors. Also same passed to callback functions in case of success.   \n
 *  \return OFU_INVALID_PARAMETER_PASSED
 *  \return OFU_GET_GROUP_STATS_MSG_SUCCESS 
 */
int32_t
of_get_group_statistics( struct of_msg         *msg,
                         uint64_t              datapath_handle,
                         uint32_t              group_id,
                         of_group_stats_cbk_fn group_stats_cbk, 
                         void                  *cbk_arg1,
                         void                  *cbk_arg2);

/** \ingroup Group_Statistics 
 * \brief Get number of action buckets supported for the given group
 * \param [in] group_id    - Group Id to get number of buckets supported.
 * \param [in] msg         - Message received from the data_path contains group statistics details\n
 *                           Same was passed to of_group_stats_cbk_fn(). 
 * \param [in] group_stats - Pointer to group statistics, same is passed to\n
 *                           of_group_stats_cbk_fn().
 * \return Number of action buckets supported by the group
 */
uint8_t
ofu_get_no_of_action_buckets_of_group_supported(uint32_t        group_id,
                                                struct of_msg  *msg,
                                                struct ofi_group_stat *group_stats);

/** \ingroup Group_Statistics 
 * \brief Get first action bucket statistics  
 *  <b>Description </b>\n
 *  if OF_MORE_ACTION_BUCKETS_PRESENT is returned, application may call\n
 *  ofu_get_next_action_bucket_stats() to get next action bucket statistics.
 *
 * \param [in] group_id    - Group Id to get action bucket details. 
 * \param [in] msg         - Message received from the data_path contains group statistics details\n
 *                           Same was passed to of_group_stats_cbk_fn(). 
 * \param [in] group_stats - Pointer to group statistics, same is passed to\n
 *                           of_group_stats_cbk_fn().
 * \param [out] first_packet_count - Packt count of first bucket
 * \param [out] first_byte_count   - Byte count of first bucket
 * \return  OF_MORE_ACTION_BUCKETS_PRESENT
 * \return  OF_LAST_ACTION_BUCKET_STATS
 * \return  OF_GET_ACTION_BUCKET_STATS_ERROR
 */
int32_t
ofu_get_first_action_bucket_stats(uint32_t group_id,
                                  struct of_msg *msg,
                                  struct ofi_group_stat *group_stats,
                                  uint64_t *first_packet_count,
                                  uint64_t *first_byte_count);

/** \ingroup Group_Statistics 
 * \brief Get next action bucket statistics  
 *  <b>Description </b>\n
 *  It always returns next to previous buckets action bucket statistics.
 *
 *  if OF_MORE_ACTION_BUCKETS_PRESENT is returned, application may call\n
 *  ofu_get_next_action_bucket_stats() again to get next action bucket statistics.
 *
 * \param [in] group_id    - Group Id to get action bucket details. 
 * \param [in] msg         - Message received from the data_path contains group statistics details\n
 *                           Same was passed to of_group_stats_cbk_fn(). 
 * \param [in] group_stats - Pointer to group statistics, same is passed to\n
 *                           of_group_stats_cbk_fn().
 * \param [out] next_packet_count - Packt count of next bucket
 * \param [out] next_byte_count  - Byte count of next bucket
 * \return  OF_MORE_ACTION_BUCKETS_PRESENT
 * \return  OF_LAST_ACTION_BUCKET_STATS
 * \return  OF_GET_ACTION_BUCKET_STATS_ERROR
 */
int32_t
ofu_get_next_action_bucket_stats(uint32_t group_id,
                                 struct of_msg *msg,
                                 struct ofi_group_stat *group_stats,
                                 uint64_t *next_packet_count,
                                 uint64_t *next_byte_count);

/************** GROUP description get api  *********************/

//!Group Description APIs 
/*! This module contains APIs related to getting group description. 
 *
 * \sa msg_mem_mgmt
 *\addtogroup Group_Description 
 */

/** \ingroup Group_Description
 * OFU_GROUP_DESC_REQ_MESSAGE_LEN
 * - Returns length of message required to send 'get group description' message'. 
 */
#define OFU_GROUP_DESC_REQ_MESSAGE_LEN sizeof(struct ofp_multipart_request)
#define OFU_METER_CONFIG_REQ_MESSAGE_LEN sizeof(struct ofp_multipart_request)
#define OFU_METER_FEATURE_REQ_MESSAGE_LEN sizeof(struct ofp_multipart_request)
#define OFU_METER_STATS_REQ_MESSAGE_LEN sizeof(struct ofp_multipart_request)
#define OFU_SWINFO_REQ_MESSAGE_LEN  sizeof(struct ofp_multipart_request)

/** \ingroup Group_Description
 *  \struct ofi_group_desc_info
 *  \brief Group description details\n
 *  <b>Description </b>\n 
 *  Data structure to hold group description supportes by data_path, all fields are converted into \n
 *  host byte order before calling callback of_get_groups_description().
 */

/*Data structure group details, all fields are in host byte order*/
struct ofi_group_desc_info
{
   of_list_node_t     next_group;
   /** Group Identified*/
   uint32_t  group_id; 
   /** Group type, as 'Select' or 'all' or 'fast failover'  or 'indirect'*/
   uint8_t   group_type; 
   uint32_t  max_bucket_id; 
   of_list_t  bucket_list; 
};
#define OF_GROUP_DESC_LISTNODE_OFFSET offsetof(struct  ofi_group_desc_info, next_group)


struct ofi_bucket
{
   of_list_node_t     next_bucket;
   uint32_t bucket_id; 
   uint16_t weight;
   uint32_t watch_port;
   uint32_t watch_group;
   of_list_t  action_list;
};

#define OF_BUCKET_LISTNODE_OFFSET offsetof(struct ofi_bucket, next_bucket)
struct ofi_action
{
  of_list_node_t     next_action;
  uint16_t type;
  uint16_t sub_type;
  uint32_t  port_no;
  uint16_t  max_len;
  uint8_t  ttl;
  uint16_t ether_type;
  uint32_t  queue_id;
  uint32_t  group_id;
  uint32_t vendorid;            /*experimenter vendor id*/
  int8_t    setfield_type;
  uint8_t   ui8_data;
  uint16_t  ui16_data;
  uint32_t  ui32_data;
  uint64_t  ui64_data;
  uint8_t   ui8_data_array[6];
  ipv6_addr_t   ipv6_addr;
  uint16_t src_offset; /* Starting bit offset in source */
  uint16_t dst_offset; /* Starting bit offset in destination */
  uint32_t oxm_src_dst[2];
	


};
#define OF_ACTION_LISTNODE_OFFSET offsetof(struct ofi_action, next_action)


/** \ingroup Switch_Description 
 *  \struct ofi_meter_rec_info
 *  \brief Data structure meter details.\n
 *  <b>Description:</b>\n
 *  Data structure meter details, all fields are in host byte order.
 */
struct ofi_meter_rec_info
{
  /** Next node */
  of_list_node_t     next_meter;
   /** Meter instance. */
   uint32_t meter_id; 
   /** Bitmap of OFPMF_* flags. OFPMF_KBPS,OFPMF_PKTPS,OFPMF_BURST,OFPMF_STATS*/
   uint16_t flags; 
   /** list of meter_band */ 
   of_list_t  meter_band_list; 
};
#define OF_METER_REC_LISTNODE_OFFSET offsetof(struct ofi_meter_rec_info, next_meter)

/** \ingroup Switch_Description 
 *  \struct ofi_meter_band
 *  \brief Data structure meter band details.\n
 *  <b>Description:</b>\n
 *  Data structure meter band details.
 */
struct ofi_meter_band
{
   /** next band */
   of_list_node_t     next_band;
   /** Band instance. */
   uint32_t band_id; 
   /** OFPMBT_DSCP_REMARK. */
   uint16_t type; 
   /** Length in bytes of this band. */
   uint16_t len; 
   /** Rate for remarking packets. */
   uint32_t rate; 
   /** Size of bursts. */
   uint32_t burst_size; 
   /** Number of drop precedence level to add. */
   uint8_t prec_level; 
};
#define OF_METER_BAND_LISTNODE_OFFSET offsetof(struct ofi_meter_band, next_band)

/** \ingroup Switch_Description 
 *  \struct ofi_meter_features_info
 *  \brief Data structure meter features information.\n
 *  <b>Description:</b>\n
 *  Data structure meter features information.
 */
struct ofi_meter_features_info
{
  /** Maximum number of meters in the system */
  uint32_t max_meter;
  /** Meter band types supported in the system */
  uint32_t band_types;
  /** Meter Capabilities */
  uint32_t capabilities;
  /** Maximum bands per meters in the system */
  uint8_t max_bands;
  /** Maximum band rate supported for meters in the system */
  uint8_t max_color;
};

/** \ingroup Switch_Description 
 *  \struct ofi_meter_stats_info
 *  \brief Meter Statistics.\n
 *  <b>Description:</b>\n
 *  Data structure meter statistics.
 */
struct ofi_meter_stats_info
{
   /** Group identifier. */
   uint32_t meter_id;
   /** Number of flows that are bound to this meter */
   uint32_t flow_count;
   /** Number of packets in by meter. */ 
   uint64_t packet_in_count;
   /** Number of bytes in by meter. */
   uint64_t byte_in_count;
   /** Time that meter has been alive in seconds. */
   uint32_t duration_sec;
   /** Time that meter has been alive in nanoseconds beyond duration_sec. */
   uint32_t duration_nsec;
   /** Number of bands available in meter. */
   uint32_t no_of_bands;
   /** band stats, one can use utility APIs to get band statistics*/
   //void *band_stats;
   /** Number of current band count.  Used in get next API. */
   //uint32_t band_cnt_curr;
   /** Number of current band statistics. Used in getNext API. */
   //void *band_stats_curr;
};

/** \ingroup Group_Description 
 * \typedef of_group_stats_cbk_fn 
 * \brief Callback function to receive group description 
 * <b>Description </b>\n
 * Callback function to pass group description details. This is response
 * to earlier call of_get_groups_description() to get group description of data_path. 
 *
 * This callback might be called  multiple times to pass all groups description.\n
 * However description details of only one group are passed for each call.\n
 * \param [in] controller_handle  - Handle of the controller
 * \param [in] domain_handle      - Handle of the domain
 * \param [in] datapath_handle    - Handle of the data_path.
 * \param [in] group_id           - Group Id to which the description details are sent.
 * \param [in] msg                - Message descriptor with buffer contains Open Flow message holding group description
 *                                  received from data_path. Controller allocates the memory required to read 
 *                                  group description received from data_path. Once application done with group description 
 *                                  received, the free function pointer available in the message descriptor will be called
 *                                  to free the  memory. 
 * \param [in] cbk_arg1           - First Application specific private info that passed in the API to
 *                                  get table statistics request.
 * \param [in] cbk_arg2           - Second Application specific private info that passed in the API to
 *                                  get table statistics request.
 * \param [in] status             - Flag indicated the status  of response to earlier multipart request.
 * \param [in] group_desc         - Pointer to group description info.
 * \param [in] more_groups        - Boolean flag, TRUE indicates still more group description are pending. FALSE used while\n
 *                                   passing last group.
 */
typedef void (*of_group_desc_cbk_fn)(struct of_msg *msg,
                                       uint64_t  controller_handle,
                                       uint64_t  domain_handle,
                                       uint64_t  datapath_handle,
                                       void     *cbk_arg1,
                                       void     *cbk_arg2,
                                       uint8_t   status,
                                       struct ofi_group_desc_info *group_desc,
                                       uint32_t more_groups);
/** \ingroup Group_Description 
 * \brief Get Group description request message.\n
 *  <b>Description </b>\n
 *  Get description of all the groups present in the data_path.
 *
 *  Callback function of_group_desc_cbk_fn() will called mulltiples times to pass\n
 *  group description details each time with details of one group.
 *
 *  The return value of OFU_GROUP_DESC_REQ_MESSAGE_LEN is used to allocate \n
 *  length of the message required to send get group description. 
 *  \param [in] msg             - Message descriptor that allocated earlier to send get group
 *                                description request.
 *  \param [in] datapath_handle - Handle of the data_path to which we are sending the get request. 
 *  \param [in] group_desc_cbk  - Callback function pointer to pass group description.
 *  \param [in] cbk_arg1        - Application private information, arg1, that will be passed to error callback in case\n
 *                                of any errors. Also same passed to callback functions in case of success.   \n
 *  \param [in] cbk_arg2        - Application private information, arg2, that will be passed to error callback in case\n
 *                                of any errors. Also same passed to callback functions in case of success.   \n
 *  \param [out] xid            - Id assigned for the transaction. The same transaction Id will be passed to
 *                                callback function. 
 *  \return OFU_INVALID_PARAMETER_PASSED
 *  \return OFU_GET_GROUP_DESC_MSG_SUCCESS 
 */
int32_t
of_get_groups_description(struct of_msg       *msg,
                         uint64_t              datapath_handle,
                         of_group_desc_cbk_fn  group_desc_cbk, 
                         void                  *cbk_arg1,
                         void                  *cbk_arg2,
                         uint32_t              *xid);

/** \ingroup Switch Info Description
 *  \struct ofi_switch_info
 *  \brief Group description details\n
 *  <b>Description </b>\n 
 *  Data structure to hold group description supportes by data_path, all fields are converted into \n
 *  host byte order before calling callback of_get_groups_description().
 */

/*Data structure switch details, all fields are in host byte order*/
struct ofi_switch_info
{
  char mfr_desc[DESC_STR_LEN];       /* Manufacturer description. */
  char hw_desc[DESC_STR_LEN];        /* Hardware description. */
  char sw_desc[DESC_STR_LEN];        /* Software description. */
  char serial_num[SERIAL_NUM_LEN];   /* Serial number. */
  char dp_desc[DESC_STR_LEN];        /* Human readable description of data_path  */
};

typedef void (*of_switch_info_cbk_fn)(struct of_msg *msg,
                                      uint64_t  datapath_handle,
                                      uint8_t   status,
                                      struct ofi_switch_info *switch_info_desc);
int32_t
of_get_switch_info_through_channel(struct of_msg       *msg,
		uint64_t               datapath_handle,
		of_switch_info_cbk_fn  switch_info_cbk,
		void                  *cbk_arg1,
		void                  *cbk_arg2,
		uint32_t              *xid);

int32_t of_group_frame_response(char *msg, struct ofi_group_desc_info *group_entry_p, 
                                uint32_t msg_length);
int32_t of_group_frame_action_response(char *msg, struct ofi_action **action_entry_pp, uint32_t *length);

typedef void (*of_meter_config_cbk_fn)(struct of_msg *msg,
                                       uint64_t  controller_handle,
                                       uint64_t  domain_handle,
                                       uint64_t  datapath_handle,
                                       void     *cbk_arg1,
                                       void     *cbk_arg2,
                                       uint8_t   status,
                                       struct ofi_meter_rec_info *meter_config_rec,
                                       uint32_t more_meters);

typedef void (*of_meter_features_cbk_fn)( struct of_msg *msg,
                                       uint64_t  controller_handle,
                                       uint64_t  domain_handle,
                                       uint64_t  datapath_handle,
                                       void     *cbk_arg1,
                                       void     *cbk_arg2,
                                       uint8_t   status,
                                       struct ofi_meter_features_info *meter_features);

typedef void (*of_meter_stats_cbk_fn)( struct of_msg *msg,
                                       uint64_t  controller_handle,
                                       uint64_t  domain_handle,
                                       uint64_t  datapath_handle,
                                       void     *cbk_arg1,
                                       void     *cbk_arg2,
                                       uint8_t   status,
                                       uint32_t no_of_meters,
                                       struct ofi_meter_stats_info *meter_stats,
                                       uint8_t more_meters);



#if 0
/************** Meter features get api  *********************/

/*Data structure to hold meter feataures supportes by switch, all fields are in host byte order*/
struct ofi_meter_features_info
{
  /** Maximum number of meters in the system */
  uint32_t max_meter;
  /** Meter band types supported in the system */
  uint32_t band_types;
  /** Meter Capabilities */
  uint32_t capabilities;
  /** Maximum bands per meters in the system */
  uint8_t max_bands;
  /** Maximum band rate supported for meters in the system */
  uint8_t max_color;
}
/** \ingroup Switch_Description
 * \typedef of_meter_features_cbk_fn 
 * \brief Callback function that passes the meter features.
 * <b>Description </b>\n
 * Callback function to pass meter features. This is response
 * to earlier call to get meter features of meters present in data_path.
 * \param [in] msg                     -Message descriptor with buffer 
 *                                      contains Open Flow message holding 
 *                                      meter features received from data_path. 
 *                                      Controller allocates the memory 
 *                                      required to read meter features received 
 *                                      from data_path. Once application done 
 *                                      with meter features received, the free 
 *                                      function pointer available in the 
 *                                      message descriptor will be called
 *                                      to free the  memory.
 * \param [in] controller_handle       -Handle of the controller
 * \param [in] domain_handle           -Handle of the domain
 * \param [in] datapath_handle         -Handle of the data_path
 * \param [in] xid                     -Transaction Id of the meter features 
 *                                      get request issued earlier.
 * \param [in] cbk_arg1                -Application specific private info that
 *                                      is passed in the API to get meter 
 *                                      statistics
 * \param [in] cbk_arg2                -Application specific private info that
 *                                      is passed in the API to get meter 
 *                                      statistics
 * \param [in] status                  -Flag indicating the status of the 
 *                                      response to earlier multipart request.
 * \param [in] meter_features          -The meter features info
 */

typedef void (*of_meter_features_cbk_fn)( struct of_msg *msg,
                                       uint64_t  controller_handle,
                                       uint64_t  domain_handle,
                                       uint64_t  datapath_handle,
                                       uint32_t  xid,
                                       void     *cbk_arg1,
                                       void     *cbk_arg2,
                                       uint8_t   status,
                                       struct ofi_meter_features_info *meter_features);

/** \ingroup Switch_Description
 * \typedef of_get_meter_features 
 * \brief API to get the features of meters in the data_path.
 * <b>Description </b>\n
 * API to get the features of meters in the data_path. 
 * \param [in] msg                     -Message descriptor that is allocated
 *                                      earlier to send get meter features
 *                                      request. 
 * \param [in] datapath_handle         -Handle of the data_path to which the
 *                                      get request is being sent.
 * \param [in] meter_id                -Id of the meter to get the features.
 * \param [in] meter_features_cbk      -Callback function pointer to pass
 *                                      meter features.
 * \param [in] cbk_arg1                -Application specific private info 
 *                                      that will be passed to the callback.  
 *                                      In case of any error, error callback 
 *                                      function will be called by passing 
 *                                      this parameter along with cbk_arg2.
 * \param [in] cbk_arg2                -Application specific private info 
 *                                      that will be passed to the callback.  
 *                                      In case of any error, error callback 
 *                                      function will be called by passing 
 *                                      this parameter along with cbk_arg1.
 * \param [out] xid                    -Id assigned for the transaction.  The
 *                                      same transaction Id will be passed to
 *                                      callback function.
 * \return DPRM_INVALID_DATAPATH_HANDLE- Invalid datapath handle
 * \return OF_MSG_BUF_SIZE_IS_IN_SUFFICIENT - Message buffer is insufficient
 * \return OF_SEND_MSG_TO_DP_ERROR     - Sending Message to DP failed
 * \return OF_SUCCESS                  - Returned if the API is successful.
 */ 

int32_t
of_get_meter_features(struct of_msg *msg,
                        uint64_t                  datapath_handle,
                        uint32_t                  meter_id,
                        of_meter_features_cbk_fn  meter_features_cbk, 
                        void                     *cbk_arg1,
                        void                     *cbk_arg2,
                        uint32_t *xid);

/************** Meter statistics get api  *********************/

/*Data structure to hold  statistics, all the fields are in
  host byte order*/
 /** \ingroup Switch_Description
 * \struct ofi_meter_stats_info
 * \brief Switch Description Reply message\n
 * <b> Description </b>\n
 *  This Data structure holds the meter statistics information.  
 *  All the fields must be converted into host_byte order before 
 *  calling the callback of_meter_stats_cbk_fn() to  handover the 
 *  switch description reply message.
 *
 *  Separate Utility functions are provided to get meter stat values.
 */

/** \ingroup Switch_Description
 * \typedef of_meter_stats_cbk_fn 
 * \brief Callback function that passes the meter statistics.
 * <b>Description </b>\n
 * Callback function that passes the meter statistics.  This is response to 
 * a earlier call to get meter statistics of data_path.  This callback will 
 * be called multiple times with one meter statistics information each time.
 * However, it will be called only once with requested meter Id details if
 * specific group Id statistics details are requested.
 * \param [in] msg                     -Pointer to Message descriptor that 
 *                                      contains OpenFlow message holding the 
 *                                      meter statistics received from 
 *                                      data_path.  Controller allocates the 
 *                                      memory required to read meter stats 
 *                                      received from data_path.  Once the 
 *                                      application has done with the meter 
 *                                      statistics received, the free function
 *                                      pointer available in the message 
 *                                      descriptor will be called to free the 
 *                                      memory.
 * \param [in] controller_handle       -Handle of the controller
 * \param [in] domain_handle           -Handle of the domain
 * \param [in] datapath_handle         -Handle of the data_path
 * \param [in] cbk_arg1                -Application specific private info that
 *                                      is passed in the API to get meter 
 *                                      statistics
 * \param [in] cbk_arg2                -Application specific private info that
 *                                      is passed in the API to get meter 
 *                                      statistics
 * \param [in] status                  -Flag indicating the status of the 
 *                                      response to earlier multipart request.
 * \param [in] no_ofmeters             -The number of meters
 * \param [in] meter_stats             -The meter statistics info
 * \param [in] more_meters             -Boolean flag.  When this is set to
 *                                      TRUE, it indicates more meter statistics
 *                                      are pending and FALSE is set while 
 *                                      passing the last meter.
 */

typedef void (*of_meter_stats_cbk_fn)( struct of_msg *msg,
                                       uint64_t  controller_handle,
                                       uint64_t  domain_handle,
                                       uint64_t  datapath_handle,
                                       void     *cbk_arg1,
                                       void     *cbk_arg2,
                                       uint8_t   status,
                                       uint32_t no_of_meters,
                                       struct ofi_meter_stats_info *meter_stats
																			 uint8_t more_meters);

/*Utility function to returns number of bands supported for the given meter*/
/** \ingroup Switch_Description
 * \typedef of_get_no_of_bands_of_meter_supported 
 * \brief API to get the number of bands of the meter.
 * <b>Description </b>\n
 * API to get the number of bands of the given meter. 
 * \param [in] msg                     -Pointer to Message descriptor for 
 *                                      getting the number of bands.
 * \param [in] meter_stats             -Pointer to the meter band statistics
 *                                      structure that holds the data. 
 */
uint8_t
ofu_get_no_of_bands_of_meter_supported(struct of_msg *msg,
                                       ofi_meter_stats_info *meter_stats);

/** \ingroup Switch_Description
 * \typedef of_get_first_meter_band_stats 
 * \brief API to get the first meter band statistics.
 * <b>Description </b>\n
 * API to get the first meter band statistics. 
 * \param [in] msg                     -Pointer to Message descriptor for 
 *                                      getting the first meter band statistics.
 * \param [in] meter_stats             -Pointer to the meter band statistics
 *                                      structure that holds the data. 
 * \param [out] next_band_packet_count  -Gives the first band packet count.
 * \param [out] next_band_byte_count    -Gives the first band byte count. 
 * \return OF_MORE_DATA  (0)           -Indicates that there is more data.
 * \return OF_LAST_DATA  (1)           -Indicates that this is the last data.
 * \return OF_ERROR     (-1)           -Indicates that an error has occured.
 */
int32_t
ofu_get_first_meter_band_stats(struct of_msg *msg,
                               ofi_meter_stats_info *meter_stats,
                               uint64_t *first_band_packet_count,
                               uint64_t *first_band_byte_count);

/** \ingroup Switch_Description
 * \typedef of_get_next_meter_band_stats 
 * \brief API to get the next meter band statistics.
 * <b>Description </b>\n
 * API to get the next meter band statistics. 
 * \param [in] msg                     -Pointer to Message descriptor for 
 *                                      getting the next meter band statistics.
 * \param [in] meter_stats             -Pointer to the meter band statistics
 *                                      structure that holds the data. 
 * \param [out] next_band_packet_count  -Gives the next band packet count. 
 * \param [out] next_band_byte_count    -Gives the next band byte count. 
 * \return OF_MORE_DATA  (0)           -Indicates that there is more data.
 * \return OF_LAST_DATA  (1)           -Indicates that this is the last data.
 * \return OF_ERROR     (-1)           -Indicates that an error has occured.
 */
int32_t
ofu_get_next_meter_band_stats(struct of_msg *msg,
                                 ofi_meter_stats_info *meter_stats,
                                 uint64_t *next_band_packet_count,
                                 uint64_t *next_band_byte_count);
/** \ingroup Switch_Description
 * \typedef of_get_meter_statistics 
 * \brief API to get the meter statistics.
 * <b>Description </b>\n
 * API to get statistics of all meter or specific meter in the data_path. 
 * \param [in] msg              -Pointer to Message descriptor for getting 
 *                               the meter statistics.  The same memory is 
 *                               used earlier to send the meter statistics 
 *                               request. 
 * \param [in] datapath_handle  -Handle of the data_path to which the request
 *                               is being sent. 
 * \param [in] meter_id         -Id of the meter for which statistics are 
 *                               requested.
 * \param [in] meter_stats_cbk  -Callback function for passing the received 
 *                               meter statistics.
 * \param [in] cbk_arg1         -Application specific private info that is 
 *                               passed to the callback.  In case of of any
 *                               error, error callback function is called 
 *                               passing this parameter along with cbk_arg2.
 * \param [in] cbk_arg2         -Application specific private info that is 
 *                               passed to the callback.  In case of of any
 *                               error, error callback function is called 
 *                               passing this parameter along with cbk_arg1.
 * \param [out] xid             -Id assigned for the transaction.  The same
 *                               transaction Id will be passed to the 
 *                               callback function.
 */
int32_t
of_get_meter_statistics(struct of_msg *msg,
                        uint64_t               datapath_handle,
                        uint32_t               meter_id,
                        of_meter_stats_cbk_fn  meter_stats_cbk, 
                        void                  *cbk_arg1,
                        void                  *cbk_arg2,
                        uint32_t *xid);
  
#endif

#define OFU_INDIVIUAL_BIND_ENTRY_STATS_REQ_MESSAGE_LEN  (sizeof(struct fslx_multipart_request) + sizeof(struct fslx_bind_obj_info_request))

enum appl_type {
  MP_BIND_OBJ_TYPE_IPSEC_POLICY = 1,
  MP_BIND_OBJ_TYPE_IPSEC_SA,
  MP_BIND_OBJ_TYPE_FIREWALL
};

struct ipsec_sa_info {
  uint32_t spi;
  uint32_t dest_ip;
  uint8_t  protocol;
};

struct ofi_bind_obj_appl_info {
 uint8_t      direction; /* 0 for INBOUND, 1 for OUTBOUND */ 
 union{
     uint32_t policy_id;
     struct ipsec_sa_info sa;
   };
};

struct ofi_bind_entry_info
{   
    /** bind object Id */
    uint32_t bind_obj_id; 
    /** Opaque value that issued by controller */
    uint64_t cookie;

    /** Application Type of bind entry */
    uint8_t                       type;
    struct ofi_bind_obj_appl_info appl_info;
    /** number of flows in the bind object*/ 
    uint64_t number_of_flows;

    /**  flow that has been alive in seconds*/
   // uint32_t alive_sec; 

    /**  flow that has been alive in nano seconds*/
   // uint32_t alive_nsec; 


    /** Idle timeout value in seconds from original flow entry. */
   // uint16_t Idle_timeout;

    /** Hard timeout value in seconds from original flow entry. */
   // uint16_t hard_timeout; /* Hard timeout from original flow mod. */

    /** Number of packets that hit the flow. */
    uint64_t packet_count;

    /** Byte counts of packet that hit the flow */
    uint64_t byte_count;

    uint64_t error_count;

    uint64_t length;

    /** list of instructions of type struct ofi_instruction*/
    of_list_t  instruction_list;   

    /* list of instructions of type struct ofi_match_field*/
   // of_list_t  match_field_list;  
};


typedef void (*of_bind_stats_cbk_fn)( uint64_t controller_handle,
                                            uint64_t domain_handle,
                                            uint64_t datapath_handle,
                                            uint8_t  experimenter_Id,
                                            struct of_msg *msg,
                                            void  *cbk_arg1,
                                            void  *cbk_arg2,
                                            uint8_t status,
                                            struct ofi_bind_entry_info *flow_entry,
                                            uint8_t more_entries);
    

int32_t
of_get_bind_entries(struct of_msg *msg,
                    uint64_t datapath_handle,
                    uint8_t  bind_obj_id,
                    of_bind_stats_cbk_fn bind_entry_cbk_fn,
                    void *cbk_arg1,
                    void *cbk_arg2);





#endif /*OF_MULTIPART_H*/
