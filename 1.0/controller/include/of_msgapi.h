/** 
 **
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
#ifndef _OF_MSGAPI_H
#define _OF_MSGAPI_H
/*!\file of_msgapi.h 
 * \author Rajesh Madabushi   
 * \author Srini Addepalli   
 * \date Jan, 2013  
 * <b>Description:</b>\n 
 * This file contains Open FLow 1.3 protocol specific message formatting APIs\n.
 * This file provides APIs 
 *   - to format and send controller to datapath messages.
 *   - to register and receive asynchronous messages.
 *   - to format and send symmetric messages.
 *   - to register and receive symmetric messages.
 * Also provides different utility functions that help in formatting and parsing
 * of_messages.
 *
 * \sa of_utility.h
 **/

/*
 * Following are the steps for Applications to send messages to datapath
 * 1. Allocate message descriptor by using ofu_alloc_message() API.
 *    As part of APIs to send messages, steps are explained to find the length 
 *    of the message to allocate.
 * 2. Call all necessary utility APIs to build required parameters like match fields, instructions,
 *    actions, etc in the message buffer.
 * 3. And then call actual API to build Open Flow message and send message to datapath. 
 *\addtogroup Cntrl_To_Data_Path_intr Introduction 
 */

/** \ingroup Cntrl_To_Data_Path_intr
 * OF_RESPONSE_STATUS_SUCCESS
 * - Response code indicate that success in getting response  
 **/
#define OF_RESPONSE_STATUS_SUCCESS  1

/** \ingroup Cntrl_To_Data_Path_intr
 * OF_RESPONSE_STATUS_ERROR
 * - Response code indicate that error in getting response  
 **/
#define OF_RESPONSE_STATUS_ERROR    2 

/** \ingroup Cntrl_To_Data_Path_intr
 * OF_RESPONSE_STATUS_TIMEOUT 
 * - Response code indicate that earlier request was timed out  
 **/
#define OF_RESPONSE_STATUS_TIMEOUT  3

/***************** Switch Features ************************/

//!APIs related to switch feature request message
/*! This module contains  APIs for formatting and sending Feature Request message.
 *  It also contains utility APIs for parsing the feature reply message received
 *  from datapath.\n
 *
 *\addtogroup Feature_Request 
 **/

/** \ingroup Feature_Request
 * OFU_SWITCH_FEATURES_REQ_MESSAGE_LEN
 * - Returns length messages to send switch feature request message
 **/
#define OFU_SWITCH_FEATURES_REQ_MESSAGE_LEN   sizeof(struct ofp_header)

/** \ingroup  Feature_Request
 * \struct ofl_switch_features
 * \brief Switch Feature Reply message\n
 * <b> Description </b>\n
 *  Data_structure to hold switch feature reply, all the fields are converted into host_byte order\n
 *  before calling API to handover the feature reply message.
 *
 */
struct ofl_switch_features {
   /** Data path Id to which this feature response belongs*/
   uint64_t dpid;

   /** Maximum number of packets that can be buffered at data_path. This is applicable in case of\n
   *   data_path supports buffering. It's value is zero in case of no buffering support exists at\n
   *   the data_path and complete packet will be send to controller. */
   uint32_t n_buffers;

   /** Total number of tables that data_path supports */
   uint8_t  n_tables;

   /** Identifier used for the connection established between data_path and controller. In case\n
   *   of main connection its value is zero */
   uint8_t auxilary_id;

   /** Capablity flags supported by the data_path, Utility functions are provided to get details of\n
   *  the capabilities supported by the data_path */
    uint32_t capabilities;
};

/** \ingroup Feature_Request
 * \typedef of_switch_feature_reply_cbk_fn
 * \brief callback function to receive Feature Reply.
 *  <b>Description </b>\n
 *  Callback function to receive switch feature reply. This callback will be
 *  called in response to earlier switch feature request that happened as part of handshake
 *  between controller and data_path.
 * 
 * \param [in] controller_handle  -Handle of the controller
 * \param [in] domain_handle      -Handle of the domain
 * \param [in] datapath_handle    -Handle of the data_path.
 * \param [in] msg                -Message descriptor with buffer contains switch feature
 *                                 reply, this message is sending just for debugging purpose.
 *                                 controller allocates the memory required to read  response
 *                                 Once application done with response , the  free function
 *                                 available in the message descriptor will be called to free memory.
 * \param [in] cbk_arg1           -First Application specific private info that passed in the API to
 *                                 send switch feature request
 * \param [in] cbk_arg2           -Second Application specific private info that passed in the API to
 *                                 send switch feature request
 * \param [in] status             -Flag indicated the status  of response to earlier switch feature request.
 *                                 The status can be success or failure or timed out.
 * \param [in] switch_features    -Details of switch features received. Utility APIs are provided to
 *                                 get the capabilities of data_path.
 **/
typedef void (*of_switch_feature_reply_cbk_fn)(uint64_t  controller_handle,
                                               uint64_t  domain_handle,
                                               uint64_t  datapath_handle,
                                               struct    of_msg *msg,
                                               void     *cbk_arg1,
                                               void     *cbk_arg2,
                                               uint8_t  status,
                                               struct ofl_switch_features *switch_features);

/** \ingroup Feature_Request 
 * \brief Send switch feature request message.  \n
 *  <b>Description </b>\n
 *  Applications or controller itself sends  feature request message for every connection request from\n
 *  data_path. This is to understand data_path features and auxiliary ID of the channel on which data_path
 *  connected to the controller. 
 *
 *  The macro OFU_SWITCH_FEATURES_REQ_MESSAGE_LEN is used to calculate the length of the message\n
 *  buffer required for feature request message.
 *
 * \param [in] msg                  - Message descriptor with buffer allocated earlier to send switch feature request. 
 * \param [in] datapath_handle      - Handle of the data_path to which we are sending the switch feature request. 
 * \param [in] sw_feature_reply_cbk - Callback function to inform the reception of switch feature reply to earlier
 *                                    request.
 * \param [in] clbk_arg1            - Application private information, arg1, that will be passed to error callback in case
 *                                    of any errors. Also same passed to callback functions in case of success.   
 * \param [in] clbk_arg2            - Application private information, arg2, that will be passed to error callback in case
 *                                    of any errors. Also same passed to callback functions in case of success.   
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_SEND_FEATURE_REQ_MSG_SUCCESS
 */
int32_t
of_switch_feature_request_msg(struct of_msg *msg,
                              uint64_t  datapath_handle,
                              of_switch_feature_reply_cbk_fn sw_feature_reply_cbk,
                              void *clbk_arg1,
                              void *clbk_arg2);

/** \ingroup Feature_Request
 * \brief Utility function to know whether flow stats supported by data_path or not \n
 * \param [in] msg             - Message descriptor with buffer feature reply message.
 *                               As a part of callback functions the same  was handover to 
 *                               application earlier. 
 * \param [in] switch_features - Switch feature response message received from data_path. This is same
 *                               as switch feature values passed as part of callback function earlier. 
 * \return TRUE  - if data_path supports flow statistics
 * \return FALSE - if data_path does not supports flow statistics.
 */
uint8_t
ofu_is_flow_stats_supported(struct of_msg *msg,
                            struct ofl_switch_features *switch_features);

/** \ingroup Feature_Request 
 * \brief Utility function to know whether table stats supported by data_path or not \n
 * \param [in] msg             - Message descriptor with buffer feature reply message,
 *                               as part of callback functions this was  passed. 
 * \param [in] switch_features - Switch feature response message received from data_path. As
 *                               part of callback function this was passed earlier. 
 * \return TRUE  - if data_path supports table /statistics
u
 * \return FALSE - if data_path does not supports table statistics
 */
uint8_t
ofu_is_table_stats_supported(struct of_msg *msg,
                            struct ofl_switch_features *switch_features);

/** \ingroup Feature_Request 
 * \brief Utility function to know whether port stats supported by data_path or not \n
 * \param [in] msg             - Message descriptor with buffer feature reply message.
 *                               As part of callback functions this was  passed. 
 * \param [in] switch_features - Switch feature response message received from data_path, as
 *                               part of callback function this was passed earlier. 
 * \return TRUE  - if data_path supports port statistics
 * \return FALSE - if data_path doesn't supports port statistics
 */
uint8_t
ofu_is_port_stats_supported(struct of_msg *msg,
                            struct ofl_switch_features *switch_features);

/** \ingroup Feature_Request 
 * \brief Utility function to know whether queue stats supported by data_path or not \n
 * \param [in] msg             - Message descriptor with buffer feature reply message,
 *                               as part of callback functions this was  passed. 
 * \param [in] switch_features - Switch feature response message received from data_path, as
 *                               part of callback function this was passed. 
 * \return TRUE  - if data_path supports queue statistics
 * \return FALSE - if data_path does not supports queue statistics
 */
uint8_t
ofu_is_queue_stats_supported(struct of_msg *msg,
                            struct ofl_switch_features *switch_features);


/** \ingroup Feature_Request 
 * \brief Utility function to know whether re-assembly of IP fragments supported by data_path or not \n
 * \param [in] msg             - Message descriptor with buffer feature reply message,
 *                               as part of callback functions this was  passed. 
 * \param [in] switch_features - Switch feature response message received from data_path, as
 *                               part of callback function this was passed. 
 * \return TRUE  - if data_path supports re-assembly of IP fragments 
 * \return FALSE - if data_path does not supports re-assembly of IP fragments 
 */
uint8_t
ofu_is_ip_reasmbly_supported(struct of_msg *msg,
                             struct ofl_switch_features *switch_features);
/** \ingroup Feature_Request 
 * \brief Utility function to know whether blocking looping ports supported by data_path or not \n
 * \param [in] msg             - Message descriptor with buffer feature reply message,
 *                               as part of callback functions this was  passed. 
 * \param [in] switch_features - Switch feature response message received from data_path, as
 *                               part of callback function this was passed. 
 * \return TRUE  - if data_path supports blocking looping ports
 * \return FALSE - if data_path does not supports blocking looping ports
 */
uint8_t
ofu_is_port_block_supported(struct of_msg *msg,
                            struct ofl_switch_features *switch_features);

/************* SWitch Configuration Message  ***************************/

//!APIs related to switch configuration message. 
/*! This module contains  APIs for formatting and sending switch configuration messages.
 *  APIs exists for both get and set switch configuration values. 
 *  It also contains utility APIs for parsing the configuration reply message received
 *  from data_path.\n
 *
 *\addtogroup Switch_Config 
 **/

/** \ingroup Switch_Config 
 *  OFU_SWITCH_CONFIG_GET_REQ_MESSAGE_LEN
 *  - Return length of messsage that used to build switch configuration get message*/
#define OFU_SWITCH_CONFIG_GET_REQ_MESSAGE_LEN   sizeof(struct ofp_header)

/** \ingroup Switch_Config 
 *  OFU_SWITCH_CONFIG_SET_REQ_MESSAGE_LEN
 *  - Return length of messsage that used to build switch configuration set message*/
#define OFU_SWITCH_CONFIG_SET_REQ_MESSAGE_LEN   sizeof(struct ofp_switch_config)

/** \ingroup Switch_Config 
 * ofu_set_switch_to_frag_normal
 * \brief Set data_path IP fragmentation handling as normal \n
 * \param [in] msg - Message buffer allocated earlier to to send set switch configuration\n
 *                   message. Before calling API of_set_switch_config_msg() to format and\n
 *                   send set configuration message,this macro need to call to\n
 *                   set required configuration value. \n
 */
#define ofu_set_switch_to_frag_normal(msg)\
       ((struct ofp_switch_config *)msg->desc.start_of_data)->flags = OFPC_FRAG_NORMAL

/** \ingroup Switch_Config 
 * ofu_set_switch_to_drop_frags
 * \brief Set data_path IP fragmentation handling to drop it \n
 * \param [in] msg - Message buffer allocated earlier to to send set switch configuration\n
 *                   message. Before calling API of_set_switch_config_msg() to format and\n
 *                   send set configuration message,this macro need to call to\n
 *                   set required configuration value. \n
 */
#define ofu_set_switch_to_drop_frags(msg)\
       ((struct ofp_switch_config *)msg->desc.start_of_data)->flags = OFPC_FRAG_DROP

/** \ingroup Switch_Config 
 * ofu_set_switch_to_re_assmble_frags
 * \brief Set data_path IP fragmentation handling to perform IP re-assembly\n
 * \param [in] msg - Message buffer allocated earlier to to send set switch configuration\n
 *                   message. Before calling API of_set_switch_config_msg() to format and\n
 *                   send set configuration message,this macro need to call to\n
 *                   set required configuration value. \n
 */
#define ofu_set_switch_to_re_assmble_frags(msg)\
        ((struct ofp_switch_config *)msg->desc.start_of_data)->flags = OFPC_FRAG_REASM

#define ofu_set_switch_to_mask_frags(msg)\
        ((struct ofp_switch_config *)msg->desc.start_of_data)->flags = OFPC_FRAG_MASK
/** \ingroup Switch_Config 
 * ofu_set_miss_send_len_in_switch
 * \brief Set maximum length of packet the switch need to send it to controller.
 * <b>Description </b>\n
 * Set packet length for which data_path needs to send it to controller due to  miss exceptions\n
 * The cause for sending message to controller is other than output action to the OFPP_CONTROLLER.\n
 * \param [in] msg - Message buffer allocated earlier to to send set switch configuration\n
 *                   message. Before calling API of_set_switch_config_msg() to format and\n
 *                   send set configuration message,this macro  need to call to\n
 *                   set required configuration value. \n
 * \param [in] miss_send_len - Maximum length of the packet to send it to controller.
 *                             It is any value between 0 and 65535.
 *                             If the value is  OFPCML_NO_BUFFER the complete packet must 
 *                             be included in the message.
 */
#define ofu_set_miss_send_len_in_switch(msg,miss_send_len_p) \
        ((struct ofp_switch_config *)msg->desc.start_of_data)->miss_send_len = htons(miss_send_len_p)

/** \ingroup  Switch_Config
 * \struct ofl_switch_config
 * \brief Switch Confiugration Reply message, all fields are in host byte order\n
 */
struct ofl_switch_config
{
  /** Switch configuration flags, mainly indicates how to handle IP fragments,\n
   * Utility functions are provided to know how data_path is handling IP fragments */
  uint16_t flags; 
 /** Maximum bytes of packet data that send it to controller. OFPCML_NO_BUFFER means\n
  * send complete packet and should not be buffered.*/
  uint16_t miss_send_len;
};

