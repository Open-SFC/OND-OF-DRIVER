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
 * File name: ucmevapi.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/25/2013
 * Description: 
*/

#ifdef CM_EVENT_NOTIFMGR_SUPPORT

/********************************************************
 * * * * * *  I n c l u d e   S e c t i o n   * * * * * *
 ********************************************************/
#include "cmincld.h"
#include "jeincld.h"
#include "transgif.h"
#include "jeldef.h"
#include "evntgdef.h"
#include "evntldef.h"
#include "evntgif.h"
#include "errno.h"

/****************************************************************************
 * Prototype declaration section:
 *****************************************************************************/

 struct cm_evmgr_event_request*  ucmEvMgrFindRegEntryByDestination(
      uint32_t peer_ip_addr,
      uint16_t uiPort,
      uint8_t protocol_ui);

 int32_t evmgr_free_reg_entry(struct cm_evmgr_event_request* cm_event_reg_p);

 int32_t evmgr_frame_event_message(struct cm_evmgr_event* cm_ev_mgr_event_p,
      char* pBuf, 
      uint32_t* pBufLen);

 int32_t evmgr_delete_event_from_list(
      struct cm_evmgr_event_request*  pRegEntry);

/****************************************************************************
 * Macro definition section:
 *****************************************************************************/
#define CM_EVMGR_MAX_EVENT_SIZE 512

/* Registered Event request node head */
UCMDllQ_t ucmEvRequestRegHead_g;

void CSDRegisterEvents(void);
extern uint32_t uiOSIntSize_g;
/****************************************************************************
 * Function definition section:
 *****************************************************************************/


void cm_evmgr_init()
{
   CM_DLLQ_INIT_LIST(&ucmEvRequestRegHead_g);
   CSDRegisterEvents();
}
/******************************************************************************
 ** Function Name : cm_evmgr_register_interested_events 
 ** Description   : This API Registers Intrested Events 
 **                 with the Event Notification manager
 ** Input params  : cm_event_reg_p - pointer to struct cm_evmgr_reg
 ** Output params : NONE
 ** Return value  : OF_SUCCESS in Success
 **                 OF_FAILURE in Failure case
 *******************************************************************************/
int32_t cm_evmgr_register_interested_events (struct cm_evmgr_reg* cm_event_reg_p)
{
   struct cm_evmgr_event_request*  pRegEntry = NULL;

   /*Input validations */
   if(cm_event_reg_p == NULL)
   {
      CM_JE_DEBUG_PRINT (" Invalid arguments.");
      return OF_FAILURE;
   }

   /* 
    * Find any Duplicate node: Based on destination. Not by module ID 
    */
   pRegEntry =  ucmEvMgrFindRegEntryByDestination(cm_event_reg_p->peer_ip_addr,
         cm_event_reg_p->peer_port_ui,
         cm_event_reg_p->protocol_ui);
   if(pRegEntry != NULL)
   {
      /* 
       * We got an already existing registration
       * Add events from cm_event_reg_p->interested_events 
       * into pRegEntry->Interested events. (Note that they are bit masked)
       */
      return OF_SUCCESS;
   }

   /* Allocate a new Reg entry */
   pRegEntry = of_calloc(1, sizeof (struct cm_evmgr_event_request));
   if(pRegEntry)
   {
      /* Fill the input parameters from cm_event_reg_p into pRegEntry. */

      pRegEntry->module_id = cm_event_reg_p->module_id;
      pRegEntry->peer_ip_addr_ui = cm_event_reg_p->peer_ip_addr;
      pRegEntry->peer_port_ui = cm_event_reg_p->peer_port_ui;
      pRegEntry->protocol_ui = cm_event_reg_p->protocol_ui;

      ISVALIDEVENT(cm_event_reg_p->interested_events)
      {
         pRegEntry->interested_events |= cm_event_reg_p->interested_events;
      }
      else
      {
         CM_JE_DEBUG_PRINT (" Invalid argument Event.");
         evmgr_free_reg_entry(pRegEntry);
         return OF_FAILURE;
      }
      /* 
       * Create a transport channel, and store it in pRegEntry. 
       * Return OF_FAILURE if could not establish transport channel
       */
      if((pRegEntry->tnsprt_channel_p = cm_tnsprt_create_channel(
                  cm_event_reg_p->protocol_ui, 
                  cm_event_reg_p->peer_ip_addr, 0,
                  cm_event_reg_p->peer_port_ui)) == NULL)
      {
         CM_JE_DEBUG_PRINT(" Failed to establish transport channel.");
         evmgr_free_reg_entry(pRegEntry); /*CID 355*/
         return OF_FAILURE;
      }

      /* Add the pRegEntry to global linked list 
       * ucmEvRequestRegList using DLL macros
       */
      CM_DLLQ_INIT_NODE ((UCMDllQNode_t *) &pRegEntry->list_node);
      CM_DLLQ_APPEND_NODE (&ucmEvRequestRegHead_g,(UCMDllQNode_t *)  pRegEntry);
      CM_JE_DEBUG_PRINT ("Successfully registered with the Event List");

      return OF_SUCCESS;
   }
   return OF_FAILURE; 
}
/******************************************************************************
 ** Function Name : cm_evmgr_deregister_interested_events 
 ** Description   : This API Deregisters intrested Events 
 **                 from the Event Notification manager
 ** Input params  : cm_event_reg_p - pointer to struct cm_evmgr_reg
 ** Output params : NONE
 ** Return value  : OF_SUCCESS in Success
 **                 OF_FAILURE in Failure case
 *******************************************************************************/
