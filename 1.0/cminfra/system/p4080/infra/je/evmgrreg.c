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
 * File name: evmgrreg.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/26/2013
 * Description: This file contains registrations for Event Notifications.
*/

#ifdef CM_EVENT_NOTIFMGR_SUPPORT

#include "cmincld.h"
#include "cmdefs.h"
#include "cmsock.h"
#include "evntgdef.h"
#include "evntgif.h"

/******************************************************************************
 ** Function Name : UCMEvMgrRegisterAllEvents
 ** Description   : This method is used to register the events.
 ** Input params  : NONE
 ** Output params : NONE
 ** Return value  : OF_SUCCESS in Success
 **                 OF_FAILURE in Failure case
 *******************************************************************************/
int32_t UCMEvMgrRegisterAllEvents()
{
  CM_EVNOTIFMGR_DEBUG_PRINT("Entered");

  struct cm_evmgr_reg UCMEventReg = { };

  UCMEventReg.module_id = 1;
  UCMEventReg.peer_ip_addr = CM_LOOPBACK_ADDR;
  UCMEventReg.peer_port_ui = 3490;
  UCMEventReg.protocol_ui = CM_IPPROTO_TCP;
  UCMEventReg.interested_events |= CM_EVMGR_EVENT_CONFIG_VERSION_CHANGE;
  UCMEventReg.no_of_events = 1;

  if((UCMEvMgrResgisterInterestedEvents(&UCMEventReg)) != OF_SUCCESS)
  {
     CM_EVNOTIFMGR_DEBUG_PRINT("Failed to register events...\r\n");
     return OF_FAILURE;
  }
  CM_EVNOTIFMGR_DEBUG_PRINT(" Successfully registered events...\r\n");
  return OF_SUCCESS;
}


#endif /* CM_EVENT_NOTIFMGR_SUPPORT */