/** \ingroup Switch_Config
 * \typedef of_switch_config_response_cbk_fn
 * \brief Callback function to receive switch configuration reply.
 * <b>Description </b>\n
 * Callback function to receive switch configuration . This callback will be
 * called in response to earlier switch configuration request.
 * \param [in] controller_handle  -Handle of the controller
 * \param [in] domain_handle      -Handle of the domain
 * \param [in] datapath_handle    -Handle of the data_path.
 * \param [in] msg                - Message descriptor with buffer contains switch configuration
 *                                  reply, this message is sending just for debugging purpose.
 *                                  controller allocates the memory required to read  response
 *                                  Once application done with response , the  free function
 *                                  available in the message descriptor will be called to free memory.
 * \param [in] cbk_arg1           - First Application specific private info that passed in the API to
 *                                  switch configuration request
 * \param [in] cbk_arg2           - Second Application specific private info that passed in the API to
 *                                  switch configuration request
 * \param [in] status             - Flag indicated the status  of response to earlier switch configuration request.
 *                                  The status can be success or failure or timed out. 
 * \param [in] switch_config     - Details of switch configuration values . Utility APIs are provided to
 *                                 get the configuration details of data_path.
 */
typedef void (*of_switch_config_response_cbk_fn)(uint64_t  controller_handle,
                                                uint64_t  domain_handle,
                                                uint64_t  datapath_handle,
                                                struct of_msg *msg,
                                                void     *cbk_arg1,
                                                void     *cbk_arg2,
                                                uint8_t  status,
                                                struct ofl_switch_config *switch_config);

/** \ingroup Switch_Config 
 * \brief Get IP fragmentation handling details \n
 * <b>Description </b>\n
 * Utitlity function get details data_path's IP fragmentation handling procedure.
 * It return configuration value w.r.t. IP re-assembly
 * \param [in] msg             - Message descriptor with buffer configuration reply message,\n
 *                               as part of callback functions this was  passed. 
 * \param [in] switch_config   - Switch configuration parameter values, as part of\n
 *                               callback functions this was  passed earlier. 
 * \return OFPC_FRAG_NORMAL    -  Indicates no special handling for fragments, fragments passed to pipeline as is
 * \return OFPC_FRAG_DROP      -  Indicates drop, if full packet is not received but received fragmented packet.
 * \return OFPC_FRAG_REASM     -  Indicates do re-assembly of all fragments before passing to pipeline, this\n 
 *                                requires switch to have capability of IP re-assembly operation.
 *                                That is ofu_is_ip_reasmbly_supported() should return TRUE.
 */
uint16_t
ofu_get_ip_frag_config(struct of_msg *msg,
                       struct ofl_switch_config *switch_config);

/** \ingroup Switch_Config 
 * \brief Get Switch configuration details\n
 * <b>Description </b>\n
 * API to send get switch configuration request message.  
 *
 * Use OFU_SWITCH_CONFIG_GET_REQ_MESSAGE_LEN for Length of message buffer required to get switch\n
 * configuration request.\n
 *
 * \param [in] msg                  - Message descriptor with buffer allocated earlier to send switch configuration request. 
 * \param [in] datapath_handle      - Handle of the data_path to which we are sending the switch configuration request. 
 * \param [in] sw_config_cbk        - Callback function to inform the reception of switch configuration reply.
 * \param [in] clbk_arg1            - Application private information, arg1, that will be passed to error callback in case
 *                                    of any errors. Also same passed to callback functions in case of success.   
 * \param [in] clbk_arg2            - Application private information, arg2, that will be passed to error callback in case
 *                                    of any errors. Also same passed to callback functions in case of success.   
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_SEND_SWITCH_CONFIG_REQ_MSG_SUCCESS
 */
int32_t
of_get_switch_config_msg(struct of_msg *msg,
                         uint64_t  datapath_handle,
                         of_switch_config_response_cbk_fn sw_config_cbk, 
                         void *clbk_arg1,
                         void *clbk_arg2);

/** \ingroup Switch_Config 
 * \brief Set Switch configuration details\n
 * <b>Description </b>\n
 * API to set switch configuration status. All the necessary utility functions\n
 * were expected call to change switch configuration parameter before actually\n
 * calling this API.
 * The macro OFU_SWITCH_CONFIG_SET_REQ_MESSAGE_LEN returns length of the message buffer need to allocate message\n
 * for sending switch configuration set message to data_path.\n
 * \param [in] msg                  - Message descriptor with buffer allocated earlier to send switch configuration set request. 
 * \param [in] datapath_handle      - Handle of the data_path to which we are sending the switch configuration set request. 
 * \param [in] clbk_command_arg1    - Application private information, arg1, that will be passed to error callback in case
 *                                    of any errors. 
 * \param [in] clbk_command_arg2    - Application private information, arg2, that will be passed to error callback in case
 *                                    of any errors. 
 * \return DPRM_INVALID_DATAPATH_HANDLE - Invalid data_path handle is passed.
 * \return OF_MSG_BUF_SIZE_IS_IN_SUFFECIENT - Message Buffer allocated is not sufficient
 * \return OF_SEND_MSG_TO_DP_ERROR   - Error in sending set configuration message to data_path.
 * \return OF_SUCCESS - Successfully send set switch configuration message.
 */
int32_t
of_set_switch_config_msg(struct    of_msg *msg,
                         uint64_t  datapath_handle,
                         void     *clbk_command_arg1,
                         void     *clbk_command_arg2,
			 void **conn_info_pp);

/****** FLOW ENTRY OPERATION APIs ************/

//!APIs related to flow entry operations 
/*! Provides APIs for add, modify, delete, modify exact and delete exact flow entries\n
 * of table. 
 *
 * Different utility functions are provided that need to call before actually calling 
 * these APIs to set flow entries.
 *
 * \sa msg_mem_mgmt
 * \sa Match_Fields
 * \sa Instructions
 *
 *\addtogroup Flow_Entry 
 **/

/** \ingroup Flow_Entry 
 *  OFU_ADD_OR_MODIFY_DELETE_FLOW_ENTRY_MESSAGE_LEN
 *  - Return length of message to add/modify/delete flow entry in the table*/
#define OFU_ADD_OR_MODIFY_OR_DELETE_FLOW_ENTRY_MESSAGE_LEN    (sizeof(struct ofp_flow_mod))

/** \ingroup Flow_Entry 
 * \brief Add an flow entry to the data_path table.\n
 * <b>Description </b>\n
 * The length of the message required to allocate message buffer to add  an flow entry is
 * length of message to add/modify/delete flow entry in the table                  
 * (OFU_ADD_OR_MODIFY_DELETE_FLOW_ENTRY_MESSAGE_LEN macro return value)            +
 * Total length of all the match field of the table entry                          +
 * Length of all instructions that are need to push for the flow entry
 * (this instruction length includes lengths of actions pushed to
 * apply_action_instruction or write_action_instruction) 
 * \param [in]  msg    -  Message descriptor with buffer allocated earlier for flow entry, 
 *                        One must call utility APIs to update with instruction set  and 
 *                        match fields before calling this API.
 * \param [in]  datapath_handle -  Handle of the data_path to which this flow entry need to add 
 * \param [in]  table_id        - Table ID to which this flow entry need to add
 * \param [in]  priority        - Priority of the flow entry. Lower the number higher the priority.
 * \param [in]  cookie          - An Opaque value selected to restrict matching of flow entry\n 
 * \param [in]  cookie_mask     - Mask to restrict bits in cookie in matching flow entries\n 
 * \param [in]  buffer_id       - Buffer ID, this is ID of the buffer maintained at data_path. As part of sending \n
 *                                packet_in message to controller, the data_path might be stored the packet in a local queue \n
 *                                and passed Buffer_Id, Applications are expected to remember the buffer Id and pass it back.\n
 * \param [in]  flags        - Action flags as defined by OFPFF_*.  
 * \param [in]  hard_timeout - Maximum life time of the  flow entry.
 * \param [in]  idle_timeout - Maximum Idle time.
 * \param [in]  clbk_command_arg1 - Application private information, arg1.In case of any errors,same will be passed to the\n
 *                                  error callback function that registered by the applications to receive errors.
 * \param [in]  clbk_command_arg2 - Application private information, arg2.In case of any errors,same will be passed to the\n
 *                                  error callback function that registered by the applications to receive errors.
 * \return DPRM_INVALID_DATAPATH_HANDLE
 * \return OFU_SEND_MSG_TO_DP_ERROR
 * \return OFU_ADD_FLOW_ENTRY_TO_TABLE_SUCCESS
 */
int32_t
of_create_add_flow_entry_msg(
                      struct of_msg *msg,
                      uint64_t  datapath_handle,
                      uint8_t   table_id,
                      uint16_t  priority,
		              uint8_t   command,
                      uint64_t  cookie,
                      uint64_t  cookie_mask,
                      uint32_t  buffer_id,
                      uint16_t  flags,
                      uint16_t  hard_timeout,
                      uint16_t  idle_timeout,
	              void **conn_info_pp);

/** \ingroup Flow_Entry 
 * \brief Modify an flow entry of the data_path table.\n
 * <b>Description </b>\n
 * The length of the message required to allocate message buffer for modify entry is
 * length of message to add/modify/delete flow entry in the table                  
 * (OFU_ADD_OR_MODIFY_DELETE_FLOW_ENTRY_MESSAGE_LEN macro return value)            +
 * Total length of all the match field of the table entry                          +
 * Length of new instructions that are need to push for the flow entry
 * (this instruction length includes lengths of actions pushed to
 * apply_action_instruction or write_action_instruction) 
 * \param [in]  msg    -  Message descriptor with buffer allocated earlier for flow entry, 
 *                        One must call utility APIs to update with instruction set  and 
 *                        match fields before calling this API.
 * \param [in]  datapath_handle -  Handle of the data_path to which this flow entry need to modify 
 * \param [in]  table_id        - Table ID to which this flow entry need to modify
 * \param [in]  cookie_mask,    -  Cookie mask is use to match the flow entries. the details of cookie_mask, defined in Open Flow 1.3 spec.  
 * \param [in]  clbk_command_arg1 - Application private information, arg1.In case of any errors,same will be passed to the\n
 *                                  error callback function that registered by the applications to receive errors.
 * \param [in]  clbk_command_arg2 - Application private information, arg2.In case of any errors,same will be passed to the\n
 *                                     error callback function that registered by the applications to receive errors.
 * \return DPRM_INVALID_DATAPATH_HANDLE
 * \return OFU_SEND_MSG_TO_DP_ERROR
 * \return OFU_MODIFY_FLOW_ENTRY_TO_TABLE_SUCCESS
 */
int32_t
of_modify_flow_entries_of_table( struct of_msg *msg,
                                 uint64_t  datapath_handle,
                                 uint8_t   table_id,
                                 uint16_t  flags,
                                 uint64_t  cookie,
                                 uint64_t  cookie_mask,
	              void **conn_info_pp);

/** \ingroup Flow_Entry 
 * \brief Modify an flow entry of the data_path table.\n
 * <b>Description </b>\n
 *  API to modify flow entry in the data_path table with strict version. All the \n
 *  instruction set of flow entries that are matched are modified with the instructions set passed with\n
 *  this API.\n
 *
 *  The Application must fill the message buffer 'msg' with match_fields and instructions set\n
 *  before calling this API.\n
 *
 *  This strict version modification API compared to 'of_modify_flow_entries_of_table' which is non-strict\n
 *  version.That is  in this version all match_fields, including their masks, and the priority, are strictly matched against \n
 *  the entry and  only an identical flow entry is modified.\n
 *
 *  The length of the message required to allocate message buffer for modify entry is\n
 *  length of message to add/modify/delete flow entry in the table                  
 *  (OFU_ADD_OR_MODIFY_DELETE_FLOW_ENTRY_MESSAGE_LEN macro return value)            +\n
 *  Total length of all the match field of the table entry                          +\n
 *  Length of all instructions that are need to push for the flow entry\n
 *  (this instruction length includes lengths of actions pushed to\n
 *  apply_action_instruction or write_action_instruction) 
 *  \param [in]  msg    -  Message descriptor with buffer allocated earlier for flow entry, 
 *                        One must call utility APIs to update with instruction set  and 
 *                        match fields before calling this API.
 *  \param [in]  datapath_handle -  Handle of the data_path to which this flow entry need to modify 
 *  \param [in]  table_id        - Table ID to which this flow entry need to modify
 *  \param [in]  priority        - Priority of flow entry to match. Lower the number higher the priority value.
 *  \param [in]  cookie_mask,    -  Cookie mask is use to match the flow entries. the details of cookie_mask,\n
 *                                  defined in Open Flow 1.3 spec.  
 *  \param [in]  clbk_command_arg1 - Application private information, arg1.In case of any errors,same will be passed to the\n
 *                                   error callback function that registered by the applications to receive errors.
 *  \param [in]  clbk_command_arg2 - Application private information, arg2.In case of any errors,same will be passed to the\n
 *                                  error callback function that registered by the applications to receive errors.
 *  \return DPRM_INVALID_DATAPATH_HANDLE
 *  \return OFU_SEND_MSG_TO_DP_ERROR
 *  \return OFU_MODIFY_FLOW_ENTRY_TO_TABLE_SUCCESS
 */
int32_t
of_create_modify_flow_entries_msg_of_table_with_strict_match(struct of_msg *msg,
                                                  uint64_t  datapath_handle,
                                                  uint8_t   table_id,
                                                  uint16_t  priority,
                                                  uint16_t  flags,
                                                  uint64_t  cookie,
                                                  uint64_t  cookie_mask,
	              void **conn_info_pp);

/** \ingroup Flow_Entry 
 * \brief Delete an flow entry of the data_path table.\n
 * <b>Description </b>\n
 * API to delete flow entry in the data_path table. All the flow entries that are matched are deleted
 * The length required to allocate message buffer for delete flow entry is
 * Length of message to add/modify/delete flow entry in the table                  
 *  (OFU_ADD_OR_MODIFY_DELETE_FLOW_ENTRY_MESSAGE_LEN macro return value)           +\n
 *  Total length of all the match field of the table entry           
 * \param [in]  msg    -  Message descriptor with buffer allocated earlier for flow entry, This buffer is
 *                        expected to update match fields by using the utility APIs.
 * \param [in]  datapath_handle -  Handle of the data_path to which this flow entry need to modify 
 * \param [in]  table_id        - Table ID to which this flow entry need to delete
 * \param [in]  cookie_mask,    -  Cookie mask is use to match the flow entries. the details of cookie_mask,\n
 *                                 defined in Open Flow 1.3 spec.  
 * \param [in] out_port        -  This is used to filter out the selection of flow entries to match for delete operation.
 *                                It can be ignore by passing 'OFPP_ANY'. If some port number is specified other than OFPP_ANY,\n
 *                                the port number will be used to select the matching flow entries.
 *                                It check whether the flow entry contain an output action directed at the specified port.
 *                                It matches the flow entries that had output action with port as out_port.\n
 * \param [in]  out_group      -  This is also used to filter out the selection of flow entries to match for delete operation.\n
 *                                It can be ignore by passing 'OFPG_ANY'. If some group number is specified other than OFPG_ANY\n,
 *                                then, it used to  matches the flow entries that had group action with group ID 'out_group'.\n
 * \param [in]  clbk_command_arg1 - Application private information, arg1.In case of any errors,same will be passed to the\n
 *                                  error callback function that registered by the applications to receive errors.
 * \param [in] clbk_command_arg2 - Application private information, arg2.In case of any errors,same will be passed to the\n
 *                                 error callback function that registered by the applications to receive errors.
 * \return DPRM_INVALID_DATAPATH_HANDLE
 * \return OFU_SEND_MSG_TO_DP_ERROR
 * \return OFU_DELETE_FLOW_ENTRY_TO_TABLE_SUCCESS
 */
