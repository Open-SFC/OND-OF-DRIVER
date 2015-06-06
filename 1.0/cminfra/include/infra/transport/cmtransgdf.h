/* 
 *
 * Copyright  2012, 2013  Freescale Semiconductor
 *
 *
 * Licensed under the Apache License, version 2.0 (the "License");
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
*/

/*
 *
 * File name: ucmtransgdf.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/26/2013
 * Description: This file contains API declarations provided by
 *              UCM transport channel layer.
 * 
 */

#ifndef TRANS_GDEF_H
#define TRANS_GDEF_H

#ifdef CM_TRANS_DEBUG
#define CM_TRANS_DEBUG_PRINT  printf("\n%35s(%4d): ",__FUNCTION__,__LINE__),\
                      printf
#else
#define CM_TRANS_DEBUG_PRINT(format,args...) 
#endif

#define CM_MAX_ADMIN_ROLE_LEN 127
#define CM_MAX_ADMIN_NAME_LEN 127

/* define this in transport channel local header file */
#define  CM_TRCHAN_ATTRIB_INACTIMEOUT_NAME "inactivity_timeout"
#define CM_TRANS_ATTRIB_INAC_TIMEOUT   0x1

/*  * * *  U C M   M A I N   C O M M A N D S * * * * * * * * * */
enum
{
  CM_COMMAND_CONFIG_SESSION_START = 1,
  CM_COMMAND_CONFIG_SESSION_UPDATE,
  CM_COMMAND_CONFIG_SESSION_END,
  CM_COMMAND_CONFIG_SESSION_GET,

  CM_COMMAND_DM_REQUEST,
  CM_COMMAND_APPL_REQUEST,
  CM_COMMAND_LDSV_REQUEST,
  CM_COMMAND_RESPONSE,

  CM_COMMAND_SET_TRANSPORT_ATTRIBS,
  CM_COMMAND_CONFIG_SESSION_TIMEOUT,
  
  CM_COMMAND_AUTHENTICATE_USER,

  CM_COMMAND_VERSION_HISTORY,
  CM_COMMAND_NOTIFICATION,
  CM_EVMGR_EVENT,
  
#ifdef CM_STATS_COLLECTOR_SUPPORT
  CM_COMMAND_STATS_REQUEST,
  CM_COMMAND_STATS_RESPONSE,
#endif 
  CM_COMMAND_REBOOT_DEVICE, 
  CM_MAX_CMDS
};

/**
\ingroup TRCHNLDS
\struct struct cm_tnsprt_channel

The struct cm_tnsprt_channel structure is used to hold the transport channel
information. This is used to hold th SocketFd, used to connect to teh UCM JE
server.\n

\verbatim
typedef struct UCMTransChannel_s
{
  cm_sock_handle sock_fd_i; 
}struct cm_tnsprt_channel;

Parameters:

sock_fd_i           This holds the Socket File descriptor. \n
\endverbatim

*/

struct cm_tnsprt_channel
{
  cm_sock_handle sock_fd_i;
};

/**
\ingroup TRCHNLDS
\struct struct cm_msg_generic_header
The struct cm_msg_generic_header structure is used to hold the Generic Header information.
All the message buffers that moves across the Transport Channel must contain this 
header at the beginning of the message.

This will specify the Command ID, Management Engine ID and Admin
roles and permissions information.

\verbatim
typedef struct UCMMsgGenericHeader_s
{
  uint32_t command_id;
  uint32_t flags;   
  uint32_t sequence_id; 
  uint32_t mgmt_engine_id; 
  unsigned char admin_name[CM_MAX_ADMIN_NAME_LEN + 1];
  unsigned char admin_role[CM_MAX_ADMIN_ROLE_LEN + 1];
 }struct cm_msg_generic_header;

Parameters::

ulLength            This holds the total length of the Message buffer.

command_ui           This specifies the Command Indetification.
                    Accepted values are: 
                      CM_COMMAND_CONFIG_SESSION_START
                      CM_COMMAND_CONFIG_SESSION_UPDATE
                      CM_COMMAND_CONFIG_SESSION_END
                      CM_COMMAND_RESPONSE
                      CM_COMMAND_GENERIC
                      CM_COMMAND_TRANSPORTCHANNEL_ATTRIBS

ulFlags             Reserved for future use.

ulSeqNum            Reserved for future use.

ulMgmtEngineId       Specifies the Management Engine ID/ Module ID. This is
                    used to identify from which module the request has come.

Userrole_info        This specifies the User role information. This holds the 
                    Admin Name and admin role information.
\endverbatim
*/

struct cm_msg_generic_header
{
  uint32_t command_id;           /*(start_config/stop_config/update_cmd/command_response) */
  uint32_t flags;             /* reserved: future purpose */
  uint32_t sequence_id;            /* reserved: future purpose */
  uint32_t mgmt_engine_id;      /* sender identification. For eg: mgmt engine id */
  unsigned char admin_role[CM_MAX_ADMIN_ROLE_LEN + 1];      /* Admin role */
  unsigned char admin_name[CM_MAX_ADMIN_NAME_LEN + 1];      /* Admin Name */
};

/* UCM Commands : Applicable to Command field of Generic header */

/**
\ingroup TRCHNLDS
\struct struct cm_tnsprt_channel

The struct cm_tnsprt_channel_attribs structure is used to hold the transport channel
attributes information. This holds the Attribute ID and attribute data.

\verbatim
typedef struct UCMTransChannelAttribs_s
{
  uint16_t uiAttribId; 
  union
  {
    uint32_t inact_time_out; 
  }attrib_data;
}struct cm_tnsprt_channel_attribs;

Parameters:

uiAttribId           This holds the Attribute ID. \n

ulInacTimeout        This is the parameter to hold the attribute data. Here
                     the data is the Transport channel inactivity timeout.
\endverbatim

*/

struct cm_tnsprt_channel_attribs
{
  uint32_t attrib_id_ui;
  union
  {
    uint32_t inact_time_out;     /* Inactivity timeout */
  } attrib_data;
};

#endif /* _TRCHGDEF_H_ */
