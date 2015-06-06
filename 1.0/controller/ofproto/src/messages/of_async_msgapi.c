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

/*!\file of_async_msgapi.c
 * Author     : Rajesh Madabushi <rajesh.madabushi@freescale.com>
 * Date       : November, 2012  
 * Description: 
 * This file contains defintions different asynchrnous messaging APIs provided to 
 * North bound interface. 
 */

/* INCLUDE_START ************************************************************/
#include "controller.h"
#include "of_utilities.h"
#include "of_multipart.h"
#include "of_msgapi.h"
#include "cntrl_queue.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "cntlr_event.h"
#include "dprmldef.h"
#include "ttpldef.h"
#include "pktmbuf.h"
/* INCLUDE_END **************************************************************/

extern void *of_app_hook_mempool_g;


extern int32_t
of_alloc_and_read_async_message(cntlr_channel_info_t     *channel,
                                struct dprm_domain_entry  *p_dp_domain,
                                struct of_async_msg_mem_primitive  *msg_type_mem_primitive,
                                struct ofp_header        *msg_hdr_p,
                                struct of_msg           **msg_p);
struct of_app_hook_info*
of_get_async_msg_app_hook(struct ttp_table_entry *table_info,
                          uint8_t  event_type,
                          uint8_t  priority);

void
of_insert_app_hook_in_a_dscnding_priority(struct ttp_table_entry      *table,
                                          uint8_t                       event_type,
                                          struct of_app_hook_info *app_entry);
#if 0
extern int32_t
cntlr_get_domain_table_info(struct dprm_domain_entry *p_domain,
                            uint8_t                   table_id,
                            struct dprm_oftable_entry  **p_p_table);
#endif
void
of_async_app_info_free_rcu(struct rcu_head *node)
{
    struct of_app_hook_info *app_info = (struct of_app_hook_info*)((uint8_t*)node - sizeof(of_list_node_t));

    mempool_release_mem_block (of_app_hook_mempool_g,(unsigned char*)app_info,app_info->is_heap);
}

inline void 
of_async_msg_table_init(struct ttp_table_entry *table)
{
    uint8_t msgno;

    for(msgno = 1; msgno <= OF_MAX_NUMBER_ASYNC_MSGS; msgno++)
    { 
       memset(&table->event_info[msgno],0,sizeof(struct async_event_info));
       table->event_info[msgno].type = msgno;
       OF_LIST_INIT(table->event_info[msgno].app_list,of_async_app_info_free_rcu)
    }
}