int32_t cm_evmgr_deregister_interested_events (struct cm_evmgr_reg* cm_event_reg_p)
{
   struct cm_evmgr_event_request*  pRegEntry;

   /*Input validations */
   if(cm_event_reg_p == NULL)
   {
      CM_JE_DEBUG_PRINT (" Invalid arguments.");
      return OF_FAILURE;
   }

   /* Find registered entry */
   pRegEntry =  ucmEvMgrFindRegEntryByDestination(cm_event_reg_p->peer_ip_addr,
         cm_event_reg_p->peer_port_ui,
         cm_event_reg_p->protocol_ui);
   if(pRegEntry == NULL)
   {
      return OF_FAILURE;
   }

   /* We got an already existing registration */

   /* 
    * - If only some of the registered events need to be deregistered,
    *   then unmask those bitFlags from pRegEntry->interested_events
    *   and return OF_SUCCESS. 
    * - If all of the events need to be deregistered,
    *   close the transport channel, Remove the pRegEntry from 
    *   the global linked list ucmEvRequestRegList and return OF_SUCCESS
    */
   if(cm_event_reg_p->no_of_events > 0)
   {
      ISVALIDEVENT(cm_event_reg_p->interested_events)
      {
         pRegEntry->interested_events &= ~cm_event_reg_p->interested_events;
      }
   }
   if(cm_event_reg_p->no_of_events == 0 || cm_event_reg_p->interested_events == 0)
   {
      /*
       * Unregister all the events
       */
      if(evmgr_delete_event_from_list(pRegEntry) != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT("Failed to delete registered entry.");
         return OF_FAILURE;
      }
      evmgr_free_reg_entry(pRegEntry);
   }

   return OF_SUCCESS;
}