int32_t
of_create_delete_flow_entries_msg_of_table( struct of_msg *msg,
                                 uint64_t  datapath_handle,
                                 uint8_t   table_id,
                                 uint16_t  flags,
                                 uint64_t  cookie,
                                 uint64_t  cookie_mask,
                                 uint32_t  out_port,
                                 uint32_t  out_group,
	              void **conn_info_pp);

/** \ingroup Flow_Entry 
 * \brief Delete an flow entry exact of the data_path table.\n
 * <b>Description </b>\n
 * All the flow entries that are matched are deleted
 * The length required to allocate message buffer for delete flow entry is
 *
 * This strict version deletion API as compared to 'of_create_delete_flow_entries_msg_of_table' which is non-strict
 * version.    That is  in this version all match_fields, including their masks, and the priority, 
 * are strictly matched against the entry,  and  only an identical flow entry is deleted.
 *
 * Length of message to add/modify/delete flow entry in the table                  
 *  (OFU_ADD_OR_MODIFY_DELETE_FLOW_ENTRY_MESSAGE_LEN macro return value)           +\n
 *  Total length of all the match field of the table entry           
 *
 * \param [in]  msg    -  Message descriptor with buffer allocated earlier for flow entry, This buffer is
 *                        expected to update match fields by using the utility APIs
 * \param [in]  datapath_handle -  Handle of the data_path to which this flow entry need to modify 
 * \param [in]  table_id        - Table ID to which this flow entry need to delete
 * \param [in]  priority        - Priority of the flow entry. Lower the number higher the priority.
 * \param [in]  cookie_mask     -  Coockie mask is use to match the flow entries. the details of cookie_mask,\n
 *                                 defined in Open Flow 1.3 spec.  
 * \param [in] out_port        -  This is used to filter out the selection of flow entries to match for delete operation.
 *                                It can be ignore by passing 'OFPP_ANY'. If some port number is specified other than OFPP_ANY,\n
 *                                then to select the matching flow entries  it check whether the flow entry contain an output\n
 *                                action directed at the specified port. It matchs the flow entries that had output action with\n
 *                                port as out_port.
 *\param [in]  out_group       -  This is also used to filter out the selection of flow entries to match for delete operation.\n
 *                                It can be ignore by passing 'OFPG_ANY'. If some group number is specified other than OFPG_ANY\n,
 *                                then to it matches the flow entries that had group action with group ID 'out_group'.\n
 * \param [in]  clbk_command_arg1 - Application private information, arg1.In case of any errors,same will be passed to the\n
 *                                  error callback function that registered by the applications to receive errors.
 * \param [in]  clbk_command_arg2 - Application private information, arg2.In case of any errors,same will be passed to the\n
 *                                  error callback function that registered by the applications to receive errors.
 *  \return DPRM_INVALID_DATAPATH_HANDLE
 *  \return OFU_SEND_MSG_TO_DP_ERROR
 *  \return OFU_DELETE_FLOW_ENTRY_TO_TABLE_SUCCESS
 */
int32_t
of_create_delete_flow_entries_msg_of_table_with_strict_match(struct of_msg *msg,
                                                  uint64_t  datapath_handle,
                                                  uint8_t   table_id,
                                                  uint16_t  priority,
                                                  uint16_t  flags,
                                                  uint64_t  cookie,
                                                  uint64_t  cookie_mask,
                                                  uint32_t  out_port,
                                                  uint32_t  out_group,
	              void **conn_info_pp);

/******  GROUP TABLE APIs ************/

//!APIs related to Group operations 
/*! Provides APIs for add, modify and delete group entries into group table 
 *
 * Different utility functions are provided that need to call before actually calling\n
 * these APIs
 *
 * \sa msg_mem_mgmt
 * \sa Action_Utilities
 * \sa Group_Utilities
 *\addtogroup Group_Operations 
 **/

/** \ingroup Group_Operations 
 * \brief Add an Group to Group Table.\n
 * <b>Description </b>\n
 * Add group to the data_path. Application must fill the message buffer descriptor 'msg' \n
 * with necessary action buckets before calling this API.\n
 *
 * Length required to allocate message buffer to add group to group table is
 * Length of the add/modify group message              +
 * Length of buckets attached to the group
 * (No_of_buckets *OFU_GROUP_ACTION_BUCKET_LEN)        + 
 * Lengths of all actions pushed to all the buckets separately) 
 * \param [in] msg      -  Message descriptor with buffer allocated earlier for adding group. 
 *                         This buffer is expected to update action buckets using utility APIs. 
 * \param [in] datapath_handle -  Handle of the data_path to add group.
 * \param [in] group_id        -  ID of the group to add.
 * \param [in] group_type      -  Type of group(All(OFPGT_ALL)/Select(OFPGT_SELECT)/Fast Fail over((OFGT_FF)) to add
 * \param [in] clbk_command_arg1 - Application private information, arg1.In case of any errors,same will be passed to the\n
 *                                 error callback function that registered by the applications to receive errors.
 * \param [in] clbk_command_arg2 - Application private information, arg2.In case of any errors,same will be passed to the\n
 *                                 error callback function that registered by the applications to receive errors.
 * \return OFU_INVALID_PARAMTERS_PASSED
 * \return OFU_ADD_GROUP_SUCCESS
 */
int32_t
of_add_group(struct of_msg *msg,
             uint64_t  datapath_handle,
             uint32_t  group_id,
             uint8_t   group_type,
	     void **conn_info_pp);

/** \ingroup Group_Operations 
 * \brief Modify an Group to Group Table.\n
 * <b>Description </b>\n
 * Modify group in  the data_path.The current actions buckets are \n
 * replaced with new action buckets.  Not only action buckets, Application  can modify the group type.\n
 * Application must fill the message buffer 'msg' with necessary action buckets before calling this API.\n
 *
 * Length required to allocate message buffer to modify group to group table is
 * Length of the add/modify group message              +
 * Length of buckets attached to the group
 * (No_of_buckets *OFU_GROUP_ACTION_BUCKET_LEN)        + 
 * Lengths of all actions pushed for each buckets separately) 
 * \param [in] msg      -  Message descriptor with buffer allocated earlier for modify group. 
 *                         This buffer is expected to update action buckets using utility APIs. 
 * \param [in] datapath_handle -  Handle of the data_path to modify group.
 * \param [in] group_id        -  ID of the group to modify.
 * \param [in] group_type      -  Type of group(All(OFPGT_ALL)/Select(OFPGT_SELECT)/Fast Fail over((OFGT_FF)) to modify
 * \param [in] clbk_command_arg1 - Application private information, arg1.In case of any errors,same will be passed to the\n
 *                                 error callback function that registered by the applications to receive errors.
 * \param [in] clbk_command_arg2 - Application private information, arg2.In case of any errors,same will be passed to the\n
 *                                 error callback function that registered by the applications to receive errors.
 * \return OFU_INVALID_PARAMTERS_PASSED
 * \return OFU_MODIFY_GROUP_SUCCESS
 */
int32_t
of_modify_group(struct of_msg *msg,
               uint64_t  datapath_handle,
               uint32_t  group_id,
               uint8_t   group_type,
		void **conn_info_pp);

/** \ingroup Group_Operations 
 * \brief Delete a Group from Group Table.\n
 * <b>Description </b>\n
 * Length required  to allocate  message  buffer to delete group from group table is
 * Length of the add/modify/delete group message itself
 * (OFU_ADD_OR_MODIFY_OR_DELETE_GROUP_MESSAGE)
 * \param [in] msg            -  Message descriptor with buffer allocated earlier for delete group. 
 * \param [in] datapath_handle -  Handle of the data_path to add group.
 * \param [in] group_id        -  ID of the group to delete.
 * \param [in] clbk_command_arg1 - Application private information, arg1.In case of any errors,same will be passed to the\n
 *                                 error callback function that registered by the applications to receive errors.
 * \param [in] clbk_command_arg2 - Application private information, arg2.In case of any errors,same will be passed to the\n
 *                                 error callback function that registered by the applications to receive errors.
 * \return OFU_INVALID_PARAMTERS_PASSED
 * \return OFU_DELETE_GROUP_SUCCESS
 */
int32_t
of_delete_group(struct of_msg *msg,
                uint64_t  datapath_handle,
                uint32_t  group_id,
		void **conn_info_pp);

/***************** PORT Modifiction message ********************/

/** \ingroup Port_Description 
 * OFU_PORT_STATUS_MODIFACTION_MESSAGE_LEN
 * - Returns  length of message buffer to modify port status
 **/
#define OFU_PORT_STATUS_MODIFACTION_MESSAGE_LEN   sizeof(struct ofp_port_mod)

/** \ingroup Port_Description 
 * \brief Port configuration modification message.\n
 * <b>Description </b>\n
 * API to modify port configuration status. Only physical configuration 
 * values can be modified.\n 
 * All the necessary utility functions were expected call to change port configuration\n
 * parameters before actually calling this API.\n
 *
 * Length of  message required to allocate port modification message is
 * return value of macro OFU_PORT_STATUS_MODIFACTION_MESSAGE_LEN
 *
 * \param [in]  port_no          - Port number to modify the configuration
 * \param [in]  datapath_handle  - Handle of the data_path for which port status is modified.
 * \param [in]  msg              - Message descriptor with buffer allocated earlier for modification
 *                                 of port configuration values.
 * \param [in] clbk_command_arg1 - Application private information, arg1.In case of any errors,same will be passed to the\n
 *                                 error callback function that registered by the applications to receive errors.
 * \param [in] clbk_command_arg2 - Application private information, arg2.In case of any errors,same will be passed to the\n
 *                                 error callback function that registered by the applications to receive errors.
 * \return OFU_INVALID_PARAMTERS_PASSED
 * \return OFU_MODIFY_PORT_SUCCESS
 */
int32_t
of_modify_port_status(uint32_t  port_no,
                      uint64_t  datapath_handle,
                      struct    of_msg *msg,
                      void      *clbk_command_arg1,
                      void      *clbk_command_arg2);

/******  METER TABLE APIs ************/

//!APIs related to Meter_Operations 
/*! Provides APIs for add, modify and delete meter entries to meter table 
 *
 * Different utility functions are provided that need to call before actually calling\n
 * these APIs.
 *
 * \sa msg_mem_mgmt
 * \sa Metering_Utilities
 *\addtogroup Meter_Operations 
 **/

/** \ingroup Meter_Operations 
 * \brief Add Meter to datapath.\n
 * <b>Description </b>\n
 * API to add a meter to meter table of data_path. Before calling this API, the application must\n
 * call meter utility functions to set meter configuration and add all the bands it wanted.
 *
 * Length required to allocate message buffer to add meter in meter table is\n
 * Length of the add/modify/deleter meter message               +\n
 * (Return value of OFU_ADD_OR_MODIFY_OR_DELETE_METER_MESSAGE)   
 * Length of all bands that added to the meter \n
 * (i.e., No_of_Meters * OFU_METER_BAND_LEN)  
 *
 * \param [in] msg - Message descriptor with buffer allocated earlier for addition of meter\n
 *                   and its  configuration values. 
 * \param [in] datapath_handle -  Handle of the data_path to which this meter  will be added.
 * \param [in]  meter_id - ID of the meter to add. It should not exceed OFPM_MAX(0xffff000).
 *                        It also can be any one of following virtual meters
 *                        OFPM_SLOWPATH    -  Meter for slow datapath, if any
 *                        OFPM_CONTROLLER  -  Meter for controller connection.
 *                        OFPM_ALL         -  Represents all meters 
 * \param [in] clbk_command_arg1 - Application private information, arg1.In case of any errors,same will be passed to the\n
 *                                 error callback function that registered by the applications to receive errors.
 * \param [in] clbk_command_arg2 - Application private information, arg2.In case of any errors,same will be passed to the\n
 *                                 error callback function that registered by the applications to receive errors.
 * \return OFU_INVALID_PARAMTERS_PASSED
 * \return OFU_ADD_METER_SUCCESS
 */
int32_t
of_add_meter_to_data_path (struct of_msg *msg,
                           uint64_t       datapath_handle,
                           uint32_t       meter_id,
                           void *clbk_command_arg1,
                           void *clbk_command_arg2);

/** \ingroup Meter_Operations 
 * \brief Modify Meter that was added earlier \n
 * <b>Description </b>\n
 * API to modify a meter in meter table of data_path. Before calling this API, the application must\n
 * call meter utility functions to set meter configuration and add all the bands it wanted.
 *
 * Length required to allocate message buffer to modify meter in meter table is\n
 * Length of the add/modify/deleter meter message  +\n 
 * Length of all bands that added to the meter \n
 * (i.e., No_of_Meters * OFU_METER_BAND_LEN)  
 *
 * \param [in] msg - Message descriptor with buffer allocated earlier for modification of meter\n
 *                   and its  configuration values. 
 * \param [in] datapath_handle -  Handle of the data_path to which this meter is modified.
 * \param [in] meter_id - ID of the meter to add. It should not exceed OFPM_MAX(0xffff000).
 *                        It also can be any one of following virtual meters
 *                        OFPM_SLOWPATH    -  Meter for slow datapath, if any
 *                        OFPM_CONTROLLER  -  Meter for controller connection.
 *                        OFPM_ALL         -  Represents all meters 
 * \param [in] clbk_command_arg1 - Application private information, arg1.In case of any errors,same will be passed to the\n
 *                                 error callback function that registered by the applications to receive errors.
 * \param [in] clbk_command_arg2 - Application private information, arg2.In case of any errors,same will be passed to the\n
 *                                 error callback function that registered by the applications to receive errors.
 * \return OFU_INVALID_PARAMTERS_PASSED
 * \return OFU_MODIFY_METER_SUCCESS
 */
int32_t
of_modify_meter_logical_switch (struct of_msg *msg,
                                uint64_t      datapath_handle,
                                uint32_t      meter_id,
                                void         *clbk_command_arg1,
                                void         *clbk_command_arg2);

/** \ingroup Meter_Operations 
 * \brief Delete Meter that was added earlier \n
 * <b>Description </b>\n
 * API to delete meter from meter table of data_path. 
 * The Length required  to allocate  message  buffer to delete meter from meter table is
 * Length of the add/modify/delete delete message itself.
 * (Return value of OFU_ADD_OR_MODIFY_OR_DELETE_METER_MESSAGE)   
 * \param [in] msg - Message descriptor with buffer allocated earlier for deletion of meter\n
 * \param [in] datapath_handle -  Handle of the data_path to which this meter  will be added.
 * \param [in] meter_id - ID of the meter to delete. 
 * \param [in] clbk_command_arg1 - Application private information, arg1.In case of any errors,same will be passed to the\n
 *                                 error callback function that registered by the applications to receive errors.
 * \param [in] clbk_command_arg2 - Application private information, arg2.In case of any errors,same will be passed to the\n
 *                                 error callback function that registered by the applications to receive errors.
 * \return OFU_INVALID_PARAMTERS_PASSED
 * \return OFU_DELETE_METER_SUCCESS
 */