/* APIs for applications to reigster callbacks to receive events */
int32_t
of_register_asynchronous_message_hook(uint64_t  domain_handle,
                                      uint8_t   table_id,
                                      uint8_t   event_type,
                                      uint8_t   priority,
                                      void     *call_back,
                                      void     *cbk_arg1,
                                      void     *cbk_arg2)
{
   struct dprm_domain_entry      *domain;
   struct of_app_hook_info       *app_entry = NULL;
   struct ttp_table_entry        *ttp_tbl_info_p=NULL;

   int32_t                        status = OF_SUCCESS;
   int32_t                        retval = OF_SUCCESS;

   if( (event_type < OF_FIRST_ASYNC_EVENT) ||
       (event_type > OF_LAST_ASYNC_EVENT)      )
   {
       OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Invalid event(%d) type passed \r\n",__FUNCTION__,__LINE__,event_type);
       return OF_REG_ASYNC_HOOK_INVALID_EVENT_TYPE;
   }

   if( (priority < OF_ASYNC_MSG_APP_MIN_PRIORITY)  ||
       (priority > OF_ASYNC_MSG_APP_MAX_PRIORITY)     )
   {
       OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Invalid Priority(%d) value passed \r\n",__FUNCTION__,__LINE__,priority);
       return OF_REG_ASYNC_HOOK_INVALID_APP_PRIORITY;
   }

   if(call_back == NULL)
   {
       OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d NULL value as callback function  \r\n",__FUNCTION__,__LINE__);
       return OF_REG_ASYNC_HOOK_NULL_CALLBACK;
   }

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_domain_byhandle(domain_handle, &domain);
      if(retval != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Error in getting domain details retval=%d \r\n",__FUNCTION__,__LINE__,retval);
         status = OF_REG_ASYNC_HOOK_INVALID_DOMAIN;
         break;
      }

      if(get_ttp_tbl_info(domain->ttp_name,table_id, &ttp_tbl_info_p)!=OF_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "Get Table type pattern info failed!.");
        status = OF_FAILURE;
        break; 
      }

      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "tableid:%d",ttp_tbl_info_p->table_id);
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "tablename:%s",ttp_tbl_info_p->table_name);

      app_entry = of_get_async_msg_app_hook(ttp_tbl_info_p,event_type,priority);
      if(app_entry != NULL)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Tyring to regisgter application with Duplication prioirty \r\n", __FUNCTION__,__LINE__);
         status = OF_REG_ASYNC_HOOK_DUPLICATE_APP_PRIOIRTY;
         app_entry = NULL;
         break;
      }
      app_entry = NULL;


      app_entry = (struct of_app_hook_info*)calloc(1,sizeof(struct of_app_hook_info));
      if(app_entry == NULL)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Tyring to regisgter application with Duplication prioirty \r\n", __FUNCTION__,__LINE__);
         status = CNTLR_MEM_ALLOC_ERROR;
         app_entry = NULL;
         break;
      } 
 
      app_entry->priority  = priority;
      app_entry->call_back = call_back;
      app_entry->cbk_arg1  = cbk_arg1;
      app_entry->cbk_arg2  = cbk_arg2;

      /* Inset application in the priority order to application list
       * Applications are maintained in descening order*/
      of_insert_app_hook_in_a_dscnding_priority(ttp_tbl_info_p,
                                                event_type,
                                                app_entry);
  }
  while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  if(status != OF_SUCCESS)
  {
     if(app_entry != NULL)
         free(app_entry);
  }

  return status;
}

/* Caller MUST use RCU_READ LOCK and RCU_READ  UNlock*/
void
of_insert_app_hook_in_a_dscnding_priority(struct ttp_table_entry      *table,
                                          uint8_t                       event_type,
                                          struct of_app_hook_info      *app_entry)
{
    struct of_app_hook_info *curr_app_entry = NULL;
    struct of_app_hook_info *prev_app_entry = NULL;
    uchar8_t offset;
    offset = OF_APP_HOOK_LISTNODE_OFFSET;
   
    OF_LIST_SCAN(table->event_info[event_type].app_list,curr_app_entry,struct of_app_hook_info*,offset)
    {
        if(app_entry->priority > curr_app_entry->priority)
        {
           OF_LIST_INSERT_NODE(table->event_info[event_type].app_list,
                               prev_app_entry,
                               app_entry,
                               curr_app_entry,offset);
           return;
        }
        prev_app_entry = curr_app_entry;
    }

    OF_APPEND_NODE_TO_LIST(table->event_info[event_type].app_list,app_entry,offset);
}

/* Caller MUST use RCU_READ LOCK and RCU_READ  unlock*/
struct of_app_hook_info*
of_get_async_msg_app_hook(struct ttp_table_entry *table_info,
                          uint8_t                  event_type,
                          uint8_t                  priority)
{
   struct of_app_hook_info *app_entry;
    uchar8_t offset;
    offset = OF_APP_HOOK_LISTNODE_OFFSET;

   OF_LIST_SCAN(table_info->event_info[event_type].app_list,app_entry,struct of_app_hook_info*,offset)
   {
     if(app_entry->priority == priority)
              return app_entry;
   }
   return NULL;
}