/******************************************************************************
 ** Function Name : evmgr_delete_event_from_list 
 ** Description   : This API Deletes Event Registration structure from Global 
 **                 registration List.
 ** Input params  : cm_event_reg_p- pointer to struct cm_evmgr_reg
 ** Output params : NONE
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
 int32_t evmgr_delete_event_from_list (struct cm_evmgr_event_request*  pRegEntry)
{
   UCMDllQNode_t *pDllQNode = NULL, *pTmpNode = NULL;
   struct cm_evmgr_event_request*  pRegDllNodeEntry = NULL;

   if (pRegEntry == NULL)
   {
      CM_JE_DEBUG_PRINT ("Transport Channel is NULL");
      return OF_FAILURE;
   }

   CM_DLLQ_DYN_SCAN (&ucmEvRequestRegHead_g, pDllQNode, pTmpNode,
         UCMDllQNode_t *)
   {
      pRegDllNodeEntry=
         CM_DLLQ_LIST_MEMBER (pDllQNode, struct cm_evmgr_event_request*, list_node);
      if (pRegDllNodeEntry == pRegEntry)
      {
         CM_DLLQ_DELETE_NODE (&ucmEvRequestRegHead_g, pDllQNode);
         return OF_SUCCESS;
      }
   }

   return OF_FAILURE;
}
/******************************************************************************
 ** Function Name : ucmEvMgrFindRegEntryByDestination 
 ** Description   : Function to find out existing Registration entry.
 **
 ** Input params  : peer_ip_addr - which is of type uint32_t
 **                 uiPort - Peer port number
 **                 protocol_ui - Protocol type(Mostly TCP)
 ** Output params : NONE
 ** Return value  :  This method searches event list for particular event 
 **                 and if event is found it will return pointer to that node
 **                 or else return null.
 *******************************************************************************/
 struct cm_evmgr_event_request* ucmEvMgrFindRegEntryByDestination(
      uint32_t peer_ip_addr,
      uint16_t uiPort,
      uint8_t protocol_ui)
{
   struct cm_evmgr_event_request *pEvMgrEvReq = NULL;

   CM_DLLQ_SCAN (&ucmEvRequestRegHead_g, pEvMgrEvReq, struct cm_evmgr_event_request *)
   {
      if (pEvMgrEvReq)
      {
         if ((pEvMgrEvReq->peer_ip_addr_ui == peer_ip_addr)
               && (pEvMgrEvReq->peer_ip_addr_ui == uiPort)
               && (pEvMgrEvReq->protocol_ui == protocol_ui))
         {
            return pEvMgrEvReq;
         }
      }
   }
   return NULL;

}

/******************************************************************************
 ** Function Name : evmgr_free_reg_entry 
 ** Description   : Function to free Registration entry.
 ** Input params  : cm_event_reg_p - pointer to struct cm_evmgr_reg
 **
 ** Output params : NONE
 ** Return value  : OF_SUCCESS in Success
 **                 OF_FAILURE in Failure case
 *******************************************************************************/
 int32_t evmgr_free_reg_entry(struct cm_evmgr_event_request* pRegEntry)
{

   if(pRegEntry == NULL)
   {
      CM_JE_DEBUG_PRINT (" Invalid arguments.");
      return OF_FAILURE;
   }
   pRegEntry->module_id = 0;
   pRegEntry->peer_ip_addr_ui = 0;
   pRegEntry->peer_port_ui = 0;
   pRegEntry->protocol_ui = 0;
   pRegEntry->interested_events = 0;

   if(pRegEntry->tnsprt_channel_p)
   {
      cm_tnsprt_close_channel((struct cm_tnsprt_channel *)pRegEntry->tnsprt_channel_p);
   }
   of_free(pRegEntry);
   pRegEntry = NULL;

   return OF_SUCCESS;
}

/******************************************************************************
 ** Function Name : cm_evmgr_dispatch_event 
 ** Description   : This method is used to dispatch the Event.
 ** Input params  : event_p - pointer to struct cm_evmgr_event
 **
 ** Output params : NONE
 ** Return value  : OF_SUCCESS in Success
 **                 OF_FAILURE in Failure case
 *******************************************************************************/
int32_t cm_evmgr_dispatch_event ( uint32_t mgmt_engine_id,
      void* tnsprt_channel_p, 
      char* pBuf,
      uint32_t buf_len_ui)
{
   struct cm_msg_generic_header *pUCMGenericHeader = NULL;
   struct cm_role_info role_info={"VortiQa", CM_VORTIQA_SUPERUSER_ROLE};

   /* Memory Allocation for Generic Header */
   pUCMGenericHeader = (struct cm_msg_generic_header *) of_calloc (1,
         sizeof
         (struct cm_msg_generic_header));
   if (pUCMGenericHeader == NULL)
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation for Generic Header failed");
      return OF_FAILURE;
   }
   /*Fill the Transportation Generic Header.*/

   cm_fill_generic_header(mgmt_engine_id, CM_EVMGR_EVENT, &role_info, 
         pUCMGenericHeader);
   /*Send Message through Transport Channel */
   if ((cm_tnsprt_send_message (tnsprt_channel_p, pUCMGenericHeader,
               pBuf, buf_len_ui)) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT("Sending through transport channel failed");
      of_free (pUCMGenericHeader);
      return OF_FAILURE;
   }
   of_free (pUCMGenericHeader);

   return OF_SUCCESS;
}

