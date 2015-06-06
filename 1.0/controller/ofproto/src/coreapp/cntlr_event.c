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

/*!\file cntlr_event.c
 * Author     : Rajesh Madabushi <rajesh.madabushi@freescale.com>
 * Date       : November, 2012  
 * Description: 
 * This file contains defintions different application event handling  
 * routines. Applications register for events at the domain.
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
/* INCLUDE_END **************************************************************/

extern void *of_app_hook_mempool_g;

static inline struct of_app_hook_info*
of_get_event_app_hook(struct dprm_domain_entry *p_domain,
                      uint8_t                  event_type,
                      uint8_t                  priority);

static inline void
cntlr_insert_app_event_hook_in_a_dscnding_priority( struct dprm_domain_entry *p_domain,
                                                    uint8_t                   event_type,
                                                    struct of_app_hook_info  *app_entry);


/* APIs for applications to reigster callbacks to receive events */
int32_t
cntlr_register_asynchronous_event_hook(uint64_t               domain_handle,
                                       uint8_t                event_type,
                                       uint8_t                priority,
                                       async_event_cbk_fn    call_back,
                                       void                  *cbk_arg1,
                                       void                  *cbk_arg2)
{
   struct dprm_domain_entry      *domain;
   struct of_app_hook_info       *app_entry = NULL;
   int32_t                        status = OF_SUCCESS;
   int32_t                        retval = OF_SUCCESS;

   if( (event_type < CNTLR_FIRST_EVENT_TYPE) ||
       (event_type > CNTLR_LAST_EVENT_TYPE)      )
   {
       OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s:%d Invalid event(%d) type passed \r\n",__FUNCTION__,__LINE__,event_type);
       return CNTLR_REG_APP_INVALID_APP_EVENT_TYPE;
   }

   if( (priority < APP_EVENT_MIN_PRIORITY) || (priority > APP_EVENT_MAX_PRIORITY) )
   {
       OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s:%d Invalid Priority(%d) value passed \r\n",__FUNCTION__,__LINE__,priority);
       return CNTLR_REG_APP_EVENT_HOOK_INVALID_PRIORITY;
   }

   if(call_back == NULL)
   {
       OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s:%d NULL value as callback function  \r\n",__FUNCTION__,__LINE__);
       return CNTLR_REG_APP_EVENT_HOOK_NULL_CALLBACK;
   }

   CNTLR_RCU_READ_LOCK_TAKE();
   do
   {
      retval = get_domain_byhandle(domain_handle, &domain);
      if(retval != DPRM_SUCCESS)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"%s:%d Error in getting domain details retval=%d \r\n",__FUNCTION__,__LINE__,retval);
         status = OF_REG_APP_EVNT_HOOK_INVALID_DOMAIN;
         break;
      }

      app_entry = of_get_event_app_hook(domain,event_type,priority);
      if(app_entry != NULL)
      {
         OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Tyring to regisgter application with Duplication prioirty \r\n", __FUNCTION__,__LINE__);
         status = OF_REG_APP_EVNT_HOOK_DUPLICATE_APP_PRIOIRTY;
         app_entry = NULL;
         break;
      }
      app_entry = NULL;
#if 0

       /*Create memory pool for application registeration info 'struct of_app_hook_info' */
      retval = mempool_get_mem_block(of_app_hook_mempool_g,
                              (unsigned char **)&app_entry,
                              (unsigned char *)&is_heap);
      if(retval != MEMPOOL_SUCCESS)
      {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"%s:%d Async application  info memory alloc failure,ret %d\r\n", __FUNCTION__,__LINE__,retval);
          status = CNTLR_MEM_ALLOC_ERROR;
          break;
      }
      app_entry->is_heap = is_heap;
#endif

      app_entry = (struct of_app_hook_info*)calloc(1,sizeof(struct of_app_hook_info));
      if(app_entry == NULL)
      {
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_WARN,"%s:%d Async application  info memory alloc failure,ret %d\r\n", __FUNCTION__,__LINE__,retval);
          status = CNTLR_MEM_ALLOC_ERROR;
          break;
      } 
      app_entry->priority  = priority;
      app_entry->call_back = (void*)call_back;
      app_entry->cbk_arg1  = cbk_arg1;
      app_entry->cbk_arg2  = cbk_arg2;


      /* Inset application in the priority order to application list at domain to receive events
       * Applications are maintained in descening order*/
      cntlr_insert_app_event_hook_in_a_dscnding_priority(domain,
                                                         event_type,
                                                         app_entry);
  }
  while(0);
  CNTLR_RCU_READ_LOCK_RELEASE();

  if(status != OF_SUCCESS)
  {

     if(app_entry != NULL)
          free(app_entry);
#if 0
        mempool_release_mem_block (of_app_hook_mempool_g,
                                   (unsigned char*)app_entry,
                                    app_entry->is_heap);
#endif
  }

  return status;
}

/* Caller MUST use RCU_READ LOCK and RCU_READ  UNlock*/
static inline void
cntlr_insert_app_event_hook_in_a_dscnding_priority( struct dprm_domain_entry *p_domain,
                                                    uint8_t                   event_type,
                                                    struct of_app_hook_info  *app_entry)
{
    struct of_app_hook_info *curr_app_entry = NULL;
    struct of_app_hook_info *prev_app_entry = NULL;
    uchar8_t offset;
    offset = OF_APP_HOOK_LISTNODE_OFFSET;

    OF_LIST_SCAN(p_domain->app_event_info[event_type].app_list,curr_app_entry,struct of_app_hook_info*,offset)
    {
        if(app_entry->priority > curr_app_entry->priority)
        {
           OF_LIST_INSERT_NODE(p_domain->app_event_info[event_type].app_list,
                               prev_app_entry,
                               app_entry,
                               curr_app_entry,offset);
           return;
        }
        prev_app_entry = curr_app_entry;
    }

    OF_APPEND_NODE_TO_LIST(p_domain->app_event_info[event_type].app_list,app_entry,offset);
}

/* Caller MUST use RCU_READ LOCK and RCU_READ  unlock*/
static inline struct of_app_hook_info*
of_get_event_app_hook(struct dprm_domain_entry *p_domain,
                      uint8_t                  event_type,
                      uint8_t                  priority)
{
   struct of_app_hook_info *app_entry;
   uchar8_t offset;
   offset = OF_APP_HOOK_LISTNODE_OFFSET;

   OF_LIST_SCAN(p_domain->app_event_info[event_type].app_list,app_entry,struct of_app_hook_info*,offset)
   {
     if(app_entry->priority == priority)
           return app_entry;
   }
   return NULL;
}

void
inform_event_to_app(struct dprm_datapath_entry *data_path,
                    uint8_t                     event_type,
                    void                       *event_info1,
                    void                       *event_info2)
{
   struct of_app_hook_info  *app_entry;
   struct dprm_domain_entry *domain;
   int32_t                   retval = PASS_EVENT_TO_NEXT_APP;
   uchar8_t offset;
   offset = OF_APP_HOOK_LISTNODE_OFFSET;

   if(CNTLR_UNLIKELY((data_path == NULL)))
     return;

   domain = data_path->domain;

   OF_LIST_SCAN(domain->app_event_info[event_type].app_list,app_entry,struct of_app_hook_info*,offset)
   {
     retval = ((async_event_cbk_fn)(app_entry->call_back))(0,
                                                           domain->domain_handle,
                                                           data_path->datapath_handle,
                                                           event_info1,
                                                           event_info2);
     if(retval == EVENT_CONSUMED_BY_APP)
       break;
   }
}