int32_t
handover_pkt_in_msg_to_app(uint64_t datapath_handle,
                          struct dprm_domain_entry *domain,
                          uint8_t                   table_id,
                          uint8_t                   msg_id,
                          struct of_msg            *msg,
                          struct ofl_packet_in     *pkt_in)
{
   struct of_app_hook_info  *app_entry;
   int32_t                  retval = OF_ASYNC_MSG_PASS_TO_NEXT_APP;
   uchar8_t offset;
   int8_t  domain_table_id;
   struct ttp_table_entry *ttp_table_info_p=NULL;
   offset = OF_APP_HOOK_LISTNODE_OFFSET;

   if (msg_id > OF_MAX_NUMBER_ASYNC_MSGS)
   {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Invalid message with id=%d  \r\n",__FUNCTION__,__LINE__,msg_id);
      return OF_FAILURE;
   }

   domain_table_id = get_domaintableid_by_dptableid(datapath_handle, table_id);
   if(domain_table_id < 0)
   { 
     OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Not able to find the domain table id for the table id=%d",table_id);
     return OF_FAILURE;
   }
   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"domain_table_id=%d table_id=%d", domain_table_id, table_id);

   if(get_ttp_tbl_info(domain->ttp_name, domain_table_id, &ttp_table_info_p)!=CNTLR_TTP_SUCCESS)
   {
     OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Table not present in the TTP.");
     return OF_FAILURE;
   }

   OF_LIST_SCAN(ttp_table_info_p->event_info[msg_id].app_list,
                app_entry,struct of_app_hook_info*,offset)
   {
     if(app_entry != NULL)
     retval = ((of_packet_in_clbk_fn)(app_entry->call_back))(controller_handle,
                                                             domain->domain_handle,
                                                             datapath_handle,
                                                             msg,
                                                             pkt_in,
                                                             app_entry->cbk_arg1,
                                                             app_entry->cbk_arg2);
     if(retval == OF_ASYNC_MSG_CONSUMED)
       break;
     else if(retval == OF_ASYNC_MSG_PASS_TO_NEXT_APP)
       continue;
     else if(retval == OF_ASYNC_MSG_DROP)
     {
       of_get_pkt_mbuf_by_of_msg_and_free(msg);
       break;
     }
   }

   if(retval == OF_ASYNC_MSG_PASS_TO_NEXT_APP)
       of_get_pkt_mbuf_by_of_msg_and_free(msg);

   return OF_SUCCESS;
}

int32_t
handover_flow_removed_event_to_app(uint64_t datapath_handle,
                                   struct dprm_domain_entry *domain,
                                   uint8_t                   table_id,
                                   uint8_t                   msg_id,
                                   struct of_msg            *msg,
                                   struct ofl_flow_removed  *flow_removed_msg)
{
   struct of_app_hook_info  *app_entry;
   int32_t                  retval = OF_ASYNC_MSG_PASS_TO_NEXT_APP;
   uchar8_t offset;
   offset = OF_APP_HOOK_LISTNODE_OFFSET;
   struct ttp_table_entry *ttp_table_info_p=NULL;

   if (msg_id > OF_MAX_NUMBER_ASYNC_MSGS)
   {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Invalid message with id=%d  \r\n",__FUNCTION__,__LINE__,msg_id);
      return OF_FAILURE;
   }
   if(get_ttp_tbl_info(domain->ttp_name, table_id, &ttp_table_info_p)!=CNTLR_TTP_SUCCESS)
   {
     OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "Table not present in the TTP.");
     return OF_FAILURE;
   }

   OF_LIST_SCAN(ttp_table_info_p->event_info[msg_id].app_list,
                app_entry,struct of_app_hook_info*,offset)
   {
    if(app_entry != NULL)
     retval = ((of_flow_removed_clbk_fn)(app_entry->call_back))(controller_handle,
                                                             domain->domain_handle,
                                                             datapath_handle,
                                                             msg,
                                                             flow_removed_msg,
                                                             app_entry->cbk_arg1,
                                                             app_entry->cbk_arg2);
     if(retval == OF_ASYNC_MSG_CONSUMED)
       break;
     else if(retval == OF_ASYNC_MSG_PASS_TO_NEXT_APP)
       continue;
     else if(retval == OF_ASYNC_MSG_DROP)
     {
       //of_get_pkt_mbuf_by_of_msg_and_free(msg);
       msg->desc.free_cbk(msg);
       break;
     }
   }

   if(retval == OF_ASYNC_MSG_PASS_TO_NEXT_APP)
      // of_get_pkt_mbuf_by_of_msg_and_free(msg);
       msg->desc.free_cbk(msg);

   return OF_SUCCESS;
}