int32_t
of_delete_meter_from_data_path (struct of_msg *msg,
                                uint64_t datapath_handle,
                                uint32_t  meter_id,
                                void *clbk_command_arg1,
                                void *clbk_command_arg2);

/************** PACKET_OUT APIs ******************/

//!Send packet to data_path API. 
/*! Provides API to send packet to data_path 
 *
 * Different utility function for adding actions should call before actually calling\n
 * the API to send packet.
 *
 * \sa msg_mem_mgmt
 * \sa Action_Utilities
 *\addtogroup Send_Packet_Out 
 **/

/** \ingroup Send_Packet_Out 
 *  OFU_PACKET_OUT_MESSAGE_LEN
 *  - Return length of packet out message, that used to send packet to data_path
 */ 
#define OFU_PACKET_OUT_MESSAGE_LEN sizeof(struct ofp_packet_out)

/** \ingroup Send_Packet_Out 
 * \brief Send packet to data_path\n
 * <b>Description </b>\n
 * API to send packet to data_path. controller can send either new  packet that generated locally\n
 * in the controller to data_path or packet that received as part of packet_in message earlier.  \n
 * Before calling this API, the application must call all the necessary utility functions to add actions\n
 * to message.\n
 *
 * Length required to allocate message buffer for packet out message is \n
 * Length of the packet out message                        
 * (return value of OFU_PACKET_OUT_MESSAGE_LEN)            + \n
 * Lengths of all the action in the action_list that are\n
 * sending with packet out message\n
 * \param [in] msg - Message descriptor with buffer allocated earlier for sending packet out message\n
 * \param [in] datapath_handle - Handle of the data_path to which sending packet.\n
 * \param [in] reply_pkt       - Boolean flag, TRUE indicates this is reply packet of earlier packet_in received.
 * \param [in] buffer_id       - The Id of the buffer used in the data_path . It is applicable only in\n
 *                               case of the  packet to data_path is same as that received earlier in the\n
 *                               packet_in message. \n
 *                               Actually as part of packet in message callback, controller\n
 *                               passed the buffer_id and it is expected to pass the same to this API.\n
 *                               In case of locally generated packet, this parameter is ignored.\n
 * \param [in] in_port - Ingress port that must be associated with the packet for Open Flow processing at data_path.\n
 *                  One can use special port OFPP_CONTROLLER indicates input port of packet received is controller\n
 *                  port.
 * \param [in] channel_no- The channel number on which packet received earlier. It is applicable only in case of the  packet to\n
 *                       data_path is same as that received earlier as part of packet_in message. \n
 *                       Actually as a part of packet in message callback, controller passed the channel number and it is expected\n
 *                       to pass the same as part of this API. In case of locally generated packet, this parameter is ignored.
 * \param [in] data_len - Length of the packet 
 * \param [in] conn_info - Connection info 
 * \param [in] data     - Pointer to the packet data.
 * \param [in] clbk_command_arg1 - Application private information, arg1.In case of any errors,same will be passed to the\n
 *                                 error callback function that registered by the applications to receive errors.
 * \param [in] clbk_command_arg2 - Application private information, arg2.In case of any errors,same will be passed to the\n
 *                                 error callback function that registered by the applications to receive errors.
 * \return OFU_INVALID_PARAMTERS_PASSED
 * \return OFU_SEND_PACKET_SUCCESS
 */
int32_t
of_add_data_to_pkt_and_send_to_dp(struct of_msg *msg,
                         uint64_t       datapath_handle,
			  void *conn_info,
			  uint16_t       data_len,
                         uint8_t       *data,
                         void          *clbk_command_arg1,
                         void          *clbk_command_arg2);

/****** Controller Role change  APIs ************/

//! Role Change request APIs. 
/*! This module contains APIs for formatting and sending role change request  message.
 *  APIs exists for both get and set controller role values. 
 *
 * \sa msg_mem_mgmt
 *\addtogroup Role_Change_Req 
 **/

/** \ingroup Role_Change_Req 
 *  OFU_ROLE_REQUEST_MESSAGE_LEN
 *  - Return length of role request message data_path*/
#define OFU_ROLE_REQUEST_MESSAGE_LEN sizeof(struct ofp_role_request)


/** \ingroup Role_Change_Req
 * \typedef of_controller
 * \brief Callback function to receive controller role change information. 
 * <b>Description </b>\n
 * Callback function to receive controller role change information. This callback will be
 * called in response to earlier role change request.
 * \param [in] controller_handle -Handle of the controller 
 * \param [in] domain_handle     -Handle of the domain 
 * \param [in] datapath_handle   -Handle of the data_path. 
 * \param [in] msg               -Message descriptor with buffer contains Open Flow role change\n
 *                                response message, this is sending just for debugging purpose.\n
 *                                controller allocates the memory required to read  response \n
 *                                Once application done with response , the  free function available in \n
 *                                the message descriptor will be called to free the  memory. \n
 * \param [in] cbk_arg1          -Application specific private info,arg1 that passed in the API to request \n
 *                                role change. \n
 * \param [in] cbk_arg2          -Application specific private info,arg2 that passed in the API to request \n
 *                                role change. \n
 * \param [in] status            - Flag indicated the status  of response to earlier role change request.\n
 *                                 The status can be success or failure or timed out.
 * \param [in] new_role          - Changed new or current controller role, it is any on one following value.\n
 *                                 OFPCR_ROLE_EQUAL    - All controllers are equal.\n
 *                                 OFPCR_ROLE_MASTER   - At most one master.\n
 *                                 OFPCR_ROLE_SLAVE    - Read-only access. \n
 */
typedef void (*of_controller_role_change_notification_fn)(uint64_t  controller_handle,
                                                         uint64_t  domain_handle,
                                                         uint64_t  datapath_handle,
                                                         struct of_msg *msg,
                                                         void     *cbk_arg1,
                                                         void     *cbk_arg2,
                                                         uint8_t   status,
                                                         uint32_t  new_role); 
/** \ingroup Role_Change_Req
 * \brief Send Role change request 
 * <b>Description </b>\n
 * API to send role change request message from controller. In case of success in\n
 * change of controller role in the data_path, new changed role will be passed ,\n
 * In case of failure of change controller role at data_path, along with calling\n
 * the callback function to inform the current role, the error callback function will be called.\n
 *
 * The macro OFU_ROLE_REQUEST_MESSAGE_LEN returns the length required to allocate message\n
 * buffer to send role change request message. 
 * \param [in] msg     - Message descriptor with buffer allocated earlier to send role change 
 *                       request message.
 * \param [in] datapath_handle - Handle of the data_path to which we are sending the controller role change request. 
 * \param [in] new_role        - New controller role to change,
 *                               Following are new roles are supported
 *                               OFPCR_ROLE_EQUAL    - All controllers are equal.
 *                               OFPCR_ROLE_MASTER   - At most one master.
 *                               OFPCR_ROLE_SLAVE    -  Read-only access. 
 * \param [in]  generation_id   - This is required in case of role change is master or slave. It identifies a given
 *                                mastership view, that used in master election mechanism as explained Open Flow
 *                                Ver 1.3.0 specification.
 * \param [in]  role_change_cbk_fn - controller role change notification.
 * \param [in]  clbk_command_arg1  - Application private information, arg1.In case of any errors,same will be passed to the\n
 *                                   error callback function that registered by the applications to receive errors.
 * \param [in]  clbk_command_arg2  - Application private information, arg2.In case of any errors,same will be passed to the\n
 *                                   error callback function that registered by the applications to receive errors.
 * \return OFU_INVALID_PARAMTERS_PASSED
 * \return OFU_SEND_ROLE_CHANGE_REQ_SUCCESS
 */
int32_t
of_send_role_change_msg(struct of_msg *msg,
                        uint64_t   datapath_handle,
                        uint32_t   new_role,
                        uint64_t   generation_id,
                        of_controller_role_change_notification_fn role_change_cbk_fn,
                        void      *clbk_command_arg1,
                        void      *clbk_command_arg2);

/** \ingroup Role_Change_Req
 * \brief Get current controller role 
 * <b>Description </b>\n
 * API to get current controller role. Callback function is passed to get current controller role.\n
 *
 * The macro OFU_ROLE_REQUEST_MESSAGE_LEN Length returns the length required to allocate message\n
 * buffer to get current role. 
 * \param [in] msg                 - Message descriptor with buffer allocated earlier to send get current role \n
 *                                   request message. Length required to allocate message buffer to role get message is \n
 *                                   Length of role request message that used to send packet to data_path.\n
 * \param [in] datapath_handle     - Handle of the data_path to which we are sending the controller role request. \n
 * \param [in] role_change_cbk_fn  - controller role change notification.
 * \param [in] clbk_command_arg1   - Application private information, arg1.In case of any errors,same will be passed to the\n
 *                                   error callback function that registered by the applications to receive errors.
 * \param [in] clbk_command_arg2   - Application private information, arg2.In case of any errors,same will be passed to the\n
 *                                   error callback function that registered by the applications to receive errors.
 * \return OFU_INVALID_PARAMTERS_PASSED
 * \return OFU_SEND_GET_CURR_ROLE_MSG_SUCCESS
 */
int32_t
of_get_role_info_msg(struct of_msg *msg,
                     uint64_t   datapath_handle,
                     of_controller_role_change_notification_fn role_change_cbk_fn,
                     void *clbk_command_arg1,
                     void *clbk_command_arg2);

/****** Get Queue Configuration APIs ************/

//!APIs related to get Queue Configuration values
/*! This module contains  APIs for formatting and sending queue configuration messages.
 *  It also contains utility APIs for parsing the configuration reply message received
 *  from data_path.\n
 *
 * \sa msg_mem_mgmt
 *\addtogroup Queue_Features 
 **/

/** \ingroup Queue_Features 
 *  OFU_QUEUE_CONFIG_REQUEST_MESSAGE_LEN
 *  - Return length of messsage to querry Queue Config message*/
#define OFU_QUEUE_CONFIG_REQUEST_MESSAGE_LEN   sizeof(struct ofp_queue_get_config_request)

/** \ingroup Queue_Features
 * \typedef of_queue_config_reply_cbk_fn
 * \brief Callback function to receive queue features. 
 * <b>Description </b>\n
 * Callback function  to pass features of all configured queues for the given port. \n
 * This callback will be called in response to earlier request on get queue features of a port. \n
 * Utility functions are used to parse and get queue configuration values.\n
 * \param [in] controller_handle -Handle of the controller 
 * \param [in] domain_handle     -Handle of the domain 
 * \param [in] datapath_handle   -Handle of the data_path. 
 * \param [in] msg        `      - Message descriptor with buffer contains list of all queue configuration\n
 *                                 values of given port. \n
 *                                 controller allocates the memory required to read  response.\n
 *                                 Once application done with response , the  free function available in \n
 *                                 the message descriptor will be called to free the  memory. \n
 * \param [in] cbk_arg1          - Application specific private info,arg1 that passed in the API to get \n
 *                                 queue configuration request. \n
 * \param [in] cbk_arg2          - Application specific private info,arg2 that passed in the API to get \n
 *                                 queue configuration request. \n
 * \param [in] status            - Flag indicated the status  of response to earlier queue configuration request.\n
 *                                 The status can be success or failure or timed out.
 * \param [in] port_no           - Port number to which queue configuration values were received.
 */
typedef void (*of_queue_config_reply_cbk_fn)(uint64_t  controller_handle,
                                             uint64_t  domain_handle,
                                             uint64_t  datapath_handle,
                                             struct of_msg *msg,
                                             void     *cbk_arg1,
                                             void     *cbk_arg2,
                                             uint8_t   status,
                                             uint32_t  port_no);

/** \ingroup Queue_Features
 * \brief Get current queue features of port 
 * <b>Description </b>\n
 * API to send queue configuration request.  One or more queues will attach to port. This will get back\n
 * configuration values of all the queues that attached to the port.
 *
 * The Macro OFU_QUEUE_CONFIG_REQUEST_MESSAGE_LEN returns length of the message required to allocate for
 * the messasge to get queue  configuration values.
 *
 * \param [in] msg                 - Message descriptor with buffer allocated earlier to send queue configuration
 *                                   get request. 
 * \param [in] datapath_handle     - Handle of the data_path to which we are sending the queue feature get request. \n
 * \param [in] port_no             - Port number to get queue configuration values.
 * \param [in] queue_config_cbk_fn - Callback function to pass queue configuration value of given port.
 * \param [in] clbk_arg1           - Application private information, arg1.In case of any errors,same will be passed to the\n
 *                                   error callback function that registered by the applications to receive errors.\n
 * \param [in] clbk_arg2           - Application private information, arg2.In case of any errors,same will be passed to the\n
 *                                   error callback function that registered by the applications to receive errors.\n
 * \return OFU_INVALID_PARAMTERS_PASSED
 * \return OFU_SEND_GET_QUEUE_CONFIG_MSG_SUCCESS
 */
int32_t
of_send_queue_config_request_msg(struct of_msg *msg,
                                 uint64_t  datapath_handle,
                                 uint32_t  port_no,
                                 of_queue_config_reply_cbk_fn queue_config_cbk_fn,
                                 void *clbk_arg1,
                                 void *clbk_arg2);

/** \ingroup Queue_Features
 * \brief Utility API to get number of queues attached to port\n
 *
 * \param [in] msg     - Message received from data_path contains queue configuration message.\n
 *                       It was received in callback of_queue_config_reply_cbk_fn earlier.
 * \param [in] port_no - Port number for which to get number of queues attached.
 *
 * \return No_of_queues_attached_to_the_port
 */
uint8_t
ofu_get_total_no_of_queues_attached(struct of_msg *msg,
                                    uint32_t port_no);

/** \ingroup Queue_Features
 * \brief Utility API to get queue property of a queue of given port
 *
 * \param [in] msg      - Message received from data_path contains queue configuration message.\n
 * \param [in] port_no  - Port number for which to get queue configuration values.\n
 * \param [in] queue_no - Queue number for which to get queue property. Queue number starts\n
 *                        with 0.
 *
 * \return OFPQT_MIN_RATE      - Minimum data rate guaranteed
 * \return OFPQT_MAX_RATE      - Maximum data rate
 * \return OFPQT_EXPERIMENTER  - Experimenter defined property.
*/
uint8_t
ofu_get_queue_properity(struct of_msg *msg,
                        uint32_t port_no,
                        uint8_t queue_no); 

