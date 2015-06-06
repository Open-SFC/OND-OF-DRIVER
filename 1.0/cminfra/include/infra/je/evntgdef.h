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
 * File name: evntgdef.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/25/2013
 * Description: This file contains defines and structure
 *              definitions provided by UCM Event Notification Manager.
 * 
 */

#ifndef CM_NOTIFIMGR_GDEF_H
#define CM_NOTIFIMGR_GDEF_H


#define CM_EVNOTIFMGR_DEBUG
#ifdef CM_EVNOTIFMGR_DEBUG
#define CM_EVNOTIFMGR_DEBUG_PRINT printf("\n%35s(%4d)::", __FUNCTION__, __LINE__),printf
#else
#define CM_EVNOTIFMGR_DEBUG_PRINT(format,args...)
#endif

#define CM_EVMGR_EVENT_CONFIG_PARAM_CHANGE		0x1   /* for TR069 kind */
#define CM_EVMGR_EVENT_CONFIG_VERSION_CHANGE	0x2   /* for HA kind */
#define CM_EVMGR_MAX_DATETIME 32
 
#define ISVALIDEVENT(interested_events) \
  if(((interested_events & CM_EVMGR_EVENT_CONFIG_PARAM_CHANGE) \
  || (interested_events & CM_EVMGR_EVENT_CONFIG_VERSION_CHANGE)))

/* 
 * Structure to register for Notification 
 */

struct cm_evmgr_reg
{
  /* Requester  Information */
  uint32_t      module_id;	/* ID of the module being registered */

  /* Transportation Information */
  uint32_t   peer_ip_addr;	/* Interested modules IP address */
  uint16_t      peer_port_ui; 	/* Inteested modules port */
  uint8_t       protocol_ui;      /* Normally it is TCP */

  /* Interested events */
  uint32_t      interested_events;  /* Bit mask specifying the events */
  uint32_t      no_of_events;	/* Inform 1 event for every N events */
};

/* Structure for Date and Time */
struct date_time
{
  time_t date_time;
};

/*
 * Structure used for Notifying the configuration change.
 */

struct cm_evmgr_event
{
  uint32_t  mgmt_engine_id; /*Management Engine ID responsible for this change*/
  uint32_t  event_type;
  uint32_t  date_time;

  union
  {
    struct
    {  
      uint64_t	version;	/* UCM config version number */
    } version_change;
    struct
    {
      char  *dm_path_p; /* DM element that got effected */
      uint32_t   operation_ui;	/* Add/Set/Del, etc */
    }param_change;
  } event;
};

/* struct cm_ip_addr should be enhanced as below for SPUTM*/
struct igw_ip_addr
{
 uint32_t ip_type_ul;
 uint32_t vsg_id_ui;
  union
  {
     uint32_t     ip_addr_ul;
     struct cm_ipv6_addr ip_v6_addr;
  }ip;
#define ip_v6_addr_8 IP.ip_v6_addr.ip_v6_addr_u8
#define ip_v6_addr_16 IP.ip_v6_addr.ip_v6_addr_u16
#define ip_v6_addr_32 IP.ip_v6_addr.ip_v6_addr_u32
#define IPv4Addr IP.ip_addr_ul
};
#define  CM_EVENT             0x00200000 /*Always check this with core module event mechanism application IDs*/
/* struct cm_ip_addr should be enhanced as above*/
struct cm_config_sync_info
{
  uint64_t		dev_id_ui;
  uint64_t      runtime_cfg_ver_num;
  uint64_t      saved_cfg_ver_num;
  uint32_t device_cluster_role_ui; /* 0 = Invalid; 1 = Master; 2 = Participant*/
  uint32_t		opaque_info_ui;
  struct igw_ip_addr	mgmt_ip;
   char       if_name[16 + 1];
};

struct igw_cm_config_sync_event
{
  uint32_t        event_type;
  uint32_t        application_type_ui;
  uint32_t        total_size_ui;
  void          *opaque_p;
  struct cm_config_sync_info event_info;
};

#endif /* CM_NOTIFIMGR_GDEF_H */

