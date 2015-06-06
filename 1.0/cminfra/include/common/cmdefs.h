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
 * File name: cmdefs.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: This file includes the common data structures  
 *              definitions used across UCM modules.
 * 
 */

#ifndef _UCMDEFS_H_
#define _UCMDEFS_H_
/*****************************************************************************
 * *  * * * * * * * * * M A C R O   D E F I N I T I O N S * * * * * * * * * *
 ****************************************************************************/
/* UCM TCP Ports */
#ifdef OF_CM_SWITCH_CBK_SUPPORT
#define UCMJE_CONFIGREQUEST_PORT 63000
#define CMLDSV_CONFIGREQUEST_PORT 63001
#endif

#ifdef OF_CM_CNTRL_SUPPORT
#define UCMJE_CONFIGREQUEST_PORT 63008
#define CMLDSV_CONFIGREQUEST_PORT 63009
#endif

#ifdef CM_STATS_COLLECTOR_SUPPORT
#define UCMSTATS_COLLECTOR_PORT 63400
#endif

/* UCM UDP Ports */
#define CM_UDP_LOOPBACK_PORT_NUM_START (63200)
#define CM_UDP_LOOPBACK_PORT_NUM_END (63300)

#define CM_LOG_CLTR_UDP_LOOPBACK_PORT_NUM_START  CM_UDP_LOOPBACK_PORT_NUM_START
#define CM_LOG_CLTR_UDP_LOOPBACK_PORT_NUM_END (CM_UDP_LOOPBACK_PORT_NUM_START + 20)

#define CM_MC_CONF_LOOPBACK_PORT  (CM_UDP_LOOPBACK_PORT_NUM_START + 25)

//#define CM_CHANNEL_DEBUG  printf
#define CM_CHANNEL_DEBUG(format,args...)

/* UCM Managment engines */
#define CM_MGMT_ENGINE_ID            0x0
#define CM_CLI_MGMT_ENGINE_ID        0x1
#define CM_HTTP_MGMT_ENGINE_ID       0x2
#define CM_LDSV_MGMT_ENGINE_ID       0x3
#define CM_TR069_MGMT_ENGINE_ID      0x4
#define CM_CONFIGSYNC_MGMT_ENGINE_ID 0x5
#define CM_REST_MGMT_ENGINE_ID       0x7

#ifdef CM_STATS_COLLECTOR_SUPPORT
#define CM_STATS_CLTR_ENGINE_ID      0x6
#endif
#define CM_GET_PASSIVE_REC_ID  5000

#define TRUE_DEL 2
#define TRUE_MOD_DEL 3
#define TRUE_ADD 4
#define NO_REGISTERED_CALLBACK_FUNCTION -2

#define CM_MAX_REQUEST_LEN (8 * 1024)
#define CM_MAX_NVPAIR_COUNT 100
#define CM_MAX_RECORD_COUNT 100
#define CM_MAX_INT_ENUM_COUNT 25
#define CM_MAX_STRENUM_COUNT  60

#ifdef CM_STATS_COLLECTOR_SUPPORT
#define CM_MAX_DEVICES 16
#endif
/* UCM Debug */
#define CM_DEBUG

#ifdef CM_DEBUG
//#define CM_COMMON_DEBUG            /* UCM common utilities Debug Macro */
#define CM_JE_DEBUG            /* UCM Daemon Debug Macro */
//#define CM_PAM_DEBUG
//#define CM_ROLEDB_DEBUG
//#define CM_DM_DEBUG            /* Data Model Debug Macro */
//#define CM_TRANS_DEBUG         /* Transport Channel */
//#define CM_JETRANS_DEBUG       /* JE Transport Channel(library) Debug */
//#define CM_DMTRANS_DEBUG       /* Data Model Transport Channel(library) Debug */
//#define CM_LDSV_DEBUG          /* Ldsv Debug Macro */
//#define CM_CLI_DEBUG           /* Cli Debug Macro */
//#define CM_HTTP_DEBUG          /* Http Debug Macro */
//#define CM_EVNOTIFMGR_DEBUG
#define CM_SECAPPL_DEBUG       /* Security Applications (Callback Library) */
#endif
#ifdef COMPILE_FOR_CM_ONLY
#define   likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)
#endif
#define TR_IMPORT extern
/* Loopback Address*/
#define CM_LOOPBACK_ADDR  0x7F000001

/* UCM Protocols*/
#define CM_IPPROTO_TCP   6
#define CM_IPPROTO_UDP   17
#define CM_IPPROTO_ICMP  1
#define CM_IPPROTO_IGMP  2
#define CM_IPPROTO_SCTP  132
#define CM_IPPROTO_IPV6  41
#define CM_IPPROTO_IPCOMP  108
#define CM_IPPROTO_ICMPV6 58
#define CM_IPV6_NOEXT_HDR 59
#define CM_IPV6_MOBILITY_HDR 135
#define CM_IPPROTO_OTHER 0
#define CM_IPPROTO_AH 51
#define CM_IPPROTO_ESP 50
#define CM_IP_PTCL  94 
#define CM_IPIP4_PTCL 4