/****** Barrier APIs ************/
//!Barrier message APIs 
/*! This module contains  APIs for formatting and sending barrier messages.
 *
 * \sa msg_mem_mgmt
 *\addtogroup Barrier_Message 
 **/

/** \ingroup Barrier_Message 
 *  OFU_BARRIER_REQUEST_MESSAGE_LEN
 *  - Return length message to send barrier request message*/
#define OFU_BARRIER_REQUEST_MESSAGE_LEN   sizeof(struct ofp_header)

/** \ingroup Barrier_Message
 * \typedef of_barrier_reply_notification_fn
 * \brief Callback function to indicate barrier response received. 
 * <b>Description </b>\n
 * Callback function indicate barrier response message received. This callback will be\n
 * called in response to earlier barrier request. This callback resumes the sending \n
 * application messages to data_path.\n
 * \param [in] controller_handle -Handle of the controller 
 * \param [in] domain_handle     -Handle of the domain 
 * \param [in] datapath_handle   -Handle of the data_path. 
 * \param [in] msg               - Message descriptor with buffer contains barrier reply\n
 *                                 message, this msg is sending just for debugging purpose.\n
 *                                 controller allocates the memory required to read  response \n
 *                                 Once application done with response , the  free function available in \n
 *                                 the message descriptor will be called to free the  memory. \n
 * \param [in] cbk_arg1          - Application specific private info,arg1 that passed in the API to send \n
 *                                 barrier request message.
 *                                 queue configuration request. \n
 * \param [in] cbk_arg2          - Application specific private info,arg2 that passed in the API to send \n
 *                                 barrier request message.
 * \param [in] status            - Flag indicated the status  of response to earlier barrier message.\n
 *                                 The status can be success or failure or timed out.
 
 */
typedef void (*of_barrier_reply_notification_fn)(uint64_t  controller_handle,
                                                 uint64_t  domain_handle,
                                                 uint64_t  datapath_handle,
                                                 struct of_msg *msg,
                                                 void     *cbk_arg1,
                                                 void     *cbk_arg2,
                                                 uint8_t   status);

/** \ingroup Barrier_Message
 * \brief Send barrier message. 
 * <b>Description </b>\n
 * API to send barrier request message.  After this call controller will not \n
 * send any Open Flow messages to  data_path until it received barrier reply\n
 * notification callback. 
 *
 * All the messages that transmitted between bariar notification and reply\n
 * are queued locally.
 *
 * The macro OFU_BARRIER_REQUEST_MESSAGE_LEN returns length of message buffer \n
 * required to send barrier message.\n
 *
 * \param [in] msg                  - Message descriptor with buffer allocated earlier to send barrier request. 
 * \param [in] datapath_handle      - Handle of the data_path to which we are sending the barrier request. 
 * \param [in] barrier_reply_cbk_fn - Callback function to inform the reception of barrier reply to barrier request.
 * \param [in] cbk_arg1             - Application specific private info,arg1, same will be passed to callback function.
 * \param [in] cbk_arg2             - Application specific private info,arg2, same will be passed to callback function.
 * \return OFU_INVALID_PARAMTERS_PASSED
 * \return OFU_SEND_BARRIER_MSG_SUCCESS
 */
int32_t
of_send_barrier_request_msg(uint64_t  datapath_handle,
                           of_barrier_reply_notification_fn barrier_reply_cbk_fn,
                           void *cbk_arg1,
                           void *cbk_arg2);

/****** Async message configuration APIs ************/

//!Set Asynchronous Configuration Message APIs
/*! This module contains APIs for get and set configuration of Asynchronous messages\n.
 *
 * It also contains utility APIs for parsing asyn configuration message  received from the data_path\n
 * as a reply to earlier get asynchronous message sent.
 *
 * \sa msg_mem_mgmt
 *\addtogroup Async_Config 
 **/

/** \ingroup Async_Config 
 *  OFU_SET_ASYNC_CONFIG_MESSAGE_LEN
 *  - Return length of message to send set asynchronous configuration message*/
#define OFU_SET_ASYNC_CONFIG_MESSAGE_LEN   sizeof(struct ofp_async_config)

/** \ingroup Async_Config 
 *  OFU_GET_ASYNC_CONFIG_MESSAGE_LEN
 *  - Return length messsage to get asynchronous configuration message*/
#define OFU_GET_ASYNC_CONFIG_MESSAGE_LEN   sizeof(struct ofp_header)

/** \ingroup Async_Config
 * \brief Set flag to indicate reason for receiving packet_in msg with role as 'master' or 'equal' 
 * <b>Description </b>\n
 * Setting controller specified interested reason flags to  generates packet in message.\n
 * Here Application can tell on what reasons controller wants to receive packet_in message \n
 * when controller role as 'master' or 'equal'. \n
 *
 * Setting FALSE to all the reason flags indicates controller don't wants to receive packet_in message\n
 * when role is 'master' or 'equal'.\n
 *
 * Application should call this utility function with necessary reason flags before actually calling\n
 * to send set asynchronous configuration message API of_set_async_configuration().
 *
 * \param [in] msg             -  Message descriptor with buffer allocated earlier for async message\n
 *                                configuration values.
 * \param [in] no_match_flow   -  Boolean flag, setting this value as TRUE indicates controller with \n
 *                                'master' or 'equal' role wants to receives packet_in messages due to\n
 *                                flow entry miss reason.  
 * \param [in] action          -  Boolean flag, setting this value TRUE indicates controller with \n
 *                                'master' or 'equal' role  wants to receives packet_in messages due\n
 *                                to action that indicated to send it to controller.  
 * \param [in] invalid_ttl     -  Boolean flag, setting this value TRUE indicates controller with\n
 *                                'master' or 'equal' role wants to receives packet_in messages due to\n
 *                                invalid_ttl reason.
 *
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_SET_PACKET_IN_GEN_REASON_FLAG_SUCCESS
 */
int32_t
ofu_set_packetin_async_msg_flags_in_master_or_equal_role(struct of_msg *msg,
                                                        uint8_t no_match_flow,
                                                        uint8_t action,
                                                        uint8_t invalid_ttl);

/** \ingroup Async_Config
 * \brief Set flag to indicate reason for receiving packet_in msg with role as 'slave'
 * <b>Description </b>\n
 * Setting controller specified interested reason flags to  generates packet in message.\n
 * Here Application can tell on what reasons controller wants to receive packet_in message \n
 * when controller role as 'slave'
 *
 * Setting FALSE to all the reason flags indicates controller don't wants to receive packet_in message\n
 * when role is 'slave'\n
 *
 * Application should call this utility function with necessary reason flags before actually calling\n
 * to send set asynchronous configuration message API of_set_async_configuration().
 *
 * \param [in] msg             -  Message descriptor with buffer allocated earlier for async message\n
 *                                configuration values.
 * \param [in] no_match_flow   -  Boolean flag, setting this value as TRUE indicates controller with \n
 *                                'slave' role wants to receives packet_in messages due to\n
 *                                flow entry miss reason.  
 * \param [in] action          -  Boolean flag, setting this value TRUE indicates controller with \n
 *                                'slave' role  wants to receives packet_in messages due\n
 *                                to action that indicated to send it to controller.  
 * \param [in] invalid_ttl     -  Boolean flag, setting this value TRUE indicates controller with\n
 *                                'slave' role wants receives packet_in messages due to invalid_ttl reason.
 *
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_SET_PACKET_IN_GEN_REASON_FLAG_SUCCESS
 */
int32_t
ofu_set_packetin_async_msg_flags_in_slave_role(struct of_msg *msg,
                                               uint8_t no_match_flow,
                                               uint8_t action,
                                               uint8_t invalid_ttl);
/** \ingroup Async_Config
 * \brief Set flag to indicate reason for receiving port status with role as 'master' or 'equal'
 * <b>Description </b>\n
 * Set controller interested reason flags that  generates  port status change message.
 * Application can tell on what reasons controller wants to receive port status change message\n
 * when its role is 'master' or 'equal'. 
 * Setting FALSE to all the flags indicates controller don't wants to receive port status \n
 * change message when role is 'master' or 'equal'. 
 *
 * Port status message is way to indicate change in status of data_path ports. The reason\n
 * for generation port status change message is due to 'add' or 'modify' or 'deletion of port.   
 *
 * Application should call this utility function with necessary configuration flags before actually\n
 * calling to send set asynchronous configuration message API of_set_async_configuration().\n
 *
 * \param [in] msg          -  Message descriptor with buffer allocated earlier for async message\n
 *                             configuration values.
 * \param [in] add_port     -  Boolean flag, setting this value TRUE indicates controller with\n
 *                             'master' or 'equal' role wants to receives port  change message \n
 *                             due to addition of new port  
 * \param [in] modify_port  -  Boolean flag, setting this value TRUE indicates controller with\n
 *                             'master' or 'equal' role wants to receives port  change message\n
 *                             due to modification of port  
 * \param [in] delete_port  -  Boolean flag, setting this value TRUE indicates controller with\n
 *                             'master' or 'equal' role wants to receives port  change message \n
 *                             due to deletion of port  
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_SET_PORT_STATUS_GEN_REASON_FLAG_SUCCESS
 */
int32_t
ofu_set_port_mod_async_msg_flags_in_master_or_equal_role(struct of_msg *msg,
                                                         uint8_t add_port,
                                                         uint8_t modify_port,
                                                         uint8_t delete_port);

/** \ingroup Async_Config
 * \brief Set flag to indicate reason for receiving port status with role as 'slave'
 * <b>Description </b>\n
 * Set controller interested reason flags that  generates  port status change message.
 * Application can tell on what reasons controller wants to receive port status change message\n
 * when its role is 'slave'. 
 * Setting FALSE to all the flags indicates controller don't wants to receive port status \n
 * change message when role is 'slave'. 
 *
 * Port status message is way to indicate change in status of data_path ports. The reason\n
 * for generation port status change message is due to 'add' or 'modify' or 'deletion of port.   
 *
 * Application should call this utility function with necessary configuration flags before actually\n
 * calling send set asynchronous configuration message API of_set_async_configuration().\n
 *
 * \param [in] msg          -  Message descriptor with buffer allocated earlier for async message\n
 *                             configuration values.
 * \param [in] add_port     -  Boolean flag, setting this value TRUE indicates controller with\n
 *                             'slave' role wants to receives port  change message \n
 *                             due to addition of new port  
 * \param [in] modify_port  -  Boolean flag, setting this value TRUE indicates controller with\n
 *                             'slave' role wants to receives port  change message\n
 *                             due to modification of port  
 * \param [in] delete_port  -  Boolean flag, setting this value TRUE indicates controller with\n
 *                             'slave' role wants to receives port  change message \n
 *                             due to deletion of port  
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_SET_PORT_STATUS_GEN_REASON_FLAG_SUCCESS
 */
int32_t
ofu_set_port_mod_async_msg_flags_in_slave_role(struct of_msg *msg,
                                               uint8_t add_port,
                                               uint8_t modify_port,
                                               uint8_t delete_port);

/** \ingroup Async_Config
 * \brief Set flag to indicate reason for receiving flow removed event with role as 'master' or 'equal'
 * <b>Description </b>\n
 *  Set controller interested reason flags that  generates flow remove event. 
 *  Application can tell on what reasons controller wants to receive flow removed event when the role\n
 *  as 'master' or 'equal'. 
 *
 *  Setting FALSE to all the flags indicates controller don't wants to receive flow removed event when \n
 *  role is 'master' or 'equal'. 
 *
 *  Application should call this utility function with necessary configuration flags before actually\n
 *  calling to send set asynchronous configuration message API of_set_async_configuration().\n
 *  \param [in] msg            -  Message descriptor with buffer allocated earlier for async message\n
 *                                configuration values.
 *  \param [in] idle_timeout   -  Boolean flag, TRUE indicates controller with 'master' or 'equal' role\n
 *                                receives flow removed event due to timeout of flow idle time.  \n
 *  \param [in] hard_timeout   -  Boolean flag, TRUE indicates controller with 'master' or 'equal'role\n
 *                                receives flow removed event due to exceed of flow hard time.  \n
 *  \param [in] delete_command -  Boolean flag, TRUE indicates controller with 'master' or 'equal'role\n
 *                                receives flow removed event due to command received to delete\n
 *                                flow entry.  \n
 *  \param [in] group_delete   - Boolean flag, TRUE indicates controller with 'master' or 'equal' role\n
 *                               receives flow removed event due to deletion of group that\n
 *                               attached in the group actions of flow. \n
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_SET_FLOW_REMOVE_GEN_REASON_FLAG_SUCCESS
 */
int32_t
ofu_set_flow_remove_async_msg_flags_in_master_or_equal_role(struct of_msg *msg,
                                                            uint8_t idle_timeout,
                                                            uint8_t hard_timeout,
                                                            uint8_t delete_command,
                                                            uint8_t group_delete);
/** \ingroup Async_Config
 * \brief Set flag to indicate reason for receiving flow removed event with role as 'slave'
 * <b>Description </b>\n
 *  Set controller interested reason flags that  generates flow remove event. \n
 *  Application can tell on what reasons controller wants to receive flow removed event when the role\n
 *  as 'slave'. 
 *
 *  Setting FALSE to all the flags indicates controller don't wants to receive flow removed event when \n
 *  role is 'slave'. 
 *
 *  Application should call this utility function with necessary configuration flags before actually\n
 *  calling send set asynchronous configuration message API of_set_async_configuration().\n
 *  \param [in] msg            -  Message descriptor with buffer allocated earlier for async message\n
 *                                configuration values.
 *  \param [in] idle_timeout   -  Boolean flag, TRUE indicates controller with 'slave' role\n
 *                                receives flow removed event due to timeout of flow idle time.  \n
 *  \param [in] hard_timeout   -  Boolean flag, TRUE indicates controller with 'slave' role\n
 *                                receives flow removed event due to exceed of flow hard time.  \n
 *  \param [in] delete_command -  Boolean flag, TRUE indicates controller with 'slave' role\n
 *                                receives flow removed event due to command received to delete\n
 *                                flow entry.  \n
 *  \param [in] group_delete   - Boolean flag, TRUE indicates controller with 'slave' role\n
 *                               receives flow removed event due to deletion of group that\n
 *                               attached in the group actions of flow. \n
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_SET_FLOW_REMOVE_GEN_REASON_FLAG_SUCCESS
 */
int32_t
ofu_set_flow_remove_async_msg_flags_in_slave_role(struct of_msg *msg,
                                                  uint8_t idle_timeout,
                                                  uint8_t hard_timeout,
                                                  uint8_t delete_command,
                                                  uint8_t group_delete);
