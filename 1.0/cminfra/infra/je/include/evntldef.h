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
 * File name: evntldef.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/05/2013
 * Description: 
*/

#ifndef CM_EVENT_LDF_H
#define CM_EVENT_LDF_H


 struct cm_evmgr_event_request;
{
   UCMDllQNode_t  list_node; /* double linked list node. 
                               contains prev, next pointers */
   /* Requester Information */
   uint32_t  module_id;  /* ID of the module being registered */

   /* Transportation Information */
   uint32_t    peer_ip_addr_ui;  /* Interested module.s IP address */
   uint16_t  peer_port_ui;   /* Inteested modules port */
   uint8_t   protocol_ui;      /* Normally it is TCP */

   /* Interested events */
   uint32_t  interested_events; /* Bit mask specifying the interested events */
   uint32_t  no_of_events;  /* Inform 1 event for every N events */

   void*   tnsprt_channel_p;  /* to communicate with peer */

};


#endif /*CM_EVENT_LDF_H*/