#define CM_MAX_REQUEST_LEN (8 * 1024)
#define CM_MAX_NVPAIR_COUNT 100

#define CM_SNET_NAME_LEN 15

/* Local IP wildcard address */
#ifndef INADDR_ANY
#define  INADDR_ANY  0x0L
#endif

#define CM_MC_CONF_WRAPPER_SRVR_ADDR "127.0.0.1"

#define CM_SMTP_PORT 25
//#define CM_SNET_SELF_PORT 0
//#define CM_SNET_NETTYPE_ANY 0

/* UCM result codes */
#define CM_UINT64_SIZE sizeof(uint64_t)
#define CM_UINT32_SIZE sizeof(uint32_t)
#define CM_UINT8_SIZE  sizeof(uint8_t)

// Used for calculation of len during MBufGet32
#define CM_UINT_OS_SIZE 4
#define CM_ULINT_OS_SIZE 8

/* UCM role definitions */
#define CM_MAX_ADMINNAME_LEN 127
#define CM_MAX_ADMINROLE_LEN 127
#ifndef _CM_LOAD_DEFS_H_ //intentionally added in cmdefs.h and clstrevt.h for avoiding redefinitions
#define _CM_LOAD_DEFS_H_
enum cm_events
{
CM_LOAD_STARTED_EVENT=0x0001,
CM_LOAD_COMPLETED_EVENT=0x0002,
CM_LOAD_FAILED_EVENT=0x0004,
CM_SAVE_STARTED_EVENT=0x0008,
CM_SAVE_COMPLETED_EVENT=0x0010,
CM_SAVE_FAILED_EVENT=0x0020,
CM_CONFIG_VERSION_UPDATE_EVENT=0x0040,
CM_CONFIG_PARAM_UPDATE_EVENT=0x0080,
CM_READY_TO_TAKE_LOAD_EVENT=0x0100,
CM_MAX_EVENT=0x0200,
};
#endif
#define CM_VERSION_FILE_WITH_FULL_PATH(version_file_a) {\
	    of_memset(version_file_a,0,sizeof(version_file_a));\
	    sprintf(version_file_a,"%s/%s",PERSISTENT_TRANSFERLIST_DIR,CM_CONFIGSYNC_CONFIG_VER_FILE);\
}

#define CM_READ_VERSION(version, aversion) \
{\
	 uint32_t image_version_ui;\
	 uint32_t cfg_version_ui;\
	 of_memset(aversion,0,sizeof(aversion));\
	 image_version_ui = (version & 0xFFFFFFFF00000000)>>32 ;\
	 cfg_version_ui = (version & 0x00000000FFFFFFFF);\
	 sprintf(aversion,"%d.%d",image_version_ui, cfg_version_ui);\
}  

/*****************************************************************************
 * * * * * * * * * S T R U C T U R E  D E F I N I T I O N S * * * * * * * * * 
 ****************************************************************************/
struct cm_nv_pair
{
  uint32_t name_length;
  char *name_p;               /* Mandatory */
  uint32_t friendly_name_length;
  char *friendly_name_p;
  uint32_t value_type;         /* Mandatory */
  uint32_t value_length;          /* Optional  */
  void *value_p;               /* Mandatory */
  uint32_t enum_val_len_ui;
  void *enum_value_p;
  uint32_t identifier_len_ui;     /* Optional */
  char *cli_identifier_p;	/* Optional */
/* Atmaram - The Following fields are not required to present in each and every
 * NvPair. This information already present in NodeInfo structure. nv_pairs are
 * allocated in huge number, and passed to mgmt engines.
 * Need to be removed following fields for effective memory usage and improve the perfomance-
 * friendly_name_length, friendly_name_p ,enum_val_len_ui, enum_value_p, identifier_len_ui, cli_identifier_p;	
 **/
};


struct cm_array_of_nv_pair
{
  uint32_t count_ui;
  struct cm_nv_pair *nv_pairs;
};

struct cm_command
{
  uint32_t command_id;
  char *dm_path_p;
  struct cm_array_of_nv_pair nv_pair_array;
};

struct cm_array_of_commands
{
  uint32_t count_ui;
  struct cm_command *Commands;
};

struct cm_iv_pair
{
  uint32_t id_ui;                /* Mandatory */
  uint32_t value_type;         /* Mandatory */
  uint32_t value_length;          /* Optional  */
  void *value_p;               /* Mandatory */
};

struct cm_array_of_iv_pairs
{
  uint32_t count_ui;
  struct cm_iv_pair *iv_pairs;
};

struct cm_app_result
{
  uint32_t result_code;
  char *result_string_p;

  union
  {
    struct
    {
      uint32_t reserved;
    } success;

    struct
    {
      struct cm_iv_pair iv_pair;
    } error;
  } result;

};
#endif