/** \ingroup Async_Config
 * \brief Set Asynchronous configuration message in the data_path
 * <b>Description </b>\n
 * API to modify asynchronous message reception configuration values.\n
 * All the necessary utility functions were expected call before actully calling this API.\n
 *
 * The Macro OFU_SET_ASYNC_CONFIG_MESSAGE_LEN returns length of message to set asynchronous\n
 * configuraion message.\n
 * 
 * \param [in] datapath_handle   - Handle of the data_path to send asynchronous configuration message. 
 * \param [in] msg               - Message descriptor with buffer allocated earlier for modification
 *                                 of async message configuration values.
 * \param [in] clbk_command_arg1 - Application private information, arg1.In case of any errors,same will be passed to the\n
 *                                 error callback function that registered by the applications to receive errors.
 * \param [in] clbk_command_arg2 - Application private information, arg2.In case of any errors,same will be passed to the\n
 *                                 error callback function that registered by the applications to receive errors.
 * \return OFU_INVALID_PARAM_PASSED
 * \return OFU_SEND_ASYNC_SET_MSG_SUCCESS
 */
int32_t
of_set_async_configuration( uint64_t  datapath_handle,
                            struct of_msg *msg,
                            void *clbk_command_arg1,
                            void *clbk_command_arg2);

/** \ingroup Async_Config
 * \typedef of_async_config_cbk_fn
 * \brief Callback function to receive async config info. 
 * <b>Description </b>\n
 * Callback function to pass asynchronous configuration flag values. This callback will be\
 * called in response to earlier get asynchronous configuration request.
 * \param [in] controller_handle  - Handle of the controller
 * \param [in] domain_handle      - Handle of the domain
 * \param [in] datapath_handle    - Handle of the data_path.
 * \param [in] msg                - Message descriptor with buffer contains Open Flow asynchronous\n
 *                                  configuration values. Controller allocates the memory required\n
 *                                  to read  response. Once application done with response , the \n
 *                                  free function available in the message descriptor will be called to\n
 *                                  free the  memory. 
 *                                  This message descriptor need to pass into different utility function\n
 *                                  to know corresponding synchronous configuration values.\n
 * \param [in] cbk_arg1           - First Application specific private info that passed in the API to\n
 *                                  to get Asynchronous configuration values.\n
 * \param [in] cbk_arg2           - Second Application specific private info that passed in the API to\n
 *                                  get  Asynchronous configuration values.\n
 * \param [in] status             - Flag indicated the status  of response to earlier  Asynchronous \n
 *                                  configuration request. The status can be success or failure or timed out.
 */
typedef void (*of_async_config_cbk_fn)(uint64_t  controller_handle,
                                       uint64_t  domain_handle,
                                       uint64_t  datapath_handle,
                                       struct of_msg *msg,
                                       void     *cbk_arg1,
                                       void     *cbk_arg2,
                                       uint8_t   status);

/** \ingroup Async_Config
 * \brief Get current Asynchronous configuration values that set at data_path.
 * <b>Description </b>\n
 * API to get current asynchronous configuration values. Callback function is passed to\n
 * get current configuration values. The asynchronous configuration values that \n
 * available as part of message descriptor need to pass different utility functions to\n
 * know whether specific configuration values set or not. \n
 *
 * The Macro OFU_GET_ASYNC_CONFIG_MESSAGE_LEN returns length of the message required to get\n
 * asynchronous configuration message.
 * \param [in]  msg                - Message descriptor with buffer allocated earlier to send get current \n
 *                                   async configuration request message. Application need to allocate\n
 *                                   message descriptor with Length to get asynchronous configuration message\n
 *                                   defined above.\n
 * \param [in] datapath_handle     - Handle of the data_path to which we are sending request. \n
 * \param [in] async_config_info_cbk_fn - Callback function to  pass asynchronous configuration values.
 * \param [in] clbk_arg1           - Application private information, arg1.In case of any errors,same will be passed to the\n
 *                                   error callback function that registered by the applications to receive errors.\n
 * \param [in] clbk_arg2           - Application private information, arg2.In case of any errors,same will be passed to the\n
 *                                   error callback function that registered by the applications to receive errors.\n
 */
int32_t
of_get_async_config_info(struct of_msg *msg,
                        uint64_t   datapath_handle,
                        of_async_config_cbk_fn async_config_info_cbk_fn,
                        void *clbk_arg1,
                        void *clbk_arg2);

/** \ingroup Async_Config
 * \brief Is controller with 'master'/'equal' receives pkt_in with reason miss_entry
 * \param [in]  msg - Message received from data_path contains asynchronous configuration values.\n
 * \return TRUE  - Indicates controller with role 'master'/'equal' can receives packet_in message\n
 *                 due to miss_entry reason.
 * \return FALSE - Indicates controller with 'master'/'equal' will not receives packet_in message\n
 *                 due to reason miss_entry.
 */
uint8_t
ofu_is_cntlr_with_role_master_or_equal_recv_pkt_in_with_reason_miss_entry(struct of_msg *msg);

/** \ingroup Async_Config
 * \brief Is controller with 'master'/'equal' receives pkt_in with reason output action
 * \param [in]  msg - Message received from data_path contains asynchronous configuration values.\n
 * \return TRUE  - Indicates controller with role 'master'/'equal' can receives packet_in message\n
 *                 due to output action reason.
 * \return FALSE - Indicates controller with 'master'/'equal' will not receives packet_in message\n
 *                 due to reason output action.
 */
uint8_t
ofu_is_cntlr_with_role_master_or_equal_recv_pkt_in_with_reason_output_action(struct of_msg *msg);

/** \ingroup Async_Config
 * \brief Is controller with 'master'/'equal' receives pkt_in with reason invalid_ttl
 * \param [in]  msg - Message received from data_path contains asynchronous configuration values.\n
 * \return TRUE  - Indicates controller with role 'master'/'equal' can receives packet_in message\n
 *                 due to invalid_ttl reason.
 * \return FALSE - Indicates controller with 'master'/'equal' will not receives packet_in message\n
 *                 due to reason invalid_ttl.
 */
uint8_t
ofu_is_cntlr_with_role_master_or_equal_recv_pkt_in_with_reason_invalid_ttl(struct of_msg *msg);

/** \ingroup Async_Config
 * \brief Is controller with 'master'/'slave' receives pkt_in with reason miss_entry
 * \param [in]  msg - Message received from data_path contains asynchronous configuration values.\n
 * \return TRUE  - Indicates controller with role 'master'/'equal' can receives packet_in message\n
 *                 due to reason of missing fl.
 * \return FALSE - Indicates controller with 'slave' will not receives packet_in message\n
 *                 due to reason miss_entry.
 */
uint8_t
ofu_is_cntlr_with_role_slave_recv_pkt_in_with_reason_miss_entry(struct of_msg *msg);

/** \ingroup Async_Config
 * \brief Is controller with 'slave' receives pkt_in with reason output action
 * \param [in]  msg - Message received from data_path contains asynchronous configuration values.\n
 * \return TRUE  - Indicates controller with role 'slave' can receives packet_in message\n
 *                 due to output action reason.
 * \return FALSE - Indicates controller with role 'slave' will not receives packet_in message\n
 *                 due to reason output action
 */
uint8_t
ofu_is_cntlr_with_role_slave_recv_pkt_in_with_reason_output_action(struct of_msg *msg);

/** \ingroup Async_Config
 * \brief Is controller with 'slave' receives pkt_in with reason invalid ttl
 * \param [in]  msg - Message received from data_path contains asynchronous configuration values.\n
 * \return TRUE  - Indicates controller with role 'slave' can receives packet_in message\n
 *                 due to invalid ttl reason.
 * \return FALSE - Indicates controller with role 'slave' will not receives packet_in message\n
 *                 due to reason invalid ttl.
 */
uint8_t
ofu_is_cntlr_with_role_slave_recv_pkt_in_with_reason_invalid_ttl(struct of_msg *msg);

/** \ingroup Async_Config
 * \brief Is controller with 'master'/'equal' receives port_status event with reason add_port
 * \param [in]  msg - Message received from data_path contains asynchronous configuration values.\n
 * \return TRUE  - Indicates controller with role 'master'/'equal' can receives port status  message\n
 *                 due to add_port reason.
 * \return FALSE - Indicates controller with 'master'/'equal' will not receives port status  message\n
 *                 due to reason add_port
 */
uint8_t
ofu_is_cntlr_with_role_master_or_equal_recv_port_status_change_with_reason_add_port(struct of_msg *msg);

/** \ingroup Async_Config
 * \brief Is controller with 'master'/'equal' receives port_status event with reason modify port
 * \param [in]  msg - Message received from data_path contains asynchronous configuration values.\n
 * \return TRUE  - Indicates controller with role 'master'/'equal' can receives port status  message\n
 *                 due to modify port reason.
 * \return FALSE - Indicates controller with 'master'/'equal' will not receives port status  message\n
 *                 due to reason modify port
 */
uint8_t
ofu_is_cntlr_with_role_master_or_equal_recv_port_status_change_with_reason_modify_port(struct of_msg *msg);

/** \ingroup Async_Config
 * \brief Is controller with 'master'/'equal' receives port_status event with reason delete port
 * \param [in]  msg - Message received from data_path contains asynchronous configuration values.\n
 * \return TRUE  - Indicates controller with role 'master'/'equal' can receives port status  message\n
 *                 due to delete port reason.
 * \return FALSE - Indicates controller with 'master'/'equal' will not receive port status  message\n
 *                 due to reason delete port.
 */
uint8_t
ofu_is_cntlr_with_role_master_or_equal_recv_port_status_change_with_reason_delete_port(struct of_msg *msg);

/** \ingroup Async_Config
 * \brief Is controller with 'slave' receives port_status event with reason add_port
 * \param [in]  msg - Message received from data_path contains asynchronous configuration values.\n
 * \return TRUE  - Indicates controller with role 'slave' can receives port status  message\n
 *                 due to add_port reason.
 * \return FALSE - Indicates controller with 'slave' will not receives port status  message\n
 *                 due to reason add_port.
 */
uint8_t
ofu_is_cntlr_with_role_slave_recv_port_status_change_with_reason_add_port(struct of_msg *msg);

/** \ingroup Async_Config
 * \brief Is controller with 'slave' receives port_status event with reason modify port
 * \param [in]  msg - Message received from data_path contains asynchronous configuration values.\n
 * \return TRUE  - Indicates controller with role 'slave' can receives port status  message\n
 *                 due to modify port reason.
 * \return FALSE - Indicates controller with role 'slave' will not receives port status  message\n
 *                 due to reason modify port.
 */
uint8_t
ofu_is_cntlr_with_role_slave_recv_port_status_change_with_reason_modify_port(struct of_msg *msg);

/** \ingroup Async_Config
 * \brief Is controller with 'slave' receives port_status event with reason delete port
 * \param [in]  msg - Message received from data_path contains asynchronous configuration values.\n
 * \return TRUE  - Indicates controller with role 'slave' can receives port status  message\n
 *                 due to delete port reason.
 * \return FALSE - Indicates controller with role 'slave' will not receives port status  message\n
 *                 due to reason delete port.
 */
uint8_t
ofu_is_cntlr_with_role_slave_recv_port_status_change_with_reason_delete_port(struct of_msg *msg);


/** \ingroup Async_Config
 * \brief Is controller with 'master'/'equal' receives flow removed event with reason idle_timeout
 * \param [in]  msg - Message received from data_path contains asynchronous configuration values.\n
 * \return TRUE  - Indicates controller with role 'master'/'equal' can receives packet_in message\n
 *                 due to idle_timeout reason.
 * \return FALSE - Indicates controller with 'master'/'equal' will not receives flow removed event message\n
 *                 due to reason idle_timeout.
 */
uint8_t
ofu_is_cntlr_with_role_master_or_equal_recv_flow_remove_event_with_reason_idle_time(struct of_msg *msg);

/** \ingroup Async_Config
 * \brief Is controller with 'master'/'equal' receives flow removed event with reason hard_timeout
 * \param [in]  msg - message received from data_path contains asynchronous configuration values.\n
 * \return true  - indicates controller with role 'master'/'equal' can receives packet_in message\n
 *                 due to hard_timeout reason.
 * \return false - indicates controller with 'master'/'equal' will not receives flow removed event message\n
 *                 due to reason hard_timeout
 */
uint8_t
ofu_is_cntlr_with_role_master_or_equal_recv_flow_remove_event_with_reason_hard_time(struct of_msg *msg);

/** \ingroup async_config
 * \brief is controller with 'master'/'equal' receives flow removed event with reason delete flow entry command.
 * \param [in]  msg - Message received from data_path contains asynchronous configuration values.\n
 * \return TRUE  - Indicates controller with role 'master'/'equal' can receives packet_in message\n
 *                 due to delete_cmd reason.
 * \return FALSE - Indicates controller with 'master'/'equal' will not receives flow removed event message\n
 *                 due to reason delete_cmd
 */
uint8_t
ofu_is_cntlr_with_role_master_or_equal_recv_flow_remove_event_with_reason_delete_cmd(struct of_msg *msg);

/** \ingroup Async_Config
 * \brief Is controller with 'master'/'equal' receives flow removed event with reason deletion of group attached to flow entry.
 * \param [in]  msg - Message received from data_path contains asynchronous configuration values.\n
 * \return TRUE  - Indicates controller with role 'master'/'equal' can receives packet_in message\n
 *                 due to delete_grp reason.
 * \return FALSE - Indicates controller with 'master'/'equal' will not receives flow removed event message\n
 *                 due to reason delete_grp
 */
uint8_t
ofu_is_cntlr_with_role_master_or_equal_recv_flow_remove_event_with_reason_grp_delete(struct of_msg *msg);

/** \ingroup Async_Config
 * \brief Is controller with 'slave' receives flow removed event with reason due to timeout of idle time. 
 * \param [in]  msg - Message received from data_path contains asynchronous configuration values.\n
 * \return TRUE  - Indicates controller with role 'slave' can receives packet_in message\n
 *                 due to idle_timeout reason.
 * \return FALSE - Indicates controller with 'slave' will not receives flow removed event message\n
 *                 due to reason idle_timeout
 */
uint8_t
ofu_is_cntlr_with_role_slave_recv_flow_remove_event_with_reason_idle_time(struct of_msg *msg);

/** \ingroup Async_Config
 * \brief Is controller with 'slave' receives flow removed event with reason due to timeout of hard time.
 * \param [in]  msg - message received from data_path contains asynchronous configuration values.\n
 * \return true  - indicates controller with role 'slave' can receives packet_in message\n
 *                 due to hard_timeout reason.
 * \return false - indicates controller with 'slave' will not receives flow removed event message\n
 *                 due to reason hard_timeout
 */
uint8_t
ofu_is_cntlr_with_role_slave_recv_flow_remove_event_with_reason_hard_time(struct of_msg *msg);

/** \ingroup async_config
 * \brief is controller with 'slave' receives flow removed event with reason deletion of flow entry.
 * \param [in]  msg - Message received from data_path contains asynchronous configuration values.\n
 * \return TRUE  - Indicates controller with role 'slave' can receives packet_in message\n
 *                 due to delete_cmd reason.
 * \return FALSE - Indicates controller with 'slave' will not receives flow removed event message\n
 *                 due to reason delete_cmd
 */