#if 0
void
of_handle_pkt_in_msg(cntlr_channel_info_t       *p_channel,
                     struct dprm_datapath_entry *p_datapath,
                     struct ofp_header          *p_msg_hdr)
{
    struct of_msg   *msg;
    struct ofp_packet_in  *pkt_in_msg_rcvd;
    struct ofl_packet_in   pkt_in_msg;
    struct ofp_match      *match_field; 
    uint8_t         msg_id = (OFPT_PACKET_IN - OF_FIRST_ASYNC_MSG_TYPE_ID +1);
    int32_t         retval = OF_SUCCESS;
    int32_t         status = OF_SUCCESS;

    do
    {
        retval = of_alloc_and_read_async_message(p_channel,p_datapath->domain,
                                                 &p_datapath->domain->msg_type_mem_primitive[msg_id],
                                                 p_msg_hdr,
                                                 &msg);
        if(retval == OF_FAILURE)
        {
           OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d Error in alloc and reading  message data \r\n",__FUNCTION__,__LINE__);
           status = OF_FAILURE;
           break;
        }

        pkt_in_msg_rcvd  = (struct ofp_packet_in*)(msg->desc.start_of_data);

        if(CNTLR_UNLIKELY(pkt_in_msg_rcvd->table_id == OFPTT_ALL))
        {
           OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d Not a usable Table ID = %d\r\n",__FUNCTION__,__LINE__,pkt_in_msg_rcvd->table_id);
           status = OF_FAILURE;
           break;
        }

        match_field = &pkt_in_msg_rcvd->match; 
        if(CNTLR_UNLIKELY(ntohs(match_field->type) != OFPMT_OXM))
        {
           OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d Unsupported match field type=%x received \r\n",__FUNCTION__,__LINE__,match_field->type);
           status = OF_FAILURE;
           break;
        }
        pkt_in_msg.match_length = ntohs(match_field->length);
        pkt_in_msg.match        = (void*)&match_field->oxm_fields;
 
        pkt_in_msg.buffer_id = ntohl(pkt_in_msg_rcvd->buffer_id);
        pkt_in_msg.reason    = pkt_in_msg_rcvd->reason;
        pkt_in_msg.table_id  = pkt_in_msg_rcvd->table_id;
        /*TBD ntohll pkt_in_msg.cookie    = pkt_in_msg_rcvd->cookie;*/
        pkt_in_msg.cookie    = 0; 
        pkt_in_msg.packet    = pkt_in_msg_rcvd->data;
        pkt_in_msg.packet_length = pkt_in_msg_rcvd->total_len;

        retval = handover_async_msg_to_app(p_datapath->datapath_handle,
                                           p_datapath->domain,
                                           pkt_in_msg_rcvd->table_id,
                                           msg_id,
                                           msg,
                                           &pkt_in_msg);
        if(retval == OF_FAILURE)
        {
           OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s %d Failed to handover the packet in message to app\r\n",__FUNCTION__,__LINE__);
           status = OF_FAILURE;
           break;
        }
                                  
    }
    while(0);
 
    if(status == OF_FAILURE)
    {
       if(msg != NULL)
          msg->desc.free_cbk(msg);
    }
}
#endif