/******************************************************************************
 ** Function Name : evmgr_frame_event_message
 ** Description   : Frame the buffer to be sent through Transport Channel.
 ** Input params  : cm_ev_mgr_event_p - pointer to the C data structure that 
 **                                  contains the Event Info.
 **                 pBuf - Caller allocated buffer.
 ** Output params : pBufLen - pointer to the buffer length. Caller sets to 
 **                           the available buffer length. This function will
 **                           set it to the valid buffer length after copying
 **                           the content into the buffer.
 ** Return value  : OF_SUCCESS in Success
 **                 OF_FAILURE in Failure case
 *******************************************************************************/
 int32_t evmgr_frame_event_message(struct cm_evmgr_event* cm_ev_mgr_event_p,
      char* pBuf, 
      uint32_t* pBufLen)
{
   char *pTmp;
   uint32_t  uiDMPathLen = 0;
   uint32_t uiBuffLen = 0;

   if(!(cm_ev_mgr_event_p) || !(pBuf))
   {
      CM_JE_DEBUG_PRINT(" Invalid Input");
      return OF_FAILURE;
   }
   pTmp = pBuf;

   /* Copy  Mgmt Engine ID */
   pTmp = (char *)of_mbuf_put_32 (pTmp, cm_ev_mgr_event_p->mgmt_engine_id);
//   uiBuffLen += sizeof (cm_ev_mgr_event_p->mgmt_engine_id);
  uiBuffLen += uiOSIntSize_g;

   /* Copy Event type */
   pTmp = (char *)of_mbuf_put_32 (pTmp, cm_ev_mgr_event_p->event_type);
//  uiBuffLen += sizeof (cm_ev_mgr_event_p->event_type);
  uiBuffLen += uiOSIntSize_g;

   /* Copy time value */
   pTmp = (char *)of_mbuf_put_32 (pTmp, cm_ev_mgr_event_p->date_time);
	 //uiBuffLen += CM_UINT32_SIZE;
   uiBuffLen += uiOSIntSize_g;



   switch (cm_ev_mgr_event_p->event_type)
   {
      case CM_CONFIG_PARAM_UPDATE_EVENT:
         /* DM Path Len*/
         uiDMPathLen=0;
         if(cm_ev_mgr_event_p->event.param_change.dm_path_p)
            uiDMPathLen = of_strlen (cm_ev_mgr_event_p->event.param_change.dm_path_p);
         pTmp = (char *)of_mbuf_put_32 (pTmp, uiDMPathLen);
				 //     uiBuffLen += CM_UINT32_SIZE;
				 uiBuffLen += uiOSIntSize_g;

         /* Copy DMPath Length */
         if(uiDMPathLen > 0 )
         {
            of_memcpy(pTmp, cm_ev_mgr_event_p->event.param_change.dm_path_p, uiDMPathLen);
            uiBuffLen += uiDMPathLen;
            pTmp += uiDMPathLen;
         }

         /* Copy Operation */
         pTmp = (char *)of_mbuf_put_32 (pTmp, cm_ev_mgr_event_p->event.param_change.operation_ui);
//         uiBuffLen += sizeof (cm_ev_mgr_event_p->event.param_change.operation_ui);
         uiBuffLen += uiOSIntSize_g;
         break;
      case CM_LOAD_STARTED_EVENT:
      case CM_LOAD_COMPLETED_EVENT:
      case CM_LOAD_FAILED_EVENT:
      case CM_SAVE_STARTED_EVENT:
      case CM_SAVE_COMPLETED_EVENT:
      case CM_SAVE_FAILED_EVENT:
      case CM_CONFIG_VERSION_UPDATE_EVENT:
         /* Copy version */
         pTmp = (char *)of_mbuf_put_64 (pTmp, 
               cm_ev_mgr_event_p->event.version_change.version);
         uiBuffLen += 
            sizeof (cm_ev_mgr_event_p->event.version_change.version);
         break;
   }
   *pBufLen = uiBuffLen;
   return OF_SUCCESS;
}

/******************************************************************************
 ** Function Name : cm_evmgr_generate_event
 ** Description   : This method is used to generate the Event and through this 
 **                 method events are informed to Event Manager (from JE).
 ** Input params  : event_p - pointer to struct cm_evmgr_event
 **
 ** Output params : NONE
 ** Return value  : OF_SUCCESS in Success
 **                 OF_FAILURE in Failure case
 *******************************************************************************/