uint8_t
ofu_is_cntlr_with_role_slave_recv_flow_remove_event_with_reason_delete_cmd(struct of_msg *msg);

/** \ingroup Async_Config
 * \brief Is controller with 'slave' receives flow removed event with reason deletion of group that attached to flow entry.
 * \param [in]  msg - Message received from data_path contains asynchronous configuration values.\n
 * \return TRUE  - Indicates controller with role 'slave' can receives packet_in message\n
 *                 due to delete_grp reason.
 * \return FALSE - Indicates controller with 'slave' will not receives flow removed event message\n
 *                 due to reason delete_grp
 */
uint8_t
ofu_is_cntlr_with_role_slave_recv_flow_remove_event_with_reason_grp_delete(struct of_msg *msg);


/****** ASYNCHRONOUS MESSAGE  APIs ************/

//!APIs used for handling Asynchronous messages
/*! Provides API for Applications to register callback functions to receive asynchronous\n
 * messages. 
 *
 * Multiple applications can register  their functions with a priority to get indications\n
 * of error replies, packet_in,  port_status and flow_removed messages.    
 *
 * Controller uses application's registered callbacks, by using APIs \n
 * ofu_reg_alloc_msg_type_mem_mgmt_primitive() or ofu_reg_dflt_alloc_recv_msg_mem_mgmt_primitive(),\
 * to allocate messages to receive asynchronous events.
 *
 * \sa msg_mem_mgmt
 * \sa Match_Fields
 * \sa Instructions
 *
 *\addtogroup async_msg_api Asynchronous Message API 
 */

/** \ingroup async_msg_api
 * OF_ASYNC_MSG_PACKET_IN_EVENT
 * - Asynchronous message type 'packet_in' 
 */
#define OF_ASYNC_MSG_PACKET_IN_EVENT     1 

/** \ingroup async_msg_api
 * OF_ASYNC_MSG_FLOW_REMOVED_EVENT
 * - Asynchronous message type 'flow removed '
 */
#define OF_ASYNC_MSG_FLOW_REMOVED_EVENT  2 

/** \ingroup async_msg_api
 * OF_ASYNC_MSG_PORT_STATUS_EVENT
 * - Asynchronous message type 'port status' 
 */
#define OF_ASYNC_MSG_PORT_STATUS_EVENT   3 

/** \ingroup async_msg_api
 * OF_ASYNC_MSG_ERROR_EVENT
 * - Asynchronous message type 'Error'  
 */
#define OF_ASYNC_MSG_ERROR_EVENT         4 

/** \ingroup async_msg_api
 * OF_ASYNC_MSG_ERROR_EVENT
 * - Asynchronous message type 'Error'  
 */
#define OF_ASYNC_MSG_EXPERIMENTER_EVENT     5 

/** \ingroup async_msg_api
 * OF_ASYNC_MSG_APP_MIN_PRIORITY  0
 * - Minimum priorioity value tht application can set for given asynchronous message.
 *   Lowever the priority value, the priority of calling the application is high.
 *
 */
#define OF_ASYNC_MSG_APP_MIN_PRIORITY     1 

/** \ingroup async_msg_api
 * OF_ASYNC_MSG_APP_MAX_PRIORITY  0
 * - Maximum priorioity value tht application can set for given asynchronous message  
 *   Higher the priority value, the application calling priority is low.
 */
#define OF_ASYNC_MSG_APP_MAX_PRIORITY     256 

/** \ingroup async_msg_api
 * OF_ASYNC_MSG_CONSUMED  1
 * - This is one of the Application return value after its own processing of async message received
 *   by its registered callback function. 
 *
 *   This indicates the message has been consumed by the application 
 *   so that the message will not be handedover to the application registered at priority. 
 *   It is responsiblity of the Applications to free up the message by calling 'free_cbk' 
 *   function available in the message descriptor.
 */
#define OF_ASYNC_MSG_CONSUMED          1

/** \ingroup async_msg_api
 * OF_ASYNC_MSG_PASS_TO_NEXT_APP
 * - This is one of the Application return value after its own processing of async message received
 *   by its registered callback function. 
 *
 *   This indicates, the current application completed its processing so that the message can be
 *   passed to Application registered at low priority.
 *
 *   The Applications should not free the message.
 */
#define OF_ASYNC_MSG_PASS_TO_NEXT_APP   2

/** \ingroup async_msg_api
 * OF_ASYNC_MSG_DROP
 * - This is one of the Application return value after its own processing of async message received
 *   by its registered callback function. 
 *
 *   This indicates the current Application informing controller to drop the current message and 
 *   it should not handover the Application of next lower priority. If no Application is registered
 *   at next low level priority, the controller will drop the message.
 *
 *   The Applications should not free the message.
 */
#define OF_ASYNC_MSG_DROP              3

/** \ingroup async_msg_api
 * \brief API for applications to register asynchronous messages callbacks.
 * \param [in] domain_handle - Domain to which application registering callback.
 * \param [in] table_id      - Table Id to which application registering callback.
 * \param [in] event_type    - Asynchronous message event type to which application\n
 *                             registering callback.  
 * \param [in] priority      - Priority of application to receive event. It indicates the\n
 *                             specific order in which it wants to receive events with respect\n
 *                             to other applications of same event_type.
 * \param [in] call_back     - Application callback function to pass messages events\n
 *                             This will be typecast to corresponding function prototype\n
 *                             depending on the event_type.
 * \param [in] cbk_arg1      - First application specific private information. Same will be\n
 *                             passed to application registered callback function.\n
 * \param [in] cbk_arg2      - Second application specific private information. Same will be\n
 *                             passed to application registered callback function.\n
 * \return OF_ASYNC_MSG_CONSUMED 
 * \return OF_ASYNC_MSG_PASS_TO_NEXT_APP 
 * \return OF_ASYNC_MSG_DROP 
 */
int32_t 
of_register_asynchronous_message_hook(uint64_t  domain_handle,
                                      uint8_t   table_id,
                                      uint8_t   event_type,
                                      uint8_t   priority,
                                      void     *call_back,
                                      void     *cbk_arg1,
                                      void     *cbk_arg2);
/** \ingroup  async_msg_api
 * \struct ofl_error_msg
 * \brief Error Message details received from data_path\n
 * <b> Description </b>\n
 *  Data_structure to hold error message details , all the fields are converted into host_byte order\n
 *  before calling registered callback 'of_error_clbk_fn()'. 
 */
struct ofl_error_msg {
 
   /** Error type, this is one of OFPET_* */
   uint16_t type;
   
   /** One of specific error code that belongs to error type */
   uint16_t code_or_exp_type;

   uint32_t vendor_id; 
   /** Vendor ID */
   /** Variable-length error description, that can be interpreted based on the 'type' and 'code'\n
    *  Utility API  ofu_get_error_msg_description() is provided get error description*/
   uint8_t *data;  

   /** Length of error description*/ 
   uint32_t length;  
};
/** \ingroup async_msg_api
 * \typedef of_error_clbk_fn
 * \brief callback function to receive error message from data path.
 *  <b>Description </b>\n
 *  Callback function to receive error message . This callback will be called whenever error\n
 *  occured at data_path as part of processing earlier controller issued command.
 * 
 * \param [in] controller_handle  -Handle of the controller
 * \param [in] domain_handle      -Handle of the domain
 * \param [in] datapath_handle    -Handle of the data_path.
 * \param [in] msg                - Message descriptor with buffer contains error information.\n
 *                                  this message is passed just for debugging purpose.\n
 *                                  Controller allocates the memory required to read  response\n
 *                                  Once application done with response , the  free function\n
 *                                  available in the message descriptor will be called to free memory.
 * \param [in] error_msg          - Pointer to error message, all the fields are converted 
 *                                  from network to local host byte order.
 * \param [in] cbk_arg1           - First application specific private information that passed during
 *                                  registration of event type OF_ASYNC_MSG_ERROR_EVENT
 * \param [in] cbk_arg1           - Second application specific private information that passed during
 *                                  registration of event type OF_ASYNC_MSG_ERROR_EVENT
 * \param [in] cbk_command_arg1   - First application specific private information that passed earlier \n
 *                                  during command issue for which error is received. 
 * \param [in] cbk_command_arg1   - Second application specific private information that passed earlier \n
 *                                  during command issue for which error is received. 
 * \return OF_ASYNC_MSG_CONSUMED 
 * \return OF_ASYNC_MSG_PASS_TO_NEXT_APP 
 * \return OF_ASYNC_MSG_DROP 
 */
typedef int32_t (*of_error_clbk_fn)(uint64_t               controller_handle,
                                    uint64_t               domain_handle,
                                    uint64_t               datapath_handle,
                                    struct of_msg         *msg,
                                    struct ofl_error_msg  *error_msg,
                                    void                  *cbk_arg1,
                                    void                  *cbk_arg2,
                                    void                  *cbk_command_arg1,
                                    void                  *cbk_command_arg2);

/** \ingroup async_msg_api
 * \brief Utility function to receive NULL terminated error message description 
 * \param [in] msg       - Message received from data_path contains error details.\n
 *                         Same was passed to of_error_clbk_fn().
 * \param [in] error_msg - Pointer to error message, same was passed to of_error_clbk_fn().
 * \return  Description of error message received or NULL in case of no error message description exist.
 */
char*
ofu_get_error_msg_description(struct of_msg         *msg,
                              struct ofl_error_msg  *error_msg);
/** \ingroup  async_msg_api
 * \struct ofl_packet_in
 * \brief Packt in Message details received from data_path\n
 * <b> Description </b>\n
 *  Data_structure to hold packet in message details , all the fields are converted into host_byte order\n
 *  before calling registered callback 'of_packet_in_clbk_fn()'. 
 */
struct ofl_packet_in {
    uint32_t buffer_id;
    /** Buffer ID */
    uint8_t  channel_no;
    /** Channel number */
    uint8_t reason;
    /** Reason */
    uint8_t table_id;
   /** ID if the table */
    uint64_t cookie;
   /** cookie */
    uint8_t *match_fields;
    /** match fields */
    uint16_t match_fields_length;
    /** length of match fields */
    uint8_t *packet;
    /** packet info */
    uint16_t packet_length;
    /** length of the packet */
};


/** \ingroup async_msg_api
 * \typedef of_packet_in_clbk_fn
 * \brief callback function to receive packet in message from data_path.
 *  <b>Description </b>\n
 *  This callback will be called due to reception of packet  from data_path.\n
 * 
 * \param [in] controller_handle  -Handle of the controller
 * \param [in] domain_handle      -Handle of the domain
 * \param [in] datapath_handle    -Handle of the data_path.
 * \param [in] msg                - Message descriptor with buffer contains packet in message.\n
 *                                  this message is passed just for debugging purpose.\n
 *                                  Controller allocates the memory required to read  response\n
 *                                  Once application done with response , the  free function\n
 *                                  available in the message descriptor will be called to free memory.
 *                                  Utility functions are provided parse match fields.
 * \param [in] pkt_in             - Pointer to Packet In  message.
 * \param [in] cbk_arg1           - First application specific private information that passed during
 *                                  registration of callback to receive packet_in message.
 * \param [in] cbk_arg1           - Second application specific private information that passed during
 *                                  registration of callback to receive packet_in message.
 * \return OF_ASYNC_EVENT_CONSUMED
 * \return OF_ASYNC_EVENT_PASS_TO_NEXT_APP
 * \return OF_ASYNC_MSG_DROP
 */
typedef int32_t (*of_packet_in_clbk_fn)( uint64_t              controller_handle,
                                         uint64_t              domain_handle,
                                         uint64_t              datapath_handle,
                                         struct of_msg        *msg,
                                         struct ofl_packet_in *pkt_in,
                                         void                 *cbk_arg1,
                                         void                 *cbk_arg2);

/** \ingroup  async_msg_api
 * \struct ofl_flow_removed
 * \brief Flow remove message details received from data_path\n
 * <b> Description </b>\n
 *  Data_structure to hold flow removed  message details , all the fields are converted into host_byte order\n
 *  before calling registered callback 'of_flow_removed_clbk_fn()'. 
 */
struct ofl_flow_removed {
    uint64_t cookie;
    /** cookie */
    uint16_t priority;
    /** priority of flow */
    uint8_t reason;
   /** reason */
    uint8_t table_id;
   /** ID of the table */
    uint32_t duration_sec;
   /** duration in seconds */
    uint32_t duration_nsec;
   /** duration in nano-seconds */
    uint16_t idle_timeout;
   /** idle time out */
    uint16_t hard_timeout;
  /** hard time out */
    uint64_t packet_count;
  /** packet count */
    uint64_t byte_count;
  /** byte count */
    uint8_t  *match_fields;
  /** match fields */
    uint16_t match_field_len;
  /** length of match fields */
};

/** \ingroup async_msg_api
 * \typedef of_flow_removed_clbk_fn
 * \brief callback function to receive flow removed event.
 *  <b>Description </b>\n
 *  Callback function to receive due to reception of flow removed event occurred at data_path.
 * 
 * \param [in] controller_handle  -Handle of the controller
 * \param [in] domain_handle      -Handle of the domain
 * \param [in] datapath_handle    -Handle of the data_path.
 * \param [in] msg                - Message descriptor with buffer contains flow removed  event.\n
 *                                  this message is passed just for debugging purpose.\n
 *                                  Controller allocates the memory required to read  response\n
 *                                  Once application done with response , the  free function\n
 *                                  available in the message descriptor will be called to free memory.
 *                                  Utility functions are provided parse match fields.
 * \param [in] flow               - Pointer to flow details which actually removed.
 * \param [in] cbk_arg1           - First application specific private information that passed during
 *                                  registration of callback to receive flow removed event.
 * \param [in] cbk_arg1           - Second application specific private information that passed during
 *                                  registration of callback to receive flow removed event.
 * \return OF_ASYNC_MSG_CONSUMED
 * \return OF_ASYNC_MSG_PASS_TO_NEXT_APP
 * \return OF_ASYNC_MSG_DROP
 */
typedef int32_t  (*of_flow_removed_clbk_fn)( uint64_t                controller_handle,
                                             uint64_t                domain_handle,
                                             uint64_t                datapath_handle,
                                             struct of_msg           *msg,
                                             struct ofl_flow_removed *flow,
                                             void                    *cbk_arg1,
                                             void                    *cbk_arg2);

/************** Port(s) description get API *********************/

//!Get Port  Descripton  APIs 
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
 * \struct ofl_port_desc_info
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

/** \ingroup Port_Descripton_Get
 * \typedef of_port_desc_cbk_fn 
 * \brief Callback function to receive port description 
 * <b>Description </b>\n
 * This callback function will be passed to of_get_port_description() API \n
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
 * \param [in] status             - Flag indicated the status  of response to earlier multi-part request.
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

/** \ingroup Port_Descripton_Get 
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

/** \ingroup  async_msg_api
 * \struct ofl_port_status
 * \brief Port Status change event details received from data_path\n
 * <b> Description </b>\n
 *  Data_structure to hold description of port whose status changed. All the fields are converted into host_byte order\n
 *  before calling registered callback 'of_port_status_clbk_fn()'. 
 */
struct ofl_port_status {
    uint8_t                    reason;
    /** reason */
    struct ofl_port_desc_info  port_info;
   /** port info */
};

/** \ingroup async_msg_api
 * \typedef of_port_status_clbk_fn
 * \brief callback function to receive  port status change event.
 *  <b>Description </b>\n
 *  Callback function called  due to reception of change in the port status in the data_path.
 * \param [in] controller_handle  -Handle of the controller
 * \param [in] domain_handle      -Handle of the domain
 * \param [in] datapath_handle    -Handle of the data_path.
 * \param [in] msg                - Message descriptor with buffer contains port status change event.\n
 *                                  this message is passed just for debugging purpose.\n
 *                                  Controller allocates the memory required to read  response\n
 *                                  Once application done with response , the  free function\n
 *                                  available in the message descriptor will be called to free memory.
 *                                  Utility functions are provided parse port description. 
 * \param [in] port_status        - Pointer to port status change details.
 * \param [in] cbk_arg1           - First application specific private information that passed during
 *                                  registration of callback to receive port events. 
 * \param [in] cbk_arg1           - Second application specific private information that passed during
 *                                  registration of callback to receive port events. 
 * \return OF_ASYNC_EVENT_CONSUMED
 * \return OF_ASYNC_EVENT_PASS_TO_NEXT_APP
 * \return OF_ASYNC_EVENT_DROPPED
 */      
typedef int32_t (*of_port_status_clbk_fn)(uint64_t controller_handle,
                                          uint64_t domain_handle,
                                          uint64_t datapath_handle,
                                          struct of_msg *msg,
                                          struct ofl_port_status *port_status,
                                          void *cbk_arg1,
                                          void *cbk_arg2);

/****** SYMMETRIC  MESSAGE  APIs ************/

//!APIs used for handling Symmetric  messages
/*! Provides API for Applications to register callback functions to receive symmetric message\n
 *
 * Only one applicatin can register  their functions to received symmetric messages.
 *
 * \sa msg_mem_mgmt
 *
 *\addtogroup symmetric_msg_api Symmetric Message API 
 */

/** \ingroup symmetric_msg_api
 * OF_SYMMETRIC_MSG_HELLO
 * - Symmetric message type 'hello' 
 */
#define OF_SYMMETRIC_MSG_HELLO             1 

/** \ingroup symmetric_msg_api
 * OF_SYMMETRIC_MSG_ECHO_REQ
 * - Symmetric message type 'echo request' 
 */
#define OF_SYMMETRIC_MSG_ECHO_REQ          2 


/** \ingroup symmetric_msg_api
 * \brief API for applications to register symmetric messages callbacks.
 *  <b>Description </b>\n
 *  Registering callback functions to receive symmetric messages from\n
 *  data-paths.
 *
 *  From the available different symmetric message, except Echo-reply message
 *  application can register any one it interested. 
 *
 *  The Application can get the echo-reply by registering callback as part of 
 *  API that sends echo-request.
 *
 * \param [in] domain_handle - Domain to which application registering callback.
 * \param [in] event_type    - Symmetric message event type to which application\n
 *                             registering callback.  
 * \param [in] call_back     - Application callback function to pass messages events\n
 *                             This will be typecast to corresponding function prototype\n
 *                             depending on the event_type.
 * \param [in] cbk_arg1      - First application specific private information. Same will be\n
 *                             passed to application registered callback function.\n
 * \param [in] cbk_arg2      - Second application specific private information. Same will be\n
 *                             passed to application registered callback function.\n
 * \return OF_REG_SYMMETRIC_MSG_SUCCESS 
 * \return OF_REG_SYMMETRIC_MSG_ERROR 
 */
int32_t 
of_register_symmetric_message_hooks(uint64_t  domain_handle,
                                    uint8_t   event_type,
                                    void     *call_back,
                                    void     *cbk_arg1,
                                    void     *cbk_arg2);

/** \ingroup symmetric_msg_api
 * \typedef hello_clbk_fn
 * \brief callback function to receive  hello.
 *  <b>Description </b>\n
 *  Callback function called  due to reception of hello message from the data_path.
 * \param [in] controller_handle  -Handle of the controller
 * \param [in] domain_handle      -Handle of the domain
 * \param [in] datapath_handle    -Handle of the data_path.
 * \param [in] msg                - Message descriptor with buffer contains hello reception\n
 *                                  this message is passed just for debugging purpose.\n
 *                                  Controller allocates the memory required to read  response\n
 *                                  Once application done with response , the  free function\n
 *                                  available in the message descriptor will be called to free memory.
 * \param [in] cbk_arg1           - First application specific private information that passed during
 *                                  registration of event type OF_SYMMETRIC_MSG_HELLO
 * \param [in] cbk_arg1           - Second application specific private information that passed during
 *                                  registration of event type OF_SYMMETRIC_MSG_HELLO
 */
typedef void (*hello_clbk_fn)(
                                 uint64_t controller_handle,
                                 uint64_t domain_handle,
                                 uint64_t datapath_handle,
                                 struct of_msg  *msg,
                                 void *cbk_arg1,
                                 void *cbk_arg2);

/** \ingroup symmetric_msg_api
 * \brief Send Hello Message.
 * \param [in] datapath_handle   -Handle of the data_path to send hello message.
 */
int32_t ofp_prepare_and_send_hello_request( uint64_t datapath_handle);

/** \ingroup  symmetric_msg_api
 * \struct ofl_echo_req_recv_msg
 * \brief Data structure to hold Echo request received message from data_path\n
 * <b> Description </b>\n
 *  All the fields are converted into host_byte order  before calling registered\n
 *  callback 'echo_req_clbk_fn()'. 
 */
struct ofl_echo_req_recv_msg {
    /** Data to send  as part of echo request message. This is optional, the application can
     * can send echo request message without any data*/
    uint8_t *data;

    /** Length of the data sending with echo request message. In case of no data send 
     * send with echo request message, this value will be zero. */
    uint16_t length;
};


/** \ingroup symmetric_msg_api
 * \typedef echo_req_clbk_fn
 * \brief callback function to receive  echo request.
 *  <b>Description </b>\n
 *  Application registered callback  function to receive echo request message from the data_path.
 * \param [in] controller_handle  -Handle of the controller
 * \param [in] domain_handle      -Handle of the domain
 * \param [in] datapath_handle    -Handle of the data_path.
 * \param [in] msg                - Message descriptor with buffer contains echo request\n
 *                                  this message is passed just for debugging purpose.\n
 *                                  Controller allocates the memory required to read  response\n
 *                                  Once application done with response , the  free function\n
 *                                  available in the message descriptor will be called to free memory.
 * \param [in] echo_req           - Pointer to echo request details.
 * \param [in] cbk_arg1           - First application specific private information that passed 
 *                                  callback.
 * \param [in] cbk_arg1           - Second application specific private information that passed during
 *                                  callback.
 */
typedef void (*echo_req_clbk_fn)(
             uint64_t controller_handle,
             uint64_t domain_handle,
             uint64_t datapath_handle,
             struct of_msg  *msg,
             struct ofl_echo_req_recv_msg *echo_req,
             void *cbk_arg1,
             void *cbk_arg2);

/** \ingroup  symmetric_msg_api
 * \struct ofl_echo_reply_msg
 * \brief Hold Echo reply received message from data_path\n
 * <b> Description </b>\n
 *  Data_structure to hold echo reply message. All the fields are converted into host_byte order\n
 *  before calling registered callback 'echo_reply_clbk_fn()'. 
 */
struct ofl_echo_reply_msg {
    /** Data received along with echo request from data_path*/
    uint8_t *data;

    /** Length of data received along with echo request message from data_path*/
    uint16_t length;
};

/** \ingroup symmetric_msg_api
 * \typedef echo_reply_clbk_fn
 * \brief callback function to receive  echo reply.
 *  <b>Description </b>\n
 *  Callback function called  due to reception of echo reply message from the data_path.
 *  This is in response to earlier send echo request by using API 
 *  ofp_prepare_and_send_echo_request().  This callback will be passed as argument to
 *  ofp_prepare_and_send_echo_request() API. 
 * \param [in] controller_handle  -Handle of the controller
 * \param [in] domain_handle      -Handle of the domain
 * \param [in] datapath_handle    -Handle of the data_path.
 * \param [in] msg                - Message descriptor with buffer contains echo reply\n
 *                                  this message is passed just for debugging purpose.\n
 *                                  Controller allocates the memory required to read  response\n
 *                                  Once application done with response , the  free function\n
 *                                  available in the message descriptor will be called to free memory.
 * \param [in] echo_reply         - Pointer to echo reply details. All the variable will be converted
 *                                  into host byte order.
 * \param [in] cbk_arg1           - First application specific private information that passed 
 *                                  callback.
 * \param [in] cbk_arg1           - Second application specific private information that passed during
 *                                  callback.
 */
typedef void (*echo_reply_clbk_fn)(
    uint64_t controller_handle,
    uint64_t domain_handle,
    uint64_t datapath_handle,
    struct of_msg  *msg,
    struct ofl_echo_reply_msg *echo_reply,
    void *cbk_arg1,
    void *cbk_arg2);

/** \ingroup  symmetric_msg_api
 *  <b>Description </b>\n
 *  Data structure to send echo request message to datapath
 * \struct ofl_echo_req_send_msg
 * \brief Data structure contains data to send echo request message to data_path\n
 */
struct ofl_echo_req_send_msg {
    /** The data that we controller sending to data_path as part of echo request  message */
    uint8_t *data;

    /** The length of the data sending to data_path as part of echo request message*/
    uint32_t length;
};

/** \ingroup symmetric_msg_api
 * \brief Send echo request  request message to data_path
 * \param [in] datapath_handle    - Handle of the data_path to send echo request message.
 * \param [in] echo_msg           - Echo message data to send along with  echo message to data_path
 * \param ]in] echo_reply_clbk_fn - Callback function to receive echo response.
 * \param [in] cbk_arg1           - First application specific private information that passed during
 *                                  callback.
 * \param [in] cbk_arg1           - Second application specific private information that passed during
 *                                  callback.
 */
int32_t
ofp_prepare_and_send_echo_request (
    uint64_t datapath_handle,
    struct ofl_echo_req_send_msg *echo_msg,
    echo_reply_clbk_fn echo_reply_clbk_fn,
    void * cbk_arg1,
    void * cbk_arg2
    );

/** \ingroup  symmetric_msg_api
	
 * \brief Hold experimenter message received from data_path.\n
 * <b> Description </b>\n
 *  Data_structure to hold the experimenter message from data_path. All the fields are converted 
 *  into host_byte order before calling registered callback 'symmetric_experimenter_fn_cbk()'. 
 */
struct ofl_symmetric_experimenter_recv_msg {

    /** 32-bit value that uniquely identifies  the experimenter message received from data_path*/
    uint32_t experimenter;

    /** Experimenter message Type defined by application, this is application specific experimenter type*/
    uint32_t exp_type;

    /** The application specific experimenter data receiving from data_path*/
    uint8_t *data;

    /** Length of the experimenter data received from data_path*/
    uint32_t length;
};

/** \ingroup symmetric_msg_api
 * \typedef symmetric_experimenter_fn_cbk
 * \brief callback function to receive  experimenter message.
 *  <b>Description </b>\n
 *  Application registered callback  function to receive experimenter symmetric message from data_path.
 * \param [in] controller_handle  -Handle of the controller
 * \param [in] domain_handle      -Handle of the domain
 * \param [in] datapath_handle    -Handle of the data_path.
 * \param [in] msg                - Message descriptor with buffer contains experimenter message\n
 *                                  this message is passed just for debugging purpose.\n
 *                                  Controller allocates the memory required to read  response\n
 *                                  Once application done with response , the  free function\n
 *                                  available in the message descriptor will be called to free memory.
 * \param [in] exp_recv_msg       - Pointer to experimenter message receive details.
 * \param [in] cbk_arg1           - First application specific private information that passed to callback.
 * \param [in] cbk_arg1           - Second application specific private information that passed to callback.
 */
typedef void (*symmetric_experimenter_fn_cbk) (
    uint64_t controller_handle,
    uint64_t domain_handle,
    uint64_t datapath_handle,
    struct of_msg  *msg,
    struct ofl_symmetric_experimenter_recv_msg *exp_recv_msg,
    void *cbk_arg1,
    void *cbk_arg2);

/** \ingroup  symmetric_msg_api
 * \struct ofl_symmetric_experimenter_send_msg
 * <b> Description </b>\n
 * Data structure to send experimenter message value to datapath
 * \brief Data_structure used to send Experimenter message value to data_path\n
 */
struct ofl_symmetric_experimenter_send_msg {
    /** 32-bit value that uniquely identifies  the experimenter message sending to data_path*/
    uint32_t experimenter_id;

    /** Experimenter message Type defined by application*/
    uint32_t exp_type;

    /** The application specific experimenter data sending to data_path,*/
    uint8_t *data;

    /** Length of the experimenter data sending to data_path*/
    uint32_t length;
};

/** \ingroup symmetric_msg_api
 * \brief Send experimenter message to data_path
 * \param [in] data-path_handle    - Handle of the data_path to send experimenter message.
 * \param [in] exp_send_msg       - Experimenter message value sending to data_path
 * \param ]in] echo_reply_clbk_fn - Callback function to receive echo response.
 * \param [in] cbk_arg1           - First application specific private information that passed during
 *                                  callback.
 * \param [in] cbk_arg1           - Second application specific private information that passed during
 *                                  callback.
 */
int32_t 
ofp_prepare_and_send_symmetric_experimenter_msg(uint64_t datapath_handle,
                               struct ofl_symmetric_experimenter_send_msg *exp_send_msg,
                               void *cbk_arg1,
                               void *cbk_arg2);

struct of_app_msg
{
	uint8_t table_id;
	int64_t controller_handle;
	uint64_t domain_handle;
	uint64_t datapath_handle;
	uint64_t dpid;
	struct ofl_packet_in  *pkt_in;
        uint8_t  *pkt_data;
	void *cbk_arg1;
	void *cbk_arg2;
};


int32_t
of_cntrlr_send_request_message(struct of_msg *msg,
		uint64_t  datapath_handle,
		void *conn_info,
		void *clbk_command_arg1,
		void *clbk_command_arg2);

int32_t of_get_flow_rem_msg_match_fields( struct ofl_flow_removed  *flow_rem_msg,struct ofl_oxm_match_fields *oxm_match_fields_p, uint64_t *flags_p);

int32_t
of_frame_pktout_msg(struct of_msg *msg,
      uint64_t       datapath_handle,
      uint8_t        reply_pkt,
      uint32_t       buffer_id,
      uint32_t       in_port,
      uint8_t        channel_no,
      void **conn_info_pp
      );

#endif /*OF_MSGAPI_H*/