int32_t cm_evmgr_generate_event(struct cm_evmgr_event* event_p)
{
   uint32_t buf_len_ui = 0;
   char *pBuf = NULL;
   struct cm_evmgr_event_request *pEvMgrEvReq = NULL;
#ifdef CONFIGSYNC_SUPPORT
   struct igw_cm_config_sync_event UCMConfigsyncEvent;
   int32_t return_value=0;
#endif /* CONFIGSYNC_SUPPORT*/

   CM_DLLQ_SCAN (&ucmEvRequestRegHead_g, pEvMgrEvReq, struct cm_evmgr_event_request *)
   {
      if (pEvMgrEvReq)
      {
         if(pEvMgrEvReq->interested_events & event_p->event_type)
         {
            CM_JE_DEBUG_PRINT ("Interested Module %d", pEvMgrEvReq->module_id);
            /* Frame the buffer : only first time */
            if(!pBuf)
            {
               pBuf = (char *)of_calloc(1, CM_EVMGR_MAX_EVENT_SIZE);
               buf_len_ui = CM_EVMGR_MAX_EVENT_SIZE;
               evmgr_frame_event_message(event_p, pBuf, &buf_len_ui);
            }
            if(cm_evmgr_dispatch_event(event_p->mgmt_engine_id, 
                     pEvMgrEvReq->tnsprt_channel_p,
                     pBuf, buf_len_ui) != OF_SUCCESS)
            {
               CM_JE_DEBUG_PRINT ("Unable to dispatch event.");
               of_free(pBuf);
               pBuf = NULL;
               return OF_FAILURE;
            }
         }
      }
   }
   if(pBuf)
   {
      of_free(pBuf);
   }

#ifdef CONFIGSYNC_SUPPORT
  of_memset(&UCMConfigsyncEvent,0,sizeof(struct igw_cm_config_sync_event));
   UCMConfigsyncEvent.event_type = event_p->event_type;
   UCMConfigsyncEvent.application_type_ui = CM_EVENT;
   UCMConfigsyncEvent.total_size_ui = sizeof(struct igw_cm_config_sync_event);

   if(event_p->event_type == CM_CONFIG_VERSION_UPDATE_EVENT)
      UCMConfigsyncEvent.event_info.runtime_cfg_ver_num = event_p->event.version_change.version;
   else if(event_p->event_type == CM_SAVE_COMPLETED_EVENT)
      UCMConfigsyncEvent.event_info.saved_cfg_ver_num = event_p->event.version_change.version;
   else if(event_p->event_type == CM_LOAD_COMPLETED_EVENT)
   {
      UCMConfigsyncEvent.event_info.runtime_cfg_ver_num = event_p->event.version_change.version;
      UCMConfigsyncEvent.event_info.saved_cfg_ver_num = event_p->event.version_change.version;
   }
   return_value =IGWUCMSendKernelSpaceEvents(&UCMConfigsyncEvent);
   if(return_value != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Unable to dispatch event.");
   }
#endif /* CONFIGSYNC_SUPPORT*/
   return OF_SUCCESS;
}

void CSDRegisterEvents()
{
   struct cm_evmgr_reg UCMEvents;
   int32_t return_value;

#define CONFIGSYNC_LOOPBACK_PORT 64000
   UCMEvents.module_id=CM_CONFIGSYNC_MGMT_ENGINE_ID;     /* ID of the module being registered */
   UCMEvents.peer_ip_addr=0;
   UCMEvents.peer_port_ui=CONFIGSYNC_LOOPBACK_PORT;
   UCMEvents.protocol_ui=CM_IPPROTO_TCP;      /* Normally it is TCP */
   UCMEvents.interested_events= CM_LOAD_STARTED_EVENT|CM_LOAD_COMPLETED_EVENT|CM_LOAD_FAILED_EVENT|CM_SAVE_STARTED_EVENT|CM_SAVE_COMPLETED_EVENT|CM_SAVE_FAILED_EVENT|CM_CONFIG_VERSION_UPDATE_EVENT;
   UCMEvents.no_of_events=7; 
   return_value=cm_evmgr_register_interested_events(&UCMEvents);
   if(return_value != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT("Config Sync Events with UCM failed");
   }
   return;
}
#endif /* CM_EVENT_NOTIFMGR_SUPPORT */

